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
 * Filename: actDFull.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"actDFull.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */

	
/* ---------------------------------------------------------------------------- */
/**
 * cmdDownloadBegin - CMD_DOWNLOAD_BEGIN
 */
IEC_UINT cmdDownloadBegin(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes	= OK;

	IEC_UDINT ulBinDLVersion;
	
	if (pBlock == NULL)
	{
		actDLBegin(pVMM, NULL, DOWN_MODE_FULL);

		RETURN(OK);
	}
	
	if (pBlock->uLen < sizeof(IEC_UINT) + VMM_GUID + sizeof(IEC_UDINT))
	{
		vmmQueueMessage(&pVMM->Shared.MsgQueue, "\n");
		vmmQueueMessage(&pVMM->Shared.MsgQueue, 
			"[VMM]: <* ERROR *>  Wrong firmware version received.\n");
		vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR*)pVMM->pBuffer, 
			"[VMM]: <* ERROR *>  Expected firmware version: %ld\n", RTS_VERSION_COMPATIBLE));

		RETURN(ERR_INVALID_DATA_SIZE);
	}

	if (pBlock->uBlock == 1)
	{
		OS_MEMSET(pVMM->pDownLoadGUID, 0x00, VMM_GUID);
		OS_MEMSET(pVMM->pProjectGUID,  0x00, VMM_GUID);

		pVMM->bProjActive = FALSE;
		pVMM->bProjLoaded = FALSE;

		uRes = actDLBegin(pVMM, pBlock, DOWN_MODE_FULL);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	OS_MEMCPY(pVMM->pDownLoadGUID, pBlock->CMD.pData + sizeof(IEC_UINT), VMM_GUID);

	TRACE_GUID("Down-Begin", "DLD", pVMM->pDownLoadGUID);
	
	// ulBinDLVersion = *(IEC_UDINT UNALIGNED *)(pBlock->CMD.pData + sizeof(IEC_UINT) + VMM_GUID);
	OS_MEMCPY(&ulBinDLVersion, pBlock->CMD.pData + sizeof(IEC_UINT) + VMM_GUID, sizeof(ulBinDLVersion));

	if (ulBinDLVersion != RTS_VERSION_COMPATIBLE)
	{
		vmmQueueMessage(&pVMM->Shared.MsgQueue, "\n");
		vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR*)pVMM->pBuffer, 
			"[VMM]: <* ERROR *>  Wrong Firmware version (%ld) received.\n", ulBinDLVersion));
		vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR*)pVMM->pBuffer, 
			"[VMM]: <* ERROR *>  Expected Firmware version: %ld\n", RTS_VERSION_COMPATIBLE));

		RETURN(ERR_INVALID_VERSION);
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDownloadBegin - CMD_DOWNLOAD_BEGIN
 */
