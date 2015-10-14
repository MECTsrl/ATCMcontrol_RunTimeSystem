
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
 * Filename: libUSB.h
 */

#ifndef __LIB_USB_H_
#define __LIB_USB_H_

#include "stdInc.h"
#include "libDef.h"

#define APP_USB_MAX 5

typedef struct app_usb_mount_t {
	char dev[15];
	char dev1[15];
	char mpoint[15];
} app_usb_mount_s;


/* Global Variables */
unsigned short app_usb_status[APP_USB_MAX + 1];
app_usb_mount_s app_usb[APP_USB_MAX];
unsigned short app_usb_feedback[2];

/* Public Functions */
short int app_usb_status_read(void);
void app_usb_init( void );
unsigned short app_usb_umount( unsigned short index ); 
unsigned short app_usb_mount( unsigned short index );
char * app_usb_mpoint_return( unsigned short index ); 

/*FarosPLC Function view for Libraries*/
/* --- 240 -------------------------------------------------------------------- */
void Usb_on(STDLIBFUNCALL);
/* --- 241 -------------------------------------------------------------------- */
void Usb_off(STDLIBFUNCALL);
/* --- 242 -------------------------------------------------------------------- */
void Usb_copy(STDLIBFUNCALL);
/* --- 243 -------------------------------------------------------------------- */
void Usb_diskcopy(STDLIBFUNCALL);
/* --- 244 -------------------------------------------------------------------- */
void Usb_delete(STDLIBFUNCALL);
/* --- 245 -------------------------------------------------------------------- */
void Usb_mkdir(STDLIBFUNCALL);
/* --- 246-------------------------------------------------------------------- */
void Usb_status(STDLIBFUNCALL);
/* --- 247 -------------------------------------------------------------------- */
void Usb_feedback(STDLIBFUNCALL);

typedef struct
{
	DEC_FUN_UINT(index);
	/*return value*/
	DEC_FUN_UINT(ret_value);
	
}USB_INDEX_PARAM;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, srcfile);
	DEC_FUN_PTR(IEC_STRING const, dstfile);
	DEC_FUN_UINT(src_index);
	DEC_FUN_UINT(dst_index);
	/*return value*/
	DEC_FUN_UINT(ret_value);
}USB_COPY_PARAM;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, srcfile);
	DEC_FUN_PTR(IEC_STRING const, dstfile);
	DEC_FUN_UINT(src_index);
	/*return value*/
	DEC_FUN_UINT(ret_value);
}USB_DISKCOPY_PARAM;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING const, filename);
	DEC_FUN_UINT(index);
	/*return value*/
	DEC_FUN_UINT(ret_value);
}USB_FILE_PARAM;

typedef struct
{
	DEC_VAR(IEC_UINT, uLen);
	DEC_VAR(IEC_INT, pElem[APP_USB_MAX]);
	
}USB_GET_STATUS_ARRAY_PARAM;

typedef struct
{
	DEC_FUN_PTR(USB_GET_STATUS_ARRAY_PARAM, status);
	/*return value*/
	DEC_FUN_UINT(ret_value);
	
}USB_GET_STATUS_PARAM;

typedef struct
{
	DEC_VAR(IEC_UINT, uLen);
	DEC_VAR(IEC_INT, pElem[2]);
	
}USB_GET_FEEDBACK_ARRAY_PARAM;

typedef struct
{
	DEC_FUN_PTR(USB_GET_FEEDBACK_ARRAY_PARAM, feedback);
	/*return value*/
	DEC_FUN_UINT(ret_value);
	
}USB_GET_FEEDBACK_PARAM;

#endif
