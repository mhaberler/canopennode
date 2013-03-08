/*
 * Eeprom object for Microchip PIC32MX microcontroller.
 *
 * @file        eeprom.h
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


#ifndef _EEPROM_H
#define _EEPROM_H


/* For documentation see file genericDriver/eeprom.h */


/*
 * PIC32MX specific
 *
 * 25LC128 eeprom from Microchip is used connected on SPI2A.
 *
 * Two blocks of CANopen Object Dictionary data are stored in eeprom:
 * OD_EEPROM     - Stored is from eeprom address 0. Data are stored automatically on
 *                 change. No data corruption control is made.
 * OD_ROM        - Stored from upper half eeprom address. Data are protected from
 *                 accidental write, can also be hardware protected. Data integrity
 *                 is verified with CRC.
 *                 Data are stored on special CANopen command - Writing 0x65766173
 *                 into Object dictionary (index 1010, subindex 1). Default values
 *                 are restored after reset, if writing 0x64616F6C into (1011, 1).
 */


/* Constants */
#define EE_SIZE         0x4000
#define EE_PAGE_SIZE    64


/* Master boot record is stored on the last page in eeprom */
typedef struct{
    uint32_t            CRC;            /* CRC code of the OD_ROM block */
    uint32_t            OD_EEPROMSize;  /* Size of OD_EEPROM block */
    uint32_t            OD_ROMSize;     /* Size of OD_ROM block */
}EE_MBR_t;


/* Eeprom object */
typedef struct{
    uint8_t            *OD_EEPROMAddress;
    uint32_t            OD_EEPROMSize;
    uint8_t            *OD_ROMAddress;
    uint32_t            OD_ROMSize;
    uint32_t            OD_EEPROMCurrentIndex;
    uint8_t             OD_EEPROMWriteEnable;

}EE_t;


/* First part of eeprom initialization.
 *
 * Allocate memory for object, configure SPI port for use with 25LCxxx, read
 * eeprom and store to OD_EEPROM and OD_ROM.
 */
int16_t EE_init_1(
        EE_t                  **EE,
        uint8_t                *OD_EEPROMAddress,
        uint32_t                OD_EEPROMSize,
        uint8_t                *OD_ROMAddress,
        uint32_t                OD_ROMSize);


/**
 * Delete EEPROM object and free memory.
 *
 * @param ppEE Pointer to pointer to EEPROM object <EE_t>.
 * Pointer to object is set to 0.
 */
uint32_t CO_ODF_1010(CO_ODF_arg_t *ODF_arg);
uint32_t CO_ODF_1011(CO_ODF_arg_t *ODF_arg);
void EE_delete(EE_t **ppEE);


/* Second part of eeprom initialization. */
#define EE_init_2(EE, EEStatus, SDO, EM)                                   \
    CO_OD_configure(SDO, 0x1010, CO_ODF_1010, (void*)EE);                  \
    CO_OD_configure(SDO, 0x1011, CO_ODF_1011, (void*)EE);                  \
    if(EEStatus) CO_errorReport(EM, ERROR_NON_VOLATILE_MEMORY, EEStatus)


/* Process eeprom object. */
void EE_process(EE_t *EE);


#endif
