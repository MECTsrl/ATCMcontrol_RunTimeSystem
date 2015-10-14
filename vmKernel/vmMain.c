
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
 * Filename: vmMain.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"vmMain.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

static IEC_UINT vmInitialize(STaskInfoVM *pVM);
static IEC_UINT vmFinalize(STaskInfoVM *pVM);
static IEC_UINT vmExecute(STaskInfoVM *pVM);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * vmMain
 *
 * Main function for the different VM (interpreter) tasks.
 *
 * To be used only for multi task operating systems not supporting 
 * interprocess communication.
 */
#if ! defined(RTS_CFG_VM_IPC)

IEC_UINT vmMain(STaskInfoVM *pVM)
{
	IEC_UDINT ulTime;
	IEC_UDINT ulState;

	if ((pVM->usFlags & TASK_FLAG_INITIALIZED) == 0)
	{
		if (vmInitialize(pVM) != OK)
		{
			vmSetException(pVM, EXCEPT_TASK_INIT);
		}		 
	}

	for ( ; ; )
	{
		ulState = pVM->Local.pState->ulState;

		if (ulState != TASK_STATE_RUNNING && ulState != TASK_STATE_STEP)
		{ 
			if (ulState == TASK_STATE_DELETED)
			{
				/* Terminate task
				 */
				vmFinalize(pVM);
				RETURN(OK);
			}

			osSleep(50);
			continue;
		}

		pVM->Local.ulCurExeTime = osGetTime32();

		if (vmExecute(pVM) != OK)
		{
			osSleep(50);
			continue;
		}

		/* Check execution time
		 */
		 ulTime = osGetTime32() - pVM->Local.ulCurExeTime; 
		 osSleep(pVM->Task.ulPara1 < ulTime ? 1ul : pVM->Task.ulPara1 - ulTime);
		
	} /* end for */
	
	RETURN(OK);
}
#endif	/* ! RTS_CFG_VM_IPC */

/* ---------------------------------------------------------------------------- */
/**
 * vmMain
 *
 * Main function for the different VM (interpreter) tasks.
 *
 * To be used only for multi task operating systems supporting inter
 * process communication.
 */
