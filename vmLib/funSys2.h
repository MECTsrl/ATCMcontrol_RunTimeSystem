
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
 * Filename: funSys2.h
 */


#if defined(RTS_CFG_SYSTEM_LIB_NT)

									EVT_GetException		,	/* 135 */
									EVT_Set 				,	/* 136 */
									NULL					,	/* 137 */
									MSG_SendToEng			,	/* 138 */
									MSG_Trace				,	/* 139 */
									NULL /* inline !*/		,	/* 140 */
									TIM_Get 				,	/* 141 */
									TSK_ClearError			,	/* 142 */
									TSK_Exception			,	/* 143 */
									TSK_GetCount			,	/* 144 */
									TSK_GetError			,	/* 145 */
									TSK_GetInfo 			,	/* 146 */
									TSK_GetMyNumber 		,	/* 147 */
									TSK_GetName 			,	/* 148 */
									TSK_GetState			,	/* 149 */
									TSK_Start				,	/* 150 */
									TSK_Stop				,	/* 151 */
									WD_Disable				,	/* 152 */
									WD_Enable				,	/* 153 */
									NULL					,	/* 154 */
									NULL					,	/* 155 */
									EVT_GetEvent			,	/* 156 */
									WD_IsEnabled			,	/* 157 */
									TSK_GetMyName			,	/* 158 */
									SYS_Coldstart			,	/* 159 */

#else	/* RTS_CFG_SYSTEM_LIB_NT */

									NULL					,	/* 135 */
									NULL					,	/* 136 */
									NULL					,	/* 137 */
									NULL					,	/* 138 */
									NULL					,	/* 139 */
									NULL					,	/* 140 */
									NULL					,	/* 141 */
									NULL					,	/* 142 */
									NULL					,	/* 143 */
									NULL					,	/* 144 */
									NULL					,	/* 145 */
									NULL					,	/* 146 */
									NULL					,	/* 147 */
									NULL					,	/* 148 */
									NULL					,	/* 149 */
									NULL					,	/* 150 */
									NULL					,	/* 151 */
									NULL					,	/* 152 */
									NULL					,	/* 153 */
									NULL					,	/* 154 */
									NULL					,	/* 155 */
									NULL					,	/* 156 */
									NULL					,	/* 157 */
									NULL					,	/* 158 */
									NULL					,	/* 159 */

#endif	/* RTS_CFG_SYSTEM_LIB_NT */

/* ---------------------------------------------------------------------------- */

