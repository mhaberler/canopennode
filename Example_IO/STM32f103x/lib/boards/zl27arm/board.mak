# Defines which are the available memory targets for the AT91SAM7X-EK board.
# flash sram
MEMORIES = flash 


# Chip used for compilation
CHIP  = stm32f103vbt6

FAMILY = stm32f10x

# Compilation Flags
CPUFLAGS = -mcpu=cortex-m3 -mthumb
