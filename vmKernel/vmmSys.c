
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
 * Filename: vmmSys.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"vmmSys.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "vmmSys.h"

#if defined(RTS_CFG_LICENSE)
  #include "md5.h"
#endif

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define RTS_BIN_KEY 	10

#define isxxdigit(c)	(((c)>='0'&&(c)<='9') || ((c)>='a'&&(c)<='f') || ((c)>='A'&&(c)<='F'))

/* ----  Global Variables:	 -------------------------------------------------- */

static IEC_BOOL g_bInitialized = FALSE;

static IEC_UINT g_uFeature	= 0;
static IEC_UINT g_uVariant	= 0;

#if defined(RTS_CFG_LICENSE)
static char hexDigits[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
#endif

/* ----  Local Functions:	--------------------------------------------------- */

#if defined(RTS_CFG_LICENSE)
  static IEC_UINT sysByteToHex(IEC_CHAR *ch, IEC_BYTE by);
  static IEC_UINT sysHexToByte(IEC_CHAR *ch, IEC_BYTE *pby);
  static IEC_UINT sysKeyToBin(IEC_CHAR *szLicKey, IEC_DATA *pLicKey);
  static IEC_UINT sysGetDataFromKey(IEC_DATA *pLicKey, IEC_UDINT *ulpProd, IEC_UDINT *ulpFeature);
  static IEC_UINT sysStrLwr(IEC_CHAR *sz);
#endif

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * sysInitialize
 *
 */
IEC_UINT sysInitialize(void)
{
	IEC_UINT uRes	 = OK;
	IEC_UINT uBuffer = 0;

	g_bInitialized	 = FALSE;

	/* ... */
	uBuffer = 0x1fffu;


	g_uVariant	= (IEC_UINT)(uBuffer & RTS_PROD_MASK);
	g_uFeature	= (IEC_UINT)(uBuffer & RTS_FB_LIC_MASK);

	g_bInitialized = TRUE;

	/* osTrace("uBuffer:0x%04x   Type:0x%04x IO:0x%04x\r\n", uBuffer, g_uVariant, g_uFeature); */

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * sysEnableIOLayer
 *
 */
IEC_UINT sysEnableIOLayer(IEC_CHAR *szName)
{
	IEC_UINT uRes = OK;

	if (g_bInitialized == FALSE)
	{
		RETURN(ERR_NOT_READY);
	}
	/* ... */


	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * sysGetFeatureAvail
 *
 */
IEC_UINT sysGetFeatureAvail(IEC_UINT *upFeature)
{
	IEC_UINT uRes = OK;

	if (g_bInitialized == FALSE)
	{
		RETURN(ERR_NOT_READY);
	}

	switch (g_uVariant)
	{
		/* ... */
		

		default:
			*upFeature = RTS_FB_LIC_MASK;
			break;
	
	} /* switch (g_uVariant) */

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * sysGetFeatureLic
 *
 */
IEC_UINT sysGetFeatureLic(IEC_UINT *upFeature)
{
	IEC_UINT uRes = OK;

	if (g_bInitialized == FALSE)
	{
		RETURN(ERR_NOT_READY);
	}

	switch (g_uVariant)
	{
		/* ... */

		default:	
			*upFeature = (IEC_UINT)(g_uFeature & RTS_FB_LIC_MASK);
			break;
	
	} /* switch (g_uVariant) */

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * sysGetVersion
 *
 */
IEC_UINT sysGetVersion(IEC_CHAR *szVersion)
{
	IEC_UINT uRes = OK;

	utilFormatString(szVersion, RTS_FULL_VERSION, PRODUCT_BUILD);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * sysGetType
 *
 */
IEC_UINT sysGetType(IEC_CHAR *szType)
{
	IEC_UINT uRes	 = OK;
	IEC_CHAR *szProd = NULL;

	if (g_bInitialized == FALSE)
	{
		RETURN(ERR_NOT_READY);
	}

	switch (g_uVariant)
	{
		/* ... */

		default:			
			szProd = "FarosPLC Embedded Windows-RTS";
			break;
	
	} /* switch (g_uVariant) */

	OS_STRCPY(szType, szProd);
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * sysGetVersionInfo
 *
 */
IEC_UINT sysGetVersionInfo(IEC_CHAR *szVersion)
{
	IEC_UINT uRes	 = OK;
	
	IEC_CHAR szProd[100];
	IEC_CHAR szVers[100];

	uRes = sysGetVersion(szVers);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = sysGetType(szProd);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	utilFormatString(szVersion, "%s %s (%s)", szProd, RTS_MAIN_VERSION, szVers);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * sysGetCommPort
 *
 */
IEC_UINT sysGetCommPort(IEC_UINT *upPort)
{
	IEC_UINT uRes = OK;

	if (g_bInitialized == FALSE)
	{
		RETURN(ERR_NOT_READY);
	}

	switch(g_uVariant)
	{
		/* ... */

		default:			
			*upPort = 17290;
			break;
	
	} /* switch (g_uVariant) */

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * sysIsAvailable
 *
 */
IEC_BOOL sysIsAvailable(IEC_UINT uFeature)
{
	return (IEC_BOOL)((g_uFeature & uFeature) != 0 ? TRUE : FALSE);
}

/* ---------------------------------------------------------------------------- */
/**
 * sysGetMacAddr
 *
 */
IEC_UINT sysGetMacAddr(IEC_CHAR pMacAddr[6])
{
	IEC_UINT uRes = OK;

	static IEC_CHAR MacAddr[6];
	static IEC_BOOL bMacRead	= FALSE;

	if (bMacRead == FALSE)
	{
		/* ... */

		OS_MEMSET(pMacAddr, 0xff, 6);

		bMacRead = TRUE;
	
	} /* if (bMacRead == FALSE) */

	OS_MEMCPY(pMacAddr, MacAddr, sizeof(MacAddr));

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * sysGetSerialNo
 *
 */
IEC_UINT sysGetSerialNo(IEC_UDINT *ulpSerialNo)
{
	IEC_UINT uRes = OK;

	static IEC_UDINT ulSerialNo = 0xffffffffu;

	if (ulSerialNo == 0xffffffffu)
	{
		/* ... */

		ulSerialNo = 0x12345678u;
	
	} /* if (ulSerialNo == 0xffffffffu) */

	*ulpSerialNo = ulSerialNo;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * sysGetInstKey
 *
 */
#if defined(RTS_CFG_LICENSE)

IEC_UINT sysGetInstKey(IEC_CHAR *szInstKey)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}

#endif /* RTS_CFG_LICENSE */

/* ---------------------------------------------------------------------------- */
/**
 * sysSetLicKey
 *
 */
#if defined(RTS_CFG_LICENSE)

IEC_UINT sysSetLicKey(IEC_CHAR *szLicKey)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}

#endif /* RTS_CFG_LICENSE */

/* ---------------------------------------------------------------------------- */
/**
 * sysByteToHex
 *
 */
#if defined(RTS_CFG_LICENSE)

static IEC_UINT sysByteToHex(IEC_CHAR *ch, IEC_BYTE by)
{
	ch[0] = hexDigits[(by>>4) & 0x0f];
	ch[1] = hexDigits[by & 0x0f];

	RETURN(OK);
}

#endif /* RTS_CFG_LICENSE */

/* ---------------------------------------------------------------------------- */
/**
 * sysHexToByte
 *
 */
#if defined(RTS_CFG_LICENSE)

static IEC_UINT sysHexToByte(IEC_CHAR *ch, IEC_BYTE *pby)
{
	char hh[3];
	
	if(!(isxxdigit(ch[0]) && isxxdigit(ch[1])))
	{
		RETURN(ERR_INVALID_PARAM);
	}

	hh[0] = ch[0];
	hh[1] = ch[1];
	hh[2] = 0;

	*pby = (IEC_BYTE)(OS_STRTOUL(hh, NULL, 16) & 0xff);
	
	RETURN(OK);
}

#endif /* RTS_CFG_LICENSE */

/* ---------------------------------------------------------------------------- */
/**
 * sysKeyToBin
 *
 */
#if defined(RTS_CFG_LICENSE)

static IEC_UINT sysKeyToBin(IEC_CHAR *szLicKey, IEC_DATA *pLicKey)
{
	IEC_UINT uRes = OK;

	
	RETURN(uRes);
}

#endif /* RTS_CFG_LICENSE */

/* ---------------------------------------------------------------------------- */
/**
 * sysGetDataFromKey
 *
 */
#if defined(RTS_CFG_LICENSE)

static IEC_UINT sysGetDataFromKey(IEC_DATA *pLicKey, IEC_UDINT *ulpProd, IEC_UDINT *ulpFeature)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}

#endif /* RTS_CFG_LICENSE */

/* ---------------------------------------------------------------------------- */
/**
 * sysStrLwr
 *
 */
#if defined(RTS_CFG_LICENSE)

static IEC_UINT sysStrLwr(IEC_CHAR *sz)
{
	IEC_UINT uRes = OK;

	while (sz != NULL && *sz != '\0')
	{
		if (*sz >= 'A' && *sz <= 'Z')
		{
			*sz = (IEC_CHAR)(*sz - 'A' + 'a');
		}

		sz++;
	}

	RETURN(uRes);
}

#endif /* RTS_CFG_LICENSE */

/* ---------------------------------------------------------------------------- */
