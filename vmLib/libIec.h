
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
 * Filename: libIec.h
 */


#ifndef _LIBIEC_H_
#define _LIBIEC_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif


/* IEC FUNCTIONS DEFINES
 * ----------------------------------------------------------------------------
 */
#define CTU_PV_MAX	  (32767)
#define CTD_PV_MIN	  (-32768)
#define CTUD_PV_MAX   CTU_PV_MAX
#define CTUD_PV_MIN   CTD_PV_MIN


/* DECLARATIONS OF IEC FUNCTIONS
 * ----------------------------------------------------------------------------
 */
void LeftString_int(STDLIBFUNCALL);
void RightString_int(STDLIBFUNCALL);
void MidString_int(STDLIBFUNCALL);
void ConcatString(STDLIBFUNCALL);
void InsertString_int(STDLIBFUNCALL);
void DeleteString_int(STDLIBFUNCALL);
void ReplaceString_int(STDLIBFUNCALL);
void FindString_int(STDLIBFUNCALL);
void LenString_int(STDLIBFUNCALL);

void bool_to_string(STDLIBFUNCALL);
void byte_to_string(STDLIBFUNCALL);
void word_to_string(STDLIBFUNCALL);
void dword_to_string(STDLIBFUNCALL);
void int_to_string(STDLIBFUNCALL);
void dint_to_string(STDLIBFUNCALL);
void real_to_string(STDLIBFUNCALL);
void lreal_to_string(STDLIBFUNCALL);

void string_to_bool(STDLIBFUNCALL);
void string_to_byte(STDLIBFUNCALL);
void string_to_word(STDLIBFUNCALL);
void string_to_dword(STDLIBFUNCALL);
void string_to_dint(STDLIBFUNCALL);
void string_to_int(STDLIBFUNCALL);
void string_to_real(STDLIBFUNCALL);
void string_to_lreal(STDLIBFUNCALL);
void string_to_time(STDLIBFUNCALL);

void iec_sin_real(STDLIBFUNCALL);
void iec_sin_lreal(STDLIBFUNCALL);
void iec_asin_real(STDLIBFUNCALL);
void iec_asin_lreal(STDLIBFUNCALL);
void iec_cos_real(STDLIBFUNCALL);
void iec_cos_lreal(STDLIBFUNCALL);
void iec_acos_real(STDLIBFUNCALL);
void iec_acos_lreal(STDLIBFUNCALL);
void iec_tan_real(STDLIBFUNCALL);
void iec_tan_lreal(STDLIBFUNCALL);
void iec_atan_real(STDLIBFUNCALL);
void iec_atan_lreal(STDLIBFUNCALL);
void iec_ln_real(STDLIBFUNCALL);
void iec_ln_lreal(STDLIBFUNCALL);
void iec_log_real(STDLIBFUNCALL);
void iec_log_lreal(STDLIBFUNCALL);
void iec_sqrt_real(STDLIBFUNCALL);
void iec_sqrt_lreal(STDLIBFUNCALL);
void iec_exp_real(STDLIBFUNCALL);
void iec_exp_lreal(STDLIBFUNCALL);
void time32_to_string(STDLIBFUNCALL);

void mux_bit(STDLIBFUNCALL);
void mux_byte(STDLIBFUNCALL);
void mux_int(STDLIBFUNCALL);
void mux_dint(STDLIBFUNCALL);
void mux_string(STDLIBFUNCALL);
void mux_64bit(STDLIBFUNCALL);

void trunc_real_dint(STDLIBFUNCALL);
void trunc_lreal_dint(STDLIBFUNCALL);

/* DECLARATIONS OF IEC FUNCTIONS BLOCKS
 * ----------------------------------------------------------------------------
 */
void rs 	(STDLIBFBCALL);
void sr 	(STDLIBFBCALL);
void tp 	(STDLIBFBCALL);  
void ton	(STDLIBFBCALL); 
void tof	(STDLIBFBCALL); 
void ctu	(STDLIBFBCALL);
void ctd	(STDLIBFBCALL);
void ctud	(STDLIBFBCALL);
void r_trig (STDLIBFBCALL);
void f_trig (STDLIBFBCALL);


