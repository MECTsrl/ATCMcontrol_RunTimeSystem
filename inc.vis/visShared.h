
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
 * Filename: visShared.h
 */


#ifndef _VISSHARED_H_
#define _VISSHARED_H_


/* visShared.h
 * ----------------------------------------------------------------------------
 * This file defines the external interface of the VisuComm library. It should
 * be the only file to be included in ordert to use the VisuComm libary.
 */


/* Common prerequisites:
 * ----------------------------------------------------------------------------
 *
 * * All memory provided to the or retrieved from the VisuComm library must be
 *	 allocated resp. freed with the functions visAlloc() resp. visFree().
 *
 * * The contents of member variables of structures whose names are beginning 
 *	 with an 'S' are always expected to be in the big/little endian format of
 *	 the VisuComm library (client) processor and are not converted by the 
 *	 VisuComm library itself.
 *
 * * The contents of member variables of structures whose names are beginning
 *	 with an 'X' are always expected to be in the big/little endian format of
 *	 the remote Run Time System (server) processor and are not converted by 
 *	 the VisuComm library itself.
 *
 * * IEC variable values (retrieved by GetValue, resp. provided within SetValue)
 *	 as member variables of XVariableXX resp. XValue structures are always 
 *	 expected to be in the big/little endian format fo the VisuComm library
 *	 (client) processor and are converted by the VisuComm library in both 
 *	 directions if necessary.
 *
 */ 


/* Advises the visInitialize() function to try to do a portscan over all know
 * FarosPLC Run Time System TCP/IP port numbers.
 */
#define VIS_PORT_SCAN	0xffffu


/* Data types used by the VisuComm library. They must be defined target specific
 * so that the given data sizes are achieved.
 */
#define VIS_USINT		unsigned char			/*	8 bit unsigned integer	*/
#define VIS_UINT		unsigned short			/* 16 bit unsigned integer	*/
#define VIS_UDINT		unsigned				/* 32 bit unsigned integer	*/
#define VIS_CHAR		char					/*	8 bit signed integer	*/
#define VIS_BOOL		unsigned char			/*	8 bit unsigned integer	*/
#define VIS_DATA		unsigned char			/*	8 bit unsigned integer	*/



/* Function declarations
 * ----------------------------------------------------------------------------
 */

