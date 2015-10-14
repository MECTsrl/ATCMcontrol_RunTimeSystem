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
 * Filename: fcDef.h
 */


#ifndef _FCDEF_H_
#define _FCDEF_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif


/* FieldController Root Directory
 * ----------------------------------------------------------------------------
 */
#define FC_ROOT_DIRECTORY			"/control/"
#define FC_FLASH_DIRECTORY			"/flash/"


/* Task / thread priorities and parameters
 * ----------------------------------------------------------------------------
 */
#ifdef __XENO__
#define  FC_PRIO_IO_CANOPEN       91  /*  highest  */
#define  FC_PRIO_IO_CANOPEN_CONF  91
#define  FC_PRIO_IO_CANOPEN_MESG  91
#define  FC_PRIO_IO_CANOPEN_STCK  91
#define  FC_PRIO_VMTIMER          81
#define  FC_PRIO_VMM              80
#define  FC_PRIO_LED              78
#define  FC_PRIO_LIST             75
#define  FC_PRIO_COM              74
#define  FC_PRIO_IO_BACNET        65
#define  FC_PRIO_IO_PROFI_DP      65
#define  FC_PRIO_IO_TEST          65
#define  FC_PRIO_IO_UDP           65
#define  FC_PRIO_IO_KEYPAD        65
#define  FC_PRIO_IO_DAT           65
#define  FC_PRIO_IO_SYN           65
#define  FC_PRIO_IO_MBTCPS        65
#define  FC_PRIO_IO_MBRTUC        65
#define  FC_PRIO_BAC_CFG          64
#define  FC_PRIO_PDP_MGT          64
#define  FC_PRIO_PDP_WORK         63
#define  FC_PRIO_VM_MAX           59
#define  FC_PRIO_VM_MIN           50
#define  FC_PRIO_BAC_DEV          45
#define  FC_PRIO_BAC_COV          44
#define  FC_PRIO_BAC_SCN          43
#define  FC_PRIO_OC               41
#define  FC_PRIO_RET              40
#define  FC_PRIO_BAC_FLH          40  /*  lowest   */
#else
#define  FC_PRIO_IO_CANOPEN       91  /*  highest  */
#define  FC_PRIO_VMTIMER          81
#define  FC_PRIO_VMM              80
#define  FC_PRIO_LED              78
#define  FC_PRIO_LIST             75
#define  FC_PRIO_COM              74
#define  FC_PRIO_IO_BACNET        65
#define  FC_PRIO_IO_DAT           65
#define  FC_PRIO_IO_KEYPAD        65
#define  FC_PRIO_IO_MBRTUC        65
#define  FC_PRIO_IO_MBTCPS        65
#define  FC_PRIO_IO_PROFI_DP      65
#define  FC_PRIO_IO_SYN           65
#define  FC_PRIO_IO_TEST          65
#define  FC_PRIO_IO_UDP           65
#define  FC_PRIO_BAC_CFG          64
#define  FC_PRIO_PDP_MGT          64
#define  FC_PRIO_PDP_WORK         63
#define  FC_PRIO_VM_MAX           59
#define  FC_PRIO_VM_MIN           50
#define  FC_PRIO_BAC_DEV          45
#define  FC_PRIO_BAC_COV          44
#define  FC_PRIO_BAC_SCN          43
#define  FC_PRIO_OC               41
#define  FC_PRIO_BAC_FLH          40
#define  FC_PRIO_RET              40  /*  lowest   */
#endif

