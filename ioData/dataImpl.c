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
#if defined(RTS_CFG_MECT_RETAIN)
#include "mectRetentive.h"
#endif

#include "libHW119.h"
#include "libModbus.h"

#define REVISION_HI  7
#define REVISION_LO  0

/* ----  Target Specific Includes:	 ------------------------------------------ */

#define MAX_SERVERS  5 // 3 RTUSRV + 1 TCPSRV + 1 TCPRTUSRV (PLC in dataMain->dataNotifySet/Get)
#define MAX_DEVICES 64 // 1 PLC + 3 RTU + n TCP + m TCPRTU + 2 CAN + 1 RTUSRV + 1 TCPSRV + 1 TCPRTUSRV
#define MAX_NODES   64 //

#define MAX_WRITES  16
#define MAX_READS   64
#define MAX_PRIORITY 3

#define THE_CONFIG_DELAY_ms     1
#define THE_ENGINE_DELAY_ms     10
#define THE_CLIENT_DELAY_ms     10
#define THE_CLIENT_uDELAY_ms    1
#define THE_SERVER_DELAY_ms     10
#define THE_CONNECTION_DELAY_ms 1000
#define THE_DEVICE_BLACKLIST_ms 4000
#define THE_DEVICE_SILENCE_ms   9999

// -------- MANAGE THREADS: DATA & SYNCRO
#define THE_UDP_TIMEOUT_ms	500
#define THE_UDP_SEND_ADDR   "127.0.0.1"

// --------
#define DimCrossTable   5472
#define DimCrossTable_2 22004
#define DimCrossTable_3 44004
#define DimCrossTable_4 (DimCrossTable_3 + 2 * DimCrossTable + 2 + 2)
#define DimAlarmsCT     1152


// -------- DATA MANAGE FROM HMI ---------------------------------------------
#define REG_DATA_NUMBER     7680 // 1+5472+(5500-5473)+5473/2+...
#define THE_DATA_SIZE       (REG_DATA_NUMBER * sizeof(u_int32_t)) // 30720 = 0x00007800 30 kiB
#define THE_DATA_UDP_SIZE   THE_DATA_SIZE
#define	THE_DATA_RECV_PORT	34903
#define	THE_DATA_SEND_PORT	34902

static u_int32_t the_IdataRegisters[REG_DATA_NUMBER]; // %I
static u_int32_t the_QdataRegisters[REG_DATA_NUMBER]; // %Q -> %M

static u_int8_t *the_QdataStates = (u_int8_t *)(&(the_QdataRegisters[22000]));
// Qdata states
#define STATO_OK            0
#define STATO_ERR           1
#define STATO_RUN           2

static u_int32_t hardware_type = 0x00000000;
// Inizializzazione dal IO layer sync
// Byte 0 can0:
//        0 no can,
//        1 tpac1006 Base
//        2 tpac1006 u315
//        3 ..
// Byte 1	can 1:
// Byte 2 modbus 0
//        0: NO modbus
//        1: tpac1007
// byte 3 modbus 1


// -------- SYNC MANAGE FROM HMI ---------------------------------------------
#define REG_SYNC_NUMBER 	6144 // 1+5472+...
#define THE_SYNC_SIZE       (REG_SYNC_NUMBER * sizeof(u_int16_t)) // 12288 = 0x00003000 12 kiB
#define THE_SYNC_UDP_SIZE   11462 // SYNCRO_SIZE_BYTE in qt_library/ATCMutility/common.h
#define	THE_SYNC_RECV_PORT	34905
#define	THE_SYNC_SEND_PORT	34904

static u_int16_t the_IsyncRegisters[REG_SYNC_NUMBER]; // %I Array delle CODE in lettura
static u_int16_t the_QsyncRegisters[REG_SYNC_NUMBER]; // %Q Array delle CODE in scrittura

#define QueueOperMask       0xE000  // BIT 15 WR EN, BIT 14 RD EN
#define QueueAddrMask       0x1FFF  // BIT 13..0 CROSSTABLE ADDRESS
// Isync Operations
#define READ                0x4000
#define WRITE_SINGLE        0x8000
#define WRITE_MULTIPLE      0xA000
#define WRITE_RIC_MULTIPLE  0xE000
#define WRITE_RIC_SINGLE    0xC000
#define WRITE_PREPARE       0x2000
// Qsync States
#define STATO_EMPTY         0
#define STATO_BUSY_WRITE    1
#define STATO_BUSY_READ     2

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
#define ERROR_FLAG_CAN          0x00000400 // bit10: errore CAN
#define ERROR_FLAG_RTUSRV       0x00000800 // bit11: errore RTUSRV
#define ERROR_FLAG_TCPSRV       0x00001000 // bit12: errore TCPSRV
#define ERROR_FLAG_TCPRTUSRV    0x00002000 // bit13: errore TCPRTUSRV
#define ERROR_FLAG_CAN_ON       0x00004000 // bit14: CAN_ON
#define ERROR_FLAG_RTUSRV_ON    0x00008000 // bit15: RTUSRV_ON
#define ERROR_FLAG_TCPSRV_ON    0x00010000 // bit16: TCPSRV_ON
#define ERROR_FLAG_TCPRTUSRV_ON 0x00020000 // bit17: TCPRTUSRV_ON

//
#ifdef __GLIBC_HAVE_LONG_LONG
#pragma message "using 64bit emulation from gcc"
#else
#error "we need 64bit emulation from gcc"
#endif
#define BlackListRTU       (*(u_int64_t *)&the_QsyncRegisters[5708])
#define CommErrRTU         (*(u_int64_t *)&the_QsyncRegisters[5712])
#define BlackListTCP       (*(u_int64_t *)&the_QsyncRegisters[5716])
#define CommErrTCP         (*(u_int64_t *)&the_QsyncRegisters[5720])
#define BlackListTCPRTU    (*(u_int64_t *)&the_QsyncRegisters[5724])
#define CommErrTCPRTU      (*(u_int64_t *)&the_QsyncRegisters[5728])

// -------- RTU CLIENT ---------------------------------------------
#define	THE_RTUC_BAUDRATE	 38400 // 230400 // 38400
#define	THE_RTUC_PARITY 	 'N'
#define	THE_RTUC_DATABIT 	 8
#define	THE_RTUC_STOPBIT 	 1

// -------- RTU SERVER ---------------------------------------------
#define REG_RTUS_NUMBER     4096 // MODBUS_MAX_READ_REGISTERS // 125.
#define THE_RTUS_SIZE       (REG_TCRS_NUMBER * sizeof(u_int16_t)) // 0x00002000 8kB
#define	THE_RTUS_DEVICE 	 "/dev/ttyS1"
#define	THE_RTUS_BAUDRATE	 38400
#define	THE_RTUS_PARITY 	 'N'
#define	THE_RTUS_DATABIT 	 8
#define	THE_RTUS_STOPBIT 	 1

// -------- TCP SERVER ---------------------------------------------
#define REG_TCPS_NUMBER     4096 // MODBUS_MAX_READ_REGISTERS // 125.
#define THE_TCPS_SIZE       (REG_TCPS_NUMBER * sizeof(u_int16_t)) // 0x00002000 8kB
#define	THE_TCPS_TCP_PORT	 502
#define	THE_TCPS_TCP_ADDR	 "127.0.0.1" // useless since modbus_tcp_listen() forces INADDR_ANY
#define	THE_TCPS_MAX_WORK	 10 // MAX CLIENTS

// -------- TCPRTU SERVER ---------------------------------------------
#define REG_TCRS_NUMBER     4096 // MODBUS_MAX_READ_REGISTERS // 125.
#define THE_TCRS_SIZE       (REG_TCRS_NUMBER * sizeof(u_int16_t)) // 0x00002000 8kB
#define	THE_TCRS_TCP_PORT	 502
#define	THE_TCRS_TCP_ADDR	 "127.0.0.1" // useless since modbus_tcp_listen() forces INADDR_ANY
#define	THE_TCRS_MAX_WORK	 10 // MAX CLIENTS

//#define RTS_CFG_DEBUG_OUTPUT
enum TableType {Crosstable_csv = 0, Alarms_csv, Commpar_csv};
enum FieldbusType {PLC = 0, RTU, TCP, TCPRTU, CAN, RTUSRV, TCPSRV, TCPRTUSRV};
enum UpdateType { Htype = 0, Ptype, Stype, Ftype};
static const char *fieldbusName[] = {"PLC", "RTU", "TCP", "TCPRTU", "CAN", "RTUSRV", "TCPSRV", "TCPRTUSRV" };

enum threadStatus  {NOT_STARTED = 0, RUNNING, EXITING};
enum DeviceStatus  {ZERO = 0, NOT_CONNECTED, CONNECTED, CONNECTED_WITH_ERRORS, DEVICE_BLACKLIST, NO_HOPE};
enum NodeStatus    {NO_NODE = 0, NODE_OK, TIMEOUT, BLACKLIST};
enum fieldbusError {NoError = 0, CommError, TimeoutError};
#undef WORD_BIT
enum varTypes {BIT = 0, BYTE_BIT, WORD_BIT, DWORD_BIT,
               UINT16BA, UINT16AB,
               INT16BA, INT16AB,
               FLOATDCBA, FLOATCDAB, FLOATABCD, FLOATBADC,
               UDINTDCBA, UDINTCDAB, UDINTABCD, UDINTBADC,
               DINTDCBA, DINTCDAB, DINTABCD, DINTBADC,
               UNKNOWN};

// manageThread: Data + Syncro
// serverThread: "RTUSRV", "TCPSRV", "TCPRTUSRV"
// clientThread: "PLC", "RTU", "TCP", "TCPRTU", "CAN","RTUSRV", "TCPSRV", "TCPRTUSRV"

static pthread_mutex_t theCrosstableClientMutex = PTHREAD_MUTEX_INITIALIZER;

#define MAX_IPADDR_LEN      17 // 123.567.901.345.
#define MAX_NUMBER_LEN      12 // -2147483648. -32768.
#define MAX_IDNAME_LEN      17 // abcdefghijklmno.
#define MAX_VARTYPE_LEN      9 // UDINTABCD.
#define MAX_PROTOCOL_LEN     9 // TCPRTUSRV.
#define MAX_DEVICE_LEN      13 // /dev/ttyUSB0.
#define MAX_THREADNAME_LEN  42 // "srv(64)TCPRTUSRV_123.567.901.345_65535"

