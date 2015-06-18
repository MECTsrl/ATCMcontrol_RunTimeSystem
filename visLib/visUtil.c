
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
 * Filename: visUtil.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"visUtil.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "visDef.h"
#include "visMain.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * domSwap16
 *
 */
IEC_UINT domSwap16(SVisInfo *pVI, IEC_UINT uVal)
{
	IEC_UINT uRet = uVal;

	if (pVI->bInitialized == TRUE && pVI->bBE_Control != pVI->bBE_This)
	{
		IEC_BYTE *pVal = (IEC_BYTE *)&uVal;
		IEC_BYTE *pRet = (IEC_BYTE *)&uRet;

		pRet[0] = pVal[1];
		pRet[1] = pVal[0];
	}

	return uRet;
}


/* ---------------------------------------------------------------------------- */
/**
 * domSwap32
 *
 */
IEC_UDINT domSwap32(SVisInfo *pVI, IEC_UDINT ulVal)
{
	IEC_UDINT ulRet = ulVal;

	if (pVI->bInitialized == TRUE && pVI->bBE_Control != pVI->bBE_This)
	{
		IEC_BYTE *pVal = (IEC_BYTE *)&ulVal;
		IEC_BYTE *pRet = (IEC_BYTE *)&ulRet;

		pRet[0] = pVal[3];
		pRet[1] = pVal[2];
		pRet[2] = pVal[1];
		pRet[3] = pVal[0];
	}

	return ulRet;
}


/* ---------------------------------------------------------------------------- */
/**
 * domSwap64
 *
 */
IEC_ULINT domSwap64(SVisInfo *pVI, IEC_ULINT ullVal)
{
	IEC_ULINT ullRet = ullVal;

	if (pVI->bInitialized == TRUE && pVI->bBE_Control != pVI->bBE_This)
	{
		IEC_BYTE *pVal = (IEC_BYTE *)&ullVal;
		IEC_BYTE *pRet = (IEC_BYTE *)&ullRet;

		pRet[0] = pVal[7];
		pRet[1] = pVal[6];
		pRet[2] = pVal[5];
		pRet[3] = pVal[4];
		pRet[4] = pVal[3];
		pRet[5] = pVal[2];
		pRet[6] = pVal[1];
		pRet[7] = pVal[0];
	}

	return ullRet;
}


/* ---------------------------------------------------------------------------- */
/**
 * domSwapVar20
 *
 * Executes a byte swap on a given value list. The corresponding value types
 * are defined in a variable list as valid prior to V2.1 firmware.
 *
 */
