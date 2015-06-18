
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
 * Filename: vmmLoad.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__		"vmmLoad.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_SYSLOAD)

#include "osFile.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

static IEC_UDINT g_pTaskID[TASK_MAX_TASK];

/* ----  Local Functions:	--------------------------------------------------- */

static IEC_UINT ldAddDiff(IEC_UDINT ulID, STaskTime *pTT, STaskTime *pTT_Diff);
static IEC_UINT ldStrDiff(IEC_CHAR *szBuff, IEC_UDINT uLen, IEC_CHAR *szWhat, IEC_UDINT ulOveral, STaskTime *pTT_Diff);
static IEC_UINT ldGetStatOveral(IEC_UDINT *ulpOveral, SProcTime *pPT);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * ldInitTaskInfo
 *
 */
IEC_UINT ldInitTaskInfo(void)
{
	IEC_UINT uRes = OK;

	IEC_BOOL bExist = FALSE;
	IEC_CHAR pszFileName[VMM_MAX_PATH +  1];

	IEC_UDINT hFile;

	IEC_UINT	 i;
	SSysLoadTask LT;

	OS_MEMSET(g_pTaskID, 0x00, sizeof(g_pTaskID));

	uRes = utilExistFile((IEC_CHAR *)pszFileName, VMM_MAX_PATH, osGetLoadDir, VMM_DIR_LOAD, VMM_FILE_LOAD, &bExist);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (bExist == TRUE)
	{
		uRes = utilDeleteFile((IEC_CHAR *)pszFileName, VMM_MAX_PATH, osGetLoadDir, VMM_DIR_LOAD, VMM_FILE_LOAD);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	uRes = utilCreateFile(&hFile, (IEC_CHAR *)pszFileName, VMM_MAX_PATH, osGetLoadDir, VMM_DIR_LOAD, VMM_FILE_LOAD);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	for (i = 0; i < TASK_MAX_TASK; i++)
	{
		OS_MEMSET(&LT, 0x00, sizeof(SSysLoadTask));

		uRes = ldGetTaskName(i, LT.szTask);
		if (uRes != OK)
		{
			xxxClose(hFile);
			RETURN(uRes);
		}

		uRes = xxxWrite(hFile, (IEC_DATA *)&LT, sizeof(SSysLoadTask));
		if (uRes != OK)
		{
			xxxClose(hFile);
			RETURN(uRes);
		}
	}

	xxxClose(hFile);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * ldWriteTaskInfo
 *
 */
IEC_UINT ldWriteTaskInfo(IEC_UINT uTask, IEC_UDINT ulID)
{
	IEC_UINT uRes = OK;

	IEC_CHAR pszFileName[VMM_MAX_PATH +  1];
	IEC_UDINT hFile;

	SSysLoadTask LT;

	if (uTask >= TASK_MAX_TASK)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	g_pTaskID[uTask] = ulID;

	OS_MEMSET(&LT, 0x00, sizeof(SSysLoadTask));

	uRes = ldGetTaskName(uTask, LT.szTask);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	LT.ulID = ulID;

	OS_SPRINTF(LT.szInfo, " *** %s=%d *** ", LT.szTask, LT.ulID);

	uRes = utilOpenFile(&hFile, (IEC_CHAR *)pszFileName, VMM_MAX_PATH, osGetLoadDir, VMM_DIR_LOAD, VMM_FILE_LOAD, FIO_MODE_RDWR);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = xxxSeek(hFile, uTask * sizeof(SSysLoadTask), FSK_SEEK_SET);
	if (uRes != OK)
	{
		xxxClose(hFile);
		RETURN(uRes);
	}

	uRes = xxxWrite(hFile, (IEC_DATA *)&LT, sizeof(SSysLoadTask));
	if (uRes != OK)
	{
		xxxClose(hFile);
		RETURN(uRes);
	}

	uRes = xxxSeek(hFile, 0, FSK_SEEK_END);
	if (uRes != OK)
	{
		xxxClose(hFile);
		RETURN(uRes);
	}

	xxxClose(hFile);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * ldClearTaskInfo
 *
 */
IEC_UINT ldClearTaskInfo(IEC_UINT uTask)
{
	IEC_UINT uRes = OK;

	IEC_CHAR pszFileName[VMM_MAX_PATH +  1];
	IEC_UDINT hFile;

	SSysLoadTask LT;
	
	if (uTask >= TASK_MAX_TASK)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	g_pTaskID[uTask] = 0ul;

	OS_MEMSET(&LT, 0x00, sizeof(SSysLoadTask));

	uRes = ldGetTaskName(uTask, LT.szTask);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = utilOpenFile(&hFile, (IEC_CHAR *)pszFileName, VMM_MAX_PATH, osGetLoadDir, VMM_DIR_LOAD, VMM_FILE_LOAD, FIO_MODE_RDWR);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = xxxSeek(hFile, uTask * sizeof(SSysLoadTask), FSK_SEEK_SET);
	if (uRes != OK)
	{
		xxxClose(hFile);
		RETURN(uRes);
	}

	uRes = xxxWrite(hFile, (IEC_DATA *)&LT, sizeof(SSysLoadTask));
	if (uRes != OK)
	{
		xxxClose(hFile);
		RETURN(uRes);
	}

	uRes = xxxSeek(hFile, 0, FSK_SEEK_END);
	if (uRes != OK)
	{
		xxxClose(hFile);
		RETURN(uRes);
	}

	xxxClose(hFile);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * ldFindTask
 *
 */
IEC_UINT ldFindTask(IEC_UDINT ulID, IEC_UINT *upTask)
{
	IEC_UINT i;

	*upTask = 0xffffu;

	for (i = 0; i < TASK_MAX_TASK; i++)
	{
		if (g_pTaskID[i] == ulID)
		{
			*upTask = i;
			break;
		}
	}

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * ldGetTaskName
 *
 */
IEC_UINT ldGetTaskName(IEC_UINT uTask, IEC_CHAR *szTask)
{
	if (uTask >= TASK_MAX_TASK)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	if (uTask >= TASK_OFFS_COM_WRK && uTask < TASK_OFFS_COM_WRK + MAX_CONNECTIONS)
	{
		OS_SPRINTF(szTask, TASK_NAME_COM_WRK, uTask - TASK_OFFS_COM_WRK);

		RETURN(OK);
	}
	else if (uTask >= TASK_OFFS_IEC_VM	&& uTask < TASK_OFFS_IEC_VM  + MAX_TASKS)
	{
		OS_SPRINTF(szTask, TASK_NAME_IEC_VM, uTask - TASK_OFFS_IEC_VM);

		RETURN(OK);
	}
	else
	{
		IEC_CHAR *szTemp = NULL;

		switch(uTask)
		{
			case TASK_OFFS_SYS_VMM: 	szTemp = TASK_NAME_SYS_VMM; 	break;
			case TASK_OFFS_SYS_TIM: 	szTemp = TASK_NAME_SYS_TIM; 	break;
			case TASK_OFFS_SYS_OCH: 	szTemp = TASK_NAME_SYS_OCH; 	break;
			case TASK_OFFS_SYS_RET: 	szTemp = TASK_NAME_SYS_RET; 	break;
			case TASK_OFFS_SYS_LED: 	szTemp = TASK_NAME_SYS_LED; 	break;

			case TASK_OFFS_IOL_BAC: 	szTemp = TASK_NAME_IOL_BAC; 	break;
			case TASK_OFFS_IOL_TST: 	szTemp = TASK_NAME_IOL_TST; 	break;
			case TASK_OFFS_IOL_CAN: 	szTemp = TASK_NAME_IOL_CAN; 	break;
			case TASK_OFFS_IOL_KPD: 	szTemp = TASK_NAME_IOL_KPD; 	break;
			case TASK_OFFS_IOL_UDP: 	szTemp = TASK_NAME_IOL_UDP; 	break;
			case TASK_OFFS_IOL_DAT: 	szTemp = TASK_NAME_IOL_DAT; 	break;
			case TASK_OFFS_IOL_SYN: 	szTemp = TASK_NAME_IOL_SYN; 	break;
			case TASK_OFFS_IOL_MTS: 	szTemp = TASK_NAME_IOL_MTS; 	break;
			case TASK_OFFS_IOL_MRC: 	szTemp = TASK_NAME_IOL_MRC; 	break;
			case TASK_OFFS_IOL_PDP: 	szTemp = TASK_NAME_IOL_PDP; 	break;

			case TASK_OFFS_BAC_DEV: 	szTemp = TASK_NAME_BAC_DEV; 	break;
			case TASK_OFFS_BAC_SCN: 	szTemp = TASK_NAME_BAC_SCN; 	break;
			case TASK_OFFS_BAC_COV: 	szTemp = TASK_NAME_BAC_COV; 	break;
			case TASK_OFFS_BAC_FLH: 	szTemp = TASK_NAME_BAC_FLH; 	break;
			case TASK_OFFS_BAC_CFG: 	szTemp = TASK_NAME_BAC_CFG; 	break;

			case TASK_OFFS_BAC_MGT: 	szTemp = TASK_NAME_BAC_MGT; 	break;
			case TASK_OFFS_BAC_COM: 	szTemp = TASK_NAME_BAC_COM; 	break;

			case TASK_OFFS_PDP_MGT: 	szTemp = TASK_NAME_PDP_MGT; 	break;
			case TASK_OFFS_PDP_WRK: 	szTemp = TASK_NAME_PDP_WRK; 	break;

			case TASK_OFFS_COM_LIS: 	szTemp = TASK_NAME_COM_LIS; 	break;
			case TASK_OFFS_COM_WRK: 	szTemp = TASK_NAME_COM_WRK; 	break;

			default:
			{
				// remember to add in the case the new TASK implemented
				RETURN(ERR_INVALID_PARAM);
			}
		}

		OS_STRCPY(szTask, szTemp);

		RETURN(OK);
	}
		
	RETURN(ERR_INVALID_PARAM);
}

/* ---------------------------------------------------------------------------- */
/**
 * ldHandleSysLibCall
 *
 */
IEC_UINT ldHandleSysLibCall(IEC_WORD wLoadType, IEC_UINT uTaskID, IEC_STRING *pLoad)
{
	IEC_UINT uRes = OK;

	IEC_CHAR szBuff[512];

	szBuff[0] = 0;
	pLoad->CurLen = 0;

	switch(wLoadType)
	{
		case LOAD_LOAD_AVERAGE:
		{
			uRes = osStrLoadAvg(szBuff, sizeof(szBuff));
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			break;
		}

		case LOAD_MEM_OVERAL:
		{
			IEC_UDINT ulMemTotal = 0;

			uRes = osStrMemInfo(szBuff, sizeof(szBuff), &ulMemTotal);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			break;
		}

		case LOAD_CPU_SYSTEM:
		{
			IEC_UDINT ulOveral	= 0;

			static IEC_BOOL  bFirst = TRUE;
			static SProcTime PT;

			if (bFirst == TRUE)
			{
				OS_MEMSET(&PT, 0x00, sizeof(SProcTime));

				bFirst = FALSE;
			}

			uRes = osStrStatDiff(szBuff, sizeof(szBuff), &ulOveral, &PT);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			break;
		}

		case LOAD_MEM_TASK:
		{
			IEC_UDINT ulMemTotal = 0;

			if (uTaskID >= TASK_MAX_TASK)
			{
				RETURN(ERR_INVALID_PARAM);
			}

			if (g_pTaskID[uTaskID] == 0)
			{
				RETURN(ERR_TASK_NOT_ACTIVE);
			}
			
			uRes = osStrMemInfo(szBuff, sizeof(szBuff), &ulMemTotal);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			uRes = osStrTaskStatM(g_pTaskID[uTaskID], szBuff, sizeof(szBuff), ulMemTotal);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			break;
		}

		case LOAD_CPU_TASK:
		{
			IEC_UDINT ulOveral	= 0;

			static IEC_BOOL  bFirst = TRUE;
			static SProcTime pPT[TASK_MAX_TASK];
			static STaskTime pTT[TASK_MAX_TASK];

			if (uTaskID >= TASK_MAX_TASK)
			{
				RETURN(ERR_INVALID_PARAM);
			}

			if (g_pTaskID[uTaskID] == 0)
			{
				RETURN(ERR_TASK_NOT_ACTIVE);
			}

			if (bFirst == TRUE)
			{
				OS_MEMSET(pPT, 0x00, sizeof(SProcTime) * TASK_MAX_TASK);
				OS_MEMSET(pTT, 0x00, sizeof(STaskTime) * TASK_MAX_TASK);

				bFirst = FALSE;
			}

			uRes = ldGetStatOveral(&ulOveral, pPT + uTaskID);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			uRes = osStrTaskStatDiff(g_pTaskID[uTaskID], szBuff, sizeof(szBuff), ulOveral, pTT + uTaskID);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			break;
		}

		case LOAD_CPU_IEC:
		{
			IEC_UDINT ulOveral	= 0;

			IEC_UINT  i;
			STaskTime TT_Diff;

			static IEC_BOOL  bFirst = TRUE;
			static SProcTime PT;
			static STaskTime pTT[MAX_TASKS];

			if (bFirst == TRUE)
			{
				OS_MEMSET(&PT, 0x00, sizeof(SProcTime));
				OS_MEMSET(pTT, 0x00, sizeof(STaskTime) * MAX_TASKS);

				bFirst = FALSE;
			}

			OS_MEMSET(&TT_Diff, 0x00, sizeof(STaskTime));

			uRes = ldGetStatOveral(&ulOveral, &PT);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			for (i = 0; i < MAX_TASKS; i++)
			{
				uRes = ldAddDiff(g_pTaskID[TASK_OFFS_IEC_VM + i], pTT + i, &TT_Diff);
				if (uRes != OK)
				{
					RETURN(uRes);
				}
			}

			uRes = ldStrDiff(szBuff, sizeof(szBuff), "IEC", ulOveral, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			break;
		}

		case LOAD_CPU_IO_BAC:
		{
			IEC_UDINT ulOveral	= 0;

			STaskTime TT_Diff;

			static IEC_BOOL  bFirst = TRUE;
			static SProcTime PT;
			static STaskTime TT_IOL_BAC;
			static STaskTime TT_BAC_DEV;
			static STaskTime TT_BAC_SCN;
			static STaskTime TT_BAC_COV;
			static STaskTime TT_BAC_FLH;
			static STaskTime TT_BAC_CFG;

			if (bFirst == TRUE)
			{
				OS_MEMSET(&PT, 0x00, sizeof(SProcTime));

				OS_MEMSET(&TT_IOL_BAC, 0x00, sizeof(STaskTime));
				OS_MEMSET(&TT_BAC_DEV, 0x00, sizeof(STaskTime));
				OS_MEMSET(&TT_BAC_SCN, 0x00, sizeof(STaskTime));
				OS_MEMSET(&TT_BAC_COV, 0x00, sizeof(STaskTime));
				OS_MEMSET(&TT_BAC_FLH, 0x00, sizeof(STaskTime));
				OS_MEMSET(&TT_BAC_CFG, 0x00, sizeof(STaskTime));

				bFirst = FALSE;
			}

			OS_MEMSET(&TT_Diff, 0x00, sizeof(STaskTime));

			uRes = ldGetStatOveral(&ulOveral, &PT);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_IOL_BAC], &TT_IOL_BAC, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_BAC_DEV], &TT_BAC_DEV, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_BAC_SCN], &TT_BAC_SCN, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_BAC_COV], &TT_BAC_COV, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_BAC_FLH], &TT_BAC_FLH, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_BAC_CFG], &TT_BAC_CFG, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			uRes = ldStrDiff(szBuff, sizeof(szBuff), "BAC", ulOveral, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			break;
		}

		case LOAD_CPU_IO_PDP:
		{
			IEC_UDINT ulOveral	= 0;

			STaskTime TT_Diff;

			static IEC_BOOL  bFirst = TRUE;
			static SProcTime PT;
			static STaskTime TT_IOL_PDP;
			static STaskTime TT_PDP_WRK;
			static STaskTime TT_PDP_MGT;

			if (bFirst == TRUE)
			{
				OS_MEMSET(&PT, 0x00, sizeof(SProcTime));

				OS_MEMSET(&TT_IOL_PDP, 0x00, sizeof(STaskTime));
				OS_MEMSET(&TT_PDP_WRK, 0x00, sizeof(STaskTime));
				OS_MEMSET(&TT_PDP_MGT, 0x00, sizeof(STaskTime));

				bFirst = FALSE;
			}

			OS_MEMSET(&TT_Diff, 0x00, sizeof(STaskTime));

			uRes = ldGetStatOveral(&ulOveral, &PT);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_IOL_PDP], &TT_IOL_PDP, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_PDP_WRK], &TT_PDP_WRK, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_PDP_MGT], &TT_PDP_MGT, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			uRes = ldStrDiff(szBuff, sizeof(szBuff), "PDP", ulOveral, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			break;
		}

		case LOAD_CPU_SYS:
		{
			IEC_UDINT ulOveral	= 0;

			IEC_UINT  i;
			STaskTime TT_Diff;

			static IEC_BOOL  bFirst = TRUE;
			static SProcTime PT;
			static STaskTime TT_SYS_VMM;
			static STaskTime TT_SYS_TIM;
			static STaskTime TT_SYS_LED;
			static STaskTime TT_COM_LIS;
			static STaskTime TT_SYS_OCH;
			static STaskTime pTT[MAX_CONNECTIONS];

			if (bFirst == TRUE)
			{
				OS_MEMSET(&PT, 0x00, sizeof(SProcTime));

				OS_MEMSET(&TT_SYS_VMM, 0x00, sizeof(STaskTime));
				OS_MEMSET(&TT_SYS_TIM, 0x00, sizeof(STaskTime));
				OS_MEMSET(&TT_SYS_LED, 0x00, sizeof(STaskTime));
				OS_MEMSET(&TT_COM_LIS, 0x00, sizeof(STaskTime));
				OS_MEMSET(&TT_SYS_OCH, 0x00, sizeof(STaskTime));
				
				OS_MEMSET(pTT, 0x00, sizeof(STaskTime) * MAX_CONNECTIONS);

				bFirst = FALSE;
			}

			OS_MEMSET(&TT_Diff, 0x00, sizeof(STaskTime));

			uRes = ldGetStatOveral(&ulOveral, &PT);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_SYS_VMM], &TT_SYS_VMM, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_SYS_TIM], &TT_SYS_TIM, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_SYS_LED], &TT_SYS_LED, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_COM_LIS], &TT_COM_LIS, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_SYS_OCH], &TT_SYS_OCH, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			for (i = 0; i < MAX_CONNECTIONS; i++)
			{
				uRes = ldAddDiff(g_pTaskID[i + TASK_OFFS_COM_WRK], pTT + i, &TT_Diff);
				if (uRes != OK)
				{
					RETURN(uRes);
				}
			}
			
			uRes = ldStrDiff(szBuff, sizeof(szBuff), "SYS", ulOveral, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			break;
		}

		case LOAD_CPU_BAC:
		{
			IEC_UDINT ulOveral	= 0;

			STaskTime TT_Diff;

			static IEC_BOOL  bFirst = TRUE;
			static SProcTime PT;
			static STaskTime TT_BAC_MGT;
			static STaskTime TT_BAC_COM;

			if (bFirst == TRUE)
			{
				OS_MEMSET(&PT, 0x00, sizeof(SProcTime));

				OS_MEMSET(&TT_BAC_MGT, 0x00, sizeof(STaskTime));
				OS_MEMSET(&TT_BAC_COM, 0x00, sizeof(STaskTime));

				bFirst = FALSE;
			}

			OS_MEMSET(&TT_Diff, 0x00, sizeof(STaskTime));

			uRes = ldGetStatOveral(&ulOveral, &PT);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_BAC_MGT], &TT_BAC_MGT, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}
			uRes = ldAddDiff(g_pTaskID[TASK_OFFS_BAC_COM], &TT_BAC_COM, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			uRes = ldStrDiff(szBuff, sizeof(szBuff), "BDV", ulOveral, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			break;
		}
		case LOAD_CPU_RTS:
		{
			IEC_UDINT ulOveral	= 0;

			IEC_UINT  i;
			STaskTime TT_Diff;

			static IEC_BOOL  bFirst = TRUE;
			static SProcTime PT;
			static STaskTime pTT[TASK_MAX_TASK];

			if (bFirst == TRUE)
			{
				OS_MEMSET(&PT, 0x00, sizeof(SProcTime));
				OS_MEMSET(pTT, 0x00, sizeof(STaskTime) * TASK_MAX_TASK);

				bFirst = FALSE;
			}

			OS_MEMSET(&TT_Diff, 0x00, sizeof(STaskTime));

			uRes = ldGetStatOveral(&ulOveral, &PT);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			for (i = 0; i < TASK_MAX_TASK; i++)
			{
				uRes = ldAddDiff(g_pTaskID[i], pTT + i, &TT_Diff);
				if (uRes != OK)
				{
					RETURN(uRes);
				}
			}

			uRes = ldStrDiff(szBuff, sizeof(szBuff), "RTS", ulOveral, &TT_Diff);
			if (uRes != OK)
			{
				RETURN(uRes);
			}

			break;
		}
		
		default:
		{
			RETURN(ERR_INVALID_PARAM);
		}
	}

	uRes = utilAnsiToIec(szBuff, pLoad);
	if (uRes != OK)
	{
		pLoad->CurLen = 0;

		RETURN(uRes);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * ldAddDiff
 *
 */
static IEC_UINT ldAddDiff(IEC_UDINT ulID, STaskTime *pTT, STaskTime *pTT_Diff)
{
	IEC_UINT uRes = OK;

	STaskTime TT_Diff;

	if (ulID == 0)
	{
		RETURN(OK);
	}

	OS_MEMSET(&TT_Diff, 0x00, sizeof(TT_Diff));

	uRes = osGetTaskStatDiff(ulID, pTT, &TT_Diff);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	pTT_Diff->ulSyst += TT_Diff.ulSyst;
	pTT_Diff->ulUser += TT_Diff.ulUser;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * ldStrDiff
 *
 */
static IEC_UINT ldStrDiff(IEC_CHAR *szBuff, IEC_UDINT uLen, IEC_CHAR *szWhat, IEC_UDINT ulOveral, STaskTime *pTT_Diff)
{
	IEC_REAL fOveral = (IEC_REAL)(ulOveral != 0 ? ulOveral : 1);
	IEC_REAL fSum	 = (IEC_REAL)(pTT_Diff->ulUser + pTT_Diff->ulSyst);

	OS_SPRINTF(szBuff, "Load_%-5s: User:% 5.1f%%  Syst:% 5.1f%%  Sum: % 5.1f%%", szWhat,
				(100.0 * (IEC_REAL)pTT_Diff->ulUser) / fOveral, 	  (100.0 * (IEC_REAL)pTT_Diff->ulSyst) / fOveral, 
				(100.0 * (IEC_REAL)fSum) / fOveral);

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * ldGetStatOveral
 *
 */
static IEC_UINT ldGetStatOveral(IEC_UDINT *ulpOveral, SProcTime *pPT)
{
	IEC_UINT uRes = OK;

	SProcTime PT_Diff;

	uRes = osGetStatDiff(pPT, &PT_Diff);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	*ulpOveral	= PT_Diff.ulUser + PT_Diff.ulNice + PT_Diff.ulSyst + PT_Diff.ulIdle;

	RETURN(uRes);
}

#endif /* RTS_CFG_SYSLOAD */

/* ---------------------------------------------------------------------------- */
