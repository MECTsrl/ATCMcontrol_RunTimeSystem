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
 * Filename: libMBRTU.c
 */

#include <fcntl.h>      /* File control definitions */
#include <stdio.h>      /* Standard input/output */
#include <string.h>
#include <stdlib.h>
#include <termio.h>     /* POSIX terminal control definitions */
#include <sys/time.h>   /* Time structures for select() */
#include <unistd.h>     /* POSIX Symbolic Constants */
#include <assert.h>
#include <errno.h>      /* Error definitions */
#include <time.h>       /* clock_gettime()   */
#include <pthread.h>     
#include <netinet/in.h> /*htons, ntohs.....*/
#include "mectCfgUtil.h"

/* 4C include*/
#include "stdInc.h"

#include "libMBRTU.h"
#define MODBUS_DEBUG 1

#define APP_MBRTU_WORKAROUND 1
#ifdef APP_MBRTU_WORKAROUND
#warning "Delay Workaround activated!"
#endif

#define VALMAR_OMRON_MX2_WORKAROUND
#ifdef VALMAR_OMRON_MX2_WORKAROUND
#warning "Valmar Workaround on function 0f activated"
#endif

#define INT2BYTE

mbrtu_cfg_s mbrtu_cfg;

/*
 * Some of these constants are specific to the application layer of the protocol
 * (i.e. mbrtu_master and slave), while others are specific of the
 * communication protocols (i.e. rtu, ascii, tcp).
 *
 * a) Unfortunately, due to the nature of the modbus protocol, that does not
 * include a frame size field in thecommunicatiob frame (see note 1), and the
 * fact that we are implementing it at the user level, the implementation
 * of rtu communication protocol need to know the content of the application protocol
 * in order to determine the size of the frame.
 *
 * b) The application message formats are in fact the same, just reversing the role
 * being played (mbrtu_master or slave).
 *
 *
 * Notes:
 *  (1) There is no communication field with the frame size, nevertheless this
 *      size can be determined indirectly due to timing restrictions on the rtu
 *      protocol. Unfortunately, due to the fact that we are implementing
 *      it at the user level, we are not guaranteed to detect these timings
 *      correctly, and therefore have to rely on application protocol info to
 *      determine the frame size.
 */


/* Layer 2 (application) Frame Structure...                */
/* Valid for both mbrtu_master and slave protocols */
#define APPLICATION_FRAME_HEADER_LENGTH    6
#define APPLICATION_FRAME_BYTECOUNT_LENGTH 1
#define APPLICATION_FRAME_DATABYTES_LENGTH 255
#define MAX_APPLICATION_FRAME_LENGTH (APPLICATION_FRAME_HEADER_LENGTH + APPLICATION_FRAME_BYTECOUNT_LENGTH +    \
		APPLICATION_FRAME_DATABYTES_LENGTH)

#define APPLICATION_FRAME_SLAVEID_OFS    0
#define APPLICATION_FRAME_FUNCTION_OFS   1


/* Communication layer - RTU Frame sizes... */
#define RTU_FRAME_CRC_LENGTH    2


/* Global Frame sizes */
#define MAX_RTU_FRAME_LENGTH MAX_APPLICATION_FRAME_LENGTH + RTU_FRAME_CRC_LENGTH


/* serial port default configuration... */
#define DEFAULT_DATA_BITS 8
#define DEFAULT_STOP_BITS_PARITY 1 /* default stop bits if parity is used     */
// #define DEFAULT_STOP_BITS_NOPARITY 2 /* default stop bits if parity is not used */
#define DEFAULT_STOP_BITS_NOPARITY 1
#define DEFAULT_BAUD_RATE 9600
#define DEFAULT_SERIAL_PORT "/dev/ttySP0" /* To be checked is the same of RS232 there is just a jumper */


/* Send retries of rtu frames... */
#define RTU_FRAME_SEND_RETRY 1
/* NOTES:
 *  - the above are the retries at the protocol level,
 *    higher layers may decide to retry for themselves!
 */


/* Buffer sizes... */
/* We use double the maximum frame length for the read buffer,
 * due to the algorithm used to work around aborted frames.
 */
#define RECV_BUFFER_SIZE_SMALL  (MAX_RTU_FRAME_LENGTH + 10)
#define RECV_BUFFER_SIZE_LARGE  (2 * MAX_RTU_FRAME_LENGTH)


/* Frame lengths... */

/* The number of bytes in each frame format, excluding CRC.
 *
 * BYTE_COUNT_3  denotes that third byte of frame contains the number of bytes;
 *                 - total number of bytes in frame is
 *                   BYTE_COUNT_3_HEADER + byte_count
 * BYTE_COUNT_34 denotes that third+fourth bytes of frame contain the number of bytes;
 *                 - total number of bytes in frame is
 *                   BYTE_COUNT_34_HEADER + byte_count
 * BYTE_COUNT_7  denotes that seventh byte of frame contain the number of bytes;
 *                 - total number of bytes in frame is
 *                   BYTE_COUNT_7_HEADER + byte_count
 * BYTE_COUNT_11 denotes that eleventh byte of frame contain the number of bytes;
 *                 - total number of bytes in frame is
 *                   BYTE_COUNT_11_HEADER + byte_count
 * BYTE_COUNT_U  denotes unknown number of bytes;
 */

#define BYTE_COUNT_3_HEADER   3
#define BYTE_COUNT_34_HEADER  4
#define BYTE_COUNT_7_HEADER   7
#define BYTE_COUNT_11_HEADER  11

#define BYTE_COUNT_3  (-3)
#define BYTE_COUNT_34 (-34)
#define BYTE_COUNT_7  (-7)
#define BYTE_COUNT_11 (-11)
#define BYTE_COUNT_U  (-128)


#define MAX_FUNCTION_CODE 0x18

#define MIN_FRAME_LENGTH       3
#define EXCEPTION_FRAME_LENGTH 3


#define QUERY_BUFFER_SIZE       MAX_APPLICATION_FRAME_LENGTH
#define DEF_LAYER2_SEND_RETRIES 1
#define DEF_IGNORE_ECHO         0

static signed char query_frame_lengths[MAX_FUNCTION_CODE+1] = {
	/* 0x00 */ 0,             /* unused                    */
	/* 0x01 */ 6,             /* Read Coil Status          */
	/* 0x02 */ 6,             /* Read Input Status         */
	/* 0x03 */ 6,             /* Read Holding Registers    */
	/* 0x04 */ 6,             /* Read Input Registers      */
	/* 0x05 */ 6,             /* Force Single Coil         */
	/* 0x06 */ 6,             /* Preset Single Register    */
	/* 0x07 */ 2,             /* Read Exception Status     */
	/* 0x08 */ 4,             /* Diagnostics               */
	/* 0x09 */ BYTE_COUNT_U,  /* Program 484               */
	/* 0x0A */ BYTE_COUNT_U,  /* Poll 484                  */
	/* 0x0B */ 2,             /* Fetch Comm. Event Counter */
	/* 0x0C */ 2,             /* Fetch Comm. Event Log     */
	/* 0x0D */ BYTE_COUNT_U,  /* Program Controller        */
	/* 0x0E */ BYTE_COUNT_U,  /* Poll Controller           */
	/* 0x0F */ BYTE_COUNT_7,  /* Force Multiple Coils      */
	/* 0x10 */ BYTE_COUNT_7,  /* Preset Multiple Registers */
	/* 0x11 */ 2,             /* Report Slave ID           */
	/* 0x12 */ BYTE_COUNT_U,  /* Program 884/M84           */
	/* 0x13 */ BYTE_COUNT_U,  /* Reset. Comm. Link         */
	/* 0x14 */ BYTE_COUNT_3,  /* Read General Reference    */
	/* 0x15 */ BYTE_COUNT_3,  /* Write General Reference   */
	/* 0x16 */ 8,             /* Mask Write 4X Register    */
	/* 0x17 */ BYTE_COUNT_11, /* Read/Write 4x Register    */
	/* 0x18 */ 4              /* Read FIFO Queue           */
};

static signed char response_frame_lengths[MAX_FUNCTION_CODE+1] = {
	/* 0x00 */ 0,             /* unused                    */
	/* 0x01 */ BYTE_COUNT_3,  /* Read Coil Status          */
	/* 0x02 */ BYTE_COUNT_3,  /* Read Input Status         */
	/* 0x03 */ BYTE_COUNT_3,  /* Read Holding Registers    */
	/* 0x04 */ BYTE_COUNT_3,  /* Read Input Registers      */
	/* 0x05 */ 6,             /* Force Single Coil         */
	/* 0x06 */ 6,             /* Preset Single Register    */
	/* 0x07 */ 3,             /* Read Exception Status     */
	/* 0x08 */ 6,/*see (1)*/  /* Diagnostics               */
	/* 0x09 */ BYTE_COUNT_U,  /* Program 484               */
	/* 0x0A */ BYTE_COUNT_U,  /* Poll 484                  */
	/* 0x0B */ 6,             /* Fetch Comm. Event Counter */
	/* 0x0C */ BYTE_COUNT_3,  /* Fetch Comm. Event Log     */
	/* 0x0D */ BYTE_COUNT_U,  /* Program Controller        */
	/* 0x0E */ BYTE_COUNT_U,  /* Poll Controller           */
	/* 0x0F */ 6,             /* Force Multiple Coils      */
	/* 0x10 */ 6,             /* Preset Multiple Registers */
	/* 0x11 */ BYTE_COUNT_3,  /* Report Slave ID           */
	/* 0x12 */ BYTE_COUNT_U,  /* Program 884/M84           */
	/* 0x13 */ BYTE_COUNT_U,  /* Reset. Comm. Link         */
	/* 0x14 */ BYTE_COUNT_3,  /* Read General Reference    */
	/* 0x15 */ BYTE_COUNT_3,  /* Write General Reference   */
	/* 0x16 */ 8,             /* Mask Write 4X Register    */
	/* 0x17 */ BYTE_COUNT_3,  /* Read/Write 4x Register    */
	/* 0x18 */ BYTE_COUNT_34  /* Read FIFO Queue           */
};

/* NOTE (1):
 *    The diagnostic function (0x08) has sub-functions. In particular,
 *    sub-function 21 (0x15) has two sub-sub-functions. In the very
 *    particular case of *one* of these sub-sub-functions, the reply
 *    frame does *not* have a size of 4, but is variable in length
 *    and includes a byte counter.
 *    To take this into account in the table would require an extra two
 *    tables.
 *    The above length has been hardcoded into the frame_length() function
 *   
 */


#define FALSE 0
#define TRUE 1


/* Thread structure and pointer*/
struct app_mbrtu_params_s {
	unsigned char function;
	unsigned char slave;
	unsigned short int subfunction;
	unsigned short int start_addr;
	unsigned short int reg_addr;
	unsigned short int reg_count;
	unsigned short int count;
	unsigned short int *dest;
	unsigned short int value;
	unsigned short int *source;
	unsigned char byteorder;
	int send_retries;
	unsigned char *error_code;
	struct timespec *response_timeout;
};

struct app_mbrtu_params_s app_mbrtu_params;
pthread_t app_mbrtu_thread_id;
pthread_attr_t app_mbrtu_attr;
struct sched_param app_mbrtu_sched_param;
struct timespec app_mbrtu_min_timeout;


/* Global Variables*/

/*********************************************/
/**                                         **/
/** CRC calculation Data Structure Instance **/
/**                                         **/
/*********************************************/

static unsigned char *crc_fast_buf = NULL;

/********************************************/
/**                                        **/
/** MODBUS Master Data Structure Instance  **/
/**                                        **/
/********************************************/

/* Master node data structure */
/* NOTE: This variable is also used to check whether the library
 *       has been previously initialised.
 *       If == NULL, «> library not yet initialised...
 */
app_mbrtu_master_node_t *mbrtu_master = NULL;


/********************************************/
/**                                        **/
/** MODBUS Query buffer Instance           **/
/**                                        **/
/********************************************/

unsigned char query_buffer[8];
unsigned char query_long_buffer[256];

/********************************************/
/**                                        **/
/** MODBUS Status			   **/
/**                                        **/
/********************************************/
short app_mbrtu_status[APP_MBRTU_STATUS];




/****************************************************/
/**                                                **/
/** MODBUS mbrtu_master Local Function Prototypes  **/
/**                                                **/
/****************************************************/

/*  Miscellaneous Utility functions */
static inline unsigned short int app_mbrtu_next_transaction_id(void);
static inline unsigned short int app_mbrtu_hton(unsigned short int h_value);
static inline unsigned short int app_mbrtu_ntoh(unsigned short int m_value);
static inline unsigned char app_mbrtu_msb(unsigned short int value);
static inline unsigned char app_mbrtu_lsb(unsigned short int value);

/*     Time format conversion and select with absolute timeout     */
static inline struct timeval app_mbrtu_double_to_timeval(double time);
static inline struct timespec app_mbrtu_double_to_timespec(double time);
static inline struct timespec app_mbrtu_timespec_dif(struct timespec ts1, struct timespec ts2);
static inline struct timespec app_mbrtu_timespec_add(struct timespec ts1, struct timespec ts2);
static inline struct timeval app_mbrtu_timespec_to_timeval(struct timespec ts);
static int app_mbrtu_select(int fd, fd_set *rfds, const struct timespec *end_time);

/*  CRC functions  */
static inline unsigned short int app_mbrtu_crc_read(unsigned char *buf, int cnt);
static inline void app_mbrtu_crc_write(unsigned char *buf, int cnt);
static unsigned short int app_mbrtu_crc_calc_fast(unsigned char *buf, int cnt);
static inline int app_mbrtu_crc_fast_init(void);
static inline void app_mbrtu_crc_fast_done(void);
static unsigned short int app_mbrtu_crc_slow(unsigned char *buf, int cnt);

/* Linear Buffer Functions */
static inline unsigned char *app_mbrtu_lb_init(app_mbrtu_lb_buf_t *buf, int size, int max_data_start);
static inline void app_mbrtu_lb_done(app_mbrtu_lb_buf_t *buf);
static inline unsigned char *app_mbrtu_lb_normalize(app_mbrtu_lb_buf_t *buf);
static inline unsigned char *app_mbrtu_lb_data(app_mbrtu_lb_buf_t *buf);
static inline unsigned char *app_mbrtu_lb_data_purge(app_mbrtu_lb_buf_t *buf, int count);
static inline void app_mbrtu_lb_data_purge_all(app_mbrtu_lb_buf_t *buf); 
static inline unsigned char *app_mbrtu_lb_free(app_mbrtu_lb_buf_t *buf); 
static inline int app_mbrtu_lb_free_count(app_mbrtu_lb_buf_t *buf);

/*   Receive buffer  functions   */
static inline unsigned char *app_mbrtu_recv_buf_init(app_mbrtu_recv_buf_t *buf, int size, int max_data_start);
static inline void app_mbrtu_recv_buf_done(app_mbrtu_recv_buf_t *buf);
static inline void app_mbrtu_recv_buf_reset(app_mbrtu_recv_buf_t *buf);

/*    Initialise a struct termios   */
static int app_mbrtu_termios_init(struct termios *tios, int baud, int parity, int data_bits, int stop_bits);

/*  Determines an acceptable timeout*/
/* NOTE: timeout values passed to modbus_read() lower than the value returned
 *       by this function may result in frames being aborted midway, since they
 *       take at least modbus_get_min_timeout() seconds to transmit.
 */
static double app_mbrtu_get_min_timeout(int baud, int parity, int data_bits, int stop_bits);

/*   Initialise a modbus mbrtu_master data structure   */
static int app_mbrtu_master_data_connect( app_mbrtu_master_node_t *node );
static int app_mbrtu_master_data_free( app_mbrtu_master_node_t *node );
static inline int app_mbrtu_master_data_is_free( app_mbrtu_master_node_t *node );

/* Setting Modbus RTU mbrtu_master Defaults  */
static void app_mbrtu_set_defaults( app_mbrtu_master_node_t *node );

/* Reading of Modbus RTU Frames Auxiliary functions  */
static int app_mbrtu_frame_length(unsigned char *frame_data, int frame_data_length, signed char  *frame_length_array) ;
static int app_mbrtu_search_for_frame(unsigned char *frame_data, int frame_data_length, int *search_history);
static inline void app_mbrtu_next_frame_offset(app_mbrtu_recv_buf_t *buf, unsigned char *slave_id);
static inline int app_mbrtu_return_frame(app_mbrtu_recv_buf_t *buf, int frame_length, unsigned char **recv_data_ptr);
static inline int app_mbrtu_read_frame(app_mbrtu_master_node_t *node, unsigned char **recv_data_ptr, struct timespec *end_time, unsigned char *slave_id);

/* read a modbus frame */
/* NOTE: calling modbus_write() will flush the input stream,
 *       which means any frames that may have arrived
 *       but have not yet been read using modbus_read()
 *       will be permanently lost...
 *
 * NOTE: Ususal select semantics for (a: recv_timeout == NULL) and
 *       (b: *recv_timeout == 0) also apply.
 *
 *       (a) Indefinite timeout
 *       (b) Try once, and and quit if no data available.
 */
/* RETURNS: number of bytes read
 *          -1 on read from file/node error
 *          -2 on timeout
 */
static int app_mbrtu_read(app_mbrtu_master_node_t *node, unsigned char **recv_data_ptr, unsigned short int *transaction_id, const unsigned char *send_data, int send_length, const struct timespec *recv_timeout); 

/* write a modbus frame */
/* WARNING: when calling this function, the *frame_data buffer
 *          must be allocated with an extra *extra_bytes
 *          beyond those required for the frame_length.
 *          This is because the extra bytes will be used
 *          to store the crc before sending the frame.
 *
 */
/* NOTE: calling this function will flush the input stream,
 *       which means any frames that may have arrived
 *       but have not yet been read using modbus_read()
 *       will be permanently lost...
 */
static int app_mbrtu_write(app_mbrtu_master_node_t *node, unsigned char *data, size_t data_length, unsigned short int transaction_id);


/* A Master/Slave transaction... */
static int app_mbrtu_transaction(unsigned char  *packet, int query_length, unsigned char  **data, app_mbrtu_master_node_t *node, int send_retries, unsigned char *error_code, const struct timespec *response_timeout);


/* Construct the required query into a modbus query packet */
static inline int build_query_packet(unsigned char  slave, unsigned char  function, unsigned short int start_addr, unsigned short int count, unsigned char *packet);



/******************************************/
/**                                      **/
/** Handlers that manage the application **/
/**      layer for modbus rtu            **/
/**                                      **/
/******************************************/

/*Diagnostics*/
static void * diagnostics( void *param );
/*Read FIFO queue*/
static void * read_fifo_queue( void *param );
/*Read Write Registers*/
static void * read_write_registers( void *param );
/*Mask write registers*/
static void * mask_write_register( void *param );
/*Write general reference*/
static void * write_general_ref( void *param );
/*Read general reference*/
static void * read_general_ref( void *param );
/*Report Slave ID*/
static void * read_slave_id( void *param );
/*Write Multiple Registers*/
static void * write_multiple_register( void *param );
/*Write multiple coil*/
static void * write_multiple_coil( void *param );
/*Read event log register*/
static void * read_event_log( void *param );
/*Read event count register*/
static void * read_event_counter( void *param );
/*REad exception status register*/
static void * read_exception_status( void *param );
/*Read status io register*/
static void * read_IO_status( void *param );
/* Read registers*/
static void * read_registers( void *param );
/*Write a single register*/
static void * set_single( void *param );



/*************************************/
/**                                 **/
/** Miscellaneous Utility functions **/
/**                                 **/
/*************************************/


/*
 * Function to determine next transaction id.
 *
 * We use a library wide transaction id, which means that we
 * use a new transaction id no matter what slave to which we will
 * be sending the request...
 */
static inline unsigned short int app_mbrtu_next_transaction_id(void)
{

	static unsigned short int next_id = 0;

	return next_id++;
}


/*
 * Functions to convert unsigned short int variables
 * between network and host byte order
 *
 * NOTE: Modbus uses MSByte first, just like
 *       tcp/ip, so we use the htons() and
 *       ntoh() functions to guarantee
 *       code portability.
 */

static inline unsigned short int app_mbrtu_hton(unsigned short int h_value) 
{
	/*  return h_value; */
	return htons(h_value);
}

static inline unsigned short int app_mbrtu_ntoh(unsigned short int m_value) 
{
	/*  return m_value; */
	return ntohs(m_value);
}

/**
 * Return Most Significant Byte of a value
 */
static inline unsigned char app_mbrtu_msb(unsigned short int value) 
{
	return (value >> 8) & 0xFF;
}

/**
 * Return Least Significant Byte of a value
 */
static inline unsigned char app_mbrtu_lsb(unsigned short int value) 
{
	return value & 0xFF;
}
#ifdef INT2BYTE
#define int2byte(char_ptr, short_var) { (char_ptr)[0] = (0x00FF & (short_var)); (char_ptr)[1] = (0x00FF & ((short_var)>>8)); }

#define byte2int(char_ptr, short_var) {(short_var) = ((char_ptr)[0]) & ((char_ptr)[1]<<8);}
#else
#define u16_v(char_ptr)  (*((unsigned short int *)(&(char_ptr))))
#endif


/************************************/
/**                                **/
/**     Time format conversion     **/
/**                                **/
/************************************/

/* Function to load a struct timespec correctly
 * from a double.
 */
static inline struct timespec app_mbrtu_double_to_timespec(double time)
{
	struct timespec tmp;

	tmp.tv_sec  = time;
	tmp.tv_nsec = 1e9*(time - tmp.tv_sec);
	return tmp;
}



/* Function to load a struct timeval correctly
 * from a double.
 */
static inline struct timeval app_mbrtu_double_to_timeval(double time) 
{
	struct timeval tmp;

	tmp.tv_sec  = time;
	tmp.tv_usec = 1e6*(time - tmp.tv_sec);
	return tmp;
}

/* Function to ... */
static inline struct timespec app_mbrtu_timespec_dif(struct timespec ts1, struct timespec ts2)
{
	struct timespec ts;

	ts.tv_sec  = ts1.tv_sec  - ts2.tv_sec;
	if(ts1.tv_nsec > ts2.tv_nsec) {
		ts.tv_nsec = ts1.tv_nsec - ts2.tv_nsec;
	} else {
		ts.tv_nsec = 1000000000 + ts1.tv_nsec - ts2.tv_nsec;
		ts.tv_sec--;
	}

	if (ts.tv_sec < 0)
		ts.tv_sec = ts.tv_nsec = 0;

	return ts;
}

