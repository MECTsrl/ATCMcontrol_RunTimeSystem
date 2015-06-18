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
 * Filename: osMsg.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"osMsg.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_TCP_NATIVE)
  #include "osSocket.h"
#endif

/* ----  Target Specific Includes:	 ------------------------------------------ */

#include "fcDef.h"

#if defined(_SOF_4CDC_SRC_)
#undef __MIPSEL__
#endif
#include <sys/ipc.h>
#include <sys/msg.h>
#if defined(_SOF_4CDC_SRC_)
#define __MIPSEL__
#endif

/* ----  Local Defines:   ----------------------------------------------------- */

#define OS_INVALID_QUEUE	-1
#define OS_HANDLE			int

/* ----  Global Variables:	 -------------------------------------------------- */

static OS_HANDLE g_hQueue[MAX_IPC_QUEUE];

/* ----  Local Functions:	--------------------------------------------------- */
typedef struct mymsgbuf {
	long    mtype;
	char    mtext[FC_MAX_MSG_LEN];
} mymsgbuf_s;
/* ----  Local Variables:	 -------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * osInitializeShared
 *
 * Do operating system dependent on time initializations here.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osInitializeShared(void)
{
	IEC_UINT uRes = OK; 
	IEC_UINT i;

	for (i = 0; i < MAX_IPC_QUEUE; i++)
	{
		g_hQueue[i] = OS_INVALID_QUEUE;
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osCreateIPCQueue
 *
 * Creates a message queue for the given queue ID.
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osCreateIPCQueue(IEC_UINT uQueue)
{
	IEC_UINT uRes = OK;

	if (uQueue >= MAX_IPC_QUEUE)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	if (g_hQueue[uQueue] != OS_INVALID_QUEUE)
	{
		osDestroyIPCQueue(uQueue);
	}

	g_hQueue[uQueue] = msgget(FC_IPC_MSQ_BASE + uQueue, IPC_CREAT | 0666);
	if (g_hQueue[uQueue] == OS_INVALID_QUEUE)
	{
		RETURN_e(ERR_CREATE_QUEUE);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osDestroyIPCQueue
 *
 * Destroys a message queue for the given queue ID.
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osDestroyIPCQueue(IEC_UINT uQueue)
{
	IEC_UINT uRes = OK;

	if (uQueue >= MAX_IPC_QUEUE)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	if (g_hQueue[uQueue] != OS_INVALID_QUEUE)
	{
		msgctl(g_hQueue[uQueue], IPC_RMID, NULL);
		g_hQueue[uQueue] = OS_INVALID_QUEUE;
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osRecvMessage
 *
 * Receive (wait) for a message on the given message queue.
 *
 * @return			OK if successful, WRN_TIME_OUT if time out 
 *					else ERR_ERROR
 */
#if defined(RTS_CFG_VM_IPC)

