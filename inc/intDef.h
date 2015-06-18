
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
 * Filename: intDef.h
 */


#ifndef _INTDEF_H_
#define _INTDEF_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif

/* Interpreter configuration
 * ----------------------------------------------------------------------------
 */
#define IP_CFG_IDX16		/* 16 bit index in object map			*/
#define IP_CFG_PTR32		/* 32 bit pointer size					*/
#define IP_CFG_AIS16		/* 16 bit max array size				*/
 

/* Special call level for performance/size optimazation
 * ----------------------------------------------------------------------------
 * Please not change ! the special levels
 */
#define IP_CFG_CLEVEL_CONV	   1
#define IP_CFG_CLEVEL_ARITH    2
#define IP_CFG_CLEVEL_WIDE16   3
#define IP_CFG_CLEVEL_WIDE32   3
#define IP_CFG_CLEVEL_WIDE64   3


/* Internal Prototyps 
 * ----------------------------------------------------------------------------
 */
typedef IEC_DATA OS_SPTR * SPTR_TYP;
typedef IEC_DATA OS_CPTR * CPTR_TYP;
typedef IEC_DATA OS_DPTR * DPTR_TYP;

IEC_UINT intConvert  (STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP);
IEC_UINT intArith	 (STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP);
IEC_UINT intWide16Ops(STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP);
IEC_UINT intWide32Ops(STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP);
IEC_UINT intWide64Ops(STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP);



/* Stack Overflow/Underflow Checking
 * ----------------------------------------------------------------------------
 */
#if ! defined(IP_CFG_NO_STACK_CHECK)

/* Stack underflow check: min n byte of parameter on stack 
 */
#define STACK_UCHK(n)  \
	if (pSP + (n) > pVM->Local.pStack + MAX_STACK_SIZE) \
	{ \
		return intSetException(pVM, EXCEPT_STACK_UNDERFLOW, pIP, pIN); \
	}

/* Stack overflow check: min n byte of space on stack 
 */
#define STACK_OCHK(n)  \
	if (pSP - (n) < pVM->Local.pStack) \
	{ \
		return intSetException(pVM, EXCEPT_STACK_OVERFLOW, pIP, pIN); \
	}

#else

#define STACK_UCHK(n)
#define STACK_OCHK(n)

#endif

#define STRING_CHECK(s)  /* ptr to string */

/* STK_UCHK1: check stack underflow for n one byte sized parameter 
 */
#if defined(IP_CFG_STACK8)
 #define STK_UCHK1(n)		 STACK_UCHK((n)*sizeof(IEC_BYTE))
#elif defined(IP_CFG_STACK16) 
 #define STK_UCHK1(n)		 STACK_UCHK((n)*sizeof(IEC_WORD))
#elif defined(IP_CFG_STACK32) 
 #define STK_UCHK1(n)		 STACK_UCHK((n)*sizeof(IEC_DWORD))
#elif defined(IP_CFG_STACK64) 
 #define STK_UCHK1(n)		 STACK_UCHK((n)*sizeof(IEC_LWORD))
#endif

/* STK_UCHK2: check stack underflow for n two byte sized parameter 
 */
#if defined(IP_CFG_STACK8)
 #define STK_UCHK2(n)		 STACK_UCHK((n)*sizeof(IEC_WORD))
#elif defined(IP_CFG_STACK16) 
 #define STK_UCHK2(n)		 STACK_UCHK((n)*sizeof(IEC_WORD))
#elif defined(IP_CFG_STACK32) 
 #define STK_UCHK2(n)		 STACK_UCHK((n)*sizeof(IEC_DWORD))
#elif defined(IP_CFG_STACK64) 
 #define STK_UCHK2(n)		 STACK_UCHK((n)*sizeof(IEC_LWORD))
#endif

/* STK_UCHK4: check stack underflow for n four byte sized parameter 
 */
#if defined(IP_CFG_STACK8)
 #define STK_UCHK4(n)		 STACK_UCHK((n)*sizeof(IEC_DWORD))
#elif defined(IP_CFG_STACK16) 
 #define STK_UCHK4(n)		 STACK_UCHK((n)*sizeof(IEC_DWORD))
#elif defined(IP_CFG_STACK32) 
 #define STK_UCHK4(n)		 STACK_UCHK((n)*sizeof(IEC_DWORD))
#elif defined(IP_CFG_STACK64) 
 #define STK_UCHK4(n)		 STACK_UCHK((n)*sizeof(IEC_LWORD))
#endif

/* STK_UCHK8: check stack underflow for n eight byte sized parameter 
 */
