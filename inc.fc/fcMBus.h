
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
 * Filename: fcMBus.h
 */


#ifndef _FCMBUS_H_
#define _FCMBUS_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif

#if defined FC_CFG_MBUS_LIB

/* error codes */
#define TELEGRAM_OK				OK
#define TELEGRAM_BAD_LENGTH    	1
#define TELEGRAM_BAD_FORMAT    	2

#define NOERROR				 	OK
#define ERR_STATE_INVALID	 	4
#define ERR_TIMEOUT            	5
#define ERR_FCT_UNKNOWN			6

#define NO_ANSWER             	10
#define MORE_DEVICES_ANSWERED	20
#define WRONG_METER_TYPE      	30
#define NON_STATUS_TELEGRAM   	40
#define NON_VARLEN_STATUS     	50
#define INVALID_SIGNATURE     	60
#define METER_STATUS_ERROR    	70
#define METER_POWER_LOW       	80
#define METER_PERMANENT_ERROR 	90
#define METER_TEMPORARY_ERROR 	100

#define COULD_NOT_OPEN			110
#define INVALID_DATA_LENGTH		120

#define READ_STATUS_AGAIN      	121
#define INCOMPLET_STRUCT       	122
#define COUNTER_ERROR          	123
#define CONTINUE_READ			124
#define MBUS_ERR_NO_LICENSE		125


/** DECLARATIONS OF FUNCTION BLOCKS
 * ----------------------------------------------------------------------------
 */
/* --- 030 -------------------------------------------------------------------- */
void fb_mbus_meter_init(STDLIBFBCALL);
/* --- 031 -------------------------------------------------------------------- */
void fb_mbus_meter_read(STDLIBFBCALL);
/* --- 032 -------------------------------------------------------------------- */
void fb_mbus_padpuls_read(STDLIBFBCALL);
/* --- 033 -------------------------------------------------------------------- */
void fb_mbus_electricity_read(STDLIBFBCALL);
/* --- 034 -------------------------------------------------------------------- */
void fb_mbus_water_read(STDLIBFBCALL);

#define RTS_PRAGMA_PACK_1	/* >>>> Align 1 Begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_1

typedef struct HEAT_METER_TAG
{
	DEC_VAR(IEC_DWORD, dwMeterId);  	/* identification number of the meter */
	                                	/* represented in BCD                 */
	DEC_VAR(IEC_DINT, ilEnergy);    	/* heat energy in kWh                 */
	DEC_VAR(IEC_DINT, ilVolume);    	/* water volume in l                  */
	DEC_VAR(IEC_DINT, ilVolumeFlow);	/* water volume flow in l/h           */
	DEC_VAR(IEC_DINT, ilPower);       	/* water power in W                   */
	DEC_VAR(IEC_DINT, ilFlowTemp);   	/* flow temperature in 1/10 C         */
	DEC_VAR(IEC_DINT, ilReturnTemp); 	/* retrun temperature in 1/10 C       */
	DEC_VAR(IEC_DINT, ilTempDiff);   	/* temperature difference in 1/10 K   */
	DEC_VAR(IEC_DINT, ilStatus);      	/* status/error information           */
} HEAT_METER;

typedef struct PADPULS_METER_TAG
{
	DEC_VAR(IEC_DWORD, dwMeterId);		/* identification number of the meter	*/
										/* represented in BCD					*/
	DEC_VAR(IEC_INT, iMedium);			/* medium type							*/
	DEC_VAR(IEC_INT, dummy1);
	DEC_VAR(IEC_DINT, ilValue);			/* measured quantity 					*/
	DEC_VAR(IEC_DINT, ilFactor);		/* multiply factor						*/
	DEC_VAR(IEC_STRING_IMPL, sMU);		/* measurement unit						*/
	DEC_VAR(IEC_STRING_IMPL, sDateTime);/* timestamp of the measurement			*/
	DEC_VAR(IEC_INT, dummy2);
	DEC_VAR(IEC_DINT, ilPrevValue);		/* previous value						*/
	DEC_VAR(IEC_STRING_IMPL, sPrevDate);/* previous measurement date			*/
	DEC_VAR(IEC_BYTE, dummy3);
	DEC_VAR(IEC_INT, dummy4);
	DEC_VAR(IEC_DINT, ilNumerator);
	DEC_VAR(IEC_DINT, ilDenominator);
	DEC_VAR(IEC_DINT, ilStatus);		/* status/error information				*/
} PADPULS_METER;