#if defined(RTS_CFG_VM_IPC)
IEC_UINT vmMain(STaskInfoVM *pVM)
{
	SMessage Message;
	IEC_UINT uRespQueue;

	STaskState OS_DPTR *pState = pVM->Local.pState;

	IEC_BOOL bCyclic = (IEC_BOOL)((pVM->Task.usAttrib & VMM_TASK_ATTR_CYCLIC) != 0);

	/* Initialize task
	 */
	IEC_UINT uRes = vmInitialize(pVM);
	if (uRes != OK)
	{
		vmSetException(pVM, EXCEPT_TASK_INIT);
		RETURN(uRes);
	}		 

	if (pVM->usTask == 0) {
		XX_GPIO_SET(1);
	}

	for ( ; ; )
	{
		if (pVM->usTask == 0) {
			XX_GPIO_CLR(1);
		}

		if (msgRecv(&Message, pVM->usTask, VMM_WAIT_FOREVER) != OK)
		{
			osSleep(50);

			if (pVM->usTask == 0) {
				XX_GPIO_SET(1);
			}

			continue;
		}

		if (pVM->usTask == 0) {
			XX_GPIO_SET(1);
		}

		uRespQueue			= Message.uRespQueue;
		Message.uRespQueue	= IPC_Q_NONE;

		switch (Message.uID)
		{
			case MSG_TIMER: /* ------------------------------------------------ */
			{
				if (pVM->Task.ulWDogCounter > 0ul)
				{
					pVM->Task.ulWDogCounter--;
				}

				if (bCyclic == FALSE)
				{
					/* Trigger watchdog only for event driven tasks!
					 */
					break;
				}

				if (pState->ulState == TASK_STATE_RUNNING || pState->ulState == TASK_STATE_STEP)
				{
					vmExecute(pVM);
				}
				break;
			}

			case MSG_EVENT: /* ------------------------------------------------ */
			{
			  #if defined(RTS_CFG_EVENTS)
				pVM->usEvent = (IEC_USINT)*(IEC_UINT *)Message.pData;
			  #endif

				if (pState->ulState == TASK_STATE_RUNNING)
				{
					vmExecute(pVM);
				}
				break;
			}

			case MSG_VM_START:	/* -------------------------------------------- */
			{
				pVM->Local.uHalt = FALSE;
				
				pState->ulState = TASK_STATE_RUNNING;
				pState->ulErrNo = OK;

				pVM->usFlags &= ~TASK_FLAG_NEW_ERROR;

			  #if defined(RTS_CFG_EVENTS)
				pVM->usException = OK;
			  #endif

				if (uRespQueue != IPC_Q_NONE)
				{
					Message.uLen = 0;
					msgSend(&Message, uRespQueue);
				}

				break;
			}
				
			case MSG_VM_STOP: /* ---------------------------------------------- */
			{
				pVM->Local.uHalt = FALSE;

				if (pState->ulState == TASK_STATE_BREAK)
				{
					vmmQueueBPNotification(pVM, BP_STATE_LEAVED);
					vmSetException(pVM, EXCEPT_TASK_BREAK);

					/* Restart the task from the beginning
					 */
					pVM->Local.uContext = 1;
				}
				else
				{
					pState->ulState = TASK_STATE_STOPPED;
				}

				if (uRespQueue != IPC_Q_NONE)
				{
					Message.uLen = 0;
					msgSend(&Message, uRespQueue);
				}

				break;
			}

			case MSG_VM_CONTINUE: /* ------------------------------------------ */
			{
				pVM->Local.uHalt = FALSE;

				if (pState->ulState != TASK_STATE_BREAK)
				{
					msgSetError(&Message, ERR_TASK_NOT_IN_BREAK);
				}
				else
				{
					Message.uLen = 0;
					vmmQueueBPNotification(pVM, BP_STATE_LEAVED);
					
					pState->ulState = TASK_STATE_RUNNING;
				}

				if (uRespQueue != IPC_Q_NONE)
				{
					msgSend(&Message, uRespQueue);
				}

				if (bCyclic == FALSE)
				{
					vmExecute(pVM);
				}

				break;
			}

			case MSG_VM_STEP: /* ---------------------------------------------- */
			{
				pVM->Local.uHalt = FALSE;

				if (pState->ulState != TASK_STATE_BREAK)
				{
					msgSetError(&Message, ERR_TASK_NOT_IN_BREAK);
				}
				else
				{
					Message.uLen = 0;
					vmmQueueBPNotification(pVM, BP_STATE_LEAVED);

					pState->ulState = TASK_STATE_STEP;
				}

				if (uRespQueue != IPC_Q_NONE)
				{
					msgSend(&Message, uRespQueue);
				}

				if (bCyclic == FALSE)
				{
					vmExecute(pVM);

					if (pState->ulState == TASK_STATE_STEP)
					{
						/* If the current cycle is executed completely
						 * without reaching another breakpoint, we
						 * have to wait for the next event.
						 */
						pState->ulState = TASK_STATE_RUNNING;
					}
				}

				break;
			}

			case MSG_VM_RESET:	/* -------------------------------------------- */
			{
				pVM->Local.uHalt = FALSE;

				if (pState->ulState == TASK_STATE_BREAK)
				{
					vmmQueueBPNotification(pVM, BP_STATE_LEAVED);
					vmSetException(pVM, EXCEPT_TASK_BREAK);

					/* Restart the task from the beginning
					 */
					pVM->Local.uContext = 1;
				}

				if (uRespQueue != IPC_Q_NONE)
				{
					Message.uLen = 0;
					msgSend(&Message, uRespQueue);
				}

				break;
			}
				
			case MSG_VM_TERMINATE: /* ----------------------------------------- */
			{
				pVM->Local.uHalt = FALSE;

				vmFinalize(pVM);

				if (uRespQueue != IPC_Q_NONE)
				{
					Message.uLen = 0;
					msgSend(&Message, uRespQueue);
				}
				goto return_ok;
			}

			default:	/* ---------------------------------------------------- */
			{
				TR_ERROR("[vmMain] Unexpected message (0x%04x) received.\r\n", Message.uID);

				if (uRespQueue != IPC_Q_NONE)
				{
					msgSetError(&Message, ERR_INVALID_MSG); 			
					msgSend(&Message, uRespQueue);
				}

				break;
			}
		
		}	/* switch (Message.uID) */
	
	}	/* for (; ;) */

return_ok:
	RETURN(OK);
}
#endif /* RTS_CFG_VM_IPC */

/* ---------------------------------------------------------------------------- */
/**
 * vmInitialize
 *
 * Initialize the VM.
 */
