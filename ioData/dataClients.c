/*
 * Copyright 2021 Mect s.r.l
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
 * Filename: dataClients.c
 */

#include "dataImpl.h"

#define __4CFILE__	"dataClients.c"

/* ---------------------------------------------------------------------------- */

static const char *deviceStatusName[] = {"ZERO", "NOT_CONNECTED", "CONNECTED", "CONNECTED_WITH_ERRORS", "DEVICE_BLACKLIST", "NO_HOPE" };
static const char *nodeStatusName[] = {"NO_NODE", "NODE_OK", "TIMEOUT", "BLACKLIST", "DISCONNECTED" };

static void fieldbusReset(u_int16_t d);
static enum fieldbusError fieldbusRead(u_int16_t d, u_int16_t DataAddr, u_int32_t DataValue[], u_int16_t DataNumber);
static enum fieldbusError fieldbusWrite(u_int16_t d, u_int16_t DataAddr, u_int32_t DataValue[], u_int16_t DataNumber);

static void doWriteDeviceRetentives(u_int32_t d);

static inline void changeDeviceStatus(u_int32_t d, enum DeviceStatus status);
static inline void changeNodeStatus(u_int32_t d, u_int16_t node, enum NodeStatus status);

static unsigned checkAddr(unsigned d, unsigned DataAddr, unsigned DataNumber);

static void startDeviceTiming(u_int32_t d);
static inline void updateDeviceTiming(u_int32_t d);
static inline void updateDeviceIdleTime(u_int32_t d);
static inline void updateDeviceBusyTime(u_int32_t d);

static u_int16_t modbusRegistersNumber(u_int16_t DataAddr, u_int16_t DataNumber);

static inline unsigned get_byte_bit(u_int8_t data, unsigned n);
static inline unsigned get_word_bit(u_int16_t data, unsigned n);
static inline unsigned get_dword_bit(u_int32_t data, unsigned n);
static inline void set_byte_bit(u_int8_t *data, unsigned n, unsigned value);
static inline void set_word_bit(u_int16_t *data, unsigned n, unsigned value);
static inline void set_dword_bit(u_int32_t *data, unsigned n, unsigned value);

/* ---------------------------------------------------------------------------- */

