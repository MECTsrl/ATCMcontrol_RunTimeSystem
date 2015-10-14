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
 * Filename: kpdImpl.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"canImpl.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"
#include "fcDef.h"

//#include <sys/io.h> // ioperm()

#if defined(RTS_CFG_IOKEYPAD)

#include <tslib.h>
#include "kpdMain.h"
#include "iolDef.h"
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <linux/input.h>

#include "mectMain.h"

#include "vmmDef.h"
#include "vmmMain.h"
/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define INPUT_BASE  (pIO->I.pAdr + pIO->I.ulOffs) 
#define OUTPUT_BASE (pIO->Q.pAdr + pIO->Q.ulOffs) 

/* each variable from PLC use 1 DWORD 
 * of the first  DWORD is used only 1 byte for the ascii code of the key
 * of the second DWORD is used only 2 byte for the short int that rapresent the X offset of the touch screen
 * of the third  DWORD is used only 2 byte for the short int that rapresent the Y offset of the touch screen
 * of the fourth DWORD is used only 1 bit for the status of key-pressed
 * of the fifth  DWORD is used only 1 bit for the status of ts-pressed
 */
#define KEY_PRESSED_OFFSET 0
#define LAST_POS_X_OFFSET  (KEY_PRESSED_OFFSET + sizeof(IEC_DWORD))
#define LAST_POS_Y_OFFSET  (LAST_POS_X_OFFSET  + sizeof(IEC_DWORD))
#define KEY_STATUS_OFFSET  (LAST_POS_Y_OFFSET  + sizeof(IEC_DWORD))
#define TS_STATUS_OFFSET   (KEY_STATUS_OFFSET  + sizeof(IEC_DWORD))
#define KEYBOARD    "/dev/input/event0"
#define TOUCHSCREEN "/dev/input/ts0"
#define BUZZER      "/dev/buzzer"
#define DURATION 120

//#define FC_KEYPAD_DEBUG
#undef FC_BEEP_DEBUG

#undef BUZZER_DISABLED

/* ----  Global Variables:	 -------------------------------------------------- */

static IEC_BOOL g_bInitialized	= FALSE;
static IEC_BOOL g_bConfigured	= FALSE;
static IEC_BOOL g_bRunning	= FALSE;
static IEC_BOOL g_bFirstStart	= FALSE;

//#if defined(CIFLESS_IO)
//static IOINFO ioInfo;
//#endif

static pthread_mutex_t mReadMutex;
static pthread_mutex_t mWriteMutex;

struct tsdev *tsfd;
static int kbdfd;
int Buzzerfd;

/* ----  Local Functions:	--------------------------------------------------- */
int readInputKpd(char *keyCode, short * pressed);
int readInputTS(short *x, short *y, short* pressed);
int writeInputTS(short x, short y);
int writeInputKpd(char keyCode);
/* ----  Implementations:	--------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/**
 * kpdInitialize
 *
 */
IEC_UINT kpdInitialize(IEC_UINT uIOLayer)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_IO_TRACE)
	osTrace("[%s] - Keypad is instantiated as IO layer %d.\n", __func__, uIOLayer );
#endif

#if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo(TASK_OFFS_IOL_KPD, osGetTaskID());
	TR_RET(uRes);
