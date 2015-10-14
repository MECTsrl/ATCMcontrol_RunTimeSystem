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
#ifdef USE_POSIX_MQUEUE
#include <mqueue.h>
#else
#include <sys/msg.h>
#endif
#if defined(_SOF_4CDC_SRC_)
#define __MIPSEL__
#endif

/* ----  Local Defines:   ----------------------------------------------------- */

#ifdef USE_POSIX_MQUEUE
#define OS_INVALID_QUEUE	-1
#define OS_HANDLE			int
#else
#define OS_INVALID_QUEUE	-1
#define OS_HANDLE			int
#endif

/* ----  Global Variables:	 -------------------------------------------------- */

static OS_HANDLE g_hQueue[MAX_IPC_QUEUE];

/* ----  Local Functions:	--------------------------------------------------- */
#ifdef USE_POSIX_MQUEUE
#define USE_TIMEDRECEIVE
#else
#error devi definire USE_POSIX_MQUEUE
typedef struct mymsgbuf {
	long    mtype;
	char    mtext[FC_MAX_MSG_LEN];
} mymsgbuf_s;
#endif
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
#ifdef USE_POSIX_MQUEUE
	char sQueueId[32];
    struct mq_attr mattr;
	sprintf(sQueueId, "/mq%03d", uQueue);
	memset(&mattr, 0, sizeof(mattr));
	mattr.mq_flags = 0;
	// sizeof(SMessage)=8014 FC_MAX_MSG_LEN=8000 /proc/sys/fs/mqueue/msgsize_max=8192
    mattr.mq_msgsize = sizeof(SMessage);
	// /proc/sys/fs/mqueue/msg_max 10
    mattr.mq_maxmsg = 8;
	mq_unlink(sQueueId);
#ifdef USE_TIMEDRECEIVE
   	g_hQueue[uQueue] = mq_open(sQueueId, O_RDWR|O_CREAT|O_EXCL           , 0000, &mattr);
#else
   	g_hQueue[uQueue] = mq_open(sQueueId, O_RDWR|O_CREAT|O_EXCL|O_NONBLOCK, 0000, &mattr);
#endif
#else
	g_hQueue[uQueue] = msgget(FC_IPC_MSQ_BASE + uQueue, IPC_CREAT | 0666);
#endif
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
#ifdef USE_POSIX_MQUEUE
		char sQueueId[32];
		sprintf(sQueueId, "/mq%03d", uQueue);
		mq_close(g_hQueue[uQueue]);
		mq_unlink(sQueueId);
#else
		msgctl(g_hQueue[uQueue], IPC_RMID, NULL);
#endif
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
#ifdef USE_TIMEDRECEIVE
	int retval;
	if (ulTimeOut == VMM_WAIT_FOREVER) {
		retval = mq_receive(g_hQueue[uQueue], (char *)pMessage, sizeof(SMessage), NULL);
	} else if (ulTimeOut == VMM_NO_WAIT) {
		struct mq_attr mattr;
		if (mq_getattr(g_hQueue[uQueue], &mattr) == -1) {
			RETURN_e(ERR_IPC_RECV_FAILED);
		}
		if (mattr.mq_curmsgs == 0) {
			RETURN_e(WRN_TIME_OUT);
		}
		retval = mq_receive(g_hQueue[uQueue], (char *)pMessage, sizeof(SMessage), NULL);
	} else {
		struct timespec abs_timeout;
		clock_gettime(CLOCK_REALTIME, &abs_timeout);
		abs_timeout.tv_nsec += ulTimeOut * 1E6; // ms
		while (abs_timeout.tv_nsec >= 1E9) {
			abs_timeout.tv_sec += 1;
			abs_timeout.tv_nsec -= 1E9;
		}
		retval = mq_timedreceive(g_hQueue[uQueue], (char *)pMessage, sizeof(SMessage), NULL, &abs_timeout);
	}
	if (retval != -1) {
		if (pMessage->uLen > FC_MAX_MSG_LEN) {
			IEC_DATA *pData = (IEC_DATA *)*(IEC_UDINT *)pMessage->pData;
			OS_MEMCPY(pMessage->pData, pData, pMessage->uLen);
			osFree(&pData);
		}
		RETURN(OK);
	}
	if (errno == ETIMEDOUT) {
		RETURN_e(WRN_TIME_OUT);
	}
	RETURN_e(ERR_IPC_RECV_FAILED);
