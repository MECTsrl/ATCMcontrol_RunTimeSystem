
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
 * Filename: intMain.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"intMain.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * intRun
 *
 */
IEC_UINT intRun(STaskInfoVM *pVM)
{
	SIntLocal	*pLocal = &pVM->Local;
	IEC_UINT	uRes;
	
	if (pLocal->usCurrent >= pVM->Task.usPrograms)
	{
		pLocal->usCurrent	= 0;
		pLocal->uContext	= 1;
	}
	
  #if defined(IP_CFG_NCC)
	pVM->Local.pNCC = pVM;
  #endif

	for ( ; pLocal->usCurrent < pVM->Task.usPrograms; pLocal->usCurrent++)
	{
		pLocal->Exception.uErrNo = OK;

		if (pLocal->uContext == 1)
		{
			pLocal->pContext[0].uCode = pLocal->pProg[pLocal->usCurrent].uCode;
			pLocal->pContext[0].uData = pLocal->pProg[pLocal->usCurrent].uData;
		}

		uRes = intExecute(pVM);

		if (uRes != OK)
		{
			if(uRes == WRN_BREAK)
			{
				/* Task is an active breakpoint
				 */
				pVM->Local.pState->ulState = TASK_STATE_BREAK;
				break;
			}
			
			if (uRes == WRN_TASK_HALTED)
			{
				break;
			}

			/* Fatal error (exception) during interpreter execution.
			 */
			vmSetException(pVM, pLocal->Exception.uErrNo);
			break;
		}
	}
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * intInitialize
 *
 */
IEC_UINT intInitialize(STaskInfoVM *pVM)
{
	pVM->Local.usCurrent	= 0;
	pVM->Local.uContext 	= 1;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * intSetException
 *
 * Throws an exception in the currently executed task. Note: This 
 * function must only be called within the interpreter execution!
 */
IEC_UINT intSetException(STaskInfoVM *pVM, IEC_UINT uErrNo, IEC_DATA OS_CPTR *pIP, IEC_DATA OS_DPTR *pIN)
{ 
	IEC_UINT u;
	
	SException *pEX = &pVM->Local.Exception;

	pEX->uCode = NO_INDEX;
	for (u = 0; u < pVM->pProject->uCode; u++)
	{
		if ((pIP >= pVM->pShared->pCode[u].pAdr) &&
			(pIP <	pVM->pShared->pCode[u].pAdr + pVM->pShared->pCode[u].ulSize))
		{
			pEX->uCode		= u;
			pEX->ulOffset	= (IEC_UDINT)pIP - (IEC_UDINT)pVM->pShared->pCode[u].pAdr;
			break;
		}
	}
	
	pEX->uData = NO_INDEX;
	for (u = MAX_SEGMENTS; u < MAX_SEGMENTS + pVM->pProject->uData; u++)
	{
		if ((pIN >= pVM->pShared->pData[u].pAdr) &&
			(pIN <	pVM->pShared->pData[u].pAdr + pVM->pShared->pData[u].ulSize))
		{
			pEX->uData = u;
			break;
		}
	}

	if (pEX->uData == NO_INDEX)
	{
		if ((pIN >= pVM->Local.pSP) &&
			(pIN <	pVM->Local.pSP + MAX_STACK_SIZE))
		{
			pEX->uData = 0xFFFE; /* a function */
		}
	}
	
	pEX->uErrNo = uErrNo;

	/* Restart the task from the beginning
	 */
	pVM->Local.uContext = 1;
	
  #if defined(RTS_CFG_EVENTS)
	pVM->usException = (IEC_USINT)uErrNo;
	/* osSetEvent(EVT_EXCEPTION); ... will be done in vmSetException */
  #endif

	osHandleException(pVM, pEX);
	
	return uErrNo;
}

/* ---------------------------------------------------------------------------- */
/**
 * intHalt
 *
 */
IEC_UINT intHalt(STaskInfoVM *pVM)
{
	/* Restart the task from the beginning
	 */
	pVM->Local.uContext = 1;

	return WRN_TASK_HALTED;
}

/* ---------------------------------------------------------------------------- */
