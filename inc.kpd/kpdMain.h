
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
 * Filename: kpdMain.h
 */


#ifndef _KPDMAIN_H_
#define _KPDMAIN_H_

#include <sys/ioctl.h>
#include <linux/mxs-buzzer.h>

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif


/* kpdImpl.c
 * ----------------------------------------------------------------------------
 */
IEC_UINT kpdInitialize(IEC_UINT uIOLayer);
IEC_UINT kpdFinalize(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT kpdNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT kpdNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT kpdNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT kpdNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify);
IEC_UINT kpdNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify);

extern int Buzzerfd;

/* kpdMain.c
 * ----------------------------------------------------------------------------
 */

IEC_UINT kpdMain(void *pPara);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif /* _KPDMAIN_H_ */

/* ---------------------------------------------------------------------------- */
