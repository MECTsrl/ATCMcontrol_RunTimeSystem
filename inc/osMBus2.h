
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
 * Filename: osMBus2.h
 */


#ifndef _OSMBUS2_H_
#define _OSMBUS2_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#define PORT_OK 				0
#define PORT_CANNOT_OPEN		1
#define PORT_NOT_SUPPORTED		2
#define PORT_NO_FREE_MEM		3
#define PORT_CANNOT_SET_PARAMS	4
#define PORT_NO_FREE_PORTS		5

/* Mutex start
 */
#define MBUS2_MUTEX_T 						pthread_mutex_t*
#define MUTEX_LOCK_OK						0

#define MBUS2_MUTEX_INIT(m) 									\
	do {														\
		m = (MBUS2_MUTEX_T)osMalloc(sizeof(pthread_mutex_t));					\
		pthread_mutex_init(m, NULL);							\
	}while(0)
#define MBUS2_MUTEX_DESTROY(m)									\
	do {														\
		pthread_mutex_destroy(m);								\
		osFree((IEC_DATA **)&(m));												\
	}while(0)

#define MBUS2_MUTEX_TRYLOCK(m)				pthread_mutex_trylock(m)
#define MBUS2_MUTEX_UNLOCK(m)				pthread_mutex_unlock(m)
/* Mutex end
 */

IEC_DINT mbOpenPort(const IEC_STRING_IMPL *strPort, const long baud);
IEC_DINT mbClosePort(const IEC_STRING_IMPL *strPort );
IEC_DINT mbSendTelegram(const IEC_STRING_IMPL *strPort, IEC_BYTE* bypInBuff, size_t len);
IEC_UINT mbReceiveTelegram(const IEC_STRING_IMPL *strPort, IEC_STRMAX* sBuff, size_t out_count);

#endif /* _OSMBUS2_H_ */

/* ---------------------------------------------------------------------------- */
