
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
 * Filename: intArith.c
 */


/* ---------------------------------------------------------------------------- */

#if (defined(IP_CFG_USINT) || defined(IP_CFG_BYTE))

		 case ASM_GT_USINT:
/*		 case ASM_GT_BYTE : */
			STK_UCHK1(2); /* 2 byte par on stack expected */
			{
			   IEC_USINT b = *(IEC_USINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_USINT OS_SPTR *)pSP > b) ? 1 : 0);
			}
			break;

		 case ASM_GE_USINT:
/*		 case ASM_GE_BYTE : */
			STK_UCHK1(2); /* 2 byte par on stack expected */
			{
			   IEC_USINT b = *(IEC_USINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_USINT OS_SPTR *)pSP >= b) ? 1 : 0);
			}
			break;

		 case ASM_EQ_USINT:
/*		 case ASM_EQ_BYTE : */
			STK_UCHK1(2); /* 2 byte par on stack expected */
			{
			   IEC_USINT b = *(IEC_USINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_USINT OS_SPTR *)pSP == b) ? 1 : 0);
			}
			break;

		 case ASM_LE_USINT:
/*		 case ASM_LE_BYTE : */
			STK_UCHK1(2); /* 2 byte par on stack expected */
			{
			   IEC_USINT b = *(IEC_USINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_USINT OS_SPTR *)pSP <= b) ? 1 : 0);
			}
			break;

		 case ASM_LT_USINT:
/*		 case ASM_LT_BYTE : */
			STK_UCHK1(2); /* 2 byte par on stack expected */
			{
			   IEC_USINT b = *(IEC_USINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_USINT OS_SPTR *)pSP < b) ? 1 : 0);
			}
			break;

		 case ASM_NE_USINT:
/*		 case ASM_NE_BYTE : */
			STK_UCHK1(2); /* 2 byte par on stack expected */
			{
			   IEC_USINT b = *(IEC_USINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_USINT OS_SPTR *)pSP != b) ? 1 : 0);
			}
			break;
#endif	 /* defined(IP_CFG_USINT) || defined(IP_CFG_BYTE) */

#if defined(IP_CFG_USINT) 
		 case ASM_ADD_USINT  :
			STK_UCHK1(2);
			{
			   IEC_USINT u = *(IEC_USINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_USINT OS_SPTR *)pSP = (IEC_USINT)(*(IEC_USINT OS_SPTR *)pSP + u); 
			}
			break;

		 case ASM_MUL_USINT  :
			STK_UCHK1(2);
			{
			   IEC_USINT u = *(IEC_USINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_USINT OS_SPTR *)pSP = (IEC_USINT)(*(IEC_USINT OS_SPTR *)pSP * u); 
			}
			break;

		 case ASM_SUB_USINT  :
			STK_UCHK1(2);
			{
			   IEC_USINT u = *(IEC_USINT OS_SPTR *)pSP;
			   SP_INC4;
			   *(IEC_USINT OS_SPTR *)pSP = (IEC_USINT)(*(IEC_USINT OS_SPTR *)pSP - u); 
			}
			break;

		 case ASM_DIV_USINT  :
			STK_UCHK1(2);
			{
			   IEC_USINT u = *(IEC_USINT OS_SPTR *)pSP;
			   if (u == 0)
			   {
				  return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
			   }
			   SP_INC4;
			   *(IEC_USINT OS_SPTR *)pSP = (IEC_USINT)(*(IEC_USINT OS_SPTR *)pSP / u); 
			}
			break;

		 case ASM_MOD_USINT  :
			STK_UCHK1(2);
			{
			   IEC_USINT u = *(IEC_USINT OS_SPTR *)pSP;
			   SP_INC4;
			   if (u != 0)
				  *(IEC_USINT OS_SPTR *)pSP = (IEC_USINT)(*(IEC_USINT OS_SPTR *)pSP % u); 
			   else
				  return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
			}
			break;

#endif /* IP_CFG_USINT */

#if defined(IP_CFG_SINT)
		 case ASM_GT_SINT:
			STK_UCHK1(2); /* 2 byte par on stack expected */
			{
			   IEC_SINT b = *(IEC_SINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_SINT OS_SPTR *)pSP > b) ? 1 : 0);
			}
			break;

		 case ASM_GE_SINT:
			STK_UCHK1(2); /* 2 byte par on stack expected */
			{
			   IEC_SINT b = *(IEC_SINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_SINT OS_SPTR *)pSP >= b) ? 1 : 0);
			}
			break;

		 case ASM_EQ_SINT:
			STK_UCHK1(2); /* 2 byte par on stack expected */
			{
			   IEC_SINT b = *(IEC_SINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_SINT OS_SPTR *)pSP == b) ? 1 : 0);
			}
			break;

		 case ASM_LE_SINT:
			STK_UCHK1(2); /* 2 byte par on stack expected */
			{
			   IEC_SINT b = *(IEC_SINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_SINT OS_SPTR *)pSP <= b) ? 1 : 0);
			}
			break;

		 case ASM_LT_SINT:
			STK_UCHK1(2); /* 2 byte par on stack expected */
			{
			   IEC_SINT b = *(IEC_SINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_SINT OS_SPTR *)pSP < b) ? 1 : 0);
			}
			break;

		 case ASM_NE_SINT:
			STK_UCHK1(2); /* 2 byte par on stack expected */
			{
			   IEC_SINT b = *(IEC_SINT OS_SPTR *)pSP;
			   SP_INC1;
			   *(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_SINT OS_SPTR *)pSP != b) ? 1 : 0);
			}
			break;
#endif /* IP_CFG_SINT */

#if defined(IP_CFG_UINT)
		 case ASM_ADD_UINT	: 
			STK_UCHK2(2);
			{
			   IEC_UINT u = *(IEC_UINT OS_SPTR *)pSP;
			   SP_INC2;
			   *(IEC_UINT OS_SPTR *)pSP = (IEC_UINT)(*(IEC_UINT OS_SPTR *)pSP + u); 
			}
			break;

		 case ASM_MUL_UINT	: 
			STK_UCHK2(2);
			{
			   IEC_UINT u = *(IEC_UINT OS_SPTR *)pSP;
			   SP_INC2;
			   *(IEC_UINT OS_SPTR *)pSP = (IEC_UINT)(*(IEC_UINT OS_SPTR *)pSP * u); 
			}
			break;
#endif /* defined(IP_CFG_UINT) */

