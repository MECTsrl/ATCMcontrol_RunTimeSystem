
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
 * Filename: canMain.h
 */


#ifndef _CANMAIN_H_
#define _CANMAIN_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif


/* canImpl.c
 * ----------------------------------------------------------------------------
 */
IEC_UINT canInitialize(IEC_UINT uIOLayer);
IEC_UINT canFinalize(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT canNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT canNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT canNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT canNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify);
IEC_UINT canNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify);

/* canMain.c
 * ----------------------------------------------------------------------------
 */

extern SIOConfig * CANOpenIO;
extern int Instance;

IEC_UINT canMain(void *pPara);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif /* _CANMAIN_H_ */

/* ---------------------------------------------------------------------------- */
