
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
 * Filename: visLic.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"visLic.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "visDef.h"
#include "visMain.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * domGetInstKey
 *
 */
IEC_UINT domGetInstKey(SVisInfo *pVI, IEC_DATA	**ppKey, IEC_UINT *upLen)
{
	IEC_UINT uRes	= OK;

	IEC_UINT uLen	= 0;
	IEC_DATA *pData = NULL;

	XInstKey *pIK	= NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	uRes = domTransferData(pVI, CMD_GET_INSTKEY, &pData, &uLen, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	if (uLen != sizeof(XInstKey))
	{
		osFree(&pData);
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	pIK = (XInstKey *)pData;

	*upLen = (IEC_UINT)(OS_STRLEN(pIK->szInstKey) + 1);
	*ppKey = osMalloc(*upLen);

	if (*ppKey == NULL)
	{
		osFree(&pData);
		RETURN(ERR_OUT_OF_MEMORY);
	}

	OS_MEMCPY(*ppKey, pIK->szInstKey, *upLen);

	uRes = osFree(&pData);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domSetLicKey
 *
 */
IEC_UINT domSetLicKey (SVisInfo *pVI, IEC_DATA	*pKey, IEC_UINT uLen)
{
	IEC_UINT uRes = OK;

	IEC_DATA *pData  = NULL; 
	IEC_UINT uDatLen = 0;

	XLicKey *pLK	 = NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	uDatLen = sizeof(XLicKey);

	pData = osMalloc(uDatLen);
	if (pData == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	pLK = (XLicKey *)pData;

	OS_STRCPY(pLK->szLicKey, pKey);

	uRes = domTransferData(pVI, CMD_SET_LICKEY, &pData, &uDatLen, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetSerialNo
 *
 */
IEC_UINT domGetSerialNo (SVisInfo *pVI, IEC_UDINT *ulpSN)
{
	IEC_UINT uRes	= OK;

	IEC_UINT uLen	= 0;
	IEC_DATA *pData = NULL;

	XSerialNo *pSN	= NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	uRes = domTransferData(pVI, CMD_GET_SERIALNO, &pData, &uLen, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (uLen != sizeof(XSerialNo))
	{
		osFree(&pData);
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	pSN = (XSerialNo *)pData;

	*ulpSN = domSwap32(pVI, pSN->ulSerialNo);

	uRes = osFree(&pData);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetFeature
 *
 */
IEC_UINT domGetFeature (SVisInfo *pVI, IEC_UINT  *upAvailable, IEC_UINT *upLicensed)
{
	IEC_UINT uRes	= OK;

	IEC_UINT uLen	= 0;
	IEC_DATA *pData = NULL;

	XFeatDef *pFD	= NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	uRes = domTransferData(pVI, CMD_GET_FEATURE, &pData, &uLen, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (uLen != sizeof(XFeatDef))
	{
		osFree(&pData);
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	pFD = (XFeatDef *)pData;

	*upAvailable = domSwap16(pVI, pFD->uAvailable);
	*upLicensed  = domSwap16(pVI, pFD->uLicensed);

	uRes = osFree(&pData);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetTargetType
 *
 */
IEC_UINT domGetTargetType (SVisInfo *pVI, IEC_DATA	**ppType, IEC_UINT *upLen)
{
	IEC_UINT uRes	= OK;

	IEC_UINT uLen	= 0;
	IEC_DATA *pData = NULL;

	XTypeInfo *pTI	= NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	uRes = domTransferData(pVI, CMD_GET_TYPE, &pData, &uLen, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (uLen != sizeof(XTypeInfo))
	{
		osFree(&pData);
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	pTI = (XTypeInfo *)pData;

	*upLen	= (IEC_UINT)(OS_STRLEN(pTI->szType) + 1);
	*ppType = osMalloc(*upLen);

	if (*ppType == NULL)
	{
		osFree(&pData);
		RETURN(ERR_OUT_OF_MEMORY);
	}

	OS_MEMCPY(*ppType, pTI->szType, *upLen);

	uRes = osFree(&pData);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domGetTargetVersion
 *
 */
IEC_UINT domGetTargetVersion (SVisInfo *pVI, IEC_DATA  **ppVersion, IEC_UINT *upLen)
{
	IEC_UINT uRes	= OK;
	
	IEC_UINT uLen	= 0;
	IEC_DATA *pData = NULL;

	XVersionInfo *pVE = NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	uRes = domTransferData(pVI, CMD_GET_VERSION, &pData, &uLen, FALSE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (uLen != sizeof(XVersionInfo))
	{
		osFree(&pData);
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	pVE = (XVersionInfo *)pData;

	*upLen		= (IEC_UINT)(OS_STRLEN(pVE->szFull) + 1);
	*ppVersion	= osMalloc(*upLen);

	if (*ppVersion == NULL)
	{
		osFree(&pData);
		RETURN(ERR_OUT_OF_MEMORY);
	}

	OS_MEMCPY(*ppVersion, pVE->szFull, *upLen);

	uRes = osFree(&pData);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domSetLicEx
 *
 */
IEC_UINT domSetLicEx (SVisInfo *pVI, IEC_DATA	*pKey, IEC_UINT uLen)
{
	IEC_UINT uRes = OK;

	IEC_DATA *pData = NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	pData = osMalloc(uLen);
	if (pData == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	OS_MEMCPY(pData, pKey, uLen);

	*(IEC_UINT *)(pData + 0)				= domSwap16(pVI, *(IEC_UINT *)(pData + 0));
	*(IEC_UINT *)(pData + sizeof(IEC_UINT))	= domSwap16(pVI, *(IEC_UINT *)(pData + sizeof(IEC_UINT)));

	uRes = domTransferData(pVI, CMD_SET_LICEX, &pData, &uLen, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
