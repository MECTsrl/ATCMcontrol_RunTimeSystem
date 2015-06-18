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
 * Filename: privUtyMect.c
 */

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "privUtyMect.h"

static struct termios tty;              /* Serial port settings */
static struct termios oldtty;           /* Serial port old settings */

/*
 * Constant declaration
 */

#define RXTIMEOUT       400000  /* Command read timeout (usec) */
//#define DBG_PRIV_UTY_MECT

/**
 * Initialize the serial port
 *
 * @return      serial port file descriptor or -1 for failure
 *
 * @ingroup s_util
 */
/*
 * Baud rate:      9600
 * Start:          1 bit
 * Data:           8 bit
 * Stop:           1 bit
 * Parity:         No
 */
int app_s_serial_init(serial_cfg_s * config)
{
	int fd = 0;

	if (config->enabled == 0)
	{
		return 0;
	}

#ifdef DBG_PRIV_UTY_MECT
	printf("%s - Opening serial port '%s'\n", __func__, SERIALPORT);
#endif
	fd = open(SERIALPORT, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd <= 0)
	{
		printf("%s - %s\n", __func__, strerror(errno));
		return fd;
	}

	/* Set serial port parameters */
	memset(&tty, 0, sizeof(tty));

#ifdef DBG_PRIV_UTY_MECT
	printf("%s - setting baudrate %d\n", __func__, config->baud);
#endif
	switch (config->baud) {
		case 0:
			tty.c_cflag |= B0;
			break;
		case 50:
			tty.c_cflag |= B50;
			break;
		case 75:
			tty.c_cflag |= B75;
			break;
		case 110:
			tty.c_cflag |= B110;
			break;
		case 134:
			tty.c_cflag |= B134;
			break;
		case 150:
			tty.c_cflag |= B150;
			break;
		case 200:
			tty.c_cflag |= B200;
			break;
		case 300:
			tty.c_cflag |= B300;
			break;
		case 600:
			tty.c_cflag |= B600;
			break;
		case 1200:
			tty.c_cflag |= B1200;
			break;
		case 1800:
			tty.c_cflag |= B1800;
			break;
		case 2400:
			tty.c_cflag |= B2400;
			break;
		case 4800:
			tty.c_cflag |= B4800;
			break;
		case 9600:
			tty.c_cflag |= B9600;
			break;
		case 19200:
			tty.c_cflag |= B19200;
			break;
		case 38400:
			tty.c_cflag |= B38400;
			break;
		case 57600:
			tty.c_cflag |= B57600;
			break;
		case 115200:
			tty.c_cflag |= B115200;
			break;
		case 230400:
			tty.c_cflag |= B230400;
			break;
		case 460800:
			tty.c_cflag |= B460800;
			break;
		case 500000:
			tty.c_cflag |= B500000;
			break;
		case 576000:
			tty.c_cflag |= B576000;
			break;
		case 921600:
			tty.c_cflag |= B921600;
			break;
		case 1000000:
			tty.c_cflag |= B1000000;
			break;
		case 1152000:
			tty.c_cflag |= B1152000;
			break;
		case 1500000:
			tty.c_cflag |= B1500000;
			break;
		case 2000000:
			tty.c_cflag |= B2000000;
			break;
		case 2500000:
			tty.c_cflag |= B2500000;
			break;
		case 3000000:
			tty.c_cflag |= B3000000;
			break;
		case 3500000:
			tty.c_cflag |= B3500000;
			break;
		case 4000000:
			tty.c_cflag |= B4000000;
			break;
		default:
			tty.c_cflag |= BAUDRATE;
#ifdef DBG_PRIV_UTY_MECT
			if (config->baud < 0)
			{
				printf("%s - Baudrate not defined, using default baudrate %d\n", __func__, BAUDRATE);
			}
			else
			{
				printf("%s - Unsupported baudrate %d setting baudrate %d\n", __func__, config->baud, BAUDRATE);
			}
#endif
			break;
	}

	tty.c_cflag &= ~(CS5 | CS6 | CS7 | CS8);

#ifdef DBG_PRIV_UTY_MECT
	printf("%s - setting databits %d\n", __func__, config->databits);
#endif
	switch (config->databits) {
		case 5:
			tty.c_cflag |= CS5;
			break;
		case 6:
			tty.c_cflag |= CS6;
			break;
		case 7:
			tty.c_cflag |= CS7;
			break;
		case 8:
			tty.c_cflag |= CS8;
			break;
		default:
			tty.c_cflag |= CS8;
#ifdef DBG_PRIV_UTY_MECT
			if (config->databits < 0)
			{
				printf("%s - Databits not defined, using default databits %d\n", __func__, 8);
			}
			else
			{
				printf("%s - Unsupported databits %d setting databits %d\n", __func__, config->databits, 8);
			}
#endif
			break;
	}

	if (config->parity == 1 )
	{
		tty.c_cflag |= PARENB;
#ifdef DBG_PRIV_UTY_MECT
		printf("%s - Parity bit set.\n", __func__);
#endif
	}
	else
	{
		tty.c_cflag &= ~PARENB;
#ifdef DBG_PRIV_UTY_MECT
		printf("%s - Parity bit none.\n", __func__);
#endif
	}

	if (config->stopbits == 2)
	{
		tty.c_cflag |= CSTOPB;
#ifdef DBG_PRIV_UTY_MECT
		printf("%s - Stop bits %d\n", __func__, 2);
#endif
	}
	else
	{
		tty.c_cflag &= ~CSTOPB;
#ifdef DBG_PRIV_UTY_MECT
		printf("%s - Stop bit %d\n", __func__, 1);
#endif
	}

	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_oflag &= ~OPOST;
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_cflag |= CLOCAL | CREAD;

	tcflush(fd, TCIFLUSH);
	/* saving the original settings */
	tcgetattr(fd, &oldtty);
	/* set the new settings */
	tcsetattr(fd, TCSANOW, &tty);

	return fd;
}

