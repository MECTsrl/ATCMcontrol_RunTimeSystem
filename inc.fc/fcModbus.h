
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
 * Filename: fcModbus.h
 */


#ifndef _FCMODBUS_H_
#define _FCMODBUS_H_

#if defined(FC_CFG_MODBUS_RTU_LIB)

#include <termios.h>

#include "fcSerCom.h"

/* --- 000 -------------------------------------------------------------------- */
void modbus_fct01(STDLIBFBCALL);
/* --- 001 -------------------------------------------------------------------- */
void modbus_fct02(STDLIBFBCALL);
/* --- 002 -------------------------------------------------------------------- */
void modbus_fct03(STDLIBFBCALL);
/* --- 003 -------------------------------------------------------------------- */
void modbus_fct04(STDLIBFBCALL);
/* --- 004 -------------------------------------------------------------------- */
void modbus_fct05(STDLIBFBCALL);
/* --- 005 -------------------------------------------------------------------- */
void modbus_fct06(STDLIBFBCALL);
/* --- 006 -------------------------------------------------------------------- */
void modbus_fct07(STDLIBFBCALL);
/* --- 007 -------------------------------------------------------------------- */
void modbus_fct08(STDLIBFBCALL);
/* --- 008 -------------------------------------------------------------------- */
void modbus_fct0F(STDLIBFBCALL);
/* --- 009 -------------------------------------------------------------------- */
void modbus_fct10(STDLIBFBCALL);

/* Error codes definitions */
#define MB_NOERROR    				 OK

/* standard error codes */
#define MB_ILLEGAL_FUNCTION          1    /* Function code received is not an allowable action */
#define MB_ILLEGAL_DATA_ADDRESS      2    /* Data address received is not an allowable address */
#define MB_ILLEGAL_DATA_VALUE        3    /* Value contained in a query is not an allowable value */
#define MB_SLAVE_DEVICE_FAILURE      4    /* Unrecoverable error while performing requested action */
#define MB_ACKNOWLEDGE               5    /* Special use for programming commands */
#define MB_SLAVE_DEVICE_BUSY         6    /* Special use for programming commands */
#define MB_MEMORY_PARITY_ERROR       8    /* Consistency check failed */
#define MB_GATEWAY_PATH_UNAVAILABLE  10   /* Gateway failed to allocate an internal communication path */
#define MB_GATEWAY_TARGET_FAILED     11   /* Device communication thru gateway failed to respond */

/* 4c error codes */
#define MB_ERR_FCT_UNKNOWN             20
#define MB_ERR_SUBFCT_UNKNOWN          21
#define MB_ERR_STATE_INVALID           22
#define MB_ERR_CRC_FAILED              23
#define MB_ERR_TIMEOUT                 24
#define MB_ERR_FRAME_LENGTH_INVALID    25
#define MB_ERR_FRAME_DATA_INVALID      26
#define MB_ERR_INTERNAL_BUFF	       27
#define MB_ERR_FATAL                   28
#define MB_ERR_NO_LICENSE              29

typedef struct MB_FCT01_PAR_TAG
{
     DEC_VAR(IEC_BOOL, bReq);            	/* If true and no other operation pending starts request */
     DEC_VAR(IEC_BYTE, dummy_08_req);
     DEC_VAR(IEC_UINT, uComIdOff);       	/* Communication channel */
     DEC_VAR(IEC_BYTE, bySlaveAddr);     	/* Slave address */
     DEC_VAR(IEC_BYTE, dummy_08_slave);
     DEC_VAR(IEC_UINT, uStartAddr);      	/* Starting Address */
     DEC_VAR(IEC_UINT, uCoilQty);        	/* Quantity of Coils */
     DEC_VAR(IEC_UINT, dummy_16_reg);
     DEC_PTR(IEC_BYTE, bypCoilStatus);    	/* Coils status */
     DEC_VAR(IEC_BOOL, bReady);          	/* Response ready */
     DEC_VAR(IEC_BYTE, byException);     	/* Exception code */
     DEC_VAR(IEC_BYTE, byState);         	/* State device values */ 
     DEC_VAR(IEC_STRMAX, sBuff);	     	/* Internal communication buffer */
} MB_FCT01_PAR;

