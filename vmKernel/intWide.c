
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
 * Filename: intWide.c
 */


/* Value declarations
 * ----------------------------------------------------------------------------
 */
#define VAL_IP1(vv) 	IEC_xx vv; \
						IP_xx(vv);

#define VAL_IP2(vv,ww)	IEC_xx vv; \
						IEC_xx ww; \
						IP_xx(vv);	\
						IP_xx(ww);

#define VAL_ARR(vv,ww)	IEC_xx vv; \
						IEC_xx ww; \
						IP_xx(ww); \
						POPxxL(vv);


/* Data type depending opcode selection
 * ----------------------------------------------------------------------------
 */
#if defined(IP_CFG_BOOL)
  #define IP_ENA_DT1
#endif

#if defined(IP_CFG_SINT) || defined(IP_CFG_USINT) || defined(IP_CFG_BYTE)
  #define IP_ENA_DT8
#endif

#if defined(IP_CFG_INT) || defined(IP_CFG_UINT) || defined(IP_CFG_WORD)
  #define IP_ENA_DT16
#endif

#if defined(IP_CFG_DINT) || defined(IP_CFG_UDINT) || defined(IP_CFG_DWORD) || defined(IP_CFG_REAL) || defined(IP_CFG_TIME)
  #define IP_ENA_DT32
#endif

#if defined(IP_CFG_LINT) || defined(IP_CFG_ULINT) || defined(IP_CFG_LWORD) || defined(IP_CFG_LREAL)
  #define IP_ENA_DT64
#endif


/* ----------------------------------------------------------------------------
 * PSHC_VV / PSHC_00 / PSHC_01
 * ----------------------------------------------------------------------------
 * * Push a constant value on the interpreter stack.
 *
 * * PSHC_VV_{w}		*SP = c
 * * PSHC_01_{w}		*SP = 1
 * * PSHC_00_{w}		*SP = 0
 * ----------------------------------------------------------------------------
 */
#if defined(IP_ENA_PSHCxx)

	case ASM_PSHC_VV_xx: /* --------------------------------------------------- */
	{
		VAL_IP1(c);
		PUSHxxL(c);
	}
	break;

	case ASM_PSHC_00_xx: /* --------------------------------------------------- */		
	{
		PUSHxxL(0);
	}
	break;	

	case ASM_PSHC_01_xx: /* --------------------------------------------------- */
	{
		PUSHxxL(1);
	}
	break;

#endif	/* IP_ENA_PSHCxx */


/* ----------------------------------------------------------------------------
 * CLST
 * ----------------------------------------------------------------------------
 * * The opcode CLST releases the stack memory on the end of a function call.
 *
 * * CLST				SP += c 
 * ----------------------------------------------------------------------------
 */
#if defined(IP_ENA_OBJxx)

	case ASM_CLST:	/* -------------------------------------------------------- */
	{
		VAL_IP1(b);
		
		STACK_UCHK(b);
		pSP += b;
	}
	break;

#endif /* IP_ENA_OBJxx */


/* ----------------------------------------------------------------------------
 * JUMP, JMPC, JMPN
 * ----------------------------------------------------------------------------
 * * The opcodes JUMP, JMPC and JMPN perform absolute respectively relative 
 *	 jumps within the interpreter code.
 *
 * * JUMP				IP += o
 * * JMPC				IP += o if *SP != 0
 * * JMPN				IP += o if *SP == 0
 * ----------------------------------------------------------------------------
 */
/* #if defined(IP_ENA_OBJxx) JUMP opcodes are always 32 bit wide */

	case ASM_JUMP: /* --------------------------------------------------------- */
	{
		VAL_IP1(o)
		
		pIP += (IEC_Sxx)o - JUMPxxOFF;

		if ((IEC_Sxx)o < 0)
		{
			OS_EXECUTION_TIME_CHECK

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
		}
	}
	break;

	case ASM_JMPC: /* --------------------------------------------------------- */
	{
		IEC_BYTE b;
		VAL_IP1(o);

		POP8L(b);

		if (b != 0)
		{
			pIP += (IEC_Sxx)o - JUMPxxOFF;

			if ((IEC_Sxx)o < 0)
			{
				OS_EXECUTION_TIME_CHECK

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
			}
		}
	}
	break;
		
	case ASM_JMPN: /* --------------------------------------------------------- */
	{
		IEC_BYTE b;
		VAL_IP1(o);

		POP8L(b);

		if (b == 0)
		{
			pIP += (IEC_Sxx)o - JUMPxxOFF;

			if ((IEC_Sxx)o < 0)
			{
				OS_EXECUTION_TIME_CHECK

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
			}
		}
	}
	break;

/* #endif */ /* IP_ENA_OBJxx */


/* ----------------------------------------------------------------------------
 * PSHD_Gx / POPD_Gx / LEAD_Gx
 * ----------------------------------------------------------------------------
 * The opcodes PSHD_Gx and POPD_Gx are used to directly access simple % addressed 
 * global variables and to access member variables of % addressed structures 
 * and arrays.
 * 
 * The opcodes LEAD_Gx are used to load the address of simple % addressed global 
 * variables and of % addressed global structures and arrays.
 *
 * Structure member variables and array elements of % addressed structures and 
 * arrays are not referenced by their ID, but the compiler computes the direct 
 * absolute offsets to the global memory segments directly.
 *
 * X is a placeholder for the three different global memory segments:
 *
 * * Access to % addressed simple global variables.
 * * Access to member variables % addressed global structures and 
 *	 function blocks.
 * * Access to array elements of % addressed global arrays.
 *
 * POPD_GO / LEAD_GO replaced by POPD_GX / LEAD_GX starting from V2.06.
 *
 * * PSHD_Gx_{w}	*SP 	 = *(X + o)
 * * POPD_Gx_{w}	*(X + o) = *SP
 * * LEAD_Gx{_1}	*SP 	 =	X + o
 * ----------------------------------------------------------------------------
 */
