# Mostly written by Jonathan Larmour, Red Hat, Inc.
# Reference to ecos.mak added by John Dallaway, eCosCentric Limited, 2003-01-20
# This file is in the public domain and may be used for any purpose

# path to CANopen node source code
CANOPENNODE_SRC =  ../CANopen_stack


# Include directories
INCLUDE_DIRS = $(CANOPENNODE_SRC) \
	-I.

# source files
SOURCES = $(CANOPENNODE_SRC)/CANopen.c \
	$(CANOPENNODE_SRC)/CO_Emergency.c \
	$(CANOPENNODE_SRC)/CO_HBconsumer.c \
	$(CANOPENNODE_SRC)/CO_NMT_Heartbeat.c \
	$(CANOPENNODE_SRC)/CO_PDO.c \
	$(CANOPENNODE_SRC)/CO_SDO.c \
	$(CANOPENNODE_SRC)/CO_SDOmaster.c \
	$(CANOPENNODE_SRC)/CO_SYNC.c \
	$(CANOPENNODE_SRC)/crc16-ccitt.c \
	CO_driver.c \
	CO_OD.c \
	main_socketcan.c \
	app_socketcan.c \
	eeprom.c

OBJSC=${SOURCES:%.c=%.o}
OBJS=${OBJSC:%.cpp=%.o}

CFLAGS        = -g -I$(INCLUDE_DIRS)
LDFLAGS       = -g

# RULES

.PHONY: all clean

all: scan_canopennode

%.o: %.c
	$(CC) $(CFLAGS) -c -o $*.o $<


scan_canopennode: $(OBJS)
	$(CC) $(LDFLAGS)  $(OBJS) -o $@

clean:
	rm -f $(OBJS) scan_canopennode

tags:
	etags $(SOURCES) *.h $(CANOPENNODE_SRC)/*.h