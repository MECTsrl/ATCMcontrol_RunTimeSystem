
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
 * Filename: osTarget.h
 */


#ifndef _OSTARGET_H_
#define _OSTARGET_H_

/* osTarget.h
 * ----------------------------------------------------------------------------
 * Header file for additional, target specific definitions. This file is 
 * included after the main configuration header file (osDef.h).
 */

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif

/* Global Include Files
 * ----------------------------------------------------------------------------
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>
#include <pthread.h>
#include <unistd.h>


/* Target specific definitions
 * ----------------------------------------------------------------------------
 */
#define RTS_CFG_IODAT

#define IOEXT_DAT "ioData"
#define IOID_DAT 2 /* see the file inc/vmmDef.h*/
#define TASK_NAME_IOL_DAT "IOL_DAT"

#endif /* _OSTARGET_H_ */

/* ---------------------------------------------------------------------------- */
