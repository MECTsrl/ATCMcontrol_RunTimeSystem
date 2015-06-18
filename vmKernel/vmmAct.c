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
 * Filename: vmmAct.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"vmmAct.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "iolDef.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

static IEC_UINT actDataDiv(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT uToCopy, IEC_UINT *upCount, void *pData);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * actDataDiv
 *
 */
static IEC_UINT actDataDiv(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT uToCopy, IEC_UINT *upCount, void *pData)
{
	if (uToCopy - pDL->uRecv > pBlock->uLen - *upCount)
	{
		/* Data is divided
		 */
		if (pDL->uRecv + pBlock->uLen - *upCount > sizeof(pDL->pRecv))
		{
			RETURN(ERR_BUFFER_TOO_SMALL);
		}

		OS_MEMCPY(pDL->pRecv + pDL->uRecv, pBlock->CMD.pData + *upCount, pBlock->uLen - *upCount);
		pDL->uRecv = (IEC_UINT)(pDL->uRecv + pBlock->uLen - *upCount);

		*upCount = pBlock->uLen;

		return WRN_DIVIDED; 	/* Just a warning! */
	}

	if (pDL->uRecv != 0)
	{
		/* Data was divided
		 */
		OS_MEMCPY(pData, pDL->pRecv, pDL->uRecv);

		uToCopy = (IEC_UINT)(uToCopy - pDL->uRecv);
	}

	/* Data completely in current block
	 */
	OS_MEMCPY((IEC_DATA *)pData + pDL->uRecv, pBlock->CMD.pData + *upCount, uToCopy);

	pDL->uRecv = 0;
	*upCount = (IEC_UINT)(*upCount + uToCopy);

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actClearResource
 *
 */
IEC_UINT actClearResource(STaskInfoVMM *pVMM, IEC_UINT *upRetry, IEC_BOOL bWarmStart)
{
	IEC_UINT uResAll = OK;
	IEC_UINT uResLoc = OK;

	IEC_UINT i;

	static IEC_UDINT ulClearTime = 0;

	SIntShared *pShared = &pVMM->Shared;

	pVMM->bProjActive = FALSE;
	pVMM->bProjLoaded = FALSE;
	
	if (*upRetry == 0)
	{
		ulClearTime = osGetTime32();

		uResLoc = osOnClearBegin(pVMM);
		if (uResLoc != OK)
		{
			uResAll = uResLoc;
			TR_RET(uResLoc);
		}

		/* Stop VM timer task
		 */
	  #if defined(RTS_CFG_VM_IPC)

		uResLoc = msgTXCommand(MSG_TI_STOP, Q_LIST_VTI, Q_RESP_VMM_VTI, VMM_TO_IPC_MSG, TRUE);
		if (uResLoc != OK)
		{
			uResAll = uResLoc;
			TR_RET(uResLoc);
		}

	  #endif

		/* Stop Online Change Helper task
		 */
	  #if defined(RTS_CFG_VM_IPC) && defined(RTS_CFG_ONLINE_CHANGE)

		uResLoc = msgTXCommand(MSG_OC_PREPARE, Q_LIST_OC, Q_RESP_VMM_OC, VMM_TO_IPC_MSG, TRUE);
		if (uResLoc != OK)
		{
			uResAll = uResLoc;
			TR_RET(uResLoc);
		}

	  #endif
		
		/* Stop Retain Update task
		 */
	  #if defined(RTS_CFG_EXT_RETAIN)

		uResLoc = msgTXCommand(MSG_RT_CLOSE, Q_LIST_RET, Q_RESP_VMM_RET, VMM_TO_IPC_MSG_LONG, TRUE);
		if (uResLoc != OK)
		{
			uResAll = uResLoc;
			TR_RET(uResLoc);
		}

	  #endif
		
		/* Delete all breakpoints
		 */
		uResLoc = bpDeleteAllBreakpoints(pVMM);
		if (uResLoc != OK)
		{
			uResAll = uResLoc;
			TR_RET(uResLoc);
		}

		/* Reset tasks with reached breakpoints
		 */
		for (i = 0; i < pVMM->uCreatedTasks; i++)
		{
			uResLoc = vmmResetTask(pVMM, i);
			if (uResLoc != OK && uResLoc != WRN_TIME_OUT)
			{
				uResAll = uResLoc;
			}

			TR_RET(uResLoc);
		}

		/* Stop & Terminate IO layer
		 */
	  #if defined(RTS_CFG_IO_LAYER)

		uResLoc = ioStartLayer(pVMM, FALSE, FALSE);
		if (uResLoc != OK)
		{
			uResAll = uResLoc;
			TR_RET(uResLoc);
		}

		uResLoc = ioTerminateLayer(pVMM, bWarmStart);
		if (uResLoc != OK)
		{
			uResAll = uResLoc;
			TR_RET(uResLoc);
		}

	  #endif

		osSleep(500);

	} /* if (upRetry == 0) */

	
  #if defined(RTS_CFG_IO_LAYER)

	uResLoc = ioVerifyTermination(pVMM, ulClearTime);
	if (uResLoc != OK && uResLoc != WRN_IN_PROGRESS)
	{
		uResAll = uResLoc;
		TR_RET(uResLoc);
	}

	if (uResLoc == WRN_IN_PROGRESS)
	{
		*upRetry += 1;

		RETURN(OK);

	} /* if (uResLoc == WRN_IN_PROGRESS) */
	
	/* Finished, everything OK.
	 */
	
  #endif

	*upRetry = 0;

	/* Kill all VM tasks
	 */
	for (i = 0; i < pVMM->uCreatedTasks; i++)
	{
		uResLoc = vmmDeleteTask(pVMM, i);
		if (uResLoc != OK)
		{
			uResAll = uResLoc;
			TR_RET(uResLoc);
		}
	}

	/* Destroy IO layer
	 */
  #if defined(RTS_CFG_IO_LAYER)

	uResLoc = ioDestroyLayer(pVMM);
	if (uResLoc != OK)
	{
		uResAll = uResLoc;
		TR_RET(uResLoc);
	}

  #endif /* RTS_CFG_IO_LAYER */
	
	osSleep(100);

	uResLoc = osOnClearAfterDelVM(pVMM);
	if (uResLoc != OK)
	{
		uResAll = uResLoc;
		TR_RET(uResLoc);
	}

	/* Clean up
	 */
	for (i = 0; i < pVMM->uCreatedTasks; i++)
	{
		/* VM should have terminate itself already!
		 */
		uResLoc = osKillVMTask(pVMM->ppVM[i]);
		if (uResLoc != OK)
		{
			uResAll = uResLoc;
			TR_RET(uResLoc);
		}
	  
	  #if defined(RTS_CFG_TASK_IMAGE) | defined(RTS_CFG_WRITE_FLAGS)

		uResLoc = vmFinalizeLocalSegments(pVMM->ppVM[i]);
		if (uResLoc != OK)
		{
			uResAll = uResLoc;
			TR_RET(uResLoc);
		}
	  
	  #endif

		uResLoc = vmFreeTaskInfoVM(pVMM->ppVM[i]);
		if (uResLoc != OK)
		{
			uResAll = uResLoc;
			TR_RET(uResLoc);
		}
	}
		
	pVMM->uCreatedTasks = 0;

	/* Finalize Debug Information
	 */
  #if defined(RTS_CFG_DEBUG_INFO)

	uResLoc = dbiFinalize(pVMM);
	if (uResLoc != OK)
	{
		uResAll = uResLoc;
		TR_RET(uResLoc);
	}

  #endif
	
	/* Invalidate current project
	 */
	pVMM->ulResState = RES_STATE_ONCREATION;
	OS_MEMSET(pVMM->pProjectGUID, 0x00, VMM_GUID);

  #if defined(RTS_CFG_COPY_DOMAIN)
	pShared->uCpyRegions	= 0;
  #endif

  #if defined(RTS_CFG_FFO)
	for (i = 0; i < pVMM->Project.uCode; i++)
	{
		/* Free code objects
		 */
		if (pShared->pCode[i].pAdr != 0)
		{
			uResLoc = osFree(&pShared->pCode[i].pAdr);
			if (uResLoc != OK)
			{
				uResAll = uResLoc;
				TR_RET(uResLoc);
			}
		}
	}

	for (i = MAX_SEGMENTS; i < MAX_SEGMENTS + pVMM->Project.uData; i++)
	{
		/* Free data objects
		 */
		if (pShared->pData[i].pAdr != 0 && (pShared->pDataSeg[i] & MASC_ALLOCATED) != 0)
		{
			uResLoc = osFree(&pShared->pData[i].pAdr);
			if (uResLoc != OK)
			{
				uResAll = uResLoc;
				TR_RET(uResLoc);
			}
		}
	}

  #endif /* RTS_CFG_FFO */

	pVMM->Project.uCode = 0;
	OS_MEMSET(pShared->pCode, 0x00, MAX_CODE_OBJ * sizeof(SObject));

	pVMM->Project.uData = 0;
	OS_MEMSET(pShared->pData + MAX_SEGMENTS, 0x00, (MAX_DATA_OBJ - MAX_SEGMENTS) * sizeof(SObject));

  #if defined(RTS_CFG_FFO)
	OS_MEMSET(pShared->pDataSeg + MAX_SEGMENTS, 0x00, (MAX_DATA_OBJ - MAX_SEGMENTS) * sizeof(IEC_USINT));
  #endif

	OS_MEMSET(&pVMM->Project, 0x00, sizeof(SProject));

  #if defined(RTS_CFG_EVENTS)

	/* Destroy task events
	 */
	uResLoc = vmmClearVMEvents(pVMM);
	if (uResLoc != OK)
	{
		uResAll = uResLoc;
		TR_RET(uResLoc);
	}

  #endif

	uResLoc = osOnClearEnd(pVMM);
	if (uResLoc != OK)
	{
		uResAll = uResLoc;
		TR_RET(uResLoc);
	}

	RETURN(uResAll);
}

/* ---------------------------------------------------------------------------- */
/**
 * actClearCustomDL
 *
 */
#if defined(RTS_CFG_CUSTOM_DL)

IEC_UINT actClearCustomDL(STaskInfoVMM *pVMM)
{
	IEC_UINT  uRes	 = OK;
	IEC_UDINT hMap	 = 0;
	IEC_BOOL  bExist = FALSE;

	IEC_CHAR  szFile[VMM_MAX_PATH];

	uRes = utilExistFile((IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), osGetCustDownDir, VMM_DIR_CUSTOM, VMM_FILE_CUST_MAP, &bExist);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (bExist == FALSE)
	{
		RETURN(OK);
	}

	uRes = utilOpenFile(&hMap, (IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), osGetCustDownDir, VMM_DIR_CUSTOM, VMM_FILE_CUST_MAP, FIO_MODE_READ);
	if (uRes == OK)
	{
		while (uRes == OK)
		{
			uRes = xxxReadLine(hMap, szFile, VMM_MAX_PATH);
			if (uRes == OK)
			{
				utilTruncCRLF(szFile);
				utilDeleteFile((IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), osGetCustDownDir, VMM_DIR_CUSTOM, szFile);
			}
		}
	
		xxxClose(hMap);

		utilDeleteFile((IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), osGetCustDownDir, VMM_DIR_CUSTOM, VMM_FILE_CUST_MAP);
	}

	RETURN(OK);
}

#endif /* RTS_CFG_CUSTOM_DL */

/* ---------------------------------------------------------------------------- */
/**
 * actGetFileData
 *
 */
#if defined(RTS_CFG_FILE_ACCESS) || defined(RTS_CFG_FILE_NATIVE)

IEC_UINT actGetFileData(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UDINT hFile)
{
	IEC_UINT uRes;
	IEC_UINT uToCopy;

	if ((IEC_UINT)(pBlock->uLen - *upCount) < pDL->HDR.ulSize)
	{
		/* File continued in the next block
		 */
		uToCopy = (IEC_UINT)(pBlock->uLen - *upCount);
	}
	else
	{
		/* File completely contained in this block
		 */
		uToCopy = (IEC_UINT)(pDL->HDR.ulSize);
	}

	uRes = xxxWrite(hFile, pBlock->CMD.pData + *upCount, uToCopy);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	*upCount		 = (IEC_UINT) (*upCount + uToCopy); 		
	pDL->HDR.ulSize -= uToCopy;

	RETURN(OK);
}

#endif /* RTS_CFG_FILE_ACCESS || RTS_CFG_FILE_NATIVE */

/* ---------------------------------------------------------------------------- */
/**
 * actGetXXXTemplate
 *
 * Template (sample) for actXXX functions.
 */
#if 0

IEC_UINT actGetXXXTemplate(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SToGet *pToGet)
{
	static IEC_UINT uToGet	= 0;						/* --> Objects already received.			*/

	XToGet xPToGet; 									/* --> Protocol structure definition		*/

	if (pBlock == NULL)
	{
		uToGet = 0; 									/* --> Initialize							*/
		RETURN(OK);
	}

	pDL->bDone	= FALSE;								/* --> True, if all objects received.		*/
	pDL->bOther = FALSE;								/* --> Received objects requires additional */
														/*	   and different data object.			*/
	while (*upCount < pBlock->uLen && uToGet < uObj)
	{
		IEC_UINT uRes = actDataDiv(pDL, pBlock, sizeof(XToGet), upCount, &xToGet);
		if (uRes != OK)
		{
			uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
			RETURN(uRes);
		}

		if (pToGet != NULL) 							/* --> Destination == NULL -> skip object.	*/
		{
			pToGet->Something = xToGet.Something;		/* --> Copy and verify to destination.		*/
														/* --> Don't use memcpy(), prot. may change!*/
			/* Add copy and verification code here */
			/* ... */
		}

		uToGet++;										/* --> Increment number of objects. 		*/
	
	} /* while (*upCount < pBlock->uLen && uToGet < uObj) */

	pDL->bDone = (IEC_BOOL)(uToGet == uObj);			/* --> All objects received?				*/

	RETURN(OK);
}

#endif

/* ---------------------------------------------------------------------------- */
/**
 * actGetOCConfig
 *
 */
IEC_UINT actGetOCConfig(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, SOnlChg *pOC, SProject const *pProject)
{
	IEC_UINT		uRes;
	XOnlineChange	xOC;

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	uRes = actDataDiv(pDL, pBlock, sizeof(XOnlineChange), upCount, &xOC);
	if (uRes != OK)
	{
		uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
		RETURN(uRes);
	}

	if (pOC != NULL)
	{
		pOC->uCode		= xOC.uCode;
		pOC->uData		= xOC.uData;
		pOC->uToCopy	= xOC.uToCopy;
		pOC->uDirVars	= xOC.uDirVars;
		pOC->iCodeDiff	= xOC.iCodeDiff;
		pOC->iDataDiff	= xOC.iDataDiff;
		pOC->uTaskInfo	= xOC.uTaskInfo;

		/* Check if not to many changed objects
		 */
		if (pOC->uCode > MAX_OC_CODE_OBJ)
		{
			RETURN(ERR_OC_TO_MANY_CODE);
		}

		if (pOC->uData > MAX_OC_DATA_OBJ)
		{
			RETURN(ERR_OC_TO_MANY_INST);
		}

		if (pOC->uToCopy > MAX_OC_COPY_REG)
		{
			RETURN(ERR_OC_TO_MANY_CL);
		}

		/* Check if the new project fits in the object lists
		 */
		if (pProject->uCode + pOC->iCodeDiff > MAX_CODE_OBJ)
		{
			RETURN(ERR_OVERRUN_CLASS);
		}

		if (pProject->uData + MAX_SEGMENTS + pOC->iDataDiff > MAX_DATA_OBJ)
		{
			RETURN(ERR_OVERRUN_INSTANCE);
		}

		/* Check if old and new objects fits into the object lists
		 */
		if (pProject->uCode + pOC->uCode + LIST_OFF(pOC->iCodeDiff) > MAX_CODE_OBJ)
		{
			RETURN(ERR_OC_TEMP_CODE);
		}

		if (pProject->uData + pOC->uData + MAX_SEGMENTS + LIST_OFF(pOC->iDataDiff) > MAX_DATA_OBJ)
		{
			RETURN(ERR_OC_TEMP_INST);
		}
	
	} /* if (pOC != NULL) */

	pDL->bDone = TRUE;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetOCList
 *
 */
IEC_UINT actGetOCList(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SOCCopy *pOCC)
{
	static IEC_UINT uList	= 0;

	XOCInstCopy xCopy;

	if (pBlock == NULL)
	{
		uList = 0;
		RETURN(OK);
	}

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	while (*upCount < pBlock->uLen && uList < uObj)
	{
		IEC_UINT uRes = actDataDiv(pDL, pBlock, sizeof(XOCInstCopy), upCount, &xCopy);
		if (uRes != OK)
		{
			uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
			RETURN(uRes);
		}

		if (pOCC != NULL)
		{
			SOCCopy *pOCCL = pOCC + uList;

			pOCCL->uOld 	= xCopy.uOld;
			pOCCL->uNew 	= xCopy.uNew;
			pOCCL->uOldOff	= xCopy.uOldOff;
			pOCCL->usOldBit = xCopy.usOldBit;
			pOCCL->uNewOff	= xCopy.uNewOff;
			pOCCL->usNewBit = xCopy.usNewBit;
			pOCCL->ulSize	= xCopy.ulSize;
		}

		uList++;
	
	} /* while (*upCount < pBlock->uLen && uProjInfo < uObj) */

	pDL->bDone = (IEC_BOOL)(uList == uObj);

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetDownHeader
 *
 */
IEC_UINT actGetDownHeader(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, SDownHeader *pHeader)
{
	IEC_UINT	uRes;
	XDownHeader xHeader;

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	uRes = actDataDiv(pDL, pBlock, sizeof(XDownHeader), upCount, &xHeader);
	if (uRes != OK)
	{
		uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
		RETURN(uRes);
	}

	if (pHeader != NULL)
	{
		pHeader->byFixed	= xHeader.byFixed;
		pHeader->byType 	= xHeader.byType;
		pHeader->uIndex 	= xHeader.uIndex;
		pHeader->ulOffset	= xHeader.ulOffset;
		pHeader->ulSize 	= xHeader.ulSize;
		pHeader->uSegment	= xHeader.uSegment;
	}

	pDL->bDone = TRUE;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetDownDirect
 *
 */
IEC_UINT actGetDownDirect(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, SDownDirect *pDirect)
{
	IEC_UINT	uRes;
	XDownDirect xDirect;

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	uRes = actDataDiv(pDL, pBlock, sizeof(XDownDirect), upCount, &xDirect);
	if (uRes != OK)
	{
		uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
		RETURN(uRes);
	}

	if (pDirect != NULL)
	{
		pDirect->uBit		= xDirect.byBit;
		pDirect->ulOffset	= xDirect.ulOffset;
		pDirect->uSegment	= xDirect.bySegment;
		pDirect->uSize		= xDirect.uSize;
	}

	pDL->bDone	= TRUE;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetFileDef
 *
 */
IEC_UINT actGetFileDef(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, SFileDef *pFileDef)
{
	IEC_UINT uRes;
	XFileDef xFileDef;

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	uRes = actDataDiv(pDL, pBlock, sizeof(XFileDef), upCount, &xFileDef);
	if (uRes != OK)
	{
		uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
		RETURN(uRes);
	}

	if (pFileDef != NULL)
	{
		pFileDef->ulDataLen = xFileDef.ulDataLen;
		pFileDef->ulOffset	= 0;
		pFileDef->uNameLen	= xFileDef.uNameLen;
		pFileDef->usCRC 	= xFileDef.usCRC;
		pFileDef->usHash	= xFileDef.usHash;

		if (pFileDef->uNameLen >= VMM_MAX_PATH)
		{
			/* uNameLen without ZB!
			 */
			RETURN(ERR_PATH_TO_LONG);
		}
	}	

	pDL->bDone	= TRUE;
	pDL->bOther = TRUE;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetFileName
 *
 */
IEC_UINT actGetFileName(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uLen, IEC_CHAR *szName)
{
	IEC_UINT uRes;

	pDL->bDone	= FALSE;

	uRes = actDataDiv(pDL, pBlock, uLen, upCount, szName);
	if (uRes != OK)
	{
		uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
		RETURN(uRes);
	}

	szName[uLen] = '\0';

	pDL->bDone	= TRUE;
	pDL->bOther = FALSE;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetProjectInfo
 *
 */
IEC_UINT actGetProjectInfo(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SProject *pProject, IEC_UDINT *ulpBinDLVersion)
{
	static IEC_UINT uProject = 0;

	IEC_UINT		uRes = OK;
	XProject_213	xProject;

	if (pBlock == NULL)
	{
		uProject = 0;
		RETURN(OK);
	}

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	while (*upCount < pBlock->uLen && uProject < uObj)
	{
		uRes = actDataDiv(pDL, pBlock, sizeof(XProject_213), upCount, &xProject);
		if (uRes != OK)
		{
			uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
			RETURN(uRes);
		}

		if (pProject != NULL)
		{
			pProject->uCode 			= xProject.uCodeObjects;
			pProject->uDirectVars		= xProject.uDirectVars;
			pProject->uData 			= xProject.uInstObjects;
			pProject->ulIECOffset		= xProject.ulIECOffset;
			pProject->ulStateVarOffs	= xProject.ulStateVarOffs;
			pProject->uTasks			= xProject.uTasks;
		  
			*ulpBinDLVersion			= xProject.ulBinDLVersion;

		  #if defined(RTS_CFG_COPY_DOMAIN)
			pProject->uCpyRegions		= xProject.uCpyRegions;
			pProject->uCpyRegConst		= xProject.uCpyRegConst;
			pProject->uCpyRegCOff		= xProject.uCpyRegCOff;
		  #endif

		  #if defined(RTS_CFG_EXT_PROJ_INFO)
			pProject->uProjInfo 		= xProject.uProjInfo;
		  #else
			*upCount = (IEC_UINT)(*upCount + xProject.uProjInfo * sizeof(XProjInfo));
		  #endif

			pProject->uIOLayer			= xProject.uIOLayer;
			
			/* Check binary download format version
			 */
			if (xProject.ulBinDLVersion != RTS_VERSION_COMPATIBLE)
			{
				/* CG version does not match
				 */
				RETURN(ERR_INVALID_VERSION);
			}	

			/* Check values
			 */
			if (pProject->uTasks > MAX_TASKS)
			{
				RETURN(ERR_OVERRUN_TASK);
			}

			if (pProject->uCode > MAX_CODE_OBJ)
			{
				RETURN(ERR_OVERRUN_CLASS);
			}	

		  #if defined(RTS_CFG_COPY_DOMAIN)
			if (pProject->uCpyRegions > MAX_COPY_REGS)
			{
				RETURN(ERR_OVERRUN_COPYSEG);
			}	
		  #endif

		} /* if (pProject != NULL) */

		uProject++;
	
	} /* while (*upCount < pBlock->uLen && uProject < uObj) */

	pDL->bDone = (IEC_BOOL)(uProject == uObj);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetExtProjectInfo
 *
 */
IEC_UINT actGetExtProjectInfo(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SProjInfo *pProjInfo)
{
	static IEC_UINT uProjInfo	= 0;

	XProjInfo xProjInfo;

	if (pBlock == NULL)
	{
		uProjInfo = 0;
		RETURN(OK);
	}

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	while (*upCount < pBlock->uLen && uProjInfo < uObj)
	{
		IEC_UINT uRes = actDataDiv(pDL, pBlock, sizeof(XProjInfo), upCount, &xProjInfo);
		if (uRes != OK)
		{
			uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
			RETURN(uRes);
		}

		if (pProjInfo != NULL)
		{
		  #if defined(RTS_CFG_EXT_PROJ_INFO)
			OS_STRCPY(pProjInfo->szProjName, xProjInfo.szProjName);
			OS_STRCPY(pProjInfo->szProjVers, xProjInfo.szProjVers);

			OS_STRCPY(pProjInfo->szCreated,  xProjInfo.szCreated);
			OS_STRCPY(pProjInfo->szModified, xProjInfo.szModified);
			OS_STRCPY(pProjInfo->szOwner,	 xProjInfo.szOwner);

			OS_STRCPY(pProjInfo->szComment1, xProjInfo.szComment1);
			OS_STRCPY(pProjInfo->szComment2, xProjInfo.szComment2);
			OS_STRCPY(pProjInfo->szComment3, xProjInfo.szComment3);
		  #endif
		}

		uProjInfo++;
	
	} /* while (*upCount < pBlock->uLen && uProjInfo < uObj) */

	pDL->bDone = (IEC_BOOL)(uProjInfo == uObj);

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetRetainList
 *
 */
IEC_UINT actGetRetainList(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SRetainReg *pRegion)
{
	static IEC_UINT uCpyRegions = 0;

	XCopyRegion xCopyRegion;

	if (pBlock == NULL)
	{
		uCpyRegions = 0;
		RETURN(OK);
	}

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	while (*upCount < pBlock->uLen && uCpyRegions < uObj)
	{
		IEC_UINT uRes = actDataDiv(pDL, pBlock, sizeof(XCopyRegion), upCount, &xCopyRegion);
		if (uRes != OK)
		{
			uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
			RETURN(uRes);
		}

		if (pRegion != NULL)
		{
			pRegion[uCpyRegions].uData		= xCopyRegion.uiInst;
			pRegion[uCpyRegions].uOffSrc	= xCopyRegion.uiOffSrc;
			pRegion[uCpyRegions].ulOffDst	= 0;
			pRegion[uCpyRegions].uSize		= xCopyRegion.uiSize;
		}

		uCpyRegions++;
	
	} /* while (*upCount < pBlock->uLen && uCpyRegions < uObj) */

	pDL->bDone = (IEC_BOOL)(uCpyRegions == uObj);

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetSegments
 *
 */
IEC_UINT actGetSegments(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, SObject *pData, SSegInfo *pSegInfo)
{
	static IEC_UINT uSegment = 0;

	XObject xSegDesc;

	if (pBlock == NULL)
	{
		uSegment = 0;
		RETURN(OK);
	}

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	while (*upCount < pBlock->uLen && uSegment < MAX_SEGMENTS)
	{
		IEC_UINT uRes = actDataDiv(pDL, pBlock, sizeof(XObject), upCount, &xSegDesc);
		if (uRes != OK)
		{
			uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
			RETURN(uRes);
		}

		if (pData != NULL)
		{
			SObject *pSeg = pData + uSegment;

			pSeg->pAdr	 = xSegDesc.pAdr;
			pSeg->ulSize = xSegDesc.ulSize;
			
		  #if ! defined(RTS_CFG_FFO)
			pSegInfo[uSegment].ulOffset   = 0;
			pSegInfo[uSegment].uLastIndex = NO_INDEX;
		  #endif

		  #if ! defined(RTS_CFG_EXT_RETAIN)

			if (uSegment == SEG_RETAIN)
			{
				pSeg->ulSize += VMM_GUID;
			}
		  #endif
			
		  #if defined(RTS_CFG_BACNET)

			if (uSegment == SEG_RETAIN)
			{
				/* Present-value persistence storage
				 */
				if (sysIsAvailable(0x0002u) == TRUE)
				{
					pSeg->ulSize += ALI(MAX_BACNET_OBJ * sizeof(IEC_UDINT));
				}
			}
		  #endif
			
			uRes = osInitializeSegment(uSegment, pSeg);
			if (uRes != OK)
			{
				RETURN(uRes);
			}			
		}

		uSegment++;
	
	} /* while (*upCount < pBlock->uLen && uSegment < MAX_SEGMENTS) */

	pDL->bDone = (IEC_BOOL)(uSegment == MAX_SEGMENTS);

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetIOLayer
 *
 */
IEC_UINT actGetIOLayer (SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SIOLayer *pIOLayer)
{
	static IEC_UINT uIOLayer = 0;

	XIOLayer xIOLayer;

	if (pBlock == NULL)
	{
		uIOLayer = 0;
		RETURN(OK);
	}

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	while (*upCount < pBlock->uLen && uIOLayer < uObj)
	{
		IEC_UINT uRes = actDataDiv(pDL, pBlock, sizeof(XIOLayer), upCount, &xIOLayer);
		if (uRes != OK)
		{
			uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
			RETURN(uRes);
		}

		if (pIOLayer != NULL)
		{		
			SIOLayer *pIO = pIOLayer + uIOLayer;

			pIO->ulIOffs	= xIOLayer.ulIOffs;
			pIO->ulISize	= xIOLayer.ulISize;
			pIO->ulQOffs	= xIOLayer.ulQOffs;
			pIO->ulQSize	= xIOLayer.ulQSize;
			pIO->ulMOffs	= xIOLayer.ulMOffs;
			pIO->ulMSize	= xIOLayer.ulMSize;

			pIO->usChannel	= xIOLayer.usChannel;

			pIO->uState 	= IO_STATE_NONE;
			
			pIO->bEnabled		= FALSE;			/* must be enabled manually!	*/
			pIO->bCreated		= FALSE;
			
			pIO->bAsyncConfig	= FALSE;			/* Synchronous as default!		*/
			
			pIO->usNotifyRd 	= IO_NOTIFY_NONE;	/* must be enabled manually!	*/
			pIO->usNotifyWr 	= IO_NOTIFY_NONE;	/* must be enabled manually!	*/

			OS_MEMCPY(pIO->szIOLayer, xIOLayer.szName, VMM_MAX_IEC_IDENT);
			pIO->szIOLayer[VMM_MAX_IEC_IDENT - 1] = 0;
		}

		uIOLayer++;

	} /* while (*upCount < pBlock->uLen && uIOLayer < uObj) */

	pDL->bDone = (IEC_BOOL)(uIOLayer == uObj);

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetPrograms
 *
 */
IEC_UINT actGetPrograms(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SIndex *pProg)
{
	static IEC_UINT uPrograms = 0;

	XIndex xIndex;

	if (pBlock == NULL)
	{
		uPrograms = 0;
		RETURN(OK);
	}

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	while (*upCount < pBlock->uLen && uPrograms < uObj)
	{
		IEC_UINT uRes = actDataDiv(pDL, pBlock, sizeof(XIndex), upCount, &xIndex);
		if (uRes != OK)
		{
			uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
			RETURN(uRes);
		}

		if (pProg != NULL)
		{
			pProg[uPrograms].uCode = xIndex.uCode;
			pProg[uPrograms].uData = xIndex.uInst;
		}

		uPrograms++;

	} /* while (*upCount < pBlock->uLen && uPrograms < uObj) */

	pDL->bDone = (IEC_BOOL)(uPrograms == uObj);

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetImageRegions
 *
 */
#if defined(RTS_CFG_TASK_IMAGE)

IEC_UINT actGetImageRegions(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uRd, IEC_UINT uWr, SRegion *pRd, SRegion *pWr)
{
	static IEC_UINT uRegRd = 0;
	static IEC_UINT uRegWr = 0;

	XMemRegion xRegion;

	if (pBlock == NULL)
	{
		uRegRd = 0;
		uRegWr = 0;

		RETURN(OK);
	}

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	while (*upCount < pBlock->uLen && (uRegRd < uRd || uRegWr < uWr))
	{
		IEC_UINT uRes = actDataDiv(pDL, pBlock, sizeof(XMemRegion), upCount, &xRegion);
		if (uRes != OK)
		{
			uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
			RETURN(uRes);
		}

		if (pRd != NULL && pWr != NULL)
		{
			SRegion *pRegion;

			if (xRegion.bySegment != SEG_OUTPUT && xRegion.bySegment != SEG_INPUT)
			{
				RETURN(ERR_INVALID_PARAM);
			}

			if (xRegion.bRead != FALSE)
			{
				pRegion = pRd + uRegRd;

				if (uRegRd >= MAX_READ_REGIONS)
				{
					RETURN(ERR_OVERRUN_REGION);
				}

				uRegRd++;
			}
			else
			{
				pRegion = pWr + uRegWr;

				if (uRegWr >= MAX_WRITE_REGIONS)
				{
					RETURN(ERR_OVERRUN_REGION);
				}

				uRegWr++;
			}

			pRegion->uSize		= xRegion.uSize;
			pRegion->ulOffset	= xRegion.ulOffset;
			pRegion->usSegment	= xRegion.bySegment;
		}
		else
		{
			/* Skip entries
			 */
			if (uRegRd < uRd)
			{
				uRegRd++;
			}
			else
			{
				uRegWr++;
			}
		}
	
	} /* while (*upCount < pBlock->uLen && (uRegRd < uRd || uRegWr < uWr)) */

	pDL->bDone = (IEC_BOOL)(uRegRd == uRd && uRegWr == uWr);

	RETURN(OK);
}

#endif /* RTS_CFG_TASK_IMAGE */

/* ---------------------------------------------------------------------------- */
/**
 * actGetTask
 *
 */
IEC_UINT actGetTask(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, STaskInfoVM **ppVM, STask *pTSK, IEC_UINT uTasks)
{
	static IEC_UINT uTask = 0;

	XTask_21001 xTask;
	STask		*pTask = pTSK;

	if (pBlock == NULL)
	{
		uTask = 0;
		RETURN(OK);
	}

	pDL->bDone	= FALSE;
	pDL->bOther = FALSE;

	while (*upCount < pBlock->uLen && uTask < uObj)
	{
		IEC_UINT uRes = actDataDiv(pDL, pBlock, sizeof(XTask_21001), upCount, &xTask);
		if (uRes != OK)
		{
			uRes = (IEC_UINT)(uRes != WRN_DIVIDED ? uRes : OK);
			RETURN(uRes);
		}

		if (ppVM != NULL)
		{
			*ppVM = vmCreateTaskInfoVM(pDL->uObj, uTasks);

			if (*ppVM == NULL)
			{
				RETURN(ERR_OUT_OF_MEMORY);
			}

			pTask = &(*ppVM)->Task;
		}

		pTask->usAttrib 	= xTask.usAttrib;
		pTask->ulPara1		= xTask.ulPara1;
		pTask->ulPara2		= xTask.ulPara2;
		pTask->usPrograms	= xTask.usPrograms;
		pTask->usPriority	= osConvertVMPriority(xTask.usPriority);

		pTask->pIR->uRegionsRd	= xTask.uRegionsRd;
		pTask->pIR->uRegionsWr	= xTask.uRegionsWr;

		pTask->bWDogEnable		= (IEC_BOOL)(xTask.ulWatchDogMS != 0 ? TRUE : FALSE);
		pTask->bWDogOldEnable	= (IEC_BOOL)(xTask.ulWatchDogMS != 0 ? TRUE : FALSE);
		pTask->ulWDogCounter	= 0;
		pTask->ulWDogTrigger	= (xTask.ulWatchDogMS == -1 || xTask.ulWatchDogMS == 0) ?
									  WD_TRIGGER_DEFAULT : xTask.ulWatchDogMS;

	  #if defined(RTS_CFG_COPY_DOMAIN)
		pTask->uCpyRegions	= xTask.uCpyRegions;
		pTask->uCpyRegOff	= xTask.uCpyRegOff;
	  #endif

		OS_MEMCPY(pTask->szName, xTask.szName, VMM_MAX_IEC_IDENT);
		pTask->szName[VMM_MAX_IEC_IDENT - 1] = 0;

		if (pTask->usPrograms != 0 || pTask->pIR->uRegionsRd == 0 || pTask->pIR->uRegionsWr == 0)
		{
			/* Prepare for program index list 
			 */
			if (pTask->usPrograms > MAX_PROGRAMS)
			{
				RETURN(ERR_OVERRUN_PROGRAM);
			}

			if (pTask->pIR->uRegionsRd > MAX_READ_REGIONS || pTask->pIR->uRegionsWr > MAX_WRITE_REGIONS)
			{
				RETURN(ERR_OVERRUN_REGION);
			}

			pDL->bOther = TRUE;
		}

		uTask++;
	}

	pDL->bDone = (IEC_BOOL)(uTask == uObj);

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actCloseFile
 *
 */
#if defined(RTS_CFG_FILE_ACCESS) || defined(RTS_CFG_FILE_NATIVE)

IEC_UINT actCloseFile(IEC_UDINT *pFile)
{
	IEC_UINT uRes = OK;

	if (*pFile != 0)
	{
		uRes = xxxClose(*pFile);
		*pFile = 0;
	}

	RETURN(uRes);
}

#endif /* RTS_CFG_FILE_ACCESS || RTS_CFG_FILE_NATIVE */

/* ---------------------------------------------------------------------------- */

