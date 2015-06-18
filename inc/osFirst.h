
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
 * Filename: osFirst.h
 */


#ifndef _OSFIRST_H_
#define _OSFIRST_H_

/* osFirst.h
 * ----------------------------------------------------------------------------
 * Adaptation header file. (This file is included before all other header 
 * files.)
 */

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif


/* Adaptation specific code ... 
 */

/* Customer Libraries
 * ----------------------------------------------------------------------------
 */
#undef	FC_CFG_UDPCOMM_LIB			/* UDP communication library			*/
#undef	FC_CFG_SERIALCOMM_LIB		/* Serial communication library 		*/
#undef	FC_CFG_TIME_LIB 			/* Time library 						*/

#if defined(_SOF_4CFC_SRC_) || defined(_SOF_4CDC_SRC_)
#define FC_CFG_MODBUS_RTU_LIB		/* Modbus RTU communication library 	*/
#endif

#if defined(_SOF_4CFC_SRC_)
#define FC_CFG_MBUS_LIB 			/* MBus communication library			*/
#define FC_CFG_PROFIDP_LIB			/* Profibus DP communication library	*/
#define FC_CFG_FC_SYSTEM_LIB		/* FC System library					*/
#define FC_CFG_BC_SYSTEM_LIB		/* BC System library					*/
#endif

#if defined(_SOF_4CDC_SRC_)
#define FC_CFG_DC_SYSTEM_LIB		/* DC System library					*/
#endif


#undef  FC_CFG_PB_TRACE_PBM			/* Profibus management tracing			*/
#undef	FC_CFG_PB_TRACE_PBW			/* Detailed Profibus activity tracing	*/

#endif /* _OSFIRST_H_ */

/* ---------------------------------------------------------------------------- */
