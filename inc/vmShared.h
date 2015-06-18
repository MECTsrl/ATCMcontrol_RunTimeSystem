
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
 * Filename: vmShared.h
 */


#ifndef _VMSHARED_H_
#define _VMSHARED_H_

/* vmShared.h
 * ----------------------------------------------------------------------------
 * This file is supposed to be included both on the PC (CG, OPC) and on the 
 * run time system side and includes the communication protocoll structures and
 * some common definitions.
 *
 * Protocoll structures must begin with an 'X' and are supposed to be two byte
 * alligned. Don't add anything not commonly shared between the the PC components 
 * and the run time system.
 *
 * IMPORTANT:	Always ensure, that the structures of this file are compile with 
 * ==========	an one byte aligment both on the Engineering as on the Run Time
 *				System side!
 */

#if ! defined(INC_RTS)

  /* Include in a windows component. Must only be included from within vmSharedDef.h
   * in order to assure the correct alignement settings.
   */

  #if !defined(_VMSHAREDDEF_H_)
	#error "Include via vmSharedDef.h only!"
  #endif

  /* Structures in this file MUST be compiled with one byte alignment!
   */
  #pragma pack(1)

#else

  /* Included in the Run Time System. Must only be included via stdInc.h in order
   * to have the correct include order.
   */
  #if !defined(_STDINC_H_)
	#error "Include via stdInc.h only!"
  #endif

  /* Structures in this file MUST be compiled with one byte alignment!
   */
  #define RTS_PRAGMA_PACK_1
  #include "osAlign.h"
  #undef  RTS_PRAGMA_PACK_1

#endif



/* Download version
 * ----------------------------------------------------------------------------
 * Version control prior to version V2.05. Starting with version V2.05 the 
 * corresponding firmware version must be used!
 *
 * Format: 0xAAAABBBB	AAAA: Last change in download format
 *						BBBB: Last change in code generation
 */
#define VM_DOWNLOAD_VERSION   0x23072307ul	/* O B S O L E T  -  DON'T USE! */



/* Communication structures 
 * ----------------------------------------------------------------------------
 */

/* Command/Response
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_BYTE,	byCommand); 	/* Command or response ID			*/
	DEC_VAR(IEC_BYTE,	bySequence);	/* Sequence number					*/

	DEC_VAR(IEC_DATA,	pData[MAX_DATA]);	/* Data buffer					*/

} XCommand; 							/* -------------------------------- */

#define HD_COMMAND		(2 * sizeof(IEC_BYTE))

/* Command/Response Block
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UINT,	uBlock);		/* Block number 					*/
	DEC_VAR(IEC_UINT,	uLen);			/* Data length						*/
	DEC_VAR(IEC_BYTE,	byLast);		/* Last block in sequence			*/
	DEC_VAR(IEC_USINT,	usSource);		/* Block source - VMM intern!		*/
	
	DEC_VAR(XCommand,	CMD);			/* Command / Response				*/

} XBlock;								/* -------------------------------- */

#define HD_BLOCK		(2 * sizeof(IEC_UINT) + 2 * sizeof(IEC_BYTE) + HD_COMMAND)

/* Block Frame
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_BYTE,	byType);		/* Block type						*/
	DEC_VAR(IEC_BYTE,	byCRC); 		/* Check sum						*/
	 
	DEC_VAR(XBlock, 	BLK);			/* Command / Response block 		*/

} XFrame;								/* -------------------------------- */

#define HD_FRAME		(2 * sizeof(IEC_BYTE) + HD_BLOCK)


/* Monitoring Structures
 * ----------------------------------------------------------------------------
 * Starting from 2.1.0, the XVariable structure is only used within the XValue 
 * structure for writing values (SET_VALUE). For reading variables (GET_VALUE)
 * the corresponding S,M,L XVariable structures are used..
 */

/* Variable Description (< 2.1.0)
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT,	ulOffset);		/* Offset in the segment			*/
	DEC_VAR(IEC_UINT,	uLen);			/* Size of var. in bytes, 1 for bit */
	DEC_VAR(IEC_UINT,	uSegment);		/* Segment number of the variable	*/
	DEC_VAR(IEC_BYTE,	byBit); 		/* Bit offset						*/
	DEC_VAR(IEC_BYTE,	byDummy);		/* Alignment - Dummy				*/

} XVariable;							/* -------------------------------- */

/* Variable Description (>= 2.1.0)
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_USINT,	usType);		/* Type (S,M,L) and bit offset		*/
	DEC_VAR(IEC_USINT,	usSegment); 	/* Segment number of the variable	*/
	DEC_VAR(IEC_USINT,	usOffset);		/* Offset in the segment			*/
	DEC_VAR(IEC_USINT,	usLen); 		/* Size of var. in bytes, 1 for bit */

} XVariableS;							/* -------------------------------- */

typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_USINT,	usType);		/* Type (S,M,L) and bit offset		*/
	DEC_VAR(IEC_USINT,	usSegment); 	/* Segment number of the variable	*/
	DEC_VAR(IEC_UINT,	uOffset);		/* Offset in the segment			*/
	DEC_VAR(IEC_UINT,	uLen);			/* Size of var. in bytes, 1 for bit */

} XVariableM;							/* -------------------------------- */

typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_USINT,	usType);		/* Type (S,M,L) and bit offset		*/
	DEC_VAR(IEC_BYTE,	byDummy);		/* Alignment - Dummy				*/
	DEC_VAR(IEC_UINT,	uLen);			/* Size of var. in bytes, 1 for bit */
	DEC_VAR(IEC_UDINT,	ulOffset);		/* Offset in the segment			*/
	DEC_VAR(IEC_UINT,	uSegment);		/* Segment number of the variable	*/

} XVariableL;							/* -------------------------------- */


/* Value Description
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(XVariable,	VAR);			/* Variable description 			*/

	DEC_VAR(IEC_DATA,	pValue[2]); 	/* Placeholder for variable value	*/

} XValue;								/* -------------------------------- */

#define 	HD_VALUE	(sizeof(XVariable))


/* Breakpoint Information
 * ----------------------------------------------------------------------------
 */

/* Breakpoint Definition - Downloaded from OPC-Server
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UINT,	uCode); 		/* Code index of breakpoint 		*/
	DEC_VAR(IEC_UINT,	uInst); 		/* Instance index of breakpoint 	*/	
	DEC_VAR(IEC_UDINT,	ulCodePos); 	/* Code position of breakpoint		*/
	DEC_VAR(IEC_UINT,	uType); 		/* Breakpoint type					*/

} XBreakpoint;							/* -------------------------------- */

/* Breakpoint Notification - Uploaded to the OPC - server
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(XBreakpoint,	BP);		/* Breakpoint definition			*/
	DEC_VAR(IEC_UINT,		uTask); 	/* Concerned task					*/
	DEC_VAR(IEC_UINT,		uState);	/* Breakpoint state 				*/

} XBPNotification;						/* -------------------------------- */


/* Index Definition
 * ----------------------------------------------------------------------------
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UINT,	uCode); 		/* Index: Code block list			*/
	DEC_VAR(IEC_UINT,	uInst); 		/* Index: Object list (instance)	*/

} XIndex;								/* -------------------------------- */


/* Task definition
 * ----------------------------------------------------------------------------
 */
#if ! defined(INC_RTS)

typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT,	ulInterval);	/* Task interval time				*/
	DEC_VAR(IEC_UINT,	uPriority); 	/* Task priority					*/
	DEC_VAR(IEC_BYTE,	byPrograms);	/* Count of programs in task		*/
	DEC_VAR(IEC_BYTE,	byStart);		/* Start interpr. after download	*/
	DEC_VAR(IEC_UINT,	uRegionsRd);	/* No. of memory read regions		*/
	DEC_VAR(IEC_UINT,	uRegionsWr);	/* No. of memory write regions		*/
	DEC_VAR(IEC_DATA,	szName[33]);	/* Taskname 			*/
	DEC_VAR(IEC_BYTE,	byDummy);		/* alignment byte					*/
										/* use only for versions < V2.05	*/
} XTask_200;							/* -------------------------------- */

#endif /* ! INC_RTS */

#if ! defined(INC_RTS)

typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT,	ulInterval);	/* Task interval time				*/
	DEC_VAR(IEC_UINT,	uPriority); 	/* Task priority					*/
	DEC_VAR(IEC_BYTE,	byPrograms);	/* Count of programs in task		*/
	DEC_VAR(IEC_BYTE,	byStart);		/* Start interpr. after download	*/
	DEC_VAR(IEC_UINT,	uRegionsRd);	/* No. of memory read regions		*/
	DEC_VAR(IEC_UINT,	uRegionsWr);	/* No. of memory write regions		*/
	DEC_VAR(IEC_UINT,	uCpyRegions);	/* Count of CopyRegions for the task*/
	DEC_VAR(IEC_UINT,	uCpyRegOff);	/* offset of CopyRegions for the task*/
	DEC_VAR(IEC_DATA,	szName[33]);	/* Taskname 			*/
	DEC_VAR(IEC_BYTE,	byDummy);		/* alignment byte					*/
										/* use only for V2.05 or higher 	*/
} XTask_205;							/* -------------------------------- */

#endif /* ! INC_RTS */

#if ! defined(INC_RTS)

typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_USINT,	usPrograms);	/* Count of programs in task		*/
	DEC_VAR(IEC_USINT,	usAttrib);		/* Task flags						*/
	DEC_VAR(IEC_UDINT,	ulPara1);		/* Task parameter #1				*/
	DEC_VAR(IEC_UDINT,	ulPara2);		/* Task parameter #2				*/
	DEC_VAR(IEC_UINT,	uRegionsRd);	/* No. of memory read regions		*/
	DEC_VAR(IEC_UINT,	uRegionsWr);	/* No. of memory write regions		*/
	DEC_VAR(IEC_UINT,	uCpyRegions);	/* Count of CopyRegions for the task*/
	DEC_VAR(IEC_UINT,	uCpyRegOff);	/* offset of CopyRegions for the task*/
	DEC_VAR(IEC_DATA,	szName[33]);	/* Taskname 			*/
	DEC_VAR(IEC_USINT,	usPriority);	/* Task priority					*/

										/* use only for V2.07 or higher 	*/
} XTask_207;							/* -------------------------------- */

#endif /* ! INC_RTS */
										
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_USINT,	usPrograms);	/* Count of programs in task		*/
	DEC_VAR(IEC_USINT,	usAttrib);		/* Task flags						*/
	DEC_VAR(IEC_UDINT,	ulPara1);		/* Task parametre #1				*/
	DEC_VAR(IEC_UDINT,	ulPara2);		/* Task parameter #2				*/
	DEC_VAR(IEC_UDINT,	ulWatchDogMS);	/* Task watchdog, 0 if no watchdog	*/
	DEC_VAR(IEC_UINT,	uRegionsRd);	/* No. of memory read regions		*/
	DEC_VAR(IEC_UINT,	uRegionsWr);	/* No. of memory write regions		*/
	DEC_VAR(IEC_UINT,	uCpyRegions);	/* Count of CopyRegions for the task*/
	DEC_VAR(IEC_UINT,	uCpyRegOff);	/* offset of CopyRegions for the task*/
	DEC_VAR(IEC_DATA,	szName[33]);	/* Taskname 						*/
	DEC_VAR(IEC_USINT,	usPriority);	/* Task priority					*/

										/* use only for V2.10.01 or higher	*/
} XTask_21001;							/* -------------------------------- */


/* Object Definition
 * ----------------------------------------------------------------------------
 */
typedef struct		/* --> aligned (2) */
{
	DEC_PTR(IEC_DATA,	pAdr);			/* Start address of object			*/
	DEC_VAR(IEC_UDINT,	ulSize);		/* Size of object					*/

} XObject;								/* -------------------------------- */


/* Memory region definition (relative to a segment address)
 * ----------------------------------------------------------------------------
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT,	ulOffset);		/* offset relative to segm address	*/
	DEC_VAR(IEC_UINT,	uSize); 		/* Size of region					*/
	DEC_VAR(IEC_BYTE,	bySegment); 	/* Segment of region				*/
	DEC_VAR(IEC_BYTE,	bRead); 		/* Region is for reading/writing	*/

} XMemRegion;							/* -------------------------------- */


/* Project Data
 * ----------------------------------------------------------------------------
 */
#if ! defined(INC_RTS)

typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT,	ulBinDLVersion);/* the version of the binary		*/
										/* download format					*/
	DEC_VAR(IEC_UDINT,	ulStateVarOffs);/* byte offset for __state variable */
	DEC_VAR(IEC_UINT,	uTasks);		/* Count of tasks					*/
	DEC_VAR(IEC_UINT,	uCodeObjects);	/* Count of Code blocks (POU's) 	*/
	DEC_VAR(IEC_UINT,	uInstObjects);	/* Count of objects (instances) 	*/
	DEC_VAR(IEC_UINT,	uDirectVars);	/* Count of direct variables		*/
	DEC_VAR(IEC_UDINT,	ulIECOffset);	/* Offset of IEC vars in GDM		*/
	DEC_VAR(IEC_UDINT,	ulIECSize); 	/* Size of IEC vars in GDM			*/
	DEC_VAR(IEC_UINT,	uDebugCode);	/* Project is prepared for debugging*/

} XProject_200; 						/* -------------------------------- */

typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT,	ulBinDLVersion);/* the version of the binary		*/
										/* download format					*/
	DEC_VAR(IEC_UDINT,	ulStateVarOffs);/* byte offset for __state variable */
	DEC_VAR(IEC_UINT,	uTasks);		/* Count of tasks					*/
	DEC_VAR(IEC_UINT,	uCodeObjects);	/* Count of Code blocks (POU's) 	*/
	DEC_VAR(IEC_UINT,	uInstObjects);	/* Count of objects (instances) 	*/
	DEC_VAR(IEC_UINT,	uDirectVars);	/* Count of direct variables		*/
	DEC_VAR(IEC_UDINT,	ulIECOffset);	/* Offset of IEC vars in GDM		*/
	DEC_VAR(IEC_UDINT,	ulIECSize); 	/* Size of IEC vars in GDM			*/
	DEC_VAR(IEC_UINT,	uDebugCode);	/* Project is prepared for debugging*/
	DEC_VAR(IEC_UINT,	uCpyRegions);	/* Count of all CopyRegions 		*/
	DEC_VAR(IEC_UINT,	uCpyRegConst);	/* Count of all CopyRegions for RConst*/
	DEC_VAR(IEC_UINT,	uCpyRegCOff);	/* Offset of CopyRegions for RConst */

} XProject_205; 						/* use only for V2.05 or higher 	*/
										/* -------------------------------- */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT,	ulBinDLVersion);/* the version of the binary		*/
										/* download format					*/
	DEC_VAR(IEC_UDINT,	ulStateVarOffs);/* byte offset for __state variable */
	DEC_VAR(IEC_UINT,	uTasks);		/* Count of tasks					*/
	DEC_VAR(IEC_UINT,	uCodeObjects);	/* Count of Code blocks (POU's) 	*/
	DEC_VAR(IEC_UINT,	uInstObjects);	/* Count of objects (instances) 	*/
	DEC_VAR(IEC_UINT,	uDirectVars);	/* Count of direct variables		*/
	DEC_VAR(IEC_UDINT,	ulIECOffset);	/* Offset of IEC vars in GDM		*/
	DEC_VAR(IEC_UDINT,	ulIECSize); 	/* Size of IEC vars in GDM			*/
	DEC_VAR(IEC_UINT,	uDebugCode);	/* Project is prepared for debugging*/
	DEC_VAR(IEC_UINT,	uCpyRegions);	/* Count of all CopyRegions 		*/
	DEC_VAR(IEC_UINT,	uCpyRegConst);	/* Count of all CopyRegions for RConst*/
	DEC_VAR(IEC_UINT,	uCpyRegCOff);	/* Offset of CopyRegions for RConst */	
	DEC_VAR(IEC_UINT,	uProjInfo); 	/* No. of project info, 0 or 1 only */

} XProject_207; 						/* use only for V2.07 or higher 	*/
										/* -------------------------------- */
#endif /* ! INC_RTS */

typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT,	ulBinDLVersion);/* the version of the binary		*/
										/* download format					*/
	DEC_VAR(IEC_UDINT,	ulStateVarOffs);/* byte offset for __state variable */
	DEC_VAR(IEC_UINT,	uTasks);		/* Count of tasks					*/
	DEC_VAR(IEC_UINT,	uCodeObjects);	/* Count of Code blocks (POU's) 	*/
	DEC_VAR(IEC_UINT,	uInstObjects);	/* Count of objects (instances) 	*/
	DEC_VAR(IEC_UINT,	uDirectVars);	/* Count of direct variables		*/
	DEC_VAR(IEC_UDINT,	ulIECOffset);	/* Offset of IEC vars in GDM		*/
	DEC_VAR(IEC_UDINT,	ulIECSize); 	/* Size of IEC vars in GDM			*/
	DEC_VAR(IEC_UINT,	uDebugCode);	/* Project is prepared for debugging*/
	DEC_VAR(IEC_UINT,	uCpyRegions);	/* Count of all CopyRegions 		*/
	DEC_VAR(IEC_UINT,	uCpyRegConst);	/* Count of all CopyRegions for RConst*/
	DEC_VAR(IEC_UINT,	uCpyRegCOff);	/* Offset of CopyRegions for RConst */	
	DEC_VAR(IEC_UINT,	uProjInfo); 	/* No. of project info, 0 or 1 only */
	DEC_VAR(IEC_UINT,	uIOLayer);		/* No. of IO (field bus) layer		*/

} XProject_213; 						/* use only for V2.1.3 or higher	*/
										/* -------------------------------- */

										/* Extended project informations
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_CHAR,	szProjVers[MAX_INFO_SHORT]);/* Project version		*/
	DEC_VAR(IEC_CHAR,	szProjName[MAX_INFO_SHORT]);/* Project name 		*/
	DEC_VAR(IEC_CHAR,	szModified[MAX_INFO_LONG]); /* Modification date	*/
	DEC_VAR(IEC_CHAR,	szCreated[MAX_INFO_LONG]);	/* Creation date		*/
	DEC_VAR(IEC_CHAR,	szOwner[MAX_INFO_SHORT]);	/* Owner (download PC)	*/
	DEC_VAR(IEC_CHAR,	szComment1[MAX_INFO_LONG]); /* Comment line one 	*/
	DEC_VAR(IEC_CHAR,	szComment2[MAX_INFO_LONG]); /* Comment line two 	*/
	DEC_VAR(IEC_CHAR,	szComment3[MAX_INFO_LONG]); /* Comment line three	*/

} XProjInfo;							/* --------------------------------- */



/* State Information Polling
 * ----------------------------------------------------------------------------
 */
typedef struct		/* --> aligned (2)	*/
{
	DEC_VAR(IEC_UDINT,	ulResState);	/* Resource state information		*/

	DEC_VAR(IEC_UINT,	uBPCount);		/* Count of reached breakpoints 	*/
	DEC_VAR(IEC_UINT,	uStrCount); 	/* Count of information strings 	*/

} XInfo;								/* -------------------------------- */


/* Download code block or initial value block
 * ----------------------------------------------------------------------------
 */

/* Automatic variables and instance objects
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UINT,	uIndex);		/* Code block index 				*/
	DEC_VAR(IEC_UINT,	uSegment);		/* Segment number					*/
	DEC_VAR(IEC_UDINT,	ulSize);		/* Size of object (in byte) 		*/
	DEC_VAR(IEC_UDINT,	ulOffset);		/* See byFixed						*/
	DEC_VAR(IEC_BYTE,	byFixed);		/* true: ulOffset = memory address	*/
										/* false: ulOffset = channel offset */
	DEC_VAR(IEC_BYTE,	byType);		/* Object Type						*/

} XDownHeader;							/* -------------------------------- */

/* Direct variables
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_BYTE,	bySegment); 	/* Segment number					*/
	DEC_VAR(IEC_BYTE,	byBit); 		/* Bit offset in byte				*/
	DEC_VAR(IEC_UINT,	uSize); 		/* Size of object (in bit)			*/
	DEC_VAR(IEC_UDINT,	ulOffset);		/* Offset to segment				*/

} XDownDirect;							/* -------------------------------- */



/* Copy region definition
 * ----------------------------------------------------------------------------
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UINT,	uiInst);		/* Index: Object list (instance)	*/
	DEC_VAR(IEC_UINT,	uiOffSrc);		/* offset relative to the instance	*/
	DEC_VAR(IEC_UINT,	uiSize);		/* Size of copy region				*/

} XCopyRegion;							/* -------------------------------- */



/* Debug information download
 * ----------------------------------------------------------------------------
 */
#if ! defined(INC_RTS)					/* Obsolet, use XFile_210 instead	*/

typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT,	ulLen); 		/* Data length of DBI file			*/
	DEC_VAR(IEC_UDINT,	ulOffset);		/* Offset of file in container		*/
	DEC_VAR(IEC_CHAR,	szName[50]);	/* DBI file name					*/

} XDBIFile_208; 						/* -------------------------------- */

#endif



/* Online Change
 * ----------------------------------------------------------------------------
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UINT,	uCode); 		/* No. of changed code objects		*/
	DEC_VAR(IEC_UINT,	uData); 		/* No. of changed data objects		*/
	DEC_VAR(IEC_UINT,	uToCopy);		/* No. of instance regions			*/
	DEC_VAR(IEC_UINT,	uDirVars);		/* No. of direct (%) variables		*/
	DEC_VAR(IEC_INT,	iCodeDiff); 	/* Change in code object list		*/
	DEC_VAR(IEC_INT,	iDataDiff); 	/* Change in data object list		*/
	DEC_VAR(IEC_UINT,	uTaskInfo); 	/* True, if task info downloaded	*/

} XOnlineChange;						/* -------------------------------- */

typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UINT,	uOld);			/* Old index of changed data object */
	DEC_VAR(IEC_UINT,	uNew);			/* New index of changed data object */
	DEC_VAR(IEC_UINT,	uOldOff);		/* Byte offset of old data			*/
	DEC_VAR(IEC_UINT,	uNewOff);		/* Byte offset of new data			*/
	DEC_VAR(IEC_UDINT,	ulSize);		/* Data size to be copied in bit	*/
	DEC_VAR(IEC_USINT,	usOldBit);		/* Bit	offset of old data			*/
	DEC_VAR(IEC_USINT,	usNewBit);		/* Bit	offset of new data			*/

} XOCInstCopy;							/* -------------------------------- */



/* Debug information download
 * ----------------------------------------------------------------------------
 */
#if ! defined(INC_RTS)					/* Obsolete, use XFile_210 instead	*/

typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT,	ulLen); 		/* Data length of DBI file			*/
	DEC_VAR(IEC_CHAR,	szName[100]);	/* DBI file name					*/

} XFile_208;							/* -------------------------------- */

#endif



/* File download
 * ----------------------------------------------------------------------------
 */

typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT,	ulDataLen); 	/* Length of file data				*/
	DEC_VAR(IEC_UINT,	uNameLen);		/* File name length - without ZB!	*/
	DEC_VAR(IEC_USINT,	usCRC); 		/* Checksum 						*/
	DEC_VAR(IEC_USINT,	usHash);		/* Hash 							*/

} XFileDef; 							/* -------------------------------- */



/* Alignment test structure
 * ----------------------------------------------------------------------------
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_CHAR,	cDummy);
	DEC_VAR(IEC_LREAL,	lDummy);

} XAlignTest;							/* -------------------------------- */



/* Configuration upload
 * ----------------------------------------------------------------------------
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_USINT,	usBigEndian);	/* TRUE, if target is Big Endian	*/
	DEC_VAR(IEC_USINT,	usDummy);		/* Alignment dummy					*/
	DEC_VAR(IEC_UINT,	uFirmware); 	/* Firmware version 				*/
	DEC_VAR(IEC_UINT,	uMaxData);		/* Comm. Buffer size (MAX_DATA) 	*/
	DEC_VAR(IEC_UINT,	uAddConfig);	/* Size of Additional Config. Data	*/

	DEC_VAR(IEC_DATA,	Reserved[100]); /* Reserverd for future used.		*/

} XConfig;								/* -------------------------------- */



/* Debug information download
 * ----------------------------------------------------------------------------
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_USINT, usType); 		/* Task index						*/
	DEC_VAR(IEC_USINT, usNameSize); 	/* Size of Instance name			*/
	DEC_VAR(IEC_UINT,  uInst);			/* Instance index					*/
	DEC_VAR(IEC_UINT,  uChildren);		/* Number of child objects			*/
	DEC_VAR(IEC_UINT,  uIndex); 		/* Index to type object 			*/
	
	DEC_VAR(IEC_UDINT, ulFOBrother);	/* File offset to next brother		*/
	DEC_VAR(IEC_UDINT, ulFOAbsolut);	/* Absolute offset in file			*/

} XDBIInstance; 						/* -------------------------------- */


typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT, ulOffset);		/* File Offset to member array		*/
	DEC_VAR(IEC_UINT,  uMember);		/* Number of member variables		*/
	DEC_VAR(IEC_UINT,  uType);			/* Number of member variables		*/

} XDBIType; 							/* -------------------------------- */


typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_USINT,	usType);		/* Type of variable 				*/
	DEC_VAR(IEC_USINT,	usNameSize);	/* Size of name (without ZB!)		*/
	DEC_VAR(IEC_UINT,	uInst); 		/* Segment / instance number		*/
	DEC_VAR(IEC_UDINT,	ulOffset);		/* Variable offset					*/
	DEC_VAR(IEC_UINT,	uLen);			/* Size / number elem / string size */
	DEC_VAR(IEC_USINT,	usBit); 		/* Bit offset						*/
	DEC_VAR(IEC_USINT,	usDummy);		/* Alignment dummy					*/
	
} XDBIVar;								/* -------------------------------- */


typedef struct		/* --> aligned (2) */
{
	DEC_VAR(XDBIVar,	xVar);			/* Variable 						*/

	DEC_VAR(IEC_CHAR,	szName[0]); 	/* Name (variable length, with ZB!) */

} XVisuVar; 							/* -------------------------------- */



/* IO Layer / Field Bus Channel Information
 * ----------------------------------------------------------------------------
 */
typedef struct		/* --> aligned (2) */
{
	DEC_VAR(IEC_UDINT,	ulIOffs);		/* Offset of input (%I) channel 	*/
	DEC_VAR(IEC_UDINT,	ulISize);		/* Size of input (%I) channel		*/
	DEC_VAR(IEC_UDINT,	ulQOffs);		/* Offset of output (%Q) channel	*/
	DEC_VAR(IEC_UDINT,	ulQSize);		/* Size of output (%Q) channel		*/
	DEC_VAR(IEC_UDINT,	ulMOffs);		/* Offset of memory (%M) channel	*/
	DEC_VAR(IEC_UDINT,	ulMSize);		/* Size of memory (%M) channel		*/

	DEC_VAR(IEC_USINT,	usChannel); 	/* Corresponding Channel number 	*/
	DEC_VAR(IEC_CHAR,	szName[33]);	/* IO layer type (fcdp, bcbac, ...) */

} XIOLayer; 							/* -------------------------------- */



/* Firmware Download
 * ----------------------------------------------------------------------------
 */
typedef struct		/* --> aligned (2)	*/ 
{
	DEC_VAR(IEC_UDINT,	uForwardID);	/* ID of firmware target control	*/
										/* (-1 if target is local control)	*/
										
	DEC_VAR(IEC_UINT,	uNameLen);		/* Length of configuration file 	*/
										/* (ZB not included!)				*/

	DEC_VAR(IEC_CHAR,	szCfgFile[0]);	/* File name of configuration file	*/
										/* (Variable sized, with ZB!)		*/

} XFWDownload;							/* -------------------------------- */



/* License Information
 * ----------------------------------------------------------------------------
 */

/* Version information upload
 */
typedef struct		/* --> aligned (2)	*/ 
{
	DEC_VAR(IEC_UDINT,	ulFirmware);	/* Firmware version of RTS. 		*/
	
	DEC_VAR(IEC_CHAR,	szMain[20]);	/* Main version number, with ZB!	*/
	DEC_VAR(IEC_CHAR,	szFull[80]);	/* Full version number, with ZB!	*/

} XVersionInfo; 						/* -------------------------------- */


/* Type information upload
 */
typedef struct		/* --> aligned (2)	*/
{
	DEC_VAR(IEC_CHAR,	szType[100]);	/* Type name of RTS.				*/

} XTypeInfo;							/* -------------------------------- */


/* Serial number upload
 */
typedef struct		/* --> aligned (2)	*/
{
	DEC_VAR(IEC_UDINT,	ulSerialNo);	/* Serial number of target system	*/

} XSerialNo;							/* -------------------------------- */


/* MAC address upload
 */
