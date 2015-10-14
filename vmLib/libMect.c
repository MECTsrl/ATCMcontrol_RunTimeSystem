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
 * Filename: libMect.c
 */

#define MECT_DEBUG   1

#include <assert.h>
#include <errno.h>
#include <pthread.h>   
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <unistd.h>
#include <time.h> 
#include "privUtyMect.h"
#include "mectCfgUtil.h"
#include "libMect.h"           /* Declare exported stuff */
/*
 * Global variables
 */

unsigned char app_mect_flag;
short app_mect_status;
float app_mect_enquiry;
serial_cfg_s mect_cfg;

/*
 * Local type definitions
 */

typedef struct data_s {
	int type;                   /* Data type */
	union data_u {
		int i;                  /* Integer value */
		unsigned int u;         /* Unsigned integer value */
		float r;                /* Real value */
	} value;
} data_t;

/*
 * Local constants
 */

/* Special IO tokens */
#define STX             0x2
#define ETX             0x3
#define EOT             0x4
#define ENQ             0x5
#define ACK             0x6
#define NACK            0x15

#define RXTIMEOUT       400     /* Character read timeout (ms) */
#define TXTIMEOUT       400000  /* Character write timeout (us) */
#define SOMTO           1       /* Char. timeout until first char. is seen */

#define MAX_LINE_SIZE   81
#define MSG_MAX_LEN     20

enum app_mect_data_e {
	APP_MECT_DATA_NONE = 0,
	APP_MECT_DATA_INT,
	APP_MECT_DATA_HEX,
	APP_MECT_DATA_REAL
};

/*
 * Local variables
 */

static int fdsio = 0;                   /* Serial I/O file descriptor */
static struct msg_s {
	int n;                              /* First free position in the message */
	char m[MSG_MAX_LEN];                         /* Message */
} msg;                                  /* Current IO message */
static struct sent_s {
	char cmd[2];                        /* Last command we sent to the slave */
} sent;



/* Thread structure and pointer*/
struct app_mect_params_s {
	short id;
	char *command;
	int dd;
	float value;
	int hex;
};



struct app_mect_params_s app_mect_params;
pthread_t app_mect_thread_id;
pthread_attr_t app_mect_attr;
struct sched_param app_mect_sched_param;

/*
 * Local function prototypes
 */

static short app_mect_msg_read(void);
static int app_mect_msg_write(void);
static int app_mect_tx_bcc_compute(void);
static int app_mect_rx_bcc_check(void);
static int app_mect_rx_msg_check(int dd);
static int app_mect_rx_nack_is(void);
static int app_mect_rx_ack_is(void);
static short app_mect_message_build(short id, char *command, data_t *data, int dd);
static short app_mect_stx(short id, char *command, float value, int dd, int hex);
static void app_mect_thread_attr_setup(void);
static void * app_mect_enq_manager( void *param );
static void * app_mect_stx_manager( void *param );
#if MECT_DEBUG
static void app_mect_msg_print(void);
#endif

/**
 * Initialize the package
 *
 * @return              0 - success
 *                      1 - error
 *
 * @ingroup mect
 */
int
app_mect_init(void)
{
#if MECT_DEBUG
	printf ("[%s] - Serial init\n", __func__);
#endif

	if (app_config_load(APP_CONF_MECT))
	{
		fprintf(stderr, "[%s]: Error Mect module configuration file is wrong: abort initialization.\n", __func__);
	}

	if (mect_cfg.enabled == 0)
	{
		fprintf(stderr, "[%s]: Warning Mect module is build but is not used: abort initialization.\n", __func__);
		return 0;
	}

	fdsio = app_s_serial_init(&mect_cfg);
	if (fdsio < 0) {
		perror("app_mect_setup: error opening serial port");

		return 1;
	}

	msg.n = 0;

	app_mect_enquiry = 0.0;

#if MECT_DEBUG
	printf ("[%s] - done\n", __func__);
#endif

	return 0;
}

