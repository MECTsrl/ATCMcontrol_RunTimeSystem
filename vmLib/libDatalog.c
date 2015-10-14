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
 * Filename: libDatalog.c
 */

//#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
 
#include "libDatalog.h"
#include "libUSB.h"
#define DLDEBUG 0


#define DATALOG_UNIT			50 /* max dim for each stored single log file*/
#define DATALOG_FILENAME_LENGHT		70 /* 18 char for path 10 char for number id, 1 char for dot, 20 char for label + 10 for safety*/
#define DATALOG_TIME_LENGHT		20 /* time stamp lenght*/
#define DATALOG_NUM_LENGHT		10 /* max lenght of numeric id of the log*/
#define DATALOG_LOCAL_BUFFER		512 /*Max dimension allowed for local data buffer available on flash device under /local/datalog*/		
#define DATALOG_MAX_FILE_LENGHT		65536 /*Max numeber of file per directory, and max allowed file lenght*/
#define DATALOG_POWEROFF_MASK	0x00010000 /*Check the bit in the app_datalog_poweroff_mgmt value that reports a power off event*/
#define DATALOG_POWEROFF_ACTIVE	0x00100000 /*Check the bit in the app_datalog_poweroff_mgmt value that signal a datalog running*/

/*
 * Global variabiles
 */
unsigned char SystemHalt = 0;

app_datalog_t datalog_data = {
	.milliseconds = 0,
	.sys_time = 0,
	.status = 0,
	.dl_index = 0,
	.lenght = 0,
	.usbkey_index = 0,
	.dir[0]="\0",
	.list = NULL,
};

/*
 * Local functions
 */
 
static void app_datalog_timestamp(char *timestamp_data);
static app_datalog_list_t * app_datalog_label_search( char *label, app_datalog_list_t **head );
static app_datalog_list_t * app_datalog_label_insert( char *label, app_datalog_list_t **head );
#if 0
static void app_datalog_label_sort(app_datalog_list_t **head, app_datalog_list_t **head_sorted);
static void app_datalog_label_delete ( app_datalog_list_t **head );
#endif

static short int app_datalog_no_get = 1; /*does not permit get during stop operations nor before start complete execution*/
static short int app_datalog_local = 0; /* when 1 allow logging on flash device with a max allowed buffer of 512Kb */

/**
 * Unmount USB key
 *
 * @return               0 - OK
 *			>0 - Cannot unmount USB device
 *			
 * @ingroup datalog
 */

short app_datalog_eject(unsigned short index)
{
	short rv= 0;
	rv = (short)app_usb_umount(index);
	
	return rv;

}

/**
 * Initialize datalog infrastructure 
 * 
 * @param	unsigned int buffer_lenght - lenght of the buffer for a single kind of data
 * @param 	unsigned short buffer_number - number of buffer that shoud be allocated (1 for each kind of data to be recorded)
 * @param	unsigned short index - index to access the usbkey on which the data should be stored
 					if set to 0 allow datalogging on the internal flash with a max allowed buffer
					of 512 kb.
 *
 * @return  	-1 if success, > 0 (actually the available space for log on the usbkey) if the requested buffer is bigger
 *		then currently available space, < -1 for error code.
 *	 
 * @error	-2 usb device cannot be mounted
 * @error	-3 cannot grab free space on the usbkey
 * @error	-4 cannot create directory to store datalog results.
 * 
 * @ingroup datalog
 */


