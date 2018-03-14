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
#include <ctype.h>

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

#include "libModbus.h"
#include "CANopen.h"

#include <native/timer.h>
#define TIMESPEC_FROM_RTIME(ts, rt) { ts.tv_sec = rt / 1000000000ULL; ts.tv_nsec = rt % 1000000000ULL; }

#define REVISION_HI  2
#define REVISION_LO  11

#if DEBUG
#undef VERBOSE_DEBUG
#endif

static int verbose_print_enabled = 0;

#if 0
// enabling FGPIO output (for clients 0,1,2,3)
#define XX_GPIO_SET_69(n) if (n <= 3) { XX_GPIO_SET(6 + n); }
#define XX_GPIO_CLR_69(n) if (n <= 3) { XX_GPIO_CLR(6 + n); }
#else
#define XX_GPIO_SET_69(n)
#define XX_GPIO_CLR_69(n)
#endif

#define PLC_time         5390
#define PLC_timeMin      5391
#define PLC_timeMax      5392
#define PLC_timeWin      5393
#define PLC_Version      5394
#define PLC_EngineStatus 5395
#define PLC_ResetValues  5396
#define PLC_buzzerOn     5397
#define PLC_PLC_Version  5398 // UINT;3;[RW] viewable in hmi menu > info
#define PLC_HMI_Version  5399 // UINT;3;[RW] viewable in hmi menu > info

#define PLC_5400         5400 // TPLC100_01_AA/AB CH0_NETRUN
#define PLC_5401         5401 // TPLC100_01_AA/AB CH0_NETGOOD
#define PLC_5402         5402 // TPLC100_01_AA/AB CH0_NETERR
#define PLC_5403         5403 // TPLC100_01_AA/AB CH0_NETRST
#define PLC_5404         5404 // TPLC100_01_AA/AB CH0_NETDIS
#define PLC_5405         5405 // TPLC100_01_AA/AB CH0_01_NODERUN
#define PLC_5406         5406 // TPLC100_01_AA/AB CH0_01_NODEGOOD
#define PLC_5407         5407 // TPLC100_01_AA/AB CH0_01_NODEERR
#define PLC_5408         5408 // TPLC100_01_AA/AB CH0_01_NODERST
#define PLC_5409         5409 // TPLC100_01_AA/AB CH0_01_NODEDIS

#define PLC_Year         5410 // [RO] 2017
#define PLC_Month        5411 // [RO] 1..12
#define PLC_Day          5412 // [RO] 1..31
#define PLC_Hours        5413 // [RO] 0..23
#define PLC_Minutes      5414 // [RO] 0..59
#define PLC_Seconds      5415 // [RO] 0..59
#define PLC_SetDate      5416 // [RW] year*65536 + month*256 + day
#define PLC_SetTime      5417 // [RW] hours*65536 + minutes*256 + seconds
#define PLC_WATCHDOGEN   5418 // BIT;;[RW] Enable Watchdog
#define PLC_WATCHDOG_ms  5419 // UDINT;0;[RW] Reset Watchdog Timer

#define PLC_5420         5420

#define PLC_5430         5430

#define PLC_BEEP_VOLUME  5435 // BYTE;0[RW] when buzzerOn
#define PLC_TOUCH_VOLUME 5436 // BYTE;0[RW] when QEvent::MouseButtonPress
#define PLC_ALARM_VOLUME 5437 // BYTE;0[RW] when alarm
#define PLC_BUZZER       5438 // UDINT;0[RW] 0x44332211 up=0x11[%] on=0x22[cs] off=0x33[cs] rep=0x44[times]
#define PLC_FastIO_Ena   5439 // UDINT;0[RW] TPAC1008_03_AX=0x000000FF TPAC1005=0x0003FF01
#define PLC_FastIO_Dir   5440 // UDINT;0[RW] TPAC1008_03_AX=0x0000000F TPAC1005=0x00020000

#define PLC_FastIO_1     5441 // BIT;;[RW] GPIO 2,14 PIN  21 SSP1_DATA0  TPAC1005=T2 TPAC1008_03_AX=FastOUT_1
#define PLC_FastIO_2     5442 // BIT;;[RW] GPIO 0,17 PIN 131 GPMI_CE1N               TPAC1008_03_AX=FastOUT_2
#define PLC_FastIO_3     5443 // BIT;;[RW] GPIO 2,12 PIN  11 SSP1_SCK                TPAC1008_03_AX=FastOUT_3
#define PLC_FastIO_4     5444 // BIT;;[RW] GPIO 3,06 PIN  78 AUART1_CTS              TPAC1008_03_AX=FastOUT_4
#define PLC_FastIO_5     5445 // BIT;;[RW] GPIO 2,20 PIN   7 SSP2_SS1                TPAC1008_03_AX=FastIN_1
#define PLC_FastIO_6     5446 // BIT;;[RW] GPIO 3,02 PIN  70 AUART0_CTS              TPAC1008_03_AX=FastIN_2
#define PLC_FastIO_7     5447 // BIT;;[RW] GPIO 3,04 PIN  81 AUART1_RX               TPAC1008_03_AX=FastIN_3
#define PLC_FastIO_8     5448 // BIT;;[RW] GPIO 3,05 PIN  65 AUART1_TX               TPAC1008_03_AX=FastIN_4

#define PLC_FastIO_9     5449 // BIT;;[RW] GPIO 2,24 PIN 286 SSP3_SCK    TPAC1005=T1 TP*=PFO
#define PLC_FastIO_10    5450 // BIT;;[RW] GPIO 2,27 PIN  15 SSP3_SS0    TPAC1005=T3
#define PLC_FastIO_11    5451 // BIT;;[RW] GPIO 2,17 PIN   1 SSP2_MOSI   TPAC1005=T4 TP*=RTC:SSP2_MOSI
#define PLC_FastIO_12    5452 // BIT;;[RW] GPIO 2,18 PIN 288 SSP2_MISO   TPAC1005=T5 TP*=RTC:SSP2_MISO
#define PLC_FastIO_13    5453 // BIT;;[RW] GPIO 2,16 PIN 280 SSP2_SCK    TPAC1005=T6 TP*=RTC:SSP2_SCK
#define PLC_FastIO_14    5454 // BIT;;[RW] GPIO 2,19 PIN   4 SSP2_SS0    TPAC1005=T7 TP*=RTC:SSP2_S0
#define PLC_FastIO_15    5455 // BIT;;[RW] GPIO 2,21 PIN  18 SSP2_SS2    TPAC1005=T8 TP*=CS
#define PLC_FastIO_16    5456 // BIT;;[RW] GPIO 2,25 PIN   9 SSP3_MOSI   TPAC1005=T9 TPAC1008*=RESET_WIFI

#define PLC_FastIO_17    5457 // BIT;;[RW] GPIO 2,26 PIN   3 SSP3_MISO   TPAC1005=T10
#define PLC_FastIO_18    5458 // BIT;;[RW] GPIO 2, 9 PIN 275 SSP0_DETECT TPAC1005=GPIO_A
#define PLC_FastIO_19    5459 // BIT;;[RW] GPIO 4,20 PIN 230 JTAG_RTCK   TPAC1005=GPIO_B
#define PLC_FastIO_20    5460
#define PLC_FastIO_21    5461
#define PLC_FastIO_22    5462
#define PLC_FastIO_23    5463
#define PLC_FastIO_24    5464

#define PLC_FastIO_25    5465
#define PLC_FastIO_26    5466
#define PLC_FastIO_27    5467
#define PLC_FastIO_28    5468
#define PLC_FastIO_29    5469
#define PLC_FastIO_30    5470
#define PLC_FastIO_31    5471
#define PLC_FastIO_32    5472


/* ----  Target Specific Includes:	 ------------------------------------------ */

#define CROSSTABLE_CSV "/local/etc/sysconfig/Crosstable.csv"

#define MAX_SERVERS  5 // 3 RTU_SRV + 1 TCP_SRV + 1 TCPRTU_SRV (PLC in dataMain->dataNotifySet/Get)
#define MAX_DEVICES 16 // 3 RTU + n TCP + m TCPRTU + 2 CANOPEN + 1 RTUSRV + 1 TCPSRV + 1 TCPRTUSRV
#define MAX_NODES   64 //

#define MAX_VALUES  64 // 16
#define MAX_PRIORITY 3

#define THE_CONFIG_DELAY_ms     10
#define THE_ENGINE_DELAY_ms     10
#define THE_SERVER_DELAY_ms     1000
#define THE_CONNECTION_DELAY_ms 1000

#define THE_DEVICE_BLACKLIST_ns  4000000000LL //  4s
#define THE_DEVICE_SILENCE_ns   20000000000LL // 20s = tpac boot time

#define THE_MAX_CLIENT_SLEEP_ns 1E9

// -------- MANAGE THREADS: DATA & SYNCRO
#define THE_UDP_PERIOD_ms	100
#define THE_UDP_TIMEOUT_ms	500
#define THE_UDP_SEND_ADDR   "127.0.0.1"

// --------
#define DimCrossTable   5472
#define DimAlarmsCT     1152
#define LAST_RETENTIVE  192

static struct system_ini system_ini;
static int system_ini_ok;

// -------- DATA MANAGE FROM HMI ---------------------------------------------
#define REG_DATA_NUMBER     7680 // 1 + 5472 + (5500-5473) + 5472 / 4 + ...
#define THE_DATA_SIZE       (REG_DATA_NUMBER * sizeof(u_int32_t)) // 30720 = 0x00007800 30 kiB
#define THE_DATA_UDP_SIZE   THE_DATA_SIZE
#define	THE_DATA_RECV_PORT	34903
#define	THE_DATA_SEND_PORT	34902

static u_int32_t the_UdataBuffer[REG_DATA_NUMBER]; // udp recv buffer
static u_int32_t the_IdataRegisters[REG_DATA_NUMBER]; // %I
static u_int32_t the_QdataRegisters[REG_DATA_NUMBER]; // %Q

static u_int8_t *the_QdataStates = (u_int8_t *)(&(the_QdataRegisters[5500])) + 1;
// Qdata states
#define DATA_OK            0    // reading/writing success
#define DATA_ERROR         1    // reading/writing failure
#define DATA_WARNING       2    // reading/writing temporary failure

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

// -------- ALL SERVERS (RTU_SRV, TCP_SRV, TCPRTU_SRV) ------------
#define REG_SRV_NUMBER      4096
#define THE_SRV_SIZE        (REG_SRV_NUMBER * sizeof(u_int16_t)) // 0x00002000 8kB
#define	THE_SRV_MAX_CLIENTS	10

//#define RTS_CFG_DEBUG_OUTPUT
enum TableType {Crosstable_csv = 0, Alarms_csv};
enum FieldbusType {PLC = 0, RTU, TCP, TCPRTU, CANOPEN, MECT, RTU_SRV, TCP_SRV, TCPRTU_SRV};
enum UpdateType { Htype = 0, Ptype, Stype, Ftype, Vtype, Xtype};
enum EventAlarm { Event = 0, Alarm};
static const char *fieldbusName[] = {"PLC", "RTU", "TCP", "TCPRTU", "CANOPEN", "MECT", "RTU_SRV", "TCP_SRV", "TCPRTU_SRV" };

enum threadStatus {NOT_STARTED = 0, RUNNING, EXITING};
enum ServerStatus {SRV_RUNNING0 = 0, SRV_RUNNING1, SRV_RUNNING2, SRV_RUNNING3, SRV_RUNNING4};
enum DeviceStatus {ZERO = 0, NOT_CONNECTED, CONNECTED, CONNECTED_WITH_ERRORS, DEVICE_BLACKLIST, NO_HOPE};
enum NodeStatus    {NO_NODE = 0, NODE_OK, TIMEOUT, BLACKLIST, DISCONNECTED};

static const char *deviceStatusName[] = {"ZERO", "NOT_CONNECTED", "CONNECTED", "CONNECTED_WITH_ERRORS", "DEVICE_BLACKLIST", "NO_HOPE" };
static const char *nodeStatusName[] = {"NO_NODE", "NODE_OK", "TIMEOUT", "BLACKLIST", "DISCONNECTED" };

enum fieldbusError {NoError = 0, CommError, TimeoutError, ConnReset};
#undef WORD_BIT
enum varTypes {BIT = 0, BYTE_BIT, WORD_BIT, DWORD_BIT,
               UINT8,
               UINT16, UINT16BA,
               INT16, INT16BA,
               REAL, REALDCBA, REALCDAB, REALBADC,
               UDINT, UDINTDCBA, UDINTCDAB, UDINTBADC,
               DINT, DINTDCBA, DINTCDAB, DINTBADC,
               UNKNOWN};

enum EngineStatus { enIdle = 0, enInitialized, enRunning, enError, enExiting };
static enum EngineStatus engineStatus = enIdle;
static pthread_mutex_t theCrosstableClientMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t theAlarmsEventsCondvar;
static sem_t newOperations[MAX_DEVICES];

#define MAX_IPADDR_LEN      17 // 123.567.901.345.
#define MAX_NUMBER_LEN      12 // -2147483648. -32768.
#define MAX_IDNAME_LEN      32 // abcdefghijklmno.
#define MAX_VARTYPE_LEN      9 // UDINTABCD.
#define MAX_PROTOCOL_LEN     9 // TCPRTUSRV.
#define MAX_DEVICE_LEN      13 // /dev/ttyUSB0.
#define MAX_THREADNAME_LEN  42 // "srv(64)TCPRTU_SRV_101.102.103.104_65535"

struct ServerStruct {
    // for serverThread
    enum FieldbusType protocol;
    u_int32_t IPaddress;
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
            u_int32_t IPaddr;
            u_int16_t port;
        } tcp_ip;
    } u;
    u_int16_t silence_ms;
    u_int16_t timeout_ms;
    u_int16_t NodeId;
    //
    char name[MAX_THREADNAME_LEN]; // "(64)TCPRTUSRV_123.567.901.345_65535"
    enum ServerStatus status;
    pthread_t thread_id;
    enum threadStatus thread_status;
    pthread_mutex_t mutex;
    modbus_t * ctx;
    modbus_mapping_t *mb_mapping;
    u_int8_t *can_buffer;
    u_int16_t diagnosticAddr;
    RTIME idle_time_ns;
    RTIME busy_time_ns;
    RTIME last_time_ns;
    RTIME last_update_ns;
} theServers[MAX_SERVERS];
static u_int16_t theServersNumber = 0;

#define MaxLocalQueue 64
struct ClientStruct {
    // for clientThread
    enum FieldbusType protocol;
    u_int32_t IPaddress;
    u_int16_t port;
    //
    u_int16_t var_num;
    struct device_var {
        u_int16_t addr;
        u_int16_t active; // 1 {P,S,F,V,X}, 0/1 {H}
    } *device_vars;
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
            u_int32_t IPaddr;
            u_int16_t port;
        } tcp_ip; // TCP, TCPRTU
        struct {
            u_int16_t bus;
            u_int32_t baudrate;
        } can;
    } u;
    int16_t silence_ms;
    u_int16_t timeout_ms;
    u_int16_t max_block_size;
    //
    char name[MAX_THREADNAME_LEN]; // "(64)TCPRTUSRV_123.567.901.345_65535"
    enum DeviceStatus status;
    pthread_t thread_id;
    enum threadStatus thread_status;
    RTIME current_time_ns;
    RTIME elapsed_time_ns;
    RTIME idle_time_ns;
    RTIME busy_time_ns;
    RTIME last_time_ns;
    RTIME last_update_ns;
    u_int16_t server; // for RTUSRV, TCPSRV, TCPRUSRV
    modbus_t * modbus_ctx; // for RTU, TCP, TCPRTU
    int mect_fd; // for MECT
    // local queue
    struct PLCwriteRequestStruct {
        u_int16_t Addr;
        u_int16_t Number;
        u_int32_t Values[MAX_VALUES];
    } PLCwriteRequests[MaxLocalQueue];
    u_int16_t PLCwriteRequestNumber;
    u_int16_t PLCwriteRequestGet;
    u_int16_t PLCwriteRequestPut;
    u_int16_t diagnosticAddr;
} theDevices[MAX_DEVICES];
static u_int16_t theDevicesNumber = 0;
static u_int16_t theTcpDevicesNumber = 0;

struct NodeStruct {
    u_int16_t device;
    u_int16_t NodeID;
    //
    enum NodeStatus status;
    int16_t retries;
    int16_t blacklist;
    u_int16_t diagnosticAddr;
} theNodes[MAX_NODES];
static u_int16_t theNodesNumber = 0;

struct CrossTableRecord {
    int16_t Enable;
    enum UpdateType Plc;
    char Tag[MAX_IDNAME_LEN];
    enum varTypes Types;
    u_int16_t Decimal;
    enum FieldbusType Protocol;
    u_int32_t IPAddress;
    u_int16_t Port;
    u_int8_t NodeId;
    u_int32_t Offset;
    u_int16_t Block;
    u_int16_t BlockBase;
    int16_t BlockSize;
    int Output;
    int16_t Counter;
    int32_t OldVal;
    int usedInAlarmsEvents;
    //
    u_int16_t device;
    u_int16_t node;
} CrossTable[1 + DimCrossTable];	 // campi sono riempiti a partire dall'indice 1

#define OPER_GREATER    41
#define OPER_GREATER_EQ 42
#define OPER_SMALLER    43
#define OPER_SMALLER_EQ 44
#define OPER_EQUAL      45
#define OPER_NOT_EQUAL  46
#define OPER_RISING     47
#define OPER_FALLING    48

#define COMP_UNSIGNED   77
#define COMP_SIGNED16   78
#define COMP_SIGNED32   79
#define COMP_FLOATING   80

struct Alarms {
    enum EventAlarm ALType;
    u_int16_t TagAddr;
    char ALSource[MAX_IDNAME_LEN];
    char ALCompareVar[MAX_IDNAME_LEN];
    u_int16_t SourceAddr;
    u_int16_t CompareAddr;
    u_int32_t ALCompareVal;
    u_int16_t ALOperator;
    u_int16_t ALFilterTime;
    u_int16_t ALFilterCount;
    u_int16_t ALError;
    int comparison;
} ALCrossTable[1 + DimAlarmsCT]; // campi sono riempiti a partire dall'indice 1
static u_int16_t lastAlarmEvent = 0;

/* ----  Global Variables:	 -------------------------------------------------- */

#if defined(RTS_CFG_MECT_RETAIN)
static u_int32_t *retentive = NULL;
#endif

static pthread_t theEngineThread_id = -1;
static pthread_t theDataSyncThread_id = -1;
static enum threadStatus theEngineThreadStatus = NOT_STARTED;
static enum threadStatus theDataSyncThreadStatus = NOT_STARTED;

static STaskInfoVMM *pVMM = NULL;

static unsigned buzzer_beep_ms = 0;
static unsigned buzzer_on_cs = 0;
static unsigned buzzer_off_cs = 0;
static unsigned buzzer_replies = 0;

static unsigned buzzer_period_tics = 0;
static unsigned buzzer_tic = 0;
static unsigned buzzer_periods = 0;


/* ----  Local Functions:	--------------------------------------------------- */

static void *engineThread(void *statusAdr);
static void *datasyncThread(void *statusAdr);
static void *serverThread(void *statusAdr);
static void *clientThread(void *statusAdr);

static int LoadXTable(void);
static void AlarmMngr(void);
static void PLCsync(void);

static void doWriteDeviceRetentives(u_int32_t d);
static unsigned doWriteVariable(unsigned addr, unsigned value, u_int32_t *values, u_int32_t *flags, unsigned addrMax);

static inline void changeDeviceStatus(u_int32_t d, enum DeviceStatus status);
static inline void changeNodeStatus(u_int32_t d, u_int16_t node, enum NodeStatus status);

static int mect_connect(unsigned devnum, unsigned baudrate, char parity, unsigned databits, unsigned stopbits, unsigned timeout_ms);
static int mect_read_ascii(int fd, unsigned node, unsigned command, float *value);
static int mect_write_ascii(int fd, unsigned node, unsigned command, float value);
static int mect_read_hexad(int fd, unsigned node, unsigned command, unsigned *value);
static int mect_write_hexad(int fd, unsigned node, unsigned command, unsigned value);
static void mect_close(int fd);

/* ----  Implementations:	--------------------------------------------------- */

void dataGetVersionInfo(char *szVersion)
{
    if (szVersion) {
        sprintf(szVersion, "v%d.%03d+ GPL", REVISION_HI, REVISION_LO);
    }
}

void dataEnableVerbosePrint(void)
{
   verbose_print_enabled = 1;
}

