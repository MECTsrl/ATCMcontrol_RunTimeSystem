
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
 * Filename: intWid64.c
 */


/* ---------------------------------------------------------------------------- */

#define IEC_xx			IEC_LWORD
#define IEC_Sxx 		IEC_LINT
#define IP_xx			IP_LWORD
#define POPxxL			POP64L
#define PUSHxxL 		PUSH64L
#define ADDxxPTR		OS_ADDPTR32
#define LABxx(ll)		ll##64

#define JUMPxOFF		(2 * sizeof(IEC_BYTE) + sizeof(IEC_xx)) /* Opc. + operand */

#define ASM_PSHC_VV_xx		ASM_PSHC_VV_64
#define ASM_PSHC_00_xx		ASM_PSHC_00_64
#define ASM_PSHC_01_xx		ASM_PSHC_01_64


/* Include PSHC
 */
#if defined(IP_CFG_LINT) || defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD) || defined(IP_CFG_LREAL)
  #define IP_ENA_PSHCxx
#endif

/* Include wide xx opcodes for global data access
 */
#if defined(IP_CFG_GLOB64)
  #define IP_ENA_GLOBxx
#endif

#if defined(IP_CFG_GLOB32)
  #define IP_ENA_GLOB_BITxx
#endif

/* Include wide xx opcodes for object data access
 */
#if defined(IP_CFG_OBJ64)
  #define IP_ENA_OBJxx
#endif

#if defined(IP_CFG_OBJ32)
  #define IP_ENA_OBJ_BITxx
#endif


/* ----------------------------------------------------------------------------
 * PSHC / PSHC_0 / PSHC_1
 * ----------------------------------------------------------------------------
 * Push a constant value on the interpreter stack.
 *
 * ASM_PSHC_{w} 		*SP = c
 * ASM_PSHC_{w}_0		*SP = 0
 * ASM_PSHC_{w}_1		*SP = 1
 * ----------------------------------------------------------------------------
 */
#if defined(IP_ENA_PSHCxx)

	case ASM_PSHC_VV_xx: /* --------------------------------------------------- */
	{
		STK_OCHK8(1);
		SP_DEC8;
		IP_DWORD(((IEC_DWORD OS_SPTR *)pSP)[0]); 
		IP_DWORD(((IEC_DWORD OS_SPTR *)pSP)[1]); 
	}
	break;

	case ASM_PSHC_00_xx: /* --------------------------------------------------- */		
	{
		STK_OCHK8(1);
		SP_DEC8;
		(*(IEC_LWORD OS_SPTR *)pSP).L = 0; 
		(*(IEC_LWORD OS_SPTR *)pSP).H = 0; 
	}
	break;	

	case ASM_PSHC_01_xx: /* --------------------------------------------------- */
	{
		STK_OCHK8(1);
		SP_DEC8;
		(*(IEC_LWORD OS_SPTR *)pSP).L = 1; 
		(*(IEC_LWORD OS_SPTR *)pSP).H = 0; 
	}
	break;

#endif	/* IP_ENA_PSHCxx */


/* ----------------------------------------------------------------------------
 * PSHD_Gx / POPD_Gx / LEAD_Gx
 * ----------------------------------------------------------------------------
 * Access to % addressed global variables (simple and complex).
 * POPD_GO / LEAD_GO replaced by POPD_GX / LEAD_GX starting from V2.06.
 *
 * ASM_PSHD_Gx_{w}		*SP 	 = *(X + o)
 * ASM_POPD_Gx_{w}		*(X + o) = *SP
 * ASM_LEAD_Gx{_1}		*SP 	 =	X + o
 * ----------------------------------------------------------------------------
 */
