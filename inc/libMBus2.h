
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
 * Filename: libMBus2.h
 */


#ifndef _LIBMBUS2_H_
#define _LIBMBUS2_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if defined(RTS_CFG_MBUS2_LIB)

/* error codes 
 */
#define TELEGRAM_OK 				OK
#define TELEGRAM_BAD_LENGTH 		1
#define TELEGRAM_BAD_FORMAT 		2

#define ERR_STATE_INVALID			4
#define ERR_TIMEOUT 				5
#define ERR_FCT_UNKNOWN 			6

#define NO_ANSWER					10
#define MORE_DEVICES_ANSWERED		20
#define WRONG_METER_TYPE			30
#define NON_STATUS_TELEGRAM 		40
#define NON_VARLEN_STATUS			50
#define INVALID_SIGNATURE			60
#define METER_STATUS_ERROR			70
#define METER_POWER_LOW 			80
#define METER_PERMANENT_ERROR		90
#define METER_TEMPORARY_ERROR		100

#define COULD_NOT_OPEN				110
#define INVALID_DATA_LENGTH 		120

#define READ_STATUS_AGAIN			121
#define INCOMPLET_STRUCT			122
#define COUNTER_ERROR				123
#define CONTINUE_READ				124
#define MBUS_ERR_NO_LICENSE 		125

#define ERR_NO_DATA 				201
#define ERR_BAD_DATA_BLOCK			202
#define ERR_DATA_NOT_FOUND			203
#define ERR_BAD_POINTER 			204

#define FC_MBUS_METER_INIT			1
#define FC_MBUS_METER_LIST_SLAVES	2
#define FC_MBUS_METER_READ			3

/* Measurements units
 */
#define MBUS_MU_ON_TIME_S			0
#define MBUS_MU_ON_TIME_M			1
#define MBUS_MU_ON_TIME_H			2
#define MBUS_MU_ON_TIME_D			3

#define MBUS_MU_OPERATING_TIME_S	4
#define MBUS_MU_OPERATING_TIME_M	5
#define MBUS_MU_OPERATING_TIME_H	6
#define MBUS_MU_OPERATING_TIME_D	7

#define MBUS_MU_W_H 				8

#define MBUS_MU_KJ					9
#define MBUS_MU_W					10

#define MBUS_MU_KJ_H				11

#define MBUS_MU_L					12

#define MBUS_MU_M3					13

#define MBUS_MU_M3_H				14
#define MBUS_MU_M3_M				15
#define MBUS_MU_M3_S				16

#define MBUS_MU_L_H 				17

#define MBUS_MU_KG					18

#define MBUS_MU_KG_H				19

#define MBUS_MU_CELSIUS 			20
#define MBUS_MU_KELVIN				21

#define MBUS_MU_BAR 				22

#define MBUS_MU_UNITS_FOR_HCA		23

#define MBUS_MU_AVERAGING_DURATION_S		24
#define MBUS_MU_AVERAGING_DURATION_M		25
#define MBUS_MU_AVERAGING_DURATION_H		26
#define MBUS_MU_AVERAGING_DURATION_D		27

#define MBUS_MU_ACTUALITY_DURATION_S		28
#define MBUS_MU_ACTUALITY_DURATION_M		29
#define MBUS_MU_ACTUALITY_DURATION_H		30
#define MBUS_MU_ACTUALITY_DURATION_D		31

#define MBUS_MU_TIME_POINT			32

/* Medium type
 */
#define MBUS_OTHER					100
#define MBUS_OIL					101
#define MBUS_ELECTRICITY			102
#define MBUS_GAS					103
#define MBUS_HEAT					104
#define MBUS_STEAM					105
#define MBUS_HOT_WATER				106
#define MBUS_WATER					107
#define MBUS_HCA					108
#define MBUS_GAS_MODE_2 			110
#define MBUS_HEAT_MODE_2			111
#define MBUS_HOT_WATER_MODE_2		112
#define MBUS_WATER_MODE_2			113
#define MBUS_HCA_MODE_2 			114


typedef struct
{
		DEC_VAR(IEC_STRLEN, curLen);
		DEC_VAR(IEC_STRLEN, maxLen);
		DEC_VAR(IEC_CHAR,	str[VMM_MAX_IEC_STRLEN]);

} IEC_STRING_IMPL;

void cleanup_mbus2_lib(void);


/** DECLARATIONS OF FUNCTION BLOCKS
 * ----------------------------------------------------------------------------
 */

void fb_mbus2_meter_init(STDLIBFBCALL);
void fb_mbus2_meter_list_slaves(STDLIBFBCALL);
void fb_mbus2_meter_read(STDLIBFBCALL);

