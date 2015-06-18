
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
 * Filename: vmmCmd.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"vmmCmd.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

#if defined(RTS_CFG_DEBUG_OUTPUT) && defined(RTS_CFG_COMM_TRACE)

  extern IEC_CHAR *szCmdText[];
  
#endif

/* ----  Local Functions:	--------------------------------------------------- */

#if defined(RTS_CFG_PROT_BLOCK)
static IEC_UINT cmdProcess(STaskInfoVMM *pVMM, IEC_UINT uSignal, XBlock *pBlock);
static IEC_UINT cmdCommand(STaskInfoVMM *pVMM, XBlock *pBlock);
static IEC_UINT cmdResponse(STaskInfoVMM *pVMM, XBlock *pBlock);
#endif
static IEC_UINT cmdErrorResponse(STaskInfoVMM *pVMM, XBlock *pBlock);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * cmdRun
 *
 */
#if defined(RTS_CFG_PROT_BLOCK)

IEC_UINT cmdRun(STaskInfoVMM *pVMM, IEC_UINT uSignal, XBlock *pBlock)
{
	SCmdVMM *pCMD	= &pVMM->CMD;
	IEC_BOOL bFirst = TRUE;

	if (uSignal == SIG_DATA && pBlock->uBlock == 1)
	{
		/* Reset
		 */
		pCMD->uState = AS_IDLE;
	}

	while (bFirst || (pCMD->uState != AS_IDLE && pCMD->uState != AS_DATA_READY && pCMD->uState != AS_RESP_READY))	
	{
		switch (pCMD->uState)
		{
			case AS_IDLE:
			{
				pCMD->uState = AS_FIRST_DATA;
				break;
			
			} /* ------------------------------------------------------------- */

			case AS_FIRST_DATA:
			{
				/* Process first data block
				 */
				if (cmdProcess (pVMM, SIG_FIRST, pBlock) != OK)
				{
					RETURN(ERR_ERROR);
				}

				pCMD->uState = (IEC_UINT)(pBlock->byLast ? AS_SWITCH : AS_DATA_READY);

				break;
			
			} /* ------------------------------------------------------------- */

			case AS_DATA_READY:
			{
				pCMD->uState = AS_PROC_DATA;

				break;
			
			} /* ------------------------------------------------------------- */

			case AS_PROC_DATA:
			{
				/* Process data block
				 */
				if (cmdProcess (pVMM, SIG_FIRST, pBlock) != OK)
				{
					RETURN(ERR_ERROR);
				}

				pCMD->uState = (IEC_UINT)(pBlock->byLast ? AS_SWITCH : AS_DATA_READY);

				break;
			
			} /* ------------------------------------------------------------- */

			case AS_SWITCH:
			{
				/* Switch beetween command / resonse
				 */
				if (cmdProcess (pVMM, SIG_SWITCH, 0) != OK)
				{
					RETURN(ERR_ERROR);
				}

				pCMD->uState = AS_RESP_READY;

				break;
			
			} /* ------------------------------------------------------------- */

			case AS_RESP_READY:
			{
				pCMD->uState = AS_PROC_RESPONSE;
				break;
			
			} /* ------------------------------------------------------------- */

			case AS_PROC_RESPONSE:
			{
				/* Process the next response block
				 */
				if (cmdProcess (pVMM, SIG_RESPONSE, pBlock) != OK)
				{
					RETURN(ERR_ERROR);
				}

				pCMD->uState = (IEC_UINT)(pBlock->byLast ? AS_IDLE : AS_RESP_READY);

				break;
			
			} /* ------------------------------------------------------------- */

			default:
			{
				RETURN(ERR_ERROR);

			} /* ------------------------------------------------------------- */

		}

		bFirst = FALSE;
	}

	RETURN(OK);
}

#endif /* RTS_CFG_PROT_BLOCK */

/* ---------------------------------------------------------------------------- */
/**
 * cmdProcess
 *
 */
#if defined(RTS_CFG_PROT_BLOCK)

static IEC_UINT cmdProcess(STaskInfoVMM *pVMM, IEC_UINT uSignal, XBlock *pBlock)
{
	SCmdVMM *pCMD = &pVMM->CMD;

	switch (uSignal)
	{
		case SIG_FIRST:
		{
			/* First command block, reset state
			 */
			pCMD->uError	= OK;
			pCMD->byCommand = pBlock->CMD.byCommand;

			cmdCommand(pVMM, 0);

			/* Fall through!! */

		}	/* ---------------------------------------------------------------- */

		case SIG_DATA:
		{
			/* Another command block to process
			 */
			if (pCMD->byCommand != pBlock->CMD.byCommand)
			{
				pCMD->uError = ERR_ERROR;
				break;
			}
			
			pCMD->uError = cmdCommand(pVMM, pBlock);

			break;

		}	/* ---------------------------------------------------------------- */

		case SIG_SWITCH:
		{
			/* We already have processed all command blocks.
			 * (Clean up code can be added here if necessary.)
			 */
			cmdResponse (pVMM, 0);
			break;

		}	/* ---------------------------------------------------------------- */

		case SIG_RESPONSE:
		{
			/* We have received a request for a response block
			 * from SoftControl. Build block.
			 */
			pBlock->CMD.byCommand = pCMD->byCommand;

			if (pCMD->uError != OK)
			{
				/* An processing error occurred processing a command block. 
				 * Build error message.
				 */
				cmdErrorResponse(pVMM, pBlock);
				
				break;
			}

			pCMD->uError = cmdResponse (pVMM, pBlock);

			if (pCMD->uError != OK)
			{
				/* An processing error occurred processing a response block. 
				 * Build error message.
				 */
				pBlock->CMD.byCommand = pCMD->byCommand;

				cmdErrorResponse(pVMM, pBlock);

				break;
			}

			break;

		}	/* ---------------------------------------------------------------- */

	} /* end switch (uSignal) */

	RETURN(OK);
}

#endif /* RTS_CFG_PROT_BLOCK */

