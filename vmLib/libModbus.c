/*
 * Copyright © 2001-2011 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
 * This library implements the Modbus protocol.
 * http://libmodbus.org/
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <assert.h>
#include <byteswap.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#ifndef _MSC_VER
# include <stdint.h>
# include <sys/time.h>
#else
# include "stdint.h"
# include <time.h>
typedef int ssize_t;
#endif
#include <sys/types.h>

/* #include <config.h> */

#define TIMEOUT_ERROR -2
#define OTHER_ERROR	-1

#undef MODBUS_DEBUG

#ifdef MODBUS_DEBUG
#define DBG_PRINT(format, args...)  \
	fprintf (stderr, "[%s:%s:%d] - ", __FILE__, __func__, __LINE__); \
fprintf (stderr, format , ## args); \
fflush(stderr);
#else
#define DBG_PRINT(format, args...)
#endif

#define ABCD2FLOAT(doubleword) (doubleword)
#define BADC2FLOAT(doubleword) ((((doubleword) & 0x000F) << 4)  | (((doubleword) & 0x00F0) >> 4) | (((doubleword) & 0x0F00) << 4) | (((doubleword) & 0xF000) >> 4))
#define CDAB2FLOAT(doubleword) ((((doubleword) & 0x000F) << 8)  | (((doubleword) & 0x00F0) << 8) | (((doubleword) & 0x0F00) >> 8) | (((doubleword) & 0xF000) >> 8))
#define DCBA2FLOAT(doubleword) ((((doubleword) & 0x000F) << 12) | (((doubleword) & 0x00F0) << 4) | (((doubleword) & 0x0F00) >> 4) | (((doubleword) & 0xF000) >> 12))

#define FLOAT2ABCD(doubleword) ABCD2FLOAT(doubleword)
#define FLOAT2BADC(doubleword) BADC2FLOAT(doubleword)
#define FLOAT2CDAB(doubleword) CDAB2FLOAT(doubleword)
#define FLOAT2DCBA(doubleword) DCBA2FLOAT(doubleword)

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1
/* Define to 1 if you have the declaration of `TIOCM_RTS', and to 0 if you
   don't. */
#define HAVE_DECL_TIOCM_RTS 1
/* Define to 1 if you have the declaration of `TIOCSRS485', and to 0 if you
   don't. */
#define HAVE_DECL_TIOCSRS485 1
/* Define to 1 if you have the declaration of `__CYGWIN__', and to 0 if you
   don't. */
#define HAVE_DECL___CYGWIN__ 0
/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1
/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1
/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1
/* Define to 1 if you have the `fork' function. */
#define HAVE_FORK 1
/* Define to 1 if you have the `getaddrinfo' function. */
#define HAVE_GETADDRINFO 1
/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1
/* Define to 1 if you have the `inet_ntoa' function. */
#define HAVE_INET_NTOA 1
/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1
/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1
/* Define to 1 if you have the <linux/serial.h> header file. */
#define HAVE_LINUX_SERIAL_H 1
/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1
/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1
/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1
/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1
/* Define to 1 if you have the <netinet/tcp.h> header file. */
#define HAVE_NETINET_TCP_H 1
/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1
/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1
/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1
/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1
/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1
/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1
/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1
/* Define to 1 if you have the `strlcpy' function. */
/* #undef HAVE_STRLCPY */
/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1
/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1
/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1
/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1
/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1
/* Define to 1 if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1
/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1
/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1
/* Define to 1 if you have the `vfork' function. */
#define HAVE_VFORK 1
/* Define to 1 if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */
/* Define to 1 if you have the <winsock2.h> header file. */
/* #undef HAVE_WINSOCK2_H */
/* Define to 1 if `fork' works. */
#define HAVE_WORKING_FORK 1
/* Define to 1 if `vfork' works. */
#define HAVE_WORKING_VFORK 1
/* Define to the sub-directory in which libtool stores uninstalled libraries.
 */
#define LT_OBJDIR ".libs/"
/* Name of package */
#define PACKAGE "libmodbus"
/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "https://github.com/stephane/libmodbus/issues"
/* Define to the full name of this package. */
#define PACKAGE_NAME "libmodbus"
/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libmodbus 3.1.0"
/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libmodbus"
/* Define to the home page for this package. */
#define PACKAGE_URL "http://libmodbus.org/"
/* Define to the version of this package. */
#define PACKAGE_VERSION "3.1.0"
/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1
/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif
/* Version number of package */
#define VERSION "3.1.0"
/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64
/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */
/* Define to 1 if on MINIX. */
/* #undef _MINIX */
/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */
/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */
/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */
/* Define as `fork' if `vfork' does not work. */
/* #undef vfork */

/* end #include <config.h> */

#include "libModbus.h"

/* #include "modbus-private.h" */
#if defined(RTS_CFG_MODBUS_LIB)
MODBUS_BEGIN_DECLS

/* It's not really the minimal length (the real one is report slave ID
 * in RTU (4 bytes)) but it's a convenient size to use in RTU or TCP
 * communications to read many values or write a single one.
 * Maximum between :
 * - HEADER_LENGTH_TCP (7) + function (1) + address (2) + number (2)
 * - HEADER_LENGTH_RTU (1) + function (1) + address (2) + number (2) + CRC (2)
 */
#define _MIN_REQ_LENGTH 12

#define _REPORT_SLAVE_ID 180

#define _MODBUS_EXCEPTION_RSP_LENGTH 5

/* Timeouts in microsecond (0.5 s) */
#define _RESPONSE_TIMEOUT    300000
#define _BYTE_TIMEOUT        300000

/* Function codes */
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

typedef enum {
	_MODBUS_BACKEND_TYPE_RTU=0,
	_MODBUS_BACKEND_TYPE_TCP,
	_MODBUS_BACKEND_TYPE_TCPRTU
} modbus_backend_type_t;

/*
 *  ---------- Request     Indication ----------
 *  | Client | ---------------------->| Server |
 *  ---------- Confirmation  Response ----------
 */
typedef enum {
	/* Request message on the server side */
	MSG_INDICATION,
	/* Request message on the client side */
	MSG_CONFIRMATION
} msg_type_t;

/* This structure reduces the number of params in functions and so
 * optimizes the speed of execution (~ 37%). */
typedef struct _sft {
	int slave;
	int function;
	int t_id;
} sft_t;

typedef struct _modbus_backend {
	unsigned int backend_type;
	unsigned int header_length;
	unsigned int checksum_length;
	unsigned int max_adu_length;
	int (*set_slave) (modbus_t *ctx, int slave);
	int (*build_request_basis) (modbus_t *ctx, int function, int addr,
			int nb, uint8_t *req);
	int (*build_response_basis) (sft_t *sft, uint8_t *rsp);
	int (*prepare_response_tid) (const uint8_t *req, int *req_length);
	int (*send_msg_pre) (uint8_t *req, int req_length);
	ssize_t (*send) (modbus_t *ctx, const uint8_t *req, int req_length);
	int (*receive) (modbus_t *ctx, uint8_t *req);
	ssize_t (*recv) (modbus_t *ctx, uint8_t *rsp, int rsp_length);
	int (*check_integrity) (modbus_t *ctx, uint8_t *msg,
			const int msg_length);
	int (*pre_check_confirmation) (modbus_t *ctx, const uint8_t *req,
			const uint8_t *rsp, int rsp_length);
	int (*connect) (modbus_t *ctx);
	void (*close) (modbus_t *ctx);
	int (*flush) (modbus_t *ctx);
	int (*select) (modbus_t *ctx, fd_set *rset, struct timeval *tv, int msg_length);
	void (*free) (modbus_t *ctx);
} modbus_backend_t;

struct _modbus {
	/* Slave address */
	int slave;
	/* Socket or file descriptor */
	int s;
	int debug;
	int error_recovery;
	struct timeval response_timeout;
	struct timeval byte_timeout;
	const modbus_backend_t *backend;
	void *backend_data;
};

void _modbus_init_common(modbus_t *ctx);
void _error_print(modbus_t *ctx, const char *context, int line);
int _modbus_receive_msg(modbus_t *ctx, uint8_t *msg, msg_type_t msg_type);

#ifndef HAVE_STRLCPY
size_t strlcpy(char *dest, const char *src, size_t dest_size);
#endif

modbus_t *ctx_rtu    = NULL;
modbus_t *ctx_tcp    = NULL;
modbus_t *ctx_tcp_pi = NULL;
modbus_t *ctx_tcprtu = NULL;

int sockettcp    = 0;
int sockettcprtu = 0;

MODBUS_END_DECLS
/* end #include modbus-private.h */

/* #include modbus-rtu-private.h */
#if defined(_WIN32)
#include <windows.h>
#else
#include <termios.h>
#endif

#define _MODBUS_RTU_HEADER_LENGTH      1
#define _MODBUS_RTU_PRESET_REQ_LENGTH  6
#define _MODBUS_RTU_PRESET_RSP_LENGTH  2

#define _MODBUS_RTU_CHECKSUM_LENGTH    2

/* Time waited beetween the RTS switch before transmit data or after transmit
   data before to read */
#define _MODBUS_RTU_TIME_BETWEEN_RTS_SWITCH 10000

#if defined(_WIN32)
#if !defined(ENOTSUP)
#define ENOTSUP WSAEOPNOTSUPP
#endif

/* WIN32: struct containing serial handle and a receive buffer */
#define PY_BUF_SIZE 512
struct win32_ser {
	/* File handle */
	HANDLE fd;
	/* Receive buffer */
	uint8_t buf[PY_BUF_SIZE];
	/* Received chars */
	DWORD n_bytes;
};
#endif /* _WIN32 */

typedef struct _modbus_rtu {
	/* Device: "/dev/ttyS0", "/dev/ttyUSB0" or "/dev/tty.USA19*" on Mac OS X. */
	char *device;
	/* Bauds: 9600, 19200, 57600, 115200, etc */
	int baud;
	/* Data bit */
	uint8_t data_bit;
	/* Stop bit */
	uint8_t stop_bit;
	/* Parity: 'N', 'O', 'E' */
	char parity;
#if defined(_WIN32)
	struct win32_ser w_ser;
	DCB old_dcb;
#else
	/* Save old termios settings */
	struct termios old_tios;
#endif
#if HAVE_DECL_TIOCSRS485
	int serial_mode;
#endif
#if HAVE_DECL_TIOCM_RTS
	int rts;
	int onebyte_time;
#endif
	/* To handle many slaves on the same link */
	int confirmation_to_ignore;
} modbus_rtu_t;

/* end #include modbus-rtu-private.h */

/*#include modbus-tcprtu-private.h*/
#define _MODBUS_TCPRTU_HEADER_LENGTH      1
#define _MODBUS_TCPRTU_PRESET_REQ_LENGTH  6
#define _MODBUS_TCPRTU_PRESET_RSP_LENGTH  2

#define _MODBUS_TCPRTU_CHECKSUM_LENGTH    2
#include <linux/types.h>

/* In both structures, the transaction ID must be placed on first position
   to have a quick access not dependant of the TCP backend */
typedef struct _modbus_tcprtu {
	/* Extract from MODBUS Messaging on TCP/IP Implementation Guide V1.0b
	   (page 23/46):
	   The transaction identifier is used to associate the future response
	   with the request. This identifier is unique on each TCP connection. */
	uint16_t t_id;
	/* TCP port */
	int port;
	/* IP address */
	char ip[16];
} modbus_tcprtu_t;

/*end #include modbus-tcprtu-private.h*/

/* #include modbus-tcp-private.h */
//#include <linux/types.h>
#define _MODBUS_TCP_HEADER_LENGTH      7
#define _MODBUS_TCP_PRESET_REQ_LENGTH 12
#define _MODBUS_TCP_PRESET_RSP_LENGTH  8

#define _MODBUS_TCP_CHECKSUM_LENGTH    0

/* In both structures, the transaction ID must be placed on first position
   to have a quick access not dependant of the TCP backend */
typedef struct _modbus_tcp {
	/* Extract from MODBUS Messaging on TCP/IP Implementation Guide V1.0b
	   (page 23/46):
	   The transaction identifier is used to associate the future response
	   with the request. This identifier is unique on each TCP connection. */
	uint16_t t_id;
	/* TCP port */
	int port;
	/* IP address */
	char ip[16];
} modbus_tcp_t;

#define _MODBUS_TCP_PI_NODE_LENGTH    1025
#define _MODBUS_TCP_PI_SERVICE_LENGTH   32

typedef struct _modbus_tcp_pi {
	/* Transaction ID */
	uint16_t t_id;
	/* TCP port */
	int port;
	/* Node */
	char node[_MODBUS_TCP_PI_NODE_LENGTH];
	/* Service */
	char service[_MODBUS_TCP_PI_SERVICE_LENGTH];
} modbus_tcp_pi_t;
/*end #include modbus-tcp-private.h*/


/* Internal use */
#define MSG_LENGTH_UNDEFINED -1

/* Exported version */
const unsigned int libmodbus_version_major = LIBMODBUS_VERSION_MAJOR;
const unsigned int libmodbus_version_minor = LIBMODBUS_VERSION_MINOR;
const unsigned int libmodbus_version_micro = LIBMODBUS_VERSION_MICRO;

/* 3 steps are used to parse the query */
typedef enum {
	_STEP_FUNCTION,
	_STEP_META,
	_STEP_DATA
} _step_t;

const char *modbus_strerror(int errnum) {
	switch (errnum) {
		case EMBXILFUN:
			return "Illegal function";
		case EMBXILADD:
			return "Illegal data address";
		case EMBXILVAL:
			return "Illegal data value";
		case EMBXSFAIL:
			return "Slave device or server failure";
		case EMBXACK:
			return "Acknowledge";
		case EMBXSBUSY:
			return "Slave device or server is busy";
		case EMBXNACK:
			return "Negative acknowledge";
		case EMBXMEMPAR:
			return "Memory parity error";
		case EMBXGPATH:
			return "Gateway path unavailable";
		case EMBXGTAR:
			return "Target device failed to respond";
		case EMBBADCRC:
			return "Invalid CRC";
		case EMBBADDATA:
			return "Invalid data";
		case EMBBADEXC:
			return "Invalid exception code";
		case EMBMDATA:
			return "Too many data";
		case EMBBADSLAVE:
			return "Response not from requested slave";
		default:
			return strerror(errnum);
	}
}

void _error_print(modbus_t *ctx, const char *context, int line)
{
	if (ctx->debug) {
		fprintf(stderr, "ERROR:%d: %s", line, modbus_strerror(errno));
		if (context != NULL) {
			fprintf(stderr, ": %s\n", context);
		} else {
			fprintf(stderr, "\n");
		}
	}
}

static void _sleep_response_timeout(modbus_t *ctx)
{
#ifdef _WIN32
	/* usleep doesn't exist on Windows */
	Sleep((ctx->response_timeout.tv_sec * 1000) +
			(ctx->response_timeout.tv_usec / 1000));
#else
	/* usleep source code */
	struct timespec request, remaining;
	request.tv_sec = ctx->response_timeout.tv_sec;
	request.tv_nsec = ((long int)ctx->response_timeout.tv_usec % 1000000)
		* 1000;
	while (nanosleep(&request, &remaining) == -1 && errno == EINTR)
		request = remaining;
#endif
}

int modbus_flush(modbus_t *ctx)
{
	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	int rc = ctx->backend->flush(ctx);
	if (rc != -1 && ctx->debug) {
		/* Not all backends are able to return the number of bytes flushed */
		fprintf(stderr,"Bytes flushed (%d)\n", rc);
	}
	return rc;
}

/* Computes the length of the expected response */
static unsigned int compute_response_length_from_request(modbus_t *ctx, uint8_t *req)
{
	int length;
	const int offset = ctx->backend->header_length;

	switch (req[offset]) {
		case _FC_READ_COILS:
		case _FC_READ_DISCRETE_INPUTS: {
										   /* Header + nb values (code from write_bits) */
										   int nb = (req[offset + 3] << 8) | req[offset + 4];
										   length = 2 + (nb / 8) + ((nb % 8) ? 1 : 0);
									   }
									   break;
		case _FC_WRITE_AND_READ_REGISTERS:
		case _FC_READ_HOLDING_REGISTERS:
		case _FC_READ_INPUT_REGISTERS:
									   /* Header + 2 * nb values */
									   length = 2 + 2 * (req[offset + 3] << 8 | req[offset + 4]);
									   break;
		case _FC_READ_EXCEPTION_STATUS:
									   length = 3;
									   break;
		case _FC_REPORT_SLAVE_ID:
									   /* The response is device specific (the header provides the
										  length) */
									   return MSG_LENGTH_UNDEFINED;
		case _FC_MASK_WRITE_REGISTER:
									   length = 7;
									   break;
		default:
									   length = 5;
	}

	return offset + length + ctx->backend->checksum_length;
}

/* Sends a request/response */
static int send_msg(modbus_t *ctx, uint8_t *msg, int msg_length)
{
	int rc;
	int i;

	msg_length = ctx->backend->send_msg_pre(msg, msg_length);

	if (ctx->debug) {
		for (i = 0; i < msg_length; i++)
			fprintf(stderr,"[%.2X]", msg[i]);
		fprintf(stderr,"\n");
	}

	/* In recovery mode, the write command will be issued until to be
	   successful! Disabled by default. */
	do {
		rc = ctx->backend->send(ctx, msg, msg_length);
		DBG_PRINT("rc =%d\n",rc);
		if (rc == -1) {
			_error_print(ctx, NULL, __LINE__);
			if (ctx->error_recovery & MODBUS_ERROR_RECOVERY_LINK) {
				int saved_errno = errno;

				if ((errno == EBADF || errno == ECONNRESET || errno == EPIPE)) {
					modbus_close(ctx);
					_sleep_response_timeout(ctx);
					modbus_connect(ctx);
				} else {
					_sleep_response_timeout(ctx);
					modbus_flush(ctx);
				}
				errno = saved_errno;
			}
		}
	} while ((ctx->error_recovery & MODBUS_ERROR_RECOVERY_LINK) &&
			rc == -1);

	if (rc > 0 && rc != msg_length) {
		errno = EMBBADDATA;
		return -1;
	}

	return rc;
}

int modbus_send_raw_request(modbus_t *ctx, uint8_t *raw_req, int raw_req_length)
{
	sft_t sft;
	uint8_t req[MAX_MESSAGE_LENGTH];
	int req_length;

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	
	if (raw_req_length < 2) {
		/* The raw request must contain function and slave at least */
		errno = EINVAL;
		return -1;
	}

	sft.slave = raw_req[0];
	sft.function = raw_req[1];
	/* The t_id is left to zero */
	sft.t_id = 0;
	/* This response function only set the header so it's convenient here */
	req_length = ctx->backend->build_response_basis(&sft, req);

	if (raw_req_length > 2) {
		/* Copy data after function code */
		memcpy(req + req_length, raw_req + 2, raw_req_length - 2);
		req_length += raw_req_length - 2;
	}

	return send_msg(ctx, req, req_length);
}

/*
 *  ---------- Request     Indication ----------
 *  | Client | ---------------------->| Server |
 *  ---------- Confirmation  Response ----------
 */

/* Computes the length to read after the function received */
static uint8_t compute_meta_length_after_function(int function,
		msg_type_t msg_type)
{
	int length;

	if (msg_type == MSG_INDICATION) {
		if (function <= _FC_WRITE_SINGLE_REGISTER) {
			length = 4;
		} else if (function == _FC_WRITE_MULTIPLE_COILS ||
				function == _FC_WRITE_MULTIPLE_REGISTERS) {
			length = 5;
		} else if (function == _FC_MASK_WRITE_REGISTER) {
			length = 6;
		} else if (function == _FC_WRITE_AND_READ_REGISTERS) {
			length = 9;
		} else {
			/* _FC_READ_EXCEPTION_STATUS, _FC_REPORT_SLAVE_ID */
			length = 0;
		}
	} else {
		/* MSG_CONFIRMATION */
		switch (function) {
			case _FC_WRITE_SINGLE_COIL:
			case _FC_WRITE_SINGLE_REGISTER:
			case _FC_WRITE_MULTIPLE_COILS:
			case _FC_WRITE_MULTIPLE_REGISTERS:
				length = 4;
				break;
			case _FC_MASK_WRITE_REGISTER:
				length = 6;
				break;
			default:
				length = 1;
		}
	}

	return length;
}

/* Computes the length to read after the meta information (address, count, etc) */
static int compute_data_length_after_meta(modbus_t *ctx, uint8_t *msg,
		msg_type_t msg_type)
{
	int function = msg[ctx->backend->header_length];
	int length;

	if (msg_type == MSG_INDICATION) {
		switch (function) {
			case _FC_WRITE_MULTIPLE_COILS:
			case _FC_WRITE_MULTIPLE_REGISTERS:
				length = msg[ctx->backend->header_length + 5];
				break;
			case _FC_WRITE_AND_READ_REGISTERS:
				length = msg[ctx->backend->header_length + 9];
				break;
			default:
				length = 0;
		}
	} else {
		/* MSG_CONFIRMATION */
		if (function <= _FC_READ_INPUT_REGISTERS ||
				function == _FC_REPORT_SLAVE_ID ||
				function == _FC_WRITE_AND_READ_REGISTERS) {
			length = msg[ctx->backend->header_length + 1];
		} else {
			length = 0;
		}
	}

	length += ctx->backend->checksum_length;

	return length;
}


/* Waits a response from a modbus server or a request from a modbus client.
   This function blocks if there is no replies (3 timeouts).

   The function shall return the number of received characters and the received
   message in an array of uint8_t if successful. Otherwise it shall return -1
   and errno is set to one of the values defined below:
   - ECONNRESET
   - EMBBADDATA
   - EMBUNKEXC
   - ETIMEDOUT
   - read() or recv() error codes
 */

int _modbus_receive_msg(modbus_t *ctx, uint8_t *msg, msg_type_t msg_type)
{
	int rc;
	int ret_val;
	fd_set rset;
	struct timeval tv;
	struct timeval *p_tv;
	int length_to_read;
	int msg_length = 0;
	_step_t step;

	if (ctx->debug) {
		if (msg_type == MSG_INDICATION) {
			fprintf(stderr,"Waiting for a indication...\n");
		} else {
			fprintf(stderr,"Waiting for a confirmation...\n");
		}
	}

	/* Add a file descriptor to the set */
	FD_ZERO(&rset);
	FD_SET(ctx->s, &rset);

	/* We need to analyse the message step by step.  At the first step, we want
	 * to reach the function code because all packets contain this
	 * information. */
	step = _STEP_FUNCTION;
	length_to_read = ctx->backend->header_length + 1;

	if (msg_type == MSG_INDICATION) {
		/* Wait for a message, we don't know when the message will be
		 * received */
		p_tv = NULL;
	} else {
		tv.tv_sec = ctx->response_timeout.tv_sec;
		tv.tv_usec = ctx->response_timeout.tv_usec;
		p_tv = &tv;
	}

	while (length_to_read != 0) {
		rc = ctx->backend->select(ctx, &rset, p_tv, length_to_read);
		if (rc == -1) {
			_error_print(ctx, "select", __LINE__);
			if (ctx->error_recovery & MODBUS_ERROR_RECOVERY_LINK) {
				int saved_errno = errno;

				if (errno == ETIMEDOUT) {
					_sleep_response_timeout(ctx);
					DBG_PRINT("Going to flush socket....\n");
					modbus_flush(ctx);
					ret_val = TIMEOUT_ERROR;

				} else if (errno == EBADF) {
					modbus_close(ctx);
					modbus_connect(ctx);
					ret_val = OTHER_ERROR;
				}
				errno = saved_errno;
            } else {
                if (errno == ETIMEDOUT) {
                    ret_val = TIMEOUT_ERROR;
                } else {
                ret_val = OTHER_ERROR;
                }
            }
			return ret_val;
		}

        if ((msg_length + length_to_read) < MAX_MESSAGE_LENGTH) {
            rc = ctx->backend->recv(ctx, msg + msg_length, length_to_read);
        } else {
            rc = -1;
        }
		if (rc == 0) {
			errno = ECONNRESET;
			rc = -1;
		}

		if (rc == -1) {
			_error_print(ctx, "read", __LINE__);
			if ((ctx->error_recovery & MODBUS_ERROR_RECOVERY_LINK) &&
					(errno == ECONNRESET || errno == ECONNREFUSED ||
					 errno == EBADF)) {
				int saved_errno = errno;
				modbus_close(ctx);
				modbus_connect(ctx);
				/* Could be removed by previous calls */
				errno = saved_errno;
			}
			ret_val = OTHER_ERROR;
			return ret_val;
		}

		/* Display the hex code of each character received */
		if (ctx->debug) {
			int i;
			for (i=0; i < rc; i++)
				fprintf(stderr,"<%.2X>", msg[msg_length + i]);
		}

		/* Sums bytes received */
		msg_length += rc;
		/* Computes remaining bytes */
		length_to_read -= rc;

		if (length_to_read == 0) {
			switch (step) {
				case _STEP_FUNCTION:
					/* Function code position */
					length_to_read = compute_meta_length_after_function(
							msg[ctx->backend->header_length],
							msg_type);
					if (length_to_read != 0) {
						step = _STEP_META;
						break;
					} /* else switches straight to the next step */
				case _STEP_META:
					length_to_read = compute_data_length_after_meta(
							ctx, msg, msg_type);
					if ((msg_length + length_to_read) > (int)ctx->backend->max_adu_length) {
						errno = EMBBADDATA;
						_error_print(ctx, "too many data", __LINE__);
						ret_val = OTHER_ERROR;
						return ret_val;
					}
					step = _STEP_DATA;
					break;
				default:
					break;
			}
		}

		if (length_to_read > 0 && ctx->byte_timeout.tv_sec != -1) {
			/* If there is no character in the buffer, the allowed timeout
			   interval between two consecutive bytes is defined by
			   byte_timeout */
			tv.tv_sec = ctx->byte_timeout.tv_sec;
			tv.tv_usec = ctx->byte_timeout.tv_usec;
			p_tv = &tv;
		}
	}

	if (ctx->debug)
		fprintf(stderr,"\n");

	return ctx->backend->check_integrity(ctx, msg, msg_length);
}

/* Receive the request from a modbus master */
int modbus_receive(modbus_t *ctx, uint8_t *req)
{

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	return ctx->backend->receive(ctx, req);
}

/* Receives the confirmation.

   The function shall store the read response in rsp and return the number of
   values (bits or words). Otherwise, its shall return -1 and errno is set.

   The function doesn't check the confirmation is the expected response to the
   initial request.
 */
int modbus_receive_confirmation(modbus_t *ctx, uint8_t *rsp)
{

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	return _modbus_receive_msg(ctx, rsp, MSG_CONFIRMATION);
}

static int check_confirmation(modbus_t *ctx, uint8_t *req,
		uint8_t *rsp, int rsp_length)
{
	int rc;
	int rsp_length_computed;
	const int offset = ctx->backend->header_length;
	const int function = rsp[offset];

	if (ctx->backend->pre_check_confirmation) {
		rc = ctx->backend->pre_check_confirmation(ctx, req, rsp, rsp_length);
		if (rc == -1) {
			if (ctx->error_recovery & MODBUS_ERROR_RECOVERY_PROTOCOL) {
				_sleep_response_timeout(ctx);
				modbus_flush(ctx);
			}
			return -1;
		}
	}

	rsp_length_computed = compute_response_length_from_request(ctx, req);

	/* Exception code */
	if (function >= 0x80) {
		if (rsp_length == (offset + 2 + (int)ctx->backend->checksum_length) &&
				req[offset] == (rsp[offset] - 0x80)) {
			/* Valid exception code received */

			int exception_code = rsp[offset + 1];
			if (exception_code < MODBUS_EXCEPTION_MAX) {
				errno = MODBUS_ENOBASE + exception_code;
			} else {
				errno = EMBBADEXC;
			}
			_error_print(ctx, NULL, __LINE__);
			return -1;
		} else {
			errno = EMBBADEXC;
			_error_print(ctx, NULL, __LINE__);
			return -1;
		}
	}

	/* Check length */
	if ((rsp_length == rsp_length_computed ||
				rsp_length_computed == MSG_LENGTH_UNDEFINED) &&
			function < 0x80) {
		int req_nb_value;
		int rsp_nb_value;

		/* Check function code */
		if (function != req[offset]) {
			if (ctx->debug) {
				fprintf(stderr,
						"Received function not corresponding to the requestd (0x%X != 0x%X)\n",
						function, req[offset]);
			}
			if (ctx->error_recovery & MODBUS_ERROR_RECOVERY_PROTOCOL) {
				_sleep_response_timeout(ctx);
				modbus_flush(ctx);
			}
			errno = EMBBADDATA;
			return -1;
		}

		/* Check the number of values is corresponding to the request */
		switch (function) {
			case _FC_READ_COILS:
			case _FC_READ_DISCRETE_INPUTS:
				/* Read functions, 8 values in a byte (nb
				 * of values in the request and byte count in
				 * the response. */
				req_nb_value = (req[offset + 3] << 8) + req[offset + 4];
				req_nb_value = (req_nb_value / 8) + ((req_nb_value % 8) ? 1 : 0);
				rsp_nb_value = rsp[offset + 1];
				break;
			case _FC_WRITE_AND_READ_REGISTERS:
			case _FC_READ_HOLDING_REGISTERS:
			case _FC_READ_INPUT_REGISTERS:
				/* Read functions 1 value = 2 bytes */
				req_nb_value = (req[offset + 3] << 8) + req[offset + 4];
				rsp_nb_value = (rsp[offset + 1] / 2);
				break;
			case _FC_WRITE_MULTIPLE_COILS:
			case _FC_WRITE_MULTIPLE_REGISTERS:
				/* N Write functions */
				req_nb_value = (req[offset + 3] << 8) + req[offset + 4];
				rsp_nb_value = (rsp[offset + 3] << 8) | rsp[offset + 4];
				break;
			case _FC_REPORT_SLAVE_ID:
				/* Report slave ID (bytes received) */
				req_nb_value = rsp_nb_value = rsp[offset + 1];
				break;
			default:
				/* 1 Write functions & others */
				req_nb_value = rsp_nb_value = 1;
		}

		if (req_nb_value == rsp_nb_value) {
			rc = rsp_nb_value;
		} else {
			if (ctx->debug) {
				fprintf(stderr,
						"Quantity not corresponding to the request (%d != %d)\n",
						rsp_nb_value, req_nb_value);
			}

			if (ctx->error_recovery & MODBUS_ERROR_RECOVERY_PROTOCOL) {
				_sleep_response_timeout(ctx);
				modbus_flush(ctx);
			}

			errno = EMBBADDATA;
			rc = -1;
		}
	} else {
		if (ctx->debug) {
			fprintf(stderr,
					"Message length not corresponding to the computed length (%d != %d)\n",
					rsp_length, rsp_length_computed);
		}
		if (ctx->error_recovery & MODBUS_ERROR_RECOVERY_PROTOCOL) {
			_sleep_response_timeout(ctx);
			modbus_flush(ctx);
		}
		errno = EMBBADDATA;
		rc = -1;
	}

	return rc;
}

static int response_io_status(int address, int nb,
		uint8_t *tab_io_status,
		uint8_t *rsp, int offset)
{
	int shift = 0;
	int byte = 0;
	int i;

	for (i = address; i < address+nb; i++) {
		byte |= tab_io_status[i] << shift;
		if (shift == 7) {
			/* Byte is full */
			rsp[offset++] = byte;
			byte = shift = 0;
		} else {
			shift++;
		}
	}

	if (shift != 0)
		rsp[offset++] = byte;

	return offset;
}

/* Build the exception response */
static int response_exception(modbus_t *ctx, sft_t *sft,
		int exception_code, uint8_t *rsp)
{
	int rsp_length;

	sft->function = sft->function + 0x80;
	rsp_length = ctx->backend->build_response_basis(sft, rsp);

	/* Positive exception code */
	rsp[rsp_length++] = exception_code;

	return rsp_length;
}

/* Send a response to the received request.
   Analyses the request and constructs a response.

   If an error occurs, this function construct the response
   accordingly.
 */
int modbus_reply(modbus_t *ctx, const uint8_t *req,
        int req_length, modbus_mapping_t *mb_mapping)
{

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	int offset = ctx->backend->header_length;
	int slave = req[offset - 1];
	int function = req[offset];
	uint16_t address = (req[offset + 1] << 8) + req[offset + 2];
	uint8_t rsp[MAX_MESSAGE_LENGTH];
	int rsp_length = 0;
	sft_t sft;

	sft.slave = slave;
	sft.function = function;
	sft.t_id = ctx->backend->prepare_response_tid(req, &req_length);

	switch (function) {
		case _FC_READ_COILS: {
								 int nb = (req[offset + 3] << 8) + req[offset + 4];

								 if ((address + nb) > mb_mapping->nb_bits) {
									 if (ctx->debug) {
										 fprintf(stderr, "Illegal data address %0X in read_bits\n",
												 address + nb);
									 }
									 rsp_length = response_exception(
											 ctx, &sft,
											 MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
								 } else {
									 rsp_length = ctx->backend->build_response_basis(&sft, rsp);
									 rsp[rsp_length++] = (nb / 8) + ((nb % 8) ? 1 : 0);
									 rsp_length = response_io_status(address, nb,
											 mb_mapping->tab_bits,
											 rsp, rsp_length);
								 }
							 }
							 break;
		case _FC_READ_DISCRETE_INPUTS: {
										   /* Similar to coil status (but too many arguments to use a
											* function) */
										   int nb = (req[offset + 3] << 8) + req[offset + 4];

										   if ((address + nb) > mb_mapping->nb_input_bits) {
											   if (ctx->debug) {
												   fprintf(stderr, "Illegal data address %0X in read_input_bits\n",
														   address + nb);
											   }
											   rsp_length = response_exception(
													   ctx, &sft,
													   MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
										   } else {
											   rsp_length = ctx->backend->build_response_basis(&sft, rsp);
											   rsp[rsp_length++] = (nb / 8) + ((nb % 8) ? 1 : 0);
											   rsp_length = response_io_status(address, nb,
													   mb_mapping->tab_input_bits,
													   rsp, rsp_length);
										   }
									   }
									   break;
		case _FC_READ_HOLDING_REGISTERS: {
											 int nb = (req[offset + 3] << 8) + req[offset + 4];

											 if ((address + nb) > mb_mapping->nb_registers) {
												 if (ctx->debug) {
													 fprintf(stderr, "Illegal data address %0X in read_registers\n",
															 address + nb);
												 }
												 rsp_length = response_exception(
														 ctx, &sft,
														 MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
											 } else {
												 int i;

												 rsp_length = ctx->backend->build_response_basis(&sft, rsp);
												 rsp[rsp_length++] = nb << 1;
												 for (i = address; i < address + nb; i++) {
													 rsp[rsp_length++] = mb_mapping->tab_registers[i] >> 8;
													 rsp[rsp_length++] = mb_mapping->tab_registers[i] & 0xFF;
												 }
											 }
										 }
										 break;
		case _FC_READ_INPUT_REGISTERS: {
										   /* Similar to holding registers (but too many arguments to use a
											* function) */
										   int nb = (req[offset + 3] << 8) + req[offset + 4];

										   if ((address + nb) > mb_mapping->nb_input_registers) {
											   if (ctx->debug) {
												   fprintf(stderr, "Illegal data address %0X in read_input_registers\n",
														   address + nb);
											   }
											   rsp_length = response_exception(
													   ctx, &sft,
													   MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
										   } else {
											   int i;

											   rsp_length = ctx->backend->build_response_basis(&sft, rsp);
											   rsp[rsp_length++] = nb << 1;
											   for (i = address; i < address + nb; i++) {
												   rsp[rsp_length++] = mb_mapping->tab_input_registers[i] >> 8;
												   rsp[rsp_length++] = mb_mapping->tab_input_registers[i] & 0xFF;
											   }
										   }
									   }
									   break;
		case _FC_WRITE_SINGLE_COIL:
									   if (address >= mb_mapping->nb_bits) {
										   if (ctx->debug) {
											   fprintf(stderr, "Illegal data address %0X in write_bit\n",
													   address);
										   }
										   rsp_length = response_exception(
												   ctx, &sft,
												   MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
									   } else {
										   int data = (req[offset + 3] << 8) + req[offset + 4];

										   if (data == 0xFF00 || data == 0x0) {
											   mb_mapping->tab_bits[address] = (data) ? ON : OFF;
											   memcpy(rsp, req, req_length);
											   rsp_length = req_length;
										   } else {
											   if (ctx->debug) {
												   fprintf(stderr,
														   "Illegal data value %0X in write_bit request at address %0X\n",
														   data, address);
											   }
											   rsp_length = response_exception(
													   ctx, &sft,
													   MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE, rsp);
										   }
									   }
									   break;
		case _FC_WRITE_SINGLE_REGISTER:
									   if (address >= mb_mapping->nb_registers) {
										   if (ctx->debug) {
											   fprintf(stderr, "Illegal data address %0X in write_register\n",
													   address);
										   }
										   rsp_length = response_exception(
												   ctx, &sft,
												   MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
									   } else {
										   int data = (req[offset + 3] << 8) + req[offset + 4];

                                           mb_mapping->tab_registers[address] = data;
										   memcpy(rsp, req, req_length);
										   rsp_length = req_length;
									   }
									   break;
		case _FC_WRITE_MULTIPLE_COILS: {
										   int nb = (req[offset + 3] << 8) + req[offset + 4];

										   if ((address + nb) > mb_mapping->nb_bits) {
											   if (ctx->debug) {
												   fprintf(stderr, "Illegal data address %0X in write_bits\n",
														   address + nb);
											   }
											   rsp_length = response_exception(
													   ctx, &sft,
													   MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
										   } else {
											   /* 6 = byte count */
											   modbus_set_bits_from_bytes(mb_mapping->tab_bits, address, nb, &req[offset + 6]);

											   rsp_length = ctx->backend->build_response_basis(&sft, rsp);
											   /* 4 to copy the bit address (2) and the quantity of bits */
											   memcpy(rsp + rsp_length, req + rsp_length, 4);
											   rsp_length += 4;
										   }
									   }
									   break;
		case _FC_WRITE_MULTIPLE_REGISTERS: {
											   int nb = (req[offset + 3] << 8) + req[offset + 4];

											   if ((address + nb) > mb_mapping->nb_registers) {
												   if (ctx->debug) {
													   fprintf(stderr, "Illegal data address %0X in write_registers\n",
															   address + nb);
												   }
												   rsp_length = response_exception(
														   ctx, &sft,
														   MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
											   } else {
												   int i, j;
												   for (i = address, j = 6; i < address + nb; i++, j += 2) {
													   /* 6 and 7 = first value */
                                                       mb_mapping->tab_registers[i] =
														   (req[offset + j] << 8) + req[offset + j + 1];
												   }

												   rsp_length = ctx->backend->build_response_basis(&sft, rsp);
												   /* 4 to copy the address (2) and the no. of registers */
												   memcpy(rsp + rsp_length, req + rsp_length, 4);
												   rsp_length += 4;
											   }
										   }
										   break;
		case _FC_REPORT_SLAVE_ID: {
									  int str_len;
									  int byte_count_pos;

									  rsp_length = ctx->backend->build_response_basis(&sft, rsp);
									  /* Skip byte count for now */
									  byte_count_pos = rsp_length++;
									  rsp[rsp_length++] = _REPORT_SLAVE_ID;
									  /* Run indicator status to ON */
									  rsp[rsp_length++] = 0xFF;
									  /* LMB + length of LIBMODBUS_VERSION_STRING */
									  str_len = 3 + strlen(LIBMODBUS_VERSION_STRING);
									  memcpy(rsp + rsp_length, "LMB" LIBMODBUS_VERSION_STRING, str_len);
									  rsp_length += str_len;
									  rsp[byte_count_pos] = rsp_length - byte_count_pos - 1;
								  }
								  break;
		case _FC_READ_EXCEPTION_STATUS:
								  if (ctx->debug) {
									  fprintf(stderr, "FIXME Not implemented\n");
								  }
								  errno = ENOPROTOOPT;
								  return -1;
								  break;
		case _FC_MASK_WRITE_REGISTER:
								  if (address >= mb_mapping->nb_registers) {
									  if (ctx->debug) {
										  fprintf(stderr, "Illegal data address %0X in write_register\n",
												  address);
									  }
									  rsp_length = response_exception(
											  ctx, &sft,
											  MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
								  } else {
									  uint16_t data = mb_mapping->tab_registers[address];
									  uint16_t and = (req[offset + 3] << 8) + req[offset + 4];
									  uint16_t or = (req[offset + 5] << 8) + req[offset + 6];

									  data = (data & and) | (or & (~and));
                                      mb_mapping->tab_registers[address] = data;
									  memcpy(rsp, req, req_length);
									  rsp_length = req_length;
								  }
								  break;
		case _FC_WRITE_AND_READ_REGISTERS: {
											   int nb = (req[offset + 3] << 8) + req[offset + 4];
											   uint16_t address_write = (req[offset + 5] << 8) + req[offset + 6];
											   int nb_write = (req[offset + 7] << 8) + req[offset + 8];

											   if ((address + nb) > mb_mapping->nb_registers ||
													   (address_write + nb_write) > mb_mapping->nb_registers) {
												   if (ctx->debug) {
													   fprintf(stderr,
															   "Illegal data read address %0X or write address %0X write_and_read_registers\n",
															   address + nb, address_write + nb_write);
												   }
												   rsp_length = response_exception(ctx, &sft,
														   MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS, rsp);
											   } else {
												   int i, j;
												   rsp_length = ctx->backend->build_response_basis(&sft, rsp);
												   rsp[rsp_length++] = nb << 1;

												   /* Write first.
													  10 and 11 are the offset of the first values to write */
												   for (i = address_write, j = 10; i < address_write + nb_write; i++, j += 2) {
                                                       mb_mapping->tab_registers[i] =
														   (req[offset + j] << 8) + req[offset + j + 1];
												   }

												   /* and read the data for the response */
												   for (i = address; i < address + nb; i++) {
													   rsp[rsp_length++] = mb_mapping->tab_registers[i] >> 8;
													   rsp[rsp_length++] = mb_mapping->tab_registers[i] & 0xFF;
												   }
											   }
										   }
										   break;

		default:
										   rsp_length = response_exception(ctx, &sft,
												   MODBUS_EXCEPTION_ILLEGAL_FUNCTION,
												   rsp);
										   break;
	}

	return send_msg(ctx, rsp, rsp_length);
}

int modbus_reply_exception(modbus_t *ctx, const uint8_t *req,
		unsigned int exception_code)
{

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	int offset = ctx->backend->header_length;
	int slave = req[offset - 1];
	int function = req[offset];
	uint8_t rsp[MAX_MESSAGE_LENGTH];
	int rsp_length;
	int dummy_length = 99;
	sft_t sft;

	sft.slave = slave;
	sft.function = function + 0x80;;
	sft.t_id = ctx->backend->prepare_response_tid(req, &dummy_length);
	rsp_length = ctx->backend->build_response_basis(&sft, rsp);

	/* Positive exception code */
	if (exception_code < MODBUS_EXCEPTION_MAX) {
		rsp[rsp_length++] = exception_code;
		return send_msg(ctx, rsp, rsp_length);
	} else {
		errno = EINVAL;
		return -1;
	}
}

/* Reads IO status */
static int read_io_status(modbus_t *ctx, int function,
		int addr, int nb, uint8_t *dest)
{
	int rc;
	int req_length;

	uint8_t req[_MIN_REQ_LENGTH];
	uint8_t rsp[MAX_MESSAGE_LENGTH];

	req_length = ctx->backend->build_request_basis(ctx, function, addr, nb, req);

	rc = send_msg(ctx, req, req_length);
	if (rc > 0) {
		int i, temp, bit;
		int pos = 0;
		int offset;
		int offset_end;

		rc = _modbus_receive_msg(ctx, rsp, MSG_CONFIRMATION);
		if (rc == OTHER_ERROR)
			return OTHER_ERROR;
		else if (rc == TIMEOUT_ERROR)
			return TIMEOUT_ERROR;

		rc = check_confirmation(ctx, req, rsp, rc);
		if (rc == -1)
			return -1;

		offset = ctx->backend->header_length + 2;
		offset_end = offset + rc;
		for (i = offset; i < offset_end; i++) {
			/* Shift reg hi_byte to temp */
			temp = rsp[i];

			for (bit = 0x01; (bit & 0xff) && (pos < nb);) {
				dest[pos++] = (temp & bit) ? TRUE : FALSE;
				bit = bit << 1;
			}

		}
	}

	return rc;
}

/* Reads the boolean status of bits and sets the array elements
   in the destination to TRUE or FALSE (single bits). */
int modbus_read_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest)
{
	int rc;


	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	if (nb > MODBUS_MAX_READ_BITS) {
		if (ctx->debug) {
			fprintf(stderr,
					"ERROR Too many bits requested (%d > %d)\n",
					nb, MODBUS_MAX_READ_BITS);
		}
		errno = EMBMDATA;
		return OTHER_ERROR;
	}

	rc = read_io_status(ctx, _FC_READ_COILS, addr, nb, dest);

	if (rc == OTHER_ERROR)
		return OTHER_ERROR;
	else if (rc == TIMEOUT_ERROR)
		return TIMEOUT_ERROR;
	else
		return nb;
}


/* Same as modbus_read_bits but reads the remote device input table */
int modbus_read_input_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest)
{
	int rc;

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}

	if (nb > MODBUS_MAX_READ_BITS) {
		if (ctx->debug) {
			fprintf(stderr,
					"ERROR Too many discrete inputs requested (%d > %d)\n",
					nb, MODBUS_MAX_READ_BITS);
		}
		errno = EMBMDATA;
		return -1;
	}

	rc = read_io_status(ctx, _FC_READ_DISCRETE_INPUTS, addr, nb, dest);

	if (rc == OTHER_ERROR)
		return OTHER_ERROR;
	else if (rc == TIMEOUT_ERROR)
		return TIMEOUT_ERROR;
	else
		return nb;
}

/* Reads the data from a remove device and put that data into an array */
static int read_registers(modbus_t *ctx, int function, int addr, int nb,
		uint16_t *dest)
{
	int rc;
	int req_length;
	uint8_t req[_MIN_REQ_LENGTH];
	uint8_t rsp[MAX_MESSAGE_LENGTH];

	if (nb > MODBUS_MAX_READ_REGISTERS) {
		if (ctx->debug) {
			fprintf(stderr,
					"ERROR Too many registers requested (%d > %d)\n",
					nb, MODBUS_MAX_READ_REGISTERS);
		}
		errno = EMBMDATA;
		return -1;
	}

	req_length = ctx->backend->build_request_basis(ctx, function, addr, nb, req);

	rc = send_msg(ctx, req, req_length);
	DBG_PRINT("rc = %d\n",rc);
	if (rc > 0) {
		int offset;
		int i;
		rc = _modbus_receive_msg(ctx, rsp, MSG_CONFIRMATION);
		DBG_PRINT("rc = %d\n",rc);
		if (rc == OTHER_ERROR)
			return OTHER_ERROR;
		else if (rc == TIMEOUT_ERROR)
			return TIMEOUT_ERROR;

		rc = check_confirmation(ctx, req, rsp, rc);
		DBG_PRINT("rc = %d\n",rc);
		if (rc == -1)
			return -1;

		offset = ctx->backend->header_length;

		for (i = 0; i < rc; i++) {
			/* shift reg hi_byte to temp OR with lo_byte */
			dest[i] = (rsp[offset + 2 + (i << 1)] << 8) |
				rsp[offset + 3 + (i << 1)];
		}
        return rc;
    } else {
        return rc;
    }

}

/* Reads the holding registers of remote device and put the data into an
   array */
int modbus_read_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest)
{
	int status;

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}

	if (nb > MODBUS_MAX_READ_REGISTERS) {
		if (ctx->debug) {
			fprintf(stderr,
					"ERROR Too many registers requested (%d > %d)\n",
					nb, MODBUS_MAX_READ_REGISTERS);
		}
		errno = EMBMDATA;
		return -1;
	}

	status = read_registers(ctx, _FC_READ_HOLDING_REGISTERS,
			addr, nb, dest);
	return status;
}

/* Reads the input registers of remote device and put the data into an array */
int modbus_read_input_registers(modbus_t *ctx, int addr, int nb,
		uint16_t *dest)
{
	int status;

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}

	if (nb > MODBUS_MAX_READ_REGISTERS) {
		fprintf(stderr,
				"ERROR Too many input registers requested (%d > %d)\n",
				nb, MODBUS_MAX_READ_REGISTERS);
		errno = EMBMDATA;
		return -1;
	}

	status = read_registers(ctx, _FC_READ_INPUT_REGISTERS,
			addr, nb, dest);

	return status;
}

/* Write a value to the specified register of the remote device.
   Used by write_bit and write_register */
static int write_single(modbus_t *ctx, int function, int addr, int value)
{
	int rc;
	int req_length;
    uint8_t req[MAX_MESSAGE_LENGTH]; // [_MIN_REQ_LENGTH];

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}

	req_length = ctx->backend->build_request_basis(ctx, function, addr, value, req);

	rc = send_msg(ctx, req, req_length);
	if (rc > 0) {
		/* Used by write_bit and write_register */
        uint8_t rsp[MAX_MESSAGE_LENGTH]; // [_MIN_REQ_LENGTH];

		rc = _modbus_receive_msg(ctx, rsp, MSG_CONFIRMATION);
		if (rc == OTHER_ERROR)
			return OTHER_ERROR;
		else if (rc == TIMEOUT_ERROR)
			return TIMEOUT_ERROR;

		rc = check_confirmation(ctx, req, rsp, rc);
	}

	return rc;
}

/* Turns ON or OFF a single bit of the remote device */
int modbus_write_bit(modbus_t *ctx, int addr, int status)
{

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	return write_single(ctx, _FC_WRITE_SINGLE_COIL, addr,
			status ? 0xFF00 : 0);
}

/* Writes a value in one register of the remote device */
int modbus_write_register(modbus_t *ctx, int addr, int value)
{

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	return write_single(ctx, _FC_WRITE_SINGLE_REGISTER, addr, value);
}

/* Write the bits of the array in the remote device */
int modbus_write_bits(modbus_t *ctx, int addr, int nb, const uint8_t *src)
{
	int rc;
	int i;
	int byte_count;
	int req_length;
	int bit_check = 0;
	int pos = 0;

	uint8_t req[MAX_MESSAGE_LENGTH];

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}

	if (nb > MODBUS_MAX_WRITE_BITS) {
		if (ctx->debug) {
			fprintf(stderr, "ERROR Writing too many bits (%d > %d)\n",
					nb, MODBUS_MAX_WRITE_BITS);
		}
		errno = EMBMDATA;
		return -1;
	}

	req_length = ctx->backend->build_request_basis(ctx,
			_FC_WRITE_MULTIPLE_COILS,
			addr, nb, req);
	byte_count = (nb / 8) + ((nb % 8) ? 1 : 0);
	req[req_length++] = byte_count;

	for (i = 0; i < byte_count; i++) {
		int bit;

		bit = 0x01;
		req[req_length] = 0;

		while ((bit & 0xFF) && (bit_check++ < nb)) {
			if (src[pos++])
				req[req_length] |= bit;
			else
				req[req_length] &=~ bit;

			bit = bit << 1;
		}
		req_length++;
	}

	rc = send_msg(ctx, req, req_length);
	if (rc > 0) {
		uint8_t rsp[MAX_MESSAGE_LENGTH];

		rc = _modbus_receive_msg(ctx, rsp, MSG_CONFIRMATION);
		if (rc == OTHER_ERROR)
			return OTHER_ERROR;
		else if (rc == TIMEOUT_ERROR)
			return TIMEOUT_ERROR;

		rc = check_confirmation(ctx, req, rsp, rc);
	}


	return rc;
}

/* Write the values from the array to the registers of the remote device */
int modbus_write_registers(modbus_t *ctx, int addr, int nb, const uint16_t *src)
{
	int rc;
	int i;
	int req_length;
	int byte_count;

	uint8_t req[MAX_MESSAGE_LENGTH];

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}

	if (nb > MODBUS_MAX_WRITE_REGISTERS) {
		if (ctx->debug) {
			fprintf(stderr,
					"ERROR Trying to write to too many registers (%d > %d)\n",
					nb, MODBUS_MAX_WRITE_REGISTERS);
		}
		errno = EMBMDATA;
		return -1;
	}

	req_length = ctx->backend->build_request_basis(ctx,
			_FC_WRITE_MULTIPLE_REGISTERS,
			addr, nb, req);
	byte_count = nb * 2;
	req[req_length++] = byte_count;

	for (i = 0; i < nb; i++) {
		req[req_length++] = src[i] >> 8;
		req[req_length++] = src[i] & 0x00FF;
	}

	rc = send_msg(ctx, req, req_length);
	if (rc > 0) {
		uint8_t rsp[MAX_MESSAGE_LENGTH];

		rc = _modbus_receive_msg(ctx, rsp, MSG_CONFIRMATION);
		if (rc == OTHER_ERROR)
			return OTHER_ERROR;
		else if (rc == TIMEOUT_ERROR)
			return TIMEOUT_ERROR;

		rc = check_confirmation(ctx, req, rsp, rc);
	}

	return rc;
}

int modbus_mask_write_register(modbus_t *ctx, int addr, uint16_t and_mask, uint16_t or_mask)
{
	int rc;
	int req_length;
	uint8_t req[_MIN_REQ_LENGTH];

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}

	req_length = ctx->backend->build_request_basis(ctx, _FC_MASK_WRITE_REGISTER, addr, 0, req);

	/* HACKISH, count is not used */
	req_length -=2;

	req[req_length++] = and_mask >> 8;
	req[req_length++] = and_mask & 0x00ff;
	req[req_length++] = or_mask >> 8;
	req[req_length++] = or_mask & 0x00ff;

	rc = send_msg(ctx, req, req_length);
	if (rc > 0) {
		/* Used by write_bit and write_register */
		uint8_t rsp[_MIN_REQ_LENGTH];

		rc = _modbus_receive_msg(ctx, rsp, MSG_CONFIRMATION);
		if (rc == OTHER_ERROR)
			return OTHER_ERROR;
		else if (rc == TIMEOUT_ERROR)
			return TIMEOUT_ERROR;

		rc = check_confirmation(ctx, req, rsp, rc);
	}

	return rc;
}

