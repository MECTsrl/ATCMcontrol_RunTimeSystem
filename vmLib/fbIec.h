
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
 * Filename: fbIec.h
 */


#if defined(IP_CFG_BOOL)
									rs, 				/*0*/	/*RS_TYP*/
#else
									NULL, 				/*0*/
#endif
#if defined(IP_CFG_BOOL)
									sr, 				/*1*/	/*SR_TYP*/
#else
									NULL, 				/*1*/
#endif
#if defined(IP_CFG_BOOL) && defined(IP_CFG_TIME)
									tp, 				/*2*/	/*TP_TON_TOF_PAR*/
#else
									NULL, 				/*2*/
#endif
#if defined(IP_CFG_BOOL) && defined(IP_CFG_TIME)
									ton,				/*3*/	/*TP_TON_TOF_PAR*/
#else
									NULL, 				/*3*/
#endif
#if defined(IP_CFG_BOOL) && defined(IP_CFG_TIME)
									tof,				/*4*/	/*TP_TON_TOF_PAR*/
#else
									NULL, 				/*4*/
#endif
#if defined(IP_CFG_BYTE) && defined(IP_CFG_INT)
									ctu,				/*5*/	/*CTU_PAR*/
#else
									NULL, 				/*5*/
#endif
#if defined(IP_CFG_BYTE) && defined(IP_CFG_INT)
									ctd,				/*6*/	/*CTD_PAR*/
#else
									NULL, 				/*6*/
#endif
#if defined(IP_CFG_BYTE) && defined(IP_CFG_INT)
									ctud,				/*7*/	/*CTUD_PAR*/
#else
									NULL, 				/*7*/
#endif
#if defined(IP_CFG_BYTE)
									r_trig, 			/*8*/	/*TRIG_PAR*/
#else
									NULL, 				/*8*/
#endif
#if defined(IP_CFG_BYTE) && defined(IP_CFG_INT)
									f_trig, 			 /*9*/	 /*TRIG_PAR*/
#else
									NULL, 				 /*9*/
#endif
									NULL, 				/*10*/
									NULL, 				/*11*/
									NULL, 				/*12*/
									NULL, 				/*13*/
									NULL, 				/*14*/
									NULL, 				/*15*/
									NULL, 				/*16*/
									NULL, 				/*17*/
									NULL, 				/*18*/
									NULL, 				/*19*/

/* ---------------------------------------------------------------------------- */