IEC_UINT domSwapVar20(SVisInfo *pVI, IEC_DATA *pVar, IEC_UINT uVar, IEC_DATA *pVal, IEC_UINT uVal)
{
	IEC_UINT uRes = OK;
	
	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	if (pVI->bBE_Control == pVI->bBE_This)
	{
		RETURN(OK);
	}

	{
		IEC_UINT i;

		IEC_UINT	uCurVal = 0;
		IEC_UINT	uValLen = 0;

		XVariable	*pxSrc	= (XVariable *)pVar;

		IEC_DATA	pRet[8];

		for (i = 0; i < uVar / sizeof(XVariable); i++)
		{
			uValLen = (IEC_UINT)(domSwap16(pVI, pxSrc->uLen) == 0 ? 1 : domSwap16(pVI, pxSrc->uLen));

			if (uCurVal + uValLen > uVal)
			{
				RETURN(ERR_INVALID_DATA_SIZE);
			}
			
			switch (uValLen)
			{
			case 2: pRet[0] = pVal[1]; pRet[1] = pVal[0]; *(IEC_UINT *)pVal = *(IEC_UINT *)pRet; break;
			case 4: pRet[0] = pVal[3]; pRet[1] = pVal[2]; pRet[2] = pVal[1]; pRet[3] = pVal[0]; *(IEC_UDINT *)pVal = *(IEC_UDINT *)pRet; break;
			case 8: pRet[0] = pVal[7]; pRet[1] = pVal[6]; pRet[2] = pVal[5]; pRet[3] = pVal[4]; pRet[4] = pVal[3]; pRet[5] = pVal[2]; pRet[6] = pVal[1]; pRet[7] = pVal[0]; *(IEC_ULINT *)pVal = *(IEC_ULINT *)pRet; break;
			}

			uCurVal = (IEC_UINT)(uCurVal + uValLen);
			
			pVal += uValLen;
			pxSrc++;
		}
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domSwapVar21
 *
 * Executes a byte swap on a given value list. The corresponding value types
 * are defined in a variable list as valid starting within V2.1 firmware.
 *
 */
IEC_UINT domSwapVar21(SVisInfo *pVI, IEC_DATA *pVar, IEC_UINT uVar, IEC_DATA *pVal, IEC_UINT uVal)
{
	IEC_UINT uRes = OK;
	
	IEC_UINT uValLen	= 0;
	IEC_UINT uVarLen	= 0;

	IEC_UINT uCurVal	= 0;
	IEC_UINT uCurVar	= 0;
	
	IEC_DATA pRet[8];

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	if (pVI->bBE_Control == pVI->bBE_This)
	{
		RETURN(OK);
	}

	while(uCurVar < uVar)
	{
		switch(*pVar & VMM_XV_TYPEMASK)
		{
			case VMM_XV_SMALL:
			{
				XVariableS *pxVar = (XVariableS *)pVar;

				if (uVar < sizeof(XVariableS))
				{
					RETURN(ERR_INVALID_DATA);
				}
				uValLen = (IEC_UINT)(pxVar->usLen == 0 ? 1 : pxVar->usLen);
				uVarLen = sizeof(XVariableS);
				break;
			}

			case VMM_XV_MEDIUM:
			{
				XVariableM *pxVar = (XVariableM *)pVar;

				if (uVar < sizeof(XVariableM))
				{
					RETURN(ERR_INVALID_DATA);
				}
				uValLen = (IEC_UINT)(domSwap16(pVI, pxVar->uLen) == 0 ? 1 : domSwap16(pVI, pxVar->uLen));
				uVarLen = sizeof(XVariableM);
				break;
			}

			case VMM_XV_LARGE:
			{
				XVariableL *pxVar = (XVariableL *)pVar;

				if (uVar < sizeof(XVariableL))
				{
					RETURN(ERR_INVALID_DATA);
				}
				uValLen = (IEC_UINT)(domSwap16(pVI, pxVar->uLen) == 0 ? 1 : domSwap16(pVI, pxVar->uLen));
				uVarLen = sizeof(XVariableL);
				break;
			}

			default:
			{
				RETURN(ERR_INVALID_DATA);
			}
		
		} /* switch(*pVar & VMM_XV_TYPEMASK) */
	
		if (uCurVal + uValLen > uVal)
		{
			RETURN(ERR_INVALID_DATA_SIZE);
		}
			
		switch (uValLen)
		{
		case 2: pRet[0] = pVal[1]; pRet[1] = pVal[0]; *(IEC_UINT *)pVal = *(IEC_UINT *)pRet; break;
		case 4: pRet[0] = pVal[3]; pRet[1] = pVal[2]; pRet[2] = pVal[1]; pRet[3] = pVal[0]; *(IEC_UDINT *)pVal = *(IEC_UDINT *)pRet; break;
		case 8: pRet[0] = pVal[7]; pRet[1] = pVal[6]; pRet[2] = pVal[5]; pRet[3] = pVal[4]; pRet[4] = pVal[3]; pRet[5] = pVal[2]; pRet[6] = pVal[1]; pRet[7] = pVal[0]; *(IEC_ULINT *)pVal = *(IEC_ULINT *)pRet; break;
		}			

		pVal	+= uValLen;
		uCurVal  = (IEC_UINT)(uCurVal + uValLen);
		
		pVar	+= uVarLen;
		uCurVar  = (IEC_UINT)(uCurVar + uVarLen);

	} /* while(uVar > 0) */ 
	
	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domSwapVal
 *
 * Executes a byte swap on a given value list. 
 *
 */
IEC_UINT domSwapVal(SVisInfo *pVI, IEC_DATA *pVal, IEC_UINT uVal)
{
	IEC_UINT uRes		= OK;

	IEC_UINT uDatLen	= 0;
	IEC_UINT uCurVal	= 0;
	IEC_DATA pRet[8];
	IEC_DATA *pDat		= NULL;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}

	if (pVI->bBE_Control == pVI->bBE_This)
	{
		RETURN(OK);
	}

	while (uCurVal < uVal)
	{
		XValue	*pxSrc	= (XValue *)(pVal + uCurVal);

		uDatLen = (IEC_UINT)(domSwap16(pVI, pxSrc->VAR.uLen) == 0 ? 1 : domSwap16(pVI, pxSrc->VAR.uLen));

		if (uCurVal + HD_VALUE + uDatLen > uVal)
		{
			RETURN(ERR_INVALID_DATA_SIZE);
		}
			
		pDat = pxSrc->pValue;

		switch (uDatLen)
		{
		case 2: pRet[0] = pDat[1]; pRet[1] = pDat[0]; *(IEC_UINT *)pDat = *(IEC_UINT *)pRet; break;
		case 4: pRet[0] = pDat[3]; pRet[1] = pDat[2]; pRet[2] = pDat[1]; pRet[3] = pDat[0]; *(IEC_UDINT *)pDat = *(IEC_UDINT *)pRet; break;
		case 8: pRet[0] = pDat[7]; pRet[1] = pDat[6]; pRet[2] = pDat[5]; pRet[3] = pDat[4]; pRet[4] = pDat[3]; pRet[5] = pDat[2]; pRet[6] = pDat[1]; pRet[7] = pDat[0]; *(IEC_ULINT *)pDat = *(IEC_ULINT *)pRet; break;
		}

		uCurVal = (IEC_UINT)(uCurVal + HD_VALUE + uDatLen); 			
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * domConvertVarToVal
 *
 */
IEC_UINT domConvertVarToVal(SVisInfo *pVI, IEC_DATA *pVar, IEC_UINT uVar, XVariable *pxDest)
{		
	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}
	if (pVI->bLogin == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	if (pVI->ulFirmware < 21000)
	{
		if (uVar < sizeof(XVariable))
		{
			RETURN(ERR_INVALID_DATA);
		}

		OS_MEMCPY(pxDest, pVar, sizeof(XVariable));

		RETURN(OK);
	}

	switch(*pVar & VMM_XV_TYPEMASK)
	{
		case VMM_XV_SMALL:
		{
			XVariableS *pxVar = (XVariableS *)pVar;

			if (uVar < sizeof(XVariableS))
			{
				RETURN(ERR_INVALID_DATA);
			}
			
			pxDest->byBit		= (IEC_USINT)(pxVar->usType & VMM_XV_BITMASK);
			pxDest->uLen		= (IEC_UINT)(pxVar->usLen == 0 ? 1 : pxVar->usLen);
			pxDest->ulOffset	= pxVar->usOffset;
			pxDest->uSegment	= pxVar->usSegment;
			
			break;
		}

		case VMM_XV_MEDIUM:
		{
			XVariableM *pxVar = (XVariableM *)pVar;

			if (uVar < sizeof(XVariableM))
			{
				RETURN(ERR_INVALID_DATA);
			}

			pxDest->byBit		= (IEC_USINT)(pxVar->usType & VMM_XV_BITMASK);
			pxDest->uLen		= (IEC_UINT)(domSwap16(pVI, pxVar->uLen) == 0 ? 1 : domSwap16(pVI, pxVar->uLen));
			pxDest->ulOffset	= pxVar->uOffset;
			pxDest->uSegment	= pxVar->usSegment;

			break;
		}

		case VMM_XV_LARGE:
		{
			XVariableL *pxVar = (XVariableL *)pVar;

			if (uVar < sizeof(XVariableL))
			{
				RETURN(ERR_INVALID_DATA);
			}

			pxDest->byBit		= (IEC_USINT)(pxVar->usType & VMM_XV_BITMASK);
			pxDest->uLen		= (IEC_UINT)(domSwap16(pVI, pxVar->uLen) == 0 ? 1 : domSwap16(pVI, pxVar->uLen));
			pxDest->ulOffset	= pxVar->ulOffset;
			pxDest->uSegment	= pxVar->uSegment;

			break;
		}

		default:
		{
			RETURN(ERR_INVALID_DATA);
		}
	
	} /* switch(*pVar & VMM_XV_TYPEMASK) */
	
	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * domConvertDBIVarToVar
 *
 */
IEC_UINT domConvertDBIVarToVar(SVisInfo *pVI, IEC_DATA *pDBIVar, IEC_DATA **ppDest, IEC_UINT *upDest)
{		
	XVisuVar *pxVisu = (XVisuVar *)pDBIVar;

	if (pVI->bInitialized == FALSE)
	{
		RETURN(ERR_INIT);
	}
	if (pVI->bLogin == FALSE)
	{
		RETURN(ERR_LOGIN);
	}

	if (*ppDest != NULL)
	{
		osFree(ppDest);
		
		*ppDest = NULL;
		*upDest = 0;
	}

	switch(pxVisu->xVar.usType & 0xE0u)
	{
		case DBI_DTM_SIMPLE:
		{
			IEC_UINT uDataLen;

			IEC_USINT usVType	= VMM_XV_LARGE;
			IEC_UINT  uVSize	= sizeof(XVariableL);

			IEC_DATA byCommand	= CMD_GET_VALUE;

			/* Get data length
			 */
			switch(pxVisu->xVar.usType & ~0xE0u)
			{
			case DBI_DT_STRING	: 
				uDataLen = (IEC_UINT)(domSwap16(pVI, pxVisu->xVar.uLen) + 2);
				break;

			default:
				uDataLen = domSwap16(pVI, pxVisu->xVar.uLen);
				break;
			}

			/* Calculate fitting variable structure
			 */
			if (domSwap32(pVI, pxVisu->xVar.ulOffset) <= 0xffu && uDataLen <= 0xffu && domSwap16(pVI, pxVisu->xVar.uInst) <= 0xffu)
			{
				usVType = VMM_XV_SMALL;
				uVSize	= sizeof(XVariableS);
			}
			else if (domSwap32(pVI, pxVisu->xVar.ulOffset) <= 0xffffu && domSwap16(pVI, pxVisu->xVar.uInst) <= 0xffu)
			{
				usVType = VMM_XV_MEDIUM;
				uVSize	= sizeof(XVariableM);
			}
			else
			{
				usVType = VMM_XV_LARGE;
				uVSize	= sizeof(XVariableL);
			}

			*upDest 	= uVSize;
			*ppDest 	= osMalloc(*upDest);

			byCommand	= CMD_GET_VALUE;

			switch(usVType)
			{	
				case VMM_XV_SMALL:
				{
					XVariableS *pxVar	= (XVariableS *)(*ppDest);

					pxVar->usType		= (IEC_USINT)(VMM_XV_SMALL | (pxVisu->xVar.usBit & VMM_XV_BITMASK));

					pxVar->usOffset 	= (IEC_USINT)domSwap32(pVI, pxVisu->xVar.ulOffset);
					pxVar->usLen		= (IEC_USINT)				uDataLen;
					pxVar->usSegment	= (IEC_USINT)domSwap16(pVI, pxVisu->xVar.uInst);
					
					break;
				}

				case VMM_XV_MEDIUM:
				{
					XVariableM *pxVar	= (XVariableM *)(*ppDest);

					pxVar->usType		= (IEC_USINT)(VMM_XV_MEDIUM | (pxVisu->xVar.usBit & VMM_XV_BITMASK));

					pxVar->uOffset		= domSwap16(pVI, (IEC_UINT) domSwap32(pVI,	pxVisu->xVar.ulOffset));
					pxVar->uLen 		= domSwap16(pVI, (IEC_UINT) 				uDataLen);
					pxVar->usSegment	=				 (IEC_USINT)domSwap16(pVI,	pxVisu->xVar.uInst);

					break;
				}

				case VMM_XV_LARGE:
				{
					XVariableL *pxVar	= (XVariableL *)(*ppDest);

					pxVar->usType		= (IEC_USINT)(VMM_XV_LARGE | (pxVisu->xVar.usBit & VMM_XV_BITMASK));

					pxVar->ulOffset 	=					pxVisu->xVar.ulOffset;
					pxVar->uLen 		= domSwap16(pVI,	uDataLen);
					pxVar->uSegment 	=					pxVisu->xVar.uInst;

					break;
				}
			}
			
		} /* case DBI_DTM_SIMPLE */
		
		break;

		case DBI_DTM_ARRAY:
		case DBI_DTM_OBJECT:
		{
			/* Only simple type can be converted
			 */
			RETURN(ERR_INVALID_PARAM);
		}

		default:
		{
			/* Invalid type!
			 */
			RETURN(ERR_INVALID_PARAM);
		}

	} /* switch(pxVisu->xVar.usType & 0xE0u) */

	RETURN(OK);
}


/* ---------------------------------------------------------------------------- */
/**
 * visError
 *
 * Determines an error text to a given FarosPLC Run Time System error number.
 *
 * I	uErrNo		Error number returned by a 4C RTS function.
 *
 * return			Corresponding error message text.
 *
 */
IEC_CHAR *visError(IEC_UINT uErrNo)
{
	IEC_CHAR *szError = NULL;

	switch(uErrNo)
	{
	/* Warnings
	 */
	case WRN_TIME_OUT			:	szError = "Time Out";													break;				
	case WRN_NO_CONNECTION		:	szError = "No connection to OPC server";								break;
	case WRN_HANDLED			:	szError = "Command already handled";									break;
	case WRN_BREAK				:	szError = "Task is in breakpoint";										break;
	case ERR_CRC				:	szError = "Checksum calculation failed";								break;
	case WRN_DIVIDED			:	szError = "Command divided";											break;
	case WRN_TRUNCATED			:	szError = "Data truncated"; 											break;
	case WRN_NO_MESSAGE 		:	szError = "No message in message queue";								break;
	case WRN_RETAIN_INVALID 	:	szError = "Retain memory invalid, executing a cold start";				break;
	case WRN_TASK_HALTED		:	szError = "Task halted immediately";									break;
	case WRN_IN_PROGRESS		:	szError = "Process still in progress";									break;

	/* Common error messages
	 */
	case ERR_INIT				:	szError = "Task initializations failed";								break;
	case ERR_INVALID_TASK		:	szError = "Invalid task number";										break;
	case ERR_INVALID_COMMAND	:	szError = "Invalid command received";									break;
	case ERR_INVALID_DATA		:	szError = "Invalid data received";										break;
	case ERR_INVALID_OFFSET 	:	szError = "Invalid object offset";										break;
	case ERR_INVALID_INSTANCE	:	szError = "Invalid instance object";									break;
	case ERR_INVALID_CLASS		:	szError = "Invalid class ID";											break;
	case ERR_INVALID_DATA_SIZE	:	szError = "Data size not valid";										break;
	case ERR_QUEUE_FULL 		:	szError = "Message or breakpoint queue full";							break;
	case ERR_OUT_OF_MEMORY		:	szError = "Out of memory";												break;
	case ERR_DEBUG_MODE 		:	szError = "Not in debug mode";											break;
	case ERR_LOGIN				:	szError = "Not logged in";												break;
	case ERR_NO_PROJECT 		:	szError = "No project loaded";											break;
	case ERR_CRITICAL_SECTION	:	szError = "Error entering a critical section";							break;
	case ERR_ACTIVATE_TIMER 	:	szError = "Error activating a VM timer";								break;
	case ERR_WRONG_PROJECT		:	szError = "Project conflict. New download required";					break;
	case ERR_TO_MANY_VARS		:	szError = "To many simultaneous variables. Buffer overrun"; 			break;
	case ERR_INVALID_PARAM		:	szError = "Invalid parameter";											break;
	case ERR_CREATE_QUEUE		:	szError = "Unable to create IPC queue"; 								break;
	case ERR_CREATE_TIMER		:	szError = "Unable to create VM timer";									break;
	case ERR_IPC_RECV_FAILED	:	szError = "Unable to receive a message on a IPC queue"; 				break;
	case ERR_IPC_SEND_FAILED	:	szError = "Unable to send a message on a IPC queue";					break;
	case ERR_IPC_VMM_TIMEOUT	:	szError = "Time Out waiting for VMM";									break;
	case ERR_ALREADY_IN_DEBUG	:	szError = "Already in Debug Mode. Only one Debug Connection possible";	break;
	case ERR_INVALID_SEGMENT	:	szError = "Invalid object segment received";							break;
	case ERR_IO_READ			:	szError = "Error reading from communication device";					break;
	case ERR_IO_WRITE			:	szError = "Error writing to communication device";						break;
	case ERR_NOT_CONNECTED		:	szError = "Not connected";												break;
	case ERR_INVALID_MSG		:	szError = "Invalid message received";									break;
	case ERR_WRITE_TO_INPUT 	:	szError = "Write access to input segment";								break;
	case ERR_TO_MUCH_DATA		:	szError = "To much data specified"; 									break;
	case ERR_NOT_SUPPORTED		:	szError = "Function/Command not supported"; 							break;
	case ERR_UNEXPECTED_MSG 	:	szError = "Wrong - not fitting - response received";					break;
	case ERR_ERRMSG_RECEIVED	:	szError = "Common error message received";								break;
	case ERR_INVALID_QUEUE		:	szError = "An invalid message queue was given"; 						break;
	case ERR_UNKNOWN_PRODUCT	:	szError = "Unknown FarosPLC product ID given";							break;
	case ERR_NOT_READY			:	szError = "Unable to execute command now";								break;
	case ERR_INVALID_IOLAYER	:	szError = "An invalid IO layer was given";								break;
	case ERR_NOT_LICENSED		:	szError = "Feature not licensed";										break;
	case ERR_INVALID_LICENSE	:	szError = "Invalid license key received";								break;
	case ERR_INVALID_PRODUCT	:	szError = "Invalid product number received";							break;

	/* Download
	 */
	case ERR_INVALID_DOMAIN 	:	szError = "Invalid download domain";									break;
	case ERR_INVALID_VERSION	:	szError = "Invalid version";											break;
	case ERR_OVERRUN_TASK		:	szError = "To many tasks specified";									break;
	case ERR_OVERRUN_INSTANCE	:	szError = "To many instance objects";									break;
	case ERR_OVERRUN_CLASS		:	szError = "To many classes (POU's)";									break;
	case ERR_OVERRUN_PROGRAM	:	szError = "To many programs per task";									break;
	case ERR_OVERRUN_CODESEG	:	szError = "Code segment full";											break;
	case ERR_OVERRUN_OBJSEG 	:	szError = "Data segment full";											break;
	case ERR_OVERRUN_REGION 	:	szError = "To many read/write memory regions";							break;
	case ERR_ENTRIES_MISSING	:	szError = "Not all expected items received";							break;
	case ERR_INVALID_SEG_INIT	:	szError = "Data segment initializations out of order";					break;
	case ERR_CREATE_TASK		:	szError = "Error creating a task or thread";							break;
	case ERR_OVERRUN_COPYSEG	:	szError = "To many local retain copy segments"; 						break;
	case ERR_BUFFER_TOO_SMALL	:	szError = "Temporary buffer is too small";								break;

	/* Breakpoints
	 */
	case ERR_INVALID_CODE_POS	:	szError = "Invalid code position";										break;
	case ERR_BP_NOT_FOUND		:	szError = "Breakpoint not found in breakpoint list";					break;
	case ERR_BP_LIST_FULL		:	szError = "Breakpoint list full";										break;
	case ERR_TASK_NOT_IN_BREAK	:	szError = "Task is not in breakpoint state";							break;

	/* File I/O
	 */
	case ERR_FILE_INIT			:	szError = "File library not initialized";								break;
	case ERR_FILE_MAX			:	szError = "To many open files"; 										break;
	case ERR_FILE_WRONG_ID		:	szError = "Invalid file ID used";										break;
	case ERR_FILE_EOF			:	szError = "End of file reached";										break;
	case ERR_FILE_OPEN			:	szError = "Error opening a file";										break;
	case ERR_FILE_CLOSE 		:	szError = "Error closing a file";										break;
	case ERR_FILE_READ			:	szError = "Error reading from file";									break;
	case ERR_FILE_WRITE 		:	szError = "Error writing to file";										break;
	case ERR_FILE_SYNC			:	szError = "Temporary file closed by another process";					break;
	case ERR_FILE_NOT_EXIST 	:	szError = "File does not exist";										break;
	case ERR_FILE_SEEK			:	szError = "Error setting file position";								break;
	case ERR_FILE_RENAME		:	szError = "Error renaming a file";										break;
	case ERR_FILE_REMOVE		:	szError = "Error deleting a file";										break;
	case ERR_INVALID_DRIVE		:	szError = "Invalid drive specification";								break;
	case ERR_CHANGE_DRIVE		:	szError = "Error changing drive";										break;
	case ERR_CHANGE_DIR 		:	szError = "Error changing directory";									break;
	case ERR_INVALID_PATH		:	szError = "Invalid directory name"; 									break;
	case ERR_INVALID_FILE_NAME	:	szError = "Invalid file name";											break;
	case ERR_PATH_IS_NOT_8_3	:	szError = "File path is not in 8.3 format"; 							break;
	case ERR_PATH_TO_LONG		:	szError = "File path to long";											break;
	case ERR_FILE_INVALID		:	szError = "Invalid file specified"; 									break;
											   
	/* Debug Interface
	 */
	case ERR_DBI_INIT			:	szError = "Debug library not initialized";								break;
	case ERR_DBI_PARAM			:	szError = "Parameter not valid";										break;
	case ERR_DBI_OBJ_NOT_FOUND	:	szError = "Object not found";											break;
	case ERR_DBI_FILE_FORMAT	:	szError = "Debug file invalid formatted";								break;
	case ERR_DBI_RETAIN_MIXED	:	szError = "Complex variable has mixed with retentive and non retentive memory"; break;
	case ERR_DBI_BUF_TOO_SMALL	:	szError = "Communication buffer to small";								break;

	/* Flash interface
	 */
	case ERR_FLASH				:	szError = "Common flash error"; 										break;
	case ERR_FLASH_INIT 		:	szError = "Error initializing the flash memory";						break;
	case ERR_FLASH_WRITE		:	szError = "Error writing into the flash memory";						break;
	case ERR_FLASH_READ 		:	szError = "Error reading from the flash memory";						break;
	case ERR_FLASH_CLEAR		:	szError = "Error erasing the flash memory"; 							break;
	case ERR_FLASH_WRONG_BLOCK	:	szError = "Flash domain does not match";								break;
		
	/* Online Change
	 */
	case ERR_OC_TASK_CHANGED	:	szError = "Number of tasks or tasks attributes changed. Online Change not possible";	break;
	case ERR_OC_PROJ_CHANGED	:	szError = "Project changed. Online Change not possible";				break;
	case ERR_OC_INVALID_CODE	:	szError = "Invalid code object index received"; 						break;
	case ERR_OC_INVALID_INST	:	szError = "Invalid data object index received"; 						break;
	case ERR_OC_COPY_LIST		:	szError = "Invalid data copy list received";							break;
	case ERR_OC_COPY_NEW		:	szError = "Invalid new data object index specified";					break;
	case ERR_OC_COPY_OLD		:	szError = "Invalid old data object index specified";					break;
	case ERR_OC_TO_MANY_CODE	:	szError = "To many changed code objects received";						break;
	case ERR_OC_TO_MANY_INST	:	szError = "To many changed data objects received";						break;
	case ERR_OC_TO_MANY_CL		:	szError = "To many entries in data object copy list";					break;
	case ERR_OC_TEMP_CODE		:	szError = "To many temporary code objects"; 							break;
	case ERR_OC_TEMP_INST		:	szError = "To many temporary data objects"; 							break;
	case ERR_OC_RETAIN_CHANGED	:	szError = "Online Change is not supported for retain variables";		break;

	/* Field Bus
	 */
	case ERR_FB_INIT			:	szError = "Failed to initialize field bus driver";						break;
	case ERR_FB_INIT_DATA		:	szError = "Field bus initialization failed - invalid configuration";	break;
	case WRN_FB_NO_INIT_DATA	:	szError = "Field bus not initialized, no configuration data";			break;
	case ERR_FB_TERM			:	szError = "Failed to terminate field bus";								break;
	case ERR_FB_NOT_INITIALIZED :	szError = "Field bus is not initialized";								break;
	case ERR_FB_NOT_OPERATING	:	szError = "Field bus is not operating"; 								break;
	case ERR_FB_UNKNOWN_FBUS	:	szError = "Field bus type unknown"; 									break;
	case ERR_FB_NOT_LICENSED	:	szError = "Given field bus type not licensed";							break;
	case ERR_FB_NOT_ENABLED 	:	szError = "Given filed bus type is not activated";						break;

		
	/* Default
	 */
	default 					: szError = "Unknown Error";				
	}

	return szError;
}

/* ---------------------------------------------------------------------------- */
