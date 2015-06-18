#include "fc_comunication.h"

#define _VIS_QT_DBG_ 0

/* this class comuniate with the 4C IDE via 4C run-time using visLib */
FC_comunication::FC_comunication()
{
	isConnected = false;
	g_uDBIVar = 0;
	g_pDBIVar = NULL;
	/* initialize the variable used by VIS */	
	visCreateVisuVar(&g_pDBIVar, &g_uDBIVar);

}

FC_comunication::~FC_comunication()
{
	disconnect();
}

bool FC_comunication::connect(char * TargetAddress, int TargetPort)
{
	VIS_UINT uRes;

	uRes = visInitialize(TargetAddress, TargetPort);

	if (uRes != OK)
	{
		OS_FPRINTF(os_stdout, "Connection to '%s' at port '%d' failed.\n", TargetAddress, TargetPort);
		return false;
	}	

	OS_FPRINTF(os_stdout, "Connection to '%s' at port '%d' done.\n", TargetAddress, TargetPort);

	/* Login to RTS
	 */
	uRes = visLogin();

	if (uRes != OK)
	{
		OS_FPRINTF(os_stdout, "Log in to control failed (%d / %s).\n", uRes, visError(uRes));
		return false;
	}

	OS_FPRINTF(os_stdout, "Log in to control OK.\n");

	/* Verify if project is still the same
	 */
	uRes = visCheckProject();

	if (uRes != OK)
	{
		OS_FPRINTF(os_stdout, "Check project failed (%d / %s).\n", uRes, visError(uRes));
		return false;
	}

	OS_FPRINTF(os_stdout, "Check project OK.\n");
	isConnected = true;

	return true;
}

/* disconnect from the 4C run-time */
bool FC_comunication::disconnect(void)
{
	VIS_UINT uRes;
	if (isConnected == false)
	{
		OS_FPRINTF(os_stdout, "Not connected yet.\n");
		return true;
	}

	isConnected = false;

	uRes = visFinalize();

	if (uRes == OK)
	{
		OS_FPRINTF(os_stdout, "Finalize OK.\n");
		return true;
	}
	else
	{
		OS_FPRINTF(os_stdout, "Finalize failed (%d / %s).\n", uRes, visError(uRes));
		return false;
	}
}

/*
 * read a 4C IDE variable identifyed by the name
 * WARNING:
 *   cast the value received in the right way in order to obtain the real value
 */
