/*
 * Copyright 2021 Mect s.r.l
 *
 * HmiPlc: MectSuite udp communication for crosstable variables between hmi and plc
 *
 * This file is part of MectSuite.
 *
 * MectSuite is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * MectSuite is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * MectSuite. If not, see http://www.gnu.org/licenses/.
 *
 */

#include "hmi_plc.h"

#include <stdlib.h>     // calloc
#include <sys/types.h>  // bsd socket
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in
#include <strings.h>    // bzero
#include <string.h>     // strdup
#include <netdb.h>      // gethostbyname
#include <unistd.h>     // close
#include <stdio.h>      // fprintf

/* --------------------------------------------------------------------------*/

#define DEFAULT_HOSTNAME "127.0.0.1"
#define PLC_UDP_PORT     34901

/* --------------------------------------------------------------------------*/

void resetHmiPlcBlocks(HmiPlcBlock *hmiBlock, HmiPlcBlock *plcBlock)
{
    if (hmiBlock) {
        bzero(hmiBlock, sizeof(HmiPlcBlock));
        hmiBlock->bytes = sizeof(HmiPlcBlock);
        memset(&hmiBlock->states, varStatus_NOP, sizeof(hmiBlock->states));
        // NB: default is (value=0, first=0, last=0)
    }
    if (plcBlock) {
        bzero(plcBlock, sizeof(HmiPlcBlock));
        plcBlock->bytes = sizeof(HmiPlcBlock);
        // NB: default is (value=0, status=varStatus_DATA_OK)
    }
}

void clearHmiBlock(HmiPlcBlock *hmiBlock)
{
    if (hmiBlock) {
        // reset optimization after successfull hmiClientPoll()
        hmiBlock->first = 0;
        hmiBlock->last = 0;
    }
}

void changeStatusHmiBlock(HmiPlcBlock *hmiBlock, unsigned addr, enum varStatus status)
{
    if (hmiBlock && addr >= 1 && addr <= DimCrossTable) {

        // change status
        hmiBlock->states[addr] = status;

        // update optimization limits
        if (status != varStatus_NOP && status != varStatus_PREPARING) {
            if (hmiBlock->first == 0 || addr < hmiBlock->first) {
                hmiBlock->first = addr;
            }
            if (hmiBlock->last == 0 || addr > hmiBlock->last) {
                hmiBlock->last = addr;
            }
        }
    }
}

/* --------------------------------------------------------------------------*/

