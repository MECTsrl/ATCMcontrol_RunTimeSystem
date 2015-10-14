
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
 * Filename: vmmMain.h
 */


#ifndef _VMMMAIN_H_
#define _VMMMAIN_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#ifdef __cplusplus
extern "C" { 
#endif

	
/* vmmMain.c - Main and Common Functions
 * ----------------------------------------------------------------------------
 */
IEC_UINT vmmMainST(void);
IEC_UINT vmmMainMT(void);
IEC_UINT vmmMainIPC(void);

IEC_UINT vmmQueueMessage(SMsgQueue *pQueue, IEC_CHAR *szMessage);
IEC_UINT vmmQueueBPNotification(STaskInfoVM *pVM, IEC_UINT uState);
IEC_UINT vmmFormatException(SException *pExcept, STaskInfoVM *pVM, IEC_UDINT ulErrNo, IEC_CHAR *szBuffer);

IEC_UINT vmmStartTask(STaskInfoVMM *pVMM, IEC_UINT uTask);
IEC_UINT vmmStopTask(STaskInfoVMM *pVMM,  IEC_UINT uTask, IEC_BOOL bSync);
IEC_UINT vmmContinueTask(STaskInfoVMM *pVMM, IEC_UINT uTask);
IEC_UINT vmmSingleStep(STaskInfoVMM *pVMM, IEC_UINT uTask);
IEC_UINT vmmDeleteTask(STaskInfoVMM *pVMM, IEC_UINT uTask);
IEC_UINT vmmResetTask(STaskInfoVMM *pVMM, IEC_UINT uTask);

IEC_UINT vmmBuildVMEvents(STaskInfoVMM *pVMM);
IEC_UINT vmmClearVMEvents(STaskInfoVMM *pVMM);
IEC_UINT vmmSetEvent(STaskInfoVMM *pVMM, IEC_UINT uEvent);


/* vmmCmd.c - Command State Machine
 * ----------------------------------------------------------------------------
 */
IEC_UINT cmdRun(STaskInfoVMM *pVMM, IEC_UINT uSignal, XBlock *pBlock);
IEC_UINT cmdExecute(STaskInfoVMM *pVMM, XBlock *pBlock);
#if defined(DEBUG)
IEC_UINT cmdBeforeExecute(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdAfterExecute(STaskInfoVMM *pVMM, XBlock *pBlock);
#endif


/* vmmCom.c - Communication State Machine
 * ----------------------------------------------------------------------------
 */
IEC_UINT comBlock(STaskInfoVMM *pVMM);
IEC_UINT comClient(STaskInfoVMM *pVMM);

#if defined(RTS_CFG_TCP_NATIVE)
  IEC_UINT sockInitialize(STaskInfoVMM *pVMM);
  IEC_UINT sockListen(void *pPara);
  IEC_UINT sockComm(void *pPara);
#endif


/* vmmBreak.c - Breakpoint Handling
 * ----------------------------------------------------------------------------
 */
IEC_UINT bpInitialize(STaskInfoVMM *pVMM);
IEC_UINT bpAddBreakpoint(STaskInfoVMM *pVMM, SBreakpoint *pBP, IEC_UDINT *pBPId);
IEC_UINT bpDeleteBreakpoint(STaskInfoVMM *pVMM, IEC_UDINT ulBPId);
IEC_UINT bpDeleteAllBreakpoints(STaskInfoVMM *pVMM);
IEC_UINT bpValidateBP(STaskInfoVMM *pVMM, SBreakpoint *pBP);
IEC_UINT bpQueryBreakpoint(SIntShared *pShared, IEC_UINT uClassId, IEC_UINT uInstP, IEC_UINT uCodePos, SBPEntry **ppBP);
IEC_UINT bpIsBPInList(SIntShared *pShared, IEC_UINT uClassId, IEC_UINT uCodePos, SBPEntry **ppBP);


/* actDebug.c / actDFull.c / actDIncr.c / actDown.c / actFile.c / actEtc.c
 * ----------------------------------------------------------------------------
 */
typedef IEC_UINT (*ACTPROC_FUN)(STaskInfoVMM *pVMM, XBlock *pBlock);

IEC_UINT cmdGetState(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdLogin(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdLogout(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdStart(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdResource(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdAllTasks(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdTask(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdOpenDebugSession(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdCloseDebugSession(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdSetBreakpoint(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdClearBreakpoint(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdClearAllBreakpoints(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdContinue(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdGetValue(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdSetValue(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDownloadBegin(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDownloadConfig(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDownloadInitial(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDownloadCode(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDownloadCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDownloadFinish(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDownloadEnd(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDownloadIOL(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDownloadClear(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdInitialize(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdClearFlash(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDBIGet(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDownloadDebug(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdGetProjectVersion(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdGetProjectInfo(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdLoadProject(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdSaveProject(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdLoadFile(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdSaveFile(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDelFile(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdDir(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdOCBegin(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdOCConfig(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdOCCode(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdOCDebug(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdOCCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdOCInitial(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdOCCommit(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdOCEnd(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdOCFinish(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdFlash(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdGetConfig(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdRetWrite(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdRetCycle(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdGetInstKey(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdSetLicKey(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdGetSerialNo(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdGetFeature(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdGetType(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdGetVersion(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT cmdSetLicEx(STaskInfoVMM *pVMM, XBlock *pBlock);

IEC_UINT resGetState(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resLogin(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resLogout(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resStart(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resResource(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resAllTasks(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resTask(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resOpenDebugSession(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resCloseDebugSession(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resSetBreakpoint(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resClearBreakpoint(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resClearAllBreakpoints(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resContinue(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resGetValue(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resSetValue(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDownloadBegin(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDownloadConfig(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDownloadInitial(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDownloadCode(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDownloadCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDownloadFinish(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDownloadEnd(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDownloadIOL(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDownloadClear(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resInitialize(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resClearFlash(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDBIGetChildren(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDBIGetAddress(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDownloadDebug(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDBIGetTaskNr(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resGetProjectVersion(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resGetProjectInfo(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resLoadProject(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resSaveProject(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resLoadFile(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resSaveFile(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDelFile(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resDir(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resOCBegin(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resOCConfig(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resOCCode(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resOCDebug(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resOCCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resOCInitial(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resOCCommit(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resOCEnd(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resOCFinish(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resFlash(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resGetConfig(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resRetWrite(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resRetCycle(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resGetInstKey(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resSetLicKey(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resGetSerialNo(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resGetFeature(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resGetType(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resGetVersion(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT resSetLicEx(STaskInfoVMM *pVMM, XBlock *pBlock);

IEC_UINT actClearResource(STaskInfoVMM *pVMM, IEC_UINT *upRetry, IEC_BOOL bWarmStart);
IEC_UINT actClearCustomDL(STaskInfoVMM *pVMM);
IEC_UINT actOCCleanUp(STaskInfoVMM *pVMM);

IEC_UINT actDLBegin (STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uMode);
IEC_UINT actDLConfig(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uMode, IEC_UINT uDomain);
IEC_UINT actDLCode	(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uMode, IEC_UINT uDomain, IEC_UINT uCode, IEC_INT iIncr);
IEC_UINT actDLInit	(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uMode, IEC_UINT uDomain, IEC_UINT uData, IEC_UINT uDirect, IEC_INT iIncr);
IEC_UINT actDLFile	(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uMode, IEC_UINT uDomain, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szMap);
IEC_UINT actDLFinish(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uMode, IEC_UINT uDomain);

IEC_UINT actCloseFile(IEC_UDINT *pFile);

IEC_UINT actCmdSaveFile(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uCmd);
IEC_UINT actResSaveFile(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uCmd);

IEC_UINT actClearDownload(STaskInfoVMM *pVMM, IEC_BYTE byCommand);


/* vmmAct.c - Action Processing
 * ----------------------------------------------------------------------------
 */
IEC_UINT actGetOCConfig 		(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, 			   SOnlChg		*pOC,		SProject const *pProject);
IEC_UINT actGetOCList			(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SOCCopy		*pOCC);
IEC_UINT actGetProjectInfo		(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SProject *pProject,	IEC_UDINT	*ulpBinDLVersion);
IEC_UINT actGetExtProjectInfo	(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SProjInfo	*pProjInfo);
IEC_UINT actGetRetainList		(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SRetainReg	*pRegion);
IEC_UINT actGetSegments 		(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, 			   SObject		*pData, 	SSegInfo	*pSegInfo);
IEC_UINT actGetIOLayer			(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SIOLayer 	*pIOLayer);
IEC_UINT actGetPrograms 		(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, SIndex		*pProg);
IEC_UINT actGetImageRegions 	(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uRd,	IEC_UINT	uWr,		SRegion *pRd, SRegion *pWr);
IEC_UINT actGetTask 			(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uObj, STaskInfoVM **ppVM,		STask		*pTSK, IEC_UINT uTasks);
IEC_UINT actGetFileData 		(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UDINT hFile);
IEC_UINT actGetDownHeader		(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, SDownHeader *pHeader);
IEC_UINT actGetDownDirect		(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, SDownDirect *pDirect);
IEC_UINT actGetFileDef			(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, SFileDef *pFileDef);
IEC_UINT actGetFileName 		(SDLBuffer *pDL, XBlock *pBlock, IEC_UINT *upCount, IEC_UINT uSize, IEC_CHAR *szFile);


/* vmmUtil.c - Common Utility Functions
 * ----------------------------------------------------------------------------
 * Usable from every task within the RTS.
 */
IEC_INT  utilIECStrCmp(IEC_STRING OS_LPTR *pStr1, IEC_STRING OS_LPTR *pStr2);
IEC_UINT utilIECStrToLong(IEC_STRING OS_LPTR *inStr, IEC_UINT bUnsigned, IEC_CHAR *szBuffer, IEC_DINT *lpValue);

IEC_INT  utilStrnicmp(IEC_CHAR OS_LPTR *pStr1, IEC_CHAR OS_LPTR *pStr2, IEC_UDINT uLen);
IEC_INT  utilStricmp(IEC_CHAR OS_LPTR *pStr1, IEC_CHAR OS_LPTR *pStr2);

IEC_CHAR *utilFormatString(IEC_CHAR *szBuffer, IEC_CHAR *szFormat, ...);
IEC_UINT utilFormatIECString(IEC_STRING OS_LPTR *strIEC, IEC_CHAR *szBuffer, IEC_CHAR *szFormat, ...);
IEC_CHAR *utilIecToAnsi(IEC_STRING OS_LPTR *strIEC, IEC_CHAR *szAnsi);
IEC_UINT utilAnsiToIec(IEC_CHAR *szAnsi, IEC_STRING OS_LPTR *strIEC);
IEC_UINT utilCopyIecString(IEC_STRING OS_LPTR *strDest, IEC_STRING OS_LPTR *strSrc);
IEC_UINT utilTruncCRLF(IEC_CHAR *szString);

IEC_DINT utilRealToDint (IEC_REAL  fValue, IEC_DINT lCast, IEC_DINT lMin, IEC_DINT lMax);
IEC_DINT utilLrealToDint(IEC_LREAL fValue, IEC_DINT lCast, IEC_DINT lMin, IEC_DINT lMax);

IEC_UINT utilAppendPath(IEC_CHAR *szDir, IEC_UINT uSize, IEC_CHAR *szSub, IEC_BOOL bDir);
IEC_BOOL utilIs_8_3(IEC_CHAR *szPath);

IEC_BOOL utilCheckString(IEC_CHAR *szString, IEC_UINT uMax);

#if defined(RTS_CFG_FILE_ACCESS) || defined(RTS_CFG_FILE_NATIVE)
IEC_UINT utilCreateDir (IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szFile);
IEC_UINT utilCreateFile(IEC_UDINT *hpFile, IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szFile);
IEC_UINT utilOpenFile(IEC_UDINT *hpFile, IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szFile, IEC_UINT uMode);
IEC_UINT utilExistFile (IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szFile, IEC_BOOL *bpExist);
IEC_UINT utilDeleteFile(IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szFile);
IEC_UINT utilRenameFile(IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szFrom, IEC_CHAR *szTo);
IEC_UINT utilCreatePath(IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szFile);
#endif

#if defined(RTS_CFG_STORE_FILES)
IEC_UINT utilDeleteList(IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szMap, IEC_CHAR *szFile);
IEC_UINT utilCreateList(IEC_CHAR *pBuffer, IEC_UINT uLen, IEC_UINT (* fpGetDir) (IEC_CHAR *, IEC_UINT), IEC_CHAR *szDir, IEC_CHAR *szMap, IEC_CHAR *szFile);
#endif

IEC_UINT utilPow2(IEC_UINT n);

IEC_UDINT utilGetTimeDiffMS(IEC_ULINT ullStart);
IEC_ULINT utilGetTimeDiffUS(IEC_ULINT ullStart);

IEC_UDINT utilGetTimeDiffMSEx(IEC_ULINT ullStart);
IEC_ULINT utilGetTimeDiffUSEx(IEC_ULINT ullStart);

 
/* vmmMsg.c - Message Handling
 * ----------------------------------------------------------------------------
 */
IEC_UINT msgSend(SMessage *pMsg, IEC_UINT uQueue);
IEC_UINT msgRecv(SMessage *pMsg, IEC_UINT uQueue, IEC_UDINT ulTimeOut);
IEC_UINT msgSendCommand(IEC_UINT uID, IEC_UINT uQueue, IEC_UINT uRespQueue);
IEC_UINT msgTXCommand(IEC_UINT uID, IEC_UINT uQueue, IEC_UINT uRespQueue, IEC_UDINT ulTimeOut, IEC_BOOL bSend);
IEC_UINT msgTXMessage(SMessage *pMsg, IEC_UINT uQueue, IEC_UDINT ulTimeOut, IEC_BOOL bSend);
IEC_UINT msgTransfer(SMessage *pMsg, IEC_UINT uQueue, IEC_UDINT ulTimeOut, IEC_BOOL bSend);
IEC_UINT msgSetError(SMessage *pMessage, IEC_UINT uError);


/* vmmOnlCh.c - Online Change
 * ----------------------------------------------------------------------------
 */
IEC_UINT ocMain(void *pPara);
IEC_UINT ocApply(STaskInfoVMM *pVMM);


/* vmmRet.c - Retentive memory handling
 * ----------------------------------------------------------------------------
 */
IEC_UINT retMain(void *pPara);


/* vmmFirm.c - Firmware update handling
 * ----------------------------------------------------------------------------
 */
IEC_UINT fwMain(void *pPara);


/* vmMain.c - VM functions
 * ----------------------------------------------------------------------------
 */
IEC_UINT vmMain(STaskInfoVM *pVM);

STaskInfoVM *vmCreateTaskInfoVM(IEC_UINT uTask, IEC_UINT uTasks);
IEC_UINT vmFreeTaskInfoVM(STaskInfoVM *pVM);

IEC_UINT vmSetException(STaskInfoVM *pVM, IEC_UDINT ulErrNo);

IEC_UINT vmInitializeLocalSegments(STaskInfoVM *pVM);
#if defined (RTS_CFG_TASK_IMAGE) | defined(RTS_CFG_WRITE_FLAGS)
  IEC_UINT vmFinalizeLocalSegments(STaskInfoVM *pVM);
#endif

#if defined(RTS_CFG_COPY_DOMAIN)
  IEC_BOOL vmmFindVarInCpyList(STaskInfoVMM  *pVMM, XVariable *pxVar, IEC_UDINT *pulOffset);
#endif


/* vmPrc.c - Task and Process Image
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_TASK_IMAGE) || defined(RTS_CFG_WRITE_FLAGS) || defined(RTS_CFG_IO_LAYER)

IEC_UINT prcGetTaskImage(STaskInfoVM *pVM);
IEC_UINT prcSetTaskImage(STaskInfoVM *pVM);

#endif


/* vmmLoad.c - System Load Evaluation
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_SYSLOAD)

IEC_UINT ldInitTaskInfo(void);
IEC_UINT ldWriteTaskInfo(IEC_UINT uTask, IEC_UDINT ulID);
IEC_UINT ldClearTaskInfo(IEC_UINT uTask);
IEC_UINT ldFindTask(IEC_UDINT ulID, IEC_UINT *upTask);
IEC_UINT ldGetTaskName(IEC_UINT uTask, IEC_CHAR *szTask);
IEC_UINT ldHandleSysLibCall(IEC_WORD wLoadType, IEC_UINT uTaskID, IEC_STRING *pLoad);

#endif


/* dbiMain.c - Debug Information Handling
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_DEBUG_INFO)

IEC_UINT dbiInitialize(STaskInfoVMM *pVMM);
IEC_UINT dbiFinalize(STaskInfoVMM *pVMM);
IEC_UINT dbiGetChildren(STaskInfoVMM *pVMM, SDBIInstance *pInst, IEC_DATA *pParent, IEC_UINT uParent, XBlock *pBlock, IEC_UINT *upProc);
IEC_UINT dbiGetAddress(STaskInfoVMM *pVMM, SDBIInstance *pInst, IEC_DATA *pParent, IEC_UINT uParent, XBlock *pBlock);
IEC_UINT dbiGetTaskNr(STaskInfoVMM *pVMM, IEC_DATA *pTask, XBlock *pBlock);

#endif


/* fileMain.c - File Handling
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_FILE_NATIVE)

IEC_UINT fileInitialize(STaskInfoVMM *pVMM);
IEC_UINT fileOpen(IEC_UDINT *hpFile, IEC_CHAR *szName, IEC_UINT uMode, IEC_BOOL bText);
IEC_UINT fileClose(IEC_UDINT hFile);
IEC_UINT fileSeek(IEC_UDINT hFile, IEC_UDINT ulOffset, IEC_INT iOrigin);
IEC_UINT fileRead(IEC_UDINT hFile, IEC_DATA *pData, IEC_UINT *upLen);
IEC_UINT fileReadLine(IEC_UDINT hFile, IEC_CHAR *szData, IEC_UINT uLen);
IEC_UINT fileWrite(IEC_UDINT hFile, IEC_DATA *pData, IEC_UINT uLen);
IEC_UINT fileWriteLine(IEC_UDINT hFile, IEC_CHAR *szData);
IEC_UINT fileCreateDir(IEC_CHAR *szPath, IEC_BOOL bDirOnly, IEC_UINT uMode);
IEC_UINT fileRename(IEC_CHAR *szFrom, IEC_CHAR *szTo);
IEC_UINT fileRemove(IEC_CHAR *szFile);
IEC_UINT fileExist(IEC_CHAR *szFile, IEC_BOOL *bpExist);

#endif


/* libDef.c - Commom library functions
 * ----------------------------------------------------------------------------
 */
IEC_UINT libGetFBCount(void);
IEC_UINT libGetFunCount(void);

IEC_UINT libSetError(STaskInfoVM *pVM, IEC_UINT uError);
IEC_UINT libSetException(STaskInfoVM *pVM, IEC_UINT uError);

IEC_UINT libGetTaskNo(STaskInfoVM *pVM, IEC_STRING OS_DPTR *strTask, IEC_UINT *upTask);

IEC_UDINT libSubstring(STaskInfoVM *pVM, IEC_STRING OS_DPTR *in, IEC_STRING OS_DPTR *out, 
					   IEC_UDINT startPos, IEC_UDINT endPos, IEC_BOOL bAppend);


/* ioMain.c
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_IO_LAYER)

IEC_UINT ioCreateLayer(STaskInfoVMM *pVMM);
IEC_UINT ioTerminateLayer(STaskInfoVMM *pVMM, IEC_BOOL bWarmStart);
IEC_UINT ioVerifyTermination(STaskInfoVMM *pVMM, IEC_UDINT ulClearTime);
IEC_UINT ioDestroyLayer(STaskInfoVMM *pVMM);
IEC_UINT ioConfigLayer(STaskInfoVMM *pVMM);
IEC_UINT ioStartLayer(STaskInfoVMM *pVMM, IEC_BOOL bStart, IEC_BOOL bAlways);
IEC_UINT ioIsBootable(STaskInfoVMM *pVMM, IEC_BOOL *bpBoot);
IEC_UINT ioNotifyLayer(STaskInfoVMM *pXXX, STaskInfoVM *pVM, IEC_BOOL bSet, IEC_UINT uSegment, IEC_UDINT ulOffset, IEC_UINT uLen, IEC_USINT usBit);
IEC_UINT ioOptimizeLayer(STaskInfoVMM *pVMM);
IEC_UINT ioMain(void *pPara);

#endif /* RTS_CFG_IO_LAYER */


/* vmTimer.c
 * ----------------------------------------------------------------------------
 */
IEC_UINT timMain(void *pPara);

IEC_UINT timEnableAllWDog(STaskInfoVMM *pVMM, IEC_BOOL bEnable);
IEC_UINT timRestoreAllWDog(STaskInfoVMM *pVMM);


/* vmmSys.c
 * ----------------------------------------------------------------------------
 */
IEC_UINT sysInitialize(void);
IEC_UINT sysEnableIOLayer(IEC_CHAR *szName);
IEC_UINT sysGetFeatureAvail(IEC_UINT *upFeature);
IEC_UINT sysGetFeatureLic(IEC_UINT *upFeature);
IEC_UINT sysGetVersion(IEC_CHAR *szVersion);
IEC_UINT sysGetType(IEC_CHAR *szType);
IEC_UINT sysGetVersionInfo(IEC_CHAR *szVersion);
IEC_UINT sysGetCommPort(IEC_UINT *upPort);
IEC_BOOL sysIsAvailable(IEC_UINT uFeature);
IEC_UINT sysGetMacAddr(IEC_CHAR pMacAddr[6]);
IEC_UINT sysGetSerialNo(IEC_UDINT *ulpSerialNo);
IEC_UINT sysGetInstKey(IEC_CHAR *szInstKey);
IEC_UINT sysSetLicKey(IEC_CHAR *szLicKey);


/* osMain.c - Common Adaptations
 * ----------------------------------------------------------------------------
 */
IEC_UINT osMain(IEC_UINT argc, IEC_CHAR *argv[]);

IEC_UINT osInitializeVM(STaskInfoVM *pVM);

IEC_UINT osOnCmdReceived(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT osOnCmdHandled(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uResult);

IEC_UINT osOnCmdComputed(STaskInfoVMM *pVMM, XBlock *pBlock);

#if defined(RTS_CFG_PROT_BLOCK)
IEC_UINT osOnRespReceived(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT osOnRespHandled(STaskInfoVMM *pVMM, XBlock *pBlock, IEC_UINT uResult);
#endif

#if defined(RTS_CFG_CUSTOM_OC)
IEC_UINT osCmdCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT osResCustom(STaskInfoVMM *pVMM, XBlock *pBlock);
#endif

#if defined(RTS_CFG_TASK_IMAGE)
IEC_UINT osBeginGetTaskImage(STaskInfoVM *pVM);
IEC_UINT osEndGetTaskImage(STaskInfoVM *pVM);
IEC_UINT osBeginSetTaskImage(STaskInfoVM *pVM);
IEC_UINT osEndSetTaskImage(STaskInfoVM *pVM);
#endif

IEC_UINT osInitializeSegment(IEC_UINT uSegment, SObject *pSegment);

#if defined(RTS_CFG_COPY_DOMAIN)
IEC_UINT vmmCopyRegions(SIntShared *pShared, IEC_UINT uStart, IEC_UINT uCount, IEC_BOOL bReverse);	
#endif

IEC_CHAR *osCreateMsgStr(IEC_UINT uMessage);
IEC_UINT osFreeMsgStr(IEC_UINT uMessage);

#if defined (RTS_CFG_TASK_IMAGE)
IEC_DATA OS_DPTR *osCreateLocSegment(IEC_UINT uTask, IEC_UINT uSegment, IEC_UDINT ulSize);
IEC_UINT osFreeLocSegment(IEC_UINT uTask, IEC_UINT uSegment, IEC_DATA OS_DPTR **ppSegment);
IEC_DATA OS_DPTR *osCreateWFSegment(IEC_UINT uTask, IEC_UDINT ulSize);
IEC_UINT osFreeWFSegment(IEC_UINT uTask, IEC_DATA OS_DPTR **ppSegment);
#endif

IEC_UINT osHandleException(STaskInfoVM *pVM, SException *pException);

IEC_UINT osNotifyGetValue(STaskInfoVMM *pVMM, IEC_DATA *pVal, IEC_UINT uSegment, IEC_UDINT ulOffset, IEC_UINT uLen, IEC_USINT usBit);
IEC_UINT osNotifySetValue(STaskInfoVMM *pVMM, IEC_DATA *pVal, IEC_UINT uSegment, IEC_UDINT ulOffset, IEC_UINT uLen, IEC_USINT usBit);

IEC_UINT osExecNativeCode(SIntShared *pShared, SIntLocal  *pLocal, void *pIN, void *pSP, void *pIP);

IEC_UINT osGetProjectDir(IEC_CHAR *szDir, IEC_UINT uSize);
IEC_UINT osGetFileDir(IEC_CHAR *szDir, IEC_UINT uSize);
IEC_UINT osGetDBIDir(IEC_CHAR *szDir, IEC_UINT uSize);
IEC_UINT osGetCustDownDir(IEC_CHAR *szDir, IEC_UINT uSize);
IEC_UINT osGetFlashDir(IEC_CHAR *szDir, IEC_UINT uSize);
IEC_UINT osGetTraceDir(IEC_CHAR *szDir, IEC_UINT uSize);
IEC_UINT osGetRetainDir(IEC_CHAR *szDir, IEC_UINT uSize);
IEC_UINT osGetFWDir(IEC_CHAR *szDir, IEC_UINT uSize);
IEC_UINT osGetLoadDir(IEC_CHAR *szDir, IEC_UINT uSize);
IEC_UINT osGetBACnetDBDir(IEC_CHAR *szDir, IEC_UINT uSize);
IEC_UINT osGetBACnetFlashDir(IEC_CHAR *szDir, IEC_UINT uSize);

IEC_UINT osCheckFilePath(IEC_CHAR *szPath, IEC_UINT uCmd);
IEC_UINT osGetFreeDiskSpace(IEC_CHAR *szPath, IEC_UDINT *ulpFree);
IEC_UINT osTriggerSystemWatchdog(void);
IEC_UINT osReboot(void);

IEC_UINT osFWPrepareUpdate(XFWDownload *pxFW);
IEC_UINT osFWExecuteUpdate(void);

IEC_UINT osCmdSetLicEx(STaskInfoVMM *pVMM, XBlock *pBlock);
IEC_UINT osResSetLicEx(STaskInfoVMM *pVMM, XBlock *pBlock);


/* osComm.c - Communication
 * ----------------------------------------------------------------------------
 */
#if ! defined(RTS_CFG_TCP_NATIVE)

IEC_UINT osOpenCom(STaskInfoVMM *pVMM);
IEC_UINT osReadBlock(STaskInfoVMM *pVMM, IEC_DATA *pData, IEC_UINT uLen);
IEC_UINT osWriteBlock(STaskInfoVMM *pVMM, IEC_DATA *pData, IEC_UINT uLen);
IEC_UINT osCheckIO(STaskInfoVMM *pVMM);

#endif


/* osTask.c - Task Handling
 * ----------------------------------------------------------------------------
 */
STaskInfoVMM *get_pVMM(void); // FIXME
IEC_UINT	 osInitialize(void);
IEC_UINT	 osInitializeVMM(STaskInfoVMM *pVMM);

STaskInfoVMM *osCreateTaskInfoVMM(void);
SObject 	*osCreateCodeList(void);

STaskInfoVM *osCreateTaskInfoVM(IEC_UINT uTask);
IEC_UINT	osFreeTaskInfoVM(STaskInfoVM *pVM);

IEC_UINT	osCreateVMMembers(STaskInfoVM *pVM);
IEC_UINT	osFreeVMMembers(STaskInfoVM *pVM);

SImageReg	*osCreateImageReg(STaskInfoVM *pVM, IEC_UINT uTasks);
IEC_UINT	osFreeImageReg(STaskInfoVM *pVM);

IEC_USINT osConvertVMPriority(IEC_USINT usPriority);
IEC_USINT osConvert4CPriority(IEC_USINT usPriority);

#if defined(RTS_CFG_TCP_NATIVE)
  IEC_UINT osCreateListenTask(SComTCP *pTCP);
#endif

IEC_UINT osCreateVMTask(STaskInfoVM *pVM);
IEC_UINT osKillVMTask(STaskInfoVM *pVM);
IEC_UINT osOnVMTerminate(STaskInfoVM *pVM);

#if defined(RTS_CFG_TCP_NATIVE) && defined(RTS_CFG_MULTILINK)
IEC_UINT osCreateCommTask(SComTCP *pTCP);
#endif

#if defined(RTS_CFG_VM_IPC) && defined(RTS_CFG_ONLINE_CHANGE)
IEC_UINT osCreateOnlineChangeTask(STaskInfoVMM *pVMM);
#endif

#if defined(RTS_CFG_VM_IPC)
IEC_UINT osCreateVMTimerTask(STaskInfoVMM *pVMM);
#endif

#if defined(RTS_CFG_EXT_RETAIN)
IEC_UINT osCreateRetainTask(STaskInfoVMM *pVMM);
#endif

#if defined(RTS_CFG_BACNET)
IEC_UINT osCreateDeviceTask(void);
IEC_UINT osCreateCOVTask(void);
IEC_UINT osCreateScanTask(void);
IEC_UINT osCreateFlashTask(void);
IEC_UINT osCreateConfigTask(SIOLayerIniVal *pIni);
#endif

#if defined(RTS_CFG_PROFI_DP)
IEC_UINT osCreatePBManagementTask(IEC_UINT uIOLayer);
#endif

#if defined(RTS_CFG_MULTILINK)
IEC_UINT osOnCommTerminate(STaskInfoVMM *pVMM, IEC_UINT uTask);
#endif

IEC_UINT osCreateSemaphore(IEC_UINT uSemaphore);
IEC_UINT osDeleteSemaphore(IEC_UINT uSemaphore);
IEC_UINT osBeginCriticalSection(IEC_UINT uSemaphore);
IEC_UINT osEndCriticalSection(IEC_UINT uSemaphore);

IEC_UINT *osCreateVMEvents(IEC_UINT uEvent);
IEC_UINT osFreeVMEvents(IEC_UINT uEvent);

IEC_UINT osOnClearBegin(STaskInfoVMM *pVMM);
IEC_UINT osOnClearAfterDelVM(STaskInfoVMM *pVMM);
IEC_UINT osOnClearEnd(STaskInfoVMM *pVMM);

#if defined(RTS_CFG_SYSLOAD)

IEC_UDINT osGetTaskID(void);

#endif


/* osFieldB.c - Field Bus Integration
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_IO_LAYER)

IEC_UINT osInitIOLayer(STaskInfoVMM *pVMM);
IEC_UINT osEnableIOLayer(STaskInfoVMM *pVMM, IEC_UINT uIOLayer);
IEC_UINT osCreateIOLayer(STaskInfoVMM *pVMM, IEC_UINT uIOLayer);
IEC_UINT osKillIOLayer(STaskInfoVMM *pVMM, IEC_UINT uIOLayer);

#endif /* RTS_CFG_IO_LAYER */


/* osFlash.c - Flash
 * ----------------------------------------------------------------------------
 */

#if defined(RTS_CFG_FLASH)

IEC_UINT osClearFlash(IEC_UINT *upRetry);
IEC_UINT osFWInit(void);
IEC_UINT osFWFinish(void);
IEC_UINT osFlashWrite(IEC_DATA *pData, IEC_UINT uLen);
IEC_UINT osFRInit(void);
IEC_UINT osFRFinish(void);
IEC_UINT osFRInitDomain(IEC_UINT uDomain, IEC_UDINT *ulpLen);
IEC_UINT osFRFinishDomain(IEC_UINT uDomain);
IEC_UINT osFlashRead(IEC_UINT uDomain, IEC_UINT uBlock, IEC_DATA *pData, IEC_UINT uLen);

#endif	/* RTS_CFG_FLASH */


/* osLib.c - Customer Library Implementation
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_CUSTOMER_LIB)

IEC_UINT osGetFunCount(void);
IEC_UINT osGetFBCount(void);

#endif


/* osFile.c - File access
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_FILE_ACCESS)

IEC_UINT osFileInitialize(STaskInfoVMM *pVMM);
IEC_UINT osOpen(IEC_UDINT *hpFile, IEC_CHAR *szName, IEC_UINT uMode, IEC_BOOL bText);
IEC_UINT osClose(IEC_UDINT hFile);
IEC_UINT osSeek(IEC_UDINT hFile, IEC_UDINT ulOffset, IEC_INT iSet);
IEC_UINT osRead(IEC_UDINT hFile, IEC_DATA *pData, IEC_UINT *upLen);
IEC_UINT osReadLine(IEC_UDINT hFile, IEC_CHAR *szData, IEC_UINT uLen);
IEC_UINT osWrite(IEC_UDINT hFile, IEC_DATA *pData, IEC_UINT uLen);
IEC_UINT osWriteLine(IEC_UDINT hFile, IEC_CHAR *szLine);
IEC_UINT osCreateDir(IEC_CHAR *szPath, IEC_BOOL bDirOnly, IEC_UINT uMode);
IEC_UINT osRename(IEC_CHAR *szFrom, IEC_CHAR *szTo);
IEC_UINT osRemove(IEC_CHAR *szFile);
IEC_UINT osExist(IEC_CHAR *szFile, IEC_BOOL *bpExist);

#endif	/* RTS_CFG_FILE_ACCESS */


/* osUtil.c - Utility Functions
 * ----------------------------------------------------------------------------
 */
int osPthreadCreate(pthread_t *thread, /*const*/ pthread_attr_t *attr,
			void *(*start_routine) (void *), void *arg,
			const char *name, size_t stacksize);
int osPthreadSetSched(int policy, int sched_priority);
IEC_UINT osSleep(IEC_UDINT ulTime);
IEC_UINT osSleepAbsolute(IEC_UDINT ulTime);

IEC_UDINT osGetTime32(void);
IEC_ULINT osGetTime64(void);
IEC_ULINT osGetTimeUS(void);

IEC_UDINT osGetTime32Ex(void);
IEC_ULINT osGetTime64Ex(void);
IEC_ULINT osGetTimeUSEx(void);

#if defined(RTS_CFG_DEBUG_OUTPUT)
IEC_UINT osTrace(IEC_CHAR *szFormat, ...);
#endif

IEC_DATA OS_LPTR *osMalloc(IEC_UDINT ulSize);
IEC_UINT osFree(IEC_DATA OS_LPTR **pData);

#if defined(RTS_CFG_DEBUG_GPIO)
void xx_gpio_init();
void xx_gpio_set(unsigned n);
void xx_gpio_clr(unsigned n);
unsigned char xx_gpio_get(unsigned n);
void xx_gpio_close();
#endif

void *osMemCpy(void *dest, const void *src, size_t n) __attribute__ ((optimize("O0")));

/* osMsg.c - Messages and Queues
 * ----------------------------------------------------------------------------
 */
IEC_UINT osInitializeShared(void);

IEC_UINT osCreateIPCQueue(IEC_UINT uQueue);
IEC_UINT osDestroyIPCQueue(IEC_UINT uQueue);

IEC_UINT osRecvMessage(SMessage *pMessage, IEC_UINT uQueue, IEC_UDINT ulTimeOut);
IEC_UINT osSendMessage(SMessage *pMessage, IEC_UINT uQueue);
IEC_UINT osGetMessageCount(IEC_UINT uQueue, IEC_UINT *upMsgCount);


/* osLoad.c - System Load Evaluation
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_SYSLOAD)

IEC_UINT osGetLoadAvg(IEC_REAL *fp1, IEC_REAL *fp5, IEC_REAL *fp15, IEC_UDINT *ulpPReady, IEC_UDINT *ulpPSum);
IEC_UINT osStrLoadAvg(IEC_CHAR *szBuff, IEC_UDINT uLen);

IEC_UINT osGetMemInfo(IEC_UDINT *ulpTotal, IEC_UDINT *ulpUsed, IEC_UDINT *ulpFree);
IEC_UINT osStrMemInfo(IEC_CHAR *szBuff, IEC_UDINT uLen, IEC_UDINT *ulpMemTotal);

IEC_UINT osGetStat(SProcTime *pPT);
IEC_UINT osStrStat(IEC_CHAR *szBuff, IEC_UDINT uLen);

IEC_UINT osGetStatDiff(SProcTime *pPT, SProcTime *pPT_Diff);
IEC_UINT osStrStatDiff(IEC_CHAR *szBuff, IEC_UDINT uLen, IEC_UDINT *ulpOveral, SProcTime *pPT);

IEC_UINT osGetTaskStat(IEC_UDINT ulID, STaskTime *pTT);
IEC_UINT osGetTaskStatDiff(IEC_UDINT ulID, STaskTime *pTT, STaskTime *pTT_Diff);
IEC_UINT osStrTaskStatDiff(IEC_UDINT ulID, IEC_CHAR *szBuff, IEC_UDINT uLen, IEC_UDINT ulOveral, STaskTime *pTT);

IEC_UINT osGetTaskStatM(IEC_UDINT ulID, IEC_UDINT *ulpSize, IEC_UDINT *ulpRes);
IEC_UINT osStrTaskStatM(IEC_UDINT ulID, IEC_CHAR *szBuff, IEC_UDINT uLen, IEC_UDINT ulOveral);

#endif


/* Interpreter
 * ----------------------------------------------------------------------------
 * Only to be called from within a VM task.
 */
IEC_UINT intRun(STaskInfoVM *pVM);
IEC_UINT intInitialize(STaskInfoVM *pVM);

IEC_UINT intExecute(STaskInfoVM *pVM);

IEC_UINT intSetException(STaskInfoVM *pVM, IEC_UINT uErrNo, IEC_DATA OS_CPTR *pIP, IEC_DATA OS_DPTR *pIN);
IEC_UINT intHalt(STaskInfoVM *pVM);


/* System Libraries
 * ----------------------------------------------------------------------------
 */
IEC_UINT flCleanUp(void);


/* IO Layer: Test
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_IOTEST)

IEC_UINT tstMain(void *pPara);

#endif


/* IO Layer: ProfiBus DP
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_PROFI_DP)

IEC_UINT dpMain(void *pPara);
IEC_UINT dpMgt(void *pPara);

#endif


/* IO Layer: BACnet
 * ----------------------------------------------------------------------------
 */
#if defined (RTS_CFG_BACNET)

IEC_UINT bacMain(void *pPara);

IEC_UINT devMain(void *pPara);
IEC_UINT covMain(void *pPara);
IEC_UINT scnMain(void *pPara);
IEC_UINT flhMain(void *pPara);
IEC_UINT cfgMain(void *pPara);

#endif


#ifdef __cplusplus
} 
#endif

#endif	/* _VMMMAIN_H_ */

/* ---------------------------------------------------------------------------- */