bool FC_comunication::readValue(char * varname, void * value)
{
	VIS_UINT uRes;
	VIS_DATA *pDBIVar;

	if (!isConnected)
	{
		return false;	
	}
#if 0
	if (updateStatus() == false)
	{
		OS_FPRINTF ("Cannot update.\n");
		return false;	
	}
	OS_FPRINTF ("Update done.\n");
#endif

	if (findVariable(varname, &pDBIVar) == false)
	{
		OS_FPRINTF (os_stdout, "Variable '%s' not found.\n", varname);
		return false;	
	}

	VIS_DATA *pVal		= NULL;
	VIS_UINT uVal		= 0;

	/* Convert XVisuVar object to XVariableXX object
	 */
	uRes = visConvertDBIVarToVar(pDBIVar, &pVal, &uVal);

	/* Get the value
	 */
	if (uRes == OK)
	{
		uRes = visGetValue(&pVal, &uVal, TRUE);
		if (uRes == OK)
		{
			VIS_CHAR szType[1000];
			XVisuVar *pxVisu = (XVisuVar *)(pDBIVar);	
#if _VIS_QT_DBG_ > 0
			OS_FPRINTF (os_stdout, "Got the value of variable '%s' ", varname);
#endif
			switch(pxVisu->xVar.usType & ~0xE0u)
			{
				case DBI_DT_BOOL:
					OS_STRCPY(szType, *pVal != 0 ? "true" : "false");
					OS_MEMCPY(value, pVal, sizeof(VIS_DATA));
					break;
				case DBI_DT_BYTE:
					OS_SPRINTF(szType, "0x%02x", *(VIS_DATA *)pVal);
					OS_MEMCPY(value, pVal, sizeof(VIS_DATA));
					break;
				case DBI_DT_WORD:
					OS_SPRINTF(szType, "0x%04x", *(VIS_UINT *)pVal);
					OS_MEMCPY(value, pVal, sizeof(VIS_UINT ));
					break;
				case DBI_DT_DWORD:
					OS_SPRINTF(szType, "0x%08x", *(VIS_UDINT *)pVal);
					OS_MEMCPY(value, pVal, sizeof(VIS_UDINT));
					break;
				case DBI_DT_SINT:
					OS_SPRINTF(szType, "%d", *(VIS_CHAR *)pVal);
					OS_MEMCPY(value, pVal, sizeof(VIS_CHAR));
					break;
				case DBI_DT_INT:
					OS_SPRINTF(szType, "%d", *(VIS_UINT *)pVal);
					OS_MEMCPY(value, pVal, sizeof(VIS_UINT));
					break;
				case DBI_DT_DINT:
					OS_SPRINTF(szType, "%d", *(VIS_UDINT *)pVal);
					OS_MEMCPY(value, pVal, sizeof(VIS_UDINT));
					break;
				case DBI_DT_REAL:
					OS_SPRINTF(szType, "%f", *(float *)pVal);
					OS_MEMCPY(value, pVal, sizeof(float));
					break;
				case DBI_DT_LREAL:
					OS_SPRINTF(szType, "%f", *(double *)pVal);
					OS_MEMCPY(value, pVal, sizeof(double));
					break;
				case DBI_DT_STRING:
					{
						OS_MEMCPY(value, pVal + 2, *pVal);
						((char*)value)[*pVal] = 0;

						OS_STRCPY(szType, (char *)value);
						break;
					}
				case DBI_DT_TIME:
					OS_SPRINTF(szType, "%u", *(VIS_UDINT *)pVal);
					OS_MEMCPY(value, pVal, sizeof(VIS_UDINT));
					break;
				default:
					szType[0] = 0;
					visFree(&pVal);
					return false;
					break;
			}
#if _VIS_QT_DBG_ > 0
			OS_FPRINTF(os_stdout, "  -  %s\n", szType);
#endif
			memcpy(value,szType, strlen(szType));
			((char *)value)[strlen(szType)] = 0;
		}
		else
		{
			OS_FPRINTF (os_stdout, "Cannot get the value of variable '%s'.\n", varname);
		}
	}
	else
	{
		OS_FPRINTF (os_stdout, "Cannot convert the value of variable '%s'.\n", varname);
	}

	visFree(&pVal);

	return true;
}

/*
 * write a 4C IDE variable identifyed by the name
 */
bool FC_comunication::writeValue(char * varname, void * value)
{
	VIS_UINT uRes;
	VIS_DATA *pVal		= NULL;
	VIS_DATA *pDBIVar;

	if (!isConnected)
	{
		return false;	
	}

#if 0
	if (updateStatus() == false)
	{
		return false;	
	}
#endif

	if (findVariable(varname, &pDBIVar) == false)
	{
		OS_FPRINTF (os_stdout, "Variable '%s' not found.\n", varname);
		return false;	
	}

	pVal = (VIS_DATA *)value;

	XVisuVar *pxVisu = (XVisuVar *)(pDBIVar);

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
		OS_FPRINTF(os_stdout, "ERROR: visSetValue returned error: %x / %s\n", uRes, visError(uRes));						
	}
	return true;
}

#if 0
bool FC_comunication::updateStatus()
{
	VIS_UINT uRes;

	/* Get resource state
	 */
	VIS_UDINT ulState = 0xfffffffful;

	uRes = visGetResourceState(&ulState);

	if (uRes != OK)
	{
		OS_FPRINTF(os_stdout, "Get resource state failed (%d / %s).\n", uRes, visError(uRes));
		return false;
	}
	OS_FPRINTF(os_stdout, "Resource state: 0x%08x.\n", ulState);

	/* Get task state
	 */
	VIS_CHAR *szTask = "task1";

	uRes = visGetTaskState(szTask, &ulState);

	if (uRes != OK)
	{
		OS_FPRINTF(os_stdout, "Get task state of task '%s' failed (%d / %s).\n", szTask, uRes, visError(uRes));
		return false;
	}
	OS_FPRINTF(os_stdout, "State of task '%s': 0x%08x.\n", szTask, ulState);
	return true;
}
#endif

/*
 * find a 4C IDE variable in the variable list identifyed by the name
 */
