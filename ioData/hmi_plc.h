/*
 * Copyright 2021 Mect s.r.l
 *
 * HmiPlc: MectSuite udp communication for crosstable variables between hmi and plc
 *
 * This file is part of MectSuite.
 *
 * MectSuite is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * MectSuite is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * MectSuite. If not, see http://www.gnu.org/licenses/.
 *
 */

#ifndef _HMI_PLC_H
#define _HMI_PLC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>

/* --------------------------------------------------------------------------*/

#define USE_HMI_PLC

/* --------------------------------------------------------------------------*/

#define DimCrossTable 5472  // a.k.a. DB_SIZE_ELEM

enum varStatus {

    // plcBlock: PLC --> HMI
    varStatus_DATA_OK      = 0x00, // ex "DONE", after successful fieldbus operation
    varStatus_DATA_ERROR   = 0x01, // ex "ERROR", after failed fieldbus operation
    varStatus_DATA_WARNING = 0x02, // ex "BUSY", after recoverable fieldbus error
    varStatus_DATA_UNK     = 0x0F, // ex "UNK", unknown status

    // hmiBlock: HMI --> PLC
    varStatus_NOP          = 0x10, // when either priority="0" or update="H"
    varStatus_DO_READ      = 0x20, // when update>"H" or added in variableList
    varStatus_PREPARING    = 0x30, // after addWrite()
    varStatus_DO_WRITE     = 0x40  // after doWrite() or endWrite()
};

#pragma pack(push, 1)

typedef union varUnion {
    uint8_t  u8;  // BYTE  BIT        BYTE_BIT   WORD_BIT   DWORD_BIT
    uint16_t u16; // UINT  UINT_BA
    uint32_t u32; // UDINT UDINT_BADC UDINT_CDAB UDINT_DCBA
    int16_t  i16; // INT   INT_BA
    int32_t  i32; // DINT  DINT_BADC  DINT_CDAB  DINT_DCBA
    float    f;   // REAL  REAL_BADC  REAL_CDAB  REAL_DCBA
} varUnion;

typedef struct {
    ssize_t bytes;
    varUnion values[1 + DimCrossTable];
    uint8_t states[1 + DimCrossTable]; // varStatus in 8 bits
} HmiPlcBlock;

#pragma pack(pop)

void resetHmiPlcBlocks(HmiPlcBlock *hmiBlock, HmiPlcBlock *plcBlock);

/* --------------------------------------------------------------------------*/

typedef struct {
    int udpSocket;
    char *hostname;
    struct sockaddr_in plcAddress;
} HmiClient;

HmiClient *newHmiClient(const char *hostname);
int hmiClientPoll(HmiClient *client, const HmiPlcBlock *hmiBlock, HmiPlcBlock *plcBlock, unsigned timeout_ms);
void deleteHmiClient(HmiClient *client);

/* --------------------------------------------------------------------------*/

typedef struct {
    int udpSocket;
    struct sockaddr_in hmiAddress;
    socklen_t hmiAddressLen;
} PlcServer;

PlcServer *newPlcServer();
int plcServerWait(PlcServer *server, HmiPlcBlock *hmiBlock, unsigned timeout_ms);
int plcServerReply(PlcServer *server, const HmiPlcBlock *plcBlock);
void deletePlcServer(PlcServer *server);

/* --------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif
