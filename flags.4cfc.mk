
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
 # Filename: flags.4cfc.mk
 #
 # ------------------------------------------------------------------------------
 #


AR               = /home/mfp/cdk/bin/powerpc-tuxbox-linux-gnu-ar
AS               = /home/mfp/cdk/bin/powerpc-tuxbox-linux-gnu-gcc
CC               = /home/mfp/cdk/bin/powerpc-tuxbox-linux-gnu-gcc
LD               = /home/mfp/cdk/bin/powerpc-tuxbox-linux-gnu-g++
RM               = rm
MD               = mkdir
GD               = ../gnu_dep
G2D              = ../gnu2vs
CRD              = ../CreatDir
DED              = ../DelDir

TFLAGS           = -mcpu=860 -DCPU=PPC860 -DCPU_FAMILY=PPC
IFLAGS           = -I. -I../inc -I../inc.fc -I../inc.dp -I../inc.vis
SFLAGS           = -isystem /LinuxPPC/usr/src/linux/include
DFLAGS           = -D_SOF_4CFC_SRC_

CFLAGS           = $(TFLAGS) -Wall -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-for-scope $(IFLAGS) $(SFLAGS) $(DFLAGS)
CFLAGS_AS        = $(CFLAGS)

LD_FLAGS         = -lm -lpthread -L../lib.bac.4cfc -lsqlite3 -lbacnapi $(CFLAGS)
AR_FLAGS         = -rc

BIN_PATH         = ../bin
LIB_PATH         = ../lib

MAKS             = ../build.mk ../flags.4cfc.mk objects.mk $(SRC_PATH).mak

# -------------------------------------------------------------------------------
