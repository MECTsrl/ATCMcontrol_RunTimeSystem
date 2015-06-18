
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
 * Filename: libSfc.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"libSfc.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_SFC)

#include "libSfc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

/* ----  Local Defines:   ----------------------------------------------------- */

typedef struct
{
	IEC_STRLEN CurLen;
	IEC_STRLEN MaxLen;
	IEC_CHAR   Contents[256];

} VMIECSTRING;


/* The defines for all qualifiers (see array 'StepActionBlockQualifier')
 * All qualifiers with bitset '>=' than VM_SFC_QUAL__TIME are time-qualifiers.
 * '>=' means unsigned comparison
 */
#define VM_SFC_QUAL_N	  0x0001
#define VM_SFC_QUAL_R	  0x0002
#define VM_SFC_QUAL_S	  0x0004
#define VM_SFC_QUAL_P	  0x0008
#define VM_SFC_QUAL_P0	  0x0010
#define VM_SFC_QUAL_P1	  0x0020

#define VM_SFC_QUAL_L	  0x0040	/* currently not supported */
#define VM_SFC_QUAL_D	  0x0080	/* currently not supported */
#define VM_SFC_QUAL_SD	  0x0100	/* currently not supported */
#define VM_SFC_QUAL_DS	  0x0200	/* currently not supported */
#define VM_SFC_QUAL_SL	  0x0400	/* currently not supported */
#define VM_SFC_QUAL_TIME  0x0040	/* used to test 'is time qualifier' */


/* Pointers (not in instance-data) to SFC structure (data not changed) 
 */
typedef  IEC_INT  OS_DPTR * PTR_followTrans 				; /* [nSteps]		*/
typedef  IEC_INT  OS_DPTR * PTR_AltTrans					; /* [nTrans]		*/
typedef  IEC_INT  OS_DPTR * PTR_NextSteps					; /* [nNextSteps]	*/
typedef  IEC_INT  OS_DPTR * PTR_NextStepsOffset 			; /* [nTrans]		*/
typedef  IEC_INT  OS_DPTR * PTR_PrevSteps					; /* [nPrevSteps]	*/
typedef  IEC_INT  OS_DPTR * PTR_PrevStepsOffset 			; /* [nTrans]		*/
typedef  IEC_INT  OS_DPTR * PTR_StepActionBlocks			; /* [nStepAction]	*/
typedef  IEC_INT  OS_DPTR * PTR_StepActionBlockOffset		; /* [nSteps]		*/
typedef  IEC_INT  OS_DPTR * PTR_StepActionBlockQualifier	; /* [nStepAction]	one of the VM_SFC_QUAL__* bits */
typedef  IEC_DINT OS_DPTR * PTR_StepActionBlockTimePara 	; /* [nStepAction]	currently not supported and all are 0. */

/* Liste zur Verkopplung der Phasen 
 */
typedef  IEC_INT  OS_DPTR * PTR_doTransAct					; /* [max(anzTrans,anzAction)+1] */

/* Step and Action attributes: 
 */
typedef  IEC_DINT OS_DPTR * PTR_step__T 					; /* [nSteps]	all step.T in ms	*/
typedef  IEC_SINT OS_DPTR * PTR_step__X 					; /* [nSteps]	all step.X			*/
typedef  IEC_SINT OS_DPTR * PTR_action__A					; /* [nActions] all action.A		*/
typedef  IEC_SINT OS_DPTR * PTR_action__Q					; /* [nActions] all action.Q		*/

/* SFC internal runtime state: 
 */
typedef  IEC_INT  OS_DPTR * PTR_activeSteps 				; /* [anzSteps] 	*/
typedef  IEC_INT  OS_DPTR * PTR_enabledTrans				; /* [anzTrans] 	*/
typedef  IEC_INT  OS_DPTR * PTR_stepsToEnable				; /* [anzSteps] 	*/
typedef  IEC_INT  OS_DPTR * PTR_stepsToDisable				; /* [anzSteps] 	*/
typedef  SFC_BOOL OS_DPTR * PTR_forcedActions				; /* [anzActions]	*/
typedef  SFC_BOOL OS_DPTR * PTR_blockedActions				; /* [anzActions]	*/
typedef  SFC_BOOL OS_DPTR * PTR_onceforcedActions			; /* [anzActions]	*/
typedef  SFC_BOOL OS_DPTR * PTR_forcedTransitions			; /* [anzTrans] 	*/
typedef  SFC_BOOL OS_DPTR * PTR_blockedTransitions			; /* [anzTrans] 	*/
typedef  SFC_BOOL OS_DPTR * PTR_onceforcedTransitions		; /* [anzTrans] 	*/
typedef  IEC_INT  OS_DPTR * PTR_actionBlockSetQualifiers	; /* [anzActions]	*/
typedef  IEC_DINT OS_DPTR * PTR_actionBlockSetTimeParameter ; /* [anzActions]	*/
typedef  SFC_BOOL OS_DPTR * PTR_actionBlockStatus			; /* [anzActions]	*/
typedef  IEC_DINT OS_DPTR * PTR_stepStartTime				; /* [anzSteps] 	*/
   

/* SFC_Header - first data in instance-data
 * ----------------------------------------------------------------------------
 */
