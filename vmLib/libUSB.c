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
 * Filename: libUSB.c
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <unistd.h>
#include <dirent.h>

#include "mectCfgUtil.h"

#include "libUSB.h"

#define SCSI_DRIVER_PATH  "/proc/scsi/usb-storage-"
#define USBDEV_COMMON "/dev/sd"
#define MNTDIR_COMMON "/tmp/mnt"

#define DBGUSB 0
#define DBGPOLL 0


typedef struct app_usb_scsiview_t {
	char vendor[40];
	char serial[40];
	char mpoint;
} app_usb_scsiview_s;

typedef struct app_usb_devices_t {
	unsigned short lev;
	unsigned short port;
	unsigned short prnt;
	unsigned short cnt;
	unsigned short dev;
	unsigned short mxch;
	unsigned short kind;
	char vendor[40];
	char serial[40];
} app_usb_device_s;


/* Global Variables */
unsigned short USBstatus[APP_USB_MAX + 1];
unsigned short USBfeedback[2];

app_usb_scsiview_s app_usb_scsiview[APP_USB_MAX];

struct copy_params {
	char *srcfile;
	char *dstfile;
	unsigned short src_index;
	unsigned short dst_index;
};
	
struct copy_params app_usb_copy_file_params;
pthread_t app_usb_copy_file_thread_id;
pthread_t app_usb_diskcopy_file_thread_id;

struct delete_params {
	char *filename;
	unsigned short index;
};

struct delete_params app_usb_delete_file_params;
pthread_t app_usb_delete_file_thread_id;


/*Local Functions*/
void *app_usb_file_copy_manager(void *param );
void *app_usb_file_diskcopy_manager(void *param );
void *app_usb_file_delete_manager(void *param );

/* Public Functions*/


/**
* Mount an usb key
*
* @param index  key to be mounted
* 
*
* @return	0 for success 1 for error 
* 
* @ingroup usb
*/

unsigned short
app_usb_mount( unsigned short index ) 
{
	char id[2]="";
	int ll;
	short rv = 1;

#if DBGUSB	
	fprintf(stderr,"Enter: %s %d\n",__func__, index); 
#endif	
	
	
	if( index > 0 && index < APP_USB_MAX && USBstatus[index] >= 97 ){
	
		id[0] = (char)(USBstatus[index] & 0x00FF);
		id[1] = (char)((USBstatus[index] >> 8) & 0x00FF);
		if (id[1] == '0')
		{
			id[1] = '\0';
		}
		else
		{
			id[2] = '\0';
		}

		/* Define the device name and the related mount point */
	
		strcat (app_usb[index].dev, USBDEV_COMMON);
		strcat (app_usb[index].dev, id);
		ll = strlen(USBDEV_COMMON)+ 1;
		app_usb[index].dev[ll] = '\0';

		strcat (app_usb[index].mpoint, MNTDIR_COMMON);
		strcat (app_usb[index].mpoint, id);
		ll = strlen(MNTDIR_COMMON)+ 1;
		app_usb[index].mpoint[ll] = '\0';
	
		strcpy(app_usb[index].dev1,app_usb[index].dev);

#if DBGUSB	
		fprintf(stderr, "%s %s %s\n", app_usb[index].dev,  app_usb[index].dev1, app_usb[index].mpoint );
#endif	
		/* test the mount point existence if failed create mpoint */
		if ( access(app_usb[index].mpoint, F_OK )!= 0) {

			if( mkdir( app_usb[index].mpoint, S_IRWXU | S_IRWXG | S_IRWXO )!= 0 ){
				fprintf(stderr, "%s : %s\n", __func__, strerror(errno));	
				return rv;
			}
		
		} 
	
		/* Trying to mount the device, mount returns 0 on success and -1 on failure*/
	
		rv = mount (app_usb[index].dev1, app_usb[index].mpoint, "vfat", 0, (void *)NULL );
		if (rv == -1){
			fprintf(stderr, "%s : %s\n", __func__, strerror(errno));	
			rv = 1;
		}
		if (rv){
			rv = mount (app_usb[index].dev, app_usb[index].mpoint, "vfat", 0, (void *)NULL );
			if (rv == -1){
				fprintf(stderr, "%s : %s\n", __func__, strerror(errno));	
				rv = 1;
			}
			
		}
		
		return rv;
	}	
	
	return rv;
}