/* Write multiple registers from src array to remote device and read multiple
   registers from remote device to dest array. */
int modbus_write_and_read_registers(modbus_t *ctx,
		int write_addr, int write_nb, const uint16_t *src,
		int read_addr, int read_nb, uint16_t *dest)

{
	int rc;
	int req_length;
	int i;
	int byte_count;
	uint8_t req[MAX_MESSAGE_LENGTH];
	uint8_t rsp[MAX_MESSAGE_LENGTH];

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}

	if (write_nb > MODBUS_MAX_RW_WRITE_REGISTERS) {
		if (ctx->debug) {
			fprintf(stderr,
					"ERROR Too many registers to write (%d > %d)\n",
					write_nb, MODBUS_MAX_RW_WRITE_REGISTERS);
		}
		errno = EMBMDATA;
		return -1;
	}

	if (read_nb > MODBUS_MAX_READ_REGISTERS) {
		if (ctx->debug) {
			fprintf(stderr,
					"ERROR Too many registers requested (%d > %d)\n",
					read_nb, MODBUS_MAX_READ_REGISTERS);
		}
		errno = EMBMDATA;
		return -1;
	}
	req_length = ctx->backend->build_request_basis(ctx,
			_FC_WRITE_AND_READ_REGISTERS,
			read_addr, read_nb, req);

	req[req_length++] = write_addr >> 8;
	req[req_length++] = write_addr & 0x00ff;
	req[req_length++] = write_nb >> 8;
	req[req_length++] = write_nb & 0x00ff;
	byte_count = write_nb * 2;
	req[req_length++] = byte_count;

	for (i = 0; i < write_nb; i++) {
		req[req_length++] = src[i] >> 8;
		req[req_length++] = src[i] & 0x00FF;
	}

	rc = send_msg(ctx, req, req_length);
	if (rc > 0) {
		int offset;

		rc = _modbus_receive_msg(ctx, rsp, MSG_CONFIRMATION);
		if (rc == OTHER_ERROR)
			return OTHER_ERROR;
		else if (rc == TIMEOUT_ERROR)
			return TIMEOUT_ERROR;

		rc = check_confirmation(ctx, req, rsp, rc);
		if (rc == -1)
			return -1;

		offset = ctx->backend->header_length;

		/* If rc is negative, the loop is jumped ! */
		for (i = 0; i < rc; i++) {
			/* shift reg hi_byte to temp OR with lo_byte */
			dest[i] = (rsp[offset + 2 + (i << 1)] << 8) |
				rsp[offset + 3 + (i << 1)];
		}
	}

	return rc;
}