IEC_UINT osRecvMessage(SMessage *pMessage, IEC_UINT uQueue, IEC_UDINT ulTimeOut)
{
	IEC_UINT	uRes	= OK;

	IEC_UDINT	ulTime	= 0;
	IEC_BOOL	bOnce	= TRUE;
	int result;
	mymsgbuf_s buffer;

	if (uQueue > MAX_IPC_QUEUE)
	{
		RETURN(ERR_INVALID_QUEUE);
	}

	if (ulTimeOut != VMM_WAIT_FOREVER && ulTimeOut != VMM_NO_WAIT)
	{
		ulTime = osGetTime32();
	}
	
	for ( ; ; )
	{
		/* this while is a work-around that force the msgrcv again in case that a signal interrupt it */
		do
		{
			result = msgrcv(g_hQueue[uQueue], &buffer, sizeof(SMessage) - sizeof(long), FC_IPC_MSG_TYPE, ulTimeOut == VMM_WAIT_FOREVER ? 0 : IPC_NOWAIT);
		} while (errno == EINTR);
		if (result != -1)
		{
			pMessage->mtype = buffer.mtype;
			OS_MEMCPY(((char*)pMessage) + sizeof(long), buffer.mtext, result);
#if 0
			{
				int i=0;
				printf("EGB-RX-MSG:");
				for (i = 0; i < result; i++)
				{
					printf(".%x.", *(((char*)pMessage) + sizeof(long) + i));
				}
				printf("\n");
			}
#endif
			if (pMessage->uLen > FC_MAX_MSG_LEN)
			{
				IEC_DATA *pData = (IEC_DATA *)*(IEC_UDINT *)pMessage->pData;
				OS_MEMCPY(pMessage->pData, pData, pMessage->uLen);
				osFree(&pData);
			}

			RETURN(OK);
		}

		if (errno == EIDRM && bOnce == TRUE)
		{
			/* A application/thread on the other side of the queue has terminated itself. However, 
			 * messages from the terminated application may still be pending in the queue, so retry.
			 */
			ulTimeOut = 100;
			ulTime = osGetTime32();

			bOnce = FALSE;

		  #if defined(RTS_CFG_DEBUG_OUTPUT)
			osTrace("WARNING: ***** EIDRM Received. ***** \r\n");
		  #endif

			continue;
		}
		
#if 0
		if (errno == EINTR)
		{
			/* This error is - hopefully - caused by the debugger, so we can ignore it.
			 */
		  #if defined(RTS_CFG_DEBUG_OUTPUT)
			osTrace("WARNING: ***** EINTR Received. ***** \r\n");
		  #endif

			if (ulTimeOut == VMM_WAIT_FOREVER)
			{
				continue;
			}
		}
		else 
#endif
			
		if (errno != ENOMSG || ulTimeOut == VMM_WAIT_FOREVER)
		{
			RETURN_e(ERR_IPC_RECV_FAILED);
		}
						
		if (ulTimeOut == VMM_NO_WAIT || osGetTime32() - ulTime >= ulTimeOut)
		{
			return WRN_TIME_OUT;
		}

		osSleep(11);
	
	} /* for ( ; ; ) */
	
	uRes = ERR_ERROR;
	RETURN(uRes);
}
#endif	/* RTS_CFG_VM_IPC */

/* ---------------------------------------------------------------------------- */
/**
 * osSendMessage
 *
 * Sends a message on the given message queue.
 *
 * @return			OK if successful, WRN_TIME_OUT if time out 
 *					else ERR_ERROR
 */
#if defined(RTS_CFG_VM_IPC)

IEC_UINT osSendMessage(SMessage *pMessage, IEC_UINT uQueue)
{
	IEC_UINT uRes = OK;
	IEC_UINT uSendLen = pMessage->uLen;

	if (uQueue > MAX_IPC_QUEUE)
	{
		RETURN(ERR_INVALID_QUEUE);
	}
	
	pMessage->mtype = FC_IPC_MSG_TYPE;

	if (pMessage->uLen > FC_MAX_MSG_LEN)
	{
		IEC_DATA *pData = osMalloc(pMessage->uLen); 

		OS_MEMCPY(pData, pMessage->pData, pMessage->uLen);
		 
		uSendLen = sizeof(IEC_UDINT);
		*(IEC_UDINT *)pMessage->pData = (IEC_UDINT)pData;
	}
	if (msgsnd(g_hQueue[uQueue], (struct msgbuf *)pMessage, HD_MESSAGE + uSendLen, IPC_NOWAIT) == -1)
	{
		RETURN_e(ERR_IPC_SEND_FAILED);
	}

	RETURN(uRes);
}
#endif	/* RTS_CFG_VM_IPC */

/* ---------------------------------------------------------------------------- */
/**
 * osGetMessageCount
 *
 * This function is called to determine the number of waiting messages in a
 * message queue associated to a VM task. (This number is used to realize
 * the watchdog functionality.)
 *
 * @return			OK if successful, else error number.
 */
#if defined(RTS_CFG_VM_IPC)

IEC_UINT osGetMessageCount(IEC_UINT uQueue, IEC_UINT *upMsgCount)
{
	IEC_UINT uRes = OK;

	struct msqid_ds msq;

	if (uQueue > MAX_IPC_QUEUE)
	{
		RETURN(ERR_INVALID_QUEUE);
	}

	if (msgctl(g_hQueue[uQueue], IPC_STAT, &msq) == -1)
	{
		RETURN_e(ERR_ERROR);
	}

	*upMsgCount = msq.msg_qnum;

	RETURN(uRes);
}
#endif	/* RTS_CFG_VM_IPC */

/* ---------------------------------------------------------------------------- */