void *clientThread(void *arg)
{
    struct ClientStruct *theDevice = (struct ClientStruct *)arg;
    u_int32_t d = (theDevice - &theDevices[0]) / sizeof(struct ClientStruct);
    u_int16_t v;

    // device connection management
    struct timeval response_timeout;

    // device queue management by priority and periods
    u_int16_t prio;                         // priority of variables
    int we_have_variables[MAX_PRIORITY];    // do we have variables at that priority?
    u_int16_t read_addr[MAX_PRIORITY];      // current read address in crosstable for each priority
    RTIME read_time_ns[MAX_PRIORITY];   // next read time for each priority

    // data for each fieldbus operation
    u_int16_t readOperation; // read/write
    u_int16_t DataAddr = 0;  // variable address in the crosstable
    u_int32_t DataValue[MAX_VALUES]; // max 64 reads and 16 writes
    u_int32_t DataNumber;    // max 64 reads and 16 writes
    u_int16_t Data_node;     // global index of the node in theNodes[]
    enum fieldbusError error;


    // ------------------------------------------ thread init
    // XX_GPIO_SET_69(d);
    osPthreadSetSched(FC_SCHED_IO_DAT, FC_PRIO_IO_DAT); // clientThread
    theDevice->modbus_ctx = NULL;
    theDevice->mect_fd = -1;
    // "new"
    switch (theDevice->protocol) {
    case PLC: // FIXME: assert
        break;
    case RTU: {
        char device[VMM_MAX_PATH];

        snprintf(device, VMM_MAX_PATH, SERIAL_DEVNAME, theDevice->u.serial.port);
        theDevice->modbus_ctx = modbus_new_rtu(device, theDevice->u.serial.baudrate,
                            theDevice->u.serial.parity, theDevice->u.serial.databits, theDevice->u.serial.stopbits);
    }   break;
    case TCP: {
        char buffer[MAX_IPADDR_LEN];

        theDevice->modbus_ctx = modbus_new_tcp(ipaddr2str(theDevice->u.tcp_ip.IPaddr, buffer), theDevice->u.tcp_ip.port);
    }   break;
    case TCPRTU: {
        char buffer[MAX_IPADDR_LEN];

        theDevice->modbus_ctx = modbus_new_tcprtu(ipaddr2str(theDevice->u.tcp_ip.IPaddr, buffer), theDevice->u.tcp_ip.port);
    }   break;
    case CANOPEN: {
        u_int16_t addr;
        CANopenStart(theDevice->u.can.bus); // may start more threads
        // get the unspecified CANopen variables offsets ("CH0_NETRUN" ...)
        for (addr = 1; addr <= DimCrossTable; ++addr) {
            if (CrossTable[addr].Enable > 0 && CrossTable[addr].device == d) {
                if (CrossTable[addr].Offset == 0) {
                    CrossTable[addr].Offset = CANopenGetVarIndex(CrossTable[addr].Port, CrossTable[addr].Tag);
                }
#ifdef VERBOSE_DEBUG
                fprintf(stderr, "CANOPEN %u.%u '%s' %u\n", CrossTable[addr].Port, CrossTable[addr].NodeId, CrossTable[addr].Tag, CrossTable[addr].Offset);
#endif
            }
        }
        if (verbose_print_enabled) {
            CANopenList(theDevice->u.can.bus);
        }
    }   break;
    case MECT: // new (nothing to do)
        break;
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        break;
    default:
        ;
    }
    // check the "new" result
    switch (theDevice->protocol) {
    case PLC: // FIXME: assert
        break;
    case RTU:
    case TCP:
    case TCPRTU:
        if (theDevice->modbus_ctx == NULL) {
            changeDeviceStatus(d, NO_HOPE);
        } else {
            changeDeviceStatus(d, NOT_CONNECTED);
        }
        break;
    case CANOPEN: {
        u_int8_t channel = theDevice->u.can.bus;
        struct CANopenStatus status;
        CANopenGetChannelStatus(channel, &status);
        if (status.running || status.error != 0xffffffff) {
            changeDeviceStatus(d, NOT_CONNECTED);
        } else {
            changeDeviceStatus(d, NO_HOPE);
        }
    }   break;
    case MECT: // check state (nothing to do)
        changeDeviceStatus(d, NOT_CONNECTED);
        break;
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        changeDeviceStatus(d, NOT_CONNECTED);
        break;
    default:
        ;
    }

    // ------------------------------------------ run
    fprintf(stderr, "%s: ", theDevice->name);
    switch (theDevice->protocol) {
    case PLC: // FIXME: assert
        break;
    case RTU:
    case MECT:
        fprintf(stderr, "@%u/%u/%c/%u, ", theDevice->u.serial.baudrate, theDevice->u.serial.databits, theDevice->u.serial.parity, theDevice->u.serial.stopbits);
        break;
    case TCP:
    case TCPRTU:
        break;
    case CANOPEN:
        break;
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        break;
    default:
        ;
    }
    fprintf(stderr, "silence_ms=%u, timeout_ms=%u\n", theDevice->silence_ms, theDevice->timeout_ms);
    response_timeout.tv_sec = theDevice->timeout_ms / 1000;
    response_timeout.tv_usec = (theDevice->timeout_ms % 1000) * 1000;
    startDeviceTiming(d);
    for (prio = 0; prio < MAX_PRIORITY; ++prio) {
        read_time_ns[prio] = theDevice->current_time_ns;
        read_addr[prio] = 0;
        we_have_variables[prio] = 0;
    }
    // for each priority check if we have variables at that priority
    for (v = 0; v < theDevice->var_num; ++v) {
        DataAddr = theDevice->device_vars[v].addr;
        if (CrossTable[DataAddr].Enable > 0 && CrossTable[DataAddr].Enable <= MAX_PRIORITY) {
            prio = CrossTable[DataAddr].Enable - 1;
            we_have_variables[prio] = 1; // also Htype
        }
    }
    if (verbose_print_enabled) {
        fprintf(stderr, "%s: reading variables {", theDevice->name);
        for (prio = 0; prio < MAX_PRIORITY; ++prio) {
            if (we_have_variables[prio]) {
                fprintf(stderr, "\n\t%u:", prio + 1);
                for (v = 0; v < theDevice->var_num; ++v) {
                    if (CrossTable[theDevice->device_vars[v].addr].Enable == (prio + 1)) {
                        fprintf(stderr, " %u%s", theDevice->device_vars[v].addr, theDevice->device_vars[v].active ? "" : "(H)");
                    }
                }
            }
        }
        fprintf(stderr, "\n}\n");
    }

    // start the fieldbus operations loop
    DataAddr = 0;
    DataNumber = 0;
    readOperation = TRUE;
    error = NoError;

    // pre-charge the output retentives
    doWriteDeviceRetentives(d);

    // let the engine continue
    theDevice->thread_status = RUNNING;

    while (engineStatus != enExiting) {

        int do_fieldbusReset = FALSE;

        // XX_GPIO_SET_69(d);
        updateDeviceTiming(d);

        // trivial scenario
        if (engineStatus != enRunning || theDevice->status == NO_HOPE) {
            // XX_GPIO_CLR_69(d);
            osSleep(THE_CONNECTION_DELAY_ms);
            continue;
        }

        // was I already doing something?
        if (DataAddr == 0) {
            int rc;
            RTIME next_ns;

            // wait for next operation or next programmed read
            next_ns = theDevice->current_time_ns + THE_MAX_CLIENT_SLEEP_ns;
            for (prio = 0; prio < MAX_PRIORITY; ++prio) {
                if (we_have_variables[prio]) {
                    if (read_time_ns[prio] < next_ns) {
                        next_ns = read_time_ns[prio];
                    }
                }
            }
            if (next_ns > theDevice->current_time_ns) {
#ifdef VERBOSE_DEBUG
                int invalid_timeout, invalid_permission, other_error;
#endif
                struct timespec abstime;
                TIMESPEC_FROM_RTIME(abstime, next_ns);

                do {
                    int saved_errno;
                    // XX_GPIO_CLR_69(d);
                    rc = sem_timedwait(&newOperations[d], &abstime);
                    saved_errno = errno;
                    // XX_GPIO_SET_69(d);

#ifdef VERBOSE_DEBUG
                    timeout = invalid_timeout = invalid_permission = other_error = FALSE;
#endif
                    errno = saved_errno;
                    if (errno ==  EINVAL) {
                        fprintf(stderr, "%s@%09llu ms: problem with (%lds, %ldns).\n",
                            theDevice->name, theDevice->current_time_ns, abstime.tv_sec, abstime.tv_nsec);
#ifdef VERBOSE_DEBUG
                        invalid_timeout = TRUE;
#endif
                        break;
                    }
                    if (errno == EINTR) {
                        continue;
                    }
                    if (rc == 0) {
                        break;
                    }
                    if (errno ==  ETIMEDOUT) {
#ifdef VERBOSE_DEBUG
                        timeout = TRUE;
#endif
                        break;
                    }
                    if (errno ==  EPERM) {
#ifdef VERBOSE_DEBUG
                        invalid_permission = TRUE;
#endif
                        break;
                    }
#ifdef VERBOSE_DEBUG
                    other_error = TRUE;
#endif
                    break;
                } while (TRUE);

                // what time is it please?
                updateDeviceTiming(d);
#ifdef VERBOSE_DEBUG
                if (invalid_timeout || invalid_permission || other_error) {
                    fprintf(stderr, "%s@%09u ms: woke up because %s (%09u ms = %u s + %d ns)\n", theDevice->name, theDevice->current_time_ms,
                        timeout?"timeout":(invalid_timeout?"invalid_timeout":(invalid_permission?"invalid_permission":(other_error?"other_error":"signal"))),
                        next_ms, abstime.tv_sec, abstime.tv_nsec);
                }
#endif
            } else {
#ifdef VERBOSE_DEBUG
                fprintf(stderr, "%s@%09u ms: immediate restart\n", theDevice->name, theDevice->current_time_ms);
#endif
            }
        }

        // choose the read/write command either in the local queue or in the queue from HMI
        if (DataAddr == 0) {
            // XX_GPIO_CLR_69(d);
            pthread_mutex_lock(&theCrosstableClientMutex);
            {
                // XX_GPIO_SET_69(d);

                // is it there an immediate write requests from PLC to this device?
                if (theDevice->PLCwriteRequestNumber > 0) {
                    u_int16_t n;

                    DataAddr = theDevice->PLCwriteRequests[theDevice->PLCwriteRequestGet].Addr;
                    // data values from the local queue only, the *_BIT management is in fieldWrite()
                    DataNumber = theDevice->PLCwriteRequests[theDevice->PLCwriteRequestGet].Number;
                    readOperation = FALSE;
                    // check
                    DataAddr = checkAddr(d, DataAddr, DataNumber);
                    if (DataAddr > 0) {
                        for (n = 0; n < DataNumber; ++n) {
                            DataValue[n] = theDevice->PLCwriteRequests[theDevice->PLCwriteRequestGet].Values[n];
                        }
                    }

                    // write requests circular buffer (even if check failed)
                    theDevice->PLCwriteRequestGet = (theDevice->PLCwriteRequestGet + 1) % MaxLocalQueue;
                    theDevice->PLCwriteRequestNumber -= 1;

                    setDiagnostic(theDevice->diagnosticAddr, DIAGNOSTIC_WRITE_QUEUE, theDevice->PLCwriteRequestNumber);
#ifdef VERBOSE_DEBUG
                    fprintf(stderr, "%s@%09u ms: write PLC [%u], there are still %u\n", theDevice->name, theDevice->current_time_ms, DataAddr, theDevice->PLCwriteRequestNumber);
#endif
                }

                // if no write then is it there a read request for this device?
                if (DataAddr == 0) {
                    u_int16_t addr;

                    // periodic read requests:
                    // -- {P,S,F} automatically enabled from Crosstable
                    // -- {H} enabled from HMI when the page is displayed
                    for (prio = 0; prio < MAX_PRIORITY; ++prio) {
                        // avoid checking if we know it's pointless
                        if (! we_have_variables[prio]) {
                            continue;
                        }
                        // only when the timer expires
                        if (read_time_ns[prio] <= theDevice->current_time_ns) {

                            // is it there anything to read at this priority for this device?
                            int found = FALSE;

                            // {P,S,F} and active {H}
                            for (v = read_addr[prio]; v < theDevice->var_num; ++v) {
                                if (theDevice->device_vars[v].active) {
                                    addr = theDevice->device_vars[v].addr;
                                    if (CrossTable[addr].Enable == (prio + 1) && addr == CrossTable[addr].BlockBase) {
                                        if (theNodes[CrossTable[addr].node].status != NODE_DISABLED) {
                                            found = TRUE;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (found) {
                                DataAddr = addr; // CrossTable[addr].BlockBase;
                                DataNumber = CrossTable[addr].BlockSize;
                                if (DataNumber > MAX_VALUES) {
                                    fprintf(stderr, "clientThread(%d) (HMI) DataNumber = %u\n", d, DataNumber);
                                    DataNumber = MAX_VALUES;
                                }
                                readOperation = TRUE;
                                // data values will be available after the fieldbus access
                                // keep the index for the next loop
                                read_addr[prio] = v + DataNumber; // may overlap DimCrossTable, it's ok
#ifdef VERBOSE_DEBUG
                                fprintf(stderr, "%s@%09u ms: read %uPSF [%u] (was [%u]), will check [%u]\n", theDevice->name, theDevice->current_time_ms, prio+1, DataAddr, addr, read_addr[prio]);
#endif
                                break;
                            } else {
                                // compute next tic for this priority, restarting from the first
                                RTIME period_ns = system_ini.system.read_period_ms[prio] * 1E6;
                                read_time_ns[prio] += period_ns;
                                if (read_time_ns[prio] <= theDevice->current_time_ns) {
                                    RTIME n = theDevice->current_time_ns / period_ns;
                                    read_time_ns[prio] = (n + 1) * period_ns;
                                }
                                read_addr[prio] = 0;
#ifdef VERBOSE_DEBUG
                                fprintf(stderr, "%s@%09u ms: no read %uHPSF will restart at %09u ms\n", theDevice->name, theDevice->current_time_ms, prio+1, read_time_ms[prio]);
#endif
                            }
                        }
                    }
                }
                // XX_GPIO_CLR_69(d);
            }
            pthread_mutex_unlock(&theCrosstableClientMutex);
        }

        if (DataAddr == 0) {
            // nothing to do
            continue;
        }
        DataAddr = checkAddr(d, DataAddr, DataNumber);
        if (DataAddr == 0) {
            // skip
            continue;
        }

        // XX_GPIO_SET_69(d);
        // maybe either a retry or a new operation
        Data_node = CrossTable[DataAddr].node;

        // manage the device status (before operation)
        switch (theDevice->status) {
        case ZERO:
        case NO_HOPE: // FIXME: assert
            break;
        case NOT_CONNECTED:
            // try connection
            switch (theDevice->protocol) {
            case PLC: // FIXME: assert
                break;
            case RTU:
                fprintf(stderr, "%s modbus_connect()\n", theDevice->name);
                if (modbus_connect(theDevice->modbus_ctx) >= 0) {
                    modbus_set_response_timeout(theDevice->modbus_ctx, &response_timeout);
                    changeDeviceStatus(d, CONNECTED);
                }
                break;
            case TCP:
            case TCPRTU:
                if (modbus_connect(theDevice->modbus_ctx) >= 0) {
                    if (modbus_flush(theDevice->modbus_ctx) >= 0) {
                        modbus_set_response_timeout(theDevice->modbus_ctx, &response_timeout);
                        changeDeviceStatus(d, CONNECTED);
                    } else {
                        modbus_close(theDevice->modbus_ctx);
                    }
                }
                break;
            case CANOPEN: {
                u_int8_t channel = theDevice->u.can.bus;
                if (CANopenConfigured(channel)) {
                    changeDeviceStatus(d, CONNECTED);
                }
            }   break;
            case MECT: // connect()
                theDevice->mect_fd = mect_connect(
                    theDevice->u.serial.port,
                    theDevice->u.serial.baudrate,
                    theDevice->u.serial.parity,
                    theDevice->u.serial.databits,
                    theDevice->u.serial.stopbits,
                    theDevice->timeout_ms);
                if (theDevice->mect_fd >= 0) {
                    changeDeviceStatus(d, CONNECTED);
                }
                break;
            case RTU_SRV:
            case TCP_SRV:
            case TCPRTU_SRV:
                changeDeviceStatus(d, CONNECTED);
                break;
            default:
                ;
            }
            // if connection attempts fail too much, then rest a bit
            if (theDevice->status == NOT_CONNECTED && theDevice->elapsed_time_ns >= THE_DEVICE_SILENCE_ns) {
                changeDeviceStatus(d, DEVICE_BLACKLIST); // also data=0 and status=DATA_ERROR
            }
            break;
        case CONNECTED:
            // ok proceed with the fieldbus operations
            break;
        case CONNECTED_WITH_ERRORS:
            // ok proceed with the fieldbus operations
            break;
        case DEVICE_BLACKLIST:
            if (theDevice->elapsed_time_ns >= THE_DEVICE_BLACKLIST_ns) {
                changeDeviceStatus(d, NOT_CONNECTED);
            }
            break;
        default:
            ;
        }

        // can we continue?
        if (theDevice->status != CONNECTED && theDevice->status != CONNECTED_WITH_ERRORS) {
            // XX_GPIO_CLR_69(d);
            osSleep(THE_CONNECTION_DELAY_ms);
            continue;
        }

        // check the node status before actually operating
        if (Data_node == 0xffff) {
            error = TimeoutError;

        } else if (theNodes[Data_node].status == DISCONNECTED || theNodes[Data_node].status == BLACKLIST) {
            error = TimeoutError;
#ifdef VERBOSE_DEBUG
              if (theDevice->protocol == RTU /*&& theDevice->port == 0 && theDevice->u.serial.baudrate == 38400*/) {
                fprintf(stderr, "%s@%09u ms: %s (blacklist) %u vars @ %u\n",
                        theDevice->name, theDevice->current_time_ms,
                        Operation == READ ? "read" : "write",
                        DataNumber, DataAddr);
              }
#endif
        } else if (theNodes[Data_node].status == NODE_DISABLED) {
            unsigned i;
            error = NoError;
            for (i = 0; i < DataNumber; ++i) {
                DataValue[i] = 0;
            }
        } else {

            // the device is connected, so operate, without locking the mutex
            updateDeviceIdleTime(d);
            if (readOperation) {
                incDiagnostic(theDevice->diagnosticAddr, DIAGNOSTIC_READS);
                error = fieldbusRead(d, DataAddr, DataValue, DataNumber);
            } else {
                incDiagnostic(theDevice->diagnosticAddr, DIAGNOSTIC_WRITES);
                if (CrossTable[DataAddr].Output) {
                    error = fieldbusWrite(d, DataAddr, DataValue, DataNumber);
                } else {
                    error = CommError;
                }
            }
            updateDeviceBusyTime(d);

            // fieldbus wait silence_ms afterwards
        }
        setDiagnostic(theDevice->diagnosticAddr, DIAGNOSTIC_LAST_ERROR, error);

        // check error and set values and flags
        // XX_GPIO_CLR_69(d);
        pthread_mutex_lock(&theCrosstableClientMutex);
        {
            u_int16_t i;

            // XX_GPIO_SET_69(d);
#ifdef VERBOSE_DEBUG
            if (theDevice->protocol == RTU /*&& theDevice->port == 0 && theDevice->u.serial.baudrate == 38400*/) {
            fprintf(stderr, "%s@%09u ms: %s %s %u vars @ %u\n", theDevice->name, theDevice->current_time_ms,
                    Operation == READ ? "read" : "write",
                    error == NoError ? "ok" : "error",
                    DataNumber, DataAddr);
            }
#endif
            // manage the data values, data status, node status and device status
            switch (error) {

            case NoError:
                // node status
                if (theNodes[Data_node].status != NODE_OK && theNodes[Data_node].status != NODE_DISABLED)
                    changeNodeStatus(d, Data_node, NODE_OK);
                // device status
                if (theDevice->status != CONNECTED)
                    changeDeviceStatus(d, CONNECTED);
                // data values and status
                for (i = 0; i < DataNumber; ++i) {
                    writeQdataRegisters(DataAddr + i, DataValue[i], DATA_OK);
                }
                DataAddr = 0; // i.e. get next
                break;

            case CommError:
                incDiagnostic(theDevice->diagnosticAddr, DIAGNOSTIC_COMM_ERRORS);
                // node status
                switch (theNodes[Data_node].status) {
                case NODE_OK:
                    if (readOperation) {
                        // data values and status
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_WARNING);
                        }
                        DataAddr = 0; // i.e. get next
                    } else if (system_ini.system.retries > 0) {
                        // write operation and we can retry
                        changeNodeStatus(d, Data_node, TIMEOUT);
                        // data values and status
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_WARNING);
                        }
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                    } else {
                        // write operation and we cannot retry
                        changeNodeStatus(d, Data_node, BLACKLIST); // also data=0 status=DATA_ERROR
                        DataAddr = 0; // i.e. we lose a write
                    }
                    break;
                case TIMEOUT:
                    theNodes[Data_node].retries += 1;
                    if (readOperation) {
                        changeNodeStatus(d, Data_node, NODE_OK);
                        // data values and status
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_WARNING);
                        }
                        DataAddr = 0; // i.e. get next
                    } else if (theNodes[Data_node].retries < system_ini.system.retries) {
                        // write operation and we can retry
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_WARNING);
                        }
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                    } else {
                        // write operation and we cannot retry
                        changeNodeStatus(d, Data_node, BLACKLIST); // also data=0 status=DATA_ERROR
                        DataAddr = 0; // i.e. we lose a write
                    }
                    break;
                case BLACKLIST:
                case DISCONNECTED:
                case NODE_DISABLED:
                default:
                    ;
                }
                // device status
                if (theDevice->status == CONNECTED_WITH_ERRORS) {
                    // we received something, even if wrong
                    changeDeviceStatus(d, CONNECTED);
                }
                break;

            case TimeoutError:
                incDiagnostic(theDevice->diagnosticAddr, DIAGNOSTIC_TIMEOUTS);
                // node status
                switch (theNodes[Data_node].status) {
                case NODE_OK:
                    if (system_ini.system.retries > 0) {
                        // any operation and we can retry
                        changeNodeStatus(d, Data_node, TIMEOUT);
                        // data values and status
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_WARNING);
                        }
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                    } else {
                        // any operation and we cannot retry
                        changeNodeStatus(d, Data_node, BLACKLIST); // also data=0 status=DATA_ERROR
                        if (readOperation) {
                            DataAddr = 0; // i.e. retry this after the others
                        } else {
                            DataAddr = 0; // i.e. we lose a write
                        }
                    }
                    break;
                case TIMEOUT:
                    theNodes[Data_node].retries += 1;
                    if (theNodes[Data_node].retries < system_ini.system.retries) {
                        // any operation and we can retry
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_WARNING);
                        }
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                    } else {
                        // any operation and we cannot retry
                        changeNodeStatus(d, Data_node, BLACKLIST); // also data=0 status=DATA_ERROR
                        if (readOperation) {
                            DataAddr = 0; // i.e. retry this after the others
                        } else {
                            DataAddr = 0; // i.e. we lose a write
                        }
                    }
                    break;
                case BLACKLIST:
                    theNodes[Data_node].blacklist -= 1;
                    // no fieldbus operations, so no error
                    if (theNodes[Data_node].blacklist > 0) {
                        // data values and status
                        for (i = 0; i < DataNumber; ++i) {
                            writeQdataRegisters(DataAddr + i, 0, DATA_ERROR);
                        }
                        if (readOperation) {
                            DataAddr = 0; // i.e. retry this after the others
                        } else {
                            DataAddr = 0; // i.e. we lose a write
                        }
                    } else {
                        changeNodeStatus(d, Data_node, TIMEOUT);
                        DataAddr = DataAddr; // i.e. RETRY this immediately
                    }
                    break;
                case DISCONNECTED:
                    // data values and status
                    for (i = 0; i < DataNumber; ++i) {
                        writeQdataRegisters(DataAddr + i, 0, DATA_ERROR);
                    }
                    if (readOperation) {
                        DataAddr = 0; // i.e. retry this after the others
                    } else {
                        DataAddr = 0; // i.e. we lose a write
                    }
                    break;
                case NODE_DISABLED:
                default:
                    ;
                }
                // device status
                if (theDevice->status == CONNECTED) {
                    changeDeviceStatus(d, CONNECTED_WITH_ERRORS);
                } else {
                    // maybe we need a reset?
                    updateDeviceTiming(d);
                    if (theDevice->elapsed_time_ns > THE_DEVICE_SILENCE_ns) {
                        // too much silence
                        do_fieldbusReset = TRUE;
                        changeDeviceStatus(d, NOT_CONNECTED); // also data=0 status=DATA_ERROR
                    }
                }
                break;

            case ConnReset:
                incDiagnostic(theDevice->diagnosticAddr, DIAGNOSTIC_TIMEOUTS); // TIMEOUTS(RESET)
                // node status
                changeNodeStatus(d, Data_node, DISCONNECTED);
                // data values and status
                for (i = 0; i < DataNumber; ++i) {
                    writeQdataRegisters(DataAddr + i, 0, DATA_ERROR);
                }
                if (readOperation) {
                    DataAddr = 0; // i.e. retry this after the others
                } else {
                    DataAddr = 0; // i.e. we lose a write
                }
                // device status
                do_fieldbusReset = TRUE;
                changeDeviceStatus(d, NOT_CONNECTED); // also data=0 status=DATA_ERROR
                break;

            default:
                ;
            }
            // XX_GPIO_CLR_69(d);
        }
        pthread_mutex_unlock(&theCrosstableClientMutex);

        // if necessary reset, with unlocked mutex
        if (do_fieldbusReset) {
            fieldbusReset(d);
        }

        // if necessary respect the silence, with unlocked mutex
        switch (theDevice->protocol) {
        case RTU:
        case MECT:
            if (theDevice->silence_ms > 0) {
                // XX_GPIO_CLR_69(d);
                osSleep(theDevice->silence_ms);
                // XX_GPIO_SET_69(d);
            }
            break;
        default:
            ;
        }

        // while
    }
    // XX_GPIO_SET_69(d);

    // thread clean
    switch (theDevice->protocol) {
    case PLC:
        // FIXME: assert
        break;
    case RTU:
    case TCP:
    case TCPRTU:
        if (theDevice->modbus_ctx != NULL) {
            modbus_close(theDevice->modbus_ctx);
            modbus_free(theDevice->modbus_ctx);
            theDevice->modbus_ctx = NULL;
        }
        break;
    case CANOPEN:
        CANopenStop(theDevice->u.can.bus);
        break;
    case MECT: // close()
        mect_close(theDevice->mect_fd);
        break;
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        break;
    default:
        ;
    }

    // cleanup
    if (theDevice->device_vars != NULL) {
        free(theDevice->device_vars);
        theDevice->device_vars = NULL;
    }
    // exit
    // XX_GPIO_CLR_69(d);
    fprintf(stderr, "EXITING: %s\n", theDevice->name);
    theDevice->thread_status = EXITING;
    return arg;
}

