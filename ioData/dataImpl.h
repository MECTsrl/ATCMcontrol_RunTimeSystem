/*
 * Copyright 2021 Mect s.r.l
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
 * Filename: dataImpl.h
 */

#ifndef DATAIMPL_H
#define DATAIMPL_H

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <ctype.h>

#include "../inc/stdInc.h"
#include "../inc.data/dataMain.h"
#include "../inc/iolDef.h"
#include "../inc.fc/fcDef.h"
#if defined(RTS_CFG_MECT_RETAIN)
#include <sys/mman.h>
#include "../inc.mect/mectRetentive.h"
#endif

#include "system_ini.h"
#include "hmi_plc.h"

#include "../vmLib/libModbus.h"
#include "../inc.data/CANopen.h"

#define REVISION_HI  2
#define REVISION_LO  26

#if defined(__linux__) && defined(__arm__)

#ifdef __XENO__
#define SERIAL_DEVNAME "rtser%u"
#else
#define SERIAL_DEVNAME "/dev/ttySP%u"
#endif

#elif defined(__linux__) && defined(__amd64__)

#define SERIAL_DEVNAME "/dev/ttyUSB%u"

#elif defined(__linux__) && defined(__i686__)

#define SERIAL_DEVNAME "/dev/ttyUSB%u"

#else
#error unknown platform

#endif

#ifdef __XENO__
#include <native/timer.h>
#else
#define RTIME uint64_t
static inline uint64_t rt_timer_read()
{
    uint64_t retval;
    struct timespec t;

    if (clock_gettime(CLOCK_MONOTONIC, &t) == 0) {
        retval = t.tv_sec * UN_MILIARDO_ULL + t.tv_nsec;
    } else {
        retval = 0ull;
    }
    return retval;
}
#endif
#define TIMESPEC_FROM_RTIME(ts, rt) { ts.tv_sec = rt / UN_MILIARDO_ULL; ts.tv_nsec = rt % UN_MILIARDO_ULL; }

#if DEBUG
#undef VERBOSE_DEBUG
#endif

#if 0
// enabling FGPIO output (for clients 0,1,2,3)
#define XX_GPIO_SET_69(n) if (n <= 3) { XX_GPIO_SET(6 + n); }
#define XX_GPIO_CLR_69(n) if (n <= 3) { XX_GPIO_CLR(6 + n); }
#else
#define XX_GPIO_SET_69(n)
#define XX_GPIO_CLR_69(n)
#endif

#define PLC_time          5390
#define PLC_timeMin       5391
#define PLC_timeMax       5392
#define PLC_timeWin       5393
#define PLC_Version       5394
#define PLC_EngineStatus  5395
#define PLC_ResetValues   5396
#define PLC_buzzerOn      5397
#define PLC_PLC_Version   5398 // UINT;3;[RW] viewable in hmi menu > info
#define PLC_HMI_Version   5399 // UINT;3;[RW] viewable in hmi menu > info

#define PLC_5400          5400 // TPLC100_01_AA/AB CH0_NETRUN
#define PLC_5401          5401 // TPLC100_01_AA/AB CH0_NETGOOD
#define PLC_5402          5402 // TPLC100_01_AA/AB CH0_NETERR
#define PLC_5403          5403 // TPLC100_01_AA/AB CH0_NETRST
#define PLC_5404          5404 // TPLC100_01_AA/AB CH0_NETDIS
#define PLC_5405          5405 // TPLC100_01_AA/AB CH0_01_NODERUN
#define PLC_5406          5406 // TPLC100_01_AA/AB CH0_01_NODEGOOD
#define PLC_5407          5407 // TPLC100_01_AA/AB CH0_01_NODEERR
#define PLC_5408          5408 // TPLC100_01_AA/AB CH0_01_NODERST
#define PLC_5409          5409 // TPLC100_01_AA/AB CH0_01_NODEDIS

#define PLC_Year          5410 // [RO] 2017
#define PLC_Month         5411 // [RO] 1..12
#define PLC_Day           5412 // [RO] 1..31
#define PLC_Hours         5413 // [RO] 0..23
#define PLC_Minutes       5414 // [RO] 0..59
#define PLC_Seconds       5415 // [RO] 0..59
#define PLC_UPTIME_s      5416 // UDINT;0;[RO] Uptime in seconds (wraps in 136 years)
#define PLC_UPTIME_cs     5417 // UDINT;0;[RO] Uptime in centiseconds = 10 ms (wraps in 497 days)
#define PLC_WATCHDOGEN    5418 // BIT;;[RW] Enable Watchdog
#define PLC_WATCHDOG_ms   5419 // UDINT;0;[RW] Reset Watchdog Timer