typedef struct _SFC_Header 
{
	/* Numbers of the various dynamically sized arrays: 
	 */
	IEC_INT 			  anzSteps;
	IEC_INT 			  anzTrans;
	IEC_INT 			  anzActions;
	IEC_INT 			  anzNextSteps;
	IEC_INT 			  anzPrevSteps;
	IEC_INT 			  anzStepAction;
	IEC_INT 			  initStep;

	/* offsets to the various dynamically sized arrays: 
	 * public state 
	 */
	IEC_INT 			  off_doTransAct			   ; /* [max(anzTrans,anzAction)+1] 	*/
	IEC_INT 			  off_step__T				   ; /* [anzSteps]		=> stepRunTime	*/
	IEC_INT 			  off_step__X				   ; /* [anzSteps]						*/
	IEC_INT 			  off_action__A 			   ; /* [anzActions]	=> actionBlockA */
	IEC_INT 			  off_action__Q 			   ; /* [anzActions]	=> actionBlockQ */

	/* SFC-structure 
	 */
	IEC_INT 			  off_followTrans;
	IEC_INT 			  off_AltTrans;
	IEC_INT 			  off_NextSteps;
	IEC_INT 			  off_NextStepsOffset;
	IEC_INT 			  off_PrevSteps;
	IEC_INT 			  off_PrevStepsOffset;
	IEC_INT 			  off_StepActionBlocks;
	IEC_INT 			  off_StepActionBlockOffset;
	IEC_INT 			  off_StepActionBlockQualifier;
	IEC_INT 			  off_StepActionBlockTimePara;
	IEC_INT 			  off_private_xx;
	
	/* Visualisation data:
	 */
	VMIECSTRING 		 __sfcvis;					  /* IEC_BYTE  __dummy1; */
	VMIECSTRING 		 __onceforcedtransitions;	  /* IEC_BYTE  __dummy2; */
	VMIECSTRING 		 __forcedtransitions;		  /* IEC_BYTE  __dummy3; */
	VMIECSTRING 		 __blockedtransitions;		  /* IEC_BYTE  __dummy4; */
	VMIECSTRING 		 __onceforcedactions;		  /* IEC_BYTE  __dummy5; */
	VMIECSTRING 		 __forcedactions;			  /* IEC_BYTE  __dummy6; */
	VMIECSTRING 		 __blockedactions;			  /* IEC_BYTE  __dummy7; */
	VMIECSTRING 		 __mancmd;					  /* IEC_BYTE  __dummy8; */
	IEC_SINT			 __control; 				  /* .0   __manualmode; */

	/* .1	__donext;	  
	 * .2	__alltransitionson;
	 * .3	__allactionsoff;
	 * .4	__resetSFC	True: SFC erster Aufruf
	 */
	IEC_SINT			__dummy;  

	/* local data to increase the speed 
	 */
	IEC_DINT						  actTime						; /* Systemzeit in Millisekunden */

	/* SFC structure 
	 */
	PTR_followTrans 				followTrans 					; /* [nSteps]		*/
	PTR_AltTrans					AltTrans						; /* [nTrans]		*/
	PTR_NextSteps					NextSteps						; /* [nNextSteps]	*/
	PTR_NextStepsOffset 			NextStepsOffset 				; /* [nTrans]		*/
	PTR_PrevSteps					PrevSteps						; /* [nPrevSteps]	*/
	PTR_PrevStepsOffset 			PrevStepsOffset 				; /* [nTrans]		*/
	PTR_StepActionBlocks			StepActionBlocks				; /* [nStepAction]	*/
	PTR_StepActionBlockOffset		StepActionBlockOffset			; /* [nSteps]		*/
	PTR_StepActionBlockQualifier	StepActionBlockQualifier		; /* [nStepAction]	one of the VM_SFC_QUAL__* bits */
	PTR_StepActionBlockTimePara 	StepActionBlockTimeParameter	; /* [nStepAction]	currently not supported and all are 0. */

	/* List 
	 */
	PTR_doTransAct					doTransAct						; /* [max(anzTrans,anzAction)+1] */

	/* SFC internal runtime state: 
	 */
	PTR_activeSteps 				activeSteps 					; /* [anzSteps] 	*/
	PTR_enabledTrans				enabledTrans					; /* [anzTrans] 	*/
	PTR_stepsToEnable				stepsToEnable					; /* [anzSteps] 	*/
	PTR_stepsToDisable				stepsToDisable					; /* [anzSteps] 	*/
	PTR_forcedActions				forcedActions					; /* [anzActions]	*/
	PTR_blockedActions				blockedActions					; /* [anzActions]	*/
	PTR_onceforcedActions			onceforcedActions				; /* [anzActions]	*/
	PTR_forcedTransitions			forcedTransitions				; /* [anzTrans] 	*/
	PTR_blockedTransitions			blockedTransitions				; /* [anzTrans] 	*/
	PTR_onceforcedTransitions		onceforcedTransitions			; /* [anzTrans] 	*/
	PTR_actionBlockSetQualifiers	actionBlockSetQualifiers		; /* [anzActions]	*/
	PTR_actionBlockSetTimeParameter actionBlockSetTimeParameter 	; /* [anzActions]	*/
	PTR_actionBlockStatus			actionBlockStatus				; /* [anzActions]	*/
	PTR_stepStartTime				stepStartTime					; /* [anzSteps] 	*/

	/* Step and Action attributes: 
	 */
	PTR_step__T 					step__T 						; /* [nSteps]	all step.T in ms	*/
	PTR_step__X 					step__X 						; /* [nSteps]	all step.X			*/
	PTR_action__A					action__A						; /* [nActions] all action.A		*/
	PTR_action__Q					action__Q						; /* [nActions] all action.Q		*/

} SFC_Header;


/* M-Lib function access structures
 * ----------------------------------------------------------------------------
 */
#define RTS_PRAGMA_PACK_1	/* >>>> Align 1 Begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_1

 typedef struct
{
	DEC_FUN_PTR(SFC_Header,pPtr);
	DEC_FUN_INT(retVal);

} SFC_CALCDOTRANS;

typedef struct
{
	DEC_FUN_PTR(SFC_Header,pPtr);

} SFC_CALCDOACT;

typedef struct
{
	DEC_FUN_PTR(SFC_Header,pPtr);

} SFC_FINALISE;

#define RTS_PRAGMA_PACK_DEF 	
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_DEF 	/* <<<< Align 1 end <<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

static void initVars	   (SFC_Header OS_DPTR * pSFCPOU);
static void activateSteps  (SFC_Header OS_DPTR * pSFCPOU);
static void deactivateSteps(SFC_Header OS_DPTR * pSFCPOU);
static void setStepTime    (SFC_Header OS_DPTR * pSFCPOU);
static IEC_INT isElementOf(IEC_INT OS_DPTR * iArray, IEC_INT elem, IEC_INT length);
static void insertIn   (IEC_INT OS_DPTR * iArray, IEC_INT elem, IEC_INT length);
static void deleteOut  (IEC_INT OS_DPTR * iArray, IEC_INT elem, IEC_INT length);
static void deleteAll  (void OS_DPTR * p, IEC_INT length);
static void enableTransitions		 (SFC_Header OS_DPTR * pSFCPOU);
static void doEnabledTransitionsPass1(SFC_Header OS_DPTR * pSFCPOU, SFC_BOOL manualMode, SFC_BOOL doNext);
static void doEnabledTransitionsPass2(SFC_Header OS_DPTR * pSFCPOU, SFC_BOOL manualMode, SFC_BOOL doNext);
static void doActiveActions 		 (SFC_Header OS_DPTR * pSFCPOU);
static void calcActionBlocks		 (SFC_Header OS_DPTR * pSFCPOU);
static void calcActionBQ			 (SFC_Header OS_DPTR * pSFCPOU, IEC_SINT OS_DPTR * q);

static void readManualModeString (VMIECSTRING OS_DPTR * manString, SFC_BOOL OS_DPTR * bArray, IEC_INT anz);
static void writeManualModeString(VMIECSTRING OS_DPTR * manString, SFC_BOOL OS_DPTR * bArray, IEC_INT anz);
static void fillVisString	(SFC_Header OS_DPTR * pSFCPOU);
static void readManCmdString(SFC_Header OS_DPTR * pSFCPOU);

static void x_itoa(IEC_CHAR OS_DPTR** ptr, IEC_INT value, IEC_CHAR OS_DPTR * end);

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * sfc_calcdotrans
 *
 * Calculate the transition numbers which have to calculated in this 
 * cycle. This is done by a library function named sfc_calcdotrans 
 * which stores the results the instance variable doTransAct[]. 
 */