#if (defined(IP_ENA_GLOBxx) || defined(IP_ENA_GLOB_BITxx)) && defined(IP_ENA_DT1)

	case ASM_PSHD_GI_1 : pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_PUSHxxxB);
	case ASM_PSHD_GO_1 : pSeg = pLocal->pSeg[SEG_OUTPUT].pAdr; goto LABxx(m_PUSHxxxB);
	case ASM_PSHD_GM_1 : pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_PUSHxxxB); 
	LABxx(m_PUSHxxxB):
	{
		VAL_IP1(o);
		PUSH1(ADDxxPTR(pSeg, o>>3), o);
	}
	break;
		
	case ASM_POPD_GI_1 : pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_POPxxxB);
	case ASM_POPD_GM_1 : pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_POPxxxB); 
	LABxx(m_POPxxxB):
	{		  
		VAL_IP1(o);
		POP1(ADDxxPTR(pSeg, o>>3), o);
	}
	break;

	case ASM_POPD_GX_1 : /* --------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POP1(ADDxxPTR(pLocal->pSeg[SEG_OUTPUT].pAdr, o/8), o);

	  #if defined(RTS_CFG_WRITE_FLAGS)
		WFLG1(ADDxxPTR(pLocal->WriteFlags.pAdr, o/8), o);
	  #endif
	}
	break;

	case ASM_LEAD_GM_1: pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_LEAxx1);
	case ASM_LEAD_GI_1: pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_LEAxx1);
	LABxx(m_LEAxx1):
	{
		VAL_IP1(o);
		PUSHBITPL(ADDxxPTR(pSeg, o>>3), o);
	}
	break;
					 
	case ASM_LEAD_GX_1: /* ---------------------------------------------------- */
	{
		VAL_IP1(o);
		PUSHBITPL(ADDxxPTR(pLocal->pSeg[SEG_OUTPUT].pAdr, o/8), o);

	  #if defined(RTS_CFG_WRITE_FLAGS)
		WFLG1(ADDxxPTR(pLocal->WriteFlags.pAdr, o/8), o)
	  #endif
	}
	break;

#endif

#if defined(IP_ENA_GLOBxx) && defined(IP_ENA_DT8)

	case ASM_PSHD_GI_8 : pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_PUSHxxx8);
	case ASM_PSHD_GO_8 : pSeg = pLocal->pSeg[SEG_OUTPUT].pAdr; goto LABxx(m_PUSHxxx8);
	case ASM_PSHD_GM_8 : pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_PUSHxxx8); 
	LABxx(m_PUSHxxx8):
	{
		VAL_IP1(o);
		PUSH8(ADDxxPTR(pSeg, o));
	}
	break;
		
	case ASM_POPD_GI_8 : pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_POPxxx8);
	case ASM_POPD_GM_8 : pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_POPxxx8);
	LABxx(m_POPxxx8):
	{
		VAL_IP1(o);
		POP8(ADDxxPTR(pSeg, o));
	}
	break;
		
	case ASM_POPD_GX_8 :	/* ---------------------------------------------------- */
	{
		VAL_IP1(o);
		POP8(ADDxxPTR(pLocal->pSeg[SEG_OUTPUT].pAdr, o));

	  #if defined(RTS_CFG_WRITE_FLAGS)
		WFLG8(ADDxxPTR(pLocal->WriteFlags.pAdr, o));
	  #endif
	}
	break;

#endif

#if defined(IP_ENA_GLOBxx) && defined(IP_ENA_DT16)

	case ASM_PSHD_GI_16 : pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_PUSHxxx16);
	case ASM_PSHD_GO_16 : pSeg = pLocal->pSeg[SEG_OUTPUT].pAdr; goto LABxx(m_PUSHxxx16);
	case ASM_PSHD_GM_16 : pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_PUSHxxx16); 
	LABxx(m_PUSHxxx16):
	{
		VAL_IP1(o);
		PUSH16(ADDxxPTR(pSeg, o));
	}
	break;
		
	case ASM_POPD_GI_16 : pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_POPxxx16);
	case ASM_POPD_GM_16 : pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_POPxxx16);
	LABxx(m_POPxxx16):
	{
		VAL_IP1(o);
		POP16(ADDxxPTR(pSeg, o));
	}
	break;
		
	case ASM_POPD_GX_16 : /* -------------------------------------------------- */
	{
		VAL_IP1(o);
		POP16(ADDxxPTR(pLocal->pSeg[SEG_OUTPUT].pAdr, o));

	  #if defined(RTS_CFG_WRITE_FLAGS)
		WFLG16(ADDxxPTR(pLocal->WriteFlags.pAdr, o));
	  #endif
	}
	break;

#endif

#if defined(IP_ENA_GLOBxx) && defined(IP_ENA_DT32)

	case ASM_PSHD_GI_32 : pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_PUSHxxx32);
	case ASM_PSHD_GO_32 : pSeg = pLocal->pSeg[SEG_OUTPUT].pAdr; goto LABxx(m_PUSHxxx32);
	case ASM_PSHD_GM_32 : pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_PUSHxxx32); 
	LABxx(m_PUSHxxx32):
	{
		VAL_IP1(o);
		PUSH32(ADDxxPTR(pSeg, o));
	}
	break;
		
	case ASM_POPD_GI_32 : pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_POPxxx32);
	case ASM_POPD_GM_32 : pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_POPxxx32);
	LABxx(m_POPxxx32):
	{
		VAL_IP1(o);
		POP32(ADDxxPTR(pSeg, o));
	}
	break;
		
	case ASM_POPD_GX_32: /* --------------------------------------------------- */
	{
		VAL_IP1(o);
		POP32(ADDxxPTR(pLocal->pSeg[SEG_OUTPUT].pAdr, o));

	  #if defined(RTS_CFG_WRITE_FLAGS)
		WFLG32(ADDxxPTR(pLocal->WriteFlags.pAdr, o));
	  #endif
	}
	break;

#endif

#if defined(IP_ENA_GLOBxx) && defined(IP_ENA_DT64)

	case ASM_PSHD_GI_64 : pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_PUSHxxx64);
	case ASM_PSHD_GO_64 : pSeg = pLocal->pSeg[SEG_OUTPUT].pAdr; goto LABxx(m_PUSHxxx64);
	case ASM_PSHD_GM_64 : pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_PUSHxxx64); 
	LABxx(m_PUSHxxx64):
	{
		VAL_IP1(o);
		PUSH64(ADDxxPTR(pSeg, o));
	}
	break;
		
	case ASM_POPD_GI_64 : pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_POPxxx64);
	case ASM_POPD_GM_64 : pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_POPxxx64);
	LABxx(m_POPxxx64):
	{
		VAL_IP1(o);
		POP64(ADDxxPTR(pSeg, o));
	}
	break;
		
	case ASM_POPD_GX_64: /* --------------------------------------------------- */
	{
		VAL_IP1(o);
		POP64(ADDxxPTR(pLocal->pSeg[SEG_OUTPUT].pAdr, o));

	  #if defined(RTS_CFG_WRITE_FLAGS)
		WFLG64(ADDxxPTR(pLocal->WriteFlags.pAdr, o));
	  #endif
	}
	break;

#endif

