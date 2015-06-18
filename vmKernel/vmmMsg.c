
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
 * Filename: vmmMsg.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"vmmMsg.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_VM_IPC)

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

#if defined(RTS_CFG_IPC_TRACE)
  static IEC_CHAR *msgGetMsgString(IEC_UINT uID);
#endif

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * msgSend
 *
 */
IEC_UINT msgSend(SMessage *pMsg, IEC_UINT uQueue)
{
	IEC_UINT uRes	= OK;
	IEC_UINT uCount = 0;
	IEC_UINT i		= 0;

	SMessage Dummy;

	if (uQueue >= MAX_IPC_QUEUE)
	{
		RETURN(ERR_INVALID_QUEUE);
	}

	if (pMsg->uRespQueue != IPC_Q_NONE && pMsg->uRespQueue >= MAX_IPC_QUEUE)
	{
		RETURN(ERR_INVALID_QUEUE);
	}

	if (pMsg->uRespQueue != IPC_Q_NONE)
	{
		uRes = osGetMessageCount(pMsg->uRespQueue, &uCount);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		for (i = 0; i < 100 && uCount != 0; )
		{
			uRes = osRecvMessage(&Dummy, pMsg->uRespQueue, VMM_NO_WAIT);
			uRes = osGetMessageCount(pMsg->uRespQueue, &uCount);

			if (uRes != OK)
			{
				RETURN(uRes);
			}

			/* Avoid endless loop
			 */
			i++;
		}
	}

	uRes = osSendMessage(pMsg, uQueue);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * msgRecv
 *
 */
IEC_UINT msgRecv(SMessage *pMsg, IEC_UINT uQueue, IEC_UDINT ulTimeOut)
{
	IEC_UINT uRes = OK;

	if (uQueue >= MAX_IPC_QUEUE)
	{
		RETURN(ERR_INVALID_QUEUE);
	}
	
	if (ulTimeOut == VMM_NO_WAIT)
	{
		IEC_UINT uCount;

		uRes = osGetMessageCount(uQueue, &uCount);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		if (uCount == 0)
		{
			return WRN_NO_MESSAGE;
		}
	}

	uRes = osRecvMessage(pMsg, uQueue, ulTimeOut);

	if (uRes == WRN_TIME_OUT)
	{
		return uRes;
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * msgSendCommand
 *
 */
IEC_UINT msgSendCommand(IEC_UINT uID, IEC_UINT uQueue, IEC_UINT uRespQueue)
{
	IEC_UINT uRes = OK;

	SMessage Message;
	Message.uID 		= uID;
	Message.uLen		= 0;
	Message.uRespQueue	= uRespQueue;

	uRes = msgSend(&Message, uQueue);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * msgTXCommand
 *
 */
IEC_UINT msgTXCommand(IEC_UINT uID, IEC_UINT uQueue, IEC_UINT uRespQueue, IEC_UDINT ulTimeOut, IEC_BOOL bSend)
{
	IEC_UINT uRes = OK;

	SMessage Message;
	Message.uID 		= uID;
	Message.uLen		= 0;
	Message.uRespQueue	= uRespQueue;

	uRes = msgTXMessage(&Message, uQueue, ulTimeOut, bSend);
	if (uRes != OK)
	{
		if (uRes == WRN_TIME_OUT)
		{
			return uRes;
		}
		
		RETURN(uRes);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * msgTXMessage
 *
 */
#if defined(RTS_CFG_IPC_TRACE)

#if ! defined(RTS_CFG_COMM_TRACE)
  #define xTAB1 "%s %-9s %d  %4dms "
  #define xTAB2 "%s %-9s %d "
#else
  #define xTAB1 "\t\t\t\t\t\t%s %-9s %d  %4dms "
  #define xTAB2 "\t\t\t\t\t\t%s %-9s %d "
#endif

IEC_UINT msgTXMessage(SMessage *pMsg, IEC_UINT uQueue, IEC_UDINT ulTimeOut, IEC_BOOL bSend)
{
	static IEC_UINT uTrans = 0;

	IEC_UINT uRes	= OK;

	IEC_CHAR *szMsg = msgGetMsgString(pMsg->uID);
	
	IEC_UINT  uID	= pMsg->uID;
  #if defined(RTS_CFG_DEBUG_OUTPUT)
	IEC_UINT  uTA	= uTrans++;
  #endif
	IEC_ULINT ullT	= 0;
	IEC_UDINT ulT	= 0;

	ullT = osGetTimeUS();

	if (bSend == TRUE)
	{
		if (szMsg != NULL)
		{
		  #if defined(RTS_CFG_DEBUG_OUTPUT)
			
			switch(pMsg->uID)
			{
			  #if defined(RTS_CFG_EXT_RETAIN)
				case MSG_RT_SET_GUID:
			  #else
				case MSG_TI_CONFIG:
			  #endif
				case MSG_VM_RESET:
				case MSG_VM_START:
				case MSG_VM_STOP:
				case MSG_TI_STOP:
				case MSG_IO_START:
					osTrace("\r\n");
					break;
			}

			osTrace(xTAB2"\r\n", ">>>", szMsg, uTA);
		  #endif
		}

		/* Send Message
		 */
		uRes = msgSend(pMsg, uQueue);
		if (uRes != OK)
		{
			if (szMsg != NULL)
			{
			  #if defined(RTS_CFG_DEBUG_OUTPUT)
				IEC_UDINT ulDiff = utilGetTimeDiffMS(ullT);
				osTrace(xTAB1"(Send failed - %d)\r\n", "ERR", szMsg, uTA, ulDiff, uRes);
			  #endif
			}

			uTrans--;
			RETURN(uRes);
		}

		if (pMsg->uRespQueue == IPC_Q_NONE)
		{
			uTrans--;

			RETURN(OK);
		}
	
	} /* if (bSend == TRUE) */

	if (pMsg->uRespQueue == IPC_Q_NONE)
	{
		/* Should already be handled directly after the send command.
		 */
		RETURN(ERR_INVALID_PARAM);
	}
	
	/* Receive Message
	 */
	uRes = msgRecv(pMsg, pMsg->uRespQueue, ulTimeOut);

	ulT = utilGetTimeDiffMS(ullT);

	if (uRes != OK)
	{
		/* Receive failed
		 */
		if (uRes == WRN_TIME_OUT)
		{
			/* Don't trace a time out
			 */
			if (szMsg != NULL)
			{
			  #if defined(RTS_CFG_DEBUG_OUTPUT)
				osTrace(xTAB1"(Receive failed - Time Out)\r\n", "ERR", szMsg, uTA, ulT);
			  #endif
			}

			uTrans--;
			return uRes;
		}

		if (szMsg != NULL)
		{
		  #if defined(RTS_CFG_DEBUG_OUTPUT)
			osTrace(xTAB1"Receive failed (0x%x)\r\n", "ERR", szMsg, uTA, ulT, uRes);
		  #endif
		}

		uTrans--;
		RETURN(uRes);
	}

	if (uID != (pMsg->uID & ~MSG_ERROR_FLAG))
	{
		szMsg = msgGetMsgString((IEC_UINT)(pMsg->uID & ~MSG_ERROR_FLAG));
	}

	if (pMsg->uID & MSG_ERROR_FLAG)
	{		
		/* Handle error response
		 */
		uRes = (IEC_UINT)(pMsg->uLen == sizeof(IEC_UINT) ? *(IEC_UINT *)pMsg->pData : ERR_ERRMSG_RECEIVED);
		
		pMsg->uID &= ~MSG_ERROR_FLAG;

		if (szMsg != NULL)
		{
		  #if defined(RTS_CFG_DEBUG_OUTPUT)
			osTrace(xTAB1"Error Message (0x%x) received.\r\n", "ERR", szMsg, uTA, ulT, uRes);
		  #endif
		}

		uTrans--;
		RETURN(uRes);
	}

	if (uID != pMsg->uID)
	{
		if (szMsg != NULL)
		{
		  #if defined(RTS_CFG_DEBUG_OUTPUT)
			osTrace(xTAB1"Unexpected Message received (0x%x instead of 0x%x).\r\n", "ERR", szMsg, uTA, ulT, pMsg->uID, uID);
		  #endif
		}

		uTrans--;
		RETURN(ERR_UNEXPECTED_MSG);
	}

	/* Everything OK
	 */
	if (szMsg != NULL)
	{
	  #if defined(RTS_CFG_DEBUG_OUTPUT)
		osTrace(xTAB1"\r\n", "<<<", szMsg, uTA, ulT);
	  #endif
	}
	uTrans--;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * msgTXMessage
 *
 */
#else /* RTS_CFG_IPC_TRACE */

IEC_UINT msgTXMessage(SMessage *pMsg, IEC_UINT uQueue, IEC_UDINT ulTimeOut, IEC_BOOL bSend)
{
	IEC_UINT uRes = OK;
	IEC_UINT uID  = pMsg->uID;

	if (bSend == TRUE)
	{
		/* Send Message
		 */
		uRes = msgSend(pMsg, uQueue);

		if (uRes != OK || pMsg->uRespQueue == IPC_Q_NONE)
		{
			RETURN(uRes);
		}
	}

	if (pMsg->uRespQueue == IPC_Q_NONE)
	{
		/* Should already be handled directly after the send command.
		 */
		RETURN(ERR_INVALID_PARAM);
	}

	/* Receive Message
	 */
	uRes = msgRecv(pMsg, pMsg->uRespQueue, ulTimeOut);

	if (uRes != OK)
	{
		/* Receive failed
		 */
		if (uRes == WRN_TIME_OUT)
		{
			/* Don't trace a time out
			 */
			return uRes;
		}

		RETURN(uRes);
	}

	if (pMsg->uID & MSG_ERROR_FLAG)
	{
		/* Handle error response
		 */
		uRes = (IEC_UINT)(pMsg->uLen == sizeof(IEC_UINT) ? *(IEC_UINT *)pMsg->pData : ERR_ERRMSG_RECEIVED);

		pMsg->uID &= ~MSG_ERROR_FLAG;
	}

	if (uID != pMsg->uID)
	{
		RETURN(ERR_UNEXPECTED_MSG);
	}

	RETURN(uRes);

}
#endif /* RTS_CFG_IPC_TRACE */
 
/* ---------------------------------------------------------------------------- */
/**
 * msgGetMsgString
 *
 */
#if defined(RTS_CFG_IPC_TRACE)

static IEC_CHAR *msgGetMsgString(IEC_UINT uID)
{
	IEC_CHAR *szMsg;

	switch (uID)
	{
		case MSG_VM_START:			szMsg = "VM_START"; 	break;
		case MSG_VM_STOP:			szMsg = "VM_STOP";		break;
		case MSG_VM_CONTINUE:		szMsg = "VM_CONT";		break;
		case MSG_VM_STEP:			szMsg = "VM_STEP";		break;
		case MSG_VM_RESET:			szMsg = "VM_RESET"; 	break;
		case MSG_VM_TERMINATE:		szMsg = "VM_TERM";		break;

		case MSG_TIMER: 			szMsg = "TIMER";		break;
		case MSG_EVENT: 			szMsg = "EVENT";		break;

		case MSG_CT_DATA:			szMsg = "CT_DATA";		break;
		case MSG_CT_TERM:			szMsg = "CT_TERM";		break;

		case MSG_OC_PREPARE:		szMsg = "OC_PREP";		break;
		case MSG_OC_COMMIT: 		szMsg = "OC_COMMIT";	break;

		case MSG_IO_CONFIG: 		szMsg = "IO_CONF";		break;
		case MSG_IO_START:			szMsg = "IO_START"; 	break;
		case MSG_IO_STOP:			szMsg = "IO_STOP";		break;
	  #if ! defined(RTS_CFG_IPC_TRACE_IO)
		case MSG_IO_NOTIFY_SET: 	szMsg = NULL;			break;
		case MSG_IO_NOTIFY_GET: 	szMsg = NULL;			break;
	  #else
		case MSG_IO_NOTIFY_SET: 	szMsg = "IO_SET";		break;
		case MSG_IO_NOTIFY_GET: 	szMsg = "IO_GET";		break;
	  #endif
		case MSG_IO_TERMINATE:		szMsg = "IO_TERM";		break;
		case MSG_IO_DONE:			szMsg = "IO_DONE";		break;
		case MSG_IO_EVENT:			szMsg = "IO_EVENT"; 	break;
		case MSG_IO_CONFIG_RES: 	szMsg = "IO_CFGRES";	break;
		case MSG_IO_TERM_RES:		szMsg = "IO_TRMRES";	break;
			
		case MSG_TI_CONFIG: 		szMsg = "TI_CONF";		break;
		case MSG_TI_START:			szMsg = "TI_START"; 	break;
		case MSG_TI_STOP:			szMsg = "TI_STOP";		break;
		case MSG_TI_TERMINATE:		szMsg = "TI_TERM";		break;
		
		case MSG_VM_COLDSTART:		szMsg = "VM_COLD";		break;
		case MSG_VM_WARMSTART:		szMsg = "VM_WARM";		break;
		case MSG_VM_REBOOT: 		szMsg = "VM_BOOT";		break;

		case MSG_RT_OPEN:			szMsg = "RT_OPEN";		break;
		case MSG_RT_CLOSE:			szMsg = "RT_CLOSE"; 	break;
		case MSG_RT_START:			szMsg = "RT_START"; 	break;
		case MSG_RT_STOP:			szMsg = "RT_STOP";		break;
		case MSG_RT_UPDATE: 		szMsg = "RT_UPD";		break;
		case MSG_RT_SET_GUID:		szMsg = "RT_GUID";		break;
		case MSG_RT_SET_SIZE:		szMsg = "RT_SIZE";		break;
		case MSG_RT_SET_CYCLE:		szMsg = "RT_CYCLE"; 	break;
			
		case MSG_FW_STATE:			szMsg = "FW_STATE"; 	break;
		case MSG_FW_EXECUTE:		szMsg = "FW_EXEC";		break;

		case MSG_CV_START:			szMsg = "CV_START"; 	break;
		case MSG_CV_STOP:			szMsg = "CV_STOP";		break;
		case MSG_CV_TERMINATE:		szMsg = "CV_TERM";		break;
			
		case MSG_SC_START:			szMsg = "SC_START"; 	break;
		case MSG_SC_STOP:			szMsg = "SC_STOP";		break;
		case MSG_SC_TERMINATE:		szMsg = "SC_TERM";		break;
			
		case MSG_DV_START:			szMsg = "DV_START"; 	break;
		case MSG_DV_STOP:			szMsg = "DV_STOP";		break;
		case MSG_DV_TERMINATE:		szMsg = "DV_TERM";		break;
			
		case MSG_FL_START:			szMsg = "FL_START"; 	break;
		case MSG_FL_STOP:			szMsg = "FL_STOP";		break;
		case MSG_FL_TERMINATE:		szMsg = "FL_TERM";		break;

		case MSG_CG_CONFIG: 		szMsg = "CG_CONF";		break;
		case MSG_CG_TERMINATE:		szMsg = "CG_TERM";		break;

		case MSG_CG_CONF_DONE:		szMsg = "CG_CDONE"; 	break;
		case MSG_CG_TERM_DONE:		szMsg = "CG_TDONE"; 	break;		
			
		case MSG_PB_CONFIG: 		szMsg = "PB_CONF";		break;
		case MSG_PB_TERMINATE:		szMsg = "PB_TERM";		break;

		case MSG_PB_CONF_DONE:		szMsg = "PB_CDONE"; 	break;
		case MSG_PB_TERM_DONE:		szMsg = "PB_TDONE"; 	break;		
			
		default:					szMsg = "NIXNUTZ";		break;
	}

	return szMsg;
}
#endif /* RTS_CFG_IPC_TRACE */

/* ---------------------------------------------------------------------------- */
/**
 * msgSetError
 *
 */
IEC_UINT msgSetError(SMessage *pMessage, IEC_UINT uError)
{
	pMessage->uID |= MSG_ERROR_FLAG;

	pMessage->uLen = sizeof(IEC_UINT);
	*(IEC_UINT *)pMessage->pData = uError;

	RETURN(OK);
}

#endif	/* RTS_CFG_VM_IPC */

/* ---------------------------------------------------------------------------- */
