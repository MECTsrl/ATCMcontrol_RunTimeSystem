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
 * Filename: dataUtils.c
 */

#include "dataImpl.h"

#define __4CFILE__	"dataUtils.c"

/* ---------------------------------------------------------------------------- */

unsigned buzzer_beep_ms = 0;
unsigned buzzer_on_cs = 0;
unsigned buzzer_off_cs = 0;
unsigned buzzer_replies = 0;

unsigned buzzer_period_tics = 0;
unsigned buzzer_tic = 0;
unsigned buzzer_periods = 0;

/* ---------------------------------------------------------------------------- */

char *strtok_csv(char *string, const char *separators, char **savedptr)
{
    char *p, *s;

    if (separators == NULL || savedptr == NULL) {
        return NULL;
    }
    if (string == NULL) {
        p = *savedptr;
        if (p == NULL) {
            return NULL;
        }
    } else {
        p = string;
    }

    s = strstr(p, separators);
    if (s == NULL) {
        *savedptr = NULL;
        return p;
    }
    *s = 0;
    *savedptr = s + 1;

    // remove spaces at head
    while (p < s && isspace(*p)) {
        ++p;
    }
    // remove spaces at tail
    --s;
    while (s > p && isspace(*s)) {
        *s = 0;
        --s;
    }
    return p;
}

/* ---------------------------------------------------------------------------- */

u_int32_t str2ipaddr(const char *str)
{
    u_int32_t ipaddr = 0;
    char buffer[MAX_IPADDR_LEN];
    char *s, *r;
    int i;

    strncpy(buffer, str, MAX_IPADDR_LEN);
    buffer[16] = 0;

    s = strtok_csv(buffer, ".", &r);
    for (i = 3; i >= 0; --i) {
        if (s == NULL) {
            return 0x00000000;
        }
        ipaddr += (strtoul(s, NULL, 10) % 255) << (i * 8);
        s = strtok_csv(NULL, ".", &r);
    }
    return ipaddr;
}

char *ipaddr2str(u_int32_t ipaddr, char *buffer)
{
    if (buffer != NULL) {
        register u_int8_t a, b, c, d;
        a = (ipaddr & 0xFF000000) >> 24;
        b = (ipaddr & 0x00FF0000) >> 16;
        c = (ipaddr & 0x0000FF00) >> 8;
        d = (ipaddr & 0x000000FF);
        sprintf(buffer, "%u.%u.%u.%u", a, b, c, d);
    }
    return buffer;
}

/* ---------------------------------------------------------------------------- */

void setEngineStatus(enum EngineStatus status)
{
    engineStatus = status;
    writeQdataRegisters(PLC_EngineStatus, status, DATA_OK);
}

/* ---------------------------------------------------------------------------- */

