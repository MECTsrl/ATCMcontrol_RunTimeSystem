
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
 * Filename: libSys2.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"libSys2.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_SYSTEM_LIB_NT)

#include "libSys2.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define DEC_PARA(ty)		ty OS_SPTR *pPar = (ty OS_SPTR *)pIN

/* ----  Global Variables:	 -------------------------------------------------- */

extern EXECUTE_FUN	g_pLibraryFun[];
extern EXECUTE_FB	g_pLibraryFB [];

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * 135 - EVT_GetException
 *
 */
void EVT_GetException(STDLIBFUNCALL)
{
	DEC_PARA(S_EVT_GetException);

	STaskInfoVMM *pVMM = (STaskInfoVMM *)pVM->pShared->pVMM;
	IEC_UINT	 uTask = 0xffffu;

  #if ! defined(RTS_CFG_EVENTS)
	pPar->wRet = ERR_NOT_SUPPORTED;
	RETURN_v(pPar->wRet);
  #endif

	pPar->wRet = ERR_INVALID_PARAM;

	for(uTask = 0; uTask < pVMM->Project.uTasks; uTask++)
	{
	  #if defined(RTS_CFG_EVENTS)
		STaskInfoVM *pVM = pVMM->ppVM[uTask];

		if(pVM->usException != 0 && pVM->Local.pState->ulState == TASK_STATE_ERROR)
		{
			*pPar->upTask	= uTask;
			*pPar->ulpEx	= pVM->usException;

			pVM->usException= OK;
			pPar->wRet		= OK;
			
			break;
		}
	  #endif
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * 136 - EVT_Set
 *
 */
void EVT_Set(STDLIBFUNCALL)
{
	DEC_PARA(S_EVT_Set);

  #if ! defined(RTS_CFG_EVENTS)
	pPar->wRet = ERR_NOT_SUPPORTED;
	RETURN_v(pPar->wRet);
  #endif

	pPar->uEvent = (IEC_UINT)(pPar->uEvent - 1);
	pPar->wRet	 = vmmSetEvent((STaskInfoVMM *)pVM->pShared->pVMM, pPar->uEvent);
}

/* ---------------------------------------------------------------------------- */
/**
 * 138 - MSG_SendToEng
 *
 */
void MSG_SendToEng(STDLIBFUNCALL)
{
	DEC_PARA(S_MSG_SendToEng);

	vmmQueueMessage(&pVM->pShared->MsgQueue, utilIecToAnsi(pPar->sMsg, pVM->Local.pBuffer));
}

/* ---------------------------------------------------------------------------- */
/**
 * 139 - MSG_Trace
 *
 */
void MSG_Trace(STDLIBFUNCALL)
{
  #if defined(RTS_CFG_DEBUG_OUTPUT)
	DEC_PARA(S_MSG_Trace);

	TR_STATE(utilIecToAnsi(pPar->sMsg, pVM->Local.pBuffer));
  #endif
}

/* --- 140 -------------------------------------------------------------------- */

/* Interpreter inline code in 4C library! */

/* ---------------------------------------------------------------------------- */
/**
 * 141 - TIM_Get
 *
 */
void TIM_Get(STDLIBFUNCALL)
{
	DEC_PARA(S_TIM_Get);

	pPar->tRet = osGetTime32();
}

/* ---------------------------------------------------------------------------- */
/**
 * 142 - MSG_SendToEng
 *
 */
void TSK_ClearError(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_ClearError);

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);
	if (pPar->wRet == OK)
	{
		((STaskInfoVMM *)pVM->pShared->pVMM)->ppVM[uTask]->Local.pState->ulErrNo = OK;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * 143 - MSG_SendToEng
 *
 */
void TSK_Exception(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_Exception);

	pPar->wRet = libSetException(pVM, pPar->uException);
}

/* ---------------------------------------------------------------------------- */
/**
 * 144 - TSK_GetCount
 */
void TSK_GetCount(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_GetCount);

	pPar->iRet = ((STaskInfoVMM *)pVM->pShared->pVMM)->Project.uTasks;
}

/* ---------------------------------------------------------------------------- */
/**
 * 145 - TSK_GetError
 *
 */