void f_mbus2_read_volume(STDLIBFUNCALL);
void f_mbus2_read_units_for_HCA(STDLIBFUNCALL);
void f_mbus2_read_time_point(STDLIBFUNCALL);
void f_mbus2_read_temperature_difference(STDLIBFUNCALL);
void f_mbus2_read_return_temperature(STDLIBFUNCALL);
void f_mbus2_read_pressure(STDLIBFUNCALL);
void f_mbus2_read_power(STDLIBFUNCALL);
void f_mbus2_read_operating_time(STDLIBFUNCALL);
void f_mbus2_read_on_time(STDLIBFUNCALL);
void f_mbus2_read_mass_flow(STDLIBFUNCALL);
void f_mbus2_read_mass(STDLIBFUNCALL);
void f_mbus2_read_flow_temperature(STDLIBFUNCALL);
void f_mbus2_read_energy(STDLIBFUNCALL);
void f_mbus2_read_averaging_duration(STDLIBFUNCALL);
void f_mbus2_read_actuality_duration(STDLIBFUNCALL);
void f_mbus2_read_volume_flow(STDLIBFUNCALL);
void f_mbus2_read_external_temperature(STDLIBFUNCALL);

/**
 * Declaration of MBus2 API 
 */

/**Init the meter with the given ID
 * @param dwMeterId the meter id
 * @param psPort the port name. OS specific, for linux "/dev/ttyS2"
 * @param ilBaud the baud rate, usually 2400
 * @param ulTimeOut the time out in miliseconds, a good value can be 1000
 * @param ilLength the length of the initialization data
 * @param bypData the initialization data
 * @param pilStatus the status, as a return variable
 */
void lib_mbus2_meter_init(	IEC_DWORD dwMeterId, IEC_STRING_IMPL *psPort, IEC_DINT ilBaud,	/* in */
							IEC_UDINT ulTimeOut, IEC_DINT ilLength, 
							IEC_BYTE  *bypData, 											/* in/out */
							IEC_DINT  *pilStatus											/* out */
							);

/**Read the meter
 * @param dwMeterID the meter ID
 * @param psPort the port name. OS specific, for linux "/dev/ttyS2"
 * @param ilBaud the baud rate, usually 2400
 * @param ulTimeOut the time out in miliseconds, a good value can be 1000
 * @param pilStatus the status, as a return variable
 * @param pbyVersion the version of the meter
 * @param psManufacturer the meanufacturer, as a 3 char string
 * @param pbyMedium the medium of the meter
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 */
void lib_mbus2_meter_read(	IEC_DWORD dwMeterID, IEC_STRING_IMPL OS_DPTR *psPort, IEC_DINT ilBaud,	
							IEC_UDINT ulTimeOut,
							IEC_DINT OS_DPTR *pilStatus,
							IEC_BYTE OS_DPTR *pbyVersion, 
							IEC_STRING_IMPL OS_DPTR *psManufacturer,
							IEC_BYTE OS_DPTR *pbyMedium,
							IEC_BYTE OS_DPTR *pbyData1, IEC_BYTE OS_DPTR *pbyData2, IEC_BYTE OS_DPTR *pbyData3, IEC_BYTE OS_DPTR *pbyData4, 
							IEC_UDINT OS_DPTR *pilLength1, IEC_UDINT OS_DPTR *pilLength2, IEC_UDINT OS_DPTR *pilLength3, IEC_UDINT OS_DPTR *pilLength4 
							);
							
/**List the slaves connected to MBus
 * @param psPort the port name. OS specific, for linux "/dev/ttyS2"
 * @param ilBaud the baud rate, usually 2400
 * @param ulTimeOut the time out in miliseconds, a good value can be 1000
 * @param bypData array of DWORD containing the read slaves ID
 * @param pilLength the length of the array out variable 
 * @param pilStatus the status, as a return variable
*/
void lib_mbus2_meter_list_slaves(IEC_STRING_IMPL OS_DPTR *psPort, IEC_DINT ilBaud,	
							IEC_UDINT ulTimeOut,
							IEC_BYTE OS_DPTR *bypData, 
							IEC_UDINT OS_DPTR *pilLength,
							IEC_DINT OS_DPTR *pilStatus 											
							);
/**Read the volume
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param pfValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_volume(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_REAL *pfValue, IEC_INT *piErrorCode);

/**Read the units for HCA
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_units_for_HCA(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piValue, IEC_INT *piErrorCode);

/**Read the time point value
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param psValue the value out variable
 * @param piErrorCode the error code
 */
 void lib_mbus2_read_time_point(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_STRING_IMPL *psValue, IEC_INT *piErrorCode);