#define RTS_PRAGMA_PACK_1	/* >>>> Align 1 Begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_1


/* IEC FUNCTION STRUCTURES
 * ----------------------------------------------------------------------------
 */
#if defined(IP_CFG_STRING)
	typedef struct LEN_STRING_PAR_TYP_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_INT(retVal);
	} LEN_STRING_PAR_TYP;


	typedef struct LEFT_STRING_PAR_TYP_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_INT(len);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} LEFT_STRING_PAR_TYP;


	typedef struct RIGHT_STRING_PAR_TYP_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_INT(len);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} RIGHT_STRING_PAR_TYP;

	typedef struct MID_STRING_PAR_TYP_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_INT(len);
		DEC_FUN_INT(startPos);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} MID_STRING_PAR_TYP;

	typedef struct CONCAT_STRING_PAR_TYP_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} CONCAT_STRING_PAR_TYP;

	typedef struct INSERT_STRING_PAR_TYP_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr1);
		DEC_FUN_PTR(IEC_STRING,inStr2);
		DEC_FUN_INT(startPos);	 
		DEC_FUN_PTR(IEC_STRING,retStr);
	} INSERT_STRING_PAR_TYP;

	typedef struct DELETE_STRING_PAR_TYP_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_INT(len);
		DEC_FUN_INT(startPos);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} DELETE_STRING_PAR_TYP;

	typedef struct REPLACE_STRING_PAR_TYP_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr1);
		DEC_FUN_PTR(IEC_STRING,inStr2);
		DEC_FUN_INT(len);
		DEC_FUN_INT(startPos);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} REPLACE_STRING_PAR_TYP;

	typedef struct
	{
		DEC_FUN_PTR(IEC_STRING,inStr1);
		DEC_FUN_PTR(IEC_STRING,inStr2);
		DEC_FUN_INT(iRetVal);

	} FIND_STRING_PAR_TYP;
#endif

#if defined(IP_CFG_BOOL) && defined(IP_CFG_STRING)
	typedef struct STRING_TO_BOOL_TAG 
	{ 
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_BOOL(retVal);
	} STRING_TO_BOOL;
#endif

#if defined(IP_CFG_BYTE) && defined(IP_CFG_STRING)
	typedef struct STRING_TO_BYTE_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_BYTE(retVal);
	} STRING_TO_BYTE;
#endif

#if (defined(IP_CFG_WORD) || defined(IP_CFG_UINT)) && defined(IP_CFG_STRING)
	typedef struct STRING_TO_WORD_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_WORD(retVal);
	} STRING_TO_WORD;
#endif

#if (defined(IP_CFG_DWORD) || defined(IP_CFG_UDINT)) && defined(IP_CFG_STRING)
	typedef struct STRING_TO_DWORD_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_DWORD(retVal);
	} STRING_TO_DWORD;
#endif

#if defined(IP_CFG_DINT) && defined(IP_CFG_STRING)
	typedef struct STRING_TO_DINT_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_DINT(retVal);
	} STRING_TO_DINT;
#endif

#if defined(IP_CFG_INT) && defined(IP_CFG_STRING)
	typedef struct STRING_TO_INT_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_INT(retVal);
	} STRING_TO_INT;
#endif

#if defined(IP_CFG_DINT) && defined(IP_CFG_STRING)
	typedef struct STRING_TO_REAL_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_REAL(retVal);
	} STRING_TO_REAL;
#endif

#if defined(IP_CFG_LREAL) && defined(IP_CFG_STRING)
	typedef struct STRING_TO_LREAL_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_LREAL(retVal);
	} STRING_TO_LREAL;
#endif

#if defined(IP_CFG_TIME) && defined(IP_CFG_STRING)
	typedef struct STRING_TO_TIME_TAG
	{
		DEC_FUN_PTR(IEC_STRING,inStr);
		DEC_FUN_TIME(retVal);
	} STRING_TO_TIME;
#endif

#if defined(IP_CFG_BOOL) && defined(IP_CFG_STRING)
	typedef struct BOOL_TO_STRING_PAR_TAG{
		DEC_FUN_BOOL(in);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} BOOL_TO_STRING_PAR;
