
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
 * Filename: fcAdapt.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"fcAdapt.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "fcDef.h"
#include "fcMain.h"

#include <sys/mman.h>
#include <fcntl.h>

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * VM_Proc
 *
 */
void *VM_Proc(void *lpParam)
{
	STaskInfoVM *pVM = (STaskInfoVM *)lpParam;

	struct sched_param sp;
	sp.sched_priority = pVM->Task.usPriority;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_VM, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

	pthread_cleanup_push(VM_CleanUp_Common , lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '" TASK_NAME_IEC_VM "' created with pid %d.\r\n", ((STaskInfoVM *)lpParam)->usTask, getpid());
  #endif

	vmMain(pVM);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '" TASK_NAME_IEC_VM "' terminated with pid %d.\r\n", ((STaskInfoVM *)lpParam)->usTask, getpid());
  #endif

	pthread_cleanup_pop(0);

	/* No detaching, VMM waits for cancellation!
	 */

	return NULL;
}

/* ---------------------------------------------------------------------------- */
/**
 * SOCKET_ListenThread
 *
 */
#if defined(RTS_CFG_TCP_NATIVE)

void *SOCKET_ListenThread(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_LIST;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_LIST, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_COM_LIS, getpid());
  #endif

	sockListen((SComTCP *)lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_COM_LIS, getpid());
  #endif

	pthread_detach(pthread_self());

	return NULL;
}

#endif /* RTS_CFG_TCP_NATIVE */


/* ---------------------------------------------------------------------------- */
/**
 * SOCKET_CommThread
 *
 */
#if ! defined(RTS_CFG_SINGLE_TASK) && defined(RTS_CFG_TCP_NATIVE) && defined(RTS_CFG_MULTILINK)

void *SOCKET_CommThread(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_COM;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_COM, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '" TASK_NAME_COM_WRK "' created with pid %d.\r\n", ((SComTCP *)lpParam)->uTask, getpid());
  #endif

	sockComm((SComTCP *)lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '" TASK_NAME_COM_WRK "' terminated with pid %d.\r\n", ((SComTCP *)lpParam)->uTask, getpid());
  #endif

	pthread_detach(pthread_self());

	return NULL;
}

#endif /* ! RTS_CFG_SINGLE_TASK && RTS_CFG_TCP_NATIVE && RTS_CFG_MULTILINK */


/* ---------------------------------------------------------------------------- */
/**
 * VMM_OnlineChangeThread
 *
 */
#if ! defined(RTS_CFG_SINGLE_TASK) && defined(RTS_CFG_VM_IPC) && defined(RTS_CFG_ONLINE_CHANGE)

void *VMM_OnlineChangeThread(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_OC;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_OC, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d\r\n", TASK_NAME_SYS_OCH, getpid());
  #endif

	ocMain(lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d\r\n", TASK_NAME_SYS_OCH, getpid());
  #endif

	pthread_detach(pthread_self());

	return NULL;
}

#endif /* ! RTS_CFG_SINGLE_TASK && RTS_CFG_VM_IPC && RTS_CFG_ONLINE_CHANGE */


/* ---------------------------------------------------------------------------- */
/**
 * VM_TimerThread
 *
 */
#if ! defined(RTS_CFG_SINGLE_TASK) && defined(RTS_CFG_VM_IPC)

void *VM_TimerThread(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_VMTIMER;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_VMTIMER, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_SYS_TIM, getpid());
  #endif

	timMain(lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_SYS_TIM, getpid());
  #endif

	pthread_detach(pthread_self());

	return NULL;
}

#endif /* ! RTS_CFG_SINGLE_TASK && RTS_CFG_VM_IPC */


/* ---------------------------------------------------------------------------- */
/**
 * VMM_RetainThread
 *
 */
#if defined(RTS_CFG_EXT_RETAIN)

void *VMM_RetainThread(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_RET;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_RET, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_SYS_RET, getpid());
  #endif

	retMain(lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_SYS_RET, getpid());
  #endif
	
	pthread_detach(pthread_self());

	return NULL;
}

#endif /* RTS_CFG_EXT_RETAIN */


/* ---------------------------------------------------------------------------- */
/**
 * BAC_DeviceThread
 *
 */
#if defined(RTS_CFG_BACNET)

void *BAC_DeviceThread(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_BAC_DEV;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_BAC_DEV, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_BAC_DEV, getpid());
  #endif

	devMain(lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_BAC_DEV, getpid());
  #endif
	
	pthread_detach(pthread_self());

	return NULL;
}

#endif /* RTS_CFG_BACNET */


/* ---------------------------------------------------------------------------- */
/**
 * BAC_COVThread
 *
 */
#if defined(RTS_CFG_BACNET)

void *BAC_COVThread(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_BAC_COV;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_BAC_COV, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_BAC_COV, getpid());
  #endif

	covMain(lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_BAC_COV, getpid());
  #endif
	
	pthread_detach(pthread_self());

	return NULL;
}