typedef struct MB_FCT02_PAR_TAG
{
     DEC_VAR(IEC_BOOL, bReq);
     DEC_VAR(IEC_BYTE, dummy_08_req);
     DEC_VAR(IEC_UINT, uComIdOff);        	/* Communication channel */
     DEC_VAR(IEC_BYTE, bySlaveAddr);      	/* Slave address */
     DEC_VAR(IEC_BYTE, dummy_08_slave);
     DEC_VAR(IEC_UINT, uStartAddr);       	/* Starting Address */
     DEC_VAR(IEC_UINT, uInputQty);        	/* Quantity of Inputs */
     DEC_VAR(IEC_UINT, dummy_16_reg);
     DEC_PTR(IEC_BYTE, bypInputStatus);    	/* Input status */
     DEC_VAR(IEC_BOOL, bReady);           	/* Response ready */
     DEC_VAR(IEC_BYTE, byException);      	/* Exception code */
     DEC_VAR(IEC_BYTE, byState);          	/* State device values */
     DEC_VAR(IEC_STRMAX, sBuff);	     	/* Internal communication buffer */
} MB_FCT02_PAR;

typedef struct MB_FCT03_PAR_TAG
{
     DEC_VAR(IEC_BOOL, bReq);
     DEC_VAR(IEC_BYTE, dummy_08_req);
     DEC_VAR(IEC_UINT, uComIdOff);			/* Communication channel */
     DEC_VAR(IEC_BYTE, bySlaveAddr);        /* Slave address */
     DEC_VAR(IEC_BYTE, dummy_08_slave);
     DEC_VAR(IEC_UINT, uStartAddr);        	/* Starting Address */
     DEC_VAR(IEC_UINT, uRegQty);           	/* Quantity of Registers */
     DEC_VAR(IEC_UINT, dummy_16_reg);
     DEC_PTR(IEC_UINT, upRegValue);        	/* Read Register Values */
     DEC_VAR(IEC_BOOL, bReady);            	/* Response ready */
     DEC_VAR(IEC_BYTE, byException);       	/* Exception code */
     DEC_VAR(IEC_BYTE, byState);           	/* State device values */
     DEC_VAR(IEC_STRMAX, sBuff);	     	/* Internal communication buffer */
} MB_FCT03_PAR;

typedef struct MB_FCT04_PAR_TAG
{
     DEC_VAR(IEC_BOOL, bReq);
     DEC_VAR(IEC_BYTE, dummy_08_req);
     DEC_VAR(IEC_UINT, uComIdOff);          /* Communication channel */
     DEC_VAR(IEC_BYTE, bySlaveAddr);        /* Slave address */
     DEC_VAR(IEC_BYTE, dummy_08_slave);
     DEC_VAR(IEC_UINT, uStartAddr);         /* Starting Address */
     DEC_VAR(IEC_UINT, uRegQty);            /* Quantity of Registers */
     DEC_VAR(IEC_UINT, dummy_16_reg);
     DEC_PTR(IEC_UINT, upRegValue);			/* Read Register Values */
     DEC_VAR(IEC_BOOL, bReady);             /* Response ready */
     DEC_VAR(IEC_BYTE, byException);        /* Exception code */
     DEC_VAR(IEC_BYTE, byState);            /* State device values */
     DEC_VAR(IEC_STRMAX, sBuff);	     	/* Internal communication buffer */
} MB_FCT04_PAR;

typedef struct MB_FCT05_PAR_TAG
{
     DEC_VAR(IEC_BOOL, bReq);
     DEC_VAR(IEC_BYTE, dummy_08_req);
     DEC_VAR(IEC_UINT, uComIdOff);         	/* Communication channel */
     DEC_VAR(IEC_BYTE, bySlaveAddr);        /* Slave address */
     DEC_VAR(IEC_BYTE, dummy_08_slave);
     DEC_VAR(IEC_UINT, uOutputAddr);        /* Output Address */
     DEC_VAR(IEC_UINT, uOutputValue);       /* Output Value */
     DEC_VAR(IEC_BOOL, bReady);             /* Response ready */
     DEC_VAR(IEC_BYTE, byException);        /* Exception code */
     DEC_VAR(IEC_BYTE, byState);            /* State device values */
     DEC_VAR(IEC_STRMAX, sBuff);	     	/* Internal communication buffer */
} MB_FCT05_PAR;

typedef struct MB_FCT06_TAG
{
     DEC_VAR(IEC_BOOL, bReq);
     DEC_VAR(IEC_BYTE, dummy_08_req);
     DEC_VAR(IEC_UINT, uComIdOff);          /* Communication channel */
     DEC_VAR(IEC_BYTE, bySlaveAddr);        /* Slave address */
     DEC_VAR(IEC_BYTE, dummy_08_slave);
     DEC_VAR(IEC_UINT, uRegAddr);		    /* Register address */
     DEC_VAR(IEC_UINT, uRegValue);          /* Register value */
     DEC_VAR(IEC_BOOL, bReady);             /* Response ready */
     DEC_VAR(IEC_BYTE, byException);        /* Exception code */
     DEC_VAR(IEC_BYTE, byState);            /* State device values */
     DEC_VAR(IEC_STRMAX, sBuff);	     	/* Internal communication buffer */
} MB_FCT06_PAR;

