
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
 * Filename: visTest.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"visTest.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#include "visDef.h"
#include "visMain.h"
#include "visShared.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

#define	VL_TEST_BROWSE
#undef	VL_TEST_FW_DL
#undef  VL_TEST_LIC
#undef	VL_TEST_RETAIN

/* ----  Global Variables:	 -------------------------------------------------- */

VIS_UINT g_uDBIVar	= 0;
VIS_DATA *g_pDBIVar = NULL;

/* ----  Local Functions:	--------------------------------------------------- */

#if defined(VL_TEST_BROWSE)
  static void BrowseWholeProject(VIS_UINT uLevel, VIS_CHAR *szParent);
#endif

#if defined(VL_TEST_FW_DL)
  static void TestFWDownload(void);
#endif

#if defined(VL_TEST_LIC)
  static void TestLic(void);
#endif

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * main	
 *
 */
int main (int argc, char *argv[])
{
	VIS_UINT uRes;

	OS_FPRINTF(os_stdout, "\r\nFarosPLC VisuComm Library " RTS_MAIN_VERSION" ("RTS_FULL_VERSION")\r\n", PRODUCT_BUILD);

	/* Initialize the jiffies to get the correct uptime 
	 */
  #if defined(_SOF_4CFC_SRC_)
	uRes = fcInitJiffies();
	if (uRes != OK)
	{
		RETURN(uRes);
	}	
  #endif
	
	if (argc < 3)
	{
//		uRes = visInitialize("172.17.5.215", VIS_PORT_SCAN);
		uRes = visInitialize("192.168.0.221", VIS_PORT_SCAN);
//		uRes = visInitialize("172.17.5.68", VIS_PORT_SCAN);
	}
	else
	{
		uRes = visInitialize(argv[1], (VIS_UINT)strtoul(argv[2], NULL, 10));
	}
	
	/* Login to RTS
	 */
	{
		uRes = visLogin();

		if (uRes == OK)
		{
			OS_FPRINTF(os_stdout, "Log in to control OK.\r\n");
		}
		else
		{
			OS_FPRINTF(os_stdout, "Log in to control failed (%d / %s).\r\n", uRes, visError(uRes));
		}
	}


	/* Verify if project is still the same
	 */
	{
		uRes = visCheckProject();

		if (uRes == OK)
		{
			OS_FPRINTF(os_stdout, "Check project OK.\r\n");
		}
		else
		{
			OS_FPRINTF(os_stdout, "Check project failed (%d / %s).\r\n", uRes, visError(uRes));
		}
	}

	
  #if defined(VL_TEST_BROWSE)
	/* Get resource state
	 */
	{
		VIS_UDINT ulState = 0xfffffffful;

		uRes = visGetResourceState(&ulState);

		if (uRes == OK)
		{
			OS_FPRINTF(os_stdout, "Resource state: 0x%08x.\r\n", ulState);
		}
		else
		{
			OS_FPRINTF(os_stdout, "Get resource state failed (%d / %s).\r\n", uRes, visError(uRes));
		}
	}


	/* Get task state
	 */
	{
		VIS_CHAR *szTask = "task1";
		VIS_UDINT ulState = 0xfffffffful;

		uRes = visGetTaskState(szTask, &ulState);

		if (uRes == OK)
		{
			OS_FPRINTF(os_stdout, "State of task '%s': 0x%08x.\r\n", szTask, ulState);
		}
		else
		{
			OS_FPRINTF(os_stdout, "Get task state of task '%s' failed (%d / %s).\r\n", szTask, uRes, visError(uRes));
		}
	}
  #endif /* VL_TEST_BROWSE */


	/* Do some tests...
	 */
  #if defined(VL_TEST_BROWSE)
	OS_FPRINTF(os_stdout, "\r\nBrowse project recursively:\r\n\r\n");

	visCreateVisuVar(&g_pDBIVar, &g_uDBIVar);

	BrowseWholeProject(0, "root");
	
	OS_FPRINTF(os_stdout, "\r\n\r\n");
  #endif /* VL_TEST_BROWSE */
	

  #if defined(VL_TEST_FW_DL)
	TestFWDownload();
  #endif
	
  #if defined(VL_TEST_LIC)
	TestLic();
  #endif


	/* Test retain functionality
	 */
  #if defined(VL_TEST_RETAIN)
	visRetainSetCycle(10000);

	visRetainWrite();
  #endif

	/* Finalize VisuComm
	 */
	{
		uRes = visFinalize();

		if (uRes == OK)
		{
			OS_FPRINTF(os_stdout, "Finalize OK.\r\n");
		}
		else
		{
			OS_FPRINTF(os_stdout, "Finalize failed (%d / %s).\r\n", uRes, visError(uRes));
		}
	}

	RETURN(uRes);
}


