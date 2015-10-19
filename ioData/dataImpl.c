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
#include "libHW119.h"
#include "libModbus.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define THE_CONFIG_DELAY_ms	1
#define THE_ENGINE_DELAY_ms	25
#define THE_MODBUS_DELAY_ms 10
#define THE_UDP_TIMEOUT_ms	500
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

static int do_recv(int s, void *buffer, ssize_t len);
static int do_sendto(int s, void *buffer, ssize_t len, struct sockaddr_in *address);
static void InitXtable(uint16_t TIPO);
static int ReadFields(int16_t Index);
static int ReadAlarmsFields(int16_t Index);
static int ReadCommFields(int16_t Index);
static int LoadXTable(char *CTFile, int32_t CTDimension, uint16_t CTType);
static void AlarmMngr(void);
static void PLCsync(void);
static void ErrorMNG(void);

/* ----  Implementations:	--------------------------------------------------- */

static int do_recv(int s, void *buffer, ssize_t len)
{
    int retval = 0;
    ssize_t sent = 0;

    retval = recv(s, buffer + sent, len - sent, 0);
    if (retval < len) {
        retval = -1;
    }
    return retval;
}

static int do_sendto(int s, void *buffer, ssize_t len, struct sockaddr_in *address)
{
    int retval = 0;
    ssize_t sent = 0;

    retval = sendto(s, buffer + sent, len - sent, 0,
                    (struct sockaddr *) address, sizeof(struct sockaddr_in));
    if (retval < len) {
        retval = -1;
    }
    return retval;
}

// fb_HW119_Init.st
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
            k = ARRAY_QUEUE[i + 1];
            ARRAY_QUEUE_OUTPUT[i + 1] = 0;
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

