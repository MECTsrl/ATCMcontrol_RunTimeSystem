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
app_property_name_check(char *line, char *prop)
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

/**
 * Load the application configuration settings from the
 * configuration file
 *
 * @return      0 for success; non-0 code for error
 *
 * @ingroup config
 */
int app_config_load(enum app_conf_section_e section)
{

	char line[MAX_LINE_SIZE];
	FILE *cf = NULL;
	unsigned actual_section = APP_CONF_NONE;
	unsigned ln = 0;

	/* reset value */
	switch (section) {
		case APP_CONF_IRQ0:
			/* Do nothing */
			break;
		case APP_CONF_IRQ1:
			/* Do nothing */
			break;
		case APP_CONF_PLC0:
			/* Do nothing */
			break;
		case APP_CONF_PLC1:
			/* Do nothing */
			break;
		case APP_CONF_PLC2:
			/* Do nothing */
			break;
		case APP_CONF_ST:
			/* Do nothing */
			break;
		case APP_CONF_MECT:
#ifdef RTS_CFG_MECT_LIB
			memset (&mect_cfg, 0xFF, sizeof(mect_cfg));
			mect_cfg.enabled = 0;
#endif
			break;
		case APP_CONF_MB:
			break;
    case APP_CONF_MB0:
#ifdef RTS_CFG_IOMBRTUC
        memset (&modbus0_cfg, 0xFF, sizeof(modbus0_cfg));
        modbus0_cfg.serial_cfg.enabled = 0;
#endif
        break;
    case APP_CONF_MB1:
#ifdef RTS_CFG_IOMBRTUC
        memset (&modbus1_cfg, 0xFF, sizeof(modbus1_cfg));
        modbus1_cfg.serial_cfg.enabled = 0;
#endif
        break;

		case APP_CONF_WD:
			/* Do nothing */
			break;
		case APP_CONF_CAN0:
#ifdef RTS_CFG_IOCANOPEN
			memset (&can0_cfg, 0xFF, sizeof(can0_cfg));
			can0_cfg.enabled = 0;
#endif
			break;
		case APP_CONF_CAN1:
#ifdef RTS_CFG_IOCANOPEN
			memset (&can1_cfg, 0xFF , sizeof(can1_cfg));
			can1_cfg.enabled = 0;
#endif
			break;
		case APP_CONF_NONE:
			/* Do nothing */
			break;

		default:
			fprintf(stderr, "%s: Unknown section %d in file %s\n", __func__, actual_section, APP_CONFIG_FILE);

			return 100;

			break;
	}

	cf = fopen(APP_CONFIG_FILE, "r");
	if (cf == NULL) {
		perror(APP_CONFIG_FILE);

		return 1;
	}

	for (ln = 1; fgets(line, MAX_LINE_SIZE, cf) == line; ln++) {
		char *l = NULL;
		unsigned ll = 0;

		ll = strlen(line);

		if (line[ll - 1] == '\n')
			line[ll - 1] = '\0';

		l = line;

		l += strspn(l, " \t");      /* Skip white space */

		if (*l == '[') {            /* Category start */

			if (strstr(l, TAG_CONF_IRQ0) == l)
				actual_section = APP_CONF_IRQ0;
			else if (strstr(l, TAG_CONF_IRQ1) == l)
				actual_section = APP_CONF_IRQ1;
			else if (strstr(l, TAG_CONF_PLC0) == l)
				actual_section = APP_CONF_PLC0;
			else if (strstr(l, TAG_CONF_PLC1) == l)
				actual_section = APP_CONF_PLC1;
			else if (strstr(l, TAG_CONF_PLC2) == l)
				actual_section = APP_CONF_PLC2;
			else if (strstr(l, TAG_CONF_ST) == l)
				actual_section = APP_CONF_ST;
			else if (strstr(l, TAG_CONF_MECT) == l)
				actual_section = APP_CONF_MECT;
            else if (strstr(l, TAG_CONF_MB) == l)
                actual_section = APP_CONF_MB;
            else if (strstr(l, TAG_CONF_MB0) == l)
                actual_section = APP_CONF_MB0;
            else if (strstr(l, TAG_CONF_MB1) == l)
                actual_section = APP_CONF_MB1;
            else if (strstr(l, TAG_CONF_WD) == l)
				actual_section = APP_CONF_WD;
			else if (strstr(l, TAG_CONF_CAN0) == l)
				actual_section = APP_CONF_CAN0;
			else if (strstr(l, TAG_CONF_CAN1) == l)
				actual_section = APP_CONF_CAN1;
			else
			{
				fprintf(stderr, "%s: Unknown section '%s' on line nb. %d in file '%s'\n", __func__, l, ln, APP_CONFIG_FILE);
				actual_section = APP_CONF_NONE;
			}
		}
		else {
			char *tl = NULL;

			switch (actual_section) {
				case APP_CONF_IRQ0:
					/* Do nothing */
					break;
				case APP_CONF_IRQ1:
					/* Do nothing */
					break;
				case APP_CONF_PLC0:
					/* Do nothing */
					break;
				case APP_CONF_PLC1:
					/* Do nothing */
					break;
				case APP_CONF_PLC2:
					/* Do nothing */
					break;
				case APP_CONF_ST:
					/* Do nothing */
					break;
				case APP_CONF_MECT:
#ifdef RTS_CFG_MECT_LIB
					if (section == APP_CONF_MECT)
					{
						if ((tl = app_property_name_check(l, "enabled"))) {
							l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
							if (l != NULL)
							{
								mect_cfg.enabled = atoi(l);
#ifdef DBG_MECT_UTIL
								fprintf(stderr, "%s: Mect protocol status '%d'\n", __func__, mect_cfg.enabled);
#endif
							}
							else
							{
								fclose(cf);
								return 42;
							}
						}

						else if ((tl = app_property_name_check(l, "baud"))) {
							l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
							if (l != NULL)
								mect_cfg.baud = atoi(l);
							else
							{
								fclose(cf);
								return 66;
							}
						}
						else if ((tl = app_property_name_check(l, "databits"))) {
							l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
							if (l != NULL)
								mect_cfg.databits = atoi(l);
							else
							{
								fclose(cf);
								return 67;
							}
						}
						else if ((tl = app_property_name_check(l, "parity"))) {
							l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
							if (l != NULL)
								mect_cfg.parity = atoi(l);
							else
							{
								fclose(cf);
								return 68;
							}
						}
						else if ((tl = app_property_name_check(l, "stopbits"))) {
							l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
							if (l != NULL)
								mect_cfg.stopbits = atoi(l);
							else
							{
								fclose(cf);
								return 69;
							}
						}
						else if ((tl = app_property_name_check(l, "ignore_echo"))) {
							l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
							if (l != NULL)
								mect_cfg.ignore_echo = atoi(l);
							else
							{
								fclose(cf);
								return 70;
							}
						}
						else if (tl != NULL) {
							fprintf(stderr, "%s: unknown property %s for serial\n", __func__, tl);

							fclose(cf);
							return 45;
						}
	
					}
#endif
					break;

				case APP_CONF_MB:

					break;

            case APP_CONF_MB0:
#ifdef RTS_CFG_IOMBRTUC
                if (section == APP_CONF_MB0)
                {
                    if ((tl = app_property_name_check(l, "enabled"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                        {
                            modbus0_cfg.serial_cfg.enabled = atoi(l);
                        }
                        else
                        {
                            fclose(cf);
                            return 43;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "ascii"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus0_cfg.ascii = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 44;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "rtu"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus0_cfg.rtu = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 65;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "baud"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus0_cfg.serial_cfg.baud = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 66;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "databits"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus0_cfg.serial_cfg.databits = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 67;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "parity"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus0_cfg.serial_cfg.parity = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 68;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "stopbits"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus0_cfg.serial_cfg.stopbits = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 69;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "ignore_echo"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus0_cfg.serial_cfg.ignore_echo = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 70;
                        }
                    }
                    else if (tl != NULL) {
                        fprintf(stderr, "%s: unknown property %s for MB\n", __func__, tl);

                        fclose(cf);
                        return 45;
                    }
                }
#endif
                break;

            case APP_CONF_MB1:
#ifdef RTS_CFG_IOMBRTUC
                if (section == APP_CONF_MB1)
                {
                    if ((tl = app_property_name_check(l, "enabled"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                        {
                            modbus1_cfg.serial_cfg.enabled = atoi(l);
                        }
                        else
                        {
                            fclose(cf);
                            return 43;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "ascii"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus1_cfg.ascii = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 44;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "rtu"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus1_cfg.rtu = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 65;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "baud"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus1_cfg.serial_cfg.baud = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 66;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "databits"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus1_cfg.serial_cfg.databits = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 67;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "parity"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus1_cfg.serial_cfg.parity = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 68;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "stopbits"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus1_cfg.serial_cfg.stopbits = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 69;
                        }
                    }
                    else if ((tl = app_property_name_check(l, "ignore_echo"))) {
                        l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
                        if (l != NULL)
                            modbus1_cfg.serial_cfg.ignore_echo = atoi(l);
                        else
                        {
                            fclose(cf);
                            return 70;
                        }
                    }
                    else if (tl != NULL) {
                        fprintf(stderr, "%s: unknown property %s for MB\n", __func__, tl);

                        fclose(cf);
                        return 45;
                    }
                }
#endif
                break;

				case APP_CONF_WD:
					/* Do nothing */
					break;
				case APP_CONF_CAN0:
#ifdef RTS_CFG_IOCANOPEN
					if (section == APP_CONF_CAN0)
					{
						if ((tl = app_property_name_check(l, "enabled"))) {
							l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
							if (l != NULL)
							{
								can0_cfg.enabled = atoi(l);
#ifdef DBG_MECT_UTIL
								fprintf(stderr, "%s: CAN0 protocol status '%d'\n", __func__, can0_cfg.enabled);
#endif
							}
							else
							{
								fclose(cf);
								return 46;
							}
						}
					}
#endif
					break;
				case APP_CONF_CAN1:
#ifdef RTS_CFG_IOCANOPEN
					if (section == APP_CONF_CAN1)
					{
						if ((tl = app_property_name_check(l, "enabled"))) {
							l = app_expect_equal(__func__, tl, ln, APP_CONFIG_FILE);
							if (l != NULL)
							{
								can1_cfg.enabled = atoi(l);
#ifdef DBG_MECT_UTIL
								fprintf(stderr, "%s: CAN1 protocol status '%d'\n", __func__, can1_cfg.enabled);
#endif
							}
							else
							{
								fclose(cf);
								return 46;
							}
						}
					}
#endif
					break;
				case APP_CONF_NONE:
					/* Do nothing */
					break;

				default:
					fprintf(stderr, "%s: Unknown section %d in file %s\n", __func__, actual_section, APP_CONFIG_FILE);

					fclose(cf);
					return 48;

					break;
			}
		}
	}
	fclose(cf);

	return 0;
}

