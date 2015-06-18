
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
 * Filename: funFile.h
 */


#if defined(RTS_CFG_FILE_LIB)

									fa_sync_openFile		,	/* 160 */
									fa_sync_readFile		,	/* 161 */
									fa_sync_renameFile		,	/* 162 */
									fa_sync_writeFile		,	/* 163 */
									fa_sync_closeFile		,	/* 164 */
									fa_sync_createDirectory ,	/* 165 */
									fa_sync_deleteFile		,	/* 166 */
									fa_sync_existsFile		,	/* 167 */
									fa_sync_getSize 		,	/* 168 */
									fa_get_diskFreeSpace	,	/* 169 */
									fa_error_string 		,	/* 170 */
									fa_sync_arrayReadFile	,	/* 171 */
									fa_sync_arrayWriteFile	,	/* 172 */
									fa_flush				,	/* 173 */
									NULL					,	/* 174 */
									NULL					,	/* 175 */
									NULL					,	/* 176 */
									NULL					,	/* 177 */
									NULL					,	/* 178 */
									NULL					,	/* 179 */

#else	/* RTS_CFG_FILE_LIB */

									NULL					,	/* 160 */
									NULL					,	/* 161 */
									NULL					,	/* 162 */
									NULL					,	/* 163 */
									NULL					,	/* 164 */
									NULL					,	/* 165 */
									NULL					,	/* 166 */
									NULL					,	/* 167 */
									NULL					,	/* 168 */
									NULL					,	/* 169 */
									NULL					,	/* 170 */
									NULL					,	/* 171 */
									NULL					,	/* 172 */
									NULL					,	/* 173 */
									NULL					,	/* 174 */
									NULL					,	/* 175 */
									NULL					,	/* 176 */
									NULL					,	/* 177 */
									NULL					,	/* 178 */
									NULL					,	/* 179 */


#endif	/* RTS_CFG_FILE_LIB */

/* ---------------------------------------------------------------------------- */