#if (defined(IP_CFG_UINT) || defined(IP_CFG_WORD))
		 case ASM_GT_UINT: 
/*		 case ASM_GT_WORD: */
			STK_UCHK2(2); /* 2 word par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_UINT b = *(IEC_UINT OS_SPTR *)pSP;
			   SP_INC2;
			   u = (IEC_BOOL)((*(IEC_UINT OS_SPTR *)pSP > b) ? 1 : 0);
			   SP_INC2;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_GE_UINT: 
/*		 case ASM_GE_WORD: */
			STK_UCHK2(2); /* 2 word par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_UINT b = *(IEC_UINT OS_SPTR *)pSP;
			   SP_INC2;
			   u = (IEC_BOOL)((*(IEC_UINT OS_SPTR *)pSP >= b) ? 1 : 0);
			   SP_INC2;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_EQ_UINT: 
/*		 case ASM_EQ_WORD: */
			STK_UCHK2(2); /* 2 word par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_UINT b = *(IEC_UINT OS_SPTR *)pSP;
			   SP_INC2;
			   u = (IEC_BOOL)((*(IEC_UINT OS_SPTR *)pSP == b) ? 1 : 0);
			   SP_INC2;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_LE_UINT: 
/*		 case ASM_LE_WORD: */
			STK_UCHK2(2); /* 2 word par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_UINT b = *(IEC_UINT OS_SPTR *)pSP;
			   SP_INC2;
			   u = (IEC_BOOL)((*(IEC_UINT OS_SPTR *)pSP <= b) ? 1 : 0);
			   SP_INC2;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_LT_UINT: 
/*		 case ASM_LT_WORD: */
			STK_UCHK2(2); /* 2 word par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_UINT b = *(IEC_UINT OS_SPTR *)pSP;
			   SP_INC2;
			   u = (IEC_BOOL)((*(IEC_UINT OS_SPTR *)pSP < b) ? 1 : 0);
			   SP_INC2;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_NE_UINT: 
/*		 case ASM_NE_WORD: */
			STK_UCHK2(2); /* 2 word par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_UINT b = *(IEC_UINT OS_SPTR *)pSP;
			   SP_INC2;
			   u = (IEC_BOOL)((*(IEC_UINT OS_SPTR *)pSP != b) ? 1 : 0);
			   SP_INC2;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;
#endif	/* defined(IP_CFG_UINT) || defined(IP_CFG_WORD) */

#if defined(IP_CFG_UINT)
		 case ASM_SUB_UINT	: 
			STK_UCHK2(2);
			{
			   IEC_UINT u = *(IEC_UINT OS_SPTR *)pSP;
			   SP_INC2;
			   *(IEC_UINT OS_SPTR *)pSP = (IEC_UINT)(*(IEC_UINT OS_SPTR *)pSP - u); 
			}
			break;

		 case ASM_DIV_UINT	: 
			STK_UCHK2(2);
			{
			   IEC_UINT u = *(IEC_UINT OS_SPTR *)pSP;
			   if (u == 0)
			   {
				  return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
			   }
			   SP_INC2;
			   *(IEC_UINT OS_SPTR *)pSP = (IEC_UINT)(*(IEC_UINT OS_SPTR *)pSP / u); 
			}
			break;

		 case ASM_MOD_UINT	: 
			STK_UCHK2(2);
			{
			   IEC_UINT u = *(IEC_UINT OS_SPTR *)pSP;
			   SP_INC2;
			   if (u != 0)
				  *(IEC_UINT OS_SPTR *)pSP = (IEC_UINT)(*(IEC_UINT OS_SPTR *)pSP % u); 
			   else
				  return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
			}
			break;
#endif /* IP_CFG_UINT */

#if defined(IP_CFG_INT)
		 case ASM_GT_INT:
			STK_UCHK2(2); /* 2 word par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_INT	b = *(IEC_INT OS_SPTR *)pSP;
			   SP_INC2;
			   u = (IEC_BOOL)((*(IEC_INT OS_SPTR *)pSP > b) ? 1 : 0);
			   SP_INC2;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_GE_INT:
			STK_UCHK2(2); /* 2 word par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_INT b = *(IEC_INT OS_SPTR *)pSP;
			   SP_INC2;
			   u = (IEC_BOOL)((*(IEC_INT OS_SPTR *)pSP >= b) ? 1 : 0);
			   SP_INC2;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_EQ_INT:
			STK_UCHK2(2); /* 2 word par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_INT b = *(IEC_INT OS_SPTR *)pSP;
			   SP_INC2;
			   u = (IEC_BOOL)((*(IEC_INT OS_SPTR *)pSP == b) ? 1 : 0);
			   SP_INC2;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_LE_INT:
			STK_UCHK2(2); /* 2 word par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_INT b = *(IEC_INT OS_SPTR *)pSP;
			   SP_INC2;
			   u = (IEC_BOOL)((*(IEC_INT OS_SPTR *)pSP <= b) ? 1 : 0);
			   SP_INC2;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_LT_INT:
			STK_UCHK2(2); /* 2 word par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_INT b = *(IEC_INT OS_SPTR *)pSP;
			   SP_INC2;
			   u = (IEC_BOOL)((*(IEC_INT OS_SPTR *)pSP < b) ? 1 : 0);
			   SP_INC2;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_NE_INT:
			STK_UCHK2(2); /* 2 word par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_INT b = *(IEC_INT OS_SPTR *)pSP;
			   SP_INC2;
			   u = (IEC_BOOL)((*(IEC_INT OS_SPTR *)pSP != b) ? 1 : 0);
			   SP_INC2;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;
#endif /* IP_CFG_INT */

#if defined(IP_CFG_UDINT)
		 case ASM_ADD_UDINT :
			STK_UCHK4(2);
			{
			   IEC_UDINT u = *(IEC_UDINT OS_SPTR *)pSP;
			   SP_INC4;
			   *(IEC_UDINT OS_SPTR *)pSP = (IEC_UDINT)(*(IEC_UDINT OS_SPTR *)pSP + u); 
			}
			break;

		 case ASM_MUL_UDINT :
			STK_UCHK4(2);
			{
			   IEC_UDINT u = *(IEC_UDINT OS_SPTR *)pSP;
			   SP_INC4;
			   *(IEC_UDINT OS_SPTR *)pSP = (IEC_UDINT)(*(IEC_UDINT OS_SPTR *)pSP * u); 
			}
			break;
#endif /* IP_CFG_UDINT */

