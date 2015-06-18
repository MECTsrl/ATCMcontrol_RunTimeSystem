
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
 * Filename: osUtil.c
 */


/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__	"osUtil.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "stdInc.h"

/* ----  Target Specific Includes:	 ------------------------------------------ */

#include <sys/time.h>
#include <stdarg.h>

#if defined(RTS_CFG_DEBUG_GPIO)
#include <fcntl.h>
#include <sys/mman.h>
#endif

/* ----  Local Defines:   ----------------------------------------------------- */

#define VM_MAGIC	0xA5A5u

/* ----  Global Variables:	 -------------------------------------------------- */

#if defined (RTS_CFG_MEM_TRACE)
  static IEC_DINT g_lMemAlloc  = 0;
  static IEC_DINT g_lMemObject = 0;
#endif

#if defined(_SOF_4CFC_SRC_)
  extern unsigned long *g_jiffies_ptr;
#endif

#if defined(RTS_CFG_DEBUG_GPIO)

#define XX_MAX_THR (1 + MAX_IO_LAYER + MAX_TASKS) // 1 timer + 5 iolayer + 25 task
#define XX_MAX_BIT 6

static unsigned long xx_addr = 0x80018000; // PINCTRL
static unsigned long xx_size = 0x00002000; 
static void * xx_base_ptr = NULL;
static int xx_fd = -1;
static pthread_mutex_t xx_mutex;
static struct { pthread_t id; unsigned bits; } xx_gpio_bits[XX_MAX_THR];
static unsigned xx_set = 0x0004;
static unsigned xx_clr = 0x0008;
static struct { unsigned offset, mask; } xx_gpio_pin[] = {
	{ 0x0720, 0x01000000 }, // TASTO 1 SSP3SCK   BANK2 PIN24
	{ 0x0720, 0x00004000 }, // TASTO 2           BANK2 PIN14
	{ 0x0720, 0x08000000 }, // TASTO 3 SSP3_SS0  BANK2 PIN27
	{ 0x0720, 0x00020000 }, // TASTO 4 SSP2_MOSI BANK2 PIN17
	{ 0x0720, 0x00040000 }, // TASTO 5 SSP2_MISO BANK2 PIN18
	{ 0x0720, 0x00010000 }, // TASTO 6 SSP_SCK   BANK2 PIN16
	{ 0xffff, 0xffffffff }
};
static struct { unsigned offset, value; } xx_gpio_enabler[] = {
// Pin Control / GPIO Interface / Output Operation Setup (HW_PINCTRL_... )
// BANK2, MUXSELx SET(GPIO) , DRIVEx SET(3.3V)  , DRIVEx CLR(4mA)   , PULLx  CLR(no)    , DOUTx  CLR        , DOEx   SET(en.) 
/*PIN24*/{0x0154,0x00030000},{0x03b4,0x00000004},{0x03b8,0x00000003},{0x0628,0x01000000},{0x0728,0x01000000},{0x0b24,0x01000000},
/*PIN14*/{0x0144,0x30000000},{0x0394,0x04000000},{0x0398,0x03000000},{0x0628,0x00004000},{0x0728,0x00004000},{0x0b24,0x00004000},
/*PIN27*/{0x0154,0x00c00000},{0x03b4,0x00004000},{0x03b8,0x00003000},{0x0628,0x08000000},{0x0728,0x08000000},{0x0b24,0x08000000},
/*PIN17*/{0x0154,0x0000000c},{0x03a4,0x00000040},{0x03a8,0x00000030},{0x0628,0x00020000},{0x0728,0x00020000},{0x0b24,0x00020000},
/*PIN18*/{0x0154,0x00000030},{0x03a4,0x00000400},{0x03a8,0x00000300},{0x0628,0x00040000},{0x0728,0x00040000},{0x0b24,0x00040000},
/*PIN16*/{0x0154,0x00000003},{0x03a4,0x00000004},{0x03a8,0x00000003},{0x0628,0x00010000},{0x0728,0x00010000},{0x0b24,0x00010000},
	 {0xffff,0xffffffff}
};

#endif

/* ----  Local Functions:	--------------------------------------------------- */

/* ----  Implementations:	--------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/**
 * osTrace
 *
 * Prints the given debug message to an appropriate device.
 *
 * @return			OK if successful else error number.
 */
#if defined(RTS_CFG_DEBUG_OUTPUT)

