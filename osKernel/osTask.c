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
 * Filename: osTask.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"osTask.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_TCP_NATIVE)
  #include "osSocket.h"
#endif

#if defined(RTS_CFG_IOCANOPEN)
#include "mectCfgUtil.h"
#endif

/* ----  Target Specific Includes:	 ------------------------------------------ */

#include "fcMain.h"
#include "fcDef.h"
#include "libMBus2.h"

#include <sys/mman.h>
#include "mectMain.h"

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

static STaskInfoVMM *the_pVMM = NULL;

/* Task Informations
 * ----------------------------------------------------------------------------
 */

/* Global informations
 */
#if ! defined(RTS_CFG_FFO)
  static STaskInfoVMM	g_TaskInfoVMM;
  static SObject		g_pCodeObj[MAX_CODE_OBJ];
#endif

/* Task local informations
 */
#if ! defined(RTS_CFG_FFO)
  static STaskInfoVM	g_TaskInfoVM[MAX_TASKS];
#endif

/* Interpreter stack for each task
 */
#if ! defined(RTS_CFG_FFO)
  static IEC_DATA	g_pStack[MAX_STACK_SIZE * MAX_TASKS];
#endif

/* String working buffer for each task
 */
#if ! defined(RTS_CFG_FFO)
  static IEC_CHAR	g_pVMBuff[(VMM_MAX_IEC_STRLEN + 1) * MAX_TASKS];
#endif

/* Image copy regions for each task
 */
#if defined(RTS_CFG_FFO)
  static SImageReg	*g_pImageReg = NULL;
  static IEC_UINT	g_uTasks	 = 0;
#else
  static SImageReg	g_pImageReg[MAX_TASKS];
#endif

/* Task handles
 * ----------------------------------------------------------------------------
 */
static pthread_t	g_hVM[MAX_TASKS];

/* Semaphores
 * ----------------------------------------------------------------------------
 */
static pthread_mutex_t	g_pCS[MAX_SEMAPHORE];

extern IEC_BOOL g_bSilentMode;

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */

STaskInfoVMM *get_pVMM(void)
{
    return the_pVMM;
}

/* ---------------------------------------------------------------------------- */
/**
 * osInitialize
 *
 * Do operating system dependent on time initializations here.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osInitialize(void)
{
	IEC_UINT uRes = OK;

	OS_MEMSET(&g_hVM,		0x00, sizeof(g_hVM));

	uRes = osInitializeShared();
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osCreateTaskInfoVMM
 *
 * Creates the VMM instance data object STaskInfoVMM.
 *
 * @return			Pointer to the created object if successful, NULL if failed.
 */
STaskInfoVMM *osCreateTaskInfoVMM(void)
{	
	STaskInfoVMM *pVMM = NULL;

  #if defined(RTS_CFG_FFO)
	pVMM = (STaskInfoVMM *)osMalloc(sizeof(STaskInfoVMM));
  #else
	pVMM = &g_TaskInfoVMM;
  #endif

	return pVMM;
}

/* ---------------------------------------------------------------------------- */
/**
 * osInitializeVMM
 *
 * Do operating system dependent on time initializations here.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osInitializeVMM(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;

    the_pVMM = pVMM;

#if defined(_SOF_4CFC_SRC_)
	pthread_t thr;

	fcSetLed(FC_LED_RUN,   FC_LED_ON_TOGGLE);
	fcSetLed(FC_LED_ERROR, FC_LED_ON_TOGGLE);

#ifndef __XENO__
	int iRes = pthread_create(&thr, NULL, LED_Thread, pVMM);
#else
	int iRes = osPthreadCreate(&thr, NULL, LED_Thread, pVMM, "LED", 0);
#endif
	if (iRes != 0)
	{
		TR_ERR("pthread_create() failed", iRes);
		
		RETURN(ERR_CREATE_TASK);
	}

	osSleep(50);
#endif
#if defined(RTS_CFG_IOCANOPEN)

	if (app_config_load(APP_CONF_CAN0))
	{
		fprintf(stderr, "[%s]: Error Can0 module configuration file is wrong.\n", __func__);
	}

	if (app_config_load(APP_CONF_CAN1))
	{
		fprintf(stderr, "[%s]: Error Can1 module configuration file is wrong.\n", __func__);
	}

	if (can0_cfg.enabled != 0 || can1_cfg.enabled != 0)
	{
	}
  #endif

  #if defined(IP_CFG_NCC)
	typedef void (*LibFct)(void);
	extern LibFct libFcts[];
	pVMM->Shared.pLibFcts = libFcts;
  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osCreateCodeList
 *
 * Creates the global interpreter code list.
 *
 * @return			Pointer to the created object if successful, NULL if failed.
 */
