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

static pthread_t theDataThread_id = -1;
static pthread_t theSyncThread_id = -1;
static enum { NOT_STARTED, RUNNING, EXITING } theDataThreadStatus = NOT_STARTED, theSyncThreadStatus = NOT_STARTED;

static uint32_t the_IdataRegisters[REG_DATA_NUMBER];
static uint32_t the_QdataRegisters[REG_DATA_NUMBER];
static IEC_BOOL the_IdataRegistersOK = FALSE;

static uint16_t the_IsyncRegisters[REG_SYNC_NUMBER];
static uint16_t the_QsyncRegisters[REG_SYNC_NUMBER];
static IEC_BOOL the_IsyncRegistersOK = FALSE;

/* ----  Local Functions:	--------------------------------------------------- */

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

void *dataThread(void *dontcare)
{
    int udp_recv_socket = -1;
    int udp_send_socket = -1;
    struct  sockaddr_in DestinationAddress;
    struct  sockaddr_in ServerAddress;
    int threadInitOK = FALSE;

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
    theDataThreadStatus = RUNNING;
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
                the_IdataRegistersOK = TRUE;
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
    theDataThreadStatus = EXITING;
    return dontcare;
}

void *syncroThread(void *dontcare)
{
    int udp_recv_socket = -1;
    int udp_send_socket = -1;
    struct  sockaddr_in DestinationAddress;
    struct  sockaddr_in ServerAddress;
    int threadInitOK = FALSE;

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
    theSyncThreadStatus = RUNNING;
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
                the_IsyncRegistersOK = TRUE;
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
    theSyncThreadStatus = EXITING;
    return dontcare;
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
    the_IsyncRegistersOK = TRUE; // FIXME
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
	OS_MEMCPY(pvIsegment, ptRetentive, lenRetentive);
	
#ifdef RTS_CFG_MEMORY_AREA_EXPORT
	void *pvMsegment = (void *)(((char *)(pIO->M.pAdr + pIO->M.ulOffs)) + 4);

	OS_MEMCPY(pvMsegment, ptRetentive, lenRetentive);
#else
	void * pvQsegment = (void *)(((char *)(pIO->Q.pAdr + pIO->Q.ulOffs)) + 4);
	OS_MEMCPY(pvQsegment, ptRetentive, lenRetentive);
#endif
#endif

    // initialize array
    PLCRevision01 = rev01;
    PLCRevision02 = rev02;

    // start the data and sync threads
    pthread_attr_t pattr;
    pthread_attr_init(&pattr);
    if (osPthreadCreate(&theDataThread_id, &pattr, &dataThread, NULL, "data", 0) == 0) {
        do {
            usleep(1000);
        } while (theDataThreadStatus != RUNNING);
    } else {
#if defined(RTS_CFG_IO_TRACE)
        osTrace("[%s] ERROR message_th: %s.\n", __func__, strerror(errno));
#endif
        uRes = ERR_FB_INIT;
    }
    pthread_attr_init(&pattr);
    if (osPthreadCreate(&theSyncThread_id, &pattr, &syncroThread, NULL, "syncro", 0) == 0) {
        do {
            usleep(1000);
        } while (theSyncThreadStatus != RUNNING);
    } else {
#if defined(RTS_CFG_IO_TRACE)
        osTrace("[%s] ERROR message_th: %s.\n", __func__, strerror(errno));
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
    while (theDataThreadStatus == RUNNING || theSyncThreadStatus == RUNNING) {
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

    if (g_bRunning && the_IdataRegistersOK) {
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
                        if (pIR->pRegionRd[r].pGetQ[uIOLayer] == FALSE && pIR->pRegionRd[r].pGetI[uIOLayer] == FALSE) {
                            continue;
                        }
                        if (pIR->pRegionRd[r].usSegment != SEG_INPUT && pIR->pRegionRd[r].usSegment != SEG_OUTPUT) {
                            continue;
                        }
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
