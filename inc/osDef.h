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
 * Filename: osDef.h
 */


#ifndef _OSDEF_H_
#define _OSDEF_H_

/* osDef.h
 * ----------------------------------------------------------------------------
 * Main configuration header file. Don't add or remove any entries, just change
 * the given values appropriate for the adaptation.
 * Additional target specific definitions can be done in osTarget.h.
 */

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif

/* BUILD VERSION */
#define VERSION "atn01-100"

/* Tasking and Communication Configurations
 * ----------------------------------------------------------------------------
 * 0 ...................... Custom Configuration. 
 *							---------------------------------------------------
 *							ST ... Single Task		MT ... Multi Task
 *							IPC .. Interprocess Communication (message queues)
 *							SM ... Shared Memory access between tasks
 *							ML ... Multi Link (requires MT, IPC & C/S!)
 *							C/S .. Client/Server	BP ... Block Protocol
 *							---------------------------------------------------
 *							tasking 		protocol (log - phys)
 *							---------------------------------------------------
 * 5 ...................... MT/IPC			BP	- serial
 * 6 ...................... MT/IPC			BP	- TCP/IP
 * 7 ...................... MT/IPC			C/S - serial
 * 8 ...................... MT/IPC			C/S - TCP/IP
 * 9 ...................... MT/IPC/ML		C/S - serial
 * 10 ..................... MT/IPC/ML		C/S - TCP/IP
 * 11 ..................... MT/SM			BP	- serial
 * 12 ..................... MT/SM			BP	- TCP/IP
 * 13 ..................... MT/SM			C/S - serial
 * 14 ..................... MT/SM			C/S - TCP/IP
 */

#if ! defined(RTS_CFG_LEVEL)
  
	#define RTS_CFG_LEVEL 10

#endif


#if RTS_CFG_LEVEL == 0

/* Custom Configuration
 * ----------------------------------------------------------------------------
 * Adaptation ......... osOpenComm / osReadBlock / osWriteBlock / osCheckIO
 * Handler ............ comBlock -> cmdRun -> cmdProcess
 */
#define RTS_CFG_PROT_BLOCK
#undef	RTS_CFG_PROT_CLIENT
#undef	RTS_CFG_TCP_NATIVE	
#undef	RTS_CFG_VM_IPC
#undef	RTS_CFG_MULTILINK
#endif


/* Multi task, IPC, block communication, serial
 * ----------------------------------------------------------------------------
 * Adaptation ......... osOpenComm / osReadBlock / osWriteBlock / osCheckIO
 * Handler ............ comBlock -> cmdRun -> cmdProcess
 */
#if RTS_CFG_LEVEL == 5

#define RTS_CFG_PROT_BLOCK
#undef	RTS_CFG_PROT_CLIENT
#undef	RTS_CFG_TCP_NATIVE	
#define RTS_CFG_VM_IPC
#undef	RTS_CFG_MULTILINK
#endif


/* Multi task, IPC, block communication, TCP/IP
 * ----------------------------------------------------------------------------
 * Adaptation ......... osCreateListenTask
 * Handler ............ comBlock -> cmdRun -> cmdProcess
 */
#if RTS_CFG_LEVEL == 6

#define RTS_CFG_PROT_BLOCK
#undef	RTS_CFG_PROT_CLIENT
#define RTS_CFG_TCP_NATIVE
#define RTS_CFG_VM_IPC
#undef	RTS_CFG_MULTILINK
#endif


/* Multi task, IPC, client server communication, serial
 * ----------------------------------------------------------------------------
 * Adaptation ......... osOpenComm / osReadBlock / osWriteBlock
 * Handler ............ comClient -> cmdExecute
 */
#if RTS_CFG_LEVEL == 7

#undef	RTS_CFG_PROT_BLOCK
#define RTS_CFG_PROT_CLIENT
#undef	RTS_CFG_TCP_NATIVE	
#define RTS_CFG_VM_IPC
#undef	RTS_CFG_MULTILINK
#endif


/* Multi task, IPC, client server communication, TCP/IP
 * ----------------------------------------------------------------------------
 * Adaptation ......... osCreateListenTask
 * Handler ............ comClient -> cmdExecute
 */
#if RTS_CFG_LEVEL == 8

#undef	RTS_CFG_PROT_BLOCK
#define RTS_CFG_PROT_CLIENT
#define RTS_CFG_TCP_NATIVE	
#define RTS_CFG_VM_IPC
#undef	RTS_CFG_MULTILINK
#endif


/* Multi task, IPC, multi link, client server communication, serial
 * ----------------------------------------------------------------------------
 * Adaptation ......... osCreateListenTask
 * Handler ............ IPC
 */
#if RTS_CFG_LEVEL == 9

#undef	RTS_CFG_PROT_BLOCK
#define RTS_CFG_PROT_CLIENT
#undef	RTS_CFG_TCP_NATIVE	
#define RTS_CFG_VM_IPC
#define RTS_CFG_MULTILINK
#endif


/* Multi task, IPC, multi link, client server communication, TCP/IP
 * ----------------------------------------------------------------------------
 * Adaptation ......... osCreateListenTask / osCreateCommTask
 * Handler ............ IPC
 */
#if RTS_CFG_LEVEL == 10

#undef	RTS_CFG_PROT_BLOCK
#define RTS_CFG_PROT_CLIENT
#define RTS_CFG_TCP_NATIVE	
#define RTS_CFG_VM_IPC
#define RTS_CFG_MULTILINK
#endif


/* Multi task, shared memory, block communication, serial
 * ----------------------------------------------------------------------------
 * Adaptation ......... osOpenComm / osReadBlock / osWriteBlock / osCheckIO
 * Handler ............ comBlock -> cmdRun -> cmdProcess
 */
#if RTS_CFG_LEVEL == 11

#define RTS_CFG_PROT_BLOCK
#undef	RTS_CFG_PROT_CLIENT
#undef	RTS_CFG_TCP_NATIVE	
#undef	RTS_CFG_VM_IPC
#undef	RTS_CFG_MULTILINK
#endif


/* Multi task, shared memory, block communication, TCP/IP
 * ----------------------------------------------------------------------------
 * Adaptation ......... osCreateListenTask
 * Handler ............ comBlock -> cmdRun -> cmdProcess
 */
#if RTS_CFG_LEVEL == 12

#define RTS_CFG_PROT_BLOCK
#undef	RTS_CFG_PROT_CLIENT
#define RTS_CFG_TCP_NATIVE
#undef	RTS_CFG_VM_IPC
#undef	RTS_CFG_MULTILINK
#endif


