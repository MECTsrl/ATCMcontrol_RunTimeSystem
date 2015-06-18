
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
 * Filename: intInter.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"intInter.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#define OPC_INTERPRETER 	1
  #include "intOpcds.h"
#undef	OPC_INTERPRETER

/* ----  Target Specific Includes:	 ------------------------------------------ */

#include <math.h>

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

#if defined(RTS_CFG_CUSTOMER_LIB)

  extern EXECUTE_FUN g_pTargetFun[];
  extern EXECUTE_FB  g_pTargetFB[];

#endif

extern EXECUTE_FUN	g_pLibraryFun[];
extern EXECUTE_FB	g_pLibraryFB[];

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * Interpreter
 *
 */
IEC_UINT intExecute(STaskInfoVM *pVM)
{
	IEC_DATA OS_CPTR *pIP;
	IEC_DATA OS_DPTR *pIN;
	IEC_DATA OS_SPTR *pSP;
	IEC_DATA OS_DPTR *pSeg;
	register IEC_DATA code;

	SContext *pConNew	= 0; 
	SContext *pConOld	= 0; 

	SIntLocal  *pLocal	= &pVM->Local;
	SIntShared *pShared = pVM->pShared;

	IEC_BYTE OS_DPTR *p;

	if (pLocal->uContext == 0)
	{
		return intSetException(pVM, EXCEPT_UNKOWN, 0, 0);
	}

	if (pLocal->uContext == 1)
	{
		pIP = pShared->pCode[pLocal->pContext->uCode].pAdr;
		pIN = pShared->pData[pLocal->pContext->uData].pAdr; 
		pSP = pLocal->pStack + MAX_STACK_SIZE;
	} 
	else 
	{
		pLocal->uContext--;

		pConNew = pLocal->pContext + pLocal->uContext;

		pIP = OS_ADDPTR(pShared->pCode[pConNew->uCode].pAdr, pConNew->uCodePos);
		pIN = pConNew->pData;
		pSP = pLocal->pSP;
	}
   
	for (;;)
	{

	code = (IEC_DATA) *(pIP++);

	switch (code)
	{
	
	case ASM_NOP: /* ---------------------------------------------------------- */
		break; 

	case ASM_BREAK: /* -------------------------------------------------------- */
	{
		if (pLocal->pState->ulState == TASK_STATE_STEP)
		{
			if (pLocal->uContext < MAX_CALLS)
			{
				pConOld = pLocal->pContext + (pLocal->uContext - 1);
				pConNew = pLocal->pContext +  pLocal->uContext;

				pConNew->uCodePos	= (IEC_UINT)OS_SUBPTR(pIP, pShared->pCode[pConOld->uCode].pAdr);
				pConNew->pData		= pIN;
				pConNew->uCode		= pConOld->uCode;
				pConNew->uData		= pConOld->uData;
				pLocal->pSP 		= pSP;

				pLocal->uContext++;    
								
				if(vmmQueueBPNotification(pVM, BP_STATE_REACHED) != OK)
				{
					return intSetException(pVM, EXCEPT_CALL_OVERFLOW, pIP, pIN);
				}

				return WRN_BREAK;
			}
		
			return intSetException(pVM, EXCEPT_CALL_OVERFLOW, pIP, pIN);
		}
			
		break;
	}
		  
	case ASM_ACTIVE_BREAK:	/* ------------------------------------------------ */
	{
		switch (pLocal->pState->ulState)
		{
		case TASK_STATE_RUNNING:	   
		{
			if (pLocal->uContext < MAX_CALLS)
			{
				SBPEntry *pBP = NULL;
				IEC_UINT uRes;

				pConOld = pLocal->pContext + (pLocal->uContext - 1);
				pConNew = pLocal->pContext + pLocal->uContext;

				uRes = bpQueryBreakpoint(pShared, pConOld->uCode, pConOld->uData,
						(IEC_UINT)(OS_SUBPTR(pIP, pShared->pCode[pConOld->uCode].pAdr) - 1), &pBP);
				
				if (uRes != OK)
				{
					return intSetException(pVM, EXCEPT_UNKOWN, pIP, pIN);
				}
		
				if (pBP == NULL)
				{
					uRes = bpIsBPInList(pShared, pConOld->uCode, 
						(IEC_UINT)(OS_SUBPTR(pIP, pShared->pCode[pConOld->uCode].pAdr) - 1), &pBP);
					
					if (uRes != OK)
					{
						return intSetException(pVM, EXCEPT_UNKOWN, pIP, pIN);
					}

					if (pBP == NULL)
					{
						/* The breakpoint is marked as active in the IP code, but not
						 * present in the VMM's breakpoint list. So we can delete it.
						 */
						*(pIP - 1) = ASM_BREAK;
					}
				
					break;
				}
				
				pConNew->uCodePos	= (IEC_UINT)(OS_SUBPTR(pIP, pShared->pCode[pConOld->uCode].pAdr));
				pConNew->pData		= pIN;
				pConNew->uCode		= pConOld->uCode;
				pConNew->uData		= pConOld->uData;
				pLocal->pSP 		= pSP;

				pLocal->uContext++; 		
				
				if(vmmQueueBPNotification(pVM, BP_STATE_REACHED) != OK)
				{
					return intSetException(pVM, EXCEPT_CALL_OVERFLOW, pIP, pIN);
				}
				
				return WRN_BREAK;
			} 

			return intSetException(pVM, EXCEPT_CALL_OVERFLOW, pIP, pIN);
		}
			
		case TASK_STATE_STEP:
		{
			if (pLocal->uContext < MAX_CALLS)
			{				
				pConOld = pLocal->pContext + (pLocal->uContext - 1);
				pConNew = pLocal->pContext + pLocal->uContext;

				pConNew->uCodePos	= (IEC_UINT)(OS_SUBPTR(pIP, pShared->pCode[pConOld->uCode].pAdr));
				pConNew->pData		= pIN;
				pConNew->uCode		= pConOld->uCode;
				pConNew->uData		= pConOld->uData;
				pLocal->pSP 		= pSP;

				pLocal->uContext++; 		
				
				if(vmmQueueBPNotification(pVM, BP_STATE_REACHED) != OK)
				{
					return intSetException(pVM, EXCEPT_CALL_OVERFLOW, pIP, pIN);
				}		
				
				return WRN_BREAK;
			}

			return intSetException(pVM, EXCEPT_CALL_OVERFLOW, pIP, pIN);
		}
		
		}	/* switch (*pTaskState) */

		break;
	}
		
#if defined(IP_CFG_NCC)
	case ASM_NATIVE:	/* ---------------------------------------------------- */
	{
		IEC_UINT uRes = osExecNativeCode(pShared, pLocal, &pIN, &pSP, &pIP);
		if (uRes == 0xFFFF)
		{
			return OK;
		} 
		else if (uRes != OK)
		{
			return intSetException(pVM, uRes, pIP, pIN);
		}
		else if (pLocal->uHalt == TRUE)
		{
			return intHalt(pVM);
		}
		break;
	}
#endif	/* IP_CFG_NCC */

	case PREF_CONV: /* -------------------------------------------------------- */
#if IP_CFG_CLEVEL >= IP_CFG_CLEVEL_CONV
		{
			IEC_UINT uRes = intConvert(pVM, &pIP, &pIN, &pSP);
			if (uRes != OK) 
			{
				return uRes;
			}
		}
#else 
		switch (*(pIP++))
		{
#include "intConv.c"
		default:
			return intSetException(pVM, EXCEPT_ILLEGAL_OPCODE, pIP, pIN);
		}
#endif
		break;

	case PREF_ARITH:	/* ---------------------------------------------------- */
#if IP_CFG_CLEVEL >= IP_CFG_CLEVEL_ARITH
		{
			IEC_UINT uRes = intArith(pVM, &pIP, &pIN, &pSP);
			if (uRes != OK) 
			{
				return uRes;
			}
		}
#else 
		switch (*(pIP++))
		{
#include "intArith.c"
		default:
			return intSetException(pVM, EXCEPT_ILLEGAL_OPCODE, pIP, pIN);
		}
#endif
		break;

	case PREF_WIDE8:	/* ---------------------------------------------------- */
		/* default, nothing to do, should never occur .. ignore this */ 
		break;

	case PREF_WIDE16:	/*----------------------------------------------------- */
#if IP_CFG_CLEVEL >= IP_CFG_CLEVEL_WIDE16
		{
			IEC_UINT uRes = intWide16Ops(pVM, &pIP, &pIN, &pSP);
			if (uRes != OK) 
			{
				return uRes;
			}
		}
#else 
		switch (*(pIP++))
		{
#include "intWid16.c"
		default:
			return intSetException(pVM, EXCEPT_ILLEGAL_OPCODE, pIP, pIN);
		}
#endif
		break;

	case PREF_WIDE32:	/*----------------------------------------------------- */
#if IP_CFG_CLEVEL >= IP_CFG_CLEVEL_WIDE32
		{
			IEC_UINT uRes = intWide32Ops(pVM, &pIP, &pIN, &pSP);
			if (uRes != OK) 
			{
				return uRes;
			}
		}
#else 
		switch (*(pIP++))
		{
#include "intWid32.c"
		default:
			return intSetException(pVM, EXCEPT_ILLEGAL_OPCODE, pIP, pIN);
		}
#endif
		break;

	case PREF_WIDE64:	/*----------------------------------------------------- */
#if IP_CFG_CLEVEL >= IP_CFG_CLEVEL_WIDE64
		{
			IEC_UINT uRes = intWide64Ops(pVM, &pIP, &pIN, &pSP);
			if (uRes != OK) 
			{
				return uRes;
			}
		}
#else 
		switch (*(pIP++))
		{
#include "intWid64.c"
		default:
			return intSetException(pVM, EXCEPT_ILLEGAL_OPCODE, pIP, pIN);
		}
#endif
		break;

/* ---------------------------------------------------------------------------- */

	case ASM_RETN:	/* -------------------------------------------------------- */
	{
		/* Check for watchdog
		 */
		if (pLocal->pState->ulState == TASK_STATE_ERROR) 
		{
			if (pLocal->pState->ulErrNo == OK)
			{
				pLocal->pState->ulErrNo = EXCEPT_UNKOWN;
			}
			
			return intSetException(pVM, (IEC_UINT)pLocal->pState->ulErrNo, pIP, pIN);
		}

		if (pLocal->uHalt == TRUE)
		{
			return intHalt(pVM);
		}
		
		if (pLocal->uContext == 1)
		{
			return OK;
		} 
		else 
		{
			pLocal->uContext--;

			pConNew = pLocal->pContext + (pLocal->uContext - 1);
			pIP 	= OS_ADDPTR(pShared->pCode[pConNew->uCode].pAdr, pConNew->uCodePos);
			pIN 	= pConNew->pData;
		}
		break;
	}
			 
/* ---------------------------------------------------------------------------- */
	
#include "intWid8.c"

/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_STRING)

	case ASM_PSHC_TX:	/* ---------------------------------------------------- */
		{
			PUSHPL(pIP);
			
			pIP += *(IEC_BYTE OS_CPTR *)pIP;
			pIP += 2;
		}
		break;

#endif /* IP_CFG_STRING */

/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_BOOL)	

	case ASM_SHL_BOOL:	/* ----------------------------------------------------*/
		STK_UCHK2(1);
		{
			IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			SP_INC2;
			STK_UCHK1(1);
			if (u != 0)
			{
				*(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)0;
			}
		}
		break;
		
	case ASM_SHR_BOOL:	/* ----------------------------------------------------*/
		STK_UCHK2(1);
		{
			IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			SP_INC2;
			STK_UCHK1(1);
			if (u != 0)
			{
				*(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)0;
			}
		}
		break;
		
	case ASM_ROR_BOOL:	/* ----------------------------------------------------*/
		STK_UCHK2(1);
		SP_INC2;
		STK_UCHK1(1);
		break;
		
	case ASM_ROL_BOOL:	/* ----------------------------------------------------*/
		STK_UCHK2(1);
		SP_INC2;
		STK_UCHK1(1);
		break;
		
	case ASM_AND_BOOL:	/* ----------------------------------------------------*/
		STK_UCHK1(2);
		{
			IEC_BOOL b = *(IEC_BOOL OS_SPTR *)pSP;
			SP_INC1;
			*(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)(*(IEC_BOOL OS_SPTR *)pSP & b);
		}
		break;
		
	case ASM_OR_BOOL:	/* ----------------------------------------------------*/
		STK_UCHK1(2);
		{
			IEC_BOOL b = *(IEC_BOOL OS_SPTR *)pSP;
			SP_INC1;
			*(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)(*(IEC_BOOL OS_SPTR *)pSP | b);
		}
		break;
		
	case ASM_XOR_BOOL:	/* ----------------------------------------------------*/
		STK_UCHK1(2);
		{
			IEC_BOOL b = *(IEC_BOOL OS_SPTR *)pSP;
			SP_INC1;
			*(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)(*(IEC_BOOL OS_SPTR *)pSP ^ b);
		}
		break;
		
	case ASM_NOT_BOOL:	/* ----------------------------------------------------*/
		STK_UCHK1(1);
		*(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)(*(IEC_BOOL OS_SPTR *)pSP ? 0 : 1);
		break;

#endif /* IP_CFG_BOOL */

/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_BYTE)

	case ASM_SHL_BYTE:	/* ---------------------------------------------------- */
		STK_UCHK2(1);
		{
			IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			SP_INC2;
			STK_UCHK1(1);
			if ((u >=  8) ||
				 (u <= -8))
			{
				*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)0;
			} else if (u >= 0) {
				*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(*(IEC_BYTE OS_SPTR *)pSP << u);
			} else {
				*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(*(IEC_BYTE OS_SPTR *)pSP >> -u);
			}
		}
		break;
		
	case ASM_SHR_BYTE:	/* ---------------------------------------------------- */
		STK_UCHK2(1);
		{
			IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			SP_INC2;
			STK_UCHK1(1);
			if ((u >=  8) ||
				 (u <= -8))
			{
				*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)0;
			} else if (u >= 0) {
				*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(*(IEC_BYTE OS_SPTR *)pSP >> u);
			} else {
				*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(*(IEC_BYTE OS_SPTR *)pSP << -u);
			}
		}
		break;
		
	case ASM_ROR_BYTE:	/* ---------------------------------------------------- */
		STK_UCHK2(1);
		{
			IEC_INT u = (IEC_INT)(*(IEC_INT OS_SPTR *)pSP & 7);
			IEC_BYTE b;
			SP_INC2;
			STK_UCHK1(1);
			b = *(IEC_BYTE OS_SPTR *)pSP;
			for ( ; u > 0; u--)
			{
				if (b & 0x01)
				{
					b = (IEC_BYTE)((b >> 1) | 0x80);
				} else {
					b >>= 1;
				}
			}
			*(IEC_BYTE OS_SPTR *)pSP = b;
		}
		break;
		
	case ASM_ROL_BYTE:	/* ---------------------------------------------------- */
		STK_UCHK2(1);
		{
			IEC_INT u = (IEC_INT)(*(IEC_INT OS_SPTR *)pSP & 7);
			IEC_BYTE b;
			SP_INC2;
			STK_UCHK1(1);
			b = *(IEC_BYTE OS_SPTR *)pSP;
			for ( ; u > 0; u--)
			{
				if (b & 0x80)
				{
					b = (IEC_BYTE)((b << 1) | 0x01);
				} else {
					b <<= 1;
				}
			}
			*(IEC_BYTE OS_SPTR *)pSP = b;
		}
		break;
		
	case ASM_AND_BYTE:	/* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_BYTE u = *(IEC_BYTE OS_SPTR *)pSP;
			SP_INC1;
			*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(*(IEC_BYTE OS_SPTR *)pSP & u);
		}
		break;
		
	case ASM_OR_BYTE:	/* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_BYTE u = *(IEC_BYTE OS_SPTR *)pSP;
			SP_INC1;
			*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(*(IEC_BYTE OS_SPTR *)pSP | u);
		}
		break;
		
	case ASM_XOR_BYTE:	/* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_BYTE u = *(IEC_BYTE OS_SPTR *)pSP;
			SP_INC1;
			*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(*(IEC_BYTE OS_SPTR *)pSP ^ u);
		}
		break;
		
	case ASM_NOT_BYTE:	/* ---------------------------------------------------- */
		STK_UCHK1(1);
		*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE) (~(*(IEC_BYTE OS_SPTR *)pSP));
		break;