/**Read the temperature difference
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param pfValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_temperature_difference(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_REAL *pfValue, IEC_INT *piErrorCode);


/**Read the return temperature
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param pfValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_return_temperature(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_REAL *pfValue, IEC_INT *piErrorCode);

/**Read the preassure
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param pfValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_pressure(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_REAL *pfValue, IEC_INT *piErrorCode);

/**Read the power
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param pfValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_power(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_REAL *pfValue, IEC_INT *piErrorCode);

/**Read the operating time
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param piValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_operating_time(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_DINT *piValue, IEC_INT *piErrorCode);

/**Read the on time value
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param piValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_on_time(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_DINT *piValue, IEC_INT *piErrorCode);

/**Read the mass flow
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param pfValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_mass_flow(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_REAL *pfValue, IEC_INT *piErrorCode);

/**Read the mass
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param pfValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_mass(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_REAL *pfValue, IEC_INT *piErrorCode);

/**Read the temperature flow
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param pfValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_flow_temperature(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_REAL *pfValue, IEC_INT *piErrorCode);

/**Read the energy
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param pfValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_energy(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_REAL *pfValue, IEC_INT *piErrorCode);

/**Read the averaging duretion
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param piValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_averaging_duration(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_DINT *piValue, IEC_INT *piErrorCode);

/**Read the actuality duration
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param piValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_actuality_duration(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_DINT *piValue, IEC_INT *piErrorCode);

/**Read the volume flow
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param pfValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_volume_flow(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_REAL *pfValue, IEC_INT *piErrorCode);

/**Read the external temperature
 * @param pbyData1 the first telegram
 * @param pbyData2 the second telegram
 * @param pbyData3 the third telegram
 * @param pbyData4 the forth telegram
 * @param pilLength1 the length of the first telegram
 * @param pilLength2 the length of the second telegram
 * @param pilLength3 the length of the third telegram
 * @param pilLength4 the length of the forth telegram 
 * @param iStorageNo the storage number
 * @param iTariffNo the tariff number
 * @param piMeasurementUnit measurement unit out variable
 * @param pfValue the value out variable
 * @param piErrorCode the error code
 */
void lib_mbus2_read_external_temperature(IEC_BYTE *bypData1, IEC_BYTE *bypData2, IEC_BYTE *bypData3, IEC_BYTE *bypData4, 
							IEC_INT len1, IEC_INT len2, IEC_INT len3, IEC_INT len4, 
							IEC_INT iStorageNo, IEC_INT iTariffNo, IEC_DINT *piMeasurementUnit, IEC_REAL *pfValue, IEC_INT *piErrorCode);

#define RTS_PRAGMA_PACK_1	/* >>>> Align 1 Begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_1

/**single char telegram, used for ack*/
typedef struct SINGLE_CHAR_TELEGRAM_TAG
{
	IEC_DATA ack;						/**single char, must be 0xE5 */
} SINGLE_CHAR_TELEGRAM;

/**short frame telegram*/
typedef struct SHORT_FRAME_TELEGRAM_TAG
{
	IEC_DATA start; 		/**start char, must be 0x10*/
	IEC_DATA c_field;			/**control field, function field*/
	IEC_DATA a_field;			/**address field*/
	IEC_DATA checksum;			/**checksum*/
	IEC_DATA stop;				/**stop char, must be 0x16*/
} SHORT_FRAME_TELEGRAM;

/**control frame telegram*/
typedef struct CONTROL_FRAME_TELEGRAM_TAG
{
	IEC_DATA start; 			/**start char, must be 0x68*/
	IEC_DATA l_field;			/**length field, must be 0x03*/
	IEC_DATA l_field_resent;	/**length field reset, must be 0x03*/
	IEC_DATA start_resent;		/**start field resent, must be 0x68*/
	IEC_DATA c_field;			/**control field, function field*/
	IEC_DATA a_field;			/**address field*/
	IEC_DATA ci_field;			/**control information field*/
	IEC_DATA checksum;			/**checksum*/
	IEC_DATA stop;				/**stop char, must be 0x16*/
} CONTROL_FRAME_TELEGRAM;


/**long frame telegram*/
typedef struct LONG_FRAME_TELEGRAM_TAG
{
	IEC_DATA start; 		/**start char, must be 0x68*/
	IEC_DATA l_field;			/**length field*/
	IEC_DATA l_field_resent;	/**length field reset*/
	IEC_DATA start_resent;		/**start field resent, must be 0x68*/
	IEC_DATA c_field;			/**control field, function field*/
	IEC_DATA a_field;			/**address field*/
	IEC_DATA ci_field;			/**control information field*/
	IEC_DATA *user_data;		/**pointer to the user data. The buffer must not exceed 252 bytes*/
	IEC_DATA checksum;			/**checksum*/
	IEC_DATA stop;				/**stop char, must be 0x16*/
} LONG_FRAME_TELEGRAM;