void sfc_calcdotrans (STDLIBFUNCALL)
{
	IEC_SINT  doNext;
	IEC_SINT  manualmode;
	
	SFC_CALCDOTRANS OS_SPTR *pPar	 = (SFC_CALCDOTRANS OS_SPTR *)pIN;
	SFC_Header		OS_DPTR *pSFCPOU = (SFC_Header OS_DPTR *)pPar->pPtr;
	
	/* manual mode check, do sfc-cycle if manual-mode is of,
	 * or manual mode is on and donext signals that the user wants to do the next cycle 
	 */
	doNext	   = (IEC_SINT) ((pSFCPOU->__control & 0x0002) ? 1 : 0);
	manualmode = (IEC_SINT) ((pSFCPOU->__control & 0x0001) ? 1 : 0);
	
	/* do reset, sfc starts with initial step 
	 */
	if (pSFCPOU->__control & 0x0010)
	{
		initVars(pSFCPOU);
		/* ((FcString)locVars[varOffset+_RETAIN_OFF]).putString(""); */
		pSFCPOU->__control &= ~0x0010;
	}
	
	/* __MANCMD ++ 
	 */
	readManCmdString(pSFCPOU);
	
	/* update runtime for all active steps	
	 */
	pSFCPOU->actTime = osGetTime32();
	
	/* check step flags if user has set or deleted some active steps
	 * sfcReadStatus(pSFCPOU);	RETAIN 
	 */
	setStepTime(pSFCPOU);
	
	/* SFC-Interpreter Phase1 
	 */
	doEnabledTransitionsPass1(pSFCPOU, manualmode, doNext);
	
	pPar->retVal = (IEC_INT)((~(manualmode & ~doNext)) & 1);
}


/* ---------------------------------------------------------------------------- */
/**
 * sfc_calcdoact
 *
 * Calculate with the results of step 2) the actions which have to be 
 * executed. This is performed by a library function sfc_calcdoact 
 * which stores its results in the instance variable doTransAct[]. 
 */
void sfc_calcdoact	 (STDLIBFUNCALL)
{
	SFC_CALCDOACT OS_SPTR *pPar    = (SFC_CALCDOACT OS_SPTR *)pIN;
	SFC_Header	  OS_DPTR *pSFCPOU = (SFC_Header	OS_DPTR *)pPar->pPtr;
	
	/* manual mode check, do sfc-cycle if manual-mode is of, 
	 * or manual mode is on and donext signals that the user wants to do the next cycle 
	 */
	IEC_SINT	doNext;
	IEC_SINT	manualmode;
	
	doNext	   = (IEC_SINT) ((pSFCPOU->__control & 0x0002) ? 1 : 0);
	manualmode = (IEC_SINT) ((pSFCPOU->__control & 0x0001) ? 1 : 0);
	
	pSFCPOU->__control &= ~0x0002;	 /* reset donext */
	
	/* SFC-Interpreter Phase2 
	 */
	doEnabledTransitionsPass2(pSFCPOU, manualmode, doNext);
	
	calcActionBlocks(pSFCPOU);
	doActiveActions(pSFCPOU);
}


/* ---------------------------------------------------------------------------- */
/**
 * sfc_finalise
 *
 * Calculate the visualization data needed in the FarosPLC workbench. 
 * This is performed by a library function sfc_finalise which stores 
 * its results in the data called 'visualization data'. 
 */
void sfc_finalise (STDLIBFUNCALL)
{
	SFC_FINALISE OS_SPTR *pPar	  = (SFC_FINALISE OS_SPTR *)pIN;
	SFC_Header	 OS_DPTR *pSFCPOU = (SFC_Header   OS_DPTR *)pPar->pPtr;

	/* generate string for visualisation 
	 */
	fillVisString(pSFCPOU);
}


/* ---------------------------------------------------------------------------- */
/**
 * enableTransitions
 *
 * Find all enabled transitions, that means all transitions that should be 
 * calculated.
 * All steps before a transition have to be active to enable a transition.
 */
static void enableTransitions(SFC_Header OS_DPTR * pSFCPOU)
{
	IEC_INT 	 i=0;
	IEC_INT 	 aStep;
	IEC_INT 	 enTrans;
	SFC_BOOL	 prevTest;
	IEC_INT 	 pSt;
	
	deleteAll(pSFCPOU->enabledTrans, pSFCPOU->anzTrans);
	while (i<pSFCPOU->anzSteps/*activeSteps.length*/)
	{
		if (pSFCPOU->activeSteps[i] == -1)
		{
			break;
		}
		aStep	= pSFCPOU->activeSteps[i];
		enTrans = pSFCPOU->followTrans[aStep];
		if (enTrans != -1)
		{
			prevTest = TRUE;
			pSt = pSFCPOU->PrevStepsOffset[enTrans];
			while (pSt<pSFCPOU->anzPrevSteps/*prevSteps.length*/)
			{
				if (pSFCPOU->PrevSteps[pSt] == -1)
				{
					break;
				}
				if (isElementOf(pSFCPOU->activeSteps, pSFCPOU->PrevSteps[pSt], pSFCPOU->anzSteps) == -1)
				{
					prevTest = FALSE;
					break;
				}
				pSt++;
			}
			if (prevTest)
			{
				insertIn(pSFCPOU->enabledTrans, enTrans, pSFCPOU->anzTrans);
			}
		}
		i++;
	}
}


/* ---------------------------------------------------------------------------- */
/**
 * doEnabledTransitionsPass1
 *
 * calculate all enabled transitons
 * if a transition does not clear, calculate all alternative
 * transitions too
 * collect all enabled transition in doTransAct[]
 */
static void doEnabledTransitionsPass1(SFC_Header OS_DPTR * pSFCPOU, SFC_BOOL manualMode, SFC_BOOL doNext)
{
	IEC_INT    transNr;
	IEC_INT    enTCount = 0;
	SFC_BOOL   allTransitionsForced;
	SFC_BOOL   transVal;
	IEC_INT    index_doTransAct = 0;
	
	enableTransitions(pSFCPOU);
	
	/* read out manual mode vars; wir kopieren keine Strings
	 * String forcedTransitionsString = ((FcString)locVars[varOffset+_FORCEDTRANSITIONS_OFF]).getString();
	 * String blockedTransitionsString = ((FcString)locVars[varOffset+_BLOCKEDTRANSITIONS_OFF]).getString();
	 *((FcBool)locVars[varOffset+_ALLTRANSITIONSON_OFF]).getBool(); 
	 */
	allTransitionsForced = (SFC_BOOL) ((pSFCPOU->__control & 0x0004) ? 1 : 0); /* alltransitionson; */
	readManualModeString(&pSFCPOU->__forcedtransitions,  pSFCPOU->forcedTransitions,	pSFCPOU->anzTrans);
	readManualModeString(&pSFCPOU->__blockedtransitions, pSFCPOU->blockedTransitions,	pSFCPOU->anzTrans);
	
	/* start doing transitions 
	 */
	pSFCPOU->doTransAct[index_doTransAct] = -1;
	while (enTCount<pSFCPOU->anzTrans/*enabledTrans.length*/)
	{
		if (pSFCPOU->enabledTrans[enTCount] == -1)
			break;
		transNr = pSFCPOU->enabledTrans[enTCount];
		while (transNr != -1)
		{
			/* manual mode features:
			 *	  permanently forced transitions
			 *	  once forced transitions
			 *	  blocked transitions
			 *	  forced all transitions permanently 
			 */
			if (manualMode && !doNext)
			{
				transVal = FALSE;
			} else if (allTransitionsForced 
				|| pSFCPOU->forcedTransitions[transNr]
				|| pSFCPOU->onceforcedTransitions[transNr])
			{
				transVal = TRUE;
			} else if (pSFCPOU->blockedTransitions[transNr])
			{
				transVal = FALSE;
			} else 
			{
				/* transVal = doTransition(globals, inoutVars, tc, transNr);
				 * insert all calculated Transitionsnumbers in the them order 
				 */
				pSFCPOU->doTransAct[index_doTransAct++] = (IEC_INT) transNr;
				pSFCPOU->doTransAct[index_doTransAct]	= -1; /* temporary end */
			}

			/* Alle alternativen Transitionen müssen mitberechnet werden 
			 */
			transNr = pSFCPOU->AltTrans[transNr];
		}
		enTCount++;
	}
}