#if defined(IP_CFG_STACK8)
 #define STK_UCHK8(n)		 STACK_UCHK((n)*sizeof(IEC_LWORD))
#elif defined(IP_CFG_STACK16) 
 #define STK_UCHK8(n)		 STACK_UCHK((n)*sizeof(IEC_LWORD))
#elif defined(IP_CFG_STACK32) 
 #define STK_UCHK8(n)		 STACK_UCHK((n)*sizeof(IEC_LWORD))
#elif defined(IP_CFG_STACK64) 
 #define STK_UCHK8(n)		 STACK_UCHK((n)*sizeof(IEC_LWORD))
#endif


/* STK_OCHK1: check stack overflow for n one byte sized parameter 
 */
#if defined(IP_CFG_STACK8)
 #define STK_OCHK1(n)		 STACK_OCHK((n)*sizeof(IEC_BYTE))
#elif defined(IP_CFG_STACK16) 
 #define STK_OCHK1(n)		 STACK_OCHK((n)*sizeof(IEC_WORD))
#elif defined(IP_CFG_STACK32) 
 #define STK_OCHK1(n)		 STACK_OCHK((n)*sizeof(IEC_DWORD))
#elif defined(IP_CFG_STACK64) 
 #define STK_OCHK1(n)		 STACK_OCHK((n)*sizeof(IEC_LWORD))
#endif

/* STK_OCHK2: check stack overflow for n two byte sized parameter 
 */
#if defined(IP_CFG_STACK8)
 #define STK_OCHK2(n)		 STACK_OCHK((n)*sizeof(IEC_WORD))
#elif defined(IP_CFG_STACK16) 
 #define STK_OCHK2(n)		 STACK_OCHK((n)*sizeof(IEC_WORD))
#elif defined(IP_CFG_STACK32) 
 #define STK_OCHK2(n)		 STACK_OCHK((n)*sizeof(IEC_DWORD))
#elif defined(IP_CFG_STACK64) 
 #define STK_OCHK2(n)		 STACK_OCHK((n)*sizeof(IEC_LWORD))
#endif

/* STK_OCHK4: check stack overflow for n four byte sized parameter 
 */
#if defined(IP_CFG_STACK8)
 #define STK_OCHK4(n)		 STACK_OCHK((n)*sizeof(IEC_DWORD))
#elif defined(IP_CFG_STACK16) 
 #define STK_OCHK4(n)		 STACK_OCHK((n)*sizeof(IEC_DWORD))
#elif defined(IP_CFG_STACK32) 
 #define STK_OCHK4(n)		 STACK_OCHK((n)*sizeof(IEC_DWORD))
#elif defined(IP_CFG_STACK64) 
 #define STK_OCHK4(n)		 STACK_OCHK((n)*sizeof(IEC_LWORD))
#endif

/* STK_OCHK8: check stack overflow for n eight byte sized parameter 
 */
#if defined(IP_CFG_STACK8)
 #define STK_OCHK8(n)		 STACK_OCHK((n)*sizeof(IEC_LWORD))
#elif defined(IP_CFG_STACK16) 
 #define STK_OCHK8(n)		 STACK_OCHK((n)*sizeof(IEC_LWORD))
#elif defined(IP_CFG_STACK32) 
 #define STK_OCHK8(n)		 STACK_OCHK((n)*sizeof(IEC_LWORD))
#elif defined(IP_CFG_STACK64) 
 #define STK_OCHK8(n)		 STACK_OCHK((n)*sizeof(IEC_LWORD))
#endif

#define i_max(a,b)			((a)>(b)?(a):(b))

#if defined(IP_CFG_STACK8)
 #define CNV_NEW(x,y)		 ((x)-i_max((y),sizeof(IEC_BYTE)))
#elif defined(IP_CFG_STACK16) 
 #define CNV_NEW(x,y)		 ((x)-i_max((y),sizeof(IEC_WORD)))
#elif defined(IP_CFG_STACK32) 
 #define CNV_NEW(x,y)		 ((x)-i_max((y),sizeof(IEC_DWORD)))
#elif defined(IP_CFG_STACK64) 
 #define CNV_NEW(x,y)		 ((x)-i_max((y),sizeof(IEC_LWORD)))
#endif

#if 0

#define i_max(a,b)			(a>b?a:b)
#define i_min(a,b)			(a<b?a:b)

#if defined(IP_CFG_STACK8)
 #define CNV_NEW(x,y)		 ( x - i_min(x, i_max(y, sizeof(IEC_BYTE)) ) )
