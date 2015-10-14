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
 * Filename: ioMain.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"ioMain.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_IO_LAYER)

#include "iolDef.h"
#include "vmmMain.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

#if defined(_SOF_4CFC_SRC_) && defined(RTS_CFG_PROFI_DP)
  extern SIOConfig *g_pProfiBusDP_IO;
  #include "../inc.dp/dpMain.h"
#endif

IEC_BOOL g_bIOConfigFailed		= FALSE;
IEC_BOOL g_bIOConfInProgress	= FALSE;
  
/* ----  Local Functions:	--------------------------------------------------- */

IEC_UINT sysEnableIOLayer(IEC_CHAR *szIOLayer);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * ioCreateLayer
 *
 */
IEC_UINT ioCreateLayer(STaskInfoVMM *pVMM)
{
	IEC_UINT i;
	IEC_UINT uRes = OK;

	SIOLayer *pIOLayer;

  #if defined(RTS_CFG_IO_TRACE)
	if (pVMM->DLB.uRetry == 0 && pVMM->Project.uIOLayer != 0)
	{
		osTrace("\r\n");
	}
  #endif

	/* Enable IO layers
	 */
	for (i = 0; i < pVMM->Project.uIOLayer; i++)
	{
		uRes = sysEnableIOLayer(pVMM->pIOLayer[i].szIOLayer);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
		
		uRes = osEnableIOLayer(pVMM, i);
		if(uRes != OK)
		{
			RETURN(uRes);
		}
	}
	
	/* The VMM response queue...
	 */
	uRes = osCreateIPCQueue(Q_RESP_VMM_IO);
	if(uRes != OK)
	{
		RETURN(uRes);
	}

	/* Calculate notification optimization
	 */
	uRes = ioOptimizeLayer(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* Create IO driver application
	 */
	for (i = 0; i < pVMM->Project.uIOLayer; i++)
	{
		pIOLayer = pVMM->pIOLayer + i;

		if (pIOLayer->bEnabled == FALSE)
		{
			continue;
		}

		uRes = osCreateIOLayer(pVMM, i);
		if(uRes != OK)
		{
			pIOLayer->bCreated = FALSE;

			RETURN(uRes);
		}
		
		pIOLayer->bCreated = TRUE;
	}

	osSleep(250);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * ioTerminateLayer
 *
 */
IEC_UINT ioTerminateLayer(STaskInfoVMM *pVMM, IEC_BOOL bWarmStart)
{
	IEC_UINT uResLoc = OK;
	IEC_UINT uResAll = OK;

	SMessage Message;

	IEC_UINT i;

	for (i = 0; i < pVMM->Project.uIOLayer; i++)
	{
		SIOLayer *pIOLayer	= pVMM->pIOLayer + i;

		if (pIOLayer->bEnabled == FALSE || pIOLayer->bCreated == FALSE)
		{
			continue;
		}

		Message.uID 		= MSG_IO_TERMINATE;
		Message.uLen		= sizeof(IEC_BOOL);
		Message.uRespQueue	= Q_RESP_VMM_IO;

		*(IEC_BOOL *)Message.pData = bWarmStart;

		uResLoc = msgTXMessage(&Message, (IEC_UINT)(i + Q_OFFS_IO), VMM_TO_IPC_MSG_LONG, TRUE);
		TR_RET(uResLoc);

		uResAll = uResLoc;
	}

	RETURN(uResAll);
}

/* ---------------------------------------------------------------------------- */
/**
 * ioVerifyTermination
 *
 */
IEC_UINT ioVerifyTermination(STaskInfoVMM *pVMM, IEC_UDINT ulClearTime)
{
	IEC_UINT uRes = OK;

	IEC_UINT uResponse	= OK;
	IEC_BOOL bRetry 	= FALSE;
	IEC_UINT i;

	SMessage Message;
	SIOLayer *pIOLayer = NULL;

	for (i = 0; i < pVMM->Project.uIOLayer; i++)
	{
		pIOLayer = pVMM->pIOLayer + i;
		
		if (pIOLayer->bEnabled == FALSE || pIOLayer->bCreated == FALSE)
		{
			continue;
		}

		Message.uID 		= MSG_IO_TERM_RES;
		Message.uLen		= 0;
		Message.uRespQueue	= Q_RESP_VMM_IO;

		uRes = msgTXMessage(&Message, (IEC_UINT)(Q_OFFS_IO + i), VMM_TO_IPC_MSG, TRUE);
		if (uRes != OK)
		{
			pIOLayer->bCreated = FALSE;
			TR_RET(uRes);
		}
		
		if (Message.uLen != sizeof(IEC_UINT))
		{
			pIOLayer->bCreated = FALSE;
			TR_RET(ERR_INVALID_PARAM);
		}

		uResponse = *(IEC_UINT *)Message.pData;

		if (uResponse == WRN_IN_PROGRESS)
		{
			/* IO layer configuration still in progress; signal 
			 * another request to OPC server.
			 */
			bRetry = TRUE;
		}
		else
		{
			pIOLayer->bCreated = FALSE;
			TR_RET(uResponse);
		}

	} /* for (i = 0; i < pVMM->Project.uIOLayer; i++) */

	if (bRetry == TRUE)
	{
		if (osGetTime32() - ulClearTime > VMM_TO_IOL_CLEAR)
		{
			for (i = 0; i < pVMM->Project.uIOLayer; i++)
			{
				pIOLayer = pVMM->pIOLayer + i;

				if (pIOLayer->bEnabled == FALSE || pIOLayer->bCreated == FALSE)
				{
					continue;
				}

				vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR*)pVMM->pBuffer, 
					"[VMM]: <* ERROR *>  IO layer '%s' failed to terminate in time!\n", pIOLayer->szIOLayer));

			  #if defined(RTS_CFG_DEBUG_OUTPUT)
				osTrace("--- VMM: *** ERROR *** IO layer '%s' failed to terminate in time!\r\n", pIOLayer->szIOLayer);
			  #endif
			}

			RETURN(WRN_TIME_OUT);

		} /* if (osGetTime32() - ulClearTime > VMM_TO_IOL_CLEAR) */

		return WRN_IN_PROGRESS;

	} /* if (bRetry == TRUE) */

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * ioDestroyLayer
 *
 */