#endif

	// initialize mutexes
	if (pthread_mutex_init(&mReadMutex, NULL) != 0) {
#if defined(RTS_CFG_IO_TRACE)
		osTrace("[%s] - ERROR pthread_mutex_init (mReadMutex): %s.\n", __func__, strerror(errno));
#endif
		uRes = ERR_FB_INIT;
		goto exit_function;
	}
	if (pthread_mutex_init(&mWriteMutex, NULL) != 0) {
#if defined(RTS_CFG_IO_TRACE)
		osTrace("[%s] - ERROR pthread_mutex_init (mWriteMutex): %s.\n", __func__, strerror(errno));
#endif
		uRes = ERR_FB_INIT;
		goto exit_function;
	}

	/* open the keyboard in non-blocking*/
	kbdfd = open(KEYBOARD, O_RDONLY | O_NONBLOCK);

	if (kbdfd < 0) {
#if defined(RTS_CFG_IO_TRACE)
		osTrace("[%s] - ERROR can't open keyboard device %s\n", __func__, KEYBOARD);
#endif
		uRes = ERR_FB_INIT;
		goto exit_function;
	}

	/* open the touchscreen in non-blocking */
	if( getenv("TSLIB_TSDEVICE") != NULL ) {
		tsfd = ts_open (getenv("TSLIB_TSDEVICE"), 1);
	} else {
		tsfd = ts_open (TOUCHSCREEN, 1);
	}

	if (tsfd < 0) {
#if defined(RTS_CFG_IO_TRACE)
		osTrace("[%s] - ERROR can't open touchscreen device %s [%s]\n", __func__, TOUCHSCREEN, strerror(errno));
#endif
		uRes = ERR_FB_INIT;
		goto exit_function;
	}

	if (ts_config(tsfd)) {
#if defined(RTS_CFG_IO_TRACE)
		osTrace("[%s] - ERROR can't config touchscreen device [%s]\n", __func__, strerror(errno));
#endif
		uRes = ERR_FB_INIT;
		goto exit_function;
	}
	/* opening BUZZER file descriptor */
#ifdef BUZZER_DISABLED
	Buzzerfd = -1;
#else
	Buzzerfd = open(BUZZER, O_RDWR);
#endif

	if (Buzzerfd < 0) {
#if defined(RTS_CFG_IO_TRACE)
		osTrace("[%s] - ERROR can't open Buzzer device %s\n", __func__, BUZZER);
#endif
	}

exit_function:
	g_bInitialized	= (IEC_BOOL)(uRes == OK);
	g_bFirstStart	= TRUE;
	g_bConfigured	= FALSE;
	g_bRunning	= FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * kpdFinalize
 *
 */
IEC_UINT kpdFinalize(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_SYSLOAD)
	uRes = ldClearTaskInfo(TASK_OFFS_IOL_KPD);
	TR_RET(uRes);
#endif

	if (pthread_mutex_destroy(&mReadMutex) != 0)
	{
#if defined(RTS_CFG_IO_TRACE)
		osTrace("[%s] - ERROR pthread_mutex_destroy (mReadMutex): %s.\n", __func__, strerror(errno));
#endif
	}
	if (pthread_mutex_destroy(&mWriteMutex) != 0)
	{
#if defined(RTS_CFG_IO_TRACE)
		osTrace("[%s] - ERROR pthread_mutex_destroy (mWriteMutex): %s.\n", __func__, strerror(errno));
#endif
	}

	/* close the keyboard */
	close(kbdfd);
	kbdfd = -1;

	/* close the buzzer */
	if (Buzzerfd != -1)
	{
		close(Buzzerfd);
		Buzzerfd = -1;
	}

	g_bInitialized	= FALSE;
	g_bConfigured	= FALSE;
	g_bRunning		= FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * kpdNotifyConfig
 *
 */
IEC_UINT kpdNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_IO_TRACE)
	osTrace("[%s] - enter.\n", __func__);
#endif
	g_bConfigured	= (IEC_BOOL)(uRes == OK);
	g_bRunning		= FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * kpdNotifyStart
 *
 */
IEC_UINT kpdNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_IO_TRACE)
	osTrace("[%s] - enter.\n", __func__);
#endif
	g_bRunning = (IEC_BOOL)(uRes == OK);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * kpdNotifyStop
 *
 */
IEC_UINT kpdNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

#if defined(RTS_CFG_IO_TRACE)
	osTrace("[%s] - enter.\n", __func__);
#endif
	g_bRunning = FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * kpdNotifySet
 *
 */