// fb_HW119_ReadVarFields.st
static int ReadFields(int16_t Index)
{
    int ERR = 0;
    IEC_STRMAX Field;
    HW119_GET_CROSS_TABLE_FIELD param;
    param.field = &Field;

    // Enable {0,1,2,3}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        CrossTable[Index].Enable = atoi(Field.Contents);
        HW119_ERR[3] = 2;
    } else {
        HW119_ERR[3] = 3;
        ERR = TRUE;
    }

    // Plc {H,P,S,F}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        switch (Field.Contents[0]) {
        case 'P':
            CrossTable[Index].Plc = TRUE;
            break;
        case 'H':
            CrossTable[Index].Plc = FALSE;
            break;
        case 'S':
        case 'F':
        default:
            // nothing
            ;
        }
    } else {
        ERR = TRUE;
    }

    // Tag {identifier}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        strncpy(CrossTable[Index].Tag, param.field->Contents, VMM_MAX_IEC_STRLEN);
    } else {
        ERR = TRUE;
    }

    // Types {UINT, UDINT, DINT, FDCBA, ...}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        if (strncmp(Field.Contents, "UINT", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = UINT16;
        } else if (strncmp(Field.Contents, "INT", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = INT16;
        } else if (strncmp(Field.Contents, "UDINT", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = UDINT32;
        } else if (strncmp(Field.Contents, "DINT", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = DINT32;
        } else if (strncmp(Field.Contents, "FDCBA", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = FLOATDCBA;
        } else if (strncmp(Field.Contents, "FCDAB", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = FLOATCDAB;
        } else if (strncmp(Field.Contents, "FABCD", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = FLOATABCD;
        } else if (strncmp(Field.Contents, "FBADC", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = FLOATBADC;
        } else if (strncmp(Field.Contents, "BIT", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = BIT;
        } else if (strncmp(Field.Contents, "UDINTDCBA", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = UDINTDCBA;
        } else if (strncmp(Field.Contents, "UDINTCDAB", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = UDINTCDAB;
        } else if (strncmp(Field.Contents, "UDINTABCD", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = UDINTABCD;
        } else if (strncmp(Field.Contents, "UDINTBADC", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = UDINTBADC;
        } else if (strncmp(Field.Contents, "DINTDCBA", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = DINTDCBA;
        } else if (strncmp(Field.Contents, "DINTCDAB", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = DINTCDAB;
        } else if (strncmp(Field.Contents, "DINTABCD", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = DINTABCD;
        } else if (strncmp(Field.Contents, "DINTBADC", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Types = DINTBADC;
        } else {
            if (CrossTable[Index].Enable > 0) {
                CrossTable[Index].Types = 100; // tipo non riconosciuto
                ERROR_FLAG |= 0x20;
            }
        }
    } else {
        ERR = TRUE;
    }

    // Decimal {0, 1, 2, 3, 4, ...}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        CrossTable[Index].Decimal = atoi(param.field->Contents);
    } else {
        ERR = TRUE;
    }

    // Protocol {"", RTU, TCP, TCPRTU}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        if (strncmp(Field.Contents, "RTU", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Protocol = 0;
            RTUProtocol_ON = TRUE;
        } else if (strncmp(Field.Contents, "TCP", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Protocol = 1;
            TCPProtocol_ON = TRUE;
        } else if (strncmp(Field.Contents, "TCPRTU", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].Protocol = 2;
            TCPRTUProtocol_ON = TRUE;
        } else {
            CrossTable[Index].Protocol = 100; // PROTOCOLLO NON RICONOSCIUTO
            // FIXME: ERR ?
        }
    } else {
        ERR = TRUE;
    }

    // IPAddress {identifier}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        strncpy(CrossTable[Index].IPAddress, param.field->Contents, VMM_MAX_IEC_STRLEN);
    } else {
        ERR = TRUE;
    }

    // Port {number}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        CrossTable[Index].Port = atoi(param.field->Contents);
    } else {
        ERR = TRUE;
    }

    // NodeId {number}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        if (strncmp(CrossTable[Index].Protocol, "TCP", VMM_MAX_IEC_STRLEN) == 0) {
            CrossTable[Index].NodeId = 1;
        } else {
            CrossTable[Index].NodeId = atoi(param.field->Contents);
        }
    } else {
        ERR = TRUE;
    }

    // Address {number}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        CrossTable[Index].Address = atoi(param.field->Contents);
    } else {
        ERR = TRUE;
    }

    // Block {number}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        CrossTable[Index].Block = atoi(param.field->Contents);
    } else {
        ERR = TRUE;
    }

    // NReg {number}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        CrossTable[Index].NReg = atoi(param.field->Contents);
    } else {
        ERR = TRUE;
    }

    // Handle {text}
    // ignored

    return ERR;
}

// fb_HW119_ReadAlarmsFields.st
static int ReadAlarmsFields(int16_t Index)
{
    int ERR = FALSE;
    IEC_STRMAX Field;
    HW119_GET_CROSS_TABLE_FIELD param;
    param.field = &Field;

    // ALType {0,1}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        ALCrossTable[Index].ALType = atoi(Field.Contents);
    } else {
        ERR = TRUE;
    }

    // ALTag {identifier}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        strncpy(ALCrossTable[Index].ALTag, param.field->Contents, VMM_MAX_IEC_STRLEN);
    } else {
        ERR = TRUE;
    }

    // ALSource {identifier}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        strncpy(ALCrossTable[Index].ALSource, param.field->Contents, VMM_MAX_IEC_STRLEN);
    } else {
        ERR = TRUE;
    }

    // ALCompareVar {identifier}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        strncpy(ALCrossTable[Index].ALCompareVar, param.field->Contents, VMM_MAX_IEC_STRLEN);
    } else {
        ERR = TRUE;
    }

    // ALCompareVal {number}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        ALCrossTable[Index].ALCompareVal = atoi(Field.Contents);
    } else {
        ERR = TRUE;
    }

    // ALOperator {number}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        ALCrossTable[Index].ALOperator = atoi(Field.Contents);
    } else {
        ERR = TRUE;
    }

    // ALFilterTime {number}
    hw119_get_cross_table_field(NULL, NULL, &param);
    if (param.ret_value == 0) {
        ALCrossTable[Index].ALFilterTime = atoi(Field.Contents);
        ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterTime;
    } else {
        ERR = TRUE;
    }

    return ERR;
}

