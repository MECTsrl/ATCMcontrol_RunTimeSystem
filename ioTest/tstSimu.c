
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
 * Filename: tstSimu.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"tstSimu.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_IOTEST)

#include "tstMain.h"
#include "iolDef.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define FBSIM_I_SIZE	0x00040000ul
#define FBSIM_Q_SIZE	0x00040000ul

/* ----  Global Variables:	 -------------------------------------------------- */

typedef struct 
{
	IEC_UDINT version;				/* Version: Major, Minor, Reserved, Reserved		*/
	IEC_UDINT last_in;				/* Last buffer used by driver for input/state data	*/
	IEC_UDINT lock_in;				/* User locks Buffer 0/1 for Input Data 			*/
	IEC_UDINT lock_out; 			/* User locks Buffer 0/1 for Output Data			*/
	IEC_UDINT InOffset[2];			/* Offset from start of this structure for Buffer x */
	IEC_UDINT OutOffset[2]; 		/* Offset from start of this structure for Buffer x */
	IEC_UDINT count_in[2];			/* Incremented when new data is available			*/
	IEC_UDINT count_out[2]; 		/* Incremented when new data is available			*/
	IEC_UDINT jiff_in[2];			/* Last time input-buffers	are accessed jiffies	*/
	IEC_UDINT jiff_out[2];			/* Last time output-buffers are accessed jiffies	*/
	IEC_DATA  state[2][0x80];		/* StateBuffer for Profibus stations				*/

} SDBMap;

IEC_DATA *g_ppImage[MAX_IO_LAYER];
IEC_UINT g_pAccess[MAX_IO_LAYER];

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * tstInitBus
 *
 */
