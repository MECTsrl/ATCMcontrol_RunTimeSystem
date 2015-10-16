/*
 * Copyright 2011 Mect s.r.l
 *
 * This file is part of FarosPLC.
 *
 * FarosPLC is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * FarosPLC is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * FarosPLC. If not, see http://www.gnu.org/licenses/.
*/

/*
 * Filename: dataImpl.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"dataImpl.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "stdInc.h"

#if defined(RTS_CFG_IODAT)

#include "dataMain.h"
#include "iolDef.h"
#include "mectCfgUtil.h"
#if defined(RTS_CFG_MECT_RETAIN)
#include "mectRetentive.h"
#endif

#include "Resource1_cst.h"
#include "crosstable_gvl.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define THE_DELAY_ms		100
#define THE_UDP_SEND_ADDR   "127.0.0.1"

#define REG_DATA_NUMBER     7680 // 1+5472+(5500-5473)+5473/2+...
#define THE_DATA_SIZE       (REG_DATA_NUMBER * sizeof(uint32_t)) // 30720 = 0x00007800 30 kiB
#define THE_DATA_UDP_SIZE   THE_DATA_SIZE
#define	THE_DATA_RECV_PORT	34903
#define	THE_DATA_SEND_PORT	34902

#define REG_SYNC_NUMBER 	6144 // 1+5472+...
#define THE_SYNC_SIZE       (REG_SYNC_NUMBER * sizeof(uint16_t)) // 12288 = 0x00003000 12 kiB
#define THE_SYNC_UDP_SIZE   11462 // SYNCRO_SIZE_BYTE in qt_library/ATCMutility/common.h
#define	THE_SYNC_RECV_PORT	34905
#define	THE_SYNC_SEND_PORT	34904

#define HARDWARE_TYPE_OFFSET 11464 	// byte offset in crosstable.gvl: HardwareType AT %ID1.11464 : DWORD ;
static unsigned int hardware_type = 0x00000000;

//#define RTS_CFG_DEBUG_OUTPUT

/* ----  Global Variables:	 -------------------------------------------------- */

static IEC_BOOL g_bInitialized	= FALSE;
static IEC_BOOL g_bConfigured	= FALSE;
static IEC_BOOL g_bRunning	= FALSE;
static IEC_BOOL g_bExiting	= FALSE;

static pthread_mutex_t theMutex = PTHREAD_MUTEX_INITIALIZER;
enum threadStatus { NOT_STARTED, RUNNING, EXITING };

static pthread_t theEngineThread_id = -1;
static pthread_t theDataThread_id = -1;
static pthread_t theSyncThread_id = -1;
static pthread_t theModbusRTU0Thread_id = -1;
static pthread_t theModbusRTU1Thread_id = -1;
static pthread_t theModbusTCPThread_id = -1;
static pthread_t theModbusTCPRTUThread_id = -1;
static enum threadStatus theEngineThreadStatus = NOT_STARTED;
static enum threadStatus theDataThreadStatus = NOT_STARTED;
static enum threadStatus theSyncThreadStatus = NOT_STARTED;
static enum threadStatus theModbusRTU0ThreadStatus = NOT_STARTED;
static enum threadStatus theModbusRTU1ThreadStatus = NOT_STARTED;
static enum threadStatus theModbusTCPThreadStatus = NOT_STARTED;
static enum threadStatus theModbusTCPRTUThreadStatus = NOT_STARTED;

static uint32_t the_IdataRegisters[REG_DATA_NUMBER];
static uint32_t the_QdataRegisters[REG_DATA_NUMBER];

static uint16_t the_IsyncRegisters[REG_SYNC_NUMBER];
static uint16_t the_QsyncRegisters[REG_SYNC_NUMBER];

/* ----  Local Functions:	--------------------------------------------------- */

static void *engineThread(void *statusAdr);
static void *modbusThread(void *statusAdr);
static void *dataThread(void *statusAdr);
static void *syncroThread(void *statusAdr);

static int do_recv(int s, void *buffer, ssize_t len)
{
    int retval = 0;
    ssize_t sent = 0;

#if 0
    while (sent < len && retval >= 0) {
        retval = recv(s, buffer + sent, len - sent, 0);
        if (retval == 0) {
            retval = -1;
        } else if (retval > 0) {
            sent += retval;
        }
    }
#else
    retval = recv(s, buffer + sent, len - sent, 0);
    if (retval < len) {
        retval = -1;
    }
#endif
    return retval;
}