/* Multi task, shared memory, client server communication, serial
 * ----------------------------------------------------------------------------
 * Adaptation ......... osOpenComm / osReadBlock / osWriteBlock
 * Handler ............ comClient -> cmdExecute
 */
#if RTS_CFG_LEVEL == 13

#undef	RTS_CFG_PROT_BLOCK
#define RTS_CFG_PROT_CLIENT
#undef	RTS_CFG_TCP_NATIVE	
#undef	RTS_CFG_VM_IPC
#undef	RTS_CFG_MULTILINK
#endif


/* Multi task, shared memory, client server communication, TCP/IP
 * ----------------------------------------------------------------------------
 * Adaptation ......... osCreateListenTask
 * Handler ............ comClient -> cmdExecute
 */
#if RTS_CFG_LEVEL == 14

#undef	RTS_CFG_PROT_BLOCK
#define RTS_CFG_PROT_CLIENT
#define RTS_CFG_TCP_NATIVE	
#undef	RTS_CFG_VM_IPC
#undef	RTS_CFG_MULTILINK
#endif



/* Configurations
 * ----------------------------------------------------------------------------
 * RTS_CFG_BIGENDIAN ...... Must be set for big endian processors. 
 *							(I. e. Motorola).
 * 
 * RTS_CFG_WINDOWS ........ Windows based RTS.
 * RTS_CFG_LINUX .......... Linux based RTS.
 * RTS_CFG_MECT ........ RTS is a Mect product.
 *
 * RTS_CFG_CUSTOMER_LIB ... Enable customer libraries. (osLib.c can be omitted.)
 * RTS_CFG_SYSTEM_LIB ..... Enable system library. (libSys.c can be omitted.)
 * RTS_CFG_UTILITY_LIB .... Enable utility library. (libUtil.c can be omitted.)
 * RTS_CFG_FILE_LIB ....... Enable file library.
 * RTS_CFG_MBUS2_LIB ...... Enable MBus2 library.
 *
 * RTS_CFG_SFC ............ Enable SFC. (libSFC.c can be omitted.)
 * RTS_CFG_FLASH .......... Eanble IEC program flashing. (osFlash.c can be omitted.)
 * RTS_CFG_DEBUG_INFO ..... Enable debug information. (dbiMain.c can be omitted.)
 *
 * RTS_CFG_EVENTS ......... Enable events (event driven tasks).
 *
 * RTS_CFG_EXT_RETAIN ..... Enable external retain task.
 *
 * RTS_CFG_TASK_IMAGE ..... Enable local task image support. (vmPrc.c can be omitted.)
 * RTS_CFG_WRITE_FLAGS .... Enable write flag computation for local task image (%Q).
 * RTS_CFG_WRITE_FLAGS_PI . Enable write flag computation for global process image (%Q). 
 *							(Requires RTS_CFG_WRITE_FLAGS to be set!)
 * RTS_CFG_COPY_DOMAIN .... Enable the CopyDomain mechanism.
 * RTS_CFG_IO_LAYER ....... Enable external IO layer handling.
 *
 * RTS_CFG_FILE_ACCESS .... Enable file access. (osFile.c can be omitted.)
 * RTS_CFG_FILE_NATIVE .... Enable built-in file access. (vmmFile.c can be omitted.)
 *							(File access is necessary for debug information, built-in
 *							file access overrides adaptation file access.)
 *
 * RTS_CFG_EXT_PROJ_INFO .. Enable extended project information storage on control
 * RTS_CFG_STORE_PROJECT .. Enables IEC project storage
 * RTS_CFG_STORE_FILES .... Enables file storage
 * RTS_CFG_CUSTOM_OC ...... Enables custom online commands
 * RTS_CFG_CUSTOM_DL ...... Enables custom download support
 *
 * RTS_CFG_ONLINE_CHANGE .. Enables Online Change Feature.
 *
 * RTS_CFG_FFO ............ Activate free floating instance objects.
 *
 * RTS_CFG_LICENSE ........ Activate feature licensing mechanism.
 *
 * RTS_CFG_BACNET ......... Activate BACnet IO layer. (Not available for all targets!)
 * RTS_CFG_PROFI_DP ....... Activate ProfiBus DP IO layer. (Not available for all targets!)
 * RTS_CFG_IOTEST ......... Activate IO Test layer. (Not available for all targets!)
 *
 * RTS_CFG_SYSLOAD ........ Activate system load/performance counter.
 * RTS_CFG_TASKSTAT ....... Activate VM/IEC task statistics.
 */


/* FarosPLC FieldController & Building Controller
 * ----------------------------------------------------------------------------
 */
#if defined(_SOF_4CFC_SRC_) /* ------------------------------------------------ */

/* ---- Synchronization point - begin - 111 - Don't change -------------------- */


/* ---- Synchronization point - end - 111 - Don't change ---------------------- */

/* FarosPLC SWB Display Controller
 * ----------------------------------------------------------------------------
 */
#elif defined(_SOF_4CDC_SRC_)	/* -------------------------------------------- */


/* FarosPLC LinuxVMM
 * ----------------------------------------------------------------------------
 */
#elif defined(_SOF_4CPC_SRC_)	/* -------------------------------------------- */

#undef	RTS_CFG_BIGENDIAN

#undef	RTS_CFG_WINDOWS
#define RTS_CFG_LINUX
#define RTS_CFG_MECT

#define RTS_CFG_CUSTOMER_LIB
#define RTS_CFG_SYSTEM_LIB
#define RTS_CFG_SYSTEM_LIB_NT
#define RTS_CFG_UTILITY_LIB
#define RTS_CFG_FILE_LIB
#undef	RTS_CFG_MBUS2_LIB
#ifdef USE_CROSSTABLE
#undef	RTS_CFG_MECT_LIB  
#undef RTS_CFG_MECT_UTY_LIB 
#define RTS_CFG_USB_LIB
#undef RTS_CFG_DATALOG_LIB
#define RTS_CFG_HW119_LIB
#define RTS_CFG_MODBUS_LIB
#elif USE_NO_CROSSTABLE
#define	RTS_CFG_MECT_LIB     /* define to enable Mect library */
#define RTS_CFG_MECT_UTY_LIB /* define to enable mect utlis library */
#define RTS_CFG_USB_LIB
#define RTS_CFG_DATALOG_LIB
#undef RTS_CFG_HW119_LIB
#undef RTS_CFG_MODBUS_LIB
#endif

