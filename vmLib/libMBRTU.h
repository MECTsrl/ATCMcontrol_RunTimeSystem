
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
 * Filename: libMBRTU.h
 */

#ifndef __APP_MBRTU__
#define __APP_MBRTU__

#include <termios.h>


#define RTS_PRAGMA_PACK_1
#include "osAlign.h"
#undef RTS_PRAGMA_PACK_1





/*Communication errors*/
#define PORT_FAILURE   -1
#define INTERNAL_ERROR -1
#define TIMEOUT        -2
#define INVALID_FRAME  -3
#define MODBUS_ERROR   -4


/* Error codes defined in modbus specification for exception frames */
#define ILLEGAL_FUNCTION -1
#define ILLEGAL_DATA_ADDRESS -2
#define ILLEGAL_DATA_VALUE -3
#define SLAVE_DEVICE_FAILURE -4
#define ACKNOWLEDGE -5
#define SLAVE_DEVICE_BUSY -6
#define NEGATIVE_ACKNOWLEDGE -7
#define MEMORY_PARITY_ERROR -8

/*PLC interface buffer dimension*/
#define APP_MBRTU_STATUS	3
#define APP_MBRTU_DATA		257

/*****************************************/
/**                                     **/
/** Linear Buffer (lb) Data Structure   **/
/**                                     **/
/*****************************************/

/*
 * The data structure stores the current data linearly in a single memory array,
 * i.e. the current data is stored from start to finish from a low address
 * to a high address, and does *not* circle back to the bottom of the address
 * space as is usual in a circular buffer. This allows the user/caller to
 * pass the structure's own byte array on to other functions such as
 * read() and write() for file operations.
*/

typedef struct {
        unsigned char *data;
        int data_size;      /* size of the *data buffer                   */
        int data_start;     /* offset within *data were valid data starts */
        int data_end;       /* offset within *data were valid data ends   */
        int max_data_start; /* optimization parameter! When should it be normalised? */
        } app_mbrtu_lb_buf_t;



/*****************************************/
/**                                     **/
/** Receiver Buffer  Data Structure     **/
/**                                     **/
/*****************************************/

/* A data structutre used for the receive buffer, i.e. the buffer
 * that stores the bytes we receive from the bus.
 *
 * What we really needed here is an unbounded buffer. This may be
 * implemented by:
 *   - a circular buffer the size of the maximum frame length
 *   - a linear buffer somewhat larger than the maximum frame length
 *
 * Due to the fact that this library's API hands over the frame data
 * in a linear buffer, and also reads the data (i,e, calls to read())
 * into a linear buffer:
 *  - the circular buffer would be more efficient in aborted frame
 *    situations
 *  - the linear is more efficient when no aborted frames are recieved.
 *
 * I have decided to optimize for the most often encountered situation,
 * i.e. when no aborted frames are received.
 *
 * The linear buffer has a size larger than the maximum
 * number of bytes we intend to store in it. We simply start ignoring
 * the first bytes in the buffer in which we are not interested in, and
 * continue with the extra bytes of the buffer. When we reach the limit
 * of these extra bytes, we shift the data down so it once again
 * uses the first bytes of the buffer. The more number of extra bytes,
 * the more efficient it will be.
 *
 * Note that if we don't receive any aborted frames, it will work as a
 * simple linear buffer, and no memory shifts will be required!
 */

typedef struct {
        app_mbrtu_lb_buf_t data_buf;
        int found_frame_boundary; /* Flag: *  1 => We have detected a frame boundary using 3.5 character silence *  0 => We have not yet detected any frame boundary  */        
        int frame_search_history;  /* Flag:  Used in the call to search_for_frame() as the history parameter!*/
        } app_mbrtu_recv_buf_t;





/*****************************************/
/**                                     **/
/**  Modbus Master node Data Structure  **/
/**                                     **/
/*****************************************/