#define PLC_PRODUCT_ID    5420 // UDINT;0;[RO] 0x100803AC <--> TPAC1008_03_AC
#define PLC_SERIAL_NUMBER 5421 // UDINT;0;[RO] 2019014321 <--> 2019014321
#define PLC_HMI_PAGE      5422 // DINT;0;[RW] 0x100 <--> page100; -1 <--> menu; ...

#define PLC_5430          5430

#define PLC_BEEP_VOLUME   5435 // BYTE;0[RW] when buzzerOn
#define PLC_TOUCH_VOLUME  5436 // BYTE;0[RW] when QEvent::MouseButtonPress
#define PLC_ALARM_VOLUME  5437 // BYTE;0[RW] when alarm
#define PLC_BUZZER        5438 // UDINT;0[RW] 0x44332211 up=0x11[%] on=0x22[cs] off=0x33[cs] rep=0x44[times]
#define PLC_FastIO_Ena    5439 // UDINT;0[RW] TPAC1008_03_AX=0x000000FF TPAC1005=0x0003FF01
#define PLC_FastIO_Dir    5440 // UDINT;0[RW] TPAC1008_03_AX=0x0000000F TPAC1005=0x00020000

#define PLC_FastIO_1      5441 // BIT;;[RW] GPIO 2,14 PIN  21 SSP1_DATA0  TPAC1005=T2 TPAC1008_03_AX=FastOUT_1 TPX10xx_03_x=FastIN_1
#define PLC_FastIO_2      5442 // BIT;;[RW] GPIO 0,17 PIN 131 GPMI_CE1N               TPAC1008_03_AX=FastOUT_2 TPX10xx_03_x=FastIN_2
#define PLC_FastIO_3      5443 // BIT;;[RW] GPIO 2,12 PIN  11 SSP1_SCK                TPAC1008_03_AX=FastOUT_3 TPX10xx_03_x=FastIN_3
#define PLC_FastIO_4      5444 // BIT;;[RW] GPIO 3,06 PIN  78 AUART1_CTS              TPAC1008_03_AX=FastOUT_4 TPX10xx_03_x=FastIN_4
#define PLC_FastIO_5      5445 // BIT;;[RW] GPIO 2,20 PIN   7 SSP2_SS1                TPAC1008_03_AX=FastIN_1  TPX10xx_03_x=FastOUT_1
#define PLC_FastIO_6      5446 // BIT;;[RW] GPIO 3,02 PIN  70 AUART0_CTS              TPAC1008_03_AX=FastIN_2  TPX10xx_03_x=FastOUT_2
#define PLC_FastIO_7      5447 // BIT;;[RW] GPIO 3,04 PIN  81 AUART1_RX               TPAC1008_03_AX=FastIN_3  TPX10xx_03_x=FastOUT_3
#define PLC_FastIO_8      5448 // BIT;;[RW] GPIO 3,05 PIN  65 AUART1_TX               TPAC1008_03_AX=FastIN_4  TPX10xx_03_x=FastOUT_4

#define PLC_FastIO_9      5449 // BIT;;[RW] GPIO 2,24 PIN 286 SSP3_SCK    TPAC1005=T1 TP*=PFO
#define PLC_FastIO_10     5450 // BIT;;[RW] GPIO 2,27 PIN  15 SSP3_SS0    TPAC1005=T3
#define PLC_FastIO_11     5451 // BIT;;[RW] GPIO 2,17 PIN   1 SSP2_MOSI   TPAC1005=T4 TP*=RTC:SSP2_MOSI
#define PLC_FastIO_12     5452 // BIT;;[RW] GPIO 2,18 PIN 288 SSP2_MISO   TPAC1005=T5 TP*=RTC:SSP2_MISO
#define PLC_FastIO_13     5453 // BIT;;[RW] GPIO 2,16 PIN 280 SSP2_SCK    TPAC1005=T6 TP*=RTC:SSP2_SCK
#define PLC_FastIO_14     5454 // BIT;;[RW] GPIO 2,19 PIN   4 SSP2_SS0    TPAC1005=T7 TP*=RTC:SSP2_S0
#define PLC_FastIO_15     5455 // BIT;;[RW] GPIO 2,21 PIN  18 SSP2_SS2    TPAC1005=T8 TP*=CS
#define PLC_FastIO_16     5456 // BIT;;[RW] GPIO 2,25 PIN   9 SSP3_MOSI   TPAC1005=T9 TPAC1008*=RESET_WIFI

