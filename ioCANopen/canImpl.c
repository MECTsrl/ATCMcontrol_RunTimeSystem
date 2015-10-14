
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
 * Filename: canImpl.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"canImpl.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"
#include "fcDef.h"

//#include <sys/io.h> // ioperm()

#if defined(RTS_CFG_IOCANOPEN)

#include "canMain.h"
#include "iolDef.h"
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include "mectMain.h"
#include "mectCfgUtil.h"

#include "vmmDef.h"
/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define SIZEOF_BYTE 8

/* ----  Global Variables:	 -------------------------------------------------- */

static IEC_BOOL g_bInitialized	= FALSE;
static IEC_BOOL g_bConfigured	= FALSE;
static IEC_BOOL g_bRunning	= FALSE;
static IEC_BOOL g_bFirstStart	= FALSE;

//#if defined(CIFLESS_IO)
//static IOINFO ioInfo;
//#endif

static pthread_mutex_t mReadMutex;
static pthread_mutex_t mWriteMutex;
extern int CanTimer;

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */

/* ---------------------------------------------------------------------------- */

void expand_bit_from_byte_to_dword(char byte, IEC_DWORD * wordArray)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		//		printf("bit[%d] %d\n",i, (byte >> i) & 1);
		wordArray[i] = (byte >> i) & 1;
	}
	return;
}

/* ---------------------------------------------------------------------------- */
/**
 * canInitialize
 *
 */
IEC_UINT canInitialize(IEC_UINT uIOLayer)
{
	IEC_UINT uRes = OK;
	static int init = 0;

#if defined(RTS_CFG_IO_TRACE)
	osTrace("[LAYER %d - CAN] - running canInitialize.\n", uIOLayer );
#endif

#if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo(TASK_OFFS_IOL_CAN, osGetTaskID());
	TR_RET(uRes);
#endif

	// initialize mutexes
	if(!init) 
	{
		if(pthread_mutex_init(&mReadMutex, NULL) == 0)
		{
			init = 1; //if called again while initializing the second CAN instance, pthread_mutex_init returns EBUSY (16)!!!!
		}
		else
		{	
#if defined(RTS_CFG_IO_TRACE)
			osTrace("[LAYER %d - CAN] - ERROR pthread_mutex_init (mReadMutex): %s.\n", uIOLayer, strerror(errno));
#endif
			fprintf(stderr,"%s: fallito init Readmutex: uIOLayer %d init %d\n",__func__, uIOLayer, init);
			uRes = ERR_FB_INIT;
			goto exit_function;
		}
		if(pthread_mutex_init(&mWriteMutex, NULL) != 0)
		{
#if defined(RTS_CFG_IO_TRACE)
			osTrace("[LAYER %d - CAN] - ERROR pthread_mutex_init (mWriteMutex): %s.\n", uIOLayer, strerror(errno));
#endif	
			fprintf(stderr,"%s: fallito init Writemutex: uIOLayer %d\n",__func__, uIOLayer);
			uRes = ERR_FB_INIT;
			goto exit_function;
		}
	}

exit_function:
	g_bInitialized	= (IEC_BOOL)(uRes == OK);
	g_bFirstStart	= TRUE;
	g_bConfigured	= FALSE;
	g_bRunning	= FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * canFinalize
 *
 */
IEC_UINT canFinalize(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_SYSLOAD)
	uRes = ldClearTaskInfo(TASK_OFFS_IOL_CAN);
	TR_RET(uRes);
#endif

	if (pthread_mutex_destroy(&mReadMutex) != 0)
	{
#if defined(RTS_CFG_IO_TRACE)
		osTrace("[LAYER %d - CAN] - ERROR pthread_mutex_destroy (mReadMutex): %s.\n", uIOLayer, strerror(errno));
#endif
	}
	if (pthread_mutex_destroy(&mWriteMutex) != 0)
	{
#if defined(RTS_CFG_IO_TRACE)
		osTrace("[LAYER %d - CAN] - ERROR pthread_mutex_destroy (mWriteMutex): %s.\n", uIOLayer, strerror(errno));
#endif
	}

	g_bInitialized	= FALSE;
	g_bConfigured	= FALSE;
	g_bRunning		= FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * canNotifyConfig
 *
 */
IEC_UINT canNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_IO_TRACE)
	osTrace("[LAYER %d - CAN] - running canNotifyConfig.\n", uIOLayer);
#endif
	g_bConfigured	= (IEC_BOOL)(uRes == OK);
	g_bRunning		= FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * canNotifyStart
 *
 */
IEC_UINT canNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

	g_bRunning = (IEC_BOOL)(uRes == OK);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * canNotifyStop
 *
 */
IEC_UINT canNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

	g_bRunning = FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * canNotifySet
 *
 */
IEC_UINT canNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;


	if (FB_STATE_OPERATING != FB_STATE_OPERATING)
	{
		/* PB is not operating
		 */
		RETURN(ERR_FB_NOT_OPERATING);
	}

	if(CanDone == 0)
	{
#if defined(RTS_CFG_IO_TRACE)
		printf("[%s] Not yet configured.\n", __func__);
#endif
		RETURN(ERR_FB_NOT_OPERATING);
	}
#if 0
	if (pNotify->uTask != 0xffffu)
	{
		/* A IEC task has written into the output segment.
		 * --------------------------------------------------------------------
		 */

	} /* if (pNotify->uTask != 0xffffu) */

	else
	{
		/* An external application (i.e. the 4C Watch) has written
		 * into the output or input segment.
		 * --------------------------------------------------------------------
		 */

	} /* else (pNotify->uTask != 0xffffu) */
#endif

	pthread_mutex_lock(&mWriteMutex);

    if ((can0_cfg.enabled) && Can0InitDone && (pIO->usChannel == 2)) { // KAD_CAN_CHANNEL_0
	}
    if ((can1_cfg.enabled) && Can1InitDone && (pIO->usChannel == 3)) { // KAD_CAN_CHANNEL_1
	}

	pthread_mutex_unlock(&mWriteMutex);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * canNotifyGet
 *
 */
IEC_UINT canNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;


	if (FB_STATE_OPERATING != FB_STATE_OPERATING)
	{
		/* PB is not operating
		 */
		RETURN(ERR_FB_NOT_OPERATING);
	}

	if(CanDone == 0)
	{
#if defined(RTS_CFG_IO_TRACE)
		printf("[%s] Not yet configured.\n", __func__);
#endif
		RETURN(ERR_FB_NOT_OPERATING);
	}
#if 0
	if (pNotify->uTask != 0xffffu)
	{
		/* A IEC task is going to read from the input and/or output segment.
		 * --------------------------------------------------------------------
		 */

	} /* if (pNotify->uTask != 0xffffu) */

	else
	{
		/* An external application (r.e. the 4C Watch) is going to read from
		 * the input and/or output segment.
		 * --------------------------------------------------------------------
		 */

	} /* else (pNotify->uTask != 0xffffu) */
#endif

	pthread_mutex_lock(&mReadMutex);
	if((can0_cfg.enabled) && Can0InitDone && (pIO->usChannel == 2) { // KAD_CAN_CHANNEL_0
	}
	if((can1_cfg.enabled) && Can1InitDone && (pIO->usChannel == 3) { // KAD_CAN_CHANNEL_1
	}

	pthread_mutex_unlock(&mReadMutex);

	RETURN(uRes);
}

#endif /* RTS_CFG_IOCANOPEN */

/* ---------------------------------------------------------------------------- */