/* ---------------------------------------------------------------------------- */

static void fieldbusReset(u_int16_t d)
{
    switch (theDevices[d].protocol) {
    case PLC:
        // FIXME: assert
        break;
    case RTU:
        modbus_close(theDevices[d].modbus_ctx);
        break;
    case TCP:
    {
        char buffer[MAX_IPADDR_LEN];

        modbus_close(theDevices[d].modbus_ctx);
        modbus_free(theDevices[d].modbus_ctx);
        theDevices[d].modbus_ctx = modbus_new_tcp(ipaddr2str(theDevices[d].u.tcp_ip.IPaddr, buffer), theDevices[d].u.tcp_ip.port);
    } break;
    case TCPRTU:
    {
        char buffer[MAX_IPADDR_LEN];

        modbus_close(theDevices[d].modbus_ctx);
        modbus_free(theDevices[d].modbus_ctx);
        theDevices[d].modbus_ctx = modbus_new_tcprtu(ipaddr2str(theDevices[d].u.tcp_ip.IPaddr, buffer), theDevices[d].u.tcp_ip.port);
    } break;
    case CANOPEN:
        // CANopenResetChannel(theDevices[d].u.can.bus);
        break;
    case MECT: // fieldbusReset()
        mect_close(theDevices[d].mect_fd);
        break;
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        break;
    default:
        ;
    }
}

