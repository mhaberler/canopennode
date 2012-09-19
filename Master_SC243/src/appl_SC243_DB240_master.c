/*******************************************************************************

   File - appl_SC243_DB240_master.c
   Application program CANopen IO device on Beck SC243 on DB240 demo board.

   Copyright (C) 2010 Janez Paternoster

   License: GNU Lesser General Public License (LGPL).

   <http://canopennode.sourceforge.net>
*/
/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.


   Author: Janez Paternoster

*******************************************************************************/
/*******************************************************************************

   Application program.

*******************************************************************************/


/*******************************************************************************
   This file is tested on DB240 board from Beck. Microcontroller is SC243.
      D0 and D1 LEDs are used as CANopen status LEDs (D1 should be red). Device
      sends bootup and Heartbeat message. Default NodeID is 0x30.
   Implemented is simple CANopen I/O device profile (DS401):
    - TPDO with address 0x1B0 is send on change of state of DIP switches.
    - LED diodes (D2...D7) are controlled by two bytes long RPDO on
      CAN address 0x230 (upper six bits from first byte is used to control LEDs).
*******************************************************************************/


#include "CANopen.h"
#include "application.h"
#include "CgiLog.h"


//constants
   #define CHIP_SELECT          1            //Chip select to use
   #define OFFSET_LED_WRITE     0            //offset of LED output register
   #define OFFSET_LED_READ      1            //offset of LED input register
   #define OFFSET_DIP_READ      0            //offset of dip switch input register


//Global variables
   extern CO_t *CO;                          //pointer to external CANopen object
   unsigned char *gBaseAddr = NULL;          //hardware base address
   UNSIGNED8 errorRegisterPrev;


//LED diodes. Two LSB bits are CANopen standard diodes.
   #define CAN_RUN_LED_ON()    gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] | 0x01
   #define CAN_RUN_LED_OFF()   gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] & 0xFE
   #define CAN_ERROR_LED_ON()  gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] | 0x02
   #define CAN_ERROR_LED_OFF() gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] & 0xFD
   #define WRITE_LEDS(val){                                                 \
      gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] & 0x03;      \
      gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] | (val&0xFC);}


/*******************************************************************************
   Indexes and Error codes for application specific errors. For information on
   error control see CO_Emergency.h file.

   'Error status bits' from 00 to 2F are reserved for use by the CANopen stack.
   Values 30 to 3F are used here as application informative errors.
   Values 40 to 4F are used here as application critical errors.
*******************************************************************************/
   #if ODL_errorStatusBits_stringLength < 10
      #error Error in Object Dictionary!
   #endif

   #define ERROR_TEST1_INFORMATIVE                    0x30, 0x1000
   #define ERROR_TEST2_CRITICAL                       0x40, 0x5000


/******************************************************************************/
void programStart(void){

   //Initialize hardware
   CsSettings csSettings;  // Chip select settings data structure
   printf("\nCANopenNode - initialising DB240 hardware ...");
   csSettings.mux       = TRUE;
   csSettings.ack       = FALSE;
   csSettings.longAle   = TRUE;
   csSettings.addrSize  = 2;
   csSettings.dataSize  = 0;
   csSettings.bankBits  = 0;
   csSettings.writeOnly = FALSE;
   csSettings.readOnly  = FALSE;
   gBaseAddr = (unsigned char *)csSetMode(CHIP_SELECT, &csSettings);
   if(!gBaseAddr)
      printf("\nError: Could not set chip select mode.");
   if(!csEnable(CHIP_SELECT, TRUE))
      printf("\nError: Could not enable chip select.");


   CAN_RUN_LED_ON(); CAN_ERROR_LED_OFF(); WRITE_LEDS(0)
}


/******************************************************************************/
void communicationReset(void){
   CAN_RUN_LED_OFF(); CAN_ERROR_LED_OFF(); WRITE_LEDS(0)
   OD_writeOutput8Bit[0] = 0;
   OD_writeOutput8Bit[1] = 0;
   errorRegisterPrev = 0;
}