int app_datalog_start(unsigned int buffer_lenght, unsigned short buffer_number, unsigned short index )
{	
	unsigned short rv = 0;
	FILE *fp = NULL;
	int dl_space;
	char *cmd = NULL;
    	char *c = NULL;
	char timestamp[DATALOG_TIME_LENGHT]="";
	
	/*Forbid datalog_get*/
	app_datalog_no_get = 1;
#ifdef POWER_FAIL_MANAGMENT	
	/*Inform POWEROFF MGMT system that datalog is running*/
	*(app_datalog_poweroff_mgmt) |= DATALOG_POWEROFF_ACTIVE;
#endif	
#if DLDEBUG
#ifdef POWER_FAIL_MANAGMENT	
	fprintf(stderr,"POWEROFF FLAG 0x%08x,\n", *(app_datalog_poweroff_mgmt));
#endif	
	fprintf(stderr,"[%s] - get status %d, exit value %d\n", __func__, app_datalog_no_get, datalog_data.status);
#endif		
	/*Copy usbkey index in datalog structure for local usage*/
	datalog_data.usbkey_index = index;
	
	/* Log on internal flash*/
	if (index == 0)
		app_datalog_local = 1;

	else{
		/* Mount the usb key given by index */
	
		rv = app_usb_mount ( datalog_data.usbkey_index );
		if (rv == 1){
			datalog_data.status = -2;
			return datalog_data.status;	
		}
	
	}
	if(!app_datalog_local){	
	   /* Check Space available for Datalog and grab it  */
	   strcpy(datalog_data.dir, app_usb_mpoint_return(datalog_data.usbkey_index));
	   cmd = strdup("df | grep %s | awk '{print $4}'");
	   assert(cmd!=NULL);
	   c = (char *)calloc((strlen(cmd) + strlen(datalog_data.dir) +1 ), sizeof(char));
	   sprintf(c, cmd, datalog_data.dir);
	   fp = popen(c, "r");
	
	   assert(fp!=NULL);
		if (fscanf(fp, "%d", &dl_space) != EOF) {
			
			pclose(fp);
			free(cmd);
			free(c);
			
			dl_space = dl_space *1024;
		
			if ( ( buffer_number * buffer_lenght * DATALOG_UNIT ) >= (unsigned int)( dl_space ) ){
			
				datalog_data.status = dl_space/(DATALOG_UNIT);
				return datalog_data.status;	 
			}
		
			datalog_data.lenght = buffer_lenght*buffer_number;			 
		
		}
		else {
			datalog_data.status = -3;
			return datalog_data.status;			 
		}
				 				
	} else {
	/* Check Space available for Datalog on local flash we allow only 512kbyte buffer and grab it  */
	   strcpy(datalog_data.dir, "/local/datalog");
	   cmd = strdup("du -s %s | awk '{print $1}'");
	   assert(cmd != NULL);
	   c = (char *)calloc((strlen(cmd) + strlen(datalog_data.dir) +1 ), sizeof(char));
	   sprintf(c, cmd, datalog_data.dir);
	   fp = popen(c, "r");
	   assert(fp!=NULL);
		if (fscanf(fp, "%d", &dl_space) != EOF) {
			
			pclose(fp);
			free(cmd);
			free(c);
			
			dl_space = (DATALOG_LOCAL_BUFFER - dl_space) * 1024;
			if ( ( buffer_number * buffer_lenght * DATALOG_UNIT ) >= (unsigned int)( dl_space ) ){
			
				datalog_data.status = dl_space/(DATALOG_UNIT);
				return datalog_data.status;	 
			}
		
			datalog_data.lenght = buffer_lenght*buffer_number;			 
		
		}
		else {
			datalog_data.status = -3;
			return datalog_data.status;			 
		}
	}	

	/*Grab Time reference*/
	time(&datalog_data.milliseconds);
#if DLDEBUG
	struct tm *tmp;
	tmp = localtime(&datalog_data.milliseconds);
	fprintf(stderr,"datalog_data.milliseconds %ul %02d.%02d.%04d %02d:%02d:%02d\n",datalog_data.milliseconds, tmp->tm_mday, tmp->tm_mon + 1, tmp->tm_year + 1900, tmp->tm_hour, tmp->tm_min, tmp->tm_sec); 
#endif		
	/* datalog_data.milliseconds = 1000 * datalog_data.milliseconds; */
	
#ifdef TIMER_GET_MACHINE_CYCLE
	assert(timer_get_machine_cycle(&datalog_data.sys_time) == 0);
#if DLDEBUG
	fprintf(stderr, "datalog_sys_time %u\n", datalog_data.sys_time);
#endif	
#endif
	
	/*Creating DATALOG dir and setting to zero current index*/
	app_datalog_timestamp(timestamp);
	strcat(datalog_data.dir, "/");
	strncat(datalog_data.dir, timestamp, 10);
	strcat(datalog_data.dir, "LOG-XXXXXX");

	if ( mkdtemp(datalog_data.dir) != NULL) {
	  	 datalog_data.dl_index = 0;
		 datalog_data.status = -1;
	} else {
	         datalog_data.status = -4;
		 return datalog_data.status;				 
	}
		
	
	if (datalog_data.status == -1)
		app_datalog_no_get = 0;  /*From now on we can get data*/
#if DLDEBUG
	fprintf(stderr, "START EXIT: datalog_data.status %d app_datalog_no_get %d\n data_dir %s\n", datalog_data.status,
	app_datalog_no_get, datalog_data.dir);
#endif	
	return datalog_data.status;	 

}


/**
 * Capture data to create single log file and the list of captured labels
 * 
 * @param	char *label - label of the captured data (20 char max without spaces)
 * @param	float value - value of the captured data
 *
 * @return  	0 success
 * @error	1 Log failed
 * @error	2 buffer exceeded
 * 
 * @ingroup datalog
 */

