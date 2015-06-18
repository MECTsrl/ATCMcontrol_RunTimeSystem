
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
 * Filename: intOpcds.h
 */


#if ( defined(OPC_CODE_GEN	  ) && !defined(CG_ASMOPCODES_H_) ) || \
	( defined(OPC_ASM		  ) && !defined(ASM_ASMOPCODES_H_)) || \
	( defined(OPC_INTERPRETER ) && !defined(IP_ASMOPCODES_H_) ) || \
	( defined(OPC_NCC_BASE	  ) && !defined(IC_ASMOPCODES_H_) ) || \
	( defined(OPC_NCC		  ) && !defined(NC_ASMOPCODES_H_) ) 

#ifdef OPC_ASM
#define ASM_ASMOPCODES_H_
#endif

#define PREF_CONV		0x0003	  /* prefix convert block				*/
#define PREF_ARITH		0x0004	  /* prefix arithmetic block			*/
	   
#define PREF_WIDE8		0x0010	  /* wide  8 for param in main table	*/
#define PREF_WIDE16 	0x0011	  /* wide 16		- " -				*/
#define PREF_WIDE32 	0x0012	  /* wide 32		- " -				*/
#define PREF_WIDE64 	0x0013	  /* wide 64		- " -				*/

#define CONVCODE(cc)	((PREF_CONV<<8)|(cc))
#define ARITHCODE(cc)	((PREF_ARITH<<8)|(cc))

#define WIDE_8CODE(cc)	((PREF_WIDE8<<8)|(cc))
#define WIDE16CODE(cc)	((PREF_WIDE16<<8)|(cc))
#define WIDE32CODE(cc)	((PREF_WIDE32<<8)|(cc))
#define WIDE64CODE(cc)	((PREF_WIDE64<<8)|(cc))


#define live_disable	0	/* Code nicht benutzt						*/
#define live_ip 		1	/* Code im IP abarbeiten					*/
#define live_nc 		2	/* Code in Maschinencode wandeln			*/

							/* ======== STACK ========													*/
#define type_push		0	/* vorher = 0, nachher = 1													*/
#define type_op1		1	/* vorher = 1, nachher = 1	; z.B. NOT aber auch alle PUSH-Vektor-Befehle	*/
#define type_op2		2	/* vorher = 2, nachher = 1	; z.B. ADD										*/
#define type_pop		3	/* vorher = 1, nachher = 0													*/
#define type_statm		4	/* vorher = 0, nachher = 0													*/
#define type_cc 		5	/* vorher = 1, nachher = 0	; bedingte Sprünge ...							*/
#define type_par		6	/* vorher = 0, nachher = 1	; formaler Parameter							*/
#define type_op3		7	/* vorher = 3, nachher = 1	; z.B. SEL, MUX -> stets IP 					*/
#define type_popv		8	/* vorher = 2, nachher = 0	; POP Vektor Befehle							*/
#define type_fct		9	/* vorher = N, nachher = 1	; Funktionsaufruf								*/

#ifndef _OPCODE_DEF_
#define _OPCODE_DEF_

typedef struct __COpcode 
{
	const	 char* OpcodeString;
	unsigned short OpCode;
	unsigned char  live;		/* free - ip - ncc						*/
	unsigned char  type;		/*										*/ 
	unsigned char  parCount;	/* number of parameter (in ip-stream)	*/
	unsigned char  dataCount;	/* size of operands in bits 			*/
} COpcode;

#endif /* _OPCODE_DEF_ */


/* OPC_CODE_GEN - 4asm.exe
 * ----------------------------------------------------------------------------
 */
#ifdef OPC_CODE_GEN

	typedef struct tag_CodeTableEntry
	{
		const char* 	OpcodeString;
		unsigned short	OpCode;
	}
	ASM_CodeTableEntry;

	int ASM_getNumberOfOpcodes();
	ASM_CodeTableEntry* ASM_getOpcode(int i);

	#undef	MAKE_ENUM
	#define MAKE_ENUM(Symbol,Opcode,Live,Type,ParCount,DataCount) {#Symbol, Opcode}


/* OPC_INTERPRETER - Run Time System
 * ----------------------------------------------------------------------------
 */
#elif OPC_INTERPRETER

	#undef	MAKE_ENUM
	#define MAKE_ENUM(Symbol,Opcode,Live,Type,ParCount,DataCount) ASM_##Symbol=Opcode&0xFF


/* OPC_NCC_BASE - NCCBase
 * ----------------------------------------------------------------------------
 */
#elif OPC_NCC_BASE

	int ASM_getNumberOfOpcodes();
	COpcode ASM_getOpcode(int opcode);

	#undef	MAKE_ENUM
	#define MAKE_ENUM(Symbol,Opcode,Live,Type,ParCount,DataCount) ASM_##Symbol=Opcode


/* OPC_NCC - NCC
 * ----------------------------------------------------------------------------
 */
#elif OPC_NCC

	#include "nccOpcds.h"	  /* read NCC code generation constants */

	int ASM_getNumberOfOpcodes();
	COpcode ASM_getOpcode(int i);

	#undef	MAKE_ENUM
	#define MAKE_ENUM(Symbol,Opcode,Live,Type,ParCount,DataCount) {#Symbol,Opcode,Live,Type,ParCount,DataCount}

#endif


#if defined (OPC_CODE_GEN) || defined(OPC_INTERPRETER) || defined(OPC_NCC_BASE) || defined(OPC_NCC)

