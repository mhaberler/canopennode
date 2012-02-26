/*******************************************************************************

   File - main_SC2x3.c
   Example code for using CANopenNode library on DB240 demo board with SC243
   processor.

   Copyright (C) 2010 Janez Paternoster

   License: GNU General Public License (GPL).

   <http://canopennode.sourceforge.net>
*/
/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.


   Author: Janez Paternoster

*******************************************************************************/


/*******************************************************************************
   This file is tested on DB240 board from Beck. Microcontroller  is SC243.
	D0 and D1 LEDs are used as CANopen status LEDs (D1 should be red). Device
	sends bootup and Heartbeat message. Default NodeID is 0x30.
   Implemented is simple CANopen I/O device profile (DS401):
    - TPDO with address 0x1B0 is send on change of state of DIP switches.
    - LED diodes (D2...D7) are controlled by two bytes long RPDO on
      CAN address 0x230 (upper six bits from first byte is used to control LEDs).
*******************************************************************************/


#include "CANopen.h"


//constants
   #define CHIP_SELECT          1           // Chip select to use
   #define OFFSET_LED_WRITE     0           // Offset of LED output register
   #define OFFSET_LED_READ      1           // Offset of LED input register
   #define OFFSET_DIP_READ      0           // Offset of dip switch input register


//macros
#define error(args)           \
   {                          \
      printf("\r\nError: ");  \
      printf args;            \
      return 1;               \
   }


//Global variables
   volatile UNSIGNED16 CO_timer1ms;
   CO_t *CO = 0;   //pointer to CANopen stack object
   unsigned char *gBaseAddr  = NULL;  // Hardware base address
   //(not implemented) eeprom_t eeprom;


//Variables for timer task
   static void TaskTimer(void);
   static RTX_ID TaskTimerID = 0;
   static DWORD TaskTimerStack[2000] ;
   static const RtxTaskDefS TaskTimerDef = {
       {'C', 'O', 'T', 'M'},               // tkdTag[4]
       (RTX_TASK_PROC) TaskTimer,          // tkdProc
       &TaskTimerStack[0],                 // tkdStackBase
       sizeof(TaskTimerStack),             // tkdStackSize
       0,                                  // tkdlParam (not used here)
       22,                                 // tkdPriority
       RTX_TA_PERIODIC | RTX_TA_CATCH_UP,  // tkdAttr
       0,                                  // tkdSlice
       1,                                  // tkdPeriod [ms]
       // Remaining members tkdPhase, and tkdPhaseRef
       // play no role in this example since their corresponding tkdAttr
       // flags are zero.
   } ;

