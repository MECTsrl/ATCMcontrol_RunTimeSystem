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
 * Filename: funUSB.h
 */

#if defined(RTS_CFG_USB_LIB)
									Usb_off,      /* 240 */
									Usb_copy,     /* 241 */
									Usb_diskcopy, /* 242 */
									Usb_delete,   /* 243 */
									Usb_mkdir,    /* 244 */
									Usb_status,   /* 245 */
									Usb_feedback, /* 246 */
									NULL,         /* 247 */
									NULL,         /* 248 */
									NULL,	      /* 249 */
#else	/* RTS_CFG_USB_LIB */
									NULL,	/* 240 */
									NULL,	/* 241 */
									NULL,	/* 242 */
									NULL,	/* 243 */
									NULL,	/* 244 */
									NULL,	/* 245 */
									NULL,	/* 246 */
									NULL,	/* 247 */
									NULL,	/* 248 */
									NULL,	/* 249 */
#endif	/* RTS_CFG_USB_LIB */
/* ---------------------------------------------------------------------------- */