IEC_UINT resDownloadBegin(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->DLB.uDomain = DOWN_BEGIN;
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdDownloadClear - CMD_DOWNLOAD_CLEAR
 */
IEC_UINT cmdDownloadClear(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes	= OK;
	
	if (pBlock == NULL)
	{
		RETURN(OK);
	}
	
	if (pVMM->DLB.uDomain != DOWN_BEGIN)
	{
		RETURN(ERR_INVALID_DOMAIN);
	}

	if (pBlock->uLen != sizeof(IEC_UINT))
	{
		RETURN(ERR_INVALID_DATA);
	}

	pVMM->DLB.uRetry = *(IEC_UINT *)pBlock->CMD.pData;
	//OS_MEMCPY(&(pVMM->DLB.uRetry), pBlock->CMD.pData, sizeof(pVMM->DLB.uRetry));

		
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDownloadClear - CMD_DOWNLOAD_CLEAR
 */
IEC_UINT resDownloadClear(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes	= OK;
	
	if (pBlock == NULL)
	{
		RETURN(OK);
	}

	uRes = actClearResource(pVMM, &pVMM->DLB.uRetry, pVMM->bWarmStart);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* Delete custom download files
	 */
  #if defined(RTS_CFG_CUSTOM_DL)
	uRes = actClearCustomDL(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
  #endif

	*(IEC_UINT *)pBlock->CMD.pData = pVMM->DLB.uRetry;
	//OS_MEMCPY(pBlock->CMD.pData, &(pVMM->DLB.uRetry), sizeof(pVMM->DLB.uRetry));
	pBlock->uLen = sizeof(IEC_UINT);

	if (pVMM->DLB.uRetry != 0)
	{
		/* IO layer yet not ready, notify Engineering for
		 * another download end command.
		 */

		RETURN(OK);
	}

	pVMM->DLB.uDomain = DOWN_CLEAR;

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdDownloadConfig - CMD_DOWNLOAD_CONFIG
 */
IEC_UINT cmdDownloadConfig(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = actDLConfig(pVMM, pBlock, DOWN_MODE_FULL, DOWN_CLEAR);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDownloadConfig - CMD_DOWNLOAD_CONFIG
 */
IEC_UINT resDownloadConfig(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->DLB.uDomain = DOWN_CONFIG;
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdDownloadCode - CMD_DOWNLOAD_CODE
 */
IEC_UINT cmdDownloadCode(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes;
	SProject *pProj = &pVMM->Project; 
	
	uRes = actDLCode(pVMM, pBlock, DOWN_MODE_FULL, DOWN_CONFIG, pProj->uCode, 0);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDownloadCode - CMD_DOWNLOAD_CODE
 */
IEC_UINT resDownloadCode(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->DLB.uDomain = DOWN_CODE;
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdDownloadInitial - CMD_DOWNLOAD_INITIAL
 */
IEC_UINT cmdDownloadInitial(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes;
	SProject *pProj = &pVMM->Project; 
	
	uRes = actDLInit(pVMM, pBlock, DOWN_MODE_FULL, DOWN_CODE, pProj->uData, pProj->uDirectVars, 0);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDownloadInitial - CMD_DOWNLOAD_INITIAL
 */
IEC_UINT resDownloadInitial(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->DLB.uDomain = DOWN_INIT;
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdDownloadCustom - CMD_DOWNLOAD_CUSTOM
 */
#if defined(RTS_CFG_CUSTOM_DL)

IEC_UINT cmdDownloadCustom(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = actDLFile(pVMM, pBlock, DOWN_MODE_FULL, DOWN_INIT, 
								osGetCustDownDir, VMM_DIR_CUSTOM, VMM_FILE_CUST_MAP);
	
	if (uRes != OK)
	{
		actCloseFile(&pVMM->DLB.FIL.hF1);
		actCloseFile(&pVMM->DLB.FIL.hF2);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDownloadCustom - CMD_DOWNLOAD_CUSTOM
 */
IEC_UINT resDownloadCustom(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->DLB.uDomain = DOWN_CUSTOM;
	}

	RETURN(OK);
}

#endif /* RTS_CFG_CUSTOM_DL */


/* ---------------------------------------------------------------------------- */
/**
 * cmdDownloadDebug - CMD_DOWNLOAD_DEBUG
 */
#if defined(RTS_CFG_DEBUG_INFO)

IEC_UINT cmdDownloadDebug(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = actDLFile(pVMM, pBlock, DOWN_MODE_FULL, DOWN_CUSTOM | DOWN_INIT,
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
 * resDownloadDebug - CMD_DOWNLOAD_DEBUG
 */
IEC_UINT resDownloadDebug(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock != NULL)
	{
		pVMM->DLB.uDomain = DOWN_DEBUG;
	}

	RETURN(OK);
}

#endif /* RTS_CFG_DEBUG_INFO */


/* ---------------------------------------------------------------------------- */
/**
 * cmdDownloadFinish - CMD_DOWNLOAD_FINISH
 */
IEC_UINT cmdDownloadFinish(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = actDLFinish(pVMM, pBlock, DOWN_MODE_FULL, DOWN_INIT | DOWN_CUSTOM | DOWN_DEBUG);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDownloadFinish - CMD_DOWNLOAD_FINISH
 */
IEC_UINT resDownloadFinish(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT	i;
	IEC_UINT	uRes;

	STaskInfoVM *pVM		= NULL;
	SSegInfo	*pSegInfo	= NULL;

	if (pBlock == NULL)
	{
		RETURN(OK);
	}

	/* Create Tasks
	 * ------------------------------------------------------------------------
	 */
	for (i = 0; i < pVMM->Project.uTasks; i++)
	{
		pVM = pVMM->ppVM[i];

		pVM->pShared	= &pVMM->Shared;
		pVM->pProject	= &pVMM->Project;

		pVM->usTask 	= (IEC_USINT)i;
		pVM->usFlags	= 0;

		pVM->Local.pState = (STaskState OS_DPTR *) (pVMM->Shared.pData[i + MAX_SEGMENTS + 2].pAdr);
		//OS_MEMCPY(&(pVM->Local.pState), &(pVMM->Shared.pData[i + MAX_SEGMENTS + 2].pAdr), sizeof(pVM->Local.pState));
		pVM->Local.pState->ulState	= TASK_STATE_ONCREATION;
		pVM->Local.pState->ulPrio	= pVM->Task.usPriority;
		pVM->Local.pState->ulCycle	= pVM->Task.ulPara1;
		pVM->Local.pState->ulETMax	= 0;
		pVM->Local.pState->ulErrNo	= OK;
		
		pVM->Local.uContext  = 1;
		OS_MEMSET(pVM->Local.pContext, 0, MAX_CALLS * sizeof(SContext));

	  #if defined(RTS_CFG_TASKSTAT)
		OS_MEMSET(&pVM->Stat, 0x00, sizeof(SStatistic));
		pVM->Stat.ulExecMin = 0xfffffffful;
	  #endif

		uRes = vmInitializeLocalSegments(pVM);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
		
	  #if defined (RTS_CFG_WRITE_FLAGS)
		OS_MEMSET(pVM->Local.WriteFlags.pAdr, 0x00, pVM->Local.WriteFlags.ulSize);
	  #endif
	}

	/* Compute VM's to be notified on a specific event
	 */
  #if defined(RTS_CFG_EVENTS)

	uRes = vmmBuildVMEvents(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
  #endif

	/* Configure VM timer
	 */
  #if defined(RTS_CFG_VM_IPC)

	uRes = msgTXCommand(MSG_TI_CONFIG, Q_LIST_VTI, Q_RESP_VMM_VTI, VMM_TO_IPC_MSG, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
  #endif	


	/* Compute used retain size
	 */
  #if defined(RTS_CFG_FFO)
	pSegInfo = &pVMM->Shared.RetainInfo;
  #else
	pSegInfo = pVMM->Shared.pSegInfo + SEG_RETAIN;
  #endif

	if (pSegInfo->uLastIndex == NO_INDEX)
	{
		pVMM->Project.ulRetainUsed = pSegInfo->ulOffset;
	}
	else
	{
		pVMM->Project.ulRetainUsed = OS_SUBPTR(pVMM->Shared.pData[pSegInfo->uLastIndex].pAdr, pVMM->Shared.pData[SEG_RETAIN].pAdr) 
										+ ALI(pVMM->Shared.pData[pSegInfo->uLastIndex].ulSize);
	}

  #if defined(RTS_CFG_COPY_DOMAIN)
	{
		SIntShared	*pShared	= &pVMM->Shared;
		SObject 	*pRetain	= pShared->pData + SEG_RETAIN;
		
		for (i = 0; i < pVMM->Project.uCpyRegions; i++) 
		{
			if (pVMM->Project.ulRetainUsed + ALI(pShared->CopyRegions[i].uSize) >= pRetain->ulSize)
			{
				RETURN(ERR_OVERRUN_OBJSEG);
			}

			pShared->CopyRegions[i].ulOffDst = pVMM->Project.ulRetainUsed;

			pVMM->Project.ulRetainUsed += ALI(pShared->CopyRegions[i].uSize);
		}

		if (pShared->uCpyRegions != 0)
		{
			IEC_BOOL bReverse = (IEC_BOOL)(pVMM->DLB.uMode & DOWN_MODE_WARM ? TRUE : FALSE);

			uRes = vmmCopyRegions(pShared, 0, pShared->uCpyRegions, bReverse);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
		}
	}
  #endif
	
  #if defined(RTS_CFG_BACNET)
	{
		if (sysIsAvailable(0x0002u) == TRUE)
		{
			pVMM->Project.ulRetainUsed += ALI(MAX_BACNET_OBJ * sizeof(IEC_UDINT));
		}
	}
  #endif

  #if defined(RTS_CFG_EXT_RETAIN)
	{
		SMessage Message;

		Message.uID 		= MSG_RT_SET_SIZE;
		Message.uLen		= sizeof(IEC_UDINT);
		Message.uRespQueue	= Q_RESP_VMM_RET;

		// *(IEC_UDINT UNALIGNED *)Message.pData = pVMM->Project.ulRetainUsed;
		OS_MEMCPY(Message.pData, &(pVMM->Project.ulRetainUsed), sizeof(pVMM->Project.ulRetainUsed));

		uRes = msgTXMessage(&Message, Q_LIST_RET, VMM_TO_IPC_MSG, TRUE);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}
  #endif

	/* Enable configuration
	 */
	pVMM->bProjLoaded = TRUE;

	pVMM->DLB.uDomain = DOWN_CFGIOL;

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdDownloadIOL - CMD_DOWNLOAD_IOL
 */
IEC_UINT cmdDownloadIOL(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock == NULL)
	{
		RETURN(OK);
	}

	if (pVMM->DLB.uDomain != DOWN_CFGIOL)
	{
		RETURN(ERR_INVALID_DOMAIN);
	}
	
	if (pBlock->uLen != sizeof(IEC_UINT))
	{
		RETURN(ERR_INVALID_DATA);
	}

	pVMM->DLB.uRetry = *(IEC_UINT *)pBlock->CMD.pData;
	//OS_MEMCPY(&(pVMM->DLB.uRetry), pBlock->CMD.pData, sizeof(pVMM->DLB.uRetry));

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDownloadIOL - CMD_DOWNLOAD_IOL
 */
IEC_UINT resDownloadIOL(STaskInfoVMM *pVMM, XBlock *pBlock)
{
  #if defined(RTS_CFG_IO_LAYER)
	IEC_UINT uRes = OK;
  #endif

	if (pBlock == NULL)
	{
		RETURN(OK);
	}

	/* Create IO Layer
	 */
	if (pVMM->DLB.uRetry == 0)
	{
	  #if defined(RTS_CFG_IO_LAYER)
		uRes = ioCreateLayer(pVMM);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	  #endif /* RTS_CFG_IO_LAYER */
	}

	/* Configure IO Layer
	 */
  #if defined(RTS_CFG_IO_LAYER)
	uRes = ioConfigLayer(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
  #endif /* RTS_CFG_IO_LAYER */

	*(IEC_UINT *)pBlock->CMD.pData = pVMM->DLB.uRetry;
	//OS_MEMCPY(pBlock->CMD.pData, &(pVMM->DLB.uRetry), sizeof(pVMM->DLB.uRetry));
	pBlock->uLen = sizeof(IEC_UINT);

	if (pVMM->DLB.uRetry != 0)
	{
		/* IO layer yet not ready, notify Engineering for
		 * another download end command.
		 */

		RETURN(OK);
	}

	pVMM->DLB.uDomain = DOWN_FINISH;
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdDownloadEnd - CMD_DOWNLOAD_END
 */
IEC_UINT cmdDownloadEnd(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock == NULL)
	{
		RETURN(OK);
	}

	if (pVMM->DLB.uDomain != DOWN_FINISH)
	{
		RETURN(ERR_INVALID_DOMAIN);
	}
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDownloadEnd - CMD_DOWNLOAD_END
 */
IEC_UINT resDownloadEnd(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT i;
	IEC_UINT uRes = OK;

	if (pBlock == NULL)
	{
		RETURN(OK);
	}

	pVMM->DLB.uDomain = DOWN_NONE;

	/* Start IO Layer
	 */
  #if defined(RTS_CFG_IO_LAYER)
	uRes = ioStartLayer(pVMM, TRUE, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
  #endif /* RTS_CFG_IO_LAYER */
	
	/* Enable configuration
	 */
	pVMM->bProjActive	= TRUE;
	pVMM->ulResState	= pVMM->DLB.usResState;

	for (i = 0; i < pVMM->Project.uTasks; i++)
	{
		if (pVMM->ulResState == RES_STATE_PAUSED)
		{
			/* Disable task initially if resource is stopped
			 */
			pVMM->ppVM[i]->Task.usAttrib &= ~VMM_TASK_ATTR_AUTOSTART;
		}

		uRes = osCreateVMTask(pVMM->ppVM[i]);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		pVMM->uCreatedTasks++;
	}

	/* Start VM timer
	 */
  #if defined(RTS_CFG_VM_IPC)
	uRes = msgTXCommand(MSG_TI_START, Q_LIST_VTI, Q_RESP_VMM_VTI, VMM_TO_IPC_MSG, TRUE);
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
		
	pBlock->uLen = 0;
	
  #if defined(RTS_CFG_EVENTS)
	vmmSetEvent(pVMM, EVT_DOWNLOAD_END);
  #endif
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */

