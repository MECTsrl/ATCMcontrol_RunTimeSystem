
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
 * Filename: libSys2.h
 */


#ifndef _LIBSYS2_H_
#define _LIBSYS2_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if defined(RTS_CFG_SYSTEM_LIB_NT)

/* --- 135 -------------------------------------------------------------------- */
void EVT_GetException(STDLIBFUNCALL);
/* --- 136 -------------------------------------------------------------------- */
void EVT_Set(STDLIBFUNCALL);
/* --- 137 -------------------------------------------------------------------- */
/* --- 138 -------------------------------------------------------------------- */
void MSG_SendToEng(STDLIBFUNCALL);
/* --- 139 -------------------------------------------------------------------- */
void MSG_Trace(STDLIBFUNCALL);
/* --- 140 -------------------------------------------------------------------- */
/* Interpreter inline code in 4C library! */
/* --- 141 -------------------------------------------------------------------- */
void TIM_Get(STDLIBFUNCALL);
/* --- 142 -------------------------------------------------------------------- */
void TSK_ClearError(STDLIBFUNCALL);
/* --- 143 -------------------------------------------------------------------- */
void TSK_Exception(STDLIBFUNCALL);
/* --- 144 -------------------------------------------------------------------- */
void TSK_GetCount(STDLIBFUNCALL);
/* --- 145 -------------------------------------------------------------------- */
void TSK_GetError(STDLIBFUNCALL);
/* --- 146 -------------------------------------------------------------------- */
void TSK_GetInfo(STDLIBFUNCALL);
/* --- 147 -------------------------------------------------------------------- */
void TSK_GetMyNumber(STDLIBFUNCALL);
/* --- 148 -------------------------------------------------------------------- */
void TSK_GetName(STDLIBFUNCALL);
/* --- 149 -------------------------------------------------------------------- */
void TSK_GetState(STDLIBFUNCALL);
/* --- 150 -------------------------------------------------------------------- */
void TSK_Start(STDLIBFUNCALL);
/* --- 151 -------------------------------------------------------------------- */
void TSK_Stop(STDLIBFUNCALL);
/* --- 152 -------------------------------------------------------------------- */
void WD_Disable(STDLIBFUNCALL);
/* --- 153 -------------------------------------------------------------------- */
void WD_Enable(STDLIBFUNCALL);
/* --- 154 -------------------------------------------------------------------- */
/* --- 155 -------------------------------------------------------------------- */
/* --- 156 -------------------------------------------------------------------- */
void EVT_GetEvent(STDLIBFUNCALL);
/* --- 157 -------------------------------------------------------------------- */
void WD_IsEnabled(STDLIBFUNCALL);
/* --- 158 -------------------------------------------------------------------- */
void TSK_GetMyName(STDLIBFUNCALL);
/* --- 159 -------------------------------------------------------------------- */
void SYS_Coldstart(STDLIBFUNCALL);
/* --- 180 -------------------------------------------------------------------- */
void SYS_Reboot(STDLIBFUNCALL);
/* --- 181 -------------------------------------------------------------------- */
void SYS_RetainWrite(STDLIBFUNCALL);
/* --- 182 -------------------------------------------------------------------- */
void SYS_Warmstart(STDLIBFUNCALL);
/* --- 183 -------------------------------------------------------------------- */
void LD_Get(STDLIBFUNCALL);
/* --- 184 -------------------------------------------------------------------- */
void WD_SetTrigger(STDLIBFUNCALL);
/* --- 185 -------------------------------------------------------------------- */
void WD_GetTrigger(STDLIBFUNCALL);
/* --- 186 -------------------------------------------------------------------- */
void TSK_GetStatistic(STDLIBFUNCALL);
/* --- 187 -------------------------------------------------------------------- */
void TSK_ClearStatistic(STDLIBFUNCALL);

#define RTS_PRAGMA_PACK_1	/* >>>> Align 1 Begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_1

/* --- 135 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_UINT,	upTask);
	DEC_FUN_PTR (IEC_UDINT, ulpEx);
	
	DEC_FUN_WORD(wRet);

} S_EVT_GetException;

/* --- 136 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_UINT(uEvent);
	
	DEC_FUN_WORD(wRet);

} S_EVT_Set;

/* --- 137 -------------------------------------------------------------------- */

/* --- 138 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sMsg);
	
} S_MSG_SendToEng;

/* --- 139 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sMsg);
	
} S_MSG_Trace;

/* --- 140 -------------------------------------------------------------------- */

/* Interpreter inline code in 4C library! */


/* --- 141 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_TIME(tRet);

} S_TIM_Get;

/* --- 142 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);

	DEC_FUN_WORD(wRet);

} S_TSK_ClearError;

/* --- 143 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_UINT(uException);

	DEC_FUN_WORD(wRet);

} S_TSK_Exception;

/* --- 144 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_INT(iRet);

} S_TSK_GetCount;

/* --- 145 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);
	DEC_FUN_PTR (IEC_UDINT,  ulErrNo);

	DEC_FUN_WORD(wRet);

} S_TSK_GetError;

/* --- 146 -------------------------------------------------------------------- */

