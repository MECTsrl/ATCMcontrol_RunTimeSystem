
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
 * Filename: md5.h
 */

 
 #ifndef _MD5_H_
 #define _MD5_H_
 
 /*
 **********************************************************************
 ** md5.h -- Header file for implementation of MD5					 **
 ** RSA Data Security, Inc. MD5 Message Digest Algorithm			 **
 ** Created: 2/17/90 RLR											 **
 ** Revised: 12/27/90 SRD,AJ,BSK,JT Reference C version 			 **
 ** Revised (for MD5): RLR 4/27/91									 **
 **   -- G modified to have y&~z instead of y&z 					 **
 **   -- FF, GG, HH modified to add in last register done			 **
 **   -- Access pattern: round 2 works mod 5, round 3 works mod 3	 **
 **   -- distinct additive constant for each step					 **
 **   -- round 4 added, working mod 7								 **
 **********************************************************************
 */

/*
 **********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
 ** 																 **
 ** License to copy and use this software is granted provided that	 **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message     **
 ** Digest Algorithm" in all material mentioning or referencing this **
 ** software or this function.										 **
 ** 																 **
 ** License is also granted to make and use derivative works		 **
 ** provided that such works are identified as "derived from the RSA **
 ** Data Security, Inc. MD5 Message Digest Algorithm" in all         **
 ** material mentioning or referencing the derived work.			 **
 ** 																 **
 ** RSA Data Security, Inc. makes no representations concerning 	 **
 ** either the merchantability of this software or the suitability	 **
 ** of this software for any particular purpose.  It is provided "as **
 ** is" without express or implied warranty of any kind.             **
 ** 																 **
 ** These notices must be retained in any copies of any part of this **
 ** documentation and/or software.									 **
 **********************************************************************
 */

/* typedef a 32 bit type */
typedef unsigned long int UINT4;

/* Data structure for MD5 (Message Digest) computation */
typedef struct {
  UINT4 i[2];					/* number of _bits_ handled mod 2^64 */
  UINT4 buf[4]; 								   /* scratch buffer */
  unsigned char in[64]; 							 /* input buffer */
  unsigned char digest[16]; 	/* actual digest after MD5Final call */
} MD5_CTX;

void MD5Init ();
void MD5Update ();
void MD5Final ();

#endif /* _MD5_H_ */

/* ---------------------------------------------------------------------------- */
