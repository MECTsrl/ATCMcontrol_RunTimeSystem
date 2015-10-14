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
 * Filename: vmmCom.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"vmmCom.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_TCP_NATIVE)
  #include "osSocket.h"
#endif

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

#if defined(RTS_CFG_DEBUG_OUTPUT) & defined(RTS_CFG_COMM_TRACE)

  IEC_CHAR *szCmdText[] = CMD_TEXT_VMM;

#endif

/* ----  Local Functions:	--------------------------------------------------- */

#if defined(RTS_CFG_TCP_NATIVE)
	#define xxxWriteBlock		comWriteBlock
	#define xxxReadBlock		comReadBlock
	#define xxxCheckIO			comCheckIO
#else
	#define xxxWriteBlock		osWriteBlock
	#define xxxReadBlock		osReadBlock
	#define xxxCheckIO			osCheckIO
#endif

#if defined(RTS_CFG_PROT_BLOCK)
  static IEC_UINT comCheckCRC(XFrame *pFrame);
  static IEC_UINT comBuildCRC(XFrame *pFrame);
  static IEC_UINT comSendAcknowledge(STaskInfoVMM *pVMM, XFrame *pFrame, IEC_BYTE byBlock);
#endif

#if defined(RTS_CFG_TCP_NATIVE)
  static IEC_UINT comReadBlock(STaskInfoVMM *pVMM, IEC_DATA *pData, IEC_UINT uLen);
  static IEC_UINT comWriteBlock(STaskInfoVMM *pVMM, IEC_DATA *pData, IEC_UINT uLen);

  static IEC_UINT sockRecv(VMS_SOCKET hSocket, IEC_DATA *pData, IEC_UINT *upLen);
  static IEC_UINT sockSend(VMS_SOCKET hSocket, IEC_DATA *pData, IEC_UINT uLen);
  static IEC_UINT sockClose(VMS_SOCKET hSocket);
#endif

#if defined(RTS_CFG_PROT_BLOCK) && defined(RTS_CFG_TCP_NATIVE)
  static IEC_UINT comCheckIO(STaskInfoVMM *pVMM);
#endif

IEC_UINT sysGetCommPort(IEC_UINT *upPort);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * comBlock
 *
 */
#if defined(RTS_CFG_PROT_BLOCK)