#if defined(IP_CFG_UDINT)
		 case ASM_SUB_UDINT:
			STK_UCHK4(2);
			{
			   IEC_UDINT u = *(IEC_UDINT OS_SPTR *)pSP;
			   SP_INC4;
			   *(IEC_UDINT OS_SPTR *)pSP = *(IEC_UDINT OS_SPTR *)pSP - u; 
			}
			break;

		 case ASM_DIV_UDINT:
			STK_UCHK4(2);
			{
			   IEC_UDINT u = *(IEC_UDINT OS_SPTR *)pSP;
			   if (u == 0)
			   {
				  return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
			   }
			   SP_INC4;
			   *(IEC_UDINT OS_SPTR *)pSP = *(IEC_UDINT OS_SPTR *)pSP / u; 
			}
			break;

		 case ASM_MOD_UDINT:
			STK_UCHK4(2);
			{
			   IEC_UDINT u = *(IEC_UDINT OS_SPTR *)pSP;
			   SP_INC4;
			   if (u != 0)
				  *(IEC_UDINT OS_SPTR *)pSP = *(IEC_UDINT OS_SPTR *)pSP % u; 
			   else
				  return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
			}
			break;
#endif /* defined(IP_CFG_UDINT) */

#if (defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD))
		 case ASM_GT_UDINT:
