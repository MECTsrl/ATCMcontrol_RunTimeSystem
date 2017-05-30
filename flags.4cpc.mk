 # Copyright 2011 Mect s.r.l
 #
 # This file is part of FarosPLC.
 #
 # FarosPLC is free software: you can redistribute it and/or modify it under
 # the terms of the GNU General Public License as published by the Free Software
 # Foundation, either version 3 of the License, or (at your option) any later
 # version.
 # 
 # FarosPLC is distributed in the hope that it will be useful, but WITHOUT ANY
 # WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 # A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 # 
 # You should have received a copy of the GNU General Public License along with
 # FarosPLC. If not, see http://www.gnu.org/licenses/.
 #
 # Filename: flags.4cpc.mk
 #
 # ------------------------------------------------------------------------------
 #

ifeq ($(FREESCALE_GCC), 1)

MECT_ROOTFS       = /imx_mect/trunk/imx28/ltib/rootfs
MECT_CC_VERSION   = gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi
#MECT_CC_VERSION  = gcc-4.3.3-glibc-2.8-cs2009q1-203
#MECT_CC_VERSION  = gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi
MECT_CC_DIRECTORY = /opt/freescale/usr/local
MECT_CC_BINDIR    = $(MECT_CC_DIRECTORY)/$(MECT_CC_VERSION)/bin/
MECT_CC_RADIX     = arm-none-linux-gnueabi
ARCH_INCLUDE      = \
  -I/imx_mect/trunk/imx28/ltib/rootfs/usr/include \
  -I/imx_mect/trunk/imx28/ltib/rootfs/usr/src/linux/include \
  -I/opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/arm-none-linux-gnueabi/sysroot/usr/include/
  #-I/opt/freescale/usr/local/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/arm-fsl-linux-gnueabi/multi-libs/usr/include/
XENO_CC = gcc
XENO_CFLAGS  = -I$(MECT_ROOTFS)/usr/xenomai/include -D_GNU_SOURCE -D_REENTRANT -Wall -Werror-implicit-function-declaration -pipe -D__XENO__ -I$(MECT_ROOTFS)/usr/xenomai/include/posix
XENO_LDFLAGS = -Wl,@$(MECT_ROOTFS)/usr/xenomai/lib/posix.wrappers -L$(MECT_ROOTFS)/usr/xenomai/lib -lpthread_rt -lxenomai -lrtdm -lpthread -lrt
XENO_LDFLAGS += -Xlinker -rpath -Xlinker $(MECT_ROOTFS)/usr/xenomai/lib

else ifeq ($(SOURCERY_GCC), 1)

MECT_ROOTFS       = /imx_mect/trunk/imx28/ltib/rootfs
MECT_CC_VERSION   = Sourcery_G++_Lite
MECT_CC_DIRECTORY = /home/imx28/CodeSourcery
MECT_CC_BINDIR    = $(MECT_CC_DIRECTORY)/$(MECT_CC_VERSION)/bin/
MECT_CC_RADIX     = arm-none-linux-gnueabi
ARCH_INCLUDE      = \
	-I$(MECT_ROOTFS)/usr/include \
	-I$(MECT_ROOTFS)/usr/src/linux/include \
	-I$(MECT_CC_DIRECTORY)/$(MECT_CC_VERSION)/$(MECT_CC_RADIX)
XENO_CC = gcc
XENO_CFLAGS = -I$(MECT_ROOTFS)/usr/xenomai/include -D_GNU_SOURCE -D_REENTRANT -Wall -Werror-implicit-function-declaration -pipe -D__XENO__ -I$(MECT_ROOTFS)/usr/xenomai/include/posix
XENO_LDFLAGS = -Wl,@$(MECT_ROOTFS)/usr/xenomai/lib/posix.wrappers -L$(MECT_ROOTFS)/usr/xenomai/lib -lpthread_rt -lxenomai -lrtdm -lpthread -lrt
XENO_LDFLAGS += -Xlinker -rpath -Xlinker $(MECT_ROOTFS)/usr/xenomai/lib

else

MECT_ROOTFS       = $(HOME)/mect_suite_3.0/imx_mect/ltib/rootfs
MECT_CC_VERSION   =
MECT_CC_DIRECTORY = /opt/CodeSourcery
MECT_CC_BINDIR    = $(MECT_CC_DIRECTORY)/$(MECT_CC_VERSION)/bin/
MECT_CC_RADIX     = arm-none-linux-gnueabi
ARCH_INCLUDE      = \
        -I$(MECT_ROOTFS)/usr/include \
        -I$(MECT_ROOTFS)/usr/src/linux/include \
        -I$(MECT_CC_DIRECTORY)/$(MECT_CC_VERSION)/$(MECT_CC_RADIX)