IEC_UINT kpdNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;
	char keyCode = ' ';
	short x = 0, y = 0;

	if (FB_STATE_OPERATING != FB_STATE_OPERATING)
	{
		/* PB is not operating
		 */
		RETURN(ERR_FB_NOT_OPERATING);
	}

#if 0
	if (pNotify->uTask != 0xffffu)
	{
		/* A IEC task has written into the output segment.
		 * --------------------------------------------------------------------
		 */

	} /* if (pNotify->uTask != 0xffffu) */

	else
	{
		/* An external application (i.e. the 4C Watch) has written
		 * into the output or input segment.
		 * --------------------------------------------------------------------
		 */

	} /* else (pNotify->uTask != 0xffffu) */
#endif

	pthread_mutex_lock(&mWriteMutex);

	/* operation to do */

#ifdef FC_KEYPAD_DEBUG
	printf("[%s] elaborating readInputKpd data\n", __func__);	
#endif
	if (OS_MEMCMP ( OUTPUT_BASE + KEY_PRESSED_OFFSET, &keyCode, sizeof(char) ) != 0)
	{
		/* the data change, copy the new value and sign for update */
		OS_MEMCPY ( &keyCode, OUTPUT_BASE + KEY_PRESSED_OFFSET, sizeof(char));

		if (writeInputKpd(keyCode) != 0)
		{
			pthread_mutex_unlock(&mWriteMutex);
			RETURN(ERR_FB_NOT_OPERATING);
		}

#ifdef FC_KEYPAD_DEBUG
		printf("[%s] written %x [%c]\n", __func__, keyCode, keyCode);
#endif
	}

#ifdef FC_KEYPAD_DEBUG
	printf("[%s] elaborating readInputKpd data\n", __func__);	
#endif
	if (OS_MEMCMP ( OUTPUT_BASE + LAST_POS_X_OFFSET, &x, sizeof(short) ) != 0 || OS_MEMCMP ( OUTPUT_BASE + LAST_POS_Y_OFFSET, &y, sizeof(char) ))
	{
		/* the data change, copy the new value and sign for update */
		OS_MEMCPY ( &x, OUTPUT_BASE + LAST_POS_X_OFFSET, sizeof(short));
		OS_MEMCPY ( &y, OUTPUT_BASE + LAST_POS_Y_OFFSET, sizeof(short));
		
		if (writeInputTS(x, y) != 0)
		{
			pthread_mutex_unlock(&mWriteMutex);
			RETURN(ERR_FB_NOT_OPERATING);
		}

#ifdef FC_KEYPAD_DEBUG
		printf("[%s] written %x [%d]; %x [%d]\n", __func__, x, y, x, y);
#endif
	}

	pthread_mutex_unlock(&mWriteMutex);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * kpdNotifyGet
 *
 */
IEC_UINT kpdNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;
	char keyCode = ' ';
	short pressed = 0;
	short x = 0, y = 0;

	if (FB_STATE_OPERATING != FB_STATE_OPERATING)
	{
		/* PB is not operating
		 */
		RETURN(ERR_FB_NOT_OPERATING);
	}

#if 0
	if (pNotify->uTask != 0xffffu)
	{
		/* A IEC task is going to read from the input and/or output segment.
		 * --------------------------------------------------------------------
		 */

	} /* if (pNotify->uTask != 0xffffu) */

	else
	{
		/* An external application (r.e. the 4C Watch) is going to read from
		 * the input and/or output segment.
		 * --------------------------------------------------------------------
		 */

	} /* else (pNotify->uTask != 0xffffu) */