short app_datalog_capture(char *label, float value)
{
	
	char timestamp[DATALOG_TIME_LENGHT]="";
	app_datalog_list_t *pt = NULL;
	int rv;

#if DLDEBUG		
	fprintf(stderr, "[%s] - flag %d label '%s' value %f\n", __func__, app_datalog_no_get, label, value);
#endif
	
	if(app_datalog_no_get == 0){
	
		/* Look for label into label list, if not found insert otherwise set occurence */
		pt = app_datalog_label_search(label, &datalog_data.list);
		
		if(pt != NULL){
			if (datalog_data.dl_index <= datalog_data.lenght)
				datalog_data.dl_index++;
			else
				/*Buffer exceeded*/
				return 2;
			app_datalog_timestamp(timestamp);
#ifdef POWER_FAIL_MANAGMENT	
			if(*app_datalog_poweroff_mgmt & DATALOG_POWEROFF_MASK)
#endif
			{

				rv = fprintf(pt->fp, "%s \t %f\n", timestamp, value);
				if( rv < 0 ){
#if DLDEBUG
			
				fprintf(stderr, "%s \t %f, file pointer %p\n error %s rv %d\n", timestamp, value,pt->fp,
				strerror(errno), rv );
#endif				
					return 1;
				}	
#if DLDEBUG
			
				fprintf(stderr, "%s \t %f\n", timestamp, value);
#endif				
				return 0;
			}
#ifdef POWER_FAIL_MANAGMENT	
			else
			{
				app_datalog_no_get = 1;
				/*Close open log file*/
				for (pt = datalog_data.list; pt != NULL; pt = pt->next)
					if(pt->fp!=NULL)
					{
#if DLDEBUG
						printf("[%s] - Closing %p\n", __func__, pt->fp);
#endif						
						fclose(pt->fp);	
					}
#if DLDEBUG
				fprintf(stderr, "GOT SHUTDOWN request!!!!\n");
#endif						
				kill(getpid(), SIGINT);
				SystemHalt = 1;
		    		return 1;
			}
#endif
			
		} else {
			/*Log failed*/
			return 1;
		}			
		
	} 

#if DLDEBUG
	else 		
		fprintf(stderr, "Forbidden GET!!!!\n");
#endif
	return 1;
		
		
}

/**
 * Create final log view
 * 
 *
 * @return              0 - OK, >0 - error code
 *			1 - cannot umount usb device
 * 
 * @ingroup datalog
 */

unsigned int app_datalog_stop(void)
{
	unsigned rv = 1;
#if DLDEBUG		
	fprintf(stderr, "Called STOP!!!!\n");
#endif

	app_datalog_list_t *pt = NULL;
	
	/*Close open log file*/
	for (pt = datalog_data.list; pt != NULL; pt = pt->next)
		if(pt->fp!=NULL)
		{
#if DLDEBUG		
			printf("[%s] - Closing %p\n", __func__, pt->fp);
#endif			
			fclose(pt->fp);
		}
#if DLDEBUG		
	fprintf(stderr, "closed log file!!!!\n");
#endif			
			
       /* Free Memory associated with the datalogger*/
       for (pt = datalog_data.list; pt != NULL; pt = pt->next)
       		if(pt!=NULL)	
			free(pt);	

#if DLDEBUG		
	fprintf(stderr, "Free memory comlete!!!!\n");
#endif	
	/*Unmount the usb device if we are logging on it*/	
	if(!app_datalog_local
#ifdef POWER_FAIL_MANAGMENT	
		 && (*app_datalog_poweroff_mgmt & DATALOG_POWEROFF_MASK)
#endif
	){
		/* Unmount the USB device */
		rv = app_usb_umount(datalog_data.usbkey_index);	
#if DLDEBUG		
	fprintf(stderr, "umount of usb %d!!!!\n", rv);
#endif		
	}	
	
	if(!rv){			
#ifdef POWER_FAIL_MANAGMENT	
		*(app_datalog_poweroff_mgmt) &= ~DATALOG_POWEROFF_ACTIVE;
#endif
		datalog_data.status = 0; 
		datalog_data.milliseconds = 0;
	 	datalog_data.sys_time = 0;
		datalog_data.dl_index = 0;
		datalog_data.lenght = 0;
		datalog_data.usbkey_index = 0;
		datalog_data.dir[0]='\0';
		datalog_data.list = NULL;	
		return datalog_data.status;
	} else {
		datalog_data.status = 0; 
		datalog_data.milliseconds = 0;
	 	datalog_data.sys_time = 0;
		datalog_data.dl_index = 0;
		datalog_data.lenght = 0;
		datalog_data.usbkey_index = 0;
		datalog_data.dir[0]='\0';
		datalog_data.list = NULL;
		if(app_datalog_local){
			app_datalog_local= 0;
			return datalog_data.status;
		}		
		else 
		 return 1;
		
	}	
 	
	
}