static enum fieldbusError fieldbusRead(u_int16_t d, u_int16_t DataAddr, u_int32_t DataValue[], u_int16_t DataNumber)
{
    enum fieldbusError retval = NoError;
    u_int16_t i, r, regs, device, server, x;
    int e = 0;
    u_int8_t bitRegs[MODBUS_MAX_READ_BITS];         // > MAX_READS
    u_int16_t uintRegs[MODBUS_MAX_READ_REGISTERS];  // > MAX_READS

    switch (theDevices[d].protocol) {
    case PLC:
        // FIXME: assert
        break;
    case RTU:
    case TCP:
    case TCPRTU:
        if (modbus_set_slave(theDevices[d].modbus_ctx, CrossTable[DataAddr].NodeId)) {
            retval = CommError;
            break;
        }
        // no break, continue in the following code
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        regs = modbusRegistersNumber(DataAddr, DataNumber);
        switch (theDevices[d].protocol) {
        case RTU:
        case TCP:
        case TCPRTU:
            if (CrossTable[DataAddr].Types == BIT) {
                bzero(bitRegs, sizeof(bitRegs));
                e = modbus_read_bits(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, bitRegs);
            } else if (CrossTable[DataAddr].Offset >= 300000 && CrossTable[DataAddr].Offset < 365536) {
                bzero(uintRegs, sizeof(uintRegs));
                e = modbus_read_input_registers(theDevices[d].modbus_ctx,
                    CrossTable[DataAddr].Offset - 300000, regs, uintRegs);
#if 0
            } else if (CrossTable[DataAddr].Offset >= 40001 && CrossTable[DataAddr].Offset < 50000) {
                bzero(uintRegs, sizeof(uintRegs));
                e = modbus_read_registers(theDevices[d].modbus_ctx,
                    CrossTable[DataAddr].Offset - 40001, regs, uintRegs);
#endif
            } else {
                bzero(uintRegs, sizeof(uintRegs));
                e = modbus_read_registers(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, uintRegs);
            }
            break;
        case RTU_SRV:
        case TCP_SRV:
        case TCPRTU_SRV:
            device = CrossTable[DataAddr].device;
            if (device != 0xffff) {
                server = theDevices[device].server;
                if (server != 0xffff && theServers[server].mb_mapping != NULL) {
                    pthread_mutex_lock(&theServers[server].mutex);
                    {
                        register u_int32_t base = CrossTable[DataAddr].Offset;
                        bzero(uintRegs, sizeof(uintRegs));
                        for (r = 0; r < regs; ++r) {
                            if ((base + r) < REG_SRV_NUMBER) {
                                uintRegs[r] = theServers[server].mb_mapping->tab_registers[base + r];
                            }
                        }
                        e = regs;
                    }
                    pthread_mutex_unlock(&theServers[server].mutex);
#ifdef VERBOSE_DEBUG
                    if (theDevices[d].protocol == TCP_SRV) {
                        fprintf(stderr, "%s read %u (%u) vars from %u (%s)\n", theDevices[d].name, DataNumber, regs, DataAddr, CrossTable[DataAddr].Tag);
                    }
#endif
                }
            }
            break;
        default:
            ;
        }
        if (e == regs) {
            retval = NoError;
            for (i = 0, r = 0; i < DataNumber && r < regs; ++i) {
#define DO_READ_OUTPUTS
#ifndef DO_READ_OUTPUTS
                if (CrossTable[DataAddr + i].Output) {
                    DataValue[i] = VAR_VALUE(DataAddr + i);
                    switch (CrossTable[DataAddr + i].Types) {
                    case BIT:
                         r += 1;
                         break;
                    case BYTE_BIT:
                    case WORD_BIT:
                    case DWORD_BIT:
                        // manage this and the other *_BIT variables of the same offset
                        do {
                            DataValue[i] = VAR_VALUE(DataAddr + i);
                        } while ((i + 1) < DataNumber
                            && CrossTable[DataAddr + (i + 1)].Types == CrossTable[DataAddr + i].Types
                            && CrossTable[DataAddr + (i + 1)].Offset == CrossTable[DataAddr + i].Offset
                            && ++i);
                        r += (CrossTable[DataAddr + i].Types == DWORD_BIT) ? 2 : 1;
                        break;
                    case UINT8:
                    case UINT16:
                    case INT16:
                    case UINT16BA:
                    case INT16BA:
                        r += 1;
                        break;
                    case UDINT:
                    case DINT:
                    case REAL:
                    case UDINTCDAB:
                    case DINTCDAB:
                    case REALCDAB:
                    case UDINTDCBA:
                    case DINTDCBA:
                    case REALDCBA:
                    case UDINTBADC:
                    case DINTBADC:
                    case REALBADC:
                        r += 2;
                        break;
                    default:
                        ;
                    }
                } else {
#endif
                    register u_int8_t a = (uintRegs[r] & 0x00FF);
                    register u_int8_t b = (uintRegs[r] & 0xFF00) >> 8;
                    register u_int8_t c = (uintRegs[r + 1] & 0x00FF);
                    register u_int8_t d = (uintRegs[r + 1] & 0xFF00) >> 8;

                    switch (CrossTable[DataAddr + i].Types) {
                    case BIT:
                        DataValue[i] = bitRegs[r];
                        r += 1;
                        break;
                    case BYTE_BIT:
                    case WORD_BIT:
                    case DWORD_BIT: {
                        register enum varTypes vartype = CrossTable[DataAddr + i].Types;
                        register u_int32_t offset = CrossTable[DataAddr + i].Offset;

                        do {
                            // manage this and the other *_BIT variables of the same offset
                            x = CrossTable[DataAddr + i].Decimal; // 1..32
                            if (x <= 16) {
                                DataValue[i] = get_word_bit(uintRegs[r], x);
                            } else if (x <= 32 && (r + 1) < regs) {
                                DataValue[i] = get_word_bit(uintRegs[r + 1], x - 16);
                            } else {
                                DataValue[i] = 0; // FIXME: assert
                            }
                        } while ((i + 1) < DataNumber
                            && CrossTable[DataAddr + (i + 1)].Types == vartype
                            && CrossTable[DataAddr + (i + 1)].Offset == offset
                            && ++i);
                        r += (vartype == DWORD_BIT) ? 2 : 1;
                    }   break;
                    case UINT8:
                    case UINT16:
                    case INT16:
                        DataValue[i] = uintRegs[r];
                        r += 1;
                        break;
                    case UINT16BA:
                    case INT16BA:
                        DataValue[i] = b + (a << 8);
                        r += 1;
                        break;
                    case UDINT:
                    case DINT:
                    case REAL:
                        DataValue[i] = a + (b << 8) + (c << 16) + (d << 24);
                        r += 2;
                        break;
                    case UDINTCDAB:
                    case DINTCDAB:
                    case REALCDAB:
                        DataValue[i] = c + (d << 8) + (a << 16) + (b << 24);
                        r += 2;
                        break;
                    case UDINTDCBA:
                    case DINTDCBA:
                    case REALDCBA:
                        DataValue[i] = d + (c << 8) + (b << 16) + (a << 24);
                        r += 2;
                        break;
                    case UDINTBADC:
                    case DINTBADC:
                    case REALBADC:
                        DataValue[i] = b + (a << 8) + (d << 16) + (c << 24);
                        r += 2;
                        break;
                    default:
                        ;
                    }
#ifndef DO_READ_OUTPUTS
                }
#endif
            }
        } else {
            if (e == -1) { // OTHER_ERROR
                if ((errno == EBADF || errno == ECONNRESET || errno == EPIPE)) {
                    retval = ConnReset;
                } else {
                    retval = CommError;
                }
            } else if (e == -2) { // TIMEOUT_ERROR
                retval = TimeoutError;
            } else {
                retval = CommError;
            }
            if (verbose_print_enabled) {
                fprintf(stderr, "fieldbusRead(%d, %u, %u): %d,%d(%s) --> %d\n",
                    d, DataAddr, DataNumber,
                    e, errno, modbus_strerror(errno),
                    retval);
            }
        }
        break;
    case CANOPEN:
        device = CrossTable[DataAddr].device;
        if (device != 0xffff) {
            u_int8_t channel = theDevices[device].port;
            for (i = 0; i < DataNumber; ++i) {
                register enum varTypes vartype = CrossTable[DataAddr + i].Types;
                register u_int32_t offset = CrossTable[DataAddr + i].Offset;

                // cannot read outputs, anyway
                if (CrossTable[DataAddr + i].Output) {
                    DataValue[i] = VAR_VALUE(DataAddr + i);
                } else {
                    switch (CrossTable[DataAddr + i].Types) {
                    case BIT: {
                        u_int8_t a;
                        e = CANopenReadPDOBit(channel, offset, &a);
                        DataValue[i] = a;
                    }   break;
                    case  BYTE_BIT: {
                        u_int8_t a;
                        e = CANopenReadPDOByte(channel, offset, &a);
                        do {
                            x = CrossTable[DataAddr + i].Decimal; // 1..8
                            DataValue[i] = get_byte_bit(a, x);
                        } while ((i + 1) < DataNumber
                            && CrossTable[DataAddr + (i + 1)].Types == vartype
                            && CrossTable[DataAddr + (i + 1)].Offset == offset
                            && ++i);
                    }   break;
                    case  WORD_BIT: {
                        u_int16_t a;
                        e = CANopenReadPDOWord(channel, offset, &a);
                        do {
                            x = CrossTable[DataAddr + i].Decimal; // 1..16
                            DataValue[i] = get_word_bit(a, x);
                        } while ((i + 1) < DataNumber
                            && CrossTable[DataAddr + (i + 1)].Types == vartype
                            && CrossTable[DataAddr + (i + 1)].Offset == offset
                            && ++i);
                    }   break;
                    case DWORD_BIT:{
                        u_int32_t a;
                        e = CANopenReadPDODword(channel, offset, &a);
                        do {
                            x = CrossTable[DataAddr + i].Decimal; // 1..32
                            DataValue[i] = get_dword_bit(a, x);
                        } while ((i + 1) < DataNumber
                            && CrossTable[DataAddr + (i + 1)].Types == vartype
                            && CrossTable[DataAddr + (i + 1)].Offset == offset
                            && ++i);
                    }   break;
                    case  UINT8: {
                        u_int8_t a;
                        e = CANopenReadPDOByte(channel, offset, &a);
                        DataValue[i] = a;
                    }   break;
                    case  UINT16:
                    case INT16: {
                        u_int16_t a;
                        e = CANopenReadPDOWord(channel, offset, &a);
                        DataValue[i] = a;
                    }   break;
                    case UDINT:
                    case DINT:
                    case REAL: {
                        u_int32_t a;
                        e = CANopenReadPDODword(channel, offset, &a);
                        DataValue[i] = a;
                    }   break;
                    case UINT16BA:
                    case INT16BA:
                    case UDINTCDAB:
                    case DINTCDAB:
                    case REALCDAB:
                    case UDINTDCBA:
                    case DINTDCBA:
                    case REALDCBA:
                    case UDINTBADC:
                    case DINTBADC:
                    case REALBADC:
                        // FIXME: assert
                        break;
                   default:
                        ;
                   }
                }
#ifdef VERBOSE_DEBUG
                fprintf(stderr, "%s: %s %s (%u)\n", theDevices[d].name, CrossTable[DataAddr + i].Tag,
                        e ? "err" : "ok", DataValue[i]);
#endif
            if (e) {
                if (e == -1) { // OTHER_ERROR
                    retval = CommError;
                } else if (e == -2) { // TIMEOUT_ERROR
                    retval = TimeoutError;
                } else if (e == -3) { // RESET_ERROR
                    retval = ConnReset;
                } else {
                    retval = CommError;
                }
                break;
            }
        }
#ifdef VERBOSE_DEBUG
            fprintf(stderr, "%d vars @ %d: ", DataNumber, DataAddr);
            for (i = 0; i < DataNumber; ++i) {
                fprintf(stderr, "%s = 0x%08x ", CrossTable[DataAddr + i].Tag, DataValue[i]);
            }
            fprintf(stderr, "\n");
#endif
        }
        break;
    case MECT: // fieldbusRead()
        for (i = 0; i < DataNumber; ++i) {

            if (CrossTable[DataAddr + i].Types == REAL) {
                float value;
                e = mect_read_ascii(theDevices[d].mect_fd, CrossTable[DataAddr + i].NodeId, CrossTable[DataAddr + i].Offset, &value);
                memcpy(&DataValue[i], &value, sizeof(u_int32_t));
                if (verbose_print_enabled) {
                    fprintf(stderr, "%s: %s=%f err=%d\n", theDevices[d].name, CrossTable[DataAddr + i].Tag, value, e);
                }
            } else if (CrossTable[DataAddr + i].Types == UINT16) {
                unsigned value = 0;
                e = mect_read_hexad(theDevices[d].mect_fd, CrossTable[DataAddr + i].NodeId, CrossTable[DataAddr + i].Offset, &value);
                DataValue[i] = value;
                if (verbose_print_enabled) {
                    fprintf(stderr, "%s: %s=0x%04x err=%d\n", theDevices[d].name, CrossTable[DataAddr + i].Tag, value, e);
                }
            }
            if (e == -1) {
                retval = CommError;
                break;
            } else if (e == -2) {
                retval = TimeoutError;
                break;
            }
        }
        break;
    default:
        ;
    }
    return retval;
}