static inline void writeQdataRegisters(u_int16_t addr, u_int32_t value, u_int8_t status)
{
    if (addr > DimCrossTable) {
        fprintf(stderr, "writeQdataRegisters(addr=%u, value=0x%08x, status=%u): wrong addr\n", addr, value,status);
        return;
    }

    if (addr == PLC_WATCHDOGEN) {
        if (value) {
            xx_watchdog_enable();
        } else {
            xx_watchdog_disable();
        }

    } if (addr == PLC_WATCHDOG_ms) {
        xx_watchdog_reset(value);

    } else if (addr == PLC_FastIO_Ena) {
        register unsigned n;

        for (n = 0; n < XX_GPIO_MAX; ++n) {
            if (value & (1 << n)) {
                xx_gpio_enable(n);
            }
        }

    } else if (addr == PLC_FastIO_Dir) {
        register unsigned n;

        for (n = 0; n < XX_GPIO_MAX; ++n) {

            // NB filtered on enabled in xx_gpio_config
            if (value & (1 << n)) {
                xx_gpio_config(n, 1);
            } else {
                xx_gpio_config(n, 0);
            }
        }

    } else if (addr >= PLC_FastIO_1 && addr <= PLC_FastIO_32) {
        // NB filtered on enabled in xx_gpio_set/clr
        if (value) {
            xx_gpio_set(addr - PLC_FastIO_1);
        } else {
            xx_gpio_clr(addr - PLC_FastIO_1);
        }

    } else if (addr == PLC_BUZZER) {

        buzzer_beep_ms =  value & 0x000000FF;
        buzzer_on_cs   = (value & 0x0000FF00) >> 8;
        buzzer_off_cs  = (value & 0x00FF0000) >> 16;
        buzzer_replies = (value & 0xFF000000) >> 24;

        buzzer_period_tics = buzzer_on_cs + buzzer_off_cs;
        buzzer_tic = 0;

        xx_pwm3_disable();
        if (buzzer_on_cs > 0 && buzzer_beep_ms > 0 && buzzer_replies > 0) {
            buzzer_tic = 1;
            buzzer_periods = 1;
            xx_pwm3_set(buzzer_beep_ms);
            xx_pwm3_enable();
        } else {
            buzzer_tic = 0;
            buzzer_periods = 0;
        }

    }

    switch (status) {

    case DATA_OK:
        // if the variable is used in alarms/events conditions
        // ... and the value changed or the state became OK (at boot-time, ...)
        if (CrossTable[addr].usedInAlarmsEvents
          && (value != the_QdataRegisters[addr] || the_QdataStates[addr] != DATA_OK)) {
            // change value and/or status ... and then re-check the alarms/events conditions
            the_QdataRegisters[addr] = value;
            the_QdataStates[addr] = status;
            pthread_cond_signal(&theAlarmsEventsCondvar);
        } else {
            // simply change value and status
            the_QdataRegisters[addr] = value;
            the_QdataStates[addr] = status;
        }
        break;

    case DATA_WARNING:
        // only status change, no value change yet
        the_QdataStates[addr] = status;
        break;

    case DATA_ERROR:
        the_QdataRegisters[addr] = value;
        the_QdataStates[addr] = status;
        break;

    default:
        ;
    }

#if defined(RTS_CFG_MECT_RETAIN)
    // if the variable is a retentive one then also update the copy
    if (retentive && addr <= LAST_RETENTIVE && status != DATA_WARNING) {
        retentive[addr -1] = value;
    }
#endif
}

#define DIAGNOSTIC_TYPE_PORT    0
#define DIAGNOSTIC_BAUDRATE     1 // IP_ADDRESS/BAUDRATE
#define DIAGNOSTIC_STATUS       2
#define DIAGNOSTIC_READS        3
#define DIAGNOSTIC_WRITES       4
#define DIAGNOSTIC_TIMEOUTS     5
#define DIAGNOSTIC_COMM_ERRORS  6
#define DIAGNOSTIC_LAST_ERROR   7
#define DIAGNOSTIC_WRITE_QUEUE  8
#define DIAGNOSTIC_BUS_LOAD     9

//1;P;RTU0_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5000;10  ;[RO]
//1;P;RTU0_BAUDRATE   ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5000;10  ;[RO]
//1;P;RTU0_STATUS     ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5000;10  ;[RO]
//1;P;RTU0_READS      ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5000;10  ;[RO]
//1;P;RTU0_WRITES     ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5000;10  ;[RO]
//1;P;RTU0_TIMEOUTS   ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5000;10  ;[RO]
//1;P;RTU0_COMM_ERRORS;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5000;10  ;[RO]
//1;P;RTU0_LAST_ERROR ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5000;10  ;[RO]
//1;P;RTU0_WRITE_QUEUE;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5000;10  ;[RO]
//1;P;RTU0_BUS_LOAD   ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5000;10  ;[RO] ex RTU0_READ_QUEUE

//1;P;RTU1_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5010;10  ;[RO]
//1;P;RTU3_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5020;10  ;[RO]
//1;P;CAN0_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5030;10  ;[RO]
//1;P;CAN1_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5040;10  ;[RO]
//1;P;TCPS_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5050;10  ;[RO]
//1;P;TCP0_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5060;10  ;[RO]
//1;P;TCP1_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5070;10  ;[RO]
//1;P;TCP2_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5080;10  ;[RO]
//1;P;TCP3_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5080;10  ;[RO]
//1;P;TCP4_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5100;10  ;[RO]
//1;P;TCP5_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5110;10  ;[RO]
//1;P;TCP6_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5120;10  ;[RO]
//1;P;TCP7_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5130;10  ;[RO]
//1;P;TCP8_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5140;10  ;[RO]
//1;P;TCP9_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5150;10  ;[RO]

static void initServerDiagnostic(u_int16_t s)
{
    u_int16_t addr = 0;

    switch (theServers[s].protocol) {
    case RTU_SRV:
        switch (theServers[s].port) {
        case 0: addr = 5000; break;
        case 1: addr = 5010; break;
        case 3: addr = 5020; break;
        default: ;
        }
        break;
    case TCP_SRV:
        switch (theServers[s].port) {
        case 502: addr = 5050; break;
        default: ;
        }
        break;
    case TCPRTU_SRV:
        break;
    default:
        ;
    }
    theServers[s].diagnosticAddr = addr;
    if (addr) {
        u_int32_t value;

        value = (theServers[s].protocol << 16) + theServers[s].port;
        writeQdataRegisters(addr + DIAGNOSTIC_TYPE_PORT, value, DATA_OK);

        switch (theServers[s].protocol) {
        case RTU_SRV:
            value = theServers[s].u.serial.baudrate;
            break;
        case TCP_SRV:
            value = theServers[s].u.tcp_ip.IPaddr;
            break;
        case TCPRTU_SRV:
            break;
        default:
            value = 0;
        }
        writeQdataRegisters(addr + DIAGNOSTIC_BAUDRATE, value, DATA_OK); // IP_ADDRESS/BAUDRATE
    }
}

static void initDeviceDiagnostic(u_int16_t d)
{
    u_int16_t addr = 0;

    switch (theDevices[d].protocol) {
    case RTU:
    case MECT:
        switch (theDevices[d].port) {
        case 0: addr = 5000; break;
        case 1: addr = 5010; break;
        case 3: addr = 5020; break;
        default: ;
        }
        break;
    case RTU_SRV:
        break;
    case CANOPEN:
        switch (theDevices[d].port) {
        case 0: addr = 5030; break;
        case 1: addr = 5040; break;
        default: ;
        }
        break;
    case TCP_SRV:
        break;
    case TCP:
        switch (theDevices[d].port) {
        case 502:
            if (theTcpDevicesNumber < 10) {
                addr = 5060 + 10 * (theTcpDevicesNumber - 1);
            }
            break;
        default: ;
        }
        break;
    default:
        ;
    }
    theDevices[d].diagnosticAddr = addr;
    if (addr) {
        u_int32_t value;

        value = (theDevices[d].protocol << 16) + theDevices[d].port;
        writeQdataRegisters(addr + DIAGNOSTIC_TYPE_PORT, value, DATA_OK);

        switch (theDevices[d].protocol) {
        case RTU:
        case MECT:
            value = theDevices[d].u.serial.baudrate;
            break;
        case CANOPEN:
            value = theDevices[d].u.serial.baudrate; // FIXME: use real baudrate
            break;
        case TCP:
            value = theDevices[d].u.tcp_ip.IPaddr;
            break;
        default:
            value = 0;
        }
        writeQdataRegisters(addr + DIAGNOSTIC_BAUDRATE, value, DATA_OK); // IP_ADDRESS/BAUDRATE
    }
}

#define DIAGNOSTIC_DEV_NODE     0
#define DIAGNOSTIC_NODE_STATUS  1
//1;P;NODE_01_DEV_NODE;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5172;2   ;[RO]
//1;P;NODE_01_STATUS  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5172;2   ;[RO]

static void initNodeDiagnostic(u_int16_t n)
{
    u_int16_t addr = 0;
    u_int32_t value;

    addr = 5172 + 2 * n;
    theNodes[n].diagnosticAddr = addr;
    value = (theNodes[n].device << 16) + theNodes[n].NodeID;
    writeQdataRegisters(addr + DIAGNOSTIC_DEV_NODE, value, DATA_OK);
    writeQdataRegisters(addr + DIAGNOSTIC_NODE_STATUS, theNodes[n].status, DATA_OK);
}

static inline void setDiagnostic(u_int16_t addr, u_int16_t offset, u_int32_t value)
{
    if (addr == 0 || addr + offset > DimCrossTable) {
        return;
    }
    addr += offset;
    writeQdataRegisters(addr, value, DATA_OK);
}

