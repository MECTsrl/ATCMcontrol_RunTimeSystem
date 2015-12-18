#ifndef CANOPEN_H
#define CANOPEN_H

#include <sys/types.h>

struct CANopenStatus {
    u_int8_t running;
    u_int8_t good;
    u_int32_t error;
};

u_int8_t CANopenChannels();

void CANopenEnable(u_int8_t channel);
void CANopenDisable(u_int8_t channel);
void CANopenStart(u_int8_t channel);
void CANopenStop(u_int8_t channel);

u_int16_t CANopenGetVarIndex(u_int8_t channel, char *name);
void CANopenGetChannelStatus(u_int8_t channel, struct CANopenStatus *status);
void CANopenGetNodeStatus(u_int8_t channel, u_int8_t node, struct CANopenStatus *status);
void CANopenResetChannel(u_int8_t channel);
void CANopenResetNode(u_int8_t channel, u_int8_t node);
void CANopenDisableChannel(u_int8_t channel);
void CANopenDisableNode(u_int8_t channel, u_int8_t node);

u_int8_t CANopenReadPDOBit(u_int8_t channel, u_int16_t address);
u_int8_t CANopenReadPDOByte(u_int8_t channel, u_int16_t address);
u_int16_t CANopenReadPDOWord(u_int8_t channel, u_int16_t address);
u_int32_t CANopenReadPDODword(u_int8_t channel, u_int16_t address);

void CANopenWritePDOBit(u_int8_t channel, u_int16_t address, u_int8_t value);
void CANopenWritePDOByte(u_int8_t channel, u_int16_t address, u_int8_t value);
void CANopenWritePDOWord(u_int8_t channel, u_int16_t address, u_int16_t value);
void CANopenWritePDODword(u_int8_t channel, u_int16_t address, u_int32_t value);

#endif // CANOPEN_H

