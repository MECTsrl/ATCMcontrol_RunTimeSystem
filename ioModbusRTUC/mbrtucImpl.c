/*
 * Copyright 2011 Mect s.r.l
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
 * Filename: mbrtucImpl.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"mbrtucImpl.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include "stdInc.h"

#if defined(RTS_CFG_IOMBRTUC)

#include "mectCfgUtil.h"
#include "mbrtucMain.h"
#include "iolDef.h"

#include "libModbus.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/*
(* 00 *) TP1007_LIO_Revision         AT %IW5.00: UINT;
(* 01 *) (lettura config.hw)         AT %IW5.02: UINT;
(* 02 *) TP1007_LIO_DigDir_1_8       AT                  %QB5.04: BYTE;
(* 03 *) TP1007_LIO_AnInConf_1       AT                  %QW5.06: WORD;
(* 04 *) TP1007_LIO_AnInConf_2       AT                  %QW5.08: WORD;
(* 05 *) TP1007_LIO_AnOut_Conf       AT                  %QW5.10: WORD;
(* 06 *) TP1007_LIO_DigIn_1_8        AT %IB5.12: BYTE;
(*    *) TP1007_LIO_DigIn_9_12       AT %IB5.13: BYTE;
(* 07 *) TP1007_LIO_AnIn_1           AT %IW5.14: INT;
(* 08 *) TP1007_LIO_AnIn_2           AT %IW5.16: INT;
(* 09 *) TP1007_LIO_Tamb             AT %IW5.18: INT;
(* 10 *) TP1007_LIO_EncoderLo        AT %IW5.20: UINT;
(* 11 *) TP1007_LIO_EncoderHi        AT %IW5.22: UINT;
(* 12 *) TP1007_LIO_DigOut_1_8       AT                  %QB5.24: BYTE;
(* 13 *) TP1007_LIO_AnOut_1          AT                  %QW5.26: INT;
(* 14 *) TP1007_LIO_EncoderEnable    AT                  %QW5.28: WORD;
(* 15 *) TP1007_LIO_EncoderReset     AT                  %QW5.30: WORD;
(* 16 *)                             AT %IW5.32: WORD;
(* 17 *)                             AT %IW5.34: WORD;
(* 18 *)                             AT %IW5.36: WORD;
(* 19 *)                             AT %IW5.38: WORD;
(* 20 *) TP1007_LIO_HeartBit         AT %IW5.40: WORD;
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define REGISTERS_NUMBER	21 // max 32
#define REGISTERS_TYPE	   uint16_t
#define MIN_CHANNEL_SIZE    (REGISTERS_NUMBER * sizeof(REGISTERS_TYPE)) // 0x00000040 64 B

#define FIRST_READ_REGISTER 6
#define READ_REGISTERS_NUM  6

#define THE_DEVICE      "/dev/ttySP3"
#define THE_DATABITS	8
#define THE_PARITY		'N'
#define THE_STOPBITS	1
#define THE_NODE		20
#if 0
#define THE_BAUDRATE	38400
#define THE_PERIOD_ms	50 // 21 regs @ 38,4 kbaud = 19,8 ms
#define THE_RESPONSE_ms	15 // response timeout (21regs=14ms 1reg=4ms)
#define THE_BYTE_us     2000 // 750 // byte timeout
#define THE_SILENCE_us	1750 // silence delay between requests
#define THE_READALL_ms  (20 + THE_SILENCE_us/1000)
#define THE_WRITEONE_ms (9 + THE_SILENCE_us/1000)
#define THE_MAXWRITINGS ((THE_PERIOD_ms - THE_READALL_ms) / THE_WRITEONE_ms) // 2,63
#else
#define THE_BAUDRATE	230400 // 225000
#define THE_PERIOD_ms	10  // 6 regs @230400 kbaud = 3 ms
#define THE_RESPONSE_ms 3   // response timeout
#define THE_BYTE_us     1000 // 750 // byte timeout
#define THE_SILENCE_us	1750 // silence delay between requests
#define THE_READALL_ms  (3 + THE_SILENCE_us/1000)
#define THE_WRITEONE_ms (1 + THE_SILENCE_us/1000)
#define THE_MAXWRITINGS ((THE_PERIOD_ms - THE_READALL_ms) / THE_WRITEONE_ms) // 2
#endif


//#define RTS_CFG_DEBUG_OUTPUT

/* ----  Global Variables:	 -------------------------------------------------- */