IEC_UINT osTrace(IEC_CHAR *szFormat, ...)
{
	va_list 	va;
	va_start(va, szFormat);
	vprintf(szFormat, va); 
	va_end(va);

  #if defined(RTS_CFG_DEBUG_FILE)
	{
		IEC_UDINT	hF;
		IEC_UINT	uRes;
		IEC_CHAR	szFile[VMM_MAX_PATH + 1];

		static	IEC_UINT uLocked = 0;

		if (uLocked != 0)
		{
			return OK;
		}

		uLocked++;

		uRes = utilOpenFile(&hF, (IEC_CHAR *)szFile, VMM_MAX_PATH, osGetTraceDir, VMM_DIR_TRACE, VMM_FILE_TRACE, FIO_MODE_APPEND);
		if (uRes == OK)
		{
			va_start(va, szFormat);
			vfprintf((FILE *)hF, szFormat, va); 
			va_end(va);
  
			xxxClose(hF);
		}

		uLocked--;
	}
  #endif

	return OK;
}
#endif /* RTS_CFG_DEBUG_OUTPUT */


/* ---------------------------------------------------------------------------- */
/**
 * osSleep			
 *
 * Suspend the task for the given time (in ms).
 *
 * @param			ulTime		Suspend time in ms.
 * @return			OK if successful else error number.
 */
