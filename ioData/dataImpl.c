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
#include <semaphore.h>

#include "stdInc.h"

#if defined(RTS_CFG_IODAT)

#include "mectCfgUtil.h"
#include "dataMain.h"
#include "iolDef.h"
#include "fcDef.h"
#if defined(RTS_CFG_MECT_RETAIN)
#include <sys/mman.h>
#include "mectRetentive.h"
#endif

#include "libHW119.h"
#include "libModbus.h"
#include "CANopen.h"

#define REVISION_HI  7
#define REVISION_LO  1

#if DEBUG
#undef VERBOSE_DEBUG
#endif
/* ----  Target Specific Includes:	 ------------------------------------------ */

#define MAX_SERVERS  5 // 3 RTU_SRV + 1 TCP_SRV + 1 TCPRTU_SRV (PLC in dataMain->dataNotifySet/Get)
#define MAX_DEVICES 32 // 1 PLC + 3 RTU + n TCP + m TCPRTU + 2 CANOPEN + 1 RTUSRV + 1 TCPSRV + 1 TCPRTUSRV
#define MAX_NODES   64 //

#define MAX_WRITES  64 // 16
#define MAX_READS   64
#define MAX_PRIORITY 3

#define THE_CONFIG_DELAY_ms     10
#define THE_ENGINE_DELAY_ms     10
#define THE_SERVER_DELAY_ms     1000
#define THE_CONNECTION_DELAY_ms 1000
#define THE_DEVICE_BLACKLIST_ms 4000
#define THE_DEVICE_SILENCE_ms   20000 // tpac boot time

// -------- MANAGE THREADS: DATA & SYNCRO
#define THE_UDP_PERIOD_ms	100
#define THE_UDP_TIMEOUT_ms	500
#define THE_UDP_SEND_ADDR   "127.0.0.1"

// --------
#define DimCrossTable   5472
#define DimCrossTable_2 22004
#define DimCrossTable_3 44004
#define DimCrossTable_4 (DimCrossTable_3 + 2 * DimCrossTable + 2 + 2)
#define DimAlarmsCT     1152
#define LAST_RETENTIVE  192

static struct system_ini system_ini;
static int system_ini_ok;

// -------- DATA MANAGE FROM HMI ---------------------------------------------
#define REG_DATA_NUMBER     7680 // 1+5472+(5500-5473)+5473/2+...
#define THE_DATA_SIZE       (REG_DATA_NUMBER * sizeof(u_int32_t)) // 30720 = 0x00007800 30 kiB
#define THE_DATA_UDP_SIZE   THE_DATA_SIZE
#define	THE_DATA_RECV_PORT	34903
#define	THE_DATA_SEND_PORT	34902

static u_int32_t the_UdataBuffer[REG_DATA_NUMBER]; // udp recv buffer
static u_int32_t the_IdataRegisters[REG_DATA_NUMBER]; // %I
static u_int32_t the_QdataRegisters[REG_DATA_NUMBER]; // %Q

static u_int8_t *the_QdataStates = (u_int8_t *)(&(the_QdataRegisters[22000]));
// Qdata states
#define DATA_OK            0    // read and ok
#define DATA_ERR           1    // read and failure
#define DATA_RUN           2    // still reading

// -------- SYNC MANAGE FROM HMI ---------------------------------------------
#define REG_SYNC_NUMBER 	6144 // 1+5472+...
#define THE_SYNC_SIZE       (REG_SYNC_NUMBER * sizeof(u_int16_t)) // 12288 = 0x00003000 12 kiB
#define THE_SYNC_UDP_SIZE   11462 // SYNCRO_SIZE_BYTE in qt_library/ATCMutility/common.h
#define	THE_SYNC_RECV_PORT	34905
#define	THE_SYNC_SEND_PORT	34904

static u_int16_t the_UsyncBuffer[REG_SYNC_NUMBER]; // udp recv buffer
static u_int16_t the_IsyncRegisters[REG_SYNC_NUMBER]; // %I Array delle CODE in lettura
static u_int16_t the_QsyncRegisters[REG_SYNC_NUMBER]; // %Q Array delle CODE in scrittura

#define QueueOperMask       0xE000  // BIT 15 WR EN, BIT 14 RD EN, BIT 13 ??
#define QueueAddrMask       0x1FFF  // BIT 12..0 CROSSTABLE ADDRESS
// Isync Operations
#define NOP                 0x0000
#define READ                0x4000
#define WRITE_SINGLE        0x8000
#define WRITE_MULTIPLE      0xA000
#define WRITE_RIC_MULTIPLE  0xE000
#define WRITE_RIC_SINGLE    0xC000
#define WRITE_PREPARE       0x2000
// Qsync States
#define QUEUE_EMPTY         0
#define QUEUE_BUSY_WRITE    1
#define QUEUE_BUSY_READ     2

#define RichiestaScrittura          the_IsyncRegisters[5500] //interrupt di scrittura
#define PLCRevision01               the_QsyncRegisters[5501]
#define PLCRevision02               the_QsyncRegisters[5502]
#define Reset_RTU                   the_IsyncRegisters[5503]
#define Reset_TCP                   the_IsyncRegisters[5504]
#define Reset_TCPRTU                the_IsyncRegisters[5505]

#define CounterRTU(i)               the_QsyncRegisters[5506 + i] // 1+64
#define CounterTCP(i)               the_QsyncRegisters[5571 + i] // 1+64
#define CounterTCPRTU(i)            the_QsyncRegisters[5636 + i] // 1+64

#define RTUBlackList_ERROR_WORD     the_QsyncRegisters[5701]
#define RTUComm_ERROR_WORD          the_QsyncRegisters[5702]
#define TCPBlackList_ERROR_WORD     the_QsyncRegisters[5703]
#define TCPComm_ERROR_WORD          the_QsyncRegisters[5704]
#define TCPRTUBlackList_ERROR_WORD  the_QsyncRegisters[5705]
#define TCPRTUComm_ERROR_WORD       the_QsyncRegisters[5706]

// segnala se almeno un errore e' stato evidenziato dall'ultimo reset:
#define ERROR_FLAG                  the_QsyncRegisters[5707]
#define ERROR_FLAG_RTU          0x00000001 // bit00: errore RTU
#define ERROR_FLAG_TCP          0x00000002 // bit01: errore TCP
#define ERROR_FLAG_TCPRTU       0x00000004 // bit02: errore TCPRTU,
#define ERROR_FLAG_COMMPAR      0x00000008 // bit03: errore commpar,
#define ERROR_FLAG_ALARMS       0x00000010 // bit04: errore CTallarmi,
#define ERROR_FLAG_CROSSTAB     0x00000020 // bit05: errore CTvariabili o  errore tipo non riconosciuto su CTvariabili
#define ERROR_FLAG_CONF_END     0x00000040 // bit06: segnalazione che PLC ha terminalo l'eloaborazine delle CT
#define ERROR_FLAG_RTU_ON       0x00000080 // bit07: RTU_ON
#define ERROR_FLAG_TCP_ON       0x00000100 // bit08: TCP_ON
#define ERROR_FLAG_TCPRTU_ON    0x00000200 // bit09: TCPRTU_ON
// -------- new protocols
#define ERROR_FLAG_CANOPEN      0x00000400 // bit10: errore CANOPEN
#define ERROR_FLAG_MECT         0x00000800 // bit11: errore MECT
#define ERROR_FLAG_RTUSRV       0x00001000 // bit12: errore RTUSRV
#define ERROR_FLAG_TCPSRV       0x00002000 // bit13: errore TCPSRV
#define ERROR_FLAG_TCPRTUSRV    0x00004000 // bit14: errore TCPRTUSRV
#define ERROR_FLAG_CANOPEN_ON   0x00008000 // bit15: CANOPEN_ON
#define ERROR_FLAG_MECT_ON      0x00010000 // bit16: MECT_ON
#define ERROR_FLAG_RTUSRV_ON    0x00020000 // bit17: RTUSRV_ON
#define ERROR_FLAG_TCPSRV_ON    0x00040000 // bit18: TCPSRV_ON
#define ERROR_FLAG_TCPRTUSRV_ON 0x00080000 // bit19: TCPRTUSRV_ON

//
#ifndef __GLIBC_HAVE_LONG_LONG
#error "we need 64bit emulation from gcc"
#endif
#define BlackListRTU       (*(u_int64_t *)&the_QsyncRegisters[5708])
#define CommErrRTU         (*(u_int64_t *)&the_QsyncRegisters[5712])
#define BlackListTCP       (*(u_int64_t *)&the_QsyncRegisters[5716])
#define CommErrTCP         (*(u_int64_t *)&the_QsyncRegisters[5720])
#define BlackListTCPRTU    (*(u_int64_t *)&the_QsyncRegisters[5724])
#define CommErrTCPRTU      (*(u_int64_t *)&the_QsyncRegisters[5728])

// -------- ALL SERVERS (RTU_SRV, TCP_SRV, TCPRTU_SRV) ------------
#define REG_SRV_NUMBER      4096
#define THE_SRV_SIZE        (REG_SRV_NUMBER * sizeof(u_int16_t)) // 0x00002000 8kB
#define	THE_SRV_MAX_CLIENTS	10

//#define RTS_CFG_DEBUG_OUTPUT
enum TableType {Crosstable_csv = 0, Alarms_csv};
enum FieldbusType {PLC = 0, RTU, TCP, TCPRTU, CANOPEN, MECT, RTU_SRV, TCP_SRV, TCPRTU_SRV};
enum UpdateType { Htype = 0, Ptype, Stype, Ftype};
enum EventAlarm { Event = 0, Alarm};
static const char *fieldbusName[] = {"PLC", "RTU", "TCP", "TCPRTU", "CANOPEN", "MECT", "RTU_SRV", "TCP_SRV", "TCPRTU_SRV" };

enum threadStatus  {NOT_STARTED = 0, RUNNING, EXITING};
enum DeviceStatus  {ZERO = 0, NOT_CONNECTED, CONNECTED, CONNECTED_WITH_ERRORS, DEVICE_BLACKLIST, NO_HOPE};
#ifdef VERBOSE_DEBUG
static const char *statusName[] = {"ZERO", "NOT_CONNECTED", "CONNECTED", "CONNECTED_WITH_ERRORS", "DEVICE_BLACKLIST", "NO_HOPE" };
#endif
enum NodeStatus    {NO_NODE = 0, NODE_OK, TIMEOUT, BLACKLIST};
enum fieldbusError {NoError = 0, CommError, TimeoutError};
#undef WORD_BIT
enum varTypes {BIT = 0, BYTE_BIT, WORD_BIT, DWORD_BIT,
               UINT16, UINT16BA,
               INT16, INT16BA,
               REAL, REALDCBA, REALCDAB, REALBADC,
               UDINT, UDINTDCBA, UDINTCDAB, UDINTBADC,
               DINT, DINTDCBA, DINTCDAB, DINTBADC,
               UNKNOWN};

static pthread_mutex_t theCrosstableClientMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t theAlarmsEventsCondvar;
static pthread_mutex_t theAlarmsEventsMutex;

#define MAX_IPADDR_LEN      17 // 123.567.901.345.
#define MAX_NUMBER_LEN      12 // -2147483648. -32768.
#define MAX_IDNAME_LEN      17 // abcdefghijklmno.
#define MAX_VARTYPE_LEN      9 // UDINTABCD.
#define MAX_PROTOCOL_LEN     9 // TCPRTUSRV.
#define MAX_DEVICE_LEN      13 // /dev/ttyUSB0.
#define MAX_THREADNAME_LEN  42 // "srv(64)TCPRTUSRV_123.567.901.345_65535"

static struct ServerStruct {
    // for serverThread
    enum FieldbusType protocol;
    char IPaddress[MAX_IPADDR_LEN];
    u_int16_t port;
    //
    union ServerData {
        struct {
            u_int16_t port;
            u_int32_t baudrate;
            char parity;
            u_int16_t databits;
            u_int16_t stopbits;
        } serial;
        struct {
            char IPaddr[MAX_IPADDR_LEN];
            u_int16_t port;
        } tcp_ip;
    } u;
    u_int16_t NodeId;
    //
    char name[MAX_THREADNAME_LEN]; // "(64)TCPRTUSRV_123.567.901.345_65535"
    pthread_t thread_id;
    enum threadStatus thread_status;
    pthread_mutex_t mutex;
    modbus_t * ctx;
    modbus_mapping_t *mb_mapping;
    u_int8_t *can_buffer;
} theServers[MAX_SERVERS];
static u_int16_t theServersNumber = 0;

#define MaxLocalQueue 64
static struct ClientStruct {
    // for clientThread
    enum FieldbusType protocol;
    char IPaddress[MAX_IPADDR_LEN];
    u_int16_t port;
    //
    union ClientData {
        // no plc client
        struct {
            u_int16_t port;
            u_int32_t baudrate;
            char parity;
            u_int16_t databits;
            u_int16_t stopbits;
        } serial; // RTU, MECT
        struct {
            char IPaddr[MAX_IPADDR_LEN];
            u_int16_t port;
        } tcp_ip; // TCP, TCPRTU
        struct {
            u_int16_t bus;
            u_int32_t baudrate;
        } can;
    } u;
    int16_t silence_ms;
    u_int16_t timeout_ms;
    //
    char name[MAX_THREADNAME_LEN]; // "(64)TCPRTUSRV_123.567.901.345_65535"
    enum DeviceStatus status;
    pthread_t thread_id;
    enum threadStatus thread_status;
    u_int32_t current_time_ms;
    u_int32_t elapsed_time_ms;
    sem_t newOperations;
    u_int16_t writeOperations;
    u_int16_t writeResetIndex;
    u_int16_t server; // for RTUSRV, TCPSRV, TCPRUSRV
    modbus_t * modbus_ctx; // for RTU, TCP, TCPRTU
    // local queue
    struct PLCwriteRequestStruct {
        u_int16_t Addr;
        u_int16_t Number;
        u_int32_t Values[MAX_WRITES];
    } PLCwriteRequests[MaxLocalQueue];
    u_int16_t PLCwriteRequestNumber;
    u_int16_t PLCwriteRequestGet;
    u_int16_t PLCwriteRequestPut;
} theDevices[MAX_DEVICES];
static u_int16_t theDevicesNumber = 0;

struct NodeStruct {
    u_int16_t device;
    u_int16_t NodeID;
    //
    enum NodeStatus status;
    int16_t RetryCounter; // TIMEOUT
    int16_t JumpRead; // BLACKLIST
} theNodes[MAX_NODES];
static u_int16_t theNodesNumber = 0;

struct  CrossTableRecord {
    int16_t Enable;
    enum UpdateType Plc;
    char Tag[MAX_IDNAME_LEN];
    enum varTypes Types;
    u_int16_t Decimal;
    enum FieldbusType Protocol;
    char IPAddress[MAX_IPADDR_LEN];
    u_int16_t Port;
    u_int8_t NodeId;
    u_int16_t Offset;
    u_int16_t Block;
    u_int16_t BlockBase;
    int16_t BlockSize;
    int Output;
    int16_t Counter;
    u_int32_t OldVal;
    u_int16_t Error;
    int usedInAlarmsEvents;
    //
    u_int16_t device;
    u_int16_t node;
};
static struct CrossTableRecord CrossTable[1 + DimCrossTable];	 // campi sono riempiti a partire dall'indice 1

#define FRONTE_SALITA   1
#define FRONTE_DISCESA  0
struct  Alarms {
    enum EventAlarm ALType;
    char ALTag[MAX_IDNAME_LEN];
    char ALSource[MAX_IDNAME_LEN];
    char ALCompareVar[MAX_IDNAME_LEN];
    u_int16_t TagAddr;
    u_int16_t SourceAddr;
    u_int16_t CompareAddr;
    u_int32_t ALCompareVal;
    u_int16_t ALOperator;
    u_int16_t ALFilterTime;
    u_int16_t ALFilterCount;
    u_int16_t ALError;
};
static struct Alarms ALCrossTable[1 + DimAlarmsCT]; // campi sono riempiti a partire dall'indice 1

/* ----  Global Variables:	 -------------------------------------------------- */

#if defined(RTS_CFG_MECT_RETAIN)
static u_int32_t *retentive = NULL;
#endif
static IEC_BOOL engineInitialized	= FALSE;
static IEC_BOOL engineRunning	= FALSE;
static IEC_BOOL engineExiting	= FALSE;

static pthread_t theEngineThread_id = -1;
static pthread_t theDataSyncThread_id = -1;
static enum threadStatus theEngineThreadStatus = NOT_STARTED;
static enum threadStatus theDataSyncThreadStatus = NOT_STARTED;

static STaskInfoVMM *pVMM = NULL;
static u_int32_t ErrorsState;
//  ErrorsState: Variabile di errore
//  bit 0:	fallita lettura crosstable
//  bit 1:	fallita lettura record crosstable
//  bit 2:	fallita lettura field crosstable
//  bit 3:	fallita chiusura crosstable
//  bit 4:	Apertura Modbus RTU fallita
//  bit 5:	Apertura Modbus TCP fallita
//  bit 6:	Apertura Modbus TCPRTU fallita
//  bit 7:	Timeout RTU
//  bit 8:	Timeout TCP
//  bit 9:	Timeout TCPRTU

static int CrossTableState;
static int ALCrossTableState;
static int CommEnabled;

/* ----  Local Functions:	--------------------------------------------------- */

static void *engineThread(void *statusAdr);
static void *datasyncThread(void *statusAdr);
static void *serverThread(void *statusAdr);
static void *clientThread(void *statusAdr);

static int ReadFields(int16_t Index);
static int ReadAlarmsFields(int16_t Index);
static int LoadXTable(enum TableType CTType);
static void AlarmMngr(void);
static void PLCsync(void);

static void zeroNodeVariables(u_int32_t node);
static void zeroDeviceVariables(u_int32_t d);

/* ----  Implementations:	--------------------------------------------------- */

static inline void writeQdataRegisters(u_int16_t addr, u_int32_t value)
{
    the_QdataRegisters[addr] = value;
    if (CrossTable[addr].usedInAlarmsEvents) {
        pthread_cond_signal(&theAlarmsEventsCondvar);
    }
#if defined(RTS_CFG_MECT_RETAIN)
    if (retentive && addr <= LAST_RETENTIVE) {
        retentive[addr -1] = value;
    }
#endif
}