#else
	IEC_UINT	uRes	= OK;
	IEC_UDINT	ulTime	= 0;
#ifdef USE_POSIX_MQUEUE
	ssize_t mq_receive_res;
#else
	IEC_BOOL	bOnce	= TRUE;
	int result;
	mymsgbuf_s buffer;
#endif

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
#ifdef USE_POSIX_MQUEUE
		mq_receive_res = mq_receive(g_hQueue[uQueue], (char *)pMessage, sizeof(SMessage), NULL);
	        if (mq_receive_res != -1) {
			if (pMessage->uLen > FC_MAX_MSG_LEN) {
				IEC_DATA *pData = (IEC_DATA *)*(IEC_UDINT *)pMessage->pData;
				OS_MEMCPY(pMessage->pData, pData, pMessage->uLen);
				osFree(&pData);
			}
			RETURN(OK);
	        }
		if (errno != EAGAIN) {
			RETURN_e(ERR_IPC_RECV_FAILED);
		}
		// errno == EAGAIN
		if (ulTimeOut == VMM_NO_WAIT) {
			return WRN_TIME_OUT;
		}
		if ((ulTimeOut != VMM_WAIT_FOREVER) && (osGetTime32() - ulTime >= ulTimeOut)) {
			return WRN_TIME_OUT;
		}
		// ulTimeOut == VMM_WAIT_FOREVER
#ifdef __XENO__
#ifdef USE_TIMEDRECEIVE
		; // retry
#else
	        osSleep(1); // 1ms
#endif
#else
		osSleep(11); // 11ms
#endif
#else
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
				// NB in questo caso la osMalloc() l'ha fatta il chiamante
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
#endif
	} /* for ( ; ; ) */
	
	uRes = ERR_ERROR;
	RETURN(uRes);
#endif
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
	
	if (pMessage->uLen > FC_MAX_MSG_LEN)
	{
		IEC_DATA *pData = osMalloc(pMessage->uLen); 

		OS_MEMCPY(pData, pMessage->pData, pMessage->uLen);
		 
		uSendLen = sizeof(IEC_UDINT);
		*(IEC_UDINT *)pMessage->pData = (IEC_UDINT)pData;
	}
#ifdef USE_POSIX_MQUEUE
    if (mq_send(g_hQueue[uQueue], (char *)pMessage, HD_MESSAGE + uSendLen, 0) == -1)
#else
	pMessage->mtype = FC_IPC_MSG_TYPE;
	if (msgsnd(g_hQueue[uQueue], (struct msgbuf *)pMessage, HD_MESSAGE + uSendLen, IPC_NOWAIT) == -1)
#endif
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

	if (uQueue > MAX_IPC_QUEUE)
	{
		RETURN(ERR_INVALID_QUEUE);
	}
#ifdef USE_POSIX_MQUEUE
       	struct mq_attr mattr;

       	if (mq_getattr(g_hQueue[uQueue], &mattr) == -1) {
            RETURN_e(ERR_ERROR);
       	}
       	*upMsgCount = mattr.mq_curmsgs;
#else
	struct msqid_ds msq;

	if (msgctl(g_hQueue[uQueue], IPC_STAT, &msq) == -1)
	{
		RETURN_e(ERR_ERROR);
	}
	*upMsgCount = msq.msg_qnum;
#endif

	RETURN(uRes);
}
#endif	/* RTS_CFG_VM_IPC */

/* ---------------------------------------------------------------------------- */
