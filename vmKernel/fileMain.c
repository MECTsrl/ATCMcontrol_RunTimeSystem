
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
 * Filename: fileMain.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"fileMain.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_FILE_NATIVE)
  #include "osFile.h"
#endif

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

static IEC_UINT fileCreateDirTree(IEC_CHAR *szPath, IEC_BOOL bDirOnly, IEC_BOOL bFirst, IEC_UINT uMode);

/* ----  Implementations:	--------------------------------------------------- */


#if defined(RTS_CFG_FILE_NATIVE)

/* ---------------------------------------------------------------------------- */
/**
 * fileInitialize
 *
 */
IEC_UINT fileInitialize(STaskInfoVMM *pVMM)
{

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * fileOpen
 *
 */
IEC_UINT fileOpen(IEC_UDINT *hpFile, IEC_CHAR *szName, IEC_UINT uMode, IEC_BOOL bText)
{
	IEC_CHAR *szMode;
	IEC_CHAR *szText;
	IEC_CHAR szBuff[10];

	switch(uMode)
	{
		case FIO_MODE_READ:
			szMode = VMF_MODE_READ;
			break;
		case FIO_MODE_WRITE:
			szMode = VMF_MODE_WRITE;
			break;
		case FIO_MODE_RDWR:
			szMode = VMF_MODE_RDWR;
			break;
		case FIO_MODE_APPEND:
		default:
			szMode = VMF_MODE_APPEND;
			break;
	}

	szText = bText ? VMF_MODE_TEXT : VMF_MODE_BIN;

	utilFormatString(szBuff, "%s%s", szMode, szText);

	*hpFile = (IEC_UDINT)osfopen(szName, szBuff);

	RETURN((IEC_UINT)(*hpFile == 0 ? ERR_FILE_OPEN : OK));
}

/* ---------------------------------------------------------------------------- */
/**
 * fileClose
 *
 */
IEC_UINT fileClose(IEC_UDINT hFile)
{
	IEC_UINT uRes = (IEC_UINT)(osfclose((VMF_FILE)hFile) == VMF_RET_OK ? OK : ERR_FILE_CLOSE);
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * fileSeek
 *
 */
IEC_UINT fileSeek(IEC_UDINT hFile, IEC_UDINT ulOffset, IEC_INT iSet)
{
	IEC_INT  iOrigin;
	IEC_UINT uRes;

	switch (iSet)
	{
	case FSK_SEEK_CUR:
		iOrigin = VMF_SEEK_CUR;
		break;
	case FSK_SEEK_END:
		iOrigin = VMF_SEEK_END;
		break;
	case FSK_SEEK_SET:
		iOrigin = VMF_SEEK_SET;
		break;
	default:
		RETURN(ERR_INVALID_PARAM);
	}

	uRes = (IEC_UINT)(osfseek((VMF_FILE)hFile, ulOffset, iOrigin) == VMF_RET_OK ? OK : ERR_FILE_SEEK);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * fileRead
 *
 */
IEC_UINT fileRead(IEC_UDINT hFile, IEC_DATA *pData, IEC_UINT *upLen)
{
	IEC_UINT uRes = OK;
	IEC_UINT uLen = *upLen;

	if (hFile == 0)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	*upLen = (IEC_UINT)osfread(pData, sizeof(IEC_DATA), *upLen, (VMF_FILE)hFile);
	
	if (*upLen == 0 && uLen != 0)
	{
		if (osfeof((VMF_FILE)hFile) != 0)
		{
			return ERR_FILE_EOF;
		}

		uRes = ERR_FILE_READ;
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * fileReadLine
 *
 */
IEC_UINT fileReadLine(IEC_UDINT hFile, IEC_CHAR *szData, IEC_UINT uLen)
{
	IEC_UINT uRes = OK;

	if (hFile == 0)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	szData = osfgets(szData, uLen, (VMF_FILE)hFile);
	
	if (szData == 0)
	{
		if (osfeof((VMF_FILE)hFile) != 0)
		{
			return ERR_FILE_EOF;
		}

		uRes = ERR_FILE_READ;
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * fileWrite
 *
 */
IEC_UINT fileWrite(IEC_UDINT hFile, IEC_DATA *pData, IEC_UINT uLen)
{
	if (hFile == 0)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	if (osfwrite(pData, sizeof(IEC_DATA), uLen, (VMF_FILE)hFile) != uLen)
	{
		RETURN(ERR_FILE_WRITE);
	}
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * fileWriteLine
 *
 */
IEC_UINT fileWriteLine(IEC_UDINT hFile, IEC_CHAR *szData)
{
	if (hFile == 0)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	if (osfputs(szData, (VMF_FILE)hFile) < 0)
	{
		RETURN(ERR_FILE_WRITE);
	}
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * fileCreateDir
 *
 */
IEC_UINT fileCreateDir(IEC_CHAR *szPath, IEC_BOOL bDirOnly, IEC_UINT uMode)
{
	IEC_UINT uPos = 0;
	IEC_UINT uRes;

	for ( ; szPath[uPos] != '\0' && szPath[uPos] != ':' && szPath[uPos] != VMM_PATH_DELI; uPos++)
	{
		;
	}

	if (uPos == 1 && szPath[uPos] == ':')
	{
		if (szPath[0] >= 'A' && szPath[0] <= 'Z')
		{
			uRes = (IEC_UINT)(oschdrive(szPath[0] - 'A' + 1) == VMF_RET_OK ? OK : ERR_CHANGE_DRIVE);
		}
		else if (szPath[0] >= 'a' && szPath[0] <= 'z')
		{
			uRes = (IEC_UINT)(oschdrive(szPath[0] - 'a' + 1) == VMF_RET_OK ? OK : ERR_CHANGE_DRIVE);
		}
		else
		{
			RETURN(ERR_INVALID_DRIVE);
		}

		if (uRes != OK)
		{
			RETURN(uRes);
		}

		uPos++;
	}
	else
	{
		uPos = 0;
	}

	uRes = fileCreateDirTree(szPath + uPos, bDirOnly, TRUE, uMode);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * fileCreateDirTree
 *
 */
IEC_UINT fileCreateDirTree(IEC_CHAR *szPath, IEC_BOOL bDirOnly, IEC_BOOL bFirst, IEC_UINT uMode)
{
	IEC_CHAR pBuff[VMM_MAX_PATH + 1];
	IEC_UINT uPos	= 0;
	IEC_UINT uStart = 0;
	IEC_UINT uRes	= OK;
	IEC_UINT uOrgMode;

	if (szPath[uPos] == VMM_PATH_DELI)
	{
		uPos++;
	}

	if (szPath[uPos] == '\0')
	{
		uRes = OK;

		if (bFirst == TRUE)
		{
			/* Root directory
			 */
			pBuff[0] = VMM_PATH_DELI;
			pBuff[1] = '\0';

			uRes = (IEC_UINT)(oschdir(pBuff) == VMF_RET_OK ? OK : ERR_CHANGE_DIR);
		}

		RETURN(uRes);
	}

	uStart = (IEC_UINT)(bFirst == TRUE ? 0 : uPos);

	for ( ; szPath[uPos] != '\0' && szPath[uPos] != VMM_PATH_DELI; uPos++)
	{
		;
	}

	if (szPath[uPos] == '\0' && bDirOnly == FALSE)
	{
		RETURN(OK);
	}

	if (bFirst == TRUE && szPath[0] == VMM_PATH_DELI)
	{
		pBuff[0] = VMM_PATH_DELI;
		pBuff[1] = '\0';

		uRes = (IEC_UINT)(oschdir(pBuff) == VMF_RET_OK ? OK : ERR_CHANGE_DIR);

		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	memcpy(pBuff, szPath + uStart, uPos - uStart);
	pBuff[uPos - uStart] = '\0';

	switch(uMode)
	{
		case DIR_MODE_READ:
			uOrgMode = VMF_DIR_READ;
			break;
		case DIR_MODE_WRITE:
			uOrgMode = VMF_DIR_WRITE;
			break;
		default:
			uOrgMode = VMF_DIR_WRITE;
			break;
	}

	osmkdir(pBuff + (bFirst == TRUE && szPath[0] == VMM_PATH_DELI ? 1 : 0),uOrgMode); 

	uRes = (IEC_UINT)(oschdir(pBuff) == VMF_RET_OK ? OK : ERR_CHANGE_DIR);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	return fileCreateDirTree(szPath + uPos, bDirOnly, FALSE, uMode);
}

/* ---------------------------------------------------------------------------- */
/**
 * fileRename
 *
 */
IEC_UINT fileRename(IEC_CHAR *szFrom, IEC_CHAR *szTo)
{
	IEC_UINT uRes = (IEC_UINT)(osrename(szFrom, szTo) == VMF_RET_OK ? OK : ERR_FILE_RENAME);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * fileRemove
 *
 */
IEC_UINT fileRemove(IEC_CHAR *szFile)
{
	IEC_UINT uRes = OK;
	IEC_BOOL bExist;
	
	uRes = fileExist(szFile, &bExist);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (bExist == TRUE)
	{
		uRes =	(IEC_UINT)(osremove(szFile) == VMF_RET_OK ? OK : ERR_FILE_REMOVE);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * fileExist
 *
 */
IEC_UINT fileExist(IEC_CHAR *szFile, IEC_BOOL *bpExist)
{
	struct osstructstat status;
		
	*bpExist = (IEC_BOOL)(osstat(szFile, &status) == 0 ? TRUE : FALSE);

	RETURN(OK);
}

#endif /* RTS_CFG_FILE_NATIVE */

/* ---------------------------------------------------------------------------- */
