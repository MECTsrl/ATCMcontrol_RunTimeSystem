
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
 * Filename: libUtil.h
 */


#ifndef _LIBUTIL_H_
#define _LIBUTIL_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if defined(RTS_CFG_UTILITY_LIB)

void copy_dword_from_byte_array(STDLIBFUNCALL);
void copy_dword_to_byte_array(STDLIBFUNCALL);
void copy_lword_from_byte_array(STDLIBFUNCALL);
void copy_lword_to_byte_array(STDLIBFUNCALL);
void copy_int_to_byte_array(STDLIBFUNCALL);
void copy_int_from_byte_array(STDLIBFUNCALL);
void copy_dint_to_byte_array(STDLIBFUNCALL);
void copy_dint_from_byte_array(STDLIBFUNCALL);
void copy_lint_to_byte_array(STDLIBFUNCALL);
void copy_lint_from_byte_array(STDLIBFUNCALL);
void copy_lreal_to_byte_array(STDLIBFUNCALL);
void copy_lreal_from_byte_array(STDLIBFUNCALL);
void copy_real_to_byte_array(STDLIBFUNCALL);
void copy_real_from_byte_array(STDLIBFUNCALL);
void copy_bool_to_byte_array(STDLIBFUNCALL);
void copy_bool_from_byte_array(STDLIBFUNCALL);
void copy_time_from_byte_array(STDLIBFUNCALL);
void copy_time_to_byte_array(STDLIBFUNCALL);
void copy_byte_from_byte_array(STDLIBFUNCALL);
void copy_byte_to_byte_array(STDLIBFUNCALL);
void copy_word_from_byte_array(STDLIBFUNCALL);
void copy_word_to_byte_array(STDLIBFUNCALL);
void copy_string_to_byte_array(STDLIBFUNCALL);
void copy_string_from_byte_array(STDLIBFUNCALL); 
void byte_swap_word(STDLIBFUNCALL);
void byte_swap_dword(STDLIBFUNCALL);
void byte_swap_lword(STDLIBFUNCALL);
void hi_byte(STDLIBFUNCALL);
void hi_word(STDLIBFUNCALL);
void hi_dword(STDLIBFUNCALL);
void lo_byte(STDLIBFUNCALL);
void lo_word(STDLIBFUNCALL);
void lo_dword(STDLIBFUNCALL);
void make_word(STDLIBFUNCALL);
void make_dword(STDLIBFUNCALL);
void make_lword(STDLIBFUNCALL);


#if defined(IP_CFG_AIS8)

#define GET_AR_LEN(pArray)			(*(IEC_USINT OS_DPTR *)pArray)
#define GET_AR_DAT(pArray)			((IEC_BYTE OS_DPTR *)(pArray) + sizeof(IEC_USINT))

#endif

#if defined(IP_CFG_AIS16)

#define GET_AR_LEN(pArray)			(*(IEC_UINT OS_DPTR *)pArray)
#define GET_AR_DAT(pArray)			((IEC_BYTE OS_DPTR *)pArray + sizeof(IEC_UINT))

#endif

#define RTS_PRAGMA_PACK_1	/* >>>> Align 1 Begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_1

#if defined(IP_CFG_DWORD) && defined(IP_CFG_BYTE)
typedef struct
{
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);
	DEC_FUN_DWORD(retVal);

} COPY_DWORD_FROM_BA_PAR;

typedef struct
{
	DEC_FUN_DWORD(inVal);
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);

} COPY_DWORD_TO_BA_PAR;
#endif

#if defined(IP_CFG_BYTE) && defined(IP_CFG_LWORD)
typedef struct
{
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);
	DEC_FUN_LWORD(retVal);

} COPY_LWORD_FROM_BA_PAR;

typedef struct
{
	DEC_FUN_LWORD(inVal);
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);

} COPY_LWORD_TO_BA_PAR;
#endif

#if defined(IP_CFG_BYTE) && defined(IP_CFG_INT)
typedef struct
{
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);
	DEC_FUN_DINT(retVal);

} COPY_INT_FROM_BA_PAR;

typedef struct
{
	DEC_FUN_INT(inVal);
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);

} COPY_INT_TO_BA_PAR;
#endif

#if defined(IP_CFG_BYTE) && defined(IP_CFG_DINT)
typedef struct
{
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);
	DEC_FUN_DINT(retVal);

} COPY_DINT_FROM_BA_PAR;

typedef struct
{
	DEC_FUN_DINT(inVal);
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);

} COPY_DINT_TO_BA_PAR;
#endif

#if defined(IP_CFG_LINT) && defined(IP_CFG_BYTE)
typedef struct
{
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);
	DEC_FUN_LINT(retVal);

} COPY_LINT_FROM_BA_PAR;

typedef struct
{
	DEC_FUN_LINT(inVal);
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);

} COPY_LINT_TO_BA_PAR;
#endif

#if defined(IP_CFG_LREAL) && defined(IP_CFG_BYTE)
typedef struct
{
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);
	DEC_FUN_LREAL(retVal);

} COPY_LREAL_FROM_BA_PAR;

typedef struct
{
	DEC_FUN_LREAL(inVal);
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);

} COPY_LREAL_TO_BA_PAR;
#endif

#if defined(IP_CFG_REAL) && defined(IP_CFG_BYTE)
typedef struct
{
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);
	DEC_FUN_REAL(retVal);

} COPY_REAL_FROM_BA_PAR;

