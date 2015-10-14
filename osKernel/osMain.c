
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
 * Filename: osMain.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"osMain.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

#include "fcDef.h"

#include <sys/mman.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <sys/timeb.h>
#include <sys/reboot.h>
#if defined(RTS_CFG_IOCANOPEN)
#include <libCanOpen.h>
#endif
#if defined(RTS_CFG_MECT_LIB)
#include <libMect.h>
#endif
#if defined(RTS_CFG_USB_LIB)
#include <libUSB.h>
#endif
#if defined(RTS_CFG_DATALOG_LIB)
#include <libDatalog.h>
#endif
#if defined(RTS_CFG_HW119_LIB)
#include <libHW119.h>
#endif
#if defined (RTS_CFG_MODBUS_LIB)
#include <libModbus.h>
#endif
/* ----  Local Defines:   ----------------------------------------------------- */

#if defined(RTS_CFG_FILE_NATIVE)
	#define xxxClose			fileClose
#endif

#if defined(RTS_CFG_FILE_ACCESS)
	#define xxxClose			osClose
#endif

/* ----  Global Variables:	 -------------------------------------------------- */

static SObject g_segGlobal	= {NULL,0};
static SObject g_segRetain	= {NULL,0};
static SObject g_segInput	= {NULL,0};
static SObject g_segOutput	= {NULL,0};
#if ! defined(RTS_CFG_FFO)
  static SObject g_segLocal = {NULL,0};
  static SObject g_segCode	= {NULL,0};
#endif
#if defined(RTS_CFG_WRITE_FLAGS_PI)
  static SObject g_segWF	= {NULL,0};
#endif

#if ! defined(RTS_CFG_FFO)
  static IEC_CHAR g_pMessageBuffer[MAX_STR_MSG_LEN * MAX_STR_MSG_QUEUE];
#endif

IEC_BOOL g_bSilentMode			= FALSE;
IEC_BOOL g_bDownloadInProgress	= FALSE;
IEC_BOOL g_bDownloadFailed		= FALSE;

#if defined(RTS_CFG_IO_LAYER)
extern IEC_BOOL g_bIOConfigFailed;
extern IEC_BOOL g_bIOConfInProgress;
#endif

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * osMain			
 *
 * Main entry function for the Virtual Machine Manager (VMM).
 *
 * @return			OK if successful, else error number
 */
