
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
 * Filename: vmmRet.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__		"vmmRet.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_EXT_RETAIN)

#include "osFile.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define RET_VALID_SEGMENT		0xaaaau

#define RET_INVALID_ID			0xfffffffful

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

static IEC_UINT retUpdate(SRetFile *pRF, SObject *pRetain);
static IEC_UINT retOpen(SRetFile **ppRF, IEC_UINT uMode, SObject *pRetain, IEC_DATA *pProjectID);
static IEC_UINT retClose(SRetFile **ppRF);
static IEC_UINT retSetGuid(SRetFile *pRF, IEC_DATA *pProjectID);
static IEC_UINT retSetSize(SRetFile *pRF, IEC_UDINT ulUsed);

static IEC_UINT retWrite(SRetFile *pRF);
static IEC_UINT retRead(IEC_UDINT ulID, IEC_UDINT ulSize, SRetFile **ppRF);

static IEC_UINT retGetFileName(IEC_CHAR *szFile, IEC_UINT uLen, IEC_UINT uFile);
static IEC_UINT retFindFile(SRetFile **ppRF, IEC_BOOL *pExist);
static IEC_UINT retClearFiles(IEC_UDINT ulIDActive, IEC_BOOL *pExist);
static IEC_UINT retExistFiles(IEC_BOOL *pExist);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * retMain
 *
 */