unsigned doWriteVariable(unsigned addr, unsigned value, u_int32_t *values, u_int32_t *flags, unsigned addrMax)
{
    unsigned retval = 0;

    if (CrossTable[addr].Protocol == PLC) {

        // PLC only, immediate writing
        if (CrossTable[addr].Types == BIT || CrossTable[addr].Types == BYTE_BIT
            || CrossTable[addr].Types == WORD_BIT || CrossTable[addr].Types == DWORD_BIT ) {
            // the engine seems to use only the bit#0 for bool variables
            if (value & 1)
                writeQdataRegisters(addr, 1, DATA_OK);
            else
                writeQdataRegisters(addr, 0, DATA_OK);
        }
        else
            writeQdataRegisters(addr, values[addr], DATA_OK);

        // wrote one
        retval = 1;
    }
    else {
        // RTU, TCP, TCPRTU, CANOPEN, MECT, RTU_SRV, TCP_SRV, TCPRTU_SRV
        u_int16_t d = CrossTable[addr].device;

        // FIXME: error recovery
        if (d >= theDevicesNumber) {
            fprintf(stderr, "%s() unknown device error, writing addr=%u to device=%u\n",
                 __func__, addr, d);
            // wrote none
            retval = 0;
        }
        else {
            struct PLCwriteRequest request;

            // (1 of 3) extract the request for the first variable
            request.Addr = addr;
            request.Number = 1;
            if (CrossTable[addr].Types == BIT || CrossTable[addr].Types == BYTE_BIT
                || CrossTable[addr].Types == WORD_BIT || CrossTable[addr].Types == DWORD_BIT )
            {
                // the engine uses only the bit#0 for bool variables
                if (value & 1)
                    request.Values[0] = 1;
                else
                    request.Values[0] = 0;
            } else {
                request.Values[0] = value;
            }
            // wrote one
            retval = 1;

            // (2 of 3) extract the requests for the other variables
            if (values) {
                register int size, protocol, node;
                register unsigned n, base, type, offset;

                // are there any other consecutive writes to the same block?
                base = CrossTable[addr].BlockBase;
                size = CrossTable[addr].BlockSize;
                type = CrossTable[addr].Types;
                protocol = CrossTable[addr].Protocol;
                offset = CrossTable[addr].Offset;
                node = CrossTable[addr].node;

                for (n = 1; (addr + n) < (base + size) && (addr + n) <= addrMax; ++n)
                {
                    if (request.Number >= MAX_VALUES) {
                        break;
                    }
                    // must be same device and node (should already be checked by Crosstable editor)
                    if (CrossTable[addr + n].device != d || CrossTable[addr + n].node != node) {
                        break;
                    }
                    // in Modbus clients we cannot mix BIT and non BIT variables
                    if ((protocol == RTU || protocol == TCP || protocol == TCPRTU || protocol == RTU_SRV || protocol == TCP_SRV || protocol == TCPRTU_SRV)
                        && ((type == BIT && CrossTable[addr + n].Types != BIT)
                           || (type != BIT && CrossTable[addr + n].Types == BIT))) {
                        break;
                    }
                    if (flags) {
                        // only sequential writes, with the exception of *_BIT of the same offset
                        if (flags[addr + n] == 0
                        && !((type == BYTE_BIT || type == WORD_BIT || type == DWORD_BIT)
                           && type == CrossTable[addr + n].Types && offset == CrossTable[addr + n].Offset)) {
                            break;
                        }
                    }

                    // ok, add another one
                    request.Number = 1 + n;
                    if (flags && flags[addr + n] == 0) {
                        // in the exception of *_BIT, we get the actual value
                        request.Values[n] = VAR_VALUE(addr + n);
                    } else {
                        // normal case
                        if (CrossTable[addr + n].Types == BIT || CrossTable[addr + n].Types == BYTE_BIT
                         || CrossTable[addr + n].Types == WORD_BIT || CrossTable[addr + n].Types == DWORD_BIT ) {
                            // the engine uses only the bit#0 for bool variables
                            if (values[addr + n] & 1) {
                                request.Values[n] = 1;
                            } else {
                                request.Values[n] = 0;
                            }
                        } else {
                            request.Values[n] = values[addr + n];
                        }
                    }

                    // wrote another one
                    retval += 1;
                }
            }

            // (3 of 3) add the request to the circular buffer:
            //     -- avoiding duplicates
            //     -- losing the oldest if buffer full
            register int i, duplicate;

            for (i = theDevices[d].PLCwriteRequestGet, duplicate = FALSE;
                 i != theDevices[d].PLCwriteRequestPut;
                 i = ((i + 1) % MaxLocalQueue))
            {
                // searching for duplicates
                if (request.Addr == theDevices[d].PLCwriteRequests[i].Addr
                 && request.Number == theDevices[d].PLCwriteRequests[i].Number) {
                    register int n, all_equal = TRUE;

                    for (n = 0; n < request.Number; ++ n) {
                        if (theDevices[d].PLCwriteRequests[i].Values[n] != request.Values[n]) {
                            all_equal = FALSE;
                            break;
                        }
                    }
                    if (all_equal) {
                        duplicate = TRUE;
                        break;
                    }
                }
            }
            if (duplicate) {
                // duplicate: ignore the write operation
#ifdef VERBOSE_DEBUG
                fprintf(stderr, "%s(): warning: duplicate, writing addr=%u, number=%u, queue=%u to device=%u\n",
                     __func__, addr, request.Number, theDevices[d].PLCwriteRequestNumber, d);
#endif
            } else if (theDevices[d].PLCwriteRequestNumber >= MaxLocalQueue) {
                // buffer full: retry afterwords
                fprintf(stderr, "%s(): error: buffer full, writing addr=%u, number=%u, queue=%u to device=%u\n",
                     __func__, addr, request.Number, theDevices[d].PLCwriteRequestNumber, d);
                retval = 0;
            } else {
                register int n;

                // add new
                i = theDevices[d].PLCwriteRequestPut;
                theDevices[d].PLCwriteRequests[i].Addr = request.Addr;
                theDevices[d].PLCwriteRequests[i].Number = request.Number;
                for (n = 0; n < request.Number; ++ n) {
                    theDevices[d].PLCwriteRequests[i].Values[n] = request.Values[n];
                }
                theDevices[d].PLCwriteRequestPut = (i + 1) % MaxLocalQueue;
                theDevices[d].PLCwriteRequestNumber += 1;

                // add the write request of 'value'
                setDiagnostic(theDevices[d].diagnosticAddr, DIAGNOSTIC_WRITE_QUEUE, theDevices[d].PLCwriteRequestNumber);

#ifdef VERBOSE_DEBUG
                {
                    int z;
                    fprintf(stderr, "\tqueued %u values in slot #%d\n", retval, i);
                    for (z = 0; z < retval; ++z)
                        fprintf(stderr, "\t%02d 0x%08x\n", z, theDevices[d].PLCwriteRequests[i].Values[z]);
                }
#endif
                // awake the device thread
                sem_post(&newOperations[d]);
            }
        }
    }

    return retval;
}

