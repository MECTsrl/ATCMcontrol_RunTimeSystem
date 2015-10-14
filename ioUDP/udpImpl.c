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
 * Filename: udpImpl.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"udpImpl.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "stdInc.h"

#if defined(RTS_CFG_IOUDP)

#include "udpMain.h"
#include "iolDef.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define	MAX_UDP_IO_SIZE	(1024 * 8)


//#define TEST_RICHIUSURA
#undef TEST_RICHIUSURA

#ifndef TEST_RICHIUSURA

#define	TRANSMITTER_PORT	34901
#define	RECEIVER_PORT		34900

#define REMOTE_ADDRESS "127.0.0.1"
//#define REMOTE_ADDRESS "192.168.0.214"

#else

#define	TRANSMITTER_PORT	34900
#define	RECEIVER_PORT		34900

#define REMOTE_ADDRESS "127.0.0.1"

#endif

#undef SEND_ONLY_DATA_CHANGED

/* ----  Global Variables:	 -------------------------------------------------- */

static IEC_BOOL g_bInitialized	= FALSE;
static IEC_BOOL g_bConfigured	= FALSE;
static IEC_BOOL g_bRunning	= FALSE;

static int iServerSocket = -1;     /* server (receive) socket */
static int iClientSocket = -1;     /* client (transmit) socket */
static struct  sockaddr_in DestinationAddress;
static int iUdpRxPort = RECEIVER_PORT;
static int iUdpTxPort = TRANSMITTER_PORT;

#ifdef SEND_ONLY_DATA_CHANGED
static char OldSendData[MAX_UDP_IO_SIZE+1];
#endif

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */

int udpInitParameters(int udp_rx_port, int udp_tx_port)
{
	IEC_UINT uRes = OK;
	iUdpRxPort = udp_rx_port;
	iUdpTxPort = udp_tx_port;
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * udpInitialize
 *
 */
IEC_UINT udpInitialize(IEC_UINT uIOLayer)
{
	IEC_UINT        uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	printf("running udpInitialize() ...\n");
#endif
#ifdef SEND_ONLY_DATA_CHANGED
	memset(OldSendData, 0, MAX_UDP_IO_SIZE);
#endif

#if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo(TASK_OFFS_IOL_UDP, osGetTaskID());
	TR_RET(uRes);
#endif

	/* Create the server (receive) socket */
	if (iServerSocket < 0) {
		struct  sockaddr_in ServerAddress; /* structure to hold server's address  */

		iServerSocket = socket(AF_INET, SOCK_DGRAM, 0);
		if (iServerSocket < 0) {
#if defined(RTS_CFG_DEBUG_OUTPUT)
			printf("[%s, socket]: %s. [%d]\n", __func__, strerror(errno), errno);
#endif
			uRes = ERR_FB_INIT;
			goto exit_function;
		}

		// transform the socket in a non-blocking socket
		if (fcntl(iServerSocket, F_SETFL, O_NONBLOCK) < 0) {
#if defined(RTS_CFG_DEBUG_OUTPUT)
			printf("[%s, fcntl]: %s. [%d]\n", __func__, strerror(errno), errno);
#endif
			uRes = ERR_FB_INIT;
			goto exit_function;
		}

		/* Bind a local address to the server socket */
		/* clear sockaddr structure */
		memset((char *)&ServerAddress,0,sizeof(ServerAddress));
		/* set family to Internet */
		ServerAddress.sin_family = AF_INET;
		/* set the local IP address */
		ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		/* set the port number */
		ServerAddress.sin_port = htons((u_short)iUdpRxPort);

		if (bind(iServerSocket, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress)) < 0) {
#if defined(RTS_CFG_DEBUG_OUTPUT)
			printf("[%s, bind]: %s. [%d]\n", __func__, strerror(errno), errno);
#endif
			uRes = ERR_FB_INIT;
			goto exit_function;
		}
#if defined(RTS_CFG_DEBUG_OUTPUT)
		printf("[%s]: bind done, waiting for data input on UDP port %d\n", __func__, iUdpRxPort);
#endif
	}

	/* Create the client (transmitter) UDP socket */
	if (iClientSocket < 0) {
		struct hostent *h;
		/* get server IP address (no check if input is IP address or DNS name */
		h = gethostbyname(REMOTE_ADDRESS);
		if(h == NULL) {
#if defined(RTS_CFG_DEBUG_OUTPUT)
			printf("[%s]: unknown host '%s' \n", __func__, REMOTE_ADDRESS);
#endif
			uRes = ERR_FB_INIT;
			goto exit_function;
		}
#if defined(RTS_CFG_DEBUG_OUTPUT)
		printf("[%s]: sending data to '%s' (IP : %s) \n", __func__,
				h->h_name,
				inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));
#endif
		/* Construct the server sockaddr_in structure */
		/* clear sockaddr structure   */
		memset(&DestinationAddress, 0, sizeof(DestinationAddress));
		/* set family to Internet     */
		DestinationAddress.sin_family = h->h_addrtype;
		/* set the local IP address   */
		memcpy((char *) &DestinationAddress.sin_addr.s_addr, 
				h->h_addr_list[0], h->h_length);
		/* server port */
		DestinationAddress.sin_port = htons(iUdpTxPort);

		/* Create the client (transmitter) UDP socket */
		if ((iClientSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
#if defined(RTS_CFG_DEBUG_OUTPUT)
			printf("client socket creation failed\n");
#endif
			uRes = ERR_FB_INIT;
			goto exit_function;
		}

#if defined(RTS_CFG_DEBUG_OUTPUT)
		printf("client socket creation done\n");
#endif
#if 0
		/* bind any port */
		cliAddr.sin_family = AF_INET;
		cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		cliAddr.sin_port = htons(0);

		if(bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr))<0) {
			printf("cannot bind port\n" );
			uRes = ERR_FB_INIT;
			goto exit_function;
		}