/* Send a request to get the slave ID of the device (only available in serial
   communication). */
int modbus_report_slave_id(modbus_t *ctx, uint8_t *dest)
{
	int rc;
	int req_length;
	uint8_t req[_MIN_REQ_LENGTH];

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}

	req_length = ctx->backend->build_request_basis(ctx, _FC_REPORT_SLAVE_ID,
			0, 0, req);

	/* HACKISH, addr and count are not used */
	req_length -= 4;

	rc = send_msg(ctx, req, req_length);
	if (rc > 0) {
		int i;
		int offset;
		uint8_t rsp[MAX_MESSAGE_LENGTH];

		rc = _modbus_receive_msg(ctx, rsp, MSG_CONFIRMATION);
		if (rc == OTHER_ERROR)
			return OTHER_ERROR;
		else if (rc == TIMEOUT_ERROR)
			return TIMEOUT_ERROR;

		rc = check_confirmation(ctx, req, rsp, rc);
		if (rc == -1)
			return -1;

		offset = ctx->backend->header_length + 2;

		/* Byte count, slave id, run indicator status,
		   additional data */
		for (i=0; i < rc; i++) {
			dest[i] = rsp[offset + i];
		}
	}

	return rc;
}

void _modbus_init_common(modbus_t *ctx)
{
	/* Slave and socket are initialized to -1 */
	ctx->slave = -1;
	ctx->s = -1;

	ctx->debug = FALSE;
	ctx->error_recovery = MODBUS_ERROR_RECOVERY_NONE;

	ctx->response_timeout.tv_sec = 0;
	ctx->response_timeout.tv_usec = _RESPONSE_TIMEOUT;

	ctx->byte_timeout.tv_sec = 0;
	ctx->byte_timeout.tv_usec = _BYTE_TIMEOUT;
}

/* Define the slave number */
int modbus_set_slave(modbus_t *ctx, int slave)
{

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	return ctx->backend->set_slave(ctx, slave);
}

int modbus_set_error_recovery(modbus_t *ctx,
		modbus_error_recovery_mode error_recovery)
{

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	/* The type of modbus_error_recovery_mode is unsigned enum */
	ctx->error_recovery = (uint8_t) error_recovery;
	return 0;
}

void modbus_set_socket(modbus_t *ctx, int socket)
{
	ctx->s = socket;
}

int modbus_get_socket(modbus_t *ctx)
{

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	return ctx->s;
}

/* Get the timeout interval used to wait for a response */
void modbus_get_response_timeout(modbus_t *ctx, struct timeval *timeout)
{
	*timeout = ctx->response_timeout;
}

void modbus_set_response_timeout(modbus_t *ctx, const struct timeval *timeout)
{
	ctx->response_timeout = *timeout;
}

/* Get the timeout interval between two consecutive bytes of a message */
void modbus_get_byte_timeout(modbus_t *ctx, struct timeval *timeout)
{
	*timeout = ctx->byte_timeout;
}

void modbus_set_byte_timeout(modbus_t *ctx, const struct timeval *timeout)
{
	ctx->byte_timeout = *timeout;
}

int modbus_get_header_length(modbus_t *ctx)
{
	return ctx->backend->header_length;
}

int modbus_connect(modbus_t *ctx)
{
    ctx->error_recovery = MODBUS_ERROR_RECOVERY_PROTOCOL; //MODBUS_ERROR_RECOVERY_LINK;
	return ctx->backend->connect(ctx);
}

void modbus_close(modbus_t *ctx)
{
	if (ctx == NULL)
		return;

	ctx->backend->close(ctx);
}

void modbus_free(modbus_t *ctx)
{
	if (ctx == NULL)
		return;

	ctx->backend->free(ctx);
	if (ctx == ctx_rtu)
	{
		ctx_rtu = NULL;
	}
	else if (ctx == ctx_tcp)
	{
		ctx_tcp = NULL;
	}
	else if (ctx == ctx_tcp_pi)
	{
		ctx_tcp_pi = NULL;
	}
	else if (ctx == ctx_tcprtu)
	{
		ctx_tcprtu = NULL;
	}
}

void modbus_set_debug(modbus_t *ctx, int boolean)
{
	ctx->debug = boolean;
}