/* ---------------------------------------------------------------------------- */
/**
 * doEnabledTransitionsPass2
 *
 * calculate all enabled transitons
 * if a transition does not clear, calculate all alternative
 * transitions too
 */
static void doEnabledTransitionsPass2(SFC_Header OS_DPTR * pSFCPOU, SFC_BOOL manualMode, SFC_BOOL doNext)
{
	IEC_INT    transNr, j;
	IEC_INT    enTCount = 0;
	SFC_BOOL   allTransitionsForced;
	IEC_INT    i, transVal;
	IEC_INT    alter;
	IEC_INT    index_doTransAct = 0;
	
	/* start doing transitions 
	 */
	allTransitionsForced = (SFC_BOOL) ((pSFCPOU->__control & 0x0004) ? 1 : 0); /* alltransitionson; */
	while (enTCount<pSFCPOU->anzTrans/*enabledTrans.length*/)
	{
		if (pSFCPOU->enabledTrans[enTCount] == -1)
		{
			break;
		}
		transNr = pSFCPOU->enabledTrans[enTCount];
		alter = 1;
		while (transNr != -1)
		{

			/* manual mode features:
			 *	  permanently forced transitions
			 *	  once forced transitions
			 *	  blocked transitions
			 *	  forced all transitions permanently 
			 */
			if (manualMode && !doNext)
			{
				transVal = FALSE;
			} 
			else if (allTransitionsForced
				|| pSFCPOU->forcedTransitions[transNr]
				|| pSFCPOU->onceforcedTransitions[transNr])
			{
				transVal = TRUE;
			} 
			else if (pSFCPOU->blockedTransitions[transNr])
			{
				transVal = FALSE;
			} else {
				/* Ergebnis zurücklesen */
				transVal = pSFCPOU->doTransAct[index_doTransAct++];
			}
			
			if (transVal && alter)
			{
				pSFCPOU->onceforcedTransitions[transNr] = FALSE;
			}
			if (transVal && alter)
			{
				alter = 0;
				i = pSFCPOU->NextStepsOffset[transNr];
				while (i<pSFCPOU->anzNextSteps/*nextSteps.length*/)
				{
					if ((j=pSFCPOU->NextSteps[i]) == -1)
					{
						break;
					}
					insertIn(pSFCPOU->stepsToEnable, j, pSFCPOU->anzSteps);
					pSFCPOU->step__X[j] = 1;
					i++;
				}
				i=pSFCPOU->PrevStepsOffset[transNr];
				while (i<pSFCPOU->anzPrevSteps/*prevSteps.length*/)
				{
					if ((j=pSFCPOU->PrevSteps[i]) == -1)
					{
						break;
					}
					insertIn(pSFCPOU->stepsToDisable, j, pSFCPOU->anzSteps);
					pSFCPOU->step__X[j] = 0;
					i++;
				}

				/* aber jetzt noch die unbenutzten alternativen Transitionen überlesen 
				 */
				transNr = pSFCPOU->AltTrans[transNr];
			}
			else
			{
				transNr = pSFCPOU->AltTrans[transNr];
			}
		}
		enTCount++;
	}
	
	/* __MANCMD ++ 
	 *
	 * StringBuffer onceFTSt = new StringBuffer(); 
	 */
	pSFCPOU->__onceforcedtransitions.CurLen = 0;
	writeManualModeString(&pSFCPOU->__onceforcedtransitions, pSFCPOU->onceforcedTransitions, pSFCPOU->anzTrans/*onceForcedTransitions.length*/);
	/*((FcString)locVars[varOffset+_ONCEFORCEDTRANSITIONS_OFF]).putString(onceFTSt.toString()); */
}


/* ---------------------------------------------------------------------------- */
/**
 * calcActionBQ
 *
 * calc Q part of an action block, only R,S,N influence Q
 * (upper part of the function block in IEC)
 * (P ->Q is handled in calcActionBlocks)
 */
static void calcActionBQ(SFC_Header OS_DPTR * pSFCPOU, IEC_SINT OS_DPTR * q)
{
	/* calc set qualifiers for the action control blocks 
	 */
	IEC_INT i;
	IEC_INT stAB;
	
	for (i=0; i<pSFCPOU->anzActions; i++)
	{
		pSFCPOU->actionBlockSetQualifiers[i] = 0;
	}
	i = 0;
	while (i<pSFCPOU->anzSteps/*activeSteps.length*/)
	{
		if (pSFCPOU->activeSteps[i] == -1)
		{
			break;
		}
		stAB = pSFCPOU->StepActionBlockOffset[pSFCPOU->activeSteps[i]];
		while (stAB < pSFCPOU->anzStepAction/*stepActionBlocks.length*/)
		{
			if (pSFCPOU->StepActionBlocks[stAB]==-1)
			{
				break;
			}
			pSFCPOU->actionBlockSetQualifiers[pSFCPOU->StepActionBlocks[stAB]] |=
				pSFCPOU->StepActionBlockQualifier[stAB];
			if (pSFCPOU->StepActionBlockQualifier[stAB]>=VM_SFC_QUAL_TIME)
			{
				pSFCPOU->actionBlockSetTimeParameter[pSFCPOU->StepActionBlocks[stAB]] =
					pSFCPOU->StepActionBlockTimeParameter[stAB];
			}
			stAB++;
		}
		i++;
	}
	
	/* calc q output of action block 
	 */
	for (i=0; i<pSFCPOU->anzActions; i++)
	{
		SFC_BOOL abfN, abfR, abfS, abfQ, s;
		
		abfN = abfR = abfS = FALSE;
		abfQ = FALSE;

		/* N 
		 */
		if ((pSFCPOU->actionBlockSetQualifiers[i]&VM_SFC_QUAL_N) > 0)
		{
			abfN = TRUE;
		}

		/* R 
		 */
		if ((pSFCPOU->actionBlockSetQualifiers[i]&VM_SFC_QUAL_R) > 0)
		{
			abfR = TRUE;
		}

		/* S 
		 */
		s	 = (SFC_BOOL)( (pSFCPOU->actionBlockSetQualifiers[i] & VM_SFC_QUAL_S) > 0 );
		abfS = (SFC_BOOL)( (!abfR) & (s | pSFCPOU->actionBlockStatus[i]) );
		pSFCPOU->actionBlockStatus[i] = abfS;
		
		abfQ = (SFC_BOOL)( (abfN | abfS) & (!abfR) );
		
		/* store q for each action 
		 */
		q[i] = (IEC_SINT)abfQ;
	}
}


