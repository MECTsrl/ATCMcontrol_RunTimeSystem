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

#ifdef __XENO__
#include <sys/io.h> // ioperm()
#include <syslog.h>
#endif

/* ----  Local Defines:   ----------------------------------------------------- */

#define VM_MAGIC            0xA5A5u

// PINCTRL register in "i.MX28 Applications Processor Reference Manual", # MCIMX28RM, rev. 1, 2010, page 125
#define XX_GPIO_ADDR        0x80018000
#define XX_GPIO_SIZE        0x00002000
#define XX_GPIO_SET_OFFSET  0x0004
#define XX_GPIO_CLR_OFFSET  0x0008
#define XX_GPIO_DIN_OFFSET	0x0000

/* ----  Global Variables:	 -------------------------------------------------- */

#if defined(_SOF_4CFC_SRC_)
extern unsigned long *g_jiffies_ptr;
#endif

/* ----  Local Variables:	 -------------------------------------------------- */

#if defined (RTS_CFG_MEM_TRACE)
static IEC_DINT g_lMemAlloc  = 0;
static IEC_DINT g_lMemObject = 0;
#endif

#if defined(RTS_CFG_DEBUG_GPIO)

#define XX_MAX_THR (1 + MAX_IO_LAYER + MAX_TASKS) // 1 timer + 5 iolayer + 25 task

/*
 * GPIO INDEX LAYOUT
 *
 * OUTPUTS:  0               ->  (XX_MAX_OUT_BIT - 1)
 * INPUTS:   XX_MAX_OUT_BIT  ->  (XX_MAX_OUT_BIT + XX_MAX_IN_BIT - 1)
 */
#define XX_MAX_OUT_BIT 5
#define XX_MAX_IN_BIT 1