// fb_HW119_ReadCommFields.st
static int ReadCommFields(int16_t Index)
{
    int ERR = FALSE;
    IEC_STRMAX Field;
    HW119_GET_CROSS_TABLE_FIELD param;
    param.field = &Field;

    switch (Index) {
    case 1: // retries
        // NumberOfFails {number}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            NumberOfFails = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // FailDivisor {number}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            FailDivisor = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        break;
    case 2: // priorities
        // Talta {number}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            Talta = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // Tmedia {number}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            Tmedia = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // Tbassa {number}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            Tbassa = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        break;
    case 3: // rtu
    case 4: // tcp
    case 5: // tcprtu
        // Device {text}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            strncpy(CommParameters[Index - 2].Device, param.field->Contents, VMM_MAX_IEC_STRLEN);
        } else {
            ERR = TRUE;
        }
        // IPaddr {text}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            strncpy(CommParameters[Index - 2].IPaddr, param.field->Contents, VMM_MAX_IEC_STRLEN);
        } else {
            ERR = TRUE;
        }
        // Port {number}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            CommParameters[Index - 2].Port = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // BaudRate {number}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            CommParameters[Index - 2].BaudRate = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // Parity {text}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            strncpy(CommParameters[Index - 2].Parity, param.field->Contents, VMM_MAX_IEC_STRLEN);
        } else {
            ERR = TRUE;
        }
        // DataBit {number}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            CommParameters[Index - 2].DataBit = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // StopBit {number}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            CommParameters[Index - 2].StopBit = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // Tmin {number}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            CommParameters[Index - 2].Tmin = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // TimeOut {number}
        hw119_get_cross_table_field(NULL, NULL, &param);
        if (param.ret_value == 0) {
            CommParameters[Index - 2].TimeOut = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        break;

    default:
        ;
    }

    return ERR;
}

// fb_HW119_LoadCrossTab.st
static int LoadXTable(char *CTFile, int32_t CTDimension, uint16_t CTType)
{
    // InitXtable: fb_HW119_Init;
    // ReadFields:fb_HW119_ReadVarFields;
    // ReadAlarmsFields:fb_HW119_ReadAlarmsFields;
    // ReadCommFields:fb_HW119_ReadCommFields;
    uint16_t FBState = 0;
    // uint16_t FunctRes;
    int32_t index;
    int32_t STEPS = 50;
    int32_t LowEdge = 0;
    int32_t HiEdge = 0;
    STaskInfoVMM *pVMM = get_pVMM();
    int END = FALSE;
    int ERR = FALSE;

    while (!END) {
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
        case 10: {
            IEC_STRMAX filename;
            filename.MaxLen = VMM_MAX_IEC_STRLEN;
            filename.CurLen = strnlen(CTFile, filename.MaxLen);
            strncpy(filename.Contents, CTFile, filename.CurLen);
            HW119_OPEN_CROSS_TABLE_PARAM param = { &filename, 0 };
            hw119_open_cross_table(NULL, NULL, &param);
             if (param.ret_value == 0)  {	// Operazione andata a buon fine
                FBState = 20;
                HW119_ERR[2] = 2;
            } else {
                FBState = 1000;	// Errore -> termina
                ErrorsState = ErrorsState | 0x01;
                HW119_ERR[2] = 3;
            }
        }   break;
        case 20: {
            HW119_READ_CROSS_TABLE_RECORD_PARAM param;
            for (index = LowEdge; index <= HiEdge; ++ index) {
                if (CTType > 0) {
                    param.error = TRUE;
                } else {
                    param.error = FALSE;
                }
                hw119_read_cross_table_record(NULL, NULL, &param);
                if (param.ret_value == 0) {
                    HW119_ERR[1] = 2;
                    switch (CTType) {
                    case 0:
                        if (ReadFields(index)) {
                            CrossTable[index].Error = 100;
                            ErrorsState |= 0x04;
                        }
                        break;
                    case 1:
                        if (ReadAlarmsFields(index)) {
                            ALCrossTable[index].ALError = 100;
                            ErrorsState |= 0x04;
                        }
                        break;
                    default:
                        if (ReadCommFields(index) && (index > 1)) {
                            CommParameters[index - 1].State = FALSE;
                        } else {
                            CommParameters[index - 1].State = TRUE;
                        }                    }
                } else {
                    HW119_ERR[1] = 3;
                    if (CTType == 0) {
                        CrossTable[index].Error = 100;
                        // marca la entry della crosstable come non ancora aggiornata
                        ErrorsState |= 0x02;
                    } else if (CTType == 1) {
                        ALCrossTable[index].ALError = 100;
                        // marca la entry della crosstable ALLARMI come non ancora aggiornata
                        ErrorsState |= 0x04;
                    }
                }
            }
            FBState = 30;
        }   break;
        case 30:
            LowEdge = HiEdge + 1;
            HiEdge = HiEdge + STEPS;
            if (LowEdge < CTDimension) {
                if (HiEdge < CTDimension) {
                    FBState = 20;
                } else if (HiEdge >= CTDimension) {
                    HiEdge = CTDimension;
                    FBState = 20;
                }
            } else {
                FBState = 40;
            }
            break;
        case 40: {
            HW119_CLOSE_CROSS_TABLE_PARAM param;
            hw119_close_cross_table(NULL, NULL, &param);
            if (param.ret_value == 0) {
                HW119_ERR[0] = 2;
                FBState = 100;
            } else {
                FBState = 1000;
                HW119_ERR[0] = 3;
                ErrorsState |= 0x80;
            }
        }   break;
        case 100:
            END = TRUE;
            break;
        case 1000: {
            HW119_CLOSE_CROSS_TABLE_PARAM param;
            hw119_close_cross_table(NULL, NULL, &param);
            FBState = 1010;
            ERR = TRUE;
        }   break;
        case 1010:
            END = TRUE;
            break;
        default:
            END = TRUE;
            ERR = TRUE;
        }
    }
    return ERR;
}

