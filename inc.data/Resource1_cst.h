#ifndef RESOURCE1_CST_H
#define RESOURCE1_CST_H

const uint16_t rev01 = 6;
const uint16_t rev02 = 13;

#define DimCrossTable   5472
#define DimCrossTable_2 22004
#define DimCrossTable_3 44004
#define DimCrossTable_4 (DimCrossTable_3 + 2 * DimCrossTable + 2 + 2)
#define DimAlarmsCT     1152
const uint16_t QueueRWMask = 0xE000;
const uint16_t QueueAddressMask = 0x1FFF;
#define READ               0x4000
#define WRITE_SINGLE        0x8000
#define WRITE_MULTIPLE      0xA000
#define WRITE_RIC_MULTIPLE  0xE000
#define WRITE_RIC_SINGLE    0xC000
#define WRITE_PREPARE       0x2000
//const uint16_t RTU = 0;
//const uint16_t TCP = 1;
//const uint16_t TCPRTU = 2;
const uint8_t STATO_OK = 0;
const uint8_t STATO_ERR = 1;
const uint8_t STATO_RUN = 2;
const uint16_t STATO_BUSY_WRITE = 1;
const uint16_t STATO_BUSY_READ = 2;

#define UINT16	   0
#define INT16     1
//#define UDINT32   2
//#define DINT32    3
#define FLOATDCBA 4
#define FLOATCDAB 5
#define FLOATABCD 6
#define FLOATBADC 7
#define BIT       8
#define UDINTDCBA 9
#define UDINTCDAB 10
#define UDINTABCD 11
#define UDINTBADC 12
#define DINTDCBA  13
#define DINTCDAB  14
#define DINTABCD  15
#define DINTBADC  16
//const uint16_t CommError = 1;
//const uint16_t TimeoutError = 16;
const int16_t WaitingCycles = 10;

const uint32_t FRONTE_SALITA = 1;
const uint32_t FRONTE_DISCESA = 0;
const char ALcrossTableFile[] = "Alarms.csv";
const char VARcrossTableFile[] = "Crosstable.csv";
const char CommParFile[] = "Commpar.csv";

#define MaxLocalQueue 15

#endif // RESOURCE1_CST_H
