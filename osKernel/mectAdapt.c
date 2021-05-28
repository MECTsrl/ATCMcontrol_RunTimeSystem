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
 * Filename: mectAdapt.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"mectAdapt.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "../inc/stdInc.h"

#include "../inc.fc/fcDef.h"
#include "../inc.data/dataMain.h"
#include "../inc.mect/mectMain.h"

#include <sys/mman.h>
#include <fcntl.h>

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/**
 * IO_Layer_Data
 *
 */
#if defined(RTS_CFG_IODAT)

void *IO_Layer_DAT(void *lpParam)
{
	int iRes = osPthreadSetSched(FC_SCHED_IO_DAT, FC_PRIO_IO_DAT);
    if (iRes != 0) {
		return NULL;
    }

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d.\r\n", TASK_NAME_IOL_DAT, getpid());
#endif

	dataMain(lpParam);

#if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' terminated with pid %d.\r\n", TASK_NAME_IOL_DAT, getpid());
#endif

	pthread_detach(pthread_self());

	return NULL;
}
#endif /* RTS_CFG_IODAT */

