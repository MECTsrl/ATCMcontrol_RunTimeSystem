
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
 * Filename: funSys22.h
 */


#if defined(RTS_CFG_SYSTEM_LIB_NT)

									SYS_Reboot				,	/* 180 */
									SYS_RetainWrite 		,	/* 181 */
									SYS_Warmstart			,	/* 182 */
									LD_Get					,	/* 183 */
									WD_SetTrigger			,	/* 184 */
									WD_GetTrigger			,	/* 185 */
									TSK_GetStatistic		,	/* 186 */
									TSK_ClearStatistic		,	/* 187 */
									NULL					,	/* 188 */
									NULL					,	/* 189 */
									NULL					,	/* 190 */
									NULL					,	/* 191 */
									NULL					,	/* 192 */
									NULL					,	/* 193 */
									NULL					,	/* 194 */
									NULL					,	/* 195 */
									NULL					,	/* 196 */
									NULL					,	/* 197 */
									NULL					,	/* 198 */
									NULL					,	/* 199 */
									
#else	/* RTS_CFG_SYSTEM_LIB_NT */

									NULL					,	/* 180 */
									NULL					,	/* 181 */
									NULL					,	/* 182 */
									NULL					,	/* 183 */
									NULL					,	/* 184 */
									NULL					,	/* 185 */
									NULL					,	/* 186 */
									NULL					,	/* 187 */
									NULL					,	/* 188 */
									NULL					,	/* 189 */
									NULL					,	/* 190 */
									NULL					,	/* 191 */
									NULL					,	/* 192 */
									NULL					,	/* 193 */
									NULL					,	/* 194 */
									NULL					,	/* 195 */
									NULL					,	/* 196 */
									NULL					,	/* 197 */
									NULL					,	/* 198 */
									NULL					,	/* 199 */

#endif	/* RTS_CFG_SYSTEM_LIB_NT */

/* ---------------------------------------------------------------------------- */

