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
 * Filename: mectCfgUtil.c
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "mectCfgUtil.h"

//#define DBG_MECT_UTIL

#define TAG_CONF_SYSTEM    "[SYSTEM]"
#define TAG_CONF_SERIAL_PORT_0 "[SERIAL_PORT_0]"
#define TAG_CONF_SERIAL_PORT_1 "[SERIAL_PORT_1]"
#define TAG_CONF_SERIAL_PORT_2 "[SERIAL_PORT_2]"
#define TAG_CONF_SERIAL_PORT_3 "[SERIAL_PORT_3]"
#define TAG_CONF_SERIAL_PORT_n "[SERIAL_PORT_n]"
#define TAG_CONF_TCP_IP_PORT   "[TCP_IP_PORT]"
#define TAG_CONF_CANOPEN_0 "[CANOPEN_0]"
#define TAG_CONF_CANOPEN_1 "[CANOPEN_1]"
#define TAG_CONF_CANOPEN_n "[CANOPEN_n]"

enum app_conf_section {
	APP_CONF_NONE = 0,
    APP_CONF_SYSTEM,
    APP_CONF_SERIAL_PORT_0,
    APP_CONF_SERIAL_PORT_1,
    APP_CONF_SERIAL_PORT_2,
    APP_CONF_SERIAL_PORT_3,
    APP_CONF_TCP_IP_PORT,
    APP_CONF_CANOPEN_0,
    APP_CONF_CANOPEN_1,
};

/**
 * Check whether the given line starts with the given property
 * name.  The property name is terminated by ' ', '\t', or '='.
 *
 * @param line      input line
 * @param prop      property name
 *
 * @return          NULL if does not match
 *                  pointer to end of property name in line if matches
 *
 * @ingroup config
 */
	static char *
app_property_name_check(char *line, const char *prop)
{
	unsigned pl = 0;

	assert(line != NULL);
	assert(prop != NULL);

	pl = strlen(prop);
	if ((strstr(line, prop) == line) && (strspn(&line[pl], " =\t") > 0))
		return &line[pl];
	else
		return NULL;
}

/**
 * Check whether the next non whitespace char is '=' in the given line
 *
 * @param fn        calling function name
 * @param line      the line to scan
 * @param ln        line number
 * @param cf        configuration file name
 *
 * @return          pointer to the start of the first identifier after '='
 *                  NULL on failure
 *
 * @ingroup config
 */
	static char *
app_expect_equal(const char *fn, char *line, unsigned ln, char *cf)
{
	assert(fn != NULL);
	assert(line != NULL);
	assert(ln > 0);
	assert(cf != NULL);

	line += strspn(line, " \t");
	if (*line != '=') {
		fprintf(stderr, "%s: expecting `=' after identifier on line %d in file %s\n", fn, ln, cf);

		return NULL;
	}
	else
		line++;
	line += strspn(line, " \t");

	return line;
}

int get_u_int16(char *ptr, const char *id, u_int16_t *value, unsigned line_num, const char *filename)
{
    ptr = app_property_name_check(ptr, id);
    if (ptr == NULL) {
        return 0;
    }
    ptr = app_expect_equal(__func__, ptr, line_num, APP_CONFIG_FILE);
    if (ptr == NULL) {
        return -1;
    }
    *value = atoi(ptr);
    return 1;
}

int get_u_int32(char *ptr, const char *id, u_int32_t *value, unsigned line_num, const char *filename)
{
    ptr = app_property_name_check(ptr, id);
    if (ptr == NULL) {
        return 0;
    }
    ptr = app_expect_equal(__func__, ptr, line_num, APP_CONFIG_FILE);
    if (ptr == NULL) {
        return -1;
    }
    *value = atoi(ptr);
    return 1;
}

int get_char(char *ptr, const char *id, char *value, unsigned line_num, const char *filename)
{
    ptr = app_property_name_check(ptr, id);
    if (ptr == NULL) {
        return 0;
    }
    ptr = app_expect_equal(__func__, ptr, line_num, APP_CONFIG_FILE);
    if (ptr == NULL) {
        return -1;
    }
    *value = *ptr;
    return 1;
}

int get_name(char *ptr, const char *id, char *name, unsigned line_num, const char *filename)
{
    ptr = app_property_name_check(ptr, id);
    if (ptr == NULL) {
        return 0;
    }
    ptr = app_expect_equal(__func__, ptr, line_num, APP_CONFIG_FILE);
    if (ptr == NULL) {
        return -1;
    }
    strncpy(name, ptr, MAX_NAMELEN);
    name[MAX_NAMELEN - 1] = 0;
    return 1;
}