void TSK_GetError(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_GetError);

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);
	if (pPar->wRet == OK)
	{
		*pPar->ulErrNo = ((STaskInfoVMM *)pVM->pShared->pVMM)->ppVM[uTask]->Local.pState->ulErrNo;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * 146 - TSK_GetInfo
 *
 */
void TSK_GetInfo(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_GetInfo);

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);
	if (pPar->wRet == OK)
	{
		STaskState OS_DPTR *pState = ((STaskInfoVMM *)pVM->pShared->pVMM)->ppVM[uTask]->Local.pState;

		pPar->pInfo->ulState	= pState->ulState;
		pPar->pInfo->ulPriority = osConvert4CPriority((IEC_USINT)pState->ulPrio);
		pPar->pInfo->tCycle 	= pState->ulCycle;
		pPar->pInfo->tETMax 	= pState->ulETMax;
		pPar->pInfo->ulErrNo	= pState->ulErrNo;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * 147 - TSK_GetMyNumber
 *
 */
void TSK_GetMyNumber(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_GetMyNumber);
	
	pPar->uTask = pVM->usTask;
}

/* ---------------------------------------------------------------------------- */
/**
 * 148 - TSK_GetName
 *
 */
void TSK_GetName(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_GetName);

	STaskInfoVMM		*pVMM	= (STaskInfoVMM *)pVM->pShared->pVMM;
	IEC_STRING OS_DPTR	*s		= pPar->sTask;
	IEC_UINT			uLen;
	
	if (pPar->uTask >= pVMM->Project.uTasks)
	{
		pPar->wRet = ERR_ERROR;
		RETURN_v(pPar->wRet);
	}

	uLen = (IEC_UINT)(OS_STRLEN(pVMM->ppVM[pPar->uTask]->Task.szName));

	s->CurLen = (IEC_USINT)(uLen < s->MaxLen ? uLen : s->MaxLen);
	OS_MEMCPY(s->Contents, pVMM->ppVM[pPar->uTask]->Task.szName, s->CurLen);

	pPar->wRet = OK;
}

/* ---------------------------------------------------------------------------- */
/**
 * 149 - TSK_GetState
 *
 */
void TSK_GetState(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_GetState);

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);
	if (pPar->wRet == OK)
	{
		*pPar->ulErrNo = ((STaskInfoVMM *)pVM->pShared->pVMM)->ppVM[uTask]->Local.pState->ulState;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * 150 - TSK_Start
 *
 */
void TSK_Start(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_Start);

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);
	if (pPar->wRet == OK)
	{
		pPar->wRet = vmmStartTask((STaskInfoVMM *)pVM->pShared->pVMM, uTask);
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * 151 - TSK_Stop
 *
 */
void TSK_Stop(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_Stop);

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);
	if (pPar->wRet == OK)
	{
		pPar->wRet = vmmStopTask((STaskInfoVMM *)pVM->pShared->pVMM, uTask, FALSE);
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * 152 - WD_Disable
 *
 */
void WD_Disable(STDLIBFUNCALL)
{
	DEC_PARA(S_WD_Disable);

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);
  
	if (pPar->wRet == OK)
	{
		STaskInfoVMM *pVMM = (STaskInfoVMM *)pVM->pShared->pVMM;

		pVMM->ppVM[uTask]->Task.bWDogOldEnable	= pVMM->ppVM[uTask]->Task.bWDogEnable;
		pVMM->ppVM[uTask]->Task.bWDogEnable 	= FALSE;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * 153 - WD_Enable
 *
 */
void WD_Enable(STDLIBFUNCALL)
{
	DEC_PARA(S_WD_Enable);

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);

	if (pPar->wRet == OK)
	{
		STaskInfoVMM *pVMM = (STaskInfoVMM *)pVM->pShared->pVMM;

		pVMM->ppVM[uTask]->Task.bWDogOldEnable	= TRUE;
		pVMM->ppVM[uTask]->Task.bWDogEnable 	= TRUE;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * 156 - EVT_GetEvent
 *
 */
void EVT_GetEvent(STDLIBFUNCALL)
{
	DEC_PARA(S_EVT_GetEvent);

	IEC_UINT uTask = 0xffffu;

  #if ! defined(RTS_CFG_EVENTS)
	pPar->wRet = ERR_NOT_SUPPORTED;
	RETURN_v(pPar->wRet);
  #endif

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);

  #if defined(RTS_CFG_EVENTS)
	if (pPar->wRet == OK)
	{
		*pPar->upEvent = ((STaskInfoVMM *)pVM->pShared->pVMM)->ppVM[uTask]->usEvent;
	}
  #endif
}

/* ---------------------------------------------------------------------------- */
/**
 * 157 - WD_IsEnabled
 *
 */
void WD_IsEnabled(STDLIBFUNCALL)
{
	DEC_PARA(S_WD_IsEnabled);

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);
  
	if (pPar->wRet == OK)
	{
		*pPar->bEnabled = ((STaskInfoVMM *)pVM->pShared->pVMM)->ppVM[uTask]->Task.bWDogEnable;
	}

}

/* ---------------------------------------------------------------------------- */
/**
 * 158 - TSK_GetMyName
 *
 */
void TSK_GetMyName(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_GetMyName);

	utilAnsiToIec(pVM->Task.szName, pPar->sTask);
}