int
app_mect_done(void)
{
	if (fdsio)
	{
		app_s_serial_close(fdsio);
		fdsio = 0;
	}
	return 0;
}

/**
 * Read next full message starting with the given starting char
 * or timeout.  In case of timeout the contents and length of
 * the message are undefined.
 *
 * @return              0 - success
 *                      1 - BCC error
 *                      2 - timeout
 *                      3 - input error
 *                      4 - timing error
 *
 * @ingroup mect
 */
static short
app_mect_msg_read(void)
{
	struct timespec start;
	struct timespec now;
	int got_som = 0;
	int got_eom = 0;

	assert(fdsio >= 0);

	if (clock_gettime(CLOCK_REALTIME, &start) < 0)
		return 1;

	/*memcpy(&now, &start, sizeof(struct timespec));*/
	now.tv_nsec = start.tv_nsec;

#if MECT_DEBUG
	printf("%s: reading %d\n", __func__, __LINE__);
#endif

	for (msg.n = 0, got_som = 0, got_eom = 0; ; ) {
		char ch = '\0';
		ssize_t read_code = 0;

		if (clock_gettime(CLOCK_REALTIME, &now) < 0)
			return 2;

		if (now.tv_nsec < start.tv_nsec)                /* Timer wrapped */
			return 5;

		if (now.tv_nsec - start.tv_nsec > RXTIMEOUT * 100000)    /* RX timeout */
		{
#if MECT_DEBUG
			printf("%s: timeout %d\n", __func__, __LINE__);
#endif
			return 2;
		}

		/* Non blocking read */
		read_code = read(fdsio, (void *)&ch, 1);

		if (read_code == -1) {          /* Read error */
			printf ("[%s] - while waiting for serial line input [%s]\n", __func__, strerror(errno));
			return 3;
		}

		if (read_code == 0) {           /* Nothing new */
			/* verificare la correttezza dell'uso di queso sleep: e' stato usato al posto dello sleep sincrono con il timer del fisico */  
			usleep(1000);
#if MECT_DEBUG
			printf ("[%s] - waiting response\n", __func__);
#endif

			continue;
		}

		/* Here we should have read a valid character */

		assert(read_code == 1);

		if (!got_som) {
			if ((ch == ACK) || (ch == NACK)) {
				/* ACK/NACK byte is the whole message */
				got_som = 1;
				got_eom = 1;
			}
			else
				got_som |= (ch == STX);

			if (!got_som) {
#if MECT_DEBUG
				printf("%s: discarding 0x%02x\n", __func__, ch);
				fflush(stdout);
#endif
				continue;           /* Do not start registering unless SOM was seen */
			}
		}

		msg.m[msg.n++] = ch;
		assert(msg.n < MSG_MAX_LEN);

		if (got_eom)
			break;                  /* ETX end of message was seen, done */

		got_eom = (ch == ETX);      /* Expect next char be the BCC and end the message */
	}

#if MECT_DEBUG
	printf("%s: done reading: ", __func__);
	app_mect_msg_print();
#endif

	return app_mect_rx_bcc_check();
}

/**
 * Send out the current message
 *
 * @return              0 - success
 *                      1 - failure
 *
 * @ingroup mect
 */