#elif defined(IP_CFG_STACK16) 
 #define CNV_NEW(x,y)		 ( x - i_min(x, i_max(y, sizeof(IEC_WORD)) ) )
#elif defined(IP_CFG_STACK32) 
 #define CNV_NEW(x,y)		 ( x - i_min(x, i_max(y, sizeof(IEC_DWORD)) ) )
#elif defined(IP_CFG_STACK64) 
 #define CNV_NEW(x,y)		 ( x - i_min(x, i_max(y, sizeof(IEC_LWORD)) ) )
#endif

#endif

/* Stack Pointer Increment/Decrement
 * ----------------------------------------------------------------------------
 */

/* SP_INC1: stack pointer increment 
 */
#if defined(IP_CFG_STACK8)
 #define SP_INC1			  pSP += 1
#elif defined(IP_CFG_STACK16)
 #define SP_INC1			  pSP += 2
#elif defined(IP_CFG_STACK32)
 #define SP_INC1			  pSP += 4
#elif defined(IP_CFG_STACK64)
 #define SP_INC1			  pSP += 8
#endif

/* SP_INC2: stack pointer increment 
 */
#if defined(IP_CFG_STACK8)
 #define SP_INC2			  pSP += 2
#elif defined(IP_CFG_STACK16)
 #define SP_INC2			  pSP += 2
#elif defined(IP_CFG_STACK32)
 #define SP_INC2			  pSP += 4
#elif defined(IP_CFG_STACK64)
 #define SP_INC2			  pSP += 8
#endif

/* SP_INC4: stack pointer increment 
 */
#if defined(IP_CFG_STACK8)
 #define SP_INC4			  pSP += 4
#elif defined(IP_CFG_STACK16)
 #define SP_INC4			  pSP += 4
#elif defined(IP_CFG_STACK32)
 #define SP_INC4			  pSP += 4
#elif defined(IP_CFG_STACK64)
 #define SP_INC4			  pSP += 8
#endif

/* SP_INC8: stack pointer increment 
 */
#if defined(IP_CFG_STACK8)
 #define SP_INC8			  pSP += 8
#elif defined(IP_CFG_STACK16)
 #define SP_INC8			  pSP += 8
#elif defined(IP_CFG_STACK32)
 #define SP_INC8			  pSP += 8
#elif defined(IP_CFG_STACK64)
 #define SP_INC8			  pSP += 8
#endif

/* SP_DEC1: stack pointer decrement 
 */
#if defined(IP_CFG_STACK8)
 #define SP_DEC1			  pSP -= 1
#elif defined(IP_CFG_STACK16)
 #define SP_DEC1			  pSP -= 2
#elif defined(IP_CFG_STACK32)
 #define SP_DEC1			  pSP -= 4
#elif defined(IP_CFG_STACK64)
 #define SP_DEC1			  pSP -= 8
#endif

/* SP_DEC2: stack pointer decrement 
 */
#if defined(IP_CFG_STACK8)
 #define SP_DEC2			  pSP -= 2
#elif defined(IP_CFG_STACK16)
 #define SP_DEC2			  pSP -= 2
#elif defined(IP_CFG_STACK32)
 #define SP_DEC2			  pSP -= 4
#elif defined(IP_CFG_STACK64)
 #define SP_DEC2			  pSP -= 8
#endif

/* SP_DEC4: stack pointer decrement 
 */
#if defined(IP_CFG_STACK8)
 #define SP_DEC4			  pSP -= 4
#elif defined(IP_CFG_STACK16)
 #define SP_DEC4			  pSP -= 4
#elif defined(IP_CFG_STACK32)
 #define SP_DEC4			  pSP -= 4
#elif defined(IP_CFG_STACK64)
 #define SP_DEC4			  pSP -= 8
#endif

/* SP_DEC8: stack pointer decrement 
 */
#if defined(IP_CFG_STACK8)
 #define SP_DEC8			  pSP -= 8
#elif defined(IP_CFG_STACK16)
 #define SP_DEC8			  pSP -= 8
#elif defined(IP_CFG_STACK32)
 #define SP_DEC8			  pSP -= 8
#elif defined(IP_CFG_STACK64)
 #define SP_DEC8			  pSP -= 8
#endif

/* Handle Different Pointer Sizes (IP_CFG_PTR)
 * ----------------------------------------------------------------------------
 */
