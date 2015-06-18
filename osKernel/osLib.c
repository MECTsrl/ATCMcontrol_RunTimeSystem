
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
 * Filename: osLib.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"osLib.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

#if defined(RTS_CFG_CUSTOMER_LIB)

/* ----  Target Specific Includes:	 ------------------------------------------ */

#include "../vmLib/libFile.h"

#include "fcComm.h"
#include "fcTime.h"
#include "fcSerCom.h"
#include "fcProfi.h"
#include "fcModbus.h"
#include "fcMBus.h"
#include "canMain.h"

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* Target library functions
 * ----------------------------------------------------------------------------
 */
EXECUTE_FUN g_pTargetFun[] = 
{ 
#if defined(RTS_CFG_FILE_LIB)
									fa_sync_openFile		,	/* 000 */
									fa_sync_readFile		,	/* 001 */
									fa_sync_renameFile		,	/* 002 */
									fa_sync_writeFile		,	/* 003 */
									fa_sync_closeFile		,	/* 004 */
									fa_sync_createDirectory ,	/* 005 */
									fa_sync_deleteFile		,	/* 006 */
									fa_sync_existsFile		,	/* 007 */
									fa_sync_getSize 		,	/* 008 */
									fa_get_diskFreeSpace	,	/* 009 */
									fa_error_string 		,	/* 010 */
									fa_sync_arrayReadFile	,	/* 011 */
									fa_sync_arrayWriteFile	,	/* 012 */
									fa_flush				,	/* 013 */
									NULL					,	/* 014 */
									NULL					,	/* 015 */
#else
									NULL					,	/* 000 */
									NULL					,	/* 001 */
									NULL					,	/* 002 */
									NULL					,	/* 003 */
									NULL					,	/* 004 */
									NULL					,	/* 005 */
									NULL					,	/* 006 */
									NULL					,	/* 007 */
									NULL					,	/* 008 */
									NULL					,	/* 009 */
									NULL					,	/* 010 */
									NULL					,	/* 011 */
									NULL					,	/* 012 */
									NULL					,	/* 013 */
									NULL					,	/* 014 */
									NULL					,	/* 015 */
#endif

/* ---------------------------------------------------------------------------- */

#ifdef FC_CFG_UDPCOMM_LIB
									fun_com_error_string	,	/* 016 */
									NULL					,	/* 017 */
									NULL					,	/* 018 */
									NULL					,	/* 019 */
									NULL					,	/* 020 */
#else
									NULL					,	/* 016 */
									NULL					,	/* 017 */
									NULL					,	/* 018 */
									NULL					,	/* 019 */
									NULL					,	/* 020 */
#endif

/* ---------------------------------------------------------------------------- */

#ifdef FC_CFG_TIME_LIB

									get_datetime			,	/* 021 */
									datetime_to_string		,	/* 022 */
									sub_datetime_datetime	,	/* 023 */
									add_datetime_dint		,	/* 024 */
									get_utc_datetime		,	/* 025 */
									set_datetime			,	/* 026 */
									set_utc_datetime		,	/* 027 */
									get_datetime2			,	/* 028 */
									datetime2_to_string 	,	/* 029 */
									sub_datetime2_datetime2 ,	/* 030 */
									add_datetime2_dint		,	/* 031 */
									get_utc_datetime2		,	/* 032 */
									NULL					,	/* 033 */
									NULL					,	/* 034 */
									NULL					,	/* 035 */
#else
									NULL					,	/* 021 */
									NULL					,	/* 022 */
									NULL					,	/* 023 */
									NULL					,	/* 024 */
									NULL					,	/* 025 */
									NULL					,	/* 026 */
									NULL					,	/* 027 */
									NULL					,	/* 028 */
									NULL					,	/* 029 */
									NULL					,	/* 030 */
									NULL					,	/* 031 */
									NULL					,	/* 032 */
									NULL					,	/* 033 */
									NULL					,	/* 034 */
									NULL					,	/* 035 */
#endif

/* ---------------------------------------------------------------------------- */

#if defined(FC_CFG_PROFIDP_LIB)
									PB_GetSlaveDiag 		,	/* 036 */
									PB_GetMasterState		,	/* 037 */
									NULL					,	/* 038 */
									NULL					,	/* 039 */
#else
									NULL					,	/* 036 */
									NULL					,	/* 037 */
									NULL					,	/* 038 */
									NULL					,	/* 039 */
#endif

/* ---------------------------------------------------------------------------- */

#if defined(FC_CFG_FC_SYSTEM_LIB)
									NULL					,	/* 040 */
									NULL					,	/* 041 */
									NULL					,	/* 042 */
									NULL					,	/* 043 */
									NULL					,	/* 044 */
#else
									NULL					,	/* 040 */
									NULL					,	/* 041 */
									NULL					,	/* 042 */
									NULL					,	/* 043 */
									NULL					,	/* 044 */
#endif

/* ---------------------------------------------------------------------------- */

#if defined(FC_CFG_BC_SYSTEM_LIB)
									NULL					,	/* 045 */
									NULL					,	/* 046 */
									NULL					,	/* 047 */
									NULL					,	/* 048 */
									NULL					,	/* 049 */
#else
									NULL					,	/* 045 */
									NULL					,	/* 046 */
									NULL					,	/* 047 */
									NULL					,	/* 048 */
									NULL					,	/* 049 */
#endif

/* ---------------------------------------------------------------------------- */

#if defined(FC_CFG_DC_SYSTEM_LIB)
									NULL					,	/* 050 */
									NULL					,	/* 051 */
									NULL					,	/* 052 */
									NULL					,	/* 053 */
									NULL					,	/* 054 */
#else
									NULL					,	/* 050 */
									NULL					,	/* 051 */
									NULL					,	/* 052 */
									NULL					,	/* 053 */
									NULL					,	/* 054 */
#endif

/* ---------------------------------------------------------------------------- */

									NULL
};

