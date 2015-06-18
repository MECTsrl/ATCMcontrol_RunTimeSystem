
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
 * Filename: osFlash.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"osFlash.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_FLASH)

#include "osFile.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define FLASH_FILE_CLOSE(fh)	xxxClose((fh)); \
								(fh)=(IEC_UDINT)VMF_INVALID_HANDLE;

/* ----  Global Variables:	 -------------------------------------------------- */

static IEC_UDINT g_hFlash		= (IEC_UDINT)VMF_INVALID_HANDLE;
static IEC_UDINT g_ulCount		= 0;

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * osClearFlash
 *
 * Clears the flash memory. 
 * If the deletion of the flashed IEC program lasts longer, the function should 
 * return even if the process is still not finished in order to avoid a 
 * communication time out in the FarosPLC Engineering. If necessary, the deletion 
 * process must be divided into different parts. Setting upRetry count unequal to 
 * zero, instructs the FarosPLC Engineering to poll/retry again after approximately 
 * one second.
 *
 * @param	upRetry 	IN:  Retry count from 4C. (-> 0 for reset.)
 *						OUT: 0 if ready, else != 0 - 4C will retry
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osClearFlash(IEC_UINT *upRetry)
{	
	IEC_BOOL bExist = FALSE;
	
	IEC_CHAR pszFileName[VMM_MAX_PATH +  1];

	*upRetry = 0;

	if (g_hFlash != (IEC_UDINT)VMF_INVALID_HANDLE)
	{
		FLASH_FILE_CLOSE(g_hFlash);
	}

	if (utilExistFile((IEC_CHAR *)pszFileName, VMM_MAX_PATH, osGetFlashDir, VMM_DIR_FLASH, VMM_FILE_FLASH, &bExist) != OK)
	{
		RETURN(ERR_FILE_REMOVE);
	}

	if (bExist == FALSE)
	{
		RETURN(OK);
	}
	
	if (utilDeleteFile((IEC_CHAR *)pszFileName, VMM_MAX_PATH, osGetFlashDir, VMM_DIR_FLASH, VMM_FILE_FLASH) != OK)
	{
		RETURN(ERR_FILE_REMOVE);
	}
	
	sync();
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * osFWInit
 *
 * Initializes the flash memory for a new write cycle.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osFWInit()
{
	IEC_CHAR pszFileName[VMM_MAX_PATH +  1];

	if (g_hFlash != (IEC_UDINT)VMF_INVALID_HANDLE)
	{
		FLASH_FILE_CLOSE(g_hFlash);
	}

	if (utilCreateFile(&g_hFlash, (IEC_CHAR *)pszFileName, VMM_MAX_PATH, osGetFlashDir, VMM_DIR_FLASH, VMM_FILE_FLASH) != OK)
	{
		RETURN(ERR_FLASH_INIT);
	}
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * osFWFinish
 *
 * Completes a write cycle into the flash memory.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osFWFinish()
{
	if (g_hFlash == (IEC_UDINT)VMF_INVALID_HANDLE)
	{
		RETURN(ERR_FLASH);
	}

	FLASH_FILE_CLOSE(g_hFlash);
	
	sync();

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * osFlashWrite
 *
 * Writes the given data block into the flash memory.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osFlashWrite(IEC_DATA *pData, IEC_UINT uLen)
{
	if (g_hFlash == (IEC_UDINT)VMF_INVALID_HANDLE)
	{
		RETURN(ERR_FLASH);
	}
	
	if (xxxWrite(g_hFlash, pData, uLen) != OK)
	{
		FLASH_FILE_CLOSE(g_hFlash);
		RETURN(ERR_FLASH_WRITE);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * osFRInit
 *
 * Initializes the flash memory for a new read cycle.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osFRInit()
{
	IEC_CHAR pszFileName[VMM_MAX_PATH + 1];
	IEC_BOOL bExist;

	if (g_hFlash != (IEC_UDINT)VMF_INVALID_HANDLE)
	{
		FLASH_FILE_CLOSE(g_hFlash);
	}

	if (utilExistFile((IEC_CHAR *)pszFileName, VMM_MAX_PATH, osGetFlashDir, VMM_DIR_FLASH, VMM_FILE_FLASH, &bExist) != OK)
	{
		RETURN(ERR_FLASH_INIT);
	}

	if (bExist == FALSE)
	{
		return ERR_FILE_NOT_EXIST;
	}
	
	if (utilOpenFile(&g_hFlash, (IEC_CHAR *)pszFileName, VMM_MAX_PATH, osGetFlashDir, VMM_DIR_FLASH, VMM_FILE_FLASH, FIO_MODE_READ) != OK)
	{
		RETURN(ERR_FLASH_INIT);
	}
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * osFRFinish
 *
 * Completes a read cycle from the flash memory.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osFRFinish()
{
	if (g_hFlash == (IEC_UDINT)VMF_INVALID_HANDLE)
	{
		RETURN(ERR_FLASH);
	}

	FLASH_FILE_CLOSE(g_hFlash);
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * osFRInitDomain
 *
 * Initializes the reading of a new flash (download) domain. Each 
 * domain equals the corresponding download domain.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osFRInitDomain(IEC_UINT uDomain, IEC_UDINT *ulpLen)
{
	IEC_UINT uNewDomain;
	IEC_UINT uRead;

	*ulpLen = 0;
	if (g_hFlash == (IEC_UDINT)VMF_INVALID_HANDLE)
	{
		RETURN(ERR_FLASH);
	}

	uRead = sizeof(uNewDomain);
	if (xxxRead(g_hFlash, (IEC_DATA *)&uNewDomain, &uRead) != OK)
	{
		FLASH_FILE_CLOSE(g_hFlash);
		RETURN(ERR_FLASH_READ);
	}

	if (uDomain != uNewDomain)
	{
		if (xxxSeek(g_hFlash, (IEC_UDINT)-(IEC_DINT)(sizeof(uNewDomain)), FSK_SEEK_CUR) != OK)
		{
			FLASH_FILE_CLOSE(g_hFlash);
			RETURN(ERR_FLASH_WRITE);
		}

		return ERR_FLASH_WRONG_BLOCK;
	}

	uRead = sizeof(g_ulCount);
	if (xxxRead(g_hFlash, (IEC_DATA *)&g_ulCount, &uRead) != OK)
	{
		FLASH_FILE_CLOSE(g_hFlash);
		RETURN(ERR_FLASH_READ);
	}
	
	*ulpLen = g_ulCount;
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * osFRFinishDomain
 *
 * Completes the reading of a flash domain.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osFRFinishDomain(IEC_UINT uDomain)
{
	if (g_hFlash == (IEC_UDINT)VMF_INVALID_HANDLE)
	{
		RETURN(ERR_FLASH);
	}

	if (g_ulCount != 0)
	{
		FLASH_FILE_CLOSE(g_hFlash);
		RETURN(ERR_FLASH);
	}
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * osFlashRead
 *
 * Reads the given data block from the flash memory.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osFlashRead(IEC_UINT uDomain, IEC_UINT uBlock, IEC_DATA *pData, IEC_UINT uLen)
{
	IEC_UINT uRead;

	if (g_hFlash == (IEC_UDINT)VMF_INVALID_HANDLE)
	{
		RETURN(ERR_FLASH);
	}

	uRead = uLen;
	if (xxxRead(g_hFlash, pData, &uRead) != OK)
	{
		FLASH_FILE_CLOSE(g_hFlash);
		RETURN(ERR_FLASH_READ);
	}
	
	if (uRead != uLen)
	{
		FLASH_FILE_CLOSE(g_hFlash);
		RETURN(ERR_FLASH_READ);
	}

	g_ulCount -= uLen;

	RETURN(OK);
}

#endif /* RTS_CFG_FLASH */

/* ---------------------------------------------------------------------------- */
