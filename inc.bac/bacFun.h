
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
 * Filename: bacFun.h
 */


#ifndef _BACFUN_H_
#define _BACFUN_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if defined(RTS_CFG_BACNET)

#ifdef __cplusplus
extern "C" { 
#endif

void BAC_FindObject(STDLIBFUNCALL);
void BAC_GetDevState(STDLIBFUNCALL);
void BAC_GetObjState(STDLIBFUNCALL);
void BAC_GetObjPrio(STDLIBFUNCALL);
void BAC_SetObjPrio(STDLIBFUNCALL);
void BAC_GetLocDeviceID(STDLIBFUNCALL);
void BAC_GP_Boolean(STDLIBFUNCALL);
void BAC_GP_Enumerated(STDLIBFUNCALL);
void BAC_GP_CharacterString(STDLIBFUNCALL);
void BAC_GP_Unsigned(STDLIBFUNCALL);
void BAC_GP_Real(STDLIBFUNCALL);
void BAC_GP_Bitstring(STDLIBFUNCALL);
void BAC_GP_DateTime(STDLIBFUNCALL);
void BAC_GP_StatusFlags(STDLIBFUNCALL);
void BAC_GP_LimitEnable(STDLIBFUNCALL);
void BAC_GP_EventTransition(STDLIBFUNCALL);
void BAC_GP_Any(STDLIBFUNCALL);
void BAC_GP_DateRange(STDLIBFUNCALL);
void BAC_GP_DevObjPropReference(STDLIBFUNCALL);
void BAC_GPA_Unsigned(STDLIBFUNCALL);
void BAC_GPA_Enumerated(STDLIBFUNCALL);
void BAC_GPA_Real(STDLIBFUNCALL);
void BAC_GPA_CharacterString(STDLIBFUNCALL);
void BAC_GPA_TimeStamp(STDLIBFUNCALL);
void BAC_GPA_DevObjPropReference(STDLIBFUNCALL);
void BAC_GPA_DailySchedule(STDLIBFUNCALL);
void BAC_GPA_SpecialEvent(STDLIBFUNCALL);
void BAC_GP_Ex_Boolean(STDLIBFUNCALL);
void BAC_GP_Ex_Enumerated(STDLIBFUNCALL);
void BAC_GP_Ex_CharacterString(STDLIBFUNCALL);
void BAC_GP_Ex_Unsigned(STDLIBFUNCALL);
void BAC_GP_Ex_Real(STDLIBFUNCALL);
void BAC_GP_Ex_Bitstring(STDLIBFUNCALL);
void BAC_GP_Ex_DateTime(STDLIBFUNCALL);
void BAC_GP_Ex_StatusFlags(STDLIBFUNCALL);
void BAC_GP_Ex_LimitEnable(STDLIBFUNCALL);
void BAC_GP_Ex_EventTransition(STDLIBFUNCALL);
void BAC_GP_Ex_Any(STDLIBFUNCALL);
void BAC_GP_Ex_DateRange(STDLIBFUNCALL);
void BAC_GP_Ex_DevObjPropReference(STDLIBFUNCALL);
void BAC_GPA_Ex_Unsigned(STDLIBFUNCALL);
void BAC_GPA_Ex_Enumerated(STDLIBFUNCALL);
void BAC_GPA_Ex_Real(STDLIBFUNCALL);
void BAC_GPA_Ex_CharacterString(STDLIBFUNCALL);
void BAC_GPA_Ex_TimeStamp(STDLIBFUNCALL);
void BAC_GPA_Ex_DevObjPropReference(STDLIBFUNCALL);
void BAC_GPA_Ex_DailySchedule(STDLIBFUNCALL);
void BAC_GPA_Ex_SpecialEvent(STDLIBFUNCALL);
void BAC_SP_Boolean(STDLIBFUNCALL);
void BAC_SP_Enumerated(STDLIBFUNCALL);
void BAC_SP_CharacterString(STDLIBFUNCALL);
void BAC_SP_Unsigned(STDLIBFUNCALL);
void BAC_SP_Real(STDLIBFUNCALL);
void BAC_SP_Bitstring(STDLIBFUNCALL);
void BAC_SP_DateTime(STDLIBFUNCALL);
void BAC_SP_LimitEnable(STDLIBFUNCALL);
void BAC_SP_EventTransition(STDLIBFUNCALL);
void BAC_SP_Any(STDLIBFUNCALL);
void BAC_SP_DateRange(STDLIBFUNCALL);
void BAC_SP_DevObjPropReference(STDLIBFUNCALL);
void BAC_SPA_Unsigned(STDLIBFUNCALL);
void BAC_SPA_CharacterString(STDLIBFUNCALL);
void BAC_SPA_DevObjPropReference(STDLIBFUNCALL);
void BAC_SPA_DailySchedule(STDLIBFUNCALL);
void BAC_SPA_SpecialEvent(STDLIBFUNCALL);
void BAC_SP_Ex_Boolean(STDLIBFUNCALL);
void BAC_SP_Ex_Enumerated(STDLIBFUNCALL);
void BAC_SP_Ex_CharacterString(STDLIBFUNCALL);
void BAC_SP_Ex_Unsigned(STDLIBFUNCALL);
void BAC_SP_Ex_Real(STDLIBFUNCALL);
void BAC_SP_Ex_Bitstring(STDLIBFUNCALL);
void BAC_SP_Ex_DateTime(STDLIBFUNCALL);
void BAC_SP_Ex_LimitEnable(STDLIBFUNCALL);
void BAC_SP_Ex_EventTransition(STDLIBFUNCALL);
void BAC_SP_Ex_Any(STDLIBFUNCALL);
void BAC_SP_Ex_DateRange(STDLIBFUNCALL);
void BAC_SP_Ex_DevObjPropReference(STDLIBFUNCALL);
void BAC_SPA_Ex_Unsigned(STDLIBFUNCALL);
void BAC_SPA_Ex_CharacterString(STDLIBFUNCALL);
void BAC_SPA_Ex_DevObjPropReference(STDLIBFUNCALL);
void BAC_SPA_Ex_DailySchedule(STDLIBFUNCALL);
void BAC_SPA_Ex_SpecialEvent(STDLIBFUNCALL);
void BAC_SubscribeProperty(STDLIBFUNCALL);
void BAC_UnSubscribeProperty(STDLIBFUNCALL);
void BAC_EventSubscribe(STDLIBFUNCALL);
void BAC_EventUnSubscribe(STDLIBFUNCALL);
void BAC_EventGet(STDLIBFUNCALL);
void BAC_EventGetString(STDLIBFUNCALL);
void BAC_EventAcknowledge(STDLIBFUNCALL);
void BAC_GetConfigRes(STDLIBFUNCALL);
void BAC_GetDevTrans(STDLIBFUNCALL);
void BAC_GetLogBuffer(STDLIBFUNCALL);
void BAC_ReinitializeDev(STDLIBFUNCALL);
void _BAC_ReinitializeDev(STDLIBFUNCALL);
void _BAC_DevCommControl(STDLIBFUNCALL);
void _BAC_ReadFile(STDLIBFUNCALL);
void _BAC_WriteFile(STDLIBFUNCALL);
void BAC_SPV_Enumerated(STDLIBFUNCALL);
void BAC_SPV_Unsigned(STDLIBFUNCALL);
void BAC_SPV_Real(STDLIBFUNCALL);
void BAC_SPV_Ex_Enumerated(STDLIBFUNCALL);
void BAC_SPV_Ex_Unsigned(STDLIBFUNCALL);
void BAC_SPV_Ex_Real(STDLIBFUNCALL);

void GP_Boolean(STDLIBFUNCALL, IEC_BOOL bSync);
void GP_Enumerated(STDLIBFUNCALL, IEC_BOOL bSync);
void GP_CharacterString(STDLIBFUNCALL, IEC_BOOL bSync);
void GP_Unsigned(STDLIBFUNCALL, IEC_BOOL bSync);
void GP_Real(STDLIBFUNCALL, IEC_BOOL bSync);
void GP_Bitstring(STDLIBFUNCALL, IEC_BOOL bSync);
void GP_DateTime(STDLIBFUNCALL, IEC_BOOL bSync);
void GP_StatusFlags(STDLIBFUNCALL, IEC_BOOL bSync);
void GP_LimitEnable(STDLIBFUNCALL, IEC_BOOL bSync);
void GP_EventTransition(STDLIBFUNCALL, IEC_BOOL bSync);
void GP_Any(STDLIBFUNCALL, IEC_BOOL bSync);
void GP_DateRange(STDLIBFUNCALL, IEC_BOOL bSync);
void GP_DevObjPropReference(STDLIBFUNCALL, IEC_BOOL bSync);
void GPA_Unsigned(STDLIBFUNCALL, IEC_BOOL bSync);
void GPA_Enumerated(STDLIBFUNCALL, IEC_BOOL bSync);
void GPA_Real(STDLIBFUNCALL, IEC_BOOL bSync);
void GPA_CharacterString(STDLIBFUNCALL, IEC_BOOL bSync);
void GPA_TimeStamp(STDLIBFUNCALL, IEC_BOOL bSync);
void GPA_DevObjPropReference(STDLIBFUNCALL, IEC_BOOL bSync);
void GPA_DailySchedule(STDLIBFUNCALL, IEC_BOOL bSync);
void GPA_SpecialEvent(STDLIBFUNCALL, IEC_BOOL bSync);

void SP_Boolean(STDLIBFUNCALL, IEC_BOOL bSync);
void SP_Enumerated(STDLIBFUNCALL, IEC_BOOL bSync);
void SP_CharacterString(STDLIBFUNCALL, IEC_BOOL bSync);
void SP_Unsigned(STDLIBFUNCALL, IEC_BOOL bSync);
void SP_Real(STDLIBFUNCALL, IEC_BOOL bSync);
void SP_Bitstring(STDLIBFUNCALL, IEC_BOOL bSync);
void SP_DateTime(STDLIBFUNCALL, IEC_BOOL bSync);
void SP_LimitEnable(STDLIBFUNCALL, IEC_BOOL bSync);
void SP_EventTransition(STDLIBFUNCALL, IEC_BOOL bSync);
void SP_Any(STDLIBFUNCALL, IEC_BOOL bSync);
void SP_DateRange(STDLIBFUNCALL, IEC_BOOL bSync);
void SP_DevObjPropReference(STDLIBFUNCALL, IEC_BOOL bSync);
void SPA_Unsigned(STDLIBFUNCALL, IEC_BOOL bSync);
void SPA_CharacterString(STDLIBFUNCALL, IEC_BOOL bSync);
void SPA_DevObjPropReference(STDLIBFUNCALL, IEC_BOOL bSync);
void SPA_DailySchedule(STDLIBFUNCALL, IEC_BOOL bSync);
void SPA_SpecialEvent(STDLIBFUNCALL, IEC_BOOL bSync);

void SPV_Enumerated(STDLIBFUNCALL, IEC_BOOL bSync);
void SPV_Unsigned(STDLIBFUNCALL, IEC_BOOL bSync);
void SPV_Real(STDLIBFUNCALL, IEC_BOOL bSync);

#ifdef __cplusplus
} 
#endif

#endif	/* RTS_CFG_BACNET */ 

#endif	/* _BACFUN_H_ */

/* ---------------------------------------------------------------------------- */
