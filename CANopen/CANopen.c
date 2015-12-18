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

#define MAX_CHANNELS 2
#define MAX_NODES 32

static int enabled[MAX_CHANNELS] = { 0, 0};
static pthread_t config_id[MAX_CHANNELS] = { NO_THREAD, NO_THREAD};
static pthread_t msgthr_id[MAX_CHANNELS] = { NO_THREAD, NO_THREAD};
static pthread_t stkthr_id[MAX_CHANNELS] = { NO_THREAD, NO_THREAD};
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

    while (clock_nanosleep(CLOCK_MONOTONIC, 0, &rqtp, &rmtp) == EINTR) {
        rqtp.tv_sec = rmtp.tv_sec;
        rqtp.tv_nsec = rmtp.tv_nsec;
    }
}

u_int8_t CANopenChannels()
{
    return MAX_CHANNELS;
}

void CANopenEnable(u_int8_t channel)
{
    if (channel > MAX_CHANNELS) {
        fprintf(stderr, "CANopenEnable, ERROR: wrong channel %u\n", channel);
        return;
    }
    enabled[channel] = TRUE;
}

void CANopenDisable(u_int8_t channel)
{
    if (channel > MAX_CHANNELS) {
        fprintf(stderr, "CANopenDisable, ERROR: wrong channel %u\n", channel);
        return;
    }
    enabled[channel] = FALSE;
}

void CANopenStart(u_int8_t channel)
{
    if (channel > MAX_CHANNELS) {
        fprintf(stderr, "CANopenStart, ERROR: wrong channel %u\n", channel);
        return;
    }
    if (! enabled[channel]) {
        return;
    }
    if (config_id[channel] != NO_THREAD) {
        fprintf(stderr, "CANopenStart[%u], ERROR: double config_th\n", channel);
        return;
    }
    if (msgthr_id[channel] != NO_THREAD) {
        fprintf(stderr, "CANopenStart[%u], ERROR: double msgthr_th\n", channel);
        return;
    }
    if (stkthr_id[channel] != NO_THREAD) {
        fprintf(stderr, "CANopenStart[%u], ERROR: double stkthr_th\n", channel);
        return;
    }

    // start
    /***************************************
     *           CANopen start             *
     ***************************************/
    fprintf(stderr, "CANopenStart[%u]: this is only a stub :(\n", channel);
}

void CANopenStop(u_int8_t channel)
{
    /***************************************
     *                 stub                *
     ***************************************/
}

u_int16_t CANopenGetVarIndex(u_int8_t channel, char *name)
{
    u_int16_t retval = 0;

    if (channel > MAX_CHANNELS) {
        return retval;
    }
    if (! enabled[channel]) {
        return retval;
    }
    if (name == NULL) {
        return retval;
    }
    /***************************************
     *                 stub                *
     ***************************************/
    return retval;
}

void CANopenGetChannelStatus(u_int8_t channel, struct CANopenStatus *status)
{
    if (channel > MAX_CHANNELS || ! enabled[channel] || status == NULL) {
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenGetNodeStatus(u_int8_t channel, u_int8_t node, struct CANopenStatus *status)
{
    if (channel > MAX_CHANNELS || node > MAX_NODES || ! enabled[channel] || status == NULL) {
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenResetChannel(u_int8_t channel)
{
    if (channel > MAX_CHANNELS || ! enabled[channel]) {
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenResetNode(u_int8_t channel, u_int8_t node)
{
    if (channel > MAX_CHANNELS || node > MAX_NODES || ! enabled[channel]) {
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenDisableChannel(u_int8_t channel)
{
    if (channel > MAX_CHANNELS || ! enabled[channel]) {
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenDisableNode(u_int8_t channel, u_int8_t node)
{
    if (channel > MAX_CHANNELS || node > MAX_NODES || ! enabled[channel]) {
        return;
    }
    /***************************************
     *                 stub                *
     ***************************************/
}

u_int8_t CANopenReadPDOBit(u_int8_t channel, u_int16_t address)
{
    u_int8_t value = 0;
    /***************************************
     *                 stub                *
     ***************************************/
    return value;
}

u_int8_t CANopenReadPDOByte(u_int8_t channel, u_int16_t address)
{
    u_int8_t value = 0;
    /***************************************
     *                 stub                *
     ***************************************/
    return value;
}

u_int16_t CANopenReadPDOWord(u_int8_t channel, u_int16_t address)
{
    u_int16_t value = 0;
    /***************************************
     *                 stub                *
     ***************************************/
    return value;
}

u_int32_t CANopenReadPDODword(u_int8_t channel, u_int16_t address)
{
    u_int32_t value = 0;
    /***************************************
     *                 stub                *
     ***************************************/
    return value;
}

void CANopenWritePDOBit(u_int8_t channel, u_int16_t address, u_int8_t value)
{
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenWritePDOByte(u_int8_t channel, u_int16_t address, u_int8_t value)
{
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenWritePDOWord(u_int8_t channel, u_int16_t address, u_int16_t value)
{
    /***************************************
     *                 stub                *
     ***************************************/
}

void CANopenWritePDODword(u_int8_t channel, u_int16_t address, u_int32_t value)
{
    /***************************************
     *                 stub                *
     ***************************************/
}
