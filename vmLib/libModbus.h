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
 */

#ifndef _MODBUS_H_
#define _MODBUS_H_

#include "inc/stdInc.h"
#include "inc/libDef.h"
#if defined(RTS_CFG_MODBUS_LIB)

/* Add this for macros that defined unix flavor */
#if (defined(__unix__) || defined(unix)) && !defined(USG)
#include <sys/param.h>
#endif

#ifndef _MSC_VER
#include <stdint.h>
#include <sys/time.h>
#else
#include "stdint.h"
#include <time.h>
#endif

/* #include modbus-version.h */

/* The major version, (1, if %LIBMODBUS_VERSION is 1.2.3) */
#define LIBMODBUS_VERSION_MAJOR (3)

/* The minor version (2, if %LIBMODBUS_VERSION is 1.2.3) */
#define LIBMODBUS_VERSION_MINOR (1)

/* The micro version (3, if %LIBMODBUS_VERSION is 1.2.3) */
#define LIBMODBUS_VERSION_MICRO (0)

/* The full version, like 1.2.3 */
#define LIBMODBUS_VERSION        3.1.0

/* The full version, in string form (suited for string concatenation)
 */
#define LIBMODBUS_VERSION_STRING "3.1.0"

/* Numerically encoded version, like 0x010203 */
#define LIBMODBUS_VERSION_HEX ((LIBMODBUS_VERSION_MAJOR << 24) |  \
                               (LIBMODBUS_VERSION_MINOR << 16) |  \
                               (LIBMODBUS_VERSION_MICRO << 8))

/* Evaluates to True if the version is greater than @major, @minor and @micro
 */
#define LIBMODBUS_VERSION_CHECK(major,minor,micro)      \
    (LIBMODBUS_VERSION_MAJOR > (major) ||               \
     (LIBMODBUS_VERSION_MAJOR == (major) &&             \
      LIBMODBUS_VERSION_MINOR > (minor)) ||             \
     (LIBMODBUS_VERSION_MAJOR == (major) &&             \
      LIBMODBUS_VERSION_MINOR == (minor) &&             \
      LIBMODBUS_VERSION_MICRO >= (micro)))
/* end #include modbus-version.h */


#if defined(_WIN32)
# if defined(DLLBUILD)
/* define DLLBUILD when building the DLL */
#  define EXPORT __declspec(dllexport)
# else
#  define EXPORT __declspec(dllimport)
# endif
#else
# define EXPORT
#endif

#ifdef  __cplusplus
# define MODBUS_BEGIN_DECLS  extern "C" {
# define MODBUS_END_DECLS    }
#else
# define MODBUS_BEGIN_DECLS
# define MODBUS_END_DECLS
#endif

MODBUS_BEGIN_DECLS

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef OFF
#define OFF 0
#endif

#ifndef ON
#define ON 1
#endif

#define MODBUS_BROADCAST_ADDRESS    0

/* Modbus_Application_Protocol_V1_1b.pdf (chapter 6 section 1 page 12)
 * Quantity of Coils to read (2 bytes): 1 to 2000 (0x7D0)
 * (chapter 6 section 11 page 29)
 * Quantity of Coils to write (2 bytes): 1 to 1968 (0x7B0)
 */
#define MODBUS_MAX_READ_BITS              2000
#define MODBUS_MAX_WRITE_BITS             1968

/* Modbus_Application_Protocol_V1_1b.pdf (chapter 6 section 3 page 15)
 * Quantity of Registers to read (2 bytes): 1 to 125 (0x7D)
 * (chapter 6 section 12 page 31)
 * Quantity of Registers to write (2 bytes) 1 to 123 (0x7B)
 * (chapter 6 section 17 page 38)
 * Quantity of Registers to write in R/W registers (2 bytes) 1 to 121 (0x79)
 */
#define MODBUS_MAX_READ_REGISTERS          125
#define MODBUS_MAX_WRITE_REGISTERS         123
#define MODBUS_MAX_RW_WRITE_REGISTERS      121

/* Random number to avoid errno conflicts */
#define MODBUS_ENOBASE 112345678

/* Protocol exceptions */
enum {
    MODBUS_EXCEPTION_ILLEGAL_FUNCTION = 0x01,
    MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS,
    MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE,
    MODBUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE,
    MODBUS_EXCEPTION_ACKNOWLEDGE,
    MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY,
    MODBUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE,
    MODBUS_EXCEPTION_MEMORY_PARITY,
    MODBUS_EXCEPTION_NOT_DEFINED,
    MODBUS_EXCEPTION_GATEWAY_PATH,
    MODBUS_EXCEPTION_GATEWAY_TARGET,
    MODBUS_EXCEPTION_MAX
};