IEC_UINT comBlock(STaskInfoVMM *pVMM)
{
	SComVMM *pCOM	= &pVMM->COM;
	IEC_BOOL bFirst = TRUE;

	while (bFirst || (pCOM->uState != CS_RECEIVING && pCOM->uState != CS_SENDING))
	{
		switch (pCOM->uState)
		{
			case CS_READY_TO_RECEIVE:
			{
				/* Start a read IO 
				 */
				IEC_UINT uError = xxxReadBlock(pVMM, (IEC_DATA *)&pCOM->RecvFrame, sizeof(pCOM->RecvFrame));

				if (uError == WRN_NO_CONNECTION)
				{
					/* No error, no connection to OPC++ server.
					 */
					RETURN(OK);
				}

				if (uError != OK)
				{
					RETURN(ERR_ERROR);
				}
				
				pCOM->uState = CS_RECEIVING;
				
				break;
			
			} /* ------------------------------------------------------------- */

			case CS_RECEIVING:
			{
				/* Poll read IO operation
				 */
				switch (xxxCheckIO(pVMM))
				{
					case CIO_RECEIVING: 	/* Still waiting, no time out */
					{
						break;
					}

					case CIO_DATA:			/* Data block ready */
					{
						pCOM->uState = CS_BLOCK_RECEIVED;
						break;
					}

					default:				/* Time out or communication error */
					{
						pCOM->uState = CS_READY_TO_RECEIVE;
						break;
					}
				} 
				
				break;
			
			} /* ------------------------------------------------------------- */

			case CS_BLOCK_RECEIVED:
			{
				/* Check received data block
				 */
				if (comCheckCRC(&pCOM->RecvFrame) != OK)
				{
					pCOM->uState = CS_READY_TO_RECEIVE;
					break;
				}

				if (pCOM->RecvFrame.byType == BT_REQ)
				{
					pCOM->uState = CS_REQ_RECEIVED;
					break;
				}
				else if (pCOM->RecvFrame.byType == BT_DATA)
				{
					pCOM->uState = CS_DATA_RECEIVED;
					break;
				}

				pCOM->uState = CS_READY_TO_RECEIVE;

				break;
			
			} /* ------------------------------------------------------------- */

			case CS_DATA_RECEIVED:
			{
				if (pCOM->RecvFrame.BLK.uBlock == 1)
				{
					/* Restart
					 */
					pCOM->uCurrDat	= 0;
					pCOM->uCurrReq	= 0;
				}

			  #if defined(RTS_CFG_DEBUG_OUTPUT) & defined(RTS_CFG_COMM_TRACE)
				{
					IEC_UINT uCmd = pCOM->RecvFrame.BLK.CMD.byCommand;

					osTrace(">>> %-8s %2d %5d    ", uCmd <= LAST_CMD_VALUE ? szCmdText[uCmd] : "Invalid", 
								pCOM->RecvFrame.BLK.uBlock, pCOM->RecvFrame.BLK.uLen);
				}
			  #endif /* RTS_CFG_DEBUG_OUTPUT & RTS_CFG_COMM_TRACE */

				if ((pCOM->RecvFrame.BLK.uBlock != pCOM->uCurrDat && pCOM->RecvFrame.BLK.uBlock != pCOM->uCurrDat + 1))
				{
					pCOM->byBlock = BT_NACK;
					pCOM->uState  = CS_SENDING;
				}
				else
				{
					pCOM->byBlock = BT_ACK;
					pCOM->uState  = CS_PROC_DATA;
				}

				if (comSendAcknowledge(pVMM, &pCOM->RecvFrame, pCOM->byBlock) != OK)
				{
					pCOM->uState = CS_READY_TO_RECEIVE;
					break;
				}

				break;
			
			} /* ------------------------------------------------------------- */

			case CS_PROC_DATA:
			{
				if (pCOM->RecvFrame.BLK.uBlock != pCOM->uCurrDat)
				{
					pCOM->uCurrDat++;

					cmdRun(pVMM, SIG_DATA, &pCOM->RecvFrame.BLK);
				}

				pCOM->uState = CS_SENDING;

				break;
			
			} /* ------------------------------------------------------------- */

			case CS_REQ_RECEIVED:
			{
			  #if defined(RTS_CFG_DEBUG_OUTPUT) & defined(RTS_CFG_COMM_TRACE)

				osTrace(">>> %-8s %2d %5d    ", "REQ", pCOM->RecvFrame.BLK.uBlock, pCOM->RecvFrame.BLK.uLen);
				
			  #endif /* RTS_CFG_DEBUG_OUTPUT & RTS_CFG_COMM_TRACE */

				if ((pCOM->RecvFrame.BLK.uBlock != pCOM->uCurrReq && pCOM->RecvFrame.BLK.uBlock != pCOM->uCurrReq + 1))
				{
					pCOM->uState = CS_READY_TO_RECEIVE;
					break;
				}

				pCOM->uState = CS_PROC_RESPONSE;

				break;
			
			} /* ------------------------------------------------------------- */

			case CS_PROC_RESPONSE:
			{
				if (pCOM->RecvFrame.BLK.uBlock != pCOM->uCurrReq)
				{
					OS_MEMCPY (&pCOM->SendFrame, &pCOM->RecvFrame, HD_FRAME - HD_COMMAND);
					
					pCOM->SendFrame.byType	 = BT_DATA;
					pCOM->SendFrame.BLK.uLen = 0;

					if (cmdRun(pVMM, SIG_RESPONSE, &pCOM->SendFrame.BLK) != OK)
					{
						pCOM->uState = CS_READY_TO_RECEIVE;
						break;
					}

					comBuildCRC(&pCOM->SendFrame);
				}
				else
				{
					;	/* re-send the last block */
				}

			  #if defined(RTS_CFG_DEBUG_OUTPUT) & defined(RTS_CFG_COMM_TRACE)
				{
					IEC_UINT uCmd = pCOM->SendFrame.BLK.CMD.byCommand;

					osTrace("<<< %-8s %2d %5d\r\n", uCmd <= LAST_CMD_VALUE ? szCmdText[uCmd] : "-FAILED-", 
								pCOM->SendFrame.BLK.uBlock, pCOM->SendFrame.BLK.uLen);
				}
			  #endif /* RTS_CFG_DEBUG_OUTPUT & RTS_CFG_COMM_TRACE */

				/* Start a read IO 
				 */
				if (xxxWriteBlock(pVMM, (IEC_DATA *)&pCOM->SendFrame, (IEC_UINT)(HD_FRAME + pCOM->SendFrame.BLK.uLen)) != OK)
				{
					pCOM->uState = CS_READY_TO_RECEIVE;
					break;
				}
				
				pCOM->byBlock = BT_REQ;
				pCOM->uState  = CS_SENDING;

				break;
			
			} /* ------------------------------------------------------------- */

			case CS_SENDING:
			{
				/* Poll write IO operation
				 */
				switch (xxxCheckIO(pVMM))
				{
					case CIO_SENDING:
					{
						break;					/* Still sending, no time out */
					}

					case CIO_READY: 			/* Data block sent */
					{
						if (pCOM->byBlock == BT_REQ)
						{
							if (pCOM->SendFrame.BLK.uBlock != pCOM->uCurrReq)
							{
								/* Increment only once!
								 */
								pCOM->uCurrReq++;
							}
						}

						pCOM->uState = CS_READY_TO_RECEIVE;
						break;
					}

					default:					/* Time out or communication error */
					{
						pCOM->uState = CS_READY_TO_RECEIVE;
						break;
					}
				} 
						
				break;
			
			} /* ------------------------------------------------------------- */

			default:
			{
				pCOM->uState = CS_READY_TO_RECEIVE;

				break;
			
			} /* ------------------------------------------------------------- */
		
		} /* switch (pCOM->uState) */

		bFirst = FALSE;

	} /* while */

	RETURN(OK);
}