IEC_UINT osMain(IEC_UINT argc, IEC_CHAR *argv[])
{
	IEC_UINT uRes = OK;
	IEC_UINT i;

  #if defined(RTS_CFG_TASK_TRACE)
	osTrace("--- TSK: Task '%s' created with pid %d from ppid %d.\r\n", "SYS_VMM", getpid(), getppid());
  #endif

	/* Check parameters 
	 */
	for (i = 1; i < argc; i++)
	{
		if (OS_STRICMP(argv[i], "-ver") == 0)
		{
			IEC_CHAR szVersion[VMM_MAX_IEC_STRLEN + HD_IEC_STRING];

			uRes = sysInitialize();
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			uRes = sysGetVersionInfo(szVersion);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			fprintf(stdout, "%s\r\n", szVersion);

			RETURN(OK);
		}
		
		if (OS_STRICMP(argv[i], "-silent") == 0)
		{
			g_bSilentMode = TRUE;
		}
	}

  #if defined(_SOF_4CFC_SRC_)

	/* Test Watchdog
	 */
	int hWatchdog = -1;

	hWatchdog = open(FC_WATCHDOG_DEVICE, O_RDWR, 0);
	if (hWatchdog == -1)
	{
		RETURN(ERR_ERROR);
	}
	
	close(hWatchdog);
  #endif

	XX_GPIO_INIT();
	#if ! defined(RTS_CFG_MULTILINK)

		uRes = vmmMainMT();

	#endif

	#if defined(RTS_CFG_VM_IPC) && defined(RTS_CFG_MULTILINK)

		uRes = vmmMainIPC();

	#endif
	XX_GPIO_CLOSE();

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osOnCmdReceived
 *
 * This function is called after a command block is received and 
 * before it is handled by the kernel.
 *
 * @return			WRN_HANDLED if command is already handled and 
 *					should not be handled by the kernel.
 *					OK if successful else error number.
 */
IEC_UINT osOnCmdReceived(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	switch (pBlock->CMD.byCommand)
	{
		case CMD_DOWNLOAD_BEGIN:
			g_bDownloadInProgress = TRUE;
			g_bDownloadFailed	  = FALSE;
			if(pVMM->bProjLoaded){
#if defined(RTS_CFG_MECT_LIB)
				app_mect_done();
#endif
#if defined(RTS_CFG_IOCANOPEN)
#endif
#if defined(RTS_CFG_USB_LIB)
#endif
#if defined(RTS_CFG_DATALOG_LIB)
#endif
#if defined(RTS_CFG_HW119_LIB)
#endif
#if defined (RTS_CFG_MODBUS_LIB)
#endif
			}
#if defined(RTS_CFG_IO_LAYER)
			g_bIOConfigFailed 	  = FALSE;
			g_bIOConfInProgress	  = FALSE;
#endif
			break;

		case CMD_DOWNLOAD_CONFIG:
		case CMD_DOWNLOAD_INITIAL:
		case CMD_DOWNLOAD_CODE:
		case CMD_DOWNLOAD_CUSTOM:
		case CMD_DOWNLOAD_FINISH:
		case CMD_DOWNLOAD_DEBUG:
		case CMD_DOWNLOAD_IOL:
		case CMD_DOWNLOAD_CLEAR:
		case CMD_OC_BEGIN:
		case CMD_OC_CODE:
		case CMD_OC_DEBUG:
		case CMD_OC_CUSTOM:
		case CMD_OC_INITIAL:
		case CMD_OC_COMMIT:
		case CMD_OC_CONFIG:
		case CMD_OC_FINISH:
			g_bDownloadInProgress = TRUE;
			g_bDownloadFailed	  = FALSE;
			break;
			
		case CMD_DOWNLOAD_END:
			if(pVMM->bProjLoaded){
#if defined(RTS_CFG_MECT_LIB)
				app_mect_init();
#endif
#if defined(RTS_CFG_IOCANOPEN)
#endif
			}		

		case CMD_OC_END:
			g_bDownloadInProgress = FALSE;
			g_bDownloadFailed	  = FALSE;
			break;

		default:
			break;
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osOnCmdComputed
 *
 * This function is called for the client/server protocol once for each computed 
 * command sequence .(-> Before the response computation is called the first time.)
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osOnCmdComputed(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	switch (pBlock->CMD.byCommand)
	{
		case CMD_DOWNLOAD_END:
		case CMD_OC_END:
		{
			/* Flush file buffer for DBI and PB config files 
			 */ 		
			sync();
		}
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osOnRespReceived
 *
 * This function is called after a request block for a response is 
 * received and before the response is computed by the kernel.
 *
 * @return			WRN_HANDLED if response is already built and 
 *					should not be built by the kernel.
 *					OK if successful else error number.
 */
#if defined(RTS_CFG_PROT_BLOCK)

IEC_UINT osOnRespReceived(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	
	RETURN(uRes);
}
#endif	/* RTS_CFG_PROT_BLOCK */

/* ---------------------------------------------------------------------------- */
/**
 * osOnCmdHandled
 *
 * This function is called after a command block is handled by the 
 * kernel. uResult represents the result of the corresponding kernel
 * function.
 *
 * @return			OK if successful else error number.
 */

IEC_UINT osOnCmdHandled(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uResult)
{
	IEC_UINT uRes = OK;

	switch (pBlock->CMD.byCommand & 0x7fu)
	{
		case CMD_DOWNLOAD_CONFIG:
		case CMD_DOWNLOAD_INITIAL:
		case CMD_DOWNLOAD_CODE:
		case CMD_DOWNLOAD_CUSTOM:
		case CMD_DOWNLOAD_FINISH:
		case CMD_DOWNLOAD_BEGIN:
		case CMD_DOWNLOAD_DEBUG:
		case CMD_DOWNLOAD_IOL:
		case CMD_DOWNLOAD_CLEAR:
		case CMD_OC_BEGIN:
		case CMD_OC_CODE:
		case CMD_OC_DEBUG:
		case CMD_OC_CUSTOM:
		case CMD_OC_INITIAL:
		case CMD_OC_COMMIT:
		case CMD_OC_CONFIG:
		case CMD_OC_FINISH:
			g_bDownloadFailed	  = (IEC_BOOL)(pBlock->CMD.byCommand > 0x7fu);
			g_bDownloadInProgress = (IEC_BOOL)(g_bDownloadFailed == FALSE);
			break;
			
		case CMD_DOWNLOAD_END:
		case CMD_OC_END:
			g_bDownloadFailed	  = (IEC_BOOL)(pBlock->CMD.byCommand > 0x7fu);
			g_bDownloadInProgress = FALSE;
			break;

		default:
			break;
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osOnRespHandled
 *
 * This function is called after a response block is computed by the
 * kernel. uResult represents the result of the corresponding kernel
 * function.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_PROT_BLOCK)

IEC_UINT osOnRespHandled(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uResult)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}
#endif	/* RTS_CFG_PROT_BLOCK */

/* ---------------------------------------------------------------------------- */
/**
 * osCmdCustom
 *
 * This function is called to handle a received data block via the
 * custom download command.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_CUSTOM_OC)

IEC_UINT osCmdCustom(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	if (pBlock == 0)
	{
		/* Initialize
		 */
		RETURN(OK);
	}

	/* Handle command
	 */

	RETURN(uRes);
}
#endif	/* RTS_CFG_CUSTOM_OC */

/* ---------------------------------------------------------------------------- */
/**
 * osResCustom
 *
 * This function is called to generate a response to a received 
 * custom download command.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_CUSTOM_OC)

IEC_UINT osResCustom(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	if (pBlock == 0)
	{
		/* Initialize
		 */
		RETURN(OK);
	}

	/* Handle response
	 */

	pBlock->uLen	= 0;
	pBlock->byLast	= TRUE;

	RETURN(uRes);
}
#endif	/* RTS_CFG_CUSTOM_OC */

/* ---------------------------------------------------------------------------- */
/**
 * osBeginGetTaskImage
 * 
 * This function is called before the process image is copied into the
 * task image of the given task.
 *
 * @return			OK if successful else error number.
 *					WRN_HANDLED if copying is already done here and should not
 *					be handled by the RTS kernel.
 */
#if defined(RTS_CFG_TASK_IMAGE)

IEC_UINT osBeginGetTaskImage(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}
#endif	/* RTS_CFG_TASK_IMAGE */

/* ---------------------------------------------------------------------------- */
/**
 * osEndGetTaskImage
 *
 * This function is called after the process image is copied into the
 * task image of the given task.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_TASK_IMAGE)

IEC_UINT osEndGetTaskImage(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}
#endif	/* RTS_CFG_TASK_IMAGE */

/* ---------------------------------------------------------------------------- */
/**
 * osBeginSetTaskImage
 *
 * This function is called before the task image of the given task is
 * copied back to the process image.
 *
 * @return			OK if successful else error number.
 *					WRN_HANDLED if copying is already done here and should not
 *					be handled by the RTS kernel.
 */
#if defined(RTS_CFG_TASK_IMAGE)

IEC_UINT osBeginSetTaskImage(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}
#endif	/* RTS_CFG_TASK_IMAGE */

/* ---------------------------------------------------------------------------- */
/**
 * osEndSetTaskImage
 *
 * This function is called after the task image of the given task is 
 * copied back to the process image.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_TASK_IMAGE)

IEC_UINT osEndSetTaskImage(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}
#endif	/* RTS_CFG_TASK_IMAGE */

/* ---------------------------------------------------------------------------- */
/**
 * osInitializeSegment
 *
 * Initialize the global memory segments if necessary. (If these 
 * informations are known by the engineering, the segment informations 
 * are already set by the download mechanism.)
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osInitializeSegment(IEC_UINT uSegment, SObject *pSegment)
{
	IEC_UINT uRes	= OK;
	SObject *pGlob	= NULL;

	if (pSegment->ulSize == 0)
	{
		/* Empty segment, Linux doesn't allocate zero sized memory.
		 */
		pSegment->pAdr = NULL;

		RETURN(OK);
	}

	/* Select global archive objects
	 * ------------------------------------------------------------------------
	 */
	switch (uSegment)
	{
		case SEG_GLOBAL:
			pGlob = &g_segGlobal;
			break;

		case SEG_RETAIN:
		{
		  #if defined(_SOF_4CFC_SRC_)

			pGlob = &g_segRetain;
			break;
		  
		  #endif /* _SOF_4CFC_SRC_ */

		  #if ! defined(RTS_CFG_EXT_RETAIN)

			if (g_segRetain.pAdr != NULL)
			{
				pSegment->pAdr	 = g_segRetain.pAdr;
				pSegment->ulSize = g_segRetain.ulSize;

				RETURN(OK);
			}
		  #endif

			pGlob = &g_segRetain;
			break;
		}

		case SEG_INPUT:
			pGlob = &g_segInput;
			break;

		case SEG_OUTPUT:
			pGlob = &g_segOutput;
			break;

		case SEG_WRITEF:
		  #if defined(RTS_CFG_WRITE_FLAGS_PI)
			pGlob = &g_segWF;
		  #endif
			break;

		case SEG_CODE:
		  #if ! defined(RTS_CFG_FFO)
			pGlob = &g_segCode;
		  #endif
			break;

		case SEG_LOCAL:
		  #if ! defined(RTS_CFG_FFO)
			pGlob = &g_segLocal;
		  #endif
			break;

		default:
			RETURN(ERR_ERROR);
	
	} /* switch (uSegment) */

	if (pGlob == NULL)
	{
		/* Segment not used.
		 */
		pSegment->pAdr	 = NULL;
		pSegment->ulSize = 0;

		RETURN(OK);
	}

	
	/* Release old memory
	 * ------------------------------------------------------------------------
	 */
	if (pGlob->pAdr != NULL && pGlob->ulSize != pSegment->ulSize)
	{
		switch(uSegment)
		{
		case SEG_RETAIN:
		  #if defined(_SOF_4CFC_SRC_)

			munmap(pGlob->pAdr, pGlob->ulSize);
			break;

		  #endif /* _SOF_4CFC_SRC_ */

		  #if defined(RTS_CFG_EXT_RETAIN)
			uRes = osFree(&pGlob->pAdr);
		  #endif
			break;

		case SEG_INPUT:
		case SEG_OUTPUT:
		case SEG_WRITEF:
			uRes = osFree(&pGlob->pAdr);
			break;

		default:
			uRes = osFree(&pGlob->pAdr);
			break;
		
		} /* switch (uSegment) */

		if (uRes != OK)
		{
			pSegment->pAdr		= NULL;
			pSegment->ulSize	= 0;
			
			RETURN(uRes);
		}

		pGlob->pAdr 	= NULL;
		pGlob->ulSize	= 0;
	
	} /* if (pGlob->pAdr != NULL) */


	/* Allocate new memory
	 * ------------------------------------------------------------------------
	 */
	if (pGlob->pAdr == NULL)
	{
		switch(uSegment)
		{
		case SEG_RETAIN:
		{
		  #if defined(_SOF_4CFC_SRC_)

			int fd = open(FC_DEV_NVRAM, O_RDWR);
			if (fd == -1)
			{
			  #if defined(RTS_CFG_DEBUG_OUTPUT)
				osTrace("[NVR] Open of '%s' failed (%d).\r\n", FC_DEV_NVRAM, errno);
			  #endif
				RETURN(ERR_FILE_OPEN);
			}
			
			pSegment->pAdr = mmap(0, pSegment->ulSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			if (pSegment->pAdr == MAP_FAILED)
			{
				close(fd);

			  #if defined(RTS_CFG_DEBUG_OUTPUT)
				osTrace("[NVR] mmap() to '%s' failed (%d).\r\n", FC_DEV_NVRAM, errno);
			 #endif
				RETURN(ERR_OUT_OF_MEMORY);
			}
			
			close(fd);
			break;

		#endif /* _SOF_4CFC_SRC_ */
		
			pSegment->pAdr = osMalloc(pSegment->ulSize);
			break;			
		}

		case SEG_INPUT:
		case SEG_OUTPUT:
		case SEG_WRITEF:
			pSegment->pAdr = osMalloc(pSegment->ulSize);
			break;

		default:
			pSegment->pAdr = osMalloc(pSegment->ulSize);
			break;
		
		} /* switch (uSegment) */
		
		if (uRes != OK)
		{
			pSegment->pAdr		= NULL;
			pSegment->ulSize	= 0;

			RETURN(uRes);
		}

		if (pSegment->pAdr == NULL)
		{
			pSegment->pAdr		= NULL;
			pSegment->ulSize	= 0;

			RETURN(ERR_OUT_OF_MEMORY);
		}
	
	} /* if (pGlob->pAdr == NULL) */

	else
	{
		pSegment->pAdr		= pGlob->pAdr;
		pSegment->ulSize	= pGlob->ulSize;
	}

	
	/* Clear memory
	 * ------------------------------------------------------------------------
	 */
	switch(uSegment)
	{
	case SEG_RETAIN:
	  #if defined(_SOF_4CFC_SRC_)

		break;

	  #endif /* _SOF_4CFC_SRC_ */

		OS_MEMSET(pSegment->pAdr, 0x00, pSegment->ulSize);
		break;
		
	default:
		OS_MEMSET(pSegment->pAdr, 0x00, pSegment->ulSize);
		break;

	} /* switch (uSegment) */


	/* Done...
	 */
	pGlob->pAdr 	= pSegment->pAdr;
	pGlob->ulSize	= pSegment->ulSize;
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osCreateMsgStr
 *
 * @return			New message if successful else NULL.
 */
#if ! defined(RTS_CFG_FFO)

IEC_CHAR *osCreateMsgStr(IEC_UINT uMessage)
{	
	if (uMessage >= MAX_STR_MSG_QUEUE)
	{
		return 0;
	}

	return g_pMessageBuffer + uMessage * MAX_STR_MSG_LEN;
}
#endif /* ! RTS_CFG_FFO */

/* ---------------------------------------------------------------------------- */
/**
 * osFreeMsgStr
 *
 * @return			OK if successful else error number.
 */
#if ! defined(RTS_CFG_FFO)

IEC_UINT osFreeMsgStr(IEC_UINT uMessage)
{
	IEC_UINT uRes = OK;

	if (uMessage >= MAX_STR_MSG_QUEUE)
	{
		RETURN(ERR_INVALID_PARAM);
	}


	RETURN(uRes);
}
#endif /* ! RTS_CFG_FFO */

/* ---------------------------------------------------------------------------- */
/**
 * osCreateLocSegment
 *
 * Creates a local segment. (A local segment is a copy of a segment
 * of the global process image used as task image for the given task.)
 *
 * @return			Pointer to the new local segment or 0 if failed.
 */
#if defined (RTS_CFG_TASK_IMAGE)

IEC_DATA OS_DPTR *osCreateLocSegment(IEC_UINT uTask, IEC_UINT uSegment, IEC_UDINT ulSize)
{
	IEC_DATA OS_DPTR *pSegment = osMalloc(ulSize);

	OS_MEMSET(pSegment, 0x00, ulSize);

	return pSegment;
}
#endif	/* RTS_CFG_TASK_IMAGE */

/* ---------------------------------------------------------------------------- */
/**
 * osFreeLocSegment
 *
 * Frees a local segment.
 *
 * @return			OK if successful else error number.
 */
#if defined (RTS_CFG_TASK_IMAGE)

IEC_UINT osFreeLocSegment(IEC_UINT uTask, IEC_UINT uSegment, IEC_DATA OS_DPTR **ppSegment)
{
	IEC_UINT uRes = OK;

	if (*ppSegment)
	{
		uRes = osFree(ppSegment);
	}

	RETURN(uRes);
}
#endif	/* RTS_CFG_TASK_IMAGE */

/* ---------------------------------------------------------------------------- */
/**
 * osCreateWFSegment
 *
 * Creates a change flag segment. (A change flag segment is of the 
 * same size as the corresponding local segment and is used to store
 * the changed flags.)
 *
 * @return			Pointer to new change flag segment or 0 if failed.
 *
 */
#if defined (RTS_CFG_WRITE_FLAGS)

IEC_DATA OS_DPTR *osCreateWFSegment(IEC_UINT uTask, IEC_UDINT ulSize)
{
	IEC_DATA OS_DPTR *pSegment = osMalloc(ulSize);

	OS_MEMSET(pSegment, 0x00, ulSize);

	return pSegment;
}
#endif	/* RTS_CFG_WRITE_FLAGS */

/* ---------------------------------------------------------------------------- */
/**
 * osFreeWFSegment
 *
 * Frees a change flag segment.
 *
 * @return			OK if successful else error number.
 */
#if defined (RTS_CFG_WRITE_FLAGS)

IEC_UINT osFreeWFSegment(IEC_UINT uTask, IEC_DATA OS_DPTR** ppSegment)
{
	IEC_UINT uRes = OK;

	if (*ppSegment != NULL)
	{
		uRes = osFree(ppSegment);
	}

	RETURN(uRes);
}
#endif	/* RTS_CFG_WRITE_FLAGS */

/* ---------------------------------------------------------------------------- */
/**
 * osHandleException
 *
 * This function is called, when the interpreter raises an exception.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osHandleException(STaskInfoVM *pVM, SException *pException)
{
	IEC_UINT uRes = OK;
	

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osNotifySetValue
 *
 * This function is called when a variable is written by the Engineering.
 *
 * @return			OK if successful else error number.
 *					WRN_HANDLED if copying is already done here and should not
 *					be handled by the RTS kernel.
 */
IEC_UINT osNotifySetValue(STaskInfoVMM *pVMM, IEC_DATA *pVal, IEC_UINT uSegment, IEC_UDINT ulOffset, IEC_UINT uLen, IEC_USINT usBit)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osNotifyGetValue
 *
 * This function is called when a variable is read by the Engineering.
 *
 * @return			OK if successful else error number.
 *					WRN_HANDLED if copying is already done here and should not
 *					be handled by the RTS kernel.
 */
IEC_UINT osNotifyGetValue(STaskInfoVMM *pVMM, IEC_DATA *pVal, IEC_UINT uSegment, IEC_UDINT ulOffset, IEC_UINT uLen, IEC_USINT usBit)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetProjectDir
 *
 * This function is called to determine the directory for the IEC
 * project storage.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_STORE_PROJECT)

