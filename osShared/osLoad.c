
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
 * Filename: osLoad.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__ "osLoad.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_SYSLOAD)

/* ----  Local Defines:   ----------------------------------------------------- */

#if defined(RTS_CFG_LINUX)
  #define  FILE_LOADAVG		"/proc/loadavg"
  #define  FILE_MEMINFO		"/proc/meminfo"
  #define  FILE_STAT		"/proc/stat"
  #define  FILE_PIDSTAT		"/proc/%u/stat"
  #define  FILE_PIDSTATM	"/proc/%u/statm"
#endif

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/**
 * osGetLoadAvg
 *
 */
IEC_UINT osGetLoadAvg(IEC_REAL *fp1, IEC_REAL *fp5, IEC_REAL *fp15, IEC_UDINT *ulpPReady, IEC_UDINT *ulpPSum)
{
	IEC_UINT uRes = OK;

  #if defined(RTS_CFG_LINUX)

	IEC_UDINT hFile = 0;
	IEC_CHAR  szBuff[2048];

	IEC_UDINT ulPLast;

	uRes = fileOpen(&hFile, FILE_LOADAVG, FIO_MODE_READ, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = fileReadLine(hFile, szBuff, sizeof(szBuff));
	if (uRes != OK)
	{
		fileClose(hFile);
		RETURN(uRes);
	}

	uRes = fileClose(hFile);

	/* 1 min. load average | 5 min. load average | 15 min load average
	 * processes ready to run | overall number of processes | last PID
	 */
	
	uRes = (IEC_UINT)OS_SSCANF(szBuff, "%f %f %f %u/%u %u", fp1, fp5, fp15, ulpPReady, ulpPSum, &ulPLast);
	if (uRes != 6)
	{
		RETURN(ERR_INVALID_DATA);
	}

	uRes = OK;

  #else

	*fp1 = *fp5 = *fp15		= 0.0;
	*ulpPReady = *ulpPSum	= 0;

  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osStrLoadAvg
 *
 */
IEC_UINT osStrLoadAvg(IEC_CHAR *szBuff, IEC_UDINT uLen)
{
	IEC_UINT uRes = OK;

	IEC_REAL  f1, f5, f15;
	IEC_UDINT ulPReady;
	IEC_UDINT ulPSum;
	
	uRes = osGetLoadAvg(&f1, &f5, &f15, &ulPReady, &ulPSum);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	OS_SPRINTF(szBuff, "LoadAVG:    1min:% 6.2f  5min:% 6.2f  15mn:% 6.2f  Proc: %2u/%2u", f1, f5, f15, ulPReady, ulPSum);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetMemInfo
 *
 */
IEC_UINT osGetMemInfo(IEC_UDINT *ulpTotal, IEC_UDINT *ulpUsed, IEC_UDINT *ulpFree)
{
	IEC_UINT uRes = OK;

  #if defined(RTS_CFG_LINUX)
	IEC_UDINT hFile = 0;
	IEC_CHAR  szBuff[2048];

	IEC_UDINT s, b, c;

	uRes = fileOpen(&hFile, FILE_MEMINFO, FIO_MODE_READ, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* Skip first line
	 */
	uRes = fileReadLine(hFile, szBuff, sizeof(szBuff));
	if (uRes != OK)
	{
		fileClose(hFile);
		RETURN(uRes);
	}

	uRes = fileReadLine(hFile, szBuff, sizeof(szBuff));
	if (uRes != OK)
	{
		fileClose(hFile);
		RETURN(uRes);
	}

	fileClose(hFile);

	/* total available | used | free | shared | buffers (kernel) | cached
	 * (in bytes)
	 */
	
	uRes = (IEC_UINT)OS_SSCANF(szBuff, "Mem:  %u %u %u %u %u %u", ulpTotal, ulpUsed, ulpFree, &s, &b, &c);
	if (uRes != 6)
	{
		RETURN(ERR_INVALID_DATA);
	}

	uRes = OK;

  #else

	*ulpTotal = *ulpUsed = *ulpFree = 0;

  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osStrMemInfo
 *
 */
IEC_UINT osStrMemInfo(IEC_CHAR *szBuff, IEC_UDINT uLen, IEC_UDINT *ulpMemTotal)
{
	IEC_UINT uRes = OK;

	IEC_UDINT ulUsed  = 0;
	IEC_UDINT ulFree  = 0;

	uRes = osGetMemInfo(ulpMemTotal, &ulUsed, &ulFree);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	OS_SPRINTF(szBuff, "Memory:     Totl:%6u  Used:%6u  Free:%6u", *ulpMemTotal / 1024u, ulUsed / 1024u, ulFree / 1024u);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetStat
 *
 */
IEC_UINT osGetStat(SProcTime *pPT)
{
	IEC_UINT uRes = OK;

  #if defined(RTS_CFG_LINUX)
	IEC_UDINT hFile = 0;
	IEC_CHAR  szBuff[2048];

	uRes = fileOpen(&hFile, FILE_STAT, FIO_MODE_READ, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = fileReadLine(hFile, szBuff, sizeof(szBuff));
	if (uRes != OK)
	{
		fileClose(hFile);
		RETURN(uRes);
	}

	fileClose(hFile);
	
	/* User | Nice | System | Idle
	 * (in jiffies)
	 */

	uRes = (IEC_UINT)OS_SSCANF(szBuff, "cpu  %u %u %u %u", &pPT->ulUser, &pPT->ulNice, &pPT->ulSyst, &pPT->ulIdle);
	if (uRes != 4)
	{
		RETURN(ERR_INVALID_DATA);
	}

	uRes = OK;

  #else

	OS_MEMSET(pPT, 0x00, sizeof(SProcTime));

  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osStrStat
 *
 */
IEC_UINT osStrStat(IEC_CHAR *szBuff, IEC_UDINT uLen)
{
	IEC_UINT uRes = OK;

	SProcTime PT;

	uRes = osGetStat(&PT);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	OS_SPRINTF(szBuff, "Time:       User:%6u  Syst:%6u  Idle:%6u", PT.ulUser * 10u, PT.ulSyst * 10u, PT.ulIdle * 10u);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetStatDiff
 *
 */
IEC_UINT osGetStatDiff(SProcTime *pPT, SProcTime *pPT_Diff)
{
	IEC_UINT uRes = OK;

	SProcTime PT;

	uRes = osGetStat(&PT);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	pPT_Diff->ulUser = PT.ulUser - pPT->ulUser;
	pPT_Diff->ulNice = PT.ulNice - pPT->ulNice;
	pPT_Diff->ulSyst = PT.ulSyst - pPT->ulSyst;
	pPT_Diff->ulIdle = PT.ulIdle - pPT->ulIdle;

	pPT->ulUser = PT.ulUser;
	pPT->ulNice = PT.ulNice;
	pPT->ulSyst = PT.ulSyst;
	pPT->ulIdle = PT.ulIdle;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * osStrStatDiff
 *
 */
IEC_UINT osStrStatDiff(IEC_CHAR *szBuff, IEC_UDINT uLen, IEC_UDINT *ulpOveral, SProcTime *pPT)
{
	IEC_UINT uRes = OK;

	SProcTime PT_Diff;

	IEC_REAL  fSum;

	uRes = osGetStatDiff(pPT, &PT_Diff);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	*ulpOveral	= PT_Diff.ulUser + PT_Diff.ulNice + PT_Diff.ulSyst + PT_Diff.ulIdle;
	fSum		= (IEC_REAL)(*ulpOveral != 0 ? *ulpOveral : 1);

	OS_SPRINTF(szBuff, "Load:       User:% 5.1f%%  Syst:% 5.1f%%  Sum: % 5.1f%%  Idle:% 5.1f%%", 
				(100.0 * (IEC_REAL)PT_Diff.ulUser) / fSum, (100.0 * (IEC_REAL)PT_Diff.ulSyst) / fSum, 
				(100.0 * (IEC_REAL)(PT_Diff.ulUser + PT_Diff.ulSyst)) / fSum, (100.0 * (IEC_REAL)PT_Diff.ulIdle) / fSum);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetTaskStat
 *
 */
IEC_UINT osGetTaskStat(IEC_UDINT ulID, STaskTime *pTT)
{
	IEC_UINT uRes = OK;

  #if defined(RTS_CFG_LINUX)

	IEC_UDINT hFile = 0;

	IEC_CHAR  szBuff[2048];
	IEC_CHAR  szDummy[2048];

	IEC_UDINT i;
	IEC_UDINT j;
	
	int				d1, d2, d3, d4, d5;
	unsigned long	u1, u2, u3, u4, u5;

	OS_SPRINTF(szBuff, FILE_PIDSTAT, ulID);

	uRes = fileOpen(&hFile, szBuff, FIO_MODE_READ, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = fileReadLine(hFile, szBuff, sizeof(szBuff));
	if (uRes != OK)
	{
		fileClose(hFile);
		RETURN(uRes);
	}

	uRes = fileClose(hFile);

	for (i = 0; szBuff[i] && szBuff[i] != ')'; i++)
	{
		;
	}
	for (j = i; szBuff[j] != '0' && j < i + 3; j++)
	{
		;
	}

	/* See /usr/src/linuxXXX/fs/proc/array.c
	 */

	uRes = (IEC_UINT)OS_SSCANF(szBuff + j,"%d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %s",
				&d1, &d2, &d3, &d4, &d5, &u1, &u2, &u3, &u4, &u5,
				(unsigned long *)&pTT->ulUser, (unsigned long *)&pTT->ulSyst, szDummy);

	if (uRes < 13)
	{
		RETURN(ERR_INVALID_DATA);
	}

	uRes = OK;

  #else

	OS_MEMSET(pTT, 0x00, sizeof(STaskTime));

  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetTaskStatDiff
 *
 */
IEC_UINT osGetTaskStatDiff(IEC_UDINT ulID, STaskTime *pTT, STaskTime *pTT_Diff)
{
	IEC_UINT uRes = OK;

	STaskTime TT;

	uRes = osGetTaskStat(ulID, &TT);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	pTT_Diff->ulUser = TT.ulUser - pTT->ulUser;
	pTT_Diff->ulSyst = TT.ulSyst - pTT->ulSyst;

	pTT->ulUser = TT.ulUser;
	pTT->ulSyst = TT.ulSyst;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osStrTaskStatDiff
 *
 */
IEC_UINT osStrTaskStatDiff(IEC_UDINT ulID, IEC_CHAR *szBuff, IEC_UDINT uLen, IEC_UDINT ulOveral, STaskTime *pTT)
{
	IEC_UINT uRes = OK;

	STaskTime TT_Diff;

	IEC_REAL fSum;
	IEC_REAL fOveral = (IEC_REAL)(ulOveral != 0 ? ulOveral : 1);

	uRes = osGetTaskStatDiff(ulID, pTT, &TT_Diff);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	fSum = (IEC_REAL)(TT_Diff.ulUser + TT_Diff.ulSyst);

	OS_SPRINTF(szBuff, "Load_%-5u: User:% 5.1f%%  Syst:% 5.1f%%  Sum: % 5.1f%%", ulID,
				(100.0 * (IEC_REAL)TT_Diff.ulUser) / fOveral,       (100.0 * (IEC_REAL)TT_Diff.ulSyst) / fOveral, 
				(100.0 * (IEC_REAL)fSum) / fOveral);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetTaskStatM
 *
 */
IEC_UINT osGetTaskStatM(IEC_UDINT ulID, IEC_UDINT *ulpSize, IEC_UDINT *ulpRes)
{
	IEC_UINT uRes = OK;

  #if defined(RTS_CFG_LINUX)
	IEC_UDINT hFile = 0;

	IEC_CHAR  szBuff[2048];
	
	IEC_UDINT ulShared, ulText, ulLib, ulData, ulDirty;

	OS_SPRINTF(szBuff, FILE_PIDSTATM, ulID);

	uRes = fileOpen(&hFile, szBuff, FIO_MODE_READ, TRUE);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = fileReadLine(hFile, szBuff, sizeof(szBuff));
	if (uRes != OK)
	{
		fileClose(hFile);
		RETURN(uRes);
	}

	uRes = fileClose(hFile);

	/* Overall Size | Resident | Shared | Text (TRS) | Library (LRS)
	 * Data (DRS) | Dirty
	 * (in pages à 4096kb)
	 */

	uRes = (IEC_UINT)OS_SSCANF(szBuff, "%u %u %u %u %u %u %u",
				ulpSize, ulpRes, &ulShared, &ulText, &ulLib, &ulData, &ulDirty);

	if (uRes < 7)
	{
		RETURN(ERR_INVALID_DATA);
	}

	uRes = OK;

	*ulpSize *= 4096;
	*ulpRes  *= 4096;

  #else

	*ulpSize = *ulpRes = 0;

  #endif

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osStrTaskStatM
 *
 */
IEC_UINT osStrTaskStatM(IEC_UDINT ulID, IEC_CHAR *szBuff, IEC_UDINT uLen, IEC_UDINT ulOveral)
{
	IEC_UINT uRes = OK;

	IEC_UDINT ulSize;
	IEC_UDINT ulRes;

	IEC_UDINT ulLocOveral = ulOveral != 0 ? ulOveral : 1;

	uRes = osGetTaskStatM(ulID, &ulSize, &ulRes);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	
	OS_SPRINTF(szBuff, "Memy_%-5u: Totl:%6u  Size:% 5.1f%%  Phys:% 5.1f%%", ulID, ulSize / 1024,
				(100.0 * (IEC_REAL)ulSize) / (IEC_REAL)ulLocOveral, (100.0 * (IEC_REAL)ulRes) / (IEC_REAL)ulLocOveral);

	RETURN(uRes);
}

#endif /* RTS_CFG_SYSLOAD */

/* ---------------------------------------------------------------------------- */