/* ---------------------------------------------------------------------------- */
/**
 * cmdCommand
 *
 */
#if defined(RTS_CFG_PROT_BLOCK)

static IEC_UINT cmdCommand (STaskInfoVMM *pVMM, XBlock *pBlock)
{
	ACTPROC_FUN fpCommand;
	IEC_UINT	uRes	= OK;
	IEC_BOOL	bInit	= FALSE;

	if (pBlock == 0)
	{
		/* Initialize
		 */
		pVMM->DLB.uError	= OK;

		RETURN(OK);
	}

	/* Only one message source possible for block protocol
	 */
	pBlock->usSource = 0;

	if ((uRes = osOnCmdReceived(pVMM, pBlock)) == WRN_HANDLED)
	{
		RETURN(OK);
	}

	if (uRes != OK)
	{
		RETURN(uRes);
	}

	switch (pBlock->CMD.byCommand)
	{
		case CMD_GET_STATE: 			fpCommand = cmdGetState;			bInit = FALSE;	break;
		case CMD_LOGIN: 				fpCommand = cmdLogin;				bInit = FALSE;	break;
		case CMD_LOGOUT:				fpCommand = cmdLogout;				bInit = FALSE;	break;
		case CMD_WARM_START:
		case CMD_COLD_START:			fpCommand = cmdStart;				bInit = FALSE;	break;
		case CMD_START_RESOURCE:
		case CMD_STOP_RESOURCE: 		fpCommand = cmdResource;			bInit = FALSE;	break;
		case CMD_START_ALL_TASKS:
		case CMD_STOP_ALL_TASKS:		fpCommand = cmdAllTasks;			bInit = FALSE;	break;
		case CMD_START_TASK:			
		case CMD_STOP_TASK: 			fpCommand = cmdTask;				bInit = FALSE;	break;
		case CMD_OPEN_DEBUG_SESSION:	fpCommand = cmdOpenDebugSession;	bInit = FALSE;	break;
		case CMD_CLOSE_DEBUG_SESSION:	fpCommand = cmdCloseDebugSession;	bInit = FALSE;	break;
		case CMD_SET_BREAKPOINT:		fpCommand = cmdSetBreakpoint;		bInit = FALSE;	break;
		case CMD_CLEAR_BREAKPOINT:		fpCommand = cmdClearBreakpoint; 	bInit = FALSE;	break;
		case CMD_CLEAR_ALL_BREAKPOINTS: fpCommand = cmdClearAllBreakpoints; bInit = FALSE;	break;
		case CMD_SINGLE_STEP:
		case CMD_CONTINUE:				fpCommand = cmdContinue;			bInit = FALSE;	break;
		case CMD_GET_VALUE: 			fpCommand = cmdGetValue;			bInit = FALSE;	break;
		case CMD_SET_VALUE: 			fpCommand = cmdSetValue;			bInit = FALSE;	break;	
		case CMD_DOWNLOAD_BEGIN:		fpCommand = cmdDownloadBegin;		bInit = TRUE;	break;
		case CMD_DOWNLOAD_CONFIG:		fpCommand = cmdDownloadConfig;		bInit = TRUE;	break;
		case CMD_DOWNLOAD_INITIAL:		fpCommand = cmdDownloadInitial; 	bInit = TRUE;	break;
		case CMD_DOWNLOAD_CODE: 		fpCommand = cmdDownloadCode;		bInit = TRUE;	break;

	  #if defined(RTS_CFG_CUSTOM_DL)
		case CMD_DOWNLOAD_CUSTOM:		fpCommand = cmdDownloadCustom;		bInit = TRUE;	break;
	  #endif

		case CMD_DOWNLOAD_FINISH:		fpCommand = cmdDownloadFinish;		bInit = TRUE;	break;
		case CMD_DOWNLOAD_END:			fpCommand = cmdDownloadEnd; 		bInit = TRUE;	break;
		case CMD_DOWNLOAD_IOL:			fpCommand = cmdDownloadIOL; 		bInit = TRUE;	break;
		case CMD_DOWNLOAD_CLEAR:		fpCommand = cmdDownloadClear;		bInit = TRUE;	break;
		case CMD_INITIALIZE_CLEAR:		fpCommand = cmdInitialize;			bInit = FALSE;	break;

	  #if defined(RTS_CFG_FLASH)
		case CMD_CLEAR_FLASH:			fpCommand = cmdClearFlash;			bInit = FALSE;	break;
		case CMD_FLASH: 				fpCommand = cmdFlash;				bInit = FALSE;	break;
	  #endif

	  #if defined(RTS_CFG_DEBUG_INFO)
		case CMD_DBI_GET_CHILDREN:
		case CMD_DBI_GET_ADDRESS:
		case CMD_DBI_GET_TASKNR:		fpCommand = cmdDBIGet;				bInit = FALSE;	break;
		case CMD_DOWNLOAD_DEBUG:		fpCommand = cmdDownloadDebug;		bInit = TRUE;	break;
	  #endif

		case CMD_GET_PROJ_VERSION:		fpCommand = cmdGetProjectVersion;	bInit = FALSE;	break;

	  #if defined(RTS_CFG_EXT_PROJ_INFO)
		case CMD_GET_PROJ_INFO: 		fpCommand = cmdGetProjectInfo;		bInit = TRUE;	break;
	  #endif
		
	  #if defined(RTS_CFG_STORE_PROJECT)
		case CMD_LOAD_PROJECT:			fpCommand = cmdLoadProject; 		bInit = TRUE;	break;
		case CMD_SAVE_PROJECT:			fpCommand = cmdSaveProject; 		bInit = TRUE;	break;
	  #endif

	  #if defined(RTS_CFG_STORE_FILES)
		case CMD_LOAD_FILE: 			fpCommand = cmdLoadFile;			bInit = TRUE;	break;
		case CMD_SAVE_FILE: 			fpCommand = cmdSaveFile;			bInit = TRUE;	break;
		case CMD_DIR_CONTENT:			fpCommand = cmdDir; 				bInit = TRUE;	break;
		case CMD_DEL_FILE:				fpCommand = cmdDelFile; 							break;
	  #endif

	  #if defined(RTS_CFG_CUSTOM_OC)
		case CMD_CUSTOM:				fpCommand = cmdCustom;				bInit = TRUE;	break;
	  #endif

	  #if defined(RTS_CFG_ONLINE_CHANGE)
		case CMD_OC_BEGIN:				fpCommand = cmdOCBegin; 			bInit = TRUE;	break;
		case CMD_OC_CONFIG: 			fpCommand = cmdOCConfig;			bInit = TRUE;	break;
		case CMD_OC_CODE:				fpCommand = cmdOCCode;				bInit = TRUE;	break;
		case CMD_OC_INITIAL:			fpCommand = cmdOCInitial;			bInit = TRUE;	break;
	  #if defined(RTS_CFG_CUSTOM_DL)
		case CMD_OC_CUSTOM: 			fpCommand = cmdOCCustom;			bInit = TRUE;	break;
	  #endif
	  #if defined(RTS_CFG_DEBUG_INFO)
		case CMD_OC_DEBUG:				fpCommand = cmdOCDebug; 			bInit = TRUE;	break;
	  #endif
		case CMD_OC_FINISH: 			fpCommand = cmdOCFinish;			bInit = TRUE;	break;
		case CMD_OC_COMMIT: 			fpCommand = cmdOCCommit;			bInit = TRUE;	break;
		case CMD_OC_END:				fpCommand = cmdOCEnd;				bInit = TRUE;	break;
	  #endif

		case CMD_GET_CONFIG:			fpCommand = cmdGetConfig;			bInit = FALSE;	break;
			
	  #if defined(RTS_CFG_EXT_RETAIN)
		case CMD_RET_WRITE: 			fpCommand = cmdRetWrite;			bInit = FALSE;	break;
		case CMD_RET_CYCLE: 			fpCommand = cmdRetCycle;			bInit = FALSE;	break;
	  #endif

	  #if defined(RTS_CFG_LICENSE)
		case CMD_GET_INSTKEY:			fpCommand = cmdGetInstKey;			bInit = FALSE;	break;
		case CMD_SET_LICKEY:			fpCommand = cmdSetLicKey;			bInit = FALSE;	break;
		case CMD_GET_SERIALNO:			fpCommand = cmdGetSerialNo; 		bInit = FALSE;	break;
		case CMD_GET_FEATURE:			fpCommand = cmdGetFeature;			bInit = FALSE;	break;
		case CMD_SET_LICEX: 			fpCommand = cmdSetLicEx;			bInit = TRUE;	break;
	  #endif
			
		case CMD_GET_TYPE:				fpCommand = cmdGetType; 			bInit = FALSE;	break;
		case CMD_GET_VERSION:			fpCommand = cmdGetVersion;			bInit = FALSE;	break;

		default:
		{
			/* Unkown command
			 */
			RETURN(ERR_INVALID_COMMAND);
		}
		
	} /* switch (pBlock->CMD.byCommand) */

	if (pBlock->uBlock == 1)
	{
		actClearDownload(pVMM, pBlock->CMD.byCommand);

		if (bInit == TRUE)
		{
			fpCommand(pVMM, 0);
		}
	}

	/* Handle command
	 */
	uRes = fpCommand(pVMM, pBlock);

	osOnCmdHandled(pVMM, pBlock, uRes);
	
	pVMM->DLB.uError = (IEC_UINT)uRes;

	RETURN(uRes);
}

