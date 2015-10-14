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
 * Filename: mbtcpsImpl.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"mbtcpsImpl.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "stdInc.h"

#if defined(RTS_CFG_IOMBTCPS)

#include "mbtcpsMain.h"
#include "iolDef.h"

#include "libModbus.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define REGISTERS_NUMBER	4096 // MODBUS_MAX_READ_REGISTERS // 125
#define REGISTERS_TYPE      uint16_t
#define MIN_CHANNEL_SIZE    (REGISTERS_NUMBER * sizeof(REGISTERS_TYPE)) // 0x00002000 8kB

#define THE_DELAY_ms		1000
#define THE_TCP_LISTEN_PORT 502
#define THE_TCP_BIND_ADDR   "127.0.0.1" // useless since modbus_tcp_listen() forces INADDR_ANY
#define THE_TCP_MAX_CLIENTS	10

//#define RTS_CFG_DEBUG_OUTPUT

/* ----  Global Variables:	 -------------------------------------------------- */

static enum { NOT_STARTED, RUNNING, EXITING } g_bThreadStatus = NOT_STARTED;
static IEC_BOOL g_bInitialized	= FALSE;
static IEC_BOOL g_bConfigured	= FALSE;
static IEC_BOOL g_bRunning	= FALSE;
static IEC_BOOL g_bExiting	= FALSE;

static pthread_t theThread_id = -1;
static pthread_mutex_t theMutex = PTHREAD_MUTEX_INITIALIZER;
static modbus_mapping_t *mb_mapping = NULL;

/* ----  Local Functions:	--------------------------------------------------- */

