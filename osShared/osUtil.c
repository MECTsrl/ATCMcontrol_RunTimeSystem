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

#define XX_GPIO_BASE        0x80000000
#define XX_GPIO_SIZE        0x00100000
#define XX_GPIO_SET_OFFSET  0x0004
#define XX_GPIO_CLR_OFFSET  0x0008

#define XX_PINCTL_BASE      0x00018000 // 0x80018000

#define XX_RTCCTL_BASE      0x00056000 // 0x80056000
#define XX_WATCHDOGEN_OFFS  0x00000000 // 0x80056000 HW_RTC_CTRL
#define XX_WATCHDOGEN_MASK  0x00000010 //            WATCHDOGEN
#define XX_WATCHDOGms_OFFS  0x00000050 // 0x80056050 HW_RTC_WATCHDOG

#define XX_PWMCTL_BASE      0x00064000 // 0x80064000
#define XX_PMW3_ENABLE_OFFS 0x00000000 // 0x80064000 HW_PWM_CTRL
#define XX_PMW3_ENABLE_MASK 0x00000008 //            PMW3_ENABLE
#define XX_PWM3_ACTIVE_OFFS 0x00000070 // 0x80064070 HW_PWM_ACTIVE3
#define XX_PWM3_PERIOD_OFFS 0x00000080 // 0x80064080 HW_PWM_ACTIVE3


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

/* Use FGPIO and SDCARD pins as GPIO */

