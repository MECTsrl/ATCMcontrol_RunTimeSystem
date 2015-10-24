
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
 # Filename: build.mk
 #
 # ------------------------------------------------------------------------------
 #


DEPS = $(OBJS:.o=.d)


all: makedirs $(DEPS) $(LIB_NAME) $(BIN_NAME)


$(LIB_NAME): $(OBJS) $(ASMS) $(MAKS)
	echo  $@
	$(AR) $(AR_FLAGS) $@ $(OBJS) $(ASMS)

$(BIN_NAME): $(OBJS) $(ASMS) $(WA_LIBS) $(LIBS) $(MFP_LIBS) $(MAKS)
	echo $@
	$(LD) -Wl,-Map,$(BIN_NAME).map $(GDBFLAG) $(LD_FLAGS) $(OBJS) -Wl,-whole-archive $(WA_LIBS) -Wl,-no-whole-archive $(LIBS) $(MFP_LIBS) $(ASMS) -o $@

$(OBJ_PATH)/%.d: %.c $(MAKS)
	@#echo $(*F).d
	-$(CRD) $(OBJ_PATH)
	$(CC) $(GDBFLAG) $(OFLAGS) $(CFLAGS) -MM -c $< -o $@
	@#$(CC) $(GDBFLAG) $(OFLAGS) $(CFLAGS) -MM -c $< > $@ 2> $(@D)/$(*F).e
	@#cat $(@D)/$(*F).e 
	@#$(G2D) $(@D)/$(*F).e $(SRC_PATH)
	$(GD) $@

$(OBJ_PATH)/%.o: %.c $(MAKS)
	@#echo $(*F).o
	-$(CRD) $(OBJ_PATH)
	$(CC) $(GDBFLAG) $(OFLAGS) $(CFLAGS) -c $< -o $@
	@#$(CC) $(GDBFLAG) $(OFLAGS) $(CFLAGS) -c $< -o $@ 2> $(@D)/$(*F).e
	@#cat $(@D)/$(*F).e 
	@#$(G2D) $(@D)/$(*F).e $(SRC_PATH)

$(OBJ_PATH)/%.d: %.cpp $(MAKS)
	@#echo $(*F).d
	-$(CRD) $(OBJ_PATH)
	$(CC) $(GDBFLAG) $(OFLAGS) $(CFLAGS) -MM -c $< -o $@
	@#$(CC) $(GDBFLAG) $(OFLAGS) $(CFLAGS) -MM -c $< > $@ 2> $(@D)/$(*F).e
	@#cat $(@D)/$(*F).e 
	@#$(G2D) $(@D)/$(*F).e $(SRC_PATH)
	$(GD) $@

$(OBJ_PATH)/%.o: %.cpp $(MAKS)
	@#echo $(*F).o
	-$(CRD) $(OBJ_PATH)
	$(CC) $(GDBFLAG) $(OFLAGS) $(CFLAGS) -c $< -o $@
	@#$(CC) $(GDBFLAG) $(OFLAGS) $(CFLAGS) -c $< -o $@ 2> $(@D)/$(*F).e
	@#cat $(@D)/$(*F).e 
	@#$(G2D) $(@D)/$(*F).e $(SRC_PATH)

$(OBJ_PATH)/%.o: %.S $(MAKS)
	@#echo $(*F).o
	-$(CRD) $(OBJ_PATH)
	$(AS) $(GDBFLAG) $(OFLAGS) $(CFLAGS_AS) -c $< -o $@
	@#$(AS) $(GDBFLAG) $(OFLAGS) $(CFLAGS_AS) -c $< -o $@ 2> $(@D)/$(*F).e
	@#cat $(@D)/$(*F).e 
	@#$(G2D) $(@D)/$(*F).e $(SRC_PATH)


makedirs:
	-$(CRD) $(OBJ_PATH)
	-$(CRD) $(BIN_PATH) 
	-$(CRD) $(LIB_PATH) 

mostlyclean:
	$(RM) -f $(OBJ_PATH)/*.o 
	$(RM) -f $(OBJ_PATH)/*.e
	$(RM) -f $(OBJ_PATH)/*.d
	$(RM) -f *.BAK
	$(RM) -f *.bak

clean: mostlyclean
	-$(RM) -f $(OBJ_PATH)/*.o 
	-$(RM) -f $(OBJ_PATH)/*.e
	-$(RM) -f $(OBJ_PATH)/*.d
ifneq ($(LIB_NAME), )
	$(RM) -f $(LIB_NAME)
endif
ifneq ($(BIN_NAME), )
	$(RM) -f $(BIN_NAME)
	$(RM) -f $(BIN_NAME).map
endif

clobber: clean
	-$(DED) $(OBJ_PATH) 
	-$(DED) $(BIN_PATH) 
	-$(DED) $(LIB_PATH) 


-include $(DEPS)

# -------------------------------------------------------------------------------