#endif /* RTS_CFG_BACNET */


/* ---------------------------------------------------------------------------- */
/**
 * BAC_ScanThread
 *
 */
#if defined(RTS_CFG_BACNET)

void *BAC_ScanThread(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_BAC_SCN;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_BAC_SCN, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_BAC_SCN, getpid());
  #endif

	scnMain(lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_BAC_SCN, getpid());
  #endif
	
	pthread_detach(pthread_self());

	return NULL;
}

#endif /* RTS_CFG_BACNET */


/* ---------------------------------------------------------------------------- */
/**
 * BAC_FlashThread
 *
 */
#if defined(RTS_CFG_BACNET)

void *BAC_FlashThread(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_BAC_FLH;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_BAC_FLH, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_BAC_FLH, getpid());
  #endif

	flhMain(lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_BAC_FLH, getpid());
  #endif
	
	pthread_detach(pthread_self());

	return NULL;
}

#endif /* RTS_CFG_BACNET */


/* ---------------------------------------------------------------------------- */
/**
 * BAC_ConfigThread
 *
 */
#if defined(RTS_CFG_BACNET)

void *BAC_ConfigThread(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_BAC_CFG;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_BAC_CFG, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_BAC_CFG, getpid());
  #endif

	cfgMain(lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_BAC_CFG, getpid());
  #endif
	
	pthread_detach(pthread_self());

	return NULL;
}

#endif /* RTS_CFG_BACNET */


/* ---------------------------------------------------------------------------- */
/**
 * PDP_ManagementThread
 *
 */
#if defined(RTS_CFG_PROFI_DP)

void *PDP_ManagementThread(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_PDP_MGT;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_PDP_MGT, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_PDP_MGT, getpid());
  #endif

	dpMgt(lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_PDP_MGT, getpid());
  #endif
	
	pthread_detach(pthread_self());

	return NULL;
}

#endif /* RTS_CFG_BACNET */


/* ---------------------------------------------------------------------------- */
/**
 * IO_Layer_BACnet
 *
 */
#if defined(RTS_CFG_BACNET)

void *IO_Layer_BACnet(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_IO_BACNET;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_IO_BACNET, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_IOL_BAC, getpid());
  #endif

	bacMain(lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_IOL_BAC, getpid());
  #endif
	
	pthread_detach(pthread_self());

	return NULL;
}

#endif /* RTS_CFG_BACNET */


/* ---------------------------------------------------------------------------- */
/**
 * IO_Layer_Test
 *
 */
#if defined(RTS_CFG_IOTEST)

void *IO_Layer_Test(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_IO_TEST;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_IO_TEST, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_IOL_TST, getpid());
  #endif

	tstMain(lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_IOL_TST, getpid());
  #endif
	
	pthread_detach(pthread_self());

	return NULL;
}

#endif /* RTS_CFG_IOTEST */


/* ---------------------------------------------------------------------------- */
/**
 * IO_Layer_ProfiDP
 *
 */
#if defined(RTS_CFG_PROFI_DP)

void *IO_Layer_ProfiDP(void *lpParam)
{
	struct sched_param sp;
	sp.sched_priority = FC_PRIO_IO_PROFI_DP;

	int iRes = pthread_setschedparam(pthread_self(), FC_SCHED_IO_PROFI_DP, &sp);
	if (iRes != 0)
	{
		TR_ERR("pthread_setschedparam() failed", iRes);

		return NULL;
	}

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_IOL_PDP, getpid());
  #endif

	dpMain(lpParam);

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_IOL_PDP, getpid());
  #endif
	
	pthread_detach(pthread_self());

	return NULL;
}

#endif /* RTS_CFG_PROFI_DP */


/* ---------------------------------------------------------------------------- */
/**
 * VMM_CleanUp_Mutex
 *
 */
#if ! defined(RTS_CFG_SINGLE_TASK)

void VMM_CleanUp_Mutex(void *lpParam)
{
	osEndCriticalSection((IEC_UDINT)lpParam);
}

#endif /* RTS_CFG_SINGLE_TASK */


/* ---------------------------------------------------------------------------- */
/**
 * VM_CleanUp_Common
 *
 */
#if ! defined(RTS_CFG_SINGLE_TASK)

void VM_CleanUp_Common(void *lpParam)
{
	STaskInfoVM *pVM = (STaskInfoVM *)lpParam;

  #if defined(RTS_CFG_DEBUG_OUTPUT)
	osTrace("\r\n--- WARNING: VM task '%s' explicitly killed from VMM.\r\n", pVM->Task.szName);
  #endif	

	/* Clean up
	 */
	osDestroyIPCQueue(pVM->usTask);
	osOnVMTerminate(pVM);
}

#endif /* RTS_CFG_SINGLE_TASK */


/* ---------------------------------------------------------------------------- */


