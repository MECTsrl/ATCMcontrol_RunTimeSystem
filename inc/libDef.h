
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
 * Filename: libDef.h
 */


#ifndef _LIBDEF_H_
#define _LIBDEF_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif

/* Function prototypes for library functions and function blocks
 * ----------------------------------------------------------------------------
 */
#define STDLIBFUNCALL  STaskInfoVM *pVM, IEC_DATA OS_CPTR *pIP, IEC_DATA OS_SPTR *pIN
#define STDLIBFBCALL   STaskInfoVM *pVM, IEC_DATA OS_CPTR *pIP, IEC_DATA OS_DPTR *pIN

typedef void (* EXECUTE_FUN) (STDLIBFUNCALL);
typedef void (* EXECUTE_FB ) (STDLIBFBCALL );


/* Extensible parameters proccessing
 * ----------------------------------------------------------------------------
 */
#if defined(IP_CFG_STACK8)
	#define DEC_EXT_CNT(v,p)		IEC_WORD   v = (IEC_WORD)*(IEC_WORD OS_SPTR *)p
	#define DEC_EXT_PAR(v,t,p)		t OS_SPTR *v = (t OS_SPTR *)(p + sizeof(IEC_WORD)) 
	#define DEC_EXT_FUN(v,t,s,p)	s OS_SPTR *v = (s OS_SPTR *)(p + sizeof(IEC_WORD) + sizeof(t) * *(IEC_WORD OS_SPTR *)p) 
#endif

#if defined(IP_CFG_STACK16)
	#define DEC_EXT_CNT(v,p)		IEC_WORD   v = (IEC_WORD)*(IEC_WORD OS_SPTR *)p
	#define DEC_EXT_PAR(v,t,p)		t OS_SPTR *v = (t OS_SPTR *)(p + sizeof(IEC_WORD)) 
	#define DEC_EXT_FUN(v,t,s,p)	s OS_SPTR *v = (s OS_SPTR *)(p + sizeof(IEC_WORD) + sizeof(t) * *(IEC_WORD OS_SPTR *)p) 
#endif

#if defined(IP_CFG_STACK32)
	#define DEC_EXT_CNT(v,p)		IEC_WORD   v = (IEC_WORD)*(IEC_WORD OS_SPTR *)p
	#define DEC_EXT_PAR(v,t,p)		t OS_SPTR *v = (t OS_SPTR *)(p + sizeof(IEC_DWORD)) 
	#define DEC_EXT_FUN(v,t,s,p)	s OS_SPTR *v = (s OS_SPTR *)(p + sizeof(IEC_DWORD) + sizeof(t) * *(IEC_WORD OS_SPTR *)p) 
#endif

#if defined(IP_CFG_STACK64)
	#define DEC_EXT_CNT(v,p)		IEC_WORD   v = (IEC_WORD)*(IEC_WORD OS_SPTR *)p
	#define DEC_EXT_PAR(v,t,p)		t OS_SPTR *v = (t OS_SPTR *)(p + sizeof(IEC_LWORD)) 
	#define DEC_EXT_FUN(v,t,s,p)	s OS_SPTR *v = (s OS_SPTR *)(p + sizeof(IEC_LWORD) + sizeof(t) * *(IEC_WORD OS_SPTR *)p) 
#endif


/* Member variable declarations for function parameter structures
 * ----------------------------------------------------------------------------
 * NOTE: NOT to be used for function block structures!
 *
 * Functions:		Every member variable is needs at least two bytes. Bits are
 *					not packed and therefor every bit needs also two bytes on the
 *					stack.
 * Function Blocks: Up to 8 bits are packed into one byte. A classical two byte
 *					allignment is applied to the structure members representing
 *					the FB parameters. (The need of fill bytes depends on the 
 *					following variable and therefor the following macros must 
 *					not be used for function blocks!)
 */

