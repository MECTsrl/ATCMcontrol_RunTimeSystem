
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
 * Filename: funIec.h
 */


#if defined(IP_CFG_STRING) 
									LeftString_int		,	/*0*/	/*LEFT_STRING_PAR_TYP		*/
									RightString_int 	,	/*1*/	/*RIGHT_STRING_PAR_TYP		*/
									MidString_int		,	/*2*/	/*MID_STRING_PAR_TYP		*/
									ConcatString		,	/*3*/	/*CONCAT_STRING_PAR_TYP 	*/
									InsertString_int	,	/*4*/	/*INSERT_STRING_PAR_TYP 	*/
									DeleteString_int	,	/*5*/	/*DELETE_STRING_PAR_TYP 	*/
									ReplaceString_int	,	/*6*/	/*REPLACE_STRING_PAR_TYP	*/
									FindString_int		,	/*7*/	/*FIND_STRING_PAR_TYP		*/
									LenString_int		,	/*8*/	/*LEN_STRING_PAR_TYP		*/
#else
									NULL, 					/*0*/
									NULL, 					/*1*/
									NULL, 					/*2*/
									NULL, 					/*3*/
									NULL, 					/*4*/	 
									NULL, 					/*5*/
									NULL, 					/*6*/
									NULL, 					/*7*/
									NULL, 					/*8*/
#endif
#if defined(IP_CFG_BOOL) && defined(IP_CFG_STRING)
									bool_to_string		,	/*9*/	/*BOOL_TO_STRING_PAR*/		 
#else
									NULL, 					/*9*/
#endif
#if defined(IP_CFG_BYTE) && defined(IP_CFG_STRING)
									byte_to_string		,	/*10*/	/*BYTE_TO_STRING_PAR*/
#else
									NULL, 					/*10*/
#endif
#if (defined(IP_CFG_WORD) || defined(IP_CFG_UINT)) && defined(IP_CFG_STRING)
									word_to_string		,	/*11*/	/*WORD_TO_STRING_PAR*/
#else
									NULL, 					/*11*/
#endif
#if (defined(IP_CFG_DWORD) || defined(IP_CFG_UDINT)) && defined(IP_CFG_STRING)
									dword_to_string 	,	/*12*/	/*DWORD_TO_STRING_PAR*/
#else
									NULL, 					/*12*/
#endif
#if defined(IP_CFG_INT) && defined(IP_CFG_STRING)
									int_to_string		,	/*13*/	/*INT_TO_STRING_PAR*/	
#else
									NULL, 					/*13*/
#endif
#if defined(IP_CFG_DINT) && defined(IP_CFG_STRING)
									dint_to_string		,	/*14*/	/*DINT_TO_STRING_PAR*/
#else
									NULL, 					/*14*/
#endif
#if defined(IP_CFG_REAL) && defined(IP_CFG_STRING)
									real_to_string		,	/*15*/	/*REAL_TO_STRING_PAR*/
#else
									NULL, 					/*15*/	 
#endif
#if defined(IP_CFG_LREAL) && defined(IP_CFG_STRING)
									lreal_to_string 	,	/*16*/	/*LREAL_TO_STRING_PAR*/
#else
									NULL, 					/*16*/
#endif
#if defined(IP_CFG_BOOL) && defined(IP_CFG_STRING)
									string_to_bool		,	/*17*/	/*STRING_TO_BOOL*/
#else
									NULL, 					/*17*/
#endif
#if defined(IP_CFG_BYTE) && defined(IP_CFG_STRING)
									string_to_byte		,	/*18*/	/*STRING_TO_BYTE*/
#else
									NULL, 					/*18*/
#endif
#if (defined(IP_CFG_WORD) || defined(IP_CFG_UINT)) && defined(IP_CFG_STRING)
									string_to_word		,	/*19*/	/*STRING_TO_WORD*/
#else
									NULL, 					/*19*/