/**
* Unmount an usb key
*
* @param index  key to be mounted
*
* @return	0 for success 1 for error 
* 
* @ingroup usb
*/

unsigned short
app_usb_umount( unsigned short index ) 
{
	char id[2]="";
	int ll;

	unsigned short rv = 1;

#if DBGUSB	
	fprintf(stderr,"Enter: %s\n",__func__);
#endif			
	if( index > 0 && index < APP_USB_MAX && USBstatus[index] >= 97 ){

		id[0] = (char)(USBstatus[index] & 0x00FF);
		id[1] = (char)((USBstatus[index] >> 8) & 0x00FF);
		if (id[1] == '0')
		{
			id[1] = '\0';
		}
		else
		{
			id[2] = '\0';
		}

		strcat (app_usb[index].mpoint, MNTDIR_COMMON);
		strcat (app_usb[index].mpoint, id);
		ll = strlen(MNTDIR_COMMON)+ 1;
		app_usb[index].mpoint[ll] = '\0';
		
		rv = umount2(app_usb[index].mpoint, MNT_FORCE);
		
		if(!rv){
	/* Clean usb info */
			strcpy(app_usb[index].dev , "");
			strcpy(app_usb[index].dev1, "");
			strcpy(app_usb[index].mpoint, "");
			return rv;
		}
		
		strcpy(app_usb[index].dev , "");
		strcpy(app_usb[index].dev1, "");
		strcpy(app_usb[index].mpoint, "");
#if DBGUSB	
		fprintf(stderr,"%s dev %s, dev1 %s, mpoint %s\n",__func__, app_usb[index].dev, app_usb[index].dev1, app_usb[index].mpoint);
#endif				
		return 1;
		
	}
	return rv;
}

/**
* Create a directory on a usb key
*
* @param char *dirname directory to be created
* @param unsigned short index  index representing the usbkey on which the directory will exist
*
* @return	0 for success 1 for error 2 directory already exist
* 
* @ingroup usb
*/

static unsigned short
app_usb_dir_create( char *dirname, unsigned short index )
{
	char *full_dirname;
	char *l;	

	printf("[%s] - dirname: '%s'\n", __func__, dirname);	
	if (!USBfeedback[0]) {
		
		full_dirname = (char *)calloc((strlen(dirname)+ 1 + strlen(app_usb[index].mpoint) + 1 + 2 ), sizeof (char));
		
		l = dirname;
		if (*l == '.' && ( *(l+1) == '/' || *(l+1) == '\\' ))
			l = dirname + 2;
						
		strcpy(full_dirname, app_usb[index].mpoint);		
		strcat(full_dirname, "/");	
		strcat(full_dirname,  l );
		for(l = full_dirname; *l != '\0'; l++)
			if (*l == '\\')
				*l = '/';		
		if ( access(full_dirname, F_OK )!= 0) {
			if( mkdir( full_dirname, S_IRWXU | S_IRWXG | S_IRWXO )!= 0 ){
				free(full_dirname);
				return 1;
			} else {
				free(full_dirname);
				return 0;
			}	
		} else {
			free(full_dirname);
			return 2;	
		}
		

	} else
		return 1;	
}




/**
* Copy either a file or a directory with all its content between usb keys
*
* @param char *filename file o directory to be copied
* @param unisgned short src_index   index in the USBstatus array  representing the source usbkey for filename
* @param unisgned short dst_index   index in the USBstatus array  representing the destination usbkey for filename
*
* @return	0 for success,  1 for error: resource already busy cannot start another usb operation
* 
* @ingroup usb
*/
static unsigned short
app_usb_file_copy(char *srcfile, char *dstfile, unsigned short src_index, unsigned short dst_index )
{
	if (!USBfeedback[0]) {
		USBfeedback[0] = app_usb_feedback[0] = 1; /*Set to one flag about operation on usb running -- usb locked*/
		USBfeedback[1] = app_usb_feedback[1] = 0; /*Reset error code for current running operation on usb*/
		
		app_usb_copy_file_params.srcfile = srcfile;
		app_usb_copy_file_params.dstfile = dstfile;
		app_usb_copy_file_params.src_index = src_index;
		app_usb_copy_file_params.dst_index = dst_index;

		osPthreadCreate(&app_usb_copy_file_thread_id, NULL, &app_usb_file_copy_manager, &app_usb_copy_file_params, "app_usb_file_copy_manager", 0);

		return 0;
	} else
		return 1;
}


/**
* Copy either a file or a directory with all its content between usb keys -- thread manager
* Status of execution handling is stored in USBfeedback[1]
*
* 				0 for success,  > 0 for error : 
*				1- cannot mount/umount usb key, 
*				2- source file does not exist, 
*				3- there's no enough space on destination usbkey to perform the copy 
*				4- copy failed
* @param		Void *param - structure containg all the info required to perform the copy.
* @return		NULL	
* 
* @ingroup usb
*/
void *
app_usb_file_copy_manager(void *param )
{
	struct copy_params *p = (struct copy_params *)param;
	
	unsigned short rv = 1;
	FILE *fp;
	int dst_free_space = 0;
	char dst_mpoint[22]="";
	char src_mpoint[22]="";
	char *c;
	char *cmd;
	char *dst_path;
	char *src_path;
	char *l;
	struct stat filename_stat;
		
	/* Mount the usb key involved in file copying */
	if ( p->src_index != p->dst_index ){
		if ( app_usb_mount( p->dst_index ) || app_usb_mount( p->src_index ) ){
			app_usb_feedback[1]= 1;
			app_usb_feedback[0]= 0; 
			return NULL;
		}	
	} 
	else {
		if ( app_usb_mount( p->src_index ) ){
			app_usb_feedback[1]= 1;
			app_usb_feedback[0]= 0; 
			return NULL;
		}	
	}	
	
	/* Check if destination has enough space to host filename*/
	strcpy ( dst_mpoint, app_usb_mpoint_return( p->dst_index ) );
#if DBGUSB	
	fprintf(stderr,"dst_mpoint: %s\n", dst_mpoint);
#endif	
	cmd = strdup("df | grep %s | awk '{print $4}'");
	assert(cmd!=NULL);
	c = (char *)calloc((strlen(cmd) + strlen(dst_mpoint) +1 ), sizeof(char));
	sprintf(c, cmd, dst_mpoint);
	fp = popen(c, "r");
	assert(fp!=NULL);
	if (fscanf(fp, "%u", &dst_free_space) != EOF) {
			
		pclose(fp);
		free(cmd);
		free(c);
	}
	
	strcpy ( src_mpoint, app_usb_mpoint_return( p->src_index ) );
#if DBGUSB	
	fprintf(stderr,"src_mpoint: %s\n", src_mpoint);
#endif	
	src_path = (char *)calloc((strlen(src_mpoint) + strlen(p->srcfile) + 2 ), sizeof(char));
	strcat(src_path, src_mpoint);
	l = p->srcfile;
	if (*l == '.' && ( *(l+1) == '/' || *(l+1) == '\\' ))
		l = p->srcfile + 2;	
	strcat(src_path, "/");	
	strcat(src_path,  l );
	for(l = src_path; *l != '\0'; l++)
		if (*l == '\\')
			*l = '/';

#if DBGUSB	
	fprintf(stderr,"file to copy: %s\n", src_path);	
#endif	
	if ( (stat(src_path, &filename_stat)) == -1 ){
	  	fprintf(stderr, "ERRORE: %s", strerror(errno));	
		app_usb_feedback[1]= 2;
		app_usb_feedback[0]= 0; 
		return NULL;
	} else if (filename_stat.st_size >= (dst_free_space * 1024) ){
#if DBGUSB	
		fprintf(stderr,"file size: %d, free space = %d\n",filename_stat.st_size, dst_free_space );	
#endif		
		app_usb_feedback[1]= 3;
		app_usb_feedback[0]= 0; 
		return NULL;
	
	} else { /* There is space to copy the file to the destination key*/
		
		/*build destination path*/
		dst_path = (char *)calloc((strlen(dst_mpoint) + strlen(p->dstfile) + 2 ), sizeof(char));
		strcat(dst_path, dst_mpoint);
		l = p->dstfile;
		if (*l == '.' && ( *(l+1) == '/' || *(l+1) == '\\' ))
			l = p->dstfile + 2;	
		strcat(dst_path, "/");	
		strcat(dst_path,  l );
		for(l = dst_path; *l != '\0'; l++)
			if (*l == '\\')
				*l = '/';
#if DBGUSB					
		fprintf(stderr,"destination: %s\n", dst_path);
#endif			
		/*perform copy*/
		cmd = strdup("cp -a %s %s");
		c = (char *)calloc((strlen(cmd) + strlen(src_path) +1 + strlen(dst_path) +1 ), sizeof(char));
		sprintf(c, cmd, src_path, dst_path);
#if DBGUSB		
		fprintf(stderr,"copy command: %s\n", c);
#endif			
		rv = system(c);
		free(cmd);
		free(c);
		free(src_path);
		free(dst_path);	
		if(rv){
			app_usb_feedback[1]= 4;
			app_usb_feedback[0]= 0; 
			return NULL;
		}	
	}
				
	/* Unmount usb devices */
	if ( p->src_index != p->dst_index ){
		if ( app_usb_umount( p->src_index ) || app_usb_umount( p->dst_index ) ){
			app_usb_feedback[1]= 1;
			app_usb_feedback[0]= 0; 
			return NULL;
		}	
	} 
	else {
		if ( app_usb_umount( p->src_index ) ){
			app_usb_feedback[1]= 1;
			app_usb_feedback[0]= 0; 
			return NULL;
		}	
	}
		
	app_usb_feedback[1]= 0;
	app_usb_feedback[0]= 0; 
	return NULL;	
	
}	


