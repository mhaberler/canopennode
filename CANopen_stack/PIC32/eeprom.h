/*******************************************************************************

   File: eeprom.h
   Internal eeprom object for Microchip PIC32MX microcontroller.

   Copyright (C) 2004-2010 Janez Paternoster

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

#ifndef _EEPROM_H
#define _EEPROM_H


/*******************************************************************************
   Topic: EEPROM

   Usage of device internal EEPROM for storing non-volatile variables.

   25LC128 eeprom from Microchip is used connected on SPI2A.

   Two blocks of CANopen Object Dictionary data are stored in eeprom:
   OD_EEPROM - Stored is from eeprom address 0. Data are stored automatically on
               change. No data corruption control is made.
   OD_ROM    - Stored from upper half eeprom address. Data are protected from
               accidental write, can also be hardware protected. Data integrity
               is verified with CRC.
               Data are stored on special CANopen command - Writing 0x65766173
               into Object dictionary (index 1010, subindex 1). Default values
               are restored after reset, if writing 0x64616F6C into (1011, 1).
*******************************************************************************/


/*******************************************************************************
   Constants: eeprom

   EE_SIZE        - Size of eeprom chip in bytes. For 25LC128 size is 0x4000 bytes.
   EE_PAGE_SIZE   - Page size in eeprom chip. For 25LC128 page size is 64 bytes.
*******************************************************************************/
#define EE_SIZE         0x4000
#define EE_PAGE_SIZE    64


/*******************************************************************************
   Object: EE_MBR_t

   Master boot record is stored on the last page in eeprom

   Variables:
      CRC            - CRC code of the OD_ROM block.
      OD_EEPROMSize  - Size of OD_EEPROM block.
      OD_ROMSize     - Size of OD_ROM block.
*******************************************************************************/
typedef struct{
   UNSIGNED32     CRC;
   UNSIGNED32     OD_EEPROMSize;
   UNSIGNED32     OD_ROMSize;
}EE_MBR_t;


/*******************************************************************************
   Object: EE_t

   Variables for eeprom object.

   Variables:
      OD_EEPROMAddress        - See parameters in <EE_init_1>.
      OD_EEPROMSize           - See parameters in <EE_init_1>.
      OD_ROMAddress           - See parameters in <EE_init_1>.
      OD_ROMSize              - See parameters in <EE_init_1>.
      OD_EEPROMCurrentIndex   - Internal variable controls the OD_EEPROM vrite.
      OD_EEPROMWriteEnable    - Writing to EEPROM is enabled.
*******************************************************************************/
typedef struct{
   UNSIGNED8     *OD_EEPROMAddress;
   UNSIGNED32     OD_EEPROMSize;
   UNSIGNED8     *OD_ROMAddress;
   UNSIGNED32     OD_ROMSize;
   UNSIGNED32     OD_EEPROMCurrentIndex;
   UNSIGNED8      OD_EEPROMWriteEnable;
}EE_t;


/*******************************************************************************
   Function: CO_ODF_1010

   Function for accessing _Store parameters_ (index 0x1010) from SDO server.

   For more information see topic <Object dictionary function>.
*******************************************************************************/
UNSIGNED32 CO_ODF_1010(CO_ODF_arg_t *ODF_arg);


/*******************************************************************************
   Function: CO_ODF_1011

   Function for accessing _Restore default parameters_ (index 0x1011) from SDO server.

   For more information see topic <Object dictionary function>.
*******************************************************************************/
UNSIGNED32 CO_ODF_1011(CO_ODF_arg_t *ODF_arg);


/*******************************************************************************
   Function: EE_init_1

   First part of eeprom initialization. Called once after microcontroller reset.

   Allocate memory for object, configure SPI port for use with 25LCxxx, read
   eeprom and store to OD_EEPROM and OD_ROM.

   Variables:
      ppEE                    - Pointer to address of eeprom object <EE_t>.
      OD_EEPROMAddress        - Address of OD_EEPROM structure from object dictionary.
      OD_EEPROMSize           - Size of OD_EEPROM structure from object dictionary.
      OD_ROMAddress           - Address of OD_ROM structure from object dictionary.
      OD_ROMSize              - Size of OD_ROM structure from object dictionary.

   Return:
      CO_ERROR_NO             - Operation completed successfully.
      CO_ERROR_OUT_OF_MEMORY  - Memory allocation failed.
      CO_ERROR_DATA_CORRUPT   - Data in eeprom corrupt.
      CO_ERROR_CRC            - CRC from MBR does not match the CRC of OD_ROM block in eeprom.
*******************************************************************************/
INTEGER16 EE_init_1(
      EE_t            **ppEE,
      UNSIGNED8        *OD_EEPROMAddress,
      UNSIGNED32        OD_EEPROMSize,
      UNSIGNED8        *OD_ROMAddress,
      UNSIGNED32        OD_ROMSize);


