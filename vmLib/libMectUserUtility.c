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
 * Filename: libMectUserUtility.c
 */

#include "libMectUserUtility.h"
#include "kpdMain.h"
#include <time.h>
#include <sys/time.h>
#include <assert.h>

//#define DBGMECTUTIL

#define MAX_LABEL_LEN  1000
#define TIME_YEAR_BASE 2000

#ifndef is_leap
# define is_leap(year)  \
	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#endif

/* How many days come before each month (0-12).  */
const unsigned short int total_day_in_month[2][13] =
{
	/* Normal years.  */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	/* Leap years.  */
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

/* How many days are in each month (0-12).  */
const unsigned short int day_in_month[2][13] =
{
	/* Normal years.  */
	{ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	/* Leap years.  */
	{ 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

enum app_data_section_e {
	APP_DATA_DAY = 0,
	APP_DATA_WEEK,
	APP_DATA_MONTH,
	APP_DATA_YEAR,
	APP_DATA_NONE,
	APP_DATA_LAST_ELEM
};

/* Date/Time vector indexes */
enum DT_e {
	DT_YEAR = 0,
	DT_MONTH,
	DT_DAY,
	DT_HOUR,
	DT_MINUTE,
	DT_SECOND,

	DT_LAST_ENTRY
};

/**
 * convert a date from a DateTime format to a tm format
 *
 * @return      0  - OK
 *              !0 - error
 */
int tm2DateTime(struct tm * time_tm, int * DateTime)
{
	DateTime[DT_YEAR] = time_tm->tm_year + 1900;
	DateTime[DT_MONTH] = time_tm->tm_mon + 1;
	DateTime[DT_DAY] = time_tm->tm_mday;
	DateTime[DT_HOUR] = time_tm->tm_hour;
	DateTime[DT_MINUTE] = time_tm->tm_min;
	DateTime[DT_SECOND] = time_tm->tm_sec;

	return 0;
}

/**
 * convert a date from a tm format to a DateTime format
 *
 * @return      0  - OK
 *              !0 - error
 */
int DateTime2tm(struct tm * time_tm, int * DateTime)
{
	time_tm->tm_year = DateTime[DT_YEAR] - 1900; 
	time_tm->tm_mon = DateTime[DT_MONTH] - 1; 
	time_tm->tm_mday = DateTime[DT_DAY]; 
	time_tm->tm_hour = DateTime[DT_HOUR]; 
	time_tm->tm_min = DateTime[DT_MINUTE]; 
	time_tm->tm_sec = DateTime[DT_SECOND]; 
	return 0;
}

/**
 * Extract year, mont, day as integer from a string
 * containing the date in format DD-MM-YYYY
 *
 * @param  char * date      - string with date in format DD-MM-YYYY
 * @param  char * separator - separator between day, month, year ("-" in the exxample)
 * @param  char * year      - integer where the extracted year will be put
 * @param  char * month     - integer where the extracted month will be put
 * @param  char * day       - integer where the extracted day will be put
 *
 * @return 0  - OK
 *         <0 - error
 */
int strdate2array(char * date, char * separator, int *year, int *month, int *day)
{
	char *date_dup = strdup(date);
	char * l;

	l = strtok(date_dup, separator);
	if (l == NULL)
	{
		free(date_dup);
		return -1;
	}
	*day = (int) strtol(l, (char **)NULL, 10);

	l = strtok(NULL, separator);
	if (l == NULL)
	{
		free(date_dup);
		return -2;
	}
	*month = (int) strtol(l, (char **)NULL, 10);

	l = strtok(NULL, separator);
	if (l == NULL)
	{
		free(date_dup);
		return -3;
	}
	*year = (int) strtol(l, (char **)NULL, 10);

	free(date_dup);
	return 0;
}

/**
 * Convert the given number of days in a date startin from time base year
 * 
 * @param       int totaldays - days to be converted in a date
 * @param       int start_day - start day to which add totaldays to
 * @param       int start_month - start month to which add totaldays to
 *
 * @return      
 * 
 * @ingroup datacal
 */

	static void
app_datacal_totaldays_to_currday(int totaldays, int start_day, int start_month, int *year, int*month, int *day)
{
	int current_year = TIME_YEAR_BASE;
	int i;

	while(totaldays != 0 && totaldays >= 365){
		totaldays -= total_day_in_month[is_leap(current_year)][12];
		current_year +=1;
	}
	if (totaldays == 0){
		*year = current_year;
		*month = start_month;
		*day = start_day;
		return;
	}
	else {
		*year = current_year;
		i = 0;
		while(i < 12) {
			if ( totaldays > total_day_in_month[is_leap(current_year)][i] )
				i++;
			else
				break;
		}
		*month = i;
		totaldays -= total_day_in_month[is_leap(current_year)][i-1];
		*day = totaldays;
		return;
	}

}

/**
 * Convert current date in days starting from time base year
 * 
 * @param       int day - current day
 * @param       int month - current month 
 * @param       int year - current year
 *
 * @return      int total_days - current date converted in days
 * 
 * @ingroup datacal
 */

	static int
app_datacal_currday_to_totaldays(int day, int month, int year)
{
	int current_year;
	int total_days = 0;

	/*Calculting how many days there are between supplied year and base*/
	for(current_year = TIME_YEAR_BASE; current_year < year; current_year++ )
		total_days += total_day_in_month[is_leap(current_year)][12];

	/*Calculating how many days there are between starting of current year and current month*/
	total_days += total_day_in_month[is_leap(current_year)][month-1];

	total_days += day;

	return total_days;

}

/**
 * Add, based on a parameter the given number of days-week-month or year to a start date
 * 
 * @param	int val - value to be added to start date
 * @param	char *start - starting date (DD-MM-YYYY)
 * @param	char *format - "D" for days, "W" for weeks, "M" for months and "Y" for years 
 *
 * @return  	0 done 
 *		!0 error
 *
 * @ingroup datacal
 */

int app_datacal_dateadd(char *format, char *start, char * final, int val)
{
	int final_day;
	int final_month;
	int final_year;
	int day_value;
	int month_value;
	int year_value;
	int added_days = 0;
	int total_days = 0;

	if (strdate2array(start, "-", &year_value, &month_value, &day_value) != 0)
	{
		fprintf(stderr, "[%s] - Start date '%s' must be 'DD-MM-YYYY'\n", __func__, start);
		return -1;
	}

	if (day_value == 0 || month_value == 0 || year_value == 0 )
	{
		fprintf(stderr, "[%s] - empty value: day_value %d, month_value %d, year_value %d\n", __func__, day_value, month_value, year_value);
		return -2;
	}

	if (year_value < TIME_YEAR_BASE ||  (month_value < 1 || month_value > 12) || (day_value < 1 || day_value > day_in_month[is_leap(year_value)][month_value]))	
	{
		fprintf(stderr, "[%s] - invalid value: day_value %d, month_value %d, year_value %d\n", __func__, day_value, month_value, year_value);
		return -3;
	}
	unsigned section = APP_DATA_NONE;

	if (strstr(format, "D") == format)
		section = APP_DATA_DAY;
	else if (strstr(format, "W") == format)
		section = APP_DATA_WEEK;
	else if (strstr(format, "M") == format)
		section = APP_DATA_MONTH;
	else if (strstr(format, "Y") == format)
		section = APP_DATA_YEAR;
	else
		fprintf(stderr, "[%s] Unknown kind of format to convert from\n", __func__);

	switch(section){
		case APP_DATA_YEAR:
			if (val == 0)
				added_days = 0; 
			while (val != 0){
				added_days += total_day_in_month[is_leap(year_value + val)][12];
				val--; 
			}
			total_days = added_days + app_datacal_currday_to_totaldays(day_value, month_value, year_value);
			app_datacal_totaldays_to_currday(total_days, day_value, month_value, &final_year, &final_month, &final_day);
			break;

		case APP_DATA_MONTH:
			final_month = ((((month_value + val)%12)==0)?12:((month_value + val)%12));

			final_year = year_value + (month_value + val)/12 - ((((month_value + val)%12)==0)?1:0);

			if(day_value == 29 && month_value == 2 && !is_leap(final_year) && final_month == 2)
				final_day = day_value-1;
			else
				final_day = day_value;	
			break;	

		case APP_DATA_WEEK:
			added_days = val*7;
			total_days = added_days + app_datacal_currday_to_totaldays(day_value, month_value, year_value);
			app_datacal_totaldays_to_currday(total_days, day_value, month_value, &final_year, &final_month, &final_day);
			break;

		case APP_DATA_DAY:
			added_days = val;
			total_days = added_days + app_datacal_currday_to_totaldays(day_value, month_value, year_value);
			app_datacal_totaldays_to_currday(total_days, day_value, month_value, &final_year, &final_month, &final_day);
			break;

		case APP_DATA_NONE:
		default:
			return -4;

	}

	/* 'DD-MM-YYYY' */
	sprintf(final, "%02d-%02d-%4d", final_day, final_month, final_year);
#ifdef DBGMECTUTIL
	fprintf(stderr, "[%s] Date '%s'\n", __func__, final);
#endif

	return 0;

}

/**
 * Leave or Convert the given number of days in weeks, months or year
 * 
 * @param	int diffdays - days to be converted in a specified format
 * @param	int end_year - end year w.r.t. we are doing the conversion
 * @param	int start_year - startyear w.r.t. we are doing the conversion
 * @param	char *format - "D" for days, "W" for weeks, "M" for months and "Y" for years 
 *
 * @return  	0 for error 
 *		> 0 for conversion with start_date before end_date
 * 		< 0 for conversion with start_date after end_date
 *
 * @ingroup datacal
 */

	static int 
app_datacal_diffdays_to_formatted_diff(int diffdays, int end_year, int start_year, int start_month, char *format)
{

	int current_year = 0;
	int converted_year = 0;
	int converted_month = 0;
	int column = 0;
	int flag = 1;
	int neg_diffdays = 0;

	unsigned section = APP_DATA_NONE;

	if (strstr(format, "D") == format)
		section = APP_DATA_DAY;
	else if (strstr(format, "W") == format)
		section = APP_DATA_WEEK;
	else if (strstr(format, "M") == format)
		section = APP_DATA_MONTH;
	else if (strstr(format, "Y") == format)
		section = APP_DATA_YEAR;
	else
		fprintf(stderr, "%s: Unknown kind of format to convert from\n", __func__);	


	if (diffdays < 0){
		diffdays--; /*This is necessary to include the starting day*/
		neg_diffdays = diffdays;
		diffdays = abs(diffdays);
		flag = 0;
	}	
	else 
		diffdays++; /*This is necessary to include the starting day*/
	switch(section){

		case APP_DATA_YEAR:  if (diffdays < total_day_in_month[is_leap(start_year)][12] && !is_leap(start_year) && !is_leap(end_year))
								 return 0;
							 else if (diffdays < total_day_in_month[is_leap(start_year)][12] && (is_leap(start_year) || is_leap(end_year))) {
								 if (start_year < end_year)
									 return 1;
								 else
									 return -1;	
							 }
							 else {
								 while(diffdays != 0 && diffdays >= 364){
									 diffdays -= total_day_in_month[is_leap(current_year + start_year)][12];
									 current_year ++;
									 if(flag)
										 converted_year++;
									 else
										 converted_year--;	

								 }
								 return converted_year;
							 }	


		case APP_DATA_MONTH: if (diffdays < day_in_month[is_leap(start_year)][start_month] || diffdays == 0)
								 return 0;
							 else {
								 column = start_month;
								 while(diffdays != 0 && diffdays >= day_in_month[is_leap(start_year)][column %13]){

									 diffdays -= day_in_month[is_leap(start_year)][column%13];
									 column++;
									 if (flag)
										 converted_month++;
									 else
										 converted_month--;		
									 if ( (column %13) == 0) {
										 start_year ++;
										 column ++;
									 }


								 }
								 return converted_month;		 
							 }

		case APP_DATA_DAY:
							 if(flag)
								 return diffdays;
							 else 
								 return neg_diffdays;	
		case APP_DATA_WEEK:
							 if(flag)	
								 return diffdays/7;
							 else
								 return neg_diffdays/7;	
		case APP_DATA_NONE:
							 return 0;
		default:
							 return 0;

	}
}

/**
 * Calculate the difference of days-weeks-months- or year between to dates based on the given format
 * 
 * @param	char *end - end date (DD-MM-YYYY)
 * @param	char *start - starting date (DD-MM-YYYY)
 * @param	char *format - "D" for days, "W" for weeks, "M" for months and "Y" for years 
 *
 * @return  	0 ok
 *		!0 error
 *
 * @ingroup datacal
 */
	static int 
app_datacal_datediff (char *start, char *end, char *format, int * value)
{
	int start_day_val;
	int start_month_val;
	int start_year_val;
	int end_day_val;
	int end_month_val;
	int end_year_val;
	int total_day_initial;
	int total_day_final;

	if (strdate2array(start, "-", &start_year_val, &start_month_val, &start_day_val) != 0)
	{
		fprintf(stderr, "[%s] - Start date '%s' must be 'DD-MM-YYYY'\n", __func__, start);
		return -1;
	}

	if (strdate2array(end, "-", &end_year_val, &end_month_val, &end_day_val) != 0)
	{
		fprintf(stderr, "[%s] - End date '%s' must be 'DD-MM-YYYY'\n", __func__, end);
		return -1;
	}

	if (start_day_val == 0 || start_month_val == 0 || start_year_val == 0 )
	{
		fprintf(stderr, "[%s] - empty value: day_value %d, month_value %d, year_value %d\n", __func__, start_day_val, start_month_val, start_year_val);
		return -2;
	}

	if (end_day_val != 0 && end_month_val != 0 && end_year_val != 0 && start_day_val != 0 && start_month_val != 0 && start_year_val != 0 ) {

		if (start_year_val > TIME_YEAR_BASE &&  (start_month_val > 1 && start_month_val < 12) && (start_day_val > 1 &&
					start_day_val < day_in_month[is_leap(start_year_val)][start_month_val]) && end_year_val > TIME_YEAR_BASE && (end_month_val
					> 1 && end_month_val < 12) && (end_day_val > 1 && end_day_val < day_in_month[is_leap(end_year_val)][end_month_val])) {	

			total_day_initial = app_datacal_currday_to_totaldays(start_day_val, start_month_val, start_year_val);
			total_day_final = app_datacal_currday_to_totaldays(end_day_val, end_month_val, end_year_val);

			*value = app_datacal_diffdays_to_formatted_diff(total_day_final-total_day_initial, end_year_val, start_year_val, start_month_val, format);
			if (*value == 0)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}
		else
			return -1;
	}
	else {
		fprintf(stderr, "%s: Date must be DD-MM-YYYY\n",__func__);
		return -2;
	}

}

/**
 * Update the PLC vars with system current date and time
 *
 * @return      0 - OK
 *              1 - error getting system time/date
 */
int app_date_time_read(int * DateTime)
{
	time_t rt = 0;
	struct tm *pt = NULL;

	rt = time(NULL);
	pt = localtime(&rt);

	if (pt == NULL) {
		fputs(__func__, stderr);
		perror(": while getting local time");
		fflush(stderr);

		return 1;
	}

	return tm2DateTime(pt, DateTime);
}

/**
 * Update the system current date and time from the PLC vars
 *
 * @return      0 - OK
 *              1 - error getting system time/date
 *              2 - error converting time/date
 *              3 - error setting time/date
 *              4 - error updating real time clock
 */
int app_date_time_write(int * DateTime)
{
	time_t rt = 0;
	struct tm *pt = NULL;
	struct timezone timez;
	struct timeval temp;
	int rc = 0;

	rt = time(NULL);
	pt = localtime(&rt);
	if (pt == NULL) {
		fputs(__func__, stderr);
		perror(": while getting local time");
		fflush(stderr);

		return 1;
	}

	DateTime2tm(pt, DateTime);

	rc = gettimeofday(&temp, &timez);
	if (rc < 0) {
		fputs(__func__, stderr);
		perror(": while getting time of day");
		fflush(stderr);

		return 2;
	}

	temp.tv_sec = mktime(pt);
	temp.tv_usec = 0;

	rc = settimeofday(&temp, &timez);
	if (rc < 0) {
		fputs(__func__, stderr);
		perror(": while setting time of day");
		fflush(stderr);

		return 3;
	}

	rc = system("/sbin/hwclock -wu");        /* Update RTC from system */
	if (rc < 0) {
		fputs(__func__, stderr);
		perror(": while updating real time clock");
		fflush(stderr);

		return 4;
	}

#ifdef DBGMECTUTIL
	printf("[%s] - RTC from system updated\n", __func__);
#endif

	return 0;
}

#define LCD_FS_ROOT           "/sys/devices/platform/mxs-bl.0/backlight/mxs-bl/"
#define LCD_ACTUAL_BRIGHTNESS LCD_FS_ROOT "actual_brightness"
#define LCD_MAX_BRIGHTNESS    LCD_FS_ROOT "max_brightness"
#define LCD_BRIGHTNESS        LCD_FS_ROOT "brightness"
#define LCD_POWER             LCD_FS_ROOT "bl_power"
#define LCD_POWER_ON  0
#define LCD_POWER_OFF 1

int app_lcd_get_backlight(int * brightness)
{
	FILE * bklghtfp;
	int dummy;

	bklghtfp = fopen(LCD_ACTUAL_BRIGHTNESS, "r");
	if (bklghtfp == NULL)
	{
		fprintf(stderr, "%s: cannot get the actual brightness from file '%s': [%s].\n",
				__func__, LCD_ACTUAL_BRIGHTNESS, strerror(errno));
		return 1;
	}
	dummy = fscanf(bklghtfp, "%d", &brightness);
	fclose(bklghtfp);

	return 0;
}

int app_lcd_set_backlight(int brightness)
{
	FILE * bklghtfp;
	static int max_brightness = -1;
	int dummy;

	if (max_brightness < 0)
	{
		bklghtfp = fopen(LCD_MAX_BRIGHTNESS, "r");
		if (bklghtfp == NULL)
		{
			fprintf(stderr, "%s: cannot get the max brightness from file '%s': [%s].\n",
					__func__, LCD_ACTUAL_BRIGHTNESS, strerror(errno));
			return 1;
		}
		dummy = fscanf(bklghtfp, "%d", &max_brightness);
		fclose(bklghtfp);
#ifdef DBGMECTUTIL
		printf("[%s] - max_brightness: %d\n", __func__, max_brightness);
#endif
	}

	if (brightness < 0 || brightness > max_brightness)
	{
		fprintf(stderr, "%s: cannot set the brightness [%d]: out of range (%d - %d).\n",
				__func__, brightness, 0, max_brightness);
		return 2;
	}

	bklghtfp = fopen(LCD_BRIGHTNESS, "w");
	if (bklghtfp == NULL)
	{
		fprintf(stderr, "%s: cannot set the max brightness into file '%s': [%s].\n",
				__func__, LCD_BRIGHTNESS, strerror(errno));
		return 3;
	}
	fprintf(bklghtfp, "%d\n", brightness);
	fclose(bklghtfp);

	return 0;
}

/*
 * FarosPLC view
 */

void Buzzer(STDLIBFUNCALL)
{
#ifdef RTS_CFG_IOKEYPAD
	BUZZER_PARAM OS_SPTR *pPara = (BUZZER_PARAM OS_SPTR *)pIN;
#ifdef DBGMECTUTIL
	printf("[%s] - duration %d\n", __func__, pPara->duration);
#endif
	pPara->ret_value = ioctl(Buzzerfd, BUZZER_BEEP, pPara->duration);
#endif
}

void Date_add(STDLIBFUNCALL)
{
	char format[MAX_LABEL_LEN];
	char start[MAX_LABEL_LEN];
	char final[MAX_LABEL_LEN];
	DATA_ADD_PARAM OS_SPTR *pPara = (DATA_ADD_PARAM OS_SPTR *)pIN;

	/* TODO: allocate it dinamically */
	assert((pPara->initDate->CurLen) < MAX_LABEL_LEN);
	assert((pPara->format->CurLen) < MAX_LABEL_LEN);

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->initDate, start);
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->format, format);
#ifdef DBGMECTUTIL
	printf("[%s] - format '%s', start '%s', increment %d\n",
			__func__, format, start, pPara->increment);
#endif
	pPara->ret_value = app_datacal_dateadd(format, start, final, pPara->increment);
	utilAnsiToIec(final, (IEC_STRING OS_LPTR *)pPara->finalDate);
#ifdef DBGMECTUTIL
	printf("[%s] - final '%s'\n", __func__, final);
#endif
}

void Date_diff(STDLIBFUNCALL)
{
	char format[MAX_LABEL_LEN];
	char start[MAX_LABEL_LEN];
	char final[MAX_LABEL_LEN];
	DATA_DIFF_PARAM OS_SPTR *pPara = (DATA_DIFF_PARAM OS_SPTR *)pIN;

	/* TODO: allocate it dinamically */
	assert((pPara->initDate->CurLen) < MAX_LABEL_LEN);
	assert((pPara->finalDate->CurLen) < MAX_LABEL_LEN);
	assert((pPara->format->CurLen) < MAX_LABEL_LEN);

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->initDate, start);
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->finalDate, final);
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->format, format);
#ifdef DBGMECTUTIL
	printf("[%s] - format '%s', start '%s', end '%s'\n",
			__func__, format, start, final);
