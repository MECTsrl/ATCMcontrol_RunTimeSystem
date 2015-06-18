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
 * Filename: privUtyMect.h
 */

#ifndef _PRIV_UTY_MECT_H_
#define _PRIV_UTY_MECT_H_

#include "mectCfgUtil.h"

// This is for the EVA BOARD
//#define SERIALPORT      "/dev/ttySP0"   /* Serial port to use */
// This is for the VAL01 BOARD
#define SERIALPORT      "/dev/ttySP3"   /* Serial port to use */
#define BAUDRATE        B9600           /* Serial communication baud rate */

/*
 * Function prototypes
 */

int app_s_serial_init(serial_cfg_s * config);
void app_s_serial_close(int fdsio);
int app_s_str_to_int(char *string);
void app_s_int_to_hex(unsigned int integer, char *hex);
void app_s_float_to_str(float absval, char *dst);
float app_s_str_to_float(char *str);
void app_s_rx_timeout_set(void);
void app_s_txrx_timeout_reset(void);
int app_s_txrx_timeout_check(void);

#endif

