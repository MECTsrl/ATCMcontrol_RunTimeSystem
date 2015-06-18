
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
 * Filename: fcProfi.h
 */


#ifndef _FCPROFI_H_
#define _FCPROFI_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if defined(FC_CFG_PROFIDP_LIB)

/* --- 036 -------------------------------------------------------------------- */
void PB_GetSlaveDiag(STDLIBFUNCALL);
/* --- 037 -------------------------------------------------------------------- */
void PB_GetMasterState(STDLIBFUNCALL);


#define RTS_PRAGMA_PACK_1	/* >>>> Align 1 Begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_1

/* --- 036 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_UINT(uAddr);

	DEC_FUN_PTR (IEC_DWORD,  dwState); 
	DEC_FUN_PTR (IEC_STRING, sState);
	DEC_FUN_PTR (IEC_STRING, sExDiag);

	DEC_FUN_WORD(wRet);

} S_PB_GetSlaveDiag;

/* --- 037 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_UINT, wState); 

	DEC_FUN_WORD(wRet);

} S_PB_GetMasterState;


#define RTS_PRAGMA_PACK_DEF 	
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_DEF 	/* <<<< Align 1 end <<<<<<<<<<<<<<<<<<<<<<<<<<< */

#endif /* FC_CFG_PROFIDP_LIB */

#endif /* _FCPROFI_H_ */

/* ---------------------------------------------------------------------------- */