#endif
	pPara->ret_value = app_datacal_datediff(start, final, format, &(pPara->diff));
#ifdef DBGMECTUTIL
	printf("[%s] - diff %d\n", __func__, pPara->diff);
#endif
}

void DateTimeRead(STDLIBFUNCALL)
{
	RTC_DATE_TIME_PARAM OS_SPTR *pPara = (RTC_DATE_TIME_PARAM OS_SPTR *)pIN;
	DATE_TIME_ARRAY_PARAM OS_DPTR *pDateTime = (DATE_TIME_ARRAY_PARAM OS_DPTR* )pPara->DateTime;
	int DateTime[6];
	int i;

#ifdef DBGMECTUTIL
	printf("[%s]\n", __func__);
#endif
	pPara->ret_value = app_date_time_read(DateTime);
	for (i = 0; i < 6; i++)
	{
		*((IEC_UINT *)(pDateTime->pElem + i)) = DateTime[i];
	}
#ifdef DBGMECTUTIL
	printf("[%s] - year %d, month %d, day %d, hour %d, minute %d, second %d\n",
			__func__, 
			*((IEC_UINT *)(pDateTime->pElem + 0)),
			*((IEC_UINT *)(pDateTime->pElem + 1)),
			*((IEC_UINT *)(pDateTime->pElem + 2)),
			*((IEC_UINT *)(pDateTime->pElem + 3)),
			*((IEC_UINT *)(pDateTime->pElem + 4)),
			*((IEC_UINT *)(pDateTime->pElem + 5)));
#endif
}