#endif /* RTS_CFG_PROT_BLOCK */

/* ---------------------------------------------------------------------------- */
/**
 * cmdResponse
 *
 */
#if defined(RTS_CFG_PROT_BLOCK)

static IEC_UINT cmdResponse (STaskInfoVMM *pVMM, XBlock *pBlock)
{
	ACTPROC_FUN fpResponse;
	IEC_UINT	uRes	= OK;
	IEC_BOOL	bInit	= FALSE;

	if (pBlock == 0)
	{
		/* Initialize
		 */
		RETURN(OK);
	}

	/* Only one message source possible for block protocol
	 */
	pBlock->usSource = 0;

	if ((uRes = osOnRespReceived(pVMM, pBlock)) == WRN_HANDLED)
	{
		RETURN(OK);
	}

	if (uRes != OK)
	{
		RETURN(uRes);
	}

	switch (pBlock->CMD.byCommand)
	{
		case CMD_GET_STATE: 			fpResponse = resGetState;			bInit = FALSE;	break;
		case CMD_LOGIN: 				fpResponse = resLogin;				bInit = FALSE;	break;
		case CMD_LOGOUT:				fpResponse = resLogout; 			bInit = FALSE;	break;
		case CMD_WARM_START:
		case CMD_COLD_START:			fpResponse = resStart;				bInit = FALSE;	break;
		case CMD_START_RESOURCE:
		case CMD_STOP_RESOURCE: 		fpResponse = resResource;			bInit = FALSE;	break;
		case CMD_START_ALL_TASKS:
		case CMD_STOP_ALL_TASKS:		fpResponse = resAllTasks;			bInit = FALSE;	break;
		case CMD_START_TASK:
		case CMD_STOP_TASK: 			fpResponse = resTask;				bInit = FALSE;	break;
		case CMD_OPEN_DEBUG_SESSION:	fpResponse = resOpenDebugSession;	bInit = FALSE;	break;
		case CMD_CLOSE_DEBUG_SESSION:	fpResponse = resCloseDebugSession;	bInit = FALSE;	break;
		case CMD_SET_BREAKPOINT:		fpResponse = resSetBreakpoint;		bInit = FALSE;	break;
		case CMD_CLEAR_BREAKPOINT:		fpResponse = resClearBreakpoint;	bInit = FALSE;	break;
		case CMD_CLEAR_ALL_BREAKPOINTS: fpResponse = resClearAllBreakpoints;bInit = FALSE;	break;
		case CMD_SINGLE_STEP:			
		case CMD_CONTINUE:				fpResponse = resContinue;			bInit = FALSE;	break;
		case CMD_GET_VALUE: 			fpResponse = resGetValue;			bInit = FALSE;	break;
		case CMD_SET_VALUE: 			fpResponse = resSetValue;			bInit = FALSE;	break;
		case CMD_DOWNLOAD_BEGIN:		fpResponse = resDownloadBegin;		bInit = TRUE;	break;
		case CMD_DOWNLOAD_CONFIG:		fpResponse = resDownloadConfig; 	bInit = TRUE;	break;
		case CMD_DOWNLOAD_INITIAL:		fpResponse = resDownloadInitial;	bInit = TRUE;	break;
		case CMD_DOWNLOAD_CODE: 		fpResponse = resDownloadCode;		bInit = TRUE;	break;

	  #if defined(RTS_CFG_CUSTOM_DL)
		case CMD_DOWNLOAD_CUSTOM:		fpResponse = resDownloadCustom; 	bInit = TRUE;	break;
	  #endif

		case CMD_DOWNLOAD_FINISH:		fpResponse = resDownloadFinish; 	bInit = TRUE;	break;
		case CMD_DOWNLOAD_END:			fpResponse = resDownloadEnd;		bInit = TRUE;	break;
		case CMD_DOWNLOAD_IOL:			fpResponse = resDownloadIOL;		bInit = TRUE;	break;
		case CMD_DOWNLOAD_CLEAR:		fpResponse = resDownloadClear;		bInit = TRUE;	break;
		case CMD_INITIALIZE_CLEAR:		fpResponse = resInitialize; 		bInit = FALSE;	break;
	  
	  #if defined(RTS_CFG_FLASH)	
		case CMD_CLEAR_FLASH:			fpResponse = resClearFlash; 		bInit = FALSE;	break;
		case CMD_FLASH: 				fpResponse = resFlash;				bInit = FALSE;	break;
	  #endif

	  #if defined(RTS_CFG_DEBUG_INFO)
		case CMD_DBI_GET_CHILDREN:		fpResponse = resDBIGetChildren; 	bInit = FALSE;	break;
		case CMD_DBI_GET_ADDRESS:		fpResponse = resDBIGetAddress;		bInit = FALSE;	break;
		case CMD_DBI_GET_TASKNR:		fpResponse = resDBIGetTaskNr;		bInit = FALSE;	break;
		case CMD_DOWNLOAD_DEBUG:		fpResponse = resDownloadDebug;		bInit = TRUE;	break;
	  #endif

		case CMD_GET_PROJ_VERSION:		fpResponse = resGetProjectVersion;	bInit = FALSE;	break;

	  #if defined(RTS_CFG_EXT_PROJ_INFO)
		case CMD_GET_PROJ_INFO: 		fpResponse = resGetProjectInfo; 	bInit = TRUE;	break;
	  #endif

	  #if defined(RTS_CFG_STORE_PROJECT)
		case CMD_LOAD_PROJECT:			fpResponse = resLoadProject;		bInit = TRUE;	break;
		case CMD_SAVE_PROJECT:			fpResponse = resSaveProject;		bInit = TRUE;	break;
	  #endif

	  #if defined(RTS_CFG_STORE_FILES)
		case CMD_LOAD_FILE: 			fpResponse = resLoadFile;			bInit = TRUE;	break;
		case CMD_SAVE_FILE: 			fpResponse = resSaveFile;			bInit = TRUE;	break;
		case CMD_DIR_CONTENT:			fpResponse = resDir;				bInit = TRUE;	break;
		case CMD_DEL_FILE:				fpResponse = resDelFile;							break;
	  #endif

	  #if defined(RTS_CFG_CUSTOM_OC)
		case CMD_CUSTOM:				fpResponse = resCustom; 			bInit = TRUE;	break;
	  #endif

	  #if defined(RTS_CFG_ONLINE_CHANGE)
		case CMD_OC_BEGIN:				fpResponse = resOCBegin;			bInit = TRUE;	break;
		case CMD_OC_CONFIG: 			fpResponse = resOCConfig;			bInit = TRUE;	break;
		case CMD_OC_CODE:				fpResponse = resOCCode; 			bInit = TRUE;	break;
		case CMD_OC_INITIAL:			fpResponse = resOCInitial;			bInit = TRUE;	break;
	  #if defined(RTS_CFG_CUSTOM_DL)
		case CMD_OC_CUSTOM: 			fpResponse = resOCCustom;			bInit = TRUE;	break;
	  #endif
	  #if defined(RTS_CFG_DEBUG_INFO)
		case CMD_OC_DEBUG:				fpResponse = resOCDebug;			bInit = TRUE;	break;
	  #endif
		case CMD_OC_FINISH: 			fpResponse = resOCFinish;			bInit = TRUE;	break;
		case CMD_OC_COMMIT: 			fpResponse = resOCCommit;			bInit = TRUE;	break;
		case CMD_OC_END:				fpResponse = resOCEnd;				bInit = TRUE;	break;
	  #endif

		case CMD_GET_CONFIG:			fpResponse = resGetConfig;			bInit = FALSE;	break;

	  #if defined(RTS_CFG_EXT_RETAIN)
		case CMD_RET_WRITE: 			fpResponse = resRetWrite;			bInit = FALSE;	break;
		case CMD_RET_CYCLE: 			fpResponse = resRetCycle;			bInit = FALSE;	break;
	  #endif

	  #if defined(RTS_CFG_LICENSE)
		case CMD_GET_INSTKEY:			fpResponse = resGetInstKey; 		bInit = FALSE;	break;
		case CMD_SET_LICKEY:			fpResponse = resSetLicKey;			bInit = FALSE;	break;
		case CMD_GET_SERIALNO:			fpResponse = resGetSerialNo;		bInit = FALSE;	break;
		case CMD_GET_FEATURE:			fpResponse = resGetFeature; 		bInit = FALSE;	break;
		case CMD_SET_LICEX: 			fpResponse = resSetLicEx;			bInit = TRUE;	break;
	  #endif
			
		case CMD_GET_TYPE:				fpResponse = resGetType;			bInit = FALSE;	break;
		case CMD_GET_VERSION:			fpResponse = resGetVersion; 		bInit = FALSE;	break;

		default:
		{
			/* Unkown command
			 */
			RETURN(ERR_INVALID_COMMAND);
		}
	
	} /* switch (pBlock->CMD.byCommand) */

	if (pBlock->uBlock == 1)
	{
		if (bInit == TRUE)
		{
			fpResponse(pVMM, 0);
		}

		pBlock->byLast	= TRUE;
		pBlock->uLen	= 0;
	}

	/* Handle response
	 */
	uRes = fpResponse(pVMM, pBlock);

	if (uRes == OK)
	{
		uRes = osOnRespHandled(pVMM, pBlock, uRes);
	}
	else
	{
		osOnRespHandled(pVMM, pBlock, uRes);
	}

	RETURN(uRes);
}

