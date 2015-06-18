
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
 * Filename: libMect.h
 */

#ifndef APP_MECT_H__
#define APP_MECT_H__

#define RTS_PRAGMA_PACK_1
#include "osAlign.h"
#undef RTS_PRAGMA_PACK_1

#include "stdInc.h"
#include "libDef.h"



extern unsigned char app_mect_flag;
extern short app_mect_status;
extern float app_mect_enquiry;

/*
 * Function prototypes
 */

int app_mect_init(void);
int app_mect_done(void);
short app_mect_enq(short id, char *command, int dd);
short app_mect_float_stx(short id, char *command, float value, int dd);
short app_mect_hex_stx(short id, char *command, unsigned short value, int dd);

/* PLC view*/
extern unsigned char PLC_MECT_flag;
extern short PLC_MECT_status;
extern float PLC_MECT_enq;

/*FarosPLC Function view for Libraries*/
/* --- 230 -------------------------------------------------------------------- */
void MECT_sread(STDLIBFUNCALL);
/* --- 231 -------------------------------------------------------------------- */
void MECT_H_sread(STDLIBFUNCALL);
/* --- 232 -------------------------------------------------------------------- */
void MECT_swrite(STDLIBFUNCALL);
/* --- 233 -------------------------------------------------------------------- */
void MECT_H_swrite(STDLIBFUNCALL);
/* --- 234 -------------------------------------------------------------------- */
void MECT_get_enquiry(STDLIBFUNCALL);
/* --- 235 -------------------------------------------------------------------- */
void MECT_get_status(STDLIBFUNCALL);
/* --- 236 -------------------------------------------------------------------- */
void MECT_get_flag(STDLIBFUNCALL);

/* FarosPLC Structures with parameters to be sent to the above defined functions*/
/* strstatusutture per passaggio array*/
typedef struct
{
	DEC_VAR(IEC_UINT, uLen);
	DEC_VAR(IEC_CHAR, pElem[2]);
	
}MECT_COMMAND_ARRAY_PARAM;

typedef struct
{
	DEC_FUN_INT(id);
	DEC_FUN_PTR(MECT_COMMAND_ARRAY_PARAM, command);
	DEC_FUN_DINT(data);
	/*return value of the function*/
	DEC_FUN_INT(ret_value);
	
}MECT_ENQUIRY_PARAM;

typedef struct
{
	DEC_FUN_INT(id);
	DEC_FUN_PTR(MECT_COMMAND_ARRAY_PARAM, command);
	DEC_FUN_REAL(value);
	DEC_FUN_INT(data);
	/*return value of the function*/
	DEC_FUN_INT(ret_value);
	
}MECT_FLOAT_STX_PARAM;

typedef struct
{
	DEC_FUN_INT(id);
	DEC_FUN_PTR(MECT_COMMAND_ARRAY_PARAM, command);
	DEC_FUN_UINT(value);
	DEC_FUN_DINT(data);
	/*return value of the function*/
	DEC_FUN_INT(ret_value);
	
}MECT_HEX_STX_PARAM;

typedef struct
{
	/*return value of the function*/
	DEC_FUN_INT(status);

}MECT_GET_STATUS;

typedef struct
{
	/*return value of the function*/
	DEC_FUN_CHAR(flag);

}MECT_GET_FLAG;

typedef struct
{
	/*return value of the function*/
	DEC_FUN_REAL(value);
	
}MECT_GET_ENQ_PARAM;

#endif