#endif /* RTS_CFG_PROT_BLOCK */


/* ---------------------------------------------------------------------------- */
/**
 * comCheckCRC
 *
 */
#if defined(RTS_CFG_PROT_BLOCK)

static IEC_UINT comCheckCRC(XFrame *pFrame)
{
	IEC_DATA *pData 	= (IEC_DATA *)pFrame;
	IEC_BYTE byCheck	= pFrame->byCRC;
	IEC_BYTE byCRC		= 0;
	IEC_UINT i;

	if (pFrame->BLK.uLen > MAX_DATA)
	{
		RETURN(ERR_CRC);
	}

	pFrame->byCRC = 0;

	for (i = 0; i < HD_FRAME + pFrame->BLK.uLen; i++)
	{
		byCRC = (IEC_BYTE)(byCRC + pData[i]);
	}

	RETURN((IEC_UINT)(byCheck == byCRC ? OK : ERR_CRC));
}

#endif /* RTS_CFG_PROT_BLOCK */


/* ---------------------------------------------------------------------------- */
/**
 * comBuildCRC
 *
 */
#if defined(RTS_CFG_PROT_BLOCK)

static IEC_UINT comBuildCRC(XFrame *pFrame)
{
	IEC_DATA *pData = (IEC_DATA *)pFrame;
	IEC_BYTE byCRC = 0;
	IEC_UINT i;

	pFrame->byCRC = 0;

	for (i = 0; i < HD_FRAME + pFrame->BLK.uLen; i++)
	{
		byCRC = (IEC_BYTE)(byCRC + pData[i]);
	}

	pFrame->byCRC = byCRC;

	RETURN(OK);
}

#endif /* RTS_CFG_PROT_BLOCK */


/* ---------------------------------------------------------------------------- */
/**
 * comSendAcknowledge
 *
 */
#if defined(RTS_CFG_PROT_BLOCK)

static IEC_UINT comSendAcknowledge(STaskInfoVMM *pVMM, XFrame *pFrame, IEC_BYTE byBlock)
{
	XFrame *pSend = &pVMM->COM.SendFrame;
	IEC_UINT uResult;

	OS_MEMCPY(pSend, pFrame, HD_FRAME);

	pSend->byType	= byBlock;
	pSend->BLK.uLen = 0;

	comBuildCRC(pSend);

  #if defined(RTS_CFG_DEBUG_OUTPUT) & defined(RTS_CFG_COMM_TRACE)
	{
		if (pSend->byType == BT_ACK)
		{
			osTrace("<<< %-8s %2d %5d\r\n", "ACK", pSend->BLK.uBlock, pSend->BLK.uLen);
		}
		else
		{
			osTrace("<<< %-8s %2d %5d\r\n", "NACK", pSend->BLK.uBlock, pSend->BLK.uLen);
		}
	}
  #endif /* RTS_CFG_DEBUG_OUTPUT & RTS_CFG_COMM_TRACE */

	uResult = xxxWriteBlock(pVMM, (IEC_DATA *)pSend, HD_FRAME);
	
	RETURN(uResult);
}

#endif /* RTS_CFG_PROT_BLOCK */


/* ---------------------------------------------------------------------------- */
/**
 * comClient
 *
 */
#if defined(RTS_CFG_PROT_CLIENT)

