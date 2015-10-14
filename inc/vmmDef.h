
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
 * Filename: vmmDef.h
 */


#ifndef _VMMDEF_H_
#define _VMMDEF_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if !defined(_STDINC_H_)
#error "Include via stdInc.h only!"
#endif


/* VMM - Version Information
 * ----------------------------------------------------------------------------
 */
#define RTS_MAIN_VERSION		"V2.2.0"			
#define RTS_FULL_VERSION		"V220.B%d.r00"


#define RTS_VERSION_COMPATIBLE	22000 	/* First compatible version 	*/


/* Common Definitions
 * ----------------------------------------------------------------------------
 */

#define VMM_MAX_IEC_IDENT		33u 	/* Max. IEC identifier length		*/
#define VMM_MAX_PATH		   260u 	/* Max. path length 				*/
	
#define VMM_GUID				16u 	/* Size of project GUID 			*/

/* IPC message data 	
 */
#define MAX_MSG_DATA			(MAX_DATA + HD_BLOCK)

/* Max. count of copy regions			
 */
#define MAX_COPY_REGS			(MAX_DATA_OBJ / 2)

										
/* IEC string type
 */
typedef struct
{
	IEC_STRLEN	CurLen;
	IEC_STRLEN	MaxLen;
	IEC_CHAR	Contents[0];  /* just a placeholder, variant size! */

} IEC_STRING;

#define HD_IEC_STRING			(2 * sizeof(IEC_STRLEN))
#define VMM_MAX_IEC_STRLEN		255

typedef struct
{
	IEC_STRLEN	CurLen;
	IEC_STRLEN	MaxLen;
	IEC_CHAR	Contents[VMM_MAX_IEC_STRLEN];

} IEC_STRMAX;


/* Messages (Generic for IPC)
 * ----------------------------------------------------------------------------
 */
typedef struct
{
#if defined(RTS_CFG_LINUX)
#define USE_POSIX_MQUEUE
#ifndef USE_POSIX_MQUEUE
    	DEC_VAR(long,		mtype);					/* Linux specific					*/
#endif
#endif

	DEC_VAR(IEC_UINT,	uRespQueue); 			/* Response Queue					*/

	DEC_VAR(IEC_UINT,	uID);					/* Message ID						*/

	DEC_VAR(IEC_UINT,	uLen);					/* Message data length				*/
	DEC_VAR(IEC_DATA,	pData[MAX_MSG_DATA]);	/* Message data buffer				*/

} SMessage; 							/* -------------------------------- */

#define HD_MESSAGE			(3 * sizeof(IEC_UINT))


/* Breakpoint Information
 * ----------------------------------------------------------------------------
 */

/* Breakpoint Definition - Downloaded from OPC-Server
 */
typedef struct
{
	IEC_UINT	uCode;					/* Code index of breakpoint 		*/
	IEC_UINT	uData;					/* Instance index of breakpoint 	*/	
	IEC_UDINT	uCodePos;				/* Code position of breakpoint		*/

} SBreakpoint;							/* -------------------------------- */

/* Breakpoint Notification - Uploaded to the OPC - server
 */
typedef struct
{
	SBreakpoint BP; 					/* Breakpoint definition			*/
	IEC_UINT	uTask;					/* Concerned task					*/
	IEC_UINT	uState; 				/* Breakpoint state 				*/

} SBPNotification;						/* -------------------------------- */

/* Breakpoint Definition - VM internal
 */
typedef struct
{
	IEC_UDINT	ulBPId; 				/* Breakpoint ID					*/
	IEC_BYTE	byNext; 				/* Next used breakpoint 			*/

	SBreakpoint BP; 					/* Breakpoint definition			*/

} SBPEntry; 							/* -------------------------------- */

/* Queue of Active Breakpoints
 */
typedef struct
{
	IEC_UINT	 uNext; 				/* Next free queue entry			*/
	IEC_UINT	 uLast; 				/* Last free queue entry			*/

	SBPNotification  pQueue[MAX_BP_QUEUE];	/* Notification queue			*/

} SBPQueue; 							/* -------------------------------- */


/* Index Definition
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UINT	uCode;					/* Index: Code block list			*/
	IEC_UINT	uData;					/* Index: Object list (instance)	*/

} SIndex;								/* -------------------------------- */


/* Copy regions for local retain support
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UINT	uData;					/* Index: Object list (instance)	*/
	IEC_UINT	uOffSrc;				/* offset relative to the instance	*/
	IEC_UDINT	ulOffDst;				/* dest offset into  Retain segm	*/
	IEC_UINT	uSize;					/* Size of copy region				*/

} SRetainReg;							/* -------------------------------- */


/* Copy Regions for local task image copy optimization
 * ----------------------------------------------------------------------------
 */

/* Read/Write memory region
 */
typedef struct
{
	IEC_UDINT	ulOffset;				/* Offset to segment base address	*/
	IEC_UINT	uSize;					/* Size of region					*/
	IEC_USINT	usSegment;				/* Segment of region				*/

	IEC_USINT	pSetQ[MAX_IO_LAYER];	/* Region - IO layer matching		*/
	IEC_USINT	pGetI[MAX_IO_LAYER];	/* Region - IO layer matching		*/
	IEC_USINT	pGetQ[MAX_IO_LAYER];	/* Region - IO layer matching		*/

} SRegion;								/* -------------------------------- */

typedef struct 
{
	IEC_UINT	uRegionsRd; 			/* No. of memory read regions		*/
	IEC_UINT	uRegionsWr; 			/* No. of memory write regions		*/

  #if defined(RTS_CFG_TASK_IMAGE)
	SRegion 	pRegionRd[MAX_READ_REGIONS];	/* Read regions in image	*/
	SRegion 	pRegionWr[MAX_WRITE_REGIONS];	/* Write regions in image	*/

	IEC_USINT	pSetQ[MAX_IO_LAYER];	/* Task - IO layer matching 		*/
	IEC_USINT	pGetI[MAX_IO_LAYER];	/* Task - IO layer matching 		*/
	IEC_USINT	pGetQ[MAX_IO_LAYER];	/* Task - IO layer matching 		*/
  #endif
	
} SImageReg;							/* -------------------------------- */


/* Task Definition
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_USINT	usAttrib;				/* Task attributes					*/
	IEC_USINT	usPrograms; 			/* Count of programs in task		*/

	IEC_UDINT	ulPara1;				/* Task Parameter #1				*/
	IEC_UDINT	ulPara2;				/* Task Parameter #2				*/

  #if defined(RTS_CFG_COPY_DOMAIN)
	IEC_UINT	uCpyRegions;			/* Count of CopyRegions for the task*/
	IEC_UINT	uCpyRegOff; 			/* offset of CopyRegions for the task*/
  #endif

	IEC_UDINT	ulWDogCounter;			/* Watchdog trigger count			*/
	IEC_UDINT	ulWDogTrigger;			/* Watchdog trigger level			*/
	IEC_BOOL	bWDogEnable;			/* Enable / Disable WDog			*/
	IEC_BOOL	bWDogOldEnable; 		/* Old WDog state					*/

	IEC_CHAR	szName[VMM_MAX_IEC_IDENT];/* Taskname						*/
	IEC_USINT	usPriority; 			/* Task priority					*/

	SImageReg	*pIR;					/* Copy regions for task image		*/

} STask;								/* -------------------------------- */


/* Task State
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	/* This structure is mapped to an object in the object list. Initial
	 * values are downloaded from 4C Engineering. If this structure is 
	 * changed, the corresponding changes must also done in the 4CCG!
	 */

	IEC_UDINT	ulState;				/* Task state						*/
	IEC_UDINT	ulPrio; 				/* Task priority					*/
	IEC_UDINT	ulCycle;				/* Task cycle time					*/
	IEC_UDINT	ulETMax;				/* Max. execution time				*/
	IEC_UDINT	ulErrNo;				/* Task Error number				*/

} STaskState;							/* -------------------------------- */


/* Task Statistics
 * ----------------------------------------------------------------------------
 */
typedef struct 
{	
	IEC_UDINT	ulExecMax;				/* Maximum execution time			*/
	IEC_UDINT	ulExecMin;				/* Minimum execution time			*/
	IEC_UDINT	ulExecAver; 			/* Average execution time			*/
	
	IEC_UDINT	ulExecCounter;			/* Execution counter				*/
	IEC_ULINT	ullExecSum; 			/* Sum off all execution times		*/

} SStatistic;							/* -------------------------------- */


/* Object Definition
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_DATA OS_LPTR *pAdr; 			/* Start address of object			*/
	IEC_UDINT		ulSize; 			/* Size of object					*/

} SObject;								/* -------------------------------- */


