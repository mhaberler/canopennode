#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile

# Environment
SHELL=cmd.exe
# Adding MPLAB X bin directory to path
PATH:=C:/Program Files (x86)/Microchip/MPLABX/mplab_ide/mplab_ide/modules/../../bin/:$(PATH)
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/dsPIC30F.X.${IMAGE_TYPE}.out
else
IMAGE_TYPE=production
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/dsPIC30F.X.${IMAGE_TYPE}.out
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/CANopen.o ${OBJECTDIR}/_ext/1472/CO_OD.o ${OBJECTDIR}/_ext/1472/main_dsPIC30F.o ${OBJECTDIR}/_ext/287056730/CO_driver.o ${OBJECTDIR}/_ext/287056730/eeprom.o ${OBJECTDIR}/_ext/287056730/memcpyram2flash.o ${OBJECTDIR}/_ext/581370467/CO_Emergency.o ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o ${OBJECTDIR}/_ext/581370467/CO_PDO.o ${OBJECTDIR}/_ext/581370467/CO_SDO.o ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o ${OBJECTDIR}/_ext/581370467/CO_SYNC.o

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/CANopen.o ${OBJECTDIR}/_ext/1472/CO_OD.o ${OBJECTDIR}/_ext/1472/main_dsPIC30F.o ${OBJECTDIR}/_ext/287056730/CO_driver.o ${OBJECTDIR}/_ext/287056730/eeprom.o ${OBJECTDIR}/_ext/287056730/memcpyram2flash.o ${OBJECTDIR}/_ext/581370467/CO_Emergency.o ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o ${OBJECTDIR}/_ext/581370467/CO_PDO.o ${OBJECTDIR}/_ext/581370467/CO_SDO.o ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o ${OBJECTDIR}/_ext/581370467/CO_SYNC.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

# Path to java used to run MPLAB X when this makefile was created
MP_JAVA_PATH="C:\Program Files (x86)\Java\jre6/bin/"
OS_CURRENT="$(shell uname -s)"
############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
MP_CC="C:\Program Files (x86)\Microchip\MPLAB C30\bin\pic30-gcc.exe"
# MP_BC is not defined
MP_AS="C:\Program Files (x86)\Microchip\MPLAB C30\bin\pic30-as.exe"
MP_LD="C:\Program Files (x86)\Microchip\MPLAB C30\bin\pic30-ld.exe"
MP_AR="C:\Program Files (x86)\Microchip\MPLAB C30\bin\pic30-ar.exe"
DEP_GEN=${MP_JAVA_PATH}java -jar "C:/Program Files (x86)/Microchip/MPLABX/mplab_ide/mplab_ide/modules/../../bin/extractobjectdependencies.jar" 
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps
MP_CC_DIR="C:\Program Files (x86)\Microchip\MPLAB C30\bin"
# MP_BC_DIR is not defined
MP_AS_DIR="C:\Program Files (x86)\Microchip\MPLAB C30\bin"
MP_LD_DIR="C:\Program Files (x86)\Microchip\MPLAB C30\bin"
MP_AR_DIR="C:\Program Files (x86)\Microchip\MPLAB C30\bin"
# MP_BC_DIR is not defined

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/dsPIC30F.X.${IMAGE_TYPE}.out

MP_PROCESSOR_OPTION=30F4011
MP_LINKER_FILE_OPTION=,-Tp30F4011.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/581370467/CO_SYNC.o: ../../CANopen_stack/CO_SYNC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SYNC.o.ok ${OBJECTDIR}/_ext/581370467/CO_SYNC.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_SYNC.o ../../CANopen_stack/CO_SYNC.c  
	
${OBJECTDIR}/_ext/581370467/CO_PDO.o: ../../CANopen_stack/CO_PDO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_PDO.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_PDO.o.ok ${OBJECTDIR}/_ext/581370467/CO_PDO.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_PDO.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_PDO.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_PDO.o ../../CANopen_stack/CO_PDO.c  
	