static inline void incDiagnostic(u_int16_t addr, u_int16_t offset)
{
    if (addr == 0 || addr + offset > DimCrossTable) {
        return;
    }
    addr += offset;
    writeQdataRegisters(addr, the_QdataRegisters[addr] + 1, DATA_OK);
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

static int newAlarmEvent(int isAlarm, u_int16_t addr, char *expr, size_t len)
{
    char *p, *r;

    if (lastAlarmEvent >= DimAlarmsCT) {
        return -1;
    }
    ++lastAlarmEvent;
    ALCrossTable[lastAlarmEvent].ALType = (isAlarm ? Alarm : Event);
    ALCrossTable[lastAlarmEvent].TagAddr = addr;

    p = strtok_r(expr, " ]", &r);
    if (p == NULL) {
        goto exit_error;
    }
    strncpy(ALCrossTable[lastAlarmEvent].ALSource, p, MAX_IDNAME_LEN);

    p = strtok_r(NULL, " ]", &r);
    if (p == NULL) {
        goto exit_error;
    }
    if (strncmp(p, ">=", 2) == 0) { // before ">" !!!
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_GREATER_EQ;
    } else if (strncmp(p, ">", 1) == 0) {
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_GREATER;
    } else if (strncmp(p, "<=", 2) == 0) { // before "<" !!!
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_SMALLER_EQ;
    } else if (strncmp(p, "<", 1) == 0) {
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_SMALLER;
    } else if (strncmp(p, "==", 2) == 0) {
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_EQUAL;
    } else if (strncmp(p, "!=", 2) == 0) {
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_NOT_EQUAL;
    } else if (strncmp(p, "RISING", 6) == 0) {
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_RISING;
    } else if (strncmp(p, "FALLING", 7) == 0) {
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_FALLING;
    } else {
        goto exit_error;
    }

    if (ALCrossTable[lastAlarmEvent].ALOperator != OPER_FALLING && ALCrossTable[lastAlarmEvent].ALOperator != OPER_RISING) {
        char *s;
        float f;

        p = strtok_r(NULL, "]", &r);
        if (p == NULL) {
            goto exit_error;
        }
        f = strtof(p, &s);
        if (s == p) {
            // identifier (check later on)
            strncpy(ALCrossTable[lastAlarmEvent].ALCompareVar, p, MAX_IDNAME_LEN);
        } else {
            // number
            ALCrossTable[lastAlarmEvent].ALCompareVar[0] = 0;
            memcpy(&ALCrossTable[lastAlarmEvent].ALCompareVal, &f, sizeof(u_int32_t));
        }
    }
    return 0;

exit_error:
    --lastAlarmEvent;
    return -1;
}

static u_int16_t tagAddr(char *tag)
{
    u_int16_t addr;

    for (addr = 1; addr <= DimCrossTable; ++addr) {
        if (strncmp(tag, CrossTable[addr].Tag, MAX_IDNAME_LEN) == 0) {
            return addr;
        }
    }
    return 0;
}

static char *strtok_csv(char *string, const char *separators, char **savedptr)
{
    char *p, *s;

    if (separators == NULL || savedptr == NULL) {
        return NULL;
    }
    if (string == NULL) {
        p = *savedptr;
        if (p == NULL) {
            return NULL;
        }
    } else {
        p = string;
    }

    s = strstr(p, separators);
    if (s == NULL) {
        *savedptr = NULL;
        return p;
    }
    *s = 0;
    *savedptr = s + 1;

    // remove spaces at head
    while (p < s && isspace(*p)) {
        ++p;
    }
    // remove spaces at tail
    --s;
    while (s > p && isspace(*s)) {
        *s = 0;
        --s;
    }
    return p;
}

static u_int32_t str2ipaddr(char *str)
{
    u_int32_t ipaddr = 0;
    char buffer[MAX_IPADDR_LEN];
    char *s, *r;
    int i;

    strncpy(buffer, str, MAX_IPADDR_LEN);
    buffer[16] = 0;

    s = strtok_csv(buffer, ".", &r);
    for (i = 3; i >= 0; --i) {
        if (s == NULL) {
            return 0x00000000;
        }
        ipaddr += (strtoul(s, NULL, 10) % 255) << (i * 8);
        s = strtok_csv(NULL, ".", &r);
    }
    return ipaddr;
}

static char *ipaddr2str(u_int32_t ipaddr, char *buffer)
{
    if (buffer != NULL) {
        register u_int8_t a, b, c, d;
        a = (ipaddr & 0xFF000000) >> 24;
        b = (ipaddr & 0x00FF0000) >> 16;
        c = (ipaddr & 0x0000FF00) >> 8;
        d = (ipaddr & 0x000000FF);
        sprintf(buffer, "%u.%u.%u.%u", a, b, c, d);
    }
    return buffer;
}

static int LoadXTable(void)
{
    u_int32_t addr, indx;
    int ERR = FALSE;
    FILE *xtable = NULL;

    // init tables
    for (addr = 1; addr <= DimCrossTable; ++addr) {
        CrossTable[addr].Enable = 0;
        CrossTable[addr].Plc = FALSE;
        CrossTable[addr].Tag[0] = UNKNOWN;
        CrossTable[addr].Types = 0;
        CrossTable[addr].Decimal = 0;
        CrossTable[addr].Protocol = PLC;
        CrossTable[addr].IPAddress = 0x00000000;
        CrossTable[addr].Port = 0;
        CrossTable[addr].NodeId = 0;
        CrossTable[addr].Offset = 0;
        CrossTable[addr].Block = 0;
        CrossTable[addr].BlockSize = 0;
        CrossTable[addr].Output = FALSE;
        CrossTable[addr].OldVal = 0;
        the_QdataStates[addr] = DATA_ERROR; // in error until we actually read it
        CrossTable[addr].device = 0xffff;
        CrossTable[addr].node = 0xffff;
        the_QsyncRegisters[addr] = QUEUE_EMPTY;
    }
    lastAlarmEvent = 0;
    for (addr = 0; addr <= DimAlarmsCT; ++addr) {
        ALCrossTable[addr].ALType = FALSE;
        ALCrossTable[addr].ALSource[0] = '\0';
        ALCrossTable[addr].ALCompareVar[0] = '\0';
        ALCrossTable[addr].TagAddr = 0;
        ALCrossTable[addr].SourceAddr = 0;
        ALCrossTable[addr].CompareAddr = 0;
        ALCrossTable[addr].ALCompareVal = 0;
        ALCrossTable[addr].ALOperator = 0;
        ALCrossTable[addr].ALFilterTime = 0;
        ALCrossTable[addr].ALFilterCount = 0;
        ALCrossTable[addr].comparison = 0;
    }

    // open file
    fprintf(stderr, "loading '%s' ...", CROSSTABLE_CSV);
    xtable = fopen(CROSSTABLE_CSV, "r");
    if (xtable == NULL)  {
        ERR = TRUE;
        goto exit_function;
    }

    // read loop
    for (addr = 1; addr <= DimCrossTable; ++ addr) {
        char row[1024], *p, *r;

        if (fgets(row, 1024, xtable) == NULL) {
            // no ERR = TRUE;
            continue;
        }

        // Enable {0,1,2,3}
        p = strtok_csv(row, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].Enable = atoi(p);
        // skip empty or disabled variables
        if (CrossTable[addr].Enable == 0) {
            continue;
        }

        // Plc {H,P,S,F}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        switch (p[0]) {
        case 'H':
            CrossTable[addr].Plc = Htype;
            break;
        case 'P':
            CrossTable[addr].Plc = Ptype;
            break;
        case 'S':
            CrossTable[addr].Plc = Stype;
            break;
        case 'F':
            CrossTable[addr].Plc = Ftype;
            break;
        case 'V':
            CrossTable[addr].Plc = Vtype;
            break;
        case 'X':
            CrossTable[addr].Plc = Xtype;
            break;
        default:
            ERR = TRUE;
        }
        if (ERR) {
            break;
        }

        // Tag {identifier}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        strncpy(CrossTable[addr].Tag, p, MAX_IDNAME_LEN);

        // Types {UINT, UDINT, DINT, FDCBA, ...}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        if (strcmp(p, "BIT") == 0) {
            CrossTable[addr].Types = BIT;
        } else if (strcmp(p, "BYTE") == 0) {
            CrossTable[addr].Types = UINT8;
        } else if (strcmp(p, "BYTE_BIT") == 0) {
            CrossTable[addr].Types = BYTE_BIT;
        } else if (strcmp(p, "WORD_BIT") == 0) {
            CrossTable[addr].Types = WORD_BIT;
        } else if (strcmp(p, "DWORD_BIT") == 0) {
            CrossTable[addr].Types = DWORD_BIT;
        } else if (strcmp(p, "UINT") == 0) {
            CrossTable[addr].Types = UINT16;
        } else if (strcmp(p, "UINTBA") == 0) {
            CrossTable[addr].Types = UINT16BA;
        } else if (strcmp(p, "INT") == 0) {
            CrossTable[addr].Types = INT16;
        } else if (strcmp(p, "INTBA") == 0) {
            CrossTable[addr].Types = INT16BA;
        } else if (strcmp(p, "UDINT") == 0) {
            CrossTable[addr].Types = UDINT;
        } else if (strcmp(p, "UDINTDCBA") == 0) {
            CrossTable[addr].Types = UDINTDCBA;
        } else if (strcmp(p, "UDINTCDAB") == 0) {
            CrossTable[addr].Types = UDINTCDAB;
        } else if (strcmp(p, "UDINTBADC") == 0) {
            CrossTable[addr].Types = UDINTBADC;
        } else if (strcmp(p, "DINT") == 0) {
            CrossTable[addr].Types = DINT;
        } else if (strcmp(p, "DINTDCBA") == 0) {
            CrossTable[addr].Types = DINTDCBA;
        } else if (strcmp(p, "DINTCDAB") == 0) {
            CrossTable[addr].Types = DINTCDAB;
        } else if (strcmp(p, "DINTBADC") == 0) {
            CrossTable[addr].Types = DINTBADC;
        } else if (strcmp(p, "REAL") == 0) {
            CrossTable[addr].Types = REAL;
        } else if (strcmp(p, "REALDCBA") == 0) {
            CrossTable[addr].Types = REALDCBA;
        } else if (strcmp(p, "REALCDAB") == 0) {
            CrossTable[addr].Types = REALCDAB;
        } else if (strcmp(p, "REALBADC") == 0) {
            CrossTable[addr].Types = REALBADC;

        } else if (strcmp(p, "UINTAB") == 0) {
            CrossTable[addr].Types = UINT16; // backward compatibility
        } else if (strcmp(p, "INTAB") == 0) {
            CrossTable[addr].Types = INT16; // backward compatibility
        } else if (strcmp(p, "UDINTABCD") == 0) {
            CrossTable[addr].Types = UDINT; // backward compatibility
        } else if (strcmp(p, "DINTABCD") == 0) {
            CrossTable[addr].Types = DINT; // backward compatibility
        } else if (strcmp(p, "FDCBA") == 0) {
            CrossTable[addr].Types = REALDCBA; // backward compatibility
        } else if (strcmp(p, "FCDAB") == 0) {
            CrossTable[addr].Types = REALCDAB; // backward compatibility
        } else if (strcmp(p, "FABCD") == 0) {
            CrossTable[addr].Types = REAL; // backward compatibility
        } else if (strcmp(p, "FBADC") == 0) {
            CrossTable[addr].Types = REALBADC; // backward compatibility

        } else {
            if (CrossTable[addr].Enable > 0) {
                CrossTable[addr].Types = UNKNOWN;
                ERR = TRUE;
                break;
            }
        }

        // Decimal {0, 1, 2, 3, 4, ...}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].Decimal = atoi(p);

        // Protocol {"", RTU, TCP, TCPRTU}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        if (strcmp(p, "PLC") == 0) {
            CrossTable[addr].Protocol = PLC;
            the_QdataStates[addr] = DATA_OK; // PLC variables are already ok at startup
        } else if (strcmp(p, "RTU") == 0) {
            CrossTable[addr].Protocol = RTU;
        } else if (strcmp(p, "TCP") == 0) {
            CrossTable[addr].Protocol = TCP;
        } else if (strcmp(p, "TCPRTU") == 0) {
            CrossTable[addr].Protocol = TCPRTU;
        } else if (strcmp(p, "CANOPEN") == 0) {
            CrossTable[addr].Protocol = CANOPEN;
        } else if (strcmp(p, "MECT") == 0) {
            CrossTable[addr].Protocol = MECT;
        } else if (strcmp(p, "RTU_SRV") == 0) {
            CrossTable[addr].Protocol = RTU_SRV;
        } else if (strcmp(p, "TCP_SRV") == 0) {
            CrossTable[addr].Protocol = TCP_SRV;
        } else if (strcmp(p, "TCPRTU_SRV") == 0) {
            CrossTable[addr].Protocol = TCPRTU_SRV;
        } else {
            CrossTable[addr].Protocol = PLC;
            ERR = TRUE;
            break;
        }

        // IPAddress {identifier}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].IPAddress = str2ipaddr(p);

        // Port {number}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].Port = atoi(p);

        // NodeId {number}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].NodeId = atoi(p);

        // Address {number}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].Offset = atoi(p);

        // Block {number}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].Block = atoi(p);

        // NReg {number}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].BlockSize = atoi(p);

        // Handle ([RO]||[RW]|[AL|EV SOURCE OP COND]){text}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        if (strncmp(p, "[RW]", 4) == 0) {
            CrossTable[addr].Output = TRUE;
        } else if (strncmp(p, "[RO]", 4) == 0) {
            CrossTable[addr].Output = FALSE;
        } else if (strncmp(p, "[AL ", 4) == 0) {
            CrossTable[addr].Output = FALSE;
            if (strlen(p) < 10 || newAlarmEvent(1, addr, &(p[3]), strlen(p) - 3)) {
                ERR = TRUE;
                break;
            }
        } else if (strncmp(p, "[EV ", 4) == 0) {
            CrossTable[addr].Output = FALSE;
            if (strlen(p) < 10 || newAlarmEvent(0, addr, &p[3], strlen(p) - 3)) {
                ERR = TRUE;
                break;
            }
        }
    }
    if (ERR) {
        goto exit_function;
    }

    // check alarms and events
    fprintf(stderr, "\nalarms/events:\n");
    for (indx = 1; indx <= lastAlarmEvent; ++indx) {
        // retrieve the source variable address
        addr = tagAddr(ALCrossTable[indx].ALSource);
        if (addr == 0) {
            ERR = TRUE;
            break;
        }
        ALCrossTable[indx].SourceAddr = addr;
        CrossTable[addr].usedInAlarmsEvents = TRUE;
        fprintf(stderr, "\t%2d: %s", indx, CrossTable[ALCrossTable[indx].TagAddr].Tag);
        fprintf(stderr, " = %s", CrossTable[ALCrossTable[indx].SourceAddr].Tag);

        // which comparison?
        switch (CrossTable[addr].Types) {
        case BIT:
        case BYTE_BIT:
        case WORD_BIT:
        case DWORD_BIT:
            ALCrossTable[indx].comparison = COMP_UNSIGNED;
            fprintf(stderr, " (u)");
            break;
        case INT16:
        case INT16BA:
            ALCrossTable[indx].comparison = COMP_SIGNED16;
            fprintf(stderr, " (s16)");
            break;
        case DINT:
        case DINTDCBA:
        case DINTCDAB:
        case DINTBADC:
            ALCrossTable[indx].comparison = COMP_SIGNED32;
            fprintf(stderr, " (s32)");
            break;
        case UINT8:
        case UINT16:
        case UINT16BA:
        case UDINT:
        case UDINTDCBA:
        case UDINTCDAB:
        case UDINTBADC:
            ALCrossTable[indx].comparison = COMP_UNSIGNED;
            fprintf(stderr, " (u)");
            break;
        case REAL:
        case REALDCBA:
        case REALCDAB:
        case REALBADC:
            ALCrossTable[indx].comparison = COMP_FLOATING;
            fprintf(stderr, " (f)");
            break;
        default:
            ; // FIXME: assert
        }

        switch (ALCrossTable[indx].ALOperator)  {
        case OPER_RISING    : fprintf(stderr, " RISING"); break;
        case OPER_FALLING   : fprintf(stderr, " FALLING"); break;
        case OPER_EQUAL     : fprintf(stderr, " =="); break;
        case OPER_NOT_EQUAL : fprintf(stderr, " !="); break;
        case OPER_GREATER   : fprintf(stderr, " >" ); break;
        case OPER_GREATER_EQ: fprintf(stderr, " >="); break;
        case OPER_SMALLER   : fprintf(stderr, " <" ); break;
        case OPER_SMALLER_EQ: fprintf(stderr, " <="); break;
        default             : ;
        }

        if (ALCrossTable[lastAlarmEvent].ALOperator != OPER_FALLING
         && ALCrossTable[lastAlarmEvent].ALOperator != OPER_RISING) {

            // if the comparison is with a variable
            if (ALCrossTable[indx].ALCompareVar[0] != 0) {
                int compatible = TRUE;

                // then retrieve the compare variable address
                addr = tagAddr(ALCrossTable[indx].ALCompareVar);
                if (addr == 0) {
                    ERR = TRUE;
                    break;
                }
                ALCrossTable[indx].CompareAddr = addr;
                CrossTable[addr].usedInAlarmsEvents = TRUE;
                fprintf(stderr, " %s", CrossTable[addr].Tag);

                // check for incompatibles types
                switch (CrossTable[ALCrossTable[indx].SourceAddr].Types) {

                case BIT:
                case BYTE_BIT:
                case WORD_BIT:
                case DWORD_BIT:
                    switch (CrossTable[ALCrossTable[indx].CompareAddr].Types) {
                    case BIT:
                    case BYTE_BIT:
                    case WORD_BIT:
                    case DWORD_BIT:
                        compatible = TRUE;
                        break;
                    case INT16:
                    case INT16BA:
                    case DINT:
                    case DINTDCBA:
                    case DINTCDAB:
                    case DINTBADC:
                        compatible = FALSE; // only == 0 and != 0
                        break;
                    case UINT8:
                    case UINT16:
                    case UINT16BA:
                    case UDINT:
                    case UDINTDCBA:
                    case UDINTCDAB:
                    case UDINTBADC:
                        compatible = FALSE; // only == 0 and != 0
                        break;
                    case REAL:
                    case REALDCBA:
                    case REALCDAB:
                    case REALBADC:
                        compatible = FALSE; // only == 0 and != 0
                        break;
                    default:
                        ; // FIXME: assert
                    }
                    break;

                case INT16:
                case INT16BA:
                case DINT:
                case DINTDCBA:
                case DINTCDAB:
                case DINTBADC:
                    switch (CrossTable[ALCrossTable[indx].CompareAddr].Types) {
                    case BIT:
                    case BYTE_BIT:
                    case WORD_BIT:
                    case DWORD_BIT:
                        compatible = FALSE;
                        break;
                    case INT16:
                    case INT16BA:
                    case DINT:
                    case DINTDCBA:
                    case DINTCDAB:
                    case DINTBADC:
                        compatible = (CrossTable[ALCrossTable[indx].SourceAddr].Decimal == CrossTable[ALCrossTable[indx].CompareAddr].Decimal);
                        break;
                    case UINT8:
                    case UINT16:
                    case UINT16BA:
                    case UDINT:
                    case UDINTDCBA:
                    case UDINTCDAB:
                    case UDINTBADC:
                        // compatible = (CrossTable[ALCrossTable[indx].SourceAddr].Decimal == CrossTable[ALCrossTable[indx].CompareAddr].Decimal);
                        compatible = FALSE;
                        break;
                    case REAL:
                    case REALDCBA:
                    case REALCDAB:
                    case REALBADC:
                        compatible = FALSE;
                        break;
                    default:
                        ; // FIXME: assert
                    }
                    break;

                case UINT8:
                case UINT16:
                case UINT16BA:
                case UDINT:
                case UDINTDCBA:
                case UDINTCDAB:
                case UDINTBADC:
                    switch (CrossTable[ALCrossTable[indx].CompareAddr].Types) {
                    case BIT:
                    case BYTE_BIT:
                    case WORD_BIT:
                    case DWORD_BIT:
                        compatible = FALSE;
                        break;
                    case INT16:
                    case INT16BA:
                    case DINT:
                    case DINTDCBA:
                    case DINTCDAB:
                    case DINTBADC:
                        // compatible = (CrossTable[ALCrossTable[indx].SourceAddr].Decimal == CrossTable[ALCrossTable[indx].CompareAddr].Decimal);
                        compatible = FALSE;
                        break;
                    case UINT8:
                    case UINT16:
                    case UINT16BA:
                    case UDINT:
                    case UDINTDCBA:
                    case UDINTCDAB:
                    case UDINTBADC:
                        compatible = (CrossTable[ALCrossTable[indx].SourceAddr].Decimal == CrossTable[ALCrossTable[indx].CompareAddr].Decimal);
                        break;
                    case REAL:
                    case REALDCBA:
                    case REALCDAB:
                    case REALBADC:
                        compatible = FALSE;
                        break;
                    default:
                        ; // FIXME: assert
                    }
                    break;

                case REAL:
                case REALDCBA:
                case REALCDAB:
                case REALBADC:
                    switch (CrossTable[ALCrossTable[indx].CompareAddr].Types) {
                    case BIT:
                    case BYTE_BIT:
                    case WORD_BIT:
                    case DWORD_BIT:
                        compatible = FALSE;
                        break;
                    case INT16:
                    case INT16BA:
                    case DINT:
                    case DINTDCBA:
                    case DINTCDAB:
                    case DINTBADC:
                        compatible = FALSE;
                        break;
                    case UINT8:
                    case UINT16:
                    case UINT16BA:
                    case UDINT:
                    case UDINTDCBA:
                    case UDINTCDAB:
                    case UDINTBADC:
                        compatible = FALSE;
                        break;
                    case REAL:
                    case REALDCBA:
                    case REALCDAB:
                    case REALBADC:
                        compatible = TRUE; // no decimal test
                        break;
                    default:
                        ; // FIXME: assert
                    }
                    break;

                default:
                    ; // FIXME: assert
                }
                if (! compatible) {
                    fprintf(stderr, " [WARNING: comparison between incompatible types]");
                }

            } else {
                // the comparison is with a fixed value, now check for the vartype
                // since we saved the value as float before
                float fvalue = *(float *)&ALCrossTable[indx].ALCompareVal;
                int n;

                switch (CrossTable[addr].Types) {
                case BIT:
                case BYTE_BIT:
                case WORD_BIT:
                case DWORD_BIT:
                    if (fvalue <= 0.0) {
                        ALCrossTable[indx].ALCompareVal = 0;
                    } else if (fvalue <= 1.0) {
                        ALCrossTable[indx].ALCompareVal = 1;
                    } else {
                        ALCrossTable[indx].ALCompareVal = 2;
                    }
                    break;
                case INT16:
                case INT16BA:
                    for (n = 0; n < CrossTable[addr].Decimal; ++n) {
                        fvalue *= 10;
                    }
                    if (fvalue <= 0.0) {
                        // NB this may either overflow or underflow
                        int16_t val = fvalue;
                        ALCrossTable[indx].ALCompareVal = 0;
                        memcpy(&ALCrossTable[indx].ALCompareVal, &val, sizeof(int16_t));
                    } else {
                        // NB this may either overflow or underflow
                        ALCrossTable[indx].ALCompareVal = fvalue;
                    }
                    break;
                case DINT:
                case DINTDCBA:
                case DINTCDAB:
                case DINTBADC:
                    for (n = 0; n < CrossTable[addr].Decimal; ++n) {
                        fvalue *= 10;
                    }
                    if (fvalue <= 0.0) {
                        // NB this may either overflow or underflow
                        int32_t val = fvalue;
                        memcpy(&ALCrossTable[indx].ALCompareVal, &val, sizeof(u_int32_t));
                    } else {
                        // NB this may either overflow or underflow
                        ALCrossTable[indx].ALCompareVal = fvalue;
                    }
                    break;
                case UINT8:
                case UINT16:
                case UINT16BA:
                case UDINT:
                case UDINTDCBA:
                case UDINTCDAB:
                case UDINTBADC:
                    if (fvalue <= 0.0) {
                        fvalue = 0.0; // why check unsigned with a negative value?
                    } else {
                        for (n = 0; n < CrossTable[addr].Decimal; ++n) {
                            fvalue *= 10;
                        }
                    }
                    // NB this may overflow
                    ALCrossTable[indx].ALCompareVal = fvalue;
                    break;
                case REAL:
                case REALDCBA:
                case REALCDAB:
                case REALBADC:
                    // the value is already stored as a float, comparisons will be ok
                    break;
                default:
                    ; // FIXME: assert
                }

                union {
                    int32_t i32value;
                    int16_t i16value;
                    u_int32_t uvalue;
                    float fvalue;
                } CompareVal;
                CompareVal.uvalue = ALCrossTable[indx].ALCompareVal;

                switch (ALCrossTable[indx].comparison)
                {
                case COMP_UNSIGNED:
                    fprintf(stderr, " %u", CompareVal.uvalue);
                    break;
                case COMP_SIGNED16:
                    fprintf(stderr, " %d", CompareVal.i16value);
                    break;
                case COMP_SIGNED32:
                    fprintf(stderr, " %d", CompareVal.i32value);
                    break;
                case COMP_FLOATING:
                    fprintf(stderr, " %f", fvalue);
                    break;
                default:
                    ;
                }
            }
        }
        fprintf(stderr, "\n");
    }

    // close file
exit_function:
    if (xtable) {
        fclose(xtable);
    }
    fprintf(stderr, " %s\n", (ERR) ? "ERROR" : "OK");
    return ERR;
}

static inline void setAlarmEvent(int i)
{
    u_int16_t addr = ALCrossTable[i].TagAddr;

    if (the_QdataRegisters[addr] == 0) {
        // set alarm and call tasks only if currently clear
        writeQdataRegisters(addr, 1, DATA_OK);
        if (ALCrossTable[i].ALType == Alarm) {
            vmmSetEvent(pVMM, EVT_RESERVED_11); // 27u --> EVENT:='28'
        } else {
            vmmSetEvent(pVMM, EVT_RESERVED_10); // 26u --> EVENT:='27'
        }
    }
}

static inline void clearAlarmEvent(int i)
{
    writeQdataRegisters(ALCrossTable[i].TagAddr, 0, DATA_OK);
    ALCrossTable[i].ALFilterCount = ALCrossTable[i].ALFilterTime;
}

static inline void checkAlarmEvent(int i, int condition)
{
    if (condition) {
        if (ALCrossTable[i].ALFilterCount == 0) {
            // setting alarm/event
            setAlarmEvent(i);
        } else {
            // filtering alarm/event
            ALCrossTable[i].ALFilterCount = ALCrossTable[i].ALFilterCount - 1;
        }
    } else {
        // clearing alarm/event
        clearAlarmEvent(i);
    }
}

static void AlarmMngr(void)
{
    register u_int16_t i;

    // already in pthread_mutex_lock(&theCrosstableClientMutex)
    for (i = 1; i <= lastAlarmEvent; ++i) {

        register u_int16_t SourceAddr = ALCrossTable[i].SourceAddr;
        register u_int16_t Operator = ALCrossTable[i].ALOperator;

        if (the_QdataStates[SourceAddr] != DATA_OK) {
            // unreliable values
            continue;
        }

        if (Operator == OPER_RISING || Operator == OPER_FALLING) {
            // checking against old value
            register int32_t SourceValue = the_QdataRegisters[SourceAddr];
            register int32_t CompareVal = CrossTable[i].OldVal;

            if (Operator == OPER_RISING) {
                if (CompareVal == 0) {
                    // checking rising edge if currently low
                    checkAlarmEvent(i, SourceValue != 0);
                } else if (SourceValue == 0){
                    // clearing alarm/event only at falling edge
                    clearAlarmEvent(i);
                }
            } else if (Operator == OPER_FALLING)  {
                if (CompareVal != 0) {
                    // checking falling edge if currently high
                    checkAlarmEvent(i, SourceValue == 0);
                } else if (SourceValue != 0){
                    // clearing alarm/event only at rising edge
                    clearAlarmEvent(i);
                }
            }
            // saving the new "old value" :)
            CrossTable[i].OldVal = SourceValue;

        } else {
            register u_int16_t CompareAddr = ALCrossTable[i].CompareAddr;
            union {
                int32_t i32value;
                int16_t i16value;
                u_int32_t uvalue;
                float fvalue;
            } SourceValue, CompareVal;

            SourceValue.uvalue = the_QdataRegisters[SourceAddr];

            // checking either against fixed value or against variable value
            if (CompareAddr == 0) {
                // fixed value
                CompareVal.uvalue = ALCrossTable[i].ALCompareVal;
            } else if (the_QdataStates[CompareAddr] != DATA_OK) {
                // unreliable values
                continue;
            } else {
                CompareVal.uvalue = the_QdataRegisters[CompareAddr];
                // FIXME: align decimals and types

            }

            // comparison types
            switch (ALCrossTable[i].comparison) {
            case COMP_UNSIGNED:
                switch (Operator) {
                case OPER_EQUAL     : checkAlarmEvent(i, SourceValue.uvalue == CompareVal.uvalue); break;
                case OPER_NOT_EQUAL : checkAlarmEvent(i, SourceValue.uvalue != CompareVal.uvalue); break;
                case OPER_GREATER   : checkAlarmEvent(i, SourceValue.uvalue >  CompareVal.uvalue); break;
                case OPER_GREATER_EQ: checkAlarmEvent(i, SourceValue.uvalue >= CompareVal.uvalue); break;
                case OPER_SMALLER   : checkAlarmEvent(i, SourceValue.uvalue <  CompareVal.uvalue); break;
                case OPER_SMALLER_EQ: checkAlarmEvent(i, SourceValue.uvalue <= CompareVal.uvalue); break;
                default             : ;
                }
                break;
            case COMP_SIGNED16:
                switch (Operator) {
                case OPER_EQUAL     : checkAlarmEvent(i, SourceValue.i16value == CompareVal.i16value); break;
                case OPER_NOT_EQUAL : checkAlarmEvent(i, SourceValue.i16value != CompareVal.i16value); break;
                case OPER_GREATER   : checkAlarmEvent(i, SourceValue.i16value >  CompareVal.i16value); break;
                case OPER_GREATER_EQ: checkAlarmEvent(i, SourceValue.i16value >= CompareVal.i16value); break;
                case OPER_SMALLER   : checkAlarmEvent(i, SourceValue.i16value <  CompareVal.i16value); break;
                case OPER_SMALLER_EQ: checkAlarmEvent(i, SourceValue.i16value <= CompareVal.i16value); break;
                default             : ;
                }
                break;
            case COMP_SIGNED32:
                switch (Operator) {
                case OPER_EQUAL     : checkAlarmEvent(i, SourceValue.i32value == CompareVal.i32value); break;
                case OPER_NOT_EQUAL : checkAlarmEvent(i, SourceValue.i32value != CompareVal.i32value); break;
                case OPER_GREATER   : checkAlarmEvent(i, SourceValue.i32value >  CompareVal.i32value); break;
                case OPER_GREATER_EQ: checkAlarmEvent(i, SourceValue.i32value >= CompareVal.i32value); break;
                case OPER_SMALLER   : checkAlarmEvent(i, SourceValue.i32value <  CompareVal.i32value); break;
                case OPER_SMALLER_EQ: checkAlarmEvent(i, SourceValue.i32value <= CompareVal.i32value); break;
                default             : ;
                }
                break;
            case COMP_FLOATING:
                switch (Operator) {
                case OPER_EQUAL     : checkAlarmEvent(i, SourceValue.fvalue == CompareVal.fvalue); break;
                case OPER_NOT_EQUAL : checkAlarmEvent(i, SourceValue.fvalue != CompareVal.fvalue); break;
                case OPER_GREATER   : checkAlarmEvent(i, SourceValue.fvalue >  CompareVal.fvalue); break;
                case OPER_GREATER_EQ: checkAlarmEvent(i, SourceValue.fvalue >= CompareVal.fvalue); break;
                case OPER_SMALLER   : checkAlarmEvent(i, SourceValue.fvalue <  CompareVal.fvalue); break;
                case OPER_SMALLER_EQ: checkAlarmEvent(i, SourceValue.fvalue <= CompareVal.fvalue); break;
                default             : ;
                }
                break;
            default:
                ;
            }
        }
    }
}

