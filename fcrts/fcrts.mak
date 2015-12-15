
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
 # Filename: fcrts.mak
 #
 # ------------------------------------------------------------------------------
 #


ifeq ($(TARGET), 4CFC)
  include ../flags.4cfc.mk
else
  ifeq ($(TARGET), 4CDC)
    include ../flags.4cdc.mk
  else
    ifeq ($(TARGET), 4CPC)
      include ../flags.4cpc.mk
    else
      include ../___TARGET_IS_MISSING___
    endif
  endif    
endif


SRC_PATH           = fcrts


ifeq ($(DEBUG), 0)    # --- Release -------------------------------------------

  EXFLAGS          =
  XFLAGS           = $(GDBFLAG) $(OFLAGS) $(EXFLAGS)

  OBJ_PATH         = ./Release

  BIN_NAME         = $(BIN_PATH)/$(SRC_PATH)
  LIB_NAME         = 

  WA_LIBS          = $(LIB_PATH)/vmKernel.a $(LIB_PATH)/osKernel.a  

  LIBS             = $(LIB_PATH)/vmLib.a   $(LIB_PATH)/osShared.a \
                     $(LIB_PATH)/ioTest.a $(LIB_PATH)/ioData.a

  MFP_LIBS         = 

else                  # --- Debug ---------------------------------------------

  EXFLAGS          =
  XFLAGS           = -g -O0 -DDEBUG $(EXFLAGS)

  OBJ_PATH         = ./Debug

  BIN_NAME         = $(BIN_PATH)/$(SRC_PATH)_d
  LIB_NAME         = 

  WA_LIBS          = $(LIB_PATH)/vmKernel_d.a $(LIB_PATH)/osKernel_d.a

  LIBS             = $(LIB_PATH)/vmLib_d.a  $(LIB_PATH)/osShared_d.a \
                     $(LIB_PATH)/ioTest_d.a $(LIB_PATH)/ioData_d.a

  MFP_LIBS         = 

endif                 # -------------------------------------------------------


ASMS               = 

include objects.mk

include ../build.mk

# -------------------------------------------------------------------------------