typedef struct		/* --> aligned (2)	*/
{
	DEC_VAR(IEC_DATA,	MacAddr[6]);	/* MAC address of target system 	*/

} XMacAddr; 							/* -------------------------------- */


/* Installation key upload
 */
typedef struct		/* --> aligned (2)	*/
{
	DEC_VAR(IEC_CHAR, szInstKey[35]);	/* HW dependent inst. key, with ZB! */

	DEC_VAR(IEC_USINT, usReserved); 	/* Reserved for future use			*/

} XInstKey; 							/* -------------------------------- */


/* License key download
 */
typedef struct		/* --> aligned (2)	*/
{
	DEC_VAR(IEC_CHAR, szLicKey[25]);	/* License key for feature, with ZB!*/

	DEC_VAR(IEC_USINT, usReserved); 	/* Reserved for future use			*/

} XLicKey;								/* -------------------------------- */


/* Feature upload
 */
typedef struct		/* --> aligned (2)	*/
{
	DEC_VAR(IEC_UINT, uLicensed);		/* Licensed features				*/

	DEC_VAR(IEC_UINT, uAvailable);		/* Available features				*/

} XFeatDef;


/* LicenseEx
 */
typedef struct		/* ---> aligned (2) */
{
	DEC_VAR(IEC_UINT, uLicense);		/* License type 					*/
	DEC_VAR(IEC_UINT, uLength); 		/* Length of license				*/

	DEC_VAR(IEC_DATA, pLicEx[0]);		/* License (placeholder, variable)	*/

} XLicEx;								/* -------------------------------- */



/* Block types
 * ----------------------------------------------------------------------------
 */
#define BT_ACK				0x33u			/* Acknowledge					*/
#define BT_NACK 			0xAAu			/* Not acknowledged (error) 	*/
#define BT_REQ				0x3Au			/* Request for response 		*/
#define BT_DATA 			0xA3u			/* Data block					*/


/* Memory segments
 * ----------------------------------------------------------------------------
 */
#define MAX_SEGMENTS		0x0006u 		/* Count of memory segments 	*/

#define SEG_INPUT			0x0000u 		/* Input variables				*/
#define SEG_OUTPUT			0x0001u 		/* Output variables 			*/
#define SEG_GLOBAL			0x0002u 		/* Global variables 			*/
#define SEG_RETAIN			0x0003u 		/* Global retain variables		*/
#define SEG_LOCAL			0x0004u 		/* Local instance data			*/
#define SEG_CODE			0x0005u 		/* Code segment 				*/

#define SEG_WRITEF			0x1111u 		/* WF's - not a real segment!	*/


/* Breakpoint states
 * ----------------------------------------------------------------------------
 */
#define BP_STATE_ERROR				0xffffu
#define BP_STATE_REACHED			0x0000u
#define BP_STATE_LEAVED 			0x0001u


/* Download mode
 * ----------------------------------------------------------------------------
 */
#define DOWN_MODE_COLD				0x0001u
#define DOWN_MODE_WARM				0x0002u
#define DOWN_MODE_NORMAL			0x0010u
#define DOWN_MODE_FLASH 			0x0020u


/* Object Types
 * ----------------------------------------------------------------------------
 */
#define VMM_OT_UNKNOWN				0xffu
#define VMM_OT_PROG 				0x00u
#define VMM_OT_FUN					0x01u
#define VMM_OT_FB					0x02u
#define VMM_OT_ARRAY				0x10u
#define VMM_OT_STRUCT				0x20u
#define VMM_OT_SIMPLE				0x30u


/* Task attributes
 * ----------------------------------------------------------------------------
 */
#define VMM_TASK_ATTR_AUTOSTART 	0x01u		/* Autostart task			*/
#define VMM_TASK_ATTR_UNLOADED		0x02u		/* Task initially unloaded */
#define VMM_TASK_ATTR_EVENT_DRIVEN	0x04u		/* Task is event driven 	*/
#define VMM_TASK_ATTR_CYCLIC		0x08u		/* Cyclic task				*/


/* XVariable types
 * ----------------------------------------------------------------------------
 */
#define VMM_XV_BITMASK				0x0fu		/* To extract bit offset	*/
#define VMM_XV_TYPEMASK 			0xf0u		/* To extract var. type 	*/

#define VMM_XV_SMALL				0x20u		/* Variable is small type	*/
#define VMM_XV_MEDIUM				0x40u		/* Variable is medium type	*/
#define VMM_XV_LARGE				0x80u		/* Variable is large type	*/


/* Name of DBI files
 * ----------------------------------------------------------------------------
 */
#define DBI_FILE_INSTANCE			"_inst.f4c"
#define DBI_FILE_TYPE				"_type.f4c"
#define DBI_FILE_VAR				"_var.f4c"


/* DBI data type masks
 * ----------------------------------------------------------------------------
 */
#define DBI_DTM_ARRAY				0x80u
#define DBI_DTM_OBJECT				0x40u
#define DBI_DTM_SIMPLE				0x20u


/* DBI Data types
 * ----------------------------------------------------------------------------
 */
#define DBI_DT_UNKNOWN				0x1fu

#define DBI_DT_BOOL 				0x01u

#define DBI_DT_BYTE 				0x02u
#define DBI_DT_WORD 				0x03u
#define DBI_DT_DWORD				0x04u
#define DBI_DT_LWORD				0x05u

#define DBI_DT_USINT				0x06u
#define DBI_DT_UINT 				0x07u
#define DBI_DT_UDINT				0x08u
#define DBI_DT_ULINT				0x09u

#define DBI_DT_SINT 				0x0au
#define DBI_DT_INT					0x0bu
#define DBI_DT_DINT 				0x0cu
#define DBI_DT_LINT 				0x0du

#define DBI_DT_REAL 				0x0eu
#define DBI_DT_LREAL				0x0fu

#define DBI_DT_STRING				0x10u

#define DBI_DT_TIME 				0x11u


/* LicenseEx types
 * ----------------------------------------------------------------------------
 */
#define LIC_EX_BACNET				0x0001u


/* Commands
 * ----------------------------------------------------------------------------
 */