/* Project Data
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UDINT	ulStateVarOffs; 		/* byte offset for __state variable */
	IEC_UINT	uTasks; 				/* Count of tasks					*/
	IEC_UINT	uCode;					/* Count of Code blocks (POU's) 	*/
	IEC_UINT	uData;					/* Count of objects (instances) 	*/
	IEC_UINT	uDirectVars;			/* Count of direct variables		*/
	IEC_UDINT	ulIECOffset;			/* Offset of IEC vars in GDM		*/

  #if defined(RTS_CFG_COPY_DOMAIN)
	IEC_UINT	uCpyRegions;			/* Count of all CopyRegions 		*/
	IEC_UINT	uCpyRegConst;			/* Count of CopyRegions for RConst	*/
	IEC_UINT	uCpyRegCOff;			/* Offset of CopyRegions for RConst */
  #endif

  #if defined(RTS_CFG_EXT_PROJ_INFO)
	IEC_UINT	uProjInfo;				/* No. of project info, 0 or 1 only */
  #endif

	IEC_UINT	uIOLayer;				/* No. of IO (field bus) layer		*/
	
	IEC_UDINT	ulRetainUsed;			/* Used bytes in retain segment.	*/

} SProject; 							/* -------------------------------- */

/* Extended project informations
 */
typedef struct
{
	IEC_CHAR	szProjVers[MAX_INFO_SHORT]; /* Project version				*/
	IEC_CHAR	szProjName[MAX_INFO_SHORT]; /* Project name 				*/
	IEC_CHAR	szModified[MAX_INFO_LONG];	/* Modification date			*/
	IEC_CHAR	szCreated[MAX_INFO_LONG];	/* Creation date				*/
	IEC_CHAR	szOwner[MAX_INFO_SHORT];	/* Owner (download PC)			*/
	IEC_CHAR	szComment1[MAX_INFO_LONG];	/* Comment line one 			*/
	IEC_CHAR	szComment2[MAX_INFO_LONG];	/* Comment line two 			*/
	IEC_CHAR	szComment3[MAX_INFO_LONG];	/* Comment line three			*/

} SProjInfo;


/* File Download
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UDINT	ulDataLen;				/* Length of file data				*/
	IEC_UDINT	ulOffset;				/* Offset in container file 		*/
	IEC_UINT	uNameLen;				/* File name length - without ZB!	*/
	IEC_USINT	usCRC;					/* Checksum 						*/
	IEC_USINT	usHash; 				/* Hash 							*/

} SFileDef; 							/* -------------------------------- */


/* System Load Evaluation
 * ----------------------------------------------------------------------------
 */
typedef struct 
{
	IEC_UDINT ulUser;					/* Processor time: User 			*/
	IEC_UDINT ulNice;					/* Processor time: Nice 			*/
	IEC_UDINT ulSyst;					/* Processor time: System			*/
	IEC_UDINT ulIdle;					/* Processor time: Idle 			*/
	
} SProcTime;							/* -------------------------------- */

typedef struct
{
	IEC_UDINT ulUser;					/* Task/Thread time: User			*/
	IEC_UDINT ulSyst;					/* Task/Thread time: System 		*/

} STaskTime;							/* -------------------------------- */


/* Additional Global Segment Information
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UINT	uLastIndex; 			/* Index of last object in segment	*/
	IEC_UDINT	ulOffset;				/* Offset to last object in segment */

} SSegInfo; 							/* -------------------------------- */


/* Interpreter Exception
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UINT	uCode;					/* Code object index				*/
	IEC_UINT	uData;					/* Instance object index			*/

	IEC_UINT	uErrNo; 				/* Error number 					*/
	IEC_UDINT	ulOffset;				/* offset in code at exception		*/
   
} SException;							/* -------------------------------- */


/* Interpreter Context
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UINT	uCode;					/* Code object index				*/
	IEC_UINT	uData;					/* Instance object index			*/
	
	IEC_UINT	uCodePos;				/* Current offset in code object	*/

	IEC_DATA /*OS_DPTR*/ *pData;

} SContext; 							/* -------------------------------- */


/* Message Queue
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UINT	uNext;					/*									*/
	IEC_UINT	uLast;					/*									*/

	IEC_CHAR	*ppMessage[MAX_STR_MSG_QUEUE];

} SMsgQueue;							/* -------------------------------- */


/* Online Change Informations
 * ----------------------------------------------------------------------------
 */

/* Instance data copy regions
 */
typedef struct
{
	IEC_UINT	uOld;					/* Old index of changed data object */
	IEC_UINT	uNew;					/* New index of changed data object */
	IEC_UDINT	ulSize; 				/* Data size to be copied in bit	*/
	IEC_UINT	uOldOff;				/* Byte offset of old data			*/
	IEC_UINT	uNewOff;				/* Byte offset of new data			*/
	IEC_USINT	usOldBit;				/* Bit	offset of old data			*/
	IEC_USINT	usNewBit;				/* Bit	offset of new data			*/

} SOCCopy;								/* -------------------------------- */

/* New Task informations
 */
typedef struct
{
	STask		Task;					/* Pointer to task definitions		*/

	SIndex		pProg[MAX_PROGRAMS];	/* Program index list				*/

} SOCTask;								/* -------------------------------- */


/* Online change data
 */
typedef struct
{	
	IEC_UINT	uCode;					/* Code objects to be changed		*/
	IEC_UINT	uData;					/* Data objects to be changed		*/
	IEC_UINT	uToCopy;				/* Number of copy regions			*/
	IEC_UINT	uDirVars;				/* Number of direct (%) data blocks */
	IEC_INT 	iCodeDiff;				/* Change in code object list		*/
	IEC_INT 	iDataDiff;				/* Change in data object list		*/
	IEC_UINT	uTaskInfo;				/* True, if task info downloaded	*/
	IEC_UINT	uProjInfo;				/* True, if proj info downloaded	*/
	
  #if ! defined(RTS_CFG_FFO)
	SSegInfo	pSegInfo[MAX_SEGMENTS]; /* Additional segment infos 		*/
  #else
	SSegInfo	RetainInfo; 			/* Additional retain segment infos	*/
  #endif

	IEC_BYTE	pState[MAX_TASKS];		/* Original task state				*/

	IEC_UINT	uRetry; 				/* Stop Task Retry count			*/
	IEC_ULINT	ullTime;				/* Online change execution time 	*/

	SOCCopy 	*pCopy; 				/* Data object copy regions 		*/		

	IEC_UINT	*pCodeInd;				/* Index for changed object 		*/
	IEC_UINT	*pDataInd;				/* Index for changed object 		*/

	IEC_DATA OS_DPTR **ppDataAdr;		/* Address for % objects			*/

	SOCTask 	*pTask; 				/* Online change task informations	*/

	SProject	Project;				/* Online change project info.		*/

} SOnlChg;								/* -------------------------------- */

#define LIST_OFF(x) 	((x) > 0 ? (x) : 0u)

 
/* (Task local) Interpreter Data
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UINT	uContext;				/* Current context					*/
	SObject 	pSeg[MAX_SEGMENTS]; 	/* Task local memory segments		*/

  #if defined(RTS_CFG_WRITE_FLAGS)
	SObject 	WriteFlags; 			/* Write flags - task image (%Q)	*/
  #endif

	IEC_USINT	usCurrent;				/* Actual running program			*/
	
	SIndex		pProg[MAX_PROGRAMS];	/* Program index list				*/
	SContext	pContext[MAX_CALLS];	/* Call stack						*/
	SException	Exception;				/* Exception informations			*/

  #if defined(IP_CFG_NCC)
	void		*pNCC;					/* Pointer to pVM - NCC use only	*/
  #endif

	STaskState	OS_DPTR *pState;		/* Pointer to task state			*/

	IEC_UINT	uHalt;					/* Flag requesting a halt			*/
	
	/* ------- ^^ variables above used in the NCC - don't change ^^ ------- */

  #if ! defined(RTS_CFG_VM_IPC)
	IEC_UDINT	ulCurExeTime;			/* Current task execution time		*/
  #endif

	IEC_DATA	OS_SPTR *pStack;		/* Interpreter stack (base) 		*/
	IEC_DATA	OS_SPTR *pSP;			/* Current stack pointer			*/

	IEC_CHAR	OS_DPTR *pBuffer;		/* Buffer for string operations 	*/

} SIntLocal;							/* -------------------------------- */


/* (Global) Interpreter Data
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	SObject 	*pCode; 				/* Code Block list					*/
  #if defined(IP_CFG_NCC)
	void (**pLibFcts)();
  #endif /* IP_CFG_NCC */
	SObject 	pData[MAX_DATA_OBJ];	/* Global instance object list		*/

	/* ------- ^^ variables above used in the NCC - don't change ^^ ------- */

  #if defined(RTS_CFG_FFO)
	IEC_USINT	pDataSeg[MAX_DATA_OBJ]; /* Segment number for data obj. 	*/
  #endif
	
  #if ! defined(RTS_CFG_FFO)
	SSegInfo	pSegInfo[MAX_SEGMENTS]; /* Additional segment infos 		*/
  #else
	SSegInfo	RetainInfo; 			/* Additional retain segment infos	*/
  #endif

	IEC_BYTE	byFirstBP;					/* First used Entry in List 	*/
	SBPEntry	pBPList[MAX_BREAKPOINTS];	/* Breakpoint list				*/

	SMsgQueue	MsgQueue;				/* Message queue					*/
	SBPQueue	BPQueue;				/* Breakpoint queue (reached BP's)	*/
  
  #if defined(RTS_CFG_COPY_DOMAIN)
	IEC_UINT	uCpyRegions;			/* Count of all CopyRegions 		*/
	SRetainReg	CopyRegions[MAX_COPY_REGS]; /* Downloaded CopyRegions		*/
  #endif

  #if defined(RTS_CFG_WRITE_FLAGS_PI)
	SObject 	WriteFlags; 			/* Write flags - process image (%Q) */
  #endif

	void		*pVMM;					/* Danger - for library usage only! */
	
} SIntShared;							/* -------------------------------- */


