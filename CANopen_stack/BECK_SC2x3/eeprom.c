/*******************************************************************************

   File - eeprom.c
   Object for retentive storage of Object Dictionary.

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


#include "eeprom.h"

#include <string.h> // for memcpy
#include <stdlib.h> // for malloc, free

/******************************************************************************/
UNSIGNED32 CO_ODF_1010( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED8   length,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData)
{
   EE_t *EE = (EE_t*) object; //this is the correct pointer type of the first argument

   if(dir == 1){ //Writing Object Dictionary variable
      if(subIndex == 1){
         UNSIGNED32 val;
         memcpySwap4((UNSIGNED8*)&val, (UNSIGNED8*)dataBuff);
         if(val == 0x65766173){
            //store parameters
            //rename current file to .old
            remove("OD_ROM01.old");
            rename("OD_ROM01.dat", "OD_ROM01.old");
            //open a file
            FILE *fp = fopen("OD_ROM01.dat", "wb");
            if(!fp) return 0x06060000;   //Access failed due to an hardware error.

            //write data to the file
            fwrite((const void *)EE->OD_ROMAddress, 1, EE->OD_ROMSize, fp);
            //write CRC to the end of the file
            UNSIGNED16 CRC = CRC16(EE->OD_ROMAddress, EE->OD_ROMSize);
            fwrite((const void *)&CRC, 1, 2, fp);
            fclose(fp);
            //verify data
            void *buf = malloc(EE->OD_ROMSize + 4);
            if(buf){
               fp = fopen("OD_ROM01.dat", "rb");
               UNSIGNED32 cnt = 0;
               UNSIGNED16 CRC2 = 0;
               if(fp){
                  cnt = fread(buf, 1, EE->OD_ROMSize, fp);
                  CRC2 = CRC16((UNSIGNED8 *)buf, EE->OD_ROMSize);
                  //read also two bytes of CRC
                  cnt += fread(buf, 1, 4, fp);
                  fclose(fp);
               }
               free(buf);
               if(cnt == (EE->OD_ROMSize + 2) && CRC == CRC2){
                  //write successfull
                  return 0;
               }
            }
            return 0x06060000;   //Access failed due to an hardware error.
         }
         else
            return 0x08000020; //Data cannot be transferred or stored to the application.
      }
   }

   return CO_ODF(object, index, subIndex, length, attribute, dir, dataBuff, pData);
}


/******************************************************************************/
UNSIGNED32 CO_ODF_1011( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED8   length,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData)
{
   if(dir == 1){ //Writing Object Dictionary variable
      if(subIndex == 1){
         UNSIGNED32 val;
         memcpySwap4((UNSIGNED8*)&val, (UNSIGNED8*)dataBuff);
         if(val == 0x64616F6C){
            //restore default parameters
            //rename current file to .old, so it no longer exist
            remove("OD_ROM01.old");
            rename("OD_ROM01.dat", "OD_ROM01.old");
            return 0;
         }
         else
            return 0x08000020; //Data cannot be transferred or stored to the application.
      }
   }

   return CO_ODF(object, index, subIndex, length, attribute, dir, dataBuff, pData);
}


/******************************************************************************/
INTEGER16 EE_init_1(
      EE_t            **ppEE,
      UNSIGNED8        *OD_EEPROMAddress,
      UNSIGNED32        OD_EEPROMSize,
      UNSIGNED8        *OD_ROMAddress,
      UNSIGNED32        OD_ROMSize)
{
   //allocate memory if not already allocated
   if((*ppEE) == NULL){
      if(((*ppEE) = (EE_t*)malloc(sizeof(EE_t))) == NULL){return CO_ERROR_OUT_OF_MEMORY;}
   }

   EE_t *EE = *ppEE; //pointer to (newly created) object

   //configure object variables
   EE->OD_EEPROMAddress = (UNSIGNED32*) OD_EEPROMAddress;
   EE->OD_EEPROMSize = OD_EEPROMSize / 4;
   EE->OD_ROMAddress = OD_ROMAddress;
   EE->OD_ROMSize = OD_ROMSize;
   EE->OD_EEPROMCurrentIndex = 0;

   //initialize SRAM
   if(RTX_MemWindow(gSysPublicData.sramAddr, gSysPublicData.sramSize))
      return CO_ERROR_OUT_OF_MEMORY;
   EE->pSRAM = gSysPublicData.sramAddr;

   //read the CO_OD_EEPROM from SRAM, first verify, if data are OK
   UNSIGNED32 firstWordRAM = *(EE->OD_EEPROMAddress);
   UNSIGNED32 firstWordEE = *(EE->pSRAM);
   UNSIGNED32 lastWordEE = *(EE->pSRAM + EE->OD_EEPROMSize - 1);
   if(firstWordRAM == firstWordEE && firstWordRAM == lastWordEE){
      unsigned int i;
      for(i=0; i<EE->OD_EEPROMSize; i++)
         (EE->OD_EEPROMAddress)[i] = (EE->pSRAM)[i];
   }

   //read the CO_OD_ROM from file and verify CRC
   void *buf = malloc(EE->OD_ROMSize);
   if(buf){
      INTEGER16 ret = CO_ERROR_NO;
      FILE *fp = fopen("OD_ROM01.dat", "rb");
      UNSIGNED32 cnt = 0;
      UNSIGNED16 CRC[2];
      if(fp){
         cnt = fread(buf, 1, EE->OD_ROMSize, fp);
         //read also two bytes of CRC from file
         cnt += fread(&CRC[0], 1, 4, fp);
         CRC[1] = CRC16((UNSIGNED8 *)buf, EE->OD_ROMSize);
         fclose(fp);
      }
      if(cnt != (EE->OD_ROMSize + 2)){
         ret = CO_ERROR_DATA_CORRUPT; //file length does not match
      }
      else if(CRC[0] != CRC[1]){
         ret = CO_ERROR_CRC;       //CRC does not match
      }

      if(!ret){
         //copy data into object dictionary
         memcpy(EE->OD_ROMAddress, buf, EE->OD_ROMSize);
      }
      free(buf);
      return ret;
   }
   else return CO_ERROR_OUT_OF_MEMORY;

   return CO_ERROR_NO;
}


