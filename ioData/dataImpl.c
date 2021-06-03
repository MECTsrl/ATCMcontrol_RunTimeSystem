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
 * Filename: dataImpl.c
 */

#define __4CFILE__	"dataImpl.c"

#include "../inc/stdInc.h"

#if defined(RTS_CFG_IODAT)

#include "dataImpl.h"

/* ----  Local Functions:	--------------------------------------------------- */

static inline void doWriteBytes(u_int32_t *values, u_int32_t *flags, unsigned ulOffs, unsigned uLen);
static inline void doWriteBit(unsigned ulOffs, unsigned usBit, int bit);

/* ----  Global Variables:	 -------------------------------------------------- */

int verbose_print_enabled = 0;
int timer_overflow_enabled = 0;

struct system_ini system_ini;

static pthread_t theEngineThread_id = WRONG_THREAD;
static enum threadStatus theEngineThreadStatus = NOT_STARTED;

/* ----  Implementations:	--------------------------------------------------- */

void dataGetVersionInfo(char *szVersion)
{
    if (szVersion) {
        sprintf(szVersion, "v%d.%03d GPL", REVISION_HI, REVISION_LO);
    }
}

void dataEnableVerbosePrint(void)
{
   verbose_print_enabled = 1;
}

void dataEnableTimerOverflow(void)
{
   timer_overflow_enabled = 1;
   clock_gettime_overflow_enable();
}

/* ---------------------------------------------------------------------------- */

void dataEngineStart(void)
{
    // start the engine thread
    engineInit();
    if (osPthreadCreate(&theEngineThread_id, NULL, &engineThread, &theEngineThreadStatus, "engine", 0) == 0) {
        do {
            osSleep(THE_CONFIG_DELAY_ms); // not sched_yield();
        } while (theEngineThreadStatus != RUNNING);
    }
}

void dataEngineStop(void)
{
    void *retval;

    if (engineStatus == enIdle) {
        // SIGINT arrived before initialization
        return;
    }
    setEngineStatus(enExiting);
    if (theEngineThread_id != WRONG_THREAD) {
        pthread_join(theEngineThread_id, &retval);
        theEngineThread_id = WRONG_THREAD;
        fprintf(stderr, "joined engine\n");
    }
    dumpRetentives();
}

void dataEnginePwrFailStop(void)
{
    // in case o power failure we have no time for waiting the threads,
    // so we only block the variables writes
    // NB: there is no unlock, it's correct

    pthread_mutex_lock(&theCrosstableClientMutex);

    dumpRetentives();
}

/* ---------------------------------------------------------------------------- */
/**
 * dataInitialize
 *
 */
