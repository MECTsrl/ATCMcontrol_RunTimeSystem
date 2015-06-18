
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
 * Filename: libUtil.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"libUtil.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "libUtil.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

#if defined(RTS_CFG_UTILITY_LIB)

extern EXECUTE_FUN	g_pLibraryFun[];
extern EXECUTE_FB	g_pLibraryFB [];

/* ----  Local Functions:	--------------------------------------------------- */

static void copy_to_byte_array(STaskInfoVM *pVM, IEC_DINT OS_DPTR *pPos,
				IEC_BYTE OS_DPTR *pDat, IEC_BYTE OS_SPTR *inVal,  IEC_UINT uLen, IEC_UINT uCopy);
static void copy_from_byte_array(STaskInfoVM *pVM, IEC_DINT OS_DPTR *pPos,
				IEC_BYTE OS_DPTR *pDat, IEC_BYTE OS_SPTR *retVal, IEC_UINT uLen, IEC_UINT uCopy);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * copy_dword_from_byte_array 
 *
 */
#if defined(IP_CFG_DWORD) && defined(IP_CFG_BYTE)
void copy_dword_from_byte_array(STDLIBFUNCALL) 
{
	COPY_DWORD_FROM_BA_PAR OS_SPTR *pPar = (COPY_DWORD_FROM_BA_PAR OS_SPTR *)pIN;
	copy_from_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray), 
			(IEC_BYTE OS_SPTR *)&pPar->retVal, 
			GET_AR_LEN(pPar->pbArray),
			sizeof(IEC_DWORD));
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_dword_to_byte_array
 *
 */
void copy_dword_to_byte_array(STDLIBFUNCALL) 
{
	COPY_DWORD_TO_BA_PAR OS_SPTR *pPar = (COPY_DWORD_TO_BA_PAR OS_SPTR *)pIN;
	copy_to_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray),
			(IEC_BYTE OS_SPTR *)&pPar->inVal, 
			GET_AR_LEN(pPar->pbArray), sizeof(IEC_DWORD));
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * copy_lword_from_byte_array
 *
 */
#if defined(IP_CFG_BYTE) && defined(IP_CFG_LWORD)
void copy_lword_from_byte_array(STDLIBFUNCALL) 
{
	COPY_LWORD_FROM_BA_PAR OS_SPTR *pPar = (COPY_LWORD_FROM_BA_PAR OS_SPTR *)pIN;
	copy_from_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray), 
			(IEC_BYTE OS_SPTR *)&pPar->retVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_LWORD));
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_lword_to_byte_array
 *
 */
void copy_lword_to_byte_array(STDLIBFUNCALL) 
{
	COPY_LWORD_TO_BA_PAR OS_SPTR *pPar = (COPY_LWORD_TO_BA_PAR OS_SPTR *)pIN;
	copy_to_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray),
			(IEC_BYTE OS_SPTR *)&pPar->inVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_LWORD));	
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * copy_int_to_byte_array
 *
 */
#if defined(IP_CFG_BYTE) && defined(IP_CFG_INT)
void copy_int_to_byte_array(STDLIBFUNCALL) 
{
	COPY_INT_TO_BA_PAR OS_SPTR *pPar = (COPY_INT_TO_BA_PAR OS_SPTR *)pIN;
	copy_to_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray),
			(IEC_BYTE OS_SPTR *)&pPar->inVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_INT));	  
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_int_from_byte_array
 *
 */
void copy_int_from_byte_array(STDLIBFUNCALL) 
{
	COPY_INT_FROM_BA_PAR OS_SPTR *pPar = (COPY_INT_FROM_BA_PAR OS_SPTR *)pIN;
	copy_from_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray), 
			(IEC_BYTE OS_SPTR *)&pPar->retVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_INT));
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * copy_dint_to_byte_array
 *
 */
#if defined(IP_CFG_BYTE) && defined(IP_CFG_DINT)
void copy_dint_to_byte_array(STDLIBFUNCALL) 
{
	COPY_DINT_TO_BA_PAR OS_SPTR *pPar = (COPY_DINT_TO_BA_PAR OS_SPTR *)pIN;
	copy_to_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray),
			(IEC_BYTE OS_SPTR *)&pPar->inVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_DINT));    
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_dint_from_byte_array
 *
 */