#endif /* RTS_CFG_PROT_BLOCK */

/* ---------------------------------------------------------------------------- */
/**
 * cmdErrorResponse
 *
 */
static IEC_UINT cmdErrorResponse (STaskInfoVMM *pVMM, XBlock *pBlock)
{	
	if (pVMM->DLB.uError == OK)
	{
		pVMM->DLB.uError = ERR_ERROR;
	}

	*(IEC_UINT *)pBlock->CMD.pData = pVMM->DLB.uError;

	pBlock->CMD.byCommand |= (IEC_BYTE)0x80u;

	pBlock->byLast	= TRUE;
	pBlock->uLen	= sizeof(IEC_UINT);
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * cmdExecute
 *
 */
#if defined(RTS_CFG_PROT_CLIENT)

IEC_UINT cmdExecute(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes	= OK;
	IEC_BOOL bInit	= FALSE;

	ACTPROC_FUN fpCommand;
	ACTPROC_FUN fpResponse;

  #if defined(RTS_CFG_DEBUG_OUTPUT) & defined(RTS_CFG_COMM_TRACE)
	{
		IEC_UINT uCmd = pBlock->CMD.byCommand;
		osTrace(">>> %-8s %2d %5d   ", uCmd <= LAST_CMD_VALUE ? szCmdText[uCmd] : "Invalid", pBlock->uBlock, pBlock->uLen);
	}
  #endif

	if (pBlock->usSource >= MAX_CONNECTIONS)
	{
		RETURN(ERR_INVALID_DATA);
	}

	if ((uRes = osOnCmdReceived(pVMM, pBlock)) == WRN_HANDLED)
	{
		RETURN(OK);
	}

	if (uRes != OK)
	{
		RETURN(uRes);
	}

	switch (pBlock->CMD.byCommand)
	{
		case CMD_GET_STATE:
			fpCommand	= cmdGetState;		
			fpResponse	= resGetState;	
			bInit		= FALSE;
			break;

		case CMD_LOGIN: 				
			fpCommand	= cmdLogin; 		
			fpResponse	= resLogin;
			bInit		= FALSE;
			break;

		case CMD_LOGOUT:				
			fpCommand	= cmdLogout;
			fpResponse	= resLogout;
			bInit		= FALSE;
			break;

		case CMD_WARM_START:			
		case CMD_COLD_START:			
			fpCommand	= cmdStart;
			fpResponse	= resStart;
			bInit		= FALSE;
			break;

		case CMD_START_RESOURCE:		
		case CMD_STOP_RESOURCE: 		
			fpCommand	= cmdResource;
			fpResponse	= resResource;
			bInit		= FALSE;
			break;

		case CMD_START_ALL_TASKS:		
		case CMD_STOP_ALL_TASKS:		
			fpCommand	= cmdAllTasks;
			fpResponse	= resAllTasks;
			bInit		= FALSE;
			break;

		case CMD_START_TASK:			
		case CMD_STOP_TASK: 			
			fpCommand	= cmdTask;
			fpResponse	= resTask;
			bInit		= FALSE;
			break;

		case CMD_OPEN_DEBUG_SESSION:	
			fpCommand	= cmdOpenDebugSession;	
			fpResponse	= resOpenDebugSession;
			bInit		= FALSE;
			break;

		case CMD_CLOSE_DEBUG_SESSION:	
			fpCommand	= cmdCloseDebugSession;
			fpResponse	= resCloseDebugSession;
			bInit		= FALSE;
			break;

		case CMD_SET_BREAKPOINT:		
			fpCommand	= cmdSetBreakpoint;
			fpResponse	= resSetBreakpoint;
			bInit		= FALSE;
			break;

		case CMD_CLEAR_BREAKPOINT:		
			fpCommand	= cmdClearBreakpoint;
			fpResponse	= resClearBreakpoint;
			bInit		= FALSE;
			break;

		case CMD_CLEAR_ALL_BREAKPOINTS: 
			fpCommand	= cmdClearAllBreakpoints;
			fpResponse	= resClearAllBreakpoints;
			bInit		= FALSE;
			break;

		case CMD_SINGLE_STEP:			
		case CMD_CONTINUE:				
			fpCommand	= cmdContinue;
			fpResponse	= resContinue;
			bInit		= FALSE;
			break;

		case CMD_GET_VALUE: 			
			fpCommand	= cmdGetValue;
			fpResponse	= resGetValue;
			bInit		= FALSE;
			break;

		case CMD_SET_VALUE: 			
			fpCommand	= cmdSetValue;
			fpResponse	= resSetValue;
			bInit		= FALSE;
			break;	

		case CMD_DOWNLOAD_BEGIN:		
			fpCommand	= cmdDownloadBegin;
			fpResponse	= resDownloadBegin;
			bInit		= TRUE;
			break;

		case CMD_DOWNLOAD_CONFIG:		
			fpCommand	= cmdDownloadConfig;
			fpResponse	= resDownloadConfig;
			bInit		= TRUE;
			break;

		case CMD_DOWNLOAD_INITIAL:		
			fpCommand	= cmdDownloadInitial;
			fpResponse	= resDownloadInitial;
			bInit		= TRUE;
			break;

		case CMD_DOWNLOAD_CODE: 		
			fpCommand	= cmdDownloadCode;
			fpResponse	= resDownloadCode;
			bInit		= TRUE;
			break;

	  #if defined(RTS_CFG_CUSTOM_DL)
		case CMD_DOWNLOAD_CUSTOM:		
			fpCommand	= cmdDownloadCustom;
			fpResponse	= resDownloadCustom;
			bInit		= TRUE;
			break;
	  #endif

		case CMD_DOWNLOAD_FINISH:		
			fpCommand	= cmdDownloadFinish;
			fpResponse	= resDownloadFinish;
			bInit		= TRUE;
			break;

		case CMD_DOWNLOAD_END:			
			fpCommand	= cmdDownloadEnd;
			fpResponse	= resDownloadEnd;
			bInit		= TRUE;
			break;

		case CMD_DOWNLOAD_IOL:
			fpCommand	= cmdDownloadIOL;
			fpResponse	= resDownloadIOL;
			bInit		= TRUE;
			break;

		case CMD_DOWNLOAD_CLEAR:
			fpCommand	= cmdDownloadClear;
			fpResponse	= resDownloadClear;
			bInit		= TRUE;
			break;

		case CMD_INITIALIZE_CLEAR:		
			fpCommand	= cmdInitialize;
			fpResponse	= resInitialize;
			bInit		= FALSE;
			break;

	  #if defined(RTS_CFG_FLASH)
		case CMD_CLEAR_FLASH:			
			fpCommand	= cmdClearFlash;
			fpResponse	= resClearFlash;
			bInit		= FALSE;
			break;

		case CMD_FLASH:
			fpCommand	= cmdFlash;
			fpResponse	= resFlash;
			bInit		= FALSE;
			break;
	  #endif

	  #if defined(RTS_CFG_DEBUG_INFO)
		case CMD_DBI_GET_CHILDREN:		
			fpCommand	= cmdDBIGet;
			fpResponse	= resDBIGetChildren;
			bInit		= FALSE;
			break;

		case CMD_DBI_GET_ADDRESS:		
			fpCommand	= cmdDBIGet;
			fpResponse	= resDBIGetAddress;
			bInit		= FALSE;
			break;

		case CMD_DOWNLOAD_DEBUG:
			fpCommand	= cmdDownloadDebug;
			fpResponse	= resDownloadDebug;
			bInit		= TRUE;
			break;

		case CMD_DBI_GET_TASKNR:
			fpCommand	= cmdDBIGet;
			fpResponse	= resDBIGetTaskNr;
			bInit		= FALSE;
			break;
	  #endif

		case CMD_GET_PROJ_VERSION:
			fpCommand	= cmdGetProjectVersion;
			fpResponse	= resGetProjectVersion;
			bInit		= FALSE;
			break;

	  #if defined(RTS_CFG_EXT_PROJ_INFO)
		case CMD_GET_PROJ_INFO:
			fpCommand	= cmdGetProjectInfo;
			fpResponse	= resGetProjectInfo;
			bInit		= TRUE;
			break;
	  #endif

	  #if defined(RTS_CFG_STORE_PROJECT)
		case CMD_LOAD_PROJECT:
			fpCommand	= cmdLoadProject;
			fpResponse	= resLoadProject;
			bInit		= TRUE;
			break;

		case CMD_SAVE_PROJECT:
			fpCommand	= cmdSaveProject;
			fpResponse	= resSaveProject;
			bInit		= TRUE;
			break;
	  #endif

	  #if defined(RTS_CFG_STORE_FILES)
		case CMD_LOAD_FILE:
			fpCommand	= cmdLoadFile;
			fpResponse	= resLoadFile;
			bInit		= TRUE;
			break;

		case CMD_SAVE_FILE:
			fpCommand	= cmdSaveFile;
			fpResponse	= resSaveFile;
			bInit		= TRUE;
			break;

		case CMD_DIR_CONTENT:
			fpCommand	= cmdDir;
			fpResponse	= resDir;
			bInit		= TRUE;
			break;

		case CMD_DEL_FILE:
			fpCommand	= cmdDelFile;
			fpResponse	= resDelFile;
			bInit		= FALSE;
			break;
	  #endif

	  #if defined(RTS_CFG_CUSTOM_OC)
		case CMD_CUSTOM:
			fpCommand	= cmdCustom;
			fpResponse	= resCustom;
			bInit		= TRUE;
			break;
	  #endif

	  #if defined(RTS_CFG_ONLINE_CHANGE)
		case CMD_OC_BEGIN:
			fpCommand	= cmdOCBegin;
			fpResponse	= resOCBegin;
			bInit		= TRUE; 
			break;

		case CMD_OC_CONFIG:
			fpCommand	= cmdOCConfig;
			fpResponse	= resOCConfig;
			bInit		= TRUE;
			break;

		case CMD_OC_CODE:			
			fpCommand	= cmdOCCode;
			fpResponse	= resOCCode;
			bInit		= TRUE; 
			break;

		case CMD_OC_INITIAL:
			fpCommand	= cmdOCInitial; 		
			fpResponse	= resOCInitial; 		
			bInit		= TRUE; 
			break;

	  #if defined(RTS_CFG_CUSTOM_DL)
		case CMD_OC_CUSTOM:
			fpCommand	= cmdOCCustom;			
			fpResponse	= resOCCustom;			
			bInit		= TRUE; 
			break;
	  #endif

	  #if defined(RTS_CFG_DEBUG_INFO)
		case CMD_OC_DEBUG:
			fpCommand	= cmdOCDebug;				
			fpResponse	= resOCDebug;				
			bInit		= TRUE; 
			break;
	  #endif

		case CMD_OC_FINISH:
			fpCommand	= cmdOCFinish;
			fpResponse	= resOCFinish;
			bInit		= TRUE;
			break;

		case CMD_OC_COMMIT:
			fpCommand	= cmdOCCommit;
			fpResponse	= resOCCommit;
			bInit		= TRUE;
			break;

		case CMD_OC_END:
			fpCommand	= cmdOCEnd; 			
			fpResponse	= resOCEnd;
			bInit		= TRUE;
			break;
	  #endif

		case CMD_GET_CONFIG:
			fpCommand	= cmdGetConfig;
			fpResponse	= resGetConfig;
			bInit		= FALSE;
			break;

	  #if defined(RTS_CFG_EXT_RETAIN)
		case CMD_RET_WRITE:
			fpCommand	= cmdRetWrite;
			fpResponse	= resRetWrite;
			bInit		= FALSE;
			break;

		case CMD_RET_CYCLE:
			fpCommand	= cmdRetCycle;
			fpResponse	= resRetCycle;
			bInit		= FALSE;
			break;
	  #endif
			
	  #if defined(RTS_CFG_LICENSE)
		case CMD_GET_INSTKEY:
			fpCommand	= cmdGetInstKey;
			fpResponse	= resGetInstKey;
			bInit		= FALSE;
			break;

		case CMD_SET_LICKEY:
			fpCommand	= cmdSetLicKey;
			fpResponse	= resSetLicKey;
			bInit		= FALSE;
			break;

		case CMD_GET_SERIALNO:
			fpCommand	= cmdGetSerialNo;
			fpResponse	= resGetSerialNo;
			bInit		= FALSE;
			break;

		case CMD_GET_FEATURE:
			fpCommand	= cmdGetFeature;
			fpResponse	= resGetFeature;
			bInit		= FALSE;
			break;
	  #endif
			
		case CMD_GET_TYPE:
			fpCommand	= cmdGetType;
			fpResponse	= resGetType;
			bInit		= FALSE;
			break;

		case CMD_GET_VERSION:
			fpCommand	= cmdGetVersion;
			fpResponse	= resGetVersion;
			bInit		= FALSE;
			break;

	  #if defined(RTS_CFG_LICENSE)
		case CMD_SET_LICEX:
			fpCommand	= cmdSetLicEx;
			fpResponse	= resSetLicEx;
			bInit		= TRUE;
			break;
	  #endif
			
		default:
		{
			/* Unkown command
			 */
			fpCommand	= 0;
			fpResponse	= 0;

			uRes		= ERR_INVALID_COMMAND;

			break;
		}

	} /* switch (pBlock->CMD.byCommand) */

	if (uRes == OK && pBlock->uBlock == 1)
	{
		actClearDownload(pVMM, pBlock->CMD.byCommand);

		if (bInit == TRUE)
		{
			uRes = fpCommand(pVMM, NULL);
		}
	}

	if (uRes == OK)
	{
		uRes = fpCommand(pVMM, pBlock);
	}
	
	pBlock->uLen	= 0;

	if (pBlock->byLast != 0)
	{
		if (uRes == OK)
		{
			uRes = osOnCmdComputed(pVMM, pBlock);
		}

		if (uRes == OK && bInit == TRUE)
		{
			uRes = fpResponse(pVMM, NULL);
		}
		
		if (uRes == OK)
		{
			uRes = fpResponse(pVMM, pBlock);
		}
	}

	if (uRes != OK)
	{
		pVMM->DLB.uError = uRes;
		cmdErrorResponse(pVMM, pBlock);
	}

  #if defined(RTS_CFG_DEBUG_OUTPUT) & defined(RTS_CFG_COMM_TRACE)
	{
		IEC_UINT uCmd = pBlock->CMD.byCommand;
		osTrace("<<< %-8s %2d %5d\r\n", uCmd <= LAST_CMD_VALUE ? szCmdText[uCmd] : "-FAILED-", pBlock->uBlock, pBlock->uLen);
	}
  #endif

	osOnCmdHandled(pVMM, pBlock, uRes);

	RETURN(OK);
}

#endif /* RTS_CFG_PROT_CLIENT */

/* ---------------------------------------------------------------------------- */
/**
 * cmdBeforeExecute
 *
 */
#if defined(DEBUG)

IEC_UINT cmdBeforeExecute(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	/* Debug and bug tracing support!
	 */
	switch (pBlock->CMD.byCommand & ~0x80u)
	{
		case CMD_GET_STATE: 				break;
		case CMD_NOT_IMPLEMENTED:			break;
		case CMD_LOGIN: 					break;
		case CMD_LOGOUT:					break;
		case CMD_WARM_START:				break;
		case CMD_COLD_START:				break;
		case CMD_START_ALL_TASKS:			break;
		case CMD_STOP_ALL_TASKS:			break;
		case CMD_START_TASK:				break;
		case CMD_STOP_TASK: 				break;
		case CMD_OPEN_DEBUG_SESSION:		break;
		case CMD_CLOSE_DEBUG_SESSION:		break;
		case CMD_SET_BREAKPOINT:			break;
		case CMD_CLEAR_BREAKPOINT:			break;
		case CMD_CLEAR_ALL_BREAKPOINTS: 	break;
		case CMD_SINGLE_STEP:				break;
		case CMD_CONTINUE:					break;
		case CMD_GET_VALUE: 				break;
		case CMD_SET_VALUE: 				break;
		case CMD_DOWNLOAD_CONFIG:			break;
		case CMD_DOWNLOAD_INITIAL:			break;
		case CMD_DOWNLOAD_CODE: 			break;
		case CMD_DOWNLOAD_CUSTOM:			break;
		case CMD_DOWNLOAD_FINISH:			break;
		case CMD_START_RESOURCE:			break;
		case CMD_STOP_RESOURCE: 			break;
		case CMD_DOWNLOAD_END:				break;
		case CMD_DOWNLOAD_BEGIN:			break;
		case CMD_INITIALIZE_CLEAR:			break;
		case CMD_CLEAR_FLASH:				break;
		case CMD_DBI_GET_CHILDREN:			break;
		case CMD_DBI_GET_ADDRESS:			break;
		case CMD_DOWNLOAD_DEBUG:			break;
		case CMD_DBI_GET_TASKNR:			break;
		case CMD_GET_PROJ_VERSION:			break;
		case CMD_GET_PROJ_INFO: 			break;
		case CMD_LOAD_PROJECT:				break;
		case CMD_SAVE_PROJECT:				break;
		case CMD_CUSTOM:					break;
		case CMD_LOAD_FILE: 				break;
		case CMD_SAVE_FILE: 				break;
		case CMD_DIR_CONTENT:				break;
		case CMD_OC_BEGIN:					break;
		case CMD_OC_CODE:					break;
		case CMD_OC_DEBUG:					break;
		case CMD_OC_CUSTOM: 				break;
		case CMD_OC_INITIAL:				break;
		case CMD_OC_COMMIT: 				break;
		case CMD_OC_END:					break;
		case CMD_OC_CONFIG: 				break;
		case CMD_OC_FINISH: 				break;
		case CMD_FLASH: 					break;
		case CMD_DEL_FILE:					break;
		case CMD_GET_CONFIG:				break;
		case CMD_RET_WRITE: 				break;
		case CMD_RET_CYCLE: 				break;
		case CMD_GET_INSTKEY:				break;
		case CMD_SET_LICKEY:				break;
		case CMD_GET_SERIALNO:				break;
		case CMD_GET_FEATURE:				break;
		case CMD_GET_TYPE:					break;
		case CMD_GET_VERSION:				break;
		case CMD_SET_LICEX: 				break;
		case CMD_DOWNLOAD_IOL:				break;
		case CMD_DOWNLOAD_CLEAR:			break;

		default:							RETURN(ERR_INVALID_COMMAND);
	
	} /* switch (pBlock->CMD.byCommand) */

	RETURN(OK);
}

#endif /* DEBUG */

/* ---------------------------------------------------------------------------- */
/**
 * cmdAfterExecute
 *
 */
#if defined(DEBUG)

IEC_UINT cmdAfterExecute(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	/* Debug and bug tracing support!
	 */
	switch (pBlock->CMD.byCommand & ~0x80u)
	{
		case CMD_GET_STATE: 				break;
		case CMD_NOT_IMPLEMENTED:			break;
		case CMD_LOGIN: 					break;
		case CMD_LOGOUT:					break;
		case CMD_WARM_START:				break;
		case CMD_COLD_START:				break;
		case CMD_START_ALL_TASKS:			break;
		case CMD_STOP_ALL_TASKS:			break;
		case CMD_START_TASK:				break;
		case CMD_STOP_TASK: 				break;
		case CMD_OPEN_DEBUG_SESSION:		break;
		case CMD_CLOSE_DEBUG_SESSION:		break;
		case CMD_SET_BREAKPOINT:			break;
		case CMD_CLEAR_BREAKPOINT:			break;
		case CMD_CLEAR_ALL_BREAKPOINTS: 	break;
		case CMD_SINGLE_STEP:				break;
		case CMD_CONTINUE:					break;
		case CMD_GET_VALUE: 				break;
		case CMD_SET_VALUE: 				break;
		case CMD_DOWNLOAD_CONFIG:			break;
		case CMD_DOWNLOAD_INITIAL:			break;
		case CMD_DOWNLOAD_CODE: 			break;
		case CMD_DOWNLOAD_CUSTOM:			break;
		case CMD_DOWNLOAD_FINISH:			break;
		case CMD_START_RESOURCE:			break;
		case CMD_STOP_RESOURCE: 			break;
		case CMD_DOWNLOAD_END:				break;
		case CMD_DOWNLOAD_BEGIN:			break;
		case CMD_INITIALIZE_CLEAR:			break;
		case CMD_CLEAR_FLASH:				break;
		case CMD_DBI_GET_CHILDREN:			break;
		case CMD_DBI_GET_ADDRESS:			break;
		case CMD_DOWNLOAD_DEBUG:			break;
		case CMD_DBI_GET_TASKNR:			break;
		case CMD_GET_PROJ_VERSION:			break;
		case CMD_GET_PROJ_INFO: 			break;
		case CMD_LOAD_PROJECT:				break;
		case CMD_SAVE_PROJECT:				break;
		case CMD_CUSTOM:					break;
		case CMD_LOAD_FILE: 				break;
		case CMD_SAVE_FILE: 				break;
		case CMD_DIR_CONTENT:				break;
		case CMD_OC_BEGIN:					break;
		case CMD_OC_CODE:					break;
		case CMD_OC_DEBUG:					break;
		case CMD_OC_CUSTOM: 				break;
		case CMD_OC_INITIAL:				break;
		case CMD_OC_COMMIT: 				break;
		case CMD_OC_END:					break;
		case CMD_OC_CONFIG: 				break;
		case CMD_OC_FINISH: 				break;
		case CMD_FLASH: 					break;
		case CMD_DEL_FILE:					break;
		case CMD_GET_CONFIG:				break;
		case CMD_RET_WRITE: 				break;
		case CMD_RET_CYCLE: 				break;
		case CMD_FW_DOWNLOAD:				break;
		case CMD_FW_EXECUTE:				break;
		case CMD_FW_RESULT: 				break;
		case CMD_RET_UPLOAD:				break;
		case CMD_RET_DOWNLOAD:				break;
		case CMD_IEC_UPLOAD:				break;
		case CMD_IEC_DOWNLOAD:				break;
		case CMD_GET_INSTKEY:				break;
		case CMD_SET_LICKEY:				break;
		case CMD_GET_SERIALNO:				break;
		case CMD_GET_FEATURE:				break;
		case CMD_GET_TYPE:					break;
		case CMD_GET_VERSION:				break;
		case CMD_SET_LICEX: 				break;
		case CMD_DOWNLOAD_IOL:				break;
		case CMD_DOWNLOAD_CLEAR:			break;

		default:							RETURN(ERR_INVALID_COMMAND);
	
	} /* switch (pBlock->CMD.byCommand) */

	RETURN(OK);
}

#endif /* DEBUG */

/* ---------------------------------------------------------------------------- */
