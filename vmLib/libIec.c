
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
 * Filename: libIec.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"libIec.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "libIec.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

#include <math.h>
#include <ctype.h>

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

extern EXECUTE_FUN	g_pLibraryFun[];
extern EXECUTE_FB	g_pLibraryFB [];

/* ----  Local Functions:	--------------------------------------------------- */

typedef struct
{
	IEC_STRLEN	CurLen;
	IEC_STRLEN	MaxLen;
	IEC_CHAR	Contents[VMM_MAX_IEC_STRLEN];

} LOC_STRING;

#define DECLARE_STRING(str, len) \
	LOC_STRING		LocStr; \
	IEC_STRING		*str = (IEC_STRING *)&LocStr; \
	str->CurLen = 0; \
	str->MaxLen = len;

#define COPY_STRING(dst, src) \
{ \
	(dst)->CurLen = (IEC_STRLEN)((dst)->MaxLen > (src)->CurLen ? (src)->CurLen : (dst)->MaxLen); \
	OS_MEMCPY((dst)->Contents, (src)->Contents, (dst)->CurLen); \
}

#define CONVERSION_ERROR		libSetError(pVM, EXCEPT_INVALID_ARG)

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * bool_to_string
 *
 */
#if defined(IP_CFG_BOOL) && defined(IP_CFG_STRING)
void bool_to_string(STDLIBFUNCALL)
{
	BOOL_TO_STRING_PAR OS_SPTR *pPar = (BOOL_TO_STRING_PAR OS_SPTR *)pIN;
	
	if (utilFormatIECString(pPar->retStr, pVM->Local.pBuffer, "%s", (pPar->in & 1) ? "true" :"false") == EXCEPT_NULL_PTR)
	{
		libSetException(pVM, EXCEPT_NULL_PTR);
	}
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * byte_to_string
 *
 */
#if defined(IP_CFG_BYTE) && defined(IP_CFG_STRING)
void byte_to_string(STDLIBFUNCALL)
{
	BYTE_TO_STRING_PAR OS_SPTR *pPar = (BYTE_TO_STRING_PAR OS_SPTR *)pIN;
	
	if (utilFormatIECString(pPar->retStr, pVM->Local.pBuffer, "%hd", (IEC_INT)pPar->in) == EXCEPT_NULL_PTR)
	{
		libSetException(pVM, EXCEPT_NULL_PTR);
	}
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * word_to_string
 *
 */
#if (defined(IP_CFG_WORD) || defined(IP_CFG_UINT)) && defined(IP_CFG_STRING)
void word_to_string(STDLIBFUNCALL)
{
	WORD_TO_STRING_PAR OS_SPTR *pPar = (WORD_TO_STRING_PAR OS_SPTR *)pIN;
			
	if (utilFormatIECString(pPar->retStr, pVM->Local.pBuffer, "%hu", pPar->in) == EXCEPT_NULL_PTR)
	{
		libSetException(pVM, EXCEPT_NULL_PTR);
	}
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * dword_to_string
 *
 */
#if (defined(IP_CFG_DWORD) || defined(IP_CFG_UDINT)) && defined(IP_CFG_STRING)
void dword_to_string(STDLIBFUNCALL)
{
	DWORD_TO_STRING_PAR OS_SPTR *pPar = (DWORD_TO_STRING_PAR OS_SPTR *)pIN;
	
	if (utilFormatIECString(pPar->retStr, pVM->Local.pBuffer, "%lu", pPar->in) == EXCEPT_NULL_PTR)
	{
		libSetException(pVM, EXCEPT_NULL_PTR);
	}	
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * int_to_string
 *
 */
#if defined(IP_CFG_INT) && defined(IP_CFG_STRING)
void int_to_string(STDLIBFUNCALL)
{
	INT_TO_STRING_PAR OS_SPTR *pPar = (INT_TO_STRING_PAR OS_SPTR *)pIN;
	
	if (utilFormatIECString(pPar->retStr, pVM->Local.pBuffer, "%d", pPar->in) == EXCEPT_NULL_PTR)
	{
		libSetException(pVM, EXCEPT_NULL_PTR);
	}
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * dint_to_string
 *
 */
#if defined(IP_CFG_DINT) && defined(IP_CFG_STRING)
void dint_to_string(STDLIBFUNCALL)
{
	DINT_TO_STRING_PAR OS_SPTR *pPar = (DINT_TO_STRING_PAR OS_SPTR *)pIN;
	
	if (utilFormatIECString(pPar->retStr, pVM->Local.pBuffer, "%ld", pPar->in) == EXCEPT_NULL_PTR)
	{
		libSetException(pVM, EXCEPT_NULL_PTR);
	}
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * real_to_string
 *
 */
#if defined(IP_CFG_REAL) && defined(IP_CFG_STRING)
void real_to_string(STDLIBFUNCALL)
{
	REAL_TO_STRING_PAR OS_SPTR *pPar = (REAL_TO_STRING_PAR OS_SPTR *)pIN;

	if (utilFormatIECString(pPar->retStr, pVM->Local.pBuffer, "%.16G" , pPar->in) == EXCEPT_NULL_PTR)
	{
		libSetException(pVM, EXCEPT_NULL_PTR);
	}
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * lreal_to_string
 *
 */
#if defined(IP_CFG_LREAL) && defined(IP_CFG_STRING)
void lreal_to_string(STDLIBFUNCALL)
{
	LREAL_TO_STRING_PAR OS_SPTR *pPar = (LREAL_TO_STRING_PAR OS_SPTR *)pIN;

	if (utilFormatIECString(pPar->retStr, pVM->Local.pBuffer, "%.16G", pPar->in) == EXCEPT_NULL_PTR)
	{
		libSetException(pVM, EXCEPT_NULL_PTR);
	}
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * LenString
 *
 */
#if defined(IP_CFG_STRING) 
void LenString_int(STDLIBFUNCALL)
{
	LEN_STRING_PAR_TYP OS_SPTR *pPar = (LEN_STRING_PAR_TYP OS_SPTR *)pIN;
	
	pPar->retVal = pPar->inStr->CurLen;
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * LeftString
 *
 */
#if defined(IP_CFG_STRING) 
void LeftString_int(STDLIBFUNCALL)
{
	LEFT_STRING_PAR_TYP OS_SPTR *pPar = (LEFT_STRING_PAR_TYP OS_SPTR OS_SPTR *)pIN;

	DECLARE_STRING(retStr, pPar->retStr->MaxLen);
	libSubstring(pVM, pPar->inStr, retStr, 0, pPar->len, FALSE);
	COPY_STRING(pPar->retStr, retStr);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * RightString
 *
 */
#if defined(IP_CFG_STRING) 
void RightString_int(STDLIBFUNCALL)
{
	RIGHT_STRING_PAR_TYP OS_SPTR *pPar = (RIGHT_STRING_PAR_TYP OS_SPTR *)pIN;
	
	IEC_UDINT ulStart = (IEC_UDINT)pPar->len > pPar->inStr->CurLen ? 0ul : pPar->inStr->CurLen - (IEC_UDINT)pPar->len;

	DECLARE_STRING(retStr, pPar->retStr->MaxLen);
	libSubstring(pVM, pPar->inStr, retStr, ulStart, pPar->inStr->CurLen, FALSE);
	COPY_STRING(pPar->retStr, retStr);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * MidString
 *
 */
#if defined(IP_CFG_STRING) 
void MidString_int(STDLIBFUNCALL)
{
	MID_STRING_PAR_TYP OS_SPTR *pPar = (MID_STRING_PAR_TYP OS_SPTR *)pIN; 
	
	IEC_UDINT ulStart = pPar->startPos != 0 ? ((IEC_UDINT)pPar->startPos & ~0x80000000) - 1ul : 0;
	IEC_UDINT ulStop  = ulStart + ((IEC_UDINT)pPar->len & ~0x80000000ul);

	DECLARE_STRING(retStr, pPar->retStr->MaxLen);
	libSubstring(pVM, pPar->inStr, retStr, ulStart, ulStop, FALSE);
	COPY_STRING(pPar->retStr, retStr);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * InsertString
 *
 */
#if defined(IP_CFG_STRING) 
void InsertString_int(STDLIBFUNCALL)
{
	INSERT_STRING_PAR_TYP OS_SPTR *pPar = (INSERT_STRING_PAR_TYP OS_SPTR *)pIN;
	
	DECLARE_STRING(retStr, pPar->retStr->MaxLen);
	libSubstring(pVM, pPar->inStr1, retStr, 0, pPar->startPos, FALSE);
	libSubstring(pVM, pPar->inStr2, retStr, 0, pPar->inStr2->CurLen, TRUE);
	libSubstring(pVM, pPar->inStr1, retStr, pPar->startPos, pPar->inStr1->CurLen, TRUE);
	COPY_STRING(pPar->retStr, retStr);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * DeleteString
 *
 */
#if defined(IP_CFG_STRING)
void DeleteString_int(STDLIBFUNCALL)
{
	DELETE_STRING_PAR_TYP OS_SPTR *pPar = (DELETE_STRING_PAR_TYP OS_SPTR *)pIN;

	IEC_UDINT ulStop  = pPar->startPos != 0 ? ((IEC_UDINT)pPar->startPos & ~0x80000000ul) - 1ul : 0;
	IEC_UDINT ulStart = ulStop + ((IEC_UDINT)pPar->len & ~0x80000000ul);

	DECLARE_STRING(retStr, pPar->retStr->MaxLen);
	libSubstring(pVM, pPar->inStr, retStr, 0, ulStop, FALSE);
	libSubstring(pVM, pPar->inStr, retStr, ulStart, pPar->inStr->CurLen, TRUE);
	COPY_STRING(pPar->retStr, retStr);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * ReplaceString
 *
 */
#if defined(IP_CFG_STRING)
void ReplaceString_int(STDLIBFUNCALL)
{
	REPLACE_STRING_PAR_TYP OS_SPTR *pPar = (REPLACE_STRING_PAR_TYP OS_SPTR *)pIN;

	IEC_UDINT ulStop  = pPar->startPos != 0 ? ((IEC_UDINT)pPar->startPos & ~0x80000000ul) - 1ul : 0;
	IEC_UDINT ulStart = ulStop + ((IEC_UDINT)pPar->len & ~0x80000000ul);

	DECLARE_STRING(retStr, pPar->retStr->MaxLen);
	libSubstring(pVM, pPar->inStr1, retStr, 0, ulStop, FALSE);
	libSubstring(pVM, pPar->inStr2, retStr, 0, pPar->inStr2->CurLen, TRUE);
	libSubstring(pVM, pPar->inStr1, retStr, ulStart, pPar->inStr1->CurLen, TRUE);
	COPY_STRING(pPar->retStr, retStr);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * ConcatString
 *
 */
#if defined(IP_CFG_STRING)
void ConcatString(STDLIBFUNCALL)
{
	IEC_UINT	i;
	IEC_UDINT	ulLen;

	DEC_EXT_CNT(argc,pIN);
	DEC_EXT_PAR(argv,SExtDec_PTR,pIN);
	DEC_EXT_FUN(pPar,SExtDec_PTR,CONCAT_STRING_PAR_TYP,pIN);
	
	IEC_STRING OS_DPTR *pS;

	DECLARE_STRING(retStr, pPar->retStr->MaxLen);

	if (pPar->inStr == 0 || pPar->retStr == 0)
	{
		libSetException(pVM, EXCEPT_NULL_PTR);
		return;
	}

	retStr->CurLen = (IEC_STRLEN)((pPar->inStr->CurLen > retStr->MaxLen) ? retStr->MaxLen : pPar->inStr->CurLen);
	OS_MEMCPY(retStr->Contents, pPar->inStr->Contents, retStr->CurLen);

	for(i = 0; i < (IEC_UINT)argc && retStr->CurLen < retStr->MaxLen; i++)
	{
		pS = (IEC_STRING OS_DPTR *)argv[i].ptr;

		ulLen = retStr->MaxLen - retStr->CurLen;
		ulLen = ulLen > pS->CurLen ? (IEC_UDINT)pS->CurLen : ulLen;
		
		OS_MEMCPY(retStr->Contents + retStr->CurLen, pS->Contents, (IEC_UINT)ulLen);

		retStr->CurLen = (IEC_STRLEN)(retStr->CurLen + ulLen);
	}

	COPY_STRING(pPar->retStr, retStr);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * FindString
 *
 */
#if defined(IP_CFG_STRING)
void FindString_int(STDLIBFUNCALL)
{
	FIND_STRING_PAR_TYP OS_SPTR *pPar = (FIND_STRING_PAR_TYP OS_SPTR *)pIN;
	
	IEC_UINT	i, j;
	IEC_BOOL	bFound = FALSE;
	
	if (pPar->inStr1 == 0 || pPar->inStr2 == 0)
	{
		libSetException(pVM, EXCEPT_NULL_PTR);
		return;
	}

	pPar->iRetVal = 0;

	if (pPar->inStr1->CurLen != 0 && pPar->inStr2->CurLen == 0)
	{
		/* Empty string
		 */
		pPar->iRetVal = 1;
		return;
	}

	if (pPar->inStr1->CurLen < pPar->inStr2->CurLen)
	{
		return;
	}

	for(i = 0; i < pPar->inStr1->CurLen - pPar->inStr2->CurLen + 1u; i++)
	{
		for(j = 0; j < pPar->inStr2->CurLen; j++)
		{
			if(pPar->inStr1->Contents[i + j] == pPar->inStr2->Contents[j])
			{
				bFound = TRUE;
			}
			else
			{
				bFound = FALSE;
				break;
			}
		}
		
		if (bFound)
		{
			pPar->iRetVal = (IEC_INT)(i + 1);
			break;
		}
	}		
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * string to bool
 *
 */
#if defined(IP_CFG_BOOL) && defined(IP_CFG_STRING)
void string_to_bool(STDLIBFUNCALL)
{
	STRING_TO_BOOL OS_SPTR *pPar = (STRING_TO_BOOL OS_SPTR *)pIN;
	
	IEC_CHAR OS_DPTR *pStr;
	IEC_UDINT ulStart;
	IEC_UDINT ulStop;
	
	for(ulStart = 0; ulStart < pPar->inStr->CurLen && OS_ISSPACE(pPar->inStr->Contents[ulStart]); ulStart++)
		;
	
	for(ulStop = pPar->inStr->CurLen; ulStop < pPar->inStr->CurLen && OS_ISSPACE(pPar->inStr->Contents[ulStart]); ulStop--)
		;
	
	if (ulStop <= ulStart)
	{
		CONVERSION_ERROR;
		pPar->retVal = FALSE;
		return;
	}

	pStr = pPar->inStr->Contents + ulStart;
	ulStart = ulStop - ulStart;
	
	if( 	OS_STRNICMP(pStr, "1", (IEC_UINT)ulStart) == 0			|| OS_STRNICMP(pStr, "true", (IEC_UINT)ulStart) == 0 
		||	OS_STRNICMP(pStr, "bool#true", (IEC_UINT)ulStart) == 0	|| OS_STRNICMP(pStr, "bool#1", (IEC_UINT)ulStart) == 0)
	{
		pPar->retVal = TRUE;
		return;
	}
	else if(	OS_STRNICMP(pStr, "0", (IEC_UINT)ulStart) == 0			|| OS_STRNICMP(pStr, "false", (IEC_UINT)ulStart) == 0 
			||	OS_STRNICMP(pStr, "bool#false", (IEC_UINT)ulStart) == 0 || OS_STRNICMP(pStr, "bool#0", (IEC_UINT)ulStart) == 0)
	{
		pPar->retVal = FALSE;
		return;
	}

	CONVERSION_ERROR;
	pPar->retVal = FALSE;
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * string to byte
 *
 */
#if defined(IP_CFG_BYTE) && defined(IP_CFG_STRING)
void string_to_byte(STDLIBFUNCALL)
{		 
	IEC_DINT lValue;
	STRING_TO_BYTE OS_SPTR *pPar = (STRING_TO_BYTE OS_SPTR *)pIN;
	
	if (utilIECStrToLong(pPar->inStr, 1, pVM->Local.pBuffer, &lValue) != OK)
	{
		CONVERSION_ERROR;
	}

	pPar->retVal = (IEC_BYTE)(lValue & 0x000000fful);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * string to word
 *
 */
#if (defined(IP_CFG_WORD) || defined(IP_CFG_UINT)) && defined(IP_CFG_STRING)
void string_to_word(STDLIBFUNCALL)
{
	IEC_DINT lValue;
	STRING_TO_WORD OS_SPTR *pPar = (STRING_TO_WORD OS_SPTR *)pIN;
	
	if (utilIECStrToLong(pPar->inStr, 1, pVM->Local.pBuffer, &lValue) != OK)
	{
		CONVERSION_ERROR;
	}

	pPar->retVal = (IEC_WORD)(lValue & 0x0000fffful);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * string to dword
 *
 */
#if (defined(IP_CFG_DWORD) || defined(IP_CFG_UDINT)) && defined(IP_CFG_STRING)
void string_to_dword(STDLIBFUNCALL)
{ 
	IEC_DINT lValue;
	STRING_TO_DWORD OS_SPTR *pPar = (STRING_TO_DWORD OS_SPTR *)pIN;
	
	if (utilIECStrToLong(pPar->inStr, 1, pVM->Local.pBuffer, &lValue) != OK)
	{
		CONVERSION_ERROR;
	}

	pPar->retVal = (IEC_DWORD)lValue;
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * string to dint
 *
 */
#if defined(IP_CFG_DINT) && defined(IP_CFG_STRING)
void string_to_dint(STDLIBFUNCALL)
{
	IEC_DINT lValue;
	STRING_TO_DINT OS_SPTR *pPar = (STRING_TO_DINT OS_SPTR *)pIN;
	
	if (utilIECStrToLong(pPar->inStr, 0, pVM->Local.pBuffer, &lValue) != OK)
	{
		CONVERSION_ERROR;
	}

	pPar->retVal = (IEC_DINT)lValue;
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * string to int
 *
 */
#if defined(IP_CFG_INT) && defined(IP_CFG_STRING)
void string_to_int(STDLIBFUNCALL)
{
	IEC_DINT lValue;
	STRING_TO_INT OS_SPTR *pPar = (STRING_TO_INT OS_SPTR *)pIN;
	
	if (utilIECStrToLong(pPar->inStr, 0, pVM->Local.pBuffer, &lValue) != OK)
	{
		CONVERSION_ERROR;
	}

	pPar->retVal = (IEC_INT)(lValue & 0x0000fffful);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * string to real
 *
 */
#if defined(IP_CFG_REAL) && defined(IP_CFG_STRING)
void string_to_real(STDLIBFUNCALL)
{
	STRING_TO_REAL OS_SPTR *pPar = (STRING_TO_REAL OS_SPTR *)pIN;
	
	pPar->retVal = (IEC_REAL)OS_ATOF(utilIecToAnsi(pPar->inStr, pVM->Local.pBuffer));
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * string to lreal
 *
 */
#if defined(IP_CFG_LREAL) && defined(IP_CFG_STRING)
void string_to_lreal(STDLIBFUNCALL)
{
	STRING_TO_LREAL OS_SPTR *pPar = (STRING_TO_LREAL OS_SPTR *)pIN;
	
	pPar->retVal = OS_ATOF(utilIecToAnsi(pPar->inStr, pVM->Local.pBuffer));
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * string_to_time
 *
 */
#if defined(IP_CFG_TIME) && defined(IP_CFG_STRING)
void string_to_time(STDLIBFUNCALL)
{
	STRING_TO_TIME OS_SPTR* pPar = (STRING_TO_TIME OS_SPTR*)pIN;
	
	IEC_BOOL	isNegative = FALSE;
	IEC_DINT	lValue;
	LOC_STRING	LocStr;
	
	IEC_CHAR OS_DPTR	*pWork = pPar->inStr->Contents;
	IEC_UINT			uLen = pPar->inStr->CurLen;
	
	IEC_CHAR *pDest = LocStr.Contents;
	LocStr.CurLen = 0;
	LocStr.MaxLen = VMM_MAX_IEC_STRLEN;
	
	pPar->retVal = 0;
	
	/* Supported time format: 
	 * t/T#[-]xxxms,t/T#[-]xxxMs,t/T#[-]xxxmS,t/T#[-]xxxMS
	 */
	
	if(pPar->inStr == 0)
	{
		CONVERSION_ERROR;
		return;
	}
	
	if (uLen == 0 || (*pWork != 't' && *pWork != 'T'))
	{
		CONVERSION_ERROR;
		return;
	}
	
	pWork++;
	uLen--;
	
	if (uLen == 0 || *pWork != '#')
	{
		CONVERSION_ERROR;
		return;
	}
	
	pWork++;
	uLen--;
	
	if(uLen == 0 || ((*pWork != '-') && (*pWork < '0' || *pWork > '9')))
	{
		CONVERSION_ERROR;
		return;
	}
	
	if(*pWork == '-')
	{
		isNegative = TRUE;
		pWork++;
		uLen--;
	}
	
	for ( ; uLen > 0; uLen--)
	{
		if (*pWork >= '0' && *pWork <= '9')
		{
			*pDest = *pWork;
			
			LocStr.CurLen++;
			
			pDest++;
			pWork++;
		}
		else
		{
			break;
		}
	}
	
	*pDest = 0;
	
	if (uLen == 0 || (*pWork != 'm' && *pWork != 'M'))
	{
		CONVERSION_ERROR;
		return;
	}
	
	pWork++;
	uLen--;
	
	if (uLen == 0 || (*pWork != 's' && *pWork != 'S'))
	{
		CONVERSION_ERROR;
		return;
	}
	
	pWork++;
	uLen--;
	
	if (utilIECStrToLong((IEC_STRING OS_DPTR *)&LocStr, 1, pVM->Local.pBuffer, &lValue) != OK)
	{
		CONVERSION_ERROR;
		return;
	}
	
	pPar->retVal = (isNegative ? -lValue : lValue);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_sin_real
 *
 */
#if defined(IP_CFG_REAL)
void iec_sin_real(STDLIBFUNCALL)
{
	SIN_REAL_PAR OS_SPTR *pPar = (SIN_REAL_PAR OS_SPTR *)pIN;

	pPar->ret = (IEC_REAL)OS_SIN(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_sin_lreal
 *
 */
#if defined(IP_CFG_LREAL)
void iec_sin_lreal(STDLIBFUNCALL)
{
	SIN_LREAL_PAR OS_SPTR *pPar = (SIN_LREAL_PAR OS_SPTR *)pIN;

	pPar->ret = OS_SIN(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_asin_real
 *
 */
#if defined(IP_CFG_REAL)
void iec_asin_real(STDLIBFUNCALL)
{
	ASIN_REAL_PAR OS_SPTR *pPar = (ASIN_REAL_PAR OS_SPTR *)pIN;
#if defined(IP_CFG_RANGE_CHECK)
	if (pPar->in < -1.0 || pPar->in > 1.0)
	{
		libSetException(pVM, EXCEPT_MATH);
		return;
	}
#endif
	pPar->ret = (IEC_REAL)OS_ASIN(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_asin_lreal
 *
 */
#if defined(IP_CFG_LREAL)
void iec_asin_lreal(STDLIBFUNCALL)
{
	ASIN_LREAL_PAR OS_SPTR *pPar = (ASIN_LREAL_PAR OS_SPTR *)pIN;
#if defined(IP_CFG_RANGE_CHECK)
	if (pPar->in < -1.0 || pPar->in > 1.0)
	{
		libSetException(pVM, EXCEPT_MATH);
		return;
	}
#endif
	pPar->ret = OS_ASIN(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_cos_real
 *
 */
#if defined(IP_CFG_REAL)
void iec_cos_real(STDLIBFUNCALL)
{
	COS_REAL_PAR OS_SPTR *pPar = (COS_REAL_PAR OS_SPTR *)pIN;
	
	pPar->ret = (IEC_REAL)OS_COS(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_cos_lreal
 *
 */
#if defined(IP_CFG_LREAL)
void iec_cos_lreal(STDLIBFUNCALL)
{
	COS_LREAL_PAR OS_SPTR *pPar = (COS_LREAL_PAR OS_SPTR *)pIN;

	pPar->ret = OS_COS(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_acos_real
 *
 */
#if defined(IP_CFG_REAL)
void iec_acos_real(STDLIBFUNCALL)
{
	ASIN_REAL_PAR OS_SPTR *pPar = (ASIN_REAL_PAR OS_SPTR *)pIN;
#if defined(IP_CFG_RANGE_CHECK)
	if (pPar->in < -1.0 || pPar->in > 1.0)
	{
		libSetException(pVM, EXCEPT_MATH);
		return;
	}
#endif
	pPar->ret = (IEC_REAL)OS_ACOS(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_acos_lreal
 *
 */
#if defined(IP_CFG_LREAL)
void iec_acos_lreal(STDLIBFUNCALL)
{
	ACOS_LREAL_PAR OS_SPTR *pPar = (ACOS_LREAL_PAR OS_SPTR *)pIN;
#if defined(IP_CFG_RANGE_CHECK)
	if (pPar->in < -1.0 || pPar->in > 1.0)
	{
		libSetException(pVM, EXCEPT_MATH);
		return;
	}
#endif
	pPar->ret = OS_ACOS(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_tan_real
 *
 */
#if defined(IP_CFG_REAL)
void iec_tan_real(STDLIBFUNCALL)
{
	TAN_REAL_PAR OS_SPTR *pPar = (TAN_REAL_PAR OS_SPTR *)pIN;
	pPar->ret = (IEC_REAL)OS_TAN(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_tan_lreal
 *
 */
#if defined(IP_CFG_LREAL)
void iec_tan_lreal(STDLIBFUNCALL)
{
	TAN_LREAL_PAR OS_SPTR *pPar = (TAN_LREAL_PAR OS_SPTR *)pIN;
	pPar->ret = OS_TAN(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_atan_real
 *
 */
#if defined(IP_CFG_REAL)
void iec_atan_real(STDLIBFUNCALL)
{
	ATAN_REAL_PAR OS_SPTR *pPar = (ATAN_REAL_PAR OS_SPTR *) pIN;
	pPar->ret = (IEC_REAL)OS_ATAN(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_atan_lreal
 *
 */
#if defined(IP_CFG_LREAL)
void iec_atan_lreal(STDLIBFUNCALL)
{
	ATAN_LREAL_PAR OS_SPTR *pPar = (ATAN_LREAL_PAR OS_SPTR *)pIN;
	pPar->ret = OS_ATAN(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_ln_real
 *
 */
#if defined(IP_CFG_REAL)
void iec_ln_real(STDLIBFUNCALL)
{
	LN_REAL_PAR OS_SPTR *pPar = (LN_REAL_PAR OS_SPTR *) pIN;
#if defined(IP_CFG_RANGE_CHECK)
	if (pPar->in <= 0.0)
	{
		libSetException(pVM, EXCEPT_MATH);
		return;
	}
#endif
	pPar->ret = (IEC_REAL)OS_LOG(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_ln_lreal
 *
 */
#if defined(IP_CFG_LREAL)
void iec_ln_lreal(STDLIBFUNCALL)
{
	LN_LREAL_PAR OS_SPTR *pPar = (LN_LREAL_PAR OS_SPTR *)pIN;
#if defined(IP_CFG_RANGE_CHECK)
	if (pPar->in <= 0.0)
	{
		libSetException(pVM, EXCEPT_MATH);
		return;
	}
#endif
	pPar->ret = OS_LOG(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_log_real
 *
 */
#if defined(IP_CFG_REAL)
void iec_log_real(STDLIBFUNCALL)
{
	LOG_REAL_PAR OS_SPTR *pPar = (LOG_REAL_PAR OS_SPTR *)pIN;
#if defined(IP_CFG_RANGE_CHECK)
	if (pPar->in < 0.0)
	{
		libSetException(pVM, EXCEPT_MATH);
		return;
	}
#endif
	pPar->ret = (IEC_REAL)OS_LOG10(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_log_lreal
 *
 */
#if defined(IP_CFG_LREAL)
void iec_log_lreal(STDLIBFUNCALL)
{
	LOG_LREAL_PAR OS_SPTR *pPar = (LOG_LREAL_PAR OS_SPTR *)pIN;
#if defined(IP_CFG_RANGE_CHECK)
	if (pPar->in < 0.0)
	{
		libSetException(pVM, EXCEPT_MATH);
		return;
	}
#endif
	pPar->ret = OS_LOG10(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_sqrt_real
 *
 */
#if defined(IP_CFG_REAL)
void iec_sqrt_real(STDLIBFUNCALL)
{
	SQRT_REAL_PAR OS_SPTR *pPar = (SQRT_REAL_PAR OS_SPTR *) pIN;
#if defined(IP_CFG_RANGE_CHECK)
	if (pPar->in < 0.0)
	{
		libSetException(pVM, EXCEPT_MATH);
		return;
	}
#endif
	pPar->ret = (IEC_REAL) OS_SQRT(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_sqrt_lreal
 *
 */
#if defined(IP_CFG_LREAL)
void iec_sqrt_lreal(STDLIBFUNCALL)
{
	SQRT_LREAL_PAR OS_SPTR *pPar = (SQRT_LREAL_PAR OS_SPTR *)pIN;
#if defined(IP_CFG_RANGE_CHECK)
	if (pPar->in < 0.0)
	{
		libSetException(pVM, EXCEPT_MATH);
		return;
	}
#endif
	pPar->ret = OS_SQRT(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_exp_real
 *
 */
#if defined(IP_CFG_REAL)
void iec_exp_real(STDLIBFUNCALL)
{
	EXP_REAL_PAR OS_SPTR *pPar = (EXP_REAL_PAR OS_SPTR *)pIN;
	pPar->ret = (IEC_REAL) OS_EXP(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * iec_exp_lreal
 *
 */
#if defined(IP_CFG_LREAL)
void iec_exp_lreal(STDLIBFUNCALL)
{
	EXP_LREAL_PAR OS_SPTR *pPar = (EXP_LREAL_PAR OS_SPTR *) pIN;
	pPar->ret = OS_EXP(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * time32_to_string
 *
 */
#if defined(IP_CFG_STRING) && defined(IP_CFG_TIME)
void time32_to_string(STDLIBFUNCALL)
{
	TIME_TO_STRING_PAR OS_SPTR *pPar = (TIME_TO_STRING_PAR OS_SPTR *)pIN;

	if (utilFormatIECString(pPar->retStr, pVM->Local.pBuffer, "t#%ldms", pPar->in) == EXCEPT_NULL_PTR)
	{
		libSetException(pVM, EXCEPT_NULL_PTR);
	}
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * mux_bit
 *
 */
#if defined(IP_CFG_BOOL)
void mux_bit(STDLIBFUNCALL)
{
	DEC_EXT_CNT(argc,pIN);
	DEC_EXT_PAR(argv,SExtDec_BYTE,pIN);
	DEC_EXT_FUN(pPar,SExtDec_BYTE,MUX_BIT_PAR_TYP,pIN);

	if(pPar->sel > (IEC_INT)argc || pPar->sel < 0)
	{
		libSetException(pVM, EXCEPT_INVALID_ARG);
		return;
	}
	
	if(pPar->sel == 0)
		pPar->ret = (IEC_BYTE)(pPar->in & 0x1);
	else
		pPar->ret = (IEC_BYTE)(argv[pPar->sel - 1].var & 0x1);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * mux_byte
 *
 */
#if defined(IP_CFG_BYTE)
void mux_byte(STDLIBFUNCALL)
{
	DEC_EXT_CNT(argc,pIN);
	DEC_EXT_PAR(argv,SExtDec_BYTE,pIN);
	DEC_EXT_FUN(pPar,SExtDec_BYTE,MUX_BYTE_PAR_TYP,pIN);
		
	if(pPar->sel > (IEC_INT)argc || pPar->sel < 0)
	{
		libSetException(pVM, EXCEPT_INVALID_ARG);
		return;
	}
	
	if(pPar->sel == 0)
		pPar->ret = pPar->in;
	else
		pPar->ret = argv[pPar->sel - 1].var;
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * mux_int
 *
 */
#if defined(IP_CFG_INT)
void mux_int(STDLIBFUNCALL)
{
	DEC_EXT_CNT(argc,pIN);
	DEC_EXT_PAR(argv,SExtDec_INT,pIN);
	DEC_EXT_FUN(pPar,SExtDec_INT,MUX_INT_PAR_TYP,pIN);
		
	if(pPar->sel > (IEC_INT)argc || pPar->sel < 0)
	{
		libSetException(pVM, EXCEPT_INVALID_ARG);
		return;
	}
	
	if(pPar->sel == 0)
		pPar->ret = pPar->in;
	else
		pPar->ret = argv[pPar->sel - 1].var;
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * mux_dint
 *
 */
#if defined(IP_CFG_DINT)
void mux_dint(STDLIBFUNCALL)
{
	DEC_EXT_CNT(argc,pIN);
	DEC_EXT_PAR(argv,SExtDec_DINT,pIN);
	DEC_EXT_FUN(pPar,SExtDec_DINT,MUX_DINT_PAR_TYP,pIN);
		
	if(pPar->sel > (IEC_INT)argc || pPar->sel < 0)
	{
		libSetException(pVM, EXCEPT_INVALID_ARG);
		return;
	}
	
	if(pPar->sel == 0)
		pPar->ret = pPar->in;
	else
		pPar->ret = argv[pPar->sel - 1].var;
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * mux_string
 *
 */
#if defined(IP_CFG_STRING)
void mux_string(STDLIBFUNCALL)
{
	DEC_EXT_CNT(argc,pIN);
	DEC_EXT_PAR(argv,SExtDec_PTR,pIN);
	DEC_EXT_FUN(pPar,SExtDec_PTR,MUX_STRING_PAR_TYP,pIN);
	
	IEC_STRING OS_DPTR *pS;

	if(pPar->sel > (IEC_INT)argc || pPar->sel < 0)
	{
		libSetException(pVM, EXCEPT_INVALID_ARG);
		return;
	}
	
	if(pPar->sel == 0)
	{
		pS = pPar->in;
	}
	else
	{
		pS = (IEC_STRING OS_DPTR *)argv[pPar->sel - 1].ptr;
	}

	pPar->ret->CurLen = (IEC_STRLEN)(pS->CurLen > pPar->ret->MaxLen ? pPar->ret->MaxLen : pS->CurLen);
	OS_MEMCPY(pPar->ret->Contents, pS->Contents, pPar->ret->CurLen);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * mux_64bit
 *
 */
#if defined(IP_CFG_LREAL)
void mux_64bit(STDLIBFUNCALL)
{
	DEC_EXT_CNT(argc,pIN);
	DEC_EXT_PAR(argv,IEC_LREAL,pIN);
	DEC_EXT_FUN(pPar,IEC_LREAL,MUX_64BIT_PAR_TYP,pIN);
	
	if(pPar->sel > (IEC_INT)argc || pPar->sel < 0)
	{
		libSetException(pVM, EXCEPT_INVALID_ARG);
		return;
	}

	if(pPar->sel == 0)
		pPar->ret = pPar->in;
	else
		pPar->ret = argv[pPar->sel - 1];
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * trunc_real_dint
 *
 */
#if defined(IP_CFG_REAL) && defined(IP_CFG_DINT)
void trunc_real_dint(STDLIBFUNCALL)
{  
	TRUNC_REAL_DINT_PAR_TYP OS_SPTR *pPar = (TRUNC_REAL_DINT_PAR_TYP OS_SPTR *) pIN;
	pPar->ret = (IEC_DINT)(pPar->in);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * trunc_lreal_dint
 *
 */
#if defined(IP_CFG_LREAL) && defined(IP_CFG_DINT)
void trunc_lreal_dint(STDLIBFUNCALL)
{  
	TRUNC_LREAL_DINT_PAR_TYP OS_SPTR *pPar = (TRUNC_LREAL_DINT_PAR_TYP OS_SPTR *) pIN;
	pPar->ret = (IEC_DINT)(pPar->in);
}
#endif



/* IEC LIBRARY FUNCTIONS BLOCKS
 * ----------------------------------------------------------------------------
 */

/* ---------------------------------------------------------------------------- */
/**
 * rs
 *
 */
#if defined(IP_CFG_BOOL)
void rs(STDLIBFBCALL)
{
	RS_TYP OS_DPTR *p = (RS_TYP OS_DPTR *) pIN;
	
	p->b  = (IEC_BOOL)(/*(p->b & 0xFB) | */
	  ((!(p->b & 2) && ((p->b & 4) || (p->b & 1))) ? (p->b | 4) : (p->b & (~4))));	/* 7 6 5 4 3 Q1 R S 	*/
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * sr
 *
 */
#if defined(IP_CFG_BOOL)
void sr(STDLIBFBCALL)
{
	SR_TYP OS_DPTR *p = (SR_TYP OS_DPTR *) pIN;
	
	p->b  = (IEC_BOOL)(/*(p->b & 0xFB) | */
		((( !(p->b & 2) && (p->b & 4)) || (p->b & 1)) ? (p->b | 4) : (p->b & (~4))));  /* 7 6 5 4 3 Q1 R S	   */
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * tp
 *
 */
#if defined(IP_CFG_BOOL) && defined(IP_CFG_TIME)
void tp(STDLIBFBCALL)
{
	TP_TON_TOF_PAR OS_DPTR *pPar = (TP_TON_TOF_PAR OS_DPTR *)pIN;
	
	if (pPar->PT == 0)
	{
		/* Invalid parameter
		 */
		pPar->Q 	= 0;
		pPar->ET	= 0;
		pPar->bTrig = FALSE;

		pPar->LastIN= pPar->IN;

		return;
	}

	if (pPar->bTrig == FALSE)
	{
		pPar->Q = 0;

		if (pPar->IN == 1 && pPar->LastIN == 0)
		{
			/* Start timer
			 */
			pPar->bTrig  = TRUE;
			pPar->tStart = osGetTime32();
			pPar->ET	 = 0;
			pPar->Q 	 = 1;
		}
		else
		{
			if (pPar->IN == 0)
			{
				pPar->ET = 0;
			}
		}

	} /* if (pPar->bTrig == FALSE) */

	else 
	{
		/* Get elapsed time 
		 */
		IEC_UDINT ulNow = osGetTime32();

		pPar->ET = ulNow - pPar->tStart;

		if(pPar->ET >= pPar->PT)
		{
			/* Timer triggered 
			 */
			pPar->bTrig = FALSE;
			pPar->Q 	= 0;

			if(pPar->LastIN == 0)
			{
				pPar->ET = 0;
			}
			else
			{
				pPar->ET = pPar->PT;
			}
		}

	} /* else (pPar->bTrig == FALSE) */

	pPar->LastIN = pPar->IN;
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * ton
 *
 */
#if defined(IP_CFG_BOOL) && defined(IP_CFG_TIME)
void ton(STDLIBFBCALL)
{
	TP_TON_TOF_PAR OS_DPTR *pPar = (TP_TON_TOF_PAR OS_DPTR *)pIN;
	
	if (pPar->PT == 0)
	{
		/* Invalid parameter
		 */
		pPar->Q 	= pPar->IN;
		pPar->ET	= 0;
		pPar->bTrig = FALSE;

		return;
	}	 

	if (pPar->IN == 0)
	{
		/* Reset
		 */
		pPar->Q 	= 0;
		pPar->ET	= 0;
		pPar->bTrig = FALSE;

		return;
	}

	if (pPar->bTrig == FALSE)
	{
		/* Start timer
		 */
		pPar->bTrig  = TRUE;
		pPar->tStart = osGetTime32();

		return;
	}

	if (pPar->Q == 0)
	{
		/* Verify timer
		 */
		IEC_UDINT ulNow = osGetTime32();
		
		pPar->ET = ulNow - pPar->tStart;

		if (pPar->ET >= pPar->PT)
		{
			/* Timer triggered
			 */
			pPar->Q  = 1;
			pPar->ET = pPar->PT;
		}

	} /* if (pPar->Q == 0) */
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * tof
 *
 */
#if defined(IP_CFG_BOOL) && defined(IP_CFG_TIME)
void tof(STDLIBFBCALL)
{
	TP_TON_TOF_PAR OS_DPTR *pPar = (TP_TON_TOF_PAR OS_DPTR *)pIN;
	
	if (pPar->PT == 0)
	{
		/* Invalid parameter
		 */
		pPar->Q 	= pPar->IN;
		pPar->ET	= 0;
		pPar->bTrig = FALSE;

		return;
	}

	if(pPar->IN == 1)
	{
		/* Reset
		 */
		pPar->Q 	= 1;
		pPar->ET	= 0;
		pPar->bTrig = FALSE;

		return;
	}

	if (pPar->bTrig == FALSE)
	{
		/* Start timer
		 */
		pPar->bTrig  = TRUE;
		pPar->tStart = osGetTime32();

		return;
	}

	if (pPar->Q == 1)
	{
		/* Verify timer
		 */
		IEC_UDINT ulNow = osGetTime32();

		pPar->ET = ulNow - pPar->tStart;

		if(pPar->ET >= pPar->PT)
		{
			/* Timer triggered
			 */
			pPar->Q  = 0;
			pPar->ET = pPar->PT;
		}

	} /* if (pPar->Q == 1) */
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * ctu
 *
 */
#if defined(IP_CFG_BOOL) && defined(IP_CFG_INT)
void ctu(STDLIBFBCALL)
{
	CTU_PAR OS_DPTR *pPar = (CTU_PAR OS_DPTR *)pIN;

	/* 7 6 5 4 3 CU_EDGE(helper) R CU */
	IEC_UINT bTmp = (IEC_UINT)((((pPar->bIn & 0x4) == 0) && (pPar->bIn & 0x1) != 0) ? 1 : 0);

	if(pPar->bIn & 0x2)
		pPar->cvOut = 0;
	else
	{		 
		pPar->bIn = (IEC_BYTE)((pPar->bIn & 0x1) ? (pPar->bIn | 0x4) : (pPar->bIn & ~0x4)); /*update state*/
		pPar->cvOut = (IEC_INT)(pPar->cvOut + (IEC_INT)((bTmp && (pPar->bIn & 0x1) && (pPar->cvOut < CTU_PV_MAX)) ? 1 : 0)); /*add 1 if needed*/
	}
	pPar->qOut = (IEC_BYTE)((pPar->cvOut >= pPar->pvIn) ? (pPar->qOut | 0x1) : (pPar->qOut & ~0x1));	
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * ctd
 *
 */
#if defined(IP_CFG_BOOL) && defined(IP_CFG_INT)
void ctd(STDLIBFBCALL)
{
	CTD_PAR OS_DPTR *pPar = (CTD_PAR OS_DPTR *)pIN;
	
	/* 7 6 5 4 3 CU_EDGE(helper) R CU */
	IEC_UINT bTmp = (IEC_UINT)((((pPar->bIn & 0x4) == 0) && (pPar->bIn & 0x1)) ? 1 : 0);

	if(pPar->bIn & 0x2)
		pPar->cvOut = pPar->pvIn;
	else
	{		 
		pPar->bIn = (IEC_BYTE)((pPar->bIn & 0x1) ? (pPar->bIn | 0x4) : (pPar->bIn & ~0x4));
		pPar->cvOut = (IEC_INT)(pPar->cvOut - (IEC_INT)((bTmp && (pPar->bIn & 0x1) && (pPar->cvOut > CTD_PV_MIN)) ? 1 : 0));
	}
	pPar->qOut = (IEC_BYTE)((pPar->cvOut <= 0) ? (pPar->qOut | 0x1) : (pPar->qOut & ~0x1));    
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * ctud
 *
 */
#if defined(IP_CFG_BOOL) && defined(IP_CFG_INT)
void ctud(STDLIBFBCALL)
{	
	CTUD_PAR OS_DPTR *pPar = (CTUD_PAR OS_DPTR *)pIN;

	/* IEC_BYTE  bIn; 7 6 CD_EDGE CU_EDGE LD R CD CU*/
	IEC_UINT bTmpCU = (IEC_UINT)((((pPar->bIn & 0x10) == 0) && (pPar->bIn & 0x1)) ? 1 : 0);
	IEC_UINT bTmpCD = (IEC_UINT)((((pPar->bIn & 0x20) == 0) && (pPar->bIn & 0x2)) ? 1 : 0);

	if (pPar->bIn & 0x4)
	{
		pPar->cvOut = 0;
	}
	else
	{
		if (pPar->bIn & 0x8)
		{
			pPar->cvOut = pPar->pvIn;
		}
		else
		{
			/* Update State
			 */
			pPar->bIn = (IEC_BYTE)((pPar->bIn & 0x1) ? (pPar->bIn | 0x10) : (pPar->bIn & ~0x10));
			pPar->bIn = (IEC_BYTE)((pPar->bIn & 0x2) ? (pPar->bIn | 0x20) : (pPar->bIn & ~0x20));

			pPar->cvOut = (IEC_INT)(pPar->cvOut + (IEC_INT)((bTmpCU && (pPar->bIn & 0x1) && (pPar->cvOut < CTU_PV_MAX)) ? 1 : 0));
			pPar->cvOut = (IEC_INT)(pPar->cvOut - (IEC_INT)((bTmpCD && (pPar->bIn & 0x2) && (pPar->cvOut > CTD_PV_MIN)) ? 1 : 0));
		}
	}

	/* 7 6 5 4 3 2 QU QD */
	pPar->bOut = (IEC_BYTE)((pPar->cvOut >= pPar->pvIn) ? (pPar->bOut | 0x1) : (pPar->bOut & ~0x1));
	pPar->bOut = (IEC_BYTE)((pPar->cvOut <= 0		  ) ? (pPar->bOut | 0x2) : (pPar->bOut & ~0x2));	
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * r_trig
 *
 */
#if defined(IP_CFG_BOOL)
void r_trig(STDLIBFBCALL)
{
	TRIG_PAR OS_DPTR *pPar = (TRIG_PAR OS_DPTR *)pIN;
	
	if(((pPar->b & 0x1) != 0) && ((pPar->b & 0x4) == 0))
		pPar->b |= 0x2;
	else
		pPar->b &= 0xfd;
	
	if(pPar->b & 0x1)
		pPar->b |= 0x4;
	else
		pPar->b &= 0xfb;
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * f_trig
 *
 */
#if defined(IP_CFG_BOOL)
void f_trig(STDLIBFBCALL)
{
	TRIG_PAR OS_DPTR *pPar = (TRIG_PAR OS_DPTR*)pIN;
	
	if(((pPar->b & 0x1) == 0) && ((pPar->b & 0x4) == 0))
		pPar->b |= 0x2;
	else
		pPar->b &= 0xfd;

	if(((pPar->b & 0x1) == 0))
		pPar->b |= 0x4;
	else
		pPar->b &= 0xfb;
}
#endif

/* ---------------------------------------------------------------------------- */