int get_system_conf(char *ptr, struct system_conf *system, unsigned line_num, const char *filename)
{
    int r;

    r = get_u_int16(ptr, "retries", &system->retries, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "blacklist", &system->blacklist, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "read_period_ms_1", &system->read_period_ms[0], line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "read_period_ms_2", &system->read_period_ms[1], line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "read_period_ms_3", &system->read_period_ms[2], line_num, filename);
    if (r) { return r; }
    r = get_name(ptr, "home_page", system->home_page, line_num, filename);
    if (r) { return r; }
    r = get_name(ptr, "start_page", system->start_page, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "buzzer_touch", &system->buzzer_touch, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "buzzer_alarm", &system->buzzer_alarm, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "pwd_timeout_s", &system->pwd_timeout_s, line_num, filename);
    if (r) { return r; }
    r = get_name(ptr, "pwd_logout_page", system->pwd_logout_page, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "screen_saver_s",&system->screen_saver_s, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "slow_log_period_s", &system->slow_log_period_s, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "fast_log_period_s", &system->fast_log_period_s, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "max_log_space_MB", &system->max_log_space_MB, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "trace_window_s", &system->trace_window_s, line_num, filename);
    if (r) { return r; }
    if (ptr != NULL) {
        fprintf(stderr, "%s: unknown property %s for %s\n", __func__, ptr, TAG_CONF_SYSTEM);
        return -1;
    }
    return 0;
}

int get_serial_conf(char *ptr, struct serial_conf *serial, unsigned line_num, const char *filename)
{
    int r;

    r = get_u_int32(ptr, "baudrate", &serial->baudrate, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "databits", &serial->databits, line_num, filename);
    if (r) { return r; }
    r = get_char(ptr, "parity", &serial->parity, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "stopbits", &serial->stopbits, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "silence_ms", &serial->silence_ms, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "timeout_ms", &serial->timeout_ms, line_num, filename);
    if (r) { return r; }
    if (ptr != NULL) {
        fprintf(stderr, "%s: unknown property %s for %s\n", __func__, ptr, TAG_CONF_SERIAL_PORT_n);
        return -1;
    }
    return 0;
}

int get_tcp_ip_conf(char *ptr, struct tcp_ip_conf *tcp_ip, unsigned line_num, const char *filename)
{
    int r;

    r = get_u_int16(ptr, "silence_ms", &tcp_ip->silence_ms, line_num, filename);
    if (r) { return r; }
    r = get_u_int16(ptr, "timeout_ms", &tcp_ip->timeout_ms, line_num, filename);
    if (r) { return r; }
    if (ptr != NULL) {
        fprintf(stderr, "%s: unknown property %s for %s\n", __func__, ptr, TAG_CONF_TCP_IP_PORT);
        return -1;
    }
    return 0;
}

int get_canopen_conf(char *ptr, struct canopen_conf *canopen, unsigned line_num, const char *filename)
{
    int r;

    r = get_u_int32(ptr, "baudrate", &canopen->baudrate, line_num, filename);
    if (r) { return r; }
    if (ptr != NULL) {
        fprintf(stderr, "%s: unknown property %s for %s\n", __func__, ptr, TAG_CONF_CANOPEN_n);
        return -1;
    }
    return 0;
}

/**
 * Load the application configuration settings from the
 * configuration file
 *
 * @return      0 for success; non-0 code for error
 *
 * @ingroup config
 */
int app_config_load(struct system_ini *system_ini)
{

	char line[MAX_LINE_SIZE];
	FILE *cf = NULL;
	unsigned line_num = 0;
    enum app_conf_section actual_section = APP_CONF_NONE;

	/* reset value */
    if (system_ini == NULL) {
        return 1;
    }
    bzero(system_ini, sizeof(struct system_ini));

	cf = fopen(APP_CONFIG_FILE, "r");
	if (cf == NULL) {
		perror(APP_CONFIG_FILE);
		return 1;
	}

	for (line_num = 1; fgets(line, MAX_LINE_SIZE, cf) != NULL; ++line_num) {
		char *ptr = line;
		unsigned line_len =  strlen(line);
        if (line[line_len - 1] == '\n') {
			line[line_len - 1] = '\0';
        }
		ptr += strspn(ptr, " \t");  // skip blanks
		if (*ptr == '[') {  // section start
			if (strstr(ptr, TAG_CONF_SYSTEM) != NULL)
				actual_section = APP_CONF_SYSTEM;
			else if (strstr(ptr, TAG_CONF_SERIAL_PORT_0) != NULL)
				actual_section = APP_CONF_SERIAL_PORT_0;
			else if (strstr(ptr, TAG_CONF_SERIAL_PORT_1) != NULL)
				actual_section = APP_CONF_SERIAL_PORT_1;
			else if (strstr(ptr, TAG_CONF_SERIAL_PORT_2) != NULL)
				actual_section = APP_CONF_SERIAL_PORT_2;
			else if (strstr(ptr, TAG_CONF_SERIAL_PORT_3) != NULL)
				actual_section = APP_CONF_SERIAL_PORT_3;
			else if (strstr(ptr, TAG_CONF_TCP_IP_PORT) != NULL)
				actual_section = APP_CONF_TCP_IP_PORT;
            else if (strstr(ptr, TAG_CONF_CANOPEN_0) != NULL)
                actual_section = APP_CONF_CANOPEN_0;
            else if (strstr(ptr, TAG_CONF_CANOPEN_1) != NULL)
                actual_section = APP_CONF_CANOPEN_1;
			else {
				fprintf(stderr, "%s: Unknown section '%s' on line nb. %d in file '%s'\n", __func__, ptr, line_num, APP_CONFIG_FILE);
				actual_section = APP_CONF_NONE;
			}
		}
		else {
			int r = 0;

			switch (actual_section) {
            case APP_CONF_NONE:
                break;
            case APP_CONF_SYSTEM:
                r = get_system_conf(ptr, &system_ini->system, line_num, APP_CONFIG_FILE);
                break;
            case APP_CONF_SERIAL_PORT_0:
                r = get_serial_conf(ptr, &system_ini->serial_port[0], line_num, APP_CONFIG_FILE);
                break;
            case APP_CONF_SERIAL_PORT_1:
                r = get_serial_conf(ptr, &system_ini->serial_port[1], line_num, APP_CONFIG_FILE);
                break;
            case APP_CONF_SERIAL_PORT_2:
                r = get_serial_conf(ptr, &system_ini->serial_port[2], line_num, APP_CONFIG_FILE);
                break;
            case APP_CONF_SERIAL_PORT_3:
                r = get_serial_conf(ptr, &system_ini->serial_port[3], line_num, APP_CONFIG_FILE);
                break;
            case APP_CONF_TCP_IP_PORT:
                r = get_tcp_ip_conf(ptr, &system_ini->tcp_ip_port, line_num, APP_CONFIG_FILE);
                break;
            case APP_CONF_CANOPEN_0:
                r = get_canopen_conf(ptr, &system_ini->canopen[0], line_num, APP_CONFIG_FILE);
                break;
            case APP_CONF_CANOPEN_1:
                r = get_canopen_conf(ptr, &system_ini->canopen[1], line_num, APP_CONFIG_FILE);
                break;
            default:
                ;
            }
            if (r < 0) {
               fclose(cf);
               return 1;
            }
		}
	}
	fclose(cf);
	return 0;
}

