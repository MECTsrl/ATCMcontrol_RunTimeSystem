
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
 * Filename: tstMain.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"tstMain.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_IOTEST)

#include "tstMain.h"
#include "iolDef.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * tstMain
 *
 */
IEC_UINT tstMain(void *pPara)
{
	SIOConfig		IO;
	SIOLayerIniVal	INI;

	SMessage Message;
	IEC_UINT uRespQueue;
	
	IEC_UINT uRes		= OK;
	IEC_UINT uResInit	= OK;

	IEC_UINT uResConfig = ERR_ERROR;
	IEC_UINT uConfDone	= ERR_ERROR;

	IEC_UINT uResTerm	= ERR_ERROR;
	IEC_UINT uTermDone	= ERR_ERROR;

	IEC_UINT uIOLayer;
	IEC_BOOL bAsyncConfig;
	IEC_BOOL bWarmStart;

	/* Initializations
	 */
	OS_MEMCPY (&INI, pPara, sizeof(SIOLayerIniVal));
	OS_MEMSET (&IO,  0x00,	sizeof(SIOConfig));

  #if defined(RTS_CFG_IO_TRACE)
	osTrace("--- IO%d: IO layer created.\r\n", INI.uIOLayer);
  #endif
	
	uIOLayer	 = INI.uIOLayer;
	bAsyncConfig = INI.bAsyncConfig;
	bWarmStart	 = INI.bWarmStart;

	uRes = osCreateIPCQueue((IEC_UINT)(Q_OFFS_IO + uIOLayer));
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uResInit = tstInitialize(uIOLayer);
	TR_RET(uResInit);

	/* Main Message Loop
	 */
	for ( ; ; )
	{
		if (msgRecv(&Message, (IEC_UINT)(Q_OFFS_IO + uIOLayer), VMM_WAIT_FOREVER) != OK)
		{
			osSleep(100);
			continue;
		}

		uRespQueue			= Message.uRespQueue;
		Message.uRespQueue	= IPC_Q_NONE;

		switch(Message.uID)
		{
			case MSG_IO_CONFIG: /* -------------------------------------------- */ 
			{
				SIOConfig *pConfig = (SIOConfig *)Message.pData;
				
			  #if defined(RTS_CFG_IO_TRACE)
				osTrace("--- IO%d: Configuration received: Channel:%d Name:%s\r\n", uIOLayer, pConfig->usChannel, pConfig->szName);
			  #endif

				uResConfig	= OK;
				uConfDone	= WRN_IN_PROGRESS;
				
				uResTerm	= ERR_ERROR;
				uTermDone	= ERR_ERROR;

				/* Verify Configuration Data
				 */
				if (Message.uLen != sizeof(SIOConfig))
				{					
				  #if defined(RTS_CFG_IO_TRACE)
					osTrace("--- IO%d: ERROR: Configuration failed: Channel:%d Name:%s\r\n", uIOLayer, pConfig->usChannel, pConfig->szName);
				  #endif

					msgSetError(&Message, ERR_INVALID_PARAM);
					
					if (uRespQueue != IPC_Q_NONE)
					{
						uRes = msgSend(&Message, uRespQueue);
						TR_RET(uRes);
					}
					break;
				}

				/* Verify Initialization Result
				 */
				if (uResInit != OK)
				{
				  #if defined(RTS_CFG_IO_TRACE)
					osTrace("--- IO%d: ERROR: Configuration failed: Channel:%d Name:%s\r\n", uIOLayer, pConfig->usChannel, pConfig->szName);
				  #endif

					msgSetError(&Message, uResInit);

					if (uRespQueue != IPC_Q_NONE)
					{
						uRes = msgSend(&Message, uRespQueue);
						TR_RET(uRes);
					}
					break;
				}

				if (uRespQueue != IPC_Q_NONE && bAsyncConfig == TRUE)
				{
					/* Asynchronous configuration: Send ACK immediately.
					 */
					Message.uLen = 0;

					uRes = msgSend(&Message, uRespQueue);
					TR_RET(uRes);
				}

				OS_MEMCPY(&IO, pConfig, sizeof(SIOConfig));

				/* Execute Configuration
				 */
				uRes = tstNotifyConfig(uIOLayer, &IO);
				TR_RET(uRes);

				uResConfig	= uRes;
				uConfDone	= OK;

				if (bAsyncConfig == FALSE && uRespQueue != IPC_Q_NONE)
				{
					/* Synchronous configuration: Send result.
					 */
				  #if defined(RTS_CFG_IO_TRACE)
					if (uResConfig != OK)
					{
						osTrace("--- IO%d: ERROR: Configuration failed: Channel:%d Name:%s\r\n", uIOLayer, IO.usChannel, IO.szName);
					}
				  #endif

					if (uResConfig == OK)
					{
						Message.uLen = 0;
					}
					else
					{
						msgSetError(&Message, uResConfig);
					}

					uRes = msgSend(&Message, uRespQueue);
					TR_RET(uRes);
				}

				if (bAsyncConfig == TRUE && uResConfig != OK)
				{
					/* Asynchronous configuration failed: Send result.
					 */
					SMessage Msg;
					SIODone  *pIOD = (SIODone *)Msg.pData;

				  #if defined(RTS_CFG_IO_TRACE)
					osTrace("--- IO%d: ERROR: Configuration failed: Channel:%d Name:%s\r\n", uIOLayer, IO.usChannel, IO.szName);
				  #endif

					Msg.uID  = MSG_IO_DONE;
					Msg.uLen = sizeof(SIODone);

					Msg.uRespQueue	= IPC_Q_NONE;

					pIOD->uRes		= uResConfig;
					pIOD->uIOLayer	= uIOLayer;
					
					pIOD->usChannel = IO.usChannel;
					OS_STRCPY(pIOD->szName, IO.szName);

					uRes = msgSend(&Msg, Q_LIST_VMM);
					TR_RET(uRes);
				}

				break;
			}
			
			case MSG_CG_CONF_DONE:	/* ---------------------------------------- */
			{
				uConfDone = (IEC_UINT)((Message.uLen == sizeof(IEC_UINT)) ? *(IEC_UINT *)Message.pData : ERR_INVALID_PARAM);

			  #if defined(RTS_CFG_IO_TRACE)
				if (uConfDone == OK)
				{
					osTrace("--- IO%d: Configuration finished: Channel:%d Name:%s\r\n", uIOLayer, IO.usChannel, IO.szName);
				}
				else
				{
					osTrace("--- IO%d: ERROR: Configuration failed: Channel:%d Name:%s\r\n", uIOLayer, IO.usChannel, IO.szName);
				}
			  #endif

				if (bAsyncConfig == TRUE)
				{
					/* Asynchronous configuration: Send result.
					 */
					SMessage Msg;
					SIODone  *pIOD = (SIODone *)Msg.pData;

					Msg.uID  = MSG_IO_DONE;
					Msg.uLen = sizeof(SIODone);

					Msg.uRespQueue	= IPC_Q_NONE;

					pIOD->uRes		= uConfDone;
					pIOD->uIOLayer	= uIOLayer;
					
					pIOD->usChannel = IO.usChannel;
					OS_STRCPY(pIOD->szName, IO.szName);

					uRes = msgSend(&Msg, Q_LIST_VMM);
					TR_RET(uRes);
				}

				break;
			}

			case MSG_IO_CONFIG_RES: /* ---------------------------------------- */ 
			{
				/* OK ................ Configuration successfully finished.
				 * Error number ...... Configuration failed.
				 * WRN_IN_PROGRESS ... Configuration not yet finished.
				 */
				if (uRespQueue != IPC_Q_NONE)
				{
					Message.uLen = sizeof(IEC_UINT);

					*(IEC_UINT *)Message.pData = (IEC_UINT)(uResConfig != OK ? uResConfig : uConfDone);

					uRes = msgSend(&Message, uRespQueue);
					TR_RET(uRes);
				}

				break;
			}

			case MSG_IO_START:	/* -------------------------------------------- */
			{
			  #if defined(RTS_CFG_IO_TRACE)
				osTrace("--- IO%d: IO layer started.\r\n", uIOLayer);
			  #endif

				uRes = tstNotifyStart(uIOLayer, &IO);
				TR_RET(uRes);

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

					uRes = msgSend(&Message, uRespQueue);
					TR_RET(uRes);
				}

				break;
			}

			case MSG_IO_STOP:	/* -------------------------------------------- */ 
			{
			  #if defined(RTS_CFG_IO_TRACE)
				osTrace("--- IO%d: IO layer stopped.\r\n", uIOLayer);
			  #endif

				uRes = tstNotifyStop(uIOLayer, &IO);
				TR_RET(uRes);
				
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

					uRes = msgSend(&Message, uRespQueue);
					TR_RET(uRes);
				}

				break;
			}

			case MSG_IO_NOTIFY_SET: /* ---------------------------------------- */
			{
				/* A IEC task or an external application has written into the 
				 * global task image (%Q area).
				 */
				
				SIONotify *pNotify = (SIONotify *)Message.pData;

				uRes = tstNotifySet(uIOLayer, &IO, pNotify);
				TR_RET(uRes);
				
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

					uRes = msgSend(&Message, uRespQueue);
					TR_RET(uRes);
				}
			
				break;
			}
			
			case MSG_IO_NOTIFY_GET: /* ---------------------------------------- */
			{
				/* A IEC task or an external application is going to access the 
				 * global task image (%I or %Q area). Update corresponding data
				 * if necessary.
				 */

				SIONotify *pNotify = (SIONotify *)Message.pData;

				uRes = tstNotifyGet(uIOLayer, &IO, pNotify);
				TR_RET(uRes);

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

					uRes = msgSend(&Message, uRespQueue);
					TR_RET(uRes);
				}

				break;
			}
			
			case MSG_IO_TERMINATE:	/* ---------------------------------------- */ 
			{
			  #if defined(RTS_CFG_IO_TRACE)
				osTrace("--- IO%d: IO layer terminating.\r\n", uIOLayer);
			  #endif
				
				uResConfig	= ERR_ERROR;
				uConfDone	= ERR_ERROR;

				uResTerm	= OK;
				uTermDone	= WRN_IN_PROGRESS;

				if (Message.uLen != sizeof(IEC_BOOL))
				{
				  #if defined(RTS_CFG_IO_TRACE)
					osTrace("--- IO%d: ERROR: Termination failed: Channel:%d Name:%s\r\n", uIOLayer, IO.usChannel, IO.szName);
				  #endif

					msgSetError(&Message, ERR_INVALID_PARAM);
					
					if (uRespQueue != IPC_Q_NONE)
					{
						msgSend(&Message, uRespQueue);
					}
					break;
				}

				bWarmStart = *(IEC_BOOL *)Message.pData;

				if (uRespQueue != IPC_Q_NONE)
				{
					Message.uLen = 0;

					uRes = msgSend(&Message, uRespQueue);
					TR_RET(uRes);
				}

				uRes = tstFinalize(uIOLayer, &IO);
				TR_RET(uRes);

				uResTerm	= uRes;
				uTermDone	= OK;

			  #if defined(RTS_CFG_IO_TRACE)
				if (uResTerm != OK)
				{
					osTrace("--- IO%d: ERROR: Termination failed: Channel:%d Name:%s\r\n", uIOLayer, IO.usChannel, IO.szName);
				}
			  #endif

				break;
			}

			case MSG_CG_TERM_DONE:	/* ---------------------------------------- */
			{
				uTermDone = (IEC_UINT)((Message.uLen == sizeof(IEC_UINT)) ? *(IEC_UINT *)Message.pData : ERR_INVALID_PARAM);

			  #if defined(RTS_CFG_IO_TRACE)
				if (uTermDone == OK)
				{
					osTrace("--- IO%d: Termination finished: Channel:%d Name:%s\r\n", uIOLayer, IO.usChannel, IO.szName);
				}
				else
				{
					osTrace("--- IO%d: ERROR: Termination failed: Channel:%d Name:%s\r\n", uIOLayer, IO.usChannel, IO.szName);
				}
			  #endif

				break;
			}

			case MSG_IO_TERM_RES: /* ------------------------------------------ */ 
			{				
				/* OK ................ Clean up successfully finished.
				 * Error number ...... Clean up failed.
				 * WRN_IN_PROGRESS ... Clean up not yet finished.
				 */
				if (uTermDone != WRN_IN_PROGRESS || uResTerm != OK)
				{
					TR_RET(uRes);
				}

				if (uRespQueue != IPC_Q_NONE)
				{
					Message.uLen = sizeof(IEC_UINT);
					*(IEC_UINT *)Message.pData = (IEC_UINT)(uResTerm != OK ? uResTerm : uTermDone);

					uRes = msgSend(&Message, uRespQueue);
					TR_RET(uRes);
				}
				
				if (uTermDone != WRN_IN_PROGRESS || uResTerm != OK)
				{
					osSleep(20);
					
					uRes = osDestroyIPCQueue((IEC_UINT)(Q_OFFS_IO + uIOLayer));
					TR_RET(uRes);

					OS_MEMSET(&IO, 0x00, sizeof(SIOConfig));

					RETURN(OK);
				}

				break;
			}

			default:			/* -------------------------------------------- */
			{
			  #if defined(RTS_CFG_IO_TRACE)
				osTrace("--- IO%d: Unexpected Message (0x%04x) received.\r\n", uIOLayer, Message.uID);
			  #endif
				break;
			}

		} /* switch(Message.uID) */

	} /* for ( ; ; ) */
	
	RETURN(OK);
}

#endif /* RTS_CFG_IOTEST */

/* ---------------------------------------------------------------------------- */
