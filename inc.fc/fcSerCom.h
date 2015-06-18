
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
 * Filename: fcSerCom.h
 */


#ifndef _FCSERCOM_H_
#define _FCSERCOM_H_

#if defined(FC_CFG_SERIALCOMM_LIB)

#include <termios.h>


/* --- 000 -------------------------------------------------------------------- */
void fb_emb_ser_connect(STDLIBFBCALL);
/* --- 001 -------------------------------------------------------------------- */
void fb_emb_ser_read(STDLIBFBCALL);
/* --- 002 -------------------------------------------------------------------- */
void fb_emb_ser_write(STDLIBFBCALL);
/* --- 009 -------------------------------------------------------------------- */
void fb_emb_ser_array_read(STDLIBFBCALL);
/* --- 010 -------------------------------------------------------------------- */
void fb_emb_ser_array_write(STDLIBFBCALL);
/* --- 011 -------------------------------------------------------------------- */
void fb_emb_ser_break_mark(STDLIBFBCALL);


/* type identifiers for communication FBs 
 */
#define CFB_EMB_Serial					0

/* Status codes as defined in IEC1131-5 Table 24 
 */
#define CFB_EMB_NoError 				0
#define CFB_EMB_ErrLowLayer 			1	 /* Error of lower layers, no communication possible */
#define CFB_EMB_ErrRegNegResponse		2	 /* Other negative response from remote communication partner */
#define CFB_EMB_ErrUnknownR_ID			3	 /* R_ID does not exist in the communication channel */
#define CFB_EMB_ErrTypeMismatch 		4	 /* Data type mismatch				*/
#define CFB_EMB_ErrResetReceived		5	 /* Reset received					*/
#define CFB_EMB_ErrReceiverNotEnabled	6	 /* Receiver not enabled			*/
#define CFB_EMB_ErrRemoteWrongState 	7	 /* Remote communication partner in wrong state */
#define CFB_EMB_ErrRemoteAccessDenied	8	 /* Access denied to remote object	*/
#define CFB_EMB_ErrReceiverOverrun		9	 /* Receiver overrun (user data are new) */
							  /* 10 .. 20		Reserved for future standardization */

/* FarosPLC implementor specific status/error codes:
 * < 0: 	error codes
 * > 20:	status codes
 */
#define CFB_EMB_ErrNotImpl					-1	/* FB not implemented			*/
#define CFB_EMB_ErrInvalidCommChannel		-2	/* Invalid COMM_CHANNEL 		*/
#define CFB_EMB_ErrTimeout					-3	/* Timout for read or write request */
#define CFB_EMB_ErrOutOfMemory				-4
#define CFB_EMB_ErrTransmitterNotEmpty      -5  /* transmitter not empty neither Break nor Mark possible */
#define CFB_EMB_ErrInvalidParameter         -6  /* Break or Mark time out off range */
#define CFB_EMB_ErrNotOpen                  -7  /* Channel not open */
						   
/* Error codes for serial communication 
 */
#define CFB_EMB_Ser_ErrInvalidPortNb	   -100 /* Invalid or unsupported COM	*/
#define CFB_EMB_Ser_ErrPortInUse		   -101 /* COM already in use			*/
#define CFB_EMB_Ser_ErrInvalidBaudRate	   -102 /* Invalid or unsupported baudrate */
#define CFB_EMB_Ser_ErrInvalidDatabits	   -103 /* Unsupported byte size		*/
#define CFB_EMB_Ser_ErrInvalidParity	   -104 /* Invlid parity				*/
#define CFB_EMB_Ser_ErrInvalidStopbits	   -105 /* Invalid stop bits			*/
#define CFB_EMB_Ser_ErrInvalidProtocol	   -106 /* Invalid protocol 			*/
#define CFB_EMB_Ser_ErrInvalidParameters   -107 /* Error in parameter settings	*/
		  
