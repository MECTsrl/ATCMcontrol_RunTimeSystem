#ifndef CANOPEN_H
#define CANOPEN_H

#include <sys/types.h>

struct CANopenStatus {
    u_int8_t running;
    u_int8_t good;
    u_int32_t error;
};

u_int8_t CANopenChannels();

void CANopenStart(u_int8_t channel);
int CANopenConfigured(u_int8_t channel);
void CANopenStop(u_int8_t channel);

u_int16_t CANopenGetVarIndex(u_int8_t channel, char *name);
void CANopenGetChannelStatus(u_int8_t channel, struct CANopenStatus *status);
void CANopenGetNodeStatus(u_int8_t channel, u_int8_t node, struct CANopenStatus *status);
void CANopenResetChannel(u_int8_t channel);
void CANopenResetNode(u_int8_t channel, u_int8_t node);
void CANopenDisableChannel(u_int8_t channel);
void CANopenDisableNode(u_int8_t channel, u_int8_t node);

int CANopenReadPDOBit(u_int8_t channel, u_int16_t address, u_int8_t *pvalue);
int CANopenReadPDOByte(u_int8_t channel, u_int16_t address, u_int8_t *pvalue);
int CANopenReadPDOWord(u_int8_t channel, u_int16_t address, u_int16_t *pvalue);
int CANopenReadPDODword(u_int8_t channel, u_int16_t address, u_int32_t *pvalue);

int CANopenWritePDOBit(u_int8_t channel, u_int16_t address, u_int8_t value);
int CANopenWritePDOByte(u_int8_t channel, u_int16_t address, u_int8_t value);
int CANopenWritePDOWord(u_int8_t channel, u_int16_t address, u_int16_t value);
int CANopenWritePDODword(u_int8_t channel, u_int16_t address, u_int32_t value);

#endif // CANOPEN_H