typedef struct ELECTRYCITY_METER_TAG
{
	DEC_VAR(IEC_DWORD, dwMeterId); 			/* identification number of the meter	*/
											/* represented in BCD 					*/
	DEC_VAR(IEC_DINT, ilEnergy);			/* heat energy in Wh                 	*/
	DEC_VAR(IEC_DINT, ilCurrentCapacity);	/* power in W                 			*/
	DEC_VAR(IEC_DINT, ilOperationHours);	/* operation time in h                	*/
	DEC_VAR(IEC_STRING_IMPL, sSystemTime);	/* date and time of the measurement		*/
	DEC_VAR(IEC_BYTE, dummy1);
	DEC_VAR(IEC_INT, dummy2);
	DEC_VAR(IEC_DINT, ilStatus);			/* status/error information 			*/
} ELECTRICITY_METER;

typedef struct WATER_METER_TAG
{
	DEC_VAR(IEC_DWORD, dwMeterId);  	/* identification number of the meter */
	                                	/* represented in BCD                 */
	DEC_VAR(IEC_DINT, ilVolume);    	/* water volume in l                  */
	DEC_VAR(IEC_DINT, ilStatus);      	/* status/error information           */
} WATER_METER;

/* --- 030 -------------------------------------------------------------------- */

typedef struct FB_METER_INIT_PAR_TAG
{
	/* input */
  	DEC_VAR(IEC_BOOL, bEnI);        /* if true init the  meter              	*/
		                        	/* if false do nothing                  	*/
  	DEC_VAR(IEC_CHAR, dummy1);
	DEC_VAR(IEC_INT, dummy2);
	DEC_VAR(IEC_DINT, ilLength);	/* length in bytes of initialization data	*/
  	/* in_out */
  	DEC_PTR(HEAT_METER, meter);     /* a structure that describes the meter 	*/
	DEC_PTR(IEC_BYTE, bypData);		/* array containg initialization data     	*/
  	/* output */
  	DEC_VAR(IEC_BOOL, bReady);      /* true if the operation is finished    	*/

	/* internal values */
	DEC_VAR(IEC_BYTE, byFsmState);	/* current finite-state-machine state value	*/ 
	DEC_VAR(IEC_BYTE, dummy3);
	DEC_VAR(IEC_BYTE, dummy4);
	DEC_VAR(IEC_DINT, ilStatus); 	/* status/error information           		*/
	DEC_VAR(IEC_UDINT, ulTimeout);	/* start finite-state-machine time			*/
	DEC_VAR(IEC_DWORD, dwMeterId);	/* meter id									*/
	DEC_VAR(IEC_STRMAX, sCurBuff);	/* current readed buffer 					*/
	DEC_VAR(IEC_STRMAX, sInitBuff);	/* initial data buffer 						*/
} FB_METER_INIT_PAR;

/* --- 031 -------------------------------------------------------------------- */

typedef struct FB_METER_READ_PAR_TAG
{
	/* input */
  	DEC_VAR(IEC_BOOL, bEnR);        	/* if true update the meter structure   	*/
		                        		/* if false do nothing                  	*/
  	DEC_VAR(IEC_CHAR, dummy1);
	DEC_VAR(IEC_UINT, dummy2);
  	/* in_out */
  	DEC_PTR(HEAT_METER, meter);     	/* a structure that describes the meter 	*/
  	/* output */
  	DEC_VAR(IEC_BOOL, bReady);       	/* true if the operation is finished    	*/
	/* internal values */
	DEC_VAR(IEC_BYTE, byFsmState);   	/* current finite-state-machine state value	*/ 
	DEC_VAR(IEC_INT, iReadAgain);		/*  										*/
	DEC_VAR(IEC_DWORD, dwReadCounters);	/* bit array containg flags for 
						   					signaling updated counters 				*/
	DEC_VAR(IEC_UDINT, ulTimeout);		/* start finite-state-machine time			*/
	DEC_VAR(IEC_UINT, uLMeterId);		/* a structure that describes the meter		*/
	DEC_VAR(IEC_STRMAX, sCurBuff);		/* current readed buffer 					*/
} FB_METER_READ_PAR;