static enum fieldbusError fieldbusWrite(u_int16_t d, u_int16_t DataAddr, u_int32_t DataValue[], u_int16_t DataNumber)
{
    enum fieldbusError retval = NoError;
    u_int16_t i, r, regs, device, server, x;
    int e = 0;
    u_int8_t bitRegs[MODBUS_MAX_WRITE_BITS];         // > MAX_WRITES
    u_int16_t uintRegs[MODBUS_MAX_WRITE_REGISTERS];  // > MAX_WRITES

    switch (theDevices[d].protocol) {
    case PLC:
        // FIXME: assert
        break;
    case RTU:
    case TCP:
    case TCPRTU:
        if (modbus_set_slave(theDevices[d].modbus_ctx, CrossTable[DataAddr].NodeId)) {
            retval = CommError;
            break;
        }
        // no break, continue in the following code
    case RTU_SRV:
    case TCP_SRV:
    case TCPRTU_SRV:
        // init values
        regs = modbusRegistersNumber(DataAddr, DataNumber);
        if (CrossTable[DataAddr].Types == BIT) {
            bzero(bitRegs, sizeof(bitRegs));
        } else {
            bzero(uintRegs, sizeof(uintRegs));
            // *_BIT management
            for (i = 0, r = 0; i < DataNumber && r < regs; ++i) {
                switch (CrossTable[DataAddr + i].Types) {
                case BIT:
                    r += 1; // FIXME: assert
                    break;
                case  BYTE_BIT:
                case  WORD_BIT:
                case DWORD_BIT: {
                    register u_int16_t addr;
                    register u_int16_t base = CrossTable[DataAddr + i].BlockBase;
                    register u_int16_t size = CrossTable[DataAddr + i].BlockSize;
                    register u_int32_t offset = CrossTable[DataAddr + i].Offset;
                    register enum varTypes vartype = CrossTable[DataAddr + i].Types;

                    // init the buffer bits with ALL the other actual bit values
                    for (addr = base; addr < (base + size); ++addr) {
                        if (CrossTable[addr].Types == vartype
                         && CrossTable[addr].Offset == offset
                         && !(DataAddr <= addr && addr < (DataAddr + DataNumber))) {
                            x = CrossTable[addr].Decimal; // 1..32
                            if (x <= 16) { // BYTE_BIT, WORD_BIT, DWORD_BIT
                                set_word_bit(&uintRegs[r], x, VAR_VALUE(addr));
                            } else if (x <= 32 && (r + 1) < regs) { // DWORD_BIT
                                set_word_bit(&uintRegs[r + 1], x - 16, VAR_VALUE(addr));
                            } else {
                                 // FIXME: assert
                            }
                        }
                    }
                    do {
                        // skip the other *_BIT variables of the same offset
                    } while ((i + 1) < DataNumber
                        && CrossTable[DataAddr + (i + 1)].Types == vartype
                        && CrossTable[DataAddr + (i + 1)].Offset == offset
                        && ++i);
                    r += (vartype == DWORD_BIT) ? 2 : 1;
                }   break;
                case UINT8:
                case UINT16:
                case INT16:
                case UINT16BA:
                case INT16BA:
                    r += 1;
                    break;
                case UDINT:
                case DINT:
                case REAL:
                case UDINTCDAB:
                case DINTCDAB:
                case REALCDAB:
                case UDINTDCBA:
                case DINTDCBA:
                case REALDCBA:
                case UDINTBADC:
                case DINTBADC:
                case REALBADC:
                    r += 2;
                    break;
                default:
                    ;
                }
            }
        }
        for (i = 0, r = 0; i < DataNumber && r < regs; ++i) {
            register u_int8_t a = (DataValue[i] & 0x000000FF);
            register u_int8_t b = (DataValue[i] & 0x0000FF00) >>  8;
            register u_int8_t c = (DataValue[i] & 0x00FF0000) >> 16;
            register u_int8_t d = (DataValue[i] & 0xFF000000) >> 24;

            switch (CrossTable[DataAddr + i].Types) {
            case BIT:
                bitRegs[r] = DataValue[i]; r += 1; break;
            case BYTE_BIT:
            case WORD_BIT:
            case DWORD_BIT: {
                register enum varTypes vartype = CrossTable[DataAddr + i].Types;
                register u_int32_t offset = CrossTable[DataAddr + i].Offset;

                do {
                    // manage this and the other *_BIT variables of the same offset
                    x = CrossTable[DataAddr + i].Decimal; // 1..32
                    if (x <= 16) { // BYTE_BIT, WORD_BIT, DWORD_BIT
                        set_word_bit(&uintRegs[r], x, DataValue[i]);
                    } else if (x <= 32 && (r + 1) < regs) { // DWORD_BIT
                        set_word_bit(&uintRegs[r + 1], x - 16, DataValue[i]);
                    }
                } while ((i + 1) < DataNumber
                      && CrossTable[DataAddr + (i + 1)].Types == vartype
                      && CrossTable[DataAddr + (i + 1)].Offset == offset
                      && ++i);
                r += (vartype == DWORD_BIT) ? 2 : 1;
            }   break;
            case UINT8:
            case UINT16:
            case INT16:
                uintRegs[r] = DataValue[i];
                r += 1;
                break;
            case UINT16BA:
            case  INT16BA:
                uintRegs[r] = b + (a << 8);
                r += 1;
                break;
            case UDINT:
            case DINT:
            case REAL:
                uintRegs[r] = a + (b << 8);
                uintRegs[r + 1] = c + (d << 8);
                r += 2;
                break;
            case UDINTCDAB:
            case DINTCDAB:
            case REALCDAB:
                uintRegs[r] = c + (d << 8);
                uintRegs[r + 1] = a + (b << 8);
                r += 2;
                break;
            case UDINTDCBA:
            case DINTDCBA:
            case REALDCBA:
                uintRegs[r] = d + (c << 8);
                uintRegs[r + 1] = b + (a << 8);
                r += 2;
                break;
            case UDINTBADC:
            case DINTBADC:
            case REALBADC:
                uintRegs[r] = b + (a << 8);
                uintRegs[r + 1] = d + (c << 8);
                r += 2;
                break;
            default:
                ;
            }
        }
        switch (theDevices[d].protocol) {
        case RTU:
        case TCP:
        case TCPRTU:
            if (CrossTable[DataAddr].Types == BIT) {
                e = modbus_write_bits(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, bitRegs);
            } else if (CrossTable[DataAddr].Offset >= 300000 && CrossTable[DataAddr].Offset < 365536) {
                e = -1; // cannot write inputs
#if 0
            } else if (CrossTable[DataAddr].Offset >= 40001 && CrossTable[DataAddr].Offset < 50000) {
                if (regs == 1){
                    e = modbus_write_register(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset - 40001, uintRegs[0]);
                } else {
                    e = modbus_write_registers(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset - 40001, regs, uintRegs);
                }
#endif
            } else {
                if (regs == 1){
                    e = modbus_write_register(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, uintRegs[0]);
                } else {
                    e = modbus_write_registers(theDevices[d].modbus_ctx, CrossTable[DataAddr].Offset, regs, uintRegs);
                }
            }
#ifdef VERBOSE_DEBUG
        {
            int z;
            fprintf(stderr, "%s wrote %u (%u) vars from %u (%s)\n", theDevices[d].name, DataNumber, regs, DataAddr, CrossTable[DataAddr].Tag);
            for (z = 0; z < regs; ++z) {
                fprintf(stderr, "\t%02d: 0x%04x\n", z, uintRegs[z]);
            }
            fprintf(stderr, "\tresult = %02d\n", e);

        }
#endif
            break;
        case RTU_SRV:
        case TCP_SRV:
        case TCPRTU_SRV:
            device = CrossTable[DataAddr].device;
            if (device != 0xffff) {
                server = theDevices[device].server;
                if (server != 0xffff && theServers[server].mb_mapping != NULL) {
                    pthread_mutex_lock(&theServers[server].mutex);
                    {
                        register u_int16_t base = CrossTable[DataAddr].Offset;
                        for (r = 0; r < regs; ++r) {
                            if ((base + r) < REG_SRV_NUMBER) {
                                theServers[server].mb_mapping->tab_registers[base + r] = uintRegs[r];
                            }
                        }
                    }
                    pthread_mutex_unlock(&theServers[server].mutex);
                }
            }
            e = regs;
            break;
        default:
            ; //FIXME: assert
        }
        if (e == regs) {
            retval = NoError;
        } else {
            if (e == -1) { // OTHER_ERROR
                if ((errno == EBADF || errno == ECONNRESET || errno == EPIPE)) {
                    retval = ConnReset;
                } else {
                    retval = CommError;
                }
            } else if (e == -2) { // TIMEOUT_ERROR
                retval = TimeoutError;
            } else {
                retval = CommError;
            }
        if (verbose_print_enabled) {
            fprintf(stderr, "fieldbusWrite(%d, %u, %u): %d,%d(%s) --> %d\n",
                d, DataAddr, DataNumber,
                e, errno, modbus_strerror(errno),
                retval);
        }
        }
        break;
    case CANOPEN:
        device = CrossTable[DataAddr].device;
        if (device != 0xffff) {
            u_int8_t channel = theDevices[device].port;
            for (i = 0; i < DataNumber; ++i) {
                register u_int32_t offset = CrossTable[DataAddr + i].Offset;

                switch (CrossTable[DataAddr + i].Types) {
                case       BIT:
                    e = CANopenWritePDOBit(channel, offset, DataValue[i]);
                    break;
                case BYTE_BIT:
                case WORD_BIT:
                case DWORD_BIT:  {
                    register u_int16_t addr;
                    register u_int16_t base = CrossTable[DataAddr + i].BlockBase;
                    register u_int16_t size = CrossTable[DataAddr + i].BlockSize;
                    register u_int32_t offset = CrossTable[DataAddr + i].Offset;
                    register enum varTypes vartype = CrossTable[DataAddr + i].Types;
                    u_int32_t buffer = 0x00000000; // BYTE_BIT, WORD_BIT, DWORD_BIT

                    // init the buffer bits with ALL the actual bit values
                    for (addr = base; addr < (base + size); ++addr) {
                        if (CrossTable[addr].Types == vartype
                         && CrossTable[addr].Offset == offset) {
                            x = CrossTable[addr].Decimal; // 1..8/16/32
                            if (DataAddr <= addr && addr < (DataAddr + DataNumber)) {
                                set_dword_bit(&buffer, x, DataValue[addr - DataAddr]);
                            } else {
                                set_dword_bit(&buffer, x, VAR_VALUE(addr));
                            }
                        }
                    }
                    // do write
                    switch (vartype) {
                    case BYTE_BIT: {
                        register u_int8_t a = (u_int8_t)(buffer & 0x000000FF);
                        e = CANopenWritePDOByte(channel, offset, a);
                    }   break;
                    case WORD_BIT: {
                        register u_int16_t a = (u_int16_t)(buffer & 0x0000FFFF);
                        e = CANopenWritePDOWord(channel, offset, a);
                    }   break;
                    case DWORD_BIT:
                        e = CANopenWritePDODword(channel, offset, buffer);
                        break;
                    default:
                        ; // FIXME: assert
                    }
                    do {
                        // skip the other *_BIT variables of the same offset
                    } while ((i + 1) < DataNumber
                        && CrossTable[DataAddr + (i + 1)].Types == vartype
                        && CrossTable[DataAddr + (i + 1)].Offset == offset
                        && ++i);
                }   break;
                case UINT8:
                    e = CANopenWritePDOByte(channel, offset, DataValue[i]);
                    break;
                case UINT16:
                case INT16:
                    e = CANopenWritePDOWord(channel, offset, DataValue[i]);
                    break;
                case UDINT:
                case DINT:
                case REAL:
                    e = CANopenWritePDODword(channel, offset, DataValue[i]);
                    break;
                case UINT16BA:
                case INT16BA:
                case UDINTCDAB:
                case  DINTCDAB:
                case REALCDAB:
                case UDINTDCBA:
                case DINTDCBA:
                case REALDCBA:
                case UDINTBADC:
                case DINTBADC:
                case REALBADC:
                    // FIXME: assert
                    break;
                default:
                    ;
                }
        if (e) {
            if (e == -1) { // OTHER_ERROR
                retval = CommError;
            } else if (e == -2) { // TIMEOUT_ERROR
                retval = TimeoutError;
            } else if (e == -3) { // RESET_ERROR
                retval = ConnReset;
            } else {
                retval = CommError;
            }
            break;
        }
        }
        }
        break;
    case MECT: // fieldbusWrite()
        for (i = 0; i < DataNumber; ++i) {

            if (CrossTable[DataAddr + i].Types == REAL) {
                float value;
                memcpy(&value, &DataValue[i], sizeof(u_int32_t));
                e = mect_write_ascii(theDevices[d].mect_fd, CrossTable[DataAddr + i].NodeId, CrossTable[DataAddr + i].Offset, value);
            } else if (CrossTable[DataAddr + i].Types == UINT16) {
                unsigned value = DataValue[i];
                e = mect_write_hexad(theDevices[d].mect_fd, CrossTable[DataAddr + i].NodeId, CrossTable[DataAddr + i].Offset, value);
            }
            if (e == -1) {
                retval = CommError;
                break;
            } else if (e == -2) {
                retval = TimeoutError;
                break;
            }
        }
        break;
    default:
        ;
    }
    return retval;
}

