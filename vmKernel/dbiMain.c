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
 * Filename: dbiMain.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"dbiMain.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_DEBUG_INFO)

#include "osFile.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

static IEC_UINT dbiLoadChildren(STaskInfoVMM *pVMM, SDBIInstance *pInst);
static IEC_UINT dbiCreateIntance(SDBIInstance **ppInst);
static IEC_UINT dbiLoadInstance(STaskInfoVMM *pVMM, SDBIInstance **ppInst, IEC_UDINT ulFOAbsolut);
static IEC_UINT dbiFindInstance(SDBIInstance *pInst, IEC_UINT uLen, IEC_CHAR *szInst, IEC_UINT *upIndex);
static IEC_UINT dbiDeleteInstance(SDBIInstance *pInst);

static IEC_UINT dbiCollectMember(STaskInfoVMM *pVMM, SDBIInstance *pInst, XBlock *pBlock, IEC_UINT *upProc);
static IEC_UINT dbiCollectAddress(STaskInfoVMM *pVMM, SDBIInstance *pInst, IEC_DATA *pParent, XBlock *pBlock);

static IEC_UINT dbiLoadTypes(STaskInfoVMM *pVMM);
static IEC_UINT dbiDeleteTypes(IEC_UINT uTypes, SDBIType *pTypes);
static IEC_UINT dbiDeleteType(SDBIType *pType);

static IEC_UINT dbiLoadMember(STaskInfoVMM *pVMM, SDBIType *pType);
static IEC_UINT dbiFindMember(SDBIType *pType, IEC_UINT uLen, IEC_CHAR *szMember, IEC_UINT *upIndex);
static IEC_UINT dbiCopyMemberXVar(SDBIInstance *pInst, SDBIType *pType, IEC_UINT uIndex, XBlock *pBlock);
static IEC_UINT dbiCopyMemberXDBI(SDBIInstance *pInst, SDBIType *pType, IEC_UINT uIndex, XBlock *pBlock);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * dbiInitialize
 *
 */
