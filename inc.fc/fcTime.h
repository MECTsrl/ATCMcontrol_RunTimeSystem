
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
 * Filename: fcTime.h
 */


#ifndef _FCTIME_H_
#define _FCTIME_H_

#if defined(FC_CFG_TIME_LIB)

/* DECLARATIONS OF FUNCTIONS
 * ----------------------------------------------------------------------------
 */
void get_datetime(STDLIBFUNCALL);
void datetime_to_string(STDLIBFUNCALL);
void sub_datetime_datetime(STDLIBFUNCALL);
void add_datetime_dint(STDLIBFUNCALL);
void get_utc_datetime(STDLIBFUNCALL);
void set_datetime(STDLIBFUNCALL);
void set_utc_datetime(STDLIBFUNCALL);
void get_datetime2(STDLIBFUNCALL);
void datetime2_to_string(STDLIBFUNCALL);
void sub_datetime2_datetime2(STDLIBFUNCALL);
void add_datetime2_dint(STDLIBFUNCALL);
void get_utc_datetime2(STDLIBFUNCALL);

/* typedefs used by Time library 
 * ----------------------------------------------------------------------------
 */
typedef struct DATETIME_TAG
{
	DEC_VAR(IEC_INT,  sec);   /* seconds		  [0..59]			 */
	DEC_VAR(IEC_INT,  min);   /* minutes		  [0..59]			 */ 
	DEC_VAR(IEC_INT,  hour);  /* hours			  [0..23]			 */
	DEC_VAR(IEC_INT,  mday);  /* day of the month [1..31]			 */
	DEC_VAR(IEC_INT,  mon);   /* month			  [1..12]			 */
	DEC_VAR(IEC_INT,  year);  /* year								 */
	DEC_VAR(IEC_INT,  wday);  /* day of the week  [1..7]  1 = Monday */
	DEC_VAR(IEC_INT,  yday);  /* day in the year  [1..366]			 */
	DEC_VAR(IEC_BOOL, isdst); /* is daylight saving time			 */
} DATETIME;

typedef struct GET_DATETIME_PAR_TAG
{
		/* output */
		DEC_FUN_PTR(DATETIME, t); /* current time of the target */
		/* result */
		DEC_FUN_BOOL(res);		  /* return value: TRUE if time successfully retrived, 
					 otherwise: FALSE  */
} GET_DATETIME_PAR;

typedef struct DATETIME_TO_STRING_PAR_TAG
{
		/* input */
		DEC_FUN_PTR(DATETIME, t); 
		/* result */
	/* return value: string representation of T
	 * on valid DATETIME parameter the string is empty */
		DEC_FUN_PTR(IEC_STRMAX, res);		  
} DATETIME_TO_STRING_PAR;

typedef struct SUB_DATETIME_DATETIME_PAR_TAG
{
		/* input */
		DEC_FUN_PTR(DATETIME, t1);	/* substracts t2 from t1 */
		DEC_FUN_PTR(DATETIME, t2); 
		/* output */
		DEC_FUN_PTR(IEC_DINT, res); /* t1 - t2 in seconds */
		/* result */
		DEC_FUN_BOOL(res_flag); 
} SUB_DATETIME_DATETIME_PAR;

typedef struct ADD_DATETIME_DINT_PAR_TAG
{
		/* input */
		DEC_FUN_PTR(DATETIME, t1);	/* add t2 sec to t1 */
		DEC_FUN_DINT(i2); 
		/* output */
		DEC_FUN_PTR(DATETIME, res); /* t1 + t2(in sec) */
		/* result */
		DEC_FUN_BOOL(res_flag); 
} ADD_DATETIME_DINT_PAR;

typedef struct GET_UTC_DATETIME_PAR_TAG
{
		/* output */
		DEC_FUN_PTR(DATETIME, t); /* current time of the target */
		/* result */
		DEC_FUN_BOOL(res);		  /* return value: TRUE if time successfully retrived, 
					 otherwise: FALSE  */
} GET_UTC_DATETIME_PAR;