static int do_sendto(int s, void *buffer, ssize_t len, struct sockaddr_in *address)
{
    int retval = 0;
    ssize_t sent = 0;

#if 0
    while (sent < len && retval >= 0) {
        retval = sendto(s, buffer + sent, len - sent, 0,
                (struct sockaddr *) address, sizeof(struct sockaddr_in));
        if (retval == 0) {
            retval = -1;
        } else if (retval > 0) {
            sent += retval;
        }
    }
#else
    retval = sendto(s, buffer + sent, len - sent, 0,
                    (struct sockaddr *) address, sizeof(struct sockaddr_in));
    if (retval < len) {
        retval = -1;
    }
#endif
    return retval;
}

static void InitXtable(uint16_t TIPO)
{
    uint16_t k;
    int32_t i;

    if (TIPO == 0) {
        for (i = 0; i <= DimCrossTable-1; ++i) {
            // TRIGGER01  =  0;
            CrossTable[i].Enable = 0;
            CrossTable[i].Plc = FALSE;
            CrossTable[i].Tag[0] = 0;
            CrossTable[i].Types = 0;
            CrossTable[i].Decimal = 0;
            CrossTable[i].Protocol = 0;
            CrossTable[i].IPAddress[0] = 0;
            CrossTable[i].Port = 0;
            CrossTable[i].NodeId = 0;
            CrossTable[i].Address = 0;
            CrossTable[i].Block = 0;
            CrossTable[i].NReg = 0;
            CrossTable[i].Handle = 0;
            CrossTable[i].OldVal = 0;
            CrossTable[i].Error = 1;
            k = ARRAY_QUEUE(i + 1);
            ARRAY_QUEUE_OUTPUT(i + 1) = 0;
            ARRAY_STATES(i + 1) = 0;
        }
        CrossTableState = TRUE;
        CommEnabled = FALSE;
        RTUProtocol_ON = FALSE;
        TCPProtocol_ON = FALSE;
        TCPRTUProtocol_ON = FALSE;
    } else if (TIPO == 1) {
        for (i = 0; i <= 159; ++i) {
            ALCrossTable[i].ALType = FALSE;
            ALCrossTable[i].ALTag[0] = 0;
            ALCrossTable[i].ALSource[0] = 0;
            ALCrossTable[i].ALCompareVar[0] = 0;
            ALCrossTable[i].ALCompareVal = 0;
            ALCrossTable[i].ALOperator = 0;
            ALCrossTable[i].ALFilterTime = 0;
        }
        ALCrossTableState = TRUE;
    }
}

static int LoadXTable(char *CTFile, int32_t CTDimension, uint16_t CTType)
{
    // InitXtable: fb_HW119_Init;
    // ReadFields:fb_HW119_ReadVarFields;
    // ReadAlarmsFields:fb_HW119_ReadAlarmsFields;
    // ReadCommFields:fb_HW119_ReadCommFields;
    uint16_t FBState = 0;
    uint16_t FunctRes;
    int32_t index;
    int32_t STEPS = 50;
    int32_t LowEdge = 0;
    int32_t HiEdge = 0;

    for (;;) {
        switch (FBState) {
        case 0:
            InitXtable(CTType); //Inizializzazione delle variabili CT
            FBState = 10;
            ErrorsState = ErrorsState & 0xF0; // reset flag di errore
            LowEdge = 1;
            HiEdge = STEPS;
            if (HiEdge > CTDimension) {
                HiEdge = CTDimension;
            }
            break;
        case 10:
            // FunctRes = HW119_OpenCrossTable(CTFile);
             if (FunctRes == 0)  {	// Operazione andata a buon fine
                FBState = 20;
                HW119_ERR[2] = 2;
            } else {
                FBState = 1000;	// Errore -> termina
                ErrorsState = ErrorsState | 0x01;
                HW119_ERR[2] = 3;
             }
            break;
        case 20:
            break;
        case 30:
            break;
        case 40:
            break;
        case 100:
            break;
        case 1000:
            break;
        }
    }
}

static void AlarmMngr(void)
{
}

static void PLCsync(void)
{
}

static void ErrorMNG(void)
{
}