/* ---------------------------------------------------------------------------- */
/**
 * 159 - SYS_Coldstart
 *
 */
void SYS_Coldstart(STDLIBFUNCALL)
{
	DEC_PARA(S_SYS_Coldstart);

  #if ! defined(RTS_CFG_FLASH)
	pPar->wRet = ERR_NOT_SUPPORTED;
	RETURN_v(pPar->wRet);
  #endif
	
	pPar->wRet = OK;

  #if defined(RTS_CFG_VM_IPC) && defined(RTS_CFG_MULTILINK)

	if (((STaskInfoVMM *)pVM->pShared->pVMM)->bExecColdStart == FALSE)
	{
		pPar->wRet = msgTXCommand(MSG_VM_COLDSTART, Q_LIST_VMM, IPC_Q_NONE, VMM_NO_WAIT, TRUE);
	}

  #else

	((STaskInfoVMM *)pVM->pShared->pVMM)->bExecColdStart = TRUE;

  #endif
}

/* ---------------------------------------------------------------------------- */
/**
 * 180 - SYS_Reboot
 *
 */
void SYS_Reboot(STDLIBFUNCALL)
{
	DEC_PARA(S_SYS_Reboot);

	pPar->wRet = OK;
	
  #if defined(RTS_CFG_VM_IPC) && defined(RTS_CFG_MULTILINK)

  #if defined(RTS_CFG_EXT_RETAIN)
	pPar->wRet = msgTXCommand(MSG_RT_UPDATE, Q_LIST_RET, IPC_Q_NONE, VMM_NO_WAIT, TRUE);
  #endif

	if (pPar->wRet == OK)
	{
		pPar->wRet = msgTXCommand(MSG_VM_REBOOT, Q_LIST_VMM, IPC_Q_NONE, VMM_NO_WAIT, TRUE);
	}
  #else
	
  #if defined(RTS_CFG_EXT_RETAIN)
	pPar->wRet = ERR_NOT_SUPPORTED; /* retUpdate((STaskInfoVMM *)pVM->pShared->pVMM); */
  #endif
	if (pPar->wRet == OK)
	{
		pPar->wRet = osReboot();
	}
  #endif
}

/* ---------------------------------------------------------------------------- */
/**
 * 181 - SYS_RetainWrite
 *
 */
void SYS_RetainWrite(STDLIBFUNCALL)
{
	DEC_PARA(S_SYS_RetainWrite);

  #if ! defined(RTS_CFG_EXT_RETAIN)
	pPar->wRet = ERR_NOT_SUPPORTED;
	RETURN_v(pPar->wRet);
  #endif

  #if defined(RTS_CFG_VM_IPC) && defined(RTS_CFG_MULTILINK)

	pPar->wRet = msgTXCommand(MSG_RT_UPDATE, Q_LIST_RET, IPC_Q_NONE, VMM_NO_WAIT, TRUE);

  #else
	
	pPar->wRet = ERR_NOT_SUPPORTED; /* retUpdate((STaskInfoVMM *)pVM->pShared->pVMM); */ 

  #endif
}

/* ---------------------------------------------------------------------------- */
/**
 * 182 - SYS_Warmstart
 *
 */
void SYS_Warmstart(STDLIBFUNCALL)
{
	DEC_PARA(S_SYS_Warmstart);

  #if ! defined(RTS_CFG_FLASH)
	pPar->wRet = ERR_NOT_SUPPORTED;
	RETURN_v(pPar->wRet);
  #endif

	pPar->wRet = OK;
	
  #if defined(RTS_CFG_VM_IPC) && defined(RTS_CFG_MULTILINK)

	if (((STaskInfoVMM *)pVM->pShared->pVMM)->bExecWarmStart == FALSE)
	{
		pPar->wRet = msgTXCommand(MSG_VM_WARMSTART, Q_LIST_VMM, IPC_Q_NONE, VMM_NO_WAIT, TRUE);
	}

  #else

	((STaskInfoVMM *)pVM->pShared->pVMM)->bExecWarmStart = TRUE;

  #endif
}

