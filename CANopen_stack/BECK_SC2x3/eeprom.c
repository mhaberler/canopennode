/*
 * Eeprom object for BECK SC243 computer.
 *
 * @file        eeprom.c
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

#include <string.h>     /* for memcpy */
#include <stdlib.h>     /* for malloc, free */

/******************************************************************************/
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
                /* store parameters */
                /* rename current file to .old */
                remove(EE_ROM_FILENAME_OLD);
                rename(EE_ROM_FILENAME, EE_ROM_FILENAME_OLD);
                /* open a file */
                FILE *fp = fopen(EE_ROM_FILENAME, "wb");
                if(!fp){
                    rename(EE_ROM_FILENAME_OLD, EE_ROM_FILENAME);
                    return 0x06060000;   /* Access failed due to an hardware error. */
                }

                /* write data to the file */
                fwrite((const void *)ee->OD_ROMAddress, 1, ee->OD_ROMSize, fp);
                /* write CRC to the end of the file */
                uint16_t CRC = crc16_ccitt((unsigned char*)ee->OD_ROMAddress, ee->OD_ROMSize, 0);
                fwrite((const void *)&CRC, 1, 2, fp);
                fclose(fp);

                /* verify data */
                void *buf = malloc(ee->OD_ROMSize + 4);
                if(buf){
                    fp = fopen(EE_ROM_FILENAME, "rb");
                    uint32_t cnt = 0;
                    uint16_t CRC2 = 0;
                    if(fp){
                        cnt = fread(buf, 1, ee->OD_ROMSize, fp);
                        CRC2 = crc16_ccitt((unsigned char*)buf, ee->OD_ROMSize, 0);
                        /* read also two bytes of CRC */
                        cnt += fread(buf, 1, 4, fp);
                        fclose(fp);
                    }
                    free(buf);
                    if(cnt == (ee->OD_ROMSize + 2) && CRC == CRC2){
                        /* write successful */
                        return 0;
                    }
                }
                /* error, set back the old file */
                remove(EE_ROM_FILENAME);
                rename(EE_ROM_FILENAME_OLD, EE_ROM_FILENAME);

                return 0x06060000;   /* Access failed due to an hardware error. */
            }
            else
                return 0x08000020; /* Data cannot be transferred or stored to the application. */
        }
    }

    return 0;
}


/******************************************************************************/
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
                /* restore default parameters */
                /* rename current file to .old, so it no longer exist */
                remove(EE_ROM_FILENAME_OLD);
                rename(EE_ROM_FILENAME, EE_ROM_FILENAME_OLD);

                /* create an empty file */
                FILE *fp = fopen(EE_ROM_FILENAME, "wt");
                if(!fp){
                    rename(EE_ROM_FILENAME_OLD, EE_ROM_FILENAME);
                    return 0x06060000;   /* Access failed due to an hardware error. */
                }
                /* write one byte '-' to the file */
                fputc('-', fp);
                fclose(fp);

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
        uint8_t                *SRAMAddress,
        uint8_t                *OD_EEPROMAddress,
        uint32_t                OD_EEPROMSize,
        uint8_t                *OD_ROMAddress,
        uint32_t                OD_ROMSize)
{

    /* configure object variables */
    ee->pSRAM = (uint32_t*)SRAMAddress;
    ee->OD_EEPROMAddress = (uint32_t*) OD_EEPROMAddress;
    ee->OD_EEPROMSize = OD_EEPROMSize / 4;
    ee->OD_ROMAddress = OD_ROMAddress;
    ee->OD_ROMSize = OD_ROMSize;
    ee->OD_EEPROMCurrentIndex = 0;
    ee->OD_EEPROMWriteEnable = 0;

    /* read the CO_OD_EEPROM from SRAM, first verify, if data are OK */
    if(ee->pSRAM == 0) return CO_ERROR_OUT_OF_MEMORY;
    uint32_t firstWordRAM = *(ee->OD_EEPROMAddress);
    uint32_t firstWordEE = *(ee->pSRAM);
    uint32_t lastWordEE = *(ee->pSRAM + ee->OD_EEPROMSize - 1);
    if(firstWordRAM == firstWordEE && firstWordRAM == lastWordEE){
        unsigned int i;
        for(i=0; i<ee->OD_EEPROMSize; i++)
            (ee->OD_EEPROMAddress)[i] = (ee->pSRAM)[i];
    }
    ee->OD_EEPROMWriteEnable = 1;

    /* read the CO_OD_ROM from file and verify CRC */
    void *buf = malloc(ee->OD_ROMSize);
    if(buf){
        int16_t ret = CO_ERROR_NO;
        FILE *fp = fopen(EE_ROM_FILENAME, "rb");
        uint32_t cnt = 0;
        uint16_t CRC[2];
        if(fp){
            cnt = fread(buf, 1, ee->OD_ROMSize, fp);
            /* read also two bytes of CRC from file */
            cnt += fread(&CRC[0], 1, 4, fp);
            CRC[1] = crc16_ccitt((unsigned char*)buf, ee->OD_ROMSize, 0);
            fclose(fp);
        }

        if(cnt == 1 && *((char*)buf) == '-'){
            ret = CO_ERROR_NO; /* file is empty, default values will be used, no error */
        }
        else if(cnt != (ee->OD_ROMSize + 2)){
            ret = CO_ERROR_DATA_CORRUPT; /* file length does not match */
        }
        else if(CRC[0] != CRC[1]){
            ret = CO_ERROR_CRC;       /* CRC does not match */
        }
        else{
            /* no errors, copy data into object dictionary */
            memcpy(ee->OD_ROMAddress, buf, ee->OD_ROMSize);
        }

        free(buf);
        return ret;
    }
    else return CO_ERROR_OUT_OF_MEMORY;

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
    if(ee && ee->OD_EEPROMWriteEnable){
        /* verify next word */
        unsigned int i = ee->OD_EEPROMCurrentIndex;
        if(++i == ee->OD_EEPROMSize) i = 0;
        ee->OD_EEPROMCurrentIndex = i;

        /* update SRAM */
        (ee->pSRAM)[i] = (ee->OD_EEPROMAddress)[i];
  }
}