/* ---------------------------------------------------------------------------- */
/**
 * osTraceLevel	
 *
 */
#if defined(VL_TEST_BROWSE)

static void osTraceLevel(VIS_UINT uLevel)
{
	VIS_UINT u;

	for (u = 0; u < uLevel; u++)
	{
		OS_FPRINTF(os_stdout, " ");
	}
}

#endif /* VL_TEST_BROWSE */


/* ---------------------------------------------------------------------------- */
/**
 * BrowseWholeProject	
 *
 */
#if defined(VL_TEST_BROWSE)

static void BrowseWholeProject(VIS_UINT uLevel, VIS_CHAR *szParent)
{
	VIS_UINT uRes;
	VIS_UINT uCount;

	VIS_UINT uDBIVar  = 0;
	VIS_DATA *pDBIVar = NULL;

	VIS_CHAR szType[1000];

	uDBIVar = g_uDBIVar;
	pDBIVar = g_pDBIVar;

	uRes = visGetChildren(&pDBIVar, &uDBIVar, FALSE);
	if (uRes != OK)
	{
		OS_FPRINTF(os_stdout, "Get Children (%s) failed (%d / %s)\r\n", szParent, uRes, visError(uRes));
	}

	osTraceLevel(uLevel);
	OS_FPRINTF(os_stdout, ">>> %s >>>\r\n", szParent);

	uLevel++;
	uLevel++;
	uLevel++;
	uLevel++;

	uCount = 0;

	while (uCount < uDBIVar)
	{
		XVisuVar *pxVisu = (XVisuVar *)(pDBIVar + uCount);

		/* Show name and type
		 * --------------------------------------------------------------------
		 */
		switch(pxVisu->xVar.usType & 0xE0u)
		{
			case DBI_DTM_SIMPLE:
			{
				VIS_DATA  *pTemp = (VIS_DATA *)pxVisu;
				VIS_UINT  uLen	 = (VIS_UINT)(sizeof(XVisuVar) + pxVisu->xVar.usNameSize + 1);

				VIS_USINT usType = 0;
			
				if (uLevel == 4)
				{
					/* Only for global variables (the first level) we can use 
					 * visGetAddrType here, because visGetChildren returns all
					 * member variables of an object without its hierarchical 
					 * information. (visGetAddrType() on the other hand expects 
					 * a fully qualified identifier.)
					 */
					uRes = visGetAddrType(&pTemp, &uLen, &usType, FALSE);
					if (uRes != OK)
					{
						OS_FPRINTF(os_stdout, "ERROR: visGetAddrType returned error: %x / %s\r\n", uRes, visError(uRes));
						
						usType = pxVisu->xVar.usType;
					}
				}
				else
				{
					usType = pxVisu->xVar.usType;
				}

				switch(usType & ~0xE0u)
				{
					case DBI_DT_BOOL	: OS_STRCPY(szType, "bool");		break;
					case DBI_DT_BYTE	: OS_STRCPY(szType, "byte");		break;
					case DBI_DT_WORD	: OS_STRCPY(szType, "word");		break;
					case DBI_DT_DWORD	: OS_STRCPY(szType, "dword");		break;
					case DBI_DT_LWORD	: OS_STRCPY(szType, "lword");		break;
					case DBI_DT_USINT	: OS_STRCPY(szType, "usint");		break;
					case DBI_DT_UINT	: OS_STRCPY(szType, "uint");		break;
					case DBI_DT_UDINT	: OS_STRCPY(szType, "udint");		break;
					case DBI_DT_ULINT	: OS_STRCPY(szType, "ulint");		break;
					case DBI_DT_SINT	: OS_STRCPY(szType, "sint");		break;
					case DBI_DT_INT 	: OS_STRCPY(szType, "int"); 		break;
					case DBI_DT_DINT	: OS_STRCPY(szType, "dint");		break;
					case DBI_DT_LINT	: OS_STRCPY(szType, "lint");		break;
					case DBI_DT_REAL	: OS_STRCPY(szType, "real");		break;
					case DBI_DT_LREAL	: OS_STRCPY(szType, "lreal");		break;
					case DBI_DT_STRING	: 
					{
						OS_SPRINTF(szType, "string(%d)", visSwap16(pxVisu->xVar.uLen) - 2);
						break;
					}
					case DBI_DT_TIME	: OS_STRCPY(szType, "time");			break;
					default 			: OS_STRCPY(szType, "## INVALID ##");	break;
				}

				if (uLevel == 4)
				{
					visFree(&pTemp);
				}
			}
			break;

			case DBI_DTM_OBJECT:
				OS_STRCPY(szType, "OBJECT");
				break;

			case DBI_DTM_ARRAY:
				OS_SPRINTF(szType, "ARRAY [%d]", visSwap16(pxVisu->xVar.uLen)); 			
				break;
			
			default:
				OS_STRCPY(szType, "## INVALID ##");
				break;
		}

		osTraceLevel(uLevel);
		OS_FPRINTF(os_stdout, "%s (%s)", pxVisu->szName, szType);


		/* Get value for simple types
		 * --------------------------------------------------------------------
		 */
		switch(pxVisu->xVar.usType & 0xE0u)
		{
			case DBI_DTM_SIMPLE:
			{
				VIS_DATA *pVal		= NULL;
				VIS_UINT uVal		= 0;

				/* Convert XVisuVar object to XVariableXX object
				 */
				uRes = visConvertDBIVarToVar(pDBIVar + uCount, &pVal, &uVal);

				/* Get the value
				 */
				if (uRes == OK)
				{
					uRes = visGetValue(&pVal, &uVal, TRUE);
				}

				if (uRes == OK)
				{
					/* Show result
					 */
					switch(pxVisu->xVar.usType & ~0xE0u)
					{
						case DBI_DT_BOOL:
							OS_STRCPY(szType, *pVal != 0 ? "true" : "false");
							break;
						case DBI_DT_BYTE:
							OS_SPRINTF(szType, "0x%02x", *(VIS_DATA *)pVal);
							break;
						case DBI_DT_WORD:
							OS_SPRINTF(szType, "0x%04x", *(VIS_UINT *)pVal);
							break;
						case DBI_DT_DWORD:
							OS_SPRINTF(szType, "0x%08x", *(VIS_UDINT *)pVal);
							break;
						case DBI_DT_SINT:
							OS_SPRINTF(szType, "%d", *(VIS_CHAR *)pVal);
							break;
						case DBI_DT_INT:
							OS_SPRINTF(szType, "%d", *(VIS_UINT *)pVal);
							break;
						case DBI_DT_DINT:
							OS_SPRINTF(szType, "%d", *(VIS_UDINT *)pVal);
							break;
						case DBI_DT_REAL:
							OS_SPRINTF(szType, "%f", *(float *)pVal);
							break;
						case DBI_DT_LREAL:
							OS_SPRINTF(szType, "%f", *(double *)pVal);
							break;
						case DBI_DT_STRING:
						{
							char sz[300];
							OS_MEMCPY(sz, pVal + 2, *pVal);
							sz[*pVal] = 0;

							OS_STRCPY(szType, sz);
							break;
						}
						case DBI_DT_TIME:
							OS_SPRINTF(szType, "%u", *(VIS_UDINT *)pVal);
							break;
						default:
							szType[0] = 0;
							break;
					}
				}

				OS_FPRINTF(os_stdout, "  -  %s\r\n", szType);

				/* Just for testing, write the value back to the control
				 */
				{
					VIS_UINT uSet = (IEC_UINT)(HD_VALUE + visSwap16(pxVisu->xVar.uLen));
					XValue *pxSet = (XValue *)visAlloc(uSet);

					pxSet->VAR.uLen 	= pxVisu->xVar.uLen;
					pxSet->VAR.ulOffset = pxVisu->xVar.ulOffset;
					pxSet->VAR.uSegment = pxVisu->xVar.uInst;
					pxSet->VAR.byBit	= pxVisu->xVar.usBit;

					OS_MEMCPY(pxSet->pValue, pVal, visSwap16(pxVisu->xVar.uLen));

					uRes = visSetValue((VIS_DATA **)&pxSet, &uSet, TRUE);
					if (uRes != OK)
					{
						OS_FPRINTF(os_stdout, "ERROR: visSetValue returned error: %x / %s\r\n", uRes, visError(uRes));						
					}
				}

				visFree(&pVal);

			} /* case DBI_DTM_SIMPLE */
			
			break;

			case DBI_DTM_ARRAY:
			case DBI_DTM_OBJECT:
			{
				/* Call GetChildren for objects again
				 */
				visAppendDBIVar(&g_pDBIVar, &g_uDBIVar, pDBIVar + uCount);

				OS_FPRINTF(os_stdout, "\r\n");
				
				/* uLevel++; */

				BrowseWholeProject(uLevel, pxVisu->szName);
				
				/* uLevel--; */

				visTruncDBIVar(&g_pDBIVar, &g_uDBIVar);

			} /* case DBI_DTM_OBJECT */

			break;

		} /* switch(pxVisu->xVar.usType & 0xE0u) */


		uCount = (VIS_UINT)(uCount + sizeof(XVisuVar) + pxVisu->xVar.usNameSize + 1);
	
	} /* while (uCount < uDBIVar) */

	uLevel--;
	uLevel--;
	uLevel--;
	uLevel--;

	osTraceLevel(uLevel);
	OS_FPRINTF(os_stdout, "<<< %s <<<\r\n", szParent);

	visFree(&pDBIVar);
}