#if defined (IP_CFG_PTR8)
	#define VPTR				IEC_BYTE
	#define SP_INC_VPTR 		SP_INC1
	#define SP_DEC_VPTR 		SP_DEC1
	#define STK_OCHK_VPTR(x)	STK_OCHK1(x)
	#define STK_UCHK_VPTR(x)	STK_UCHK1(x)
	#define VBITPTR 			IEC_WORD
	#define SP_INC_VBITPTR		SP_INC2
	#define SP_DEC_VBITPTR		SP_DEC2
	#define STK_OCHK_VBITPTR(x) STK_OCHK2(x)
	#define STK_UCHK_VBITPTR(x) STK_UCHK2(x)

#elif defined (IP_CFG_PTR16)
	#define VPTR				IEC_WORD
	#define SP_INC_VPTR 		SP_INC2
	#define SP_DEC_VPTR 		SP_DEC2
	#define STK_OCHK_VPTR(x)	STK_OCHK2(x)
	#define STK_UCHK_VPTR(x)	STK_UCHK2(x)
	#define VBITPTR 			IEC_WORD
	#define SP_INC_VBITPTR		SP_INC4
	#define SP_DEC_VBITPTR		SP_DEC4
	#define STK_OCHK_VBITPTR(x) STK_OCHK4(x)
	#define STK_UCHK_VBITPTR(x) STK_UCHK4(x)

#elif defined (IP_CFG_PTR32)
	#define VPTR				IEC_DWORD
	#define SP_INC_VPTR 		SP_INC4
	#define SP_DEC_VPTR 		SP_DEC4
	#define STK_OCHK_VPTR(x)	STK_OCHK4(x)
	#define STK_UCHK_VPTR(x)	STK_UCHK4(x)
	#define VBITPTR 			IEC_LWORD
	#define SP_INC_VBITPTR		SP_INC8
	#define SP_DEC_VBITPTR		SP_DEC8
	#define STK_OCHK_VBITPTR(x) STK_OCHK8(x)
	#define STK_UCHK_VBITPTR(x) STK_UCHK8(x)

#elif defined (IP_CFG_PTR64)
	#define VPTR				IEC_LWORD
	#define SP_INC_VPTR 		SP_INC8
	#define SP_DEC_VPTR 		SP_DEC8
	#define STK_OCHK_VPTR(x)	STK_OCHK8(x)
	#define STK_UCHK_VPTR(x)	STK_UCHK8(x)
	/* there is no representation for bit pointer on 64 address machines */

#endif


/* Handle Different Object Index Sizes (IP_CFG_IDX)
 * ----------------------------------------------------------------------------
 */
#if defined   (IP_CFG_IDX8)
	#define VIDX				IEC_BYTE
#elif defined (IP_CFG_IDX16)
	#define VIDX				IEC_WORD
#elif defined (IP_CFG_IDX32)
	#define VIDX				IEC_DWORD
#elif defined (IP_CFG_IDX64)
	#define VIDX				IEC_LWORD
#endif


/* Handle Different Array Index Sizes (IP_CFG_AIS)
 * ----------------------------------------------------------------------------
 */
#if defined   (IP_CFG_AIS8)
	#define VAIS				IEC_BYTE
#elif defined (IP_CFG_AIS16)
	#define VAIS				IEC_WORD
#elif defined (IP_CFG_AIS32)
	#define VAIS				IEC_DWORD
#elif defined (IP_CFG_AIS64)
	#define VAIS				IEC_LWORD
#endif


/* Handle different instance data aligment for array index sizes
 * ----------------------------------------------------------------------------
 */

#if defined(IP_CFG_AIS8)	/* ------------------------------------------------ */

  #if defined(IP_CFG_INST8)
	#define VAIO1				sizeof(IEC_BYTE)
	#define VAIO8				sizeof(IEC_BYTE)
	#define VAIO16				sizeof(IEC_BYTE)
	#define VAIO32				sizeof(IEC_BYTE)
	#define VAIO64				sizeof(IEC_BYTE)
	#define VAIO(s) 			sizeof(IEC_BYTE)
  #endif

  #if defined(IP_CFG_INST16)
	#define VAIO1				sizeof(IEC_BYTE)
	#define VAIO8				sizeof(IEC_BYTE)
	#define VAIO16				sizeof(IEC_WORD)
	#define VAIO32				sizeof(IEC_WORD)
	#define VAIO64				sizeof(IEC_WORD)
	#define VAIO(s) 			( (s) <= (sizeof(IEC_BYTE)*8)  ? sizeof(IEC_BYTE)  : sizeof(IEC_WORD) )
  #endif

  #if defined(IP_CFG_INST32)
	#define VAIO1				sizeof(IEC_BYTE)
	#define VAIO8				sizeof(IEC_BYTE)
	#define VAIO16				sizeof(IEC_WORD)
	#define VAIO32				sizeof(IEC_DWORD)
	#define VAIO64				sizeof(IEC_DWORD)
	#define VAIO(s) 			( (s) <= (sizeof(IEC_BYTE)*8)  ? sizeof(IEC_BYTE)  : \
								( (s) == (sizeof(IEC_WORD)*8)  ? sizeof(IEC_WORD)  : sizeof(IEC_DWORD) ) )
  #endif

  #if defined(IP_CFG_INST64)
	#define VAIO1				sizeof(IEC_BYTE)
	#define VAIO8				sizeof(IEC_BYTE)
	#define VAIO16				sizeof(IEC_WORD)
	#define VAIO32				sizeof(IEC_DWORD)
	#define VAIO64				sizeof(IEC_LWORD)
	#define VAIO(s) 			( (s) <= (sizeof(IEC_BYTE)*8)  ? sizeof(IEC_BYTE)  : \
								( (s) == (sizeof(IEC_WORD)*8)  ? sizeof(IEC_WORD)  : \
								( (s) == (sizeof(IEC_DWORD)*8) ? sizeof(IEC_DWORD) : sizeof(IEC_LWORD) ) ) )
  #endif