#define CFB_EMB_Ser_ErrReceiveBreak 	   -110 /* break condition detected 	*/
#define CFB_EMB_Ser_ErrReceiveFrame 	   -111 /* Framing error detected		*/
#define CFB_EMB_Ser_ErrReceiveOverrun	   -112 /* character-buffer overrun has occurred */
#define CFB_EMB_Ser_ErrReceiveParity	   -113 /* Parity error detected		*/
#define CFB_EMB_Ser_ErrWriteFull		   -114 /* The output buffer is full	*/

#define LOW 								0
#define HIGH_READY							1
#define HIGH_OP_PENDING 					2 

typedef struct EMB_COMM_CHANNEL_TAG
{
	DEC_VAR(IEC_DINT, hChannel);		/* channel handle						*/
	DEC_VAR(IEC_DINT, iCommType);		/* identifies type of communication; currently SERIAL only ! */
	DEC_VAR(IEC_DINT, iTimeout);		/* timeout for read and write requests; 
											if iTimeout = 0 no timeout is used	*/
} EMB_COMM_CHANNEL;

typedef struct
{
	DEC_VAR(IEC_DINT,		 iCommType);	/* Identifies type of communication; currently SERIAL only ! */
	DEC_VAR(IEC_DINT,		 iTimeout); 	/* Timeout for read and write requests; 
												if iTimeout = 0 no timeout is used */
	DEC_VAR(IEC_STRMAX, sParam);			/* Parameters for comm. specific initialization */
						  /* Serial comm. parameter string: 
							"PortNb,BaudRate,DataBits,Parity,StopBits,Protocol"
							 ProtNb:	 1,2,...
							 BaudRate:	 110,300,600,1200,2400,4800,9600,...
							 DataBits:	 4,5,6,7,8
							 Parity:	 N,O,E,M,S	 (for: no, odd, even, mark, space)
							 StopBits:	 1, 1.5, 2
							 Protocol:	 NO, SW, HW  (for: no, Xon/Xoff, RTS/CTS)
							 If the parameter string is empty a default parameter string is used: 
							 "1,9600,8,N,1,NO"	*/
} EMB_COMM_PARTNER;


/* --- 000 -------------------------------------------------------------------- */

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

typedef struct							/* Connection management				*/
{
	DEC_VAR(IEC_BOOL, en_c);			/* enable/startup connection			*/
	DEC_VAR(IEC_UINT, partner_off); 	/* identifies communication partner and */
	DEC_VAR(IEC_BOOL, valid_error); 	/* connection is valid/established		*/
	DEC_VAR(IEC_DINT, status);			/* last detected status 				*/
	DEC_VAR(IEC_UINT, id_off);			/* established communication channel	*/

}  EMB_SER_CONNECT_PAR;

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

typedef struct							/* Connection management				*/
{
	DEC_VAR(IEC_BOOL, en_c);			/* enable/startup connection			*/
	DEC_VAR(IEC_BYTE, dummy_08_en_c);
	DEC_VAR(IEC_UINT, partner_off); 	/* identifies communication partner and */
	DEC_VAR(IEC_BOOL, valid_error); 	/* connection is valid/established		*/
	DEC_VAR(IEC_BYTE, dummy_08_valid_error);
	DEC_VAR(IEC_DINT, status);			/* last detected status 				*/
	DEC_VAR(IEC_UINT, id_off);			/* established communication channel	*/

}  EMB_SER_CONNECT_PAR;

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

typedef struct							/* Connection management				*/
{
	DEC_VAR(IEC_BOOL, en_c);			/* enable/startup connection			*/
	DEC_VAR(IEC_BYTE, dummy_08_en_c);
	DEC_VAR(IEC_UINT, partner_off); 	/* identifies communication partner and */
	DEC_VAR(IEC_BOOL, valid_error); 	/* connection is valid/established		*/
	DEC_VAR(IEC_BYTE, dummy_08_valid_error);
	DEC_VAR(IEC_WORD, dummy_16_valid_error);
	DEC_VAR(IEC_DINT, status);			/* last detected status 				*/
	DEC_VAR(IEC_UINT, id_off);			/* established communication channel	*/

}  EMB_SER_CONNECT_PAR;

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */


/* --- 001 -------------------------------------------------------------------- */

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

typedef struct EMB_SER_READ_PAR_TAG 	/* Polled data acquisition				*/
{
	DEC_VAR(IEC_BOOL,	req);			/* Request								*/
	DEC_VAR(IEC_UINT,	id_off);		/* Communication channel				*/
	DEC_VAR(IEC_DINT,	rd_len_1);		/* Max. length of the data to be recd.	*/
	DEC_VAR(IEC_BOOL,	read_all__ndr_error);	
	DEC_VAR(IEC_DINT,	status);		/* Last detected status 				*/
	DEC_VAR(IEC_STRMAX, rd_1);			/* Received user data					*/
	DEC_VAR(IEC_BYTE,	prev_state);
	DEC_VAR(IEC_UDINT,	start_time);

} EMB_SER_READ_PAR;

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

typedef struct EMB_SER_READ_PAR_TAG 	/* Polled data acquisition				*/
{
	DEC_VAR(IEC_BOOL,	req);			/* Request								*/
	DEC_VAR(IEC_BYTE,	dummy_08_req);
	DEC_VAR(IEC_UINT,	id_off);		/* Communication channel				*/
	DEC_VAR(IEC_DINT,	rd_len_1);		/* Max. length of the data to be recd.	*/
	DEC_VAR(IEC_BOOL,	read_all__ndr_error);	
	DEC_VAR(IEC_BYTE,	dummy_08_all);
	DEC_VAR(IEC_DINT,	status);		/* Last detected status 				*/
	DEC_VAR(IEC_STRMAX, rd_1);			/* Received user data					*/
	DEC_VAR(IEC_BYTE,	prev_state);
	DEC_VAR(IEC_UDINT,	start_time);

} EMB_SER_READ_PAR;

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

typedef struct EMB_SER_READ_PAR_TAG 	/* Polled data acquisition				*/
{
	DEC_VAR(IEC_BOOL,	req);			/* Request								*/
	DEC_VAR(IEC_BYTE,	dummy_08_req);
	DEC_VAR(IEC_UINT,	id_off);		/* Communication channel				*/
	DEC_VAR(IEC_DINT,	rd_len_1);		/* Max. length of the data to be recd.	*/
	DEC_VAR(IEC_BOOL,	read_all__ndr_error);	
	DEC_VAR(IEC_BYTE,	dummy_08_all);
	DEC_VAR(IEC_WORD,	dummy_16_all);
	DEC_VAR(IEC_DINT,	status);		/* Last detected status 				*/
	DEC_VAR(IEC_STRMAX, rd_1);			/* Received user data					*/
	DEC_VAR(IEC_BYTE,	prev_state);
	DEC_VAR(IEC_WORD,	dummy_16_prev);
	DEC_VAR(IEC_UDINT,	start_time);

} EMB_SER_READ_PAR;

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */


/* --- 002 -------------------------------------------------------------------- */

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

	DEC_VAR(IEC_BOOL,	req);			/* Request								*/
	DEC_VAR(IEC_UINT,	id_off);		/* Communication channel				*/
	DEC_VAR(IEC_STRMAX, sd_1);			/* value of to be set					*/
	DEC_VAR(IEC_BOOL,	done_error);	/* Function performed					*/
	DEC_VAR(IEC_DINT,	status);		/* Last detected status 				*/
	DEC_VAR(IEC_CHAR,	prev_state);
	DEC_VAR(IEC_UDINT,	start_time);

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

