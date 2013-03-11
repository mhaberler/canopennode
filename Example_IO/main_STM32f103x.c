/*
 * CANopen main program file for STM32f103x microcontroller.
 *
 * Example code.
 *
 * @file        main_STM32f103x.c
 * @version     SVN: \$Id$
 * @author      Janez Paternoster
 * @author      Ondrej Netik
 * @copyright   2010 - 2013 Janez Paternoster, Ondrej Netik
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
#include "co_driver.h"
#include "timer.h"
#define cVer            "02.00"
#define SysTick_1ms    (SystemCoreClock / 1000)
#define SysTick_1us    (SystemCoreClock / 1000000)

/* Global variables */
volatile uint32_t CO_timer1ms = 0;
/* (not implemented) eeprom_t eeprom; */
uint8_t canTimerOff = 1;

void ClearWDT() {

}

uint32_t gMsCnt = 0;

uint32_t getTimer_us() {
    uint32_t tms, t;

    do {
        tms = gMsCnt;
        t = SysTick->VAL;
    }while(tms != gMsCnt);

    return (tms+1)*1000 - (t / SysTick_1us);
}

/* main ***********************************************************************/
int main(void) {
    uint8_t reset = 0;

    InitCanLeds();
    DBGU_Configure(115200);
    TRACE_INFO_WP("\n\rCanOpenNode %s (%s %s)\n\r", cVer, __DATE__, __TIME__);
    /* Configure Timer interrupt function for execution every 1 millisecond */
    if (SysTick_Config(SysTick_1ms))
        TRACE_FATAL("SysTick_Config\n\r");
    initTimer(getTimer_us);

    /* Todo: initialize EEPROM */

    /*  Todo: Loading COD */
    TRACE_INFO("Loading COD\n\r");

    /* Verify, if OD structures have proper alignment of initial values */
    TRACE_DEBUG("Checking COD in RAM (size=%d)\n\r", &CO_OD_RAM.LastWord - &CO_OD_RAM.FirstWord);
    if (CO_OD_RAM.FirstWord != CO_OD_RAM.LastWord)
        TRACE_FATAL("Err COD in RAM\n\r");
    TRACE_DEBUG("Checking COD in EEPROM (size=%d)\n\r", &CO_OD_EEPROM.LastWord - &CO_OD_EEPROM.FirstWord);
    if (CO_OD_EEPROM.FirstWord != CO_OD_EEPROM.LastWord)
        TRACE_FATAL("Err COD in EEPROM\n\r");
    TRACE_DEBUG("Checking COD in ROM (size=%d)\n\r", &CO_OD_ROM.LastWord - &CO_OD_ROM.FirstWord);
    if (CO_OD_ROM.FirstWord != CO_OD_ROM.LastWord)
        TRACE_FATAL("Err COD in ROM\n\r");

    /* increase variable each startup. Variable is stored in eeprom. */
    OD_powerOnCounter++;

    TRACE_INFO("CO power-on (BTR=%dk Node=0x%x)\n\r", CO_OD_ROM.CANBitRate, CO_OD_ROM.CANNodeID);
    ttimer tprof;
    while (reset != 2) {
        /* CANopen communication reset - initialize CANopen objects *******************/
        static uint32_t timer1msPrevious;
        CO_ReturnError_t err;

        /* disable timer interrupts, turn on red LED */
        canTimerOff = 1;
        CanLedsSet(eCoLed_Red);

        /* initialize CANopen */
        err = CO_init();
        if (err) {
            TRACE_FATAL("CO_init\n\r");
            /* CO_errorReport(CO->EM, ERROR_MEMORY_ALLOCATION_ERROR, err); */
        }

        /* start Timer */
        canTimerOff = 0;

        reset = 0;
        timer1msPrevious = CO_timer1ms;

        TRACE_INFO("CO (re)start\n\r");
        while (reset == 0) {
            saveTime(&tprof);
            /* loop for normal program execution ******************************************/
            uint32_t timer1msDiff;

            timer1msDiff = CO_timer1ms - timer1msPrevious;
            timer1msPrevious = CO_timer1ms;

            ClearWDT();

            /* CANopen process */
            reset = CO_process(CO, timer1msDiff);

            CanLedsSet((LED_GREEN_RUN(CO->NMT)>0 ? eCoLed_Green : 0) | (LED_RED_ERROR(CO->NMT)>0 ? eCoLed_Red : 0));

            ClearWDT();

            /* (not implemented) eeprom_process(&eeprom); */
            uint32_t t = getTime_us(&tprof);
            OD_performance[ODA_performance_mainCycleTime] = t;
            if (t > OD_performance[ODA_performance_mainCycleMaxTime])
                OD_performance[ODA_performance_mainCycleMaxTime] = t;

        } /*  while (reset != 0) */
    } /*  while (reset != 2) */
    /* program exit ***************************************************************/
    /* save variables to eeprom */
    DISABLE_INTERRUPTS();

    CanLedsSet(eCoLed_None);
    /* (not implemented) eeprom_saveAll(&eeprom); */
    CanLedsSet(eCoLed_Red);
    /* delete CANopen object from memory */
    CO_delete();

    /* reset - by WD */
    return 0;
}

/* timer interrupt function executes every millisecond ************************/
void CO_TIMER_ISR() {
    static ttimer tprof;
    saveTime(&tprof);
    CO_timer1ms++;

    CO_process_RPDO(CO);
    /*  TODO: zpracovani RPDO dat */

    /*  TODO: priprava dat pro TPDO */
    CO_process_TPDO(CO);

    /* calculate cycle time for performance measurement */
    uint32_t t = getTime_us(&tprof);
    OD_performance[ODA_performance_timerCycleTime] = t;
    if (t > OD_performance[ODA_performance_timerCycleMaxTime])
        OD_performance[ODA_performance_timerCycleMaxTime] = t;
}

/* System Timer interrupt function*/
void SysTick_Handler(void) {
    gMsCnt++;
    if (!canTimerOff)
        CO_TIMER_ISR();
}

/* CAN interrupt function *****************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void) {
    CO_CANinterrupt_Rx(CO->CANmodule[0]);
}

void USB_HP_CAN1_TX_IRQHandler(void) {
    CO_CANinterrupt_Tx(CO->CANmodule[0]);
}

void CAN1_SCE_IRQHandler(void) {
/*  not used */
}

void HardFault_Handler(void) {
    TRACE_FATAL("HardFault\n\r");
}
