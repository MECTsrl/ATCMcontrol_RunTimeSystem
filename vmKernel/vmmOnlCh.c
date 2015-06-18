
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
 * Filename: vmmOnlCh.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__		"vmmOnlCh.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_ONLINE_CHANGE)

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * ocMain
 *
 */
IEC_UINT ocMain(void *pPara)
{
	STaskInfoVMM *pVMM = (STaskInfoVMM *)pPara;

	SMessage Message;
	IEC_UINT uRespQueue;

	IEC_UINT uRes = osCreateIPCQueue(Q_LIST_OC);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

  #if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo(TASK_OFFS_SYS_OCH, osGetTaskID());
	TR_RET(uRes);
  #endif
	
	for ( ; ; )
	{
		if (msgRecv(&Message, Q_LIST_OC, VMM_WAIT_FOREVER) != OK)
		{
			osSleep(100);
			continue;
		}

		uRespQueue			= Message.uRespQueue;
		Message.uRespQueue	= IPC_Q_NONE;

		switch(Message.uID)
		{
			case MSG_OC_PREPARE:	/* ---------------------------------------- */
			{
				if (uRespQueue != IPC_Q_NONE)
				{
					Message.uLen = 0;
					msgSend(&Message, uRespQueue);
				}
				break;
			}

			case MSG_OC_COMMIT: /* -------------------------------------------- */
			{
				IEC_BOOL bStopped	= FALSE;
				IEC_BOOL bBreak 	= FALSE;

				IEC_UINT uTask;

				IEC_UDINT ulTime	= osGetTime32();

				/* Wait for tasks to stop
				 */
				for ( ; bStopped == FALSE && bBreak == FALSE && osGetTime32() - ulTime < VMM_TO_IPC_MSG_LONG; )
				{
					bStopped = TRUE;

					for (uTask = 0; uTask < pVMM->Project.uTasks; uTask++)
					{
						IEC_UDINT ulState = pVMM->ppVM[uTask]->Local.pState->ulState;

						if (ulState != TASK_STATE_STOPPED && ulState != TASK_STATE_ERROR)
						{
							bStopped = FALSE;
							break;
						}
					}

					if (bStopped == FALSE)
					{
						/* Not all tasks stopped.
						 */
						SMessage Message;

						if (msgRecv(&Message, Q_LIST_OC, VMM_NO_WAIT) == OK)
						{
							/* Check for cancel signal from VMM
							 */
							uRespQueue = Message.uRespQueue;
							Message.uRespQueue = IPC_Q_NONE;

							if (Message.uID == MSG_OC_PREPARE)
							{
								Message.uLen = 0;
								msgSend(&Message, uRespQueue);

								bBreak = TRUE;
							}
						}

						osSleep(WAIT_OC_VMFINISH);

					} /* if (bStopped == FALSE) */

				} /* for (	) */

				if (bStopped == TRUE)
				{
					IEC_UINT uRes = ocApply(pVMM);
					
					if (uRes == OK)
					{
						Message.uID  = MSG_OC_COMMIT;
						Message.uLen = 0;
					}
					else
					{
						Message.uID  = MSG_OC_COMMIT | MSG_ERROR_FLAG;
						Message.uLen = sizeof(IEC_UINT);
						
						*(IEC_UINT *)Message.pData = uRes;
					}

					msgSend(&Message, uRespQueue);
				}

				if (bStopped == FALSE)
				{
					Message.uID  = MSG_OC_COMMIT | MSG_ERROR_FLAG;
					Message.uLen = sizeof(IEC_UINT);

					*(IEC_UINT *)Message.pData = WRN_TIME_OUT;
					
					msgSend(&Message, uRespQueue);
				}

				break;
			
			} /* case MSG_OC_COMMIT */

			default:			/* -------------------------------------------- */
			{
				TR_ERROR("[ocMain] Unexpected message (0x%04x) received.\r\n", Message.uID);
				
				if (uRespQueue != IPC_Q_NONE)
				{
					msgSetError(&Message, ERR_INVALID_MSG);
					msgSend(&Message, uRespQueue);
				}

				break;
			}

		} /* switch(Message.uID) */

	} /* for ( ; ; ) */

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * ocApply
 *
 */
IEC_UINT ocApply(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;

	SProject *pProj = &pVMM->Project;
	SOnlChg *pOC	= &pVMM->OnlChg;

	SObject *pCode	= pVMM->Shared.pCode;
	SObject *pData	= pVMM->Shared.pData;

	SObject *pSrc	= NULL;
	SObject *pDst	= NULL;

	IEC_UINT i;
	IEC_UINT j;

	IEC_UINT uLast	= 0xffffu;


  #if defined(RTS_CFG_OC_TRACE)
	osTrace("--- [ONLCHG] Tasks stopped, applying changes. (%ldms)\r\n\r\n", utilGetTimeDiffMS(pOC->ullTime));
  #endif


	/* Apply code changes
	 * ------------------------------------------------------------------------
	 */
	for (i = 0; i < pOC->uCode; i++)
	{
		IEC_UINT uSrc = (IEC_UINT)(pProj->uCode + LIST_OFF(pOC->iCodeDiff) + i);
		IEC_UINT uDst = (IEC_UINT)(pOC->pCodeInd[i]);

		if (pOC->pCodeInd[i] >= pProj->uCode + pOC->iCodeDiff)
		{
			RETURN(ERR_OC_INVALID_CODE);
		}

		pSrc = pCode + uSrc;
		pDst = pCode + uDst;

	  #if defined(RTS_CFG_OC_TRACE)
		osTrace ("--- Replace CodeObj: %02d -> %02d   0x%08x -> 0x%08x\r\n", uSrc, uDst, pSrc->pAdr, pDst->pAdr);
	  #endif

	  #if defined(RTS_CFG_FFO)

		uRes = osFree(&pDst->pAdr);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

	  #endif

		pDst->pAdr	 = pSrc->pAdr;
		pDst->ulSize = pSrc->ulSize;
	}

  #if defined(RTS_CFG_OC_TRACE)
	if (pOC->uCode != 0)
	{
		osTrace("\r\n");
	}
  #endif

	pProj->uCode = (IEC_UINT)(pProj->uCode + pOC->iCodeDiff);

	pOC->uCode		= 0;
	pOC->iCodeDiff	= 0;


	/* Copy unchanged data values
	 * ------------------------------------------------------------------------
	 */
	for (i = 0; i < pOC->uToCopy; i++)
	{
		SOCCopy *pOCC = pOC->pCopy + i;

		IEC_UINT uSrc = pOCC->uOld;
		IEC_UINT uDst = 0;

		/* Check index
		 */
		if (pOCC->uNew >= pProj->uData + MAX_SEGMENTS + pOC->iDataDiff)
		{
			RETURN(ERR_OC_INVALID_INST);
		}

		if (pOCC->uOld >= pProj->uData + MAX_SEGMENTS)
		{
			RETURN(ERR_OC_INVALID_INST);
		}

		if (uLast != pOCC->uNew)
		{
			/* Find current source index if changed. (The new object is still
			 * at the end of the data object list.)
			 */
			for (j = 0; j < pOC->uData; j++)
			{
				if (pOCC->uNew == pOC->pDataInd[j])
				{
					break;
				}
			}

			if (j == pOC->uData)
			{
				/* Uh-oh, copy list not consistent
				 */
				RETURN(ERR_OC_COPY_LIST);
			}

			/* Copy the old values into the new object, so source and destination is 
			 * reverted in this case!
			 */

			uSrc = pOCC->uOld;
			uDst = (IEC_UINT)(MAX_SEGMENTS + pProj->uData + LIST_OFF(pOC->iDataDiff) + j);

			pSrc = pData + uSrc;
			pDst = pData + uDst;

		  #if defined(RTS_CFG_OC_TRACE)
			osTrace ("--- Look-Up DataObj: %02d -> %02d [-> %02d] (%3d -> %3d)\r\n", 
						uSrc, uDst, pOCC->uNew, pSrc->ulSize, pDst->ulSize);
		  #endif

			uLast = pOCC->uNew;
		
		} /* if (uLast != pOCC->uNew) */

		/* Check offsets and sizes
		 */
		if (pOCC->uNewOff + pOCC->ulSize / 8u + (pOCC->ulSize % 8u != 0 ? 1u : 0u) > pDst->ulSize)
		{
			RETURN(ERR_OC_COPY_NEW);
		}

		if (pOCC->uOldOff + pOCC->ulSize / 8u + (pOCC->ulSize % 8u != 0 ? 1u : 0u) > pSrc->ulSize)
		{
			RETURN(ERR_OC_COPY_OLD);
		}

		if (pOCC->ulSize % 8 != 0 || pOCC->usOldBit != pOCC->usNewBit || pOCC->usOldBit != 0)
		{
		  #if defined(RTS_CFG_OC_TRACE)
			osTrace ("--- Backup  DataObj: 0x%08x >> 0x%08x  %3d.%01d from %3d.%d to %3d.%d\r\n", 
				pSrc->pAdr, pDst->pAdr, pOCC->ulSize / 8, pOCC->ulSize % 8, pOCC->uOldOff, pOCC->usOldBit, pOCC->uNewOff, pOCC->usNewBit);
		  #endif

			while(pOCC->ulSize != 0)
			{
				IEC_UINT uMask;
				IEC_UINT uToCopy = (IEC_UINT)(vmm_min(8u - pOCC->usOldBit, 8u - pOCC->usNewBit));

				uToCopy = (IEC_UINT)(vmm_min(uToCopy, pOCC->ulSize));

				uMask = (IEC_UINT)(utilPow2(uToCopy) - 1u);

				pDst->pAdr[pOCC->uNewOff] = (IEC_USINT)(( pDst->pAdr[pOCC->uNewOff] & ~(uMask << pOCC->usNewBit) ) 
							| ( ( (pSrc->pAdr[pOCC->uOldOff] >> pOCC->usOldBit) & uMask ) << pOCC->usNewBit));
				
				pOCC->ulSize -= uToCopy;
				
				pOCC->uOldOff  = (IEC_UINT)(pOCC->uOldOff + (pOCC->usOldBit + uToCopy) / 8);
				pOCC->uNewOff  = (IEC_UINT)(pOCC->uNewOff + (pOCC->usNewBit + uToCopy) / 8);

				pOCC->usOldBit = (IEC_USINT)((pOCC->usOldBit + uToCopy) % 8);
				pOCC->usNewBit = (IEC_USINT)((pOCC->usNewBit + uToCopy) % 8);
			}
		}
		else
		{
		  #if defined(RTS_CFG_OC_TRACE)
			osTrace ("--- Backup  DataObj: 0x%08x >> 0x%08x  %3d   from %3d   to %3d\r\n", 
				pSrc->pAdr, pDst->pAdr, pOCC->ulSize / 8, pOCC->uOldOff, pOCC->uNewOff);
		  #endif

			OS_MEMCPY(pDst->pAdr + pOCC->uNewOff, pSrc->pAdr + pOCC->uOldOff, pOCC->ulSize / 8);
		}

	} /* for (i = 0; i < pOC->uToCopy; i++) */

  #if defined(RTS_CFG_OC_TRACE)
	if (pOC->uToCopy != 0)
	{
		osTrace("\r\n");
	}
  #endif

	pOC->uToCopy = 0;


	/* Replace data objects
	 * ------------------------------------------------------------------------
	 */
	for (i = 0; i < pOC->uData; i++)
	{
		IEC_UINT uSrc = (IEC_UINT)(MAX_SEGMENTS + pProj->uData + LIST_OFF(pOC->iDataDiff) + i);
		IEC_UINT uDst = (IEC_UINT)(pOC->pDataInd[i]);

		if (pOC->pDataInd[i] >= pProj->uData + MAX_SEGMENTS + pOC->iDataDiff)
		{
			RETURN(ERR_OC_INVALID_INST);
		}

		pSrc = pData + uSrc;
		pDst = pData + uDst;

		if (pOC->ppDataAdr[i] == NULL)
		{
			/* Automatic objects
			 */
		  #if defined(RTS_CFG_OC_TRACE)
			osTrace ("--- Replace DataObj: %2d -> %2d   0x%08x -> 0x%08x\r\n", uSrc, uDst, pSrc->pAdr, pDst->pAdr);
		  #endif

		  #if defined(RTS_CFG_FFO)

			if (pDst->pAdr != NULL && (pVMM->Shared.pDataSeg[uDst] & MASC_ALLOCATED) != 0)
			{
				uRes = osFree(&pDst->pAdr);
				if (uRes != OK)
				{
					RETURN(uRes);
				}
			}

			pVMM->Shared.pDataSeg[uDst] = pVMM->Shared.pDataSeg[uSrc];
		  #endif

			pDst->pAdr	 = pSrc->pAdr;
			pDst->ulSize = pSrc->ulSize;
		}
		else
		{
			/* Direct (% addressed) objects
			 */
		  #if defined(RTS_CFG_OC_TRACE)
			osTrace ("--- Copy    DataObj: %2d >> io   0x%08x >> 0x%08x  S:%3d\r\n", uSrc, pSrc->pAdr, pOC->ppDataAdr[i], pSrc->ulSize);
		  #endif

			OS_MEMCPY(pOC->ppDataAdr[i], pSrc->pAdr, pSrc->ulSize);

		  #if defined(RTS_CFG_OC_TRACE)
			osTrace ("--- Replace DataObj: io -> %2d   0x%08x -> 0x%08x\r\n", uDst, pOC->ppDataAdr[i], pDst->pAdr);
		  #endif

			pDst->pAdr	 = pOC->ppDataAdr[i];
			pDst->ulSize = pSrc->ulSize;
			
		  #if defined(RTS_CFG_FFO)
			uRes = osFree(&pSrc->pAdr);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			pVMM->Shared.pDataSeg[uDst] = (IEC_USINT)(pVMM->Shared.pDataSeg[uSrc] & ~MASC_ALLOCATED);

		  #endif
		}
	
	} /* for (i = 0; i < pOC->uData; i++) */

  #if defined(RTS_CFG_OC_TRACE)
	if (pOC->uData != 0)
	{
		osTrace("\r\n");
	}
  #endif

	pProj->uData = (IEC_UINT)(pProj->uData + pOC->iDataDiff);

	pOC->uData		= 0;
	pOC->iDataDiff	= 0;


	/* Change program index list and copy regions
	 * ------------------------------------------------------------------------
	 */
	if(pOC->uTaskInfo != 0)
	{
		STaskInfoVM *pVM;
		SImageReg	*pIR_dst;
		SImageReg	*pIR_src;

	  #if defined(RTS_CFG_OC_TRACE)
		osTrace ("--- Task: Index list.\r\n");
	  #endif

		for (i = 0; i < pProj->uTasks; i++)
		{
			pVM = pVMM->ppVM[i];
			
			pIR_dst = pVM->Task.pIR;
			pIR_src = pOC->pTask[i].Task.pIR;

			OS_MEMCPY(pVM->Local.pProg, pOC->pTask[i].pProg, sizeof(SIndex) * pOC->pTask[i].Task.usPrograms);

			pVM->Task.usPrograms = pOC->pTask[i].Task.usPrograms;

		  #if defined(RTS_CFG_TASK_IMAGE)
			{		
				OS_MEMCPY(pIR_dst->pRegionRd, pIR_src->pRegionRd, sizeof(SRegion) * pIR_src->uRegionsRd);
				OS_MEMCPY(pIR_dst->pRegionWr, pIR_src->pRegionWr, sizeof(SRegion) * pIR_src->uRegionsWr);

				pIR_dst->uRegionsRd = pIR_src->uRegionsRd;
				pIR_dst->uRegionsWr = pIR_src->uRegionsWr;
		
			  #if defined(RTS_CFG_IO_LAYER)
				uRes = ioOptimizeLayer(pVMM);
				if (uRes != OK)
				{
					RETURN(uRes);
				}
			  #endif
			}			
		  #endif
		}
	}

	pOC->uTaskInfo = 0;


	/* Update project information
	 * ------------------------------------------------------------------------
	 */
	if(pOC->uProjInfo != 0)
	{
		pVMM->Project.ulStateVarOffs = pOC->Project.ulStateVarOffs;
	}

	pOC->uProjInfo = 0;
	

  #if defined(RTS_CFG_OC_TRACE)
	osTrace("\r\n--- [ONLCHG] Changes applied, starting tasks. (%ldms)\r\n", utilGetTimeDiffMS(pOC->ullTime));
  #endif

	RETURN(uRes);
}

#endif /* RTS_CFG_ONLINE_CHANGE */

/* ---------------------------------------------------------------------------- */