/* ---------------------------------------------------------------------------- */

static void doWriteDeviceRetentives(u_int32_t d)
{
    unsigned addr, written;

    for (addr = 1;  addr <= LAST_RETENTIVE; ++addr) {
        if (CrossTable[addr].device == d && CrossTable[addr].Output) {

            //                        addr  value                     values              flags addrMax
            written = doWriteVariable(addr, VAR_VALUE(addr), (uint32_t *)plcBlock.values, NULL, LAST_RETENTIVE);
            if (written > 1)
                addr += written - 1;
        }
    }
}

/* ---------------------------------------------------------------------------- */

static inline void changeDeviceStatus(u_int32_t d, enum DeviceStatus status)
{
    u_int16_t addr;
    int n;
    enum DeviceStatus previous_status;

    if (verbose_print_enabled) {
        fprintf(stderr, "%s: status = %s\n", theDevices[d].name, deviceStatusName[status]);
    }
    previous_status = theDevices[d].status;
    theDevices[d].status = status;
    setDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_STATUS, status);

    theDevices[d].elapsed_time_ns = 0;

    switch (status) {
    case ZERO:
    case NO_HOPE:
        break;
    case NOT_CONNECTED:
        if (previous_status == CONNECTED_WITH_ERRORS || previous_status == CONNECTED) {
            // set to 0 the input variables for all device nodes
            for (n = 0; n < theDevices[d].var_num; ++n) {
                addr = theDevices[d].device_vars[n].addr;
                if (! CrossTable[addr].Output) {
                    writeQdataRegisters(addr, 0, DATA_ERROR);
                }
            }
            // stop the device nodes on connection failure
            for (n = 0; n < theNodesNumber; ++n) {
                if (theNodes[n].device == d && theNodes[n].status != DISCONNECTED && theNodes[n].status != NODE_DISABLED) {
                    changeNodeStatus(d, n, DISCONNECTED);
                }
            }
        }
        break;
    case DEVICE_BLACKLIST:
        if (previous_status == NOT_CONNECTED) {
            // stop the device nodes on connection failure
            for (n = 0; n < theNodesNumber; ++n) {
                if (theNodes[n].device == d && theNodes[n].status != DISCONNECTED && theNodes[n].status != NODE_DISABLED) {
                    changeNodeStatus(d, n, DISCONNECTED);
                }
            }
        }
        break;
    case CONNECTED:
        if (previous_status == NOT_CONNECTED) {
            // try resurrecting the device nodes on connection success
            for (n = 0; n < theNodesNumber; ++n) {
                if (theNodes[n].device == d && theNodes[n].status != NODE_OK && theNodes[n].status != NODE_DISABLED) {
                    changeNodeStatus(d, n, NODE_OK);
                }
            }
        }
        break;
    case CONNECTED_WITH_ERRORS:
        break;
    default:
        ;
    }
}