#define RTS_CFG_SFC
#define RTS_CFG_FLASH
#define RTS_CFG_DEBUG_INFO

#define RTS_CFG_EVENTS

#undef RTS_CFG_EXT_RETAIN
#ifdef USE_CROSSTABLE
#define RTS_CFG_MECT_RETAIN

#endif
#define RTS_CFG_TASK_IMAGE
#define RTS_CFG_WRITE_FLAGS
#define RTS_CFG_WRITE_FLAGS_PI
#define RTS_CFG_COPY_DOMAIN
#define RTS_CFG_IO_LAYER
#define RTS_CFG_MEMORY_AREA_EXPORT

#define RTS_CFG_EXT_PROJ_INFO
#define RTS_CFG_STORE_PROJECT
#define RTS_CFG_STORE_FILES
#define RTS_CFG_CUSTOM_OC
#define RTS_CFG_CUSTOM_DL

#undef	RTS_CFG_FILE_ACCESS
#define RTS_CFG_FILE_NATIVE

#define RTS_CFG_ONLINE_CHANGE

#define RTS_CFG_FFO

#undef  RTS_CFG_LICENSE

#undef	RTS_CFG_BACNET
#undef	RTS_CFG_PROFI_DP
#undef  RTS_CFG_IOTEST

#define RTS_CFG_SYSLOAD
#define RTS_CFG_TASKSTAT

#else

#error No target specified. Valid targets are 4CFC, 4CDC and 4CPC.

#endif



/* Debug Outpus
 * ----------------------------------------------------------------------------
 *
 * RTS_CFG_DEBUG_OUTPUT ... Enable general debug outputs.
 * RTS_CFG_DEBUG_FILE ..... Trace debug outputs into files also.
 * RTS_CFG_DEBUG_GPIO   ... Enable gpio debug outputs (fast i/o on soc pins).
 *
 * RTS_CFG_COMM_TRACE ..... Debug outputs for communication.
 * RTS_CFG_IPC_TRACE ...... Debug outputs for interprocess communication.
 * RTS_CFG_IPC_TRACE_IO ... Debug outputs for ipc + IO layer notifications.
 * RTS_CFG_OBJ_TRACE ...... Debug outputs for created data and code objects.
 * RTS_CFG_OC_TRACE ....... Debug outputs for online change.
 * RTS_CFG_PRJ_TRACE ...... Debug outputs for general project information.
 * RTS_CFG_ALI_TRACE ...... Debug outputs for structure alignment.
 * RTS_CFG_MEM_TRACE ...... Debug outputs for memory usage.
 * RTS_CFG_RET_TRACE ...... Debug outputs for retain file usage.
 * RTS_CFG_IO_TRACE ....... Debug outputs for general IO layer management.
 * RTS_CFG_IO_VM_TRACE .... Debug outputs for process image access by VM tasks.
 * RTS_CFG_IO_PRC_TRACE ... Debug outputs for task image access by VM tasks.
 * RTS_CFG_IO_BAC_TRACE ... Debug outputs for BACnet IO layer (standard).
 * RTS_CFG_IO_BAC_TRACE_EX. Debug outputs for BACnet IO layer (extended).
 * RTS_CFG_TASK_TRACE ..... Debug outputs for created tasks.
 *
 * RTS_CFG_MEMORY_CHECK ... Activate memory checking for instance objects.
 */
#if defined(DEBUG)

#define RTS_CFG_DEBUG_OUTPUT
#undef	RTS_CFG_DEBUG_FILE
#define	RTS_CFG_DEBUG_GPIO
#warning "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  INFO  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
#warning "IF RTS_CFG_DEBUG_GPIO is enabled it conflicts with the DS1390 spi based RTC"
#warning "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  INFO  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

#undef	RTS_CFG_COMM_TRACE
#undef	RTS_CFG_IPC_TRACE
#undef	RTS_CFG_IPC_TRACE_IO
#undef	RTS_CFG_OBJ_TRACE
#undef RTS_CFG_OC_TRACE
#undef RTS_CFG_PRJ_TRACE
#undef	RTS_CFG_ALI_TRACE
#undef	RTS_CFG_MEM_TRACE
#undef	RTS_CFG_RET_TRACE
#undef RTS_CFG_IO_TRACE
#undef	RTS_CFG_IO_VM_TRACE
#undef	RTS_CFG_IO_PRC_TRACE
#undef RTS_CFG_IO_BAC_TRACE
#undef RTS_CFG_IO_BAC_TRACE_EX
#undef	RTS_CFG_TASK_TRACE

#undef RTS_CFG_MEMORY_CHECK

#else	/* DEBUG */

#undef 	RTS_CFG_DEBUG_OUTPUT
#undef	RTS_CFG_DEBUG_FILE
#define	RTS_CFG_DEBUG_GPIO
#warning "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  INFO  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
#warning "IF RTS_CFG_DEBUG_GPIO is enabled it conflicts with the DS1390 spi based RTC"
#warning "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  INFO  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

#undef	RTS_CFG_COMM_TRACE
#undef	RTS_CFG_IPC_TRACE
#undef	RTS_CFG_IPC_TRACE_IO
#undef	RTS_CFG_OBJ_TRACE
#undef	RTS_CFG_OC_TRACE
#define RTS_CFG_PRJ_TRACE
#undef	RTS_CFG_ALI_TRACE
#undef	RTS_CFG_MEM_TRACE
#undef	RTS_CFG_RET_TRACE
#undef	RTS_CFG_IO_TRACE
#undef	RTS_CFG_IO_VM_TRACE
#undef	RTS_CFG_IO_PRC_TRACE
#undef	RTS_CFG_IO_BAC_TRACE
#undef	RTS_CFG_IO_BAC_TRACE_EX
#undef	RTS_CFG_TASK_TRACE

#undef	RTS_CFG_MEMORY_CHECK

#endif	/* DEBUG */


/* VMM & VM Definitions
 * ----------------------------------------------------------------------------
 * MAX_DATA must be an even number!
 * MAX_EVENTS max. (2 * sizeof(IEC_UDINT) * 8)!
 */

/* FarosPLC SWB Display Controller
 * ----------------------------------------------------------------------------
 */
#if defined(_SOF_4CDC_SRC_) /* ------------------------------------------------ */