/* Function to ... */
static inline struct timespec app_mbrtu_timespec_add(struct timespec ts1, struct timespec ts2)
{
	struct timespec ts;

	ts.tv_sec  = ts1.tv_sec  + ts2.tv_sec;
	ts.tv_nsec = ts1.tv_nsec + ts2.tv_nsec;
	ts.tv_sec += ts.tv_nsec / 1000000000;
	ts.tv_nsec = ts.tv_nsec % 1000000000;
	return ts;
}

/* Function to convert a struct timespec to a struct timeval. */
static inline struct timeval app_mbrtu_timespec_to_timeval(struct timespec ts)
{
	struct timeval tv;

	tv.tv_sec  = ts.tv_sec;
	tv.tv_usec = ts.tv_nsec/1000;
	return tv;
}


/************************************/
/**                                **/
/** select() with absolute timeout **/
/**                                **/
/************************************/


/* My private version of select using an absolute timeout, instead of the
 * usual relative timeout.
 *
 * NOTE: Ususal select semantics for (a: end_time == NULL) and
 *       (b: *end_time == 0) also apply.
 *
 *       (a) Indefinite timeout
 *       (b) Try once, and and quit if no data available.
 */
/* Returns: -1 on error
 *           0 on timeout
 *          >0 on success
 */
static int app_mbrtu_select(int fd, fd_set *rfds, const struct timespec *end_time)
{

	int res;
	struct timespec cur_time;
	struct timeval timeout, *tv_ptr;
	fd_set tmp_fds;

	/*============================*
	 * wait for data availability *
	 *============================*/
	do {
		tmp_fds = *rfds;
		/* NOTE: To do the timeout correctly we would have to revert to timers
		 *       and asociated signals. That is not very thread friendly, and is
		 *       probably too much of a hassle trying to figure out which signal
		 *       to use. What if we don't have any free signals?
		 *
		 *       The following solution is not correct, as it includes a race
		 *       condition. The following five lines of code should really
		 *       be atomic!
		 *
		 * NOTE: see also the timeout related comment in the
		 *       modbus_tcp_read() function!
		 */
		if (end_time == NULL) {
			tv_ptr = NULL;
		} else {
			tv_ptr = &timeout;
			if ((end_time->tv_sec == 0) && (end_time->tv_nsec == 0)) {
				timeout.tv_sec = timeout.tv_usec = 0;
			} else {
				/* ATOMIC - start */
				if (clock_gettime(CLOCK_REALTIME, &cur_time) < 0)
					return -1;
				timeout = app_mbrtu_timespec_to_timeval(app_mbrtu_timespec_dif(*end_time, cur_time));
			}
		}

		res = select(fd, &tmp_fds, NULL, NULL, tv_ptr);
		/* ATOMIC - end */

		if (res == 0) {
#ifdef MODBUS_DEBUG
			fprintf(stderr,"Comms time out\n");
#endif
			return 0;
		}
		if ((res < 0) && (errno != EINTR)) {
			return -1;
		}
	} while (res <= 0);

	*rfds = tmp_fds;
	return res;
}


/**************************************/
/**                                  **/
/** CRC  functions                   **/
/**                                  **/
/**************************************/


/**     Read the CRC of a frame    **/
/* NOTE: cnt is number of bytes in the frame _excluding_ CRC! */
static inline unsigned short int app_mbrtu_crc_read(unsigned char *buf, int cnt)
{
	/* For some strange reason, the crc is transmited
	 * LSB first, unlike all other values...
	 */
	return (buf[cnt + 1] << 8) | buf[cnt];
}

/**    Write the CRC of a frame    **/
/* NOTE: cnt is number of bytes in the frame _excluding_ CRC! */
static inline void app_mbrtu_crc_write(unsigned char *buf, int cnt)
{
	/* For some strange reason, the crc is transmited
	 * LSB first, unlike all other values...
	 */
	unsigned short int crc = app_mbrtu_crc_calc_fast(buf, cnt);
	buf[cnt]   = app_mbrtu_lsb(crc);
	buf[cnt+1] = app_mbrtu_msb(crc);
}

