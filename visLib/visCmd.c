
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
 * Filename: visCmd.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"visCmd.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "osSocket.h"

#include "visDef.h"
#include "visMain.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * domGetConfig
 *
 * Reads the actual configuration data (firmware version, command buffer length
 * and big/little endian settings) from the attached RTS.
 *
 */
IEC_UINT domGetConfig(SVisInfo *pVI, XConfig *xpConf)
{
	IEC_UINT uLen	= 0;
	IEC_UINT uRes	= OK;
	
	XBlock *pBlock	= (XBlock *)osMalloc(HD_BLOCK + sizeof(XConfig));

	pBlock->CMD.byCommand	= CMD_GET_CONFIG;

	/* Should work also, if client and server are different big and little
	 * Endian processors!
	 */
	pBlock->uBlock			= 1;	
	pBlock->uLen			= 0;
	pBlock->byLast			= TRUE;

	uRes = domSockSend(pVI, (IEC_DATA *)pBlock, (IEC_UINT)(HD_BLOCK + 0));
	if (uRes != OK)
	{
		uRes = osFree((IEC_DATA **)&pBlock);
		RETURN(uRes);
	}

	uLen = HD_BLOCK + sizeof(XConfig);

	uRes = domSockRecv(pVI, (IEC_DATA *)pBlock, &uLen);
	if (uRes != OK)
	{
		uRes = osFree((IEC_DATA **)&pBlock);
		RETURN(uRes);
	}

	if (uLen != HD_BLOCK + sizeof(XConfig))
	{
		uRes = osFree((IEC_DATA **)&pBlock);
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	OS_MEMCPY(xpConf, pBlock->CMD.pData, sizeof(XConfig));

	uRes = osFree((IEC_DATA **)&pBlock);

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetProjVersion
 *
 * Reads the actual project version (if any) from the attached RTS.
 *
 */
IEC_UINT domGetProjVersion(SVisInfo *pVI, IEC_DATA **ppGUID)
{
	IEC_UINT uRes	= OK;
	
	IEC_UINT uLen	= 0;
	IEC_DATA *pData = NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	uRes = domTransferData(pVI, CMD_GET_PROJ_VERSION, &pData, &uLen, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	if (uLen != VMM_GUID)
	{
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	*ppGUID = pData;

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * domLogin
 *
 * Login to a connected RTS. (A project must be loaded and the corresponding
 * project GUID must be provided.)
 *
 */
IEC_UINT domLogin(SVisInfo *pVI, IEC_DATA *pProjectID)
{
	IEC_UINT uRes	= OK;

	IEC_UINT uLen	= VMM_GUID;
	IEC_DATA *pData = pProjectID;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	pVI->bLogin = FALSE;

	uRes = domTransferData(pVI, CMD_LOGIN, &pData, &uLen, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	pVI->bLogin = TRUE;

	if (pData != NULL)
	{
		uRes = osFree(&pData);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}
	
	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domLogout
 *
 * Logout from a connected RTS.
 *
 */
IEC_UINT domLogout(SVisInfo *pVI)
{
	IEC_UINT uRes	= OK;

	IEC_UINT uLen	= 0;
	IEC_DATA *pData = NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	pVI->bLogin = FALSE;

	uRes = domTransferData(pVI, CMD_LOGOUT, &pData, &uLen, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (pData != NULL)
	{
		uRes = osFree(&pData);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetChildren
 *
 * Retrieves the children (if any) of a given (fully qualified) IEC identifier.
 *
 */
IEC_UINT domGetChildren(SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease)
{
	IEC_UINT uRes = OK;
	
	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}
	if (pVI->bLogin == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	uRes = domTransferData(pVI, CMD_DBI_GET_CHILDREN, ppData, upLen, bRelease);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetAddress
 *
 * Retrieves the address a given (fully qualified) IEC identifier.
 *
 */
IEC_UINT domGetAddress(SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease)
{
	IEC_UINT uRes = OK;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}
	if (pVI->bLogin == FALSE)
	{
		RETURN(ERR_LOGIN);
	}
	
	uRes = domTransferData(pVI, CMD_DBI_GET_ADDRESS, ppData, upLen, bRelease);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetValue
 *
 */
IEC_UINT domGetValue(SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease)
{
	IEC_UINT uRes = OK;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}
	if (pVI->bLogin == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	if (pVI->ulFirmware < 21000)
	{
		uRes = domGetValue20(pVI, ppData, upLen, bRelease);
	}
	else
	{
		uRes = domGetValue21(pVI, ppData, upLen, bRelease);
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetValue20
 *
 * Execute CMD_GET_VALUE for RTS prior to V2.1.0.
 *
 */
IEC_UINT domGetValue20(SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease)
{
	IEC_UINT uRes		= OK;

	IEC_UINT i;
	IEC_UINT uResultLen = 0;

	IEC_DATA *pResult	= NULL;
	IEC_DATA *pData 	= NULL;
	
	IEC_UINT uResult	= 0;
	IEC_UINT uResLen	= 0;
	IEC_UINT uCmdLen	= 0;

	XVariable *pxDes	= NULL;
	XVariable *pxSrc	= (XVariable *)*ppData;
	
	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}
	if (pVI->bLogin == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	/* Calculate destination length
	 */
	for (i = 0; i < *upLen / sizeof(XVariable); i++)
	{
		uResultLen = (IEC_UINT)(uResultLen + domSwap16(pVI, pxSrc->uLen));
		pxSrc++;
	}
	
	if (*upLen <= pVI->ulMaxData && uResultLen <= pVI->ulMaxData)
	{
		/* Command and response fits in one block
		 */
		IEC_DATA *pOrig = *ppData;
		IEC_UINT uOrig	= *upLen;

		uRes = domTransferData(pVI, CMD_GET_VALUE, ppData, upLen, FALSE);

		if (uRes == OK)
		{
			uRes = domSwapVar20(pVI, pOrig, uOrig, *ppData, *upLen);
		}

		if (bRelease == TRUE)
		{
			osFree(&pOrig);
		}

		RETURN(uRes);
	}
	
	
	/* Split data block
	 */
	pxSrc = (XVariable *)*ppData;
		
	for (i = 0; i < *upLen / sizeof(XVariable); i++)
	{
		if (pData == NULL)
		{
			/* Alloc new buffer
			 */
			pData = (IEC_DATA *)osMalloc(pVI->ulMaxData);
			pxDes = (XVariable *)pData;

			if (pData == NULL)
			{
				RETURN(ERR_OUT_OF_MEMORY);
			}

		}
		
		uCmdLen = (IEC_UINT)(uCmdLen + sizeof(XVariable));
		uResLen = (IEC_UINT)(uResLen + domSwap16(pVI, pxSrc->uLen));
		
		OS_MEMCPY(pxDes, pxSrc, sizeof(XVariable));
		
		pxDes++;
		pxSrc++;
		
		if (   uCmdLen + sizeof(XVariable) > pVI->ulMaxData 						/* Block full for download		*/
			|| uResLen + domSwap16(pVI, pxSrc->uLen)  > (IEC_UINT)pVI->ulMaxData	/* Block full for upload		*/
			|| i == (*upLen / sizeof(XVariable) - 1u))								/* Last block (must always fit) */
		{
			uRes = domTransferData(pVI, CMD_GET_VALUE, &pData, &uCmdLen, TRUE);
			if (uRes != OK || uResult + uCmdLen > uResultLen)
			{
				/* Communication error
				 */
				osFree(&pData);
				osFree(&pResult);

				uCmdLen = 0;
				uResult = 0;
				
				break;
			}
						
			if (pResult == NULL)
			{
				/* Alloc result buffer (for all values)
				 */
				pResult = (IEC_DATA *)osMalloc(uResultLen);
				if (pResult == NULL)
				{
					RETURN(ERR_OUT_OF_MEMORY);
				}
			}
			
			OS_MEMCPY(pResult + uResult, pData, uCmdLen);
			uResult = (IEC_UINT)(uResult + uCmdLen);
			
			osFree(&pData);

			
			uResLen = 0;
			uCmdLen = 0;
		}
	}
	
	if (uRes == OK)
	{
		uRes = domSwapVar20(pVI, *ppData, *upLen, pResult, uResult);
	}

	if (bRelease == TRUE)
	{
		osFree(ppData);
	}

	*ppData = pResult;
	*upLen	= uResult;
	
	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetValue21
 *
 * Execute CMD_GET_VALUE for RTS starting from V2.1.0.
 *
 */
IEC_UINT domGetValue21(SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease)
{
	IEC_UINT uRes		= OK;

	IEC_UINT uVarCount	= 0;
	IEC_UINT uResultLen = 0;

	IEC_DATA *pResult	= NULL;
	IEC_UINT uResult	= 0;
	
	IEC_UINT uResLen	= 0;
	IEC_UINT uCmdLen	= 0;
	
	IEC_DATA *pBuffer	= NULL;

	IEC_UINT i;

	IEC_DATA *pData 	= *ppData;
	IEC_UINT uLen		= *upLen;

	IEC_UINT uNextCmdLen = 0;
	IEC_UINT uNextResLen = 0;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}
	if (pVI->bLogin == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	while(uLen > 0)
	{
		/* Compute expected result data size
		 */
		switch(*pData & VMM_XV_TYPEMASK)
		{
			case VMM_XV_SMALL:
			{
				XVariableS *pxVar = (XVariableS *)pData;

				if (uLen < sizeof(XVariableS))
				{
					return FALSE;
				}
				uResultLen	 = (IEC_UINT)(uResultLen + (pxVar->usLen == 0 ? 1 : pxVar->usLen));
				pData		+= sizeof(XVariableS);
				uLen		-= sizeof(XVariableS);
				break;
			}

			case VMM_XV_MEDIUM:
			{
				XVariableM *pxVar = (XVariableM *)pData;

				if (uLen < sizeof(XVariableM))
				{
					return FALSE;
				}
				uResultLen	 = (IEC_UINT)(uResultLen + (pxVar->uLen == 0 ? 1 : domSwap16(pVI, pxVar->uLen)));
				pData		+= sizeof(XVariableM);
				uLen		-= sizeof(XVariableM);
				break;
			}

			case VMM_XV_LARGE:
			{
				XVariableL *pxVar = (XVariableL *)pData;

				if (uLen < sizeof(XVariableL))
				{
					return FALSE;
				}
				uResultLen	 = (IEC_UINT)(uResultLen + (pxVar->uLen == 0 ? 1 : domSwap16(pVI, pxVar->uLen)));
				pData		+= sizeof(XVariableL);
				uLen		-= sizeof(XVariableL);
				break;
			}

			default:
			{
				RETURN(ERR_ERROR);
			}
		}

		uVarCount++;
	}
	

	if (*upLen <= pVI->ulMaxData && uResultLen <= pVI->ulMaxData)
	{
		/* Command and response fits into one block
		 */
		IEC_DATA *pOrig = *ppData;
		IEC_UINT uOrig	= *upLen;

		uRes = domTransferData(pVI, CMD_GET_VALUE, ppData, upLen, FALSE);

		if (uRes == OK)
		{
			uRes = domSwapVar21(pVI, pOrig, uOrig, *ppData, *upLen);
		}

		if (bRelease == TRUE)
		{
			osFree(&pOrig);
		}

		RETURN(uRes);
	}
	
		
	pData = *ppData;
	
	for (i = 0; i < uVarCount; i++)
	{
		if (pBuffer == NULL)
		{
			/* Alloc new buffer
			 */
			pBuffer = osMalloc(pVI->ulMaxData);
			if (pBuffer == NULL)
			{
				RETURN(ERR_OUT_OF_MEMORY);
			}
		}

		switch(*pData & VMM_XV_TYPEMASK)
		{
			case VMM_XV_SMALL:
			{		
				XVariableS *pxVar = (XVariableS *)pData;
				
				OS_MEMCPY(pBuffer + uCmdLen, pData, sizeof(XVariableS));
				
				uCmdLen += sizeof(XVariableS);
				uResLen  = (IEC_UINT)(uResLen + (pxVar->usLen == 0 ? 1 : pxVar->usLen));
				pData	+= sizeof(XVariableS);
				
				break;
			}

			case VMM_XV_MEDIUM:
			{		
				XVariableM *pxVar = (XVariableM *)pData;
				
				OS_MEMCPY(pBuffer + uCmdLen, pData, sizeof(XVariableM));
				
				uCmdLen += sizeof(XVariableM);
				uResLen  = (IEC_UINT)(uResLen + (pxVar->uLen == 0 ? 1 : domSwap16(pVI, pxVar->uLen)));
				pData	+= sizeof(XVariableM);
				
				break;
			}

			case VMM_XV_LARGE:
			{		
				XVariableL *pxVar = (XVariableL *)pData;
				
				OS_MEMCPY(pBuffer + uCmdLen, pData, sizeof(XVariableL));
				
				uCmdLen += sizeof(XVariableL);
				uResLen  = (IEC_UINT)(uResLen + (pxVar->uLen == 0 ? 1 : domSwap16(pVI, pxVar->uLen)));
				pData	+= sizeof(XVariableL);
				
				break;
			}

			default:
			{
				RETURN(ERR_ERROR);
			}
		}

		uNextCmdLen = 0;
		uNextResLen = 0;

		if (i < uVarCount - 1)
		{
			switch(*pData & VMM_XV_TYPEMASK)
			{
				case VMM_XV_SMALL:
				{		
					XVariableS *pxVar = (XVariableS *)pData;

					uNextCmdLen = sizeof(XVariableS);
					uNextResLen = (IEC_UINT)(pxVar->usLen == 0 ? 1 : pxVar->usLen);
									
					break;
				}

				case VMM_XV_MEDIUM:
				{		
					XVariableM *pxVar = (XVariableM *)pData;

					uNextCmdLen = sizeof(XVariableM);
					uNextResLen = (IEC_UINT)(pxVar->uLen == 0 ? 1 : domSwap16(pVI, pxVar->uLen));
					
					break;
				}

				case VMM_XV_LARGE:
				{		
					XVariableL *pxVar = (XVariableL *)pData;

					uNextCmdLen = sizeof(XVariableL);
					uNextResLen = (IEC_UINT)(pxVar->uLen == 0 ? 1 : domSwap16(pVI, pxVar->uLen));
					
					break;
				}

				default:
				{
					RETURN(ERR_ERROR);
				}
			}
		}

		if (   uCmdLen + uNextCmdLen  > (IEC_UINT)pVI->ulMaxData	/* Block full for download		*/
			|| uResLen + uNextResLen  > (IEC_UINT)pVI->ulMaxData	/* Block full for upload		*/
			|| i == uVarCount - 1u) 								/* Last block (must always fit) */
		{
			uRes = domTransferData(pVI, CMD_GET_VALUE, &pBuffer, &uCmdLen, TRUE);
			if (uRes != OK || uResult + uCmdLen > uResultLen)
			{
				/* Communication error
				 */
				osFree(&pBuffer);
				osFree(&pResult);

				uCmdLen = 0;
				uResult = 0;

				break;
			}
						
			if (pResult == NULL)
			{
				/* Alloc result buffer (for all values)
				 */
				pResult = (IEC_DATA *)osMalloc(uResultLen);
				if (pResult == NULL)
				{
					RETURN(ERR_OUT_OF_MEMORY);
				}
			}
			
			OS_MEMCPY(pResult + uResult, pBuffer, uCmdLen);
			uResult = (IEC_UINT)(uResult + uCmdLen);

			osFree(&pBuffer);
		
			uResLen = 0;
			uCmdLen = 0;
		}
	}
		
	if (uRes == OK)
	{
		uRes = domSwapVar21(pVI, *ppData, *upLen, pResult, uResult);
	}

	if (bRelease == TRUE)
	{
		osFree(ppData);
	}

	*ppData = pResult;
	*upLen	= uResult;
	
	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domSetValue
 *
 */
IEC_UINT domSetValue(SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease)
{
	IEC_UINT uRes = OK;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}
	if (pVI->bLogin == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	if (pVI->ulFirmware <= 20500)
	{
		uRes = domSetValue20(pVI, ppData, upLen, bRelease);
	}
	else
	{
		uRes = domSetValue21(pVI, ppData, upLen, bRelease);
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domSetValue20
 *
 */
IEC_UINT domSetValue20(SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease)
{
	IEC_UINT uRes = OK;

	IEC_UINT i;
	IEC_UINT uCount 	= 0;
	IEC_UINT uToCopy	= 0;
	IEC_UINT uLen		= 0;

	XValue	 *pxVal 	= NULL;
	IEC_DATA *pData 	= NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}
	if (pVI->bLogin == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	/* Split to single commands
	 */
	uCount = 0;
	
	for (i = 0; uCount < *upLen; i++)
	{
		pxVal = (XValue *)(*ppData + uCount);

		uLen	= (IEC_UINT)(HD_VALUE + domSwap16(pVI, pxVal->VAR.uLen));
		uToCopy = uLen;
		
		pData = (IEC_DATA *)osMalloc(uLen);
		if (pData == NULL)
		{
			RETURN(ERR_OUT_OF_MEMORY);
		}

		OS_MEMCPY(pData, pxVal, uLen);

		uRes = domSwapVal(pVI, pData, uLen);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		uRes = domTransferData(pVI, CMD_SET_VALUE, &pData, &uLen, TRUE);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
		
		uCount = (IEC_UINT)(uCount + uToCopy);
	}
	
	if (bRelease == TRUE)
	{
		osFree(ppData);
		*upLen = 0;
	}
	
	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domSetValue21
 *
 */
IEC_UINT domSetValue21(SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease)
{
	IEC_UINT uRes		= OK;

	IEC_UINT uCount 	= 0;
	IEC_UINT uLen		= 0;
	IEC_UINT uToCopy	= 0;

	IEC_DATA *pData 	= NULL;
	
	XValue	 *pxVal 	= NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}
	if (pVI->bLogin == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	/* VMM can handle multiple SetValue's
	 */

	if (*upLen <= pVI->ulMaxData)
	{
		/* Command and response fits in one block
		 */
		uRes = domSwapVal(pVI, *ppData, *upLen);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		uRes = domTransferData(pVI, CMD_SET_VALUE, ppData, upLen, bRelease);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		RETURN(uRes);
	}

	while (uCount < *upLen)
	{
		uLen = 0;

		pData = (IEC_DATA *)osMalloc(pVI->ulMaxData);
		if (pData == NULL)
		{
			RETURN(ERR_OUT_OF_MEMORY);
		}

		while (uLen < pVI->ulMaxData && uCount < *upLen)
		{
			/* Build block
			 */

			pxVal = (XValue *)(*ppData + uCount);

			uToCopy = (IEC_UINT)(HD_VALUE + domSwap16(pVI, pxVal->VAR.uLen));

			if (uLen + uToCopy > (IEC_UINT)(pVI->ulMaxData))
			{
				break;
			}

			OS_MEMCPY(pData + uLen, *ppData + uCount, uToCopy);

			uLen	= (IEC_UINT)(uLen	+ uToCopy);
			uCount	= (IEC_UINT)(uCount + uToCopy);
		}

		/* Transfer block
		 */
		uRes = domSwapVal(pVI, pData, uLen);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		uRes = domTransferData(pVI, CMD_SET_VALUE, &pData, &uLen, TRUE);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

	} /* while (uCount < *upLen) */

	if (bRelease == TRUE)
	{
		osFree(ppData);
		*upLen = 0;
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetTaskState
 *
 * Execute CMD_GET_VALUE for RTS starting from V2.1.0.
 *
 */
IEC_UINT domGetResState(SVisInfo *pVI, IEC_UDINT *ulpState)
{
	IEC_UINT uRes = OK;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}
	if (pVI->bLogin == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	uRes = domGetXState(pVI, NULL, ulpState);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetTaskState
 *
 * Execute CMD_GET_VALUE for RTS starting from V2.1.0.
 *
 */
IEC_UINT domGetTaskState(SVisInfo *pVI, IEC_CHAR *szTask, IEC_UDINT *ulpState)
{
	IEC_UINT uRes = OK;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}
	if (pVI->bLogin == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	uRes = domGetXState(pVI, szTask, ulpState);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetXState
 *
 * Execute CMD_GET_VALUE for RTS starting from V2.1.0.
 *
 */
IEC_UINT domGetXState(SVisInfo *pVI, IEC_CHAR *szTask, IEC_UDINT *ulpState)
{
	IEC_UINT uRes = OK;
	IEC_UINT uLen = 0;
	IEC_UINT uType= 0;
	
	IEC_DATA  *pData;

	XVisuVar *pxVisu;

	IEC_UINT  uVSize;

	/* Alloc DBI identifier
	 */
	uLen = (IEC_UINT)(sizeof(XDBIVar) + OS_STRLEN("__state") + 1);
	uLen = (IEC_UINT)(uLen + (szTask == NULL ? 0 : sizeof(XDBIVar) + OS_STRLEN(szTask) + 1));

	pData = (IEC_DATA *)osMalloc(uLen);
	if (pData == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	/* Create DBI identifier
	 */
	uLen = 0;
	pxVisu = (XVisuVar *)(pData + uLen);

	if (szTask != NULL)
	{
		pxVisu->xVar.uInst			= NO_INDEX;
		pxVisu->xVar.usNameSize 	= (IEC_USINT)OS_STRLEN(szTask);

		OS_STRCPY(pxVisu->szName, szTask);

		uLen = (IEC_UINT)(uLen + sizeof(XDBIVar) + pxVisu->xVar.usNameSize + 1);
	}
		
	pxVisu = (XVisuVar *)(pData + uLen);

	pxVisu->xVar.uInst		= NO_INDEX;
	pxVisu->xVar.usNameSize = (IEC_USINT)OS_STRLEN("__state");

	OS_STRCPY(pxVisu->szName, "__state");

	uLen = (IEC_UINT)(uLen + sizeof(XDBIVar) + pxVisu->xVar.usNameSize + 1);


	/* Get the address of the task state variable
	 */
	uRes = domGetAddress(pVI, &pData, &uLen, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	/* Calculate variable structure size
	 */
	switch(*pData & VMM_XV_TYPEMASK)
	{
		case VMM_XV_SMALL:
			uVSize	= sizeof(XVariableS);
			break;

		case VMM_XV_MEDIUM:
			uVSize	= sizeof(XVariableM);
			break;

		case VMM_XV_LARGE:
			uVSize	= sizeof(XVariableL);
			break;
			
		default:
			osFree(&pData);
			RETURN(ERR_INVALID_DATA);
	}

	if (pVI->ulFirmware < 21030)
	{
		uType = 0;
	}
	else
	{
		/* Starting from V2.1.3 we append the data type to the normal
		 * GetAddress() data.
		 */
		uType = sizeof(IEC_UINT);
	}

	if (uLen != uVSize + uType)
	{
		osFree(&pData);
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	/* Ignore the data type
	 */
	uLen = (IEC_UINT)(uLen - uType);
	
	/* Get Value
	 */
	uRes = domGetValue(pVI, &pData, &uLen, TRUE);

	if (uLen != sizeof(IEC_UDINT))
	{
		osFree(&pData);
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	*ulpState = *(IEC_UDINT *)pData;

	osFree(&pData);

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domRetainWrite
 *
 * Writes the retain segment to the file system.
 *
 */
IEC_UINT domRetainWrite(SVisInfo *pVI)
{
	IEC_UINT uRes	= OK;

	IEC_UINT uLen	= 0;
	IEC_DATA *pData = NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	uRes = domTransferData(pVI, CMD_RET_WRITE, &pData, &uLen, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (pData != NULL)
	{
		uRes = osFree(&pData);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domRetainSetCycle
 *
 * Sets the retain write cycle time. (0 to stop.)
 *
 */
IEC_UINT domRetainSetCycle(SVisInfo *pVI, IEC_UDINT ulCycle)
{
	IEC_UINT uRes	= OK;

	IEC_UINT uLen	= 0;
	IEC_DATA *pData = NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	pData = osMalloc(sizeof(IEC_UDINT));
	if (pData == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	ulCycle = domSwap32(pVI, ulCycle);

	uLen = sizeof(IEC_UDINT);
	OS_MEMCPY(pData, &ulCycle, uLen);

	uRes = domTransferData(pVI, CMD_RET_CYCLE, &pData, &uLen, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (pData != NULL)
	{
		uRes = osFree(&pData);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