#define PLC_FastIO_17     5457 // BIT;;[RW] GPIO 2,26 PIN   3 SSP3_MISO   TPAC1005=T10
#define PLC_FastIO_18     5458 // BIT;;[RW] GPIO 2, 9 PIN 275 SSP0_DETECT TPAC1005=GPIO_A
#define PLC_FastIO_19     5459 // BIT;;[RW] GPIO 4,20 PIN 230 JTAG_RTCK   TPAC1005=GPIO_B
#define PLC_FastIO_20     5460
#define PLC_FastIO_21     5461
#define PLC_FastIO_22     5462
#define PLC_FastIO_23     5463
#define PLC_FastIO_24     5464

#define PLC_FastIO_25     5465
#define PLC_FastIO_26     5466
#define PLC_FastIO_27     5467
#define PLC_FastIO_28     5468
#define PLC_FastIO_29     5469
#define PLC_FastIO_30     5470
#define PLC_FastIO_31     5471
#define PLC_FastIO_32     5472

// -------------------------------------------------------------------------------------------

#define CROSSTABLE_CSV "/local/etc/sysconfig/Crosstable.csv"
#define HMI_INI        "/local/root/hmi.ini"
#define ROOTFS_VERSION "/rootfs_version"
#define SERIAL_CONF    "/etc/serial.conf"

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

// --------
#define DimCrossTable   5472
#define DimAlarmsCT     1152
#define LAST_RETENTIVE  192

extern struct system_ini system_ini;

// -------- ALL SERVERS (RTU_SRV, TCP_SRV, TCPRTU_SRV) ------------
#define REG_SRV_NUMBER      4096
#define THE_SRV_SIZE        (REG_SRV_NUMBER * sizeof(u_int16_t)) // 0x00002000 8kB
#define	THE_SRV_MAX_CLIENTS	10

//#define RTS_CFG_DEBUG_OUTPUT
enum TableType {Crosstable_csv = 0, Alarms_csv};
enum FieldbusType {PLC = 0, RTU, TCP, TCPRTU, CANOPEN, MECT, RTU_SRV, TCP_SRV, TCPRTU_SRV};
enum UpdateType { Htype = 0, Ptype, Stype, Ftype, Vtype, Xtype};
enum EventAlarm { Event = 0, Alarm};

enum threadStatus {NOT_STARTED = 0, RUNNING, EXITING};
enum ServerStatus {SRV_RUNNING0 = 0, SRV_RUNNING1, SRV_RUNNING2, SRV_RUNNING3, SRV_RUNNING4};
enum DeviceStatus {ZERO = 0, NOT_CONNECTED, CONNECTED, CONNECTED_WITH_ERRORS, DEVICE_BLACKLIST, NO_HOPE};
enum NodeStatus   {NO_NODE = 0, NODE_OK, TIMEOUT, BLACKLIST, DISCONNECTED, NODE_DISABLED};

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
};
extern struct ServerStruct theServers[MAX_SERVERS];
extern u_int16_t theServersNumber;

#define MaxLocalQueue 64
struct PLCwriteRequest {
    u_int16_t Addr;
    u_int16_t Number;
    u_int32_t Values[MAX_VALUES];
};

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
    struct PLCwriteRequest PLCwriteRequests[MaxLocalQueue];
    u_int16_t PLCwriteRequestNumber;
    u_int16_t PLCwriteRequestGet;
    u_int16_t PLCwriteRequestPut;
    u_int16_t diagnosticAddr;
};
extern struct ClientStruct theDevices[MAX_DEVICES];
extern u_int16_t theDevicesNumber;
extern u_int16_t theTcpDevicesNumber;

