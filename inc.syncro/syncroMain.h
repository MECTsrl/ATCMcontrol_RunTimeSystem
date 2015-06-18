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
 * Filename: syncroMain.h
 */


#ifndef _SYNCROMAIN_H_
#define _SYNCROMAIN_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif


/* syncroImpl.c
 * ----------------------------------------------------------------------------
 */
IEC_UINT syncroInitialize(IEC_UINT uIOLayer);
IEC_UINT syncroFinalize(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT syncroNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT syncroNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT syncroNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT syncroNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify);
IEC_UINT syncroNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify);

/* syncroMain.c
 * ----------------------------------------------------------------------------
 */
IEC_UINT syncroMain(void *pPara);

#endif /* _SYNCROMAIN_H_ */

/* ---------------------------------------------------------------------------- */