/** crc optimized for speed **/
static unsigned short int app_mbrtu_crc_calc_fast(unsigned char *buf, int cnt)
{
	/* *buf: message to calculate CRC upon, cnt message lenght */
	unsigned char crc_msb = 0xFF;
	unsigned char crc_lsb = 0xFF;
	int index;

	if (cnt <= 0) {
		fprintf(stderr, "\nInternal program error in file %s at line %d\n\n\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
	}

	while (cnt-- != 0) {
		index = 2 * (crc_lsb ^ *buf++);
		crc_lsb = crc_msb ^ crc_fast_buf[index];
		crc_msb = crc_fast_buf[index + 1];
	}

	return crc_msb*0x0100 + crc_lsb;
}


/**  init() functions of fast CRC version  **/
static inline int app_mbrtu_crc_fast_init(void)
{
	int i;
	unsigned char  data[2];
	unsigned short int tmp_crc;

	if ((crc_fast_buf = (unsigned char *)malloc(256 * 2)) == NULL)
		return -1;

	for (i = 0x00; i < 0x100; i++) {
		data[0] = 0xFF;
		data[1] = i;
		data[1] = ~data[1];
		tmp_crc = app_mbrtu_crc_slow(data, 2);
		crc_fast_buf[2*i    ] = app_mbrtu_lsb(tmp_crc);
		crc_fast_buf[2*i + 1] = app_mbrtu_msb(tmp_crc);
	}

	return 0;
}

/**  done() functions of fast CRC version  **/
static inline void app_mbrtu_crc_fast_done(void) {
	free(crc_fast_buf);
}


/** crc optimized for smallest memory footprint **/
static unsigned short int app_mbrtu_crc_slow(unsigned char *buf, int cnt)
{
	int bit;
	unsigned short int temp,flag;

	temp=0xFFFF;

	while (cnt-- != 0) {
		temp=temp ^ *buf++;
		for (bit=1; bit<=8; bit++) {
			flag = temp & 0x0001;
			/* NOTE:
			 *  - since temp is unsigned, we are guaranteed a zero in MSbit;
			 *  - if it were signed, the value placed in the MSbit would be
			 *      compiler dependent!
			 */
			temp >>= 1;
			if (flag)
				temp=temp ^ 0xA001;
		}
	}
	return(temp);
}

/**************************************/
/**                                  **/
/** linear buffer  functions         **/
/**                                  **/
/**************************************/

/* An unbounded FIFO data structure.
 *
 * The user/caller writes and reads directly from the data structure's buffer,
 * which eliminates slow copying of bytes between the user's and the structure's
 * local memory.
 *
 *
 * The FIFO is implemented by allocating more memory than the maximum number
 * of bytes it will ever hold, and using the extra bytes at the top of the
 * array as the bottom data bytes are released. When we run out of extra bytes,
 * (actually, when the number of un-used bytes at the beginning is larger than
 * a configured maximum), the whole data is moved down, freeing once again the
 * extra bytes at the top of the array.
 *
 * Remember that we can optimize the data structure so that whenever it becomes
 * empty, we can reset it to start off at the bottom of the byte array, i.e. we
 * can set the start = end = 0; instead of simply setting the start = end, which
 * may point to any position in the array.
 *
 *
 * The user is expected to call
 *   lb_init() -> to initialize the structure
 *   lb_done() -> to free the data structure's memory
 *
 * The user can store data starting off from...
 *   lb_free() -> pointer to address of free memory
 *   lb_free_count() -> number of free bytes available
 * and then call
 *   lb_data_add()
 * to add the data to the data structure
 *
 * Likewise, the user can read the data directly from
 *   lb_data() -> pointer to address of data location
 *   lb_free_count() -> number of data bytes available
 * and free the data using
 *   lb_data_purge()
 * to remove the data from the data structure
 */

static inline unsigned char *app_mbrtu_lb_init(app_mbrtu_lb_buf_t *buf, int size, int max_data_start)
{
	if (size <= 0)
		return NULL;

	if (max_data_start >= size)
		max_data_start = size - 1;

	buf->data_size  = size;
	buf->data_start = 0;
	buf->data_end   = 0;
	buf->max_data_start = max_data_start;
	buf->data = (unsigned char *)calloc(size, sizeof(unsigned char));
	return buf->data;
}

static inline void app_mbrtu_lb_done(app_mbrtu_lb_buf_t *buf)
{
	free(buf->data);
	buf->data = NULL;
}

static inline unsigned char *app_mbrtu_lb_normalize(app_mbrtu_lb_buf_t *buf)
{
	return (unsigned char *)memmove(buf->data,
			buf->data + buf->data_start,
			buf->data_end - buf->data_start);
}

static inline unsigned char *app_mbrtu_lb_data(app_mbrtu_lb_buf_t *buf)
{
	return buf->data + buf->data_start;
}

static inline int app_mbrtu_lb_data_count(app_mbrtu_lb_buf_t *buf)
{
	return buf->data_end - buf->data_start;
}

static inline void app_mbrtu_lb_data_add(app_mbrtu_lb_buf_t *buf, int count)
{
	if ((buf->data_end += count) >= buf->data_size)
		buf->data_end = buf->data_size - 1;
}

static inline unsigned char *app_mbrtu_lb_data_purge(app_mbrtu_lb_buf_t *buf, int count) 
{
	buf->data_start += count;
	if (buf->data_start > buf->data_end)
		buf->data_start = buf->data_end;

	if ((buf->data_end == buf->data_size) || (buf->data_start >= buf->max_data_start))
		return app_mbrtu_lb_normalize(buf);

	return buf->data + buf->data_start;
}

static inline void app_mbrtu_lb_data_purge_all(app_mbrtu_lb_buf_t *buf) 
{
	buf->data_start = buf->data_end = 0;
}

static inline unsigned char *app_mbrtu_lb_free(app_mbrtu_lb_buf_t *buf) 
{
	return buf->data + buf->data_end;
}

static inline int app_mbrtu_lb_free_count(app_mbrtu_lb_buf_t *buf) {
	return buf->data_size - buf->data_end;
}

/**************************************/
/**                                  **/
/** Receive buffer  functions        **/
/**                                  **/
/**************************************/

/* Init buffer */
static inline unsigned char *app_mbrtu_recv_buf_init(app_mbrtu_recv_buf_t *buf, int size, int max_data_start)
{
	buf->found_frame_boundary = 0;
	buf->frame_search_history = 0;
	return app_mbrtu_lb_init(&buf->data_buf, size, max_data_start);
}


/* Quit buffer */
static inline void app_mbrtu_recv_buf_done(app_mbrtu_recv_buf_t *buf)
{
	buf->found_frame_boundary = 0;
	buf->frame_search_history = 0;
	app_mbrtu_lb_done(&buf->data_buf);
}


/* Reset buffer */
static inline void app_mbrtu_recv_buf_reset(app_mbrtu_recv_buf_t *buf)
{
	buf->found_frame_boundary = 0;
	buf->frame_search_history = 0;
	app_mbrtu_lb_data_purge_all(&buf->data_buf);
}

/**************************************/
/**                                  **/
/**    Initialise a struct termios   **/
/**                                  **/
/**************************************/
static int app_mbrtu_termios_init(struct termios *tios, int baud, int parity, int data_bits, int stop_bits) 
{
	speed_t baud_rate;

	if (tios == NULL)
		return -1;

	/* reset all the values... */
	/* NOTE: the following are initialised later on...
	   tios->c_iflag = 0;
	   tios->c_oflag = 0;
	   tios->c_cflag = 0;
	   tios->c_lflag = 0;
	 */
	tios->c_line  = 0;

	/* The minimum number of characters that should be received
	 * to satisfy a call to read().
	 */
	tios->c_cc[VMIN ] = 0;

	/* The maximum inter-arrival interval between two characters,
	 * in deciseconds.
	 *
	 * NOTE: we could use this to detect the end of RTU frames,
	 *       but we prefer to use select() that has higher resolution,
	 *       even though this higher resolution is most probably not
	 *       supported, and the effective resolution is 10ms,
	 *       one tenth of a decisecond.
	 */
	tios->c_cc[VTIME] = 0;

	/* configure the input modes... */
	tios->c_iflag =  IGNBRK |  /* ignore BREAK condition on input         */
		IGNPAR |  /* ignore framing errors and parity errors */
		IXANY;    /* enable any character to restart output  */
	/*               BRKINT       Only active if IGNBRK is not set.
	 *                            generate SIGINT on BREAK condition,
	 *                            otherwise read BREAK as character \0.
	 *               PARMRK       Only active if IGNPAR is not set.
	 *                            replace bytes with parity errors with
	 *                            \377 \0, instead of \0.
	 *               INPCK        enable input parity checking
	 *               ISTRIP       strip off eighth bit
	 *               IGNCR        ignore carriage return on input
	 *               INLCR        only active if IGNCR is not set.
	 *                            translate newline to carriage return  on input
	 *               ICRNL        only active if IGNCR is not set.
	 *                            translate carriage return to newline on input
	 *               IUCLC        map uppercase characters to lowercase on input
	 *               IXON         enable XON/XOFF flow control on output
	 *               IXOFF        enable XON/XOFF flow control on input
	 *               IMAXBEL      ring bell when input queue is full
	 */

	/* configure the output modes... */
	tios->c_oflag = OPOST;     /* enable implementation-defined output processing */
	/*              ONOCR         don't output CR at column 0
	 *              OLCUC         map lowercase characters to uppercase on output
	 *              ONLCR         map NL to CR-NL on output
	 *              OCRNL         map CR to NL on output
	 *              OFILL         send fill characters for a delay, rather than
	 *                            using a timed delay
	 *              OFDEL         fill character is ASCII DEL. If unset, fill
	 *                            character is ASCII NUL
	 *              ONLRET        don't output CR
	 *              NLDLY         NL delay mask. Values are NL0 and NL1.
	 *              CRDLY         CR delay mask. Values are CR0, CR1, CR2, or CR3.
	 *              TABDLY        horizontal tab delay mask. Values are TAB0, TAB1,
	 *                            TAB2, TAB3, or XTABS. A value of XTABS expands
	 *                            tabs to spaces (with tab stops every eight columns).
	 *              BSDLY         backspace delay mask. Values are BS0 or BS1.
	 *              VTDLY         vertical tab delay mask. Values are VT0 or VT1.
	 *              FFDLY         form feed delay mask. Values are FF0 or FF1.
	 */

	/* configure the control modes... */
	tios->c_cflag = CREAD |    /* enable receiver.               */
		CLOCAL;    /* ignore modem control lines     */
	/*              HUPCL         lower modem control lines after last process
	 *                            closes the device (hang up).
	 *  CRTSCTS   flow control (Request/Clear To Send).
	 */
	if (data_bits == 5) tios->c_cflag |= CS5;
	else if (data_bits == 6) tios->c_cflag |= CS6;
	else if (data_bits == 7) tios->c_cflag |= CS7;
	else if (data_bits == 8) tios->c_cflag |= CS8;
	else return -1;

	if (stop_bits == 1) tios->c_cflag &=~ CSTOPB;
	else if (stop_bits == 2) tios->c_cflag |= CSTOPB;
	else return -1;

	if(parity == 0) { /* none */
		tios->c_cflag &=~ PARENB;
		tios->c_cflag &=~ PARODD;
	} else if(parity == 2)  { /* even */
		tios->c_cflag |= PARENB;
		tios->c_cflag &=~ PARODD;
	} else if(parity == 1)  { /* odd */
		tios->c_cflag |= PARENB;
		tios->c_cflag |= PARODD;
	} else return -1;


	/* configure the local modes... */
	tios->c_lflag = IEXTEN;    /* enable implementation-defined input processing   */
	/*              ISIG          when any of the characters INTR, QUIT, SUSP, or DSUSP
	 *                            are received, generate the corresponding signal.
	 *              ICANON        enable canonical mode. This enables the special
	 *                            characters EOF, EOL, EOL2, ERASE, KILL, REPRINT,
	 *                            STATUS, and WERASE, and buffers by lines.
	 *              ECHO          echo input characters.
	 */

	/* Set the baud rate */
	/* Must be done before reseting all the values to 0! */
	switch(baud) {
		case 110:    baud_rate = B110;    break;
		case 300:    baud_rate = B300;    break;
		case 600:    baud_rate = B600;    break;
		case 1200:   baud_rate = B1200;   break;
		case 2400:   baud_rate = B2400;   break;
		case 4800:   baud_rate = B4800;   break;
		case 9600:   baud_rate = B9600;   break;
		case 19200:  baud_rate = B19200;  break;
		case 38400:  baud_rate = B38400;  break;
		case 57600:  baud_rate = B57600;  break;
		case 115200: baud_rate = B115200; break;
		default: return -1;
	} /* switch() */

	if ((cfsetispeed(tios, baud_rate) < 0) ||
			(cfsetospeed(tios, baud_rate) < 0))
		return -1;;

	return 0;
}

/****************************************************/
/**                                                **/
/**    Initialise a modbus mbrtu_master data structure   **/
/**                                                **/
/****************************************************/


static int app_mbrtu_master_data_connect( app_mbrtu_master_node_t *node )
{

	int parity_bits, start_bits, char_bits;
	struct termios settings;
	int buf_size;


	if (node->fd >= 0)
		goto error_exit_0;

	/* initialise the termios data structure */
	if (app_mbrtu_termios_init(&settings,
				node->baud,
				node->parity,
				node->data_bits,
				node->stop_bits)
			< 0) {
		fprintf(stderr, "Invalid serial line settings"
				"(baud=%d, parity=%d, data_bits=%d, stop_bits=%d)\n",
				node->baud,
				node->parity,
				node->data_bits,
				node->stop_bits);
		goto error_exit_1;
	}


	/* initialise recv buffer */
	buf_size = RECV_BUFFER_SIZE_LARGE;
	if (app_mbrtu_recv_buf_init(&node->recv_buf, buf_size, buf_size - MAX_RTU_FRAME_LENGTH)
			== NULL) {
		fprintf(stderr, "Out of memory: error initializing receive buffer\n");
		goto error_exit_2;
	}

	/* open the serial port */
	if((node->fd = open(node->device, O_RDWR | O_NOCTTY | O_NDELAY))
			< 0) {
		fprintf(stderr, "Error opening device %s (errno=%d)\n", node->device, errno);
		goto error_exit_3;
	}

	if(tcgetattr(node->fd, &node->old_tty_settings) < 0) {
		fprintf(stderr, "Error reading device's %s original settings.\n", node->device);
		goto error_exit_4;
	}

	if(tcsetattr(node->fd, TCSANOW, &settings) < 0) {
		fprintf(stderr, "Error configuring device %s"
				"(baud=%d, parity=%d, data_bits=%d, stop_bits=%d)\n",node->device, node->baud, node->parity, node->data_bits, node->stop_bits);
		goto error_exit_4;
	}

	parity_bits   = (node->parity == 0)?0:1;
	start_bits    = 1;
	char_bits     = start_bits  + node->data_bits + parity_bits + node->stop_bits;
	node->time_15_char = app_mbrtu_double_to_timeval(1.5*char_bits/node->baud);
	node->time_35_char = app_mbrtu_double_to_timeval(3.5*char_bits/node->baud);
	//node->time_35_char = app_mbrtu_double_to_timeval(4*3.5*char_bits/node->baud);

#ifdef MODBUS_DEBUG
	fprintf(stderr, "app_mbrtu_master_data_connect(): %s open\n", node->device );
	fprintf(stderr, "app_mbrtu_master_data_connect(): returning fd=%d\n", node->fd);
#endif
	return node->fd;

error_exit_4:
	close(node->fd);
error_exit_3:
	app_mbrtu_recv_buf_done(&node->recv_buf);
error_exit_2:
error_exit_1:
	node->fd = -1; /* set the node as free... */
error_exit_0:
	return -1;
}

static int app_mbrtu_master_data_free(app_mbrtu_master_node_t *node)
{
	if (node->fd < 0)
		/* already free */
		return -1;

	/* reset the tty device old settings... */
	if(tcsetattr(node->fd, TCSANOW, &node->old_tty_settings) < 0)
		fprintf(stderr, "Error reconfiguring serial port to it's original settings.\n");

	app_mbrtu_recv_buf_done(&node->recv_buf);
	if (close(node->fd) != 0)
	{
#ifdef MODBUS_DEBUG
		fprintf(stderr, "[%s]: cannot close device %s : %s\n", __func__, node->device, strerror(errno));
#endif
		return -1;
	}
	node->fd = -1;

	return 0;
}

static inline int app_mbrtu_master_data_is_free(app_mbrtu_master_node_t *node)
{
	return (node->fd < 0);
}

/************************************************************************/
/****                                                                ****/
/****        Initialising and Shutting Down Modbus RTU mbrtu_master  ****/
/****                                                                ****/
/************************************************************************/

/******************************/
/**                          **/
/**   Load Default Values    **/
/**                          **/
/******************************/

static void app_mbrtu_set_defaults( app_mbrtu_master_node_t *node )
{
	//int default_value = 0;
	fprintf(stderr, "[%s] - enter...\n", __func__);
	//fprintf(stderr, "[%s] - %d baud %p\n", __func__, __LINE__,baud);
	//fprintf(stderr, "[%s] - %d baud %d\n", __func__, __LINE__,*baud);
	/* Set the default values, if required... */
	if (node->baud == -1)
	{
		//*baud = DEFAULT_BAUD_RATE;
		node->baud = DEFAULT_BAUD_RATE;
#if 0
	fprintf(stderr, "[%s] - %d\n", __func__, __LINE__);
		default_value=DEFAULT_BAUD_RATE;
	fprintf(stderr, "[%s] - %d\n", __func__, __LINE__);
		memcpy(baud,  &default_value, sizeof(int));
#endif
	fprintf(stderr, "[%s] - %d\n", __func__, __LINE__);
	}
	fprintf(stderr, "[%s] - %d\n", __func__, __LINE__);
	if (node->data_bits == -1 )
	{
		node->data_bits = DEFAULT_DATA_BITS;
#if 0
		default_value=DEFAULT_DATA_BITS;
		memcpy(data_bits,  &default_value, sizeof(int));
#endif
	}
	fprintf(stderr, "[%s] - %d\n", __func__, __LINE__);
	if (node->parity ==  -1)
	{
		node->parity = 0;
#if 0
		default_value=0;
		memcpy(parity,  &default_value, sizeof(int));
#endif
	}
	fprintf(stderr, "[%s] - %d\n", __func__, __LINE__);
	if (node->stop_bits == -1)
	{
		fprintf(stderr, "[%s] - %d\n", __func__, __LINE__);
		if (node->parity == 0)
		{
			node->stop_bits = DEFAULT_STOP_BITS_NOPARITY; /* no parity */
#if 0
			default_value=DEFAULT_STOP_BITS_NOPARITY;
			memcpy(stop_bits,  &default_value, sizeof(int));
#endif
			fprintf(stderr, "[%s] - %d\n", __func__, __LINE__);
		}
		else
		{
			node->stop_bits = DEFAULT_STOP_BITS_PARITY; /* parity used */
#if 0
			default_value=DEFAULT_STOP_BITS_PARITY;
			memcpy(stop_bits,  &default_value, sizeof(int));
#endif
			fprintf(stderr, "[%s] - %d\n", __func__, __LINE__);
		}
	}
	fprintf(stderr, "[%s] - exit...\n", __func__);
}


/******************************/
/**                          **/
/**    Initialise Library    **/
/**                          **/
/******************************/

int app_mbrtu_init_cfg(int confbaud, int confdatabits, int confparity, int confstopbits, int ignore_echo)
{
#ifdef MODBUS_DEBUG
	fprintf(stderr, "[%s]: called...\n", __func__);
#endif

	if (mbrtu_master != NULL)
	{
#ifdef MODBUS_DEBUG
		fprintf(stderr, "[%s]: already initialized\n", __func__);
#endif
		/* already initialized... */
		return -1;
	}

	if (app_mbrtu_crc_fast_init() < 0) {
		fprintf(stderr, "Out of memory: error initializing crc buffers\n");
		goto error_exit_0;
	}

	/* allocate mbrtu_master data structure */
	if ((mbrtu_master = calloc(1, sizeof(app_mbrtu_master_node_t))) == NULL) {
		fprintf(stderr, "Out of memory: error initializing node table.\n");
		goto error_exit_1;
	}

	/* initialise mbrtu_master node file descriptor and serial port to be used */
	mbrtu_master->fd = -1;
	mbrtu_master->baud = confbaud;
	mbrtu_master->parity = confparity;
	mbrtu_master->data_bits = confdatabits;
	mbrtu_master->stop_bits = confstopbits;
	mbrtu_master->ignore_echo = ignore_echo;
	strcpy (mbrtu_master->device, DEFAULT_SERIAL_PORT);

#ifdef MODBUS_DEBUG
	fprintf(stderr, "[%s]: returning successfully...\n", __func__);
#endif

	/* initialize status  data structure */
	app_mbrtu_status[0] = app_mbrtu_status[1] =  app_mbrtu_status[2] = 0;

	return 0;

error_exit_1:
	free(mbrtu_master); mbrtu_master = NULL;
error_exit_0:
	return -1;
}

int app_mbrtu_init(void)
{
	int d_min_timeout;
	fprintf(stderr, "[%s]: modbus init...\n", __func__);
	if (app_config_load(APP_CONF_MB) != 0)
	{
		fprintf(stderr, "[%s]: Error loading config file.\n", __func__);
		return -1;
	}
	if ( mbrtu_cfg.serial_cfg.enabled == 0 )
	{
		fprintf(stderr, "[%s]: Warning Modbus module is build but is not used: abort initialization.\n", __func__);
		return 0;
	}
	if ( app_mbrtu_init_cfg(mbrtu_cfg.serial_cfg.baud, mbrtu_cfg.serial_cfg.databits, mbrtu_cfg.serial_cfg.parity, mbrtu_cfg.serial_cfg.stopbits, mbrtu_cfg.serial_cfg.ignore_echo) != 0)
	{
		fprintf(stderr, "[%s]: Error initialize mbrtu.\n", __func__);
		return -2;
	}
	if ( app_mbrtu_connect( mbrtu_master ) <= 0 )
	{
		fprintf(stderr, "[%s]: Error connecting mbrtu.\n", __func__);
		return -3;
	}
	d_min_timeout = app_mbrtu_get_min_timeout(mbrtu_master->baud, mbrtu_master->parity, mbrtu_master->data_bits, mbrtu_master->stop_bits);
	app_mbrtu_min_timeout = app_mbrtu_double_to_timespec(d_min_timeout);
	app_mbrtu_params.response_timeout = &app_mbrtu_min_timeout;
	fprintf(stderr, "[%s]: done.\n", __func__);
	return 0;
}



/******************************/
/**                          **/
/**    Open node descriptor  **/
/**                          **/
/******************************/

/* Open a node for mbrtu_master operation.
 * Returns the node descriptor, or -1 on error.
 *
 */
int app_mbrtu_connect(app_mbrtu_master_node_t *node) 
{
	/* set the default values... */
	fprintf(stderr, "[%s] - node %p\n", __func__, node);
	fprintf(stderr, "[%s] - node %p\n", __func__, node->baud);
	fprintf(stderr, "[%s] - node %p\n", __func__, &(node->baud));
	fprintf(stderr, "[%s] - node %d\n", __func__, node->baud);
#if 0
	app_mbrtu_set_defaults(&(node->baud),
			&(node->parity),
			&(node->data_bits),
			&(node->stop_bits));
#endif
	app_mbrtu_set_defaults(node);
#ifdef MODBUS_DEBUG
	fprintf(stderr, "[%s] - called...\n", __func__);
	fprintf(stderr, "opening %s\n", node->device);
	fprintf(stderr, "baud_rate = %d\n", node->baud);
	fprintf(stderr, "parity = %d\n", node->parity);
	fprintf(stderr, "data_bits = %d\n", node->data_bits);
	fprintf(stderr, "stop_bits = %d\n", node->stop_bits);
	fprintf(stderr, "ignore_echo = %d\n", node->ignore_echo);
#endif

	if (app_mbrtu_master_data_connect( node ) < 0)
		goto error_exit_0;

#ifdef MODBUS_DEBUG
	fprintf(stderr, "[%s] - %s open\n", __func__, node->device);
	fprintf(stderr, "[%s] - returning fd=%d\n", __func__, node->fd);
#endif
	return node->fd;

error_exit_0:
#ifdef MODBUS_DEBUG
	fprintf(stderr, "[%s] - error!\n", __func__);
#endif
	return -1;
}


/************************************/
/**                                **/
/**   Shutdown Modbus mbrtu_master **/
/**                                **/
/************************************/

	int app_mbrtu_done(void) {
		if ( mbrtu_cfg.serial_cfg.enabled == 0 )
		{
			return 0;
		}
		if (mbrtu_master == NULL)
			return 0;

		app_mbrtu_master_data_free(mbrtu_master);
		free(mbrtu_master); mbrtu_master = NULL;
		app_mbrtu_crc_fast_done();
#ifdef MODBUS_DEBUG
		fprintf(stderr, "app_mbrtu_done(): Everithing should be closed!\n");
#endif  

		return 0;
	}

/*********************************************/
/**                                         **/
/**    Get min timeout                      **/
/**                                         **/
/*********************************************/


static double app_mbrtu_get_min_timeout(int baud, int parity, int data_bits, int stop_bits) 
{
	int parity_bits, start_bits, char_bits;
	char_bits = 8;

	double timeout = (double)( 20.0 * (((float)(MAX_RTU_FRAME_LENGTH * char_bits)) / ((float)baud))  );
#ifdef MODBUS_DEBUG
	printf("TIMEOUT: %d", timeout);
#endif
	return timeout;
}



/**************************************************************/
/**                                                          **/
/**                Sending of Modbus RTU Frames              **/
/**                                                          **/
/**************************************************************/

/*  W A R N I N G
 *  =============
 *     The app_mbrtu_write() function assumes that the caller
 *     has allocated a few bytes extra for the buffer containing
 *     the data. These bytes will be used to write the crc.
 *
 *     The caller of this function MUST make sure that the data
 *     buffer, although only containing data_length bytes, has
 *     been allocated with a size equal to or larger than
 *     data_length + RTU_FRAME_CRC_LENGTH bytes
 *
 */
static int app_mbrtu_write(app_mbrtu_master_node_t *node, unsigned char *data, size_t data_length, unsigned short int transaction_id)
{
	fd_set rfds;
	struct timeval timeout;
	int res, send_retries;

	/* check if fd is initialzed... */
	if (node->fd < 0)
	{
		return -1;
	}

	/**************************
	 * append crc to frame... *
	 **************************/
	/* WARNING:
	 *     The app_mbrtu_crc_write() function assumes that we have an extra
	 *     RTU_FRAME_CRC_LENGTH free bytes at the end of the *data
	 *     buffer.
	 *
	 * REASONS:
	 *     We want to write the data and the crc in a single call to
	 *     the OS. This is the only way we can minimally try to gurantee
	 *     that we will not be introducing a silence of more than 1.5
	 *     character transmission times between any two characters.
	 *
	 *     We could do the above using one of two methods:
	 *       (a) use a special writev() call in which the data
	 *           to be sent is stored in two buffers (one for the
	 *           data and the other for the crc).
	 *       (b) place all the data in a single linear buffer and
	 *           use the normal write() function.
	 *
	 *     We cannot use (a) since the writev(2) function does not seem
	 *     to be POSIX compliant...
	 *     (b) has the drawback that we would need to allocate a new buffer,
	 *      and copy all the data into that buffer. We have enough copying of
	 *      data between buffers as it is, so we won't be doing it here
	 *      yet again!
	 *
	 *      The only option that seems left over is to have the caller
	 *      of this function allocate a few extra bytes. 
	 */
	app_mbrtu_crc_write(data, data_length); 
	data_length += RTU_FRAME_CRC_LENGTH; 

#ifdef MODBUS_DEBUG
	/* Print the hex value of each character that is about to be
	 * sent over the bus.
	 */
	{ int i;
		fprintf(stderr,"TX: ");
		for(i = 0; i < data_length; i++)
			fprintf(stderr,"[0x%2X]", data[i]);
		fprintf(stderr,"\n");
	}
#endif
//#define SWAP_RTS
#ifdef SWAP_RTS
			ioctl(node->fd, TIOCMGET, &res);

			fprintf(stderr,"[%s] - OCCUPY THE LINE SWAP RTS: %x", __func__, res);
			res |= TIOCM_RTS;
			//res &= ~TIOCM_RTS;

			ioctl(node->fd, TIOCMSET, &res);

			fprintf(stderr," -> %x", res);

			ioctl(node->fd, TIOCMGET, &res);

			fprintf(stderr,"(%x)\n", res);
#endif

	/* THE MAIN LOOP!!! */
	/* NOTE: The modbus standard specifies that the message must
	 *       be sent continuosly over the wire with maximum
	 *       inter-character delays of 1.5 character intervals.
	 *
	 *       If the write() call is interrupted by a signal, then
	 *       this delay will most probably be exceeded. We should then
	 *       re-start writing the query from the begining.
	 *
	 *       BUT, can we really expect the write() call to return
	 *       query_length on every platform when no error occurs?
	 *       The write call would still be correct if it only wrote
	 *       1 byte at a time!
	 *
	 *       To avoid an infinte loop in the
	 *       above cases, we specify a maximum number of retries, and
	 *       hope for the best...! The worst will now be we simply do
	 *       not get to send out a whole frame, and will therefore always
	 *       fail on writing a modbus frame!
	 */
	send_retries = RTU_FRAME_SEND_RETRY + 1; /* must try at least once... */
	while (send_retries > 0) {

		/*******************************
		 * synchronise with the bus... *
		 *******************************/
		/* Remember that a RS485 bus is half-duplex, so we have to wait until
		 * nobody is transmitting over the bus for our turn to transmit.
		 * This will never happen on a modbus network if the mbrtu_master and
		 * slave state machines never get out of synch, but we want
		 * to make sure we can re-synchronise if they ever do get out of synch.
		 *
		 * The following lines will guarantee that we will re-synchronise our
		 * state machine with the current state of the bus.
		 *
		 * We first wait until the bus has been silent for at least
		 * char_interval_timeout (i.e. 3.5 character interval). We then flush
		 * any input and output that might be on the cache.
		 */
		/* NOTES:
		 *   - we do not need to reset the rfds with FD_SET(ttyfd, &rfds)
		 *     before every call to select! We only wait on one file descriptor,
		 *     so if select returns succesfully, it must have that same file
		 *     decriptor set in the rdfs!
		 *     If select returns with a timeout, then we do not get to call
		 *     select again!
		 *   -  On Linux, timeout (i.e. timeout) is modified by select() to
		 *      reflect the amount of time not slept; most other implementations
		 *      do not do this. In the cases in which timeout is not modified,
		 *      we will simply have to wait for longer periods if select is
		 *      interrupted by a signal.
		 */
		FD_ZERO(&rfds);
		FD_SET(node->fd, &rfds);
		timeout = node->time_35_char;
		while ((res = select(node->fd+1, &rfds, NULL, NULL, &timeout)) != 0) {
			if (res > 0) {
				/* we are receiving data over the serial port! */
				/* Throw the data away!                        */
				tcflush(node->fd, TCIFLUSH); /* flush the input stream */
				/* reset the timeout value! */
				timeout = node->time_35_char;
				/* We do not need to reset the FD SET here! */
			} else {
				/* some kind of error ocurred */
				if (errno != EINTR)
					/* we were not interrupted by a signal */
					return -1;
				/* We will be calling select() again.
				 * We need to reset the FD SET !
				 */
				FD_ZERO(&rfds);
				FD_SET(node->fd, &rfds);
			}
		} /* while (select()) */

		/* Flush both input and output streams... */
		/* NOTE: Due to the nature of the modbus protocol,
		 *       when a frame is sent all previous
		 *       frames that may have arrived at the sending node become
		 *       irrelevant.
		 */
		tcflush(node->fd, TCIOFLUSH);       /* flush the input & output streams */
		app_mbrtu_recv_buf_reset(&node->recv_buf);   /* reset the recv buffer            */

		/**********************
		 * write to output... *
		 **********************/
		/* Please see the comment just above the main loop!! */
		if ((res = write(node->fd, data, data_length)) != data_length) {
			if ((res < 0) && (errno != EAGAIN ) && (errno != EINTR ))
			{
				return -1;
			}
		} else {
#if MODBUS_DEBUG
			fprintf(stderr,".........query succesfully sent!..............\n");
#endif      
#ifdef SWAP_RTS
			usleep(5000);
			ioctl(node->fd, TIOCMGET, &res);

			fprintf(stderr,"[%s] -  FREE the LINE SWAP RTS: %x", __func__, res);
			res &= ~TIOCM_RTS;
			//res |= TIOCM_RTS;

			ioctl(node->fd, TIOCMSET, &res);

			fprintf(stderr," -> %x", res);

			ioctl(node->fd, TIOCMGET, &res);

			fprintf(stderr,"(%x)\n", res);
#endif

			/* query succesfully sent! */
			/* res == query_length     */

			/*  NOTE: We do not flush the input stream after sending the frame!
			 *        If the process gets swapped out between the end of writing
			 *        to the serial port, and the call to flush the input of the
			 *        same serial port, the response to the modbus query may be
			 *        sent over between those two calls. This would result in the
			 *        tcflush(ttyfd, TCIFLUSH) call flushing out the response
			 *        to the query we have just sent!
			 *        Not a good thing at all... ;-)
			 */
			return data_length - RTU_FRAME_CRC_LENGTH;
		}
		/* NOTE: The maximum inter-character delay of 1.5 character times
		 *       has most probably been exceeded, so we abort the frame and
		 *       retry again...
		 */
		send_retries--;
	} /* while()  MAIN LOOP */

	/* maximum retries exceeded */
	return -1;  
}



/**********************************************************/
/**                                                      **/
/**              Receiving Modbus RTU Frames             **/
/**                                                      **/
/**********************************************************/


#if     MIN_FRAME_LENGTH < 2
#error  Modbus RTU frames have a minimum length larger than MIN_FRAME_LENGTH.
#endif

/************************************/
/**                                **/
/**     Print recv buffer content   **/
/**       for debug purposes         **/
/**                                **/
/************************************/
#if APP_MBRTU_WORKAROUND
static int app_mbrtu_recv_buffer_print( app_mbrtu_recv_buf_t * recv_buf){


	int i;
	fprintf(stderr, "found_frame_boundary = %d\n", recv_buf->found_frame_boundary);
	fprintf(stderr, "frame_search_history = %d\n", recv_buf->frame_search_history);
	fprintf(stderr, "data_size = %d\n", recv_buf->data_buf.data_size);
	fprintf(stderr, "data_start = %d\n", recv_buf->data_buf.data_start);
	fprintf(stderr, "data_end = %d\n", recv_buf->data_buf.data_end);
	fprintf(stderr, "max_data_start = %d\n", recv_buf->data_buf.max_data_start);
	fprintf(stderr, "PRINTING DATA\n");
	for(i = 0; i < RECV_BUFFER_SIZE_LARGE; i+=8){
		fprintf(stderr, "0x%02x-0x%02x-0x%02x-0x%02x-0x%02x-0x%02x-0x%02x-0x%02x\n",  
				recv_buf->data_buf.data[i], recv_buf->data_buf.data[i+1], recv_buf->data_buf.data[i+2], 
				recv_buf->data_buf.data[i+3], recv_buf->data_buf.data[i+4], recv_buf->data_buf.data[i+5], 
				recv_buf->data_buf.data[i+6], recv_buf->data_buf.data[i+7]);
	}

	return 0;

}
#endif
#if MODBUS_DEBUG
static int app_mbrtu_recv_buffer_print_short( app_mbrtu_recv_buf_t * recv_buf){


	fprintf(stderr, "found_frame_boundary = %d\n", recv_buf->found_frame_boundary);
	fprintf(stderr, "frame_search_history = %d\n", recv_buf->frame_search_history);
	fprintf(stderr, "data_size = %d\n", recv_buf->data_buf.data_size);
	fprintf(stderr, "data_start = %d\n", recv_buf->data_buf.data_start);
	fprintf(stderr, "data_end = %d\n", recv_buf->data_buf.data_end);
	fprintf(stderr, "max_data_start = %d\n", recv_buf->data_buf.max_data_start);

	return 0;

}
#endif

/************************************/
/**                                **/
/**     Guess length of frame      **/
/**         being read.            **/
/**                                **/
/************************************/

/*  Auxiliary function to the search_for_frame() function.
 *
 *  NOTE: data_byte_count must be >=2 for correct operation, therefore
 *        the #error condition above.
 *
 *  Function to determine the length of the frame currently being read,
 *  assuming it is a query/response frame.
 *
 *  The guess is obtained by analysing the bytes that have already been
 *  read. Sometimes we cannot be sure what is the frame length, because
 *  not enough bytes of the frame have been read yet (for example, frames
 *  that have a byte_count value which has not yet been read). In these
 *  cases we return not the frame length, but an error (-1).
 *
 *  If we find the data does not make any sense (i.e. it cannot be a valid
 *  modbus frame), we return -1.
 */
/* The array containing the lengths of frames. */
/*   - query_frame_length[]
 *   - response_frame_length[]
 */

static int app_mbrtu_frame_length(unsigned char *frame_data, int frame_data_length, signed char  *frame_length_array) 
{

	unsigned char  function_code;
	int res;

	/* check consistency of input parameters... */

	if ((frame_data == NULL) || (frame_length_array == NULL) || (frame_data_length < 2))
		return -1;


	function_code = frame_data[APPLICATION_FRAME_FUNCTION_OFS];

	/* hard code the length of response to diagnostic function 8 (0x08), with
	 * subfunction 21 (0x15), and sub-sub-function (a.k.a. operation) 3 (0x03),
	 * which contains a byte count...
	 */
	if ((function_code == 0x08) && (frame_length_array == response_frame_lengths)) {
		if (frame_data_length < 4) {
			/* not enough info to determine the sub-function... */
			return -1;
		} else {
			if ((frame_data[2] == 0x00) && (frame_data[3] == 0x15)) {
				/* we need a couple more bytes to figure out the sub-sub-function... */
				if (frame_data_length < 6) {
					/* not enough info to determine the sub-sub-function... */
					return -1;
				} else {
					if ((frame_data[4] == 0x00) && (frame_data[5] == 0x03)) {
						/* We have found a response frame to diagnostic sub-function ... */
						if (frame_data_length < 8) {
							/* not enough info to determine the frame length */
							return -1;
						} else {
#ifdef INT2BYTE
							unsigned short int value;
							byte2int(&frame_data[6], value);
							return /*HEADER*/ 6 + app_mbrtu_ntoh(value) + RTU_FRAME_CRC_LENGTH;
#else
							return /*HEADER*/ 6 + app_mbrtu_ntoh(u16_v(frame_data[6])) + RTU_FRAME_CRC_LENGTH;
#endif
						}
					}
				}
			}
		}
	}

	res = frame_length_array[function_code];

	switch(res) {
		case BYTE_COUNT_3 :
			if (frame_data_length >= 3)
				return BYTE_COUNT_3_HEADER  + frame_data[2] + RTU_FRAME_CRC_LENGTH;
			break;
		case BYTE_COUNT_34:
			if (frame_data_length >= 4)
			{
#ifdef INT2BYTE
				unsigned short int value;
				byte2int(&frame_data[2], value);
				return BYTE_COUNT_34_HEADER + app_mbrtu_ntoh(value) + RTU_FRAME_CRC_LENGTH;
#else
				return BYTE_COUNT_34_HEADER + app_mbrtu_ntoh(u16_v(frame_data[2])) + RTU_FRAME_CRC_LENGTH;
#endif
			}
			break;
		case BYTE_COUNT_7 :
			if (frame_data_length >= 7)
				return BYTE_COUNT_7_HEADER  + frame_data[6] + RTU_FRAME_CRC_LENGTH;
			break;
		case BYTE_COUNT_11:
			if (frame_data_length >= 11)
				return BYTE_COUNT_11_HEADER + frame_data[10] + RTU_FRAME_CRC_LENGTH;
			break;
		case BYTE_COUNT_U :
			return -1;
		default:
			return res + RTU_FRAME_CRC_LENGTH;
	} /* switch() */

	/* unknown frame length */
	return -1;
}



/************************************/
/**                                **/
/**      Search for a frame        **/
/**                                **/
/************************************/

/* Search for a valid frame in the current data.
 * If no valid frame is found, then we return -1.
 *
 * NOTE: Since frame verification is done by calculating the CRC, which is rather
 *       CPU intensive, and this function may be called several times with the same,
 *       data, we keep state regarding the result of previous invocations...
 *       That is the reason for the *search_history parameter!
 */
static int app_mbrtu_search_for_frame(unsigned char *frame_data, int frame_data_length, int *search_history) 
{
	int query_length, resp_length;
	unsigned char  function_code;
	/* *search_history flag will have or'ed of following values... */
#define SFF_HIST_NO_QUERY_FRAME     0x01
#define SFF_HIST_NO_RESPONSE_FRAME  0x02
#define SFF_HIST_NO_FRAME  (SFF_HIST_NO_RESPONSE_FRAME + SFF_HIST_NO_QUERY_FRAME)

#if MODBUS_DEBUG
	fprintf(stderr,"%s: frame_data_content 0x%02x  frame_data_length %d, search_hist %d\n",
			__func__, *frame_data,frame_data_length,*search_history );
#endif

	if ((*search_history == SFF_HIST_NO_FRAME) ||
			(frame_data_length < MIN_FRAME_LENGTH) ||
			(frame_data_length > MAX_RTU_FRAME_LENGTH))
		return -1;

	function_code = frame_data[APPLICATION_FRAME_FUNCTION_OFS];

	/* check for exception frame... */
	if ((function_code && 0x80) == 0x80) {
		if (frame_data_length >= EXCEPTION_FRAME_LENGTH + RTU_FRAME_CRC_LENGTH) {
			/* let's check CRC for valid frame. */
			if ( app_mbrtu_crc_calc_fast(frame_data, EXCEPTION_FRAME_LENGTH)  ==  app_mbrtu_crc_read(frame_data, EXCEPTION_FRAME_LENGTH) )
				return EXCEPTION_FRAME_LENGTH + RTU_FRAME_CRC_LENGTH;
			else
				/* We have checked the CRC, and it is not a valid frame! */
				*search_history |= SFF_HIST_NO_FRAME;
		}
		return -1;
	}

	/* check for valid function code */
	if ((function_code > MAX_FUNCTION_CODE) || (function_code < 1)) {
		/* This is an invalid frame!!! */
		*search_history |= SFF_HIST_NO_FRAME;
		return -1;
	}

	/* let's guess the frame length */
	query_length = resp_length = -1;
	if ((*search_history & SFF_HIST_NO_QUERY_FRAME) == 0)
		query_length = app_mbrtu_frame_length(frame_data, frame_data_length, query_frame_lengths);
	if ((*search_history & SFF_HIST_NO_RESPONSE_FRAME) == 0)
		resp_length  = app_mbrtu_frame_length(frame_data, frame_data_length, response_frame_lengths);

	/* let's check whether any of the lengths are valid...*/
	/* If any of the guesses coincides with the available data length
	 * we check that length first...
	 */
	if ((frame_data_length == query_length) || (frame_data_length == resp_length)) {
		if (   app_mbrtu_crc_calc_fast(frame_data, frame_data_length - RTU_FRAME_CRC_LENGTH)
				== app_mbrtu_crc_read(frame_data, frame_data_length - RTU_FRAME_CRC_LENGTH)){
#if MODBUS_DEBUG
			fprintf(stderr," returning frame_data_length %d\n",frame_data_length  );
#endif	
			return frame_data_length;
		}      
		/* nope, wrong guess...*/
		if (frame_data_length == query_length)
			*search_history |= SFF_HIST_NO_QUERY_FRAME;
		if (frame_data_length == resp_length)
			*search_history |= SFF_HIST_NO_RESPONSE_FRAME;
	}

	/* let's shoot for a query frame */
	if ((*search_history & SFF_HIST_NO_QUERY_FRAME) == 0) {
		if (query_length >= 0) {
			if (frame_data_length >= query_length) {
				/* let's check if we have a valid frame */
				if (   app_mbrtu_crc_calc_fast(frame_data, query_length - RTU_FRAME_CRC_LENGTH)
						== app_mbrtu_crc_read(frame_data, query_length - RTU_FRAME_CRC_LENGTH)){
#if MODBUS_DEBUG
					fprintf(stderr," returning query_length %d\n",query_length  );
#endif

					return query_length;
				}
				else
					/* We have checked the CRC, and it is not a valid frame! */
					*search_history |= SFF_HIST_NO_QUERY_FRAME;
			}
		}
	}

	/* let's shoot for a response frame */
	if ((*search_history & SFF_HIST_NO_RESPONSE_FRAME) == 0) {
		if (resp_length >= 0) {
			if (frame_data_length >= resp_length) {
				/* let's check if we have a valid frame */
				if (   app_mbrtu_crc_calc_fast(frame_data, resp_length - RTU_FRAME_CRC_LENGTH)
						== app_mbrtu_crc_read(frame_data, resp_length - RTU_FRAME_CRC_LENGTH)){
#if MODBUS_DEBUG
					fprintf(stderr," returning resp_length %d\n",resp_length  );
#endif	    
					return resp_length;
				}  
				else
					*search_history |= SFF_HIST_NO_RESPONSE_FRAME;
			}
		}
	}

	/* Could not find valid frame... */
	return -1;
}



/************************************/
/**                                **/
/**          Read a frame          **/
/**                                **/
/************************************/

/* A small auxiliary function, just to make the code easier to read... */
static inline void app_mbrtu_next_frame_offset(app_mbrtu_recv_buf_t *buf, unsigned char *slave_id)
{
	buf->frame_search_history = 0;
	app_mbrtu_lb_data_purge(&(buf->data_buf), 1 /* skip one byte */);

	if (slave_id == NULL)
		return;

	/* keep ignoring bytes, until we find one == *slave_id,
	 * or no more bytes...
	 */
	while (app_mbrtu_lb_data_count(&(buf->data_buf)) != 0) {
		if (*app_mbrtu_lb_data(&(buf->data_buf)) == *slave_id)
			return;
		app_mbrtu_lb_data_purge(&(buf->data_buf), 1 /* skip one byte */);
	}
}

/* A small auxiliary function, just to make the code easier to read... */
static inline int app_mbrtu_return_frame(app_mbrtu_recv_buf_t *buf, int frame_length, unsigned char **recv_data_ptr)
{
#ifdef MODBUS_DEBUG
	fprintf(stderr,"\nreturning valid frame of %d bytes.\n", frame_length);
#endif
	/* set the data pointer */
	*recv_data_ptr = app_mbrtu_lb_data(&(buf->data_buf));
	/* remove the frame bytes off the buffer */
	app_mbrtu_lb_data_purge(&(buf->data_buf), frame_length);
	/* reset the search_history flag */
	buf->frame_search_history = 0;
	/* if the buffer becomes empty, then reset boundary flag */
	if (app_mbrtu_lb_data_count(&(buf->data_buf)) <= 0)
		buf->found_frame_boundary = 0;
	/* return the frame length, excluding CRC */
	return frame_length - RTU_FRAME_CRC_LENGTH;
}

/* A function to read a valid frame off the rtu bus.
 *
 * NOTES:
 *        - The returned frame is guaranteed to be a valid frame.
 *        - The returned length does *not* include the CRC.
 *        - The returned frame is not guaranteed to have the same
 *          slave id as that stored in (*slave_id). This value is used
 *          merely in optimizing the search for wanted valid frames
 *          after reading an aborted frame. Only in this situation do
 *          we limit our search for frames with a slvae id == (*slave_id).
 *          Under normal circumstances, the value in (*slave_id) is
 *          simply ignored...
 *          If any valid frame is desired, then slave_id should be NULL.
 *
 */

/* NOTE: We cannot relly on the 3.5 character interval between frames to detect
 *       end of frame. We are reading the bytes from a user process, so in
 *       essence the bytes we are reading are coming off a cache.
 *       Any inter-character delays between the arrival of the bytes are
 *       lost as soon as they were placed in the cache.
 *
 *       Our only recourse is to analyse the frame we are reading in real-time,
 *       and check if it is a valid frame by checking it's CRC.
 *       To optimise this, we must be able to figure out the length
 *       of the frame currently being received by analysing the first bytes
 *       of that frame. Unfortunately, we have three problems with this:
 *         1) The spec does not specify the format of every possible modbus
 *            frame. For ex.functions 9, 10, 13, 14, 18 and 19(?).
 *         2) It is not possible to figure out whether a frame is a query
 *            or a response by just analysing the frame, and query and response
 *            frames have different sizes...
 *         3) A frame may be aborted in the middle! We have no easy way of telling
 *            if what we are reading is a partial (aborted) frame, followed by a
 *            correct frame.
 *       Possible solutions to:
 *         1) We could try to reverse engineer, but at the moment I have no
 *            PLCs that will generate the required frames.
 *            The chosen method is to verify the CRC if we are lucky enough to
 *            detect the 3.5 frame boundary imediately following one of these
 *            frames of unknown length.
 *            If we do not detect any frame boundary, then our only option
 *            is to consider it an aborted frame.
 *         2) We aim for the query frame (usually the shortest), and check
 *            it's CRC. If it matches, we accept, the frame, otherwise we try
 *            a response frame.
 *         3) The only way is to consider a frame boundary after each byte,
 *            (i.e. ignore one bye at a time) and verify if the following bytes
 *            constitue a valid frame (by checking the CRC).
 *
 *       When reading an aborted frame followed by two or more valid frames, if
 *       we are unlucky and do not detetect any frame boundary using the 3.5
 *       character interval, then we will most likely be reading in bytes
 *       beyond the first valid frame. This means we will have to store the extra
 *       bytes we have already read, so they may be handled the next time the
 *       read_frame() function is called.
 */
/*
 * NOTE: The modbus RTU spec is inconsistent on how to handle
 *       inter-character delays larger than 1.5 characters.
 *       - On one paragraph it is stated that any delay larger than
 *         1.5 character times aborts the current frame, and a new
 *         frame is started.
 *       - On another paragraph it is stated that a frame must begin
 *         with a silence of 3.5 character times.
 *
 * We will therefore consider that any delay larger than 1.5 character
 * times terminates a valid frame. All the above references to the 3.5 character
 * interval should therefore be read as a 1.5 character interval.
 */
/* NOTE: This function is only called from one place in the rest of the code,
 * so we might just as well make it inline...
 */
/* RETURNS: number of bytes in received frame
 *          -1 on read file error
 *          -2 on timeout
 */
static inline int app_mbrtu_read_frame(app_mbrtu_master_node_t *node, unsigned char **recv_data_ptr, struct timespec *end_time, unsigned char *slave_id)
{

	/* temporary variables... */
	fd_set rfds;
	struct timeval timeout;
	int res, read_stat;
	int frame_length;
	app_mbrtu_recv_buf_t *recv_buf = &node->recv_buf;

	int found_aborted_frame; /*Flag: 1 reading an aborted frame, start ignoring bytes ....*/

	/* assume error... */
	*recv_data_ptr = NULL;

	/*===================================*
	 * Check for frame in left over data *
	 *===================================*/
	/* If we have any data left over from previous call to read_frame()
	 * (i.e. this very same function), then we try to interpret that
	 * data, and do not wait for any extra bytes...
	 */
#if APP_MBRTU_WORKAROUND

	/* This printf is necessary as a workaround for a problem arisen in the communication with the Siemens 
	   modbus slave. We seem to be too quick for such slave so we have to slow down. Besides this since we are
	   working in half -duplex mode an hardware workaround has been added to eliminate the need of managing the
	   echo of the modbus request. */

#if MODBUS_DEBUG
	fprintf(stderr,"LETTURA 1\n");
#endif
	app_mbrtu_recv_buffer_print(recv_buf);
#endif


	frame_length = app_mbrtu_search_for_frame(app_mbrtu_lb_data(&recv_buf->data_buf), app_mbrtu_lb_data_count(&recv_buf->data_buf), &recv_buf->frame_search_history);
#if MODBUS_DEBUG
	fprintf(stderr,"we have any data left we try \n to interpret that data frame_length =%d\n",frame_length);
#endif
	if (frame_length > 0)  
		/* We found a valid frame! */
		return app_mbrtu_return_frame(recv_buf, frame_length, recv_data_ptr);


	/* If the left over data finished at a frame boundary, and since it
	 * doesn't contain any valid frame, we discard those bytes...
	 */
#if MODBUS_DEBUG
	fprintf(stderr,"data finished at a frame boundary with no valid frame\n resetting recv buffer =%d\n",recv_buf->found_frame_boundary);
#endif   
	if (recv_buf->found_frame_boundary == 1){
		app_mbrtu_recv_buf_reset(recv_buf);
#if MODBUS_DEBUG
		fprintf(stderr,"LETTURA 2\n");
		app_mbrtu_recv_buffer_print_short(recv_buf);
#endif    
	}

	/*============================*
	 * wait for data availability *
	 *============================*/
	/* if we can't find a valid frame in the existing data, or no data
	 * was left over, then we need to read more bytes!
	 */
#if MODBUS_DEBUG
	fprintf(stderr,"we read more bytes\n");
#endif 


	FD_ZERO(&rfds);
	FD_SET(node->fd, &rfds);
	{ int sel_res = app_mbrtu_select(node->fd + 1, &rfds, end_time);
		//int sel_res = app_mbrtu_select(node->fd + 1, &rfds, NULL);
#if MODBUS_DEBUG
		fprintf(stderr,"select result %d\n",sel_res );
#endif      
		if (sel_res < 0)
			return -1;
		if (sel_res == 0)
			return -2;

	}

	/*==============*
	 * read a frame *
	 *==============*/
	/* The main loop that reads one frame               */
	/*  (multiple calls to read() )                     */
	/* and jumps out as soon as it finds a valid frame. */

	found_aborted_frame = 0;
	FD_ZERO(&rfds);
	FD_SET(node->fd, &rfds);
#if MODBUS_DEBUG
	printf("%s:%d waiting a frame.\n", __FILE__, __LINE__ );
#endif      
	while (1) {

		/*------------------*
		 * read frame bytes *
		 *------------------*/
		/* Read in as many bytes as possible...
		 * But only if we have not found a frame boundary. Once we find
		 *  a frame boundary, we do not want to read in any more bytes
		 *  and mix them up with the current frame's bytes.
		 */
		if (recv_buf->found_frame_boundary == 0) {
#if MODBUS_DEBUG
			printf("%s:%d read a frame.\n", __FILE__, __LINE__ );
#endif      
			read_stat = read(node->fd, app_mbrtu_lb_free(&recv_buf->data_buf), app_mbrtu_lb_free_count(&recv_buf->data_buf));


#if MODBUS_DEBUG
			fprintf(stderr,"LETTURA 3\n");
			app_mbrtu_recv_buffer_print_short(recv_buf);
#endif      
			if (read_stat < 0) {
				if (errno != EINTR)
					return -1;
				else
					read_stat = 0;
			}
#ifdef MODBUS_DEBUG
			{/* display the hex code of each character received */
				int i;
				fprintf(stderr, "RX: ");
				for (i=0; i < read_stat; i++)
					fprintf(stderr, "<0x%2X>", *(app_mbrtu_lb_free(&recv_buf->data_buf) + i));
				fprintf(stderr, "\n");  
			}
#endif
			app_mbrtu_lb_data_add(&recv_buf->data_buf, read_stat);
#if MODBUS_DEBUG
			fprintf(stderr,"LETTURA 4\n");
			app_mbrtu_recv_buffer_print_short(recv_buf);
#endif
		}

		/*-----------------------*
		 * check for valid frame *
		 *-----------------------*/
		frame_length = app_mbrtu_search_for_frame(app_mbrtu_lb_data(&recv_buf->data_buf), app_mbrtu_lb_data_count(&recv_buf->data_buf), &recv_buf->frame_search_history);

		if (frame_length > 0)
			/* We found a valid frame! */
			return app_mbrtu_return_frame(recv_buf, frame_length, recv_data_ptr);

		/* if we reach this point, we are sure we do not have valid frame
		 * of known length in the current data with the current offset...
		 */

		/*---------------------------------*
		 * Have we found an aborted frame? *
		 *---------------------------------*/
		if (app_mbrtu_lb_data_count(&recv_buf->data_buf) >= MAX_RTU_FRAME_LENGTH){
			found_aborted_frame = 1;
#if MODBUS_DEBUG
			fprintf(stderr,"LETTURA 5\n");
			app_mbrtu_recv_buffer_print_short(recv_buf);
#endif      
		}

		/*---------------------------------*
		 * Must we try a new frame_offset? *
		 *---------------------------------*/
		if (found_aborted_frame == 1) {
			/* Note that the found_aborted_frame flag is only set if:
			 *   1 - we have previously detected a frame_boundary,
			 *       (i.e. found_frame_boundary is == 1 !!) so we won't be
			 *       reading in more bytes;
			 *   2 - we have read more bytes than the maximum frame length
			 *
			 * Considering we have just failed finding a valid frame, and the above
			 * points (1) and (2), then there is no way we are still going to
			 * find a valid frame in the current data.
			 * We must therefore try a new first byte for the frame...
			 */
			app_mbrtu_next_frame_offset(recv_buf, slave_id);
#if MODBUS_DEBUG
			fprintf(stderr,"LETTURA 6\n");
			app_mbrtu_recv_buffer_print_short(recv_buf);
#endif      
		}

		/*-----------------------------*
		 * check for data availability *
		 *-----------------------------*/
		if (recv_buf->found_frame_boundary == 0) {
			/* We need more bytes!! */
			/*
			 * if no character at the buffer, then we wait time_15_char
			 * before accepting end of frame
			 */
			/* NOTES:
			 *   - On Linux, timeout is modified by select() to reflect
			 *     the amount of time not slept; most other implementations do
			 *     not do this. On those platforms we will simply have to wait
			 *     longer than we wished if select() is by any chance interrupted
			 *     by a signal...
			 */

			timeout = node->time_15_char;
			while ((res = select(node->fd+1, &rfds, NULL, NULL, &timeout)) < 0) {
				if (errno != EINTR)
					return -1;
				/* We will be calling select() again.
				 * We need to reset the FD SET !
				 */
				FD_ZERO(&rfds);
				FD_SET(node->fd, &rfds);
			}

			if (res == 0) {
				int frame_length = app_mbrtu_lb_data_count(&recv_buf->data_buf);
				/* We have detected an end of frame using timing boundaries... */
				recv_buf->found_frame_boundary = 1; /* => stop trying to read any more bytes! */


#if MODBUS_DEBUG

				fprintf(stderr,"LETTURA 7\n");
				app_mbrtu_recv_buffer_print_short(recv_buf);
#endif	

				/* Let's check if we happen to have a correct frame... */
				if ((frame_length <= MAX_RTU_FRAME_LENGTH) &&
						(frame_length - RTU_FRAME_CRC_LENGTH > 0)) {
					if (   app_mbrtu_crc_calc_fast(app_mbrtu_lb_data(&recv_buf->data_buf), frame_length - RTU_FRAME_CRC_LENGTH)
							== app_mbrtu_crc_read(app_mbrtu_lb_data(&recv_buf->data_buf), frame_length - RTU_FRAME_CRC_LENGTH)) {
						/* We have found a valid frame. Let's get out of here! */
						return app_mbrtu_return_frame(recv_buf, frame_length, recv_data_ptr);
					}
				}

				/* We have detected a frame boundary, but the frame we read
				 * is not valid...
				 *
				 * One of the following reasons must be the cause:
				 *   1 - we are reading a single aborted frame.
				 *   2 - we are reading more than one frame. The first frame,
				 *       followed by any number of valid and/or aborted frames,
				 *       may be one of:
				 *       a - a valid frame whose length is unknown to us,
				 *           i.e. it is not specified in the public Modbus spec.
				 *       b - an aborted frame.
				 *
				 * Due to the complexity of reading 2a as a correct frame, we will
				 * consider it as an aborted frame. (NOTE: it is possible, but
				 * we will ignore it until the need arises... hopefully, never!)
				 *
				 * To put it succintly, what we now have is an 'aborted' frame
				 * followed by one or more aborted and/or valid frames. To get to
				 * any valid frames, and since we do not know where they begin,
				 * we will have to consider every byte as the possible begining
				 * of a valid frame. For this permutation, we ignore the first byte,
				 * and carry on from there...
				 */
				found_aborted_frame = 1;
				app_mbrtu_lb_data_purge(&recv_buf->data_buf, 1 /* skip one byte */);
				recv_buf->frame_search_history = 0;
#if MODBUS_DEBUG
				fprintf(stderr,"LETTURA 8\n");
				app_mbrtu_recv_buffer_print_short(recv_buf);
#endif	
			}
		}

		/*-------------------------------*
		 * check for data yet to process *
		 *-------------------------------*/
		if ((app_mbrtu_lb_data_count(&recv_buf->data_buf) < MIN_FRAME_LENGTH) &&
				(recv_buf->found_frame_boundary == 1)) {
			/* We have no more data to process, and will not read anymore! */
			app_mbrtu_recv_buf_reset(recv_buf);
#if MODBUS_DEBUG
			fprintf(stderr,"LETTURA 9\n");
			app_mbrtu_recv_buffer_print_short(recv_buf);
#endif      
#if MODBUS_DEBUG
			fprintf(stderr,"no more data to process, and will not read anymore\n" );
#endif       
			/* Return TIMEOUT error */
			return -2;
		}
	} /* while (1)*/

	/* humour the compiler... */
	return -1;
}





/************************************/
/**                                **/
/**    Read a Modbus RTU frame     **/
/**                                **/
/************************************/

/* The public function that reads a valid modbus frame.
 *
 * The returned frame is guaranteed to be different to the
 * the frame stored in send_data, and to start with the
 * same slave address stored in send_data[0].
 *
 * If send_data is NULL, send_data_length = 0, or
 * ignore_echo == 0, then the first valid frame read off
 * the bus is returned.
 *
 * return value: The length (in bytes) of the valid frame,
 *               -1 on error
 *               -2 on timeout
 */

static int app_mbrtu_read(app_mbrtu_master_node_t *node, unsigned char **recv_data_ptr, unsigned short int *transaction_id, const unsigned char *send_data, int send_length, const struct timespec *recv_timeout) 
{
	struct timespec cur_time, end_time, *ts_ptr;
	int res, recv_length, first;
	unsigned char *local_recv_data_ptr;
	unsigned char *slave_id, local_slave_id;

	/* Check input parameters... */
	if (node == NULL)
	{
		return -1;
	}

	if (recv_data_ptr == NULL)
		recv_data_ptr = &local_recv_data_ptr;

	if ((send_data == NULL) && (send_length != 0))
	{
		return -1;
	}

	/* check if nd is initialzed... */
	if (node->fd < 0)
	{
		return -1;
	}

	slave_id = NULL;
	if (send_length > APPLICATION_FRAME_SLAVEID_OFS) {
		local_slave_id = send_data[APPLICATION_FRAME_SLAVEID_OFS];
		slave_id = &local_slave_id;
	}

	/* We will potentially read many frames, and we cannot reset the timeout
	 * for every frame we read. We therefore determine the absolute time_out,
	 * and use this as a parameter for each call to read_frame() instead of
	 * using a relative timeout.
	 *
	 * NOTE: see also the timeout related comment in the read_frame()= function!
	 */
	/* get the current time... */
	if (recv_timeout == NULL) {
		ts_ptr = NULL;
	} else {
		ts_ptr = &end_time;
		if ((recv_timeout->tv_sec == 0) && (recv_timeout->tv_nsec == 0)) {
			end_time = *recv_timeout;
		} else {
			if (clock_gettime(CLOCK_REALTIME, &cur_time) < 0)
				return -1;
			end_time = app_mbrtu_timespec_add(cur_time, *recv_timeout);
		}
	}

	/* NOTE: When using a half-duplex RS-485 bus, some (most ?) RS232-485
	 *       converters will send back to the RS232 port whatever we write,
	 *       so we will read in whatever we write out onto the bus.
	 *       We will therefore have to compare
	 *       the first frame we read with the one we sent. If they are
	 *       identical it is because we are in fact working on a RS-485
	 *       bus and must therefore read in a second frame which will be
	 *       the true response to our query.
	 *       If the first frame we receive is different to the query we
	 *       just sent, then we are *not* working on a RS-485 bus, and
	 *       that is already the real response to our query.
	 *
	 *       Flushing the input cache immediately after sending the query
	 *       could solve this issue, but we have no guarantee that this
	 *       process would not get swapped out between the write() and
	 *       flush() calls, and we could therefore be flushing the response
	 *       frame!
	 */

	first = -1;
	while ((res = recv_length = app_mbrtu_read_frame(node, recv_data_ptr, ts_ptr, slave_id)) >= 0) {

		if (first == -1 ) 
		{
			first = 1;
#if MODBUS_DEBUG
			fprintf(stderr, "first %d\n",first );
#endif    
		}
		else 
		{
			first = 0;
		}
		if ((send_length <= 0) || (node->ignore_echo == 0))
		{
#if MODBUS_DEBUG
			fprintf(stderr, "valid frame.\n" );
#endif    
			/* any valid frame will do... */
			return recv_length;
		}
#if MODBUS_DEBUG
		else if (node->ignore_echo)
		{
			fprintf(stderr, "echo: ignored.\n" );
		}
#endif    

		if ((send_length > APPLICATION_FRAME_SLAVEID_OFS + 1) && (first == 1))
		{
			/* We have a frame in send_data,
			 * so we must make sure we are not reading in the frame just sent...
			 *
			 * We must only do this for the first frame we read. Subsequent
			 * frames are guaranteed not to be the previously sent frame
			 * since the modbus_rtu_write() resets the recv buffer.
			 * Remember too that valid modbus responses may be exactly the same
			 * as the request frame!!
			 */
			if (recv_length == send_length)
			{
				if (memcmp(*recv_data_ptr, send_data, recv_length) == 0){
					/* recv == send !!! */
					/* read in another frame.  */
#if MODBUS_DEBUG
					fprintf(stderr, "recv == send !!! ....read in another frame.\n");
#endif	  
					continue;
				}  
			}
		}

		/* The frame read is either:
		 *  - different to the frame in send_data
		 *  - or there is only the slave id in send_data[0]
		 *  - or both of the above...
		 */
		if (send_length > APPLICATION_FRAME_SLAVEID_OFS)
		{
			if (recv_length > APPLICATION_FRAME_SLAVEID_OFS)
			{
				/* check that frame is from/to the correct slave... */
				if ((*recv_data_ptr)[APPLICATION_FRAME_SLAVEID_OFS] == send_data[APPLICATION_FRAME_SLAVEID_OFS]){
					/* yep, it is... */
#if MODBUS_DEBUG
					fprintf(stderr,"yep, it is... recv_length = %d \n", recv_length);
#endif	  
					return recv_length;
				}	  
			}	  
		}	  

		/* The frame we have received is not acceptable...
		 * Let's read a new frame.
		 */
	} /* while(...) */

	/* error reading response! */
	/* Return the error returned by read_frame! */
#if MODBUS_DEBUG
	fprintf(stderr, "error reading response!.. returning read_frame error  = %d \n", res);
#endif  
	return res;
}



/***********************************************/
/**                                           **/
/**    Modbus Transaction Function            **/
/**                                           **/
/***********************************************/


/* A Master/Slave transaction... */
static int app_mbrtu_transaction(unsigned char  *packet, int query_length, unsigned char  **data, app_mbrtu_master_node_t *node, int send_retries, unsigned char *error_code, const struct timespec *response_timeout)
{

	int error = INTERNAL_ERROR;
	int response_length = INTERNAL_ERROR;
	unsigned short int send_transaction_id, recv_transaction_id;

#if MODBUS_DEBUG
	printf("[%s] - send_retries %d\n", __func__, send_retries);
#endif  
	for (send_retries++; send_retries > 0; send_retries--) {

		/* We must also initialize the recv_transaction_id with the same value,
		 * since some layer 1 protocols do not support transaction id's, so
		 * simply return the recv_transaction_id variable without any changes...
		 */
		send_transaction_id = recv_transaction_id = app_mbrtu_next_transaction_id();

#if MODBUS_DEBUG
		printf("[%s] - function %d\n", __func__, packet[1]);
#endif  

		if (app_mbrtu_write(node, packet, query_length, send_transaction_id) < 0)
		{
#if MODBUS_DEBUG
			printf("[%s] - PORT_FAILURE\n", __func__);
#endif  
			error = PORT_FAILURE;
			continue;
		}

		response_length = app_mbrtu_read(node, data, &recv_transaction_id, packet, query_length, response_timeout);

		if(response_length == -2)
		{
//#if MODBUS_DEBUG
			printf("[%s] - response_length %d TIMEOUT\n", __func__, response_length);
//#endif  
			return TIMEOUT;
		}
		if(response_length < 0)
		{
//#if MODBUS_DEBUG
			printf("[%s] - PORT_FAILURE2\n", __func__);
//#endif  
			error = PORT_FAILURE;
			continue;
		}
		if( response_length < 3 )
		{
			/* This should never occur! Modbus_read() should only return valid frames! */
//#if MODBUS_DEBUG
			printf("[%s] - response_length %d INTERNAL_ERROR\n", __func__, response_length);
//#endif  
			return INTERNAL_ERROR;
		}

		/* first check whether we have correct transaction id */
		if (send_transaction_id != recv_transaction_id)
		{
//#if MODBUS_DEBUG
			printf("[%s] - INVALID_FRAME\n", __func__);
//#endif  
			error = INVALID_FRAME;
			continue;
		}

		/* NOTE: no need to check whether (*data)[0] = slave!              */
		/*       This has already been done by the modbus_read() function! */

		/* Check whether the response frame is a response to _our_ query */
		if (((*data)[1] & ~0x80) != packet[1])
		{
//#if MODBUS_DEBUG
			printf("[%s] - INVALID_FRAME2\n", __func__);
//#endif  
			error = INVALID_FRAME;
			continue;
		}

		/* Now check whether we received a Modbus Exception frame */
		if (((*data)[1] & 0x80) != 0) {
			/* we have an exception frame! */
			if (error_code != NULL)
				/* NOTE: we have already checked above that data[2] exists! */
				*error_code = (*data)[2];
//#if MODBUS_DEBUG
			printf("[%s] - error_code %x MODBUS_ERROR\n", __func__, error_code);
//#endif  
			return MODBUS_ERROR;
		}

		/* everything seems to be OK. Let's get out of the send retry loop... */
		/* success! */
#if MODBUS_DEBUG
		printf("[%s] - response_length %d DONE\n", __func__, response_length);
#endif  
		return response_length;
	}

	/* reached the end of the retries... */
//#if MODBUS_DEBUG
	printf("[%s] - error %d BUS_ERROR\n", __func__, error);
//#endif  
	return error;
}






/*****************************************************/
/*****************************************************/
/*****************************************************/
/*						     */
/*	Application Level Functions		     */
/*						     */
/*****************************************************/
/*****************************************************/
/*****************************************************/


/* Local Defines -- application dependant*/
#define MAX_READ_REGS 16
#define MAX_INPUT_REGS 16
#define HOLDING_REGISTER_BASE 40000 
#define INPUT_REGISTER_BASE 30000 
#define INPUT_STATUS_BASE 10000 
#define MAX_WRITE_COILS 256
#define MAX_WRITE_REGS 100

/**********************************************************************

  thread attribute  set up function


 ***********************************************************************/

static void app_mbrtu_thread_attr_setup(void)
{



	pthread_attr_init(&app_mbrtu_attr);
	pthread_attr_setdetachstate(&app_mbrtu_attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setschedpolicy(&app_mbrtu_attr, SCHED_FIFO);
	app_mbrtu_sched_param.sched_priority = 90; /* to be verified........................................*/
	pthread_attr_setinheritsched(&app_mbrtu_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedparam(&app_mbrtu_attr, &app_mbrtu_sched_param);
	pthread_attr_setscope(&app_mbrtu_attr, PTHREAD_SCOPE_SYSTEM);

}


/***********************************************************************

  The following functions construct the required query into
  a modbus query packet.

 ***********************************************************************/

static inline int build_query_packet(unsigned char  slave, unsigned char  function, unsigned short int start_addr, unsigned short int count, unsigned char *packet)
{
	packet[ 0 ] = slave;
	packet[ 1 ] = function;
	/* NOTE:
	 *  Modbus uses high level addressing starting off from 1, but
	 *  this is sent as 0 on the wire!
	 */
#ifdef INT2BYTE
	int2byte(&packet[2], app_mbrtu_hton(start_addr - 1));
	int2byte(&packet[4], app_mbrtu_hton(count));
#else
	u16_v(packet[2]) = app_mbrtu_hton(start_addr - 1); 
	u16_v(packet[4]) = app_mbrtu_hton(count); 
#endif
	return 6;
}

/**
 *
 *	Modbus function 08 provides a series of tests for checking 
 *	the communication system between the master and slave, or for
 *	checking various internal error conditions within the slave. 
 *	Broadcast is not supported.
 *	
 *	Called by fn_08_diagnostics
 *
 */

static void * diagnostics( void *param )
{
	unsigned char *packet = query_buffer;
	unsigned char *data;
	int response_length, query_length;
	int temp, i = 0;
	unsigned char byte_count = 0;
	int j;

	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	/*Build query*/
	packet[ 0 ] = p->slave;
	packet[ 1 ] = p->function;
#ifdef INT2BYTE
	int2byte(&packet[2], app_mbrtu_hton(p->subfunction));
	int2byte(&packet[4], app_mbrtu_hton(p->value));
#else
	u16_v(packet[2]) = app_mbrtu_hton(p->subfunction);
	u16_v(packet[4]) = app_mbrtu_hton(p->value); /*contains the data for the query*/
#endif
	query_length = 6;

	response_length = app_mbrtu_transaction(packet, query_length, &data, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);

	if (response_length < 0){
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	if (response_length < 2){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	} 

	/*check the answer*/
	if ( p->subfunction != 0x15 ){
		if (packet[2] != data[2] || packet[3] != data[3] ){
			app_mbrtu_status[1] = INVALID_FRAME;
			app_mbrtu_status[0] = 0;
			return NULL;	   
		}
		/*retrieve da data hi and lo byte of the answer*/
		temp = data[ 4 ] << 8;
		/* OR with lo_byte           */
		temp = temp | data[ 5 ];
		p->dest[0] = temp;              
	}
	else {
		if (data[5] == 0x03) {
			byte_count = data[ 6 ] << 8;
			byte_count = byte_count | data[ 7 ];
			p->dest[0] = byte_count/2;
			j = 1;
			for(i = 0; i < byte_count/2 ; i++ ) { 
				/* shift reg hi_byte to temp */
				temp = data[ 8 + i *2 ] << 8;
				/* OR with lo_byte           */
				temp = temp | data[ 9 + i * 2 ];
				p->dest[j] = temp;
				j++;
			}

		}
		if (data[5] == 0x04 && p->value != 0x04) {
			app_mbrtu_status[1] = INVALID_FRAME;
			app_mbrtu_status[0] = 0;
			return NULL;	   	   	
		}

	}

	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
	return NULL;   	       
}


/**
 *	Read FIFO queue
 *	Reads the contents of a First In First Out (FIFO)
 *	queue of 4XXXX registers. The function returns a count of the 
 *	registers in the queue, followed by the queued data.
 *	Up to 32 registers can be read: the count, plus up to 31 queued 
 *	data registers. The queue count register is returned first, 
 *	followed by the queued data registers.
 *	The function reads the queue contents, but does not clear them. 
 *	Broadcast is not supported.	
 *	
 */
static void * read_fifo_queue( void *param )
{
	unsigned char *packet = query_buffer;
	unsigned char *data;
	int response_length, query_length;
	int temp, i = 0;
	unsigned char byte_count = 0;
	int dest_pos = 0;

	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	/*Build query*/
	packet[ 0 ] = p->slave;
	packet[ 1 ] = p->function;
#ifdef INT2BYTE
	int2byte(&packet[2], app_mbrtu_hton(p->start_addr - 1));
#else
	u16_v(packet[2]) = app_mbrtu_hton(p->start_addr - 1); 
#endif
	query_length = 4;


#ifdef MODBUS_DEBUG
	printf("[%s]: calling app_mbrtu_transaction \n", __func__);
#endif
	response_length = app_mbrtu_transaction(packet, query_length, &data, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);


	if (response_length < 0){
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;
#ifdef MODBUS_DEBUG
		printf("[%s]: app_mbrtu_transaction problem1 %d\n", __func__, response_length);
#endif
		return NULL;
	}  

	if (response_length < 2){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
#ifdef MODBUS_DEBUG
		printf("[%s]: app_mbrtu_transaction problem2 %d\n", __func__, response_length);
#endif
		return NULL;
	} 

	/* check the answer*/
	/* we want data bye MSB first*/
	byte_count = data[ 2 ] << 8;
	byte_count = byte_count | data[ 3 ];
	p->dest[dest_pos] = byte_count/2;
	for( i = 0; i < (byte_count/2); i++ ) { 
		dest_pos++;
		/* shift reg hi_byte to temp */
		temp = data[ 4 + i * 2 ] << 8;
		/* OR with lo_byte           */
		temp = temp | data[ 5 + i * 2 ];
		p->dest[dest_pos] = temp;
	}

	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
#ifdef MODBUS_DEBUG
	printf("[%s]: app_mbrtu_transaction DONE %d\n", __func__, response_length);
#endif
	return NULL;   	

}


/**
 *	Read/Write 4X Registers
 *	Performs a combination of one read and one write operation in 
 *	a single Modbus transaction. The function can write new contents 
 *	to a group of 4XXXX registers, and then return the contents of 
 *	another group of 4XXXX registers. Broadcast is not supported.
 *
 */
static void * read_write_registers( void *param )
{
	unsigned char *packet = query_long_buffer;
	unsigned short *data;
	unsigned char *rdata;
	int response_length, query_length;
	int temp, i = 0;
	int dest_pos = 0;


	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	data = p->source; /* get the data for the query from MODBUSdata buffer */

	/*Build query*/
	packet[ 0 ] = p->slave;
	packet[ 1 ] = p->function;
#ifdef INT2BYTE
	int2byte(&packet[2], app_mbrtu_hton(p->start_addr - 1));
	int2byte(&packet[4], app_mbrtu_hton(p->count));
	int2byte(&packet[6], app_mbrtu_hton(p->reg_addr));
	int2byte(&packet[8], app_mbrtu_hton(p->reg_count));
#else
	u16_v(packet[2]) = app_mbrtu_hton(p->start_addr - 1); 
	u16_v(packet[4]) = app_mbrtu_hton(p->count); 
	u16_v(packet[6]) = app_mbrtu_hton(p->reg_addr);        
	u16_v(packet[8]) = app_mbrtu_hton(p->reg_count);  
#endif
	packet[10] = ( unsigned char )((p->reg_count)*2);

	query_length = 10;

	query_length++;

	for (i = 0; i < p->reg_count; i++){  
#ifdef INT2BYTE
		int2byte(&packet[query_length], app_mbrtu_hton(data[i]));
#else
		u16_v(packet[query_length]) = app_mbrtu_hton(data[i]); 
#endif
		query_length = query_length + 2;	  
	}  

	response_length = app_mbrtu_transaction(packet, query_length, &rdata, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);

	if (response_length < 0){
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	if (response_length < 2){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	} 

	p->dest[dest_pos] = p->count;
	if ( p->byteorder == 0 ){
		dest_pos++;
		/* we want data bye MSB first*/
		for(i = 0; i < (rdata[2]/2); i++ ) {
			/* shift reg hi_byte to temp */
			temp = rdata[ 3 + i *2 ] << 8;
			/* OR with lo_byte           */
			temp = temp | rdata[ 4 + i * 2 ];
			p->dest[dest_pos++] = temp;
		}
	} 
	else { /*we want to perform inversion*/
		/* we want data bye LSB first*/
		dest_pos++;
		for(i = 0; i < (rdata[2]/2); i++ ) {
			/* shift reg hi_byte to temp */
			temp = rdata[ 4 + i * 2 ] << 8;
			/* OR with lo_byte           */
			temp = temp | rdata[ 3 + i *2 ];
			p->dest[dest_pos++] = temp;
		}	
	}


	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
	return NULL;        
}


/**
 *	
 *	Mask write 4X register
 *	Modifies the contents of a specified 4XXXX register using a 
 *	combination of an AND mask, an OR mask, and the registers current
 *	contents. The function can be used to set or clear individual 
 *	bits in the register. Broadcast is not supported.
 *	
 *	called by fn_16_mask_write_registers
 *	
 */
static void * mask_write_register( void *param )
{
	unsigned char *packet = query_long_buffer;
	unsigned short *data;
	unsigned char *rdata;
	int response_length, query_length;
	int i = 0;

	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	data = p->source; /* get the data for the query from MODBUSdata buffer */

	/*Build query*/
	packet[ 0 ] = p->slave;
	packet[ 1 ] = p->function;
#ifdef INT2BYTE
	int2byte(&packet[2], app_mbrtu_hton(data[p->start_addr - 1]));
	int2byte(&packet[4], app_mbrtu_hton(data[0]));
	int2byte(&packet[6], app_mbrtu_hton(data[1]));
#else
	u16_v(packet[2]) = app_mbrtu_hton(p->start_addr - 1); 
	u16_v(packet[4]) = app_mbrtu_hton(data[0]); 
	u16_v(packet[6]) = app_mbrtu_hton(data[1]); 
#endif
	query_length = 8;

	response_length = app_mbrtu_transaction(packet, query_length, &rdata, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);


	if (response_length < 0){
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	if (response_length < 2){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	} 

	/*Check that the answer is an echo of the request */
	for( i = 0; i < query_length; i++ ) 		
		if ( packet[i] != rdata[i]){
			app_mbrtu_status[1] = INVALID_FRAME;
			app_mbrtu_status[0] = 0;
			return NULL;		  
		}

	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
	return NULL;			                     	
}


/**********************************************************************
  Write General Reference

  Writes the contents of registers in Extended Memory file 
  (6XXXXX) references.
  Broadcast is not supported.
  The function can write multiple groups of references. 
  The groups can be separate (noncontiguous), but the references 
  within each group must be sequential.

NOTE: to work properly this function requires that the information
about data to be written are packeted as follow in the MODBUSdata
array:

2 byte for the reference type fixed to 0x0006
2 byte for Extended Memory file number (1 to 10, hex 0001 to 000A);

2 byte starting register address within the file
2 byte quantity of registers to be written
2 byte  per register for the data to be written.

2 byte --> unsigned short int, hence 1 location in the array	

called by fn_15_write_general_reference

 ***********************************************************************/

static void * write_general_ref( void *param )
{
	unsigned char *packet = query_long_buffer;
	unsigned short *data;
	unsigned char *rdata;
	int response_length, query_length;
	int j, i = 0;
	int flag = 0;
	int counter = 0;
	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	data = p->source; /*get the data for the query from MODBUSdata buffer*/

	packet[0] = p->slave;
	packet[1] = p->function;
	packet[2] = (p->count)*2;

	/*Check request length to be greter than the minimum allowed for this function and shorter than the max allowed modbus frame*/
	if ( packet[2] < 9 || packet[2] > 250 ){
		app_mbrtu_status[1] = -5; /*invalid request*/
		app_mbrtu_status[0] = 0;
		return NULL;
	}
	else {
		j = 3;
		for (i = 0; i < (p->count) ; i++){

			switch(flag){
				case 0:	/*here we handle reference type packet*/
					packet[j] = data[i];
					flag = 1; 
					packet[2]--;
					j++;
					break;
				case 1: /*here we handle file number, starting addr and register count */
					counter ++;
					if ( counter < 2 ){
#ifdef INT2BYTE
						int2byte(&packet[j], app_mbrtu_hton(data[i]));
#else
						u16_v(packet[ j ]) = app_mbrtu_hton(data [i]);
#endif
						j = j+2;					   	
					}
					else {
#ifdef INT2BYTE
						int2byte(&packet[j], app_mbrtu_hton(data[i]));
#else
						u16_v(packet[ j ]) = app_mbrtu_hton(data [i]);
#endif
						j = j+2;
						counter = data[i];
						flag = 2;
					}
					break;
				case 2: /*here we handle data*/
					counter --;
					if ( counter > 0 ){
#ifdef INT2BYTE
						int2byte(&packet[j], app_mbrtu_hton(data[i]));
#else
						u16_v(packet[ j ]) = app_mbrtu_hton(data [i]);
#endif
						j = j+2;					   	
					}
					else {
#ifdef INT2BYTE
						int2byte(&packet[j], app_mbrtu_hton(data[i]));
#else
						u16_v(packet[ j ]) = app_mbrtu_hton(data [i]);
#endif
						j = j+2;
						flag = 0;
					}  				
					break;
				default:
					app_mbrtu_status[1] = -5; /*invalid request*/
					app_mbrtu_status[0] = 0;
					return NULL;

			}
		}     

		query_length = 3 + packet[2];
		response_length = app_mbrtu_transaction(packet, query_length, &rdata, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);

		if (response_length < 0){
			app_mbrtu_status[1] = response_length;
			app_mbrtu_status[0] = 0;
			return NULL;
		}  

		if (response_length < 2){
			app_mbrtu_status[1] = INVALID_FRAME;
			app_mbrtu_status[0] = 0;
			return NULL;
		} 

		/*Check that the answer is an echo of the request */
		for( i = 0; i < query_length; i++ ) 		
			if ( packet[i] != rdata[i]){
				app_mbrtu_status[1] = INVALID_FRAME;
				app_mbrtu_status[0] = 0;
				return NULL;		  
			}

		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;	
		return NULL;			 	
	}

}





/**********************************************************************
  Read General Reference
  Returns the contents of registers in Extended Memory 
  file (6XXXXX) references.
  Broadcast is not supported
NOTE: to work properly this function requires that the information
about the register to be read are packeted as follow in the MODBUSdata
array:
2 byte for Extended Memory file number (1 to 10, hex 0001 to 000A);

2 byte starting register address within the file
2 byte quantity of registers to be read

2 byte --> unsigned short int, hence 1 location in the array	

called by fn_14_read_general_reference

 ***********************************************************************/
static void * read_general_ref( void *param )
{
	unsigned char *packet = query_long_buffer;
	unsigned short *data;
	unsigned char *rdata;
	int response_length, query_length;
	int temp, j, i = 0;
	int total_bc;
	int group_bc;
	int dest_pos = 0;
	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	data = p->source; /*get the data for the query from MODBUSdata buffer*/

	packet[0] = p->slave;
	packet[1] = p->function;
	packet[2] = p->count * 7;

	if ( p->count < 1 ){
		app_mbrtu_status[1] = -5; /*invalid request*/
		app_mbrtu_status[0] = 0;
		return NULL;
	}
	else {
		for (i = 0; i < p->count; i++){

			packet[ 3 + i*7 ] = 0x06;
#ifdef INT2BYTE
			int2byte(& packet[ 4 + i*7 ], app_mbrtu_hton(data [0 + i*3]));
			int2byte(& packet[ 6 + i*7 ], app_mbrtu_hton(data [1 + i*3]));
			int2byte(& packet[ 8 + i*7 ], app_mbrtu_hton(data [2 + i*3]));
#else
			u16_v(packet[ 4 + i*7 ]) = app_mbrtu_hton(data [0 + i*3]);
			u16_v(packet[ 6 + i*7 ]) = app_mbrtu_hton(data [1 + i*3]);
			u16_v(packet[ 8 + i*7 ]) = app_mbrtu_hton(data [2 + i*3]);
#endif
		}

		query_length = 3 + p->count;
		response_length = app_mbrtu_transaction(packet, query_length, &rdata, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);

		if (response_length < 0){
			app_mbrtu_status[1] = response_length;
			app_mbrtu_status[0] = 0;
			return NULL;
		}  

		if (response_length < 2){
			app_mbrtu_status[1] = INVALID_FRAME;
			app_mbrtu_status[0] = 0;
			return NULL;
		} 

		/*Gather the answer: the received data are stored in the MODBUSdata array:
		  in MODBUSdata[0] we have the total number of entries, the other data are oaganized as follows:
		  MODBUSdata[1] = # of data N for group reference 1
		  MODBUSdata[2] = data0 for group reference 1
		  MODBUSdata[3] = data1 for group reference 1
		  .....
		  MODBUSdata[K] = dataN for group reference 1
		  MODBUSdata[K+1] = # of data M for group reference 2
		  MODBUSdata[K+2] = data0 for group reference 2
		  .....
		  MODBUSdata[H] = dataM for group reference 2

		 */
		total_bc = rdata[2];
		i = 3;
		dest_pos = 1;
		while (total_bc > 0){
			group_bc = rdata[i];
			i+=2;
			p->dest[dest_pos++] = (group_bc - 1)/2;
			/*in MODBUSdata[0] we have the total number of entries in the result array*/
			p->dest[0] += (group_bc - 1)/2 + 1;
			for(j = 0; j < (group_bc - 1)/2; j++){
				temp = rdata[ i ] << 8;
				/* OR with lo_byte           */
				temp = temp | rdata[i + 1 ];
				p->dest[dest_pos++] = temp;
				i+=2;			
			}
			total_bc -= (group_bc + 1);
		}


		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;	
		return NULL;			 	
	}

}

/**
 *	Report Slave ID - called by function fn_11
 *	Output is controller type dependand so the 
 *	function just propagates the received byte without
 *	any guess about the format.
 *
 */
static void * read_slave_id( void *param )
{
	unsigned char *packet = query_buffer;
	unsigned char *data;
	int response_length, query_length;
	int i = 0;

	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	packet[0] = p->slave;
	packet[1] = p->function;
	query_length = 2;

	response_length = app_mbrtu_transaction(packet, query_length, &data, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);

	if (response_length < 0){
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	if (response_length < 2){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  
	/*save byte count in MODBUSdata[0]*/
	p->dest[i] = data[2];
	/*in the following MODBUSdata position put the received byte*/
	for( i = 1; i <= data[2]; i++ ) 		
		p->dest[i] = data[ 2 + i ];

	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
	return NULL;	
}




/**
 *	
 *	Presets values into a sequence of holding registers 
 *	(4X references). When	broadcast, the function presets the same 
 *	register references in all attached slaves
 *	Copy the values in an array to an array on the slave.
 * 	Called by function fn_10_preset_multiple_registers()
 *
 *	Function DOES support Broadcast.
 *	
 */
static void * write_multiple_register( void *param )
{
	unsigned char  byte_count;
	int i, query_length, response_length;
	unsigned char  *packet = query_long_buffer;
	unsigned char  *rdata; /*received buffer data points to the linear buffer*/
	unsigned short *data; /* data to be written in the register points to the MODBUSdata array*/

	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	data = p->source;

	if( p->count > MAX_WRITE_REGS ) {
		app_mbrtu_status[1] = -5;
		app_mbrtu_status[0] = 0;
		return NULL;
	}

	query_length = build_query_packet(p->slave, p->function,  p->reg_addr, p->reg_count, packet);

	if (query_length < 0){
		app_mbrtu_status[1] = INTERNAL_ERROR;
		app_mbrtu_status[0] = 0;
		return NULL;
	}

	byte_count = p->reg_count*2;
	packet[query_length] = byte_count;

	for( i = 0; i < p->reg_count; i++ ) {
		packet[++query_length] = data[i] >> 8; /* get HI byte */
		packet[++query_length] = data[i] & 0x00FF; /* get LO byte */
	}

	response_length = app_mbrtu_transaction(packet, ++query_length, &rdata, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);


	/*Manage broadcast: we do not have answer so we check that we exited from the transaction with a time out error*/
	if (p->slave == 0){
		if ( response_length == -2 ){
			app_mbrtu_status[1] = 1;
			app_mbrtu_status[0] = 0;	
			return NULL;		
		}
		else {	
			app_mbrtu_status[1] = response_length;
			app_mbrtu_status[0] = 0;	
			return NULL;
		}
	}

	if (response_length < 0) {
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	if (response_length != 6){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	} 

	if ((rdata[2] != packet[2]) || (rdata[3] != packet[3]) || (rdata[4] != packet[4]) || (rdata[5] != packet[5])){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
	return NULL;
}





/**
 *	write_multiple_coil
 *
 *	Forces each coil (0X reference) in a sequence of coils to either
 *	ON or OFF. When broadcast, the function forces the same coil 
 *	references in all attached slaves.
 *	Useb by function fn_0f_force_multiple_coil() 
 *
 *	Coil Status is retrieved from the Modbusdata array where
 *	for each element we store a coil status so as if MODBUSdata[i] = 1 coil status is ON 
 * 	if  MODBUSdata[i] = 0 is OFF starting from the lower and going to the higher coil requested
 * 	Function DOES support broadcast.
 *
 */

static void * write_multiple_coil( void *param )
{
	int byte_count, i;
	unsigned char  bit;
	int coil_check = 0;
	int data_array_pos = 0;
	int query_length, response_length;
	unsigned char  *packet = query_long_buffer;
	unsigned short  *data; /* data buffer with value for coils. It points to the Modbusdata array where
				  for each element we store a coil status so as if MODBUSdata[i] = 1 coil status is ON 
				  if  MODBUSdata[i] = 0 is OFF */

	unsigned char  *rdata; /*received data point to the linear buffer defined inside the master data structure*/

	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	data = p->source;

	if( p->count > MAX_WRITE_COILS ) {
		app_mbrtu_status[1] = -5;
		app_mbrtu_status[0] = 0;
		return NULL;
	}

	query_length = build_query_packet(p->slave, p->function, p->start_addr, p->count, packet);

	if (query_length < 0){
		app_mbrtu_status[1] = INTERNAL_ERROR;
		app_mbrtu_status[0] = 0;
		return NULL;
	}
	/* NOTE: Integer division. This is equivalent of determining the ceil(count/8) */
	byte_count = (p->count+7)/8;
#ifdef MODBUS_DEBUG
	printf("[%s] - byte_count = %d\n", __func__, byte_count);
#endif
#ifdef VALMAR_OMRON_MX2_WORKAROUND
	/* byte count must be odd */
	if (byte_count%2!= 0 && p->start_addr%2 != 0)
	{
		byte_count++;
#ifdef MODBUS_DEBUG
		printf("VALMAR FIX [%s] - byte_count = %d start_addr = %d\n", __func__, byte_count, p->start_addr);
#endif
	}
#endif
	packet[query_length] = byte_count;

	bit = 0x01;

	for( i = 0; i < byte_count; i++) {
		packet[++query_length] = 0;
		while( bit & 0xFF && coil_check++ < p->count ) {
			if( data[ data_array_pos++ ] ) {
				packet[ query_length ] |= bit;
			} else {
				packet[ query_length ] &=~ bit;
			}
			bit <<= 1;
		}
		bit = 0x01;
	}

	response_length = app_mbrtu_transaction(packet, ++query_length, &rdata, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);

	/*Manage broadcast: we do not have answer so we check that we exited from the transaction with a time out error*/
	if (p->slave == 0){
		if ( response_length == -2 ){
			app_mbrtu_status[1] = 1;
			app_mbrtu_status[0] = 0;	
			return NULL;		
		}
		else {	
			app_mbrtu_status[1] = response_length;
			app_mbrtu_status[0] = 0;	
			return NULL;
		}
	}

	if (response_length < 0) {
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	if (response_length != 6){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	if ((rdata[2] != packet[2]) || (rdata[3] != packet[3]) || (rdata[4] != packet[4]) || (rdata[5] != packet[5])){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
	return NULL;
}


/**
 *
 *	read_event_log
 *
 *	Returns a status word, event count, message count, and a field of 
 *	event bytes from the slave. Broadcast is not supported.
 *
 *	Used by fn_0c_read_event_log 
 *
 *	Parameters:
 *	unsigned char function 		- function code 0x0c 
 *	unsigned char slave    		- slave address
 *	
 */
static void * read_event_log( void *param )
{
	unsigned char *packet = query_buffer;
	unsigned char *data;
	int response_length, query_length;
	int temp, i, dest_pos = 0;

	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	packet[0] = p->slave;
	packet[1] = p->function;
	query_length = 2;

	response_length = app_mbrtu_transaction(packet, query_length, &data, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);

	if (response_length < 0){
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	/*The normal response contains a two byte status word field, a two byte event
	  count field, a two byte message count field, and a field containing 0-64 bytes of
	  events. A byte count field defines the total length of the data in these four fields.*/
	if (response_length < 9){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	p->dest[dest_pos] = data[2] - 6 ; /*Number of the event bytes*/
	dest_pos++;
	for(i = 0; i < 3; i++ ) {
		/* shift reg hi_byte to temp */
		temp = data[ 3 + i *2 ] << 8;
		/* OR with lo_byte           */
		temp = temp | data[ 4 + i * 2 ];
		p->dest[dest_pos] = temp;
		dest_pos++;
	}

	for(i = 0; i < (data[2] - 6) ; i++ ) {
		p->dest[dest_pos] = data[ 9 + i];
		dest_pos++;
	}

	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
	return NULL;	
}



/**
 *
 *	read_event_counter
 *
 *	Returns a status word and an event count from the slave's 
 *	communications event counter. By fetching the current count before 
 *	and after a series of messages, a master can determine whether the 
 *	messages were handled normally by the slave.
 *	Broadcast is not supported.
 *	Used by fn_0b_read_event_counter 
 *
 *	Parameters:
 *	unsigned char function 		- function code 0x0b 
 *	unsigned char slave    		- slave address
 *	
 */
static void * read_event_counter( void *param )
{
	unsigned char *packet = query_buffer;
	unsigned char *data;
	int response_length, query_length;
	int temp, i = 0;

	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	packet[0] = p->slave;
	packet[1] = p->function;
	query_length = 2;

	response_length = app_mbrtu_transaction(packet, query_length, &data, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);

	if (response_length < 0){
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	/*slave addr + func code + 4 byte data + 2 byte crc*/
	if (response_length != 8){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	for(i = 0; i < 2; i++ ) {
		/* shift reg hi_byte to temp */
		temp = data[ 2 + i *2 ] << 8;
		/* OR with lo_byte           */
		temp = temp | data[ 3 + i * 2 ];
		p->dest[i] = temp;
	}


	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
	return NULL;	
}

/**
 *
 *	read_exception_status
 *
 *	Reads the contents of eight Exception Status coils
 *	within the slave controller.
 *	Certain coils have predefined assignments in the various controllers. 
 *	Other coils can be programmed by the user to hold information about 
 *	the contoller's status. 
 *	The Exception Coil references are known (no coil reference 
 *	is needed in the function).
 *	Broadcast is not supported.
 *	Used by fn_07_read_exception_status 
 *
 *	Parameters:
 *	unsigned char slave    		- slave address
 *	unsigned char function 		- function code 0x07 
 *	
 */
static void * read_exception_status( void *param )
{
	unsigned char *packet = query_buffer;
	unsigned char *data;
	int response_length, query_length;
	int temp, bit, dest_pos = 0;
	int coils_processed = 0;

	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	packet[0] = p->slave;
	packet[1] = p->function;
	query_length = 2;

	response_length = app_mbrtu_transaction(packet, query_length, &data, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);

	if (response_length < 0){
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	/*slave addr + func code + 1 byte data + 2 byte crc*/
	if (response_length != 5){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	p->dest[dest_pos] = 8;
	temp = data[ 2 ] ;
	for( bit = 0x01; (bit & 0xff) && (coils_processed < 8); ) {
		dest_pos++;
		p->dest[dest_pos] = (temp & bit)?TRUE:FALSE;
		coils_processed++;
		bit = bit << 1;
	}

	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
	return NULL;
}

/**
 *
 *	read_IO_status handler
 *
 *	read_coil_stat_query and read_coil_stat_response interogate
 *	a modbus slave to get coil status. An array of coils shall be
 *	set to TRUE or FALSE according to the response from the slave.
 *	Used by fn_01_read_coil_status and by fn_02_read_input_status
 *
 *	Parameters(passed through the param structure):
 *	unsigned char function 		- function code 0x01 or 0x02
 *	unsigned char slave    		- slave address
 *	unsigned short int start_addr 	- starting address for the read operation
 *	unsigned short int count	- number of the coils to be read
 *	Returns in MODBUSdata the value of the coils:
 *	
 *	MODBUSdata[0] = # of the entries in  MODBUSdata
 *	MODBUSdata[i] = ON/OFF coil status from the lower to higher address requested.
 *	
 */
static void * read_IO_status( void *param )
{
	unsigned char *packet = query_buffer;
	unsigned char *data;
	int response_length, query_length;
	int temp, i, bit, dest_pos = 0;
	int coils_processed = 0;

	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	query_length = build_query_packet(p->slave, p->function, p->start_addr, p->count, packet);
	if (query_length < 0){
		app_mbrtu_status[1] = INTERNAL_ERROR;
		app_mbrtu_status[0] = 0;
		return NULL;	  
	}  

	response_length = app_mbrtu_transaction(packet, query_length, &data, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);

	if (response_length < 0){
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	/* NOTE: Integer division. This is equivalent of determining the ceil(count/8) */
	if (response_length != 3 + (p->count+7)/8){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	if (data[2] != (p->count+7)/8){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  
	/* Put in the first location of the destination buffer the number of valid data */
	p->dest[dest_pos] = p->count;
	dest_pos++;
	for( i = 0; (i < data[2]) && (i < 256); i++ ) {
		/* shift reg byte to temp */
		temp = data[ 3 + i ] ;
		for( bit = 0x01; (bit & 0xff) && (coils_processed < p->count); ) {
			p->dest[dest_pos] = (temp & bit)?TRUE:FALSE;
			coils_processed++;
			dest_pos++;
			bit = bit << 1;
		}
	}

	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
	return NULL;
}


/**
 *
 *	read_registers
 *
 *	Reads the binary contents of holding registers (4X references) in the slave.
 *	Broadcast is not supported
 *        
 *	Parameters(passed through the param structure):
 *	unsigned char function 		- function code 0x03 or 0x04
 *	unsigned char slave    		- slave address
 *	unsigned short int start_addr 	- starting address for the read operation
 *	unsigned short int count	- number of the registers to be read
 *	unsigned char byteorder 	- if 1 perform data inversion, otherwise we are modbus compliant
 *	Returns in MODBUSdata the value of regiters data:
 *
 *	MODBUSdata[0] = # of the entries in  MODBUSdata
 *	MODBUSdata[i] = register content
 */

static void * read_registers(void *param)
{
	unsigned char *data;
	unsigned char *packet = query_buffer;
	int response_length;
	int query_length;
	int temp,i;
	int dest_pos = 0;
	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	query_length = build_query_packet(p->slave, p->function, p->start_addr, p->count, packet);

#ifdef MODBUS_DEBUG
	printf( "[%s] - slave %d, function %d, start_addr %d, count %d packet '%p'.\n", __func__, p->slave, p->function, p->start_addr, p->count, packet);
	printf( "[%s] - function %d vs %d\n",__func__, p->function, packet[1]);
#endif
	if (query_length < 0){
		app_mbrtu_status[1] = INTERNAL_ERROR;
		app_mbrtu_status[0] = 0;
		return NULL;
	}	  

	response_length = app_mbrtu_transaction(packet, query_length, &data, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);
#ifdef MODBUS_DEBUG
	printf("[%s]: app_mbrtu_transaction DONE %d\n", __func__, response_length);
#endif

	if (response_length < 0) {
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;
#ifdef MODBUS_DEBUG
		printf( "[%s:%d] - invalid len (%d)\n",__func__,__LINE__, response_length);
#endif
		return NULL;
	}  

	if (response_length != 3 + 2*p->count){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
#ifdef MODBUS_DEBUG
		printf( "[%s:%d] - invalid frame lenght (%d vs %d)\n",__func__, __LINE__, response_length, 3 + 2*p->count);
#endif
		return NULL;
	}  

	if (data[2] != 2*p->count){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
#ifdef MODBUS_DEBUG
		printf( "[%s:%d] - invalid frame lenght (%d vs %d)\n",__func__, __LINE__, data[2], 2*p->count);
#endif
		return NULL;
	}

#ifdef MODBUS_DEBUG
	printf( "[%s:%d] - VALID frame\n",__func__, __LINE__, p->function);
#endif
	p->dest[dest_pos] = p->count;
	if ( p->byteorder == 0 ){
		/* we want data bye MSB first*/
		for(i = 0; i < (data[2]/2); i++ ) {
			dest_pos++;
			/* shift reg hi_byte to temp */
			temp = data[ 3 + i * 2 ] << 8;
			/* OR with lo_byte           */
			temp = temp | data[ 4 + i * 2 ];
			p->dest[dest_pos] = temp;
		}
	} 
	else { /*we want to perform inversion*/
		/* we want data bye LSB first*/
		for(i = 0; i < (data[2]/2); i++ ) {
			dest_pos++;
			/* shift reg hi_byte to temp */
			temp = data[ 4 + i * 2 ] << 8;
			/* OR with lo_byte           */
			temp = temp | data[ 3 + i *2 ];
			p->dest[dest_pos] = temp;
		}	
	}

	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
	return NULL;
}

/**
 *      Set_single handler
 *      set_single sends a value to a register in a slave.
 *
 *      Parameters(passed through the param structure):
 *
 *      unsigned char function 		- function code: 0x05, 0x06
 *      unsigned char slave 		- slave address
 *      unsigned short int start_addr 	- address of the register to be addressed, register 1 has address 0
 *      unsigned short int value 	- value to be written in the register
 *      
 *      Return: it echoes the query so we check the echo and return the exit succes code.
 *      NO value needs to be writte inside the MODBUSdata array.	
 *
 */


static void * set_single( void *param )
{
	unsigned char*packet = query_buffer;
	unsigned char *data;
	int query_length, response_length;

	struct app_mbrtu_params_s *p = (struct app_mbrtu_params_s *)param;

	query_length = build_query_packet(p->slave, p->function, p->start_addr, p->value, packet);

	if (query_length < 0){
		app_mbrtu_status[1] = INTERNAL_ERROR;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

#if MODBUS_DEBUG
	fprintf(stderr, "send_retries %d funtion %d\n", p->send_retries, packet[2]);	
#endif
	response_length = app_mbrtu_transaction(packet, query_length, &data, mbrtu_master, p->send_retries, p->error_code, p->response_timeout);
#if MODBUS_DEBUG
	fprintf(stderr, "response lenght = %d function %d\n", response_length, packet[2]);	
#endif
	/*Manage broadcast: we do not have answer so we check that we exited from the transaction with a time out error*/
	if (p->slave == 0){
		if ( response_length == -2 ){
			app_mbrtu_status[1] = 1;
			app_mbrtu_status[0] = 0;	
			return NULL;		
		}
		else {	
			app_mbrtu_status[1] = response_length;
			app_mbrtu_status[0] = 0;	
			return NULL;
		}
	} 	

	if (response_length < 0){
		app_mbrtu_status[1] = response_length;
		app_mbrtu_status[0] = 0;	
		return NULL;
	}  

	if (response_length != 6){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	}

	if ((data[2] != packet[2]) || (data[3] != packet[3]) ||
			(data[4] != packet[4]) || (data[5] != packet[5])){
		app_mbrtu_status[1] = INVALID_FRAME;
		app_mbrtu_status[0] = 0;
		return NULL;
	}  

	app_mbrtu_status[1] = response_length;
	app_mbrtu_status[0] = 0;	
	return NULL;	
}


/**
 *
 *	fn_01_read_coil_status() -- handler read_IO_status
 *	
 *	Reads the ON/OFF status of discrete outputs (0X references, coils) 
 *	in the slave. Function DOES NOT support broadcast.
 *
 *	@parameter unsigned short int slave  -- slave address
 *	@parameter unsigned short int start_addr -- starting coil to be read (coil 1 actually addresses coil 0)
 *	@parameter unsigned short int count -- quantity of the coil to be read MAX count allowed is 256
 * 	
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler, they are written in the MODBUSdata array
 *			MODBUSdata[0] = # of the entries in  MODBUSdata
 *			MODBUSdata[i] = ON/OFF coil status from the lower to higher address requested.
 *	@return 0 if ok, 1 if modbus is busy.
 */
unsigned short int fn_01_read_coil_status(unsigned short int slave, unsigned short int start_addr, unsigned short int count)
{
	int j;

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d addr: %d count: %d\n", __func__,slave, start_addr, count);
#endif

	if (!MODBUSstatus[0]){
		/*Set modbus status to BUSY*/
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		

		/* Reset Response buffer */
		for(j = 0; j < APP_MBRTU_DATA; j++)
			MODBUSdata[j] = 0;

		if(count > 256){ /* if number of coil to read is greater than the max allowed, its our limitation  NOT a protocol one*/

			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -5;
			return 0;		

		}	

		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x01;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];	
		app_mbrtu_params.start_addr = start_addr;
		app_mbrtu_params.count = count;

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}				

		app_mbrtu_thread_attr_setup();

		pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &read_IO_status, &app_mbrtu_params);
		return 0;	

	}
	return 1;				      		

}

/**
 *
 *	fn_02_read_input_status() -- handler read_IO_status
 *	
 *	Reads the ON/OFF status of discrete inputs (1X references) in the slave.
 *	Function DOES NOT support broadcast.
 *	in the slave. Function DOES NOT support broadcast.
 *	@parameter unsigned short int slave  -- slave address
 *	@parameter unsigned short int start_addr -- starting coil to be read (coil 1 actually addresses coil 0)
 *	@parameter unsigned short int count -- quantity of the coil to be read MAX count allowed is 256
 * 	
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler, they are written in the MODBUSdata array
 *			MODBUSdata[0] = # of the entries in  MODBUSdata
 *			MODBUSdata[i] = ON/OFF coil status from the lower to higher address requested.
 *	@return 0 if ok, 1 if modbus is busy.
 *
 */
unsigned short int fn_02_read_input_status(unsigned short int slave, unsigned short int start_addr, unsigned short int count)
{
	unsigned short int local_start_addr;
	int j;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d addr: %d count: %d\n", __func__,slave, start_addr, count);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){
		/*Set modbus status to BUSY*/
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		

		/* Reset Response buffer */
		for(j = 0; j < APP_MBRTU_DATA; j++)
			MODBUSdata[j] = 0;

		if(count > 256){ /* if number of coil to read is greater than the max allowed, its our limitation  NOT a protocol one*/

			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -5;
			return 0;		

		}

		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x02;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];	
		app_mbrtu_params.count = count;	

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}	

		if ( start_addr >= INPUT_STATUS_BASE ){

			local_start_addr = start_addr - INPUT_STATUS_BASE;	
			app_mbrtu_params.start_addr = local_start_addr;

			app_mbrtu_thread_attr_setup();

			pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &read_IO_status, &app_mbrtu_params);
			return 0;	
		}
		else {
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -5;
			return 0;
		}	
	}
	return 1;				      
}

/**
 *
 *	fn_03_read_holding_registers() -- handler read_registers
 *
 *	Read the holding registers in a slave and put the data into
 *	an array (4XXXX reference). Function DOES NOT support broadcast.
 *	@parameter unsigned short int slave  -- slave address
 *	@parameter unsigned short int start_addr -- starting register addres to be read (register 40001 actually addresses register 40000)
 *	@parameter unsigned short int count -- quantity of the registers to be read, limited by MAX_INPUT_REGS
 *	@parameter unsigned char byteorder -- modbus RTU data bytes are organized with HI byte first. 
 *					 Instead at least gavazzi's controllers put LO byte first.
 *					 We use byteorder to rearrange the received data when necessary.
 *					 If byteorder is 1 --> LO byte first, byteorder 0 --> HI byte first						
 * 	
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler, they are written in the MODBUSdata array
 *	             MODBUSdata[0] = # of the entries in  MODBUSdata
 *	             MODBUSdata[i] = register content
 *	@return 0 if ok, 1 if modbus is busy.
 *
 */

unsigned short int fn_03_read_holding_register(unsigned short int slave, unsigned short int start_addr, unsigned short int count,  unsigned char byteorder)
{
	unsigned short int local_start_addr;
	int j;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d addr: %d count: %d byteorder: '%d'\n", __func__,slave, start_addr, count, byteorder);
#endif
	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){
		/*Set modbus status to BUSY*/
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		

		/* Reset Response buffer */
		for(j = 0; j < APP_MBRTU_DATA; j++)
			MODBUSdata[j] = 0;

		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x03;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.byteorder = byteorder;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];		

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}	

		if ( start_addr >= HOLDING_REGISTER_BASE ){

			local_start_addr = start_addr - HOLDING_REGISTER_BASE;	
			app_mbrtu_params.start_addr = local_start_addr;

#warning "MODBUS_RTU FN03-FN04:   MAX_INPUT_REGS has been FIXED to 12 according to GAVAZZI's requirement, maybe this need to be changed!!!!!!!"	

			if( count > MAX_INPUT_REGS ) {
				count = MAX_INPUT_REGS;
#ifdef MODBUS_DEBUG
				fprintf( stderr, "Too many input registers requested.\n" );
#endif
			}
			app_mbrtu_params.count = count;


			app_mbrtu_thread_attr_setup();

			pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &read_registers, &app_mbrtu_params);
			return 0;	
		}
		else {
#ifdef MODBUS_DEBUG
			printf( "[%s] - Base address wrong %d < %d.\n", __func__, start_addr, HOLDING_REGISTER_BASE );
#endif
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -5;
			return 0;
		}	
	}
	return 1;				      

}


/**
 *
 *	fn_04_read_input_registers() -- handler read_registers
 *
 *	Read the holding registers in a slave and put the data into
 *	an array (3XXXX reference). Function DOES NOT support broadcast.
 *	@parameter unsigned short int slave  -- slave address
 *	@parameter unsigned short int start_addr -- starting register address to be read (register 30001 actually addresses register 30000)
 *	@parameter unsigned short int count -- quantity of the registers to be read, limited by MAX_INPUT_REGS
 *	@parameter unsigned char byteorder -- modbus RTU data bytes are organized with HI byte first. 
 *					 Instead at least gavazzi's controllers put LO byte first.
 *					 We use byteorder to rearrange the received data when necessary.
 *					 If byteorder is 1 --> LO byte first, byteorder 0 --> HI byte first						
 * 	
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler, they are written in the MODBUSdata array
 *	             MODBUSdata[0] = # of the entries in  MODBUSdata
 *	             MODBUSdata[i] = register content
 *	@return 0 if ok, 1 if modbus is busy.
 *
 */


unsigned short int fn_04_read_input_registers(unsigned short int slave, unsigned short int start_addr, unsigned short int count, unsigned char byteorder )
{

	unsigned short int local_start_addr;
	int j;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d addr: %d count: %d byteorder: %d\n", __func__,slave, start_addr, count, byteorder);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){
		/*Set modbus status to BUSY*/
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		

		/* Reset Response buffer */
		for(j = 0; j < APP_MBRTU_DATA; j++)
			MODBUSdata[j] = 0;

		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x04;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.byteorder = byteorder;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];		

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}	

		if ( start_addr >= INPUT_REGISTER_BASE ){

			local_start_addr = start_addr - INPUT_REGISTER_BASE;	
			app_mbrtu_params.start_addr = local_start_addr;

			//#warning "MODBUS_RTU:   MAX_INPUT_REGS has been FIXED to 12 according to GAVAZZI requirement, maybe this need to be changed!!!!!!!"	
#warning "MODBUS_RTU:   MAX_INPUT_REGS has been FIXED to 16 according to MX2 requirement, maybe this need to be changed!!!!!!!"	

			if( count > MAX_INPUT_REGS ){
				count = MAX_INPUT_REGS;
#ifdef MODBUS_DEBUG
				fprintf( stderr, "Too many input registers requested.\n" );
#endif
			}
			app_mbrtu_params.count = count;

			app_mbrtu_thread_attr_setup();

			pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &read_registers, &app_mbrtu_params);
			return 0;	
		}
		else {
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -5;
			return 0;
		}	
	}
	return 1;				      
}


/**
 *
 *	fn_05_force_single_coil() -- handler set_single
 *
 *	(0X reference) to either ON or OFF. When broadcast, the function 
 *	forces the same coil reference in all attached slaves.
 *	The query message specifies the coil reference to be forced. 
 *	Coils are addressed starting at zero: coil 1 is addressed as 0.	
 *	Function DOES support broadcast.
 *
 *	@parameter unsigned short int slave  -- slave address
 *	@parameter unsigned short int coil_addr -- coil address to be refrenced
 *	@parameter unsigned short int coil_state -- 0xFF00 --> coil ON, 0x0000 --> coil OFF
 * 	
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: none;
 *	@return 0 if ok, 1 if modbus is busy.
 *
 */

unsigned short int fn_05_force_single_coil(unsigned short int slave, unsigned short int coil_addr, unsigned short int coil_state)
{

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d addrl_addr: %d coil_state: %d\n", __func__,slave, coil_addr, coil_state);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){

		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;

		app_mbrtu_params.function = 0x05;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.start_addr = coil_addr;
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];

		if (coil_state)
			app_mbrtu_params.value = 0xff00;
		else
			app_mbrtu_params.value = 0x0000;	

		app_mbrtu_thread_attr_setup();

		pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &set_single, &app_mbrtu_params);
		return 0;	
	}
	return 1;	

}

/**
 *
 *	fn_06_preset_single_register() -- handler set_single
 *
 *	Presets a value into a single holding register (4X reference). 
 *	When broadcast, the function presets the same register reference 
 *	in all attached slaves. Registers are addressed starting at zero: 
 *	register 1 is addressed as 0.	
 *	NOTE: value to be written in the addressed register must be stored in 
 *	MODBUSdata[0]
 *	
 *	@parameter unsigned short int slave  -- slave address
 *	@parameter unsigned short int reg_addr -- register address to be refrenced (register 40001 actually references register 0)					
 * 	
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: none;
 *	@return 0 if ok, 1 if modbus is busy.
 *
 */

unsigned short int fn_06_preset_single_register(unsigned short int slave, unsigned short int reg_addr )
{

	/* NOTE: 
	   At the beginning the function took as parameter also an  
	   int send_retries number of retries at the application level, 
	   unsigned char *error_code pointer to a memory area where the exception error code will be eventually written to
	   unsigned short int app_timeout  application timeout to be checked against min_timeout .
	   Then we decided to be compliant with already existing modbus ascii application function prototypes so we decided to:
	   put send_retires number fixed to 0
	   put the error_code in the global variable app_mbrtu_status[2] if necessary
	   put the app_timeout to the fixed value 4.0*min_timeout thus we do not need anymore to make any check between app_timeout and min_timeout
	   values before starting the mbrtu thread for set_single or read_register operations
	 */

	unsigned short int local_start_addr;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d reg_addr: %d\n", __func__,slave, reg_addr);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){


		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;

		app_mbrtu_params.function = 0x06;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.value = MODBUSdata[0];
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];

		if ( reg_addr >= HOLDING_REGISTER_BASE ){

			local_start_addr = reg_addr - HOLDING_REGISTER_BASE;	
			app_mbrtu_params.start_addr = local_start_addr;

			app_mbrtu_thread_attr_setup();

			pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &set_single, &app_mbrtu_params);
			return 0;
		}
		else {
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -5;
			return 0;
		}	
	}
	return 1;	

}


/**
 *
 *	fn_07_read_exception_status() -- Handler read_exception_status
 *	
 *	Reads the contents of eight Exception Status coils
 *	within the slave controller.
 *	Certain coils have predefined assignments in the various controllers. 
 *	Other coils can be programmed by the user to hold information about 
 *	the contoller's status. 
 *	The Exception Coil references are known (no coil reference 
 *	is needed in the function).
 *	Broadcast is not supported.
 *	
 *	@parameter unsigned short int slave  -- slave address			
 * 	
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler, they are written in the MODBUSdata array;
 *		     MODBUSdata[0] = Number of the exception status coils (8);
 *		     MODBUSdata[1] = exception status  coil1  value (ON/OFF) 
 *		     MODBUSdata[2] = exception status  coil2  value (ON/OFF) ...
 *	
 *	@return 0 if ok, 1 if modbus is busy.
 *
 */

unsigned short int fn_07_read_exception_status(unsigned short int slave)
{
	int j;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d\n", __func__,slave);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){
		/*Set modbus status to BUSY*/
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		

		/* Reset Response buffer */
		for(j = 0; j < APP_MBRTU_DATA; j++)
			MODBUSdata[j] = 0;

		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x07;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];	

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}				

		app_mbrtu_thread_attr_setup();

		pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &read_exception_status, &app_mbrtu_params);
		return 0;	

	}
	return 1;				      		

}