/**
 * Restore serial port setings from backup and close it
 *
 * @ingroup s_util
 */
	void
app_s_serial_close(int fdsio)
{
	tcsetattr(fdsio, TCSANOW, &oldtty);
	close(fdsio);
}

/**
 * Convert formatted ASCII string to integer.
 * The data is formatted as follows:
 *
 *     string byte: 7 6 5 4 3 2 1 0
 * ASCII hex value:       > 3 2 1 0
 * ASCII int value:         3 2 1 0
 *
 * @param string        formatted ASCII data to convert
 *
 * @return              integer value
 *
 * @ingroup s_util
 */
	int
app_s_str_to_int(char *string)
{
	char *hex = NULL;
	char buffer[8];
	unsigned int v = 0;
	int base = 0;

	assert(string != NULL);

	strncpy(buffer, string, 8);

	hex = strchr(buffer, '>');
	if (hex != NULL) {
		base = 16;
		*hex = ' ';
	}
	else
		base = 10;

	v = (unsigned int)strtol(buffer, NULL, base);

	return v;
}

/**
 * Convert the given integer to formatted ASCII hex string.
 * The ASCII hex data is formatted as follows:
 *
 *     string byte: 7 6 5 4 3 2 1 0
 * ASCII hex value:       > 3 2 1 0
 *
 * @param integer       integer value to convert
 * @param hex           ASCII hex string to write to
 *
 * @ingroup s_util
 */
	void
app_s_int_to_hex(unsigned int integer, char *hex)
{
	int i = 0;

	assert(hex != NULL);

	*hex++ = ' ';
	*hex++ = ' ';
	*hex++ = ' ';
	*hex++ = '>';

	for (hex += 3, i = 0; i < 4; integer /= 16, hex--, i++) {
		int digit = 0;

		digit = integer % 16;
		if (digit <= 9)
			*hex = digit + '0';
		else
			*hex = (digit - 10) + 'A';
	}
}

/**
 * Convert the given float number to formatted string.
 * The string data is formatted as follows:
 *
 * float number   : - 6 . 5 4 3
 * string obtained:     - 6 . 5 4 3
 *
 * @param absval        signed float value to convert
 * @param *dst          pointer to the string
 *
 * @ingroup s_util
 */
	void
app_s_float_to_str(float absval, char *dst)
{
	int cf = 0;
	int width = 8;
	int zeros = 0;
	int prec = 5;
	char cfc = '\0';
	int i = 0;
	int j = 0;
	int Whole = 0;
	int k = 0;
	int zr = 0;
	float Frac = 0;

	/* Resolving the sign */
	if (absval < 0) {
		cfc = '-';
		*dst = cfc;
		absval = -absval;
		j = 1;
	}
	else
		j = 0;

	/* "Dissecting" double into Whole part and Frac part */
	Whole = (int)absval;
	Frac = (absval - (float)Whole);

	/* Count Whole digits */
	i = 10;
	k = j + 1;
	while (i < Whole) {
		i *= 10;
		k++;
	}

	/* Checking if there is enough width to cover prec + whole part + point */
	if ((prec + 1 + k) > width)
		width = prec + 1 + k;

	/* If zeros = true */
	if (zeros == 1) {
		zr = width - prec - 1 - k;
	}

	for (i = 0; i < zr; i++) {
		cfc = '0';
		*(dst + j + i) = cfc;
	}

	j = k + zr - 1;

	/* Add Whole to the string */
	do {
		cf = Whole % 10;
		*(dst + j) = '0' + cf;

		j--;
		Whole /= 10;
	} while (Whole > 0);

	/* Add "floating point" to the string */
	k += zr - 1;
	if (prec != 0) {
		k++;
		*(dst + k) = '.';

		/* Add Frac to the string */
		j = k + 1;
		do {
			if (Frac != 0) {
				cf = (int)(Frac * 10);
				Frac = Frac * 10 - (int)(Frac * 10);
			}
			else {
				if (zeros)
					cf = 0;
				else
					prec = 0;
			}

			*(dst + j) = '0' + cf;

			j++;
			prec--;
		} while (prec > 0);
	}                           /* from non-null prec condition */

	/*BOGDAN: Not to be used in here!*/
	/* Add NULL termination to the string */
	/* *(dst + j) = '\0'; */
	/* return dst; */
}