${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o: ../../CANopen_stack/CO_HBconsumer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.ok ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o ../../CANopen_stack/CO_HBconsumer.c  
	
${OBJECTDIR}/_ext/287056730/eeprom.o: ../../CANopen_stack/dsPIC30F/eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/287056730 
	@${RM} ${OBJECTDIR}/_ext/287056730/eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/287056730/eeprom.o.ok ${OBJECTDIR}/_ext/287056730/eeprom.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/287056730/eeprom.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/287056730/eeprom.o.d" -o ${OBJECTDIR}/_ext/287056730/eeprom.o ../../CANopen_stack/dsPIC30F/eeprom.c  
	
${OBJECTDIR}/_ext/581370467/CO_Emergency.o: ../../CANopen_stack/CO_Emergency.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_Emergency.o.ok ${OBJECTDIR}/_ext/581370467/CO_Emergency.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_Emergency.o ../../CANopen_stack/CO_Emergency.c  
	
${OBJECTDIR}/_ext/581370467/CO_SDO.o: ../../CANopen_stack/CO_SDO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SDO.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SDO.o.ok ${OBJECTDIR}/_ext/581370467/CO_SDO.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_SDO.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_SDO.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_SDO.o ../../CANopen_stack/CO_SDO.c  
	
${OBJECTDIR}/_ext/1472/CO_OD.o: ../CO_OD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/CO_OD.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/CO_OD.o.ok ${OBJECTDIR}/_ext/1472/CO_OD.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/CO_OD.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/1472/CO_OD.o.d" -o ${OBJECTDIR}/_ext/1472/CO_OD.o ../CO_OD.c  
	
${OBJECTDIR}/_ext/287056730/CO_driver.o: ../../CANopen_stack/dsPIC30F/CO_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/287056730 
	@${RM} ${OBJECTDIR}/_ext/287056730/CO_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/287056730/CO_driver.o.ok ${OBJECTDIR}/_ext/287056730/CO_driver.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/287056730/CO_driver.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/287056730/CO_driver.o.d" -o ${OBJECTDIR}/_ext/287056730/CO_driver.o ../../CANopen_stack/dsPIC30F/CO_driver.c  
	
${OBJECTDIR}/_ext/287056730/memcpyram2flash.o: ../../CANopen_stack/dsPIC30F/memcpyram2flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/287056730 
	@${RM} ${OBJECTDIR}/_ext/287056730/memcpyram2flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/287056730/memcpyram2flash.o.ok ${OBJECTDIR}/_ext/287056730/memcpyram2flash.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/287056730/memcpyram2flash.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/287056730/memcpyram2flash.o.d" -o ${OBJECTDIR}/_ext/287056730/memcpyram2flash.o ../../CANopen_stack/dsPIC30F/memcpyram2flash.c  
	
${OBJECTDIR}/_ext/1472/main_dsPIC30F.o: ../main_dsPIC30F.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main_dsPIC30F.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main_dsPIC30F.o.ok ${OBJECTDIR}/_ext/1472/main_dsPIC30F.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main_dsPIC30F.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/1472/main_dsPIC30F.o.d" -o ${OBJECTDIR}/_ext/1472/main_dsPIC30F.o ../main_dsPIC30F.c  
	
${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o: ../../CANopen_stack/CO_NMT_Heartbeat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.ok ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o ../../CANopen_stack/CO_NMT_Heartbeat.c  
	
${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o: ../../CANopen_stack/CO_SDOmaster.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.ok ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o ../../CANopen_stack/CO_SDOmaster.c  
	
${OBJECTDIR}/_ext/1472/CANopen.o: ../CANopen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/CANopen.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/CANopen.o.ok ${OBJECTDIR}/_ext/1472/CANopen.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/CANopen.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/1472/CANopen.o.d" -o ${OBJECTDIR}/_ext/1472/CANopen.o ../CANopen.c  
	
else
${OBJECTDIR}/_ext/581370467/CO_SYNC.o: ../../CANopen_stack/CO_SYNC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SYNC.o.ok ${OBJECTDIR}/_ext/581370467/CO_SYNC.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_SYNC.o ../../CANopen_stack/CO_SYNC.c  
	
${OBJECTDIR}/_ext/581370467/CO_PDO.o: ../../CANopen_stack/CO_PDO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_PDO.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_PDO.o.ok ${OBJECTDIR}/_ext/581370467/CO_PDO.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_PDO.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_PDO.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_PDO.o ../../CANopen_stack/CO_PDO.c  
	
${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o: ../../CANopen_stack/CO_HBconsumer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.ok ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o ../../CANopen_stack/CO_HBconsumer.c  
	
${OBJECTDIR}/_ext/287056730/eeprom.o: ../../CANopen_stack/dsPIC30F/eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/287056730 
	@${RM} ${OBJECTDIR}/_ext/287056730/eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/287056730/eeprom.o.ok ${OBJECTDIR}/_ext/287056730/eeprom.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/287056730/eeprom.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/287056730/eeprom.o.d" -o ${OBJECTDIR}/_ext/287056730/eeprom.o ../../CANopen_stack/dsPIC30F/eeprom.c  
	
${OBJECTDIR}/_ext/581370467/CO_Emergency.o: ../../CANopen_stack/CO_Emergency.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_Emergency.o.ok ${OBJECTDIR}/_ext/581370467/CO_Emergency.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_Emergency.o ../../CANopen_stack/CO_Emergency.c  
	
${OBJECTDIR}/_ext/581370467/CO_SDO.o: ../../CANopen_stack/CO_SDO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SDO.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SDO.o.ok ${OBJECTDIR}/_ext/581370467/CO_SDO.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_SDO.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_SDO.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_SDO.o ../../CANopen_stack/CO_SDO.c  
	
${OBJECTDIR}/_ext/1472/CO_OD.o: ../CO_OD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/CO_OD.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/CO_OD.o.ok ${OBJECTDIR}/_ext/1472/CO_OD.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/CO_OD.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/1472/CO_OD.o.d" -o ${OBJECTDIR}/_ext/1472/CO_OD.o ../CO_OD.c  
	
${OBJECTDIR}/_ext/287056730/CO_driver.o: ../../CANopen_stack/dsPIC30F/CO_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/287056730 
	@${RM} ${OBJECTDIR}/_ext/287056730/CO_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/287056730/CO_driver.o.ok ${OBJECTDIR}/_ext/287056730/CO_driver.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/287056730/CO_driver.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/287056730/CO_driver.o.d" -o ${OBJECTDIR}/_ext/287056730/CO_driver.o ../../CANopen_stack/dsPIC30F/CO_driver.c  
	
${OBJECTDIR}/_ext/287056730/memcpyram2flash.o: ../../CANopen_stack/dsPIC30F/memcpyram2flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/287056730 
	@${RM} ${OBJECTDIR}/_ext/287056730/memcpyram2flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/287056730/memcpyram2flash.o.ok ${OBJECTDIR}/_ext/287056730/memcpyram2flash.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/287056730/memcpyram2flash.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/287056730/memcpyram2flash.o.d" -o ${OBJECTDIR}/_ext/287056730/memcpyram2flash.o ../../CANopen_stack/dsPIC30F/memcpyram2flash.c  
	
${OBJECTDIR}/_ext/1472/main_dsPIC30F.o: ../main_dsPIC30F.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main_dsPIC30F.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main_dsPIC30F.o.ok ${OBJECTDIR}/_ext/1472/main_dsPIC30F.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main_dsPIC30F.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/1472/main_dsPIC30F.o.d" -o ${OBJECTDIR}/_ext/1472/main_dsPIC30F.o ../main_dsPIC30F.c  
	
${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o: ../../CANopen_stack/CO_NMT_Heartbeat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.ok ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o ../../CANopen_stack/CO_NMT_Heartbeat.c  
	
${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o: ../../CANopen_stack/CO_SDOmaster.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.ok ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o ../../CANopen_stack/CO_SDOmaster.c  
	
${OBJECTDIR}/_ext/1472/CANopen.o: ../CANopen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/CANopen.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/CANopen.o.ok ${OBJECTDIR}/_ext/1472/CANopen.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/CANopen.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"../../CANopen_stack" -I"../../CANopen_stack/dsPIC30F" -I".." -O1 -MMD -MF "${OBJECTDIR}/_ext/1472/CANopen.o.d" -o ${OBJECTDIR}/_ext/1472/CANopen.o ../CANopen.c  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/dsPIC30F.X.${IMAGE_TYPE}.out: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/dsPIC30F.X.${IMAGE_TYPE}.out ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1,--heap=1200,-L"..",-Map="${DISTDIR}/dsPIC30F.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_REAL_ICE=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/dsPIC30F.X.${IMAGE_TYPE}.out: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/dsPIC30F.X.${IMAGE_TYPE}.out ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1,--heap=1200,-L"..",-Map="${DISTDIR}/dsPIC30F.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/dsPIC30F.X.${IMAGE_TYPE}.out -omf=elf
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard $(addsuffix .d, ${OBJECTFILES}))
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
