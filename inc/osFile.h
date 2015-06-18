
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
 * Filename: osFile.h
 */


#ifndef _OSFILE_H_
#define _OSFILE_H_

/* Operating system dependent include files
 * ----------------------------------------------------------------------------
 */
#include <sys/stat.h>


/* File IO Definitions
 * ----------------------------------------------------------------------------
 */
#define VMF_INVALID_HANDLE			NULL			/* Invalid handle value */

#define VMF_FILE					FILE *			/* File pointer 		*/

#define VMF_MODE_READ				"r" 			/* File open mode:		*/
#define VMF_MODE_WRITE				"w+"			/* -> Access mode		*/
#define VMF_MODE_APPEND 			"a+"
#define VMF_MODE_RDWR				"r+"

#define VMF_MODE_TEXT				"t" 			/* File open mode:		*/
#define VMF_MODE_BIN				"b" 			/* -> Text/binary file	*/

#define VMF_DIR_READ				02555
#define VMF_DIR_WRITE				02775

#define VMF_SEEK_CUR				SEEK_CUR		/* Seek origins 		*/
#define VMF_SEEK_END				SEEK_END
#define VMF_SEEK_SET				SEEK_SET


/* Return Values
 * ----------------------------------------------------------------------------
 */
#define VMF_RET_OK					0				/* Positive return val. */


/* API functions
 * ----------------------------------------------------------------------------
 */
#define osfclose(str)				fclose(str)
#define osfopen(nam,mod)			fopen(nam,mod)
#define osfseek(str,off,org)		fseek(str,off,org)
#define osfread(buf,siz,cnt,str)	fread(buf,siz,cnt,str)
#define osfeof(str) 				feof(str)
#define osfgets(buf,n,str)			fgets(buf,n,str)
#define osfwrite(buf,siz,cnt,str)	fwrite(buf,siz,cnt,str)
#define osfputs(buf,str)			fputs(buf,str)
#define osfflush(str)				fflush(str)

#define osstrerror					strerror(errno)

#define osremove(nam)				unlink(nam)
#define osrename(from,to)			rename(from,to)

#define oschdir(dn) 				chdir(dn)
#define osmkdir(dn,mod) 			mkdir(dn,mod)
#define oschdrive(dn)				VMF_RET_OK

#define osstat(fn,buf)				stat(fn,buf)
#define osstructstat				stat

#endif /* _OSFILE_H_ */

/* ---------------------------------------------------------------------------- */