#endif

#if defined(IP_CFG_BYTE) && defined(IP_CFG_STRING)
	typedef struct BYTE_TO_STRING_PAR_TAG{
		DEC_FUN_BYTE(in);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} BYTE_TO_STRING_PAR;
#endif

#if (defined(IP_CFG_WORD) || defined(IP_CFG_UINT)) && defined(IP_CFG_STRING)
	typedef struct WORD_TO_STRING_PAR_TAG{
		DEC_FUN_WORD(in);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} WORD_TO_STRING_PAR;
#endif

#if (defined(IP_CFG_DWORD) || defined(IP_CFG_UDINT)) && defined(IP_CFG_STRING)
	typedef struct DWORD_TO_STRING_PAR_TAG
	{
		DEC_FUN_DWORD(in);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} DWORD_TO_STRING_PAR;
#endif

#if defined(IP_CFG_INT) && defined(IP_CFG_STRING)
	typedef struct INT_TO_STRING_PAR_TAG
	{
		DEC_FUN_INT(in);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} INT_TO_STRING_PAR;
#endif

#if defined(IP_CFG_DINT) && defined(IP_CFG_STRING)
	typedef struct DINT_TO_STRING_PAR_TAG
	{
		DEC_FUN_DINT(in);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} DINT_TO_STRING_PAR;
#endif

#if defined(IP_CFG_REAL) && defined(IP_CFG_STRING)
	typedef struct REAL_TO_STRING_PAR_TAG
	{
		DEC_FUN_REAL(in);
		DEC_FUN_PTR(IEC_STRING,retStr);
	} REAL_TO_STRING_PAR;
#endif

#if defined(IP_CFG_LREAL) && defined(IP_CFG_STRING)
	typedef struct LREAL_TO_STRING_PAR_TAG
	{
		DEC_FUN_LREAL(in);
		IEC_STRING OS_DPTR* retStr;
	} LREAL_TO_STRING_PAR;
#endif

#if defined(IP_CFG_REAL)
	typedef struct SIN_REAL_PAR_TAG
	{
		DEC_FUN_REAL(in);
		DEC_FUN_REAL(ret);
	} SIN_REAL_PAR;
#endif

#if defined(IP_CFG_LREAL)
	typedef struct SIN_LREAL_PAR_TAG
	{
		DEC_FUN_LREAL(in);
		DEC_FUN_LREAL(ret);
	} SIN_LREAL_PAR;
#endif

#if defined(IP_CFG_REAL)
	typedef struct ASIN_REAL_PAR_TAG
	{
		DEC_FUN_REAL(in);
		DEC_FUN_REAL(ret);
	} ASIN_REAL_PAR;
#endif

#if defined(IP_CFG_LREAL)
	typedef struct ASIN_LREAL_PAR_TAG
	{
		DEC_FUN_LREAL(in);
		DEC_FUN_LREAL(ret);
	} ASIN_LREAL_PAR;
#endif

#if defined(IP_CFG_REAL)
	typedef struct COS_REAL_PAR_TAG
	{
		DEC_FUN_REAL(in);
		DEC_FUN_REAL(ret);
	} COS_REAL_PAR;
#endif

#if defined(IP_CFG_LREAL)
	typedef struct COS_LREAL_PAR_TAG
	{
		DEC_FUN_LREAL(in);
		DEC_FUN_LREAL(ret);
	} COS_LREAL_PAR;
#endif

#if defined(IP_CFG_REAL)
	typedef struct ACOS_REAL_PAR_TAG
	{
		DEC_FUN_REAL(in);
		DEC_FUN_REAL(ret);
	} ACOS_REAL_PAR;
#endif

#if defined(IP_CFG_LREAL)
	typedef struct ACOS_LREAL_PAR_TAG
	{
		DEC_FUN_LREAL(in);
		DEC_FUN_LREAL(ret);
	} ACOS_LREAL_PAR;
#endif

#if defined(IP_CFG_REAL)
	typedef struct TAN_REAL_PAR_TAG
	{
		DEC_FUN_REAL(in);
		DEC_FUN_REAL(ret);
	} TAN_REAL_PAR;
#endif

