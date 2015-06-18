
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
 * Filename: libHW119.h
 */


#ifndef _LIBHW119_H_
#define _LIBHW119_H_

#if defined(RTS_CFG_HW119_LIB)

#define HW119_MAX_FIELD_NB 16

void hw119_open_cross_table(STDLIBFUNCALL);
void hw119_close_cross_table(STDLIBFUNCALL);
void hw119_read_cross_table_record(STDLIBFUNCALL);
void hw119_get_cross_table_field(STDLIBFUNCALL);
void hw119_get_addr(STDLIBFUNCALL);
void hw119_write_var_bit(STDLIBFUNCALL);
void hw119_write_var_byte(STDLIBFUNCALL);
void hw119_write_var_uint(STDLIBFUNCALL);
void hw119_write_var_int(STDLIBFUNCALL);
void hw119_write_var_udint(STDLIBFUNCALL);
void hw119_write_var_dint(STDLIBFUNCALL);
void hw119_write_var_real(STDLIBFUNCALL);
void hw119_dword2float(STDLIBFUNCALL);
void hw119_float2dword(STDLIBFUNCALL);

#define RTS_PRAGMA_PACK_1	/* >>>> Align 1 Begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_1

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, filename);
	/*return value*/
	DEC_FUN_UINT(ret_value);
}HW119_OPEN_CROSS_TABLE_PARAM;

typedef struct
{
	/*return value*/
	DEC_FUN_UINT(ret_value);
}HW119_CLOSE_CROSS_TABLE_PARAM;

typedef struct
{
	DEC_VAR(IEC_UINT, uLen);
	DEC_VAR(IEC_INT, pElem[HW119_MAX_FIELD_NB]);
	
}HW119_FIELD_ARRAY_PARAM;

typedef struct
{
	DEC_FUN_BOOL(error);
	/*return value*/
	DEC_FUN_UINT(ret_value);
	
}HW119_READ_CROSS_TABLE_RECORD_PARAM;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, field);
	/*return value*/
	DEC_FUN_UINT(ret_value);
}HW119_GET_CROSS_TABLE_FIELD;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, varname);
	/*return value*/
	DEC_FUN_WORD(ret_value);
}HW119_GET_ADDR;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, varname);
	DEC_FUN_UINT(value);
	/*return value*/
	DEC_FUN_UINT(ret_value);
}HW119_WRITE_VAR_UINT;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, varname);
	DEC_FUN_UINT(value);
	/*return value*/
	DEC_FUN_INT(ret_value);
}HW119_WRITE_VAR_INT;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, varname);
	DEC_FUN_UDINT(value);
	/*return value*/
	DEC_FUN_UINT(ret_value);
}HW119_WRITE_VAR_UDINT;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, varname);
	DEC_FUN_UINT(value);
	/*return value*/
	DEC_FUN_DINT(ret_value);
}HW119_WRITE_VAR_DINT;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, varname);
	DEC_FUN_REAL(value);
	/*return value*/
	DEC_FUN_UINT(ret_value);
}HW119_WRITE_VAR_REAL;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, varname);
	DEC_FUN_BOOL(value);
	/*return value*/
	DEC_FUN_UINT(ret_value);
}HW119_WRITE_VAR_BIT;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, varname);
	DEC_FUN_BYTE(value);
	/*return value*/
	DEC_FUN_UINT(ret_value);
}HW119_WRITE_VAR_BYTE;

typedef struct
{
	DEC_FUN_REAL(value);
	/*return value*/
	DEC_FUN_DWORD(ret_value);
}HW119_FLOAT2DWORD_PARAM;

typedef struct
{
	DEC_FUN_DWORD(value);
	/*return value*/
	DEC_FUN_REAL(ret_value);
}HW119_DWORD2FLOAT_PARAM;

#define RTS_PRAGMA_PACK_DEF 	
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_DEF 	/* <<<< Align 1 end <<<<<<<<<<<<<<<<<<<<<<<<<<< */

#endif	/* RTS_CFG_HW119_LIB */

#endif	/* _LIBHW119_H_ */

/* ---------------------------------------------------------------------------- */