static void *xx_base_ptr = NULL;
static int xx_fd = -1;
static struct {
	unsigned offset;
	unsigned value;
} xx_gpio_enabler[] = {

	// PINCTRL register in "i.MX28 Applications Processor Reference Manual", # MCIMX28RM, rev. 1, 2010, page 688

    // XX_GPIO( 0) FastIO_1 bank 2, pin 14 (pin 21, SSP1_DATA0)
	{ 0x0144, 0x30000000 },		//MUXSEL4 SET(GPIO), page 705
	{ 0x0394, 0x04000000 },		//DRIVE9 SET(3.3V), page 756
	{ 0x0398, 0x03000000 },		//DRIVE9 CLR(4mA), page 756
	{ 0x0628, 0x00004000 },		//PULL2 CLR(no), page 790
	{ 0x0728, 0x00004000 },		//DOUT2 CLR, page 801

    // XX_GPIO( 1) FastIO_2 bank 0, pin 17 (pin 131, GPMI_CE1N)
	{ 0x0114, 0x0000000c },		//MUXSEL1 SET(GPIO), page 696
	{ 0x0324, 0x00000040 },		//DRIVE2 SET(3.3V), page 734
	{ 0x0328, 0x00000030 },		//DRIVE2 CLR(4mA), page 734
	{ 0x0608, 0x00020000 },		//PULL0 CLR(no), page 785
	{ 0x0708, 0x00020000 },		//DOUT0 CLR, page 800

    // XX_GPIO( 2) FastIO_3 bank 2, pin 12 (pin 11, SSP1_SCK)
	{ 0x0144, 0x03000000 },		//MUXSEL4 SET(GPIO), page 705
	{ 0x0394, 0x00040000 },		//DRIVE9 SET(3.3V), page 756
	{ 0x0398, 0x00030000 },		//DRIVE9 CLR(4mA), page 756
	{ 0x0628, 0x00001000 },		//PULL2 CLR(no), page 790
	{ 0x0728, 0x00001000 },		//DOUT2 CLR, page 801

    // XX_GPIO( 3) FastIO_4 bank 3, pin 6 (pin 78, AUART1_CTS)
	{ 0x0164, 0x00003000 },		//MUXSEL6 SET(GPIO), page 710
	{ 0x03c4, 0x04000000 },		//DRIVE12 SET(3.3V), page 764
	{ 0x03c8, 0x03000000 },		//DRIVEx CLR(4mA), page 764
	{ 0x0638, 0x00000040 },		//PULL3 CLR(no), page 791
	{ 0x0738, 0x00000040 },		//DOUT3 CLR, page 802

    // XX_GPIO( 4) FastIO_5 bank 2, pin 20 (pin 7, SSP2_SS1)
	{ 0x0154, 0x00000300 },		//MUXSEL5 SET(GPIO), page 708
	{ 0x03a4, 0x00040000 },		//DRIVE10 SET(3.3V), page 760
	{ 0x03a8, 0x00030000 },		//DRIVE10 CLR(4mA), page 760
	{ 0x0628, 0x00100000 },		//PULL2 CLR(no), page 790
	{ 0x0728, 0x00100000 },		//DOUT2 CLR, page 801

    // XX_GPIO( 5) FastIO_6 bank 3, pin 2 (pin 70, AUART0_CTS)
    { 0x0164, 0x00000030 },		//MUXSEL6 SET(GPIO), page 711
	{ 0x03c4, 0x00000400 },		//DRIVE12 SET(3.3V), page 764
	{ 0x03c8, 0x00000300 },		//DRIVE12 CLR(4mA), page 764
	{ 0x0638, 0x00000004 },		//PULL3 CLR(no), page 791
	{ 0x0738, 0x00000004 },		//DOUT3 CLR, page 802

    // XX_GPIO( 6) FastIO_7 bank 3, pin 4 (pin 81, AUART1_RX)
	{ 0x0164, 0x00000300 },		//MUXSEL6 SET(GPIO), page 710
	{ 0x03c4, 0x00040000 },		//DRIVE12 SET(3.3V), page 764
	{ 0x03c8, 0x00030000 },		//DRIVE12 CLR(4mA), page 764
	{ 0x0638, 0x00000010 },		//PULL3 CLR(no), page 791
	{ 0x0738, 0x00000010 },		//DOUT3 CLR, page 802

    // XX_GPIO( 7) FastIO_8 bank 3, pin 5 (pin 65, AUART1_TX)
	{ 0x0164, 0x00000c00 },		//MUXSEL6 SET(GPIO), page 710
	{ 0x03c4, 0x00400000 },		//DRIVE12 SET(3.3V), page 764
	{ 0x03c8, 0x00300000 },		//DRIVE12 CLR(4mA), page 764
	{ 0x0638, 0x00000020 },		//PULL3 CLR(no), page 791
	{ 0x0738, 0x00000020 },		//DOUT3 CLR, page 802

    // XX_GPIO( 8) FastIO_9 bank 2, pin 24 (pin 286, SSP3_SCK)
    { 0x0154, 0x00030000 },		//MUXSEL5 SET(GPIO), page 708
    { 0x03b4, 0x00000004 },		//DRIVE11 SET(3.3V), page 762
    { 0x03b8, 0x00000003 },		//DRIVE11 CLR(4mA), page 762
    { 0x0628, 0x01000000 },		//PULL2 CLR(no), page 790
    { 0x0728, 0x01000000 },		//DOUT2 CLR, page 801

    // XX_GPIO( 9) FastIO_10 bank 2, pin 27 (pin 15, SSP3_SS0)
    { 0x0154, 0x00300000 },		//MUXSEL5 SET(GPIO), page 708
    { 0x03b4, 0x00004000 },		//DRIVE11 SET(3.3V), page 762
    { 0x03b8, 0x00003000 },		//DRIVE11 CLR(4mA), page 762
    { 0x0628, 0x08000000 },		//PULL2 CLR(no), page 790
    { 0x0728, 0x08000000 },		//DOUT2 CLR, page 801

    // XX_GPIO(10) FastIO_11 bank 2, pin 17 (pin 1, SSP2_MOSI)
    { 0x0154, 0x0000000c },		//MUXSEL5 SET(GPIO), page 708
    { 0x03a4, 0x00000040 },		//DRIVE10 SET(3.3V), page 759
    { 0x03a8, 0x00000030 },		//DRIVE10 CLR(4mA), page 759
    { 0x0628, 0x00020000 },		//PULL2 CLR(no), page 790
    { 0x0728, 0x00020000 },		//DOUT2 CLR, page 801

    // XX_GPIO(11) FastIO_12 bank 2, pin 18 (pin 288, SSP2_MISO)
    { 0x0154, 0x00000030 },		//MUXSEL5 SET(GPIO), page 708
    { 0x03a4, 0x00000400 },		//DRIVE10 SET(3.3V), page 759
    { 0x03a8, 0x00000300 },		//DRIVE10 CLR(4mA), page 759
    { 0x0628, 0x00040000 },		//PULL2 CLR(no), page 790
    { 0x0728, 0x00040000 },		//DOUT2 CLR, page 801

    // XX_GPIO(12) FastIO_13 bank 2, pin 16 (pin 280, SSP2_SCK)
    { 0x0154, 0x00000003 },		//MUXSEL5 SET(GPIO), page 708
    { 0x03a4, 0x00000004 },		//DRIVE10 SET(3.3V), page 759
    { 0x03a8, 0x00000003 },		//DRIVE10 CLR(4mA), page 759
    { 0x0628, 0x00010000 },		//PULL2 CLR(no), page 790
    { 0x0728, 0x00010000 },		//DOUT2 CLR, page 801

    // XX_GPIO(13) FastIO_14 bank 2, pin 19 (pin 4, SSP2_SS0)
    { 0x0154, 0x000000c0 },		//MUXSEL5 SET(GPIO), page 708
    { 0x03a4, 0x00004000 },		//DRIVE10 SET(3.3V), page 759
    { 0x03a8, 0x00003000 },		//DRIVE10 CLR(4mA), page 759
    { 0x0628, 0x00080000 },		//PULL2 CLR(no), page 790
    { 0x0728, 0x00080000 },		//DOUT2 CLR, page 801

    // XX_GPIO(14) FastIO_15 bank 2, pin 21 (pin 18, SSP2_SS2)
    { 0x0154, 0x00000c00 },		//MUXSEL5 SET(GPIO), page 708
    { 0x03a4, 0x00040000 },		//DRIVE10 SET(3.3V), page 759
    { 0x03a8, 0x00030000 },		//DRIVE10 CLR(4mA), page 759
    { 0x0628, 0x00200000 },		//PULL2 CLR(no), page 790
    { 0x0728, 0x00200000 },		//DOUT2 CLR, page 801

    // XX_GPIO(15) FastIO_16 bank 2, pin 25 (pin 9, SSP3_MOSI)
    { 0x0154, 0x000c0000 },		//MUXSEL5 SET(GPIO), page 708
    { 0x03b4, 0x00000040 },		//DRIVE11 SET(3.3V), page 762
    { 0x03b8, 0x00000030 },		//DRIVE11 CLR(4mA), page 762
    { 0x0628, 0x02000000 },		//PULL2 CLR(no), page 790
    { 0x0728, 0x02000000 },		//DOUT2 CLR, page 801

    // XX_GPIO(16) FastIO_17 bank 2, pin 26 (pin 3, SSP3_MISO)
    { 0x0154, 0x00300000 },		//MUXSEL5 SET(GPIO), page 708
    { 0x03b4, 0x00000400 },		//DRIVE11 SET(3.3V), page 762
    { 0x03b8, 0x00000300 },		//DRIVE11 CLR(4mA), page 762
    { 0x0628, 0x04000000 },		//PULL2 CLR(no), page 790
    { 0x0728, 0x04000000 },		//DOUT2 CLR, page 801

    // XX_GPIO(17) FastIO_18 bank 2, pin 9 (pin 275, SSP0_DETECT)
    { 0x0144, 0x000c0000 },		//MUXSEL4 SET(GPIO), page 705
    { 0x0394, 0x00000040 },		//DRIVE9 SET(3.3V), page 756
    { 0x0398, 0x00000030 },		//DRIVE9 CLR(4mA), page 756
    { 0x0628, 0x00000200 },		//PULL2 CLR(no), page 790
    { 0x0728, 0x00000200 },		//DOUT2 CLR, page 801

    // THE END
	{ 0xffff, 0xffffffff }
};