/**
* Copy either a file or a directory with all its content from a usb key to internal flash
*
* @param char *srcfile file o directory to be copied
* @param char *dstfile filename o directory name  for destination
* @param unisgned short src_index   index in the USBstatus array  representing the source usbkey for filename
*
* @return	0 for success,  1 for error: resource already busy cannot start another usb operation
* 
* @ingroup usb
*/
static unsigned short
app_usb_file_diskcopy(char *srcfile, char *dstfile, unsigned short src_index )
{
	
	if (!USBfeedback[0]) {
		USBfeedback[0] = app_usb_feedback[0] = 1; /*Set to one flag about operation on usb running -- usb locked*/
		USBfeedback[1] = app_usb_feedback[1] = 0; /*Reset error code for current running operation on usb*/
		
		app_usb_copy_file_params.srcfile = srcfile;
		app_usb_copy_file_params.dstfile = dstfile;
		app_usb_copy_file_params.src_index = src_index;
		app_usb_copy_file_params.dst_index = 0;

		osPthreadCreate(&app_usb_diskcopy_file_thread_id, NULL, &app_usb_file_diskcopy_manager, &app_usb_copy_file_params, "app_usb_file_diskcopy_manager", 0);
		
		return 0;
	} else
		return 1;
	
}


/**
* Copy either a file or a directory with all its content between usb keys -- thread manager
* Status of execution handling is stored in USBfeedback[1]
*
* 				0 for success,  > 0 for error : 
*				1- cannot mount/umount usb key, 
*				2- source file does not exist, 
*				3- there's no enough space on destination usbkey to perform the copy 
*				4- copy failed
* @param		Void *param - structure containg all the info required to perform the copy.
* @return		NULL	
* 
* @ingroup usb
*/
void *
app_usb_file_diskcopy_manager(void *param )
{
#define DST_MPOINT APP_CONFIG_DIR
	
	struct copy_params *p = (struct copy_params *)param;
	
	unsigned short rv = 1;
	FILE *fp;
	int dst_free_space = 0;
	char dst_mpoint[22]="";
	char src_mpoint[22]="";
	char *c;
	char *cmd;
	char *dst_path;
	char *src_path;
	char *l;
	struct stat filename_stat;
		
	/* Mount the usb key involved in file copying */
	if ( app_usb_mount( p->src_index ) ){
		app_usb_feedback[1]= 1;
		app_usb_feedback[0]= 0; 
		return NULL;
	}	
		
	
	/* Check if destination has enough space to host filename*/
	strcpy ( dst_mpoint, DST_MPOINT );
#if DBGUSB	
	fprintf(stderr,"dst_mpoint: %s\n", dst_mpoint);
#endif	
	cmd = strdup("df %s | awk 'NR==2 {print $4}'");
	assert(cmd!=NULL);
	c = (char *)calloc((strlen(cmd) + strlen(dst_mpoint) +1 ), sizeof(char));
	sprintf(c, cmd, dst_mpoint);
#if DBGUSB	
		fprintf(stderr,"command %s\n",c );	
#endif	
	fp = popen(c, "r");
	assert(fp!=NULL);
	if (fscanf(fp, "%u", &dst_free_space) != EOF) {
			
		pclose(fp);
		free(cmd);
		free(c);
	}
#if DBGUSB	
		fprintf(stderr," free space = %d\n", dst_free_space );	
#endif		
	strcpy ( src_mpoint, app_usb_mpoint_return( p->src_index ) );
#if DBGUSB	
	fprintf(stderr,"src_mpoint: %s\n", src_mpoint);
#endif	
	src_path = (char *)calloc((strlen(src_mpoint) + strlen(p->srcfile) + 2 ), sizeof(char));
	strcat(src_path, src_mpoint);
	l = p->srcfile;
	if (*l == '.' && ( *(l+1) == '/' || *(l+1) == '\\' ))
		l = p->srcfile + 2;	
	strcat(src_path, "/");	
	strcat(src_path,  l );
	for(l = src_path; *l != '\0'; l++)
		if (*l == '\\')
			*l = '/';

#if DBGUSB	
	fprintf(stderr,"file to copy: %s\n", src_path);	
#endif	
	if ( (stat(src_path, &filename_stat)) == -1 ){
	  	fprintf(stderr, "ERRORE: %s", strerror(errno));	
		app_usb_feedback[1]= 2;
		app_usb_feedback[0]= 0; 
		return NULL;
	} else if (filename_stat.st_size >= (dst_free_space * 1024) ){
#if DBGUSB	
		fprintf(stderr,"file size: %d, free space = %d\n",filename_stat.st_size, dst_free_space );	
#endif		
		app_usb_feedback[1]= 3;
		app_usb_feedback[0]= 0; 
		return NULL;
	
	} else { /* There is space to copy the file to the destination key*/
		
		/*build destination path*/
		dst_path = (char *)calloc((strlen(dst_mpoint) + strlen(p->dstfile) + 2 ), sizeof(char));
		strcat(dst_path, dst_mpoint);
		l = p->dstfile;
		if (*l == '.' && ( *(l+1) == '/' || *(l+1) == '\\' ))
			l = p->dstfile + 2;	
		strcat(dst_path, "/");	
		strcat(dst_path,  l );
		for(l = dst_path; *l != '\0'; l++)
			if (*l == '\\')
				*l = '/';
#if DBGUSB					
		fprintf(stderr,"destination: %s\n", dst_path);
#endif			
		/*perform copy*/
		cmd = strdup("cp -a %s %s");
		c = (char *)calloc((strlen(cmd) + strlen(src_path) +1 + strlen(dst_path) +1 ), sizeof(char));
		sprintf(c, cmd, src_path, dst_path);
#if DBGUSB		
		fprintf(stderr,"copy command: %s\n", c);
#endif			
		rv = system(c);
		free(cmd);
		free(c);
		free(src_path);
		free(dst_path);	
		if(rv){
			app_usb_feedback[1]= 4;
			app_usb_feedback[0]= 0; 
			return NULL;
		}	
	}
				
	/* Unmount usb devices */

	if ( app_usb_umount( p->src_index ) ){
		app_usb_feedback[1]= 1;
		app_usb_feedback[0]= 0; 
		return NULL;
	}	
		
	app_usb_feedback[1]= 0;
	app_usb_feedback[0]= 0; 
	return NULL;	

#undef DST_MPOINT	
}	




/**
* Delete a file from a usb key
*
* @param char *filename file o directory to be deleted
* @param unsigned short src_index   index representing the usbkey on which the file is stored
*
* @return	0 for success 1 for error 
* 
* @ingroup usb
*/

static unsigned short
app_usb_file_delete( char *filename, unsigned short index )
{
	
	if (!USBfeedback[0]) {
		USBfeedback[0] = app_usb_feedback[0] = 1; /*Set to one flag about operation on usb running -- usb locked*/
		USBfeedback[1] = app_usb_feedback[1] = 0; /*Reset error code for current running operation on usb*/
		
		app_usb_delete_file_params.filename = filename;
		app_usb_delete_file_params.index = index;

		osPthreadCreate(&app_usb_delete_file_thread_id, NULL, &app_usb_file_delete_manager, &app_usb_delete_file_params, "app_usb_file_delete_manager", 0);
		
		return 0;
	} else
		return 1;	
}



/**
* Delete a file from a usb key -- thread manager
* Status of execution handling is stored in USBfeedback[1]: 0 for success,   1 for error: 
*
* @param	void *param -- parameters required to handle delete operation
* @return	NULL
* 
* @ingroup usb
*/

void *
app_usb_file_delete_manager( void *param )
{
	struct delete_params *p = (struct delete_params *)param;
	
	unsigned short rv = 1;
	char mpoint[22]="";
	char *c;
	char *cmd;
	char *path;
	char *l;	
	
	if ( app_usb_mount( p->index ) ){
		app_usb_feedback[1]= 1;
		app_usb_feedback[0]= 0; 
		return NULL;	
	
	}
	
	strcpy ( mpoint, app_usb_mpoint_return( p->index ) );
	path = (char *)calloc((strlen(mpoint) + strlen(p->filename) + 2 ), sizeof(char));
	strcat(path, mpoint);
	l = p->filename;
	if (*l == '.' && ( *(l+1) == '/' || *(l+1) == '\\' ))
		l = p->filename + 2;	
	strcat(path, "/");	
	strcat(path,  l );
	for(l = path; *l != '\0'; l++)
		if (*l == '\\')
			*l = '/';
	
	/*perform delete*/
	cmd = strdup("rm -rf %s");
	c = (char *)calloc((strlen(cmd) + strlen(path) +1 ), sizeof(char));
	sprintf(c, cmd, path);
	rv = system(c);
	free(cmd);
	free(c);
	free(path);
	if(rv){
		app_usb_feedback[1]= 1;
		app_usb_feedback[0]= 0; 
		return NULL;
	   
	}   									
	/*Unmount the key*/	
	if ( app_usb_umount( p->index ) ){
		app_usb_feedback[1]= 1;
		app_usb_feedback[0]= 0; 
		return NULL;
	}
	
	app_usb_feedback[1]= 0;
	app_usb_feedback[0]= 0; 
	return NULL;

}



/**
* Retrieve the mountpoint for the given index key
*
* @param 	index key whose mount point needs to be retrieved.
* 
* @return	char pointer to the mpoint.
* 
* @ingroup usb
*/

char *
app_usb_mpoint_return( unsigned short index ){

	return app_usb[index].mpoint;
}

/**
* Init Data structure that contains usb keys mapping device and mountpoints, scsi info and usb operation feedback.
*
* @param 	void
* 
* @return	void
* 
* @ingroup usb
*/

void
app_usb_init( void )
{
  int i;
  
  for (i = 0; i < APP_USB_MAX; i++) {
	strcpy(app_usb[i].dev, "");
	strcpy(app_usb[i].dev1, "");
	strcpy(app_usb[i].mpoint, "");
	app_usb_scsiview[i].mpoint = 'a' + i;
	
	if((i == 0) || (i == 1))
		app_usb_feedback[i] = 0;
  }	
	
	
}


/**
* Usb system manager thread
*
* @param 	none
* 
* @return	short int n number of port detected, < 0 for error 
* @return	USBstatus[port] = <mount letter> (eg. port 2 on /dev/sda -> USBstatus[2]='a')
* @return	USBstatus[0] = <number of mounted usb storage>
*
* @error	USBstatus[APP_USB_MAX] = 10 - Detected another multiport hub, only one supported
* @error	USBstatus[APP_USB_MAX] = 20 - Parsing error
* @error	USBstatus[APP_USB_MAX] = 30 - Cannot get device info for usb subsystem
*
* @ingroup usb
*/
#define LINE_SIZE 256
#define SCSI_ID_DIR "/proc/scsi/usb-storage"
#define ADDRESS_TAG "usb-storage: device found at"