IEC_UINT comClient(STaskInfoVMM *pVMM)
{
	IEC_UINT uLen;
	IEC_UINT uRes;
	XBlock	 Block;

  #if defined(RTS_CFG_TCP_NATIVE)
	if (pVMM->pTCP[0].hSocket == VMS_INVALID_SOCKET)
	{
		return WRN_NO_CONNECTION;
	}
  #endif

	uLen = HD_BLOCK + MAX_DATA;

	uRes = xxxReadBlock(pVMM, (IEC_DATA *)&Block, uLen);
	
	if (uRes == WRN_NO_CONNECTION)
	{
		RETURN(OK);
	}

	if (uRes != OK)
	{
		RETURN(uRes);
	}

	Block.usSource = 0; /* Only message source possible! */

	cmdExecute(pVMM, &Block);

	uRes = xxxWriteBlock(pVMM, (IEC_DATA *)&Block, (IEC_UINT)(HD_BLOCK + Block.uLen));

	if (uRes == WRN_NO_CONNECTION)
	{
		RETURN(OK);
	}

	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(OK);
}

#endif /* RTS_CFG_PROT_CLIENT */


/* ---------------------------------------------------------------------------- */
/**
 * comReadBlock
 *
 */
#if defined(RTS_CFG_TCP_NATIVE)

static IEC_UINT comReadBlock(STaskInfoVMM *pVMM, IEC_DATA *pData, IEC_UINT uLen)
{
	IEC_UINT uRes	= OK;

	SComTCP *pTCP = pVMM->pTCP;
		
	if (pTCP->hSocket == VMS_INVALID_SOCKET)
	{				
		osSleep(100);
		return WRN_NO_CONNECTION;
	}
	
	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

	OS_BEGIN_CRITICAL_SECTION(SEM_COMMUNICATION)
	{
		uRes = sockRecv((VMS_SOCKET)pTCP->hSocket, pData, &uLen);
		if (uRes != OK)
		{
			sockClose((VMS_SOCKET)pTCP->hSocket);
			pTCP->hSocket	= VMS_INVALID_SOCKET;

			pTCP->uState	= CIO_READY;
			pTCP->uLen		= 0;
			
			uRes = WRN_NO_CONNECTION;
		}

		if (uRes == OK && uLen == 0)
		{
			pTCP->uState	= CIO_READY;
			pTCP->uLen		= 0;

			uRes = WRN_NO_CONNECTION;
		}

		if (uRes == OK)
		{
			pTCP->uState	= CIO_RECEIVING;
			pTCP->uLen		= uLen;
		}

	} OS_END_CRITICAL_SECTION(SEM_COMMUNICATION)

	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  -------------------- */
	
	RETURN(uRes);
}

#endif /* RTS_CFG_TCP_NATIVE */


/* ---------------------------------------------------------------------------- */
/**
 * comCheckIO
 *
 */
#if defined(RTS_CFG_PROT_BLOCK) && defined(RTS_CFG_TCP_NATIVE)

static IEC_UINT comCheckIO(STaskInfoVMM *pVMM)
{	
	SComTCP *pTCP = pVMM->pTCP;

	switch (pTCP->uState)
	{
		case CIO_SENDING:
		{
			pTCP->uState = CIO_READY;
			break;
		}

		case CIO_RECEIVING:
		{
			/* There is no real state "RECEIVUNG" or "SENDING", the 
			 * data is already processed. Just tell this to the VMM.
			 */
			pTCP->uState = CIO_DATA;
			break;
		}

		default:
		{
			pTCP->uState = CIO_READY;
			break;
		}
	}
	
	return pTCP->uState;
}

#endif /* RTS_CFG_PROT_BLOCK && RTS_CFG_TCP_NATIVE */


/* ---------------------------------------------------------------------------- */
/**
 * comWriteBlock
 *
 */
#if defined(RTS_CFG_TCP_NATIVE)

static IEC_UINT comWriteBlock(STaskInfoVMM *pVMM, IEC_DATA *pData, IEC_UINT uLen)
{
	IEC_UINT uRes = OK;
	SComTCP *pTCP = pVMM->pTCP;
	
	if (pTCP->hSocket == VMS_INVALID_SOCKET)
	{
		pTCP->uState = CIO_READY;
		pTCP->uLen	 = 0;

		return WRN_NO_CONNECTION;
	}

	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

	OS_BEGIN_CRITICAL_SECTION(SEM_COMMUNICATION)
	{
		uRes = sockSend((VMS_SOCKET)pTCP->hSocket, pData, uLen);
		if (uRes != OK)
		{
			sockClose((VMS_SOCKET)pTCP->hSocket);
			pTCP->hSocket	= VMS_INVALID_SOCKET;
			
			pTCP->uState	= CIO_READY;
			pTCP->uLen		= 0;

			uRes = WRN_NO_CONNECTION;
		}

		if (uRes == OK)
		{
			pTCP->uState	= CIO_SENDING;
			pTCP->uLen		= 0;
		}

	} OS_END_CRITICAL_SECTION(SEM_COMMUNICATION)

	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ------------------------ */

	RETURN(uRes);
}

