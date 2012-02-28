/*******************************************************************************

   File - main_dsPIC33F.c
   Example code for using CANopenNode library Explorer16 board and
   dsPIC33FJ256GP710 microcontroller.

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

/*** Freescale includes ****/
#include "support_common.h" /* include peripheral declarations and more */
#if (CONSOLE_IO_SUPPORT || ENABLE_UART_SUPPORT) 
/* Standard IO is only possible if Console or UART support is enabled. */
#include <stdio.h>
#endif


#define CO_FSYS 64000	/* system clock : 64MHz */
#include "CANopen.h"
//(not implemented) #include "eeprom.h"



/* macros *********************************************************************/

//configuration macros
   #define CO_INTERRUPT_PRIORITY 1 				//interrupt priorities (CAN & timer)
   #define configCPU_CLOCK_HZ					( ( unsigned long ) 64000000 )
   #define CO_TMR_TICK_RATE_HZ					( ( unsigned long ) 1000 )
   #define CO_TMR_PRESCALE_REG_SETTING			( 5 << 8 ) 	
   #define CO_TMR_PRESCALE_VALUE				64

//timer macro
   #define CO_TIMER_ISR() 			__declspec(interrupt:0) void __isr_interrupt_119( void )
   #define CO_TMR_ISR_FLAG			((MCF_PIT0_PMR & MCF_PIT_PCSR_PIF )>>2)
   #define CO_TMR_ISR_CLRFLAG() 	{MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;}
   #define CO_TMR_TMR				MCF_PIT0_PMR
   #define CO_TMR_CompareMatchValue ((configCPU_CLOCK_HZ / CO_TMR_PRESCALE_VALUE ) / CO_TMR_TICK_RATE_HZ )
   #define CO_FCY					(CO_FSYS/CO_TMR_PRESCALE_VALUE)
   
//CAN macros   
   #define CO_CAN_ICR(x)   			{MCF_INTC1_ICR(x) = (1 | ( CO_INTERRUPT_PRIORITY << 3 )) ;} 
   #define CO_CAN_ISR(x)   			__declspec(interrupt:0) void __isr_flexcan_MB##x( void )
   #define CO_CAN_ISR_MACRO(ICODE) 	{ if(MCF_FlexCAN_IFLAG&MCF_FlexCAN_IFLAG_BUF(ICODE)) { MCF_FlexCAN_IFLAG |= MCF_FlexCAN_IFLAG_BUF(ICODE); CO_CANinterrupt(CO->CANmodule[0],ICODE);}}


/* prototypes *****************************************************************/
   CO_CAN_ISR(0);
   CO_CAN_ISR(1);
   CO_CAN_ISR(2);
   CO_CAN_ISR(3);
   CO_CAN_ISR(4);
   CO_CAN_ISR(5);
   CO_CAN_ISR(6);
   CO_CAN_ISR(7);
   CO_CAN_ISR(8);
   CO_CAN_ISR(9);
   CO_CAN_ISR(10);
   CO_CAN_ISR(11);
   CO_CAN_ISR(12);
   CO_CAN_ISR(13);
   CO_CAN_ISR(14);
   CO_CAN_ISR(15);
   CO_TIMER_ISR();  

/* Global variables ***********************************************************/
   const CO_CANbitRateData_t  CO_CANbitRateData[8] = {CO_CANbitRateDataInitializers};
   volatile UNSIGNED16 CO_timer1ms;
   CO_t *CO = 0;   //pointer to CANopen stack object
   //(not implemented) eeprom_t eeprom;