IEC_UINT retMain(void *pPara)
{
	SObject  Retain;
	SRetFile *pRF = NULL;

	SMessage Message;
	IEC_UINT uRespQueue;

	IEC_BOOL bUpdate	= FALSE;
	IEC_BOOL bFirst 	= TRUE;

	IEC_UDINT ulCycle	= VMM_RET_UPD_CYCLE;

	IEC_UDINT ulUpdate	= 0ul;

	IEC_UINT uRes = osCreateIPCQueue(Q_LIST_RET);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

  #if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo(TASK_OFFS_SYS_RET, osGetTaskID());
	TR_RET(uRes);
  #endif
	for ( ; ; )
	{
		IEC_UDINT ulWait = bUpdate == TRUE ? VMM_RET_UPD_FIRST : VMM_WAIT_FOREVER;
			
		if (bUpdate == TRUE && bFirst == FALSE)
		{
			ulWait = ulCycle - (osGetTime32() - ulUpdate);
			if (ulWait > ulCycle)
			{
				ulWait = 100u;
			}
		}

		uRes = msgRecv(&Message, Q_LIST_RET, ulWait);

		if (uRes == WRN_TIME_OUT && bUpdate == TRUE)
		{
			ulUpdate = osGetTime32();

			retUpdate(pRF, &Retain);

			bFirst = FALSE;
			continue;
		}

		if (uRes != OK)
		{
			osSleep(100);
			continue;
		}

		uRespQueue			= Message.uRespQueue;
		Message.uRespQueue	= IPC_Q_NONE;

		switch(Message.uID)
		{
			case MSG_RT_OPEN:	/* -------------------------------------------- */
			{
				bUpdate = FALSE;

				if (Message.uLen != sizeof(SRetExt))
				{
					uRes = ERR_INVALID_PARAM;
				}

				if (uRes == OK)
				{
					SRetExt *pRE = (SRetExt *)Message.pData;

					OS_MEMCPY(&Retain, &pRE->Retain, sizeof(SObject));

					uRes = retOpen(&pRF, pRE->uMode, &Retain, pRE->pProjectID);
				}

				if (uRespQueue != IPC_Q_NONE)
				{
					if (uRes == OK)
					{
						Message.uLen = 0;
					}
					else
					{
						msgSetError(&Message, uRes);
					}

					msgSend(&Message, uRespQueue);
				}

				break;
			}

			case MSG_RT_CLOSE:	/* -------------------------------------------- */
			{
				if (bUpdate == TRUE)
				{
					ulUpdate = osGetTime32();
					uRes	 = retUpdate(pRF, &Retain);
				}

				bUpdate = FALSE;

				uRes = retClose(&pRF);

				if (uRespQueue != IPC_Q_NONE)
				{
					if (uRes == OK)
					{
						Message.uLen = 0;
					}
					else
					{
						msgSetError(&Message, uRes);
					}

					msgSend(&Message, uRespQueue);
				}

				break;
			}

			case MSG_RT_START:	/* -------------------------------------------- */
			{
				bUpdate = TRUE;
				bFirst	= TRUE;

				if (uRespQueue != IPC_Q_NONE)
				{
					Message.uLen = 0;
					msgSend(&Message, uRespQueue);
				}

				break;
			}

			case MSG_RT_STOP:	/* -------------------------------------------- */
			{
				if (bUpdate == TRUE)
				{
					ulUpdate = osGetTime32();
					uRes	 = retUpdate(pRF, &Retain);
				}
				
				bUpdate = FALSE;

				if (uRespQueue != IPC_Q_NONE)
				{
					Message.uLen = 0;
					msgSend(&Message, uRespQueue);
				}

				break;
			}

			case MSG_RT_UPDATE: /* -------------------------------------------- */
			{
				if (bUpdate == TRUE)
				{
					bFirst	 = FALSE;

					ulUpdate = osGetTime32();
					uRes	 = retUpdate(pRF, &Retain);
				}
				else
				{
					uRes = ERR_NOT_READY;
				}
				
				if (uRespQueue != IPC_Q_NONE)
				{
					if (uRes == OK)
					{
						Message.uLen = 0;
					}
					else
					{
						msgSetError(&Message, uRes);
					}

					msgSend(&Message, uRespQueue);
				}

				break;
			}

			case MSG_RT_SET_GUID: /* ------------------------------------------ */
			{
				if (Message.uLen != VMM_GUID)
				{
					uRes = ERR_INVALID_PARAM;
				}

				if (uRes == OK)
				{
					uRes = retSetGuid(pRF, Message.pData);
				}

				if (uRespQueue != IPC_Q_NONE)
				{
					if (uRes == OK)
					{
						Message.uLen = 0;
					}
					else
					{
						msgSetError(&Message, uRes);
					}

					msgSend(&Message, uRespQueue);
				}

				break;
			}

			case MSG_RT_SET_SIZE: /* ------------------------------------------ */
			{
				if (Message.uLen != sizeof(IEC_UDINT))
				{
					uRes = ERR_INVALID_PARAM;
				}

				if (uRes == OK)
				{
					//uRes = retSetSize(pRF, *(UNALIGNED IEC_UDINT *)Message.pData);
					IEC_UDINT ulUsed;
					OS_MEMCPY(&ulUsed, Message.pData, sizeof(IEC_UDINT));
					uRes = retSetSize(pRF, ulUsed);
				}

				if (uRespQueue != IPC_Q_NONE)
				{
					if (uRes == OK)
					{
						Message.uLen = 0;
					}
					else
					{
						msgSetError(&Message, uRes);
					}

					msgSend(&Message, uRespQueue);
				}

				break;
			}

			case MSG_RT_SET_CYCLE: /* ----------------------------------------- */
			{
				if (Message.uLen != sizeof(IEC_UDINT))
				{
					uRes = ERR_INVALID_PARAM;
				}

				if (uRes == OK)
				{
					ulCycle = *(IEC_UDINT *)Message.pData;
				}

				if (uRespQueue != IPC_Q_NONE)
				{
					if (uRes == OK)
					{
						Message.uLen = 0;
					}
					else
					{
						msgSetError(&Message, uRes);
					}

					msgSend(&Message, uRespQueue);
				}

				break;
			}

			default:	/* ---------------------------------------------------- */
			{
				TR_ERROR("[retMain] Unexpected message (0x%04x) received.\r\n", Message.uID);
				
				if (uRespQueue != IPC_Q_NONE)
				{
					msgSetError(&Message, ERR_INVALID_MSG);
					msgSend(&Message, uRespQueue);
				}

				break;
			}

		} /* switch(Message.uID) */

	} /* for ( ; ; ) */

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * retUpdate
 *
 */
static IEC_UINT retUpdate(SRetFile *pRF, SObject *pRetain)
{
	IEC_UINT uRes  = OK;

  #if defined(RTS_CFG_RET_TRACE)
	IEC_ULINT ullT = osGetTimeUS();
  #endif

	if (pRF == NULL)
	{
		RETURN(ERR_INVALID_COMMAND);
	}
	
	OS_MEMCPY(pRF->pRetain, pRetain->pAdr, pRF->ulUsed);

	pRF->uValid = RET_VALID_SEGMENT;
	
	uRes = retWrite(pRF);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	pRF->ulID++;

  #if defined(RTS_CFG_RET_TRACE)
	ullT = utilGetTimeDiffUS(ullT);
	osTrace("--- RET: %6ldms\r\n", (IEC_UDINT)((ullT + 500) / 1000));
  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * retOpen
 *
 */
static IEC_UINT retOpen(SRetFile **ppRF, IEC_UINT uMode, SObject *pRetain, IEC_DATA *pProjectID)
{
	IEC_UINT uRes		= OK;
	IEC_UINT i;

	IEC_BOOL pExist[VMM_RETFILE_TO - VMM_RETFILE_FROM + 1];

	/* Check for existing retain files
	 */
	for (i = 0; i < VMM_RETFILE_TO - VMM_RETFILE_FROM + 1; i++)
	{
		pExist[i] = FALSE;
	}

	uRes = retExistFiles(pExist);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	if ((uMode & DOWN_MODE_WARM) != 0)
	{
		/* Warm start, try to find stored retain segments
		 * --------------------------------------------------------------------
		 */
		uRes = retFindFile(ppRF, pExist);
		if (uRes != OK)
		{
			goto new_retain_file;
		}

		if ((*ppRF)->ulSize != pRetain->ulSize)
		{
			/* Size of retain memory doesn't match
			 */
			uRes = ERR_ERROR;
			goto new_retain_file;
		}

		TRACE_GUID("ERet-Cmp", "---", (*ppRF)->pProjectID);
		TRACE_GUID("ERet-Cmp", "---", pProjectID);

		if (OS_MEMCMP((*ppRF)->pProjectID, pProjectID, VMM_GUID) != 0)
		{
			/* Project GUID in retain area doesn't match
			 * with new download project GUID.
			 */
			uRes = ERR_ERROR;
			goto new_retain_file;
		}

	  new_retain_file:
		
		if (uRes == OK)
		{
			/* Everything OK -> Done!
			 */
			OS_MEMCPY(pRetain->pAdr, (*ppRF)->pRetain, (*ppRF)->ulUsed);

			if ((*ppRF)->ulID == 0)
			{
				uRes = retClearFiles(0, pExist);
			}
			else
			{
				(*ppRF)->ulID++;
			}

			RETURN(uRes);
		}

		/* Error: Clear retain and rewrite flash file
		 */
		if (*ppRF != NULL)
		{
			uRes = osFree((IEC_DATA **)ppRF);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			*ppRF = NULL;
		}
	}


	/* Cold start or retain segments doesn't match
	 * ------------------------------------------------------------------------
	 */
	uRes = OK;

	*ppRF = (SRetFile *)osMalloc(sizeof(SRetFile) + pRetain->ulSize);
	if (*ppRF == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	OS_MEMSET(*ppRF, 0x00, sizeof(SRetFile) + pRetain->ulSize);
	OS_MEMSET(pRetain->pAdr, 0x00, pRetain->ulSize);

	(*ppRF)->ulID	= 1;
	(*ppRF)->uValid = 0;
	(*ppRF)->ulSize = pRetain->ulSize;
	(*ppRF)->ulUsed = 0;

	TRACE_GUID("ERet-Set1", "---", pProjectID);

	OS_MEMCPY((*ppRF)->pProjectID, pProjectID, VMM_GUID);

	uRes = retClearFiles(RET_INVALID_ID, pExist);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (uMode & DOWN_MODE_WARM)
	{
		/* Must have been an error reading the retain file.
		 */
		for (i = 0; i < VMM_RETFILE_TO - VMM_RETFILE_FROM + 1; i++)
		{
			if (pExist[i] == TRUE)
			{
				return WRN_RETAIN_INVALID;
			}
		}
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * retClose
 *
 */
static IEC_UINT retClose(SRetFile **ppRF)
{
	IEC_UINT uRes = OK;

	if (*ppRF != NULL)
	{
		uRes = osFree((IEC_DATA **)ppRF);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * retSetGuid
 *
 */
static IEC_UINT retSetGuid(SRetFile *pRF, IEC_DATA *pProjectID)
{
	IEC_UINT uRes = OK;

	if (pRF == NULL)
	{
		RETURN(ERR_INVALID_COMMAND);
	}

	TRACE_GUID("ERet-Set2", "---", pProjectID);

	OS_MEMCPY(pRF->pProjectID, pProjectID, VMM_GUID);
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * retSetSize
 *
 */
static IEC_UINT retSetSize(SRetFile *pRF, IEC_UDINT ulUsed)
{
	IEC_UINT uRes = OK;

	if (pRF == NULL)
	{
		RETURN(ERR_INVALID_COMMAND);
	}

	pRF->ulUsed = ulUsed;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * retWrite
 *
 */
static IEC_UINT retWrite(SRetFile *pRF)
{
	IEC_UINT uRes = OK;

	IEC_UDINT hRetain	= (IEC_UDINT)VMF_INVALID_HANDLE;
	IEC_CHAR  szRetain[VMM_MAX_PATH +  1];

	IEC_UDINT ulRemain;
	IEC_UDINT ulDone;

	IEC_UINT  uCRC1;
	IEC_UINT  uCRC2;

	IEC_UDINT i;

	if (pRF == NULL)
	{
		RETURN(ERR_INVALID_COMMAND);
	}

	/* Compute checksum
	 */
	uCRC1		= 0u;
	uCRC2		= 0u;

	pRF->uCRC1	= 0u;
	pRF->uCRC2	= 0u;
	
	for (i = 0; i < sizeof(SRetFile) + pRF->ulUsed; i++)
	{
		uCRC1 = (IEC_UINT)(uCRC1 + *((IEC_DATA *)pRF + i));
		uCRC2 = (IEC_UINT)(uCRC2 + uCRC1);
	}

	pRF->uCRC1	= uCRC1;
	pRF->uCRC2	= uCRC2;
	
	/* Open retain file
	 */
	uRes = retGetFileName(szRetain, VMM_MAX_PATH, (IEC_UINT)(VMM_RETFILE_FROM + pRF->ulID % (VMM_RETFILE_TO - VMM_RETFILE_FROM + 1)));
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = xxxOpen(&hRetain, szRetain, FIO_MODE_WRITE, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* Write data to file
	 */
	ulRemain = sizeof(SRetFile) + pRF->ulUsed;
	ulDone	 = 0;

	while (ulRemain > 0)
	{
		IEC_UINT uToWrite = (IEC_UINT)(ulRemain > 0xffffu ? 0xffffu : ulRemain);

		uRes = xxxWrite(hRetain, (IEC_DATA *)pRF + ulDone, uToWrite);
		if (uRes != OK)
		{
			xxxClose(hRetain);
			RETURN(uRes);
		}

		ulRemain -= uToWrite;
		ulDone	 += uToWrite;
	}

	xxxClose(hRetain);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * retRead
 *
 */
static IEC_UINT retRead(IEC_UDINT ulID, IEC_UDINT ulUsed, SRetFile **ppRF)
{
	IEC_UINT uRes = OK;

	IEC_UDINT hRetain	= (IEC_UDINT)VMF_INVALID_HANDLE;
	IEC_CHAR  szRetain[VMM_MAX_PATH +  1];
	
	IEC_UDINT ulRemain	= sizeof(SRetFile) + ulUsed;
	IEC_UDINT ulDone	= 0;

	IEC_UINT  uCRC1;
	IEC_UINT  uCRC2;
	IEC_UINT  uCRCOrg1;
	IEC_UINT  uCRCOrg2;

	IEC_UDINT i;

	/* Open retain file
	 */
	uRes = retGetFileName(szRetain, VMM_MAX_PATH, (IEC_UINT)(VMM_RETFILE_FROM + ulID % (VMM_RETFILE_TO - VMM_RETFILE_FROM + 1)));
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = xxxOpen(&hRetain, szRetain, FIO_MODE_READ, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* Allocate memory
	 */
	if (*ppRF != NULL)
	{
		uRes = osFree((IEC_DATA **)ppRF);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		*ppRF = NULL;
	}

	*ppRF = (SRetFile *)osMalloc(sizeof(SRetFile) + ulUsed);
	if (*ppRF == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	/* Read File
	 */
	while (ulRemain > 0)
	{
		IEC_UINT uToRead = (IEC_UINT)(ulRemain > 0xffffu ? 0xffffu : ulRemain);
		IEC_UINT uRead	 = uToRead;

		uRes = xxxRead(hRetain, (IEC_DATA *)*ppRF + ulDone, &uRead);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		if (uToRead != uRead)
		{
			RETURN(ERR_FILE_READ);
		}

		ulRemain -= uToRead;
		ulDone	 += uToRead;
	}

	xxxClose(hRetain);

	/* Verify retain data
	 */
	if ((*ppRF)->ulUsed != ulUsed || (*ppRF)->ulUsed > (*ppRF)->ulSize)
	{
		RETURN(ERR_INVALID_DATA);
	}

	if ((*ppRF)->uValid != RET_VALID_SEGMENT ||(*ppRF)->ulID != ulID)
	{
		RETURN(ERR_INVALID_DATA);
	}

	uCRC1 = 0;
	uCRC2 = 0;

	uCRCOrg1 = (*ppRF)->uCRC1;
	uCRCOrg2 = (*ppRF)->uCRC2;

	(*ppRF)->uCRC1 = 0;
	(*ppRF)->uCRC2 = 0;

	for (i = 0; i < sizeof(SRetFile) + (*ppRF)->ulUsed; i++)
	{
		uCRC1 = (IEC_UINT)(uCRC1 + *((IEC_DATA *)*ppRF + i));
		uCRC2 = (IEC_UINT)(uCRC2 + uCRC1);
	}

	if (uCRC1 != uCRCOrg1 || uCRC2 != uCRCOrg2)
	{
		RETURN(ERR_CRC);
	}

	(*ppRF)->uCRC1 = uCRC1;
	(*ppRF)->uCRC1 = uCRC2;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * retGetFileName
 *
 */
static IEC_UINT retGetFileName(IEC_CHAR *szFile, IEC_UINT uLen, IEC_UINT uFile)
{
	IEC_UINT uRes = OK;
	IEC_CHAR szTemp[VMM_MAX_PATH +	1];

	OS_SPRINTF(szTemp, VMM_FILE_RETAIN, uFile);

	uRes = utilCreatePath(szFile, uLen, osGetRetainDir, VMM_DIR_RETAIN, "");
	if (uRes != OK)
	{
		RETURN(OK);
	}

	uRes = xxxCreateDir(szFile, TRUE, DIR_MODE_WRITE);
	if (uRes != OK)
	{
		RETURN(OK);
	}

	uRes = utilCreatePath(szFile, uLen, osGetRetainDir, VMM_DIR_RETAIN, szTemp);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * retFindFile
 *
 */
static IEC_UINT retFindFile(SRetFile **ppRF, IEC_BOOL *pExist)
{
	IEC_UINT  uRes = OK;

	IEC_CHAR  szRetain[VMM_MAX_PATH +  1];
	IEC_UDINT hRetain;

	IEC_UDINT ulIDLatest1 = RET_INVALID_ID;
	IEC_UDINT ulIDLatest2 = RET_INVALID_ID;

	IEC_UDINT ulSize1	  = 0;
	IEC_UDINT ulSize2	  = 0;

	IEC_BOOL  bContinue   = FALSE;

	SRetFile  RF;

	IEC_UINT  i;

	for (i = 0; i < VMM_RETFILE_TO - VMM_RETFILE_FROM + 1; i++)
	{
		IEC_UINT uLen	= 0;

		if (pExist[i] == FALSE || bContinue == TRUE)
		{
			continue;
		}

		uRes = retGetFileName(szRetain, VMM_MAX_PATH, (IEC_UINT)(VMM_RETFILE_FROM + i));
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		uRes = xxxOpen(&hRetain, szRetain, FIO_MODE_READ, FALSE);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		uLen = sizeof(SRetFile);

		uRes = xxxRead(hRetain, (IEC_DATA *)&RF, &uLen);
		if (uRes != OK || uLen != sizeof(SRetFile))
		{
			xxxClose(hRetain);
			continue;
		}

		xxxClose(hRetain);

		if (i == 0 && RF.ulID == 0 && RF.uValid == RET_VALID_SEGMENT)
		{
			/* New file directly downloaded.
			 */
			ulIDLatest1 = 0;
			ulIDLatest2 = RET_INVALID_ID;

			ulSize1 	= RF.ulUsed;
			ulSize2 	= 0;

			bContinue = TRUE;

			continue;
		}

		if (RF.uValid != RET_VALID_SEGMENT)
		{
			continue;
		}

		if (RF.ulID > ulIDLatest1 || ulIDLatest1 == RET_INVALID_ID)
		{
			ulIDLatest2 = ulIDLatest1;
			ulIDLatest1 = RF.ulID;

			ulSize2 	= ulSize1;
			ulSize1 	= RF.ulUsed;
		}

		if ((RF.ulID > ulIDLatest2 || ulIDLatest2 == RET_INVALID_ID) && RF.ulID < ulIDLatest1)
		{
			ulIDLatest2 = RF.ulID;
			ulSize2 	= RF.ulUsed;
		}
	}

	/* Try to read latest retain segment
	 */
	if (ulIDLatest1 == RET_INVALID_ID)
	{
		return WRN_RETAIN_INVALID;
	}

	uRes = retRead(ulIDLatest1, ulSize1, ppRF);
	if (uRes == OK)
	{
		RETURN(OK);
	}
	
	uRes = OK;

	/* Try to read one before latest retain segment
	 */
	if (ulIDLatest2 == RET_INVALID_ID)
	{
		return WRN_RETAIN_INVALID;
	}

	uRes = retRead(ulIDLatest2, ulSize2, ppRF);
	if (uRes == OK)
	{
		RETURN(OK);
	}

	return WRN_RETAIN_INVALID;
}

/* ---------------------------------------------------------------------------- */
/**
 * retClear
 *
 */
static IEC_UINT retClearFiles(IEC_UDINT ulIDActive, IEC_BOOL *pExist)
{
	IEC_UINT uRes = OK;
	IEC_UINT i;

	IEC_CHAR  szRetain[VMM_MAX_PATH +  1];
	IEC_UDINT hRetain;

	SRetFile RF;

	OS_MEMSET(&RF, 0x00, sizeof(SRetFile));

	RF.ulID = RET_INVALID_ID;

	for (i = 0; i < VMM_RETFILE_TO - VMM_RETFILE_FROM + 1; i++)
	{
		if (pExist[i] == FALSE)
		{
			continue;
		}

		if (ulIDActive != RET_INVALID_ID && i == ulIDActive % (VMM_RETFILE_TO - VMM_RETFILE_FROM + 1))
		{
			continue;
		}

		uRes = retGetFileName(szRetain, VMM_MAX_PATH, (IEC_UINT)(VMM_RETFILE_FROM + i));
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		uRes = xxxOpen(&hRetain, szRetain, FIO_MODE_WRITE, FALSE);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		uRes = xxxWrite(hRetain, (IEC_DATA *)&RF, sizeof(SRetFile));
		if (uRes != OK)
		{
			xxxClose(hRetain);
			RETURN(uRes);
		}

		xxxClose(hRetain);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * retExistFiles
 *
 */
static IEC_UINT retExistFiles(IEC_BOOL *pExist)
{
	IEC_UINT uRes = OK;
	IEC_UINT i;

	IEC_CHAR  szRetain[VMM_MAX_PATH +  1];

	for (i = 0; i < VMM_RETFILE_TO - VMM_RETFILE_FROM + 1; i++)
	{
		uRes = retGetFileName(szRetain, VMM_MAX_PATH, (IEC_UINT)(VMM_RETFILE_FROM + i));
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		uRes = xxxExist(szRetain, pExist + i);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	RETURN(uRes);
}

#endif /* RTS_CFG_EXT_RETAIN */

/* ---------------------------------------------------------------------------- */
