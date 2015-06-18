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
 * Filename: libMectUserUtility.h
 */

#ifndef _MECT_USER_UTILITY_H_
#define _MECT_USER_UTILITY_H_

#include "stdInc.h"
#include "libDef.h"

/* --- 260 -------------------------------------------------------------------- */
void Buzzer(STDLIBFUNCALL);
/* --- 261 -------------------------------------------------------------------- */
void Date_add(STDLIBFUNCALL);
/* --- 262 -------------------------------------------------------------------- */
void Date_diff(STDLIBFUNCALL);
/* --- 263 -------------------------------------------------------------------- */
void DateTimeRead(STDLIBFUNCALL);
/* --- 264 -------------------------------------------------------------------- */
void DateTimeSync(STDLIBFUNCALL);
/* --- 265 -------------------------------------------------------------------- */
void DateTimeWrite(STDLIBFUNCALL);
/* --- 266 -------------------------------------------------------------------- */
void LCD_get_backlight(STDLIBFUNCALL);
/* --- 267 -------------------------------------------------------------------- */
void LCD_set_backlight(STDLIBFUNCALL);

/* FarosPLC Structures with parameters to be sent to the above defined functions*/

typedef struct
{
	DEC_FUN_UINT(duration);
	/*return value*/
	DEC_FUN_UINT(ret_value);

}BUZZER_PARAM;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, initDate);
	DEC_FUN_PTR(IEC_STRING const, format);
	DEC_FUN_DINT(increment);
	DEC_FUN_PTR(IEC_STRING const, finalDate);
	/*return value*/
	DEC_FUN_UINT(ret_value);

}DATA_ADD_PARAM;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, initDate);
	DEC_FUN_PTR(IEC_STRING const, finalDate);
	DEC_FUN_PTR(IEC_STRING const, format);
	DEC_FUN_DINT(diff);
	/*return value*/
	DEC_FUN_UINT(ret_value);

}DATA_DIFF_PARAM;

typedef struct
{
	DEC_VAR(IEC_UINT, uLen);
	DEC_VAR(IEC_UINT, pElem[6]);
}DATE_TIME_ARRAY_PARAM;

typedef struct
{
	DEC_FUN_PTR(DATE_TIME_ARRAY_PARAM, DateTime);
	/*return value*/
	DEC_VAR(IEC_UINT, ret_value);
}RTC_DATE_TIME_PARAM;

typedef struct
{
	/*return value*/
	DEC_VAR(IEC_UINT, ret_value);
}RTC_DATE_TIME_SYNC_PARAM;

typedef struct
{
	DEC_FUN_UINT(level);
	/*return value*/
	DEC_VAR(IEC_UINT, ret_value);

}LCD_BACKLIGHT_PARAM;

#endif

