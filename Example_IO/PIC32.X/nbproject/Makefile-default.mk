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
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/PIC32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/PIC32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/CANopen.o ${OBJECTDIR}/_ext/581370467/CO_Emergency.o ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o ${OBJECTDIR}/_ext/1472/CO_OD.o ${OBJECTDIR}/_ext/581370467/CO_PDO.o ${OBJECTDIR}/_ext/581370467/CO_SDO.o ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o ${OBJECTDIR}/_ext/581370467/CO_SYNC.o ${OBJECTDIR}/_ext/542917987/CO_driver.o ${OBJECTDIR}/_ext/542917987/eeprom.o ${OBJECTDIR}/_ext/1472/main_PIC32.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/CANopen.o.d ${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d ${OBJECTDIR}/_ext/1472/CO_OD.o.d ${OBJECTDIR}/_ext/581370467/CO_PDO.o.d ${OBJECTDIR}/_ext/581370467/CO_SDO.o.d ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d ${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d ${OBJECTDIR}/_ext/542917987/CO_driver.o.d ${OBJECTDIR}/_ext/542917987/eeprom.o.d ${OBJECTDIR}/_ext/1472/main_PIC32.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/CANopen.o ${OBJECTDIR}/_ext/581370467/CO_Emergency.o ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o ${OBJECTDIR}/_ext/1472/CO_OD.o ${OBJECTDIR}/_ext/581370467/CO_PDO.o ${OBJECTDIR}/_ext/581370467/CO_SDO.o ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o ${OBJECTDIR}/_ext/581370467/CO_SYNC.o ${OBJECTDIR}/_ext/542917987/CO_driver.o ${OBJECTDIR}/_ext/542917987/eeprom.o ${OBJECTDIR}/_ext/1472/main_PIC32.o


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
MP_CC="C:\Program Files (x86)\Microchip\mplabc32\v2.02\bin\pic32-gcc.exe"
# MP_BC is not defined
MP_AS="C:\Program Files (x86)\Microchip\mplabc32\v2.02\bin\pic32-as.exe"
MP_LD="C:\Program Files (x86)\Microchip\mplabc32\v2.02\bin\pic32-ld.exe"
MP_AR="C:\Program Files (x86)\Microchip\mplabc32\v2.02\bin\pic32-ar.exe"
DEP_GEN=${MP_JAVA_PATH}java -jar "C:/Program Files (x86)/Microchip/MPLABX/mplab_ide/mplab_ide/modules/../../bin/extractobjectdependencies.jar" 
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps
MP_CC_DIR="C:\Program Files (x86)\Microchip\mplabc32\v2.02\bin"
# MP_BC_DIR is not defined
MP_AS_DIR="C:\Program Files (x86)\Microchip\mplabc32\v2.02\bin"
MP_LD_DIR="C:\Program Files (x86)\Microchip\mplabc32\v2.02\bin"
MP_AR_DIR="C:\Program Files (x86)\Microchip\mplabc32\v2.02\bin"
# MP_BC_DIR is not defined

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/PIC32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX775F256L
MP_LINKER_FILE_OPTION=
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
${OBJECTDIR}/_ext/1472/CANopen.o: ../CANopen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/CANopen.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/CANopen.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/CANopen.o.d" -o ${OBJECTDIR}/_ext/1472/CANopen.o ../CANopen.c  
	
${OBJECTDIR}/_ext/581370467/CO_Emergency.o: ../../CANopen_stack/CO_Emergency.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_Emergency.o ../../CANopen_stack/CO_Emergency.c  
	
${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o: ../../CANopen_stack/CO_HBconsumer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o ../../CANopen_stack/CO_HBconsumer.c  
	
${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o: ../../CANopen_stack/CO_NMT_Heartbeat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o ../../CANopen_stack/CO_NMT_Heartbeat.c  
	
${OBJECTDIR}/_ext/1472/CO_OD.o: ../CO_OD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/CO_OD.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/CO_OD.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/CO_OD.o.d" -o ${OBJECTDIR}/_ext/1472/CO_OD.o ../CO_OD.c  
	
${OBJECTDIR}/_ext/581370467/CO_PDO.o: ../../CANopen_stack/CO_PDO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_PDO.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_PDO.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_PDO.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_PDO.o ../../CANopen_stack/CO_PDO.c  
	
${OBJECTDIR}/_ext/581370467/CO_SDO.o: ../../CANopen_stack/CO_SDO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SDO.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_SDO.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_SDO.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_SDO.o ../../CANopen_stack/CO_SDO.c  
	
${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o: ../../CANopen_stack/CO_SDOmaster.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o ../../CANopen_stack/CO_SDOmaster.c  
	
${OBJECTDIR}/_ext/581370467/CO_SYNC.o: ../../CANopen_stack/CO_SYNC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_SYNC.o ../../CANopen_stack/CO_SYNC.c  
	
${OBJECTDIR}/_ext/542917987/CO_driver.o: ../../CANopen_stack/PIC32/CO_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/542917987 
	@${RM} ${OBJECTDIR}/_ext/542917987/CO_driver.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/542917987/CO_driver.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/542917987/CO_driver.o.d" -o ${OBJECTDIR}/_ext/542917987/CO_driver.o ../../CANopen_stack/PIC32/CO_driver.c  
	
${OBJECTDIR}/_ext/542917987/eeprom.o: ../../CANopen_stack/PIC32/eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/542917987 
	@${RM} ${OBJECTDIR}/_ext/542917987/eeprom.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/542917987/eeprom.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/542917987/eeprom.o.d" -o ${OBJECTDIR}/_ext/542917987/eeprom.o ../../CANopen_stack/PIC32/eeprom.c  
	
${OBJECTDIR}/_ext/1472/main_PIC32.o: ../main_PIC32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main_PIC32.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main_PIC32.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/main_PIC32.o.d" -o ${OBJECTDIR}/_ext/1472/main_PIC32.o ../main_PIC32.c  
	
else
${OBJECTDIR}/_ext/1472/CANopen.o: ../CANopen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/CANopen.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/CANopen.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/CANopen.o.d" -o ${OBJECTDIR}/_ext/1472/CANopen.o ../CANopen.c  
	
${OBJECTDIR}/_ext/581370467/CO_Emergency.o: ../../CANopen_stack/CO_Emergency.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_Emergency.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_Emergency.o ../../CANopen_stack/CO_Emergency.c  
	
${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o: ../../CANopen_stack/CO_HBconsumer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_HBconsumer.o ../../CANopen_stack/CO_HBconsumer.c  
	
${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o: ../../CANopen_stack/CO_NMT_Heartbeat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_NMT_Heartbeat.o ../../CANopen_stack/CO_NMT_Heartbeat.c  
	
${OBJECTDIR}/_ext/1472/CO_OD.o: ../CO_OD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/CO_OD.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/CO_OD.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/CO_OD.o.d" -o ${OBJECTDIR}/_ext/1472/CO_OD.o ../CO_OD.c  
	
${OBJECTDIR}/_ext/581370467/CO_PDO.o: ../../CANopen_stack/CO_PDO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_PDO.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_PDO.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_PDO.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_PDO.o ../../CANopen_stack/CO_PDO.c  
	
${OBJECTDIR}/_ext/581370467/CO_SDO.o: ../../CANopen_stack/CO_SDO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SDO.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_SDO.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_SDO.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_SDO.o ../../CANopen_stack/CO_SDO.c  
	
${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o: ../../CANopen_stack/CO_SDOmaster.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_SDOmaster.o ../../CANopen_stack/CO_SDOmaster.c  
	
${OBJECTDIR}/_ext/581370467/CO_SYNC.o: ../../CANopen_stack/CO_SYNC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/581370467 
	@${RM} ${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/581370467/CO_SYNC.o.d" -o ${OBJECTDIR}/_ext/581370467/CO_SYNC.o ../../CANopen_stack/CO_SYNC.c  
	
${OBJECTDIR}/_ext/542917987/CO_driver.o: ../../CANopen_stack/PIC32/CO_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/542917987 
	@${RM} ${OBJECTDIR}/_ext/542917987/CO_driver.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/542917987/CO_driver.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/542917987/CO_driver.o.d" -o ${OBJECTDIR}/_ext/542917987/CO_driver.o ../../CANopen_stack/PIC32/CO_driver.c  
	
${OBJECTDIR}/_ext/542917987/eeprom.o: ../../CANopen_stack/PIC32/eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/542917987 
	@${RM} ${OBJECTDIR}/_ext/542917987/eeprom.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/542917987/eeprom.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/542917987/eeprom.o.d" -o ${OBJECTDIR}/_ext/542917987/eeprom.o ../../CANopen_stack/PIC32/eeprom.c  
	
${OBJECTDIR}/_ext/1472/main_PIC32.o: ../main_PIC32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main_PIC32.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main_PIC32.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -I"../../CANopen_stack" -I"../../CANopen_stack/PIC32" -I".." -Wall -MMD -MF "${OBJECTDIR}/_ext/1472/main_PIC32.o.d" -o ${OBJECTDIR}/_ext/1472/main_PIC32.o ../main_PIC32.c  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/PIC32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_REAL_ICE=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/PIC32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_REAL_ICE=1,--defsym=_min_heap_size=5000,-L"/C/Program Files/Microchip/MPLAB C32/lib",-L"/C/Program Files/Microchip/MPLAB C32/pic32mx/lib",-Map="${DISTDIR}/PIC32.X.${IMAGE_TYPE}.map" 
else
dist/${CND_CONF}/${IMAGE_TYPE}/PIC32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/PIC32.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=5000,-L"/C/Program Files/Microchip/MPLAB C32/lib",-L"/C/Program Files/Microchip/MPLAB C32/pic32mx/lib",-Map="${DISTDIR}/PIC32.X.${IMAGE_TYPE}.map"
	${MP_CC_DIR}\\pic32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/PIC32.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