static IEC_UINT vmInitialize(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;

	/* Initialize Interpreter
	 */
	uRes = intInitialize(pVM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = osCreateIPCQueue(pVM->usTask);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* OS dependent initializations
	 */
	uRes = osInitializeVM(pVM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (pVM->Task.usAttrib & VMM_TASK_ATTR_UNLOADED)
	{
		pVM->Local.pState->ulState = TASK_STATE_UNLOADED;
	}
	else
	{
		pVM->Local.pState->ulState = pVM->Task.usAttrib & VMM_TASK_ATTR_AUTOSTART ? TASK_STATE_RUNNING : TASK_STATE_STOPPED;		
	}

  #if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo((IEC_UINT)(TASK_OFFS_IEC_VM + pVM->usTask), osGetTaskID());
	TR_RET(uRes);
  #endif

	pVM->usFlags |= TASK_FLAG_INITIALIZED;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmFinalize
 *
 */
static IEC_UINT vmFinalize(STaskInfoVM *pVM)
{
	IEC_UINT uRes = osDestroyIPCQueue(pVM->usTask);

	uRes |= osOnVMTerminate(pVM);

  #if defined(RTS_CFG_SYSLOAD)
	uRes = ldClearTaskInfo((IEC_UINT)(TASK_OFFS_IEC_VM + pVM->usTask));
	TR_RET(uRes);
  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmCreateTaskInfoVM
 *
 */
STaskInfoVM *vmCreateTaskInfoVM(IEC_UINT uTask, IEC_UINT uTasks)
{
	IEC_UINT uRes = OK;

	/* Create STaskInfoVM
	 */
	STaskInfoVM *pVM = osCreateTaskInfoVM(uTask);
	if (pVM == NULL)
	{
		return NULL;
	}

	OS_MEMSET(pVM, 0x00, sizeof(STaskInfoVM));
	
	pVM->usTask = (IEC_USINT)uTask;

	uRes = osCreateVMMembers(pVM);
	if (uRes != OK)
	{
		vmFreeTaskInfoVM(pVM);
		return NULL;
	}

	pVM->Task.pIR = osCreateImageReg(pVM, uTasks);
	if (pVM->Task.pIR == NULL)
	{
		vmFreeTaskInfoVM(pVM);
		return NULL;
	}
  
	OS_MEMSET(pVM->Task.pIR, 0x00, sizeof(SImageReg));

	return pVM;
}

/* ---------------------------------------------------------------------------- */
/**
 * vmFreeTaskInfoVM
 *
 */
IEC_UINT vmFreeTaskInfoVM(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;

	if (pVM->Task.pIR != NULL)
	{		
		uRes |= osFreeImageReg(pVM);
	}

	uRes |= osFreeVMMembers(pVM);
	uRes |= osFreeTaskInfoVM(pVM);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmSetException
 *
 * Sets the task in an error state. (The task is stopped and the 
 * error code is forwarded to the engineering.)
 */
IEC_UINT vmSetException(STaskInfoVM *pVM, IEC_UDINT ulErrNo)
{
	SException Exception;

	IEC_UDINT ulOld = pVM->Local.pState->ulErrNo;

	pVM->Local.pState->ulState	= TASK_STATE_ERROR;
	pVM->Local.pState->ulErrNo |= ulErrNo;

	if ((ulOld & ulErrNo) == 0 || ulErrNo == EXCEPT_WATCHDOG)
	{
		/* New error, signal to VMM
		 */
		pVM->usFlags |= TASK_FLAG_NEW_ERROR;
	}
	
	Exception.uErrNo	= (IEC_UINT)ulErrNo;

	Exception.uCode 	= NO_INDEX;
	Exception.uData 	= NO_INDEX;
	Exception.ulOffset	= 0;

  #if defined(RTS_CFG_EVENTS)
	pVM->usException = (IEC_USINT)ulErrNo;
	vmmSetEvent((STaskInfoVMM *)pVM->pShared->pVMM, EVT_EXCEPTION);
  #endif

	osHandleException(pVM, &Exception);

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmInitializeLocalSegments
 *
 */
IEC_UINT vmInitializeLocalSegments(STaskInfoVM *pVM)
{
	IEC_UINT	uRes = OK;

	SIntShared *pShared = pVM->pShared;
	SIntLocal  *pLocal	= &pVM->Local;

	OS_MEMCPY(pLocal->pSeg, pShared->pData, sizeof(SObject) * MAX_SEGMENTS);
  #if defined(RTS_CFG_WRITE_FLAGS)
	OS_MEMSET(&pLocal->WriteFlags, 0x00, sizeof(SObject));
  #endif

  #if defined (RTS_CFG_TASK_IMAGE)

	if ((pLocal->pSeg[SEG_INPUT].pAdr  = osCreateLocSegment(pVM->usTask, SEG_INPUT, pShared->pData[SEG_INPUT].ulSize)) == 0)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}
	if ((pLocal->pSeg[SEG_OUTPUT].pAdr = osCreateLocSegment(pVM->usTask, SEG_OUTPUT, pShared->pData[SEG_OUTPUT].ulSize)) == 0)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

  #endif

  #if defined (RTS_CFG_WRITE_FLAGS)

	if ((pLocal->WriteFlags.pAdr = osCreateWFSegment(pVM->usTask, pShared->pData[SEG_OUTPUT].ulSize)) == 0)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}
	pLocal->WriteFlags.ulSize = pShared->pData[SEG_OUTPUT].ulSize;

  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmFinalizeLocalSegments
 *
 */
#if defined (RTS_CFG_TASK_IMAGE) | defined(RTS_CFG_WRITE_FLAGS)

IEC_UINT vmFinalizeLocalSegments(STaskInfoVM *pVM)
{
	SIntLocal  *pLocal	= &pVM->Local;

  #if defined (RTS_CFG_TASK_IMAGE)

	osFreeLocSegment(pVM->usTask, SEG_INPUT, &pLocal->pSeg[SEG_INPUT].pAdr);
	osFreeLocSegment(pVM->usTask, SEG_OUTPUT, &pLocal->pSeg[SEG_OUTPUT].pAdr);

  #endif	/* RTS_CFG_TASK_IMAGE */

  #if defined (RTS_CFG_WRITE_FLAGS)

	osFreeWFSegment(pVM->usTask, &pLocal->WriteFlags.pAdr);

  #endif	/* RTS_CFG_WRITE_FLAGS */

	RETURN(OK);
}
#endif /* RTS_CFG_TASK_IMAGE | RTS_CFG_WRITE_FLAGS */

/* ---------------------------------------------------------------------------- */
/**
 * vmExecute
 *
 */
static IEC_UINT vmExecute(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;

  #if defined(RTS_CFG_TASKSTAT)
	IEC_ULINT ll = osGetTimeUSEx();
  #endif

	/* Get local task image
	 */
  #if defined(RTS_CFG_TASK_IMAGE) || defined(RTS_CFG_WRITE_FLAGS)
	uRes = prcGetTaskImage(pVM);
	if (uRes != OK)
	{
		vmSetException(pVM, EXCEPT_TASK_IMAGE);
		RETURN(uRes);
	}
  #endif

	/* Execute interpreter
	 */
	uRes = intRun(pVM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	/* Update global process image
	 */
  #if defined(RTS_CFG_TASK_IMAGE) || defined(RTS_CFG_WRITE_FLAGS)
	uRes = prcSetTaskImage(pVM);
	if (uRes != OK)
	{
		vmSetException(pVM, EXCEPT_TASK_IMAGE);
		RETURN(uRes);
	}
  #endif

	/* Handle local retain variables
	 */
  #if defined(RTS_CFG_COPY_DOMAIN)
	uRes = vmmCopyRegions(pVM->pShared, pVM->Task.uCpyRegOff, pVM->Task.uCpyRegions, FALSE);
	if (uRes != OK)
	{
		vmSetException(pVM, EXCEPT_UNKOWN);
		RETURN(uRes);
	}
  #endif

  #if defined(RTS_CFG_TASKSTAT)
	{
		SStatistic *pStat = &pVM->Stat;

		IEC_UDINT ulDiff = utilGetTimeDiffMSEx(ll);

		pStat->ulExecCounter++;
		pStat->ullExecSum += ulDiff;

		if (pStat->ulExecCounter > 50)
		{
			if (ulDiff > pStat->ulExecMax)
			{
				pStat->ulExecMax = ulDiff;
			}
			if (ulDiff < pVM->Stat.ulExecMin)
			{
				pStat->ulExecMin = ulDiff;
			}
		}

		pStat->ulExecAver = (IEC_UDINT)(pStat->ullExecSum / pStat->ulExecCounter);
	}
  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