#endif	/* IP_CFG_AIS8 */

#if defined(IP_CFG_AIS16)	/* ------------------------------------------------ */

  #if defined(IP_CFG_INST8)
	#define VAIO1				sizeof(IEC_WORD)
	#define VAIO8				sizeof(IEC_WORD)
	#define VAIO16				sizeof(IEC_WORD)
	#define VAIO32				sizeof(IEC_WORD)
	#define VAIO64				sizeof(IEC_WORD)
	#define VAIO(s) 			sizeof(IEC_WORD)
  #endif

  #if defined(IP_CFG_INST16)
	#define VAIO1				sizeof(IEC_WORD)
	#define VAIO8				sizeof(IEC_WORD)
	#define VAIO16				sizeof(IEC_WORD)
	#define VAIO32				sizeof(IEC_WORD)
	#define VAIO64				sizeof(IEC_WORD)
	#define VAIO(s) 			sizeof(IEC_WORD)
  #endif

  #if defined(IP_CFG_INST32)
	#define VAIO1				sizeof(IEC_WORD)
	#define VAIO8				sizeof(IEC_WORD)
	#define VAIO16				sizeof(IEC_WORD)
	#define VAIO32				sizeof(IEC_DWORD)
	#define VAIO64				sizeof(IEC_DWORD)
	#define VAIO(s) 			( (s) <= (sizeof(IEC_WORD)*8)  ? sizeof(IEC_WORD)  : sizeof(IEC_DWORD) )
  #endif

  #if defined(IP_CFG_INST64)
	#define VAIO1				sizeof(IEC_WORD)
	#define VAIO8				sizeof(IEC_WORD)
	#define VAIO16				sizeof(IEC_WORD)
	#define VAIO32				sizeof(IEC_DWORD)
	#define VAIO64				sizeof(IEC_LWORD)
	#define VAIO(s) 			( (s) <= (sizeof(IEC_WORD)*8)  ? sizeof(IEC_WORD)  : \
								( (s) == (sizeof(IEC_DWORD)*8) ? sizeof(IEC_DWORD) : sizeof(IEC_LWORD) ) )
  #endif

#endif	/* IP_CFG_AIS16 */

#if defined(IP_CFG_AIS32)	/* ------------------------------------------------ */

  #if defined(IP_CFG_INST8)
	#define VAIO1				sizeof(IEC_DWORD)
	#define VAIO8				sizeof(IEC_DWORD)
	#define VAIO16				sizeof(IEC_DWORD)
	#define VAIO32				sizeof(IEC_DWORD)
	#define VAIO64				sizeof(IEC_DWORD)
	#define VAIO(s) 			sizeof(IEC_DWORD)
  #endif

  #if defined(IP_CFG_INST16)
	#define VAIO1				sizeof(IEC_DWORD)
	#define VAIO8				sizeof(IEC_DWORD)
	#define VAIO16				sizeof(IEC_DWORD)
	#define VAIO32				sizeof(IEC_DWORD)
	#define VAIO64				sizeof(IEC_DWORD)
	#define VAIO(s) 			sizeof(IEC_DWORD)
  #endif

  #if defined(IP_CFG_INST32)
	#define VAIO1				sizeof(IEC_DWORD)
	#define VAIO8				sizeof(IEC_DWORD)
	#define VAIO16				sizeof(IEC_DWORD)
	#define VAIO32				sizeof(IEC_DWORD)
	#define VAIO64				sizeof(IEC_DWORD)
	#define VAIO(s) 			sizeof(IEC_DWORD)