typedef struct SET_DATETIME_PAR_TAG
{
		/* output */
		DEC_FUN_PTR(DATETIME, t); /* current time of the target */
		/* input */
		DEC_FUN_BOOL(rtc);		  /* current time of the target */
		/* result */
		DEC_FUN_BOOL(res);		  /* return value: TRUE if time successfully retrived, 
					 otherwise: FALSE  */
} SET_DATETIME_PAR;

typedef struct SET_UTC_DATETIME_PAR_TAG
{
		/* output */
		DEC_FUN_PTR(DATETIME, t); /* current time of the target */
		/* input */
		DEC_FUN_BOOL(rtc); /* current time of the target */
		/* result */
		DEC_FUN_BOOL(res);		  /* return value: TRUE if time successfully retrived, 
					 otherwise: FALSE  */
} SET_UTC_DATETIME_PAR;

typedef struct DATETIME2_TAG
{
	DEC_VAR(IEC_INT,  msec);  /* miliseconds	  [0..999]			 */
	DEC_VAR(IEC_INT,  sec);   /* seconds		  [0..59]			 */
	DEC_VAR(IEC_INT,  min);   /* minutes		  [0..59]			 */ 
	DEC_VAR(IEC_INT,  hour);  /* hours			  [0..23]			 */
	DEC_VAR(IEC_INT,  mday);  /* day of the month [1..31]			 */
	DEC_VAR(IEC_INT,  mon);   /* month			  [1..12]			 */
	DEC_VAR(IEC_INT,  year);  /* year								 */
	DEC_VAR(IEC_INT,  wday);  /* day of the week  [1..7]  1 = Monday */
	DEC_VAR(IEC_INT,  yday);  /* day in the year  [1..366]			 */
	DEC_VAR(IEC_BOOL, isdst); /* is daylight saving time			 */
} DATETIME2;

typedef struct GET_DATETIME2_PAR_TAG
{
		/* output */
		DEC_FUN_PTR(DATETIME2, t); /* current time of the target */
		/* result */
		DEC_FUN_BOOL(res);		  /* return value: TRUE if time successfully retrived, 
					 otherwise: FALSE  */
} GET_DATETIME2_PAR;

typedef struct DATETIME2_TO_STRING_PAR_TAG
{
		/* input */
		DEC_FUN_PTR(DATETIME2, t); 
		/* result */
	/* return value: string representation of T
	 * on valid DATETIME parameter the string is empty */
		DEC_FUN_PTR(IEC_STRMAX, res);		  
} DATETIME2_TO_STRING_PAR;

typedef struct SUB_DATETIME2_DATETIME2_PAR_TAG
{
		/* input */
		DEC_FUN_PTR(DATETIME2, t1);  /* substracts t2 from t1 */
		DEC_FUN_PTR(DATETIME2, t2); 
		/* output */
		DEC_FUN_PTR(IEC_DINT, res_sec); /* t1 - t2 seconds part */
		DEC_FUN_PTR(IEC_INT, res_msec); /* t1 - t2 miliseconds part*/
		/* result */
		DEC_FUN_BOOL(res_flag); 
} SUB_DATETIME2_DATETIME2_PAR;

typedef struct ADD_DATETIME2_DINT_PAR_TAG
{
		/* input */
		DEC_FUN_PTR(DATETIME2, t1);  /* add t2 sec to t1 */
		DEC_FUN_DINT(i2_sec); 
		DEC_FUN_INT(i2_msec); 
		/* output */
		DEC_FUN_PTR(DATETIME2, res); /* t1 + i2(in msec) */
		/* result */
		DEC_FUN_BOOL(res_flag); 
} ADD_DATETIME2_DINT_PAR;

typedef struct GET_UTC_DATETIME2_PAR_TAG
{
		/* output */
		DEC_FUN_PTR(DATETIME2, t); /* current time of the target */
		/* input */
		DEC_FUN_BOOL(rtc); /* current time of the target */
		/* result */
		DEC_FUN_BOOL(res);		  /* return value: TRUE if time successfully retrived, 
					 otherwise: FALSE  */
} GET_UTC_DATETIME2_PAR;

#endif /* FC_CFG_TIME_LIB */

#endif /* _FCTIME_H_ */

/* ---------------------------------------------------------------------------- */
