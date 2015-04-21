/*
 * Application program for CANopen IO device on Beck SC243 on DB240
 * demo board.
 *
 * @file        appl_SC243_DB240_master.c
 * @version     SVN: \$Id$
 * @author      Janez Paternoster
 * @copyright   2010 - 2013 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <http://canopennode.sourceforge.net>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * CANopenNode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
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


/*
 * This file is tested on DB240 board from Beck. Microcontroller is SC243.
 *     D0 and D1 LEDs are used as CANopen status LEDs (D1 should be red). Device
 *     sends bootup and Heartbeat message. Default NodeID is 0x20.
 * Implemented is simple CANopen I/O device profile (DS401):
 *  - TPDO with address 0x1A0 is send on change of state of DIP switches.
 *  - LED diodes (D2...D7) are controlled by two bytes long RPDO on
 *     CAN address 0x220 (upper six bits from first byte is used to control LEDs).
 */


#include "CANopen.h"
#include "application.h"
#include "CgiLog.h"


/* constants */
    #define CHIP_SELECT        1                 /* Chip select to use */
    #define OFFSET_LED_WRITE   0                 /* offset of LED output register */
    #define OFFSET_LED_READ    1                 /* offset of LED input register */
    #define OFFSET_DIP_READ    0                 /* offset of dip switch input register */


/* Global variables */
    uint8_t errorRegisterPrev;
    unsigned char *gBaseAddr = NULL;    /* hardware base address */


/* LED diodes. Two LSB bits are CANopen standard diodes. */
    #define CAN_RUN_LED_ON()    gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] | 0x01
    #define CAN_RUN_LED_OFF()   gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] & 0xFE
    #define CAN_ERROR_LED_ON()  gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] | 0x02
    #define CAN_ERROR_LED_OFF() gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] & 0xFD
    #define WRITE_LEDS(val){                                                  \
        gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] & 0x03;      \
        gBaseAddr[OFFSET_LED_WRITE] = gBaseAddr[OFFSET_LED_READ] | (val&0xFC);}


/*
 * Indexes and Error codes for application specific errors. For information on
 * error control see CO_Emergency.h file.
 *
 * 'Error status bits' from 00 to 2F are reserved for use by the CANopen stack.
 * Values 30 to 3F are used here as application informative errors.
 * Values 40 to 4F are used here as application critical errors.
 */
#if ODL_errorStatusBits_stringLength < 10
    #error Error in Object Dictionary!
#endif

typedef enum{
    CO_EMA_TEST1_INFORMATIVE        = 0x30,
    CO_EMA_TEST2_CRITICAL           = 0x40
}CO_EM_ApplicationErrorStatusBits_t;


#ifdef OD_testVar
/*
 * Function - ODF_testDomain
 *
 * Function for accessing _test var_ (index 0x2120) from SDO server.
 *
 * For more information see topic <Object dictionary function>.
 */
    #define ODF_testDomain_index     0x2120
    CO_SDO_abortCode_t ODF_testDomain(CO_ODF_arg_t *ODF_arg);
#endif


/******************************************************************************/
void programStart(void){

    /* Initialize hardware */
    CsSettings csSettings;  /*  Chip select settings data structure */
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


    CAN_RUN_LED_OFF(); CAN_ERROR_LED_ON();
    WRITE_LEDS(0)
}


/******************************************************************************/
void communicationReset(void){
    OD_writeOutput8Bit[0] = 0;
    OD_writeOutput8Bit[1] = 0;
    errorRegisterPrev = 0;

#ifdef OD_testVar
    CO_OD_configure(CO->SDO, ODF_testDomain_index, ODF_testDomain, 0, 0, 0);
#endif

    CAN_RUN_LED_OFF(); CAN_ERROR_LED_OFF();
    WRITE_LEDS(0)
}


/******************************************************************************/
void programEnd(void){
    CAN_RUN_LED_OFF(); CAN_ERROR_LED_OFF();
    WRITE_LEDS(0)
}