static enum { NOT_STARTED, RUNNING, EXITING } g_bThreadStatus = NOT_STARTED;
static IEC_BOOL g_bInitialized	= FALSE;
static IEC_BOOL g_bConfigured	= FALSE;
static IEC_BOOL g_bRunning	= FALSE;
static IEC_BOOL g_bExiting	= FALSE;

static pthread_t theThread_id = -1;
static pthread_mutex_t theMutex = PTHREAD_MUTEX_INITIALIZER;
static REGISTERS_TYPE the_Iregisters[REGISTERS_NUMBER];
static REGISTERS_TYPE the_Qregisters[REGISTERS_NUMBER];
static int the_Wregisters[REGISTERS_NUMBER]; // write flags
static modbus_t *theContext = NULL;
static IEC_BOOL g_bModbusOK = FALSE;
static sem_t theWritingSem;

/* ----  Local Functions:	--------------------------------------------------- */

void *mbrtucThread(void *dontcare)
{
	uint16_t buffer[REGISTERS_NUMBER];
    int threadInitOK = FALSE;
    g_bModbusOK = FALSE;
    struct timespec abstime;
    STaskInfoVMM *pVMM = get_pVMM();

    // thread init
    theContext = modbus_new_rtu(THE_DEVICE, THE_BAUDRATE, THE_PARITY, THE_DATABITS, THE_STOPBITS);
    if (theContext != NULL) {
        threadInitOK = TRUE;
    }

    // run
    g_bThreadStatus = RUNNING;
    XX_GPIO_SET(4);
    for (;;) {
        // next time computation
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += (THE_PERIOD_ms / 1000);
        abstime.tv_nsec += (THE_PERIOD_ms % 1000) * 1000 * 1000; // ms -> ns
        if (abstime.tv_nsec >= (1000*1000*1000)) {
            abstime.tv_sec += abstime.tv_nsec / (1000*1000*1000);
            abstime.tv_nsec = abstime.tv_nsec % (1000*1000*1000);
        }
        // action
        if (g_bRunning && threadInitOK) {
            int rc;
            if (!g_bModbusOK) {
                // first period action
                struct timeval timeout;
                timeout.tv_sec = 0;
                timeout.tv_usec = (THE_RESPONSE_ms % 1000) * 1000; // ms -> us
                modbus_set_response_timeout(theContext, &timeout);
                timeout.tv_sec = 0;
                timeout.tv_usec = (THE_BYTE_us % (1000 * 1000)); // us -> us
                modbus_set_byte_timeout(theContext, &timeout);
                if (modbus_set_slave(theContext, THE_NODE) == 0
               /*&& modbus_rtu_set_serial_mode(theContext, MODBUS_RTU_RS485) == 0*/
                 && modbus_connect(theContext) == 0) {
                    g_bModbusOK = TRUE;
                    // first read
                    rc = modbus_read_registers(theContext, 0x0000, REGISTERS_NUMBER, the_Iregisters);
                }
            }
            // normal action
            rc = modbus_read_registers(theContext, FIRST_READ_REGISTER, READ_REGISTERS_NUM, buffer);
            if (rc > 0) {
                pthread_mutex_lock(&theMutex);
                {
                    int n;
                    for (n = 0; n < READ_REGISTERS_NUM; ++n) {
                        if (the_Iregisters[FIRST_READ_REGISTER + n] != buffer[n]) {
                            OS_MEMCPY(&the_Iregisters[FIRST_READ_REGISTER], buffer, READ_REGISTERS_NUM * sizeof(REGISTERS_TYPE));
                            if (pVMM) {
                                vmmSetEvent(pVMM, EVT_RESERVED_13);
                            }
                            break;
                        }
                    }
//                    if (! memcmp(&the_Iregisters[FIRST_READ_REGISTER], buffer, READ_REGISTERS_NUM * sizeof(REGISTERS_TYPE))) {
//                        // OS_MEMCPY(the_Iregisters, buffer, REGISTERS_NUMBER * sizeof(REGISTERS_TYPE));
//                        OS_MEMCPY(&the_Iregisters[FIRST_READ_REGISTER], buffer, READ_REGISTERS_NUM * sizeof(REGISTERS_TYPE));
//                        if (pVMM) {
//                            vmmSetEvent(pVMM, EVT_RESERVED_13);
//                        }
//                    }
                }
                pthread_mutex_unlock(&theMutex);
            }
            // in the following time serve the write requests and eventually read registers
            int done_writings = 0;
            while (done_writings < THE_MAXWRITINGS) {
				XX_GPIO_CLR(4);
                rc = sem_timedwait(&theWritingSem, &abstime);
                XX_GPIO_SET(4);
                if (rc == -1 && errno == EINTR){
                    continue;
                }
                if (rc == -1 && errno == EINVAL) {
                    break;
                }
                if (rc == -1 && errno == ETIMEDOUT) {
                    break;
                }
                // do writing
                pthread_mutex_lock(&theMutex);
                {
                    int i, retval;
                    uint16_t value;
                    for (i = 0; i < REGISTERS_NUMBER && done_writings < THE_MAXWRITINGS; ++i) {
                        if (the_Wregisters[i]) {
                            value = the_Qregisters[i];
                            pthread_mutex_unlock(&theMutex);
                            {
                                struct timespec timeout, remaining;

                                retval = modbus_write_register(theContext, i, value);
                                timeout.tv_sec = 0;
                                timeout.tv_nsec = (THE_SILENCE_us % (1000 * 1000)) * 1000; // us -> ns
                                while (nanosleep(&timeout, &remaining) == -1 && errno == EINTR) {
                                    timeout = remaining;
                                }
                                ++done_writings;
                            }
                            pthread_mutex_lock(&theMutex);
                            if (retval == 1 && value == the_Qregisters[i]) {
                                the_Wregisters[i] = 0;
                            }
                         }
                    }
                }
                pthread_mutex_unlock(&theMutex);
            }
        } else if (g_bExiting) {
            // exiting
			break;	
        } else {
            // idle action
            int rc;
            XX_GPIO_CLR(4);
            do {
                rc = sem_timedwait(&theWritingSem, &abstime);
            } while (rc  == -1 && errno == EINTR);
            XX_GPIO_SET(4);
        }
	}

    // thread clean
    if (theContext != NULL) {
        pthread_mutex_lock(&theMutex);
        {
            modbus_free(theContext);
            g_bModbusOK = FALSE;
            theContext = NULL;
        }
        pthread_mutex_unlock(&theMutex);
    }
	// exit
    g_bThreadStatus = EXITING;
    return dontcare;
}

