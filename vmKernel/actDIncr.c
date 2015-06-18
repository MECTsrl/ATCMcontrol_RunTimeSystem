
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
 * Filename: actDIncr.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"actDIncr.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */

#if defined(RTS_CFG_ONLINE_CHANGE)

/* ---------------------------------------------------------------------------- */
/**
 * cmdOCBegin - CMD_OC_BEGIN
 */
IEC_UINT cmdOCBegin(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uCount 	= 0;
	IEC_UINT uRes		= OK;

	SDLBuffer	*pDL	= &pVMM->DLB;
	SOnlChg 	*pOC	= &pVMM->OnlChg;
	
	if (pBlock == 0)
	{				
		actDLBegin(pVMM, NULL, DOWN_MODE_INCR);

		actGetOCList(NULL, NULL, NULL, 0, NULL);

		RETURN(OK);
	}
	

	/* Is Online Change possible?
	 * ------------------------------------------------------------------------
	 */
	if (pVMM->bProjLoaded == FALSE)
	{
		RETURN(ERR_NO_PROJECT);
	}

	if (pVMM->bProjActive == FALSE)
	{
		RETURN(ERR_NO_PROJECT);
	}

	if (pVMM->pLogin[pBlock->usSource] == FALSE)
	{
		RETURN(ERR_LOGIN);
	}


	/* Prepare & Notify Online Change helper task
	 * ------------------------------------------------------------------------
	 */
	if (pBlock->uBlock == 1)
	{
		uRes = actDLBegin(pVMM, pBlock, DOWN_MODE_INCR);
		if (uRes != OK)
		{
			RETURN(OK);
		}

	  #if defined(RTS_CFG_VM_IPC)
		{
			/* Notify Online Change helper task
			 */
			uRes = msgTXCommand(MSG_OC_PREPARE, Q_LIST_OC, Q_RESP_VMM_OC, VMM_TO_IPC_MSG, TRUE);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
		}
	  #endif

		pDL->bOCStarted = TRUE; 	/* <-- point of no return, clean up necessary */

	  #if defined(RTS_CFG_DEBUG_INFO)
		dbiFinalize(pVMM);
	  #endif

	} /* if (pBlock->uBlock == 1) */


	/* Get Online Change data
	 * ------------------------------------------------------------------------
	 */
	if (pBlock->uBlock == 1)
	{
		/* Online Change configuration
		 */
		uRes = actGetOCConfig(pDL, pBlock, &uCount, pOC, &pVMM->Project);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	if (uCount < pBlock->uLen)
	{
		/* Copy list
		 */
		uRes = actGetOCList(pDL, pBlock, &uCount, pOC->uToCopy, pOC->pCopy);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}


	/* Check result
	 * ------------------------------------------------------------------------
	 */
	if (uCount != pBlock->uLen)
	{
		/* We should have consumed the whole data block
		 */
		RETURN(ERR_INVALID_DATA);
	}

	if (pBlock->byLast == TRUE && pDL->bDone == FALSE)
	{
		/* All configurations downloaded?
		 */
		RETURN(ERR_ENTRIES_MISSING);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resOCBegin - CMD_OC_BEGIN
 */
IEC_UINT resOCBegin(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->DLB.uDomain = OCHG_BEGIN;
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdOCConfig - CMD_OC_CONFIG
 */
IEC_UINT cmdOCConfig(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = actDLConfig(pVMM, pBlock, DOWN_MODE_INCR, OCHG_BEGIN);
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resOCConfig - CMD_OC_CONFIG
 */
IEC_UINT resOCConfig(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->DLB.uDomain = OCHG_CONFIG;
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdOCCode - CMD_OC_CODE
 */
IEC_UINT cmdOCCode(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes;
	SOnlChg  *pOC = &pVMM->OnlChg;

	uRes = actDLCode(pVMM, pBlock, DOWN_MODE_INCR, (IEC_UINT) (pOC->uTaskInfo ? OCHG_CONFIG : OCHG_BEGIN), pOC->uCode, pOC->iCodeDiff);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resOCCode - CMD_OC_CODE
 */
IEC_UINT resOCCode(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->DLB.uDomain = OCHG_CODE;
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdOCInitial - CMD_OC_INITIAL
 */
IEC_UINT cmdOCInitial(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes;
	SOnlChg  *pOC = &pVMM->OnlChg;
		
	uRes = actDLInit(pVMM, pBlock, DOWN_MODE_INCR, OCHG_CONFIG | OCHG_CODE | OCHG_BEGIN, pOC->uData, pOC->uDirVars, pOC->iDataDiff);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resOCInitial - CMD_OC_INITIAL
 */
IEC_UINT resOCInitial(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->DLB.uDomain = OCHG_INIT;
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdOCCustom - CMD_OC_CUSTOM
 */
#if defined(RTS_CFG_CUSTOM_DL)

IEC_UINT cmdOCCustom(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = actDLFile(pVMM, pBlock, DOWN_MODE_INCR, OCHG_CONFIG | OCHG_CODE | OCHG_BEGIN | OCHG_INIT, 
								osGetCustDownDir, VMM_DIR_CUSTOM, VMM_FILE_CUST_MAP);

	if (uRes != OK)
	{
		actCloseFile(&pVMM->DLB.FIL.hF1);
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resOCCustom - CMD_OC_CUSTOM
 */
IEC_UINT resOCCustom(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->DLB.uDomain = OCHG_CUSTOM;
	}

	RETURN(OK);
}

#endif /* RTS_CFG_CUSTOM_DL */


/* ---------------------------------------------------------------------------- */
/**
 * cmdOCDebug - CMD_OC_DEBUG
 */
#if defined(RTS_CFG_DEBUG_INFO)

IEC_UINT cmdOCDebug(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = actDLFile(pVMM, pBlock, DOWN_MODE_INCR, OCHG_CONFIG | OCHG_CODE | OCHG_BEGIN | OCHG_INIT | OCHG_CUSTOM,
								osGetDBIDir, VMM_DIR_DBI, NULL);

	if (uRes != OK)
	{
		actCloseFile(&pVMM->DLB.FIL.hF1);
		actCloseFile(&pVMM->DLB.FIL.hF2);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resOCDebug - CMD_OC_DEBUG
 */
IEC_UINT resOCDebug(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->DLB.uDomain = OCHG_DEBUG;
	}

	RETURN(OK);
}

#endif /* RTS_CFG_DEBUG_INFO */


/* ---------------------------------------------------------------------------- */
/**
 * cmdOCFinish - CMD_OC_FINISH
 */
IEC_UINT cmdOCFinish(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = actDLFinish(pVMM, pBlock, DOWN_MODE_INCR, OCHG_CONFIG | OCHG_CODE | OCHG_BEGIN | OCHG_INIT | OCHG_CUSTOM | OCHG_DEBUG);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resOCFinish - CMD_OC_FINISH
 */
IEC_UINT resOCFinish(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->bProjActive = FALSE;
		bpDeleteAllBreakpoints(pVMM);

		pVMM->DLB.uDomain = OCHG_FINISH;
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdOCEnd - CMD_OC_END
 */
IEC_UINT cmdOCEnd(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pVMM->DLB.uDomain != OCHG_COMMIT)
	{
		RETURN(ERR_INVALID_DOMAIN);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resOCEnd - CMD_OC_END
 */
IEC_UINT resOCEnd(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->bProjActive = TRUE;

		pVMM->DLB.uDomain = DOWN_NONE;
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdOCCommit - CMD_OC_COMMIT
 */
IEC_UINT cmdOCCommit(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SOnlChg  *pOC	= &pVMM->OnlChg;
	SProject *pProj = &pVMM->Project;
	
	IEC_UINT  uRes = OK;
	IEC_UINT  uTask;
	IEC_UDINT ulState;

	if (pBlock == NULL)
	{
		if (pOC->uRetry == 0)
		{
			TR_STATE("\r\n");
		}
		
		RETURN(OK);
	}

	if (pVMM->DLB.uDomain != OCHG_FINISH)
	{
		RETURN(ERR_INVALID_DOMAIN);
	}

	if (pVMM->pLogin[pBlock->usSource] == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	if (pBlock->uLen != sizeof(IEC_UINT))
	{
		RETURN(ERR_INVALID_DATA);
	}

	pOC->uRetry = *(IEC_UINT *)pBlock->CMD.pData;

	/* Disable project
	 */
	pVMM->bProjActive = FALSE;
	pVMM->bProjLoaded = FALSE;

	if (pOC->uRetry == 0)
	{
		pOC->ullTime = osGetTimeUS();

	  #if defined(RTS_CFG_OC_TRACE)
		osTrace("--- [ONLCHG] Download ready, stopping IOLayer.  (%ldms)\r\n", 0);
	  #endif
		
		/* Stop IO layer
		 */
	  #if defined(RTS_CFG_IO_LAYER)
		uRes = ioStartLayer(pVMM, FALSE, TRUE);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	  #endif

		/* Stop Retain Update Task
		 */
	  #if defined(RTS_CFG_EXT_RETAIN)
		uRes = msgTXCommand(MSG_RT_STOP, Q_LIST_RET, Q_RESP_VMM_RET, VMM_TO_IPC_MSG, TRUE);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	  #endif

	  #if defined(RTS_CFG_OC_TRACE)
		osTrace("--- [ONLCHG] Stopping IEC tasks.  (%ldms)\r\n", utilGetTimeDiffMS(pOC->ullTime));
	  #endif

		/* Stop IEC tasks
		 */
		for (uTask = 0; uTask < pVMM->Project.uTasks; uTask++)
		{	
			pOC->pState[uTask] = (IEC_BYTE)pVMM->ppVM[uTask]->Local.pState->ulState;

			uRes = vmmStopTask(pVMM, uTask, FALSE);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
		}
	}

	{	/* Ensure, that the following code is included only once! ------------- */

	  #if defined(RTS_CFG_VM_IPC) /* -------- */

		IEC_UINT uEnsureOnlyOnce;
	
		{
			uRes = msgTXCommand(MSG_OC_COMMIT, Q_LIST_OC, Q_RESP_VMM_OC, VMM_TO_IPC_MSG, (IEC_BOOL)(pOC->uRetry == 0));

			if (uRes == WRN_TIME_OUT)
			{
				/* Retry
				 */
				pOC->uRetry = (IEC_UINT)(pOC->uRetry + 1);
				RETURN(OK);
			}

			if (uRes != OK)
			{
				RETURN(uRes);
			}

			pOC->uRetry = 0;
		}

	  #endif /* RTS_CFG_VM_IPC */

	  #if ! defined(RTS_CFG_VM_IPC) /* ------ */

		IEC_UINT uEnsureOnlyOnce;

		{
			/* No possibility to check, if the tasks has really stopped!
			 */
			osSleep(100);

			pOC->uRetry = 0;

			ocApply(pVMM);
		}

	  #endif /* ! RTS_CFG_VM_IPC */

		uEnsureOnlyOnce = 0x4711u;
	
	} /* ---------------------------------------------------------------------- */

	/* Restart IO layer
	 */
  #if defined(RTS_CFG_IO_LAYER)
	uRes = ioStartLayer(pVMM, TRUE, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
  #endif

	/* Start Retain Update Task
	 */
  #if defined(RTS_CFG_EXT_RETAIN)
	uRes = msgTXCommand(MSG_RT_START, Q_LIST_RET, Q_RESP_VMM_RET, VMM_TO_IPC_MSG, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
  #endif

  #if defined(RTS_CFG_OC_TRACE)
	osTrace("--- [ONLCHG] IO layer started. (%ldms)\r\n", utilGetTimeDiffMS(pOC->ullTime));
  #endif

	/* Restart the tasks
	 */
	for (uTask = 0; uTask < pVMM->Project.uTasks; uTask++)
	{		
		ulState = pVMM->ppVM[uTask]->Local.pState->ulState;

	  #if defined(RTS_CFG_TASKSTAT)
		OS_MEMSET(&pVMM->ppVM[uTask]->Stat, 0x00, sizeof(SStatistic));
		pVMM->ppVM[uTask]->Stat.ulExecMin = 0xfffffffful;
	  #endif

		if (pOC->pState[uTask] == TASK_STATE_RUNNING && ulState != TASK_STATE_ERROR)
		{
			uRes = vmmStartTask(pVMM, uTask);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
		}
	}

  #if defined(RTS_CFG_EVENTS)
	vmmSetEvent(pVMM, EVT_ONLCHG_END);
  #endif

  #if defined(RTS_CFG_OC_TRACE)
	osTrace("--- [ONLCHG] IEC tasks started, resource running. (%ldms)\r\n", utilGetTimeDiffMS(pOC->ullTime));
  #endif


	/* Clean Up
	 * ------------------------------------------------------------------------
	 */
	OS_MEMSET(pOC->pCodeInd, 0x00, sizeof(IEC_UINT) * MAX_OC_CODE_OBJ);
	OS_MEMSET(pOC->pDataInd, 0x00, sizeof(IEC_UINT) * MAX_OC_DATA_OBJ);
	OS_MEMSET(pOC->pCopy,	 0x00, sizeof(SOCCopy)	* MAX_OC_COPY_REG);

	OS_MEMSET(pOC->ppDataAdr,0x00, sizeof(IEC_DATA OS_DPTR *) * MAX_OC_DATA_OBJ);

	pOC->iCodeDiff	= 0;
	pOC->iDataDiff	= 0;
	pOC->uCode		= 0;
	pOC->uData		= 0;
	pOC->uToCopy	= 0;
	pOC->uDirVars	= 0;

	OS_MEMSET(pVMM->Shared.pCode				+ pProj->uCode, 0x00, (MAX_CODE_OBJ 			   - pProj->uCode) * sizeof(SObject));
	OS_MEMSET(pVMM->Shared.pData + MAX_SEGMENTS + pProj->uData, 0x00, (MAX_DATA_OBJ - MAX_SEGMENTS - pProj->uData) * sizeof(SObject));

	pVMM->DLB.bOCStarted = FALSE;

	pVMM->bProjLoaded = TRUE;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resOCCommit - CMD_OC_COMMIT
 */
IEC_UINT resOCCommit(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SOnlChg *pOC = &pVMM->OnlChg;

	if (pBlock == 0)
	{
		RETURN(OK);
	}

	if (pOC->uRetry == 0)
	{
		pVMM->DLB.uDomain = OCHG_COMMIT;

		pOC->uCode	= 0;
		pOC->uData	= 0;
	}

	*(IEC_UINT *)pBlock->CMD.pData = pOC->uRetry;
	pBlock->uLen = sizeof(IEC_UINT);

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * actOCCleanUp
 */
IEC_UINT actOCCleanUp(STaskInfoVMM *pVMM)
{
	SOnlChg 	*pOC	 = &pVMM->OnlChg;

  #if defined(RTS_CFG_FFO)
	SIntShared	*pShared = &pVMM->Shared;
	SProject	*pProj	 = &pVMM->Project;

	IEC_UINT uCodeOff	 = (IEC_UINT)(LIST_OFF(pOC->iCodeDiff));
	IEC_UINT uDataOff	 = (IEC_UINT)(LIST_OFF(pOC->iDataDiff) + MAX_SEGMENTS);
  #endif

	pVMM->DLB.bOCStarted = FALSE;

	if (pVMM->bProjActive != FALSE)
	{
	  #if defined(RTS_CFG_FFO)

		IEC_UINT i;

		for (i = (IEC_UINT)(pProj->uCode + uCodeOff); i < pProj->uCode + uCodeOff + pOC->uCode; i++)
		{
			/* Free code objects
			 */
			if (pShared->pCode[i].pAdr != NULL)
			{
				IEC_UINT uRes = osFree(&pShared->pCode[i].pAdr);
				if (uRes != OK)
				{
					RETURN(uRes);
				}
			}
		}

		for (i = (IEC_UINT)(pProj->uData + uDataOff); i < pProj->uData + uDataOff + pOC->uData; i++)
		{
			/* Free data objects
			 */
			if (pShared->pData[i].pAdr != NULL && (pShared->pDataSeg[i] & MASC_ALLOCATED) != 0)
			{
				IEC_UINT uRes = osFree(&pShared->pData[i].pAdr);
				if (uRes != OK)
				{
					RETURN(uRes);
				}
			}
		}

	  #endif /* RTS_CFG_FFO */
	}

	OS_MEMSET(pOC->pCodeInd, 0x00, sizeof(IEC_UINT) * MAX_OC_CODE_OBJ);
	OS_MEMSET(pOC->pDataInd, 0x00, sizeof(IEC_UINT) * MAX_OC_DATA_OBJ);
	OS_MEMSET(pOC->pCopy,	 0x00, sizeof(SOCCopy)	* MAX_OC_COPY_REG);

	OS_MEMSET(pOC->ppDataAdr,0x00, sizeof(IEC_DATA OS_DPTR *) * MAX_OC_DATA_OBJ);

	pOC->iCodeDiff	= 0;
	pOC->iDataDiff	= 0;
	pOC->uCode		= 0;
	pOC->uData		= 0;
	pOC->uToCopy	= 0;
	pOC->uDirVars	= 0;
	
	pOC->uProjInfo	= 0;
	pOC->uTaskInfo	= 0;

	RETURN(OK);
}

#endif	/* RTS_CFG_ONLINE_CHANGE */

/* ---------------------------------------------------------------------------- */