/**
 * Convert the given 8 character string to corresponding
 * float number.  At most 5 significative digits.
 *
 * The string data is formatted as follows:
 *
 * index :  0 1 2 3 4 5 6 7
 * string :     - 6 . 5 4 3
 * float number :    -6.543
 * string : - 0 0 6 . 5 4 3
 * float number :    -6.543
 * 
 * The parsing stops at the first illegal character:
 *
 * index :   0 1 2 3 4 5 6 7
 * string :  - 6 . 5 4 a 3 2
 * float number :      -6.54
 *
 * @param dst           string to convert
 *
 * @return              converted float value
 *
 * @ingroup s_util
 */
	float
app_s_str_to_float(char *str)
{
	char *c = NULL;                     /* Current string position */
	int ival = 0;                       /* Integer value */
	int minus = 0;                      /* Is it negative? */
	int d = 0;                          /* Decimal position */
	int i = 0;

	assert(str != NULL);

	/* Is this an integer? */
	if ((strchr(str, '.') == NULL) || (strchr(str, '>') != NULL))
		return (float)app_s_str_to_int(str);

	c = str;

	for (; *c == ' '; c++) ;            /* Skip leading spaces */

	if (*c == '-') {                    /* Grab sign */
		minus = 1;
		c++;
	}
	else
		minus = 0;

	for (; *c == '0'; c++) ;            /* Skip leading zeros */

	/* Convert float to int.  Grab decimal point position */
	for (i = c - str, ival = 0, d = 0; i < 8; i++, c++) {
		if (!(((*c >= '0') && (*c <= '9')) || (*c == '.')))
			break;

		if (*c != '.') {
			ival *= 10;
			ival += *c - '0';
		}
		else
			d = 7 - i;
	}

	if (minus)
		ival = -ival;

	if (i < 8) {                        /* Bad character break */
		printf("character %c cannot be converted to float.\n", *c);

		if (d != 0)
			d -= 8 - i;                 /* Adjust for bad character break */
	}

	/* Scale according to decimal point */
	switch (d) {
		case 1:
			return (float)ival / 10.0;
			break;

		case 2:
			return (float)ival / 100.0;
			break;

		case 3:
			return (float)ival / 1000.0;
			break;

		case 4:
			return (float)ival / 10000.0;
			break;

		case 5:
			return (float)ival / 100000.0;
			break;

		default:
			return (float)ival;
			break;
	}
}

/**
 * Set the read timeout timer
 *
 * @ingroup s_util
 */
	void
app_s_rx_timeout_set(void)
{
	struct itimerval itv;

	/* Single shot */
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 0;

	/* Timeout */
	itv.it_value.tv_sec = 0;
	itv.it_value.tv_usec = RXTIMEOUT;

	if (setitimer(ITIMER_REAL, &itv, NULL) < 0)
		perror("app_s_rx_timeout_set: cannot set the RX timer");
}

/**
 * Reset the read timeout timer
 *
 * @ingroup s_util
 */
	void
app_s_txrx_timeout_reset(void)
{
	struct itimerval itv;

	/* Single shot */
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 0;

	/* Timeout */
	itv.it_value.tv_sec = 0;
	itv.it_value.tv_usec = 0;

	if (setitimer(ITIMER_REAL, &itv, NULL) < 0)
		perror("app_s_txrx_timeout_reset: cannot reset the RX timer");
}

/**
 * Check whether the timeout occured
 *
 * @return              1 - timed out; 0 - still counting; -1 - error
 *
 * @ingroup s_util
 */
	int
app_s_txrx_timeout_check(void)
{
	struct itimerval itv;

	if (getitimer(ITIMER_REAL, &itv) < 0) {
		perror("app_s_txrx_timeout_check");

		return -1;
	}
	else
		return (itv.it_value.tv_sec == 0) && (itv.it_value.tv_usec == 0);
}