#endif
#if (defined(IP_CFG_DWORD) || defined(IP_CFG_UDINT)) && defined(IP_CFG_STRING)
									string_to_dword 	,	/*20*/	/*STRING_TO_DWORD*/
#else
									NULL,
#endif
#if defined(IP_CFG_INT) && defined(IP_CFG_STRING)
									string_to_int		,	/*21*/	/*STRING_TO_INT*/
#else
									NULL, 					/*21*/
#endif
#if defined(IP_CFG_DINT) && defined(IP_CFG_STRING)
									string_to_dint		,	/*22*/	/*STRING_TO_DINT*/ 
#else
									NULL, 					/*22*/
#endif
#if defined(IP_CFG_REAL) && defined(IP_CFG_STRING)
									string_to_real		,	/*23*/	/*STRING_TO_REAL*/
#else
									NULL, 					/*23*/
#endif
#if defined(IP_CFG_LREAL) && defined(IP_CFG_STRING)
									string_to_lreal 	,	/*24*/	/*STRING_TO_LREAL*/
#else
									NULL, 					/*24*/
#endif									  
#if defined(IP_CFG_TIME) && defined(IP_CFG_STRING)
									string_to_time		,	/*25*/
#else
									NULL, 					/*25*/
#endif
#if defined(IP_CFG_REAL)
									iec_sin_real		,	/*26*/	/*SIN_REAL_PAR*/
#else
									NULL, 					/*26*/
#endif
#if defined(IP_CFG_LREAL)
									iec_sin_lreal		,	/*27*/	/*SIN_LREAL_PAR*/
#else
									NULL, 					/*27*/
#endif
#if defined(IP_CFG_REAL)
									iec_asin_real		,	/*28*/	/*ASIN_REAL_PAR*/
#else
									NULL, 					/*28*/
#endif
#if defined(IP_CFG_LREAL)
									iec_asin_lreal		,	/*29*/	/*ASIN_LREAL_PAR*/
#else
									NULL, 					/*29*/
#endif
#if defined(IP_CFG_REAL)
									iec_cos_real		,	/*30*/	/*COS_REAL_PAR*/
#else
									NULL, 					/*30*/	
#endif
#if defined(IP_CFG_LREAL)
									iec_cos_lreal		,	/*31*/	/*COS_LREAL_PAR*/
#else
									NULL, 					/*31*/
#endif
#if defined(IP_CFG_REAL)
									iec_acos_real		,	/*32*/	/*ASIN_REAL_PAR*/
#else
									NULL, 					/*32*/
#endif
#if defined(IP_CFG_LREAL)
									iec_acos_lreal		,	/*33*/	/*ACOS_LREAL_PAR*/
#else
									NULL, 					/*33*/
#endif
#if defined(IP_CFG_REAL)
									iec_tan_real		,	/*34*/	/*TAN_REAL_PAR*/
#else
									NULL, 					/*34*/
#endif
#if defined(IP_CFG_LREAL)
									iec_tan_lreal		,	/*35*/	/*TAN_LREAL_PAR*/
#else
									NULL, 					/*35*/
#endif
#if defined(IP_CFG_REAL)
									iec_atan_real		,	/*36*/	/*ATAN_REAL_PAR*/
#else
									NULL, 					/*36*/
#endif
#if defined(IP_CFG_LREAL)
									iec_atan_lreal		,	/*37*/	/*ATAN_LREAL_PAR*/
#else
									NULL, 					/*37*/
#endif
#if defined(IP_CFG_REAL)
									iec_ln_real 		,	/*38*/	/*LN_REAL_PAR*/
#else
									NULL, 					/*38*/
#endif
#if defined(IP_CFG_LREAL)
									iec_ln_lreal		,	/*39*/	/*LN_LREAL_PAR*/
#else
									NULL, 					/*39*/
#endif
#if defined(IP_CFG_REAL)
									iec_log_real		,	/*40*/	/*LOG_REAL_PAR*/