/* ----  Implementations:	--------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/**
 * mbrtucInitialize
 *
 */
IEC_UINT mbrtucInitialize(IEC_UINT uIOLayer)
{
	IEC_UINT        uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running mbrtucInitialize() ...\n");
#endif
#if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo(TASK_OFFS_IOL_MRC, osGetTaskID());
	TR_RET(uRes);
#endif
    sem_init(&theWritingSem, 0, 0);
    theThread_id = -1;
    g_bThreadStatus = NOT_STARTED;

    g_bInitialized	= TRUE;
	g_bConfigured	= FALSE;
	g_bRunning	= FALSE;
	g_bExiting	= FALSE;
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * mbrtucFinalize
 *
 */
IEC_UINT mbrtucFinalize(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT        uRes = OK;

#if defined(RTS_CFG_SYSLOAD)
	uRes = ldClearTaskInfo(TASK_OFFS_IOL_MRC);
	TR_RET(uRes);
#endif

	// io layer clean
	g_bInitialized	= FALSE;
	g_bConfigured	= FALSE;
	g_bRunning	= FALSE;
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * mbrtucNotifyConfig
 *
 */
IEC_UINT mbrtucNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running mbrtucNotifyConfig() ...\n");
#endif
    if (pIO->I.ulSize < MIN_CHANNEL_SIZE
     || pIO->Q.ulSize < MIN_CHANNEL_SIZE
     || pIO->M.ulSize < MIN_CHANNEL_SIZE) {
        uRes = ERR_INVALID_PARAM;
    }
	g_bConfigured	= TRUE;
	g_bRunning	= FALSE;
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * mbrtucNotifyStart
 *
 */
IEC_UINT mbrtucNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

    if (app_config_load(APP_CONF_MB0))
    {
        fprintf(stderr, "[%s]: Error Modbus0 module configuration file is wrong.\n", __func__);
    }

    if (app_config_load(APP_CONF_MB1))
    {
        fprintf(stderr, "[%s]: Error Modbus1 module configuration file is wrong.\n", __func__);
    }

    if ((uIOLayer == 5 && modbus0_cfg.serial_cfg.enabled) || (uIOLayer == 6 && modbus1_cfg.serial_cfg.enabled))
    {
#if defined(RTS_CFG_DEBUG_OUTPUT)
        fprintf(stderr,"running mbrtucNotifyStart() ... \n");
#endif
        memset(the_Wregisters, 0, REGISTERS_NUMBER * sizeof(int));
        // start the thread
        pthread_attr_t pattr;
        pthread_attr_init(&pattr);
#ifndef __XENO__
        if (pthread_create(&theThread_id, &pattr, &mbrtucThread, NULL) == 0) {
#else
		if (osPthreadCreate(&theThread_id, &pattr, &mbrtucThread, NULL, "mbrtuc", 0) == 0) {
#endif
            do {
                usleep(1000);
            } while (g_bThreadStatus != RUNNING);
        } else {
#if defined(RTS_CFG_IO_TRACE)
            osTrace("[%s] ERROR message_th: %s.\n", __func__, strerror(errno));
#endif
            uRes = ERR_FB_INIT;
        }

        g_bRunning = TRUE;
    } else {
        g_bRunning = FALSE;
    }

    RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * mbrtucNotifyStop
 *
 */
IEC_UINT mbrtucNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running mbrtucNotifyStop() ...\n");
#endif
	g_bRunning = FALSE;
    // stop the thread
    if (g_bThreadStatus == RUNNING) {
        g_bExiting	= TRUE;
        do {
            usleep(1000);
        } while (g_bThreadStatus != EXITING);
    }

    RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * mbrtucNotifySet
 *
 */
IEC_UINT mbrtucNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;

    if (g_bRunning) {
        XX_GPIO_SET(3);
        pthread_mutex_lock(&theMutex);
        {
            if (pNotify->uTask != 0xffffu) {
                // notify from a plc task

                // check the write copy regions
                SImageReg   *pIRs = (SImageReg *)pIO->C.pAdr;
                SImageReg   *pIR = &pIRs[pNotify->uTask];

                if (pIR->pSetQ[uIOLayer]) {
                    // copy the whole __%Q__ segment ignoring the %Q write copy regions
                    void *pvQsegment = pIO->Q.pAdr + pIO->Q.ulOffs;
                    OS_MEMCPY(the_Qregisters, pvQsegment, REGISTERS_NUMBER * sizeof(REGISTERS_TYPE));
                    if (g_bModbusOK) {
                        // write from __%Q__ segment only if changed (using the %W write flags)
                        int i;
                        void *pvWsegment = pIO->W.pAdr + pIO->W.ulOffs;
                        REGISTERS_TYPE *flags = (REGISTERS_TYPE *)pvWsegment;
                        for (i = 0; i < REGISTERS_NUMBER; ++i) {
                            if (flags[i] != 0) {
                                flags[i] = 0;
                                the_Wregisters[i] = 1;
                                sem_post(&theWritingSem);
                             }
                        }
                    }
                }
            } else if (pNotify->usSegment != SEG_OUTPUT) {
                uRes = ERR_WRITE_TO_INPUT;
            } else if (pNotify->usBit != 0) {
                uRes = ERR_INVALID_PARAM;
            } else {
                // notify from others
                IEC_UDINT ulStart = vmm_max(pNotify->ulOffset, pIO->Q.ulOffs);
                IEC_UDINT ulStop = vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->Q.ulOffs + pIO->Q.ulSize);
                if (ulStart < ulStop) {
                    // translate memory area to registers, then copy to registers
                    register int i, first, last;
                    first = (ulStart - pIO->Q.ulOffs) / sizeof(REGISTERS_TYPE);
                    last = (ulStop - ulStart) / sizeof(REGISTERS_TYPE);
                    void *pvSource = pIO->Q.pAdr + ulStart;
                    REGISTERS_TYPE * source = (REGISTERS_TYPE *)pvSource;
                    for (i = first; i < last && i < REGISTERS_NUMBER; ++i) {
                       the_Qregisters[i] = source[i];
                       the_Wregisters[i] = 1;
                       sem_post(&theWritingSem);
                    }
                }
            }
        }
        pthread_mutex_unlock(&theMutex);
        XX_GPIO_CLR(3);
    }
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * mbrtucNotifyGet
 *
 */
IEC_UINT mbrtucNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;

    if (g_bRunning) {
        XX_GPIO_SET(2);
        pthread_mutex_lock(&theMutex);
        {
            if (pNotify->uTask != 0xffffu) {
                // notify from a plc task

                // check the read copy regions
                SImageReg   *pIRs = (SImageReg *)pIO->C.pAdr;
                SImageReg   *pIR = &pIRs[pNotify->uTask];
                void * source;
                void * dest;

                if (pIR->pGetI[uIOLayer]) {
                    source = the_Iregisters;
                    dest = pIO->I.pAdr + pIO->I.ulOffs;
                    OS_MEMCPY(dest, source, REGISTERS_NUMBER * sizeof(REGISTERS_TYPE));
                } else if (pIR->pGetQ[uIOLayer]) {
                    source = the_Qregisters;
                    dest = pIO->Q.pAdr + pIO->Q.ulOffs;
                    OS_MEMCPY(dest, source, REGISTERS_NUMBER * sizeof(REGISTERS_TYPE));
                } else {
                    // nothing to do
                }
            } else if (pNotify->usBit != 0) {
                // no bit access
                uRes = ERR_INVALID_PARAM;
            } else if (pNotify->usSegment != SEG_INPUT && pNotify->usSegment != SEG_OUTPUT){
                uRes = ERR_INVALID_PARAM;
            } else {
                // notify from others
                IEC_UDINT	ulStart;
                IEC_UDINT	ulStop;
                void * source;
                void * dest;

                if (pNotify->usSegment == SEG_INPUT) {
                    ulStart	= vmm_max(pNotify->ulOffset, pIO->I.ulOffs);
                    ulStop	= vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->I.ulOffs + pIO->I.ulSize);
                    source = the_Iregisters;
                    source += ulStart - pIO->I.ulOffs;
                    dest = pIO->I.pAdr + ulStart;
               } else  { // pNotify->usSegment == SEG_OUTPUT
                    ulStart	= vmm_max(pNotify->ulOffset, pIO->Q.ulOffs);
                    ulStop	= vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->Q.ulOffs + pIO->Q.ulSize);
                    source = the_Qregisters;
                    source += ulStart - pIO->Q.ulOffs;
                    dest = pIO->Q.pAdr + ulStart;
                }
                if (ulStart < ulStop) {
                    OS_MEMCPY(dest, source, ulStop - ulStart);
                }
            }
        }
        pthread_mutex_unlock(&theMutex);
        XX_GPIO_CLR(2);
    }
	RETURN(uRes);
}

#endif /* RTS_CFG_IOMBRTUC */

/* ---------------------------------------------------------------------------- */
