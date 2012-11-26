/*******************************************************************************

   File - main_SC2x3.c
   Main program file for SC243 micro controller.

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


#include "CANopen.h"
#include "application.h"
#include "eeprom.h"
#include "CgiLog.h"
#include "CgiOD.h"
#include "CgiSend.h"


//Global variables and objects
   volatile UNSIGNED16        CO_timer1ms=0; //variable increments each millisecond
   CO_t                      *CO = 0;        //pointer to CANopen object
   EE_t                      *EE = 0;        //pointer to eeprom object
   CgiLog_t                  *CgiLog = 0;    //pointer to CGILog object
   CgiCli_t                  *CgiCli = 0;    //pointer to CGICli object
   CgiSend_t                 *CgiSend = 0;   //pointer to CGISend object


//Variables for timer task
   static void TaskTimer(void);
   static RTX_ID TaskTimerID = 0;
   static DWORD TaskTimerStack[10000] ;
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
   UNSIGNED8 reset = 0;
   UNSIGNED8 powerOn = 1;

   printf("\nCANopenNode - starting ...");


   //Verify, if OD structures have proper alignment of initial values
   if(CO_OD_RAM.FirstWord != CO_OD_RAM.LastWord)      {printf("\nError: Error in CO_OD_RAM.");   return 0;}
   if(CO_OD_EEPROM.FirstWord != CO_OD_EEPROM.LastWord){printf("\nError: Error in CO_OD_EEPROM.");return 0;}
   if(CO_OD_ROM.FirstWord != CO_OD_ROM.LastWord)      {printf("\nError: Error in CO_OD_ROM.");   return 0;}


   //initialize battery powered SRAM
   struct{
      UNSIGNED8* EEPROMptr;
      UNSIGNED32 EEPROMsize;
      UNSIGNED8* emcyBufPtr;
      UNSIGNED32 emcyBufSize;
      UNSIGNED32 size;
   }SRAM;

   SRAM.size = 0;
   //Sizes must be divisible by 4. (struct should have UNSIGNED32 as last element.)
   SRAM.EEPROMsize = sizeof(CO_OD_EEPROM);                   SRAM.size += SRAM.EEPROMsize;
   SRAM.emcyBufSize = OD_CANopenLog.emcySRAMSize&0xFFFFFFFC; SRAM.size += SRAM.emcyBufSize;

   if(RTX_MemWindow(gSysPublicData.sramAddr, SRAM.size)){
      SRAM.EEPROMptr = 0;
      SRAM.emcyBufPtr = 0;
      printf("\nError: SRAM initialization failed.");
      return 0;
   }
   else{
      SRAM.EEPROMptr = gSysPublicData.sramAddr;
      SRAM.emcyBufPtr = SRAM.EEPROMptr + SRAM.EEPROMsize;
   }


   //initialize EEPROM - part 1
   INTEGER16 EEStatus = EE_init_1(
                       &EE,
                        SRAM.EEPROMptr,
          (UNSIGNED8*) &CO_OD_EEPROM,
                        sizeof(CO_OD_EEPROM),
          (UNSIGNED8*) &CO_OD_ROM,
                        sizeof(CO_OD_ROM));


   //initialize CGI interface. These functions are bound with a fixed name and
   //are executed by the Web server task, if a HTTP request with such a fixed
   //name comes in. This mechanism allows dynamic usage of the IPC@CHIP Web server.
   if(   CgiLog_init_1(&CgiLog, SRAM.emcyBufPtr, SRAM.emcyBufSize, OD_CANopenLog.CANLogSize, OD_CANopenLog.maxDumpFiles)
      || CgiCli_init_1(&CgiCli, 0x10000, 0x800)
      || CgiSend_init_1(&CgiSend, 1000))
   {
      printf("\nError: CGI initialization failed.");
      return 0;
   }


   programStart();


   //increase variable each startup. Variable is stored in eeprom.
   OD_powerOnCounter++;


   while(reset < 2){
/* CANopen communication reset - initialize CANopen objects *******************/
      enum CO_ReturnError err;
      UNSIGNED16 timer1msPrevious;

      printf("\nCANopenNode - communication reset ...");

      //disable timer and CAN interrupts
      if(TaskTimerID) RTX_Suspend_Task(TaskTimerID);
      CO_CANsetConfigurationMode(ADDR_CAN1);


      //initialize CANopen
      err = CO_init(&CO);
      if(err){
         printf("\nError: CANopen initialization failed.");
         //CO_errorReport(CO->EM, ERROR_MEMORY_ALLOCATION_ERROR, err);
      }


      //initialize eeprom - part 2
      EE_init_2(EE, EEStatus, CO->SDO, CO->EM);


      communicationReset();


      //just for info
      if(powerOn){
         CO_errorReport(CO->EM, ERROR_MICROCONTROLLER_RESET, OD_powerOnCounter);
         powerOn = 0;
      }


      //start CAN
      CO_CANsetNormalMode(ADDR_CAN1);


      //Configure Timer interrupt function for execution every 1 millisecond
      if(TaskTimerID){
         RTX_Resume_Task(TaskTimerID);
      }
      else{
         TaskTimerID = RTX_NewTask(&TaskTimerDef);
      }

      OD_performance[ODA_performance_mainCycleMaxTime] = 0;
      OD_performance[ODA_performance_timerCycleMaxTime] = 0;
      reset = 0;
      timer1msPrevious = CO_timer1ms;
      printf("\nCANopenNode - running ...");


      while(reset == 0){
/* loop for normal program execution ******************************************/
         UNSIGNED16 timer1msCopy, timer1msDiff;

         //read start time for performance measurement
         UNSIGNED32 TB_prev = readTB();

         timer1msCopy = CO_timer1ms;
         timer1msDiff = timer1msCopy - timer1msPrevious;
         timer1msPrevious = timer1msCopy;


         //Application asynchronous program
         programAsync(timer1msDiff);


         //CANopen process
         reset = CO_process(CO, timer1msDiff);

         if(kbhit()) if(getch() == 'q') reset = 3; //quit only, no reboot

         CgiLogEmcyProcess(CgiLog);


         //calculate cycle time for performance measurement
         //units are 0,01 * ms (percent of timer cycle time)
         UNSIGNED32 ticks = readTB() - TB_prev;
         ticks /= RTX_TB_TICKS_PER_us * 10;
         if(ticks > 0xFFFF) ticks = 0xFFFF;
         OD_performance[ODA_performance_mainCycleTime] = ticks;
         if(ticks > OD_performance[ODA_performance_mainCycleMaxTime])
            OD_performance[ODA_performance_mainCycleMaxTime] = ticks;


         RTX_Sleep_Time(50);


         EE_process(EE);
      }
   }