#endif  /* VL_TEST_BROWSE */


/* ---------------------------------------------------------------------------- */
/**
 * TestFWDownload	
 *
 */
#if defined(VL_TEST_FW_DL)

static void TestFWDownload(void)
{
	IEC_UINT uRes = OK;

	IEC_UINT uResult = 0;

	IEC_UDINT ulLen;
	IEC_DATA p[100000];
	IEC_DATA x[100100];

	/* 1
	 */
	FILE *f = fopen("D:\\FWDown\\Firmware1.img", "rb");
	if (f == NULL)
	{
		return;
	}

	ulLen = 100000;
	ulLen = fread(p, 1, ulLen, f);
	
	fclose(f);

	if (ulLen == 0)
	{
		return;
	}

	strcpy(x, "Firmware1.img");
	memcpy(x + strlen("Firmware1.img") + 1, p, ulLen);

	uRes = visFWDownload(x, ulLen + strlen("Firmware1.img") + 1);
	if (uRes == OK)
	{
		OS_FPRINTF(os_stdout, "FW Download of '%s'\r\n", "D:\\FWDown\\Firmware1.img");
	}
	else
	{
		OS_FPRINTF(os_stdout, "FW Download of '%s' failed: 0x%04x / %s\r\n", "D:\\FWDown\\Firmware1.img", uRes, visError(uRes));
		return;
	}


	/* 2
	 */
	f = fopen("D:\\FWDown\\Firmware2.img", "rb");
	if (f == NULL)
	{
		return;
	}

	ulLen = 100000;
	ulLen = fread(p, 1, ulLen, f);
	
	fclose(f);

	if (ulLen == 0)
	{
		return;
	}

	strcpy(x, "Firmware2.img");
	memcpy(x + strlen("Firmware2.img") + 1, p, ulLen);

	uRes = visFWDownload(x, ulLen + strlen("Firmware2.img") + 1);
	if (uRes == OK)
	{
		OS_FPRINTF(os_stdout, "FW Download of '%s'\r\n", "D:\\FWDown\\Firmware2.img");
	}
	else
	{
		OS_FPRINTF(os_stdout, "FW Download of '%s' failed: 0x%04x / %s\r\n", "D:\\FWDown\\Firmware2.img", uRes, visError(uRes));
		return;
	}



	/* CFG
	 */
	f = fopen("D:\\FWDown\\Firmware.cfg", "rb");
	if (f == NULL)
	{
		return;
	}

	ulLen = 100000;
	ulLen = fread(p, 1, ulLen, f);
	
	fclose(f);

	if (ulLen == 0)
	{
		return;
	}

	strcpy(x, "Firmware.cfg");
	memcpy(x + strlen("Firmware.cfg") + 1, p, ulLen);

	uRes = visFWDownload(x, ulLen + strlen("Firmware.cfg") + 1);
	if (uRes == OK)
	{
		OS_FPRINTF(os_stdout, "FW Download of '%s'\r\n", "D:\\FWDown\\Firmware.cfg");
	}
	else
	{
		OS_FPRINTF(os_stdout, "FW Download of '%s' failed: 0x%04x / %s\r\n", "D:\\FWDown\\Firmware.cfg", uRes, visError(uRes));
		return;
	}


	/* Execute
	 */

	uRes = visFWExecute(0xfffffffful, "Firmware.cfg");
	if (uRes == OK)
	{
		OS_FPRINTF(os_stdout, "FW Execute of '%s'\r\n", "D:\\FWDown\\Firmware.cfg");
	}
	else
	{
		OS_FPRINTF(os_stdout, "FW Execute of '%s' failed: 0x%04x / %s\r\n", "D:\\FWDown\\Firmware.cfg", uRes, visError(uRes));
		return;
	}

	/* Get Result
	 */
	uResult = 0;

	do
	{
		uRes = visFWGetResult(&uResult);
		if (uRes == OK)
		{
			OS_FPRINTF(os_stdout, "FW GetResult: %d\r\n", uResult);
		}
		else
		{
			OS_FPRINTF(os_stdout, "FW GetResult failed: 0x%04x / %s\r\n", uRes, visError(uRes));
			return;
		}

		osSleep(100);
	}
	while (uResult != 0 && uRes == OK);
}