IEC_UINT osGetProjectDir(IEC_CHAR *szDir, IEC_UINT uSize)
{
	IEC_UINT uRes = OK;

	OS_STRCPY(szDir, FC_ROOT_DIRECTORY);

	RETURN(uRes);
}
#endif /* RTS_CFG_STORE_PROJECT */

/* ---------------------------------------------------------------------------- */
/**
 * osGetFileDir
 *
 * This function is called to determine the directory for the
 * additional file storage.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_STORE_FILES)

IEC_UINT osGetFileDir(IEC_CHAR *szDir, IEC_UINT uSize)
{
	IEC_UINT uRes = OK;

	OS_STRCPY(szDir, FC_ROOT_DIRECTORY);

	RETURN(uRes);
}
#endif /* RTS_CFG_STORE_FILES */

/* ---------------------------------------------------------------------------- */
/**
 * osGetDBIDir
 *
 * This function is called to determine the directory for the debug
 * information storage.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_DEBUG_INFO)

IEC_UINT osGetDBIDir(IEC_CHAR *szDir, IEC_UINT uSize)
{
	IEC_UINT uRes = OK;

	OS_STRCPY(szDir, FC_ROOT_DIRECTORY);

	RETURN(uRes);
}
#endif /* RTS_CFG_DEBUG_INFO */