#if defined(IP_CFG_STACK8)	/* ------------------------------------------------ */

	#define DEC_FUN_BOOL(v) 		DEC_VAR(IEC_BOOL,	v)

	#define DEC_FUN_SINT(v) 		DEC_VAR(IEC_SINT,	v)
	#define DEC_FUN_INT(v)			DEC_VAR(IEC_INT,	v)
	#define DEC_FUN_DINT(v) 		DEC_VAR(IEC_DINT,	v)
	#define DEC_FUN_LINT(v) 		DEC_VAR(IEC_LINT,	v)

	#define DEC_FUN_USINT(v)		DEC_VAR(IEC_USINT,	v)
	#define DEC_FUN_UINT(v) 		DEC_VAR(IEC_UINT,	v)
	#define DEC_FUN_UDINT(v)		DEC_VAR(IEC_UDINT,	v)
	#define DEC_FUN_ULINT(v)		DEC_VAR(IEC_ULINT,	v)

	#define DEC_FUN_REAL(v) 		DEC_VAR(IEC_REAL,	v)
	#define DEC_FUN_LREAL(v)		DEC_VAR(IEC_LREAL,	v)

	#define DEC_FUN_TIME(v) 		DEC_VAR(IEC_TIME,	v)

	#define DEC_FUN_BYTE(v) 		DEC_VAR(IEC_BYTE,	v)
	#define DEC_FUN_WORD(v) 		DEC_VAR(IEC_WORD,	v)
	#define DEC_FUN_DWORD(v)		DEC_VAR(IEC_DWORD,	v)
	#define DEC_FUN_LWORD(v)		DEC_VAR(IEC_LWORD,	v)

	#define DEC_FUN_CHAR(v) 		DEC_VAR(IEC_CHAR,	v)

  #if defined (IP_CFG_PTR8)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v)
			
	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo)
  #elif defined (IP_CFG_PTR16)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_BYTE,	dummy_08_##bo)
  #elif defined (IP_CFG_PTR32)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_BYTE,	dummy_08_##bo); \
									DEC_VAR(IEC_WORD,	dummy_16_##bo)
  #elif defined (IP_CFG_PTR64)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_BYTE,	dummy_08_##bo); \
									DEC_VAR(IEC_WORD,	dummy_16_##bo); \
									DEC_VAR(IEC_DWORD,	dummy_32_##bo)
  #endif

#endif /* IP_CFG_STACK8 */

#if defined(IP_CFG_STACK16) /* ------------------------------------------------ */

	#define DEC_FUN_BOOL(v) 		DEC_VAR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v)

	#define DEC_FUN_SINT(v) 		DEC_VAR(IEC_SINT,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v)
	#define DEC_FUN_INT(v)			DEC_VAR(IEC_INT,	v)
	#define DEC_FUN_DINT(v) 		DEC_VAR(IEC_DINT,	v)
	#define DEC_FUN_LINT(v) 		DEC_VAR(IEC_LINT,	v)

	#define DEC_FUN_USINT(v)		DEC_VAR(IEC_USINT,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v)
	#define DEC_FUN_UINT(v) 		DEC_VAR(IEC_UINT,	v)
	#define DEC_FUN_UDINT(v)		DEC_VAR(IEC_UDINT,	v)
	#define DEC_FUN_ULINT(v)		DEC_VAR(IEC_ULINT,	v)

	#define DEC_FUN_REAL(v) 		DEC_VAR(IEC_REAL,	v)
	#define DEC_FUN_LREAL(v)		DEC_VAR(IEC_LREAL,	v)

	#define DEC_FUN_TIME(v) 		DEC_VAR(IEC_TIME,	v)

	#define DEC_FUN_BYTE(v) 		DEC_VAR(IEC_BYTE,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v)
	#define DEC_FUN_WORD(v) 		DEC_VAR(IEC_WORD,	v)
	#define DEC_FUN_DWORD(v)		DEC_VAR(IEC_DWORD,	v)
	#define DEC_FUN_LWORD(v)		DEC_VAR(IEC_LWORD,	v)

	#define DEC_FUN_CHAR(v) 		DEC_VAR(IEC_CHAR,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v)

  #if defined (IP_CFG_PTR8)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo)
  #elif defined (IP_CFG_PTR16)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_BYTE,	dummy_08_##bo)
  #elif defined (IP_CFG_PTR32)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_BYTE,	dummy_08_##bo); \
									DEC_VAR(IEC_WORD,	dummy_16_##bo)
  #elif defined (IP_CFG_PTR64)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_BYTE,	dummy_08_##bo); \
									DEC_VAR(IEC_WORD,	dummy_16_##bo); \
									DEC_VAR(IEC_DWORD,	dummy_32_##bo)									
  #endif

#endif /* IP_CFG_STACK16 */

#if defined(IP_CFG_STACK32) /* ------------------------------------------------ */

	#define DEC_FUN_BOOL(v) 		DEC_VAR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v)

	#define DEC_FUN_SINT(v) 		DEC_VAR(IEC_SINT,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v)
	#define DEC_FUN_INT(v)			DEC_VAR(IEC_INT,	v); 			\
									DEC_VAR(IEC_WORD,	dummy_16_##v)
	#define DEC_FUN_DINT(v) 		DEC_VAR(IEC_DINT,	v)
	#define DEC_FUN_LINT(v) 		DEC_VAR(IEC_LINT,	v)

	#define DEC_FUN_USINT(v)		DEC_VAR(IEC_USINT,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v)
	#define DEC_FUN_UINT(v) 		DEC_VAR(IEC_UINT,	v); 			\
									DEC_VAR(IEC_WORD,	dummy_16_##v)
	#define DEC_FUN_UDINT(v)		DEC_VAR(IEC_UDINT,	v)
	#define DEC_FUN_ULINT(v)		DEC_VAR(IEC_ULINT,	v)

	#define DEC_FUN_REAL(v) 		DEC_VAR(IEC_REAL,	v)
	#define DEC_FUN_LREAL(v)		DEC_VAR(IEC_LREAL,	v)

	#define DEC_FUN_TIME(v) 		DEC_VAR(IEC_TIME,	v)

	#define DEC_FUN_BYTE(v) 		DEC_VAR(IEC_BYTE,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v)
	#define DEC_FUN_WORD(v) 		DEC_VAR(IEC_WORD,	v); 			\
									DEC_VAR(IEC_WORD,	dummy_16_##v)
	#define DEC_FUN_DWORD(v)		DEC_VAR(IEC_DWORD,	v)
	#define DEC_FUN_LWORD(v)		DEC_VAR(IEC_LWORD,	v)

	#define DEC_FUN_CHAR(v) 		DEC_VAR(IEC_CHAR,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v)
	
  #if defined (IP_CFG_PTR8)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_WORD,	dummy_16_##v)
   
  #elif defined (IP_CFG_PTR16)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v); 			\
									DEC_VAR(IEC_WORD,	dummy_16_##v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v)
  #elif defined (IP_CFG_PTR32)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_BYTE,	dummy_08_##bo); \
									DEC_VAR(IEC_WORD,	dummy_16_##bo)
  #elif defined (IP_CFG_PTR64)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_BYTE,	dummy_08_##bo); \
									DEC_VAR(IEC_WORD,	dummy_16_##bo); \
									DEC_VAR(IEC_DWORD,	dummy_32_##bo)
  #endif

#endif /* IP_CFG_STACK32 */

#if defined(IP_CFG_STACK64) /* ------------------------------------------------ */

	#define DEC_FUN_BOOL(v) 		DEC_VAR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)

	#define DEC_FUN_SINT(v) 		DEC_VAR(IEC_SINT,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
	#define DEC_FUN_INT(v)			DEC_VAR(IEC_INT,	v); 			\
									DEC_VAR(IEC_WORD,	dummy_16_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
	#define DEC_FUN_DINT(v) 		DEC_VAR(IEC_DINT,	v); 			\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
	#define DEC_FUN_LINT(v) 		DEC_VAR(IEC_LINT,	v)

	#define DEC_FUN_USINT(v)		DEC_VAR(IEC_USINT,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
	#define DEC_FUN_UINT(v) 		DEC_VAR(IEC_UINT,	v); 			\
									DEC_VAR(IEC_WORD,	dummy_16_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
	#define DEC_FUN_UDINT(v)		DEC_VAR(IEC_UDINT,	v); 			\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
	#define DEC_FUN_ULINT(v)		DEC_VAR(IEC_ULINT,	v)

	#define DEC_FUN_REAL(v) 		DEC_VAR(IEC_REAL,	v); 			\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
	#define DEC_FUN_LREAL(v)		DEC_VAR(IEC_LREAL,	v)

	#define DEC_FUN_TIME(v) 		DEC_VAR(IEC_TIME,	v); 			\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)

	#define DEC_FUN_BYTE(v) 		DEC_VAR(IEC_BYTE,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
	#define DEC_FUN_WORD(v) 		DEC_VAR(IEC_WORD,	v); 			\
									DEC_VAR(IEC_WORD,	dummy_16_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
	#define DEC_FUN_DWORD(v)		DEC_VAR(IEC_DWORD,	v); 			\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
	#define DEC_FUN_LWORD(v)		DEC_VAR(IEC_LWORD,	v)

	#define DEC_FUN_CHAR(v) 		DEC_VAR(IEC_CHAR,	v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)

  #if defined (IP_CFG_PTR8)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v); 			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_WORD,	dummy_16_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
  #elif defined (IP_CFG_PTR16)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v); 			\
									DEC_VAR(IEC_WORD,	dummy_16_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
  #elif defined (IP_CFG_PTR32)
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v); 			\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v)
  #else
	#define DEC_FUN_PTR(type,v) 	DEC_PTR(type,		v)

	#define DEC_FUN_BITPTR(v,bo)	DEC_PTR(IEC_BOOL,	v); 			\
									DEC_VAR(IEC_BYTE,	bo);			\
									DEC_VAR(IEC_BYTE,	dummy_08_##v);	\
									DEC_VAR(IEC_WORD,	dummy_16_##v);	\
									DEC_VAR(IEC_DWORD,	dummy_32_##v)
  #endif

#endif /* IP_CFG_STACK64 */


#define SET_BIT(v,bo,value) 	(v)[(bo)/8] = ((IEC_BOOL)((value) ?  ((v)[(bo)/8] | 1<<((bo)%8)) : ((v)[(bo)/8] & ~(1<<((bo)%8)))))
#define GET_BIT(v,bo)			((((v)[(bo)/8] & (1<<((bo)%8))) > 0) ? TRUE : FALSE)


/* Helper structures to access extensible parameters
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	DEC_FUN_BYTE(var);

} SExtDec_BYTE;

typedef struct
{
	DEC_FUN_WORD(var);

} SExtDec_WORD;

typedef struct
{
	DEC_FUN_DWORD(var);

} SExtDec_DWORD;

/* LWORD: 64 bit data types can be used directly!
 */

typedef struct
{
	DEC_FUN_SINT(var);

} SExtDec_SINT;

typedef struct
{
	DEC_FUN_INT(var);

} SExtDec_INT;

typedef struct
{
	DEC_FUN_DINT(var);

} SExtDec_DINT;

/* LINT: 64 bit data types can be used directly!
 */

typedef struct
{
	DEC_FUN_PTR(IEC_BYTE OS_DPTR *,ptr);

} SExtDec_PTR;


#endif	/* _LIBDEF_H_ */

/* ---------------------------------------------------------------------------- */