typedef struct {
        int fd; 			   /* The file descriptor associated with the master node NOTE: if the node is not yet in use, i.e. if the node is free,then fd will be set to -1 */         
        struct timeval time_15_char;	   /* the time it takes to transmit 1.5 characters at the current baud rate */        
        struct timeval time_35_char;	   /* the time it takes to transmit 3.5 characters at the current baud rate */
        app_mbrtu_recv_buf_t recv_buf;     /* Due to the algorithm used to work around aborted frames, the modbus_read() function might read beyond the current modbus frame. The extra bytes must be stored for the subsequent call to modbus_read().*/
        struct termios old_tty_settings;   /* The old settings of the serial port, to be reset when the library is closed... */
	char device[15];
        int baud;       		   /* plain baud rate, eg 2400; zero for the default 9600 */
        int parity;                        /* 0 for none, 1 for odd, 2 for even                   */
        int data_bits;
        int stop_bits;	
	int ignore_echo;
          				/* ignore echo flag.
           				* If set to 1, then it means that we will be reading every byte we
           				* ourselves write out to the bus, so we must ignore those bytes read
           				* before we really read the data sent by remote nodes.
           				* This comes in useful when using a RS232-RS485 converter that does
           				* not correctly control the RTS-CTS lines...
           				*/
         } app_mbrtu_master_node_t;


/* Global Variables*/

app_mbrtu_master_node_t *mbrtu_master;
//extern signed short app_mbrtu_status[APP_MBRTU_STATUS];



/* Public Functions */


/*****************************************/
/**                                     **/
/**  Modbus Master node Init/Shutdown   **/
/**                                     **/
/*****************************************/

 /* init the library for modbus master */

int app_mbrtu_init_cfg(int confbaud, int confdatabits, int confparity, int confstopbits, int ignore_echo);
int app_mbrtu_init( void );

 /* shutdown the library for modbus master*/
int app_mbrtu_done(void);


/* Open a node for master operation.
 * Returns the node descriptor, or -1 on error.
 */
int app_mbrtu_connect(app_mbrtu_master_node_t *node);



/*****************************************/
/**                                     **/
/**  Modbus Master Functions            **/
/**                                     **/
/*****************************************/


/***********************************************************************

	 Note: All functions used for sending or receiving data via
	       app_mbrtu return these return values.


	Returns:	string_length if OK
			-1 on internal error or port failure
			-2 on timeout
			-3 if a valid yet un-expected frame is received!
			-4 for modbus exception errors
			   (in this case exception code is returned in *error_code)
			-5 invalid request 
			-6 broadcast request not supported  

***********************************************************************/

