
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
 * Filename: mectMain.h
 */


#ifndef _MECTMAIN_H_
#define _MECTMAIN_H_

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif

#ifdef __cplusplus
extern "C" {
#endif
#define THREAD
/*
 * ----------------------------------------------------------------------------
 */
void *IO_Layer_UDP(void *lpParam);
void *IO_Layer_CANopen(void *lpParam);
void *IO_Layer_Keypad(void *lpParam);
void *IO_Layer_DAT(void *lpParam);
void *IO_Layer_SYN(void *lpParam);
void *IO_Layer_ModbusTCPS(void *lpParam);
void *IO_Layer_ModbusRTUC(void *lpParam);

#ifdef THREAD
#warning thread and nanosleep  are used for CanOpen implementation
extern pthread_t msg0_threadid;
extern pthread_t msg1_threadid;
extern pthread_t stack0_threadid;
extern pthread_t stack1_threadid;
#else
#warning real-time timer are used for CanOpen implementation
extern timer_t msg_timerid;
extern timer_t stack_timerid;
#endif

extern int Can0InitDone;
extern int Can1InitDone;
extern int CanDone;

#ifdef __cplusplus
}
#endif

#endif	/* _MECTMAIN_H_ */

/* ---------------------------------------------------------------------------- */