#define EMBXILFUN  (MODBUS_ENOBASE + MODBUS_EXCEPTION_ILLEGAL_FUNCTION)
#define EMBXILADD  (MODBUS_ENOBASE + MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS)
#define EMBXILVAL  (MODBUS_ENOBASE + MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE)
#define EMBXSFAIL  (MODBUS_ENOBASE + MODBUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE)
#define EMBXACK    (MODBUS_ENOBASE + MODBUS_EXCEPTION_ACKNOWLEDGE)
#define EMBXSBUSY  (MODBUS_ENOBASE + MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY)
#define EMBXNACK   (MODBUS_ENOBASE + MODBUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE)
#define EMBXMEMPAR (MODBUS_ENOBASE + MODBUS_EXCEPTION_MEMORY_PARITY)
#define EMBXGPATH  (MODBUS_ENOBASE + MODBUS_EXCEPTION_GATEWAY_PATH)
#define EMBXGTAR   (MODBUS_ENOBASE + MODBUS_EXCEPTION_GATEWAY_TARGET)

/* Native libmodbus error codes */
#define EMBBADCRC  (EMBXGTAR + 1)
#define EMBBADDATA (EMBXGTAR + 2)
#define EMBBADEXC  (EMBXGTAR + 3)
#define EMBUNKEXC  (EMBXGTAR + 4)
#define EMBMDATA   (EMBXGTAR + 5)
#define EMBBADSLAVE (EMBXGTAR + 6)

extern const unsigned int libmodbus_version_major;
extern const unsigned int libmodbus_version_minor;
extern const unsigned int libmodbus_version_micro;

typedef struct _modbus modbus_t;

extern modbus_t *ctx_rtu;
extern modbus_t *ctx_tcp;
extern modbus_t *ctx_tcprtu;

extern int sockettcp;
extern int sockettcprtu;

#define CONTEXT_RTU "RTU"
#define CONTEXT_TCP "TCP"
#define CONTEXT_TCPRTU "TCPRTU"

typedef struct {
    int nb_bits;
    int nb_input_bits;
    int nb_input_registers;
    int nb_registers;
    uint8_t *tab_bits;
    uint8_t *tab_input_bits;
    uint16_t *tab_input_registers;
    uint16_t *tab_registers;
} modbus_mapping_t;

typedef enum
{
    MODBUS_ERROR_RECOVERY_NONE          = 0,
    MODBUS_ERROR_RECOVERY_LINK          = (1<<1),
    MODBUS_ERROR_RECOVERY_PROTOCOL      = (1<<2),
} modbus_error_recovery_mode;

EXPORT int modbus_set_slave(modbus_t* ctx, int slave);
EXPORT int modbus_set_error_recovery(modbus_t *ctx, modbus_error_recovery_mode error_recovery);
EXPORT void modbus_set_socket(modbus_t *ctx, int socket);
EXPORT int modbus_get_socket(modbus_t *ctx);

EXPORT void modbus_get_response_timeout(modbus_t *ctx, struct timeval *timeout);
EXPORT void modbus_set_response_timeout(modbus_t *ctx, const struct timeval *timeout);

EXPORT void modbus_get_byte_timeout(modbus_t *ctx, struct timeval *timeout);
EXPORT void modbus_set_byte_timeout(modbus_t *ctx, const struct timeval *timeout);

EXPORT int modbus_get_header_length(modbus_t *ctx);

EXPORT int modbus_connect(modbus_t *ctx);
EXPORT void modbus_close(modbus_t *ctx);

EXPORT void modbus_free(modbus_t *ctx);

EXPORT int modbus_flush(modbus_t *ctx);
EXPORT void modbus_set_debug(modbus_t *ctx, int boolean);

EXPORT const char *modbus_strerror(int errnum);

EXPORT int modbus_read_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest);
EXPORT int modbus_read_input_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest);
EXPORT int modbus_read_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest);
EXPORT int modbus_read_input_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest);
EXPORT int modbus_write_bit(modbus_t *ctx, int coil_addr, int status);
EXPORT int modbus_write_register(modbus_t *ctx, int reg_addr, int value);
EXPORT int modbus_write_bits(modbus_t *ctx, int addr, int nb, const uint8_t *data);
EXPORT int modbus_write_registers(modbus_t *ctx, int addr, int nb, const uint16_t *data);
EXPORT int modbus_mask_write_register(modbus_t *ctx, int addr, uint16_t and_mask, uint16_t or_mask);
EXPORT int modbus_write_and_read_registers(modbus_t *ctx, int write_addr, int write_nb,
                                           const uint16_t *src, int read_addr, int read_nb,
                                           uint16_t *dest);
EXPORT int modbus_report_slave_id(modbus_t *ctx, uint8_t *dest);

