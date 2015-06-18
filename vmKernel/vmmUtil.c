
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
 * Filename: vmmUtil.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__		"vmmUtil.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

#include <ctype.h>
#include <stdarg.h>

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * utilIECStrCmp
 *
 */
IEC_INT utilIECStrCmp(IEC_STRING OS_LPTR *pStr1, IEC_STRING OS_LPTR *pStr2)
{
	IEC_UINT i;
	IEC_UINT uLen;
	
	uLen = (IEC_UINT)(pStr1->CurLen < pStr2->CurLen ? pStr1->CurLen : pStr2->CurLen);

	for (i = 0; i < uLen; i++)
	{
		if (pStr1->Contents[i] > pStr2->Contents[i])
		{
			return 1;
		}
		else if (pStr1->Contents[i] < pStr2->Contents[i])
		{
			return -1;
		}
	}

	if (pStr1->CurLen < pStr2->CurLen)
	{
		return -1;
	}
	else if (pStr1->CurLen > pStr2->CurLen)
	{
		return 1;
	}

	return 0;
}

/* ---------------------------------------------------------------------------- */
/**
 * utilIECStrToLong
 *
 * Converts an IEC string to a long value.
 * 
 * @return			Long value of string or 0 if conversion failed.
 */
IEC_UINT utilIECStrToLong(IEC_STRING OS_LPTR *inStr, IEC_UINT bUnsigned, IEC_CHAR *szBuffer, IEC_DINT *lpValue)
{
	IEC_BOOL	bNeg		= FALSE;
	IEC_CHAR	*pStopChar;
	IEC_UINT	i;
	IEC_INT 	base		= 10;
	IEC_INT 	iOffs		= 0;

	*lpValue = 0;

	/* Strip leading white space
	 */
	for(i = 0; i < inStr->CurLen && OS_ISSPACE(inStr->Contents[i]); i++)
		;
	
	if (i == inStr->CurLen)
	{
		RETURN(ERR_ERROR);
	}

	/* Get a possible sign or radix prefix 
	 */
	if (i < inStr->CurLen && inStr->Contents[i] == '-')
	{
		bNeg = TRUE;
		i++;
	}
	else if (i < inStr->CurLen && inStr->Contents[i] == '+')
	{
		bNeg = FALSE;
		i++;
	}
	else if (i + 1 < inStr->CurLen && inStr->Contents[i + 1] == '#')
	{
		if (inStr->Contents[i] == '2')
		{
			base  = 2;
			i	 += 2; 
		}
		else if (inStr->Contents[i] == '8')
		{
			base  = 8;
			i	 += 2;
		}
		else
		{
			RETURN(ERR_ERROR);
		}
	} 
	else if (i + 2 < inStr->CurLen && inStr->Contents[i + 2] == '#')
	{
		if (inStr->Contents[i] == '1' && inStr->Contents[i + 1] == '6')
		{
			base  = 16;
			i	 += 3;
		}
		else
		{
			RETURN(ERR_ERROR);
		}
	}
	
	if (i >= inStr->CurLen)
	{
		RETURN(ERR_ERROR);
	}

	/* Skip leading zero's
	 */
	for( ; i < inStr->CurLen && inStr->Contents[i] == '0'; i++)
		;

	if (i >= inStr->CurLen)
	{
		/* We have a zero
		 */
		RETURN(OK);
	}

	if (inStr->Contents[i] == 'x' || inStr->Contents[i] == 'X')
	{
		/* x will advice strtol to convert to a hexadecimal number
		 */
		RETURN(ERR_ERROR);
	}

	if (bNeg)
	{
		szBuffer[0] = '-';
		iOffs = 1;
	}

	OS_MEMCPY(szBuffer + iOffs, inStr->Contents + i, inStr->CurLen - i);
	szBuffer[inStr->CurLen - i + iOffs] = 0;

	if (bUnsigned)
	{
		*lpValue = OS_STRTOUL(szBuffer, &pStopChar, base);
	}
	else
	{
		*lpValue  = OS_STRTOL(szBuffer, &pStopChar, base);
	}
	
	if (*lpValue == 0)
	{
		/* We must not have a zero at this point, because the zero
		 * is handled above.
		 */
		RETURN(ERR_ERROR);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * utilStrnicmp
 *
 * compare two NULL terminate strings case non sensitiv
 * 
 * @return <0 => str1 < str2; 0 => str1 = str2 ; 0< => str1 > str2 
 */
IEC_INT utilStrnicmp(IEC_CHAR OS_LPTR *szStr1, IEC_CHAR OS_LPTR *szStr2, IEC_UDINT uLen)
{
	IEC_INT  iRes = 0;
	IEC_CHAR ch1;
	IEC_CHAR ch2;
	
	while (uLen) 
	{
		ch1 = *szStr1;
		ch2 = *szStr2;
		
		if (ch1 >= 'a' && ch1 <= 'z')
			ch1 -= 0x20;
		
		if (ch2 >= 'a' && ch2 <= 'z')
			ch2 -= 0x20;
		
		iRes = (IEC_INT)(ch1 - ch2);
		
		if (iRes != 0 || ch1 == 0)
		{
			break;
		}
		
		szStr1++;
		szStr2++;
		
		uLen--;
	}

	return iRes;
}

/* ---------------------------------------------------------------------------- */
/**
 * utilStricmp
 *
 * compare two NULL terminate strings case non sensitiv
 * 
 * @return <0 => str1 < str2; 0 => str1 = str2 ; 0< => str1 > str2 
 */
IEC_INT utilStricmp(IEC_CHAR OS_LPTR *szStr1, IEC_CHAR OS_LPTR *szStr2)
{
	IEC_INT iLen1 = (IEC_INT)OS_STRLEN(szStr1);
	IEC_INT iLen2 = (IEC_INT)OS_STRLEN(szStr2);

	if (iLen1 != iLen2)
	{
		return (IEC_INT)(iLen1 - iLen2);
	}

	return utilStrnicmp(szStr1, szStr2, iLen1);
}

/* ---------------------------------------------------------------------------- */
/**
 * utilIecToAnsi
 *
 * Converts an IEC string to a null terminated ANSI string. The ANSI
 * string is truncated if necessary.
 *
 * Note: szAnsi must be sized at least (MAX_IEC_STRLEN + 1)!
 */
IEC_CHAR *utilIecToAnsi(IEC_STRING OS_LPTR *strIEC, IEC_CHAR *szAnsi)
{
	OS_MEMCPY(szAnsi, strIEC->Contents, strIEC->CurLen); 
	szAnsi[strIEC->CurLen] = 0;

	return szAnsi;
}

/* ---------------------------------------------------------------------------- */
/**
 * utilAnsiToIec
 *
 * Converts an null terminated ANSI string to an IEC string. The string is
 * truncated if necessary.
 *
 */
IEC_UINT utilAnsiToIec(IEC_CHAR *szAnsi, IEC_STRING OS_LPTR *strIEC)
{
	IEC_UINT uRes = OK;

	if (utilCheckString(szAnsi, strIEC->MaxLen))
	{
		strIEC->CurLen = (IEC_STRLEN)OS_STRLEN(szAnsi);
	}
	else
	{
		strIEC->CurLen = strIEC->MaxLen;
		uRes = WRN_TRUNCATED;
	}

	OS_MEMCPY(strIEC->Contents, szAnsi, strIEC->CurLen); 

	return uRes;
}

/* ---------------------------------------------------------------------------- */
/**
 * utilCopyIecString
 *
 * Copies an IEC string to another.
 *
 * @return	OK				Source string fits into destination string.
 *			WRN_TRUNCATED	String doesn't fit into destionation string and has
 *							been truncated.
 */
IEC_UINT utilCopyIecString(IEC_STRING OS_LPTR *strDest, IEC_STRING OS_LPTR *strSrc)
{
	IEC_UINT uRes = (IEC_UINT)(strDest->MaxLen >= strSrc->CurLen ? OK : WRN_TRUNCATED);

	strDest->CurLen = (IEC_STRLEN)(uRes == OK ? strSrc->CurLen : strDest->MaxLen);	
	OS_MEMCPY(strDest->Contents, strSrc->Contents, strDest->CurLen);

	return uRes;
}

/* ---------------------------------------------------------------------------- */
/**
 * utilFormatIECString
 *
 * Formats an IEC string like sprintf. szBuffer is used as a temporary
 * buffer and must be sized at least (MAX_IEC_STRLEN + 1)!
 */
IEC_UINT utilFormatIECString(IEC_STRING OS_LPTR *strIEC, IEC_CHAR *szBuffer, IEC_CHAR *szFormat, ...)
{
	IEC_INT iCount;
	va_list va;

	if(strIEC == 0 || strIEC->MaxLen == 0)
	{
		RETURN(EXCEPT_NULL_PTR);
	}
	
	va_start(va, szFormat);
	iCount = (IEC_INT)OS_VSPRINTF(szBuffer, szFormat, va); 
	va_end(va);

	if (iCount < 0)
	{
		strIEC->CurLen = 0;
		RETURN(ERR_ERROR);
	}

	strIEC->CurLen = (IEC_STRLEN)((IEC_STRLEN)iCount > strIEC->MaxLen ? strIEC->MaxLen : (IEC_STRLEN)iCount);
	OS_MEMCPY (strIEC->Contents, szBuffer, strIEC->CurLen);

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * utilFormatString
 *
 * Formats an ANSI string like sprintf. 
 */
IEC_CHAR *utilFormatString(IEC_CHAR *szBuffer, IEC_CHAR *szFormat, ...)
{	
	IEC_INT iCount;
	va_list va;

	va_start(va, szFormat);
	iCount = (IEC_INT)OS_VSPRINTF(szBuffer, szFormat, va); 
	va_end(va);
		
	if (iCount < 0)
	{
		szBuffer[0] = 0;
	}

	return szBuffer;
}

/* ---------------------------------------------------------------------------- */
/**
 * utilTruncCRLF
 *
 * Truncates CR/LF characters from the end of a string. 
 */
IEC_UINT utilTruncCRLF(IEC_CHAR *szString)
{
	IEC_UINT uLen;

	for (uLen = (IEC_UINT)OS_STRLEN(szString); uLen != 0 && (szString[uLen - 1] == '\r' || szString[uLen -1] == '\n'); uLen--)
	{
		szString[uLen - 1] = '\0';
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * utilRealToDint
 *
 */

#if defined(IP_CFG_REAL)

IEC_DINT utilRealToDint(IEC_REAL fValue, IEC_DINT lCast, IEC_DINT lMin, IEC_DINT lMax)
{
	/* NOTE: When changing this function, change the (same) function utilRealToDint()
	 * from above also.
	 */

	if (fValue >= (IEC_UDINT)lMax)
	{
		return lMax;
	}

	else if (fValue <= lMin)
	{
		return lMin;
	}
		
	/* Round the casted value according to IEC 559. 
	 */
	fValue = fValue - lCast;
	
	if (fValue > 0.5)
	{
		/* +3.7 - +3 --> +3 + +1 = +4
		 */
		lCast += 1;
	}
	else if (fValue < -0.5)
	{
		/* -3.7 - -3 --> -3 - -1 = -4
		 */
		lCast -= 1;
	}
	else if (fValue == 0.5)
	{
		/* +3.5 - +3 --> +3 + +1 = +4
		 * +4.5 - +4 --> +4 + +0 = +4
		 */
		lCast += (lCast & 0x1);
	}
	else if (fValue == -0.5)
	{
		/* -3.5 - -3 --> -3 + -1 = -4
		 * -4.5 - -4 --> -4 + -0 = -4
		 */
		lCast -= (lCast & 0x1);
	}
	
	return lCast;
}

#endif	/* defined(IP_CFG_REAL) */

/* ---------------------------------------------------------------------------- */
/**
 * utilLrealToDint
 *
 */

#if defined(IP_CFG_LREAL)

IEC_DINT utilLrealToDint(IEC_LREAL fValue, IEC_DINT lCast, IEC_DINT lMin, IEC_DINT lMax)
{
	/* NOTE: When changing this function, change the (same) function utilRealToDint()
	 * from above also.
	 */

	if (fValue >= (IEC_UDINT)lMax)
	{
		return lMax;
	}

	else if (fValue <= lMin)
	{
		return lMin;
	}
		
	/* Round the casted value according to IEC 559. 
	 */
	fValue = fValue - lCast;
	
	if (fValue > 0.5)
	{
		/* +3.7 - +3 --> +3 + +1 = +4
		 */
		lCast += 1;
	}
	else if (fValue < -0.5)
	{
		/* -3.7 - -3 --> -3 - -1 = -4
		 */
		lCast -= 1;
	}
	else if (fValue == 0.5)
	{
		/* +3.5 - +3 --> +3 + +1 = +4
		 * +4.5 - +4 --> +4 + +0 = +4
		 */
		lCast += (lCast & 0x1);
	}
	else if (fValue == -0.5)
	{
		/* -3.5 - -3 --> -3 + -1 = -4
		 * -4.5 - -4 --> -4 + -0 = -4
		 */
		lCast += (lCast & 0x1);
	}
	
	return lCast;
}

#endif	/* defined(IP_CFG_LREAL) */

/* ---------------------------------------------------------------------------- */
/**
 * utilAppendPath
 *
 */
#if defined(RTS_CFG_FILE_NATIVE) || defined(RTS_CFG_FILE_ACCESS)

IEC_UINT utilAppendPath(IEC_CHAR *szDir, IEC_UINT uSize, IEC_CHAR *szAppend, IEC_BOOL bDir)
{
	IEC_UINT uLen = (IEC_UINT)OS_STRLEN(szDir);

	/* old + new + zero byte + final '\' + delimiter '\'
	 */
	if (OS_STRLEN(szDir) + OS_STRLEN(szAppend) + 1 + (bDir ? 1 : 0) + (szDir[uLen] != VMM_PATH_DELI ? 1 : 0) > uSize)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	if (szDir[uLen - 1] != VMM_PATH_DELI)
	{
		szDir[uLen++] = VMM_PATH_DELI;
		szDir[uLen	] = '\0';
	}

	OS_STRCAT(szDir, szAppend);

	if (bDir == TRUE)
	{
		uLen = (IEC_UINT)OS_STRLEN(szDir);

		szDir[uLen++] = VMM_PATH_DELI;
		szDir[uLen	] = '\0';
	}
	
	RETURN(OK);
}

#endif /* RTS_CFG_FILE_NATIVE || RTS_CFG_FILE_ACCESS */

/* ---------------------------------------------------------------------------- */
/**
 * utilIs_8_3
 *
 */
#if defined(RTS_CFG_FILE_NATIVE) || defined(RTS_CFG_FILE_ACCESS)

IEC_BOOL utilIs_8_3(IEC_CHAR *szPath)
{
	IEC_UINT i;
	IEC_UINT uLen = (IEC_UINT)OS_STRLEN(szPath);

	for (i = 0; i <= uLen; i++)
	{
		IEC_UINT j;

		for (j = 0; szPath[i] != '\0' && szPath[i] != '.' && szPath[i] != VMM_PATH_DELI; j++)
		{
			i++;
		}

		if (j > 8)
		{
			return FALSE;
		}
		else if (szPath[i] == '\0')
		{
			return (IEC_BOOL)(j <= 8);
		}
		else if (szPath[i] == '.')
		{
			return (IEC_BOOL)(uLen - i - 1 <= 3);
		}

	}

	/* Should not reach this point
	 */
	return FALSE;
}

#endif /* RTS_CFG_FILE_NATIVE || RTS_CFG_FILE_ACCESS */

/* ---------------------------------------------------------------------------- */
/**
 * utilCheckString
 *
 */
IEC_BOOL utilCheckString(IEC_CHAR *szString, IEC_UINT uMax)
{
	IEC_UINT i;

	for (i = 0; i < uMax; i++)
	{
		if (szString[i] == '\0')
		{
			/* OK
			 */
			return TRUE;
		}
	}

	/* String is larger then uMax
	 */
	return FALSE;
}

/* ---------------------------------------------------------------------------- */
/**
 * utilCreateDir
 *
 */
#if defined(RTS_CFG_FILE_ACCESS) || defined(RTS_CFG_FILE_NATIVE)

IEC_UINT utilCreateDir(IEC_CHAR *pBuffer, IEC_UINT uLen, 
			IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szFile)
{
	IEC_UINT uDummy;

	IEC_UINT uRes = utilCreatePath(pBuffer, uLen, fpGetDir, szDir, szFile);
	if (uRes != OK)
	{
		RETURN(OK);
	}

	uDummy = (IEC_UINT)(OS_STRLEN(pBuffer));
	pBuffer[uDummy] 	= VMM_PATH_DELI;
	pBuffer[uDummy + 1] = '\0';

	uRes = xxxCreateDir(pBuffer, TRUE, DIR_MODE_WRITE);

	RETURN(uRes);
}

#endif /* RTS_CFG_FILE_ACCESS || RTS_CFG_FILE_NATIVE */

/* ---------------------------------------------------------------------------- */
/**
 * utilCreateFile
 *
 */
#if defined(RTS_CFG_FILE_ACCESS) || defined(RTS_CFG_FILE_NATIVE)

IEC_UINT utilCreateFile(IEC_UDINT *hpFile, IEC_CHAR *pBuffer, IEC_UINT uLen, 
			IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szFile)
{
	IEC_UINT uRes = fpGetDir(pBuffer, uLen);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = utilAppendPath(pBuffer, uLen, szDir, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = xxxCreateDir(pBuffer, TRUE, DIR_MODE_WRITE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = utilAppendPath(pBuffer, uLen, szFile, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = xxxOpen(hpFile, pBuffer, FIO_MODE_WRITE, FALSE);

	RETURN(uRes);
}

#endif /* RTS_CFG_FILE_ACCESS || RTS_CFG_FILE_NATIVE */

/* ---------------------------------------------------------------------------- */
/**
 * utilOpenFile
 *
 */
#if defined(RTS_CFG_FILE_ACCESS) || defined(RTS_CFG_FILE_NATIVE)

IEC_UINT utilOpenFile(IEC_UDINT *hpFile, IEC_CHAR *pBuffer, IEC_UINT uLen, 
					  IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, 
					  IEC_CHAR *szFile, IEC_UINT uMode)
{
	IEC_UINT uRes = utilCreatePath(pBuffer, uLen, fpGetDir, szDir, szFile);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = xxxOpen(hpFile, pBuffer, uMode, FALSE);

	RETURN(uRes);
}

#endif /* RTS_CFG_FILE_ACCESS || RTS_CFG_FILE_NATIVE */

/* ---------------------------------------------------------------------------- */
/**
 * utilExistFile
 *
 */
#if defined(RTS_CFG_FILE_ACCESS) || defined(RTS_CFG_FILE_NATIVE)

IEC_UINT utilExistFile(IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), 
					   IEC_CHAR *szDir, IEC_CHAR *szFile, IEC_BOOL *bpExist)
{
	IEC_UINT uRes = utilCreatePath(pBuffer, uLen, fpGetDir, szDir, szFile);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = xxxExist(pBuffer, bpExist);

	RETURN(uRes);
}

#endif /* RTS_CFG_FILE_ACCESS || RTS_CFG_FILE_NATIVE */

/* ---------------------------------------------------------------------------- */
/**
 * utilDeleteFile
 *
 */
#if defined(RTS_CFG_FILE_ACCESS) || defined(RTS_CFG_FILE_NATIVE)

IEC_UINT utilDeleteFile(IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), 
						IEC_CHAR *szDir, IEC_CHAR *szFile)
{
	IEC_UINT uRes = utilCreatePath(pBuffer, uLen, fpGetDir, szDir, szFile);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	xxxRemove(pBuffer);

	RETURN(OK);
}

#endif /* RTS_CFG_FILE_ACCESS || RTS_CFG_FILE_NATIVE */

/* ---------------------------------------------------------------------------- */
/**
 * utilRenameFile
 *
 */
#if defined(RTS_CFG_FILE_ACCESS) || defined(RTS_CFG_FILE_NATIVE)

IEC_UINT utilRenameFile(IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), 
						IEC_CHAR *szDir, IEC_CHAR *szFrom, IEC_CHAR *szTo)
{
	IEC_UINT uRes = OK;
	
	uLen = (IEC_UINT)(uLen / 2);

	uRes = utilCreatePath(pBuffer, uLen, fpGetDir, szDir, szFrom);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = utilCreatePath(pBuffer + uLen, uLen, fpGetDir, szDir, szTo);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = xxxRename(pBuffer, pBuffer + uLen);

	RETURN(uRes);
}

#endif /* RTS_CFG_FILE_ACCESS || RTS_CFG_FILE_NATIVE */

/* ---------------------------------------------------------------------------- */
/**
 * utilDeleteList
 *
 */
#if defined(RTS_CFG_STORE_FILES) || defined(RTS_CFG_FIRMWARE_DL)

IEC_UINT utilDeleteList(IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), 
						IEC_CHAR *szDir, IEC_CHAR *szMap, IEC_CHAR *szFile)
{
	IEC_UINT	uRes = OK;
	IEC_UDINT	hMap;
	IEC_UDINT	hTemp;
	IEC_BOOL	bExist;

	/* Open map file and create temporary file
	 */
	uRes = utilExistFile(pBuffer, uLen, fpGetDir, szDir, szMap, &bExist);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (bExist == FALSE)
	{
		/* We (hopefully) don't have a map file, so there should be nothing to delete
		 */
		RETURN(OK);
	}

	uRes = utilOpenFile(&hMap, pBuffer, uLen, fpGetDir, szDir, szMap, FIO_MODE_READ);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = utilCreateFile(&hTemp, pBuffer, uLen, fpGetDir, szDir, VMM_FILE_TEMP);
	if (uRes != OK)
	{
		xxxClose(hMap);
		RETURN(uRes);
	}

	/* Copy entries
	 */
	for(;;)
	{
		IEC_UINT uRead = VMM_MAX_PATH;

		uRes = xxxRead(hMap, (IEC_DATA *)pBuffer, &uRead);
		if (uRes == ERR_FILE_EOF)
		{
			break;
		}

		if (uRes != OK)
		{
			xxxClose(hMap);
			xxxClose(hTemp);
			RETURN(uRes);
		}

		pBuffer[VMM_MAX_PATH] = '\0';

		if (OS_STRICMP(pBuffer, szFile) == 0)
		{
			continue;
		}

		uRes = xxxWrite(hTemp, (IEC_DATA *)pBuffer, VMM_MAX_PATH);
		if (uRes != OK)
		{
			xxxClose(hMap);
			xxxClose(hTemp);
			RETURN(uRes);
		}

	} /* while(1) */

	xxxClose(hMap);
	xxxClose(hTemp);

	/* Delete map file
	 */
	uRes = utilDeleteFile(pBuffer, uLen, fpGetDir, szDir, szMap);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* Rename file
	 */
	uRes = utilRenameFile(pBuffer, uLen, fpGetDir, szDir, VMM_FILE_TEMP, szMap);

	RETURN(uRes);
}

#endif /* RTS_CFG_STORE_FILES || RTS_CFG_FIRMWARE_DL */

/* ---------------------------------------------------------------------------- */
/**
 * utilCreateList
 *
 */
  #if defined(RTS_CFG_STORE_FILES) || defined(RTS_CFG_FIRMWARE_DL)

IEC_UINT utilCreateList(IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), 
						IEC_CHAR *szDir, IEC_CHAR *szMap, IEC_CHAR *szFile)
{
	IEC_UINT	uRes = OK;
	IEC_UDINT	hMap;

	/* Create/open map file
	 */
	uRes = utilOpenFile(&hMap, pBuffer, uLen, fpGetDir, szDir, szMap, FIO_MODE_APPEND);
	if(uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = xxxSeek(hMap, 0, FSK_SEEK_SET);
	if(uRes != OK)
	{
		xxxClose(hMap);
		RETURN(uRes);
	}

	/* Entry already exists?
	 */
	for(;;)
	{
		IEC_UINT uRead = VMM_MAX_PATH;

		uRes = xxxRead(hMap, (IEC_DATA *)pBuffer, &uRead);
		if (uRes == ERR_FILE_EOF)
		{
			break;
		}

		if (uRes != OK)
		{
			xxxClose(hMap);
			RETURN(uRes);
		}

		pBuffer[VMM_MAX_PATH] = '\0';

		if (OS_STRICMP(pBuffer, szFile) == 0)
		{
			xxxClose(hMap);
			RETURN(OK);
		}
	
	} /* while(1) */

	/* Add entry
	 */
	uRes = xxxSeek(hMap, 0, FSK_SEEK_END);
	if(uRes != OK)
	{
		xxxClose(hMap);
		RETURN(uRes);
	}

	OS_MEMCPY(pBuffer, szFile, VMM_MAX_PATH);

	uRes = xxxWrite(hMap, (IEC_DATA *)pBuffer, VMM_MAX_PATH);

	xxxClose(hMap);

	RETURN(uRes);
}

#endif /* RTS_CFG_STORE_FILES || RTS_CFG_FIRMWARE_DL */

/* ---------------------------------------------------------------------------- */
/**
 * utilCreatePath
 *
 */
#if defined(RTS_CFG_FILE_ACCESS) || defined(RTS_CFG_FILE_NATIVE)

IEC_UINT utilCreatePath(IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), 
						IEC_CHAR *szDir, IEC_CHAR *szFile)
{
	IEC_UINT uRes = fpGetDir(pBuffer, uLen);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = utilAppendPath(pBuffer, uLen, szDir, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (szFile != NULL)
	{
		uRes = utilAppendPath(pBuffer, uLen, szFile, FALSE);
	}

	RETURN(uRes);
}

#endif /* RTS_CFG_FILE_ACCESS || RTS_CFG_FILE_NATIVE */

/* ---------------------------------------------------------------------------- */
/**
 * utilPow2
 *
 */
IEC_UINT utilPow2(IEC_UINT n)
{
	IEC_UINT r = 2;
	IEC_UINT i;

	if (n == 0)
	{
		return 0;
	}

	for (i = 0; i < n - 1; i++)
	{
		r = (IEC_UINT)(r * 2u);
	}

	return r;
}

/* ---------------------------------------------------------------------------- */
/**
 * utilGetTimeDiffMS
 *
 */
IEC_UDINT utilGetTimeDiffMS(IEC_ULINT ullStart)
{
	return (IEC_UDINT)(((osGetTimeUS() - ullStart) + 500) / 1000);
}

/* ---------------------------------------------------------------------------- */
/**
 * utilGetTimeDiffUS
 *
 */
IEC_ULINT utilGetTimeDiffUS(IEC_ULINT ullStart)
{
	return osGetTimeUS() - ullStart;
}

/* ---------------------------------------------------------------------------- */
/**
 * utilGetTimeDiffMSEx
 *
 */
IEC_UDINT utilGetTimeDiffMSEx(IEC_ULINT ullStart)
{
	return (IEC_UDINT)(((osGetTimeUSEx() - ullStart) + 500) / 1000);
}

/* ---------------------------------------------------------------------------- */
/**
 * utilGetTimeDiffUSEx
 *
 */
IEC_ULINT utilGetTimeDiffUSEx(IEC_ULINT ullStart)
{
	return osGetTimeUSEx() - ullStart;
}

/* ---------------------------------------------------------------------------- */