static struct {
    unsigned offset;
    unsigned mask;
} xx_gpio_doe[] = {

    // PINCTRL register in "i.MX28 Applications Processor Reference Manual", # MCIMX28RM, rev. 1, 2010, page 688

    // XX_GPIO( 0) FastIO_1 bank 2, pin 14 (pin 21, SSP1_DATA0)
    { 0x0b20, 0x00004000 },		//DOE2 SET/CLR(en.), page 810

    // XX_GPIO( 1) FastIO_2 bank 0, pin 17 (pin 131, GPMI_CE1N)
    { 0x0b00, 0x00020000 },		//DOEx SET/CLR(en.), page 810

    // XX_GPIO( 2) FastIO_3 bank 2, pin 12 (pin 11, SSP1_SCK)
    { 0x0b20, 0x00001000 },		//DOE2 SET/CLR(en.), page 810

    // XX_GPIO( 3) FastIO_4 bank 3, pin 6 (pin 78, AUART1_CTS)
    { 0x0b30, 0x00000040 },		//DOE3 SET/CLR(en.), page 810

    // XX_GPIO( 4) FastIO_5 bank 2, pin 20 (pin 7, SSP2_SS1)
    { 0x0b20, 0x00100000 },		//DOE2 SET/CLR(en.), page 810

    // XX_GPIO( 5) FastIO_6 bank 3, pin 2 (pin 70, AUART0_CTS)
    { 0x0b30, 0x00000004 },		//DOE3 SET/CLR(en.), page 811

    // XX_GPIO( 6) FastIO_7 bank 3, pin 4 (pin 81, AUART1_RX)
    { 0x0b30, 0x00000010 },		//DOE3 SET/CLR(en.), page 810

    // XX_GPIO( 7) FastIO_8 bank 3, pin 5 (pin 65, AUART1_TX)
    { 0x0b30, 0x00000020 },		//DOE3 SET/CLR(en.), page 810

    // XX_GPIO( 8) FastIO_9 bank 2, pin 24 (pin 286, SSP3_SCK)
    { 0x0b20, 0x01000000 },		//DOE2 SET/CLR(en.), page 810

    // XX_GPIO( 9) FastIO_10 bank 2, pin 27 (pin 15, SSP3_SS0)
    { 0x0b20, 0x08000000 },		//DOE2 SET/CLR(en.), page 810

    // XX_GPIO(10) FastIO_11 bank 2, pin 17 (pin 1, SSP2_MOSI)
    { 0x0b20, 0x00020000 },		//DOE2 SET/CLR(en.), page 810

    // XX_GPIO(11) FastIO_12 bank 2, pin 18 (pin 288, SSP2_MISO)
    { 0x0b20, 0x00040000 },		//DOE2 SET/CLR(en.), page 810

    // XX_GPIO(12) FastIO_13 bank 2, pin 16 (pin 280, SSP2_SCK)
    { 0x0b20, 0x00010000 },		//DOE2 SET/CLR(en.), page 810

    // XX_GPIO(13) FastIO_14 bank 2, pin 19 (pin 4, SSP2_SS0)
    { 0x0b20, 0x00080000 },		//DOE2 SET/CLR(en.), page 810

    // XX_GPIO(14) FastIO_15 bank 2, pin 21 (pin 18, SSP2_SS2)
    { 0x0b20, 0x00200000 },		//DOE2 SET/CLR(en.), page 810

    // XX_GPIO(15) FastIO_16 bank 2, pin 25 (pin 9, SSP3_MOSI)
    { 0x0b20, 0x02000000 },		//DOE2 SET/CLR(en.), page 810

    // XX_GPIO(16) FastIO_17 bank 2, pin 26 (pin 3, SSP3_MISO)
    { 0x0b20, 0x04000000 },		//DOE2 SET/CLR(en.), page 810

    // XX_GPIO(17) FastIO_18 bank 2, pin 9 (pin 275, SSP0_DETECT)
    { 0x0b20, 0x00000200 },		//DOE2 SET/CLR(en.), page 810

    // THE END
    { 0xffff, 0xffffffff }
};

