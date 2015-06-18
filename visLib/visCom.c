
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
 * Filename: visCom.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"visCom.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "visShared.h"

#include "osSocket.h"

#include "visDef.h"
#include "visMain.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

#include <arpa/inet.h>

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

static VMS_SOCKET g_hSock = VMS_INVALID_SOCKET;

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * domInitialize
 *
 * Initializes the TCP/IP communication interface of the VisuComm library.
 *
 */
IEC_UINT domInitComm(SVisInfo *pVI)
{
	IEC_UINT uRes = OK;

	/* Call target specific initializations
	 */
	uRes = domVisuCommInit();
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetPort
 *
 * Iterates through all possible TCP/IP 4C RTS port numbers. (To do a port scan
 * in the order given by i.)
 *
 */
IEC_UINT domGetPort(IEC_UINT i)
{
	switch(i)
	{
		case 0:  return 17290;	/* FCTPAC020 */
		case 1:  return 17275;	/* 4CFC 	> 21000 */
		case 2:  return 17276;	/* 4CBC 			*/
		case 3:  return 17280;	/* 4CGA				*/
		case 4:  return 17264;	/* 4CWin	> 20500 */
		case 5:  return 17265;	/* 4CVxW			*/
		case 6:  return 17270;	/* DCU01			*/
		case 7:  return 17271;	/* CMZ				*/
		case 8:  return 17273;	/* 4COsaiCE 		*/
		case 9:  return 17267;	/* 4CDemo			*/
		case 10:  return 17274;	/* 4CSDK			*/
		case 11: return 17278;	/* 4CDC 			*/
		case 12: return 17292;	/* Gekko 			*/
	}

	return 0;	/* Port no. 0 is exit value for port scan! */
}


/* ---------------------------------------------------------------------------- */
/**
 * domOpenComm
 *
 * Creates a TCP/IP connection to a remote FarosPLC Run Time System.
 *
 */
IEC_UINT domOpenComm(SVisInfo *pVI, IEC_CHAR const *szAddress, IEC_UINT uPort)
{
	IEC_UDINT ulTimeOut = VIS_TCP_TIMEOUT;
	IEC_UINT  uLocPort;
	IEC_UINT  i;

	VMS_sockaddr_in Addr;

	if (g_hSock != VMS_INVALID_SOCKET)
	{
		domCloseComm(pVI);
	}

	TR_STATE("[TCP] Creating listening socket.\r\n");
	
	/* Create a socket for listening on the port for new connections
	 */
	g_hSock = osSocket(VMS_AF_INET, VMS_SOCK_STREAM, VMS_IPPROTO_TCP);
	if (g_hSock == VMS_INVALID_SOCKET)
	{
		TR_ERROR("[TCP] ERROR(%d): Create Listen socket failed.\r\n", VMS_ERRNO);
		RETURN(ERR_ERROR);
	}

	{
		IEC_BOOL bTrue = TRUE;

	  #ifdef DEBUG
		osSetSockOpt(g_hSock, VMS_SOL_SOCKET, VMS_SO_DEBUG, (char *)&bTrue, sizeof(bTrue));
	  #endif

		osSetSockOpt(g_hSock, VMS_SOL_SOCKET, VMS_SO_REUSEADDR , (char *)&bTrue, sizeof(bTrue));
	}

	osSetSockOpt(g_hSock, VMS_SOL_SOCKET, VMS_SO_RCVTIMEO, (const IEC_CHAR OS_LPTR *)&ulTimeOut, sizeof(ulTimeOut));
	osSetSockOpt(g_hSock, VMS_SOL_SOCKET, VMS_SO_SNDTIMEO, (const IEC_CHAR OS_LPTR *)&ulTimeOut, sizeof(ulTimeOut));

	/* Connect the socket to the target
	 */
	if (uPort != VIS_PORT_SCAN)
	{
		OS_MEMSET(&Addr, 0, sizeof(Addr));
		Addr.sin_family 	 = VMS_AF_INET;
		Addr.sin_port		 = htons(uPort);
		Addr.sin_addr.s_addr = osInet_Addr(szAddress);
		
		if (osConnect(g_hSock, (struct sockaddr *)(&Addr), sizeof(Addr)) != VMS_RET_OK)
		{
			TR_ERROR("[TCP] ERROR(%d): Connect failed.\r\n", VMS_ERRNO);

			osShutDown(g_hSock, VMS_SD_BOTH);
			osCloseSocket(g_hSock);
			
			g_hSock = VMS_INVALID_SOCKET;
			
			RETURN(ERR_ERROR);
		}

	}
	else
	{
		uLocPort = domGetPort(0);

		for (i = 0; uLocPort != 0; i++)
		{
			/* Iterate through possible port numbers
			 */
			uLocPort = domGetPort(i);

			OS_MEMSET(&Addr, 0, sizeof(Addr));
			Addr.sin_family 	 = VMS_AF_INET;
			Addr.sin_port		 = htons(uLocPort);
			Addr.sin_addr.s_addr = osInet_Addr(szAddress);
			
			TR_STATE1("[TCP] Try to connect to port %d...\r\n", uLocPort);			
			if (osConnect(g_hSock, (struct sockaddr *)(&Addr), sizeof(Addr)) == VMS_RET_OK)
			{
				TR_STATE1("[TCP] Connect to port %d done.\r\n", uLocPort);			
				break;
			}

			TR_STATE1("[TCP] Connect to port %d failed.\r\n", uLocPort);			
		}

		if (uLocPort == 0)
		{
			TR_ERROR("[TCP] ERROR(%d): Connect failed.\r\n", VMS_ERRNO);

			osShutDown(g_hSock, VMS_SD_BOTH);
			osCloseSocket(g_hSock);
			
			g_hSock = VMS_INVALID_SOCKET;
			
			RETURN(ERR_ERROR);
		}
	}
	
	osSleep(250);

	TR_STATE1("[TCP] Connected to port %d.\r\n", uLocPort);

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * domCloseComm
 *
 * Close a open TCP/IP connection to a FarosPLC Run Time System.
 *
 */
IEC_UINT domCloseComm(SVisInfo *pVI)
{
	if (g_hSock == VMS_INVALID_SOCKET)
	{
		RETURN(OK);
	}

	osShutDown(g_hSock, VMS_SD_BOTH);
	osCloseSocket(g_hSock);
	
	g_hSock = VMS_INVALID_SOCKET;

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * sockRecv - domSockRecv
 *
 * Receives a data block from an attached FarosPLC Run Time System.
 *
 */
#if defined(RTS_CFG_TCP_NATIVE)

IEC_UINT domSockRecv(SVisInfo *pVI, IEC_DATA *pData, IEC_UINT *upLen)
{
	IEC_UINT  uDataLen	   = 0;
	IEC_UDINT ulTransfered = 0;

	if (g_hSock == VMS_INVALID_SOCKET)
	{
		RETURN(ERR_NOT_CONNECTED);
	}

	for(;;)
	{
		ulTransfered = osRecv(g_hSock, (IEC_CHAR OS_LPTR *)(pData + uDataLen), *upLen - uDataLen, 0);

		if (ulTransfered == VMS_SOCKET_ERROR)
		{
			if (VMS_ERRNO != VMS_ETIMEDOUT)
			{
				/* Any error except a timeout signals a bad connection.
				 */
				TR_ERROR("[TCP] ERROR(%d): Receive failed.\r\n", VMS_ERRNO);
				RETURN(ERR_ERROR);
			}

			/* Not an own thread within VisuComm library so don't retry!
			 */
			*upLen = 0;
			RETURN(WRN_TIME_OUT);
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
		if (uDataLen >= HD_BLOCK && uDataLen == domSwap16(pVI, ((XBlock *)pData)->uLen) 	+ HD_BLOCK)
		{
	  #endif

	  #if defined(RTS_CFG_PROT_BLOCK)
		if (uDataLen >= HD_FRAME && uDataLen == domSwap16(pVI, ((XFrame *)pData)->BLK.uLen) + HD_FRAME)
		{
	  #endif
			*upLen = uDataLen;
			RETURN(OK);
		}

	} /* for(;;) */

	RETURN(ERR_ERROR);
}

#endif /* RTS_CFG_TCP_NATIVE */


/* ---------------------------------------------------------------------------- */
/**
 * sockSend - domSockSend
 *
 * Sends a data block to an attached FarosPLC Run Time System.
 *
 */
#if defined(RTS_CFG_TCP_NATIVE)

IEC_UINT domSockSend(SVisInfo *pVI, IEC_DATA *pData, IEC_UINT uLen)
{
	IEC_UINT  uDataLen	   = 0;
	IEC_UDINT ulTransfered = 0;

	if (g_hSock == VMS_INVALID_SOCKET)
	{
		RETURN(ERR_NOT_CONNECTED);
	}

	for(;;)
	{
		ulTransfered = osSend(g_hSock, (const IEC_CHAR OS_LPTR *)(pData + uDataLen), uLen - uDataLen, 0);
		
		if (ulTransfered == VMS_SOCKET_ERROR)
		{
			if (VMS_ERRNO != VMS_ETIMEDOUT)
			{
				/* Any error except a timeout signals a bad connection
				 */
				TR_ERROR("[TCP] ERROR(%d): Send failed.\r\n", VMS_ERRNO);
				RETURN(ERR_ERROR);
			}

			/* Not an own thread within VisuComm library so don't retry!
			 */
			RETURN(WRN_TIME_OUT);
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
 * domTransferData
 *
 * Sends a single command to the RTS. The data (send & receive) must fit into
 * on Client/Server communication frame. 
 *
 */
IEC_UINT domTransferData(SVisInfo *pVI, IEC_BYTE byCmd, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease)
{
	IEC_UINT uRes;
	IEC_UINT uLen;
	
	XBlock	 *pBlock	= (XBlock *)pVI->pCmdBuff;
	
	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	uLen	= *upLen;
	*upLen	= 0;

	if (uLen > pVI->ulMaxData)
	{
		if (bRelease == TRUE)
		{
			osFree(ppData);
		}
		*ppData = NULL;

		RETURN(ERR_INVALID_DATA_SIZE);
	}

	/* Prepare data block
	 */
	pBlock->CMD.byCommand	= byCmd;

	pBlock->uLen			= domSwap16(pVI, uLen);
	pBlock->uBlock			= domSwap16(pVI, 1);
	pBlock->byLast			= 1;

	if (uLen != 0 && *ppData != NULL)
	{
		OS_MEMCPY(pBlock->CMD.pData, *ppData, uLen);

		if (bRelease == TRUE)
		{
			osFree(ppData);
		}
		*ppData = NULL;
	}

	uRes = domSockSend(pVI, pVI->pCmdBuff, (IEC_UINT)(HD_BLOCK + uLen));
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uLen = (IEC_UINT)(HD_BLOCK + pVI->ulMaxData);

	uRes = domSockRecv(pVI, pVI->pCmdBuff, &uLen);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (uLen < HD_BLOCK)
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	/* Check command - a negative command signals an error from the RTS.
	 */
	if (byCmd != (pBlock->CMD.byCommand & 0x7fu))
	{
		RETURN(ERR_INVALID_COMMAND);
	}

	if ((pBlock->CMD.byCommand & 0x80u) != 0)
	{
		uRes = domSwap16(pVI, *(IEC_UINT *)pBlock->CMD.pData);
		
		RETURN(uRes);
	}

	uLen = domSwap16(pVI, pBlock->uLen);

	/* Copy message data
	 */
	if (uLen != 0)
	{
		*ppData = (IEC_DATA *)osMalloc(uLen);
		if (*ppData == NULL)
		{
			RETURN(ERR_OUT_OF_MEMORY);
		}

		OS_MEMCPY(*ppData, pBlock->CMD.pData, uLen);
		*upLen = uLen;
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
