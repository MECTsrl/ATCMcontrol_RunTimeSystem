
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
 # Filename: flags.4cdc.mk
 #
 # ------------------------------------------------------------------------------
 #


AR               = /usr/bin/mipsel-linux-ar
AS               = /usr/bin/mipsel-linux-gcc
CC               = /usr/bin/mipsel-linux-gcc
LD               = /usr/bin/mipsel-linux-gcc
RM               = rm
MD               = mkdir
GD               = ../gnu_dep
G2D              = ../gnu2vs
CRD              = ../CreatDir
DED              = ../DelDir

TFLAGS           = -mtune=r4100 -mips2 -Wa,--trap
IFLAGS           = -I. -I../inc -I../inc.fc -I../inc.dp -I../inc.vis
SFLAGS           = 
DFLAGS           = -D_SOF_4CDC_SRC_

CFLAGS           = $(TFLAGS) -Wall -DRW_MULTI_THREAD -D_REENTRANT -fvolatile -fno-builtin -fno-strict-aliasing $(IFLAGS) $(SFLAGS) $(DFLAGS)
CFLAGS_AS        = $(CFLAGS)

LD_FLAGS         = -lm -lpthread $(CFLAGS)
AR_FLAGS         = -rc

BIN_PATH         = ../bin
LIB_PATH         = ../lib

MAKS             = ../build.mk ../flags.4cdc.mk objects.mk $(SRC_PATH).mak

# -------------------------------------------------------------------------------