void copy_dint_from_byte_array(STDLIBFUNCALL) 
{
	COPY_DINT_FROM_BA_PAR OS_SPTR *pPar = (COPY_DINT_FROM_BA_PAR OS_SPTR *)pIN;
	copy_from_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray), 
			(IEC_BYTE OS_SPTR *)&pPar->retVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_DINT));
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * copy_lint_to_byte_array
 *
 */
#if defined(IP_CFG_LINT) && defined(IP_CFG_BYTE)
void copy_lint_to_byte_array(STDLIBFUNCALL) 
{
	COPY_LINT_TO_BA_PAR OS_SPTR *pPar = (COPY_LINT_TO_BA_PAR OS_SPTR *)pIN;
	copy_to_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray),
			(IEC_BYTE OS_SPTR *)&pPar->inVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_LINT));    
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_lint_from_byte_array
 *
 */
void copy_lint_from_byte_array(STDLIBFUNCALL) 
{
	COPY_LINT_FROM_BA_PAR OS_SPTR *pPar = (COPY_LINT_FROM_BA_PAR OS_SPTR *)pIN;
	copy_from_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray), 
			(IEC_BYTE OS_SPTR *)&pPar->retVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_LINT));
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * copy_lreal_to_byte_array
 *
 */
#if defined(IP_CFG_LREAL) && defined(IP_CFG_BYTE)
void copy_lreal_to_byte_array(STDLIBFUNCALL) 
{
	COPY_LREAL_TO_BA_PAR OS_SPTR *pPar = (COPY_LREAL_TO_BA_PAR OS_SPTR *)pIN;
	copy_to_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray),
			(IEC_BYTE OS_SPTR *)&pPar->inVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_LREAL));	
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_lreal_from_byte_array
 *
 */
void copy_lreal_from_byte_array(STDLIBFUNCALL) 
{
	COPY_LREAL_FROM_BA_PAR OS_SPTR *pPar = (COPY_LREAL_FROM_BA_PAR OS_SPTR *)pIN;
	copy_from_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray), 
			(IEC_BYTE OS_SPTR *)&pPar->retVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_LREAL));
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * copy_real_to_byte_array
 *
 */
#if defined(IP_CFG_REAL) && defined(IP_CFG_BYTE)
void copy_real_to_byte_array(STDLIBFUNCALL) 
{
	COPY_REAL_TO_BA_PAR OS_SPTR *pPar = (COPY_REAL_TO_BA_PAR OS_SPTR *)pIN;
	copy_to_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray),
			(IEC_BYTE OS_SPTR *)&pPar->inVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_REAL));    
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_real_from_byte_array
 *
 */
void copy_real_from_byte_array(STDLIBFUNCALL) 
{
	COPY_REAL_FROM_BA_PAR OS_SPTR *pPar = (COPY_REAL_FROM_BA_PAR OS_SPTR *)pIN;
	copy_from_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray), 
			(IEC_BYTE OS_SPTR *)&pPar->retVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_REAL));
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * copy_bool_to_byte_array
 *
 */
#if defined(IP_CFG_BOOL) && defined(IP_CFG_BYTE)
void copy_bool_to_byte_array(STDLIBFUNCALL) 
{
	COPY_BOOL_TO_BA_PAR OS_SPTR *pPar = (COPY_BOOL_TO_BA_PAR OS_SPTR *)pIN;
	copy_to_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray),
			(IEC_BYTE OS_SPTR *)&pPar->inVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_BOOL));    
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_bool_from_byte_array
 *
 */
void copy_bool_from_byte_array(STDLIBFUNCALL) 
{
	COPY_BOOL_FROM_BA_PAR OS_SPTR *pPar = (COPY_BOOL_FROM_BA_PAR OS_SPTR *)pIN;
	copy_from_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray), 
			(IEC_BYTE OS_SPTR *)&pPar->retVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_BOOL));
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * copy_time_from_byte_array
 *
 */

