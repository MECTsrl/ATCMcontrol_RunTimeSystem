
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
 * Filename: vmmBreak.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"vmmBreak.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#define OPC_INTERPRETER    1
  #include "intOpcds.h"
#undef	OPC_INTERPRETER

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

static IEC_UINT bpDelEntry(STaskInfoVMM *pVMM, SBPEntry *pEntry);
static SBPEntry *bpGetEntry(STaskInfoVMM *pVMM);
static SBPEntry *bpSearchEntry(STaskInfoVMM *pVMM, IEC_UDINT ulBPId);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * bpInitialize
 *
 * create and initialise structure that controll breakpoint list this 
 * procedure must be called at begin system initialisation 
 */
IEC_UINT bpInitialize(STaskInfoVMM *pVMM)
{	
	OS_MEMSET(pVMM->Shared.pBPList, 0x00, MAX_BREAKPOINTS * sizeof(SBPEntry));	

	pVMM->Shared.byFirstBP = 0xFFu;

	pVMM->ulUniqueNumber++;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * bpAddBreakpoint
 *
 */
IEC_UINT bpAddBreakpoint(STaskInfoVMM *pVMM, SBreakpoint *pBP, IEC_UDINT *pBPId)
{
	IEC_UINT	uRes	= OK;
	SBPEntry	*pEntry = NULL;
	
	uRes = bpValidateBP(pVMM, pBP);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

	OS_BEGIN_CRITICAL_SECTION(SEM_BP_LIST)
	{
		// *(IEC_UDINT UNALIGNED *)pBPId = pVMM->ulUniqueNumber++;
		OS_MEMCPY(pBPId, &pVMM->ulUniqueNumber, sizeof(IEC_UDINT)); ++pVMM->ulUniqueNumber;

		pEntry = bpGetEntry(pVMM);
		if (pEntry != NULL)
		{
			OS_MEMCPY(&pEntry->BP, pBP, sizeof(SBreakpoint));

			// pEntry->ulBPId	 = *(IEC_UDINT UNALIGNED *)pBPId;
			OS_MEMCPY(&pEntry->ulBPId, pBPId, sizeof(IEC_UDINT));

		  #if defined(RTS_CFG_ONLINE_CHANGE)
			{
				IEC_UINT i;
				SOnlChg *pOC = &pVMM->OnlChg;

				if (pVMM->DLB.uDomain == OCHG_FINISH && pOC->uCode != 0 && 
					pOC->uCode + pVMM->Project.uCode < MAX_CODE_OBJ)
				{
					for (i = 0; i < pOC->uCode; i++)
					{
						if (pOC->pCodeInd[i] == pBP->uCode)
						{
							/* In case of online change, set the breakpoint into the new 
							 * code object.
							 */
							pBP->uCode = (IEC_UINT)(pVMM->Project.uCode + i);
							break;
						}
					}
				}
			}
		  #endif

			if (pVMM->Shared.pCode[pBP->uCode].ulSize != 0)
			{
				pVMM->Shared.pCode[pBP->uCode].pAdr[pBP->uCodePos] = ASM_ACTIVE_BREAK;
			}
		}
		else
		{
			uRes = ERR_BP_LIST_FULL;
		}

	} OS_END_CRITICAL_SECTION(SEM_BP_LIST)

	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ------------------------ */
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * bpDeleteBreakpoint
 *
 */
IEC_UINT bpDeleteBreakpoint(STaskInfoVMM *pVMM, IEC_UDINT ulBPId)
{
	IEC_UINT uRes	= OK;

	IEC_UINT i;
	IEC_UINT uFound;

	SBPEntry *pList = pVMM->Shared.pBPList;

	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

	OS_BEGIN_CRITICAL_SECTION(SEM_BP_LIST)
	{
		SBPEntry *pEntry = bpSearchEntry (pVMM, ulBPId);
		
		if (pEntry == NULL)
		{
			uRes = ERR_BP_NOT_FOUND;
		}
		else
		{
			uFound = 0;
			
			for(i = 0; i < MAX_BREAKPOINTS && uFound < 2; i++ ) 			   
			{
				/* Search for other breakpoints on the same code position
				 */
				if(pList[i].ulBPId != 0 && (pList[i].BP.uCode == pEntry->BP.uCode) && 
					(pList[i].BP.uCodePos == pEntry->BP.uCodePos))
				{
					uFound++;
				}
			}
			
			if (uFound == 1 && pVMM->Shared.pCode[pEntry->BP.uCode].ulSize != 0)
			{
				/* Only delete the breakpoint in the IP code, if no other BP in the
				 * BP list points to the same code location.
				 */
				pVMM->Shared.pCode[pEntry->BP.uCode].pAdr[pEntry->BP.uCodePos] = ASM_BREAK;
			}
			
			if (bpDelEntry(pVMM, pEntry) != OK)
			{
				uRes = ERR_BP_NOT_FOUND;
			}
		}

	} OS_END_CRITICAL_SECTION(SEM_BP_LIST)

	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ------------------------ */

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * bpDeleteAllBreakpoints
 *
 */
IEC_UINT bpDeleteAllBreakpoints(STaskInfoVMM *pVMM)
{	
	SBPEntry *pEntry = NULL;
	SBPEntry *pList  = pVMM->Shared.pBPList;

	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

	OS_BEGIN_CRITICAL_SECTION(SEM_BP_LIST)
	{
		IEC_UINT i;

		for (i = pVMM->Shared.byFirstBP; i != 0xFFu; i = pList[i].byNext)
		{
			pEntry = pList + i;

			if (pVMM->Shared.pCode[pEntry->BP.uCode].ulSize != 0)
			{
				pVMM->Shared.pCode[pEntry->BP.uCode].pAdr[pEntry->BP.uCodePos] = ASM_BREAK;
			}
		}

		bpInitialize(pVMM);
	
	} OS_END_CRITICAL_SECTION(SEM_BP_LIST)
	
	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ------------------------ */

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * bpQueryBreakpoint
 *
 */
IEC_UINT bpQueryBreakpoint(SIntShared *pShared, IEC_UINT uClassId, IEC_UINT uInstP, IEC_UINT uCodePos, SBPEntry **ppBP)
{
	*ppBP = NULL;

	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

	OS_BEGIN_CRITICAL_SECTION(SEM_BP_LIST)
	{
		SBPEntry *pList  = pShared->pBPList;
		IEC_UINT  i;

		for (i = pShared->byFirstBP; i != 0xFFu; i = pList[i].byNext)
		{
			if (pList[i].ulBPId 		!= 0		&& 
				pList[i].BP.uCode		== uClassId &&
				pList[i].BP.uCodePos	== uCodePos && 
				pList[i].BP.uData		== uInstP )
			{
				*ppBP = pShared->pBPList + i;
				break;
			}
		}

	} OS_END_CRITICAL_SECTION(SEM_BP_LIST)

	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ------------------------ */

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * bpIsBPInList
 *
 */
IEC_UINT bpIsBPInList(SIntShared *pShared, IEC_UINT uClassId, IEC_UINT uCodePos, SBPEntry **ppBP)
{
	*ppBP = NULL;

	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

	OS_BEGIN_CRITICAL_SECTION(SEM_BP_LIST)
	{
		SBPEntry *pList  = pShared->pBPList;
		IEC_UINT i;

		for (i = pShared->byFirstBP; i != 0xFFu; i = pList[i].byNext)
		{
			if (pList[i].ulBPId 		!= 0		 && 
				pList[i].BP.uCode		== uClassId  &&
				pList[i].BP.uCodePos	== uCodePos )
			{
				*ppBP = pShared->pBPList + i;
				break;
			}
		}

	} OS_END_CRITICAL_SECTION(SEM_BP_LIST)

	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ------------------------ */

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * bpValidateBP
 *
 */
IEC_UINT bpValidateBP(STaskInfoVMM *pVMM, SBreakpoint *pBP)
{
	if (pBP->uCode >= pVMM->Project.uCode)
	{
		RETURN(ERR_INVALID_CLASS);
	}

	if (pBP->uData >= pVMM->Project.uData + MAX_SEGMENTS)
	{
		RETURN(ERR_INVALID_INSTANCE);
	}

	if (pBP->uCodePos >= pVMM->Shared.pCode[pBP->uCode].ulSize 
		&& pVMM->Shared.pCode[pBP->uCode].ulSize != 0)
	{
		RETURN(ERR_INVALID_CODE_POS);
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * bpGetEntry
 *
 * Get a new breakpoint list entry. Must be executed within a critical
 * section.
 */
static SBPEntry *bpGetEntry(STaskInfoVMM *pVMM)
{	 
	IEC_UINT i;
	SBPEntry *pList = pVMM->Shared.pBPList;

	for (i = 0; i < MAX_BREAKPOINTS; i++)
	{
		if (pList[i].ulBPId == 0)
		{
			pList[i].byNext 		= pVMM->Shared.byFirstBP;
			pVMM->Shared.byFirstBP	= (IEC_BYTE)i;

			return pList + i;
		}
	}
		
	return NULL;
}

/* ---------------------------------------------------------------------------- */
/**
 * bpDelEntry
 *
 * Frees a breakpoint list entry. Must be executed within a critical
 * section.
 */
static IEC_UINT bpDelEntry(STaskInfoVMM *pVMM, SBPEntry *pEntry)
{
	SBPEntry *pList = pVMM->Shared.pBPList;
	
	IEC_UINT uToDel = (IEC_UINT)(pEntry - pList);
	IEC_UINT i;
	
	if (uToDel < MAX_BREAKPOINTS)
	{
		pList[uToDel].ulBPId = 0l;

		if (uToDel == pVMM->Shared.byFirstBP)
		{
			pVMM->Shared.byFirstBP	= pList[uToDel].byNext;
			pList[uToDel].byNext	= 0xFFu;
		}
		else
		{
			for (i = pVMM->Shared.byFirstBP; i != 0xFFu; i = pList[i].byNext)
			{
				if (pList[i].byNext == (IEC_USINT)uToDel)
				{
					pList[i].byNext = pList[uToDel].byNext;
					break;
				}
			}
		}
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * bpSearchEntry
 *
 */
static SBPEntry *bpSearchEntry(STaskInfoVMM *pVMM, IEC_UDINT ulBPId)
{
	SBPEntry *pList = pVMM->Shared.pBPList;
	IEC_UINT i;
	
	for (i = pVMM->Shared.byFirstBP; i != 0xFFu; i = pList[i].byNext)
	{
		if (pList[i].ulBPId == ulBPId)
		{
			return pList + i;
		}
	}
	
	return NULL;
}

/* ---------------------------------------------------------------------------- */
