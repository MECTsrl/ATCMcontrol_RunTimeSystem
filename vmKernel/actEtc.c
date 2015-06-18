
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
 * Filename: actEtc.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"actEtc.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */

	
/* ---------------------------------------------------------------------------- */
/**
 * cmdDBIGet
 */
#if defined(RTS_CFG_DEBUG_INFO)

IEC_UINT cmdDBIGet(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes  = OK;
	SDLDebug *pDBI = &pVMM->DLB.DBI;

	if (pVMM->bProjLoaded == FALSE)
	{
		RETURN(ERR_NO_PROJECT);
	}
	if (pVMM->pLogin[pBlock->usSource] == FALSE)
	{
		RETURN(ERR_LOGIN);
	}
	
	uRes = dbiInitialize(pVMM);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (pBlock->uBlock == 1)
	{
		if (pBlock->uLen == 0)
		{
			if (pDBI->uRecv != 0 && pBlock->byLast == TRUE)
			{
				/* Re-use last String
				 */
				RETURN(OK);
			}
			else
			{
				RETURN(ERR_INVALID_PARAM);
			}
		}
		
		pDBI->uRecv = 0;
		pDBI->uProc = 0;
	}

	if (pDBI->uRecv + pBlock->uLen > MAX_DBI_RECV_BUFF)
	{
		RETURN(ERR_DBI_BUF_TOO_SMALL);
	}

	OS_MEMCPY(pDBI->pRecv + pDBI->uRecv, pBlock->CMD.pData, pBlock->uLen);
	pDBI->uRecv = (IEC_UINT)(pDBI->uRecv + pBlock->uLen);

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDBIGetChildren
 */
IEC_UINT resDBIGetChildren(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SDLDebug *pDBI = &pVMM->DLB.DBI;

	IEC_UINT uRes	= dbiGetChildren(pVMM, NULL, pDBI->pRecv, pDBI->uRecv, pBlock, &pDBI->uProc);
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDBIGetAddress
 */
IEC_UINT resDBIGetAddress(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes	= OK;

	SDLDebug *pDBI	= &pVMM->DLB.DBI;

	uRes = dbiGetAddress(pVMM, NULL, pDBI->pRecv, pDBI->uRecv, pBlock);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resDBIGetTaskNr
 */
IEC_UINT resDBIGetTaskNr(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	SDLDebug *pDBI = &pVMM->DLB.DBI;

	IEC_UINT uRes = dbiGetTaskNr(pVMM, pDBI->pRecv, pBlock);	

	RETURN(uRes);
}

#endif	/* RTS_CFG_DEBUG_INFO */

/* ---------------------------------------------------------------------------- */
/**
 * cmdGetProjectInfo
 */
#if defined(RTS_CFG_EXT_PROJ_INFO)

IEC_UINT cmdGetProjectInfo(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock == 0)
	{
		RETURN(OK);
	}

	if (pVMM->bProjLoaded == FALSE)
	{
		RETURN(ERR_NO_PROJECT);
	}

	if (pVMM->Project.uProjInfo != 1)
	{
		RETURN(ERR_INVALID_DATA);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resGetProjectInfo
 */
IEC_UINT resGetProjectInfo(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	XProjInfo xProjInfo;

	SProjInfo	*pProjInfo = &pVMM->ProjInfo;
	SDLProjInfo *pPIN	   = &pVMM->DLB.PIN;

	if (pBlock == NULL)
	{
		pVMM->DLB.PIN.uDone = 0;

		RETURN(OK);
	}

	OS_STRCPY(xProjInfo.szProjName, pProjInfo->szProjName);
	OS_STRCPY(xProjInfo.szProjVers, pProjInfo->szProjVers);

	OS_STRCPY(xProjInfo.szCreated,	pProjInfo->szCreated);
	OS_STRCPY(xProjInfo.szModified, pProjInfo->szModified);
	OS_STRCPY(xProjInfo.szOwner,	pProjInfo->szOwner);

	OS_STRCPY(xProjInfo.szComment1, pProjInfo->szComment1);
	OS_STRCPY(xProjInfo.szComment2, pProjInfo->szComment2);
	OS_STRCPY(xProjInfo.szComment3, pProjInfo->szComment3);

	if (sizeof(xProjInfo) - pPIN->uDone > MAX_DATA)
	{
		pBlock->uLen	= MAX_DATA;
		pBlock->byLast	= FALSE;
	}
	else
	{
		pBlock->uLen	= (IEC_UINT)(sizeof(xProjInfo) - pPIN->uDone);
		pBlock->byLast	= TRUE;
	}

	OS_MEMCPY(pBlock->CMD.pData, (IEC_DATA *)&xProjInfo + pPIN->uDone, pBlock->uLen);

	pPIN->uDone = (IEC_UINT)(pPIN->uDone + pBlock->uLen);
	
	RETURN(OK);
}

#endif /* RTS_CFG_EXT_PROJ_INFO */


/* ---------------------------------------------------------------------------- */
/**
 * cmdClearFlash
 */
#if defined(RTS_CFG_FLASH)

IEC_UINT cmdClearFlash(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	if (pBlock->uLen != sizeof(IEC_UINT))
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	pVMM->DLB.CFL.uRetry = *(IEC_UINT *)pBlock->CMD.pData;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resClearFlash
 */
IEC_UINT resClearFlash(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	uRes = osClearFlash(&pVMM->DLB.CFL.uRetry);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	*(IEC_UINT *)pBlock->CMD.pData = pVMM->DLB.CFL.uRetry;
	pBlock->uLen = sizeof(IEC_UINT);
	
	RETURN(OK);
}

#endif	/* RTS_CFG_FLASH */


/* ---------------------------------------------------------------------------- */
/**
 * cmdFlash
 */
#if defined(RTS_CFG_FLASH)

IEC_UINT cmdFlash(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	if (pBlock->uBlock == 1)
	{
		uRes = osFWInit();
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	if (pBlock->uBlock == 1)
	{
		if (pBlock->uLen >= 2 * sizeof(IEC_UINT) + VMM_GUID + sizeof(IEC_UDINT))
		{
			/* Copy project GUID into retain segment. It would be applied
			 * on the next warm/coldstart. (This is OK, because retentive
			 * memory changes are not supported.)
			 */
			IEC_UINT uFlashDomain = *(IEC_UINT *)pBlock->CMD.pData;

			if (uFlashDomain == FLASH_DOMAIN_BEGIN)
			{
				IEC_DATA *pGUID = pBlock->CMD.pData + 2 * sizeof(IEC_UINT) + sizeof(IEC_UDINT);

			  #if defined(RTS_CFG_EXT_RETAIN)
				SMessage Message;

				Message.uID 		= MSG_RT_SET_GUID;
				Message.uLen		= VMM_GUID;
				Message.uRespQueue	= Q_RESP_VMM_RET;

				TRACE_GUID("Ret-Cfg1", "FLH", pGUID);

				OS_MEMCPY(Message.pData, pGUID, VMM_GUID);

				uRes = msgTXMessage(&Message, Q_LIST_RET, VMM_TO_IPC_MSG, TRUE);
				if (uRes != OK)
				{
					RETURN(uRes);
				}
			  #else
				SObject *pRetain = pVMM->Shared.pData + SEG_RETAIN;

				TRACE_GUID("Ret-Cfg1", "FLH", pGUID);

				OS_MEMCPY(pRetain->pAdr + (pRetain->ulSize - VMM_GUID), pGUID, VMM_GUID);
			  #endif
			}
		}

	} /* if (pBlock->uBlock == 1) */
	
  #if defined(RTS_CFG_VM_IPC)
	uRes = timEnableAllWDog(pVMM, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
  #endif

	uRes = osFlashWrite(pBlock->CMD.pData, pBlock->uLen);

  #if defined(RTS_CFG_VM_IPC)
	timRestoreAllWDog(pVMM);
  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resFlash
 */
IEC_UINT resFlash(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	uRes = osFWFinish();
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	pBlock->uLen	= 0;
	pBlock->byLast	= TRUE;
	
	RETURN(OK);
}

#endif	/* RTS_CFG_FLASH */


/* ---------------------------------------------------------------------------- */
/**
 * cmdGetConfig
 */
IEC_UINT cmdGetConfig(STaskInfoVMM *pVMM, XBlock *pBlock)
{

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resGetConfig
 *
 */
IEC_UINT resGetConfig(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	XConfig *pConf = (XConfig *)pBlock->CMD.pData;

  #if defined(RTS_CFG_BIGENDIAN)
	pConf->usBigEndian	= 1;
  #else
	pConf->usBigEndian	= 0;
  #endif
	
	pConf->uFirmware	= RTS_VERSION_COMPATIBLE;
	pConf->uMaxData 	= MAX_DATA;
	pConf->uAddConfig	= 0;

	pBlock->uLen	= (IEC_UINT)(sizeof(XConfig) + pConf->uAddConfig);
	pBlock->byLast	= TRUE;
	
	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdRetWrite
 *
 */
#if defined(RTS_CFG_EXT_RETAIN)

IEC_UINT cmdRetWrite(STaskInfoVMM *pVMM, XBlock *pBlock)
{

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resRetWrite
 *
 */
IEC_UINT resRetWrite(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	uRes = msgTXCommand(MSG_RT_UPDATE, Q_LIST_RET, IPC_Q_NONE, VMM_NO_WAIT, TRUE);

	RETURN(uRes);
}

#endif /* RTS_CFG_EXT_RETAIN */


/* ---------------------------------------------------------------------------- */
/**
 * cmdRetCycle
 *
 */
#if defined(RTS_CFG_EXT_RETAIN)

IEC_UINT cmdRetCycle(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;
	
	SMessage Message;

	if (pBlock->uLen != sizeof(IEC_UDINT))
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	Message.uID 		= MSG_RT_SET_CYCLE;
	Message.uLen		= sizeof(IEC_UDINT);
	Message.uRespQueue	= Q_RESP_VMM_RET;

	*(IEC_UDINT *)Message.pData = *(IEC_UDINT *)pBlock->CMD.pData;

	uRes = msgTXMessage(&Message, Q_LIST_RET, VMM_TO_IPC_MSG, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resRetCycle
 *
 */
IEC_UINT resRetCycle(STaskInfoVMM *pVMM, XBlock *pBlock)
{

	RETURN(OK);
}

#endif /* RTS_CFG_EXT_RETAIN */



/* ---------------------------------------------------------------------------- */
/**
 * cmdGetInstKey
 *
 */
#if defined(RTS_CFG_LICENSE)

IEC_UINT cmdGetInstKey(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resGetInstKey
 *
 */
IEC_UINT resGetInstKey(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;
	
	XInstKey *pIK = (XInstKey *)pBlock->CMD.pData;

	uRes = sysGetInstKey(pIK->szInstKey);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	pBlock->uLen	= sizeof(XInstKey);
	pBlock->byLast	= TRUE;

	RETURN(uRes);
}

#endif /* RTS_CFG_LICENSE */


/* ---------------------------------------------------------------------------- */
/**
 * cmdSetLicKey
 *
 */
#if defined(RTS_CFG_LICENSE)

IEC_UINT cmdSetLicKey(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	XLicKey *pLK  = (XLicKey *)pBlock->CMD.pData;

	if (pBlock->uLen != sizeof(XLicKey))
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}
	
	uRes = sysSetLicKey(pLK->szLicKey);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resSetLicKey
 *
 */
IEC_UINT resSetLicKey(STaskInfoVMM *pVMM, XBlock *pBlock)
{

	RETURN(OK);
}

#endif /* RTS_CFG_LICENSE */


/* ---------------------------------------------------------------------------- */
/**
 * cmdGetSerialNo
 *
 */
#if defined(RTS_CFG_LICENSE)

IEC_UINT cmdGetSerialNo(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resGetSerialNo
 *
 */
IEC_UINT resGetSerialNo(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	XSerialNo *pSN	= (XSerialNo *)pBlock->CMD.pData;

	pBlock->uLen	= sizeof(XSerialNo);
	pBlock->byLast	= TRUE;

	uRes = sysGetSerialNo(&pSN->ulSerialNo);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}

#endif /* RTS_CFG_LICENSE */


/* ---------------------------------------------------------------------------- */
/**
 * cmdGetFeature
 *
 */
#if defined(RTS_CFG_LICENSE)

IEC_UINT cmdGetFeature(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resGetFeature
 *
 */
IEC_UINT resGetFeature(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	XFeatDef *pFD	= (XFeatDef *)pBlock->CMD.pData;

	pBlock->uLen	= sizeof(XFeatDef);
	pBlock->byLast	= TRUE;

	uRes = sysGetFeatureAvail(&pFD->uAvailable);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = sysGetFeatureLic(&pFD->uLicensed);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}

#endif /* RTS_CFG_LICENSE */


/* ---------------------------------------------------------------------------- */
/**
 * cmdSetLicEx
 *
 */
#if defined(RTS_CFG_LICENSE)

IEC_UINT cmdSetLicEx(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	XLicEx *pLE;

	if (pBlock == NULL)
	{
		RETURN(OK);
	}

	if (pBlock->uLen < sizeof(XLicEx))
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}
	
	pLE = (XLicEx *)pBlock->CMD.pData;

	if (pBlock->uLen != sizeof(XLicEx) + pLE->uLength)
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	uRes = osCmdSetLicEx(pVMM, pBlock);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * resSetLicEx
 *
 */
IEC_UINT resSetLicEx(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	uRes = osResSetLicEx(pVMM, pBlock);

	RETURN(uRes);
}

#endif /* RTS_CFG_LICENSE */


/* ---------------------------------------------------------------------------- */
/**
 * cmdGetType
 *
 */
IEC_UINT cmdGetType(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resGetType
 *
 */
IEC_UINT resGetType(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	XTypeInfo *pTI	= (XTypeInfo *)pBlock->CMD.pData;

	pBlock->uLen	= sizeof(XTypeInfo);
	pBlock->byLast	= TRUE;

	uRes = sysGetType(pTI->szType);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * cmdGetVersion
 *
 */
IEC_UINT cmdGetVersion(STaskInfoVMM *pVMM, XBlock *pBlock)
{

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * resGetVersion
 *
 */
IEC_UINT resGetVersion(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	XVersionInfo *pVI = (XVersionInfo *)pBlock->CMD.pData;

	pBlock->uLen	= sizeof(XVersionInfo);
	pBlock->byLast	= TRUE;

	pVI->ulFirmware = RTS_VERSION_COMPATIBLE;

	uRes = sysGetVersion(pVI->szFull);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	OS_STRCPY(pVI->szMain, RTS_MAIN_VERSION);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
