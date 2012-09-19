/*******************************************************************************

   File - eeprom.c
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


#include "CO_driver.h"
#include "eeprom.h"


/******************************************************************************/
UNSIGNED32 CO_ODF_1010( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED16 *pLength,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData)
{
   return 0x06020000;   //Object does not exist in the object dictionary.
}


/******************************************************************************/
UNSIGNED32 CO_ODF_1011( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED16 *pLength,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData)
{
   return 0x06020000;   //Object does not exist in the object dictionary.
}


/******************************************************************************/
void eeprom_init(    eeprom_t            *eeprom,
                     unsigned int         tblOffset,
                     unsigned int        *RAMblock,
                     unsigned int         RAMblockSize)
{
   unsigned char EEfirstByte, EElastByte, RAMfirstByte, RAMlastByte;

   eeprom->tblOffset = tblOffset;
   eeprom->RAMblock = RAMblock;
   eeprom->RAMblockSize = RAMblockSize;
   eeprom->curentIndex = 0;
   eeprom->C_W = 0;

   //set page register to EEPROM
   TBLPAG = 0x7F;

   //copy data to RAM if EEPROM is valid.
   EEfirstByte = (unsigned char) __builtin_tblrdl(tblOffset);
   EElastByte = (unsigned char) __builtin_tblrdl(tblOffset + RAMblockSize - 2);
   RAMfirstByte = *((unsigned char*)RAMblock);
   RAMlastByte = *(((unsigned char*)RAMblock) + RAMblockSize - 2);

   if(EEfirstByte == RAMfirstByte && EElastByte == RAMlastByte){
      unsigned int i;
      unsigned int *RAMptr;
      i = RAMblockSize / 2;   //size in in words
      RAMptr = RAMblock + i - 1;  //points to last word in RAMblock
      tblOffset += RAMblockSize;  //points past last word in EEPROM
      for( ; i>0; i--){
         tblOffset -= 2;
         *(RAMptr--) = __builtin_tblrdl(tblOffset);
      }
   }
}


/******************************************************************************/
void eeprom_process(eeprom_t *eeprom){

   //make sure, that previous erase/write to EEPROM is not in progress.
   if(NVMCONbits.WR == 0){
      //set page register to EEPROM
      TBLPAG = 0x7F;

      if(eeprom->C_W == 0){
         //verify next word
         if(eeprom->curentIndex == 0) eeprom->curentIndex = eeprom->RAMblockSize;
         eeprom->curentIndex -= 2;

         //read eeprom
         unsigned int RAMdata = *(eeprom->RAMblock + eeprom->curentIndex/2);
         unsigned int EEdata = __builtin_tblrdl(eeprom->tblOffset + eeprom->curentIndex);

         //if byte in EEPROM is different than in RAM, then write to EEPROM
         if(EEdata != RAMdata){
            int old_ipl;

            //Erase EEPROM word
            NVMADRU = 0x7F;
            NVMADR = eeprom->tblOffset + eeprom->curentIndex;
            NVMCON = 0x4044;
            SET_AND_SAVE_CPU_IPL(old_ipl, 7);   //disable interrupts
            __builtin_write_NVM();
            RESTORE_CPU_IPL(old_ipl);           //enable interrupts
            //Write will be performed in one of the next cycles
            eeprom->C_W = 1;
         }
      }
      else{
         //Write EEPROM word (EEPROM was previously erased)
         int old_ipl;
         unsigned int RAMdata = *(eeprom->RAMblock + eeprom->curentIndex/2);
         __builtin_tblwtl(eeprom->tblOffset + eeprom->curentIndex, RAMdata);

         NVMCON = 0x4004;
         SET_AND_SAVE_CPU_IPL(old_ipl, 7);   //disable interrupts
         __builtin_write_NVM();
         RESTORE_CPU_IPL(old_ipl);           //enable interrupts
         eeprom->C_W = 0;    //reset write flag bit
      }
   }
}


/******************************************************************************/
void eeprom_saveAll(eeprom_t *eeprom){
   unsigned int i;

   //set page register to EEPROM
   TBLPAG = 0x7F;

   for(i = 0; i < eeprom->RAMblockSize; i += 2){
      //read RAM and eeprom
      unsigned int RAMdata = *(eeprom->RAMblock + i/2);
      unsigned int EEdata = __builtin_tblrdl(eeprom->tblOffset + i);

      //wait while previous erase/write to EEPROM is in progress.
      while(NVMCONbits.WR) ClrWdt();

      //if byte in EEPROM is different than in RAM, then write to EEPROM
      if(EEdata != RAMdata){
         //Erase EEPROM word
         NVMADRU = 0x7F;
         NVMADR = eeprom->tblOffset + i;
         NVMCON = 0x4044;
         __builtin_write_NVM();

         while(NVMCONbits.WR) ClrWdt();

         //Write EEPROM word
         __builtin_tblwtl(eeprom->tblOffset + i, RAMdata);
         NVMCON = 0x4004;
         __builtin_write_NVM();
      }
   }
}