static struct {
    unsigned offset;
    unsigned mask;
} xx_gpio_dout[] = {

    // PINCTRL register in "i.MX28 Applications Processor Reference Manual", # MCIMX28RM, rev. 1, 2010, page 688

    // XX_GPIO( 0) FastIO_1 bank 2, pin 14 (pin 21, SSP1_DATA0)
    { 0x0720, 0x00004000 },		//DOUT2, page 801

    // XX_GPIO( 1) FastIO_2 bank 0, pin 17 (pin 131, GPMI_CE1N)
    { 0x0700, 0x00020000 },		//DOUT0, page 800

    // XX_GPIO( 2) FastIO_3 bank 2, pin 12 (pin 11, SSP1_SCK)
    { 0x0720, 0x00001000 },		//DOUT2, page 801

    // XX_GPIO( 3) FastIO_4 bank 3, pin 6 (pin 78, AUART1_CTS)
    { 0x0730, 0x00000040 },		//DOUT3, page 802

    // XX_GPIO( 4) FastIO_5 bank 2, pin 20 (pin 7, SSP2_SS1)
    { 0x0720, 0x00100000 },		//DOUT2, page 801

    // XX_GPIO( 5) FastIO_6 bank 3, pin 2 (pin 70, AUART0_CTS)
    { 0x0730, 0x00000004 },		//DOUT3, page 802

    // XX_GPIO( 6) FastIO_7 bank 3, pin 4 (pin 81, AUART1_RX)
    { 0x0730, 0x00000010 },		//DOUT3, page 802

    // XX_GPIO( 7) FastIO_8 bank 3, pin 5 (pin 65, AUART1_TX)
    { 0x0730, 0x00000020 },		//DOUT3, page 802

    // XX_GPIO( 8) FastIO_9 bank 2, pin 24 (pin 286, SSP3_SCK)
    { 0x0720, 0x01000000 },		//DOUT2, page 801

    // XX_GPIO( 9) FastIO_10 bank 2, pin 27 (pin 15, SSP3_SS0)
    { 0x0720, 0x08000000 },		//DOUT2, page 801

    // XX_GPIO(10) FastIO_11 bank 2, pin 17 (pin 1, SSP2_MOSI)
    { 0x0720, 0x00020000 },		//DOUT2, page 801

    // XX_GPIO(11) FastIO_12 bank 2, pin 18 (pin 288, SSP2_MISO)
    { 0x0720, 0x00040000 },		//DOUT2, page 801

    // XX_GPIO(12) FastIO_13 bank 2, pin 16 (pin 280, SSP2_SCK)
    { 0x0720, 0x00010000 },		//DOUT2, page 801

    // XX_GPIO(13) FastIO_14 bank 2, pin 19 (pin 4, SSP2_SS0)
    { 0x0720, 0x00080000 },		//DOUT2, page 801

    // XX_GPIO(14) FastIO_15 bank 2, pin 21 (pin 18, SSP2_SS2)
    { 0x0720, 0x00200000 },		//DOUT2, page 801

    // XX_GPIO(15) FastIO_16 bank 2, pin 25 (pin 9, SSP3_MOSI)
    { 0x0720, 0x02000000 },		//DOUT2, page 801

    // XX_GPIO(16) FastIO_17 bank 2, pin 26 (pin 3, SSP3_MISO)
    { 0x0720, 0x04000000 },		//DOUT2, page 801

    // XX_GPIO(17) FastIO_18 bank 2, pin 9 (pin 275, SSP0_DETECT)
    { 0x0720, 0x00000200 },		//DOUT2, page 801

    // THE END
    { 0xffff, 0xffffffff }
};