#define MAX_TASKS			   5u	/* Max. number of IEC tasks 			*/
#define MAX_BREAKPOINTS 	  10u	/* Max. number of breakpoints			*/
#define MAX_PROGRAMS		  10u	/* Max. number of programs per task 	*/
#define MAX_CODE_OBJ		 250u	/* Max. number of code objects			*/
#define MAX_DATA_OBJ		 800u	/* Max. number of data objects			*/
#define MAX_CALLS			  10u	/* Max. number of IEC fun/FB calls		*/
#define MAX_READ_REGIONS	   0u	/* Max. memory read regions per task	*/
#define MAX_WRITE_REGIONS	   0u	/* Max. memory write regions per task	*/

#define MAX_OC_COPY_REG 	 100u	/* Max. OC instance data copy regions	*/ 
#define MAX_OC_CODE_OBJ 	  50u	/* Max. online change code objects		*/
#define MAX_OC_DATA_OBJ 	  50u	/* Max. online change data objects		*/

#define MAX_DATA		   25000u	/* Max. count of data bytes per block	*/
#define MAX_BP_QUEUE	  (25u+1u)	/* Max. count (-1) of BP notifications	*/
#define MAX_STR_MSG_QUEUE (25u+1u)	/* Max. count (-1) of Messages stored	*/
#define MAX_STR_MSG_LEN 	 100u	/* Max. length of messages				*/
#define MAX_STACK_SIZE		5000u	/* Max. stack size						*/
#define MAX_CONNECTIONS 	   5u	/* Max. count of multi-link connections */

#define MAX_INFO_SHORT		  32u	/* Max. no. of chars for short prj info */
#define MAX_INFO_LONG		 100u	/* Max. no. of chars for long prj info	*/

#define MAX_EVENTS			  64u	/* Max. no. of events					*/

#define MAX_IO_LAYER		   0u	/* Max. no. of IO layer 				*/

#define MAX_BACNET_OBJ		   0u	/* Max. no. of BACnet data objects		*/

/* FarosPLC all other Linux controls
 * ----------------------------------------------------------------------------
 */
#else /* _SOF_4CDC_SRC_ */	/* ------------------------------------------------ */

/* ---- Synchronization point - begin - 222 - Don't change -------------------- */

#define MAX_TASKS			  25u	/* Max. number of IEC tasks 			*/
#define MAX_BREAKPOINTS 	  25u	/* Max. number of breakpoints			*/
#define MAX_PROGRAMS		  25u	/* Max. number of programs per task 	*/
/* Warning: MAX_CODE_OBJ and MAX_DATA_OBJ are compiled into the NCC!		*/
#define MAX_CODE_OBJ		2000u	/* Max. number of code objects			*/
#define MAX_DATA_OBJ		8000u	/* Max. number of data objects			*/
#define MAX_CALLS			  25u	/* Max. number of IEC fun/FB calls		*/
#define MAX_READ_REGIONS	2100u	/* Max. memory read regions per task	*/
#define MAX_WRITE_REGIONS	2100u	/* Max. memory write regions per task	*/

#define MAX_OC_COPY_REG 	5000u	/* Max. OC instance data copy regions	*/ 
#define MAX_OC_CODE_OBJ 	1000u	/* Max. online change code objects		*/
#define MAX_OC_DATA_OBJ 	1000u	/* Max. online change data objects		*/

#define MAX_DATA		   25000u	/* Max. count of data bytes per block	*/
#define MAX_BP_QUEUE	  (25u+1u)	/* Max. count (-1) of BP notifications	*/
#define MAX_STR_MSG_QUEUE (25u+1u)	/* Max. count (-1) of Messages stored	*/
#define MAX_STR_MSG_LEN 	 100u	/* Max. length of messages				*/
#define MAX_STACK_SIZE		5000u	/* Max. stack size						*/
#define MAX_CONNECTIONS 	   5u	/* Max. count of multi-link connections */

#define MAX_INFO_SHORT		  32u	/* Max. no. of chars for short prj info */
#define MAX_INFO_LONG		 100u	/* Max. no. of chars for long prj info	*/

#define MAX_EVENTS			  64u	/* Max. no. of events					*/

#define MAX_IO_LAYER		   7u	/* Max. no. of IO layer 				*/

#define MAX_BACNET_OBJ		2000u	/* Max. no. of BACnet data objects		*/

/* ---- Synchronization point - end - 222 - Don't change ---------------------- */

#endif	/* _SOF_4CDC_SRC_ */


/* Internal communication receive buffer for divided data structures. Should 
 * be at least 1k if extended project info is enabled.
 */
#define MAX_COM_RECV_BUFF		MAX_DATA

/* Internal communication receive buffer for DBI data transfer.
 */
#define MAX_DBI_RECV_BUFF		MAX_DATA



/* Files & Directories
 * ----------------------------------------------------------------------------
 */

#define VMM_DIR_PROJECT 	"vmm/_proj"
#define VMM_DIR_FILE		"vmm/_file"
#define VMM_DIR_CUSTOM		"vmm/_cust"
#define VMM_DIR_DBI 		"vmm/_dbi"
#define VMM_DIR_FLASH		"vmm/_flash"
#define VMM_DIR_TRACE		"vmm/_trace"
#define VMM_DIR_LOAD		"vmm/_load"
#define VMM_DIR_BACNET_DB	"vmm/_bacnet"
#define VMM_DIR_BACNET_FL	"vmm/_bacnet"

#define VMM_FILE_TEMP		"____temp.f4c"
#define VMM_FILE_PROJECT	"_project.f4c"
#define VMM_FILE_FILE_MAP	"_fil_map.f4c"
#define VMM_FILE_FLASH		"_flash.f4c"
#define VMM_FILE_TRACE		"_trace.f4c"
#define VMM_FILE_BACNET 	"_bac_log.f4c"
#define VMM_FILE_CUST_MAP	"_cst_map.f4c"
#define VMM_FILE_LOAD		"_threads.f4c"
#define VMM_FILE_BACNET_DB	"bacnet.db"
#define VMM_FILE_FL_ANA 	"_bac_flash_ana.f4c"
#define VMM_FILE_FL_BIN 	"_bac_flash_bin.f4c"
#define VMM_FILE_FL_MUL 	"_bac_flash_mul.f4c"

/* Retain files
 */
#define VMM_RETFILE_FROM	1
#define VMM_RETFILE_TO		2
#define VMM_DIR_RETAIN		"vmm/_retain"
#define VMM_FILE_RETAIN 	"_retain_%02d.f4c"

#define VMM_PATH_DELI		'/'



/* OS Dependent Wrapper
 * ----------------------------------------------------------------------------
 */