/* main ***********************************************************************/
int main (void){
   UNSIGNED8   reset = 0;
   int i=0;

   //Initialize led diodes. Two LSB bits are CANopen standard diodes.
   //Six MSB bits are for RPDO.
   /* TO DO */
   #define CAN_RUN_LED_ON()    	asm(nop);
   #define CAN_RUN_LED_OFF()   	asm(nop);
   #define CAN_ERROR_LED_ON()  	asm(nop);
   #define CAN_ERROR_LED_OFF() 	asm(nop);
   #define WRITE_LEDS(val) 		asm(nop);
   /*#define WRITE_LEDS(val)                                                     \
      {                                                                        \
         gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] & 0x03;      \
         gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] | (val&0xFC);\
      }*/




   //Verify, if OD structures have proper alignment of initial values
   if(CO_OD_RAM.FirstWord != CO_OD_RAM.LastWord) printf(("\nError in CO_OD_RAM."));
   if(CO_OD_EEPROM.FirstWord != CO_OD_EEPROM.LastWord) printf(("\nError in CO_OD_EEPROM."));
   if(CO_OD_ROM.FirstWord != CO_OD_ROM.LastWord) printf(("\nError in CO_OD_ROM."));


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
      DISABLE_INTERRUPTS();
      
      CO_CANsetConfigurationMode(ADDR_CAN1);
      CAN_RUN_LED_OFF();
      CAN_ERROR_LED_ON();

      //Initialize digital outputs
      WRITE_LEDS(0);
      OD_writeOutput8Bit[0] = 0;
      OD_writeOutput8Bit[1] = 0;

      //initialize CANopen
      err = (enum CO_ReturnError) (CO_init(&CO));
      if(err){
         printf(("\nCANopen initialization failed."));
         //CO_errorReport(CO->EM, ERROR_MEMORY_ALLOCATION_ERROR, err);
      }

      //start CAN
      CO_CANsetNormalMode(ADDR_CAN1);


	  //Configure interrupt priority and level and unmask interrupt for MB interrupts
	  MCF_INTC1_IMRL &= 0xFF0000FD; //Global interrupt mask for MB[15-0] <=> src[23-8]
	  for(i=8;i<24;i++) 
	  {
	  	CO_CAN_ICR(i); // interrupt configurations registers (src[23-8])
	  }

	  
      //Configure Timer interrupt function for execution every 1 millisecond
      MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;
      MCF_PIT0_PCSR = ( CO_TMR_PRESCALE_REG_SETTING | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_EN );
   	  MCF_PIT0_PMR = CO_TMR_CompareMatchValue;
	  
	  /* Configure interrupt priority and level and unmask interrupt for PIT0. */
	  MCF_INTC0_ICR55 = ( 1 | ( CO_INTERRUPT_PRIORITY << 3 ) );
	  MCF_INTC0_IMRH &= ~( MCF_INTC_IMRH_INT_MASK55 );

      reset = 0;
      timer1msPrevious = CO_timer1ms;

      while(reset == 0){
/* loop for normal program execution ******************************************/
         UNSIGNED16 timer1msDiff;
         static UNSIGNED16 TMR_TMR_PREV = 0;
         UNSIGNED16 t0,t;

         timer1msDiff = (UNSIGNED16) (CO_timer1ms - timer1msPrevious);
         timer1msPrevious = CO_timer1ms;


         //calculate cycle time for performance measurement
         t0 = CO_TMR_TMR;
         t = t0;
         if(t >= TMR_TMR_PREV){
            t = (UNSIGNED16) (t - TMR_TMR_PREV);
            t = (UNSIGNED16) ((timer1msDiff * 100) + (t / (CO_FCY / 100)));
         }
         else if(timer1msDiff){
            t = (UNSIGNED16) (TMR_TMR_PREV - t);
            t = (UNSIGNED16) ((timer1msDiff * 100) - (t / (CO_FCY / 100)));
         }
         else t = 0;
         t=0;
         OD_performance[ODA_performance_mainCycleTime] = t;
         if(t > OD_performance[ODA_performance_mainCycleMaxTime])
            OD_performance[ODA_performance_mainCycleMaxTime] = t;
         TMR_TMR_PREV = t0;

         //CANopen process
         reset = CO_process(CO, timer1msDiff);

         if(LED_GREEN_RUN(CO->NMT)){CAN_RUN_LED_ON();} else{CAN_RUN_LED_OFF();}
         if(LED_RED_ERROR(CO->NMT)){CAN_ERROR_LED_ON();} else{CAN_ERROR_LED_OFF();}

         //(not implemented) eeprom_process(&eeprom);
      }
   }
/* program exit ***************************************************************/
   printf("\nCANopenNode - exiting ...");

   //signal leds
   CAN_RUN_LED_OFF();
   CAN_ERROR_LED_ON();

   //delete CANopen object from memory
   CO_delete(&CO);

   //save variables to eeprom
   //(not implemented) eeprom_saveAll(&eeprom);

   CAN_ERROR_LED_OFF();
   WRITE_LEDS(0);
   return 0;
}


