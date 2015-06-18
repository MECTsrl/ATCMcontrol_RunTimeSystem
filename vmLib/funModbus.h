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
 * Filename: funModbus.h
 */

#if defined(RTS_CFG_MODBUS_LIB)
	mb_set_slave,					/* 300 */
	mb_get_response_timeout,		/* 301 */
	mb_set_response_timeout,        /* 302 */
	mb_get_byte_timeout,        	/* 303 */
	mb_set_byte_timeout,			/* 304 */
	mb_get_header_length,			/* 305 */
	mb_connect,						/* 306 */
	mb_close,						/* 307 */
	mb_free,						/* 308 */
	mb_flush,						/* 309 */
	mb_read_bits,					/* 310 */
	mb_read_input_bits,				/* 311 */
	mb_read_registers,				/* 312 */
	mb_read_input_registers,		/* 313 */
	mb_write_bit,					/* 314 */
	mb_write_register,				/* 315 */
	mb_write_bits,					/* 316 */
	mb_write_registers,				/* 317 */
	mb_mask_write_register,			/* 318 */
	mb_write_and_read_registers,	/* 319 */
	mb_report_slave_id,				/* 320 */
	mb_new_tcp,						/* 321 */
	mb_tcp_listen,					/* 322 */
	mb_tcp_accept,					/* 323 */
	mb_new_tcp_pi,					/* 324 */
	mb_tcp_pi_listen,				/* 325 */
	mb_tcp_pi_accept,				/* 326 */
	mb_new_tcprtu,					/* 327 */
	mb_tcprtu_listen,				/* 328 */
	mb_tcprtu_accept,				/* 329 */
	mb_new_rtu,						/* 330 */
	mb_rtu_set_serial_mode,			/* 331 */
	mb_rtu_get_serial_mode,			/* 332 */
	mb_rtu_get_rts,					/* 333 */
	mb_rtu_set_rts,					/* 334 */
	mb_set_bits_from_byte,			/* 335 */
	mb_set_bits_from_bytes,			/* 336 */
	mb_get_byte_from_bits,			/* 337 */
	mb_get_float,					/* 338 */
	mb_get_float_dcba,				/* 339 */
	mb_get_float_badc,				/* 340 */
	mb_get_float_cdab,				/* 341 */
	mb_set_float,					/* 342 */
	mb_set_float_dcba,				/* 343 */
	mb_set_float_badc,				/* 344 */
	mb_set_float_cdab,				/* 345 */
	mb_set_error_recovery,          /* 346 */
	NULL,   /* 347 */
	NULL,   /* 348 */
	NULL,   /* 349 */
#else	/* RTS_CFG_MODBUS_LIB */
	NULL,   /* 300 */
	NULL,   /* 301 */
	NULL,   /* 302 */
	NULL,   /* 303 */
	NULL,   /* 304 */
	NULL,   /* 305 */
	NULL,   /* 306 */
	NULL,   /* 307 */
	NULL,   /* 308 */
	NULL,   /* 309 */
	NULL,   /* 310 */
	NULL,   /* 311 */
	NULL,   /* 312 */
	NULL,   /* 313 */
	NULL,   /* 314 */
	NULL,   /* 315 */
	NULL,   /* 316 */
	NULL,   /* 317 */
	NULL,   /* 318 */
	NULL,   /* 319 */
	NULL,   /* 320 */
	NULL,   /* 321 */
	NULL,   /* 322 */
	NULL,   /* 323 */
	NULL,   /* 324 */
	NULL,   /* 325 */
	NULL,   /* 326 */
	NULL,   /* 327 */
	NULL,   /* 328 */
	NULL,   /* 329 */
	NULL,   /* 330 */
	NULL,   /* 331 */
	NULL,   /* 332 */
	NULL,   /* 333 */
	NULL,   /* 334 */
	NULL,   /* 335 */
	NULL,   /* 336 */
	NULL,   /* 337 */
	NULL,   /* 338 */
	NULL,   /* 339 */
	NULL,   /* 340 */
	NULL,   /* 342 */
	NULL,   /* 342 */
	NULL,   /* 343 */
	NULL,   /* 344 */
	NULL,   /* 345 */
	NULL,   /* 346 */
	NULL,   /* 347 */
	NULL,   /* 348 */
	NULL,   /* 349 */
#endif	/* RTS_CFG_MODBUS_LIB */
	/* ---------------------------------------------------------------------------- */