/**
 *
 *	fn_0b_read_event_counter() -- Handler read_event_counter
 *	
 *	Returns a status word and an event count from the slave's 
 *	communications event counter. By fetching the current count before 
 *	and after a series of messages, a master can determine whether the 
 *	messages were handled normally by the slave.
 *
 *	Broadcast is not supported.
 *
 *	@parameter unsigned short int slave  -- slave address			
 * 	
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler, they are written in the MODBUSdata array:
 *		       MODBUSdata[0] = status word
 *		       MODBUSdata[1] = event count
 *	@return 0 if ok, 1 if modbus is busy.
 *
 */
unsigned short int fn_0b_read_event_counter(unsigned short int slave)
{
	int j;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d\n", __func__,slave);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){
		/*Set modbus status to BUSY*/
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		

		/* Reset Response buffer */
		for(j = 0; j < APP_MBRTU_DATA; j++)
			MODBUSdata[j] = 0;

		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x0b;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];	

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}				

		app_mbrtu_thread_attr_setup();

		pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &read_event_counter, &app_mbrtu_params);
		return 0;	

	}
	return 1;				      		

}

/**
 *
 *	fn_0c_read_event_log() -- Handler read_event_log
 *	
 *	Returns a status word, event count, message count, and a field of 
 *	event bytes from the slave.
 *	Broadcast is not supported.
 *
 *	@parameter unsigned short int slave  -- slave address			
 * 	
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler, they are written in the MODBUSdata array:
 *		       MODBUSdata[0] = Number of event (0-64)
 *		       MODBUSdata[1] = Status Word
 *		       MODBUSdata[2] = Event Count
 *		       MODBUSdata[3] = Message Count
 *		       MODBUSdata[4] = Event 0 byte
 *		       MODBUSdata[5] = Event 1 byte ....
 *
 *	@return 0 if ok, 1 if modbus is busy.
 *
 */