void *engineThread(void *statusAdr)
{
    int threadInitOK = FALSE;

    // InitComm:fb_HW119_InitComm;
    // AlarmMngr:fb_HW119_AlarmsMngr;
    // LoadXTable:fb_HW119_LoadCrossTab;
    // ErrorMNG:fb_HW119_ErrorMng;
    // PLCsync:fb_HW119_PLCsync;
    // Reconnect:fb_HW119_Check;
    // TPAC1006_LIOsync:fb_TPAC1006_LIOsync;
    // TPAC1007_LIOsync:fb_TPAC1007_LIOsync;
    // TICtimer:fb_TPAC_tic;
    uint16_t FunctRes = 0;
    uint32_t CommState = 0;
    uint32_t COUNTER = 0;
    // timer_sync:TON;

    // thread init
    enum threadStatus *threadStatusPtr = (enum threadStatus *)statusAdr;

    // run
    *threadStatusPtr = RUNNING;
    for (;;) {
        if (g_bRunning && threadInitOK) {
            // ready data: receive and immediately reply
            pthread_mutex_lock(&theMutex);
            {
                switch (CommState) {
                case 0:
                    // FunctRes = HW119_CloseCrossTable();
                    // LoadXTable(ENABLE:=FALSE);
                    // InitComm(ENABLE:=FALSE);
                    ErrorsState = 0;
                    CommState = 10;
                    COUNTER = 0;
                    RTU_RUN = FALSE;
                    TCP_RUN = FALSE;
                    TCPRTU_RUN = FALSE;
                    ERROR_FLAG = 0;
                    break;
                case 10: // Lettura crosstable allarmi
                    if (LoadXTable(ALcrossTableFile, DimAlarmsCT, 1)) {
                        CommState = 20;	// Fallita lettura crosstable allrmi -> prosegue con lettura cross table variabili
                        ALCrossTableState = FALSE;
                        // LoadXTable(ENABLE:=FALSE);
                        ERROR_FLAG = ERROR_FLAG | 0x10; //SEGNALO L'ERRORE SUL BIT 4
                    } else {
                        CommState = 20;				//lettura crosstable  Allarmi  OK  -> prosegue
                        // FunctRes:=HW119_CloseCrossTable();
                        // LoadXTable(ENABLE:=FALSE);
                    }
                    break;
                case 20: // Lettura crosstable variabili
                    if (LoadXTable(VARcrossTableFile, DimCrossTable, 0)) {
                        CommState = 1000;	// Fallita lettura crosstable  variabili: FINE
                        CrossTableState = FALSE;
                        // LoadXTable(ENABLE:=FALSE);
                        ERROR_FLAG = ERROR_FLAG | 0x20; //SEGNALO L'ERRORE SUL BIT 5
                    } else {
                        CommState = 30;				// lettura crosstable  variabili  OK ->  prosegue
                        // LoadXTable(ENABLE:=FALSE);
                    }
                    break;
                case 30: // Lettura crosstable parametri di comunicazione
                    if (LoadXTable(CommParFile, 5, 2)) {
                        CommState = 1000;	// Fallita lettura crosstable  parametri di comunicazione: FINE
                        // LoadXTable(ENABLE:=FALSE);
                        ERROR_FLAG = ERROR_FLAG | 0x08; //SEGNALO L'ERRORE SUL BIT 3
                    } else {
                        CommState = 40;				// lettura crosstable  parametri di comunicazione  OK ->  prosegue
                        // LoadXTable(ENABLE:=FALSE);
                    }
                    break;
                case 40:
                    if (osPthreadCreate(&theModbusRTU1Thread_id, &theModbusRTU1ThreadStatus, &modbusThread, NULL, "modbusRTU1", 0) == 0) {
                        do {
                            usleep(1000);
                        } while (theModbusRTU1ThreadStatus != RUNNING);
                        ERROR_FLAG = ERROR_FLAG | 0x80; // SEGNALO CHE IL TASK RTU è PARTITO
                        RTU_RUN = TRUE;
                    } else {
                #if defined(RTS_CFG_IO_TRACE)
                        osTrace("[%s] ERROR creating modbus RTU1 thread: %s.\n", __func__, strerror(errno));
                #endif
                        RTU_RUN = FALSE;
                    }
                    CommState = 50;
                    break;
                case 50:
                    if (osPthreadCreate(&theModbusTCPThread_id, &theModbusTCPThreadStatus, &modbusThread, NULL, "modbusTCP", 0) == 0) {
                        do {
                            usleep(1000);
                        } while (theModbusTCPThreadStatus != RUNNING);
                        ERROR_FLAG = ERROR_FLAG | 0x100; // SEGNALO CHE IL TASK TCP è PARTITO
                        TCP_RUN = TRUE;
                    } else {
                #if defined(RTS_CFG_IO_TRACE)
                        osTrace("[%s] ERROR creating modbus TCP thread: %s.\n", __func__, strerror(errno));
                #endif
                        TCP_RUN = FALSE;
                    }
                    CommState = 60;
                    break;
                case 60:
                    if (osPthreadCreate(&theModbusTCPRTUThread_id, &theModbusTCPRTUThreadStatus, &modbusThread, NULL, "modbusTCPRTU", 0) == 0) {
                        do {
                            usleep(1000);
                        } while (theModbusTCPRTUThreadStatus != RUNNING);
                        ERROR_FLAG = ERROR_FLAG | 0x200; // SEGNALO CHE IL TASK TCPRTU è PARTITO
                        TCPRTU_RUN = TRUE;
                    } else {
                #if defined(RTS_CFG_IO_TRACE)
                        osTrace("[%s] ERROR creating modbus TCPRU thread: %s.\n", __func__, strerror(errno));
                #endif
                        TCPRTU_RUN = FALSE;
                    }
                    CommState = 70;
                    break;
                case 70:
                    ERROR_FLAG = ERROR_FLAG | 0x40;	// SEGNALA SUL BIT 6 CHE HA TERMINATO L'INIZIALIZZAZIONE DELLE CT
                    // TRIGGER02:=TRIGGER02;
                    // TRIGGER01:=TRIGGER01;
                    if  (ALCrossTableState) {
                        CommEnabled = TRUE;
                    }
                    // FunctRes:=WORD_TO_UINT(TSK_Start('TASK0_Control'));		(* partenza task PLC*)
                    CommState = 80;
                    break;
                case 80:
                    // TRIGGER02 = TRIGGER02;
                    // TRIGGER01 = TRIGGER01;
                    if (CommEnabled)  {
                        AlarmMngr();					// Gestione allarmi*)
                    }
                    // Reconnect();						// Riconnessione per protocolli TCP e TCPRTU	*)
                    PLCsync();							// sincronizzazione tra variabili PLC e HMI	*)
//                    timer_sync(IN = TRUE,PT = T#50ms);
//                    if timer_sync.Q  {
//                        timer_sync(IN = FALSE);
//                        TICtimer(tic = T#50ms);
//                         if (HardwareType AND 16#000000FF) >= 16#01  {
//                            TPAC1006_LIOsync();
//                         }
//                         if (HardwareType AND 16#00FF0000) >= 16#01  {
//                            TPAC1007_LIOsync();
//                         }
//                    }
                    break;
                case 1000:
                    // TRIGGER02:=TRIGGER02;
                    ERROR_FLAG = ERROR_FLAG | 0x40; // SEGNALA SUL BIT 6 CHE HA TERMINATO L'INIZIALIZZAZIONE DELLE CT*)
                    ErrorMNG();
                    break;
                default:
                    ;
                }
            }
            pthread_mutex_unlock(&theMutex);
            usleep(1000);
        } else if (g_bExiting) {
            break;
        } else {
            usleep(THE_DELAY_ms * 1000);
        }
    }

    // thread clean

    // exit
    *threadStatusPtr = EXITING;
    return NULL;
}

