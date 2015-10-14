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
 * Filename: mectCfgUtil.h
 */

#ifndef H_MECTCFGUTIL
#define H_MECTCFGUTIL

#include "stdInc.h"

#define APP_CONFIG_DIR                  "/local/etc/sysconfig/"
#define APP_CONFIG_FILE                 APP_CONFIG_DIR "application.conf"

#define APP_CONFIG_BUILD_ID_FILE        APP_CONFIG_DIR ".fw_versions"
#define APP_CONFIG_MAC_FILE             "/etc/mac.conf"
#define APP_CONFIG_SERIAL_FILE          "/etc/serial.conf"
#define APP_CONFIG_IPADDR_FILE          APP_CONFIG_DIR "net.info"

#define MAX_LINE_SIZE       81

/**
 *
 * Local Defines
 *
 */
typedef struct serial_cfg_s {
	int enabled;            /* Is enabled? */
	int baud;               /* Baud rate configuration*/
	int databits;           /* Databits bit configuration*/
	int parity;             /* Parity bit configuration*/
	int stopbits;           /* Stop bit configuration*/
	int ignore_echo;        /* if 1, ignore the command echo */
} serial_cfg_s;

typedef struct mbrtu_cfg_s {      /* MODBUS stack settings */
	serial_cfg_s serial_cfg; 
	int ascii;              /* Use ASCII protocol */
	int rtu;                /* Use RTU protocol */
} mbrtu_cfg_s;

#ifdef RTS_CFG_MECT_LIB
extern serial_cfg_s mect_cfg;
#endif
#ifdef RTS_CFG_IOCANOPEN
extern serial_cfg_s can0_cfg;
extern serial_cfg_s can1_cfg;
#endif
#ifdef RTS_CFG_IOMBRTUC
extern mbrtu_cfg_s modbus0_cfg;
extern mbrtu_cfg_s modbus1_cfg;
#endif

enum app_conf_section_e {
	APP_CONF_NONE = 0,
	APP_CONF_IRQ0,              /* IRQ0 section */
	APP_CONF_IRQ1,              /* IRQ1 section */
	APP_CONF_PLC0,              /* PLC0 section */
	APP_CONF_PLC1,              /* PLC1 section */
	APP_CONF_PLC2,              /* PLC2 section */
	APP_CONF_ST,                /* MECT serial test protocol (slave) section */
	APP_CONF_MECT,              /* MECT serial protocol (master) section */
    APP_CONF_MB,                /* MODBUS protocol section */
    APP_CONF_MB0,               /* MODBUS0 protocol section */
    APP_CONF_MB1,               /* MODBUS1 protocol section */
    APP_CONF_WD,                /* Watch dog section */
	APP_CONF_CAN0,              /* Can0 section */
	APP_CONF_CAN1,              /* Can1 section */
	APP_CONF_LAST_ELEM          /* PLC2 section */
};

#define TAG_CONF_IRQ0 "[IRQ0]"
#define TAG_CONF_IRQ1 "[IRQ1]"
#define TAG_CONF_PLC0 "[PLC0]"
#define TAG_CONF_PLC1 "[PLC1]"
#define TAG_CONF_PLC2 "[PLC2]"
#define TAG_CONF_ST   "[ST]"
#define TAG_CONF_MECT "[MECT]"
#define TAG_CONF_MB   "[MODBUS]"
#define TAG_CONF_MB0  "[MODBUS0]"
#define TAG_CONF_MB1  "[MODBUS1]"
#define TAG_CONF_WD   "[WD]"
#define TAG_CONF_CAN0 "[CAN0]"
#define TAG_CONF_CAN1 "[CAN1]"

int app_config_load(enum app_conf_section_e section);

#endif