unsigned short int fn_0c_read_event_log(unsigned short int slave)
{
	int j;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d\n", __func__,slave);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){
		/*Set modbus status to BUSY*/
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		

		/* Reset Response buffer */
		for(j = 0; j < APP_MBRTU_DATA; j++)
			MODBUSdata[j] = 0;

		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x0c;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];	

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}				

		app_mbrtu_thread_attr_setup();

		pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &read_event_log, &app_mbrtu_params);
		return 0;	

	}
	return 1;				      		

}


/**
 *
 *	fn_0f_force_multiple_coil() -- Handler write_multiple_coil
 *	
 *	Forces each coil (0X reference) in a sequence of coils to either ON 
 *	or OFF. When broadcast, the function forces the same coil references 
 *	in all attached slaves.
 *
 *	Function DOES support broadcast.
 *
 *	@parameter unsigned short int slave  -- slave address
 *	@parameter unsigned short int start_addr  -- coil start address	
 *	@parameter unsigned short int count  -- number of coil to set				
 * 	
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler, none.
 *	Data to be forced for the coils are taken from MODBUSdata[] array.
 *		MODBUSdata[0] = coil0 
 *		MODBUSdata[1] = coil1...
 *
 *	@return 0 if ok, 1 if modbus is busy.
 *
 */