static inline unsigned get_byte_bit(u_int8_t data, unsigned n)
{
    // bits 1..8
    if (data & (1 << (n - 1))) {
        return 1;
    } else {
        return 0;
    }
}

static inline unsigned get_word_bit(u_int16_t data, unsigned n)
{
    // bits 1..16
    if (data & (1 << (n - 1))) {
        return 1;
    } else {
        return 0;
    }
}

static inline unsigned get_dword_bit(u_int32_t data, unsigned n)
{
    // bits 1..32
    if (data & (1 << (n - 1))) {
        return 1;
    } else {
        return 0;
    }
}

static inline void set_byte_bit(u_int8_t *data, unsigned n, unsigned value)
{
    // bits 1..8
    if (value) {
        *data |= (1 << (n - 1));
    } else {
        *data &= ~(1 << (n - 1));
    }
}

static inline void set_word_bit(u_int16_t *data, unsigned n, unsigned value)
{
    // bits 1..16
    if (value) {
        *data |= (1 << (n - 1));
    } else {
        *data &= ~(1 << (n - 1));
    }
}

static inline void set_dword_bit(u_int32_t *data, unsigned n, unsigned value)
{
    // bits 1..32
    if (value) {
        *data |= (1 << (n - 1));
    } else {
        *data &= ~(1 << (n - 1));
    }
}

static int ReadFields(int16_t Index)
{
    int ERR = 0;
    IEC_STRMAX Field = { 0, VMM_MAX_IEC_STRLEN, ""};
    HW119_GET_CROSS_TABLE_FIELD param = {(IEC_STRING *)&Field, 0};

    // Enable {0,1,2,3}
    Field.MaxLen = MAX_NUMBER_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        CrossTable[Index].Enable = atoi(Field.Contents);
    } else {
        ERR = TRUE;
    }

    // Plc {H,P,S,F}
    Field.MaxLen = 1 + 1;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        switch (Field.Contents[0]) {
        case 'H':
            CrossTable[Index].Plc = Htype;
            break;
        case 'P':
            CrossTable[Index].Plc = Ptype;
            break;
        case 'S':
            CrossTable[Index].Plc = Stype;
            break;
        case 'F':
            CrossTable[Index].Plc = Ftype;
            break;
        default:
            ERR = TRUE;
        }
    } else {
        ERR = TRUE;
    }

    // Tag {identifier}
    Field.MaxLen = MAX_IDNAME_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        strncpy(CrossTable[Index].Tag, param.field->Contents, MAX_IDNAME_LEN);
    } else {
        ERR = TRUE;
    }

    // Types {UINT, UDINT, DINT, FDCBA, ...}
    Field.MaxLen = MAX_VARTYPE_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        if (strncmp(Field.Contents, "BIT", Field.CurLen) == 0) {
            CrossTable[Index].Types = BIT;
        } else if (strncmp(Field.Contents, "BYTE_BIT", Field.CurLen) == 0) {
            CrossTable[Index].Types = BYTE_BIT;
        } else if (strncmp(Field.Contents, "WORD_BIT", Field.CurLen) == 0) {
            CrossTable[Index].Types = WORD_BIT;
        } else if (strncmp(Field.Contents, "DWORD_BIT", Field.CurLen) == 0) {
            CrossTable[Index].Types = DWORD_BIT;
        } else if (strncmp(Field.Contents, "UINT", Field.CurLen) == 0) {
            CrossTable[Index].Types = UINT16;
        } else if (strncmp(Field.Contents, "UINTBA", Field.CurLen) == 0) {
            CrossTable[Index].Types = UINT16BA;
        } else if (strncmp(Field.Contents, "INT", Field.CurLen) == 0) {
            CrossTable[Index].Types = INT16;
        } else if (strncmp(Field.Contents, "INTBA", Field.CurLen) == 0) {
            CrossTable[Index].Types = INT16BA;
        } else if (strncmp(Field.Contents, "UDINT", Field.CurLen) == 0) {
            CrossTable[Index].Types = UDINT;
        } else if (strncmp(Field.Contents, "UDINTDCBA", Field.CurLen) == 0) {
            CrossTable[Index].Types = UDINTDCBA;
        } else if (strncmp(Field.Contents, "UDINTCDAB", Field.CurLen) == 0) {
            CrossTable[Index].Types = UDINTCDAB;
        } else if (strncmp(Field.Contents, "UDINTBADC", Field.CurLen) == 0) {
            CrossTable[Index].Types = UDINTBADC;
        } else if (strncmp(Field.Contents, "DINT", Field.CurLen) == 0) {
            CrossTable[Index].Types = DINT;
        } else if (strncmp(Field.Contents, "DINTDCBA", Field.CurLen) == 0) {
            CrossTable[Index].Types = DINTDCBA;
        } else if (strncmp(Field.Contents, "DINTCDAB", Field.CurLen) == 0) {
            CrossTable[Index].Types = DINTCDAB;
        } else if (strncmp(Field.Contents, "DINTBADC", Field.CurLen) == 0) {
            CrossTable[Index].Types = DINTBADC;
        } else if (strncmp(Field.Contents, "REAL", Field.CurLen) == 0) {
            CrossTable[Index].Types = REAL;
        } else if (strncmp(Field.Contents, "REALDCBA", Field.CurLen) == 0) {
            CrossTable[Index].Types = REALDCBA;
        } else if (strncmp(Field.Contents, "REALCDAB", Field.CurLen) == 0) {
            CrossTable[Index].Types = REALCDAB;
        } else if (strncmp(Field.Contents, "REALBADC", Field.CurLen) == 0) {
            CrossTable[Index].Types = REALBADC;

        } else if (strncmp(Field.Contents, "UINTAB", Field.CurLen) == 0) {
            CrossTable[Index].Types = UINT16; // backward compatibility
        } else if (strncmp(Field.Contents, "INTAB", Field.CurLen) == 0) {
            CrossTable[Index].Types = INT16; // backward compatibility
        } else if (strncmp(Field.Contents, "UDINTABCD", Field.CurLen) == 0) {
            CrossTable[Index].Types = UDINT; // backward compatibility
        } else if (strncmp(Field.Contents, "DINTABCD", Field.CurLen) == 0) {
            CrossTable[Index].Types = DINT; // backward compatibility
        } else if (strncmp(Field.Contents, "FDCBA", Field.CurLen) == 0) {
            CrossTable[Index].Types = REALDCBA; // backward compatibility
        } else if (strncmp(Field.Contents, "FCDAB", Field.CurLen) == 0) {
            CrossTable[Index].Types = REALCDAB; // backward compatibility
        } else if (strncmp(Field.Contents, "FABCD", Field.CurLen) == 0) {
            CrossTable[Index].Types = REAL; // backward compatibility
        } else if (strncmp(Field.Contents, "FBADC", Field.CurLen) == 0) {
            CrossTable[Index].Types = REALBADC; // backward compatibility

        } else {
            if (CrossTable[Index].Enable > 0) {
                CrossTable[Index].Types = UNKNOWN;
                ERROR_FLAG |= ERROR_FLAG_CROSSTAB;
            }
        }
    } else {
        ERR = TRUE;
    }

    // Decimal {0, 1, 2, 3, 4, ...}
    Field.MaxLen = MAX_NUMBER_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        CrossTable[Index].Decimal = atoi(param.field->Contents);
    } else {
        ERR = TRUE;
    }

    // Protocol {"", RTU, TCP, TCPRTU}
    Field.MaxLen = MAX_PROTOCOL_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        if (strncmp(Field.Contents, "PLC", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = PLC;
        } else if (strncmp(Field.Contents, "RTU", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = RTU;
        } else if (strncmp(Field.Contents, "TCP", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = TCP;
        } else if (strncmp(Field.Contents, "TCPRTU", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = TCPRTU;
        } else if (strncmp(Field.Contents, "CANOPEN", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = CANOPEN;
        } else if (strncmp(Field.Contents, "MECT", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = MECT;
        } else if (strncmp(Field.Contents, "RTU_SRV", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = RTU_SRV;
        } else if (strncmp(Field.Contents, "TCP_SRV", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = TCP_SRV;
        } else if (strncmp(Field.Contents, "TCPRTU_SRV", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = TCPRTU_SRV;
        } else {
            CrossTable[Index].Protocol = PLC;
            // FIXME: ERR ?
        }
    } else {
        ERR = TRUE;
    }

    // IPAddress {identifier}
    Field.MaxLen = MAX_IPADDR_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        strncpy(CrossTable[Index].IPAddress, param.field->Contents, MAX_IPADDR_LEN);
    } else {
        ERR = TRUE;
    }

    // Port {number}
    Field.MaxLen = MAX_NUMBER_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        CrossTable[Index].Port = atoi(param.field->Contents);
    } else {
        ERR = TRUE;
    }

    // NodeId {number}
    Field.MaxLen = MAX_NUMBER_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        CrossTable[Index].NodeId = atoi(param.field->Contents);
    } else {
        ERR = TRUE;
    }

    // Address {number}
    Field.MaxLen = MAX_NUMBER_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        CrossTable[Index].Offset = atoi(param.field->Contents);
    } else {
        ERR = TRUE;
    }

    // Block {number}
    Field.MaxLen = MAX_NUMBER_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        CrossTable[Index].Block = atoi(param.field->Contents);
    } else {
        ERR = TRUE;
    }

    // NReg {number}
    Field.MaxLen = MAX_NUMBER_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        CrossTable[Index].BlockSize = atoi(param.field->Contents);
    } else {
        ERR = TRUE;
    }

    // Handle ([RO]|[RD]|[RW]){text}
    Field.MaxLen = MAX_LINE_SIZE;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        if (strncmp(param.field->Contents, "[RW]", 4) == 0) {
            CrossTable[Index].Output = TRUE;
        } else {
            CrossTable[Index].Output = FALSE;
        }
    } else {
        ERR = TRUE;
    }

    return ERR;
}

// fb_HW119_ReadAlarmsFields.st
static int ReadAlarmsFields(int16_t Index)
{
    int ERR = FALSE;
    IEC_STRMAX Field = { 0, VMM_MAX_IEC_STRLEN, ""};
    HW119_GET_CROSS_TABLE_FIELD param = {(IEC_STRING *)&Field, 0};

    // ALType {0,1}
    Field.MaxLen = 1 + 1;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        switch (atoi(Field.Contents)) {
        case 0: ALCrossTable[Index].ALType = Event; break;
        case 1: ALCrossTable[Index].ALType = Alarm; break;
        default:
            ERR = TRUE;
        }
    } else {
        ERR = TRUE;
    }

    // ALTag {identifier}
    Field.MaxLen = MAX_IDNAME_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        strncpy(ALCrossTable[Index].ALTag, param.field->Contents, MAX_IDNAME_LEN);
    } else {
        ERR = TRUE;
    }

    // ALSource {identifier}
    Field.MaxLen = MAX_IDNAME_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        strncpy(ALCrossTable[Index].ALSource, param.field->Contents, MAX_IDNAME_LEN);
    } else {
        ERR = TRUE;
    }

    // ALCompareVar {identifier}
    Field.MaxLen = MAX_IDNAME_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        strncpy(ALCrossTable[Index].ALCompareVar, param.field->Contents, MAX_IDNAME_LEN);
    } else {
        ERR = TRUE;
    }

    // ALCompareVal {number}
    Field.MaxLen = MAX_NUMBER_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        ALCrossTable[Index].ALCompareVal = atoi(Field.Contents);
    } else {
        ERR = TRUE;
    }

    // ALOperator {number}
    Field.MaxLen = MAX_NUMBER_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        ALCrossTable[Index].ALOperator = atoi(Field.Contents);
    } else {
        ERR = TRUE;
    }

    // ALFilterTime {number}
    Field.MaxLen = MAX_NUMBER_LEN;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        ALCrossTable[Index].ALFilterTime = atoi(Field.Contents);
        ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterTime;
    } else {
        ERR = TRUE;
    }

    return ERR;
}

// fb_HW119_Init.st
// fb_HW119_LoadCrossTab.st
static int LoadXTable(enum TableType CTType)
{
    char *CTFile;
    int32_t CTDimension;
    int32_t addr;
    int ERR = FALSE;
    IEC_STRMAX filename;
    HW119_OPEN_CROSS_TABLE_PARAM open_param = {(IEC_STRING *)&filename, 0 };
    HW119_READ_CROSS_TABLE_RECORD_PARAM read_param;
    HW119_CLOSE_CROSS_TABLE_PARAM close_param;

    // init table
    ErrorsState = ErrorsState & 0xF0; // reset flag di errore
    switch (CTType) {
    case Crosstable_csv:
        CTFile = "/local/etc/sysconfig/Crosstable.csv";
        CTDimension = DimCrossTable;
        for (addr = 1; addr <= DimCrossTable; ++addr) {
            CrossTable[addr].Enable = 0;
            CrossTable[addr].Plc = FALSE;
            CrossTable[addr].Tag[0] = UNKNOWN;
            CrossTable[addr].Types = 0;
            CrossTable[addr].Decimal = 0;
            CrossTable[addr].Protocol = PLC;
            CrossTable[addr].IPAddress[0] = '\0';
            CrossTable[addr].Port = 0;
            CrossTable[addr].NodeId = 0;
            CrossTable[addr].Offset = 0;
            CrossTable[addr].Block = 0;
            CrossTable[addr].BlockSize = 0;
            CrossTable[addr].Output = FALSE;
            CrossTable[addr].OldVal = 0;
            CrossTable[addr].Error = 1;
            CrossTable[addr].device = 0xffff;
            CrossTable[addr].node = 0xffff;
            the_QsyncRegisters[addr] = QUEUE_EMPTY;
            the_QdataStates[addr] = DATA_OK;
        }
        CrossTableState = TRUE;
        CommEnabled = FALSE;
        break;
    case Alarms_csv:
        CTFile = "/local/etc/sysconfig/Alarms.csv";
        CTDimension = DimAlarmsCT;
        for (addr = 0; addr <= DimAlarmsCT; ++addr) {
            ALCrossTable[addr].ALType = FALSE;
            ALCrossTable[addr].ALTag[0] = '\0';
            ALCrossTable[addr].ALSource[0] = '\0';
            ALCrossTable[addr].ALCompareVar[0] = '\0';
            ALCrossTable[addr].TagAddr = 0;
            ALCrossTable[addr].SourceAddr = 0;
            ALCrossTable[addr].CompareAddr = 0;
            ALCrossTable[addr].ALCompareVal = 0;
            ALCrossTable[addr].ALOperator = 0;
            ALCrossTable[addr].ALFilterTime = 0;
        }
        ALCrossTableState = TRUE;
        break;
    default:
        CTFile = "(unknown)";
        CTDimension = -1;
    }

    // open file
    fprintf(stderr, "loading '%s' ...", CTFile);
    filename.MaxLen = VMM_MAX_IEC_STRLEN; // VMM_MAX_PATH is higher than 256
    filename.CurLen = strnlen(CTFile, VMM_MAX_IEC_STRLEN);
    strncpy(filename.Contents, CTFile, VMM_MAX_IEC_STRLEN);
    hw119_open_cross_table(NULL, NULL, (unsigned char *)&open_param);
    if (open_param.ret_value)  {
        ErrorsState = ErrorsState | 0x01;
        ERR = TRUE;
        goto exit_function;
    }

    // read loop
    for (addr = 1; addr <= CTDimension; ++ addr) {

        switch (CTType) {
        case Crosstable_csv:
            read_param.error = FALSE;
            break;
        case Alarms_csv:
            read_param.error = TRUE;
            break;
        default:
            ;
        }
        hw119_read_cross_table_record(NULL, NULL, (unsigned char *)&read_param);
        if (read_param.ret_value) {
            switch (CTType) {
            case Crosstable_csv:
                CrossTable[addr].Error = 100;
                ErrorsState |= 0x02;
                break;
            case Alarms_csv:
                ALCrossTable[addr].ALError = 100;
                ErrorsState |= 0x04;
                break;
            default:
                ;
            }
            // no ERR = TRUE;
            continue;
        }
        switch (CTType) {
        case Crosstable_csv:
            if (ReadFields(addr)) {
                CrossTable[addr].Error = 100;
                ErrorsState |= 0x04;
            }
            break;
        case Alarms_csv:
            if (ReadAlarmsFields(addr)) {
                ALCrossTable[addr].ALError = 100;
                ErrorsState |= 0x04;
            }
            break;
        default:
            ;
        }
    }

    // close file
exit_function:
    hw119_close_cross_table(NULL, NULL, (unsigned char *)&close_param);
    if (close_param.ret_value) {
        ErrorsState |= 0x80;
        ERR = TRUE;
    }
    fprintf(stderr, " %s\n", (ERR) ? "ERROR" : "OK");
    return ERR;
}

static inline void setEventAlarm(int i)
{
    writeQdataRegisters(ALCrossTable[i].TagAddr, 1);
    switch (ALCrossTable[i].ALType ) {
    case Event:
        vmmSetEvent(pVMM, EVT_RESERVED_10);
        break;
    case Alarm:
        vmmSetEvent(pVMM, EVT_RESERVED_11);
        break;
    default:
        ;
    }
}

static inline void clrEventAlarm(int i)
{
    writeQdataRegisters(ALCrossTable[i].TagAddr, 0);
}

static inline void checkThis(int i, int condition)
{
    if (condition) {
        if (ALCrossTable[i].ALFilterCount == 0) {
            setEventAlarm(i);
        } else {
            ALCrossTable[i].ALFilterCount = ALCrossTable[i].ALFilterCount - 1;
        }
    } else {
        clrEventAlarm(i);
        ALCrossTable[i].ALFilterCount = ALCrossTable[i].ALFilterTime;
    }
}