void *modbusThread(void *statusAdr)
{
    int threadInitOK = FALSE;

    // thread init
    enum threadStatus *threadStatusPtr = (enum threadStatus *)statusAdr;
    int status = 0;

    // run
    *threadStatusPtr = RUNNING;
    for (;;) {
        if (g_bRunning && threadInitOK) {
            // ready data: receive and immediately reply
            switch (status) {
            case 0:
                break;
            case 1:
                pthread_mutex_lock(&theMutex);
                {
                }
                pthread_mutex_unlock(&theMutex);
                break;
            case 2:
                break;
            default:
                ;
            }
            usleep(1000);
        } else if (g_bExiting) {
            break;
        } else {
            usleep(THE_DELAY_ms * 1000);
        }
    }

    // thread clean

    // exit
    *threadStatusPtr = EXITING;
    return NULL;
}

void *dataThread(void *statusAdr)
{
    int udp_recv_socket = -1;
    int udp_send_socket = -1;
    struct  sockaddr_in DestinationAddress;
    struct  sockaddr_in ServerAddress;
    int threadInitOK = FALSE;
    enum threadStatus *threadStatusPtr = (enum threadStatus *)statusAdr;

    // thread init
    udp_recv_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_recv_socket != -1) {
        if (fcntl(udp_recv_socket, F_SETFL, O_NONBLOCK) >= 0) {
            memset((char *)&ServerAddress,0,sizeof(ServerAddress));
            ServerAddress.sin_family = AF_INET;
            ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
            ServerAddress.sin_port = htons((u_short)THE_DATA_RECV_PORT);
            if (bind(udp_recv_socket, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress)) >= 0) {
                udp_send_socket = socket(AF_INET, SOCK_DGRAM, 0);
                if (udp_send_socket >= 0) {
                    struct hostent *h = gethostbyname(THE_UDP_SEND_ADDR);
                    if (h != NULL) {
                        memset(&DestinationAddress, 0, sizeof(DestinationAddress));
                        DestinationAddress.sin_family = h->h_addrtype;
                        memcpy((char *) &DestinationAddress.sin_addr.s_addr,
                                h->h_addr_list[0], h->h_length);
                        DestinationAddress.sin_port = htons(THE_DATA_SEND_PORT);
                        threadInitOK = TRUE;
                    }
                }
            }
        }
    }

    // run
    *threadStatusPtr = RUNNING;
    for (;;) {
        if (g_bRunning && threadInitOK) {
            // wait on server socket, only until timeout
            fd_set recv_set;
            struct timeval tv;
            FD_ZERO(&recv_set);
            FD_SET(udp_recv_socket, &recv_set);
            tv.tv_sec = THE_DELAY_ms / 1000;
            tv.tv_usec = (THE_DELAY_ms % 1000) * 1000;
            if (select(udp_recv_socket + 1, &recv_set, NULL, NULL, &tv) <= 0) {
                // timeout or error
                continue;
            }
            // ready data: receive and immediately reply
            pthread_mutex_lock(&theMutex);
            {
                int rc = do_recv(udp_recv_socket, the_IdataRegisters, THE_DATA_UDP_SIZE);
                if (rc != THE_DATA_UDP_SIZE) {
                    // ?
                }
                int sn = do_sendto(udp_send_socket, the_QdataRegisters, THE_DATA_UDP_SIZE,
                                   &DestinationAddress);
                if (sn != THE_DATA_UDP_SIZE) {
                    // ?
                }
            }
            pthread_mutex_unlock(&theMutex);
        } else if (g_bExiting) {
            break;
        } else {
            usleep(THE_DELAY_ms * 1000);
        }
    }

    // thread clean
    if (udp_recv_socket != -1) {
        shutdown(udp_recv_socket, SHUT_RDWR);
        close(udp_recv_socket);
        udp_recv_socket = -1;
     }
    if (udp_send_socket != -1) {
        shutdown(udp_send_socket, SHUT_RDWR);
        close(udp_send_socket);
        udp_send_socket = -1;
     }
    // exit
    *threadStatusPtr = EXITING;
    return NULL;
}