// fb_HW119_AlarmsMngr.st
static void AlarmMngr(void)
{
}

// fb_HW119_PLCsync.st (NB: NOW IT'S CALLED BY SYNCRO)
static void PLCsync(void)
{
    uint16_t IndexPLC;
    uint16_t RW;
    int16_t addr;

    for (IndexPLC = 1; IndexPLC <= DimCrossTable; ++IndexPLC) {
        RW = ARRAY_QUEUE[IndexPLC] & QueueRWMask;
        addr = ARRAY_QUEUE[IndexPLC] & QueueAddressMask;
        if (addr < DimCrossTable) {
            if (RW == 0 && addr == 0) {
                ARRAY_QUEUE_OUTPUT[IndexPLC] = 0;
                break;
            }
            if (CrossTable[addr].Protocol == 100) {
                if (RW == 0 && ARRAY_QUEUE_OUTPUT[IndexPLC] == STATO_BUSY_WRITE) {
                    ARRAY_QUEUE_OUTPUT[IndexPLC] = 0;
                } else if ((RW == WRITE || RW == WRITE_RIC_SINGLE || RW == WRITE_MULTIPLE) && ARRAY_QUEUE_OUTPUT[IndexPLC] != STATO_BUSY_WRITE) {
                    ARRAY_QUEUE_OUTPUT[IndexPLC] = STATO_BUSY_WRITE;
                    if (RW == WRITE || RW == WRITE_RIC_SINGLE) {
                        ARRAY_CROSSTABLE_INPUT[addr] = ARRAY_CROSSTABLE_OUTPUT[addr];
                    } else {
                        uint16_t I;

                        for (I = 0; I <= (CrossTable[addr].NReg - 1); ++I) {
                            ARRAY_CROSSTABLE_INPUT[addr + I] = ARRAY_CROSSTABLE_OUTPUT[addr + I];
                            CrossTable[addr + I].Error = 0; // FIXME: remove line
                        }
                    }
                }
            }
        }
    }
}