/* ---------------------------------------------------------------------------- */
/**
 * osGetCustDownDir
 *
 * This function is called to determine the directory for the custom
 * download file storage.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_CUSTOM_DL)

IEC_UINT osGetCustDownDir(IEC_CHAR *szDir, IEC_UINT uSize)
{
	IEC_UINT uRes = OK;

	OS_STRCPY(szDir, FC_ROOT_DIRECTORY);

	RETURN(uRes);
}
#endif /* RTS_CFG_CUSTOM_DL */

/* ---------------------------------------------------------------------------- */
/**
 * osGetFlashDir
 *
 * This function is called to determine the directory for the flash
 * data file storage.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_FLASH)

IEC_UINT osGetFlashDir(IEC_CHAR *szDir, IEC_UINT uSize)
{
	IEC_UINT uRes = OK;

	OS_STRCPY(szDir, FC_ROOT_DIRECTORY);

	RETURN(uRes);
}
#endif /* RTS_CFG_FLASH */

/* ---------------------------------------------------------------------------- */
/**
 * osGetTraceDir
 *
 * This function is called to determine the directory for the trace
 * data file storage.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_DEBUG_FILE) || defined(RTS_CFG_BACNET)

IEC_UINT osGetTraceDir(IEC_CHAR *szDir, IEC_UINT uSize)
{
	IEC_UINT uRes = OK;

	OS_STRCPY(szDir, FC_ROOT_DIRECTORY);

	RETURN(uRes);
}
#endif /* RTS_CFG_DEBUG_FILE */

