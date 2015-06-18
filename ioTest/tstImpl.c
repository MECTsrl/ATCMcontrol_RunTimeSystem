
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
 * Filename: tstImpl.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"tstImpl.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_IOTEST)

#include "tstMain.h"
#include "iolDef.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

static IEC_BOOL g_bInitialized	= FALSE;
static IEC_BOOL g_bConfigured	= FALSE;
static IEC_BOOL g_bRunning		= FALSE;

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * tstInitialize
 *
 */
IEC_UINT tstInitialize(IEC_UINT uIOLayer)
{
	IEC_UINT uRes = OK;
	
  #if defined(RTS_CFG_SYSLOAD)
	uRes = ldWriteTaskInfo(TASK_OFFS_IOL_TST, osGetTaskID());
	TR_RET(uRes);
  #endif

	uRes = tstInitBus(uIOLayer);

	g_bInitialized	= (IEC_BOOL)(uRes == OK);
	g_bConfigured	= FALSE;
	g_bRunning		= FALSE;
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstFinalize
 *
 */
IEC_UINT tstFinalize(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;
	
	uRes = tstFinalBus(uIOLayer);
	TR_RET(uRes);

  #if defined(RTS_CFG_SYSLOAD)
	uRes = ldClearTaskInfo(TASK_OFFS_IOL_TST);
	TR_RET(uRes);
  #endif

	g_bInitialized	= FALSE;
	g_bConfigured	= FALSE;
	g_bRunning		= FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstNotifyConfig
 *
 */
IEC_UINT tstNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;
	
	uRes = tstConfigBus(uIOLayer, pIO);

	g_bConfigured	= (IEC_BOOL)(uRes == OK);
	g_bRunning		= FALSE;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstNotifyStart
 *
 */
IEC_UINT tstNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

	g_bRunning = (IEC_BOOL)(uRes == OK);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstNotifyStop
 *
 */
IEC_UINT tstNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO)
{
	IEC_UINT uRes = OK;

	g_bRunning = FALSE;
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstNotifySet
 *
 */
IEC_UINT tstNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;

	IEC_DATA OS_DPTR *pOut	= NULL;
	IEC_DATA OS_DPTR *pX	= NULL;
	
	if (tstGetBusState(uIOLayer) != FB_STATE_OPERATING)
	{
		/* PB is not operating
		 */
		RETURN(ERR_FB_NOT_OPERATING);
	}
	
	if (pNotify->uTask != 0xffffu)
	{
		/* A IEC task has written into the output segment.
		 * --------------------------------------------------------------------
		 */
		IEC_UDINT r;
	  #if defined(RTS_CFG_WRITE_FLAGS)
		IEC_UDINT j;
	  #endif
		IEC_UDINT x;

		IEC_UDINT ulStart;
		IEC_UDINT ulStop;

		SImageReg	*pIR = ((SImageReg *)pIO->C.pAdr) + pNotify->uTask;
		SIOSegment	*pIOS;

	  #if defined(RTS_CFG_TASK_IMAGE)
		if (pIR->pSetQ[uIOLayer] == FALSE)
		{
			/* Nothing to do
			 */
			RETURN(OK);
		}
	  #endif
		
		for (x = 0; uRes == OK && x <= 1; x++)
		{
			uRes = (IEC_UINT)(x == 0 ? tstLockWriteFirst(uIOLayer, &pOut) : tstLockWriteSecond(uIOLayer, &pOut));

			for (r = 0; uRes == OK && r < pIR->uRegionsWr; r++)
			{
				SRegion *pReg = pIR->pRegionWr + r;
				
				if (pReg->pSetQ[uIOLayer] == FALSE)
				{
					/* Areas don't match
					 */
					continue;
				}

				pIOS	= &pIO->Q;
				pX		= pOut;

				ulStart = vmm_max(pReg->ulOffset, pIOS->ulOffs);
				ulStop	= vmm_min(pReg->ulOffset + pReg->uSize, pIOS->ulOffs + pIOS->ulSize);

				if (ulStart >= ulStop)
				{
					/* Areas don't match. (Should not happen, because already checked above!)
					 */
					continue;
				}

				if (x == 0)
				{
					TR_IWR7("Seg %s:  O:%6x L:%6x   Chn: O:%6x L:%6x   Reg: O:%6x L:%6x",
						"Q", ulStart, ulStop - ulStart, 
						pIOS->ulOffs, pIOS->ulSize, pReg->ulOffset, pReg->uSize);
				}

			  #if ! defined(RTS_CFG_WRITE_FLAGS)

				OS_MEMCPY(pX + (ulStart - pIOS->ulOffs), pIOS->pAdr + ulStart, ulStop - ulStart);

			  #else

				for (j = ulStart; j < ulStop; j++)
				{
					pX[j - pIOS->ulOffs] = (IEC_DATA)((pX[j - pIOS->ulOffs] & ~pIO->W.pAdr[j]) | (pIOS->pAdr[j] & pIO->W.pAdr[j]));

					if (x == 1)
					{
						pIO->W.pAdr[j] = 0;
					}
				}

			  #endif /* ! RTS_CFG_WRITE_FLAGS */
			
			} /* for (r = 0; uRes == OK && r < pIR->uRegionsWr; r++) */

		} /* for (x = 0; uRes == OK && x <= 1; x++) */

		if (uRes == OK)
		{
			uRes = tstReleaseWrite(uIOLayer);
		}
	
	} /* if (pNotify->uTask != 0xffffu) */ 

	else
	{
		/* An external application (i.e. the 4C Watch) has written
		 * into the output or input segment.
		 * --------------------------------------------------------------------
		 */

		IEC_UDINT x;
		
		SIOSegment *pIOS = pNotify->usSegment == SEG_OUTPUT ? &pIO->Q : &pIO->I;

		IEC_UDINT ulStart	= vmm_max(pNotify->ulOffset, pIOS->ulOffs);
		IEC_UDINT ulStop	= vmm_min(pNotify->ulOffset + pNotify->uLen, pIOS->ulOffs + pIOS->ulSize);

		IEC_UINT  uM		= (IEC_UINT)(1u << (pNotify->usBit - 1u));

		if (ulStart >= ulStop)
		{
			/* Variable value memory area and associated IO memory area doesn't match
			 */
			RETURN(OK);
		}

		if (pNotify->usSegment != SEG_OUTPUT) 
		{
			/* Input values can't be written
			 */
			RETURN(ERR_WRITE_TO_INPUT);
		}

		for (x = 0; uRes == OK && x <= 1; x++)
		{
			uRes = (IEC_UINT)(x == 0 ? tstLockWriteFirst(uIOLayer, &pOut) : tstLockWriteSecond(uIOLayer, &pOut));

			if (uRes == OK)
			{
				pX = pOut;

				TR_WWR7("Seg %s:  O:%6x L:%6x   Chn: O:%6x L:%6x   Ext: O:%6x L:%6x",
					"Q", ulStart, ulStop - ulStart, 
					pIOS->ulOffs, pIOS->ulSize, pNotify->ulOffset, pNotify->uLen);

				if (pNotify->usBit == 0)
				{
					OS_MEMCPY(pX + (ulStart - pIOS->ulOffs), pIOS->pAdr + ulStart, ulStop - ulStart);
				}
				else
				{		
					pX[ulStart - pIOS->ulOffs] = (IEC_DATA) ( (pX[ulStart - pIOS->ulOffs] & ~uM) | (pIOS->pAdr[ulStart] & uM) ); 
				}

			} /* if (uRes == OK) */

		} /* for (x = 0; uRes == OK && x <= 1; x++) */
		
		if (uRes == OK)
		{
			uRes = tstReleaseWrite(uIOLayer);
		}
		
	} /* else (pNotify->uTask != 0xffffu) */	

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * tstNotifyGet
 *
 */
IEC_UINT tstNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify)
{
	IEC_UINT uRes = OK;

	IEC_DATA OS_DPTR *pIn	= NULL;
	IEC_DATA OS_DPTR *pOut	= NULL;
	IEC_DATA OS_DPTR *pX	= NULL;

	if (tstGetBusState(uIOLayer) != FB_STATE_OPERATING)
	{
		/* PB is not operating
		 */
		RETURN(ERR_FB_NOT_OPERATING);
	}

	if (pNotify->uTask != 0xffffu)
	{
		/* A IEC task is going to read from the input and/or output segment.
		 * --------------------------------------------------------------------
		 */ 
		IEC_UINT	r;
		
		IEC_UDINT	ulStart;
		IEC_UDINT	ulStop;

		SImageReg	*pIR  = ((SImageReg *)pIO->C.pAdr) + pNotify->uTask;
		SIOSegment	*pIOS;

		if (pIR->pGetQ[uIOLayer] == FALSE && pIR->pGetI[uIOLayer] == FALSE)
		{
			/* Nothing to do
			 */
			RETURN(OK);
		}

		uRes = tstLockRead(uIOLayer, &pIn, &pOut);
		
		for (r = 0; uRes == OK && r < pIR->uRegionsRd; r++)
		{
			SRegion *pReg = pIR->pRegionRd + r;

			if (pReg->pGetQ[uIOLayer] == FALSE && pReg->pGetI[uIOLayer] == FALSE)
			{
				/* Areas don't match
				 */
				continue;
			}

			pIOS	= pReg->usSegment == SEG_OUTPUT ? &pIO->Q : &pIO->I;
			pX		= pReg->usSegment == SEG_OUTPUT ? pOut	   : pIn;

			ulStart = vmm_max(pReg->ulOffset, pIOS->ulOffs);
			ulStop	= vmm_min(pReg->ulOffset + pReg->uSize, pIOS->ulOffs + pIOS->ulSize);

			if (ulStart >= ulStop)
			{
				/* Areas don't match. (Should not happen, because already checked above!)
				 */
				continue;
			}

			TR_IRD7("Seg %s:  O:%6x L:%6x   Chn: O:%6x L:%6x   Reg: O:%6x L:%6x",
				pReg->usSegment == SEG_INPUT ? "I" : "Q", ulStart, ulStop - ulStart, 
				pIOS->ulOffs, pIOS->ulSize, pReg->ulOffset, pReg->uSize);

			OS_MEMCPY(pIOS->pAdr + ulStart, pX + (ulStart - pIOS->ulOffs), ulStop - ulStart);
		}

		if (uRes == OK)
		{
			uRes = tstReleaseRead(uIOLayer);
		}

	} /* if (pNotify->uTask != 0xffffu) */ 

	else
	{
		/* An external application (r.e. the 4C Watch) is going to read from
		 * the input and/or output segment.
		 * --------------------------------------------------------------------
		 */

		SIOSegment *pIOS = pNotify->usSegment == SEG_OUTPUT ? &pIO->Q : &pIO->I;

		IEC_UDINT ulStart	= vmm_max(pNotify->ulOffset, pIOS->ulOffs);
		IEC_UDINT ulStop	= vmm_min(pNotify->ulOffset + pNotify->uLen, pIOS->ulOffs + pIOS->ulSize);

		IEC_UINT  uM		= (IEC_UINT)(1u << (pNotify->usBit - 1u));

		if (ulStart >= ulStop)
		{
			/* Variable value memory area and associated IO memory area doesn't match
			 */
			RETURN(OK);;
		}
	
		uRes = tstLockRead(uIOLayer, &pIn, &pOut);
		
		if (uRes == OK)
		{
			pX = pNotify->usSegment == SEG_INPUT ? pIn : pOut;

			TR_WRD7("Seg %s:  O:%6x L:%6x   Chn: O:%6x L:%6x   Ext: O:%6x L:%6x",
				pNotify->usSegment == SEG_INPUT ? "I" : "Q", ulStart, ulStop - ulStart, 
				pIOS->ulOffs, pIOS->ulSize, pNotify->ulOffset, pNotify->uLen);

			if (pNotify->usBit == 0)
			{
				OS_MEMCPY(pIOS->pAdr + ulStart, pX + (ulStart - pIOS->ulOffs), ulStop - ulStart);	
			}
			else
			{		
				pIOS->pAdr[ulStart] = (IEC_DATA) ( (pIOS->pAdr[ulStart] & ~uM) | (pX[ulStart - pIOS->ulOffs] & uM) );
			}

		} /* if (uRes == OK) */

		if (uRes == OK)
		{
			uRes = tstReleaseRead(uIOLayer);
		}

	} /* else (pNotify->uTask != 0xffffu) */

	RETURN(uRes);
}

#endif /* RTS_CFG_IOTEST */

/* ---------------------------------------------------------------------------- */