static void PLCsync(void)
{
    u_int16_t indx;
    u_int16_t oper;
    u_int16_t addr;
    int d, n;

    // clear H variables status for each device
    for (d = 0; d < theDevicesNumber; ++d) {
        for (n = 0; n < theDevices[d].var_num; ++n) {
            addr = theDevices[d].device_vars[n].addr;
            if (CrossTable[addr].Plc == Htype) {
                theDevices[d].device_vars[n].active = 0;
            }
        }
    }

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
                // set H variables status for each device
                if (CrossTable[addr].Plc == Htype && CrossTable[addr].device != 0xffff) {
                    d = CrossTable[addr].device;
                    for (n = 0; n < theDevices[d].var_num; ++n) {
                        if (theDevices[d].device_vars[n].addr == addr) {
                            theDevices[d].device_vars[n].active = 1;
                        }
                    }
                }
                if (the_QsyncRegisters[indx] != QUEUE_BUSY_READ) {
                    the_QsyncRegisters[indx] = QUEUE_BUSY_READ;
                    switch (CrossTable[addr].Protocol) {
                    case PLC:
                        // immediate read: no fieldbus
                        // ready "by design" the_QdataRegisters[addr] = the_QdataRegisters[addr];
                        // the_QdataStates[addr] = DATA_OK;
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
                            // activate semaphore only the first time
                            sem_post(&newOperations[CrossTable[addr].device]);
                        } else {
                            the_QsyncRegisters[indx] = QUEUE_EMPTY;
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
                    switch (CrossTable[addr].Protocol) {
                    case PLC:
                        // immediate write: no fieldbus
                        writeQdataRegisters(addr, the_IdataRegisters[addr], DATA_OK);
                        the_QsyncRegisters[indx] = QUEUE_BUSY_WRITE;
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
                            fprintf(stderr, "_________: write(0x%04x) [%u]@%u value=%u %s\n", oper, addr, indx, the_IdataRegisters[addr], CrossTable[addr].Tag);
#endif
                            // search for consecutive writes
                            register int i;
                            register u_int16_t oper_i;
                            register u_int16_t addr_i;
                            register unsigned written;

                            for (i = 1; i < MAX_VALUES && (indx + i) <= DimCrossTable; ++i) {
                                oper_i = the_IsyncRegisters[indx + i] & QueueOperMask;
                                addr_i = the_IsyncRegisters[indx + i] & QueueAddrMask;

                                if (the_QsyncRegisters[indx + i] != QUEUE_BUSY_WRITE
                                 && oper_i == oper && addr_i == (addr + i)
                                 && CrossTable[addr].device == CrossTable[addr + i].device)
                                    continue;
                                else
                                    break;
                            }

                            // trying multiple writes
                            i = i - 1; // may be 0
#ifdef VERBOSE_DEBUG
                            {
                                int z;
                                fprintf(stderr, "PLCsync() --> doWriteVariable(%u, 0x%08x, , , %d) [%d]\n",
                                        addr, the_IdataRegisters[addr], addr + i, i + 1);
                                for (z = 0; z < i + 1; ++z)
                                    fprintf(stderr, "\t%02d: 0x%08x\n", z, the_IdataRegisters[addr + z]);
                            }
#endif
                            written = doWriteVariable(addr, the_IdataRegisters[addr], the_IdataRegisters, NULL, addr + i);
#ifdef VERBOSE_DEBUG
                            fprintf(stderr, "PLCsync() <-- written = %u\n", written);
#endif
                            // acknowledge the written items
                            for (i = 0; i < written; ++i) {
                                the_QsyncRegisters[indx + i] = QUEUE_BUSY_WRITE;
                            }
                            // adjust the loop index
                            if (written > 1) {
                                indx += written - 1;
                            }
                        }
                        break;
                    default:
                        ;
                    }
                }
                break;
            case WRITE_PREPARE:
                the_QsyncRegisters[indx] = QUEUE_EMPTY;
                break; // nop
            default:
                ;
            }
        }
    }
#ifdef VERBOSE_DEBUG
    static int counter = 0;

    if (counter++ >= 10) {
        // circa ogni secondo
        counter = 0;
        fprintf(stderr, "_________\n");
        u_int16_t n;
        for (n = 1; n <= DimCrossTable; ++n) {
            u_int16_t Oper = the_IsyncRegisters[n] & QueueOperMask;
            u_int16_t Addr = the_IsyncRegisters[n] & QueueAddrMask;

            if (1 <= Addr && Addr <= DimCrossTable && CrossTable[Addr].Plc == Htype) {
                fprintf(stderr, "\t[%u] 0x%04x %s", n, Oper, CrossTable[Addr].Tag);
                for (d = 0; d < theDevicesNumber; ++d) {
                    int i;
                    for (i = 0; i < theDevices[d].var_num; ++i) {
                        addr = theDevices[d].device_vars[i].addr;
                        if (addr == Addr && theDevices[d].device_vars[i].active) {
                            fprintf(stderr, " (%s)", theDevices[d].name);
                        }
                    }
                }
                fprintf(stderr, "\n");
            }
        }
    }
#endif
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
            u_int16_t d;
            u_int16_t n;
            u_int16_t p;

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
                // add unique variable's server
                for (s = 0; s < theServersNumber; ++s) {
                    if (RTU_SRV == theServers[s].protocol && CrossTable[i].Port == theServers[s].port) {
                        // already present
                        if (theServers[s].IPaddress != CrossTable[i].IPAddress) {
                            char str1[42];
                            char str2[42];
                            fprintf(stderr,
                                "%s() WARNING in variable #%u wrong 'IP Address' %s (should be %s)\n",
                                __func__, i, ipaddr2str(CrossTable[i].IPAddress, str2), ipaddr2str(theServers[s].IPaddress, str1));
                        }
                        if (theServers[s].NodeId != CrossTable[i].NodeId) {
                            fprintf(stderr,
                                "%s() WARNING in variable #%u wrong 'Node ID' %u (should be %u)\n",
                                __func__, i, CrossTable[i].NodeId, theServers[s].NodeId);
                        }
                        break;
                    }
                }
                goto add_server;
                // no break;
            case TCP_SRV:
            case TCPRTU_SRV:
                // add unique variable's server
                for (s = 0; s < theServersNumber; ++s) {
                    if (CrossTable[i].Protocol == theServers[s].protocol) {
                        // already present
                        if (theServers[s].IPaddress != CrossTable[i].IPAddress) {
                            char str1[42];
                            char str2[42];
                            fprintf(stderr,
                                "%s() WARNING in variable #%u wrong 'IP Address' %s (should be %s)\n",
                                __func__, i, ipaddr2str(CrossTable[i].IPAddress, str2), ipaddr2str(theServers[s].IPaddress, str1));
                        }
                        if (theServers[s].port != CrossTable[i].Port) {
                            fprintf(stderr,
                                "%s() WARNING in variable #%u wrong 'Port' %s (should be %u)\n",
                                __func__, i, CrossTable[i].Port, theServers[s].port);
                        }
                        if (theServers[s].NodeId != CrossTable[i].NodeId) {
                            fprintf(stderr,
                                "%s() WARNING in variable #%u wrong 'Node ID' %u (should be %u)\n",
                                __func__, i, CrossTable[i].NodeId, theServers[s].NodeId);
                        }
                        break;
                    }
                }
            add_server:
                if (s < theServersNumber) {
                    // ok already present
                } else if (theServersNumber >= MAX_SERVERS) {
                    fprintf(stderr, "%s() too many servers\n", __func__, MAX_SERVERS);
                    retval = -1;
                } else {
                    // new server entry
                    ++theServersNumber;
                    theServers[s].protocol = CrossTable[i].Protocol;
                    theServers[s].IPaddress = CrossTable[i].IPAddress;
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
                            if (system_ini.serial_port[port].baudrate == 0) {
                                fprintf(stderr, "%s: missing port %u in system.ini for RTU_SRV variable #%u\n", __func__, port, i);
                                retval = -1;
                            } else {
                                theServers[s].u.serial.port = port;
                                theServers[s].u.serial.baudrate = system_ini.serial_port[port].baudrate;
                                theServers[s].u.serial.parity = system_ini.serial_port[port].parity;
                                theServers[s].u.serial.databits = system_ini.serial_port[port].databits;
                                theServers[s].u.serial.stopbits = system_ini.serial_port[port].stopbits;
                                theServers[s].silence_ms = system_ini.serial_port[port].silence_ms;
                                theServers[s].timeout_ms = system_ini.serial_port[port].timeout_ms;
                            }
                            break;
                        default:
                            fprintf(stderr, "%s: bad RTU_SRV port %u for variable #%u\n", __func__, port, i);
                            retval = -1;
                        }
                        theServers[s].ctx = NULL;
                    }   break;
                    case TCP_SRV:
                        theServers[s].u.tcp_ip.IPaddr = CrossTable[i].IPAddress;
                        theServers[s].u.tcp_ip.port = CrossTable[i].Port;
                        theServers[s].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theServers[s].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        theServers[s].ctx = NULL;
                        break;
                    case TCPRTU_SRV:
                        theServers[s].u.tcp_ip.IPaddr = CrossTable[i].IPAddress;
                        theServers[s].u.tcp_ip.port = CrossTable[i].Port;
                        theServers[s].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theServers[s].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        theServers[s].ctx = NULL;
                        break;
                    default:
                        ;
                    }
                    theServers[s].NodeId = CrossTable[i].NodeId;
                    theServers[s].thread_id = 0;
                    theServers[s].status = SRV_RUNNING0;
                    theServers[s].thread_status = NOT_STARTED;
                    // theServers[s].serverMutex = PTHREAD_MUTEX_INITIALIZER;
                    snprintf(theServers[s].name, MAX_THREADNAME_LEN, "srv[%d]%s_0x%08x_%d", s, fieldbusName[theServers[s].protocol], theServers[s].IPaddress, theServers[s].port);
                    initServerDiagnostic(s);
                    theServers[s].idle_time_ns = 0;
                    theServers[s].busy_time_ns = 0;
                    theServers[s].last_time_ns = 0;
                    theServers[s].last_update_ns = 0;
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
                // add unique variable's device (Protocol, IPAddress, Port, --)
                for (d = 0; d < theDevicesNumber; ++d) {
                    if (CrossTable[i].Protocol == theDevices[d].protocol
                     && CrossTable[i].IPAddress == theDevices[d].IPaddress
                     && CrossTable[i].Port == theDevices[d].port) {
                        // already present
                        break;
                    }
                }
                goto add_device;
                // no break
            case RTU_SRV:
                // add unique variable's device (Protocol, --, Port, --)
                for (d = 0; d < theDevicesNumber; ++d) {
                    if (CrossTable[i].Protocol == theDevices[d].protocol
                     && CrossTable[i].Port == theDevices[d].port) {
                        // already present
                        break;
                    }
                }
                goto add_device;
                // no break
            case TCP_SRV:
            case TCPRTU_SRV:
                // add unique variable's device (Protocol, --, --, --)
                for (d = 0; d < theDevicesNumber; ++d) {
                    if (CrossTable[i].Protocol == theDevices[d].protocol) {
                        // already present
                        break;
                    }
                }
            add_device:
                if (d < theDevicesNumber) {
                    CrossTable[i].device = d; // found
                    theDevices[d].var_num += 1; // this one, also Htype
                } else if (theDevicesNumber >= MAX_DEVICES) {
                    CrossTable[i].device = 0xffff; // FIXME: error
                    fprintf(stderr, "%s() too many devices\n", __func__, MAX_DEVICES);
                    retval = -1;
                } else {
                    // new device entry
                    CrossTable[i].device = theDevicesNumber;
                    ++theDevicesNumber;
                    theDevices[d].protocol = CrossTable[i].Protocol;
                    theDevices[d].IPaddress = CrossTable[i].IPAddress;
                    theDevices[d].port = p = CrossTable[i].Port;
                    theDevices[d].var_num = 1; // this one, also Htype
                    theDevices[d].device_vars = NULL; // calloc later on
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
                            if (system_ini.serial_port[p].baudrate == 0) {
                                fprintf(stderr, "%s: missing port %u in system.ini for RTU variable #%u\n", __func__, p, i);
                                retval = -1;
                            } else {
                                theDevices[d].u.serial.port = p;
                                theDevices[d].u.serial.baudrate = system_ini.serial_port[p].baudrate;
                                theDevices[d].u.serial.parity = system_ini.serial_port[p].parity;
                                theDevices[d].u.serial.databits = system_ini.serial_port[p].databits;
                                theDevices[d].u.serial.stopbits = system_ini.serial_port[p].stopbits;
                                theDevices[d].silence_ms = system_ini.serial_port[p].silence_ms;
                                theDevices[d].timeout_ms = system_ini.serial_port[p].timeout_ms;
                                theDevices[d].max_block_size = system_ini.serial_port[p].max_block_size;
                            }
                            break;
                        default:
                            fprintf(stderr, "%s: bad %s port %u for variable #%u\n", __func__,
                                    (theDevices[d].protocol == RTU ? "RTU" : "MECT"), p, i);
                            retval = -1;
                        }
                        break;
                    case TCP:
                        ++theTcpDevicesNumber;
                        theDevices[d].u.tcp_ip.IPaddr = CrossTable[i].IPAddress;
                        theDevices[d].u.tcp_ip.port = CrossTable[i].Port;
                        theDevices[d].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theDevices[d].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        theDevices[d].max_block_size = system_ini.tcp_ip_port.max_block_size;
                        break;
                    case TCPRTU:
                        theDevices[d].u.tcp_ip.IPaddr = CrossTable[i].IPAddress;
                        theDevices[d].u.tcp_ip.port = CrossTable[i].Port;
                        theDevices[d].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theDevices[d].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        theDevices[d].max_block_size = system_ini.tcp_ip_port.max_block_size;
                        break;
                    case CANOPEN:
                        switch (p) {
                        case 0:
                        case 1:
                            theDevices[d].u.can.bus = p;
                            theDevices[d].u.can.baudrate = system_ini.canopen[p].baudrate;
                            theDevices[d].silence_ms = 0;
                            theDevices[d].timeout_ms = 0;
                            theDevices[d].max_block_size = system_ini.canopen[p].max_block_size;
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
                        theDevices[d].max_block_size = system_ini.serial_port[p].max_block_size;
                        break;
                    case TCP_SRV:
                    case TCPRTU_SRV:
                        theDevices[d].server = s; // searched before
                        theDevices[d].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theDevices[d].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        theDevices[d].max_block_size = system_ini.tcp_ip_port.max_block_size;
                        break;
                    default:
                        ;
                    }
                    snprintf(theDevices[d].name, MAX_THREADNAME_LEN, "dev(%d)%s_0x%08x_%u", d, fieldbusName[theDevices[d].protocol], theDevices[d].IPaddress, theDevices[d].port);
                    if (theDevices[d].timeout_ms == 0 && theDevices[d].protocol == RTU) {
                        theDevices[d].timeout_ms = 300;
                        fprintf(stderr, "%s: TimeOut of device '%s' forced to %u ms\n", __func__, theDevices[d].name, theDevices[d].timeout_ms);
                    }
                    if (i == base && CrossTable[i].BlockSize > theDevices[d].max_block_size) {
                        fprintf(stderr, "%s: warning: variable #%u block #%u size %u, exceeding max_block_size %u (%s)\n",
                                __func__, i, block, CrossTable[i].BlockSize, theDevices[d].max_block_size, theDevices[d].name);
                    }
                    theDevices[d].elapsed_time_ns = 0;
                    theDevices[d].idle_time_ns = 0;
                    theDevices[d].busy_time_ns = 0;
                    theDevices[d].last_time_ns = 0;
                    theDevices[d].last_update_ns = 0;
                    theDevices[d].status = ZERO;
                    // theDevices[d].thread_id = 0;
                    theDevices[d].thread_status = NOT_STARTED;
                    sem_init(&newOperations[d], 0, 0);
                    theDevices[d].PLCwriteRequestNumber = 0;
                    theDevices[d].PLCwriteRequestGet = 0;
                    theDevices[d].PLCwriteRequestPut = 0;
                    // theDevices[d].modbus_ctx .last_good_ms, PLCwriteRequests, PLCwriteRequestNumber, PLCwriteRequestGet, PLCwriteRequestPut
                    initDeviceDiagnostic(d);
                }
                // add unique variable's node
                for (n = 0; n < theNodesNumber; ++n) {
                     if (CrossTable[i].device == theNodes[n].device) {
                         if (CrossTable[i].Protocol == RTU_SRV || CrossTable[i].Protocol == TCP_SRV  || CrossTable[i].Protocol == TCPRTU_SRV) {
                             // already present (any NodeId)
                             break;
                         } else if (CrossTable[i].NodeId == theNodes[n].NodeID) {
                             // already present
                             break;
                         }
                    }
                }
            add_node:
                if (n < theNodesNumber) {
                    CrossTable[i].node = n; // found
                } else if (theNodesNumber >= MAX_NODES) {
                    CrossTable[i].node = 0xffff;
                    fprintf(stderr, "%s() too many nodes\n", __func__, MAX_NODES);
                    retval = -1;
                } else {
                    // new node entry
                    CrossTable[i].node = theNodesNumber;
                    ++theNodesNumber;
                    theNodes[n].device = CrossTable[i].device;
                    theNodes[n].NodeID = CrossTable[i].NodeId;
                    theNodes[n].status = NODE_OK;
                    // theNodes[n].RetryCounter .JumpRead
                    initNodeDiagnostic(n);
                }
                break;
            default:
                break;
            }
        }
    }

  {
    u_int16_t d;
    u_int16_t var_max[MAX_DEVICES];

    for (i = 1; i <= DimCrossTable; ++i) {
        if (CrossTable[i].Enable > 0) {

            // client variables =---> create and fill the variables addresses array
            switch (CrossTable[i].Protocol) {
            case PLC:
                // no plc client
                break;
            case RTU:
            case TCP:
            case TCPRTU:
            case CANOPEN:
            case MECT:
            case RTU_SRV:
            case TCP_SRV:
            case TCPRTU_SRV:
                d = CrossTable[i].device;
                if (d != 0xffff) {
                    if (theDevices[d].device_vars == NULL) {
                        theDevices[d].device_vars = calloc(theDevices[d].var_num, sizeof(struct device_var));
                        var_max[d] = 0;
                    }
                    if (theDevices[d].device_vars == NULL) {
                        fprintf(stderr, "%s() memory full\n", __func__);
                        retval = -1;
                        break;
                    }
                    if (var_max[d] < theDevices[d].var_num) {
                        theDevices[d].device_vars[var_max[d]].addr = i;
                        if (CrossTable[i].Plc > Htype) {
                            theDevices[d].device_vars[var_max[d]].active = 1;
                        }
                        ++var_max[d];
                    }
                }
                break;
            default:
                break;
            }
        }
    }
  }
    return retval;
}

static void setEngineStatus(enum EngineStatus status)
{
    engineStatus = status;
    writeQdataRegisters(PLC_EngineStatus, status, DATA_OK);
}