/* ---------------------------------------------------------------------------- */
/**
 * calcActionBlocks
 *
 */
static void calcActionBlocks(SFC_Header OS_DPTR * pSFCPOU)
{
	IEC_INT    i;
	IEC_INT    an;
	IEC_INT    stAB;
	IEC_SINT  allActionsOff;
	
	/* calc old Q outputs of action blocks 
	 */
	calcActionBQ(pSFCPOU, pSFCPOU->action__A/*actionBlockA*/);
	
	/* calc new active steps 
	 */
	deactivateSteps(pSFCPOU);
	activateSteps(pSFCPOU);
	
	/* calc new Q outputs of action blocks 
	 */
	calcActionBQ(pSFCPOU, pSFCPOU->action__Q);
	
	/* do all P qualifier first : P,P0,P1 
	 */
	
	/* steps with falling edge (look at P,P0) 
	 */
	i=0;
	while (i<pSFCPOU->anzSteps/*stepsToDisable.length*/)
	{
		if (pSFCPOU->stepsToDisable[i]==-1)
			break;

		/* look at all action blocks connected to that step 
		 */
		stAB = pSFCPOU->StepActionBlockOffset[pSFCPOU->stepsToDisable[i]];
		while (pSFCPOU->anzStepAction/*stAB<stepActionBlocks.length*/)
		{
			if (pSFCPOU->StepActionBlocks[stAB]==-1)
			{
				break;
			}

			/* P, P0 -> falling edge -> last calc of action (A=1,Q=0) 
			 */
			if (  ((pSFCPOU->StepActionBlockQualifier[stAB]&VM_SFC_QUAL_P)	> 0)
				||((pSFCPOU->StepActionBlockQualifier[stAB]&VM_SFC_QUAL_P0) > 0) )
			{
				pSFCPOU->action__A[pSFCPOU->StepActionBlocks[stAB]] = TRUE;
			}
			stAB++;
		}
		i++;
	}
	
	/* steps with rising edge (look at P,P1) 
	 */
	i = 0;
	while (i<pSFCPOU->anzSteps/*stepsToEnable.length*/)
	{
		if (pSFCPOU->stepsToEnable[i] == -1)
		{
			break;
		}

		/* look at all action blocks connected to that step 
		 */
		stAB = pSFCPOU->StepActionBlockOffset[pSFCPOU->stepsToEnable[i]];
		while (stAB<pSFCPOU->anzStepAction/*stepActionBlocks.length*/)
		{
			if (pSFCPOU->StepActionBlocks[stAB] == -1)
			{
				break;
			}

			/* P1 -> rising edge -> last calc of action (A=1,Q=0) 
			 */
			if ( (pSFCPOU->StepActionBlockQualifier[stAB]&VM_SFC_QUAL_P1)  > 0 )
			{
				pSFCPOU->action__A[pSFCPOU->StepActionBlocks[stAB]] = TRUE;
			}

			/* P -> rising edge -> last calc of action (A=1,Q=1) 
			 */
			if ( (pSFCPOU->StepActionBlockQualifier[stAB]&VM_SFC_QUAL_P) > 0)
			{
				pSFCPOU->action__Q[pSFCPOU->StepActionBlocks[stAB]] = TRUE;
			}
			stAB++;
		}
		i++;
	}
	
	/* now merge calculated Q,A for P qualifier with Q outputs stored in abQ[] 
	 */
	for (i=0; i<pSFCPOU->anzActions; i++)
	{
		pSFCPOU->action__A[i] |= pSFCPOU->action__Q[i];
	}
	
	deleteAll(pSFCPOU->stepsToDisable, pSFCPOU->anzSteps);
	deleteAll(pSFCPOU->stepsToEnable,  pSFCPOU->anzSteps);
	
	/* read out manual mode vars
	 * String forcedActionsString = ((FcString)locVars[varOffset+_FORCEDACTIONS_OFF]).getString();
	 * String blockedActionsString = ((FcString)locVars[varOffset+_BLOCKEDACTIONS_OFF]).getString();
	 * String onceForcedActionsString;
	 * onceForcedActionsString = ((FcString)locVars[varOffset+_ONCEFORCEDACTIONS_OFF]).getString();
	 * allActionsOff = ((FcBool)locVars[varOffset+_ALLACTIONSOFF_OFF]).getBool();
	 * if (!onceForcedActionsString.equals("")) ((FcString)locVars[varOffset+_ONCEFORCEDACTIONS_OFF]).putString(""); 
	 */
	
	allActionsOff = (IEC_SINT) ((pSFCPOU->__control & 0x0008) ? 1 : 0); /* allactionsoff; */
	readManualModeString(&pSFCPOU->__forcedactions,  pSFCPOU->forcedActions,  pSFCPOU->anzActions);
	readManualModeString(&pSFCPOU->__blockedactions, pSFCPOU->blockedActions, pSFCPOU->anzActions);
	readManualModeString(&pSFCPOU->__onceforcedactions, pSFCPOU->onceforcedActions, pSFCPOU->anzActions);
	pSFCPOU->__onceforcedactions.CurLen = 0;
	
	for (an=0; an<pSFCPOU->anzActions; an++)
	{
		if (allActionsOff)
		{
			pSFCPOU->action__A[an] = FALSE;
			pSFCPOU->action__Q[an] = FALSE;
		}
		else if (pSFCPOU->forcedActions[an] || pSFCPOU->onceforcedActions[an])
		{
			pSFCPOU->action__A[an] = TRUE;
			pSFCPOU->action__Q[an] = TRUE;
			pSFCPOU->onceforcedActions[an] = TRUE;
		}
		else if (pSFCPOU->blockedActions[an])
		{
			pSFCPOU->action__A[an] = FALSE;
			pSFCPOU->action__Q[an] = FALSE;
		}
	}
}


/* ---------------------------------------------------------------------------- */
/**
 * doActiveActions
 *
 */
