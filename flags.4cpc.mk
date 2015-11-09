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

ifeq ("$(PRODUCT)", "USE_CROSSTABLE")
$(info "Product CROSSTABLE")
else ifeq ("$(PRODUCT)", "USE_NO_CROSSTABLE")
$(info "Product NO CROSSTABLE")
else
$(error "SPECIFY PRODUCT (USE_CROSSTABLE|USE_NO_CROSSTABLE)")
endif

ifeq ($(FREESCALE_GCC), 1)

ROOTFS = /imx_mect/trunk/imx28/ltib/rootfs
CC_VERSION       = gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi
#CC_VERSION       = gcc-4.3.3-glibc-2.8-cs2009q1-203
#CC_VERSION       = gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi
CC_DIRECTORY     = /opt/freescale/usr/local/$(CC_VERSION)/bin/
CC_RADIX         = arm-none-linux-gnueabi-
ARCH_INCLUDE     = \
  -I/imx_mect/trunk/imx28/ltib/rootfs/usr/include \
  -I/imx_mect/trunk/imx28/ltib/rootfs/usr/src/linux/include \
  -I/opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/arm-none-linux-gnueabi/sysroot/usr/include/
  #-I/opt/freescale/usr/local/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/arm-fsl-linux-gnueabi/multi-libs/usr/include/
XENO_CC = gcc
XENO_CFLAGS = -I$(ROOTFS)/usr/xenomai/include -D_GNU_SOURCE -D_REENTRANT -Wall -Werror-implicit-function-declaration -pipe -D__XENO__ -I$(ROOTFS)/usr/xenomai/include/posix
XENO_LDFLAGS = -Wl,@$(ROOTFS)/usr/xenomai/lib/posix.wrappers -L$(ROOTFS)/usr/xenomai/lib -lpthread_rt -lxenomai -lrtdm -lpthread -lrt
XENO_LDFLAGS += -Xlinker -rpath -Xlinker $(ROOTFS)/usr/xenomai/lib

else ifeq ($(SOURCERY_GCC), 1)

ROOTFS = /imx_mect/trunk/imx28/ltib/rootfs
CC_VERSION       = Sourcery_G++_Lite
CC_DIRECTORY     = /home/imx28/CodeSourcery/$(CC_VERSION)/bin/
CC_RADIX         = arm-none-linux-gnueabi-
ARCH_INCLUDE     = \
	-I/imx_mect/trunk/imx28/ltib/rootfs/usr/include \
	-I/imx_mect/trunk/imx28/ltib/rootfs/usr/src/linux/include \
	-I/home/imx28/CodeSourcery/Sourcery_G++_Lite/arm-none-linux-gnueabi
XENO_CC = gcc
XENO_CFLAGS = -I$(ROOTFS)/usr/xenomai/include -D_GNU_SOURCE -D_REENTRANT -Wall -Werror-implicit-function-declaration -pipe -D__XENO__ -I$(ROOTFS)/usr/xenomai/include/posix
XENO_LDFLAGS = -Wl,@$(ROOTFS)/usr/xenomai/lib/posix.wrappers -L$(ROOTFS)/usr/xenomai/lib -lpthread_rt -lxenomai -lrtdm -lpthread -lrt
XENO_LDFLAGS += -Xlinker -rpath -Xlinker $(ROOTFS)/usr/xenomai/lib

else

ROOTFS = /home/imx_mect/ltib/rootfs
CC_VERSION       =
CC_DIRECTORY     = /opt/CodeSourcery/$(CC_VERSION)/bin/
CC_RADIX         = arm-none-linux-gnueabi-
ARCH_INCLUDE     = \
        -I$(ROOTFS)/usr/include \
        -I$(ROOTFS)/usr/src/linux/include \
        -I$(CC_DIRECTORY)/arm-none-linux-gnueabi
XENO_CC = gcc
XENO_CFLAGS = -I$(ROOTFS)/usr/xenomai/include -D_GNU_SOURCE -D_REENTRANT -Werror-implicit-function-declaration -pipe -D__XENO__ -I$(ROOTFS)/usr/xenomai/include/posix
XENO_LDFLAGS = -Wl,@$(ROOTFS)/usr/xenomai/lib/posix.wrappers -L$(ROOTFS)/usr/xenomai/lib -lpthread_rt -lxenomai -lrtdm -lpthread -lrt
XENO_LDFLAGS += -Xlinker -rpath -Xlinker $(ROOTFS)/usr/xenomai/lib

