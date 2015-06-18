
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
 * Filename: iolDef.h
 */


#ifndef _IOLDEF_H_
#define _IOLDEF_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif


/* Field bus states
 * ----------------------------------------------------------------------------
 */
#define FB_STATE_NONE			0x00u		/* FB state not initialized 	*/
#define FB_STATE_STARTED		0x01u		/* FB driver is started 		*/
#define FB_STATE_OPERATING		0x02u		/* FB is operating normally 	*/
#define FB_STATE_ERROR			0x03u		/* FB has an error				*/
#define FB_STATE_CONFIG 		0x04u		/* FB is configuring			*/
#define FB_STATE_INIT_IO		0x05u		/* FB is initializing IO objects*/

#define FB_STATE_NO_CONN		0x10u		/* FB is not connected to HW	*/


/* IOLayer tracing methods
 * ----------------------------------------------------------------------------
 */

/* Debug outputs for IO layer process image access by IEC/VM tasks.
 */
#if defined(RTS_CFG_IO_VM_TRACE)
	#define TR_IRDx 						"--- IRD: "
	#define TR_IWRx 						"+++ IWR: "
	#define TR_IRD7(s,p1,p2,p3,p4,p5,p6,p7) osTrace(TR_IRDx s "\r\n",p1,p2,p3,p4,p5,p6,p7)
	#define TR_IWR7(s,p1,p2,p3,p4,p5,p6,p7) osTrace(TR_IWRx s "\r\n",p1,p2,p3,p4,p5,p6,p7)
#else
	#define TR_IRDx
	#define TR_IWRx
	#define TR_IRD7(s,p1,p2,p3,p4,p5,p6,p7)
	#define TR_IWR7(s,p1,p2,p3,p4,p5,p6,p7)
#endif

/* Debug outputs for IO layer process image access by 4C Watch or other external
 * applications.
 */
#if defined(RTS_CFG_IO_WATCH_TRACE)
	#define TR_WRDx 						"--- WRD: "
	#define TR_WWRx 						"+++ WWR: "
	#define TR_WRD7(s,p1,p2,p3,p4,p5,p6,p7) osTrace(TR_WRDx s "\r\n",p1,p2,p3,p4,p5,p6,p7)
	#define TR_WWR7(s,p1,p2,p3,p4,p5,p6,p7) osTrace(TR_WWRx s "\r\n",p1,p2,p3,p4,p5,p6,p7)
#else
	#define TR_WRDx
	#define TR_WWRx
	#define TR_WRD7(s,p1,p2,p3,p4,p5,p6,p7)
	#define TR_WWR7(s,p1,p2,p3,p4,p5,p6,p7)
#endif


#endif /* _IOLDEF_H_ */

/* ---------------------------------------------------------------------------- */
