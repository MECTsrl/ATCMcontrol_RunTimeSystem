#ifndef CROSSTABLE_GVL_H
#define CROSSTABLE_GVL_H

#define ARRAY_CROSSTABLE_INPUT      the_QdataRegisters      // %M -> %Q
#define ARRAY_CROSSTABLE_OUTPUT     the_IdataRegisters      // %I

#define ARRAY_STATES(i) (((uint8_t *)(the_QdataRegisters))[22000 + i])

#define ARRAY_QUEUE                 the_IsyncRegisters   // Array delle CODE in lettura
#define ARRAY_QUEUE_OUTPUT          the_QsyncRegisters   // Array delle CODE in scrittura
                                                            // 	BIT 15 WR EN,
                                                            // BIT 14 RD EN
                                                            // BIT	13..0	ADDRESS CROSSTABLE
#define RichiestaScrittura          the_IsyncRegisters[5500]//interrupt di scrittura

#define PLCRevision01               the_QsyncRegisters[5501]
#define PLCRevision02               the_QsyncRegisters[5502]
#define Reset_RTU                   the_IsyncRegisters[5503]
#define Reset_TCP                   the_IsyncRegisters[5504]
#define Reset_TCPRTU                the_IsyncRegisters[5505]

#define CounterRTU(i)               the_QsyncRegisters[5506 + i]
#define CounterTCP(i)               the_QsyncRegisters[5571 + i]
#define CounterTCPRTU(i)            the_QsyncRegisters[5636 + i]

#define RTUBlackList_ERROR_WORD     the_QsyncRegisters[5701]
#define RTUComm_ERROR_WORD          the_QsyncRegisters[5702]
#define TCPBlackList_ERROR_WORD     the_QsyncRegisters[5703]
#define TCPComm_ERROR_WORD          the_QsyncRegisters[5704]
#define TCPRTUBlackList_ERROR_WORD  the_QsyncRegisters[5705]
#define TCPRTUComm_ERROR_WORD       the_QsyncRegisters[5706]
#define ERROR_FLAG                  the_QsyncRegisters[5707]

// sgnala se almeno un errore e' stato evidenziato dall'ultimo reset:
// bit0: errore RTU,
// bit1: errore TCP,
// bit2: errore TCPRTU,
// bit3: errore commpar,
// bit4: errore CTallarmi,
// bit5: errore CTvariabili o  errore tipo non riconosciuto su CTvariabili
// bit6: segnalazione che PLC ha terminalo l'eloaborazine delle CT
// bit7: RTU_ON
// bit8: TCP_ON
// bit9: TCPRTU_ON

#define BlackListRTUL      (*(uint32_t *)&the_QsyncRegisters[5708])
#define BlackListRTUH      (*(uint32_t *)&the_QsyncRegisters[5710])
#define CommErrRTUL        (*(uint32_t *)&the_QsyncRegisters[5712])
#define CommErrRTUH        (*(uint32_t *)&the_QsyncRegisters[5714])
#define BlackListTCPL      (*(uint32_t *)&the_QsyncRegisters[5716])
#define BlackListTCPH      (*(uint32_t *)&the_QsyncRegisters[5718])
#define CommErrTCPL        (*(uint32_t *)&the_QsyncRegisters[5720])
#define CommErrTCPH        (*(uint32_t *)&the_QsyncRegisters[5722])
#define BlackListTCPRTUL   (*(uint32_t *)&the_QsyncRegisters[5724])
#define BlackListTCPRTUH   (*(uint32_t *)&the_QsyncRegisters[5726])
#define CommErrTCPRTUL     (*(uint32_t *)&the_QsyncRegisters[5728])
#define CommErrTCPRTUH     (*(uint32_t *)&the_QsyncRegisters[5730])

// bit 0: BL rtu,
// bit 1: communication error rtu
// bit 2: BL tcp,
// bit 3: communication error tcp
// bit 4: BL tcprtu,
// bit 5: communication error tcprtu


#define HardwareType		the_QdataRegisters[5393]
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

static uint32_t ErrorsState;
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

