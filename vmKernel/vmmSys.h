
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
 * Filename: vmmSys.h
 */


#ifndef _VMMSYS_H_
#define _VMMSYS_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif


/* Features
 * ----------------------------------------------------------------------------
 */
#define RTS_FB_LIC_MASK 	0x0fffu

#define RTS_FB_LIC_PROFI	0x0001u
#define RTS_FB_LIC_BAC		0x0002u
#define RTS_FB_LIC_MODRTU	0x0004u
#define RTS_FB_LIC_MODIP	0x0008u
#define RTS_FB_LIC_MBUS 	0x0010u
#define RTS_FB_LIC_LON		0x0020u
#define RTS_FB_LIC_EIB		0x0040u
#define RTS_FB_LIC_FMS		0x0080u
#define RTS_FB_LIC_RES2 	0x0100u
#define RTS_FB_LIC_RES3 	0x0200u
#define RTS_FB_LIC_RES4 	0x0400u
#define RTS_FB_LIC_WEBVIS	0x0800u


/* Product Types
 * ----------------------------------------------------------------------------
 */
#define RTS_PROD_MASK		0xf000u

#define RTS_PROD_FC 		0x1000u
#define RTS_PROD_BC 		0x2000u
#define RTS_PROD_PC 		0x3000u
#define RTS_PROD_DC 		0x4000u
#define RTS_PROD_GA			0x5000u

#endif	/* _VMMSYS_H_ */

/* ---------------------------------------------------------------------------- */
