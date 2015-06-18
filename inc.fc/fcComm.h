
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
 * Filename: fcComm.h
 */


#ifndef _FCCOMM_H_
#define _FCCOMM_H_

#if defined(FC_CFG_UDPCOMM_LIB)

/* --- 016 -------------------------------------------------------------------- */
void fun_com_error_string(STDLIBFUNCALL); 

/* --- 003 -------------------------------------------------------------------- */
void fb_udp_connect(STDLIBFBCALL);
/* --- 005 -------------------------------------------------------------------- */
void fb_udp_urcv(STDLIBFBCALL);
/* --- 006 -------------------------------------------------------------------- */
void fb_udp_usend(STDLIBFBCALL);
/* --- 007 -------------------------------------------------------------------- */
void fb_udp_array_urcv(STDLIBFBCALL);
/* --- 008 -------------------------------------------------------------------- */
void fb_udp_array_usend(STDLIBFBCALL);

#define C_EMB_NOERROR						0

/* error numbers of udp functionblocks 
 */
#define C_EMB_UDP_ERR_SOCK_INIT_FAILED	-201	   /* the udp socket communication could not been initialized */
#define C_EMB_UDP_ERR_SOCK_NOT_OPENED	-202	   /* the udp socket has not been opened before sending or recieving */
#define C_EMB_UDP_ERR_UNKNOWN_HOST		-203	   /* the host address could not be resoved */
#define C_EMB_UDP_ERR_SEND				-204	   /* there has been an io error while sending a message */
#define C_EMB_UDP_ERR_RECV				-205	   /* there has been an io error while trying to recieve a message */
#define C_EMB_UDP_ERR_RECV_MSG_LONGER	-206	   /* the received packet contained more bytes then parameter DATA_LEN
														the message has been truncated */
#define C_EMB_UDP_ERR_SEND_MSG_TOO_LONG -207	   /* the message contains to much bytes, that can't be trasnported by 
														a udp socket */
#define C_EMB_UDP_ERR_SEND_HOST_UNREACH -208	   /* the specified partner host can't be reached at the moment */
#define C_EMB_UDP_ERR_LOOKUP_RUNNING	-209	   /* Lookup HostName is still running, but request has been changed */
#define C_EMB_UDP_ERR_LOOKUP_HN_CHANGE	-210	   /* HostName input of UDP_Lookup changed during lookup has been
														 still in process */
#define C_EMB_UDP_ERR_MULTCAST_PARAM	-211	   /* Multicast address not permited */    
#define C_EMB_UDP_ERR_PORTNR_OUTOFRANGE -212	   /* Port Nr has been out of range, valid port nr: (0..65535) */

#define C_EMB_NOERROR_STR					"Success"
#define C_EMB_UDP_ERR_SOCK_INIT_FAILED_STR	"Udp socket communication not initialized";
#define C_EMB_UDP_ERR_SOCK_NOT_OPENED_STR	"Udp socket not open";
#define C_EMB_UDP_ERR_UNKNOWN_HOST_STR		 "Unknown host";
#define C_EMB_UDP_ERR_SEND_STR				"Udp send IO error";
#define C_EMB_UDP_ERR_RECV_STR				"Udp receive IO error";
#define C_EMB_UDP_ERR_RECV_MSG_LONGER_STR	"Received packet truncated, because it was been longer than expected";
#define C_EMB_UDP_ERR_SEND_MSG_TOO_LONG_STR "Message too long";
#define C_EMB_UDP_ERR_SEND_HOST_UNREACH_STR "Partner host unreachable";
#define C_EMB_UDP_ERR_LOOKUP_RUNNING_STR	"Address lookup is still running";
#define C_EMB_UDP_ERR_LOOKUP_HN_CHANGE_STR	"Host name changed since last lookup";
#define C_EMB_UDP_ERR_MULTCAST_PARAM_STR	"Invalid multicast paramter";
#define C_EMB_UDP_ERR_PORTNR_OUTOFRANGE_STR "Port nr out of range";
					


typedef struct
{
	DEC_VAR(IEC_DINT,		 LocalPort);
	DEC_VAR(IEC_BOOL,		 BroadcastSocket);

} EMB_UDP_SOCKET_HANDLE;

