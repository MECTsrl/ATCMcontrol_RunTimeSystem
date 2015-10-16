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
const uint16_t READ = 0x4000;
const uint16_t WRITE = 0x8000;
const uint16_t WRITE_SINGLE = 0x8000;
const uint16_t WRITE_MULTIPLE = 0xA000;
const uint16_t WRITE_RIC_MULTIPLE = 0xE000;
const uint16_t WRITE_RIC_SINGLE = 0xC000;
const uint16_t WRITE_PREPARE = 0x2000;
const uint16_t RTU = 0;
const uint16_t TCP = 1;
const uint16_t TCPRTU = 2;
const uint8_t STATO_OK = 0;
const uint8_t STATO_ERR = 1;
const uint8_t STATO_RUN = 2;
const uint16_t STATO_BUSY_WRITE = 1;
const uint16_t STATO_BUSY_READ = 2;
const uint16_t UINT16 = 0;
const uint16_t INT16 = 1;
const uint16_t UDINT32 = 2;
const uint16_t DINT32 = 3;
const uint16_t FLOATDCBA = 4;
const uint16_t FLOATCDAB = 5;
const uint16_t FLOATABCD = 6;
const uint16_t FLOATBADC = 7;
const uint16_t BIT = 8;
const uint16_t UDINTDCBA = 9;
const uint16_t UDINTCDAB = 10;
const uint16_t UDINTABC = 11;
const uint16_t UDINTBADC = 12;
const uint16_t DINTDCBA = 13;
const uint16_t DINTCDAB = 14;
const uint16_t DINTABCD = 15;
const uint16_t DINTBADC = 16;
const uint16_t CommError = 1;
const uint16_t TimeoutError = 16;
const int16_t WaitingCycles = 10;

const uint32_t FRONTE_SALITA = 1;
const uint32_t FRONTE_DISCESA = 0;
const char ALcrossTableFile[] = "Alarms.csv";
const char VARcrossTableFile[] = "Crosstable.csv";
const char CommParFile[] = "Commpar.csv";

#define MaxLocalQueue 15

#endif // RESOURCE1_CST_H
