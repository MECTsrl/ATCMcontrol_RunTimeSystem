
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
 * Filename: libFile.h
 */


#ifndef _LIBFILE_H_
#define _LIBFILE_H_

#if defined(INC_4C)
#error "Must not be included in 4C!"
#endif

#if defined(RTS_CFG_FILE_LIB)

/* --- 160 -------------------------------------------------------------------- */
void fa_sync_openFile(STDLIBFUNCALL);
/* --- 161 -------------------------------------------------------------------- */
void fa_sync_readFile(STDLIBFUNCALL);
/* --- 162 -------------------------------------------------------------------- */
void fa_sync_renameFile(STDLIBFUNCALL);
/* --- 163 -------------------------------------------------------------------- */
void fa_sync_writeFile(STDLIBFUNCALL);
/* --- 164 -------------------------------------------------------------------- */
void fa_sync_closeFile(STDLIBFUNCALL);
/* --- 165 -------------------------------------------------------------------- */
void fa_sync_createDirectory(STDLIBFUNCALL);
/* --- 166 -------------------------------------------------------------------- */
void fa_sync_deleteFile(STDLIBFUNCALL);
/* --- 167 -------------------------------------------------------------------- */
void fa_sync_existsFile(STDLIBFUNCALL);
/* --- 168 -------------------------------------------------------------------- */
void fa_sync_getSize(STDLIBFUNCALL);
/* --- 169 -------------------------------------------------------------------- */
void fa_get_diskFreeSpace(STDLIBFUNCALL);
/* --- 170 -------------------------------------------------------------------- */
void fa_error_string(STDLIBFUNCALL); 
/* --- 171 -------------------------------------------------------------------- */
void fa_sync_arrayReadFile(STDLIBFUNCALL);
/* --- 172 -------------------------------------------------------------------- */
void fa_sync_arrayWriteFile(STDLIBFUNCALL);
/* --- 173 -------------------------------------------------------------------- */
void fa_flush(STDLIBFUNCALL);



#define CFA_BOF 					0		/* Begin Of File					*/
#define CFA_EOF 					-1		/* End Of File						*/

#define CFA_READ					2		/* read file						*/
#define CFA_READ_WRITE				6		/* read and write					*/
#define CFA_APPEND					7		/* append to file					*/

#define CFA_NoError 				0
#define CFA_NoErrorStr				"Success."

#define CFA_CommonError 			10

#define CFA_InternalError			11
#define CFA_InternalErrorStr		"Internal Error."

#define CFA_InvalidArgument 		20
#define CFA_InvalidArgumentStr		"Invalid Argument."

#define CFA_FileNotOpenError		30
#define CFA_FileNotOpenErrorStr 	"The specified file is not open."
	
#define CFA_FileAlreadyOpen 		50
#define CFA_FileAlreadyOpenStr		"The specified file is already open."


#if defined(IP_CFG_AIS8)

#define GET_AR_LEN(pArray)			(*(IEC_USINT OS_DPTR *)pArray)
#define GET_AR_DAT(pArray)			((IEC_BYTE OS_DPTR *)(pArray) + sizeof(IEC_USINT))

#endif

#if defined(IP_CFG_AIS16)

#define GET_AR_LEN(pArray)			(*(IEC_UINT OS_DPTR *)pArray)
#define GET_AR_DAT(pArray)			((IEC_BYTE OS_DPTR *)pArray + sizeof(IEC_UINT))

#endif


#define RTS_PRAGMA_PACK_1	/* >>>> Align 1 Begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_1


/* FILE FUNCTION STRUCTURES
 * ----------------------------------------------------------------------------
 */

typedef struct
{
	IEC_STRLEN	CurLen;
	IEC_STRLEN	MaxLen;
	IEC_CHAR	Contents[VMM_MAX_IEC_STRLEN];

} LOC_STRING;

#if defined(IP_CFG_INST8)	/* ------------------------------------------------ */

typedef struct
{
	DEC_VAR(LOC_STRING, sName); 		/* File name							*/
	DEC_VAR(IEC_UDINT,	ulMode);		/* File access mode 					*/
	DEC_VAR(IEC_DINT,	lPos);			/* Start position.						*/
	DEC_VAR(IEC_UDINT,	ulError);		/* Last error code						*/
	DEC_VAR(IEC_BOOL,	bOpen); 		/* File open?							*/

} FA_SYNC_FILE;

#endif /* IP_CFG_INST8 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST16)	/* ------------------------------------------------ */

typedef struct
{
	DEC_VAR(LOC_STRING, sName); 		/* File name							*/
	DEC_VAR(IEC_BYTE,	dummy_08_fileName);
	DEC_VAR(IEC_UDINT,	ulMode);		/* File access mode 					*/
	DEC_VAR(IEC_DINT,	lPos);			/* Start position.						*/
	DEC_VAR(IEC_UDINT,	ulError);		/* Last error code						*/
	DEC_VAR(IEC_BOOL,	bOpen); 		/* File open?							*/

} FA_SYNC_FILE;