/**------------40---------------*/
typedef struct FB_MBUS2_METER_INIT_PAR_TAG
{
	/* input */
	DEC_VAR(IEC_BOOL, bEnI);			/* if true init the  meter					*/
										/* if false do nothing						*/
	DEC_VAR(IEC_CHAR, dummy1);
	DEC_VAR(IEC_INT,  dummy2);
	
	DEC_VAR(IEC_DWORD, dwMeterId);		/* meter id */

	DEC_VAR(IEC_STRING_IMPL, sPort);	/* port name, OS specific*/

	DEC_VAR(IEC_CHAR, dummy3);
	DEC_VAR(IEC_INT,  dummy4);

	DEC_VAR(IEC_DINT, ilBaud);			/* the port baud rate */
	
	DEC_VAR(IEC_UDINT, ulTimeOut);		/* time out for the communication channel */
	
	DEC_VAR(IEC_DINT, ilLength);		/* length in bytes of initialization data	*/

	/* in_out */
	DEC_PTR(IEC_BYTE, bypData); 		/* array containg initialization data		*/

	/* output */
	DEC_VAR(IEC_DINT, ilStatus);		/* status/error information 				*/	

	DEC_VAR(IEC_BOOL, bReady);			/* true if the operation is finished		*/
	DEC_VAR(IEC_BYTE, dummy5);
	/* internal values */
	DEC_VAR(IEC_BYTE, byFsmState);		/* current finite-state-machine state value */ 
	DEC_VAR(IEC_BYTE, dummy6);
	
	DEC_VAR(IEC_UDINT, ulLastTimeStamp);/* start finite-state-machine time			*/
	DEC_VAR(IEC_INT, wInternalBufferID);/* internal data buffer needed to keep class */
										/*2 data of incomplete telegrams between states*/
	DEC_VAR(IEC_UDINT, internalLen);	/* len of the internal buffer*/
} FB_MBUS2_METER_INIT_PAR;

/**------------41---------------*/
typedef struct FB_MBUS2_METER_LIST_SLAVES_PAR_TAG
{
	/* input */
	DEC_VAR(IEC_BOOL, bEnI);				/* if true init the  meter					*/
											/* if false do nothing						*/
	DEC_VAR(IEC_STRING_IMPL, sPort);		/* port name, OS specific					*/


	DEC_VAR(IEC_INT, dummy4);
	
	DEC_VAR(IEC_DINT,  ilBaud); 			/* the port baud rate						*/
	DEC_VAR(IEC_UDINT, iTimeOut);			/* time out for the communication channel	*/

	/* in/out */
	DEC_PTR(IEC_BYTE, bypData); 			/* array containg the device list			*/
		
	/* output */
	DEC_VAR(IEC_DINT, ilLength);			/* length in elements of device list		*/

	DEC_VAR(IEC_DINT, ilStatus);			/* status/error information   */
	DEC_VAR(IEC_BOOL, bReady);				/* true if the operation is finished		*/
	DEC_VAR(IEC_INT,  dummy5);


	/* internal values */
	DEC_VAR(IEC_BYTE, wInternalBufferID);	/* array ID containging the intermediar values	*/
											/*	read from the serial port					*/
	DEC_VAR(IEC_DINT, ilInternalLen);		/* the len of the internal buffer				*/

	DEC_VAR(IEC_BYTE, byFsmState);			/* current finite-state-machine state value */ 
	DEC_VAR(IEC_INT,  dummy7);
	DEC_VAR(IEC_BYTE, dummy8);	
	DEC_VAR(IEC_UDINT, ulLastTimeStamp);	/* start finite-state-machine time			*/
	
	DEC_VAR(IEC_DWORD, dwLastID);			/* last address scanned 					*/

} FB_MBUS2_METER_LIST_SLAVES_PAR;