/* Reads the ON/OFF status of discrete outputs (0X references, coils)  in the slave. Function DOES NOT support broadcast.*/
unsigned short int fn_01_read_coil_status(unsigned short int slave, unsigned short int start_addr, unsigned short int count);
/*Reads the ON/OFF status of discrete inputs (1X references) in the slave. Function DOES NOT support broadcast*/
unsigned short int fn_02_read_input_status(unsigned short int slave, unsigned short int start_addr, unsigned short int count);
/* Read the holding registers in a slave and put the data into an array. Function DOES NOT support broadcast. */
unsigned short int fn_03_read_holding_register(unsigned short int slave, unsigned short int start_addr, unsigned short int count, unsigned char byteorder);
/* Read the inputg registers in a slave and put the data into an array.  Function DOES NOT support broadcast. */
unsigned short int fn_04_read_input_registers(unsigned short int slave, unsigned short int start_addr, unsigned short int count, unsigned char byteorder);
/*  Forces a single coil (0X reference) to either ON or OFF. When broadcast, the function forces the same coil reference in all attached slaves*/
unsigned short int fn_05_force_single_coil(unsigned short int slave, unsigned short int coil_addr, unsigned short int coil_state);
/* Sets a value in one holding register in the slave device. Function DOES support broadcast. */
unsigned short int fn_06_preset_single_register(unsigned short int slave, unsigned short int reg_addr);
/* Reads the contents of eight Exception Status coils within the slave controller. */
unsigned short int fn_07_read_exception_status(unsigned short int slave);
/* Returns a status word and an event count from the slave's communications event counter.*/
unsigned short int fn_0b_read_event_counter(unsigned short int slave);
/* Returns a status word, event count, message count, and a field of event bytes from the slave. */
unsigned short int fn_0c_read_event_log(unsigned short int slave);
/* Forces each coil (0X reference) in a sequence of coils to either ON or OFF.*/
unsigned short int fn_0f_force_multiple_coil(unsigned short int slave, unsigned short int coil_addr, unsigned short int coil_count);
/* Presets values into a sequence of holding registers (4X references). */
unsigned short int fn_10_preset_multiple_registers(unsigned short int slave, unsigned short int start_addr, unsigned short int count);
/* Returns a description of the type of controller present at the slave address */
unsigned short int fn_11_report_slave_id(unsigned short int slave);
/* Returns the contents of registers in Extended Memory file (6XXXXX) references.*/
unsigned short int fn_14_read_general_reference(unsigned short int slave );
/*Writes the contents of registers in Extended Memory file (6XXXXX) references.*/
unsigned short int fn_15_write_general_reference(unsigned short int slave, unsigned short int byte_count );
/*Modifies the contents of a specified 4XXXX register using a combination of an AND mask, an OR mask */
unsigned short int fn_16_mask_write_registers(unsigned short int slave, unsigned short int start_addr);
/*Read/Write 4X Registers*/
unsigned short int fn_17_read_write_registers(unsigned short int slave, unsigned short int start_addr, unsigned short int count, unsigned short int reg_addr,unsigned short int reg_count, unsigned char byteorder);
/*Reads the contents of a First In First Out queue of 4XXXX registers*/
unsigned short int fn_18_read_fifo_queue(unsigned short int slave, unsigned short int start_addr);
/*Modbus function 08 provides a series of tests for checking the communication system between the master and slave*/
unsigned short int fn_08_diagnostics(unsigned short int slave, unsigned short int subfunction );


/* Invert response byte order in case slave answer is lsb first*/
unsigned short int fn_response_byteorder_invert(unsigned short int count);
/* PLC view*/

unsigned short int MB_byteorder(unsigned short int count);

short MODBUSstatus[APP_MBRTU_STATUS];
unsigned short MODBUSdata[APP_MBRTU_DATA]; /* 256 data + 1 for count of valid data */

/*FarosPLC Function view for Libraries*/
/* --- 200 -------------------------------------------------------------------- */
void MB_f01(STDLIBFUNCALL);
/* --- 201 -------------------------------------------------------------------- */
void MB_f02(STDLIBFUNCALL);
/* --- 202 -------------------------------------------------------------------- */
void MB_f03(STDLIBFUNCALL);
/* --- 203 -------------------------------------------------------------------- */
void MB_f04(STDLIBFUNCALL);
/* --- 204 -------------------------------------------------------------------- */
void MB_f05(STDLIBFUNCALL);
/* --- 205 -------------------------------------------------------------------- */
void MB_f06(STDLIBFUNCALL);
/* --- 206 -------------------------------------------------------------------- */
void MB_f07(STDLIBFUNCALL);
/* --- 207 -------------------------------------------------------------------- */
void MB_f08(STDLIBFUNCALL);
/* --- 208 -------------------------------------------------------------------- */
void MB_f0b(STDLIBFUNCALL);
/* --- 209 -------------------------------------------------------------------- */
void MB_f0c(STDLIBFUNCALL);
/* --- 210 -------------------------------------------------------------------- */
void MB_f0f(STDLIBFUNCALL);
/* --- 211 -------------------------------------------------------------------- */
void MB_f10(STDLIBFUNCALL);
/* --- 212 -------------------------------------------------------------------- */
void MB_f11(STDLIBFUNCALL);
/* --- 213 -------------------------------------------------------------------- */
void MB_f14(STDLIBFUNCALL);
/* --- 214 -------------------------------------------------------------------- */
void MB_f15(STDLIBFUNCALL);
/* --- 215 -------------------------------------------------------------------- */
void MB_f16(STDLIBFUNCALL);
/* --- 216 -------------------------------------------------------------------- */
void MB_f17(STDLIBFUNCALL);
/* --- 217 -------------------------------------------------------------------- */
void MB_f18(STDLIBFUNCALL);
/* --- 219 -------------------------------------------------------------------- */
void MB_data(STDLIBFUNCALL);
/* --- 218 -------------------------------------------------------------------- */
void MB_get_status(STDLIBFUNCALL);