unsigned short int fn_0f_force_multiple_coil(unsigned short int slave, unsigned short int coil_addr, unsigned short int coil_count)
{


#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d coil_addr: %d coil_count: %d\n", __func__,slave, coil_addr, coil_count);
	{
		int i;
		for (i = 0; i < coil_count; i++)
		{
			printf("[%s]: MODBUSdata[%d] (coil%d) = %2x\n", __func__, i, i, MODBUSdata[i]);
		}
	}
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){

		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;

		app_mbrtu_params.function = 0x0f;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.start_addr = coil_addr;
		app_mbrtu_params.count = coil_count;
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];
		app_mbrtu_params.source = &MODBUSdata[0];

		app_mbrtu_thread_attr_setup();

		pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &write_multiple_coil, &app_mbrtu_params);
		return 0;	
	}
	return 1;	

}


/**
 *
 *	fn_10_preset_multiple_registers() -- Handler write_multiple_register
 *	
 *	Presets values into a sequence of holding registers (4X references). 
 *	When broadcast, the function presets the same register references
 *	in all attached slaves
 *	
 *	Function DOES support broadcast.
 *
 *	@parameter unsigned short int slave  -- slave address
 *	@parameter unsigned short int start_addr  -- register start address	
 *	@parameter unsigned short int count  -- number of register to set				
 * 	
 *	Data to be written in the registers are taken from MODBUSdata[] array.
 *		MODBUSdata[0] = data_reg0 
 *		MODBUSdata[1] = data_reg1...
 *
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler, none.
 *
 *	@return 0 if ok, 1 if modbus is busy.
 */