#ifdef OPC_CODE_GEN
 #define CG_ASMOPCODES_H_
	ASM_CodeTableEntry OpcodeTable[] = {
#elif OPC_INTERPRETER
 #define IP_ASMOPCODES_H_
	enum ASM_Opcodes {
#elif OPC_NCC_BASE
 #define IC_ASMOPCODES_H_
	typedef enum ASM_Opcodes {
#elif OPC_NCC
 #define NC_ASMOPCODES_H_
	COpcode OpcodeList[] = {
#endif

/* -------- M A I N   T A B L E  -------------------------------------------------------------------------- */

	MAKE_ENUM(ILLEGAL			,0xFFFF 			,NCC_ILLEGAL			, 0, 0, 0), /*					*/
/* -0x000x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(NOP				,0x0000 			,NCC_NOP				, 4, 0, 0), /*					*/
	MAKE_ENUM(BREAK 			,0x0001 			,NCC_BREAK				, 4, 0, 0), /*					*/
	MAKE_ENUM(ACTIVE_BREAK		,0x0002 			,NCC_ACTIVE_BREAK		, 4, 0, 0), /*					*/
/*	MAKE_ENUM(PREF_CONV 		,0x0003 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(PREF_ARITH		,0x0004 			,						, 0, 0, 0), 					*/
	MAKE_ENUM(NATIVE			,0x0005 			,NCC_NATIVE 			, 0, 0, 0), /*					*/
	MAKE_ENUM(CALB_SN			,0x0006 			,NCC_CALB_SN			, 4, 2, 0), /* CALFBSYSID		*/
	MAKE_ENUM(CALB_SI			,0x0007 			,NCC_CALB_SI			, 4, 2, 0), /* CALFBSYS 		*/
	MAKE_ENUM(PSHI_IN_1 		,0x0008 			,NCC_PSHI_IN_1			, 0, 1, 1), /* PUSHINDID1		*/
	MAKE_ENUM(PSHI_IN_8 		,0x0009 			,NCC_PSHI_IN_8			, 0, 1, 8), /* PUSHINDID8		*/
	MAKE_ENUM(PSHI_IN_16		,0x000A 			,NCC_PSHI_IN_16 		, 0, 1,16), /* PUSHINDID16		*/
	MAKE_ENUM(PSHI_IN_32		,0x000B 			,NCC_PSHI_IN_32 		, 0, 1,32), /* PUSHINDID32		*/
	MAKE_ENUM(PSHI_IN_64		,0x000C 			,NCC_PSHI_IN_64 		, 0, 1,64), /* PUSHINDID64		*/
	MAKE_ENUM(RETN				,0x000D 			,NCC_RETN				, 4, 0, 0), /* RET				*/
/*	MAKE_ENUM(					,0x000E 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x000F 			,						, 0, 0, 0), 					*/
/* -0x001x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(PREF_WIDE8		,0x0010 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(PREF_WIDE16		,0x0011 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(PREF_WIDE32		,0x0012 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(PREF_WIDE64		,0x0013 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x0014 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x0015 			,						, 0, 0, 0), 					*/
	MAKE_ENUM(CALF_SL			,0x0016 			,NCC_CALF_SL			, 9, 2, 0), /* CALFSYS			*/
/*	MAKE_ENUM(					,0x0017 			,						, 0, 0, 0), 					*/
	MAKE_ENUM(POPI_IN_1 		,0x0018 			,NCC_POPI_IN_1			, 3, 1, 1), /* POPINDID1		*/
	MAKE_ENUM(POPI_IN_8 		,0x0019 			,NCC_POPI_IN_8			, 3, 1, 8), /* POPINDID8		*/
	MAKE_ENUM(POPI_IN_16		,0x001A 			,NCC_POPI_IN_16 		, 3, 1,16), /* POPINDID16		*/
	MAKE_ENUM(POPI_IN_32		,0x001B 			,NCC_POPI_IN_32 		, 3, 1,32), /* POPINDID32		*/
	MAKE_ENUM(POPI_IN_64		,0x001C 			,NCC_POPI_IN_64 		, 3, 1,64), /* POPINDID64		*/

  #ifdef OPC_NCC_BASE  /* NCC only */
	MAKE_ENUM(PUSHC 			,0x001D 			,NCC_PUSHC				, 0, 1, 0), /* PUSHI			*/
	MAKE_ENUM(PUSHC_0			,0x001E 			,NCC_PUSHC_0			, 0, 0, 0), /* PUSHI_0			*/
	MAKE_ENUM(PUSHC_1			,0x001F 			,NCC_PUSHC_1			, 0, 0, 0), /* PUSHI_1			*/
  #endif

	MAKE_ENUM(PSHC_VV_8 		,0x001D 			,NCC_PSHC_VV_8			, 0, 1, 8), /* PUSHI8			*/
	MAKE_ENUM(PSHC_VV_16		,WIDE16CODE(0x1D)	,NCC_PSHC_VV_16 		, 0, 1,16), /* PUSHI16			*/
	MAKE_ENUM(PSHC_VV_32		,WIDE32CODE(0x1D)	,NCC_PSHC_VV_32 		, 0, 1,32), /* PUSHI32			*/
	MAKE_ENUM(PSHC_VV_64		,WIDE64CODE(0x1D)	,NCC_PSHC_VV_64 		, 0, 1,64), /* PUSHI64			*/
	MAKE_ENUM(PSHC_00_1 		,0x001E 			,NCC_PSHC_00_1			, 0, 0, 1), /* PUSHI1_0 		*/
	MAKE_ENUM(PSHC_00_8 		,0x001E 			,NCC_PSHC_00_8			, 0, 0, 8), /* PUSHI8_0 		*/
	MAKE_ENUM(PSHC_00_16		,WIDE16CODE(0x1E)	,NCC_PSHC_00_16 		, 0, 0,16), /* PUSHI16_0		*/
	MAKE_ENUM(PSHC_00_32		,WIDE32CODE(0x1E)	,NCC_PSHC_00_32 		, 0, 0,32), /* PUSHI32_0		*/
	MAKE_ENUM(PSHC_00_64		,WIDE64CODE(0x1E)	,NCC_PSHC_00_64 		, 0, 0,64), /* PUSHI64_0		*/
	MAKE_ENUM(PSHC_01_1 		,0x001F 			,NCC_PSHC_01_1			, 0, 0, 1), /* PUSHI1_1 		*/
	MAKE_ENUM(PSHC_01_8 		,0x001F 			,NCC_PSHC_01_8			, 0, 0, 8), /* PUSHI8_1 		*/
	MAKE_ENUM(PSHC_01_16		,WIDE16CODE(0x1F)	,NCC_PSHC_01_16 		, 0, 0,16), /* PUSHI16_1		*/
	MAKE_ENUM(PSHC_01_32		,WIDE32CODE(0x1F)	,NCC_PSHC_01_32 		, 0, 0,32), /* PUSHI32_1		*/
	MAKE_ENUM(PSHC_01_64		,WIDE64CODE(0x1F)	,NCC_PSHC_01_64 		, 0, 0,64), /* PUSHI34_1		*/
/* -0x002x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(SHL_BOOL			,0x0020 			,NCC_SHL_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(SHR_BOOL			,0x0021 			,NCC_SHR_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(ROR_BOOL			,0x0022 			,NCC_ROR_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(ROL_BOOL			,0x0023 			,NCC_ROL_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(AND_BOOL			,0x0024 			,NCC_AND_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(OR_BOOL			,0x0025 			,NCC_OR_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(XOR_BOOL			,0x0026 			,NCC_XOR_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(NOT_BOOL			,0x0027 			,NCC_NOT_BOOL			, 1, 0, 1), /*					*/
/*	MAKE_ENUM(					,0x0028 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x0029 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x002A 			,						, 0, 0, 0), 					*/ 
/*	MAKE_ENUM(					,0x002B 			,						, 0, 0, 0), 					*/ 
/*	MAKE_ENUM(					,0x002C 			,						, 0, 0, 0), 					*/ 
/*	MAKE_ENUM(					,0x002D 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x002E 			,						, 0, 0, 0), 					*/ 
/*	MAKE_ENUM(					,0x002F 			,						, 0, 0, 0), 					*/ 
/* -0x003x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(SHL_BYTE			,0x0030 			,NCC_SHL_BYTE			, 2, 0, 8), /*					*/
	MAKE_ENUM(SHR_BYTE			,0x0031 			,NCC_SHR_BYTE			, 2, 0, 8), /*					*/
	MAKE_ENUM(ROR_BYTE			,0x0032 			,NCC_ROR_BYTE			, 2, 0, 8), /*					*/
	MAKE_ENUM(ROL_BYTE			,0x0033 			,NCC_ROL_BYTE			, 2, 0, 8), /*					*/
	MAKE_ENUM(AND_BYTE			,0x0034 			,NCC_AND_BYTE			, 2, 0, 8), /*					*/
	MAKE_ENUM(OR_BYTE			,0x0035 			,NCC_OR_BYTE			, 2, 0, 8), /*					*/
	MAKE_ENUM(XOR_BYTE			,0x0036 			,NCC_XOR_BYTE			, 2, 0, 8), /*					*/
	MAKE_ENUM(NOT_BYTE			,0x0037 			,NCC_NOT_BYTE			, 1, 0, 8), /*					*/
	MAKE_ENUM(CLST				,0x0038 			,NCC_CLST				, 4, 1, 0), /* LEAVE			*/ 
	MAKE_ENUM(PSHO_ST_1 		,0x0039 			,NCC_PSHO_ST_1			, 1, 1, 1), /* PUSHINDOS1		*/
	MAKE_ENUM(PSHO_ST_8 		,0x003A 			,NCC_PSHO_ST_8			, 1, 1, 8), /* PUSHINDOS8		*/
	MAKE_ENUM(PSHO_ST_16		,0x003B 			,NCC_PSHO_ST_16 		, 1, 1,16), /* PUSHINDOS16		*/
	MAKE_ENUM(PSHO_ST_32		,0x003C 			,NCC_PSHO_ST_32 		, 1, 1,32), /* PUSHINDOS32		*/
	MAKE_ENUM(PSHO_ST_64		,0x003D 			,NCC_PSHO_ST_64 		, 1, 1,64), /* PUSHINDOS64		*/
	MAKE_ENUM(PSHO_IN_1 		,0x003E 			,NCC_PSHO_IN_1			, 0, 2, 0), /* PUSHINSTOID1 	*/
	MAKE_ENUM(POPO_IN_1 		,0x003F 			,NCC_POPO_IN_1			, 3, 2, 0), /* POPINSTOID1		*/
/* -0x004x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(SHL_WORD			,0x0040 			,NCC_SHL_WORD			, 2, 0,16), /*					*/
	MAKE_ENUM(SHR_WORD			,0x0041 			,NCC_SHR_WORD			, 2, 0,16), /*					*/
	MAKE_ENUM(ROR_WORD			,0x0042 			,NCC_ROR_WORD			, 2, 0,16), /*					*/
	MAKE_ENUM(ROL_WORD			,0x0043 			,NCC_ROL_WORD			, 2, 0,16), /*					*/
	MAKE_ENUM(AND_WORD			,0x0044 			,NCC_AND_WORD			, 2, 0,16), /*					*/
	MAKE_ENUM(OR_WORD			,0x0045 			,NCC_OR_WORD			, 2, 0,16), /*					*/
	MAKE_ENUM(XOR_WORD			,0x0046 			,NCC_XOR_WORD			, 2, 0,16), /*					*/
	MAKE_ENUM(NOT_WORD			,0x0047 			,NCC_NOT_WORD			, 1, 0,16), /*					*/
	MAKE_ENUM(GT_BOOL			,0x0048 			,NCC_GT_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(GE_BOOL			,0x0049 			,NCC_GE_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(EQ_BOOL			,0x004A 			,NCC_EQ_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(LE_BOOL			,0x004B 			,NCC_LE_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(LT_BOOL			,0x004C 			,NCC_LT_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(NE_BOOL			,0x004D 			,NCC_NE_BOOL			, 2, 0, 1), /*					*/
	MAKE_ENUM(PSHO_IN_8 		,0x004E 			,NCC_PSHO_IN_8			, 0, 2, 8), /* PUSHINSTOID8 	*/
	MAKE_ENUM(POPO_IN_8 		,0x004F 			,NCC_POPO_IN_8			, 3, 2, 8), /* POPINSTOID8		*/
/* -0x005x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(PSHD_IN_1 		,0x0050 			,NCC_PSHD_IN_1			, 0, 1, 1), /* PUSHID1			*/
	MAKE_ENUM(PSHD_GR_1 		,0x0051 			,NCC_PSHD_GR_1			, 0, 1, 1), /* PUSHGDR1 		*/
	MAKE_ENUM(PSHD_GI_1 		,0x0052 			,NCC_PSHD_GI_1			, 0, 1, 1), /* PUSHGDI1 		*/
	MAKE_ENUM(PSHD_GO_1 		,0x0053 			,NCC_PSHD_GO_1			, 0, 1, 1), /* PUSHGDO1 		*/
	MAKE_ENUM(PSHD_GM_1 		,0x0054 			,NCC_PSHD_GM_1			, 0, 1, 1), /* PUSHGDM1 		*/
	MAKE_ENUM(POPD_IN_1 		,0x0055 			,NCC_POPD_IN_1			, 3, 1, 1), /* POPID1			*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(POPD_GR_1 		,0x0056 			,NCC_POPD_GR_1			, 3, 1, 1), /* prior V2.1.0 	*/
  #endif
	MAKE_ENUM(POPD_GI_1 		,0x0057 			,NCC_POPD_GI_1			, 3, 1, 1), /* POPGDI1			*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(POPD_GO_1 		,0x0058 			,NCC_POPD_GO_1			, 3, 1, 1), /* prior V2.06		*/
  #endif
	MAKE_ENUM(POPD_GM_1 		,0x0059 			,NCC_POPD_GM_1			, 3, 1, 1), /* POPGDM1			*/
	MAKE_ENUM(PSHO_IN_16		,0x005A 			,NCC_PSHO_IN_16 		, 0, 2,16), /* PUSHINSTOID16	*/
	MAKE_ENUM(POPO_IN_16		,0x005B 			,NCC_POPO_IN_16 		, 3, 2,16), /* POPINSTOID16 	*/
	MAKE_ENUM(POPO_IN_32		,0x005C 			,NCC_POPO_IN_32 		, 3, 2,32), /* POPINSTOID32 	*/
	MAKE_ENUM(PSHO_IN_32		,0x005D 			,NCC_PSHO_IN_32 		, 0, 2,32), /* PUSHINSTOID32	*/
	MAKE_ENUM(POPO_IN_64		,0x005E 			,NCC_POPO_IN_64 		, 3, 2,64), /* POPINSTOID64 	*/
	MAKE_ENUM(PSHO_IN_64		,0x005F 			,NCC_PSHO_IN_64 		, 0, 2,64), /* PUSHINSTOID64	*/
/* -0x006x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(PSHV_IN_1 		,0x0060 			,NCC_PSHV_IN_1			, 1, 1, 1), /* PUSHVID1 		*/
	MAKE_ENUM(PSHV_ID_1 		,0x0061 			,NCC_PSHV_ID_1			, 1, 1, 1), /* PUSHV1			*/
	MAKE_ENUM(POPD_GX_1 		,0x0062 			,NCC_POPD_GX_1			, 3, 1, 1), /* POPGDOX1 		*/
	MAKE_ENUM(POPD_GX_8 		,0x0063 			,NCC_POPD_GX_8			, 3, 1, 8), /* POPGDOX8 		*/
	MAKE_ENUM(POPD_GX_16		,0x0064 			,NCC_POPD_GX_16 		, 3, 1,16), /* POPGDOX16		*/
	MAKE_ENUM(POPV_IN_1 		,0x0065 			,NCC_POPV_IN_1			, 8, 1, 1), /* POPVID1			*/
	MAKE_ENUM(POPV_ID_1 		,0x0066 			,NCC_POPV_ID_1			, 8, 1, 1), /* POPV1			*/
	MAKE_ENUM(POPD_GX_32		,0x0067 			,NCC_POPD_GX_32 		, 3, 1,32), /* POPGDOX32		*/
	MAKE_ENUM(POPD_GX_64		,0x0068 			,NCC_POPD_GX_64 		, 3, 1,64), /* POPGDOX64		*/
	MAKE_ENUM(LEAD_GX			,0x0069 			,NCC_LEAD_GX			, 0, 2,32), /* LEAGDX			*/
	MAKE_ENUM(PSHO_ID_1 		,0x006A 			,NCC_PSHO_ID_1			, 0, 2, 1), /* PUSHINSTO1		*/
	MAKE_ENUM(POPO_ID_1 		,0x006B 			,NCC_POPO_ID_1			, 3, 2, 1), /* POPINSTO1		*/
	MAKE_ENUM(PSHO_ID_8 		,0x006C 			,NCC_PSHO_ID_8			, 0, 2, 8), /* PUSHINSTO8		*/
	MAKE_ENUM(POPO_ID_8 		,0x006D 			,NCC_POPO_ID_8			, 3, 2, 8), /* POPINSTO8		*/
	MAKE_ENUM(PSHO_ID_16		,0x006E 			,NCC_PSHO_ID_16 		, 0, 2,16), /* PUSHINSTO16		*/
	MAKE_ENUM(POPO_ID_16		,0x006F 			,NCC_POPO_ID_16 		, 3, 2,16), /* POPINSTO16		*/
/* -0x007x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(PSHD_IN_8 		,0x0070 			,NCC_PSHD_IN_8			, 0, 1, 8), /* PUSHID8			*/
	MAKE_ENUM(PSHD_GR_8 		,0x0071 			,NCC_PSHD_GR_8			, 0, 1, 8), /* PUSHGDR8 		*/
	MAKE_ENUM(PSHD_GI_8 		,0x0072 			,NCC_PSHD_GI_8			, 0, 1, 8), /* PUSHGDI8 		*/
	MAKE_ENUM(PSHD_GO_8 		,0x0073 			,NCC_PSHD_GO_8			, 0, 1, 8), /* PUSHGDO8 		*/
	MAKE_ENUM(PSHD_GM_8 		,0x0074 			,NCC_PSHD_GM_8			, 0, 1, 8), /* PUSHGDM8 		*/
	MAKE_ENUM(POPD_IN_8 		,0x0075 			,NCC_POPD_IN_8			, 3, 1, 8), /* POPID8			*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(POPD_GR_8 		,0x0076 			,NCC_POPD_GR_8			, 3, 1, 8), /* prior V2.1.0 	*/
  #endif
	MAKE_ENUM(POPD_GI_8 		,0x0077 			,NCC_POPD_GI_8			, 3, 1, 8), /* POPGDI8			*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(POPD_GO_8 		,0x0078 			,NCC_POPD_GO_8			, 3, 1, 8), /* prior V2.06		*/
  #endif
	MAKE_ENUM(POPD_GM_8 		,0x0079 			,NCC_POPD_GM_8			, 3, 1, 8), /* POPGDM8			*/
	MAKE_ENUM(PSHO_ID_32		,0x007A 			,NCC_PSHO_ID_32 		, 0, 2,32), /* PUSHINSTO32		*/
	MAKE_ENUM(POPO_ID_32		,0x007B 			,NCC_POPO_ID_32 		, 3, 2,32), /* POPINSTO32		*/
	MAKE_ENUM(PSHO_ID_64		,0x007C 			,NCC_PSHO_ID_64 		, 0, 2,64), /* PUSHINSTO64		*/
	MAKE_ENUM(POPO_ID_64		,0x007D 			,NCC_POPO_ID_64 		, 3, 2,64), /* POPINSTO64		*/
	MAKE_ENUM(LEAD_GI_1 		,0x007E 			,NCC_LEAD_GI_1			, 0, 1,64), /* LEAGDI1			*/
	MAKE_ENUM(LEAD_GX_1 		,0x007F 			,NCC_LEAD_GX_1			, 0, 1,64), /* LEAGDOX1 		*/
/* -0x008x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(PSHV_IN_8 		,0x0080 			,NCC_PSHV_IN_8			, 1, 1, 8), /* PUSHVID8 		*/
	MAKE_ENUM(PSHV_ID_8 		,0x0081 			,NCC_PSHV_ID_8			, 1, 1, 8), /* PUSHV8			*/
	MAKE_ENUM(LEAD_GM_1 		,0x0082 			,NCC_LEAD_GM_1			, 0, 1,64), /* LEAGDM1			*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(LEAD_GR_1 		,0x0083 			,NCC_LEAD_GR_1			, 0, 1, 0), /* prior V2.1.0 	*/
  #endif
	MAKE_ENUM(LEAD_IN_1 		,0x0084 			,NCC_LEAD_IN_1			, 0, 1,64), /* LEAID1			*/ 
	MAKE_ENUM(POPV_IN_8 		,0x0085 			,NCC_POPV_IN_8			, 8, 1, 8), /* POPVID8			*/
	MAKE_ENUM(POPV_ID_8 		,0x0086 			,NCC_POPV_ID_8			, 8, 1, 8), /* POPV8			*/
/*	MAKE_ENUM(					,0x0087 			,						, 0, 0, 0), 					*/
	MAKE_ENUM(POPO_ST_1 		,0x0088 			,NCC_POPO_ST_1			, 2, 1, 1), /* POPINDOS1		*/
	MAKE_ENUM(POPO_ST_8 		,0x0089 			,NCC_POPO_ST_8			, 2, 1, 8), /* POPINDOS8		*/
	MAKE_ENUM(POPO_ST_16		,0x008A 			,NCC_POPO_ST_16 		, 2, 1,16), /* POPINDOS16		*/
	MAKE_ENUM(POPO_ST_32		,0x008B 			,NCC_POPO_ST_32 		, 2, 1,32), /* POPINDOS32		*/
	MAKE_ENUM(POPO_ST_64		,0x008C 			,NCC_POPO_ST_64 		, 2, 1,64), /* POPINDOS64		*/
	MAKE_ENUM(JMP				,0x008D 			,NCC_JMP				, 4, 1, 0), /*					*/
	MAKE_ENUM(JMPC				,0x008E 			,NCC_JMPC				, 5, 1, 0), /*					*/
	MAKE_ENUM(JMPCN 			,0x008F 			,NCC_JMPCN				, 5, 1, 0), /*					*/
/* -0x009x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(NEG_SINT			,0x0090 			,NCC_NEG_SINT			, 1, 0, 8), /*					*/
	MAKE_ENUM(ABS_SINT			,0x0091 			,NCC_ABS_SINT			, 1, 0, 8), /*					*/
	MAKE_ENUM(ADD_SINT			,0x0092 			,NCC_ADD_SINT			, 2, 0, 8), /*					*/
	MAKE_ENUM(MUL_SINT			,0x0093 			,NCC_MUL_SINT			, 2, 0, 8), /*					*/
	MAKE_ENUM(SUB_SINT			,0x0094 			,NCC_SUB_SINT			, 2, 0, 8), /*					*/
	MAKE_ENUM(DIV_SINT			,0x0095 			,NCC_DIV_SINT			, 2, 0, 8), /*					*/
	MAKE_ENUM(MOD_SINT			,0x0096 			,NCC_MOD_SINT			, 2, 0, 8), /*					*/
	MAKE_ENUM(CALF_CL			,0x0097 			,NCC_CALF_CL			, 9, 2, 0), /* CALFM			*/
	MAKE_ENUM(NEG_INT			,0x0098 			,NCC_NEG_INT			, 1, 0,16), /*					*/
	MAKE_ENUM(ABS_INT			,0x0099 			,NCC_ABS_INT			, 1, 0,16), /*					*/
	MAKE_ENUM(ADD_INT			,0x009A 			,NCC_ADD_INT			, 2, 0,16), /*					*/
	MAKE_ENUM(MUL_INT			,0x009B 			,NCC_MUL_INT			, 2, 0,16), /*					*/
	MAKE_ENUM(SUB_INT			,0x009C 			,NCC_SUB_INT			, 2, 0,16), /*					*/
	MAKE_ENUM(DIV_INT			,0x009D 			,NCC_DIV_INT			, 2, 0,16), /*					*/
	MAKE_ENUM(MOD_INT			,0x009E 			,NCC_MOD_INT			, 2, 0,16), /*					*/
	MAKE_ENUM(CALB_CN			,0x009F 			,NCC_CALB_CN			, 4, 2, 0), /* CALFBMID 		*/
/* -0x00Ax ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(PSHD_IN_16		,0x00A0 			,NCC_PSHD_IN_16 		, 0, 1,16), /* PUSHID16 		*/
	MAKE_ENUM(PSHD_GR_16		,0x00A1 			,NCC_PSHD_GR_16 		, 0, 1,16), /* PUSHGDR16		*/
	MAKE_ENUM(PSHD_GI_16		,0x00A2 			,NCC_PSHD_GI_16 		, 0, 1,16), /* PUSHGDI16		*/
	MAKE_ENUM(PSHD_GO_16		,0x00A3 			,NCC_PSHD_GO_16 		, 0, 1,16), /* PUSHGDO16		*/
	MAKE_ENUM(PSHD_GM_16		,0x00A4 			,NCC_PSHD_GM_16 		, 0, 1,16), /* PUSHGDM16		*/
	MAKE_ENUM(POPD_IN_16		,0x00A5 			,NCC_POPD_IN_16 		, 3, 1,16), /* POPID16			*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(POPD_GR_16		,0x00A6 			,NCC_POPD_GR_16 		, 3, 1,16), /* prior V2.1.0 	*/
  #endif	
	MAKE_ENUM(POPD_GI_16		,0x00A7 			,NCC_POPD_GI_16 		, 3, 1,16), /* POPGDI16 		*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(POPD_GO_16		,0x00A8 			,NCC_POPD_GO_16 		, 3, 1,16), /* prior V2.06		*/
  #endif
	MAKE_ENUM(POPD_GM_16		,0x00A9 			,NCC_POPD_GM_16 		, 3, 1,16), /* POPGDM16 		*/
	MAKE_ENUM(LEAD_IN			,0x00AA 			,NCC_LEAD_IN			, 0, 1,32), /* LEAID			*/
	MAKE_ENUM(LEAD_GM			,0x00AB 			,NCC_LEAD_GM			, 0, 1,32), /* LEAGDM			*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(LEAD_GR			,0x00AC 			,NCC_LEAD_GR			, 0, 1, 0), /* prior V2.1.0 	*/
  #endif
	MAKE_ENUM(LEAD_GI			,0x00AD 			,NCC_LEAD_GI			, 0, 1,32), /* LEAGDI			*/
	MAKE_ENUM(LEAO_IN			,0x00AE 			,NCC_LEAO_IN			, 0, 2,32), /* LEAINSTOID		*/
	MAKE_ENUM(LEAO_ID			,0x00AF 			,NCC_LEAO_ID			, 0, 2,32), /* LEAINSTO 		*/
/* -0x00Bx ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(PSHV_IN_16		,0x00B0 			,NCC_PSHV_IN_16 		, 1, 1,16), /* PUSHVID16		*/
	MAKE_ENUM(PSHV_ID_16		,0x00B1 			,NCC_PSHV_ID_16 		, 1, 1,16), /* PUSHV16			*/
  #ifndef OPC_INTERPRETER	
	MAKE_ENUM(LEAD_GO			,0x00B2 			,NCC_LEAD_GO			, 0, 1, 0), /* prior V2.06		*/
  #endif
	MAKE_ENUM(LEAO_ID_1 		,0x00B3 			,NCC_LEAO_ID_1			, 0, 2,64), /* LEAINSTO1		*/
	MAKE_ENUM(LEAO_IN_1 		,0x00B4 			,NCC_LEAO_IN_1			, 0, 2,64), /* LEAINSTOID1		*/
	MAKE_ENUM(POPV_IN_16		,0x00B5 			,NCC_POPV_IN_16 		, 8, 1,16), /* POPVID16 		*/
	MAKE_ENUM(POPV_ID_16		,0x00B6 			,NCC_POPV_ID_16 		, 8, 1,16), /* POPV16			*/
	MAKE_ENUM(LEAV_TX			,0x00B7 			,NCC_LEAV_TX			, 2, 0, 0), /* LEASTRVS 		*/
	MAKE_ENUM(LEAV_SS			,0x00B8 			,NCC_LEAV_SS			, 2, 0, 0), /* LEAINSTVS		*/
/*	MAKE_ENUM(					,0x00B9 			,						, 0, 0, 0), 					*/
	MAKE_ENUM(LEAO_XN			,0x00BA 			,NCC_LEAO_XN			, 0, 1,32), /* LEAINSTID		*/
	MAKE_ENUM(LEAO_XD			,0x00BB 			,NCC_LEAO_XD			, 0, 1,32), /* LEAINST			*/
	MAKE_ENUM(LEAI_ID			,0x00BC 			,NCC_LEAI_ID			, 1, 1,32), /* LEAINSTS 		*/
	MAKE_ENUM(LEAV_IN			,0x00BD 			,NCC_LEAV_IN			, 7, 1, 0), /* LEAVID			*/
	MAKE_ENUM(LEAV_ID			,0x00BE 			,NCC_LEAV_ID			, 7, 1, 0), /* LEAV 			*/
	MAKE_ENUM(LEAO_ST_1 		,0x00BF 			,NCC_LEAO_ST_1			, 1, 1,64), /* LEAINDOS1		*/
/* -0x00Cx ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(PSHD_IN_32		,0x00C0 			,NCC_PSHD_IN_32 		, 0, 1,32), /* PUSHID32 		*/
	MAKE_ENUM(PSHD_GR_32		,0x00C1 			,NCC_PSHD_GR_32 		, 0, 1,32), /* PUSHGDR32		*/
	MAKE_ENUM(PSHD_GI_32		,0x00C2 			,NCC_PSHD_GI_32 		, 0, 1,32), /* PUSHGDI32		*/
	MAKE_ENUM(PSHD_GO_32		,0x00C3 			,NCC_PSHD_GO_32 		, 0, 1,32), /* PUSHGDO32		*/
	MAKE_ENUM(PSHD_GM_32		,0x00C4 			,NCC_PSHD_GM_32 		, 0, 1,32), /* PUSHGDM32		*/
	MAKE_ENUM(POPD_IN_32		,0x00C5 			,NCC_POPD_IN_32 		, 3, 1,32), /* POPID32			*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(POPD_GR_32		,0x00C6 			,NCC_POPD_GR_32 		, 3, 1,32), /* prior V2.1.0 	*/
  #endif
	MAKE_ENUM(POPD_GI_32		,0x00C7 			,NCC_POPD_GI_32 		, 3, 1,32), /* POPGDI32 		*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(POPD_GO_32		,0x00C8 			,NCC_POPD_GO_32 		, 3, 1,32), /* prior V2.06		*/
  #endif
	MAKE_ENUM(POPD_GM_32		,0x00C9 			,NCC_POPD_GM_32 		, 3, 1,32), /* POPGDM32 		*/
	MAKE_ENUM(LEAV_ST			,0x00CA 			,NCC_LEAV_ST			, 0, 1, 0), /*					*/
	MAKE_ENUM(LEAO_ST			,0x00CB 			,NCC_LEAO_ST			, 1, 1,32), /* LEAINDOS 		*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(CALF				,0x00CC 			,NCC_CALF				, 9, 2, 0), /* prior V2.06		*/ 
  #endif
	MAKE_ENUM(CALF_PR			,0x00CD 			,NCC_CALF_PR			, 9, 3, 0), /* CALFX			*/
	MAKE_ENUM(CALB_PN			,0x00CE 			,NCC_CALB_PN			, 4, 2, 0), /* CALFBID			*/
	MAKE_ENUM(CALB_PI			,0x00CF 			,NCC_CALB_PI			, 4, 2, 0), /* CALFB			*/
/* -0x00Dx ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(PSHV_IN_32		,0x00D0 			,NCC_PSHV_IN_32 		, 1, 1,32), /* PUSHVID32		*/
	MAKE_ENUM(PSHV_ID_32		,0x00D1 			,NCC_PSHV_ID_32 		, 1, 1,32), /* PUSHV32			*/
/*	MAKE_ENUM(					,0x00D2 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x00D3 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x00D4 			,						, 0, 0, 0), 					*/
	MAKE_ENUM(POPV_IN_32		,0x00D5 			,NCC_POPV_IN_32 		, 8, 1,32), /* POPVID32 		*/
	MAKE_ENUM(POPV_ID_32		,0x00D6 			,NCC_POPV_ID_32 		, 8, 1,32), /* POPV32			*/
/*	MAKE_ENUM(					,0x00D7 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x00D8 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x00D9 			,						, 0, 0, 0), 					*/
	MAKE_ENUM(MCPY				,0x00DA 			,NCC_MCPY				, 8, 1, 0), /* MEMCPY			*/
/*	MAKE_ENUM(					,0x00DB 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x00DC 			,						, 0, 0, 0), 					*/
	MAKE_ENUM(PSHX____16		,0x00DD 			,NCC_PSHX____16 		, 1, 1,16), /* PUSHIDXID		*/
/*	MAKE_ENUM(					,0x00DE 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x00DF 			,						, 0, 0, 0), 					*/
/* -0x00Ex ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(PSHV_ST_1 		,0x00E0 			,NCC_PSHV_ST_1			, 2, 0, 1), /* PUSHINDVS1		*/
	MAKE_ENUM(POPV_ST_1 		,0x00E1 			,NCC_POPV_ST_1			, 7, 0, 1), /* POPINDVS1		*/
	MAKE_ENUM(PSHV_ST_8 		,0x00E2 			,NCC_PSHV_ST_8			, 2, 0, 8), /* PUSHINDVS8		*/
	MAKE_ENUM(POPV_ST_8 		,0x00E3 			,NCC_POPV_ST_8			, 7, 0, 8), /* POPINDVS8		*/
	MAKE_ENUM(PSHV_ST_16		,0x00E4 			,NCC_PSHV_ST_16 		, 2, 0,16), /* PUSHINDVS16		*/
	MAKE_ENUM(POPV_ST_16		,0x00E5 			,NCC_POPV_ST_16 		, 7, 0,16), /* POPINDVS16		*/
	MAKE_ENUM(PSHV_ST_32		,0x00E6 			,NCC_PSHV_ST_32 		, 2, 0,32), /* PUSHINDVS32		*/
	MAKE_ENUM(POPV_ST_32		,0x00E7 			,NCC_POPV_ST_32 		, 7, 0,32), /* POPINDVS32		*/
	MAKE_ENUM(PSHV_ST_64		,0x00E8 			,NCC_PSHV_ST_64 		, 2, 0,64), /* PUSHINDVS64		*/
	MAKE_ENUM(POPV_ST_64		,0x00E9 			,NCC_POPV_ST_64 		, 7, 0,64), /* POPINDVS64		*/
	MAKE_ENUM(PSHC_TX			,0x00EA 			,NCC_PSHC_TX			, 0, 0, 0), /* PUSHISTR 		*/ 
/*	MAKE_ENUM(					,0x00EB 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x00EC 			,						, 0, 0, 0), 					*/
	MAKE_ENUM(POPX____16		,0x00ED 			,NCC_POPX____16 		, 8, 1,16), /* POPIDXID16		*/
/*	MAKE_ENUM(					,0x00EE 			,						, 0, 0, 0), 					*/
	MAKE_ENUM(CALB_CI			,0x00EF 			,NCC_CALB_CI			, 4, 2, 0), /* CALFBM			*/
/* -0x00Fx ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(PSHD_IN_64		,0x00F0 			,NCC_PSHD_IN_64 		, 0, 1,64), /* PUSHID64 		*/
	MAKE_ENUM(PSHD_GR_64		,0x00F1 			,NCC_PSHD_GR_64 		, 0, 1,64), /* PUSHGDR64		*/
	MAKE_ENUM(PSHD_GI_64		,0x00F2 			,NCC_PSHD_GI_64 		, 0, 1,64), /* PUSHGDI64		*/
	MAKE_ENUM(PSHD_GO_64		,0x00F3 			,NCC_PSHD_GO_64 		, 0, 1,64), /* PUSHGDO64		*/
	MAKE_ENUM(PSHD_GM_64		,0x00F4 			,NCC_PSHD_GM_64 		, 0, 1,64), /* PUSHGDM64		*/
	MAKE_ENUM(POPD_IN_64		,0x00F5 			,NCC_POPD_IN_64 		, 3, 1,64), /* POPID64			*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(POPD_GR_64		,0x00F6 			,NCC_POPD_GR_64 		, 3, 1,64), /* prior V2.1.0 	*/
  #endif
	MAKE_ENUM(POPD_GI_64		,0x00F7 			,NCC_POPD_GI_64 		, 3, 1,64), /* POPGDI64 		*/
  #ifndef OPC_INTERPRETER
	MAKE_ENUM(POPD_GO_64		,0x00F8 			,NCC_POPD_GO_64 		, 3, 1,64), /* prior V2.06		*/
  #endif
	MAKE_ENUM(POPD_GM_64		,0x00F9 			,NCC_POPD_GM_64 		, 3, 1,64), /* POPGDM64 		*/
	MAKE_ENUM(PSHV_IN_64		,0x00FA 			,NCC_PSHV_IN_64 		, 1, 1,64), /* PUSHVID64		*/
	MAKE_ENUM(PSHV_ID_64		,0x00FB 			,NCC_PSHV_ID_64 		, 1, 1,64), /* PUSHV64			*/
	MAKE_ENUM(POPV_IN_64		,0x00FC 			,NCC_POPV_IN_64 		, 8, 1,64), /* POPVID64 		*/	
	MAKE_ENUM(POPV_ID_64		,0x00FD 			,NCC_POPV_ID_64 		, 8, 1,64), /* POPV64			*/	
/*	MAKE_ENUM(					,0x00FE 			,						, 0, 0, 0), 					*/
/*	MAKE_ENUM(					,0x00FF 			,						, 0, 0, 0), 					*/


/* -------- C O N V E R S A T I O N   T A B L E  ---------------------------------------------------------- */

/* -0x000x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(BOOL_TO_USINT 	,CONVCODE(0x00) 	,NCC_BOOL_TO_USINT		, 1, 0, 8), 
	MAKE_ENUM(BOOL_TO_BYTE		,CONVCODE(0x00) 	,NCC_BOOL_TO_BYTE		, 1, 0, 8), 
	MAKE_ENUM(BOOL_TO_SINT		,CONVCODE(0x01) 	,NCC_BOOL_TO_SINT		, 1, 0, 8), 
	MAKE_ENUM(BOOL_TO_UINT		,CONVCODE(0x02) 	,NCC_BOOL_TO_UINT		, 1, 0,16), 
	MAKE_ENUM(BOOL_TO_WORD		,CONVCODE(0x02) 	,NCC_BOOL_TO_WORD		, 1, 0,16), 
	MAKE_ENUM(BOOL_TO_INT		,CONVCODE(0x03) 	,NCC_BOOL_TO_INT		, 1, 0,16), 
	MAKE_ENUM(BOOL_TO_UDINT 	,CONVCODE(0x04) 	,NCC_BOOL_TO_UDINT		, 1, 0,32), 
	MAKE_ENUM(BOOL_TO_DWORD 	,CONVCODE(0x04) 	,NCC_BOOL_TO_DWORD		, 1, 0,32), 
	MAKE_ENUM(BOOL_TO_DINT		,CONVCODE(0x05) 	,NCC_BOOL_TO_DINT		, 1, 0,32), 
	MAKE_ENUM(BOOL_TO_REAL		,CONVCODE(0x06) 	,NCC_BOOL_TO_REAL		, 1, 0,32), 
/*	MAKE_ENUM(					,CONVCODE(0x07) 	,						, 0, 0, 0), */
	MAKE_ENUM(BOOL_TO_ULINT 	,CONVCODE(0x08) 	,NCC_BOOL_TO_ULINT		, 1, 0,64), 
	MAKE_ENUM(BOOL_TO_LWORD 	,CONVCODE(0x08) 	,NCC_BOOL_TO_LWORD		, 1, 0,64), 
	MAKE_ENUM(BOOL_TO_LINT		,CONVCODE(0x09) 	,NCC_BOOL_TO_LINT		, 1, 0,64), 
	MAKE_ENUM(BOOL_TO_LREAL 	,CONVCODE(0x0A) 	,NCC_BOOL_TO_LREAL		, 1, 0,64), 
	MAKE_ENUM(USINT_TO_BOOL 	,CONVCODE(0x0B) 	,NCC_USINT_TO_BOOL		, 1, 0, 1), 
	MAKE_ENUM(BYTE_TO_BOOL		,CONVCODE(0x0B) 	,NCC_BYTE_TO_BOOL		, 1, 0, 1), 
	MAKE_ENUM(USINT_TO_SINT 	,CONVCODE(0x0C) 	,NCC_USINT_TO_SINT		, 1, 0, 8), 
	MAKE_ENUM(BYTE_TO_SINT		,CONVCODE(0x0C) 	,NCC_BYTE_TO_SINT		, 1, 0, 8), 
	MAKE_ENUM(USINT_TO_BYTE 	,CONVCODE(0x0D) 	,NCC_USINT_TO_BYTE		, 1, 0, 8), 
	MAKE_ENUM(USINT_TO_UINT 	,CONVCODE(0x0E) 	,NCC_USINT_TO_UINT		, 1, 0,16), 
	MAKE_ENUM(BYTE_TO_UINT		,CONVCODE(0x0E) 	,NCC_BYTE_TO_UINT		, 1, 0,16), 
	MAKE_ENUM(USINT_TO_WORD 	,CONVCODE(0x0E) 	,NCC_USINT_TO_WORD		, 1, 0,16), 
	MAKE_ENUM(BYTE_TO_WORD		,CONVCODE(0x0E) 	,NCC_BYTE_TO_WORD		, 1, 0,16), 
	MAKE_ENUM(USINT_TO_INT		,CONVCODE(0x0F) 	,NCC_USINT_TO_INT		, 1, 0,16), 
	MAKE_ENUM(BYTE_TO_INT		,CONVCODE(0x0F) 	,NCC_BYTE_TO_INT		, 1, 0,16), 
/* -0x001x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(USINT_TO_UDINT	,CONVCODE(0x10) 	,NCC_USINT_TO_UDINT 	, 1, 0,32),
	MAKE_ENUM(BYTE_TO_UDINT 	,CONVCODE(0x10) 	,NCC_BYTE_TO_UDINT		, 1, 0,32),
	MAKE_ENUM(USINT_TO_DWORD	,CONVCODE(0x10) 	,NCC_USINT_TO_DWORD 	, 1, 0,32),
	MAKE_ENUM(BYTE_TO_DWORD 	,CONVCODE(0x10) 	,NCC_BYTE_TO_DWORD		, 1, 0,32),
	MAKE_ENUM(USINT_TO_DINT 	,CONVCODE(0x11) 	,NCC_USINT_TO_DINT		, 1, 0,32),
	MAKE_ENUM(BYTE_TO_DINT		,CONVCODE(0x11) 	,NCC_BYTE_TO_DINT		, 1, 0,32),
	MAKE_ENUM(USINT_TO_REAL 	,CONVCODE(0x12) 	,NCC_USINT_TO_REAL		, 1, 0,32),
	MAKE_ENUM(BYTE_TO_REAL		,CONVCODE(0x12) 	,NCC_BYTE_TO_REAL		, 1, 0,32),
/*	MAKE_ENUM(					,CONVCODE(0x13) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x13) 	,						, 0, 0, 0), */
	MAKE_ENUM(USINT_TO_ULINT	,CONVCODE(0x14) 	,NCC_USINT_TO_ULINT 	, 1, 0,64),
	MAKE_ENUM(BYTE_TO_ULINT 	,CONVCODE(0x14) 	,NCC_BYTE_TO_ULINT		, 1, 0,64),
	MAKE_ENUM(USINT_TO_LWORD	,CONVCODE(0x14) 	,NCC_USINT_TO_LWORD 	, 1, 0,64),
	MAKE_ENUM(BYTE_TO_LWORD 	,CONVCODE(0x14) 	,NCC_BYTE_TO_LWORD		, 1, 0,64),
	MAKE_ENUM(USINT_TO_LINT 	,CONVCODE(0x15) 	,NCC_USINT_TO_LINT		, 1, 0,64),
	MAKE_ENUM(BYTE_TO_LINT		,CONVCODE(0x15) 	,NCC_BYTE_TO_LINT		, 1, 0,64),
	MAKE_ENUM(USINT_TO_LREAL	,CONVCODE(0x16) 	,NCC_USINT_TO_LREAL 	, 1, 0,64),
	MAKE_ENUM(BYTE_TO_LREAL 	,CONVCODE(0x16) 	,NCC_BYTE_TO_LREAL		, 1, 0,64),
	MAKE_ENUM(SINT_TO_BOOL		,CONVCODE(0x17) 	,NCC_SINT_TO_BOOL		, 1, 0, 1),
	MAKE_ENUM(SINT_TO_USINT 	,CONVCODE(0x18) 	,NCC_SINT_TO_USINT		, 1, 0, 8),
	MAKE_ENUM(SINT_TO_BYTE		,CONVCODE(0x18) 	,NCC_SINT_TO_BYTE		, 1, 0, 8),
	MAKE_ENUM(SINT_TO_UINT		,CONVCODE(0x19) 	,NCC_SINT_TO_UINT		, 1, 0,16),
	MAKE_ENUM(SINT_TO_WORD		,CONVCODE(0x19) 	,NCC_SINT_TO_WORD		, 1, 0,16),
	MAKE_ENUM(SINT_TO_INT		,CONVCODE(0x1A) 	,NCC_SINT_TO_INT		, 1, 0,16),
	MAKE_ENUM(SINT_TO_UDINT 	,CONVCODE(0x1B) 	,NCC_SINT_TO_UDINT		, 1, 0,32),
	MAKE_ENUM(SINT_TO_DWORD 	,CONVCODE(0x1B) 	,NCC_SINT_TO_DWORD		, 1, 0,32),
	MAKE_ENUM(SINT_TO_DINT		,CONVCODE(0x1C) 	,NCC_SINT_TO_DINT		, 1, 0,32),
	MAKE_ENUM(SINT_TO_REAL		,CONVCODE(0x1D) 	,NCC_SINT_TO_REAL		, 1, 0,32),
/*	MAKE_ENUM(					,CONVCODE(0x1E) 	,						, 0, 0, 0), */
	MAKE_ENUM(SINT_TO_ULINT 	,CONVCODE(0x1F) 	,NCC_SINT_TO_ULINT		, 1, 0,64),
	MAKE_ENUM(SINT_TO_LWORD 	,CONVCODE(0x1F) 	,NCC_SINT_TO_LWORD		, 1, 0,64),
/* -0x002x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(SINT_TO_LINT		,CONVCODE(0x20) 	,NCC_SINT_TO_LINT		, 1, 0,64),
	MAKE_ENUM(SINT_TO_LREAL 	,CONVCODE(0x21) 	,NCC_SINT_TO_LREAL		, 1, 0,64),
	MAKE_ENUM(BYTE_TO_USINT 	,CONVCODE(0x22) 	,NCC_BYTE_TO_USINT		, 1, 0, 8),
	MAKE_ENUM(UINT_TO_BOOL		,CONVCODE(0x23) 	,NCC_UINT_TO_BOOL		, 1, 0, 1),
	MAKE_ENUM(WORD_TO_BOOL		,CONVCODE(0x23) 	,NCC_WORD_TO_BOOL		, 1, 0, 1),
	MAKE_ENUM(UINT_TO_USINT 	,CONVCODE(0x24) 	,NCC_UINT_TO_USINT		, 1, 0, 8),
	MAKE_ENUM(WORD_TO_USINT 	,CONVCODE(0x24) 	,NCC_WORD_TO_USINT		, 1, 0, 8),
	MAKE_ENUM(UINT_TO_BYTE		,CONVCODE(0x24) 	,NCC_UINT_TO_BYTE		, 1, 0, 8),
	MAKE_ENUM(WORD_TO_BYTE		,CONVCODE(0x24) 	,NCC_WORD_TO_BYTE		, 1, 0, 8),
	MAKE_ENUM(UINT_TO_SINT		,CONVCODE(0x25) 	,NCC_UINT_TO_SINT		, 1, 0, 8),
	MAKE_ENUM(WORD_TO_SINT		,CONVCODE(0x25) 	,NCC_WORD_TO_SINT		, 1, 0, 8),
	MAKE_ENUM(UINT_TO_INT		,CONVCODE(0x26) 	,NCC_UINT_TO_INT		, 1, 0,16),
	MAKE_ENUM(WORD_TO_INT		,CONVCODE(0x26) 	,NCC_WORD_TO_INT		, 1, 0,16),
	MAKE_ENUM(UINT_TO_WORD		,CONVCODE(0x27) 	,NCC_UINT_TO_WORD		, 1, 0,16),
	MAKE_ENUM(UINT_TO_UDINT 	,CONVCODE(0x28) 	,NCC_UINT_TO_UDINT		, 1, 0,32),
	MAKE_ENUM(WORD_TO_UDINT 	,CONVCODE(0x28) 	,NCC_WORD_TO_UDINT		, 1, 0,32),
	MAKE_ENUM(UINT_TO_DWORD 	,CONVCODE(0x28) 	,NCC_UINT_TO_DWORD		, 1, 0,32),
	MAKE_ENUM(WORD_TO_DWORD 	,CONVCODE(0x28) 	,NCC_WORD_TO_DWORD		, 1, 0,32),
	MAKE_ENUM(UINT_TO_DINT		,CONVCODE(0x29) 	,NCC_UINT_TO_DINT		, 1, 0,32),
	MAKE_ENUM(WORD_TO_DINT		,CONVCODE(0x29) 	,NCC_WORD_TO_DINT		, 1, 0,32),
	MAKE_ENUM(UINT_TO_REAL		,CONVCODE(0x2A) 	,NCC_UINT_TO_REAL		, 1, 0,32),
	MAKE_ENUM(WORD_TO_REAL		,CONVCODE(0x2A) 	,NCC_WORD_TO_REAL		, 1, 0,32),
/*	MAKE_ENUM(					,CONVCODE(0x2B) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x2B) 	,						, 0, 0, 0), */
	MAKE_ENUM(UINT_TO_ULINT 	,CONVCODE(0x2C) 	,NCC_UINT_TO_ULINT		, 1, 0,64),
	MAKE_ENUM(WORD_TO_ULINT 	,CONVCODE(0x2C) 	,NCC_WORD_TO_ULINT		, 1, 0,64),
	MAKE_ENUM(UINT_TO_LWORD 	,CONVCODE(0x2C) 	,NCC_UINT_TO_LWORD		, 1, 0,64),
	MAKE_ENUM(WORD_TO_LWORD 	,CONVCODE(0x2C) 	,NCC_WORD_TO_LWORD		, 1, 0,64),
	MAKE_ENUM(UINT_TO_LINT		,CONVCODE(0x2D) 	,NCC_UINT_TO_LINT		, 1, 0,64),
	MAKE_ENUM(WORD_TO_LINT		,CONVCODE(0x2D) 	,NCC_WORD_TO_LINT		, 1, 0,64),
	MAKE_ENUM(UINT_TO_LREAL 	,CONVCODE(0x2E) 	,NCC_UINT_TO_LREAL		, 1, 0,64),
	MAKE_ENUM(WORD_TO_LREAL 	,CONVCODE(0x2E) 	,NCC_WORD_TO_LREAL		, 1, 0,64),
	MAKE_ENUM(INT_TO_BOOL		,CONVCODE(0x2F) 	,NCC_INT_TO_BOOL		, 1, 0, 1),
/* -0x003x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(INT_TO_USINT		,CONVCODE(0x30) 	,NCC_INT_TO_USINT		, 1, 0, 8),
	MAKE_ENUM(INT_TO_BYTE		,CONVCODE(0x30) 	,NCC_INT_TO_BYTE		, 1, 0, 8),
	MAKE_ENUM(INT_TO_SINT		,CONVCODE(0x31) 	,NCC_INT_TO_SINT		, 1, 0, 8),
	MAKE_ENUM(INT_TO_UINT		,CONVCODE(0x32) 	,NCC_INT_TO_UINT		, 1, 0,16),
	MAKE_ENUM(INT_TO_WORD		,CONVCODE(0x32) 	,NCC_INT_TO_WORD		, 1, 0,16),
	MAKE_ENUM(INT_TO_UDINT		,CONVCODE(0x33) 	,NCC_INT_TO_UDINT		, 1, 0,32),
	MAKE_ENUM(INT_TO_DWORD		,CONVCODE(0x33) 	,NCC_INT_TO_DWORD		, 1, 0,32),
	MAKE_ENUM(INT_TO_DINT		,CONVCODE(0x34) 	,NCC_INT_TO_DINT		, 1, 0,32),
	MAKE_ENUM(INT_TO_REAL		,CONVCODE(0x35) 	,NCC_INT_TO_REAL		, 1, 0,32),
/*	MAKE_ENUM(					,CONVCODE(0x36) 	,						, 0, 0, 0), */
	MAKE_ENUM(INT_TO_LINT		,CONVCODE(0x37) 	,NCC_INT_TO_LINT		, 1, 0,64),
	MAKE_ENUM(INT_TO_ULINT		,CONVCODE(0x38) 	,NCC_INT_TO_ULINT		, 1, 0,64),
	MAKE_ENUM(INT_TO_LWORD		,CONVCODE(0x38) 	,NCC_INT_TO_LWORD		, 1, 0,64),
	MAKE_ENUM(INT_TO_LREAL		,CONVCODE(0x39) 	,NCC_INT_TO_LREAL		, 1, 0,64),
	MAKE_ENUM(WORD_TO_UINT		,CONVCODE(0x3A) 	,NCC_WORD_TO_UINT		, 1, 0,16),
	MAKE_ENUM(UDINT_TO_BOOL 	,CONVCODE(0x3B) 	,NCC_UDINT_TO_BOOL		, 1, 0, 1),
	MAKE_ENUM(DWORD_TO_BOOL 	,CONVCODE(0x3B) 	,NCC_DWORD_TO_BOOL		, 1, 0, 1),
	MAKE_ENUM(UDINT_TO_USINT	,CONVCODE(0x3C) 	,NCC_UDINT_TO_USINT 	, 1, 0, 8),
	MAKE_ENUM(DWORD_TO_USINT	,CONVCODE(0x3C) 	,NCC_DWORD_TO_USINT 	, 1, 0, 8),
	MAKE_ENUM(UDINT_TO_BYTE 	,CONVCODE(0x3C) 	,NCC_UDINT_TO_BYTE		, 1, 0, 8),
	MAKE_ENUM(DWORD_TO_BYTE 	,CONVCODE(0x3C) 	,NCC_DWORD_TO_BYTE		, 1, 0, 8),
	MAKE_ENUM(UDINT_TO_SINT 	,CONVCODE(0x3D) 	,NCC_UDINT_TO_SINT		, 1, 0, 8),
	MAKE_ENUM(DWORD_TO_SINT 	,CONVCODE(0x3D) 	,NCC_DWORD_TO_SINT		, 1, 0, 8),
	MAKE_ENUM(UDINT_TO_UINT 	,CONVCODE(0x3E) 	,NCC_UDINT_TO_UINT		, 1, 0,16),
	MAKE_ENUM(DWORD_TO_UINT 	,CONVCODE(0x3E) 	,NCC_DWORD_TO_UINT		, 1, 0,16),
	MAKE_ENUM(UDINT_TO_WORD 	,CONVCODE(0x3E) 	,NCC_UDINT_TO_WORD		, 1, 0,16),
	MAKE_ENUM(UDINT_TO_INT		,CONVCODE(0x3F) 	,NCC_UDINT_TO_INT		, 1, 0,16),
	MAKE_ENUM(DWORD_TO_INT		,CONVCODE(0x3F) 	,NCC_DWORD_TO_INT		, 1, 0,16),
/* -0x004x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(UDINT_TO_DINT 	,CONVCODE(0x40) 	,NCC_UDINT_TO_DINT		, 1, 0,32),
	MAKE_ENUM(DWORD_TO_DINT 	,CONVCODE(0x40) 	,NCC_DWORD_TO_DINT		, 1, 0,32),
	MAKE_ENUM(UDINT_TO_DWORD	,CONVCODE(0x40) 	,NCC_UDINT_TO_DWORD 	, 1, 0,32),
	MAKE_ENUM(UDINT_TO_REAL 	,CONVCODE(0x41) 	,NCC_UDINT_TO_REAL		, 1, 0,32),
	MAKE_ENUM(DWORD_TO_REAL 	,CONVCODE(0x41) 	,NCC_DWORD_TO_REAL		, 1, 0,32),
/*	MAKE_ENUM(					,CONVCODE(0x42) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x42) 	,						, 0, 0, 0), */
	MAKE_ENUM(UDINT_TO_ULINT	,CONVCODE(0x43) 	,NCC_UDINT_TO_ULINT 	, 1, 0,64),
	MAKE_ENUM(DWORD_TO_ULINT	,CONVCODE(0x43) 	,NCC_DWORD_TO_ULINT 	, 1, 0,64),
	MAKE_ENUM(UDINT_TO_LWORD	,CONVCODE(0x43) 	,NCC_UDINT_TO_LWORD 	, 1, 0,64),
	MAKE_ENUM(DWORD_TO_LWORD	,CONVCODE(0x43) 	,NCC_DWORD_TO_LWORD 	, 1, 0,64),
	MAKE_ENUM(UDINT_TO_LINT 	,CONVCODE(0x44) 	,NCC_UDINT_TO_LINT		, 1, 0,64),
	MAKE_ENUM(DWORD_TO_LINT 	,CONVCODE(0x44) 	,NCC_DWORD_TO_LINT		, 1, 0,64),
	MAKE_ENUM(UDINT_TO_LREAL	,CONVCODE(0x45) 	,NCC_UDINT_TO_LREAL 	, 1, 0,64),
	MAKE_ENUM(DWORD_TO_LREAL	,CONVCODE(0x45) 	,NCC_DWORD_TO_LREAL 	, 1, 0,64),
	MAKE_ENUM(DINT_TO_BOOL		,CONVCODE(0x46) 	,NCC_DINT_TO_BOOL		, 1, 0, 1),
	MAKE_ENUM(DINT_TO_USINT 	,CONVCODE(0x47) 	,NCC_DINT_TO_USINT		, 1, 0, 8),
	MAKE_ENUM(DINT_TO_BYTE		,CONVCODE(0x47) 	,NCC_DINT_TO_BYTE		, 1, 0, 8),
	MAKE_ENUM(DINT_TO_SINT		,CONVCODE(0x48) 	,NCC_DINT_TO_SINT		, 1, 0, 8),
	MAKE_ENUM(DINT_TO_UINT		,CONVCODE(0x49) 	,NCC_DINT_TO_UINT		, 1, 0,16),
	MAKE_ENUM(DINT_TO_WORD		,CONVCODE(0x49) 	,NCC_DINT_TO_WORD		, 1, 0,16),
	MAKE_ENUM(DINT_TO_INT		,CONVCODE(0x4A) 	,NCC_DINT_TO_INT		, 1, 0,16),
	MAKE_ENUM(DINT_TO_UDINT 	,CONVCODE(0x4B) 	,NCC_DINT_TO_UDINT		, 1, 0,32),
	MAKE_ENUM(DINT_TO_DWORD 	,CONVCODE(0x4B) 	,NCC_DINT_TO_DWORD		, 1, 0,32),
	MAKE_ENUM(DINT_TO_REAL		,CONVCODE(0x4C) 	,NCC_DINT_TO_REAL		, 1, 0,32),
/*	MAKE_ENUM(					,CONVCODE(0x4D) 	,						, 0, 0, 0), */
	MAKE_ENUM(DINT_TO_ULINT 	,CONVCODE(0x4E) 	,NCC_DINT_TO_ULINT		, 1, 0,64),
	MAKE_ENUM(DINT_TO_LWORD 	,CONVCODE(0x4E) 	,NCC_DINT_TO_LWORD		, 1, 0,64),
	MAKE_ENUM(DINT_TO_LINT		,CONVCODE(0x4F) 	,NCC_DINT_TO_LINT		, 1, 0,64),
/* -0x005x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(DINT_TO_LREAL 	,CONVCODE(0x50) 	,NCC_DINT_TO_LREAL		, 1, 0,64),
	MAKE_ENUM(DWORD_TO_UDINT	,CONVCODE(0x51) 	,NCC_DWORD_TO_UDINT 	, 1, 0,32),
	MAKE_ENUM(REAL_TO_BOOL		,CONVCODE(0x52) 	,NCC_REAL_TO_BOOL		, 1, 0, 1),
	MAKE_ENUM(REAL_TO_USINT 	,CONVCODE(0x53) 	,NCC_REAL_TO_USINT		, 1, 0, 8),
	MAKE_ENUM(REAL_TO_BYTE		,CONVCODE(0x53) 	,NCC_REAL_TO_BYTE		, 1, 0, 8),
	MAKE_ENUM(REAL_TO_SINT		,CONVCODE(0x54) 	,NCC_REAL_TO_SINT		, 1, 0, 8),
	MAKE_ENUM(REAL_TO_UINT		,CONVCODE(0x55) 	,NCC_REAL_TO_UINT		, 1, 0,16),
	MAKE_ENUM(REAL_TO_WORD		,CONVCODE(0x55) 	,NCC_REAL_TO_WORD		, 1, 0,16),
	MAKE_ENUM(REAL_TO_INT		,CONVCODE(0x56) 	,NCC_REAL_TO_INT		, 1, 0,16),
	MAKE_ENUM(REAL_TO_UDINT 	,CONVCODE(0x57) 	,NCC_REAL_TO_UDINT		, 1, 0,32),
	MAKE_ENUM(REAL_TO_DWORD 	,CONVCODE(0x57) 	,NCC_REAL_TO_DWORD		, 1, 0,32),
	MAKE_ENUM(REAL_TO_DINT		,CONVCODE(0x58) 	,NCC_REAL_TO_DINT		, 1, 0,32),
/*	MAKE_ENUM(					,CONVCODE(0x59) 	,						, 1, 0, 0), */
	MAKE_ENUM(REAL_TO_ULINT 	,CONVCODE(0x5A) 	,NCC_REAL_TO_ULINT		, 1, 0,64),
	MAKE_ENUM(REAL_TO_LWORD 	,CONVCODE(0x5A) 	,NCC_REAL_TO_LWORD		, 1, 0,64),
	MAKE_ENUM(REAL_TO_LINT		,CONVCODE(0x5B) 	,NCC_REAL_TO_LINT		, 1, 0,64),
	MAKE_ENUM(REAL_TO_LREAL 	,CONVCODE(0x5C) 	,NCC_REAL_TO_LREAL		, 1, 0,64),
/*	MAKE_ENUM(					,CONVCODE(0x5D) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x5E) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x5F) 	,						, 0, 0, 0), */
/* -0x006x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,CONVCODE(0x60) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x61) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x62) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x63) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x64) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x65) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x66) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x67) 	,						, 0, 0, 0), */
	MAKE_ENUM(ULINT_TO_BOOL 	,CONVCODE(0x68) 	,NCC_ULINT_TO_BOOL		, 1, 0, 1),
	MAKE_ENUM(LWORD_TO_BOOL 	,CONVCODE(0x68) 	,NCC_LWORD_TO_BOOL		, 1, 0, 1),
	MAKE_ENUM(ULINT_TO_USINT	,CONVCODE(0x69) 	,NCC_ULINT_TO_USINT 	, 1, 0, 8),
	MAKE_ENUM(LWORD_TO_USINT	,CONVCODE(0x69) 	,NCC_LWORD_TO_USINT 	, 1, 0, 8),
	MAKE_ENUM(ULINT_TO_BYTE 	,CONVCODE(0x69) 	,NCC_ULINT_TO_BYTE		, 1, 0, 8),
	MAKE_ENUM(LWORD_TO_BYTE 	,CONVCODE(0x69) 	,NCC_LWORD_TO_BYTE		, 1, 0, 8),
	MAKE_ENUM(ULINT_TO_SINT 	,CONVCODE(0x6A) 	,NCC_ULINT_TO_SINT		, 1, 0, 8),
	MAKE_ENUM(LWORD_TO_SINT 	,CONVCODE(0x6A) 	,NCC_LWORD_TO_SINT		, 1, 0, 8),
	MAKE_ENUM(ULINT_TO_UINT 	,CONVCODE(0x6B) 	,NCC_ULINT_TO_UINT		, 1, 0,16),
	MAKE_ENUM(LWORD_TO_UINT 	,CONVCODE(0x6B) 	,NCC_LWORD_TO_UINT		, 1, 0,16),
	MAKE_ENUM(ULINT_TO_WORD 	,CONVCODE(0x6B) 	,NCC_ULINT_TO_WORD		, 1, 0,16),
	MAKE_ENUM(LWORD_TO_WORD 	,CONVCODE(0x6B) 	,NCC_LWORD_TO_WORD		, 1, 0,16),
	MAKE_ENUM(ULINT_TO_INT		,CONVCODE(0x6C) 	,NCC_ULINT_TO_INT		, 1, 0,16),
	MAKE_ENUM(LWORD_TO_INT		,CONVCODE(0x6C) 	,NCC_LWORD_TO_INT		, 1, 0,16),
	MAKE_ENUM(ULINT_TO_UDINT	,CONVCODE(0x6D) 	,NCC_ULINT_TO_UDINT 	, 1, 0,32),
	MAKE_ENUM(LWORD_TO_UDINT	,CONVCODE(0x6D) 	,NCC_LWORD_TO_UDINT 	, 1, 0,32),
	MAKE_ENUM(ULINT_TO_DWORD	,CONVCODE(0x6D) 	,NCC_ULINT_TO_DWORD 	, 1, 0,32),
	MAKE_ENUM(LWORD_TO_DWORD	,CONVCODE(0x6D) 	,NCC_LWORD_TO_DWORD 	, 1, 0,32),
	MAKE_ENUM(ULINT_TO_DINT 	,CONVCODE(0x6E) 	,NCC_ULINT_TO_DINT		, 1, 0,32),
	MAKE_ENUM(LWORD_TO_DINT 	,CONVCODE(0x6E) 	,NCC_LWORD_TO_DINT		, 1, 0,32),
	MAKE_ENUM(ULINT_TO_REAL 	,CONVCODE(0x6F) 	,NCC_ULINT_TO_REAL		, 1, 0,32),
	MAKE_ENUM(LWORD_TO_REAL 	,CONVCODE(0x6F) 	,NCC_LWORD_TO_REAL		, 1, 0,32),
/* -0x007x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,CONVCODE(0x70) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x70) 	,						, 0, 0, 0), */
	MAKE_ENUM(ULINT_TO_LINT 	,CONVCODE(0x71) 	,NCC_ULINT_TO_LINT		, 1, 0,64),
	MAKE_ENUM(LWORD_TO_LINT 	,CONVCODE(0x71) 	,NCC_LWORD_TO_LINT		, 1, 0,64),
	MAKE_ENUM(ULINT_TO_LWORD	,CONVCODE(0x72) 	,NCC_ULINT_TO_LWORD 	, 1, 0,64),
	MAKE_ENUM(ULINT_TO_LREAL	,CONVCODE(0x73) 	,NCC_ULINT_TO_LREAL 	, 1, 0,64),
	MAKE_ENUM(LWORD_TO_LREAL	,CONVCODE(0x73) 	,NCC_LWORD_TO_LREAL 	, 1, 0,64),
	MAKE_ENUM(LINT_TO_BOOL		,CONVCODE(0x74) 	,NCC_LINT_TO_BOOL		, 1, 0, 1),
	MAKE_ENUM(LINT_TO_USINT 	,CONVCODE(0x75) 	,NCC_LINT_TO_USINT		, 1, 0, 8),
	MAKE_ENUM(LINT_TO_BYTE		,CONVCODE(0x75) 	,NCC_LINT_TO_BYTE		, 1, 0, 8),
	MAKE_ENUM(LINT_TO_SINT		,CONVCODE(0x76) 	,NCC_LINT_TO_SINT		, 1, 0, 8),
	MAKE_ENUM(LINT_TO_UINT		,CONVCODE(0x77) 	,NCC_LINT_TO_UINT		, 1, 0, 8),
	MAKE_ENUM(LINT_TO_WORD		,CONVCODE(0x77) 	,NCC_LINT_TO_WORD		, 1, 0,16),
	MAKE_ENUM(LINT_TO_INT		,CONVCODE(0x78) 	,NCC_LINT_TO_INT		, 1, 0,16),
	MAKE_ENUM(LINT_TO_UDINT 	,CONVCODE(0x79) 	,NCC_LINT_TO_UDINT		, 1, 0,32),
	MAKE_ENUM(LINT_TO_DWORD 	,CONVCODE(0x79) 	,NCC_LINT_TO_DWORD		, 1, 0,32),
	MAKE_ENUM(LINT_TO_DINT		,CONVCODE(0x7A) 	,NCC_LINT_TO_DINT		, 1, 0,32),
	MAKE_ENUM(LINT_TO_REAL		,CONVCODE(0x7B) 	,NCC_LINT_TO_REAL		, 1, 0,32),
/*	MAKE_ENUM(					,CONVCODE(0x7C) 	,						, 0, 0, 0), */
	MAKE_ENUM(LINT_TO_ULINT 	,CONVCODE(0x7D) 	,NCC_LINT_TO_ULINT		, 1, 0,64),
	MAKE_ENUM(LINT_TO_LWORD 	,CONVCODE(0x7D) 	,NCC_LINT_TO_LWORD		, 1, 0,64),
	MAKE_ENUM(LINT_TO_LREAL 	,CONVCODE(0x7E) 	,NCC_LINT_TO_LREAL		, 1, 0,64),
	MAKE_ENUM(LWORD_TO_ULINT	,CONVCODE(0x7F) 	,NCC_LWORD_TO_ULINT 	, 1, 0,64),
/* -0x008x ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(LREAL_TO_BOOL 	,CONVCODE(0x80) 	,NCC_LREAL_TO_BOOL		, 1, 0, 1),
	MAKE_ENUM(LREAL_TO_USINT	,CONVCODE(0x81) 	,NCC_LREAL_TO_USINT 	, 1, 0, 8),
	MAKE_ENUM(LREAL_TO_BYTE 	,CONVCODE(0x81) 	,NCC_LREAL_TO_BYTE		, 1, 0, 8),
	MAKE_ENUM(LREAL_TO_SINT 	,CONVCODE(0x82) 	,NCC_LREAL_TO_SINT		, 1, 0, 8),
	MAKE_ENUM(LREAL_TO_UINT 	,CONVCODE(0x83) 	,NCC_LREAL_TO_UINT		, 1, 0,16),
	MAKE_ENUM(LREAL_TO_WORD 	,CONVCODE(0x83) 	,NCC_LREAL_TO_WORD		, 1, 0,16),
	MAKE_ENUM(LREAL_TO_INT		,CONVCODE(0x84) 	,NCC_LREAL_TO_INT		, 1, 0,16),
	MAKE_ENUM(LREAL_TO_UDINT	,CONVCODE(0x85) 	,NCC_LREAL_TO_UDINT 	, 1, 0,32),
	MAKE_ENUM(LREAL_TO_DWORD	,CONVCODE(0x85) 	,NCC_LREAL_TO_DWORD 	, 1, 0,32),
	MAKE_ENUM(LREAL_TO_DINT 	,CONVCODE(0x86) 	,NCC_LREAL_TO_DINT		, 1, 0,32),
	MAKE_ENUM(LREAL_TO_REAL 	,CONVCODE(0x87) 	,NCC_LREAL_TO_REAL		, 1, 0,32),
	MAKE_ENUM(LREAL_TO_ULINT	,CONVCODE(0x89) 	,NCC_LREAL_TO_ULINT 	, 1, 0,64),
	MAKE_ENUM(LREAL_TO_LWORD	,CONVCODE(0x89) 	,NCC_LREAL_TO_LWORD 	, 1, 0,64),
	MAKE_ENUM(LREAL_TO_LINT 	,CONVCODE(0x8A) 	,NCC_LREAL_TO_LINT		, 1, 0,64),
/*	MAKE_ENUM(					,CONVCODE(0x8B) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x8C) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x8D) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x8E) 	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,CONVCODE(0x8F) 	,						, 0, 0, 0), */


/* -------- A R I T H M E T I C   T A B L E  ------------------------------------------------------------- */

/* -0x000x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,ARITHCODE(0x00)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x01)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x02)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x03)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x04)	,						, 0, 0, 0), */
	MAKE_ENUM(GT_USINT			,ARITHCODE(0x05)	,NCC_GT_USINT			, 2, 0, 1),
	MAKE_ENUM(GT_BYTE			,ARITHCODE(0x05)	,NCC_GT_BYTE			, 2, 0, 1),
	MAKE_ENUM(GE_USINT			,ARITHCODE(0x06)	,NCC_GE_USINT			, 2, 0, 1),
	MAKE_ENUM(GE_BYTE			,ARITHCODE(0x06)	,NCC_GE_BYTE			, 2, 0, 1),
	MAKE_ENUM(EQ_USINT			,ARITHCODE(0x07)	,NCC_EQ_USINT			, 2, 0, 1), 
	MAKE_ENUM(EQ_BYTE			,ARITHCODE(0x07)	,NCC_EQ_BYTE			, 2, 0, 1),
	MAKE_ENUM(LE_USINT			,ARITHCODE(0x08)	,NCC_LE_USINT			, 2, 0, 1),
	MAKE_ENUM(LE_BYTE			,ARITHCODE(0x08)	,NCC_LE_BYTE			, 2, 0, 1),
	MAKE_ENUM(LT_USINT			,ARITHCODE(0x09)	,NCC_LT_USINT			, 2, 0, 1),
	MAKE_ENUM(LT_BYTE			,ARITHCODE(0x09)	,NCC_LT_BYTE			, 2, 0, 1),
	MAKE_ENUM(NE_USINT			,ARITHCODE(0x0A)	,NCC_NE_USINT			, 2, 0, 1),
	MAKE_ENUM(NE_BYTE			,ARITHCODE(0x0A)	,NCC_NE_BYTE			, 2, 0, 1),
	MAKE_ENUM(ADD_USINT 		,ARITHCODE(0x0B)	,NCC_ADD_USINT			, 2, 0, 8),
	MAKE_ENUM(MUL_USINT 		,ARITHCODE(0x0C)	,NCC_MUL_USINT			, 2, 0, 8),
	MAKE_ENUM(SUB_USINT 		,ARITHCODE(0x0D)	,NCC_SUB_USINT			, 2, 0, 8),
	MAKE_ENUM(DIV_USINT 		,ARITHCODE(0x0E)	,NCC_DIV_USINT			, 2, 0, 8),
	MAKE_ENUM(MOD_USINT 		,ARITHCODE(0x0F)	,NCC_MOD_USINT			, 2, 0, 8),
/* -0x001x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,ARITHCODE(0x10)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x11)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x12)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x13)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x14)	,						, 0, 0, 0), */
	MAKE_ENUM(GT_SINT			,ARITHCODE(0x15)	,NCC_GT_SINT			, 2, 0, 1),
	MAKE_ENUM(GE_SINT			,ARITHCODE(0x16)	,NCC_GE_SINT			, 2, 0, 1),
	MAKE_ENUM(EQ_SINT			,ARITHCODE(0x17)	,NCC_EQ_SINT			, 2, 0, 1),
	MAKE_ENUM(LE_SINT			,ARITHCODE(0x18)	,NCC_LE_SINT			, 2, 0, 1),
	MAKE_ENUM(LT_SINT			,ARITHCODE(0x19)	,NCC_LT_SINT			, 2, 0, 1),
	MAKE_ENUM(NE_SINT			,ARITHCODE(0x1A)	,NCC_NE_SINT			, 2, 0, 1),
/*								,ARITHCODE(0x1B)	,						, 0, 0, 0), */
	MAKE_ENUM(SHL_DWORD 		,ARITHCODE(0x1C)	,NCC_SHL_DWORD			, 2, 0,32),
	MAKE_ENUM(SHR_DWORD 		,ARITHCODE(0x1D)	,NCC_SHR_DWORD			, 2, 0,32),
	MAKE_ENUM(ROR_DWORD 		,ARITHCODE(0x1E)	,NCC_ROR_DWORD			, 2, 0,32),
	MAKE_ENUM(ROL_DWORD 		,ARITHCODE(0x1F)	,NCC_ROL_DWORD			, 2, 0,32),
/* -0x002x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,ARITHCODE(0x20)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x21)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x22)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x23)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x24)	,						, 0, 0, 0), */
	MAKE_ENUM(GT_UINT			,ARITHCODE(0x25)	,NCC_GT_UINT			, 2, 0, 1),
	MAKE_ENUM(GT_WORD			,ARITHCODE(0x25)	,NCC_GT_WORD			, 2, 0, 1),
	MAKE_ENUM(GE_UINT			,ARITHCODE(0x26)	,NCC_GE_UINT			, 2, 0, 1),
	MAKE_ENUM(GE_WORD			,ARITHCODE(0x26)	,NCC_GE_WORD			, 2, 0, 1),
	MAKE_ENUM(EQ_UINT			,ARITHCODE(0x27)	,NCC_EQ_UINT			, 2, 0, 1),
	MAKE_ENUM(EQ_WORD			,ARITHCODE(0x27)	,NCC_EQ_WORD			, 2, 0, 1),
	MAKE_ENUM(LE_UINT			,ARITHCODE(0x28)	,NCC_LE_UINT			, 2, 0, 1),
	MAKE_ENUM(LE_WORD			,ARITHCODE(0x28)	,NCC_LE_WORD			, 2, 0, 1),
	MAKE_ENUM(LT_UINT			,ARITHCODE(0x29)	,NCC_LT_UINT			, 2, 0, 1),
	MAKE_ENUM(LT_WORD			,ARITHCODE(0x29)	,NCC_LT_WORD			, 2, 0, 1),
	MAKE_ENUM(NE_UINT			,ARITHCODE(0x2A)	,NCC_NE_UINT			, 2, 0, 1),
	MAKE_ENUM(NE_WORD			,ARITHCODE(0x2A)	,NCC_NE_WORD			, 2, 0, 1),
	MAKE_ENUM(ADD_UINT			,ARITHCODE(0x2B)	,NCC_ADD_UINT			, 2, 0,16),
	MAKE_ENUM(MUL_UINT			,ARITHCODE(0x2C)	,NCC_MUL_UINT			, 2, 0,16),
	MAKE_ENUM(SUB_UINT			,ARITHCODE(0x2D)	,NCC_SUB_UINT			, 2, 0,16),
	MAKE_ENUM(DIV_UINT			,ARITHCODE(0x2E)	,NCC_DIV_UINT			, 2, 0,16),
	MAKE_ENUM(MOD_UINT			,ARITHCODE(0x2F)	,NCC_MOD_UINT			, 2, 0,16),
/* -0x003x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,ARITHCODE(0x30)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x31)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x32)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x33)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x34)	,						, 0, 0, 0), */
	MAKE_ENUM(GT_INT			,ARITHCODE(0x35)	,NCC_GT_INT 			, 2, 0, 1),
	MAKE_ENUM(GE_INT			,ARITHCODE(0x36)	,NCC_GE_INT 			, 2, 0, 1),
	MAKE_ENUM(EQ_INT			,ARITHCODE(0x37)	,NCC_EQ_INT 			, 2, 0, 1),
	MAKE_ENUM(LE_INT			,ARITHCODE(0x38)	,NCC_LE_INT 			, 2, 0, 1),
	MAKE_ENUM(LT_INT			,ARITHCODE(0x39)	,NCC_LT_INT 			, 2, 0, 1),
	MAKE_ENUM(NE_INT			,ARITHCODE(0x3A)	,NCC_NE_INT 			, 2, 0, 1),
/*								,ARITHCODE(0x3B)	,						, 0, 0, 0), */
	MAKE_ENUM(AND_DWORD 		,ARITHCODE(0x3C)	,NCC_AND_DWORD			, 2, 0,32),
	MAKE_ENUM(OR_DWORD			,ARITHCODE(0x3D)	,NCC_OR_DWORD			, 2, 0,32),
	MAKE_ENUM(XOR_DWORD 		,ARITHCODE(0x3E)	,NCC_XOR_DWORD			, 2, 0,32),
	MAKE_ENUM(NOT_DWORD 		,ARITHCODE(0x3F)	,NCC_NOT_DWORD			, 1, 0,32),
/* -0x004x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,ARITHCODE(0x40)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x41)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x42)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x43)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x44)	,						, 0, 0, 0), */
	MAKE_ENUM(GT_UDINT			,ARITHCODE(0x45)	,NCC_GT_UDINT			, 2, 0, 1), 
	MAKE_ENUM(GT_DWORD			,ARITHCODE(0x45)	,NCC_GT_DWORD			, 2, 0, 1),
	MAKE_ENUM(GE_UDINT			,ARITHCODE(0x46)	,NCC_GE_UDINT			, 2, 0, 1),
	MAKE_ENUM(GE_DWORD			,ARITHCODE(0x46)	,NCC_GE_DWORD			, 2, 0, 1),
	MAKE_ENUM(EQ_UDINT			,ARITHCODE(0x47)	,NCC_EQ_UDINT			, 2, 0, 1),
	MAKE_ENUM(EQ_DWORD			,ARITHCODE(0x47)	,NCC_EQ_DWORD			, 2, 0, 1),
	MAKE_ENUM(LE_UDINT			,ARITHCODE(0x48)	,NCC_LE_UDINT			, 2, 0, 1),
	MAKE_ENUM(LE_DWORD			,ARITHCODE(0x48)	,NCC_LE_DWORD			, 2, 0, 1),
	MAKE_ENUM(LT_UDINT			,ARITHCODE(0x49)	,NCC_LT_UDINT			, 2, 0, 1),
	MAKE_ENUM(LT_DWORD			,ARITHCODE(0x49)	,NCC_LT_DWORD			, 2, 0, 1),
	MAKE_ENUM(NE_UDINT			,ARITHCODE(0x4A)	,NCC_NE_UDINT			, 2, 0, 1),
	MAKE_ENUM(NE_DWORD			,ARITHCODE(0x4A)	,NCC_NE_DWORD			, 2, 0, 1),
	MAKE_ENUM(ADD_UDINT 		,ARITHCODE(0x4B)	,NCC_ADD_UDINT			, 2, 0,32),
	MAKE_ENUM(MUL_UDINT 		,ARITHCODE(0x4C)	,NCC_MUL_UDINT			, 2, 0,32),
	MAKE_ENUM(SUB_UDINT 		,ARITHCODE(0x4D)	,NCC_SUB_UDINT			, 2, 0,32),
	MAKE_ENUM(DIV_UDINT 		,ARITHCODE(0x4E)	,NCC_DIV_UDINT			, 2, 0,32),
	MAKE_ENUM(MOD_UDINT 		,ARITHCODE(0x4F)	,NCC_MOD_UDINT			, 2, 0,32),
/* -0x005x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,ARITHCODE(0x50)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x51)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x52)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x53)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x54)	,						, 0, 0, 0), */
	MAKE_ENUM(GT_DINT			,ARITHCODE(0x55)	,NCC_GT_DINT			, 2, 0, 1),
	MAKE_ENUM(GE_DINT			,ARITHCODE(0x56)	,NCC_GE_DINT			, 2, 0, 1),
	MAKE_ENUM(EQ_DINT			,ARITHCODE(0x57)	,NCC_EQ_DINT			, 2, 0, 1),
	MAKE_ENUM(LE_DINT			,ARITHCODE(0x58)	,NCC_LE_DINT			, 2, 0, 1),
	MAKE_ENUM(LT_DINT			,ARITHCODE(0x59)	,NCC_LT_DINT			, 2, 0, 1),
	MAKE_ENUM(NE_DINT			,ARITHCODE(0x5A)	,NCC_NE_DINT			, 2, 0, 1),
	MAKE_ENUM(NEG_LREAL 		,ARITHCODE(0x5B)	,NCC_NEG_LREAL			, 1, 0,64),
	MAKE_ENUM(ABS_LREAL 		,ARITHCODE(0x5C)	,NCC_ABS_LREAL			, 1, 0,64),
	MAKE_ENUM(ADD_LREAL 		,ARITHCODE(0x5D)	,NCC_ADD_LREAL			, 2, 0,64),
	MAKE_ENUM(MUL_LREAL 		,ARITHCODE(0x5E)	,NCC_MUL_LREAL			, 2, 0,64),
	MAKE_ENUM(SUB_LREAL 		,ARITHCODE(0x5F)	,NCC_SUB_LREAL			, 2, 0,64),
/* -0x006x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,ARITHCODE(0x60)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x61)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x62)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x63)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x64)	,						, 0, 0, 0), */
	MAKE_ENUM(GT_ULINT			,ARITHCODE(0x65)	,NCC_GT_ULINT			, 2, 0, 1),
	MAKE_ENUM(GT_LWORD			,ARITHCODE(0x65)	,NCC_GT_LWORD			, 2, 0, 1),
	MAKE_ENUM(GE_ULINT			,ARITHCODE(0x66)	,NCC_GE_ULINT			, 2, 0, 1),
	MAKE_ENUM(GE_LWORD			,ARITHCODE(0x66)	,NCC_GE_LWORD			, 2, 0, 1),
	MAKE_ENUM(EQ_ULINT			,ARITHCODE(0x67)	,NCC_EQ_ULINT			, 2, 0, 1),
	MAKE_ENUM(EQ_LWORD			,ARITHCODE(0x67)	,NCC_EQ_LWORD			, 2, 0, 1),
	MAKE_ENUM(LE_ULINT			,ARITHCODE(0x68)	,NCC_LE_ULINT			, 2, 0, 1),
	MAKE_ENUM(LE_LWORD			,ARITHCODE(0x68)	,NCC_LE_LWORD			, 2, 0, 1),
	MAKE_ENUM(LT_ULINT			,ARITHCODE(0x69)	,NCC_LT_ULINT			, 2, 0, 1),
	MAKE_ENUM(LT_LWORD			,ARITHCODE(0x69)	,NCC_LT_LWORD			, 2, 0, 1),
	MAKE_ENUM(NE_ULINT			,ARITHCODE(0x6A)	,NCC_NE_ULINT			, 2, 0, 1),
	MAKE_ENUM(NE_LWORD			,ARITHCODE(0x6A)	,NCC_NE_LWORD			, 2, 0, 1),
	MAKE_ENUM(DIV_LREAL 		,ARITHCODE(0x6B)	,NCC_DIV_LREAL			, 2, 0,64),
	MAKE_ENUM(SHL_LWORD 		,ARITHCODE(0x6C)	,NCC_SHL_LWORD			, 2, 0,64),
	MAKE_ENUM(SHR_LWORD 		,ARITHCODE(0x6D)	,NCC_SHR_LWORD			, 2, 0,64),
	MAKE_ENUM(ROR_LWORD 		,ARITHCODE(0x6E)	,NCC_ROR_LWORD			, 2, 0,64),
	MAKE_ENUM(ROL_LWORD 		,ARITHCODE(0x6F)	,NCC_ROL_LWORD			, 2, 0,64),
/* -0x007x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,ARITHCODE(0x70)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x71)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x72)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x73)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x74)	,						, 0, 0, 0), */
	MAKE_ENUM(GT_LINT			,ARITHCODE(0x75)	,NCC_GT_LINT			, 2, 0, 1),
	MAKE_ENUM(GE_LINT			,ARITHCODE(0x76)	,NCC_GE_LINT			, 2, 0, 1),
	MAKE_ENUM(EQ_LINT			,ARITHCODE(0x77)	,NCC_EQ_LINT			, 2, 0, 1),
	MAKE_ENUM(LE_LINT			,ARITHCODE(0x78)	,NCC_LE_LINT			, 2, 0, 1),
	MAKE_ENUM(LT_LINT			,ARITHCODE(0x79)	,NCC_LT_LINT			, 2, 0, 1),
	MAKE_ENUM(NE_LINT			,ARITHCODE(0x7A)	,NCC_NE_LINT			, 2, 0, 1),
	MAKE_ENUM(MOD_LREAL 		,ARITHCODE(0x7B)	,NCC_MOD_LREAL			, 2, 0,64),
	MAKE_ENUM(AND_LWORD 		,ARITHCODE(0x7C)	,NCC_AND_LWORD			, 2, 0,64),
	MAKE_ENUM(OR_LWORD			,ARITHCODE(0x7D)	,NCC_OR_LWORD			, 2, 0,64),
	MAKE_ENUM(XOR_LWORD 		,ARITHCODE(0x7E)	,NCC_XOR_LWORD			, 2, 0,64),
	MAKE_ENUM(NOT_LWORD 		,ARITHCODE(0x7F)	,NCC_NOT_LWORD			, 1, 0,64),
/* -0x008x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,ARITHCODE(0x80)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x81)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x82)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x83)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x84)	,						, 0, 0, 0), */
	MAKE_ENUM(GT_REAL			,ARITHCODE(0x85)	,NCC_GT_REAL			, 2, 0, 1),
	MAKE_ENUM(GE_REAL			,ARITHCODE(0x86)	,NCC_GE_REAL			, 2, 0, 1),
	MAKE_ENUM(EQ_REAL			,ARITHCODE(0x87)	,NCC_EQ_REAL			, 2, 0, 1),
	MAKE_ENUM(LE_REAL			,ARITHCODE(0x88)	,NCC_LE_REAL			, 2, 0, 1),
	MAKE_ENUM(LT_REAL			,ARITHCODE(0x89)	,NCC_LT_REAL			, 2, 0, 1),
	MAKE_ENUM(NE_REAL			,ARITHCODE(0x8A)	,NCC_NE_REAL			, 2, 0, 1),
	MAKE_ENUM(ADD_ULINT 		,ARITHCODE(0x8B)	,NCC_ADD_ULINT			, 2, 0,64),
	MAKE_ENUM(MUL_ULINT 		,ARITHCODE(0x8C)	,NCC_MUL_ULINT			, 2, 0,64),
	MAKE_ENUM(SUB_ULINT 		,ARITHCODE(0x8D)	,NCC_SUB_ULINT			, 2, 0,64),
	MAKE_ENUM(DIV_ULINT 		,ARITHCODE(0x8E)	,NCC_DIV_ULINT			, 2, 0,64),
	MAKE_ENUM(MOD_ULINT 		,ARITHCODE(0x8F)	,NCC_MOD_ULINT			, 2, 0,64),
/* -0x009x ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,ARITHCODE(0x90)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x91)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x92)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x93)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0x94)	,						, 0, 0, 0), */
	MAKE_ENUM(GT_LREAL			,ARITHCODE(0x95)	,NCC_GT_LREAL			, 2, 0, 1),
	MAKE_ENUM(GE_LREAL			,ARITHCODE(0x96)	,NCC_GE_LREAL			, 2, 0, 1),
	MAKE_ENUM(EQ_LREAL			,ARITHCODE(0x97)	,NCC_EQ_LREAL			, 2, 0, 1),
	MAKE_ENUM(LE_LREAL			,ARITHCODE(0x98)	,NCC_LE_LREAL			, 2, 0, 1),
	MAKE_ENUM(LT_LREAL			,ARITHCODE(0x99)	,NCC_LT_LREAL			, 2, 0, 1),
	MAKE_ENUM(NE_LREAL			,ARITHCODE(0x9A)	,NCC_NE_LREAL			, 2, 0, 1),
	MAKE_ENUM(NEG_LINT			,ARITHCODE(0x9B)	,NCC_NEG_LINT			, 1, 0,64),
	MAKE_ENUM(ABS_LINT			,ARITHCODE(0x9C)	,NCC_ABS_LINT			, 1, 0,64),
	MAKE_ENUM(ADD_LINT			,ARITHCODE(0x9D)	,NCC_ADD_LINT			, 2, 0,64),
	MAKE_ENUM(MUL_LINT			,ARITHCODE(0x9E)	,NCC_MUL_LINT			, 2, 0,64),
	MAKE_ENUM(SUB_LINT			,ARITHCODE(0x9F)	,NCC_SUB_LINT			, 2, 0,64),
/* -0x00Ax ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,ARITHCODE(0xA0)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xA1)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xA2)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xA3)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xA4)	,						, 0, 0, 0), */
	MAKE_ENUM(GT_STRING 		,ARITHCODE(0xA5)	,NCC_GT_STRING			, 2, 0, 1),
	MAKE_ENUM(GE_STRING 		,ARITHCODE(0xA6)	,NCC_GE_STRING			, 2, 0, 1),
	MAKE_ENUM(EQ_STRING 		,ARITHCODE(0xA7)	,NCC_EQ_STRING			, 2, 0, 1),
	MAKE_ENUM(LE_STRING 		,ARITHCODE(0xA8)	,NCC_LE_STRING			, 2, 0, 1),
	MAKE_ENUM(LT_STRING 		,ARITHCODE(0xA9)	,NCC_LT_STRING			, 2, 0, 1),
	MAKE_ENUM(NE_STRING 		,ARITHCODE(0xAA)	,NCC_NE_STRING			, 2, 0, 1),
	MAKE_ENUM(DIV_LINT			,ARITHCODE(0xAB)	,NCC_DIV_LINT			, 2, 0,64),
	MAKE_ENUM(MOD_LINT			,ARITHCODE(0xAC)	,NCC_MOD_LINT			, 2, 0,64),
	MAKE_ENUM(MOVE_STRING		,ARITHCODE(0xAD)	,NCC_MOVE_STRING		, 2, 0, 0),
	MAKE_ENUM(LEN_STRING_INT	,ARITHCODE(0xAE)	,NCC_LEN_STRING_INT 	, 1, 0, 8),
	MAKE_ENUM(INIT_STRING		,ARITHCODE(0xAF)	,NCC_INIT_STRING		, 2 ,1, 0),
/* -0x00Bx ------------------------------------------------------------------------------------------------ */
/*	MAKE_ENUM(					,ARITHCODE(0xB0)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xB1)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xB2)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xB3)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xB4)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xB5)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xB6)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xB7)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xB8)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xB9)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xBA)	,						, 0, 0, 0), */
/*	MAKE_ENUM(					,ARITHCODE(0xBB)	,						, 0, 0, 0), */
	MAKE_ENUM(POW_REAL_REAL 	,ARITHCODE(0xBC)	,NCC_POW_REAL_REAL		, 2, 0,32),
	MAKE_ENUM(POW_REAL_INT		,ARITHCODE(0xBD)	,NCC_POW_REAL_INT		, 2, 0,32),
	MAKE_ENUM(POW_REAL_DINT 	,ARITHCODE(0xBE)	,NCC_POW_REAL_DINT		, 2, 0,32),
	MAKE_ENUM(POW_LREAL_LREAL	,ARITHCODE(0xBF)	,NCC_POW_LREAL_LREAL	, 2, 0,64),
/* -0x00Cx ------------------------------------------------------------------------------------------------ */
	MAKE_ENUM(POW_LREAL_INT 	,ARITHCODE(0xC0)	,NCC_POW_LREAL_INT		, 2, 0,64),
	MAKE_ENUM(POW_LREAL_DINT	,ARITHCODE(0xC1)	,NCC_POW_LREAL_DINT 	, 2, 0,64),
	MAKE_ENUM(NEG_REAL			,ARITHCODE(0xC2)	,NCC_NEG_REAL			, 1, 0,32),
	MAKE_ENUM(ABS_REAL			,ARITHCODE(0xC3)	,NCC_ABS_REAL			, 1, 0,32),
	MAKE_ENUM(ADD_REAL			,ARITHCODE(0xC4)	,NCC_ADD_REAL			, 2, 0,32),
	MAKE_ENUM(MUL_REAL			,ARITHCODE(0xC5)	,NCC_MUL_REAL			, 2, 0,32),
	MAKE_ENUM(SUB_REAL			,ARITHCODE(0xC6)	,NCC_SUB_REAL			, 2, 0,32),
	MAKE_ENUM(DIV_REAL			,ARITHCODE(0xC7)	,NCC_DIV_REAL			, 2, 0,32),
	MAKE_ENUM(MOD_REAL			,ARITHCODE(0xC8)	,NCC_MOD_REAL			, 2, 0,32),
	MAKE_ENUM(NEG_DINT			,ARITHCODE(0xC9)	,NCC_NEG_DINT			, 1, 0,32),
	MAKE_ENUM(ABS_DINT			,ARITHCODE(0xCA)	,NCC_ABS_DINT			, 1, 0,32),
	MAKE_ENUM(ADD_DINT			,ARITHCODE(0xCB)	,NCC_ADD_DINT			, 2, 0,32),
	MAKE_ENUM(MUL_DINT			,ARITHCODE(0xCC)	,NCC_MUL_DINT			, 2, 0,32), 
	MAKE_ENUM(SUB_DINT			,ARITHCODE(0xCD)	,NCC_SUB_DINT			, 2, 0,32),
	MAKE_ENUM(DIV_DINT			,ARITHCODE(0xCE)	,NCC_DIV_DINT			, 2, 0,32),
	MAKE_ENUM(MOD_DINT			,ARITHCODE(0xCF)	,NCC_MOD_DINT			, 2, 0,32)
};
#endif

/* ---------------------------------------------------------------------------- */

#if defined(OPC_CODE_GEN)
	inline int ASM_getNumberOfOpcodes()
	{
		return sizeof(OpcodeTable) / sizeof(ASM_CodeTableEntry);
	}

	inline ASM_CodeTableEntry* ASM_getOpcode(int i)
	{
		return &OpcodeTable[i];
	}
#endif /* OPC_CODE_GEN */

/* ---------------------------------------------------------------------------- */

#if defined(OPC_NCC)
	int ASM_getNumberOfOpcodes()
	{
		return sizeof(OpcodeList) / sizeof(OpcodeList[0]);
	}

	COpcode ASM_getOpcode(int opcode)
	{
		COpcode* pOpC = NULL;

		for (int i=0; i<sizeof(OpcodeList) / sizeof(OpcodeList[0]); i++)
		{
		   if (opcode == OpcodeList[i].OpCode)
		   {
			  return OpcodeList[i];
		   }
		   else if ((opcode&0xFF) == OpcodeList[i].OpCode)
		   {
			  pOpC = &OpcodeList[i];
		   }
		}
		if(pOpC)
			return *pOpC;

	  #ifdef _DEBUG
		_ASSERT(0);
	  #endif

		return OpcodeList[0];  /* illegal opcode */
	}
#endif

/* ---------------------------------------------------------------------------- */

#define ASM_JUMP			ASM_JMP
#define ASM_JMPC			ASM_JMPC
#define ASM_JMPN			ASM_JMPCN

#endif

/* ---------------------------------------------------------------------------- */
