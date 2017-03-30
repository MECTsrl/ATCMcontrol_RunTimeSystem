/*
 * Filename: CANopen.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"CANopen.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "fcDef.h"
#include "mectMain.h"

#include <sys/mman.h>
#include <fcntl.h>

#include <time.h>
#include <signal.h>
#include <assert.h>
#include "mectCfgUtil.h"


#include <pthread.h>
#include "CANopen.h"

#define NO_THREAD 0xFFFFFFFFL
#define STACK_RATE_MS   10
#define MESSAGE_RATE_MS 1

#define CAN_CHANNELS 2
#define MAX_NODES 32

struct statusID {
    u_int16_t runningID;
    u_int16_t goodID;
    u_int16_t errorID;
    u_int16_t resetID;
    u_int16_t disableID;
};

static inline void do_sleep_ms(unsigned delay_ms)
{
    struct timespec rqtp, rmtp;
    ldiv_t q;

    q = ldiv(delay_ms, 1000);
    rqtp.tv_sec = q.quot;
    rqtp.tv_nsec = q.rem * 1E6; // ms -> ns

    while (clock_nanosleep(CLOCK_REALTIME, 0, &rqtp, &rmtp) == EINTR) {
        rqtp.tv_sec = rmtp.tv_sec;
        rqtp.tv_nsec = rmtp.tv_nsec;
    }
}

u_int8_t CANopenChannels()
{
    return CAN_CHANNELS;
}

void CANopenStart(u_int8_t channel)
{
    if (channel > CAN_CHANNELS) {
        fprintf(stderr, "CANopenStart, ERROR: wrong channel %u\n", channel);
        return;
    }
    // start
    /***************************************
     *           CANopen start             *
     ***************************************/
}

void CANopenList(u_int8_t channel)
{
    if (channel >= CAN_CHANNELS) {
        fprintf(stderr, "CANopenStart, ERROR: wrong channel %u\n", channel);
        return;
    }
    // list
    /***************************************
     *           CANopen list              *
     ***************************************/
}

int CANopenConfigured(u_int8_t channel)
{
    if (channel > CAN_CHANNELS) {
        fprintf(stderr, "CANopenConfigured, ERROR: wrong channel %u\n", channel);
        return FALSE;
    }
    /***************************************
     *                 stub                *
     ***************************************/
    return FALSE;
}

void CANopenStop(u_int8_t channel)
{
    if (channel > CAN_CHANNELS) {
        fprintf(stderr, "CANopenStart, ERROR: wrong channel %u\n", channel);
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

u_int16_t CANopenGetVarIndex(u_int8_t channel, char *name)
{
    u_int16_t retval = 0;

    if (channel > CAN_CHANNELS || name == NULL) {
        return retval;
    }
    /***************************************
     *                 stub                *
     ***************************************/
    return retval;
}

void CANopenGetChannelStatus(u_int8_t channel, struct CANopenStatus *status)
{
    if (channel > CAN_CHANNELS || status == NULL) {
        if (status) {
            status->running = FALSE;
            status->good = FALSE;
            status->error = 0xffffffff;
        }
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenGetNodeStatus(u_int8_t channel, u_int8_t node, struct CANopenStatus *status)
{
    if (channel > CAN_CHANNELS || node > MAX_NODES || status == NULL) {
        if (status) {
            status->running = FALSE;
            status->good = FALSE;
            status->error = 0xffffffff;
        }
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenResetChannel(u_int8_t channel)
{
    if (channel > CAN_CHANNELS) {
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenResetNode(u_int8_t channel, u_int8_t node)
{
    if (channel > CAN_CHANNELS || node > MAX_NODES) {
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenDisableChannel(u_int8_t channel)
{
    if (channel > CAN_CHANNELS) {
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenDisableNode(u_int8_t channel, u_int8_t node)
{
    if (channel > CAN_CHANNELS || node > MAX_NODES) {
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

int CANopenReadPDOBit(u_int8_t channel, u_int16_t address, u_int8_t *pvalue)
{
    /***************************************
     *                 stub                *
     ***************************************/
    return 0;
}

int CANopenReadPDOByte(u_int8_t channel, u_int16_t address, u_int8_t *pvalue)
{
    /***************************************
     *                 stub                *
     ***************************************/
    return 0;
}

int CANopenReadPDOWord(u_int8_t channel, u_int16_t address, u_int16_t *pvalue)
{
    /***************************************
     *                 stub                *
     ***************************************/
    return 0;
}

int CANopenReadPDODword(u_int8_t channel, u_int16_t address, u_int32_t *pvalue)
{
    /***************************************
     *                 stub                *
     ***************************************/
    return 0;
}

int CANopenWritePDOBit(u_int8_t channel, u_int16_t address, u_int8_t value)
{
    /***************************************
     *                 stub                *
     ***************************************/
	return 0;
}

int CANopenWritePDOByte(u_int8_t channel, u_int16_t address, u_int8_t value)
{
    /***************************************
     *                 stub                *
     ***************************************/
	return 0;
}

int CANopenWritePDOWord(u_int8_t channel, u_int16_t address, u_int16_t value)
{
    /***************************************
     *                 stub                *
     ***************************************/
	return 0;
}

int CANopenWritePDODword(u_int8_t channel, u_int16_t address, u_int32_t value)
{
    /***************************************
     *                 stub                *
     ***************************************/
	return 0;
}