static void doActiveActions(SFC_Header OS_DPTR * pSFCPOU)
{		 
	/* Do action, to which the step is no longer active, that means they are
	 * calculated for the last time 
	 */
	IEC_INT    i;
	IEC_INT    index_doTransAct = 0;
	
	pSFCPOU->doTransAct[index_doTransAct] = -1;
	for (i=0; i<pSFCPOU->anzActions; i++)
	{
		if (pSFCPOU->action__A[i] && !pSFCPOU->action__Q[i])
		{
			/* insert all calculated Transitionsnumbers in the them order 
			 */
			pSFCPOU->doTransAct[index_doTransAct++] =  (IEC_INT)i;
			pSFCPOU->doTransAct[index_doTransAct]	=  (IEC_INT)(-1); /* temporary end */
			/* doAction(globals,inoutVars,tc,i); */
		}
	}

	/* do the rest of the actions 
	 */
	for (i=0; i<pSFCPOU->anzActions; i++)
	{
		if (pSFCPOU->action__Q[i])
		{
			/* insert all calculated Transitionsnumbers in the them order 
			 */
			pSFCPOU->doTransAct[index_doTransAct++] = (IEC_INT)i;
			pSFCPOU->doTransAct[index_doTransAct]	= (IEC_INT)(-1); /* temporary end */
			/* doAction(globals,inoutVars,tc,i); */
		}
	}
}


/* ---------------------------------------------------------------------------- */
/**
 * fillVisString
 *
 * fill the string for the visualisation
 * Syntax: nSteps; {Stepnumbers;} nAction; {Actionnumbers;} nTrans; {Transnumbers;}
 */
static void fillVisString(SFC_Header OS_DPTR * pSFCPOU)
{	   
	IEC_INT 			 i, n, t;
	IEC_INT 			 size= pSFCPOU->__sfcvis.MaxLen; 
	IEC_CHAR OS_DPTR *	ps	= pSFCPOU->__sfcvis.Contents;
	IEC_CHAR OS_DPTR *	p	= ps;
	IEC_CHAR OS_DPTR *	pe	= p + size - 2;
	
	/* Anzahl der Steps bestimmen
	 */
	for (n = i = 0; i < pSFCPOU->anzSteps && pSFCPOU->activeSteps[i]!=-1; i++,n++)
		;

	x_itoa(&p, n, pe);
	*p++ = ';'; 
	if( p >= pe ) goto end;

	/* Stepnumbers 
	 */
	for (i = 0; i < n;i++) 
	{
		x_itoa(&p, pSFCPOU->activeSteps[i], pe);
		*p++ = ';'; 
		if( p >= pe ) goto end;
	}
	
	/* Anzahl der Actions bestimmen
	 */
	for (n = i = 0; i < pSFCPOU->anzActions; i++)
	{
		if( pSFCPOU->action__A[i] ) n++;
	}

	x_itoa(&p, n, pe);
	*p++ = ';'; 
	if( p >= pe ) goto end;
	
	/* Actionnumbers 
	 */
	for (i = 0; i < pSFCPOU->anzActions; i++) 
	{
		if( pSFCPOU->action__A[i] )
		{
			x_itoa(&p, i, pe);
			*p++ = ';'; 
			if( p >= pe ) goto end;
		}
	}
	
	/* Anzahl der Transitionen bestimmen
	 */
	for (n = i = 0; i<pSFCPOU->anzTrans &&(t=pSFCPOU->enabledTrans[i])!=-1;i++) 
	{
		n++; 
		while((t = pSFCPOU->AltTrans[t]) != -1 ) n++;
	}

	x_itoa(&p, n, pe);
	*p++ = ';'; 
	if( p >= pe ) goto end;


	/* Transitionnumbers 
	 */
	for (i = 0; i<pSFCPOU->anzTrans && (t=pSFCPOU->enabledTrans[i])!=-1; i++) 
	{
		x_itoa(&p, t, pe);
		*p++ = ';'; 
		if( p >= pe ) goto end;

		while((t = pSFCPOU->AltTrans[t]) != -1 )
		{	
			x_itoa(&p, t, pe);
			*p++ = ';'; 
			if( p >= pe ) goto end;
		}
	}
end:	  
	pSFCPOU->__sfcvis.CurLen = (IEC_SINT)(p-ps);
}


/* ---------------------------------------------------------------------------- */
/**
 * readManCmdString
 *
 * Syntax: "sa"|"ra"|{("s number" |"r number") ";" } ("s number" |"r number")
 */
static void readManCmdString(SFC_Header OS_DPTR * pSFCPOU)
{
	IEC_INT   i, num;
	IEC_INT   cmd = -1; /* -1 no cmd
						 *	1 set once forced transitions
						 *	2 reset once forced transitions
						 *	3 set all transitions once forced
						 *	4 reset all transitions once forced */
	IEC_CHAR OS_DPTR * p = pSFCPOU->__mancmd.Contents;
	IEC_CHAR OS_DPTR * pe; 
	
	pe = p + pSFCPOU->__mancmd.CurLen;
	/*String cmdString = ((FcString)locVars[varOffset+_MANCMD_OFF]).getString(); */
	/*((FcString)locVars[varOffset+_MANCMD_OFF]).putString(""); */
	
	if ( p < pe  )
	{
		while( p < pe )
		{  
			while ((p < pe) && (*p == ';')) p++;
			if (p >= pe) break;
			if( *p == 's' ) 
			{
				p++;
				if (p >= pe) break;
				if(*p == 'a') { cmd = 3; p++; if (p >= pe) break; }
				else cmd = 1;
			} 
			else if( *p == 'r' ) 
			{
				p++;
				if (p >= pe) break;
				if(*p == 'a') { cmd = 4; p++; if (p >= pe) break; }
				else cmd = 2;
			}
			while ((p < pe) && (*p == ';')) p++;
			if (p >= pe) break;
			num = 0;
			while ((p < pe) && (*p != ';')) 
			{
				if ((*p < '0') || (*p > '9'))
				{
					break;
				}
				num *= 10;
				num = (IEC_INT)(num + *p - '0');
				p++;
			}
			
			if (cmd==1)
			{
				if (num < pSFCPOU->anzTrans/*onceForcedTransitions.length*/ && num >= 0)
				{
					pSFCPOU->onceforcedTransitions[num] = TRUE;
				}
			}
			else if (cmd==2)
			{
				if (num < pSFCPOU->anzTrans/*onceForcedTransitions.length*/ && num >= 0)
				{
					pSFCPOU->onceforcedTransitions[num] = FALSE;
				}
			}
		}
		
		if (cmd==3)
		{
			for (i=0; i<pSFCPOU->anzTrans/*onceForcedTransitions.length*/; i++)
			{
				pSFCPOU->onceforcedTransitions[i] = TRUE;
			}
		}
		else if (cmd==4)
		{
			for (i=0; i<pSFCPOU->anzTrans/*onceForcedTransitions.length*/; i++)
			{
				pSFCPOU->onceforcedTransitions[i] = FALSE;
			}
		}
		
		/*StringBuffer onceFTSt = new StringBuffer(); */
		pSFCPOU->__onceforcedtransitions.CurLen = 0;
		writeManualModeString(&pSFCPOU->__onceforcedtransitions, pSFCPOU->onceforcedTransitions, pSFCPOU->anzTrans/*onceForcedTransitions.length*/);
	}
	
	pSFCPOU->__mancmd.CurLen = 0;
}