/* Allocates 4 arrays to store bits, input bits, registers and inputs
   registers. The pointers are stored in modbus_mapping structure.

   The modbus_mapping_new() function shall return the new allocated structure if
   successful. Otherwise it shall return NULL and set errno to ENOMEM. */
modbus_mapping_t* modbus_mapping_new(int nb_bits, int nb_input_bits,
		int nb_registers, int nb_input_registers)
{
	modbus_mapping_t *mb_mapping;

	mb_mapping = (modbus_mapping_t *)malloc(sizeof(modbus_mapping_t));
	if (mb_mapping == NULL) {
		return NULL;
	}

	/* 0X */
	mb_mapping->nb_bits = nb_bits;
	if (nb_bits == 0) {
		mb_mapping->tab_bits = NULL;
	} else {
		/* Negative number raises a POSIX error */
		mb_mapping->tab_bits =
			(uint8_t *) malloc(nb_bits * sizeof(uint8_t));
		if (mb_mapping->tab_bits == NULL) {
			free(mb_mapping);
			return NULL;
		}
		memset(mb_mapping->tab_bits, 0, nb_bits * sizeof(uint8_t));
	}

	/* 1X */
	mb_mapping->nb_input_bits = nb_input_bits;
	if (nb_input_bits == 0) {
		mb_mapping->tab_input_bits = NULL;
	} else {
		mb_mapping->tab_input_bits =
			(uint8_t *) malloc(nb_input_bits * sizeof(uint8_t));
		if (mb_mapping->tab_input_bits == NULL) {
			free(mb_mapping->tab_bits);
			free(mb_mapping);
			return NULL;
		}
		memset(mb_mapping->tab_input_bits, 0, nb_input_bits * sizeof(uint8_t));
	}

	/* 4X */
	mb_mapping->nb_registers = nb_registers;
	if (nb_registers == 0) {
		mb_mapping->tab_registers = NULL;
	} else {
		mb_mapping->tab_registers =
			(uint16_t *) malloc(nb_registers * sizeof(uint16_t));
		if (mb_mapping->tab_registers == NULL) {
			free(mb_mapping->tab_input_bits);
			free(mb_mapping->tab_bits);
			free(mb_mapping);
			return NULL;
		}
		memset(mb_mapping->tab_registers, 0, nb_registers * sizeof(uint16_t));
	}

	/* 3X */
	mb_mapping->nb_input_registers = nb_input_registers;
	if (nb_input_registers == 0) {
		mb_mapping->tab_input_registers = NULL;
	} else {
		mb_mapping->tab_input_registers =
			(uint16_t *) malloc(nb_input_registers * sizeof(uint16_t));
		if (mb_mapping->tab_input_registers == NULL) {
			free(mb_mapping->tab_registers);
			free(mb_mapping->tab_input_bits);
			free(mb_mapping->tab_bits);
			free(mb_mapping);
			return NULL;
		}
		memset(mb_mapping->tab_input_registers, 0,
				nb_input_registers * sizeof(uint16_t));
	}

	return mb_mapping;
}

/* Frees the 4 arrays */
void modbus_mapping_free(modbus_mapping_t *mb_mapping)
{
	if (mb_mapping == NULL) {
		return;
	}

	free(mb_mapping->tab_input_registers);
	free(mb_mapping->tab_registers);
	free(mb_mapping->tab_input_bits);
	free(mb_mapping->tab_bits);
	free(mb_mapping);
}

#ifndef HAVE_STRLCPY
/*
 * Function strlcpy was originally developed by
 * Todd C. Miller <Todd.Miller@courtesan.com> to simplify writing secure code.
 * See ftp://ftp.openbsd.org/pub/OpenBSD/src/lib/libc/string/strlcpy.3
 * for more information.
 *
 * Thank you Ulrich Drepper... not!
 *
 * Copy src to string dest of size dest_size.  At most dest_size-1 characters
 * will be copied.  Always NUL terminates (unless dest_size == 0).  Returns
 * strlen(src); if retval >= dest_size, truncation occurred.
 */
size_t strlcpy(char *dest, const char *src, size_t dest_size)
{
	register char *d = dest;
	register const char *s = src;
	register size_t n = dest_size;

	/* Copy as many bytes as will fit */
	if (n != 0 && --n != 0) {
		do {
			if ((*d++ = *s++) == 0)
				break;
		} while (--n != 0);
	}

	/* Not enough room in dest, add NUL and traverse rest of src */
	if (n == 0) {
		if (dest_size != 0)
			*d = '\0'; /* NUL-terminate dest */
		while (*s++)
			;
	}

	return (s - src - 1); /* count does not include NUL */
}
#endif

/****************************************
 *
 * modbus-data.c
 *
 *
 ****************************************/

/* Sets many bits from a single byte value (all 8 bits of the byte value are
   set) */
void modbus_set_bits_from_byte(uint8_t *dest, int index, const uint8_t value)
{
	int i;

	for (i=0; i < 8; i++) {
		dest[index+i] = (value & (1 << i)) ? 1 : 0;
	}
}

/* Sets many bits from a table of bytes (only the bits between index and
   index + nb_bits are set) */
void modbus_set_bits_from_bytes(uint8_t *dest, int index, unsigned int nb_bits,
		const uint8_t *tab_byte)
{
	unsigned int i;
	int shift = 0;

	for (i = index; i < index + nb_bits; i++) {
		dest[i] = tab_byte[(i - index) / 8] & (1 << shift) ? 1 : 0;
		/* gcc doesn't like: shift = (++shift) % 8; */
		shift++;
		shift %= 8;
	}
}

/* Gets the byte value from many bits.
   To obtain a full byte, set nb_bits to 8. */
uint8_t modbus_get_byte_from_bits(const uint8_t *src, int index,
		unsigned int nb_bits)
{
	unsigned int i;
	uint8_t value = 0;

	if (nb_bits > 8) {
		/* Assert is ignored if NDEBUG is set */
		assert(nb_bits < 8);
		nb_bits = 8;
	}

	for (i=0; i < nb_bits; i++) {
		value |= (src[index+i] << i);
	}

	return value;
}

/* Get a float from 4 bytes in Modbus format (ABCD) */
float modbus_get_float(const uint16_t *src)
{
	float f;
	uint32_t i;
	uint32_t i_sw;

	i = (((uint32_t)src[1]) << 16) + src[0];

	i_sw = ABCD2FLOAT(i);

	memcpy(&f, &i, sizeof(float));

	return f;
}

/* Get a float from 4 bytes in inversed Modbus format (DCBA) */
float modbus_get_float_dcba(const uint16_t *src)
{
	float f;
	uint32_t i;
	uint32_t i_sw;

	i = (((uint32_t)src[1]) << 16) + src[0];

	i_sw = DCBA2FLOAT(i);

	memcpy(&f, &i, sizeof(float));

	return f;
}

/* Get a float from 4 bytes in inversed Modbus format (BADC) */
float modbus_get_float_badc(const uint16_t *src)
{
	float f;
	uint32_t i;
	uint32_t i_sw;

	i = (((uint32_t)src[1]) << 16) + src[0];

	i_sw = BADC2FLOAT(i);

	memcpy(&f, &i, sizeof(float));

	return f;
}

/* Get a float from 4 bytes in inversed Modbus format (CDAB) */
float modbus_get_float_cdab(const uint16_t *src)
{
	float f;
	uint32_t i;
	uint32_t i_sw;

	i = (((uint32_t)src[1]) << 16) + src[0];

	i_sw = CDAB2FLOAT(i);

	memcpy(&f, &i, sizeof(float));

	return f;
}

/* Set a float to 4 bytes in Modbus format (ABCD) */
void modbus_set_float(float f, uint16_t *dest)
{
	uint32_t i;
	uint32_t i_sw;

	memcpy(&i, &f, sizeof(uint32_t));
	i_sw = FLOAT2ABCD(i);
	dest[0] = (uint16_t)i_sw;
	dest[1] = (uint16_t)(i_sw >> 16);
}

/* Set a float to 4 bytes in inversed Modbus format (DCBA) */
void modbus_set_float_dcba(float f, uint16_t *dest)
{
	uint32_t i;
	uint32_t i_sw;

	memcpy(&i, &f, sizeof(uint32_t));
	i_sw = FLOAT2DCBA(i);
	dest[0] = (uint16_t)i_sw;
	dest[1] = (uint16_t)(i_sw >> 16);
}

/* Set a float to 4 bytes in inversed Modbus format (BADC) */
void modbus_set_float_badc(float f, uint16_t *dest)
{
	uint32_t i;
	uint32_t i_sw;

	memcpy(&i, &f, sizeof(uint32_t));
	i_sw = FLOAT2BADC(i);
	dest[0] = (uint16_t)i_sw;
	dest[1] = (uint16_t)(i_sw >> 16);
}

/* Set a float to 4 bytes in inversed Modbus format (CDAB) */
void modbus_set_float_cdab(float f, uint16_t *dest)
{
	uint32_t i;
	uint32_t i_sw;

	memcpy(&i, &f, sizeof(uint32_t));
	i_sw = FLOAT2CDAB(i);
	dest[0] = (uint16_t)i_sw;
	dest[1] = (uint16_t)(i_sw >> 16);
}
/*****************************************
 *
 *   modbus-rtu.c
 *
 *
 ******************************************/
#if HAVE_DECL_TIOCSRS485 || HAVE_DECL_TIOCM_RTS
#include <sys/ioctl.h>
#endif

#if HAVE_DECL_TIOCSRS485
#include <linux/serial.h>
#endif

/* Table of CRC values for high-order byte */
static const uint8_t table_crc_hi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

/* Define the slave ID of the remote device to talk in master mode or set the
 * internal slave ID in slave mode */
static int _modbus_set_slave(modbus_t *ctx, int slave)
{
	/* Broadcast address is 0 (MODBUS_BROADCAST_ADDRESS) */
	if (slave >= 0 && slave <= 255) {
		ctx->slave = slave;
	} else {
		errno = EINVAL;
		return -1;
	}

	return 0;
}

/* Builds a RTU request header */
static int _modbus_rtu_build_request_basis(modbus_t *ctx, int function,
		int addr, int nb,
		uint8_t *req)
{
	assert(ctx->slave != -1);
	req[0] = ctx->slave;
	req[1] = function;
	req[2] = addr >> 8;
	req[3] = addr & 0x00ff;
	req[4] = nb >> 8;
	req[5] = nb & 0x00ff;

	return _MODBUS_RTU_PRESET_REQ_LENGTH;
}

/* Builds a RTU response header */
static int _modbus_rtu_build_response_basis(sft_t *sft, uint8_t *rsp)
{
	/* In this case, the slave is certainly valid because a check is already
	 * done in _modbus_rtu_listen */
	rsp[0] = sft->slave;
	rsp[1] = sft->function;

	return _MODBUS_RTU_PRESET_RSP_LENGTH;
}

static uint16_t crc16(uint8_t *buffer, uint16_t buffer_length)
{
	uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
	uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
	unsigned int i; /* will index into CRC lookup */

	/* pass through message buffer */
	while (buffer_length--) {
		i = crc_hi ^ *buffer++; /* calculate the CRC  */
		crc_hi = crc_lo ^ table_crc_hi[i];
		crc_lo = table_crc_lo[i];
	}

	return (crc_hi << 8 | crc_lo);
}

static int _modbus_rtu_prepare_response_tid(__attribute((unused)) const uint8_t *req, int *req_length)
{
	(*req_length) -= _MODBUS_RTU_CHECKSUM_LENGTH;
	/* No TID */
	return 0;
}

static int _modbus_rtu_send_msg_pre(uint8_t *req, int req_length)
{
	uint16_t crc = crc16(req, req_length);
	req[req_length++] = crc >> 8;
	req[req_length++] = crc & 0x00FF;

	return req_length;
}

#if defined(_WIN32)

/* This simple implementation is sort of a substitute of the select() call,
 * working this way: the win32_ser_select() call tries to read some data from
 * the serial port, setting the timeout as the select() call would. Data read is
 * stored into the receive buffer, that is then consumed by the win32_ser_read()
 * call.  So win32_ser_select() does both the event waiting and the reading,
 * while win32_ser_read() only consumes the receive buffer.
 */

static void win32_ser_init(struct win32_ser *ws) {
	/* Clear everything */
	memset(ws, 0x00, sizeof(struct win32_ser));

	/* Set file handle to invalid */
	ws->fd = INVALID_HANDLE_VALUE;
}

/* FIXME Try to remove length_to_read -> max_len argument, only used by win32 */
static int win32_ser_select(struct win32_ser *ws, int max_len,
		struct timeval *tv) {
	COMMTIMEOUTS comm_to;
	unsigned int msec = 0;

	/* Check if some data still in the buffer to be consumed */
	if (ws->n_bytes > 0) {
		return 1;
	}

	/* Setup timeouts like select() would do.
	   FIXME Please someone on Windows can look at this?
	   Does it possible to use WaitCommEvent?
	   When tv is NULL, MAXDWORD isn't infinite!
	 */
	if (tv == NULL) {
		msec = MAXDWORD;
	} else {
		msec = tv->tv_sec * 1000 + tv->tv_usec / 1000;
		if (msec < 1)
			msec = 1;
	}

	comm_to.ReadIntervalTimeout = msec;
	comm_to.ReadTotalTimeoutMultiplier = 0;
	comm_to.ReadTotalTimeoutConstant = msec;
	comm_to.WriteTotalTimeoutMultiplier = 0;
	comm_to.WriteTotalTimeoutConstant = 1000;
	SetCommTimeouts(ws->fd, &comm_to);

	/* Read some bytes */
	if ((max_len > PY_BUF_SIZE) || (max_len < 0)) {
		max_len = PY_BUF_SIZE;
	}

	if (ReadFile(ws->fd, &ws->buf, max_len, &ws->n_bytes, NULL)) {
		/* Check if some bytes available */
		if (ws->n_bytes > 0) {
			/* Some bytes read */
			return 1;
		} else {
			/* Just timed out */
			return 0;
		}
	} else {
		/* Some kind of error */
		return -1;
	}
}

static int win32_ser_read(struct win32_ser *ws, uint8_t *p_msg,
		unsigned int max_len) {
	unsigned int n = ws->n_bytes;

	if (max_len < n) {
		n = max_len;
	}

	if (n > 0) {
		memcpy(p_msg, ws->buf, n);
	}

	ws->n_bytes -= n;

	return n;
}
#endif


static void _modbus_rtu_ioctl_rts(int fd, int on)
{
#if HAVE_DECL_TIOCM_RTS
	int flags;

	ioctl(fd, TIOCMGET, &flags);
	if (on) {
		flags |= TIOCM_RTS;
	} else {
		flags &= ~TIOCM_RTS;
	}
	ioctl(fd, TIOCMSET, &flags);
#endif
}


static ssize_t _modbus_rtu_send(modbus_t *ctx, const uint8_t *req, int req_length)
{
#if defined(_WIN32)
	modbus_rtu_t *ctx_rtu = ctx->backend_data;
	DWORD n_bytes = 0;
	return (WriteFile(ctx_rtu->w_ser.fd, req, req_length, &n_bytes, NULL)) ? n_bytes : -1;
#else
#if HAVE_DECL_TIOCM_RTS
	modbus_rtu_t *ctx_rtu = ctx->backend_data;
	if (ctx_rtu->rts != MODBUS_RTU_RTS_NONE) {
		ssize_t size;

		if (ctx->debug) {
			fprintf(stderr, "Sending request using RTS signal\n");
		}

		_modbus_rtu_ioctl_rts(ctx->s, ctx_rtu->rts == MODBUS_RTU_RTS_UP);
		usleep(_MODBUS_RTU_TIME_BETWEEN_RTS_SWITCH);

		size = write(ctx->s, req, req_length);

		usleep(ctx_rtu->onebyte_time * req_length + _MODBUS_RTU_TIME_BETWEEN_RTS_SWITCH);
		_modbus_rtu_ioctl_rts(ctx->s, ctx_rtu->rts != MODBUS_RTU_RTS_UP);

		return size;
	} else {
#endif
		return write(ctx->s, req, req_length);
#if HAVE_DECL_TIOCM_RTS
	}
#endif
#endif
}

static int _modbus_rtu_receive(modbus_t *ctx, uint8_t *req)
{
	int rc;
	modbus_rtu_t *ctx_rtu = ctx->backend_data;

	if (ctx_rtu->confirmation_to_ignore) {
		_modbus_receive_msg(ctx, req, MSG_CONFIRMATION);
		/* Ignore errors and reset the flag */
		ctx_rtu->confirmation_to_ignore = FALSE;
		rc = 0;
		if (ctx->debug) {
			fprintf(stderr,"Confirmation to ignore\n");
		}
	} else {
		rc = _modbus_receive_msg(ctx, req, MSG_INDICATION);
		if (rc == 0) {
			/* The next expected message is a confirmation to ignore */
			ctx_rtu->confirmation_to_ignore = TRUE;
		}
	}
	return rc;
}

static ssize_t _modbus_rtu_recv(modbus_t *ctx, uint8_t *rsp, int rsp_length)
{
#if defined(_WIN32)
	return win32_ser_read(&((modbus_rtu_t *)ctx->backend_data)->w_ser, rsp, rsp_length);
#else
	return read(ctx->s, rsp, rsp_length);
#endif
}

static int _modbus_rtu_flush(modbus_t *);

static int _modbus_rtu_pre_check_confirmation(modbus_t *ctx, const uint8_t *req,
		const uint8_t *rsp, __attribute__((unused)) int rsp_length)
{
	/* Check responding slave is the slave we requested (except for broacast
	 * request) */
	if (req[0] != 0 && req[0] != rsp[0]) {
		if (ctx->debug) {
			fprintf(stderr,
					"The responding slave %d isn't the requested slave %d",
					rsp[0], req[0]);
		}
		errno = EMBBADSLAVE;
		return -1;
	} else {
		return 0;
	}
}

/* The check_crc16 function shall return 0 is the message is ignored and the
   message length if the CRC is valid. Otherwise it shall return -1 and set
   errno to EMBADCRC. */
static int _modbus_rtu_check_integrity(modbus_t *ctx, uint8_t *msg,
		const int msg_length)
{
	uint16_t crc_calculated;
	uint16_t crc_received;
	int slave = msg[0];

	/* Filter on the Modbus unit identifier (slave) in RTU mode */
	if (slave != ctx->slave && slave != MODBUS_BROADCAST_ADDRESS) {
		/* Ignores the request (not for me) */
		if (ctx->debug) {
			fprintf(stderr,"Request for slave %d ignored (not %d)\n", slave, ctx->slave);
		}

		return 0;
	}

	crc_calculated = crc16(msg, msg_length - 2);
	crc_received = (msg[msg_length - 2] << 8) | msg[msg_length - 1];

	/* Check CRC of msg */
	if (crc_calculated == crc_received) {
		return msg_length;
	} else {
		if (ctx->debug) {
			fprintf(stderr, "ERROR CRC received %0X != CRC calculated %0X\n",
					crc_received, crc_calculated);
		}

		if (ctx->error_recovery & MODBUS_ERROR_RECOVERY_PROTOCOL) {
			_modbus_rtu_flush(ctx);
		}
		errno = EMBBADCRC;
		return OTHER_ERROR;
	}
}