struct DevicesStruct {
    uint16_t Protocollo;
    uint16_t BlackListed;
    int16_t RetryCounter;
    int16_t JumpRead;
};
static struct DevicesStruct RTUListDevices[1 + 254];
static struct DevicesStruct TCPListDevices[1 + 254];
static struct DevicesStruct TCPRTUListDevices[1 + 254];

struct  CrossTableRecord {
    int16_t Enable;
    int Plc;
    char Tag[VMM_MAX_IEC_STRLEN];
    uint16_t Types;
    uint16_t Decimal;
    uint16_t Protocol;	// RTU = 0, TCP=1, TCPRTU=2
    char IPAddress[VMM_MAX_IEC_STRLEN];
    uint16_t Port;
    uint8_t NodeId;
    uint16_t Address;
    uint16_t Block;
    int16_t NReg;
    uint16_t Handle;
    int16_t Counter;
    uint32_t OldVal;
    uint32_t PLCWriteVal;
    uint16_t Error;
};
static struct CrossTableRecord CrossTable[0 + DimCrossTable];	 // campi sono riempiti a partire dall'indice 1

struct  Alarms {
    int ALType;
    char ALTag[VMM_MAX_IEC_STRLEN];
    char ALSource[VMM_MAX_IEC_STRLEN];
    char ALCompareVar[VMM_MAX_IEC_STRLEN];
    uint32_t ALCompareVal;
    uint16_t ALOperator;
    uint16_t ALFilterTime;
    uint16_t ALFilterCount;
    uint16_t ALError;
};
static struct Alarms ALCrossTable[0 + DimAlarmsCT]; // campi sono riempiti a partire dall'indice 1
static int CrossTableState;
static int ALCrossTableState;

// variabili di passaggio tra TASK

static int16_t RTUCurrID;
static uint32_t RTUCurrVal[0 + MaxLocalQueue];
static int16_t RTURequestPLCWrite;
static int16_t RTUCurrNumber;

static uint16_t LocalRTUQueue[0 + MaxLocalQueue];
static int16_t IndexRTUQueuePut;
static int16_t IndexRTUQueueGet;

static int16_t TCPCurrID;
static uint32_t TCPCurrVal[0 + MaxLocalQueue];
static int16_t TCPRequestPLCWrite;
static int16_t TCPCurrNumber;
static uint16_t LocalTCPQueue[0 + MaxLocalQueue];
static int16_t IndexTCPQueuePut;
static int16_t IndexTCPQueueGet;

static int16_t TCPRTUCurrID;
static uint32_t TCPRTUCurrVal[0 + MaxLocalQueue];
static int16_t TCPRTURequestPLCWrite;
static int16_t TCPRTUCurrNumber;
static uint16_t LocalTCPRTUQueue[0 + MaxLocalQueue];
static int16_t IndexTCPRTUQueuePut;
static int16_t IndexTCPRTUQueueGet;

static int RTUProtocol_ON;
static int TCPProtocol_ON;
static int TCPRTUProtocol_ON;
//static int RTU_RUN;
//static int TCP_RUN;
//static int TCPRTU_RUN;
static int CommEnabled;


//  Parametri di comunicazione
struct CommParameters {
    char Device[VMM_MAX_IEC_STRLEN];
    char IPaddr[VMM_MAX_IEC_STRLEN];
    uint16_t Port;
    uint16_t BaudRate; // FIXME (limit 65535)
    char Parity[VMM_MAX_IEC_STRLEN];
    uint16_t DataBit;
    uint16_t StopBit;
    int16_t Tmin;
    uint16_t TimeOut;
    int State;
    uint16_t Retry;
};
static struct  CommParameters CommParameters[1 + 4];
static int16_t NumberOfFails;
static uint8_t FailDivisor;
static int32_t Talta;
static int32_t Tmedia;
static int32_t Tbassa;
static int32_t RTUTaskCycle = 30;
static int32_t TCPTaskCycle = 30;
static int32_t TCPRTUTaskCycle = 30;

//	Variabili di debug	*)
// static uint16_t maxQueue = 30; // massima lunhezza di coda raggiunta
static uint16_t HW119_ERR[0 + 10];
static uint16_t MODBUS_ERR[0 + 50];

#endif // CROSSTABLE_GVL_H