static void AlarmMngr(void)
{
    u_int32_t i, oper, bit;
    u_int16_t SourceAddr;
    u_int32_t CompareVal;
    u_int32_t value, old_value;

    // already in pthread_mutex_lock(&theCrosstableClientMutex)
    for (i = 1; i < DimAlarmsCT; ++i) {

        if (ALCrossTable[i].TagAddr == 0) {
            // last alarm
            break;
        }
        SourceAddr = ALCrossTable[i].SourceAddr;
        if (CrossTable[SourceAddr].Error > 0 && CrossTable[SourceAddr].Protocol != PLC) {
            // unreliable values
            continue;
        }
        oper = ALCrossTable[i].ALOperator & 0xFF00;
        bit = ALCrossTable[i].ALOperator & 0x00FF;
        if (ALCrossTable[i].CompareAddr == SourceAddr) {
            // checking rising and falling edges (only bit testing)
            if (oper == 0 && 1 <= bit && bit <= 32) {
                value = get_dword_bit(the_QdataRegisters[SourceAddr], bit);
                old_value = get_dword_bit(CrossTable[SourceAddr].OldVal, bit);
                switch (ALCrossTable[i].ALCompareVal) {
                case FRONTE_SALITA:
                    checkThis(i, old_value == 0 && value == 1);
                    break;
                case FRONTE_DISCESA:
                    checkThis(i, old_value == 1 && value == 0);
                    break;
                default:
                    ; // FIXME: assert
                }
            } else {
                ; // FIXME: assert
            }
        } else {
            // checking against either fixed or variable values
            if (ALCrossTable[i].CompareAddr == 0) {
                CompareVal = ALCrossTable[i].ALCompareVal;
            } else {
                CompareVal = the_QdataRegisters[ALCrossTable[i].CompareAddr];
            }
            switch (oper) {
            case 0x100: //  >
                checkThis(i, the_QdataRegisters[SourceAddr] > CompareVal);
                break;
            case 0x200: //  >=
                checkThis(i, the_QdataRegisters[SourceAddr] >= CompareVal);
                break;
            case 0x300: //  <
                checkThis(i, the_QdataRegisters[SourceAddr] < CompareVal);
                break;
            case 0x400: //  <=
                checkThis(i, the_QdataRegisters[SourceAddr] <= CompareVal);
                break;
            case 0x500: //  ==
                checkThis(i, the_QdataRegisters[SourceAddr] == CompareVal);
                break;
            case 0x600: //  !=
                checkThis(i, the_QdataRegisters[SourceAddr] != CompareVal);
                break;
            case 0x000: // bit test
                value = get_dword_bit(the_QdataRegisters[SourceAddr], bit);
                checkThis(i, value == CompareVal);
                break;
            default:
                ; // FIXME: assert
            }
        }
    }
    // save current bit values ("future" old values)
    for (i = 1; i < DimAlarmsCT; ++i) {

        if (ALCrossTable[i].TagAddr == 0) {
            // last alarm
            break;
        }
        SourceAddr = ALCrossTable[i].SourceAddr;
        if (CrossTable[SourceAddr].Error > 0 && CrossTable[SourceAddr].Protocol != PLC) {
            // unreliable values
            continue;
        }
        oper = ALCrossTable[i].ALOperator & 0xFF00;
        bit = ALCrossTable[i].ALOperator & 0x00FF;
        if (ALCrossTable[i].CompareAddr == SourceAddr) {
            // checking rising and falling edges (only bit testing)
            if (oper == 0 && 1 <= bit && bit <= 32) {
                value = get_dword_bit(the_QdataRegisters[SourceAddr], bit);
                set_dword_bit(&CrossTable[SourceAddr].OldVal, bit, value);
            } else {
                ; // FIXME: assert
            }
        }
    }
}

static void PLCsync(void)
{
    u_int16_t indx;
    u_int16_t oper;
    u_int16_t addr;

    // already in pthread_mutex_lock(&theCrosstableClientMutex)
    for (indx = 1; indx <= DimCrossTable; ++indx) {
        oper = the_IsyncRegisters[indx] & QueueOperMask;
        addr = the_IsyncRegisters[indx] & QueueAddrMask;
        if (1 <= addr && addr <= DimCrossTable) {
            switch (oper) {
            case NOP:
                the_QsyncRegisters[indx] = QUEUE_EMPTY;
                // span the whole queue each time a new udp packet arrives
                // if (addr == 0) {
                //     indx = DimCrossTable; // queue tail
                //     break;
                // }
                break;
            case READ:
                if (the_QsyncRegisters[indx] != QUEUE_BUSY_READ) {
                    the_QsyncRegisters[indx] = QUEUE_BUSY_READ;
                    switch (CrossTable[addr].Protocol) {
                    case PLC:
                        // immediate read: no fieldbus
                        // ready "by design" the_QdataRegisters[addr] = the_QdataRegisters[addr];
                        the_QdataStates[addr] = DATA_OK;
                        break;
                    case RTU:
                    case TCP:
                    case TCPRTU:
                    case CANOPEN:
                    case MECT:
                    case RTU_SRV:
                    case TCP_SRV:
                    case TCPRTU_SRV:
                        // consider only "H" variables, because the "P,S,F" are already managed by clientThread
                        if (CrossTable[addr].Plc == Htype && CrossTable[addr].device != 0xffff) {
                            sem_post(&theDevices[CrossTable[addr].device].newOperations);
                        }
                        break;
                    default:
                        ;
                    }
                }
                break;
            case WRITE_SINGLE:
            case WRITE_MULTIPLE:
            case WRITE_RIC_SINGLE:
            case WRITE_RIC_MULTIPLE:
                if (the_QsyncRegisters[indx] != QUEUE_BUSY_WRITE) {
                    the_QsyncRegisters[indx] = QUEUE_BUSY_WRITE;
                    switch (CrossTable[addr].Protocol) {
                    case PLC:
                        // immediate write: no fieldbus
                        writeQdataRegisters(addr, the_IdataRegisters[addr]);
                        the_QdataStates[addr] = DATA_OK;
                        break;
                    case RTU:
                    case TCP:
                    case TCPRTU:
                    case CANOPEN:
                    case MECT:
                    case RTU_SRV:
                    case TCP_SRV:
                    case TCPRTU_SRV:
                        if (CrossTable[addr].device != 0xffff) {
#ifdef VERBOSE_DEBUG
                            fprintf(stderr, "_________: write(0x%04x) [%u]@%u value=%u\n", oper, addr, indx, the_IdataRegisters[addr]);
#endif
                            theDevices[CrossTable[addr].device].writeOperations += 1;
                            if (oper == WRITE_RIC_SINGLE || oper == WRITE_RIC_MULTIPLE) {
                                theDevices[CrossTable[addr].device].writeResetIndex = 1;
                            }
                            sem_post(&theDevices[CrossTable[addr].device].newOperations);
                        }
                        break;
                    default:
                        ;
                    }
                }
                break;
            case WRITE_PREPARE:
                break; // nop
            default:
                ;
            }
        }
    }
}

static int checkEventsandAlarms()
{
    int retval = 0, i;
    IEC_STRMAX varname = { 0, MAX_IDNAME_LEN, ""};
    HW119_GET_ADDR param = {(IEC_STRING *)&varname, 0 };

    fprintf(stderr, "%s()\n", __func__);
    for (i = 1; i < DimAlarmsCT; ++i) {

        if (ALCrossTable[i].ALTag[0] == '\0') {
            // last alarm
            break;
        }
        varname.CurLen = strnlen(ALCrossTable[i].ALTag, MAX_IDNAME_LEN);
        strncpy(varname.Contents, ALCrossTable[i].ALTag, MAX_IDNAME_LEN);
        hw119_get_addr(NULL, NULL, (unsigned char *)&param);
        ALCrossTable[i].TagAddr = param.ret_value;

        varname.CurLen = strnlen(ALCrossTable[i].ALSource, MAX_IDNAME_LEN);
        strncpy(varname.Contents, ALCrossTable[i].ALSource, MAX_IDNAME_LEN);
        hw119_get_addr(NULL, NULL, (unsigned char *)&param);
        ALCrossTable[i].SourceAddr = param.ret_value;

        if (ALCrossTable[i].ALCompareVar[0] == '\0') {
            ALCrossTable[i].CompareAddr = 0;
        } else {
            varname.CurLen = strnlen(ALCrossTable[i].ALCompareVar, MAX_IDNAME_LEN);
            strncpy(varname.Contents, ALCrossTable[i].ALCompareVar, MAX_IDNAME_LEN);
            hw119_get_addr(NULL, NULL, (unsigned char *)&param);
            ALCrossTable[i].CompareAddr = param.ret_value;
        }

        if (ALCrossTable[i].TagAddr == 0xffff || ALCrossTable[i].SourceAddr == 0xffff || ALCrossTable[i].CompareAddr == 0xffff) {
            fprintf(stderr, "%s: bad alarm/event #%d\n", i);
            retval = -1;
            break;
        }
        CrossTable[ALCrossTable[i].SourceAddr].usedInAlarmsEvents = TRUE;
        if (ALCrossTable[i].CompareAddr != 0) {
            CrossTable[ALCrossTable[i].CompareAddr].usedInAlarmsEvents = TRUE;
        }
    }
    return retval;
}

static int checkServersDevicesAndNodes()
{
    int retval = 0;

    // init tables
    theDevicesNumber = 0;
    theNodesNumber = 0;
    bzero(&theDevices[0], sizeof(theDevices));
    bzero(&theNodes[0], sizeof(theNodes));

    // for each enabled variable
    u_int16_t i, base, block;
    fprintf(stderr, "%s()\n", __func__);
    for (i = 1, base = 1, block = 0; i <= DimCrossTable; ++i) {

        // find block base addresses
        if (CrossTable[i].Block != block) {
            base = i;
            block = CrossTable[i].Block;
        }
        CrossTable[i].BlockBase = base;

        if (CrossTable[i].Enable > 0) {
            u_int16_t s = MAX_SERVERS;

            // server variables =---> enable the server thread
            switch (CrossTable[i].Protocol) {
            case PLC:
                // no plc server
                break;
            case RTU:
            case TCP:
            case TCPRTU:
            case CANOPEN:
            case MECT:
                // nothing to do for server
                break;
            case RTU_SRV:
            case TCP_SRV:
            case TCPRTU_SRV:
                // add unique variable's server
                for (s = 0; s < theServersNumber; ++s) {
                    if (CrossTable[i].Protocol == theServers[s].protocol
                     && strncmp(CrossTable[i].IPAddress, theServers[s].IPaddress, MAX_IPADDR_LEN) == 0
                     && CrossTable[i].Port == theServers[s].port) {
                        // already present
                        break;
                    }
                }
                if (s < theServersNumber) {
                    // ok already present
                } else if (theServersNumber >= MAX_SERVERS) {
                    fprintf(stderr, "%s() too many servers\n", __func__, MAX_SERVERS);
                    retval = -1;
                } else {
                    // new server entry
                    ++theServersNumber;
                    theServers[s].protocol = CrossTable[i].Protocol;
                    strncpy(theServers[s].IPaddress, CrossTable[i].IPAddress, MAX_IPADDR_LEN);
                    theServers[s].port = CrossTable[i].Port;
                    switch (theServers[s].protocol) {
                    case PLC:
                    case RTU:
                    case TCP:
                    case TCPRTU:
                    case CANOPEN:
                    case MECT:
                        // FIXME: assert
                        break;
                    case RTU_SRV: {
                        u_int16_t port = CrossTable[i].Port;
                        switch (port) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                            theServers[s].u.serial.port = port;
                            theServers[s].u.serial.baudrate = system_ini.serial_port[port].baudrate;
                            theServers[s].u.serial.parity = system_ini.serial_port[port].parity;
                            theServers[s].u.serial.databits = system_ini.serial_port[port].databits;
                            theServers[s].u.serial.stopbits = system_ini.serial_port[port].stopbits;
                            break;
                        default:
                            fprintf(stderr, "%s: bad RTU_SRV port %u for variable #%u", __func__, port, i);
                            retval = -1;
                        }
                        theServers[s].ctx = NULL;
                    }   break;
                    case TCP_SRV:
                        strncpy(theServers[s].u.tcp_ip.IPaddr, CrossTable[i].IPAddress, MAX_IPADDR_LEN);
                        theServers[s].u.tcp_ip.port = CrossTable[i].Port;
                        theServers[s].ctx = NULL;
                        break;
                    case TCPRTU_SRV:
                        strncpy(theServers[s].u.tcp_ip.IPaddr, CrossTable[i].IPAddress, MAX_IPADDR_LEN);
                        theServers[s].u.tcp_ip.port = CrossTable[i].Port;
                        theServers[s].ctx = NULL;
                        break;
                    default:
                        ;
                    }
                    theServers[s].NodeId = CrossTable[i].NodeId;
                    theServers[s].thread_id = 0;
                    theServers[s].thread_status = NOT_STARTED;
                    // theServers[s].serverMutex = PTHREAD_MUTEX_INITIALIZER;
                    snprintf(theServers[s].name, MAX_THREADNAME_LEN, "srv[%d]%s_%s_%d", s, fieldbusName[theServers[s].protocol], theServers[s].IPaddress, theServers[s].port);
                }
                break;
            default:
                break;
            }

            // client variables =---> link to the server and add unique devices and nodes
            switch (CrossTable[i].Protocol) {
            case PLC:
                // no plc client
                CrossTable[i].device = 0xffff;
                CrossTable[i].node = 0xffff;
                break;
            case RTU:
            case TCP:
            case TCPRTU:
            case CANOPEN:
            case MECT:
            case RTU_SRV:
            case TCP_SRV:
            case TCPRTU_SRV: {
                u_int16_t d;
                u_int16_t n;
                u_int16_t p;
                // add unique variable's device
                for (d = 0; d < theDevicesNumber; ++d) {
                    if (CrossTable[i].Protocol == theDevices[d].protocol
                     && strncmp(CrossTable[i].IPAddress, theDevices[d].IPaddress, MAX_IPADDR_LEN) == 0
                     && CrossTable[i].Port == theDevices[d].port) {
                        // already present
                        break;
                    }
                }
                if (d < theDevicesNumber) {
                    CrossTable[i].device = d; // found
                } else if (theDevicesNumber >= MAX_DEVICES) {
                    CrossTable[i].device = 0xffff; // FIXME: error
                    fprintf(stderr, "%s() too many devices\n", __func__, MAX_DEVICES);
                    retval = -1;
                } else {
                    // new device entry
                    CrossTable[i].device = theDevicesNumber;
                    ++theDevicesNumber;
                    theDevices[d].protocol = CrossTable[i].Protocol;
                    strncpy(theDevices[d].IPaddress, CrossTable[i].IPAddress, MAX_IPADDR_LEN);
                    theDevices[d].port = p = CrossTable[i].Port;
                    theDevices[d].server = 0xffff;
                    switch (theDevices[d].protocol) {
                    case PLC:
                        // FIXME: assert
                        break;
                    case RTU:
                    case MECT:
                        switch (p) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                            theDevices[d].u.serial.port = p;
                            theDevices[d].u.serial.baudrate = system_ini.serial_port[p].baudrate;
                            theDevices[d].u.serial.parity = system_ini.serial_port[p].parity;
                            theDevices[d].u.serial.databits = system_ini.serial_port[p].databits;
                            theDevices[d].u.serial.stopbits = system_ini.serial_port[p].stopbits;
                            theDevices[d].silence_ms = system_ini.serial_port[p].silence_ms;
                            theDevices[d].timeout_ms = system_ini.serial_port[p].timeout_ms;
                            break;
                        default:
                            fprintf(stderr, "%s: bad %s port %u for variable #%u", __func__,
                                    (theDevices[d].protocol == RTU ? "RTU" : "MECT"), p, i);
                            retval = -1;
                        }
                        break;
                    case TCP:
                        strncpy(theDevices[d].u.tcp_ip.IPaddr, CrossTable[i].IPAddress, MAX_IPADDR_LEN);
                        theDevices[d].u.tcp_ip.port = CrossTable[i].Port;
                        theDevices[d].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theDevices[d].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        break;
                    case TCPRTU:
                        strncpy(theDevices[d].u.tcp_ip.IPaddr, CrossTable[i].IPAddress, MAX_IPADDR_LEN);
                        theDevices[d].u.tcp_ip.port = CrossTable[i].Port;
                        theDevices[d].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theDevices[d].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        break;
                    case CANOPEN:
                        switch (p) {
                        case 0:
                        case 1:
                            theDevices[d].u.can.bus = p;
                            theDevices[d].u.can.baudrate = system_ini.canopen[p].baudrate;
                            theDevices[d].silence_ms = 0;
                            theDevices[d].timeout_ms = 0;
                            break;
                        default:
                            fprintf(stderr, "%s: bad CANOPEN port %u for variable #%u", __func__, p, i);
                            retval = -1;
                        }
                        break;
                    case RTU_SRV:
                        theDevices[d].server = s; // searched before
                        theDevices[d].silence_ms = system_ini.serial_port[p].silence_ms;
                        theDevices[d].timeout_ms = system_ini.serial_port[p].timeout_ms;
                        break;
                    case TCP_SRV:
                    case TCPRTU_SRV:
                        theDevices[d].server = s; // searched before
                        theDevices[d].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theDevices[d].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        break;
                    default:
                        ;
                    }
                    snprintf(theDevices[d].name, MAX_THREADNAME_LEN, "dev(%d)%s_%s_%u", d, fieldbusName[theDevices[d].protocol], theDevices[d].IPaddress, theDevices[d].port);
                    if (theDevices[d].timeout_ms == 0 && theDevices[d].protocol == RTU) {
                        theDevices[d].timeout_ms = 300;
                        fprintf(stderr, "%s: TimeOut of device '%s' forced to %u ms\n", __func__, theDevices[d].name, theDevices[d].timeout_ms);
                    }
                    theDevices[d].elapsed_time_ms = 0;
                    theDevices[d].status = ZERO;
                    // theDevices[d].thread_id = 0;
                    theDevices[d].thread_status = NOT_STARTED;
                    sem_init(&theDevices[d].newOperations, 0, 0);
                    theDevices[d].writeOperations = 0;
                    theDevices[d].writeResetIndex = 0;
                    theDevices[d].PLCwriteRequestNumber = 0;
                    theDevices[d].PLCwriteRequestGet = 0;
                    theDevices[d].PLCwriteRequestPut = 0;
                    // theDevices[d].modbus_ctx .last_good_ms, PLCwriteRequests, PLCwriteRequestNumber, PLCwriteRequestGet, PLCwriteRequestPut
                }
                // add variable's node
                for (n = 0; n < theNodesNumber; ++n) {
                    if (CrossTable[i].device == theNodes[n].device
                     && CrossTable[i].NodeId == theNodes[n].NodeID) {
                        // already present
                        break;
                    }
                }
                if (n < theNodesNumber) {
                    CrossTable[i].node = n; // found
                } else if (theNodesNumber >= MAX_NODES) {
                    CrossTable[i].node = 0xffff;
                    fprintf(stderr, "%s() too many nodes\n", __func__, MAX_NODES);
                    retval = -1;
                } else {
                    // new device entry
                    CrossTable[i].node = theNodesNumber;
                    ++theNodesNumber;
                    theNodes[n].device = CrossTable[i].device;
                    theNodes[n].NodeID = CrossTable[i].NodeId;
                    theNodes[n].status = NODE_OK;
                    // theNodes[n].RetryCounter .JumpRead
                }
            }   break;
            default:
                break;
            }
        }
    }
    return retval;
}

