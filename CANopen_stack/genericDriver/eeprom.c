/**
 * Microcontroller specific code for CANopenNode nonvolatile variables.
 *
 * This file is a template for other microcontrollers.
 *
 * @file        eeprom.h
 * @ingroup     CO_eeprom
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


#include "CO_driver.h"
#include "CO_SDO.h"
#include "CO_Emergency.h"
#include "eeprom.h"
#include "crc16-ccitt.h"


/**
 * OD function for accessing _Store parameters_ (index 0x1010) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
static uint32_t CO_ODF_1010(CO_ODF_arg_t *ODF_arg){
    CO_EE_t *ee;
    uint32_t *value;

    ee = (CO_EE_t*) ODF_arg->object;
    value = (uint32_t*) ODF_arg->data;

    if(!ODF_arg->reading){
        /* don't change the old value */
        uint32_t valueCopy = *value;
        uint32_t *valueOld = (uint32_t*) ODF_arg->ODdataStorage;
        *value = *valueOld;

        if(ODF_arg->subIndex == 1){
            if(valueCopy == 0x65766173){
                /* write ee->OD_ROMAddress, ee->OD_ROMSize to eeprom (blocking function) */

                /* verify data */
                if(1){
                    /* write successfull */
                    return 0;
                }
                return 0x06060000;   /* Access failed due to an hardware error. */
            }
            else
                return 0x08000020; /* Data cannot be transferred or stored to the application. */
        }
    }

    return 0;
}


/**
 * OD function for accessing _Restore default parameters_ (index 0x1011) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
static uint32_t CO_ODF_1011(CO_ODF_arg_t *ODF_arg){
    CO_EE_t *ee;
    uint32_t *value;

    ee = (CO_EE_t*) ODF_arg->object;
    value = (uint32_t*) ODF_arg->data;

    if(!ODF_arg->reading){
        /* don't change the old value */
        uint32_t valueCopy = *value;
        uint32_t *valueOld = (uint32_t*) ODF_arg->ODdataStorage;
        *value = *valueOld;

        if(ODF_arg->subIndex >= 1){
            if(valueCopy == 0x64616F6C){


                /* Clear the eeprom */
                return 0;
            }
            else
                return 0x08000020; /* Data cannot be transferred or stored to the application. */
        }
    }

    return 0;
}


/******************************************************************************/
int16_t CO_EE_init_1(
        CO_EE_t                *ee,
        uint8_t                *OD_EEPROMAddress,
        uint32_t                OD_EEPROMSize,
        uint8_t                *OD_ROMAddress,
        uint32_t                OD_ROMSize)
{

    /* Configure eeprom */


    /* configure object variables */
    ee->OD_EEPROMAddress = OD_EEPROMAddress;
    ee->OD_EEPROMSize = OD_EEPROMSize;
    ee->OD_ROMAddress = OD_ROMAddress;
    ee->OD_ROMSize = OD_ROMSize;
    ee->OD_EEPROMCurrentIndex = 0;
    ee->OD_EEPROMWriteEnable = 0;

    /* read the CO_OD_EEPROM from EEPROM, first verify, if data are OK */

    /* read the CO_OD_ROM from EEPROM and verify CRC */

    return CO_ERROR_NO;
}


/******************************************************************************/
void CO_EE_init_2(
        CO_EE_t                *ee,
        int16_t                 eeStatus,
        CO_SDO_t               *SDO,
        CO_EM_t                *em)
{
    CO_OD_configure(SDO, 0x1010, CO_ODF_1010, (void*)ee, 0, 0);
    CO_OD_configure(SDO, 0x1011, CO_ODF_1011, (void*)ee, 0, 0);
    if(eeStatus) CO_errorReport(em, CO_EM_NON_VOLATILE_MEMORY, CO_EMC_HARDWARE, eeStatus);
}


/******************************************************************************/
void CO_EE_process(CO_EE_t *ee){
    if(ee && ee->OD_EEPROMWriteEnable/* && !isWriteInProcess()*/){
        /* verify next word */
        if(++ee->OD_EEPROMCurrentIndex == ee->OD_EEPROMSize) ee->OD_EEPROMCurrentIndex = 0;
        unsigned int i = ee->OD_EEPROMCurrentIndex;

        /* read eeprom */
        uint8_t RAMdata = ee->OD_EEPROMAddress[i];
        uint8_t EEdata = 0/*EE_readByte(i)*/;

        /* if bytes in EEPROM and in RAM are different, then write to EEPROM */
        if(EEdata != RAMdata){
            /* EE_writeByteNoWait(RAMdata, i);*/
        }
    }
}