typedef struct
{
	DEC_VAR(IEC_STRLEN, curLen);
	DEC_VAR(IEC_STRLEN, maxLen);
	DEC_VAR(IEC_CHAR,	str[15]);

} IEC_STRING15_IMPL;

typedef struct
{
	DEC_VAR(IEC_STRLEN, curLen);
	DEC_VAR(IEC_STRLEN, maxLen);
	DEC_VAR(IEC_CHAR,	str[VMM_MAX_IEC_STRLEN]);

} IEC_STRING_IMPL;

typedef struct udp_sd_pair_tag
{
	const EMB_UDP_SOCKET_HANDLE*  udp_socket;	/* socket structure */
	int 						  sd;			/* socket descriptor */
	struct udp_sd_pair_tag* 	  next;

} udp_sd_pair, udp_sd_map;


/* --- 003 -------------------------------------------------------------------- */

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

typedef struct
{
	DEC_VAR(IEC_BOOL,		  en_c);		/* true: open and connect the socket 
												the field LocalPort of SOCKET_HANDLE has to be filled
												with the local port number where the socket wants to
												receive messages				*/
											/* false: close the socket
												this FunctionBlock should be called only once, when the
												socket should be opened or closed */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE, socket_handle); /* a structure that describes the socket */

	DEC_VAR(IEC_BOOL,		  valid);		  /* shows if the socket is open or close */
	DEC_VAR(IEC_DINT,		  error);		  /* error information */

} FB_UDP_CONNECT_PAR;

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

typedef struct
{
	DEC_VAR(IEC_BOOL,		  en_c);		/* true: open and connect the socket 
												the field LocalPort of SOCKET_HANDLE has to be filled
												with the local port number where the socket wants to
												receive messages				*/
											/* false: close the socket
												this FunctionBlock should be called only once, when the
												socket should be opened or closed */
	DEC_VAR(IEC_CHAR,		  dummy1);
	DEC_PTR(EMB_UDP_SOCKET_HANDLE, socket_handle); /* a structure that describes the socket */
	DEC_VAR(IEC_BOOL,		  valid);		  /* shows if the socket is open or close */
	DEC_VAR(IEC_CHAR,		  dummy2);
	DEC_VAR(IEC_DINT,		  error);		  /* error information */

} FB_UDP_CONNECT_PAR;

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

typedef struct FB_UDP_CONNECT_PAR_TAG
{
	DEC_VAR(IEC_BOOL,		  en_c);		/* true: open and connect the socket 
												the field LocalPort of SOCKET_HANDLE has to be filled
												with the local port number where the socket wants to
												receive messages				*/
											/* false: close the socket
												this FunctionBlock should be called only once, when the
												socket should be opened or closed */
	DEC_VAR(IEC_BYTE,		  dummy1);
	DEC_VAR(IEC_WORD,		  dummy1a);

	DEC_PTR(EMB_UDP_SOCKET_HANDLE, socket_handle); /* a structure that describes the socket */

	DEC_VAR(IEC_BOOL,		  valid);		  /* shows if the socket is open or close */
	DEC_VAR(IEC_CHAR,		  dummy2);
	DEC_VAR(IEC_WORD,		  dummy2a);
	DEC_VAR(IEC_DINT,		  error);		  /* error information */

} FB_UDP_CONNECT_PAR;

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */


/* --- 005 -------------------------------------------------------------------- */

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

typedef struct
{
	/* input */
	DEC_VAR(IEC_BOOL,			en_r);			/* true: look for a new msg, false: do nothing */
	/* in_out */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE, socket_handle);/* a handle to a opened socket */
	DEC_PTR(IEC_DINT,			length);		/* the maximum number of bytes that want to be received */
												/*	udp is a datagram based protocol, that means the
													data will be received in packets. If the available packet is	  
													longer than DATA_LEN, only DATA_LEN bytes will be returend,
													the rest is lost */
	/* output */			
	DEC_VAR(IEC_STRMAX, 		data);			/* contains the msg data, if a new msg has been received */
	DEC_VAR(IEC_STRING15_IMPL,	partner_addr);	/* the address and port of the communication partner */
	DEC_VAR(IEC_DINT,			partner_port);	/* that sent the received msg	*/
	DEC_VAR(IEC_BOOL,			ndr);			/* hows if a new msg has been received */
	DEC_VAR(IEC_DINT,			error); 		/* error information			*/

} FB_UDP_URCV_PAR;

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