IEC_UINT dbiInitialize(STaskInfoVMM *pVMM)
{
	SDBIInfo *pDBI = &pVMM->DBI;
	IEC_UINT uRes  = OK;

	if (pDBI->bInitialized == TRUE)
	{
		RETURN(OK);
	}

	pDBI->hInst = (IEC_UDINT)VMF_INVALID_HANDLE;
	pDBI->hVar	= (IEC_UDINT)VMF_INVALID_HANDLE;

	pDBI->pInstRoot = NULL;
	pDBI->pTypes	= NULL;
	
	uRes = dbiLoadInstance(pVMM, &pDBI->pInstRoot, 0ul);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	pDBI->bInitialized = TRUE;
	
	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiFinalize
 *
 */
IEC_UINT dbiFinalize(STaskInfoVMM *pVMM)
{
	SDBIInfo *pDBI = &pVMM->DBI;
	IEC_UINT uRes  = OK;

	if (pDBI->bInitialized == FALSE)
	{
		RETURN(OK);
	}

	/* Free instances
	 */
	uRes = dbiDeleteInstance(pDBI->pInstRoot);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = osFree((IEC_DATA OS_LPTR **)&pDBI->pInstRoot);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* Free types
	 */
	uRes = dbiDeleteTypes(pDBI->uTypes, pDBI->pTypes);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uRes = osFree((IEC_DATA OS_LPTR **)&pDBI->pTypes);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if ((VMF_FILE)pDBI->hInst != VMF_INVALID_HANDLE)
	{
		xxxClose(pDBI->hInst);
		pDBI->hInst = (IEC_UDINT)VMF_INVALID_HANDLE;
	}

	if ((VMF_FILE)pDBI->hVar != VMF_INVALID_HANDLE)
	{
		xxxClose(pDBI->hVar);
		pDBI->hVar = (IEC_UDINT)VMF_INVALID_HANDLE;
	}

	pDBI->bInitialized = FALSE;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiGetChildren
 *
 */
IEC_UINT dbiGetChildren(STaskInfoVMM *pVMM, SDBIInstance *pInst, IEC_DATA *pParent, IEC_UINT uParent, XBlock *pBlock, IEC_UINT *upProc)
{
	SDBIInfo	*pDBI	= &pVMM->DBI;
	XVisuVar	*xpVisu = (XVisuVar *)pParent;
	XDBIVar 	*xpVar	= &xpVisu->xVar;

	IEC_UINT	uRes	= OK;
	IEC_UINT	uIndex;

	if (pInst == NULL)
	{
		pInst = pDBI->pInstRoot;
	}

	if (pDBI->bInitialized == FALSE || pInst == NULL)
	{
		RETURN(ERR_DBI_INIT);
	}

	if (uParent == 0)
	{
		/* Instance found, collect children
		 */
		uRes = dbiCollectMember(pVMM, pInst, pBlock, upProc);
		RETURN(uRes);
	}

	if (uParent < sizeof(XDBIVar) + 1 || uParent < sizeof(XDBIVar) + xpVar->usNameSize + 1)
	{
		/* Emergency exit
		 */
		RETURN(ERR_INVALID_DATA_SIZE);
	}


	if (uParent == sizeof(XDBIVar) + xpVar->usNameSize + 1)
	{
		/* Handle root element ("")
		 */
		if (xpVar->usNameSize == 0)
		{
			uRes = dbiCollectMember(pVMM, pInst, pBlock, upProc);
			RETURN(uRes);
		}
	}

	if (pInst->uChildren == 0)
	{
		RETURN(ERR_DBI_OBJ_NOT_FOUND);
	}

	/* Create / Load children
	 */
	if (pInst->ppChildren == NULL)
	{
		uRes = dbiLoadChildren(pVMM, pInst);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	uIndex = NO_INDEX;

	if (xpVar->uInst != NO_INDEX && xpVar->uInst < pInst->uChildren)
	{
		/* Children already looked up. Verify if index is valid.
		 */
		SDBIInstance *pChild = pInst->ppChildren[xpVar->uInst];

		if (0 == OS_STRCMP(xpVisu->szName, pChild->szName))
		{
			uIndex = xpVar->uInst;
		}
	}

	if (uIndex == NO_INDEX)
	{
		/* Find child
		 */
		uRes = dbiFindInstance(pInst, xpVar->usNameSize, xpVisu->szName, &uIndex);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	/* Patch incoming parent string in order to optimize access for
	 * any possible next block.
	 */
	xpVar->uInst = uIndex;

	pParent =			 pParent + sizeof(XDBIVar) + xpVar->usNameSize + 1;
	uParent = (IEC_UINT)(uParent - sizeof(XDBIVar) - xpVar->usNameSize - 1);

	uRes = dbiGetChildren(pVMM, pInst->ppChildren[uIndex], pParent, uParent, pBlock, upProc);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiGetAddress
 *
 */
IEC_UINT dbiGetAddress(STaskInfoVMM *pVMM, SDBIInstance *pInst, IEC_DATA *pParent, IEC_UINT uParent, XBlock *pBlock)
{
	SDBIInfo	*pDBI	= &pVMM->DBI;
	XVisuVar	*xpVisu = (XVisuVar *)pParent;
	XDBIVar 	*xpVar	= &xpVisu->xVar;

	IEC_UINT	uRes	= OK;
	IEC_UINT	uIndex;

	if (pInst == NULL)
	{
		pInst = pDBI->pInstRoot;
	}

	if (pDBI->bInitialized == FALSE || pInst == NULL)
	{
		RETURN(ERR_DBI_INIT);
	}

	if (uParent < sizeof(XDBIVar) + 1 || uParent < sizeof(XDBIVar) + xpVar->usNameSize + 1)
	{
		/* Emergency exit
		 */
		RETURN(ERR_INVALID_DATA_SIZE);
	}

	if (uParent == sizeof(XDBIVar) + xpVar->usNameSize + 1)
	{
		if (xpVar->usNameSize == 0)
		{
			/* Root element (".") not allowed here
			 */
			RETURN(ERR_INVALID_PARAM);
		}

		uRes = dbiCollectAddress(pVMM, pInst, pParent, pBlock);

		RETURN(uRes);
	}

	if (pInst->uChildren == 0)
	{
		RETURN(ERR_DBI_OBJ_NOT_FOUND);
	}

	/* Create / Load children
	 */
	if (pInst->ppChildren == NULL)
	{
		uRes = dbiLoadChildren(pVMM, pInst);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	uIndex = NO_INDEX;

	if (xpVar->uInst != NO_INDEX && xpVar->uInst < pInst->uChildren)
	{
		/* Children already looked up. Verify if index is valid.
		 */
		SDBIInstance *pChild = pInst->ppChildren[xpVar->uInst];

		if (0 == OS_STRCMP(xpVisu->szName, pChild->szName))
		{
			uIndex = xpVar->uInst;
		}
	}

	if (uIndex == NO_INDEX)
	{
		/* Find child
		 */
		uRes = dbiFindInstance(pInst, xpVar->usNameSize, xpVisu->szName, &uIndex);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	/* Patch incoming parent string in order to optimize access for
	 * any possible next block.
	 */
	xpVar->uInst = uIndex;

	pParent =			 pParent + sizeof(XDBIVar) + xpVar->usNameSize + 1;
	uParent = (IEC_UINT)(uParent - sizeof(XDBIVar) - xpVar->usNameSize - 1);

	uRes = dbiGetAddress(pVMM, pInst->ppChildren[uIndex], pParent, uParent, pBlock);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiGetTaskNr
 *
 */
IEC_UINT dbiGetTaskNr(STaskInfoVMM *pVMM, IEC_DATA *pTask, XBlock *pBlock)
{
	IEC_UINT i;

	XVisuVar *xpVisu = (XVisuVar *)pTask;

	for (i = 0; i < pVMM->Project.uTasks; i++)
	{
		if (OS_STRICMP(xpVisu->szName, pVMM->ppVM[i]->Task.szName) == 0)
		{
			pBlock->uLen	= sizeof(IEC_UINT);
			pBlock->byLast	= TRUE;

			RETURN(OK);
		}
	}

	pBlock->uLen	= 0;
	pBlock->byLast	= TRUE;

	RETURN(ERR_DBI_OBJ_NOT_FOUND);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiLoadChildren
 *
 */
static IEC_UINT dbiLoadChildren(STaskInfoVMM *pVMM, SDBIInstance *pInst)
{
	IEC_UINT  uRes = OK;
	IEC_UINT  i;
	IEC_UDINT ulFOAbsolut = pInst->ulFOAbsolut + sizeof(XDBIInstance) + pInst->usNameSize;

	/* Allocate children list
	 */
	pInst->ppChildren = (SDBIInstance **)osMalloc(pInst->uChildren * sizeof(SDBIInstance *));

	if (pInst->ppChildren == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	/* Create / Load children
	 */
	for (i = 0; i < pInst->uChildren; i++)
	{
		uRes = dbiLoadInstance(pVMM, &pInst->ppChildren[i], ulFOAbsolut);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		ulFOAbsolut += pInst->ppChildren[i]->ulFOBrother;
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiCreateIntance
 *
 */
static IEC_UINT dbiCreateIntance(SDBIInstance **ppInst)
{
	*ppInst = (SDBIInstance *)osMalloc(sizeof(SDBIInstance));
	if (*ppInst == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	OS_MEMSET(*ppInst, 0x00, sizeof(SDBIInstance));

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiLoadInstance
 *
 */
static IEC_UINT dbiLoadInstance(STaskInfoVMM *pVMM, SDBIInstance **ppInst, IEC_UDINT ulFOAbsolut)
{
	IEC_UINT uRes	= OK;
	SDBIInfo *pDBI	= &pVMM->DBI;

	XDBIInstance xInst;
	IEC_UINT	 uLen;

	uRes = dbiCreateIntance(ppInst);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if ((VMF_FILE)pDBI->hInst == VMF_INVALID_HANDLE)
	{
		uRes =	utilOpenFile(&pDBI->hInst, (IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), osGetDBIDir,
								VMM_DIR_DBI, DBI_FILE_INSTANCE, FIO_MODE_READ);

		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	uRes = xxxSeek(pDBI->hInst, ulFOAbsolut, FSK_SEEK_SET);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	uLen = sizeof(XDBIInstance);
	
	uRes = xxxRead(pDBI->hInst, (IEC_DATA *)&xInst, &uLen);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (uLen != sizeof(XDBIInstance))
	{
		RETURN(ERR_FILE_READ);
	}

	(*ppInst)->usType		= xInst.usType;
	(*ppInst)->usNameSize	= xInst.usNameSize;
	(*ppInst)->uInst		= xInst.uInst;
	(*ppInst)->uIndex		= xInst.uIndex;
	(*ppInst)->uChildren	= xInst.uChildren;	
	(*ppInst)->ulFOBrother	= xInst.ulFOBrother;
	(*ppInst)->ulFOAbsolut	= xInst.ulFOAbsolut;

	if (xInst.ulFOAbsolut != ulFOAbsolut)
	{
		/* Absolute file offset from OPC server must match with
		 * local computation.
		 */
		RETURN(ERR_ERROR);
	}

	(*ppInst)->szName = (IEC_CHAR *)osMalloc((*ppInst)->usNameSize + 1);
	if ((*ppInst)->szName == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	uLen = (*ppInst)->usNameSize;

	uRes = xxxRead(pDBI->hInst, (IEC_DATA *)(*ppInst)->szName, &uLen);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (uLen != (*ppInst)->usNameSize)
	{
		RETURN(ERR_FILE_READ);
	}

	(*ppInst)->szName[(*ppInst)->usNameSize] = 0;

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiFindInstance
 *
 */
static IEC_UINT dbiFindInstance(SDBIInstance *pInst, IEC_UINT uLen, IEC_CHAR *szInst, IEC_UINT *upIndex)
{
	IEC_UINT uFrom	 = 0u;
	IEC_UINT uTo	 = (IEC_UINT)(pInst->uChildren - 1);
	IEC_UINT uMiddle = 0u;

	IEC_DINT  iDiff;

	SDBIInstance *pChild;

	*upIndex = NO_INDEX;

	if (pInst->uChildren == 0)
	{
		RETURN(ERR_DBI_OBJ_NOT_FOUND);
	}

	for(;;)
	{
		uMiddle = (IEC_UINT)(uFrom + (uTo - uFrom) / 2);

		pChild = pInst->ppChildren[uMiddle];

		iDiff = OS_STRCMP(szInst, pChild->szName);
		
		if (iDiff == 0)
		{
			*upIndex = uMiddle;
			RETURN(OK);
		}

		if (uTo == uFrom)
		{
			RETURN(ERR_DBI_OBJ_NOT_FOUND);
		}

		if (iDiff <= 0)
		{
			uTo   = (IEC_UINT)(uMiddle - (uMiddle > uFrom ? 1 : 0));
		}
		else
		{
			uFrom = (IEC_UINT)(uMiddle + (uMiddle < uTo   ? 1 : 0));
		}
	}

	RETURN(ERR_ERROR);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiDeleteInstance
 *
 */
static IEC_UINT dbiDeleteInstance(SDBIInstance *pInst)
{
	IEC_UINT uRes = OK;
	IEC_UINT i;

	for (i = 0; i < pInst->uChildren; i++)
	{
		if (pInst->ppChildren == NULL || pInst->ppChildren[i] == NULL)
		{
			/* Child not loaded
			 */
			continue;
		}

		uRes = dbiDeleteInstance(pInst->ppChildren[i]);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		uRes = osFree((IEC_DATA OS_LPTR **)&pInst->ppChildren[i]);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	uRes = osFree((IEC_DATA OS_LPTR **)&pInst->szName);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	if (pInst->ppChildren != NULL)
	{
		uRes = osFree((IEC_DATA OS_LPTR **)&pInst->ppChildren);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiCollectMember
 *
 */
static IEC_UINT dbiCollectMember(STaskInfoVMM *pVMM, SDBIInstance *pInst, XBlock *pBlock, IEC_UINT *upProc)
{
	IEC_UINT uRes	= OK;
	IEC_UINT uElem;

	SDBIInfo *pDBI	= &pVMM->DBI;
	SDBIType *pType;

	if (pDBI->pTypes == NULL)
	{
		uRes = dbiLoadTypes(pVMM);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	if (pInst->uIndex >= pDBI->uTypes)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	pType = pDBI->pTypes + pInst->uIndex;

	if (pType->pMember == NULL)
	{
		uRes = dbiLoadMember(pVMM, pType);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}
	
	pBlock->byLast = TRUE;

	if ((pType->uType & DBI_DTM_ARRAY) != 0 && (pType->uType & DBI_DTM_SIMPLE) != 0)
	{
		/* For simple arrays we only have the first array element stored
		 * within the Debug Information.
		 */
		uElem = 1;
	}
	else
	{
		uElem = pType->uMember;
	}

	for(; *upProc < uElem; (*upProc)++)
	{	
		uRes = dbiCopyMemberXDBI(pInst, pType, *upProc, pBlock);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiCollectAddress
 *
 */
static IEC_UINT dbiCollectAddress(STaskInfoVMM *pVMM, SDBIInstance *pInst, IEC_DATA *pParent, XBlock *pBlock)
{
	IEC_UINT uRes		= OK;
	IEC_UINT uIndex 	= NO_INDEX;

	SDBIInfo  *pDBI 	= &pVMM->DBI;
	SDBIType  *pType;
	XVisuVar  *xpVisu	= (XVisuVar *)pParent;

	if (pDBI->pTypes == NULL)
	{
		uRes = dbiLoadTypes(pVMM);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	if (pInst->uIndex >= pDBI->uTypes)
	{
		RETURN(ERR_INVALID_PARAM);
	}

	pType = pDBI->pTypes + pInst->uIndex;

	if (pType->pMember == NULL)
	{
		uRes = dbiLoadMember(pVMM, pType);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}
	
	pBlock->byLast = TRUE;

	uRes = dbiFindMember(pType, xpVisu->xVar.usNameSize, xpVisu->szName, &uIndex);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	uRes = dbiCopyMemberXVar(pInst, pType, uIndex, pBlock);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiLoadTypes
 *
 */
static IEC_UINT dbiLoadTypes(STaskInfoVMM *pVMM)
{
	IEC_UINT  uRes = OK;

	IEC_UDINT hFile;
	IEC_UINT  uLen;
	IEC_UDINT ulTypes;
	IEC_UINT  i;

	SDBIInfo *pDBI	= &pVMM->DBI;
	XDBIType  xType;

	uRes =	utilOpenFile(&hFile, (IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), osGetDBIDir, VMM_DIR_DBI, DBI_FILE_TYPE, FIO_MODE_READ);
	if (uRes != OK)
	{
		RETURN(uRes);
	}

	/* Read number of types first
	 */
	uLen = sizeof(IEC_UDINT);
	
	uRes = xxxRead(hFile, (IEC_DATA *)&ulTypes, &uLen);
	if (uRes != OK)
	{
		xxxClose(hFile);
		RETURN(uRes);
	}

	if (uLen != sizeof(IEC_UDINT))
	{
		xxxClose(hFile);
		RETURN(ERR_FILE_READ);
	}

	pDBI->uTypes = (IEC_UINT)ulTypes;

	pDBI->pTypes = (SDBIType *)osMalloc(pDBI->uTypes * sizeof(SDBIType));
	if (pDBI->pTypes == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	for (i = 0; i < pDBI->uTypes; i++)
	{
		uLen = sizeof(XDBIType);
		
		uRes = xxxRead(hFile, (IEC_DATA *)&xType, &uLen);
		if (uRes != OK)
		{
			xxxClose(hFile);
			RETURN(uRes);
		}

		if (uLen != sizeof(XDBIType))
		{
			xxxClose(hFile);
			RETURN(ERR_FILE_READ);
		}

		pDBI->pTypes[i].pMember = NULL;
		
		pDBI->pTypes[i].ulOffset = xType.ulOffset;
		pDBI->pTypes[i].uMember  = xType.uMember;
		pDBI->pTypes[i].uType	 = xType.uType;
	}

	xxxClose(hFile);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiDeleteTypes
 *
 */
static IEC_UINT dbiDeleteTypes(IEC_UINT uTypes, SDBIType *pTypes)
{
	IEC_UINT uRes = OK;
	IEC_UINT i;

	for (i = 0; i < uTypes; i++)
	{
		uRes = dbiDeleteType(pTypes + i);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiDeleteType
 *
 */
static IEC_UINT dbiDeleteType(SDBIType *pType)
{
	IEC_UINT uRes = OK;
	IEC_UINT i;
	IEC_UINT uElem;

	if (pType->pMember == NULL)
	{
		RETURN(OK);
	}

	if ((pType->uType & DBI_DTM_ARRAY) != 0 && (pType->uType & DBI_DTM_SIMPLE) != 0)
	{
		/* For simple arrays we only have the first array element stored
		 * within the Debug Information.
		 */
		uElem = 1;
	}
	else
	{
		uElem = pType->uMember;
	}

	for (i = 0; i < uElem; i++)
	{
		uRes = osFree((IEC_DATA OS_LPTR **)&pType->pMember[i].szName);
		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	uRes = osFree((IEC_DATA OS_LPTR **)&pType->pMember);

	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiLoadMember
 *
 */
static IEC_UINT dbiLoadMember(STaskInfoVMM *pVMM, SDBIType *pType)
{
	IEC_UINT uRes	= OK;
	IEC_UINT uLen;
	IEC_UINT i;
	IEC_UINT uElem;

	SDBIInfo *pDBI	= &pVMM->DBI;
	SDBIVar  *pVar;

	if ((VMF_FILE)pDBI->hVar == VMF_INVALID_HANDLE)
	{
		uRes =	utilOpenFile(&pDBI->hVar, (IEC_CHAR*)pVMM->pBuffer, sizeof(pVMM->pBuffer), osGetDBIDir,
								VMM_DIR_DBI, DBI_FILE_VAR, FIO_MODE_READ);

		if (uRes != OK)
		{
			RETURN(uRes);
		}
	}

	uRes = xxxSeek(pDBI->hVar, pType->ulOffset, FSK_SEEK_SET);
	if (uRes != OK)
	{
		RETURN(uRes);
	}
	
	if ((pType->uType & DBI_DTM_ARRAY) != 0 && (pType->uType & DBI_DTM_SIMPLE) != 0)
	{
		/* For simple arrays we only have the first array element stored
		 * within the Debug Information.
		 */
		uElem = 1;
	}
	else
	{
		uElem = pType->uMember;
	}

	pType->pMember = (SDBIVar *)osMalloc(uElem * sizeof(SDBIVar));
	if (pType->pMember == NULL)
	{
		RETURN(ERR_OUT_OF_MEMORY);
	}

	for (i = 0; i < uElem; i++)
	{
		pVar = pType->pMember + i;

		/* Read variable definition
		 */
		uLen = sizeof(XDBIVar);
	
		uRes = xxxRead(pDBI->hVar, (IEC_DATA *)&pVar->xVar, &uLen);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		if (uLen != sizeof(XDBIVar))
		{
			RETURN(ERR_FILE_READ);
		}

		/* Read variable name
		 */
		uLen = pVar->xVar.usNameSize;

		pVar->szName = (IEC_CHAR *)osMalloc(uLen + 1);
		if (pVar->szName == NULL)
		{
			RETURN(ERR_OUT_OF_MEMORY);
		}

		uRes = xxxRead(pDBI->hVar, (IEC_DATA *)pVar->szName, &uLen);
		if (uRes != OK)
		{
			RETURN(uRes);
		}

		if (uLen != pVar->xVar.usNameSize)
		{
			RETURN(ERR_FILE_READ);
		}

		pVar->szName[uLen] = 0;
	}
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiFindMember
 *
 */
static IEC_UINT dbiFindMember(SDBIType *pType, IEC_UINT uLen, IEC_CHAR *szMember, IEC_UINT *upIndex)
{
	IEC_UINT uFrom	 = 0u;
	IEC_UINT uTo	 = (IEC_UINT)(pType->uMember - 1);
	IEC_UINT uMiddle = 0u;

	IEC_DINT iDiff;

	SDBIVar *pVar;

	*upIndex = NO_INDEX;

	if (pType->uMember == 0)
	{
		RETURN(ERR_DBI_OBJ_NOT_FOUND);
	}

	if ((pType->uType & DBI_DTM_ARRAY) != 0 && (pType->uType & DBI_DTM_SIMPLE) != 0)
	{
		/* For simple arrays we only have the first array element stored
		 * within the Debug Information.
		 */
		IEC_UINT uArrayIndex = (IEC_UINT)OS_STRTOUL(szMember, NULL, 10);
		if (uArrayIndex >= pType->uMember)
		{
			RETURN(ERR_DBI_OBJ_NOT_FOUND);
		}

		*upIndex = uArrayIndex;
		RETURN(OK);
	}

	for(;;)
	{
		uMiddle = (IEC_UINT)(uFrom + (uTo - uFrom) / 2);

		pVar = pType->pMember + uMiddle;

		iDiff = OS_STRCMP(szMember, pVar->szName);
		
		if (iDiff == 0)
		{
			*upIndex = uMiddle;
			RETURN(OK);
		}

		if (uTo == uFrom)
		{
			RETURN(ERR_DBI_OBJ_NOT_FOUND);
		}

		if (iDiff <= 0)
		{
			uTo   = (IEC_UINT)(uMiddle - (uMiddle > uFrom ? 1 : 0));
		}
		else
		{
			uFrom = (IEC_UINT)(uMiddle + (uMiddle < uTo   ? 1 : 0));
		}
	}

	RETURN(ERR_ERROR);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiCopyMemberXVar
 *
 */
static IEC_UINT dbiCopyMemberXVar(SDBIInstance *pInst, SDBIType *pType, IEC_UINT uIndex, XBlock *pBlock)
{
	SDBIVar *pVar;
	XDBIVar *pxVar;

	IEC_USINT usVType = VMM_XV_LARGE;
	IEC_UINT  uVSize  = sizeof(XVariableL);

	IEC_UINT  uTempInst;
	IEC_UDINT ulTempOffs;
	
	IEC_UINT  uStrAdd = 0;

	if ((pType->uType & DBI_DTM_ARRAY) != 0 && (pType->uType & DBI_DTM_SIMPLE) != 0)
	{
		/* For simple arrays we only have the first array element stored
		 * within the Debug Information.
		 */
		pVar = pType->pMember + 0;
	}
	else
	{
		pVar = pType->pMember + uIndex;
	}

	pxVar = &pVar->xVar;

	if (pxVar->uInst == NO_INSTANCE && ((pxVar->usType & DBI_DTM_SIMPLE) != 0))
	{
		/* For simple member variables, use the instance ID of the parent!
		 */
		uTempInst = pInst->uInst;
	}
	else
	{
		uTempInst = pxVar->uInst;
	}

	if ((pxVar->usType & DBI_DT_STRING) != 0)
	{
		/* For strings also calculate the two length
		 * bytes!
		 */
		uStrAdd = 2 * sizeof(IEC_STRLEN);
	}
	else
	{
		uStrAdd = 0;
	}

	if ((pType->uType & DBI_DTM_ARRAY) != 0 && (pType->uType & DBI_DTM_SIMPLE) != 0)
	{
		/* For simple arrays we only have the first array element stored
		 * within the Debug Information.
		 */
		ulTempOffs = pxVar->ulOffset + (pxVar->uLen + uStrAdd) * uIndex;
	}
	else
	{
		ulTempOffs = pxVar->ulOffset;
	}
	
	/* Compute XVariable type ...
	 */
	if (ulTempOffs <= 0xffu && pxVar->uLen + uStrAdd <= 0xffu && uTempInst <= 0xffu)
	{
		usVType = VMM_XV_SMALL;
		uVSize	= sizeof(XVariableS);
	}
	else if (ulTempOffs <= 0xffffu && uTempInst <= 0xffu)
	{
		usVType = VMM_XV_MEDIUM;
		uVSize	= sizeof(XVariableM);
	}
	else
	{
		usVType = VMM_XV_LARGE;
		uVSize	= sizeof(XVariableL);
	}
	
	/* Check space in command buffer
	 */
	if (pBlock->uLen + uVSize + sizeof(IEC_UINT) > MAX_DATA)
	{
		pBlock->byLast = FALSE;
		RETURN(OK);
	}

	/* Copy data
	 */
	switch(usVType)
	{	
		case VMM_XV_SMALL:
		{
			XVariableS *pxVarS	= (XVariableS *)(pBlock->CMD.pData + pBlock->uLen);
	
			pxVarS->usType		= (IEC_USINT)(VMM_XV_SMALL | (pxVar->usBit & VMM_XV_BITMASK));

			pxVarS->usOffset	= (IEC_USINT)ulTempOffs;
			pxVarS->usLen		= (IEC_USINT)(pxVar->uLen + uStrAdd);
			pxVarS->usSegment	= (IEC_USINT)uTempInst;
						
			break;
		}

		case VMM_XV_MEDIUM:
		{
			XVariableM *pxVarM	= (XVariableM *)(pBlock->CMD.pData + pBlock->uLen);

			pxVarM->usType		= (IEC_USINT)(VMM_XV_MEDIUM | (pxVar->usBit & VMM_XV_BITMASK));

			pxVarM->uOffset 	= (IEC_UINT) ulTempOffs;
			pxVarM->uLen		= (IEC_UINT)(pxVar->uLen + uStrAdd);
			pxVarM->usSegment	= (IEC_USINT)uTempInst;

			break;
		}

		case VMM_XV_LARGE:
		{
			XVariableL *pxVarL	= (XVariableL *)(pBlock->CMD.pData + pBlock->uLen);

			pxVarL->usType		= (IEC_USINT)(VMM_XV_LARGE | (pxVar->usBit & VMM_XV_BITMASK));

			pxVarL->ulOffset	= (IEC_UDINT)ulTempOffs;
			pxVarL->uLen		= (IEC_UINT)(pxVar->uLen + uStrAdd);
			pxVarL->uSegment	= (IEC_UINT) uTempInst;

			break;
		}
	
	} /* switch(usVType) */

	*(IEC_UINT *)(pBlock->CMD.pData + pBlock->uLen + uVSize) = pxVar->usType;
	
	pBlock->uLen	= (IEC_UINT)(pBlock->uLen + uVSize + sizeof(IEC_UINT));
	pBlock->byLast	= TRUE;

	RETURN(OK);
}

/* ---------------------------------------------------------------------------- */
/**
 * dbiCopyMemberXDBI
 *
 */
static IEC_UINT dbiCopyMemberXDBI(SDBIInstance *pInst, SDBIType *pType, IEC_UINT uIndex, XBlock *pBlock)
{
	SDBIVar  *pVar;
	XVisuVar *pxVisu;
	
	XDBIVar  *pxDst;
	XDBIVar  *pxSrc;

	if ((pType->uType & DBI_DTM_ARRAY) != 0 && (pType->uType & DBI_DTM_SIMPLE) != 0)
	{
		/* For simple arrays we only have the first array element stored
		 * within the Debug Information.
		 */
		pVar = pType->pMember + 0;
	}
	else
	{
		pVar = pType->pMember + uIndex;
	}

	pxSrc = &pVar->xVar;

	if (pBlock->uLen + sizeof(XDBIVar) + pxSrc->usNameSize + 1 > MAX_DATA)
	{
		pBlock->byLast = FALSE;
		RETURN(OK);
	}

	pxVisu = (XVisuVar *)(pBlock->CMD.pData + pBlock->uLen);
	pxDst  = &pxVisu->xVar;

	OS_MEMCPY(pxDst, pxSrc, sizeof(XDBIVar));

	if ((pType->uType & DBI_DTM_ARRAY) != 0 && (pType->uType & DBI_DTM_SIMPLE) != 0)
	{
		/* For simple arrays we only have the first array element stored
		 * within the Debug Information.
		 */
		
		pxDst->ulOffset = pxDst->ulOffset + pxDst->uLen * uIndex;

		if ((pxDst->usType & DBI_DT_STRING) != 0)
		{
			/* For strings also calculate the two length
			 * bytes!
			 */
			pxDst->ulOffset = pxDst->ulOffset + 2 * sizeof(IEC_STRLEN) * uIndex;
		}
	}

	if ((pxDst->usType & DBI_DT_STRING) != 0)
	{
		/* For strings also calculate the two length
		 * bytes!
		 */
		pxDst->uLen = (IEC_UINT)(pxDst->uLen + 2 * sizeof(IEC_STRLEN));
	}

	OS_MEMCPY(pxVisu->szName, pVar->szName, pxDst->usNameSize);
	pxVisu->szName[pxDst->usNameSize] = '\0';

	pBlock->uLen = (IEC_UINT)(pBlock->uLen + sizeof(XDBIVar) + pxDst->usNameSize + 1);

	if (pxDst->uInst == NO_INSTANCE && ((pxDst->usType & DBI_DTM_SIMPLE) != 0))
	{
		pxDst->uInst = pInst->uInst;
	}

	pBlock->byLast = TRUE;

	RETURN(OK);
}

#endif	/* RTS_CFG_DEBUG_INFO */

/* ---------------------------------------------------------------------------- */