short int
app_usb_status_read(void)
{
	struct dirent **namelist;
	int n, item = 0; 
	FILE * pipe;
	int scsiid=-1;
	int address=-1;
	int port=-1;
	char line[LINE_SIZE];
	char temp[LINE_SIZE];
	char mountpoint[LINE_SIZE];

	mountpoint[0] = '\0';
	memset(app_usb_status, 0, sizeof(app_usb_status));

	/* 
	   looking for each scsi ID of the actual attached USB parse the dmesg
	   in order to extract the port and the mount point
	 */
	n = scandir(SCSI_ID_DIR, &namelist, 0, alphasort); 
	if (n < 0) 
	{
		printf("no usb connected\n");
		return 0;
	}
	else
	{
		while(n--)
		{
			/* skip the item "." and ".."  */
			if (namelist[n]->d_name[0] == '.')
			{
				continue;
			}

			/* get the scsi id (equal to the file name in the SCSI_ID_DIR) */
			scsiid = atoi(namelist[n]->d_name);
			pipe = popen("dmesg", "r");
			sprintf(temp, "scsi%d", scsiid);

			/* seek  in the dmesg file the scsi id section */
			while(fgets(line, LINE_SIZE, pipe) != NULL)
			{
				if (strstr(line, temp) != NULL)
				{
					break;
				}
			}
			if (feof(pipe))
			{
				printf("cannot find %s\n", temp);
				pclose(pipe);
				app_usb_status[APP_USB_MAX] = 30;
				return -2;
			}
#if DBGPOLL
			printf("##### found scsiid : %d\n", scsiid);
#endif
			/* looking for address in the dmesg file */
			while(fgets(line, LINE_SIZE, pipe) != NULL)
			{
				if (strncmp(ADDRESS_TAG, line, strlen(ADDRESS_TAG))==0)
				{
					sscanf(line + strlen(ADDRESS_TAG), "%d", &address);
					break;
				}
			}
			if (address == -1)
			{
				printf("cannot find usb address\n");
				pclose(pipe);
				app_usb_status[APP_USB_MAX] = 20;
				return -3;
			}
#if DBGPOLL
			printf("found address : %d\n", address);
#endif
			/* looking for mount point in the dmesg file */
			sprintf(temp, "sd %d:", scsiid);
			while(fgets(line, LINE_SIZE, pipe) != NULL)
			{
				if (strncmp( temp, line, strlen(temp))==0)
				{
					if (strchr (line, '[') != NULL)
					{
						strcpy(mountpoint, strchr (line, '[') + 1);
						if (strchr (mountpoint, ']'))
						{
							*strchr (mountpoint, ']') = '\0';
							break;
						}
						else
						{
							pclose(pipe);
							app_usb_status[APP_USB_MAX] = 20;
							return -4;
						}
					}
					else
					{
						pclose(pipe);
						app_usb_status[APP_USB_MAX] = 20;
						app_usb_status[APP_USB_MAX] = 20;
						return -5;
					}
				}
			}
			if (mountpoint[0] == '\0' )
			{
				printf("cannot found mountpoint\n");
				pclose(pipe);
				app_usb_status[APP_USB_MAX] = 20;
				return -6;
			}

			if (feof(pipe))
			{
				printf("cannot found mountpoint\n");
				pclose(pipe);
				app_usb_status[APP_USB_MAX] = 20;
				return -7;
			}

			/* looking for id mount point */
			sprintf(temp, "%s:", mountpoint);
			while(fgets(line, LINE_SIZE, pipe) != NULL)
			{
				if (strstr(line, temp) != NULL)
				{
					if (strlen(strchr(line, ':') + 1) > 0)
					{
						sscanf(strchr(line, ':') + 1, "%s", mountpoint);
					}
				}

			}
			pclose(pipe);

#if DBGPOLL
			printf("# mountpoint %s, %d (%c)\n",mountpoint, mountpoint[2], mountpoint[2]);
#endif
			/* extract port */
			sprintf(temp, "dmesg | grep usb | grep \"address %d\"",address); 
			pipe = popen(temp, "r");
			if(fgets(line, LINE_SIZE, pipe) != NULL)
			{
				if (strchr(line, '.') && strchr(line, ':') && strchr(line, '.')< strchr(line, ':'))
				{
					*strchr(line, ':') = '\0';
					port = atoi(strchr(line, '.')+1);
				}
				else
				{
					printf("malformed line %s\n",line);
					printf("cannot found port\n");
					pclose(pipe);
					app_usb_status[APP_USB_MAX] = 20;
					return -8;
				}
			}
			else
			{
				printf("cannot found port\n");
				pclose(pipe);
				app_usb_status[APP_USB_MAX] = 20;
				return -9;
			}
			pclose(pipe);
#if DBGPOLL
			printf("port %d\n", port);
#endif
			if (port > APP_USB_MAX)
			{
				printf("the hub have too many port (%d). only %d port are managed\n", port, APP_USB_MAX);
				
				app_usb_status[APP_USB_MAX] = 20;
				app_usb_status[APP_USB_MAX] = 30;
				continue;
			}
			item++;
			{
				char mountid = ((mountpoint[3] == '\0')? '0' : mountpoint[3]);
				app_usb_status[port] = ( ((mountpoint[2]) & 0x00FF) | ((mountid & 0x00FF) << 8) );
			}
		} 
	} 
	app_usb_status[0] = item;
	return item;
}

