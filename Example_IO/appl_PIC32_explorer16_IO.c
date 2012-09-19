/*******************************************************************************

   File - appl_PIC32_explorer16_IO.c
   Application program CANopen IO device on Microchip PIC32 on Explorer16
   demo board.

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
   This file is tested on explorer16 board from Microchip. Microcontroller is
   PIC32MX795F512L. D3 and D4 LEDs are used as CANopen status LEDs (D4 should
   be red). Device sends bootup and Heartbeat message. Default NodeID is 0x30.
   Implemented is simple CANopen I/O device profile (DS401):
    - TPDO with address 0x1B0 is send, if any button (S3, S6, S4) is pressed.
    - LED diodes (D5...D10) are controlled by two bytes long RPDO on
      CAN address 0x230 (upper six bits from first byte are used to control LEDs).
*******************************************************************************/


#include "CANopen.h"
#include "application.h"


//Global variables
   extern CO_t *CO;                          //pointer to external CANopen object


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

   //Initialize two CAN led diodes
   #define CAN_RUN_LED        LATAbits.LATA0
   #define CAN_ERROR_LED      LATAbits.LATA1
   TRISAbits.TRISA0 = 0;
   TRISAbits.TRISA1 = 0;
   CAN_RUN_LED = 0;      CAN_ERROR_LED = 1;

   //Initialize other LED diodes for RPDO
   TRISAbits.TRISA2 = 0; LATAbits.LATA2 = 0;
   TRISAbits.TRISA3 = 0; LATAbits.LATA3 = 0;
   TRISAbits.TRISA4 = 0; LATAbits.LATA4 = 0;
   TRISAbits.TRISA5 = 0; LATAbits.LATA5 = 0;
   TRISAbits.TRISA6 = 0; LATAbits.LATA6 = 0;
   TRISAbits.TRISA7 = 0; LATAbits.LATA7 = 0;
}


/******************************************************************************/
void communicationReset(void){
   CAN_RUN_LED = 0; CAN_ERROR_LED = 0;
   OD_writeOutput8Bit[0] = 0;
   OD_writeOutput8Bit[1] = 0;
}


/******************************************************************************/
void programEnd(void){
   CAN_RUN_LED = 0; CAN_ERROR_LED = 0;
}


/******************************************************************************/
void programAsync(unsigned int timer1msDiff){

   CAN_RUN_LED = LED_GREEN_RUN(CO->NMT);
   CAN_ERROR_LED = LED_RED_ERROR(CO->NMT);

   //Is any application critical error set?
   //If error register is set, device will leave operational state.
   if(CO->EM->errorStatusBits[8] || CO->EM->errorStatusBits[9])
      *CO->EMpr->errorRegister |= 0x20;
}


/******************************************************************************/
void program1ms(void){
   UNSIGNED8 leds, buttons;

   //Read RPDO and show it on LEDS on Explorer16.
   leds = OD_writeOutput8Bit[0];
   LATAbits.LATA2 = (leds&0x04) ? 1 : 0;
   LATAbits.LATA3 = (leds&0x08) ? 1 : 0;
   LATAbits.LATA4 = (leds&0x10) ? 1 : 0;
   LATAbits.LATA5 = (leds&0x20) ? 1 : 0;
   LATAbits.LATA6 = (leds&0x40) ? 1 : 0;
   LATAbits.LATA7 = (leds&0x80) ? 1 : 0;


   //Verify operating state of this node
   //LATAbits.LATA2 = (CO->NMT->operatingState == CO_NMT_OPERATIONAL) ? 1 : 0;

   //Verify operating state of monitored nodes
   //LATAbits.LATA3 = (CO->HBcons->allMonitoredOperational) ? 1 : 0;

   //Example error is simulated from buttons on Explorer16
   if(!PORTDbits.RD6) CO_errorReport(CO->EM, ERROR_TEST1_INFORMATIVE, 0x12345678L);
   if(!PORTDbits.RD7) CO_errorReset(CO->EM, ERROR_TEST1_INFORMATIVE, 0xAAAAAABBL);


   //Prepare TPDO from buttons on Explorer16.
   //According to PDO mapping and communication parameters, first TPDO is sent
   //automatically on change of state of OD_readInput8Bit[0] variable.
   buttons = 0;
   if(!PORTDbits.RD6)  buttons |= 0x08;
   if(!PORTDbits.RD7)  buttons |= 0x04;
   if(!PORTDbits.RD13) buttons |= 0x01;
   OD_readInput8Bit[0] = buttons;
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