/* timer interrupt function executes every millisecond ************************/
CO_TIMER_ISR(){
   UNSIGNED16 t;
   
   /* Clear interrupt flag */
   CO_TMR_ISR_CLRFLAG();	

   CO_timer1ms++;

   CO_process_RPDO(CO);


   //read RPDO and show it on example LEDS on Explorer16
   /*UNSIGNED8 leds = OD_writeOutput8Bit[0];
   LATAbits.LATA2 = (leds&0x04) ? 1 : 0;
   LATAbits.LATA3 = (leds&0x08) ? 1 : 0;
   LATAbits.LATA4 = (leds&0x10) ? 1 : 0;
   LATAbits.LATA5 = (leds&0x20) ? 1 : 0;
   LATAbits.LATA6 = (leds&0x40) ? 1 : 0;
   LATAbits.LATA7 = (leds&0x80) ? 1 : 0;*/

   //prepare TPDO from example buttons on Explorer16
   /*UNSIGNED8 but = 0;
   if(!PORTDbits.RD6)  but |= 0x08;
   if(!PORTDbits.RD7)  but |= 0x04;
   if(!PORTDbits.RD13) but |= 0x01;
   OD_readInput8Bit[0] = but;*/


   CO_process_TPDO(CO);

   //verify timer overflow
   if(CO_TMR_ISR_FLAG == 1){
      CO_errorReport(CO->EM, ERROR_ISR_TIMER_OVERFLOW, 0);
      CO_TMR_ISR_CLRFLAG();
   }

   //calculate cycle time for performance measurement
   t = (UNSIGNED16) (CO_TMR_TMR / (CO_FCY / 100));
   OD_performance[ODA_performance_timerCycleTime] = t;
   if(t > OD_performance[ODA_performance_timerCycleMaxTime])
      OD_performance[ODA_performance_timerCycleMaxTime] = t;
}


/************************** CAN interrupt functions (one for each buffer) ****************************/


/*-----------------------------------------------------------*/
/* MB0 interrupt function */
CO_CAN_ISR(0)
{
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(0);
}

/*-----------------------------------------------------------*/
/* MB1 interrupt function */
CO_CAN_ISR(1)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(1);
}

/*-----------------------------------------------------------*/
/* MB2 interrupt function */
CO_CAN_ISR(2)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(2);
}

/*-----------------------------------------------------------*/
/* MB3 interrupt function */
CO_CAN_ISR(3)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(3);
}

/*-----------------------------------------------------------*/
/* MB4 interrupt function */
CO_CAN_ISR(4)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(4);
}

/*-----------------------------------------------------------*/
/* MB5 interrupt function */
CO_CAN_ISR(5)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(5);
}

/*-----------------------------------------------------------*/
/* MB6 interrupt function */
CO_CAN_ISR(6)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(6);
}

/*-----------------------------------------------------------*/
/* MB7 interrupt function */
CO_CAN_ISR(7)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(7);
}

/*-----------------------------------------------------------*/
/* MB8 interrupt function */
CO_CAN_ISR(8)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(8);
}

/*-----------------------------------------------------------*/
/* MB9 interrupt function */
CO_CAN_ISR(9)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(9);
}

/*-----------------------------------------------------------*/
/* MB10 interrupt function */
CO_CAN_ISR(10)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(10);
}

/*-----------------------------------------------------------*/
/* MB11 interrupt function */
CO_CAN_ISR(11)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(11);
}

/*-----------------------------------------------------------*/
/* MB12 interrupt function */
CO_CAN_ISR(12)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(12);
}

/*-----------------------------------------------------------*/
/* MB13 interrupt function */
CO_CAN_ISR(13)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(13);
}

/*-----------------------------------------------------------*/
/* MB14 interrupt function */
CO_CAN_ISR(14)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(14);
}

/*-----------------------------------------------------------*/
/* MB15 interrupt function */
CO_CAN_ISR(15)
{	
   //check interrupt & clear flag
	CO_CAN_ISR_MACRO(15);
}