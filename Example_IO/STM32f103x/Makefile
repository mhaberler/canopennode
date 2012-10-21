# Optimization level, put in comment for debugging
OPTIMIZATION = -O1

# AT91 library directory
SYSLIB = $(SMT32_LIB)

TRACE_LEVEL=5

# Compile for all memories available on the board (this sets $(MEMORIES))
include $(SYSLIB)/boards/$(BOARD)/board.mak

# Output file basename
OUTPUT = start-$(BOARD)-$(CHIP)

# Output directories
BIN = bin
OBJ = obj


# Append OBJ and BIN directories to output filename
OUTPUT := $(BIN)/$(OUTPUT)

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------
INCL_APPL    = -I. -I..  
#-------------------------------------------------------------------------------
INCL_LIB     = -I$(CO_LIB) -I$(CO_LIB)/stm32f103x 
#-------------------------------------------------------------------------------
INCL_SYSLIB  = -I$(SYSLIB) -I$(SYSLIB)/cpus -I$(SYSLIB)/cpus/$(FAMILY) -I$(SYSLIB)/cpus/$(CHIP) 
INCL_SYSLIB += -I$(SYSLIB)/peripherals -I$(SYSLIB)/boards/$(BOARD) -I$(SYSLIB)/cpus/cmsis -I$(SYSLIB)/debug
#-------------------------------------------------------------------------------
INCLUDES  = $(INCL_APPL) $(INCL_LIB) $(INCL_SYSLIB)   
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#		Tools
#-------------------------------------------------------------------------------
# Tool suffix when cross-compiling
CROSS_COMPILE = arm-none-eabi-

# Compilation tools
CC = $(CROSS_COMPILE)gcc
SIZE = $(CROSS_COMPILE)size
STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy


CFLAGS =  -Wall -mlong-calls -ffunction-sections -fsigned-char $(CPUFLAGS) -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER
CFLAGS += -g $(OPTIMIZATION) $(INCLUDES) -D$(CHIP) -DTRACE_LEVEL=$(TRACE_LEVEL)
ASFLAGS = -g $(OPTIMIZATION) $(INCLUDES) $(CPUFLAGS) -D$(CHIP) -D__ASSEMBLY__
LDFLAGS = -g $(OPTIMIZATION) $(CPUFLAGS) -nostartfiles -Wl,--gc-sections,-Map,$(OUTPUT)-$$@.map 
# -nodefaultlibs

#-------------------------------------------------------------------------------
#		Files
#-------------------------------------------------------------------------------
VPATH += .. $(SYSLIB) $(SYSLIB)/cpus $(SYSLIB)/peripherals $(SYSLIB)/debug $(SYSLIB)/cpus/$(FAMILY) $(SYSLIB)/cpus/cmsis $(CO_LIB) $(CO_LIB)/stm32f103x ../src


C_OBJECTS_CANOPEN += CO_Emergency.o CO_HBconsumer.o CO_NMT_Heartbeat.o CO_PDO.o CO_SDO.o CO_SDOmaster.o CO_SYNC.o CO_driver.o eeprom.o  
C_SYSTEM = core_cm3.o system_stm32f10x.o stdio.o timer.o
C_PERIPHERY = stm32f10x_gpio.o  stm32f10x_rcc.o stm32f10x_can.o stm32f10x_usart.o
C_MAIN = main_STM32f103x.o co_od.o canopen.o syscalls.o dbgu.o misc.o

C_OBJECTS = $(C_OBJECTS_CANOPEN) $(C_MAIN) $(C_SYSTEM) $(C_PERIPHERY)

ASM_OBJECTS = startup_stm32f10x_md.o

#-------------------------------------------------------------------------------
#		Rules
#-------------------------------------------------------------------------------

all: $(BIN) $(OBJ) $(MEMORIES)

$(BIN) $(OBJ):
	mkdir $@

define RULES
C_OBJECTS_$(1) = $(addprefix $(OBJ)/$(1)_, $(C_OBJECTS))
ASM_OBJECTS_$(1) = $(addprefix $(OBJ)/$(1)_, $(ASM_OBJECTS))

$(1): $$(ASM_OBJECTS_$(1)) $$(C_OBJECTS_$(1))
	$(CC) $(LDFLAGS) -T"$(SYSLIB)/cpus/$(CHIP)/$$@_s.ld" -o $(OUTPUT)-$$@.elf $$^	
	$(OBJCOPY) -O binary $(OUTPUT)-$$@.elf $(OUTPUT)-$$@.bin
	$(SIZE) $$^ $(OUTPUT)-$$@.elf

$$(C_OBJECTS_$(1)): $(OBJ)/$(1)_%.o: %.c Makefile $(OBJ) $(BIN)
	$(CC) $(CFLAGS) -D$(1) -c -o $$@ $$<
#	$(CC) $(CFLAGS) -D$(1) -S -c $$<		# pro generovani asm listingu

$$(ASM_OBJECTS_$(1)): $(OBJ)/$(1)_%.o: %.S Makefile $(OBJ) $(BIN)
	$(CC) $(ASFLAGS) -D$(1) -c -o $$@ $$<

debug_$(1): $(1)
	perl ../resources/gdb/debug.pl $(OUTPUT)-$(1).elf

endef

$(foreach MEMORY, $(MEMORIES), $(eval $(call RULES,$(MEMORY))))

clean:
	-rm -f $(OBJ)/*.o $(BIN)/*.bin 