void *syncroThread(void *statusAdr)
{
    int udp_recv_socket = -1;
    int udp_send_socket = -1;
    struct  sockaddr_in DestinationAddress;
    struct  sockaddr_in ServerAddress;
    int threadInitOK = FALSE;
    enum threadStatus *threadStatusPtr = (enum threadStatus *)statusAdr;

    // thread init
    udp_recv_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_recv_socket != -1) {
        if (fcntl(udp_recv_socket, F_SETFL, O_NONBLOCK) >= 0) {
            memset((char *)&ServerAddress,0,sizeof(ServerAddress));
            ServerAddress.sin_family = AF_INET;
            ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
            ServerAddress.sin_port = htons((u_short)THE_SYNC_RECV_PORT);
            if (bind(udp_recv_socket, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress)) >= 0) {
                udp_send_socket = socket(AF_INET, SOCK_DGRAM, 0);
                if (udp_send_socket >= 0) {
                    struct hostent *h = gethostbyname(THE_UDP_SEND_ADDR);
                    if (h != NULL) {
                        memset(&DestinationAddress, 0, sizeof(DestinationAddress));
                        DestinationAddress.sin_family = h->h_addrtype;
                        memcpy((char *) &DestinationAddress.sin_addr.s_addr,
                                h->h_addr_list[0], h->h_length);
                        DestinationAddress.sin_port = htons(THE_SYNC_SEND_PORT);
                        threadInitOK = TRUE;
                    }
                }
            }
        }
    }

    // run
    *threadStatusPtr = RUNNING;
    for (;;) {
        if (g_bRunning && threadInitOK) {
            // wait on server socket, only until timeout
            fd_set recv_set;
            struct timeval tv;
            FD_ZERO(&recv_set);
            FD_SET(udp_recv_socket, &recv_set);
            tv.tv_sec = THE_DELAY_ms / 1000;
            tv.tv_usec = (THE_DELAY_ms % 1000) * 1000;
            if (select(udp_recv_socket + 1, &recv_set, NULL, NULL, &tv) <= 0) {
                // timeout or error
                continue;
            }
            // ready data: receive and immediately reply
            pthread_mutex_lock(&theMutex);
            {
                int rc = do_recv(udp_recv_socket, the_IsyncRegisters, THE_SYNC_UDP_SIZE);
                if (rc != THE_SYNC_UDP_SIZE) {
                    // ?
                }
                // maintain hardware type for the plc application (see syncroInitialize())
                // hardware type is a 32 bit register and the offset is in bytes
                uint32_t *data = (uint32_t *)the_IsyncRegisters;
                data[HARDWARE_TYPE_OFFSET/sizeof(uint32_t)] = hardware_type;
                int sn = do_sendto(udp_send_socket, the_QsyncRegisters, THE_SYNC_UDP_SIZE,
                        &DestinationAddress);
                if (sn != THE_SYNC_UDP_SIZE) {
                    // ?
                }
            }
            pthread_mutex_unlock(&theMutex);
        } else if (g_bExiting) {
            break;
        } else {
            usleep(THE_DELAY_ms * 1000);
        }
    }

    // thread clean
    if (udp_recv_socket != -1) {
        shutdown(udp_recv_socket, SHUT_RDWR);
        close(udp_recv_socket);
        udp_recv_socket = -1;
     }
    if (udp_send_socket != -1) {
        shutdown(udp_send_socket, SHUT_RDWR);
        close(udp_send_socket);
        udp_send_socket = -1;
     }
    // exit
    *threadStatusPtr = EXITING;
    return NULL;
}

