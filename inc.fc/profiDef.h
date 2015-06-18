
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
 * Filename: profiDef.h
 */


#ifndef _PROFIDEF_H_
#define _PROFIDEF_H_

#include "keywords.h"
#include "type.h"

/* Maximum number of PB slaves
 */
#define FC_PB_MAX_SLAVES				128

/* Default slave data length
 */
#define FC_PB_DEF_SLAVE_DATA_LEN		244

/* Size of diag data
 */
#define FC_PB_DATA_DIAG 				32

#define FC_PB_MAX_PDU_DATA_BUFF_LEN 	264

/* PB Device Files
 */
#define FC_PB_DEV_SERVICE				"/dev/pbservice0"
#define FC_PB_DEV_DATA					"/dev/pbdata0"
#define FC_PB_DEV_BOARD 				"/dev/pbboard0"

/* PB Configuration Files
 */
#define FC_PB_BIN_CFG_FILE				"/control/"VMM_DIR_CUSTOM"/DP@card0.bin"
#define FC_PB_SLV_CFG_FILE				"/control/"VMM_DIR_CUSTOM"/DP@card0.cfg"

/* PB Configuration Time Outs
 */
#define FC_PB_WAIT_FOR_PB_STARTUP		15000
#define FC_PB_WAIT_FOR_PB_TERMINATE 	15000


enum PROFISTATE
{
	PB_NULL = 0,
	PB_IO_ERROR,
	PB_FMB_EVENT,
	PB_FMB_SET_CONFIG_REQ,
	PB_FMB_SET_CONFIG_CON,
	PB_FMB_SET_BUSPARA_REQ,
	PB_FMB_SET_BUSPARA_CON,
	PB_DP_INIT_MASTER_REQ,
	PB_DP_INIT_MASTER_CON,
	PB_DP_SET_BUSPARAMETER_REQ,
	PB_DP_SET_BUSPARAMETER_CON,
	PB_DP_DOWNLOAD_LOC_REQ,
	PB_DP_DOWNLOAD_LOC_CON,
	PB_DP_ACT_PARAM_LOC_STOP_REQ,
	PB_DP_ACT_PARAM_LOC_STOP_CON,
	PB_DP_ACT_PARAM_LOC_CLEAR_REQ,
	PB_DP_ACT_PARAM_LOC_CLEAR_CON,
	PB_DP_ACT_PARAM_LOC_OPERATE_REQ,
	PB_DP_ACT_PARAM_LOC_OPERATE_CON,
	PB_DP_ACT_PARAM_LOC_CON,
	PB_DP_GET_SLAVE_PARAM_REQ,
	PB_DP_GET_SLAVE_PARAM_CON,
	PB_DP_GET_SLAVE_DIAG,
	PB_DP_GET_SLAVE_DIAG_LAST,
	PB_DP_FLUSH_DIAG_BUFFER,
	PB_DP_SLAVE_DIAG,
	PB_DP_OPERATE,
	PB_DP_EXIT_REQ,
	PB_DP_EXIT_CON,
	PB_DP_ERROR
};

typedef struct
{
	int 	CurSlaveStatusLen;
	USIGN8	SlaveStatus[FC_PB_DATA_DIAG];

}  _T_SLAVE_STATUS;


typedef struct 
{
	IEC_UINT	uAddr;

	IEC_DWORD	dwState;
	IEC_STRMAX	sState;
	IEC_STRMAX	sExDiag;

} SSlaveDiag;

IEC_UINT dpGetSlaveDiag(SSlaveDiag *pDiag);


/* Profibus DP master states
 */
#define		PB_OK					0x00000000ul
#define		PB_INVALID_SLAVE_NUMBER	0xfffffffdul
#define		PB_MASTER_NOT_OPERATING	0xfffffffeul
#define		PB_SLAVE_NOT_CONFIGURED	0xfffffffful


/* Normal PB Management Tracing
 */
#if defined(FC_CFG_PB_TRACE_PBM)
	#define	TR_PBMx						"--- PBM: "
	#define TR_PBM(s)					osTrace(TR_PBMx "%s\r\n",s)
	#define TR_PBM1(s,p1)				osTrace(TR_PBMx s "\r\n",p1)
	#define TR_PBM2(s,p1,p2)			osTrace(TR_PBMx s "\r\n",p1,p2)
	#define TR_PBM_ERR1(s,d)			osTrace("*** PBM: ERROR: %s. (%s)\r\n",s,d)
	#define TR_PBM_ERR2(s,d,e)			osTrace("*** PBM: ERROR: %s. (%s / %s)\r\n",s,d,e)
	#define TR_PBM_NL					osTrace("\r\n")
#else
	#define	TR_PBMx						
	#define TR_PBM(s)				
	#define TR_PBM1(s,p1)				
	#define TR_PBM2(s,p1,p2)			
	#define TR_PBM_ERR1(s,d)
	#define TR_PBM_ERR2(s,d,e)
	#define TR_PBM_NL
#endif

#if defined(FC_CFG_PB_TRACE_PBW)
	#define	TR_PBWx						"+++ PBW: "
	#define TR_PBW_ST(s)				osTrace(TR_PBWx "%s\r\n",s)
	#define TR_PBW(s)					osTrace(TR_PBWx "%s\r\n",s)
	#define TR_PBW1(s,p1)				osTrace(TR_PBWx s "\r\n",p1)
	#define TR_PBW2(s,p1,p2)			osTrace(TR_PBWx s "\r\n",p1,p2)
	#define TR_PBW_ERR1(s,d)			osTrace("*** PBW: ERROR: %s. (%s)\r\n",s,d)
	#define TR_PBW_ERR2(s,d,e)			osTrace("*** PBW: ERROR: %s. (%s / %s)\r\n",s,d,e)
#else
	#define	TR_PBWx						
	#define TR_PBW_ST(s)
	#define TR_PBW(s)				
	#define TR_PBW1(s,p1)				
	#define TR_PBW2(s,p1,p2)			
	#define TR_PBW_ERR1(s,d)
	#define TR_PBW_ERR2(s,d,e)
#endif

#endif	/* _PROFIDEF_H_ */

/* ---------------------------------------------------------------------------- */