static int
app_mect_msg_write(void)
{
	//int i = 0;

#if MECT_DEBUG
	printf("%s: fdsio:'%d'\n", __func__, fdsio);
#endif
	assert(fdsio >= 0);

#if MECT_DEBUG
	printf("%s: writing:\n", __func__);
#endif
	//    for (i = 0; i < msg.n; i++) {
	fd_set wfds;                    /* File descriptors to monitor for events */
	struct timeval tv;              /* Timeout for file descriptor monitor */
	int ready = 0;
	ssize_t write_code = 0;

	/* Add the read file descriptor to the monitored set */
	FD_ZERO(&wfds);
	FD_SET(fdsio, &wfds);

	/* Set the write timeout */
	tv.tv_sec = 0;
	tv.tv_usec = TXTIMEOUT;

	ready = select(fdsio + 1, NULL, &wfds, NULL, &tv);

	if (ready == -1) {              /* Error */
		perror("app_mect_msg_write: while waiting for serial line output");

		return 1;
	}

	if (ready == 0)                 /* Timeout */
		return 2;

	assert(FD_ISSET(fdsio, &wfds));

	/* Here we've got the green light to send a char. */
#if MECT_DEBUG
	printf("%s:\n", __func__);
	app_mect_msg_print();
#endif
	write_code = write(fdsio, (void *)&(msg.m[0]), msg.n);
	if (write_code < 0) {
		perror("app_mect_msg_write: error writing output stream");

		return 1;
	}
	else
	{
		assert(write_code == msg.n);        /* assert(write_code == 1);  Read exactly one byte at a time */
	}
	//    }
#if MECT_DEBUG
	printf ("[%s] done.\n", __func__);
	fflush(stdout);
#endif

	return 0;
}

/**
 * Compute the BCC for the current message.  The message should
 * not have the BCC attached.
 *
 * @return              the bcc
 *
 * @ingroup mect
 */
static int
app_mect_tx_bcc_compute(void)
{
	int bcc = 0;
	int i = 0;

	/* Start one byte after STX */
	for (i = 7, bcc = msg.m[6]; i < msg.n; i++)
		bcc ^= msg.m[i];

	return bcc;
}

/**
 * Check the BCC of the RX message
 *
 * @return              0 - BCC checks
 *                      1 - BCC check failed
 *
 * @ingroup mect
 */
static int
app_mect_rx_bcc_check(void)
{
	int bcc = 0;
	int i = 0;

	assert(msg.n < MSG_MAX_LEN);
#if MECT_DEBUG
	fprintf(stderr,"%s: msg.n= %d\n",__func__, msg.n);

#endif    

	if ((msg.n == 1) && ((msg.m[0] == ACK) || (msg.m[0] == NACK)))
		return 0;
	else if (msg.n != 13 && msg.n != 11)
		return 1;

	/* Skip leading STX and stop before the RX message BCC */
	for (i = 2, bcc = msg.m[1]; i < msg.n - 1; i++)
		bcc ^= msg.m[i];

#if MECT_DEBUG
	fprintf(stderr,"%s:\n",__func__);
	app_mect_msg_print();
	fprintf(stderr,"bcc_check %02x, bcc_recv %02x \n",bcc, msg.m[msg.n - 1]);
#endif	

	/* Check against the BCC of the RX message */
	return bcc != msg.m[msg.n - 1];
}

/**
 * Check the validity of the reply message with the given
 * data bytes
 *
 * @param dd            data digits (6 or 8 digit protocol)
 *
 * @return              0 - a valid reply
 *                      1 - not a valid reply
 *
 * @ingroup mect
 */
static int
app_mect_rx_msg_check(int dd)
{
	if ((dd <= 0) || (dd >= 10))
		return 1;

	if (msg.n == 1)
		return !((msg.m[0] == ACK) || (msg.m[0] == NACK));
	else if ((msg.n - dd) == 5)
		return strncmp(&(msg.m[1]), sent.cmd, 2) != 0;
	else
		return 1;               /* Not valid */
}

/**
 * Check whether the current message is a NACK
 *
 * @return              0 - message is a NACK
 *                      1 - message is not a NACK
 *
 * @ingroup mect
 */
static int
app_mect_rx_nack_is(void)
{
	return !((msg.n == 1) && (msg.m[0] == NACK));
}

/**
 * Check whether the current message is an ACK
 *
 * @return              0 - message is an ACK
 *                      1 - message is not an ACK
 *
 * @ingroup mect
 */
static int
app_mect_rx_ack_is(void)
{
	return !((msg.n == 1) && (msg.m[0] == ACK));
}