IEC_UINT osSleep(IEC_UDINT ulTime)
{
	IEC_UINT uRes = OK;

	usleep(ulTime * 1000);
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetTime32
 *
 * @return			Current time as 32 bit value in milliseconds.
 */
IEC_UDINT osGetTime32(void)
{
  #if defined(_SOF_4CFC_SRC_)
	union 
	{
		struct 
		{
			IEC_UDINT msecs_hi;
			IEC_UDINT msecs_lo;
		
		} msecs_hl;
		
		IEC_ULINT msecs;
	
	} u_msecs;
	
	/* Get and calculate the current time in ms 
	 */
	u_msecs.msecs = (IEC_ULINT)(*g_jiffies_ptr) * (IEC_ULINT)10ull;
	
	/* Return the lower long 
	 */
	return u_msecs.msecs_hl.msecs_lo;

#else

	return osGetTime32Ex();

#endif
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetTime64
 *
 * @return			Current time as 64 bit value in milliseconds.
 */
IEC_ULINT osGetTime64(void)
{
  #if defined(_SOF_4CFC_SRC_)
	
	return (IEC_ULINT)*g_jiffies_ptr * (IEC_ULINT)10ull;
	
#else

	return osGetTime32Ex();

#endif
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetTimeUS
 *
 * @return			Current time as 64 bit value in microseconds.
 */
IEC_ULINT osGetTimeUS(void)
{
  #if defined(_SOF_4CFC_SRC_)
	
	return (IEC_ULINT)*g_jiffies_ptr * (IEC_ULINT)10000ull;
	
#else

	return osGetTime32Ex();

#endif
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetTime32Ex
 *
 * @return			Current time as 32 bit value in milliseconds.
 */
IEC_UDINT osGetTime32Ex(void)
{
	static struct timeval tv;

	if (gettimeofday(&tv, NULL) == -1)
	{
		return 0;
	}

	return (IEC_UDINT)((IEC_ULINT)tv.tv_sec * (IEC_ULINT)1000u + (tv.tv_usec + 500u) / 1000u);
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetTime64
 *
 * @return			Current time as 64 bit value in milliseconds.
 */
IEC_ULINT osGetTime64Ex(void)
{
	static struct timeval tv;

	if (gettimeofday(&tv, NULL) == -1)
	{
		return 0;
	}

	return (IEC_ULINT)tv.tv_sec * (IEC_ULINT)1000u + (tv.tv_usec + 500u) / 1000u;
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetTimeUS
 *
 * @return			Current time as 64 bit value in microseconds.
 */
IEC_ULINT osGetTimeUSEx(void)
{
	static struct timeval tv;

	if (gettimeofday(&tv, NULL) == -1)
	{
		return 0;
	}
	
	return (IEC_ULINT)tv.tv_sec * (IEC_ULINT)1000000u + tv.tv_usec;
}

/* ---------------------------------------------------------------------------- */
/**
 * osMalloc
 *
 * Allocates an memory object within the given Segment.
 *
 * @return			OK if successful else error number.
 */
IEC_DATA OS_LPTR *osMalloc(IEC_UDINT ulSize)
{
	IEC_DATA OS_LPTR *pRet	= NULL;

  #if defined(RTS_CFG_MEMORY_CHECK)

	IEC_UINT uMagic = VM_MAGIC;

	#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64)
	  IEC_UINT uAli = 2;
	#else
	  IEC_UINT uAli = 0;
	#endif

	IEC_DATA OS_LPTR *pData = OS_MALLOC(sizeof(uMagic) + uAli + sizeof(ulSize) + ulSize + sizeof(uMagic));

	if (pData == NULL)
	{
	  #if defined(RTS_CFG_DEBUG_OUTPUT)
		osTrace("\r\n*** [M E M O R Y   E R R O R]: Memory allocation (%ld bytes) failed!\r\n", ulSize);
	  #endif
		return NULL;
	}

	pRet = pData + sizeof(uMagic) + uAli + sizeof(ulSize);
	pRet = OS_NORMALIZEPTR(pRet);

	*(IEC_UINT UNALIGNED *)pData = (IEC_UINT)(uMagic ^ (IEC_UINT)ulSize);
	pData += sizeof(uMagic) + uAli;

	*(IEC_UDINT *)pData = ulSize;
	pData += sizeof(ulSize) + ulSize;

	// *(IEC_UINT UNALIGNED *)pData = (IEC_UINT)(uMagic ^ ~(IEC_UINT)ulSize);
	IEC_UINT u = (IEC_UINT)(uMagic ^ ~(IEC_UINT)ulSize);
	OS_MEMCPY(pData, &u, sizeof(IEC_UINT));

  #if defined (RTS_CFG_MEM_TRACE)
	g_lMemObject++;
	g_lMemAlloc += sizeof(uMagic) + uAli + sizeof(ulSize) + ulSize + sizeof(uMagic);
  #if defined(RTS_CFG_DEBUG_OUTPUT)
	osTrace("--- Alloc    %7ld   Bytes at 0x%08lx (%02ld/%05ld)\r\n", ulSize, pRet, g_lMemObject, g_lMemAlloc / 1024);
  #endif
  #endif

	return pRet;

  #else

	pRet = (IEC_DATA OS_LPTR *)OS_MALLOC(ulSize);
	pRet = OS_NORMALIZEPTR(pRet);

	return pRet;

  #endif
}

/* ---------------------------------------------------------------------------- */
/**
 * osFree
 *
 * Frees a memory object.
 *
 * @return			OK if successful else error number.
 */
IEC_UINT osFree(IEC_DATA OS_LPTR **ppData)
{
	IEC_UINT uRes = OK;

  #if defined(RTS_CFG_MEMORY_CHECK)

	#if defined(IP_CFG_INST32) || defined(IP_CFG_INST64)
	  IEC_UINT uAli = 2;
	#else
	  IEC_UINT uAli = 0;
	#endif

	IEC_UINT  uMagic = 0;
	IEC_UDINT ulSize = 0;
	IEC_DATA OS_LPTR *pData = NULL;
	IEC_DATA OS_LPTR *pEnd	= NULL;

	if (ppData == NULL || *ppData == NULL)
	{
		RETURN(OK);
	}

	pData	= *ppData;

	pData  -= sizeof(ulSize);
	ulSize	= *(IEC_UDINT *)pData;

	pData  -= uAli + sizeof(uMagic);
	// uMagic	= *(IEC_UINT UNALIGNED *)pData;
	OS_MEMCPY(&uMagic, pData, sizeof(IEC_UINT));

	if (uMagic != (IEC_UINT)(VM_MAGIC ^ (IEC_UINT)ulSize))
	{
	  #if defined(RTS_CFG_DEBUG_OUTPUT)
		osTrace("\r\n*** [M E M O R Y   E R R O R]: Memory overwriting at 0x%08lx (begin) detected!\r\n", *ppData);
	  #endif
		*ppData = NULL;
		RETURN(ERR_ERROR);
	}

	pEnd = pData + sizeof(uMagic) + uAli + sizeof(ulSize) + ulSize;
	// uMagic = *(IEC_UINT UNALIGNED *)pEnd;
	OS_MEMCPY(&uMagic, pEnd, sizeof(IEC_UINT));

	if (uMagic != (IEC_UINT)(VM_MAGIC ^ ~(IEC_UINT)ulSize))
	{
	  #if defined(RTS_CFG_DEBUG_OUTPUT)
		osTrace("\r\n*** [M E M O R Y   E R R O R]: Memory overwriting at 0x%08lx (end) detected!\r\n", *ppData);
	  #endif
		*ppData = NULL;
		RETURN(ERR_ERROR);
	}

  #if defined (RTS_CFG_MEM_TRACE)
	g_lMemObject--;
	g_lMemAlloc -= sizeof(uMagic) + uAli + sizeof(ulSize) + ulSize + sizeof(uMagic);
  #if defined(RTS_CFG_DEBUG_OUTPUT)
	osTrace("--- Free     %7ld   Bytes at 0x%08lx (%02ld/%05ld)\r\n", ulSize, *ppData, g_lMemObject, g_lMemAlloc / 1024);
  #endif
  #endif

	OS_FREE(pData);
	*ppData = NULL;

	RETURN(uRes);

  #else

	OS_FREE(*ppData);
	*ppData = NULL;
	
	RETURN(uRes);

  #endif
}

/* ---------------------------------------------------------------------------- */

#if defined(RTS_CFG_DEBUG_GPIO)

void xx_gpio_init()
{
	xx_fd = open("/dev/mem", O_RDWR, 0);
	if (xx_fd > 0) {
		xx_base_ptr = mmap(NULL, xx_size, PROT_READ | PROT_WRITE, MAP_SHARED, xx_fd, xx_addr);
	}
	if (xx_fd > 0 && xx_base_ptr) {
		unsigned i = 0;
		unsigned * reg_ptr;
		pthread_mutexattr_t attr;

		while (xx_gpio_enabler[i].offset < 0xffff) {
			reg_ptr = (unsigned *)(xx_base_ptr + xx_gpio_enabler[i].offset);
			*reg_ptr = xx_gpio_enabler[i].value;
			++i;
		}
		for (i = 0; i < XX_MAX_THR; ++i) {
			xx_gpio_bits[i].id = 0;
			xx_gpio_bits[i].bits = 0x00000000;
		}
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
		pthread_mutex_init(&xx_mutex, &attr);
	}
}

void xx_gpio_enable_thread()
{
	if (xx_base_ptr) {
		unsigned i;
		int found = -1;
		int first_free = -1;
		pthread_t id = pthread_self();

		pthread_mutex_lock(&xx_mutex);
		first_free = -1;
		found = -1;
		for (i = 0; i < XX_MAX_THR; ++i) {
			if (xx_gpio_bits[i].id == id) {
				found = i;
				break;
			}
			if (xx_gpio_bits[i].id == 0 && first_free == -1) {
				first_free = i;
			}
		}
		if (found == -1 && first_free != -1) {
			xx_gpio_bits[first_free].id = id;
			xx_gpio_bits[first_free].bits = 0x00000000;
		}
		pthread_mutex_unlock(&xx_mutex);
	}
}

static void xx_gpio_update(unsigned n)
{
	register unsigned i, bits;
	register unsigned * reg_ptr;

	bits = 0x00000000;
	for (i = 0; i < XX_MAX_THR; ++i) {
		bits |= xx_gpio_bits[i].bits;
	}
	if (bits & (1 << n)) {
		reg_ptr = (unsigned *)(xx_base_ptr + xx_gpio_pin[n].offset + xx_set);
		*reg_ptr = xx_gpio_pin[n].mask;
	} else {
		reg_ptr = (unsigned *)(xx_base_ptr + xx_gpio_pin[n].offset + xx_clr);
		*reg_ptr = xx_gpio_pin[n].mask;
	}
}

void xx_gpio_set(unsigned n)
{
	if (xx_base_ptr && n < XX_MAX_BIT) {
		unsigned i;
		pthread_t id = pthread_self();

		for (i = 0; i < XX_MAX_THR; ++i) {
			if (xx_gpio_bits[i].id == id) {
				xx_gpio_bits[i].bits |= (1 << n);
				pthread_mutex_lock(&xx_mutex);
				xx_gpio_update(n);
				pthread_mutex_unlock(&xx_mutex);
				break;
			}
		}
	}
}

void xx_gpio_clr(unsigned n)
{
	if (xx_base_ptr && n < XX_MAX_BIT) {
		unsigned i;
		pthread_t id = pthread_self();

		for (i = 0; i < XX_MAX_THR; ++i) {
			if (xx_gpio_bits[i].id == id) {
				xx_gpio_bits[i].bits &= ~(1 << n);
				pthread_mutex_lock(&xx_mutex);
				xx_gpio_update(n);
				pthread_mutex_unlock(&xx_mutex);
				break;
			}
		}
	}
}

void xx_gpio_disable_thread()
{
	if (xx_base_ptr) {
		unsigned i;
		pthread_t id = pthread_self();

		pthread_mutex_lock(&xx_mutex);
		for (i = 0; i < XX_MAX_THR; ++i) {
			if (xx_gpio_bits[i].id == id) {
				unsigned n;

				xx_gpio_bits[i].id = 0;
				for (n = 0; n < XX_MAX_BIT; ++n) {
					if (xx_gpio_bits[i].bits & (1 << n)) {
						xx_gpio_update(n);
					}
				}
				xx_gpio_bits[i].bits = 0x00000000;
				break;
			}
		}
		pthread_mutex_unlock(&xx_mutex);
	}
}

void xx_gpio_close()
{
	if (xx_base_ptr) {
		pthread_mutex_destroy(&xx_mutex);
		munmap(xx_base_ptr, xx_size);
		xx_base_ptr = NULL;
	}
	if (xx_fd > 0) {
	    close(xx_fd);
		xx_fd = -1;
	}
}

#endif

/* ---------------------------------------------------------------------------- */