static inline void changeNodeStatus(u_int32_t d, u_int16_t node, enum NodeStatus status)
{
    u_int16_t addr;
    int n;

    if (verbose_print_enabled) {
        fprintf(stderr, "node #%02u: status = %s\n", node+1, nodeStatusName[status]);
    }
    theNodes[node].status = status;
    setDiagnostic(theNodes[node].diagnosticAddr, DIAGNOSTIC_NODE_STATUS, theNodes[node].status);

    switch (status) {
    case NO_NODE:
    case NODE_OK:
        break;
    case TIMEOUT:
        theNodes[node].retries = 0;
        break;
    case BLACKLIST:
        theNodes[node].blacklist = system_ini.system.blacklist;
        // zeroNodeVariables
        for (n = 0; n < theDevices[d].var_num; ++n) {
            addr = theDevices[d].device_vars[n].addr;
            if (CrossTable[addr].node == node && ! CrossTable[addr].Output) {
#ifdef VERBOSE_DEBUG
                fprintf(stderr, "\tnode #%02u, addr #%04u(%s): value=0 status = DATA_ERROR\n", node+1, addr, CrossTable[addr].Tag);
#endif
                writeQdataRegisters(addr, 0, DATA_ERROR);
            }
        }
        break;
    case DISCONNECTED:
    case NODE_DISABLED:
    default:
        ;
    }
}