typedef struct
{
	/* input */
	DEC_VAR(IEC_BOOL,			en_r);			/* true: look for a new msg, false: do nothing */
	DEC_VAR(IEC_CHAR,			dummy1);
	/* in_out */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE, socket_handle);/* a handle to a opened socket */
	DEC_PTR(IEC_DINT,			length);		/* the maximum number of bytes that want to be received */
												/*	udp is a datagram based protocol, that means the
													data will be received in packets. If the available packet is	  
													longer than DATA_LEN, only DATA_LEN bytes will be returend,
													the rest is lost */
	/* output */			
	DEC_VAR(IEC_STRMAX, 		data);			/* contains the msg data, if a new msg has been received */
	DEC_VAR(IEC_STRING15_IMPL,	partner_addr);	/* the address and port of the communication partner */
	DEC_VAR(IEC_DINT,			partner_port);	/* that sent the received msg	*/
	DEC_VAR(IEC_BOOL,			ndr);			/* hows if a new msg has been received */
	DEC_VAR(IEC_CHAR,			dummy2);
	DEC_VAR(IEC_DINT,			error); 		/* error information			*/

} FB_UDP_URCV_PAR;

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

typedef struct
{
	/* input */
	DEC_VAR(IEC_BOOL,			en_r);			/* true: look for a new msg, false: do nothing */
	DEC_VAR(IEC_CHAR,			dummy1);
	DEC_VAR(IEC_WORD,			dummy1a);
	/* in_out */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE, socket_handle);/* a handle to a opened socket */
	DEC_PTR(IEC_DINT,			length);		/* the maximum number of bytes that want to be received */
												/*	udp is a datagram based protocol, that means the
													data will be received in packets. If the available packet is	  
													longer than DATA_LEN, only DATA_LEN bytes will be returend,
													the rest is lost */
	/* output */			
	DEC_VAR(IEC_STRMAX, 		data);			/* contains the msg data, if a new msg has been received */
	DEC_VAR(IEC_STRING15_IMPL,	partner_addr);	/* the address and port of the communication partner */
	DEC_VAR(IEC_WORD,			dummy3);
	DEC_VAR(IEC_DINT,			partner_port);	/* that sent the received msg	*/
	DEC_VAR(IEC_BOOL,			ndr);			/* hows if a new msg has been received */
	DEC_VAR(IEC_CHAR,			dummy2);
	DEC_VAR(IEC_WORD,			dummy2a);
	DEC_VAR(IEC_DINT,			error); 		/* error information			*/

} FB_UDP_URCV_PAR;

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */


/* --- 006 -------------------------------------------------------------------- */

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

typedef struct
{
	/* input */
	DEC_VAR(IEC_BOOL,			en_s);			/* true: send the msg, false: do nothing */
	DEC_VAR(IEC_STRING15_IMPL,	partner_addr);	/* the address and port of the communication partner */
	DEC_VAR(IEC_DINT,			partner_port);	/* that sent the received msg	*/
	DEC_VAR(IEC_STRMAX, 		data);			/* the data of the msg that will be sent */
	/* in_out */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE,socket_handle);/* a handle to a opened socket */
	/* output */			
	DEC_VAR(IEC_DINT,			error); 		/* error information			*/

} FB_UDP_USEND_PAR;

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

typedef struct
{
	/* input */
	DEC_VAR(IEC_BOOL,			en_s);			/* true: send the msg, false: do nothing */
	DEC_VAR(IEC_STRING15_IMPL,	partner_addr);	/* the address and port of the communication partner */
	DEC_VAR(IEC_DINT,			partner_port);	/* that sent the received msg	*/
	DEC_VAR(IEC_STRMAX, 		data);			/* the data of the msg that will be sent */
	DEC_VAR(IEC_CHAR,			dummy1);
	/* in_out */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE,socket_handle);/* a handle to a opened socket */
	/* output */			
	DEC_VAR(IEC_DINT,			error); 		/* error information			*/

} FB_UDP_USEND_PAR;

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

