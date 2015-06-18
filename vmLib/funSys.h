
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
 * Filename: funSys.h
 */


#if defined(RTS_CFG_SYSTEM_LIB)

									ClearLocalTaskErrno 	,	/*120*/
									ClearTaskErrno			,	/*121*/
									NULL					,	/*122*/
									GetLocalTaskErrno		,	/*123*/
									GetLocalTaskInfo		,	/*124*/
									GetLocalTaskState		,	/*125*/
									GetTaskErrno			,	/*126*/
									GetTimeSinceSystemBoot	,	/*127*/
									OutputDebugString		,	/*128*/
									StartLocalTask			,	/*129*/
									StopLocalTask			,	/*130*/
									ThrowException			,	/*131*/
									NULL, 						/*132*/
									NULL, 						/*133*/
									NULL, 						/*134*/

#else	/* RTS_CFG_SYSTEM_LIB */

									NULL, 						/*120*/
									NULL, 						/*121*/
									NULL, 						/*122*/
									NULL, 						/*123*/
									NULL, 						/*124*/
									NULL, 						/*125*/
									NULL, 						/*126*/
									NULL, 						/*127*/
									NULL, 						/*128*/
									NULL, 						/*129*/
									NULL, 						/*130*/
									NULL, 						/*131*/
									NULL, 						/*132*/
									NULL, 						/*133*/
									NULL, 						/*134*/

#endif	/* RTS_CFG_SYSTEM_LIB */

/* ---------------------------------------------------------------------------- */