/*		 case ASM_GT_DWORD: */
			STK_UCHK4(2); /* 2 dword par on stack expected */
			{
			   IEC_BOOL  u;
			   IEC_UDINT b = *(IEC_UDINT OS_SPTR *)pSP;
			   SP_INC4;
			   u = (IEC_BOOL)((*(IEC_UDINT OS_SPTR *)pSP > b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_GE_UDINT:
/*		 case ASM_GE_DWORD: */
			STK_UCHK4(2); /* 2 dword par on stack expected */
			{
			   IEC_BOOL  u;
			   IEC_UDINT b = *(IEC_UDINT OS_SPTR *)pSP;
			   SP_INC4;
			   u = (IEC_BOOL)((*(IEC_UDINT OS_SPTR *)pSP >= b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_EQ_UDINT:
/*		 case ASM_EQ_DWORD: */
			STK_UCHK4(2); /* 2 dword par on stack expected */
			{
			   IEC_BOOL  u;
			   IEC_UDINT b = *(IEC_UDINT OS_SPTR *)pSP;
			   SP_INC4;
			   u = (IEC_BOOL)((*(IEC_UDINT OS_SPTR *)pSP == b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_LE_UDINT:
/*		 case ASM_LE_DWORD: */
			STK_UCHK4(2); /* 2 dword par on stack expected */
			{
			   IEC_BOOL  u;
			   IEC_UDINT b = *(IEC_UDINT OS_SPTR *)pSP;
			   SP_INC4;
			   u = (IEC_BOOL)((*(IEC_UDINT OS_SPTR *)pSP <= b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_LT_UDINT:
/*		 case ASM_LT_DWORD: */
			STK_UCHK4(2); /* 2 dword par on stack expected */
			{
			   IEC_BOOL  u;
			   IEC_UDINT b = *(IEC_UDINT OS_SPTR *)pSP;
			   SP_INC4;
			   u = (IEC_BOOL)((*(IEC_UDINT OS_SPTR *)pSP < b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_NE_UDINT:
/*		 case ASM_NE_DWORD: */
			STK_UCHK4(2); /* 2 dword par on stack expected */
			{
			   IEC_BOOL  u;
			   IEC_UDINT b = *(IEC_UDINT OS_SPTR *)pSP;
			   SP_INC4;
			   u = (IEC_BOOL)((*(IEC_UDINT OS_SPTR *)pSP != b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;
#endif /* defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) */

#if defined(IP_CFG_DWORD)
		 case ASM_SHL_DWORD:
			STACK_UCHK(REGI_DWORD + REGI_WORD);
			{
			   IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			   SP_INC2;
			   if ((u >=  32) ||
				   (u <= -32))
			   {
				  *(IEC_DWORD OS_SPTR *)pSP = (IEC_DWORD)0;
			   } else if (u >= 0) {
				  *(IEC_DWORD OS_SPTR *)pSP = (IEC_DWORD)(*(IEC_DWORD OS_SPTR *)pSP << u);
			   } else {
				  *(IEC_DWORD OS_SPTR *)pSP = (IEC_DWORD)(*(IEC_DWORD OS_SPTR *)pSP >> -u);
			   }
			}
			break;

		 case ASM_SHR_DWORD:
			STACK_UCHK(REGI_DWORD + REGI_WORD);
			{
			   IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			   SP_INC2;
			   if ((u >=  32) ||
				   (u <= -32))
			   {
				  *(IEC_DWORD OS_SPTR *)pSP = (IEC_DWORD)0;
			   } else if (u >= 0) {
				  *(IEC_DWORD OS_SPTR *)pSP = (IEC_DWORD)(*(IEC_DWORD OS_SPTR *)pSP >> u);
			   } else {
				  *(IEC_DWORD OS_SPTR *)pSP = (IEC_DWORD)(*(IEC_DWORD OS_SPTR *)pSP << -u);
			   }
			}
			break;

		 case ASM_ROR_DWORD:
			STACK_UCHK(REGI_DWORD + REGI_WORD);
			{
			   IEC_INT u = (IEC_INT)(*(IEC_INT OS_SPTR *)pSP & 31);
			   IEC_DWORD b;
			   SP_INC2;
			   b = *(IEC_DWORD OS_SPTR *)pSP;
			   for ( ; u > 0; u--)
			   {
				  if (b & 0x00000001)
				  {
					 b = (IEC_DWORD)((b >> 1) | 0x80000000);
				  } else {
					 b >>= 1;
				  }
			   }
			   *(IEC_DWORD OS_SPTR *)pSP = b;
			}
			break;

		 case ASM_ROL_DWORD:
			STACK_UCHK(REGI_DWORD + REGI_WORD);
			{
			   IEC_INT u = (IEC_INT)(*(IEC_INT OS_SPTR *)pSP & 31);
			   IEC_DWORD b;
			   SP_INC2;
			   b = *(IEC_DWORD OS_SPTR *)pSP;
			   for ( ; u > 0; u--)
			   {
				  if (b & 0x80000000)
				  {
					 b = (IEC_DWORD)((b << 1) | 0x00000001);
				  } else {
					 b <<= 1;
				  }
			   }
			   *(IEC_DWORD OS_SPTR *)pSP = b;
			}
			break;
#endif /* IP_CFG_DWORD */

#if defined(IP_CFG_DINT)

		 case ASM_GT_DINT:
			STK_UCHK4(2); /* 2 dword par on stack expected */
			{
			   IEC_BYTE u;
			   IEC_DINT b = *(IEC_DINT*)pSP;
			   SP_INC4;
			   u = (IEC_BYTE)((*(IEC_DINT*)pSP > b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_GE_DINT:
			STK_UCHK4(2); /* 2 dword par on stack expected */
			{
			   IEC_BYTE u;
			   IEC_DINT b = *(IEC_DINT*)pSP;
			   SP_INC4;
			   u = (IEC_BYTE)((*(IEC_DINT*)pSP >= b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_EQ_DINT:
			STK_UCHK4(2); /* 2 dword par on stack expected */
			{
			   IEC_BYTE u;
			   IEC_DINT b = *(IEC_DINT*)pSP;
			   SP_INC4;
			   u = (IEC_BYTE)((*(IEC_DINT*)pSP == b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_LE_DINT:
			STK_UCHK4(2); /* 2 dword par on stack expected */
			{
			   IEC_BYTE u;
			   IEC_DINT b = *(IEC_DINT*)pSP;
			   SP_INC4;
			   u = (IEC_BYTE)((*(IEC_DINT*)pSP <= b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_LT_DINT:
			STK_UCHK4(2); /* 2 dword par on stack expected */
			{
			   IEC_BYTE u;
			   IEC_DINT b = *(IEC_DINT*)pSP;
			   SP_INC4;
			   u = (IEC_BYTE)((*(IEC_DINT*)pSP < b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_NE_DINT:
			STK_UCHK4(2); /* 2 dword par on stack expected */
			{
			   IEC_BYTE u;
			   IEC_DINT b = *(IEC_DINT*)pSP;
			   SP_INC4;
			   u = (IEC_BYTE)((*(IEC_DINT*)pSP != b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

#endif /* IP_CFG_DINT */

#if defined(IP_CFG_DWORD)
		 case ASM_AND_DWORD  :
			STK_UCHK4(2);
			{
			   IEC_DWORD w = *(IEC_DWORD*)pSP;
			   SP_INC4;
			   *(IEC_DWORD*)pSP = (IEC_DWORD)(*(IEC_DWORD*)pSP & w);
			}
			break;

		 case ASM_OR_DWORD	 :
			STK_UCHK4(2);
			{
			   IEC_DWORD w = *(IEC_DWORD*)pSP;
			   SP_INC4;
			   *(IEC_DWORD*)pSP = (IEC_DWORD)(*(IEC_DWORD*)pSP | w);
			}
			break;

		 case ASM_XOR_DWORD  :
			STK_UCHK4(2);
			{
			   IEC_DWORD w = *(IEC_DWORD*)pSP;
			   SP_INC4;
			   *(IEC_DWORD*)pSP = (IEC_DWORD)(*(IEC_DWORD*)pSP ^ w);
			}
			break;

		 case ASM_NOT_DWORD  :
			STK_UCHK4(1);
			*(IEC_DWORD*)pSP = (IEC_DWORD) (~(*(IEC_DWORD*)pSP));
			break;
#endif /* IP_CFG_DWORD */

#if defined(IP_CFG_ULINT)

		 case ASM_ADD_ULINT:
			STK_UCHK8(2);
			{
			   IEC_ULINT u;
			   u.H = (*(IEC_ULINT*)pSP).H;
			   u.L = (*(IEC_ULINT*)pSP).L;
			   SP_INC8;
			   (*(IEC_ULINT*)pSP).H += u.H; 
			   (*(IEC_ULINT*)pSP).L += u.L; 
			   if ((*(IEC_ULINT*)pSP).L < u.L)
			   {
				  (*(IEC_ULINT*)pSP).H++;
			   }
			}
			break;

		 case ASM_MUL_ULINT:
			STK_UCHK8(2);
			{
			   IEC_ULINT u = *(IEC_ULINT*)pSP;
			   u.H = (*(IEC_ULINT*)pSP).H;
			   u.L = (*(IEC_ULINT*)pSP).L;
			   SP_INC8;
			   /* TBD !!! */
			   (*(IEC_ULINT*)pSP).H *= u.H; 
			   (*(IEC_ULINT*)pSP).L *= u.L; 
			}
			break;
#endif /* defined(IP_CFG_ULINT) */

#if (defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD))
		 case ASM_GT_ULINT:
/*		 case ASM_GT_LWORD: */
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BOOL  u;
			   IEC_ULINT b = *(IEC_ULINT OS_SPTR *)pSP;
			   SP_INC8;
			   u = (IEC_BOOL)
				  (( ((*(IEC_ULINT OS_SPTR *)pSP).H >  b.H) || 
					(((*(IEC_ULINT OS_SPTR *)pSP).H == b.H) && ((*(IEC_ULINT OS_SPTR *)pSP).L > b.L))) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_GE_ULINT:
/*		 case ASM_GE_LWORD: */
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BOOL  u;
			   IEC_ULINT b = *(IEC_ULINT OS_SPTR *)pSP;
			   SP_INC8;
			   u = (IEC_BOOL)
				  (( ((*(IEC_ULINT OS_SPTR *)pSP).H >  b.H) || 
					(((*(IEC_ULINT OS_SPTR *)pSP).H == b.H) && ((*(IEC_ULINT OS_SPTR *)pSP).L >= b.L))) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BOOL OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_EQ_ULINT:
/*		 case ASM_EQ_LWORD: */
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BYTE  u;
			   IEC_ULINT b = *(IEC_ULINT OS_SPTR *)pSP;
			   SP_INC8;
			   u = (IEC_BOOL)
				  ((((*(IEC_ULINT OS_SPTR *)pSP).H == b.H) && ((*(IEC_ULINT OS_SPTR *)pSP).L > b.L)) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BYTE OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_LE_ULINT:
/*		 case ASM_LE_LWORD: */
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BYTE  u;
			   IEC_ULINT b = *(IEC_ULINT OS_SPTR *)pSP;
			   SP_INC8;
			   u = (IEC_BOOL)
				  (( ((*(IEC_ULINT OS_SPTR *)pSP).H <  b.H) || 
					(((*(IEC_ULINT OS_SPTR *)pSP).H == b.H) && ((*(IEC_ULINT OS_SPTR *)pSP).L <= b.L))) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BYTE OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_LT_ULINT:
/*		 case ASM_LT_LWORD: */
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BYTE  u;
			   IEC_ULINT b = *(IEC_ULINT OS_SPTR *)pSP;
			   SP_INC8;
			   u = (IEC_BOOL)
				  (( ((*(IEC_ULINT OS_SPTR *)pSP).H <  b.H) || 
					(((*(IEC_ULINT OS_SPTR *)pSP).H == b.H) && ((*(IEC_ULINT OS_SPTR *)pSP).L < b.L))) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BYTE OS_SPTR *)pSP = u;
			}
			break;

		 case ASM_NE_ULINT:
/*		 case ASM_NE_LWORD: */
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BYTE  u;
			   IEC_ULINT b = *(IEC_ULINT OS_SPTR *)pSP;
			   SP_INC8;
			   u = (IEC_BOOL)
				  ((((*(IEC_ULINT OS_SPTR *)pSP).H != b.H) || ((*(IEC_ULINT OS_SPTR *)pSP).L != b.L)) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BYTE OS_SPTR *)pSP = u;
			}
			break;
#endif /* (defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD)) */

#if defined(IP_CFG_LINT)

		 case ASM_GT_LINT:
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_LINT b = *(IEC_LINT*)pSP;
			   SP_INC8;
			   u = (IEC_BOOL)
				  ((((*(IEC_LINT*)pSP).H >	b.H) ||
				   (((*(IEC_LINT*)pSP).H == b.H) && ((*(IEC_LINT*)pSP).L > b.L) )) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BOOL*)pSP = u;
			}
			break;

		 case ASM_GE_LINT:
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_LINT b = *(IEC_LINT*)pSP;
			   SP_INC8;
			   u = (IEC_BOOL)
				  ((((*(IEC_LINT*)pSP).H >	b.H) ||
				   (((*(IEC_LINT*)pSP).H == b.H) && ((*(IEC_LINT*)pSP).L >= b.L) )) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BOOL*)pSP = u;
			}
			break;

		 case ASM_EQ_LINT:
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_LINT b = *(IEC_LINT*)pSP;
			   SP_INC8;
			   u = (IEC_BOOL)
				  ((((*(IEC_LINT*)pSP).H == b.H) && ((*(IEC_LINT*)pSP).L == b.L) ) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BOOL*)pSP = u;
			}
			break;

		 case ASM_LE_LINT:
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_LINT b = *(IEC_LINT*)pSP;
			   SP_INC8;
			   u = (IEC_BOOL)
				  ((((*(IEC_LINT*)pSP).H <	b.H) ||
				   (((*(IEC_LINT*)pSP).H == b.H) && ((*(IEC_LINT*)pSP).L <= b.L) )) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BOOL*)pSP = u;
			}
			break;

		 case ASM_LT_LINT:
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_LINT b = *(IEC_LINT*)pSP;
			   SP_INC8;
			   u = (IEC_BOOL)
				  ((((*(IEC_LINT*)pSP).H <	b.H) ||
				   (((*(IEC_LINT*)pSP).H == b.H) && ((*(IEC_LINT*)pSP).L < b.L) )) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BOOL*)pSP = u;
			}
			break;

		 case ASM_NE_LINT:
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BOOL u;
			   IEC_LINT b = *(IEC_LINT*)pSP;
			   SP_INC8;
			   u = (IEC_BOOL)
				  ((((*(IEC_LINT*)pSP).H != b.H) || ((*(IEC_LINT*)pSP).L != b.L) ) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BOOL*)pSP = u;
			}
			break;

#endif /* IP_CFG_LINT */

#if defined(IP_CFG_REAL)

		 case ASM_GT_REAL:
			STK_UCHK4(2); /* 2 word par on stack expected */
			{
			   IEC_BYTE u;
			   IEC_REAL b = *(IEC_REAL*)pSP;
			   SP_INC4;
			   u = (IEC_BYTE)((*(IEC_REAL*)pSP > b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_GE_REAL:
			STK_UCHK4(2); /* 2 word par on stack expected */
			{
			   IEC_BYTE u;
			   IEC_REAL b = *(IEC_REAL*)pSP;
			   SP_INC4;
			   u = (IEC_BYTE)((*(IEC_REAL*)pSP >= b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_EQ_REAL:
			STK_UCHK4(2); /* 2 word par on stack expected */
			{
			   IEC_BYTE u;
			   IEC_REAL b = *(IEC_REAL*)pSP;
			   SP_INC4;
			   u = (IEC_BYTE)((*(IEC_REAL*)pSP == b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_LE_REAL:
			STK_UCHK4(2); /* 2 word par on stack expected */
			{
			   IEC_BYTE u;
			   IEC_REAL b = *(IEC_REAL*)pSP;
			   SP_INC4;
			   u = (IEC_BYTE)((*(IEC_REAL*)pSP <= b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_LT_REAL:
			STK_UCHK4(2); /* 2 word par on stack expected */
			{
			   IEC_BYTE u;
			   IEC_REAL b = *(IEC_REAL*)pSP;
			   SP_INC4;
			   u = (IEC_BYTE)((*(IEC_REAL*)pSP < b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_NE_REAL:
			STK_UCHK4(2); /* 2 word par on stack expected */
			{
			   IEC_BYTE u;
			   IEC_REAL b = *(IEC_REAL*)pSP;
			   SP_INC4;
			   u = (IEC_BYTE)((*(IEC_REAL*)pSP != b) ? 1 : 0);
			   SP_INC4;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

#endif /* IP_CFG_REAL */

#if defined(IP_CFG_STRING)
		 case ASM_MOVE_STRING:
			{
			   IEC_BYTE   OS_DPTR *s;
			   IEC_BYTE   OS_DPTR *d;
			   IEC_STRING OS_DPTR *ps;
			   IEC_STRING OS_DPTR *pd;
			   IEC_UINT   i;

			   POPPL(d);
			   POPPL(s);

			   ps = (IEC_STRING OS_DPTR *)s;
			   pd = (IEC_STRING OS_DPTR *)d;

			   OS_LOCK_MEM;
			   {
				   STRING_CHECK(ps);
				   STRING_CHECK(pd);

				   i = ps->CurLen;
				   if (i > pd->MaxLen) 
				   {
					  i = pd->MaxLen;
				   }
				   pd->CurLen = (IEC_STRLEN) i;
				   if (i != 0)
				   {
					   s = (IEC_BYTE OS_DPTR *)ps + 2;
					   d = (IEC_BYTE OS_DPTR *)pd + 2;
					   for ( ; i>0; i--)
					   {
						   *(d++) = *(s++);
					   }
				   }
			   
			   } OS_FREE_MEM;
			}	  
			break;

		 case ASM_INIT_STRING :
			{
			   IEC_STRING OS_DPTR *ps;
			   IEC_STRING OS_DPTR *pd;
			   IEC_BYTE   OS_DPTR *s;
			   IEC_BYTE   OS_DPTR *d;
			   IEC_UINT   i;
			   IEC_BYTE   b;
			   IP_BYTE(b);

			   POPPL(d);
			   POPPL(s);

			   ps = (IEC_STRING OS_DPTR *)s;
			   pd = (IEC_STRING OS_DPTR *)d;

			   OS_LOCK_MEM;
			   {
				   STRING_CHECK(ps);
				   STRING_CHECK(pd);

				   pd->MaxLen = (IEC_USINT) b;	/* init max strlen */
				   i = ps->CurLen;
				   if (i > b) 
				   {
					  i = b;
				   }
				   pd->CurLen = (IEC_STRLEN) i;
				   if (i != 0)
				   {
					   s = (IEC_BYTE OS_DPTR *)ps + 2;
					   d = (IEC_BYTE OS_DPTR *)pd + 2;
					   for ( ; i>0; i--)
					   {
						   *(d++) = *(s++);
					   }
					}

			   } OS_FREE_MEM;
			}	  
			break;

		 case ASM_GT_STRING:
		 case ASM_GE_STRING:
		 case ASM_EQ_STRING:
		 case ASM_LE_STRING:
		 case ASM_LT_STRING:
		 case ASM_NE_STRING:
			{
			   IEC_BYTE   OS_DPTR *s;
			   IEC_BYTE   OS_DPTR *d;
			   IEC_STRING OS_DPTR *p1;
			   IEC_STRING OS_DPTR *p2;
			   IEC_CHAR   u = 0;

			   POPPL(d);
			   POPPL(s);

			   p1 = (IEC_STRING OS_DPTR *)s;
			   p2 = (IEC_STRING OS_DPTR *)d;

			   OS_LOCK_MEM;
			   {
				   STRING_CHECK(p1);
				   STRING_CHECK(p2);

				   switch(*(pIP-1))
				   {
				   case ASM_GT_STRING:
					   u = (IEC_CHAR) (utilIECStrCmp(p1, p2) == 1 ? 1 : 0);
					   break;
				   case ASM_GE_STRING:
					   u = (IEC_CHAR) (utilIECStrCmp(p1, p2) >= 0 ? 1 : 0);
					   break;
				   case ASM_EQ_STRING:
					   u = (IEC_CHAR) (utilIECStrCmp(p1, p2) == 0 ? 1 : 0);
					   break;
				   case ASM_LE_STRING:
					   u = (IEC_CHAR) (utilIECStrCmp(p1, p2) <= 0 ? 1 : 0);
					   break;
				   case ASM_LT_STRING:
					   u = (IEC_CHAR) (utilIECStrCmp(p1, p2) ==-1 ? 1 : 0);
					   break;
				   case ASM_NE_STRING:
					   u = (IEC_CHAR) (utilIECStrCmp(p1, p2) != 0 ? 1 : 0);
					   break;
				   }

			   } OS_FREE_MEM;

			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}	  
			break;

#endif /* defined(IP_CFG_STRING) */

#if defined(IP_CFG_LREAL)

		 case ASM_GT_LREAL: 
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BYTE  u;
			   IEC_LREAL b = *(IEC_LREAL*)pSP;
			   SP_INC8;
			   u = (IEC_BYTE)((*(IEC_LREAL*)pSP > b) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_GE_LREAL: 
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BYTE  u;
			   IEC_LREAL b = *(IEC_LREAL*)pSP;
			   SP_INC8;
			   u = (IEC_BYTE)((*(IEC_LREAL*)pSP >= b) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_EQ_LREAL: 
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BYTE  u;
			   IEC_LREAL b = *(IEC_LREAL*)pSP;
			   SP_INC8;
			   u = (IEC_BYTE)((*(IEC_LREAL*)pSP == b) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_LE_LREAL: 
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BYTE  u;
			   IEC_LREAL b = *(IEC_LREAL*)pSP;
			   SP_INC8;
			   u = (IEC_BYTE)((*(IEC_LREAL*)pSP <= b) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_LT_LREAL: 
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BYTE  u;
			   IEC_LREAL b = *(IEC_LREAL*)pSP;
			   SP_INC8;
			   u = (IEC_BYTE)((*(IEC_LREAL*)pSP < b) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_NE_LREAL: 
			STK_UCHK8(2); /* 2 lword par on stack expected */
			{
			   IEC_BYTE  u;
			   IEC_LREAL b = *(IEC_LREAL*)pSP;
			   SP_INC8;
			   u = (IEC_BYTE)((*(IEC_LREAL*)pSP != b) ? 1 : 0);
			   SP_INC8;
			   SP_DEC1;
			   *(IEC_BYTE*)pSP = u;
			}
			break;

		 case ASM_NEG_LREAL:   
			STK_UCHK8(1);
			*(IEC_LREAL*)pSP = -(IEC_LREAL)(*(IEC_LREAL*)pSP); 
			break;

		 case ASM_ABS_LREAL:   
			STK_UCHK8(1);
			if (*(IEC_LREAL*)pSP < 0)
			{
			   *(IEC_LREAL*)pSP = -(IEC_LREAL)(*(IEC_LREAL*)pSP); 
			}
			break;

		 case ASM_ADD_LREAL:   
			STK_UCHK8(2);
			{
			   IEC_LREAL u = *(IEC_LREAL*)pSP;
			   SP_INC8;
			   *(IEC_LREAL*)pSP = *(IEC_LREAL*)pSP + u; 
			}
			break;

		 case ASM_MUL_LREAL:   
			STK_UCHK8(2);
			{
			   IEC_LREAL u = *(IEC_LREAL*)pSP;
			   SP_INC8;
			   *(IEC_LREAL*)pSP = *(IEC_LREAL*)pSP * u; 
			}
			break;

		 case ASM_SUB_LREAL:   
			STK_UCHK8(2);
			{
			   IEC_LREAL u = *(IEC_LREAL*)pSP;
			   SP_INC8;
			   *(IEC_LREAL*)pSP = *(IEC_LREAL*)pSP - u; 
			}
			break;

		 case ASM_DIV_LREAL:   
			STK_UCHK8(2);
			{
			   IEC_LREAL u = *(IEC_LREAL*)pSP;
			   if (u == 0.0)
			   {
				  return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
			   }
			   SP_INC8;
			   *(IEC_LREAL*)pSP = *(IEC_LREAL*)pSP / u; 
			}
			break;

		 case ASM_MOD_LREAL:   
			STK_UCHK8(2);
			{
			   IEC_LREAL u = *(IEC_LREAL*)pSP;
			   SP_INC8;
/* fmod from math.h should exist ?! */
			   *(IEC_LREAL*)pSP = (IEC_LREAL) OS_FMOD(*(IEC_LREAL*)pSP, u); 
			}
			break;

#endif /* IP_CFG_REAL */

#if defined(IP_CFG_LWORD)
		 case ASM_SHL_LWORD:   
			STACK_UCHK(REGI_LWORD + REGI_WORD);
			{
			   IEC_INT u = *(IEC_INT OS_SPTR *)pSP;
			   SP_INC2;
			   if (abs(u) >= sizeof(IEC_LWORD)*8)
			   {
				  (*(IEC_LWORD OS_SPTR *)pSP).L = 0;
				  (*(IEC_LWORD OS_SPTR *)pSP).H = 0;
			   } else if (u >= 0) {
				  (*(IEC_LWORD OS_SPTR *)pSP).H = ((*(IEC_LWORD OS_SPTR *)pSP).H << u) | 
												  ((*(IEC_LWORD OS_SPTR *)pSP).L >> -u);
				  (*(IEC_LWORD OS_SPTR *)pSP).L = ((*(IEC_LWORD OS_SPTR *)pSP).L << u);
			   } else {
				  (*(IEC_LWORD OS_SPTR *)pSP).L = ((*(IEC_LWORD OS_SPTR *)pSP).L >> u) | 
												  ((*(IEC_LWORD OS_SPTR *)pSP).H << -u);
				  (*(IEC_LWORD OS_SPTR *)pSP).H = ((*(IEC_LWORD OS_SPTR *)pSP).H >> u);
			   }
			}
			break;
#endif

		 case ASM_LEN_STRING_INT :
		   {
			   IEC_BYTE   OS_DPTR *s;
			   IEC_STRING OS_DPTR *str;

			   POPPL(s);
			   str = (IEC_STRING OS_DPTR *)s;

			   OS_LOCK_MEM;
			   {
					STRING_CHECK(str);
					PUSH16L(str->CurLen);
			   
			   } OS_FREE_MEM;
			   break;
			}

#if defined(IP_CFG_REAL)
		 case ASM_POW_REAL_REAL:
			 STK_UCHK4(2);
			 {
				 IEC_REAL in1;
				 IEC_REAL in2 = *(IEC_REAL OS_SPTR *)pSP;
				 SP_INC4;
				 in1 = *(IEC_REAL OS_SPTR *)pSP;

				 if (in1 < 0.0)
					 return intSetException(pVM, EXCEPT_MATH, pIP, pIN);
				 if (in2 < 0.0 && in1 == 0.0)
					 return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);

				 *(IEC_REAL OS_SPTR *)pSP = (IEC_REAL)(in2 == 0.0 ? 1.0 : (in1 != 0.0 ? OS_EXP(OS_LOG(in1)*in2) : 0.0));
			 }
			 break;
		 case ASM_POW_REAL_INT:
			 STK_UCHK4(1); 
			 STK_UCHK2(1); 
			 {
				 IEC_REAL in1;
				 IEC_REAL accu = (IEC_REAL)1.0;
				 IEC_INT in2 = *(IEC_INT OS_SPTR *)pSP;
				 SP_INC2;
				 in1 = *(IEC_REAL OS_SPTR *)pSP;

				 if (in2<0)
				 {
					 if (in1 == 0.0)
						 return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);

					 in1 = (IEC_REAL) (1.0/in1);
					 in2 = (IEC_INT) -in2;
				 }
				 for(;;)
				 { 
					 if ((in2 & 1) != 0)
						 accu *= in1;
					 
					 in2 >>= 1;
					 
					 if (in2==0)
						 break;
					 
					 in1 = in1*in1;
				 }
				 *(IEC_REAL OS_SPTR *)pSP = accu;
			 }
			 break;
		 case ASM_POW_REAL_DINT:
			 STK_UCHK4(1);	
			 STK_UCHK4(1);
			 {
				 IEC_REAL in1;
				 IEC_REAL accu = (IEC_REAL)1.0;
				 IEC_DINT in2 = *(IEC_DINT OS_SPTR *)pSP;
				 SP_INC4;
				 in1 = *(IEC_REAL OS_SPTR *)pSP;

				 if (in2<0)
				 {
					 if (in1 == 0.0)
						 return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);

					 in1 = (IEC_REAL) (1.0/in1);
					 in2 = -in2;
				 }
				 for(;;)
				 { 
					 if ((in2 & 1) != 0)
						 accu *= in1;
					 
					 in2 >>= 1;
					 
					 if (in2==0)
						 break;
					 
					 in1 = in1*in1;
				 }
				 *(IEC_REAL OS_SPTR *)pSP = accu;
			 }
			 break;
#endif /* IP_CFG_REAL */

#if defined(IP_CFG_LREAL)
		 case ASM_POW_LREAL_LREAL:
			 STK_UCHK8(2);
			 {
				 IEC_LREAL in1;
				 IEC_LREAL in2 = *(IEC_LREAL OS_SPTR *)pSP;
				 SP_INC8;
				 in1 = *(IEC_LREAL OS_SPTR *)pSP;

				 if (in1 < 0.0)
					 return intSetException(pVM, EXCEPT_MATH, pIP, pIN);
				 if (in2 < 0.0 && in1 == 0.0)
					 return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);

				 *(IEC_LREAL OS_SPTR *)pSP = (IEC_LREAL)(in2 == 0.0 ? 1.0 : (in1 != 0.0 ? OS_EXP(OS_LOG(in1)*in2) : 0.0));
			 }
			 break;
		 case ASM_POW_LREAL_INT:
			 STK_UCHK8(1);
			 STK_UCHK2(1);
			 {
				 IEC_LREAL in1;
				 IEC_LREAL accu = 1.0;
				 IEC_INT in2 = *(IEC_INT OS_SPTR *)pSP;
				 SP_INC2;
				 in1 = *(IEC_LREAL OS_SPTR *)pSP;

				 if (in2<0)
				 {
					 if (in1 == 0.0)
						 return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);

					 in1 = 1.0/in1;
					 in2 = (IEC_INT) -in2;
				 }
				 for(;;)
				 { 
					 if ((in2 & 1) != 0)
						 accu *= in1;
					 
					 in2 >>= 1;
					 
					 if (in2==0)
						 break;
					 
					 in1 = in1*in1;
				 }
				 *(IEC_LREAL OS_SPTR *)pSP = accu;
			 }
			 break;
		 case ASM_POW_LREAL_DINT:
			 STK_UCHK8(1);
			 STK_UCHK4(1);
			 {
				 IEC_LREAL in1;
				 IEC_LREAL accu = 1.0;
				 IEC_DINT in2 = *(IEC_DINT OS_SPTR *)pSP;
				 SP_INC4;
				 in1 = *(IEC_LREAL OS_SPTR *)pSP;

				 if (in2<0)
				 {
					 if (in1 == 0.0)
						 return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);

					 in1 = 1.0/in1;
					 in2 = -in2;
				 }
				 for(;;)
				 { 
					 if ((in2 & 1) != 0)
						 accu *= in1;
					 
					 in2 >>= 1;
					 
					 if (in2==0)
						 break;
					 
					 in1 = in1*in1;
				 }
				 *(IEC_LREAL OS_SPTR *)pSP = accu;
			 }
			 break;
#endif /* IP_CFG_LREAL */

#if defined(IP_CFG_REAL)
			 case ASM_NEG_REAL:
				STK_UCHK4(1);
				*(IEC_REAL OS_SPTR *)pSP = (IEC_REAL)-(*(IEC_REAL OS_SPTR *)pSP); 
				break;

			 case ASM_ABS_REAL:
				STK_UCHK4(1);
				if (*(IEC_REAL OS_SPTR *)pSP < 0)
				{
				   *(IEC_REAL OS_SPTR *)pSP = (IEC_REAL)-(*(IEC_REAL OS_SPTR *)pSP); 
				}
				break;

			 case ASM_ADD_REAL:
				STK_UCHK4(2);
				{
				   IEC_REAL u = *(IEC_REAL OS_SPTR *)pSP;
				   SP_INC4;
				   *(IEC_REAL OS_SPTR *)pSP = *(IEC_REAL OS_SPTR *)pSP + u; 
				}
				break;

			 case ASM_MUL_REAL:
				STK_UCHK4(2);
				{
				   IEC_REAL u = *(IEC_REAL OS_SPTR *)pSP;
				   SP_INC4;
				   *(IEC_REAL OS_SPTR *)pSP = *(IEC_REAL OS_SPTR *)pSP * u; 
				}
				break;

			 case ASM_SUB_REAL:
				STK_UCHK4(2);
				{
				   IEC_REAL u = *(IEC_REAL OS_SPTR *)pSP;
				   SP_INC4;
				   *(IEC_REAL OS_SPTR *)pSP = *(IEC_REAL OS_SPTR *)pSP - u; 
				}
				break;

			 case ASM_DIV_REAL:
				STK_UCHK4(2);
				{
				   IEC_REAL u = *(IEC_REAL OS_SPTR *)pSP;
				   if (u == 0.0)
				   {
					  return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
				   }
				   SP_INC4;
				   *(IEC_REAL OS_SPTR *)pSP = *(IEC_REAL OS_SPTR *)pSP / u; 
				}
				break;

			 case ASM_MOD_REAL: 
				STK_UCHK4(2);
				{
				   IEC_REAL u = *(IEC_REAL OS_SPTR *)pSP;
				   SP_INC4;
				   *(IEC_REAL OS_SPTR *)pSP = (IEC_REAL) OS_FMOD(*(IEC_REAL OS_SPTR *)pSP, u); 
				}
				break;
#endif /* IP_CFG_REAL */

#if defined(IP_CFG_DINT)

	  case ASM_NEG_DINT:
		 STK_UCHK4(1);
		 *(IEC_DINT OS_SPTR *)pSP = -(IEC_DINT)(*(IEC_DINT OS_SPTR *)pSP); 
		 break;

	  case ASM_ABS_DINT:
		 STK_UCHK4(1);
		 if (*(IEC_DINT OS_SPTR *)pSP < 0)
		 {
			*(IEC_DINT OS_SPTR *)pSP = -(IEC_DINT)(*(IEC_DINT OS_SPTR *)pSP); 
		 }
		 break;

	  case ASM_ADD_DINT:
		 STK_UCHK4(2);
		 {
			IEC_DINT u = *(IEC_DINT OS_SPTR *)pSP;
			SP_INC4;
			*(IEC_DINT OS_SPTR *)pSP = *(IEC_DINT OS_SPTR *)pSP + u; 
		 }
		 break;

	  case ASM_MUL_DINT:
		 STK_UCHK4(2);
		 {
			IEC_DINT u = *(IEC_DINT OS_SPTR *)pSP;
			SP_INC4;
			*(IEC_DINT OS_SPTR *)pSP = *(IEC_DINT OS_SPTR *)pSP * u; 
		 }
		 break;

	  case ASM_SUB_DINT:
		 STK_UCHK4(2);
		 {
			IEC_DINT u = *(IEC_DINT OS_SPTR *)pSP;
			SP_INC4;
			*(IEC_DINT OS_SPTR *)pSP = *(IEC_DINT OS_SPTR *)pSP - u; 
		 }
		 break;

	  case ASM_DIV_DINT:
		 STK_UCHK4(2);
		 {
			IEC_DINT u = *(IEC_DINT OS_SPTR *)pSP;
			if (u == 0)
			{
			   return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
			}
			SP_INC4;
			*(IEC_DINT OS_SPTR *)pSP = *(IEC_DINT OS_SPTR *)pSP / u; 
		 }
		 break;

	  case ASM_MOD_DINT: 
		 STK_UCHK4(2);
		 {
			IEC_DINT u = *(IEC_DINT OS_SPTR *)pSP;
			SP_INC4;
			if (u != 0)
			   *(IEC_DINT OS_SPTR *)pSP = *(IEC_DINT OS_SPTR *)pSP % u; 
			else
			   return intSetException(pVM, EXCEPT_DIVISION_BY_ZERO, pIP, pIN);
		 }
		 break;
#endif /* IP_CFG_DINT */


/* ---------------------------------------------------------------------------- */