#endif /* VL_TEST_FW_DL */


/* ---------------------------------------------------------------------------- */
/**
 * TestLic	
 *
 */
#if defined(VL_TEST_LIC)

static void TestLic(void)
{
	IEC_UINT uRes = OK;
	
	IEC_CHAR szLic[100];
	
	IEC_CHAR *szBuffer;
	IEC_UINT uLen;

	IEC_UDINT ulSN;

	IEC_UINT uAvail;
	IEC_UINT uLic;

	IEC_DATA *pKeyEx = NULL;

	uRes = visGetTargetType(&szBuffer, &uLen);
	if (uRes == OK)
	{
		OS_FPRINTF(os_stdout, "visGetTargetType: %s\r\n", szBuffer);
		osFree(&szBuffer);
	}
	else
	{
		OS_FPRINTF(os_stdout, "visGetTargetType failed: 0x%04x / %s\r\n", uRes, visError(uRes));
		return;
	}

	uRes = visGetTargetVersion(&szBuffer, &uLen);
	if (uRes == OK)
	{
		OS_FPRINTF(os_stdout, "visGetTargetVersion: %s\r\n", szBuffer);
		osFree(&szBuffer);
	}
	else
	{
		OS_FPRINTF(os_stdout, "visGetTargetVersion failed: 0x%04x / %s\r\n", uRes, visError(uRes));
		return;
	}

	uRes = visGetInstKey(&szBuffer, &uLen);
	if (uRes == OK)
	{
		OS_FPRINTF(os_stdout, "visGetInstKey: %s\r\n", szBuffer);
		osFree(&szBuffer);
	}
	else
	{
		OS_FPRINTF(os_stdout, "visGetInstKey failed: 0x%04x / %s\r\n", uRes, visError(uRes));
		return;
	}

	uRes = visGetSerialNo(&ulSN);
	if (uRes == OK)
	{
		OS_FPRINTF(os_stdout, "visGetSerialNo: %ld\r\n", ulSN);
		osFree(&szBuffer);
	}
	else
	{
		OS_FPRINTF(os_stdout, "visGetSerialNo failed: 0x%04x / %s\r\n", uRes, visError(uRes));
		return;
	}

	uRes = visGetFeature(&uAvail, &uLic);
	if (uRes == OK)
	{
		OS_FPRINTF(os_stdout, "visGetFeature: Avail:0x%04x Lic:0x%04x\r\n", uAvail, uLic);
		osFree(&szBuffer);
	}
	else
	{
		OS_FPRINTF(os_stdout, "visGetFeature failed: 0x%04x / %s\r\n", uRes, visError(uRes));
		return;
	}

	OS_STRCPY(szLic, "0650-0000-0000-c28d-4f7d");

	uRes = visSetLicKey(szLic, (IEC_UINT)(OS_STRLEN(szLic) + 1));
	if (uRes == OK)
	{
		OS_FPRINTF(os_stdout, "visSetLicKey: OK\r\n");
		osFree(&szBuffer);
	}
	else
	{
		OS_FPRINTF(os_stdout, "visSetLicKey failed: 0x%04x / %s\r\n", uRes, visError(uRes));
	}

	pKeyEx = osMalloc(sizeof(IEC_UINT) + sizeof(IEC_UINT) + 48);

	*(IEC_UINT *)(pKeyEx + 0)				 = LIC_EX_BACNET;
	*(IEC_UINT *)(pKeyEx + sizeof(IEC_UINT)) = 48;
	OS_MEMCPY(pKeyEx + 2 * sizeof(IEC_UINT), "123456789012345678901234567890123456789012345678", 48);

	uRes = visSetLicEx(pKeyEx, sizeof(IEC_UINT) + sizeof(IEC_UINT) + 48);
	if (uRes == OK)
	{
		OS_FPRINTF(os_stdout, "visSetLicEx: OK\r\n");
	}
	else
	{
		OS_FPRINTF(os_stdout, "visSetLicEx failed: 0x%04x / %s\r\n", uRes, visError(uRes));
	}

	osFree(&pKeyEx);

}
#endif /* VL_TEST_LIC */


/* ---------------------------------------------------------------------------- */
