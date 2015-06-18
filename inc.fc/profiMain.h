
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
 * Filename: profiMain.h
 */


#ifndef _PROFIMAIN_H_
#define _PROFIMAIN_H_

#include "pb_conf.h"
#include "pb_if.h"
#include "pb_err.h"
#include "pb_dp.h"


/* profiWork.c
 * ----------------------------------------------------------------------------
 */
USIGN16 swap_16_intel_motorola(USIGN16 input16);
USIGN32 swap_32_intel_motorola(USIGN32 input32);

#define _SWAP16(x)		  (swap_16_intel_motorola(x))
#define _SWAP32(x)		  (swap_32_intel_motorola(x))

void SetDirection(int iDir);
enum PROFISTATE  GetProfiState(void);
void SetProfiState(enum PROFISTATE	newProfiState);

void *PDP_WorkThread(void *lpParam);

void PBTermRequest(void);
void PBReleaseServiceFiles(void);


/* profiNew.c
 * ----------------------------------------------------------------------------
 */
IEC_UINT fcCreatePBData(void);
IEC_UINT fcFreePBData(void);

IEC_UINT fcGetPBVersion(IEC_CHAR *szVersion, IEC_UINT uLen);


IEC_UINT fcGetPBState(void);
IEC_UINT fcSetPBState(IEC_UINT uPBState);

void FCPB_CleanUp_Mutex_Direct(void *lpParam);


/* profiData.c
 * ----------------------------------------------------------------------------
 */
void SetSlaveExist(USIGN8 SlaveAdress, BOOL bExist);
_T_SLAVE_STATUS *GetSlaveStatusPtr(USIGN8 SlaveAdress);

IEC_UINT fcInitPBConfig(void);
IEC_UINT fcFinalPBConfig(void);

const T_DP_SLAVE_PARA_SET	*GetSlaveConfig(int iSlaveNumber);
const T_DP_BUS_PARA_SET 	*GetMasterConfig(void);
T_DP_SLAVE_PARAM_SLAVE_INFO *GetSlaveInfo(int iSlaveNumber);
const T_DP_INIT_MASTER_REQ	*GetMasterReqData(void);
const T_DP_SLAVE_PARAM_SLAVE_INFO *QuerySlaveInfo(int iSlaveNumber);

int  GetSlaveCount(void);
int  ExistsSlave(int iSlaveNumber);


/* profiFmb.h
 * ----------------------------------------------------------------------------
 */
void fmb_display_event(IN	 USIGN8*  buffer_ptr);

int fmb_set_configuration(int file);
int fmb_set_busparameter(int file);
int fmb_exit(int file);

enum PROFISTATE fmb_handle_event(USIGN8*  buffer_ptr);


/* profiDp.h
 * ----------------------------------------------------------------------------
 */
BOOL dp_init_master
  (
  IN	int file
  );

BOOL dp_set_busparameter
  (
  IN	int file
  );

enum PROFISTATE dp_handle_event
  (
  IN	USIGN8*  buffer_ptr
  );

BOOL dp_download_loc
  (
  IN	const T_DP_SLAVE_PARA_SET*	dp_slave_para_set_ptr,
  IN	USIGN8						SlaveAdress,
  IN	int 						file
  );
BOOL dp_get_slave_param
  (
  IN	USIGN8					  SlaveAdress,
  IN	int 					  file
  );
BOOL dp_act_param_loc
  (
  IN	USIGN8	 mode,
  IN	int 	 file
  );
BOOL dp_get_slave_diag
  (
  IN	int 	 file
  );
BOOL dp_exit_master
  (
  IN	int 	 file
  );
BOOL dp_flush_diag_buffer
  (
  IN	USIGN8	SlaveAdress,
  IN	int file
  );


#endif	/* _PROFIMAIN_H_ */

/* ---------------------------------------------------------------------------- */
