
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

GDBFLAG = -g

all: osShared vmKernel vmLib osKernel ioTest ioCANopen ioUDP ioKeypad ioData ioSyncro ioModbusTCPS ioModbusRTUC fcrts

rebuild: osSharedCL vmKernelCL osKernelCL vmLibCL ioTestCL ioCANopenCL ioUDPCL ioKeypadCL ioDataCL ioSyncroCL ioModbusTCPSCL ioModbusRTUCCL fcrtsCL osShared vmKernel osKernel vmLib ioTest ioCANopen ioKeypad ioUDP ioData ioSyncro ioModbusTCPS ioModbusRTUC fcrts

mostlyclean: osSharedML vmKernelML osKernelML vmLibML ioTestML ioCANopenML ioUDPML ioKeypadML ioDataML ioSyncroML ioModbusTCPSML ioModbusRTUCML fcrtsML

clean: osSharedCL vmKernelCL osKernelCL vmLibCL ioTestCL ioCANopenCL ioUDPCL ioKeypadCL ioDataCL ioSyncroCL ioModbusTCPSCL ioModbusRTUCCL fcrtsCL

clobber: osSharedCO vmKernelCO osKernelCO vmLibCO ioTestCO ioCANopenCO ioUDPCO ioKeypadCO ioDataCO ioSyncroCO ioModbusTCPSCO ioModbusRTUCCO fcrtsCO


# Normal build
# -----------------------------------------------------------------------------

.PHONY : vmKernel
vmKernel:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s
  
.PHONY : osKernel
osKernel:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s

.PHONY : vmLib
vmLib:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s

.PHONY : fcrts
fcrts:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s

.PHONY : osShared
osShared:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s

.PHONY : ioTest
ioTest:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s

.PHONY : ioCANopen
ioCANopen:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s

.PHONY : ioUDP
ioUDP:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s

.PHONY : ioKeypad
ioKeypad:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s

.PHONY : ioData
ioData:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s

.PHONY : ioSyncro
ioSyncro:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s

.PHONY : ioModbusTCPS
ioModbusTCPS:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s

.PHONY : ioModbusRTUC
ioModbusRTUC:
	@echo Building $@...
	@$(MAKE) -C $@ -f $@$(DEBREL).mak -s

# Mostly Clean (keep library and executables)
# -----------------------------------------------------------------------------

.PHONY : vmKernelML
vmKernelML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

.PHONY : vmLibML
vmLibML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

.PHONY : osKernelML
osKernelML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

.PHONY : fcrtsML
fcrtsML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

.PHONY : osSharedML
osSharedML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

.PHONY : ioTestML
ioTestML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

.PHONY : ioCANopenML
ioCANopenML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

.PHONY : ioUDPML
ioUDPML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

.PHONY : ioKeypadML
ioKeypadML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

.PHONY : ioDataML
ioDataML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

.PHONY : ioSyncroML
ioSyncroML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

.PHONY : ioModbusTCPSML
ioModbusTCPSML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

.PHONY : ioModbusRTUCML
ioModbusRTUCML:
	@echo Cleaning $(subst ML,,$@)...
	@$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean -s

# Clean
# -----------------------------------------------------------------------------

.PHONY : vmKernelCL
vmKernelCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

.PHONY : osKernelCL
osKernelCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

.PHONY : vmLibCL
vmLibCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

.PHONY : fcrtsCL
fcrtsCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

.PHONY : osSharedCL
osSharedCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

.PHONY : ioTestCL
ioTestCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

.PHONY : ioCANopenCL
ioCANopenCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

.PHONY : ioUDPCL
ioUDPCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

.PHONY : ioKeypadCL
ioKeypadCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

.PHONY : ioDataCL
ioDataCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

.PHONY : ioSyncroCL
ioSyncroCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

.PHONY : ioModbusTCPSCL
ioModbusTCPSCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

.PHONY : ioModbusRTUCCL
ioModbusRTUCCL:
	@echo Cleaning $(subst CL,,$@)...
	@$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean -s

# Clobber
# -----------------------------------------------------------------------------

.PHONY : vmKernelCO
vmKernelCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

.PHONY : osKernelCO
osKernelCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

.PHONY : vmLibCO
vmLibCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

.PHONY : fcrtsCO
fcrtsCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

.PHONY : osSharedCO
osSharedCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

.PHONY : ioTestCO
ioTestCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

.PHONY : ioCANopenCO
ioCANopenCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

.PHONY : ioUDPCO
ioUDPCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

.PHONY : ioKeypadCO
ioKeypadCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

.PHONY : ioDataCO
ioDataCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

.PHONY : ioSyncroCO
ioSyncroCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

.PHONY : ioModbusTCPSCO
ioModbusTCPSCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

.PHONY : ioModbusRTUCCO
ioModbusRTUCCO:
	@echo Cleaning $(subst CO,,$@)...
	@$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber -s

# -------------------------------------------------------------------------------
