/*******************************************************************************

   File: eeprom.h
   Internal eeprom object for Microchip dsPIC30F and Microchip C30 compiler (>= V3.00).

   Copyright (C) 2004-2008 Janez Paternoster

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


#include <p30fxxxx.h>       //processor header file


/*******************************************************************************
   Topic: EEPROM

   Usage of device internal EEPROM for storing RAM data block.

   <eeprom_init> functions initializes <eeprom_t> object and copies memory from
   internal device EEPROM into RAM data block.

   <eeprom_process> function compares EEPROM and RAM values word by word and
   copies one word at a cycle (from RAM to EEPROM) if values are different.

   *It is not possible tu use more than one <eeprom_t> object.*

   Strategy for EEPROM initial values:
   If the first and the last byte in EEPROM are not equal to initial values in
   RAM data block, then EEPROM is not read. This is the case, when the  EEPROM
   is still empty or when the RAM data block structure has changed. In this
   case initial values for the RAM data block are valid and EEPROM will be
   updated from them.
*******************************************************************************/


/*******************************************************************************
   Object: eeprom_t

   Object controls internal error state and sends emergency message.

   Variables:
      tblOffset    - See parameters in <eeprom_init>.
      RAMblock     - See parameters in <eeprom_init>.
      RAMblockSize - See parameters in <eeprom_init>.
      curentIndex  - Index of byte, which is curently processing.
      C_W          - If true, one word in EEPROM was just erased. Next
                     operation in <eeprom_process> will be 'EEPROM write'.
*******************************************************************************/
typedef struct{
   unsigned int         tblOffset;
   unsigned int        *RAMblock;
   unsigned int         RAMblockSize;
   unsigned int         curentIndex;
   unsigned char        C_W;
}eeprom_t;


/*******************************************************************************
   Function: eeprom_init

   Initialize eeprom object.

   Function must be called in the device reset section.

   Parameters:
      eeprom       - Pointer to eeprom object <eeprom_t>.
      tblOffset    - Offset address of the first EEPROM word used.
                     See _Program space memory map_ for specific dsPIC30F device.
                     EEPROM memory is located from address 0x7Fxxxx, where xxxx
                     is tblOffset. (TBLPAG register is always 0x7F.) For dsPIC30F
                     devices with 1 kB EEPROM, starting address for tblOffset is 0xFC00.
      RAMblock     - Pointer to RAM data block to be stored in eeprom.
      RAMblockSize - Size of RAM data block.
*******************************************************************************/
void eeprom_init(    eeprom_t            *eeprom,
                     unsigned int         tblOffset,
                     unsigned int        *RAMblock,
                     unsigned int         RAMblockSize);


/*******************************************************************************
   Function: eeprom_process

   Process eeprom object.

   Function must be called cyclically.

   Parameters:
      eeprom       - Pointer to eeprom object <eeprom_t>.
*******************************************************************************/
void eeprom_process(eeprom_t *eeprom);


/*******************************************************************************
   Function: eeprom_saveAll

   Save all variables to EEPROM.

   Function may be used instead of <eeprom_process> or as addition to it. It
   saves all variables to eeprom. It is blocking function. Interrupts *must* be
   disabled.

   Parameters:
      eeprom       - Pointer to eeprom object <eeprom_t>.
*******************************************************************************/
void eeprom_saveAll(eeprom_t *eeprom);

#endif
