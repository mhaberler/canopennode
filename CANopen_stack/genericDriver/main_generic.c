/*
 * CANopen main program file.
 *
 * This file is a template for other microcontrollers.
 *
 * @file        main_generic.c
 * @version     SVN: \$Id$
 * @author      Janez Paternoster
 * @copyright   2004 - 2013 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <http://canopennode.sourceforge.net>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * CANopenNode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "CANopen.h"
#include "application.h"


/* Global variables and objects */
    volatile uint16_t CO_timer1ms=0;    /* variable increments each millisecond */


/* main ***********************************************************************/
int main (void){
    uint8_t reset = 0;

    /* Configure microcontroller. */


    /* initialize EEPROM */


    /* Application interface */
    programStart();


    /* increase variable each startup. Variable is stored in EEPROM. */
    OD_powerOnCounter++;


    while(reset < 2){
/* CANopen communication reset - initialize CANopen objects *******************/
        CO_ReturnError_t err;
        uint16_t timer1msPrevious;

        /* disable timer and CAN interrupts */


        /* initialize CANopen */
        err = CO_init();
        if(err){
            while(1);
            /* CO_errorReport(CO->EM, ERROR_MEMORY_ALLOCATION_ERROR, err); */
        }


        /* initialize variables */
        timer1msPrevious = CO_timer1ms;
        reset = 0;


        /* Configure Timer interrupt function for execution every 1 millisecond */


        /* Configure CAN transmit and receive interrupt */


        /* Application interface */
        communicationReset();


        /* start CAN and enable interrupts */
        CO_CANsetNormalMode(ADDR_CAN1);


        while(reset == 0){
/* loop for normal program execution ******************************************/
            uint16_t timer1msDiff;

            DISABLE_INTERRUPTS();
            timer1msDiff = CO_timer1ms - timer1msPrevious;
            timer1msPrevious = CO_timer1ms;
            ENABLE_INTERRUPTS();


            /* Application interface */
            programAsync(timer1msDiff);


            /* CANopen process */
            reset = CO_process(CO, timer1msDiff);


            /* Process EEPROM */
        }
    }


/* program exit ***************************************************************/
    DISABLE_INTERRUPTS();


    /* Application interface */
    programEnd();


    /* delete objects from memory */
    CO_delete();


    /* reset */
    return 0;
}


/* timer interrupt function executes every millisecond ************************/
void /* interrupt */ CO_TimerInterruptHandler(void){

    /* clear interrupt flag */


    CO_timer1ms++;


    CO_process_RPDO(CO);


    /* Application interface */
    program1ms();


    CO_process_TPDO(CO);


    /* verify timer overflow (is flag set again?) */
    if(0){
        CO_errorReport(CO->EM, ERROR_ISR_TIMER_OVERFLOW, 0);
    }
}


/* CAN interrupt function *****************************************************/
void /* interrupt */ CO_CAN1InterruptHandler(void){
    CO_CANinterrupt(CO->CANmodule[0]);


    /* clear interrupt flag */
}