#if defined(IP_CFG_LREAL)
	typedef struct TAN_LREAL_PAR_TAG
	{
		DEC_FUN_LREAL(in);
		DEC_FUN_LREAL(ret);
	} TAN_LREAL_PAR;
#endif

#if defined(IP_CFG_REAL)
	typedef struct ATAN_REAL_PAR_TAG
	{
		DEC_FUN_REAL(in);
		DEC_FUN_REAL(ret);
	} ATAN_REAL_PAR;
#endif

#if defined(IP_CFG_LREAL)
	typedef struct ATAN_LREAL_PAR_TAG
	{
		DEC_FUN_LREAL(in);
		DEC_FUN_LREAL(ret);
	} ATAN_LREAL_PAR;
#endif

#if defined(IP_CFG_REAL)
	typedef struct LN_REAL_PAR_TAG
	{
		DEC_FUN_REAL(in);
		DEC_FUN_REAL(ret);
	} LN_REAL_PAR;
#endif

#if defined(IP_CFG_LREAL)
	typedef struct LN_LREAL_PAR_TAG
	{
		DEC_FUN_LREAL(in);
		DEC_FUN_LREAL(ret);
	} LN_LREAL_PAR;
#endif

#if defined(IP_CFG_REAL)
	typedef struct LOG_REAL_PAR_TAG
	{
		DEC_FUN_REAL(in);
		DEC_FUN_REAL(ret);
	} LOG_REAL_PAR;
#endif

#if defined(IP_CFG_LREAL)
	typedef struct LOG_LREAL_PAR_TAG
	{
		DEC_FUN_LREAL(in);
		DEC_FUN_LREAL(ret);
	} LOG_LREAL_PAR;
#endif

#if defined(IP_CFG_REAL)
	typedef struct SQRT_REAL_PAR_TAG
	{
		DEC_FUN_REAL(in);
		DEC_FUN_REAL(ret);
	} SQRT_REAL_PAR;
#endif

#if defined(IP_CFG_LREAL)
	typedef struct SQRT_LREAL_PAR_TAG
	{
		DEC_FUN_LREAL(in);
		DEC_FUN_LREAL(ret);
	} SQRT_LREAL_PAR;
#endif

#if defined(IP_CFG_REAL)
	typedef struct EXP_REAL_PAR_TAG
	{
		DEC_FUN_REAL(in);
		DEC_FUN_REAL(ret);
	} EXP_REAL_PAR;
#endif

#if defined(IP_CFG_LREAL)
	typedef struct EXP_LREAL_PAR_TAG
	{
		DEC_FUN_LREAL(in);
		DEC_FUN_LREAL(ret);
	} EXP_LREAL_PAR;
#endif

#if defined(IP_CFG_STRING) && defined(IP_CFG_TIME)
	typedef struct TIME_TO_STRING_PAR_TAG
	{
		DEC_FUN_TIME( in );
		DEC_FUN_PTR(IEC_STRING,retStr);
	} TIME_TO_STRING_PAR;
#endif

#if defined(IP_CFG_BOOL)
	typedef struct MUX_BIT_PAR_TYP_TAG
	{
		DEC_FUN_INT(sel);
		DEC_FUN_BYTE(in);
		DEC_FUN_BYTE(ret);
	} MUX_BIT_PAR_TYP;
#endif

#if defined(IP_CFG_BYTE)
	typedef struct MUX_BYTE_PAR_TYP_TAG
	{
		DEC_FUN_INT(sel);
		DEC_FUN_BYTE(in);
		DEC_FUN_BYTE(ret);
	} MUX_BYTE_PAR_TYP;
#endif

#if defined(IP_CFG_INT)
	typedef struct MUX_INT_PAR_TYP_TAG
	{
		DEC_FUN_INT(sel);
		DEC_FUN_INT(in);
		DEC_FUN_INT(ret);
	} MUX_INT_PAR_TYP;
#endif

#if defined(IP_CFG_DINT)
	typedef struct MUX_DINT_PAR_TYP_TAG
	{
		DEC_FUN_INT(sel);
		DEC_FUN_DINT(in);
		DEC_FUN_DINT(ret);
	} MUX_DINT_PAR_TYP;
