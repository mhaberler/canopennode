/*******************************************************************************

   File - memcpyram2flash.c
   Internal flash write for Microchip dsPIC30F and Microchip C30 compiler (>= V3.00).

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


#include <p30fxxxx.h>
#include <string.h>


/******************************************************************************/
void memcpyram2flash(const void* dest, void* src, unsigned int memsize){
   unsigned int romTemp[32];
   unsigned char romTempHi[32];
   unsigned int TBL_offset;
   unsigned char offsetInRow;
   int old_ipl;               //backup variable for interrupt priority level

   //Disable all interrupts
   SET_AND_SAVE_CPU_IPL(old_ipl, 7);

   //Set table address pointers to point to destination address (PSV window is default)
   TBLPAG = PSVPAG >> 1;
   TBL_offset = (unsigned int) dest;
   TBL_offset &= 0x7FFF;
   TBL_offset |= (PSVPAG&0x0001) << 15;

   offsetInRow = (unsigned char)TBL_offset & 0x3F;  //offset in first row
   TBL_offset -= offsetInRow;                       //TBL_offset points now to the beginning of the row

   while(memsize){
      unsigned char i;
      unsigned char curentDataSize;

      //copy row from program memory into RAM
      for(i=0; i<32; i++){
         romTemp[i] = __builtin_tblrdl(TBL_offset+i*2);
         romTempHi[i] = __builtin_tblrdh(TBL_offset+i*2);
      }

      //determine size of data to be copied into row
      curentDataSize = 64 - offsetInRow;
      if(curentDataSize > memsize) curentDataSize = memsize;

      //update row with new data
      memcpy(((void*)romTemp)+offsetInRow, src, curentDataSize);

      //wait if write to EEPROM is curently in progress
      while(NVMCONbits.WR);

      //erase row
      NVMCON = 0x4041;
      NVMADRU = TBLPAG;
      NVMADR = TBL_offset;
      __builtin_write_NVM();

      //load write latches
      for(i=0; i<32; i++){
         __builtin_tblwtl(TBL_offset+i*2, romTemp[i]);
         __builtin_tblwth(TBL_offset+i*2, romTempHi[i]);
      }

      //write row
      NVMCON = 0x4001;
      __builtin_write_NVM();

      //prepare variables for next row
      memsize -= curentDataSize;
      src += curentDataSize;
      offsetInRow = 0;   //offset is 0 for following rows
      TBL_offset += 64;
   }

   //restore interrupts
   RESTORE_CPU_IPL(old_ipl);

}