typedef struct
{
	/* input */
	DEC_VAR(IEC_BOOL,			en_s);			/* true: send the msg, false: do nothing */
	DEC_VAR(IEC_STRING15_IMPL,	partner_addr);	/* the address and port of the communication partner */
	DEC_VAR(IEC_WORD,			dummy2);
	DEC_VAR(IEC_DINT,			partner_port);	/* that sent the received msg	*/
	DEC_VAR(IEC_STRMAX, 		data);			/* the data of the msg that will be sent */
	DEC_VAR(IEC_CHAR,			dummy1);
	DEC_VAR(IEC_WORD,			dummy2a);
	/* in_out */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE,socket_handle);/* a handle to a opened socket */
	/* output */			
	DEC_VAR(IEC_DINT,			error); 		/* error information			*/

} FB_UDP_USEND_PAR;

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */


/* --- 007 -------------------------------------------------------------------- */

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

typedef struct
{
	/* input */
	DEC_VAR(IEC_BOOL,			en_r);			/* true: look for a new msg, false: do nothing */
	DEC_VAR(IEC_DINT,			pos);			/* index in array where to store data */
	/* in_out */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE,socket_handle);/* a handle to a opened socket */
	DEC_PTR(IEC_BYTE,			data);			/* contains the msg data, if a new msg has been received */
	DEC_PTR(IEC_DINT,			length);		/* the maximum number of bytes that want to be received */
												/*	udp is a datagram based protocol, that means the
													data will be received in packets. If the available packet is	  
													longer than DATA_LEN, only DATA_LEN bytes will be returend,
													the rest is lost */
	/* output */		
	DEC_VAR(IEC_STRING15_IMPL,	partner_addr);	/* the address and port of the communication partner */
	DEC_VAR(IEC_DINT,			partner_port);	/* that sent the received msg	*/
	DEC_VAR(IEC_BOOL,			ndr);			/* hows if a new msg has been received */
	DEC_VAR(IEC_DINT,			error); 		/* error information			*/

} FB_UDP_ARRAY_URCV_PAR;

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

typedef struct
{
	/* input */
	DEC_VAR(IEC_BOOL,			en_r);			/* true: look for a new msg, false: do nothing */
	DEC_VAR(IEC_CHAR,			dummy1);
	DEC_VAR(IEC_DINT,			pos);			/* index in array where to store data */
	/* in_out */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE,socket_handle);/* a handle to a opened socket */
	DEC_PTR(IEC_BYTE,			data);			/* contains the msg data, if a new msg has been received */
	DEC_PTR(IEC_DINT,			length);		/* the maximum number of bytes that want to be received */
												/*	udp is a datagram based protocol, that means the
													data will be received in packets. If the available packet is	  
													longer than DATA_LEN, only DATA_LEN bytes will be returend,
													the rest is lost */
	/* output */		
	DEC_VAR(IEC_STRING15_IMPL,	partner_addr);	/* the address and port of the communication partner */
	DEC_VAR(IEC_CHAR,			dummy2);
	DEC_VAR(IEC_DINT,			partner_port);	/* that sent the received msg	*/
	DEC_VAR(IEC_BOOL,			ndr);			/* hows if a new msg has been received */
	DEC_VAR(IEC_CHAR,			dummy3);
	DEC_VAR(IEC_DINT,			error); 		/* error information			*/

} FB_UDP_ARRAY_URCV_PAR;

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