#define CMD_GET_STATE				0x00u		/* Remember to change the	*/
#define CMD_NOT_IMPLEMENTED 		0x01u		/* string array in vmmDef.h */
#define CMD_LOGIN					0x02u		/* also after changing a	*/
#define CMD_LOGOUT					0x03u		/* command value!			*/
#define CMD_WARM_START				0x04u
#define CMD_COLD_START				0x05u
#define CMD_START_ALL_TASKS 		0x06u
#define CMD_STOP_ALL_TASKS			0x07u
#define CMD_START_TASK				0x08u
#define CMD_STOP_TASK				0x09u
#define CMD_OPEN_DEBUG_SESSION		0x0Au
#define CMD_CLOSE_DEBUG_SESSION 	0x0Bu
#define CMD_SET_BREAKPOINT			0x0Cu
#define CMD_CLEAR_BREAKPOINT		0x0Du
#define CMD_CLEAR_ALL_BREAKPOINTS	0x0Eu
#define CMD_SINGLE_STEP 			0x0Fu
#define CMD_CONTINUE				0x10u
#define CMD_GET_VALUE				0x11u
#define CMD_SET_VALUE				0x12u
#define CMD_DOWNLOAD_CONFIG 		0x13u
#define CMD_DOWNLOAD_INITIAL		0x14u
#define CMD_DOWNLOAD_CODE			0x15u
#define CMD_DOWNLOAD_CUSTOM 		0x16u
#define CMD_DOWNLOAD_FINISH 		0x17u
#define CMD_START_RESOURCE			0x18u
#define CMD_STOP_RESOURCE			0x19u
#define CMD_DOWNLOAD_END			0x1Au
#define CMD_DOWNLOAD_BEGIN			0x1Bu
#define CMD_INITIALIZE_CLEAR		0x1Cu
#define CMD_CLEAR_FLASH 			0x1Du
#define CMD_DBI_GET_CHILDREN		0x1Eu
#define CMD_DBI_GET_ADDRESS 		0x1Fu
#define CMD_DOWNLOAD_DEBUG			0x20u
#define CMD_DBI_GET_TASKNR			0x21u
#define CMD_GET_PROJ_VERSION		0x22u
#define CMD_GET_PROJ_INFO			0x23u
#define CMD_LOAD_PROJECT			0x24u
#define CMD_SAVE_PROJECT			0x25u
#define CMD_CUSTOM					0x26u
#define CMD_LOAD_FILE				0x27u
#define CMD_SAVE_FILE				0x28u
#define CMD_DIR_CONTENT 			0x29u
#define CMD_OC_BEGIN				0x2Au
#define CMD_OC_CODE 				0x2Bu
#define CMD_OC_DEBUG				0x2Cu
#define CMD_OC_CUSTOM				0x2Du
#define CMD_OC_INITIAL				0x2Eu
#define CMD_OC_COMMIT				0x2Fu
#define CMD_OC_END					0x30u
#define CMD_OC_CONFIG				0x31u
#define CMD_OC_FINISH				0x32u
#define CMD_FLASH					0x33u
#define CMD_DEL_FILE				0x34u
#define CMD_GET_CONFIG				0x35u
#define CMD_RET_WRITE				0x36u
#define CMD_RET_CYCLE				0x37u
#define CMD_FW_DOWNLOAD 			0x38u
#define CMD_FW_EXECUTE				0x39u
#define CMD_FW_RESULT				0x3Au
#define CMD_RET_UPLOAD				0x3Bu
#define CMD_RET_DOWNLOAD			0x3Cu
#define CMD_IEC_UPLOAD				0x3Du
#define CMD_IEC_DOWNLOAD			0x3Eu
#define CMD_GET_INSTKEY 			0x3Fu
#define CMD_SET_LICKEY				0x40u
#define CMD_GET_SERIALNO			0x41u
#define CMD_GET_FEATURE 			0x42u
#define CMD_GET_TYPE				0x43u
#define CMD_GET_VERSION 			0x44u
#define CMD_SET_LICEX				0x45u
#define CMD_DOWNLOAD_IOL			0x46u
#define CMD_DOWNLOAD_CLEAR			0x47u


#define LAST_CMD_VALUE				CMD_DOWNLOAD_CLEAR



/* Interpreter Exceptions (Reported back to 4c; see vmmFormatTaskError())
 * Note: Changes here should also be done in 4C_System.4cl's 4C_System.cst's 
 * FC_ERRNO_* CONST and in the following EXCEPTION_TEXT define!
 * -----------------------------------------------------------------------------------
 */
#define EXCEPT_ILLEGAL_OPCODE		0x20u	/* Illegal opcode							*/
#define EXCEPT_STACK_OVERFLOW		0x21u	/* Stack overflow							*/
#define EXCEPT_STACK_UNDERFLOW		0x22u	/* Stack underflow							*/
#define EXCEPT_DIVISION_BY_ZERO 	0x23u	/* Division by zero 						*/
#define EXCEPT_ARRAY_RANGE			0x24u	/* Array range								*/
#define EXCEPT_STRING_LEN			0x25u	/* String length							*/
#define EXCEPT_NULL_PTR 			0x26u	/* String length							*/
#define EXCEPT_CALL_OVERFLOW		0x27u	/* Call Stack overflow						*/
#define EXCEPT_INVALID_LIB_CALL 	0x28u	/* Illegal library function/FB call 		*/ 
#define EXCEPT_INVALID_ARG			0x29u	/* Invalid argument passed to fun/FB		*/
#define EXCEPT_MATH 				0x2au	/* General mathematic exception 			*/
#define EXCEPT_WATCHDOG 			0x2bu	/* Watchdog triggered for task				*/
#define EXCEPT_EXE_TIME_OVERRUN 	0x2cu	/* Task execution time overrun				*/
#define EXCEPT_USER_EXCEPTION		0x2du	/* UserException							*/
#define EXCEPT_TASK_IMAGE			0x2eu	/* Error accessing the VM task image		*/
#define EXCEPT_TASK_TIMER			0x2fu	/* Error activating the VM task timer		*/
#define EXCEPT_TASK_INIT			0x30u	/* Error initializing the VM task			*/
#define EXCEPT_TASK_BREAK			0x31u	/* VM in breakpoint when close debug mode	*/
#define EXCEPT_UNKOWN				0x32u	/* Everything else							*/


/* Value of first and last exception. (To compute offset in EXCEPTION_TEXT array
 */
#define EXCEPT_FIRST				EXCEPT_ILLEGAL_OPCODE
#define EXCEPT_LAST 				EXCEPT_UNKOWN						


/* Error numbers
 * ----------------------------------------------------------------------------
 * Range 0x0020u - 0x00ffu is reserved for interpreter exceptions!
 */

#define ERR_OK					0x0000u /* Not to be used, use OK					*/
#define ERR_ERROR				0x0001u /* Generalerror							*/
#define ERR_TIMEOUT				0x0010u	/* timeout error*/

#undef	OK
#define OK						ERR_OK

/* Warnings
 */
#define WRN_TIME_OUT			0x0100u /* Time out occurred (communication)		*/
#define WRN_NO_CONNECTION		0x0101u /* No connection to OPC++ server			*/
#define WRN_HANDLED 			0x0102u /* Command/response already handled 		*/
#define WRN_BREAK			0x0103u /* Breakpoint reached.						*/
#define ERR_CRC 			0x0104u /* Checksum calculation failed				*/
#define WRN_DIVIDED 			0x0105u /* Data block is divided					*/
#define WRN_TRUNCATED			0x0106u /* String has been truncated.				*/
#define WRN_NO_MESSAGE			0x0107u /* No message in queue. 					*/
#define WRN_RETAIN_INVALID		0x0108u /* Retain memory invalid -> Cold Start! 	*/
#define WRN_TASK_HALTED 		0x0109u /* Task halted immediately. 				*/
#define WRN_IN_PROGRESS 		0x010au /* Process still in progress.				*/
#define WRN_NOT_HANDLED 		0x010bu /* Object not yet handled.					*/
#define WRN_FILE_NOT_EXIST		0x010cu /* File does not exist. 					*/

/* Common error messages
 */