XENO_CC = gcc
##XENO_CFLAGS  = -D_GNU_SOURCE -D_REENTRANT -Werror-implicit-function-declaration -pipe
##XENO_LDFLAGS = -lpthread -lrt
XENO_CFLAGS  = -I$(MECT_ROOTFS)/usr/xenomai/include -D_GNU_SOURCE -D_REENTRANT -Werror-implicit-function-declaration -pipe -D__XENO__ -I$(MECT_ROOTFS)/usr/xenomai/include/posix
XENO_LDFLAGS = -Wl,@$(MECT_ROOTFS)/usr/xenomai/lib/posix.wrappers -L$(MECT_ROOTFS)/usr/xenomai/lib -lpthread_rt -lxenomai -lrtdm -lpthread -lrt
XENO_LDFLAGS += -Xlinker -rpath -Xlinker $(MECT_ROOTFS)/usr/xenomai/lib

## XENOCONFIG   = /usr/bin/xeno-config
## XENO_CC      = $(shell $(XENOCONFIG) --cc)
## XENO_CFLAGS  = $(shell $(XENOCONFIG) --skin=posix --cflags)
## XENO_LDFLAGS = $(shell $(XENOCONFIG) --skin=posix --ldflags)
## # This includes the library path of given Xenomai into the binary to make live
## # easier for beginners if Xenomai's libs are not in any default search path.
## XENO_LDFLAGS += -Xlinker -rpath -Xlinker $(shell $(XENOCONFIG) --libdir)

endif

# /opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/arm-none-linux-gnueabi/sysroot/usr/include/pthread.h
# /opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/arm-none-linux-gnueabi/sysroot/vfp/usr/include/pthread.h
# /opt/freescale/usr/local/gcc-4.3.3-glibc-2.8-cs2009q1-203/arm-none-linux-gnueabi/arm-none-linux-gnueabi/libc/usr/include/pthread.h
# /opt/freescale/usr/local/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/arm-fsl-linux-gnueabi/multi-libs/usr/include/pthread.h

#ARCH_DFLAGS       = "-D__arm -march=armv5te -mcpu=arm926ej-s -mtune=arm926ej-s -mthumb -mthumb-interwork -mword-relocations"
ARCH_DFLAGS       = -march=armv5te -mtune=arm926ej-s

#MECT_CC_DIRECTORY = /home/imx28/tpac020_01-eva/br/build_arm/staging_dir/usr/bin/
#MECT_CC_RADIX     = arm-linux-uclibc-
#ARCH_INCLUDE      = -I/home/imx28/tpac020_01-eva/br/build_arm/staging_dir/include
#ARCH_DFLAGS       = -D__arm

#MECT_CC_DIRECTORY =
#MECT_CC_RADIX     =
#ARCH_INCLUDE      =
#ARCH_DFLAGS       =


ifdef MECT_CC_RADIX
AR                = $(MECT_CC_BINDIR)$(MECT_CC_RADIX)-ar
AS                = $(MECT_CC_BINDIR)$(MECT_CC_RADIX)-gcc
##CC                = $(MECT_CC_BINDIR)$(MECT_CC_RADIX)-gcc -Wcast-align
CC                = $(MECT_CC_BINDIR)$(MECT_CC_RADIX)-gcc
LD                = $(MECT_CC_BINDIR)$(MECT_CC_RADIX)-gcc
else
AR                = $(MECT_CC_BINDIR)ar
AS                = $(MECT_CC_BINDIR)gcc
##CC                = $(MECT_CC_BINDIR)gcc -Wcast-align
CC                = $(MECT_CC_BINDIR)gcc
LD                = $(MECT_CC_BINDIR)gcc
endif
RM                = rm
MD                = mkdir
GD                = ../gnu_dep
G2D               = ../gnu2vs
CRD               = ../CreatDir
DED               = ../DelDir

TFLAGS            =
IFLAGS            = -I. -I../inc -I../inc.fc -I../inc.dp -I../inc.vis -I../vmLib -I../inc.mect -I../inc.data $(ARCH_INCLUDE)
SFLAGS            =
DFLAGS            = -D_SOF_4CPC_SRC_ $(ARCH_DFLAGS) -DXENO_RTDM=1
ifeq ($(DEBUG), 1)
  DFLAGS += -DDEBUG
  OFLAGS = -O0
else
  OFLAGS = -O2
endif

CFLAGS            = $(XENO_CFLAGS) $(TFLAGS) -DRW_MULTI_THREAD -D_GNU_SOURCE -D_REENTRANT -Wall -Wno-attributes -fno-builtin -fno-strict-aliasing $(IFLAGS) $(SFLAGS) $(DFLAGS)
CFLAGS_AS         = $(CFLAGS)

LD_FLAGS          = $(XENO_LDFLAGS) -lm -lpthread $(CFLAGS) -lrt -L$(MECT_ROOTFS)/usr/lib -lts -lsocketcan -ldl
AR_FLAGS          = -rc

BIN_PATH          = ../bin
LIB_PATH          = ../lib

MAKS              = ../build.mk ../flags.4cpc.mk objects.mk $(SRC_PATH).mak

# -------------------------------------------------------------------------------