/* ---------------------------------------------------------------------------- */

void writeQdataRegisters(u_int16_t addr, u_int32_t value, u_int8_t status)
{
    if (addr > DimCrossTable) {
        fprintf(stderr, "writeQdataRegisters(addr=%u, value=0x%08x, status=%u): wrong addr\n", addr, value,status);
        return;
    }

    if (addr == PLC_WATCHDOGEN) {
        if (value) {
            xx_watchdog_enable();
        } else {
            xx_watchdog_disable();
        }

    } else if (addr == PLC_WATCHDOG_ms) {
        xx_watchdog_reset(value);

    } else if (addr == PLC_FastIO_Ena) {
        register unsigned n;

        for (n = 0; n < XX_GPIO_MAX; ++n) {
            if (value & (1 << n)) {
                xx_gpio_enable(n);
            }
        }

    } else if (addr == PLC_FastIO_Dir) {
        register unsigned n;

        for (n = 0; n < XX_GPIO_MAX; ++n) {

            // NB filtered on enabled in xx_gpio_config
            if (value & (1 << n)) {
                xx_gpio_config(n, 1);
            } else {
                xx_gpio_config(n, 0);
            }
        }

    } else if (addr >= PLC_FastIO_1 && addr <= PLC_FastIO_32) {
        // NB filtered on enabled in xx_gpio_set/clr
        if (value) {
            xx_gpio_set(addr - PLC_FastIO_1);
        } else {
            xx_gpio_clr(addr - PLC_FastIO_1);
        }

    } else if (addr == PLC_BUZZER) {

        buzzer_beep_ms =  value & 0x000000FF;
        buzzer_on_cs   = (value & 0x0000FF00) >> 8;
        buzzer_off_cs  = (value & 0x00FF0000) >> 16;
        buzzer_replies = (value & 0xFF000000) >> 24;

        buzzer_period_tics = buzzer_on_cs + buzzer_off_cs;
        buzzer_tic = 0;

        xx_pwm3_disable();
        if (buzzer_on_cs > 0 && buzzer_beep_ms > 0 && buzzer_replies > 0) {
            buzzer_tic = 1;
            buzzer_periods = 1;
            xx_pwm3_set(buzzer_beep_ms);
            xx_pwm3_enable();
        } else {
            buzzer_tic = 0;
            buzzer_periods = 0;
        }

    } else {
        register unsigned n;

        for (n = 0; n < theNodesNumber; ++n) {
            if (addr == (theNodes[n].diagnosticAddr + DIAGNOSTIC_NODE_STATUS)) {
                if (value == 0) {
                    if (theNodes[n].status != NODE_DISABLED) {
                        // cannot use: changeNodeStatus(theNodes[n].device, n, NODE_DISABLED);
                        theNodes[n].status = NODE_DISABLED;
                        value = NODE_DISABLED;
                    } else {
                        value = theNodes[n].status;
                    }
                } else {
                    if (theNodes[n].status == NODE_DISABLED) {
                        // cannot use: changeNodeStatus(theNodes[n].device, n, NODE_OK);
                        theNodes[n].status = NODE_OK;
                        value = NODE_OK;
                    } else {
                        value = theNodes[n].status;
                    }
                }
                break;
            }
        }
    }

    switch (status) {

    case DATA_OK:
        // if the variable is used in alarms/events conditions
        // ... and the value changed or the state became OK (at boot-time, ...)
        if (CrossTable[addr].usedInAlarmsEvents
          && (value != VAR_VALUE(addr)|| VAR_STATE(addr) != DATA_OK)) {
            // change value and/or status ... and then re-check the alarms/events conditions
            VAR_VALUE(addr) = value;
            VAR_STATE(addr) = DATA_OK;
            pthread_cond_signal(&theAlarmsEventsCondvar);
        } else {
            // simply change value and status
            VAR_VALUE(addr) = value;
            VAR_STATE(addr) = DATA_OK;
        }
        break;

    case DATA_WARNING:
        // only status change, no value change yet
        VAR_STATE(addr) = DATA_WARNING;
        break;

    case DATA_ERROR:
        VAR_VALUE(addr) = value;
        VAR_STATE(addr) = DATA_ERROR;
        break;

    default:
        ;
    }

#if defined(RTS_CFG_MECT_RETAIN)
    // if the variable is a retentive one then also update the copy
    if (retentive && addr > 0 && addr <= LAST_RETENTIVE && status != DATA_WARNING) {
        retentive[addr -1] = value;
        do_flush_retentives = TRUE;
    }
#endif
}

/* ---------------------------------------------------------------------------- */
