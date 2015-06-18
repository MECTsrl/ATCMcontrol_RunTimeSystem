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
 * Filename: funDatalog.h
 */

#if defined(RTS_CFG_DATALOG_LIB)
									Datalog_get,  /* 250 */
									Datalog_stop, /* 251 */
									Datalog_eject,/* 252 */
									NULL,   /* 253 */
									NULL,   /* 254 */
									NULL,   /* 255 */
									NULL,   /* 256 */
									NULL,   /* 257 */
									NULL,   /* 258 */
									NULL,   /* 259 */
#else	/* RTS_CFG_DATALOG_LIB */
									NULL,	/* 250 */
									NULL,	/* 251 */
									NULL,	/* 252 */
									NULL,	/* 253 */
									NULL,	/* 254 */
									NULL,	/* 255 */
									NULL,	/* 256 */
									NULL,	/* 257 */
									NULL,	/* 258 */
									NULL,	/* 259 */
#endif	/* RTS_CFG_DATALOG_LIB */
/* ---------------------------------------------------------------------------- */