typedef struct MB_FCT07_PAR_TAG
{
     DEC_VAR(IEC_BOOL, bReq);
     DEC_VAR(IEC_BYTE, dummy_08_req);
     DEC_VAR(IEC_UINT, uComIdOff);          /* Communication channel */
     DEC_VAR(IEC_BYTE, bySlaveAddr);        /* Slave address */
     DEC_VAR(IEC_BYTE, byOutputData);       /* Output data */
     DEC_VAR(IEC_BOOL, bReady);             /* Response ready */
     DEC_VAR(IEC_BYTE, byException);        /* Exception code */
     DEC_VAR(IEC_BYTE, byState);            /* State device values */
     DEC_VAR(IEC_STRMAX, sBuff);	     	/* Internal communication buffer */
} MB_FCT07_PAR;

typedef struct MB_FCT08_PAR_TAG
{
     DEC_VAR(IEC_BOOL, bReq);
     DEC_VAR(IEC_BYTE, dummy_08_req);
     DEC_VAR(IEC_UINT, uComIdOff);          /* Communication channel */
     DEC_VAR(IEC_BYTE, bySlaveAddr);        /* Slave address */
     DEC_VAR(IEC_BYTE, dummy_08_slave);
     DEC_VAR(IEC_UINT, uCode);              /* Sub-function code */
     DEC_VAR(IEC_BYTE, bySendLen);          /* Number of elements send in request */
     DEC_VAR(IEC_BYTE, dummy_08_len);
     DEC_VAR(IEC_UINT, dummy_16_len);
     DEC_PTR(IEC_UINT, upData);				/* Request/response data values */
     DEC_VAR(IEC_BYTE, byRecvLen);          /* Number of elements receive from response */
     DEC_VAR(IEC_BOOL, bReady);             /* Response ready */
     DEC_VAR(IEC_BYTE, byException);        /* Exception code */
     DEC_VAR(IEC_BYTE, byState);            /* State device values */
     DEC_VAR(IEC_STRMAX, sBuff);	     	/* Internal communication buffer */
} MB_FCT08_PAR;

typedef struct MB_FCT0F_PAR_TAG
{
     DEC_VAR(IEC_BOOL, bReq);
     DEC_VAR(IEC_BYTE, dummy_08_req);
     DEC_VAR(IEC_UINT, uComIdOff);          /* Communication channel */
     DEC_VAR(IEC_BYTE, bySlaveAddr);        /* Slave address */
     DEC_VAR(IEC_BYTE, dummy_08_slave);
     DEC_VAR(IEC_UINT, uStartAddr);         /* Starting Address */
     DEC_VAR(IEC_UINT, uOutputQty);         /* Quantity of Outputs */
     DEC_VAR(IEC_UINT, dummy_16_reg);
     DEC_PTR(IEC_BYTE, bypOutputValue);		/* Output Values */
     DEC_VAR(IEC_BOOL, bReady);             /* Response ready */
     DEC_VAR(IEC_BYTE, byException);        /* Exception code */
     DEC_VAR(IEC_BYTE, byState);            /* State device values */
     DEC_VAR(IEC_STRMAX, sBuff);	     	/* Internal communication buffer */
} MB_FCT0F_PAR;

typedef struct MB_FCT10_TAG
{
     DEC_VAR(IEC_BOOL, bReq);
     DEC_VAR(IEC_BYTE, dummy_08_req);
     DEC_VAR(IEC_UINT, uComIdOff);          /* Communication channel */
     DEC_VAR(IEC_BYTE, bySlaveAddr);        /* Slave address */
     DEC_VAR(IEC_BYTE, dummy_08_slave);
     DEC_VAR(IEC_UINT, uRegAddr);           /* Register address */
     DEC_VAR(IEC_UINT, uRegQty);            /* Quantity of Registers */
     DEC_VAR(IEC_UINT, dummy_16_reg);
     DEC_PTR(IEC_BYTE, bypRegValue);         /* Registers Values */
     DEC_VAR(IEC_BOOL, bReady);             /* Response ready */
     DEC_VAR(IEC_BYTE, byException);        /* Exception code */
     DEC_VAR(IEC_BYTE, byState);            /* State device values */
     DEC_VAR(IEC_STRMAX, sBuff);	     	/* Internal communication buffer */
} MB_FCT10_PAR;

#endif	/* FC_CFG_MODBUS_RTU_LIB */

#endif	/* _FCMODBUS_H_ */