static void *xx_base_ptr = NULL;
static int xx_fd = -1;
static struct {
	unsigned offset;
	unsigned value;
} xx_gpio_enabler[] = {
	// PINCTRL register in "i.MX28 Applications Processor Reference Manual", # MCIMX28RM, rev. 1, 2010, page 688

	// [0] OUT: bank 2, pin 2 (pin 2, SSP0_DATA2)
	// 5 4 G 3 V 2 1 X
	{ 0x0144, 0x00000030 },		//MUXSELx SET(GPIO), page 699
	{ 0x0384, 0x00000400 },		//DRIVEx SET(3.3V), page 753, 756
	{ 0x03b8, 0x00000300 },		//DRIVEx CLR(4mA), page 753, 756
	{ 0x0628, 0x00000004 },		//PULLx CLR(no), page 789
	{ 0x0728, 0x00000004 },		//DOUTx CLR, page 801
	{ 0x0b24, 0x00000004 },		//DOEx SET(en.), page 810

	// [1] OUT: bank 2, pin 3 (pin 274, SSP0_DATA3)
	// 5 4 G 3 V 2 X 0
	{ 0x0144, 0x000000c0 },		//MUXSELx SET(GPIO), page 699
	{ 0x0384, 0x00004000 },		//DRIVEx SET(3.3V), page 753, 756
	{ 0x03a8, 0x00003000 },		//DRIVEx CLR(4mA), page 753, 756
	{ 0x0628, 0x00000008 },		//PULLx CLR(no), page 789
	{ 0x0728, 0x00000008 },		//DOUTx CLR, page 801
	{ 0x0b24, 0x00000008 },		//DOEx SET(en.), page 810

	// [2] OUT: bank 2, pin 8 (pin 276, SSP0_CMD)
	// 5 4 G 3 V X 1 0
	{ 0x0144, 0x00030000 },		//MUXSELx SET(GPIO), page 699
	{ 0x0394, 0x00000004 },		//DRIVEx SET(3.3V), page 753, 756
	{ 0x03a8, 0x00000003 },		//DRIVEx CLR(4mA), page 753, 756
	{ 0x0628, 0x00000100 },		//PULLx CLR(no), page 789
	{ 0x0728, 0x00000100 },		//DOUTx CLR, page 801
	{ 0x0b24, 0x00000100 },		//DOEx SET(en.), page 810

	// [3] OUT: bank 2, pin 10 (pin 268, SSP0_SCK)
	// 5 4 G X V 2 1 0
	{ 0x0144, 0x00300000 },		//MUXSELx SET(GPIO), page 699
	{ 0x0394, 0x00000400 },		//DRIVEx SET(3.3V), page 753, 756
	{ 0x03a8, 0x00000300 },		//DRIVEx CLR(4mA), page 753, 756
	{ 0x0628, 0x00000400 },		//PULLx CLR(no), page 789
	{ 0x0728, 0x00000400 },		//DOUTx CLR, page 801
	{ 0x0b24, 0x00000400 },		//DOEx SET(en.), page 810

	// [4] OUT: bank 2, pin 0 (pin 270, SSP0_DATA0)
	// 5 X G 3 V 2 1 0
	{ 0x0144, 0x00000003 },		//MUXSELx SET(GPIO), page 699
	{ 0x0384, 0x00000004 },		//DRIVEx SET(3.3V), page 753, 756
	{ 0x03b8, 0x00000003 },		//DRIVEx CLR(4mA), page 753, 756
	{ 0x0628, 0x00000001 },		//PULLx CLR(no), page 789
	{ 0x0728, 0x00000001 },		//DOUTx CLR, page 801
	{ 0x0b24, 0x00000001 },		//DOEx SET(en.), page 810

	// [5] IN : bank 2, pin 1 (pin 289, SSP0_DATA1)
	// X 4 G 3 V 2 1 0
	{ 0x0144, 0x0000000c },		//MUXSELx SET(GPIO), page 699
	{ 0x0384, 0x00000040 },		//DRIVEx SET(3.3V), page 753, 756
	{ 0x0398, 0x00000030 },		//DRIVEx CLR(4mA), page 753, 756
	{ 0x0628, 0x00000002 },		//PULLx CLR(no), page 789
	{ 0x0728, 0x00000002 },		//DOUTx CLR, page 801
	{ 0x0b28, 0x00000002 },		//DOEx CLR(en.), page 810

	// THE END
	{ 0xffff, 0xffffffff }
};
static struct {
	unsigned offset;
	unsigned mask;
} xx_gpio_pin[] = {
	// PINCTRL register in "i.MX28 Applications Processor Reference Manual", # MCIMX28RM, rev. 1, 2010, page 688

	// [0] OUT: bank 2, pin  2 (pin   2, SSP0_DATA2)
	// 5 4 G 3 V 2 1 X
	{ 0x0720, 0x00000004 },		//DOUTx, page 801

	// [1] OUT: bank 2, pin  3 (pin 274, SSP0_DATA3)
	// 5 4 G 3 V 2 X 0
	{ 0x0720, 0x00000008 },		//DOUTx, page 801

	// [2] OUT: bank 2, pin  8 (pin 276, SSP0_CMD)
	// 5 4 G 3 V X 1 0
	{ 0x0720, 0x00000100 },		//DOUTx, page 801

	// [3] OUT: bank 2, pin 10 (pin 268, SSP0_SCK)
	// 5 4 G X V 2 1 0
	{ 0x0720, 0x00000400 },		//DOUTx, page 801

	// [4] OUT: bank 2, pin  0 (pin 270, SSP0_DATA0)
	// 5 X G 3 V 2 1 0
	{ 0x0720, 0x00000001 },		//DOUTx, page 801

	// [5] IN:  bank 2, pin  1 (pin 289, SSP0_DATA1)
	// X 4 G 3 V 2 1 0
	{ 0x0920, 0x00000002 },		//DINx, page 806

	// THE END
	{ 0xffff, 0xffffffff }
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
#ifdef __XENO__
	vsyslog(LOG_INFO, szFormat, va);
#else
	vprintf(szFormat, va); 
#endif
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

static unsigned threads_num = 0;

int osPthreadCreate(pthread_t *thread, /*const*/ pthread_attr_t *attr,
                        void *(*start_routine)(void *), void *arg,
                        const char *name, size_t stacksize)
{
	int retval;

#ifdef __XENO__
	pthread_attr_t attr_x;
	pthread_attr_t *attr_p;

	printf("osPthreadCreate: %02u %s", ++threads_num, name);

	attr_p = attr;
	if (attr_p == NULL) {
		attr_p = &attr_x;

		pthread_attr_init(attr_p);
	}

	if (stacksize < PTHREAD_STACK_MIN) {
		stacksize = PTHREAD_STACK_MIN;
	}

    pthread_attr_setdetachstate(attr_p, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(attr_p, stacksize);
	pthread_attr_setinheritsched(attr_p, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setscope(attr_p, PTHREAD_SCOPE_PROCESS);

	printf("\n");
	fflush(stdout);

	retval = pthread_create(thread, attr, start_routine, arg);
    if (retval) {
        // error
    } else {
        pthread_set_name_np(*thread, name);
    }
#else
	retval = pthread_create(thread, attr, start_routine, arg);
#endif

	return retval;
}

int osPthreadSetSched(int policy, int sched_priority)
{
	struct sched_param sp;

	if (policy != SCHED_OTHER) {
		sp.sched_priority = sched_priority;
	}

	int iRes = pthread_setschedparam(pthread_self(), policy, &sp);
	if (iRes != 0) {
		TR_ERR("pthread_setschedparam() failed", iRes);
	}

	return iRes;
}

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

#ifdef __XENO__
    struct timespec timer_next;
    struct timespec timer_now;
	ldiv_t x;
	int retval; 

	// get time
	clock_gettime(CLOCK_REALTIME, &timer_now);
	// add delay
	x = ldiv(ulTime, 1000L);
    timer_next.tv_sec = timer_now.tv_sec + x.quot;
    timer_next.tv_nsec = timer_now.tv_nsec + (x.rem * 1E6);
	// check tv_nsec overflow
    if (timer_next.tv_nsec >= 1E9) {
        x = ldiv(timer_next.tv_nsec, 1E9);
        timer_next.tv_sec += x.quot;
        timer_next.tv_nsec = x.rem;
    }
	// normalize to timer milliseconds base
	x = ldiv(timer_next.tv_nsec, 1E6);
	timer_next.tv_nsec = x.quot * 1E6;
	// do wait
	do {
        retval = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &timer_next, NULL);
	} while (retval == EINTR);
#else
	usleep(ulTime * 1000);
#endif
	
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osSleepAbsolute			
 *
 * Suspend the task until the given absolute time (in ms).
 *
 * @param			ulTime		Suspend time in ms.
 * @return			OK if successful else error number.
 */
IEC_UINT osSleepAbsolute(IEC_UDINT ulTime)
{
	IEC_UINT uRes = OK;

#ifdef __XENO__
        struct timespec timer_next;
	ldiv_t x;
	int retval; 

	// compute time
    x = ldiv(ulTime, 1000L);
    timer_next.tv_sec = x.quot;
    timer_next.tv_nsec = x.rem * 1E6;
	// do wait
	do {
       	retval = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &timer_next, NULL);
	} while (retval == EINTR);
#else
	IEC_UDINT now = osGetTime32();

	if (ulTime > now) {
		usleep((ulTime - now) * 1000);
	}
#endif
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
#if 0
	return osGetTime32Ex();
#else
	return osGetTimeUSEx();
#endif
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
#ifdef __XENO__
	struct timespec tv;

	clock_gettime(CLOCK_REALTIME, &tv);

	return (IEC_UDINT)((IEC_ULINT)tv.tv_sec * (IEC_ULINT)1000u + (tv.tv_nsec) / 1E6);
#else
	static struct timeval tv;

	if (gettimeofday(&tv, NULL) == -1)
	{
		return 0;
	}
	return (IEC_UDINT)((IEC_ULINT)tv.tv_sec * (IEC_ULINT)1000u + (tv.tv_usec + 500u) / 1000u);
#endif
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetTime64
 *
 * @return			Current time as 64 bit value in milliseconds.
 */
IEC_ULINT osGetTime64Ex(void)
{
#ifdef __XENO__
        struct timespec tv;

        clock_gettime(CLOCK_REALTIME, &tv);

	return (IEC_ULINT)tv.tv_sec * (IEC_ULINT)1000u + (tv.tv_nsec) / 1E6;
#else
	static struct timeval tv;

	if (gettimeofday(&tv, NULL) == -1)
	{
		return 0;
	}
	return (IEC_ULINT)tv.tv_sec * (IEC_ULINT)1000u + (tv.tv_usec + 500u) / 1000u;
#endif
}

/* ---------------------------------------------------------------------------- */
/**
 * osGetTimeUS
 *
 * @return			Current time as 64 bit value in microseconds.
 */
IEC_ULINT osGetTimeUSEx(void)
{
#ifdef __XENO__
    struct timespec tv;

    clock_gettime(CLOCK_REALTIME, &tv);

	return (IEC_ULINT)tv.tv_sec * (IEC_ULINT)1000000u + (tv.tv_nsec/1E3);
#else
	static struct timeval tv;

	if (gettimeofday(&tv, NULL) == -1)
	{
		return 0;
	}
	return (IEC_ULINT)tv.tv_sec * (IEC_ULINT)1000000u + tv.tv_usec;
#endif
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

/*
 * ----------------------------------------------------------------------------
 * GPIO section
 * ----------------------------------------------------------------------------
 */

#if defined(RTS_CFG_DEBUG_GPIO)

void
xx_gpio_init(void)
{
	xx_fd = open("/dev/mem", O_RDWR, 0);
	if (xx_fd <= 0)
		return;

	xx_base_ptr = mmap(NULL, XX_GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, xx_fd, XX_GPIO_ADDR);
	if ((xx_fd <= 0) && (xx_base_ptr == NULL))
		return;

	unsigned i = 0;
	unsigned *reg_ptr = NULL;

	// Set up all input and output pins.
	for (i = 0; xx_gpio_enabler[i].offset < 0xffff; i++) {
		reg_ptr = (unsigned *)(xx_base_ptr + xx_gpio_enabler[i].offset);
		*reg_ptr = xx_gpio_enabler[i].value;
	}

	// Toggle all output pins.

	for (i = 0; i < XX_MAX_OUT_BIT; ++i)
		xx_gpio_clr(i);

	for (i = 0; i < XX_MAX_OUT_BIT; ++i)
		xx_gpio_set(i);

	for (i = 0; i < XX_MAX_OUT_BIT; ++i)
		xx_gpio_clr(i);
}

void
xx_gpio_set(unsigned n)
{
	if (xx_base_ptr && n < XX_MAX_OUT_BIT) {
		register unsigned *reg_ptr;

		reg_ptr = (unsigned *)(xx_base_ptr + xx_gpio_pin[n].offset + XX_GPIO_SET_OFFSET);
		*reg_ptr = xx_gpio_pin[n].mask;
	}
}

void
xx_gpio_clr(unsigned n)
{
	if (xx_base_ptr && n < XX_MAX_OUT_BIT) {
		register unsigned *reg_ptr;

		reg_ptr = (unsigned *)(xx_base_ptr + xx_gpio_pin[n].offset + XX_GPIO_CLR_OFFSET);
		*reg_ptr = xx_gpio_pin[n].mask;
	}
}

/**
 * Read the level of the given GPIO pin ID.
 *
 * @param n				pin ID to read.
 *
 * @return				pin level.
 */
unsigned char
xx_gpio_get(unsigned n)
{
	if ((xx_base_ptr == NULL) || (n < XX_MAX_OUT_BIT) || (n >= (XX_MAX_OUT_BIT + XX_MAX_IN_BIT)))
		return 0;

	return (*((unsigned *)(xx_base_ptr + xx_gpio_pin[n].offset + XX_GPIO_DIN_OFFSET)) & xx_gpio_pin[n].mask) ? 1 : 0;
}

void
xx_gpio_close(void)
{
	if (xx_base_ptr) {
		munmap(xx_base_ptr, XX_GPIO_SIZE);
		xx_base_ptr = NULL;
	}

	if (xx_fd > 0) {
		close(xx_fd);
		xx_fd = -1;
	}
}

#endif

void *osMemCpy(void *dest, const void *src, size_t n) // __attribute__ ((optimize("O0")))
{
	return memcpy(dest, src, n);
}
/* ---------------------------------------------------------------------------- */