typedef struct
{
	DEC_VAR(IEC_UDINT, ulState);
	DEC_VAR(IEC_DINT,  ulPriority);
	DEC_VAR(IEC_UDINT, tCycle);
	DEC_VAR(IEC_UDINT, tETMax);
	DEC_VAR(IEC_DINT,  ulErrNo);

} ETaskInfo;

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);
	DEC_FUN_PTR (ETaskInfo,  pInfo);

	DEC_FUN_UINT(wRet);

} S_TSK_GetInfo;

/* --- 147 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_UINT(uTask);

} S_TSK_GetMyNumber;

/* --- 148 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_UINT(uTask);
	DEC_FUN_PTR (IEC_STRING, sTask);

	DEC_FUN_WORD(wRet);

} S_TSK_GetName;

/* --- 149 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);
	DEC_FUN_PTR (IEC_UDINT,  ulErrNo);

	DEC_FUN_WORD(wRet);

} S_TSK_GetState;

/* --- 150 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);

	DEC_FUN_WORD(wRet);

} S_TSK_Start;

/* --- 151 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);

	DEC_FUN_WORD(wRet);

} S_TSK_Stop;

/* --- 152 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);

	DEC_FUN_WORD(wRet);

} S_WD_Disable;

/* --- 153 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);

	DEC_FUN_WORD(wRet);

} S_WD_Enable;

/* --- 154 -------------------------------------------------------------------- */

/* --- 155 -------------------------------------------------------------------- */

/* --- 156 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);
	DEC_FUN_PTR (IEC_UINT,	 upEvent);

	DEC_FUN_WORD(wRet);

} S_EVT_GetEvent;

/* --- 157 -------------------------------------------------------------------- */

typedef struct 
{
	DEC_FUN_PTR (IEC_STRING, sTask);
	DEC_FUN_PTR (IEC_BOOL, bEnabled);

	DEC_FUN_WORD(wRet);

} S_WD_IsEnabled;

/* --- 158 -------------------------------------------------------------------- */

typedef struct 
{
	DEC_FUN_PTR (IEC_STRING, sTask);

} S_TSK_GetMyName;

/* --- 159 -------------------------------------------------------------------- */

typedef struct 
{
	DEC_FUN_WORD(wRet);

} S_SYS_Coldstart;

/* --- 180 -------------------------------------------------------------------- */

typedef struct 
{
	DEC_FUN_WORD(wRet);

} S_SYS_Reboot;

/* --- 181 -------------------------------------------------------------------- */

typedef struct 
{

	DEC_FUN_WORD(wRet);

} S_SYS_RetainWrite;

/* --- 182 -------------------------------------------------------------------- */

typedef struct 
{
	DEC_FUN_WORD(wRet);

} S_SYS_Warmstart;

/* --- 183 -------------------------------------------------------------------- */

typedef struct 
{
	DEC_FUN_WORD(wLoadType);
	DEC_FUN_UINT(uTaskID);

	DEC_FUN_PTR (IEC_STRING, pLoad);

	DEC_FUN_WORD(wRet);

} S_LD_Get;

/* --- 184 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);
	
	DEC_FUN_UDINT (ulWDogTrigger);

	DEC_FUN_UINT(wRet);

} S_WD_SetTrigger;

/* --- 185 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);
	
	DEC_FUN_PTR (IEC_UDINT,  ulpWDogTrigger);
	DEC_FUN_PTR (IEC_UDINT,  ulpWDogCounter);

	DEC_FUN_UINT(wRet);

} S_WD_GetTrigger;

/* --- 186 -------------------------------------------------------------------- */

typedef struct
{
	DEC_VAR(IEC_UDINT, ulTaskState);
	DEC_VAR(IEC_UDINT, ulCycleTime);
	DEC_VAR(IEC_UDINT, ulExeMax);
	DEC_VAR(IEC_UDINT, ulExeMin);
	DEC_VAR(IEC_UDINT, ulExeAver);
	DEC_VAR(IEC_UDINT, ulExeCount);
	DEC_VAR(IEC_UDINT, ulWDCounter);
	DEC_VAR(IEC_UDINT, ulWDTrigger);

} ETaskStat;

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);
	DEC_FUN_PTR (ETaskStat,  pStat);

	DEC_FUN_UINT(wRet);

} S_TSK_GetStatistic;

/* --- 187 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sTask);

	DEC_FUN_UINT(wRet);

} S_TSK_ClearStatistic;

#define RTS_PRAGMA_PACK_DEF 	
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_DEF 	/* <<<< Align 1 end <<<<<<<<<<<<<<<<<<<<<<<<<<< */

#endif	/* RTS_CFG_SYSTEM_LIB_NT */

#endif	/* _LIBSYS_H_ */

/* ---------------------------------------------------------------------------- */