/**------------42---------------*/
typedef struct FB_MBUS2_METER_READ_TAG
{
	/* input */
	DEC_VAR(IEC_BOOL, bEnI);			/* if true init the  meter						*/
										/* if false do nothing							*/
	DEC_VAR(IEC_CHAR, dummy1);
	DEC_VAR(IEC_INT, dummy2);
	
	DEC_VAR(IEC_DWORD, dwMeterID);		/* the meter ID 								*/
	
	DEC_VAR(IEC_STRING_IMPL, sPort);	/* port name, OS specific						*/

	DEC_VAR(IEC_CHAR, dummy3);
	DEC_VAR(IEC_INT, dummy4);
	
	DEC_VAR(IEC_DINT, ilBaud);			/* the port baud rate							*/
	DEC_VAR(IEC_UDINT, iTimeOut);			/* time out for the communication channel	*/
	
	/* output */
	DEC_VAR(IEC_DINT, ilStatus);			/* status/error information   */	
	
	DEC_VAR(IEC_BOOL, bReady);				/* true if the operation is finished		*/

	DEC_VAR(IEC_BYTE, byVersion);
	
	DEC_VAR(IEC_STRING_IMPL, sManufacturer);
	DEC_VAR(IEC_BYTE, byMedium);

	/* internal values */
	DEC_VAR(IEC_BYTE, byFsmState);			/* current finite-state-machine state value */ 
	DEC_VAR(IEC_BYTE, dummy6);
	DEC_VAR(IEC_INT, dummy7);

	DEC_VAR(IEC_UDINT, ulLastTimeStamp);	/* start finite-state-machine time			*/
	
	DEC_VAR(IEC_INT, byData1);				/* array containing the data read from the meter*/
	DEC_VAR(IEC_INT, byData2);				/* array containing the data read from the meter*/
	DEC_VAR(IEC_INT, byData3);				/* array containing the data read from the meter*/
	DEC_VAR(IEC_INT, byData4);				/* array containing the data read from the meter*/
	DEC_VAR(IEC_DINT, ilLength1);				/* length in bytes of data 1	*/
	DEC_VAR(IEC_DINT, ilLength2);				/* length in bytes of data 2	*/
	DEC_VAR(IEC_DINT, ilLength3);				/* length in bytes of data 3	*/
	DEC_VAR(IEC_DINT, ilLength4);				/* length in bytes of data 4	*/
	
	DEC_VAR(IEC_BYTE, byTelegramIndex); 		/* the number of received telegrams*/
} FB_MBUS2_METER_READ;

/**------------assign number---------------*/
typedef struct FUN_METER_REAL_TAG
{
	/* input */
	DEC_FUN_INT( iTariffNo);
	DEC_FUN_INT( iStorageNo);

	/* in/out */
	DEC_FUN_PTR(FB_MBUS2_METER_READ, meter);
	
	/* output */
	DEC_FUN_PTR(IEC_REAL, fValue);
	DEC_FUN_PTR(IEC_DINT, iMeasurementUnit);
	
	/* return */
	DEC_FUN_DINT( iErrorCode);
	
} FUN_METER_REAL;

/**------------assign number---------------*/
typedef struct FUN_METER_INT_TAG
{
	/* input */
	DEC_FUN_INT( iTariffNo);
	DEC_FUN_INT( iStorageNo);
	
	/* in/out */
	DEC_PTR(FB_MBUS2_METER_READ, meter);
	
	/* output */
	DEC_FUN_PTR(IEC_DINT, iValue);
	DEC_FUN_PTR(IEC_DINT, iMeasurementUnit);
	
	/* return */
	DEC_FUN_DINT(iErrorCode);
	
} FUN_METER_INT;

/**------------assign number---------------*/
typedef struct FUN_METER_INT_NO_MU_TAG
{
	/* input */
	DEC_FUN_INT(iTariffNo);
	DEC_FUN_INT(iStorageNo);
	
	/* in/out */
	DEC_PTR(FB_MBUS2_METER_READ, meter);
	
	/* output */
	DEC_FUN_PTR(IEC_DINT, iValue);
	
	/* return */
	DEC_FUN_DINT( iErrorCode);
	
} FUN_METER_INT_NO_MU;


typedef struct FUN_METER_STRING_TAG
{
	/* input */
	DEC_FUN_INT(iTariffNo);
	DEC_FUN_INT(iStorageNo);
	
	/* in/out */
	DEC_PTR(FB_MBUS2_METER_READ, meter);
	
	/* output */
	DEC_FUN_PTR(IEC_STRING_IMPL, sValue);
	DEC_FUN_PTR(IEC_DINT, iMeasurementUnit);
	
	/* return */
	DEC_FUN_DINT(iErrorCode);
	
} FUN_METER_STRING;

#define RTS_PRAGMA_PACK_DEF 	
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_DEF 	/* <<<< Align 1 end <<<<<<<<<<<<<<<<<<<<<<<<<<< */

#endif /* RTS_CFG_MBUS2_LIB */

#endif /* _LIBMBUS2_H_ */

/* ---------------------------------------------------------------------------- */