#endif

  #if defined(IP_CFG_INST64)
	#define VAIO1				sizeof(IEC_DWORD)
	#define VAIO8				sizeof(IEC_DWORD)
	#define VAIO16				sizeof(IEC_DWORD)
	#define VAIO32				sizeof(IEC_DWORD)
	#define VAIO64				sizeof(IEC_LWORD)
	#define VAIO(s) 			( (s) <= (sizeof(IEC_DWORD)*8) ? sizeof(IEC_DWORD) : sizeof(IEC_LWORD) )
  #endif

#endif	/* IP_CFG_AIS32 */

#if defined(IP_CFG_AIS64)	/* ------------------------------------------------ */

  #if defined(IP_CFG_INST8)
	#define VAIO1				sizeof(IEC_LWORD)
	#define VAIO8				sizeof(IEC_LWORD)
	#define VAIO16				sizeof(IEC_LWORD)
	#define VAIO32				sizeof(IEC_LWORD)
	#define VAIO64				sizeof(IEC_LWORD)
	#define VAIO(s) 			sizeof(IEC_LWORD)
  #endif

  #if defined(IP_CFG_INST16)
	#define VAIO1				sizeof(IEC_LWORD)
	#define VAIO8				sizeof(IEC_LWORD)
	#define VAIO16				sizeof(IEC_LWORD)
	#define VAIO32				sizeof(IEC_LWORD)
	#define VAIO64				sizeof(IEC_LWORD)
	#define VAIO(s) 			sizeof(IEC_LWORD)
  #endif

  #if defined(IP_CFG_INST32)
	#define VAIO1				sizeof(IEC_LWORD)
	#define VAIO8				sizeof(IEC_LWORD)
	#define VAIO16				sizeof(IEC_LWORD)
	#define VAIO32				sizeof(IEC_DWORD)
	#define VAIO64				sizeof(IEC_LWORD)
	#define VAIO(s) 			sizeof(IEC_LWORD)
  #endif

  #if defined(IP_CFG_INST64)
	#define VAIO1				sizeof(IEC_LWORD)
	#define VAIO8				sizeof(IEC_LWORD)
	#define VAIO16				sizeof(IEC_LWORD)
	#define VAIO32				sizeof(IEC_LWORD)
	#define VAIO64				sizeof(IEC_LWORD)
	#define VAIO(s) 			sizeof(IEC_LWORD)
  #endif

#endif	/* IP_CFG_AIS64 */



/* Handle Different Parameter Alignment on Stack (IP_CFG_STACK)
 * ----------------------------------------------------------------------------
 */
#if defined (IP_CFG_STACK8)

	#define REGI_BYTE			sizeof(IEC_BYTE)
	#define REGI_WORD			sizeof(IEC_WORD)
	#define REGI_DWORD			sizeof(IEC_DWORD)
	#define REGI_LWORD			sizeof(IEC_LWORD)
	
 #if   defined (IP_CFG_PTR8)
	#define REG_VPTR			sizeof(IEC_BYTE)
 #elif defined (IP_CFG_PTR16)
	#define REG_VPTR			sizeof(IEC_WORD)
 #elif defined (IP_CFG_PTR32)
	#define REG_VPTR			sizeof(IEC_DWORD)
 #elif defined (IP_CFG_PTR64)
	#define REG_VPTR			sizeof(IEC_LWORD)
 #endif

#elif defined (IP_CFG_STACK16)

	#define REGI_BYTE			sizeof(IEC_WORD)
	#define REGI_WORD			sizeof(IEC_WORD)
	#define REGI_DWORD			sizeof(IEC_DWORD)
	#define REGI_LWORD			sizeof(IEC_LWORD)

 #if   defined (IP_CFG_PTR8)
	#define REG_VPTR			sizeof(IEC_WORD)
 #elif defined (IP_CFG_PTR16)
	#define REG_VPTR			sizeof(IEC_WORD)
 #elif defined (IP_CFG_PTR32)
	#define REG_VPTR			sizeof(IEC_DWORD)
 #elif defined (IP_CFG_PTR64)
	#define REG_VPTR			sizeof(IEC_LWORD)
 #endif