/* VM Task Information
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_USINT	usTask; 				/* Task index						*/
	IEC_USINT	usFlags;				/* Task state flags 				*/
		
	STask		Task;					/* Pointer to task definitions		*/
	SIntLocal	Local;					/* Local  interpreter data			*/

	SIntShared	*pShared;				/* Shared interpreter data			*/
	SProject	*pProject;				/* Shared project data				*/

  #if defined(RTS_CFG_TASK_IMAGE)
	SRegion 	*pRegionRd; 			/* Read regions - local task image	*/
	SRegion 	*pRegionWr; 			/* Write regions - local tasks image*/
  #endif

  #if defined(RTS_CFG_EVENTS)
	IEC_USINT	usException;			/* Last task exception				*/
	IEC_USINT	usEvent;				/* Last task event					*/
  #endif

  #if defined(RTS_CFG_TASKSTAT)
	SStatistic	Stat;					/* Task statistics					*/
  #endif

  #if defined(_SOF_4CFC_SRC_) && defined(RTS_CFG_BACNET)
	IEC_BOOL	bInitDone;				/* Special BC BACnet hack!			*/
  #endif

} STaskInfoVM;							/* -------------------------------- */


/* Communication State Machine Variables
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_PROT_BLOCK)

typedef struct
{
	IEC_UINT	uState; 				/* Communication state machine		*/
	IEC_UINT	uCurrDat;				/* Current data blk in sequence 	*/
	IEC_UINT	uCurrReq;				/* Current request blk in sequence	*/
	IEC_BYTE	byBlock;				/* Current block type				*/

	XFrame		SendFrame;				/* Send buffer						*/
	XFrame		RecvFrame;				/* Receive buffer					*/

} SComVMM;								/* -------------------------------- */


/* Command Handler State Machine Variables
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UINT	uState; 				/* Command handler state machine	*/

	IEC_UINT	uError; 				/* Current error state				*/
	IEC_BYTE	byCommand;				/* Current command					*/

} SCmdVMM;								/* -------------------------------- */

#endif /* RTS_CFG_PROT_BLOCK */


/* Built-In TCP/IP Support
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_TCP_NATIVE)

typedef struct
{
	IEC_UINT	uState;
	IEC_UINT	uLen;

	IEC_UINT	uTask;					/* ID of the communication task 	*/
	IEC_UDINT	hSocket;				/* OS-handle of the socket. 		*/
	IEC_UDINT	hTask;					/* OS-handle of the task.			*/

} SComTCP;								/* -------------------------------- */

#endif


/* Object Download Header
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UDINT	ulOffset;				/* See byFixed						*/
	IEC_UDINT	ulSize; 				/* Size of object (in byte) 		*/
	IEC_UINT	uIndex; 				/* Code block index 				*/
	IEC_UINT	uSegment;				/* Segment number					*/
	IEC_BYTE	byFixed;				/* true:  ulOffset = memory address */									
										/* false: ulOffset = channel offset */
	IEC_BYTE	byType; 				/* Object Type						*/

} SDownHeader;							/* -------------------------------- */


/* Direct Variables Download Header
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UDINT	ulOffset;				/* Offset to segment				*/
	IEC_UINT	uSegment;				/* Segment number					*/
	IEC_UINT	uBit;					/* Bit offset in byte				*/
	IEC_UINT	uSize;					/* Size of data (in bit)			*/

} SDownDirect;							/* -------------------------------- */


/* Download Header
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_UDINT	ulSize; 				/* Size of object in byte.			*/
	IEC_UDINT	ulInitSize; 			/* Size of initial value in bit.	*/
	IEC_UDINT	ulOffset;				/* Offset of object within segment. */
	IEC_UINT	uIndex; 				/* Index of object. 				*/
	IEC_UINT	uSegment;				/* Segment of object				*/
	IEC_UINT	uBit;					/* Bit number of initial value. 	*/

} SDLHeader;							/* -------------------------------- */


/* Download Project
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_STORE_PROJECT)

typedef struct
{
	IEC_UDINT	hLoad;					/* Handle for project loading.		*/
	IEC_UDINT	hSave;					/* Handle for project storing.		*/

} SDLProj;								/* -------------------------------- */

#endif


/* Download Data Files
 * ----------------------------------------------------------------------------
 */
  #if defined(RTS_CFG_STORE_FILES)

typedef struct
{
	IEC_UDINT	hLoad;					/* Handle for data file loading.	*/
	IEC_UDINT	hSave;					/* Handle for data file storing.	*/
	IEC_UDINT	hDir;					/* Handle for directory operations. */

	IEC_UINT	uRetry; 				/* Retry count. 					*/

} SDLData;								/* -------------------------------- */

#endif


/* Download Custom
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_CUSTOM_DL) || defined(RTS_CFG_DEBUG_INFO)

typedef struct
{
	IEC_UDINT	hF1;
	IEC_UDINT	hF2;
	
	SFileDef	FD;

} SDLFile;								/* -------------------------------- */

#endif


/* Task/Thread Information
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_SYSLOAD)

typedef struct
{
	IEC_UDINT	ulID;

	IEC_CHAR	szTask[8];
	IEC_CHAR	szInfo[28];

} SSysLoadTask;

#endif


/* Debug Info Handling
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_DEBUG_INFO) 		/* Receive buffer for DBI messages	*/

typedef struct
{		
	IEC_DATA	pRecv[MAX_DBI_RECV_BUFF];

	IEC_UINT	uRecv;					/* Number of bytes in buffer		*/
	IEC_UINT	uProc;					/* Processed bytes					*/

} SDLDebug; 							/* -------------------------------- */

typedef struct 
{
	XDBIVar 	xVar;					/* Variable defintion				*/

	IEC_CHAR	*szName;				/* Name of this variable (with ZB!) */
	
} SDBIVar;								/* -------------------------------- */

typedef struct 
{
	IEC_UDINT	ulOffset;				/* File Offset to member array		*/
	IEC_UINT	uMember;				/* Number of member variables		*/
	IEC_UINT	uType;					/* */

	SDBIVar 	*pMember;				/* Member variables 				*/

} SDBIType; 							/* -------------------------------- */

typedef struct tagSDBIInst
{
	IEC_USINT	usType; 				/* Task index						*/
	IEC_USINT	usNameSize; 			/* Size of Instance name			*/
	IEC_UINT	uInst;					/* Instance index					*/
	IEC_UINT	uIndex; 				/* Index to type object 			*/
	IEC_UINT	uChildren;				/* Number of child objects			*/

	IEC_UDINT	ulFOBrother;			/* File offset to next brother		*/
	IEC_UDINT	ulFOAbsolut;			/* Absolute file offset 			*/

	IEC_CHAR	*szName;				/* Instance Name					*/

	struct tagSDBIInst **ppChildren;	/* Children of this instance		*/

} SDBIInstance; 						/* -------------------------------- */

typedef struct 
{
	IEC_BOOL	bInitialized;			/* DBI module initialized?			*/

	SDBIInstance *pInstRoot;			/* Instance tree					*/

	IEC_UINT	uTypes; 				/* Number of elements in type list	*/
	SDBIType	*pTypes;				/* Type List						*/

	IEC_UDINT	hInst;
	IEC_UDINT	hVar;

} SDBIInfo; 							/* -------------------------------- */

#endif


/* Download buffer
 * ----------------------------------------------------------------------------
 */

/* CMD_GET_VALUE
 */
typedef struct 
{
	IEC_DATA	pRecv[MAX_COM_RECV_BUFF]; /* Recv buffer for diverted data	*/

	IEC_UINT	uRecv;					/* Received download data.			*/
	IEC_UINT	uDone;					/* Handled download data.			*/

} SDLGetVal;							/* -------------------------------- */

/* CMD_START_TASK / CMD_STOP_TASK
 */
typedef struct 
{
	IEC_UINT	uTask;					/* Task number to be stopped.		*/
	
} SDLTask;								/* -------------------------------- */

/* Breakpoint Handling
 */
typedef struct 
{
	SBreakpoint BP; 					/* Breakpoint to be set.			*/
	
	IEC_UDINT	ulBPId; 				/* Breakpoint to be deleted.		*/
	IEC_UINT	uTask;					/* Task to be continued/stepped.	*/

} SDLBreak; 							/* -------------------------------- */

/* Get Project Info
 */