/* --- 032 -------------------------------------------------------------------- */

typedef struct FB_PADPULS_READ_PAR_TAG
{
	/* input */
	DEC_VAR(IEC_BOOL, bEnR);			/* if true update the meter structure		*/
										/* if false do nothing						*/
	DEC_VAR(IEC_CHAR, dummy1);
	DEC_VAR(IEC_UINT, dummy2);
	/* in_out */
	DEC_PTR(PADPULS_METER, meter);		/* a structure that tescribes the meter		*/
	/* output */
	DEC_VAR(IEC_BOOL, bReady);			/* true if the operation has finished		*/
	/* internal values */
	DEC_VAR(IEC_BYTE, byFsmState);		/* current finite state machine state value	*/
	DEC_VAR(IEC_INT, iReadAgain);		/*  										*/
	DEC_VAR(IEC_DWORD, dwReadCounters);	/* bit array containg flags for 
						   					signaling updated counters 				*/
	DEC_VAR(IEC_UDINT, ulTimeout);		/* start finite-state-machine time			*/
	DEC_VAR(IEC_UINT, uLMeterId);		/* a structure that describes the meter		*/
	DEC_VAR(IEC_STRMAX, sCurBuff);		/* current readed buffer 					*/
} FB_PADPULS_READ_PAR;

/* --- 033 -------------------------------------------------------------------- */

typedef struct FB_ELECTRICITY_READ_PAR_TAG
{
	/* input */
	DEC_VAR(IEC_BOOL, bEnR);			/* if true update the meter structure		*/
										/* if false do nothing						*/
	DEC_VAR(IEC_CHAR, dummy1);
	DEC_VAR(IEC_UINT, dummy2);
	/* in_out */
	DEC_PTR(ELECTRICITY_METER, meter);	/* a structure that describes the meter		*/
	/* output */
	DEC_VAR(IEC_BOOL, bReady);			/* true if the operation has finished		*/
	/* internal values */
	DEC_VAR(IEC_BYTE, byFsmState);		/* current finite state machine state value	*/
	DEC_VAR(IEC_INT, iReadAgain);		/*  										*/
	DEC_VAR(IEC_DWORD, dwReadCounters);	/* bit array containg flags for 
						   					signaling updated counters 				*/
	DEC_VAR(IEC_UDINT, ulTimeout);		/* start finite-state-machine time			*/
	DEC_VAR(IEC_UINT, uLMeterId);		/* a structure that describes the meter		*/
	DEC_VAR(IEC_STRMAX, sCurBuff);		/* current readed buffer 					*/
} FB_ELECTRICITY_READ_PAR;

typedef struct FB_WATER_READ_PAR_TAG
{
	/* input */
  	DEC_VAR(IEC_BOOL, bEnR);        	/* if true update the meter structure   	*/
		                        		/* if false do nothing                  	*/
  	DEC_VAR(IEC_CHAR, dummy1);
	DEC_VAR(IEC_UINT, dummy2);
  	/* in_out */
  	DEC_PTR(WATER_METER, meter);     	/* a structure that describes the meter 	*/
  	/* output */
  	DEC_VAR(IEC_BOOL, bReady);       	/* true if the operation is finished    	*/
	/* internal values */
	DEC_VAR(IEC_BYTE, byFsmState);   	/* current finite-state-machine state value	*/ 
	DEC_VAR(IEC_INT, iReadAgain);		/*  										*/
	DEC_VAR(IEC_DWORD, dwReadCounters);	/* bit array containg flags for 
						   					signaling updated counters 				*/
	DEC_VAR(IEC_UDINT, ulTimeout);		/* start finite-state-machine time			*/
	DEC_VAR(IEC_UINT, uLMeterId);		/* a structure that describes the meter		*/
	DEC_VAR(IEC_STRMAX, sCurBuff);		/* current readed buffer 					*/
} FB_WATER_READ_PAR;

#define RTS_PRAGMA_PACK_DEF 	
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_DEF 	/* <<<< Align 1 end <<<<<<<<<<<<<<<<<<<<<<<<<<< */


#endif	/* FC_CFG_MBUS_LIB */

#endif	/* _FCMBUS_H_ */

/* ---------------------------------------------------------------------------- */