/* main ***********************************************************************/
int main (void){
   UNSIGNED8   reset = 0;

   //Initialize hardware
   CsSettings csSettings;  // Chip select settings data structure
   printf("\nCANopenNode - initialising DB240 hardware...");
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
      error(("\nCould not set chip select mode."));
   if(!csEnable(CHIP_SELECT, TRUE))
      error(("\nCould not enable chip select."));


   //Initialize led diodes. Two LSB bits are CANopen standard diodes.
   //Six MSB bits are for RPDO.
   gBaseAddr[OFFSET_LED_WRITE] = 0x02; //only error led is on
   #define CAN_RUN_LED_ON()    gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] | 0x01;
   #define CAN_RUN_LED_OFF()   gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] & 0xFE;
   #define CAN_ERROR_LED_ON()  gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] | 0x02;
   #define CAN_ERROR_LED_OFF() gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] & 0xFD;
   #define WRITE_LEDS(val)                                                     \
      {                                                                        \
         gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] & 0x03;      \
         gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] | (val&0xFC);\
      }


   //Verify, if OD structures have proper alignment of initial values
   if(CO_OD_RAM.FirstWord != CO_OD_RAM.LastWord) error(("\nError in CO_OD_RAM."));
   if(CO_OD_EEPROM.FirstWord != CO_OD_EEPROM.LastWord) error(("\nError in CO_OD_EEPROM."));
   if(CO_OD_ROM.FirstWord != CO_OD_ROM.LastWord) error(("\nError in CO_OD_ROM."));


   //initialize EEPROM
      //(not implemented)

   //increase variable each startup. Variable is stored in eeprom.
   OD_powerOnCounter++;

   while(reset != 2){
/* CANopen communication reset - initialize CANopen objects *******************/
      static UNSIGNED16 timer1msPrevious;
      enum CO_ReturnError err;

      printf("\nCANopenNode - communication reset ...");

      //disable timer and CAN interrupts, turn on red LED
      if(TaskTimerID) RTX_Suspend_Task(TaskTimerID);
      CO_CANsetConfigurationMode(ADDR_CAN1);
      CAN_RUN_LED_OFF();
      CAN_ERROR_LED_ON();

      //Initialize digital outputs
      WRITE_LEDS(0);
      OD_writeOutput8Bit[0] = 0;
      OD_writeOutput8Bit[1] = 0;


      //initialize CANopen
      err = CO_init(&CO);
      if(err){
         error(("\nCANopen initialization failed."));
         //CO_errorReport(CO->EM, ERROR_MEMORY_ALLOCATION_ERROR, err);
      }


      //initialize some variables
      CO_timer1ms = 0;
      reset = 0;
      timer1msPrevious = CO_timer1ms;
      OD_performance[ODA_performance_mainCycleMaxTime] = 0;
      OD_performance[ODA_performance_timerCycleMaxTime] = 0;

      //start CAN
      CO_CANsetNormalMode(ADDR_CAN1);


      //Configure Timer interrupt function for execution every 1 millisecond
      if(TaskTimerID){
         RTX_Resume_Task(TaskTimerID);
      }
      else{
         TaskTimerID = RTX_NewTask(&TaskTimerDef);
      }

      printf("\nCANopenNode - running ...");

      while(reset == 0){
/* loop for normal program execution ******************************************/
         UNSIGNED16 timer1msDiff;

         timer1msDiff = CO_timer1ms - timer1msPrevious;
         timer1msPrevious = CO_timer1ms;

         //calculate cycle time for performance measurement
         //units are 0,01 * ms (percent of timer cycle time)
         static UNSIGNED32 TB_prev = 0;
         UNSIGNED32 TB = readTB();
         UNSIGNED32 ticks = TB - TB_prev;
         ticks /= RTX_TB_TICKS_PER_us * 10;
         if(ticks > 0xFFFF) ticks = 0xFFFF;
         OD_performance[ODA_performance_mainCycleTime] = ticks;
         if(ticks > OD_performance[ODA_performance_mainCycleMaxTime])
            if(OD_performance[ODA_performance_mainCycleMaxTime]) //ignore startup pass
               OD_performance[ODA_performance_mainCycleMaxTime] = ticks;
         TB_prev = TB;

         //CANopen process
         reset = CO_process(CO, timer1msDiff);

         if(LED_GREEN_RUN(CO->NMT)){CAN_RUN_LED_ON();} else{CAN_RUN_LED_OFF();}
         if(LED_RED_ERROR(CO->NMT)){CAN_ERROR_LED_ON();} else{CAN_ERROR_LED_OFF();}

         RTX_Sleep_Time(50);

         //(not implemented) eeprom_process(&eeprom);
      }
   }
/* program exit ***************************************************************/

   printf("\nCANopenNode - exiting ...");

   //signal leds
   CAN_RUN_LED_OFF();
   CAN_ERROR_LED_ON();

   //delete timer task
   RTX_Delete_Task(TaskTimerID);

   //delete CANopen object from memory
   CO_delete(&CO);

   //save variables to eeprom
   //(not implemented) eeprom_saveAll(&eeprom);

   CAN_ERROR_LED_OFF();
   WRITE_LEDS(0);
   return 0;
}


/* timer interrupt function executes every millisecond ************************/
static void TaskTimer(void){
   volatile static UNSIGNED8 catchUp = 0;

   CO_timer1ms++;

   //verify overflow
   if(catchUp++){
      OD_performance[ODA_performance_timerCycleMaxTime] = 100;
      CO_errorReport(CO->EM, ERROR_ISR_TIMER_OVERFLOW, 0);
      return;
   }


   CO_process_RPDO(CO);


   //read RPDO and show it on LEDS on DB240
   WRITE_LEDS(OD_writeOutput8Bit[0]);

   //prepare TPDO from DIP switch on DB240
   OD_readInput8Bit[0] = gBaseAddr[OFFSET_DIP_READ];


   CO_process_TPDO(CO);

   //calculate cycle time for performance measurement
   //units are 0,01 * ms (percent of timer cycle time)
   static UNSIGNED32 TB_prev = 0;
   UNSIGNED32 TB = readTB();
   UNSIGNED32 ticks = TB - TB_prev;
   ticks /= RTX_TB_TICKS_PER_us * 10;
   if(ticks > 0xFFFF) ticks = 0xFFFF;
   OD_performance[ODA_performance_timerCycleTime] = ticks;
   if(ticks > OD_performance[ODA_performance_timerCycleMaxTime])
      if(OD_performance[ODA_performance_timerCycleMaxTime]) //ignore startup pass
         OD_performance[ODA_performance_timerCycleMaxTime] = ticks;
   TB_prev = TB;

   catchUp--;
}


/* CAN callback function ******************************************************/
int CAN1callback(CanEvent event, const CanMsg *msg){
   return CO_CANinterrupt(CO->CANmodule[0], event, msg);
}

int CAN2callback(CanEvent event, const CanMsg *msg){
   return CO_CANinterrupt(CO->CANmodule[1], event, msg);
}