/* ----  Implementations:	--------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/**
 * dataInitialize
 *
 */
IEC_UINT dataInitialize(IEC_UINT uIOLayer)
{
	IEC_UINT        uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running dataInitialize() ...\n");
#endif
#if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo(TASK_OFFS_IOL_DAT, osGetTaskID());
	TR_RET(uRes);
#endif

    theDataThread_id = -1;
    theDataThreadStatus = NOT_STARTED;
    theSyncThread_id = -1;
    theSyncThreadStatus = NOT_STARTED;

    g_bInitialized	= TRUE;
	g_bConfigured	= FALSE;
	g_bRunning	= FALSE;
	g_bExiting	= FALSE;
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dataFinalize
 *
 */
IEC_UINT dataFinalize(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT        uRes = OK;

#if defined(RTS_CFG_SYSLOAD)
	uRes = ldClearTaskInfo(TASK_OFFS_IOL_DAT);
	TR_RET(uRes);
#endif

	// io layer clean
	g_bInitialized	= FALSE;
	g_bConfigured	= FALSE;
	g_bRunning	= FALSE;
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dataNotifyConfig
 *
 */
IEC_UINT dataNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running dataNotifyConfig() ...\n");
#endif
    if (pIO->I.ulSize < THE_DATA_SIZE
     || pIO->Q.ulSize < THE_DATA_SIZE
     || pIO->M.ulSize < THE_DATA_SIZE) {
        uRes = ERR_INVALID_PARAM;
    }
    // set hardware type according to configuration files
#ifdef RTS_CFG_IOCANOPEN
    app_config_load(APP_CONF_CAN0);
    app_config_load(APP_CONF_CAN1);
    if (can0_cfg.enabled) {
        hardware_type |= (can0_cfg.enabled & 0xFF) << 0;	// 0x000000FF
    }
    if (can1_cfg.enabled) {
        hardware_type |= (can1_cfg.enabled & 0xFF) << 8;	// 0x0000FF00
    }
#endif
#ifdef RTS_CFG_IOMBRTUC
    app_config_load(APP_CONF_MB0);
    app_config_load(APP_CONF_MB1);
    if (modbus0_cfg.serial_cfg.enabled) {
        hardware_type |= (modbus0_cfg.serial_cfg.enabled & 0xFF) << 16;	// 0x00FF0000
    }
    if (modbus1_cfg.serial_cfg.enabled) {
        hardware_type |= (modbus1_cfg.serial_cfg.enabled & 0xFF) << 24;	// 0xFF000000
    }
#endif
#ifdef RTS_CFG_MECT_LIB // now undefined
#endif
    // maintain hardware type for the plc application
    // hardware type is a 32 bit register and the offset is in bytes
    HardwareType = hardware_type;
    g_bConfigured	= TRUE;
	g_bRunning	= FALSE;
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dataNotifyStart
 *
 */
IEC_UINT dataNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running dataNotifyStart() ... \n");
#endif

    // iolayer init
    // load retentive area in %I %M %Q
#if defined(RTS_CFG_MECT_RETAIN)
	void *pvIsegment = (void *)(((char *)(pIO->I.pAdr + pIO->I.ulOffs)) + 4);
    void *pvMsegment = (void *)(((char *)(pIO->M.pAdr + pIO->M.ulOffs)) + 4);
    void *pvQsegment = (void *)(((char *)(pIO->Q.pAdr + pIO->Q.ulOffs)) + 4);

    OS_MEMCPY(pvIsegment, ptRetentive, lenRetentive);
	OS_MEMCPY(pvMsegment, ptRetentive, lenRetentive);
	OS_MEMCPY(pvQsegment, ptRetentive, lenRetentive);
#endif

    // initialize array
    PLCRevision01 = rev01;
    PLCRevision02 = rev02;

    // start the engine, data and sync threads
    if (osPthreadCreate(&theEngineThread_id, &theEngineThreadStatus, &engineThread, NULL, "engine", 0) == 0) {
        do {
            usleep(1000);
        } while (theEngineThreadStatus != RUNNING);
    } else {
#if defined(RTS_CFG_IO_TRACE)
        osTrace("[%s] ERROR creating engine thread: %s.\n", __func__, strerror(errno));
#endif
        uRes = ERR_FB_INIT;
    }
    if (osPthreadCreate(&theDataThread_id, &theDataThreadStatus, &dataThread, NULL, "data", 0) == 0) {
        do {
            usleep(1000);
        } while (theDataThreadStatus != RUNNING);
    } else {
#if defined(RTS_CFG_IO_TRACE)
        osTrace("[%s] ERROR creating data thread: %s.\n", __func__, strerror(errno));
#endif
        uRes = ERR_FB_INIT;
    }
    if (osPthreadCreate(&theSyncThread_id, &theSyncThreadStatus, &syncroThread, NULL, "syncro", 0) == 0) {
        do {
            usleep(1000);
        } while (theSyncThreadStatus != RUNNING);
    } else {
#if defined(RTS_CFG_IO_TRACE)
        osTrace("[%s] ERROR creating syncro thread: %s.\n", __func__, strerror(errno));
#endif
        uRes = ERR_FB_INIT;
    }

    g_bRunning = TRUE;
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dataNotifyStop
 *
 */
IEC_UINT dataNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running dataNotifyStop() ...\n");
#endif
	g_bRunning = FALSE;
    // stop the threads
    g_bExiting	= TRUE;
    while (theEngineThreadStatus == RUNNING
        || theModbusRTU1ThreadStatus == RUNNING
        || theModbusTCPThreadStatus == RUNNING
        || theModbusTCPRTUThreadStatus == RUNNING
        || theDataThreadStatus == RUNNING
        || theSyncThreadStatus == RUNNING) {
        usleep(1000);
    }
    RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dataNotifySet
 *
 */
IEC_UINT dataNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;

    if (g_bRunning) {
        pthread_mutex_lock(&theMutex);
        {
			if (pNotify->uTask != 0xffffu) {
                // notify from a plc task

                // check the write copy regions
                SImageReg   *pIRs = (SImageReg *)pIO->C.pAdr;
                SImageReg   *pIR = &pIRs[pNotify->uTask];

                if (pIR->pSetQ[uIOLayer] == FALSE) {
                    // nothing to do
                    uRes = OK;
                } else {
                   // copy the whole __%M__ segment to the Q registers (completely ignoring the %Q write copy regions)
                    void *pvMsegment = pIO->M.pAdr + pIO->M.ulOffs;
                    void *pvQsegment = pIO->Q.pAdr + pIO->Q.ulOffs;
                    // OS_MEMCPY(the_QdataRegisters, pvMsegment, THE_DATA_SIZE);
                    // OS_MEMCPY(pvQsegment, pvMsegment, THE_DATA_SIZE);
#if defined(RTS_CFG_MECT_RETAIN)
                    // retentive memory update
                    // void *pvSendDataToRetentive = pvMsegment + 4;
                    // OS_MEMCPY(ptRetentive, pvSendDataToRetentive, lenRetentive);
#endif
                }
            } else if (pNotify->usSegment != SEG_OUTPUT) {
                uRes = ERR_WRITE_TO_INPUT;
            } else if (pNotify->usBit != 0) {
                uRes = ERR_INVALID_PARAM;
            } else {
                // notify from others
                IEC_UDINT ulStart = vmm_max(pNotify->ulOffset, pIO->Q.ulOffs);
                IEC_UDINT ulStop = vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->Q.ulOffs + pIO->Q.ulSize);
                if (ulStart < ulStop) {
                    void * source = pIO->Q.pAdr + ulStart;
                    void * dest = the_QdataRegisters;
                    dest += ulStart - pIO->Q.ulOffs;
                    OS_MEMCPY(dest, source, ulStop - ulStart);
                }
            }
        }
        pthread_mutex_unlock(&theMutex);
    }
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dataNotifyGet
 *
 */
IEC_UINT dataNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;

    if (g_bRunning) {
        pthread_mutex_lock(&theMutex);
        {
            if (pNotify->uTask != 0xffffu) {
                // notify from a plc task

                // check the read copy regions
                SImageReg   *pIRs = (SImageReg *)pIO->C.pAdr;
                SImageReg   *pIR = &pIRs[pNotify->uTask];

                if (pIR->pGetQ[uIOLayer] == FALSE && pIR->pGetI[uIOLayer] == FALSE) {
                    // nothing to do
                    uRes = OK;
                } else {
                    // search the read copy regions for requests to this IOLayer
                    IEC_UINT	r;

                    for (r = 0; uRes == OK && r < pIR->uRegionsRd; ++r) {
                        // commented out for enabling %M forced update
                        // if (pIR->pRegionRd[r].pGetQ[uIOLayer] == FALSE && pIR->pRegionRd[r].pGetI[uIOLayer] == FALSE) {
                        //     continue;
                        // }
                        IEC_UDINT	ulStart;
                        IEC_UDINT	ulStop;
                        void * source;
                        void * dest;

                        if (pIR->pRegionRd[r].usSegment == SEG_GLOBAL) {
                            ulStart = vmm_max(pIR->pRegionRd[r].ulOffset, pIO->M.ulOffs);
                            ulStop	= vmm_min(pIR->pRegionRd[r].ulOffset + pIR->pRegionRd[r].uSize, pIO->M.ulOffs + pIO->M.ulSize);
                            source = the_QdataRegisters; // i.e. forcing %M from %Q for retro compatibility
                            source += ulStart - pIO->Q.ulOffs;
                            dest = pIO->M.pAdr + ulStart;
                        } else if (pIR->pRegionRd[r].usSegment == SEG_OUTPUT) {
                            ulStart = vmm_max(pIR->pRegionRd[r].ulOffset, pIO->Q.ulOffs);
                            ulStop	= vmm_min(pIR->pRegionRd[r].ulOffset + pIR->pRegionRd[r].uSize, pIO->Q.ulOffs + pIO->Q.ulSize);
                            source = the_QdataRegisters;
                            source += ulStart - pIO->Q.ulOffs;
                            dest = pIO->Q.pAdr + ulStart;
                        } else if (pIR->pRegionRd[r].usSegment == SEG_INPUT) {
                            ulStart = vmm_max(pIR->pRegionRd[r].ulOffset, pIO->I.ulOffs);
                            ulStop	= vmm_min(pIR->pRegionRd[r].ulOffset + pIR->pRegionRd[r].uSize, pIO->I.ulOffs + pIO->I.ulSize);
                            source = the_IdataRegisters;
                            source += ulStart - pIO->I.ulOffs;
                            dest = pIO->I.pAdr + ulStart;
                        } else {
                            // wrong data
                            continue;
                        }
                        if (ulStart < ulStop) {
                            OS_MEMCPY(dest, source, ulStop - ulStart);
                        }
                    }
                }
            } else if (pNotify->usBit != 0) {
                // no bit access
                uRes = ERR_INVALID_PARAM;
            } else if (pNotify->usSegment != SEG_INPUT && pNotify->usSegment != SEG_OUTPUT){
                uRes = ERR_INVALID_PARAM;
            } else {
                // notify from others
                IEC_UDINT	ulStart;
                IEC_UDINT	ulStop;
                void * source;
                void * dest;

                if (pNotify->usSegment == SEG_INPUT) {
                    ulStart	= vmm_max(pNotify->ulOffset, pIO->I.ulOffs);
                    ulStop	= vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->I.ulOffs + pIO->I.ulSize);
                    source = the_IdataRegisters;
                    source += ulStart - pIO->I.ulOffs;
                    dest = pIO->I.pAdr + ulStart;
                } else { // pNotify->usSegment == SEG_OUTPUT
                    ulStart	= vmm_max(pNotify->ulOffset, pIO->Q.ulOffs);
                    ulStop	= vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->Q.ulOffs + pIO->Q.ulSize);
                    source = the_QdataRegisters;
                    source += ulStart - pIO->Q.ulOffs;
                    dest = pIO->Q.pAdr + ulStart;
                }
                if (ulStart < ulStop) {
                    OS_MEMCPY(dest, source, ulStop - ulStart);
                }
            }
        }
        pthread_mutex_unlock(&theMutex);
    }
	RETURN(uRes);
}

#endif /* RTS_CFG_IODAT */

/* ---------------------------------------------------------------------------- */