static void *engineThread(void *statusAdr)
{
    enum threadStatus *threadStatusPtr = (enum threadStatus *)statusAdr;
    int allOK = FALSE;
    pVMM = get_pVMM();

    // thread init
    osPthreadSetSched(FC_SCHED_VMM, FC_PRIO_VMM); // engineThread
    setEngineStatus(enInitialized);

    pthread_mutex_lock(&theCrosstableClientMutex);
    {
        int s, d, n;

        // XX_GPIO_SET(1);
        // load configuration
        if (LoadXTable()) {
            goto exit_initialization;
        }
        if (!system_ini_ok) {
            goto exit_initialization;
        }
        if (checkServersDevicesAndNodes()) {
            goto exit_initialization;
        }

        if (verbose_print_enabled) {
            fprintf(stderr, "engineThread(): %u servers, %u devices, %u nodes\n", theServersNumber, theDevicesNumber, theNodesNumber);
            for (s = 0; s < theServersNumber; ++s) {
                fprintf(stderr, "\t0x%02x: %s\n", s, theServers[s].name);
            }
            for (d = 0; d < theDevicesNumber; ++d) {
                fprintf(stderr, "\t0x%02x: %s\n", d, theDevices[d].name);
                for (n = 0; n < theNodesNumber; ++n) {
                    if (theNodes[n].device == d) {
                        fprintf(stderr, "\t\tnode#%2d NodeID=%u\n", n+1, theNodes[n].NodeID);
                    }
                }
            }
        }

        // i.MX28 workaround
        xx_pwm3_set(0);
        xx_pwm3_enable();

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
        // create udp server
        if (osPthreadCreate(&theDataSyncThread_id, NULL, &datasyncThread, &theDataSyncThreadStatus, "datasync", 0) == 0) {
            do {
                osSleep(THE_CONFIG_DELAY_ms); // not sched_yield();
            } while (theDataSyncThreadStatus != RUNNING);
        }
        // ok, all done
        allOK = TRUE;

    exit_initialization:
         // XX_GPIO_CLR(1);
        buzzer_periods = 0;
        buzzer_tic = 0;
        xx_pwm3_disable();
    }
    pthread_mutex_unlock(&theCrosstableClientMutex);

    if (allOK) {
        fprintf(stderr, "PLC engine is running\n");
        setEngineStatus(enRunning);
    } else {
        fprintf(stderr, "**********************************************************\n");
        fprintf(stderr, "* PLC engine is in error status: application won't work! *\n");
        fprintf(stderr, "**********************************************************\n");
        setEngineStatus(enError);;
    }
    // run
    *threadStatusPtr = RUNNING;
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);

    // NO default Fast I/O config PLC_FastIO_Dir

    pthread_mutex_lock(&theCrosstableClientMutex);
    // XX_GPIO_SET(1);
    int tic = 0;
    while (engineStatus != enExiting) {

        // trivial scenario
        if (engineStatus != enRunning) {
            // XX_GPIO_CLR(1);
            pthread_mutex_unlock(&theCrosstableClientMutex);
            osSleep(THE_ENGINE_DELAY_ms);
            pthread_mutex_lock(&theCrosstableClientMutex);
            // XX_GPIO_SET(1);
            continue;
        }

        // abstime.tv_sec += THE_ENGINE_DELAY_ms / 1000;
        abstime.tv_nsec = abstime.tv_nsec + (THE_ENGINE_DELAY_ms * 1E6); // (THE_ENGINE_DELAY_ms % 1000) * 1E6;
        if (abstime.tv_nsec >= 1E9) {
            ldiv_t x = ldiv(abstime.tv_nsec, 1E9);
            abstime.tv_sec += x.quot;
            abstime.tv_nsec = x.rem;
        }
        while (TRUE) {
            int e;
            // XX_GPIO_CLR(1);
            e = pthread_cond_timedwait(&theAlarmsEventsCondvar, &theCrosstableClientMutex, &abstime);
            // XX_GPIO_SET(1);
            if (e == ETIMEDOUT) {
                break;
            }
            AlarmMngr();
        }

        tic = (tic + 1) % 10;
        if (tic == 1) {
            // datetime   NB no writeQdataRegisters();
            struct timespec tv;
            struct tm datetime;

            clock_gettime(CLOCK_HOST_REALTIME, &tv);
            if (localtime_r(&tv.tv_sec, &datetime)) {

                the_QdataRegisters[PLC_Seconds] = datetime.tm_sec;
                the_QdataRegisters[PLC_Minutes] = datetime.tm_min;
                the_QdataRegisters[PLC_Hours] = datetime.tm_hour;
                the_QdataRegisters[PLC_Day] = datetime.tm_mday;
                the_QdataRegisters[PLC_Month] = datetime.tm_mon + 1;
                the_QdataRegisters[PLC_Year] = 1900 + datetime.tm_year;
            }
        }

        if (tic == 1 || tic == 6) {
            // TICtimer  NB no writeQdataRegisters();

            float plc_time, plc_timeMin, plc_timeMax, plc_timeWin;
            RTIME tic_ms = rt_timer_read() / 1000000u;

            tic_ms = tic_ms % (86400 * 1000); // 1 day overflow
            plc_time = tic_ms / 1000.0;
            memcpy(&plc_timeWin, &the_QdataRegisters[PLC_timeWin], sizeof(u_int32_t));
            if (plc_timeWin < 5.0) {
                plc_timeWin = 5.0;
            }
            if (plc_time <= plc_timeWin) {
                plc_timeMin = 0;
                plc_timeMax = plc_timeWin;
            } else {
                plc_timeMin = plc_time - plc_timeWin;
                plc_timeMax = plc_time;
            }
            memcpy(&the_QdataRegisters[PLC_time], &plc_time, sizeof(u_int32_t));
            memcpy(&the_QdataRegisters[PLC_timeMin], &plc_timeMin, sizeof(u_int32_t));
            memcpy(&the_QdataRegisters[PLC_timeMax], &plc_timeMax, sizeof(u_int32_t));
            memcpy(&the_QdataRegisters[PLC_timeWin], &plc_timeWin, sizeof(u_int32_t));
        }

        if (the_QdataRegisters[PLC_ResetValues]) {
            u_int16_t addr;
            for (addr = 5000; addr < 5160; addr += 10) {
                the_QdataRegisters[addr + 3] = 0; // READS
                the_QdataRegisters[addr + 4] = 0; // WRITES
                the_QdataRegisters[addr + 5] = 0; // TIMEOUTS
                the_QdataRegisters[addr + 6] = 0; // COMM_ERRORS
                the_QdataRegisters[addr + 7] = 0; // LAST_ERROR
            }
            the_QdataRegisters[PLC_ResetValues] = 0;
        }

        // PLC_FastIO, both In and Out
        unsigned addr;
        unsigned value;

        for (addr = PLC_FastIO_1; addr < (PLC_FastIO_1 + XX_GPIO_MAX); ++addr) {
            value = xx_gpio_get(addr - PLC_FastIO_1);

            if (value != the_QdataRegisters[addr]) {
                writeQdataRegisters(addr, value, DATA_OK);
            }
        }

        addr = PLC_WATCHDOG_ms;
        value = xx_watchdog_get();
        if (value != the_QdataRegisters[addr]) {
            // NB no writeQdataRegisters();
            the_QdataRegisters[addr] = value;
        }

        // BUZZER
        if (buzzer_periods > 0) {
            ++buzzer_tic;

            if (buzzer_tic > buzzer_period_tics) {
                ++buzzer_periods;

                if (buzzer_periods > buzzer_replies) {
                    xx_pwm3_disable();
                    buzzer_tic = 0;
                    buzzer_periods = 0;
                } else {
                    xx_pwm3_enable();
                    buzzer_tic = 1;
                }
            } else if (buzzer_tic == buzzer_on_cs) {
                xx_pwm3_disable();
            }
        }
    }
    pthread_mutex_unlock(&theCrosstableClientMutex);

    // thread clean
    // see dataEngineStop()

    // exit
    // XX_GPIO_CLR(1);
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
        case UINT8:
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
            register u_int32_t offset = CrossTable[DataAddr + i].Offset;

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