/* Sets up a serial port for RTU communications */
static int _modbus_rtu_connect(modbus_t *ctx)
{
#if defined(_WIN32)
	DCB dcb;
#else
	struct termios tios;
	speed_t speed;
	int flags;
#endif
	modbus_rtu_t *ctx_rtu = ctx->backend_data;

	if (ctx->debug) {
		fprintf(stderr,"Opening %s at %d bauds (%c, %d, %d)\n",
				ctx_rtu->device, ctx_rtu->baud, ctx_rtu->parity,
				ctx_rtu->data_bit, ctx_rtu->stop_bit);
	}

#if defined(_WIN32)
	/* Some references here:
	 * http://msdn.microsoft.com/en-us/library/aa450602.aspx
	 */
	win32_ser_init(&ctx_rtu->w_ser);

	/* ctx_rtu->device should contain a string like "COMxx:" xx being a decimal
	 * number */
	ctx_rtu->w_ser.fd = CreateFileA(ctx_rtu->device,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

	/* Error checking */
	if (ctx_rtu->w_ser.fd == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "ERROR Can't open the device %s (LastError %d)\n",
				ctx_rtu->device, (int)GetLastError());
		return -1;
	}

	/* Save params */
	ctx_rtu->old_dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(ctx_rtu->w_ser.fd, &ctx_rtu->old_dcb)) {
		fprintf(stderr, "ERROR Error getting configuration (LastError %d)\n",
				(int)GetLastError());
		CloseHandle(ctx_rtu->w_ser.fd);
		ctx_rtu->w_ser.fd = INVALID_HANDLE_VALUE;
		return -1;
	}

	/* Build new configuration (starting from current settings) */
	dcb = ctx_rtu->old_dcb;

	/* Speed setting */
	switch (ctx_rtu->baud) {
		case 110:
			dcb.BaudRate = CBR_110;
			break;
		case 300:
			dcb.BaudRate = CBR_300;
			break;
		case 600:
			dcb.BaudRate = CBR_600;
			break;
		case 1200:
			dcb.BaudRate = CBR_1200;
			break;
		case 2400:
			dcb.BaudRate = CBR_2400;
			break;
		case 4800:
			dcb.BaudRate = CBR_4800;
			break;
		case 9600:
			dcb.BaudRate = CBR_9600;
			break;
		case 14400:
			dcb.BaudRate = CBR_14400;
			break;
		case 19200:
			dcb.BaudRate = CBR_19200;
			break;
		case 38400:
			dcb.BaudRate = CBR_38400;
			break;
		case 57600:
			dcb.BaudRate = CBR_57600;
			break;
		case 115200:
			dcb.BaudRate = CBR_115200;
			break;
		case 230400:
			/* CBR_230400 - not defined */
			dcb.BaudRate = 230400;
			break;
		case 250000:
			dcb.BaudRate = 250000;
			break;
		case 460800:
			dcb.BaudRate = 460800;
			break;
		case 500000:
			dcb.BaudRate = 500000;
			break;
		case 921600:
			dcb.BaudRate = 921600;
			break;
		case 1000000:
			dcb.BaudRate = 1000000;
			break;
		default:
			dcb.BaudRate = CBR_9600;
			fprintf(stderr,"WARNING Unknown baud rate %d for %s (B9600 used)\n",
					ctx_rtu->baud, ctx_rtu->device);
	}

	/* Data bits */
	switch (ctx_rtu->data_bit) {
		case 5:
			dcb.ByteSize = 5;
			break;
		case 6:
			dcb.ByteSize = 6;
			break;
		case 7:
			dcb.ByteSize = 7;
			break;
		case 8:
		default:
			dcb.ByteSize = 8;
			break;
	}

	/* Stop bits */
	if (ctx_rtu->stop_bit == 1)
		dcb.StopBits = ONESTOPBIT;
	else /* 2 */
		dcb.StopBits = TWOSTOPBITS;

	/* Parity */
	if (ctx_rtu->parity == 'N') {
		dcb.Parity = NOPARITY;
		dcb.fParity = FALSE;
	} else if (ctx_rtu->parity == 'E') {
		dcb.Parity = EVENPARITY;
		dcb.fParity = TRUE;
	} else {
		/* odd */
		dcb.Parity = ODDPARITY;
		dcb.fParity = TRUE;
	}

	/* Hardware handshaking left as default settings retrieved */

	/* No software handshaking */
	dcb.fTXContinueOnXoff = TRUE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;

	/* Binary mode (it's the only supported on Windows anyway) */
	dcb.fBinary = TRUE;

	/* Don't want errors to be blocking */
	dcb.fAbortOnError = FALSE;

	/* TODO: any other flags!? */

	/* Setup port */
	if (!SetCommState(ctx_rtu->w_ser.fd, &dcb)) {
		fprintf(stderr, "ERROR Error setting new configuration (LastError %d)\n",
				(int)GetLastError());
		CloseHandle(ctx_rtu->w_ser.fd);
		ctx_rtu->w_ser.fd = INVALID_HANDLE_VALUE;
		return -1;
	}
#else
	/* The O_NOCTTY flag tells UNIX that this program doesn't want
	   to be the "controlling terminal" for that port. If you
	   don't specify this then any input (such as keyboard abort
	   signals and so forth) will affect your process

	   Timeouts are ignored in canonical input mode or when the
	   NDELAY option is set on the file via open or fcntl */
	flags = O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL;
#ifdef O_CLOEXEC
	flags |= O_CLOEXEC;
#endif

	ctx->s = open(ctx_rtu->device, flags);
	if (ctx->s == -1) {
		fprintf(stderr, "ERROR Can't open the device %s (%s)\n",
				ctx_rtu->device, strerror(errno));
		return -1;
	}

	/* Save */
	tcgetattr(ctx->s, &(ctx_rtu->old_tios));

	memset(&tios, 0, sizeof(struct termios));

	/* C_ISPEED     Input baud (new interface)
	   C_OSPEED     Output baud (new interface)
	 */
	switch (ctx_rtu->baud) {
		case 110:
			speed = B110;
			break;
		case 300:
			speed = B300;
			break;
		case 600:
			speed = B600;
			break;
		case 1200:
			speed = B1200;
			break;
		case 2400:
			speed = B2400;
			break;
		case 4800:
			speed = B4800;
			break;
		case 9600:
			speed = B9600;
			break;
		case 19200:
			speed = B19200;
			break;
		case 38400:
			speed = B38400;
			break;
#ifdef B57600
		case 57600:
			speed = B57600;
			break;
#endif
#ifdef B115200
		case 115200:
			speed = B115200;
			break;
#endif
#ifdef B230400
		case 230400:
			speed = B230400;
			break;
#endif
#ifdef B460800
		case 460800:
			speed = B460800;
			break;
#endif
#ifdef B500000
		case 500000:
			speed = B500000;
			break;
#endif
#ifdef B576000
		case 576000:
			speed = B576000;
			break;
#endif
#ifdef B921600
		case 921600:
			speed = B921600;
			break;
#endif
#ifdef B1000000
		case 1000000:
			speed = B1000000;
			break;
#endif
#ifdef B1152000
		case 1152000:
			speed = B1152000;
			break;
#endif
#ifdef B1500000
		case 1500000:
			speed = B1500000;
			break;
#endif
#ifdef B2500000
		case 2500000:
			speed = B2500000;
			break;
#endif
#ifdef B3000000
		case 3000000:
			speed = B3000000;
			break;
#endif
#ifdef B3500000
		case 3500000:
			speed = B3500000;
			break;
#endif
#ifdef B4000000
		case 4000000:
			speed = B4000000;
			break;
#endif
		default:
			speed = B9600;
			if (ctx->debug) {
				fprintf(stderr,
						"WARNING Unknown baud rate %d for %s (B9600 used)\n",
						ctx_rtu->baud, ctx_rtu->device);
			}
	}

	/* Set the baud rate */
	if ((cfsetispeed(&tios, speed) < 0) ||
			(cfsetospeed(&tios, speed) < 0)) {
		close(ctx->s);
		ctx->s = -1;
		return -1;
	}

	/* C_CFLAG      Control options
	   CLOCAL       Local line - do not change "owner" of port
	   CREAD        Enable receiver
	 */
	tios.c_cflag |= (CREAD | CLOCAL);
	/* CSIZE, HUPCL, CRTSCTS (hardware flow control) */

	/* Set data bits (5, 6, 7, 8 bits)
	   CSIZE        Bit mask for data bits
	 */
	tios.c_cflag &= ~CSIZE;
	switch (ctx_rtu->data_bit) {
		case 5:
			tios.c_cflag |= CS5;
			break;
		case 6:
			tios.c_cflag |= CS6;
			break;
		case 7:
			tios.c_cflag |= CS7;
			break;
		case 8:
		default:
			tios.c_cflag |= CS8;
			break;
	}

	/* Stop bit (1 or 2) */
	if (ctx_rtu->stop_bit == 1)
		tios.c_cflag &=~ CSTOPB;
	else /* 2 */
		tios.c_cflag |= CSTOPB;

	/* PARENB       Enable parity bit
	   PARODD       Use odd parity instead of even */
	if (ctx_rtu->parity == 'N') {
		/* None */
		tios.c_cflag &=~ PARENB;
	} else if (ctx_rtu->parity == 'E') {
		/* Even */
		tios.c_cflag |= PARENB;
		tios.c_cflag &=~ PARODD;
	} else {
		/* Odd */
		tios.c_cflag |= PARENB;
		tios.c_cflag |= PARODD;
	}

	/* Read the man page of termios if you need more information. */

	/* This field isn't used on POSIX systems
	   tios.c_line = 0;
	 */

	/* C_LFLAG      Line options

	   ISIG Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals
	   ICANON       Enable canonical input (else raw)
	   XCASE        Map uppercase \lowercase (obsolete)
	   ECHO Enable echoing of input characters
	   ECHOE        Echo erase character as BS-SP-BS
	   ECHOK        Echo NL after kill character
	   ECHONL       Echo NL
	   NOFLSH       Disable flushing of input buffers after
	   interrupt or quit characters
	   IEXTEN       Enable extended functions
	   ECHOCTL      Echo control characters as ^char and delete as ~?
	   ECHOPRT      Echo erased character as character erased
	   ECHOKE       BS-SP-BS entire line on line kill
	   FLUSHO       Output being flushed
	   PENDIN       Retype pending input at next read or input char
	   TOSTOP       Send SIGTTOU for background output

	   Canonical input is line-oriented. Input characters are put
	   into a buffer which can be edited interactively by the user
	   until a CR (carriage return) or LF (line feed) character is
	   received.

	   Raw input is unprocessed. Input characters are passed
	   through exactly as they are received, when they are
	   received. Generally you'll deselect the ICANON, ECHO,
	   ECHOE, and ISIG options when using raw input
	 */

	/* Raw input */
	tios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	/* C_IFLAG      Input options

	   Constant     Description
	   INPCK        Enable parity check
	   IGNPAR       Ignore parity errors
	   PARMRK       Mark parity errors
	   ISTRIP       Strip parity bits
	   IXON Enable software flow control (outgoing)
	   IXOFF        Enable software flow control (incoming)
	   IXANY        Allow any character to start flow again
	   IGNBRK       Ignore break condition
	   BRKINT       Send a SIGINT when a break condition is detected
	   INLCR        Map NL to CR
	   IGNCR        Ignore CR
	   ICRNL        Map CR to NL
	   IUCLC        Map uppercase to lowercase
	   IMAXBEL      Echo BEL on input line too long
	 */
	if (ctx_rtu->parity == 'N') {
		/* None */
		tios.c_iflag &= ~INPCK;
	} else {
		tios.c_iflag |= INPCK;
	}

	/* Software flow control is disabled */
	tios.c_iflag &= ~(IXON | IXOFF | IXANY);

	/* C_OFLAG      Output options
	   OPOST        Postprocess output (not set = raw output)
	   ONLCR        Map NL to CR-NL

	   ONCLR ant others needs OPOST to be enabled
	 */

	/* Raw ouput */
	tios.c_oflag &=~ OPOST;

	/* C_CC         Control characters
	   VMIN         Minimum number of characters to read
	   VTIME        Time to wait for data (tenths of seconds)

	   UNIX serial interface drivers provide the ability to
	   specify character and packet timeouts. Two elements of the
	   c_cc array are used for timeouts: VMIN and VTIME. Timeouts
	   are ignored in canonical input mode or when the NDELAY
	   option is set on the file via open or fcntl.

	   VMIN specifies the minimum number of characters to read. If
	   it is set to 0, then the VTIME value specifies the time to
	   wait for every character read. Note that this does not mean
	   that a read call for N bytes will wait for N characters to
	   come in. Rather, the timeout will apply to the first
	   character and the read call will return the number of
	   characters immediately available (up to the number you
	   request).

	   If VMIN is non-zero, VTIME specifies the time to wait for
	   the first character read. If a character is read within the
	   time given, any read will block (wait) until all VMIN
	   characters are read. That is, once the first character is
	   read, the serial interface driver expects to receive an
	   entire packet of characters (VMIN bytes total). If no
	   character is read within the time allowed, then the call to
	   read returns 0. This method allows you to tell the serial
	   driver you need exactly N bytes and any read call will
	   return 0 or N bytes. However, the timeout only applies to
	   the first character read, so if for some reason the driver
	   misses one character inside the N byte packet then the read
	   call could block forever waiting for additional input
	   characters.

	   VTIME specifies the amount of time to wait for incoming
	   characters in tenths of seconds. If VTIME is set to 0 (the
	   default), reads will block (wait) indefinitely unless the
	   NDELAY option is set on the port with open or fcntl.
	 */
	/* Unused because we use open with the NDELAY option */
	tios.c_cc[VMIN] = 0;
	tios.c_cc[VTIME] = 0;

	if (tcsetattr(ctx->s, TCSANOW, &tios) < 0) {
		close(ctx->s);
		ctx->s = -1;
		return -1;
	}
#endif

	return 0;
}

int modbus_rtu_set_serial_mode(modbus_t *ctx, int mode)
{
	if (ctx == NULL || ctx->backend == NULL)
	{
		DBG_PRINT("\n");
		return -1;
	}
	if (ctx->backend->backend_type == _MODBUS_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCSRS485
		modbus_rtu_t *ctx_rtu = ctx->backend_data;
		struct serial_rs485 rs485conf;
		memset(&rs485conf, 0x0, sizeof(struct serial_rs485));

		if (mode == MODBUS_RTU_RS485) {
			rs485conf.flags = SER_RS485_ENABLED;
			if (ioctl(ctx->s, TIOCSRS485, &rs485conf) < 0) {
				return -1;
			}

			ctx_rtu->serial_mode = MODBUS_RTU_RS485;
			return 0;
		} else if (mode == MODBUS_RTU_RS232) {
			/* Turn off RS485 mode only if required */
			if (ctx_rtu->serial_mode == MODBUS_RTU_RS485) {
				/* The ioctl call is avoided because it can fail on some RS232 ports */
				if (ioctl(ctx->s, TIOCSRS485, &rs485conf) < 0) {
					return -1;
				}
			}
			ctx_rtu->serial_mode = MODBUS_RTU_RS232;
			return 0;
		}
#else
		if (ctx->debug) {
			fprintf(stderr, "This function isn't supported on your platform\n");
		}
		errno = ENOTSUP;
		return -1;
#endif
	}

	/* Wrong backend and invalid mode specified */
	errno = EINVAL;
	return -1;
}

int modbus_rtu_get_serial_mode(modbus_t *ctx) {
	if (ctx->backend->backend_type == _MODBUS_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCSRS485
		modbus_rtu_t *ctx_rtu = ctx->backend_data;
		return ctx_rtu->serial_mode;
#else
		if (ctx->debug) {
			fprintf(stderr, "This function isn't supported on your platform\n");
		}
		errno = ENOTSUP;
		return -1;
#endif
	} else {
		errno = EINVAL;
		return -1;
	}
}

int modbus_rtu_set_rts(modbus_t *ctx, int mode)
{
	if (ctx->backend->backend_type == _MODBUS_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCM_RTS
		modbus_rtu_t *ctx_rtu = ctx->backend_data;

		if (mode == MODBUS_RTU_RTS_NONE || mode == MODBUS_RTU_RTS_UP ||
				mode == MODBUS_RTU_RTS_DOWN) {
			ctx_rtu->rts = mode;

			/* Set the RTS bit in order to not reserve the RS485 bus */
			_modbus_rtu_ioctl_rts(ctx->s, ctx_rtu->rts != MODBUS_RTU_RTS_UP);

			return 0;
		}
#else
		if (ctx->debug) {
			fprintf(stderr, "This function isn't supported on your platform\n");
		}
		errno = ENOTSUP;
		return -1;
#endif
	}
	/* Wrong backend or invalid mode specified */
	errno = EINVAL;
	return -1;
}

int modbus_rtu_get_rts(modbus_t *ctx) {
	if (ctx->backend->backend_type == _MODBUS_BACKEND_TYPE_RTU) {
#if HAVE_DECL_TIOCM_RTS
		modbus_rtu_t *ctx_rtu = ctx->backend_data;
		return ctx_rtu->rts;
#else
		if (ctx->debug) {
			fprintf(stderr, "This function isn't supported on your platform\n");
		}
		errno = ENOTSUP;
		return -1;
#endif
	} else {
		errno = EINVAL;
		return -1;
	}
}

static void _modbus_rtu_close(modbus_t *ctx)
{
	/* Restore line settings and close file descriptor in RTU mode */
	modbus_rtu_t *ctx_rtu = ctx->backend_data;

#if defined(_WIN32)
	/* Revert settings */
	if (!SetCommState(ctx_rtu->w_ser.fd, &ctx_rtu->old_dcb))
		fprintf(stderr, "ERROR Couldn't revert to configuration (LastError %d)\n",
				(int)GetLastError());

	if (!CloseHandle(ctx_rtu->w_ser.fd))
		fprintf(stderr, "ERROR Error while closing handle (LastError %d)\n",
				(int)GetLastError());
#else
	if (ctx->s != -1) {
		tcsetattr(ctx->s, TCSANOW, &(ctx_rtu->old_tios));
		close(ctx->s);
	}
#endif
}

static int _modbus_rtu_flush(modbus_t *ctx)
{
	DBG_PRINT("enter\n");
#if defined(_WIN32)
	modbus_rtu_t *ctx_rtu = ctx->backend_data;
	ctx_rtu->w_ser.n_bytes = 0;
	return (FlushFileBuffers(ctx_rtu->w_ser.fd) == FALSE);
#else
	return tcflush(ctx->s, TCIOFLUSH);
#endif
}

static int _modbus_rtu_select(modbus_t *ctx, fd_set *rset,
		struct timeval *tv, __attribute__((unused)) int length_to_read)
{
	int s_rc;
#if defined(_WIN32)
	s_rc = win32_ser_select(&(((modbus_rtu_t*)ctx->backend_data)->w_ser),
			length_to_read, tv);
	if (s_rc == 0) {
		errno = ETIMEDOUT;
		return -1;
	}

	if (s_rc < 0) {
		return -1;
	}
#else
	while ((s_rc = select(ctx->s+1, rset, NULL, NULL, tv)) == -1) {
		if (errno == EINTR) {
			if (ctx->debug) {
				fprintf(stderr, "A non blocked signal was caught\n");
			}
			/* Necessary after an error */
			FD_ZERO(rset);
			FD_SET(ctx->s, rset);
		} else {
			return -1;
		}
	}

	if (s_rc == 0) {
		/* Timeout */
		errno = ETIMEDOUT;
		return -1;
	}
#endif

	return s_rc;
}

static void _modbus_rtu_free(modbus_t *ctx) {
	free(((modbus_rtu_t*)ctx->backend_data)->device);
	free(ctx->backend_data);
	free(ctx);
}

const modbus_backend_t _modbus_rtu_backend = {
	_MODBUS_BACKEND_TYPE_RTU,
	_MODBUS_RTU_HEADER_LENGTH,
	_MODBUS_RTU_CHECKSUM_LENGTH,
	MODBUS_RTU_MAX_ADU_LENGTH,
	_modbus_set_slave,
	_modbus_rtu_build_request_basis,
	_modbus_rtu_build_response_basis,
	_modbus_rtu_prepare_response_tid,
	_modbus_rtu_send_msg_pre,
	_modbus_rtu_send,
	_modbus_rtu_receive,
	_modbus_rtu_recv,
	_modbus_rtu_check_integrity,
	_modbus_rtu_pre_check_confirmation,
	_modbus_rtu_connect,
	_modbus_rtu_close,
	_modbus_rtu_flush,
	_modbus_rtu_select,
	_modbus_rtu_free
};

modbus_t* modbus_new_rtu(const char *device,
		int baud, char parity, int data_bit,
		int stop_bit)
{
	modbus_t *ctx;
	modbus_rtu_t *ctx_rtu;
	size_t device_size;

	ctx = (modbus_t *) malloc(sizeof(modbus_t));
	_modbus_init_common(ctx);

	ctx->backend = &_modbus_rtu_backend;
	ctx->backend_data = (modbus_rtu_t *) malloc(sizeof(modbus_rtu_t));
	ctx_rtu = (modbus_rtu_t *)ctx->backend_data;

	/* Device name and \0 */
	device_size = (strlen(device) + 1) * sizeof(char);
	if (device_size == 0) {
		fprintf(stderr, "The device string is empty\n");
		modbus_free(ctx);
		errno = EINVAL;
		return NULL;
	}

	ctx_rtu->device = (char *) malloc(device_size);
	strcpy(ctx_rtu->device, device);

	ctx_rtu->baud = baud;
	if (parity == 'N' || parity == 'E' || parity == 'O') {
		ctx_rtu->parity = parity;
	} else {
		modbus_free(ctx);
		errno = EINVAL;
		return NULL;
	}
	ctx_rtu->data_bit = data_bit;
	ctx_rtu->stop_bit = stop_bit;

#if HAVE_DECL_TIOCSRS485
	/* The RS232 mode has been set by default */
	ctx_rtu->serial_mode = MODBUS_RTU_RS232;
#endif

#if HAVE_DECL_TIOCM_RTS
	/* The RTS use has been set by default */
	ctx_rtu->rts = MODBUS_RTU_RTS_NONE;

	/* Calculate estimated time in micro second to send one byte */
	ctx_rtu->onebyte_time = (1000 * 1000) * (1 + data_bit + (parity == 'N' ? 0 : 1) + stop_bit) / baud;
#endif

	ctx_rtu->confirmation_to_ignore = FALSE;

#ifdef MODBUS_DEBUG
	ctx->debug = TRUE;
#endif
	return ctx;
}