unsigned short int fn_10_preset_multiple_registers(unsigned short int slave, unsigned short int start_addr, unsigned short int count)
{

	unsigned short int local_start_addr;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d start_addr: %d count: %d\n", __func__,slave, start_addr, count);
	{
		int i;
		for (i = 0; i < count; i++)
		{
			printf("[%s]: MODBUSdata[%d] (data_reg%d) = %2x\n", __func__, i, i, MODBUSdata[i]);
		}
	}
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){

		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;

		app_mbrtu_params.function = 0x10;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.reg_addr = start_addr;
		app_mbrtu_params.reg_count = count;
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];
		app_mbrtu_params.source = &MODBUSdata[0];

		if ( start_addr >= HOLDING_REGISTER_BASE ){

			local_start_addr = start_addr - HOLDING_REGISTER_BASE;
			app_mbrtu_params.reg_addr = local_start_addr;			

			app_mbrtu_thread_attr_setup();

			pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &write_multiple_register, &app_mbrtu_params);
			return 0;
		}
		else {
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -5;
			return 0;
		}					
	}
	return 1;	

}


/**
 *
 *	fn_11_report_slave_id() -- Handler read_slave_id
 *	
 *	Returns a description of the type of controller present at the 
 *	slave address, the current status of the slave Run indicator, 
 *	and other information specific to the
 *	slave device. Broadcast is not supported
 *	@parameter unsigned short int slave  -- slave address			
 * 	
 *
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler. Output is controller type dependand so the 
 *	function just propagates the received byte without
 *	any guess about the format/meaning.
 *		MODBUSdata[0] = # of the received byte, each of them is stored in the following array location
 *			in the same arrangement they are received
 *		MODBUSdata[i] = data byte
 *
 *	@return 0 if ok, 1 if modbus is busy.	
 *
 */
unsigned short int fn_11_report_slave_id(unsigned short int slave)
{
	int j;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d\n", __func__,slave);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){
		/*Set modbus status to BUSY*/
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		

		/* Reset Response buffer */
		for(j = 0; j < APP_MBRTU_DATA; j++)
			MODBUSdata[j] = 0;

		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x11;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];	

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}				

		app_mbrtu_thread_attr_setup();

		pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &read_slave_id, &app_mbrtu_params);
		return 0;	

	}
	return 1;				      		

}


/**
 *
 *	fn_14_read_general_reference() -- Handler read_general_ref
 *	
 *	Returns the contents of registers in Extended Memory 
 *	file (6XXXXX) references.
 *	The function can read multiple groups of references. The groups can be separate
 *	but the references within each group must be sequential
 *	Broadcast is not supported
 *
 *	@parameter unsigned short int slave  -- slave address
 *	@parameter unsigned short int byte_count  -- request byte number			
 * 	
 *	The query requires that for the group or groups of reference to be read
 *	the following information:
 *	2 byte for Extended Memory file number (1 to 10, hex 0001 to 000A);	
 *	2 byte starting register address within the file
 *	2 byte quantity of registers to be read.
 *
 *	The quantity of registers to be read, combined with all other fields in the expected
 *	response, must not exceed the allowable length of Modbus messages: 256 bytes.
 *
 *	These data are gathered from the MODBUSdata array:
 *		MODBUSdata[0] = number of group of refrence to be read (from 1 ...to N)
 *		MODBUSdata[1] = extended Memory file number for group 1
 *		MODBUSdata[2] = starting register address for group 1
 *		MODBUSdata[3] = quantity of registers to be read for group 1
 *		MODBUSdata[4] = extended Memory file number for group 2
 *		MODBUSdata[5] = starting register address for group 2
 *		MODBUSdata[6] = quantity of registers to be read for group 2
 *		........
 *
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler. Output is organized as follows:
 *
 *		MODBUSdata[0] = the total number of valid entries in the following array location
 *		MODBUSdata[1] = # of data N for group reference 1
 *		MODBUSdata[2] = data0 for group reference 1
 *		MODBUSdata[3] = data1 for group reference 1
 *		.....
 *		MODBUSdata[K] = dataN for group reference 1
 *		MODBUSdata[K+1] = # of data M for group reference 2
 *		MODBUSdata[K+2] = data0 for group reference 2
 *		.....
 *		MODBUSdata[H] = dataM for group reference 2
 *
 *
 *	@return 0 if ok, 1 if modbus is busy.	
 *
 */
unsigned short int fn_14_read_general_reference(unsigned short int slave )
{

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d\n", __func__,slave);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}


	if (!MODBUSstatus[0]){
		/* Set modbus status to BUSY */
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		


		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x14;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.source = &MODBUSdata[0];
		app_mbrtu_params.count = MODBUSdata[0]; /*count of the reference group*/
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];	

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}				

		app_mbrtu_thread_attr_setup();

		pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &read_general_ref, &app_mbrtu_params);
		return 0;	

	}
	return 1;				      		

}

/**
 *
 *	fn_15_write_general_reference() -- Handler write_general_ref
 *	
 *	Writes the contents of registers in Extended Memory file 
 *	(6XXXXX) references.
 *
 *	Broadcast is not supported.
 *
 *	The function can write multiple groups of references. 
 *	The groups can be separate (noncontiguous), but the references 
 *	within each group must be sequential.
 *	
 *	NOTE: to work properly this function requires that the information
 *	about data to be written are packeted as follow in the MODBUSdata
 *	array for each group of references to be written:
 *	MODBUSdata[0] = 0x06 the reference type fixed to 0x0006
 *	MODBUSdata[1] = Extended Memory file number (1 to 10, hex 0001 to 000A);
 *	MODBUSdata[2] = starting register address within the file
 *	MODBUSdata[3] = quantity of registers to be written
 *	MODBUSdata[4] =  data to be written
 *	MODBUSdata[5] =  data to be written
 *	MODBUSdata[6] =  data to be written
 *	MODBUSdata[7] =  data to be written
 *	....
 *	MODBUSdata[n] = 0x06 the reference type fixed to 0x0006
 *	MODBUSdata[n+1] = Extended Memory file number (1 to 10, hex 0001 to 000A);
 *	MODBUSdata[n+2] = starting register address within the file
 *	MODBUSdata[n+3] = quantity of registers to be written
 *	MODBUSdata[n+4] =  data to be written
 *	MODBUSdata[n+5] =  data to be written
 *	MODBUSdata[n+6] =  data to be written
 *	MODBUSdata[n+7] =  data to be written
 *	....
 *	
 *	The count parametr gives the number of valid entries in the MODBUSdata array.
 *
 *	2 byte --> unsigned short int, hence 1 location in the array	
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler. Output is an echo of the query,
 *	the output is checked by the handler and nothing is returned to the application.
 *
 *	@return 0 if ok, 1 if modbus is busy.	
 *	
 */
