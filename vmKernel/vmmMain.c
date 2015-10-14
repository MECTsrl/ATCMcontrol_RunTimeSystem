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
 * Filename: vmmMain.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__		"vmmMain.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"
#include "fcDef.h"

#if defined(RTS_CFG_ALI_TRACE)
#include "libIec.h"
#endif

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

static IEC_CHAR *g_szExc[]	= EXCEPTION_TEXT;

extern IEC_BOOL g_bIOConfigFailed;
extern IEC_BOOL g_bIOConfInProgress;

/* ----  Local Functions:	--------------------------------------------------- */

static IEC_UINT 	 vmmInitialize(STaskInfoVMM **ppVMM);
static STaskInfoVMM *vmmCreateTaskInfoVMM(void);

static IEC_UINT vmmRestart(STaskInfoVMM *pVMM);
static IEC_UINT vmmReadFlash(STaskInfoVMM *pVMM);
#if defined(RTS_CFG_ALI_TRACE)
static IEC_UINT vmmCheckAlignment(STaskInfoVMM *pVMM);
#endif

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * vmmMain
 *
 * Main function for the Virtual Machine Manager (VMM) task.
 *
 * To be used only for multi task operating systems not supporting 
 * interprocess communication.
 */
#if ! defined(RTS_CFG_MULTILINK)

IEC_UINT vmmMainMT(void)
{
	STaskInfoVMM *pVMM = NULL;

	IEC_UINT uRes = vmmInitialize(&pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	for ( ; ;)
	{
		/* Communication and Command Handling
		 */
#if defined(RTS_CFG_PROT_BLOCK)
		if (comBlock(pVMM) != OK)
		{
			osSleep(100);
		}
#endif

#if defined(RTS_CFG_PROT_CLIENT)
		if (comClient(pVMM) != OK)
		{
			osSleep(100);
		}
#endif

		if (pVMM->bProjLoaded == TRUE)
		{
			/* Refresh the resource state
			 */
			*(IEC_UDINT OS_DPTR *)(pVMM->Shared.pData[MAX_SEGMENTS].pAdr + pVMM->Project.ulStateVarOffs) 
				= pVMM->ulResState;
		}

#if defined(RTS_CFG_FLASH)
		if (pVMM->bExecColdStart == TRUE || pVMM->bExecWarmStart == TRUE)
		{
			vmmRestart(pVMM);
		}
#endif

	} /* for ( ; ; ) */

	RETURN(OK);
}
#endif /* ! RTS_CFG_MULTILINK */

/* ---------------------------------------------------------------------------- */
/**
 * vmmMain
 *
 * Main function for the Virtual Machine Manager (VMM) task.
 *
 * To be used only for multi task operating systems supporting inter
 * process communication.
 */
#if defined(RTS_CFG_VM_IPC) && defined(RTS_CFG_MULTILINK)

IEC_UINT vmmMainIPC(void)
{
	STaskInfoVMM *pVMM = NULL; 
	SMessage Message;
	IEC_UINT uRespQueue;

	osPthreadSetSched(FC_SCHED_VMM, FC_PRIO_VMM);
	IEC_UINT uRes = vmmInitialize(&pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	for ( ; ; ) 
	{
		if (msgRecv(&Message, Q_LIST_VMM, VMM_WAIT_FOREVER) != OK)
		{
			osSleep(100);
			continue;
		}

		uRespQueue			= Message.uRespQueue;
		Message.uRespQueue	= IPC_Q_NONE;

		switch (Message.uID)
		{
			case MSG_CT_DATA:	/* -------------------------------------------- */
				{
					XBlock *pBlock = (XBlock *)Message.pData;

#if defined(DEBUG)
					cmdBeforeExecute(pVMM, pBlock);
#endif

					cmdExecute(pVMM, pBlock);

#if defined(DEBUG)
					cmdAfterExecute(pVMM, pBlock);
#endif

					if (uRespQueue != IPC_Q_NONE)
					{
						Message.uLen = (IEC_UINT)(pBlock->uLen + HD_BLOCK);
						msgSend(&Message, uRespQueue);
					}

					break;
				}

			case MSG_CT_TERM:	/* -------------------------------------------- */
				{
					/* A Communication task terminates itself.
					 */
					if (Message.uLen == sizeof(IEC_UINT))
					{
						IEC_UINT uTask = *(IEC_UINT *)Message.pData;
						if (uTask < MAX_CONNECTIONS)
						{
							osOnCommTerminate(pVMM, uTask);
						}
					}

					if (uRespQueue != IPC_Q_NONE)
					{
						Message.uLen = 0;
						msgSend(&Message, uRespQueue);
					}
					break;
				}

#if defined(RTS_CFG_FLASH)
			case MSG_VM_COLDSTART:	/* ---------------------------------------- */
				{
					pVMM->bExecColdStart = TRUE;

					if (uRespQueue != IPC_Q_NONE)
					{
						Message.uLen = 0;
						msgSend(&Message, uRespQueue);
					}

					break;
				}
#endif

#if defined(RTS_CFG_FLASH)
			case MSG_VM_WARMSTART:	/* ---------------------------------------- */
				{
					pVMM->bExecWarmStart = TRUE;

					if (uRespQueue != IPC_Q_NONE)
					{
						Message.uLen = 0;
						msgSend(&Message, uRespQueue);
					}

					break;
				}
#endif

			case MSG_VM_REBOOT: /* -------------------------------------------- */
				{
					uRes = osReboot();

					if (uRespQueue != IPC_Q_NONE)
					{
						if (uRes != OK)
						{
							msgSetError(&Message, uRes);
						}
						else
						{
							Message.uLen = 0;
						}
						msgSend(&Message, uRespQueue);
					}

					break;
				}

#if defined(RTS_CFG_IO_LAYER)

			case MSG_IO_DONE: /* -------------------------------------- */
				{
					SIODone *pDone = (SIODone *)Message.pData;

					if (Message.uLen != sizeof(SIODone) || pDone->uIOLayer >= MAX_IO_LAYER)
					{
						break;
					}

					if (pDone->uRes == OK)
					{
#if defined(RTS_CFG_IO_TRACE)
						osTrace("--- IO%d: Configuration finished: Channel:%d Name:%s\r\n", pDone->uIOLayer, pDone->usChannel, pDone->szName);
#endif

						vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR*)pVMM->pBuffer, 
									"[VMM]: <--------->  Configuration - Channel %d (%s) finished\n", pDone->usChannel, pDone->szName));
					}
					else
					{
#if defined(RTS_CFG_IO_TRACE)
						osTrace("--- IO%d: ERROR: Configuration failed: Channel:%d Name:%s\r\n", pDone->uIOLayer, pDone->usChannel, pDone->szName);
#endif

						vmmQueueMessage(&pVMM->Shared.MsgQueue, utilFormatString((IEC_CHAR*)pVMM->pBuffer, 
									"[VMM]: <* ERROR *>  Configuration - Channel %d (%s) failed: 0x%04x\n", pDone->usChannel, pDone->szName, pDone->uRes));
					}

					if (pDone->uRes == OK)
					{
						pDone->uRes = msgTXCommand(MSG_IO_START, (IEC_UINT)(pDone->uIOLayer + Q_OFFS_IO), Q_RESP_VMM_IO, VMM_TO_IPC_MSG_LONG, TRUE);
					}

#if defined(RTS_CFG_BACNET)

					pVMM->uResConfigBACnet = pDone->uRes;

					uRes = vmmSetEvent(pVMM, EVT_BACNET_CONFIG);
					TR_RET(uRes);

#endif

					pVMM->pIOLayer[pDone->uIOLayer].uState = (IEC_UINT)(pDone->uRes == OK ? IO_STATE_OK : IO_STATE_ERROR);

					g_bIOConfInProgress  = FALSE;
					g_bIOConfigFailed	|= (pDone->uRes == OK ? FALSE : TRUE);

					break;
				}

			case MSG_IO_EVENT:	/* -------------------------------------------- */
				{
					if (Message.uLen == sizeof(IEC_UINT))
					{
						vmmSetEvent(pVMM, *(IEC_UINT *)Message.pData);
					}

					break;
				}

#endif /* RTS_CFG_IO_LAYER */

			default:	/* ---------------------------------------------------- */
				{
					TR_ERROR("[vmmMain] Unexpected message (0x%04x) received.\r\n", Message.uID);

					if (uRespQueue != IPC_Q_NONE)
					{
						msgSetError(&Message, ERR_INVALID_MSG);
						msgSend(&Message, uRespQueue);
					}
					break;
				}

		} /* switch (Message.uID) */

		if (pVMM->bProjLoaded == TRUE)
		{
			/* Refresh the resource state
			 */
			*(IEC_UDINT OS_DPTR *)(pVMM->Shared.pData[MAX_SEGMENTS].pAdr + pVMM->Project.ulStateVarOffs) 
				= pVMM->ulResState;
		}

		/* Check for reboot...
		 */
#if defined(RTS_CFG_FLASH)
		if (pVMM->bExecColdStart == TRUE || pVMM->bExecWarmStart == TRUE)
		{
			vmmRestart(pVMM);
		}
#endif /* RTS_CFG_FLASH */


	} /* for ( ; ; ) */

	RETURN(OK);
}
#endif /* RTS_CFG_VM_IPC && RTS_CFG_MULTILINK*/