#define ERR_INIT			0x0200u /* Initialization failed.					*/
#define ERR_INVALID_TASK		0x0201u /* Invalid task number. 					*/
#define ERR_INVALID_COMMAND 		0x0202u /* Invalid command. 						*/
#define ERR_INVALID_DATA		0x0203u /* Invalid data.							*/
#define ERR_INVALID_OFFSET		0x0204u /* Invalid object offset received.			*/
#define ERR_INVALID_INSTANCE	0x0205u /* Invalid instance.						*/
#define ERR_INVALID_CLASS		0x0206u /* Invalid class number.					*/
#define ERR_INVALID_DATA_SIZE	0x0207u /* Invalid data size.						*/
#define ERR_QUEUE_FULL			0x0208u /* Queue full.								*/
#define ERR_OUT_OF_MEMORY		0x0209u /* Out of memory.							*/
#define ERR_DEBUG_MODE			0x020au /* Can't execute, not in debug mode.		*/
#define ERR_LOGIN				0x020bu /* Can't execute, not logged in.			*/
#define ERR_NO_PROJECT			0x020cu /* No project loaded.						*/
#define ERR_CRITICAL_SECTION	0x020du /* Failed to enter a critical section.		*/
#define ERR_ACTIVATE_TIMER		0x020eu /* Failed to activate VM timer. 			*/
#define ERR_WRONG_PROJECT		0x020fu /* Wrong project or project version.		*/
#define ERR_TO_MANY_VARS		0x0210u /* To many variables for Get/SetValue		*/
#define ERR_INVALID_PARAM		0x0211u /* Invalid parameter.						*/
#define ERR_CREATE_QUEUE		0x0212u /* Failed to create a IPC message queue.	*/
#define ERR_CREATE_TIMER		0x0213u /* Failed to create a VM timer object.		*/
#define ERR_IPC_RECV_FAILED 	0x0214u /* Failed to receive a IPC message. 		*/
#define ERR_IPC_SEND_FAILED 	0x0215u /* Failed to send a IPC message.			*/
#define ERR_IPC_VMM_TIMEOUT 	0x0216u /* Timeout waiting for answer from VMM. 	*/
#define ERR_ALREADY_IN_DEBUG	0x0217u /* Only one debug mode possible.			*/
#define ERR_INVALID_SEGMENT 	0x0218u /* Invalid object segment.					*/
#define ERR_IO_READ 			0x0219u /* Error reading from comm. device. 		*/
#define ERR_IO_WRITE			0x021au /* Error writing to comm. device.			*/
#define ERR_NOT_CONNECTED		0x021bu /* Not connected.							*/
#define ERR_INVALID_MSG 		0x021cu /* Invalid message received.				*/
#define ERR_WRITE_TO_INPUT		0x021du /* Write access to input segment.			*/
#define ERR_TO_MUCH_DATA		0x021eu /* To much data specified.					*/
#define ERR_NOT_SUPPORTED		0x021fu /* Function/Command not supported.			*/
#define ERR_UNEXPECTED_MSG		0x0220u /* Wrong - not fitting - response received. */
#define ERR_ERRMSG_RECEIVED 	0x0221u /* Common error message received.			*/
#define ERR_INVALID_QUEUE		0x0222u /* An invalid message queue was given.		*/
#define ERR_UNKNOWN_PRODUCT 	0x0223u /* Unknown FarosPLC product ID given.		*/
#define ERR_NOT_READY			0x0224u /* Unable to execute command now.			*/
#define ERR_INVALID_IOLAYER 	0x0225u /* An invalid IO layer was given.			*/
#define ERR_NOT_LICENSED		0x0226u /* Feature not licensed.					*/
#define ERR_INVALID_LICENSE 	0x0227u /* Invalid license key received.			*/
#define ERR_INVALID_PRODUCT 	0x0228u /* Invalid product number received. 		*/
#define ERR_NULL_POINTER		0x0229u /* Null pointer as parameter.				*/
#define ERR_NOT_CONFIGURED		0x022au /* Not configured.							*/

/* Download
 */
#define ERR_INVALID_DOMAIN		0x0300u /* Invalid download domain. 				*/
#define ERR_INVALID_VERSION 	0x0301u /* Control newer than Engineering.			*/
#define ERR_OVERRUN_TASK		0x0302u /* Number of tasks to large 				*/
#define ERR_OVERRUN_INSTANCE	0x0303u /* Number of objects to large				*/
#define ERR_OVERRUN_CLASS		0x0304u /* Number of code blocks to large			*/
#define ERR_OVERRUN_PROGRAM 	0x0305u /* Number of programs per task to large 	*/
#define ERR_OVERRUN_CODESEG 	0x0306u /* Code segment overrun 					*/
#define ERR_OVERRUN_OBJSEG		0x0307u /* Object segment overrun					*/
#define ERR_OVERRUN_REGION		0x0308u /* Number of memory regions to large		*/
#define ERR_ENTRIES_MISSING 	0x0309u /* Not all list entries correctly received	*/
#define ERR_INVALID_SEG_INIT	0x030au /* Common segment initialization not first	*/
#define ERR_CREATE_TASK 		0x0310u /* Error creating a task or thread. 		*/
#define ERR_OVERRUN_COPYSEG 	0x0311u /* Number of copy regions to large			*/
#define ERR_BUFFER_TOO_SMALL	0x0312u /* Buffer for diveded data too small		*/

/* Breakpoints
 */
#define ERR_INVALID_CODE_POS	0x0400u /* Invalid code offset in BP specification	*/
#define ERR_BP_NOT_FOUND		0x0401u /* Given breakpoint not found in BP list	*/
#define ERR_BP_LIST_FULL		0x0402u /* Breakpoint list is full					*/
#define ERR_TASK_NOT_IN_BREAK	0x0403u /* Task has no reached breakpoint			*/

/* File I/O
 */
#define ERR_FILE_INIT			0x0500u /* file functionality not initialized		*/
#define ERR_FILE_MAX			0x0501u /* maximum number of open files reached 	*/
#define ERR_FILE_WRONG_ID		0x0502u /* File id is not valid.					*/
#define ERR_FILE_EOF			0x0503u /* the end of the file has been reached 	*/
#define ERR_FILE_OPEN			0x0504u /* Error opening a file.					*/
#define ERR_FILE_CLOSE			0x0505u /* Error closing a file.					*/
#define ERR_FILE_READ			0x0506u /* Error reading from file. 				*/
#define ERR_FILE_WRITE			0x0507u /* Error writing to file.					*/
#define ERR_FILE_SYNC			0x0508u /* Temporary file closed by another process */
#define ERR_FILE_NOT_EXIST		0x0509u /* File doesn't exist.						*/
#define ERR_FILE_SEEK			0x050au /* Error seeking a file.					*/
#define ERR_FILE_RENAME 		0x050bu /* Error renaming a file.					*/
#define ERR_FILE_REMOVE 		0x050cu /* Error deleting a file.					*/
#define ERR_INVALID_DRIVE		0x050du /* Invalid drive specification. 			*/
#define ERR_CHANGE_DRIVE		0x050eu /* Error changing a drive.					*/
#define ERR_CHANGE_DIR			0x050fu /* Error changing a directory				*/
#define ERR_INVALID_PATH		0x0510u /* Invalid path specified.					*/
#define ERR_INVALID_FILE_NAME	0x0511u /* Invalid file name specified. 			*/
#define ERR_PATH_IS_NOT_8_3 	0x0512u /* Path is not in 8.3 format.				*/
#define ERR_PATH_TO_LONG		0x0513u /* Path is to long. 						*/
#define ERR_FILE_INVALID		0x0514u /* Invalid file specified.					*/
#define ERR_FILE_NOT_OPEN		0x0515u /* File is not open.						*/
										   
/* Debug Interface
 */