/* ---------------------------------------------------------------------------- */
/**
 * osGetRetainDir
 *
 * This function is called to determine the directory for the retain memory
 * data file storage.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_EXT_RETAIN)

IEC_UINT osGetRetainDir(IEC_CHAR *szDir, IEC_UINT uSize)
{
	IEC_UINT uRes = OK;

	OS_STRCPY(szDir, FC_ROOT_DIRECTORY);

	RETURN(uRes);
}
#endif /* RTS_CFG_EXT_RETAIN */

/* ---------------------------------------------------------------------------- */
/**
 * osGetLoadDir
 *
 * This function is called to determine the directory for the system load
 * data file storage.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_SYSLOAD)

IEC_UINT osGetLoadDir(IEC_CHAR *szDir, IEC_UINT uSize)
{
	IEC_UINT uRes = OK;

	OS_STRCPY(szDir, FC_ROOT_DIRECTORY);

	RETURN(uRes);
}
#endif /* RTS_CFG_SYSLOAD */

/* ---------------------------------------------------------------------------- */
/**
 * osGetBACnetDBDir
 *
 * This function is called to determine the directory for the system load
 * data file storage.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_BACNET)

IEC_UINT osGetBACnetDBDir(IEC_CHAR *szDir, IEC_UINT uSize)
{
	IEC_UINT uRes = OK;

	OS_STRCPY(szDir, FC_FLASH_DIRECTORY);

	RETURN(uRes);
}
#endif /* RTS_CFG_BACNET */