typedef struct EMB_SER_WRITE_PAR_TAG	/* Parametric control					*/
{
	DEC_VAR(IEC_BOOL,	req);			/* Request								*/
	DEC_VAR(IEC_BYTE,	dummy_08_req);
	DEC_VAR(IEC_UINT,	id_off);		/* Communication channel				*/
	DEC_VAR(IEC_STRMAX, sd_1);			/* value of to be set					*/
	DEC_VAR(IEC_BOOL,	done_error);	/* Function performed					*/
	DEC_VAR(IEC_DINT,	status);		/* Last detected status 				*/
	DEC_VAR(IEC_CHAR,	prev_state);
	DEC_VAR(IEC_BYTE,	dummy_08_prev);
	DEC_VAR(IEC_UDINT,	start_time);

}  EMB_SER_WRITE_PAR;

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

typedef struct EMB_SER_WRITE_PAR_TAG	/* Parametric control					*/
{
	DEC_VAR(IEC_BOOL,	req);			/* Request								*/
	DEC_VAR(IEC_BYTE,	dummy_08_req);
	DEC_VAR(IEC_UINT,	id_off);		/* Communication channel				*/
	DEC_VAR(IEC_STRMAX, sd_1);			/* value of to be set					*/
	DEC_VAR(IEC_BOOL,	done_error);	/* Function performed					*/
	DEC_VAR(IEC_WORD,	dummy_16_done);
	DEC_VAR(IEC_DINT,	status);		/* Last detected status 				*/
	DEC_VAR(IEC_CHAR,	prev_state);
	DEC_VAR(IEC_BYTE,	dummy_08_prev);
	DEC_VAR(IEC_WORD,	dummy_16_prev);
	DEC_VAR(IEC_UDINT,	start_time);

}  EMB_SER_WRITE_PAR;

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */


/* --- 009 -------------------------------------------------------------------- */

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

typedef struct							/* Polled data acquisition				*/
{
	DEC_VAR(IEC_BOOL,  req);			/* Request								*/
	DEC_VAR(IEC_UINT,  id_off); 		/* Communication channel				*/
	DEC_VAR(IEC_DINT,  rd_len_1);		/* Max. length of the data to be receiv */
	DEC_VAR(IEC_BOOL,  read_all);		/* TRUE: all available data will be rea */
	DEC_VAR(IEC_DINT,  pos);			/* position in array where to store dat */
	DEC_PTR(IEC_BYTE,  rd_1);			/* Received user data					*/	
	DEC_VAR(IEC_BOOL,  ndr_error);		/* New user data received				*/
	DEC_VAR(IEC_DINT,  status); 		/* Last detected status 				*/
	DEC_VAR(IEC_DINT,  rd_len_2);		/* Length of the received data			*/
	DEC_VAR(IEC_BYTE,  prev_state);
	DEC_VAR(IEC_UDINT, start_time);

} EMB_SER_ARRAY_READ_PAR;

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

typedef struct							/* Polled data acquisition				*/
{
	DEC_VAR(IEC_BOOL,  req);			/* Request								*/
	DEC_VAR(IEC_CHAR,  dummy1);
	DEC_VAR(IEC_UINT,  id_off); 		/* Communication channel				*/
	DEC_VAR(IEC_DINT,  rd_len_1);		/* Max. length of the data to be receiv */
	DEC_VAR(IEC_BOOL,  read_all);		/* TRUE: all available data will be rea */
	DEC_VAR(IEC_CHAR,  dummy2);
	DEC_VAR(IEC_DINT,  pos);			/* position in array where to store dat */
	DEC_PTR(IEC_BYTE,  rd_1);			/* Received user data					*/	
	DEC_VAR(IEC_BOOL,  ndr_error);		/* New user data received				*/
	DEC_VAR(IEC_CHAR,  dummy3);
	DEC_VAR(IEC_DINT,  status); 		/* Last detected status 				*/
	DEC_VAR(IEC_DINT,  rd_len_2);		/* Length of the received data			*/
	DEC_VAR(IEC_BYTE,  prev_state);
	DEC_VAR(IEC_CHAR,  dummy4);
	DEC_VAR(IEC_UDINT, start_time);

} EMB_SER_ARRAY_READ_PAR;

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

