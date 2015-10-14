
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
 * Filename: libDef.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"libDef.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* Declarations IEC Library
 */
#include "libIec.h" 

/* Declarations System Library
 */
#include "libSys.h"

/* Declarations Utility Library
 */
#include "libUtil.h"

/* Declarations SFC Library
 */
#include "libSfc.h"

/* Declarations System Library NT
 */
#include "libSys2.h"

/* Declarations File Library
 */
#include "libFile.h"

/* Declarations MBus2 Library
 */
#include "libMBus2.h"

/* Declarations BACnet Library
 */
#include "bacFun.h"

/* Declarations Mect Library
 */
#include "libMect.h"

/* Declarations USB Library
 */
#include "libUSB.h"

/* Declarations Datalogger Library
 */
#include "libDatalog.h"

/* Declarations Mect user utility Library
 */
#include "libMectUserUtility.h"

/* Declarations HW119 utility Library
 */
#include "libHW119.h"

/* Declarations Modbus utility Library
 */
#include "libModbus.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* Library function definitions
 * ----------------------------------------------------------------------------
 */
EXECUTE_FUN g_pLibraryFun[] = 
{ 
	/* libIec.c - IEC library
	 * ------------------------------------------------------------------------
	 */
	#include "funIec.h" 				/* Reserved Numbers:	0	-	79	*/

	/* libUtil.c - Utility library
	 * ------------------------------------------------------------------------
	 */
	#include "funUtil.h"				/* Reserved Numbers:	80	-	119 */

	/* libSys.c - System library
	 * ------------------------------------------------------------------------
	 */
	#include "funSys.h" 				/* Reserved Numbers:	120 -	134 */

	/* libSys2.c - System library NT
	 * ------------------------------------------------------------------------
	 */
	#include "funSys2.h"				/* Reserved Numbers:	135 -	169 */

	/* libFile.c - File Access library
	 * ------------------------------------------------------------------------
	 */
	#include "funFile.h"				/* Reserved Numbers:	160 -	179 */

	/* libSys2.c - System library NT
	 * ------------------------------------------------------------------------
	 */
	#include "funSys22.h"				/* Reserved Numbers:	180 -	199 */

#if 0
	/* funMBus2.c - M-Bus library NT
	 * ------------------------------------------------------------------------
	 */
	#include "funMBus2.h"				/* Reserved Numbers:	200 -	229 */

	/* libBac.c - BACnet
	 * ------------------------------------------------------------------------
	 */
	#include "funBac.h" 				/* Reserved Numbers:	230 -	239 */
#endif
	
	/* funMBRTU.c - MODBUS RTU library - MTL: now removed (2015-07-28).
	 * ------------------------------------------------------------------------
	 */

										/* Reserved Numbers:	200 -	229 */

	/* 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, */
	   NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	/* 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229 */
	   NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* funMect.c - Mect library 
	 * ------------------------------------------------------------------------
	 */
	#include "funMect.h"				/* Reserved Numbers:	230 -	239 */

	/* funUSB.c - USB library 
	 * ------------------------------------------------------------------------
	 */
	#include "funUSB.h"					/* Reserved Numbers:	240 -	249 */

	/* funDatalog.c - Datalogger library 
	 * ------------------------------------------------------------------------
	 */
	#include "funDatalog.h"				/* Reserved Numbers:	250 -	259 */

	/* funMectUserUtility.c - Datalogger library 
	 * ------------------------------------------------------------------------
	 */
	#include "funMectUserUtility.h"		/* Reserved Numbers:	260 -	279 */

	/* funHW119.c - Datalogger library 
	 * ------------------------------------------------------------------------
	 */
	#include "funHW119.h"				/* Reserved Numbers:	280 -	299 */

	/* funModbus.c - Datalogger library 
	 * ------------------------------------------------------------------------
	 */
	#include "funModbus.h"				/* Reserved Numbers:	300 -	359 */
};


/* Library function block definitions
 * ----------------------------------------------------------------------------
 */