static void fieldbusReset(u_int16_t d)
{
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
    case MECT: // fieldbusReset()
        mect_close(theDevices[d].mect_fd);
        break;
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        break;
    default:
        ;
    }
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
            } else if (CrossTable[DataAddr].Offset >= 300000 && CrossTable[DataAddr].Offset < 365536) {
                bzero(uintRegs, sizeof(uintRegs));
                e = modbus_read_input_registers(theDevices[d].modbus_ctx,
                    CrossTable[DataAddr].Offset - 300000, regs, uintRegs);
#if 0
            } else if (CrossTable[DataAddr].Offset >= 40001 && CrossTable[DataAddr].Offset < 50000) {
                bzero(uintRegs, sizeof(uintRegs));
                e = modbus_read_registers(theDevices[d].modbus_ctx,
                    CrossTable[DataAddr].Offset - 40001, regs, uintRegs);
#endif
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
                if (server != 0xffff && theServers[server].mb_mapping != NULL) {
                    pthread_mutex_lock(&theServers[server].mutex);
                    {
                        register u_int32_t base = CrossTable[DataAddr].Offset;
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
                    case UINT8:
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
                        register u_int32_t offset = CrossTable[DataAddr + i].Offset;

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
                    case UINT8:
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
        } else {
            if (e == -1) { // OTHER_ERROR
                if ((errno == EBADF || errno == ECONNRESET || errno == EPIPE)) {
                    retval = ConnReset;
                } else {
                    retval = CommError;
                }
            } else if (e == -2) { // TIMEOUT_ERROR
                retval = TimeoutError;
            } else {
                retval = CommError;
            }
            if (verbose_print_enabled) {
                fprintf(stderr, "fieldbusRead(%d, %u, %u): %d,%d(%s) --> %d\n",
                    d, DataAddr, DataNumber,
                    e, errno, modbus_strerror(errno),
                    retval);
            }
        }
        break;
    case CANOPEN:
        device = CrossTable[DataAddr].device;
        if (device != 0xffff) {
            u_int8_t channel = theDevices[device].port;
            for (i = 0; i < DataNumber; ++i) {
                register enum varTypes vartype = CrossTable[DataAddr + i].Types;
                register u_int32_t offset = CrossTable[DataAddr + i].Offset;

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
                    case  UINT8: {
                        u_int8_t a;
                        e = CANopenReadPDOByte(channel, offset, &a);
                        DataValue[i] = a;
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
            if (e) {
                if (e == -1) { // OTHER_ERROR
                    retval = CommError;
                } else if (e == -2) { // TIMEOUT_ERROR
                    retval = TimeoutError;
                } else if (e == -3) { // RESET_ERROR
                    retval = ConnReset;
                } else {
                    retval = CommError;
                }
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
    case MECT: // fieldbusRead()
        for (i = 0; i < DataNumber; ++i) {

            if (CrossTable[DataAddr + i].Types == REAL) {
                float value;
                e = mect_read_ascii(theDevices[d].mect_fd, CrossTable[DataAddr + i].NodeId, CrossTable[DataAddr + i].Offset, &value);
                memcpy(&DataValue[i], &value, sizeof(u_int32_t));
            } else if (CrossTable[DataAddr + i].Types == UINT16) {
                unsigned value;
                e = mect_read_hexad(theDevices[d].mect_fd, CrossTable[DataAddr + i].NodeId, CrossTable[DataAddr + i].Offset, &value);
                DataValue[i] = value;
            }
            if (e == -1) {
                retval = CommError;
                break;
            } else if (e == -2) {
                retval = TimeoutError;
                break;
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
                    register u_int32_t offset = CrossTable[DataAddr + i].Offset;
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
                case UINT8:
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
                register u_int32_t offset = CrossTable[DataAddr + i].Offset;

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
            case UINT8:
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
            } else if (CrossTable[DataAddr].Offset >= 300000 && CrossTable[DataAddr].Offset < 365536) {
                e = -1; // cannot write inputs
#if 0
            } else if (CrossTable[DataAddr].Offset >= 40001 && CrossTable[DataAddr].Offset < 50000) {
                if (regs == 1){
                    e = modbus_write_register(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset - 40001, uintRegs[0]);
                } else {
                    e = modbus_write_registers(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset - 40001, regs, uintRegs);
                }
#endif
            } else {
                if (regs == 1){
                    e = modbus_write_register(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, uintRegs[0]);
                } else {
                    e = modbus_write_registers(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, uintRegs);
                }
            }
#ifdef VERBOSE_DEBUG
        {
            int z;
            fprintf(stderr, "%s wrote %u (%u) vars from %u (%s)\n", theDevices[d].name, DataNumber, regs, DataAddr, CrossTable[DataAddr].Tag);
            for (z = 0; z < regs; ++z) {
                fprintf(stderr, "\t%02d: 0x%04x\n", z, uintRegs[z]);
            }
            fprintf(stderr, "\tresult = %02d\n", e);

        }
#endif
            break;
        case RTU_SRV:
        case TCP_SRV:
        case TCPRTU_SRV:
            device = CrossTable[DataAddr].device;
            if (device != 0xffff) {
                server = theDevices[device].server;
                if (server != 0xffff && theServers[server].mb_mapping != NULL) {
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
            e = regs;
            break;
        default:
            ; //FIXME: assert
        }
        if (e == regs) {
            retval = NoError;
        } else {
            if (e == -1) { // OTHER_ERROR
                if ((errno == EBADF || errno == ECONNRESET || errno == EPIPE)) {
                    retval = ConnReset;
                } else {
                    retval = CommError;
                }
            } else if (e == -2) { // TIMEOUT_ERROR
                retval = TimeoutError;
            } else {
                retval = CommError;
            }
	    if (verbose_print_enabled) {
		    fprintf(stderr, "fieldbusWrite(%d, %u, %u): %d,%d(%s) --> %d\n",
			    d, DataAddr, DataNumber,
			    e, errno, modbus_strerror(errno),
			    retval);
	    }
        }
        break;
    case CANOPEN:
        device = CrossTable[DataAddr].device;
        if (device != 0xffff) {
            u_int8_t channel = theDevices[device].port;
            for (i = 0; i < DataNumber; ++i) {
                register u_int32_t offset = CrossTable[DataAddr + i].Offset;

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
                    register u_int32_t offset = CrossTable[DataAddr + i].Offset;
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
                case UINT8:
                    e = CANopenWritePDOByte(channel, offset, DataValue[i]);
                    break;
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
			if (e == -1) { // OTHER_ERROR
			    retval = CommError;
			} else if (e == -2) { // TIMEOUT_ERROR
			    retval = TimeoutError;
			} else if (e == -3) { // RESET_ERROR
			    retval = ConnReset;
			} else {
			    retval = CommError;
			}
			break;
		}
	    }
        }
        break;
    case MECT: // fieldbusWrite()
        for (i = 0; i < DataNumber; ++i) {

            if (CrossTable[DataAddr + i].Types == REAL) {
                float value;
                memcpy(&value, &DataValue[i], sizeof(u_int32_t));
                e = mect_write_ascii(theDevices[d].mect_fd, CrossTable[DataAddr + i].NodeId, CrossTable[DataAddr + i].Offset, value);
            } else if (CrossTable[DataAddr + i].Types == UINT16) {
                unsigned value = DataValue[i];
                e = mect_write_hexad(theDevices[d].mect_fd, CrossTable[DataAddr + i].NodeId, CrossTable[DataAddr + i].Offset, value);
            }
            if (e == -1) {
                retval = CommError;
                break;
            } else if (e == -2) {
                retval = TimeoutError;
                break;
            }
        }
        break;
    default:
        ;
    }
    return retval;
}

static inline void changeServerStatus(u_int32_t s, enum ServerStatus status)
{
    theServers[s].status = status;
    setDiagnostic(theServers[s].diagnosticAddr, DIAGNOSTIC_STATUS, status);
#ifdef VERBOSE_DEBUG
    fprintf(stderr, "%s: status = %d\n", theServers[s].name, status);
#endif
}

static void startServerTiming(u_int32_t s)
{
    RTIME now_ns = rt_timer_read();

    theServers[s].idle_time_ns = 0;
    theServers[s].busy_time_ns = 0;
    theServers[s].last_time_ns = now_ns;
    theServers[s].last_update_ns = now_ns;
}

static inline void updateServerTiming(u_int32_t s)
{
    RTIME now_ns = rt_timer_read();

    // update idle/busy statistics each 5 seconds
    if ((now_ns - theServers[s].last_update_ns) > 5000000000ULL) {
        RTIME delta_ns;
        unsigned bus_load;

        // add last idle time
        delta_ns = now_ns - theServers[s].last_time_ns;
        theServers[s].idle_time_ns += delta_ns;

        // update diagnostics: busy load percentage is: (100 * busy_time / total_time)
        if (theServers[s].busy_time_ns == 0) {
            bus_load = 0;
        } else {
            lldiv_t x;

            x = lldiv((100 * theServers[s].busy_time_ns), (theServers[s].busy_time_ns + theServers[s].idle_time_ns));
            bus_load = x.quot;
        }
        setDiagnostic(theServers[s].diagnosticAddr, DIAGNOSTIC_BUS_LOAD, bus_load);

        // reset
        theServers[s].idle_time_ns = 0;
        theServers[s].busy_time_ns = 0;
        theServers[s].last_time_ns = now_ns;
        theServers[s].last_update_ns = now_ns;
    }
}

static inline void updateServerIdleTime(u_int32_t s)
{
    RTIME now_ns = rt_timer_read();

    RTIME delta_ns = now_ns - theServers[s].last_time_ns;
    theServers[s].idle_time_ns += delta_ns;
    theServers[s].last_time_ns = now_ns;
}

static inline void updateServerBusyTime(u_int32_t s)
{
    RTIME now_ns = rt_timer_read();

    RTIME delta_ns = now_ns - theServers[s].last_time_ns;
    theServers[s].busy_time_ns += delta_ns;
    theServers[s].last_time_ns = now_ns;
}

static void *serverThread(void *arg)
{
    u_int32_t s = (u_int32_t)arg;
    u_int8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int master_socket;
    int rc;
    fd_set refset;
    fd_set rdset;
    int fdmax = 0;
    int server_socket = -1;
    int threadInitOK = FALSE;
    struct timeval timeout_tv;

    // thread init
    // XX_GPIO_SET(4);
    osPthreadSetSched(FC_SCHED_IO_DAT, FC_PRIO_IO_DAT); // serverThread
    pthread_mutex_init(&theServers[s].mutex, NULL);
    timeout_tv.tv_sec = theServers[s].timeout_ms  / 1000;
    timeout_tv.tv_usec = (theServers[s].timeout_ms % 1000) * 1000;
    switch (theServers[s].protocol) {
    case RTU_SRV: {
        char device[VMM_MAX_PATH];

#if XENO_RTDM
        snprintf(device, VMM_MAX_PATH, "rtser%u", theServers[s].u.serial.port);
#else
        snprintf(device, VMM_MAX_PATH, "/dev/ttySP%u", theServers[s].u.serial.port);
#endif
        theServers[s].ctx = modbus_new_rtu(device, theServers[s].u.serial.baudrate,
                            theServers[s].u.serial.parity, theServers[s].u.serial.databits, theServers[s].u.serial.stopbits);
        theServers[s].mb_mapping = modbus_mapping_new(0, 0, REG_SRV_NUMBER, 0);
    }   break;
    case TCP_SRV: {
        char buffer[MAX_IPADDR_LEN];

        theServers[s].ctx = modbus_new_tcp(ipaddr2str(theServers[s].u.tcp_ip.IPaddr, buffer), theServers[s].u.tcp_ip.port);
        theServers[s].mb_mapping = modbus_mapping_new(0, 0, REG_SRV_NUMBER, 0);
    }   break;
    case TCPRTU_SRV: {
        char buffer[MAX_IPADDR_LEN];

        theServers[s].ctx = modbus_new_tcprtu(ipaddr2str(theServers[s].u.tcp_ip.IPaddr, buffer), theServers[s].u.tcp_ip.port);
        theServers[s].mb_mapping = modbus_mapping_new(0, 0, REG_SRV_NUMBER, 0);
    }   break;
    default:
        ;
    }
    if (theServers[s].ctx != NULL && theServers[s].mb_mapping != NULL
     && modbus_set_slave(theServers[s].ctx, theServers[s].NodeId) == 0) {
        threadInitOK = TRUE;
    }

    // run
    theServers[s].thread_status = RUNNING;
    if (!threadInitOK)
        changeServerStatus(s, SRV_RUNNING1);
    else
        changeServerStatus(s, SRV_RUNNING2);
    startServerTiming(s);

    while (engineStatus != enExiting) {

        // XX_GPIO_SET(4);
        updateServerTiming(s);

        // trivial scenario
        if (engineStatus != enRunning || !threadInitOK) {
            // XX_GPIO_CLR(4);
            osSleep(THE_CONNECTION_DELAY_ms);
            continue;
        }

        // get file descriptor or bind and listen
        if (server_socket == -1) {
            switch (theServers[s].protocol) {
            case RTU_SRV:
                if (modbus_connect(theServers[s].ctx)) {
                    server_socket = -1;
                } else {
                    server_socket = modbus_get_socket(theServers[s].ctx); // here socket is file descriptor
                    modbus_set_response_timeout(theServers[s].ctx, &timeout_tv);
                }
                break;
            case TCP_SRV:
                server_socket = modbus_tcp_listen(theServers[s].ctx, THE_SRV_MAX_CLIENTS);
                break;
            case TCPRTU_SRV:
                server_socket = modbus_tcprtu_listen(theServers[s].ctx, THE_SRV_MAX_CLIENTS);
                break;
            default:
                ;
            }
            if (server_socket >= 0) {
                FD_ZERO(&refset);
                FD_SET(server_socket, &refset);
                fdmax = server_socket;
                changeServerStatus(s, SRV_RUNNING3);
            }
        }
        if (server_socket < 0) {
            osSleep(THE_SERVER_DELAY_ms);
            continue;
        }

        // XX_GPIO_CLR(4);
        if (theServers[s].protocol == RTU_SRV) {
            // (single connection) force silence then read with timeout
            osSleep(theServers[s].silence_ms);
        } else {
            // (multiple connection) wait on server socket, only until timeout
            do {
                rdset = refset;
                timeout_tv.tv_sec = theServers[s].timeout_ms  / 1000;
                timeout_tv.tv_usec = (theServers[s].timeout_ms % 1000) * 1000;
                rc = select(fdmax+1, &rdset, NULL, NULL, &timeout_tv);
            } while ((rc == 0 || (rc < 0 && errno == EINTR)) && engineStatus != enExiting);
            // condition above for:
            // 1) avoid useless mode-switches (gatekeeper at high cpu%)
            // 2) optimize response time
            // 3) clean exiting

            if (rc == 0 || engineStatus == enExiting) { // timeout and/or exiting
                continue;
            } else if (rc < 0) { // error
                osSleep(THE_SERVER_DELAY_ms);
                continue;
            }
        }

        // XX_GPIO_SET(4);
        changeServerStatus(s, SRV_RUNNING4);
        updateServerIdleTime(s);

        // accept requests
#define _FC_READ_COILS                0x01
#define _FC_READ_DISCRETE_INPUTS      0x02
#define _FC_READ_HOLDING_REGISTERS    0x03
#define _FC_READ_INPUT_REGISTERS      0x04
#define _FC_WRITE_SINGLE_COIL         0x05
#define _FC_WRITE_SINGLE_REGISTER     0x06
#define _FC_READ_EXCEPTION_STATUS     0x07
#define _FC_WRITE_MULTIPLE_COILS      0x0F
#define _FC_WRITE_MULTIPLE_REGISTERS  0x10
#define _FC_REPORT_SLAVE_ID           0x11
#define _FC_MASK_WRITE_REGISTER       0x16
#define _FC_WRITE_AND_READ_REGISTERS  0x17
        switch (theServers[s].protocol) {
        case RTU_SRV:
            // unique client (serial line)
            rc = modbus_receive(theServers[s].ctx, query);
            if (rc > 0) {
                // XX_GPIO_CLR(4);
                osSleep(theServers[s].silence_ms);
                pthread_mutex_lock(&theServers[s].mutex);
                {
                    // XX_GPIO_SET(4);
                    switch (query[1]) {
                    case _FC_READ_COILS:
                    case _FC_READ_DISCRETE_INPUTS:
                    case _FC_READ_HOLDING_REGISTERS:
                    case _FC_READ_INPUT_REGISTERS:
                    case _FC_READ_EXCEPTION_STATUS:
                    case _FC_REPORT_SLAVE_ID:
                        incDiagnostic(theServers[s].diagnosticAddr, DIAGNOSTIC_READS);
                        break;
                    case _FC_WRITE_SINGLE_COIL:
                    case _FC_WRITE_SINGLE_REGISTER:
                    case _FC_WRITE_MULTIPLE_COILS:
                    case _FC_WRITE_MULTIPLE_REGISTERS:
                    case _FC_MASK_WRITE_REGISTER:
                        incDiagnostic(theServers[s].diagnosticAddr, DIAGNOSTIC_WRITES);
                        break;
                    case _FC_WRITE_AND_READ_REGISTERS:
                        incDiagnostic(theServers[s].diagnosticAddr, DIAGNOSTIC_WRITES);
                        incDiagnostic(theServers[s].diagnosticAddr, DIAGNOSTIC_READS);
                        break;
                    default:
                        break;
                    }
                    modbus_reply(theServers[s].ctx, query, rc, theServers[s].mb_mapping);
                    // XX_GPIO_CLR(4);
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
#ifdef VERBOSE_DEBUG
                        perror("Server accept() error");
#endif
                    } else {
                        FD_SET(newfd, &refset);

                        if (newfd > fdmax) {
                            fdmax = newfd;
                        }
#ifdef VERBOSE_DEBUG
                        fprintf(stderr, "New connection from %s:%d on socket %d\n",
                            inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
#endif
                    }
                } else {
                    modbus_set_socket(theServers[s].ctx, master_socket);
                    rc = modbus_receive(theServers[s].ctx, query);
                    if (rc > 0 && theServers[s].mb_mapping != NULL) {
                        // XX_GPIO_CLR(4);
                        pthread_mutex_lock(&theServers[s].mutex);
                        {
                            // XX_GPIO_SET(4);
                            switch (query[(theServers[s].protocol == TCP_SRV) ? 7 : 1]) {
                            case _FC_READ_COILS:
                            case _FC_READ_DISCRETE_INPUTS:
                            case _FC_READ_HOLDING_REGISTERS:
                            case _FC_READ_INPUT_REGISTERS:
                            case _FC_READ_EXCEPTION_STATUS:
                            case _FC_REPORT_SLAVE_ID:
                                incDiagnostic(theServers[s].diagnosticAddr, DIAGNOSTIC_READS);
                                break;
                            case _FC_WRITE_SINGLE_COIL:
                            case _FC_WRITE_SINGLE_REGISTER:
                            case _FC_WRITE_MULTIPLE_COILS:
                            case _FC_WRITE_MULTIPLE_REGISTERS:
                            case _FC_MASK_WRITE_REGISTER:
                                incDiagnostic(theServers[s].diagnosticAddr, DIAGNOSTIC_WRITES);
                                break;
                            case _FC_WRITE_AND_READ_REGISTERS:
                                incDiagnostic(theServers[s].diagnosticAddr, DIAGNOSTIC_WRITES);
                                incDiagnostic(theServers[s].diagnosticAddr, DIAGNOSTIC_READS);
                                break;
                            default:
                                ;
                            }
                            modbus_reply(theServers[s].ctx, query, rc, theServers[s].mb_mapping);
                            // XX_GPIO_CLR(4);
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
        changeServerStatus(s, SRV_RUNNING3);
        updateServerBusyTime(s);
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
        if (theServers[s].ctx != NULL) {
            modbus_close(theServers[s].ctx);
            modbus_free(theServers[s].ctx);
        }
        break;
    default:
        ;
    }

    // exit
    // XX_GPIO_CLR(4);
    fprintf(stderr, "EXITING: %s\n", theServers[s].name);
    theServers[s].thread_status = EXITING;
    return arg;
}

static void startDeviceTiming(u_int32_t d)
{
    RTIME now_ns = rt_timer_read();

    theDevices[d].elapsed_time_ns = 0;
    theDevices[d].current_time_ns = now_ns;

    theDevices[d].idle_time_ns = 0;
    theDevices[d].busy_time_ns = 0;
    theDevices[d].last_time_ns = now_ns;
    theDevices[d].last_update_ns = now_ns;
}

static inline void updateDeviceTiming(u_int32_t d)
{
    RTIME now_ns = rt_timer_read();

    RTIME delta_ns = now_ns - theDevices[d].current_time_ns;
    theDevices[d].elapsed_time_ns += delta_ns;
    theDevices[d].current_time_ns = now_ns;

    // update idle/busy statistics each 5 seconds
    if ((now_ns - theDevices[d].last_update_ns) > 5000000000ULL) {
        unsigned bus_load;

        // add last idle time
        delta_ns = now_ns - theDevices[d].last_time_ns;
        theDevices[d].idle_time_ns += delta_ns;

        // update diagnostics: busy load percentage is: (100 * busy_time / total_time)
        if (theDevices[d].busy_time_ns == 0) {
            bus_load = 0;
        } else {
            lldiv_t x;

            x = lldiv((100 * theDevices[d].busy_time_ns), (theDevices[d].busy_time_ns + theDevices[d].idle_time_ns));
            bus_load = x.quot;
        }
        setDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_BUS_LOAD, bus_load);
        // reset
        theDevices[d].idle_time_ns = 0;
        theDevices[d].busy_time_ns = 0;
        theDevices[d].last_time_ns = now_ns;
        theDevices[d].last_update_ns = now_ns;
    }
}

static inline void updateDeviceIdleTime(u_int32_t d)
{
    RTIME now_ns = rt_timer_read();
    RTIME delta_ns = now_ns - theDevices[d].last_time_ns;

    theDevices[d].idle_time_ns += delta_ns;
    theDevices[d].last_time_ns = now_ns;
}

static inline void updateDeviceBusyTime(u_int32_t d)
{
    RTIME now_ns = rt_timer_read();

    RTIME delta_ns = now_ns - theDevices[d].last_time_ns;
    theDevices[d].busy_time_ns += delta_ns;
    theDevices[d].last_time_ns = now_ns;
}

static inline void changeDeviceStatus(u_int32_t d, enum DeviceStatus status)
{
    u_int16_t addr;
    int n;
    enum DeviceStatus previous_status;

    if (verbose_print_enabled) {
        fprintf(stderr, "%s: status = %s\n", theDevices[d].name, deviceStatusName[status]);
    }
    previous_status = theDevices[d].status;
    theDevices[d].status = status;
    setDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_STATUS, status);

    theDevices[d].elapsed_time_ns = 0;

    switch (status) {
    case ZERO:
    case NO_HOPE:
        break;
    case NOT_CONNECTED:
        if (previous_status == CONNECTED_WITH_ERRORS) {
            // set to 0 the input variables for all device nodes
            for (n = 0; n < theDevices[d].var_num; ++n) {
                addr = theDevices[d].device_vars[n].addr;
                if (! CrossTable[addr].Output) {
                    writeQdataRegisters(addr, 0, DATA_ERROR);
                }
            }
            // stop the device nodes on connection failure
            for (n = 0; n < theNodesNumber; ++n) {
                if (theNodes[n].device == d && theNodes[n].status != DISCONNECTED) {
                    changeNodeStatus(d, n, DISCONNECTED);
                }
            }
        }
        break;
    case DEVICE_BLACKLIST:
        if (previous_status == NOT_CONNECTED) {
            // stop the device nodes on connection failure
            for (n = 0; n < theNodesNumber; ++n) {
                if (theNodes[n].device == d && theNodes[n].status != DISCONNECTED) {
                    changeNodeStatus(d, n, DISCONNECTED);
                }
            }
        }
        break;
    case CONNECTED:
        if (previous_status == NOT_CONNECTED) {
            // try resurrecting the device nodes on connection success
            for (n = 0; n < theNodesNumber; ++n) {
                if (theNodes[n].device == d && theNodes[n].status != NODE_OK) {
                    changeNodeStatus(d, n, NODE_OK);
                }
            }
        }
        break;
    case CONNECTED_WITH_ERRORS:
        break;
    default:
        ;
    }
}

static inline void changeNodeStatus(u_int32_t d, u_int16_t node, enum NodeStatus status)
{
    u_int16_t addr;
    int n;

    if (verbose_print_enabled) {
        fprintf(stderr, "node #%02u: status = %s\n", node+1, nodeStatusName[status]);
    }
    theNodes[node].status = status;
    setDiagnostic(theNodes[node].diagnosticAddr, DIAGNOSTIC_NODE_STATUS, theNodes[node].status);

    switch (status) {
    case NO_NODE:
    case NODE_OK:
        break;
    case TIMEOUT:
        theNodes[node].retries = 0;
        break;
    case BLACKLIST:
        theNodes[node].blacklist = system_ini.system.blacklist;
        // zeroNodeVariables
        for (n = 0; n < theDevices[d].var_num; ++n) {
            addr = theDevices[d].device_vars[n].addr;
            if (CrossTable[addr].node == node && ! CrossTable[addr].Output) {
#ifdef VERBOSE_DEBUG
                fprintf(stderr, "\tnode #%02u, addr #%04u(%s): value=0 status = DATA_ERROR\n", node+1, addr, CrossTable[addr].Tag);
#endif
                writeQdataRegisters(addr, 0, DATA_ERROR);
            }
        }
        break;
    case DISCONNECTED:
    default:
        ;
    }
}

static unsigned checkAddr(unsigned d, unsigned DataAddr, unsigned DataNumber)
{
    unsigned n;

    if (DataAddr == 0 || DataAddr > DimCrossTable) {
        fprintf(stderr, "clientThread(%u) wrong DataAddr %u[%u]\n", d, DataAddr, DataNumber);
        return 0;
    } else if (DataNumber == 0 || DataNumber > MAX_VALUES || (DataAddr + DataNumber) > DimCrossTable) {
        fprintf(stderr, "clientThread(%u) wrong DataNumber %u[%u]\n", d, DataAddr, DataNumber);
        return 0;
    } else {
        for (n = 0; n < DataNumber; ++n) {
            if (CrossTable[DataAddr + n].device != d) {
                fprintf(stderr, "clientThread(%u) wrong device %u @(%u+%u)\n", d, CrossTable[DataAddr + n].device, DataAddr, n);
                return 0;
            }
        }
    }
    return DataAddr;
}

static void *clientThread(void *arg)
{
    u_int32_t d = (u_int32_t)arg;
    u_int16_t v;

    // device connection management
    struct timeval response_timeout;

    // device queue management by priority and periods
    u_int16_t prio;                         // priority of variables
    u_int16_t write_index;                  // current write index in queue
    int we_have_variables[MAX_PRIORITY];    // do we have variables at that priority?
    u_int16_t read_addr[MAX_PRIORITY];      // current read address in crosstable for each priority
    RTIME read_time_ns[MAX_PRIORITY];   // next read time for each priority

    // data for each fieldbus operation
    u_int16_t QueueIndex = 0;// command index in the queue
    u_int16_t readOperation; // read/write
    u_int16_t DataAddr = 0;  // variable address in the crosstable
    u_int32_t DataValue[MAX_VALUES]; // max 64 reads and 16 writes
    u_int32_t DataNumber;    // max 64 reads and 16 writes
    u_int16_t DataNodeId;    // variable node ID ("0" if not applicable)
    u_int16_t Data_node;     // global index of the node in theNodes[]
    enum fieldbusError error;


    // ------------------------------------------ thread init
    // XX_GPIO_SET_69(d);
    osPthreadSetSched(FC_SCHED_IO_DAT, FC_PRIO_IO_DAT); // clientThread
    theDevices[d].modbus_ctx = NULL;
    theDevices[d].mect_fd = -1;
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
    case TCP: {
        char buffer[MAX_IPADDR_LEN];

        theDevices[d].modbus_ctx = modbus_new_tcp(ipaddr2str(theDevices[d].u.tcp_ip.IPaddr, buffer), theDevices[d].u.tcp_ip.port);
    }   break;
    case TCPRTU: {
        char buffer[MAX_IPADDR_LEN];

        theDevices[d].modbus_ctx = modbus_new_tcprtu(ipaddr2str(theDevices[d].u.tcp_ip.IPaddr, buffer), theDevices[d].u.tcp_ip.port);
    }   break;
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
        if (verbose_print_enabled) {
            CANopenList(theDevices[d].u.can.bus);
        }
    }   break;
    case MECT: // new (nothing to do)
        break;
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
    case MECT: // check state (nothing to do)
        changeDeviceStatus(d, NOT_CONNECTED);
        break;
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        changeDeviceStatus(d, NOT_CONNECTED);
        break;
    default:
        ;
    }

    // ------------------------------------------ run
    fprintf(stderr, "%s: ", theDevices[d].name);
    switch (theDevices[d].protocol) {
    case PLC: // FIXME: assert
        break;
    case RTU:
    case MECT:
        fprintf(stderr, "@%u/%u/%c/%u, ", theDevices[d].u.serial.baudrate, theDevices[d].u.serial.databits, theDevices[d].u.serial.parity, theDevices[d].u.serial.stopbits);
        break;
    case TCP:
    case TCPRTU:
        break;
    case CANOPEN:
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
        read_time_ns[prio] = theDevices[d].current_time_ns;
        read_addr[prio] = 0;
        we_have_variables[prio] = 0;
    }
    // for each priority check if we have variables at that priority
    for (v = 0; v < theDevices[d].var_num; ++v) {
        DataAddr = theDevices[d].device_vars[v].addr;
        if (CrossTable[DataAddr].Enable > 0 && CrossTable[DataAddr].Enable <= MAX_PRIORITY) {
            prio = CrossTable[DataAddr].Enable - 1;
            we_have_variables[prio] = 1; // also Htype
        }
    }
    if (verbose_print_enabled) {
        fprintf(stderr, "%s: reading variables {", theDevices[d].name);
        for (prio = 0; prio < MAX_PRIORITY; ++prio) {
            if (we_have_variables[prio]) {
                fprintf(stderr, "\n\t%u:", prio + 1);
                for (v = 0; v < theDevices[d].var_num; ++v) {
                    if (CrossTable[theDevices[d].device_vars[v].addr].Enable == (prio + 1)) {
                        fprintf(stderr, " %u%s", theDevices[d].device_vars[v].addr, theDevices[d].device_vars[v].active ? "" : "(H)");
                    }
                }
            }
        }
        fprintf(stderr, "\n}\n");
    }

    // start the fieldbus operations loop
    DataAddr = 0;
    DataNumber = 0;
    readOperation = TRUE;
    error = NoError;

    // pre-charge the output retentives
    doWriteDeviceRetentives(d);

    // let the engine continue
    theDevices[d].thread_status = RUNNING;

    while (engineStatus != enExiting) {

        int do_fieldbusReset = FALSE;

        // XX_GPIO_SET_69(d);
        updateDeviceTiming(d);

        // trivial scenario
        if (engineStatus != enRunning || theDevices[d].status == NO_HOPE) {
            // XX_GPIO_CLR_69(d);
            osSleep(THE_CONNECTION_DELAY_ms);
            continue;
        }

        // was I already doing something?
        if (DataAddr == 0) {
            int rc;
            RTIME next_ns;

            // wait for next operation or next programmed read
            next_ns = theDevices[d].current_time_ns + THE_MAX_CLIENT_SLEEP_ns;
            for (prio = 0; prio < MAX_PRIORITY; ++prio) {
                if (we_have_variables[prio]) {
                    if (read_time_ns[prio] < next_ns) {
                        next_ns = read_time_ns[prio];
                    }
                }
            }
            if (next_ns > theDevices[d].current_time_ns) {
                int timeout, invalid_timeout, invalid_permission, other_error;
                struct timespec abstime;
                TIMESPEC_FROM_RTIME(abstime, next_ns);

                do {
                    int saved_errno;
                    // XX_GPIO_CLR_69(d);
                    rc = sem_timedwait(&newOperations[d], &abstime);
                    saved_errno = errno;
                    // XX_GPIO_SET_69(d);

                    timeout = invalid_timeout = invalid_permission = other_error = FALSE;
                    errno = saved_errno;
                    if (errno ==  EINVAL) {
                        fprintf(stderr, "%s@%09llu ms: problem with (%us, %ldns).\n",
                            theDevices[d].name, theDevices[d].current_time_ns, abstime.tv_sec, abstime.tv_nsec);
                        invalid_timeout = TRUE;
                        break;
                    }
                    if (errno == EINTR) {
                        continue;
                    }
                    if (rc == 0) {
                        break;
                    }
                    if (errno ==  ETIMEDOUT) {
                        timeout = TRUE;
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
                if (invalid_timeout || invalid_permission || other_error) {
                    fprintf(stderr, "%s@%09u ms: woke up because %s (%09u ms = %u s + %d ns)\n", theDevices[d].name, theDevices[d].current_time_ms,
                        timeout?"timeout":(invalid_timeout?"invalid_timeout":(invalid_permission?"invalid_permission":(other_error?"other_error":"signal"))),
                        next_ms, abstime.tv_sec, abstime.tv_nsec);
                }
#endif
            } else {
#ifdef VERBOSE_DEBUG
                fprintf(stderr, "%s@%09u ms: immediate restart\n", theDevices[d].name, theDevices[d].current_time_ms);
#endif
            }
        }

        // choose the read/write command either in the local queue or in the queue from HMI
        if (DataAddr == 0) {
            // XX_GPIO_CLR_69(d);
            pthread_mutex_lock(&theCrosstableClientMutex);
            {
                // XX_GPIO_SET_69(d);

                // is it there an immediate write requests from PLC to this device?
                if (theDevices[d].PLCwriteRequestNumber > 0) {
                    u_int16_t n;

                    QueueIndex = 0;
                    DataAddr = theDevices[d].PLCwriteRequests[theDevices[d].PLCwriteRequestGet].Addr;
                    // data values from the local queue only, the *_BIT management is in fieldWrite()
                    DataNumber = theDevices[d].PLCwriteRequests[theDevices[d].PLCwriteRequestGet].Number;
                    readOperation = FALSE;
                    // check
                    DataAddr = checkAddr(d, DataAddr, DataNumber);
                    if (DataAddr > 0) {
                        for (n = 0; n < DataNumber; ++n) {
                            DataValue[n] = theDevices[d].PLCwriteRequests[theDevices[d].PLCwriteRequestGet].Values[n];
                        }
                    }

                    // write requests circular buffer (even if check failed)
                    theDevices[d].PLCwriteRequestGet = (theDevices[d].PLCwriteRequestGet + 1) % MaxLocalQueue;
                    theDevices[d].PLCwriteRequestNumber -= 1;

                    setDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_WRITE_QUEUE, theDevices[d].PLCwriteRequestNumber);
#ifdef VERBOSE_DEBUG
                    fprintf(stderr, "%s@%09u ms: write PLC [%u], there are still %u\n", theDevices[d].name, theDevices[d].current_time_ms, DataAddr, theDevices[d].PLCwriteRequestNumber);
#endif
                }

                // if no write then is it there a read request for this device?
                if (DataAddr == 0) {
                    u_int16_t addr;

                    // periodic read requests:
                    // -- {P,S,F} automatically enabled from Crosstable
                    // -- {H} enabled from HMI when the page is displayed
                    for (prio = 0; prio < MAX_PRIORITY; ++prio) {
                        // avoid checking if we know it's pointless
                        if (! we_have_variables[prio]) {
                            continue;
                        }
                        // only when the timer expires
                        if (read_time_ns[prio] <= theDevices[d].current_time_ns) {

                            // is it there anything to read at this priority for this device?
                            int found = FALSE;

                            // {P,S,F} and active {H}
                            for (v = read_addr[prio]; v < theDevices[d].var_num; ++v) {
                                if (theDevices[d].device_vars[v].active) {
                                    addr = theDevices[d].device_vars[v].addr;
                                    if (CrossTable[addr].Enable == (prio + 1) && addr == CrossTable[addr].BlockBase) {
                                        found = TRUE;
                                        break;
                                    }
                                }
                            }
                            if (found) {
                                QueueIndex = 0;
                                DataAddr = addr; // CrossTable[addr].BlockBase;
                                DataNumber = CrossTable[addr].BlockSize;
                                if (DataNumber > MAX_VALUES) {
                                    fprintf(stderr, "clientThread(%d) (HMI) DataNumber = %u\n", d, DataNumber);
                                    DataNumber = MAX_VALUES;
                                }
                                readOperation = TRUE;
                                // data values will be available after the fieldbus access
                                // keep the index for the next loop
                                read_addr[prio] = v + DataNumber; // may overlap DimCrossTable, it's ok
#ifdef VERBOSE_DEBUG
                                fprintf(stderr, "%s@%09u ms: read %uPSF [%u] (was [%u]), will check [%u]\n", theDevices[d].name, theDevices[d].current_time_ms, prio+1, DataAddr, addr, read_addr[prio]);
#endif
                                break;
                            } else {
                                // compute next tic for this priority, restarting from the first
                                RTIME period_ns = system_ini.system.read_period_ms[prio] * 1E6;
                                read_time_ns[prio] += period_ns;
                                if (read_time_ns[prio] <= theDevices[d].current_time_ns) {
                                    RTIME n = theDevices[d].current_time_ns / period_ns;
                                    read_time_ns[prio] = (n + 1) * period_ns;
                                }
                                read_addr[prio] = 0;
#ifdef VERBOSE_DEBUG
                                fprintf(stderr, "%s@%09u ms: no read %uHPSF will restart at %09u ms\n", theDevices[d].name, theDevices[d].current_time_ms, prio+1, read_time_ms[prio]);
#endif
                            }
                        }
                    }
                }
                // XX_GPIO_CLR_69(d);
            }
            pthread_mutex_unlock(&theCrosstableClientMutex);
        }

        if (DataAddr == 0) {
            // nothing to do
            continue;
        }
        DataAddr = checkAddr(d, DataAddr, DataNumber);
        if (DataAddr == 0) {
            // skip
            continue;
        }

        // XX_GPIO_SET_69(d);
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
                fprintf(stderr, "%s modbus_connect()\n", theDevices[d].name);
                if (modbus_connect(theDevices[d].modbus_ctx) >= 0) {
                    modbus_set_response_timeout(theDevices[d].modbus_ctx, &response_timeout);
                    changeDeviceStatus(d, CONNECTED);
                }
                break;
            case TCP:
            case TCPRTU:
                if (modbus_connect(theDevices[d].modbus_ctx) >= 0) {
                    if (modbus_flush(theDevices[d].modbus_ctx) >= 0) {
                        modbus_set_response_timeout(theDevices[d].modbus_ctx, &response_timeout);
                        changeDeviceStatus(d, CONNECTED);
                    } else {
                        modbus_close(theDevices[d].modbus_ctx);
                    }
                }
                break;
            case CANOPEN: {
                u_int8_t channel = theDevices[d].u.can.bus;
                if (CANopenConfigured(channel)) {
                    changeDeviceStatus(d, CONNECTED);
                }
            }   break;
            case MECT: // connect()
                theDevices[d].mect_fd = mect_connect(
                    theDevices[d].u.serial.port,
                    theDevices[d].u.serial.baudrate,
                    theDevices[d].u.serial.parity,
                    theDevices[d].u.serial.databits,
                    theDevices[d].u.serial.stopbits,
                    theDevices[d].timeout_ms);
                if (theDevices[d].mect_fd >= 0) {
                    changeDeviceStatus(d, CONNECTED);
                }
                break;
            case RTU_SRV:
            case TCP_SRV:
            case TCPRTU_SRV:
                changeDeviceStatus(d, CONNECTED);
                break;
            default:
                ;
            }
            // if connection attempts fail too much, then rest a bit
            if (theDevices[d].status == NOT_CONNECTED && theDevices[d].elapsed_time_ns >= THE_DEVICE_SILENCE_ns) {
                changeDeviceStatus(d, DEVICE_BLACKLIST); // also data=0 and status=DATA_ERROR
            }
            break;
        case CONNECTED:
            // ok proceed with the fieldbus operations
            break;
        case CONNECTED_WITH_ERRORS:
            // ok proceed with the fieldbus operations
            break;
        case DEVICE_BLACKLIST:
            if (theDevices[d].elapsed_time_ns >= THE_DEVICE_BLACKLIST_ns) {
                changeDeviceStatus(d, NOT_CONNECTED);
            }
            break;
        default:
            ;
        }

        // can we continue?
        if (theDevices[d].status != CONNECTED && theDevices[d].status != CONNECTED_WITH_ERRORS) {
            // XX_GPIO_CLR_69(d);
            osSleep(THE_CONNECTION_DELAY_ms);
            continue;
        }

        // check the node status before actually operating
        if (Data_node == 0xffff) {
            error = TimeoutError;

        } else if (theNodes[Data_node].status == DISCONNECTED || theNodes[Data_node].status == BLACKLIST) {
            error = TimeoutError;
#ifdef VERBOSE_DEBUG
              if (theDevices[d].protocol == RTU /*&& theDevices[d].port == 0 && theDevices[d].u.serial.baudrate == 38400*/) {
                fprintf(stderr, "%s@%09u ms: %s (blacklist) %u vars @ %u\n",
                        theDevices[d].name, theDevices[d].current_time_ms,
                        Operation == READ ? "read" : "write",
                        DataNumber, DataAddr);
              }
#endif
        } else {

            // the device is connected, so operate, without locking the mutex
            updateDeviceIdleTime(d);
            if (readOperation) {
                incDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_READS);
                error = fieldbusRead(d, DataAddr, DataValue, DataNumber);
            } else {
                incDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_WRITES);
                if (CrossTable[DataAddr].Output) {
                    error = fieldbusWrite(d, DataAddr, DataValue, DataNumber);
                } else {
                    error = CommError;
                }
            }
            updateDeviceBusyTime(d);

            // fieldbus wait silence_ms afterwards
        }
        setDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_LAST_ERROR, error);

        // check error and set values and flags
        // XX_GPIO_CLR_69(d);
        pthread_mutex_lock(&theCrosstableClientMutex);
        {
            u_int16_t i;

            // XX_GPIO_SET_69(d);
#ifdef VERBOSE_DEBUG
            if (theDevices[d].protocol == RTU /*&& theDevices[d].port == 0 && theDevices[d].u.serial.baudrate == 38400*/) {
            fprintf(stderr, "%s@%09u ms: %s %s %u vars @ %u\n", theDevices[d].name, theDevices[d].current_time_ms,
                    Operation == READ ? "read" : "write",
                    error == NoError ? "ok" : "error",
                    DataNumber, DataAddr);
            }
#endif
            // manage the data values, data status, node status and device status
            switch (error) {

            case NoError:
                // node status
                if (theNodes[Data_node].status != NODE_OK)
                    changeNodeStatus(d, Data_node, NODE_OK);
                // device status
                if (theDevices[d].status != CONNECTED)
                    changeDeviceStatus(d, CONNECTED);
                // data values and status
                for (i = 0; i < DataNumber; ++i) {
                    writeQdataRegisters(DataAddr + i, DataValue[i], DATA_OK);
                }
                DataAddr = 0; // i.e. get next
                break;

            case CommError:
                incDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_COMM_ERRORS);
                // node status
                switch (theNodes[Data_node].status) {
                case NODE_OK:
                    if (readOperation) {
                        // data values and status
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_WARNING);
                        }
                        DataAddr = 0; // i.e. get next
                    } else if (system_ini.system.retries > 0) {
                        // write operation and we can retry
                        changeNodeStatus(d, Data_node, TIMEOUT);
                        // data values and status
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_WARNING);
                        }
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                    } else {
                        // write operation and we cannot retry
                        changeNodeStatus(d, Data_node, BLACKLIST); // also data=0 status=DATA_ERROR
                        DataAddr = 0; // i.e. we lose a write
                    }
                    break;
                case TIMEOUT:
                    theNodes[Data_node].retries += 1;
                    if (readOperation) {
                        changeNodeStatus(d, Data_node, NODE_OK);
                        // data values and status
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_WARNING);
                        }
                        DataAddr = 0; // i.e. get next
                    } else if (theNodes[Data_node].retries < system_ini.system.retries) {
                        // write operation and we can retry
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_WARNING);
                        }
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                    } else {
                        // write operation and we cannot retry
                        changeNodeStatus(d, Data_node, BLACKLIST); // also data=0 status=DATA_ERROR
                        DataAddr = 0; // i.e. we lose a write
                    }
                    break;
                case BLACKLIST:
                case DISCONNECTED:
                default:
                    ;
                }
                // device status
                if (theDevices[d].status == CONNECTED_WITH_ERRORS) {
                    // we received something, even if wrong
                    changeDeviceStatus(d, CONNECTED);
                }
                break;

            case TimeoutError:
                incDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_TIMEOUTS);
                // node status
                switch (theNodes[Data_node].status) {
                case NODE_OK:
                    if (system_ini.system.retries > 0) {
                        // any operation and we can retry
                        changeNodeStatus(d, Data_node, TIMEOUT);
                        // data values and status
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_WARNING);
                        }
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                    } else {
                        // any operation and we cannot retry
                        changeNodeStatus(d, Data_node, BLACKLIST); // also data=0 status=DATA_ERROR
                        if (readOperation) {
                            DataAddr = 0; // i.e. retry this after the others
                        } else {
                            DataAddr = 0; // i.e. we lose a write
                        }
                    }
                    break;
                case TIMEOUT:
                    theNodes[Data_node].retries += 1;
                    if (theNodes[Data_node].retries < system_ini.system.retries) {
                        // any operation and we can retry
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_WARNING);
                        }
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                    } else {
                        // any operation and we cannot retry
                        changeNodeStatus(d, Data_node, BLACKLIST); // also data=0 status=DATA_ERROR
                        if (readOperation) {
                            DataAddr = 0; // i.e. retry this after the others
                        } else {
                            DataAddr = 0; // i.e. we lose a write
                        }
                    }
                    break;
                case BLACKLIST:
                    theNodes[Data_node].blacklist -= 1;
                    // no fieldbus operations, so no error
                    if (theNodes[Data_node].blacklist > 0) {
                        // data values and status
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_ERROR);
                        }
                        if (readOperation) {
                            DataAddr = 0; // i.e. retry this after the others
                        } else {
                            DataAddr = 0; // i.e. we lose a write
                        }
                    } else {
                        changeNodeStatus(d, Data_node, TIMEOUT);
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                    }
                    break;
                case DISCONNECTED:
                    // data values and status
                    for (i = 0; i < DataNumber; ++i) {
                        writeQdataRegisters(DataAddr + i, 0, DATA_ERROR);
                    }
                    if (readOperation) {
                        DataAddr = 0; // i.e. retry this after the others
                    } else {
                        DataAddr = 0; // i.e. we lose a write
                    }
                    break;
                default:
                    ;
                }
                // device status
                if (theDevices[d].status == CONNECTED) {
                    changeDeviceStatus(d, CONNECTED_WITH_ERRORS);
                } else {
                    // maybe we need a reset?
                    updateDeviceTiming(d);
                    if (theDevices[d].elapsed_time_ns > THE_DEVICE_SILENCE_ns) {
                        // too much silence
                        do_fieldbusReset = TRUE;
                        changeDeviceStatus(d, NOT_CONNECTED); // also data=0 status=DATA_ERROR
                    }
                }
                break;

            case ConnReset:
                incDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_TIMEOUTS); // TIMEOUTS(RESET)
                // node status
                changeNodeStatus(d, Data_node, DISCONNECTED);
                // data values and status
                for (i = 0; i < DataNumber; ++i) {
                    writeQdataRegisters(DataAddr + i, 0, DATA_ERROR);
                }
                if (readOperation) {
                    DataAddr = 0; // i.e. retry this after the others
                } else {
                    DataAddr = 0; // i.e. we lose a write
                }
                // device status
                do_fieldbusReset = TRUE;
                changeDeviceStatus(d, NOT_CONNECTED); // also data=0 status=DATA_ERROR
                break;

            default:
                ;
            }
            // XX_GPIO_CLR_69(d);
        }
        pthread_mutex_unlock(&theCrosstableClientMutex);

        // if necessary reset, with unlocked mutex
        if (do_fieldbusReset) {
            fieldbusReset(d);
        }

        // if necessary respect the silence, with unlocked mutex
        switch (theDevices[d].protocol) {
        case RTU:
        case MECT:
            if (theDevices[d].silence_ms > 0) {
                // XX_GPIO_CLR_69(d);
                osSleep(theDevices[d].silence_ms);
                // XX_GPIO_SET_69(d);
            }
            break;
        default:
            ;
        }

        // while
    }
    // XX_GPIO_SET_69(d);

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
    case MECT: // close()
        mect_close(theDevices[d].mect_fd);
        break;
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        break;
    default:
        ;
    }

    // cleanup
    if (theDevices[d].device_vars != NULL) {
        free(theDevices[d].device_vars);
        theDevices[d].device_vars = NULL;
    }
    // exit
    // XX_GPIO_CLR_69(d);
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
    // XX_GPIO_SET(2);
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
    while (engineStatus != enExiting) {

        // trivial scenario
        // XX_GPIO_SET(2);
        if ((engineStatus != enRunning && engineStatus != enError) || !threadInitOK) {
            // XX_GPIO_CLR(2);
            osSleep(THE_UDP_TIMEOUT_ms);
            continue;
        }

        fd_set recv_set;
        struct timeval tv;
        int rc;

        // (1) data recv, only until timeout
        FD_ZERO(&recv_set);
        FD_SET(dataRecvSock, &recv_set);
        tv.tv_sec = THE_UDP_TIMEOUT_ms / 1000;
        tv.tv_usec = (THE_UDP_TIMEOUT_ms % 1000) * 1000;
        // XX_GPIO_CLR(2);
        if (select(dataRecvSock + 1, &recv_set, NULL, NULL, &tv) <= 0) {
            // timeout or error
            continue;
        }
        // XX_GPIO_SET(2);
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
        // XX_GPIO_CLR(2);
        if (select(syncRecvSock + 1, &recv_set, NULL, NULL, &tv) <= 0) {
            // timeout or error
            continue;
        }
        // XX_GPIO_SET(2);
        rc = recv(syncRecvSock, the_UsyncBuffer, THE_SYNC_UDP_SIZE, 0);
        if (rc != THE_SYNC_UDP_SIZE) {
            // error recovery
            continue;
        }

        // (3) compute data sync
        // XX_GPIO_CLR(2);
        pthread_mutex_lock(&theCrosstableClientMutex);
        {
            if (engineStatus != enExiting) {
                // XX_GPIO_SET(2);
                memcpy(the_IdataRegisters, the_UdataBuffer, sizeof(the_IdataRegisters));
                memcpy(the_IsyncRegisters, the_UsyncBuffer, sizeof(the_IsyncRegisters));
                PLCsync();
                memcpy(the_UdataBuffer, the_QdataRegisters, sizeof(the_UdataBuffer));
                memcpy(the_UsyncBuffer, the_QsyncRegisters, sizeof(the_UsyncBuffer));
                // XX_GPIO_CLR(2);
            }
        }
        pthread_mutex_unlock(&theCrosstableClientMutex);

        // (4) data send
        // XX_GPIO_SET(2);
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
        // XX_GPIO_CLR(2);
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
    // XX_GPIO_CLR(2);
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
    int s, d, n;

    // read the configuration file
    if (app_config_load(&system_ini)) {
        fprintf(stderr, "[%s]: Error loading config file.\n", __func__);
        system_ini_ok = FALSE;
    } else {
        system_ini_ok = TRUE;
    }
    // non null default values
    for (n = 0; n < MAX_SERIAL_PORT; ++n) {
        if (system_ini.serial_port[n].max_block_size <= 0 || system_ini.serial_port[n].max_block_size > MAX_VALUES)
            system_ini.serial_port[n].max_block_size = MAX_VALUES;
    }
    if (system_ini.tcp_ip_port.max_block_size <= 0 || system_ini.tcp_ip_port.max_block_size > MAX_VALUES)
        system_ini.tcp_ip_port.max_block_size = MAX_VALUES;
    for (n = 0; n < MAX_CANOPEN; ++n) {
        if (system_ini.canopen[n].max_block_size <= 0 || system_ini.canopen[n].max_block_size> MAX_VALUES)
            system_ini.canopen[n].max_block_size = MAX_VALUES;
    }

    if (verbose_print_enabled) {
        app_config_dump(&system_ini);
    }

    // cleanup variables
    bzero(the_QdataRegisters, sizeof(the_QdataRegisters));
    bzero(the_IdataRegisters, sizeof(the_IdataRegisters));
    bzero(the_QsyncRegisters, sizeof(the_QsyncRegisters));
    bzero(the_IsyncRegisters, sizeof(the_IsyncRegisters));

    // default values
    the_QdataRegisters[PLC_Version] = REVISION_HI * 1000 + REVISION_LO;
    the_QdataRegisters[PLC_BEEP_VOLUME] = 0x00000064; // duty=100%
    the_QdataRegisters[PLC_TOUCH_VOLUME] = 0x00000064; // duty=100%
    the_QdataRegisters[PLC_ALARM_VOLUME] = 0x00000064; // duty=100%

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
    pthread_mutex_init(&theCrosstableClientMutex, NULL);
    pthread_cond_init(&theAlarmsEventsCondvar, NULL);
    for (s = 0; s < MAX_SERVERS; ++s) {
        theServers[s].thread_id = -1;
    }
    for (d = 0; d < MAX_DEVICES; ++d) {
        theDevices[d].thread_id = -1;
    }

    // start the engine thread
    if (osPthreadCreate(&theEngineThread_id, NULL, &engineThread, &theEngineThreadStatus, "engine", 0) == 0) {
        do {
            osSleep(THE_CONFIG_DELAY_ms); // not sched_yield();
        } while (theEngineThreadStatus != RUNNING);
    }
}