typedef struct							/* Polled data acquisition				*/
{
	DEC_VAR(IEC_BOOL,  req);			/* Request								*/
	DEC_VAR(IEC_CHAR,  dummy1);
	DEC_VAR(IEC_UINT,  id_off); 		/* Communication channel				*/
	DEC_VAR(IEC_DINT,  rd_len_1);		/* Max. length of the data to be receiv */
	DEC_VAR(IEC_BOOL,  read_all);		/* TRUE: all available data will be rea */
	DEC_VAR(IEC_CHAR,  dummy2);
	DEC_VAR(IEC_WORD,  dummy2a);
	DEC_VAR(IEC_DINT,  pos);			/* position in array where to store dat */
	DEC_PTR(IEC_BYTE,  rd_1);			/* Received user data					*/	
	DEC_VAR(IEC_BOOL,  ndr_error);		/* New user data received				*/
	DEC_VAR(IEC_CHAR,  dummy3);
	DEC_VAR(IEC_WORD,  dummy3a);
	DEC_VAR(IEC_DINT,  status); 		/* Last detected status 				*/
	DEC_VAR(IEC_DINT,  rd_len_2);		/* Length of the received data			*/
	DEC_VAR(IEC_BYTE,  prev_state);
	DEC_VAR(IEC_CHAR,  dummy4);
	DEC_VAR(IEC_WORD,  dummy4a);
	DEC_VAR(IEC_UDINT, start_time);

} EMB_SER_ARRAY_READ_PAR;

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */


/* --- 010 -------------------------------------------------------------------- */

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

typedef struct							/* Parametric control					*/
{
	DEC_VAR(IEC_BOOL,  req);			/* Request								*/
	DEC_VAR(IEC_UINT,  id_off); 		/* Communication channel				*/
	DEC_VAR(IEC_DINT,  sd_len_1);		/* Data length							*/
	DEC_VAR(IEC_DINT,  pos);			/* position in array where to store dat */
	DEC_PTR(IEC_BYTE,  sd_1);			/* Send user data						*/
	DEC_VAR(IEC_BOOL,  done_error); 	/* Function performed					*/
	DEC_VAR(IEC_DINT,  status); 		/* Last detected status 				*/
	DEC_VAR(IEC_CHAR,  prev_state);
	DEC_VAR(IEC_UDINT, start_time);

} EMB_SER_ARRAY_WRITE_PAR;

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

typedef struct							/* Parametric control					*/
{
	DEC_VAR(IEC_BOOL,  req);			/* Request								*/
	DEC_VAR(IEC_CHAR,  dummy1);
	DEC_VAR(IEC_UINT,  id_off); 		/* Communication channel				*/
	DEC_VAR(IEC_DINT,  sd_len_1);		/* Data length							*/
	DEC_VAR(IEC_DINT,  pos);			/* position in array where to store dat */
	DEC_PTR(IEC_BYTE,  sd_1);			/* Send user data						*/
	DEC_VAR(IEC_BOOL,  done_error); 	/* Function performed					*/
	DEC_VAR(IEC_CHAR,  dummy2);
	DEC_VAR(IEC_DINT,  status); 		/* Last detected status 				*/
	DEC_VAR(IEC_CHAR,  prev_state);
	DEC_VAR(IEC_CHAR,  dummy3);
	DEC_VAR(IEC_UDINT, start_time);

} EMB_SER_ARRAY_WRITE_PAR;

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