// Program1.st
static void *engineThread(void *statusAdr)
{
    enum threadStatus *threadStatusPtr = (enum threadStatus *)statusAdr;
    pVMM = get_pVMM();

    // thread init
    osPthreadSetSched(FC_SCHED_VMM, FC_PRIO_VMM); // engineThread

    XX_GPIO_SET(3);
    pthread_mutex_lock(&theCrosstableClientMutex);
    {
        int s, d;
        ErrorsState = 0;
        ERROR_FLAG = 0;

        if (LoadXTable(Alarms_csv)) {
            ALCrossTableState = FALSE;
            ERROR_FLAG |= ERROR_FLAG_ALARMS;
            // continue anyway
        }
        if (LoadXTable(Crosstable_csv)) {
            CrossTableState = FALSE;
            ERROR_FLAG |= ERROR_FLAG_CROSSTAB;
            goto exit_initialization;
        }
        if (!system_ini_ok) {
            goto exit_initialization;
        }
        if (checkServersDevicesAndNodes()) {
            ERROR_FLAG |= ERROR_FLAG_COMMPAR;
            goto exit_initialization;
        }
        if (checkEventsandAlarms()) {
            ERROR_FLAG |= ERROR_FLAG_COMMPAR;
            goto exit_initialization;
        }
        // create servers
        for (s = 0; s < theServersNumber; ++s) {
            theServers[s].thread_status = NOT_STARTED;
            if (osPthreadCreate(&theServers[s].thread_id, NULL, &serverThread, (void *)s, theServers[s].name, 0) == 0) {
                do {
                    osSleep(THE_CONFIG_DELAY_ms); // not sched_yield();
                } while (theServers[s].thread_status != RUNNING);
            } else {
                fprintf(stderr, "[%s] ERROR creating server thread %s: %s.\n", __func__, theServers[s].name, strerror(errno));
            }
        }
        // create clients
        for (d = 0; d < theDevicesNumber; ++d) {
            theDevices[d].thread_status = NOT_STARTED;
            if (osPthreadCreate(&theDevices[d].thread_id, NULL, &clientThread, (void *)d, theDevices[d].name, 0) == 0) {
                do {
                    osSleep(THE_CONFIG_DELAY_ms); // not sched_yield();
                } while (theDevices[d].thread_status != RUNNING);
            } else {
                fprintf(stderr, "[%s] ERROR creating device thread %s: %s.\n", __func__, theDevices[d].name, strerror(errno));
            }
        }
        // create udp servers
        if (osPthreadCreate(&theDataSyncThread_id, NULL, &datasyncThread, &theDataSyncThreadStatus, "datasync", 0) == 0) {
            do {
                osSleep(THE_CONFIG_DELAY_ms); // not sched_yield();
            } while (theDataSyncThreadStatus != RUNNING);
        }
        if  (ALCrossTableState) {
            CommEnabled = TRUE;
        }

    exit_initialization:
        ERROR_FLAG |= ERROR_FLAG_CONF_END;
    }
    pthread_mutex_unlock(&theCrosstableClientMutex);

    if (CommEnabled) {
        fprintf(stderr, "PLC communication enabled\n");
    } else {
        fprintf(stderr, "PLC communication disabled: application won't work.\n");
    }
    // run
    *threadStatusPtr = RUNNING;
    struct timespec abstime;
    ldiv_t x;
    clock_gettime(CLOCK_REALTIME, &abstime);
    pthread_mutex_lock(&theAlarmsEventsMutex);
	XX_GPIO_SET(3);

    while (!engineExiting) {

        if (engineRunning && CommEnabled) {
            // abstime.tv_sec += THE_ENGINE_DELAY_ms / 1000;
            abstime.tv_nsec = abstime.tv_nsec + (THE_ENGINE_DELAY_ms * 1E6); // (THE_ENGINE_DELAY_ms % 1000) * 1E6;
            if (abstime.tv_nsec >= 1E9) {
                x = ldiv(abstime.tv_nsec, 1E9);
                abstime.tv_sec += x.quot;
                abstime.tv_nsec = x.rem;
            }
            while (TRUE) {
                int e;
				XX_GPIO_CLR(3);
                e = pthread_cond_timedwait(&theAlarmsEventsCondvar, &theAlarmsEventsMutex, &abstime);
				XX_GPIO_SET(3);
                if (e == ETIMEDOUT) {
                    break;
                }
                pthread_mutex_lock(&theCrosstableClientMutex);
                {
                    AlarmMngr();
                }
                pthread_mutex_unlock(&theCrosstableClientMutex);
            }
            // TICtimer
            float PLC_time, PLC_timeMin, PLC_timeMax, PLC_timeWin;
            u_int32_t tic_ms;

            tic_ms = osGetTime32Ex() % (86400 * 1000); // 1 day overflow
            PLC_time = tic_ms / 1000.0;
            // PLC_timeWin    AT %QD0.21572: REAL; 5393
            memcpy(&PLC_timeWin, &the_QdataRegisters[5393], sizeof(u_int32_t));
            if (PLC_timeWin < 5.0) {
                PLC_timeWin = 5.0;
            }
            if (PLC_time <= PLC_timeWin) {
                PLC_timeMin = 0;
                PLC_timeMax = PLC_timeWin;
            } else {
                PLC_timeMin = PLC_time - PLC_timeWin;
                PLC_timeMax = PLC_time;
            }
            pthread_mutex_lock(&theCrosstableClientMutex);
            {
                // PLC_time         AT %ID0.21560: REAL; 5390
                // PLC_timeMin      AT %ID0.21564: REAL; 5391
                // PLC_timeMax      AT %ID0.21568: REAL; 5392
                memcpy(&the_QdataRegisters[5390], &PLC_time, sizeof(u_int32_t));
                memcpy(&the_QdataRegisters[5391], &PLC_timeMin, sizeof(u_int32_t));
                memcpy(&the_QdataRegisters[5392], &PLC_timeMax, sizeof(u_int32_t));
                memcpy(&the_QdataRegisters[5393], &PLC_timeWin, sizeof(u_int32_t));
                // NB no writeQdataRegisters();
            }
            pthread_mutex_unlock(&theCrosstableClientMutex);
        } else {
			XX_GPIO_CLR(3);
            osSleep(THE_ENGINE_DELAY_ms);
			XX_GPIO_SET(3);
        }
    }

    // thread clean
    // see dataEngineStop()
    pthread_mutex_unlock(&theAlarmsEventsMutex);

    // exit

    fprintf(stderr, "EXITING: engineThread\n");
    *threadStatusPtr = EXITING;
    return NULL;
}

static u_int16_t modbusRegistersNumber(u_int16_t DataAddr, u_int16_t DataNumber)
{
    u_int16_t retval = 0, i;

    for (i = 0; i < DataNumber; ++i) {
        switch (CrossTable[DataAddr + i].Types) {
        case  DINT: case  DINTDCBA: case  DINTCDAB: case  DINTBADC:
        case UDINT: case UDINTDCBA: case UDINTCDAB: case UDINTBADC:
        case REAL: case REALDCBA: case REALCDAB: case REALBADC:
            retval += 2;
            break;
        case  INT16: case  INT16BA:
        case UINT16: case UINT16BA:
            retval += 1;
            break;
        case BIT:
            retval += 1;
            break;
        case BYTE_BIT:
        case WORD_BIT:
        case DWORD_BIT: {
            register enum varTypes vartype = CrossTable[DataAddr + i].Types;
            register u_int16_t offset = CrossTable[DataAddr + i].Offset;

            do {
                // skip the other *_BIT variables of the same offset
            } while ((i + 1) < DataNumber
                && CrossTable[DataAddr + (i + 1)].Types == vartype
                && CrossTable[DataAddr + (i + 1)].Offset == offset
                && ++i);
            retval += (vartype == DWORD_BIT) ? 2 : 1;
        }   break;
        default:
            ; // FIXME: assert
        }
    }
    return retval;
}

static enum fieldbusError fieldbusRead(u_int16_t d, u_int16_t DataAddr, u_int32_t DataValue[], u_int16_t DataNumber)
{
    enum fieldbusError retval = NoError;
    u_int16_t i, r, regs, device, server, x;
    int e = 0;
    u_int8_t bitRegs[MODBUS_MAX_READ_BITS];         // > MAX_READS
    u_int16_t uintRegs[MODBUS_MAX_READ_REGISTERS];  // > MAX_READS

    switch (theDevices[d].protocol) {
    case PLC:
        // FIXME: assert
        break;
    case RTU:
    case TCP:
    case TCPRTU:       
        if (modbus_set_slave(theDevices[d].modbus_ctx, CrossTable[DataAddr].NodeId)) {
            retval = CommError;
            break;
        }
        // no break, continue in the following code
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        regs = modbusRegistersNumber(DataAddr, DataNumber);
        switch (theDevices[d].protocol) {
        case RTU:
        case TCP:
        case TCPRTU:
            if (CrossTable[DataAddr].Types == BIT) {
                bzero(bitRegs, sizeof(bitRegs));
                e = modbus_read_bits(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, bitRegs);
            } else {
                bzero(uintRegs, sizeof(uintRegs));
                e = modbus_read_registers(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, uintRegs);
            }
            break;
        case RTU_SRV:
        case TCP_SRV:
        case TCPRTU_SRV:
            device = CrossTable[DataAddr].device;
            if (device != 0xffff) {
                server = theDevices[device].server;
                if (server != 0xffff) {
                    pthread_mutex_lock(&theServers[server].mutex);
                    {
                        register u_int16_t base = CrossTable[DataAddr].Offset;
                        bzero(uintRegs, sizeof(uintRegs));
                        for (r = 0; r < regs; ++r) {
                            if ((base + r) < REG_SRV_NUMBER) {
                                uintRegs[r] = theServers[server].mb_mapping->tab_registers[base + r];
                            }
                        }
                        e = regs;
                    }
                    pthread_mutex_unlock(&theServers[server].mutex);
#ifdef VERBOSE_DEBUG
                    if (theDevices[d].protocol == TCP_SRV) {
                        fprintf(stderr, "%s read %u (%u) vars from %u (%s)\n", theDevices[d].name, DataNumber, regs, DataAddr, CrossTable[DataAddr].Tag);
                    }
#endif
                }
            }
            break;
        default:
            ;
        }
        if (e == regs) {
            retval = NoError;
            for (i = 0, r = 0; i < DataNumber && r < regs; ++i) {
#define DO_READ_OUTPUTS
#ifndef DO_READ_OUTPUTS
                if (CrossTable[DataAddr + i].Output) {
                    DataValue[i] = the_QdataRegisters[DataAddr + i];
                    switch (CrossTable[DataAddr + i].Types) {
                    case BIT:
                         r += 1;
                         break;
                    case BYTE_BIT:
                    case WORD_BIT:
                    case DWORD_BIT:
                        // manage this and the other *_BIT variables of the same offset
                        do {
                            DataValue[i] = the_QdataRegisters[DataAddr + i];
                        } while ((i + 1) < DataNumber
                            && CrossTable[DataAddr + (i + 1)].Types == CrossTable[DataAddr + i].Types
                            && CrossTable[DataAddr + (i + 1)].Offset == CrossTable[DataAddr + i].Offset
                            && ++i);
                        r += (CrossTable[DataAddr + i].Types == DWORD_BIT) ? 2 : 1;
                        break;
                    case UINT16:
                    case INT16:
                    case UINT16BA:
                    case INT16BA:
                        r += 1;
                        break;
                    case UDINT:
                    case DINT:
                    case REAL:
                    case UDINTCDAB:
                    case DINTCDAB:
                    case REALCDAB:
                    case UDINTDCBA:
                    case DINTDCBA:
                    case REALDCBA:
                    case UDINTBADC:
                    case DINTBADC:
                    case REALBADC:
                        r += 2;
                        break;
                    default:
                        ;
                    }
                } else {
#endif
                    register u_int8_t a = (uintRegs[r] & 0x00FF);
                    register u_int8_t b = (uintRegs[r] & 0xFF00) >> 8;
                    register u_int8_t c = (uintRegs[r + 1] & 0x00FF);
                    register u_int8_t d = (uintRegs[r + 1] & 0xFF00) >> 8;

                    switch (CrossTable[DataAddr + i].Types) {
                    case BIT:
                        DataValue[i] = bitRegs[r];
                        r += 1;
                        break;
                    case BYTE_BIT:
                    case WORD_BIT:
                    case DWORD_BIT: {
                        register enum varTypes vartype = CrossTable[DataAddr + i].Types;
                        register u_int16_t offset = CrossTable[DataAddr + i].Offset;

                        do {
                            // manage this and the other *_BIT variables of the same offset
                            x = CrossTable[DataAddr + i].Decimal; // 1..32
                            if (x <= 16) {
                                DataValue[i] = get_word_bit(uintRegs[r], x);
                            } else if (x <= 32 && (r + 1) < regs) {
                                DataValue[i] = get_word_bit(uintRegs[r + 1], x - 16);
                            } else {
                                DataValue[i] = 0; // FIXME: assert
                            }
                        } while ((i + 1) < DataNumber
                            && CrossTable[DataAddr + (i + 1)].Types == vartype
                            && CrossTable[DataAddr + (i + 1)].Offset == offset
                            && ++i);
                        r += (vartype == DWORD_BIT) ? 2 : 1;
                    }   break;
                    case UINT16:
                    case INT16:
                        DataValue[i] = uintRegs[r];
                        r += 1;
                        break;
                    case UINT16BA:
                    case INT16BA:
                        DataValue[i] = b + (a << 8);
                        r += 1;
                        break;
                    case UDINT:
                    case DINT:
                    case REAL:
                        DataValue[i] = a + (b << 8) + (c << 16) + (d << 24);
                        r += 2;
                        break;
                    case UDINTCDAB:
                    case DINTCDAB:
                    case REALCDAB:
                        DataValue[i] = c + (d << 8) + (a << 16) + (b << 24);
                        r += 2;
                        break;
                    case UDINTDCBA:
                    case DINTDCBA:
                    case REALDCBA:
                        DataValue[i] = d + (c << 8) + (b << 16) + (a << 24);
                        r += 2;
                        break;
                    case UDINTBADC:
                    case DINTBADC:
                    case REALBADC:
                        DataValue[i] = b + (a << 8) + (d << 16) + (c << 24);
                        r += 2;
                        break;
                    default:
                        ;
                    }
#ifndef DO_READ_OUTPUTS
                }
#endif
            }
        } else if (e == -1) { // OTHER_ERROR
            retval = CommError;
        } else if (e == -2) { // TIMEOUT_ERROR
            retval = TimeoutError;
        } else {
#ifdef VERBOSE_DEBUG
			fprintf(stderr, "fieldbusRead(): regs = %d, e = %d\n", regs, e); 
#endif
            retval = CommError;
        }
        break;
    case CANOPEN:
        device = CrossTable[DataAddr].device;
        if (device != 0xffff) {
            u_int8_t channel = theDevices[device].port;
            for (i = 0; i < DataNumber; ++i) {
                register enum varTypes vartype = CrossTable[DataAddr + i].Types;
                register u_int16_t offset = CrossTable[DataAddr + i].Offset;

                // cannot read outputs, anyway
                if (CrossTable[DataAddr + i].Output) {
                    DataValue[i] = the_QdataRegisters[DataAddr + i];                    
                } else {
                    switch (CrossTable[DataAddr + i].Types) {
                    case BIT: {
                        u_int8_t a;
                        e = CANopenReadPDOBit(channel, offset, &a);
                        DataValue[i] = a;
                    }   break;
                    case  BYTE_BIT: {
                        u_int8_t a;
                        e = CANopenReadPDOByte(channel, offset, &a);
                        do {
                            x = CrossTable[DataAddr + i].Decimal; // 1..8
                            DataValue[i] = get_byte_bit(a, x);
                        } while ((i + 1) < DataNumber
                            && CrossTable[DataAddr + (i + 1)].Types == vartype
                            && CrossTable[DataAddr + (i + 1)].Offset == offset
                            && ++i);
                    }   break;
                    case  WORD_BIT: {
                        u_int16_t a;
                        e = CANopenReadPDOWord(channel, offset, &a);
                        do {
                            x = CrossTable[DataAddr + i].Decimal; // 1..16
                            DataValue[i] = get_word_bit(a, x);
                        } while ((i + 1) < DataNumber
                            && CrossTable[DataAddr + (i + 1)].Types == vartype
                            && CrossTable[DataAddr + (i + 1)].Offset == offset
                            && ++i);
                    }   break;
                    case DWORD_BIT:{
                        u_int32_t a;
                        e = CANopenReadPDODword(channel, offset, &a);
                        do {
                            x = CrossTable[DataAddr + i].Decimal; // 1..32
                            DataValue[i] = get_dword_bit(a, x);
                        } while ((i + 1) < DataNumber
                            && CrossTable[DataAddr + (i + 1)].Types == vartype
                            && CrossTable[DataAddr + (i + 1)].Offset == offset
                            && ++i);
                    }   break;
                    case  UINT16:
                    case INT16: {
                        u_int16_t a;
                        e = CANopenReadPDOWord(channel, offset, &a);
                        DataValue[i] = a;
                    }   break;
                    case UDINT:
                    case DINT:
                    case REAL: {
                        u_int32_t a;
                        e = CANopenReadPDODword(channel, offset, &a);
                        DataValue[i] = a;
                    }   break;
                    case UINT16BA:
                    case INT16BA:
                    case UDINTCDAB:
                    case DINTCDAB:
                    case REALCDAB:
                    case UDINTDCBA:
                    case DINTDCBA:
                    case REALDCBA:
                    case UDINTBADC:
                    case DINTBADC:
                    case REALBADC:
                        // FIXME: assert
                        break;
                   default:
                        ;
                   }
                }
#ifdef VERBOSE_DEBUG
                fprintf(stderr, "%s: %s %s (%u)\n", theDevices[d].name, CrossTable[DataAddr + i].Tag,
                        e ? "err" : "ok", DataValue[i]);
#endif
                if (e == -1) { // OTHER_ERROR
                    retval = CommError;
                    // break;
                } else if (e == -2) { // TIMEOUT_ERROR
                    retval = TimeoutError;
                    break;
                }
            }
#ifdef VERBOSE_DEBUG
            fprintf(stderr, "%d vars @ %d: ", DataNumber, DataAddr);
            for (i = 0; i < DataNumber; ++i) {
                fprintf(stderr, "%s = 0x%08x ", CrossTable[DataAddr + i].Tag, DataValue[i]);
            }
            fprintf(stderr, "\n");
#endif
        }
        break;
    case MECT:
        // FIXME: TODO
        break;
    default:
        ;
    }
    return retval;
}

