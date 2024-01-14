CC = gcc
LINK = gcc
CFLAGS = -march=rv64imafd -mabi=lp64d
LFLAGS = 

TARGET = DeviceControl
SOURCE_CODE = .
OUT_DIR = out
DEP_DIR = dep
OBJ_DIR = obj
RM = rm
ECHO = echo

$(shell mkdir -p $(OUT_DIR))
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(DEP_DIR))

C_SOURCE +=  \
$(wildcard *.c)

$(info $(C_SOURCE))
# compiler objects
OBJECTS  = $(addprefix $(OBJ_DIR)/,$(notdir $(C_SOURCE:.c=.o)))
 vpath %.c $(sort $(dir $(C_SOURCE)))

DEPS = $(addprefix $(DEP_DIR)/,$(notdir $(C_SOURCE:.c=.d)))
 vpath %.c $(sort $(dir $(C_SOURCE)))

$(info $(OBJECTS))
$(info $(DEPS))

$(OUT_DIR)/$(TARGET).out: $(OBJECTS)
	$(LINK) $(OBJECTS) $(LFLAGS) -o $@ -Wl,-M=$(OUT_DIR)/$(TARGET).map

-include $(DEPS)

# compiler process
$(OBJ_DIR)/%.o : %.d  
$(OBJ_DIR)/%.o : %.c  Makefile 
	$(ECHO)  'Compiling' $<
	$(CC) -c  $(CFLAGS) $< -o $@ 

$(DEP_DIR)/%.d : OBJ_BNAME=$(basename $(notdir $@))
$(DEP_DIR)/%.d : %.c  Makefile 
	$(ECHO) "Update dependency Compiling" $<
	$(CC) -M   $(CFLAGS) $< -o "$(OBJ_DIR)/$(OBJ_BNAME).o $(DEP_DIR)/$(OBJ_BNAME).d"  >  $@
	$(CC) -c   $(CFLAGS) $< -o $(OBJ_DIR)/$(OBJ_BNAME).o

.PHONY : clean
clean:
	$(RM) $(DEP_DIR)/*
	$(RM) $(OBJ_DIR)/*
	$(RM) $(OUT_DIR)/*
	