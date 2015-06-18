
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
 * Filename: intLevel.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"intLevel.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include <math.h>

#define OPC_INTERPRETER    1
  #include "intOpcds.h"
#undef	OPC_INTERPRETER

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define pIP 	  (*ppIP)
#define pIN 	  (*ppIN)
#define pSP 	  (UNALIGNED *ppSP)

/* ----  Global Variables:	 -------------------------------------------------- */

#if defined(RTS_CFG_CUSTOMER_LIB)

extern EXECUTE_FUN g_pTargetFun[];
extern EXECUTE_FB  g_pTargetFB [];

#endif

#if defined(RTS_CFG_SYSTEM_LIB) || defined(RTS_CFG_SYSTEM_LIB_NT) || defined(RTS_CFG_UTILITY_LIB)

extern EXECUTE_FUN	g_pLibraryFun[];
extern EXECUTE_FB	g_pLibraryFB [];

#endif

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * intConvert
 *
 */
#if IP_CFG_CLEVEL >= IP_CFG_CLEVEL_CONV

IEC_UINT intConvert(STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP)
{
	/* Opcodes with PREF_CONV 
	 */
	IEC_DATA OS_DPTR *p;

	switch (*(pIP++))
	{
		#include "intConv.c"

		default:
			return intSetException(pVM, EXCEPT_ILLEGAL_OPCODE, pIP, pIN);
	}

	RETURN(OK);

	p++;
}

#endif

/* ---------------------------------------------------------------------------- */
/**
 * intArith
 *
 */
#if IP_CFG_CLEVEL >= IP_CFG_CLEVEL_ARITH

IEC_UINT intArith(STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP)
{	
	/* Opcodes with PREF_ARITH 
	 */
	IEC_DATA OS_DPTR *p;
	
	switch (*(pIP++))
	{
		#include "intArith.c"

		default:
			return intSetException(pVM, EXCEPT_ILLEGAL_OPCODE, pIP, pIN);
	}
	RETURN(OK);
	
	p;
}

#endif

/* ---------------------------------------------------------------------------- */
/**
 * intWide16Ops
 *
 */
#if IP_CFG_CLEVEL >= IP_CFG_CLEVEL_WIDE16

IEC_UINT intWide16Ops(STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP)
{
	/* Opcodes with PREF_WIDE16 
	 */
	IEC_DATA OS_DPTR *pSeg;
	IEC_DATA OS_DPTR *p;
	
	SIntShared *pShared = pVM->pShared;
	SIntLocal  *pLocal	= &pVM->Local;
	
	switch (*(pIP++))
	{
		#include "intWid16.c"
		
		default:
			return intSetException(pVM, EXCEPT_ILLEGAL_OPCODE, pIP, pIN);
	}
	RETURN(OK);
	
	p;
}

#endif

/* ---------------------------------------------------------------------------- */
/** 
 * intWide32Ops
 *
 */
#if IP_CFG_CLEVEL >= IP_CFG_CLEVEL_WIDE32

IEC_UINT intWide32Ops(STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP)
{
   /* Opcodes with PREF_WIDE32 
	*/
	IEC_DATA OS_DPTR *pSeg;
	IEC_DATA OS_DPTR *p;

	SIntShared *pShared = pVM->pShared;
	SIntLocal  *pLocal	= &pVM->Local;
	
	switch (*(pIP++))
	{
		#include "intWid32.c"

		default:
			return intSetException(pVM, EXCEPT_ILLEGAL_OPCODE, pIP, pIN);
	}

	RETURN(OK);
	
	p;
}

#endif

/* ---------------------------------------------------------------------------- */
/** 
 * intWide64Ops
 *
 */
#if IP_CFG_CLEVEL >= IP_CFG_CLEVEL_WIDE64

IEC_UINT intWide64Ops(STaskInfoVM *pVM, CPTR_TYP *ppIP, DPTR_TYP *ppIN, SPTR_TYP *ppSP)
{
	/* Opcodes with PREF_WIDE64 
	 */
	IEC_DATA OS_DPTR *pSeg;
	IEC_DATA OS_DPTR *p;
   
	SIntLocal  *pLocal	= &pVM->Local;
   
	switch (*(pIP++))
	{
		#include "intWid64.c"

		default:
			return intSetException(pVM, EXCEPT_ILLEGAL_OPCODE, pIP, pIN);
	}

   RETURN(OK);

   p;
}

#endif

/* ---------------------------------------------------------------------------- */