#if defined(IP_ENA_GLOBxx)

	case ASM_LEAD_GM: pSeg = pLocal->pSeg[SEG_GLOBAL].pAdr; goto LABxx(m_LEAxx);
	case ASM_LEAD_GI: pSeg = pLocal->pSeg[SEG_INPUT ].pAdr; goto LABxx(m_LEAxx);
	LABxx(m_LEAxx):
	{
		VAL_IP1(o);
		PUSHPL(ADDxxPTR(pSeg, o));
	}
	break;
					 
	case ASM_LEAD_GX:	/* ---------------------------------------------------- */	
	{
		VAL_IP2(o,s);
		PUSHPL(ADDxxPTR(pLocal->pSeg[SEG_OUTPUT].pAdr, o));

	  #if defined(RTS_CFG_WRITE_FLAGS)
		WFLGnn(ADDxxPTR(pLocal->WriteFlags.pAdr, o), s)
	  #endif
	}
	break;

#endif


/* ----------------------------------------------------------------------------
 * PSHD_IN / POPD_IN / LEAD_IN
 * ----------------------------------------------------------------------------
 * The opcodes PSHD_IN and POPD_IN are used to directly access simple values 
 * within the instance data object of the currently executed POU. 
 *
 * The opcode LEAD_IN is used to load the address of simple variables within 
 * the instance data object of the currently executed POU - for example when 
 * passing a simple variable as VAR_IN_OUT to functions.
 *
 * * Access to local variables of programs, functions and function blocks. 
 *	 (= currently executed POU).
 *
 * * PSHD_IN_{w}		*SP 	  = *(IN + o)
 * * POPD_IN_{w}		*(IN + o) = *SP
 * * LEAD_IN{_1}		*SP 	  = IN + o
 * ----------------------------------------------------------------------------
 */