#endif /* IP_CFG_BYTE */

/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_WORD)

	case ASM_SHL_WORD:	/* ---------------------------------------------------- */
		STK_UCHK2(2);
		{
			IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			SP_INC2;
			if ((u >=  16) ||
				 (u <= -16))
			{
				*(IEC_WORD OS_SPTR *)pSP = (IEC_WORD)0;
			} else if (u >= 0) {
				*(IEC_WORD OS_SPTR *)pSP = (IEC_WORD)(*(IEC_WORD OS_SPTR *)pSP << u);
			} else {
				*(IEC_WORD OS_SPTR *)pSP = (IEC_WORD)(*(IEC_WORD OS_SPTR *)pSP >> -u);
			}
		}
		break;
		
	case ASM_SHR_WORD:	/* ---------------------------------------------------- */
		STK_UCHK2(2);
		{
			IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			SP_INC2;
			if ((u >=  16) ||
				 (u <= -16))
			{
				*(IEC_WORD OS_SPTR *)pSP = (IEC_WORD)0;
			} else if (u >= 0) {
				*(IEC_WORD OS_SPTR *)pSP = (IEC_WORD)(*(IEC_WORD OS_SPTR *)pSP >> u);
			} else {
				*(IEC_WORD OS_SPTR *)pSP = (IEC_WORD)(*(IEC_WORD OS_SPTR *)pSP << -u);
			}
		}
		break;
		
	case ASM_ROR_WORD:	/* ---------------------------------------------------- */
		STK_UCHK2(2);
		{
			IEC_INT u = (IEC_INT)(*(IEC_INT OS_SPTR *)pSP & 15);
			IEC_WORD b;
			SP_INC2;
			b = *(IEC_WORD OS_SPTR *)pSP;
			for ( ; u > 0; u--)
			{
				if (b & 0x0001)
				{
					b = (IEC_WORD)((b >> 1) | 0x8000);
				} else {
					b >>= 1;
				}
			}
			*(IEC_WORD OS_SPTR *)pSP = b;
		}
		break;
		
	case ASM_ROL_WORD:	/* ---------------------------------------------------- */
		STK_UCHK2(2);
		{
			IEC_INT u = (IEC_INT)(*(IEC_INT OS_SPTR *)pSP & 15);
			IEC_WORD b;
			SP_INC2;
			b = *(IEC_WORD OS_SPTR *)pSP;
			for ( ; u > 0; u--)
			{
				if (b & 0x8000)
				{
					b = (IEC_WORD)((b << 1) | 0x0001);
				} else {
					b <<= 1;
				}
			}
			*(IEC_WORD OS_SPTR *)pSP = b;
		}
		break;
		
	case ASM_AND_WORD:	/* ---------------------------------------------------- */
		STK_UCHK2(2);
		{
			IEC_WORD w = *(IEC_WORD OS_SPTR *)pSP;
			SP_INC2;
			*(IEC_WORD OS_SPTR *)pSP = (IEC_WORD)(*(IEC_WORD OS_SPTR *)pSP & w);
		}
		break;
		
	case ASM_OR_WORD:	/* ---------------------------------------------------- */
		STK_UCHK2(2);
		{
			IEC_WORD w = *(IEC_WORD OS_SPTR *)pSP;
			SP_INC2;
			*(IEC_WORD OS_SPTR *)pSP = (IEC_WORD)(*(IEC_WORD OS_SPTR *)pSP | w);
		}
		break;
		
	case ASM_XOR_WORD:	/* ---------------------------------------------------- */
		STK_UCHK2(2);
		{
			IEC_WORD w = *(IEC_WORD OS_SPTR *)pSP;
			SP_INC2;
			*(IEC_WORD OS_SPTR *)pSP = (IEC_WORD)(*(IEC_WORD OS_SPTR *)pSP ^ w);
		}
		break;
		
	case ASM_NOT_WORD:	/* ---------------------------------------------------- */
		STK_UCHK2(1);
		*(IEC_WORD OS_SPTR *)pSP = (IEC_WORD) (~(*(IEC_WORD OS_SPTR *)pSP));
		break;
		