#endif

#if defined(IP_CFG_STRING)
	typedef struct MUX_STRING_PAR_TYP_TAG
	{
		DEC_FUN_INT(sel);
		DEC_FUN_PTR(IEC_STRING,in);
		DEC_FUN_PTR(IEC_STRING,ret);
	} MUX_STRING_PAR_TYP;
#endif

#if defined(IP_CFG_LREAL)
	typedef struct MUX_64BIT_PAR_TYP_TAG
	{
		DEC_FUN_INT(sel);
		DEC_FUN_LREAL(in);
		DEC_FUN_LREAL(ret);
	} MUX_64BIT_PAR_TYP;
#endif

#if defined(IP_CFG_REAL) && defined(IP_CFG_DINT)
	typedef struct TRUNC_REAL_DINT_PAR_TYP_TAG
	{
		DEC_FUN_REAL(in);
		DEC_FUN_DINT(ret);
	} TRUNC_REAL_DINT_PAR_TYP;
#endif

#if defined(IP_CFG_LREAL) && defined(IP_CFG_DINT)
	typedef struct TRUNC_LREAL_DINT_PAR_TYP_TAG
	{
		DEC_FUN_LREAL(in);
		DEC_FUN_DINT(ret);
	} TRUNC_LREAL_DINT_PAR_TYP;
#endif

	typedef struct FUNC_ALI_TEST_TAG
	{
		DEC_FUN_BYTE(bDummy);
		DEC_FUN_LREAL(lDummy);

	} FUNC_ALI_TEST;


/* IEC FUNCTION BLOCKS STRUCTURES
 * ----------------------------------------------------------------------------
 */

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

#if defined(IP_CFG_BOOL)
	typedef struct
	{
		DEC_VAR(IEC_BYTE, b);			/* 7 6 5 4 3 Q1 RESET SET1 */
	
	} SR_TYP;
	
	typedef struct
	{
		DEC_VAR(IEC_BYTE, b);			/* 7 6 5 4 3 Q1 RESET SET1 */

	} RS_TYP;
#endif

#if defined(IP_CFG_BOOL) && defined(IP_CFG_TIME)
	typedef struct
	{
		/* input variables
		 */
		DEC_VAR(IEC_BYTE,  IN); 		/* 7 6 5 4 3 2 1 bIn */
		DEC_VAR(IEC_UDINT, PT);
		
		/* output variables
		 */
		DEC_VAR(IEC_BYTE,  Q);			/* 7 6 5 4 3 2 1 bOut */		 
		DEC_VAR(IEC_UDINT, ET);
		
		/* non retain
		 */
		DEC_VAR(IEC_UDINT, tStart);
		DEC_VAR(IEC_BOOL,  bTrig);
		DEC_VAR(IEC_BYTE,  LastIN);

	} TP_TON_TOF_PAR;
#endif

#if defined(IP_CFG_BOOL) && defined(IP_CFG_INT)
	typedef struct
	{
		/* input variable
		 */
		DEC_VAR(IEC_BYTE, bIn); 		/* 7 6 5 4 3 CU_EDGE(helper) R CU */ 
		DEC_VAR(IEC_INT,  pvIn);
		
		/* output variable
		 */
		DEC_VAR(IEC_BYTE, qOut);		/* 7 6 5 4 3 2 1 Q */
		DEC_VAR(IEC_INT,  cvOut);

	} CTU_PAR;
	
	typedef struct
	{
		/* input variable
		 */
		DEC_VAR(IEC_BYTE, bIn); 		/* 7 6 5 4 3 CD_EDGE(helper) LD CD */
		DEC_VAR(IEC_INT,  pvIn);
		
		/* output variable
		 */
		DEC_VAR(IEC_BYTE, qOut);		/* 7 6 5 4 3 2 1 Q */
		DEC_VAR(IEC_INT,  cvOut);

	} CTD_PAR;
	
	typedef struct
	{
		/* input variable
		 */
		DEC_VAR(IEC_BYTE, bIn); 		/* 7 6 CD_EDGE CU_EDGE LD R CD CU */
		DEC_VAR(IEC_INT,  pvIn);
		
		/* output variable
		 */
		DEC_VAR(IEC_BYTE, bOut);		/* 7 6 5 4 3 2 qdOut quOut */
		DEC_VAR(IEC_INT,  cvOut);

	} CTUD_PAR;