/* FarosPLC Structures with parameters to be sent to the above defined functions*/

/* strutture per passaggio array*/
typedef struct
{
	DEC_VAR(IEC_UINT, uLen);
	DEC_VAR(IEC_UINT, pElem[APP_MBRTU_DATA]);
	
}MB_DATA_ARRAY_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(start_addr);
	DEC_FUN_UINT(count);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F01_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(start_addr);
	DEC_FUN_UINT(count);
	/*return value of the MB_f02 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F02_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(start_addr);
	DEC_FUN_UINT(count);
	DEC_FUN_CHAR(byteorder);
	/*return value of the MB_f03 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F03_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(start_addr);
	DEC_FUN_UINT(count);
	DEC_FUN_CHAR(byteorder);
	/*return value of the MB_f04 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F04_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(coil_addr);
	DEC_FUN_UINT(coil_state);
	/*return value of the MB_f05 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F05_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(reg_addr);
	DEC_FUN_UINT(value);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F06_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F07_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(subfunction);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F08_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F0b_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F0c_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(coil_addr);
	DEC_FUN_UINT(coil_count);
	DEC_FUN_PTR(MB_DATA_ARRAY_PARAM, data);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F0f_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(start_addr);
	DEC_FUN_UINT(count);
	DEC_FUN_PTR(MB_DATA_ARRAY_PARAM, data);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F10_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F11_PARAM;

typedef struct
{
        DEC_FUN_UINT(slave);
        DEC_FUN_PTR(MB_DATA_ARRAY_PARAM, data);
        /*return value of the MB_f01 function*/
        DEC_FUN_UINT(ret_value);

}MB_F14_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(count);
	DEC_FUN_PTR(MB_DATA_ARRAY_PARAM, data);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F15_PARAM;

typedef struct
{
	DEC_VAR(IEC_UINT, uLen);
	DEC_VAR(IEC_UINT, pElem[2]);
	
}MB_DATA_ARRAY_PARAM_LEN_2;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(start_addr);
	DEC_FUN_PTR(MB_DATA_ARRAY_PARAM_LEN_2, data);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F16_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(start_addr);
	DEC_FUN_UINT(count);
	DEC_FUN_UINT(reg_addr);
	DEC_FUN_UINT(reg_count);
	DEC_FUN_CHAR(byteorder);
	DEC_FUN_PTR(MB_DATA_ARRAY_PARAM, data);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F17_PARAM;

typedef struct
{
	DEC_FUN_UINT(slave);
	DEC_FUN_UINT(start_addr);
	/*return value of the MB_f01 function*/
	DEC_FUN_UINT(ret_value);
	
}MB_F18_PARAM;

typedef struct
{
	DEC_FUN_PTR(MB_DATA_ARRAY_PARAM, data);
	/*return value*/
	DEC_FUN_UINT(ret_value);
	
}MB_DATA_PARAM;

/**/

typedef struct
{
	DEC_VAR(IEC_UINT, uLen);
	DEC_VAR(IEC_INT, pElem[APP_MBRTU_STATUS]);
	
}MB_GET_STATUS_ARRAY_PARAM;

typedef struct
{
	DEC_FUN_PTR(MB_GET_STATUS_ARRAY_PARAM, status);
	/*return value*/
	DEC_FUN_UINT(ret_value);
	
}MB_GET_STATUS_PARAM;

#endif
