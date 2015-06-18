
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
 * Filename: osAlign.h
 */


/* osAlign.h
 * ----------------------------------------------------------------------------
 * Switch compiler alignmnet to 1 byte for shared communication protocoll
 * structures and IEC function structures.
 */
#if defined(RTS_PRAGMA_PACK_1)
	/* Set compiler alignment to 1 byte
	 */
	#pragma pack(1)
    #define UNALIGNED  __attribute__((packed))

#elif defined(RTS_PRAGMA_PACK_DEF)
	/* Set compiler aligment back to default
	 */
	#pragma pack()
    #define UNALIGNED  __attribute__((packed))

#else
	#error	Alignment type not defined!
#endif


/* ---------------------------------------------------------------------------- */