/**************************************
 *
 *	modbus-tcprtu.c
 *
 *
 **************************************/
#if defined(_WIN32)
# define OS_WIN32
/* ws2_32.dll has getaddrinfo and freeaddrinfo on Windows XP and later.
 * minwg32 headers check WINVER before allowing the use of these */
# ifndef WINVER
# define WINVER 0x0501
# endif
/* Already set in modbus-tcp.h but it seems order matters in VS2005 */
# include <winsock2.h>
# include <ws2tcpip.h>
# define SHUT_RDWR 2
# define close closesocket
#else
# include <sys/socket.h>
# include <sys/ioctl.h>

#if defined(__OpenBSD__) || (defined(__FreeBSD__) && __FreeBSD__ < 5)
# define OS_BSD
# include <netinet/in_systm.h>
#endif

# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/tcp.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif

#if !defined(MSG_NOSIGNAL)
#define MSG_NOSIGNAL 0
#endif

#ifdef OS_WIN32
static int _modbus_tcp_init_win32(void)
{
	/* Initialise Windows Socket API */
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup() returned error code %d\n",
				(unsigned int)GetLastError());
		errno = EIO;
		return -1;
	}
	return 0;
}
#endif


/* Builds a RTU request header */
static int _modbus_tcprtu_build_request_basis(modbus_t *ctx, int function,
		int addr, int nb,
		uint8_t *req)
{
	assert(ctx->slave != -1);
	req[0] = ctx->slave;
	req[1] = function;
	req[2] = addr >> 8;
	req[3] = addr & 0x00ff;
	req[4] = nb >> 8;
	req[5] = nb & 0x00ff;

	return _MODBUS_TCPRTU_PRESET_REQ_LENGTH;
}

/* Builds a RTU response header */
static int _modbus_tcprtu_build_response_basis(sft_t *sft, uint8_t *rsp)
{
	/* In this case, the slave is certainly valid because a check is already
	 * done in _modbus_rtu_listen */
	rsp[0] = sft->slave;
	rsp[1] = sft->function;

	return _MODBUS_TCPRTU_PRESET_RSP_LENGTH;
}

static int _modbus_tcprtu_prepare_response_tid(__attribute__((unused)) const uint8_t *req, int *req_length)
{
	(*req_length) -= _MODBUS_TCPRTU_CHECKSUM_LENGTH;
	/* No TID */
	return 0;
}


static int _modbus_tcprtu_send_msg_pre(uint8_t *req, int req_length)
{
	uint16_t crc = crc16(req, req_length);
	req[req_length++] = crc >> 8;
	req[req_length++] = crc & 0x00FF;

	return req_length;
}

static ssize_t _modbus_tcprtu_send(modbus_t *ctx, const uint8_t *req, int req_length)
{
	/* MSG_NOSIGNAL
	   Requests not to send SIGPIPE on errors on stream oriented
	   sockets when the other end breaks the connection.  The EPIPE
	   error is still returned. */
	return send(ctx->s, (const char*)req, req_length, MSG_NOSIGNAL);
}

static int _modbus_tcprtu_flush(modbus_t *ctx)
{
	int rc;
	int rc_sum = 0;

	do {
		/* Extract the garbage from the socket */
		char devnull[MODBUS_TCPRTU_MAX_ADU_LENGTH];
#ifndef OS_WIN32
		rc = recv(ctx->s, devnull, MODBUS_TCPRTU_MAX_ADU_LENGTH, MSG_DONTWAIT);
#else
		/* On Win32, it's a bit more complicated to not wait */
		fd_set rset;
		struct timeval tv;

		tv.tv_sec = 0;
		tv.tv_usec = 0;
		FD_ZERO(&rset);
		FD_SET(ctx->s, &rset);
		rc = select(ctx->s+1, &rset, NULL, NULL, &tv);
		if (rc == -1) {
			return -1;
		}

		if (rc == 1) {
			/* There is data to flush */
			rc = recv(ctx->s, devnull, MODBUS_TCPRTU_MAX_ADU_LENGTH, 0);
		}
#endif
		if (rc > 0) {
			rc_sum += rc;
		}
	} while (rc == MODBUS_TCPRTU_MAX_ADU_LENGTH);

	return rc_sum;
}

static int _modbus_tcprtu_receive(modbus_t *ctx, uint8_t *req) {
	return _modbus_receive_msg(ctx, req, MSG_INDICATION);
}

static ssize_t _modbus_tcprtu_recv(modbus_t *ctx, uint8_t *rsp, int rsp_length) {
	return recv(ctx->s, (char *)rsp, rsp_length, 0);
}

/* The check_crc16 function shall return 0 is the message is ignored and the
   message length if the CRC is valid. Otherwise it shall return -1 and set
   errno to EMBADCRC. */
static int _modbus_tcprtu_check_integrity(modbus_t *ctx, uint8_t *msg,
		const int msg_length)
{
	uint16_t crc_calculated;
	uint16_t crc_received;
	int slave = msg[0];

	/* Filter on the Modbus unit identifier (slave) in RTU mode */
	if (slave != ctx->slave && slave != MODBUS_BROADCAST_ADDRESS) {
		/* Ignores the request (not for me) */
		if (ctx->debug) {
			fprintf(stderr,"Request for slave %d ignored (not %d)\n", slave, ctx->slave);
		}

		return 0;
	}

	crc_calculated = crc16(msg, msg_length - 2);
	crc_received = (msg[msg_length - 2] << 8) | msg[msg_length - 1];

	/* Check CRC of msg */
	if (crc_calculated == crc_received) {
		return msg_length;
	} else {
		if (ctx->debug) {
			fprintf(stderr, "ERROR CRC received %0X != CRC calculated %0X\n",
					crc_received, crc_calculated);
		}

		if (ctx->error_recovery & MODBUS_ERROR_RECOVERY_PROTOCOL) {
			_modbus_tcprtu_flush(ctx);
		}
		errno = EMBBADCRC;
		return OTHER_ERROR;
	}
}

static int _modbus_tcprtu_pre_check_confirmation(modbus_t *ctx, const uint8_t *req,
		const uint8_t *rsp, __attribute__((unused)) int rsp_length)
{
	/* Check responding slave is the slave we requested (except for broacast
	 * request) */
	if (req[0] != 0 && req[0] != rsp[0]) {
		if (ctx->debug) {
			fprintf(stderr,
					"The responding slave %d isn't the requested slave %d",
					rsp[0], req[0]);
		}
		errno = EMBBADSLAVE;
		return -1;
	} else {
		return 0;
	}
}

static int _modbus_tcp_set_ipv4_options(int s)
{
	int rc;
	int option;

	/* Set the TCP no delay flag */
	/* SOL_TCP = IPPROTO_TCP */
	option = 1;
	rc = setsockopt(s, IPPROTO_TCP, TCP_NODELAY,
			(const void *)&option, sizeof(int));
	if (rc == -1) {
		DBG_PRINT("setsockopt TCP_NODELAY ['%s']\n", strerror(errno));
		return -1;
	}

	option = 1;
	rc = setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
			(const void *)&option, sizeof(int));
	if (rc == -1) {
		DBG_PRINT("setsockopt SO_REUSEADDR ['%s']\n", strerror(errno));
		return -1;
	}

	/* If the OS does not offer SOCK_NONBLOCK, fall back to setting FIONBIO to
	 * make sockets non-blocking */
	/* Do not care about the return value, this is optional */
	option = 1;
#if !defined(SOCK_NONBLOCK) && defined(FIONBIO)
#ifdef OS_WIN32
	/* Setting FIONBIO expects an unsigned long according to MSDN */
	ioctlsocket(s, FIONBIO, &option);
#else
	ioctl(s, FIONBIO, &option);
#endif
#endif

#ifndef OS_WIN32
	/**
	 * Cygwin defines IPTOS_LOWDELAY but can't handle that flag so it's
	 * necessary to workaround that problem.
	 **/
	/* Set the IP low delay option */
	option = IPTOS_LOWDELAY;
	rc = setsockopt(s, IPPROTO_IP, IP_TOS,
			(const void *)&option, sizeof(int));
	if (rc == -1) {
		return -1;
	}
#endif

	return 0;
}

static int _connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen,
		struct timeval *tv)
{
	int rc;

	rc = connect(sockfd, addr, addrlen);

#ifdef OS_WIN32
	if (rc == -1 && WSAGetLastError() == WSAEINPROGRESS) {
#else
		if (rc == -1 && errno == EINPROGRESS) {
#endif
			fd_set wset;
			int optval;
			socklen_t optlen = sizeof(optval);

			/* Wait to be available in writing */
			FD_ZERO(&wset);
			FD_SET(sockfd, &wset);
			rc = select(sockfd + 1, NULL, &wset, NULL, tv);
			if (rc <= 0) {
				/* Timeout or fail */
				return -1;
			}

			/* The connection is established if SO_ERROR and optval are set to 0 */
			rc = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void *)&optval, &optlen);
			if (rc == 0 && optval == 0) {
				return 0;
			} else {
				errno = ECONNREFUSED;
				return -1;
			}
#ifdef OS_WIN32
		}
#else
	}
#endif
	return rc;
}

/* Establishes a modbus TCP connection with a Modbus server. */
static int _modbus_tcprtu_connect(modbus_t *ctx)
{
	int rc;
	/* Specialized version of sockaddr for Internet socket address (same size) */
	struct sockaddr_in addr;
	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	modbus_tcprtu_t *ctx_tcprtu = ctx->backend_data;
	int flags = SOCK_STREAM;

#ifdef OS_WIN32
	if (_modbus_tcp_init_win32() == -1) {
		return -1;
	}
#endif

#ifdef SOCK_CLOEXEC
	flags |= SOCK_CLOEXEC;
#endif

#ifdef SOCK_NONBLOCK
	flags |= SOCK_NONBLOCK;
#endif

	ctx->s = socket(PF_INET, flags, 0);
	if (ctx->s == -1) {
		return -1;
	}

	rc = _modbus_tcp_set_ipv4_options(ctx->s);
	if (rc == -1) {
		close(ctx->s);
		return -1;
	}

	if (ctx->debug) {
		fprintf(stderr,"Connecting to %s:%d\n", ctx_tcprtu->ip, ctx_tcprtu->port);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(ctx_tcprtu->port);
	addr.sin_addr.s_addr = inet_addr(ctx_tcprtu->ip);
	rc = _connect(ctx->s, (struct sockaddr *)&addr, sizeof(addr), &ctx->response_timeout);
	if (rc == -1) {
		if (ctx->debug) {
			perror("connect");
		}
		close(ctx->s);
		return -1;
	}

	return 0;
}



/* Closes the network connection and socket in TCP mode */
static void _modbus_tcprtu_close(modbus_t *ctx)
{
	if (ctx->s != -1) {
		shutdown(ctx->s, SHUT_RDWR);
		close(ctx->s);
	}
}

/* Listens for any request from one or many modbus masters in TCP */
int modbus_tcprtu_listen(modbus_t *ctx, int nb_connection)
{
	int new_socket;
	int yes;
	struct sockaddr_in addr;
	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}
	modbus_tcprtu_t *ctx_tcprtu = ctx->backend_data;

#ifdef OS_WIN32
	if (_modbus_tcp_init_win32() == -1) {
		return -1;
	}
#endif

	new_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (new_socket == -1) {
		return -1;
	}

	yes = 1;
	if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR,
				(char *) &yes, sizeof(yes)) == -1) {
		close(new_socket);
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	/* If the modbus port is < to 1024, we need the setuid root. */
	addr.sin_port = htons(ctx_tcprtu->port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(new_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		close(new_socket);
		return -1;
	}

	if (listen(new_socket, nb_connection) == -1) {
		close(new_socket);
		return -1;
	}

	return new_socket;
}


/* On success, the function return a non-negative integer that is a descriptor
   for the accepted socket. On error, -1 is returned, and errno is set
   appropriately. */
int modbus_tcprtu_accept(modbus_t *ctx, int *socket)
{
	struct sockaddr_in addr;
	socklen_t addrlen;

	addrlen = sizeof(addr);
#ifdef HAVE_ACCEPT4
	/* Inherit socket flags and use accept4 call */
	ctx->s = accept4(*socket, (struct sockaddr *)&addr, &addrlen, SOCK_CLOEXEC);
#else
	ctx->s = accept(*socket, (struct sockaddr *)&addr, &addrlen);
#endif

	if (ctx->s == -1) {
		close(*socket);
		*socket = 0;
		return -1;
	}

	if (ctx->debug) {
		fprintf(stderr,"The client connection from %s is accepted\n",
				inet_ntoa(addr.sin_addr));
	}

	return ctx->s;
}

static int _modbus_tcprtu_select(modbus_t *ctx, fd_set *rset, struct timeval *tv, __attribute__((unused)) int length_to_read)
{
	int s_rc;
	while ((s_rc = select(ctx->s+1, rset, NULL, NULL, tv)) == -1) {
		if (errno == EINTR) {
			if (ctx->debug) {
				fprintf(stderr, "A non blocked signal was caught\n");
			}
			/* Necessary after an error */
			FD_ZERO(rset);
			FD_SET(ctx->s, rset);
		} else {
			return -1;
		}
	}

	if (s_rc == 0) {
		if (ctx->debug) {
			fprintf(stderr, "%s: TIMEOUT CONDITION\n", __func__);
		}
		errno = ETIMEDOUT;
		return -1;
	}

	return s_rc;
}

static void _modbus_tcprtu_free(modbus_t *ctx) {
	free(ctx->backend_data);
	free(ctx);
}

const modbus_backend_t _modbus_tcprtu_backend = {
	_MODBUS_BACKEND_TYPE_TCPRTU,
	_MODBUS_TCPRTU_HEADER_LENGTH, 
	_MODBUS_TCPRTU_CHECKSUM_LENGTH, 
	MODBUS_TCPRTU_MAX_ADU_LENGTH,
	_modbus_set_slave,
	_modbus_tcprtu_build_request_basis,
	_modbus_tcprtu_build_response_basis,
	_modbus_tcprtu_prepare_response_tid,
	_modbus_tcprtu_send_msg_pre,
	_modbus_tcprtu_send,
	_modbus_tcprtu_receive,
	_modbus_tcprtu_recv,
	_modbus_tcprtu_check_integrity,
	_modbus_tcprtu_pre_check_confirmation,
	_modbus_tcprtu_connect,
	_modbus_tcprtu_close,
	_modbus_tcprtu_flush,
	_modbus_tcprtu_select,
	_modbus_tcprtu_free
};

modbus_t* modbus_new_tcprtu(const char *ip, int port)
{
	modbus_t *ctx;
	modbus_tcprtu_t *ctx_tcprtu;
	size_t dest_size;
	size_t ret_size;

#if defined(OS_BSD)
	/* MSG_NOSIGNAL is unsupported on *BSD so we install an ignore
	   handler for SIGPIPE. */
	struct sigaction sa;

	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &sa, NULL) < 0) {
		/* The debug flag can't be set here... */
		fprintf(stderr, "Coud not install SIGPIPE handler.\n");
		return NULL;
	}
#endif

	ctx = (modbus_t *) malloc(sizeof(modbus_t));
	_modbus_init_common(ctx);

	/* Could be changed after to reach a remote serial Modbus device */
	ctx->slave = MODBUS_TCP_SLAVE;

	ctx->backend = &(_modbus_tcprtu_backend);

	ctx->backend_data = (modbus_tcprtu_t *) malloc(sizeof(modbus_tcprtu_t));
	ctx_tcprtu = (modbus_tcprtu_t *)ctx->backend_data;

	dest_size = sizeof(char) * 16;
	ret_size = strlcpy(ctx_tcprtu->ip, ip, dest_size);
	if (ret_size == 0) {
		fprintf(stderr, "The IP string is empty\n");
		modbus_free(ctx);
		errno = EINVAL;
		return NULL;
	}

	if (ret_size >= dest_size) {
		fprintf(stderr, "The IP string has been truncated\n");
		modbus_free(ctx);
		errno = EINVAL;
		return NULL;
	}

	ctx_tcprtu->port = port;
	ctx_tcprtu->t_id = 0;

#ifdef MODBUS_DEBUG
	ctx->debug = TRUE;
#endif
	return ctx;
}

/********************************
 *
 *  modbus-tcp.c
 *
 *
 *********************************/
/* Builds a TCP request header */
static int _modbus_tcp_build_request_basis(modbus_t *ctx, int function,
		int addr, int nb,
		uint8_t *req)
{
	modbus_tcp_t *ctx_tcp = ctx->backend_data;

	/* Increase transaction ID */
	if (ctx_tcp->t_id < UINT16_MAX)
		ctx_tcp->t_id++;
	else
		ctx_tcp->t_id = 0;
	req[0] = ctx_tcp->t_id >> 8;
	req[1] = ctx_tcp->t_id & 0x00ff;

	/* Protocol Modbus */
	req[2] = 0;
	req[3] = 0;

	/* Length will be defined later by set_req_length_tcp at offsets 4
	   and 5 */

	req[6] = ctx->slave;
	req[7] = function;
	req[8] = addr >> 8;
	req[9] = addr & 0x00ff;
	req[10] = nb >> 8;
	req[11] = nb & 0x00ff;

	return _MODBUS_TCP_PRESET_REQ_LENGTH;
}

/* Builds a TCP response header */
static int _modbus_tcp_build_response_basis(sft_t *sft, uint8_t *rsp)
{
	/* Extract from MODBUS Messaging on TCP/IP Implementation
	   Guide V1.0b (page 23/46):
	   The transaction identifier is used to associate the future
	   response with the request. */
	rsp[0] = sft->t_id >> 8;
	rsp[1] = sft->t_id & 0x00ff;

	/* Protocol Modbus */
	rsp[2] = 0;
	rsp[3] = 0;

	/* Length will be set later by send_msg (4 and 5) */

	/* The slave ID is copied from the indication */
	rsp[6] = sft->slave;
	rsp[7] = sft->function;

	return _MODBUS_TCP_PRESET_RSP_LENGTH;
}


static int _modbus_tcp_prepare_response_tid(const uint8_t *req, __attribute__((unused)) int *req_length)
{
	return (req[0] << 8) + req[1];
}

static int _modbus_tcp_send_msg_pre(uint8_t *req, int req_length)
{
	/* Substract the header length to the message length */
	int mbap_length = req_length - 6;

	req[4] = mbap_length >> 8;
	req[5] = mbap_length & 0x00FF;

	return req_length;
}

static ssize_t _modbus_tcp_send(modbus_t *ctx, const uint8_t *req, int req_length)
{
	/* MSG_NOSIGNAL
	   Requests not to send SIGPIPE on errors on stream oriented
	   sockets when the other end breaks the connection.  The EPIPE
	   error is still returned. */
	return send(ctx->s, (const char*)req, req_length, MSG_NOSIGNAL);
}

static int _modbus_tcp_receive(modbus_t *ctx, uint8_t *req) {
	return _modbus_receive_msg(ctx, req, MSG_INDICATION);
}

static ssize_t _modbus_tcp_recv(modbus_t *ctx, uint8_t *rsp, int rsp_length) {
	return recv(ctx->s, (char *)rsp, rsp_length, 0);
}

static int _modbus_tcp_check_integrity(__attribute__((unused)) modbus_t *ctx, __attribute__((unused)) uint8_t *msg, const int msg_length)
{
	return msg_length;
}

