
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
 * Filename: osFieldB.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"osFieldB.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_IO_LAYER)

/* ----  Target Specific Includes:	 ------------------------------------------ */

#include "fcDef.h"
#include "mectMain.h"

/* ----  Local Defines:   ----------------------------------------------------- */

#define OS_INVALID_HANDLE	0
#define OS_HANDLE			pthread_t

/* ----  Global Variables:	 -------------------------------------------------- */

static OS_HANDLE g_hIO[MAX_IO_LAYER];
static SIOLayerIniVal g_pIOIV[MAX_IO_LAYER];
int CanTimer;

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * osInitIOLayer
 *
 * Operating system dependent one time initializations for the IO layer 
 * interface.
 *
 * @return			OK if successful, else error number
 */
IEC_UINT osInitIOLayer(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;

	OS_MEMSET(g_hIO, 0x00, sizeof(OS_HANDLE) * MAX_IO_LAYER);
	OS_MEMSET(g_pIOIV, 0x00, sizeof(SIOLayerIniVal) * MAX_IO_LAYER);

	CanTimer = 0;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osEnableIOLayer
 *
 * With this function each downloaded IO layer can be enabled or disabled
 * individually.
 *
 * @return			OK if successful, else error number
 */
IEC_UINT osEnableIOLayer(STaskInfoVMM *pVMM, IEC_UINT uIOLayer)
{
	IEC_UINT uRes = OK;

	SIOLayer *pIOL;

	if (uIOLayer >= MAX_IO_LAYER)
	{
		RETURN(ERR_INVALID_IOLAYER);
	}

	pIOL = pVMM->pIOLayer + uIOLayer;

	/* IO Layer: BACnet
	 * ------------------------------------------------------------------------
	 */
	if (OS_STRICMP(IOEXT_BACNET, pIOL->szIOLayer) == 0)
	{
	  #if defined(RTS_CFG_BACNET)
		pIOL->bEnabled	= TRUE;
	  #else
		pIOL->bEnabled	= FALSE;
	  #endif
	
		pIOL->usNotifyRd = IO_NOTIFY_NONE;
		pIOL->usNotifyWr = IO_NOTIFY_NONE;
		
		pIOL->bAsyncConfig = TRUE;

		pIOL->uIOLType = IOID_BACNET;
	}

	/* IO Layer: Profibus DP
	 * ------------------------------------------------------------------------
	 */
	else if (OS_STRICMP(IOEXT_PROFIDP, pIOL->szIOLayer) == 0)
	{
	  #if defined(RTS_CFG_PROFI_DP)
		pIOL->bEnabled	= TRUE;
	  #else
		pIOL->bEnabled	= FALSE;
	  #endif
	
		pIOL->usNotifyRd = IO_NOTIFY | IO_NOTIFY_SYNC;
		pIOL->usNotifyWr = IO_NOTIFY | IO_NOTIFY_SYNC;
		
		pIOL->bAsyncConfig = FALSE;

		pIOL->uIOLType = IOID_PROFIDP;
	}

	/* IO Layer: Test
	 * ------------------------------------------------------------------------
	 */
	else if (OS_STRICMP(IOEXT_TEST, pIOL->szIOLayer) == 0)
	{
	  #if defined(RTS_CFG_IOTEST)
		pIOL->bEnabled	= TRUE;
	  #else
		pIOL->bEnabled	= FALSE;
	  #endif

		pIOL->usNotifyRd = IO_NOTIFY | IO_NOTIFY_SYNC;
		pIOL->usNotifyWr = IO_NOTIFY | IO_NOTIFY_SYNC;

		pIOL->bAsyncConfig = FALSE;

		pIOL->uIOLType = IOID_TEST;
	}

	/* IO Layer: CANopen
	 * ------------------------------------------------------------------------
	 */
	else if (OS_STRICMP(IOEXT_CANOPEN, pIOL->szIOLayer) == 0)
	{
	  #if defined(RTS_CFG_IOCANOPEN)
		pIOL->bEnabled	= TRUE;
	  #else
		pIOL->bEnabled	= FALSE;
	  #endif

		pIOL->usNotifyRd = IO_NOTIFY | IO_NOTIFY_SYNC;
		pIOL->usNotifyWr = IO_NOTIFY | IO_NOTIFY_SYNC;

		pIOL->bAsyncConfig = FALSE;

		pIOL->uIOLType = IOID_CANOPEN;
	}

	/* IO Layer: UDP
	 * ------------------------------------------------------------------------
	 */
	else if (OS_STRICMP(IOEXT_UDP, pIOL->szIOLayer) == 0)
	{
	  #if defined(RTS_CFG_IOUDP)
		pIOL->bEnabled	= TRUE;
	  #else
		pIOL->bEnabled	= FALSE;
	  #endif

		pIOL->usNotifyRd = IO_NOTIFY | IO_NOTIFY_SYNC;
		pIOL->usNotifyWr = IO_NOTIFY | IO_NOTIFY_SYNC;

		pIOL->bAsyncConfig = FALSE;

		pIOL->uIOLType = IOID_UDP;
	}

	/* IO Layer: DATA
	 * ------------------------------------------------------------------------
	 */
	else if (OS_STRICMP(IOEXT_DAT, pIOL->szIOLayer) == 0)
	{
	  #if defined(RTS_CFG_IODAT)
		pIOL->bEnabled	= TRUE;
	  #else
		pIOL->bEnabled	= FALSE;
	  #endif

		pIOL->usNotifyRd = IO_NOTIFY | IO_NOTIFY_SYNC;
		pIOL->usNotifyWr = IO_NOTIFY | IO_NOTIFY_SYNC;

		pIOL->bAsyncConfig = FALSE;

		pIOL->uIOLType = IOID_DAT;
	}

	/* IO Layer: SYNCRO
	 * ------------------------------------------------------------------------
	 */
	else if (OS_STRICMP(IOEXT_SYN, pIOL->szIOLayer) == 0)
	{
	  #if defined(RTS_CFG_IOSYN)
		pIOL->bEnabled	= TRUE;
	  #else
		pIOL->bEnabled	= FALSE;
	  #endif

		pIOL->usNotifyRd = IO_NOTIFY | IO_NOTIFY_SYNC;
		pIOL->usNotifyWr = IO_NOTIFY | IO_NOTIFY_SYNC;

		pIOL->bAsyncConfig = FALSE;

		pIOL->uIOLType = IOID_SYN;
	}
	/* IO Layer: KEYPAD
	 * ------------------------------------------------------------------------
	 */
	else if (OS_STRICMP(IOEXT_KEYPAD, pIOL->szIOLayer) == 0)
	{
	  #if defined(RTS_CFG_IOKEYPAD)
		pIOL->bEnabled	= TRUE;
	  #else
		pIOL->bEnabled	= FALSE;
	  #endif

		pIOL->usNotifyRd = IO_NOTIFY | IO_NOTIFY_SYNC;
		pIOL->usNotifyWr = IO_NOTIFY | IO_NOTIFY_SYNC;

		pIOL->bAsyncConfig = FALSE;

		pIOL->uIOLType = IOID_KEYPAD;
	}

	/* IO Layer: ModbusTCPS
	 * ------------------------------------------------------------------------
	 */
	else if (OS_STRICMP(IOEXT_MBTCPS, pIOL->szIOLayer) == 0)
	{
	  #if defined(RTS_CFG_IOMBTCPS)
		pIOL->bEnabled	= TRUE;
	  #else
		pIOL->bEnabled	= FALSE;
	  #endif

		pIOL->usNotifyRd = IO_NOTIFY | IO_NOTIFY_SYNC;
		pIOL->usNotifyWr = IO_NOTIFY | IO_NOTIFY_SYNC;

		pIOL->bAsyncConfig = FALSE;

		pIOL->uIOLType = IOID_MBTCPS;
	}

	/* IO Layer: ModbusRTUC
	 * ------------------------------------------------------------------------
	 */
	else if (OS_STRICMP(IOEXT_MBRTUC, pIOL->szIOLayer) == 0)
	{
	  #if defined(RTS_CFG_IOMBRTUC)
		pIOL->bEnabled	= TRUE;
	  #else
		pIOL->bEnabled	= FALSE;
	  #endif

		pIOL->usNotifyRd = IO_NOTIFY | IO_NOTIFY_SYNC;
		pIOL->usNotifyWr = IO_NOTIFY | IO_NOTIFY_SYNC;

		pIOL->bAsyncConfig = FALSE;

		pIOL->uIOLType = IOID_MBRTUC;
	}

	else
	{
		fprintf(stderr,"--- IOL: IO layer '%s' (channel: %d) Unknown.\r\n", pIOL->szIOLayer, pIOL->usChannel);
		RETURN(ERR_FB_UNKNOWN_FBUS);
	}

  #if defined(RTS_CFG_IO_TRACE)
	osTrace("--- IOL: IO layer '%s' (channel: %d) %s.\r\n", pIOL->szIOLayer, pIOL->usChannel, pIOL->bEnabled == TRUE ? "enabled" : "disabled");
  #endif
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osCreateIOLayer
 *
 * Creates an IO layer driver application. (Only if necessary; on some target
 * adaptations the drive may be a real operating system driver already integrated
 * in the operating system. So in this case it would be probably enougt to only
 * establish a connection at this point.)
 *
 * @return			OK if successful, else error number
 */
IEC_UINT osCreateIOLayer(STaskInfoVMM *pVMM, IEC_UINT uIOLayer)
{
	IEC_UINT uRes = OK;

	IEC_CHAR *szName;
	void * (* lpStartAddress)(void *) = NULL;
	
	SIOLayerIniVal *pIOIV;

	if (uIOLayer >= MAX_IO_LAYER)
	{
		RETURN(ERR_INVALID_IOLAYER);
	}
	
	pIOIV = g_pIOIV + uIOLayer;

	/* Determine main thread procedure
	 */
	szName = pVMM->pIOLayer[uIOLayer].szIOLayer;


	if (OS_STRICMP(szName, IOEXT_PROFIDP) == 0)
	{
	  #if defined(RTS_CFG_PROFI_DP)
		lpStartAddress = IO_Layer_ProfiDP;
	  #else
		lpStartAddress	= NULL;
	  #endif
	}
	else if (OS_STRICMP(szName, IOEXT_BACNET) == 0)
	{
	  #if defined(RTS_CFG_BACNET)
		lpStartAddress	= IO_Layer_BACnet;
	  #else
		lpStartAddress	= NULL;
	  #endif
	}
	else if (OS_STRICMP(szName, IOEXT_TEST) == 0)
	{
	  #if defined(RTS_CFG_IOTEST)
		lpStartAddress = IO_Layer_Test;
	  #else
		lpStartAddress	= NULL;
	  #endif
	}
	else if (OS_STRICMP(szName, IOEXT_CANOPEN) == 0)
	{
	  #if defined(RTS_CFG_IOCANOPEN)
		lpStartAddress = IO_Layer_CANopen;
	  #else
		lpStartAddress	= NULL;
	  #endif
	}
	else if (OS_STRICMP(szName, IOEXT_UDP) == 0)
	{
	  #if defined(RTS_CFG_IOUDP)
		lpStartAddress = IO_Layer_UDP;
	  #else
		lpStartAddress	= NULL;
	  #endif
	}
	else if (OS_STRICMP(szName, IOEXT_DAT) == 0)
	{
	  #if defined(RTS_CFG_IODAT)
		lpStartAddress = IO_Layer_DAT;
	  #else
		lpStartAddress	= NULL;
	  #endif
	}
	else if (OS_STRICMP(szName, IOEXT_SYN) == 0)
	{
	  #if defined(RTS_CFG_IOSYN)
		lpStartAddress = IO_Layer_SYN;
	  #else
		lpStartAddress	= NULL;
	  #endif
	}
	else if (OS_STRICMP(szName, IOEXT_KEYPAD) == 0)
	{
	  #if defined(RTS_CFG_IOKEYPAD)
		lpStartAddress = IO_Layer_Keypad;
	  #else
		lpStartAddress	= NULL;
	  #endif
	}
	else if (OS_STRICMP(szName, IOEXT_MBTCPS) == 0)
	{
	  #if defined(RTS_CFG_IOMBTCPS)
		lpStartAddress = IO_Layer_ModbusTCPS;
	  #else
		lpStartAddress	= NULL;
	  #endif
	}
	else if (OS_STRICMP(szName, IOEXT_MBRTUC) == 0)
	{
	  #if defined(RTS_CFG_IOMBRTUC)
		lpStartAddress = IO_Layer_ModbusRTUC;
	  #else
		lpStartAddress	= NULL;
	  #endif
	}
	else
	{
		RETURN(ERR_INVALID_PARAM);
	}

	if (lpStartAddress == NULL)
	{
		RETURN(ERR_INVALID_IOLAYER);
	}

	/* IO Layer initial configuration values
	 */
	pIOIV->uIOLayer 		= uIOLayer;
	pIOIV->bAsyncConfig 	= pVMM->pIOLayer[uIOLayer].bAsyncConfig;
	pIOIV->bWarmStart		= pVMM->bWarmStart;

	/* Create IO layer thread
	 */
	int iRes = osPthreadCreate(&g_hIO[uIOLayer], NULL, lpStartAddress, pIOIV, szName, 0);
	if (iRes != 0)
	{
		RETURN_e(ERR_CREATE_TASK);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osKillIOLayer
 *
 * Deletes (destroys) a given IO layer driver application if necessary. The 
 * corresponding IO task or thread already have received a MSG_TERMINATE message. 
 * However it is not guaranteed, the the task has received and processed the
 * message.
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osKillIOLayer(STaskInfoVMM *pVMM, IEC_UINT uIOLayer)
{
	if (uIOLayer >= MAX_IO_LAYER)
	{
		RETURN(ERR_INVALID_IOLAYER);
	}


	RETURN(OK);
}

#endif /* RTS_CFG_IO_LAYER */

/* ---------------------------------------------------------------------------- */
