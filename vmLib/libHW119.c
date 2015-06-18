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
 * Filename: libHW119.c
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define HW119_MAX_FIELD_LENGHT 256
#define HW119_MAX_LINE_LEN (HW119_MAX_FIELD_NB * HW119_MAX_FIELD_LENGHT)
#define HW119_SEPARATOR ";"

#undef HW119_DBG

#ifdef HW119_DBG
#define DBG_PRINT(format, args...)  \
	fprintf (stderr, "[%s:%s:%d] - ", __FILE__, __func__, __LINE__); \
fprintf (stderr, format , ## args); \
fflush(stderr);
#else
#define DBG_PRINT(format, args...)
#endif

/**
 * calculate the address
 */
#define BIT_OCCUPATION_BYTE 1
#define REG_OCCUPATION_BYTE 4
/**
 * Retentive Register
 */
#define RET_REG_NB          64
#define RET_REG_BASE_BYTE   0
#define RET_REG_SIZE_BYTE   RET_REG_NB * REG_OCCUPATION_BYTE
/**
 * Retentive Bit
 */
#define RET_BIT_NB          128
#define RET_BIT_BASE_BYTE   RET_REG_BASE_BYTE + RET_REG_SIZE_BYTE
#define RET_BIT_SIZE_BYTE   RET_BIT_NB * BIT_OCCUPATION_BYTE
/**
 * Mirror Register
 */
#define MIR_REG_NB          32
#define MIR_REG_BASE_BYTE   RET_BIT_BASE_BYTE + RET_BIT_SIZE_BYTE
#define MIR_REG_SIZE_BYTE   MIR_REG_NB * REG_OCCUPATION_BYTE
/**
 * Retentive Bit
 */
#define MIR_BIT_NB          128
#define MIR_BIT_BASE_BYTE   MIR_REG_BASE_BYTE + MIR_REG_SIZE_BYTE
#define MIR_BIT_SIZE_BYTE   MIR_BIT_NB * BIT_OCCUPATION_BYTE
/**
 * Non Retentive Bit
 */
#define NRE_BIT_NB          1024
#define NRE_BIT_BASE_BYTE   MIR_BIT_BASE_BYTE + MIR_BIT_SIZE_BYTE
#define NRE_BIT_SIZE_BYTE   NRE_BIT_NB * BIT_OCCUPATION_BYTE
/**
 * Non Retentive Register
 */
#define NRE_REG_NB          4096
#define NRE_REG_BASE_BYTE   NRE_BIT_BASE_BYTE + NRE_BIT_SIZE_BYTE
#define NRE_REG_SIZE_BYTE   NRE_REG_NB * REG_OCCUPATION_BYTE

/* WARNING this value is used also into the HMI, so if you change the value here you must change it also into common/common.h */
#define CROSS_TABLE "Crosstable.csv"

/* ----  Includes:	 ---------------------------------------------------------- */
#include <ctype.h>

#include "stdInc.h"

#include "libHW119.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

#include "uthash.h"

/* ----  Local Defines:   ----------------------------------------------------- */

/* ----  Global Variables:	 -------------------------------------------------- */
static FILE * hw119_fp = NULL;
#if defined(RTS_CFG_HW119_LIB)
static char hw119_record[HW119_MAX_LINE_LEN] = "";
#endif
static char * hw119_record_iterator = NULL;
static int hw119_field_number = 0;
static int hw119_record_number = 0;

struct var_map_hash {
	char varname[32];  /* we'll use this field as the key */
	int address;
	UT_hash_handle hh; /* makes this structure hashable */
};
struct var_map_hash *var_map = NULL;

#if defined(RTS_CFG_HW119_LIB)

/* ----  Local Functions:	--------------------------------------------------- */
int add_var(int address, char *varname);
char * mystrtok(char * string, char * token, const char * separator);

/* ----  Implementations:	--------------------------------------------------- */

#define PATH_CFG_FILE "/local/path.conf"
#define BASE_ETC_DIR "BaseEtcDir:"
#define MAX_LINE 1024

int getBasePath(char * valueread)
{
	FILE *fp;
	char _label [MAX_LINE] = "";
	char _value [MAX_LINE] = "";
	char line [MAX_LINE] = "";
	fp = fopen(PATH_CFG_FILE, "r");
	if (!fp)
	{
		/*
		   printf("canno open cfg fle '%s'\n", PATH_CFG_FILE);
		 */
		return -1;
	}
	while (fgets(line, MAX_LINE, fp) != NULL)
	{

		if (line[0] != '#')
		{
			_label[0] = '\0';
			_value[0] = '\0';
			sscanf(line, "%s %s", _label, _value);
			if (_label[0] != '\0')
			{
				if (strcmp(_label, BASE_ETC_DIR) == 0)
				{
					strcpy(valueread, _value);
					fclose(fp);
					return 0;
				}
			}
		}
	}
	fclose(fp);
	return 1;
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_open_cross_table 
 *
 */
void hw119_open_cross_table(STDLIBFUNCALL)
{
	char filename[VMM_MAX_IEC_STRLEN] = "";
	char fullfilename[VMM_MAX_IEC_STRLEN] = "";
	char basepath[VMM_MAX_IEC_STRLEN] = "";
	HW119_OPEN_CROSS_TABLE_PARAM OS_SPTR *pPara = (HW119_OPEN_CROSS_TABLE_PARAM OS_SPTR *)pIN;

	if ((pPara->filename->CurLen) >= VMM_MAX_IEC_STRLEN)
	{
		DBG_PRINT("Cannot open '%s'\n", filename);
		pPara->ret_value = ERR_ERROR;
		return;
	}

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->filename, filename);

	if (getBasePath(basepath) != 0)
	{
		DBG_PRINT("Cannot get base path '%s'\n", basepath);
		fprintf(stderr,"################# Cannot get base path '%s'\n", basepath);
		pPara->ret_value = ERR_ERROR;
		return;
	}

	sprintf(fullfilename, "%s/%s", basepath, filename);
	fprintf(stderr,"################# Base path '%s' fullfilename '%s'\n", basepath, fullfilename);

	hw119_fp = fopen(fullfilename, "r");
	if (hw119_fp == NULL)
	{
		DBG_PRINT("Cannot open '%s'\n", fullfilename);
		pPara->ret_value = ERR_ERROR;
	}

	if (var_map != NULL && strcmp(filename, CROSS_TABLE) == 0)
	{
		DBG_PRINT("CLEAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		HASH_CLEAR(hh,var_map);
		var_map = NULL;
	}

	pPara->ret_value = OK;
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_close_cross_table 
 *
 */
void hw119_close_cross_table(STDLIBFUNCALL)
{
	HW119_CLOSE_CROSS_TABLE_PARAM OS_SPTR *pPara = (HW119_CLOSE_CROSS_TABLE_PARAM OS_SPTR *)pIN;

	hw119_record[0] = '\0';
	hw119_record_iterator = NULL;
	hw119_field_number = 0;
	hw119_record_number = 0;

	if (hw119_fp != NULL)
	{
		fclose(hw119_fp);
		hw119_fp = NULL;
		pPara->ret_value = OK;
	}
	else
	{
		pPara->ret_value = ERR_ERROR;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_read_cross_table_record 
 *
 */
char * mystrtok(char * string, char * token, const char * separator)
{
    char * p;
    int i;
    
    if (string == NULL)
    {
        if (token != NULL)
        {
            token[0] = '\0';
        }
        return NULL;
    }
    
    strcpy(token, string);
    if (strstr(token, separator) != NULL)
    {
        *strstr(token, separator) = '\0';
    }
#if 1
    for (i = strlen(token) - 1; i > 0 && isspace(token[i]); i--);
    token[i + 1] = '\0';
    for (i = 0; (unsigned int) i < strlen(token) && isspace(token[i]); i++);
    if (i > 0)
    {
        strcpy(token, &(token[i]));
    }
#else
    sscanf(token, "%s", token);
#endif
    p = strstr(string, separator);
    if (p!= NULL)
    {
        p++;
    }
    return p;
}

#define TAG_LEN 16

void hw119_read_cross_table_record(STDLIBFUNCALL)
{
	int address;
	HW119_READ_CROSS_TABLE_RECORD_PARAM OS_SPTR *pPara = (HW119_READ_CROSS_TABLE_RECORD_PARAM OS_SPTR *)pIN;
	pPara->ret_value = OK;

	hw119_record[0] = '\0';
	hw119_field_number = 0;
	hw119_record_iterator = NULL;
	char token[HW119_MAX_LINE_LEN] = "";

	if(hw119_fp != NULL && fgets(hw119_record, HW119_MAX_LINE_LEN, hw119_fp) != NULL)
	{
		if (pPara->error == 0)
		{
#if 1
			
			/* field extract */
			/* add the new variable to the hash table */
			/* extract the var name */
			char * tmp = hw119_record;
			char varname[256] = "";
			
			 /* extract the enable/disable field */
			tmp = mystrtok(tmp, token, HW119_SEPARATOR);
			if (tmp == NULL && token[0] == '\0')
			{
			    unsigned int i;
			    for (i = 0; i < strlen(hw119_record) && isspace(hw119_record[i]); i++);
			    if (i == strlen(hw119_record))
			    {
				DBG_PRINT("Empty line skipped\n");
			    }
			    else
			    {
				DBG_PRINT("Malformed record 'enabled/disabled' '%s' at line %d\n", hw119_record, hw119_record_number);
				pPara->ret_value = ERR_ERROR;
			    }
			}
	        	tmp = mystrtok(tmp, token, HW119_SEPARATOR);
			if (tmp == NULL && token[0] == '\0')
			{
			    DBG_PRINT("Malformed record ''Promoted' '%s' at line %d\n", hw119_record, hw119_record_number);
			    pPara->ret_value = ERR_ERROR;
			}
			/* Tag */
			tmp = mystrtok(tmp, token, HW119_SEPARATOR);
			if (tmp == NULL && token[0] == '\0')
			{
			     DBG_PRINT("Malformed element 'Tag' '%s' at line %d\n", hw119_record, hw119_record_number);
			     pPara->ret_value = ERR_ERROR;
			}
			if (strlen(token) > TAG_LEN)
			{
			    
			    DBG_PRINT("Tag '%s' too long, maximum lenght is %d at line %d\n", hw119_record, TAG_LEN, hw119_record_number);
			     pPara->ret_value = ERR_ERROR;
			}
#endif
			sscanf(token, "%s", varname);
#if 0
					/* calculate the address
					 */
					/* Retentive Registers */
					if (hw119_record_number <= RET_REG_NB)
					{
						address = RET_REG_BASE_BYTE + hw119_record_number * REG_OCCUPATION_BYTE;
					}
					/* Retentive Bit */
					else if (hw119_record_number <= RET_REG_NB + RET_BIT_NB)
					{
						address = RET_BIT_BASE_BYTE + (hw119_record_number - RET_REG_NB) * BIT_OCCUPATION_BYTE;
					}
					/* Mirror Registers */
					else if (hw119_record_number <= RET_REG_NB + RET_BIT_NB + MIR_REG_NB)
					{
						address = MIR_REG_BASE_BYTE + (hw119_record_number - (RET_REG_NB + RET_BIT_NB)) * REG_OCCUPATION_BYTE;
					}
					/* Mirror Bit */
					else if (hw119_record_number <= RET_REG_NB + RET_BIT_NB + MIR_REG_NB + MIR_BIT_NB)
					{
						address = MIR_BIT_BASE_BYTE + (hw119_record_number - (RET_REG_NB + RET_BIT_NB + MIR_REG_NB)) * BIT_OCCUPATION_BYTE;
					}
					/* Non Retentive Bit */
					else if (hw119_record_number <= RET_REG_NB + RET_BIT_NB + MIR_REG_NB + MIR_BIT_NB + NRE_BIT_NB)
					{
						address = NRE_BIT_BASE_BYTE + (hw119_record_number - (RET_REG_NB + RET_BIT_NB + MIR_REG_NB + MIR_BIT_NB)) * BIT_OCCUPATION_BYTE;
					}
					/* Non Retentive Byte */
					else if (hw119_record_number <= RET_REG_NB + RET_BIT_NB + MIR_REG_NB + MIR_BIT_NB + NRE_BIT_NB + NRE_REG_NB)
					{
						address = NRE_REG_BASE_BYTE + (hw119_record_number - (RET_REG_NB + RET_BIT_NB + MIR_REG_NB + MIR_BIT_NB + NRE_BIT_NB)) * REG_OCCUPATION_BYTE;
					}
					else
					{
						pPara->ret_value = ERR_ERROR;
						DBG_PRINT("Unexpected record nb %d, expected maximum %d records\n", hw119_record_number, RET_REG_NB + RET_BIT_NB + MIR_REG_NB + MIR_BIT_NB + NRE_BIT_NB + NRE_REG_NB);
					}
#else
					address = hw119_record_number + 1;
#endif
					if (pPara->ret_value != ERR_ERROR)
					{
						/* insert into hash table */
						if (add_var(address, varname) == 0)
						{
							pPara->ret_value = OK;
						}
						else
						{
							/* the address is already used */
							pPara->ret_value = ERR_ERROR;
							DBG_PRINT("Duplicated address %d for variable %s\n", address, varname);
						}
					}
				
				hw119_record_number++;
		}
		else
		{
			hw119_record_number++;
		}
	} 
	else
	{
		/* no field extract */
		//DBG_PRINT("End of file\n");
		pPara->ret_value = ERR_ERROR;
	}
}

int add_var(int addr, char *name)
{
	struct var_map_hash *s;

	HASH_FIND_STR(var_map, name, s);  /* id already in the hash? */
	if (s==NULL)
	{
		DBG_PRINT("Adding var '%s' address %d\n", name, addr);
		s = (struct var_map_hash*)malloc(sizeof(struct var_map_hash));
		strcpy(s->varname, name);
		s->address = addr;
		HASH_ADD_STR( var_map, varname, s );  /* id: name of key field */
#if 0
		s = NULL;
		HASH_FIND_STR( var_map, name, s );

		if (s != NULL)
		{
			DBG_PRINT("address for '%s' is 0x%X - %d\n", name, s->address, s->address);
		}
		else
		{
			DBG_PRINT("Cannot found address for '%s'\n", name);
		}
#endif
		return 0;
	}
	return 1;
}



/* ---------------------------------------------------------------------------- */
/**
 * hw119_get_cross_table_field 
 *
 */
void hw119_get_cross_table_field(STDLIBFUNCALL)
{
	char token[HW119_MAX_FIELD_LENGHT] = "";

	HW119_GET_CROSS_TABLE_FIELD OS_SPTR *pPara = (HW119_GET_CROSS_TABLE_FIELD OS_SPTR *)pIN;

	if (hw119_field_number == 0 && hw119_record_iterator == NULL)
	{
		hw119_record_iterator = mystrtok(hw119_record, token, HW119_SEPARATOR);
	}
	else if (hw119_record_iterator != NULL)
	{
		hw119_record_iterator = mystrtok(hw119_record_iterator, token, HW119_SEPARATOR);
	}
	else
	{
		DBG_PRINT("ERROR token '%s' full token '%s' record '%s'\n", token, hw119_record_iterator, hw119_record);
		utilAnsiToIec(token, (IEC_STRING OS_LPTR *)(pPara->field));
		pPara->ret_value = ERR_ERROR;
		return;
	}

	//DBG_PRINT("token '%s' full token '%s' record '%s'\n", token, hw119_record_iterator, hw119_record);
	pPara->ret_value = utilAnsiToIec(token, (IEC_STRING OS_LPTR *)(pPara->field));
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_get_addr 
 *
 */
void hw119_get_addr(STDLIBFUNCALL)
{
	char variablename[VMM_MAX_IEC_STRLEN];
	HW119_GET_ADDR OS_SPTR *pPara = (HW119_GET_ADDR OS_SPTR *)pIN;

	if ((pPara->varname->CurLen) >= VMM_MAX_IEC_STRLEN)
	{
		pPara->ret_value = 0xFFFF;
		return;
	}

	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->varname, variablename);

	/* search and return the var address */
	struct var_map_hash *s;

	DBG_PRINT("Looking for '%s'\n", variablename);
	HASH_FIND_STR( var_map, variablename, s );

	if (s != NULL)
	{
		DBG_PRINT("address for '%s' is 0x%X\n", variablename, s->address);
		pPara->ret_value = s->address;
	}
	else
	{
		DBG_PRINT("Cannot found address for '%s'\n", variablename);
		pPara->ret_value = 0xFFFF;
	}

	return;
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_write_var_bit 
 *
 */
void hw119_write_var_bit(STDLIBFUNCALL)
{
	char variablename[VMM_MAX_IEC_STRLEN];
	HW119_WRITE_VAR_BIT OS_SPTR *pPara = (HW119_WRITE_VAR_BIT OS_SPTR *)pIN;
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->varname, variablename);

	/* search and return the var address */
	struct var_map_hash *s;

	HASH_FIND_STR( var_map, variablename, s );

	if (s != NULL)
	{
		/* TODO: write a bit */
		pPara->ret_value = OK;
	}
	else
	{
		pPara->ret_value = ERR_ERROR;
	}
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_write_var_byte 
 *
 */
void hw119_write_var_byte(STDLIBFUNCALL)
{
	char variablename[VMM_MAX_IEC_STRLEN];
	HW119_WRITE_VAR_BYTE OS_SPTR *pPara = (HW119_WRITE_VAR_BYTE OS_SPTR *)pIN;
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->varname, variablename);
	/* TODO: write a bit */

	pPara->ret_value = OK;
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_write_var_uint 
 *
 */
void hw119_write_var_uint(STDLIBFUNCALL)
{
	char variablename[VMM_MAX_IEC_STRLEN];
	HW119_WRITE_VAR_UINT OS_SPTR *pPara = (HW119_WRITE_VAR_UINT OS_SPTR *)pIN;
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->varname, variablename);
	/* TODO: write a bit */

	pPara->ret_value = OK;
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_write_var_int 
 *
 */
void hw119_write_var_int(STDLIBFUNCALL)
{
	char variablename[VMM_MAX_IEC_STRLEN];
	HW119_WRITE_VAR_INT OS_SPTR *pPara = (HW119_WRITE_VAR_INT OS_SPTR *)pIN;
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->varname, variablename);
	/* TODO: write a bit */

	pPara->ret_value = OK;
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_write_var_udint 
 *
 */
void hw119_write_var_udint(STDLIBFUNCALL)
{
	char variablename[VMM_MAX_IEC_STRLEN];
	HW119_WRITE_VAR_UDINT OS_SPTR *pPara = (HW119_WRITE_VAR_UDINT OS_SPTR *)pIN;
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->varname, variablename);
	/* TODO: write a bit */

	pPara->ret_value = OK;
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_write_var_dint 
 *
 */
void hw119_write_var_dint(STDLIBFUNCALL)
{
	char variablename[VMM_MAX_IEC_STRLEN];
	HW119_WRITE_VAR_DINT OS_SPTR *pPara = (HW119_WRITE_VAR_DINT OS_SPTR *)pIN;
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->varname, variablename);
	/* TODO: write a bit */

	pPara->ret_value = OK;
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_write_var_real 
 *
 */
void hw119_write_var_real(STDLIBFUNCALL)
{
	char variablename[VMM_MAX_IEC_STRLEN];
	HW119_WRITE_VAR_REAL OS_SPTR *pPara = (HW119_WRITE_VAR_REAL OS_SPTR *)pIN;
	utilIecToAnsi((IEC_STRING OS_LPTR *)pPara->varname, variablename);
	/* TODO: write a bit */

	pPara->ret_value = OK;
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_double2float 
 *
 */
void hw119_dword2float(STDLIBFUNCALL)
{
	HW119_DWORD2FLOAT_PARAM OS_SPTR *pPara = (HW119_DWORD2FLOAT_PARAM OS_SPTR *)pIN;

	DBG_PRINT("Copying '%X'\n", pPara->value);
	memcpy(&(pPara->ret_value), &(pPara->value), sizeof(pPara->ret_value));
	DBG_PRINT("Copyed '%f'\n", pPara->ret_value);
}

/* ---------------------------------------------------------------------------- */
/**
 * hw119_float2double
 *
 */
void hw119_float2dword(STDLIBFUNCALL)
{
	HW119_FLOAT2DWORD_PARAM OS_SPTR *pPara = (HW119_FLOAT2DWORD_PARAM OS_SPTR *)pIN;

	DBG_PRINT("Copying '%f'\n", pPara->value);
	memcpy(&(pPara->ret_value), &(pPara->value), sizeof(pPara->value));
	DBG_PRINT("Copyed '%X'\n", pPara->ret_value);
}

#endif