/******************************************************************************/
void programAsync(uint16_t timer1msDiff){

    if(LED_GREEN_RUN(CO->NMT))CAN_RUN_LED_ON();   else CAN_RUN_LED_OFF();
    if(LED_RED_ERROR(CO->NMT))CAN_ERROR_LED_ON(); else CAN_ERROR_LED_OFF();

    /* Is any application critical error set? */
    /* If error register is set, device will leave operational state. */
    if(CO->em->errorStatusBits[8] || CO->em->errorStatusBits[9])
        *CO->emPr->errorRegister |= 0x20;

    /* if error register is set, make dump of CAN message log */
    if(!errorRegisterPrev && *CO->emPr->errorRegister){
        extern CgiLog_t *CgiLog;
        CgiLogSaveBuffer(CgiLog);
        errorRegisterPrev = *CO->emPr->errorRegister;
    }
}


/******************************************************************************/
void program1ms(void){

    /* Read RPDO and show it on LEDS on DB240 */
    WRITE_LEDS(OD_writeOutput8Bit[0]);


    /* Verify operating state of this node */
    /* LATAbits.LATA2 = (CO->NMT->operatingState == CO_NMT_OPERATIONAL) ? 1 : 0; */

    /* Verify operating state of monitored nodes */
    /* LATAbits.LATA3 = (CO->HBcons->allMonitoredOperational) ? 1 : 0; */

    /* Example error */
    /* if(errorCondition) CO_errorReport(CO->em, CO_EMA_TEST1_INFORMATIVE, CO_EMC_GENERIC, 0x12345678L); */
    /* if(errorSolved) CO_errorReset(CO->em, CO_EMA_TEST1_INFORMATIVE, 0xAAAAAABBL); */


    /* Prepare TPDO from buttons on Explorer16. */
    /* According to PDO mapping and communication parameters, first TPDO is sent */
    /* automatically on change of state of OD_readInput8Bit[0] variable. */
    OD_readInput8Bit[0] = gBaseAddr[OFFSET_DIP_READ];
}


#ifdef OD_testVar
/******************************************************************************/
/* Function passes some data to SDO server on testDomain variable access */
CO_SDO_abortCode_t ODF_testDomain(CO_ODF_arg_t *ODF_arg){

    /* domain data type is on subIndex 5, nothing to do on other subObjects */
    if(ODF_arg->subIndex != 5) return CO_SDO_AB_NONE;

    /* reading object dictionary */
    if(ODF_arg->reading){
        /* SDO buffer is filled with sequence 0x01, 0x02, ... */
        /* If domainFileSize is greater than SDObufferSize, this function will */
        /* be called multiple times during one SDO communication cycle */

        const uint32_t domainFileSize = 0x500;
        static uint32_t offset = 0;

        uint16_t i;
        uint16_t SDObufferSize = ODF_arg->dataLength;

        /* new SDO read cycle */
        if(ODF_arg->firstSegment){
            ODF_arg->dataLengthTotal = domainFileSize;
            offset = 0;
        }

        /* fill SDO buffer */
        for(i = 0; offset < domainFileSize; i++, offset++){
            if(i >= SDObufferSize){
                /* SDO buffer is full */
                ODF_arg->lastSegment = CO_false;
                break;
            }
            ODF_arg->data[i] = (uint8_t)(offset+1);
        }

        /* all data was copied */
        if(offset == domainFileSize){
            ODF_arg->lastSegment = CO_true;
            ODF_arg->dataLength = i;
        }

        /* return OK */
        return CO_SDO_AB_NONE;
    }

    /* writing object dictionary */
    else{
        uint16_t i;
        uint16_t err = 0;
        uint16_t dataSize = ODF_arg->dataLength;
        static uint32_t offset = 0;

        /* new SDO read cycle */
        if(ODF_arg->firstSegment){
            /* if(ODF_arg->dataLengthTotal) printf("\nWill receive %d bytes of data.\n", ODF_arg->dataLengthTotal); */
            offset = 0;
        }

        /* do something with data, here just verify if they are the same as above */
        for(i=0; i<dataSize; i++, offset++){
            uint8_t b = ODF_arg->data[i];
            if(b != (uint8_t)(offset+1)) err++;
            /* printf("%02X ", b); */
        }

        if(err) return CO_SDO_AB_INVALID_VALUE;

        /* end of transfer */
        /* if(ODF_arg->lastSegment) */
            /* printf("\nReceived %d bytes of data.\n", offset); */

        /* return OK */
        return CO_SDO_AB_NONE;
    }
}
#endif