/* program exit ***************************************************************/
   //delete timer task
   RTX_Delete_Task(TaskTimerID);

   //delete objects from memory
   programEnd();
   CO_delete(&CO);
   EE_delete(&EE);
   CgiLog_delete(&CgiLog);
   CgiCli_delete(&CgiCli);
   CgiSend_delete(&CgiSend);


   printf("\nCANopenNode finished");
   if(reset == 2) BIOS_Reboot(); //CANopen reset node
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

   //read start time for performance measurement
   UNSIGNED32 TB_prev = readTB();

   CO_process_RPDO(CO);


   program1ms();


   CO_process_TPDO(CO);

   //calculate cycle time for performance measurement
   //units are 0,01 * ms (percent of timer cycle time)
   UNSIGNED32 ticks = readTB() - TB_prev;
   ticks /= RTX_TB_TICKS_PER_us * 10;
   if(ticks > 0xFFFF) ticks = 0xFFFF;
   OD_performance[ODA_performance_timerCycleTime] = ticks;
   if(ticks > OD_performance[ODA_performance_timerCycleMaxTime])
      OD_performance[ODA_performance_timerCycleMaxTime] = ticks;

   catchUp--;
}


/* CAN callback function ******************************************************/
int CAN1callback(CanEvent event, const CanMsg *msg){
   return CO_CANinterrupt(CO->CANmodule[0], event, msg);
}

int CAN2callback(CanEvent event, const CanMsg *msg){
   return CO_CANinterrupt(CO->CANmodule[1], event, msg);
}