#define VMM_WAIT_FOREVER	0xfffffffful	/* Wait forever 				*/
#define VMM_NO_WAIT 		0x00000000ul	/* No wait						*/



/* Time Out Values
 * ----------------------------------------------------------------------------
 * VMM_TO_IPC_MSG ......... Common IPC time out value (short).
 * VMM_TO_IPC_MSG_LONG .... Common IPC time out value (long).
 * VMM_TO_OPC_MSG_HUGE .... Common IPC time out value (huge).
 * VMM_TO_EXT_MSG ......... Overall time out value for handling an external
 *							command - i.e. received from the OPC server or via
 *							a VisuComm application.
 *
 * VMM_TO_IOL_CONFIG ...... Overal time out value for all IO layer configurations
 */
#define VMM_TO_IPC_MSG			2000u
#define VMM_TO_IPC_MSG_SHORT	1000u
#define VMM_TO_IPC_MSG_LONG 	6000u
#define VMM_TO_IPC_MSG_HUGE 	8000u
#define VMM_TO_EXT_MSG			8000u

#define VMM_TO_IOL_CONFIG	   20000u
#define VMM_TO_IOL_CLEAR	   20000u



/* Online Chane Wait Time
 * ----------------------------------------------------------------------------
 * Wait time before the Online Change task is going to verify if all IEC tasks 
 * has already stopped. If the Online Chang task has real low(est) priority, 
 * this time value can be zero.
 */
#define WAIT_OC_VMFINISH		0u	

 

/* Minimum cycle time for cyclic IEC (VM) tasks
 * ----------------------------------------------------------------------------
 */
#if defined(_SOF_4CDC_SRC_)

#define VM_MIN_CYCLE_TIME		50u

#else /* _SOF_4CDC_SRC_ */

#define VM_MIN_CYCLE_TIME		1u

#endif /* _SOF_4CDC_SRC_ */



/* Period for debug connection checking
 * ----------------------------------------------------------------------------
 */
#define VMM_DEBUG_INTERVAL		(3 * VMM_TO_EXT_MSG + 1000)



/* Retain update time
 * ----------------------------------------------------------------------------
 */
#define VMM_RET_UPD_FIRST	   10000u
#define VMM_RET_UPD_CYCLE	   60000u



/* Timer task definitions
 * ----------------------------------------------------------------------------
 */
/* Maximum timer task suspend time
 */
#define VMM_MAX_TIMER_SUSPEND	150u

/* Timer task message check interval
 */
#define VMM_TIM_MESSAGE_CHECK	(2ul * VMM_MAX_TIMER_SUSPEND)

/* Operating system fixed suspend overhead (in ms)
 * (--> osSlee(x) lasts always (x + VMM_SLEEP_OFFSET) milliseconds)
 */
#define VMM_SLEEP_OFFSET		0u // 10u


/* VM First Execution Delay
 * ----------------------------------------------------------------------------
 */
#define VM_FIRST_EXEC_DELAY 	1000u



/* Watchdog definitions
 * ----------------------------------------------------------------------------
 * WD_TRIGGER_DEFAULT ..... Specifies the default value of the the number of not 
 *							consumed timer messages in order to trigger the 
 *							watchdog.
 * WD_MAX_PENDING_INACTIVE  Maximum number of timer messages if the watchdog is
 *                          deactivated for a specific task. (Avoid message 
 *                          queue overrun.)
 * WD_EVTTASK_CYCLE ....... Specifies the trigger time for event driven tasks.
 */ 

#if defined(_SOF_4CDC_SRC_)

#define WD_TRIGGER_DEFAULT			100
#define WD_MAX_PENDING_INACTIVE		10

#define WD_EVTTASK_CYCLE			250

#else /* _SOF_4CDC_SRC_ */

#define WD_TRIGGER_DEFAULT			25
#define WD_MAX_PENDING_INACTIVE		10

#define WD_EVTTASK_CYCLE			250

#endif /* _SOF_4CDC_SRC_ */


/* Pointer Usage
 * ----------------------------------------------------------------------------
 * OS_CPTR ............ Qualifier for code (instruction) pointer.
 * OS_DPTR ............ Qualifier for data (instance) pointer.
 * OS_SPTR ............ Qualifier for stack pointer.
 * OS_LPTR ............ Must be set to largest qualifier of code, stack and data
 *
 * DEC_VAR ............ Declares a normal variable in a structure.
 * DEC_PTR ............ Declares a pointer variable in a structure.
 *
 * OS_SUBPTR .......... Subtracts a general pointer or a constant.
 * OS_ADDPTR .......... Adds a general pointer or a constant.
 * OS_ADDPTR8 ......... Adds a general pointer or a 8 bit constant.
 * OS_ADDPTR16 ........ Adds a general pointer or a 16 bit constant.
 * OS_ADDPTR32 ........ Adds a general pointer or a 32 bit constant.
 * OS_ADDPTR64 ........ Adds a general pointer or a 64 bit constant.
 */

/* Pointer qualifier
 */
#define OS_CPTR // __attribute__ ((align(4)))
#define OS_DPTR UNALIGNED
#define OS_SPTR // __attribute__ ((align(8)))

/* Must be set to the largest qualifier of code(OS_CPTR), data(OS_DPTR) and 
 * stack(OS_SPTR) pointers!
 */
#define OS_LPTR // __attribute__ ((align(8)))

/* Common structure member declaration
 */
#define DEC_VAR(type, var)		type   var __attribute__ ((packed)) // necessario
#define DEC_PTR(type, var)		type * var __attribute__ ((packed)) // necessario

/* Pointer arithmetic
 */
#define OS_SUBPTR(ptr1,ptr2)	((ptr1) - (ptr2))
#define OS_ADDPTR(ptr1,ptr2)	((ptr1) + (ptr2))

#define OS_ADDPTR8(ptr1,ptr2)	OS_ADDPTR((ptr1),(ptr2))
#define OS_ADDPTR16(ptr1,ptr2)	OS_ADDPTR((ptr1),(ptr2))
#define OS_ADDPTR32(ptr1,ptr2)	OS_ADDPTR((ptr1),(ptr2))
#define OS_ADDPTR64(ptr1,ptr2)	OS_ADDPTR((ptr1),(ptr2))

/* Pointer nomalization
 */
#define OS_NORMALIZEPTR(ptr1)	(ptr1)

/* NULL pointer definition
 */
#ifndef NULL
#define NULL		((void *)0)
#endif



