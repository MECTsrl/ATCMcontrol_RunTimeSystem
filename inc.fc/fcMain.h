
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
 * Filename: fcMain.h
 */


#ifndef _FCMAIN_H_
#define _FCMAIN_H_

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif

#ifdef __cplusplus
extern "C" { 
#endif


/* Adapt/fcAdapt.c
 * ----------------------------------------------------------------------------
 */
void *SOCKET_ListenThread(void *lpParam);
void *SOCKET_CommThread(void *lpParam);
void *VM_Proc(void *lpParam);
void *VMM_OnlineChangeThread(void *lpParam);
void *VM_TimerThread(void *lpParam);
void *IO_Layer_Test(void *lpParam);
void *IO_Layer_BACnet(void *lpParam);
void *IO_Layer_ProfiDP(void *lpParam);
void *VMM_RetainThread(void *lpParam);
void *BAC_DeviceThread(void *lpParam);
void *BAC_COVThread(void *lpParam);
void *BAC_ScanThread(void *lpParam);
void *BAC_FlashThread(void *lpParam);
void *BAC_ConfigThread(void *lpParam);
void *PDP_ManagementThread(void *lpParam);

void VMM_CleanUp_Mutex(void *lpParam);

void VM_CleanUp_Common(void *lpParam);


/* fcLed.c
 * ----------------------------------------------------------------------------
 */
#define FC_LED_ERROR		0
#define FC_LED_RUN			1

#define FC_LED_ON			0
#define FC_LED_OFF			1
#define FC_LED_ON_TOGGLE    2
#define FC_LED_B_FAST		3
#define FC_LED_B_SLOW		4
#define FC_LED_FLICKER		5
#define FC_LED_SHORT_OFF    6

void *LED_Thread(void *lpParam);

IEC_UINT fcSetLed(IEC_UINT uLed, IEC_UINT uState);

IEC_UINT fcSetSystemWatchdog(void);

IEC_UINT fcInitJiffies(void);


/* 
 * ----------------------------------------------------------------------------
 */
void cleanup_comm_lib(void);
void cleanup_emb_comm_lib(void);
void cleanup_mbus_lib(void);
void cleanup_modbus_lib(void);

#ifdef __cplusplus
} 
#endif

#endif	/* _FCMAIN_H_ */

/* ---------------------------------------------------------------------------- */
