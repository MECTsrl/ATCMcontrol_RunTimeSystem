
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
 * Filename: intConv.c
 */


/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_BOOL) 

#if defined(IP_CFG_USINT) || defined(IP_CFG_BYTE) || defined(IP_CFG_SINT) /* -- */
	case ASM_BOOL_TO_USINT:
 /* case ASM_BOOL_TO_BYTE : */
	case ASM_BOOL_TO_SINT :
		STK_UCHK1(1);
		*(IEC_USINT OS_SPTR *)pSP = (IEC_USINT)((*(IEC_BOOL OS_SPTR *)pSP & 1)?1:0);
		break;
#endif
			
#if defined(IP_CFG_UINT) || defined(IP_CFG_WORD) || defined(IP_CFG_INT) /* ---- */
	case ASM_BOOL_TO_UINT :
 /* case ASM_BOOL_TO_WORD : */
	case ASM_BOOL_TO_INT  :
		STK_UCHK1(1);
	  #if defined(IP_CFG_STACK8)
		STACK_OCHK(1);
		{
			IEC_UINT u = (IEC_UINT)((*(IEC_BOOL OS_SPTR *)pSP & 1)?1:0);
			pSP--;
			*(IEC_UINT OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_UINT OS_SPTR *)pSP = (IEC_UINT) ((*(IEC_BOOL OS_SPTR *)pSP & 1)?1:0);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) || defined(IP_CFG_DINT) /* - */
	case ASM_BOOL_TO_UDINT :
 /* case ASM_BOOL_TO_DWORD : */
	case ASM_BOOL_TO_DINT  :
		STK_UCHK1(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		STACK_OCHK(CNV_NEW(4,1));
		{
			IEC_UDINT u = (IEC_UDINT)((*(IEC_BOOL OS_SPTR *)pSP & 1)?1:0);
			pSP -= CNV_NEW(4,1);
			*(IEC_UDINT OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_UDINT OS_SPTR *)pSP = (IEC_UDINT)((*(IEC_BOOL OS_SPTR *)pSP & 1)?1:0);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_REAL) /* --------------------------------------------------- */
	case ASM_BOOL_TO_REAL : 
		STK_UCHK1(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		STACK_OCHK(CNV_NEW(4,1));
		{
			IEC_REAL u = (IEC_REAL)((*(IEC_BOOL OS_SPTR *)pSP & 1)?1:0);
			pSP -= CNV_NEW(4,1);
			*(IEC_REAL OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_REAL OS_SPTR *)pSP = (IEC_REAL)((*(IEC_BOOL OS_SPTR *)pSP & 1)?1:0);
	  #endif
		break;
#endif
	  
#if defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD) || defined(IP_CFG_LINT) /* - */
	case ASM_BOOL_TO_ULINT :
 /* case ASM_BOOL_TO_LWORD : */
	case ASM_BOOL_TO_LINT  :
		STK_UCHK1(1);
		STACK_OCHK(CNV_NEW(8,1));
		{
			IEC_ULINT u;
			u.H = 0;
			u.L = (IEC_DWORD)((*(IEC_BOOL OS_SPTR *)pSP & 1)?1:0);
			pSP -= CNV_NEW(8,1);
			*(IEC_ULINT OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_LREAL) /* -------------------------------------------------- */
	case ASM_BOOL_TO_LREAL:
		STK_UCHK1(1);
		STACK_OCHK(CNV_NEW(8,1));
		{
			IEC_LREAL u = (IEC_LREAL)((*(IEC_BOOL OS_SPTR *)pSP & 1)?1:0);
			pSP -= CNV_NEW(8,1);
			*(IEC_LREAL OS_SPTR *)pSP = u;
		}
		break;
#endif

#endif /* IP_CFG_BOOL */

/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_USINT) || defined(IP_CFG_BYTE)

#if defined(IP_CFG_BOOL) /* --------------------------------------------------- */
	case ASM_USINT_TO_BOOL :
 /* case ASM_BYTE_TO_BOOL  : */
		STK_UCHK1(1);
		*(IEC_BOOL OS_SPTR *)pSP = (IEC_BYTE)((*(IEC_USINT OS_SPTR *)pSP)==0?0:1);
		break;
#endif
		
	case ASM_USINT_TO_BYTE : /* ----------------------------------------------- */
 /* case ASM_USINT_TO_USINT: */
 /* case ASM_BYTE_TO_USINT : */
		STK_UCHK1(1);
		break;
		
  #if defined(IP_CFG_SINT) /* ------------------------------------------------- */
	case ASM_USINT_TO_SINT:
 /* case ASM_BYTE_TO_SINT : */
		STK_UCHK1(1);
		break;
#endif
		
#if defined(IP_CFG_UINT) || defined(IP_CFG_WORD) || defined (IP_CFG_INT) /* --- */
	case ASM_USINT_TO_UINT:
 /* case ASM_USINT_TO_WORD: */
 /* case ASM_BYTE_TO_UINT : */
 /* case ASM_BYTE_TO_WORD : */ 
	case ASM_USINT_TO_INT : 
 /* case ASM_BYTE_TO_INT  : */
		STK_UCHK1(1);
	  #if defined(IP_CFG_STACK8)
		STACK_OCHK(1);
		{
			IEC_UINT u = (IEC_UINT)(*(IEC_USINT OS_SPTR *)pSP);
			pSP--;
			*(IEC_UINT OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_UINT OS_SPTR *)pSP = (IEC_UINT)(*(IEC_USINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) || defined(IP_CFG_DINT) /* - */
	case ASM_USINT_TO_UDINT:
 /* case ASM_USINT_TO_DWORD: */
 /* case ASM_BYTE_TO_UDINT : */
 /* case ASM_BYTE_TO_DWORD : */
	case ASM_USINT_TO_DINT :	
 /* case ASM_BYTE_TO_DINT  : */
		STK_UCHK1(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		STACK_OCHK(CNV_NEW(4,1));
		{
			IEC_UDINT u = (IEC_UDINT)(*(IEC_USINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(4,1);
			*(IEC_UDINT OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_UDINT OS_SPTR *)pSP = (IEC_UDINT)(*(IEC_USINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_REAL) /* --------------------------------------------------- */
	case ASM_USINT_TO_REAL: 
 /* case ASM_BYTE_TO_REAL : */
		STK_UCHK1(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		STACK_OCHK(CNV_NEW(4,1));
		{
			IEC_REAL u = (IEC_REAL)(*(IEC_USINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(4,1);
			*(IEC_REAL OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_REAL OS_SPTR *)pSP = (IEC_REAL)(*(IEC_USINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD) || defined(IP_CFG_LINT) /* - */
	case ASM_USINT_TO_ULINT:
 /* case ASM_USINT_TO_LWORD: */
 /* case ASM_BYTE_TO_ULINT : */
 /* case ASM_BYTE_TO_LWORD : */
	case ASM_USINT_TO_LINT :
 /* case ASM_BYTE_TO_LINT  : */
		STK_UCHK1(1);
		STACK_OCHK(CNV_NEW(8,1));
		{
			IEC_ULINT u;
			u.H = 0;
			u.L = (IEC_DWORD)(*(IEC_USINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,1);
			*(IEC_ULINT OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_LREAL) /* -------------------------------------------------- */
	case ASM_USINT_TO_LREAL: 
 /* case ASM_BYTE_TO_LREAL : */
		STK_UCHK1(1);
		STACK_OCHK(CNV_NEW(8,1));
		{
			IEC_LREAL u = (IEC_LREAL)(*(IEC_USINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,1);
			*(IEC_LREAL OS_SPTR *)pSP = u;
		}
		break;
#endif

#endif /* IP_CFG_USINT || IP_CFG_BYTE */

/* ---------------------------------------------------------------------------- */
	  
#if defined(IP_CFG_SINT)

#if defined(IP_CFG_BOOL) /* --------------------------------------------------- */
	case ASM_SINT_TO_BOOL : 
		STK_UCHK1(1);
		*(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_SINT OS_SPTR *)pSP)==0?0:1);
		break;
#endif
	  
#if defined(IP_CFG_USINT) || defined(IP_CFG_BYTE)	/* ------------------------ */
	case ASM_SINT_TO_USINT:
 /* case ASM_SINT_TO_BYTE : */
		STK_UCHK1(1);
		break;
#endif
			
#if defined(IP_CFG_UINT) || defined(IP_CFG_WORD) || defined(IP_CFG_INT) /* ---- */
	case ASM_SINT_TO_UINT : 
 /* case ASM_SINT_TO_WORD : */
	case ASM_SINT_TO_INT  :
		STK_UCHK1(1);
	  #if defined(IP_CFG_STACK8)
		STACK_OCHK(1);
		{
			IEC_UINT u = (IEC_UINT)(*(IEC_SINT OS_SPTR *)pSP);
			pSP--;
			*(IEC_UINT OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_UINT OS_SPTR *)pSP = (IEC_UINT)(*(IEC_SINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) || defined(IP_CFG_DINT) /* - */
	case ASM_SINT_TO_UDINT:
 /* case ASM_SINT_TO_DWORD: */
	case ASM_SINT_TO_DINT :
		STK_UCHK1(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		STACK_OCHK(CNV_NEW(4,1));
		{
			IEC_UDINT u = (IEC_UDINT)(*(IEC_SINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(4,1);
			*(IEC_UDINT OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_UDINT OS_SPTR *)pSP = (IEC_UDINT)(*(IEC_SINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_REAL) /* --------------------------------------------------- */
	case ASM_SINT_TO_REAL :
		STK_UCHK1(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		STACK_OCHK(CNV_NEW(4,1));
		{
			IEC_REAL u = (IEC_REAL)(*(IEC_SINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(4,1);
			*(IEC_REAL OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_REAL OS_SPTR *)pSP = (IEC_REAL)(*(IEC_SINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD) || defined(IP_CFG_LINT) /* - */
	case ASM_SINT_TO_ULINT:
 /* case ASM_SINT_TO_LWORD: */
	case ASM_SINT_TO_LINT :
		STK_UCHK1(1);
		STACK_OCHK(CNV_NEW(8,1));
		{
			IEC_ULINT u;
			u.H = (*(IEC_SINT OS_SPTR *)pSP < 0) ? 0xFFFFFFFF : 0;
			u.L = (IEC_DWORD)(*(IEC_SINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,1);
			*(IEC_ULINT OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_LREAL) /* -------------------------------------------------- */
	case ASM_SINT_TO_LREAL:
		STK_UCHK1(1);
		STACK_OCHK(CNV_NEW(8,1));
		{
			IEC_LREAL u = (IEC_LREAL)(*(IEC_SINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,1);
			*(IEC_LREAL OS_SPTR *)pSP = u;
		}
		break;
#endif

#endif /* IP_CFG_SINT */

/* ---------------------------------------------------------------------------- */
	  
#if defined(IP_CFG_UINT) || defined(IP_CFG_WORD)

#if defined(IP_CFG_BOOL) /* --------------------------------------------------- */ 
	case ASM_UINT_TO_BOOL :
 /* case ASM_WORD_TO_BOOL : */
		STK_UCHK2(1);
	  #if defined(IP_CFG_STACK8)
		{
			IEC_BOOL u = (IEC_BOOL)((*(IEC_UINT OS_SPTR *)pSP)==0?0:1);
			pSP++;
			*(IEC_BOOL OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_UINT OS_SPTR *)pSP)==0?0:1);
	  #endif
		break;
#endif
	  
#if defined(IP_CFG_USINT) || defined(IP_CFG_BYTE) || defined(IP_CFG_SINT) /* -- */
	case ASM_UINT_TO_USINT:
 /* case ASM_UINT_TO_BYTE : */
 /* case ASM_WORD_TO_USINT: */
 /* case ASM_WORD_TO_BYTE : */
	case ASM_UINT_TO_SINT :
		STK_UCHK2(1);
	  #if defined(IP_CFG_STACK8)
		{
			IEC_USINT u = (IEC_USINT)(*(IEC_UINT OS_SPTR *)pSP);
			pSP++;
			*(IEC_USINT OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_USINT OS_SPTR *)pSP = (IEC_USINT)(*(IEC_UINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			  
#if defined(IP_CFG_UINT) || defined(IP_CFG_WORD) || defined(IP_CFG_INT) /* ---- */
	case ASM_UINT_TO_WORD :
 /* case ASM_WORD_TO_UINT : */
	case ASM_UINT_TO_INT  :
		STK_UCHK2(1);
		break;
#endif
	  
#if defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) || defined(IP_CFG_DINT) /* - */
	case ASM_UINT_TO_UDINT:   
 /* case ASM_UINT_TO_DWORD: */
 /* case ASM_WORD_TO_UDINT: */
 /* case ASM_WORD_TO_DWORD: */
	case ASM_UINT_TO_DINT :   
 /* case ASM_WORD_TO_DINT : */
		STK_UCHK2(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		STACK_OCHK(CNV_NEW(4,2));
		{
			IEC_UDINT u = (IEC_UDINT)(*(IEC_UINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(4,2);
			*(IEC_UDINT OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_UDINT OS_SPTR *)pSP = (IEC_UDINT)(*(IEC_UINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_REAL) /* --------------------------------------------------- */
	case ASM_UINT_TO_REAL :
 /* case ASM_WORD_TO_REAL : */
		STK_UCHK2(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		STACK_OCHK(CNV_NEW(4,2));
		{
			IEC_REAL u = (IEC_REAL)(*(IEC_UINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(4,2);
			*(IEC_REAL OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_REAL OS_SPTR *)pSP = (IEC_REAL)(*(IEC_UINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD) || defined(IP_CFG_LINT) /* - */
	case ASM_UINT_TO_ULINT:   
 /* case ASM_UINT_TO_LWORD: */
 /* case ASM_WORD_TO_ULINT: */
 /* case ASM_WORD_TO_LWORD: */
	case ASM_UINT_TO_LINT :   
 /* case ASM_WORD_TO_LINT : */
		STK_UCHK2(1);
		STACK_OCHK(CNV_NEW(8,2));
		{
			IEC_ULINT u;
			u.H = 0;
			u.L = (IEC_DWORD)(*(IEC_UINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,2);
			*(IEC_ULINT OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_LREAL) /* -------------------------------------------------- */
	case ASM_UINT_TO_LREAL: 
 /* case ASM_WORD_TO_LREAL: */
		STK_UCHK2(1);
		STACK_OCHK(CNV_NEW(8,2));
		{
			IEC_LREAL u = (IEC_LREAL)(*(IEC_UINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,2);
			*(IEC_LREAL OS_SPTR *)pSP = u;
		}
		break;
#endif

#endif /* IP_CFG_UINT || IP_CFG_WORD */

/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_INT)

#if defined(IP_CFG_BOOL) /* --------------------------------------------------- */ 
	case ASM_INT_TO_BOOL  :
		STK_UCHK2(1);
	  #if defined(IP_CFG_STACK8)
		{
			IEC_BOOL u = (IEC_BOOL)((*(IEC_INT OS_SPTR *)pSP)==0?0:1);
			pSP++;
			*(IEC_BOOL OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_INT OS_SPTR *)pSP)==0?0:1);
	  #endif
		break;
#endif
	  
#if defined(IP_CFG_USINT) || defined(IP_CFG_BYTE) || defined(IP_CFG_SINT)
	case ASM_INT_TO_USINT :
 /* case ASM_INT_TO_BYTE  : */
	case ASM_INT_TO_SINT  :
		STK_UCHK2(1);
	  #if defined(IP_CFG_STACK8)
		{
			IEC_USINT u = (IEC_USINT)(*(IEC_INT OS_SPTR *)pSP);
			pSP++;
			*(IEC_USINT OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_USINT OS_SPTR *)pSP = (IEC_USINT)(*(IEC_INT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_UINT) || defined(IP_CFG_WORD) /* --------------------------- */
	case ASM_INT_TO_UINT :	 
 /* case ASM_INT_TO_WORD : */
		STK_UCHK2(1);
		break;
#endif
			
#if defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) || defined(IP_CFG_DINT) /* - */
	case ASM_INT_TO_UDINT :   
 /* case ASM_INT_TO_DWORD : */
	case ASM_INT_TO_DINT  :
		STK_UCHK2(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		STACK_OCHK(CNV_NEW(4,2));
		{
			IEC_INT u = (*(IEC_INT OS_SPTR *)pSP);
			pSP -= CNV_NEW(4,2);
			*(IEC_UDINT OS_SPTR *)pSP = (IEC_UDINT) u;
		}
	  #else 
		*(IEC_UDINT OS_SPTR *)pSP = (IEC_UDINT)(*(IEC_INT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_REAL) /* --------------------------------------------------- */
	case ASM_INT_TO_REAL  :   
		STK_UCHK2(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		STACK_OCHK(CNV_NEW(4,2));
		{
			IEC_REAL u = (IEC_REAL)(*(IEC_INT OS_SPTR *)pSP);
			pSP -= CNV_NEW(4,2);
			*(IEC_REAL OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_REAL OS_SPTR *)pSP = (IEC_REAL)(*(IEC_INT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD) || defined(IP_CFG_LINT) /* - */
	case ASM_INT_TO_ULINT :
 /* case ASM_INT_TO_LWORD : */
	case ASM_INT_TO_LINT  :   
		STK_UCHK2(1);
		STACK_OCHK(CNV_NEW(8,2));
		{
			IEC_ULINT u;
			u.H = (*(IEC_INT OS_SPTR *)pSP < 0) ? 0xFFFFFFFF : 0;
			u.L = (IEC_DWORD)(*(IEC_INT OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,2);
			*(IEC_ULINT OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_LREAL) /* -------------------------------------------------- */
	case ASM_INT_TO_LREAL :
		STK_UCHK2(1);
		STACK_OCHK(CNV_NEW(8,2));
		{
			IEC_LREAL u = (IEC_LREAL)(*(IEC_INT OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,2);
			*(IEC_LREAL OS_SPTR *)pSP = u;
		}
		break;
#endif

#endif /* IP_CFG_INT */

/* ---------------------------------------------------------------------------- */
	  
#if defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD)

 #if defined(IP_CFG_BOOL) /* -------------------------------------------------- */ 
	case ASM_UDINT_TO_BOOL :
 /* case ASM_DWORD_TO_BOOL : */
		STK_UCHK4(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		{
			IEC_BOOL u = (IEC_BOOL)((*(IEC_UDINT OS_SPTR *)pSP)==0?0:1);
			pSP += CNV_NEW(4,1);
			*(IEC_BOOL OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_UDINT OS_SPTR *)pSP)==0?0:1);
	  #endif
		break;
#endif
	  
#if defined(IP_CFG_USINT) || defined(IP_CFG_BYTE) || defined(IP_CFG_SINT) /* -- */
	case ASM_UDINT_TO_USINT:
 /* case ASM_UDINT_TO_BYTE : */
 /* case ASM_DWORD_TO_USINT: */
 /* case ASM_DWORD_TO_BYTE : */
	case ASM_UDINT_TO_SINT :	 
 /* case ASM_DWORD_TO_SINT : */
		STK_UCHK4(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		{
			IEC_USINT u = (IEC_USINT)(*(IEC_UDINT OS_SPTR *)pSP);
			pSP += CNV_NEW(4,1);
			*(IEC_USINT OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_USINT OS_SPTR *)pSP = (IEC_USINT)(*(IEC_UDINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_UINT) || defined(IP_CFG_WORD) || defined(IP_CFG_INT) /* ---- */
	case ASM_UDINT_TO_UINT :
 /* case ASM_UDINT_TO_WORD : */
 /* case ASM_DWORD_TO_UINT : */
 /* case ASM_DWORD_TO_WORD : */
	case ASM_UDINT_TO_INT  :   
 /* case ASM_DWORD_TO_INT  : */
		STK_UCHK4(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		{
			IEC_UINT u = (IEC_UINT)(*(IEC_UDINT OS_SPTR *)pSP);
			pSP += CNV_NEW(4,2);
			*(IEC_UINT OS_SPTR *)pSP = u;
		}
	  #else 
		*(IEC_UINT OS_SPTR *)pSP = (IEC_UINT)(*(IEC_UDINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) || defined(IP_CFG_DINT) /* - */
	case ASM_UDINT_TO_DWORD:   
 /* case ASM_DWORD_TO_UDINT: */
 /* case ASM_UDINT_TO_DINT : */
 /* case ASM_DWORD_TO_DINT : */
		STK_UCHK4(1);
		break;
#endif
	  
#if defined(IP_CFG_REAL) /* --------------------------------------------------- */ 
	case ASM_UDINT_TO_REAL :   
 /* case ASM_DWORD_TO_REAL : */
		STK_UCHK4(1);
		*(IEC_REAL OS_SPTR *)pSP = (IEC_REAL)(*(IEC_UDINT OS_SPTR *)pSP);
		break;
#endif
			
#if defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD) || defined(IP_CFG_LINT) /* - */
	case ASM_UDINT_TO_ULINT:   
 /* case ASM_UDINT_TO_LWORD: */
 /* case ASM_DWORD_TO_ULINT: */
 /* case ASM_DWORD_TO_LWORD: */
 /* case ASM_UDINT_TO_LINT : */
 /* case ASM_DWORD_TO_LINT : */
		STK_UCHK4(1);
		STACK_OCHK(CNV_NEW(8,4));
		{
			IEC_ULINT u;
			u.H = 0;
			u.L = (IEC_DWORD)(*(IEC_UDINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,4);
			*(IEC_ULINT OS_SPTR *)pSP = u;
		}
		break;
#endif

#if defined(IP_CFG_LREAL) /* -------------------------------------------------- */
	case ASM_UDINT_TO_LREAL:  
 /* case ASM_DWORD_TO_LREAL: */
		STK_UCHK4(1);
		STACK_OCHK(CNV_NEW(8,4));
		{
			IEC_LREAL u = (IEC_LREAL)(*(IEC_UDINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,4);
			*(IEC_LREAL OS_SPTR *)pSP = u;
		}
		break;
#endif

#endif /* IP_CFG_UDINT || IP_CFG_DWORD */

/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_DINT)

#if defined(IP_CFG_BOOL) /* --------------------------------------------------- */ 
	case ASM_DINT_TO_BOOL  :
		STK_UCHK4(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		{
			IEC_BOOL u = (IEC_BOOL)((*(IEC_DINT OS_SPTR *)pSP)==0?0:1);
			pSP += CNV_NEW(4,1);
			*(IEC_BOOL OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_DINT OS_SPTR *)pSP)==0?0:1);
	  #endif
		break;
#endif
	  
#if defined(IP_CFG_USINT) || defined(IP_CFG_BYTE) || defined(IP_CFG_SINT) /* -- */
	case ASM_DINT_TO_USINT :
 /* case ASM_DINT_TO_BYTE  : */
	case ASM_DINT_TO_SINT  :
		STK_UCHK4(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		{
			IEC_USINT u = (IEC_USINT)(*(IEC_DINT OS_SPTR *)pSP);
			pSP += CNV_NEW(4,1);
			*(IEC_USINT OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_USINT OS_SPTR *)pSP = (IEC_USINT)(*(IEC_DINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
			
#if defined(IP_CFG_UINT) || defined(IP_CFG_WORD) || defined(IP_CFG_INT) /* ---- */
	case ASM_DINT_TO_UINT  :   
 /* case ASM_DINT_TO_WORD  : */
	case ASM_DINT_TO_INT   :
		STK_UCHK4(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		{
			IEC_UINT u = (IEC_UINT)(*(IEC_DINT OS_SPTR *)pSP);
			pSP += CNV_NEW(4,2);
			*(IEC_UINT OS_SPTR *)pSP = u;
		}
	  #else 
		*(IEC_UINT OS_SPTR *)pSP = (IEC_UINT)(*(IEC_DINT OS_SPTR *)pSP);
	  #endif
		break;
#endif
  
#if defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) /* ------------------------- */
	case ASM_DINT_TO_UDINT :   
 /* case ASM_DINT_TO_DWORD : */
		STK_UCHK4(1);
		break;
#endif
	  
#if defined(IP_CFG_REAL) /* --------------------------------------------------- */ 
	case ASM_DINT_TO_REAL  :   
		STK_UCHK4(1);
		*(IEC_REAL OS_SPTR *)pSP = (IEC_REAL)(*(IEC_DINT OS_SPTR *)pSP);
		break;
#endif
			
#if defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD) || defined(IP_CFG_LINT) /* - */
	case ASM_DINT_TO_ULINT :   
 /* case ASM_DINT_TO_LWORD : */
 /* case ASM_DINT_TO_LINT  : */
		STK_UCHK4(1);
		STACK_OCHK(CNV_NEW(8,4));
		{
			IEC_ULINT u;
			u.H = (*(IEC_DINT OS_SPTR *)pSP < 0) ? 0xFFFFFFFF : 0;
			u.L = (IEC_DWORD)(*(IEC_DINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,4);
			*(IEC_ULINT OS_SPTR *)pSP = u;
		}
		break;
#endif
	  
#if defined(IP_CFG_LREAL) /* -------------------------------------------------- */
	case ASM_DINT_TO_LREAL :
		STK_UCHK4(1);
		STACK_OCHK(CNV_NEW(8,4));
		{
			IEC_LREAL u = (IEC_LREAL)(*(IEC_DINT OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,4);
			*(IEC_LREAL OS_SPTR *)pSP = u;
		}
		break;
#endif

#endif /* IP_CFG_DINT */
	  
/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_REAL)

#if defined(IP_CFG_BOOL) /* --------------------------------------------------- */ 
	case ASM_REAL_TO_BOOL :
		STK_UCHK4(1);
	  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
		{
			IEC_BOOL u = (IEC_BOOL)((*(IEC_REAL OS_SPTR *)pSP)==(IEC_REAL)0.0?0:1);
			pSP += CNV_NEW(4,1);
			*(IEC_BOOL OS_SPTR *)pSP = u;
		}
	  #else
		*(IEC_BOOL OS_SPTR *)pSP = (IEC_BOOL)((*(IEC_REAL OS_SPTR *)pSP)==(IEC_REAL)0.0?0:1);
	  #endif
		break;
#endif
	  
#if defined(IP_CFG_USINT) || defined(IP_CFG_BYTE) /* -------------------------- */
	case ASM_REAL_TO_USINT:
 /* case ASM_REAL_TO_BYTE : */
		STK_UCHK4(1);
		{
			IEC_REAL aReal = *(IEC_REAL OS_SPTR *)pSP;
			IEC_USINT u = (IEC_USINT)(*(IEC_REAL OS_SPTR *)pSP);

		  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
			pSP += CNV_NEW(4,1);
		  #endif

			*(IEC_USINT OS_SPTR *)pSP = (IEC_USINT)utilRealToDint(aReal, u, 0, 0xffu);
		}
		break;
#endif
	  
#if defined(IP_CFG_SINT) /* --------------------------------------------------- */ 
	case ASM_REAL_TO_SINT :
		STK_UCHK4(1);
		{
			IEC_REAL aReal = *(IEC_REAL OS_SPTR *)pSP;
			IEC_SINT u = (IEC_SINT)(*(IEC_REAL OS_SPTR *)pSP);

		  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
			pSP += CNV_NEW(4,1);
		  #endif

			*(IEC_SINT OS_SPTR *)pSP = (IEC_SINT)utilRealToDint(aReal, u, IEC_MIN_SINT, IEC_MAX_SINT);
		}
		break;
#endif
	  
#if defined(IP_CFG_UINT) || defined(IP_CFG_WORD) /* --------------------------- */
	case ASM_REAL_TO_UINT :
 /* case ASM_REAL_TO_WORD : */
		STK_UCHK4(1);
		{
			IEC_REAL aReal = *(IEC_REAL OS_SPTR *)pSP;
			IEC_UINT u = (IEC_UINT)(*(IEC_REAL OS_SPTR *)pSP);

		  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
			pSP += CNV_NEW(4,2);
		  #endif

			*(IEC_UINT OS_SPTR *)pSP = (IEC_UINT)utilRealToDint(aReal, u, 0, 0xffffu);
		}
		break;
#endif
	  
#if defined(IP_CFG_INT) /* ---------------------------------------------------- */
	case ASM_REAL_TO_INT :
		STK_UCHK4(1);
		{
			IEC_REAL aReal = *(IEC_REAL OS_SPTR *)pSP;
			IEC_INT u = (IEC_INT)(*(IEC_REAL OS_SPTR *)pSP);

		  #if defined(IP_CFG_STACK8) || defined(IP_CFG_STACK16)
			pSP += CNV_NEW(4,2);
		  #endif

			*(IEC_INT OS_SPTR *)pSP = (IEC_INT)utilRealToDint(aReal, u, IEC_MIN_INT, IEC_MAX_INT);
		}
		break;
#endif
	  
#if defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) /* ------------------------- */
	case ASM_REAL_TO_UDINT:
 /* case ASM_REAL_TO_DWORD: */
		STK_UCHK4(1);
		{
			IEC_REAL aReal = *(IEC_REAL OS_SPTR *)pSP;
			IEC_UDINT u = (IEC_UDINT)(*(IEC_REAL OS_SPTR *)pSP);
			*(IEC_UDINT OS_SPTR *)pSP = utilRealToDint(aReal, u, 0, 0xffffffffu);
		}
		break;
#endif
	  
#if defined(IP_CFG_DINT) /* --------------------------------------------------- */ 
	case ASM_REAL_TO_DINT :   
		STK_UCHK4(1);
		{
			IEC_REAL aReal = *(IEC_REAL OS_SPTR *)pSP;
			IEC_DINT u = (IEC_DINT)(*(IEC_REAL OS_SPTR *)pSP);
			*(IEC_DINT OS_SPTR *)pSP = utilRealToDint(aReal, u, IEC_MIN_DINT, IEC_MAX_DINT);
		}
		break;
#endif
			
#if defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD) /* ------------------------- */
	case ASM_REAL_TO_ULINT:   
 /* case ASM_REAL_TO_LWORD: */
		STK_UCHK4(1);
		STACK_OCHK(CNV_NEW(8,4));
		{
			IEC_ULINT u;
			u.H = 0;
			u.L = (IEC_DWORD)(*(IEC_REAL OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,4);
			*(IEC_ULINT OS_SPTR *)pSP = u;
		}
		break;
#endif
	  
#if defined(IP_CFG_LINT) /* --------------------------------------------------- */
	case ASM_REAL_TO_LINT :   
		STK_UCHK4(1);
		STACK_OCHK(CNV_NEW(8,4));
		{
			IEC_LINT u;
			u.H = (*(IEC_REAL OS_SPTR *)pSP < (IEC_REAL)0.0) ? 0xFFFFFFFF : 0;
			u.L = (IEC_DWORD)(*(IEC_REAL OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,4);
			*(IEC_LINT OS_SPTR *)pSP = u;
		}
		break;
#endif
	   
#if defined(IP_CFG_LREAL) /* -------------------------------------------------- */
	case ASM_REAL_TO_LREAL:   
		STK_UCHK4(1);
		STACK_OCHK(CNV_NEW(8,4));
		{
			IEC_LREAL u = (IEC_LREAL)(*(IEC_REAL OS_SPTR *)pSP);
			pSP -= CNV_NEW(8,4);
			*(IEC_LREAL OS_SPTR *)pSP = u;
		}
		break;
#endif

#endif /* IP_CFG_REAL */
			
/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD)
 
#if defined(IP_CFG_BOOL) /* --------------------------------------------------- */ 
	case ASM_ULINT_TO_BOOL :
 /* case ASM_LWORD_TO_BOOL : */
		STK_UCHK8(1);
		{
			IEC_BOOL u = (IEC_BOOL)(((((*(IEC_ULINT OS_SPTR *)pSP).H)==0) &&
				(((*(IEC_ULINT OS_SPTR *)pSP).L)==0)) ? 0 : 1);
			pSP += CNV_NEW(8,1);
			*(IEC_BOOL OS_SPTR *)pSP = u;
		}
		break;
#endif
	  
#if defined(IP_CFG_USINT) || defined(IP_CFG_BYTE) || defined(IP_CFG_SINT) /* -- */
	case ASM_ULINT_TO_USINT:
 /* case ASM_ULINT_TO_BYTE : */
 /* case ASM_LWORD_TO_USINT: */
 /* case ASM_LWORD_TO_BYTE : */
	case ASM_ULINT_TO_SINT :
 /* case ASM_LWORD_TO_SINT : */
		STK_UCHK8(1);
		{
			IEC_USINT u = (IEC_USINT)(*(IEC_ULINT OS_SPTR *)pSP).L;
			pSP += CNV_NEW(8,1);
			*(IEC_USINT OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_UINT) || defined(IP_CFG_WORD) || defined(IP_CFG_INT) /* ---- */
	case ASM_ULINT_TO_UINT :
 /* case ASM_ULINT_TO_WORD : */
 /* case ASM_LWORD_TO_UINT : */
 /* case ASM_LWORD_TO_WORD : */  
	case ASM_ULINT_TO_INT  :   
 /* case ASM_LWORD_TO_INT  : */
		STK_UCHK8(1);
		{
			IEC_UINT u = (IEC_UINT)(*(IEC_ULINT OS_SPTR *)pSP).L;
			pSP += CNV_NEW(8,2);
			*(IEC_UINT OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) || defined(IP_CFG_DINT) /* - */
	case ASM_ULINT_TO_UDINT:
 /* case ASM_ULINT_TO_DWORD: */
 /* case ASM_LWORD_TO_UDINT: */
 /* case ASM_LWORD_TO_DWORD: */
	case ASM_ULINT_TO_DINT :   
 /* case ASM_LWORD_TO_DINT : */
		STK_UCHK8(1);
		{
			IEC_UDINT u = (IEC_UDINT)(*(IEC_ULINT OS_SPTR *)pSP).L;
			pSP += CNV_NEW(8,4);
			*(IEC_UDINT OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_REAL) /* --------------------------------------------------- */ 
	case ASM_ULINT_TO_REAL :
 /* case ASM_LWORD_TO_REAL : */
		STK_UCHK8(1);
		{
			IEC_REAL u = (IEC_REAL)(*(IEC_ULINT OS_SPTR *)pSP).H * (IEC_REAL)4294967296.0 + (IEC_REAL)(*(IEC_ULINT OS_SPTR *)pSP).L;
			pSP += CNV_NEW(8,4);
			*(IEC_REAL OS_SPTR *)pSP = u;
		}
		break;
#endif
			
	case ASM_ULINT_TO_LWORD: /* ----------------------------------------------- */
	case ASM_LWORD_TO_ULINT:
	case ASM_ULINT_TO_LINT :   
 /* case ASM_LWORD_TO_LINT : */
		STK_UCHK8(1);
		break;
			
#if defined(IP_CFG_LREAL) /* -------------------------------------------------- */
	case ASM_ULINT_TO_LREAL:
 /* case ASM_LWORD_TO_LREAL: */
		STK_UCHK8(1);
		*(IEC_LREAL OS_SPTR *)pSP = (IEC_LREAL)(*(IEC_ULINT OS_SPTR *)pSP).H * (IEC_LREAL)4294967296.0 + (IEC_LREAL)(*(IEC_ULINT OS_SPTR *)pSP).L;
		break;
#endif

#endif /* IP_CFG_ULINT */

/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_LINT)

#if defined(IP_CFG_BOOL) /* --------------------------------------------------- */ 
	case ASM_LINT_TO_BOOL :
		STK_UCHK8(1);
		{
			IEC_BOOL u = (IEC_BOOL)((((*(IEC_LINT OS_SPTR *)pSP).L==0) && ((*(IEC_LINT OS_SPTR *)pSP).H==0)) ? 0 : 1);
			pSP += CNV_NEW(8,1);
			*(IEC_BOOL OS_SPTR *)pSP = u;
		}
		break;
#endif
	  
 #if defined(IP_CFG_USINT) || defined(IP_CFG_BYTE) || defined(IP_CFG_SINT) /* - */
	case ASM_LINT_TO_USINT:
 /* case ASM_LINT_TO_BYTE : */
	case ASM_LINT_TO_SINT :
		STK_UCHK8(1);
		{
			IEC_USINT u = (IEC_USINT)(*(IEC_LINT OS_SPTR *)pSP).L;
			pSP += CNV_NEW(8,1);
			*(IEC_USINT OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_UINT) || defined(IP_CFG_WORD) || defined(IP_CFG_INT) /* ---- */
	case ASM_LINT_TO_UINT :
 /* case ASM_LINT_TO_WORD : */
	case ASM_LINT_TO_INT  :
		STK_UCHK8(1);
		{
			IEC_UINT u = (IEC_UINT)(*(IEC_LINT OS_SPTR *)pSP).L;
			pSP += CNV_NEW(8,2);
			*(IEC_UINT OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) || defined(IP_CFG_DINT) /* - */
	case ASM_LINT_TO_UDINT:   
 /* case ASM_LINT_TO_DWORD: */
	case ASM_LINT_TO_DINT :
		STK_UCHK8(1);
		{
			IEC_UDINT u = (IEC_UDINT)(*(IEC_LINT OS_SPTR *)pSP).L;
			pSP += CNV_NEW(8,4);
			*(IEC_UDINT OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_REAL) /* --------------------------------------------------- */ 
	case ASM_LINT_TO_REAL :   
		STK_UCHK8(1);
		{
			IEC_REAL u = (IEC_REAL)(*(IEC_LINT OS_SPTR *)pSP).H * (IEC_REAL)4294967296.0 + (IEC_REAL)(*(IEC_ULINT OS_SPTR *)pSP).L;
			pSP += CNV_NEW(8,4);
			*(IEC_REAL OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD) /* ------------------------- */
	case ASM_LINT_TO_ULINT:
 /* case ASM_LINT_TO_LWORD: */
		STK_UCHK8(1);
		break;
#endif
			
#if defined(IP_CFG_LREAL) /* -------------------------------------------------- */
	case ASM_LINT_TO_LREAL: 
		STK_UCHK8(1);
		*(IEC_LREAL OS_SPTR *)pSP = (IEC_LREAL)(*(IEC_LINT OS_SPTR *)pSP).H * (IEC_LREAL)4294967296.0 + (IEC_LREAL)(*(IEC_ULINT OS_SPTR *)pSP).L;
		break;
#endif

#endif /* IP_CFG_LINT */
	  
/* ---------------------------------------------------------------------------- */

#if defined(IP_CFG_LREAL)

#if defined(IP_CFG_BOOL) /* --------------------------------------------------- */ 
	case ASM_LREAL_TO_BOOL :
		STK_UCHK8(1);
		{
			IEC_BOOL u = (IEC_BOOL)((*(IEC_LREAL OS_SPTR *)pSP)==0?0:1);
			pSP += CNV_NEW(8,1);
			*(IEC_BOOL OS_SPTR *)pSP = u;
		}
		break;
#endif
	  
#if defined(IP_CFG_USINT) || defined(IP_CFG_BYTE) /* -------------------------- */
	case ASM_LREAL_TO_USINT:
 /* case ASM_LREAL_TO_BYTE : */
		STK_UCHK8(1);
		{
			IEC_LREAL aLreal = *(IEC_LREAL OS_SPTR *)pSP;
			IEC_USINT u = (IEC_USINT)(*(IEC_LREAL OS_SPTR *)pSP);
			pSP += CNV_NEW(8,1);
			*(IEC_USINT OS_SPTR *)pSP = (IEC_BYTE)utilLrealToDint(aLreal, u, 0, 0xffu);
		}
		break;
#endif
	  
#if defined(IP_CFG_SINT) /* --------------------------------------------------- */ 
	case ASM_LREAL_TO_SINT :
		STK_UCHK8(1);
		{
			IEC_LREAL aLreal = *(IEC_LREAL OS_SPTR *)pSP;
			IEC_SINT u = (IEC_SINT)(*(IEC_LREAL OS_SPTR *)pSP);
			pSP += CNV_NEW(8,1);
			*(IEC_SINT OS_SPTR *)pSP = (IEC_SINT)utilLrealToDint(aLreal, u, IEC_MIN_SINT, IEC_MAX_SINT);
		}
		break;
#endif
	  
#if defined(IP_CFG_UINT) || defined(IP_CFG_WORD) /* --------------------------- */
	case ASM_LREAL_TO_UINT :
 /* case ASM_LREAL_TO_WORD : */
		STK_UCHK8(1);
		{
			IEC_LREAL aLreal = *(IEC_LREAL OS_SPTR *)pSP;	
			IEC_UINT u = (IEC_UINT)(*(IEC_LREAL OS_SPTR *)pSP);
			pSP += CNV_NEW(8,2);
			*(IEC_UINT OS_SPTR *)pSP = (IEC_UINT)utilLrealToDint(aLreal, u, 0, 0xffffu);
		}
		break;
#endif
	  
#if defined(IP_CFG_INT) /* ---------------------------------------------------- */ 
	case ASM_LREAL_TO_INT  :
		STK_UCHK8(1);
		{
			IEC_LREAL aLreal = *(IEC_LREAL OS_SPTR *)pSP;
			IEC_INT u = (IEC_INT)(*(IEC_LREAL OS_SPTR *)pSP);
			pSP += CNV_NEW(8,2);
			*(IEC_INT OS_SPTR *)pSP = (IEC_INT)utilLrealToDint(aLreal, u, IEC_MIN_INT, IEC_MAX_INT);
		}
		break;
#endif
	  
#if defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) /* ------------------------- */
	case ASM_LREAL_TO_UDINT:   
 /* case ASM_LREAL_TO_DWORD: */
		STK_UCHK8(1);
		{
			IEC_LREAL aLreal = *(IEC_LREAL OS_SPTR *)pSP;
			IEC_UDINT u = (IEC_UDINT)(*(IEC_LREAL OS_SPTR *)pSP);
			pSP += CNV_NEW(8,4);
			*(IEC_UDINT OS_SPTR *)pSP = utilLrealToDint(aLreal, u, 0, 0xffffffffu);
		}
		break;
#endif
	  
#if defined(IP_CFG_DINT) /* --------------------------------------------------- */ 
	case ASM_LREAL_TO_DINT :
		STK_UCHK8(1);
		{
			IEC_LREAL aLreal = *(IEC_LREAL OS_SPTR *)pSP;
			IEC_DINT u = (IEC_DINT)(*(IEC_LREAL OS_SPTR *)pSP);
			pSP += CNV_NEW(8,4);
			*(IEC_DINT OS_SPTR *)pSP = utilLrealToDint(aLreal, u, IEC_MIN_DINT, IEC_MAX_DINT);
		}
		break;
#endif
	  
#if defined(IP_CFG_REAL) /* --------------------------------------------------- */ 
	case ASM_LREAL_TO_REAL :   
		STK_UCHK8(1);
		{
			IEC_REAL u = (IEC_REAL)(*(IEC_LREAL OS_SPTR *)pSP);
			pSP += CNV_NEW(8,4);
			*(IEC_REAL OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#if defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD) || defined(IP_CFG_LINT)
	case ASM_LREAL_TO_ULINT:
 /* case ASM_LREAL_TO_LWORD: */
   case ASM_LREAL_TO_LINT :
		STK_UCHK8(1);
		{
			IEC_ULINT u; 
			u.H = (IEC_DWORD)(*(IEC_LREAL OS_SPTR *)pSP / (IEC_LREAL)4294967296.0);
			u.L = (IEC_DWORD)(*(IEC_LREAL OS_SPTR *)pSP - (IEC_LREAL)4294967296.0 * u.H);
			*(IEC_ULINT OS_SPTR *)pSP = u;
		}
		break;
#endif
			
#endif /* IP_CFG_LREAL */	   

/* ---------------------------------------------------------------------------- */
