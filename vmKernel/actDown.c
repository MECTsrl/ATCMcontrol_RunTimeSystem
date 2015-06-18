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
 * Filename: actDown.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"actDown.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

#include <math.h>

/* ----  Local Defines:   ----------------------------------------------------- */

#if defined(RTS_CFG_FILE_NATIVE)
	#define xxxWrite			fileWrite
	#define xxxWriteLine		fileWriteLine
#endif

#if defined(RTS_CFG_FILE_ACCESS)
	#define xxxWrite			osWrite
	#define xxxWriteLine		osWriteLine
#endif

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

static IEC_UINT actCreateObject(STaskInfoVMM *pVMM, SDownHeader *pHeader, IEC_UINT uMode);
static IEC_UINT actCheckDomain(IEC_UINT uCurrent, IEC_UINT uMode, IEC_UINT uDomain);
static IEC_UINT actGetObjectData(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT *upCount, SObject *pList);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * actDLBegin
 */
IEC_UINT actDLBegin(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uMode)
{
	IEC_UINT	i;
	SDLBuffer	*pDL	= &pVMM->DLB;

	if (pBlock == NULL)
	{
		pDL->uDomain = DOWN_NONE;

	  #if defined(RTS_CFG_ONLINE_CHANGE)
		
		if (pDL->bOCStarted == TRUE)
		{
			actOCCleanUp(pVMM);
		}		
	  #endif

		pDL->bOCStarted = FALSE;

		RETURN(OK);
	}

	/* Check if IO layer are ready...
	 */
  #if defined(RTS_CFG_IO_LAYER)
	{
		IEC_BOOL bBoot;
		IEC_UINT uRes = ioIsBootable(pVMM, &bBoot);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		if (bBoot == FALSE)
		{
			vmmQueueMessage(&pVMM->Shared.MsgQueue, 
				"[VMM]: < WARNING >  Download currently not possible. IO layer is still starting up!\n");
			RETURN(ERR_NOT_READY);
		}
	}
  #endif

	/* Logout all other users
	 */
	for (i = 0; i < MAX_CONNECTIONS; i++)
	{
		if (i != pBlock->usSource)
		{
			pVMM->pLogin[i] = FALSE;
		}
	}

	/* Store current resource state
	 */
	pDL->usResState = (IEC_USINT)(pVMM->ulResState == RES_STATE_PAUSED ? RES_STATE_PAUSED : RES_STATE_RUNNING);

	/* Download mode
	 */
	if (uMode == DOWN_MODE_FULL)
	{
		pDL->uMode = *(IEC_UINT *)pBlock->CMD.pData;
	}
	else
	{
		pDL->uMode = DOWN_MODE_WARM;
	}

	pVMM->bWarmStart = (IEC_BOOL)(((pDL->uMode & DOWN_MODE_WARM) != 0) ? TRUE : FALSE);

  #if defined(RTS_CFG_EVENTS)
	
	if ((pDL->uMode & DOWN_MODE_FLASH) == 0)
	{
		/* Raise event
		 */
		vmmSetEvent(pVMM, (IEC_UINT)(uMode == DOWN_MODE_FULL ? EVT_DOWNLOAD_BEGIN : EVT_ONLCHG_BEGIN));
		osSleep(50);
	}

  #endif

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actDLConfig
 */
IEC_UINT actDLConfig(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uMode, IEC_UINT uDomain)
{
	IEC_UINT uCount  = 0;
	IEC_UINT uRes	 = OK;

	SDLBuffer *pDL = &pVMM->DLB;
	
	if (pBlock == NULL)
	{
		actGetProjectInfo	(NULL, NULL, NULL, 0, NULL, NULL);
		actGetExtProjectInfo(NULL, NULL, NULL, 0, NULL);
		actGetSegments		(NULL, NULL, NULL, 0, NULL);
		actGetPrograms		(NULL, NULL, NULL, 0, NULL);
		actGetTask			(NULL, NULL, NULL, 0, NULL, NULL, 0);

	  #if defined(RTS_CFG_TASK_IMAGE)
		actGetImageRegions	(NULL, NULL, NULL, 0, 0, NULL, NULL);
	  #endif

	  #if defined(RTS_CFG_COPY_DOMAIN)
		actGetRetainList	(NULL, NULL, NULL, 0, NULL);
	  #endif
		
		actGetIOLayer		(NULL, NULL, NULL, 0, NULL);

		RETURN(OK);
	}
	else
	{
		uRes = actCheckDomain(pDL->uDomain, uMode, uDomain);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}


	/* Project Information
	 * ------------------------------------------------------------------------
	 */
	if (pBlock->uBlock == 1)
	{
		IEC_UDINT ulBinDLVersion = 0;

		if (uMode == DOWN_MODE_FULL)
		{
			uRes = actGetProjectInfo(pDL, pBlock, &uCount, 1, &pVMM->Project, &ulBinDLVersion);
		}
	  #if defined(RTS_CFG_ONLINE_CHANGE)
		else
		{
			SProject *pOldPrj = &pVMM->Project;
			SProject *pNewPrj = &pVMM->OnlChg.Project;

			uRes = actGetProjectInfo(pDL, pBlock, &uCount, 1, pNewPrj, &ulBinDLVersion);

			if (pDL->bDone == TRUE && ( 
					pOldPrj->uTasks 		!= pNewPrj->uTasks
			  #if defined(RTS_CFG_EXT_PROJ_INFO)
				||	pOldPrj->uProjInfo		!= pNewPrj->uProjInfo
			  #endif
			  #if defined(RTS_CFG_COPY_DOMAIN)
				||	pOldPrj->uCpyRegCOff	!= pNewPrj->uCpyRegCOff
				||	pOldPrj->uCpyRegConst	!= pNewPrj->uCpyRegConst
				||	pOldPrj->uCpyRegions	!= pNewPrj->uCpyRegions
			  #endif
				||	pOldPrj->ulIECOffset	!= pNewPrj->ulIECOffset
				||	pOldPrj->uIOLayer		!= pNewPrj->uIOLayer))
			{
				RETURN(ERR_OC_PROJ_CHANGED);
			}

			pVMM->OnlChg.uProjInfo++;
		}
	  #endif

		if (uRes == ERR_INVALID_VERSION)
		{
			vmmQueueMessage(&pVMM->Shared.MsgQueue, "\n");
			vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR*)pVMM->pBuffer, 
				"[VMM]: <* ERROR *>  Wrong Firmware version (%ld) received.\n", ulBinDLVersion));
			vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR*)pVMM->pBuffer, 
				"[VMM]: <* ERROR *>  Expected Firmware version: %ld\n", RTS_VERSION_COMPATIBLE));
		}

		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}


	/* Extended Project Information
	 * ------------------------------------------------------------------------
	 */
	if (uCount < pBlock->uLen)
	{
	  #if defined(RTS_CFG_EXT_PROJ_INFO)
		uRes = actGetExtProjectInfo(pDL, pBlock, &uCount, pVMM->Project.uProjInfo, &pVMM->ProjInfo);

		#if defined(RTS_CFG_PRJ_TRACE) & defined(RTS_CFG_DEBUG_OUTPUT)
			osTrace("\r\n--- PRJ: %s   %s - %s\r\n", pVMM->ProjInfo.szProjName, pVMM->ProjInfo.szProjVers, pVMM->ProjInfo.szOwner);
		#endif

	  #else
		uRes = actGetExtProjectInfo(pDL, pBlock, &uCount, 0, NULL);
	  #endif
		
		if (uRes != OK) 
		{
			RETURN(uRes);
		}
	}


	/* Task Information
	 * ------------------------------------------------------------------------
	 */
	while (uCount < pBlock->uLen && pDL->uObj < pVMM->Project.uTasks)
	{
		if (pDL->bSwitch == FALSE)
		{
			/* Task Definitions
			 * ----------------------------------------------------------------
			 */
			if (uMode == DOWN_MODE_FULL)
			{
				uRes = actGetTask(pDL, pBlock, &uCount, 1, pVMM->ppVM + pDL->uObj, NULL, pVMM->Project.uTasks);
			}
		  #if defined(RTS_CFG_ONLINE_CHANGE)
			else
			{
				STask *pOldTask = &pVMM->ppVM[pDL->uObj]->Task;
				STask *pNewTask = &pVMM->OnlChg.pTask[pDL->uObj].Task;

				uRes = actGetTask(pDL, pBlock, &uCount, 1, NULL, pNewTask, 0);

				if (pDL->bDone == TRUE && (
						pOldTask->usAttrib	!= pNewTask->usAttrib
					||	pOldTask->ulPara1	!= pNewTask->ulPara1
					||	pOldTask->ulPara2	!= pNewTask->ulPara2
					||	pOldTask->usPriority!= pNewTask->usPriority))
				{
					RETURN(ERR_OC_TASK_CHANGED);
				}
			}
		  #endif

			if (uRes != OK) 
			{
				RETURN(uRes);
			}

			if (pDL->bOther == TRUE)
			{
				pDL->bSwitch = TRUE;
			}
			else if (pDL->bDone == TRUE)
			{
				pDL->uObj++;
			}

			if (pDL->bDone == TRUE)
			{
				STask *pTask = &pVMM->ppVM[pDL->uObj]->Task;

				if ((pTask->usAttrib & VMM_TASK_ATTR_CYCLIC) != 0 && pTask->ulPara1 < VM_MIN_CYCLE_TIME)
				{
					vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR*)pVMM->pBuffer, 
						"[VMM]: < WARNING >  Cycle time increased from %dms to %dms for task '%s'!\n", pTask->ulPara1, VM_MIN_CYCLE_TIME, pTask->szName));

				  #if defined(RTS_CFG_DEBUG_OUTPUT)
					osTrace("--- VMM: *** WARNING *** Cycle time increased from %dms to %dms for task '%s'!\r\n", pTask->ulPara1, VM_MIN_CYCLE_TIME, pTask->szName);
				  #endif

					pTask->ulPara1 = VM_MIN_CYCLE_TIME;
				}

				if ((pTask->usAttrib & VMM_TASK_ATTR_EVENT_DRIVEN) != 0 && pTask->ulPara1 == 0 && pTask->ulPara2 == 0)
				{
					vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR*)pVMM->pBuffer, 
						"[VMM]: < WARNING >  No event specified for task '%s'!\n", pTask->szName));

				  #if defined(RTS_CFG_DEBUG_OUTPUT)
					osTrace("--- VMM: *** WARNING *** No event specified for task '%s'!\r\n", pTask->szName);
				  #endif
				}

				if ((pTask->usAttrib & VMM_TASK_ATTR_CYCLIC) == 0 && (pTask->usAttrib & VMM_TASK_ATTR_EVENT_DRIVEN) == 0)
				{
					vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR*)pVMM->pBuffer, 
						"[VMM]: < WARNING >  Task '%s' is neither cyclic nor event driven!\n", pTask->szName));

				  #if defined(RTS_CFG_DEBUG_OUTPUT)
					osTrace("--- VMM: *** WARNING *** Task '%s' is neither cyclic nor event driven!\r\n", pTask->szName);
				  #endif
				}

				actGetTask(NULL, NULL, NULL, 0, NULL, NULL, 0);
			}
		
		} /* if (pDL->bSwitch == FALSE) */ 
		else
		{
			/* Program Index List (for each task)
			 * ----------------------------------------------------------------
			 */
			if (uCount < pBlock->uLen)
			{
				if (uMode == DOWN_MODE_FULL)
				{
					STaskInfoVM *pVM = pVMM->ppVM[pDL->uObj];

					uRes = actGetPrograms(pDL, pBlock, &uCount, pVM->Task.usPrograms, pVM->Local.pProg);
				}
			  #if defined(RTS_CFG_ONLINE_CHANGE)
				else
				{
					SOCTask *pOCT = pVMM->OnlChg.pTask + pDL->uObj;

					uRes = actGetPrograms(pDL, pBlock, &uCount, pOCT->Task.usPrograms, pOCT->pProg);
				}
			  #endif

				if (uRes != OK) 
				{
					RETURN(uRes);
				}
			
			} /* if (uCount < pBlock->uLen) */

			/* Task Info Copy Regions (for each task)
			 * ----------------------------------------------------------------
			 */
		  #if defined(RTS_CFG_TASK_IMAGE)
			if (uCount < pBlock->uLen)
			{
				SImageReg *pIR = NULL;

				if (uMode == DOWN_MODE_FULL)
				{
					pIR = pVMM->ppVM[pDL->uObj]->Task.pIR;
				}
			  #if defined(RTS_CFG_ONLINE_CHANGE)
				else
				{
					pIR = pVMM->OnlChg.pTask[pDL->uObj].Task.pIR;
				}
			  #endif
					
				uRes = actGetImageRegions(pDL, pBlock, &uCount, pIR->uRegionsRd, pIR->uRegionsWr, pIR->pRegionRd, pIR->pRegionWr);
				if (uRes != OK) 
				{
					RETURN(uRes);
				}
			
			} /* if (uCount < pBlock->uLen) */
		  #endif

			if (pDL->bDone == TRUE)
			{
				/* Switch to memory regions
				 */
				pDL->uObj++;

				actGetPrograms(NULL, NULL, NULL, 0, NULL);
			  #if defined(RTS_CFG_TASK_IMAGE)
				actGetImageRegions(NULL, NULL, NULL, 0, 0, NULL, NULL); 
			  #endif

				pDL->bSwitch = FALSE;
			}

		} /* else (pDL->bSwitch == FALSE) */
	
	} /* while (uCount < pBlock->uLen && pDL->uObj < pVMM->Project.uTasks) */


	/* Global Memory Segments
	 * ------------------------------------------------------------------------
	 */
	if (uCount < pBlock->uLen)
	{
		if (uMode == DOWN_MODE_FULL)
		{
		  #if defined(RTS_CFG_FFO)
			pVMM->Shared.RetainInfo.ulOffset	= 0;
			pVMM->Shared.RetainInfo.uLastIndex	= NO_INDEX;

			uRes = actGetSegments(pDL, pBlock, &uCount, pVMM->Shared.pData, NULL);
		  #else
			uRes = actGetSegments(pDL, pBlock, &uCount, pVMM->Shared.pData, pVMM->Shared.pSegInfo);
		  #endif

			/* Create write flags in global process image
			 */
		  #if defined(RTS_CFG_WRITE_FLAGS_PI)
			
			if (pDL->bDone == TRUE)
			{
				pVMM->Shared.WriteFlags.ulSize = pVMM->Shared.pData[SEG_OUTPUT].ulSize;

				uRes = osInitializeSegment(SEG_WRITEF, &pVMM->Shared.WriteFlags);
				if (uRes != OK)
				{
					RETURN(uRes);
				}
			}

		  #endif

		  #if defined(RTS_CFG_EXT_RETAIN)

			if (pDL->bDone == TRUE)
			{
				/* Configure Retain Update Task
				 */
				SMessage Message;
				SRetExt  *pRE = (SRetExt *)Message.pData;

				pRE->uMode	= pDL->uMode;
				OS_MEMCPY(&pRE->Retain, pVMM->Shared.pData + SEG_RETAIN, sizeof(SObject));
				OS_MEMCPY(pRE->pProjectID, pVMM->pDownLoadGUID, VMM_GUID);

				TRACE_GUID("Ret-Cfg1", "DLD", pVMM->pDownLoadGUID);

				Message.uID 		= MSG_RT_OPEN;
				Message.uLen		= sizeof(SRetExt);
				Message.uRespQueue	= Q_RESP_VMM_RET;
				
				uRes = msgTXMessage(&Message, Q_LIST_RET, VMM_TO_IPC_MSG_LONG, TRUE);
				
				if (uRes == WRN_RETAIN_INVALID)
				{
					vmmQueueMessage(&pVMM->Shared.MsgQueue, 
						"[VMM]: < WARNING >  Retain memory doesn't match with project, executing a cold start!\n");

				  #if defined(RTS_CFG_DEBUG_OUTPUT)
					osTrace("--- VMM: *** WARNING *** Retain memory doesn't match project, executing a cold start!\r\n");
				  #endif

					pDL->uMode &= ~DOWN_MODE_WARM;
					pDL->uMode |= DOWN_MODE_COLD;

					pVMM->bWarmStart = FALSE;

					uRes = OK;
				}
				
				if (uRes != OK)
				{
					RETURN(uRes);
				}
			}

		  #else /* RTS_CFG_EXT_RETAIN */

			if (pDL->bDone == TRUE && (pDL->uMode & DOWN_MODE_WARM) != 0)
			{
				/* Check project GUID
				 */
				SObject *pRetain = pVMM->Shared.pData + SEG_RETAIN;

				TRACE_GUID("Ret-Cmp", "DLD", pVMM->pDownLoadGUID);
				TRACE_GUID("Ret-Cmp", "RET", pRetain->pAdr + (pRetain->ulSize - VMM_GUID));

				if (OS_MEMCMP(pVMM->pDownLoadGUID, pRetain->pAdr + (pRetain->ulSize - VMM_GUID), VMM_GUID) != 0)
				{
					vmmQueueMessage(&pVMM->Shared.MsgQueue, 
						"[VMM]: < WARNING >  Retain memory doesn't match with project, executing a cold start!\n");

				  #if defined(RTS_CFG_DEBUG_OUTPUT)
					osTrace("--- VMM: *** WARNING *** Retain memory doesn't match project, executing a cold start!\r\n");
				  #endif

					pDL->uMode &= ~DOWN_MODE_WARM;
					pDL->uMode |= DOWN_MODE_COLD;

					pVMM->bWarmStart = FALSE;
				}
			}

			if (pDL->bDone == TRUE)
			{
				/* Copy project GUID into retain segment
				 */
				SObject *pRetain = pVMM->Shared.pData + SEG_RETAIN;

				TRACE_GUID("Ret-Cfg2", "DLD", pVMM->pDownLoadGUID);

				OS_MEMCPY(pRetain->pAdr + (pRetain->ulSize - VMM_GUID), pVMM->pDownLoadGUID, VMM_GUID);
			}

		  #endif /* RTS_CFG_EXT_RETAIN */
		
		} /* if (uMode == DOWN_MODE_FULL) */
		
	  #if defined(RTS_CFG_ONLINE_CHANGE)
		else
		{
			uRes = actGetSegments(pDL, pBlock, &uCount, NULL, NULL);
		
		} /* else (uMode == DOWN_MODE_FULL) */
	  #endif

		if (uRes != OK) 
		{
			RETURN(uRes);
		}
	
	} /* if (uCount < pBlock->uLen) */


	/* Retain Copy Regions
	 * ------------------------------------------------------------------------
	 */
  #if defined(RTS_CFG_COPY_DOMAIN)
	if (uCount < pBlock->uLen)
	{
		if (uMode == DOWN_MODE_FULL)
		{
			uRes = actGetRetainList(pDL, pBlock, &uCount, pVMM->Project.uCpyRegions, pVMM->Shared.CopyRegions);
			pVMM->Shared.uCpyRegions = pVMM->Project.uCpyRegions;
		}
	  #if defined(RTS_CFG_ONLINE_CHANGE)
		else
		{
			uRes = actGetRetainList(pDL, pBlock, &uCount, pVMM->Project.uCpyRegions, NULL);
		}
	  #endif

		if (uRes != OK) 
		{
			RETURN(uRes);
		}
	}
  #endif

	
	/* Field bus channel information
	 * ------------------------------------------------------------------------
	 */

	if (uCount < pBlock->uLen)
	{
		if (uMode == DOWN_MODE_FULL)
		{
		  #if defined(RTS_CFG_IO_LAYER)
			uRes = actGetIOLayer(pDL, pBlock, &uCount, pVMM->Project.uIOLayer, pVMM->pIOLayer);
		  #else
			uRes = actGetIOLayer(pDL, pBlock, &uCount, pVMM->Project.uIOLayer, NULL);
		  #endif
		}
	  #if defined(RTS_CFG_ONLINE_CHANGE)
		else
		{
			uRes = actGetIOLayer(pDL, pBlock, &uCount, pVMM->Project.uIOLayer, NULL);
		}
	  #endif

		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	/* Check Result
	 * ------------------------------------------------------------------------
	 */
	if (uCount != pBlock->uLen)
	{
		/* We should have consumed the whole data block
		 */
		RETURN(ERR_INVALID_DATA);
	}

	if (pBlock->byLast == TRUE)
	{
		/* All configurations downloaded?
		 */
		if (pVMM->Project.uTasks != pDL->uObj)
		{
			RETURN(ERR_ENTRIES_MISSING);
		}
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * actDLCode
 */
IEC_UINT actDLCode	(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uMode, IEC_UINT uDomain, IEC_UINT uCode, IEC_INT iIncr)
{
	SDownHeader Header;

	IEC_UINT uCount 	= 0;
	IEC_UINT uRes		= OK;

	SDLBuffer *pDL		= &pVMM->DLB;
	SDLHeader *pDH		= &pVMM->DLB.HDR;

	SObject *pList		= pVMM->Shared.pCode;

	if (pBlock == NULL)
	{
	  #if defined(RTS_CFG_COMM_TRACE) && defined(RTS_CFG_OBJ_TRACE)
		osTrace("\r\n");
	  #endif

	  #if defined(RTS_CFG_ONLINE_CHANGE) && ! defined(RTS_CFG_FFO)
		if (uMode == DOWN_MODE_INCR)
		{
			/* Keep the original segment info, use copy for OC!
			 */
			OS_MEMCPY(&pVMM->OnlChg.pSegInfo[SEG_CODE], &pVMM->Shared.pSegInfo[SEG_CODE], sizeof(SSegInfo));
		}
	  #endif

		RETURN(OK);
	}
	else
	{
		uRes = actCheckDomain(pDL->uDomain, uMode, uDomain);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	if (pDL->uObj > uCode)
	{
		RETURN(ERR_OVERRUN_CLASS);
	}

	if (uMode == DOWN_MODE_FULL)
	{
		uRes = bpDeleteAllBreakpoints(pVMM);
		if(uRes != OK)
		{
			RETURN(uRes);
		}
	}

	while (uCount < pBlock->uLen && pDL->uObj < uCode)
	{
		if (pDL->bSwitch == FALSE)
		{
			/* Download header
			 */
			uRes = actGetDownHeader(pDL, pBlock, &uCount, &Header);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			if (pDL->bDone == TRUE)
			{
				if (Header.uIndex >= pVMM->Project.uCode + iIncr)
				{
					/* Entries not consistent with downloaded configuration
					 */
					RETURN(ERR_INVALID_CLASS);
				}

				if (Header.uSegment != SEG_CODE)
				{
					RETURN(ERR_INVALID_SEGMENT);
				}

			  #if defined(RTS_CFG_ONLINE_CHANGE)
				if (uMode == DOWN_MODE_INCR)
				{
					/* Translate and store the object index
					 */
					pVMM->OnlChg.pCodeInd[pDL->uObj] = Header.uIndex;
					Header.uIndex = (IEC_UINT)(pVMM->Project.uCode + LIST_OFF(iIncr) + pDL->uObj);

					if (Header.uIndex >= MAX_CODE_OBJ)
					{
						RETURN(ERR_OVERRUN_CLASS);
					}
				}
			  #endif

				uRes = actCreateObject(pVMM, &Header, uMode);
				if (uRes != OK)
				{
					RETURN(uRes);
				}
				
				pDH->uIndex = Header.uIndex;

				if (pList[pDH->uIndex].ulSize != 0)
				{
					/* Prepare for code download
					 */
					pDH->ulSize 	= 0;
					pDH->uSegment	= Header.uSegment;

					pDL->bSwitch	= TRUE;
				}
				else
				{
					/* Empty object, continue with next header
					 */
					pDL->uObj++;
				}

			} /* if (pDL->bDone == TRUE) */
			
		} /* if (pDL->bSwitch == FALSE) */
		else
		{
			/* Download code
			 */
			uRes = actGetObjectData(pVMM, pBlock, &uCount, pList);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

		} /* else (pDL->bSwitch == FALSE) */
	
	} /* while (uCount < pBlock->uLen && pDL->uObj < uCode) */


	if (uCount != pBlock->uLen)
	{
		RETURN(ERR_INVALID_DATA);
	}

	if (pBlock->byLast == TRUE && uCode != pDL->uObj)
	{
		RETURN(ERR_ENTRIES_MISSING);
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * actDLInit
 */
IEC_UINT actDLInit(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uMode, IEC_UINT uDomain, IEC_UINT uData, IEC_UINT uDirVars, IEC_INT iIncr)
{
	SDownHeader Header;
	SDownDirect Direct;

	IEC_UINT uToCopy	= 0;
	IEC_UINT uCount 	= 0;
	IEC_UINT uRes		= OK;
	
	SDLBuffer *pDL		= &pVMM->DLB;
	SDLHeader *pDH		= &pVMM->DLB.HDR;
	
	SObject  *pList 	= pVMM->Shared.pData;

	if (pBlock == NULL)
	{
	  #if defined(RTS_CFG_COMM_TRACE) && defined(RTS_CFG_OBJ_TRACE)
		osTrace("\r\n");
	  #endif

	  #if defined(RTS_CFG_ONLINE_CHANGE)
		if (uMode == DOWN_MODE_INCR)
		{
			/* Keep the original segment info, use copy for OC!
			 */
			SIntShared *pShared = &pVMM->Shared;

		  #if ! defined(RTS_CFG_FFO)
			OS_MEMCPY(pVMM->OnlChg.pSegInfo, pShared->pSegInfo, MAX_SEGMENTS * sizeof(SSegInfo));
		  #else
			OS_MEMCPY(&pVMM->OnlChg.RetainInfo, &pShared->RetainInfo, sizeof(SSegInfo));
		  #endif
		}
	  #endif

		RETURN(OK);
	}
	else
	{
		uRes = actCheckDomain(pDL->uDomain, uMode, uDomain);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	if (pDL->uObj > uData)
	{
		RETURN(ERR_OVERRUN_INSTANCE);
	}


	/* Download Instance Objects
	 * -------------------------------------------------------------------------
	 */
	while (uCount < pBlock->uLen && pDL->uObj < uData)
	{
		if (pDL->bSwitch == FALSE)
		{
			/* Download header
			 */
			uRes = actGetDownHeader(pDL, pBlock, &uCount, &Header);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			if (pDL->bDone == TRUE)
			{
				if (Header.uIndex >= MAX_DATA_OBJ || Header.uIndex < MAX_SEGMENTS)
				{
					/* Entries not consistent with downloaded configuration
					 */
					RETURN(ERR_INVALID_INSTANCE);
				}
				
				if (Header.uSegment >= MAX_SEGMENTS)
				{
				   /* Entries not consistent with downloaded configuration
					*/
					RETURN(ERR_INVALID_INSTANCE);
				}

				if (Header.uSegment == SEG_RETAIN && uMode == DOWN_MODE_INCR)
				{
					/* Online Change is not supported for retentive variables
					 * and objects.
					 */
					RETURN(ERR_OC_RETAIN_CHANGED);
				}
				
				if (Header.byFixed != 0 && uMode == DOWN_MODE_FULL)
				{
					/* % - Addressed instance objects
					 * ----------------------------------------------------
					 * Position in memory is defined by the code generator.
					 * (Including any necessary alignment.)
					 */

					SObject *pData = pList + Header.uIndex;
					SObject *pSeg  = pList + Header.uSegment;

					if (Header.ulSize + Header.ulOffset > pSeg->ulSize)
					{
						/* Segment overflow
						 */
						RETURN(ERR_OVERRUN_OBJSEG);
					}
					
					pData->pAdr  = OS_ADDPTR(pSeg->pAdr, Header.ulOffset);
					pData->pAdr  = OS_NORMALIZEPTR(pData->pAdr);

					pData->ulSize = Header.ulSize;

				  #if defined(RTS_CFG_FFO)
					pVMM->Shared.pDataSeg[Header.uIndex] = (IEC_USINT)Header.uSegment;
				  #endif

					TR_FIXED(Header.uIndex, pData->ulSize, pData->pAdr, Header.uSegment);

				} /* if (Header.byFixed != 0 && uMode == DOWN_MODE_FULL) */

				else
				{
					/* Automatic instance objects
					 * ----------------------------------------------------
					 */
				  #if defined(RTS_CFG_ONLINE_CHANGE)
					if (uMode == DOWN_MODE_INCR)
					{
						/* Translate and store the object index
						 */
						pVMM->OnlChg.pDataInd[pDL->uObj] = Header.uIndex;
						Header.uIndex = (IEC_UINT)(MAX_SEGMENTS + pVMM->Project.uData + LIST_OFF(iIncr) + pDL->uObj);

						if (Header.uIndex >= MAX_DATA_OBJ)
						{
							RETURN(ERR_OVERRUN_INSTANCE);
						}

						if (Header.byFixed != 0)
						{
							/* Object is originally a % addressed object. Base address must be calculated
							 * already here.
							 */
							IEC_DATA OS_DPTR **ppData	= pVMM->OnlChg.ppDataAdr + pDL->uObj;
							SObject *pSeg				= pList + Header.uSegment;

							if (Header.ulSize + Header.ulOffset > pSeg->ulSize)
							{
								/* Segment overflow
								 */
								RETURN(ERR_OVERRUN_OBJSEG);
							}

							*ppData = OS_ADDPTR(pSeg->pAdr, Header.ulOffset);
							*ppData = OS_NORMALIZEPTR(*ppData);
							
						  #if defined(RTS_CFG_OC_TRACE)
							osTrace ("--- Fixed    Object: %2d  I:%02d  to be placed at 0x%08x  S:%d\r\n", pDL->uObj, Header.uIndex, *ppData, Header.ulSize);
						  #endif
						}
						else
						{
							pVMM->OnlChg.ppDataAdr[pDL->uObj] = NULL;
						}

					} /* if (uMode == DOWN_MODE_INCR) */
				  #endif

					uRes = actCreateObject(pVMM, &Header, uMode);
					if (uRes != OK)
					{
						RETURN(uRes);
					}
				
				} /* else (Header.byFixed != 0 && uMode == DOWN_MODE_FULL) */
				
				pDH->uIndex = Header.uIndex;

				if (pList[pDH->uIndex].ulSize != 0)
				{
					/* Prepare for initial value download
					 */
					pDH->ulSize 	= 0;
					pDH->uSegment	= Header.uSegment;

					pDL->bSwitch = TRUE;
				}
				else
				{
					/* Empty object, continue with next header
					 */
					pDL->uObj++;
				}
				
			} /* if (pDL->bDone == TRUE) */

		} /* if (pDL->bSwitch == FALSE) */
		else
		{
			/* Download initial value
			 */
			uRes = actGetObjectData(pVMM, pBlock, &uCount, pList);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

		} /* else (pDL->bSwitch == FALSE) */
	
	} /* while (uCount < pBlock->uLen && pDL->uObj < pVMM->Project.uObjects) */


	/* % Addressed Simple Variables
	 * ------------------------------------------------------------------------
	 */
	while (uCount < pBlock->uLen && pDL->uObj2 < uDirVars)
	{
		if (pDL->bSwitch == FALSE)
		{
			/* Download header
			 */
			uRes = actGetDownDirect(pDL, pBlock, &uCount, &Direct);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			if (pDL->bDone == TRUE)
			{
				if (Direct.uSegment >= MAX_SEGMENTS)
				{
				   /* Entries not consistent with downloaded configuration
					*/
					RETURN(ERR_INVALID_INSTANCE);
				}

				if (Direct.ulOffset + Direct.uSize / 8 > pVMM->Shared.pData[Direct.uSegment].ulSize)
				{
					/* Segment overflow
					 */
					RETURN(ERR_OVERRUN_OBJSEG);
				}

				/* Prepare for initial value download
				 */
				pDH->ulSize 		= 0;
				
				pDH->ulInitSize 	= Direct.uSize;
				pDH->uSegment		= Direct.uSegment;
				pDH->ulOffset		= Direct.ulOffset;
				pDH->uBit			= Direct.uBit;

				pDL->bSwitch = TRUE;
				
				TR_FIX_SIMP(pDH->ulInitSize / 8, pDH->ulInitSize % 8, pDH->uSegment, pDH->ulOffset, pDH->uBit);

			} /* if (uRes == OK) */

		} /* if (pDL->bDone == TRUE) */
		else
		{
			/* Download initial values
			 */
			IEC_DATA OS_DPTR *pVal = OS_ADDPTR(pVMM->Shared.pData[pDH->uSegment].pAdr, pDH->ulOffset);

			if (pDH->ulInitSize == 1)
			{
				/* Just a bit
				 */
				uRes = osNotifySetValue(pVMM, (IEC_DATA *)&pDH->uBit, pDH->uSegment, pDH->ulOffset, (IEC_UINT)pDH->ulInitSize, (IEC_USINT)(OS_LOG(pDH->uBit) / OS_LOG(2.0)));

				if (uRes == OK)
				{
					*pVal = (IEC_BYTE)((*pVal & ~pDH->uBit) | pDH->uBit);
				}
				else if (uRes == WRN_HANDLED)
				{
					uRes = OK;
				}

				uCount++;

				pDL->uObj2++;
				pDL->bSwitch = FALSE;
			}
			else
			{
				IEC_UDINT ulSize = pDH->ulInitSize / 8;
			
				if (pDH->ulSize + pBlock->uLen - uCount < ulSize)
				{
					/* Code continued in the next block
					 */
					uToCopy = (IEC_UINT)(pBlock->uLen - uCount);
				}
				else
				{
					/* Code completely contained in this block
					 */
					uToCopy = (IEC_UINT)(ulSize - pDH->ulSize);
				}

				uRes = osNotifySetValue(pVMM, pBlock->CMD.pData + uCount, pDH->uSegment, pDH->ulOffset + pDH->ulSize, uToCopy, 0);
				
				if (uRes == OK)
				{
					OS_MEMCPY(OS_ADDPTR(pVal, pDH->ulSize), pBlock->CMD.pData + uCount, uToCopy);
				}
				else if (uRes == WRN_HANDLED)
				{
					uRes = OK;
				}
	
				uCount		  = (IEC_UINT) (uCount + uToCopy);			
				pDH->ulSize  += uToCopy;

				if (pDH->ulSize >= ulSize)
				{
					pDL->uObj2++;
					pDL->bSwitch = FALSE;
				}
			
			} /* else (pDH->ulInitSize == 1) */

		} /* else (pDL->bSwitch == FALSE) */
	
	} /* while (uCount < pBlock->uLen && pDL->uObj2 < uDirVars) */


	if (uCount != pBlock->uLen)
	{
		RETURN(ERR_INVALID_DATA);
	}

	if (pBlock->byLast == TRUE && uData != pDL->uObj)
	{
		RETURN(ERR_ENTRIES_MISSING);
	}

	if (pBlock->byLast == TRUE && uDirVars != pDL->uObj2)
	{
		RETURN(ERR_ENTRIES_MISSING);
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * actDLFile
 */
#if defined(RTS_CFG_CUSTOM_DL) || defined(RTS_CFG_DEBUG_INFO)

IEC_UINT actDLFile(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uMode, IEC_UINT uDomain, 
				   IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szMap)
{
	IEC_UINT	uRes	= OK;
	IEC_UINT	uCount	= 0;

	SDLBuffer	*pDL	= &pVMM->DLB;
	SDLFile 	*pFIL	= &pVMM->DLB.FIL;

	if (pBlock == NULL)
	{
		pDL->HDR.ulSize = 0;
		
		actCloseFile(&pFIL->hF1);
		actCloseFile(&pFIL->hF2);

		RETURN(OK);
	}
	else
	{
		uRes = actCheckDomain(pDL->uDomain, uMode, uDomain);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	if (pBlock->uBlock == 1 && szMap != NULL)
	{
		/* Create map file
		 */
		uRes = utilCreateFile(&pFIL->hF2, (IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), fpGetDir, szDir, szMap);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	while (uCount < pBlock->uLen)
	{
		if (pDL->bSwitch == FALSE && pDL->bOther == FALSE)
		{
			/* Get file definition
			 */
			uRes = actGetFileDef(pDL, pBlock, &uCount, &pFIL->FD);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			pDL->bOther = pDL->bDone;
		}
		else if (pDL->bSwitch == FALSE && pDL->bOther == TRUE)
		{
			/* Get file name
			 */
			IEC_CHAR	szName[VMM_MAX_PATH];

			uRes = actGetFileName(pDL, pBlock, &uCount, pFIL->FD.uNameLen, szName);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			if (pDL->bDone == TRUE)
			{
				uRes = osCheckFilePath(szName, CMD_DOWNLOAD_CUSTOM);
				if (uRes != OK)
				{
					RETURN(OK);
				}

				if (pFIL->FD.ulDataLen == 0xfffffffful)
				{
					/* A directory
					 */
					uRes = utilCreateDir((IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), fpGetDir, szDir, szName);
					if (uRes != OK)
					{
						RETURN(uRes);
					}

					pDL->HDR.ulSize = 0;
					pDL->bSwitch	= FALSE;

				}
				else
				{
					/* A file
					 */
					if (pFIL->hF2 != 0)
					{
						/* Create map entry
						 */
						uRes = xxxWriteLine(pFIL->hF2, szName);
						if (uRes != OK)
						{
							RETURN(uRes);
						}

						uRes = xxxWriteLine(pFIL->hF2, "\r\n");
						if (uRes != OK)
						{
							RETURN(uRes);
						}
					}

					uRes = utilCreateFile(&pFIL->hF1, (IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), fpGetDir, szDir, szName);
					if (uRes != OK)
					{
						RETURN(uRes);
					}

					if (pFIL->FD.ulDataLen != 0)
					{
						pDL->HDR.ulSize = pFIL->FD.ulDataLen;
						pDL->bSwitch	= TRUE;
					}
					else
					{
						actCloseFile(&pFIL->hF1);
						pDL->bSwitch = FALSE;
					}
				}

				pDL->bOther = FALSE;

			} /* if (pDL->bDone == TRUE) */
		
		} /* if (pDL->bSwitch == FALSE && pDL->bOther == TRUE) */

		else
		{
			/* Get file data
			 */
			uRes = actGetFileData(pDL, pBlock, &uCount, pFIL->hF1);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			if (pDL->HDR.ulSize == 0)
			{
				actCloseFile(&pFIL->hF1);

				pDL->bSwitch = FALSE;
			}
			
		} /* else (pDL->bSwitch == FALSE && pDL->bOther == TRUE) */

	} /* while (uCount < pBlock->uLen) */

	if (uCount != pBlock->uLen)
	{
		RETURN(ERR_INVALID_DATA);
	}

	if (pBlock->byLast == TRUE && pFIL->hF1 != 0)
	{
		RETURN(ERR_INVALID_DATA);
	}

	if (pBlock->byLast == TRUE && pDL->HDR.ulSize != 0)
	{
		RETURN(ERR_INVALID_DATA);
	}

	if (pBlock->byLast == TRUE && pFIL->hF2 != 0)
	{
		actCloseFile(&pFIL->hF2);
	}

	RETURN(OK);
}

#endif /* RTS_CFG_CUSTOM_DL || RTS_CFG_DEBUG_INFO */

/* ---------------------------------------------------------------------------- */
/**
 * actDLFinish
 */
IEC_UINT actDLFinish(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uMode, IEC_UINT uDomain)
{
	IEC_UINT	uRes;
	SDLBuffer	*pDL	= &pVMM->DLB;

	if (pBlock == NULL)
	{
		RETURN(OK);
	}
	else
	{
		uRes = actCheckDomain(pDL->uDomain, uMode, uDomain);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	if (pBlock->uLen != VMM_GUID)
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	TRACE_GUID("DL-Finish", "PRJ", pBlock->CMD.pData);

	OS_MEMCPY(pVMM->pProjectGUID, pBlock->CMD.pData, VMM_GUID);

	/* Copy project GUID into retain segment
	 */
  #if defined(RTS_CFG_EXT_RETAIN)
	{
		SMessage Message;

		Message.uID 		= MSG_RT_SET_GUID;
		Message.uLen		= VMM_GUID;
		Message.uRespQueue	= Q_RESP_VMM_RET;

		TRACE_GUID("Ret-Cfg3", "DLD", pVMM->pDownLoadGUID);

		OS_MEMCPY(Message.pData, pVMM->pDownLoadGUID, VMM_GUID);

		uRes = msgTXMessage(&Message, Q_LIST_RET, VMM_TO_IPC_MSG, TRUE);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}
  #else
	{
		SObject *pRetain = pVMM->Shared.pData + SEG_RETAIN;

		TRACE_GUID("Ret-Cfg4", "DLD", pVMM->pDownLoadGUID);
		
		OS_MEMCPY(pRetain->pAdr + (pRetain->ulSize - VMM_GUID), pVMM->pDownLoadGUID, VMM_GUID);
	}

  #endif

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actCreateObject
 */
static IEC_UINT actCreateObject(STaskInfoVMM *pVMM, SDownHeader *pHeader, IEC_UINT uMode)
{
	SObject  *pList 	= NULL;

	if (pHeader->uSegment == SEG_CODE) 
	{
		pList = pVMM->Shared.pCode;
	}
	else
	{
		pList = pVMM->Shared.pData;
	}

	pList[pHeader->uIndex].ulSize = 0;
	pList[pHeader->uIndex].pAdr   = NULL;

  #if defined(RTS_CFG_FFO)
	if (pHeader->uSegment != SEG_RETAIN)
	{
		if (pHeader->ulSize != 0)
		{
			pList[pHeader->uIndex].pAdr = osMalloc(pHeader->ulSize);
			if (pList[pHeader->uIndex].pAdr == NULL)
			{
				RETURN(ERR_OUT_OF_MEMORY);
			}
		}

		pList[pHeader->uIndex].ulSize  = pHeader->ulSize;

		if (pHeader->uSegment != SEG_CODE)
		{
			/* Only for data objects!
			 */
			pVMM->Shared.pDataSeg[pHeader->uIndex] = (IEC_USINT)(pHeader->uSegment | MASC_ALLOCATED);
		}	
	}
	else
  #endif
	{
		SObject  *pSegment	= &pVMM->Shared.pData[pHeader->uSegment];
		SSegInfo *pSegInfo	= NULL;

		if (pHeader->ulSize != 0)
		{
		  #if ! defined(RTS_CFG_FFO)

			  #if defined(RTS_CFG_ONLINE_CHANGE)
				if (uMode == DOWN_MODE_INCR)
				{
					pSegInfo = pVMM->OnlChg.pSegInfo + pHeader->uSegment;
				}
				else
			  #endif
				{
					pSegInfo = pVMM->Shared.pSegInfo + pHeader->uSegment;
				}

		  #else /* ! RTS_CFG_FFO */

			  #if defined(RTS_CFG_ONLINE_CHANGE)
				if (uMode == DOWN_MODE_INCR)
				{
					pSegInfo = &pVMM->OnlChg.RetainInfo;
				}
				else
			  #endif
				{
					pSegInfo = &pVMM->Shared.RetainInfo;
				}

		  #endif /* RTS_CFG_FFO */

			if (pSegInfo->uLastIndex == NO_INDEX)
			{
				/* First entry in object list
				 */
				if (ALI(pHeader->ulSize) + ALI(pSegInfo->ulOffset) > pSegment->ulSize)
				{
					/* Segment overflow
					 */
					IEC_UINT uRes = (IEC_UINT)(pHeader->uSegment == SEG_CODE ? ERR_OVERRUN_CODESEG : ERR_OVERRUN_OBJSEG);
					RETURN(uRes);
				}

				pList[pHeader->uIndex].pAdr = OS_ADDPTR(pSegment->pAdr, ALI(pSegInfo->ulOffset));
				pList[pHeader->uIndex].pAdr = OS_NORMALIZEPTR(pList[pHeader->uIndex].pAdr);
			}
			else
			{
				/* Second and following entries in object list
				 */
				if (OS_SUBPTR(pList[pSegInfo->uLastIndex].pAdr, pSegment->pAdr) 
					+ ALI(pList[pSegInfo->uLastIndex].ulSize) + ALI(pHeader->ulSize) 
					> pSegment->ulSize)
				{
					/* Segment overflow
					 */
					IEC_UINT uRes = (IEC_UINT)(pHeader->uSegment == SEG_CODE ? ERR_OVERRUN_CODESEG : ERR_OVERRUN_OBJSEG);
					RETURN(uRes);
				}

				pList[pHeader->uIndex].pAdr = OS_ADDPTR(pList[pSegInfo->uLastIndex].pAdr,ALI(pList[pSegInfo->uLastIndex].ulSize));
				pList[pHeader->uIndex].pAdr = OS_NORMALIZEPTR(pList[pHeader->uIndex].pAdr );

			} /* if (pSegInfo->uLastIndex == NO_INDEX) */

			pList[pHeader->uIndex].ulSize	= pHeader->ulSize;
			pSegInfo->uLastIndex			= pHeader->uIndex;
		
		} /* if (pHeader->ulSize != 0) */
		
	  #if defined(RTS_CFG_FFO)
		pVMM->Shared.pDataSeg[pHeader->uIndex] = (IEC_USINT)(pHeader->uSegment);
	  #endif

	} /* else (pHeader->uSegment != SEG_RETAIN) */

	if (pHeader->uSegment == SEG_CODE)
	{
		TR_CODE(pHeader->uIndex, pHeader->ulSize, pList[pHeader->uIndex].pAdr);
	}
	else
	{
		TR_OBJECT(pHeader->uIndex, pHeader->ulSize, pList[pHeader->uIndex].pAdr, pHeader->uSegment);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actCheckDomain
 */
static IEC_UINT actCheckDomain(IEC_UINT uCurrent, IEC_UINT uMode, IEC_UINT uDomain)
{
	if ((uCurrent & 0xf000u) != (uMode == DOWN_MODE_INCR ? MASC_DOWN_INCR : MASC_DOWN_FULL))
	{
		RETURN(ERR_INVALID_DOMAIN);
	}

	if ((uCurrent & uDomain & 0x0fffu) == 0)
	{
		RETURN(ERR_INVALID_DOMAIN);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actGetObjectData
 */
IEC_UINT actGetObjectData(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT *upCount, SObject *pList)
{
	IEC_UINT	uToCopy;

	SDLBuffer	*pDL = &pVMM->DLB;
	SDLHeader	*pDH = &pVMM->DLB.HDR;

	if (pDH->ulSize + pBlock->uLen - *upCount < pList[pDH->uIndex].ulSize)
	{
		/* Object continued in the next block
		 */
		uToCopy = (IEC_UINT)(pBlock->uLen - *upCount);
	}
	else
	{
		/* Object completely contained in this block
		 */
		uToCopy = (IEC_UINT)(pList[pDH->uIndex].ulSize - pDH->ulSize);
	}

	if ( pDH->uSegment != SEG_RETAIN || 
		(pDH->uSegment == SEG_RETAIN && (pDL->uMode & DOWN_MODE_COLD) != 0))
	{
		OS_MEMCPY (OS_ADDPTR(pList[pDH->uIndex].pAdr, pDH->ulSize), pBlock->CMD.pData + *upCount, uToCopy);
	}
	else
	{
		/* Warm start: Skip initial values.
		 */
		;
	}

	*upCount	 = (IEC_UINT)(*upCount	  + uToCopy);			
	pDH->ulSize  = (IEC_UINT)(pDH->ulSize + uToCopy);

	if (pDH->ulSize >= pList[pDH->uIndex].ulSize)
	{
		/* Switch back to header download
		 */ 
		pDL->uObj++;
		pDL->bSwitch = FALSE;
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * actClearDownload
 */
IEC_UINT actClearDownload(STaskInfoVMM *pVMM, IEC_BYTE byCommand)
{
	switch (byCommand & ~0x80u)
	{
		case CMD_DOWNLOAD_CONFIG:
		case CMD_DOWNLOAD_INITIAL:
		case CMD_DOWNLOAD_CODE:
		case CMD_DOWNLOAD_CUSTOM:
		case CMD_DOWNLOAD_FINISH:
		case CMD_DOWNLOAD_END:
		case CMD_DOWNLOAD_BEGIN:
		case CMD_DOWNLOAD_DEBUG:
		case CMD_DOWNLOAD_IOL:
		case CMD_DOWNLOAD_CLEAR:
		case CMD_OC_BEGIN:
		case CMD_OC_CODE:
		case CMD_OC_DEBUG:
		case CMD_OC_CUSTOM:
		case CMD_OC_INITIAL:
		case CMD_OC_COMMIT:
		case CMD_OC_END:
		case CMD_OC_CONFIG:
		case CMD_OC_FINISH:
		{
			pVMM->DLB.uRecv 	= 0;
			pVMM->DLB.uObj		= 0;
			pVMM->DLB.uObj2 	= 0;
			pVMM->DLB.bSwitch	= FALSE;
		}	

	} /* switch (byCommand & ~0x80u) */

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */

