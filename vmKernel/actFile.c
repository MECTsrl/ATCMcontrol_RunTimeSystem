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
 * Filename: actFile.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"actFile.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/**
 * cmdSaveProject
 */
#if defined(RTS_CFG_STORE_PROJECT)

IEC_UINT cmdSaveProject(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SDLProj *pPRJ = &pVMM->DLB.PRJ;

	IEC_UINT uRes = OK;

	if (pBlock == 0)
	{
		actCloseFile(&pPRJ->hSave);

		RETURN(OK);
	}

	if (pBlock->uBlock == 1)
	{
		/* Delete old project file first
		 */
		utilDeleteFile((IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), 
				osGetProjectDir, VMM_DIR_PROJECT, VMM_FILE_PROJECT);

		/* Create temp file
		 */
		uRes = utilCreateFile(&pPRJ->hSave, (IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), 
				osGetProjectDir, VMM_DIR_PROJECT, VMM_FILE_TEMP);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	if (pPRJ->hSave == 0)
	{
		RETURN(ERR_FILE_SYNC);
	}

	uRes = xxxWrite(pPRJ->hSave, pBlock->CMD.pData, pBlock->uLen);

	if (pBlock->byLast == TRUE)
	{
		actCloseFile(&pPRJ->hSave);

		/* Rename the file
		 */
		uRes = utilRenameFile((IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), 
				osGetProjectDir, VMM_DIR_PROJECT, VMM_FILE_TEMP, VMM_FILE_PROJECT);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resSaveProject
 */
IEC_UINT resSaveProject(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	
	RETURN(OK);
}

#endif /* RTS_CFG_STORE_PROJECT */


/* ---------------------------------------------------------------------------- */
/**
 * cmdLoadProject
 */
#if defined(RTS_CFG_STORE_PROJECT)

IEC_UINT cmdLoadProject(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SDLProj *pPRJ = &pVMM->DLB.PRJ;

	IEC_UINT uRes = OK;

	if (pBlock == 0)
	{
		actCloseFile(&pPRJ->hLoad);

		RETURN(OK);
	}

	if (pBlock->uBlock == 1)
	{
		uRes = utilOpenFile(&pPRJ->hLoad, (IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), 
				osGetProjectDir, VMM_DIR_PROJECT, VMM_FILE_PROJECT, FIO_MODE_READ);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resLoadProject
 */
IEC_UINT resLoadProject(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SDLProj *pPRJ = &pVMM->DLB.PRJ;

	IEC_UINT uRes = OK;

	if (pBlock == 0)
	{
		RETURN(OK);
	}

	pBlock->uLen = MAX_DATA;

	uRes = xxxRead(pPRJ->hLoad, pBlock->CMD.pData, &pBlock->uLen);
	
	if (uRes != OK && uRes != ERR_FILE_EOF)
	{
		pBlock->uLen = 0;
		actCloseFile(&pPRJ->hLoad);

		RETURN(uRes);
	}

	pBlock->byLast = (IEC_BOOL)(pBlock->uLen < MAX_DATA || uRes == ERR_FILE_EOF);

	if (pBlock->byLast)
	{
		actCloseFile(&pPRJ->hLoad);
	}
	
	RETURN(OK);
}

#endif /* RTS_CFG_STORE_PROJECT */


/* ---------------------------------------------------------------------------- */
/**
 * cmdSaveFile
 */
#if defined(RTS_CFG_STORE_FILES)

IEC_UINT cmdSaveFile(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	uRes = actCmdSaveFile(pVMM, pBlock, CMD_SAVE_FILE);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resSaveFile
 */
IEC_UINT resSaveFile(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	uRes = actResSaveFile(pVMM, pBlock, CMD_SAVE_FILE);

	RETURN(uRes);
}

#endif /* RTS_CFG_STORE_FILES */


/* ---------------------------------------------------------------------------- */
/**
 * cmdLoadFile
 */

#if defined(RTS_CFG_STORE_FILES)

IEC_UINT cmdLoadFile(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SDLData *pDAT = &pVMM->DLB.DAT;

	IEC_UINT uRes = OK;

	if (pBlock == 0)
	{
		actCloseFile(&pDAT->hLoad);

		RETURN(OK);
	}

	if (pBlock->uBlock == 1)
	{
		/* Check data file name
		 */
		if (utilCheckString((IEC_CHAR*)pBlock->CMD.pData, (IEC_UINT)(VMM_MAX_PATH > pBlock->uLen ? VMM_MAX_PATH : pBlock->uLen)) == FALSE)
		{
			RETURN(ERR_INVALID_PARAM);
		}

		uRes = osCheckFilePath((IEC_CHAR*)pBlock->CMD.pData, CMD_LOAD_FILE);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		uRes = utilOpenFile(&pDAT->hLoad, (IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), 
				osGetFileDir, VMM_DIR_FILE, (IEC_CHAR*)pBlock->CMD.pData, FIO_MODE_READ);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resLoadFile
 */
IEC_UINT resLoadFile(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SDLData *pDAT = &pVMM->DLB.DAT;

	IEC_UINT uRes = OK;

	if (pBlock == 0)
	{
		RETURN(OK);
	}

	if (pDAT->hLoad == 0)
	{
		RETURN(ERR_FILE_SYNC);
	}

	pBlock->uLen = MAX_DATA;

	uRes = xxxRead(pDAT->hLoad, pBlock->CMD.pData, &pBlock->uLen);
	
	if (uRes != OK && uRes != ERR_FILE_EOF)
	{
		pBlock->uLen = 0;

		actCloseFile(&pDAT->hLoad);

		RETURN(uRes);
	}

	pBlock->byLast = (IEC_BOOL)(pBlock->uLen < MAX_DATA || uRes == ERR_FILE_EOF);

	if (pBlock->byLast)
	{
		actCloseFile(&pDAT->hLoad);
	}
	
	RETURN(OK);
}

#endif /* RTS_CFG_STORE_FILES */


/* ---------------------------------------------------------------------------- */
/**
 * cmdDelFile
 */
#if defined(RTS_CFG_STORE_FILES)

IEC_UINT cmdDelFile(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	if (pBlock->uBlock != 1)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	/* Check data file name
	 */
	if (utilCheckString((IEC_CHAR*)pBlock->CMD.pData, (IEC_UINT)(VMM_MAX_PATH > pBlock->uLen ? VMM_MAX_PATH : pBlock->uLen)) == FALSE)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	uRes = osCheckFilePath((IEC_CHAR*)pBlock->CMD.pData, CMD_DEL_FILE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = utilDeleteList((IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), osGetFileDir, VMM_DIR_FILE, VMM_FILE_FILE_MAP, (IEC_CHAR*)pBlock->CMD.pData);
	if (uRes != OK && uRes != ERR_FILE_NOT_EXIST)
	{
		RETURN(uRes);
	}

	utilDeleteFile((IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), osGetFileDir, VMM_DIR_FILE, (IEC_CHAR*)pBlock->CMD.pData);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDelFile
 */
IEC_UINT resDelFile(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	
	RETURN(OK);
}

#endif /* RTS_CFG_STORE_FILES */


/* ---------------------------------------------------------------------------- */
/**
 * cmdDir
 */
#if defined(RTS_CFG_STORE_FILES)

IEC_UINT cmdDir(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock == 0)
	{
		actCloseFile(&pVMM->DLB.DAT.hDir);
		RETURN(OK);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDir
 */
IEC_UINT resDir(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SDLData *pDAT = &pVMM->DLB.DAT;

	IEC_UINT uRes = OK;

	if (pBlock == 0)
	{
		RETURN(OK);
	}

	if (pBlock->uBlock == 1)
	{
		uRes = utilOpenFile(&pDAT->hDir, (IEC_CHAR*)pVMM->pBuffer, sizeof((IEC_CHAR*)pVMM->pBuffer), 
				osGetFileDir, VMM_DIR_FILE, VMM_FILE_FILE_MAP, FIO_MODE_READ);
		if (uRes != OK)
		{
			RETURN(OK);
		}
	}

	if (pDAT->hDir == 0)
	{
		RETURN(ERR_FILE_SYNC);
	}

	pBlock->uLen = MAX_DATA;

	uRes = xxxRead(pDAT->hDir, pBlock->CMD.pData, &pBlock->uLen);
	
	if (uRes != OK && uRes != ERR_FILE_EOF)
	{
		pBlock->uLen = 0;

		actCloseFile(&pDAT->hDir);

		RETURN(uRes);
	}

	if (pBlock->uLen < MAX_DATA || uRes == ERR_FILE_EOF)
	{
		/* Create the terminating zero byte
		 */
		if (pBlock->uLen < MAX_DATA)
		{
			pBlock->CMD.pData[pBlock->uLen] = '\0';
			pBlock->uLen++;
		}
		else
		{
			uRes = OK;
		}
	}

	pBlock->byLast = (IEC_BOOL)(pBlock->uLen < MAX_DATA || uRes == ERR_FILE_EOF);

	if (pBlock->byLast)
	{
		actCloseFile(&pDAT->hDir);
	}
	
	RETURN(OK);
}

#endif /* RTS_CFG_STORE_FILES */


/* ---------------------------------------------------------------------------- */
/**
 * actCmdSaveFile
 */
#if defined(RTS_CFG_STORE_FILES)

IEC_UINT actCmdSaveFile(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uCmd)
{
	SDLData *pDAT = &pVMM->DLB.DAT;

	IEC_UINT uRes = OK;
	IEC_UINT uOff = 0;

	if (pBlock == 0)
	{
		actCloseFile(&pDAT->hSave);

		RETURN(OK);
	}

	if (pBlock->uBlock == 1)
	{
		IEC_CHAR *szDir  = NULL;
		IEC_CHAR *szMap  = NULL;
		IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT) = NULL;

		/* Check data file name
		 */
		if (utilCheckString((IEC_CHAR*)pBlock->CMD.pData, (IEC_UINT)(VMM_MAX_PATH > pBlock->uLen ? VMM_MAX_PATH : pBlock->uLen)) == FALSE)
		{
			RETURN(ERR_INVALID_PARAM);
		}

		uRes = osCheckFilePath((IEC_CHAR*)pBlock->CMD.pData, uCmd);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		switch(uCmd)
		{
		  #if defined(RTS_CFG_STORE_FILES)			
			case CMD_SAVE_FILE:
				szDir	 = VMM_DIR_FILE;
				szMap	 = VMM_FILE_FILE_MAP;
				fpGetDir = osGetFileDir;
				break;
		  #endif

			default:
				uRes = ERR_INVALID_COMMAND;
				break;
		}

		if (uRes != OK)
		{
			RETURN(uRes);
		}

		/* Delete old file first
		 */
		uRes = utilDeleteList((IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), 
				fpGetDir, szDir, szMap, (IEC_CHAR*)pBlock->CMD.pData);
		if (uRes != OK && uRes != ERR_FILE_NOT_EXIST)
		{
			RETURN(uRes);
		}

		utilDeleteFile((IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), 
				fpGetDir, szDir, (IEC_CHAR*)pBlock->CMD.pData);

		/* Create temporary file
		 */
		uRes = utilCreateFile(&pDAT->hSave, (IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), 
				fpGetDir, szDir, VMM_FILE_TEMP);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		/* Offset to the data
		 */
		uOff = (IEC_UINT)(OS_STRLEN((IEC_CHAR*)pBlock->CMD.pData) + 1);

		OS_MEMCPY(pVMM->DLB.pRecv, pBlock->CMD.pData, uOff);
	}

	if (pDAT->hSave == 0)
	{
		/* File closed by a synchronous transfer from another client
		 */
		RETURN(ERR_FILE_SYNC);
	}

	uRes = xxxWrite(pDAT->hSave, pBlock->CMD.pData + uOff, (IEC_UINT)(pBlock->uLen - uOff));
	if (uRes != OK)
	{
		actCloseFile(&pDAT->hSave);
	}

	if (pBlock->byLast == TRUE)
	{
		actCloseFile(&pDAT->hSave);
	}

	RETURN(uRes);
}

#endif /* RTS_CFG_STORE_FILES */


/* ---------------------------------------------------------------------------- */
/**
 * actResSaveFile
 */
#if defined(RTS_CFG_STORE_FILES)

IEC_UINT actResSaveFile(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uCmd)
{
	IEC_UINT uRes = OK;
	
	IEC_CHAR *szDir  = NULL;
	IEC_CHAR *szMap  = NULL;
	IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT) = NULL;

	if (pBlock == 0)
	{
		RETURN(OK);
	}

	switch(uCmd)
	{
	  #if defined(RTS_CFG_STORE_FILES)			
		case CMD_SAVE_FILE:
			szDir	 = VMM_DIR_FILE;
			szMap	 = VMM_FILE_FILE_MAP;
			fpGetDir = osGetFileDir;
			break;
	  #endif

		default:
			uRes = ERR_INVALID_COMMAND;
			break;
	}

	/* Rename the file
	 */
	uRes = utilRenameFile((IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), 
			fpGetDir, szDir, VMM_FILE_TEMP, (IEC_CHAR*)pVMM->DLB.pRecv);
	if (uRes != OK)
	{
		RETURN(OK);
	}

	/* Create directory entry
	 */
	uRes = utilCreateList((IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), 
			fpGetDir, szDir, szMap, (IEC_CHAR*)pVMM->DLB.pRecv);
	
	RETURN(uRes);

}

#endif /* RTS_CFG_STORE_FILES */

/* ---------------------------------------------------------------------------- */