static enum fieldbusError fieldbusWrite(u_int16_t d, u_int16_t DataAddr, u_int32_t DataValue[], u_int16_t DataNumber)
{
    enum fieldbusError retval = NoError;
    u_int16_t i, r, regs, device, server, x;
    int e = 0;
    u_int8_t bitRegs[MODBUS_MAX_WRITE_BITS];         // > MAX_WRITES
    u_int16_t uintRegs[MODBUS_MAX_WRITE_REGISTERS];  // > MAX_WRITES

    switch (theDevices[d].protocol) {
    case PLC:
        // FIXME: assert
        break;
    case RTU:
    case TCP:
    case TCPRTU:
        if (modbus_set_slave(theDevices[d].modbus_ctx, CrossTable[DataAddr].NodeId)) {
            retval = CommError;
            break;
        }
        // no break, continue in the following code
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        // init values
        regs = modbusRegistersNumber(DataAddr, DataNumber);
        if (CrossTable[DataAddr].Types == BIT) {
            bzero(bitRegs, sizeof(bitRegs));
        } else {
            bzero(uintRegs, sizeof(uintRegs));
            // *_BIT management
            for (i = 0, r = 0; i < DataNumber && r < regs; ++i) {
                switch (CrossTable[DataAddr + i].Types) {
                case BIT:
                    r += 1; // FIXME: assert
                    break;
                case  BYTE_BIT:
                case  WORD_BIT:
                case DWORD_BIT: {
                    register u_int16_t addr;
                    register u_int16_t base = CrossTable[DataAddr + i].BlockBase;
                    register u_int16_t size = CrossTable[DataAddr + i].BlockSize;
                    register u_int16_t offset = CrossTable[DataAddr + i].Offset;
                    register enum varTypes vartype = CrossTable[DataAddr + i].Types;

                    // init the buffer bits with ALL the other actual bit values from the_QdataRegisters
                    for (addr = base; addr < (base + size); ++addr) {
                        if (CrossTable[addr].Types == vartype
                         && CrossTable[addr].Offset == offset
                         && !(DataAddr <= addr && addr < (DataAddr + DataNumber))) {
                            x = CrossTable[addr].Decimal; // 1..32
                            if (x <= 16) { // BYTE_BIT, WORD_BIT, DWORD_BIT
                                set_word_bit(&uintRegs[r], x, the_QdataRegisters[addr]);
                            } else if (x <= 32 && (r + 1) < regs) { // DWORD_BIT
                                set_word_bit(&uintRegs[r + 1], x - 16, the_QdataRegisters[addr]);
                            } else {
                                 // FIXME: assert
                            }
                        }
                    }
                    do {
                        // skip the other *_BIT variables of the same offset
                    } while ((i + 1) < DataNumber
                        && CrossTable[DataAddr + (i + 1)].Types == vartype
                        && CrossTable[DataAddr + (i + 1)].Offset == offset
                        && ++i);
                    r += (vartype == DWORD_BIT) ? 2 : 1;
                }   break;
                case UINT16:
                case INT16:
                case UINT16BA:
                case INT16BA:
                    r += 1;
                    break;
                case UDINT:
                case DINT:
                case REAL:
                case UDINTCDAB:
                case DINTCDAB:
                case REALCDAB:
                case UDINTDCBA:
                case DINTDCBA:
                case REALDCBA:
                case UDINTBADC:
                case DINTBADC:
                case REALBADC:
                    r += 2;
                    break;
                default:
                    ;
                }
            }
        }
        for (i = 0, r = 0; i < DataNumber && r < regs; ++i) {
            register u_int8_t a = (DataValue[i] & 0x000000FF);
            register u_int8_t b = (DataValue[i] & 0x0000FF00) >>  8;
            register u_int8_t c = (DataValue[i] & 0x00FF0000) >> 16;
            register u_int8_t d = (DataValue[i] & 0xFF000000) >> 24;

            switch (CrossTable[DataAddr + i].Types) {
            case BIT:
                bitRegs[r] = DataValue[i]; r += 1; break;
            case BYTE_BIT:
            case WORD_BIT:
            case DWORD_BIT: {
                register enum varTypes vartype = CrossTable[DataAddr + i].Types;
                register u_int16_t offset = CrossTable[DataAddr + i].Offset;

                do {
                    // manage this and the other *_BIT variables of the same offset
                    x = CrossTable[DataAddr + i].Decimal; // 1..32
                    if (x <= 16) { // BYTE_BIT, WORD_BIT, DWORD_BIT
                        set_word_bit(&uintRegs[r], x, DataValue[i]);
                    } else if (x <= 32 && (r + 1) < regs) { // DWORD_BIT
                        set_word_bit(&uintRegs[r + 1], x - 16, DataValue[i]);
                    }
                } while ((i + 1) < DataNumber
                      && CrossTable[DataAddr + (i + 1)].Types == vartype
                      && CrossTable[DataAddr + (i + 1)].Offset == offset
                      && ++i);
                r += (vartype == DWORD_BIT) ? 2 : 1;
            }   break;
            case UINT16:
            case INT16:
                uintRegs[r] = DataValue[i];
                r += 1;
                break;
            case UINT16BA:
            case  INT16BA:
                uintRegs[r] = b + (a << 8);
                r += 1;
                break;
            case UDINT:
            case DINT:
            case REAL:
                uintRegs[r] = a + (b << 8);
                uintRegs[r + 1] = c + (d << 8);
                r += 2;
                break;
            case UDINTCDAB:
            case DINTCDAB:
            case REALCDAB:
                uintRegs[r] = c + (d << 8);
                uintRegs[r + 1] = a + (b << 8);
                r += 2;
                break;
            case UDINTDCBA:
            case DINTDCBA:
            case REALDCBA:
                uintRegs[r] = d + (c << 8);
                uintRegs[r + 1] = b + (a << 8);
                r += 2;
                break;
            case UDINTBADC:
            case DINTBADC:
            case REALBADC:
                uintRegs[r] = b + (a << 8);
                uintRegs[r + 1] = d + (c << 8);
                r += 2;
                break;
            default:
                ;
            }
        }
        switch (theDevices[d].protocol) {
        case RTU:
        case TCP:
        case TCPRTU:
            if (CrossTable[DataAddr].Types == BIT) {
                e = modbus_write_bits(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, bitRegs);
            } else if (regs == 1){
                e = modbus_write_register(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, uintRegs[0]);
            } else {
                e = modbus_write_registers(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, uintRegs);
            }
#ifdef VERBOSE_DEBUG
            if (theDevices[d].protocol == TCP) {
                fprintf(stderr, "%s wrote %u (%u) vars from %u (%s)\n", theDevices[d].name, DataNumber, regs, DataAddr, CrossTable[DataAddr].Tag);
            }
#endif
            break;
        case RTU_SRV:
        case TCP_SRV:
        case TCPRTU_SRV:
            device = CrossTable[DataAddr].device;
            if (device != 0xffff) {
                server = theDevices[device].server;
                if (server != 0xffff) {
                    pthread_mutex_lock(&theServers[server].mutex);
                    {
                        register u_int16_t base = CrossTable[DataAddr].Offset;
                        for (r = 0; r < regs; ++r) {
                            if ((base + r) < REG_SRV_NUMBER) {
                                theServers[server].mb_mapping->tab_registers[base + r] = uintRegs[r];
                            }
                        }
                    }
                    pthread_mutex_unlock(&theServers[server].mutex);
                }
            }
            break;
        default:
            ; //FIXME: assert
        }
        switch (e) {
        case -1: // OTHER_ERROR
            retval = CommError;
            break;
        case -2: // TIMEOUT_ERROR
            retval = TimeoutError;
            break;
        default:
            retval = NoError;
        }
        break;
    case CANOPEN:
        device = CrossTable[DataAddr].device;
        if (device != 0xffff) {
            u_int8_t channel = theDevices[device].port;
            for (i = 0; i < DataNumber; ++i) {
                register u_int16_t offset = CrossTable[DataAddr + i].Offset;

                switch (CrossTable[DataAddr + i].Types) {
                case       BIT:
                    e = CANopenWritePDOBit(channel, offset, DataValue[i]);
                    break;
                case BYTE_BIT:
                case WORD_BIT:
                case DWORD_BIT:  {
                    register u_int16_t addr;
                    register u_int16_t base = CrossTable[DataAddr + i].BlockBase;
                    register u_int16_t size = CrossTable[DataAddr + i].BlockSize;
                    register u_int16_t offset = CrossTable[DataAddr + i].Offset;
                    register enum varTypes vartype = CrossTable[DataAddr + i].Types;
                    u_int32_t buffer = 0x00000000; // BYTE_BIT, WORD_BIT, DWORD_BIT

                    // init the buffer bits with ALL the actual bit values from the_QdataRegisters
                    for (addr = base; addr < (base + size); ++addr) {
                        if (CrossTable[addr].Types == vartype
                         && CrossTable[addr].Offset == offset) {
                            x = CrossTable[addr].Decimal; // 1..8/16/32
                            if (DataAddr <= addr && addr < (DataAddr + DataNumber)) {
                                set_dword_bit(&buffer, x, DataValue[addr - DataAddr]);
                            } else {
                                set_dword_bit(&buffer, x, the_QdataRegisters[addr]);
                            }
                        }
                    }
                    // do write
                    switch (vartype) {
                    case BYTE_BIT: {
                        register u_int8_t a = (u_int8_t)(buffer & 0x000000FF);
                        e = CANopenWritePDOByte(channel, offset, a);
                    }   break;
                    case WORD_BIT: {
                        register u_int16_t a = (u_int16_t)(buffer & 0x0000FFFF);
                        e = CANopenWritePDOWord(channel, offset, a);
                    }   break;
                    case DWORD_BIT:
                        e = CANopenWritePDODword(channel, offset, buffer);
                        break;
                    default:
                        ; // FIXME: assert
                    }
                    do {
                        // skip the other *_BIT variables of the same offset
                    } while ((i + 1) < DataNumber
                        && CrossTable[DataAddr + (i + 1)].Types == vartype
                        && CrossTable[DataAddr + (i + 1)].Offset == offset
                        && ++i);
                }   break;
                case UINT16:
                case INT16:
                    e = CANopenWritePDOWord(channel, offset, DataValue[i]);
                    break;
                case UDINT:
                case DINT:
                case REAL:
                    e = CANopenWritePDODword(channel, offset, DataValue[i]);
                    break;
                case UINT16BA:
                case INT16BA:
                case UDINTCDAB:
                case  DINTCDAB:
                case REALCDAB:
                case UDINTDCBA:
                case DINTDCBA:
                case REALDCBA:
                case UDINTBADC:
                case DINTBADC:
                case REALBADC:
                    // FIXME: assert
                    break;
                default:
                    ;
                }
                if (e) {
                    retval = TimeoutError; // CommError;
                    break;
                }
            }
        }
        break;
    case MECT:
        // FIXME: TODO
        break;
    default:
        ;
    }
    return retval;
}