#endif /* IP_CFG_WORD */

/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_BOOL)

	case ASM_GT_BOOL:	/* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_BOOL b = (IEC_BYTE)(*(IEC_BOOL OS_SPTR *)pSP & 1);
			SP_INC1;
			*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(((*(IEC_BOOL OS_SPTR *)pSP & 1u) > b) ? 1 : 0);
		}
		break;
		
	case ASM_GE_BOOL:	/* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_BOOL b = (IEC_BYTE)(*(IEC_BOOL OS_SPTR *)pSP & 1);
			SP_INC1;
			*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(((*(IEC_BOOL OS_SPTR *)pSP & 1u) >= b) ? 1 : 0);
		}
		break;
		
	case ASM_EQ_BOOL:	/* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_BOOL b = (IEC_BYTE)(*(IEC_BOOL OS_SPTR *)pSP & 1);
			SP_INC1;
			*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(((*(IEC_BOOL OS_SPTR *)pSP & 1u) == b) ? 1 : 0);
		}
		break;
		
	case ASM_LE_BOOL:	/* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_BOOL b = (IEC_BYTE)(*(IEC_BOOL OS_SPTR *)pSP & 1);
			SP_INC1;
			*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(((*(IEC_BOOL OS_SPTR *)pSP & 1u) <= b) ? 1 : 0);
		}
		break;
		
	case ASM_LT_BOOL:	/* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_BOOL b = (IEC_BYTE)(*(IEC_BOOL OS_SPTR *)pSP & 1);
			SP_INC1;
			*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(((*(IEC_BOOL OS_SPTR *)pSP & 1u) < b) ? 1 : 0);
		}
		break;
		
	case ASM_NE_BOOL:	/* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_BOOL b = (IEC_BYTE)(*(IEC_BOOL OS_SPTR *)pSP & 1);
			SP_INC1;
			*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)(((*(IEC_BOOL OS_SPTR *)pSP & 1u) != b) ? 1 : 0);
		}
		break;

#endif /* IP_CFG_BOOL */

