
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
 * Filename: udpMain.h
 */


#ifndef _UDPMAIN_H_
#define _UDPMAIN_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif


/* udpImpl.c
 * ----------------------------------------------------------------------------
 */
int udpInitParameters(int udp_rx_port, int udp_tx_port);

IEC_UINT udpInitialize(IEC_UINT uIOLayer);
IEC_UINT udpFinalize(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT udpNotifyConfig(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT udpNotifyStart(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT udpNotifyStop(IEC_UINT uIOLayer, SIOConfig *pIO);
IEC_UINT udpNotifyGet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify);
IEC_UINT udpNotifySet(IEC_UINT uIOLayer, SIOConfig *pIO, SIONotify *pNotify);

/* udpMain.c
 * ----------------------------------------------------------------------------
 */
IEC_UINT udpMain(void *pPara);

#endif /* _UDPMAIN_H_ */

/* ---------------------------------------------------------------------------- */