#if (defined(IP_ENA_OBJxx) || defined(IP_ENA_OBJ_BITxx)) && defined(IP_ENA_DT1)

	case ASM_PSHD_IN_1: /* ---------------------------------------------------- */
	{
		VAL_IP1(o);
		PUSH1(ADDxxPTR(pIN, o/8), o);
	}
	break;
		
	case ASM_POPD_IN_1: /* ---------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POP1(ADDxxPTR(pIN, o/8), o);
	}
	break;

	case ASM_LEAD_IN_1: /* ---------------------------------------------------- */
	{
		VAL_IP1(o);
		PUSHBITPL(ADDxxPTR(pIN, o/8), o);
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT8)

	case ASM_PSHD_IN_8: /* ---------------------------------------------------- */
	{
		VAL_IP1(o);
		PUSH8(ADDxxPTR(pIN, o));
	}
	break;
		
	case ASM_POPD_IN_8: /* ---------------------------------------------------- */
	{
		VAL_IP1(o);
		POP8(ADDxxPTR(pIN, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT16)

	case ASM_PSHD_IN_16: /* --------------------------------------------------- */
	{
		VAL_IP1(o);
		PUSH16(ADDxxPTR(pIN, o));
	}
	break;
		
	case ASM_POPD_IN_16: /* --------------------------------------------------- */
	{
		VAL_IP1(o);
		POP16(ADDxxPTR(pIN, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT32)

	case ASM_PSHD_IN_32: /* --------------------------------------------------- */
	{
		VAL_IP1(o);
		PUSH32(ADDxxPTR(pIN, o));
	}
	break;
		
	case ASM_POPD_IN_32: /* --------------------------------------------------- */
	{
		VAL_IP1(o);
		POP32(ADDxxPTR(pIN, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT64)

	case ASM_PSHD_IN_64: /* --------------------------------------------------- */
	{
		VAL_IP1(o);
		PUSH64(ADDxxPTR(pIN, o));
	}
	break;
		
	case ASM_POPD_IN_64: /* --------------------------------------------------- */
	{
		VAL_IP1(o);
		POP64(ADDxxPTR(pIN, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx)

	case ASM_LEAD_IN: /* ------------------------------------------------------ */
	{
		VAL_IP1(o);
		PUSHPL(ADDxxPTR(pIN, o));
	}
	break;

#endif


/* ----------------------------------------------------------------------------
 * PSHO_ID / POPO_ID / LEAO_ID / LEAO_XD
 * ----------------------------------------------------------------------------
 * The opcode groups PSHO_ID, POPO_ID and LEAO_ID are used to access simple 
 * member variables of global complex objects (function blocks and structures) 
 * and because of automatic global and global retentive variables are grouped 
 * together in two special data objects also to access simple automatic global 
 * and simple global retentive variables.
 * 
 * The opcode LEAO_XD is used to load the address of automatic global data 
 * objects.
 *
 * * Access to simple member variables of global structures and function 
 *	 blocks (_ID).
 * * Access to simple automatic global variables (_ID).
 * * Access to simple retentive global variables (_ID).
 * * LEA of global structures and function blocks (_XD).
 * 
 * * PSHO_ID_{w}		*SP 		= *([id] + o)
 * * POPO_ID_{w}		*([id] + o) = *SP
 * * LEAO_ID{_1}		*SP 		= [id] + o
 * * LEAO_XD			*SP 		= [id]
 * ----------------------------------------------------------------------------
 */
#if (defined(IP_ENA_OBJxx) || defined(IP_ENA_OBJ_BITxx)) && defined(IP_ENA_DT1)

	case ASM_PSHO_ID_1: /* ---------------------------------------------------- */
	{
		VAL_IP2(id,o);
		PUSH1(ADDxxPTR(pShared->pData[id].pAdr, o/8), o);
	}
	break;
				
	case ASM_POPO_ID_1: /* ---------------------------------------------------- */
	{
		VAL_IP2(id,o);
		POP1(ADDxxPTR(pShared->pData[id].pAdr, o/8), o);
	}
	break;

	case ASM_LEAO_ID_1: /* ---------------------------------------------------- */
	{
		VAL_IP2(id,o);		
		PUSHBITPL(ADDxxPTR(pShared->pData[id].pAdr, o/8), o);
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT8)

	case ASM_PSHO_ID_8: /* ---------------------------------------------------- */
	{
		VAL_IP2(id,o);
		PUSH8(ADDxxPTR(pShared->pData[id].pAdr, o)); 
	}
	break;
				
	case ASM_POPO_ID_8: /* ---------------------------------------------------- */
	{
		VAL_IP2(id,o);
		POP8(ADDxxPTR(pShared->pData[id].pAdr, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT16)

	case ASM_PSHO_ID_16: /* --------------------------------------------------- */
	{
		VAL_IP2(id,o);
		PUSH16(ADDxxPTR(pShared->pData[id].pAdr, o)); 
	}
	break;
				
	case ASM_POPO_ID_16: /* --------------------------------------------------- */
	{
		VAL_IP2(id,o);
		POP16(ADDxxPTR(pShared->pData[id].pAdr, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT32)

	case ASM_PSHO_ID_32: /* --------------------------------------------------- */
	{
		VAL_IP2(id,o);
		PUSH32(ADDxxPTR(pShared->pData[id].pAdr, o)); 
	}
	break;
				
	case ASM_POPO_ID_32: /* --------------------------------------------------- */
	{
		VAL_IP2(id,o);
		POP32(ADDxxPTR(pShared->pData[id].pAdr, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT64)

	case ASM_PSHO_ID_64: /* --------------------------------------------------- */
	{
		VAL_IP2(id,o);
		PUSH64(ADDxxPTR(pShared->pData[id].pAdr, o)); 
	}
	break;
				
	case ASM_POPO_ID_64: /* --------------------------------------------------- */
	{
		VAL_IP2(id,o);
		POP64(ADDxxPTR(pShared->pData[id].pAdr, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx)

	case ASM_LEAO_XD: /* ------------------------------------------------------ */
	{
		VAL_IP1(id);
		PUSHPL(pShared->pData[id].pAdr);
	}
	break;

	case ASM_LEAO_ID: /* ------------------------------------------------------ */
	{
		VAL_IP2(id,o);
		PUSHPL(ADDxxPTR(pShared->pData[id].pAdr, o));
	}
	break;

#endif
						 

/* ----------------------------------------------------------------------------
 * PSHO_IN / POPO_IN / LEAO_IN / LEAO_XN
 * ----------------------------------------------------------------------------
 * The opcode groups PSHO_IN, POPO_IN and LEAO_ID are used to access simple 
 * member variables of local complex objects (function blocks and structures).
 *
 * The opcode LEAO_XN is used to load the address of automatic global data 
 * objects.
 *
 * * Access to simple member variables of local structures and function 
 *	 blocks (_IN).
 * * LEA of local structures and function blocks (_XN).
 * 
 * * PSHO_IN_{w}		*SP 			   = *([*(IN + i)] + o)
 * * POPO_IN_{w}		*([*(IN + i)] + o) = *SP
 * * LEAO_IN{_1}		*SP 			   = [*(IN + i)] + o
 * * LEAO_XN			*SP 			   = [*(IN + i)]
 * ----------------------------------------------------------------------------
 */
#if (defined(IP_ENA_OBJxx) || defined(IP_ENA_OBJ_BITxx)) && defined(IP_ENA_DT1)

	case ASM_PSHO_IN_1: /* ---------------------------------------------------- */
	{
		VAL_IP2(i,o);
		PUSH1(ADDxxPTR(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr, o/8), o);
	}
	break;
				
	case ASM_POPO_IN_1: /* ---------------------------------------------------- */
	{
		VAL_IP2(i,o);
		POP1(ADDxxPTR(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr, o/8), o);
	}
	break;

	case ASM_LEAO_IN_1: /* ---------------------------------------------------- */
	{
		VAL_IP2(i,o);
		PUSHBITPL(ADDxxPTR(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr, o/8), o);
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT8)

	case ASM_PSHO_IN_8: /* ---------------------------------------------------- */
	{
		VAL_IP2(i,o);
		PUSH8(ADDxxPTR(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr, o)); 
	}
	break;
				
	case ASM_POPO_IN_8: /* ---------------------------------------------------- */
	{
		VAL_IP2(i,o);	
		POP8(ADDxxPTR(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT16)

	case ASM_PSHO_IN_16: /* --------------------------------------------------- */
	{
		VAL_IP2(i,o);
		PUSH16(ADDxxPTR(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr, o)); 
	}
	break;
				
	case ASM_POPO_IN_16: /* --------------------------------------------------- */
	{
		VAL_IP2(i,o);	
		POP16(ADDxxPTR(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT32)

	case ASM_PSHO_IN_32: /* -------------------------------------------------- */
	{
		VAL_IP2(i,o);
		PUSH32(ADDxxPTR(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr, o)); 
	}
	break;
				
	case ASM_POPO_IN_32: /* --------------------------------------------------- */
	{
		VAL_IP2(i,o);	
		POP32(ADDxxPTR(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT64)

	case ASM_PSHO_IN_64: /* --------------------------------------------------- */
	{
		VAL_IP2(i,o);
		PUSH64(ADDxxPTR(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr, o)); 
	}
	break;
				
	case ASM_POPO_IN_64: /* --------------------------------------------------- */
	{
		VAL_IP2(i,o);
		POP64(ADDxxPTR(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx)

	case ASM_LEAO_XN: /* ------------------------------------------------------ */
	{
		VAL_IP1(i);
		PUSHPL(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr);
	}
	break;

	case ASM_LEAO_IN: /* ------------------------------------------------------ */
	{
		VAL_IP2(i,o);
		PUSHPL(ADDxxPTR(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr, o));
	}
	break;						 

#endif
				

/* ----------------------------------------------------------------------------
 * PSHO_ST / POPO_ST / LEAO_ST
 * ----------------------------------------------------------------------------
 * * Access to simple member variables of structures and function blocks passed 
 *	 as VAR_IN_OUT to functions or function blocks.
 * * LEA of simple member variables of structures and function blocks passed as 
 *	 VAR_IN_OUT to functions or function blocks. (For example when passing such 
 *	 a member variable to another function as VAR_IN_OUT.)
 * 
 * * ASM_PSHO_ST_{w}		*SP 		= *(*SP + o)
 * * ASM_POPO_ST_{w}		*(*SP + o)	= *SP
 * * ASM_LEAO_ST{_1}		*SP 		= *SP + o
 * ----------------------------------------------------------------------------
 */
#if (defined(IP_ENA_OBJxx) || defined(IP_ENA_OBJ_BITxx)) && defined(IP_ENA_DT1)

	case ASM_PSHO_ST_1: /* ---------------------------------------------------- */
	{
		VAL_IP1(o);
		POPPL(p);
		PUSH1(ADDxxPTR(p, o/8), o);
	}
	break;

	case ASM_POPO_ST_1: /* ---------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POPPL(p);
		POP1(ADDxxPTR(p, o/8), o);
	}
	break;

	case ASM_LEAO_ST_1: /* ---------------------------------------------------- */
	{
		VAL_IP1(o);
		POPPL(p);
		PUSHBITPL(ADDxxPTR(p, o/8), o);
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT8)

	case ASM_PSHO_ST_8: /* ---------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POPPL(p);
		PUSH8(ADDxxPTR(p, o)); 
	}
	break;
	
	case ASM_POPO_ST_8: /* ---------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POPPL(p);
		POP8(ADDxxPTR(p, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT16)

	case ASM_PSHO_ST_16: /* --------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POPPL(p);
		PUSH16(ADDxxPTR(p, o)); 
	}
	break;
	
	case ASM_POPO_ST_16: /* --------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POPPL(p);
		POP16(ADDxxPTR(p, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT32)

	case ASM_PSHO_ST_32: /* --------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POPPL(p);
		PUSH32(ADDxxPTR(p, o)); 
	}
	break;
	
	case ASM_POPO_ST_32: /* --------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POPPL(p);
		POP32(ADDxxPTR(p, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT64)

	case ASM_PSHO_ST_64: /* --------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POPPL(p);
		PUSH64(ADDxxPTR(p, o)); 
	}
	break;
	
	case ASM_POPO_ST_64: /* --------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POPPL(p);
		POP64(ADDxxPTR(p, o));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx)

	case ASM_LEAO_ST: /* ------------------------------------------------------ */
	STACK_UCHK(REG_VPTR);
	{
		VAL_IP1(o);
		POPPL(p);
		PUSHPL(ADDxxPTR(p,o));
	}
	break;

#endif


/* ----------------------------------------------------------------------------
 * PSHI_IN / POPI_IN
 * ----------------------------------------------------------------------------
 * * Access to simple variables passed to structures and function blocks as 
 *	 VAR_IN_OUT.
 * 
 * * PSHI_IN_{w}		*SP 		 = *(*(IN + o))
 * * POPI_IN_{w}		*(*(IN + o)) = *SP
 * ----------------------------------------------------------------------------
 */
#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT1)

	case ASM_PSHI_IN_1: /* ---------------------------------------------------- */
	{		  
		VAL_IP1(o);
		p = ADDxxPTR(pIN, o);
		PUSH1(*(VPTR OS_DPTR *)p, *(p + sizeof(VPTR))); 
	}
	break;

	case ASM_POPI_IN_1: /* ---------------------------------------------------- */
	{		  
		VAL_IP1(o);
		p = ADDxxPTR(pIN, o);
		POP1(*(VPTR OS_DPTR *)p, *(p + sizeof(VPTR)));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT8)

	case ASM_PSHI_IN_8: /* ---------------------------------------------------- */
	{
		VAL_IP1(o);
		PUSH8(*(VPTR OS_DPTR *)ADDxxPTR(pIN, o)); 
	}
	break;

	case ASM_POPI_IN_8: /* ---------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POP8(*(VPTR OS_DPTR *)ADDxxPTR(pIN, o));
	}
	break;
		
#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT16)

	case ASM_PSHI_IN_16: /* --------------------------------------------------- */
	{
		VAL_IP1(o);
		PUSH16(*(VPTR OS_DPTR *)ADDxxPTR(pIN, o)); 
	}
	break;

	case ASM_POPI_IN_16: /* --------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POP16(*(VPTR OS_DPTR *)ADDxxPTR(pIN, o));
	}
	break;
		
#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT32)

	case ASM_PSHI_IN_32: /* --------------------------------------------------- */
	{
		VAL_IP1(o);
		PUSH32(*(VPTR OS_DPTR *)ADDxxPTR(pIN, o)); 
	}
	break;

	case ASM_POPI_IN_32: /* --------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POP32(*(VPTR OS_DPTR *)ADDxxPTR(pIN, o));
	}
	break;
		
#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT64)

	case ASM_PSHI_IN_64: /* --------------------------------------------------- */
	{
		VAL_IP1(o);
		PUSH64(*(VPTR OS_DPTR *)ADDxxPTR(pIN, o)); 
	}
	break;

	case ASM_POPI_IN_64: /* --------------------------------------------------- */
	{		  
		VAL_IP1(o);
		POP64(*(VPTR OS_DPTR *)ADDxxPTR(pIN, o));
	}
	break;
		
#endif


/* ----------------------------------------------------------------------------
 * PSHV_ID / POPV_ID / LEAV_ID
 * ----------------------------------------------------------------------------
 * The opcode groups PSHV_ID, POPV_ID and LEAV_ID are used to access array 
 * elements of global array objects.
 *
 * * Access to array elements of global arrays.
 *
 * * PSHV_ID_{w}		*SP 						= *([id] + {AS} + ai*{SOE})
 * * POPV_ID_{w}		*([id] + {AS} + ai*{SOE})	= *SP
 * * LEAV_ID			*SP 						= [id] + {AS} + ai*{SOE}
 * ----------------------------------------------------------------------------
 */
#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT1)
					
	case ASM_PSHV_ID_1: /* ---------------------------------------------------- */
	{
		VAL_ARR(a,id);

		p = pShared->pData[id].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO1;

		PUSH1(ADDxxPTR(p, a/8), a);
	}
	break;
										
	case ASM_POPV_ID_1: /* ---------------------------------------------------- */
	{
		VAL_ARR(a,id);

		p = pShared->pData[id].pAdr;

		CHECK_ARRAY(p, a);
		p += VAIO1;

		POP1(ADDxxPTR(p, a/8), a);
	}
	break;
					
#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT8)
		
	case ASM_PSHV_ID_8: /* ---------------------------------------------------- */
	{
		VAL_ARR(a,id);

		p = pShared->pData[id].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO8;

		PUSH8(ADDxxPTR(p, a*1)); 
	}
	break;
		
	case ASM_POPV_ID_8: /* ---------------------------------------------------- */
	{		  
		VAL_ARR(a,id);

		p = pShared->pData[id].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO8;

		POP8(ADDxxPTR(p, a*1));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT16)
		
	case ASM_PSHV_ID_16: /* --------------------------------------------------- */
	{
		VAL_ARR(a,id);

		p = pShared->pData[id].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO16;

		PUSH16(ADDxxPTR(p, a*2)); 
	}
	break;
		
	case ASM_POPV_ID_16: /* --------------------------------------------------- */
	{		  
		VAL_ARR(a,id);

		p = pShared->pData[id].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO16;

		POP16(ADDxxPTR(p, a*2));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT32)
		
	case ASM_PSHV_ID_32: /* --------------------------------------------------- */
	{
		VAL_ARR(a,id);

		p = pShared->pData[id].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO32;

		PUSH32(ADDxxPTR(p, a*4)); 
	}
	break;
		
	case ASM_POPV_ID_32: /* --------------------------------------------------- */
	{		  
		VAL_ARR(a,id);

		p = pShared->pData[id].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO32;

		POP32(ADDxxPTR(p, a*4));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT64)
		
	case ASM_PSHV_ID_64: /* --------------------------------------------------- */
	{
		VAL_ARR(a,id);

		p = pShared->pData[id].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO64;

		PUSH64(ADDxxPTR(p, a*8)); 
	}
	break;
		
	case ASM_POPV_ID_64: /* --------------------------------------------------- */
	{		  
		VAL_ARR(a,id);

		p = pShared->pData[id].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO64;

		POP64(ADDxxPTR(p, a*8));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx)

	case ASM_LEAV_ID: /* ------------------------------------------------------ */
	{
		IEC_xx e;
		IEC_xx a;
		VAL_IP1(id);

		POPxxL(e);
		POPxxL(a);
		
		p = pShared->pData[id].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO(e);
		
		if (e == 1) {
			PUSHBITPL(ADDxxPTR(p,a/8), a);
		}
		else {
			PUSHPL(ADDxxPTR(p, a * (e/8)));
		}
	}
	break;

#endif


/* ----------------------------------------------------------------------------
 * PSHV_IN / POPV_IN / LEAV_IN
 * ----------------------------------------------------------------------------
 * The opcode groups PSHV_IN, POPV_IN and LEAV_IN are used to access array 
 * elements of local array objects.
 *
 * * Access to array elements of local arrays.
 *
 * * PSHV_IN_{w}	*SP 						  = *([*(IN+i)] + {AS} + ai*{SOE})
 * * POPV_IN_{w}	*([*(IN+i)] + {AS} + ai*{SOE})= *SP
 * * LEAV_IN		*SP 						  = [*(IN+i)] + {AS} + ai*{SOE}
 * ----------------------------------------------------------------------------
 */
#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT1)

	case ASM_PSHV_IN_1: /* ---------------------------------------------------- */
	{		  
		VAL_ARR(a,i);

		p = pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO1;

		PUSH1(ADDxxPTR(p, a/8), a);
	}
	break;
										
	case ASM_POPV_IN_1: /* ---------------------------------------------------- */
	{ 
		VAL_ARR(a,i);

		p = pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO1;

		POP1(ADDxxPTR(p, a/8), a);
	}
	break;
										
#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT8)
		
	case ASM_PSHV_IN_8: /* ---------------------------------------------------- */
	{
		VAL_ARR(a,i);

		p = pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO8;

		PUSH8(ADDxxPTR(p, a*1));
	}
	break;
		
	case ASM_POPV_IN_8: /* ---------------------------------------------------- */
	{		  
		VAL_ARR(a,i);

		p = pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO8;

		POP8(ADDxxPTR(p, a*1));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT16)
		
	case ASM_PSHV_IN_16: /* --------------------------------------------------- */
	{
		VAL_ARR(a,i);

		p = pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO16;

		PUSH16(ADDxxPTR(p, a*2));
	}
	break;
		
	case ASM_POPV_IN_16: /* --------------------------------------------------- */
	{		  
		VAL_ARR(a,i);

		p = pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO16;

		POP16(ADDxxPTR(p, a*2));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT32)
		
	case ASM_PSHV_IN_32: /* --------------------------------------------------- */
	{
		VAL_ARR(a,i);

		p = pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO32;

		PUSH32(ADDxxPTR(p, a*4));
	}
	break;
		
	case ASM_POPV_IN_32: /* --------------------------------------------------- */
	{		  
		VAL_ARR(a,i);

		p = pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO32;

		POP32(ADDxxPTR(p, a*4));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT64)
		
	case ASM_PSHV_IN_64: /* --------------------------------------------------- */
	{
		VAL_ARR(a,i);

		p = pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO64;

		PUSH64(ADDxxPTR(p, a*8));
	}
	break;
		
	case ASM_POPV_IN_64: /* --------------------------------------------------- */
	{		  
		VAL_ARR(a,i);

		p = pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO64;

		POP64(ADDxxPTR(p, a*8));
	}
	break;

#endif

#if defined(IP_ENA_OBJxx)

	case ASM_LEAV_IN: /* ------------------------------------------------------ */
	{
		IEC_xx e;
		IEC_xx a;
		VAL_IP1(i);

		POPxxL(e);
		POPxxL(a);
		
		p = pShared->pData[*(VIDX OS_DPTR *)(ADDxxPTR(pIN, i))].pAdr;
		
		CHECK_ARRAY(p, a);
		p += VAIO(e);

		if (e == 1) {
			PUSHBITPL(ADDxxPTR(p,a/8), a);
		}
		else {
			PUSHPL(ADDxxPTR(p, a * (e/8)));
		}
	}
	break;

#endif


/* ----------------------------------------------------------------------------
 * PSHV_ST / POPV_ST / LEAV_ST
 * ----------------------------------------------------------------------------
 * * Access to elements of arrays passed as VAR_IN_OUT to functions or function 
 *	 blocks.
 * * LEA of elements of arrays passed as VAR_IN_OUT to functions or function 
 *	 blocks. (For example when passing such a member variable to another 
 *	 function as VAR_IN_OUT.)
 *
 * * PSHV_ST_{w}		*SP 						= *(*SP + {AS} + ai*{SOE})
 * * POPV_ST_{w}		*(*SP + {AS} + ai*{SOE})	= *SP
 * * LEAV_ST			*SP 						= *SP + {AS} + ai*{SOE}
 * ----------------------------------------------------------------------------
 */
#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT1)

	case ASM_PSHV_ST_1: /* ---------------------------------------------------- */
	{
		IEC_xx a;

		POPxxL(a);
		POPPL(p);
		
		CHECK_ARRAY(p,a);
		p += VAIO1;

		PUSH1(ADDxxPTR(p, a/8), a);
	}
	break;
		  
	case ASM_POPV_ST_1: /* ---------------------------------------------------- */
	{
		IEC_xx a;
		
		POPxxL(a);
		POPPL(p);
		
		CHECK_ARRAY(p,a);
		p += VAIO1;

		POP1(ADDxxPTR(p, a/8), a);
	}
	break;
			   
#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT8)

	case ASM_PSHV_ST_8: /* ---------------------------------------------------- */
		{
			IEC_xx u;
			
			POPxxL(u);
			POPPL(p);
			
			CHECK_ARRAY(p,u);
			p += VAIO8;

			PUSH8(ADDxxPTR(p, u*1)); 
		}
		break;
		
	case ASM_POPV_ST_8: /* ---------------------------------------------------- */
		{
			IEC_xx u;
			
			POPxxL(u);
			POPPL(p);
			
			CHECK_ARRAY(p,u);
			p += VAIO8;

			POP8(ADDxxPTR(p, u*1));
		}
		break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT16)

	case ASM_PSHV_ST_16: /* --------------------------------------------------- */
		{
			IEC_xx u;
			
			POPxxL(u);
			POPPL(p);
			
			CHECK_ARRAY(p,u);
			p += VAIO16;

			PUSH16(ADDxxPTR(p, u*2)); 
		}
		break;
		
	case ASM_POPV_ST_16: /* --------------------------------------------------- */
		{
			IEC_xx u;
			
			POPxxL(u);
			POPPL(p);
			
			CHECK_ARRAY(p,u);
			p += VAIO16;

			POP16(ADDxxPTR(p, u*2));
		}
		break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT32)

	case ASM_PSHV_ST_32: /* --------------------------------------------------- */
		{
			IEC_xx u;
			
			POPxxL(u);
			POPPL(p);
			
			CHECK_ARRAY(p,u);
			p += VAIO32;

			PUSH32(ADDxxPTR(p, u*4)); 
		}
		break;
		
	case ASM_POPV_ST_32: /* --------------------------------------------------- */
		{
			IEC_xx u;
			
			POPxxL(u);
			POPPL(p);
			
			CHECK_ARRAY(p,u);
			p += VAIO32;

			POP32(ADDxxPTR(p, u*4));
		}
		break;

#endif

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT64)

	case ASM_PSHV_ST_64: /* --------------------------------------------------- */
		{
			IEC_xx u;
			
			POPxxL(u);
			POPPL(p);
			
			CHECK_ARRAY(p,u);
			p += VAIO64;

			PUSH64(ADDxxPTR(p, u*8)); 
		}
		break;
		
	case ASM_POPV_ST_64: /* --------------------------------------------------- */
		{
			IEC_xx u;
			
			POPxxL(u);
			POPPL(p);
			
			CHECK_ARRAY(p,u);
			p += VAIO64;

			POP64(ADDxxPTR(p, u*8));
		}
		break;

	case ASM_LEAV_ST: /* ------------------------------------------------------ */
	{
		IEC_xx a;
		VAL_IP1(e);

		POPxxL(a);
		POPPL(p);

		CHECK_ARRAY(p,a);
		p += VAIO(e);

		if (e == 1) {
			PUSHBITPL(ADDxxPTR(p, a/8), a);
		}
		else {
			PUSHPL(ADDxxPTR(p, a * (e/8)));
		}
	}
	break;

#endif


/* LEA...
 * ----------------------------------------------------------------------------
 */
#if defined(IP_ENA_GLOBxx)

	case ASM_LEAI_ID: /* ------------------------------------------------------ */
	{
		VAL_IP1(o);
		POPPL(p);		
		PUSHPL(pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(p,o)].pAdr);
	}
	break;

	case ASM_LEAV_TX: /* ------------------------------------------------------ */
	{
		IEC_xx a;
		IEC_BYTE len;
		
		POPxxL(a);
		POPPL(p);
		
		CHECK_ARRAY(p,a);
		p += sizeof(VAIS);
			
		len = *(p + sizeof(IEC_STRLEN));
		PUSHPL(p + a * (len + 2 * sizeof(IEC_STRLEN))); 	
	}
	break;
		
	case ASM_LEAV_SS: /* ------------------------------------------------------ */
	{
		IEC_xx a;
		
		POPxxL(a);
		POPPL(p);
		
		CHECK_ARRAY(p,a);
		p += sizeof(VAIS);
			
		PUSHPL(pShared->pData[*((VIDX OS_DPTR *)p + a)].pAdr);
			
		break;
	}

#endif


/* MEMCPY
 * ----------------------------------------------------------------------------
 */
#if defined(IP_ENA_GLOBxx)

	case ASM_MCPY: /* --------------------------------------------------------- */
	{
		IEC_DATA OS_DPTR *pcs;
		IEC_DATA OS_DPTR *pcd;

		IEC_xx b;
		IP_xx(b);
		
		POPPL(pcd);
		POPPL(pcs);
		
		if (b != 0)
		{
			if (pcs == NULL || pcd == NULL)
				return intSetException(pVM, EXCEPT_NULL_PTR, pIP, pIN);
			
			OS_LOCK_MEM;
			{
				OS_MEMCPY(pcd, pcs, b);
			
			} OS_FREE_MEM;
		}
	}	  
	break;

#endif


/* ---------------------------------------------------------------------------- */
/* ASM_PUSH_ID_x		*([id] + o) 		Push value, ID of object within 	*/
/*											instance data.						*/
/*																				*/
/* Access to elements of global structures and to simple, automatic global		*/
/* variables.																	*/
/* ---------------------------------------------------------------------------- */

#if defined(IP_ENA_OBJxx) && defined(IP_ENA_DT16)

	case ASM_PSHX____16: /* --------------------------------------------------- */
	{
		IEC_xx index; 
		VAL_IP1(i);
		
		POPxxL(index);
		PUSH16(ADDxxPTR((IEC_WORD OS_DPTR *)ADDxxPTR(pIN, i), index));
	}
	break;
		
	case ASM_POPX____16: /* --------------------------------------------------- */
	{		  
		IEC_xx index; 
		VAL_IP1(i);

		POPxxL(index);
					
		POP16(ADDxxPTR((IEC_WORD OS_DPTR *)ADDxxPTR(pIN, i), index)); 
	}
	break;

#endif


/* ---------------------------------------------------------------------------- */
/* ASM_PUSH_ID_x		*([id] + o) 		Push value, ID of object within 	*/
/*											instance data.						*/
/*																				*/
/* Access to elements of global structures and to simple, automatic global		*/
/* variables.																	*/
/* ---------------------------------------------------------------------------- */

#if defined(IP_ENA_OBJxx)

#if defined(RTS_CFG_SYSTEM_LIB) || defined(RTS_CFG_SYSTEM_LIB_NT) || defined(RTS_CFG_UTILITY_LIB)

	case ASM_CALF_SL: /* ------------------------------------------------------.*/
	{
		VAL_IP2(f,s);
		
		if (pLocal->uHalt == TRUE)
		{
			return intHalt(pVM);
		}

		if ((IEC_UINT)f < libGetFunCount() && g_pLibraryFun[f] != 0)
		{
			g_pLibraryFun[f](pVM, pIP, pSP);

			STACK_UCHK(s);
			pSP += s;
			
			if (pLocal->Exception.uErrNo != OK) {
				return intSetException(pVM, pLocal->Exception.uErrNo, pIP, pIN);
			}

		} else {
			return intSetException(pVM, EXCEPT_INVALID_LIB_CALL, pIP, pIN);
		}
	}
	break;

	case ASM_CALB_SN: /* ------------------------------------------------------ */
	{
		VAL_IP2(fb,i);
		
		if (pLocal->uHalt == TRUE)
		{
			return intHalt(pVM);
		}

		if ((IEC_UINT)fb < libGetFBCount() && g_pLibraryFB[fb] != 0)
		{
			g_pLibraryFB[fb](pVM, pIP, pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN,i)].pAdr);
			
			if (pLocal->Exception.uErrNo != OK) {
				return intSetException(pVM, pLocal->Exception.uErrNo, pIP, pIN);
			} 

		} else {
			return intSetException(pVM, EXCEPT_INVALID_LIB_CALL, pIP, pIN);
		}
	}
	break;
						  
	case ASM_CALB_SI: /* ------------------------------------------------------ */
	{
		VAL_IP2(fb,id);
		
		if (pLocal->uHalt == TRUE)
		{
			return intHalt(pVM);
		}

		if ((IEC_UINT)fb < libGetFBCount() && g_pLibraryFB[fb] != 0)
		{
			g_pLibraryFB[fb](pVM, pIP, pShared->pData[id].pAdr);
			
			if (pLocal->Exception.uErrNo != OK) {
				return intSetException(pVM, pLocal->Exception.uErrNo, pIP, pIN);
			} 

		} else {
			return intSetException(pVM, EXCEPT_INVALID_LIB_CALL, pIP, pIN);
		}
	}
	break;

#endif /* RTS_CFG_SYSTEM_LIB || RTS_CFG_SYSTEM_LIB_NT || RTS_CFG_UTILITY_LIB */

/* ---------------------------------------------------------------------------- */

#if defined(RTS_CFG_CUSTOMER_LIB)

	case ASM_CALF_CL: /* ------------------------------------------------------ */
	{
		VAL_IP2(f,s);
			
		if (pLocal->uHalt == TRUE)
		{
			return intHalt(pVM);
		}

		if ((IEC_UINT)f < osGetFunCount() && g_pTargetFun[f] != 0)
		{
			g_pTargetFun[f](pVM, pIP, pSP);

			STACK_UCHK(s);
			pSP += s;
			
			if (pLocal->Exception.uErrNo != OK) {
				return intSetException(pVM, pLocal->Exception.uErrNo, pIP, pIN);
			} 
		
		} else {
			return intSetException(pVM, EXCEPT_INVALID_LIB_CALL, pIP, pIN);
		}
	}
	break;

	case ASM_CALB_CN: /* ------------------------------------------------------ */
	{
		VAL_IP2(fb,i);
			
		if (pLocal->uHalt == TRUE)
		{
			return intHalt(pVM);
		}

		if ((IEC_UINT)fb < osGetFBCount() && g_pTargetFB[fb] != 0)
		{				
			g_pTargetFB[fb](pVM, pIP, pShared->pData[*(VIDX OS_DPTR *)ADDxxPTR(pIN, i)].pAdr);				
			
			if (pLocal->Exception.uErrNo != OK) {
				return intSetException(pVM, pLocal->Exception.uErrNo, pIP, pIN);
			} 

		} else {
			return intSetException(pVM, EXCEPT_INVALID_LIB_CALL, pIP, pIN);
		}
	}
	break;

	case ASM_CALB_CI: /* ------------------------------------------------------ */
	{
		VAL_IP2(fb,id);
			
		if (pLocal->uHalt == TRUE)
		{
			return intHalt(pVM);
		}

		if ((IEC_UINT)fb < osGetFBCount() && g_pTargetFB[fb] != 0)
		{
			g_pTargetFB[fb](pVM, pIP, pShared->pData[id].pAdr);
			
			if (pLocal->Exception.uErrNo != OK) {
				return intSetException(pVM, pLocal->Exception.uErrNo, pIP, pIN);
			} 

		} else {
			return intSetException(pVM, EXCEPT_INVALID_LIB_CALL, pIP, pIN);
		}
	}
	break;

#endif	/* RTS_CFG_CUSTOMER_LIB */

/* ---------------------------------------------------------------------------- */

	case ASM_CALF_PR: /* ------------------------------------------------------ */		
	{
		IEC_xx	u;
		
		SContext *pConOld = pLocal->pContext + (pLocal->uContext - 1);
		SContext *pConNew = pLocal->pContext +	pLocal->uContext;

		if (pLocal->uHalt == TRUE)
		{
			return intHalt(pVM);
		}

		if (pLocal->uContext < MAX_CALLS)
		{
			pConOld->uCodePos	= (IEC_UINT)(OS_SUBPTR(pIP + 3 * sizeof(IEC_xx), pShared->pCode[pConOld->uCode].pAdr));
			pConOld->pData		= pIN;				
			pLocal->uContext++;
		
		} else {
			return intSetException(pVM, EXCEPT_CALL_OVERFLOW, pIP, pIN);
		}

		/* set new state */
		IP_xx(u);	pConNew->uCode = (IEC_UINT)u;		/* index in code map */
					pConNew->uData = 0xFFFF;			/* for function call */

		IP_xx(u);			/* Number of bytes to be reserved on the stack for local function variables  */
		STACK_OCHK(u);
		
		pSP -= u;

		IP_xx(u);			/* Number of bytes already occupied on the stack by the function parameters */
							/* (Can be ignored here, just necessary for the NCC.) */
		
		pIP = pShared->pCode[pConNew->uCode].pAdr;
		pIN = pSP;
	}
	break;
		
	case ASM_CALB_PN: /* ------------------------------------------------------ */
	{
		IEC_xx u;
		
		SContext *pConOld = pLocal->pContext + (pLocal->uContext - 1);
		SContext *pConNew = pLocal->pContext +	pLocal->uContext;

		if (pLocal->uHalt == TRUE)
		{
			return intHalt(pVM);
		}

		if (pLocal->uContext < MAX_CALLS)
		{
			pConOld->uCodePos	= (IEC_UINT)(OS_SUBPTR(pIP + 2 * sizeof(IEC_xx), pShared->pCode[pConOld->uCode].pAdr));
			pConOld->pData		= pIN;
			pLocal->uContext++;
		
		} else {
			return intSetException(pVM, EXCEPT_CALL_OVERFLOW, pIP, pIN);
		}

		/* set new state */
		IP_xx(u);	pConNew->uCode = (IEC_UINT) u;
		IP_xx(u);	pConNew->uData = (IEC_UINT) *(VIDX OS_DPTR *)(ADDxxPTR(pIN, u));
		
		pIP = pShared->pCode[pConNew->uCode].pAdr;
		pIN = pShared->pData[pConNew->uData].pAdr;
	}
	break;
					   
	case ASM_CALB_PI: /* ------------------------------------------------------ */
	{
		IEC_xx u;

		SContext *pConOld = pLocal->pContext + (pLocal->uContext - 1);
		SContext *pConNew = pLocal->pContext +	pLocal->uContext;

		if (pLocal->uHalt == TRUE)
		{
			return intHalt(pVM);
		}

		if (pLocal->uContext < MAX_CALLS)
		{
			pConOld->uCodePos	= (IEC_UINT)(OS_SUBPTR(pIP + 2 * sizeof(IEC_xx), pShared->pCode[pConOld->uCode].pAdr));
			pConOld->pData		= pIN;
			pLocal->uContext++;
		
		} else {
			return intSetException(pVM, EXCEPT_CALL_OVERFLOW, pIP, pIN);
		}

		/* set new state */
		IP_xx(u);	pConNew->uCode = (IEC_UINT) u;
		IP_xx(u);	pConNew->uData = (IEC_UINT) u;

		pIP = pShared->pCode[pConNew->uCode].pAdr;
		pIN = pShared->pData[pConNew->uData].pAdr;
	}
	break;

#endif

/* ---------------------------------------------------------------------------- */