/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_SINT)

	case ASM_NEG_SINT : /* ---------------------------------------------------- */
		STK_UCHK1(1);
		*(IEC_SINT OS_SPTR *)pSP = (IEC_SINT) -(*(IEC_SINT OS_SPTR *)pSP); 
		break;
		
	case ASM_ABS_SINT : /* ---------------------------------------------------- */
		STK_UCHK1(1);
		if (*(IEC_SINT OS_SPTR *)pSP < 0)
		{
			*(IEC_SINT OS_SPTR *)pSP = (IEC_SINT) -(*(IEC_SINT OS_SPTR *)pSP); 
		}
		break;
		
	case ASM_ADD_SINT : /* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_SINT u = *(IEC_SINT OS_SPTR *)pSP;
			SP_INC1;
			*(IEC_SINT OS_SPTR *)pSP = (IEC_SINT)(*(IEC_SINT OS_SPTR *)pSP + u); 
		}
		break;
		
	case ASM_MUL_SINT : /* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_SINT u = *(IEC_SINT OS_SPTR *)pSP;
			SP_INC1;
			*(IEC_SINT OS_SPTR *)pSP = (IEC_SINT)(*(IEC_SINT OS_SPTR *)pSP * u); 
		}
		break;
		
	case ASM_SUB_SINT : /* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_SINT u = *(IEC_SINT OS_SPTR *)pSP;
			SP_INC1;
			*(IEC_SINT OS_SPTR *)pSP = (IEC_SINT)(*(IEC_SINT OS_SPTR *)pSP - u); 
		}
		break;
		
	case ASM_DIV_SINT : /* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_SINT u = *(IEC_SINT OS_SPTR *)pSP;
			if (u == 0)
			{
				return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
			}
			SP_INC1;
			*(IEC_SINT OS_SPTR *)pSP = (IEC_SINT)(*(IEC_SINT OS_SPTR *)pSP / u); 
		}
		break;
		
	case ASM_MOD_SINT : /* ---------------------------------------------------- */
		STK_UCHK1(2);
		{
			IEC_SINT u = *(IEC_SINT OS_SPTR *)pSP;
			SP_INC1;
			if (u != 0)
				*(IEC_SINT OS_SPTR *)pSP = (IEC_SINT)(*(IEC_SINT OS_SPTR *)pSP % u); 
			else
				return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
		}
		break;