/******************************************************************************/
void programEnd(void){
   CAN_RUN_LED_OFF(); CAN_ERROR_LED_OFF(); WRITE_LEDS(0)
}


/******************************************************************************/
void programAsync(unsigned int timer1msDiff){

   if(LED_GREEN_RUN(CO->NMT))CAN_RUN_LED_ON();   else CAN_RUN_LED_OFF();
   if(LED_RED_ERROR(CO->NMT))CAN_ERROR_LED_ON(); else CAN_ERROR_LED_OFF();

   //Is any application critical error set?
   //If error register is set, device will leave operational state.
   if(CO->EM->errorStatusBits[8] || CO->EM->errorStatusBits[9])
      *CO->EMpr->errorRegister |= 0x20;

   //if error register is set, make dump of CAN message log
   if(!errorRegisterPrev && *CO->EMpr->errorRegister){
      extern CgiLog_t *CgiLog;
      CgiLogSaveBuffer(CgiLog);
      errorRegisterPrev = *CO->EMpr->errorRegister;
   }
}


/******************************************************************************/
void program1ms(void){

   //Read RPDO and show it on LEDS on DB240
   WRITE_LEDS(OD_writeOutput8Bit[0]);


   //Verify operating state of this node
   //LATAbits.LATA2 = (CO->NMT->operatingState == CO_NMT_OPERATIONAL) ? 1 : 0;

   //Verify operating state of monitored nodes
   //LATAbits.LATA3 = (CO->HBcons->allMonitoredOperational) ? 1 : 0;

   //Example error
   //if(errorCondition) CO_errorReport(CO->EM, ERROR_TEST1_INFORMATIVE, 0x12345678L);
   //if(errorSolved) CO_errorReset(CO->EM, ERROR_TEST1_INFORMATIVE, 0xAAAAAABBL);


   //Prepare TPDO from buttons on Explorer16.
   //According to PDO mapping and communication parameters, first TPDO is sent
   //automatically on change of state of OD_readInput8Bit[0] variable.
   OD_readInput8Bit[0] = gBaseAddr[OFFSET_DIP_READ];
}


/******************************************************************************/
//Function passes data to SDO server on testDomain variable access
UNSIGNED32 ODF_testDomain( void       *object,
                           UNSIGNED16  index,
                           UNSIGNED8   subIndex,
                           UNSIGNED16 *pLength,
                           UNSIGNED16  attribute,
                           UNSIGNED8   dir,
                           void       *dataBuff,
                           const void *pData){
   //pData is not valid here

   if(dir == 0){  //reading object dictionary
      UNSIGNED16 i;
      UNSIGNED16 dataSize = 889; //889 = 127*7 = 0x379 = maximum block transfer

      //verify max buffer size
      if(dataSize > CO_OD_MAX_OBJECT_SIZE) //default is 32. Redefine macro in CO_driver.h to 889.
         return 0x06040047L;  //general internal incompatibility in the device

      //fill buffer
      for(i=0; i<dataSize; i++)
         ((UNSIGNED8*) dataBuff)[i] = i+1;

      //indicate length
      *pLength = dataSize;

      //return OK
      return 0;
   }

   else{          //writing object dictionary
      UNSIGNED16 i;
      UNSIGNED16 err = 0;
      UNSIGNED16 dataSize = *pLength;

      //do something with data, here just verify if they are the same as above
      //printf("\nData received, %d bytes:\n", dataSize);
      for(i=0; i<dataSize; i++){
         UNSIGNED8 b = ((UNSIGNED8*) dataBuff)[i];
         if(b != ((i+1)&0xFF)) err++;
         //printf("%02X ", b);
      }

      if(err) return 0x06090030; //Invalid value for parameter (download only).

      //return OK
      return 0;
   }
}
