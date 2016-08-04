#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DimCrossTable   5472
#define MAX_WRITES      128
#define MAX_READS       128

#define COMMCH_UDP_PORT 34906

#define MAGIC_CMD       0xC1A00000
#define MAGIC_REPLY     0x0000C1A0

/* Status of variables */
#define STATUS_OK       0x01
#define STATUS_ERR      0x02
#define STATUS_BUSY_R   0x04
#define STATUS_BUSY_W   0x08
#define STATUS_ACK_R    0x10
#define STATUS_ACK_W    0x20
#define STATUS_FAIL_W   0x40
#define STATUS_ENABLED  0x80

struct udp_cmd
{
    uint32_t seq;
    uint16_t write_num;
    uint16_t read_num;
    uint16_t write_addr[MAX_WRITES];
    uint32_t write_value[MAX_WRITES];
    uint16_t read_addr[MAX_READS];
    uint32_t magic;
};

struct udp_reply
{
    uint32_t seq;
    uint32_t value[DimCrossTable];
    uint8_t status[DimCrossTable];
    uint32_t magic;
};

#ifdef __cplusplus
}
#endif

#endif // PROTOCOL_H