#endif /* IP_CFG_SINT */

/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_INT)

	case ASM_NEG_INT:	/* ---------------------------------------------------- */
		STK_UCHK2(1);
		*(IEC_INT OS_SPTR *)pSP = (IEC_INT)-(*(IEC_INT OS_SPTR *)pSP); 
		break;
		
	case ASM_ABS_INT:	/* ---------------------------------------------------- */
		STK_UCHK2(1);
		if (*(IEC_INT OS_SPTR *)pSP < 0)
		{
			*(IEC_INT OS_SPTR *)pSP = (IEC_INT)-(*(IEC_INT OS_SPTR *)pSP); 
		}
		break;
		
	case ASM_ADD_INT:	/* ---------------------------------------------------- */
		STK_UCHK2(2);
		{
			IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			SP_INC2;
			*(IEC_INT OS_SPTR *)pSP = (IEC_INT)(*(IEC_INT OS_SPTR *)pSP + u); 
		}
		break;
		
	case ASM_MUL_INT:	/* ---------------------------------------------------- */
		STK_UCHK2(2);
		{
			IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			SP_INC2;
			*(IEC_INT OS_SPTR *)pSP = (IEC_INT)(*(IEC_INT OS_SPTR *)pSP * u); 
		}
		break;
		
	case ASM_SUB_INT:	/* ---------------------------------------------------- */
		STK_UCHK2(2);
		{
			IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			SP_INC2;
			*(IEC_INT OS_SPTR *)pSP = (IEC_INT)(*(IEC_INT OS_SPTR *)pSP - u); 
		}
		break;
		
	case ASM_DIV_INT:	/* ---------------------------------------------------- */
		STK_UCHK2(2);
		{
			IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			if (u == 0)
			{
				return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
			}
			SP_INC2;
			*(IEC_INT OS_SPTR *)pSP = (IEC_INT)(*(IEC_INT OS_SPTR *)pSP / u); 
		}
		break;
		
	case ASM_MOD_INT:	/* ---------------------------------------------------- */
		STK_UCHK2(2);
		{
			IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			SP_INC2;
			if (u != 0)
				*(IEC_INT OS_SPTR *)pSP = (IEC_INT)(*(IEC_INT OS_SPTR *)pSP % u); 
			else
				return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
		}
		break;

#endif	/* IP_CFG_INT */

	default:
		return intSetException(pVM, EXCEPT_ILLEGAL_OPCODE, pIP, pIN);
		
	} /* end switch(code) */

	} /* end for(; ;) */
}

/* ---------------------------------------------------------------------------- */