#elif defined (IP_CFG_STACK32)

	#define REGI_BYTE			sizeof(IEC_DWORD)
	#define REGI_WORD			sizeof(IEC_DWORD)
	#define REGI_DWORD			sizeof(IEC_DWORD)
	#define REGI_LWORD			sizeof(IEC_LWORD)

 #if   defined (IP_CFG_PTR8)
	#define REG_VPTR			sizeof(IEC_DWORD)
 #elif defined (IP_CFG_PTR16)
	#define REG_VPTR			sizeof(IEC_DWORD)
 #elif defined (IP_CFG_PTR32)
	#define REG_VPTR			sizeof(IEC_DWORD)
 #elif defined (IP_CFG_PTR64)
	#define REG_VPTR			sizeof(IEC_LWORD)
 #endif

#elif defined (IP_CFG_STACK64)

	#define REGI_BYTE			sizeof(IEC_LWORD)
	#define REGI_WORD			sizeof(IEC_LWORD)
	#define REGI_DWORD			sizeof(IEC_LWORD)
	#define REGI_LWORD			sizeof(IEC_LWORD)

 #if   defined (IP_CFG_PTR8)
	#define REG_VPTR			sizeof(IEC_LWORD)
 #elif defined (IP_CFG_PTR16)
	#define REG_VPTR			sizeof(IEC_LWORD)
 #elif defined (IP_CFG_PTR32)
	#define REG_VPTR			sizeof(IEC_LWORD)
 #elif defined (IP_CFG_PTR64)
	#define REG_VPTR			sizeof(IEC_LWORD)
 #endif

#endif


/* Push/Pop Data (Global Memory)
 * ----------------------------------------------------------------------------
 */
#define PUSH1(src,bit)														\
		STK_OCHK1(1);														\
		SP_DEC1;															\
		*(IEC_BYTE OS_SPTR *)(pSP) = (IEC_BYTE)((*(IEC_BYTE OS_DPTR *)(src) & (1 << ((bit) & 7))) ? 1 : 0); 

#define POP1(des,bit)														\
		STK_UCHK1(1);														\
		OS_LOCK_BIT;														\
		*(IEC_BYTE OS_DPTR *)(des) = (IEC_BYTE)((*(IEC_BYTE OS_DPTR *)(des) & ~(1 << ((bit) & 7))) | ((*(IEC_BYTE OS_SPTR *)(pSP) & 1) << ((bit) & 7))); \
		OS_FREE_BIT;														\
		SP_INC1;

#define PUSH8(src)															\
		STK_OCHK1(1);														\
		SP_DEC1;															\
		OS_LOCK8;															\
		OS_MOVE8((IEC_BYTE OS_SPTR *)(pSP), (IEC_BYTE OS_DPTR *)(src)); 	\
		OS_FREE8;
		
#define POP8(des)															\
		STK_UCHK1(1);														\
		OS_LOCK8;															\
		OS_MOVE8((IEC_BYTE OS_DPTR *)(des), (IEC_BYTE OS_SPTR *)(pSP)); 	\
		OS_FREE8;															\
		SP_INC1;

#define PUSH16(src) 														\
		STK_OCHK2(1);														\
		SP_DEC2;															\
		OS_LOCK16;															\
		OS_MOVE16((IEC_WORD OS_SPTR *)(pSP), (IEC_WORD OS_DPTR *)(src));	\
		OS_FREE16;
		
#define POP16(des)															\
		STK_UCHK2(1);														\
		OS_LOCK16;															\
		OS_MOVE16((IEC_WORD OS_DPTR *)(des), (IEC_WORD OS_SPTR *)(pSP));	\
		OS_FREE16;															\
		SP_INC2;

#define PUSH32(src) 														\
		STK_OCHK4(1);														\
		SP_DEC4;															\
		OS_LOCK32;															\
		OS_MOVE32((IEC_DWORD OS_SPTR *)(pSP), (IEC_DWORD OS_DPTR *)(src));	\
		OS_FREE32;
		
#define POP32(des)															\
		STK_UCHK4(1);														\
		OS_LOCK32;															\
		OS_MOVE32((IEC_DWORD OS_DPTR *)(des), (IEC_DWORD OS_SPTR *)(pSP));	\
		OS_FREE32;															\
		SP_INC4;

#define PUSH64(src) 														\
		STK_OCHK8(1);														\
		SP_DEC8;															\
		OS_LOCK64;															\
		OS_MOVE64((IEC_LWORD OS_SPTR *)(pSP), (IEC_LWORD OS_DPTR *)(src));	\
		OS_FREE64;
		
#define POP64(des)															\
		STK_UCHK8(1);														\
		OS_LOCK64;															\
		OS_MOVE64((IEC_LWORD OS_DPTR *)(des), (IEC_LWORD OS_SPTR *)(pSP));	\
		OS_FREE64;															\
		SP_INC8;


