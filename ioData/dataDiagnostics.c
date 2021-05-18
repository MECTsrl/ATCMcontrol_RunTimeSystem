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
 * Filename: dataDiagnostics.c
 */

#include "dataImpl.h"

#define __4CFILE__	"dataDiagnostics.c"

/* ---------------------------------------------------------------------------- */

void initServerDiagnostic(u_int16_t s)
{
    u_int16_t addr = 0;

    switch (theServers[s].protocol) {
    case RTU_SRV:
        switch (theServers[s].port) {
        case 0: addr = 5000; break;
        case 1: addr = 5010; break;
        case 3: addr = 5020; break;
        default: ;
        }
        break;
    case TCP_SRV:
        switch (theServers[s].port) {
        case 502: addr = 5050; break;
        default: ;
        }
        break;
    case TCPRTU_SRV:
        break;
    default:
        ;
    }
    theServers[s].diagnosticAddr = addr;
    if (addr) {
        u_int32_t value;

        value = (theServers[s].protocol << 16) + theServers[s].port;
        writeQdataRegisters(addr + DIAGNOSTIC_TYPE_PORT, value, DATA_OK);

        switch (theServers[s].protocol) {
        case RTU_SRV:
            value = theServers[s].u.serial.baudrate;
            break;
        case TCP_SRV:
            value = theServers[s].u.tcp_ip.IPaddr;
            break;
        case TCPRTU_SRV:
            break;
        default:
            value = 0;
        }
        writeQdataRegisters(addr + DIAGNOSTIC_BAUDRATE, value, DATA_OK); // IP_ADDRESS/BAUDRATE
    }
}

void initDeviceDiagnostic(u_int16_t d)
{
    u_int16_t addr = 0;

    switch (theDevices[d].protocol) {
    case RTU:
    case MECT:
        switch (theDevices[d].port) {
        case 0: addr = 5000; break;
        case 2: addr = 5010; break;
        case 3: addr = 5020; break;
        default: ;
        }
        break;
    case RTU_SRV:
        break;
    case CANOPEN:
        switch (theDevices[d].port) {
        case 0: addr = 5030; break;
        case 1: addr = 5040; break;
        default: ;
        }
        break;
    case TCP_SRV:
        break;
    case TCP:
        switch (theDevices[d].port) {
        case 502:
            if (theTcpDevicesNumber < 10) {
                addr = 5060 + 10 * (theTcpDevicesNumber - 1);
            }
            break;
        default: ;
        }
        break;
    default:
        ;
    }
    theDevices[d].diagnosticAddr = addr;
    if (addr) {
        u_int32_t value;

        value = (theDevices[d].protocol << 16) + theDevices[d].port;
        writeQdataRegisters(addr + DIAGNOSTIC_TYPE_PORT, value, DATA_OK);

        switch (theDevices[d].protocol) {
        case RTU:
        case MECT:
            value = theDevices[d].u.serial.baudrate;
            break;
        case CANOPEN:
            value = theDevices[d].u.serial.baudrate; // FIXME: use real baudrate
            break;
        case TCP:
            value = theDevices[d].u.tcp_ip.IPaddr;
            break;
        default:
            value = 0;
        }
        writeQdataRegisters(addr + DIAGNOSTIC_BAUDRATE, value, DATA_OK); // IP_ADDRESS/BAUDRATE
    }
}

void setDiagnostic(u_int16_t addr, u_int16_t offset, u_int32_t value)
{
    if (addr == 0 || addr + offset > DimCrossTable) {
        return;
    }
    addr += offset;
    writeQdataRegisters(addr, value, DATA_OK);
}

void incDiagnostic(u_int16_t addr, u_int16_t offset)
{
    if (addr == 0 || addr + offset > DimCrossTable) {
        return;
    }
    addr += offset;
    writeQdataRegisters(addr, VAR_VALUE(addr) + 1, DATA_OK);
}

/* ---------------------------------------------------------------------------- */