unsigned short int fn_15_write_general_reference(unsigned short int slave, unsigned short int count )
{

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d count %d\n", __func__,slave, count);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){
		/* Set modbus status to BUSY */
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		


		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x15;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.source = &MODBUSdata[0];
		app_mbrtu_params.count = count;
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];	

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}				

		app_mbrtu_thread_attr_setup();

		pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &write_general_ref, &app_mbrtu_params);
		return 0;	

	}
	return 1;				      		

}

/**
 *
 *	fn_16_mask_write_registers() -- Handler mask_write_register()
 *	
 *	Modifies the contents of a specified 4XXXX register using a 
 *	combination of an AND mask, an OR mask, and the registers current
 *	contents. The function can be used to set or clear individual 
 *	bits in the register. Broadcast is not supported.
 *	
 *	NOTE: to work properly this function requires that the information
 *	about data to be written are packeted as follow in the MODBUSdata
 *	array:
 *	MODBUSdata[0] = And Mask
 *	MODBUSdata[1] = Or Mask
 *	
 *	2 byte --> unsigned short int, hence 1 location in the array	
 *
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler. Output is an echo of the query,
 *	the output is checked by the handler and nothing is returned to the application.
 *
 *	@return 0 if ok, 1 if modbus is busy.	
 *		
 *
 */
unsigned short int fn_16_mask_write_registers(unsigned short int slave, unsigned short int start_addr)
{
	unsigned short int local_start_addr;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d start_addr %d\n", __func__,slave, start_addr);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){
		/*Set modbus status to BUSY*/
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		

		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x16;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.source = &MODBUSdata[0];
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];		

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}	

		if ( start_addr >= HOLDING_REGISTER_BASE ){

			local_start_addr = start_addr - HOLDING_REGISTER_BASE;	
			app_mbrtu_params.start_addr = local_start_addr;

			app_mbrtu_thread_attr_setup();

			pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &mask_write_register, &app_mbrtu_params);
			return 0;	
		}
		else {
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -5;
			return 0;
		}	
	}
	return 1;				      

}


/**
 *	fn_17_read_write_registers() -- handler read_write_registers
 *	
 *	Read/Write 4X Registers
 *	Performs a combination of one read and one write operation in 
 *	a single Modbus transaction. The function can write new contents 
 *	to a group of 4XXXX registers, and then return the contents of 
 *	another group of 4XXXX registers. Broadcast is not supported.
 *
 *	NOTE: to work properly this function requires that the information
 *	about data to be written are packeted as follow in the MODBUSdata
 *	array:
 *	MODBUSdata[0] = Write DATA 0
 *	MODBUSdata[1] = Write DATA 1
 *	MODBUSdata[2] = Write DATA 2
 *	......
 *	MODBUSdata[N] = Write DATA N
 *
 *	@parameter unsigned short int slave  -- slave address
 *	@parameter unsigned short int start_addr  -- address of the register to be read
 *	@parameter unsigned short int count  -- number of location to be read
 *	@parameter unsigned short int reg_addr  -- address of the register to be written
 *	@parameter unsigned short int reg_count  -- number of the register to be written
 *	@parameter unsigned short int byteorder -- modbus RTU data bytes are organized with HI byte first. 
 *					 Instead at least gavazzi's controllers put LO byte first.
 *					 We use byteorder to rearrange the received data when necessary.
 *					 If byteorder is 1 --> LO byte first, byteorder 0 --> HI byte first
 *
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler. Values read by the handler are store in the MODBUSdata array.
 *	MODBUSdata[0] = Number of read entries
 *	MODBUSdata[1] = Read DATA 0
 *	MODBUSdata[2] = Read DATA 1
 *	......
 *	MODBUSdata[N] = Read DATA N
 *
 *	@return 0 if ok, 1 if modbus is busy.
 */
unsigned short int fn_17_read_write_registers(unsigned short int slave, unsigned short int start_addr, unsigned short int count, unsigned short int reg_addr,unsigned short int reg_count, unsigned char byteorder)
{
	unsigned short int local_start_addr;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d start_addr %d, count %d, reg_addr %d, reg_count %d, byteorder %d\n", __func__, slave, start_addr, count, reg_addr, reg_count, byteorder);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){
		/*Set modbus status to BUSY*/
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		

		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x17;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.source = &MODBUSdata[0];
		app_mbrtu_params.reg_addr = reg_addr;
		app_mbrtu_params.reg_count = reg_count;
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.byteorder = byteorder;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];		

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}	

		if ( start_addr >= HOLDING_REGISTER_BASE ){

			local_start_addr = start_addr - HOLDING_REGISTER_BASE;	
			app_mbrtu_params.start_addr = local_start_addr;
			app_mbrtu_params.count = count;

			app_mbrtu_thread_attr_setup();

			pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &read_write_registers, &app_mbrtu_params);
			return 0;	
		}
		else {
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -5;
			return 0;
		}	
	}
	return 1;			



}

/**
 *	fn_18_read_fifo_queue -- Handler read_fifo_queue
 *	
 *	Reads the contents of a First In First Out (FIFO)
 *	queue of 4XXXX registers. The function returns a count of the 
 *	registers in the queue, followed by the queued data.
 *	Up to 32 registers can be read: the count, plus up to 31 queued 
 *	data registers. The queue count register is returned first, 
 *	followed by the queued data registers.
 *	The function reads the queue contents, but does not clear them. 
 *	Broadcast is not supported.	
 *
 *	error code : from the handler, they are written in MODBUSstatus[1];
 *	return value: from the handler. Values read by the handler are store in the MODBUSdata array.
 *	MODBUSdata[0] = Number of read entries
 *	MODBUSdata[1] = Read DATA 1
 *	MODBUSdata[2] = Read DATA 2
 *	......
 *	MODBUSdata[N] = Read DATA N
 *
 *	@return 0 if ok, 1 if modbus is busy.	
 */

unsigned short int fn_18_read_fifo_queue(unsigned short int slave, unsigned short int start_addr)
{
	unsigned short int local_start_addr;
	int j;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d start_addr %d MODBUSstatus[0] %d\n", __func__, slave, start_addr, MODBUSstatus[0]);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){
		/*Set modbus status to BUSY*/
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		

		/* Reset Response buffer */
		for(j = 0; j < APP_MBRTU_DATA; j++)
			MODBUSdata[j] = 0;

		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x18;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];		

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}	

		if ( start_addr >= HOLDING_REGISTER_BASE ){

			local_start_addr = start_addr - HOLDING_REGISTER_BASE;	
			app_mbrtu_params.start_addr = local_start_addr;

			app_mbrtu_thread_attr_setup();

#ifdef MODBUS_DEBUG
			printf("[%s]: thread read_fifo_queue starting\n", __func__ );
#endif
			pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &read_fifo_queue, &app_mbrtu_params);
#ifdef MODBUS_DEBUG
			printf("[%s]: DONE1: %d MODBUSstatus[0] %d \n", __func__, MODBUSstatus[0]);
#endif
			return 0;	
		}
		else {
#ifdef MODBUS_DEBUG
			printf("[%s]: DONE2 MODBUSstatus[0] %d\n", __func__, MODBUSstatus[0]);
#endif
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -5;
			return 0;
		}	
	}
#ifdef MODBUS_DEBUG
	printf("[%s]: BUSY MODBUSstatus[0] %d\n", __func__, MODBUSstatus[0]);
#endif
	return 1;				      
}


/**
 *	fn_08_diagnostics
 *	
 *	Modbus function 08 provides a series of tests for checking 
 *	the communication system between the master and slave, or for
 *	checking various internal error conditions within the slave. 
 *	Broadcast is not supported.
 *	
 *	The function uses a two byte subfunction code field in the query 
 *	to define the type of test to be performed. 
 *	The slave echoes both the function code and subfunction code in 
 *	a normal response.
 *	Most of the diagnostic queries use a two byte data field to send 
 *	diagnostic data or control information to the slave. 
 *	Some of the diagnostics cause data to be returned
 *	from the slave in the data field of a normal response.
 *	
 *	For all subfunction code the data field to be sent in the query must be stored
 *	in MODBUSdata[0]	
 *
 *	for all subfunction code that returns data on two byte (all except 
 *	0x00 0x01 0x04 0x14 and 0x015) the returned value is stored in 
 *	MODBUSdata[0].
 *	Subfunction 0x00 0x01 0x04 0x014 returns just an echo of the query
 *	so we check if the answer is valid without explicitly returning any value.
 *	Subfunction 0x15 may return up to 108 bytes that are store in the 
 *	MODBUSdata array:  MODBUSdata[0] contains the bytecount the subsequent
 *	array location contains the retrieved data.
 *	
 */

unsigned short int fn_08_diagnostics(unsigned short int slave, unsigned short int subfunction )
{
	int j;

#ifdef MODBUS_DEBUG
	printf("[%s]: slave: %d subfunction: %d\n", __func__,slave, subfunction);
#endif

	if (mbrtu_master == NULL)
	{
		printf("[%s] : Error, modbus not initialized.\n",__func__);
		app_mbrtu_status[1] = MODBUS_ERROR;
		app_mbrtu_status[0] = 0;
		return 0;
	}

	if (!MODBUSstatus[0]){
		/*Set modbus status to BUSY*/
		MODBUSstatus[0] = app_mbrtu_status[0] = 1;

		/* Reset error status*/
		app_mbrtu_status[1] = 0;		

		/* Reset Response buffer */
		for(j = 0; j < APP_MBRTU_DATA; j++)
			MODBUSdata[j] = 0;

		/*Copy the input parameter in a suitable parameter structure (necessary to the subsequent thread) */
		app_mbrtu_params.function = 0x08;
		app_mbrtu_params.subfunction = subfunction;
		app_mbrtu_params.slave = (unsigned char)slave;
		app_mbrtu_params.dest = &MODBUSdata[0];
		app_mbrtu_params.value = MODBUSdata[0];
		app_mbrtu_params.send_retries = 0;
		app_mbrtu_params.error_code = (unsigned char *)&app_mbrtu_status[2];		

		if (slave == 0){
			app_mbrtu_status[0] = 0;
			app_mbrtu_status[1] = -6;
			return 0;
		}	

		app_mbrtu_thread_attr_setup();

		pthread_create(&app_mbrtu_thread_id, &app_mbrtu_attr, &diagnostics, &app_mbrtu_params);
		return 0;	

	}
	return 1;				      

}



unsigned short int fn_response_byteorder_invert(unsigned short int count){

	unsigned char data[2];
	int temp,i;


	for(i = 0; i < count; i++ ) {
		/* low byte */
		data[0] = MODBUSdata[i] >> 8; 
		/* high byte */
		data[1] = MODBUSdata[i] ; 

		/* shift reg hi_byte to temp */
		temp = data[1] << 8;
		/* OR with lo_byte           */
		temp = temp | data[0];
		MODBUSdata[i] = temp;
	}
	return 0;

}



unsigned short int MB_byteorder(unsigned short int count)
{
	return fn_response_byteorder_invert(count);
}



/****************************************************/
/****************************************************/
/*		Main program to test		    */	
/*		Modbus RTU functions		    */	
/****************************************************/
/****************************************************/

/*  int main(void)
    {

// unsigned short MODBUSdata[256];
// signed short MODBUSstatus[2];
int rv = 0;
int j = 0;


if ( app_mbrtu_init(9600,8,0,1 ) == 0 ){
if ( app_mbrtu_connect( mbrtu_master ) ){


// From here we should be able to satisfy modbus request 
#if MODBUS_DEBUG
fprintf(stderr, "ATTEMPTING fn_06_preset_single_register\n");
#endif			
MODBUSstatus[0] = 0;
app_mbrtu_status[0] = app_mbrtu_status[1] = 0;
MODBUSdata[0] = 0;
rv = fn_06_preset_single_register(0x01, 0xCF41);
sleep(20);
#if MODBUS_DEBUG

if( rv == 0 )
fprintf(stderr, "SHOULD BE A SUCCESS for fn_06_preset_single_register, %d\n", app_mbrtu_status[1]);
else
fprintf(stderr, "SHOULD BE A MESS for fn_06_preset_single_register %d\n", app_mbrtu_status[1]);

#endif			



fprintf(stderr, "ATTEMPTING fn_04_read_input_registers\n");
MODBUSstatus[0] = 0;
app_mbrtu_status[0] = app_mbrtu_status[1] = 0;
rv = fn_04_read_input_registers(1, 30641, 1, 1);


sleep(20);
if( rv == 0 )
fprintf(stderr, "SHOULD BE A SUCCESS for fn_04_read_input_registers%d\n", app_mbrtu_status[1]);
else
fprintf(stderr, "SHOULD BE A MESS for fn_04_read_input_registers%d\n", app_mbrtu_status[1]);

j++;

fprintf(stderr, "response buffer0 = %d\n", MODBUSdata[0]);
// fn_response_byteorder_invert(1);
fprintf(stderr, "response buffer1 = %d\n", MODBUSdata[1]);
sleep(10);
return app_mbrtu_done();


}
else {
return app_mbrtu_done();
}

}

return 0;
} 

 */



/****************************************************/
/****************************************************/
/*		FarosPLC Wrapper to access	    */	
/*		Modbus RTU functions		    */	
/****************************************************/
/****************************************************/
void MB_f01(STDLIBFUNCALL)
{
	MB_F01_PARAM OS_SPTR *pPara = (MB_F01_PARAM OS_SPTR *)pIN;

	pPara->ret_value = fn_01_read_coil_status(pPara->slave, pPara->start_addr, pPara->count);
}

void MB_f02(STDLIBFUNCALL)
{
	MB_F02_PARAM OS_SPTR *pPara = (MB_F02_PARAM OS_SPTR *)pIN;

	pPara->ret_value = fn_02_read_input_status(pPara->slave, pPara->start_addr, pPara->count);
}

void MB_f03(STDLIBFUNCALL)
{
	MB_F03_PARAM OS_SPTR *pPara = (MB_F03_PARAM OS_SPTR *)pIN;

	pPara->ret_value = fn_03_read_holding_register(pPara->slave, pPara->start_addr, pPara->count, pPara->byteorder);
}

void MB_f04(STDLIBFUNCALL)
{
	MB_F04_PARAM OS_SPTR *pPara = (MB_F04_PARAM OS_SPTR *)pIN;

	pPara->ret_value = fn_04_read_input_registers(pPara->slave, pPara->start_addr, pPara->count, pPara->byteorder);
}

void MB_f05(STDLIBFUNCALL)
{
	MB_F05_PARAM OS_SPTR *pPara = (MB_F05_PARAM OS_SPTR *)pIN;

	pPara->ret_value = fn_05_force_single_coil(pPara->slave, pPara->coil_addr, pPara->coil_state);
}

void MB_f06(STDLIBFUNCALL)
{
	MB_F06_PARAM OS_SPTR *pPara = (MB_F06_PARAM OS_SPTR *)pIN;

	MODBUSdata[0] =  pPara->value;
	pPara->ret_value = fn_06_preset_single_register(pPara->slave, pPara->reg_addr);
}

void MB_f07(STDLIBFUNCALL)
{
	MB_F07_PARAM OS_SPTR *pPara = (MB_F07_PARAM OS_SPTR *)pIN;

	/* NOTE: you need oly the first 3 values */
	pPara->ret_value = fn_07_read_exception_status(pPara->slave);
}

void MB_f08(STDLIBFUNCALL)
{
	MB_F08_PARAM OS_SPTR *pPara = (MB_F08_PARAM OS_SPTR *)pIN;

	pPara->ret_value = fn_08_diagnostics(pPara->slave, pPara->subfunction);
}

void MB_f0b(STDLIBFUNCALL)
{
	MB_F0b_PARAM OS_SPTR *pPara = (MB_F0b_PARAM OS_SPTR *)pIN;

	/* NOTE: you need oly the first 2 values */
	pPara->ret_value = fn_0b_read_event_counter(pPara->slave);
}

void MB_f0c(STDLIBFUNCALL)
{
	MB_F0c_PARAM OS_SPTR *pPara = (MB_F0c_PARAM OS_SPTR *)pIN;

	pPara->ret_value = fn_0c_read_event_log(pPara->slave);
}

void MB_f0f(STDLIBFUNCALL)
{
	MB_F0f_PARAM OS_SPTR *pPara = (MB_F0f_PARAM OS_SPTR *)pIN;
	MB_DATA_ARRAY_PARAM OS_DPTR *pData = (MB_DATA_ARRAY_PARAM OS_DPTR* )pPara->data;

	/* the coils value to set must be stored into MODBUSdata array */
	memcpy(&MODBUSdata[0], pData->pElem, APP_MBRTU_DATA);
	pPara->ret_value = fn_0f_force_multiple_coil(pPara->slave, pPara->coil_addr, pPara->coil_count);
}

void MB_f10(STDLIBFUNCALL)
{
	MB_F10_PARAM OS_SPTR *pPara = (MB_F10_PARAM OS_SPTR *)pIN;
	MB_DATA_ARRAY_PARAM OS_DPTR *pData = (MB_DATA_ARRAY_PARAM OS_DPTR* )pPara->data;

	/* the registers value to set must be stored into MODBUSdata array */
	memcpy(&MODBUSdata[0], pData->pElem, APP_MBRTU_DATA);
	pPara->ret_value = fn_10_preset_multiple_registers(pPara->slave, pPara->start_addr, pPara->count);
}

void MB_f11(STDLIBFUNCALL)
{
	MB_F11_PARAM OS_SPTR *pPara = (MB_F11_PARAM OS_SPTR *)pIN;

	pPara->ret_value = fn_11_report_slave_id(pPara->slave);
}

void MB_f14(STDLIBFUNCALL)
{
	MB_F14_PARAM OS_SPTR *pPara = (MB_F14_PARAM OS_SPTR *)pIN;
	MB_DATA_ARRAY_PARAM OS_DPTR *pData = (MB_DATA_ARRAY_PARAM OS_DPTR* )pPara->data;

	memcpy(&MODBUSdata[0], pData->pElem, APP_MBRTU_DATA);
	pPara->ret_value = fn_14_read_general_reference(pPara->slave);
}

void MB_f15(STDLIBFUNCALL)
{
	MB_F15_PARAM OS_SPTR *pPara = (MB_F15_PARAM OS_SPTR *)pIN;
	MB_DATA_ARRAY_PARAM OS_DPTR *pData = (MB_DATA_ARRAY_PARAM OS_DPTR* )pPara->data;

	memcpy(&MODBUSdata[0], pData->pElem, APP_MBRTU_DATA);
	pPara->ret_value = fn_15_write_general_reference(pPara->slave, pPara->count);
}

void MB_f16(STDLIBFUNCALL)
{
	MB_F16_PARAM OS_SPTR *pPara = (MB_F16_PARAM OS_SPTR *)pIN;
	MB_DATA_ARRAY_PARAM OS_DPTR *pData = (MB_DATA_ARRAY_PARAM OS_DPTR* )pPara->data;

	memcpy(&MODBUSdata[0], pData->pElem, 2);
	pPara->ret_value = fn_16_mask_write_registers(pPara->slave, pPara->start_addr);
}

void MB_f17(STDLIBFUNCALL)
{
	MB_F17_PARAM OS_SPTR *pPara = (MB_F17_PARAM OS_SPTR *)pIN;
	MB_DATA_ARRAY_PARAM OS_DPTR *pData = (MB_DATA_ARRAY_PARAM OS_DPTR* )pPara->data;

	memcpy(&MODBUSdata[0], pData->pElem, APP_MBRTU_DATA);
	pPara->ret_value = fn_17_read_write_registers(pPara->slave, pPara->start_addr, pPara->count, pPara->reg_addr, pPara->reg_count, pPara->byteorder);
}

void MB_f18(STDLIBFUNCALL)
{
	MB_F18_PARAM OS_SPTR *pPara = (MB_F18_PARAM OS_SPTR *)pIN;

	pPara->ret_value = fn_18_read_fifo_queue(pPara->slave, pPara->start_addr);
}

void MB_data(STDLIBFUNCALL)
{
	MB_DATA_PARAM OS_SPTR *pPara = (MB_DATA_PARAM OS_SPTR *)pIN;
	MB_DATA_ARRAY_PARAM OS_DPTR *pData = (MB_DATA_ARRAY_PARAM OS_DPTR* )pPara->data;

	memcpy(pData->pElem, &MODBUSdata[0], APP_MBRTU_DATA);
#ifdef NMODBUS_DEBUG
	{
		int i;
		printf("[%s]: MODBUSdata = ", __func__);
		for (i = 0; i < APP_MBRTU_DATA; i++)
		{
			printf("[%d:%2x]", i, MODBUSdata[i]);
		}
	}
	printf("\n");
#endif
	pPara->ret_value = 1;
}

void MB_get_status(STDLIBFUNCALL)
{
	MB_GET_STATUS_PARAM OS_SPTR *pPara = (MB_GET_STATUS_PARAM OS_SPTR *)pIN;
	MB_GET_STATUS_ARRAY_PARAM OS_DPTR *pStatus = (MB_GET_STATUS_ARRAY_PARAM OS_DPTR* )pPara->status;

	/* For some strange reason, I lost the signed information with the memcopy... */
#if 0
	memcpy( MODBUSstatus, app_mbrtu_status, APP_MBRTU_STATUS);
	memcpy(pStatus->pElem, MODBUSstatus, APP_MBRTU_STATUS);
#endif
	{
		int i;
#ifdef MODBUS_DEBUG
		printf("[%s]: MODBUSstatus = ", __func__);
#endif
		for (i = 0; i < APP_MBRTU_STATUS; i++)
		{
			MODBUSstatus[i] = app_mbrtu_status[i];
			*((short *)(pStatus->pElem + i)) = app_mbrtu_status[i];
#ifdef MODBUS_DEBUG
			printf("[%d:%2x]", i, MODBUSstatus[i]);
#endif
		}
	}
#ifdef MODBUS_DEBUG
	printf("\n");
#endif
	pPara->ret_value = 1;
}