void DateTimeSync(STDLIBFUNCALL)
{
	RTC_DATE_TIME_SYNC_PARAM OS_SPTR *pPara = (RTC_DATE_TIME_SYNC_PARAM OS_SPTR *)pIN;
#ifdef DBGMECTUTIL
	printf("[%s]\n", __func__);
#endif
	pPara->ret_value = system("/sbin/hwclock -su");        /* Update system from RTC */
}

void DateTimeWrite(STDLIBFUNCALL)
{
	RTC_DATE_TIME_PARAM OS_SPTR *pPara = (RTC_DATE_TIME_PARAM OS_SPTR *)pIN;
	DATE_TIME_ARRAY_PARAM OS_DPTR *pDateTime = (DATE_TIME_ARRAY_PARAM OS_DPTR* )pPara->DateTime;
	int DateTime[6];
	int i;

#ifdef DBGMECTUTIL
	printf("[%s] - year %d, month %d, day %d, hour %d, minute %d, second %d\n",
			__func__, 
			*((IEC_UINT *)(pDateTime->pElem + 0)),
			*((IEC_UINT *)(pDateTime->pElem + 1)),
			*((IEC_UINT *)(pDateTime->pElem + 2)),
			*((IEC_UINT *)(pDateTime->pElem + 3)),
			*((IEC_UINT *)(pDateTime->pElem + 4)),
			*((IEC_UINT *)(pDateTime->pElem + 5)));
#endif
	for (i = 0; i < 6; i++)
	{
		DateTime[i] = *((IEC_UINT *)(pDateTime->pElem + i));
	}
	pPara->ret_value = app_date_time_write(DateTime);
#ifdef DBGMECTUTIL
	printf("[%s] - exit\n", __func__);
#endif
}

void LCD_get_backlight(STDLIBFUNCALL)
{
	int level;
	LCD_BACKLIGHT_PARAM OS_SPTR *pPara = (LCD_BACKLIGHT_PARAM OS_SPTR *)pIN;
#ifdef DBGMECTUTIL
	printf("[%s]\n", __func__);
#endif
	pPara->ret_value = app_lcd_get_backlight(&level);
	pPara->level = level;
}

void LCD_set_backlight(STDLIBFUNCALL)
{
	LCD_BACKLIGHT_PARAM OS_SPTR *pPara = (LCD_BACKLIGHT_PARAM OS_SPTR *)pIN;
#ifdef DBGMECTUTIL
	printf("[%s] - level %d\n", __func__, pPara->level);
#endif
	pPara->ret_value = app_lcd_set_backlight(pPara->level);
}