#if (defined(IP_ENA_GLOBxx) || defined(IP_ENA_GLOB_BITxx)) && defined(IP_ENA_DT1)

	case ASM_PSHD_GI_1 : pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_PUSHxxxB);
	case ASM_PSHD_GO_1 : pSeg = pLocal->pSeg[SEG_OUTPUT].pAdr; goto LABxx(m_PUSHxxxB);
	case ASM_PSHD_GM_1 : pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_PUSHxxxB); 
	LABxx(m_PUSHxxxB):
	{
		VAL_IP1(o);

		STK_OCHK1(1);
		SP_DEC1;
		*(IEC_BYTE OS_SPTR *)pSP = (IEC_BYTE)((*(IEC_BYTE OS_DPTR *)( (OS_ADDPTR(pSeg,(o.H<<29)+ (o.L>>3))) ) & (1 << (o.L & 7))) ? 1 : 0); 
	}
	break;
		
	case ASM_POPD_GI_1 : pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_POPxxxB);
	case ASM_POPD_GM_1 : pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_POPxxxB); 
	LABxx(m_POPxxxB):
	{		  
		VAL_IP1(o);

		STK_UCHK1(1);
		OS_LOCK_BIT;
		*(IEC_BYTE OS_DPTR *)((OS_ADDPTR(pSeg,(o.H<<29)+ (o.L>>3)))) = (IEC_BYTE)((*(IEC_BYTE OS_DPTR *)((OS_ADDPTR(pSeg,(o.H<<29)+ (o.L>>3)))) & ~(1 << (o.L & 7))) | ((*(IEC_BYTE OS_SPTR *)pSP & 1) << (o.L & 7))); 
		OS_FREE_BIT;
		SP_INC1;
	}
	break;

	case ASM_POPD_GX_1 : /* --------------------------------------------------- */
	{		  
		VAL_IP1(o);

		STK_UCHK1(1);
		OS_LOCK_BIT;
		*(IEC_BYTE OS_DPTR *)((OS_ADDPTR(pLocal->pSeg[SEG_OUTPUT].pAdr,(o.H<<29)+ (o.L>>3)))) = (IEC_BYTE)((*(IEC_BYTE OS_DPTR *)((OS_ADDPTR(pLocal->pSeg[SEG_OUTPUT].pAdr,(o.H<<29)+ (o.L>>3)))) & ~(1 << (o.L & 7))) | ((*(IEC_BYTE OS_SPTR *)pSP & 1) << (o.L & 7))); 
		OS_FREE_BIT;
		SP_INC1;

	  #if defined(RTS_CFG_WRITE_FLAGS)
		WFLG1(ADDxxPTR(pLocal->WriteFlags.pAdr, (o.H<<29)+ (o.L>>3)), o.L)
	  #endif
	}
	break;

	case ASM_LEAD_GM_1: pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_LEAxx1);
	case ASM_LEAD_GI_1: pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_LEAxx1);
	LABxx(m_LEAxx1):
	{
		VAL_IP1(o);
		PUSHBITPL(ADDxxPTR(pSeg, (o.H<<29) + (o.L>>3)), o.L);
	}
	break;
					 
	case ASM_LEAD_GX_1: /* ---------------------------------------------------- */	
	{
		VAL_IP1(o);
		PUSHBITPL(ADDxxPTR(pLocal->pSeg[SEG_OUTPUT].pAdr, (o.H<<29) + (o.L>>3)), o.L);

	  #if defined(RTS_CFG_WRITE_FLAGS)
		WFLG1(ADDxxPTR(pLocal->WriteFlags.pAdr, (o.H<<29)+ (o.L>>3)), o.L)
	  #endif
	}
	break;

#endif

/* ---------------------------------------------------------------------------- */

#undef IEC_xx
#undef IEC_Sxx
#undef IP_xx
#undef POPxxL
#undef PUSHxxL
#undef ADDxxPTR
#undef LABxx
#undef JUMPxxOFF
#undef ASM_PSHC_VV_xx
#undef ASM_PSHC_00_xx
#undef ASM_PSHC_01_xx

#undef IP_ENA_PSHCxx

#undef IP_ENA_GLOBxx
#undef IP_ENA_GLOB_BITxx
#undef IP_ENA_OBJxx
#undef IP_ENA_OBJ_BITxx

/* ---------------------------------------------------------------------------- */
