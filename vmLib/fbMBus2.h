
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
 * Filename: fbMBus2.h
 */


#if defined(RTS_CFG_MBUS2_LIB)

									fb_mbus2_meter_init			,	/* 30 */
									fb_mbus2_meter_list_slaves	,	/* 31 */
									fb_mbus2_meter_read			,	/* 32 */
									NULL						,	/* 33 */
									NULL						,	/* 34 */
									NULL						,	/* 35 */
									NULL						,	/* 36 */
									NULL						,	/* 37 */
									NULL						,	/* 38 */
									NULL						,	/* 39 */

#else	/* RTS_CFG_MBUS2_LIB */

									NULL						,	/* 30 */
									NULL						,	/* 31 */
									NULL						,	/* 32 */
									NULL						,	/* 33 */
									NULL						,	/* 34 */
									NULL						,	/* 35 */
									NULL						,	/* 36 */
									NULL						,	/* 37 */
									NULL						,	/* 38 */
									NULL						,	/* 39 */

#endif	/* RTS_CFG_MBUS2_LIB */

/* ---------------------------------------------------------------------------- */