/* ---------------------------------------------------------------------------- */

static unsigned checkAddr(unsigned d, unsigned DataAddr, unsigned DataNumber)
{
    unsigned n;

    if (DataAddr == 0 || DataAddr > DimCrossTable) {
        fprintf(stderr, "clientThread(%u) wrong DataAddr %u[%u]\n", d, DataAddr, DataNumber);
        return 0;
    } else if (DataNumber == 0 || DataNumber > MAX_VALUES || (DataAddr + DataNumber) > DimCrossTable) {
        fprintf(stderr, "clientThread(%u) wrong DataNumber %u[%u]\n", d, DataAddr, DataNumber);
        return 0;
    } else {
        for (n = 0; n < DataNumber; ++n) {
            if (CrossTable[DataAddr + n].device != d) {
                fprintf(stderr, "clientThread(%u) wrong device %u @(%u+%u)\n", d, CrossTable[DataAddr + n].device, DataAddr, n);
                return 0;
            }
        }
    }
    return DataAddr;
}

/* ---------------------------------------------------------------------------- */

static void startDeviceTiming(u_int32_t d)
{
    RTIME now_ns = rt_timer_read();

    theDevices[d].elapsed_time_ns = 0;
    theDevices[d].current_time_ns = now_ns;

    theDevices[d].idle_time_ns = 0;
    theDevices[d].busy_time_ns = 0;
    theDevices[d].last_time_ns = now_ns;
    theDevices[d].last_update_ns = now_ns;
}

static inline void updateDeviceTiming(u_int32_t d)
{
    RTIME now_ns = rt_timer_read();

    RTIME delta_ns = now_ns - theDevices[d].current_time_ns;
    theDevices[d].elapsed_time_ns += delta_ns;
    theDevices[d].current_time_ns = now_ns;

    // update idle/busy statistics each 5 seconds
    if ((now_ns - theDevices[d].last_update_ns) > 5000000000ULL) {
        unsigned bus_load;

        // add last idle time
        delta_ns = now_ns - theDevices[d].last_time_ns;
        theDevices[d].idle_time_ns += delta_ns;

        // update diagnostics: busy load percentage is: (100 * busy_time / total_time)
        if (theDevices[d].busy_time_ns == 0) {
            bus_load = 0;
        } else {
            lldiv_t x;

            x = lldiv((100 * theDevices[d].busy_time_ns), (theDevices[d].busy_time_ns + theDevices[d].idle_time_ns));
            bus_load = x.quot;
        }
        setDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_BUS_LOAD, bus_load);
        // reset
        theDevices[d].idle_time_ns = 0;
        theDevices[d].busy_time_ns = 0;
        theDevices[d].last_time_ns = now_ns;
        theDevices[d].last_update_ns = now_ns;
    }
}

static inline void updateDeviceIdleTime(u_int32_t d)
{
    RTIME now_ns = rt_timer_read();
    RTIME delta_ns = now_ns - theDevices[d].last_time_ns;

    theDevices[d].idle_time_ns += delta_ns;
    theDevices[d].last_time_ns = now_ns;
}

static inline void updateDeviceBusyTime(u_int32_t d)
{
    RTIME now_ns = rt_timer_read();

    RTIME delta_ns = now_ns - theDevices[d].last_time_ns;
    theDevices[d].busy_time_ns += delta_ns;
    theDevices[d].last_time_ns = now_ns;
}

/* ---------------------------------------------------------------------------- */

static u_int16_t modbusRegistersNumber(u_int16_t DataAddr, u_int16_t DataNumber)
{
    u_int16_t retval = 0, i;

    for (i = 0; i < DataNumber; ++i) {
        switch (CrossTable[DataAddr + i].Types) {
        case  DINT: case  DINTDCBA: case  DINTCDAB: case  DINTBADC:
        case UDINT: case UDINTDCBA: case UDINTCDAB: case UDINTBADC:
        case REAL: case REALDCBA: case REALCDAB: case REALBADC:
            retval += 2;
            break;
        case UINT8:
        case  INT16: case  INT16BA:
        case UINT16: case UINT16BA:
            retval += 1;
            break;
        case BIT:
            retval += 1;
            break;
        case BYTE_BIT:
        case WORD_BIT:
        case DWORD_BIT: {
            register enum varTypes vartype = CrossTable[DataAddr + i].Types;
            register u_int32_t offset = CrossTable[DataAddr + i].Offset;

            do {
                // skip the other *_BIT variables of the same offset
            } while ((i + 1) < DataNumber
                && CrossTable[DataAddr + (i + 1)].Types == vartype
                && CrossTable[DataAddr + (i + 1)].Offset == offset
                && ++i);
            retval += (vartype == DWORD_BIT) ? 2 : 1;
        }   break;
        default:
            ; // FIXME: assert
        }
    }
    return retval;
}

/* ---------------------------------------------------------------------------- */

static inline unsigned get_byte_bit(u_int8_t data, unsigned n)
{
    // bits 1..8
    if (data & (1 << (n - 1))) {
        return 1;
    } else {
        return 0;
    }
}

static inline unsigned get_word_bit(u_int16_t data, unsigned n)
{
    // bits 1..16
    if (data & (1 << (n - 1))) {
        return 1;
    } else {
        return 0;
    }
}

static inline unsigned get_dword_bit(u_int32_t data, unsigned n)
{
    // bits 1..32
    if (data & (1 << (n - 1))) {
        return 1;
    } else {
        return 0;
    }
}

static inline void set_byte_bit(u_int8_t *data, unsigned n, unsigned value)
{
    // bits 1..8
    if (value) {
        *data |= (1 << (n - 1));
    } else {
        *data &= ~(1 << (n - 1));
    }
}

static inline void set_word_bit(u_int16_t *data, unsigned n, unsigned value)
{
    // bits 1..16
    if (value) {
        *data |= (1 << (n - 1));
    } else {
        *data &= ~(1 << (n - 1));
    }
}

static inline void set_dword_bit(u_int32_t *data, unsigned n, unsigned value)
{
    // bits 1..32
    if (value) {
        *data |= (1 << (n - 1));
    } else {
        *data &= ~(1 << (n - 1));
    }
}

/* ---------------------------------------------------------------------------- */