/* ---------------------------------------------------------------------------- */
/**
 * osGetBACnetFlashDir
 *
 * This function is called to determine the directory for the system load
 * data file storage.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_BACNET)

IEC_UINT osGetBACnetFlashDir(IEC_CHAR *szDir, IEC_UINT uSize)
{
	IEC_UINT uRes = OK;

	OS_STRCPY(szDir, FC_FLASH_DIRECTORY);

	RETURN(uRes);
}
#endif /* RTS_CFG_BACNET */

/* ---------------------------------------------------------------------------- */
/**
 * osCheckFilePath
 *
 * Check if the given file path is a valid path for the given operating system.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_FILE_NATIVE) || defined(RTS_CFG_FILE_ACCESS)

IEC_UINT osCheckFilePath(IEC_CHAR *szPath, IEC_UINT uCmd)
{
	IEC_UINT uRes = OK;
	IEC_UINT i;
	IEC_UINT uLen = OS_STRLEN(szPath);

	for (i = 0; i < uLen; i++)
	{
		if (szPath[i] == '\\')
		{
			szPath[i] = '/';
		}
	}
	
	switch(uCmd)
	{
	case CMD_LOAD_FILE:
	case CMD_SAVE_FILE:
	case CMD_DEL_FILE:
	case CMD_DOWNLOAD_CUSTOM:
		/* IEC BOOL bRes = utilIs_8_3(szPath);
		 * RETURN((IEC_UINT)(bRes ? OK : ERR_PATH_IS_NOT_8_3));
		 */
		RETURN(OK);

	default:
		RETURN(ERR_INVALID_PATH);
	}

	RETURN(uRes);
}
#endif /* RTS_CFG_FILE_NATIVE || RTS_CFG_FILE_ACCESS */

