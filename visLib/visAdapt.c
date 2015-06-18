
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
 * Filename: visAdapt.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"visAdapt.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */
#include "osSocket.h"

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * domVisuCommInit	
 *
 */
IEC_UINT domVisuCommInit(void)
{
	static IEC_BOOL bOnce = FALSE;
	
	if (bOnce == FALSE)
	{

		bOnce = TRUE;
	}
	
	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