static struct {
    unsigned offset;
    unsigned mask;
} xx_gpio_din[] = {

    // PINCTRL register in "i.MX28 Applications Processor Reference Manual", # MCIMX28RM, rev. 1, 2010, page 688

    // XX_GPIO( 0) FastIO_1 bank 2, pin 14 (pin 21, SSP1_DATA0)
    { 0x0920, 0x00004000 },		//DIN2, page 806

    // XX_GPIO( 1) FastIO_2 bank 0, pin 17 (pin 131, GPMI_CE1N)
    { 0x0900, 0x00020000 },		//DIN0, page 804

    // XX_GPIO( 2) FastIO_3 bank 2, pin 12 (pin 11, SSP1_SCK)
    { 0x0920, 0x00001000 },		//DIN2, page 806

    // XX_GPIO( 3) FastIO_4 bank 3, pin 6 (pin 78, AUART1_CTS)
    { 0x0930, 0x00000040 },		//DIN3, page 802

    // XX_GPIO( 4) FastIO_5 bank 2, pin 20 (pin 7, SSP2_SS1)
    { 0x0920, 0x00100000 },		//DIN2, page 806

    // XX_GPIO( 5) FastIO_6 bank 3, pin 2 (pin 70, AUART0_CTS)
    { 0x0930, 0x00000004 },		//DIN3, page 806

    // XX_GPIO( 6) FastIO_7 bank 3, pin 4 (pin 81, AUART1_RX)
    { 0x0930, 0x00000010 },		//DIN3, page 806

    // XX_GPIO( 7) FastIO_8 bank 3, pin 5 (pin 65, AUART1_TX)
    { 0x0930, 0x00000020 },		//DIN3, page 806

    // XX_GPIO( 8) FastIO_9 bank 2, pin 24 (pin 286, SSP3_SCK)
    { 0x0920, 0x01000000 },		//DIN2, page 806

    // XX_GPIO( 9) FastIO_10 bank 2, pin 27 (pin 15, SSP3_SS0)
    { 0x0920, 0x08000000 },		//DIN2, page 806

    // XX_GPIO(10) FastIO_11 bank 2, pin 17 (pin 1, SSP2_MOSI)
    { 0x0920, 0x00020000 },		//DIN2, page 806

    // XX_GPIO(11) FastIO_12 bank 2, pin 18 (pin 288, SSP2_MISO)
    { 0x0920, 0x00040000 },		//DIN2, page 806

    // XX_GPIO(12) FastIO_13 bank 2, pin 16 (pin 280, SSP2_SCK)
    { 0x0920, 0x00010000 },		//DIN2, page 806

    // XX_GPIO(13) FastIO_14 bank 2, pin 19 (pin 4, SSP2_SS0)
    { 0x0920, 0x00080000 },		//DIN2, page 806

    // XX_GPIO(14) FastIO_15 bank 2, pin 21 (pin 18, SSP2_SS2)
    { 0x0920, 0x00200000 },		//DIN2, page 806

    // XX_GPIO(15) FastIO_16 bank 2, pin 25 (pin 9, SSP3_MOSI)
    { 0x0920, 0x02000000 },		//DIN2, page 806

    // XX_GPIO(16) FastIO_17 bank 2, pin 26 (pin 3, SSP3_MISO)
    { 0x0920, 0x04000000 },		//DIN2, page 806

    // XX_GPIO(17) FastIO_18 bank 2, pin 9 (pin 275, SSP0_DETECT)
    { 0x0920, 0x00000200 },		//DIN2, page 806

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

#ifdef __XENO__
static unsigned threads_num = 0;
#endif
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
    if (stacksize < 65536) {
		stacksize = 65536;
	}

    pthread_attr_setdetachstate(attr_p, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(attr_p, stacksize);
	pthread_attr_setinheritsched(attr_p, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setscope(attr_p, PTHREAD_SCOPE_PROCESS);

	printf("\n");
	fflush(stdout);

	retval = pthread_create(thread, attr, start_routine, arg);
    if (retval) {
        fprintf(stderr, "[%s] ERROR creating %s thread: %s.\n", __func__, name, strerror(errno));
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

#if 0
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
    struct timespec rqtp, rmtp;
    ldiv_t q;

    q = ldiv(ulTime, 1000);
    rqtp.tv_sec = q.quot;
    rqtp.tv_nsec = q.rem * 1E6; // ms -> ns

    while (clock_nanosleep(CLOCK_REALTIME, 0, &rqtp, &rmtp) == EINTR) {
        rqtp.tv_sec = rmtp.tv_sec;
        rqtp.tv_nsec = rmtp.tv_nsec;
    }
#endif
	RETURN(uRes);
}

/* ---------------------------------------------------------------------------- */
/**
 * osSleepAbsolute			
 *
 * Suspend the task until the given absolute time (in ms).
 *
 * @param			ullTime		Suspend time in ms. (64bit)
 * @return			OK if successful else error number.
 */
IEC_UINT osSleepAbsolute(IEC_ULINT ullTime)
{
	IEC_UINT uRes = OK;

#ifdef __XENO__
    struct timespec timer_next;
    lldiv_t x;
	int retval; 

	// compute time
    x = lldiv(ullTime, 1000ULL);
    timer_next.tv_sec = x.quot;
    timer_next.tv_nsec = x.rem * 1E6;
	// do wait
	do {
       	retval = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &timer_next, NULL);
	} while (retval == EINTR);
#else
    IEC_ULINT now = osGetTime64();

    if (ullTime > now) {
        IEC_UDINT delta_ms = ullTime - now;
        usleep(delta_ms * 1000);
	}
#endif
	RETURN(uRes);
}

IEC_UDINT osElapsedTime32(IEC_UDINT now, IEC_UDINT start)
{
    IEC_UDINT elapsed;

    if (now >= start)
        elapsed = now - start;
    else
        elapsed = (0xFFFFffff - start) + now + 1;

    return elapsed;
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

unsigned xx_gpio_enabled;

void xx_gpio_init(void)
{
    xx_gpio_enabled = 0;

	xx_fd = open("/dev/mem", O_RDWR, 0);
	if (xx_fd <= 0)
		return;

    xx_base_ptr = mmap(NULL, XX_GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, xx_fd, XX_GPIO_BASE);
}

void xx_gpio_enable(unsigned n)
{
    if (xx_base_ptr && n < XX_GPIO_MAX) {
        // configure as GPIO
        unsigned i = 0;
        unsigned *reg_ptr = NULL;

        for (i = (n*5); i < ((n+1)*5) && xx_gpio_enabler[i].offset < 0xffff; i++) {
            reg_ptr = (unsigned *)(xx_base_ptr + XX_PINCTL_BASE + xx_gpio_enabler[i].offset);
            *reg_ptr = xx_gpio_enabler[i].value;
        }

        // configure as input (default)
        xx_gpio_config(n, 0);
        xx_gpio_enabled |= (1 << n);
    }
}

void xx_gpio_config(unsigned n, int output)
{
    if (xx_base_ptr && n < XX_GPIO_MAX && (xx_gpio_enabled & (1 << n))) {
        register unsigned *reg_ptr;

        if (output)
            reg_ptr = (unsigned *)(xx_base_ptr + XX_PINCTL_BASE + xx_gpio_doe[n].offset + XX_GPIO_SET_OFFSET);
        else
            reg_ptr = (unsigned *)(xx_base_ptr + XX_PINCTL_BASE + xx_gpio_doe[n].offset + XX_GPIO_CLR_OFFSET);
        *reg_ptr = xx_gpio_doe[n].mask;
    }
}

void xx_gpio_set(unsigned n)
{
    if (xx_base_ptr && n < XX_GPIO_MAX && (xx_gpio_enabled & (1 << n))) {
		register unsigned *reg_ptr;

        reg_ptr = (unsigned *)(xx_base_ptr + XX_PINCTL_BASE + xx_gpio_dout[n].offset + XX_GPIO_SET_OFFSET);
        *reg_ptr = xx_gpio_dout[n].mask;
	}
}

void xx_gpio_clr(unsigned n)
{
    if (xx_base_ptr && n < XX_GPIO_MAX && (xx_gpio_enabled & (1 << n))) {
		register unsigned *reg_ptr;

        reg_ptr = (unsigned *)(xx_base_ptr + XX_PINCTL_BASE + xx_gpio_dout[n].offset + XX_GPIO_CLR_OFFSET);
        *reg_ptr = xx_gpio_dout[n].mask;
	}
}

int xx_gpio_get(unsigned n)
{
    register int retval = 0;

    if (xx_base_ptr && n < XX_GPIO_MAX && (xx_gpio_enabled & (1 << n))) {
        retval = (*((unsigned *)(xx_base_ptr + XX_PINCTL_BASE + xx_gpio_din[n].offset)) & xx_gpio_din[n].mask) ? 1 : 0;
    }
    return retval;
}

void xx_gpio_close(void)
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

void xx_watchdog_enable()
{
    if (xx_base_ptr) {
        register unsigned *reg_ptr;

        reg_ptr = (unsigned *)(xx_base_ptr + XX_RTCCTL_BASE + XX_WATCHDOGEN_OFFS + XX_GPIO_SET_OFFSET);
        *reg_ptr = XX_WATCHDOGEN_MASK;
    }
}

void xx_watchdog_disable()
{
    if (xx_base_ptr) {
        register unsigned *reg_ptr;

        reg_ptr = (unsigned *)(xx_base_ptr + XX_RTCCTL_BASE + XX_WATCHDOGEN_OFFS + XX_GPIO_CLR_OFFSET);
        *reg_ptr = XX_WATCHDOGEN_MASK;
    }
}

void xx_watchdog_reset(unsigned value_ms)
{
    if (xx_base_ptr) {
        register unsigned *reg_ptr;

        reg_ptr = (unsigned *)(xx_base_ptr + XX_RTCCTL_BASE + XX_WATCHDOGms_OFFS);
        *reg_ptr = value_ms;
    }
}

unsigned xx_watchdog_get()
{
    register unsigned retval = 0;

    if (xx_base_ptr) {
        register unsigned *reg_ptr;
        reg_ptr = (unsigned *)(xx_base_ptr + XX_RTCCTL_BASE + XX_WATCHDOGms_OFFS);
        retval = *reg_ptr;
    }
    return retval;
}

void xx_pwm3_set(unsigned duty_cycle)
{
    if (xx_base_ptr) {
        register unsigned *reg_ptr;
        //register unsigned period_cycles = 37495; // 100ms = 37495 * 2.667us (24Mhz/64)
        register unsigned period_cycles = 3750; // 10ms = 3750 * 2.667us (24Mhz/64)

        // PERIOD=period_cycles ACTIVE_STATE=1 INACTIVE_STATE=0 CDIV=DIV_64
        reg_ptr = (unsigned *)(xx_base_ptr + XX_PWMCTL_BASE + XX_PWM3_PERIOD_OFFS);
        *reg_ptr = period_cycles + 0x005B0000;

        // ACTIVE=0 INACTIVE=(duty cycle)
        if (duty_cycle > 100)
            duty_cycle = 100;
        reg_ptr = (unsigned *)(xx_base_ptr + XX_PWMCTL_BASE + XX_PWM3_ACTIVE_OFFS);
        *reg_ptr = (period_cycles * duty_cycle / 100) << 16;
    }
}

void xx_pwm3_enable()
{
    if (xx_base_ptr) {
        register unsigned *reg_ptr;

        reg_ptr = (unsigned *)(xx_base_ptr + XX_PWMCTL_BASE + XX_PMW3_ENABLE_OFFS + XX_GPIO_SET_OFFSET);
        *reg_ptr = XX_PMW3_ENABLE_MASK;
    }
}

void xx_pwm3_disable()
{
    if (xx_base_ptr) {
        register unsigned *reg_ptr;

        reg_ptr = (unsigned *)(xx_base_ptr + XX_PWMCTL_BASE + XX_PMW3_ENABLE_OFFS  + XX_GPIO_CLR_OFFSET);
        *reg_ptr = XX_PMW3_ENABLE_MASK;
    }
}


#endif

void *osMemCpy(void *dest, const void *src, size_t n) // __attribute__ ((optimize("O0")))
{
	return memcpy(dest, src, n);
}
/* ---------------------------------------------------------------------------- */