/* Data Types
 * ----------------------------------------------------------------------------
 * The following data types MUST always be expand to the corresponding data
 * type. All Macros must be defined, even if the corresponding data type is 
 * not enabled in the Interpreter.
 *
 * NOTE: If the target system is ANSI compatible, the following definitions 
 * normally do not need to be changed!
 *
 * NS = Data Type is not supported and should not be enabled.
 */
typedef struct
{
  #if defined(RTS_CFG_BIGENDIAN)
			 long H;							/* 32 bit signed			*/
	 unsigned long L;							/* 32 bit unsigned			*/
  #else
	 unsigned long L;							/* 32 bit unsigned			*/
			 long H;							/* 32 bit signed			*/
  #endif

} sint64;

typedef struct
{
  #if defined(RTS_CFG_BIGENDIAN)
	unsigned long H;							/* 32 bit unsigned			*/
	unsigned long L;							/* 32 bit unsigned			*/
  #else
	unsigned long L;							/* 32 bit unsigned			*/
	unsigned long H;							/* 32 bit unsigned			*/
  #endif

} uint64;

#define IEC_BOOL	unsigned char			/*  8 bit unsigned		*/

#define IEC_SINT	signed char 			/*  8 bit signed (NS)		*/
#define IEC_INT 	signed short			/* 16 bit signed		*/
#define IEC_DINT	signed int			/* 32 bit signed		*/
#define IEC_LINT	signed long long		/* 64 bit signed (NS)		*/

#define IEC_USINT	unsigned char			/*  8 bit unsigned (NS)  	*/
#define IEC_UINT	unsigned short			/* 16 bit unsigned (NS) 	*/
#define IEC_UDINT	unsigned int			/* 32 bit unsigned (NS) 	*/
#define IEC_ULINT	unsigned long long		/* 64 bit unsigned (NS) 	*/

#define IEC_REAL	float
#define IEC_LREAL	double

#define IEC_TIME	signed long 			/* 32 bit signed			*/
#define IEC_DATE					/* nyi - NS 				*/
#define IEC_TOD 					/* nyi - NS 				*/
#define IEC_DT						/* nyi - NS 				*/

#define IEC_BYTE	unsigned char			/*	8 bit signed			*/
#define IEC_WORD	unsigned short			/* 16 bit signed			*/
#define IEC_DWORD	unsigned			/* 32 bit signed			*/
#define IEC_LWORD	uint64				/* 64 bit signed (NS)		*/

#define IEC_DATA	unsigned char			/*	8 bit unsigned			*/

#define IEC_CHAR	char				/*	8 bit signed			*/
#define IEC_STRLEN	unsigned char			/*	8 bit signed			*/


#ifndef FALSE
#define FALSE		0
#endif

#ifndef TRUE
#define TRUE		1
#endif



/* Interpreter - Supported Data Types
 * ----------------------------------------------------------------------------
 */

#define IP_CFG_BOOL 		/* IP supports BOOL   */

#define IP_CFG_SINT 		/* IP supports SINT   */
#define IP_CFG_INT			/* IP supports INT	  */
#define IP_CFG_DINT 		/* IP supports DINT   */
#undef	IP_CFG_LINT 		/* IP supports LINT   */	/* not supported */

#undef	IP_CFG_USINT		/* IP supports USINT  */	/* not supported */
#define	IP_CFG_UINT 		/* IP supports UINT   */
#define	IP_CFG_UDINT		/* IP supports UDINT  */
#undef	IP_CFG_ULINT		/* IP supports ULINT  */	/* not supported */

#define IP_CFG_REAL 		/* IP supports REAL   */
#define IP_CFG_LREAL		/* IP supports LREAL  */

#undef	IP_CFG_DATE 		/* IP supports DATE   */	/* not supported */
#undef	IP_CFG_TOD			/* IP supports TOD	  */	/* not supported */
#undef	IP_CFG_DT			/* IP supports DT	  */	/* not supported */
#define IP_CFG_TIME 		/* IP supports TIME   */

#define IP_CFG_STRING		/* IP supports STRING */

#define IP_CFG_BYTE 		/* IP supports BYTE   */
#define IP_CFG_WORD 		/* IP supports WORD   */
#define IP_CFG_DWORD		/* IP supports DWORD  */
#undef	IP_CFG_LWORD		/* IP supports LWORD  */	/* not supported */



/* Interpreter - Performance / Size Optimizations
 * ----------------------------------------------------------------------------
 * Call level for performance/size optimization
 * 
 * Level 0 ............ no calls, max. performance, big function
 * Level 1 ............ conversion opcodes in own function.
 * Level 2 ............ conversion and arithmetic opcodes in own function
 * Level 3 ............ additionally also wide 16, 32 and 64 opcodes in own functions
 */

#define IP_CFG_CLEVEL		0



/* Common Interpreter Configuration
 * ----------------------------------------------------------------------------
 * IP_CFG_NCC ............. Support native code execution.
 *
 * IP_CFG_NO_STACK_CHECK .. Disable stack checking.
 * IP_CFG_NO_ARRAY_CHECK .. Disable array boundary checking.
 * IP_CFG_RANGE_CHECK ..... Enable range check for mathematical functions.
 */

#if defined(_SOF_4CFC_SRC_)
#define IP_CFG_NCC
#else
#undef	IP_CFG_NCC
#endif

#undef  IP_CFG_NO_STACK_CHECK				/* Activate for develop...		*/
#undef	IP_CFG_NO_ARRAY_CHECK
		
#undef	IP_CFG_RANGE_CHECK



/* Alignment Configuration
 * ----------------------------------------------------------------------------
 * Select only one!
 *
 * IP_CFG_STACK8 .......... Minimal parameter size on interpreter stack = 8  bit
 * IP_CFG_STACK16 ......... Minimal parameter size on interpreter stack = 16 bit
 * IP_CFG_STACK32 ......... Minimal parameter size on interpreter stack = 32 bit
 * IP_CFG_STACK64 ......... Minimal parameter size on interpreter stack = 64 bit
 *
 * IP_CFG_INST8 ........... Instance data alignement = 8  bit
 * IP_CFG_INST16 .......... Instance data alignement = 16 bit
 * IP_CFG_INST32 .......... Instance data alignement = 32 bit
 * IP_CFG_INST64 .......... Instance data alignement = 64 bit
 */

#undef	IP_CFG_STACK8
#undef	IP_CFG_STACK16
#define IP_CFG_STACK32
#undef	IP_CFG_STACK64

#undef	IP_CFG_INST8
#undef	IP_CFG_INST16
#undef	IP_CFG_INST32
#define IP_CFG_INST64