#if defined(RTS_CFG_EXT_PROJ_INFO)

typedef struct 
{
	IEC_UINT	uDone;					/* Transfered project info. 		*/

} SDLProjInfo;							/* -------------------------------- */

#endif

/* CMD_CLEAR_FLASH
 */
#if defined(RTS_CFG_FLASH)

typedef struct 
{
	IEC_UINT	uRetry; 				/* Clear flash retry count. 		*/

} SDLClrFlash;							/* -------------------------------- */

#endif

/* CMD_INITIALIZE_CLEAR
 */
typedef struct
{
	IEC_UINT	uRetry; 				/* Initialize/Clear retry count.	*/

} SDLInitClear; 						/* -------------------------------- */


typedef struct
{
	IEC_DATA	pRecv[MAX_COM_RECV_BUFF]; /* Recv buffer for diverted data	*/

	/* Changed by actDataDiv() only.
	 */
	IEC_UINT	uRecv;					/* No. of bytes already processed	*/
	IEC_BOOL	bDone;					/* Current object finished. 		*/
	IEC_BOOL	bOther; 				/* Other data attached to this obj. */

	/* Changed by callers of actDataDiv() only.
	 */
	IEC_UINT	uObj;					/* Global number of objects.		*/
	IEC_UINT	uObj2;					/* Global number of objects (2).	*/
	IEC_BOOL	bSwitch;				/* True if processing other data.	*/

	/* General download state variables
	 */
	IEC_UINT	uError; 				/* Current command handling error.	*/
	IEC_UINT	uDomain;				/* Current download domain. 		*/
	IEC_UINT	uMode;					/* Current download mode.			*/

	IEC_BOOL	bOCStarted; 			/* Online change procedure started. */
	IEC_USINT	usResState; 			/* Resource state before download.	*/

	IEC_UINT	uRetry; 				/* Download end retry count.		*/

	/* Additional DL helpers
	 */
	SDLHeader	HDR;					/* Additional download data.		*/

  #if defined(RTS_CFG_CUSTOM_DL) || defined(RTS_CFG_DEBUG_INFO)
	SDLFile 	FIL;					
  #endif
  #if defined(RTS_CFG_STORE_PROJECT)
	SDLProj 	PRJ;
  #endif
  #if defined(RTS_CFG_STORE_FILES)
	SDLData 	DAT;
  #endif
  #if defined(RTS_CFG_DEBUG_INFO)
	SDLDebug	DBI;
  #endif

	SDLGetVal	GET;					/* CMD_GET_VALUE					*/
	SDLTask 	TSK;					/* CMD_START_TASK / CMD_STOP_TASK	*/
	SDLBreak	BRK;					/* Breakpoint Handling				*/
  #if defined(RTS_CFG_EXT_PROJ_INFO)
	SDLProjInfo PIN;					/* Project information upload.		*/
  #endif
  #if defined(RTS_CFG_FLASH)
	SDLClrFlash CFL;					/* CMD_CLEAR_FLASH. 				*/
  #endif

	SDLInitClear CIC;					/* CMD_INITIALIZE_CLEAR.			*/

} SDLBuffer;							/* -------------------------------- */


/* Field Bus Channel Information
 * ----------------------------------------------------------------------------
 */
typedef struct 
{
	IEC_UDINT	ulIOffs;				/* Offset of input (%I) channel 	*/
	IEC_UDINT	ulISize;				/* Size of input (%I) channel		*/
	IEC_UDINT	ulQOffs;				/* Offset of output (%Q) channel	*/
	IEC_UDINT	ulQSize;				/* Size of output (%Q) channel		*/
	IEC_UDINT	ulMOffs;				/* Offset of memory (%M) channel	*/
	IEC_UDINT	ulMSize;				/* Size of memory (%M) channel		*/

	IEC_UINT	uState; 				/* Current state of IO layer		*/

	IEC_BOOL	bEnabled;				/* IO layer enabled or not? 		*/
	IEC_BOOL	bCreated;				/* IO layer created or not? 		*/
	IEC_USINT	usChannel;				/* Corresponding Channel number 	*/

	IEC_USINT	usNotifyRd; 			/* Notify IO layer for read access	*/
	IEC_USINT	usNotifyWr; 			/* Notify IO layer for write access */

	IEC_BOOL	bAsyncConfig;			/* Config IO layer asynchronously	*/

	IEC_CHAR	szIOLayer[VMM_MAX_IEC_IDENT];	/* IO layer type			*/
	IEC_UINT	uIOLType;				/* IO layer type			*/

} SIOLayer; 							/* -------------------------------- */


/* IO Layer Initial Configuration Values
 * ----------------------------------------------------------------------------
 */
typedef struct	
{
	IEC_UINT uIOLayer;

	IEC_BOOL bAsyncConfig;
	IEC_BOOL bWarmStart;

} SIOLayerIniVal;


/* Shared memory definition for IO layer applications
 * ----------------------------------------------------------------------------
 */
typedef struct 
{
	DEC_VAR(IEC_UDINT,	ulOffs); 				/* Offset for IO layer in segment	*/
	DEC_VAR(IEC_UDINT,	ulSize); 				/* Size for IO layer in segment 	*/
	DEC_VAR(IEC_UDINT,	ulSegSize);				/* Whole size of segment			*/

	DEC_PTR(IEC_DATA, pAdr); 			/* Start address of segment 		*/

} SIOSegment;							/* -------------------------------- */

typedef struct
{
	SIOSegment	I;						/* Input segment (%I) description	*/
	SIOSegment	Q;						/* Output segment (%Q) description	*/
  #if defined(RTS_CFG_MEMORY_AREA_EXPORT)
    SIOSegment	M;						/* Output segment (%M) description	*/
  #endif
	SIOSegment	W;						/* Write flag description			*/
	SIOSegment	C;						/* Task image copy regions			*/
	SIOSegment	R;						/* Retain segment					*/

	DEC_VAR(IEC_USINT, usChannel);				/* Corresponding Channel number 	*/
	DEC_VAR(IEC_CHAR,  szName[VMM_MAX_IEC_IDENT]);/* IO layer type (fcdp, bcbac, ...) */
	
} SIOConfig;							/* -------------------------------- */

typedef struct
{
	IEC_UINT  uIOLayer; 				/* Corresponding IO layer			*/
	IEC_UINT  uRes; 					/* Result of configuration			*/

	IEC_USINT usChannel;				/* Corresponding Channel number 	*/
	IEC_CHAR  szName[VMM_MAX_IEC_IDENT];/* IO layer type (fcdp, bcbac, ...) */
	
} SIODone;								/* -------------------------------- */

typedef struct
{
	DEC_VAR(IEC_UINT,	uTask); 		/* IEC Task number					*/

	/* If uTask == -1, the following values gives the changed or requested
	 * data area.
	 */
	DEC_VAR(IEC_UINT,	uLen);			/* Data length to be accessed		*/
	DEC_VAR(IEC_UDINT,	ulOffset);		/* Offset in segment (%I, %Q)		*/
	DEC_VAR(IEC_USINT,	usBit); 		/* Bit offset						*/
	DEC_VAR(IEC_USINT,	usSegment); 	/* Segment number (%I, %Q)			*/

} SIONotify;							/* -------------------------------- */


/* External Retain update task
 * ----------------------------------------------------------------------------
 */
typedef struct 
{
	IEC_UINT	uMode;					/* Current download mode			*/
	SObject 	Retain; 				/* Retain segment definition.		*/

	IEC_DATA	pProjectID[VMM_GUID];	/* Current project GUID 			*/

} SRetExt;								/* -------------------------------- */

typedef struct 
{
	IEC_UDINT	ulID;					/* Retain copy ID					*/

	IEC_UDINT	ulSize; 				/* Size of retain segment			*/
	IEC_UDINT	ulUsed; 				/* Used retain segment size 		*/

	IEC_UINT	uCRC1;					/* Checksum 1 (sum) 				*/
	IEC_UINT	uCRC2;					/* Checksum 2 (hash)				*/

	IEC_DATA	pProjectID[VMM_GUID];	/* Project GUID of retain file		*/

	IEC_UINT	uValid; 				/* Retain segment valid 			*/

	IEC_DATA	pRetain[0]; 			/* Retain data						*/

} SRetFile;



/* VMM Task Information
 * ----------------------------------------------------------------------------
 */
