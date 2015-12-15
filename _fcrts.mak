
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

all: osShared vmKernel vmLib osKernel ioTest ioData fcrts

rebuild: osSharedCL vmKernelCL osKernelCL vmLibCL ioTestCL ioDataCL fcrtsCL osShared vmKernel osKernel vmLib ioTest ioData fcrts 

mostlyclean: osSharedML vmKernelML osKernelML vmLibML ioTestML ioDataML fcrtsML

clean: osSharedCL vmKernelCL osKernelCL vmLibCL ioTestCL ioDataCL fcrtsCL

clobber: osSharedCO vmKernelCO osKernelCO vmLibCO ioTestCO ioDataCO fcrtsCO


# Normal build
# -----------------------------------------------------------------------------

.PHONY : vmKernel
vmKernel:
	$(MAKE) -C $@ -f $@$(DEBREL).mak
  
.PHONY : osKernel
osKernel:
	$(MAKE) -C $@ -f $@$(DEBREL).mak

.PHONY : vmLib
vmLib:
	$(MAKE) -C $@ -f $@$(DEBREL).mak

.PHONY : fcrts
fcrts:
	$(MAKE) -C $@ -f $@$(DEBREL).mak

.PHONY : osShared
osShared:
	$(MAKE) -C $@ -f $@$(DEBREL).mak

.PHONY : ioTest
ioTest:
	$(MAKE) -C $@ -f $@$(DEBREL).mak

.PHONY : ioData
ioData:
	$(MAKE) -C $@ -f $@$(DEBREL).mak

# Mostly Clean (keep library and executables)
# -----------------------------------------------------------------------------

.PHONY : vmKernelML
vmKernelML:
	$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : vmLibML
vmLibML:
	$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : osKernelML
osKernelML:
	$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : fcrtsML
fcrtsML:
	$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : osSharedML
osSharedML:
	$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : ioTestML
ioTestML:
	$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : ioDataML
ioDataML:
	$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

# Clean
# -----------------------------------------------------------------------------

.PHONY : vmKernelCL
vmKernelCL:
	$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : osKernelCL
osKernelCL:
	$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : vmLibCL
vmLibCL:
	$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : fcrtsCL
fcrtsCL:
	$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : osSharedCL
osSharedCL:
	$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : ioTestCL
ioTestCL:
	$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : ioDataCL
ioDataCL:
	echo Cleaning $(subst CL,,$@)...
	$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

# Clobber
# -----------------------------------------------------------------------------

.PHONY : vmKernelCO
vmKernelCO:
	$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : osKernelCO
osKernelCO:
	$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : vmLibCO
vmLibCO:
	$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : fcrtsCO
fcrtsCO:
	$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : osSharedCO
osSharedCO:
	$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : ioTestCO
ioTestCO:
	$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : ioDataCO
ioDataCO:
	$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

# -------------------------------------------------------------------------------