SObject *osCreateCodeList(void)
{
	SObject *pObject = NULL;

  #if defined(RTS_CFG_FFO)
	pObject = (SObject *)osMalloc(sizeof(SObject) * MAX_CODE_OBJ);
  #else
	pObject = g_pCodeObj;
  #endif

	return pObject;
}

/* ---------------------------------------------------------------------------- */
/**
 * osCreateTaskInfoVM
 *
 * Creates the VM instance data for a corresponding IEC/VM task.
 * 
 * @return			Pointer to the created object if successful, NULL if failed.
 */
STaskInfoVM *osCreateTaskInfoVM(IEC_UINT uTask)
{
	STaskInfoVM *pVM = NULL;

  #if defined(RTS_CFG_FFO)
	pVM = (STaskInfoVM *)osMalloc(sizeof(STaskInfoVM));
  #else
	pVM = g_TaskInfoVM + uTask;
  #endif

	return pVM;
}

/* ---------------------------------------------------------------------------- */
/**
 * osFreeTaskInfoVM
 *
 * Free the VM instance data object.
 * 
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osFreeTaskInfoVM(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;

  #if defined(RTS_CFG_FFO)
	uRes = osFree((IEC_DATA **)&pVM);
  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osCreateVMMembers
 *
 * Creates the different memory objects for a given VM instance data object:
 *
 * STaskInfoVM.pStack		MAX_STACK_SIZE
 * STaskInfoVM.pBuffer		VMM_MAX_IEC_STRLEN + 1
 * 
 * @return			OK if successful else error number
 */