/* ---------------------------------------------------------------------------- */
/**
 * osGetFreeDiskSpace
 *
 * Get the free disk space in kilo bytes.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_FILE_LIB)

IEC_UINT osGetFreeDiskSpace(IEC_CHAR *szPath, IEC_UDINT *ulpFree)
{
	IEC_UINT uRes = OK;

	struct statfs status;
		
	if(statfs(szPath, &status))
	{
		*ulpFree = (IEC_UDINT)-1;
		RETURN(ERR_ERROR);		
	}

	*ulpFree = status.f_bavail * status.f_bsize / 1024;

	RETURN(uRes);
}
#endif /* RTS_CFG_FILE_LIB */

/* ---------------------------------------------------------------------------- */
/**
 * osTriggerSystemWatchdog
 *
 * Trigger system (still alive) watchdog.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_VM_IPC)

IEC_UINT osTriggerSystemWatchdog(void)
{
	IEC_UINT uRes = OK;

	uRes = fcSetSystemWatchdog();

	RETURN(uRes);
}
#endif	/* RTS_CFG_VM_IPC */

/* ---------------------------------------------------------------------------- */
/**
 * osReboot
 *
 * Reboots the control.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osReboot(void)
{
	IEC_UINT uRes = OK;

	sync();
	osSleep(10000);
	uRes = reboot(RB_AUTOBOOT);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osCmdSetLicEx
 *
 */
#if defined(RTS_CFG_LICENSE)

IEC_UINT osCmdSetLicEx(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	XLicEx *pLE = NULL;

	if (pBlock == NULL)
	{
		/* ... */

		RETURN(OK);
	}

	pLE = (XLicEx *)pBlock->CMD.pData;

	switch (pLE->uLicense)
	{

		/* ... */

		default:
			RETURN(ERR_INVALID_LICENSE);
			break;
	
	} /* switch (pLE->uLicense) */

	RETURN(uRes);
}

#endif /* RTS_CFG_LICENSE */

/* ---------------------------------------------------------------------------- */
/**
 * osResSetLicEx
 *
 */
#if defined(RTS_CFG_LICENSE)

IEC_UINT osResSetLicEx(STaskInfoVMM *pVMM, XBlock *pBlock)
{
	IEC_UINT uRes = OK;

	if (pBlock == NULL)
	{
		/* ... */

		RETURN(OK);
	}

	/* ... */

	RETURN(uRes);
}

#endif /* RTS_CFG_LICENSE */

/* ---------------------------------------------------------------------------- */
