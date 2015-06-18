
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
 * Filename: funUtil.h
 */


#if defined(RTS_CFG_UTILITY_LIB)

#if defined(IP_CFG_DWORD) && defined(IP_CFG_BYTE)
									copy_dword_from_byte_array	,	/*80*/
									copy_dword_to_byte_array	,	/*81*/
#else
									NULL, 							/*80*/
									NULL, 							/*81*/
#endif
#if defined(IP_CFG_BYTE) && defined(IP_CFG_LWORD)
									copy_lword_from_byte_array	,	/*82*/
									copy_lword_to_byte_array	,	/*83*/
#else
									NULL, 							/*82*/
									NULL, 							/*83*/
#endif
#if defined(IP_CFG_BYTE) && defined(IP_CFG_DINT)
									copy_dint_to_byte_array 	,	/*84*/
									copy_dint_from_byte_array	,	/*85*/
#else
									NULL, 							/*84*/
									NULL, 							/*85*/
#endif
#if defined(IP_CFG_LINT) && defined(IP_CFG_BYTE)
									copy_lint_to_byte_array 	,	/*86*/
									copy_lint_from_byte_array	,	/*87*/
#else
									NULL, 							/*86*/
									NULL, 							/*87*/
#endif
#if defined(IP_CFG_LREAL) && defined(IP_CFG_BYTE)
									copy_lreal_to_byte_array	,	/*88*/
									copy_lreal_from_byte_array	,	/*89*/
#else
									NULL, 							/*88*/
									NULL, 							/*89*/
#endif
#if defined(IP_CFG_BOOL) && defined(IP_CFG_BYTE)
									copy_bool_to_byte_array 	,	/*90*/
									copy_bool_from_byte_array	,	/*91*/
#else
									NULL, 							/*90*/
									NULL, 							/*91*/
#endif
#if defined(IP_CFG_TIME) && defined(IP_CFG_BYTE)
									copy_time_from_byte_array	,	/*92*/
									copy_time_to_byte_array 	,	/*93*/	  
#else
									NULL, 							/*92*/
									NULL, 							/*93*/
#endif
#if defined(IP_CFG_BYTE)
									copy_byte_from_byte_array	,	/*94*/
									copy_byte_to_byte_array 	,	/*95*/	  
#else
									NULL, 							/*94*/
									NULL, 							/*95*/
#endif
#if defined(IP_CFG_WORD) && defined(IP_CFG_BYTE)
									copy_word_from_byte_array	,	/*96*/
									copy_word_to_byte_array 	,	/*97*/
#else
									NULL, 							/*96*/
									NULL, 							/*97*/
#endif
#if defined(IP_CFG_STRING) && defined(IP_CFG_BYTE)
									copy_string_from_byte_array ,	/*98*/
									copy_string_to_byte_array	,	/*99*/
#else
									NULL, 							/*98*/
									NULL, 							/*99*/
#endif
#if defined(IP_CFG_BYTE) && defined(IP_CFG_INT)
									copy_int_to_byte_array	   ,	/*100*/
									copy_int_from_byte_array   ,	/*101*/
#else
									NULL, 							/*100*/
									NULL, 							/*101*/
#endif
#if defined(IP_CFG_REAL) && defined(IP_CFG_BYTE)
									copy_real_to_byte_array    ,	/*102*/
									copy_real_from_byte_array  ,	/*103*/
#else
									NULL, 							/*102*/
									NULL, 							/*103*/
#endif
#if defined(IP_CFG_WORD)
									byte_swap_word				,	/*104*/
#else
									NULL, 							/*104*/
#endif
#if defined(IP_CFG_DWORD)
									byte_swap_dword 			,	/*105*/
#else
									NULL, 							/*105*/
#endif
#if defined(IP_CFG_LWORD)
									byte_swap_lword 			,	/*106*/
#else
									NULL, 							/*106*/
#endif
#if defined(IP_CFG_BYTE) && defined(IP_CFG_WORD)
									hi_byte 					,	/*107*/
#else
									NULL, 							/*107*/
#endif
#if defined(IP_CFG_WORD) && defined(IP_CFG_DWORD)
									hi_word 					,	/*108*/
#else
									NULL, 							/*108*/
#endif
#if defined(IP_CFG_DWORD) && defined(IP_CFG_LWORD)
									hi_dword					,	/*109*/
#else
									NULL, 							/*109*/
#endif
#if defined(IP_CFG_BYTE) && defined(IP_CFG_WORD)
									lo_byte 					,	/*110*/
#else
									NULL, 							/*110*/
#endif
#if defined(IP_CFG_WORD) && defined(IP_CFG_DWORD)
									lo_word 					,	/*111*/
#else
									NULL, 							/*111*/
#endif
#if defined(IP_CFG_DWORD) && defined(IP_CFG_LWORD)
									lo_dword					,	/*112*/
#else
									NULL, 							/*112*/
#endif
#if defined(IP_CFG_BYTE) && defined(IP_CFG_WORD)
									make_word					,	/*113*/
#else
									NULL, 							/*113*/
#endif
#if defined(IP_CFG_WORD) && defined(IP_CFG_DWORD)
									make_dword					,	/*114*/
#else
									NULL, 							/*114*/
#endif
#if defined(IP_CFG_DWORD) && defined(IP_CFG_LWORD)
									make_lword					,	/*115*/
#else
									NULL, 							/*115*/
#endif

									NULL, 							/*116*/
									NULL, 							/*117*/
									NULL, 							/*118*/
									NULL, 							/*119*/

#else	/* RTS_CFG_UTILITY_LIB */

									NULL, 							/*80*/
									NULL, 							/*81*/
									NULL, 							/*82*/
									NULL, 							/*83*/
									NULL, 							/*84*/
									NULL, 							/*85*/
									NULL, 							/*86*/
									NULL, 							/*87*/
									NULL, 							/*88*/
									NULL, 							/*89*/
									NULL, 							/*90*/
									NULL, 							/*91*/
									NULL, 							/*92*/
									NULL, 							/*93*/
									NULL, 							/*94*/
									NULL, 							/*95*/
									NULL, 							/*96*/
									NULL, 							/*97*/
									NULL, 							/*98*/
									NULL, 							/*99*/
									NULL, 							/*100*/
									NULL, 							/*101*/
									NULL, 							/*102*/
									NULL, 							/*103*/
									NULL, 							/*104*/
									NULL, 							/*105*/
									NULL, 							/*106*/
									NULL, 							/*107*/
									NULL, 							/*108*/
									NULL, 							/*109*/
									NULL, 							/*110*/
									NULL, 							/*111*/
									NULL, 							/*112*/
									NULL, 							/*113*/
									NULL, 							/*114*/
									NULL, 							/*115*/
									NULL, 							/*116*/
									NULL, 							/*117*/
									NULL, 							/*118*/
									NULL, 							/*119*/

#endif	/* RTS_CFG_UTILITY_LIB */

/* ---------------------------------------------------------------------------- */
