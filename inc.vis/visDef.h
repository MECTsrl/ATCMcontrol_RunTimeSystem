
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
 * Filename: visDef.h
 */


#ifndef _VISDEF_H_
#define _VISDEF_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif


/* Communication time out for the response from the remote FarosPLC
 * Run Time System.
 */
#define VIS_TCP_TIMEOUT 		6000


/* State variables of the FarosPLC VisuComm library.
 */
typedef struct 
{
	IEC_BOOL  bInitialized; 	/* Connected to remote RTS and		*/
								/* configuration received.			*/
	IEC_BOOL  bLogin;			/* Logged in to remote RTS and		*/
								/* IEC project ID received. 		*/

	/* The following values are retrieved from the remote RTS and	*/
	/* are only valid if the library is connected to an 4C RTS. 	*/

	IEC_BOOL  bBE_Control;		/* TRUE, if target is Big Endian	*/
	IEC_BOOL  bBE_This; 		/* TRUE, if I'm Big Endian			*/

	IEC_UDINT ulFirmware;		/* Firmware version 				*/
	IEC_UDINT ulMaxData;		/* Comm. Buffer size (MAX_DATA) 	*/
	IEC_UDINT ulPort;			/* TCP/IP Portnummer				*/

	IEC_DATA  pProjectID[VMM_GUID]; /* Current project GUID 		*/	
	
	IEC_DATA  *pCmdBuff;		/* Communication buffer 			*/

} SVisInfo; 					/* -------------------------------- */


#endif /* _VISDEF_H_ */

/* ---------------------------------------------------------------------------- */