static void *serverThread(void *arg)
{
    u_int32_t s = (u_int32_t)arg;
    modbus_t * modbus_ctx = NULL;
    u_int8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int master_socket;
    int rc;
    fd_set refset;
    fd_set rdset;
    int fdmax = 0;
    int server_socket = -1;
    int threadInitOK = FALSE;

    // thread init
    osPthreadSetSched(FC_SCHED_IO_DAT, FC_PRIO_IO_DAT); // serverThread
    pthread_mutex_init(&theServers[s].mutex, NULL);
    switch (theServers[s].protocol) {
    case RTU_SRV: {
        char device[VMM_MAX_PATH];

#if XENO_RTDM
        snprintf(device, VMM_MAX_PATH, "rtser%u", theServers[s].u.serial.port);
#else
        snprintf(device, VMM_MAX_PATH, "/dev/ttySP%u", theServers[s].u.serial.port);
#endif
        modbus_ctx = modbus_new_rtu(device, theServers[s].u.serial.baudrate,
                            theServers[s].u.serial.parity, theServers[s].u.serial.databits, theServers[s].u.serial.stopbits);
        theServers[s].mb_mapping = modbus_mapping_new(0, 0, REG_SRV_NUMBER, 0);
    }   break;
    case TCP_SRV:
        modbus_ctx = modbus_new_tcp(theServers[s].u.tcp_ip.IPaddr, theServers[s].u.tcp_ip.port);
        theServers[s].mb_mapping = modbus_mapping_new(0, 0, REG_SRV_NUMBER, 0);
        break;
    case TCPRTU_SRV:
        modbus_ctx = modbus_new_tcprtu(theServers[s].u.tcp_ip.IPaddr, theServers[s].u.tcp_ip.port);
        theServers[s].mb_mapping = modbus_mapping_new(0, 0, REG_SRV_NUMBER, 0);
        break;
    default:
        ;
    }
    if (modbus_ctx != NULL && theServers[s].mb_mapping != NULL
     && modbus_set_slave(modbus_ctx, theServers[s].NodeId) == 0) {
        threadInitOK = TRUE;
    }

    // run
    theServers[s].thread_status = RUNNING;
    while (!engineExiting) {

        // trivial scenario
        if (!engineRunning || !threadInitOK) {
            osSleep(THE_CONNECTION_DELAY_ms);
            continue;
        }

        // get file descriptor or bind and listen
        if (server_socket == -1) {
            switch (theServers[s].protocol) {
            case RTU_SRV:
                server_socket = modbus_get_socket(modbus_ctx); // here socket is file descriptor
                break;
            case TCP_SRV:
                server_socket = modbus_tcp_listen(modbus_ctx, THE_SRV_MAX_CLIENTS);
                break;
            case TCPRTU_SRV:
                server_socket = modbus_tcprtu_listen(modbus_ctx, THE_SRV_MAX_CLIENTS);
                break;
            default:
                ;
            }
            if (server_socket >= 0) {
                FD_ZERO(&refset);
                FD_SET(server_socket, &refset);
                fdmax = server_socket;
            }
        }
        if (server_socket < 0) {
            osSleep(THE_SERVER_DELAY_ms);
            continue;
        }

        // wait on server socket, only until timeout
        struct timeval tv;
        rdset = refset;
        tv.tv_sec = THE_SERVER_DELAY_ms / 1000;
        tv.tv_usec = THE_SERVER_DELAY_ms % 1000;
        if (select(fdmax+1, &rdset, NULL, NULL, &tv) <= 0) {
            // timeout or error
            osSleep(THE_SERVER_DELAY_ms);
            continue;
        }

        // accept requests
        switch (theServers[s].protocol) {
        case RTU_SRV:
            // unique client (serial line)
            rc = modbus_receive(modbus_ctx, query);
            if (rc > 0) {
                pthread_mutex_lock(&theServers[s].mutex);
                {
                    modbus_reply(modbus_ctx, query, rc, theServers[s].mb_mapping);
                }
                pthread_mutex_unlock(&theServers[s].mutex);
            }
            break;
        case TCP_SRV:
        case TCPRTU_SRV:
            // multiple clients (tcp/ip server)
            for (master_socket = 0; master_socket <= fdmax; ++master_socket) {
                if (!FD_ISSET(master_socket, &rdset)) {
                    continue;
                }
                if (master_socket == server_socket) {
                    socklen_t addrlen;
                    struct sockaddr_in clientaddr;
                    int newfd;

                    addrlen = sizeof(clientaddr);
                    memset(&clientaddr, 0, sizeof(clientaddr));
                    newfd = accept(server_socket, (struct sockaddr *)&clientaddr, &addrlen);
                    if (newfd == -1) {
                        perror("Server accept() error");
                    } else {
                        FD_SET(newfd, &refset);

                        if (newfd > fdmax) {
                            fdmax = newfd;
                        }
                        fprintf(stderr, "New connection from %s:%d on socket %d\n",
                            inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
                    }
                } else {
                    modbus_set_socket(modbus_ctx, master_socket);
                    rc = modbus_receive(modbus_ctx, query);
                    if (rc > 0) {
                        pthread_mutex_lock(&theServers[s].mutex);
                        {
                            modbus_reply(modbus_ctx, query, rc, theServers[s].mb_mapping);
                        }
                        pthread_mutex_unlock(&theServers[s].mutex);
                    } else if (rc == -1) {
                        fprintf(stderr, "Connection closed on socket %d\n", master_socket);
                        close(master_socket);

                        FD_CLR(master_socket, &refset);
                        if (master_socket == fdmax) {
                            --fdmax;
                        }
                    }
                }
            }
            break;
        default:
            ;
        }
    }

    // thread clean
    switch (theServers[s].protocol) {
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        if (theServers[s].mb_mapping != NULL) {
            modbus_mapping_free(theServers[s].mb_mapping);
            theServers[s].mb_mapping = NULL;
        }
        if (server_socket != -1) {
            shutdown(server_socket, SHUT_RDWR);
            close(server_socket);
            server_socket = -1;
         }
        if (modbus_ctx != NULL) {
            modbus_close(modbus_ctx);
            modbus_free(modbus_ctx);
        }
        break;
    default:
        ;
    }

    // exit
    fprintf(stderr, "EXITING: %s\n", theServers[s].name);
    theServers[s].thread_status = EXITING;
    return arg;
}

static void zeroNodeVariables(u_int32_t node)
{
    u_int16_t addr;

    if (theDevices[theNodes[node].device].protocol != CANOPEN) {
        fprintf(stderr, "should zeroNodeVariables() node=%u (%u) in %s\n", node, theNodes[node].NodeID, theDevices[theNodes[node].device].name);
#if 0
        for (addr = 1; addr <= DimCrossTable; ++addr) {
            if (CrossTable[addr].Enable > 0 && CrossTable[addr].node == node) {
                writeQdataRegisters(addr, 0);
            }
        }
#endif
    }
}

static void zeroDeviceVariables(u_int32_t d)
{
    u_int16_t addr;

    if (theDevices[d].protocol != CANOPEN) {
        fprintf(stderr, "should zeroDeviceVariables() device=%u %s\n", d, theDevices[d].name);
#if 0
        for (addr = 1; addr <= DimCrossTable; ++addr) {
            if (CrossTable[addr].Enable > 0 && CrossTable[addr].device == d) {
                writeQdataRegisters(addr, 0);
            }
        }
#endif
    }
}

static inline void startDeviceTiming(u_int32_t d)
{
    struct timespec abstime;
    register u_int32_t now_ms;

    clock_gettime(CLOCK_REALTIME, &abstime);
    now_ms = abstime.tv_sec * 1000 + abstime.tv_nsec / 1E6;
    theDevices[d].elapsed_time_ms = 0;
    theDevices[d].current_time_ms = now_ms;
}

static inline void updateDeviceTiming(u_int32_t d)
{
    struct timespec abstime;
    register u_int32_t now_ms, delta_ms;

    clock_gettime(CLOCK_REALTIME, &abstime);
    now_ms = abstime.tv_sec * 1000 + abstime.tv_nsec / 1E6;
    delta_ms = now_ms - theDevices[d].current_time_ms;
    theDevices[d].elapsed_time_ms += delta_ms;
    theDevices[d].current_time_ms = now_ms;
}

static inline void changeDeviceStatus(u_int32_t d, enum DeviceStatus status)
{
    theDevices[d].elapsed_time_ms = 0;
    theDevices[d].status = status;
#ifdef VERBOSE_DEBUG
    fprintf(stderr, "%s: status = %s\n", theDevices[d].name, statusName[status]);
#endif
}

static void *clientThread(void *arg)
{
    u_int32_t d = (u_int32_t)arg;

    // device connection management
    struct timeval response_timeout;

    // device queue management by priority and periods
    u_int16_t prio;                         // priority of variables
    u_int16_t write_index;                  // current write index in queue
    u_int16_t read_addr[MAX_PRIORITY];      // current read address in crosstable for each priority
    u_int16_t read_index[MAX_PRIORITY];     // current read indexes in queue for each priority
    u_int32_t read_time_ms[MAX_PRIORITY];   // next read time for each priority

    // data for each fieldbus operation
    u_int16_t QueueIndex;    // command index in the queue
    u_int16_t Operation;     // read/write normal/recipe single/multiple
    u_int16_t DataAddr = 0;  // variable address in the crosstable
    u_int32_t DataValue[64]; // max 64 reads and 16 writes
    u_int32_t DataNumber;    // max 64 reads and 16 writes
    u_int16_t DataNodeId;    // variable node ID ("0" if not applicable)
    u_int16_t Data_node;     // global index of the node in theNodes[]
    enum fieldbusError error;


    // ------------------------------------------ thread init
    osPthreadSetSched(FC_SCHED_IO_DAT, FC_PRIO_IO_DAT); // clientThread
    theDevices[d].modbus_ctx = NULL;
    // "new"
    switch (theDevices[d].protocol) {
    case PLC: // FIXME: assert
        break;
    case RTU: {
        char device[VMM_MAX_PATH];

#if XENO_RTDM
        snprintf(device, VMM_MAX_PATH, "rtser%u", theDevices[d].u.serial.port);
#else
        snprintf(device, VMM_MAX_PATH, "/dev/ttySP%u", theDevices[d].u.serial.port);
#endif
        theDevices[d].modbus_ctx = modbus_new_rtu(device, theDevices[d].u.serial.baudrate,
                            theDevices[d].u.serial.parity, theDevices[d].u.serial.databits, theDevices[d].u.serial.stopbits);
    }   break;
    case TCP:
        theDevices[d].modbus_ctx = modbus_new_tcp(theDevices[d].u.tcp_ip.IPaddr, theDevices[d].u.tcp_ip.port);
        break;
    case TCPRTU:
        theDevices[d].modbus_ctx = modbus_new_tcprtu(theDevices[d].u.tcp_ip.IPaddr, theDevices[d].u.tcp_ip.port);
        break;
    case CANOPEN: {
        u_int16_t addr;
        CANopenStart(theDevices[d].u.can.bus); // may start more threads
        // get the unspecified CANopen variables offsets ("CH0_NETRUN" ...)
        for (addr = 1; addr <= DimCrossTable; ++addr) {
            if (CrossTable[addr].Enable > 0 && CrossTable[addr].device == d) {
                if (CrossTable[addr].Offset == 0) {
                    CrossTable[addr].Offset = CANopenGetVarIndex(CrossTable[addr].Port, CrossTable[addr].Tag);
                }
#ifdef VERBOSE_DEBUG
                fprintf(stderr, "CANOPEN %u.%u '%s' %u\n", CrossTable[addr].Port, CrossTable[addr].NodeId, CrossTable[addr].Tag, CrossTable[addr].Offset);
#endif
            }
        }
    }   break;
    case MECT:
        break; // FIXME: check can state
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        break;
    default:
        ;
    }
    // check the "new" result
    switch (theDevices[d].protocol) {
    case PLC: // FIXME: assert
        break;
    case RTU:
    case TCP:
    case TCPRTU:
        if (theDevices[d].modbus_ctx == NULL) {
            changeDeviceStatus(d, NO_HOPE);
        } else {
            changeDeviceStatus(d, NOT_CONNECTED);
        }
        break;
    case CANOPEN: {
        u_int8_t channel = theDevices[d].u.can.bus;
        struct CANopenStatus status;
        CANopenGetChannelStatus(channel, &status);
        if (status.running || status.error != 0xffffffff) {
            changeDeviceStatus(d, NOT_CONNECTED);
        } else {
            changeDeviceStatus(d, NO_HOPE);
        }
    }   break;
    case MECT:
        changeDeviceStatus(d, NOT_CONNECTED); // FIXME: check state
        break;
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        changeDeviceStatus(d, NOT_CONNECTED);
        break;
    default:
        ;
    }
    // update the error flags
    if (theDevices[d].status == NOT_CONNECTED) {
        switch (theDevices[d].protocol) {
        case PLC: // FIXME: assert
            break;
        case RTU:
            ERROR_FLAG |= ERROR_FLAG_RTU_ON;
            break;
        case TCP:
            ERROR_FLAG |= ERROR_FLAG_TCP_ON;
            break;
        case TCPRTU:
            ERROR_FLAG |= ERROR_FLAG_TCPRTU_ON;
            break;
        case CANOPEN:
            ERROR_FLAG |= ERROR_FLAG_CANOPEN_ON;
            break;
        case MECT:
            ERROR_FLAG |= ERROR_FLAG_MECT_ON;
            break;
        case RTU_SRV:
            ERROR_FLAG |= ERROR_FLAG_RTUSRV_ON;
            break;
        case TCP_SRV:
            ERROR_FLAG |= ERROR_FLAG_TCPSRV_ON;
            break;
        case TCPRTU_SRV:
            ERROR_FLAG |= ERROR_FLAG_TCPRTUSRV_ON;
            break;
        default:
            ;
        }
    }

    // ------------------------------------------ run
    fprintf(stderr, "%s: ", theDevices[d].name);
    switch (theDevices[d].protocol) {
    case PLC: // FIXME: assert
        break;
    case RTU:
        fprintf(stderr, "@%u/%u/%c/%u, ", theDevices[d].u.serial.baudrate, theDevices[d].u.serial.databits, theDevices[d].u.serial.parity, theDevices[d].u.serial.stopbits);
        break;
    case TCP:
    case TCPRTU:
        break;
    case CANOPEN:
        break;
    case MECT:
        break;
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        break;
    default:
        ;
    }
    fprintf(stderr, "silence_ms=%u, timeout_ms=%u\n", theDevices[d].silence_ms, theDevices[d].timeout_ms);
    response_timeout.tv_sec = theDevices[d].timeout_ms / 1000;
    response_timeout.tv_usec = (theDevices[d].timeout_ms % 1000) * 1000;
    startDeviceTiming(d);
    write_index = 1;
    for (prio = 0; prio < MAX_PRIORITY; ++prio) {
        read_time_ms[prio] = theDevices[d].current_time_ms;
        read_addr[prio] = 1;
        read_index[prio] = 1;
    }

    // start the fieldbus operations loop
    DataAddr = 0;
    DataNumber = 0;
    Operation = NOP;
    error = NoError;
    theDevices[d].thread_status = RUNNING;
    XX_GPIO_SET(5);
    while (!engineExiting) {

        updateDeviceTiming(d);

        // trivial scenario
        if (!engineRunning || theDevices[d].status == NO_HOPE) {
            XX_GPIO_CLR(5);
            osSleep(THE_CONNECTION_DELAY_ms);
            XX_GPIO_SET(5);
            continue;
        }

        // was I already doing something?
        if (DataAddr == 0) {
            int rc;
            u_int32_t next_ms;

            // wait for next operation or next programmed read
            next_ms = theDevices[d].current_time_ms + THE_UDP_TIMEOUT_ms;
            for (prio = 0; prio < MAX_PRIORITY; ++prio) {
                if (read_time_ms[prio] < theDevices[d].current_time_ms) {
                    read_time_ms[prio] = theDevices[d].current_time_ms;
                }
                if (read_time_ms[prio] < next_ms) {
                    next_ms = read_time_ms[prio];
                }
            }
            if (next_ms > theDevices[d].current_time_ms) {
                int timeout, invalid_timeout, invalid_permission, other_error;
                ldiv_t q;
                struct timespec abstime;
                q = ldiv(next_ms, 1000);
                abstime.tv_sec = q.quot;
                abstime.tv_nsec = q.rem * 1E6; // ms -> ns
                do {
                    XX_GPIO_CLR(5);
                    rc = sem_timedwait(&theDevices[d].newOperations, &abstime);
                    XX_GPIO_SET(5);
                    timeout = invalid_timeout = invalid_permission = other_error = FALSE;
                    if (rc == 0) {
                        break;
                    }
                    if (errno == EINTR) {
                        continue;
                    }
                    if (errno ==  ETIMEDOUT) {
                        timeout = TRUE;
                        break;
                    }
                    if (errno ==  EINVAL) {
                        invalid_timeout = TRUE;
                        break;
                    }
                    if (errno ==  EPERM) {
                        invalid_permission = TRUE;
                        break;
                    }
                    other_error = TRUE;
                    break;
                } while (TRUE);
                // what time is it please?
                updateDeviceTiming(d);
#ifdef VERBOSE_DEBUG
                fprintf(stderr, "%s@%09u ms: woke up because %s (%09u ms = %u s + %d ns)\n", theDevices[d].name, theDevices[d].current_time_ms,
                    timeout?"timeout":(invalid_timeout?"invalid_timeout":(invalid_permission?"invalid_permission":(other_error?"other_error":"signal"))),
                    next_ms, abstime.tv_sec, abstime.tv_nsec);
#endif
            } else {
#ifdef VERBOSE_DEBUG
                fprintf(stderr, "%s@%09u ms: immediate restart\n", theDevices[d].name, theDevices[d].current_time_ms);
#endif
            }
        }

        // manage the reset of the error flags
        switch (theDevices[d].protocol) {
        case PLC: // FIXME: assert
            break;
        case RTU:
            if (Reset_RTU) {
                int i;
                for (i = 1; i <= 64; ++i) {
                    CounterRTU(i) = 0;
                }
                RTUBlackList_ERROR_WORD = 0;
                RTUComm_ERROR_WORD = 0;
                BlackListRTU = 0ULL;
                CommErrRTU = 0ULL;
                ERROR_FLAG &= ~ERROR_FLAG_RTU;
            }
            break;
        case TCP:
            if (Reset_TCP) {
                int i;
                for (i = 1; i <= 64; ++i) {
                    CounterTCP(i) = 0;
                }
                TCPBlackList_ERROR_WORD = 0;
                TCPComm_ERROR_WORD = 0;
                BlackListTCP = 0ULL;
                CommErrTCP = 0ULL;
                ERROR_FLAG &= ~ERROR_FLAG_TCP;
            }
            break;
        case TCPRTU:
            if (Reset_TCPRTU) {
                int i;
                for (i = 1; i <= 64; ++i) {
                    CounterTCPRTU(i) = 0;
                }
                TCPRTUBlackList_ERROR_WORD = 0;
                TCPRTUComm_ERROR_WORD = 0;
                BlackListTCPRTU = 0ULL;
                CommErrTCPRTU = 0ULL;
                ERROR_FLAG &= ~ERROR_FLAG_TCPRTU;
            }
            break;
        case CANOPEN:
        case MECT:
        case RTU_SRV:
        case TCP_SRV:
        case TCPRTU_SRV:
            // FIXME: add flags (missing the Reset_XXX)
            break;
        default:
            ;
        }

        // choose the read/write command either in the local queue or in the queue from HMI
        if (DataAddr == 0) {
            pthread_mutex_lock(&theCrosstableClientMutex);
            {
                // is it there an immediate write requests from PLC to this device?
                if (theDevices[d].PLCwriteRequestNumber > 0) {
                    u_int16_t n;

                    QueueIndex = 0;
                    DataAddr = theDevices[d].PLCwriteRequests[theDevices[d].PLCwriteRequestGet].Addr;
                    Operation = (DataNumber == 1) ? WRITE_SINGLE : WRITE_MULTIPLE;
                    // data values from the local queue only, the *_BIT management is in fieldWrite()
                    DataNumber = theDevices[d].PLCwriteRequests[theDevices[d].PLCwriteRequestGet].Number;
                    for (n = 0; n < DataNumber; ++n) {
                        DataValue[n] = theDevices[d].PLCwriteRequests[theDevices[d].PLCwriteRequestGet].Values[n];
                    }
                    // local queue management
                    theDevices[d].PLCwriteRequestGet = (theDevices[d].PLCwriteRequestGet + 1) % MaxLocalQueue;
                    theDevices[d].PLCwriteRequestNumber -= 1;
#ifdef VERBOSE_DEBUG
                    fprintf(stderr, "%s@%09u ms: write PLC [%u], there are still %u\n", theDevices[d].name, theDevices[d].current_time_ms, DataAddr, theDevices[d].PLCwriteRequestNumber);
#endif
                // is it there a write requests from PLC to this device?
                } else if (theDevices[d].writeOperations > 0) {
                    u_int16_t indx, oper, addr;

                    // it should be there something to write from HMI to this device
                    int found = FALSE;
                    if (theDevices[d].writeResetIndex) {
                        write_index = 1; // for recipes
                        theDevices[d].writeResetIndex = 0;
                    }
                    for (indx = write_index; indx <= DimCrossTable; ++indx) {
                        oper = the_IsyncRegisters[indx] & QueueOperMask;
                        addr = the_IsyncRegisters[indx] & QueueAddrMask;
                        if (oper == 0 && addr == 0) {
                            // queue tail, jump to the array end
                            indx = DimCrossTable;
                        } else if (CrossTable[addr].device == d
                               && (oper == WRITE_SINGLE || oper == WRITE_MULTIPLE
                                || oper == WRITE_RIC_SINGLE || oper == WRITE_RIC_MULTIPLE)) {
                            found = TRUE;
                            break;
                        }
                    }
                    if (found) {
                        QueueIndex = indx;
                        Operation = oper; // WRITE_*
                        DataAddr = addr;
                        DataNumber = 1;
                        DataValue[0] = the_IdataRegisters[addr];
                        // FIXME: we could search the Isync for other writes in order at the same priority
                        //        of the same type to the same fieldbus, but the complex thing is the
                        //        management of the Qsync and of the errors
                        theDevices[d].writeOperations -= 1;
                        write_index = indx + 1; // may overlap DimCrossTable, it's ok
#ifdef VERBOSE_DEBUG
                        fprintf(stderr, "%s@%09u ms: write [%u]@%u value=%u, will check @%u\n", theDevices[d].name, theDevices[d].current_time_ms, DataAddr, indx, DataValue[0], write_index);
#endif
                    } else {
                        // next time we'll restart from the first one
                        write_index = 1;
                    }
                }

                // if no write then is it there a read request for this device?
                if (DataAddr == 0) {
                    u_int16_t indx, oper, addr;

                    // periodic read requests:
                    // -- {P,S,F} automatically enabled from Crosstable
                    // -- {H} enabled from HMI when the page is displayed
                    for (prio = 0; prio < MAX_PRIORITY; ++prio) {
                        // only when the timer expires
                        if (read_time_ms[prio] <= theDevices[d].current_time_ms) {

                            // is it there anything to read at this priority for this device?
                            int found = FALSE;

                            // {P,S,F} from Crosstable
                            for (addr = read_addr[prio]; addr <= DimCrossTable; ++addr) {
                                if (CrossTable[addr].device == d
                                 && CrossTable[addr].Enable == (prio + 1)
                                 && CrossTable[addr].Plc > Htype
                                 && addr == CrossTable[addr].BlockBase // reading by block only
                                 && the_QdataStates[addr] != DATA_RUN) {
                                    found = TRUE;
                                    break;
                                }
                            }
                            if (found) {
                                QueueIndex = 0;
                                DataAddr = addr; // CrossTable[addr].BlockBase;
                                DataNumber = CrossTable[addr].BlockSize;
                                Operation = READ;
                                // data values will be available after the fieldbus access
                                // keep the index for the next loop
                                read_addr[prio] = DataAddr + DataNumber; // may overlap DimCrossTable, it's ok
#ifdef VERBOSE_DEBUG
                                fprintf(stderr, "%s@%09u ms: read %uPSF [%u] (was [%u]), will check [%u]\n", theDevices[d].name, theDevices[d].current_time_ms, prio+1, DataAddr, addr, read_addr[prio]);
#endif
                                break;
                            } else {
                                // read_time_ms[prio] only set if no H as well
                                read_addr[prio] = 1;
                            }

                            // {H} from HMI
                            for (indx = read_index[prio]; indx <= DimCrossTable; ++indx) {
                                oper = the_IsyncRegisters[indx] & QueueOperMask;
                                addr = the_IsyncRegisters[indx] & QueueAddrMask;
                                if (oper == 0 && addr == 0) {
                                    // queue tail, jump to the array end
                                    indx = DimCrossTable;
                                } else if (CrossTable[addr].device == d && oper == READ
                                        && CrossTable[addr].Enable == (prio + 1)
                                        && CrossTable[addr].Plc == Htype
                                        && addr == CrossTable[addr].BlockBase // reading by block only
                                        && the_QdataStates[addr] != DATA_RUN) {
                                    // NB: DATA_RUN is active while processing the read,
                                    //     while DATA_OK and DATA_ERR are the possible output when finished
                                    //     do not check Qsync for QUEUE_BUSY_READ, that is an acknowledge
                                    //     and that never changes for "P,S,F" variables (and "H" while in page)
                                    found = TRUE;
                                    break;
                                }
                            }
                            if (found) {
                                QueueIndex = indx;
                                DataAddr = addr; // CrossTable[addr].BlockBase;
                                DataNumber = CrossTable[addr].BlockSize;
                                Operation = READ;
                                // data values will be available after the fieldbus access
                                // keep the index for the next loop
                                read_index[prio] = indx + 1; // may overlap DimCrossTable, it's ok
#ifdef VERBOSE_DEBUG
                                fprintf(stderr, "%s@%09u ms: read %uH [%u]@%u, will check @%u\n", theDevices[d].name, theDevices[d].current_time_ms, prio+1, DataAddr, indx, read_index[prio]);
#endif
                                break;
                            } else {
                                // compute next tic for this priority, restarting from the first
                                read_time_ms[prio] += system_ini.system.read_period_ms[prio];
                                read_index[prio] = 1;
#ifdef VERBOSE_DEBUG
                                fprintf(stderr, "%s@%09u ms: no read %uHPSF will restart at %09u ms\n", theDevices[d].name, theDevices[d].current_time_ms, prio+1, read_time_ms[prio]);
#endif
                            }
                        }
                    }
                }
            }
            pthread_mutex_unlock(&theCrosstableClientMutex);
        }

        if (DataAddr == 0) {
            // nothing to do
            continue;
        }

        // maybe either a retry or a new operation
        DataNodeId = CrossTable[DataAddr].NodeId;
        Data_node = CrossTable[DataAddr].node;

        // manage the device status (before operation)
        switch (theDevices[d].status) {
        case ZERO:
        case NO_HOPE: // FIXME: assert
            break;
        case NOT_CONNECTED:
            // try connection
            switch (theDevices[d].protocol) {
            case PLC: // FIXME: assert
                break;
            case RTU:
            case TCP:
            case TCPRTU:
                if (modbus_connect(theDevices[d].modbus_ctx) >= 0) {
                    if (modbus_flush(theDevices[d].modbus_ctx) >= 0) {
                        modbus_set_response_timeout(theDevices[d].modbus_ctx, &response_timeout);
                        changeDeviceStatus(d, CONNECTED);
                    } else {
                        modbus_close(theDevices[d].modbus_ctx);
                        changeDeviceStatus(d, DEVICE_BLACKLIST);
                        zeroDeviceVariables(d);
                    }
                } else {
                    changeDeviceStatus(d, DEVICE_BLACKLIST);
                    zeroDeviceVariables(d);
                }
                break;
            case CANOPEN: {
                u_int8_t channel = theDevices[d].u.can.bus;
                if (CANopenConfigured(channel)) {
                    changeDeviceStatus(d, CONNECTED);
                } else if (theDevices[d].elapsed_time_ms >= THE_DEVICE_SILENCE_ms) {
                    // CANopenResetChannel(theDevices[d].u.can.bus);
                    changeDeviceStatus(d, DEVICE_BLACKLIST);
                    zeroDeviceVariables(d);
                }
            }   break;
            case MECT:
                changeDeviceStatus(d, CONNECTED); // FIXME: check bus status
                break;
            case RTU_SRV:
            case TCP_SRV:
            case TCPRTU_SRV:
                changeDeviceStatus(d, CONNECTED);
                break;
            default:
                ;
            }
            break;
        case CONNECTED:
            // ok proceed with the fieldbus operations
            break;
        case CONNECTED_WITH_ERRORS:
            // ok proceed with the fieldbus operations
            break;
        case DEVICE_BLACKLIST:
            if (theDevices[d].elapsed_time_ms >= THE_DEVICE_BLACKLIST_ms) {
                changeDeviceStatus(d, NOT_CONNECTED);
            }
            break;
        default:
            ;
        }

        // can we continue?
        if (theDevices[d].status == NOT_CONNECTED || theDevices[d].status == DEVICE_BLACKLIST) {
            osSleep(THE_CONNECTION_DELAY_ms);
            continue;
        }

        // check the node status
        if (Data_node == 0xffff || theNodes[Data_node].status == BLACKLIST) {
            error = TimeoutError;
        } else {
            // the device is connected, so operate, without locking the mutex
            u_int16_t i;

            for (i = 0; i < DataNumber; ++i) {
                // CrossTable[DataAddr + i].Error = 0;
                the_QdataStates[DataAddr + i] = DATA_RUN;
            }
            XX_GPIO_CLR(5);
            switch (Operation) {
            case READ:
                error = fieldbusRead(d, DataAddr, DataValue, DataNumber);
                // fieldbusWait afterwards
                break;
            case WRITE_SINGLE:
            case WRITE_MULTIPLE:
            case WRITE_RIC_MULTIPLE:
            case WRITE_RIC_SINGLE:
                if (CrossTable[DataAddr].Output) {
                    error = fieldbusWrite(d, DataAddr, DataValue, DataNumber);
                } else {
                    error = CommError;
                }
                // fieldbusWait afterwards
                break;
            case WRITE_PREPARE:
                error = NoError;
                break; // nop
            default:
                ;
            }
            XX_GPIO_SET(5);
            // check error and set values and flags
            pthread_mutex_lock(&theCrosstableClientMutex);
            {
                u_int16_t i;

#ifdef VERBOSE_DEBUG
                fprintf(stderr, "%s@%09u ms: %s %s %u vars @ %u\n", theDevices[d].name, theDevices[d].current_time_ms,
                        Operation == READ ? "read" : "write",
                        error == NoError ? "ok" : "error",
                        DataNumber, DataAddr);
#endif
                // manage the data values
                switch (error) {
                case NoError:
                    for (i = 0; i < DataNumber; ++i) {
                        CrossTable[DataAddr + i].Error = 0;
                        the_QdataStates[DataAddr + i] = DATA_OK;
                    }
                    switch (Operation) {
                    case READ:
                    case WRITE_SINGLE:
                    case WRITE_MULTIPLE:
                    case WRITE_RIC_MULTIPLE:
                    case WRITE_RIC_SINGLE:
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, DataValue[i]);
                        }
                        break;
                    case WRITE_PREPARE:
                        break; // nop
                    default:
                        ;
                    }
                    break;
                case CommError:
                    switch (theDevices[d].protocol) {
                    case PLC:       ; break; // FIXME: assert
                    case RTU:       RTUComm_ERROR_WORD = 1;     CommErrRTU |= (2 ^ DataNodeId); break;
                    case TCP:       TCPComm_ERROR_WORD = 1;     CommErrTCP |= (2 ^ DataNodeId); break;
                    case TCPRTU:    TCPRTUComm_ERROR_WORD = 1;  CommErrTCPRTU |= (2 ^ DataNodeId); break;
                    case CANOPEN:   break; // FIXME: add error flags
                    case MECT:      break; // FIXME: add error flags
                    case RTU_SRV:    break; // FIXME: add error flags
                    case TCP_SRV:    break; // FIXME: add error flags
                    case TCPRTU_SRV: break; // FIXME: add error flags
                    default:        ;
                    }
                    // no break, continue with TimeoutError case
                case TimeoutError:
                    for (i = 0; i < DataNumber; ++i) {
                       CrossTable[DataAddr + i].Error = 1;
                       the_QdataStates[DataAddr + i] = DATA_ERR;
                    }
                    switch (theDevices[d].protocol) {
                    case PLC:       ; break; // FIXME: assert
                    case RTU:       ERROR_FLAG |= ERROR_FLAG_RTU;       CounterRTU(DataNodeId) += 1; break;
                    case TCP:       ERROR_FLAG |= ERROR_FLAG_TCP;       CounterTCP(DataNodeId) += 1; break;
                    case TCPRTU:    ERROR_FLAG |= ERROR_FLAG_TCPRTU;    CounterTCPRTU(DataNodeId) += 1; break;
                    case CANOPEN:   ERROR_FLAG |= ERROR_FLAG_CANOPEN;   break; // FIXME: add error flags
                    case MECT:      ERROR_FLAG |= ERROR_FLAG_MECT;      break; // FIXME: add error flags
                    case RTU_SRV:    ERROR_FLAG |= ERROR_FLAG_RTUSRV;    break; // FIXME: add error flags
                    case TCP_SRV:    ERROR_FLAG |= ERROR_FLAG_TCPSRV;    break; // FIXME: add error flags
                    case TCPRTU_SRV: ERROR_FLAG |= ERROR_FLAG_TCPRTUSRV; break; // FIXME: add error flags
                    default:        ;
                    }
                    break;
                default:
                    ;
                }
            }
            pthread_mutex_unlock(&theCrosstableClientMutex);
        }
        // manage the node status (see also the device status)
        if (Data_node != 0xffff) {
            switch (theNodes[Data_node].status) {
            case NODE_OK:
                switch (error) {
                case NoError:
                    theNodes[Data_node].status = NODE_OK;
                    DataAddr = 0; // i.e. get next
                    break;
                case CommError:
                    switch (Operation) {
                    case READ:
                        theNodes[Data_node].status = NODE_OK;
                        DataAddr = 0; // i.e. get next
                        break;
                    case WRITE_SINGLE:
                    case WRITE_MULTIPLE:
                    case WRITE_RIC_MULTIPLE:
                    case WRITE_RIC_SINGLE:
                        theNodes[Data_node].status = NODE_OK;
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                        break;
                    case WRITE_PREPARE:
                    default:
                        ;
                    }
                    break;
                case TimeoutError:
                    theNodes[Data_node].RetryCounter = 0;
                    theNodes[Data_node].status = TIMEOUT;
                    DataAddr = DataAddr; // i.e. RETRY this immediately
                    break;
                default:
                    ;
                }
                break;
            case TIMEOUT:
                switch (error) {
                case NoError:
                    theNodes[Data_node].status = NODE_OK;
                    DataAddr = 0; // i.e. get next
                    break;
                case CommError:
                    switch (Operation) {
                    case READ:
                        theNodes[Data_node].status = NODE_OK;
                        DataAddr = 0; // i.e. get next
                        break;
                    case WRITE_SINGLE:
                    case WRITE_MULTIPLE:
                    case WRITE_RIC_MULTIPLE:
                    case WRITE_RIC_SINGLE:
                        theNodes[Data_node].status = NODE_OK;
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                        break;
                    case WRITE_PREPARE:
                    default:
                        ;
                    }
                    break;
                case TimeoutError:
                    theNodes[Data_node].RetryCounter += 1;
                    if (theNodes[Data_node].RetryCounter < system_ini.system.retries) {
                        theNodes[Data_node].status = TIMEOUT;
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                    } else {
                        theNodes[Data_node].JumpRead = system_ini.system.blacklist;
                        theNodes[Data_node].status = BLACKLIST;
                        switch (Operation) {
                        case READ:
                            zeroNodeVariables(Data_node);
                            break;
                        case WRITE_SINGLE:
                        case WRITE_MULTIPLE:
                        case WRITE_RIC_MULTIPLE:
                        case WRITE_RIC_SINGLE:
                            break;
                        case WRITE_PREPARE:
                        default:
                            ;
                        }
                        DataAddr = 0; // i.e. retry this after the others
                        sem_post(&theDevices[d].newOperations);
                    }
                    break;
                default:
                    ;
                }
                break;
            case BLACKLIST:
                // no fieldbus operations, so no error
                theNodes[Data_node].JumpRead -= 1;
                if (theNodes[Data_node].JumpRead > 0) {
                    theNodes[Data_node].status = BLACKLIST;
                    DataAddr = 0; // i.e. retry this after the others
                    sem_post(&theDevices[d].newOperations);
                } else {
                    theNodes[Data_node].status = NODE_OK;
                    DataAddr = DataAddr; // i.e. RETRY this immediately
                }
                break;
            default:
                ;
            }
        }
        // manage the device status (after operation)
        switch (theDevices[d].status) {
        case ZERO:
        case NO_HOPE:
        case NOT_CONNECTED:
            // FIXME: assert
            break;
        case CONNECTED:
            // ok proceed with the fieldbus operations
            if (error == TimeoutError) {
                changeDeviceStatus(d, CONNECTED_WITH_ERRORS);
            }
            break;
        case CONNECTED_WITH_ERRORS:
            // ok proceed with the fieldbus operations
            if (error == NoError || error == CommError) {
                changeDeviceStatus(d, CONNECTED);
            } else {
                // error == TimeoutError
                updateDeviceTiming(d);
                if (theDevices[d].elapsed_time_ms <= THE_DEVICE_SILENCE_ms) {
                    // do nothing
                } else {
                    // too much silence
                    switch (theDevices[d].protocol) {
                    case PLC:
                        // FIXME: assert
                        break;
                    case RTU:
                    case TCP:
                    case TCPRTU:
                        modbus_close(theDevices[d].modbus_ctx);
                        break;
                    case CANOPEN:
                        // CANopenResetChannel(theDevices[d].u.can.bus);
                        break;
                    case MECT:
                        // FIXME: check state
                        break;
                    case RTU_SRV:
                    case TCP_SRV:
                    case TCPRTU_SRV:
                        break;
                    default:
                        ;
                    }
                    changeDeviceStatus(d, NOT_CONNECTED);
                    XX_GPIO_CLR(5);
                    osSleep(THE_CONNECTION_DELAY_ms);
                    XX_GPIO_SET(5);
                }
            }
            break;
        case DEVICE_BLACKLIST:
            // FIXME: assert
            break;
        default:
            ;
        }
        // wait, if necessary, after fieldbus operations
        switch (theDevices[d].protocol) {
        case PLC:
            // FIXME: assert
            break;
        case RTU:
            if (theDevices[d].silence_ms > 0) {
                XX_GPIO_CLR(5);
                osSleep(theDevices[d].silence_ms);
                XX_GPIO_SET(5);
            }
            break;
        case TCP:
        case TCPRTU:
        case CANOPEN:
            // nothing to do
            break;
        case MECT:
            // FIXME: check state
            break;
        case RTU_SRV:
        case TCP_SRV:
        case TCPRTU_SRV:
            break;
        default:
            ;
        }
    }

    // thread clean
    switch (theDevices[d].protocol) {
    case PLC:
        // FIXME: assert
        break;
    case RTU:
    case TCP:
    case TCPRTU:
        if (theDevices[d].modbus_ctx != NULL) {
            modbus_close(theDevices[d].modbus_ctx);
            modbus_free(theDevices[d].modbus_ctx);
            theDevices[d].modbus_ctx = NULL;
        }
        break;
    case CANOPEN:
        CANopenStop(theDevices[d].u.can.bus);
        break;
    case MECT:
        // FIXME: check state
        break;
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        break;
    default:
        ;
    }

    // exit
    fprintf(stderr, "EXITING: %s\n", theDevices[d].name);
    theDevices[d].thread_status = EXITING;
    return arg;
}

