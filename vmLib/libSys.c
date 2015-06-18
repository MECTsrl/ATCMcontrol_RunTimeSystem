
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
 * Filename: libSys.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"libSys.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "libSys.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

extern EXECUTE_FUN	g_pLibraryFun[];
extern EXECUTE_FB	g_pLibraryFB [];

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


#if defined(RTS_CFG_SYSTEM_LIB)

/* ---------------------------------------------------------------------------- */
/**
 * ClearTaskErrno
 *
 */
void ClearTaskErrno(STDLIBFUNCALL)
{
	pVM->Local.pState->ulErrNo = OK;
}

/* ---------------------------------------------------------------------------- */
/**
 * GetLocalTaskErrno
 *
 */
void GetLocalTaskErrno(STDLIBFUNCALL)
{
	IEC_UINT	uTask;

	SGetLocalTaskErrno OS_SPTR *pPar = (SGetLocalTaskErrno OS_SPTR *)pIN;

	if (libGetTaskNo(pVM, pPar->strTask, &uTask) == OK)
	{
		pPar->lRet = ((STaskInfoVMM *)pVM->pShared->pVMM)->ppVM[uTask]->Local.pState->ulErrNo;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * GetLocalTaskState
 *
 */
void GetLocalTaskState(STDLIBFUNCALL)
{
	IEC_UINT	uTask;

	SGetLocalTaskState OS_SPTR *pPar = (SGetLocalTaskState OS_SPTR *)pIN;

	if (libGetTaskNo(pVM, pPar->strTask, &uTask) == OK)
	{
		pPar->lRet = ((STaskInfoVMM *)pVM->pShared->pVMM)->ppVM[uTask]->Local.pState->ulState;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * GetTaskErrno
 *
 */
void GetTaskErrno(STDLIBFUNCALL)
{
	SGetTaskErrno OS_SPTR *pPar = (SGetTaskErrno OS_SPTR *)pIN;

	pPar->lRet = pVM->Local.pState->ulErrNo;
}

/* ---------------------------------------------------------------------------- */
/**
 * GetTimeSinceSystemBoot
 *
 */
void GetTimeSinceSystemBoot(STDLIBFUNCALL)
{
	SGetTimeSinceSystemBoot OS_SPTR *pPar = (SGetTimeSinceSystemBoot OS_SPTR *)pIN;

	pPar->tRet = osGetTime32();
}

/* ---------------------------------------------------------------------------- */
/**
 * OutputDebugString
 *
 */
void OutputDebugString(STDLIBFUNCALL)
{
	SOutputDebugString OS_SPTR *pPar = (SOutputDebugString OS_SPTR *)pIN;

	vmmQueueMessage(&pVM->pShared->MsgQueue, utilIecToAnsi(pPar->strMsg, pVM->Local.pBuffer));
}

/* ---------------------------------------------------------------------------- */
/**
 * SignalError
 *
 */
void SignalError(STDLIBFBCALL)
{
	SSignalError OS_SPTR *pPar = (SSignalError OS_SPTR *)pIN;

	if (pPar->bEnable & 0x1)
	{
		libSetError(pVM, (IEC_UINT)pPar->lErrNo);
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * StartLocalTask
 *
 */
void StartLocalTask(STDLIBFUNCALL)
{
	IEC_UINT uTask;

	SStartLocalTask OS_SPTR *pPar = (SStartLocalTask OS_SPTR *)pIN;

	if (libGetTaskNo(pVM, pPar->strTask, &uTask) == OK)
	{
		vmmStartTask((STaskInfoVMM *)pVM->pShared->pVMM, uTask);
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * StopLocalTask
 *
 */
void StopLocalTask(STDLIBFUNCALL)
{
	IEC_UINT uTask;

	SStopLocalTask OS_SPTR *pPar = (SStopLocalTask OS_SPTR *)pIN;

	if (libGetTaskNo(pVM, pPar->strTask, &uTask) == OK)
	{
		vmmStopTask((STaskInfoVMM *)pVM->pShared->pVMM, uTask, FALSE);
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * ClearLocalTaskErrno
 *
 */
void ClearLocalTaskErrno(STDLIBFUNCALL)
{
	IEC_UINT uTask;
	
	SClearLocalTaskErrno OS_SPTR *pPar = (SClearLocalTaskErrno OS_SPTR *)pIN;

	if (libGetTaskNo(pVM, pPar->strTask, &uTask) == OK)
	{
		((STaskInfoVMM *)pVM->pShared->pVMM)->ppVM[uTask]->Local.pState->ulErrNo = OK;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * ThrowException
 *
 */
void ThrowException(STDLIBFUNCALL)
{
	libSetException(pVM, EXCEPT_USER_EXCEPTION);
}

/* ---------------------------------------------------------------------------- */
/**
 * GetLocalTaskInfo
 *
 */
void GetLocalTaskInfo(STDLIBFUNCALL)
{
	IEC_UINT uTask;

	SGetLocalTaskInfo OS_SPTR *pPar = (SGetLocalTaskInfo OS_SPTR *)pIN;

	if (libGetTaskNo(pVM, pPar->strTask, &uTask) == OK)
	{
		STaskState OS_DPTR *pState = ((STaskInfoVMM *)pVM->pShared->pVMM)->ppVM[uTask]->Local.pState;

		pPar->pInfo->lState 	= pState->ulState;
		pPar->pInfo->lPriority	= pState->ulPrio;
		pPar->pInfo->tCycle 	= pState->ulCycle;
		pPar->pInfo->tETMax 	= pState->ulETMax;
		pPar->pInfo->lErrno 	= pState->ulErrNo;
	}
}


#endif	/* RTS_CFG_SYSTEM_LIB */

/* ---------------------------------------------------------------------------- */