#else
									NULL, 					/*40*/
#endif
#if defined(IP_CFG_LREAL)
									iec_log_lreal		,	/*41*/	/*LOG_LREAL_PAR*/
#else
									NULL, 					/*41*/
#endif
#if defined(IP_CFG_REAL)
									iec_sqrt_real		,	/*42*/	/*SQRT_REAL_PAR*/
#else
									NULL, 					/*42*/
#endif
#if defined(IP_CFG_LREAL)
									iec_sqrt_lreal		,	/*43*/	/*SQRT_LREAL_PAR*/
#else
									NULL, 					/*43*/
#endif
#if defined(IP_CFG_REAL)
									iec_exp_real		,	/*44*/	/*EXP_REAL_PAR*/
#else
									NULL, 					/*44*/
#endif
#if defined(IP_CFG_LREAL)
									iec_exp_lreal		,	/*45*/	/*EXP_LREAL_PAR*/
#else
									NULL, 					/*45*/
#endif
#if defined(IP_CFG_STRING) && defined(IP_CFG_TIME)
									time32_to_string	,	/*46*/	/*TIME_TO_STRING_PAR*/
#else
									NULL, 					/*46*/
#endif
#if defined(IP_CFG_BOOL)
									mux_bit 			,	/*47*/	/*MUX_BIT_PAR_TYP*/
#else
									NULL, 					/*47*/
#endif
#if defined(IP_CFG_BYTE)
									mux_byte			,	/*48*/	/*MUX_BIT_PAR_TYP*/
#else
									NULL, 					/*48*/
#endif
#if defined(IP_CFG_INT)
									mux_int 			,	/*49*/	/*MUX_INT_PAR_TYP*/
#else
									NULL, 					/*49*/
#endif
#if defined(IP_CFG_DINT)
									mux_dint			,	/*50*/	/*MUX_DINT_PAR_TYP*/
#else
									NULL, 					/*50*/
#endif
#if defined(IP_CFG_STRING)
									mux_string			,	/*51*/	 /*MUX_STRING_PAR_TYP*/ 
#else
									NULL, 					/*51*/
#endif
#if defined(IP_CFG_LREAL)
									mux_64bit			,	/*52*/	 /*MUX_64BIT_PAR_TYP*/			  
#else
									NULL, 					/*52*/
#endif
									NULL, 					/*53*/	/* free indexes*/
									NULL, 					/*54*/
									NULL, 					/*55*/
									NULL, 					/*56*/
									NULL, 					/*57*/
									NULL, 					/*58*/
									NULL, 					/*59*/
									NULL, 					/*60*/
									NULL, 					/*61*/
									NULL, 					/*62*/
									NULL, 					/*63*/
									NULL, 					/*64*/
									NULL, 					/*65*/
									NULL, 					/*66*/
									NULL, 					/*67*/
									NULL, 					/*68*/
									NULL, 					/*69*/
									NULL, 					/*70*/
									NULL, 					/*71*/
#if defined(RTS_CFG_SFC)
					   sfc_calcdotrans, 					/*72*/
						 sfc_calcdoact, 					/*73*/
						  sfc_finalise, 					/*74*/
#else
									NULL, 					/*72*/
									NULL, 					/*73*/
									NULL, 					/*74*/
#endif
#if defined(IP_CFG_REAL) && defined(IP_CFG_DINT)
					   trunc_real_dint, 					/*75*/ /*TRUNC_REAL_DINT_PAR_TYP*/
#else
									NULL, 					/*75*/
#endif
#if defined(IP_CFG_LREAL) && defined(IP_CFG_DINT)
					  trunc_lreal_dint, 					/*76*/ /*TRUNC_LREAL_DINT_PAR_TYP*/
#else
									NULL, 					/*76*/
#endif
									NULL, 					/*77*/ /* free indexes*/
									NULL, 					/*78*/
									NULL, 					/*79*/


/* ---------------------------------------------------------------------------- */

