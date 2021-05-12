
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

#include "inc/stdInc.h"

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
                                            /* Reserved Numbers:	230 -	239 */
    /* 230, 231, 232, 233, 234, 235, 236, 237, 238, 239 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* funUSB.c - USB library 
	 * ------------------------------------------------------------------------
	 */
                                                /* Reserved Numbers:	240 -	249 */
    /* 240, 241, 242, 243, 244, 245, 246, 247, 248, 249 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* funDatalog.c - Datalogger library 
	 * ------------------------------------------------------------------------
	 */
                                                /* Reserved Numbers:	250 -	259 */
    /* 250, 251, 252, 253, 254, 255, 256, 257, 258, 259 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* funMectUserUtility.c - Datalogger library 
	 * ------------------------------------------------------------------------
	 */
                                                    /* Reserved Numbers:	260 -	279 */
    /* 260, 261, 262, 263, 264, 265, 266, 267, 268, 269 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    /* 270, 271, 272, 273, 274, 275, 276, 277, 278, 279 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* funHW119.c - Datalogger library 
	 * ------------------------------------------------------------------------
	 */
                                                    /* Reserved Numbers:	280 -	299 */
    /* 280, 281, 282, 283, 284, 285, 286, 287, 288, 289 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    /* 290, 291, 292, 293, 294, 295, 296, 297, 298, 299 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* funModbus.c - Datalogger library 
	 * ------------------------------------------------------------------------
	 */
                                                    /* Reserved Numbers:	300 -	359 */
    /* 300, 301, 302, 303, 304, 305, 306, 307, 308, 309 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    /* 310, 311, 312, 313, 314, 315, 316, 317, 318, 319 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    /* 320, 321, 322, 323, 324, 325, 326, 327, 328, 329 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    /* 330, 331, 332, 333, 334, 335, 336, 337, 338, 339 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    /* 340, 341, 342, 343, 344, 345, 346, 347, 348, 349 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    /* 350, 351, 352, 353, 354, 355, 356, 357, 358, 359 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
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
                                        /* Reserved Numbers:	30	-	39	*/
    /*  30,  31,  32,  33,  34,  35,  36,  37,  38,  39 */
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

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
