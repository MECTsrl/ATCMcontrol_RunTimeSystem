
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
 * Filename: stdInc.h
 */


#ifndef _STDINC_H_
#define _STDINC_H_

/* This file must be included before all other include files in the VM. The 
 * following header files must be included in the the given order!
 */

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

/* Included in the run time system
 */
#define INC_RTS

#include "BuildNr.h"

#ifndef _OSFIRST_H_
#include "osFirst.h"
#endif

#ifndef _OSDEF_H_
#include "osDef.h"
#endif

#ifndef _OSTARGET_H_
#include "osTarget.h"
#endif

#ifndef _VMSHARED_H_
#include "vmShared.h"
#endif

#ifndef _VMMDEF_H_
#include "vmmDef.h"
#endif

#ifndef _INTDEF_H_
#include "intDef.h"
#endif

#ifndef _LIBDEF_H_
#include "libDef.h"
#endif

#ifndef _VMMMAIN_H_
#include "vmmMain.h"
#endif

#ifndef _OSLAST_H_
#include "osLast.h"
#endif

#endif	/* _STDINC_H_ */

/* ---------------------------------------------------------------------------- */