/* PLC Views - 4C */

void Usb_on(STDLIBFUNCALL)
{
	USB_INDEX_PARAM OS_SPTR *pPara = (USB_INDEX_PARAM OS_SPTR *)pIN;

	pPara->ret_value = app_usb_mount(pPara->index);
}

void Usb_off(STDLIBFUNCALL)
{
	USB_INDEX_PARAM OS_SPTR *pPara = (USB_INDEX_PARAM OS_SPTR *)pIN;

	pPara->ret_value = app_usb_umount(pPara->index);
}

#define MAX_FILENAME_LEN 1024
void Usb_copy(STDLIBFUNCALL)
{
	char srcfile[MAX_FILENAME_LEN], dstfile[MAX_FILENAME_LEN];
	USB_COPY_PARAM OS_SPTR *pPara = (USB_COPY_PARAM OS_SPTR *)pIN;
	
	/* TODO: allocate it dinamically */
	assert((pPara->srcfile->CurLen) < MAX_FILENAME_LEN);
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->srcfile, srcfile);

	/* TODO: allocate it dinamically */
	assert((pPara->dstfile->CurLen) < MAX_FILENAME_LEN);
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->dstfile, dstfile);
        
	pPara->ret_value = app_usb_file_copy(srcfile, dstfile, pPara->src_index, pPara->dst_index );	
}

void Usb_diskcopy(STDLIBFUNCALL)
{
	char srcfile[MAX_FILENAME_LEN], dstfile[MAX_FILENAME_LEN];
	USB_COPY_PARAM OS_SPTR *pPara = (USB_COPY_PARAM OS_SPTR *)pIN;

	/* TODO: allocate it dinamically */
	assert((pPara->srcfile->CurLen) < MAX_FILENAME_LEN);
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->srcfile, srcfile);

	/* TODO: allocate it dinamically */
	assert((pPara->dstfile->CurLen) < MAX_FILENAME_LEN);
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->dstfile, dstfile);

	pPara->ret_value = app_usb_file_diskcopy(srcfile, dstfile, pPara->src_index);	
}

void Usb_delete(STDLIBFUNCALL)
{
	char filename[MAX_FILENAME_LEN];
	USB_FILE_PARAM OS_SPTR *pPara = (USB_FILE_PARAM OS_SPTR *)pIN;

	/* TODO: allocate it dinamically */
	assert((pPara->filename->CurLen) < MAX_FILENAME_LEN);
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->filename, filename);

	pPara->ret_value = app_usb_file_delete(filename, pPara->index);	
}

void Usb_mkdir(STDLIBFUNCALL)
{
	char dirname[MAX_FILENAME_LEN];
	USB_FILE_PARAM OS_SPTR *pPara = (USB_FILE_PARAM OS_SPTR *)pIN;

	/* TODO: allocate it dinamically */
	assert((pPara->filename->CurLen) < MAX_FILENAME_LEN);
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->filename, dirname);

	pPara->ret_value = app_usb_dir_create(dirname, pPara->index);	
}

void Usb_status(STDLIBFUNCALL)
{
	USB_GET_STATUS_PARAM OS_SPTR *pPara = (USB_GET_STATUS_PARAM OS_SPTR *)pIN;
	USB_GET_STATUS_ARRAY_PARAM OS_DPTR *pStatus = (USB_GET_STATUS_ARRAY_PARAM OS_DPTR* )pPara->status;
	
	app_usb_status_read();
	OS_MEMCPY(USBstatus, app_usb_status, sizeof(USBstatus));
	OS_MEMCPY(pStatus->pElem, USBstatus, sizeof(pStatus->pElem));
	pPara->ret_value = 1;
}

void Usb_feedback(STDLIBFUNCALL)
{
	USB_GET_FEEDBACK_PARAM OS_SPTR *pPara = (USB_GET_FEEDBACK_PARAM OS_SPTR *)pIN;
	USB_GET_FEEDBACK_ARRAY_PARAM OS_DPTR *pFeedBack = (USB_GET_FEEDBACK_ARRAY_PARAM OS_DPTR* )pPara->feedback;
	
	OS_MEMCPY(USBfeedback, app_usb_feedback, 2);
	OS_MEMCPY(pFeedBack->pElem, USBfeedback, 2);
	pPara->ret_value = 1;
}

