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
 * Filename: dataServers.c
 */

#include "dataImpl.h"

#define __4CFILE__	"dataServers.c"

/* ---------------------------------------------------------------------------- */

static inline void changeServerStatus(u_int32_t s, enum ServerStatus status);

static void startServerTiming(u_int32_t s);
static inline void updateServerTiming(u_int32_t s);
static inline void updateServerIdleTime(u_int32_t s);
static inline void updateServerBusyTime(u_int32_t s);

/* ---------------------------------------------------------------------------- */

void *serverThread(void *arg)
{
    struct ServerStruct *theServer = (struct ServerStruct *)arg;
    u_int32_t s = (theServer - &theServer[0]) / sizeof(struct ServerStruct);
    u_int8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int master_socket;
    int rc;
    fd_set refset;
    fd_set rdset;
    int fdmax = 0;
    int server_socket = -1;
    int threadInitOK = FALSE;
    struct timeval timeout_tv;

    // thread init
    // XX_GPIO_SET(4);
    osPthreadSetSched(FC_SCHED_IO_DAT, FC_PRIO_IO_DAT); // serverThread
    pthread_mutex_init(&theServer->mutex, NULL);
    timeout_tv.tv_sec = theServer->timeout_ms  / 1000;
    timeout_tv.tv_usec = (theServer->timeout_ms % 1000) * 1000;
    switch (theServer->protocol) {
    case RTU_SRV: {
        char device[VMM_MAX_PATH];

        snprintf(device, VMM_MAX_PATH, SERIAL_DEVNAME, theServer->u.serial.port);
        theServer->ctx = modbus_new_rtu(device, theServer->u.serial.baudrate,
                            theServer->u.serial.parity, theServer->u.serial.databits, theServer->u.serial.stopbits);
        theServer->mb_mapping = modbus_mapping_new(0, 0, REG_SRV_NUMBER, 0);
    }   break;
    case TCP_SRV: {
        char buffer[MAX_IPADDR_LEN];

        theServer->ctx = modbus_new_tcp(ipaddr2str(theServer->u.tcp_ip.IPaddr, buffer), theServer->u.tcp_ip.port);
        theServer->mb_mapping = modbus_mapping_new(0, 0, REG_SRV_NUMBER, 0);
    }   break;
    case TCPRTU_SRV: {
        char buffer[MAX_IPADDR_LEN];

        theServer->ctx = modbus_new_tcprtu(ipaddr2str(theServer->u.tcp_ip.IPaddr, buffer), theServer->u.tcp_ip.port);
        theServer->mb_mapping = modbus_mapping_new(0, 0, REG_SRV_NUMBER, 0);
    }   break;
    default:
        ;
    }
    if (theServer->ctx != NULL && theServer->mb_mapping != NULL
     && modbus_set_slave(theServer->ctx, theServer->NodeId) == 0) {
        threadInitOK = TRUE;
    }

    // run
    theServer->thread_status = RUNNING;
    if (!threadInitOK)
        changeServerStatus(s, SRV_RUNNING1);
    else
        changeServerStatus(s, SRV_RUNNING2);
    startServerTiming(s);

    while (engineStatus != enExiting) {

        // XX_GPIO_SET(4);
        updateServerTiming(s);

        // trivial scenario
        if (engineStatus != enRunning || !threadInitOK) {
            // XX_GPIO_CLR(4);
            osSleep(THE_CONNECTION_DELAY_ms);
            continue;
        }

        // get file descriptor or bind and listen
        if (server_socket == -1) {
            switch (theServer->protocol) {
            case RTU_SRV:
                if (modbus_connect(theServer->ctx)) {
                    server_socket = -1;
                } else {
                    server_socket = modbus_get_socket(theServer->ctx); // here socket is file descriptor
                    modbus_set_response_timeout(theServer->ctx, &timeout_tv);
                }
                break;
            case TCP_SRV:
                server_socket = modbus_tcp_listen(theServer->ctx, THE_SRV_MAX_CLIENTS);
                break;
            case TCPRTU_SRV:
                server_socket = modbus_tcprtu_listen(theServer->ctx, THE_SRV_MAX_CLIENTS);
                break;
            default:
                ;
            }
            if (server_socket >= 0) {
                FD_ZERO(&refset);
                FD_SET(server_socket, &refset);
                fdmax = server_socket;
                changeServerStatus(s, SRV_RUNNING3);
            }
        }
        if (server_socket < 0) {
            osSleep(THE_SERVER_DELAY_ms);
            continue;
        }

        // XX_GPIO_CLR(4);
        if (theServer->protocol == RTU_SRV) {
            // (single connection) force silence then read with timeout
            osSleep(theServer->silence_ms);
        } else {
            // (multiple connection) wait on server socket, only until timeout
            do {
                rdset = refset;
                timeout_tv.tv_sec = theServer->timeout_ms  / 1000;
                timeout_tv.tv_usec = (theServer->timeout_ms % 1000) * 1000;
                rc = select(fdmax+1, &rdset, NULL, NULL, &timeout_tv);
            } while ((rc == 0 || (rc < 0 && errno == EINTR)) && engineStatus != enExiting);
            // condition above for:
            // 1) avoid useless mode-switches (gatekeeper at high cpu%)
            // 2) optimize response time
            // 3) clean exiting

            if (rc == 0 || engineStatus == enExiting) { // timeout and/or exiting
                continue;
            } else if (rc < 0) { // error
                osSleep(THE_SERVER_DELAY_ms);
                continue;
            }
        }

        // XX_GPIO_SET(4);
        changeServerStatus(s, SRV_RUNNING4);
        updateServerIdleTime(s);

        // accept requests
#define _FC_READ_COILS                0x01
#define _FC_READ_DISCRETE_INPUTS      0x02
#define _FC_READ_HOLDING_REGISTERS    0x03
#define _FC_READ_INPUT_REGISTERS      0x04
#define _FC_WRITE_SINGLE_COIL         0x05
#define _FC_WRITE_SINGLE_REGISTER     0x06
#define _FC_READ_EXCEPTION_STATUS     0x07
#define _FC_WRITE_MULTIPLE_COILS      0x0F
#define _FC_WRITE_MULTIPLE_REGISTERS  0x10
#define _FC_REPORT_SLAVE_ID           0x11
#define _FC_MASK_WRITE_REGISTER       0x16
#define _FC_WRITE_AND_READ_REGISTERS  0x17
        switch (theServer->protocol) {
        case RTU_SRV:
            // unique client (serial line)
            rc = modbus_receive(theServer->ctx, query);
            if (rc > 0) {
                // XX_GPIO_CLR(4);
                osSleep(theServer->silence_ms);
                pthread_mutex_lock(&theServer->mutex);
                {
                    // XX_GPIO_SET(4);
                    switch (query[1]) {
                    case _FC_READ_COILS:
                    case _FC_READ_DISCRETE_INPUTS:
                    case _FC_READ_HOLDING_REGISTERS:
                    case _FC_READ_INPUT_REGISTERS:
                    case _FC_READ_EXCEPTION_STATUS:
                    case _FC_REPORT_SLAVE_ID:
                        incDiagnostic(theServer->diagnosticAddr, DIAGNOSTIC_READS);
                        break;
                    case _FC_WRITE_SINGLE_COIL:
                    case _FC_WRITE_SINGLE_REGISTER:
                    case _FC_WRITE_MULTIPLE_COILS:
                    case _FC_WRITE_MULTIPLE_REGISTERS:
                    case _FC_MASK_WRITE_REGISTER:
                        incDiagnostic(theServer->diagnosticAddr, DIAGNOSTIC_WRITES);
                        break;
                    case _FC_WRITE_AND_READ_REGISTERS:
                        incDiagnostic(theServer->diagnosticAddr, DIAGNOSTIC_WRITES);
                        incDiagnostic(theServer->diagnosticAddr, DIAGNOSTIC_READS);
                        break;
                    default:
                        break;
                    }
                    modbus_reply(theServer->ctx, query, rc, theServer->mb_mapping);
                    // XX_GPIO_CLR(4);
                }
                pthread_mutex_unlock(&theServer->mutex);
            }
            break;
        case TCP_SRV:
        case TCPRTU_SRV:
            // multiple clients (tcp/ip server)
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
#ifdef VERBOSE_DEBUG
                        perror("Server accept() error");
#endif
                    } else {
                        FD_SET(newfd, &refset);

                        if (newfd > fdmax) {
                            fdmax = newfd;
                        }
#ifdef VERBOSE_DEBUG
                        fprintf(stderr, "New connection from %s:%d on socket %d\n",
                            inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
#endif
                    }
                } else {
                    modbus_set_socket(theServer->ctx, master_socket);
                    rc = modbus_receive(theServer->ctx, query);
                    if (rc > 0 && theServer->mb_mapping != NULL) {
                        // XX_GPIO_CLR(4);
                        pthread_mutex_lock(&theServer->mutex);
                        {
                            // XX_GPIO_SET(4);
                            switch (query[(theServer->protocol == TCP_SRV) ? 7 : 1]) {
                            case _FC_READ_COILS:
                            case _FC_READ_DISCRETE_INPUTS:
                            case _FC_READ_HOLDING_REGISTERS:
                            case _FC_READ_INPUT_REGISTERS:
                            case _FC_READ_EXCEPTION_STATUS:
                            case _FC_REPORT_SLAVE_ID:
                                incDiagnostic(theServer->diagnosticAddr, DIAGNOSTIC_READS);
                                break;
                            case _FC_WRITE_SINGLE_COIL:
                            case _FC_WRITE_SINGLE_REGISTER:
                            case _FC_WRITE_MULTIPLE_COILS:
                            case _FC_WRITE_MULTIPLE_REGISTERS:
                            case _FC_MASK_WRITE_REGISTER:
                                incDiagnostic(theServer->diagnosticAddr, DIAGNOSTIC_WRITES);
                                break;
                            case _FC_WRITE_AND_READ_REGISTERS:
                                incDiagnostic(theServer->diagnosticAddr, DIAGNOSTIC_WRITES);
                                incDiagnostic(theServer->diagnosticAddr, DIAGNOSTIC_READS);
                                break;
                            default:
                                ;
                            }
                            modbus_reply(theServer->ctx, query, rc, theServer->mb_mapping);
                            // XX_GPIO_CLR(4);
                        }
                        pthread_mutex_unlock(&theServer->mutex);
                    } else if (rc == -1) {
                        fprintf(stderr, "Connection closed on socket %d\n", master_socket);
                        close(master_socket);

                        FD_CLR(master_socket, &refset);
                        if (master_socket == fdmax) {
                            --fdmax;
                        }
                    }
                }
            }
            break;
        default:
            ;
        }
        changeServerStatus(s, SRV_RUNNING3);
        updateServerBusyTime(s);
    }

    // thread clean
    switch (theServer->protocol) {
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        if (theServer->mb_mapping != NULL) {
            modbus_mapping_free(theServer->mb_mapping);
            theServer->mb_mapping = NULL;
        }
        if (server_socket != -1) {
            shutdown(server_socket, SHUT_RDWR);
            close(server_socket);
            server_socket = -1;
         }
        if (theServer->ctx != NULL) {
            modbus_close(theServer->ctx);
            modbus_free(theServer->ctx);
        }
        break;
    default:
        ;
    }

    // exit
    // XX_GPIO_CLR(4);
    fprintf(stderr, "EXITING: %s\n", theServer->name);
    theServer->thread_status = EXITING;
    return arg;
}

/* ---------------------------------------------------------------------------- */

static inline void changeServerStatus(u_int32_t s, enum ServerStatus status)
{
    theServers[s].status = status;
    setDiagnostic(theServers[s].diagnosticAddr, DIAGNOSTIC_STATUS, status);
#ifdef VERBOSE_DEBUG
    fprintf(stderr, "%s: status = %d\n", theServers[s].name, status);
#endif
}

/* ---------------------------------------------------------------------------- */

static void startServerTiming(u_int32_t s)
{
    RTIME now_ns = rt_timer_read();

    theServers[s].idle_time_ns = 0;
    theServers[s].busy_time_ns = 0;
    theServers[s].last_time_ns = now_ns;
    theServers[s].last_update_ns = now_ns;
}

static inline void updateServerTiming(u_int32_t s)
{
    RTIME now_ns = rt_timer_read();

    // update idle/busy statistics each 5 seconds
    if ((now_ns - theServers[s].last_update_ns) > 5000000000ULL) {
        RTIME delta_ns;
        unsigned bus_load;

        // add last idle time
        delta_ns = now_ns - theServers[s].last_time_ns;
        theServers[s].idle_time_ns += delta_ns;

        // update diagnostics: busy load percentage is: (100 * busy_time / total_time)
        if (theServers[s].busy_time_ns == 0) {
            bus_load = 0;
        } else {
            lldiv_t x;

            x = lldiv((100 * theServers[s].busy_time_ns), (theServers[s].busy_time_ns + theServers[s].idle_time_ns));
            bus_load = x.quot;
        }
        setDiagnostic(theServers[s].diagnosticAddr, DIAGNOSTIC_BUS_LOAD, bus_load);

        // reset
        theServers[s].idle_time_ns = 0;
        theServers[s].busy_time_ns = 0;
        theServers[s].last_time_ns = now_ns;
        theServers[s].last_update_ns = now_ns;
    }
}

static inline void updateServerIdleTime(u_int32_t s)
{
    RTIME now_ns = rt_timer_read();

    RTIME delta_ns = now_ns - theServers[s].last_time_ns;
    theServers[s].idle_time_ns += delta_ns;
    theServers[s].last_time_ns = now_ns;
}

static inline void updateServerBusyTime(u_int32_t s)
{
    RTIME now_ns = rt_timer_read();

    RTIME delta_ns = now_ns - theServers[s].last_time_ns;
    theServers[s].busy_time_ns += delta_ns;
    theServers[s].last_time_ns = now_ns;
}

/* ---------------------------------------------------------------------------- */
