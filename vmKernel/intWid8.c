
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
 * Filename: intWid8.c
 */


/* ---------------------------------------------------------------------------- */

#define IEC_xx			IEC_BYTE
#define IEC_Sxx 		IEC_SINT
#define IP_xx			IP_BYTE
#define POPxxL			POP8L
#define PUSHxxL 		PUSH8L
#define ADDxxPTR		OS_ADDPTR8
#define LABxx(ll)		ll##8

#define JUMPxxOFF		(sizeof(IEC_BYTE) + sizeof(IEC_xx)) /* Opc. + operand */

#define ASM_PSHC_VV_xx		ASM_PSHC_VV_8
#define ASM_PSHC_00_xx		ASM_PSHC_00_8
#define ASM_PSHC_01_xx		ASM_PSHC_01_8


/* Include PSHC
 */
#if defined(IP_CFG_BOOL) || defined(IP_CFG_SINT) || defined(IP_CFG_USINT) || defined(IP_CFG_BYTE)
  #define IP_ENA_PSHCxx
#endif

/* Include wide xx opcodes for global data access
 */
#if defined(IP_CFG_GLOB8)
  #define IP_ENA_GLOBxx
#endif

#if defined(IP_CFG_GLOB8)
  #define IP_ENA_GLOB_BITxx
#endif

/* Include wide xx opcodes for object data access
 */
#if defined(IP_CFG_OBJ8)
  #define IP_ENA_OBJxx
#endif

#if defined(IP_CFG_OBJ8)
  #define IP_ENA_OBJ_BITxx
#endif

/* ---------------------------------------------------------------------------- */

#include "intWide.c"	/* ---------------------------------------------------- */

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