static int _modbus_tcp_pre_check_confirmation(modbus_t *ctx, const uint8_t *req,
		const uint8_t *rsp, __attribute__((unused)) int rsp_length)
{
	int rc;
	/* Check TID */
	if (req[0] != rsp[0] || req[1] != rsp[1]) {
		if (ctx->debug) {
			fprintf(stderr, "Invalid TID received 0x%X (not 0x%X)\n",
					(rsp[0] << 8) + rsp[1], (req[0] << 8) + req[1]);
		}

		if(((rsp[0] << 8) + rsp[1]) < ((req[0] << 8) + req[1]))
		{
			do
			{
				rc = _modbus_receive_msg(ctx, rsp, MSG_CONFIRMATION);
				if (rc == -1)
				{
					errno = EMBBADDATA;
					return OTHER_ERROR;
				}
				else
				{
					errno = ETIMEDOUT;
					return TIMEOUT_ERROR;
				}
			}while(((rsp[0] << 8) + rsp[1]) == ((req[0] << 8) + req[1]));
		}
		else
		{
			errno = EMBBADDATA;
			return -1;
		}
	} else {
		return 0;
	}
}

/* Establishes a modbus TCP connection with a Modbus server. */
static int _modbus_tcp_connect(modbus_t *ctx)
{
	int rc;
	/* Specialized version of sockaddr for Internet socket address (same size) */
	struct sockaddr_in addr;
	modbus_tcp_t *ctx_tcp = ctx->backend_data;
	int flags = SOCK_STREAM;

#ifdef OS_WIN32
	if (_modbus_tcp_init_win32() == -1) {
		return -1;
	}
#endif

#ifdef SOCK_CLOEXEC
	flags |= SOCK_CLOEXEC;
#endif

#ifdef SOCK_NONBLOCK
	flags |= SOCK_NONBLOCK;
#endif

	ctx->s = socket(PF_INET, flags, 0);
	if (ctx->s == -1) {
		return -1;
	}

	rc = _modbus_tcp_set_ipv4_options(ctx->s);
	if (rc == -1) {
		close(ctx->s);
		return -1;
	}

	if (ctx->debug) {
		fprintf(stderr,"Connecting to %s:%d\n", ctx_tcp->ip, ctx_tcp->port);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(ctx_tcp->port);
	addr.sin_addr.s_addr = inet_addr(ctx_tcp->ip);
	rc = _connect(ctx->s, (struct sockaddr *)&addr, sizeof(addr), &ctx->response_timeout);
	if (rc == -1) {
		if (ctx->debug) {
			perror("connect");
		}
		close(ctx->s);
		return -1;
	}

	return 0;
}

/* Establishes a modbus TCP PI connection with a Modbus server. */
static int _modbus_tcp_pi_connect(modbus_t *ctx)
{
	int rc;
	struct addrinfo *ai_list;
	struct addrinfo *ai_ptr;
	struct addrinfo ai_hints;
	modbus_tcp_pi_t *ctx_tcp_pi = ctx->backend_data;

#ifdef OS_WIN32
	if (_modbus_tcp_init_win32() == -1) {
		return -1;
	}
#endif

	memset(&ai_hints, 0, sizeof(ai_hints));
#ifdef AI_ADDRCONFIG
	ai_hints.ai_flags |= AI_ADDRCONFIG;
#endif
	ai_hints.ai_family = AF_UNSPEC;
	ai_hints.ai_socktype = SOCK_STREAM;
	ai_hints.ai_addr = NULL;
	ai_hints.ai_canonname = NULL;
	ai_hints.ai_next = NULL;

	ai_list = NULL;
	rc = getaddrinfo(ctx_tcp_pi->node, ctx_tcp_pi->service,
			&ai_hints, &ai_list);
	if (rc != 0) {
		if (ctx->debug) {
			fprintf(stderr, "Error returned by getaddrinfo: %s\n", gai_strerror(rc));
		}
		errno = ECONNREFUSED;
		return -1;
	}

	for (ai_ptr = ai_list; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next) {
		int flags = ai_ptr->ai_socktype;
		int s;

#ifdef SOCK_CLOEXEC
		flags |= SOCK_CLOEXEC;
#endif

#ifdef SOCK_NONBLOCK
		flags |= SOCK_NONBLOCK;
#endif

		s = socket(ai_ptr->ai_family, flags, ai_ptr->ai_protocol);
		if (s < 0)
			continue;

		if (ai_ptr->ai_family == AF_INET)
			_modbus_tcp_set_ipv4_options(s);

		if (ctx->debug) {
			fprintf(stderr,"Connecting to [%s]:%s\n", ctx_tcp_pi->node, ctx_tcp_pi->service);
		}

		rc = _connect(s, ai_ptr->ai_addr, ai_ptr->ai_addrlen, &ctx->response_timeout);
		if (rc == -1) {
			if (ctx->debug) {
				perror("connect");
			}
			close(s);
			continue;
		}

		ctx->s = s;
		break;
	}

	freeaddrinfo(ai_list);

	if (ctx->s < 0) {
		return -1;
	}

	return 0;
}

/* Closes the network connection and socket in TCP mode */
static void _modbus_tcp_close(modbus_t *ctx)
{
	if (ctx->s != -1) {
		shutdown(ctx->s, SHUT_RDWR);
		close(ctx->s);
	}
}

static int _modbus_tcp_flush(modbus_t *ctx)
{
	int rc;
	int rc_sum = 0;
	DBG_PRINT("Enter\n");
	do {
		/* Extract the garbage from the socket */
		char devnull[MODBUS_TCP_MAX_ADU_LENGTH];
#ifndef OS_WIN32
		rc = recv(ctx->s, devnull, MODBUS_TCP_MAX_ADU_LENGTH, MSG_DONTWAIT);
		DBG_PRINT("current rc =%d\n",rc);
#else
		/* On Win32, it's a bit more complicated to not wait */
		fd_set rset;
		struct timeval tv;

		tv.tv_sec = 0;
		tv.tv_usec = 0;
		FD_ZERO(&rset);
		FD_SET(ctx->s, &rset);
		rc = select(ctx->s+1, &rset, NULL, NULL, &tv);
		DBG_PRINT("current rc =%d\n",rc);
		if (rc == -1) {
			DBG_PRINT("no data to be flushed\n");
			return -1;
		}

		if (rc == 1) {
			/* There is data to flush */
			rc = recv(ctx->s, devnull, MODBUS_TCP_MAX_ADU_LENGTH, 0);
			DBG_PRINT("flushed rc =%d\n",rc);
		}
#endif
		if (rc > 0) {
			DBG_PRINT("flushed rc =%d\n",rc);
			rc_sum += rc;
		}
		DBG_PRINT("current rc =%d\n",rc);
	} while (rc == MODBUS_TCP_MAX_ADU_LENGTH);
	
	DBG_PRINT("current rc_sum =%d\n",rc_sum);
	return rc_sum;
}

/* Listens for any request from one or many modbus masters in TCP */
int modbus_tcp_listen(modbus_t *ctx, int nb_connection)
{
	int new_socket;
	int yes;
	struct sockaddr_in addr;

	if (ctx == NULL)
	{
		DBG_PRINT("ERR\n");
		return -1;
	}

	modbus_tcp_t *ctx_tcp = ctx->backend_data;

#ifdef OS_WIN32
	if (_modbus_tcp_init_win32() == -1) {
		return -1;
	}
#endif

	new_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (new_socket == -1) {
		return -1;
	}

	yes = 1;
	if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR,
				(char *) &yes, sizeof(yes)) == -1) {
		close(new_socket);
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	/* If the modbus port is < to 1024, we need the setuid root. */
	addr.sin_port = htons(ctx_tcp->port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(new_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		close(new_socket);
		return -1;
	}

	if (listen(new_socket, nb_connection) == -1) {
		close(new_socket);
		return -1;
	}

	return new_socket;
}

int modbus_tcp_pi_listen(modbus_t *ctx, int nb_connection)
{
	int rc;
	struct addrinfo *ai_list;
	struct addrinfo *ai_ptr;
	struct addrinfo ai_hints;
	const char *node;
	const char *service;
	int new_socket;
	modbus_tcp_pi_t *ctx_tcp_pi = ctx->backend_data;

	if (ctx_tcp_pi->node[0] == 0)
		node = NULL; /* == any */
	else
		node = ctx_tcp_pi->node;

	if (ctx_tcp_pi->service[0] == 0)
		service = "502";
	else
		service = ctx_tcp_pi->service;

	memset(&ai_hints, 0, sizeof (ai_hints));
	ai_hints.ai_flags |= AI_PASSIVE;
#ifdef AI_ADDRCONFIG
	ai_hints.ai_flags |= AI_ADDRCONFIG;
#endif
	ai_hints.ai_family = AF_UNSPEC;
	ai_hints.ai_socktype = SOCK_STREAM;
	ai_hints.ai_addr = NULL;
	ai_hints.ai_canonname = NULL;
	ai_hints.ai_next = NULL;

	ai_list = NULL;
	rc = getaddrinfo(node, service, &ai_hints, &ai_list);
	if (rc != 0) {
		if (ctx->debug) {
			fprintf(stderr, "Error returned by getaddrinfo: %s\n", gai_strerror(rc));
		}
		errno = ECONNREFUSED;
		return -1;
	}

	new_socket = -1;
	for (ai_ptr = ai_list; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next) {
		int s;

		s = socket(ai_ptr->ai_family, ai_ptr->ai_socktype,
				ai_ptr->ai_protocol);
		if (s < 0) {
			if (ctx->debug) {
				perror("socket");
			}
			continue;
		} else {
			int yes = 1;
			rc = setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
					(void *) &yes, sizeof (yes));
			if (rc != 0) {
				close(s);
				if (ctx->debug) {
					perror("setsockopt");
				}
				continue;
			}
		}

		rc = bind(s, ai_ptr->ai_addr, ai_ptr->ai_addrlen);
		if (rc != 0) {
			close(s);
			if (ctx->debug) {
				perror("bind");
			}
			continue;
		}

		rc = listen(s, nb_connection);
		if (rc != 0) {
			close(s);
			if (ctx->debug) {
				perror("listen");
			}
			continue;
		}

		new_socket = s;
		break;
	}
	freeaddrinfo(ai_list);

	if (new_socket < 0) {
		return -1;
	}

	return new_socket;
}

/* On success, the function return a non-negative integer that is a descriptor
   for the accepted socket. On error, -1 is returned, and errno is set
   appropriately. */
int modbus_tcp_accept(modbus_t *ctx, int *socket)
{
	struct sockaddr_in addr;
	socklen_t addrlen;

	addrlen = sizeof(addr);
#ifdef HAVE_ACCEPT4
	/* Inherit socket flags and use accept4 call */
	ctx->s = accept4(*socket, (struct sockaddr *)&addr, &addrlen, SOCK_CLOEXEC);
#else
	ctx->s = accept(*socket, (struct sockaddr *)&addr, &addrlen);
#endif

	if (ctx->s == -1) {
		close(*socket);
		*socket = 0;
		return -1;
	}

	if (ctx->debug) {
		fprintf(stderr,"The client connection from %s is accepted\n",
				inet_ntoa(addr.sin_addr));
	}

	return ctx->s;
}

int modbus_tcp_pi_accept(modbus_t *ctx, int *socket)
{
	struct sockaddr_storage addr;
	socklen_t addrlen;

	addrlen = sizeof(addr);
	ctx->s = accept(*socket, (void *)&addr, &addrlen);
	if (ctx->s == -1) {
		close(*socket);
		*socket = 0;
	}

	if (ctx->debug) {
		fprintf(stderr,"The client connection is accepted.\n");
	}

	return ctx->s;
}

static int _modbus_tcp_select(modbus_t *ctx, fd_set *rset, struct timeval *tv, __attribute__((unused)) int length_to_read)
{
	int s_rc;
	while ((s_rc = select(ctx->s+1, rset, NULL, NULL, tv)) == -1) {
		if (errno == EINTR) {
			if (ctx->debug) {
				fprintf(stderr, "A non blocked signal was caught\n");
			}
			/* Necessary after an error */
			FD_ZERO(rset);
			FD_SET(ctx->s, rset);
		} else {
			return -1;
		}
	}

	if (s_rc == 0) {
		errno = ETIMEDOUT;
		return -1;
	}

	return s_rc;
}

static void _modbus_tcp_free(modbus_t *ctx) {
	free(ctx->backend_data);
	free(ctx);
}

const modbus_backend_t _modbus_tcp_backend = {
	_MODBUS_BACKEND_TYPE_TCP,
	_MODBUS_TCP_HEADER_LENGTH,
	_MODBUS_TCP_CHECKSUM_LENGTH,
	MODBUS_TCP_MAX_ADU_LENGTH,
	_modbus_set_slave,
	_modbus_tcp_build_request_basis,
	_modbus_tcp_build_response_basis,
	_modbus_tcp_prepare_response_tid,
	_modbus_tcp_send_msg_pre,
	_modbus_tcp_send,
	_modbus_tcp_receive,
	_modbus_tcp_recv,
	_modbus_tcp_check_integrity,
	_modbus_tcp_pre_check_confirmation,
	_modbus_tcp_connect,
	_modbus_tcp_close,
	_modbus_tcp_flush,
	_modbus_tcp_select,
	_modbus_tcp_free
};


const modbus_backend_t _modbus_tcp_pi_backend = {
	_MODBUS_BACKEND_TYPE_TCP,
	_MODBUS_TCP_HEADER_LENGTH,
	_MODBUS_TCP_CHECKSUM_LENGTH,
	MODBUS_TCP_MAX_ADU_LENGTH,
	_modbus_set_slave,
	_modbus_tcp_build_request_basis,
	_modbus_tcp_build_response_basis,
	_modbus_tcp_prepare_response_tid,
	_modbus_tcp_send_msg_pre,
	_modbus_tcp_send,
	_modbus_tcp_receive,
	_modbus_tcp_recv,
	_modbus_tcp_check_integrity,
	_modbus_tcp_pre_check_confirmation,
	_modbus_tcp_pi_connect,
	_modbus_tcp_close,
	_modbus_tcp_flush,
	_modbus_tcp_select,
	_modbus_tcp_free
};

modbus_t* modbus_new_tcp(const char *ip, int port)
{
	modbus_t *ctx;
	modbus_tcp_t *ctx_tcp;
	size_t dest_size;
	size_t ret_size;

#if defined(OS_BSD)
	/* MSG_NOSIGNAL is unsupported on *BSD so we install an ignore
	   handler for SIGPIPE. */
	struct sigaction sa;

	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &sa, NULL) < 0) {
		/* The debug flag can't be set here... */
		fprintf(stderr, "Coud not install SIGPIPE handler.\n");
		return NULL;
	}
#endif

	ctx = (modbus_t *) malloc(sizeof(modbus_t));
	_modbus_init_common(ctx);

	/* Could be changed after to reach a remote serial Modbus device */
	ctx->slave = MODBUS_TCP_SLAVE;

	ctx->backend = &(_modbus_tcp_backend);

	ctx->backend_data = (modbus_tcp_t *) malloc(sizeof(modbus_tcp_t));
	ctx_tcp = (modbus_tcp_t *)ctx->backend_data;

	dest_size = sizeof(char) * 16;
	ret_size = strlcpy(ctx_tcp->ip, ip, dest_size);
	if (ret_size == 0) {
		fprintf(stderr, "The IP string is empty\n");
		modbus_free(ctx);
		errno = EINVAL;
		return NULL;
	}

	if (ret_size >= dest_size) {
		fprintf(stderr, "The IP string has been truncated\n");
		modbus_free(ctx);
		errno = EINVAL;
		return NULL;
	}

	ctx_tcp->port = port;
	ctx_tcp->t_id = 0;

#ifdef MODBUS_DEBUG
	ctx->debug = TRUE;
#endif
	return ctx;
}


modbus_t* modbus_new_tcp_pi(const char *node, const char *service)
{
	modbus_t *ctx;
	modbus_tcp_pi_t *ctx_tcp_pi;
	size_t dest_size;
	size_t ret_size;

	ctx = (modbus_t *) malloc(sizeof(modbus_t));
	_modbus_init_common(ctx);

	/* Could be changed after to reach a remote serial Modbus device */
	ctx->slave = MODBUS_TCP_SLAVE;

	ctx->backend = &(_modbus_tcp_pi_backend);

	ctx->backend_data = (modbus_tcp_pi_t *) malloc(sizeof(modbus_tcp_pi_t));
	ctx_tcp_pi = (modbus_tcp_pi_t *)ctx->backend_data;

	dest_size = sizeof(char) * _MODBUS_TCP_PI_NODE_LENGTH;
	ret_size = strlcpy(ctx_tcp_pi->node, node, dest_size);
	if (ret_size == 0) {
		fprintf(stderr, "The node string is empty\n");
		modbus_free(ctx);
		errno = EINVAL;
		return NULL;
	}

	if (ret_size >= dest_size) {
		fprintf(stderr, "The node string has been truncated\n");
		modbus_free(ctx);
		errno = EINVAL;
		return NULL;
	}

	dest_size = sizeof(char) * _MODBUS_TCP_PI_SERVICE_LENGTH;
	ret_size = strlcpy(ctx_tcp_pi->service, service, dest_size);
	if (ret_size == 0) {
		fprintf(stderr, "The service string is empty\n");
		modbus_free(ctx);
		errno = EINVAL;
		return NULL;
	}

	if (ret_size >= dest_size) {
		fprintf(stderr, "The service string has been truncated\n");
		modbus_free(ctx);
		errno = EINVAL;
		return NULL;
	}

	ctx_tcp_pi->t_id = 0;

	return ctx;
}

/*********************************


  PLC VIEW OF MODBUS LIBRARY


 ********************************/

modbus_t * mb_get_context(char *label)
{
	if ( strcmp(label, CONTEXT_RTU) == 0 )
	{
		return ctx_rtu;
	}	
	else if ( strcmp(label, CONTEXT_TCP)==0 )
	{
		return ctx_tcp;
	}
	else if ( strcmp(label, CONTEXT_TCPRTU)==0 )
	{
		return ctx_tcprtu;
	}
	else
	{
		return NULL;
	}
}