IEC_UINT ioDestroyLayer(STaskInfoVMM *pVMM)
{
	IEC_UINT i;
	IEC_UINT uRes = OK;

	/* Kill IO driver application
	 */
	for (i = 0; i < pVMM->Project.uIOLayer; i++)
	{
		if (pVMM->pIOLayer[i].bEnabled == FALSE)
		{
			continue;
		}

		/* ###TODO### BACnet IO layer must be killed!
		 */
		uRes = osKillIOLayer(pVMM, i);
		if(uRes != OK)
		{
			RETURN(uRes);
		}
	}

	/* The response queues...
	 */
	uRes = osDestroyIPCQueue(Q_RESP_VMM_IO);
	if(uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * ioConfigLayer
 *
 */
IEC_UINT ioConfigLayer(STaskInfoVMM *pVMM)
{
	IEC_UINT i;
	IEC_UINT uRes = OK;

	SDLBuffer *pDL		 = &pVMM->DLB;
	SIOConfig *pIOConfig = NULL;

	IEC_BOOL bRetry = FALSE;
	IEC_BOOL bAsync = FALSE;

	static IEC_UDINT ulConfigTime = 0;

	if (pVMM->Project.uIOLayer == 0)
	{		
		pDL->uRetry = 0;
		
		RETURN(OK);
	}
	
	if (pDL->uRetry == 0)
	{
		ulConfigTime = osGetTime32();

		for (i = 0; i < pVMM->Project.uIOLayer; i++)
		{
			SIOLayer *pIOLayer	= pVMM->pIOLayer + i;
			SMessage Message;

			if (pIOLayer->bEnabled == FALSE)
			{
				continue;
			}
			
			g_bIOConfInProgress = TRUE;

			Message.uID 		= MSG_IO_CONFIG;
			Message.uLen		= sizeof(SIOConfig);
			Message.uRespQueue	= Q_RESP_VMM_IO;

			pIOConfig = (SIOConfig *)Message.pData;

			OS_MEMSET(pIOConfig, 0x00, sizeof(SIOConfig));

			/* Normal configuration
			 */
			pIOConfig->I.ulOffs 	= pIOLayer->ulIOffs;
			pIOConfig->I.ulSize 	= pIOLayer->ulISize;
			pIOConfig->I.ulSegSize	= pVMM->Shared.pData[SEG_INPUT].ulSize;
			pIOConfig->I.pAdr		= pVMM->Shared.pData[SEG_INPUT].pAdr;

			pIOConfig->Q.ulOffs 	= pIOLayer->ulQOffs;
			pIOConfig->Q.ulSize 	= pIOLayer->ulQSize;
			pIOConfig->Q.ulSegSize	= pVMM->Shared.pData[SEG_OUTPUT].ulSize;
			pIOConfig->Q.pAdr		= pVMM->Shared.pData[SEG_OUTPUT].pAdr;

		  #if defined(RTS_CFG_MEMORY_AREA_EXPORT)
			pIOConfig->M.ulOffs 	= pIOLayer->ulMOffs;
			pIOConfig->M.ulSize 	= pIOLayer->ulMSize;
			pIOConfig->M.ulSegSize	= pVMM->Shared.pData[SEG_GLOBAL].ulSize;
			pIOConfig->M.pAdr		= pVMM->Shared.pData[SEG_GLOBAL].pAdr;
		  #endif

		  #if defined(RTS_CFG_WRITE_FLAGS_PI)
			pIOConfig->W.ulOffs 	= pIOLayer->ulQOffs;
			pIOConfig->W.ulSize 	= pIOLayer->ulQSize;
			pIOConfig->W.ulSegSize	= pVMM->Shared.pData[SEG_OUTPUT].ulSize;
			pIOConfig->W.pAdr		= pVMM->Shared.WriteFlags.pAdr;
		  #endif

		  #if defined(RTS_CFG_TASK_IMAGE)
			pIOConfig->C.ulOffs 	= 0;
			pIOConfig->C.ulSize 	= sizeof(SImageReg) * pVMM->Project.uTasks;
			pIOConfig->C.ulSegSize	= sizeof(SImageReg) * pVMM->Project.uTasks; 	
			if (pVMM->Project.uTasks == 0)
			{
				pIOConfig->C.pAdr	= NULL;
			}
			else
			{
				pIOConfig->C.pAdr	= (IEC_DATA *)pVMM->ppVM[0]->Task.pIR;
			}
		  #endif

		  #if defined(RTS_CFG_BACNET)
			if (pIOLayer->uIOLType == IOID_BACNET)
			{
				pIOConfig->R.ulOffs 	= 0;
				pIOConfig->R.ulSize 	= MAX_BACNET_OBJ * sizeof(IEC_UDINT);
				pIOConfig->R.ulSegSize	= MAX_BACNET_OBJ * sizeof(IEC_UDINT);

			  #if defined(RTS_CFG_EXT_RETAIN)
				pIOConfig->R.pAdr		= pVMM->Shared.pData[SEG_RETAIN].pAdr 
					+ (pVMM->Project.ulRetainUsed - ALI(MAX_BACNET_OBJ * sizeof(IEC_UDINT)));
			  #else
				pIOConfig->R.pAdr		= pVMM->Shared.pData[SEG_RETAIN].pAdr 
					+ (pVMM->Shared.pData[SEG_RETAIN].ulSize - VMM_GUID - ALI(MAX_BACNET_OBJ * sizeof(IEC_UDINT)));
			  #endif
			}
		  #endif

			pIOConfig->usChannel = pIOLayer->usChannel;

			OS_MEMCPY(pIOConfig->szName, pIOLayer->szIOLayer, VMM_MAX_IEC_IDENT);
			pIOConfig->szName[VMM_MAX_IEC_IDENT - 1] = 0;
						
			uRes = msgTXMessage(&Message, (IEC_UINT)(Q_OFFS_IO + i), VMM_TO_IPC_MSG, TRUE);
			if (uRes != OK)
			{
				g_bIOConfigFailed = TRUE;
				pIOLayer->uState  = IO_STATE_ERROR;			/* ###TODO### Error messages */

				RETURN(uRes);
			}

			pIOLayer->uState = IO_STATE_CONFIG;

		} /* for (i = 0; i < pVMM->Project.uIOLayer; i++) */

		osSleep(500);

	} /* if (pDL->uRetry == 0) */

	for (i = 0; i < pVMM->Project.uIOLayer; i++)
	{
		SIOLayer *pIOLayer	= pVMM->pIOLayer + i;
		SMessage Message;

		IEC_UINT uResponse = OK;

		bAsync |= pIOLayer->bAsyncConfig;

		if (pIOLayer->bEnabled == FALSE 
			|| pIOLayer->uState != IO_STATE_CONFIG
			|| pIOLayer->bAsyncConfig == TRUE)
		{
			continue;
		}

		Message.uID 		= MSG_IO_CONFIG_RES;
		Message.uLen		= 0;
		Message.uRespQueue	= Q_RESP_VMM_IO;

		uRes = msgTXMessage(&Message, (IEC_UINT)(Q_OFFS_IO + i), VMM_TO_IPC_MSG, TRUE);
		if (uRes != OK)
		{
			g_bIOConfigFailed = TRUE;
			pIOLayer->uState  = IO_STATE_ERROR;

			RETURN(uRes);
		}
		
		if (Message.uLen != sizeof(IEC_UINT))
		{
			g_bIOConfigFailed = TRUE;
			pIOLayer->uState  = IO_STATE_ERROR;

			RETURN(ERR_INVALID_PARAM);
		}

		uResponse = *(IEC_UINT *)Message.pData;

		if (uResponse == WRN_IN_PROGRESS)
		{
			/* IO layer configuration still in progress; signal 
			 * another request to OPC server.
			 */
			bRetry = TRUE;
		}
		else if (uResponse != OK)
		{
			g_bIOConfigFailed = TRUE;
			pIOLayer->uState  = IO_STATE_ERROR;

			RETURN(uResponse);
		}
		else
		{
			pIOLayer->uState = IO_STATE_OK;
		}
	
	} /* for (i = 0; i < pVMM->Project.uIOLayer; i++) */

	if (bRetry == TRUE)
	{
		if (osGetTime32() - ulConfigTime > VMM_TO_IOL_CONFIG)
		{
			for (i = 0; i < pVMM->Project.uIOLayer; i++)
			{
				SIOLayer *pIOLayer	= pVMM->pIOLayer + i;

				if (pIOLayer->bEnabled == FALSE)
				{
					continue;
				}

				if (pIOLayer->uState == IO_STATE_CONFIG)
				{
					pIOLayer->uState = IO_STATE_ERROR;

					vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR*)pVMM->pBuffer, 
						"[VMM]: <* ERROR *>  IO layer '%s' failed to configure in time!\n", pIOLayer->szIOLayer));

				  #if defined(RTS_CFG_DEBUG_OUTPUT)
					osTrace("--- VMM: *** ERROR *** IO layer '%s' failed to configure in time!\r\n", pIOLayer->szIOLayer);
				  #endif
				}
			}

			RETURN(WRN_TIME_OUT);
		}

		pDL->uRetry = (IEC_UINT)(pDL->uRetry + 1);
	}
	else
	{
		/* Finished, everything OK.
		 */
		pDL->uRetry = 0;

		g_bIOConfInProgress = bAsync;
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * ioStartLayer
 *
 */
IEC_UINT ioStartLayer(STaskInfoVMM *pVMM, IEC_BOOL bStart, IEC_BOOL bAlways)
{
	IEC_UINT i;
	IEC_UINT uRes = OK;
	IEC_UINT uID;

  #if defined(RTS_CFG_IPC_TRACE)
	IEC_BOOL bNL = TRUE;
  #endif

	for (i = 0; i < pVMM->Project.uIOLayer; i++)
	{
		SIOLayer *pIOLayer	= pVMM->pIOLayer + i;

		if (pIOLayer->bEnabled == FALSE || pIOLayer->bCreated == FALSE 
				|| pIOLayer->uState != IO_STATE_OK)
		{
			continue;
		}

		uID = (IEC_UINT)(bStart == TRUE ? MSG_IO_START : MSG_IO_STOP);

		if (pIOLayer->uIOLType != IOID_BACNET || bAlways == TRUE)
		{
			uRes = msgTXCommand(uID, (IEC_UINT)(i + Q_OFFS_IO), Q_RESP_VMM_IO, VMM_TO_IPC_MSG_LONG, TRUE);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
		}

	  #if defined(RTS_CFG_IPC_TRACE)
		bNL = FALSE;
	  #endif
	}
	
  #if defined(RTS_CFG_IPC_TRACE)
	if (bNL == TRUE)
	{
		osTrace("\r\n");
	}
  #endif	

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * ioIsBootable
 *
 */
IEC_UINT ioIsBootable(STaskInfoVMM *pVMM, IEC_BOOL *bpBoot)
{
	IEC_UINT uRes = OK;
	IEC_UINT i;

	*bpBoot = TRUE;

	for (i = 0; i < pVMM->Project.uIOLayer; i++)
	{
		SIOLayer *pIOLayer	= pVMM->pIOLayer + i;

		if (pIOLayer->bEnabled == FALSE || pIOLayer->bCreated == FALSE)
		{
			continue;
		}

		if (pIOLayer->bAsyncConfig == FALSE)
		{
			/* Configuration is done synchronous, we can
			 * always reboot.
			 */
			continue;
		}

		if (pIOLayer->uState == IO_STATE_CONFIG)
		{
			*bpBoot = FALSE;

			break;
		}
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * ioNotifyLayer
 *
 */
IEC_UINT ioNotifyLayer(STaskInfoVMM *pXXX, STaskInfoVM *pVM, IEC_BOOL bSet, IEC_UINT uSegment, IEC_UDINT ulOffset, IEC_UINT uLen, IEC_USINT usBit)
{
	IEC_UINT i;
	IEC_UINT uRes = OK;
	IEC_UINT uID;

	IEC_UDINT ulStart;
	IEC_UDINT ulStop;

	SIONotify *pIONotify;

	STaskInfoVMM *pVMM = pXXX;
	SMessage Message;

	if (pVMM == NULL && pVM == NULL)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	if (pVMM == NULL)
	{
		pVMM = (STaskInfoVMM *)pVM->pShared->pVMM;
	}

	for (i = 0; i < pVMM->Project.uIOLayer; i++)
	{
		SIOLayer *pIO = pVMM->pIOLayer + i;

		if (pIO->bEnabled == FALSE || pIO->bCreated == FALSE || pIO->uState != IO_STATE_OK
			|| ((bSet == TRUE ? pIO->usNotifyWr : pIO->usNotifyRd) & IO_NOTIFY) == 0)
		{
			continue;
		}

		if (pVM != NULL && bSet == TRUE  && pVM->Task.pIR->pSetQ[i] == FALSE)
		{
			continue;
		}

		if (pVM != NULL && bSet == FALSE && pVM->Task.pIR->pGetI[i] == FALSE && pVM->Task.pIR->pGetQ[i] == FALSE)
		{
			continue;
		}

		if (pVM == NULL)
		{
			/* Notification for an external changed or requested (i.e. 4C Watch) variable
			 */
			if (uSegment == SEG_OUTPUT)
			{
				ulStart = vmm_max(ulOffset, pIO->ulQOffs);
				ulStop	= vmm_min(ulOffset + uLen, pIO->ulQOffs + pIO->ulQSize);
			}
			else if (uSegment == SEG_INPUT)
			{
				ulStart = vmm_max(ulOffset, pIO->ulIOffs);
				ulStop	= vmm_min(ulOffset + uLen, pIO->ulIOffs + pIO->ulISize);
			}
			else
			{
				continue;
			}
		
			if (ulStart >= ulStop)
			{
				/* Changed or requested area doesn't match with this IO layer
				 */
				continue;
			}

		} /* if (pVM == NULL) */

		uID 				= (IEC_UINT)(bSet == TRUE ? MSG_IO_NOTIFY_SET : MSG_IO_NOTIFY_GET);
		Message.uID 		= uID;
		Message.uLen		= sizeof(SIONotify);
		Message.uRespQueue	= (IEC_UINT)(((bSet == TRUE ? pIO->usNotifyWr : pIO->usNotifyRd) & IO_NOTIFY_SYNC) != 0 ? Q_RESP_VMM_IO : IPC_Q_NONE);

		pIONotify = (SIONotify *)Message.pData;

		if (pVM == NULL)
		{
			pIONotify->uTask = 0xffffu;
		}
		else
		{
			pIONotify->uTask = pVM->usTask;
		}

		pIONotify->usSegment	= (IEC_USINT)uSegment;
		pIONotify->ulOffset 	= ulOffset;
		pIONotify->uLen 		= uLen;
		pIONotify->usBit		= usBit;

	  #if defined(_SOF_4CFC_SRC_) && defined(RTS_CFG_PROFI_DP)
		if (pIO->uIOLType == IOID_PROFIDP)
		{
			uRes = bSet  == TRUE ? dpNotifySet(i, g_pProfiBusDP_IO, pIONotify) 
								 : dpNotifyGet(i, g_pProfiBusDP_IO, pIONotify);
		}
		else
		{
	  #endif

		uRes = msgTXMessage(&Message, (IEC_UINT)(i + Q_OFFS_IO), VMM_TO_IPC_MSG, TRUE);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		if (Message.uID != uID)
		{
			RETURN(ERR_UNEXPECTED_MSG);
		}

	  #if defined(_SOF_4CFC_SRC_) && defined(RTS_CFG_PROFI_DP)
		}
	  #endif
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * ioOptimizeLayer
 *
 */
IEC_UINT ioOptimizeLayer(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;

  #if defined(RTS_CFG_TASK_IMAGE)

	IEC_UDINT ulStart;
	IEC_UDINT ulStop;

	IEC_UINT io;
	
	for (io = 0; io < pVMM->Project.uIOLayer; io++)
	{
		/* For each IO layer...
		 */
		SIOLayer *pIO = pVMM->pIOLayer + io;

		IEC_UINT tsk;
		
		for (tsk = 0; tsk < pVMM->Project.uTasks; tsk++)
		{
			/* For each task...
			 */
			SImageReg	*pIR = pVMM->ppVM[tsk]->Task.pIR;

			IEC_UINT	reg;

			pIR->pGetI[io] = FALSE;
			pIR->pGetQ[io] = FALSE;
			pIR->pSetQ[io] = FALSE;

			for (reg = 0; reg < pIR->uRegionsWr; reg++)
			{
				/* For each write region...
				 */
				SRegion *pReg = pIR->pRegionWr + reg;

				ulStart = vmm_max(pReg->ulOffset, pIO->ulQOffs);
				ulStop	= vmm_min(pReg->ulOffset + pReg->uSize, pIO->ulQOffs + pIO->ulQSize);					
				
				if (ulStart < ulStop)
				{
					/* Areas do match
					 */
					pIR->pSetQ[io]	= TRUE; 		
					pReg->pSetQ[io] = TRUE;
				}
				else
				{
					pReg->pSetQ[io] = FALSE;
				}
				
			} /* for (reg = 0; reg < pIR->uRegionsWr; reg++) */

			for (reg = 0; reg < pIR->uRegionsRd; reg++)
			{
				/* For each output read region...
				 */
				SRegion *pReg = pIR->pRegionRd + reg;

				if (pReg->usSegment != SEG_OUTPUT)
				{
					continue;
				}

				ulStart = vmm_max(pReg->ulOffset, pIO->ulQOffs);
				ulStop	= vmm_min(pReg->ulOffset + pReg->uSize, pIO->ulQOffs + pIO->ulQSize);					
				
				if (ulStart < ulStop)
				{
					/* Areas do match
					 */
					pIR->pGetQ[io]	= TRUE; 		
					pReg->pGetQ[io] = TRUE;
				}
				else
				{
					pReg->pGetQ[io] = FALSE;
				}
			
			} /* for (reg = 0; reg < pIR->uRegionsRd; reg++) */

			for (reg = 0; reg < pIR->uRegionsRd; reg++)
			{
				/* For each input read region...
				 */
				SRegion *pReg = pIR->pRegionRd + reg;

				if (pReg->usSegment != SEG_INPUT)
				{
					continue;
				}

				ulStart = vmm_max(pReg->ulOffset, pIO->ulIOffs);
				ulStop	= vmm_min(pReg->ulOffset + pReg->uSize, pIO->ulIOffs + pIO->ulISize);					
				
				if (ulStart < ulStop)
				{
					/* Areas do match
					 */
					pIR->pGetI[io]	= TRUE; 		
					pReg->pGetI[io] = TRUE;
				}
				else
				{
					pReg->pGetI[io] = FALSE;
				}
			
			} /* for (reg = 0; reg < pIR->uRegionsRd; reg++) */

		} /* for (tsk = 0; tsk < pVMM->Project.uTasks; tsk++) */

	} /* for (io = 0; io < pVMM->Project.uIOLayer; io++) */

  #endif /* RTS_CFG_TASK_IMAGE */

	RETURN(uRes);
}

#endif /* RTS_CFG_IO_LAYER */

/* ---------------------------------------------------------------------------- */