// fb_TPAC_tic.st
// fb_TPAC1006_LIOsync.st
// fb_TPAC1007_LIOsync.st
static void LocalIO(void)
{
    // TICtimer
    float PLC_time, PLC_timeMin, PLC_timeMax;
    uint32_t tic_ms;

    tic_ms = osGetTime32Ex() % (86400 * 1000); // 1 day overflow
    PLC_time = tic_ms / 1000.0;
    if (PLC_time <= 10.0) {
        PLC_timeMin = 0;
        PLC_timeMax = 10.0;
    } else {
        PLC_timeMin = PLC_time - 10.0;
        PLC_timeMax = PLC_time;
    }
    // PLC_time    AT %MD0.21560:REAL; 5390
    // PLC_timeMin AT %MD0.21564:REAL; 5391
    // PLC_timeMax AT %MD0.21568:REAL; 5392
    memcpy(&ARRAY_CROSSTABLE_INPUT[5390], &PLC_time, sizeof(uint32_t));
    memcpy(&ARRAY_CROSSTABLE_INPUT[5391], &PLC_timeMin, sizeof(uint32_t));
    memcpy(&ARRAY_CROSSTABLE_INPUT[5392], &PLC_timeMax, sizeof(uint32_t));

    // TPAC1006, TPAC1008
    if (HardwareType & 0x000000FF) {
        // TPAC1006_LIOsync();
    }
    // TPAC1007
    if (HardwareType & 0x00FF0000) {
        // TPAC1007_LIOsync();
    }

}

// fb_HW119_ErrorMng.st
static void ErrorMNG(void)
{
    // empty
}