#endif /* RTS_CFG_TCP_NATIVE */


/* ---------------------------------------------------------------------------- */
/**
 * sockInitialize
 *
 */
#if defined(RTS_CFG_TCP_NATIVE)

IEC_UINT sockInitialize(STaskInfoVMM *pVMM)
{
	IEC_UINT i;
	SComTCP *pTCP = pVMM->pTCP;

	for (i = 0; i < MAX_CONNECTIONS; i++)
	{
		pTCP[i].hSocket = VMS_INVALID_SOCKET;
	}

	RETURN(OK);
}

#endif /* RTS_CFG_TCP_NATIVE */


/* ---------------------------------------------------------------------------- */
/**
 * sockRecv - visSockRecv
 */
#if defined(RTS_CFG_TCP_NATIVE)

static IEC_UINT sockRecv(VMS_SOCKET hSocket, IEC_DATA *pData, IEC_UINT *upLen)
{
	IEC_UINT  uDataLen	   = 0;
	IEC_UDINT ulTransfered = 0;

	for(;;)
	{
		ulTransfered = osRecv(hSocket, (IEC_CHAR OS_LPTR *)(pData + uDataLen), *upLen - uDataLen, 0);

		if (ulTransfered == VMS_SOCKET_ERROR)
		{
			if (VMS_ERRNO != VMS_ETIMEDOUT)
			{
				/* Any error except a timeout signals a bad connection
				 */
				TR_ERROR("[TCP] ERROR(%d): Receive failed.\r\n", VMS_ERRNO);
				RETURN(ERR_ERROR);
			}

			/* Timeout - continue receiving
			 */
			continue;
		}

		if (ulTransfered == 0)
		{
			/* Connection closed by client
			 */
			return ERR_ERROR;
		}
			
		uDataLen = (IEC_UINT)(uDataLen + ulTransfered);

		if (uDataLen >= *upLen)
		{
			RETURN(OK);
		}

	  #if defined(RTS_CFG_PROT_CLIENT) || defined(RTS_CFG_MULTILINK)
		if (uDataLen >= HD_BLOCK && uDataLen == ((XBlock *)pData)->uLen 	+ HD_BLOCK)
		{
	  #endif

	  #if defined(RTS_CFG_PROT_BLOCK)
		if (uDataLen >= HD_FRAME && uDataLen == ((XFrame *)pData)->BLK.uLen + HD_FRAME)
		{
	  #endif
			*upLen = uDataLen;
			RETURN(OK);
		}

	} /* while (1) */

	RETURN(ERR_ERROR);
}

#endif /* RTS_CFG_TCP_NATIVE */


/* ---------------------------------------------------------------------------- */
/**
 * sockSend - visSockSend
 */
#if defined(RTS_CFG_TCP_NATIVE)

static IEC_UINT sockSend(VMS_SOCKET hSocket, IEC_DATA *pData, IEC_UINT uLen)
{
	IEC_UINT  uDataLen	   = 0;
	IEC_UDINT ulTransfered = 0;

	for(;;)
	{
		ulTransfered = osSend(hSocket, (const IEC_CHAR OS_LPTR *)(pData + uDataLen), uLen - uDataLen, 0);

		if (ulTransfered == VMS_SOCKET_ERROR)
		{
			if (VMS_ERRNO != VMS_ETIMEDOUT)
			{
				/* Any error except a timeout signals a bad connection
				 */
				TR_ERROR("[TCP] ERROR(%d): Send failed.\r\n", VMS_ERRNO);
				RETURN(ERR_ERROR);
			}

			/* Timeout - continue sending
			 */
			continue;
		}

		if (ulTransfered == 0)
		{
			/* Connection closed by client
			 */
			return ERR_ERROR;
		}

		uDataLen = (IEC_UINT)(uDataLen + ulTransfered);

		if (uDataLen >= uLen)
		{
			RETURN(OK);
		}
	
	} /* while (1) */

	RETURN(ERR_ERROR);
}

#endif /* RTS_CFG_TCP_NATIVE */


/* ---------------------------------------------------------------------------- */
/**
 * sockClose
 */
#if defined(RTS_CFG_TCP_NATIVE)

static IEC_UINT sockClose(VMS_SOCKET hSocket)
{
	TR_STATE("[TCP] Working socket closed.\r\n");

	if (osShutDown(hSocket, VMS_SD_BOTH) != 0)
	{
		TR_ERROR("[TCP] ERROR(%d): Shutdown failed.\r\n", VMS_ERRNO);
	}

	if (osCloseSocket(hSocket) != 0)
	{
		TR_ERROR("[TCP] ERROR(%d): Close failed.\r\n", VMS_ERRNO);
	}

	RETURN(OK);
}

#endif /* RTS_CFG_TCP_NATIVE */


/* ---------------------------------------------------------------------------- */
/**
 * sockListen
 */
#if defined(RTS_CFG_TCP_NATIVE)

IEC_UINT sockListen(void *pPara)
{
	static VMS_SOCKET hListen	= VMS_INVALID_SOCKET;

	IEC_UINT		uRes		= OK;

	VMS_sockaddr_in Addr;
	VMS_fd_set		list;
	VMS_timeval 	timeout;

	IEC_BOOL		bFirst		= TRUE;

	IEC_UINT		uPort		= 0xffffu;

	SComTCP *pTCP = (SComTCP *)pPara;
	if (pTCP == 0)
	{
		RETURN(ERR_ERROR);
	}

  #if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo(TASK_OFFS_COM_LIS, osGetTaskID());
	TR_RET(uRes);
  #endif

	if(hListen == VMS_INVALID_SOCKET)
	{
		TR_STATE("[TCP] Creating listening socket.\r\n");
		
		/* Create a socket for listening on the port for new connections
		 */
		hListen = osSocket(VMS_AF_INET, VMS_SOCK_STREAM, VMS_IPPROTO_TCP);
		if (hListen == VMS_INVALID_SOCKET)
		{
			TR_ERROR("[TCP] ERROR(%d): Create Listen socket failed.\r\n", VMS_ERRNO);
			RETURN(ERR_ERROR);
		}

		{
			IEC_BOOL bTrue = TRUE;

		  #ifdef DEBUG
			osSetSockOpt(hListen, VMS_SOL_SOCKET, VMS_SO_DEBUG, (char *)&bTrue, sizeof(bTrue));
		  #endif

			osSetSockOpt(hListen, VMS_SOL_SOCKET, VMS_SO_REUSEADDR , (char *)&bTrue, sizeof(bTrue));
		}

		/* Bind the socket to the port, waiting for any address
		 */
		uRes = sysGetCommPort(&uPort);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		memset(&Addr, 0, sizeof(Addr));
		Addr.sin_family 	 = VMS_AF_INET;
		Addr.sin_port		 = htons(uPort);
		Addr.sin_addr.s_addr = htonl(VMS_INADDR_ANY);
		
#ifdef __XENO__
		if (osBind(hListen, (struct sockaddr *)(&Addr), sizeof(Addr)) != VMS_RET_OK)
		{
			TR_ERROR("[TCP] ERROR(%d): Bind failed.\r\n", VMS_ERRNO);
			RETURN(ERR_ERROR);
		}
#else
		while (osBind(hListen, (struct sockaddr *)(&Addr), sizeof(Addr)) != VMS_RET_OK)
		{
			TR_ERROR("[TCP] ERROR(%d): Bind failed. Retries in 5 sec ...\r\n", VMS_ERRNO);
			//RETURN(ERR_ERROR);
			sleep(5);
		}
#endif
		TR_STATE("[TCP] bind done.\r\n");
		
		/* Set the port into passive (listening) mode
		 */
		if (osListen(hListen, VMS_BACKLOG) != VMS_RET_OK)
		{
			TR_ERROR("[TCP] ERROR(%d): Listen failed.\r\n", VMS_ERRNO);
			RETURN(ERR_ERROR);
		}
	}

	while (bFirst)
	{
		IEC_DINT nSelRet = 0;
		IEC_UINT i;
		IEC_BOOL bContinue = FALSE;

		while (nSelRet == 0)
		{
			/* Create a list of (one) sockets, waiting for a connection
			 */ 	
		  #if defined(MS_VCC_60)
		  #pragma warning(disable:4127)
		  #endif
			VMS_FD_ZERO(&list);
			VMS_FD_SET(hListen, &list);
		  #if defined(MS_VCC_60)
		  #pragma warning(default:4127)
		  #endif
			
			/* Wait for one second (blocking)
			 */
			timeout.tv_sec	= VMS_TO_SELECT_SEC;
			timeout.tv_usec = VMS_TO_SELECT_USEC;
			
			/* select (the only) one connection waiting for this server
			 */
			nSelRet = osSelect(VMS_SELPAR_LISTEN, &list, 0, 0, &timeout);
			if (nSelRet == VMS_SOCKET_ERROR)
			{
				/* An error occurred -> try it again
				 */
				TR_ERROR("[TCP] ERROR(%d): Select failed.\r\n", VMS_ERRNO);
				osSleep(VMS_TCP_ERROR_DELAY);
				nSelRet = 0;
			}
		
		} /* while(nSelRet == 0) */

		/* The new connection has to be on the (only) listening socket
		 */
		if (! (VMS_FD_ISSET(hListen, &list)))
		{
			TR_ERROR("[TCP] ERROR(%d): ISSET failed.\r\n", VMS_ERRNO);
			osSleep(VMS_TCP_ERROR_DELAY);
			continue;
		}

		/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  ---------------- */
		
		OS_BEGIN_CRITICAL_SECTION(SEM_COMMUNICATION)
		{
			bContinue = FALSE;

			/* Look for a free working socket
			 */
			for (i = 0; i < MAX_CONNECTIONS; i++)
			{
				if (pTCP[i].hSocket == VMS_INVALID_SOCKET)
				{
					break;
				}
			}

			if (i == MAX_CONNECTIONS)
			{
				TR_STATE("[TCP] No free connection.\r\n");
				osSleep(VMS_TCP_ERROR_DELAY);

				bContinue = TRUE;
				goto end_crit_sec;
			}

			TR_STATE1("[TCP] Creating new working socket (%d).\r\n", i);

			/* Get the new working socket for the following communications
			 */
			pTCP[i].hSocket = osAccept(hListen, 0, 0);
			if (pTCP[i].hSocket == VMS_INVALID_SOCKET)
			{
				TR_ERROR("[TCP] ERROR(%d): Accept failed.\r\n", VMS_ERRNO);
				osSleep(VMS_TCP_ERROR_DELAY);

				bContinue = TRUE;
				goto end_crit_sec;
			}

			{
				/* Set the timeouts for the working socket. The timeout have no effect on 
				 * detection connection breaks. After each timeout an other receive or 
				 * send call is done, a infinite timeout might be possible too.
				 */
				IEC_UDINT ulTimeOut = VMS_TCP_TIMEOUT;

				osSetSockOpt(pTCP[i].hSocket, VMS_SOL_SOCKET, VMS_SO_RCVTIMEO, (const IEC_CHAR OS_LPTR *)&ulTimeOut, sizeof(ulTimeOut));
				osSetSockOpt(pTCP[i].hSocket, VMS_SOL_SOCKET, VMS_SO_SNDTIMEO, (const IEC_CHAR OS_LPTR *)&ulTimeOut, sizeof(ulTimeOut));
			}

		  end_crit_sec:
			;			

		} OS_END_CRITICAL_SECTION(SEM_COMMUNICATION);

		/* <<<	C R I T I C A L   S E C T I O N  -	E N D  -------------------- */

		if (bContinue == TRUE)
		{
			/* Error handling for critical section from above.
			 */
			continue;
		}

		bContinue = FALSE;

	  #if defined(RTS_CFG_MULTILINK)

		pTCP[i].uTask = i;

		if (osCreateCommTask(pTCP + i) != OK)
		{
			/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  ------------ */

			OS_BEGIN_CRITICAL_SECTION(SEM_COMMUNICATION);
			{
				osShutDown(pTCP[i].hSocket, VMS_SD_BOTH);
				osCloseSocket(pTCP[i].hSocket);
				
				pTCP[i].hSocket = VMS_INVALID_SOCKET;
			
			} OS_END_CRITICAL_SECTION(SEM_COMMUNICATION);

			/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ---------------- */

			TR_STATE("[TCP] ERROR: Create working task failed.\r\n");
			osSleep(VMS_TCP_ERROR_DELAY);

			continue;
		}

	  #endif

		TR_STATE("[TCP] Connected.\r\n");
	
	} /* while (bFirst) */

	RETURN(OK);
}

#endif /* RTS_CFG_TCP_NATIVE */


/* ---------------------------------------------------------------------------- */
/**
 * sockComm
 */
#if defined(RTS_CFG_TCP_NATIVE) && defined(RTS_CFG_MULTILINK)

IEC_UINT sockComm(void *pPara)
{
	SComTCP 	*pTCP	= (SComTCP *)pPara;
	SMessage	Message;
	
	IEC_ULINT ullT;

	IEC_UINT uRes	= OK;
	IEC_UINT uQueue = (IEC_UINT)(pTCP->uTask + IPC_VMM_QUEUES + MAX_TASKS);
	
	if (pTCP->uTask >= MAX_CONNECTIONS || pTCP->hSocket == VMS_INVALID_SOCKET)
	{
		RETURN(ERR_ERROR);
	}

	if (osCreateIPCQueue(uQueue) != OK)
	{
		RETURN(ERR_ERROR);
	}

  #if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo((IEC_UINT)(TASK_OFFS_COM_WRK + pTCP->uTask), osGetTaskID());
	TR_RET(uRes);
  #endif

	for( ; ; )
	{
		XBlock	  *pBlock = (XBlock *)Message.pData;
		
		IEC_BYTE  bySequence;
		IEC_BYTE  byCommand;

		IEC_BOOL  bSend;

		IEC_UDINT ulTimeOut;

		Message.uLen = HD_BLOCK + MAX_DATA;

		if (sockRecv((VMS_SOCKET)pTCP->hSocket, Message.pData, &Message.uLen) != OK)
		{
			/* Connection terminated
			 */
			break;
		}
		
		byCommand  = pBlock->CMD.byCommand;
		bySequence = pBlock->CMD.bySequence;

		bSend		= TRUE;
		ullT		= osGetTimeUS();
		ulTimeOut	= VMM_TO_EXT_MSG;

		for ( ; ;)
		{
			Message.uID 		= MSG_CT_DATA;
			Message.uRespQueue	= uQueue;

			pBlock->usSource	= (IEC_BYTE)pTCP->uTask;

			ulTimeOut = bSend == TRUE ? VMM_TO_EXT_MSG : ulTimeOut - utilGetTimeDiffMS(ullT);
			if (ulTimeOut > VMM_TO_EXT_MSG)
			{
				/* Avoid overflow
				 */
				ulTimeOut = VMM_NO_WAIT;
			}

			uRes = msgTXMessage(&Message, Q_LIST_VMM, VMM_TO_EXT_MSG, bSend);
			if (uRes != OK)
			{
				/* IPC error; build error message for OPC server.
				 */
				pBlock->uBlock	= 1;
				pBlock->uLen	= sizeof(uRes);
				pBlock->byLast	= TRUE;
				
				pBlock->CMD.bySequence			= bySequence;
				pBlock->CMD.byCommand			= (IEC_BYTE)(byCommand | 0x80u);
				*(IEC_UINT *)pBlock->CMD.pData	= (IEC_UINT)(uRes != WRN_TIME_OUT ? uRes : ERR_IPC_VMM_TIMEOUT);

				Message.uLen	= (IEC_UINT)(HD_BLOCK + pBlock->uLen);

				break;
			}

			if (byCommand != (pBlock->CMD.byCommand & ~0x80u) || bySequence != pBlock->CMD.bySequence)
			{
				/* Not fitting response block, probably old message in queue --> skip!
				 */
				bSend = FALSE;
				continue;
			}

			break;
		}

		if (sockSend((VMS_SOCKET)pTCP->hSocket, Message.pData, Message.uLen) != OK)
		{
			/* Connection terminated
			 */
			break;
		}
	
	} /* while(1) */

	/* Notify the VMM
	 */
	Message.uID 		= MSG_CT_TERM;
	Message.uLen		= sizeof(pTCP->uTask);

	*(IEC_UINT *)Message.pData	= pTCP->uTask;
	Message.uRespQueue			= uQueue;
	
	msgTXMessage(&Message, Q_LIST_VMM, VMM_TO_IPC_MSG, TRUE);

	/* Clean Up
	 */
	sockClose((VMS_SOCKET)pTCP->hSocket);

	if (osDestroyIPCQueue(uQueue) != OK)
	{
		RETURN(ERR_ERROR);
	}

	pTCP->hSocket	= VMS_INVALID_SOCKET;
	pTCP->hTask 	= 0;

  #if defined(RTS_CFG_SYSLOAD)
	uRes = ldClearTaskInfo((IEC_UINT)(TASK_OFFS_COM_WRK + pTCP->uTask));
	TR_RET(uRes);
  #endif

	RETURN(OK);
}

#endif /* RTS_CFG_TCP_NATIVE && (RTS_CFG_MULTILINK) */


/* ---------------------------------------------------------------------------- */