/*******************************************************************************
   Function: EE_delete

   Delete EEPROM object and free memory.

   Parameters:
      ppEE              - Pointer to pointer to EEPROM object <EE_t>.
                          Pointer to object is set to 0.
*******************************************************************************/
void EE_delete(EE_t **ppEE);


/*******************************************************************************
   Function: EE_init_2

   Second part of eeprom initialization. Called after CANopen communication reset.

   Call functions CO_OD_configureArgumentForODF() and CO_errorReport() if necessary.

   Variables:
      EE          - Pointer eeprom object <EE_t>.
      EEStatus    - Return value from <EE_init_1>.
      SDO         - Pointer to SDO object <CO_SDO_t>.
      EM          - Pointer to Emergency object <CO_emergencyReport_t>.
*******************************************************************************/
#define EE_init_2(EE, EEStatus, SDO, EM)                                   \
   CO_OD_configure(SDO, 0x1010, CO_ODF_1010, (void*)EE);                   \
   CO_OD_configure(SDO, 0x1011, CO_ODF_1010, (void*)EE);                   \
   if(EEStatus) CO_errorReport(EM, ERROR_NON_VOLATILE_MEMORY, EEStatus)


/*******************************************************************************
   Function: EE_process

   Process eeprom object.

   Function must be called cyclically. It strores variables from OD_EEPROM data
   block into eeprom byte by byte (only if values are different).

   Parameters:
      EE       - Pointer to eeprom object <EE_t>.
*******************************************************************************/
void EE_process(EE_t *EE);


/*******************************************************************************
   Function: EE_writeByteNoWait

   Write one byte of data to eeprom. It triggers write, but it does not wait for
   write cycle to complete. Before next write cycle, <EE_isWriteInProcess> must
   be checked.

   Parameters:
      data     - Data byte to be written.
      addr     - Address in eeprom, where data will be written.
*******************************************************************************/
void EE_writeByteNoWait(UNSIGNED8 data, UNSIGNED32 addr);


/*******************************************************************************
   Function: EE_readByte

   Read one byte of data from eeprom.

   Parameters:
      addr     - Address in eeprom, where data will be written.

   Return:
      data     - Data byte read.
*******************************************************************************/
UNSIGNED8 EE_readByte(UNSIGNED32 addr);


/*******************************************************************************
   Function: EE_writeBlock

   Write block of data to eeprom. It is blockung function, so it waits, untill
   all data are written.

   Parameters:
      data     - Pointer to data to be written.
      addr     - Address in eeprom, where data will be written. *If data are
                 stored accross multiple pages, address must be aligned with page.*
      len      - Length of the data block.
*******************************************************************************/
void EE_writeBlock(UNSIGNED8 *data, UNSIGNED32 addr, UNSIGNED32 len);


/*******************************************************************************
   Function: EE_readBlock

   Read block of data from eeprom.

   Parameters:
      data     - Pointer to data buffer, where data will be stored.
      addr     - Address in eeprom, from where data will be read.
      len      - Length of the data block to be read.
*******************************************************************************/
void EE_readBlock(UNSIGNED8 *data, UNSIGNED32 addr, UNSIGNED32 len);


/*******************************************************************************
   Function: EE_verifyBlock

   Compare block of data with data stored in eeprom.

   Parameters:
      data     - Pointer to data buffer, which will be compared.
      addr     - Address of data in eeprom, which will be compared.
      len      - Length of the data block to be compared.

   Return:
      0 - comparision failed.
      1 - data are equal.
*******************************************************************************/
UNSIGNED8 EE_verifyBlock(UNSIGNED8 *data, UNSIGNED32 addr, UNSIGNED32 len);


/*******************************************************************************
   Function: EE_writeStatus

   Write eeprom status register.

   Parameters:
      data     - Data byte to be written to status register.
*******************************************************************************/
void EE_writeStatus(UNSIGNED8 data);


/*******************************************************************************
   Function: EE_readStatus

   Read eeprom status register.

   Return:
      Data read from status register.
*******************************************************************************/
UNSIGNED8 EE_readStatus();


/*******************************************************************************
   Function: EE_isWriteInProcess

   Verify, if eeprom write is in process.

   Return:
      True if write is in process.
*******************************************************************************/
#define EE_isWriteInProcess()   (EE_readStatus() & 0x01)


#endif