#endif /* IP_CFG_INST16 */	/* ------------------------------------------------ */

#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64) /* ----------------------- */

typedef struct
{
	DEC_VAR(LOC_STRING, sName); 		/* File name							*/
	DEC_VAR(IEC_BYTE,	dummy_08_fileName);
	DEC_VAR(IEC_WORD,	dummy_16_fileName);
	DEC_VAR(IEC_UDINT,	ulMode);		/* File access mode 					*/
	DEC_VAR(IEC_DINT,	lPos);			/* Start position.						*/
	DEC_VAR(IEC_UDINT,	ulError);		/* Last error code						*/
	DEC_VAR(IEC_BOOL,	bOpen); 		/* File open?							*/

} FA_SYNC_FILE;

#endif /* IP_CFG_INST32 || IP_CFG_INST64 */ /* -------------------------------- */


typedef struct fa_fd_pair_tag
{
	FA_SYNC_FILE*		   pFile;
	IEC_UDINT			   hFile;
	struct fa_fd_pair_tag *pNext;

} fa_fd_pair, fa_fd_map;


/* --- 160 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (FA_SYNC_FILE,	file);	
	
	DEC_FUN_BOOL(bRet); 				

} FA_SYNC_OPENFILE_PAR;

/* --- 161 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (FA_SYNC_FILE,	file);
	DEC_FUN_PTR (IEC_UDINT, 	ulLength);
	DEC_FUN_PTR (IEC_STRING,	pData);
	
	DEC_FUN_BOOL(bRet);

} FA_SYNC_READFILE_PAR;

/* --- 162 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_UDINT(ulPos);
	DEC_FUN_PTR  (FA_SYNC_FILE, file);
	DEC_FUN_PTR  (IEC_UDINT,	ulLength);
	DEC_FUN_PTR  (IEC_DATA, 	pData);
	
	DEC_FUN_BOOL(bRet);

} FA_SYNC_ARRAYREADFILE_PAR;

/* --- 163 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING,	newName);
	DEC_FUN_PTR (FA_SYNC_FILE,	file);
	
	DEC_FUN_BOOL(bRet);

} FA_SYNC_RENAMEFILE_PAR;

/* --- 164 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING,	pData);
	DEC_FUN_PTR (FA_SYNC_FILE,	file);

	DEC_FUN_BOOL(bRet);

} FA_SYNC_WRITEFILE_PAR;

/* --- 164 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR  (IEC_BYTE, 	pData);
	DEC_FUN_UDINT(ulPos);
	DEC_FUN_UDINT(ulLen);
	DEC_FUN_PTR  (FA_SYNC_FILE, file);
	
	DEC_FUN_BOOL(bRet);

} FA_SYNC_ARRAYWRITEFILE_PAR;

/* --- 166 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_UDINT(ulRet);

} FA_FLUSH_PAR;

/* --- 167 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (FA_SYNC_FILE, file);

	DEC_FUN_BOOL(bRet);

} FA_SYNC_CLOSEFILE_PAR;

/* --- 168 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (FA_SYNC_FILE, file);

	DEC_FUN_BOOL(bRet);

} FA_SYNC_CREATEDIRECTORY_PAR;

/* --- 169 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (FA_SYNC_FILE, file);

	DEC_FUN_BOOL(bRet);

} FA_SYNC_DELETEFILE_PAR;

/* --- 170 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR(FA_SYNC_FILE, file);

	DEC_FUN_BOOL(bRet);

} FA_SYNC_EXISTSFILE_PAR;

/* --- 171 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (FA_SYNC_FILE, file);
	DEC_FUN_PTR (IEC_DINT,	   lpSize);

	DEC_FUN_BOOL(bRet);

} FA_SYNC_GETSIZE_PAR;

/* --- 172 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_PTR (IEC_STRING, sPath);
	DEC_FUN_PTR (IEC_DINT,	lpFree);

	DEC_FUN_UDINT(ulRet);

} FA_GET_DISKFREESPACE_PAR;

/* --- 173 -------------------------------------------------------------------- */

typedef struct
{
	DEC_FUN_DINT(lError);

	DEC_FUN_PTR (IEC_STRING, sError);

} FA_ERROR_STRING_PAR;


#define RTS_PRAGMA_PACK_DEF 	
#include "osAlign.h"
#undef	RTS_PRAGMA_PACK_DEF 	/* <<<< Align 1 end <<<<<<<<<<<<<<<<<<<<<<<<<<< */

#endif	/* RTS_CFG_FILE_LIB */ 

#endif	/* _LIBFILE_H_ */

/* ---------------------------------------------------------------------------- */