void app_config_dump(struct system_ini *system_ini)
{
    int n;

    fprintf(stderr, "%s\n", TAG_CONF_SYSTEM);
    fprintf(stderr, "retries = %u\n", system_ini->system.retries);
    fprintf(stderr, "blacklist = %u\n", system_ini->system.blacklist);
    fprintf(stderr, "read_period_ms_1 = %u\n", system_ini->system.read_period_ms[0]);
    fprintf(stderr, "read_period_ms_2 = %u\n", system_ini->system.read_period_ms[1]);
    fprintf(stderr, "read_period_ms_3 = %u\n", system_ini->system.read_period_ms[2]);
    fprintf(stderr, "home_page = %s\n", system_ini->system.home_page);
    fprintf(stderr, "start_page = %s\n", system_ini->system.start_page);
    fprintf(stderr, "buzzer_touch = %u\n", system_ini->system.buzzer_touch);
    fprintf(stderr, "buzzer_alarm = %u\n", system_ini->system.buzzer_alarm);
    fprintf(stderr, "pwd_timeout_s = %u\n", system_ini->system.pwd_timeout_s);
    fprintf(stderr, "pwd_logout_page = %s\n", system_ini->system.pwd_logout_page);
    fprintf(stderr, "screen_saver_s = %u\n", system_ini->system.screen_saver_s);
    fprintf(stderr, "slow_log_period_s = %u\n", system_ini->system.slow_log_period_s);
    fprintf(stderr, "fast_log_period_s = %u\n", system_ini->system.fast_log_period_s);
    fprintf(stderr, "max_log_space_MB = %u\n", system_ini->system.max_log_space_MB);
    fprintf(stderr, "trace_window_s = %u\n", system_ini->system.trace_window_s);

    for (n = 0; n < MAX_SERIAL_PORT; ++n) {
        fprintf(stderr, "%s %d\n", TAG_CONF_SERIAL_PORT_n, n);
        fprintf(stderr, "baudrate = %u\n", system_ini->serial_port[n].baudrate);
        fprintf(stderr, "databits = %u\n", system_ini->serial_port[n].databits);
        fprintf(stderr, "parity = %c\n", system_ini->serial_port[n].parity);
        fprintf(stderr, "stopbits = %u\n", system_ini->serial_port[n].stopbits);
        fprintf(stderr, "silence_ms = %u\n", system_ini->serial_port[n].silence_ms);
        fprintf(stderr, "timeout_ms = %u\n", system_ini->serial_port[n].timeout_ms);
    }

    fprintf(stderr, "%s\n", TAG_CONF_TCP_IP_PORT);
    fprintf(stderr, "silence_ms = %u\n", system_ini->tcp_ip_port.silence_ms);
    fprintf(stderr, "timeout_ms = %u\n", system_ini->tcp_ip_port.timeout_ms);

    for (n = 0; n < MAX_CANOPEN; ++n) {
        fprintf(stderr, "%s %d\n", TAG_CONF_CANOPEN_n, n);
        fprintf(stderr, "baudrate = %u\n", system_ini->serial_port[n].baudrate);
    }
}