typedef struct
{
	IEC_BOOL	bProjLoaded;			/* Project (configuration) loaded?	*/
	IEC_BOOL	bProjActive;			/* Project (configuration) active?	*/

	IEC_UINT	uCreatedTasks;			/* Count of created VM tasks		*/

	IEC_BOOL	bDebugMode; 			/* Control is in debug mode 		*/
	IEC_BOOL	pLogin[MAX_CONNECTIONS];/* Current logged in CT's			*/

	IEC_BOOL	bExecColdStart; 		/* Execute a cold start 			*/
	IEC_BOOL	bExecWarmStart; 		/* Execute a warm start 			*/

	IEC_BOOL	bWarmStart; 			/* Current boot is a warm start 	*/

	IEC_UDINT	ulResState; 			/* VMM (resource) state 			*/
	IEC_UDINT	ulUniqueNumber; 		/* Unique number in all systems 	*/
	IEC_UDINT	ulLastStateReq; 		/* Last state request from OPC-serv.*/

	STaskInfoVM *ppVM[MAX_TASKS];		/* Task information of VM's 		*/
	
	SIntShared	Shared; 				/* Shared interpreter data			*/
	SProject	Project;				/* Project data 					*/

  #if defined(RTS_CFG_PROT_BLOCK)
	SComVMM 	COM;					/* Communication state machine		*/
	SCmdVMM 	CMD;					/* Command handler state machine	*/
  #endif

  #if defined(RTS_CFG_TCP_NATIVE)
	SComTCP 	pTCP[MAX_CONNECTIONS];	/* Built-In TCP/IP support			*/
  #endif
	SDLBuffer	DLB;					/* Action processing values 		*/

	IEC_DATA	pProjectGUID[VMM_GUID]; /* Current project GUID 			*/
	IEC_DATA	pDownLoadGUID[VMM_GUID];/* Download project GUID. (Only to	*/
										/* verify retain data!) 			*/

  #if defined(RTS_CFG_FILE_ACCESS) || defined(RTS_CFG_FILE_NATIVE)	
	IEC_DATA	pBuffer[((VMM_MAX_IEC_STRLEN + HD_IEC_STRING) > (2 * VMM_MAX_PATH) ? (VMM_MAX_IEC_STRLEN + HD_IEC_STRING) : (2 * VMM_MAX_PATH)) + 2];
  #else
	IEC_DATA	pBuffer[VMM_MAX_IEC_STRLEN + HD_IEC_STRING];
  #endif

  #if defined(RTS_CFG_EXT_PROJ_INFO)
	SProjInfo	ProjInfo;				/* Extended project informations	*/
  #endif
	
  #if defined(RTS_CFG_ONLINE_CHANGE)
	SOnlChg 	OnlChg; 				/* Online Change Information		*/
  #endif

  #if defined(RTS_CFG_DEBUG_INFO)
	SDBIInfo	DBI;					/* DBI informations 				*/
  #endif

  #if defined(RTS_CFG_IO_LAYER)
	SIOLayer	pIOLayer[MAX_IO_LAYER]; /* IO layer information 			*/ 
  #endif

  #if defined(RTS_CFG_EVENTS)
	IEC_UINT	pVMEventCount[MAX_EVENTS];	/* VM's waiting on a specific event */
	IEC_UINT	*ppVMEvents[MAX_EVENTS];	/* VM's waiting on a specific event */
  #endif

  #if defined(RTS_CFG_BACNET)
	IEC_UINT	uResConfigBACnet;		/* BACnet configuration result		*/
  #endif

} STaskInfoVMM; 						/* -------------------------------- */


#define MASC_ALLOCATED		0x80u

#define vmm_max(a,b)	(((a) > (b)) ? (a) : (b))
#define vmm_min(a,b)	(((a) < (b)) ? (a) : (b))


/* File Access
 * ----------------------------------------------------------------------------
 */
#if defined(RTS_CFG_FILE_NATIVE)
	#define xxxClose			fileClose
	#define xxxWrite			fileWrite
	#define xxxRead 			fileRead
	#define xxxSeek 			fileSeek
	#define xxxReadLine 		fileReadLine
	#define xxxOpen 			fileOpen
	#define xxxExist			fileExist
	#define xxxCreateDir		fileCreateDir
	#define xxxRemove			fileRemove
	#define xxxRename			fileRename
#endif

#if defined(RTS_CFG_FILE_ACCESS)
	#define xxxClose			osClose
	#define xxxWrite			osWrite
	#define xxxRead 			osRead
	#define xxxSeek 			osSeek
	#define xxxReadLine 		osReadLine
	#define xxxOpen 			osOpen
	#define xxxExist			osExist
	#define xxxCreateDir		osCreateDir
	#define xxxRemove			osRemove
	#define xxxRename			osRename
#endif


/* Align code and instance blocks to even addresses
 * ----------------------------------------------------------------------------
 */
#if defined(IP_CFG_INST8)
  #define ALI(x)		( x )
#endif
#if defined(IP_CFG_INST16)
  #define ALI(x)		( (((IEC_UDINT)(x) & 1ul) == 0) ? (x) : (IEC_UDINT)(x) + 1ul )
#endif
#if defined(IP_CFG_INST32)
  #define ALI(x)		( (((IEC_UDINT)(x) & 3ul) == 0) ? (x) : (IEC_UDINT)(x) + (sizeof(IEC_DWORD) - ((IEC_UDINT)(x) & 3ul)) )
#endif
#if defined(IP_CFG_INST64)
  #define ALI(x)		( (((IEC_UDINT)(x) & 7ul) == 0) ? (x) : (IEC_UDINT)(x) + (sizeof(IEC_LWORD) - ((IEC_UDINT)(x) & 7ul)) )
#endif


/* IO layer extensions
 * ----------------------------------------------------------------------------
 */
#define IOEXT_TEST		"iotst"
#define IOEXT_BACNET		"iobac"
#define IOEXT_PROFIDP		"fcdp"
/* see also inc/osTarget.h */

#define IOID_TEST		1
#define IOID_BACNET 		2
#define IOID_PROFIDP		3
/* see also inc/osTarget.h */


/* Command state machine states
 * ----------------------------------------------------------------------------
 */
#define AS_IDLE 			0x00u
#define AS_FIRST_DATA		0x01u
#define AS_DATA_READY		0x02u
#define AS_PROC_DATA		0x03u
#define AS_SWITCH			0x04u
#define AS_RESP_READY		0x05u
#define AS_PROC_RESPONSE	0x06u


/* Synchronisation between state machines
 * ----------------------------------------------------------------------------
 */
#define SIG_DATA			0x00u
#define SIG_RESPONSE		0x01u
#define SIG_FIRST			0x02u
#define SIG_SWITCH			0x03u


/* Communication state machine states
 * ----------------------------------------------------------------------------
 */
#define CS_READY_TO_RECEIVE 0x00u
#define CS_RECEIVING		0x01u
#define CS_BLOCK_RECEIVED	0x02u
#define CS_DATA_RECEIVED	0x03u
#define CS_REQ_RECEIVED 	0x04u
#define CS_PROC_DATA		0x05u
#define CS_PROC_RESPONSE	0x06u
#define CS_SENDING			0x07u


/* Communication states
 * ----------------------------------------------------------------------------
 */
#define CIO_READY			0x00u			/* Comm. ready (no IO pending)	*/
#define CIO_DATA			0x01u			/* Data received				*/
#define CIO_SENDING 		0x02u			/* Currently sending data		*/
#define CIO_RECEIVING		0x03u			/* Currently receiving data 	*/
#define CIO_TIMEOUT 		0x04u			/* Time out occured 			*/
#define CIO_ERROR			0x05u			/* General communication error	*/


/* File IO modes
 * ---------------------------------------------------------------------------
 */
#define FIO_MODE_READ			0x00u	
#define FIO_MODE_WRITE			0x01u
#define FIO_MODE_APPEND 		0x02u
#define FIO_MODE_RDWR			0x03u

/* FIO_MODE_READ:	Opens for reading. If the file does not exist or cannot be 
 *					found, the fopen call fails.
 *
 * FIO_MODE_WRITE:	Opens an empty file for both reading and writing. If the 
 *					given file exists, its contents are destroyed.
 *
 * FIO_MODE_APPEND: Opens for both reading and writing. (The file must exist.)
 *
 * FIO_MODE_RDWR:	Opens for reading and appending; the appending operation 
 *					includes the removal of the EOF marker before new data is 
 *					written to the file and the EOF marker is restored after 
 *					writing is complete; creates the file first if it doesn’t 
 *					exist.
 */


/* Directory access modes
 * ---------------------------------------------------------------------------
 */
#define DIR_MODE_READ			0x00u
#define DIR_MODE_WRITE			0x01u


/* File seek modes
 * ----------------------------------------------------------------------------
 */
#define FSK_SEEK_CUR			0x00u
#define FSK_SEEK_END			0x01u
#define FSK_SEEK_SET			0x02u


/* IO Layer notification options
 * ----------------------------------------------------------------------------
 */
#define IO_NOTIFY_NONE			0x00u
#define IO_NOTIFY				0x10u

#define IO_NOTIFY_SYNC			0x01u
#define IO_NOTIFY_ASYNC 		0x02u


/* IO Layer states
 * ----------------------------------------------------------------------------
 */
#define IO_STATE_NONE			0x0000u
#define IO_STATE_CONFIG 		0x0001u
#define IO_STATE_OK 			0x0002u
#define IO_STATE_ERROR			0x8000u


/* Download states
 * ----------------------------------------------------------------------------
 */
#define DOWN_NONE				0x0000u