typedef struct
{
	/* input */
	DEC_VAR(IEC_BOOL,			en_r);			/* true: look for a new msg, false: do nothing */
	DEC_VAR(IEC_CHAR,			dummy1);
	DEC_VAR(IEC_WORD,			dummy1a);
	DEC_VAR(IEC_DINT,			pos);			/* index in array where to store data */
	/* in_out */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE,socket_handle);/* a handle to a opened socket */
	DEC_PTR(IEC_BYTE,			data);			/* contains the msg data, if a new msg has been received */
	DEC_PTR(IEC_DINT,			length);		/* the maximum number of bytes that want to be received */
												/*	udp is a datagram based protocol, that means the
													data will be received in packets. If the available packet is	  
													longer than DATA_LEN, only DATA_LEN bytes will be returend,
													the rest is lost */
	/* output */		
	DEC_VAR(IEC_STRING15_IMPL,	partner_addr);	/* the address and port of the communication partner */
	DEC_VAR(IEC_CHAR,			dummy2);
	DEC_VAR(IEC_WORD,			dummy2a);
	DEC_VAR(IEC_DINT,			partner_port);	/* that sent the received msg	*/
	DEC_VAR(IEC_BOOL,			ndr);			/* hows if a new msg has been received */
	DEC_VAR(IEC_CHAR,			dummy3);
	DEC_VAR(IEC_WORD,			dummy3a);
	DEC_VAR(IEC_DINT,			error); 		/* error information			*/

} FB_UDP_ARRAY_URCV_PAR;

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */


/* --- 008 -------------------------------------------------------------------- */

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

typedef struct
{
	/* input */
	DEC_VAR(IEC_BOOL,			en_s);			/* true: send the msg, false: do nothing */
	DEC_VAR(IEC_STRING15_IMPL,	partner_addr);	/* the address and port of the communication partner */
	DEC_VAR(IEC_DINT,			partner_port);	/* that sent the received msg	*/
	DEC_VAR(IEC_DINT,			pos);			/* index in array from where to read data */
	DEC_VAR(IEC_DINT,			length);		/* msg lenght in bytes			*/
	/* in_out */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE,socket_handle);/* a handle to a opened socket */
	DEC_PTR(IEC_BYTE,			data);			/* the data of the msg that will be sent */
	/* output */			
	DEC_VAR(IEC_DINT,			error); 		/* error information			*/

} FB_UDP_ARRAY_USEND_PAR;

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

typedef struct
{
	/* input */
	DEC_VAR(IEC_BOOL,			en_s);			/* true: send the msg, false: do nothing */
	DEC_VAR(IEC_STRING15_IMPL,	partner_addr);	/* the address and port of the communication partner */
	DEC_VAR(IEC_DINT,			partner_port);	/* that sent the received msg	*/
	DEC_VAR(IEC_DINT,			pos);			/* index in array from where to read data */
	DEC_VAR(IEC_DINT,			length);		/* msg lenght in bytes			*/
	/* in_out */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE,socket_handle);/* a handle to a opened socket */
	DEC_PTR(IEC_BYTE,			data);			/* the data of the msg that will be sent */
	/* output */			
	DEC_VAR(IEC_DINT,			error); 		/* error information			*/

} FB_UDP_ARRAY_USEND_PAR;

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

typedef struct
{
	/* input */
	DEC_VAR(IEC_BOOL,			en_s);			/* true: send the msg, false: do nothing */
	DEC_VAR(IEC_STRING15_IMPL,	partner_addr);	/* the address and port of the communication partner */
	DEC_VAR(IEC_WORD,			dummy1);
	DEC_VAR(IEC_DINT,			partner_port);	/* that sent the received msg	*/
	DEC_VAR(IEC_DINT,			pos);			/* index in array from where to read data */
	DEC_VAR(IEC_DINT,			length);		/* msg lenght in bytes			*/
	/* in_out */
	DEC_PTR(EMB_UDP_SOCKET_HANDLE,socket_handle);/* a handle to a opened socket */
	DEC_PTR(IEC_BYTE,			data);			/* the data of the msg that will be sent */
	/* output */			
	DEC_VAR(IEC_DINT,			error); 		/* error information			*/

} FB_UDP_ARRAY_USEND_PAR;

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */


/* --- 003 -------------------------------------------------------------------- */

typedef struct FUN_COM_ERROR_STRING_PAR_TAG
{
	DEC_FUN_DINT(errcd);					/* error code						*/
	DEC_FUN_PTR(IEC_STRMAX, errstr);		/* return value: error string		*/

} FUN_COM_ERROR_STRING_PAR;

#endif	/* FC_CFG_UDPCOMM_LIB */

#endif /* _FCCOMM_H_ */

/* ---------------------------------------------------------------------------- */