/* Write Write Flags
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_WRITE_FLAGS)

#define WFLG1(wfl,bit)														\
		*(IEC_BYTE OS_DPTR *)(wfl) = (IEC_BYTE)( *(IEC_BYTE OS_DPTR *)(wfl) | (1 << ((bit) & 7)) ); \

#define WFLG8(wfl)															\
		*(IEC_BYTE OS_DPTR *)(wfl) = (IEC_BYTE)0xffu;

#define WFLG16(wfl) 														\
		*(IEC_WORD OS_DPTR *)(wfl) = (IEC_WORD)0xffffu;

#define WFLG32(wfl) 														\
		*(IEC_DWORD OS_DPTR *)(wfl) = (IEC_DWORD)0xfffffffflu;

#define WFLG64(wfl) 														\
		*((IEC_DWORD OS_DPTR *)(wfl) + 0)	= (IEC_DWORD)0xfffffffflu;		\
		*((IEC_DWORD OS_DPTR *)(wfl) + 1)	= (IEC_DWORD)0xfffffffflu;

#define WFLGnn(wfl,nn)														\
		OS_MEMSET((IEC_BYTE OS_DPTR *)(wfl), 0xff, nn);

#endif /* RTS_CFG_WRITE_FLAGS */


/* Pop/Push to local variables
 * ----------------------------------------------------------------------------
 */ 	
#define PUSHPL(src) 														\
		STK_OCHK_VPTR(1);													\
		SP_DEC_VPTR;														\
		*(VPTR OS_SPTR *)pSP = (VPTR)(src);

#define POPPL(des)															\
		STK_UCHK_VPTR(1);													\
		(des) = (IEC_BYTE OS_DPTR *)*(VPTR OS_SPTR *)pSP; 					\
		SP_INC_VPTR;

#define PUSHBITPL(src,bit)													\
		STK_OCHK_VBITPTR(1);												\
		SP_DEC_VBITPTR; 													\
		*(VPTR OS_SPTR *)pSP = (VPTR)(src); 								\
		*(((IEC_BYTE*)pSP) + sizeof(VPTR)) = (IEC_BYTE)((bit)&0x07u);

#define POPBITPL(des,bit)													\
		STK_UCHK_VBITPTR(1);												\
		(des) = (IEC_BYTE OS_DPTR *)*(VBITPTR OS_SPTR *)pSP;				\
		(bit) = *(((IEC_BYTE*)pSP)+sizeof(VPTR));							\
		SP_INC_VBITPTR;

#define PUSH8L(des) 														\
		STK_OCHK1(1);														\
		SP_DEC1;															\
		*(IEC_BYTE OS_SPTR *)pSP = (des);

#define POP8L(des)															\
		STK_UCHK1(1);														\
		(des) = *(IEC_BYTE OS_SPTR *)pSP;									\
		SP_INC1;

#define PUSH16L(des)														\
		STK_OCHK2(1);														\
		SP_DEC2;															\
		*(IEC_WORD OS_SPTR *)pSP = (des);

#define POP16L(des) 														\
		STK_UCHK2(1);														\
		(des) = *(IEC_WORD OS_SPTR *)pSP;									\
		SP_INC2;

#define PUSH32L(des)														\
		STK_OCHK4(1);														\
		SP_DEC4;															\
		*(IEC_DWORD OS_SPTR *)pSP = (des);

#define POP32L(des) 														\
		STK_UCHK4(1);														\
		(des) = *(IEC_DWORD OS_SPTR *)pSP;									\
		SP_INC4;

#define PUSH64L(des)														\
		STK_OCHK8(1);														\
		SP_DEC8;															\
		((IEC_LWORD OS_SPTR *)pSP)->L = (des).L;							\
		((IEC_LWORD OS_SPTR *)pSP)->H = (des).H;

#define POP64L(des) 														\
		STK_UCHK8(1);														\
		(des).L = ((IEC_LWORD OS_SPTR *)pSP)->L;							\
		(des).H = ((IEC_LWORD OS_SPTR *)pSP)->H;							\
		SP_INC8;


/* Check Array Range
 * ----------------------------------------------------------------------------
 */
#if ! defined(IP_CFG_NO_ARRAY_CHECK)
	#define CHECK_ARRAY(arr,ind)												\
			if (*(VAIS OS_DPTR *)(arr) <= (ind))								\
			{																	\
				return intSetException(pVM, EXCEPT_ARRAY_RANGE, pIP, pIN);		\
			}
#else
	#define CHECK_ARRAY(arr,ind)
#endif

#endif /* _INTDEF_H_ */

/* ---------------------------------------------------------------------------- */
