
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
 * Filename: libSfc.h
 */


#ifndef _INTSFC_H_
#define _INTSFC_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if defined(RTS_CFG_SFC)

/* Don't use! Just to replace former OS_BOOL (16 bit) until it is safe
 * to replace it with IEC_BOOL (8 bit).
 */
#define SFC_BOOL IEC_INT


/* Function Declarations
 * ----------------------------------------------------------------------------
 */

void sfc_calcdotrans (STDLIBFUNCALL);		 /* ordinal 72 */
void sfc_calcdoact	 (STDLIBFUNCALL);		 /* ordinal 73 */
void sfc_finalise	 (STDLIBFUNCALL);		 /* ordinal 74 */

#endif	/* RTS_CFG_SFC */

#endif	/* _INTSFC_H_ */

/* ---------------------------------------------------------------------------- */