/* Opcode prefix configuration
 * ----------------------------------------------------------------------------
 * IP_CFG_GLOB8  .......... IP can handle 2^8  bytes of global memory (wide  8).
 * IP_CFG_GLOB16 .......... IP can handle 2^16 bytes of global memory (wide 16).
 * IP_CFG_GLOB32 .......... IP can handle 2^32 bytes of global memory (wide 32).
 * IP_CFG_GLOB64 .......... IP can handle 2^64 bytes of global memory (wide 64).
 *
 * IP_CFG_OBJ8	........... IP can handle 2^8  byte sized objects (wide  8).
 * IP_CFG_OBJ16 ........... IP can handle 2^16 byte sized objects (wide 16).
 * IP_CFG_OBJ32 ........... IP can handle 2^32 byte sized objects --> Not supported!
 * IP_CFG_OBJ64 ........... IP can handle 2^64 byte sized objects --> Not supported!
 */

#define IP_CFG_GLOB8
#define IP_CFG_GLOB16
#define IP_CFG_GLOB32
#undef	IP_CFG_GLOB64

#define IP_CFG_OBJ8
#define IP_CFG_OBJ16
#define IP_CFG_OBJ32
#undef	IP_CFG_OBJ64



/* Common Function Declaration
 * ----------------------------------------------------------------------------
 */

#define OS_MALLOC				malloc
#define OS_FREE 				free

#if 0
#define OS_MEMCPY(d, s, l)			memcpy((UNALIGNED void *)d, (UNALIGNED void *)s, l)
#define OS_MEMMOVE(d, s, l)			memmove((UNALIGNED void *)d, (UNALIGNED void *)s, l)
#define OS_MEMSET(d, b, l)			memset((UNALIGNED void *)d, b, l)
#define OS_MEMCMP(d, s, l)			memcmp((UNALIGNED void *)d, (UNALIGNED void *)s, l)
#else
#define OS_MEMCPY				osMemCpy // memcpy
#define OS_MEMMOVE				memmove
#define OS_MEMSET				memset
#define OS_MEMCMP				memcmp
#endif

#define OS_STRCPY				strcpy
#define OS_STRNCPY				strncpy
#define OS_STRLEN				strlen
#define OS_SPRINTF				sprintf
#define OS_SSCANF				sscanf
#define OS_FPRINTF				fprintf
#define OS_STRNICMP 			strncasecmp
#define OS_STRICMP				strcasecmp
#define OS_STRCMP				strcmp
#define OS_STRCAT				strcat

#define OS_ISSPACE				isspace
#define OS_ISDIGIT				isdigit

#define OS_ATOF 				atof
#define OS_STRTOL				strtol
#define OS_STRTOUL				strtoul

#define OS_VSPRINTF 			vsprintf

#define OS_SIN					sin
#define OS_ASIN 				asin
#define OS_COS					cos
#define OS_ACOS 				acos
#define OS_TAN					tan
#define OS_ATAN 				atan
#define OS_LOG					log
#define OS_LOG10				log10
#define OS_SQRT 				sqrt
#define OS_EXP					exp
#define OS_FMOD 				fmod

#define os_stdout				stdout
#define os_errno				errno

#define OS_STRERROR 			strerror(os_errno)



/* Fast Time Access
 * ----------------------------------------------------------------------------
 */
#if defined(_SOF_4CFC_SRC_)
#define OS_TIME32				((IEC_UDINT)((IEC_ULINT)(*g_jiffies_ptr) * (IEC_ULINT)10ull))
#else
#define OS_TIME32				osGetTime32()
#endif


/* Shared Data Access - Locking
 * ----------------------------------------------------------------------------
 */

#define OS_LOCK_BIT
#define OS_FREE_BIT

#define OS_LOCK_MEM
#define OS_FREE_MEM

#define OS_LOCK8
#define OS_FREE8

#define OS_LOCK16
#define OS_FREE16

#define OS_LOCK32
#define OS_FREE32

#define OS_LOCK64
#define OS_FREE64



/* Critical Sections - Mapping
 * ----------------------------------------------------------------------------
 */
#define OS_BEGIN_CRITICAL_SECTION(cs)									\
	{																	\
		pthread_cleanup_push(VMM_CleanUp_Mutex, (void *)(cs));			\
		IEC_UINT uCSResCS;												\
		uCSResCS = osBeginCriticalSection(cs);							\
		TR_RET(uCSResCS);

#define OS_END_CRITICAL_SECTION(cs) 									\
		uCSResCS = osEndCriticalSection(cs);							\
		TR_RET(uCSResCS);												\
		pthread_cleanup_pop(0);											\
	}



/* Target Specific Interpreter Execution checking
 * ----------------------------------------------------------------------------
 */
#define OS_EXECUTION_TIME_CHECK 											\
	pthread_testcancel();



/* Shared Data Access - Stack
 * ----------------------------------------------------------------------------
 * Copy data from stack to local data and reverse.
 *
 * - Problems with misaligned access must be considered.
 * - Lock problems have to be solved within the corresponding OS_LOCKxx/OS_FREExx 
 *	 macros.
 */
#define OS_MOVE8(des,src)		*(des) = *(src)
#define OS_MOVE16(des,src)		*(des) = *(src)
#define OS_MOVE32(des,src)		*(des) = *(src)
#define OS_MOVE64(des,src)		*(des) = *(src)
/* #define OS_MOVE64(des,src)	(des)->L = (src)->L; (des)->H = (src)->H */



/* Shared Data Access - IP Stream 
 * ----------------------------------------------------------------------------
 * Copy data from IP data stream.
 * 
 * - dst is always local in C functions of interpreter
 * - src is always the opcode stream as OS_CPTR, must be incremented after op!
 * - Problems with misaligned access must be considered.
 */
