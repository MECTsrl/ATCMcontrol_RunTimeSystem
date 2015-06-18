
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
 * Filename: tstMain.h
 */


#ifndef _TSTMAIN_H_
#define _TSTMAIN_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif


/* tstImpl.c
 * ----------------------------------------------------------------------------
 */
IEC_UINT tstInitialize(IEC_UINT uIOLayer);
IEC_UINT tstFinalize(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT tstNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT tstNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT tstNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT tstNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify);
IEC_UINT tstNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify);


/* tstMain.c
 * ----------------------------------------------------------------------------
 */
IEC_UINT tstInitBus(IEC_UINT uIOLayer);
IEC_UINT tstFinalBus(IEC_UINT uIOLayer);

IEC_UINT tstConfigBus(IEC_UINT uIOLayer, SIOConfig *pIO);

IEC_UINT tstLockRead(IEC_UINT uIOLayer, IEC_DATA OS_DPTR **ppIn, IEC_DATA OS_DPTR **ppOut);
IEC_UINT tstReleaseRead(IEC_UINT uIOLayer);

IEC_UINT tstLockWriteFirst(IEC_UINT uIOLayer, IEC_DATA OS_DPTR **ppOut);
IEC_UINT tstLockWriteSecond(IEC_UINT uIOLayer, IEC_DATA OS_DPTR **ppOut);
IEC_UINT tstReleaseWrite(IEC_UINT uIOLayer);

IEC_UINT tstGetBusState(IEC_UINT uIOLayer);

#endif /* _TSTMAIN_H_ */

/* ---------------------------------------------------------------------------- */