#define DOWN_BEGIN				0x1001u
#define DOWN_CLEAR				0x1002u
#define DOWN_CONFIG 			0x1004u
#define DOWN_CODE				0x1008u
#define DOWN_INIT				0x1010u
#define DOWN_CUSTOM 			0x1020u
#define DOWN_DEBUG				0x1040u
#define DOWN_CFGIOL 			0x1080u
#define DOWN_FINISH 			0x1100u

#define OCHG_BEGIN				0x2001u
#define OCHG_CONFIG 			0x2002u
#define OCHG_CODE				0x2004u
#define OCHG_INIT				0x2008u
#define OCHG_CUSTOM 			0x2010u
#define OCHG_DEBUG				0x2020u
#define OCHG_FINISH 			0x2040u
#define OCHG_COMMIT 			0x2080u

#define MASC_DOWN_FULL			0x1000u
#define MASC_DOWN_INCR			0x2000u


/* Task states
 * ----------------------------------------------------------------------------
 */
#define TASK_STATE_ERROR		0xffffffffu
#define TASK_STATE_ONCREATION	0x00000000u
#define TASK_STATE_STOPPED		0x00000001u
#define TASK_STATE_RUNNING		0x00000002u
#define TASK_STATE_BREAK		0x00000003u
#define TASK_STATE_CONTINUED	0x00000004u 	/* Not to be used in the VM! */
#define TASK_STATE_STEP 		0x00000005u
#define TASK_STATE_DELETED		0x00000006u
#define TASK_STATE_UNLOADED 	0x00000010u 	/* OSAI, no product usage!	 */ 	


/* Download modes
 * ----------------------------------------------------------------------------
 */
#define DOWN_MODE_FULL			0x01u
#define DOWN_MODE_INCR			0x02u


/* Task flags
 * ----------------------------------------------------------------------------
 */
#define TASK_FLAG_INITIALIZED	0x01u
#define TASK_FLAG_NEW_ERROR 	0x02u


/* Resource states
 * ----------------------------------------------------------------------------
 */
#define RES_STATE_NOTRUNNING	0xfffffffeu
#define RES_STATE_ERROR 		0xffffffffu
#define RES_STATE_ONCREATION	0x00000000u
#define RES_STATE_PAUSED		0x00000001u
#define RES_STATE_RUNNING		0x00000002u


/* Firmware update states
 * ----------------------------------------------------------------------------
 */
#define FW_UPD_READY			0x0000u
#define FW_UPD_IN_PROGRESS		0x0001u


/* Flash domain definition
 * ----------------------------------------------------------------------------
 */
#define MAX_FLASH_DOMAIN		0x000Au

#define FLASH_DOMAIN_BEGIN		0x0000u
#define FLASH_DOMAIN_CLEAR		0x0001u
#define FLASH_DOMAIN_CONFIG 	0x0002u
#define FLASH_DOMAIN_CODE		0x0003u
#define FLASH_DOMAIN_INIT		0x0004u
#define FLASH_DOMAIN_CUSTOM 	0x0005u
#define FLASH_DOMAIN_DEBUG		0x0006u
#define FLASH_DOMAIN_FINISH 	0x0007u
#define FLASH_DOMAIN_CFGIOL 	0x0008u
#define FLASH_DOMAIN_END		0x0009u


/* Min. and Max. IEC values
 * ----------------------------------------------------------------------------
 */
#define IEC_MAX_SINT			(+127)
#define IEC_MIN_SINT			(-IEC_MAX_SINT-1)

#define IEC_MAX_INT 			(+32767)
#define IEC_MIN_INT 			(-IEC_MAX_INT-1)

#define IEC_MAX_DINT			(+2147483647l)
#define IEC_MIN_DINT			(-IEC_MAX_DINT-1)


/* Special value signaling an empty entry
 * ----------------------------------------------------------------------------
 */
#define NO_INSTANCE 			((IEC_UINT) 0xFFFFu)
#define NO_INDEX				((IEC_UINT) 0xFFFFu)
#define NO_DEBUG_CONN			((IEC_USINT)0xFFu)


/* Semaphore ID's
 * ----------------------------------------------------------------------------
 */
#define SEM_BP_LIST 		0x00u		/* Breakpoint list					*/
#define SEM_BP_QUEUE		0x01u		/* BP notification queue			*/
#define SEM_MSG_QUEUE		0x02u		/* Msg notification queue			*/
#define SEM_TASK_IMAGE		0x03u		/* Access to task image 			*/
#define SEM_RETAIN			0x04u		/* Access to retain segment 		*/
#define SEM_COMMUNICATION	0x05u		/* Synchronization with comm. task	*/
#define SEM_FILE_LIBRARY	0x06u		/* File system access library		*/

#if defined(RTS_CFG_BACNET)
#define SEM_BAC_DEVICE		0x07u		/* BACnet device objects			*/
#define SEM_BAC_LIB 		0x08u		/* BACnet library handling			*/
#define SEM_BAC_OBJECT		0x09u		/* BACnet object handling			*/
#define SEM_BAC_FLASH		0x0Au		/* BACnet property persistence		*/
#define SEM_BAC_STACK		0x0Bu		/* BACnet stack access				*/
#endif

#if defined(RTS_CFG_BACNET)
#define MAX_SEMAPHORE		0x0Cu		/* Count of semaphores				*/
#else
#define MAX_SEMAPHORE		0x07u		/* Count of semaphores				*/
#endif


/* Message Queue ID's
 * ----------------------------------------------------------------------------
 */
#define IPC_Q_NONE			0xffffu
#define IPC_VMM_QUEUES		0x15u

#define Q_LIST_VMM			(MAX_TASKS + 0x00u) /* VMM listening queue		*/
#define Q_LIST_OC			(MAX_TASKS + 0x01u) /* OC  listening queue		*/
#define Q_LIST_VTI			(MAX_TASKS + 0x02u) /* VTI listening queue		*/
#define Q_LIST_RET			(MAX_TASKS + 0x03u) /* RET listening queue		*/
#define Q_LIST_DEV			(MAX_TASKS + 0x04u) /* DEV listening queue		*/
#define Q_LIST_COV			(MAX_TASKS + 0x05u) /* COV listening queue		*/
#define Q_LIST_SCN			(MAX_TASKS + 0x06u) /* SCN listening queue		*/
#define Q_LIST_FLH			(MAX_TASKS + 0x07u) /* SCN listening queue		*/
#define Q_LIST_MGT			(MAX_TASKS + 0x08u) /* MGT listening queue		*/
#define Q_LIST_CFG			(MAX_TASKS + 0x09u) /* CFG listening queue		*/

#define Q_RESP_VMM_VM		(MAX_TASKS + 0x0Au) /* VM->VMM	response queue	*/
#define Q_RESP_VMM_OC		(MAX_TASKS + 0x0Bu) /* OC->VMM	response queue	*/
#define Q_RESP_VMM_VTI		(MAX_TASKS + 0x0Cu) /* TI->VMM	response queue	*/
#define Q_RESP_VMM_RET		(MAX_TASKS + 0x0Du) /* RET->VMM response queue	*/
#define Q_RESP_VMM_IO		(MAX_TASKS + 0x0Eu) /* IO->VMM	response queue	*/

#define Q_RESP_IO_DEV		(MAX_TASKS + 0x0Fu) /* DEV->IO	response queue	*/
#define Q_RESP_IO_COV		(MAX_TASKS + 0x10u) /* COV->IO	response queue	*/
#define Q_RESP_IO_SCN		(MAX_TASKS + 0x11u) /* SCN->IO	response queue	*/
#define Q_RESP_IO_FLH		(MAX_TASKS + 0x12u) /* FLH->IO	response queue	*/
#define Q_RESP_IO_CFG		(MAX_TASKS + 0x13u) /* CFG->IO	response queue	*/

#define Q_RESP_IO_MGT		(MAX_TASKS + 0x14u) /* MGT->IO	response queue	*/

#define Q_OFFS_IO			(MAX_TASKS + IPC_VMM_QUEUES + MAX_CONNECTIONS)

/* Count of message queues	
 */
#define MAX_IPC_QUEUE		(MAX_TASKS + IPC_VMM_QUEUES + MAX_CONNECTIONS + MAX_IO_LAYER)


/* Messages
 * ----------------------------------------------------------------------------
 */
#define MSG_ERROR_FLAG			0x8000u 	/* Error response to any mesage */

/* VMM --> VM
 */
#define MSG_VM_START			0x0010u 	/* Start a given VM task		*/
#define MSG_VM_STOP 			0x0011u 	/* Stop a given VM task 		*/
#define MSG_VM_CONTINUE 		0x0012u 	/* Continue VM with reached BP	*/
#define MSG_VM_STEP 			0x0013u 	/* Single step					*/
#define MSG_VM_RESET			0x0014u 	/* Resets a given VM task		*/
#define MSG_VM_TERMINATE		0x0015u 	/* Terminate VM task			*/

/* Timer/Event -> VM
 */
#define MSG_TIMER				0x0020u 	/* Timer triggered (Execute IP) */
#define MSG_EVENT				0x0021u 	/* Event occurred (Execute IP)	*/