static struct ServerStruct {
    // for serverThread
    enum FieldbusType Protocol;
    char IPAddr[MAX_IPADDR_LEN];
    u_int16_t Port;
    //
    union ServerData {
        struct {
            u_int16_t Port;
            u_int32_t BaudRate;
            char Parity;
            u_int16_t DataBit;
            u_int16_t StopBit;
        } rtusrv;
        struct {
            char IPaddr[MAX_IPADDR_LEN];
            u_int16_t Port;
        } tcpsrv;
        struct {
            char IPaddr[MAX_IPADDR_LEN];
            u_int16_t Port;
        } tcprtusrv;
    } u;
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

#define MaxLocalQueue 15
static struct ClientStruct {
    // for clientThread
    enum FieldbusType Protocol;
    char IPAddr[MAX_IPADDR_LEN];
    u_int16_t Port;
    //
    union ClientData {
        // no plc client
        struct {
            u_int16_t Port;
            u_int32_t BaudRate;
            char Parity;
            u_int16_t DataBit;
            u_int16_t StopBit;
        } rtu;
        struct {
            char IPaddr[MAX_IPADDR_LEN];
            u_int16_t Port;
        } tcp;
        struct {
            char IPaddr[MAX_IPADDR_LEN];
            u_int16_t Port;
        } tcprtu;
        struct {
            u_int16_t bus;
        } can;
    } u;
    int16_t Tmin;
    u_int16_t TimeOut;
    //
    char name[MAX_THREADNAME_LEN]; // "(64)TCPRTUSRV_123.567.901.345_65535"
    enum DeviceStatus status;
    pthread_t thread_id;
    enum threadStatus thread_status;
    sem_t newOperations;
    u_int16_t writeOperations;
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
    u_int32_t PLCWriteVal;
    u_int16_t Error;
    //
    u_int16_t device;
    u_int16_t node;
};
static struct CrossTableRecord CrossTable[1 + DimCrossTable];	 // campi sono riempiti a partire dall'indice 1

#define FRONTE_SALITA   1
#define FRONTE_DISCESA  0
struct  Alarms {
    int ALType;
    char ALTag[MAX_IDNAME_LEN];
    char ALSource[MAX_IDNAME_LEN];
    char ALCompareVar[MAX_IDNAME_LEN];
    u_int32_t ALCompareVal;
    u_int16_t ALOperator;
    u_int16_t ALFilterTime;
    u_int16_t ALFilterCount;
    u_int16_t ALError;
};
static struct Alarms ALCrossTable[1 + DimAlarmsCT]; // campi sono riempiti a partire dall'indice 1

struct CommParameters {
    char Device[MAX_DEVICE_LEN];
    char IPaddr[MAX_IPADDR_LEN];
    u_int16_t Port;
    u_int16_t BaudRate; // FIXME (limit 65535)
    char Parity;
    u_int16_t DataBit;
    u_int16_t StopBit;
    int16_t Tmin;
    u_int16_t TimeOut;
    int State;
    u_int16_t Retry;
};
static struct  CommParameters CommParameters[1 + 4];
static int16_t NumberOfFails;
static u_int8_t FailDivisor;
static int32_t read_period_ms[MAX_PRIORITY];

/* ----  Global Variables:	 -------------------------------------------------- */

static IEC_BOOL g_bInitialized	= FALSE;
static IEC_BOOL g_bConfigured	= FALSE;
static IEC_BOOL g_bRunning	= FALSE;
static IEC_BOOL g_bExiting	= FALSE;

static pthread_t theEngineThread_id = -1;
static pthread_t theDataThread_id = -1;
static pthread_t theSyncThread_id = -1;
static enum threadStatus theEngineThreadStatus = NOT_STARTED;
static enum threadStatus theDataThreadStatus = NOT_STARTED;
static enum threadStatus theSyncThreadStatus = NOT_STARTED;

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
static void *dataThread(void *statusAdr);
static void *syncroThread(void *statusAdr);
static void *serverThread(void *statusAdr);
static void *clientThread(void *statusAdr);

static int do_recv(int s, void *buffer, ssize_t len);
static int do_sendto(int s, void *buffer, ssize_t len, struct sockaddr_in *address);
static int ReadFields(int16_t Index);
static int ReadAlarmsFields(int16_t Index);
static int ReadCommFields(int16_t Index);
static int LoadXTable(enum TableType CTType);
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

// fb_HW119_ReadVarFields.st
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
    Field.MaxLen = 1;
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
            CrossTable[Index].Types = UINT16AB; // backward compatibility
        } else if (strncmp(Field.Contents, "UINTAB", Field.CurLen) == 0) {
            CrossTable[Index].Types = UINT16AB;
        } else if (strncmp(Field.Contents, "UINTBA", Field.CurLen) == 0) {
            CrossTable[Index].Types = UINT16BA;
        } else if (strncmp(Field.Contents, "INT", Field.CurLen) == 0) {
            CrossTable[Index].Types = INT16AB; // backward compatibility
        } else if (strncmp(Field.Contents, "INTAB", Field.CurLen) == 0) {
            CrossTable[Index].Types = INT16AB;
        } else if (strncmp(Field.Contents, "INTBA", Field.CurLen) == 0) {
            CrossTable[Index].Types = INT16BA;
        } else if (strncmp(Field.Contents, "UDINT", Field.CurLen) == 0) {
            CrossTable[Index].Types = UDINTABCD; // backward compatibility
        } else if (strncmp(Field.Contents, "DINT", Field.CurLen) == 0) {
            CrossTable[Index].Types = DINTABCD; // backward compatibility
        } else if (strncmp(Field.Contents, "FDCBA", Field.CurLen) == 0) {
            CrossTable[Index].Types = FLOATDCBA;
        } else if (strncmp(Field.Contents, "FCDAB", Field.CurLen) == 0) {
            CrossTable[Index].Types = FLOATCDAB;
        } else if (strncmp(Field.Contents, "FABCD", Field.CurLen) == 0) {
            CrossTable[Index].Types = FLOATABCD;
        } else if (strncmp(Field.Contents, "FBADC", Field.CurLen) == 0) {
            CrossTable[Index].Types = FLOATBADC;
        } else if (strncmp(Field.Contents, "UDINTDCBA", Field.CurLen) == 0) {
            CrossTable[Index].Types = UDINTDCBA;
        } else if (strncmp(Field.Contents, "UDINTCDAB", Field.CurLen) == 0) {
            CrossTable[Index].Types = UDINTCDAB;
        } else if (strncmp(Field.Contents, "UDINTABCD", Field.CurLen) == 0) {
            CrossTable[Index].Types = UDINTABCD;
        } else if (strncmp(Field.Contents, "UDINTBADC", Field.CurLen) == 0) {
            CrossTable[Index].Types = UDINTBADC;
        } else if (strncmp(Field.Contents, "DINTDCBA", Field.CurLen) == 0) {
            CrossTable[Index].Types = DINTDCBA;
        } else if (strncmp(Field.Contents, "DINTCDAB", Field.CurLen) == 0) {
            CrossTable[Index].Types = DINTCDAB;
        } else if (strncmp(Field.Contents, "DINTABCD", Field.CurLen) == 0) {
            CrossTable[Index].Types = DINTABCD;
        } else if (strncmp(Field.Contents, "DINTBADC", Field.CurLen) == 0) {
            CrossTable[Index].Types = DINTBADC;
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
        } else if (strncmp(Field.Contents, "CAN", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = CAN;
        } else if (strncmp(Field.Contents, "RTUSRV", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = RTUSRV;
        } else if (strncmp(Field.Contents, "TCPSRV", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = TCPSRV;
        } else if (strncmp(Field.Contents, "TCPRTUSRV", Field.CurLen) == 0) {
            CrossTable[Index].Protocol = TCPRTUSRV;
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
    Field.MaxLen = 1;
    hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
    if (param.ret_value == 0) {
        ALCrossTable[Index].ALType = atoi(Field.Contents);
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

// fb_HW119_ReadCommFields.st
static int ReadCommFields(int16_t Index)
{
    int ERR = FALSE;
    IEC_STRMAX Field = { 0, VMM_MAX_IEC_STRLEN, ""};
    HW119_GET_CROSS_TABLE_FIELD param = {(IEC_STRING *)&Field, 0};

    switch (Index) {
    case 1: // retries
        // NumberOfFails {number}
        Field.MaxLen = MAX_NUMBER_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            NumberOfFails = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // FailDivisor {number}
        Field.MaxLen = MAX_NUMBER_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            FailDivisor = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        break;
    case 2: // priorities
        // Talta {number}
        Field.MaxLen = MAX_NUMBER_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            read_period_ms[0] = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // Tmedia {number}
        Field.MaxLen = MAX_NUMBER_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            read_period_ms[1] = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // Tbassa {number}
        Field.MaxLen = MAX_NUMBER_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            read_period_ms[2] = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        break;
    case 3: // rtu
    case 4: // tcp
    case 5: // tcprtu
        // Device {text}
        Field.MaxLen = MAX_DEVICE_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            strncpy(CommParameters[Index - 2].Device, param.field->Contents, MAX_DEVICE_LEN);
        } else {
            ERR = TRUE;
        }
        // IPaddr {text}
        Field.MaxLen = MAX_IPADDR_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            strncpy(CommParameters[Index - 2].IPaddr, param.field->Contents, MAX_IPADDR_LEN);
        } else {
            ERR = TRUE;
        }
        // Port {number}
        Field.MaxLen = MAX_NUMBER_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            CommParameters[Index - 2].Port = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // BaudRate {number}
        Field.MaxLen = MAX_NUMBER_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            CommParameters[Index - 2].BaudRate = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // Parity {text}
        Field.MaxLen = 1;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            CommParameters[Index - 2].Parity = param.field->Contents[0];
        } else {
            ERR = TRUE;
        }
        // DataBit {number}
        Field.MaxLen = MAX_NUMBER_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            CommParameters[Index - 2].DataBit = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // StopBit {number}
        Field.MaxLen = MAX_NUMBER_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            CommParameters[Index - 2].StopBit = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // Tmin {number}
        Field.MaxLen = MAX_NUMBER_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
        if (param.ret_value == 0) {
            CommParameters[Index - 2].Tmin = atoi(Field.Contents);
        } else {
            ERR = TRUE;
        }
        // TimeOut {number}
        Field.MaxLen = MAX_NUMBER_LEN;
        hw119_get_cross_table_field(NULL, NULL, (unsigned char *)&param);
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

// fb_HW119_Init.st
// fb_HW119_LoadCrossTab.st
static int LoadXTable(enum TableType CTType)
{
    char *CTFile;
    int32_t CTDimension;
    int32_t index;
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
        for (index = 0; index <= DimCrossTable-1; ++index) {
            CrossTable[index].Enable = 0;
            CrossTable[index].Plc = FALSE;
            CrossTable[index].Tag[0] = UNKNOWN;
            CrossTable[index].Types = 0;
            CrossTable[index].Decimal = 0;
            CrossTable[index].Protocol = PLC;
            CrossTable[index].IPAddress[0] = '\0';
            CrossTable[index].Port = 0;
            CrossTable[index].NodeId = 0;
            CrossTable[index].Offset = 0;
            CrossTable[index].Block = 0;
            CrossTable[index].BlockSize = 0;
            CrossTable[index].Output = FALSE;
            CrossTable[index].OldVal = 0;
            CrossTable[index].Error = 1;
            CrossTable[index].device = 0xffff;
            CrossTable[index].node = 0xffff;
            the_QsyncRegisters[index + 1] = STATO_EMPTY;
            the_QdataStates[index + 1] = STATO_OK;
        }
        CrossTableState = TRUE;
        CommEnabled = FALSE;
        break;
    case Alarms_csv:
        CTFile = "/local/etc/sysconfig/Alarms.csv";
        CTDimension = DimAlarmsCT;
        for (index = 0; index <= DimAlarmsCT; ++index) {
            ALCrossTable[index].ALType = FALSE;
            ALCrossTable[index].ALTag[0] = '\0';
            ALCrossTable[index].ALSource[0] = '\0';
            ALCrossTable[index].ALCompareVar[0] = '\0';
            ALCrossTable[index].ALCompareVal = 0;
            ALCrossTable[index].ALOperator = 0;
            ALCrossTable[index].ALFilterTime = 0;
        }
        ALCrossTableState = TRUE;
        break;
    case Commpar_csv:
        CTFile = "/local/etc/sysconfig/Commpar.csv";
        CTDimension = 5;
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
    for (index = 1; index <= CTDimension; ++ index) {

        switch (CTType) {
        case Crosstable_csv:
            read_param.error = FALSE;
            break;
        case Alarms_csv:
        case Commpar_csv:
            read_param.error = TRUE;
            break;
        default:
            ;
        }        hw119_read_cross_table_record(NULL, NULL, (unsigned char *)&read_param);
        if (read_param.ret_value) {
            switch (CTType) {
            case Crosstable_csv:
                CrossTable[index].Error = 100;
                ErrorsState |= 0x02;
                break;
            case Alarms_csv:
                ALCrossTable[index].ALError = 100;
                ErrorsState |= 0x04;
                break;
            case 2:
            default:
                ;
            }
            // no ERR = TRUE;
            continue;
        }
        switch (CTType) {
        case Crosstable_csv:
            if (ReadFields(index)) {
                CrossTable[index].Error = 100;
                ErrorsState |= 0x04;
            }
            break;
        case Alarms_csv:
            if (ReadAlarmsFields(index)) {
                ALCrossTable[index].ALError = 100;
                ErrorsState |= 0x04;
            }
            break;
        case Commpar_csv:
            if (ReadCommFields(index) && (index > 1)) {
                CommParameters[index - 1].State = FALSE;
            } else {
                CommParameters[index - 1].State = TRUE;
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

// fb_HW119_AlarmsMngr.st
static void AlarmMngr(void)
{
    // FIXME: TODO
    u_int32_t Index = 0;
    int16_t SourceAddr;
    int16_t CompareAddr;
    int16_t TagAddr;
    u_int32_t  CompareVal;
    u_int32_t  tmp, tmpOld;
    int ERRFlag;
    u_int32_t ERRORVAL = 0x00000001;
    IEC_STRMAX varname;

    for (Index = 1; Index < DimAlarmsCT; ++Index) {

        if (ALCrossTable[Index].ALTag[0] == '\0') {
            // last alarm
            break;
        }
        ERRFlag = FALSE;
        varname.MaxLen = MAX_IDNAME_LEN;
        varname.CurLen = strnlen(ALCrossTable[Index].ALSource, MAX_IDNAME_LEN);
        strncpy(varname.Contents, ALCrossTable[Index].ALSource, MAX_IDNAME_LEN);
        HW119_GET_ADDR param = {(IEC_STRING *)&varname, 0 };
        hw119_get_addr(NULL, NULL, (unsigned char *)&param);
        SourceAddr = param.ret_value;
        if (SourceAddr == 0xFFFF || SourceAddr > DimCrossTable || SourceAddr == 0) {
            ERRFlag  = TRUE;
        } else {
            if (CrossTable[SourceAddr].Error > 0 && CrossTable[SourceAddr].Protocol != PLC) {
                ERRFlag  = TRUE;
            } else {
                CompareAddr = -1;
                if (ALCrossTable[Index].ALCompareVar[0] == '\0') {
                    CompareVal = ALCrossTable[Index].ALCompareVal;
                } else {
                    varname.CurLen = strnlen(ALCrossTable[Index].ALCompareVar, MAX_IDNAME_LEN);
                    strncpy(varname.Contents, ALCrossTable[Index].ALCompareVar, MAX_IDNAME_LEN);
                    hw119_get_addr(NULL, NULL, (unsigned char *)&param);
                    CompareAddr = param.ret_value;
                    if (CompareAddr == 0xffff || CompareAddr >DimCrossTable || CompareAddr == 0) {
                        ERRFlag  = TRUE;
                    } else {
                        CompareVal = the_QdataRegisters[CompareAddr];
                    }
                }
            }
            varname.CurLen = strnlen(ALCrossTable[Index].ALTag, MAX_IDNAME_LEN);
            strncpy(varname.Contents, ALCrossTable[Index].ALTag, MAX_IDNAME_LEN);
            hw119_get_addr(NULL, NULL, (unsigned char *)&param);
            TagAddr  = param.ret_value;
            if (TagAddr == 0xFFFF || TagAddr > DimCrossTable || TagAddr == 0) {
                ERRFlag  = TRUE;
            }
        }
        if (! ERRFlag) {
            tmp = ALCrossTable[Index].ALOperator & 0xFF00;
            switch (tmp) {
            case 0x100: // >
                if (the_QdataRegisters[SourceAddr] > CompareVal) {
                    if (ALCrossTable[Index].ALFilterCount == 0) {
                        the_QdataRegisters[TagAddr] = ERRORVAL;
                    } else {
                        ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterCount - 1;
                    }
                } else {
                    the_QdataRegisters[TagAddr] = 0;
                    ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterTime;
                }
                break;
            case 0x200: // >=
                if (the_QdataRegisters[SourceAddr] >= CompareVal) {
                    if (ALCrossTable[Index].ALFilterCount == 0) {
                        the_QdataRegisters[TagAddr] = ERRORVAL;
                    } else {
                        ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterCount - 1;
                    }
                } else {
                    the_QdataRegisters[TagAddr] = 0;
                    ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterTime;
                }
                break;
            case 0x300: // <
                if (the_QdataRegisters[SourceAddr] < CompareVal) {
                    if (ALCrossTable[Index].ALFilterCount == 0) {
                        the_QdataRegisters[TagAddr] = ERRORVAL;
                    } else {
                        ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterCount - 1;
                    }
                } else {
                    the_QdataRegisters[TagAddr] = 0;
                    ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterTime;
                }
                break;
            case 0x400: // <=
                if (the_QdataRegisters[SourceAddr] <= CompareVal) {
                    if (ALCrossTable[Index].ALFilterCount == 0) {
                        the_QdataRegisters[TagAddr] = ERRORVAL;
                    } else {
                        ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterCount - 1;
                    }
                } else {
                    the_QdataRegisters[TagAddr] = 0;
                    ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterTime;
                }
                break;
            case 0x500: // ==
                if (the_QdataRegisters[SourceAddr] == CompareVal) {
                    if (ALCrossTable[Index].ALFilterCount == 0) {
                        the_QdataRegisters[TagAddr] = ERRORVAL;
                    } else {
                        ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterCount - 1;
                    }
                } else {
                    the_QdataRegisters[TagAddr] = 0;
                    ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterTime;
                }
                break;
            case 0x600: // !=
                if (the_QdataRegisters[SourceAddr] != CompareVal) {
                    if (ALCrossTable[Index].ALFilterCount == 0) {
                        the_QdataRegisters[TagAddr] = ERRORVAL;
                    } else {
                        ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterCount - 1;
                    }
                } else {
                    the_QdataRegisters[TagAddr] = 0;
                    ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterTime;
                }
                break;
            default:    // bit
                tmp = the_QdataRegisters[SourceAddr] >> ((ALCrossTable[Index].ALOperator & 0x00FF) - 1);
                tmp &= 1;
                if (CompareAddr == -1) {
                    if (tmp == CompareVal) {
                        if (ALCrossTable[Index].ALFilterCount == 0) {
                            the_QdataRegisters[TagAddr] = ERRORVAL;
                        } else {
                            ALCrossTable[Index].ALFilterCount -= 1;
                        }
                    } else {
                        the_QdataRegisters[TagAddr] = 0;
                        ALCrossTable[Index].ALFilterCount = ALCrossTable[Index].ALFilterTime;
                    }
                } else if (CompareAddr == SourceAddr) {
                    tmpOld = CrossTable[SourceAddr].OldVal >> ((ALCrossTable[Index].ALOperator & 0x00FF) - 1);
                    tmpOld &= 1;
                    if (tmp == 1 && tmpOld == 0 && ALCrossTable[Index].ALCompareVal == FRONTE_SALITA) {
                        the_QdataRegisters[TagAddr] = ERRORVAL;
                    } else if (tmp == 1 && tmpOld == 1 && ALCrossTable[Index].ALCompareVal == FRONTE_DISCESA) {
                        the_QdataRegisters[TagAddr] = ERRORVAL;
                    } else if (tmp == 0 && ALCrossTable[Index].ALCompareVal == FRONTE_SALITA) {
                        the_QdataRegisters[TagAddr] = 0;
                    } else if (tmp == 1 && ALCrossTable[Index].ALCompareVal == FRONTE_DISCESA) {
                        the_QdataRegisters[TagAddr] = 0;
                    }
                    if (tmp == 0) {
                        CrossTable[SourceAddr].OldVal &= ~(2 ^ ((ALCrossTable[Index].ALOperator & 0x0020) - 1));
                    } else {
                        CrossTable[SourceAddr].OldVal |=  (2 ^ ((ALCrossTable[Index].ALOperator & 0x0020) - 1));
                    }
                }
            }
        }
    }
}

// fb_HW119_PLCsync.st (NB: NOW IT'S CALLED BY SYNCRO)
static void PLCsync(void)
{
    u_int16_t indx;
    u_int16_t RW;
    int16_t addr;
    u_int16_t i;

    // already in pthread_mutex_lock(&theCrosstableClientMutex)
    for (indx = 1; indx <= DimCrossTable; ++indx) {
        RW = the_IsyncRegisters[indx] & QueueOperMask;
        addr = the_IsyncRegisters[indx] & QueueAddrMask;
        if (addr < DimCrossTable) {
            switch (RW) {
            case 0:
                the_QsyncRegisters[indx] = STATO_EMPTY;
                // span the whole queue each time a new udp packet arrives
                // if (addr == 0) {
                //     indx = DimCrossTable; // queue tail
                //     break;
                // }
                break;
            case READ:
                if (the_QsyncRegisters[indx] != STATO_BUSY_READ) {
                    switch (CrossTable[addr].Protocol) {
                    case PLC:
                        // all PLC variables are always ready
                        // the_QdataRegisters[addr] = the_QdataRegisters[addr]; :)
                        break;
                    case RTU:
                    case TCP:
                    case TCPRTU:
                    case CAN:
                    case RTUSRV:
                    case TCPSRV:
                    case TCPRTUSRV:
                        the_QsyncRegisters[indx] = STATO_BUSY_READ;
                        for (i = 1; i <= (CrossTable[addr].BlockSize - 1); ++i) { // FIXME: capoblocco?
                            the_QsyncRegisters[indx + i] = STATO_BUSY_READ;
                        }
                        if (CrossTable[addr].device != 0xffff) {
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
                if (the_QsyncRegisters[indx] != STATO_BUSY_WRITE) {
                    switch (CrossTable[addr].Protocol) {
                    case PLC:
                        the_QsyncRegisters[indx] = STATO_BUSY_WRITE;
                        the_QdataRegisters[addr] = the_IdataRegisters[addr];
                        if (RW == WRITE_MULTIPLE || RW == WRITE_RIC_MULTIPLE) {
                            for (i = 1; i <= (CrossTable[addr].BlockSize - 1); ++i) { // FIXME: capoblocco?
                                the_QsyncRegisters[indx + i] = STATO_BUSY_WRITE;
                                the_QdataRegisters[addr + i] = the_IdataRegisters[addr + i];
                            }
                        }
                        break;
                    case RTU:
                    case TCP:
                    case TCPRTU:
                    case CAN:
                    case RTUSRV:
                    case TCPSRV:
                    case TCPRTUSRV:
                        the_QsyncRegisters[indx] = STATO_BUSY_WRITE;
                        if (CrossTable[addr].device != 0xffff) {
                            theDevices[CrossTable[addr].device].writeOperations += 1;
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

// fb_TPAC_tic.st
// fb_TPAC1006_LIOsync.st
// fb_TPAC1007_LIOsync.st
static void LocalIO(void)
{
    // TICtimer
    float PLC_time, PLC_timeMin, PLC_timeMax;
    u_int32_t tic_ms;

    tic_ms = osGetTime32Ex() % (86400 * 1000); // 1 day overflow
    PLC_time = tic_ms / 1000.0;
    if (PLC_time <= 10.0) {
        PLC_timeMin = 0;
        PLC_timeMax = 10.0;
    } else {
        PLC_timeMin = PLC_time - 10.0;
        PLC_timeMax = PLC_time;
    }
    // PLC_time         AT %QD0.21560: REAL; 5390
    // PLC_timeMin      AT %QD0.21564: REAL; 5391
    // PLC_timeMax      AT %QD0.21568: REAL; 5392
    // PLC_hardwareType AT %QD0.21572:UDINT; 5393
    memcpy(&the_QdataRegisters[5390], &PLC_time, sizeof(u_int32_t));
    memcpy(&the_QdataRegisters[5391], &PLC_timeMin, sizeof(u_int32_t));
    memcpy(&the_QdataRegisters[5392], &PLC_timeMax, sizeof(u_int32_t));
    the_QdataRegisters[5393] = hardware_type;

    // TPAC1006, TPAC1008
    if (hardware_type & 0x000000FF) {
        // TPAC1006_LIOsync();
    }
    // TPAC1007
    if (hardware_type & 0x00FF0000) {
        // TPAC1007_LIOsync();
    }

}

// fb_HW119_ErrorMng.st
static void ErrorMNG(void)
{
    // empty
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
        if (block > 0) {
            CrossTable[i].BlockBase = base;
        } else {
            CrossTable[i].BlockBase = 0;
        }
        if (CrossTable[i].Enable > 0) {

            // server variables =---> enable the server thread
            switch (CrossTable[i].Protocol) {
            case PLC:
                // no plc client
                break;
            case RTU:
            case TCP:
            case TCPRTU:
                // nothing to do for client
                break;
            case CAN: // FIXME
                break;
            case RTUSRV:
            case TCPSRV:
            case TCPRTUSRV: {
                u_int16_t s;
                // add unique variable's server
                for (s = 0; s < theServersNumber; ++s) {
                    if (CrossTable[i].Protocol == theServers[s].Protocol
                     && strncmp(CrossTable[i].IPAddress, theServers[s].IPAddr, MAX_IPADDR_LEN) == 0
                     && CrossTable[i].Port == theServers[s].Port) {
                        // already present
                        break;
                    }
                }
                if (s < theServersNumber) {
                    // ok already present
                } else if (theServersNumber >= MAX_SERVERS) {
                    // FIXME: error
                } else {
                    // new server entry
                    ++theServersNumber;
                    theServers[s].Protocol = CrossTable[i].Protocol;
                    strncpy(theServers[s].IPAddr, CrossTable[i].IPAddress, MAX_IPADDR_LEN);
                    theServers[s].Port = CrossTable[i].Port;
                    switch (theServers[s].Protocol) {
                    case PLC:
                    case RTU:
                    case TCP:
                    case TCPRTU:
                    case CAN:
                        // FIXME: assert
                        break;
                    case RTUSRV:
                        theServers[s].u.rtusrv.Port = CrossTable[i].Port;
                        switch (theServers[s].u.rtusrv.Port) {
                        case 0:
                            theServers[s].u.rtusrv.BaudRate = modbus0_cfg.serial_cfg.baud;
                            switch (modbus0_cfg.serial_cfg.parity) {
                            case 0: theServers[s].u.rtusrv.Parity = 'N'; break;
                            case 1: theServers[s].u.rtusrv.Parity = 'O'; break;
                            case 2: theServers[s].u.rtusrv.Parity = 'E'; break;
                            default:
                                ;
                            }
                            theServers[s].u.rtusrv.DataBit = modbus0_cfg.serial_cfg.databits;
                            theServers[s].u.rtusrv.StopBit = modbus0_cfg.serial_cfg.stopbits;
                            break;
                        case 1:
                            theServers[s].u.rtusrv.BaudRate = modbus1_cfg.serial_cfg.baud;
                            switch (modbus1_cfg.serial_cfg.parity) {
                            case 0: theServers[s].u.rtusrv.Parity = 'N'; break;
                            case 1: theServers[s].u.rtusrv.Parity = 'O'; break;
                            case 2: theServers[s].u.rtusrv.Parity = 'E'; break;
                            default:
                                ;
                            }
                            theServers[s].u.rtusrv.DataBit = modbus1_cfg.serial_cfg.databits;
                            theServers[s].u.rtusrv.StopBit = modbus1_cfg.serial_cfg.stopbits;
                            break;
                        case 3:
                            theServers[s].u.rtusrv.BaudRate = THE_RTUS_BAUDRATE;
                            theServers[s].u.rtusrv.Parity = THE_RTUS_PARITY;
                            theServers[s].u.rtusrv.DataBit = THE_RTUS_DATABIT;
                            theServers[s].u.rtusrv.StopBit = THE_RTUS_STOPBIT;
                            break;
                        default:
                            ;
                        }
                        theServers[s].ctx = NULL;
                        break;
                    case TCPSRV:
                        strncpy(theServers[s].u.tcpsrv.IPaddr, CrossTable[i].IPAddress, MAX_IPADDR_LEN);
                        theServers[s].u.tcpsrv.Port = CrossTable[i].Port;
                        theServers[s].ctx = NULL;
                        break;
                    case TCPRTUSRV:
                        strncpy(theServers[s].u.tcprtusrv.IPaddr, CrossTable[i].IPAddress, MAX_IPADDR_LEN);
                        theServers[s].u.tcprtusrv.Port = CrossTable[i].Port;
                        theServers[s].ctx = NULL;
                        break;
                    default:
                        ;
                    }
                    theServers[s].thread_id = 0;
                    theServers[s].thread_status = NOT_STARTED;
                    // theServers[s].serverMutex = PTHREAD_MUTEX_INITIALIZER;
                    snprintf(theServers[s].name, MAX_THREADNAME_LEN, "srv[%d]%s_%s_%d", s, fieldbusName[theServers[s].Protocol], theServers[s].IPAddr, theServers[s].Port);
                }
            }   break;
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
            case CAN:
            case RTUSRV:
            case TCPSRV:
            case TCPRTUSRV: {
                u_int16_t d;
                u_int16_t n;
                u_int16_t s;
                // add unique variable's device
                for (d = 0; d < theDevicesNumber; ++d) {
                    if (CrossTable[i].Protocol == theDevices[d].Protocol
                     && strncmp(CrossTable[i].IPAddress, theDevices[d].IPAddr, MAX_IPADDR_LEN) == 0
                     && CrossTable[i].Port == theDevices[d].Port) {
                        // already present
                        break;
                    }
                }
                if (d < theDevicesNumber) {
                    CrossTable[i].device = d; // found
                } else if (theDevicesNumber >= MAX_DEVICES) {
                    CrossTable[i].device = 0xffff; // FIXME: error
                } else {
                    // new device entry
                    CrossTable[i].device = theDevicesNumber;
                    ++theDevicesNumber;
                    theDevices[d].Protocol = CrossTable[i].Protocol;
                    strncpy(theDevices[d].IPAddr, CrossTable[i].IPAddress, MAX_IPADDR_LEN);
                    theDevices[d].Port = CrossTable[i].Port;
                    switch (theDevices[d].Protocol) {
                    case PLC:
                        // FIXME: assert
                        break;
                    case RTU:
                        theDevices[d].u.rtu.Port = CrossTable[i].Port;
                        switch (theDevices[d].u.rtu.Port) {
                        case 0:
                            theDevices[d].u.rtu.BaudRate = modbus0_cfg.serial_cfg.baud;
                            switch (modbus0_cfg.serial_cfg.parity) {
                            case 0: theDevices[d].u.rtu.Parity = 'N'; break;
                            case 1: theDevices[d].u.rtu.Parity = 'O'; break;
                            case 2: theDevices[d].u.rtu.Parity = 'E'; break;
                            default:
                                ;
                            }
                            theDevices[d].u.rtu.DataBit = modbus0_cfg.serial_cfg.databits;
                            theDevices[d].u.rtu.StopBit = modbus0_cfg.serial_cfg.stopbits;
                            break;
                        case 1:
                            theDevices[d].u.rtu.BaudRate = modbus1_cfg.serial_cfg.baud;
                            switch (modbus1_cfg.serial_cfg.parity) {
                            case 0: theDevices[d].u.rtu.Parity = 'N'; break;
                            case 1: theDevices[d].u.rtu.Parity = 'O'; break;
                            case 2: theDevices[d].u.rtu.Parity = 'E'; break;
                            default:
                                ;
                            }
                            theDevices[d].u.rtu.DataBit = modbus1_cfg.serial_cfg.databits;
                            theDevices[d].u.rtu.StopBit = modbus1_cfg.serial_cfg.stopbits;
                            break;
                        case 3:
                            theDevices[d].u.rtu.BaudRate = THE_RTUC_BAUDRATE;
                            theDevices[d].u.rtu.Parity = THE_RTUC_PARITY;
                            theDevices[d].u.rtu.DataBit = THE_RTUC_DATABIT;
                            theDevices[d].u.rtu.StopBit = THE_RTUC_STOPBIT;
                            break;
                        default:
                            fprintf(stderr, "%s: bad RTU port %u for variable #%u", __func__, CrossTable[i].Port, i);
                            ; // FIXME: error
                        }
                        break;
                    case TCP:
                        strncpy(theDevices[d].u.tcp.IPaddr, CrossTable[i].IPAddress, MAX_IPADDR_LEN);
                        theDevices[d].u.tcp.Port = CrossTable[i].Port;
                        break;
                    case TCPRTU:
                        strncpy(theDevices[d].u.tcprtu.IPaddr, CrossTable[i].IPAddress, MAX_IPADDR_LEN);
                        theDevices[d].u.tcprtu.Port = CrossTable[i].Port;
                        break;
                    case CAN:
                        theDevices[d].u.can.bus = CrossTable[i].Port;
                        // FIXME: baudrate
                        break;
                    case RTUSRV:
                    case TCPSRV:
                    case TCPRTUSRV:
                        for (s = 0; s < theServersNumber; ++s) {
                            if (theServers[s].Protocol == theDevices[d].Protocol
                             && strncmp(theServers[s].IPAddr, theDevices[d].IPAddr, MAX_IPADDR_LEN) == 0
                             && theServers[s].Port == theDevices[d].Port) {
                                theDevices[d].server = s;
                                break;
                            }
                        }
                        if (theDevices[d].server == 0xffff) {
                            // FIXME
                        }
                        break;
                    default:
                        ;
                    }
                    theDevices[d].Tmin = CommParameters[RTU].Tmin; // FIXME
                    theDevices[d].TimeOut = CommParameters[RTU].TimeOut; // FIXME
                    snprintf(theDevices[d].name, MAX_THREADNAME_LEN, "dev(%d)%s_%s_%u", d, fieldbusName[theDevices[d].Protocol], theDevices[d].IPAddr, theDevices[d].Port);
                    if (theDevices[d].TimeOut == 0) {
                        theDevices[d].TimeOut = 300;
                        fprintf(stderr, "%s: TimeOut of device '%s' forced to %u ms\n", __func__, theDevices[d].name, theDevices[d].TimeOut);
                    }
                    theDevices[d].status = ZERO;
                    // theDevices[d].thread_id = 0;
                    theDevices[d].thread_status = NOT_STARTED;
                    sem_init(&theDevices[d].newOperations, 0, 0);
                    theDevices[d].writeOperations = 0;
                    theDevices[d].server = 0xffff;
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
                    CrossTable[i].node = 0xffff; // FIXME: error
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
    // thread init
    enum threadStatus *threadStatusPtr = (enum threadStatus *)statusAdr;

    pthread_mutex_lock(&theCrosstableClientMutex);
    {
        // case 0: init
        ErrorsState = 0;
        ERROR_FLAG = 0;

        // 10: READ ALARMS FILE
        if (LoadXTable(Alarms_csv)) {
            ALCrossTableState = FALSE;
            ERROR_FLAG |= ERROR_FLAG_ALARMS;
            // continue anyway
        }

        // 20: READ CROSSTABLE FILE
        if (LoadXTable(Crosstable_csv)) {
            CrossTableState = FALSE;
            ERROR_FLAG |= ERROR_FLAG_CROSSTAB;
            goto exit_initialization;
        }

        // 30: READ DEVICES FILE
        if (LoadXTable(Commpar_csv)) {
            ERROR_FLAG |= ERROR_FLAG_COMMPAR;
            goto exit_initialization;
         }

        // 40: CREATE SERVER, DEVICES AND NODES TABLES
        if (checkServersDevicesAndNodes()) {
            ERROR_FLAG |= ERROR_FLAG_COMMPAR;
            goto exit_initialization;
        }

        // 50: CREATE SERVER THREADS
        {
            int s;
            for (s = 0; s < theServersNumber; ++s) {
                theServers[s].thread_status = NOT_STARTED;
                if (osPthreadCreate(&theServers[s].thread_id, NULL, &serverThread, (void *)s, theServers[s].name, 0) == 0) {
                    do {
                        usleep(1000);
                    } while (theServers[s].thread_status != RUNNING);
                } else {
            #if defined(RTS_CFG_IO_TRACE)
                    osTrace("[%s] ERROR creating server thread %s: %s.\n", __func__, theServers[n].name, strerror(errno));
            #endif
                }
            }
        }

        // case 60: CREATE DEVICE THREADS
        {
            int d;
            for (d = 0; d < theDevicesNumber; ++d) {
                theDevices[d].thread_status = NOT_STARTED;
                if (osPthreadCreate(&theDevices[d].thread_id, NULL, &clientThread, (void *)d, theDevices[d].name, 0) == 0) {
                    do {
                        usleep(1000);
                    } while (theDevices[d].thread_status != RUNNING);
                } else {
            #if defined(RTS_CFG_IO_TRACE)
                    osTrace("[%s] ERROR creating device thread %s: %s.\n", __func__, theDevices[d].name, strerror(errno));
            #endif
                }
            }
        }

        // 70: ok
        if  (ALCrossTableState) {
            CommEnabled = TRUE;
        }

    exit_initialization:
        ERROR_FLAG |= ERROR_FLAG_CONF_END;
    }
    pthread_mutex_unlock(&theCrosstableClientMutex);

    // run
    *threadStatusPtr = RUNNING;
    while (!g_bExiting) {
        usleep(THE_ENGINE_DELAY_ms * 1000);
        if (g_bRunning) {
            pthread_mutex_lock(&theCrosstableClientMutex);
            {
                if (CommEnabled)  {
                    AlarmMngr();
                    LocalIO();
                } else {
                    ErrorMNG();
                }
            }
            pthread_mutex_unlock(&theCrosstableClientMutex);
        }
    }

    // thread clean
    // see dataNotifyStop()

    // exit
    *threadStatusPtr = EXITING;
    return NULL;
}

static u_int16_t modbusRegistersNumber(u_int16_t DataAddr, u_int16_t DataNumber)
{
    u_int16_t retval = 0, i;

    for (i = 0; i < DataNumber; ++i) {
        switch (CrossTable[DataAddr + i].Types) {
        case  DINTABCD: case  DINTDCBA: case  DINTCDAB: case  DINTBADC:
        case UDINTABCD: case UDINTDCBA: case UDINTCDAB: case UDINTBADC:
        case FLOATABCD: case FLOATDCBA: case FLOATCDAB: case FLOATBADC:
            retval += 2;
            break;
        case  INT16AB: case  INT16BA:
        case UINT16AB: case UINT16BA:
            retval += 1;
            break;
        case BIT:
            retval += 1;
            break;
        case BYTE_BIT:
        case WORD_BIT:
        case DWORD_BIT:
            retval += (CrossTable[DataAddr + i].Types == DWORD_BIT) ? 2 : 1;
            // skip the other *_BIT variables of the same offset
            while (i < DataNumber
                && CrossTable[DataAddr + i + 1].Types == CrossTable[DataAddr + i].Types
                && CrossTable[DataAddr + i + 1].Offset == CrossTable[DataAddr + i].Offset) {
                ++i;
            }
            break;
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

    switch (theDevices[d].Protocol) {
    case PLC:
        // FIXME: assert
        break;
    case RTU:
    case TCP:
    case TCPRTU:
        regs = modbusRegistersNumber(DataAddr, DataNumber);

        if (modbus_set_slave(theDevices[d].modbus_ctx, CrossTable[DataAddr].NodeId)) {
            retval = CommError;
            break;
        }
        if (CrossTable[DataAddr].Types == BIT) {
            bzero(bitRegs, sizeof(bitRegs));
            e = modbus_read_bits(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, bitRegs);
        } else if (regs == 1) {
            bzero(uintRegs, sizeof(uintRegs));
            e = modbus_read_registers(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, 1, uintRegs);
        } else if (regs > 1) {
            bzero(uintRegs, sizeof(uintRegs));
            e = modbus_read_registers(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, uintRegs);
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
            for (i = 0, r = 0; i < DataNumber && r < regs; ++i) {
                register u_int8_t *p = (u_int8_t *)&uintRegs[r];
                register u_int8_t a = p[0];
                register u_int8_t b = p[1];
                register u_int8_t c = p[2];
                register u_int8_t d = p[3];

                switch (CrossTable[DataAddr + i].Types) {
                case       BIT:
                        DataValue[i] = bitRegs[r]; r += 1;
                        break;
                case  BYTE_BIT:
                case  WORD_BIT:
                case DWORD_BIT:
                        // manage this and the other *_BIT variables of the same offset
                        do {
                            x = CrossTable[DataAddr + i].Decimal; // 1..32
                            if (x <= 16 && regs >= 1) {
                                DataValue[i] = (uintRegs[r] & (1 << (x - 1))) ? 1 : 0;
                            } else if (x <= 32 && regs == 2) {
                                DataValue[i] = (uintRegs[r + 1] & (1 << (x - 17))) ? 1 : 0;
                            } else {
                                // FIXME: assert
                            }
                        } while ((i + 1) < DataNumber
                            && CrossTable[DataAddr + (i + 1)].Types == CrossTable[DataAddr + i].Types
                            && CrossTable[DataAddr + (i + 1)].Offset == CrossTable[DataAddr + i].Offset
                            && ++i);
                        r += (CrossTable[DataAddr + i].Types == DWORD_BIT) ? 2 : 1;
                        break;
                case  UINT16AB:
                case   INT16AB:
                    DataValue[i] = uintRegs[r]; r += 1; break;
                case  UINT16BA:
                case   INT16BA:
                    DataValue[i] = b + (a << 8); r += 1; break;
                case UDINTABCD:
                case  DINTABCD:
                case FLOATABCD:
                    DataValue[i] = a + (b << 8) + (c << 16) + (d << 24); r += 2; break;
                case UDINTCDAB:
                case  DINTCDAB:
                case FLOATCDAB:

                    DataValue[i] = c + (d << 8) + (a << 16) + (b << 24); r += 2; break;
                case UDINTDCBA:
                case  DINTDCBA:
                case FLOATDCBA:
                    DataValue[i] = d + (c << 8) + (b << 16) + (a << 24); r += 2; break;
                case UDINTBADC:
                case  DINTBADC:
                case FLOATBADC:
                    DataValue[i] = b + (a << 8) + (d << 16) + (c << 24); r += 2; break;
                default:
                    ;
                }
            }
        }
        break;
    case CAN:
        device = CrossTable[DataAddr].device;
        if (device != 0xffff) {
            server = theDevices[device].server;
            if (server != 0xffff) {
                pthread_mutex_lock(&theServers[server].mutex);
                {
                    for (i = 0; i < DataNumber; ++i) {
                        register u_int16_t offset = CrossTable[DataAddr + i].Offset;
                        register u_int8_t *p = (u_int8_t *)&theServers[server].can_buffer[offset];
                        register u_int8_t a = p[0];
                        register u_int8_t b = p[1];
                        register u_int8_t c = p[2];
                        register u_int8_t d = p[3];

                        switch (CrossTable[DataAddr + i].Types) {
                        case       BIT:
                            DataValue[i] = a; break;
                        case  BYTE_BIT:
                        case  WORD_BIT:
                        case DWORD_BIT:
                            x = CrossTable[DataAddr + i].Decimal; // 1..32
                            if (x <= 8) {
                                DataValue[i] = (a & (1 << (x - 1))) ? 1 : 0;
                            } else if (x <= 16) {
                                DataValue[i] = (b & (1 << (x - 9))) ? 1 : 0;
                            } else if (x <= 24) {
                                DataValue[i] = (c & (1 << (x - 17))) ? 1 : 0;
                            } else if (x <= 32)  {
                                DataValue[i] = (d & (1 << (x - 25))) ? 1 : 0;
                            } else {
                                // FIXME: assert
                            }
                            break;
                        case  UINT16AB:
                        case   INT16AB:
                            DataValue[i] = a + (b << 8); break;
                        case  UINT16BA:
                        case   INT16BA:
                            DataValue[i] = b + (a << 8); break;
                        case UDINTABCD:
                        case  DINTABCD:
                        case FLOATABCD:
                            DataValue[i] = a + (b << 8) + (c << 16) + (d << 24); break;
                        case UDINTCDAB:
                        case  DINTCDAB:
                        case FLOATCDAB:
                            DataValue[i] = c + (d << 8) + (a << 16) + (b << 24); break;
                        case UDINTDCBA:
                        case  DINTDCBA:
                        case FLOATDCBA:
                            DataValue[i] = d + (c << 8) + (b << 16) + (a << 24); break;
                        case UDINTBADC:
                        case  DINTBADC:
                        case FLOATBADC:
                            DataValue[i] = b + (a << 8) + (d << 16) + (c << 24); break;
                        default:
                            ;
                        }
                    }
                }
                pthread_mutex_unlock(&theServers[server].mutex);
            }
        }
        break;
    case RTUSRV:
    case TCPSRV:
    case TCPRTUSRV:
        device = CrossTable[DataAddr].device;
        if (device != 0xffff) {
            server = theDevices[device].server;
            if (server != 0xffff) {
                pthread_mutex_lock(&theServers[server].mutex);
                {
                    for (i = 0; i < DataNumber; ++i) {
                        // FIXME: no byte swapping should be ok,
                        //        but what do we write in the manual?
                        DataValue[i] = theServers[server].mb_mapping->tab_registers[i];
                    }
                }
                pthread_mutex_unlock(&theServers[server].mutex);
            }
        }
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

    switch (theDevices[d].Protocol) {
    case PLC:
        // FIXME: assert
        break;
    case RTU:
    case TCP:
    case TCPRTU:
        regs = modbusRegistersNumber(DataAddr, DataNumber);

        if (modbus_set_slave(theDevices[d].modbus_ctx, CrossTable[DataAddr].NodeId)) {
            retval = CommError;
            break;
        }
        // init values
        if (CrossTable[DataAddr].Types == BIT) {
            bzero(bitRegs, sizeof(bitRegs));
        } else {
            bzero(uintRegs, sizeof(uintRegs));
            // *_BIT management
            for (i = 0, r = 0; i < DataNumber && r < regs; ++i) {
                switch (CrossTable[DataAddr + i].Types) {
                case       BIT:
                    r += 1; break;
                case  BYTE_BIT:
                case  WORD_BIT:
                case DWORD_BIT:
                    // init the buffer bits with ALL the actual bit values from the_QdataRegisters
                    {
                        register int addr, base, size;

                        base = CrossTable[DataAddr + i].BlockBase;
                        size = CrossTable[DataAddr + i].BlockSize;
                        for (addr = base; addr < (base + size); ++addr) {
                            if (CrossTable[addr].Types == CrossTable[DataAddr + i].Types
                             && CrossTable[addr].Offset == CrossTable[DataAddr + i].Offset) {
                                x = CrossTable[addr].Decimal; // 1..32
                                if (x <= 16) {
                                    uintRegs[r] |= ((the_QdataRegisters[addr] ? 1 : 0) << (x - 1));
                                } else if (x <= 32) {
                                     uintRegs[r + 1] |= ((the_QdataRegisters[addr] ? 1 : 0) << (x - 17));
                                } else {
                                     // FIXME: assert
                                }
                            }
                        }
                    }
                    r += (CrossTable[DataAddr + i].Types == DWORD_BIT) ? 2 : 1;
                    // skip the other *_BIT variables of the same offset
                    while ((i + 1) < DataNumber
                        && CrossTable[DataAddr + (i + 1)].Types == CrossTable[DataAddr + i].Types
                        && CrossTable[DataAddr + (i + 1)].Offset == CrossTable[DataAddr + i].Offset) {
                        ++i;
                    }
                    break;
                case  UINT16AB:
                case   INT16AB:
                case  UINT16BA:
                case   INT16BA:
                    r += 1; break;
                case UDINTABCD:
                case  DINTABCD:
                case FLOATABCD:
                case UDINTCDAB:
                case  DINTCDAB:
                case FLOATCDAB:
                case UDINTDCBA:
                case  DINTDCBA:
                case FLOATDCBA:
                case UDINTBADC:
                case  DINTBADC:
                case FLOATBADC:
                    r += 2; break;
                default:
                    ;
                }
            }
        }
        for (i = 0, r = 0; i < DataNumber && r < regs; ++i) {
            register u_int8_t *p = (u_int8_t *)&DataValue[i];
            register u_int8_t a = p[0];
            register u_int8_t b = p[1];
            register u_int8_t c = p[2];
            register u_int8_t d = p[3];

            switch (CrossTable[DataAddr + i].Types) {
            case       BIT:
                    bitRegs[r] = DataValue[i]; r += 1; break;
            case  BYTE_BIT:
            case  WORD_BIT:
            case DWORD_BIT:
                    // manage this and the other *_BIT variables of the same offset
                    do {
                        x = CrossTable[DataAddr + i].Decimal; // 1..32
                        if (x <= 16 && regs >= 1) {
                            uintRegs[r] |= ((DataValue[i] ? 1 : 0) << (x - 1));
                        } else if (x <= 32 && regs == 2) {
                            uintRegs[r + 1] |= ((DataValue[i] ? 1 : 0) << (x - 17));
                        } else {
                            // FIXME: assert
                        }
                    } while ((i + 1) < DataNumber
                          && CrossTable[DataAddr + (i + 1)].Types == CrossTable[DataAddr + i].Types
                          && CrossTable[DataAddr + (i + 1)].Offset == CrossTable[DataAddr + i].Offset
                          && ++i);
                    r += (CrossTable[DataAddr + i].Types == DWORD_BIT) ? 2 : 1;
                    break;
            case  UINT16AB:
            case   INT16AB:
                uintRegs[r] = DataValue[i]; r += 1; break;
            case  UINT16BA:
            case   INT16BA:
                uintRegs[r] = b + (a << 8); r += 1; break;
            case UDINTABCD:
            case  DINTABCD:
            case FLOATABCD:
                uintRegs[r] = a + (b << 8); uintRegs[r + 1] = c + (d << 8); r += 2; break;
            case UDINTCDAB:
            case  DINTCDAB:
            case FLOATCDAB:
                uintRegs[r] = c + (d << 8); uintRegs[r + 1] = a + (b << 8); r += 2; break;
            case UDINTDCBA:
            case  DINTDCBA:
            case FLOATDCBA:
                uintRegs[r] = d + (c << 8); uintRegs[r + 1] = b + (a << 8); r += 2; break;
            case UDINTBADC:
            case  DINTBADC:
            case FLOATBADC:
                uintRegs[r] = b + (a << 8); uintRegs[r + 1] = d + (c << 8); r += 2; break;
            default:
                ;
            }
        }
        if (CrossTable[DataAddr].Types == BIT) {
            e = modbus_write_bits(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, bitRegs);
        } else if (regs == 1){
            e = modbus_write_register(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, uintRegs[0]);
        } else {
            e = modbus_write_registers(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, uintRegs);
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
    case CAN:
        device = CrossTable[DataAddr].device;
        if (device != 0xffff) {
            server = theDevices[device].server;
            if (server != 0xffff) {
                pthread_mutex_lock(&theServers[server].mutex);
                {
                    for (i = 0; i < DataNumber; ++i) {
                        u_int16_t offset = CrossTable[DataAddr + i].Offset;
                        u_int8_t *p = (u_int8_t *)&DataValue[i];
                        u_int8_t a = p[0];
                        u_int8_t b = p[1];
                        u_int8_t c = p[2];
                        u_int8_t d = p[3];
                        u_int8_t * p8 = &theServers[server].can_buffer[offset];
                        u_int16_t * p16 = (u_int16_t *)&theServers[server].can_buffer[offset];
                        u_int32_t * p32 = (u_int32_t *)&theServers[server].can_buffer[offset];

                        switch (CrossTable[DataAddr + i].Types) {
                        case       BIT:
                                *p8 = a; break;
                        case  BYTE_BIT:
                            x = CrossTable[DataAddr + i].Decimal; // 1..32
                            if (x <= 8) {
                                *p8 |= (a & (1 << (x - 1))) ? 1 : 0;
                            } else {
                                // FIXME: assert
                            }
                            break;
                        case  WORD_BIT:
                            x = CrossTable[DataAddr + i].Decimal; // 1..32
                            if (x <= 8) {
                                *p16 |= (a & (1 << (x - 1))) ? 1 : 0;
                            } else if (x <= 16) {
                                *p16 |= (b & (1 << (x - 1))) ? 1 : 0;
                            } else {
                                // FIXME: assert
                            }
                            break;
                        case DWORD_BIT:
                            x = CrossTable[DataAddr + i].Decimal; // 1..32
                            if (x <= 8) {
                                *p32 |= (a & (1 << (x - 1))) ? 1 : 0;
                            } else if (x <= 16) {
                                *p32 |= (b & (1 << (x - 1))) ? 1 : 0;
                            } else if (x <= 24) {
                                *p32 |= (c & (1 << (x - 1))) ? 1 : 0;
                            } else if (x <= 32)  {
                                *p32 |= (d & (1 << (x - 1))) ? 1 : 0;
                            } else {
                                // FIXME: assert
                            }
                            break;
                        case    UINT16AB:
                        case     INT16AB:
                            *p16 = a + (b << 8); break;
                        case    UINT16BA:
                        case     INT16BA:
                            *p16 = b + (a << 8); break;
                        case UDINTABCD:
                        case  DINTABCD:
                        case FLOATABCD:
                            *p32 = a + (b << 8) + (c << 16) + (d << 24); break;
                        case UDINTCDAB:
                        case  DINTCDAB:
                        case FLOATCDAB:
                            *p32 = c + (d << 8) + (a << 16) + (b << 24); break;
                        case UDINTDCBA:
                        case  DINTDCBA:
                        case FLOATDCBA:
                            *p32 = d + (c << 8) + (b << 16) + (a << 24); break;
                        case UDINTBADC:
                        case  DINTBADC:
                        case FLOATBADC:
                            *p32 = b + (a << 8) + (d << 16) + (c << 24); break;
                        default:
                            ;
                        }
                    }
                    retval = NoError; // FIXME: check bus state
                }
                pthread_mutex_unlock(&theServers[server].mutex);
            }
        }
        break;
    case RTUSRV:
    case TCPSRV:
    case TCPRTUSRV: {
        u_int16_t device = CrossTable[DataAddr].device;
        if (device != 0xffff) {
            u_int16_t server = theDevices[device].server;
            if (server != 0xffff) {
                pthread_mutex_lock(&theServers[server].mutex);
                {
                    for (i = 0; i < DataNumber; ++i) {
                        // FIXME: no byte swapping should be ok,
                        //        but what do we write in the manual?
                        theServers[server].mb_mapping->tab_registers[i] = DataValue[i];
                    }
                }
                pthread_mutex_unlock(&theServers[server].mutex);
            }
        }
    }   break;
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
    int fdmax;
    int server_socket = -1;
    int threadInitOK = FALSE;

    // thread init
    pthread_mutex_init(&theServers[s].mutex, NULL);
    switch (theServers[s].Protocol) {
    case RTUSRV: {
        char device[VMM_MAX_PATH];

        snprintf(device, VMM_MAX_PATH, "/dev/ttySP%u", theServers[s].u.rtusrv.Port);
        modbus_ctx = modbus_new_rtu(device, theServers[s].u.rtusrv.BaudRate,
                            theServers[s].u.rtusrv.Parity, theServers[s].u.rtusrv.DataBit, theServers[s].u.rtusrv.StopBit);
        theServers[s].mb_mapping = modbus_mapping_new(0, 0, REG_RTUS_NUMBER, 0);
    }   break;
    case TCPSRV:
        modbus_ctx = modbus_new_tcp(theServers[s].u.tcpsrv.IPaddr, theServers[s].u.tcpsrv.Port);
        theServers[s].mb_mapping = modbus_mapping_new(0, 0, REG_TCPS_NUMBER, 0);
        break;
    case TCPRTUSRV:
        modbus_ctx = modbus_new_tcprtu(theServers[s].u.tcprtusrv.IPaddr, theServers[s].u.tcprtusrv.Port);
        theServers[s].mb_mapping = modbus_mapping_new(0, 0, REG_TCRS_NUMBER, 0);
        break;
    default:
        ;
    }
    if (modbus_ctx != NULL && theServers[s].mb_mapping != NULL) {
        threadInitOK = TRUE;
    }

    // run
    theServers[s].thread_status = RUNNING;
    while (!g_bExiting) {
        if (g_bRunning && threadInitOK) {
            // get file descriptor or bind and listen
            if (server_socket == -1) {
                switch (theServers[s].Protocol) {
                case RTUSRV:
                    server_socket = modbus_get_socket(modbus_ctx); // here socket is file descriptor
                    break;
                case TCPSRV:
                    server_socket = modbus_tcp_listen(modbus_ctx, THE_TCPS_MAX_WORK);
                    break;
                case TCPRTUSRV:
                    server_socket = modbus_tcprtu_listen(modbus_ctx, THE_TCRS_MAX_WORK);
                    break;
                default:
                    ;
                }
                if (server_socket >= 0) {
                    FD_ZERO(&refset);
                    FD_SET(server_socket, &refset);
                    fdmax = server_socket;
                } else {
                    usleep(THE_SERVER_DELAY_ms * 1000);
                    continue;
                }
            }
            // wait on server socket, only until timeout
            struct timeval tv;
            rdset = refset;
            tv.tv_sec = THE_SERVER_DELAY_ms / 1000;
            tv.tv_usec = THE_SERVER_DELAY_ms % 1000;
            if (select(fdmax+1, &rdset, NULL, NULL, &tv) <= 0) {
                // timeout or error
                continue;
            }
            // accept requests
            switch (theServers[s].Protocol) {
            case RTUSRV:
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
            case TCPSRV:
            case TCPRTUSRV:
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
                                printf("New connection from %s:%d on socket %d\n",
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
                            printf("Connection closed on socket %d\n", master_socket);
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
        } else {
            usleep(THE_SERVER_DELAY_ms * 1000);
        }
    }

    // thread clean
    switch (theServers[s].Protocol) {
    case RTUSRV:
    case TCPSRV:
    case TCPRTUSRV:
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
    theServers[s].thread_status = EXITING;
    return arg;
}

// RTU_Communication.st
// TCP_Communication.st
// TCPRTU_Communication.st
// fb_HW119_Check.st (no more fb_HW119_Reconnect.st)
// fb_HW119_MODBUS.st
// fb_HW119_InitComm.st
static void *clientThread(void *arg)
{
    u_int32_t d = (u_int32_t)arg;

    // device connection management
    struct timeval response_timeout;
    u_int32_t device_blacklist_ms = 0;
    u_int32_t last_good_ms;

    // device queue management by priority and periods
    u_int16_t prio;                         // priority of variables
    u_int16_t write_index;                  // current write index in queue
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

    struct timespec abstime;
    u_int32_t now_ms;
    u_int32_t this_loop_start_ms;

    // ------------------------------------------ thread init
    theDevices[d].modbus_ctx = NULL;
    // "new"
    switch (theDevices[d].Protocol) {
    case PLC: // FIXME: assert
        break;
    case RTU: {
        char device[VMM_MAX_PATH];

        snprintf(device, VMM_MAX_PATH, "/dev/ttySP%u", theDevices[d].u.rtu.Port);
        theDevices[d].modbus_ctx = modbus_new_rtu(device, theDevices[d].u.rtu.BaudRate,
                            theDevices[d].u.rtu.Parity, theDevices[d].u.rtu.DataBit, theDevices[d].u.rtu.StopBit);
    }   break;
    case TCP:
        theDevices[d].modbus_ctx = modbus_new_tcp(theDevices[d].u.tcp.IPaddr, theDevices[d].u.tcp.Port);
        break;
    case TCPRTU:
        theDevices[d].modbus_ctx = modbus_new_tcprtu(theDevices[d].u.tcprtu.IPaddr, theDevices[d].u.tcprtu.Port);
        break;
    case CAN:
        break; // FIXME: check can state
    case RTUSRV:
    case TCPSRV:
    case TCPRTUSRV:
        break;
    default:
        ;
    }
    // check the "new" result
    switch (theDevices[d].Protocol) {
    case PLC: // FIXME: assert
        break;
    case RTU:
    case TCP:
    case TCPRTU:
        if (theDevices[d].modbus_ctx == NULL) {
            theDevices[d].status = NO_HOPE;
        } else {
            theDevices[d].status = NOT_CONNECTED;
        }
        break;
    case CAN:
        theDevices[d].status = NOT_CONNECTED; // FIXME: check can state
        break;
    case RTUSRV:
    case TCPSRV:
    case TCPRTUSRV:
        theDevices[d].status = NOT_CONNECTED;
        break;
    default:
        ;
    }
    // update the error flags
    if (theDevices[d].status == NOT_CONNECTED) {
        switch (theDevices[d].Protocol) {
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
        case CAN:
            ERROR_FLAG |= ERROR_FLAG_CAN_ON;
            break;
        case RTUSRV:
            ERROR_FLAG |= ERROR_FLAG_RTUSRV_ON;
            break;
        case TCPSRV:
            ERROR_FLAG |= ERROR_FLAG_TCPSRV_ON;
            break;
        case TCPRTUSRV:
            ERROR_FLAG |= ERROR_FLAG_TCPRTUSRV_ON;
            break;
        default:
            ;
        }
    }

    // ------------------------------------------ run
    theDevices[d].thread_status = RUNNING;
    response_timeout.tv_sec = 0;
    response_timeout.tv_usec = theDevices[d].TimeOut * 1000;

    clock_gettime(CLOCK_REALTIME, &abstime);
    this_loop_start_ms = abstime.tv_sec * 1000 + abstime.tv_nsec / 1E6;
    write_index = 1;
    for (prio = 0; prio < MAX_PRIORITY; ++prio) {
        read_time_ms[prio] = this_loop_start_ms;
        read_index[prio] = 1;
    }
    last_good_ms = this_loop_start_ms;

    // start the fieldbus operations loop
    DataAddr = 0;
    while (!g_bExiting) {

        // trivial scenario
        if (!g_bRunning || theDevices[d].status == NO_HOPE) {
            usleep(THE_CONNECTION_DELAY_ms * 1000);
            continue;
        }

        // what time is it please?
        clock_gettime(CLOCK_REALTIME, &abstime);
        now_ms = abstime.tv_sec * 1000 + abstime.tv_nsec / 1E6;
        if (DataAddr == 0) {
            int rc;
            u_int32_t next_ms;

            // wait for next operation or next programmed read
            next_ms = now_ms + THE_UDP_TIMEOUT_ms;
            for (prio = 0; prio < MAX_PRIORITY; ++prio) {
                if (read_time_ms[prio] < next_ms) {
                    next_ms = read_time_ms[prio];
                }
            }
            abstime.tv_sec = (next_ms / 1000);
            abstime.tv_nsec += (next_ms % 1000) * 1000 * 1000; // ms -> ns
            do {
                rc = sem_timedwait(&theDevices[d].newOperations, &abstime);
                if (rc == -1 && errno == EINTR){
                    continue;
                } else {
                    break;
                }
            } while (TRUE);

            // what time is it please?
            clock_gettime(CLOCK_REALTIME, &abstime);
            now_ms = abstime.tv_sec * 1000 + abstime.tv_nsec / 1E6;
        }

        // manage the reset of the error flags
        switch (theDevices[d].Protocol) {
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
        case CAN:
        case RTUSRV:
        case TCPSRV:
        case TCPRTUSRV:
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
                    write_index = 1; // FIXME: "FlagPrimoCiclo"

                } else if (theDevices[d].writeOperations > 0) { // instead of "RichiestaScrittura"
                    u_int16_t indx, oper, addr;

                    // it should be there something to write from HMI to this device
                    int found = FALSE;
                    int first = TRUE;
                    for (indx = write_index;
                         first || indx != write_index;
                         indx = (indx == DimCrossTable) ? 1 : (indx + 1))
                    {
                        first = FALSE;
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
                        u_int16_t n;

                        QueueIndex = indx;
                        DataAddr = addr;
                        Operation = oper;
                        // data values from the the udp input, the *_BIT management is in fieldWrite()
                        if (oper == WRITE_SINGLE || oper == WRITE_RIC_SINGLE) {
                            DataNumber = 1;
                            DataValue[0] = the_IdataRegisters[DataAddr];
                        } else { // oper == WRITE_MULTIPLE || oper == WRITE_RIC_MULTIPLE
                            // NB the HMI programmer must prepare the write for the whole block
                            DataNumber = CrossTable[DataAddr].BlockSize; // FIXME: assert this is the block start
                            for (n = 0; n < DataNumber; ++n) {
                                DataValue[n] = the_IdataRegisters[DataAddr + n];
                            }
                        }
                        write_index = (indx == DimCrossTable) ? 1 : (indx + 1);
                    } else {
                        // FIXME: assert
                    }
                    theDevices[d].writeOperations -= 1;
                }
                if (DataAddr == 0) {
                    u_int16_t indx, oper, addr;

                    // periodic read requests (enabled from HMI)
                    // check by priority if timer is over
                    for (prio = 0; prio < MAX_PRIORITY; ++prio) {
                        if (read_time_ms[prio] <= now_ms) {

                            // is it there anything to read at this priority for this device?
                            int found = FALSE;
                            int first = TRUE;
                            for (indx = read_index[prio];
                                 first || indx != read_index[prio];
                                 indx = (indx == DimCrossTable) ? 1 : (indx + 1))
                            {
                                first = FALSE;
                                oper = the_IsyncRegisters[indx] & QueueOperMask;
                                addr = the_IsyncRegisters[indx] & QueueAddrMask;
                                if (oper == 0 && addr == 0) {
                                    // queue tail, jump to the array end
                                    indx = DimCrossTable;
                                } else if (CrossTable[addr].device == d && oper == READ
                                        && CrossTable[addr].Enable == (prio + 1)) {
                                    // NB: do not check Qsync for BUSY_READ
                                    found = TRUE;
                                    break;
                                }
                            }
                            if (found) {
                                u_int16_t n;

                                QueueIndex = indx;
                                DataAddr = addr;
                                DataNumber = CrossTable[addr].BlockSize; // FIXME: capoblocco?
                                Operation = READ;
                                for (n = 0; n < DataNumber; ++n) {
                                    // data values will be available after the fieldbus access
                                    // FIXME: let's default to the current values, just in case ...
                                    DataValue[n] = the_QdataRegisters[DataAddr + n];
                                }
                                read_index[prio] = (indx == DimCrossTable) ? 1 : (indx + 1);
                                break;
                            } else {
                                // compute next tic for this priority
                                read_time_ms[prio] = now_ms + read_period_ms[prio];
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
            switch (theDevices[d].Protocol) {
            case PLC: // FIXME: assert
                break;
            case RTU:
            case TCP:
            case TCPRTU:
                if (modbus_connect(theDevices[d].modbus_ctx) >= 0) {
                    if (modbus_flush(theDevices[d].modbus_ctx) >= 0) {
                        modbus_set_response_timeout(theDevices[d].modbus_ctx, &response_timeout);
                        theDevices[d].status = CONNECTED;
                    } else {
                        modbus_close(theDevices[d].modbus_ctx);
                        device_blacklist_ms = 0;
                        theDevices[d].status = DEVICE_BLACKLIST;
                    }
                } else {
                    device_blacklist_ms = 0;
                    theDevices[d].status = DEVICE_BLACKLIST;
                }
                break;
            case CAN:
                theDevices[d].status = CONNECTED; // FIXME: check bus status
                break;
            case RTUSRV:
            case TCPSRV:
            case TCPRTUSRV:
                theDevices[d].status = CONNECTED;
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
            device_blacklist_ms += THE_CONNECTION_DELAY_ms;
            if (device_blacklist_ms >= THE_DEVICE_BLACKLIST_ms) {
                theDevices[d].status = NOT_CONNECTED;
            }
            break;
        default:
            ;
        }

        // can we continue?
        if (theDevices[d].status == NOT_CONNECTED || theDevices[d].status == DEVICE_BLACKLIST) {
            usleep(THE_CONNECTION_DELAY_ms * 1000);
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
                the_QdataStates[DataAddr + i] = STATO_RUN;
            }
            switch (Operation) {
            case READ:
                if (QueueIndex > 0) {
                    the_QsyncRegisters[QueueIndex] = STATO_BUSY_READ;
                }
                error = fieldbusRead(d, DataAddr, DataValue, DataNumber);
                // fieldbusWait afterwards
                fprintf(stderr, "(%u) @%u #%u = %d\n", d, DataAddr, DataNumber, error);
                break;
            case WRITE_SINGLE:
            case WRITE_MULTIPLE:
            case WRITE_RIC_MULTIPLE:
            case WRITE_RIC_SINGLE:
                if (QueueIndex > 0) {
                    the_QsyncRegisters[QueueIndex] = STATO_BUSY_WRITE;
                }
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
            // check error and set values and flags
            pthread_mutex_lock(&theCrosstableClientMutex);
            {
                u_int16_t i;

                // manage the data values
                switch (error) {
                case NoError:
                    for (i = 0; i < DataNumber; ++i) {
                        CrossTable[DataAddr + i].Error = 0;
                        the_QdataStates[DataAddr + i] = STATO_OK;
                    }
                    switch (Operation) {
                    case READ:
                        for (i = 0; i < DataNumber; ++i) {
                            the_QdataRegisters[DataAddr + i] = DataValue[i];
                        }
                        break;
                    case WRITE_SINGLE:
                    case WRITE_MULTIPLE:
                    case WRITE_RIC_MULTIPLE:
                    case WRITE_RIC_SINGLE:
                        for (i = 0; i < DataNumber; ++i) {
                            if (QueueIndex == 0) {
                                the_QdataRegisters[DataAddr + i] = CrossTable[DataAddr + i].PLCWriteVal;
                            } else {
                                the_QdataRegisters[DataAddr + i] = the_IdataRegisters[DataAddr + i];
                            }
                        }
                        break;
                    case WRITE_PREPARE:
                        break; // nop
                    default:
                        ;
                    }
#if defined(RTS_CFG_MECT_RETAIN)
                {
                    // retentive memory update
                    u_int32_t *retentive = (u_int32_t *)ptRetentive;
                    u_int16_t i;

                    for (i = 0; i < DataNumber; ++i) {
                        retentive[(DataAddr - 1) + i ] = the_QdataRegisters[DataAddr + i];
                    }
                }
#endif
                    break;
                case CommError:
                    switch (theDevices[d].Protocol) {
                    case PLC:       ; break; // FIXME: assert
                    case RTU:       RTUComm_ERROR_WORD = 1;     CommErrRTU |= (2 ^ DataNodeId); break;
                    case TCP:       TCPComm_ERROR_WORD = 1;     CommErrTCP |= (2 ^ DataNodeId); break;
                    case TCPRTU:    TCPRTUComm_ERROR_WORD = 1;  CommErrTCPRTU |= (2 ^ DataNodeId); break;
                    case CAN:       break; // FIXME: add error flags
                    case RTUSRV:    break; // FIXME: add error flags
                    case TCPSRV:    break; // FIXME: add error flags
                    case TCPRTUSRV: break; // FIXME: add error flags
                    default:        ;
                    }
                    // no break, continue with TimeoutError case
                case TimeoutError:
                    for (i = 0; i < DataNumber; ++i) {
                       CrossTable[DataAddr + i].Error = 1;
                       the_QdataStates[DataAddr + i] = STATO_ERR;
                    }
                    switch (theDevices[d].Protocol) {
                    case PLC:       ; break; // FIXME: assert
                    case RTU:       ERROR_FLAG |= ERROR_FLAG_RTU;       CounterRTU(DataNodeId) += 1; break;
                    case TCP:       ERROR_FLAG |= ERROR_FLAG_TCP;       CounterTCP(DataNodeId) += 1; break;
                    case TCPRTU:    ERROR_FLAG |= ERROR_FLAG_TCPRTU;    CounterTCPRTU(DataNodeId) += 1; break;
                    case CAN:       ERROR_FLAG |= ERROR_FLAG_CAN;       break; // FIXME: add error flags
                    case RTUSRV:    ERROR_FLAG |= ERROR_FLAG_RTUSRV;    break; // FIXME: add error flags
                    case TCPSRV:    ERROR_FLAG |= ERROR_FLAG_TCPSRV;    break; // FIXME: add error flags
                    case TCPRTUSRV: ERROR_FLAG |= ERROR_FLAG_TCPRTUSRV; break; // FIXME: add error flags
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
                case CommError:
                    theNodes[Data_node].status = NODE_OK;
                    DataAddr = 0; // i.e. get next
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
                case CommError:
                    theNodes[Data_node].status = NODE_OK;
                    DataAddr = 0; // i.e. get next
                    break;
                case TimeoutError:
                    theNodes[Data_node].RetryCounter += 1;
                    if (theNodes[Data_node].RetryCounter < NumberOfFails) {
                        theNodes[Data_node].status = TIMEOUT;
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                    } else {
                        theNodes[Data_node].JumpRead = FailDivisor;
                        theNodes[Data_node].status = BLACKLIST;
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
        case NO_HOPE: // FIXME: assert
            break;
        case NOT_CONNECTED:
            // FIXME: assert
            break;
        case CONNECTED:
            // ok proceed with the fieldbus operations
            if (error == TimeoutError) {
                theDevices[d].status = CONNECTED_WITH_ERRORS;
            } else {
                last_good_ms = now_ms;
            }
            break;
        case CONNECTED_WITH_ERRORS:
            // ok proceed with the fieldbus operations
            if (error == NoError || error == CommError) {
                theDevices[d].status = CONNECTED;
                last_good_ms = now_ms;
            } else {
                // error == TimeoutError
                if ((now_ms - last_good_ms) > THE_DEVICE_SILENCE_ms) {
                    switch (theDevices[d].Protocol) {
                    case PLC:
                        // FIXME: assert
                        break;
                    case RTU:
                    case TCP:
                    case TCPRTU:
                        modbus_close(theDevices[d].modbus_ctx);
                        break;
                    case CAN:
                        // FIXME: check can state
                        break;
                    case RTUSRV:
                    case TCPSRV:
                    case TCPRTUSRV:
                        break;
                    default:
                        ;
                    }
                }
                theDevices[d].status = NOT_CONNECTED;
            }
            break;
        case DEVICE_BLACKLIST:
            // FIXME: assert
            break;
        default:
            ;
        }
        // wait, if necessary, after fieldbus operations
        switch (theDevices[d].Protocol) {
        case PLC:
            // FIXME: assert
            break;
        case RTU:
            usleep(CommParameters[0].Tmin * 1000);
            break;
        case TCP:
        case TCPRTU:
            // nothing to do
            break;
        case CAN:
            // FIXME: check can state
            break;
        case RTUSRV:
        case TCPSRV:
        case TCPRTUSRV:
            break;
        default:
            ;
        }
    }

    // thread clean
    switch (theDevices[d].Protocol) {
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
    case CAN:
        // FIXME: check can state
        break;
    case RTUSRV:
    case TCPSRV:
    case TCPRTUSRV:
        break;
    default:
        ;
    }

    // exit
    theDevices[d].thread_status = EXITING;
    return arg;
}

static void *dataThread(void *statusAdr)
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
            pthread_mutex_lock(&theCrosstableClientMutex);
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
            pthread_mutex_unlock(&theCrosstableClientMutex);
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

static void *syncroThread(void *statusAdr)
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
            pthread_mutex_lock(&theCrosstableClientMutex);
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
            pthread_mutex_unlock(&theCrosstableClientMutex);
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
    // read configuration file
    if (app_config_load(APP_CONF_CAN0)) {
        fprintf(stderr, "[%s]: Error loading config file (can0).\n", __func__);
    }
    if (app_config_load(APP_CONF_CAN1)) {
        fprintf(stderr, "[%s]: Error loading config file (can1).\n", __func__);
    }
    if (app_config_load(APP_CONF_MB0)) {
        fprintf(stderr, "[%s]: Error loading config file (mb0).\n", __func__);
    }
    if (app_config_load(APP_CONF_MB1)) {
        fprintf(stderr, "[%s]: Error loading config file (mb1).\n", __func__);
    }
    // set hardware type according to configuration file
    if (can0_cfg.enabled) {
        hardware_type |= (can0_cfg.enabled & 0xFF) << 0;	// 0x000000FF
    }
    if (can1_cfg.enabled) {
        hardware_type |= (can1_cfg.enabled & 0xFF) << 8;	// 0x0000FF00
    }
    if (modbus0_cfg.serial_cfg.enabled) {
        hardware_type |= (modbus0_cfg.serial_cfg.enabled & 0xFF) << 16;	// 0x00FF0000
    }
    if (modbus1_cfg.serial_cfg.enabled) {
        hardware_type |= (modbus1_cfg.serial_cfg.enabled & 0xFF) << 24;	// 0xFF000000
    }
#ifdef RTS_CFG_MECT_LIB // now undefined
#endif
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
    // load retentive area in %I %Q
#if defined(RTS_CFG_MECT_RETAIN)
	void *pvIsegment = (void *)(((char *)(pIO->I.pAdr + pIO->I.ulOffs)) + 4);
    //void *pvMsegment = (void *)(((char *)(pIO->M.pAdr + pIO->M.ulOffs)) + 4);
    void *pvQsegment = (void *)(((char *)(pIO->Q.pAdr + pIO->Q.ulOffs)) + 4);

    OS_MEMCPY(pvIsegment, ptRetentive, lenRetentive);
    // OS_MEMCPY(pvMsegment, ptRetentive, lenRetentive);
	OS_MEMCPY(pvQsegment, ptRetentive, lenRetentive);
    OS_MEMCPY(the_IdataRegisters, ptRetentive, lenRetentive);
    OS_MEMCPY(the_QdataRegisters, ptRetentive, lenRetentive);
#endif

    // initialize array
    PLCRevision01 = REVISION_HI;
    PLCRevision02 = REVISION_LO;
    pthread_mutex_init(&theCrosstableClientMutex, NULL);

    // start the engine, data and sync threads
    if (osPthreadCreate(&theEngineThread_id, NULL, &engineThread, &theEngineThreadStatus, "engine", 0) == 0) {
        do {
            usleep(1000);
        } while (theEngineThreadStatus != RUNNING);
    } else {
#if defined(RTS_CFG_IO_TRACE)
        osTrace("[%s] ERROR creating engine thread: %s.\n", __func__, strerror(errno));
#endif
        uRes = ERR_FB_INIT;
    }
    if (osPthreadCreate(&theDataThread_id, NULL, &dataThread, &theDataThreadStatus, "data", 0) == 0) {
        do {
            usleep(1000);
        } while (theDataThreadStatus != RUNNING);
    } else {
#if defined(RTS_CFG_IO_TRACE)
        osTrace("[%s] ERROR creating data thread: %s.\n", __func__, strerror(errno));
#endif
        uRes = ERR_FB_INIT;
    }
    if (osPthreadCreate(&theSyncThread_id, NULL, &syncroThread, &theSyncThreadStatus, "syncro", 0) == 0) {
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
    int still_running;
    do {
        usleep(1000);
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
        pthread_mutex_lock(&theCrosstableClientMutex);
        {
			if (pNotify->uTask != 0xffffu) {
                // notify from a plc task

                // check the write copy regions
                SImageReg   *pIRs = (SImageReg *)pIO->C.pAdr;
                SImageReg   *pIR = &pIRs[pNotify->uTask];

                if (pIR->pSetQ[uIOLayer]) {
                    // write from __%Q__ segment only if changed (using the %W write flags)
                    int i;
                    void *pvQsegment = pIO->Q.pAdr + pIO->Q.ulOffs;
                    void *pvWsegment = pIO->W.pAdr + pIO->W.ulOffs;
                    u_int32_t *values = (u_int32_t *)pvQsegment;
                    u_int32_t *flags = (u_int32_t *)pvWsegment;
                    for (i = 0; i < REG_DATA_NUMBER; ++i) {
                        if (flags[i] != 0) {
                            u_int16_t d = CrossTable[i].device;
                            if (d == 0xffff) {
                                if (CrossTable[i].Protocol == PLC) {
                                    flags[i] = 0;
                                    the_QdataRegisters[i] = values[i];
                                }
                            } else {
                                // RTU, TCP, TCPRTU, CAN, RTUSRV, TCPSRV, TCPRTUSRV
                                if (theDevices[d].PLCwriteRequestNumber < MaxLocalQueue) {
                                    register int base, size, n, total;

                                    flags[i] = 0; // zeroes the write flag only if can write in queue
                                    theDevices[d].PLCwriteRequests[theDevices[d].PLCwriteRequestPut].Addr = i;
                                    theDevices[d].PLCwriteRequests[theDevices[d].PLCwriteRequestPut].Values[0] = values[i];
                                    // are there any other consecutive writes to the same block?
                                    base = CrossTable[i].BlockBase;
                                    size = CrossTable[i].BlockSize;
                                    for (n = 1, total = 1; (i + n) < (base + size) && total < MAX_WRITES; ++n) {
                                        if (flags[i + n] != 0) {
                                            total += 1; // will be WRITE_MULTIPLE
                                            flags[i + n] = 0;
                                            theDevices[d].PLCwriteRequests[theDevices[d].PLCwriteRequestPut].Values[n] = values[i + n];
                                        } else {
                                            break; // only sequential writes
                                        }
                                    }
                                    theDevices[d].PLCwriteRequests[theDevices[d].PLCwriteRequestPut].Number = total;
                                    i += (total - 1);
                                    // awake the device thread
                                    sem_post(&theDevices[d].newOperations);
                                    // manage local queue
                                    theDevices[d].PLCwriteRequestNumber += 1;
                                    theDevices[d].PLCwriteRequestPut += 1;
                                    theDevices[d].PLCwriteRequestPut %= MaxLocalQueue;
                                }
                            }
                         }
                    }
#if defined(RTS_CFG_MECT_RETAIN)
                    // vedi clientThread (PLC)
#endif
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

    if (g_bRunning) {
        pthread_mutex_lock(&theCrosstableClientMutex);
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
        }
        pthread_mutex_unlock(&theCrosstableClientMutex);
    }
	RETURN(uRes);
}

#endif /* RTS_CFG_IODAT */

/* ---------------------------------------------------------------------------- */