IEC_UINT osCreateVMMembers(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;

  #if defined(RTS_CFG_FFO)
	pVM->Local.pStack	= osMalloc(MAX_STACK_SIZE);
	pVM->Local.pBuffer	= (IEC_CHAR *)osMalloc(VMM_MAX_IEC_STRLEN + 1);

	if (pVM->Local.pStack == NULL || pVM->Local.pBuffer == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}
  #else
	pVM->Local.pStack = g_pStack + pVM->usTask * MAX_STACK_SIZE;
	pVM->Local.pBuffer = g_pVMBuff + pVM->usTask * (VMM_MAX_IEC_STRLEN + 1);
  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osFreeVMMembers
 *
 * Frees the different memory objects for a given VM instance data object:
 * 
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osFreeVMMembers(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;

  #if defined(RTS_CFG_FFO)
	uRes |= osFree((IEC_DATA **)&pVM->Local.pStack);
	uRes |= osFree((IEC_DATA **)&pVM->Local.pBuffer);
  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osCreateImageReg
 *
 * Creates memory for IO layer process image information.
 *
 * STaskInfoVM.Task.pIR 	sizeof(SImageReg)
 * 
 * @return			OK if successful else error number
 */
SImageReg *osCreateImageReg(STaskInfoVM *pVM, IEC_UINT uTasks)
{

  #if defined(RTS_CFG_FFO)

	if (g_uTasks != uTasks)
	{
		if (g_pImageReg != NULL)
		{
			IEC_UINT uRes = osFree((IEC_DATA **)&g_pImageReg);
			TR_RET(uRes);
			uRes++;
		}
	}

	g_uTasks = uTasks;

	if (g_pImageReg == NULL)
	{
		g_pImageReg = (SImageReg *)(osMalloc(sizeof(SImageReg) * uTasks));
		if (g_pImageReg == NULL)
		{
			TR_RET(ERR_OUT_OF_MEMORY);

			return NULL;
		}
	}
  #endif

	return g_pImageReg + pVM->usTask;
}

/* ---------------------------------------------------------------------------- */
/**
 * osFreeImageReg
 *
 * Frees the different memory objects for a given VM instance data object:
 * 
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osFreeImageReg(STaskInfoVM *pVM)
{

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * osInitializeVM	
 *
 * Operating system dependent initializations for each Virtual 
 * Machine (VM) task.
 *
 * @return			OK if successful else error number
 */
IEC_UINT osInitializeVM(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;

	/* the downloaded code can be also executable code 
	 */
	void*	 ptr;
	SObject* pCode = pVM->pShared->pCode;
	size_t	 unAlign;

	unAlign = ((int)pCode->pAdr + getpagesize()) % getpagesize();
	ptr = pCode->pAdr - unAlign;

	if(mprotect(ptr, pCode->ulSize+unAlign, PROT_READ|PROT_WRITE|PROT_EXEC) == -1)
	{
		RETURN_e(ERR_ERROR);

	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osCreateVMTask
 *
 * Create a Virtual Machine task here. 
 *
 * vmMain() is the main function for the VM task.
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osCreateVMTask (STaskInfoVM *pVM)
{	
	IEC_UINT uRes = OK;

	pthread_t hThread = 0;
	char name[3+VMM_MAX_IEC_IDENT];
	snprintf(name, sizeof(name), "%02u:%s", pVM->usTask, pVM->Task.szName);

	int iRes = osPthreadCreate(&hThread, NULL, VM_Proc, pVM, name, 0);
	if (iRes != 0)
	{
		RETURN_e(ERR_CREATE_TASK);
	}

	g_hVM[pVM->usTask] = hThread;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osKillVMTask
 *
 * Deletes (destroys) a given VM task. The corresponding VM task or
 * thread already have received a MSG_TERMINATE message. However it
 * is not guaranteed, the the task has received and processed the
 * message (I. e. if the task executes a very large IEC program.)
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osKillVMTask(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;

	int iRes;
	iRes = pthread_cancel(g_hVM[pVM->usTask]);
	iRes = pthread_join(g_hVM[pVM->usTask], NULL);	/* Danger, may wait forever! */

	g_hVM[pVM->usTask] = 0;

	flCleanUp();
#if 0
	cleanup_comm_lib();
	cleanup_emb_comm_lib();
#endif
	
  #if defined(FC_CFG_MODBUS_RTU_LIB)
	cleanup_modbus_lib();
  #endif

  #if defined(FC_CFG_MBUS_LIB)
	cleanup_mbus_lib();
  #endif
	
  #if defined(RTS_CFG_MBUS2_LIB)
	cleanup_mbus2_lib();
  #endif
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osOnVMTerminate
 *
 * This function is called before a VM task terminates itself by 
 * leaving its main routine.
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osOnVMTerminate(STaskInfoVM *pVM)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osCreateListenTask
 *
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_TCP_NATIVE)

IEC_UINT osCreateListenTask(SComTCP *pTCP)
{
	IEC_UINT uRes = OK;

	pthread_t hThread = 0;
	
	if (g_bSilentMode == FALSE)
	{
		int iRes = osPthreadCreate(&hThread, NULL, SOCKET_ListenThread, pTCP, "SOCKET_Listen", 0);
		if (iRes != 0)
		{
			RETURN_e(ERR_CREATE_TASK);
		}
	}

	RETURN(uRes);
}
#endif /* RTS_CFG_TCP_NATIVE */

/* ---------------------------------------------------------------------------- */
/**
 * osCreateCommTask
 *
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_TCP_NATIVE) && defined(RTS_CFG_MULTILINK)

IEC_UINT osCreateCommTask(SComTCP *pTCP)
{
	IEC_UINT uRes = OK;

	pthread_t hThread = 0;
	
	int iRes = osPthreadCreate(&hThread, NULL, SOCKET_CommThread, pTCP, "SOCKET_Comm", 0);
	if (iRes != 0)
	{
		RETURN_e(ERR_CREATE_TASK);
	}
	
	pTCP->hTask = hThread;

	RETURN(uRes);
}
#endif /* RTS_CFG_TCP_NATIVE && RTS_CFG_MULTILINK */

/* ---------------------------------------------------------------------------- */
/**
 * osCreateOnlineChangeTask
 *
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_VM_IPC) && defined(RTS_CFG_ONLINE_CHANGE)

IEC_UINT osCreateOnlineChangeTask(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;

	pthread_t hThread = 0;
	
	int iRes = osPthreadCreate(&hThread, NULL, VMM_OnlineChangeThread, pVMM, "VMM_OnlineChange", 0);
	if (iRes != 0)
	{
		RETURN_e(ERR_CREATE_TASK);
	}
	
	RETURN(uRes);
}
#endif /* RTS_CFG_VM_IPC && RTS_CFG_ONLINE_CHANGE */

/* ---------------------------------------------------------------------------- */
/**
 * osCreateVMTimerTask
 *
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_VM_IPC)

IEC_UINT osCreateVMTimerTask(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;

	pthread_t hThread = 0;
	
	int iRes = osPthreadCreate(&hThread, NULL, VM_TimerThread, pVMM, "VM_Timer", 0);
	if (iRes != 0)
	{
		RETURN_e(ERR_CREATE_TASK);
	}

	RETURN(uRes);
}
#endif /* RTS_CFG_VM_IPC */

/* ---------------------------------------------------------------------------- */
/**
 * osCreateRetainTask
 *
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_EXT_RETAIN)

IEC_UINT osCreateRetainTask(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;

	pthread_t hThread = 0;
	
	int iRes = osPthreadCreate(&hThread, NULL, VMM_RetainThread, pVMM, "VMM_Retain", 0);
	if (iRes != 0)
	{
		RETURN_e(ERR_CREATE_TASK);
	}

	RETURN(uRes);
}
#endif /* RTS_CFG_EXT_RETAIN */

/* ---------------------------------------------------------------------------- */
/**
 * osCreateDeviceTask
 *
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_BACNET)
IEC_UINT osCreateDeviceTask(void)
{
	IEC_UINT uRes = OK;

	pthread_t hThread = 0;
	
#ifndef __XENO__
	int iRes = pthread_create(&hThread, NULL, BAC_DeviceThread, NULL);
#else
	int iRes = osPthreadCreate(&hThread, NULL, BAC_DeviceThread, NULL, "BAC_Device", 0);
#endif
	if (iRes != 0)
	{
		RETURN_e(ERR_CREATE_TASK);
	}

	RETURN(uRes);
}
#endif /* RTS_CFG_BACNET */

/* ---------------------------------------------------------------------------- */
/**
 * osCreateCOVTask
 *
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_BACNET)
IEC_UINT osCreateCOVTask(void)
{
	IEC_UINT uRes = OK;

	pthread_t hThread = 0;
	
#ifndef __XENO__
	int iRes = pthread_create(&hThread, NULL, BAC_COVThread, NULL);
#else
	int iRes = osPthreadCreate(&hThread, NULL, BAC_COVThread, NULL, "BAC_COV", 0);
#endif
	if (iRes != 0)
	{
		RETURN_e(ERR_CREATE_TASK);
	}

	RETURN(uRes);
}
#endif /* RTS_CFG_BACNET */

/* ---------------------------------------------------------------------------- */
/**
 * osCreateScanTask
 *
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_BACNET)
IEC_UINT osCreateScanTask(void)
{
	IEC_UINT uRes = OK;

	pthread_t hThread = 0;
	
#ifndef __XENO__
	int iRes = pthread_create(&hThread, NULL, BAC_ScanThread, NULL);
#else
	int iRes = osPthreadCreate(&hThread, NULL, BAC_ScanThread, NULL, "BAC_Scan", 0);
#endif
	if (iRes != 0)
	{
		RETURN_e(ERR_CREATE_TASK);
	}

	RETURN(uRes);
}
#endif /* RTS_CFG_BACNET */

/* ---------------------------------------------------------------------------- */
/**
 * osCreateFlashTask
 *
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_BACNET)

IEC_UINT osCreateFlashTask(void)
{
	IEC_UINT uRes = OK;

	pthread_t hThread = 0;
	
#ifndef __XENO__
	int iRes = pthread_create(&hThread, NULL, BAC_FlashThread, NULL);
#else
	int iRes = osPthreadCreate(&hThread, NULL, BAC_FlashThread, NULL, "BAC_Flash", 0);
#endif
	if (iRes != 0)
	{
		RETURN_e(ERR_CREATE_TASK);
	}
	RETURN(uRes);
}
#endif /* RTS_CFG_BACNET */

/* ---------------------------------------------------------------------------- */
/**
 * osCreateConfigTask
 *
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_BACNET)

IEC_UINT osCreateConfigTask(SIOLayerIniVal *pIni)
{
	IEC_UINT uRes = OK;

	pthread_t hThread = 0;
	
#ifndef __XENO__
	int iRes = pthread_create(&hThread, NULL, BAC_ConfigThread, pIni);
#else
	int iRes = osPthreadCreate(&hThread, NULL, BAC_ConfigThread, pIni, "BAC_Config", 0);
#endif
	if (iRes != 0)
	{
		RETURN_e(ERR_CREATE_TASK);
	}
	RETURN(uRes);
}
#endif /* RTS_CFG_BACNET */

/* ---------------------------------------------------------------------------- */
/**
 * osCreatePBManagementTask
 *
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_PROFI_DP)

IEC_UINT osCreatePBManagementTask(IEC_UINT uIOLayer)
{
	IEC_UINT uRes = OK;

	IEC_UDINT ulIOLayer = uIOLayer;

	pthread_t hThread = 0;
	
#ifndef __XENO__
	int iRes = pthread_create(&hThread, NULL, PDP_ManagementThread, (IEC_UDINT *)ulIOLayer);
#else
	int iRes = osPthreadCreate(&hThread, NULL, PDP_ManagementThread, (IEC_UDINT *)ulIOLayer, "PDP_Management", 0);
#endif
	if (iRes != 0)
	{
		RETURN_e(ERR_CREATE_TASK);
	}

	RETURN(uRes);
}
#endif /* RTS_CFG_PROFI_DP */

/* ---------------------------------------------------------------------------- */
/**
 * osOnCommTerminate
 *
 * This function is called before a communication task terminates itself by 
 * leaving its main routine.
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_MULTILINK)

IEC_UINT osOnCommTerminate(STaskInfoVMM *pVMM, IEC_UINT uTask)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}
#endif /* RTS_CFG_MULTILINK */

/* ---------------------------------------------------------------------------- */
/**
 * osCreateSemaphore
 *
 * Create a semaphore for the given ID. The maximum count of 
 * semaphores is given by MAX_SEMAPHORES. 
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osCreateSemaphore(IEC_UINT uSemaphore)
{	
	IEC_UINT uRes = OK;

	if (uSemaphore >= MAX_SEMAPHORE)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	pthread_mutex_init(g_pCS + uSemaphore, NULL);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osDeleteSemaphore
 *
 * Deletes the semaphore given by the ID. The maximum count of 
 * semaphores is given by MAX_SEMAPHORES. 
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osDeleteSemaphore(IEC_UINT uSemaphore)
{	
	IEC_UINT uRes = OK;

	if(uSemaphore >= MAX_SEMAPHORE)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	if (pthread_mutex_destroy(g_pCS + uSemaphore) != 0)
	{
		RETURN_e(ERR_ERROR);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osBeginCriticalSection
 *
 * A task tries to enter a critical section defined by the given 
 * semaphore ID. If the resource is looked, the task must be suspended
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osBeginCriticalSection(IEC_UINT uSemaphore)
{
	IEC_UINT uRes = OK;

	if(uSemaphore >= MAX_SEMAPHORE)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	if (pthread_mutex_lock((g_pCS + uSemaphore)) != 0)
	{
		RETURN_e(ERR_ERROR);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osEndCriticalSection
 *
 * A task leaves a critical section defined by the given semaphore ID.
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osEndCriticalSection(IEC_UINT uSemaphore)
{
	IEC_UINT uRes = OK;

	if(uSemaphore >= MAX_SEMAPHORE)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	if (pthread_mutex_unlock((g_pCS + uSemaphore)) != 0)
	{
		RETURN_e(ERR_ERROR);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osCreateVMEvents
 *
 * Allocates memory: sizeof(IEC_UINT) * MAX_TASKS. Called for each event!
 *
 * @return			Pointer to the created object if successful, NULL if failed.
 */
#if ! defined(RTS_CFG_FFO) && defined(RTS_CFG_EVENTS)

IEC_UINT *osCreateVMEvents(IEC_UINT uEvent)
{

	return NULL;
}
#endif	/* ! RTS_CFG_FFO && RTS_CFG_EVENTS */

/* ---------------------------------------------------------------------------- */
/**
 * osFreeVMEvents
 *
 * @return			OK if successful else ERR_ERROR
 */
#if ! defined(RTS_CFG_FFO) && defined(RTS_CFG_EVENTS)

IEC_UINT osFreeVMEvents(IEC_UINT uEvent)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}
#endif	/* ! RTS_CFG_FFO && RTS_CFG_EVENTS */

/* ---------------------------------------------------------------------------- */
/**
 * osConvertVMPriority
 *
 * Converts FarosPLC priorities (0-9) into corresponding operating
 * system priorities.
 *
 * @return			Operating system depending VM priority.
 */
IEC_USINT osConvertVMPriority(IEC_USINT usPriority)
{

	return FC_PRIO_VM_MAX - usPriority;
}

/* ---------------------------------------------------------------------------- */
/**
 * osConvert4CPriority
 *
 * Converts operating system priorities into corresponding 
 * FarosPLC priorities (0-9).
 *
 * @return			Operating system depending VM priority.
 */
IEC_USINT osConvert4CPriority(IEC_USINT usPriority)
{

	return FC_PRIO_VM_MAX - usPriority;
}

/* ---------------------------------------------------------------------------- */
/**
 * osOnClearBegin
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osOnClearBegin(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osOnClearAfterDelVM
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osOnClearAfterDelVM(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osOnClearEnd
 *
 * @return			OK if successful else ERR_ERROR
 */
IEC_UINT osOnClearEnd(STaskInfoVMM *pVMM)
{
	IEC_UINT uRes = OK;


	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetTaskID
 *
 * @return			OK if successful else ERR_ERROR
 */
#if defined(RTS_CFG_SYSLOAD)

IEC_UDINT osGetTaskID(void)
{

	return getpid();
}

#endif	/* RTS_CFG_SYSLOAD */

/* ---------------------------------------------------------------------------- */