/**
 * Get current timestamp value from system time
 * 
 * @param	char *timestamp_data - time value
 *
 * @return  
 * 
 * @ingroup datalog
 */


static void 
app_datalog_timestamp(char *timestamp_data)
{
  
  struct tm *tmp;
  time_t mytime;

#ifdef TIMER_GET_MACHINE_CYCLE
  unsigned int current_time;

  	assert(timer_get_machine_cycle(&current_time) == 0);
 
#if DLDEBUG  	
  fprintf(stderr, "current_time = %u, sys_time = %u\n", current_time, datalog_data.sys_time);
#endif  
  if ( current_time > datalog_data.sys_time){
  	mytime = datalog_data.milliseconds + ( current_time - datalog_data.sys_time )/1000;

  }
  else {
		/*Grab Time reference*/
	time(&datalog_data.milliseconds);
	assert(timer_get_machine_cycle(&datalog_data.sys_time) == 0);
	mytime = datalog_data.milliseconds;
  }
#endif
  tmp = localtime(&mytime); 

  sprintf(timestamp_data, "%02d.%02d.%04d %02d:%02d:%02d", tmp->tm_mday, tmp->tm_mon + 1, tmp->tm_year + 1900, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
 
}

#if 0
/**
 * Free label list memory when unuseful
 * 
 * @param	app_datalog_list_t **head - head of the list to be deleted
 *
 * @return  
 * 
 * @ingroup datalog
 */


static void app_datalog_label_delete ( app_datalog_list_t **head )
{	
	app_datalog_list_t *pt;
	
	pt = *head;
	*head = pt->next;
	free(pt);
	

} 
#endif

/**
 * Search in the list of label for current label, if not present add to the list
 * 
 * @param	app_datalog_list_t **head - head of the list to be searched in
 * @param 	char *label - current label
 *
 * @return  	app_datalog_list_t *pt pointer to the found label
 * @error	NULL if failed to open log file or if no more file can be opened. 
 * 
 * @ingroup datalog
 */


static app_datalog_list_t *
app_datalog_label_search( char *label, app_datalog_list_t **head )
{
	app_datalog_list_t *pt = NULL;
	int flag = 0;
	char *l = NULL;
	
#if DLDEBUG		
	printf("[%s] - label %s\n", __func__, label);
#endif
	/* Change all uppercase letter in lowercase */
	l = label;
	
	for (l = label; *l !='\0'; l++){
	  if (*l >= 'A' && *l <= 'Z')
           *l = *l + 'a' - 'A';
	} 	
	
	/* Look in the list to see if label is already in*/
	if (*head != NULL) {
		for (pt = *head; pt != NULL; pt = pt->next)
		{
			if ( strcmp(pt->label, label)== 0){
				flag =1;
				pt->occurence++;
				if(pt->occurence < DATALOG_MAX_FILE_LENGHT)
					break;
				else{
					
					if(pt->log_num < DATALOG_MAX_FILE_LENGHT){
						/*Close old file*/
#if DLDEBUG		
						printf("[%s] - Closing %p\n", __func__, pt->fp);
#endif
						fclose(pt->fp);
						pt->log_num++;
						l = (char *)calloc(strlen(datalog_data.dir) + 1 + 6 + strlen(label) + 1, sizeof(char));
						if(l!= NULL){
							sprintf(l,"%s/%05d.%s", datalog_data.dir, pt->log_num, pt->label);
	#if DLDEBUG
							fprintf(stderr,"log file %s\n", l);
	#endif						
							pt->fp = fopen(l, "a");
	#if DLDEBUG
							fprintf(stderr, "%s: open filename '%s', pointer %p\n", __func__, l, pt->fp);
							fprintf(stderr,"log file pointer  %p\n", pt->fp);
	#endif								
							pt->occurence = 1;
							if(pt->fp != NULL)
								free(l);
							
							else {
								free(l);
								return NULL;
							}	
								
						}
					} else 
						return NULL; 		
				}	
			}
		}
	}
	if (!flag)
	{
		printf("[%s] - Adding new label '%s'\n", __func__, label);
		return app_datalog_label_insert(label, head);
	}
	else
	{
		printf("[%s] - label '%s' found.\n", __func__, label);
		return pt;
	}
}

/**
 * Insert a label into the headlist of labels
 * 
 * @param	app_datalog_list_t **head - head of the list 
 * @param 	char *label - current label
 *
 * @return       app_datalog_list_t *pt - pointer to the new label inserted in the list.
 * 
 * @error	NULL if the new log file can't be opened
 *
 * @ingroup datalog
 */


static app_datalog_list_t *
app_datalog_label_insert(char *label, app_datalog_list_t **head) 
{
	app_datalog_list_t *qt = NULL;
	
	char *filename = NULL;
	
	filename = (char *)calloc(strlen(datalog_data.dir) + 1 + 6 + strlen(label) + 1, sizeof(char));
	
	qt = (app_datalog_list_t *)malloc(1 * sizeof (app_datalog_list_t ));
	assert (qt != NULL);
	strcpy (qt->label, label);
	qt->occurence = 1;
	qt->log_num = 1;
	if(filename != NULL)
	{
		sprintf(filename,"%s/%05d.%s", datalog_data.dir, qt->log_num, qt->label);
	}
	qt->fp = fopen(filename, "a");
#if DLDEBUG
	fprintf(stderr, "%s: open filename '%s', pointer %p\n", __func__, filename, qt->fp);
#endif	
	if(qt->fp == NULL){
		fprintf(stderr, "%s: Cannot open file '%s' [%s]\n", __func__, filename, strerror(errno));
		free(filename);
		return NULL;
	}
		
	qt->next = *head;	
	*head = qt;	
	free(filename);
	
#if DLDEBUG
	fprintf(stderr, "%s: done.\n", __func__);
#endif	
	return qt;	
}
#if 0
/**
 * Sort label based on their occurrence number from MAX to MIN
 * 
 * @param	app_datalog_list_t **head - head of the list to be sorted
 * @param	app_datalog_list_t **head_sorted - head of the sorted list
 *
 * @return  
 * 
 * @ingroup datalog
 */


static void
app_datalog_label_sort(app_datalog_list_t **head, app_datalog_list_t **head_sorted) 
{
	app_datalog_list_t *qt, *pt, *ptmin = NULL;
	unsigned int min;
	
	
	while(*head != NULL){
		min = (*head)->occurence;
		ptmin = (*head);
		for(pt = (*head); pt != NULL; pt = pt->next ){
			if(pt->occurence < min){
				min = pt->occurence;
				ptmin = pt;
			}
		}
		if (ptmin != (*head)) {
			qt = (*head);
			while (qt->next != ptmin)
				qt = qt->next;
			qt->next = ptmin->next; /* removing element from list */
			if (*head_sorted == NULL){
				*head_sorted = ptmin;
				(*head_sorted)->next = NULL;
			}
			else {
				qt = (*head_sorted);
				while (qt->next != NULL)
					qt = qt->next;
				qt->next = ptmin;
				ptmin->next = NULL;	
			}
				
		}
		else { /* deleting from head no need for previous element*/
			*head = ptmin->next;
			if (*head_sorted == NULL){
				*head_sorted = ptmin;
				(*head_sorted)->next = NULL;
			}
			else {
				qt = (*head_sorted);
				while (qt->next != NULL)
					qt = qt->next;
				qt->next = ptmin;
				ptmin->next = NULL;	
			}
		
		}

	}
		
}	
#endif

/*
 * FarosPLC view
 */

void Datalog_start(STDLIBFUNCALL)
{
	DATALOG_START_PARAM OS_SPTR *pPara = (DATALOG_START_PARAM OS_SPTR *)pIN;

	pPara->ret_value = app_datalog_start(pPara->buffer_lenght, pPara->buffer_number, pPara->index);
}

#define MAX_LABEL_LEN 1024
void Datalog_get(STDLIBFUNCALL)
{
	char label[MAX_LABEL_LEN];
	DATALOG_GET_PARAM OS_SPTR *pPara = (DATALOG_GET_PARAM OS_SPTR *)pIN;

	/* TODO: allocate it dinamically */
	assert((pPara->label->CurLen) < MAX_LABEL_LEN);

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->label, label);
	pPara->ret_value = app_datalog_capture(label, pPara->value);	
}

void Datalog_stop(STDLIBFUNCALL)
{
	DATALOG_STOP_PARAM OS_SPTR *pPara = (DATALOG_STOP_PARAM OS_SPTR *)pIN;
	pPara->ret_value = app_datalog_stop();
}

void Datalog_eject(STDLIBFUNCALL)
{
	DATALOG_EJECT_PARAM OS_SPTR *pPara = (DATALOG_EJECT_PARAM OS_SPTR *)pIN;
	pPara->ret_value = app_datalog_eject(pPara->index);
}