IEC_UINT tstInitBus(IEC_UINT uIOLayer)
{
	IEC_UINT uRes = OK;

	static IEC_BOOL bFirst = TRUE;

	SDBMap *pMap = NULL;

	if (bFirst == TRUE)
	{
		OS_MEMSET(g_ppImage, 0x00, sizeof(IEC_DATA *) * MAX_IO_LAYER);
		OS_MEMSET(g_pAccess, 0x00, sizeof(IEC_UINT) * MAX_IO_LAYER);

		bFirst = FALSE;
	}

	if (uIOLayer >= MAX_IO_LAYER)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	tstFinalBus(uIOLayer);
	
	g_ppImage[uIOLayer] = osMalloc(sizeof(SDBMap) + 2 * FBSIM_I_SIZE + 2 * FBSIM_Q_SIZE);
	if (g_ppImage[uIOLayer] == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	OS_MEMSET(g_ppImage[uIOLayer], 0x00, sizeof(SDBMap) + 2 * FBSIM_I_SIZE + 2 * FBSIM_Q_SIZE);

	pMap = (SDBMap *)g_ppImage[uIOLayer];

	pMap->InOffset[0]	= sizeof(SDBMap);
	pMap->OutOffset[0]	= pMap->InOffset[0] 	+ FBSIM_I_SIZE;
	pMap->InOffset[1]	= pMap->OutOffset[0]	+ FBSIM_Q_SIZE;
	pMap->OutOffset[1]	= pMap->InOffset[1] 	+ FBSIM_I_SIZE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstFinalBus
 *
 */
IEC_UINT tstFinalBus(IEC_UINT uIOLayer)
{
	IEC_UINT uRes = OK;

	if (uIOLayer >= MAX_IO_LAYER)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	uRes = osFree(g_ppImage + uIOLayer);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstConfigBus
 *
 */
IEC_UINT tstConfigBus(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

	SDBMap *pMap = (SDBMap *)g_ppImage[uIOLayer];
	
	OS_MEMCPY(g_ppImage[uIOLayer] + pMap->OutOffset[0], pIO->Q.pAdr + pIO->Q.ulOffs, vmm_min(FBSIM_Q_SIZE, pIO->Q.ulSize));
	OS_MEMCPY(g_ppImage[uIOLayer] + pMap->OutOffset[1], pIO->Q.pAdr + pIO->Q.ulOffs, vmm_min(FBSIM_Q_SIZE, pIO->Q.ulSize));
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstLockRead
 *
 */
IEC_UINT tstLockRead(IEC_UINT uIOLayer, IEC_DATA OS_DPTR **ppIn, IEC_DATA OS_DPTR **ppOut)
{
	IEC_UINT uRes = OK;

	SDBMap *pMap = (SDBMap *)g_ppImage[uIOLayer];

	*ppIn  = NULL;
	*ppOut = NULL;

	if (uIOLayer >= MAX_IO_LAYER || g_ppImage[uIOLayer] == NULL)
	{
		RETURN(ERR_FB_NOT_INITIALIZED);
	}

	if (g_pAccess[uIOLayer] > 0)
	{
		RETURN(ERR_INVALID_DATA);
	}

	g_pAccess[uIOLayer]++;

	pMap->lock_in = pMap->last_in;

	*ppIn  = g_ppImage[uIOLayer] + pMap->InOffset[pMap->lock_in];
	*ppOut = g_ppImage[uIOLayer] + pMap->OutOffset[pMap->lock_out];
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstReleaseRead
 *
 */
IEC_UINT tstReleaseRead(IEC_UINT uIOLayer)
{
	IEC_UINT uRes = OK;

	if (uIOLayer >= MAX_IO_LAYER || g_ppImage[uIOLayer] == NULL)
	{
		RETURN(ERR_FB_NOT_INITIALIZED);
	}

	g_pAccess[uIOLayer]--;
	
	if (g_pAccess[uIOLayer] > 0)
	{
		RETURN(ERR_INVALID_DATA);
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstLockWriteFirst
 *
 */
IEC_UINT tstLockWriteFirst(IEC_UINT uIOLayer, IEC_DATA OS_DPTR **ppOut)
{
	IEC_UINT uRes = OK;

	SDBMap *pMap = (SDBMap *)g_ppImage[uIOLayer];

	*ppOut = NULL;

	if (uIOLayer >= MAX_IO_LAYER || g_ppImage[uIOLayer] == NULL)
	{
		RETURN(ERR_FB_NOT_INITIALIZED);
	}

	if (g_pAccess[uIOLayer] > 0)
	{
		RETURN(ERR_INVALID_DATA);
	}

	g_pAccess[uIOLayer]++;

	*ppOut = g_ppImage[uIOLayer] + pMap->OutOffset[pMap->lock_out];

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstLockWriteSecond
 *
 */
IEC_UINT tstLockWriteSecond(IEC_UINT uIOLayer, IEC_DATA OS_DPTR **ppOut)
{
	IEC_UINT uRes = OK;

	SDBMap *pMap = (SDBMap *)g_ppImage[uIOLayer];

	*ppOut = NULL;

	if (uIOLayer >= MAX_IO_LAYER || g_ppImage[uIOLayer] == NULL)
	{
		RETURN(ERR_FB_NOT_INITIALIZED);
	}

	if (g_pAccess[uIOLayer] != 1)
	{
		RETURN(ERR_INVALID_DATA);
	}

	g_pAccess[uIOLayer]++;

	pMap->count_out[pMap->lock_out]++;

	pMap->lock_out = pMap->lock_out == 0 ? 1 : 0;

	*ppOut = g_ppImage[uIOLayer] + pMap->OutOffset[pMap->lock_out];

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstReleaseWrite
 *
 */
IEC_UINT tstReleaseWrite(IEC_UINT uIOLayer)
{
	IEC_UINT uRes = OK;

	SDBMap *pMap = (SDBMap *)g_ppImage[uIOLayer];

	if (uIOLayer >= MAX_IO_LAYER || g_ppImage[uIOLayer] == NULL)
	{
		RETURN(ERR_FB_NOT_INITIALIZED);
	}

	pMap->count_out[pMap->lock_out]++;

	g_pAccess[uIOLayer]--;
	g_pAccess[uIOLayer]--;
	
	if (g_pAccess[uIOLayer] > 0)
	{
		RETURN(ERR_INVALID_DATA);
	}

	{
		IEC_DATA *pIn	= g_ppImage[uIOLayer] + pMap->InOffset[pMap->last_in == 0 ? 1 : 0];
		IEC_DATA *pOut	= g_ppImage[uIOLayer] + pMap->OutOffset[pMap->lock_out == 0 ? 1 : 0];

		OS_MEMCPY(pIn, pOut, vmm_min(FBSIM_I_SIZE, FBSIM_Q_SIZE));

		pMap->last_in = pMap->last_in == 0 ? 1 : 0;
		pMap->count_in[pMap->last_in]++;
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstGetBusState
 *
 */
IEC_UINT tstGetBusState(IEC_UINT uIOLayer)
{
	return FB_STATE_OPERATING;
}

#endif /* RTS_CFG_IOTEST */

/* ---------------------------------------------------------------------------- */