#endif
	}

exit_function:
	g_bInitialized	= (IEC_BOOL)(uRes == OK);
	g_bConfigured	= FALSE;
	g_bRunning	= FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * udpFinalize
 *
 */
IEC_UINT udpFinalize(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT        uRes = OK;

#if defined(RTS_CFG_SYSLOAD)
	uRes = ldClearTaskInfo(TASK_OFFS_IOL_UDP);
	TR_RET(uRes);
#endif

	close(iServerSocket);
	close(iClientSocket);

	iServerSocket = -1;
	iClientSocket = -1;

	// exit_function:
	g_bInitialized	= FALSE;
	g_bConfigured	= FALSE;
	g_bRunning		= FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * udpNotifyConfig
 *
 */
IEC_UINT udpNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	printf("running udpNotifyConfig() ...\n");
#endif

	g_bConfigured	= (IEC_BOOL)(uRes == OK);
	g_bRunning		= FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * udpNotifyStart
 *
 */
IEC_UINT udpNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	printf("running udpNotifyStart() ... \n");
#endif

	g_bRunning = (IEC_BOOL)(uRes == OK);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * udpNotifyStop
 *
 */
IEC_UINT udpNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	printf("running udpNotifyStop() ...\n");
#endif

	g_bRunning = FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * udpNotifySet
 *
 */
IEC_UINT udpNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;
	unsigned int usSendSize = MAX_UDP_IO_SIZE;
	void           *pvSendData = pIO->Q.pAdr + pIO->Q.ulOffs;
	int ret;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	{
	int i;
	printf("[%s]: ", __func__);
	for (i = 0; i < MAX_UDP_IO_SIZE; i++)
	{
		printf("%X.", *(unsigned char *)(pvSendData + i));
	}
	printf("\n");
	}
#endif
#if 0
	if (pNotify->uTask != 0xffffu)
	{

	} /* if (pNotify->uTask != 0xffffu) */

	else
	{

	} /* else (pNotify->uTask != 0xffffu) */
#endif
#ifdef SEND_ONLY_DATA_CHANGED
	if (memcmp(OldSendData, pvSendData, MAX_UDP_IO_SIZE) != 0) {
		// il pacchetto ricevuto viene trasmesso
		// solo se il contenuto è cambiato
		memcpy(OldSendData, pvSendData, MAX_UDP_IO_SIZE);
#endif
		ret = sendto(iClientSocket, pvSendData, usSendSize, 0,
				(struct sockaddr *) &DestinationAddress,
				sizeof(DestinationAddress));
		if ( ret != usSendSize) {
#if defined(RTS_CFG_DEBUG_OUTPUT)
			printf("sendto: mismatch in number of sent bytes %d/%d\n", ret, MAX_UDP_IO_SIZE);
#endif
		}
#if defined(RTS_CFG_DEBUG_OUTPUT)
		printf(" bytes %d sent\n", ret);
#endif
#ifdef SEND_ONLY_DATA_CHANGED
	}
#endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * udpNotifyGet
 *
 */
IEC_UINT udpNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;
	unsigned int usReceiveSize = MAX_UDP_IO_SIZE;
	void           *pvReceiveData = pIO->I.pAdr + pIO->I.ulOffs;
	int            iByteNum;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	printf("[%s]\n", __func__);
#endif
#if 0 
	if (pNotify->uTask != 0xffffu)
	{
		/* A IEC task is going to read from the input and/or output segment.
		 * --------------------------------------------------------------------
		 */
	} /* if (pNotify->uTask != 0xffffu) */

	else
	{
		/* An external application (r.e. the 4C Watch) is going to read from
		 * the input and/or output segment.
		 * --------------------------------------------------------------------
		 */

	} /* else (pNotify->uTask != 0xffffu) */
#endif
	iByteNum = recv(iServerSocket, pvReceiveData, usReceiveSize, 0);
	if (iByteNum < 0) {
		if (errno == EAGAIN) {
			// no message available
			// printf("recv: no message available\n", );
		} else {
#if defined(RTS_CFG_DEBUG_OUTPUT)
			printf("recv ERROR errno %d\n", errno);
#endif
		}
	} else {
		// printf("recv: %d bytes received\n", iByteNum);
	}

#if defined(RTS_CFG_DEBUG_OUTPUT)
	{
	int i;
	printf("[%s] ", __func__);
	for (i = 0; i < iByteNum; i++)
	{
		printf(".%X.", *(unsigned char *)(pvReceiveData + i));
	}
	printf("\n");
	}
#endif
	RETURN(uRes);
}

#else

#endif /* RTS_CFG_IOUDP */

/* ---------------------------------------------------------------------------- */
