
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
 # Filename: _vistest.mak
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

all: visLib osShared vistest

rebuild: visLibCL osSharedCL vistestCL visLib osShared vistest

mostlyclean: visLibML osSharedML vistestML

clean: visLibCL osSharedCL vistestCL

clobber: visLibCO osSharedCO vistestCO


# Normal build
# -----------------------------------------------------------------------------

.PHONY : visLib
visLib:
	@#echo Building $@...
	$(MAKE) -C $@ -f $@$(DEBREL).mak DEBUG=$(DEBUG)
  
.PHONY : osShared
osShared:
	@#echo Building $@...
	$(MAKE) -C $@ -f $@$(DEBREL).mak DEBUG=$(DEBUG)
  
.PHONY : vistest
vistest:
	@#echo Building $@...
	$(MAKE) -C $@ -f $@$(DEBREL).mak



# Mostly Clean (keep library and executables)
# -----------------------------------------------------------------------------

.PHONY : visLibML
visLibML:
	@#echo Cleaning $(subst ML,,$@)...
	$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : osSharedML
osSharedML:
	@#echo Cleaning $(subst ML,,$@)...
	$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean

.PHONY : vistestML
vistestML:
	@#echo Cleaning $(subst ML,,$@)...
	$(MAKE) -C $(subst ML,,$@) -f $(subst ML,,$@)$(DEBREL).mak mostlyclean


# Clean
# -----------------------------------------------------------------------------

.PHONY : visLibCL
visLibCL:
	@#echo Cleaning $(subst CL,,$@)...
	$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : osSharedCL
osSharedCL:
	@#echo Cleaning $(subst CL,,$@)...
	$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean

.PHONY : vistestCL
vistestCL:
	@#echo Cleaning $(subst CL,,$@)...
	$(MAKE) -C $(subst CL,,$@) -f $(subst CL,,$@)$(DEBREL).mak clean



# Clobber
# -----------------------------------------------------------------------------

.PHONY : visLibCO
visLibCO:
	@#echo Cleaning $(subst CO,,$@)...
	$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : osSharedCO
osSharedCO:
	@#echo Cleaning $(subst CO,,$@)...
	$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber

.PHONY : vistestCO
vistestCO:
	@echo Cleaning $(subst CO,,$@)...
	$(MAKE) -C $(subst CO,,$@) -f $(subst CO,,$@)$(DEBREL).mak clobber



# -------------------------------------------------------------------------------