void dataEngineStop(void)
{
    void *retval;
    int n;

    if (engineStatus == enIdle) {
        // SIGINT arrived before initialization
        return;
    }
    setEngineStatus(enExiting);

    for (n = 0; n < theDevicesNumber; ++n) {
        if (theDevices[n].thread_id != -1) {
            pthread_join(theDevices[n].thread_id, &retval);
            theDevices[n].thread_id = -1;
            fprintf(stderr, "joined dev(%d)\n", n);
        }
    }
    for (n = 0; n < theServersNumber; ++n) {
        if (theServers[n].thread_id != -1) {
            pthread_join(theServers[n].thread_id, &retval);
            theServers[n].thread_id = -1;
        fprintf(stderr, "joined srv(%d)\n", n);
        }
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

static unsigned doWriteVariable(unsigned addr, unsigned value, u_int32_t *values, u_int32_t *flags, unsigned addrMax)
{
    unsigned retval = 0;

    if (CrossTable[addr].Protocol == PLC) {

        // PLC only, immediate writing
        if (CrossTable[addr].Types == BIT || CrossTable[addr].Types == BYTE_BIT
            || CrossTable[addr].Types == WORD_BIT || CrossTable[addr].Types == DWORD_BIT ) {
            // the engine seems to use only the bit#0 for bool variables
            if (value & 1)
                writeQdataRegisters(addr, 1, DATA_OK);
            else
                writeQdataRegisters(addr, 0, DATA_OK);
        }
        else
            writeQdataRegisters(addr, values[addr], DATA_OK);

        // wrote one
        retval = 1;
    }
    else {
        // RTU, TCP, TCPRTU, CANOPEN, MECT, RTU_SRV, TCP_SRV, TCPRTU_SRV
        u_int16_t d = CrossTable[addr].device;

        // FIXME: error recovery
        if (d >= theDevicesNumber) {
            fprintf(stderr, "%s() unknown device error, writing addr=%u to device=%u\n",
                 __func__, addr, d);
            // wrote none
            retval = 0;
        }
        else if (theDevices[d].PLCwriteRequestNumber >= MaxLocalQueue) {
            fprintf(stderr, "%s() buffer full error (%u), writing addr=%u to device=%u\n",
                 __func__, theDevices[d].PLCwriteRequestNumber, addr, d);
            // wrote none
            retval = 0;
        }
        else {
            register int base, size, type, node, n, offset;

            // write requests circular buffer
            register int i = theDevices[d].PLCwriteRequestPut;
            theDevices[d].PLCwriteRequestPut = (i + 1) % MaxLocalQueue;
            theDevices[d].PLCwriteRequestNumber += 1;

            // add the write request of 'value'
            setDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_WRITE_QUEUE, theDevices[d].PLCwriteRequestNumber);
            theDevices[d].PLCwriteRequests[i].Addr = addr;
            theDevices[d].PLCwriteRequests[i].Number = 1;

            if (CrossTable[addr].Types == BIT || CrossTable[addr].Types == BYTE_BIT
                || CrossTable[addr].Types == WORD_BIT || CrossTable[addr].Types == DWORD_BIT ) {
                // the engine uses only the bit#0 for bool variables
                if (value & 1)
                    theDevices[d].PLCwriteRequests[i].Values[0] = 1;
                else
                    theDevices[d].PLCwriteRequests[i].Values[0] = 0;
            } else
                theDevices[d].PLCwriteRequests[i].Values[0] = value;

            // wrote one
            retval = 1;

            if (values) {
                // are there any other consecutive writes to the same block?
                base = CrossTable[addr].BlockBase;
                size = CrossTable[addr].BlockSize;
                type = CrossTable[addr].Types;
                offset = CrossTable[addr].Offset;
                node = CrossTable[addr].node;

                for (n = 1; (addr + n) < (base + size) && (addr + n) <= addrMax; ++n)
                {
                    if (theDevices[d].PLCwriteRequests[i].Number >= MAX_VALUES) {
                        break;
                    }
                    // must be same device and node (should already be checked by Crosstable editor)
                    if (CrossTable[addr + n].device != d || CrossTable[addr + n].node != node) {
                        break;
                    }
                    // in Modbus clients we cannot mix BIT and non BIT variables
                    if ((type == RTU || type == TCP || type == TCPRTU || type == RTU_SRV || type == TCP_SRV || type == TCPRTU_SRV)
                        && ((type == BIT && CrossTable[addr + n].Types != BIT)
                           || (type != BIT && CrossTable[addr + n].Types == BIT))) {
                        break;
                    }
                    if (flags) {
                        // only sequential writes, with the exception of *_BIT of the same offset
                        if (flags[addr + n] == 0
                        && !((type == BYTE_BIT || type == WORD_BIT || type == DWORD_BIT)
                           && type == CrossTable[addr + n].Types && offset == CrossTable[addr + n].Offset)) {
                            break;
                        }
                    }

                    // ok, add another one
                    theDevices[d].PLCwriteRequests[i].Number = 1 + n;
                    if (flags && flags[addr + n] == 0) {
                        // in the exception of *_BIT, we get the actual value
                        theDevices[d].PLCwriteRequests[i].Values[n] = the_QdataRegisters[addr + n];
                    } else {
                        // normal case
                        if (CrossTable[addr + n].Types == BIT || CrossTable[addr + n].Types == BYTE_BIT
                         || CrossTable[addr + n].Types == WORD_BIT || CrossTable[addr + n].Types == DWORD_BIT ) {
                            // the engine uses only the bit#0 for bool variables
                            if (values[addr + n] & 1) {
                                theDevices[d].PLCwriteRequests[i].Values[n] = 1;
                            } else {
                                theDevices[d].PLCwriteRequests[i].Values[n] = 0;
                            }
                        } else {
                            theDevices[d].PLCwriteRequests[i].Values[n] = values[addr + n];
                        }
                    }

                    // wrote another one
                    retval += 1;
                }
            }
#ifdef VERBOSE_DEBUG
            {
                int z;
                fprintf(stderr, "\tqueued %u values in slot #%d\n", retval, i);
                for (z = 0; z < retval; ++z)
                    fprintf(stderr, "\t%02d 0x%08x\n", z, theDevices[d].PLCwriteRequests[i].Values[z]);
            }
#endif
            // awake the device thread
            sem_post(&newOperations[d]);
        }
    }

    return retval;
}

static void doWriteDeviceRetentives(u_int32_t d)
{
    unsigned addr, written;

    for (addr = 1;  addr <= LAST_RETENTIVE; ++addr) {
        if (CrossTable[addr].device == d && CrossTable[addr].Output) {

            //                        addr  value                     values              flags addrMax
            written = doWriteVariable(addr, the_QdataRegisters[addr], the_QdataRegisters, NULL, LAST_RETENTIVE);
            if (written > 1)
                addr += written - 1;
        }
    }
}

static inline void doWriteBytes(u_int32_t *values, u_int32_t *flags, unsigned ulOffs, unsigned uLen)
{
    unsigned addrMin = ulOffs / 4;
    unsigned addrMax = (ulOffs + uLen) / 4;
    unsigned shiftMin = ulOffs % 4 ; // 0 1 2 3
    unsigned shiftMax = (ulOffs + uLen) % 4 ; // 0 1 2 3
    unsigned addr, written, n;

    if (shiftMin > 0)
        fprintf(stderr, "%s() called with %u shiftMin instead of 0\n", __func__, shiftMin);
    if (shiftMax > 0) {
        addrMax += 1;
    }

    for (addr = addrMin; addr < addrMax && addr <= DimCrossTable; ++addr) {

        if (flags && flags[addr] == 0)
            continue;

        written = doWriteVariable(addr, values[addr], values, flags, addrMax);

        if (flags) {
            for (n = 0; n < written; ++n)
                flags[addr + n] = 0;
        }

        if (written > 1)
            addr += written - 1;
    }
}

static inline void doWriteBit(unsigned ulOffs, unsigned usBit, int bit)
{
    unsigned addr = ulOffs / 4;
    unsigned shift = ulOffs % 4 * 8; // 0 8 16 24
    u_int32_t mask;
    u_int32_t value;
    unsigned written;

    mask = 1 << (usBit - 1); // 0x01 ... 0x80
    mask = mask << shift;    // 0x00000001 ... 0x80000000

    if (bit)
        value = the_QdataRegisters[addr] |= mask;
    else
        value = the_QdataRegisters[addr] &= ~mask;

    written = doWriteVariable(addr, value, NULL, NULL, addr);

    if (written > 1)
        fprintf(stderr, "%s() wrote %u variables instead of 1", __func__, written);
}

IEC_UINT dataNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;

    if (engineStatus != enExiting) {
        if (engineStatus != enRunning) {
            fprintf(stderr, "dataNotifySet: called with no engine running\n");
            uRes = ERR_NOT_CONFIGURED;
            RETURN(uRes);
        }
        pthread_mutex_lock(&theCrosstableClientMutex);
        {
            // XX_GPIO_SET(3);
			if (pNotify->uTask != 0xffffu) {
                // notify from a plc task

                // check the write copy regions
                SImageReg   *pIRs = (SImageReg *)pIO->C.pAdr;
                SImageReg   *pIR = &pIRs[pNotify->uTask];

                if (pIR->pSetQ[uIOLayer]) {
                    // write from __%Q__ segment only if changed (using the %W write flags)
                    void *pvQsegment = pIO->Q.pAdr + pIO->Q.ulOffs;
                    void *pvWsegment = pIO->W.pAdr + pIO->W.ulOffs;
                    u_int32_t *values = (u_int32_t *)pvQsegment;
                    u_int32_t *flags = (u_int32_t *)pvWsegment;

                    doWriteBytes(values, flags, 0, (1 + DimCrossTable)*sizeof(u_int32_t)); // < sizeof(the_QdataRegisters)
                }
            } else if (pNotify->usSegment != SEG_OUTPUT) {
                uRes = ERR_WRITE_TO_INPUT;
            } else {
                // notify from others
                IEC_UDINT ulStart = vmm_max(pNotify->ulOffset, pIO->Q.ulOffs);
                IEC_UDINT ulStop = vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->Q.ulOffs + pIO->Q.ulSize);
                if (pNotify->usBit == 0) {
                    // write a byte region
                    if (ulStart < ulStop) {
                        doWriteBytes((u_int32_t *)(pIO->Q.pAdr), NULL, (ulStart - pIO->Q.ulOffs), pNotify->uLen);
                    }
                } else {
                    // write a bit in a byte
                    void * source = pIO->Q.pAdr + ulStart;
                    IEC_UINT uM = (IEC_UINT)(1u << (pNotify->usBit - 1u));
                    if ((*(IEC_DATA *)source) & uM) {
                        doWriteBit(ulStart - pIO->Q.ulOffs, pNotify->usBit, 1);
                    } else {
                        doWriteBit(ulStart - pIO->Q.ulOffs, pNotify->usBit, 0);
                    }
                }
            }
            // XX_GPIO_CLR(3);
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

    if (engineStatus != enExiting) {
        if (engineStatus != enRunning) {
            fprintf(stderr, "dataNotifyGet: called with no running engine\n");
            uRes = OK; // ERR_NOT_CONFIGURED;
            RETURN(uRes);
        }
        pthread_mutex_lock(&theCrosstableClientMutex);
        {
            // XX_GPIO_SET(3);
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
            // XX_GPIO_CLR(3);
        }
        pthread_mutex_unlock(&theCrosstableClientMutex);
    }
	RETURN(uRes);
}

#include <rtdm/rtserial.h>

static int mect_connect(unsigned devnum, unsigned baudrate, char parity, unsigned databits, unsigned stopbits, unsigned timeout_ms)
{
    int fd;
    char devname[VMM_MAX_PATH];
    struct rtser_config rt_serial_config;

    snprintf(devname, VMM_MAX_PATH, "rtser%u", devnum);
    fd = rt_dev_open(devname, 0);
    if (fd < 0) {
        return -1;
    }
    rt_serial_config.config_mask =
        RTSER_SET_BAUD
        | RTSER_SET_DATA_BITS
        | RTSER_SET_STOP_BITS
        | RTSER_SET_PARITY
        | RTSER_SET_HANDSHAKE
        | RTSER_SET_TIMEOUT_RX
        ;
    /* Set Baud rate */
    rt_serial_config.baud_rate = baudrate;

    /* Set data bits (5, 6, 7, 8 bits) */
    switch (databits) {
        case 5:  rt_serial_config.data_bits = RTSER_5_BITS; break;
        case 6:  rt_serial_config.data_bits = RTSER_6_BITS; break;
        case 7:  rt_serial_config.data_bits = RTSER_7_BITS; break;
        case 8:  rt_serial_config.data_bits = RTSER_8_BITS; break;
        default: rt_serial_config.data_bits = RTSER_8_BITS; break;
    }

    /* Stop bit (1 or 2) */
    if (stopbits == 1)
        rt_serial_config.stop_bits = RTSER_1_STOPB;
    else /* 2 */
        rt_serial_config.stop_bits = RTSER_2_STOPB;

    /* Parity bit */
    if (parity == 'N')
        rt_serial_config.parity = RTSER_NO_PARITY;		/* None */
    else if (parity == 'E')
        rt_serial_config.parity = RTSER_EVEN_PARITY;	/* Even */
    else
        rt_serial_config.parity = RTSER_ODD_PARITY;		/* Odd */

    /* Disable software flow control */
    rt_serial_config.handshake = RTSER_NO_HAND;

    /* Response timeout in ns */
    rt_serial_config.rx_timeout = timeout_ms * 1E6;

    int err = rt_dev_ioctl(fd, RTSER_RTIOC_SET_CONFIG, &rt_serial_config);
    if (err) {
        fprintf(stderr, "%s(uart%u) error: %s\n", __func__, devnum, strerror(-err));
        rt_dev_close(fd);
        fd = -1;
        return -1;
    }
    fprintf(stderr, "%s(%d=uart%u) ok\n", __func__, fd, devnum);

    return fd;
}

static int mect_bcc(char *buf, unsigned len)
{
    int i, bcc = buf[0];

    for (i = 1; i < len; ++i) {
        bcc ^= buf[i];
    }

    return bcc;
}

#ifdef VERBOSE_DEBUG
static void mect_printbuf(char *msg, char *buf, unsigned len)
{
    int i;
    fprintf(stderr, msg);
    for (i = 0; i < len; ++i) {
        fprintf(stderr, " %02x", buf[i]);
    }
    fprintf(stderr, "\n");
}
#endif

static int mect_read_ascii(int fd, unsigned node, unsigned command, float *value)
{
    char nn[2+1];
    char cc[2+1];
    char buf[13+1];
    int retval;
    char *p;

    if (fd < 0 || value == NULL) {
        return -1;
    }

    // requesting value from node "12" and obtaining "1234.678"

    // question: EOT '1' '1' '2' '2' 'R' 'O' ENQ
    snprintf(nn, 2+1, "%02u", node);
    snprintf(cc, 2+1, "%c%c", (command & 0x7F00) >> 8, (command & 0x007F));
    snprintf(buf, 8+1, "\004%c%c%c%c%s\005", nn[0], nn[0], nn[1], nn[1], cc);
    rt_dev_write(fd, buf, 8);
#ifdef VERBOSE_DEBUG
    mect_printbuf("mect_read_ascii: wrote", buf, 8);
#endif

    // answer: STX 'R' 'O' '1' '2' '3' '4' '.' '6' '7' '8' ETX BCC
    //         [0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10][11][12]
    retval = rt_dev_read(fd, buf, 13);

#ifdef VERBOSE_DEBUG
    mect_printbuf("mect_read_ascii: read", buf, 13);
#endif
    if (retval <= 0 || buf[0] != '\002' || buf[1] != cc[0] || buf[2] != cc[1] || buf[11] != '\003'
        || buf[12] != mect_bcc(&buf[1], 11)) {
#ifdef VERBOSE_DEBUG
        fprintf(stderr, "mect_read_ascii: error retval=%u 0:%02x 1:%02x 2:%02x 11:%02x 12:%02x\n",
            retval, buf[0], buf[1], buf[2], buf[11], buf[12]);
#endif
        return -1;
    }
    buf[11] = '\0';
    *value = strtof(&buf[3], &p);
    if (p == &buf[3]) {
#ifdef VERBOSE_DEBUG
        fprintf(stderr, "mect_read_ascii: error float\n");
#endif
        return -1;
    }
#ifdef VERBOSE_DEBUG
    fprintf(stderr, "mect_read_ascii: value=%f\n", *value);
#endif
    return 0;
}

static int mect_write_ascii(int fd, unsigned node, unsigned command, float value)
{
    char nn[2+1];
    char cc[2+1];
    char buf[18+1];
    int retval;

    if (fd < 0) {
        return -1;
    }

    // command: EOT '1' '1' '2' '2' STX 'I' 'U' '1' '2' '3' '.' '5' '6' '7' '8' ETX BCC
    //          [0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10][11][12][13][14][15][16][17]
    snprintf(nn, 2+1, "%02u", node);
    snprintf(cc, 2+1, "%c%c", (command & 0x7F00) >> 8, (command & 0x007F));
    snprintf(buf, 18+1, "\004%c%c%c%c\002%s%8.0f\003%c",
        nn[0], nn[0], nn[1], nn[1], cc, value, 0x00);
    buf[17] = mect_bcc(&buf[6], 11);
    rt_dev_write(fd, buf, 18);
#ifdef VERBOSE_DEBUG
    mect_printbuf("mect_write_ascii: wrote", buf, 18);
#endif

    // reply: ACK / NAK
    //        [0]
    retval = rt_dev_read(fd, buf, 1);
#ifdef VERBOSE_DEBUG
    mect_printbuf("mect_write_ascii: read", buf, 1);
#endif
    if (retval < 0 || buf[0] != '\006') {
#ifdef VERBOSE_DEBUG
        fprintf(stderr, "mect_write_ascii: error retval=%u 0:%02x\n",
            retval, buf[0]);
#endif
        return -1;
    }
    return 0;
}

static int mect_read_hexad(int fd, unsigned node, unsigned command, unsigned *value)
{
    char nn[2+1];
    char cc[2+1];
    char buf[13+1];
    int retval;
    char *p;

    if (fd < 0 || value == NULL) {
        return -1;
    }

    // requesting value from node "12" and obtaining "1234.678"

    // question: EOT '1' '1' '2' '2' 'R' 'O' ENQ
    snprintf(nn, 2+1, "%02u", node);
    snprintf(cc, 2+1, "%c%c", (command & 0x7F00) >> 8, (command & 0x007F));
    snprintf(buf, 8+1, "\004%c%c%c%c%s\005", nn[0], nn[0], nn[1], nn[1], cc);
    rt_dev_write(fd, buf, 8);
#ifdef VERBOSE_DEBUG
    mect_printbuf("mect_read_hexad: wrote", buf, 8);
#endif

    // answer: STX 'R' 'O' ' ' ' ' ' ' '>' '0' '0' 'F' 'F' ETX BCC
    //         [0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10][11][12]
    retval = rt_dev_read(fd, buf, 13);
#ifdef VERBOSE_DEBUG
    mect_printbuf("mect_write_ascii: read", buf, 13);
#endif
    if (retval < 0 || buf[0] != '\002' || buf[1] != cc[0] || buf[2] != cc[1] || buf[3] != ' ' || buf[4] != ' '
        || buf[5] != ' '  || buf[6] != '>'|| buf[11] != '\003' || buf[12] != mect_bcc(&buf[1], 11)) {
#ifdef VERBOSE_DEBUG
        fprintf(stderr, "mect_read_hexad: error retval=%u 0:%02x 1:%02x 2:%02x 11:%02x 12:%02x\n",
            retval, buf[0], buf[1], buf[2], buf[11], buf[12]);
#endif
        return -1;
    }
    buf[11] = '\0';
    *value = strtoul(&buf[7], &p, 16);
    if (p == &buf[7]) {
#ifdef VERBOSE_DEBUG
        fprintf(stderr, "mect_read_hexad: error hexadecimal\n");
#endif
        return -1;
    }
#ifdef VERBOSE_DEBUG
    fprintf(stderr, "mect_read_hexad: value=0x%04xn", *value);
#endif
    return 0;
}

static int mect_write_hexad(int fd, unsigned node, unsigned command, unsigned value)
{
    char nn[2+1];
    char cc[2+1];
    char buf[18+1];
    int retval;

    if (fd < 0) {
        return -1;
    }

    // command: EOT '1' '1' '2' '2' STX 'P' 'T' ' ' ' ' ' ' '>' '0' '0' 'F' 'F' ETX BCC
    //          [0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10][11][12][13][14][15][16][17]
    snprintf(nn, 2+1, "%02u", node);
    snprintf(cc, 2+1, "%c%c", (command & 0x7F00) >> 8, (command & 0x007F));
    snprintf(buf, 18+1, "\004%c%c%c%c\002%s   >%04x\003%c",
        nn[0], nn[0], nn[1], nn[1], cc, (value & 0xFFFF), 0x00);
    buf[17] = mect_bcc(&buf[6], 11);
    rt_dev_write(fd, buf, 18);
#ifdef VERBOSE_DEBUG
    mect_printbuf("mect_write_hexad: wrote", buf, 18);
#endif

    // reply: ACK / NAK
    //        [0]
    retval = rt_dev_read(fd, buf, 1);
#ifdef VERBOSE_DEBUG
    mect_printbuf("mect_write_hexad: read", buf, 1);
#endif
    if (retval < 0 || buf[0] != '\006') {
#ifdef VERBOSE_DEBUG
        fprintf(stderr, "mect_write_hexad: error retval=%u 0:%02x\n",
            retval, buf[0]);
#endif
        return -1;
    }
    return 0;
}

static void mect_close(int fd)
{
    if (fd >= 0) {
        rt_dev_close(fd);
    }
    fprintf(stderr, "%s(%d)\n", __func__, fd);
}

#endif /* RTS_CFG_IODAT */

/* ---------------------------------------------------------------------------- */