/* ---------------------------------------------------------------------------- */
/**
 * 183 - LD_Get
 *
 */
void LD_Get(STDLIBFUNCALL)
{
	DEC_PARA(S_LD_Get);

  #if ! defined(RTS_CFG_SYSLOAD)
	pPar->wRet = ERR_NOT_SUPPORTED;
	RETURN_v(pPar->wRet);
  #else

	pPar->wRet = ldHandleSysLibCall(pPar->wLoadType, pPar->uTaskID, pPar->pLoad);

  #endif

}

/* ---------------------------------------------------------------------------- */
/**
 * 184 - WD_SetTrigger
 *
 */
void WD_SetTrigger(STDLIBFUNCALL)
{
	DEC_PARA(S_WD_SetTrigger);

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);

	if (pPar->wRet == OK)
	{
		STaskInfoVMM *pVMM = (STaskInfoVMM *)pVM->pShared->pVMM;

		pVMM->ppVM[uTask]->Task.ulWDogTrigger = pPar->ulWDogTrigger;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * 185 - WD_GetTrigger
 *
 */
void WD_GetTrigger(STDLIBFUNCALL)
{
	DEC_PARA(S_WD_GetTrigger);

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);

	if (pPar->wRet == OK)
	{
		STaskInfoVMM *pVMM = (STaskInfoVMM *)pVM->pShared->pVMM;

		*pPar->ulpWDogTrigger = pVMM->ppVM[uTask]->Task.ulWDogTrigger;
		*pPar->ulpWDogCounter = pVMM->ppVM[uTask]->Task.ulWDogCounter;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * 186 - TSK_GetStatistic
 *
 */
void TSK_GetStatistic(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_GetStatistic);

  #if ! defined(RTS_CFG_TASKSTAT)
	pPar->wRet = ERR_NOT_SUPPORTED;
	RETURN_v(pPar->wRet);
  #else

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);

	if (pPar->wRet == OK)
	{
		STaskInfoVMM *pVMM = (STaskInfoVMM *)pVM->pShared->pVMM;

		pPar->pStat->ulTaskState	= pVMM->ppVM[uTask]->Local.pState->ulState;
		pPar->pStat->ulCycleTime	= pVMM->ppVM[uTask]->Task.ulPara1;
		pPar->pStat->ulExeMax		= pVMM->ppVM[uTask]->Stat.ulExecMax;
		pPar->pStat->ulExeMin		= pVMM->ppVM[uTask]->Stat.ulExecMin;
		pPar->pStat->ulExeAver		= pVMM->ppVM[uTask]->Stat.ulExecAver;
		pPar->pStat->ulExeCount 	= pVMM->ppVM[uTask]->Stat.ulExecCounter;
		pPar->pStat->ulWDCounter	= pVMM->ppVM[uTask]->Task.ulWDogCounter;
		pPar->pStat->ulWDTrigger	= pVMM->ppVM[uTask]->Task.ulWDogTrigger;
	}

  #endif
}

/* ---------------------------------------------------------------------------- */
/**
 * 187 - TSK_ClearStatistic
 *
 */
void TSK_ClearStatistic(STDLIBFUNCALL)
{
	DEC_PARA(S_TSK_ClearStatistic);

  #if ! defined(RTS_CFG_TASKSTAT)
	pPar->wRet = ERR_NOT_SUPPORTED;
	RETURN_v(pPar->wRet);
  #else

	IEC_UINT uTask;

	pPar->wRet = libGetTaskNo(pVM, pPar->sTask, &uTask);

	if (pPar->wRet == OK)
	{
		STaskInfoVMM *pVMM = (STaskInfoVMM *)pVM->pShared->pVMM;

		OS_MEMSET(&pVMM->ppVM[uTask]->Stat, 0x00, sizeof(SStatistic));
		pVMM->ppVM[uTask]->Stat.ulExecMin = 0xfffffffful;
	}

  #endif

}

#endif	/* RTS_CFG_SYSTEM_LIB_NT */

/* ---------------------------------------------------------------------------- */
