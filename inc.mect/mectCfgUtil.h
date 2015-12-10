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
#define APP_CONFIG_FILE                 APP_CONFIG_DIR "system.ini"

#define MAX_LINE_SIZE   81
#define MAX_SERIAL_PORT  4
#define MAX_CANOPEN      2
#define MAX_NAMELEN     17
#define MAX_PRIORITY     3

/**
 *
 * Local Defines
 *
 */
struct serial_conf {
	u_int32_t baudrate;     // baudrate = 38400
	u_int16_t databits;      // databits = 8
	char parity;            // parity = N
	u_int16_t stopbits;     // stopbits = 1
    u_int16_t silence_ms;   // silence_ms = 30
    u_int16_t timeout_ms;   // timeout_ms = 130
};

struct tcp_ip_conf {
    u_int16_t silence_ms;   // silence_ms = 30
    u_int16_t timeout_ms;   // timeout_ms = 130
};

struct canopen_conf {
    u_int32_t baudrate;     // baudrate = 125000
};

struct system_conf {
    u_int16_t retries;                      // retries = 5
    u_int16_t blacklist;                    // blacklist = 10
    u_int16_t read_period_ms[MAX_PRIORITY]; // read_period_ms_1 = 10 read_period_ms_2 = 100 read_period_ms_3 = 1000
    char home_page[MAX_NAMELEN];            // home_page = page100
    char start_page[MAX_NAMELEN];           // start_page = page100
    u_int16_t buzzer_touch;                 // buzzer_touch = 1
    u_int16_t buzzer_alarm;                 // buzzer_alarm = 1
    u_int16_t pwd_timeout_s;                // pwd_timeout_s = 0
    char pwd_logout_page[MAX_NAMELEN];      // pwd_logout_page =
    u_int16_t screen_saver_s;               // screen_saver_s = 0
    u_int16_t slow_log_period_s;            // slow_log_period_ms = 10
    u_int16_t fast_log_period_s;            // fast_log_period_s = 1
    u_int16_t max_log_space_MB;             // max_log_space_MB = 5
    u_int16_t trace_window_s;               // trace_window_s = 60
};

struct system_ini {
    struct system_conf system;
    struct serial_conf serial_port[MAX_SERIAL_PORT];
    struct tcp_ip_conf tcp_ip_port;
    struct canopen_conf canopen[MAX_CANOPEN];
};

int app_config_load(struct system_ini * system_ini);
void app_config_dump(struct system_ini * system_ini);

#endif