// Program1.st
void *engineThread(void *statusAdr)
{
    // InitComm:fb_HW119_InitComm;
    // AlarmMngr:fb_HW119_AlarmsMngr;
    // LoadXTable:fb_HW119_LoadCrossTab;
    // ErrorMNG:fb_HW119_ErrorMng;
    // PLCsync:fb_HW119_PLCsync;
    // Reconnect:fb_HW119_Check;
    // TPAC1006_LIOsync:fb_TPAC1006_LIOsync;
    // TPAC1007_LIOsync:fb_TPAC1007_LIOsync;
    // TICtimer:fb_TPAC_tic;
    // uint16_t FunctRes = 0;
    uint32_t CommState = 0;
    // uint32_t COUNTER = 0;
    // timer_sync:TON;

    // thread init
    enum threadStatus *threadStatusPtr = (enum threadStatus *)statusAdr;
    while (!g_bExiting && CommState < 80) {
        usleep(THE_CONFIG_DELAY_ms * 1000);
        if (g_bRunning) {
            pthread_mutex_lock(&theMutex);
            {
                switch (CommState) {
                case 0:
                    // FunctRes = HW119_CloseCrossTable();
                    // LoadXTable(ENABLE:=FALSE);
                    // InitComm(ENABLE:=FALSE);
                    ErrorsState = 0;
                    CommState = 10;
                    // COUNTER = 0;
                    // RTU_RUN = FALSE;
                    // TCP_RUN = FALSE;
                    // TCPRTU_RUN = FALSE;
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
                        // RTU_RUN = TRUE;
                    } else {
                #if defined(RTS_CFG_IO_TRACE)
                        osTrace("[%s] ERROR creating modbus RTU1 thread: %s.\n", __func__, strerror(errno));
                #endif
                        // RTU_RUN = FALSE;
                    }
                    CommState = 50;
                    break;
                case 50:
                    if (osPthreadCreate(&theModbusTCPThread_id, &theModbusTCPThreadStatus, &modbusThread, NULL, "modbusTCP", 0) == 0) {
                        do {
                            usleep(1000);
                        } while (theModbusTCPThreadStatus != RUNNING);
                        ERROR_FLAG = ERROR_FLAG | 0x100; // SEGNALO CHE IL TASK TCP è PARTITO
                        // TCP_RUN = TRUE;
                    } else {
                #if defined(RTS_CFG_IO_TRACE)
                        osTrace("[%s] ERROR creating modbus TCP thread: %s.\n", __func__, strerror(errno));
                #endif
                        // TCP_RUN = FALSE;
                    }
                    CommState = 60;
                    break;
                case 60:
                    if (osPthreadCreate(&theModbusTCPRTUThread_id, &theModbusTCPRTUThreadStatus, &modbusThread, NULL, "modbusTCPRTU", 0) == 0) {
                        do {
                            usleep(1000);
                        } while (theModbusTCPRTUThreadStatus != RUNNING);
                        ERROR_FLAG = ERROR_FLAG | 0x200; // SEGNALO CHE IL TASK TCPRTU è PARTITO
                        // TCPRTU_RUN = TRUE;
                    } else {
                #if defined(RTS_CFG_IO_TRACE)
                        osTrace("[%s] ERROR creating modbus TCPRU thread: %s.\n", __func__, strerror(errno));
                #endif
                        // TCPRTU_RUN = FALSE;
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
                default:
                    ;
                }
            }
            pthread_mutex_unlock(&theMutex);
        }
    }

    // run
    *threadStatusPtr = RUNNING;
    while (!g_bExiting) {
        usleep(THE_ENGINE_DELAY_ms * 1000);
        if (g_bRunning) {
            pthread_mutex_lock(&theMutex);
            {
                switch (CommState) {
                case 80:
                    // TRIGGER02 = TRIGGER02;
                    // TRIGGER01 = TRIGGER01;
                    if (CommEnabled)  {
                        AlarmMngr();					// Gestione allarmi*)
                    }
                    // Reconnect();	<-----= NOW IT'S MANAGED BY MODBUS
                    // PLCsync(); <-----= NOW IT'S CALLED BY SYNCRO
                    LocalIO();
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
        }
    }

    // thread clean
    // see dataNotifyStop()

    // exit
    *threadStatusPtr = EXITING;
    return NULL;
}


// RTU_Communication.st
// TCP_Communication.st
// TCPRTU_Communication.st
// fb_HW119_Check.st (no more fb_HW119_Reconnect.st)
// fb_HW119_MODBUS.st
// fb_HW119_InitComm.st
void *modbusThread(void *statusAdr)
{
    enum threadStatus *threadStatusPtr = (enum threadStatus *)statusAdr;
    enum modbusStatus {ZERO, NOT_CONNECTED, CONNECTED, TIMEOUT, ERROR, NO_HOPE};
    enum modbusStatus status = ZERO;

    int XProtocol_ON;
    uint32_t elapsed_ms = 0;
    uint32_t failures = 0;
    uint16_t CommID = 0;
    uint16_t ErroID = 0;
    modbus_t * ctx = NULL;
    struct timeval timeout;

    // thread init
    if (threadStatusPtr == &theModbusRTU0ThreadStatus) {
        // FIXME
        return;
    } else if (threadStatusPtr == &theModbusRTU1ThreadStatus) {
        XProtocol_ON = RTUProtocol_ON;
        CommID = 1;
        ErroID = 0x0000010;
    } else if (threadStatusPtr == &theModbusTCPThreadStatus) {
        XProtocol_ON = TCPProtocol_ON;
        CommID = 2;
        ErroID = 0x0000020;
    } else if (threadStatusPtr == &theModbusTCPRTUThreadStatus) {
        XProtocol_ON = TCPRTUProtocol_ON;
        CommID = 3;
        ErroID = 0x0000040;
    }

    if (CommParameters[CommID].TimeOut == 0) {
        CommParameters[CommID].TimeOut = 300;
    }
    timeout.tv_sec = 0;
    timeout.tv_usec = CommParameters[CommID].TimeOut * 1000;

    // run
    *threadStatusPtr = RUNNING;
    while (!g_bExiting) {

        usleep(THE_MODBUS_DELAY_ms * 1000);
        if (g_bRunning && XProtocol_ON) {
            switch (status) {

            case ZERO:
                if (threadStatusPtr == &theModbusRTU0ThreadStatus) {
                    // FIXME
                    return;
                } else if (threadStatusPtr == &theModbusRTU1ThreadStatus) {
                    ctx = modbus_new_rtu(CommParameters[CommID].Device,
                            CommParameters[CommID].BaudRate, CommParameters[CommID].Parity,
                            CommParameters[CommID].DataBit, CommParameters[CommID].StopBit);
                } else if (threadStatusPtr == &theModbusTCPThreadStatus) {
                    ctx = modbus_new_tcp(CommParameters[CommID].IPaddr,
                            CommParameters[CommID].Port);
                } else if (threadStatusPtr == &theModbusTCPRTUThreadStatus) {
                    ctx = modbus_new_tcprtu(CommParameters[CommID].IPaddr,
                            CommParameters[CommID].Port);
                }
                if (ctx != NULL) {
                    status = NOT_CONNECTED;
                    MODBUS_ERR[1 + CommID] = ERR_OK + 2;
                } else {
                    status = NO_HOPE; // FIXME
                    MODBUS_ERR[1 + CommID] = ERR_ERROR + 2;
                }
                break;

            case NOT_CONNECTED:
                MODBUS_ERR[5 + CommID -1] = 2;
                if (modbus_connect(ctx) >= 0) {
                    if (modbus_flush(ctx) >= 0) {
                        MODBUS_ERR[8] = 2;
                        modbus_set_response_timeout(ctx, &timeout);
                        MODBUS_ERR[9] = 2;
                        status = CONNECTED;
                    } else {
                        MODBUS_ERR[8] = 2;
                    }
                } else {
                    status = ERROR;
                }
               break;

            case CONNECTED:

                pthread_mutex_lock(&theMutex);
                {
                }
                pthread_mutex_unlock(&theMutex);
                if (FALSE) {
                    ++failures;
                    if (failures < 10) {
                        status = TIMEOUT;
                    } else {
                        failures = 0;
                        status = ERROR;
                    }
                }
                break;

            case TIMEOUT:
                elapsed_ms += THE_MODBUS_DELAY_ms;
                if (elapsed_ms >= 4000) {
                    elapsed_ms = 0;
                    status = NOT_CONNECTED;
                }
                break;

            case ERROR:
                ErrorsState |= ErroID;
                if (ctx != NULL) {
                    modbus_free(ctx);
                    ctx = NULL;
                }
                elapsed_ms += THE_MODBUS_DELAY_ms;
                if (elapsed_ms >= 10000) {
                    elapsed_ms = 0;
                    status = ZERO;
                }
                break;

            case NO_HOPE:
            default:
                ;
            }
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
    while (!g_bExiting) {
        if (g_bRunning && threadInitOK) {
            // wait on server socket, only until timeout
            fd_set recv_set;
            struct timeval tv;
            FD_ZERO(&recv_set);
            FD_SET(udp_recv_socket, &recv_set);
            tv.tv_sec = THE_UDP_TIMEOUT_ms / 1000;
            tv.tv_usec = (THE_UDP_TIMEOUT_ms % 1000) * 1000;
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
        } else {
            usleep(THE_UDP_TIMEOUT_ms * 1000);
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
    while (!g_bExiting) {
        if (g_bRunning && threadInitOK) {
            // wait on server socket, only until timeout
            fd_set recv_set;
            struct timeval tv;
            FD_ZERO(&recv_set);
            FD_SET(udp_recv_socket, &recv_set);
            tv.tv_sec = THE_UDP_TIMEOUT_ms / 1000;
            tv.tv_usec = (THE_UDP_TIMEOUT_ms % 1000) * 1000;
            if (select(udp_recv_socket + 1, &recv_set, NULL, NULL, &tv) <= 0) {
                // timeout or error
                continue;
            }
            // ready data: receive and immediately reply
            pthread_mutex_lock(&theMutex);
            {
                int rc = do_recv(udp_recv_socket, the_IsyncRegisters, THE_SYNC_UDP_SIZE);
                if (rc != THE_SYNC_UDP_SIZE) {
                    // FIXME: error recovery
                }
                // PLC INTERNAL VARIABLES MANAGEMENT
                PLCsync();
                int sn = do_sendto(udp_send_socket, the_QsyncRegisters, THE_SYNC_UDP_SIZE,
                        &DestinationAddress);
                if (sn != THE_SYNC_UDP_SIZE) {
                    // FIXME: error recovery
                }
            }
            pthread_mutex_unlock(&theMutex);
        } else {
            usleep(THE_UDP_TIMEOUT_ms * 1000);
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