#ifdef __cplusplus
extern "C" { 
#endif


/* ---------------------------------------------------------------------------- */
/**
 * visIsInitialized
 *
 * Verifies if the VisuComm library is fully initialized. The library is fully 
 * initialized if it has a valid connection to a remote 4C Run Time System and 
 * the remote RTS configuration has been received correctly.
 *
 * return			TRUE if fully initialized, else FALSE.
 *
 */
VIS_BOOL visIsInitialized(void);


/* ---------------------------------------------------------------------------- */
/**
 * visIsLoggedIn
 *
 * Verifies if the VisuComm library is logged into a IEC project on a remote
 * FarosPLC Run Time System.
 *
 * return			TRUE if logged into a IEC project, else FALSE.
 *
 */
VIS_BOOL visIsLoggedIn(void);


/* ---------------------------------------------------------------------------- */
/**
 * visGetFirmware
 *
 * Retrieves the firmware version of the connected remote FarosPLC Run Time 
 * System. If the VisuComm library is not fully initialized, this function 
 * returns zero.
 *
 * return			Firmware version of connected 4C RTS.
 *
 */
VIS_UDINT visGetFirmware(void);


/* ---------------------------------------------------------------------------- */
/**
 * visNeedByteSwap
 *
 * This function returns TRUE if data from or to the connected remote FarosPLC
 * Run Time System must be byte swapped.
 *
 * return			TRUE if byte swapp is necessary, else FALSE.
 *
 */
VIS_BOOL visNeedByteSwap(void);


/* ---------------------------------------------------------------------------- */
/**
 * visGetMaxData
 *
 * Retrieves the maximal communication buffer size for the communication with the
 * actual connected FarosPLC Run Time System. If the VisuComm library is not fully 
 * initialized, this function returns zero
 *
 * return			Communication buffer size in bytes.
 *
 */
VIS_UDINT visGetMaxData(void);


/* ---------------------------------------------------------------------------- */
/**
 * visAlloc
 *
 * Allocates the given amount of memory. All memory passed to this library must
 * be allocated with this function.
 *
 * I	ulLen		Amount of memory (in bytes to be allocated.)
 *
 * return			Pointer to allocated memory or NULL if failed.
 *
 */
VIS_DATA *visAlloc(VIS_UDINT ulLen);


/* ---------------------------------------------------------------------------- */
/**
 * visFree
 *
 * Releases (frees) memory. All memory retrieved from the VisuComm library must
 * be release with this function.
 *
 * IO	ppMemory	Pointer to pointer to memory to be freed. (Original pointer
 *					will be nulled after the memory release.)
 *
 * return			void.
 *
 */
void visFree(VIS_DATA **ppMemory);


/* ---------------------------------------------------------------------------- */
/**
 * visSwap16
 *
 * The function domSwap16 executes a byte swap on the given 16 bit value if it
 * is necessary. (The big/little endian configuration of this libraries host and
 * the attached remote RTS is different.)
 *
 * Note: This function only works if this library is connected to a remote 
 *		 FarosPLC Run Time System. (If it is not connected, this function 
 *		 returns always the input value untouched!)
 *
 * I	uVal		16 bit input value to be converted if necessary.
 *
 * return			Swapped/Original 16 bit value - see above.
 *
 */
VIS_UINT  visSwap16(VIS_UINT  uVal);


/* ---------------------------------------------------------------------------- */
/**
 * visSwap32
 *
 * The function domSwap32 executes a byte swap on the given 32 bit value if it
 * is necessary. (The big/little endian configuration of this libraries host and
 * the attached remote RTS is different.)
 *
 * Note: This function only works if this library is connected to a remote 
 *		 FarosPLC Run Time System. (If it is not connected, this function 
 *		 returns always the input value untouched!)
 *
 * I	uVal		32 bit input value to be converted if necessary.
 *
 * return			Swapped/Original 32 bit value - see above.
 *
 */
VIS_UDINT visSwap32(VIS_UDINT ulVal);


/* ---------------------------------------------------------------------------- */
/**
 * visSwap64
 *
 * The function domSwap64 executes a byte swap on the given 64 bit value if it
 * is necessary. (The big/little endian configuration of this libraries host and
 * the attached remote RTS is different.)
 *
 * Note: This function only works if this library is connected to a remote 
 *		 FarosPLC Run Time System. (If it is not connected, this function 
 *		 returns always the input value untouched!)
 *
 * I	uVal		64 bit input value to be converted if necessary.
 *
 * return			Swapped/Original 64 bit value - see above.
 *
 */
void visSwap64(VIS_DATA *pVaL);



/* ---------------------------------------------------------------------------- */
/**
 * visError
 *
 * Determines an error text to a given FarosPLC Run Time System error number.
 *
 * I	uErrNo		Error number returned by a 4C RTS function.
 *
 * return			Corresponding error message text.
 *
 */
VIS_CHAR *visError(VIS_UINT uErrNo);


/* ---------------------------------------------------------------------------- */
/**
 * visConvertVarToVal
 *
 * This function converts a (variable sized) XVariableXX structure into a fixed
 * size XVariable structure to be used with the visSetValue() function.
 *
 * See the visGetValue() and the visSetValue() function for a detailed description 
 * of the different applications of the the different XVariableXX structures.
 *
 * I	pVar		Pointer to the variable structure to be converted.
 * I	uVar		Size of the variable structure data.
 *
 * O	pxDest		Converted variable structures to be used within visSetValue().
 *
 * return			OK if successful, else error number.
 *
 */
VIS_UINT visConvertVarToVal(VIS_DATA *pVar, VIS_UINT uVar, VIS_DATA *pxDest);


/* ---------------------------------------------------------------------------- */
/**
 * visInitialize
 *
 * This function initializes the VisuComm interface, establishes a TCP/IP
 * connection to a FarosPLC Run Time System and gets its corresponding 
 * configuration.
 *
 * I	szAddr		TCP/IP address of the remote 4C Run Time System.
 * I	uPort		Port number of the remote 4C Run Time System.
 *					VIS_PORT_SCAN if the function should try to scan
 *					for the port number by itself. (May take some time!)
 *
 * return			OK if successful, else error number.
 *
 */
VIS_UINT visInitialize(VIS_CHAR const *szAddr, VIS_UINT uPort);


/* ---------------------------------------------------------------------------- */
/**
 * visFinalize
 *
 * Closes any connection to a remote 4C Run Time System and skips any 
 * stored configuration data.
 *
 * return			OK if successful, else error number.
 */
VIS_UINT visFinalize(void);


/* ---------------------------------------------------------------------------- */
/**
 * visLogin
 *
 * Login to a connected remote FarosPLC Run Time System and retrieve the actual
 * project ID from the attached RTS.
 * 
 * This function will fail, if no project is loaded on the attached RTS.
 *
 * return				OK if successful, else error number.
 */
VIS_UINT visLogin(void);


/* ---------------------------------------------------------------------------- */
/**
 * visLogout
 *
 * Logout from a connected remote FarosPLC Run Time System.
 *
 * Important: After calling this function, any stored FarosPLC interpreter 
 * addresses must be abandoned and reloaded after a new login.
 *
 * return			OK if successful, else error number.
 */
VIS_UINT visLogout(void);


/* ---------------------------------------------------------------------------- */
/**
 * visCheckProject
 *
 * Verifies if the currently loaded project ID of this library instance is still
 * the same as of the project ID of the attached FarosPLC Run Time System.
 *
 * Note: If this function fails, all stored addresses must be abandoned and 
 * reloaded again from the attached RTS. (Possibly a new download to the remote
 * RTS has been done.)
 *
 * return			OK if successful, else error number.
 *
 */
VIS_UINT visCheckProject(void);


/* ---------------------------------------------------------------------------- */
/**
 * visGetChildren
 *
 * Gets the children (or member variables) of an given IEC variable. The IEC
 * variable must be given in a chain of consecutive XVisuVar objects.
 * This XVisuVar chain can also be represented in the following format
 * (ZB == zero byte):
 *
 * | XDBIVar | VarName | ZB | ... | ... | XDBIVar | VarName | ZB | ZB
 *
 * Each XDBIVar/VarName/ZB pair represents one hierarchy of the corresponding
 * fully qualified IEC identifier (i.e. prog_inst1.data_type_inst_1.member1).
 * 
 * The uInst member of the different XVisuVar/XDBIVar structures must be 
 * initialized with 0xffff, the usVarSize member is the string length of the 
 * variable name without the terminating zero byte.
 *
 * All other members are ignored by the remote Run Time System. However, in the
 * result (the different child variables) all member variables of this structure
 * are valid for simple IEC variables. (So the corresponding values can be 
 * used directly to access the corresponding data values without calling the
 * GetAddress() function first.
 *
 * The result (the corresponding children or member variables) is given in the 
 * same format as described above, except that each list entry shows only the 
 * specific variable and not the fully qualified (hierarchical) identifier.
 * 
 * So a specific element of the result list (a specific child or member 
 * variable) can be attached directly to the original IEC identifier (with the
 * function visAppendDBIVar() in order to compute again the fully qualified 
 * (hierarchical) IEC identifier in DBI format.
 *
 * IO	ppData		I	Pointer to IEC identifies (variable) in DBI format.
 *					O	Pointer to children / member list in DBI format.
 *						Note: This function allocates new memory for the 
 *							  result. This memory must be released by 
 *							  the caller!
 *
 * IO	upLen		I	Length in bytes of the IEC identifier.
 *					O	Length in bytes of children list.
 *
 * I	bRelease	TRUE if memory of *ppData should be released by this
 *					function.
 *
 * return			OK if successful, else error number.
 *
 */
VIS_UINT visGetChildren(VIS_DATA **ppData, VIS_UINT *upLen, VIS_BOOL bRelease);


/* ---------------------------------------------------------------------------- */
/**
 * visGetAddrType
 *
 * Gets the interpreter address of an given IEC identifier (variable). The IEC
 * identifier must be given in the following DBI format (ZB == zero byte):
 *
 * | XDBIVar | VarName | ZB | ... | ... | XDBIVar | VarName | ZB | ZB
 *
 * Each XDBIVar/VarName/ZB pair represents one hierarchy of the corresponding
 * fully qualified IEC identifier (i.e. prog_inst1.data_type_inst_1.member1).
 * 
 * The uInst member of the different XDBIIdent structures must be initialized
 * with 0xffff, the usVarSize member is the string length of the variable
 * name without the ZB.
 *
 * The result is given in XVariableXX structures depending from the firmware
 * version of the attached remote 4C Run Time System. See the function
 * GetValue() for details.
 *
 * IO	ppData		I	Pointer to IEC identifier (variable) in DBI format.
 *					O	Pointer to address of variable (XVariableXX format).
 *						Note: This function allocates new memory for the 
 *							  result. This memory must be released by 
 *							  the caller!
 *
 * IO	upLen		I	Length in bytes of the IEC variable.
 *					O	Length in bytes of address of variable.
 *
 * O	upType		Type of variable. See DBI_DTM_xxx and DBI_DT_xxx 
 *					definitions in vmShared.h.
 *
 * I	bRelease	TRUE if memory of *ppData should be released.
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visGetAddrType(VIS_DATA **ppData, VIS_UINT *upLen, VIS_USINT *uspType, VIS_BOOL bRelease);

/* ---------------------------------------------------------------------------- */
/**
 * visGetAddress
 *
 * Gets the interpreter address of an given IEC identifier (variable). The IEC
 * identifier must be given in the following DBI format (ZB == zero byte):
 *
 * | XDBIVar | VarName | ZB | ... | ... | XDBIVar | VarName | ZB | ZB
 *
 * Each XDBIVar/VarName/ZB pair represents one hierarchy of the corresponding
 * fully qualified IEC identifier (i.e. prog_inst1.data_type_inst_1.member1).
 * 
 * The uInst member of the different XDBIIdent structures must be initialized
 * with 0xffff, the usVarSize member is the string length of the variable
 * name without the ZB.
 *
 * The result is given in XVariableXX structures depending from the firmware
 * version of the attached remote 4C Run Time System. See the function
 * GetValue() for details.
 *
 * IO	ppData		I	Pointer to IEC identifier (variable) in DBI format.
 *					O	Pointer to address of variable (XVariableXX format).
 *						Note: This function allocates new memory for the 
 *							  result. This memory must be released by 
 *							  the caller!
 *
 * IO	upLen		I	Length in bytes of the IEC variable.
 *					O	Length in bytes of address of variable.
 *
 * I	bRelease	TRUE if memory of *ppData should be released.
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visGetAddress(VIS_DATA **ppData, VIS_UINT *upLen, VIS_BOOL bRelease);


/* ---------------------------------------------------------------------------- */
/**
 * visGetValue
 *
 * Gets the values for the given variable list from the attached FarosPLC 
 * Run Time System. On attached RTS prior to firmware version 21000, the 
 * variable list is given as an arbitrary length array of XVariable structures.
 * representing the corresponding IEC addresses.
 *
 * For firmware version after and including 21000 it is an array of variable
 * sized structures XVariableS, XVariableM, XVariableL. The type and the size
 * is of these structures is stored in the first byte (the member variable
 * usType).
 *
 * The corresponding structure type can be computed using the following 
 * algorithm:
 * (**ppData & VMM_XV_TYPEMASK) == VMM_XV_SMALL 	==> XVariableS
 * (**ppData & VMM_XV_TYPEMASK) == VMM_XV_MEDIUM	==> XVariableM
 * (**ppData & VMM_XV_TYPEMASK) == VMM_XV_LARGE 	==> XVariableL
 *
 * For both versions, the corresponding XVariableXX structures must be received
 * by the visGetAddress() function. (Addresses in the FarosPLC interpreter ares
 * computed also during Run Time and may be different on each system boot.)
 *
 * The number of variable elements in this list is not limited.
 *
 * As a result, the corresponding data values are delivered directly without
 * the original XVariableXX address structure objects.
 * 
 * Regarding big/little endian processors, the data values are always converted
 * into the correct format of this libraries host. (The member variables of 
 * the different XVariableXX structures must always be in the remote format!)
 *
 * IO	ppData		I	Pointer to an address list (see above).
 *					O	Pointer to the corresponding data values.
 *						Note: This function allocates memory for the result.
 *						This memory must be released by the caller!
 *
 * IO	upLen		I	Length (in bytes) of address list.
 *					O	Length (in bytes) of data value list.
 *
 * I	bRelease	TRUE if input memory of *ppData should be released by this
 *					function.
 * 
 * return			OK if successful, else error number.
 *
 */
VIS_UINT visGetValue(VIS_DATA **ppData, VIS_UINT *upLen, VIS_BOOL bRelease);


/* ---------------------------------------------------------------------------- */
/**
 * visSetValue
 *
 * Writes the given value list into the attached FarosPLC Run Time System.
 *
 * The value list is given by multiple [XValue | Data] pairs. The length of the 
 * different data blocks is defined by the corresponding member of the 
 * associated XValue structure.
 *
 * The number of variable elements in this list is not limited.
 *
 * Regarding big/little endian processors, the data values must be provided
 * in the format of this libraries host and are converted by this function
 * if necessary. (The member variables of the different XValue address structures
 * must be provided always in the format of the attached remote 4C RTS.)
 *
 * Note, that the embedded XVariable address objects are always the same for 
 * each firmware version. So for firmware versions beginning with 21000, the 
 * XVariableXX objects received by the visGetAddress() function must be converted
 * accordingly. (This can be done by using the function visConvertVarToVal().)
 *
 * IMPORTANT: If necessary, the data values provided to this function are swapped.
 *			  This is done in place, so even if bRelease equals FALSE and the 
 *			  memory area is not released by this function the original memory
 *			  area is changed and can not be used a second time with this function.
 *
 * I	ppData		Pointer to an address/data list (see above).
 * I	upLen		Length (in bytes) of address/data list.
 * I	bRelease	TRUE if input memory of *ppData should be released by this
 *					function. (*upLen will be set to zero in this case also.)
 *
 * return			OK if successful, else error number.
 *					
 */
VIS_UINT visSetValue(VIS_DATA **ppData, VIS_UINT *upLen, VIS_BOOL bRelease);


/* ---------------------------------------------------------------------------- */
/**
 * visGetTaskState
 *
 * Retrieves the task state of the a task of the IEC project of the connected 
 * Run Time System.
 *
 * I	szTask		Task name of the corresponding task.
 * O	ulpState	Task state of the task of the attached IEC project. See 
 *					vmmDef.h for possible values:
 *
 *					#define TASK_STATE_ERROR		0xffffffffu
 *					#define TASK_STATE_ONCREATION	0x00000000u
 *					#define TASK_STATE_STOPPED		0x00000001u
 *					#define TASK_STATE_RUNNING		0x00000002u
 *					#define TASK_STATE_BREAK		0x00000003u
 *					#define TASK_STATE_STEP 		0x00000005u
 *					#define TASK_STATE_DELETED		0x00000006u
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visGetTaskState(VIS_CHAR *szTask, VIS_UDINT *ulpState);


/* ---------------------------------------------------------------------------- */
/**
 * visGetResourceState
 *
 * Retrieves the resource state of the IEC project (if any) of the connected 
 * Run Time System.
 *
 * O	ulpState	Resource state of attached IEC project. See vmmDef.h for
 *					possible values:
 *
 *					#define RES_STATE_NOTRUNNING	0xfffffffeu
 *					#define RES_STATE_ERROR 		0xffffffffu
 *					#define RES_STATE_ONCREATION	0x00000000u
 *					#define RES_STATE_PAUSED		0x00000001u
 *					#define RES_STATE_RUNNING		0x00000002u
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visGetResourceState(VIS_UDINT *ulpState);


/* ---------------------------------------------------------------------------- */
/**
 * visCreateVisuVar
 *
 * Creates an empty VisuComm variable (XVisuVar object) which can be used as a 
 * root element in order to retrieve the children of the IEC project on the 
 * highest level.
 *
 * If the provided pointer is not NULL the old memory is freed by this function.
 * The memory allocated by this function must be freed by using the visFree()
 * function.
 *
 * IO	ppDBIVar	I	Pointer to an old VisuComm object (will be freed).
 *					O	Pointer to an empty(root) VisuComm variable.
 *						Note: This function allocates memory for the result.
 *						This memory must be released by the caller!
 *
 * IO	upDBIVar	I	Length (in bytes) of VisuComm variable.
 *					O	Length (in bytes) of VisuComm variable.
 * 
 * return			OK if successful else error number.
 *
 */
VIS_UINT visCreateVisuVar(VIS_DATA **ppDBIVar, VIS_UINT *upDBIVar);


/* ---------------------------------------------------------------------------- */
/**
 * visAppendDBIVar
 *
 * This function appends a given VisuComm variable (pNewVar, XVisuVar object) 
 * to an existing VisuComm variable (chain of XVisuVar objects).
 *
 * The memory allocated by this function must be freed by using the visFree()
 * function!
 *
 * IO	ppDBIVar	I	Pointer to an existing VisuComm variable (XVisuVar objects,
 *						will be freed or re-allocated).
 *					O	Pointer to the new, extended VisuComm variable.
 *						Note: This function allocates memory for the result.
 *						This memory must be released by the caller!
 *
 * IO	upDBIVar	I	Length (in bytes) of existing VisuComm variable.
 *					O	Length (in bytes) of extended VisuComm variable.
 *
 * I	pNewVar 	VisuComm Variable (XVisuVar object) to be appended.
 * 
 * return			OK if successful else error number.
 *
 */
VIS_UINT visAppendDBIVar(VIS_DATA **ppDBIVar, VIS_UINT *upDBIVar, VIS_DATA *pNewVar);


/* ---------------------------------------------------------------------------- */
/**
 * visTruncDBIVar
 *
 * This function truncates the last VisuComm variable (XVisuVar object) from a 
 * given VisuComm variable (chain of XVisuVar objects).
 *
 * If the last VisuComm variable is removed from the variable chain, a new, 
 * empty VisuComm variable is created.
 *
 * The memory allocated by this function must be freed by using the visFree()
 * function!
 *
 * IO	ppDBIVar	I	Pointer to an existing VisuComm variable (XVisuVar objects,
 *						will be freed or re-allocated).
 *					O	Pointer to the new, truncated VisuComm variable.
 *						Note: This function allocates memory for the result.
 *						This memory must be released by the caller!
 *
 * IO	upDBIVar	I	Length (in bytes) of existing VisuComm variable.
 *					O	Length (in bytes) of extended VisuComm variable.
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visTruncDBIVar(VIS_DATA **ppDBIVar, VIS_UINT *upDBIVar);


/* ---------------------------------------------------------------------------- */
/**
 * visConvertDBIVarToVar
 *
 * This function converts a VisuComm variable (a XVisuVar object) into a
 * variable object (XVariableXX object) which can be used directly with the 
 * visGetValue() function.
 *
 * If the last VisuComm variable is removed from the variable chain, a new, 
 * empty VisuComm variable is created.
 *
 * The memory allocated by this function must be freed using visFree()!
 *
 * I	ppDBIVar	Pointer to a VisuComm variable (XVisuVar object).
 *
 * O	ppVar		New XVariableXX object. (If ppVar is not NULL the
 *					corresponding memory is freed by this function.)
 *
 * O	upVar		Length (in bytes) of variable.
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visConvertDBIVarToVar(VIS_DATA *pDBIVar, VIS_DATA **ppVar, VIS_UINT *upVar);


/* ---------------------------------------------------------------------------- */
/**
 * visRetainWrite
 *
 * This function advices the connected FarosPLC Run Time System to write the 
 * retentive memory area to the file system. (This function is only supported
 * on Run Time Systems with no real physical retain memory.)
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visRetainWrite(void);


/* ---------------------------------------------------------------------------- */
/**
 * visRetainSetCycle
 *
 * With this function, the cycle time of the cyclic retain write process to the
 * file system can be set. A cycle time of 0 stops the update process. (This 
 * function is only supported on Run Time Systems with no real physical retain 
 * memory.)
 *
 * I	ulCycle 	Retain write cycle time. (0 to stop writing.)
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visRetainSetCycle(VIS_UDINT ulCycle);


/* ---------------------------------------------------------------------------- */
/**
 * visFWGetResult - Pre-Release, not finished - use at own risk.
 *
 * This function can be used in order to transfer a firmware image or the firmware
 * update configuration file to the attached FarosPLC Run Time System.
 *
 * The given file is stored to the local file system. The firmware update process
 * itself must be started and handled with the functions visFWExecute() and 
 * visFWGetResult().
 *
 * I	pData		Pointer to the firmware download file.
 *					Format: FileName | Zero Byte | File Data
 *
 * I	ulLen		Length in bytes of the firmware download file.
 *
 * return			OK if successful, else error number.
 *
 */
VIS_UINT visFWDownload(VIS_DATA *pData, VIS_UDINT ulLen);


/* ---------------------------------------------------------------------------- */
/**
 * visFWGetResult - Pre-Release, not finished - use at own risk.
 *
 * This function starts the actual firmware update process. The state and result
 * of the update can (resp. must) be polled with the function visFWGetResult().
 *
 * The specified firmware update configuration file must be previously downloaded
 * with the function visFWDownload().
 *
 * I	ulClientID		Client ID of the actual Run Time System to be updated.
 *						(0xfffffffful if the attached RTS should be updated.)
 *
 * I	szConfigFile	File name of the firmware update configuration file
 *						previously downloaded with the function visFWDownload().
 *
 * return				OK if successful, else error number.
 *
 */
VIS_UINT visFWExecute(VIS_UDINT ulClientID, VIS_CHAR *szConfigFile);


/* ---------------------------------------------------------------------------- */
/**
 * visFWGetResult - Pre-Release, not finished - use at own risk.
 *
 * This function polls the state resp. result of a previously started firmware
 * update process.
 *
 * The initial value of upResult must be zero. If the update process is still
 * in progress this value is returned incremented by one. If the update process 
 * is ready *upResult equals zero.
 *
 * IO	upResult		I	Zero or last returned value.
 *						O	Zero if ready, else input value incremented by one.
 *
 * return				OK if successful, else error number.
 *
 */
VIS_UINT visFWGetResult(VIS_UINT *upResult);


/* ---------------------------------------------------------------------------- */
/**
 * visGetInstKey
 *
 * Reads the installation key from the attached FarosPLC Run Time System.
 *
 * O	ppKey		Pointer to the installation key. (String, zero byte
 *					terminated.
 *					Note: This function allocates new memory for the 
 *						  result. This memory must be released by 
 *						  the caller!
 *
 * O	upLen		Length in bytes of the installation key.
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visGetInstKey(VIS_DATA **ppKey, VIS_UINT *upLen);


/* ---------------------------------------------------------------------------- */
/**
 * visSetLicKey
 *
 * Writes a license key to the attached FarosPLC Run Time System.
 *
 * I	pKey		Pointer to the license key. (String, zero byte terminated.)
 *
 * I	uLen		Length in bytes of the license key.
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visSetLicKey(VIS_DATA *pKey, VIS_UINT uLen);


/* ---------------------------------------------------------------------------- */
/**
 * visGetSerialNo
 *
 * Reads the serial number of the attached FarosPLC Run Time System.
 *
 * O	ulpSN		Pointer to the serial number.
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visGetSerialNo(VIS_UDINT *ulpSN);


/* ---------------------------------------------------------------------------- */
/**
 * visGetFeature
 *
 * Reads the available and licensed features of the attached FarosPLC Run Time 
 * System. (The association of the different bits to corresponding Run Time
 * System features is target dependent.)
 *
 * O	upAvailable Pointer to available features.
 *
 * O	upLicensed	Pointer to licensed features.
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visGetFeature(VIS_UINT *upAvailable, VIS_UINT *upLicensed);


/* ---------------------------------------------------------------------------- */
/**
 * visGetTargetType
 *
 * Reads the target type (i.e. "FarosPLC BuildingController" from the attached 
 * FarosPLC Run Time System.
 *
 * O	ppType		Pointer to the target type. (String, zero byte terminated).
 *					Note: This function allocates new memory for the 
 *						  result. This memory must be released by 
 *						  the caller!
 *
 * O	upLen		Length in bytes of the target type.
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visGetTargetType(VIS_DATA **ppType, VIS_UINT *upLen);


/* ---------------------------------------------------------------------------- */
/**
 * visGetTargetVersion
 *
 * Reads the target version (i.e. "V2.1.3 (V123.B3550.r01)" from the attached 
 * FarosPLC Run Time System.
 *
 * O	ppType		Pointer to the target version. (String, zero byte terminated).
 *					Note: This function allocates new memory for the 
 *						  result. This memory must be released by 
 *						  the caller!
 *
 * O	upLen		Length in bytes of the target type.
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visGetTargetVersion(VIS_DATA **ppVersion, VIS_UINT *upLen);


/* ---------------------------------------------------------------------------- */
/**
 * visSetLicEx
 *
 * Writes an extended license key to the attached FarosPLC Run Time System.
 *
 * I	pKey		Pointer to the license key. 
 *					2 bytes     | 2 bytes       | xx bytes
 *                  type of key | length of key | key 
 *
 * I	uLen		Length in bytes of the whole license key.
 *
 * return			OK if successful else error number.
 *
 */
VIS_UINT visSetLicEx(VIS_DATA *pKey, VIS_UINT uLen);


#ifdef __cplusplus
} 
#endif

#endif /* _VISSHARED_H_ */

/* ---------------------------------------------------------------------------- */