/* ---------------------------------------------------------------------------- */
/**
 * deleteAll
 *
 * deletes an integer array
 * that means filling the array with -1
 *
 * p	   base address
 * count   length to fill of 16bit elements
 */
static void deleteAll(void OS_DPTR * p, IEC_INT count)
{
	OS_MEMSET(p, 0xff, count * sizeof(IEC_INT));
}


/* ---------------------------------------------------------------------------- */
/**
 * insertIn
 *
 * Inserts one element elem in an integer array iArray	after all other members 
 * of the array, empty places are marked with -1.
 * If elem is already a membor of thar array no new element is added.
 */
static void insertIn(IEC_INT OS_DPTR * iArray, IEC_INT elem, IEC_INT length)
{
	IEC_INT i=0;
	IEC_INT j;
	
	while (i < length)
	{
		if (iArray[i]==elem)
		{
			return;
		}
		if (iArray[i]==-1 || iArray[i]>elem)
		{
			break;
		}
		i++;
	}
	
	if (i<length)
	{
		for (j=(IEC_INT)(length-1); j>i; j--)
		{
			iArray[j] = iArray[j-1];
		}
		iArray[i] = elem;
	}
}


/* ---------------------------------------------------------------------------- */
/**
 * deleteOut
 *
 * Deletes an element elem out of an integer array iArray the empty space is 
 * filled by the following elements.
 */
static void deleteOut(IEC_INT OS_DPTR * iArray, IEC_INT elem, IEC_INT length)
{
	IEC_INT nr;
	IEC_INT i;
	
	nr = isElementOf(iArray, elem, length);
	if (nr != -1)
	{
		for (i=nr; i<length-1; i++)
		{
			iArray[i] = iArray[i+1];
		}
		iArray[i] = -1; /* i=iArray.length-1 */
	}
}


/* ---------------------------------------------------------------------------- */
/**
 * isElementOf
 *
 * searchs for elem in an integer array iArray
 * returns the position of elem or -1 if elem is not an element of iArray
 */
static IEC_INT isElementOf(IEC_INT OS_DPTR * iArray, IEC_INT elem, IEC_INT length)
{
	IEC_INT i=0;
	
	while (i<length)
	{
		if (iArray[i] == elem)
		{
			return i;
		}
		if (iArray[i] == -1)
		{
			return -1;
		}
		i++;
	}
	return -1;
}


/* ---------------------------------------------------------------------------- */
/**
 * deactivateSteps
 *
 * Deactivate all steps in front of all cleared transitions these steps are 
 * marked in stepsToDisable
 */
static void deactivateSteps(SFC_Header OS_DPTR * pSFCPOU)
{
	IEC_INT i=0;
	
	while (i<pSFCPOU->anzSteps/*stepsToDisable.length*/)
	{
		if (pSFCPOU->stepsToDisable[i] == -1)
		{
			break;
		}
		deleteOut(pSFCPOU->activeSteps, pSFCPOU->stepsToDisable[i], pSFCPOU->anzSteps);
		
		i++;
	}
}


/* ---------------------------------------------------------------------------- */
/**
 * activateSteps
 *
 * activate all steps following a cleared transition these steps are marked 
 * in stepsToEnable
 */
static void activateSteps(SFC_Header OS_DPTR * pSFCPOU)
{
	IEC_INT i=0;
	
	while (i<pSFCPOU->anzSteps/*stepsToEnable.length*/)
	{
		if (pSFCPOU->stepsToEnable[i] == -1)
		{
			break;
		}
		if (isElementOf(pSFCPOU->activeSteps, pSFCPOU->stepsToEnable[i], pSFCPOU->anzSteps) == -1)
		{
			insertIn(pSFCPOU->activeSteps, pSFCPOU->stepsToEnable[i], pSFCPOU->anzSteps);
			
			pSFCPOU->stepStartTime[pSFCPOU->stepsToEnable[i]] = pSFCPOU->actTime;
			pSFCPOU->step__T[pSFCPOU->stepsToEnable[i]] = 0;
		}
		i++;
	}
}


/* ---------------------------------------------------------------------------- */
/**
 * setStepTime 
 *
 * update runtime for all active steps
 */
static void setStepTime(SFC_Header OS_DPTR * pSFCPOU)
{
	IEC_DINT  runTime;
	IEC_INT   i=0;
	
	while (i < pSFCPOU->anzSteps/*activeSteps.length*/)
	{
		if (pSFCPOU->activeSteps[i] == -1)
		{
			break;
		}
		if (isElementOf(pSFCPOU->stepsToDisable, pSFCPOU->activeSteps[i], pSFCPOU->anzSteps) == -1)
		{
			runTime = pSFCPOU->actTime - pSFCPOU->stepStartTime[pSFCPOU->activeSteps[i]];
			pSFCPOU->step__T[pSFCPOU->activeSteps[i]] = runTime;
		}
		i++;
	}
}


/* ---------------------------------------------------------------------------- */
/**
 * readManualModeString
 *
 * read a manual mode string and fill a boolean array, all numbers in the contained in the string
 * = true, the rest is false
 * syntax: manString ::= {number;} number		
 *		   number	 ::= {digit} 
 */
static void readManualModeString(VMIECSTRING OS_DPTR * manString, SFC_BOOL OS_DPTR * bArray, IEC_INT anz)
{
	IEC_INT 		   i, num;
	IEC_INT 		   len = (IEC_INT)manString->CurLen;
	IEC_CHAR OS_DPTR * p   = manString->Contents;
	
	for (i=0; i<anz; i++)
	{
		bArray[i] = FALSE;
	}
	
	while( len > 0 )
	{
		num = 0;
		while((*p != ';') && (len > 0)) 
		{
			if( (*p < '0') || (*p > '9') )
			{
				break;
			}
			num *= 10;
			num  = (IEC_INT)(num + *p - '0');
			p++; len--;
		}
		p++; len--;
		if (num<anz)
		{
			bArray[num] = TRUE;
		}
	}
}


/* ---------------------------------------------------------------------------- */
/**
 * x_itoa
 *
 * Konvertiere eine positive Dezimalzahl value nach *ptr 
 * *end zeigt auf letztes Byte 
 */
static void x_itoa(IEC_CHAR OS_DPTR** ptr, IEC_INT value, IEC_CHAR OS_DPTR * end)
{
	IEC_CHAR OS_DPTR * p		= *ptr;
	IEC_INT 			dezpos	= 10000;
	
	if (! value)
	{
		*p++ = '0';
	}
	else 
	{
		while( value < dezpos ) dezpos /= 10;

		while( dezpos > 0 && p < end) 
		{
			if( value >= dezpos ) 
			{
				*p++ = (IEC_CHAR)( (value / dezpos) + '0' );
				value %= dezpos;
			} else {
				*p++ = '0';
			}
			dezpos /= 10;
		}
	}
	*ptr = p;
}