HmiClient *newHmiClient(const char *hostname)
{
    HmiClient *client = calloc(1, sizeof(HmiClient));

    if (client) {
        struct hostent *host = NULL;

        client->udpSocket = socket(AF_INET, SOCK_DGRAM, 0); // NB: no SOCK_NONBLOCK
        if (client->udpSocket < 0) {
            fprintf(stderr, "%s() socket failure\n", __func__);
            goto exit_failure;
        }
        if (! hostname ) {
            hostname = DEFAULT_HOSTNAME;
            // fprintf(stderr, "%s() using default plc hostname '%s'\n", __func__, hostname);
        }
        client->hostname = strdup(hostname);
        if (! client->hostname ) {
            fprintf(stderr, "%s() strdup failure\n", __func__);
            goto exit_failure;
        }
        host = gethostbyname(client->hostname);
        if (! host) {
            fprintf(stderr, "%s() gethostbyname failure\n", __func__);
            goto exit_failure;
        }
        client->plcAddress.sin_family = host->h_addrtype;
        memcpy((char *)&client->plcAddress.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
        client->plcAddress.sin_port = htons(PLC_UDP_PORT);
    }
    return client;

exit_failure:
    deleteHmiClient(client);
    return NULL;
}

void deleteHmiClient(HmiClient *client)
{
    if (client) {
        if (client->udpSocket >= 0) {
            close(client->udpSocket);
        }
        if (client->hostname) {
            free(client->hostname);
        }
        free(client);
    }
}

int hmiClientPoll(HmiClient *client, const HmiPlcBlock *hmiBlock, HmiPlcBlock *plcBlock, unsigned timeout_ms)
{
    int retval = 0;

    if (client && hmiBlock && plcBlock) {
        ssize_t bytes;

        // 1. send block to plc
        bytes = sendto(client->udpSocket, hmiBlock, sizeof(HmiPlcBlock),
                       0, (struct sockaddr *)&client->plcAddress, sizeof(client->plcAddress));
        if (bytes != sizeof(HmiPlcBlock)) {
            fprintf(stderr, "%s() sendto failure bytes=%d (%d)\n", __func__, bytes, sizeof(HmiPlcBlock));
            retval = -1; // send error
        } else {
            fd_set recv_set;
            struct timeval tv;
            int e;

            // 2. wait
            FD_ZERO(&recv_set);
            FD_SET(client->udpSocket, &recv_set);
            tv.tv_sec = timeout_ms / 1000;
            tv.tv_usec = (timeout_ms % 1000) * 1000;

            e = select(client->udpSocket + 1, &recv_set, NULL, NULL, &tv);
            if (e == 0) {
                retval = -1; // timeout: no reply from plc :(
            } else if (e < 0) {
                fprintf(stderr, "%s() select failure e=%d\n", __func__, e);
                retval = -1; // error
            } else {
                struct sockaddr_in udpAddress;
                socklen_t len = sizeof(udpAddress);

                // 3. receive block from plc
                bzero(&udpAddress, sizeof(udpAddress));
                bytes = recvfrom(client->udpSocket, plcBlock, sizeof(HmiPlcBlock),
                                 0, (struct sockaddr *)&udpAddress, &len);
                if (bytes == sizeof(HmiPlcBlock) && bytes == plcBlock->bytes
                    && udpAddress.sin_port == htons(PLC_UDP_PORT)
                    && udpAddress.sin_addr.s_addr == client->plcAddress.sin_addr.s_addr) {
                    retval = 1; // send and recv ok
                } else {
                    fprintf(stderr, "%s() recvfrom failure bytes=%d (%d)\n", __func__, bytes, sizeof(HmiPlcBlock));
                    retval = -1; // recv error
                }
            }
        }
    }
    return retval;
}

/* --------------------------------------------------------------------------*/

PlcServer *newPlcServer()
{
    PlcServer *server = calloc(1, sizeof(PlcServer));

    if (server) {
        struct sockaddr_in selfAddress;

        server->udpSocket = socket(AF_INET, SOCK_DGRAM, 0); // NB: no SOCK_NONBLOCK
        if (server->udpSocket < 0) {
            fprintf(stderr, "%s() socket failure\n", __func__);
            goto exit_failure;
        }
        bzero(&selfAddress, sizeof(selfAddress));
        selfAddress.sin_family = AF_INET;
        selfAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        selfAddress.sin_port = htons((u_short)PLC_UDP_PORT);
        if (bind(server->udpSocket, (struct sockaddr *)&selfAddress, sizeof(selfAddress)) < 0) {
            fprintf(stderr, "%s() bind failure\n", __func__);
            goto exit_failure;
        }
    }
    fprintf(stderr, "%s() success\n", __func__);
    return server;

exit_failure:
    deletePlcServer(server);
    fprintf(stderr, "%s() failure\n", __func__);
    return NULL;
}

void deletePlcServer(PlcServer *server)
{
    if (server) {
        if (server->udpSocket >= 0) {
            close(server->udpSocket);
        }
        free(server);
    }
}

int plcServerWait(PlcServer *server, HmiPlcBlock *hmiBlock, unsigned timeout_ms)
{
    int retval = 0;

    if (server && hmiBlock) {
        fd_set recv_set;
        struct timeval tv;
        int e;

        // 1. wait
        FD_ZERO(&recv_set);
        FD_SET(server->udpSocket, &recv_set);
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;

        e = select(server->udpSocket + 1, &recv_set, NULL, NULL, &tv);
        if (e == 0) {
            retval = 0; // timeout: maybe there is still no hmi
        } else if (e < 0) {
            fprintf(stderr, "%s() select failure e=%d\n", __func__, e);
            retval = -1; // error
        } else {
            ssize_t bytes = 0;

            // 2. receive block
            bzero(&server->hmiAddress, sizeof(server->hmiAddress));
            server->hmiAddressLen = sizeof(server->hmiAddress);
            bytes = recvfrom(server->udpSocket, hmiBlock, sizeof(HmiPlcBlock),
                     0, (struct sockaddr *)&server->hmiAddress, &server->hmiAddressLen);

            if (bytes == sizeof(HmiPlcBlock) && bytes == hmiBlock->bytes) {
                // fprintf(stderr, "%s() recvfrom success bytes=%d (%d) (%d) 0x%08x:%d %d\n", __func__,
                //         bytes, sizeof(HmiPlcBlock), hmiBlock->bytes,
                //         server->hmiAddress.sin_addr.s_addr, server->hmiAddress.sin_port, server->hmiAddressLen);
                retval = 1; // recv ok
            } else {
                fprintf(stderr, "%s() recvfrom failure bytes=%d (%d)\n", __func__, bytes, sizeof(HmiPlcBlock));
                retval = -1; // recv error
            }
        }
    }
    return retval;
}

int plcServerReply(PlcServer *server, const HmiPlcBlock *plcBlock)
{
    int retval = 0;

    if (server && plcBlock) {
        ssize_t bytes;

        // 1. send block
        bytes = sendto(server->udpSocket, plcBlock, sizeof(HmiPlcBlock),
                       0, (struct sockaddr *)&server->hmiAddress, server->hmiAddressLen);
        if (bytes == sizeof(HmiPlcBlock)) {
            // fprintf(stderr, "%s() sendto success bytes=%d (%d) 0x%08x:%d %d\n", __func__,
            //         bytes, sizeof(HmiPlcBlock),
            //         server->hmiAddress.sin_addr.s_addr, server->hmiAddress.sin_port, server->hmiAddressLen);
            retval = 1; // send ok
        } else {
            fprintf(stderr, "%s() sendto failure bytes=%d (%d)\n", __func__, bytes, sizeof(HmiPlcBlock));
            retval = -1; // send error
        }
    }
    return retval;
}

/* --------------------------------------------------------------------------*/