EXECUTE_FB	g_pLibraryFB [] = 
{
	/* libIec.c - IEC library
	 * ------------------------------------------------------------------------
	 */
	#include "fbIec.h"					/* Reserved Numbers:	0	-	19	*/

	/* libUtil.c - Utility library
	 * ------------------------------------------------------------------------
	 */
	#include "fbUtil.h" 				/* Reserved Numbers:	20	-	24	*/

	/* libSys.c - System library
	 * ------------------------------------------------------------------------
	 */
	#include "fbSys.h"					/* Reserved Numbers:	25	-	29	*/

	/* libMBus2.c - M-Bus library NT
	 * ------------------------------------------------------------------------
	 */
	#include "fbMBus2.h"				/* Reserved Numbers:	30	-	39	*/

	/* libSys2.c - System library NT
	 * ------------------------------------------------------------------------
	 */
	#include "fbSys2.h" 				/* Reserved Numbers:		-		*/

	/* libFile.c - File Access library
	 * ------------------------------------------------------------------------
	 */
	#include "fbFile.h" 				/* Reserved Numbers:		-		*/

};

/* ---------------------------------------------------------------------------- */
/**
 * libGetFunCount
 *
 * @return			Number of system library functions in function
 *					array.
 */
IEC_UINT libGetFunCount()
{
	return sizeof(g_pLibraryFun) / sizeof(g_pLibraryFun[0]);
}

/* ---------------------------------------------------------------------------- */
/**
 * libGetFBCount
 *
 * @return			Number of system library function blocks in 
 *					function block array.
 */
IEC_UINT libGetFBCount()
{
	return sizeof(g_pLibraryFB) / sizeof(g_pLibraryFB[0]);
}

/* ---------------------------------------------------------------------------- */
/**
 * libSetError
 *
 * Sets a none-fatal error for this task. The task execution is 
 * continued normally.
 *
 * @return			OK always.
 */
IEC_UINT libSetError(STaskInfoVM *pVM, IEC_UINT uError)
{
	pVM->Local.pState->ulErrNo = uError;
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * libSetException
 *
 * Creates an exception for this task. (The task is stopped 
 * immediately after returning from this library call.)
 *
 * @return			OK always.
 */
IEC_UINT libSetException(STaskInfoVM *pVM, IEC_UINT uError)
{
	pVM->Local.Exception.uErrNo = uError;
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * libSubstring
 *
 * Gets a substring out of a given IEC string.
 * 
 * @return			Count of added bytes to output string.
 */
IEC_UDINT libSubstring(STaskInfoVM *pVM, IEC_STRING OS_DPTR *in, IEC_STRING OS_DPTR *out, IEC_UDINT startPos, IEC_UDINT endPos, IEC_BOOL bAppend)
{
	IEC_UDINT ulLen = 0;

	if (in == 0 || out == 0)
	{
		pVM->Local.Exception.uErrNo = EXCEPT_NULL_PTR;
		return 0;
	}

	if(startPos > endPos)
	{
		IEC_UDINT tmp = startPos;
		startPos = endPos;
		endPos	 = tmp;
	}
	
	if (bAppend == FALSE)
	{
		out->CurLen = 0;
	}

	if (startPos >= in->CurLen)
	{
		return out->CurLen;
	}

	ulLen = endPos - startPos > (IEC_UDINT)(out->MaxLen - out->CurLen) ? (IEC_UDINT)(out->MaxLen - out->CurLen) : (IEC_UDINT)(endPos - startPos);
	ulLen = startPos + ulLen > in->CurLen ? in->CurLen - startPos : ulLen;
		
	if (ulLen != 0)
	{
		OS_MEMCPY(out->Contents + out->CurLen, &in->Contents[startPos], ulLen);
		out->CurLen = (IEC_STRLEN)(out->CurLen + ulLen);
	}
	
	return out->CurLen;
}

/* ---------------------------------------------------------------------------- */
/**
 * libGetTaskNo
 *
 */
IEC_UINT libGetTaskNo(STaskInfoVM *pVM, IEC_STRING OS_DPTR *strTask, IEC_UINT *upTask)
{
	IEC_UINT i;
	STaskInfoVMM *pVMM = (STaskInfoVMM *)pVM->pShared->pVMM;

	if (pVMM == NULL)
	{
		RETURN(ERR_INVALID_TASK);
	}

	for (i = 0; i < pVMM->Project.uTasks; i++)
	{
		if (OS_STRNICMP(utilIecToAnsi(strTask, pVM->Local.pBuffer), (IEC_CHAR *)pVMM->ppVM[i]->Task.szName, VMM_MAX_IEC_IDENT) == 0)
		{
			break;
		}
	}

	if (i >= pVMM->Project.uTasks)
	{
		RETURN(ERR_INVALID_TASK);
	}

	*upTask = i;

	RETURN(OK);
} 

/* ---------------------------------------------------------------------------- */