static void *datasyncThread(void *statusAdr)
{
    // data
    int dataRecvSock = -1;
    int dataSendSock = -1;
    struct  sockaddr_in dataSendAddr;
    struct  sockaddr_in dataRecvAddr;
    // sync
    int syncRecvSock = -1;
    int syncSendSock = -1;
    struct  sockaddr_in syncSendAddr;
    struct  sockaddr_in syncRecvAddr;
    // datasync
    int threadInitOK = FALSE;
    enum threadStatus *threadStatusPtr = (enum threadStatus *)statusAdr;

    // thread init (datasync)
    osPthreadSetSched(FC_SCHED_IO_DAT, FC_PRIO_UDP_DAT); // datasyncThread
    //osPthreadSetSched(SCHED_FIFO, 0); // datasyncThread
    //osPthreadSetSched(SCHED_OTHER, FC_PRIO_UDP_DAT); // datasyncThread
    //osPthreadSetSched(SCHED_OTHER, 0); // datasyncThread
    pthread_set_mode_np(0, PTHREAD_RPIOFF); // avoid problems from the udp send calls

    // thread init (data)
    dataRecvSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (dataRecvSock != -1) {
        memset((char *)&dataRecvAddr,0,sizeof(dataRecvAddr));
        dataRecvAddr.sin_family = AF_INET;
        dataRecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        dataRecvAddr.sin_port = htons((u_short)THE_DATA_RECV_PORT);
        if (bind(dataRecvSock, (struct sockaddr *)&dataRecvAddr, sizeof(dataRecvAddr)) >= 0) {
            dataSendSock = socket(AF_INET, SOCK_DGRAM, 0);
            if (dataSendSock >= 0) {
                struct hostent *h = gethostbyname(THE_UDP_SEND_ADDR);
                if (h != NULL) {
                    memset(&dataSendAddr, 0, sizeof(dataSendAddr));
                    dataSendAddr.sin_family = h->h_addrtype;
                    memcpy((char *) &dataSendAddr.sin_addr.s_addr,
                            h->h_addr_list[0], h->h_length);
                    dataSendAddr.sin_port = htons(THE_DATA_SEND_PORT);
                    threadInitOK = TRUE;
                }
            }
        }
    }

    // thread init (sync)
    syncRecvSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (syncRecvSock != -1) {
        memset((char *)&syncRecvAddr,0,sizeof(syncRecvAddr));
        syncRecvAddr.sin_family = AF_INET;
        syncRecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        syncRecvAddr.sin_port = htons((u_short)THE_SYNC_RECV_PORT);
        if (bind(syncRecvSock, (struct sockaddr *)&syncRecvAddr, sizeof(syncRecvAddr)) >= 0) {
            syncSendSock = socket(AF_INET, SOCK_DGRAM, 0);
            if (syncSendSock >= 0) {
                struct hostent *h = gethostbyname(THE_UDP_SEND_ADDR);
                if (h != NULL) {
                    memset(&syncSendAddr, 0, sizeof(syncSendAddr));
                    syncSendAddr.sin_family = h->h_addrtype;
                    memcpy((char *) &syncSendAddr.sin_addr.s_addr,
                            h->h_addr_list[0], h->h_length);
                    syncSendAddr.sin_port = htons(THE_SYNC_SEND_PORT);
                    threadInitOK = TRUE;
                }
            }
        }
    }

    // run (datasync)
    *threadStatusPtr = RUNNING;
    while (!engineExiting) {
        if (!engineRunning || !threadInitOK) {
            osSleep(THE_UDP_TIMEOUT_ms);
        } else {
            fd_set recv_set;
            struct timeval tv;
            int rc;

            // (1) data recv, only until timeout
            FD_ZERO(&recv_set);
            FD_SET(dataRecvSock, &recv_set);
            tv.tv_sec = THE_UDP_TIMEOUT_ms / 1000;
            tv.tv_usec = (THE_UDP_TIMEOUT_ms % 1000) * 1000;
            if (select(dataRecvSock + 1, &recv_set, NULL, NULL, &tv) <= 0) {
                // timeout or error
                continue;
            }
            rc = recv(dataRecvSock, the_UdataBuffer, THE_DATA_UDP_SIZE, 0);
            if (rc != THE_DATA_UDP_SIZE) {
                // error recovery
                continue;
            }

            // (2) sync recv, only until timeout
            FD_ZERO(&recv_set);
            FD_SET(syncRecvSock, &recv_set);
            tv.tv_sec = THE_UDP_TIMEOUT_ms / 1000;
            tv.tv_usec = (THE_UDP_TIMEOUT_ms % 1000) * 1000;
            if (select(syncRecvSock + 1, &recv_set, NULL, NULL, &tv) <= 0) {
                // timeout or error
                continue;
            }
            rc = recv(syncRecvSock, the_UsyncBuffer, THE_SYNC_UDP_SIZE, 0);
            if (rc != THE_SYNC_UDP_SIZE) {
                // error recovery
                continue;
            }

            // (3) compute data sync
            pthread_mutex_lock(&theCrosstableClientMutex);
            {
                memcpy(the_IdataRegisters, the_UdataBuffer, sizeof(the_IdataRegisters));
                memcpy(the_IsyncRegisters, the_UsyncBuffer, sizeof(the_IsyncRegisters));
                PLCsync();
                memcpy(the_UdataBuffer, the_QdataRegisters, sizeof(the_UdataBuffer));
                memcpy(the_UsyncBuffer, the_QsyncRegisters, sizeof(the_UsyncBuffer));
            }
            pthread_mutex_unlock(&theCrosstableClientMutex);

            // (4) data send
            rc = sendto(dataSendSock, the_UdataBuffer, THE_DATA_UDP_SIZE, 0, (struct sockaddr *)&dataSendAddr, sizeof(struct sockaddr_in));
            if (rc != THE_DATA_UDP_SIZE) {
                // FIXME: error recovery
                fprintf(stderr,"data sendto rc=%d vs %d\n", rc, THE_DATA_UDP_SIZE);
            }                       

            // (5) sync send
            rc = sendto(syncSendSock, the_UsyncBuffer, THE_SYNC_UDP_SIZE, 0, (struct sockaddr *)&syncSendAddr, sizeof(struct sockaddr_in));
            if (rc != THE_SYNC_UDP_SIZE) {
                // FIXME: error recovery
                fprintf(stderr,"sync sendto rc=%d vs %d\n", rc, THE_SYNC_UDP_SIZE);
            }
        }
    }

    // thread clean (data)
    if (dataRecvSock != -1) {
        shutdown(dataRecvSock, SHUT_RDWR);
        close(dataRecvSock);
        dataRecvSock = -1;
    }
    if (dataSendSock != -1) {
        shutdown(dataSendSock, SHUT_RDWR);
        close(dataSendSock);
        dataSendSock = -1;
    }

    // thread clean (sync)
    if (syncRecvSock != -1) {
        shutdown(syncRecvSock, SHUT_RDWR);
        close(syncRecvSock);
        syncRecvSock = -1;
    }
    if (syncSendSock != -1) {
        shutdown(syncSendSock, SHUT_RDWR);
        close(syncSendSock);
        syncSendSock = -1;
    }

    // exit
    fprintf(stderr, "EXITING: datasyncThread\n");
    *threadStatusPtr = EXITING;
    return NULL;
}