typedef struct
{
	DEC_FUN_REAL(inVal);
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);

} COPY_REAL_TO_BA_PAR;
#endif

#if defined(IP_CFG_BOOL) && defined(IP_CFG_BYTE)
typedef struct
{
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);
	DEC_FUN_BOOL(retVal);

} COPY_BOOL_FROM_BA_PAR;

typedef struct
{
	DEC_FUN_BOOL(inVal);
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);

} COPY_BOOL_TO_BA_PAR;
#endif

#if defined(IP_CFG_TIME) && defined(IP_CFG_BYTE)
typedef struct
{
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);
	DEC_FUN_TIME(retVal);

} COPY_TIME_FROM_BA_PAR;

typedef struct
{
	DEC_FUN_TIME(inVal);
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);

} COPY_TIME_TO_BA_PAR;
#endif

#if defined(IP_CFG_BYTE)
typedef struct
{
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);
	DEC_FUN_BYTE(retVal);

} COPY_BYTE_FROM_BA_PAR;

typedef struct
{
	DEC_FUN_BYTE(inVal);
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);

} COPY_BYTE_TO_BA_PAR;
#endif

#if defined(IP_CFG_WORD) && defined(IP_CFG_BYTE)
typedef struct
{
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);
	DEC_FUN_WORD(retVal);

} COPY_WORD_FROM_BA_PAR;

typedef struct
{
	DEC_FUN_WORD(inVal);
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);

} COPY_WORD_TO_BA_PAR;
#endif

#if defined(IP_CFG_STRING) && defined(IP_CFG_BYTE)
typedef struct
{
	/*DEC_FUN_DINT(inNum);*/
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);
	DEC_FUN_PTR(IEC_STRING,retVal);

} COPY_STRING_FROM_BA_PAR;

typedef struct
{
	DEC_FUN_PTR(IEC_STRING,inVal);
	DEC_FUN_PTR(IEC_DINT,pPos);
	DEC_FUN_PTR(IEC_BYTE,pbArray);

} COPY_STRING_TO_BA_PAR;
#endif

#if defined(IP_CFG_WORD)
typedef struct 
{
	DEC_FUN_WORD(wVal);
	DEC_FUN_WORD(wRet);

} BYTE_SWAP_WORD_PAR;
#endif

#if defined(IP_CFG_DWORD)
typedef struct 
{
	DEC_FUN_DWORD(dwVal);
	DEC_FUN_DWORD(dwRet);
	
} BYTE_SWAP_DWORD_PAR;
#endif

#if defined(IP_CFG_LWORD)
typedef struct 
{
	DEC_FUN_LWORD(lwVal);
	DEC_FUN_LWORD(lwRet);

} BYTE_SWAP_LWORD_PAR;
#endif

#if defined(IP_CFG_BYTE) && defined(IP_CFG_WORD)
typedef struct 
{
	DEC_FUN_WORD(wVal);
	DEC_FUN_BYTE(byRet);

} HI_BYTE_PAR;
#endif

#if defined(IP_CFG_WORD) && defined(IP_CFG_DWORD)
typedef struct 
{
	DEC_FUN_DWORD(dwVal);
	DEC_FUN_WORD(wRet);

} HI_WORD_PAR;
#endif

#if defined(IP_CFG_DWORD) && defined(IP_CFG_LWORD)
typedef struct 
{
	DEC_FUN_LWORD(lwVal);
	DEC_FUN_DWORD(dwRet);

} HI_DWORD_PAR;
#endif

#if defined(IP_CFG_BYTE) && defined(IP_CFG_WORD)
typedef struct 
{
	DEC_FUN_WORD(wVal);
	DEC_FUN_BYTE(byRet);

} LO_BYTE_PAR;
#endif

#if defined(IP_CFG_WORD) && defined(IP_CFG_DWORD)
typedef struct 
{
	DEC_FUN_DWORD(dwVal);
	DEC_FUN_WORD(wRet);

} LO_WORD_PAR;
#endif

#if defined(IP_CFG_DWORD) && defined(IP_CFG_LWORD)
typedef struct 
{
	DEC_FUN_LWORD(lwVal);
	DEC_FUN_DWORD(dwRet);

} LO_DWORD_PAR;
#endif

#if defined(IP_CFG_BYTE) && defined(IP_CFG_WORD)
typedef struct 
{
	DEC_FUN_BYTE(byLO);
	DEC_FUN_BYTE(byHI);
	DEC_FUN_WORD(wRet);

} MAKE_WORD_PAR;
#endif

#if defined(IP_CFG_WORD) && defined(IP_CFG_DWORD)
typedef struct 
{
	DEC_FUN_WORD(wLO);
	DEC_FUN_WORD(wHI);
	DEC_FUN_DWORD(dwRet);

} MAKE_DWORD_PAR;
#endif

#if defined(IP_CFG_DWORD) && defined(IP_CFG_LWORD)
typedef struct 
{
	DEC_FUN_DWORD(dwLO);
	DEC_FUN_DWORD(dwHI);
	DEC_FUN_LWORD(lwRet);
	
} MAKE_LWORD_PAR;
#endif


#define RTS_PRAGMA_PACK_DEF 	
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_DEF 	/* <<<< Align 1 end <<<<<<<<<<<<<<<<<<<<<<<<<<< */

#endif	/* RTS_CFG_UTILITY_LIB */

#endif	/* _LIBUTIL_H_ */

/* ---------------------------------------------------------------------------- */