bool FC_comunication::findVariable(char * varname, VIS_DATA **ppDBIVar)
{
	VIS_UINT uRes;
	VIS_UINT uCount;

	VIS_UINT uDBIVar  = 0;
	VIS_DATA *pDBIVar = NULL;

	uDBIVar = g_uDBIVar;
	pDBIVar = g_pDBIVar;

	uRes = visGetChildren(&pDBIVar, &uDBIVar, FALSE);
	if (uRes != OK)
	{
		OS_FPRINTF(os_stdout, "Get Children failed (%d / %s)\n", uRes, visError(uRes));
	}

	uCount = 0;

#if _VIS_QT_DBG_ > 1
	OS_FPRINTF(os_stdout, "looking for %s\n", varname);
#endif

	while (uCount < uDBIVar)
	{
		XVisuVar *pxVisu = (XVisuVar *)(pDBIVar + uCount);

		/* Get value for simple types
		 * --------------------------------------------------------------------
		 */
		switch(pxVisu->xVar.usType & 0xE0u)
		{
			case DBI_DTM_SIMPLE:
				{
#if _VIS_QT_DBG_ > 2
					OS_FPRINTF(os_stdout, "%s vs %s ...", varname, pxVisu->szName);
#endif
					if (strcasecmp (pxVisu->szName, varname) == 0)
					{
						*ppDBIVar = pDBIVar + uCount;
						visFree(&pDBIVar);
#if _VIS_QT_DBG_ > 1
						OS_FPRINTF(os_stdout, "found\n");
#endif
						return true;
					}
#if _VIS_QT_DBG_ > 2
					OS_FPRINTF(os_stdout, "\n" );
#endif

				} /* case DBI_DTM_SIMPLE */

				break;

			case DBI_DTM_ARRAY:
				/* TODO: manage array */
#if _VIS_QT_DBG_ > 2
				OS_FPRINTF(os_stdout, "%s is an ARRAY. skip it\n", pxVisu->szName);
#endif
				break;
			case DBI_DTM_OBJECT:
				{
					/* Call GetChildren for objects again
					 */
					visAppendDBIVar(&g_pDBIVar, &g_uDBIVar, pDBIVar + uCount);

#if _VIS_QT_DBG_ > 2
					OS_FPRINTF(os_stdout, "%s is an OBJECT analyze it\n", pxVisu->szName);
#endif

					if (findVariable(varname, ppDBIVar) == true)
					{
						visTruncDBIVar(&g_pDBIVar, &g_uDBIVar);
						visFree(&pDBIVar);
#if _VIS_QT_DBG_ > 1
						OS_FPRINTF(os_stdout, "'%s' found under object '%s'\n", varname, pxVisu->szName);
#endif
						return true;
					}
#if _VIS_QT_DBG_ > 1
					OS_FPRINTF(os_stdout, "'%s' NOT found under object '%s'\n", varname, pxVisu->szName);
#endif

					visTruncDBIVar(&g_pDBIVar, &g_uDBIVar);

				} /* case DBI_DTM_OBJECT */

				break;

		} /* switch(pxVisu->xVar.usType & 0xE0u) */


		uCount = (VIS_UINT)(uCount + sizeof(XVisuVar) + pxVisu->xVar.usNameSize + 1);

	} /* while (uCount < uDBIVar) */

	visFree(&pDBIVar);
	return false;
}

#if 0

bool FC_comunication::getVariableList(VIS_DATA **ppDBIVar, VIS_UINT  * uDBIVar)
{
	*uDBIVar = g_uDBIVar;
	*ppDBIVar = g_pDBIVar;
	VIS_UINT uRes;
	VariableListuCount = 0;

	/* Get the variables */	
	uRes = visGetChildren(ppDBIVar, uDBIVar, FALSE);
	if (uRes != OK)
	{
		OS_FPRINTF(os_stdout, "Get Children failed (%d / %s)\n", uRes, visError(uRes));
		return false;
	}
	return true;
}

bool FC_comunication::getNextVariable(VIS_DATA **ppDBIVar, VIS_UINT uDBIVar, XVisuVar **ppxVisu)
{
	/* looking for the target variable */	
	for 	(
			*ppxVisu = (XVisuVar *)(*ppDBIVar);
			VariableListuCount < uDBIVar;
			)
	{
		*ppxVisu = (XVisuVar *)(*ppDBIVar + VariableListuCount);		
		VariableListuCount = (VIS_UINT)(VariableListuCount + sizeof(XVisuVar) + (*ppxVisu)->xVar.usNameSize + 1);
		if (((*ppxVisu)->xVar.usType & 0xE0u) == DBI_DTM_SIMPLE)
		{
			return true;
		}
	}
	return false;
}
#endif