#define ERR_DBI_INIT			0x0600u /* DBI functionality not initialized		*/
#define ERR_DBI_PARAM			0x0601u /* One parameter was not valid				*/
#define ERR_DBI_OBJ_NOT_FOUND	0x0602u /* Object not found 						*/
#define ERR_DBI_FILE_FORMAT 	0x0603u /* A file has been wrong formatted			*/
#define ERR_DBI_RETAIN_MIXED	0x0604u /* Complex variable is located in retain 
										   and non retain segment.					*/
#define ERR_DBI_BUF_TOO_SMALL	0x0605u /* Transmission buffer has been to small	*/

/* Flash interface
 */
#define ERR_FLASH				0x0700u /* Common flash error.						*/
#define ERR_FLASH_INIT			0x0701u /* Error initializating flash memory.		*/
#define ERR_FLASH_WRITE 		0x0702u /* Error writing into flash memory. 		*/
#define ERR_FLASH_READ			0x0703u /* Error reading from flash memory. 		*/
#define ERR_FLASH_CLEAR 		0x0704u /* Error clearing th flash memory.			*/
#define ERR_FLASH_WRONG_BLOCK	0x0705u /* Flash block does not match.				*/

/* Online Change
 */
#define ERR_OC_TASK_CHANGED 	0x0800u /* Count of tasks or attributes changed.	*/
#define ERR_OC_PROJ_CHANGED 	0x0801u /* Project changed, OC not possible.		*/
#define ERR_OC_INVALID_CODE 	0x0802u /* Invalid code object index received.		*/
#define ERR_OC_INVALID_INST 	0x0803u /* Invalid instance object index received.	*/
#define ERR_OC_COPY_LIST		0x0804u /* Invalid instance obj. copy list received.*/
#define ERR_OC_COPY_NEW 		0x0805u /* Invalid new instance object specified.	*/
#define ERR_OC_COPY_OLD 		0x0806u /* Invalid old instance object specified.	*/
#define ERR_OC_TO_MANY_CODE 	0x0807u /* To many changed code objects received.	*/
#define ERR_OC_TO_MANY_INST 	0x0808u /* To many changed instance objects recvd.	*/
#define ERR_OC_TO_MANY_CL		0x0809u /* To many entries in copy list.			*/
#define ERR_OC_TEMP_CODE		0x080au /* To many temporary code objects.			*/
#define ERR_OC_TEMP_INST		0x080bu /* To many temporary instance objects.		*/
#define ERR_OC_RETAIN_CHANGED	0x080cu	/* OC not supported for retain variables.	*/

/* Field Bus
 */
#define ERR_FB_INIT 			0x0a00u /* Failed to initialize field bus			*/
#define ERR_FB_INIT_DATA		0x0a01u /* FB init. failed - inval. config data 	*/
#define WRN_FB_NO_INIT_DATA 	0x0a02u /* FB not initialized, no config data		*/
#define ERR_FB_TERM 			0x0a03u /* Error terminating FB 					*/
#define ERR_FB_NOT_INITIALIZED	0x0a04u /* FB is not initialized					*/
#define ERR_FB_NOT_OPERATING	0x0a05u /* FB is not operating						*/
#define ERR_FB_UNKNOWN_FBUS 	0x0a06u /* Field bus type unknown					*/
#define ERR_FB_NOT_LICENSED 	0x0a07u /* Given field bus type not licensed.		*/
#define ERR_FB_NOT_ENABLED		0x0a08u /* Given field bus is not activated.		*/

/* BACnet
 */
#define ERR_BAC_OBJ_NOT_INIT	0x0b00u /* Object is not initialized.				*/
#define ERR_BAC_OBJ_CREATE		0x0b01u /* Failed to create object in BACnet stack. */
#define ERR_INVALID_PRIORITY	0x0b02u /* Invalid priority value for object.		*/
#define ERR_PROP_NOT_SUPPORTED	0x0b03u /* Property is not supported.				*/
#define ERR_SET_PROP_LOCAL		0x0b04u /* Error setting local property.			*/
#define ERR_SET_PROP_REMOTE 	0x0b05u /* Error setting remote property.			*/
#define ERR_COV_SUBSCRIBE		0x0b06u /* COV subscription failed. 				*/
#define ERR_CFG_FILE_NOT_FOUND	0x0b07u /* Configuration file not found.			*/
#define ERR_LIST_FULL			0x0b08u /* Object list full.						*/
#define ERR_INVALID_CONFIG		0x0b09u /* Invalid configuration file.				*/
#define ERR_INVALID_OBJECT_TYPE 0x0b0au /* Invalid object type. 					*/
#define ERR_INVALID_PROP_TYPE	0x0b0cu /* Invalid property type.					*/
#define ERR_INVALID_OBJECT_ID	0x0b0du /* Invalid object ID.						*/
#define ERR_API_CALL_FAILED 	0x0b0eu /* BACnet API call failed.					*/
#define ERR_OBJ_NOT_FOUND		0x0b0fu /* Object not found.						*/
#define ERR_BACNET				0x0b10u /* Common BACnet error. 					*/
#define ERR_DEVICE_NOT_FOUND	0x0b11u /* BACnet Device not found. 				*/
#define ERR_GET_PROP_LOCAL		0x0b12u /* Error getting local property.			*/
#define ERR_GET_PROP_REMOTE 	0x0b13u /* Error getting remote property.			*/
#define ERR_INVALID_DATA_TYPE	0x0b14u /* Invalid data type.						*/
#define ERR_READ_ONLY			0x0b15u /* Error Read Only. 						*/
#define ERR_COV_NOT_SUPPORTED	0x0b16u /* COV notifications not supported. 		*/
#define ERR_NO_EVENTS			0x0b17u /* No events queued.						*/
#define ERR_TASK_NOT_ACTIVE 	0x0b18u /* Task is not active.						*/
#define ERR_DUPLICATE_OBJECT	0x0b19u /* Duplicate BACnet object specified.		*/
#define ERR_TOO_MANY_OBJECTS	0x0b1au /* Too many BACnet objects downloaded.		*/
#define ERR_NO_LOCAL_DEVICE 	0x0b1bu /* No local BACnet device defined.			*/
#define ERR_STACK_NOT_INIT		0x0b1cu /* BACnet stack not yet initialized.		*/
#define ERR_SHUTDOWN_PENDING	0x0b1du /* BACnet stack shutdown is in progress.	*/

#define WRN_BAC_OBJ_NOT_EXIST	0x8000u /* BACnet object does not exist.			*/
#define WRN_PROPVAL_BAD 		0x8001u /* Property value is BAD.					*/
#define WRN_OBJECT_BAD			0x8002u /* Object is BAD.							*/
#define WRN_NO_PROP_VALUE		0x8003u /* No property value available (yet).		*/
#define WRN_OBJ_NOT_EXIST		0x8004u /* Object does not exist.					*/

/* Profibus DP
 */
#define ERR_MASTER_ERROR		0x0c00u /* Profibus Master switch to error state.	*/
#define ERR_INIT_TIMEOUT		0x0c01u /* Profibus Master failed to init in time.	*/


/* Structures in this file MUST be compiled with one byte alignment!
 */
#if ! defined(INC_RTS)

  #pragma pack()

#else

  #define RTS_PRAGMA_PACK_DEF
  #include "osAlign.h"
  #undef  RTS_PRAGMA_PACK_DEF

#endif

#endif	/* _VMSHARED_H_ */

/* ---------------------------------------------------------------------------- */