#endif

#if defined(IP_CFG_BOOL)
	typedef struct
	{
		/* input variable
		 */
		DEC_VAR(IEC_BYTE, b);			/* 7 6 5 4 3 m q clk */

	} TRIG_PAR;
#endif

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

#if defined(IP_CFG_BOOL)
	typedef struct
	{
		DEC_VAR(IEC_BYTE, b);			/* 7 6 5 4 3 Q1 RESET SET1 */
	
	} SR_TYP;
	
	typedef struct
	{
		DEC_VAR(IEC_BYTE, b);			/* 7 6 5 4 3 Q1 RESET SET1 */

	} RS_TYP;
#endif

#if defined(IP_CFG_BOOL) && defined(IP_CFG_TIME)
	typedef struct
	{
		/* input variables
		 */
		DEC_VAR(IEC_BYTE,  IN); 		/* 7 6 5 4 3 2 1 bIn */ 		
		DEC_VAR(IEC_BYTE,  dummy_08_bIn);
		DEC_VAR(IEC_UDINT, PT);
		
		/* output variables
		 */
		DEC_VAR(IEC_BYTE,  Q);			/* 7 6 5 4 3 2 1 bOut */		 
		DEC_VAR(IEC_BYTE,  dummy_08_bOut);
		DEC_VAR(IEC_UDINT, ET);
		
		/* non retain
		 */
		DEC_VAR(IEC_UDINT, tStart);
		DEC_VAR(IEC_BOOL,  bTrig);
		DEC_VAR(IEC_BYTE,  LastIN);

	} TP_TON_TOF_PAR;
#endif

#if defined(IP_CFG_BOOL) && defined(IP_CFG_INT)
	typedef struct
	{
		/* input variable
		 */
		DEC_VAR(IEC_BYTE, bIn); 		/* 7 6 5 4 3 CU_EDGE(helper) R CU */ 
		DEC_VAR(IEC_BYTE, dummy_08_bIn);
		DEC_VAR(IEC_INT,  pvIn);
		
		/* output variable
		 */
		DEC_VAR(IEC_BYTE, qOut);		/* 7 6 5 4 3 2 1 Q */
		DEC_VAR(IEC_BYTE, dummy_08_qOut);
		DEC_VAR(IEC_INT,  cvOut);

	} CTU_PAR;
	
	typedef struct
	{
		/* input variable
		 */
		DEC_VAR(IEC_BYTE, bIn); 		/* 7 6 5 4 3 CD_EDGE(helper) LD CD */
		DEC_VAR(IEC_BYTE, dummy_08_bIn);
		DEC_VAR(IEC_INT,  pvIn);
		
		/* output variable
		 */
		DEC_VAR(IEC_BYTE, qOut);		/* 7 6 5 4 3 2 1 Q */
		DEC_VAR(IEC_BYTE, dummy_08_qOut);
		DEC_VAR(IEC_INT,  cvOut);

	} CTD_PAR;
	
	typedef struct
	{
		/* input variable
		 */
		DEC_VAR(IEC_BYTE, bIn); 		/* 7 6 CD_EDGE CU_EDGE LD R CD CU */
		DEC_VAR(IEC_BYTE, dummy_08_bIn);
		DEC_VAR(IEC_INT,  pvIn);
		
		/* output variable
		 */
		DEC_VAR(IEC_BYTE, bOut);		/* 7 6 5 4 3 2 qdOut quOut */
		DEC_VAR(IEC_BYTE, dummy_08_qOut);
		DEC_VAR(IEC_INT,  cvOut);

	} CTUD_PAR;
#endif

#if defined(IP_CFG_BOOL)
	typedef struct
	{
		/* input variable
		 */
		DEC_VAR(IEC_BYTE, b);			/* 7 6 5 4 3 m q clk */

	} TRIG_PAR;
#endif

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