#if defined(IP_CFG_TIME) && defined(IP_CFG_BYTE)
void copy_time_from_byte_array(STDLIBFUNCALL) 
{
	COPY_TIME_FROM_BA_PAR OS_SPTR *pPar = (COPY_TIME_FROM_BA_PAR OS_SPTR *)pIN;
								 
	IEC_BYTE	OS_DPTR *pDat	= GET_AR_DAT(pPar->pbArray);
	IEC_DINT			 pos	= *pPar->pPos;
	IEC_BYTE	OS_SPTR *retVal = (IEC_BYTE OS_SPTR *)&pPar->retVal;

	if ((IEC_UDINT)(pos + 2 * sizeof(IEC_TIME)) > GET_AR_LEN(pPar->pbArray))
	{
		libSetException(pVM, EXCEPT_ARRAY_RANGE);
		*pPar->pPos = -1;
		return;
	}

#ifdef RTS_CFG_BIGENDIAN
	retVal[0] = pDat[pos++];
	retVal[1] = pDat[pos++];
	retVal[2] = pDat[pos++];
	retVal[3] = pDat[pos++];
	
	/* check Time64 --> Time32 overrun and skip 4 bytes */
	if(pDat[pos + 3] & 0x80) {
		/* sign: negativ */
		if((pDat[pos++] & pDat[pos++] & pDat[pos++] & pDat[pos++]) != 0xff) {
			libSetError(pVM, EXCEPT_INVALID_ARG);
		}
	} else {
		/* sign: positiv */
		if((pDat[pos++] | pDat[pos++] | pDat[pos++] | pDat[pos++]) != 0x00) {
			libSetError(pVM, EXCEPT_INVALID_ARG);
		}
	}
#else
	/* check Time64 --> Time32 overrun and skip 4 bytes */
	if(pDat[pos] & 0x80) {
		/* sign: negativ */
		if((pDat[pos+0] & pDat[pos+1] & pDat[pos+2] & pDat[pos+3]) != 0xff) {
			libSetError(pVM, EXCEPT_INVALID_ARG);
		}
		pos += 4;
	} else {
		/* sign: positiv */
		if((pDat[pos+0] | pDat[pos+1] | pDat[pos+2] | pDat[pos+3]) != 0x00) {
			libSetError(pVM, EXCEPT_INVALID_ARG);
		}
		pos += 4;
	}

	retVal[3] = pDat[pos++];
	retVal[2] = pDat[pos++];
	retVal[1] = pDat[pos++];
	retVal[0] = pDat[pos++];
#endif 

	*pPar->pPos = pos;	  
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_time_to_byte_array
 *
 */
void copy_time_to_byte_array(STDLIBFUNCALL) 
{
	COPY_TIME_TO_BA_PAR OS_SPTR *pPar = (COPY_TIME_TO_BA_PAR OS_SPTR *)pIN;
	
	IEC_BYTE	OS_DPTR *pDat	= GET_AR_DAT(pPar->pbArray);
	IEC_DINT			 pos	= *pPar->pPos;
	IEC_BYTE	OS_SPTR *inVal	= (IEC_BYTE OS_SPTR *)&pPar->inVal;
	
	if ((IEC_UDINT)(pos + 2 * sizeof(IEC_TIME)) > GET_AR_LEN(pPar->pbArray))
	{
		libSetException(pVM, EXCEPT_ARRAY_RANGE);
		*pPar->pPos = -1;
		return;
	}
	
	if(pPar->inVal >= 0)
	{/* 00 00 00 00 first 4 bytes */
#ifdef RTS_CFG_BIGENDIAN
		pDat[pos++] = inVal[0];
		pDat[pos++] = inVal[1];
		pDat[pos++] = inVal[2];
		pDat[pos++] = inVal[3];
		pDat[pos++] = 0;	/*inVal[4];*/
		pDat[pos++] = 0;	/*inVal[5];*/
		pDat[pos++] = 0;	/*inVal[6];*/
		pDat[pos++] = 0;	/*inVal[7];*/
#else
		pDat[pos++] = 0;	/*inVal[7];*/
		pDat[pos++] = 0;	/*inVal[6];*/
		pDat[pos++] = 0;	/*inVal[5];*/
		pDat[pos++] = 0;	/*inVal[4];*/
		pDat[pos++] = inVal[3];
		pDat[pos++] = inVal[2];
		pDat[pos++] = inVal[1];
		pDat[pos++] = inVal[0];
#endif
	}
	else
	{	/* FF FF FF FF first 4 bytes*/
#ifdef RTS_CFG_BIGENDIAN
		pDat[pos++] = inVal[0];
		pDat[pos++] = inVal[1];
		pDat[pos++] = inVal[2];
		pDat[pos++] = inVal[3];
		pDat[pos++] = 255;	/*inVal[4];*/
		pDat[pos++] = 255;	/*inVal[5];*/
		pDat[pos++] = 255;	/*inVal[6];*/
		pDat[pos++] = 255;	/*inVal[7];*/
#else
		pDat[pos++] = 255;	/*inVal[7];*/
		pDat[pos++] = 255;	/*inVal[6];*/
		pDat[pos++] = 255;	/*inVal[5];*/
		pDat[pos++] = 255;	/*inVal[4];*/
		pDat[pos++] = inVal[3];
		pDat[pos++] = inVal[2];
		pDat[pos++] = inVal[1];
		pDat[pos++] = inVal[0];
#endif	 
	}
	
	*pPar->pPos = pos;
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * copy_byte_from_byte_array
 *
 */
#if defined(IP_CFG_BYTE)
void copy_byte_from_byte_array(STDLIBFUNCALL) 
{
	COPY_BYTE_FROM_BA_PAR OS_SPTR *pPar = (COPY_BYTE_FROM_BA_PAR OS_SPTR *)pIN;
	copy_from_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray), 
			(IEC_BYTE OS_SPTR *)&pPar->retVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_BYTE));
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_byte_to_byte_array
 *
 */