/* ---------------------------------------------------------------------------- */
/**
 * vmmCreateTaskInfoVMM
 *
 */
static STaskInfoVMM *vmmCreateTaskInfoVMM(void)
{
	IEC_UINT uRes;

	/* STaskInfoVMM
	 */
	STaskInfoVMM *pVMM = osCreateTaskInfoVMM();
	if (pVMM == NULL)
	{
		return NULL;
	}

	OS_MEMSET(pVMM, 0x00, sizeof(STaskInfoVMM));

	pVMM->Shared.pVMM = pVMM;

	uRes = osInitializeVMM(pVMM);
	if (uRes != OK)
	{
		return NULL;
	}

	/* Code object list
	 */
	pVMM->Shared.pCode = osCreateCodeList();
	if (pVMM->Shared.pCode == NULL)
	{
		return NULL;
	}

	OS_MEMSET(pVMM->Shared.pCode, 0x00, sizeof(SObject) * MAX_CODE_OBJ);

	/* Online change data
	 */
#if defined(RTS_CFG_ONLINE_CHANGE)
	{
		IEC_UINT	i;

		SOnlChg 	*pOC = &pVMM->OnlChg;
		SImageReg	*pIR = NULL;

		/* Online Change
		 */
		pOC->pCopy		= (SOCCopy *)osMalloc (sizeof(SOCCopy)		 * MAX_OC_COPY_REG);
		pOC->pCodeInd	= (IEC_UINT *)osMalloc(sizeof(IEC_UINT) 	 * MAX_OC_CODE_OBJ);
		pOC->pDataInd	= (IEC_UINT *)osMalloc(sizeof(IEC_UINT) 	 * MAX_OC_DATA_OBJ);
		pOC->ppDataAdr	= (IEC_DATA OS_DPTR **)osMalloc(sizeof(IEC_DATA OS_DPTR *) * MAX_OC_DATA_OBJ);
		pOC->pTask		= (SOCTask *)osMalloc(sizeof(SOCTask)		 * MAX_TASKS);

		OS_MEMSET(pOC->pCopy,		0x00, sizeof(SOCCopy)			 * MAX_OC_COPY_REG);
		OS_MEMSET(pOC->pCodeInd,	0x00, sizeof(IEC_UINT)			 * MAX_OC_CODE_OBJ);
		OS_MEMSET(pOC->pDataInd,	0x00, sizeof(IEC_UINT)			 * MAX_OC_DATA_OBJ);
		OS_MEMSET(pOC->ppDataAdr,	0x00, sizeof(IEC_DATA OS_DPTR *) * MAX_OC_DATA_OBJ);
		OS_MEMSET(pOC->pTask,		0x00, sizeof(SOCTask)			 * MAX_TASKS);

		pIR = (SImageReg *)osMalloc(	  sizeof(SImageReg) 		 * MAX_TASKS);
		OS_MEMSET(pIR,				0x00, sizeof(SImageReg) 		 * MAX_TASKS);

		for (i = 0; i < MAX_TASKS; i++)
		{
			pOC->pTask[i].Task.pIR = pIR + i;
		}
	}

#endif

	return pVMM;
}

/* ---------------------------------------------------------------------------- */
/**
 * vmmInitialize
 *
 */