## XENOCONFIG = /usr/bin/xeno-config
## XENO_CC = $(shell $(XENOCONFIG) --cc)
## XENO_CFLAGS = $(shell $(XENOCONFIG) --skin=posix --cflags)
## XENO_LDFLAGS = $(shell $(XENOCONFIG) --skin=posix --ldflags)
## # This includes the library path of given Xenomai into the binary to make live
## # easier for beginners if Xenomai's libs are not in any default search path.
## XENO_LDFLAGS += -Xlinker -rpath -Xlinker $(shell $(XENOCONFIG) --libdir)

endif

# /opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/arm-none-linux-gnueabi/sysroot/usr/include/pthread.h
# /opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/arm-none-linux-gnueabi/sysroot/vfp/usr/include/pthread.h
# /opt/freescale/usr/local/gcc-4.3.3-glibc-2.8-cs2009q1-203/arm-none-linux-gnueabi/arm-none-linux-gnueabi/libc/usr/include/pthread.h
# /opt/freescale/usr/local/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/arm-fsl-linux-gnueabi/multi-libs/usr/include/pthread.h

#ARCH_DFLAGS      = "-D__arm -march=armv5te -mcpu=arm926ej-s -mtune=arm926ej-s -mthumb -mthumb-interwork -mword-relocations"
ARCH_DFLAGS      = -march=armv5te -mtune=arm926ej-s

#CC_DIRECTORY     = /home/imx28/tpac020_01-eva/br/build_arm/staging_dir/usr/bin/
#CC_RADIX         = arm-linux-uclibc-
#ARCH_INCLUDE     = -I/home/imx28/tpac020_01-eva/br/build_arm/staging_dir/include
#ARCH_DFLAGS      = -D__arm

#CC_DIRECTORY     =
#CC_RADIX         =
#ARCH_INCLUDE     =
#ARCH_DFLAGS      =


AR               = $(CC_DIRECTORY)$(CC_RADIX)ar
AS               = $(CC_DIRECTORY)$(CC_RADIX)gcc
##CC               = $(CC_DIRECTORY)$(CC_RADIX)gcc -Wcast-align
CC               = $(CC_DIRECTORY)$(CC_RADIX)gcc
LD               = $(CC_DIRECTORY)$(CC_RADIX)gcc
RM               = rm
MD               = mkdir
GD               = ../gnu_dep
G2D              = ../gnu2vs
CRD              = ../CreatDir
DED              = ../DelDir

TFLAGS           =
IFLAGS           = -I. -I../inc -I../inc.fc -I../inc.dp -I../inc.vis -I../vmLib -I../inc.can -I../inc.mect -I../inc.udp -I../inc.kpd -I../inc.data -I../inc.syncro -I../inc.mbtcps -I../inc.mbrtuc $(ARCH_INCLUDE) -D$(PRODUCT)
SFLAGS           =
DFLAGS           = -D_SOF_4CPC_SRC_ $(ARCH_DFLAGS)
ifeq ($(DEBUG), 1)
  DFLAGS += -DDEBUG
  OFLAGS = -O0
else
  OFLAGS = -O2
endif

CFLAGS           = $(XENO_CFLAGS) $(TFLAGS) -DRW_MULTI_THREAD -D_GNU_SOURCE -D_REENTRANT -Wall -Wno-attributes -fno-builtin -fno-strict-aliasing $(IFLAGS) $(SFLAGS) $(DFLAGS)
CFLAGS_AS        = $(CFLAGS)

LD_FLAGS         = $(XENO_LDFLAGS) -lm -lpthread $(CFLAGS) -lrt -L$(ROOTFS)/usr/lib -lts -lsocketcan -ldl
AR_FLAGS         = -rc

BIN_PATH         = ../bin
LIB_PATH         = ../lib

MAKS             = ../build.mk ../flags.4cpc.mk objects.mk $(SRC_PATH).mak

# -------------------------------------------------------------------------------
