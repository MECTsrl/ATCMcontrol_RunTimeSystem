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
 * Filename: mectAdapt.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"mectAdapt.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "fcDef.h"
#include "canMain.h"
#include "udpMain.h"
#include "dataMain.h"
#include "syncroMain.h"
#include "mbtcpsMain.h"
#include "mbrtucMain.h"
#include "mectMain.h"
#include "kpdMain.h"

#include <sys/mman.h>
#include <fcntl.h>

#include <time.h>
#include <signal.h>
#include <assert.h>
#include "mectCfgUtil.h"

/* ----  Local Defines:   ----------------------------------------------------- */
/* step in ms of the canopen stack manager */
#define STACK_RATE_MS   10
/* step in ms of the canopen message manager */
#define MESSAGE_RATE_MS 1
/*
   kind of clock (could be CLOCK_MONOTONIC or REAL_TIME)
CLOCK_MONOTONIC: the timer return time in seconds and milliseconds since the timer creation
CLOCK_REALTIME: the timer return the time in seconds and milliseconds since the Epoch (January 1, 1970)
 */
#define CLOCKID CLOCK_MONOTONIC

#define SIG_STACK SIGRTMIN
#define SIG_MSG   SIGRTMAX
#define MAX_LINE_SIZE 81

/* ----  Global Variables:	 -------------------------------------------------- */
int Can0InitDone;
int Can1InitDone;
int CanDone;
#ifdef RTS_CFG_IOCANOPEN
serial_cfg_s can0_cfg;
serial_cfg_s can1_cfg;
#endif
#ifdef RTS_CFG_IOMBRTUC
mbrtu_cfg_s modbus0_cfg;
mbrtu_cfg_s modbus1_cfg;
#endif

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * IO_Layer_CANopen
 *
 */
#if defined(RTS_CFG_IOCANOPEN)

void *IO_Layer_CANopen(void *lpParam)
{
	if (app_config_load(APP_CONF_CAN0))
	{
		//fprintf(stderr, "[%s]: Error Can0 module configuration file is wrong: abort initialization.\n", __func__);
	}

	if (app_config_load(APP_CONF_CAN1))
	{
		//fprintf(stderr, "[%s]: Error Can1 module configuration file is wrong: abort initialization.\n", __func__);
	}

	if (can0_cfg.enabled == 0 && can1_cfg.enabled == 0)
	{
		//fprintf(stderr, "[%s]: Warning Can module is build but is not used: abort initialization.\n", __func__);
	}

	osPthreadSetSched(FC_SCHED_IO_CANOPEN, FC_PRIO_IO_CANOPEN);

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_IOL_CAN, getpid());
#endif

	canMain(lpParam); // in ioCANopen/canMain.c

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_IOL_CAN, getpid());
#endif

	pthread_detach(pthread_self());

	return NULL;
}

#endif /* RTS_CFG_IOCANOPEN */


/* ---------------------------------------------------------------------------- */
/**
 * IO_Layer_Udp
 *
 */
#if defined(RTS_CFG_IOUDP)

void *IO_Layer_UDP(void *lpParam)
{
	osPthreadSetSched(FC_SCHED_IO_UDP, FC_PRIO_IO_UDP);

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_IOL_UDP, getpid());
#endif

	udpMain(lpParam);

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_IOL_UDP, getpid());
#endif

	pthread_detach(pthread_self());

	return NULL;
}
#endif /* RTS_CFG_IOUDP */

/* ---------------------------------------------------------------------------- */
/**
 * IO_Layer_Data
 *
 */
#if defined(RTS_CFG_IODAT)

void *IO_Layer_DAT(void *lpParam)
{
#ifndef __XENO__
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_IO_DAT; // see fcDef.h (general Linux priorities)

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_IO_DAT, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}
#else
	int iRes = osPthreadSetSched(FC_SCHED_IO_DAT, FC_PRIO_IO_DAT);
	if (iRes != 0)
		return NULL;
#endif

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_IOL_DAT, getpid());
#endif

	dataMain(lpParam);

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_IOL_DAT, getpid());
#endif

	pthread_detach(pthread_self());

	return NULL;
}
#endif /* RTS_CFG_IODAT */


/* ---------------------------------------------------------------------------- */
/**
 * IO_Layer_SYN
 *
 */
#if defined(RTS_CFG_IOSYN)

void *IO_Layer_SYN(void *lpParam)
{
#ifndef __XENO__
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_IO_SYN; // see fcDef.h (general Linux priorities)

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_IO_SYN, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}
#else
	int iRes = osPthreadSetSched(FC_SCHED_IO_SYN, FC_PRIO_IO_SYN);
	if (iRes != 0)
		return NULL;
#endif

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_IOL_SYN, getpid());
#endif

	syncroMain(lpParam);

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_IOL_SYN, getpid());
#endif

	pthread_detach(pthread_self());

	return NULL;
}
#endif /* RTS_CFG_IOSYN */


/* ---------------------------------------------------------------------------- */
/**
 * IO_Layer_Keypad
 *
 */
#if defined(RTS_CFG_IOKEYPAD)

void *IO_Layer_Keypad(void *lpParam)
{
	osPthreadSetSched(FC_SCHED_IO_KEYPAD, FC_PRIO_IO_KEYPAD);

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_IOL_KPD, getpid());
#endif

	kpdMain(lpParam);

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_IOL_KPD, getpid());
#endif

	pthread_detach(pthread_self());

	return NULL;
}
#endif /* RTS_CFG_IOKEYPAD */

/* ---------------------------------------------------------------------------- */
/**
 * IO_Layer_ModbusTCPS
 *
 */
#if defined(RTS_CFG_IOMBTCPS)

void *IO_Layer_ModbusTCPS(void *lpParam)
{
#ifndef __XENO__
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_IO_MBTCPS; // see fcDef.h (general Linux priorities)

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_IO_MBTCPS, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}
#else
	int iRes = osPthreadSetSched(FC_SCHED_IO_MBTCPS, FC_PRIO_IO_MBTCPS);
	if (iRes != 0)
		return NULL;
#endif

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_IOL_MBTCPS, getpid());
#endif

	mbtcpsMain(lpParam);

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_IOL_MBTCPS, getpid());
#endif

	pthread_detach(pthread_self());

	return NULL;
}
#endif /* RTS_CFG_IOMBTCPS */

/* ---------------------------------------------------------------------------- */
/**
 * IO_Layer_ModbusRTUC
 *
 */
#if defined(RTS_CFG_IOMBRTUC)

void *IO_Layer_ModbusRTUC(void *lpParam)
{
#ifndef __XENO__
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_IO_MBRTUC; // see fcDef.h (general Linux priorities)

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_IO_MBRTUC, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}
#else
	int iRes = osPthreadSetSched(FC_SCHED_IO_MBRTUC, FC_PRIO_IO_MBRTUC);
	if (iRes != 0)
		return NULL;
#endif

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_IOL_MBRTUC, getpid());
#endif

	mbrtucMain(lpParam);

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_IOL_MBRTUC, getpid());
#endif

	pthread_detach(pthread_self());

	return NULL;
}
#endif /* RTS_CFG_IOMBRTUC */
