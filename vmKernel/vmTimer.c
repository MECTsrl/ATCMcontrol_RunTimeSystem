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
 * Filename: vmTimer.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"vmTimer.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_VM_IPC)

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

typedef struct 
{
	IEC_UDINT	tCycle;
	IEC_UDINT	tNextExec;
	
} SVMAction;

#undef	VMM_TIMER_DEBUG
#define VMM_TRACE_DEPTH 	100

/* ----  Global Variables:	 -------------------------------------------------- */

static IEC_UDINT g_ulSystemWatchDog = 0xfffffffful;

/* ----  Local Functions:	--------------------------------------------------- */

static void timInit(STaskInfoVMM *pVMM, SVMAction *pACT);

#define tim_osSleep(delay_ms) 	\
do {				\
	XX_GPIO_CLR(0);		\
	osSleep(delay_ms);	\
	XX_GPIO_SET(0);		\
} while (0)

#define tim_osSleepAbsolute(time_ms) 	\
do {					\
	XX_GPIO_CLR(0);			\
	osSleepAbsolute(time_ms);	\
	XX_GPIO_SET(0);			\
} while (0)

/* ----  Implementations:	--------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/**
 * timMain
 *
 */
IEC_UINT timMain(void *pPara)
{
	STaskInfoVMM *pVMM = (STaskInfoVMM *)pPara;
	
	SVMAction	*pACT = NULL;

	SMessage inMsg;
	SMessage outMsg;

	IEC_UDINT bCheck = FALSE;
	IEC_UDINT bExec  = FALSE;
	
	IEC_UDINT bDelay = FALSE;

	IEC_UDINT i;
	IEC_UINT  uRes		= OK;

	IEC_UDINT tCurrent	= 0ul;
	IEC_UDINT tDiff 	= 0ul;
	IEC_UDINT tSuspend	= 0xfffffffful;
	IEC_UDINT tCheckMsg = 0ul;

	IEC_UDINT ulTasks	= 0ul;

  #if defined(VMM_TIMER_DEBUG)
	IEC_UDINT xx[VMM_TRACE_DEPTH];	/* Timer suspend intervals		*/
	IEC_UDINT ts[VMM_TRACE_DEPTH];	/* Timer execution time stamps	*/
	IEC_UDINT ex[VMM_TRACE_DEPTH];	/* Timer cycle execution time	*/

	IEC_UDINT tStart	= 0ul;
	IEC_UDINT tTimExec	= 0ul;

	IEC_UDINT ul		= 0ul;
	IEC_UDINT ulB		= 0ul;
  #endif

	/* Initialize
	 */
	pACT = (SVMAction *)osMalloc(sizeof(SVMAction) * MAX_TASKS);
	if (pACT == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	OS_MEMSET(pACT, 0x00, sizeof(SVMAction) * MAX_TASKS);

  #if defined(VMM_TIMER_DEBUG)
	OS_MEMSET(xx, 0x00, sizeof(IEC_UDINT) * VMM_TRACE_DEPTH);
	OS_MEMSET(ts, 0x00, sizeof(IEC_UDINT) * VMM_TRACE_DEPTH);
	OS_MEMSET(ex, 0x00, sizeof(IEC_UDINT) * VMM_TRACE_DEPTH);
  #endif

	outMsg.uID			= MSG_TIMER;
	outMsg.uLen 		= 0;
	outMsg.uRespQueue	= IPC_Q_NONE;

	uRes = osCreateIPCQueue(Q_LIST_VTI);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

  #if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo(TASK_OFFS_SYS_TIM, osGetTaskID());
	TR_RET(uRes);
  #endif
	
	//XX_GPIO_ENABLE_THREAD();
	XX_GPIO_SET(0); /* vedi tim_osSleep() */

	for ( ; ; )
	{
		tCurrent = osGetTime32Ex();
		
	  #if defined(VMM_TIMER_DEBUG)
		tTimExec = tCurrent;
	  #endif

		/* Execute pending IEC/VM tasks
		 * --------------------------------------------------------------------
		 */
		if (bCheck == FALSE && bExec == TRUE)
		{
			for (i = 0; i < ulTasks; i++)
			{
				SVMAction	*pAction = pACT + i;
				STaskInfoVM *pVM	 = pVMM->ppVM[i];

				if (tCurrent >= pAction->tNextExec)
				{
					pAction->tNextExec += pAction->tCycle;

					if (pVM->Task.bWDogEnable == TRUE)
					{
						/* Handle watch dog
						 */
						if (pVM->Task.ulWDogCounter > pVM->Task.ulWDogTrigger)
						{
							pVM->Local.pState->ulState	= TASK_STATE_ERROR;
							pVM->Local.pState->ulErrNo |= EXCEPT_WATCHDOG;

							pVM->Task.ulWDogCounter = 0;

							continue;
						}
					}
					else
					{
						if (pVM->Task.ulWDogCounter > WD_MAX_PENDING_INACTIVE)
						{
							continue;
						}
					}

					pVM->Task.ulWDogCounter++;

					uRes = msgSend(&outMsg, (IEC_UINT)i);
					TR_RET(uRes);

				} /* if (tCurrent >= pAction->tNextExec) */

			} /* for (i = 0; i < ulTasks; i++) */

		} /* if (bCheck == FALSE && bExec == TRUE) */


		/* Check for waiting messages
		 * --------------------------------------------------------------------
		 */
		if (tCurrent - tCheckMsg >= VMM_TIM_MESSAGE_CHECK)
		{
			tCheckMsg = tCurrent;
			
			uRes = msgRecv(&inMsg, Q_LIST_VTI, VMM_NO_WAIT);
			if (uRes == OK)
			{
				IEC_UINT uRespQueue = inMsg.uRespQueue;
				inMsg.uRespQueue	= IPC_Q_NONE;

				switch(inMsg.uID)
				{
					case MSG_TI_CONFIG:
					{
						ulTasks = pVMM->Project.uTasks;
						timInit(pVMM, pACT);
						bDelay = TRUE;

						break;
					}

					case MSG_TI_STOP:
					{
						bExec = FALSE;

						break;
					}

					case MSG_TI_START:
					{
						for (i = 0; i < ulTasks; i++)
						{
							pACT[i].tNextExec = tCurrent + /* $TODOD$ pACT[i].tCycle*/ + (bDelay == TRUE ? VM_FIRST_EXEC_DELAY : 0);
						}

					  #if defined(VMM_TIMER_DEBUG)
						tStart	= tCurrent;
					  #endif
						bExec	= TRUE;

						break;
					}

					default:
						msgSetError(&inMsg, ERR_INVALID_MSG);
						break;

				} /* switch(inMsg.uID) */

				if (uRespQueue != IPC_Q_NONE)
				{
					inMsg.uLen = 0;
					msgSend(&inMsg, uRespQueue);
				}

				if (inMsg.uID == MSG_TI_TERMINATE)
				{
					osFree((IEC_DATA **)&pACT);
					
					uRes = osDestroyIPCQueue(Q_LIST_VTI);

					RETURN(uRes);
				}
				
			} /* if (uRes == OK) */

		} /* if (tCurrent - tCheckMsg >= VMM_TIM_MESSAGE_CHECK) */


		/* Trigger system (still alive) watchdog
		 * --------------------------------------------------------------------
		 */
		if ((++g_ulSystemWatchDog % 10) == 0)
		{
			osTriggerSystemWatchdog();
		}


		/* Timer is not running
		 * --------------------------------------------------------------------
		 */
		if (bExec == FALSE)
		{
			tim_osSleep(VMM_MAX_TIMER_SUSPEND - VMM_SLEEP_OFFSET);
			bCheck = FALSE;

			continue;
		}


		/* Calculate next suspend time
		 * --------------------------------------------------------------------
		 */
		tSuspend = 0xfffffffful;
		tCurrent = osGetTime32Ex();

		for (i = 0; i < ulTasks; i++)
		{
			SVMAction *pAction = pACT + i;

			tDiff	= pAction->tNextExec - tCurrent;

			if (tDiff > 3ul * pAction->tCycle + (bDelay == TRUE ? VM_FIRST_EXEC_DELAY : 0))
			{
				pAction->tNextExec = tCurrent + pAction->tCycle;
				tDiff = pAction->tCycle;
			}

			if (tDiff < tSuspend)
			{
				tSuspend = tDiff;
			}
		}

		if (tSuspend >= VMM_SLEEP_OFFSET)
		{
			tSuspend -= VMM_SLEEP_OFFSET;
		}


		/* Suspend Timer
		 * --------------------------------------------------------------------
		 */
		if (tSuspend >= VMM_MAX_TIMER_SUSPEND)
		{
			tim_osSleep(VMM_MAX_TIMER_SUSPEND - VMM_SLEEP_OFFSET);

			bCheck = TRUE;

			continue;
		}

		if (tSuspend != 0)
		{
			tim_osSleepAbsolute(tCurrent + tSuspend);
		/*
		Questa modifica e' stata fatta da PE perche' a volte lo 
		scheduler fermava per 1ms l'esecuzione dei vari task.
		*/
		} else {
			tim_osSleep(1);
		}

		bDelay = FALSE;
		bCheck = FALSE;


		/* Debug Support
		 * --------------------------------------------------------------------
		 */
	  #if defined(VMM_TIMER_DEBUG)

		xx[ul]	= tSuspend + VMM_SLEEP_OFFSET;
		ts[ul]	= tCurrent - tStart - VM_FIRST_EXEC_DELAY;
		ex[ul]	= tCurrent - tTimExec;

		ul++;

		if (ul >= VMM_TRACE_DEPTH)
		{
			if (ulB == 0ul)
			{
				xx[0] = 0ul;
				ts[0] = 0ul;
			}

			ulB++;
			ul	= 0ul;
		}

	  #endif /* VMM_TIMER_DEBUG */
		
	} /* for ( ; ; ) */
	
	//XX_GPIO_DISABLE_THREAD();
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * timInit
 *
 */
static void timInit(STaskInfoVMM *pVMM, SVMAction *pACT)
{
	IEC_UDINT i;

	OS_MEMSET(pACT, 0x00, sizeof(SVMAction) * MAX_TASKS);

	for (i = 0; i < pVMM->Project.uTasks; i++)
	{
		SVMAction *pAction = pACT + i;

		if ((pVMM->ppVM[i]->Task.usAttrib & VMM_TASK_ATTR_CYCLIC) != 0)
		{
			pAction->tCycle = pVMM->ppVM[i]->Task.ulPara1;
		}
		else
		{
			pAction->tCycle = WD_EVTTASK_CYCLE;
		}

		pVMM->ppVM[i]->Task.ulWDogCounter = 0;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * timEnableAllWDog
 *
 */
IEC_UINT timEnableAllWDog(STaskInfoVMM *pVMM, IEC_BOOL bEnable)
{
	IEC_UINT uRes = OK;
	IEC_UINT i;

	for (i = 0; i < pVMM->Project.uTasks; i++)
	{
		if (bEnable == FALSE)
		{
			pVMM->ppVM[i]->Task.bWDogOldEnable = pVMM->ppVM[i]->Task.bWDogEnable;
		}
		else
		{
			pVMM->ppVM[i]->Task.bWDogOldEnable = TRUE;
		}

		pVMM->ppVM[i]->Task.bWDogEnable = bEnable;
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * timRestoreAllWDog
 *
 */
IEC_UINT timRestoreAllWDog(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;
	IEC_UINT i;

	for (i = 0; i < pVMM->Project.uTasks; i++)
	{
		pVMM->ppVM[i]->Task.bWDogEnable = pVMM->ppVM[i]->Task.bWDogOldEnable;
	}

	RETURN(uRes);
}

#endif	/* RTS_CFG_VM_IPC */

/* ---------------------------------------------------------------------------- */