struct NodeStruct {
    u_int16_t device;
    u_int16_t NodeID;
    //
    enum NodeStatus status;
    int16_t retries;
    int16_t blacklist;
    u_int16_t diagnosticAddr;
};
extern struct NodeStruct theNodes[MAX_NODES];
extern u_int16_t theNodesNumber;

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
    u_int16_t BlockSize;
    int Output;
    int16_t Counter;
    int32_t OldVal;
    int usedInAlarmsEvents;
    //
    u_int16_t device;
    u_int16_t node;
} CrossTable[1 + DimCrossTable];	 // campi sono riempiti a partire dall'indice 1

struct Alarms {
    enum EventAlarm ALType;
    u_int16_t TagAddr;
    char ALSource[MAX_IDNAME_LEN];
    char ALCompareVar[MAX_IDNAME_LEN];
    u_int16_t SourceAddr;
    u_int16_t CompareAddr;
    varUnion  ALCompareVal;
    u_int16_t ALOperator;
    u_int16_t ALFilterTime;
    u_int16_t ALFilterCount;
    u_int16_t ALError;
    int comparison;
} ALCrossTable[1 + DimAlarmsCT]; // campi sono riempiti a partire dall'indice 1

/* ----  Global Variables:	 -------------------------------------------------- */

extern int verbose_print_enabled;
extern int timer_overflow_enabled;

#if defined(RTS_CFG_MECT_RETAIN)
extern u_int32_t *retentive;
extern int do_flush_retentives;
#endif

extern unsigned buzzer_beep_ms;
extern unsigned buzzer_on_cs;
extern unsigned buzzer_off_cs;
extern unsigned buzzer_replies;

extern unsigned buzzer_period_tics;
extern unsigned buzzer_tic;
extern unsigned buzzer_periods;

/* ------- dataEngine.c -------- */

extern enum EngineStatus engineStatus;
extern pthread_mutex_t theCrosstableClientMutex;
extern pthread_cond_t theAlarmsEventsCondvar;
extern sem_t newOperations[MAX_DEVICES];

void engineInit();
void *engineThread(void *statusAdr);

/* ------- dataClients.c -------- */

void *clientThread(void *statusAdr);

/* ------- dataServers.c -------- */

void *serverThread(void *statusAdr);

/* ------- dataHmiPlc.c -------- */

extern HmiPlcBlock plcBlock;
extern HmiPlcBlock hmiBlock;
#define THE_DATA_SIZE sizeof(HmiPlcBlock)

#define VAR_VALUE(n) plcBlock.values[n].u32
#define VAR_STATE(n) plcBlock.states[n]

#define DATA_OK      varStatus_DATA_OK
#define DATA_WARNING varStatus_DATA_WARNING
#define DATA_ERROR   varStatus_DATA_ERROR

void *datasyncThread(void *statusAdr);

/* ------- dataFbMect.c -------- */

int mect_connect(unsigned devnum, unsigned baudrate, char parity, unsigned databits, unsigned stopbits, unsigned timeout_ms);
int mect_read_ascii(int fd, unsigned node, unsigned command, float *value);
int mect_write_ascii(int fd, unsigned node, unsigned command, float value);
int mect_read_hexad(int fd, unsigned node, unsigned command, unsigned *value);
int mect_write_hexad(int fd, unsigned node, unsigned command, unsigned value);
void mect_close(int fd);

/* ------- dataUtils.c -------- */

char *strtok_csv(char *string, const char *separators, char **savedptr);

u_int32_t str2ipaddr(const char *str);
char *ipaddr2str(u_int32_t ipaddr, char *buffer);

void setEngineStatus(enum EngineStatus status);

unsigned doWriteVariable(unsigned addr, unsigned value, u_int32_t *values, u_int32_t *flags, unsigned addrMax);
void writeQdataRegisters(u_int16_t addr, u_int32_t value, u_int8_t status);

/* ------- dataDiagnostics.c -------- */

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

//1;P;RTU2_TYPE_PORT  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5010;10  ;[RO]
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

#define DIAGNOSTIC_DEV_NODE     0
#define DIAGNOSTIC_NODE_STATUS  1
//1;P;NODE_01_DEV_NODE;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5172;2   ;[RO]
//1;P;NODE_01_STATUS  ;UDINT    ;0   ;PLC       ;               ;    ;    ;    ;5172;2   ;[RO]

void initServerDiagnostic(u_int16_t s);
void initDeviceDiagnostic(u_int16_t d);
void setDiagnostic(u_int16_t addr, u_int16_t offset, u_int32_t value);
void incDiagnostic(u_int16_t addr, u_int16_t offset);

#endif // DATAIMPL_H