static IEC_UINT vmmInitialize(STaskInfoVMM **ppVMM)
{
	IEC_UINT uRes = OK;

	STaskInfoVMM *pVMM = NULL;

	/* OS dependent initializations
	 */
	uRes = osInitialize();
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* Create VMM task information structure
	 */
	pVMM = vmmCreateTaskInfoVMM();
	if (pVMM == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	*ppVMM = pVMM;

	uRes = sysInitialize();
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* Print version info
	 */
#if defined(RTS_CFG_DEBUG_OUTPUT)
	{	
		IEC_UINT uLen;

		uRes = sysGetVersionInfo(pVMM->pBuffer);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		uLen = (IEC_UINT)OS_STRLEN(pVMM->pBuffer);

		osTrace("\r\n%s\r\n", pVMM->pBuffer);

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

		osTrace("%s\r\n", pVMM->pBuffer);
	}
#endif

	/* Initialize System Load evaluation
	 */
#if defined(RTS_CFG_SYSLOAD)

	uRes = ldInitTaskInfo();
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = ldWriteTaskInfo(TASK_OFFS_SYS_VMM, osGetTaskID());
	TR_RET(uRes);

#endif

	/* Create file for debug information tracing
	 */
#if defined(RTS_CFG_DEBUG_FILE)
	{
		IEC_UDINT hF;

		uRes = utilCreateFile(&hF, (IEC_CHAR *)pVMM->pBuffer, VMM_MAX_PATH, osGetTraceDir, VMM_DIR_TRACE, VMM_FILE_TRACE);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
		xxxClose(hF);
	}
#endif

	/* Initialize states
	 */
#if defined(RTS_CFG_PROT_BLOCK)
	pVMM->COM.uState	= CS_READY_TO_RECEIVE;
	pVMM->COM.byBlock	= BT_DATA;

	pVMM->CMD.uState	= AS_IDLE;
	pVMM->CMD.uError	= OK;
	pVMM->CMD.byCommand = CMD_NOT_IMPLEMENTED;
#endif

	/* Check data alignment
	 */
#if defined(RTS_CFG_ALI_TRACE)

	uRes = vmmCheckAlignment(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
#endif

	/* Initialize IO layer
	 */
#if defined(RTS_CFG_IO_LAYER)

	uRes = osInitIOLayer(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
#endif

	/* Create semaphores
	 */
	{
		IEC_UINT i;

		for (i = 0; i < MAX_SEMAPHORE; i++)
		{
			uRes = osCreateSemaphore(i);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
		}
	}

	/* Create common VMM message queues
	 */
#if defined(RTS_CFG_VM_IPC)

	uRes = osCreateIPCQueue(Q_LIST_VMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = osCreateIPCQueue(Q_RESP_VMM_VM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
#endif

	/* Create online change VMM message queues
	 */
#if defined(RTS_CFG_VM_IPC) && defined(RTS_CFG_ONLINE_CHANGE)

	uRes = osCreateIPCQueue(Q_RESP_VMM_OC);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

#endif

	/* Create VM timer task message queues
	 */
#if defined(RTS_CFG_VM_IPC)

	uRes = osCreateIPCQueue(Q_RESP_VMM_VTI);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

#endif

	/* Create retain update task message queues
	 */
#if defined(RTS_CFG_EXT_RETAIN)

	uRes = osCreateIPCQueue(Q_RESP_VMM_RET);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

#endif

	/* Create firmware update task message queues
	 */
#if defined(RTS_CFG_FIRMWARE_DL)

	uRes = osCreateIPCQueue(Q_RESP_VMM_FW);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

#endif

	/* Create online change support task
	 */
#if defined(RTS_CFG_VM_IPC) && defined(RTS_CFG_ONLINE_CHANGE)

	uRes = osCreateOnlineChangeTask(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

#endif

	/* Create retain update task
	 */
#if defined(RTS_CFG_EXT_RETAIN)

	uRes = osCreateRetainTask(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

#endif

	/* Create firmware update task
	 */
#if defined(RTS_CFG_FIRMWARE_DL)

	uRes = osCreateFWUpdateTask(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

#endif

	/* Create VM timer task
	 */
#if defined(RTS_CFG_VM_IPC)

	uRes = osCreateVMTimerTask(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

#endif

	/* Initialize communication
	 */
#if defined(RTS_CFG_TCP_NATIVE)

	uRes = sockInitialize(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = osCreateListenTask(pVMM->pTCP);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

#else

	osOpenCom(pVMM);

#endif

	/* Breakpoint initializations
	 */
	uRes = bpInitialize(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* File access initializations
	 */
#if defined(RTS_CFG_FILE_ACCESS)

	uRes = osFileInitialize(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

#endif

#if defined(RTS_CFG_FILE_NATIVE)

	uRes = fileInitialize(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

#endif

	/* OK, calm down a little ...
	 */
	osSleep(250);

	/* Boot from flash ...
	 */
#if defined(RTS_CFG_FLASH)

	pVMM->bWarmStart	= TRUE;

	uRes = vmmReadFlash(pVMM);
	if (uRes != ERR_FILE_NOT_EXIST)
	{
		XBlock Block;

		Block.byLast	= TRUE;
		Block.uBlock	= 1;
		Block.uLen		= sizeof(IEC_UINT);
		Block.usSource	= 0;

		Block.CMD.byCommand = CMD_DOWNLOAD_END | 0x80u;
		*(IEC_UINT *)Block.CMD.pData = uRes;

		osOnCmdHandled(pVMM, &Block, uRes);

		TR_RET(uRes);
	}

	pVMM->bWarmStart	 = FALSE;

	pVMM->bExecColdStart = FALSE;
	pVMM->bExecWarmStart = FALSE;

#endif

	pVMM->ulResState	= RES_STATE_RUNNING;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmmRestart
 *
 */
#if defined(RTS_CFG_FLASH)

static IEC_UINT vmmRestart(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;

	/* Reboot (cold start or warm start) the VMM
	 * (== Do a new download!)
	 */

	pVMM->bWarmStart = pVMM->bExecWarmStart;

	uRes = vmmReadFlash(pVMM);
	if (uRes != ERR_FILE_NOT_EXIST)
	{
		XBlock Block;

		Block.byLast	= TRUE;
		Block.uBlock	= 1;
		Block.uLen		= sizeof(IEC_UINT);
		Block.usSource	= 0;

		Block.CMD.byCommand = CMD_DOWNLOAD_END | 0x80u;
		*(IEC_UINT *)Block.CMD.pData = uRes;

		osOnCmdHandled(pVMM, &Block, uRes);

		TR_RET(uRes);
	}

	if (uRes != OK)
	{
		IEC_UINT uRetry = 0;

		uRes = OK;

		while (uRes == OK)
		{
			uRes = actClearResource(pVMM, &uRetry, pVMM->bWarmStart);
			TR_RET(uRes);

			if (uRetry == 0 || uRetry > 25)
			{
				break;
			}
		}

#if defined(RTS_CFG_CUSTOM_DL)
		uRes = actClearCustomDL(pVMM);
		TR_RET(uRes);
#endif
	}

#if defined(RTS_CFG_EVENTS)
	vmmSetEvent(pVMM, (IEC_UINT)(pVMM->bExecColdStart == TRUE ? EVT_AFTER_COLDSTART : EVT_AFTER_WARMSTART));
	osSleep(50);
#endif

	pVMM->bWarmStart	 = FALSE;

	pVMM->bExecColdStart = FALSE;
	pVMM->bExecWarmStart = FALSE;

	RETURN(OK);
}
#endif /* RTS_CFG_FLASH */

/* ---------------------------------------------------------------------------- */
/**
 * vmmQueueMessage
 *
 * Writes a given string into the VMM Message Queue. (Contents will
 * be sent to the Engineering with the next CMD_GET_STATE command.)
 */
IEC_UINT vmmQueueMessage(SMsgQueue *pQueue, IEC_CHAR *szMessage)
{
	IEC_UINT uLen;
	IEC_UINT uRes	= OK;

	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

	OS_BEGIN_CRITICAL_SECTION(SEM_MSG_QUEUE)
	{
		if ((pQueue->uNext + 1) % MAX_STR_MSG_QUEUE == pQueue->uLast)
		{
			/* Queue full, skip (overwrite) oldest entry
			 */
#if defined(RTS_CFG_FFO)
			osFree((IEC_DATA **)&pQueue->ppMessage[pQueue->uLast]);
#else
			osFreeMsgStr(pQueue->uLast);
#endif

			pQueue->uLast = (IEC_UINT)((pQueue->uLast + 1) % MAX_STR_MSG_QUEUE);
		}

		/* Create new queue entry
		 */
#if defined(RTS_CFG_FFO)
		pQueue->ppMessage[pQueue->uNext] = (IEC_CHAR *)osMalloc(MAX_STR_MSG_LEN);
#else
		pQueue->ppMessage[pQueue->uNext] = osCreateMsgStr(pQueue->uNext);
#endif
		if (pQueue->ppMessage[pQueue->uNext] == NULL)
		{
			uRes = ERR_OUT_OF_MEMORY;
		}

		if (uRes == OK)
		{
			/* Message length fixed, just to be sure...
			 */
			uLen = (IEC_UINT)(OS_STRLEN(szMessage));
			uLen = (IEC_UINT)(uLen >= MAX_STR_MSG_LEN ? MAX_STR_MSG_LEN - 1 : uLen);

			OS_MEMCPY(pQueue->ppMessage[pQueue->uNext], szMessage, uLen);
			pQueue->ppMessage[pQueue->uNext][uLen] = 0;

			pQueue->uNext = (IEC_UINT)((pQueue->uNext + 1) % MAX_STR_MSG_QUEUE);
		}

	} OS_END_CRITICAL_SECTION(SEM_MSG_QUEUE)

	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ------------------------ */

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmmQueueBPNotification
 *
 * Writes a given (reached) breakpoint to the breakpoint notification
 * queue. (Contents will be sent to the Engineering with the next 
 * CMD_GET_STATE command.)
 *
 */
IEC_UINT vmmQueueBPNotification(STaskInfoVM *pVM, IEC_UINT uState)
{
	IEC_UINT uRes = OK;

	SBPQueue *pQueue = &pVM->pShared->BPQueue;
	SContext *pContext;

	if (pVM->Local.uContext == 1)
	{
		RETURN(ERR_ERROR);
	}

	pContext = pVM->Local.pContext + pVM->Local.uContext - 1u;

	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

	OS_BEGIN_CRITICAL_SECTION(SEM_BP_QUEUE)
	{
		SBPNotification *pNotify = pQueue->pQueue + pQueue->uNext;

		if ((pQueue->uNext + 1) % MAX_STR_MSG_QUEUE == pQueue->uLast)
		{
			/* Queue full, wait until next GetState command
			 */
			uRes = ERR_QUEUE_FULL;
		}

		if (uRes == OK)
		{
			pNotify->BP.uCode		= pContext->uCode;
			pNotify->BP.uCodePos	= pContext->uCodePos - 1u;
			pNotify->BP.uData		= pContext->uData;

			pNotify->uState 		= uState;
			pNotify->uTask			= pVM->usTask;

			pQueue->uNext = (IEC_UINT)((pQueue->uNext + 1) % MAX_BP_QUEUE);
		}

	} OS_END_CRITICAL_SECTION(SEM_BP_QUEUE)

	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ------------------------ */

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmmFormatException
 *
 * Formats (converts) a given task exception or common error state 
 * into an error message.
 *
 */
IEC_UINT vmmFormatException(SException *pExcept, STaskInfoVM *pVM, IEC_UDINT ulErrNo, IEC_CHAR *szBuffer)
{ 
	ulErrNo = ulErrNo & 0x000000ff;

	if (ulErrNo < EXCEPT_FIRST || ulErrNo > EXCEPT_LAST)
	{
		ulErrNo = EXCEPT_UNKOWN;
	}

	if (ulErrNo < EXCEPT_EXE_TIME_OVERRUN)
	{
		utilFormatString(szBuffer, TERR_S_EXCEPTION1, pVM->Task.szName, ulErrNo, 
				g_szExc[ulErrNo - EXCEPT_FIRST], 
				pExcept->uCode, pExcept->ulOffset, pExcept->uData);
	}
	else
	{
		utilFormatString(szBuffer, TERR_S_EXCEPTION2, pVM->Task.szName, ulErrNo, 
				g_szExc[ulErrNo - EXCEPT_FIRST]);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmmReadFlash
 *
 * Reads the flash contents and initializes a download process.
 */
#if defined(RTS_CFG_FLASH)

static IEC_UINT vmmReadFlash(STaskInfoVMM *pVMM)
{
	IEC_UINT	uDomain;
	IEC_UINT	uBlock;
	IEC_UINT	uRes;

	IEC_UDINT	ulBlockLen	= 0;
	IEC_UDINT	ulCount 	= 0;
	IEC_BOOL	bFirst;
	IEC_BOOL	bHandleRetry;

	XBlock	 Block;

	ACTPROC_FUN fpCommand;
	ACTPROC_FUN fpResponse;

	/* Initialize Flash
	 */
	uRes = osFRInit();
	if (uRes != OK)
	{
		if (uRes == ERR_FILE_NOT_EXIST)
		{
			return uRes;
		}

		RETURN(uRes);
	}

	for (uDomain = 0; uDomain < MAX_FLASH_DOMAIN; uDomain++)
	{
		/* For all download blocks...
		 */

		uRes = osFRInitDomain(uDomain, &ulBlockLen);		
		if (uRes == ERR_FLASH_WRONG_BLOCK 
				&& (uDomain == FLASH_DOMAIN_CUSTOM || uDomain == FLASH_DOMAIN_DEBUG))
		{
			/* Ignore, domain was probably empty.
			 */
			continue;
		}

		if (uRes != OK)
		{
			RETURN(uRes);
		}

		ulCount = 0;
		bFirst = TRUE;

		for (uBlock = 0; ulCount < ulBlockLen || bFirst == TRUE; uBlock++)
		{
			/* For all block sequences...
			 */

			Block.uBlock	= (IEC_UINT)(uBlock + 1);

			Block.usSource	= 0;

			if (ulBlockLen - ulCount > MAX_DATA)
			{
				Block.uLen	 = MAX_DATA;
				Block.byLast = FALSE;
			}
			else
			{
				Block.uLen	 = (IEC_UINT)(ulBlockLen - ulCount);
				Block.byLast = TRUE;
			}

			bFirst = FALSE;

			/* Read block
			 */
			uRes = osFlashRead(uDomain, uBlock, Block.CMD.pData, Block.uLen);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			if (uDomain == FLASH_DOMAIN_BEGIN && uBlock == 0)
			{
				/* Signal the downloader, that this is a cold-/warmstart out from
				 * flash data.
				 */
				*(IEC_UINT *)Block.CMD.pData = 
					(IEC_UINT)(DOWN_MODE_FLASH | (pVMM->bWarmStart ? DOWN_MODE_WARM : DOWN_MODE_COLD));
			}

			ulCount = (IEC_UDINT)(ulCount + Block.uLen);

#if ! defined(RTS_CFG_DEBUG_INFO)

			if (uDomain == FLASH_DOMAIN_DEBUG)
			{
				/* Not supported; ignore.
				 */
				continue;
			}

#endif

			switch(uDomain)
			{
				case FLASH_DOMAIN_BEGIN:
					{
						Block.CMD.byCommand = CMD_DOWNLOAD_BEGIN;
						fpCommand			= cmdDownloadBegin;
						fpResponse			= resDownloadBegin;
						bHandleRetry		= FALSE;
						break;
					}

				case FLASH_DOMAIN_CLEAR:
					{
						Block.CMD.byCommand = CMD_DOWNLOAD_CLEAR;
						fpCommand			= cmdDownloadClear;
						fpResponse			= resDownloadClear;
						bHandleRetry		= TRUE;
						break;
					}

				case FLASH_DOMAIN_CONFIG:
					{
						Block.CMD.byCommand = CMD_DOWNLOAD_CONFIG;
						fpCommand			= cmdDownloadConfig;
						fpResponse			= resDownloadConfig;
						bHandleRetry		= FALSE;
						break;
					}

				case FLASH_DOMAIN_CODE:
					{
						Block.CMD.byCommand = CMD_DOWNLOAD_CODE;
						fpCommand			= cmdDownloadCode;
						fpResponse			= resDownloadCode;
						bHandleRetry		= FALSE;
						break;
					}

				case FLASH_DOMAIN_INIT:
					{
						Block.CMD.byCommand = CMD_DOWNLOAD_INITIAL;
						fpCommand			= cmdDownloadInitial;
						fpResponse			= resDownloadInitial;
						bHandleRetry		= FALSE;
						break;
					}

#if defined(RTS_CFG_CUSTOM_DL)
				case FLASH_DOMAIN_CUSTOM:
					{
						Block.CMD.byCommand = CMD_DOWNLOAD_CUSTOM;
						fpCommand			= cmdDownloadCustom;
						fpResponse			= resDownloadCustom;
						bHandleRetry		= FALSE;
						break;
					}
#endif

#if defined(RTS_CFG_DEBUG_INFO)
				case FLASH_DOMAIN_DEBUG:
					{
						Block.CMD.byCommand = CMD_DOWNLOAD_DEBUG;
						fpCommand			= cmdDownloadDebug;
						fpResponse			= resDownloadDebug;
						bHandleRetry		= FALSE;
						break;
					}
#endif 

				case FLASH_DOMAIN_FINISH:
					{
						Block.CMD.byCommand = CMD_DOWNLOAD_FINISH;
						fpCommand			= cmdDownloadFinish;
						fpResponse			= resDownloadFinish;
						bHandleRetry		= FALSE;
						break;
					}

				case FLASH_DOMAIN_CFGIOL:
					{
						Block.CMD.byCommand = CMD_DOWNLOAD_IOL;
						fpCommand			= cmdDownloadIOL;
						fpResponse			= resDownloadIOL;
						bHandleRetry		= TRUE;
						break;
					}

				case FLASH_DOMAIN_END:
					{
						Block.CMD.byCommand = CMD_DOWNLOAD_END;
						fpCommand			= cmdDownloadEnd;
						fpResponse			= resDownloadEnd;
						bHandleRetry		= FALSE;
						break;
					}

				default:
					{
						RETURN(ERR_INVALID_DOMAIN);
					}

			} /* switch(uDomain) */


			uRes = osOnCmdReceived(pVMM, &Block);
			if (uRes != OK)
			{
				RETURN(uRes);
			}


			/* Do the download
			 */
			if (uBlock == 0)
			{
				actClearDownload(pVMM, Block.CMD.byCommand);

				fpCommand(pVMM, 0);
			}

			/* Download
			 */
			uRes = fpCommand(pVMM, &Block);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			if (Block.byLast == TRUE)
			{
				uRes = osOnCmdComputed(pVMM, &Block);
				if (uRes != OK)
				{
					RETURN(uRes);
				}

				uRes = fpResponse(pVMM, 0);
				if (uRes != OK)
				{
					RETURN(uRes);
				}

				Block.uBlock = 1;
				Block.uLen	 = 0;

				/* Response
				 */
				uRes = fpResponse(pVMM, &Block);
				if (uRes != OK)
				{
					RETURN(uRes);
				}

				if (bHandleRetry == TRUE)
				{
					IEC_UINT uRetry = *(IEC_UINT *)Block.CMD.pData;

					while (uRetry != 0 && uRes == OK)
					{
						osSleep(1000);

						uRes = fpResponse(pVMM, &Block);
						if (uRes != OK)
						{
							RETURN(uRes);
						}

						uRetry = *(IEC_UINT *)Block.CMD.pData;

						if (uRetry > 25)
						{
							RETURN(WRN_TIME_OUT);
						}
					}					
				}

				uRes = osOnCmdHandled(pVMM, &Block, OK);
				if (uRes != OK)
				{
					RETURN(uRes);
				}
			}

		} /* for (uBlock = 0; ulCount < ulBlockLen; uBlock++) */

		/* Download block finished
		 */
		uRes = osFRFinishDomain(uDomain);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

	} /* for (uDomain = 0; uDomain < MAX_FLASH_DOMAIN; uDomain++) */

	uRes = osFRFinish();

	if (uRes == OK)
	{
		vmmQueueMessage(&pVMM->Shared.MsgQueue, "[VMM]: <--------->  Project loaded from flash.\r\n");
		TR_STATE("--- PRJ: Loaded from flash.\r\n");
	}

#if defined(RTS_CFG_EVENTS)
	vmmSetEvent(pVMM, EVT_BOOTED);
#endif

	RETURN(uRes);
}
#endif	/* RTS_CFG_FLASH */

/* ---------------------------------------------------------------------------- */
/**
 * vmmStartTask
 *
 * @return		OK if successful else error number.
 */
IEC_UINT vmmStartTask(STaskInfoVMM *pVMM, IEC_UINT uTask)
{
	IEC_UINT uRes = OK;

	if (uTask >= pVMM->Project.uTasks)
	{
		RETURN(ERR_INVALID_TASK);
	}

#if ! defined(RTS_CFG_VM_IPC)

	pVMM->ppVM[uTask]->Local.pState->ulState = TASK_STATE_RUNNING;
	pVMM->ppVM[uTask]->Local.pState->ulErrNo = OK;

#if defined(RTS_CFG_EVENTS)
	pVMM->ppVM[uTask]->usException			 = OK;
#endif

#else

	uRes = msgTXCommand(MSG_VM_START, uTask, Q_RESP_VMM_VM, VMM_TO_IPC_MSG_LONG, TRUE);

#endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmmStopTask
 *
 * @return		OK if successful else error number.
 */
IEC_UINT vmmStopTask(STaskInfoVMM *pVMM, IEC_UINT uTask, IEC_BOOL bSync)
{
	IEC_UINT uRes = OK;

	if (uTask >= pVMM->Project.uTasks)
	{
		RETURN(ERR_INVALID_TASK);
	}

#if ! defined(RTS_CFG_VM_IPC)
	{
		STaskInfoVM *pVM = pVMM->ppVM[uTask];

		if (pVM->Local.pState->ulState == TASK_STATE_BREAK)
		{
			vmmQueueBPNotification(pVM, BP_STATE_LEAVED);			
			vmSetException(pVM, EXCEPT_TASK_BREAK);

			pVM->Local.uContext = 1;
		}

		pVM->Local.pState->ulState = TASK_STATE_STOPPED;
	}
#else

	pVMM->ppVM[uTask]->Local.uHalt = TRUE;

	uRes = msgTXCommand(MSG_VM_STOP, uTask, (IEC_UINT)(bSync ? Q_RESP_VMM_VM : IPC_Q_NONE), VMM_TO_IPC_MSG_LONG, TRUE);

#endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmmContinueTask
 *
 */
IEC_UINT vmmContinueTask(STaskInfoVMM *pVMM, IEC_UINT uTask)
{
	IEC_UINT uRes = OK;

	if (uTask >= pVMM->Project.uTasks)
	{
		RETURN(ERR_INVALID_TASK);
	}

#if ! defined(RTS_CFG_VM_IPC)
	{
		STaskInfoVM *pVM = pVMM->ppVM[uTask];

		if (pVM->Local.pState->ulState != TASK_STATE_BREAK)
		{
			RETURN(ERR_TASK_NOT_IN_BREAK);
		}

		vmmQueueBPNotification(pVM, BP_STATE_LEAVED);
		pVM->Local.pState->ulState = TASK_STATE_RUNNING;
	}
#else

	uRes = msgTXCommand(MSG_VM_CONTINUE, uTask, Q_RESP_VMM_VM, VMM_TO_IPC_MSG_LONG, TRUE);

#endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmmSingleStep
 *
 */
IEC_UINT vmmSingleStep(STaskInfoVMM *pVMM, IEC_UINT uTask)
{
	IEC_UINT uRes = OK;

	if (uTask >= pVMM->Project.uTasks)
	{
		RETURN(ERR_INVALID_TASK);
	}

#if ! defined(RTS_CFG_VM_IPC)
	{
		STaskInfoVM *pVM = pVMM->ppVM[uTask];

		if (pVM->Local.pState->ulState != TASK_STATE_BREAK)
		{
			RETURN(ERR_TASK_NOT_IN_BREAK);
		}

		vmmQueueBPNotification(pVM, BP_STATE_LEAVED);
		pVM->Local.pState->ulState = TASK_STATE_STEP;
	}
#else

	uRes = msgTXCommand(MSG_VM_STEP, uTask, Q_RESP_VMM_VM, VMM_TO_IPC_MSG_LONG, TRUE);

#endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmmDeleteTask
 *
 */
IEC_UINT vmmDeleteTask(STaskInfoVMM *pVMM, IEC_UINT uTask)
{
	IEC_UINT uRes = OK;

	if (uTask >= pVMM->Project.uTasks)
	{
		RETURN(ERR_INVALID_TASK);
	}

#if ! defined(RTS_CFG_VM_IPC)

	pVMM->ppVM[uTask]->Local.pState->ulState = TASK_STATE_DELETED;

#else

	pVMM->ppVM[uTask]->Local.uHalt = TRUE;

	uRes = msgTXCommand(MSG_VM_TERMINATE, uTask, Q_RESP_VMM_VM, VMM_TO_IPC_MSG_LONG, TRUE);

#endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmmResetTask
 *
 */
IEC_UINT vmmResetTask(STaskInfoVMM *pVMM, IEC_UINT uTask)
{
	IEC_UINT uRes = OK;

	if (uTask >= pVMM->Project.uTasks)
	{
		RETURN(ERR_INVALID_TASK);
	}

#if ! defined(RTS_CFG_VM_IPC)
	{
		STaskInfoVM *pVM = pVMM->ppVM[uTask];

		if (pVM->Local.pState->ulState == TASK_STATE_BREAK)
		{
			vmmQueueBPNotification(pVM, BP_STATE_LEAVED);
			vmSetException(pVM, EXCEPT_TASK_BREAK);

			/* Restart the task from the beginning
			 */
			pVM->Local.uContext = 1;
		}
	}
#else

	uRes = msgTXCommand(MSG_VM_RESET, uTask, Q_RESP_VMM_VM, VMM_TO_IPC_MSG_LONG, TRUE);

#endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * vmmFindVarInCpyList
 * searches the given Var in the CopyList and returns 
 * the destination if the Var in list
 * in:	uiInst	 - instance / segment of the variable
 *		ulOffset - offset of the variable in this instance / segment
 *		uiSize	 - length of the variable
 * out: pulOffset- pointer to the offset in the destination
 * ret: true if destination address found
 *		false otherwise
 */
#if defined(RTS_CFG_COPY_DOMAIN)

IEC_BOOL vmmFindVarInCpyList(STaskInfoVMM  *pVMM, XVariable *pxVar, IEC_UDINT *pulOffset)
{
	IEC_UINT i;

	SRetainReg *pCopyRegions = pVMM->Shared.CopyRegions;

	for(i = 0; i < pVMM->Project.uCpyRegConst; i++) 
	{
		if (pCopyRegions[i].uData == pxVar->uSegment	&&
				pxVar->ulOffset >= pCopyRegions[i].uOffSrc	&& 
				pxVar->ulOffset + pxVar->uLen <= (IEC_UDINT)pCopyRegions[i].uOffSrc + pCopyRegions[i].uSize)
		{
			*pulOffset = pCopyRegions[i].ulOffDst + pxVar->ulOffset - pCopyRegions[i].uOffSrc;
			return TRUE;
		}
	}

	*pulOffset = 0;
	return FALSE;
}
#endif /* RTS_CFG_COPY_DOMAIN */

/* ---------------------------------------------------------------------------- */
/**
 * vmmCopyRegions
 * does all copy operations from the CpyRegionList in pShared
 * in:	uStart	- first list element to copy
 *		uCount	- number of list elements to copy
 *		bReverse - direction of copy operation
 *				   true:  Segment --> instance
 *				   flase: instance --> Segment
 * ret: OK
 *		ERR_INVALID_PARAM if the index is out of scope of CpyList
 */
#if defined(RTS_CFG_COPY_DOMAIN)

IEC_UINT vmmCopyRegions(SIntShared *pGlobal, IEC_UINT uStart, IEC_UINT uCount, IEC_BOOL bReverse) 
{
	IEC_UINT i;
	IEC_DATA OS_DPTR *pSrc;
	IEC_DATA OS_DPTR *pDst;
	SRetainReg *pRegion;

	if(uStart + uCount > pGlobal->uCpyRegions) 
	{
		RETURN(ERR_INVALID_PARAM);
	}

	for (i = uStart; i < uStart + uCount; i++) 
	{
		pRegion = pGlobal->CopyRegions + i;

		pSrc = OS_ADDPTR(pGlobal->pData[pRegion->uData].pAdr, pRegion->uOffSrc);

		pDst = OS_ADDPTR(pGlobal->pData[SEG_RETAIN].pAdr, pRegion->ulOffDst);
		pDst = OS_NORMALIZEPTR(pDst);

		if(bReverse) 
		{
			/* Reverse direction Segment --> instance  
			 */
			OS_MEMCPY(pSrc, pDst, pRegion->uSize);
		} 
		else 
		{
			/* Normal direction instance --> Segment   
			 */
			OS_MEMCPY(pDst, pSrc, pRegion->uSize);
		}
	}

	RETURN(OK);
}
#endif /* RTS_CFG_COPY_DOMAIN */

/* ---------------------------------------------------------------------------- */
/**
 * vmmCheckAlignment
 *
 */
#if defined(RTS_CFG_ALI_TRACE)

static IEC_UINT vmmCheckAlignment(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;

	SContext CON;
	IEC_DATA *pCON = (IEC_DATA *)&CON;

	SException EXC;
	IEC_DATA *pEXC = (IEC_DATA *)&EXC;

#if defined(RTS_CFG_WRITE_FLAGS)
	SIntLocal SIL;
	IEC_DATA *pSIL = (IEC_DATA *)&SIL;
#endif

	osTrace("Communication structure alignment (2 Byte):\r\n");

	osTrace("sizeof(XBreakpoint) = %2d == %2d -> %d\r\n", sizeof(XBreakpoint), 10, sizeof(XBreakpoint) == 10);
	uRes |= sizeof(XBreakpoint) == 10 ? OK : ERR_ERROR;
	osTrace("sizeof(XTask_21001) = %2d == %2d -> %d\r\n", sizeof(XTask_21001), 56, sizeof(XTask_21001) == 56);
	uRes |= sizeof(XTask_21001) == 56 ? OK : ERR_ERROR;
	osTrace("sizeof(XAlignTest)  = %2d == %2d -> %d\r\n", sizeof(XAlignTest),	9, sizeof(XAlignTest)  ==  9);
	uRes |= sizeof(XAlignTest)	==	9 ? OK : ERR_ERROR;


#if defined(IP_CFG_STACK8)

	osTrace("\r\nIEC function parameter alignment (IP_CFG_STACK%d):\r\n", 8);

	osTrace("sizeof(MUX_64BIT_PAR_TYP) = %2d == %2d -> %d\r\n", sizeof(MUX_64BIT_PAR_TYP),	 18, sizeof(MUX_64BIT_PAR_TYP)	==	18);
	uRes |= sizeof(MUX_64BIT_PAR_TYP)  ==  18 ? OK : ERR_ERROR;
	osTrace("sizeof(FUNC_ALI_TEST)     = %2d == %2d -> %d\r\n", sizeof(FUNC_ALI_TEST),	 9, sizeof(FUNC_ALI_TEST)  ==  9);
	uRes |= sizeof(FUNC_ALI_TEST)  ==  9 ? OK : ERR_ERROR;

#elif defined(IP_CFG_STACK16)

	osTrace("\r\nIEC function parameter alignment (IP_CFG_STACK%d):\r\n", 16);

	osTrace("sizeof(MUX_64BIT_PAR_TYP) = %2d == %2d -> %d\r\n", sizeof(MUX_64BIT_PAR_TYP),	 18, sizeof(MUX_64BIT_PAR_TYP)	==	18);
	uRes |= sizeof(MUX_64BIT_PAR_TYP)  ==  18 ? OK : ERR_ERROR;
	osTrace("sizeof(FUNC_ALI_TEST)     = %2d == %2d -> %d\r\n", sizeof(FUNC_ALI_TEST),	 10, sizeof(FUNC_ALI_TEST)	==	10);
	uRes |= sizeof(FUNC_ALI_TEST)  ==  10 ? OK : ERR_ERROR;

#elif defined(IP_CFG_STACK32)

	osTrace("\r\nIEC function parameter alignment (IP_CFG_STACK%d):\r\n", 32);

	osTrace("sizeof(MUX_64BIT_PAR_TYP) = %2d == %2d -> %d\r\n", sizeof(MUX_64BIT_PAR_TYP),	 20, sizeof(MUX_64BIT_PAR_TYP)	==	20);
	uRes |= sizeof(MUX_64BIT_PAR_TYP)  ==  20 ? OK : ERR_ERROR;
	osTrace("sizeof(FUNC_ALI_TEST)     = %2d == %2d -> %d\r\n", sizeof(FUNC_ALI_TEST),	 12, sizeof(FUNC_ALI_TEST)	==	12);
	uRes |= sizeof(FUNC_ALI_TEST)  ==  12 ? OK : ERR_ERROR;

#elif defined(IP_CFG_STACK64)

	osTrace("\r\nIEC function parameter alignment (IP_CFG_STACK%d):\r\n", 64);

	osTrace("sizeof(MUX_64BIT_PAR_TYP) = %2d == %2d -> %d\r\n", sizeof(MUX_64BIT_PAR_TYP),	 24, sizeof(MUX_64BIT_PAR_TYP)	==	24);
	uRes |= sizeof(MUX_64BIT_PAR_TYP)  ==  24 ? OK : ERR_ERROR;
	osTrace("sizeof(FUNC_ALI_TEST)     = %2d == %2d -> %d\r\n", sizeof(FUNC_ALI_TEST),	 16, sizeof(FUNC_ALI_TEST)	==	16);
	uRes |= sizeof(FUNC_ALI_TEST)  ==  16 ? OK : ERR_ERROR;

#else
	uRes |= ERR_ERROR;
#endif


#if defined(IP_CFG_INST8)

	osTrace("\r\nIEC instance data alignment (IP_CFG_INST%d):\r\n", 8);

	osTrace("sizeof(TP_TON_TOF_PAR) = %2d == %2d -> %d\r\n", sizeof(TP_TON_TOF_PAR),   17, sizeof(TP_TON_TOF_PAR)  ==  17);
	uRes |= sizeof(TP_TON_TOF_PAR)	==	17 ? OK : ERR_ERROR;
	osTrace("sizeof(FB_ALI_TEST)    = %2d == %2d -> %d\r\n", sizeof(FB_ALI_TEST),	9, sizeof(FB_ALI_TEST)	==	9);
	uRes |= sizeof(FB_ALI_TEST)  ==  9 ? OK : ERR_ERROR;

#elif defined(IP_CFG_INST16)

	osTrace("\r\nIEC instance data alignment (IP_CFG_INST%d):\r\n", 16);

	osTrace("sizeof(TP_TON_TOF_PAR) = %2d == %2d -> %d\r\n", sizeof(TP_TON_TOF_PAR),   20, sizeof(TP_TON_TOF_PAR)  ==  20);
	uRes |= sizeof(TP_TON_TOF_PAR)	==	20 ? OK : ERR_ERROR;
	osTrace("sizeof(FB_ALI_TEST)    = %2d == %2d -> %d\r\n", sizeof(FB_ALI_TEST),	9, sizeof(FB_ALI_TEST)	==	9);
	uRes |= sizeof(FB_ALI_TEST)  ==  9 ? OK : ERR_ERROR;

#elif defined(IP_CFG_INST32)

	osTrace("\r\nIEC instance data alignment (IP_CFG_INST%d):\r\n", 32);

	osTrace("sizeof(TP_TON_TOF_PAR) = %2d == %2d -> %d\r\n", sizeof(TP_TON_TOF_PAR),   24, sizeof(TP_TON_TOF_PAR)  ==  24);
	uRes |= sizeof(TP_TON_TOF_PAR)	==	24 ? OK : ERR_ERROR;
	osTrace("sizeof(FB_ALI_TEST)    = %2d == %2d -> %d\r\n", sizeof(FB_ALI_TEST),	9, sizeof(FB_ALI_TEST)	==	9);
	uRes |= sizeof(FB_ALI_TEST)  ==  9 ? OK : ERR_ERROR;

#elif defined(IP_CFG_INST64)

	osTrace("\r\nIEC instance data alignment (IP_CFG_INST%d):\r\n", 64);

	osTrace("sizeof(TP_TON_TOF_PAR) = %2d == %2d -> %d\r\n", sizeof(TP_TON_TOF_PAR),   24, sizeof(TP_TON_TOF_PAR)  ==  24);
	uRes |= sizeof(TP_TON_TOF_PAR)	==	24 ? OK : ERR_ERROR;
	osTrace("sizeof(FB_ALI_TEST)    = %2d == %2d -> %d\r\n", sizeof(FB_ALI_TEST),	9, sizeof(FB_ALI_TEST)	==	9);
	uRes |= sizeof(FB_ALI_TEST)  ==  9 ? OK : ERR_ERROR;

#else
	uRes |= ERR_ERROR;
#endif

	osTrace("\r\n");

	osTrace("SObject:    so=%05d\r\n", 
			sizeof(SObject));
	osTrace("SContext:   so=%05d  .uData=%d .uCodePos=%d  .pData=%d\r\n", 
			sizeof(SContext), (IEC_DATA *)&CON.uData - pCON, (IEC_DATA *)&CON.uCodePos - pCON, (IEC_DATA *)&CON.pData - pCON);
	osTrace("SException: so=%05d  .uData=%d .uErrNo=%d    .ulOffset=%d\r\n", 
			sizeof(SException), (IEC_DATA *)&EXC.uData - pEXC, (IEC_DATA *)&EXC.uErrNo - pEXC, (IEC_DATA *)&EXC.ulOffset - pEXC);
#if defined(RTS_CFG_WRITE_FLAGS)
	osTrace("SIntLocal:  so=%05d  .pSeg=%d  .WriteFlg=%d .usCurrent=%d .pProg=%d .pContext=%d .Exception=%d\r\n", 
			sizeof(SIntLocal), (IEC_DATA*)&SIL.pSeg - pSIL, (IEC_DATA*)&SIL.WriteFlags - pSIL, (IEC_DATA*)&SIL.usCurrent - pSIL, (IEC_DATA*)&SIL.pProg - pSIL, (IEC_DATA*)&SIL.pContext - pSIL, (IEC_DATA*)&SIL.Exception - pSIL);
#endif
	osTrace("SIntShared: so=%05d  .pCode=%d\r\n", 
			sizeof(SIntShared), (IEC_DATA*)&pVMM->Shared.pCode - (IEC_DATA *)&pVMM->Shared);

	osTrace("\r\n");
	{
		printf("IEC_BOOL %d\n", sizeof (IEC_BOOL));
		printf("IEC_UINT %d\n", sizeof (IEC_UINT));
		printf("IEC_UDINT %d\n", sizeof (IEC_UDINT));
		printf("IEC_USINT %d\n", sizeof (IEC_USINT));
		printf("IEC_BYTE %d\n", sizeof (IEC_BYTE));
		printf("IEC_DATA %d\n", sizeof (IEC_DATA));
		printf("IEC_BYTE %d\n", sizeof (IEC_BYTE));
		printf("IEC_WORD %d\n", sizeof (IEC_WORD));
		printf("IEC_DWORD %d\n", sizeof (IEC_DWORD));
		printf("IEC_LWORD %d\n", sizeof (IEC_LWORD));
#if 1
		printf("STaskInfoVM %d\n", sizeof (STaskInfoVM));
		printf("SIntShared %d\n", sizeof (SIntShared));
		printf("SProject %d\n", sizeof (SProject));
		printf("SComTCP %d\n", sizeof (SComTCP));
		printf("SDLBuffer %d\n", sizeof (SDLBuffer));
		printf("SProjInfo %d\n", sizeof (SProjInfo));
		printf("SOnlChg %d\n", sizeof (SOnlChg));
		printf("SDBIInfo %d\n", sizeof (SDBIInfo));
		printf("SIOLayer %d\n", sizeof (SIOLayer));
		printf("SDLClrFlash %d\n", sizeof (SDLClrFlash));
		printf("SDLInitClear %d\n", sizeof (SDLInitClear));
		printf("SDLProjInfo %d\n", sizeof (SDLProjInfo));
		printf("SDLBreak %d\n", sizeof (SDLBreak));
		printf("SDLTask %d\n", sizeof (SDLTask));
		printf("SDLGetVal %d\n", sizeof (SDLGetVal));
		printf("SDLDebug %d\n", sizeof (SDLDebug));
		printf("SDLData %d\n", sizeof (SDLData));
		printf("SDLProj %d\n", sizeof (SDLProj));
		printf("SDLFile %d\n", sizeof (SDLFile));
		printf("SDLHeader %d\n", sizeof (SDLHeader));
		printf("STaskInfoVMM %d\n", sizeof (STaskInfoVMM));
		printf("SMessage %d\n", sizeof (SMessage));
#endif
		printf("XCommand %d\n", sizeof (XCommand));
		printf("XBlock %d\n", sizeof (XBlock));
		printf("XObject %d\n", sizeof (XObject));
		printf("XFrame %d\n", sizeof (XFrame));
		printf("XVariable %d\n", sizeof (XVariable));
		printf("XVariableS %d\n", sizeof (XVariableS));
		printf("XVariableM %d\n", sizeof (XVariableM));
		printf("XVariableL %d\n", sizeof (XVariableL));
		printf("XValue %d\n", sizeof (XValue));
		printf("XBreakpoint %d\n", sizeof (XBreakpoint));
#if ! defined(INC_RTS)
		printf("XTask_200 %d\n", sizeof (XTask_200));
		printf("XTask_205 %d\n", sizeof (XTask_205));
		printf("XTask_207 %d\n", sizeof (XTask_207));
		printf("XProject_200 %d\n", sizeof (XProject_200));
		printf("XProject_205 %d\n", sizeof (XProject_205));
		printf("XProject_207 %d\n", sizeof (XProject_207));
		printf("XDBIFile_208 %d\n", sizeof (XDBIFile_208));
		printf("XFile_208 %d\n", sizeof (XFile_208));
#endif
		printf("XTask_21001 %d\n", sizeof (XTask_21001));
		printf("XMemRegion %d\n", sizeof (XMemRegion));
		printf("XProject_213 %d\n", sizeof (XProject_213));
		printf("XProjInfo %d\n", sizeof (XProjInfo));
		printf("XInfo %d\n", sizeof (XInfo));
		printf("XDownHeader %d\n", sizeof (XDownHeader));
		printf("XDownDirect %d\n", sizeof (XDownDirect));
		printf("XCopyRegion %d\n", sizeof (XCopyRegion));
		printf("XOnlineChange %d\n", sizeof (XOnlineChange));
		printf("XOCInstCopy %d\n", sizeof (XOCInstCopy));
		printf("XFileDef %d\n", sizeof (XFileDef));
		printf("XAlignTest %d\n", sizeof (XAlignTest));
		printf("XConfig %d\n", sizeof (XConfig));
		printf("XDBIInstance %d\n", sizeof (XDBIInstance));
		printf("XDBIVar %d\n", sizeof (XDBIVar));
		printf("XVisuVar %d\n", sizeof (XVisuVar));
		printf("XFWDownload %d\n", sizeof (XFWDownload));
		printf("XVersionInfo %d\n", sizeof (XVersionInfo));
		printf("XTypeInfo %d\n", sizeof (XTypeInfo));
		printf("XSerialNo %d\n", sizeof (XSerialNo));
		printf("XMacAddr %d\n", sizeof (XMacAddr));
		printf("XLicKey %d\n", sizeof (XLicKey));
		printf("XFeatDef %d\n", sizeof (XFeatDef));
		printf("XLicEx %d\n", sizeof (XLicEx));
		printf("XBPNotification %d\n", sizeof (XBPNotification));
		printf("XIndex %d\n", sizeof (XIndex));
		printf("XDBIType %d\n", sizeof (XDBIType));
		printf("XIOLayer %d\n", sizeof (XIOLayer));
		printf("XInstKey %d\n", sizeof (XInstKey));

	}
	RETURN(uRes);
}
#endif /* RTS_CFG_ALI_TRACE */

/* ---------------------------------------------------------------------------- */
/**
 * vmmBuildVMEvents
 *
 */
#if defined(RTS_CFG_EVENTS)

IEC_UINT vmmBuildVMEvents(STaskInfoVMM *pVMM)
{
	IEC_UINT	uRes	= OK;
	IEC_UINT	uTask	= 0;
	IEC_UINT	uNotify = 0;
	IEC_UINT	uEvent	= 0;
	IEC_UDINT	ulMask	= 0;
	STaskInfoVM *pVM	= NULL;
	IEC_UINT	pTask[MAX_TASKS];

	for (uEvent = 0; uEvent < MAX_EVENTS; uEvent++)
	{
		uNotify = 0;

		pVMM->ppVMEvents[uEvent]	= NULL;
		pVMM->pVMEventCount[uEvent] = 0;

		if (uEvent == 0 || uEvent == sizeof(IEC_UDINT) * 8)
		{
			ulMask = 0x01u;
		}

		for (uTask = 0; uTask < pVMM->Project.uTasks; uTask++)
		{
			pVM = pVMM->ppVM[uTask];

			if ((pVM->Task.usAttrib & VMM_TASK_ATTR_EVENT_DRIVEN) != 0)
			{
				if (((uEvent < sizeof(IEC_UDINT) * 8 ? pVM->Task.ulPara1 : pVM->Task.ulPara2) & ulMask) != 0)
				{
					pTask[uNotify] = uTask;
					uNotify++;
				}
			}
		}

		if (uNotify != 0)
		{
#if defined(RTS_CFG_FFO)		
			pVMM->ppVMEvents[uEvent] = (IEC_UINT *)osMalloc(uNotify * sizeof(IEC_UINT));
#else
			pVMM->ppVMEvents[uEvent] = osCreateVMEvents(uEvent);
#endif

			OS_MEMCPY(pVMM->ppVMEvents[uEvent], pTask, uNotify * sizeof(IEC_UINT));
			pVMM->pVMEventCount[uEvent] = uNotify;
		}

		ulMask <<= 1;

	} /* for (uEvent = 0; uEvent < MAX_EVENTS; uEvent++) */

	RETURN(uRes);
}
#endif /* RTS_CFG_EVENTS */

/* ---------------------------------------------------------------------------- */
/**
 * vmmClearVMEvents
 *
 */
#if defined(RTS_CFG_EVENTS)

IEC_UINT vmmClearVMEvents(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;
	IEC_UINT i;

	for (i = 0; i < MAX_EVENTS; i++)
	{
		if (pVMM->ppVMEvents[i] != NULL)
		{
#if defined(RTS_CFG_FFO)			
			uRes |= osFree((IEC_DATA **)&pVMM->ppVMEvents[i]);
#else
			uRes |= osFreeVMEvents(i);
#endif
		}

		pVMM->ppVMEvents[i] 	= NULL;
		pVMM->pVMEventCount[i]	= 0;
	}

	RETURN(uRes);
}
#endif /* RTS_CFG_EVENTS */

/* ---------------------------------------------------------------------------- */
/**
 * vmmSetEvent
 *
 */
#if defined(RTS_CFG_EVENTS)

IEC_UINT vmmSetEvent(STaskInfoVMM *pVMM, IEC_UINT uEvent)
{
	IEC_UINT uRes = OK;
	IEC_UINT i;

	SMessage Message;

	if (uEvent > MAX_EVENTS)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	if (pVMM->ppVMEvents[uEvent] == NULL)
	{
		RETURN(OK);
	}

	Message.uID 		= MSG_EVENT;
	Message.uLen		= sizeof(IEC_UINT);
	Message.uRespQueue	= IPC_Q_NONE;

	*(IEC_UINT *)Message.pData = uEvent;

	for (i = 0; i < pVMM->pVMEventCount[uEvent]; i++)
	{
		uRes |= msgSend(&Message, pVMM->ppVMEvents[uEvent][i]);
	}

	RETURN(uRes);
}
#endif /* RTS_CFG_EVENTS */

/* ---------------------------------------------------------------------------- */

