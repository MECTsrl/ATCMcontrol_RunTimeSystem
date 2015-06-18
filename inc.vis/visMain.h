
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
 * Filename: visMain.h
 */


#ifndef _VISMAIN_H_
#define _VISMAIN_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif

#ifdef __cplusplus
extern "C" { 
#endif


/* Internal Interfaces
 * ----------------------------------------------------------------------------
 */

/* visAdapt.c
 */
IEC_UINT domVisuCommInit(void);

/* visCmd.c
 */
IEC_UINT domGetConfig(SVisInfo *pVI, XConfig *xpConf);
IEC_UINT domGetProjVersion(SVisInfo *pVI, IEC_DATA **ppGUID);
IEC_UINT domLogin (SVisInfo *pVI, IEC_DATA *pProjectID);
IEC_UINT domLogout(SVisInfo *pVI);

IEC_UINT domGetChildren(SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease);
IEC_UINT domGetAddress (SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease);

IEC_UINT domGetValue   (SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease);
IEC_UINT domGetValue20 (SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease);
IEC_UINT domGetValue21 (SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease);

IEC_UINT domSetValue   (SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease);
IEC_UINT domSetValue20 (SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease);
IEC_UINT domSetValue21 (SVisInfo *pVI, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease);

IEC_UINT domGetResState (SVisInfo *pVI, IEC_UDINT *ulpState);
IEC_UINT domGetTaskState(SVisInfo *pVI, IEC_CHAR *szTask, IEC_UDINT *ulpState);
IEC_UINT domGetXState	(SVisInfo *pVI, IEC_CHAR *szTask, IEC_UDINT *ulpState);

IEC_UINT domRetainWrite   (SVisInfo *pVI);
IEC_UINT domRetainSetCycle(SVisInfo *pVI, IEC_UDINT ulCycle);

/* visLic.c
 */
IEC_UINT domGetInstKey		(SVisInfo *pVI, IEC_DATA  **ppKey,		IEC_UINT *upLen);
IEC_UINT domSetLicKey		(SVisInfo *pVI, IEC_DATA  *pKey,		IEC_UINT uLen);
IEC_UINT domGetSerialNo 	(SVisInfo *pVI, IEC_UDINT *ulpSN);
IEC_UINT domGetFeature		(SVisInfo *pVI, IEC_UINT  *upAvailable, IEC_UINT *upLicensed);
IEC_UINT domGetTargetType	(SVisInfo *pVI, IEC_DATA  **ppType, 	IEC_UINT *upLen);
IEC_UINT domGetTargetVersion(SVisInfo *pVI, IEC_DATA  **ppVersion,	IEC_UINT *upLen);
IEC_UINT domSetLicEx		(SVisInfo *pVI, IEC_DATA  *pKey,		IEC_UINT uLen);

/* visCom.c
 */
IEC_UINT domInitComm	(SVisInfo *pVI);
IEC_UINT domGetPort(IEC_UINT i);
IEC_UINT domOpenComm	(SVisInfo *pVI, IEC_CHAR const *szAddress, IEC_UINT uPort);
IEC_UINT domCloseComm	(SVisInfo *pVI);
IEC_UINT domSockSend	(SVisInfo *pVI, IEC_DATA *pData, IEC_UINT uLen);
IEC_UINT domSockRecv	(SVisInfo *pVI, IEC_DATA *pData, IEC_UINT *upLen);
IEC_UINT domTransferData(SVisInfo *pVI, IEC_BYTE byCmd, IEC_DATA **ppData, IEC_UINT *upLen, IEC_BOOL bRelease);

/* visUtil.c
 */
IEC_UINT  domSwap16(SVisInfo *pVI, IEC_UINT  uVal);
IEC_UDINT domSwap32(SVisInfo *pVI, IEC_UDINT ulVal);
IEC_ULINT domSwap64(SVisInfo *pVI, IEC_ULINT ullVal);

IEC_UINT  domSwapVar20(SVisInfo *pVI, IEC_DATA *pVar, IEC_UINT uVar, IEC_DATA *pVal, IEC_UINT uVal);
IEC_UINT  domSwapVar21(SVisInfo *pVI, IEC_DATA *pVar, IEC_UINT uVar, IEC_DATA *pVal, IEC_UINT uVal);

IEC_UINT  domSwapVal(SVisInfo *pVI, IEC_DATA *pVal, IEC_UINT uVal);

IEC_UINT  domConvertVarToVal(SVisInfo *pVI, IEC_DATA *pVar, IEC_UINT uVar, XVariable *pxDest);
IEC_UINT  domConvertDBIVarToVar(SVisInfo *pVI, IEC_DATA *pDBIVar, IEC_DATA **ppDest, IEC_UINT *upDest);

#ifdef __cplusplus
} 
#endif

#endif /* _VISMAIN_H_ */

/* ---------------------------------------------------------------------------- */