EXPORT modbus_mapping_t* modbus_mapping_new(int nb_bits, int nb_input_bits,
                                            int nb_registers, int nb_input_registers);
EXPORT void modbus_mapping_free(modbus_mapping_t *mb_mapping);

EXPORT int modbus_send_raw_request(modbus_t *ctx, uint8_t *raw_req, int raw_req_length);

EXPORT int modbus_receive(modbus_t *ctx, uint8_t *req);
EXPORT int modbus_receive_from(modbus_t *ctx, int sockfd, uint8_t *req);

EXPORT int modbus_receive_confirmation(modbus_t *ctx, uint8_t *rsp);

EXPORT int modbus_reply(modbus_t *ctx, const uint8_t *req,
                        int req_length, modbus_mapping_t *mb_mapping);
EXPORT int modbus_reply_exception(modbus_t *ctx, const uint8_t *req,
                                  unsigned int exception_code);

EXPORT void modbus_set_bits_from_bytes(uint8_t *dest, int index, unsigned int nb_bits,
                                       const uint8_t *tab_byte);
/*  #include "modbus-tcp.h" */
MODBUS_BEGIN_DECLS

#if defined(_WIN32) && !defined(__CYGWIN__)
/* Win32 with MinGW, supplement to <errno.h> */
#include <winsock2.h>
#if !defined(ECONNRESET)
#define ECONNRESET   WSAECONNRESET
#endif
#if !defined(ECONNREFUSED)
#define ECONNREFUSED WSAECONNREFUSED
#endif
#if !defined(ETIMEDOUT)
#define ETIMEDOUT    WSAETIMEDOUT
#endif
#if !defined(ENOPROTOOPT)
#define ENOPROTOOPT  WSAENOPROTOOPT
#endif
#if !defined(EINPROGRESS)
#define EINPROGRESS  WSAEINPROGRESS
#endif
#endif

#define MODBUS_TCP_DEFAULT_PORT   502
#define MODBUS_TCP_SLAVE         0xFF

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
 * TCP MODBUS ADU = 253 bytes + MBAP (7 bytes) = 260 bytes
 */
#define MODBUS_TCP_MAX_ADU_LENGTH  260

EXPORT modbus_t* modbus_new_tcp(const char *ip_address, int port);
EXPORT int modbus_tcp_listen(modbus_t *ctx, int nb_connection);
EXPORT int modbus_tcp_accept(modbus_t *ctx, int *socket);

EXPORT modbus_t* modbus_new_tcp_pi(const char *node, const char *service);
EXPORT int modbus_tcp_pi_listen(modbus_t *ctx, int nb_connection);
EXPORT int modbus_tcp_pi_accept(modbus_t *ctx, int *socket);

MODBUS_END_DECLS
/*  end #include "modbus-tcp.h" */


/* #include "modbus-tcprtu.h" */

MODBUS_BEGIN_DECLS

/* This is a fake implementation for the MODBUS TCP protocol: it's just and RTU master that communicate over TCP/IP
 * 
 */
#define MODBUS_TCPRTU_MAX_ADU_LENGTH  256

EXPORT modbus_t* modbus_new_tcprtu(const char *ip_address, int port);
EXPORT int modbus_tcprtu_listen(modbus_t *ctx, int nb_connection);
EXPORT int modbus_tcprtu_accept(modbus_t *ctx, int *socket);


MODBUS_END_DECLS

/* end #include "modbus-tcprtu.h" */


/* #include "modbus-rtu.h" */

MODBUS_BEGIN_DECLS

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
 * RS232 / RS485 ADU = 253 bytes + slave (1 byte) + CRC (2 bytes) = 256 bytes
 */
#define MODBUS_RTU_MAX_ADU_LENGTH  256

EXPORT modbus_t* modbus_new_rtu(const char *device, int baud, char parity,
                                int data_bit, int stop_bit);

#define MODBUS_RTU_RS232 0
#define MODBUS_RTU_RS485 1

EXPORT int modbus_rtu_set_serial_mode(modbus_t *ctx, int mode);
EXPORT int modbus_rtu_get_serial_mode(modbus_t *ctx);

#define MODBUS_RTU_RTS_NONE   0
#define MODBUS_RTU_RTS_UP     1
#define MODBUS_RTU_RTS_DOWN   2

EXPORT int modbus_rtu_set_rts(modbus_t *ctx, int mode);
EXPORT int modbus_rtu_get_rts(modbus_t *ctx);

MODBUS_END_DECLS

/* end #include "modbus-rtu.h" */

MODBUS_END_DECLS

/* Max between RTU and TCP max adu length (so TCP) */
#define MAX_MESSAGE_LENGTH 260

#endif	/* RTS_CFG_MODBUS_LIB */

#endif  /* _MODBUS_H_ */