/******************************************************************************/
void EE_delete(EE_t **ppEE){
   if(*ppEE){
      free(*ppEE);
      *ppEE = 0;
   }
}


/******************************************************************************/
void EE_process(EE_t *EE){
   //verify next word
   unsigned int i = EE->OD_EEPROMCurrentIndex;
   if(++i == EE->OD_EEPROMSize) i = 0;
   EE->OD_EEPROMCurrentIndex = i;

   //update SRAM
   (EE->pSRAM)[i] = (EE->OD_EEPROMAddress)[i];
}


/******************************************************************************/
/* CRC function ***************************************************************/
/******************************************************************************/
const UNSIGNED8 CRC16TabLo[256]={
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
                0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40};

const UNSIGNED8 CRC16TabHi[256]={
                0x00,0xc0,0xc1,0x01,0xc3,0x03,0x02,0xc2,
                0xc6,0x06,0x07,0xc7,0x05,0xc5,0xc4,0x04,
                0xcc,0x0c,0x0d,0xcd,0x0f,0xcf,0xce,0x0e,
                0x0a,0xca,0xcb,0x0b,0xc9,0x09,0x08,0xc8,
                0xd8,0x18,0x19,0xd9,0x1b,0xdb,0xda,0x1a,
                0x1e,0xde,0xdf,0x1f,0xdd,0x1d,0x1c,0xdc,
                0x14,0xd4,0xd5,0x15,0xd7,0x17,0x16,0xd6,
                0xd2,0x12,0x13,0xd3,0x11,0xd1,0xd0,0x10,
                0xf0,0x30,0x31,0xf1,0x33,0xf3,0xf2,0x32,
                0x36,0xf6,0xf7,0x37,0xf5,0x35,0x34,0xf4,
                0x3c,0xfc,0xfd,0x3d,0xff,0x3f,0x3e,0xfe,
                0xfa,0x3a,0x3b,0xfb,0x39,0xf9,0xf8,0x38,
                0x28,0xe8,0xe9,0x29,0xeb,0x2b,0x2a,0xea,
                0xee,0x2e,0x2f,0xef,0x2d,0xed,0xec,0x2c,
                0xe4,0x24,0x25,0xe5,0x27,0xe7,0xe6,0x26,
                0x22,0xe2,0xe3,0x23,0xe1,0x21,0x20,0xe0,
                0xa0,0x60,0x61,0xa1,0x63,0xa3,0xa2,0x62,
                0x66,0xa6,0xa7,0x67,0xa5,0x65,0x64,0xa4,
                0x6c,0xac,0xad,0x6d,0xaf,0x6f,0x6e,0xae,
                0xaa,0x6a,0x6b,0xab,0x69,0xa9,0xa8,0x68,
                0x78,0xb8,0xb9,0x79,0xbb,0x7b,0x7a,0xba,
                0xbe,0x7e,0x7f,0xbf,0x7d,0xbd,0xbc,0x7c,
                0xb4,0x74,0x75,0xb5,0x77,0xb7,0xb6,0x76,
                0x72,0xb2,0xb3,0x73,0xb1,0x71,0x70,0xb0,
                0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,
                0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,
                0x9c,0x5c,0x5d,0x9d,0x5f,0x9f,0x9e,0x5e,
                0x5a,0x9a,0x9b,0x5b,0x99,0x59,0x58,0x98,
                0x88,0x48,0x49,0x89,0x4b,0x8b,0x8a,0x4a,
                0x4e,0x8e,0x8f,0x4f,0x8d,0x4d,0x4c,0x8c,
                0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,
                0x82,0x42,0x43,0x83,0x41,0x81,0x80,0x40};


UNSIGNED16 CRC16(UNSIGNED8 *str, UNSIGNED16 len){
   UNSIGNED8 CRC16Lo = 0, CRC16Hi = 0;
   UNSIGNED16 i;

   for(i=0; i<len; i++){
      UNSIGNED8 t = CRC16Lo ^ str[i];
      CRC16Lo = CRC16TabLo[t] ^ CRC16Hi;
      CRC16Hi = CRC16TabHi[t];
   }

   return (UNSIGNED16)CRC16Hi << 8 | CRC16Lo;
}