#endif

	pthread_mutex_lock(&mReadMutex);

	/* operation to do */
	if ( readInputKpd(&keyCode, &pressed) == 0)
	{
#ifdef FC_KEYPAD_DEBUG
		printf("[%s] elaborating readInputKpd data\n", __func__);	
#endif
//		printf("INPUT_BASE = %p + %p\n", pIO->I.pAdr, pIO->I.ulOffs); 
//		printf("OUTPUT_BASE = %p + %p\n", pIO->Q.pAdr, pIO->Q.ulOffs); 
//		printf("INPUT_BASE + KEY_PRESSED_OFFSET = %p\n", INPUT_BASE + KEY_PRESSED_OFFSET);
//		if (OS_MEMCMP ( INPUT_BASE + KEY_PRESSED_OFFSET, &keyCode, sizeof(char) ) != 0)
		{
			/* the data change, copy the new value and sign for update */
			OS_MEMCPY ( INPUT_BASE + KEY_PRESSED_OFFSET, &keyCode, sizeof(char));
			OS_MEMCPY ( INPUT_BASE + KEY_STATUS_OFFSET, &pressed, sizeof(char));
#ifdef FC_KEYPAD_DEBUG
			printf("[%s] - read %x [%c] pressed %d\n", __func__, keyCode, keyCode, pressed);
#endif
		}
	}

	if ( readInputTS(&x, &y, &pressed) == 0 )
	{
#ifdef FC_KEYPAD_DEBUG
		printf("[%s] elaborating readInputTS data\n", __func__);	
#endif
//		if (OS_MEMCMP ( INPUT_BASE + LAST_POS_X_OFFSET, &x, 2 * sizeof(char) ) != 0 || OS_MEMCMP ( INPUT_BASE + LAST_POS_Y_OFFSET, &y, 2 * sizeof(char) ))
		{
			/* the data change, copy the new value and sign for update */
			OS_MEMCPY ( INPUT_BASE + LAST_POS_X_OFFSET, &x, sizeof(short));
			OS_MEMCPY ( INPUT_BASE + LAST_POS_Y_OFFSET, &y, sizeof(short));
			OS_MEMCPY ( INPUT_BASE + TS_STATUS_OFFSET, &pressed, sizeof(char));
#ifdef FC_KEYPAD_DEBUG
			if (pressed)
			{
			printf("[%s] read x = %d [%x]; y = %d [%x] pressed = %d [%x]\n", __func__, x, x, y, y, pressed, pressed);
			printf("[%s] read x = %d [%x]; y = %d [%x] pressed = %d [%x]\n", __func__,
					*((short*)(INPUT_BASE + LAST_POS_X_OFFSET)),
					*((short*)(INPUT_BASE + LAST_POS_X_OFFSET)),
					*((short*)(INPUT_BASE + LAST_POS_Y_OFFSET)),
					*((short*)(INPUT_BASE + LAST_POS_Y_OFFSET)),
					*((short*)(INPUT_BASE + TS_STATUS_OFFSET)),
					*((short*)(INPUT_BASE + TS_STATUS_OFFSET)));
			}
//			printf("x .%x.%x.\n",*(INPUT_BASE + LAST_POS_X_OFFSET+1), *(INPUT_BASE + LAST_POS_X_OFFSET)); 
//			printf("y .%x.%x.\n",*(INPUT_BASE + LAST_POS_Y_OFFSET+1), *(INPUT_BASE + LAST_POS_Y_OFFSET)); 
#endif
		}
	}

	pthread_mutex_unlock(&mReadMutex);

	RETURN(uRes);
}

/* readInputKpd:
 * read the input key pressed from the keypad driver
 */