#ifdef __XENO__
#define  FC_SCHED_IO_CANOPEN       SCHED_FIFO
#define  FC_SCHED_IO_CANOPEN_CONF  SCHED_FIFO
#define  FC_SCHED_IO_CANOPEN_MESG  SCHED_FIFO
#define  FC_SCHED_IO_CANOPEN_STCK  SCHED_FIFO
#define  FC_SCHED_VMTIMER          SCHED_FIFO
#define  FC_SCHED_VMM              SCHED_FIFO
#define  FC_SCHED_LED              SCHED_FIFO
#define  FC_SCHED_LIST             SCHED_FIFO
#define  FC_SCHED_COM              SCHED_FIFO
#define  FC_SCHED_IO_BACNET        SCHED_FIFO
#define  FC_SCHED_IO_PROFI_DP      SCHED_FIFO
#define  FC_SCHED_IO_TEST          SCHED_FIFO
#define  FC_SCHED_IO_UDP           SCHED_FIFO
#define  FC_SCHED_IO_KEYPAD        SCHED_FIFO
#define  FC_SCHED_IO_DAT           SCHED_FIFO
#define  FC_SCHED_IO_SYN           SCHED_FIFO
#define  FC_SCHED_IO_MBTCPS        SCHED_FIFO
#define  FC_SCHED_IO_MBRTUC        SCHED_FIFO
#define  FC_SCHED_BAC_CFG          SCHED_FIFO
#define  FC_SCHED_PDP_MGT          SCHED_FIFO
#define  FC_SCHED_PDP_WORK         SCHED_FIFO
#define  FC_SCHED_VM               SCHED_RR
#define  FC_SCHED_BAC_DEV          SCHED_FIFO
#define  FC_SCHED_BAC_COV          SCHED_FIFO
#define  FC_SCHED_BAC_SCN          SCHED_FIFO
#define  FC_SCHED_OC               SCHED_FIFO
#define  FC_SCHED_RET              SCHED_FIFO
#define  FC_SCHED_BAC_FLH          SCHED_FIFO
#else
#define  FC_SCHED_VMTIMER          SCHED_FIFO
#define  FC_SCHED_VMM              SCHED_FIFO
#define  FC_SCHED_LED              SCHED_FIFO
#define  FC_SCHED_LIST             SCHED_FIFO
#define  FC_SCHED_COM              SCHED_FIFO
#define  FC_SCHED_IO_TEST          SCHED_FIFO
#define  FC_SCHED_IO_CANOPEN       SCHED_FIFO
#define  FC_SCHED_IO_MBTCPS        SCHED_FIFO
#define  FC_SCHED_IO_MBRTUC        SCHED_FIFO
#define  FC_SCHED_IO_UDP           SCHED_FIFO
#define  FC_SCHED_IO_DAT           SCHED_FIFO
#define  FC_SCHED_IO_SYN           SCHED_FIFO
#define  FC_SCHED_IO_KEYPAD        SCHED_FIFO
#define  FC_SCHED_IO_PROFI_DP      SCHED_FIFO
#define  FC_SCHED_IO_BACNET        SCHED_FIFO
#define  FC_SCHED_BAC_CFG          SCHED_FIFO
#define  FC_SCHED_PDP_MGT          SCHED_FIFO
#define  FC_SCHED_PDP_WORK         SCHED_FIFO
#define  FC_SCHED_VM               SCHED_RR
#define  FC_SCHED_BAC_SCN          SCHED_FIFO
#define  FC_SCHED_BAC_COV          SCHED_FIFO
#define  FC_SCHED_BAC_DEV          SCHED_FIFO
#define  FC_SCHED_OC               SCHED_FIFO
#define  FC_SCHED_RET              SCHED_FIFO
#define  FC_SCHED_BAC_FLH          SCHED_FIFO
#endif


/* Device Files
 * ----------------------------------------------------------------------------
 */
#define FC_DEV_NVRAM		 		"/dev/nvrammfp"


/* Message Handling
 * ----------------------------------------------------------------------------
 */
#define FC_IPC_MSQ_BASE 			111111
#define FC_IPC_MSG_TYPE 			1

#define FC_MAX_MSG_LEN				8000


/* FC Watchdog Handling
 * ----------------------------------------------------------------------------
 */
#define FC_WATCHDOG_TIME			30000				/* in seconds / 100 */	/* TODO */
#define FC_WATCHDOG_DEVICE			"/dev/watchdog"


#endif	/* _FCDEF_H_ */

/* ---------------------------------------------------------------------------- */