/* CT -> VMM
 */
#define MSG_CT_DATA 			0x0040u 	/* Data received in comm. task	*/
#define MSG_CT_TERM 			0x0041u 	/* The CT terminates			*/

/* VMM -> OC
 */
#define MSG_OC_PREPARE			0x0080u 	/* Prepare for online change	*/
#define MSG_OC_COMMIT			0x0081u 	/* Commit online change 		*/

/* VMM -> IO
 */
#define MSG_IO_CONFIG			0x0100u 	/* Configuration information	*/
#define MSG_IO_START			0x0101u 	/* Start IO layer				*/
#define MSG_IO_STOP 			0x0102u 	/* Stop IO layer				*/
#define MSG_IO_NOTIFY_SET		0x0103u 	/* PI (%Q) has been changed 	*/
#define MSG_IO_NOTIFY_GET		0x0104u 	/* PI (%I,%Q) will be read		*/
#define MSG_IO_TERMINATE		0x0105u 	/* Terminate IO layer			*/
#define MSG_IO_CONFIG_RES		0x0106u 	/* Configuration result 		*/
#define MSG_IO_TERM_RES 		0x0107u 	/* Termination result			*/

/* IO -> VMM
 */
#define MSG_IO_DONE 			0x01feu 	/* Configuration done (async!)	*/
#define MSG_IO_EVENT			0x01ffu 	/* IO layer raises VM event 	*/

/* VMM --> Timer
 */
#define MSG_TI_CONFIG			0x0200u 	/* Send configuration info		*/
#define MSG_TI_START			0x0201u 	/* Start VM task execution		*/
#define MSG_TI_STOP 			0x0202u 	/* Stop VM task execution		*/
#define MSG_TI_TERMINATE		0x0203u 	/* Terminate timer task 		*/

/* VM -> VMM
 */
#define MSG_VM_COLDSTART		0x0400u 	/* Request a cold start 		*/
#define MSG_VM_WARMSTART		0x0401u 	/* Request a warm start 		*/
#define MSG_VM_REBOOT			0x0402u 	/* Request a reboot 			*/

/* VMM -> RET
 */
#define MSG_RT_OPEN 			0x0800u 	/* Open and config retain file	*/
#define MSG_RT_CLOSE			0x0801u 	/* Close retain file			*/
#define MSG_RT_START			0x0802u 	/* Start retain updating		*/
#define MSG_RT_STOP 			0x0803u 	/* Stop retain updating 		*/
#define MSG_RT_UPDATE			0x0804u 	/* Update once now				*/
#define MSG_RT_SET_GUID 		0x0805u 	/* Update project GUID			*/
#define MSG_RT_SET_SIZE 		0x0806u 	/* Update used retain sizeof	*/
#define MSG_RT_SET_CYCLE		0x0807u 	/* Set retain write cycle time	*/

/* VMM -> FW
 */
#define MSG_FW_STATE			0x0900u 	/* Get firmware update state	*/
#define MSG_FW_EXECUTE			0x0901u 	/* Execute firmware update		*/

/* IOBAC -> COV
 */
#define MSG_CV_START			0x0A00u 	/* Start COV subscriptions		*/
#define MSG_CV_STOP 			0x0A01u 	/* Stop COV subscriptions		*/
#define MSG_CV_TERMINATE		0x0A03u 	/* Terminate COV sub. task		*/

/* IOBAC -> SCAN
 */
#define MSG_SC_START			0x0B00u 	/* Start IEC change scanning	*/
#define MSG_SC_STOP 			0x0B01u 	/* Stop IEC change scanning 	*/
#define MSG_SC_TERMINATE		0x0B03u 	/* Terminate scan task			*/

/* IOBAC -> DEV
 */
#define MSG_DV_START			0x0C00u 	/* Start device scanning		*/
#define MSG_DV_STOP 			0x0C01u 	/* Stop device scanning 		*/
#define MSG_DV_TERMINATE		0x0C02u 	/* Terminate device task		*/

/* IOBAC -> FLH
 */
#define MSG_FL_START			0x0D00u 	/* Start flashing				*/
#define MSG_FL_STOP 			0x0D01u 	/* Stop flashing				*/
#define MSG_FL_TERMINATE		0x0D03u 	/* Terminate flash task 		*/

/* IOBAC -> CFG
 */
#define MSG_CG_CONFIG			0x0E01u 	/* Config. data for BACnet. 	*/
#define MSG_CG_TERMINATE		0x0E02u 	/* Terminate BACnet config. 	*/

/* CFG -> IOBAC
 */
#define MSG_CG_CONF_DONE		0x0EF0u 	/* BAC configuration completed	*/
#define MSG_CG_TERM_DONE		0x0EF1u 	/* BAC termination completed	*/

/* IODP -> MGT
 */
#define MSG_PB_CONFIG			0x0F01u 	/* Config. data for PG mgt. 	*/
#define MSG_PB_TERMINATE		0x0F02u 	/* Terminate PB management		*/

/* MGT -> IODP
 */
#define MSG_PB_CONF_DONE		0x0FF0u 	/* PB configuration completed	*/
#define MSG_PB_TERM_DONE		0x0FF1u 	/* PB termination completed 	*/


/* Events
 * ----------------------------------------------------------------------------
 */

/* System Events
 */
#define EVT_IO_DRIVER_ERROR 	0u	/* General IO driver error				*/	  
#define EVT_POWER_FAIL			1u	/* Power fail							*/
#define EVT_EXCEPTION			2u	/* General IEC task exception			*/
#define EVT_BOOTED				3u	/* System has booted					*/

#define EVT_RESERVED_00 		4u	/* --- Reserved 						*/
#define EVT_RESERVED_01 		5u	/* --- Reserved 						*/
#define EVT_RESERVED_02 		6u	/* --- Reserved 						*/
#define EVT_RESERVED_03 		7u	/* --- Reserved 						*/
#define EVT_RESERVED_04 		8u	/* --- Reserved 						*/
#define EVT_RESERVED_05 		9u	/* --- Reserved 						*/

/* Engineering command events
 */
#define EVT_DOWNLOAD_BEGIN		10u /* Download command received			*/
#define EVT_DOWNLOAD_END		11u /* Download finished, resource started	*/
#define EVT_ONLCHG_BEGIN		12u /* Online Change command received		*/
#define EVT_ONLCHG_END			13u /* Online Change finished, res. started */
#define EVT_BEFORE_COLDSTART	14u /* Coldstart command received			*/
#define EVT_BEFORE_WARMSTART	15u /* Warmstart command received			*/
#define EVT_AFTER_COLDSTART 	16u /* Coldstart executed					*/
#define EVT_AFTER_WARMSTART 	17u /* Warmstart executed					*/
#define EVT_START_RESOURCE		18u /* Start Resource command received		*/
#define EVT_STOP_RESOURCE		19u /* Stop Resource command received		*/
#define EVT_START_TASK			20u /* Start Task command executed			*/
#define EVT_STOP_TASK			21u /* Stop Task command executed			*/
#define EVT_INITIALIZE			22u /* Initialize/Clear command received	*/

#define EVT_BACNET_EVENT		23u /* BACnet event received				*/
#define EVT_BACNET_DEVSTATE 	24u /* BACnet device changed state			*/
#define EVT_BACNET_CONFIG		25u /* BACnet configuration result event	*/
#define EVT_RESERVED_10 		26u /* --- Reserved 						*/
#define EVT_RESERVED_11 		27u /* --- Reserved 						*/
#define EVT_RESERVED_12 		28u /* --- Reserved 						*/
#define EVT_RESERVED_13 		29u /* --- Reserved 						*/
#define EVT_RESERVED_14 		30u /* --- Reserved 						*/
#define EVT_RESERVED_15 		31u /* --- Reserved 						*/

/* User events (target dependent interpretation)
 */
#define EVT_USER_01 			32u /* --- User Event						*/
#define EVT_USER_02 			33u /* --- User Event						*/
#define EVT_USER_03 			34u /* --- User Event						*/
#define EVT_USER_04 			35u /* --- User Event						*/
#define EVT_USER_05 			36u /* --- User Event						*/
#define EVT_USER_06 			37u /* --- User Event						*/
#define EVT_USER_07 			38u /* --- User Event						*/
#define EVT_USER_08 			39u /* --- User Event						*/
#define EVT_USER_09 			40u /* --- User Event						*/
#define EVT_USER_10 			41u /* --- User Event						*/
#define EVT_USER_11 			42u /* --- User Event						*/
#define EVT_USER_12 			43u /* --- User Event						*/
#define EVT_USER_13 			44u /* --- User Event						*/
#define EVT_USER_14 			45u /* --- User Event						*/
#define EVT_USER_15 			46u /* --- User Event						*/
#define EVT_USER_16 			47u /* --- User Event						*/
#define EVT_USER_17 			48u /* --- User Event						*/
#define EVT_USER_18 			49u /* --- User Event						*/
#define EVT_USER_19 			50u /* --- User Event						*/
#define EVT_USER_20 			51u /* --- User Event						*/
#define EVT_USER_21 			52u /* --- User Event						*/
#define EVT_USER_22 			53u /* --- User Event						*/
#define EVT_USER_23 			54u /* --- User Event						*/
#define EVT_USER_24 			55u /* --- User Event						*/
#define EVT_USER_25 			56u /* --- User Event						*/
#define EVT_USER_26 			57u /* --- User Event						*/
#define EVT_USER_27 			58u /* --- User Event						*/
#define EVT_USER_28 			59u /* --- User Event						*/
#define EVT_USER_29 			60u /* --- User Event						*/
#define EVT_USER_30 			61u /* --- User Event						*/
#define EVT_USER_31 			62u /* --- User Event						*/
#define EVT_USER_32 			63u /* --- User Event						*/