/* ---------------------------------------------------------------------------- */

void dataEngineStart(void)
{
    // initialize
    theDataSyncThread_id = -1;
    theDataSyncThreadStatus = NOT_STARTED;

    // read the configuration file
    if (app_config_load(&system_ini)) {
        fprintf(stderr, "[%s]: Error loading config file.\n", __func__);
        system_ini_ok = FALSE;
    } else {
        system_ini_ok = TRUE;
    }
#ifdef DEBUG
    app_config_dump(&system_ini);
#endif

    // cleanup variables
    bzero(the_QdataRegisters, sizeof(the_QdataRegisters));
    bzero(the_IdataRegisters, sizeof(the_IdataRegisters));
    bzero(the_QsyncRegisters, sizeof(the_QsyncRegisters));
    bzero(the_IsyncRegisters, sizeof(the_IsyncRegisters));

    // retentive variables
#if defined(RTS_CFG_MECT_RETAIN)
    if (ptRetentive == MAP_FAILED) {
        retentive = NULL;
        fprintf(stderr, "Missing retentive file.\n");
    } else {
        retentive = (u_int32_t *)ptRetentive;
        if (lenRetentive == LAST_RETENTIVE * 4) {
            OS_MEMCPY(&the_QdataRegisters[1], retentive, LAST_RETENTIVE * 4);
        } else {
            fprintf(stderr, "Wrong retentive file size: got %u expecting %u.\n", lenRetentive, LAST_RETENTIVE * 4);
        }
    }
#endif

    // initialize data array
    PLCRevision01 = REVISION_HI;
    PLCRevision02 = REVISION_LO;
    pthread_mutex_init(&theCrosstableClientMutex, NULL);
    pthread_mutex_init(&theAlarmsEventsMutex, NULL);
    pthread_cond_init(&theAlarmsEventsCondvar, NULL);
    engineInitialized	= TRUE;

    // start the engine thread
    if (osPthreadCreate(&theEngineThread_id, NULL, &engineThread, &theEngineThreadStatus, "engine", 0) == 0) {
        do {
            osSleep(THE_CONFIG_DELAY_ms); // not sched_yield();
        } while (theEngineThreadStatus != RUNNING);
    }
    engineRunning = TRUE;
}

void dataEngineStop(void)
{
    if (!engineInitialized) {
        // SIGINT arrived before initialization
        return;
    }
    engineRunning	= FALSE;
    engineExiting	= TRUE;
#if 0
    int still_running;
    do {
        do_sleep_ms(1);
        int n;
        still_running = FALSE;
        for (n = 0; n < theDevicesNumber && ! still_running; ++n) {
            if (theDevices[n].thread_status == RUNNING) {
                still_running = TRUE;
            }
        }
        for (n = 0; n < theServersNumber && ! still_running; ++n) {
            if (theServers[n].thread_status == RUNNING) {
                still_running = TRUE;
            }
        }
    } while (theEngineThreadStatus == RUNNING
          || theDataThreadStatus == RUNNING
          || theSyncThreadStatus == RUNNING
          || still_running);
#else
    void *retval;
    int n;
    for (n = 0; n < theDevicesNumber; ++n) {
        pthread_join(theDevices[n].thread_id, &retval);
        theDevices[n].thread_id = -1;
        fprintf(stderr, "joined dev(%d)\n", n);
    }
    for (n = 0; n < theServersNumber; ++n) {
        pthread_join(theServers[n].thread_id, &retval);
        fprintf(stderr, "joined srv(%d)\n", n);
    }
    if (theDataSyncThread_id != -1) {
        pthread_join(theDataSyncThread_id, &retval);
        theDataSyncThread_id = -1;
        fprintf(stderr, "joined datasync\n");
    }
    if (theEngineThread_id != -1) {
        pthread_join(theEngineThread_id, &retval);
        theEngineThread_id = -1;
        fprintf(stderr, "joined engine\n");
    }

#endif
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
    if (pIO->I.ulSize < THE_DATA_SIZE || pIO->Q.ulSize < THE_DATA_SIZE) {
        uRes = ERR_INVALID_PARAM;
    }
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
	void *pvIsegment = (void *)(((char *)(pIO->I.pAdr + pIO->I.ulOffs)) + 4);
    void *pvQsegment = (void *)(((char *)(pIO->Q.pAdr + pIO->Q.ulOffs)) + 4);

    OS_MEMCPY(pvIsegment, the_QdataRegisters, sizeof(the_QdataRegisters)); // always Qdata
    OS_MEMCPY(pvQsegment, the_QdataRegisters, sizeof(the_QdataRegisters));

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

    if (engineInitialized) {
        if (!engineRunning) {
            fprintf(stderr, "dataNotifySet: called with no engine running\n");
            uRes = ERR_NOT_CONFIGURED;
            RETURN(uRes);
        }
        pthread_mutex_lock(&theCrosstableClientMutex);
        {
            XX_GPIO_SET(4);
			if (pNotify->uTask != 0xffffu) {
                // notify from a plc task

                // check the write copy regions
                SImageReg   *pIRs = (SImageReg *)pIO->C.pAdr;
                SImageReg   *pIR = &pIRs[pNotify->uTask];

                if (pIR->pSetQ[uIOLayer]) {
                    // write from __%Q__ segment only if changed (using the %W write flags)
                    u_int16_t addr;
                    void *pvQsegment = pIO->Q.pAdr + pIO->Q.ulOffs;
                    void *pvWsegment = pIO->W.pAdr + pIO->W.ulOffs;
                    u_int32_t *values = (u_int32_t *)pvQsegment;
                    u_int32_t *flags = (u_int32_t *)pvWsegment;
                    for (addr = 1; addr <= DimCrossTable; ++addr) {
                        if (flags[addr] != 0) {
                            if (CrossTable[addr].Protocol == PLC) {
                                // PLC only, immediate writing
                                if (CrossTable[addr].Types == BIT || CrossTable[addr].Types == BYTE_BIT
                                    || CrossTable[addr].Types == WORD_BIT || CrossTable[addr].Types == DWORD_BIT ) {
                                    // the engine seems to use only the bit#0 for bool variables
                                    if (values[addr] & 1) {
                                        writeQdataRegisters(addr, 1);
                                    } else {
                                        writeQdataRegisters(addr, 0);
                                    }
                                } else {
                                    writeQdataRegisters(addr, values[addr]);
                                }
                                flags[addr] = 0;
                            } else {
                                // RTU, TCP, TCPRTU, CANOPEN, MECT, RTU_SRV, TCP_SRV, TCPRTU_SRV
                                u_int16_t d = CrossTable[addr].device;

                                if (d != 0xffff && theDevices[d].PLCwriteRequestNumber < MaxLocalQueue) {
                                    register int base, size, type, node, n, offset;
                                    register int i = theDevices[d].PLCwriteRequestPut;

                                    theDevices[d].PLCwriteRequestNumber += 1;
                                    theDevices[d].PLCwriteRequestPut = (i + 1) % MaxLocalQueue;
                                    theDevices[d].PLCwriteRequests[i].Addr = addr;
                                    theDevices[d].PLCwriteRequests[i].Number = 1;
                                    if (CrossTable[addr].Types == BIT || CrossTable[addr].Types == BYTE_BIT
                                        || CrossTable[addr].Types == WORD_BIT || CrossTable[addr].Types == DWORD_BIT ) {
                                        // the engine seems to use only the bit#0 for bool variables
                                        if (values[addr] & 1) {
                                            theDevices[d].PLCwriteRequests[i].Values[0] = 1;
                                        } else {
                                            theDevices[d].PLCwriteRequests[i].Values[0] = 0;
                                        }
                                    } else {
                                        theDevices[d].PLCwriteRequests[i].Values[0] = values[addr];
                                    }
                                    flags[addr] = 0;
                                    // are there any other consecutive writes to the same block?
                                    base = CrossTable[addr].BlockBase;
                                    size = CrossTable[addr].BlockSize;
                                    type = CrossTable[addr].Types;
                                    offset = CrossTable[addr].Offset;
                                    node = CrossTable[addr].node;
                                    for (n = 1; (addr + n) < (base + size); ++n) {
                                        if (theDevices[d].PLCwriteRequests[i].Number >= MAX_WRITES) {
                                            break;
                                        }
                                        // must be same device and node (should already be checked by Crosstable editor)
                                        if (CrossTable[addr + n].device != d || CrossTable[addr + n].node != node) {
                                            break;
                                        }
                                        // in Modbus clients we cannot mix BIT and non BIT variables
                                        if ((type == RTU || type == TCP || type == TCPRTU)
                                            && ((type == BIT && CrossTable[addr + n].Types != BIT)
                                               || (type != BIT && CrossTable[addr + n].Types == BIT))) {
                                            break;
                                        }
                                        // only sequential writes, with the exception of *_BIT of the same offset
                                        if (flags[addr + n] == 0
                                           && !((type == BYTE_BIT || type == WORD_BIT || type == DWORD_BIT)
                                                && type == CrossTable[addr + n].Types && offset == CrossTable[addr + n].Offset)) {
                                            break;
                                        }
                                        // ok, add another one
                                        theDevices[d].PLCwriteRequests[i].Number += 1;
                                        if (flags[addr + n] != 0) {
                                            if (CrossTable[addr + n].Types == BIT || CrossTable[addr + n].Types == BYTE_BIT
                                                || CrossTable[addr + n].Types == WORD_BIT || CrossTable[addr + n].Types == DWORD_BIT ) {
                                                // the engine seems to use only the bit#0 for bool variables
                                                if (values[addr + n] & 1) {
                                                    theDevices[d].PLCwriteRequests[i].Values[n] = 1;
                                                } else {
                                                    theDevices[d].PLCwriteRequests[i].Values[n] = 0;
                                                }
                                            } else {
                                                theDevices[d].PLCwriteRequests[i].Values[n] = values[addr + n];
                                            }
                                            flags[addr + n] = 0;
                                        } else {
                                            // in the exception of *_BIT, we get the actual value
                                            theDevices[d].PLCwriteRequests[i].Values[n] = the_QdataRegisters[addr + n];
                                        }
                                    }
                                    // manage the for index
                                    if (theDevices[d].PLCwriteRequests[i].Number > 1) {
                                        addr += (theDevices[d].PLCwriteRequests[i].Number - 1);
                                    }
                                    // awake the device thread
                                    sem_post(&theDevices[d].newOperations);
                                }
                            }
                         }
                    }
                }
            } else if (pNotify->usSegment != SEG_OUTPUT) {
                uRes = ERR_WRITE_TO_INPUT;
            } else {
                // notify from others
                IEC_UDINT ulStart = vmm_max(pNotify->ulOffset, pIO->Q.ulOffs);
                IEC_UDINT ulStop = vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->Q.ulOffs + pIO->Q.ulSize);
                void * source = pIO->Q.pAdr + ulStart;
                void * dest = the_QdataRegisters;
                dest += ulStart - pIO->Q.ulOffs;
                if (pNotify->usBit == 0) {
                    if (ulStart < ulStop) {
                        OS_MEMCPY(dest, source, ulStop - ulStart);
                    }
                } else {
                    IEC_UINT uM = (IEC_UINT)(1u << (pNotify->usBit - 1u));
                    if ((*(IEC_DATA *)source) & uM) {
                        *(IEC_DATA *)dest |= uM;
                    } else {
                        *(IEC_DATA *)dest &= ~uM;
                    }
                    // FIXME: create a write request
                }
            }
            XX_GPIO_CLR(4);
        }
        pthread_mutex_unlock(&theCrosstableClientMutex);
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

    if (!engineInitialized) {
        fprintf(stderr, "dataNotifyGet: called with no engine initialized\n");
        // continue anyway
    }
    if (engineInitialized) {
        if (!engineRunning) {
            fprintf(stderr, "dataNotifyGet: called with no engine running\n");
            // continue anyway
        }
        pthread_mutex_lock(&theCrosstableClientMutex);
        {
            XX_GPIO_SET(4);
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
                        IEC_UDINT	ulStart;
                        IEC_UDINT	ulStop;
                        void * source;
                        void * dest;

                        if (pIR->pRegionRd[r].usSegment == SEG_OUTPUT) {
                            ulStart = vmm_max(pIR->pRegionRd[r].ulOffset, pIO->Q.ulOffs);
                            ulStop	= vmm_min(pIR->pRegionRd[r].ulOffset + pIR->pRegionRd[r].uSize, pIO->Q.ulOffs + pIO->Q.ulSize);
                            source = the_QdataRegisters;
                            source += ulStart - pIO->Q.ulOffs;
                            dest = pIO->Q.pAdr + ulStart;
                        } else { // pIR->pRegionRd[r].usSegment == SEG_INPUT
                            ulStart = vmm_max(pIR->pRegionRd[r].ulOffset, pIO->I.ulOffs);
                            ulStop	= vmm_min(pIR->pRegionRd[r].ulOffset + pIR->pRegionRd[r].uSize, pIO->I.ulOffs + pIO->I.ulSize);
                            source = the_QdataRegisters; // never from the_IdataRegisters
                            source += ulStart - pIO->I.ulOffs;
                            dest = pIO->I.pAdr + ulStart;
                        }
                        if (ulStart < ulStop) {
                            OS_MEMCPY(dest, source, ulStop - ulStart);
                        }
                    }
                }
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
                    source = the_QdataRegisters; // never from the_IdataRegisters
                    source += ulStart - pIO->I.ulOffs;
                    dest = pIO->I.pAdr + ulStart;
                } else { // pNotify->usSegment == SEG_OUTPUT
                    ulStart	= vmm_max(pNotify->ulOffset, pIO->Q.ulOffs);
                    ulStop	= vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->Q.ulOffs + pIO->Q.ulSize);
                    source = the_QdataRegisters;
                    source += ulStart - pIO->Q.ulOffs;
                    dest = pIO->Q.pAdr + ulStart;
                }
                if (pNotify->usBit == 0) {
                    if (ulStart < ulStop) {
                        OS_MEMCPY(dest, source, ulStop - ulStart);
                    }
                } else {
                    IEC_UINT uM = (IEC_UINT)(1u << (pNotify->usBit - 1u));
                    IEC_DATA byte = *(IEC_DATA *)dest & ~uM;
                    byte |= *(IEC_DATA *)source & uM;
                    *(IEC_DATA *)dest = byte;
                }
            }
            XX_GPIO_CLR(4);
        }
        pthread_mutex_unlock(&theCrosstableClientMutex);
    }
	RETURN(uRes);
}

#endif /* RTS_CFG_IODAT */

/* ---------------------------------------------------------------------------- */