IEC_UINT dataInitialize(IEC_UINT uIOLayer)
{
	IEC_UINT        uRes = OK;
    (void)uIOLayer;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running dataInitialize() ...\n");
#endif
#if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo(TASK_OFFS_IOL_DAT, osGetTaskID());
	TR_RET(uRes);
#endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dataFinalize
 *
 */
IEC_UINT dataFinalize(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT        uRes = OK;
    (void)uIOLayer;
    (void)pIO;

#if defined(RTS_CFG_SYSLOAD)
	uRes = ldClearTaskInfo(TASK_OFFS_IOL_DAT);
	TR_RET(uRes);
#endif

	// io layer clean
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dataNotifyConfig
 *
 */
IEC_UINT dataNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;
    (void)uIOLayer;
    (void)pIO;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running dataNotifyConfig() ...\n");
#endif
    if (pIO->I.ulSize < THE_DATA_SIZE || pIO->Q.ulSize < THE_DATA_SIZE) {
        uRes = ERR_INVALID_PARAM;
    }
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dataNotifyStart
 *
 */
IEC_UINT dataNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;
    (void)uIOLayer;
    (void)pIO;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running dataNotifyStart() ... \n");
#endif

    // iolayer init
	void *pvIsegment = (void *)(((char *)(pIO->I.pAdr + pIO->I.ulOffs)) + 4);
    void *pvQsegment = (void *)(((char *)(pIO->Q.pAdr + pIO->Q.ulOffs)) + 4);

    OS_MEMCPY(pvIsegment, plcBlock.values, sizeof(plcBlock.values)); // always Qdata
    OS_MEMCPY(pvQsegment, plcBlock.values, sizeof(plcBlock.values));

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dataNotifyStop
 *
 */
IEC_UINT dataNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;
    (void)uIOLayer;
    (void)pIO;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running dataNotifyStop() ...\n");
#endif

    RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dataNotifySet
 *
 */

IEC_UINT dataNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;

    if (engineStatus != enExiting) {
        if (engineStatus != enRunning) {
            fprintf(stderr, "dataNotifySet: called with no engine running\n");
            uRes = ERR_NOT_CONFIGURED;
            RETURN(uRes);
        }
        pthread_mutex_lock(&theCrosstableClientMutex);
        {
            // XX_GPIO_SET(3);
			if (pNotify->uTask != 0xffffu) {
                // notify from a plc task

                // check the write copy regions
                SImageReg   *pIRs = (SImageReg *)pIO->C.pAdr;
                SImageReg   *pIR = &pIRs[pNotify->uTask];

                if (pIR->pSetQ[uIOLayer]) {
                    // write from __%Q__ segment only if changed (using the %W write flags)
                    void *pvQsegment = pIO->Q.pAdr + pIO->Q.ulOffs;
                    void *pvWsegment = pIO->W.pAdr + pIO->W.ulOffs;
                    u_int32_t *values = (u_int32_t *)pvQsegment;
                    u_int32_t *flags = (u_int32_t *)pvWsegment;

                    doWriteBytes(values, flags, 0, (1 + DimCrossTable)*sizeof(u_int32_t));
                }
            } else if (pNotify->usSegment != SEG_OUTPUT) {
                uRes = ERR_WRITE_TO_INPUT;
            } else {
                // notify from others
                IEC_UDINT ulStart = vmm_max(pNotify->ulOffset, pIO->Q.ulOffs);
                IEC_UDINT ulStop = vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->Q.ulOffs + pIO->Q.ulSize);

                if (pNotify->usBit == 0) {
                    // write a byte region
                    if (ulStart < ulStop) {
#ifdef VERBOSE_DEBUG
                        int i;
                        unsigned char *p;

                        fprintf(stderr, "calling doWriteBytes() uTask=0x%04x ulStart=%u ulOffs=%u len=%u data[]=",
                            pNotify->uTask, ulStart, pIO->Q.ulOffs, pNotify->uLen);
                        p = pIO->Q.pAdr + (ulStart - pIO->Q.ulOffs);
                        for (i = 0; i < (pNotify->uLen + 2); ++i) {
                            fprintf(stderr, " %02x", p[i]);
                        }
                        fprintf(stderr, "\n");
#endif
                        doWriteBytes((u_int32_t *)(pIO->Q.pAdr), NULL, (ulStart - pIO->Q.ulOffs), pNotify->uLen);
                    }
                } else {
                    // write a bit in a byte
                    void * source = pIO->Q.pAdr + ulStart;
                    IEC_UINT uM = (IEC_UINT)(1u << (pNotify->usBit - 1u));
                    if ((*(IEC_DATA *)source) & uM) {
                        doWriteBit(ulStart - pIO->Q.ulOffs, pNotify->usBit, 1);
                    } else {
                        doWriteBit(ulStart - pIO->Q.ulOffs, pNotify->usBit, 0);
                    }
                }
            }
            // XX_GPIO_CLR(3);
        }
        pthread_mutex_unlock(&theCrosstableClientMutex);
    }
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dataNotifyGet
 *
 */
IEC_UINT dataNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;

    if (engineStatus != enExiting) {
        if (engineStatus != enRunning) {
            fprintf(stderr, "dataNotifyGet: called with no running engine\n");
            uRes = OK; // ERR_NOT_CONFIGURED;
            RETURN(uRes);
        }
        pthread_mutex_lock(&theCrosstableClientMutex);
        {
            // XX_GPIO_SET(3);
            if (pNotify->uTask != 0xffffu) {
                // notify from a plc task

                // check the read copy regions
                SImageReg   *pIRs = (SImageReg *)pIO->C.pAdr;
                SImageReg   *pIR = &pIRs[pNotify->uTask];

                if (pIR->pGetQ[uIOLayer] == FALSE && pIR->pGetI[uIOLayer] == FALSE) {
                    // nothing to do
                    uRes = OK;
                } else {
                    // search the read copy regions for requests to this IOLayer
                    IEC_UINT	r;

                    for (r = 0; uRes == OK && r < pIR->uRegionsRd; ++r) {
                        if (pIR->pRegionRd[r].pGetQ[uIOLayer] == FALSE && pIR->pRegionRd[r].pGetI[uIOLayer] == FALSE) {
                             continue;
                        }
                        IEC_UDINT	ulStart;
                        IEC_UDINT	ulStop;
                        void * source;
                        void * dest;

                        source = plcBlock.values;
                        if (pIR->pRegionRd[r].usSegment == SEG_OUTPUT) {
                            ulStart = vmm_max(pIR->pRegionRd[r].ulOffset, pIO->Q.ulOffs);
                            ulStop	= vmm_min(pIR->pRegionRd[r].ulOffset + pIR->pRegionRd[r].uSize, pIO->Q.ulOffs + pIO->Q.ulSize);
                            source += ulStart - pIO->Q.ulOffs;
                            dest = pIO->Q.pAdr + ulStart;
                        } else { // pIR->pRegionRd[r].usSegment == SEG_INPUT
                            ulStart = vmm_max(pIR->pRegionRd[r].ulOffset, pIO->I.ulOffs);
                            ulStop	= vmm_min(pIR->pRegionRd[r].ulOffset + pIR->pRegionRd[r].uSize, pIO->I.ulOffs + pIO->I.ulSize);
                            source += ulStart - pIO->I.ulOffs;
                            dest = pIO->I.pAdr + ulStart;
                        }
                        if (ulStart < ulStop) {
                            OS_MEMCPY(dest, source, ulStop - ulStart);
                        }
                    }
                }
            } else if (pNotify->usSegment != SEG_INPUT && pNotify->usSegment != SEG_OUTPUT){
                uRes = ERR_INVALID_PARAM;
            } else {
                // notify from others
                IEC_UDINT	ulStart;
                IEC_UDINT	ulStop;
                void * source;
                void * dest;

                source = plcBlock.values;
                if (pNotify->usSegment == SEG_INPUT) {
                    ulStart	= vmm_max(pNotify->ulOffset, pIO->I.ulOffs);
                    ulStop	= vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->I.ulOffs + pIO->I.ulSize);
                    source += ulStart - pIO->I.ulOffs;
                    dest = pIO->I.pAdr + ulStart;
                } else { // pNotify->usSegment == SEG_OUTPUT
                    ulStart	= vmm_max(pNotify->ulOffset, pIO->Q.ulOffs);
                    ulStop	= vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->Q.ulOffs + pIO->Q.ulSize);
                    source += ulStart - pIO->Q.ulOffs;
                    dest = pIO->Q.pAdr + ulStart;
                }
                if (pNotify->usBit == 0) {
                    if (ulStart < ulStop) {
                        OS_MEMCPY(dest, source, ulStop - ulStart);
                    }
                } else {
                    IEC_UINT uM = (IEC_UINT)(1u << (pNotify->usBit - 1u));
                    IEC_DATA byte = *(IEC_DATA *)dest & ~uM;
                    byte |= *(IEC_DATA *)source & uM;
                    *(IEC_DATA *)dest = byte;
                }
            }
            // XX_GPIO_CLR(3);
        }
        pthread_mutex_unlock(&theCrosstableClientMutex);
    }
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */

static inline void doWriteBytes(u_int32_t *values, u_int32_t *flags, unsigned ulOffs, unsigned uLen)
{
    unsigned addrMin = ulOffs / 4;
    unsigned addrMax = (ulOffs + uLen) / 4;
    unsigned shiftMin = ulOffs % 4 ; // 0 1 2 3
    unsigned shiftMax = (ulOffs + uLen) % 4 ; // 0 1 2 3
    unsigned addr, written, n;

    if (shiftMin > 0)
        fprintf(stderr, "%s() called with %u shiftMin instead of 0\n", __func__, shiftMin);
    if (shiftMax > 0) {
        addrMax += 1;
    }

    for (addr = addrMin; addr < addrMax && addr <= DimCrossTable; ++addr) {

        if (flags && flags[addr] == 0)
            continue;

        written = doWriteVariable(addr, values[addr], values, flags, addrMax);

        if (flags) {
            for (n = 0; n < written; ++n)
                flags[addr + n] = 0;
        }

        if (written > 1)
            addr += written - 1;
    }
}

static inline void doWriteBit(unsigned ulOffs, unsigned usBit, int bit)
{
    unsigned addr = ulOffs / 4;
    unsigned shift = ulOffs % 4 * 8; // 0 8 16 24
    u_int32_t mask;
    u_int32_t value;
    unsigned written;

    mask = 1 << (usBit - 1); // 0x01 ... 0x80
    mask = mask << shift;    // 0x00000001 ... 0x80000000

    if (bit)
        value = VAR_VALUE(addr) |= mask;
    else
        value = VAR_VALUE(addr) &= ~mask;

    written = doWriteVariable(addr, value, NULL, NULL, addr);

    if (written > 1)
        fprintf(stderr, "%s() wrote %u variables instead of 1", __func__, written);
}

#endif /* RTS_CFG_IODAT */

/* ---------------------------------------------------------------------------- */
