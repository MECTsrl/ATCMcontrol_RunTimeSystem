
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
 * Filename: vmPrc.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"vmPrc.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_TASK_IMAGE) | defined(RTS_CFG_WRITE_FLAGS) | defined(RTS_CFG_IO_LAYER)

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


#if defined(RTS_CFG_WRITE_FLAGS_PI) && ! defined(RTS_CFG_WRITE_FLAGS)
#error RTS_CFG_WRITE_FLAGS_PI requires RTS_CFG_WRITE_FLAGS to be also set in osDef.h!
#endif

/* ---------------------------------------------------------------------------- */
/**
 * prcGetTaskImage
 *
 */
IEC_UINT prcGetTaskImage(STaskInfoVM *pVM)
{
	IEC_UINT  uRes = OK;
	SImageReg *pIR = pVM->Task.pIR;

	TR_GET3("TSK %d:  R:%6d W:%6d   ----------", pVM->usTask, pIR->uRegionsRd, pIR->uRegionsWr);

	if (pIR->uRegionsRd == 0)
	{
		/* Nothing to do...
		 */
		RETURN(OK);
	}
	
	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

  #if defined(RTS_CFG_TASK_IMAGE)
	OS_BEGIN_CRITICAL_SECTION(SEM_TASK_IMAGE)
  #endif  

	{	/* -------------------------------------------------------------------- */

  #if defined(RTS_CFG_TASK_IMAGE)
	
  #if ! defined(RTS_CFG_MECT)
	uRes = osBeginGetTaskImage(pVM);
	TR_RET(uRes);
  #endif

  #if defined(RTS_CFG_IO_LAYER)
	if (uRes == OK)
	{
		uRes = ioNotifyLayer(NULL, pVM, FALSE, 0, 0, 0, 0);
		TR_RET(uRes);
	}
  #endif

	if (uRes == OK)
	{
		IEC_UDINT i;

		SObject *pSeg  = pVM->Local.pSeg;
		SObject *pData = pVM->pShared->pData;
		
		/* IEC_ULINT ull = osGetTimeUSEx(); */

		for (i = 0; i < pIR->uRegionsRd; i++)
		{
			SRegion *pReg = pIR->pRegionRd + i;

		  #if defined(_SOF_4CFC_SRC_) && defined(RTS_CFG_BACNET)

			if (pVM->bInitDone == TRUE && pReg->ulOffset >= 0x010000ul && (pReg->uSize == 72 || pReg->uSize == 60))
			{
				/* Special BC BACnet hack: Offset of BACnet object is always greater
				 * than 0x010000ul and only the last 16 bytes are changed.
				 */
				IEC_UDINT ulOffs = pReg->ulOffset + pReg->uSize - 2 * sizeof(IEC_ULINT);

				*(IEC_ULINT *)(pSeg[pReg->usSegment].pAdr + ulOffs) =
						*(IEC_ULINT *)(pData[pReg->usSegment].pAdr + ulOffs);

				ulOffs += sizeof(IEC_ULINT);
				
				*(IEC_ULINT *)(pSeg[pReg->usSegment].pAdr + ulOffs) =
						*(IEC_ULINT *)(pData[pReg->usSegment].pAdr + ulOffs);

				continue;
			}
		  #endif
			
			if (pReg->uSize == sizeof(IEC_ULINT))
			{
				*(IEC_ULINT *)(pSeg[pReg->usSegment].pAdr + pReg->ulOffset) =
						*(IEC_ULINT *)(pData[pReg->usSegment].pAdr + pReg->ulOffset);
			}
			if (pReg->uSize == sizeof(IEC_UDINT))
			{
				*(IEC_UDINT *)(pSeg[pReg->usSegment].pAdr + pReg->ulOffset) =
						*(IEC_UDINT *)(pData[pReg->usSegment].pAdr + pReg->ulOffset);
			}
			else if (pReg->uSize == sizeof(IEC_UINT))
			{
				*(IEC_UINT *)(pSeg[pReg->usSegment].pAdr + pReg->ulOffset) =
						*(IEC_UINT *)(pData[pReg->usSegment].pAdr + pReg->ulOffset);
			}
			else if (pReg->uSize == sizeof(IEC_BYTE))
			{
				*(pSeg[pReg->usSegment].pAdr + pReg->ulOffset) =
						*(pData[pReg->usSegment].pAdr + pReg->ulOffset);
			}
			else
			{
				OS_MEMCPY(pSeg [pReg->usSegment].pAdr + pReg->ulOffset, 
						pData[pReg->usSegment].pAdr + pReg->ulOffset, pReg->uSize);
			}
			
			TR_GET3("Seg %s:  O:%6x L:%6x  ", 
				pReg->usSegment == SEG_INPUT ? "I" : "Q", pReg->ulOffset, pReg->uSize);

		} /* for (i = 0; i < pIR->uRegionsRd; i++) */
	
		/* fprintf(stdout, "Tg: %5d\r\n", (IEC_UDINT)utilGetTimeDiffUSEx(ull)); */

	} /* if (uRes == OK) */

  #if defined(_SOF_4CFC_SRC_) && defined(RTS_CFG_BACNET)

	/* Special BC BACnet hack: Only the last 16 bytes off BACnet objects 
	 * are changed. However we need to initially initilize the other 
	 * members.
	 */
	pVM->bInitDone = TRUE;

  #endif

  #if ! defined(RTS_CFG_MECT)
	if (uRes == OK)
	{
		uRes = osEndGetTaskImage(pVM);
		TR_RET(uRes);
	}
  #endif

	if (uRes == WRN_HANDLED)
	{
		uRes = OK;
	}

  #endif /* RTS_CFG_TASK_IMAGE */

	}	/* -------------------------------------------------------------------- */
		
  #if defined(RTS_CFG_TASK_IMAGE)
	OS_END_CRITICAL_SECTION(SEM_TASK_IMAGE)
  #endif

	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ------------------------ */
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * prcSetTaskImage
 *
 */
IEC_UINT prcSetTaskImage(STaskInfoVM *pVM)
{
	IEC_UINT uRes		= OK;
  #if defined(RTS_CFG_IO_LAYER)
	IEC_BOOL bChanged	= FALSE;
  #endif

	SImageReg *pIR = pVM->Task.pIR;

	if (pIR->uRegionsWr == 0)
	{
		/* Nothing to do...
		 */
		RETURN(OK);
	}

	/* >>>	C R I T I C A L   S E C T I O N  -	B E G I N  -------------------- */

  #if defined(RTS_CFG_TASK_IMAGE)
	OS_BEGIN_CRITICAL_SECTION(SEM_TASK_IMAGE)
  #endif  

	{	/* -------------------------------------------------------------------- */

  #if defined(RTS_CFG_TASK_IMAGE)

  #if ! defined(RTS_CFG_MECT)
	uRes = osBeginSetTaskImage(pVM);
	TR_RET(uRes);
  #endif

	if (uRes == OK)
	{
		IEC_UDINT	i;

		IEC_DATA	OS_DPTR *pSeg	= pVM->Local.pSeg[SEG_OUTPUT].pAdr;
		IEC_DATA	OS_DPTR *pData	= pVM->pShared->pData[SEG_OUTPUT].pAdr;

	  #if defined(RTS_CFG_WRITE_FLAGS)
		IEC_DATA	OS_DPTR *pChg	= pVM->Local.WriteFlags.pAdr;
		IEC_UDINT	j;
	  #endif

	  #if defined(RTS_CFG_WRITE_FLAGS_PI)
		IEC_DATA	OS_DPTR *pWF	= pVM->pShared->WriteFlags.pAdr;
	  #endif

		/* IEC_ULINT ull = osGetTimeUSEx(); */

		for (i = 0; i < pIR->uRegionsWr; i++)
		{
			SRegion   *pReg  = pIR->pRegionWr + i;

		  #if ! defined(RTS_CFG_WRITE_FLAGS)

			OS_MEMCPY(OS_ADDPTR(pData, pReg->ulOffset), OS_ADDPTR(pSeg, pReg->ulOffset), pReg->uSize);

		  #if defined(RTS_CFG_IO_LAYER)
			bChanged = TRUE;
		  #endif				

		  #else /* ! RTS_CFG_WRITE_FLAGS */

			IEC_UDINT ulStop = pReg->ulOffset + pReg->uSize;

			TR_SET3("Seg %s:  O:%6x L:%6x  ", "Q", pReg->ulOffset, pReg->uSize);

		  #if defined(_SOF_4CFC_SRC_) && defined(RTS_CFG_BACNET)

			if (pReg->ulOffset >= 0x010000ul && pReg->uSize == 72)
			{
				/* Special BC BACnet hack: Offset of BACnet object is always greater
				 * than 0x010000ul and we only write to the last 4 bytes.
				 */
				IEC_UDINT ulOffs = pReg->ulOffset + pReg->uSize - sizeof(IEC_UDINT);
				IEC_UDINT uChg	 = *(IEC_UDINT *)(pChg + ulOffs);

				if (uChg != 0)
				{
					*(IEC_UDINT *)(pChg  + ulOffs) = 0;
					*(IEC_UDINT *)(pData + ulOffs) = 
						(IEC_UDINT)((*(IEC_UDINT *)(pData + ulOffs) & ~uChg) | (*(IEC_UDINT *)(pSeg + ulOffs) & uChg));

				  #if defined(RTS_CFG_WRITE_FLAGS_PI)
					*(IEC_UDINT *)(pWF + ulOffs)	 = (IEC_UDINT)(*(IEC_UDINT *)(pWF + ulOffs) | uChg);
				  #endif

				  #if defined(RTS_CFG_IO_LAYER)
					TR_PLUS;
					bChanged = TRUE;
				  #endif
				}

				continue;
			}
		  #endif

			for (j = pReg->ulOffset; j + sizeof(IEC_ULINT) <= ulStop; j = j + sizeof(IEC_ULINT))
			{
				IEC_ULINT uChg = *(IEC_ULINT *)(pChg + j);

				if (uChg != 0)
				{
					*(IEC_ULINT *)(pChg  + j) = 0;
					*(IEC_ULINT *)(pData + j) = 
						(IEC_ULINT)((*(IEC_ULINT *)(pData + j) & ~uChg) | (*(IEC_ULINT *)(pSeg + j) & uChg));

				  #if defined(RTS_CFG_WRITE_FLAGS_PI)
					*(IEC_ULINT *)(pWF + j)  = (IEC_ULINT)(*(IEC_ULINT *)(pWF + j) | uChg);
				  #endif

				  #if defined(RTS_CFG_IO_LAYER)
					TR_PLUS;
					bChanged = TRUE;
				  #endif
				}

			} /* for (j = pReg->ulOffset; j + sizeof(IEC_ULINT) <= ulStop; j = j + sizeof(IEC_ULINT)) */

			for (; j + sizeof(IEC_UDINT) <= ulStop; j = j + sizeof(IEC_UDINT))
			{
				IEC_UDINT uChg = *(IEC_UDINT *)(pChg + j);

				if (uChg != 0)
				{
					*(IEC_UDINT *)(pChg  + j) = 0;
					*(IEC_UDINT *)(pData + j) = 
						(IEC_UDINT)((*(IEC_UDINT *)(pData + j) & ~uChg) | (*(IEC_UDINT *)(pSeg + j) & uChg));

				  #if defined(RTS_CFG_WRITE_FLAGS_PI)
					*(IEC_UDINT *)(pWF + j)  = (IEC_UDINT)(*(IEC_UDINT *)(pWF + j) | uChg);
				  #endif

				  #if defined(RTS_CFG_IO_LAYER)
					TR_PLUS;
					bChanged = TRUE;
				  #endif
				}

			} /* for (; j + sizeof(IEC_UDINT) <= ulStop; j = j + sizeof(IEC_UDINT)) */

			for (; j + sizeof(IEC_UINT) <= ulStop; j = j + sizeof(IEC_UINT))
			{
				IEC_UINT uChg = *(IEC_UINT *)(pChg + j);

				if (uChg != 0)
				{
					*(IEC_UINT *)(pChg	+ j) = 0;
					*(IEC_UINT *)(pData + j) = 
						(IEC_UINT)((*(IEC_UINT *)(pData + j) & ~uChg) | (*(IEC_UINT *)(pSeg + j) & uChg));

				  #if defined(RTS_CFG_WRITE_FLAGS_PI)
					*(IEC_UINT *)(pWF + j)	 = (IEC_UINT)(*(IEC_UINT *)(pWF + j) | uChg);
				  #endif

				  #if defined(RTS_CFG_IO_LAYER)
					TR_PLUS;
					bChanged = TRUE;
				  #endif
				}

			} /* for (; j + sizeof(IEC_UINT) <= ulStop; j = j + sizeof(IEC_UINT)) */

			for (; j + sizeof(IEC_USINT) <= ulStop; j = j + sizeof(IEC_USINT))
			{
				IEC_USINT uChg = *(IEC_USINT *)(pChg + j);
				
				if (uChg != 0)
				{
					*(IEC_USINT *)(pChg  + j) = 0;
					*(IEC_USINT *)(pData + j) = 
						(IEC_USINT)((*(IEC_USINT *)(pData + j) & ~uChg) | (*(IEC_USINT *)(pSeg + j) & uChg));
					
				  #if defined(RTS_CFG_WRITE_FLAGS_PI)
					*(IEC_USINT *)(pWF	 + j) = (IEC_USINT)(*(IEC_USINT *)(pWF + j) | uChg);
				  #endif

				  #if defined(RTS_CFG_IO_LAYER)
					TR_PLUS;
					bChanged = TRUE;
				  #endif
				}
			
			} /* for (; j + sizeof(IEC_USINT) <= ulStop; j = j + sizeof(IEC_USINT)) */

			TR_SET_NL;

		  #endif /* ! RTS_CFG_WRITE_FLAGS */

		} /* for (i = 0; i < pIR->uRegionsWr; i++) */
	
		/* fprintf(stdout, "T2: %5d\r\n", (IEC_UDINT)utilGetTimeDiffUSEx(ull)); */

	} /* if (uRes == OK) */

  #if defined(RTS_CFG_IO_LAYER)
	if (uRes == OK && bChanged == TRUE)
	{
		uRes = ioNotifyLayer(NULL, pVM, TRUE, 0, 0, 0, 0);
		TR_RET(uRes);
	}
  #endif

  #if ! defined(RTS_CFG_MECT)
	if (uRes == OK)
	{
		uRes = osEndSetTaskImage(pVM);
		TR_RET(uRes);
	}
  #endif

	if (uRes == WRN_HANDLED)
	{
		uRes = OK;
	}
	
  #endif  /* RTS_CFG_TASK_IMAGE */
	
	}	/* -------------------------------------------------------------------- */
		
  #if defined(RTS_CFG_TASK_IMAGE)
	OS_END_CRITICAL_SECTION(SEM_TASK_IMAGE)
  #endif

	/* <<<	C R I T I C A L   S E C T I O N  -	E N D  ------------------------ */

	RETURN(uRes);
}

#endif	/* RTS_CFG_TASK_IMAGE | RTS_CFG_WRITE_FLAGS | RTS_CFG_IO_LAYER */

/* ---------------------------------------------------------------------------- */
