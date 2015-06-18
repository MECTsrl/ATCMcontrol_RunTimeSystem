
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
 * Filename: funMect.h
 */


#if defined(RTS_CFG_MECT_LIB)
									MECT_sread, /* 230 */
									MECT_H_sread, /* 231 */
									MECT_swrite, /* 232 */
									MECT_H_swrite, /* 233 */
									MECT_get_enquiry, /* 234 */
									MECT_get_flag, /* 235 */
									MECT_get_status, /* 236 */
									NULL, /* 237 */
									NULL, /* 238 */
									NULL, /* 239 */

#else	/* RTS_CFG_MECT_LIB */

									NULL,	/* 230 */
									NULL,	/* 231 */
									NULL,	/* 232 */
									NULL,	/* 233 */
									NULL,	/* 234 */
									NULL,	/* 235 */
									NULL,	/* 236 */
									NULL,	/* 237 */
									NULL,	/* 238 */
									NULL,	/* 239 */

#endif	/* RTS_CFG_MECT_LIB */
/* ---------------------------------------------------------------------------- */