void *mbtcpsThread(void *dontcare)
{
    static modbus_t *ctx;
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
	int master_socket;
	int rc;
	fd_set refset;
	fd_set rdset;
	int fdmax;
    int server_socket = -1;
    int threadInitOK = FALSE;

    // thread init
    ctx = modbus_new_tcp(THE_TCP_BIND_ADDR, THE_TCP_LISTEN_PORT);
    if (ctx != NULL) {
        threadInitOK = TRUE;
    }

    // run
    g_bThreadStatus = RUNNING;
	for (;;) {
        if (g_bRunning && threadInitOK) {
            // bind and listen
            if (server_socket == -1) {
                server_socket = modbus_tcp_listen(ctx, THE_TCP_MAX_CLIENTS);
                if (server_socket >= 0) {
                    FD_ZERO(&refset);
                    FD_SET(server_socket, &refset);
                    fdmax = server_socket;
                } else {
                    usleep(THE_DELAY_ms * 1000);
                    continue;
                }
            }
            // wait on server socket, only until timeout
            struct timeval tv;
            rdset = refset;
            tv.tv_sec = THE_DELAY_ms / 1000;
            tv.tv_usec = THE_DELAY_ms % 1000;
            if (select(fdmax+1, &rdset, NULL, NULL, &tv) <= 0) {
                // timeout or error
				continue;
			}
			for (master_socket = 0; master_socket <= fdmax; ++master_socket) {
				if (!FD_ISSET(master_socket, &rdset)) {
					continue;
				}
				if (master_socket == server_socket) {
					socklen_t addrlen;
					struct sockaddr_in clientaddr;
					int newfd;

					addrlen = sizeof(clientaddr);
					memset(&clientaddr, 0, sizeof(clientaddr));
					newfd = accept(server_socket, (struct sockaddr *)&clientaddr, &addrlen);
					if (newfd == -1) {
					    perror("Server accept() error");
					} else {
					    FD_SET(newfd, &refset);

					    if (newfd > fdmax) {
						fdmax = newfd;
                        }
					    printf("New connection from %s:%d on socket %d\n",
						   inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
					}
				} else {
					modbus_set_socket(ctx, master_socket);
					rc = modbus_receive(ctx, query);
					if (rc > 0) {
                        pthread_mutex_lock(&theMutex);
						{
                            modbus_reply(ctx, query, rc, mb_mapping);
						}
                        pthread_mutex_unlock(&theMutex);
					} else if (rc == -1) {
					    printf("Connection closed on socket %d\n", master_socket);
					    close(master_socket);

					    FD_CLR(master_socket, &refset);
					    if (master_socket == fdmax) {
                            --fdmax;
					    }
					}
				}
			}
		} else if (g_bExiting) {
			break;	
        } else {
            usleep(THE_DELAY_ms * 1000);
        }
	}

    // thread clean
    if (server_socket != -1) {
        shutdown(server_socket, SHUT_RDWR);
        close(server_socket);
        server_socket = -1;
     }
    if (ctx != NULL) {
        modbus_free(ctx);
        ctx = NULL;
	}
	// exit
    g_bThreadStatus = EXITING;
    return dontcare;
}

/* ----  Implementations:	--------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/**
 * mbtcpsInitialize
 *
 */
IEC_UINT mbtcpsInitialize(IEC_UINT uIOLayer)
{
	IEC_UINT        uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running mbtcpsInitialize() ...\n");
#endif
#if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo(TASK_OFFS_IOL_MTS, osGetTaskID());
	TR_RET(uRes);
#endif

    mb_mapping = NULL;
    theThread_id = -1;
    g_bThreadStatus = NOT_STARTED;

    g_bInitialized	= TRUE;
	g_bConfigured	= FALSE;
	g_bRunning	= FALSE;
	g_bExiting	= FALSE;
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * mbtcpsFinalize
 *
 */
IEC_UINT mbtcpsFinalize(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT        uRes = OK;

#if defined(RTS_CFG_SYSLOAD)
	uRes = ldClearTaskInfo(TASK_OFFS_IOL_MTS);
	TR_RET(uRes);
#endif

	g_bInitialized	= FALSE;
	g_bConfigured	= FALSE;
	g_bRunning	= FALSE;
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * mbtcpsNotifyConfig
 *
 */
IEC_UINT mbtcpsNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running mbtcpsNotifyConfig() ...\n");
#endif
    if (pIO->I.ulSize < MIN_CHANNEL_SIZE
     || pIO->Q.ulSize < MIN_CHANNEL_SIZE
     || pIO->M.ulSize < MIN_CHANNEL_SIZE) {
        uRes = ERR_INVALID_PARAM;
    }
	g_bConfigured	= TRUE;
	g_bRunning	= FALSE;
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * mbtcpsNotifyStart
 *
 */
IEC_UINT mbtcpsNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running mbtcpsNotifyStart() ... \n");
#endif

    // iolayer init
    mb_mapping = modbus_mapping_new(0, 0, REGISTERS_NUMBER, 0);
    if (mb_mapping == NULL) {
        uRes = ERR_FB_INIT;
        goto exit_function;
    }

    // start the thread
    pthread_attr_t pattr;
    pthread_attr_init(&pattr);
#ifndef __XENO__
    if (pthread_create(&theThread_id, &pattr, &mbtcpsThread, NULL) == 0) {
#else
	if (osPthreadCreate(&theThread_id, &pattr, &mbtcpsThread, NULL, "mbtcps", 0) == 0) {
#endif
        do {
            usleep(1000);
        } while (g_bThreadStatus != RUNNING);
    } else {
#if defined(RTS_CFG_IO_TRACE)
        osTrace("[%s] ERROR message_th: %s.\n", __func__, strerror(errno));
#endif
        uRes = ERR_FB_INIT;
    }
    g_bRunning = TRUE;

exit_function:
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * mbtcpsNotifyStop
 *
 */
IEC_UINT mbtcpsNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_DEBUG_OUTPUT)
	fprintf(stderr,"running mbtcpsNotifyStop() ...\n");
#endif
    // stop the thread
    g_bRunning = FALSE;
    if (g_bThreadStatus == RUNNING) {
        g_bExiting	= TRUE;
        do {
            usleep(1000);
        } while (g_bThreadStatus != EXITING);
    }

    // io layer clean
    if (mb_mapping != NULL) {
        modbus_mapping_free(mb_mapping);
        mb_mapping = NULL;
    }
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * mbtcpsNotifySet
 *
 */
IEC_UINT mbtcpsNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;

    if (g_bRunning) {
        pthread_mutex_lock(&theMutex);
        {
			if (pNotify->uTask != 0xffffu) {
                // notify from a plc task

                // check the write copy regions
                SImageReg   *pIRs = (SImageReg *)pIO->C.pAdr;
                SImageReg   *pIR = &pIRs[pNotify->uTask];

                if (pIR->pSetQ[uIOLayer]) {
                    // write from __%Q__ segment only if changed (using the %W write flags)
                    int i;
                    void *pvQsegment = pIO->Q.pAdr + pIO->Q.ulOffs;
                    void *pvWsegment = pIO->W.pAdr + pIO->W.ulOffs;
                    REGISTERS_TYPE *values = (REGISTERS_TYPE *)pvQsegment;
                    REGISTERS_TYPE *flags = (REGISTERS_TYPE *)pvWsegment;
                    for (i = 0; i < REGISTERS_NUMBER; ++i) {
                        if (flags[i] != 0) {
                            flags[i] = 0;
                            mb_mapping->tab_registers[i] = values[i];
                         }
                    }
                }
            } else if (pNotify->usSegment != SEG_OUTPUT) {
                uRes = ERR_WRITE_TO_INPUT;
            } else if (pNotify->usBit != 0) {
                uRes = ERR_INVALID_PARAM;
            } else {
                // notify from others
                IEC_UDINT ulStart = vmm_max(pNotify->ulOffset, pIO->Q.ulOffs);
                IEC_UDINT ulStop = vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->Q.ulOffs + pIO->Q.ulSize);
                if (ulStart < ulStop) {
					void * source = pIO->Q.pAdr + ulStart;
                    void * dest = mb_mapping->tab_registers;
                    dest += ulStart - pIO->Q.ulOffs;
                    OS_MEMCPY(dest, source, ulStop - ulStart);
                }
            }
        }
        pthread_mutex_unlock(&theMutex);
    }
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * mbtcpsNotifyGet
 *
 */
IEC_UINT mbtcpsNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;

    if (g_bRunning) {
        pthread_mutex_lock(&theMutex);
        {
            if (pNotify->uTask != 0xffffu) {
                // notify from a plc task

                // check the read copy regions
                SImageReg   *pIRs = (SImageReg *)pIO->C.pAdr;
                SImageReg   *pIR = &pIRs[pNotify->uTask];
                void * source;
                void * dest;

                source = mb_mapping->tab_registers;
                if (pIR->pGetI[uIOLayer]) {
                    dest = pIO->I.pAdr + pIO->I.ulOffs;
                    OS_MEMCPY(dest, source, REGISTERS_NUMBER * sizeof(REGISTERS_TYPE));
                } else if (pIR->pGetQ[uIOLayer]) {
                    dest = pIO->Q.pAdr + pIO->Q.ulOffs;
                    OS_MEMCPY(dest, source, REGISTERS_NUMBER * sizeof(REGISTERS_TYPE));
                } else {
                    // nothing to do
                }
            } else if (pNotify->usBit != 0) {
                // no bit access
                uRes = ERR_INVALID_PARAM;
            } else if (pNotify->usSegment != SEG_INPUT && pNotify->usSegment != SEG_OUTPUT){
                uRes = ERR_INVALID_PARAM;
            } else {
                // notify from others
				IEC_UDINT ulStart;
				IEC_UDINT ulStop;
                void * source;
                void * dest;

                source = mb_mapping->tab_registers;
                if (pNotify->usSegment == SEG_INPUT) {
                    ulStart	= vmm_max(pNotify->ulOffset, pIO->I.ulOffs);
                    ulStop	= vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->I.ulOffs + pIO->I.ulSize);
                    source += (ulStart - pIO->I.ulOffs);
                    dest = pIO->I.pAdr + ulStart;
               } else  { // pNotify->usSegment == SEG_OUTPUT
                    ulStart	= vmm_max(pNotify->ulOffset, pIO->Q.ulOffs);
                    ulStop	= vmm_min(pNotify->ulOffset + pNotify->uLen, pIO->Q.ulOffs + pIO->Q.ulSize);
                    source += (ulStart - pIO->Q.ulOffs);
                    dest = pIO->Q.pAdr + ulStart;
                }
                if (ulStart < ulStop) {
                    OS_MEMCPY(dest, source, ulStop - ulStart);
                }
            }
        }
        pthread_mutex_unlock(&theMutex);
    }
	RETURN(uRes);
}

#endif /* RTS_CFG_IOMBTCPS */

/* ---------------------------------------------------------------------------- */
