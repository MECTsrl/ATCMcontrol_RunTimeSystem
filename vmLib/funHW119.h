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
 * Filename: funHW119.h
 */

#if defined(RTS_CFG_HW119_LIB)
									hw119_open_cross_table,        /* 280 */
									hw119_close_cross_table,       /* 281 */
									hw119_read_cross_table_record, /* 282 */
									hw119_get_cross_table_field,   /* 283 */
									hw119_get_addr,                /* 284 */
									hw119_float2dword,             /* 285 */
									hw119_dword2float,             /* 286 */
									hw119_write_var_bit,           /* 287 */
									hw119_write_var_byte,          /* 288 */
									hw119_write_var_uint,          /* 289 */
									hw119_write_var_int,           /* 290 */
									hw119_write_var_udint,         /* 291 */
									hw119_write_var_dint,          /* 292 */
									hw119_write_var_real,          /* 293 */
									NULL,                         /* 294 */
									NULL,                         /* 295 */
									NULL,                         /* 296 */
									NULL,                         /* 297 */
									NULL,                         /* 298 */
									NULL,                         /* 299 */
#else	/* RTS_CFG_HW119_LIB */
									NULL,	/* 280 */
									NULL,	/* 281 */
									NULL,	/* 282 */
									NULL,	/* 283 */
									NULL,	/* 284 */
									NULL,	/* 285 */
									NULL,	/* 286 */
									NULL,	/* 287 */
									NULL,	/* 288 */
									NULL,	/* 289 */
									NULL,	/* 290 */
									NULL,	/* 291 */
									NULL,	/* 292 */
									NULL,	/* 293 */
									NULL,	/* 294 */
									NULL,	/* 295 */
									NULL,	/* 296 */
									NULL,	/* 297 */
									NULL,	/* 298 */
									NULL,	/* 299 */
#endif	/* RTS_CFG_HW119_LIB */
/* ---------------------------------------------------------------------------- */