/*
 * RX:
 *   EOT GID GID UID UID C1  C2  ENQ
 *   EOT GID GID UID UID STX C1  C2  D1...D8 ETX BCC
 *   BCC = C1 ^ C2 ^ D1 ^ D2 ^ D3 ^ D4 ^ D5 ^ D6 ^ D7 ^ D8 ^ ETX
 *
 * TX:
 *   STX C1 C2 D1...D8 ETX BCC
 *   BCC = C1 ^ C2 ^ D1 ^ D2 ^ D3 ^ D4 ^ D5 ^ D6 ^ D7 ^ D8 ^ ETX
 */

/**
 * Build the message using the given slave id, command code,
 * and, optionally, data load
 *
 * @param id            slave ID
 * @param command       slave command to activate
 * @param data          integer/real data (optional)
 * @param dd            data digits (6 or 8 digit protocol)
 *
 * @return              0 - success
 *                      1 - ID error
 *                      2 - command code error
 *                      3 - integer value error
 *                      4 - hex value error
 *                      5 - real value error
 *                      6 - data type error
 *
 * @ingroup mect
 */
static short
app_mect_message_build(short id, char *command, data_t *data, int dd)
{
	char buffer[9];

	assert((dd > 0) && (dd < 10));

	msg.n = 0;                  /* Message reset */

	if (id > 99)
		return 1;               /* Invalid slave ID */
	if (command == NULL)
		return 2;               /* Invalid command code */
	if (strlen(command) != 2)
		return 2;               /* Invalid command code */

	msg.m[msg.n++] = EOT;       /* Message start */

	if (snprintf(buffer, 3, "%02d", id) != 2)
		return 1;               /* Slave ID conversion error */

	msg.m[msg.n++] = buffer[0]; /* Add the slave ID to the message */
	msg.m[msg.n++] = buffer[0];
	msg.m[msg.n++] = buffer[1];
	msg.m[msg.n++] = buffer[1];

	if (data == NULL) {
		msg.m[msg.n++] = command[0];
		msg.m[msg.n++] = command[1];

		msg.m[msg.n++] = ENQ;
	}
	else {
		char format[9];
		int i = 0;

		msg.m[msg.n++] = STX;

		msg.m[msg.n++] = command[0];
		msg.m[msg.n++] = command[1];

		assert(snprintf(format, 3, "%%%d", dd) == 2);
		if (data->type == APP_MECT_DATA_INT) {
			int code = 0;

			assert(snprintf(format, 4, "%%%dd", dd) == 3);

			code = snprintf(buffer, dd + 1, format, data->value.i);
			if ((code >= (dd + 1)) || (code < 0))
				return 3;       /* Conversion error */
		}
		else if (data->type == APP_MECT_DATA_HEX) {
			int code = 0;

			assert(snprintf(format, 9, "%%%dc>%%04X", dd - 5) == 8);

			code = snprintf(buffer, dd + 1, format, ' ', data->value.u);
			if ((code >= (dd + 1)) || (code < 0))
				return 3;       /* Conversion error */
		}
		else if (data->type == APP_MECT_DATA_REAL) {
			int code = 0;

			assert(snprintf(format, 4, "%%%dg", dd) == 3);

			code = snprintf(buffer, dd + 1, format, data->value.r);
			if ((code >= (dd + 1)) || (code < 0))
				return 4;       /* Conversion error */
		}
		else
			return 5;           /* Unknown data type */

		for (i = 0; i < dd; i++)
			msg.m[msg.n++] = buffer[i];

		msg.m[msg.n++] = ETX;

		msg.m[msg.n] = app_mect_tx_bcc_compute();
		msg.n++;                /* Avoid side effects with BCC computation function */
	}

	assert(msg.n < MSG_MAX_LEN);
	/* Store last command sent, to check the slave reply against */
	strncpy(sent.cmd, command, 2);

	return 0;
}

/**
 * Thread attribute  set up function
 *
 * @return              void
 *                      
 *
 * @ingroup mect
 */
