
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
 * Filename: libFile.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"libFile.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_FILE_LIB)

#include "osFile.h"
#include "libFile.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

static fa_fd_map	*g_pOpenFiles = NULL;

/* ----  Local Functions:	--------------------------------------------------- */

static IEC_UINT get_fd(FA_SYNC_FILE *pFile, fa_fd_pair **ppFD);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * flCleanUp
 *
 */
IEC_UINT flCleanUp(void)
{
	fa_fd_pair *pFD;

	if (g_pOpenFiles == NULL)
	{
		RETURN(OK);
	}
	
	OS_BEGIN_CRITICAL_SECTION(SEM_FILE_LIBRARY)
	{
		while (g_pOpenFiles != NULL)
		{
			pFD = g_pOpenFiles;
			g_pOpenFiles = pFD->pNext;

			if(pFD->hFile)
			{
				xxxClose(pFD->hFile);
			}
			
			osFree((IEC_DATA **)&pFD);
		}

	} OS_END_CRITICAL_SECTION(SEM_FILE_LIBRARY);

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * get_fd
 *
 */
static IEC_UINT get_fd(FA_SYNC_FILE *pFile, fa_fd_pair **ppFD)
{
	*ppFD = NULL;

	OS_BEGIN_CRITICAL_SECTION(SEM_FILE_LIBRARY)
	{
		for(*ppFD = g_pOpenFiles; *ppFD != NULL; *ppFD = (*ppFD)->pNext)
		{
			if (pFile == (*ppFD)->pFile)
			{
				break;
			}
		}

		if (*ppFD == NULL)
		{
			*ppFD 			= (fa_fd_pair *)osMalloc(sizeof(fa_fd_pair));
			(*ppFD)->pFile	= pFile;
			(*ppFD)->hFile	= 0;
			(*ppFD)->pNext	= g_pOpenFiles;
			g_pOpenFiles	= *ppFD;
		}

	} OS_END_CRITICAL_SECTION(SEM_FILE_LIBRARY)

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * 160 - fa_sync_openFile
 *
 */
void fa_sync_openFile(STDLIBFUNCALL)
{
	FA_SYNC_OPENFILE_PAR OS_SPTR *pPar	= (FA_SYNC_OPENFILE_PAR OS_SPTR *)pIN;
	FA_SYNC_FILE OS_DPTR *pFile 		= pPar->file;

	fa_fd_pair	*pFD;
	IEC_CHAR	szFile[VMM_MAX_IEC_STRLEN + 1];
	IEC_UINT	uMode;
	IEC_UDINT	uRes = OK;

	utilIecToAnsi((IEC_STRING OS_LPTR *)&pFile->sName, szFile);
	
	pPar->bRet = TRUE;

	if (pFile->bOpen)
	{
		pFile->ulError = CFA_FileAlreadyOpen;
		return;
	}
	
	switch(pFile->ulMode)
	{
		case CFA_READ:
			uMode = FIO_MODE_READ;
			break;
		case CFA_READ_WRITE:
			uMode = FIO_MODE_WRITE;
			break;
		case CFA_APPEND:
			uMode = FIO_MODE_APPEND;
			break;
		default:
			pFile->ulError = CFA_InvalidArgument;
			return;
	}		
	
	if (get_fd(pFile, &pFD) != OK)
	{
		pFile->ulError = CFA_InternalError;
		return;
	}

	if (pFD->hFile != 0)
	{
		xxxClose(pFD->hFile);
	}

	uRes = xxxOpen(&pFD->hFile, szFile, uMode, FALSE);
	if (uRes != OK)
	{
		pFile->ulError = CFA_CommonError;
		return;
	}
	
	pFile->bOpen	= TRUE;
	pFile->lPos 	= pFile->ulMode != CFA_APPEND ? CFA_BOF : CFA_EOF;
	pFile->ulError	= CFA_NoError;

	pPar->bRet		= FALSE;
}


/* ---------------------------------------------------------------------------- */
/**
 * 161 - fa_sync_readFile
 *
 */
void fa_sync_readFile(STDLIBFUNCALL)
{
	FA_SYNC_READFILE_PAR OS_SPTR *pPar	= (FA_SYNC_READFILE_PAR OS_SPTR *)pIN;
	FA_SYNC_FILE OS_DPTR *pFile 		= pPar->file;

	fa_fd_pair	*pFD;

	IEC_UINT uRes = OK;
	IEC_UINT uLen = (IEC_UINT)*pPar->ulLength;
	
	pPar->bRet		= TRUE;
	*pPar->ulLength = 0;
	pFile->ulError	= CFA_NoError;
	
	if (uLen > pPar->pData->MaxLen)
	{
		pFile->ulError = CFA_InvalidArgument;	
		return; 
	}
	
	if (get_fd(pFile, &pFD) != OK)
	{
		pFile->ulError = CFA_InternalError;
		return;
	}

	if(pFD == NULL || pFD->hFile == 0)
	{
		pFile->ulError = CFA_FileNotOpenError;
		return;
	}

	if(pFile->lPos < -1)
	{
		pFile->lPos = CFA_BOF;
		xxxSeek(pFD->hFile, 0, FSK_SEEK_SET);
	}
	else if(pFile->lPos == CFA_EOF)
	{
		pPar->bRet = FALSE;
		return;
	}
	else			
	{
		if(xxxSeek(pFD->hFile, pFile->lPos, FSK_SEEK_SET))
		{
			pFile->lPos = CFA_EOF;
			pPar->bRet	= FALSE;
			return;
		}
	}

	uRes = xxxRead(pFD->hFile, (IEC_DATA*)pPar->pData->Contents, &uLen);
	
	if (uRes != OK && uRes != ERR_FILE_EOF)
	{
		pFile->ulError = CFA_CommonError;
		return;
	}

	pPar->pData->CurLen = (IEC_STRLEN)uLen;
		
	if (uRes == ERR_FILE_EOF || osfeof((VMF_FILE)pFD->hFile) != 0)
	{
		pFile->lPos = CFA_EOF;
	}
	else
	{
		pFile->lPos += uLen;
	}
		
	*pPar->ulLength = uLen;
	pPar->bRet		= FALSE;
}


/* ---------------------------------------------------------------------------- */
/**
 * 162 - fa_sync_renameFile
 *
 */
void fa_sync_renameFile(STDLIBFUNCALL)
{
	FA_SYNC_RENAMEFILE_PAR OS_SPTR *pPar	= (FA_SYNC_RENAMEFILE_PAR OS_SPTR *)pIN;
	FA_SYNC_FILE OS_DPTR *pFile 		= pPar->file;

	IEC_CHAR szOld[VMM_MAX_IEC_STRLEN + 1];
	IEC_CHAR szNew[VMM_MAX_IEC_STRLEN + 1];
	
	utilIecToAnsi((IEC_STRING OS_LPTR *)&pFile->sName, szOld);
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPar->newName, szNew);
	
	pPar->bRet = TRUE;
	if(xxxRename(szOld, szNew) != OK)
	{
		pFile->ulError = CFA_CommonError;
		return;
	}
	
	utilCopyIecString((IEC_STRING OS_DPTR *)&pFile->sName, pPar->newName);

	pPar->bRet		= FALSE;
	pFile->ulError	= CFA_NoError;
}


/* ---------------------------------------------------------------------------- */
/**
 * 163 - fa_sync_writeFile
 *
 */
void fa_sync_writeFile(STDLIBFUNCALL)
{
	FA_SYNC_WRITEFILE_PAR OS_SPTR *pPar = (FA_SYNC_WRITEFILE_PAR OS_SPTR *)pIN;
	FA_SYNC_FILE OS_DPTR *pFile 		= pPar->file;

	fa_fd_pair	*pFD;

	IEC_UINT uRes;
	
	pPar->bRet = TRUE;
	
	if (get_fd(pFile, &pFD) != OK)
	{
		pFile->ulError = CFA_InternalError;
		return;
	}

	if(pFD == NULL || pFD->hFile == 0)
	{
		pFile->ulError = CFA_FileNotOpenError;
		return;
	}

	if(pFile->lPos < -1)
	{
		pFile->lPos = CFA_BOF;
		xxxSeek(pFD->hFile, 0, FSK_SEEK_SET);
	}
	else if(pFile->lPos == CFA_EOF)
	{
		xxxSeek(pFD->hFile, 0, FSK_SEEK_END);
	}
	else
	{
		if(xxxSeek(pFD->hFile, pFile->lPos, FSK_SEEK_SET))
		{
			pFile->lPos = CFA_EOF;
			xxxSeek(pFD->hFile, 0, FSK_SEEK_END);
		}
	}

	uRes = xxxWrite(pFD->hFile, (IEC_DATA*)pPar->pData->Contents, pPar->pData->CurLen);

	if (uRes != OK)
	{
		pFile->ulError = CFA_CommonError;
		return;
	}
	
	if (pFile->lPos != CFA_EOF)
	{
		if (osfeof((VMF_FILE)pFD->hFile) != 0)
		{
			pFile->lPos = CFA_EOF;
		}
		else
		{
			pFile->lPos += pPar->pData->CurLen;
		}
	}

	pPar->bRet		= FALSE;
	pFile->ulError	= CFA_NoError;
}


/* ---------------------------------------------------------------------------- */
/**
 * 164 - fa_sync_closeFile
 *
 */
void fa_sync_closeFile(STDLIBFUNCALL)
{
	FA_SYNC_CLOSEFILE_PAR OS_SPTR *pPar = (FA_SYNC_CLOSEFILE_PAR OS_SPTR *)pIN;
	FA_SYNC_FILE OS_DPTR *pFile 		= pPar->file;

	fa_fd_pair	*pFD;
	IEC_CHAR	szFile[VMM_MAX_IEC_STRLEN + 1];
	
	pPar->bRet = TRUE;
	utilIecToAnsi((IEC_STRING OS_LPTR *)&pFile->sName, szFile);
	
	if (get_fd(pFile, &pFD) != OK)
	{
		pFile->ulError = CFA_InternalError;
		return;
	}
	
	if (pFD && pFD->hFile)
	{
		if(xxxClose(pFD->hFile))
		{
			pFile->ulError = CFA_CommonError;
			return;
		}
		else
		{
			pFD->hFile = 0;
		}
	}

	pFile->bOpen	= FALSE;
	pFile->ulError	= CFA_NoError;

	pPar->bRet		= FALSE;
}


/* ---------------------------------------------------------------------------- */
/**
 * 165 - fa_sync_createDirectory
 *
 */
void fa_sync_createDirectory(STDLIBFUNCALL)
{
	FA_SYNC_CREATEDIRECTORY_PAR OS_SPTR *pPar	= (FA_SYNC_CREATEDIRECTORY_PAR OS_SPTR *)pIN;
	FA_SYNC_FILE OS_DPTR *pFile 				= pPar->file;

	IEC_CHAR	szFile[VMM_MAX_IEC_STRLEN + 1];
	IEC_UINT	uMode;

	utilIecToAnsi((IEC_STRING OS_LPTR *)&pFile->sName, szFile);
	
	switch(pFile->ulMode)
	{
		case CFA_READ:
			uMode = DIR_MODE_READ;
			break;
		case CFA_READ_WRITE:
			uMode = DIR_MODE_WRITE;
			break;
		case CFA_APPEND:
		default:
			pPar->bRet	   = TRUE;
			pFile->ulError = CFA_InvalidArgument;
			return;
	}		
	
	if(xxxCreateDir(szFile, TRUE, uMode) != OK)
	{
		pPar->bRet		= TRUE;
		pFile->ulError	= CFA_CommonError;
		return; 	
	}
	
	pFile->ulError	= CFA_NoError;
	pPar->bRet		= FALSE;
}


/* ---------------------------------------------------------------------------- */
/**
 * 166 - fa_sync_deleteFile
 *
 */
void fa_sync_deleteFile(STDLIBFUNCALL)
{
	FA_SYNC_DELETEFILE_PAR OS_SPTR *pPar	= (FA_SYNC_DELETEFILE_PAR OS_SPTR *)pIN;
	FA_SYNC_FILE OS_DPTR *pFile 			= pPar->file;

	IEC_CHAR szFile[VMM_MAX_IEC_STRLEN + 1];

	utilIecToAnsi((IEC_STRING OS_DPTR *)&pFile->sName, szFile);
	
	if (xxxRemove(szFile) != OK)
	{
		pPar->bRet		= TRUE;
		pFile->ulError	= CFA_CommonError;
		return; 	
	}
	
	pFile->ulError	= CFA_NoError;
	pPar->bRet		= FALSE;
}


/* ---------------------------------------------------------------------------- */
/**
 * 167 - fa_sync_existsFile
 *
 */
void fa_sync_existsFile(STDLIBFUNCALL)
{
	FA_SYNC_EXISTSFILE_PAR OS_SPTR *pPar	= (FA_SYNC_EXISTSFILE_PAR OS_SPTR *)pIN;
	FA_SYNC_FILE OS_DPTR *pFile 			= pPar->file;

	IEC_CHAR szFile[VMM_MAX_IEC_STRLEN + 1];

	struct osstructstat 	status;
	
	utilIecToAnsi((IEC_STRING OS_DPTR *)&pFile->sName, szFile);
	
	if(osstat(szFile, &status))
	{
		pFile->ulError	= CFA_NoError;
		pPar->bRet		= FALSE;
		return;
	}
	
	pFile->ulError	= CFA_NoError;
	pPar->bRet		= TRUE;
}


/* ---------------------------------------------------------------------------- */
/**
 * 168 - fa_sync_getSize
 *
 */
void fa_sync_getSize(STDLIBFUNCALL)
{
	FA_SYNC_GETSIZE_PAR OS_SPTR *pPar	= (FA_SYNC_GETSIZE_PAR OS_SPTR *)pIN;
	FA_SYNC_FILE OS_DPTR *pFile 		= pPar->file;

	IEC_CHAR szFile[VMM_MAX_IEC_STRLEN + 1];

	struct osstructstat 	status;
	
	utilIecToAnsi((IEC_STRING OS_DPTR *)&pFile->sName, szFile);
	
	if(osstat(szFile, &status))
	{
		*pPar->lpSize	= -1;
		pFile->ulError	= CFA_CommonError;
		pPar->bRet		= TRUE;
		return; 	
	}
	
	*pPar->lpSize	= status.st_size;
	pPar->bRet		= FALSE;
	pFile->ulError	= CFA_NoError;
}


/* ---------------------------------------------------------------------------- */
/**
 * 169 - fa_get_diskFreeSpace
 *
 */
void fa_get_diskFreeSpace(STDLIBFUNCALL)
{
	FA_GET_DISKFREESPACE_PAR OS_SPTR *pPar	= (FA_GET_DISKFREESPACE_PAR OS_SPTR *)pIN;

	IEC_UDINT ulFree;
	IEC_CHAR  szFile[VMM_MAX_IEC_STRLEN + 1];
	
	utilIecToAnsi(pPar->sPath, szFile);

	if(osGetFreeDiskSpace(szFile, &ulFree) != OK)
	{
		*pPar->lpFree = -1;
		pPar->ulRet   = CFA_CommonError;
		return; 	
	}

	*pPar->lpFree = ulFree;
	pPar->ulRet   = CFA_NoError;
}


/* ---------------------------------------------------------------------------- */
/**
 * 170 - fa_error_string
 *
 */
void fa_error_string(STDLIBFUNCALL)
{
	FA_ERROR_STRING_PAR OS_SPTR *pPar = (FA_ERROR_STRING_PAR OS_SPTR *)pIN;

	IEC_CHAR *szTemp;

	switch(pPar->lError)
	{
		case CFA_NoError:
			szTemp = CFA_NoErrorStr;
			break;
		case CFA_CommonError:
			szTemp = osstrerror;
			break;
		case CFA_InternalError:
			szTemp = CFA_InternalErrorStr;
			break;
		case CFA_InvalidArgument:
			szTemp = CFA_InvalidArgumentStr;
			break;
		case CFA_FileNotOpenError:
			szTemp = CFA_FileNotOpenErrorStr;
			break;
		case CFA_FileAlreadyOpen:
			szTemp = CFA_FileAlreadyOpenStr;
			break;
		default:
			szTemp = "Unknown error code";
	}
	
	utilAnsiToIec(szTemp, pPar->sError);
}


/* ---------------------------------------------------------------------------- */
/**
 * 171 - fa_sync_arrayReadFile
 *
 */
void fa_sync_arrayReadFile(STDLIBFUNCALL)
{
	FA_SYNC_ARRAYREADFILE_PAR OS_SPTR *pPar = (FA_SYNC_ARRAYREADFILE_PAR OS_SPTR *)pIN;
	FA_SYNC_FILE OS_DPTR *pFile 			= pPar->file;

	fa_fd_pair	*pFD;

	IEC_UINT uRes = OK;
	IEC_UINT uLen = (IEC_UINT)*pPar->ulLength;
	
	pPar->bRet			= TRUE;
	*pPar->ulLength 	= 0;
	pFile->ulError		= CFA_NoError;
	
	if (uLen > GET_AR_LEN(pPar->pData) - pPar->ulPos || pPar->ulPos < 0)
	{
		libSetException(pVM, EXCEPT_ARRAY_RANGE);
		pFile->ulError = CFA_InvalidArgument;	
		return; 
	}
	
	if (get_fd(pFile, &pFD) != OK)
	{
		pFile->ulError = CFA_InternalError;
		return;
	}

	if(pFD == NULL || pFD->hFile == 0)
	{
		pFile->ulError = CFA_FileNotOpenError;
		return;
	}

	if(pFile->lPos < -1)
	{
		pFile->lPos = CFA_BOF;
		xxxSeek(pFD->hFile, 0, FSK_SEEK_SET);
	}
	else if(pFile->lPos == CFA_EOF)
	{
		pPar->bRet = FALSE;
		return;
	}
	else			
	{
		if(xxxSeek(pFD->hFile, pFile->lPos, FSK_SEEK_SET))
		{
			pFile->lPos = CFA_EOF;
			pPar->bRet	= FALSE;
			return;
		}
	}

	uRes = xxxRead(pFD->hFile, GET_AR_DAT(pPar->pData) + pPar->ulPos, &uLen);
	
	if (uRes != OK && uRes != ERR_FILE_EOF)
	{
		pFile->ulError = CFA_CommonError;
		return;
	}
		
	if (uRes == ERR_FILE_EOF || osfeof((VMF_FILE)pFD->hFile) != 0)
	{
		pFile->lPos = CFA_EOF;
	}
	else
	{
		pFile->lPos += uLen;
	}
		
	*pPar->ulLength = uLen;
	pPar->bRet		= FALSE;
}


/* ---------------------------------------------------------------------------- */
/**
 * 172 - fa_sync_arrayWriteFile
 *
 */
void fa_sync_arrayWriteFile(STDLIBFUNCALL)
{
	FA_SYNC_ARRAYWRITEFILE_PAR OS_SPTR *pPar	= (FA_SYNC_ARRAYWRITEFILE_PAR OS_SPTR *)pIN;
	FA_SYNC_FILE OS_DPTR *pFile 				= pPar->file;

	fa_fd_pair	*pFD;

	IEC_UINT uRes;
	
	pPar->bRet = TRUE;
	
	if (get_fd(pFile, &pFD) != OK)
	{
		pFile->ulError = CFA_InternalError;
		return;
	}

	if (pPar->ulLen > GET_AR_LEN(pPar->pData) - pPar->ulPos || pPar->ulPos < 0 ||  pPar->ulLen < 0)
	{
		libSetException(pVM, EXCEPT_ARRAY_RANGE);
		pFile->ulError = CFA_InvalidArgument;	
		return; 
	}

	if(pFD == NULL || pFD->hFile == 0)
	{
		pFile->ulError = CFA_FileNotOpenError;
		return;
	}
	
	if(pFile->lPos < -1)
	{
		pFile->lPos = CFA_BOF;
		xxxSeek(pFD->hFile, 0, FSK_SEEK_SET);
	}
	else if(pFile->lPos == CFA_EOF)
	{
		xxxSeek(pFD->hFile, 0, FSK_SEEK_END);
	}
	else
	{
		if(xxxSeek(pFD->hFile, pFile->lPos, FSK_SEEK_SET))
		{
			pFile->lPos = CFA_EOF;
			xxxSeek(pFD->hFile, 0, FSK_SEEK_END);
		}
	}

	uRes = xxxWrite(pFD->hFile, GET_AR_DAT(pPar->pData) + pPar->ulPos, (IEC_UINT)pPar->ulLen);

	if (uRes != OK)
	{
		pFile->ulError = CFA_CommonError;
		return;
	}
	
	if (pFile->lPos != CFA_EOF)
	{
		if (osfeof((VMF_FILE)pFD->hFile) != 0)
		{
			pFile->lPos = CFA_EOF;
		}
		else
		{
			pFile->lPos += pPar->ulLen;
		}
	}

	pPar->bRet = FALSE;
	pFile->ulError = CFA_NoError;
}


/* ---------------------------------------------------------------------------- */
/**
 * 173 - fa_flush
 *
 */
void fa_flush(STDLIBFUNCALL)
{
	FA_FLUSH_PAR* pPar = (FA_FLUSH_PAR*)pIN;
	
	pPar->ulRet = osfflush(NULL) == EOF ? CFA_CommonError : CFA_NoError;
}

#endif	/* RTS_CFG_FILE_LIB */ 