#if defined(IP_CFG_BOOL)
	typedef struct
	{
		DEC_VAR(IEC_BYTE, b);			/* 7 6 5 4 3 Q1 RESET SET1 */
	
	} SR_TYP;
	
	typedef struct
	{
		DEC_VAR(IEC_BYTE, b);			/* 7 6 5 4 3 Q1 RESET SET1 */

	} RS_TYP;
#endif

#if defined(IP_CFG_BOOL) && defined(IP_CFG_TIME)
	typedef struct
	{
		/* input variables
		 */
		DEC_VAR(IEC_BYTE,  IN); 		/* 7 6 5 4 3 2 1 IN */
		DEC_VAR(IEC_BYTE,  dummy_08_bIn);
		DEC_VAR(IEC_WORD,  dummy_16_bIn);
		DEC_VAR(IEC_UDINT, PT);
		
		/* output variables
		 */
		DEC_VAR(IEC_BYTE,  Q);		/* 7 6 5 4 3 2 1 Q */
		DEC_VAR(IEC_BYTE,  dummy_08_bOut);
		DEC_VAR(IEC_WORD,  dummy_16_bOut);
		DEC_VAR(IEC_UDINT, ET);
		
		/* non retain
		 */
		DEC_VAR(IEC_UDINT, tStart);
		DEC_VAR(IEC_BOOL,  bTrig);
		DEC_VAR(IEC_BYTE,  LastIN);
		DEC_VAR(IEC_WORD,  dummy_16_m);

	} TP_TON_TOF_PAR;
#endif

#if defined(IP_CFG_BOOL) && defined(IP_CFG_INT)
	typedef struct
	{
		/* input variable
		 */
		DEC_VAR(IEC_BYTE, bIn); 		/* 7 6 5 4 3 CU_EDGE(helper) R CU */ 
		DEC_VAR(IEC_BYTE, dummy_08_bIn);
		DEC_VAR(IEC_INT,  pvIn);
		
		/* output variable
		 */
		DEC_VAR(IEC_BYTE, qOut);		/* 7 6 5 4 3 2 1 Q */
		DEC_VAR(IEC_BYTE, dummy_08_bOut);
		DEC_VAR(IEC_INT,  cvOut);

	} CTU_PAR;
	
	typedef struct
	{
		/* input variable
		 */
		DEC_VAR(IEC_BYTE, bIn); 		/* 7 6 5 4 3 CD_EDGE(helper) LD CD */
		DEC_VAR(IEC_BYTE, dummy_08_bIn);
		DEC_VAR(IEC_INT,  pvIn);
		
		/* output variable
		 */
		DEC_VAR(IEC_BYTE, qOut);		/* 7 6 5 4 3 2 1 Q */
		DEC_VAR(IEC_BYTE, dummy_08_bOut);
		DEC_VAR(IEC_INT,  cvOut);

	} CTD_PAR;
	
	typedef struct
	{
		/* input variable
		 */
		DEC_VAR(IEC_BYTE, bIn); 		/* 7 6 CD_EDGE CU_EDGE LD R CD CU */
		DEC_VAR(IEC_BYTE, dummy_08_bIn);
		DEC_VAR(IEC_INT,  pvIn);
		
		/* output variable
		 */
		DEC_VAR(IEC_BYTE, bOut);		/* 7 6 5 4 3 2 qdOut quOut */
		DEC_VAR(IEC_BYTE, dummy_08_bOut);
		DEC_VAR(IEC_INT,  cvOut);

	} CTUD_PAR;
#endif

#if defined(IP_CFG_BOOL)
	typedef struct
	{
		/* input variable
		 */
		DEC_VAR(IEC_BYTE, b);			/* 7 6 5 4 3 m q clk */

	} TRIG_PAR;
#endif

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */

	typedef struct
	{
		DEC_VAR(IEC_BYTE,  bDummy);
		DEC_VAR(IEC_LREAL, lDummy);
	
	} FB_ALI_TEST;

#define RTS_PRAGMA_PACK_DEF 	
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_DEF 	/* <<<< Align 1 end <<<<<<<<<<<<<<<<<<<<<<<<<<< */

#endif	/* _LIBIEC_H_ */

/* ---------------------------------------------------------------------------- */