/* Task/Thread Names
 * ----------------------------------------------------------------------------
 */
#define TASK_NAME_SYS_VMM		"SYS_VMM"
#define TASK_NAME_SYS_TIM		"SYS_TIM"
#define TASK_NAME_SYS_OCH		"SYS_OCH"
#define TASK_NAME_SYS_RET		"SYS_RET"
#define TASK_NAME_SYS_LED		"SYS_LED"

#define TASK_NAME_IOL_BAC		"IOL_BAC"
#define TASK_NAME_IOL_TST		"IOL_TST"
#define TASK_NAME_IOL_MTS		"IOL_MTS"
#define TASK_NAME_IOL_MRC		"IOL_MRC"
#define TASK_NAME_IOL_PDP		"IOL_PDP"

#define TASK_NAME_BAC_DEV		"BAC_DEV"
#define TASK_NAME_BAC_SCN		"BAC_SCN"
#define TASK_NAME_BAC_COV		"BAC_COV"
#define TASK_NAME_BAC_FLH		"BAC_FLH"
#define TASK_NAME_BAC_CFG		"BAC_CFG"

#define TASK_NAME_BAC_MGT		"BAC_MGT"
#define TASK_NAME_BAC_COM		"BAC_COM"

#define TASK_NAME_PDP_WRK		"PDP_WRK"
#define TASK_NAME_PDP_MGT		"PDP_MGT"

#define TASK_NAME_COM_LIS		"COM_LIS"
#define TASK_NAME_COM_WRK		"COM_W%02d"

#define TASK_NAME_IEC_VM		"IEC_V%02d"


/* Task ID's. 
 * ----------------------------------------------------------------------------
 * If order is changed, vmmLoad.c must be adapted too!
 */
#define TASK_OFFS_SYS_VMM		0
#define TASK_OFFS_SYS_TIM		1
#define TASK_OFFS_SYS_OCH		2
#define TASK_OFFS_SYS_RET		3
#define TASK_OFFS_SYS_LED		4
#define TASK_OFFS_COM_LIS		5

#define TASK_OFFS_IOL_BAC		6
#define TASK_OFFS_IOL_TST		7
#define TASK_OFFS_IOL_PDP		8

#define TASK_OFFS_BAC_DEV		9
#define TASK_OFFS_BAC_SCN		10
#define TASK_OFFS_BAC_COV		11
#define TASK_OFFS_BAC_FLH		12
#define TASK_OFFS_BAC_CFG		13

#define TASK_OFFS_BAC_MGT		14
#define TASK_OFFS_BAC_COM		15

#define TASK_OFFS_PDP_MGT		16
#define TASK_OFFS_PDP_WRK		17

#define TASK_OFFS_IOL_CAN		18
#define TASK_OFFS_IOL_UDP		19
#define TASK_OFFS_IOL_DAT		20
#define TASK_OFFS_IOL_SYN		21
#define TASK_OFFS_IOL_MTS		22
#define TASK_OFFS_IOL_MRC		23
#define TASK_OFFS_IOL_KPD		24

#define TASK_OFFS_COM_WRK		25

#define TASK_OFFS_IEC_VM		(TASK_OFFS_COM_WRK + MAX_CONNECTIONS)

#define TASK_MAX_TASK			(TASK_OFFS_COM_WRK + MAX_CONNECTIONS + MAX_TASKS)


/* Load Display Types
 * ----------------------------------------------------------------------------
 */
#define LOAD_LOAD_AVERAGE		1			/* FC/BC load average			*/
#define LOAD_MEM_OVERAL 		2			/* Memory usage whole FC/BC 	*/
#define LOAD_CPU_SYSTEM 		3			/* CPU usage whole FC/BC		*/
   
#define LOAD_CPU_TASK			4			/* CPU usage of a spec. task	*/
#define LOAD_MEM_TASK			5			/* Memory usage of a spec. task */

#define LOAD_CPU_IEC			6			/* CPU - All VM/IEC tasks		*/
#define LOAD_CPU_IO_BAC 		7			/* CPU - BACnet IO layer		*/
#define LOAD_CPU_IO_PDP 		8			/* CPU - Profibus DP IO layer	*/
#define LOAD_CPU_SYS			9			/* CPU - System tasks			*/

#define LOAD_CPU_BAC			10			/* CPU - BACnet Driver			*/
#define LOAD_CPU_RTS			11			/* CPU - Whole Run Time System	*/


/* GUID Tracing
 * ----------------------------------------------------------------------------
 */
#if 1
#define TRACE_GUID(h,t,ptr)
#else
#define TRACE_GUID(h,t,ptr) 																	\
{																								\
	IEC_DATA *p = ptr;																			\
	osTrace("%-12s %s: {%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x}\r\n", \
			h, t,																				\
			p[0], p[1], p[2],  p[3],  p[4],  p[5],	p[6],  p[7],								\
			p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);								\
}
#endif


/* Error strings
 * ----------------------------------------------------------------------------
 * Note: The following strings are mapped to the exception numbers defined
 * above. Always change them together!
 */
#define TERR_S_EXCEPTION1	"[%s]: FATAL ERROR B%04ld: %s {POU:0x%04x Off:0x%04x Inst:0x%04x}\r\n"
#define TERR_S_EXCEPTION2	"[%s]: FATAL ERROR B%04ld: %s\r\n"
#define EXCEPTION_TEXT {											\
							"Illegal Opcode",						\
							"Stack Overflow",						\
							"Stack Underflow",						\
							"Division By Zero", 					\
							"Array Range",							\
							"String Length",						\
							"Null Pointer", 						\
							"Call Stack Overflow",					\
							"Invalid Library Function/FB Call", 	\
							"Invalid Library Argument", 			\
							"Mathematic (Invalid Argument)",		\
							"Watchdog Triggered",					\
							"Task Execution Time Overrun",			\
							"User Exception",						\
							"Task Image Access",					\
							"Timer Activation", 					\
							"Task Initialization",					\
							"Task in breakpoint when leaving Debug Mode",\
							"Unknown Exception" 					\
						}	


#define CMD_TEXT_VMM {						\
						"State",			\
	/* max. 8 chars */	"NYI",				\
						"Login",			\
						"Logout",			\
						"WrmStart", 		\
						"CldStart", 		\
						"StartAll", 		\
						"StopAll",			\
						"StartTsk", 		\
						"StopTask", 		\
						"OpenDbg",			\
						"CloseDbg", 		\
						"SetBP",			\
						"ClearBP",			\
						"ClearAll", 		\
						"SinglStp", 		\
						"Continue", 		\
						"GetValue", 		\
						"SetValue", 		\
						"DwnConf",			\
						"DwnInit",			\
						"DwnCode",			\
						"DwnCust",			\
						"DwnFin",			\
						"StartRes", 		\
						"StopRes",			\
						"DwnEnd",			\
						"DwnBegin", 		\
						"InitClr",			\
						"ClrFlash", 		\
						"DBIGChil", 		\
						"DBIGAddr", 		\
						"DwnDebug", 		\
						"GetTskNr", 		\
						"GetPrjVr", 		\
						"GetPrjIn", 		\
						"LoadPrj",			\
						"SavePrj",			\
						"Custom",			\
						"LoadFile", 		\
						"SaveFile", 		\
						"DirCont",			\
						"OcBegin",			\
						"OcCode",			\
						"OcDebug",			\
						"OcCustom", 		\
						"OcInit",			\
						"OcCommit", 		\
						"OcEnd",			\
						"OcConfig", 		\
						"OcFinish", 		\
						"Flash",			\
						"DelFile",			\
						"GetConf",			\
						"RetWrite", 		\
						"RetCycle", 		\
						"FWDown",			\
						"FWExec",			\
						"FWResult", 		\
						"RetUpld",			\
						"RetDown",			\
						"IECUpld",			\
						"IECDown",			\
						"GetIKey",			\
						"SetLKey",			\
						"GetSNo",			\
						"GetFeat",			\
						"GetType",			\
						"GetVers",			\
/* max. 8 chars */		"SetLicEx", 		\
						"ConfigIO", 		\
						"ClearCtl"			\
					}

#endif /* _VMMDEF_H_ */

/* ---------------------------------------------------------------------------- */
