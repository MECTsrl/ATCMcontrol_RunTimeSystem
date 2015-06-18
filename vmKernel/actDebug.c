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
 * Filename: actDebug.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"actDebug.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

#if defined(RTS_CFG_IO_LAYER) || defined(RTS_CFG_WRITE_FLAGS_PI)
static IEC_UINT actIsInSegment(STaskInfoVMM *pVMM, IEC_UINT uData, IEC_UINT uSegment, IEC_BOOL *bpWithin, IEC_UDINT *ulpOffset);
#endif

/* ----  Implementations:	--------------------------------------------------- */

	
/* ---------------------------------------------------------------------------- */
/**
 * cmdGetState
 */
IEC_UINT cmdGetState(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UDINT	ulActStateReq = osGetTime32();
	IEC_UINT	i;
	
	if (ulActStateReq - pVMM->ulLastStateReq > VMM_DEBUG_INTERVAL)
	{
		for (i = 0; i < pVMM->Project.uTasks; i++)
		{
			bpDeleteAllBreakpoints(pVMM);
			vmmResetTask(pVMM, i);
		}
	}

	pVMM->ulLastStateReq = ulActStateReq;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resGetState
 */
IEC_UINT resGetState(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	STaskInfoVM 	*pVM;
	SMsgQueue		*pMsgQueue	= &pVMM->Shared.MsgQueue;
	SBPQueue		*pBPQueue	= &pVMM->Shared.BPQueue;

	XInfo		xInfo;

	IEC_UINT	i			= 0;
	IEC_UINT	uCount		= 0;
	IEC_UINT	uToCopy 	= 0;

	OS_MEMSET(&xInfo, 0x00, sizeof(XInfo));

	xInfo.ulResState = pVMM->ulResState;

	uCount += sizeof(XInfo);


	/* Breakpoint information
	 * ------------------------------------------------------------------------
	 */
	xInfo.uBPCount	= 0;
	
	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

	OS_BEGIN_CRITICAL_SECTION(SEM_BP_QUEUE)
	{
		/* Copy breakpoint notifications into send buffer
		 */

		while (uCount <= MAX_DATA && pBPQueue->uLast != pBPQueue->uNext)
		{
			XBPNotification *pxNotify;
			SBPNotification *pzNotify;

			if (uCount + sizeof(XBPNotification) <= (IEC_UINT)MAX_DATA)
			{
				xInfo.uBPCount++;
				
				pzNotify  = pBPQueue->pQueue + pBPQueue->uLast;
				pxNotify = (XBPNotification *)(pBlock->CMD.pData + uCount);

				pxNotify->uState		= pzNotify->uState;
				pxNotify->uTask 		= pzNotify->uTask;
				pxNotify->BP.uCode		= pzNotify->BP.uCode;
				pxNotify->BP.ulCodePos	= pzNotify->BP.uCodePos;
				pxNotify->BP.uInst		= pzNotify->BP.uData;

				uCount = (IEC_UINT)(uCount + sizeof(XBPNotification));
				
				pBPQueue->uLast = (IEC_UINT)((pBPQueue->uLast + 1) % MAX_BP_QUEUE);
			}
			else
			{
				/* Buffer full
				 */
				break;
			}
		
		} /* while (uCount <= MAX_DATA && pBPQueue->uLast != pBPQueue->uNext) */

	} OS_END_CRITICAL_SECTION(SEM_BP_QUEUE)
		
	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ------------------------ */

		
	/* Debug strings
	 * ------------------------------------------------------------------------
	 */
	 
	xInfo.uStrCount = 0;

	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

	OS_BEGIN_CRITICAL_SECTION(SEM_MSG_QUEUE)
	{
		while (uCount <= MAX_DATA && pMsgQueue->uLast != pMsgQueue->uNext)
		{
			uToCopy = (IEC_UINT) (OS_STRLEN(pMsgQueue->ppMessage[pMsgQueue->uLast]) + 1);

			if (uCount + uToCopy <= (IEC_UINT)MAX_DATA)
			{
				xInfo.uStrCount++;

				OS_MEMCPY(pBlock->CMD.pData + uCount, pMsgQueue->ppMessage[pMsgQueue->uLast], uToCopy);
				uCount = (IEC_UINT)(uCount + uToCopy);

			  #if defined(RTS_CFG_FFO)
				osFree((IEC_DATA **)&pMsgQueue->ppMessage[pMsgQueue->uLast]);
			  #else
				osFreeMsgStr(pMsgQueue->uLast);
			  #endif

				pMsgQueue->uLast = (IEC_UINT)((pMsgQueue->uLast + 1) % MAX_STR_MSG_QUEUE);
			}
			else
			{
				/* Buffer full
				 */
				break;
			}
		} /* while (uCount <= MAX_DATA && pMsgQueue->uLast != pMsgQueue->uNext) */

	} OS_END_CRITICAL_SECTION(SEM_MSG_QUEUE)

	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ------------------------ */


	/* Task error strings
	 * ------------------------------------------------------------------------
	 */
	for (i = 0; i < pVMM->Project.uTasks; i++)
	{
		pVM = pVMM->ppVM[i];

		if (pVM == 0 || pVMM->bProjLoaded == FALSE)
		{
			break;
		}

		if (pVM->usFlags & TASK_FLAG_NEW_ERROR)
		{
			IEC_CHAR szBuffer[2 * MAX_STR_MSG_LEN];

			vmmFormatException(&pVM->Local.Exception, pVM, pVM->Local.pState->ulErrNo, szBuffer);

			szBuffer[MAX_STR_MSG_LEN] = 0;
			uToCopy = (IEC_UINT)(OS_STRLEN(szBuffer) + 1);
						 
			if (uCount + uToCopy <= (IEC_UINT)MAX_DATA)
			{
				xInfo.uStrCount++;
				
				OS_MEMCPY(pBlock->CMD.pData + uCount, szBuffer, uToCopy);
				uCount = (IEC_UINT)(uCount + uToCopy);
						
				pVM->usFlags &= ~TASK_FLAG_NEW_ERROR;
			}
			else
			{
				/* Buffer full
				 */
				break;
			}
			 
		} /* if (bNew == TRUE) */

	} /* for (i = 0; i < pVMM->Project.uTasks; i++) */
	

	/* Info block
	 * ------------------------------------------------------------------------
	 */
	OS_MEMCPY(pBlock->CMD.pData, &xInfo, sizeof(XInfo));

	pBlock->uLen	= uCount;
	pBlock->byLast	= TRUE;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdLogin
 */
IEC_UINT cmdLogin(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uLen;

	/* Queue version info into target view.
	 */
	IEC_UINT uRes = sysGetVersionInfo((IEC_CHAR*)pVMM->pBuffer);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	uLen = (IEC_UINT)OS_STRLEN((IEC_CHAR*)pVMM->pBuffer);
	OS_STRCAT((IEC_CHAR*)pVMM->pBuffer, "\n");

	vmmQueueMessage(&pVMM->Shared.MsgQueue, "\n");
	vmmQueueMessage(&pVMM->Shared.MsgQueue, (IEC_CHAR*)pVMM->pBuffer);

	pVMM->pBuffer[0]	= '-';
	pVMM->pBuffer[uLen] = '\0';
	if (uLen != 0)
	{
		uLen--;
	}

	for ( ; uLen != 0; uLen--)
	{
		pVMM->pBuffer[uLen] = '-';
	}

	OS_STRCAT((IEC_CHAR*)pVMM->pBuffer, "\n");

	vmmQueueMessage(&pVMM->Shared.MsgQueue, (IEC_CHAR*)pVMM->pBuffer);

	if (pVMM->bProjLoaded == FALSE)
	{
		vmmQueueMessage(&pVMM->Shared.MsgQueue, "No project loaded.\n");
		vmmQueueMessage(&pVMM->Shared.MsgQueue, "\n");
		return ERR_NO_PROJECT;
	}

  #if defined(RTS_CFG_EXT_PROJ_INFO) && defined(RTS_CFG_PRJ_TRACE)
	vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR *)pVMM->pBuffer,
		"Project:  %s\n", pVMM->ProjInfo.szProjName));
	vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR *)pVMM->pBuffer,
		"Modified: %s (%s)\n", pVMM->ProjInfo.szModified, pVMM->ProjInfo.szOwner));
  #endif
	vmmQueueMessage(&pVMM->Shared.MsgQueue, "\n");

	if (pBlock->uLen != VMM_GUID)
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	TRACE_GUID("Login", "PRJ", pVMM->pProjectGUID);
	TRACE_GUID("Login", "NEW", pBlock->CMD.pData);

	if (OS_MEMCMP(pVMM->pProjectGUID, pBlock->CMD.pData, VMM_GUID) != 0)
	{
		/* Don't trace a error message at this point. This case occurs
		 * quite often!
		 */
		return ERR_WRONG_PROJECT;
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resLogin
 */
IEC_UINT resLogin(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	pVMM->pLogin[pBlock->usSource] = TRUE;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdLogout
 */
IEC_UINT cmdLogout(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resLogout
 */
IEC_UINT resLogout(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	pVMM->pLogin[pBlock->usSource] = FALSE;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdStart
 */
IEC_UINT cmdStart(STaskInfoVMM *pVMM, XBlock *pBlock)
{	

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resStart
 */
IEC_UINT resStart(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock->CMD.byCommand == CMD_WARM_START)
	{
		pVMM->bExecWarmStart = TRUE;

	  #if defined(RTS_CFG_EVENTS)
		vmmSetEvent(pVMM, EVT_BEFORE_WARMSTART);
		osSleep(50);
	  #endif
	}
	else
	{
		pVMM->bExecColdStart = TRUE;

	  #if defined(RTS_CFG_EVENTS)
		vmmSetEvent(pVMM, EVT_BEFORE_COLDSTART);
		osSleep(50);
	  #endif
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdResource
 */
IEC_UINT cmdResource(STaskInfoVMM *pVMM, XBlock *pBlock)
{	
	if (pVMM->bProjActive == FALSE)
	{
		RETURN(ERR_NO_PROJECT);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resResource
 */
IEC_UINT resResource(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uTask;

  #if defined(RTS_CFG_EVENTS)
	vmmSetEvent(pVMM, (IEC_UINT)(pBlock->CMD.byCommand == CMD_START_RESOURCE ? EVT_START_RESOURCE : EVT_STOP_RESOURCE));
	osSleep(50);
  #endif

	for (uTask = 0; uTask < pVMM->Project.uTasks; uTask++)
	{		
		if (pBlock->CMD.byCommand == CMD_START_RESOURCE)
		{
			vmmStartTask(pVMM, uTask);
		}
		else
		{
			vmmStopTask(pVMM, uTask, TRUE);
		}
	}

	pVMM->ulResState = pBlock->CMD.byCommand == CMD_START_RESOURCE ? RES_STATE_RUNNING : RES_STATE_PAUSED;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdAllTasks
 */
IEC_UINT cmdAllTasks(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pVMM->bProjActive == FALSE)
	{
		RETURN(ERR_NO_PROJECT);
	}

	if (pVMM->pLogin[pBlock->usSource] == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resAllTasks
 */
IEC_UINT resAllTasks(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uTask;

  #if defined(RTS_CFG_EVENTS)
	vmmSetEvent(pVMM, (IEC_UINT)(pBlock->CMD.byCommand == CMD_START_ALL_TASKS ? EVT_START_TASK : EVT_STOP_TASK));
	osSleep(50);
  #endif

	for (uTask = 0; uTask < pVMM->Project.uTasks; uTask++)
	{
		if (pBlock->CMD.byCommand == CMD_START_ALL_TASKS)
		{
			vmmStartTask(pVMM, uTask);
		}
		else
		{
			vmmStopTask(pVMM, uTask, TRUE);
		}
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdTask
 */
IEC_UINT cmdTask(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pVMM->bProjActive == FALSE)
	{
		RETURN(ERR_NO_PROJECT);
	}
	if (pVMM->pLogin[pBlock->usSource] == FALSE)
	{
		RETURN(ERR_LOGIN);
	}
	if (pBlock->uLen != sizeof(IEC_UINT))
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	pVMM->DLB.TSK.uTask = *(IEC_UINT *)pBlock->CMD.pData;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resTask
 */
IEC_UINT resTask(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes;
	
  #if defined(RTS_CFG_EVENTS)
	vmmSetEvent(pVMM, (IEC_UINT)(pBlock->CMD.byCommand == CMD_START_TASK ? EVT_START_TASK : EVT_STOP_TASK));
	osSleep(50);
  #endif

	if (pBlock->CMD.byCommand == CMD_START_TASK)
	{
		uRes = vmmStartTask(pVMM, pVMM->DLB.TSK.uTask);
	}
	else
	{
		uRes = vmmStopTask(pVMM, pVMM->DLB.TSK.uTask, TRUE);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdOpenDebugSession
 */
IEC_UINT cmdOpenDebugSession(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	/* $TODO$
	if (pVMM->usDebugMode != NO_DEBUG_CONN && pVMM->usDebugMode != pBlock->usSource)
	{
		RETURN(ERR_ALREADY_IN_DEBUG);
	}
	*/

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resOpenDebugSession
 */
IEC_UINT resOpenDebugSession(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	pVMM->bDebugMode = TRUE;
	/* pVMM->usDebugMode = pBlock->usSource; */
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdCloseDebugSession
 */
IEC_UINT cmdCloseDebugSession(STaskInfoVMM *pVMM, XBlock *pBlock)
{

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resCloseDebugSession
 */
IEC_UINT resCloseDebugSession(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uTask;

	pVMM->bDebugMode = FALSE;
	/* pVMM->usDebugMode = NO_DEBUG_CONN; */
	bpDeleteAllBreakpoints(pVMM);

	/* Stop all tasks in BREAKPOINT
	 */
	for (uTask = 0; uTask < pVMM->Project.uTasks; uTask++)
	{
		vmmResetTask(pVMM, uTask);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdSetBreakpoint
 */
IEC_UINT cmdSetBreakpoint(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SBreakpoint *pBP	= &pVMM->DLB.BRK.BP;
	XBreakpoint *pXBP	= (XBreakpoint *)pBlock->CMD.pData;

	if (pVMM->bProjLoaded == FALSE)
	{
		RETURN(ERR_NO_PROJECT);
	}
	if (pVMM->bDebugMode == FALSE)
	{
		RETURN(ERR_DEBUG_MODE);
	}
	if (pBlock->uLen != sizeof(XBreakpoint))
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	pBP->uCode	  = pXBP->uCode;
	pBP->uData	  = pXBP->uInst;
	pBP->uCodePos = (IEC_UINT)pXBP->ulCodePos;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resSetBreakpoint
 */
IEC_UINT resSetBreakpoint(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes= bpAddBreakpoint(pVMM, &pVMM->DLB.BRK.BP, (IEC_UDINT *)pBlock->CMD.pData);

	pBlock->uLen = (IEC_UINT)(uRes == OK ? sizeof(IEC_UDINT) : 0);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdClearBreakpoint
 */
IEC_UINT cmdClearBreakpoint(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pVMM->bProjLoaded == FALSE)
	{
		RETURN(ERR_NO_PROJECT);
	}
	if (pVMM->bDebugMode == FALSE)
	{
		RETURN(ERR_DEBUG_MODE);
	}
	if (pBlock->uLen != sizeof(IEC_UDINT))
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	// pVMM->DLB.BRK.ulBPId = *(IEC_UDINT UNALIGNED *)pBlock->CMD.pData;
	OS_MEMCPY(&pVMM->DLB.BRK.ulBPId, pBlock->CMD.pData, sizeof(IEC_UDINT));

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resClearBreakpoint
 */
IEC_UINT resClearBreakpoint(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = bpDeleteBreakpoint (pVMM, pVMM->DLB.BRK.ulBPId);
	
	pBlock->uLen = (IEC_UINT) (uRes == OK ? sizeof(IEC_UDINT) : 0);

	OS_MEMCPY(pBlock->CMD.pData, &pVMM->DLB.BRK.ulBPId, pBlock->uLen);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdClearAllBreakpoints
 */
IEC_UINT cmdClearAllBreakpoints(STaskInfoVMM *pVMM, XBlock *pBlock)
{

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resClearAllBreakpoints
 */
IEC_UINT resClearAllBreakpoints(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	if (pVMM->bProjLoaded == TRUE && pVMM->bDebugMode == TRUE)
	{
		uRes = bpDeleteAllBreakpoints(pVMM);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdContinue
 */
IEC_UINT cmdContinue(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pVMM->bProjLoaded == FALSE)
	{
		RETURN(ERR_NO_PROJECT);
	}
	if (pVMM->bDebugMode == FALSE)
	{
		RETURN(ERR_DEBUG_MODE);
	}
	if (pBlock->uLen != sizeof(IEC_UINT))
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	pVMM->DLB.BRK.uTask = *(IEC_UINT *)pBlock->CMD.pData;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resContinue
 */
IEC_UINT resContinue(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes;

	if (pBlock->CMD.byCommand == CMD_CONTINUE)
	{
		uRes = vmmContinueTask(pVMM, pVMM->DLB.BRK.uTask);
	}
	else
	{
		uRes = vmmSingleStep(pVMM, pVMM->DLB.BRK.uTask);
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdGetValue
 */
IEC_UINT cmdGetValue(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SDLGetVal *pGET = &pVMM->DLB.GET;

	if (pVMM->bProjLoaded == FALSE)
	{
		return ERR_NO_PROJECT;
	}
	if (pVMM->pLogin[pBlock->usSource] == FALSE)
	{
		return ERR_LOGIN;
	}

	OS_MEMCPY(pGET->pRecv, pBlock->CMD.pData, pBlock->uLen);
	
	pGET->uRecv = pBlock->uLen;
	pGET->uDone = 0;
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resGetValue
 */
IEC_UINT resGetValue(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SDLGetVal			*pGET = &pVMM->DLB.GET;
	IEC_DATA OS_DPTR	*p;

	IEC_UDINT	ulOffset;
	IEC_UINT	uLen;
	IEC_UINT	uSegment;
	IEC_USINT	usBit;
	IEC_UINT	uSizeOfVar;

	IEC_UINT	uRes	= OK;

  #if defined(RTS_CFG_IO_LAYER)
	IEC_BOOL	bOut		= FALSE;
	IEC_BOOL	bIn 		= FALSE;
	IEC_UDINT	ulOutOffset = 0;
	IEC_UDINT	ulInOffset	= 0;
  #endif

	pBlock->byLast	= TRUE;

	while (pGET->uDone < pGET->uRecv)
	{
		SObject *pData = 0;

		IEC_USINT usType = (IEC_USINT)(*(IEC_USINT *)(pGET->pRecv + pGET->uDone) & VMM_XV_TYPEMASK);

		switch(usType)
		{
			case VMM_XV_SMALL:
			{
				XVariableS *pxVar = (XVariableS *)(pGET->pRecv + pGET->uDone); 
				
				ulOffset = pxVar->usOffset;
				uLen	 = (IEC_UINT)(pxVar->usLen == 0 ? 1u : pxVar->usLen);
				uSegment = pxVar->usSegment;
				usBit	 = (IEC_USINT)(pxVar->usType & VMM_XV_BITMASK);
				
				uSizeOfVar = sizeof(XVariableS);

				break;
			}

			case VMM_XV_MEDIUM:
			{
				XVariableM *pxVar = (XVariableM *)(pGET->pRecv + pGET->uDone); 

				ulOffset = pxVar->uOffset;
				uLen	 = (IEC_UINT)(pxVar->uLen == 0 ? 1 : pxVar->uLen);
				uSegment = pxVar->usSegment;
				usBit	 = (IEC_USINT)(pxVar->usType & VMM_XV_BITMASK);
				
				uSizeOfVar = sizeof(XVariableM);

				break;
			}

			case VMM_XV_LARGE:
			{
				XVariableL *pxVar = (XVariableL *)(pGET->pRecv + pGET->uDone); 

				ulOffset = pxVar->ulOffset;
				uLen	 = (IEC_UINT)(pxVar->uLen == 0 ? 1 : pxVar->uLen);
				uSegment = pxVar->uSegment;
				usBit	 = (IEC_USINT)(pxVar->usType & VMM_XV_BITMASK);

				uSizeOfVar = sizeof(XVariableL);

				break;
			}

			default:
			{
				RETURN(ERR_INVALID_DATA_SIZE);
			}
		
		}	/* switch(usType) */

		/* Check variable
		 */
		if (uSegment >= pVMM->Project.uData + MAX_SEGMENTS)
		{
			RETURN(ERR_INVALID_INSTANCE);
		}

		pData = &pVMM->Shared.pData[uSegment];

		if (ulOffset + uLen > pData->ulSize)
		{
			RETURN(ERR_INVALID_OFFSET);
		}

		if (pBlock->uLen + uLen > (IEC_UINT)MAX_DATA)
		{
			/* Buffer full, we need another frame
			 */
			pBlock->byLast = FALSE;
			break;
		}

	  #if defined(RTS_CFG_IO_LAYER)
		uRes = actIsInSegment(pVMM, uSegment, SEG_OUTPUT, &bOut, &ulOutOffset);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
		uRes = actIsInSegment(pVMM, uSegment, SEG_INPUT,  &bIn,  &ulInOffset);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	  #endif

		/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  ---------------- */

	  #if defined(RTS_CFG_TASK_IMAGE)
		OS_BEGIN_CRITICAL_SECTION(SEM_TASK_IMAGE)
	  #endif  

		{	/* ---------------------------------------------------------------- */
			
			uRes = osNotifyGetValue(pVMM, OS_ADDPTR(pBlock->CMD.pData, pBlock->uLen), uSegment, ulOffset, uLen, usBit);
			TR_RET(uRes);

		  #if defined(RTS_CFG_IO_LAYER)
			if (uRes == OK && bOut == TRUE)
			{
				uRes = ioNotifyLayer(pVMM, NULL, FALSE, SEG_OUTPUT, ulOutOffset + ulOffset, uLen, usBit);
				TR_RET(uRes);
			}
			if (uRes == OK && bIn == TRUE)
			{
				uRes = ioNotifyLayer(pVMM, NULL, FALSE, SEG_INPUT,	ulInOffset	+ ulOffset, uLen, usBit);
				TR_RET(uRes);
			}
		  #endif

			if (uRes == OK)
			{
				p = (IEC_DATA OS_DPTR *)OS_ADDPTR(pData->pAdr, ulOffset);
				p = OS_NORMALIZEPTR(p);

				if (usBit == 0)
				{
					OS_MEMCPY(pBlock->CMD.pData + pBlock->uLen, p, uLen);
				}
				else
				{
					*(pBlock->CMD.pData + pBlock->uLen) = (IEC_DATA) ( *p & (1u << (usBit - 1u)) );

				}
			}

			if (uRes == WRN_HANDLED)
			{
				uRes = OK;
			}

		}	/* ---------------------------------------------------------------- */
					
	  #if defined(RTS_CFG_TASK_IMAGE)
		OS_END_CRITICAL_SECTION(SEM_TASK_IMAGE)
	  #endif

		/* <<<	C R I T I C A L   S E C T I O N  -	E N D  -------------------- */

		if (uRes != OK)
		{
			RETURN(uRes);
		}
		
		pBlock->uLen = (IEC_UINT)(pBlock->uLen + uLen); 	
		pGET->uDone  = (IEC_UINT)(pGET->uDone  + uSizeOfVar);
	
	}	/* while (pGET->uDone < pGET->uRecv) */
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdSetValue
 */
IEC_UINT cmdSetValue(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_DATA OS_DPTR	*p;
  #if defined(RTS_CFG_WRITE_FLAGS_PI)
	IEC_DATA OS_DPTR	*w;
  #endif
	
	IEC_UINT	uCount	= 0;
	IEC_UINT	uRes	= OK;

  #if defined(RTS_CFG_WRITE_FLAGS_PI)
	SObject 	*pWF	= &pVMM->Shared.WriteFlags;
  #endif

  #if defined(RTS_CFG_IO_LAYER) || defined(RTS_CFG_WRITE_FLAGS_PI)
	IEC_BOOL	bOut		= FALSE;
	IEC_BOOL	bIn 		= FALSE;
	IEC_UDINT	ulOutOffset = 0;
	IEC_UDINT	ulInOffset	= 0;
  #endif
	
	if (pVMM->bProjLoaded == FALSE)
	{
		RETURN(ERR_NO_PROJECT);
	}
	if (pVMM->pLogin[pBlock->usSource] == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	while (uCount < pBlock->uLen)
	{
		XValue		*pxVal = (XValue *)(pBlock->CMD.pData + uCount);
		XVariable	*pxVar = &pxVal->VAR;
		SObject 	*pData = pVMM->Shared.pData + pxVar->uSegment;

		/* Check variable
		 */
		if (pxVar->uSegment >= pVMM->Project.uData + MAX_SEGMENTS)
		{
			RETURN(ERR_INVALID_INSTANCE);
		}
		if (pxVar->ulOffset + pxVar->uLen > pData->ulSize)
		{
			RETURN(ERR_INVALID_OFFSET);
		}

		if (uCount + pxVar->uLen > MAX_DATA)
		{
			RETURN(ERR_INVALID_DATA);
		}

	  #if defined(RTS_CFG_IO_LAYER) || defined(RTS_CFG_WRITE_FLAGS_PI)
		uRes = actIsInSegment(pVMM, pxVar->uSegment, SEG_OUTPUT, &bOut, &ulOutOffset);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
		uRes = actIsInSegment(pVMM, pxVar->uSegment, SEG_INPUT,  &bIn,	&ulInOffset);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	  #endif

		/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  ---------------- */

	  #if defined(RTS_CFG_TASK_IMAGE)
		OS_BEGIN_CRITICAL_SECTION(SEM_TASK_IMAGE)
	  #endif  

		{	/* ---------------------------------------------------------------- */

			uRes = osNotifySetValue(pVMM, pxVal->pValue, pxVar->uSegment, pxVar->ulOffset, pxVar->uLen, pxVar->byBit);
			TR_RET(uRes);

			if (uRes == OK)
			{
				p = (IEC_DATA OS_DPTR *)OS_ADDPTR(pData->pAdr, pxVar->ulOffset);
				p = OS_NORMALIZEPTR(p);
				
				if (pxVar->byBit == 0)
				{
					OS_MEMCPY(p, pxVal->pValue, pxVar->uLen);
				}
				else
				{
					*p = (IEC_DATA) ((*p & ~(1u << (pxVar->byBit - 1u))) | (*pxVal->pValue << (pxVar->byBit - 1u))); 
				}
				
			  #if defined(RTS_CFG_WRITE_FLAGS_PI)
				if (bOut == TRUE)
				{
					w = (IEC_DATA OS_DPTR *)OS_ADDPTR(pWF->pAdr, (ulOutOffset + pxVar->ulOffset));
					w = OS_NORMALIZEPTR(w);

					if (pxVar->byBit == 0)
					{
						OS_MEMSET(w, 0xff, pxVar->uLen);
					}
					else
					{
						*w = (IEC_DATA) (*w | (1u << (pxVar->byBit - 1u))); 
					}
				}
			  #endif				
			
			} /* if (uRes == OK) */
			
		  #if defined(RTS_CFG_IO_LAYER)
			if (uRes == OK && bOut == TRUE)
			{
				uRes = ioNotifyLayer(pVMM, NULL, TRUE, SEG_OUTPUT, ulOutOffset + pxVar->ulOffset, pxVar->uLen, pxVar->byBit);
				TR_RET(uRes);
			}
			if (uRes == OK && bIn == TRUE)
			{
				uRes = ioNotifyLayer(pVMM, NULL, TRUE, SEG_INPUT,  ulInOffset  + pxVar->ulOffset, pxVar->uLen, pxVar->byBit);
				TR_RET(uRes);
			}
		  #endif
				
			if (uRes == WRN_HANDLED)
			{
				uRes = OK;
			}

		}	/* ---------------------------------------------------------------- */

	  #if defined(RTS_CFG_TASK_IMAGE)
		OS_END_CRITICAL_SECTION(SEM_TASK_IMAGE)
	  #endif

		/* <<<	C R I T I C A L   S E C T I O N  -	E N D  -------------------- */

		if (uRes != OK)
		{
			RETURN(uRes);
		}
		
	  #if defined(RTS_CFG_COPY_DOMAIN)
		{
			IEC_UDINT	ulOffDst;
			if(vmmFindVarInCpyList(pVMM, pxVar, &ulOffDst)) 
			{
				p = OS_ADDPTR(pVMM->Shared.pData[SEG_RETAIN].pAdr, ulOffDst);
				p = OS_NORMALIZEPTR(p);

				if (pxVar->byBit == 0) 
				{
					OS_MEMCPY(p, pxVal->pValue, pxVar->uLen);
				} 
				else 
				{
					*p = (IEC_DATA) ((*p & ~(1u << (pxVar->byBit - 1u))) | (*pxVal->pValue << (pxVar->byBit - 1u))); 
				}
			}
		} 
	  #endif /* RTS_CFG_COPY_DOMAIN */

		uCount = (IEC_UINT)(uCount + HD_VALUE + pxVar->uLen);
	
	} /* while (uCount < pBlock->uLen) */
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resSetValue
 */
IEC_UINT resSetValue(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	pBlock->byLast	= TRUE;

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdInitialize
 */
IEC_UINT cmdInitialize(STaskInfoVMM *pVMM, XBlock *pBlock)
{	
  #if defined(RTS_CFG_IO_LAYER)
	IEC_BOOL bBoot;
	IEC_UINT uRes = ioIsBootable(pVMM, &bBoot);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (bBoot == FALSE)
	{
		vmmQueueMessage(&pVMM->Shared.MsgQueue, 
			"[VMM]: < WARNING >  Initialize/Clear currently not possible. IO layer is still starting up!\n");
		RETURN(ERR_NOT_READY);
	}
  #endif /* RTS_CFG_IO_LAYER */

	if (pBlock->uLen != sizeof(IEC_UINT))
	{
		RETURN(ERR_INVALID_DATA);
	}

	pVMM->DLB.CIC.uRetry = *(IEC_UINT *)pBlock->CMD.pData;

  #if defined(RTS_CFG_EVENTS)
	if (pVMM->DLB.CIC.uRetry == 0)
	{
		vmmSetEvent(pVMM, EVT_INITIALIZE);
		osSleep(50);
	}
  #endif

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resInitialize
 */
IEC_UINT resInitialize(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;
	
	uRes = actClearResource(pVMM, &pVMM->DLB.CIC.uRetry, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	*(IEC_UINT *)pBlock->CMD.pData = pVMM->DLB.CIC.uRetry;
	pBlock->uLen = sizeof(IEC_UINT);
	
	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdGetProjectVersion
 */
IEC_UINT cmdGetProjectVersion(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pVMM->bProjLoaded == FALSE)
	{
		RETURN(ERR_NO_PROJECT);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resGetProjectVersion
 */
IEC_UINT resGetProjectVersion(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	OS_MEMCPY(pBlock->CMD.pData, pVMM->pProjectGUID, VMM_GUID);

	pBlock->uLen	= VMM_GUID;
	pBlock->byLast	= TRUE;
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdCustom
 */

#if defined(RTS_CFG_CUSTOM_OC)

IEC_UINT cmdCustom(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = osCmdCustom(pVMM, pBlock);

	RETURN(uRes);
}

#endif /* RTS_CFG_CUSTOM_OC */


/* ---------------------------------------------------------------------------- */
/**
 * resCustom
 */

#if defined(RTS_CFG_CUSTOM_OC)

IEC_UINT resCustom(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = osResCustom(pVMM, pBlock);
	
	RETURN(uRes);
}

#endif /* RTS_CFG_CUSTOM_OC */


/* ---------------------------------------------------------------------------- */
/**
 * actIsInSegment
 *
 */
#if defined(RTS_CFG_IO_LAYER) || defined(RTS_CFG_WRITE_FLAGS_PI)

static IEC_UINT actIsInSegment(STaskInfoVMM *pVMM, IEC_UINT uData, IEC_UINT uSegment, IEC_BOOL *bpWithin, IEC_UDINT *ulpOffset)
{
	SObject *pData = NULL;
	SObject *pSeg  = NULL;

	*bpWithin	= FALSE;
	*ulpOffset	= 0;

	if (uData == uSegment)
	{
		*bpWithin	= TRUE;
		*ulpOffset	= 0;

		RETURN(OK);
	}

	/* Check object
	 */
	if (uData >= pVMM->Project.uData + MAX_SEGMENTS)
	{
		RETURN(ERR_INVALID_INSTANCE);
	}

	pData = pVMM->Shared.pData + uData;
	pSeg  = pVMM->Shared.pData + uSegment;

	/* Is object in segment?
	 */
	if (pData->pAdr < pSeg->pAdr || pData->pAdr >= pSeg->pAdr + pSeg->ulSize)
	{
		*bpWithin	= FALSE;
		*ulpOffset	= 0;

		RETURN(OK);
	}

	/* Determine offset
	 */
	*bpWithin	= TRUE;
	*ulpOffset	= pData->pAdr - pSeg->pAdr;
	
	RETURN(OK);
}

#endif

/* ---------------------------------------------------------------------------- */

