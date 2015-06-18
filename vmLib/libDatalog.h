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
 * Filename: libDatalog.h
 */

#ifndef DATALOG_H__
#define DATALOG_H__

#include "stdInc.h"
#include "libDef.h"
#include <time.h>


typedef struct app_datalog_list_s {

	char label[20];
	unsigned short int occurence;
	FILE *fp;
	unsigned short int log_num;
	struct app_datalog_list_s *next;
}app_datalog_list_t;
 
typedef struct app_datalog_s {

	time_t milliseconds;
	unsigned int sys_time;	
	int status;
	int dl_index;
	int lenght;
	unsigned short usbkey_index;
	char dir[40];
	app_datalog_list_t *list;

}app_datalog_t;	


/*
 * Global variabiles
 */
extern app_datalog_t datalog_data;

/* Report information from the retentive drive to signal a poweroff event*/
unsigned int *app_datalog_poweroff_mgmt;


/*
 * Funtion prototypes
 */
int app_datalog_start(unsigned int buffer_lenght, unsigned short buffer_number, unsigned short index );
short app_datalog_capture(char *label, float value);
unsigned int app_datalog_stop(void);
short app_datalog_eject(unsigned short index);

/*FarosPLC Function view for Libraries*/
/* --- 250 -------------------------------------------------------------------- */
void Datalog_start(STDLIBFUNCALL);
/* --- 251 -------------------------------------------------------------------- */
void Datalog_get(STDLIBFUNCALL);
/* --- 252 -------------------------------------------------------------------- */
void Datalog_stop(STDLIBFUNCALL);
/* --- 253 -------------------------------------------------------------------- */
void Datalog_eject(STDLIBFUNCALL);

/* FarosPLC Structures with parameters to be sent to the above defined functions*/

typedef struct
{
	DEC_FUN_UDINT(buffer_lenght);
	DEC_FUN_UINT(buffer_number);
	DEC_FUN_UINT(index);
	/*return value*/
	DEC_FUN_UINT(ret_value);
	
}DATALOG_START_PARAM;

typedef struct
{
	/*return value*/
	DEC_FUN_UINT(ret_value);
	
}DATALOG_STOP_PARAM;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, label);
        DEC_VAR(IEC_REAL, value);
	/*return value*/
        DEC_VAR(IEC_UDINT, ret_value);
}DATALOG_GET_PARAM;

typedef struct
{
	DEC_FUN_UINT(index);
	/*return value*/
	DEC_FUN_UINT(ret_value);
	
}DATALOG_EJECT_PARAM;

#endif