/* ---------------------------------------------------------------------------- */
/**
  * 
  *
  * write a manual mode string from a boolean array, all numbers in the contained in the string
  * = true, the rest is false 
  */
static void writeManualModeString(VMIECSTRING OS_DPTR * manString, SFC_BOOL OS_DPTR * bArray, IEC_INT anz)
{
	IEC_CHAR OS_DPTR *p 	= manString->Contents;
	IEC_CHAR OS_DPTR *pe	= p + manString->MaxLen - 1;
	IEC_INT 		  first = TRUE;
	IEC_INT 		  n;
	
	for (n=0; n<anz; n++)
	{
		if (bArray[n])
		{
			if (first) first = FALSE; else *p++ = ';';
			x_itoa(&p, n, pe);
			if( p >= pe ) break;
		}
	}
	manString->CurLen = (IEC_SINT)(p-manString->Contents);
}


/* ---------------------------------------------------------------------------- */
/** 
 * initVars
 *
 */
static void initVars(SFC_Header OS_DPTR *pSFCPOU)
{
	IEC_SINT OS_DPTR *pBYTE = (IEC_SINT OS_DPTR *)pSFCPOU;
	IEC_INT  i;
	
	/* Pointers (not in instance-data) to SFC structure (data not changed) 
	 */
	pSFCPOU->followTrans				   = (PTR_followTrans)				   (pBYTE + pSFCPOU->off_followTrans);
	pSFCPOU->AltTrans					   = (PTR_AltTrans) 				   (pBYTE + pSFCPOU->off_AltTrans);
	pSFCPOU->NextSteps					   = (PTR_NextSteps)				   (pBYTE + pSFCPOU->off_NextSteps);
	pSFCPOU->NextStepsOffset			   = (PTR_NextStepsOffset)			   (pBYTE + pSFCPOU->off_NextStepsOffset);
	pSFCPOU->PrevSteps					   = (PTR_PrevSteps)				   (pBYTE + pSFCPOU->off_PrevSteps);
	pSFCPOU->PrevStepsOffset			   = (PTR_PrevStepsOffset)			   (pBYTE + pSFCPOU->off_PrevStepsOffset);
	pSFCPOU->StepActionBlocks			   = (PTR_StepActionBlocks) 		   (pBYTE + pSFCPOU->off_StepActionBlocks);
	pSFCPOU->StepActionBlockOffset		   = (PTR_StepActionBlockOffset)	   (pBYTE + pSFCPOU->off_StepActionBlockOffset);
	pSFCPOU->StepActionBlockQualifier	   = (PTR_StepActionBlockQualifier)    (pBYTE + pSFCPOU->off_StepActionBlockQualifier);
	pSFCPOU->StepActionBlockTimeParameter  = (PTR_StepActionBlockTimePara)	   (pBYTE + pSFCPOU->off_StepActionBlockTimePara);
	
	/* Step and Action attributes: 
	 */
	pSFCPOU->doTransAct 				   = (PTR_doTransAct)				   (pBYTE + pSFCPOU->off_doTransAct);
	pSFCPOU->step__T					   = (PTR_step__T)					   (pBYTE + pSFCPOU->off_step__T);
	pSFCPOU->step__X					   = (PTR_step__X)					   (pBYTE + pSFCPOU->off_step__X);
	pSFCPOU->action__A					   = (PTR_action__A)				   (pBYTE + pSFCPOU->off_action__A);
	pSFCPOU->action__Q					   = (PTR_action__Q)				   (pBYTE + pSFCPOU->off_action__Q);
	
	/* Pointers to local data	
	 */
	pBYTE += pSFCPOU->off_private_xx;
	pSFCPOU->activeSteps				   = (PTR_activeSteps)				   pBYTE; pBYTE += sizeof(IEC_INT ) * pSFCPOU->anzSteps;
	pSFCPOU->enabledTrans				   = (PTR_enabledTrans) 			   pBYTE; pBYTE += sizeof(IEC_INT ) * pSFCPOU->anzTrans;
	pSFCPOU->stepsToEnable				   = (PTR_stepsToEnable)			   pBYTE; pBYTE += sizeof(IEC_INT ) * pSFCPOU->anzSteps;
	pSFCPOU->stepsToDisable 			   = (PTR_stepsToDisable)			   pBYTE; pBYTE += sizeof(IEC_INT ) * pSFCPOU->anzSteps;
	pSFCPOU->forcedActions				   = (PTR_forcedActions)			   pBYTE; pBYTE += sizeof(SFC_BOOL) * pSFCPOU->anzActions;
	pSFCPOU->blockedActions 			   = (PTR_blockedActions)			   pBYTE; pBYTE += sizeof(SFC_BOOL) * pSFCPOU->anzActions;
	pSFCPOU->onceforcedActions			   = (PTR_onceforcedActions)		   pBYTE; pBYTE += sizeof(SFC_BOOL) * pSFCPOU->anzActions;
	pSFCPOU->forcedTransitions			   = (PTR_forcedTransitions)		   pBYTE; pBYTE += sizeof(SFC_BOOL) * pSFCPOU->anzTrans;
	pSFCPOU->blockedTransitions 		   = (PTR_blockedTransitions)		   pBYTE; pBYTE += sizeof(SFC_BOOL) * pSFCPOU->anzTrans;
	pSFCPOU->onceforcedTransitions		   = (PTR_onceforcedTransitions)	   pBYTE; pBYTE += sizeof(SFC_BOOL) * pSFCPOU->anzTrans;
	pSFCPOU->actionBlockSetQualifiers	   = (PTR_actionBlockSetQualifiers)    pBYTE; pBYTE += sizeof(IEC_INT ) * pSFCPOU->anzActions;
	pSFCPOU->actionBlockSetTimeParameter   = (PTR_actionBlockSetTimeParameter) pBYTE; pBYTE += sizeof(IEC_DINT) * pSFCPOU->anzActions;
	pSFCPOU->actionBlockStatus			   = (PTR_actionBlockStatus)		   pBYTE; pBYTE += sizeof(SFC_BOOL) * pSFCPOU->anzActions;
	pSFCPOU->stepStartTime				   = (PTR_stepStartTime)			   pBYTE; 
	
	/* Initialize 
	 */
	deleteAll(pSFCPOU->enabledTrans,  pSFCPOU->anzTrans);
	deleteAll(pSFCPOU->activeSteps,   pSFCPOU->anzSteps);
	deleteAll(pSFCPOU->stepsToEnable, pSFCPOU->anzSteps);
	deleteAll(pSFCPOU->stepsToDisable,pSFCPOU->anzSteps);
	
	for (i=0; i<pSFCPOU->anzActions; i++)
	{
		pSFCPOU->actionBlockStatus[i] = FALSE;
	}
	
	/* activated Initialstep  
	 */
	insertIn(pSFCPOU->stepsToEnable, pSFCPOU->initStep, pSFCPOU->anzSteps);
	pSFCPOU->step__X[pSFCPOU->initStep] = 1;
}

#endif	/* RTS_CFG_SFC */

/* ---------------------------------------------------------------------------- */
