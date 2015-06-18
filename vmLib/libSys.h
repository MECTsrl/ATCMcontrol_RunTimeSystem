
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
 * Filename: libSys.h
 */


#ifndef _LIBSYS_H_
#define _LIBSYS_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if defined(RTS_CFG_SYSTEM_LIB)

/* Function Declarations
 * ----------------------------------------------------------------------------
 */

/* Fun's and FB's also defined in PC-Target
 */
void ClearTaskErrno(STDLIBFUNCALL);
void GetLocalTaskErrno(STDLIBFUNCALL);
void GetLocalTaskState(STDLIBFUNCALL);
void GetTaskErrno(STDLIBFUNCALL);
void GetTimeSinceSystemBoot(STDLIBFUNCALL);
void OutputDebugString(STDLIBFUNCALL);
void SignalError(STDLIBFBCALL);
void StartLocalTask(STDLIBFUNCALL);
void StopLocalTask(STDLIBFUNCALL);

/* Additional Fun's for embedded targets only
 */
void ClearLocalTaskErrno(STDLIBFUNCALL);
void ThrowException(STDLIBFUNCALL);
void GetLocalTaskInfo(STDLIBFUNCALL);


#define RTS_PRAGMA_PACK_1	/* >>>> Align 1 Begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_1

typedef struct
{
	DEC_VAR(IEC_DINT, lState);
	DEC_VAR(IEC_DINT, lPriority);
	DEC_VAR(IEC_DINT, tCycle);
	DEC_VAR(IEC_DINT, tETMax);
	DEC_VAR(IEC_DINT, lErrno);

} TASKINFO;


/* Function Parameters
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	DEC_FUN_PTR (IEC_STRING, strTask);
	DEC_FUN_DINT(lRet);

} SGetLocalTaskErrno;	/* ---------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, strTask);
	DEC_FUN_DINT(lRet);

} SGetLocalTaskState;	/* ---------------------------------------------------- */

typedef struct
{
	DEC_FUN_DINT(lRet);

} SGetTaskErrno;	/* -------------------------------------------------------- */

typedef struct
{
	DEC_FUN_TIME(tRet);

} SGetTimeSinceSystemBoot;	/* ------------------------------------------------ */

typedef struct
{
	DEC_FUN_PTR(IEC_STRING, strMsg);

} SOutputDebugString;	/* ---------------------------------------------------- */

#if defined(IP_CFG_INST8)
typedef struct
{	
	DEC_VAR(IEC_BYTE, bEnable);
	DEC_VAR(IEC_DINT, lErrNo);

} SSignalError; /* ------------------------------------------------------------ */
#endif

#if defined(IP_CFG_INST16)
typedef struct
{	
	DEC_VAR(IEC_BYTE, bEnable);
	DEC_VAR(IEC_BYTE, dummy_08_bEnable);
	DEC_VAR(IEC_DINT, lErrNo);

} SSignalError; /* ------------------------------------------------------------ */
#endif

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64)
typedef struct
{	
	DEC_VAR(IEC_BYTE, bEnable);
	DEC_VAR(IEC_BYTE, dummy_08_bEnable);
	DEC_VAR(IEC_WORD, dummy_16_bEnable);
	DEC_VAR(IEC_DINT, lErrNo);

} SSignalError; /* ------------------------------------------------------------ */
#endif

typedef struct
{
	DEC_FUN_PTR(IEC_STRING, strTask);

} SStartLocalTask;	/* -------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR(IEC_STRING, strTask);

} SStopLocalTask;	/* -------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR(IEC_STRING, strTask);

} SClearLocalTaskErrno; /* ---------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR(IEC_STRING, strTask);
	DEC_FUN_PTR(TASKINFO, pInfo);

} SGetLocalTaskInfo;	/* ---------------------------------------------------- */

#define RTS_PRAGMA_PACK_DEF 	
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_DEF 	/* <<<< Align 1 end <<<<<<<<<<<<<<<<<<<<<<<<<<< */

#endif	/* RTS_CFG_SYSTEM_LIB */

#endif	/* _LIBSYS_H_ */

/* ---------------------------------------------------------------------------- */