typedef struct							/* Parametric control					*/
{
	DEC_VAR(IEC_BOOL,  req);			/* Request								*/
	DEC_VAR(IEC_CHAR,  dummy1);
	DEC_VAR(IEC_UINT,  id_off); 		/* Communication channel				*/
	DEC_VAR(IEC_DINT,  sd_len_1);		/* Data length							*/
	DEC_VAR(IEC_DINT,  pos);			/* position in array where to store dat */
	DEC_PTR(IEC_BYTE,  sd_1);			/* Send user data						*/
	DEC_VAR(IEC_BOOL,  done_error); 	/* Function performed					*/
	DEC_VAR(IEC_CHAR,  dummy2);
	DEC_VAR(IEC_WORD,  dummy2a);
	DEC_VAR(IEC_DINT,  status); 		/* Last detected status 				*/
	DEC_VAR(IEC_CHAR,  prev_state);
	DEC_VAR(IEC_CHAR,  dummy3);
	DEC_VAR(IEC_WORD,  dummy3a);
	DEC_VAR(IEC_UDINT, start_time);

} EMB_SER_ARRAY_WRITE_PAR;

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */

/* --- 011 -------------------------------------------------------------------- */

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

typedef struct EMB_SER_BREAK_MARK_PAR_TAG
{                                       /* sending Break and / or Mark			*/
	DEC_VAR(IEC_BOOL,	req);			/* Request								*/
	DEC_VAR(IEC_UINT,	id_off);		/* Communication channel				*/
	DEC_VAR(IEC_DINT,	break_len);		/* length of the Break in ms.       	*/
	DEC_VAR(IEC_DINT,	mark_len);		/* length of the Mark in ms.       	    */
	DEC_VAR(IEC_BOOL,   error); 	    /* Function performed					*/
	DEC_VAR(IEC_DINT,	status);		/* Last detected status 				*/

} EMB_SER_BREAK_MARK_PAR;

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

typedef struct EMB_SER_BREAK_MARK_PAR_TAG
{                                       /* sending Break and / or Mark			*/
	DEC_VAR(IEC_BOOL,	req);			/* Request								*/
	DEC_VAR(IEC_BYTE,	dummy_08_req);
	DEC_VAR(IEC_UINT,	id_off);		/* Communication channel				*/
	DEC_VAR(IEC_DINT,	break_len);		/* length of the Break in ms.       	*/
	DEC_VAR(IEC_DINT,	mark_len);		/* length of the Mark in ms.       	    */
	DEC_VAR(IEC_BOOL,   error); 	    /* Function performed					*/
	DEC_VAR(IEC_CHAR,   dummy2);
	DEC_VAR(IEC_DINT,	status);		/* Last detected status 				*/

} EMB_SER_BREAK_MARK_PAR;

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

typedef struct EMB_SER_BREAK_MARK_PAR_TAG
{                                       /* sending Break and / or Mark			*/
	DEC_VAR(IEC_BOOL,	req);			/* Request								*/
	DEC_VAR(IEC_BYTE,	dummy_08_req);
	DEC_VAR(IEC_UINT,	id_off);		/* Communication channel				*/
	DEC_VAR(IEC_DINT,	break_len);		/* length of the Break in ms.       	*/
	DEC_VAR(IEC_DINT,	mark_len);		/* length of the Mark in ms.       	    */
	DEC_VAR(IEC_BOOL,   error); 	    /* Function performed					*/
	DEC_VAR(IEC_CHAR,   dummy2);
	DEC_VAR(IEC_WORD,   dummy2a);
	DEC_VAR(IEC_DINT,	status);		/* Last detected status 				*/

} EMB_SER_BREAK_MARK_PAR;

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */



typedef struct emb_cc_pair_tag
{
	EMB_SER_CONNECT_PAR*	 pConnect;
	int 					 ccd;		/* communication channel descriptor; 
										 -1 means that the communication channel is closed */
	struct termios			 old_tio;
	int 					 timeout;	/* channel timeout						*/
	char					 status;
	
	struct emb_cc_pair_tag*  next;

} emb_cc_pair, emb_cc_map;

#endif	/* FC_CFG_SERIALCOMM_LIB */

#endif	/* _FCSERCOM_H_ */

/* ---------------------------------------------------------------------------- */