int readInputKpd(char *keyCode, short *pressed)
{
	*pressed = 0;
	*keyCode = 0;
	int ret = 0;
	struct input_event ev;

#ifdef FC_KEYPAD_DEBUG
	printf("[%s]\n", __func__);
#endif
	ret = read(kbdfd, &ev, sizeof(struct input_event));

	if (ret < 0)
	{
#ifdef FC_KEYPAD_DEBUG
		printf("[%s] - cannot read: %s.\n", __func__, strerror(errno));
#endif
		return -1;
	}
	else if (ret == 0)
	{
		/* nothing to read */
#ifdef FC_KEYPAD_DEBUG
//		printf("[%s] - nothing to read.\n", __func__);
#endif
		return 0;
	}

	if (ret != sizeof(struct input_event)) {
#ifdef FC_KEYPAD_DEBUG
		printf("%s: short key read: %d(%d)\n", __func__, ret, sizeof(struct input_event));
#endif
		return -2;
	}

	if (ev.value == 0)          /* Key released */
	{
		*pressed = 0;
	}
	else if (ev.value == 1)     /* Key pressed but not released */
	{
		*pressed = 1;
		if (Buzzerfd != -1 && ioctl(Buzzerfd, BUZZER_BEEP, DURATION) != 0)
		{
#ifdef FC_KEYPAD_DEBUG
			printf("[%s] - cannot play the buzzer.\n", __func__);
#endif
		}
#ifdef FC_BEEP_DEBUG
		else
		{
			printf("[%s] - K Beeeep 1 !\n", __func__);
		}
#endif
	}
	else if (ev.value == 2)     /* Key pressed but not released */
	{
		*pressed = 1;
		if (Buzzerfd != -1 && ioctl(Buzzerfd, BUZZER_BEEP, DURATION) != 0)
		{
#ifdef FC_KEYPAD_DEBUG
			printf("[%s] - cannot play the buzzer.\n", __func__);
#endif
		}
#ifdef FC_BEEP_DEBUG
		else
		{
			printf("[%s] - K Beeeep 2 !\n", __func__);
		}
#endif
		return 0;               /* Ignore autorepeat */
	}
	else
	{
#ifdef FC_KEYPAD_DEBUG
		printf("[%s] - could not understand the type of event %d, ret = %d\n", __func__, ev.value, ret);
#endif
		return -1;
	}

#ifdef FC_KEYPAD_DEBUG
	printf("[%s] - read .%c. .%X. pressed %d\n", __func__, ev.code, ev.code, *pressed);
#endif
	*keyCode = ev.code;
	return 0;
}

/* readInputTS:
 * read the input x y pressed from the touchscreen driver
 */
int readInputTS(short *x, short *y, short *pressed)
{
	struct ts_sample samp;
	int ret;
	static int last_pressed = 0;

	*x = 0;
	*y = 0;
	*pressed = 0;

#ifdef FC_KEYPAD_DEBUG
	printf("[%s]\n", __func__);
#endif
	ret = ts_read(tsfd, &samp, 1);

	if (ret < 0)
	{
#ifdef FC_KEYPAD_DEBUG
		printf("[%s] - ts_read: %s.\n", __func__, strerror(errno));
#endif
		return -1;
	}
	else if (ret == 0)
	{
		/* nothing to read */
#ifdef FC_KEYPAD_DEBUG
//		printf("[%s] - nothing to read.\n", __func__);
#endif
		return 0;
	}

#ifdef FC_KEYPAD_DEBUG
	printf("[%s] - x: %6d y: %6d pressed: %6d\n", __func__, samp.x, samp.y, samp.pressure);
#endif
	*x = samp.x;
	*y = samp.y;
	*pressed = samp.pressure;

	if ( (*pressed) == 1 && last_pressed == 0)
	{
		if ( Buzzerfd != -1 && ioctl(Buzzerfd, BUZZER_BEEP, DURATION) != 0)
		{
#ifdef FC_KEYPAD_DEBUG
			printf("[%s] - cannot play the buzzer.\n", __func__);
#endif
		}
#ifdef FC_BEEP_DEBUG
		else
		{
			printf("[%s] - T Beeeep!\n", __func__);
		}
#endif
	}
	last_pressed = *pressed;

	return 0;
}

/* writeInputKpd:
 *  write the key ASCII code received from the PLC into the global key structure
 */
int writeInputKpd(char keyCode)
{
	return 0;
}

/* writeInputTS:
 *  write the key ASCII code received from the PLC into the global key structure
 */
int writeInputTS(short x, short y)
{
	return 0;
}

#endif /* RTS_CFG_IOKEYPAD */

/* ---------------------------------------------------------------------------- */