static void 
app_mect_thread_attr_setup(void)
{
	pthread_attr_init(&app_mect_attr);
	pthread_attr_setdetachstate(&app_mect_attr, PTHREAD_CREATE_DETACHED);
#if 0
	pthread_attr_setschedpolicy(&app_mect_attr, SCHED_FIFO);
	app_mect_sched_param.sched_priority = 89;
#else
	pthread_attr_setschedpolicy(&app_mect_attr, SCHED_OTHER);
#endif
	pthread_attr_setinheritsched(&app_mect_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedparam(&app_mect_attr, &app_mect_sched_param);
	pthread_attr_setscope(&app_mect_attr, PTHREAD_SCOPE_SYSTEM);
}

/**
 * Perform a value enquire using the given slave id, command
 * code, and number of digits and return the read value in the
 * proper PLC memory bank.  In case of error the read value
 * is undefined.
 *
 * @param id            slave ID
 * @param command       slave command to activate
 * @param dd            data digits (protocol variants)
 *
 * @return              0  - success
 *                      1  - RX timeout
 *                      2  - RX error
 *                      3  - TX error
 *                      9  - data format error
 *                      11 - TX ID error
 *                      12 - TX command code error
 *                      13 - TX integer value error
 *                      14 - TX hex value error
 *                      15 - TX real value error
 *                      16 - TX data type error
 *                      21 - TX error
 *                      31 - RX BCC error
 *                      32 - RX timeout
 *                      33 - RX input error
 *                      34 - RX timing error
 *                      41 - reply format error
 *
 * @ingroup mect
 */
short
app_mect_enq(short id, char *command, int dd)
{
	if (!app_mect_flag){

		/* Set mect serial master to BUSY */
		app_mect_flag = 1;

#if MECT_DEBUG
		printf ("[%s] - enter - flag: %d\n", __func__, app_mect_flag);
#endif
		/* Set the communication parameters */
		app_mect_params.id = id;
		app_mect_params.command = command;
		app_mect_params.dd = dd;

#if MECT_DEBUG
		printf ("[%s] - prepare thread: id: %d, command: '%s', dd: %d\n", __func__, id, command, dd);
		printf ("[%s] - prepare thread: id: %d, command: '%s', dd: %d\n", __func__, app_mect_params.id , app_mect_params.command, app_mect_params.dd);
#endif
		/* Set thread parameters */
		app_mect_thread_attr_setup();

#if MECT_DEBUG
		printf ("[%s] - create thread\n", __func__);
#endif
		osPthreadCreate(&app_mect_thread_id, &app_mect_attr, &app_mect_enq_manager, &app_mect_params, "app_mect_enq_manager", 0);
		pthread_attr_destroy(&app_mect_attr);
#if MECT_DEBUG
		printf ("[%s] - thread done\n", __func__);
#endif
		return 0;
	}	

	return 1;
}

static void *
app_mect_enq_manager( void *param )
{
	short rv = 0;
	short id;
	char * command;
	int dd;

	struct app_mect_params_s *p = (struct app_mect_params_s *)param;
	id = p->id;
	command = p->command;
	dd = p->dd;


	if (id > 99){
		app_mect_status = 9;
		app_mect_flag = 0;
		return NULL;
	}	
	if (command == NULL){
		app_mect_status = 9;
		app_mect_flag = 0;
		return NULL;
	}	
#if MECT_DEBUG
	printf ("[%s] - '%s' [%d]\n", __func__, command , strlen(command));
#endif
	if (strlen(command) != 2){
		app_mect_status = 9;
		app_mect_flag = 0;
		return NULL;
	}	
#if MECT_DEBUG
	printf ("[%s] - dd: %d\n", __func__, dd);
#endif
	if ((dd <= 0) || (dd >= 10)){
		app_mect_status = 9;
		app_mect_flag = 0;
		return NULL;
	}

#if MECT_DEBUG
	printf ("[%s] - id: %d\n", __func__, id );
#endif
	rv = app_mect_message_build(id, command, NULL, dd);
	if (rv != 0){
		app_mect_status = rv + 10;
		app_mect_flag = 0;
		return NULL;
	}	

#if MECT_DEBUG
	printf ("[%s] - 1\n", __func__ );
#endif
	rv = app_mect_msg_write();
	if (rv != 0){
		app_mect_status = rv + 20;
		app_mect_flag = 0;
		return NULL;
	}	

#if MECT_DEBUG
	printf ("[%s] - 2\n", __func__ );
#endif
	rv = app_mect_msg_read();
	if (rv != 0){
		app_mect_status = rv + 30;
		app_mect_flag = 0;
		return NULL;
	}	

#if MECT_DEBUG
	printf ("[%s] - 3\n", __func__ );
#endif
	rv = app_mect_rx_msg_check(dd);
	if (rv != 0){
		app_mect_status = rv + 40;
		app_mect_flag = 0;
		return NULL;
	}	

	app_mect_enquiry = app_s_str_to_float(&(msg.m[3]));

	app_mect_status = 0;
	app_mect_flag = 0;
#if MECT_DEBUG
	printf ("[%s] - app_mect_enquiry %f app_mect_status %d app_mect_flag %d\n", __func__, app_mect_enquiry, app_mect_status, app_mect_flag );
#endif
	pthread_exit(0);
	return NULL;           /* All checks out */
}

/**
 * Perform an STX query using the given slave id, command code,
 * data, and data size (in bytes)
 *
 * @param id            slave ID
 * @param command       slave command to activate
 * @param data          real data
 * @param dd            data digits (protocol variants)
 * @param hex           data is hex
 *
 * @return              0   - ACK -- success
 *                      1   - NACK
 *                      9   - data format error
 *                      11  - TX ID error
 *                      12  - TX command code error
 *                      13  - TX integer value error
 *                      14  - TX hex value error
 *                      15  - TX real value error
 *                      16  - TX data type error
 *                      21  - TX error
 *                      31  - RX BCC error
 *                      32  - RX timeout
 *                      33  - RX input error
 *                      41  - reply format error
 *                      100 - unknown error
 *
 * @ingroup mect
 */
short
app_mect_stx(short id, char *command, float value, int dd, int hex)
{
	if (!app_mect_flag){

		/* Set mect serial master to BUSY */
		app_mect_flag = 1;

		/* Set the communication parameters */
		app_mect_params.id = id;
		app_mect_params.command = command;
		app_mect_params.dd = dd;
		app_mect_params.value = value;
		app_mect_params.hex = hex;

		/* Set thread parameters */
		app_mect_thread_attr_setup();

		osPthreadCreate(&app_mect_thread_id, &app_mect_attr, &app_mect_stx_manager, &app_mect_params, "app_mect_stx_manager", 0);
		return 0;
	}	

	return 1;

}

static void *
app_mect_stx_manager( void *param )
{

	data_t data;
	short rv = 0;

	short id;
	char *command;
	int dd;
	float value;
	int hex;

	struct app_mect_params_s *p = (struct app_mect_params_s *)param;
	id = p->id;
	command = p->command;
	dd = p->dd;
	value = p->value;
	hex = p->hex;

	if (id > 99){
		app_mect_status = 9;
		app_mect_flag = 0;    
		return NULL;
	}	
	if (command == NULL){
		app_mect_status = 9;
		app_mect_flag = 0;    
		return NULL;
	}	
	if (strlen(command) != 2){
		app_mect_status = 9;
		app_mect_flag = 0;    
		return NULL;
	}	
	if ((dd <= 0) || (dd >= 10)){
		app_mect_status = 9;
		app_mect_flag = 0;    
		return NULL;
	}	

	if (hex) {
		data.type = APP_MECT_DATA_HEX;
		data.value.u = (unsigned int)value;
	}
	else {
		data.type = APP_MECT_DATA_REAL;
		data.value.r = value;
	}

	rv = app_mect_message_build(id, command, &data, dd);
	if (rv != 0){
		app_mect_status = rv + 10;
		app_mect_flag = 0;    
		return NULL;    
	}

	rv = app_mect_msg_write();
	if (rv != 0){
		app_mect_status = rv + 20;
		app_mect_flag = 0;    
		return NULL;      
	}

	rv = app_mect_msg_read();
	if (rv != 0){
		app_mect_status = rv + 30;
		app_mect_flag = 0;    
		return NULL;      
	}

	rv = app_mect_rx_msg_check(dd);
	if (rv != 0){
		app_mect_status = rv + 40;
		app_mect_flag = 0;    
		return NULL;  	
	}

	if (app_mect_rx_nack_is() == 0){
		app_mect_status = 1;
		app_mect_flag = 0;    
		return NULL;      
	}

	if (app_mect_rx_ack_is() == 0){
		app_mect_status = 0;
		app_mect_flag = 0;    
		return NULL;      
	}

	app_mect_status = 100;
	app_mect_flag = 0;    
	return NULL;      

}

/**
 * Wrapper for app_mect_stx() for floating point values
 * 
 * @ingroup mect
 */
short
app_mect_float_stx(short id, char *command, float value, int dd)
{
#if MECT_DEBUG
	printf("%s: got: id: %d, cmd: %s, value: %g, dd: %d\n", __func__, id, command, value, dd);
	fflush(stdout);
#endif

	return app_mect_stx(id, command, value, dd, 0);
}

/**
 * Wrapper for app_mect_stx() for hex values
 * 
 * @ingroup mect
 */
short
app_mect_hex_stx(short id, char *command, unsigned short value, int dd)
{
#if MECT_DEBUG
	printf("%s: got: id: %d, cmd: %s, value: %g, dd: %d\n", __func__, id, command, value, dd);
	fflush(stdout);
#endif

	return app_mect_stx(id, command, value, dd, 1);
}

#if MECT_DEBUG
/**
 * Print to stdout the current message, for debug purposes
 */
static void
app_mect_msg_print(void)
{
	int i = 0;


	printf("%s current message:", __func__);
	fflush(stdout);
	for (i = 0; i < msg.n; i++) {
		printf(" 0x%02x[%c]", msg.m[i], msg.m[i]);
		fflush(stdout);
	}
	printf(" done.\n");
	fflush(stdout);
}

#endif

static	char dummy[8];
/****************************************************/
/****************************************************/
/*		FarosPLC Wrapper to access	    */	
/*		MECT functions		    */	
/****************************************************/
/****************************************************/
void
MECT_sread(STDLIBFUNCALL)
{
	MECT_ENQUIRY_PARAM OS_SPTR *pPara = (MECT_ENQUIRY_PARAM OS_SPTR *)pIN;
	MECT_COMMAND_ARRAY_PARAM OS_DPTR *pData = (MECT_COMMAND_ARRAY_PARAM OS_DPTR* )pPara->command;

        memcpy(dummy, pData->pElem, 2);
        dummy[2] = 0;
//	pData->pElem[2]='\0';
#if MECT_DEBUG
	printf ("[%s] - enter id %d command '%s' data %d\n", __func__,pPara->id, dummy, pPara->data);
#endif
	pPara->ret_value = app_mect_enq((short)pPara->id, (char*)dummy, (int)pPara->data);
#if MECT_DEBUG
	printf ("[%s] - done\n", __func__);
#endif
}

void
MECT_H_sread(STDLIBFUNCALL)
{
	MECT_ENQUIRY_PARAM OS_SPTR *pPara = (MECT_ENQUIRY_PARAM OS_SPTR *)pIN;
	MECT_COMMAND_ARRAY_PARAM OS_DPTR *pData = (MECT_COMMAND_ARRAY_PARAM OS_DPTR* )pPara->command;

	pData->pElem[2]='\0';
#if MECT_DEBUG
	printf ("[%s] - enter id %d command '%s' data %d\n", __func__,pPara->id, pData->pElem, pPara->data);
#endif
	pPara->ret_value = app_mect_enq((short)pPara->id, (char*)pData->pElem, (int)pPara->data);
#if MECT_DEBUG
	printf ("[%s] - done\n", __func__);
#endif
}

void
MECT_swrite(STDLIBFUNCALL)
{
	MECT_FLOAT_STX_PARAM OS_SPTR *pPara = (MECT_FLOAT_STX_PARAM OS_SPTR *)pIN;
	MECT_COMMAND_ARRAY_PARAM OS_DPTR *pData = (MECT_COMMAND_ARRAY_PARAM OS_DPTR* )pPara->command;

	pData->pElem[2]='\0';
#if MECT_DEBUG
	printf ("[%s] - enter id %d command '%s' value '%f' data %d\n", __func__,pPara->id, pData->pElem, pPara->value, pPara->data);
#endif
	pPara->ret_value = app_mect_float_stx((short)pPara->id, (char*)pData->pElem, (float)pPara->value, (int)pPara->data);
}

void
MECT_H_swrite(STDLIBFUNCALL)
{
	MECT_HEX_STX_PARAM OS_SPTR *pPara = (MECT_HEX_STX_PARAM OS_SPTR *)pIN;
	MECT_COMMAND_ARRAY_PARAM OS_DPTR *pData = (MECT_COMMAND_ARRAY_PARAM OS_DPTR* )pPara->command;
	short id=(int)(pPara->id);
	unsigned short value=(int)(pPara->value);
	int data=(int)(pPara->data);

        memcpy(dummy, pData->pElem, 2);
        dummy[2] = 0;
//	pData->pElem[2]='\0';
#if MECT_DEBUG
	printf ("[%s] - enter id %d command '%s' value '%d' data %d\n", __func__,pPara->id, dummy, pPara->value, pPara->data);
	printf ("[%s] - enter id %d command '%s' value '%d' data %d\n", __func__,(short)pPara->id, (char*)pData->pElem, (unsigned short)pPara->value, (int)pPara->data);
	printf ("[%s] - enter id %d command '%s' value '%g' data %d\n", __func__,(short)pPara->id, (char*)pData->pElem, (unsigned short)pPara->value, (int)pPara->data);
	printf ("[%s] - enter id %d command '%s' value '%g' data %d\n", __func__,id, (char*)pData->pElem, value, data);
#endif
	//pPara->ret_value = app_mect_hex_stx((short)pPara->id, (char*)pData->pElem, (unsigned short)pPara->value, (int)pPara->data);
	pPara->ret_value = app_mect_hex_stx(id, (char*)dummy, value, data);
}

void
MECT_get_enquiry(STDLIBFUNCALL)
{

	MECT_GET_ENQ_PARAM OS_SPTR *pPara = (MECT_GET_ENQ_PARAM OS_SPTR *)pIN;
#if MECT_DEBUG
	printf ("[%s] - enter\n", __func__);
#endif
	if ( app_mect_status == 0 && app_mect_flag == 0 )
	{
		pPara->value = app_mect_enquiry;
	}
#if MECT_DEBUG
	printf ("[%s] - done [%f]\n", __func__, app_mect_enquiry);
#endif
}

void
MECT_get_status(STDLIBFUNCALL)
{
	MECT_GET_STATUS OS_SPTR *pPara = (MECT_GET_STATUS OS_SPTR *)pIN;
#if MECT_DEBUG
	printf ("[%s] - status: '%d'\n", __func__, app_mect_status);
#endif
	pPara->status = app_mect_status;
}

void
MECT_get_flag(STDLIBFUNCALL)
{
	MECT_GET_FLAG OS_SPTR *pPara = (MECT_GET_FLAG OS_SPTR *)pIN;
#if MECT_DEBUG
	printf ("[%s] - flag: '%d'\n", __func__,app_mect_flag);
#endif
	pPara->flag = app_mect_flag;
}