void copy_byte_to_byte_array(STDLIBFUNCALL) 
{
	COPY_BYTE_TO_BA_PAR OS_SPTR *pPar = (COPY_BYTE_TO_BA_PAR OS_SPTR *)pIN;

	copy_to_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray),
			(IEC_BYTE OS_SPTR *)&pPar->inVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_BYTE));    
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * copy_word_from_byte_array
 *
 */
#if defined(IP_CFG_WORD) && defined(IP_CFG_BYTE)
void copy_word_from_byte_array(STDLIBFUNCALL) 
{
	COPY_WORD_FROM_BA_PAR OS_SPTR *pPar = (COPY_WORD_FROM_BA_PAR OS_SPTR *)pIN;
	copy_from_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray), 
			(IEC_BYTE OS_SPTR *)&pPar->retVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_WORD));
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_word_to_byte_array
 *
 */
void copy_word_to_byte_array(STDLIBFUNCALL) 
{
	COPY_WORD_TO_BA_PAR OS_SPTR *pPar = (COPY_WORD_TO_BA_PAR OS_SPTR *)pIN;
	copy_to_byte_array(pVM, pPar->pPos, GET_AR_DAT(pPar->pbArray),
			(IEC_BYTE OS_SPTR *)&pPar->inVal, GET_AR_LEN(pPar->pbArray), sizeof(IEC_WORD));    
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * copy_string_from_byte_array
 *
 */
#if defined(IP_CFG_STRING) && defined(IP_CFG_BYTE)
void copy_string_from_byte_array(STDLIBFUNCALL) 
{
	IEC_UINT			i;
	IEC_BYTE OS_DPTR	*pDat;
	IEC_BYTE OS_DPTR	*retVal;

	COPY_STRING_FROM_BA_PAR OS_SPTR *pPar = (COPY_STRING_FROM_BA_PAR OS_SPTR *)pIN;

	if ((IEC_UINT)(*(pPar->pPos) + (IEC_UINT)pPar->retVal->MaxLen) > GET_AR_LEN(pPar->pbArray))
	{
		libSetException(pVM, EXCEPT_ARRAY_RANGE);
		*(pPar->pPos) = (IEC_UDINT)-1;
		return;
	}

	pDat = GET_AR_DAT(pPar->pbArray);
	retVal = (IEC_BYTE OS_DPTR *)pPar->retVal->Contents;

	for (i = 0; i < pPar->retVal->MaxLen; i++)
	{
		retVal[i] = pDat[(*(pPar->pPos))++];
		if(retVal[i] == 0)
		{
			break;
		}
	}
	pPar->retVal->CurLen = (IEC_STRLEN)(i);
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_string_to_byte_array
 *
 */
void copy_string_to_byte_array(STDLIBFUNCALL) 
{
	IEC_UINT		iDiff;
	IEC_UINT		i;
	IEC_BYTE		OS_DPTR *pDat;
	IEC_BYTE		OS_DPTR *inVal;

	COPY_STRING_TO_BA_PAR OS_SPTR *pPar = (COPY_STRING_TO_BA_PAR OS_SPTR *)pIN;
	
	if ((IEC_UINT)(*(pPar->pPos) + pPar->inVal->MaxLen) > GET_AR_LEN(pPar->pbArray))
	{
		libSetException(pVM, EXCEPT_ARRAY_RANGE);
		*(pPar->pPos) = (IEC_UDINT)-1;
		return;
	}
	
	pDat = GET_AR_DAT(pPar->pbArray);
	inVal = (IEC_BYTE OS_DPTR *)pPar->inVal->Contents;

	for (i = 0; i < pPar->inVal->CurLen; i++)
	{
		pDat[(*(pPar->pPos))++] = inVal[i];
	}

	/*if the actual length of the string is less than the maximum length, use this dummy to fill*/
	/*the difference with 0's*/

	iDiff = (IEC_UINT)(pPar->inVal->MaxLen - pPar->inVal->CurLen);
	if(iDiff > 0)
	{
		for(i = 0; i < iDiff; i++)
		{
			pDat[(*(pPar->pPos))++] = 0;
		}
	}
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * byte_swap_word 
 *
 */
#if defined(IP_CFG_WORD)

void byte_swap_word(STDLIBFUNCALL)
{
	BYTE_SWAP_WORD_PAR OS_SPTR *pPar = (BYTE_SWAP_WORD_PAR OS_SPTR *)pIN;

	IEC_BYTE OS_SPTR *pVal = (IEC_BYTE OS_SPTR *)&pPar->wVal;
	IEC_BYTE OS_SPTR *pRet = (IEC_BYTE OS_SPTR *)&pPar->wRet;

	IEC_UINT u,d;
	for (u = (IEC_UINT)(sizeof(IEC_WORD) - 1), d = 0; u < sizeof(IEC_WORD); u--, d++)
	{
		pRet[u] = pVal[d];
	}
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * byte_swap_dword 
 *
 */
#if defined(IP_CFG_DWORD)

void byte_swap_dword(STDLIBFUNCALL)
{
	BYTE_SWAP_DWORD_PAR OS_SPTR *pPar = (BYTE_SWAP_DWORD_PAR OS_SPTR *)pIN;

	IEC_BYTE OS_SPTR *pVal = (IEC_BYTE OS_SPTR *)&pPar->dwVal;
	IEC_BYTE OS_SPTR *pRet = (IEC_BYTE OS_SPTR *)&pPar->dwRet;

	IEC_UINT u,d;
	for (u = (IEC_UINT)(sizeof(IEC_DWORD) - 1), d = 0; u < sizeof(IEC_DWORD); u--, d++)
	{
		pRet[u] = pVal[d];
	}
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * byte_swap_lword 
 *
 */
#if defined(IP_CFG_LWORD)

void byte_swap_lword(STDLIBFUNCALL)
{
	BYTE_SWAP_LWORD_PAR OS_SPTR *pPar = (BYTE_SWAP_LWORD_PAR OS_SPTR *)pIN;

	IEC_BYTE OS_SPTR *pVal = (IEC_BYTE OS_SPTR *)&pPar->lwVal;
	IEC_BYTE OS_SPTR *pRet = (IEC_BYTE OS_SPTR *)&pPar->lwRet;

	IEC_UINT u,d;
	for (u = (IEC_UINT)(sizeof(IEC_LWORD) - 1), d = 0; u < sizeof(IEC_LWORD); u--, d++)
	{
		pRet[u] = pVal[d];
	}

}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * hi_byte 
 *
 */
#if defined(IP_CFG_BYTE) && defined(IP_CFG_WORD)

void hi_byte(STDLIBFUNCALL)
{
	HI_BYTE_PAR OS_SPTR *pPar = (HI_BYTE_PAR OS_SPTR *)pIN;

	pPar->byRet = (IEC_BYTE)((pPar->wVal & 0xff00u) >> 8u);

}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * hi_word 
 *
 */
#if defined(IP_CFG_WORD) && defined(IP_CFG_DWORD)

void hi_word(STDLIBFUNCALL)
{
	HI_WORD_PAR OS_SPTR *pPar = (HI_WORD_PAR OS_SPTR *)pIN;

	pPar->wRet = (IEC_WORD)((pPar->dwVal & 0xffff0000lu) >> 16u);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * hi_dword 
 *
 */
#if defined(IP_CFG_DWORD) && defined(IP_CFG_LWORD)

void hi_dword(STDLIBFUNCALL)
{
	HI_DWORD_PAR OS_SPTR *pPar = (HI_DWORD_PAR OS_SPTR *)pIN;

	pPar->dwRet = pPar->lwVal.H;
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * lo_byte 
 *
 */
#if defined(IP_CFG_BYTE) && defined(IP_CFG_WORD)

void lo_byte(STDLIBFUNCALL)
{
	LO_BYTE_PAR OS_SPTR *pPar = (LO_BYTE_PAR OS_SPTR *)pIN;

	pPar->byRet = (IEC_BYTE)(pPar->wVal & 0x00ffu);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * lo_word 
 *
 */
#if defined(IP_CFG_WORD) && defined(IP_CFG_DWORD)

void lo_word(STDLIBFUNCALL)
{
	LO_WORD_PAR OS_SPTR *pPar = (LO_WORD_PAR OS_SPTR *)pIN;

	pPar->wRet = (IEC_WORD)(pPar->dwVal & 0x0000fffflu);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * lo_dword 
 *
 */
#if defined(IP_CFG_DWORD) && defined(IP_CFG_LWORD)

void lo_dword(STDLIBFUNCALL)
{
	LO_DWORD_PAR OS_SPTR *pPar = (LO_DWORD_PAR OS_SPTR *)pIN;

	pPar->dwRet = pPar->lwVal.L;
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * make_word 
 *
 */
#if defined(IP_CFG_BYTE) && defined(IP_CFG_WORD)

void make_word(STDLIBFUNCALL)
{
	MAKE_WORD_PAR OS_SPTR *pPar = (MAKE_WORD_PAR OS_SPTR *)pIN;

	IEC_WORD wHi = pPar->byHI;
	IEC_WORD wLo = pPar->byLO;

	pPar->wRet = (IEC_WORD)((wHi << 8u) + wLo);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * make_dword 
 *
 */
#if defined(IP_CFG_WORD) && defined(IP_CFG_DWORD)

void make_dword(STDLIBFUNCALL)
{
	MAKE_DWORD_PAR OS_SPTR *pPar = (MAKE_DWORD_PAR OS_SPTR *)pIN;

	IEC_DWORD dwHi = pPar->wHI;
	IEC_DWORD dwLo = pPar->wLO;

	pPar->dwRet = (IEC_DWORD)((dwHi << 16u) + dwLo);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * make_lword 
 *
 */
#if defined(IP_CFG_DWORD) && defined(IP_CFG_LWORD)

void make_lword(STDLIBFUNCALL)
{
	MAKE_LWORD_PAR OS_SPTR *pPar = (MAKE_LWORD_PAR OS_SPTR *)pIN;

	pPar->lwRet.H = pPar->dwHI;
	pPar->lwRet.L = pPar->dwLO;
}
#endif


/* ---------------------------------------------------------------------------- */
/**
 * copy_to_byte_array 
 *
 */
static void copy_to_byte_array(STaskInfoVM			*pVM,		IEC_DINT	OS_DPTR *pPos, 
							   IEC_BYTE 	OS_DPTR *pDat,		IEC_BYTE	OS_SPTR *inVal, 
							   IEC_UINT 			 uLen,		IEC_UINT			 uCopy)
{
	IEC_UINT	i;

	if (((IEC_UDINT)*pPos + uCopy) > (IEC_UDINT)uLen)
	{
		libSetException(pVM, EXCEPT_ARRAY_RANGE);
		*pPos = (IEC_UDINT)-1;
		return;
	}

#ifdef RTS_CFG_BIGENDIAN
	for (i = 0; i < uCopy; i++)
#else
	for (i = (IEC_UINT)(uCopy - 1); i < uCopy; i--)
#endif
	{
		pDat[(*pPos)++] = inVal[i];
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * copy_from_byte_array 
 *
 */
static void copy_from_byte_array(STaskInfoVM		*pVM,		IEC_DINT	OS_DPTR *pPos, 
								 IEC_BYTE	OS_DPTR *pDat,		IEC_BYTE	OS_SPTR *retVal, 
								 IEC_UINT			 uLen,		IEC_UINT			 uCopy)
{	
	IEC_UINT i;

	if (((IEC_UDINT)*pPos + uCopy) > (IEC_UDINT)uLen)
	{
		libSetException(pVM, EXCEPT_ARRAY_RANGE);
		*pPos = (IEC_UDINT)-1;
		return;
	}

#ifdef RTS_CFG_BIGENDIAN
	for (i = 0; i < uCopy; i++)
#else
	for (i = (IEC_UINT)(uCopy - 1); i < uCopy; i--)
#endif
	{
		retVal[i] = pDat[(*pPos)++];
	}
}

#endif	/* RTS_CFG_UTILITY_LIB */

/* ---------------------------------------------------------------------------- */