#if 0
#define IP_BYTE(dst)		*((IEC_BYTE  *)&dst) = *((IEC_BYTE	OS_CPTR *)pIP); pIP++;
#define IP_WORD(dst)		*((IEC_WORD  UNALIGNED *)&dst) = *((IEC_WORD  OS_CPTR UNALIGNED *)pIP); pIP += 2;
#define IP_DWORD(dst)		*((IEC_DWORD UNALIGNED *)&dst) = *((IEC_DWORD OS_CPTR UNALIGNED *)pIP); pIP += 4;
#define IP_LWORD(dst)		*((IEC_LWORD UNALIGNED *)&dst) = *((IEC_LWORD OS_CPTR UNALIGNED *)pIP); pIP += 8;
#else
#define IP_BYTE(dst)		*((IEC_BYTE  *)&dst) = *((IEC_BYTE	OS_CPTR *)pIP); pIP++;
#define IP_WORD(dst)		OS_MEMCPY(&dst, pIP, sizeof(IEC_WORD));             pIP += sizeof(IEC_WORD);
#define IP_DWORD(dst)		OS_MEMCPY(&dst, pIP, sizeof(IEC_DWORD));            pIP += sizeof(IEC_DWORD);
#define IP_LWORD(dst)		OS_MEMCPY(&dst, pIP, sizeof(IEC_LWORD));            pIP += sizeof(IEC_LWORD);
#endif

/* Debug Support
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_DEBUG_OUTPUT)

	#define TR_STATE(s) 			osTrace(s)
	#define TR_STATE1(s,e)			osTrace(s,e)
	#define TR_ERROR(s,e)			osTrace(s,e)
	
	#define TR_ERR(s,e) 			osTrace("ERR %s(%04ld): %s (reason:%d).\r\n", __4CFILE__, (IEC_UDINT)__LINE__, (s), (e));

	#define TR_ERR_RET(err) 		osTrace("ERR %s(%04ld): 0x%04x\r\n", __4CFILE__, (IEC_UDINT)__LINE__, (err))

	#define RETURN(err) 						\
		{										\
			IEC_UINT xxOK	= OK;				\
			IEC_UINT xxWRN1 = WRN_PROPVAL_BAD;	\
			IEC_UINT xxWRN2 = WRN_HANDLED;		\
												\
			if ((IEC_UINT)(err) != xxOK   &&	\
				(IEC_UINT)(err) != xxWRN1 &&	\
				(IEC_UINT)(err) != xxWRN2 ) 	\
			{									\
				TR_ERR_RET(err);				\
			}									\
			return(err);						\
		}

	#define RETURN_v(err)						\
		{										\
			IEC_UINT xxOK	= OK;				\
			IEC_UINT xxWRN1 = WRN_PROPVAL_BAD;	\
			IEC_UINT xxWRN2 = WRN_HANDLED;		\
												\
			if ((IEC_UINT)(err) != xxOK   &&	\
				(IEC_UINT)(err) != xxWRN1 &&	\
				(IEC_UINT)(err) != xxWRN2 ) 	\
			{									\
				TR_ERR_RET(err);				\
			}									\
			return; 							\
		}

	#define TR_RET(err) 						\
		{										\
			IEC_UINT xxOK	= OK;				\
			IEC_UINT xxWRN1 = WRN_PROPVAL_BAD;	\
			IEC_UINT xxWRN2 = WRN_HANDLED;		\
												\
			if ((IEC_UINT)(err) != xxOK   &&	\
				(IEC_UINT)(err) != xxWRN1 &&	\
				(IEC_UINT)(err) != xxWRN2 ) 	\
			{									\
				TR_ERR_RET(err);				\
			}									\
		}

	#define RETURN_e(err)						\
		{										\
			IEC_UINT xxOK = OK; 				\
			if ((IEC_UDINT)(err) != xxOK)		\
			{									\
				osTrace("ERR %s(%04ld): 0x%04x (reason:%d / %s)\r\n", __4CFILE__, (IEC_UDINT)__LINE__, (err), os_errno, OS_STRERROR); \
			}									\
			return(err);						\
		}
#else

	#define TR_STATE(s) 
	#define TR_ERROR(s,e)
	#define TR_STATE1(s,e)

	#define TR_ERR(s,e)

	#define TR_ERR_RET(err)

	#define RETURN(err) 			return(err)
	#define RETURN_v(err)			return
	#define TR_RET(err)
	#define RETURN_e(err)			return(err)

#endif	/* RTS_CFG_DEBUG_OUTPUT */

#if defined(RTS_CFG_DEBUG_GPIO)

#define XX_GPIO_INIT()				xx_gpio_init()
#define XX_GPIO_SET(n)				xx_gpio_set(n)
#define XX_GPIO_CLR(n)				xx_gpio_clr(n)
#define XX_GPIO_GET(n)				xx_gpio_get(n)
#define XX_GPIO_CLOSE()				xx_gpio_close()

#else

#define XX_GPIO_INIT()
#define XX_GPIO_SET(n)
#define XX_GPIO_CLR(n)
#define XX_GPIO_GET(n)
#define XX_GPIO_CLOSE()

#endif	/* RTS_CFG_DEBUG_GPIO */

#if defined(RTS_CFG_OBJ_TRACE)

	#define TR_FIXED(i,s,a,se)		osTrace("--- %%Ins    %3d %4ld   Bytes at 0x%08lx (%d)\r\n", i, s, a, se)
	#define TR_OBJECT(i,s,a,se) 	osTrace("--- Inst    %3d %4ld   Bytes at 0x%08lx (%d)\r\n", i, s, a, se)
	#define TR_CODE(i,s,a)			osTrace("--- Code    %3d %4ld   Bytes at 0x%08lx\r\n", i, s, a)
	#define TR_FIX_SIMP(y,i,se,o,b) osTrace("--- %%Sim   %3ld/%ld       Bytes/Bits in segment %d at offset %ld (0x%02x)\r\n", y, i, se, o, b)

#else

	#define TR_FIXED(i,s,a,se)
	#define TR_OBJECT(i,s,a,se)
	#define TR_CODE(i,s,a)
	#define TR_FIX_SIMP(y,i,se,o,b)

#endif	/* RTS_CFG_OBJ_TRACE */


/* Debug outputs for task image access by IEC/VM tasks.
 */
#if defined(RTS_CFG_IO_PRC_TRACE)
	#define TR_SETx 						"+++ SET: "
	#define TR_GETx 						"--- GET: "
	#define TR_SET3(s,p1,p2,p3) 			osTrace(TR_SETx s,p1,p2,p3)
	#define TR_GET3(s,p1,p2,p3) 			osTrace(TR_GETx s "\r\n",p1,p2,p3)
	#define TR_SET_NL						osTrace("\r\n")
	#define TR_PLUS 						osTrace("+")
#else
	#define TR_SETx
	#define TR_GETx
	#define TR_SET3(s,p1,p2,p3)
	#define TR_GET3(s,p1,p2,p3)
	#define TR_SET_NL
	#define TR_PLUS
#endif

#endif	/* _OSDEF_H_ */

/* ---------------------------------------------------------------------------- */
