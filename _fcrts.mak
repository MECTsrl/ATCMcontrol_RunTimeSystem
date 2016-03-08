
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
 # Filename: _fcrts.mak
 #
 # ------------------------------------------------------------------------------
 #


ifneq ($(DEBUG), 0)
DEBUG=1
endif

ifeq ($(GDB), 1)    # --- build with gdb symbols -------------------------------------------
GDBFLAG = -g
else                # --- build without gdb symbols -------------------------------------------
GDBFLAG =
endif               # -------------------------------------------------------

export DEBUG GDBFLAG

all: osShared vmKernel vmLib osKernel ioTest ioData CANopen fcrts

mostlyclean: osSharedML vmKernelML osKernelML vmLibML ioTestML ioDataML CANopenML fcrtsML

clean: osSharedCL vmKernelCL osKernelCL vmLibCL ioTestCL ioDataCL CANopenCL fcrtsCL

clobber: osSharedCO vmKernelCO osKernelCO vmLibCO ioTestCO ioDataCO CANopenCO fcrtsCO

rebuild: clean all

# Normal build
# -----------------------------------------------------------------------------

.PHONY : vmKernel
vmKernel:
	$(MAKE) -j1 -C $@ -f $@$(DEBREL).mak
  
.PHONY : osKernel
osKernel:
	$(MAKE) -j1 -C $@ -f $@$(DEBREL).mak

.PHONY : vmLib
vmLib:
	$(MAKE) -j1 -C $@ -f $@$(DEBREL).mak

.PHONY : fcrts
fcrts:
	$(MAKE) -j1 -C $@ -f $@$(DEBREL).mak

.PHONY : osShared
osShared:
	$(MAKE) -j1 -C $@ -f $@$(DEBREL).mak

.PHONY : ioTest
ioTest:
	$(MAKE) -j1 -C $@ -f $@$(DEBREL).mak

.PHONY : ioData
ioData:
	$(MAKE) -j1 -C $@ -f $@$(DEBREL).mak

.PHONY : CANopen
CANopen:
	$(MAKE) -j1 -C $@ -f $@$(DEBREL).mak

# Mostly Clean (keep library and executables)
# -----------------------------------------------------------------------------

.PHONY : vmKernelML
vmKernelML:
	$(MAKE) -j1 -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : vmLibML
vmLibML:
	$(MAKE) -j1 -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : osKernelML
osKernelML:
	$(MAKE) -j1 -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : fcrtsML
fcrtsML:
	$(MAKE) -j1 -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : osSharedML
osSharedML:
	$(MAKE) -j1 -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : ioTestML
ioTestML:
	$(MAKE) -j1 -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : ioDataML
ioDataML:
	$(MAKE) -j1 -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : CANopenML
CANopenML:
	$(MAKE) -j1 -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

# Clean
# -----------------------------------------------------------------------------

.PHONY : vmKernelCL
vmKernelCL:
	$(MAKE) -j1 -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : osKernelCL
osKernelCL:
	$(MAKE) -j1 -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : vmLibCL
vmLibCL:
	$(MAKE) -j1 -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : fcrtsCL
fcrtsCL:
	$(MAKE) -j1 -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : osSharedCL
osSharedCL:
	$(MAKE) -j1 -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : ioTestCL
ioTestCL:
	$(MAKE) -j1 -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : ioDataCL
ioDataCL:
	echo Cleaning $(subst CL,,$@)...
	$(MAKE) -j1 -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : CANopenCL
CANopenCL:
	echo Cleaning $(subst CL,,$@)...
	$(MAKE) -j1 -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

# Clobber
# -----------------------------------------------------------------------------

.PHONY : vmKernelCO
vmKernelCO:
	$(MAKE) -j1 -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : osKernelCO
osKernelCO:
	$(MAKE) -j1 -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : vmLibCO
vmLibCO:
	$(MAKE) -j1 -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : fcrtsCO
fcrtsCO:
	$(MAKE) -j1 -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : osSharedCO
osSharedCO:
	$(MAKE) -j1 -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : ioTestCO
ioTestCO:
	$(MAKE) -j1 -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : ioDataCO
ioDataCO:
	$(MAKE) -j1 -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : CANopenCO
CANopenCO:
	$(MAKE) -j1 -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

# -------------------------------------------------------------------------------