/* Target library function blocks
 * ----------------------------------------------------------------------------
 */
EXECUTE_FB	g_pTargetFB[] = 
{ 
#ifdef FC_CFG_SERIALCOMM_LIB
									fb_emb_ser_connect		,	/* 000 */
									fb_emb_ser_read 		,	/* 001 */
									fb_emb_ser_write		,	/* 002 */
#else
									NULL					,	/* 000 */
									NULL					,	/* 001 */
									NULL					,	/* 002 */
#endif

/* ---------------------------------------------------------------------------- */

#ifdef FC_CFG_UDPCOMM_LIB
									fb_udp_connect			,	/* 003 */
									NULL					,	/* 004 */
									fb_udp_urcv 			,	/* 005 */
									fb_udp_usend			,	/* 006 */
									fb_udp_array_urcv		,	/* 007 */
									fb_udp_array_usend		,	/* 008 */
#else
									NULL					,	/* 003 */
									NULL					,	/* 004 */
									NULL					,	/* 005 */
									NULL					,	/* 006 */
									NULL					,	/* 007 */
									NULL					,	/* 008 */
#endif

/* ---------------------------------------------------------------------------- */

#ifdef FC_CFG_SERIALCOMM_LIB
									fb_emb_ser_array_read	,	/* 009 */
									fb_emb_ser_array_write	,	/* 010 */
									fb_emb_ser_break_mark	,	/* 011 */
									NULL					,	/* 012 */
									NULL					,	/* 013 */
									NULL					,	/* 014 */
#else
									NULL					,	/* 009 */
									NULL					,	/* 010 */
									NULL					,	/* 011 */
									NULL					,	/* 012 */
									NULL					,	/* 013 */
									NULL					,	/* 014 */
#endif

/* ---------------------------------------------------------------------------- */

#ifdef FC_CFG_MODBUS_RTU_LIB
									modbus_fct01			,	/* 015 */
									modbus_fct02			,	/* 016 */
									modbus_fct03			,	/* 017 */
									modbus_fct04			,	/* 018 */
									modbus_fct05			,	/* 019 */
									modbus_fct06			,	/* 020 */
									modbus_fct07			,	/* 021 */
									modbus_fct08			,	/* 022 */
									modbus_fct0F			,	/* 023 */
									modbus_fct10			,	/* 024 */
									NULL					,	/* 025 */
									NULL					,	/* 026 */
									NULL					,	/* 027 */
									NULL					,	/* 028 */
									NULL					,	/* 029 */
#else
									NULL					,	/* 015 */
									NULL					,	/* 016 */
									NULL					,	/* 017 */
									NULL					,	/* 018 */
									NULL					,	/* 019 */
									NULL					,	/* 020 */
									NULL					,	/* 021 */
									NULL					,	/* 022 */
									NULL					,	/* 023 */
									NULL					,	/* 024 */
									NULL					,	/* 025 */
									NULL					,	/* 026 */
									NULL					,	/* 027 */
									NULL					,	/* 028 */
									NULL					,	/* 029 */
#endif

/* ---------------------------------------------------------------------------- */

#ifdef FC_CFG_MBUS_LIB
									fb_mbus_meter_init		,	/* 030 */
									fb_mbus_meter_read		,	/* 031 */
									fb_mbus_padpuls_read		,	/* 032 */
									fb_mbus_electricity_read,	/* 033 */
									fb_mbus_water_read		,	/* 034 */
#else
									NULL					,	/* 030 */
									NULL					,	/* 031 */
									NULL					,	/* 032 */
									NULL					,	/* 033 */
									NULL					,	/* 034 */
#endif

/* ---------------------------------------------------------------------------- */

									NULL
};

/* ---------------------------------------------------------------------------- */
/**
 * osGetFunCount
 *
 * @return			Number of customer library functions in function
 *					array.
 */
IEC_UINT osGetFunCount()
{
	return sizeof(g_pTargetFun) / sizeof(g_pTargetFun[0]);
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetFBCount
 *
 * @return			Number of customer library function blocks in 
 *					function block array.
 */
IEC_UINT osGetFBCount()
{
	return sizeof(g_pTargetFB) / sizeof(g_pTargetFB[0]);
}

#endif /* RTS_CFG_CUSTOMER_LIB */

/* ---------------------------------------------------------------------------- */