void mb_set_slave(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_SET_SLAVE_PARAM OS_SPTR *pPara = (MB_SET_SLAVE_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s slave %d\n", current_ctx, (int)pPara->slave);

    ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_set_slave(ctx, (int)pPara->slave);
	if (!ret)
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
	else
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
}

void mb_get_response_timeout(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];

	MB_TIMEOUT_PARAM OS_SPTR *pPara = (MB_TIMEOUT_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	fprintf(stderr, "%s:%d - current_ctx %s\n", __func__, __LINE__, current_ctx); 
	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	struct timeval timeout;

	modbus_get_response_timeout(ctx, &timeout);
	pPara->timeout_ms = timeout.tv_usec / 1000 + timeout.tv_sec * 1000;
	fprintf(stderr, "%s:%d - tv_usec %d tv_sec %d msec %d\n", __func__, __LINE__, timeout.tv_usec, timeout.tv_sec, pPara->timeout_ms);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_set_response_timeout(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];

	MB_TIMEOUT_PARAM OS_SPTR *pPara = (MB_TIMEOUT_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	fprintf(stderr, "%s:%d - current_ctx %s\n", __func__, __LINE__, current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = pPara->timeout_ms * 1000;

	modbus_set_response_timeout(ctx, &timeout);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_get_byte_timeout(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];

	MB_TIMEOUT_PARAM OS_SPTR *pPara = (MB_TIMEOUT_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	struct timeval timeout;

	modbus_get_byte_timeout(ctx, &timeout);
	pPara->timeout_ms = timeout.tv_usec / 1000 + timeout.tv_sec * 1000;
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_set_byte_timeout(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];

	MB_TIMEOUT_PARAM OS_SPTR *pPara = (MB_TIMEOUT_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = pPara->timeout_ms * 1000;

	modbus_set_byte_timeout(ctx, &timeout);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_get_header_length(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_CTX_PARAM OS_SPTR *pPara = (MB_CTX_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_get_header_length(ctx);
	if (ret < 0)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		pPara->ret_value = ret;
	}
}

void mb_connect(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_CTX_PARAM OS_SPTR *pPara = (MB_CTX_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_connect(ctx);
	if (ret < 0)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
}

void mb_close(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];

	MB_CTX_PARAM OS_SPTR *pPara = (MB_CTX_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		modbus_close(ctx);
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
}

void mb_free(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];

	MB_CTX_PARAM OS_SPTR *pPara = (MB_CTX_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	modbus_free(ctx);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_flush(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_CTX_PARAM OS_SPTR *pPara = (MB_CTX_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_flush(ctx);
	if (ret < 0)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
}

void mb_read_bits(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_READ_BITS_PARAM OS_SPTR *pPara = (MB_READ_BITS_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_BITS_ARRAY OS_DPTR *pData = (MB_READ_WRITE_BITS_ARRAY OS_DPTR* )pPara->data;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_read_bits(ctx, pPara->addr, pPara->nb_bits, pData->pElem);
	if (ret == OTHER_ERROR)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		 if (ret == TIMEOUT_ERROR )
		{
			pPara->ret_value = ERR_TIMEOUT;
			DBG_PRINT("@@@@@TIMEOUT\n");
		}
		else
		{
			pPara->ret_value = OK;
			DBG_PRINT("OK\n");
		}
	}
}

void mb_read_input_bits(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_READ_BITS_PARAM OS_SPTR *pPara = (MB_READ_BITS_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_BITS_ARRAY OS_DPTR *pData = (MB_READ_WRITE_BITS_ARRAY OS_DPTR* )pPara->data;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_read_input_bits(ctx, pPara->addr, pPara->nb_bits, pData->pElem);
	if (ret == OTHER_ERROR)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		 if (ret == TIMEOUT_ERROR )
		{
			pPara->ret_value = ERR_TIMEOUT;
			DBG_PRINT("@@@@@TIMEOUT\n");
		}
		else
		{
			pPara->ret_value = OK;
			DBG_PRINT("OK\n");
		}
	}
}

void mb_read_registers(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_READ_REGISTERS_PARAM OS_SPTR *pPara = (MB_READ_REGISTERS_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pData = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->data;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s addr %d nb_regs %d\n", current_ctx,  pPara->addr, pPara->nb_regs);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}
	ret = modbus_read_registers(ctx, pPara->addr, pPara->nb_regs, (uint16_t *)(pData->pElem));
	if (ret == OTHER_ERROR)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
		 if (ret == TIMEOUT_ERROR )
		{
			pPara->ret_value = ERR_TIMEOUT;
			DBG_PRINT("@@@@@TIMEOUT\n");
		}
		else
		{
			pPara->ret_value = OK;
	#ifdef MODBUS_DEBUG
			{
				int i;
				fprintf(stderr, "%s:%d - OK\n", __func__, __LINE__);
				for (i = 0; i < pPara->nb_regs; i++)
				{
					fprintf(stderr, "    read[%d] : %d\n", i, ((uint16_t*)(pData->pElem))[i]);
				}
			}
	#endif
		}
	DBG_PRINT("%s\n", current_ctx);
}

void mb_read_input_registers(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_READ_REGISTERS_PARAM OS_SPTR *pPara = (MB_READ_REGISTERS_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pData = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->data;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_read_input_registers(ctx, pPara->addr, pPara->nb_regs, pData->pElem);
	if (ret == OTHER_ERROR)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		 if (ret == TIMEOUT_ERROR )
		{
			pPara->ret_value = ERR_TIMEOUT;
			DBG_PRINT("@@@@@TIMEOUT\n");
		}
		else
		{
			pPara->ret_value = OK;
			DBG_PRINT("OK\n");
		}
	}
}

void mb_write_bit(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_WRITE_BIT_PARAM OS_SPTR *pPara = (MB_WRITE_BIT_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_write_bit(ctx, pPara->addr, pPara->status);
	if (ret == OTHER_ERROR)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		 if (ret == TIMEOUT_ERROR )
		{
			pPara->ret_value = ERR_TIMEOUT;
			DBG_PRINT("@@@@@TIMEOUT\n");
		}
		else
		{
			pPara->ret_value = OK;
			DBG_PRINT("OK\n");
		}
	}
}

void mb_write_register(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_WRITE_REGISTER_PARAM OS_SPTR *pPara = (MB_WRITE_REGISTER_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_write_register(ctx, pPara->addr, pPara->value);
	if (ret == OTHER_ERROR)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		 if (ret == TIMEOUT_ERROR )
		{
			pPara->ret_value = ERR_TIMEOUT;
			DBG_PRINT("@@@@@TIMEOUT\n");
		}
		else
		{
			pPara->ret_value = OK;
			DBG_PRINT("OK\n");
		}
	}
	DBG_PRINT("%s\n", ctx);
}

void mb_write_bits(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_WRITE_BITS_PARAM OS_SPTR *pPara = (MB_WRITE_BITS_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_BITS_ARRAY OS_DPTR *pData = (MB_READ_WRITE_BITS_ARRAY OS_DPTR* )pPara->data;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_write_bits(ctx, pPara->addr, pPara->nb_bits, pData->pElem);
	if (ret == OTHER_ERROR)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		 if (ret == TIMEOUT_ERROR )
		{
			pPara->ret_value = ERR_TIMEOUT;
			DBG_PRINT("@@@@@TIMEOUT\n");
		}
		else
		{
			pPara->ret_value = OK;
			DBG_PRINT("OK\n");
		}
	}
}

void mb_write_registers(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_WRITE_REGISTERS_PARAM OS_SPTR *pPara = (MB_WRITE_REGISTERS_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pData = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->data;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);
	{
		int i;
		DBG_PRINT("%s:%d - current_ctx %s addr %d nb_regs %d\n", __func__, __LINE__, current_ctx,  pPara->addr, pPara->nb_regs);
		for (i = 0; i < pPara->nb_regs; i++)
		{
			DBG_PRINT("    write[%d] : %d\n", i, ((uint16_t*)(pData->pElem))[i]);
		}
	}

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_write_registers(ctx, pPara->addr, pPara->nb_regs, (uint16_t *)pData->pElem);
	if (ret == OTHER_ERROR)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		 if (ret == TIMEOUT_ERROR )
		{
			pPara->ret_value = ERR_TIMEOUT;
			DBG_PRINT("@@@@@TIMEOUT\n");
		}
		else
		{
			pPara->ret_value = OK;
			DBG_PRINT("OK\n");
		}
	}
}

void mb_mask_write_register(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_MASK_WRITE_REGISTER_PARAM OS_SPTR *pPara = (MB_MASK_WRITE_REGISTER_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_mask_write_register(ctx, pPara->addr, pPara->and_mask, pPara->or_mask);
	if (ret == OTHER_ERROR)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		 if (ret == TIMEOUT_ERROR )
		{
			pPara->ret_value = ERR_TIMEOUT;
			DBG_PRINT("@@@@@TIMEOUT\n");
		}
		else
		{
			pPara->ret_value = OK;
			DBG_PRINT("OK\n");
		}
	}
}

void mb_write_and_read_registers(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_WRITE_AND_READ_REGISTERS_PARAM OS_SPTR *pPara = (MB_WRITE_AND_READ_REGISTERS_PARAM OS_SPTR *)pIN;

	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pSrc = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->src;
	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pDest = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->dest;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_write_and_read_registers(ctx,
			pPara->write_addr, pPara->write_nb, pSrc->pElem,
			pPara->read_addr, pPara->read_nb, pDest->pElem);
	if (ret == OTHER_ERROR)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		 if (ret == TIMEOUT_ERROR )
		{
			pPara->ret_value = ERR_TIMEOUT;
			DBG_PRINT("@@@@@TIMEOUT\n");
		}
		else
		{
			pPara->ret_value = OK;
			DBG_PRINT("OK\n");
		}
	}

}

void mb_report_slave_id(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_REPORT_SLAVE_ID_PARAM OS_SPTR *pPara = (MB_REPORT_SLAVE_ID_PARAM OS_SPTR *)pIN;
#if 0
	MB_REPORT_SLAVE_ID_ARRAY OS_DPTR *pDest = (MB_REPORT_SLAVE_ID_ARRAY OS_DPTR* )pPara->dest;
#endif

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_report_slave_id(ctx, (uint8_t)pPara->slave);
	if (ret == OTHER_ERROR)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		 if (ret == TIMEOUT_ERROR )
		{
			pPara->ret_value = ERR_TIMEOUT;
			DBG_PRINT("@@@@@TIMEOUT\n");
		}
		else
		{
			pPara->ret_value = OK;
			DBG_PRINT("OK\n");
		}
	}
}

/* TODO
//void mb_mapping_new(STDLIBFUNCALL);
//void mb_mapping_free(STDLIBFUNCALL);
//void mb_send_raw_request(STDLIBFUNCALL);
//void mb_receive(STDLIBFUNCALL);
//void mb_receive_from(STDLIBFUNCALL);
//void mb_receive_confirmation(STDLIBFUNCALL);
void modbus_reply(STDLIBFUNCALL)
{
}

void mb_reply_exception(STDLIBFUNCALL)
{
modbus_t * ctx = NULL;
char current_ctx[8];
int ret;

MB_REPLY_EXCEPTION_PARAM OS_SPTR *pPara = (MB_REPLY_EXCEPTION_PARAM OS_SPTR *)pIN;

utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

ctx = mb_get_context(current_ctx);
if (ctx == NULL)
{
pPara->ret_value = ERR_ERROR;
DBG_PRINT("ERR\n");
return;
}

ret = modbus_reply_exception(ctx, pPara->req, pPara->exception_code);
if (ret < 0)
{
pPara->ret_value = ERR_ERROR;
DBG_PRINT("ERR\n");
}
else
{
pPara->ret_value = OK;
DBG_PRINT("OK\n");
}
}
 */

void mb_new_tcp(STDLIBFUNCALL)
{
	char ip_address[_MODBUS_TCP_PI_NODE_LENGTH];
	MB_NEW_TCP_PARAM OS_SPTR *pPara = (MB_NEW_TCP_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ip_address, ip_address);

	DBG_PRINT("%s:%d\n", ip_address, pPara->port);
	ctx_tcp = modbus_new_tcp(ip_address, pPara->port);
	if (ctx_tcp != NULL)
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
	else
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
}

void mb_tcp_listen(STDLIBFUNCALL)
{
	MB_TCP_LISTEN_PARAM OS_SPTR *pPara = (MB_TCP_LISTEN_PARAM OS_SPTR *)pIN;

	if (modbus_tcp_listen(ctx_tcp, 1) != 0)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
}

void mb_tcp_accept(STDLIBFUNCALL)
{
	MB_TCP_ACCEPT_PARAM OS_SPTR *pPara = (MB_TCP_ACCEPT_PARAM OS_SPTR *)pIN;

	DBG_PRINT("\n");
	if (modbus_tcp_accept(ctx_tcp, &sockettcp) != 0)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
}
void mb_new_tcp_pi(STDLIBFUNCALL)
{
	char node[_MODBUS_TCP_PI_NODE_LENGTH];
	char service[_MODBUS_TCP_PI_SERVICE_LENGTH];
	MB_NEW_TCP_PI_PARAM OS_SPTR *pPara = (MB_NEW_TCP_PI_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->node, node);
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->service, service);

	DBG_PRINT("\n");
	ctx_tcp_pi = modbus_new_tcp_pi(node, service);
	if (ctx_tcp_pi != NULL)
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
	else
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
}

void mb_tcp_pi_listen(STDLIBFUNCALL)
{
	MB_TCP_PI_LISTEN_PARAM OS_SPTR *pPara = (MB_TCP_PI_LISTEN_PARAM OS_SPTR *)pIN;

	DBG_PRINT("\n");
	if (modbus_tcp_pi_listen(ctx_tcp_pi, 1) != 0)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
}

void mb_tcp_pi_accept(STDLIBFUNCALL)
{
	MB_TCP_PI_ACCEPT_PARAM OS_SPTR *pPara = (MB_TCP_PI_ACCEPT_PARAM OS_SPTR *)pIN;

	DBG_PRINT("\n");
	if (modbus_tcp_pi_accept(ctx_tcp_pi, &sockettcp) != 0)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
}

void mb_new_tcprtu(STDLIBFUNCALL)
{
	char ip_address[16];
	MB_NEW_TCPRTU_PARAM OS_SPTR *pPara = (MB_NEW_TCPRTU_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ip_address, ip_address);

	DBG_PRINT("\n");
	ctx_tcprtu = modbus_new_tcprtu(ip_address, pPara->port);
	if (ctx_tcprtu != NULL)
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
	else
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
}

void mb_tcprtu_listen(STDLIBFUNCALL)
{
	MB_TCPRTU_LISTEN_PARAM OS_SPTR *pPara = (MB_TCPRTU_LISTEN_PARAM OS_SPTR *)pIN;
	DBG_PRINT("\n");
	if (modbus_tcprtu_listen(ctx_tcprtu, 1) != 0)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
}

void mb_tcprtu_accept(STDLIBFUNCALL)
{
	MB_TCPRTU_ACCEPT_PARAM OS_SPTR *pPara = (MB_TCPRTU_ACCEPT_PARAM OS_SPTR *)pIN;
	DBG_PRINT("\n");
	if ( modbus_tcprtu_accept(ctx_tcprtu, &sockettcprtu) != 0 )
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
}

void mb_new_rtu(STDLIBFUNCALL)
{
	char device[32];
	char parity[8];
	MB_NEW_RTU_PARAM OS_SPTR *pPara = (MB_NEW_RTU_PARAM OS_SPTR *)pIN;

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->device, device);
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->parity, parity);

	DBG_PRINT("device %s baud %d parity %c data_bit %d stop_bit %d\n", device, pPara->baud, parity[0], pPara->data_bit, pPara->stop_bit);
	ctx_rtu = modbus_new_rtu(device, pPara->baud, parity[0], pPara->data_bit, pPara->stop_bit);
	if (ctx_rtu != NULL)
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
	else
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERROR\n");
	}
}

void mb_rtu_set_serial_mode(STDLIBFUNCALL)
{
	int ret;
	MB_RTU_SET_SERIAL_MODE_PARAM OS_SPTR *pPara = (MB_RTU_SET_SERIAL_MODE_PARAM OS_SPTR *)pIN;
	DBG_PRINT("\n");
	ret = modbus_rtu_set_serial_mode(ctx_rtu, pPara->mode);
	if ( ret == 0 )
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
	else
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
}

void mb_rtu_get_serial_mode(STDLIBFUNCALL)
{
	int ret;
	MB_RTU_GET_SERIAL_MODE_PARAM OS_SPTR *pPara = (MB_RTU_GET_SERIAL_MODE_PARAM OS_SPTR *)pIN;
	DBG_PRINT("\n");
	pPara->mode = modbus_rtu_get_serial_mode(ctx_rtu);
	if ( ret < 0)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		pPara->mode = ret;
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
}

void mb_rtu_get_rts(STDLIBFUNCALL)
{
	int ret;
	MB_RTU_GET_RTS_PARAM OS_SPTR *pPara = (MB_RTU_GET_RTS_PARAM OS_SPTR *)pIN;
	DBG_PRINT("\n");
	ret = modbus_rtu_get_rts(ctx_rtu);
	if ( ret < 0)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		pPara->rts = ret;
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
}

void mb_rtu_set_rts(STDLIBFUNCALL)
{
	int ret;
	MB_RTU_SET_RTS_PARAM OS_SPTR *pPara = (MB_RTU_SET_RTS_PARAM OS_SPTR *)pIN;
	DBG_PRINT("\n");
	ret = modbus_rtu_set_rts(ctx_rtu, pPara->rts);
	if (!ret)
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
	else
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
}

void mb_set_bits_from_byte(STDLIBFUNCALL)
{
	MB_SET_BITS_FROM_BYTE_PARAM OS_SPTR *pPara = (MB_SET_BITS_FROM_BYTE_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_BITS_ARRAY OS_DPTR *pDest = (MB_READ_WRITE_BITS_ARRAY OS_DPTR* )pPara->dest;
	DBG_PRINT("\n");
	modbus_set_bits_from_byte(pDest->pElem, pPara->index, pPara->value);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_set_bits_from_bytes(STDLIBFUNCALL)
{
	MB_SET_BITS_FROM_BYTES_PARAM OS_SPTR *pPara = (MB_SET_BITS_FROM_BYTES_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_BITS_ARRAY OS_DPTR *pDest = (MB_READ_WRITE_BITS_ARRAY OS_DPTR* )pPara->dest;
	MB_READ_WRITE_BITS_ARRAY OS_DPTR *pTabByte = (MB_READ_WRITE_BITS_ARRAY OS_DPTR* )pPara->tab_byte;
	DBG_PRINT("\n");
	modbus_set_bits_from_bytes(pDest->pElem, pPara->index, pPara->nb_bits, pTabByte->pElem);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_get_byte_from_bits(STDLIBFUNCALL)
{
	MB_GET_BYTE_FROM_BITS_PARAM OS_SPTR *pPara = (MB_GET_BYTE_FROM_BITS_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_BITS_ARRAY OS_DPTR *pSrc = (MB_READ_WRITE_BITS_ARRAY OS_DPTR* )pPara->src;
	DBG_PRINT("\n");
	pPara->byte = modbus_get_byte_from_bits(pSrc->pElem, pPara->index, pPara->nb_bits);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_get_float(STDLIBFUNCALL)
{
	MB_GET_FLOAT_PARAM OS_SPTR *pPara = (MB_GET_FLOAT_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pSrc = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->src;
	DBG_PRINT("\n");
	pPara->value = modbus_get_float(pSrc->pElem);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}
void mb_get_float_dcba(STDLIBFUNCALL)
{
	MB_GET_FLOAT_PARAM OS_SPTR *pPara = (MB_GET_FLOAT_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pSrc = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->src;
	DBG_PRINT("\n");
	pPara->value = modbus_get_float_dcba(pSrc->pElem);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_get_float_badc(STDLIBFUNCALL)
{
	MB_GET_FLOAT_PARAM OS_SPTR *pPara = (MB_GET_FLOAT_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pSrc = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->src;
	DBG_PRINT("\n");
	pPara->value = modbus_get_float_badc(pSrc->pElem);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_get_float_cdab(STDLIBFUNCALL)
{
	MB_GET_FLOAT_PARAM OS_SPTR *pPara = (MB_GET_FLOAT_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pSrc = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->src;
	DBG_PRINT("\n");
	pPara->value = modbus_get_float_cdab(pSrc->pElem);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_set_float(STDLIBFUNCALL)
{
	MB_SET_FLOAT_PARAM OS_SPTR *pPara = (MB_SET_FLOAT_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pDest = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->dest;
	DBG_PRINT("\n");
	modbus_set_float(pPara->value, pDest->pElem);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_set_float_dcba(STDLIBFUNCALL)
{
	MB_SET_FLOAT_PARAM OS_SPTR *pPara = (MB_SET_FLOAT_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pDest = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->dest;
	DBG_PRINT("\n");
	modbus_set_float_dcba(pPara->value, pDest->pElem);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_set_float_badc(STDLIBFUNCALL)
{
	MB_SET_FLOAT_PARAM OS_SPTR *pPara = (MB_SET_FLOAT_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pDest = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->dest;
	DBG_PRINT("\n");
	modbus_set_float_badc(pPara->value, pDest->pElem);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_set_float_cdab(STDLIBFUNCALL)
{
	MB_SET_FLOAT_PARAM OS_SPTR *pPara = (MB_SET_FLOAT_PARAM OS_SPTR *)pIN;
	MB_READ_WRITE_REGISTER_ARRAY OS_DPTR *pDest = (MB_READ_WRITE_REGISTER_ARRAY OS_DPTR* )pPara->dest;
	DBG_PRINT("\n");
	modbus_set_float_cdab(pPara->value, pDest->pElem);
	pPara->ret_value = OK;
	DBG_PRINT("OK\n");
}

void mb_set_error_recovery(STDLIBFUNCALL)
{
	modbus_t * ctx = NULL;
	char current_ctx[8];
	int ret;

	MB_SET_ERROR_RECOVERY_PARAM OS_SPTR *pPara = (MB_SET_ERROR_RECOVERY_PARAM OS_SPTR *)pIN;
#if 0
	MB_REPORT_SLAVE_ID_ARRAY OS_DPTR *pDest = (MB_REPORT_SLAVE_ID_ARRAY OS_DPTR* )pPara->dest;
#endif

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->ctx, current_ctx);

	DBG_PRINT("current_ctx %s\n", current_ctx);

	ctx = mb_get_context(current_ctx);
	if (ctx == NULL)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
		return;
	}

	ret = modbus_set_error_recovery(ctx, pPara->recovery_mode);
	if (ret < 0)
	{
		pPara->ret_value = ERR_ERROR;
		DBG_PRINT("ERR\n");
	}
	else
	{
		pPara->ret_value = OK;
		DBG_PRINT("OK\n");
	}
}

#endif /* RTS_CFG_MODBUS_LIB */
