/*******************************************************************************

   File - eeprom.c
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


#include "eeprom.h"


/******************************************************************************/
#define EE_SS_TRIS()    TRISGCLR = 0x0200
#define EE_SSLow()      PORTGCLR = 0x0200
#define EE_SSHigh()     PORTGSET = 0x0200
#define SPIBUF          SPI2ABUF
#define SPICON          SPI2ACON
#define SPISTAT         SPI2ASTAT
#define SPISTATbits     SPI2ASTATbits
#define SPIBRG          SPI2ABRG


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
            EE_MBR_t MBR;
            //unprotect data
            EE_writeStatus(0);
            //write to eeprom (blocking function)
            EE_writeBlock(EE->OD_ROMAddress, EE_SIZE/2, EE->OD_ROMSize);
            //write CRC to the last page in eeprom
            MBR.CRC = CRC16(EE->OD_ROMAddress, EE->OD_ROMSize);
            MBR.OD_EEPROMSize = EE->OD_EEPROMSize;
            MBR.OD_ROMSize = EE->OD_ROMSize;
            EE_writeBlock((UNSIGNED8*)&MBR, EE_SIZE - EE_PAGE_SIZE, sizeof(MBR));
            //protect data
            EE_writeStatus(0x88);
            //verify data and MBR and status register
            if(   EE_verifyBlock(EE->OD_ROMAddress, EE_SIZE/2, EE->OD_ROMSize) == 1
               && EE_verifyBlock((UNSIGNED8*)&MBR, EE_SIZE - EE_PAGE_SIZE, sizeof(MBR)) == 1
               && (EE_readStatus()&0x8C) == 0x88){
               //write successfull
               return 0;
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
   EE_t *EE = (EE_t*) object; //this is the correct pointer type of the first argument

   if(dir == 1){ //Writing Object Dictionary variable
      if(subIndex == 1){
         UNSIGNED32 val;
         memcpySwap4((UNSIGNED8*)&val, (UNSIGNED8*)dataBuff);
         if(val == 0x64616F6C){
            //restore default parameters
            EE_MBR_t MBR;
            //unprotect data
            EE_writeStatus(0);
            //write special code to the last page in eeprom
            MBR.CRC = 0;
            MBR.OD_EEPROMSize = EE->OD_EEPROMSize;
            MBR.OD_ROMSize = 0;
            EE_writeBlock((UNSIGNED8*)&MBR, EE_SIZE - EE_PAGE_SIZE, sizeof(MBR));
            //protect data
            EE_writeStatus(0x88);
            //verify MBR and status register
            if(   EE_verifyBlock((UNSIGNED8*)&MBR, EE_SIZE - EE_PAGE_SIZE, sizeof(MBR)) == 1
               && (EE_readStatus()&0x8C) == 0x88){
               //write successfull
               return 0;
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
INTEGER16 EE_init_1(
      EE_t            **ppEE,
      UNSIGNED8        *OD_EEPROMAddress,
      UNSIGNED32        OD_EEPROMSize,
      UNSIGNED8        *OD_ROMAddress,
      UNSIGNED32        OD_ROMSize)
{
   UNSIGNED32 rdata;

   //allocate memory if not already allocated
   if((*ppEE) == NULL){
      if(((*ppEE) = (EE_t*)malloc(sizeof(EE_t))) == NULL){return CO_ERROR_OUT_OF_MEMORY;}
   }

   EE_t *EE = *ppEE; //pointer to (newly created) object


   //Configure SPI port for use with eeprom
   SPICON = 0;           //Stops and restes the SPI
   SPISTAT = 0;
   rdata = SPIBUF;       //Clear the receive buffer
   SPIBRG = 4;           //Clock = FPB / ((4+1) * 2)
   SPICON = 0x00018120;  //MSSEN = 0 - Master mode slave select enable bit
                           //ENHBUF(bit16) = 1 - Enhanced buffer enable bit
                           //Enable SPI, 8-bit mode
                           //SMP = 0, CKE = 1, CKP = 0
                           //MSTEN = 1 - master mode enable bit

   //Set IOs directions for EEPROM SPI
   EE_SSHigh();
   EE_SS_TRIS();

   //verify variables
   if(OD_ROMSize > (EE_SIZE/2 - EE_PAGE_SIZE)) OD_ROMSize = EE_SIZE/2 - EE_PAGE_SIZE;
   if(OD_EEPROMSize > EE_SIZE/2) OD_EEPROMSize = EE_SIZE/2;

   //configure object variables
   EE->OD_EEPROMAddress = OD_EEPROMAddress;
   EE->OD_EEPROMSize = OD_EEPROMSize;
   EE->OD_ROMAddress = OD_ROMAddress;
   EE->OD_ROMSize = OD_ROMSize;
   EE->OD_EEPROMCurrentIndex = 0;

   //read the master boot record from the last page in eeprom
   EE_MBR_t MBR;
   EE_readBlock((UNSIGNED8*)&MBR, EE_SIZE - EE_PAGE_SIZE, sizeof(MBR));

   if(MBR.CRC == 0xFFFFFFFF || MBR.OD_EEPROMSize != OD_EEPROMSize || MBR.OD_ROMSize != OD_ROMSize){
      //eeprom is empty or corrupt, so don't load any variables from it. Default values will stay.
      return CO_ERROR_DATA_CORRUPT;
   }

   //read the CO_OD_EEPROM from EEPROM, first verify, if data are OK
   UNSIGNED32 firstWordRAM = *((UNSIGNED32*)OD_EEPROMAddress);
   UNSIGNED32 firstWordEE, lastWordEE;
   EE_readBlock((UNSIGNED8*)&firstWordEE, 0, 4);
   EE_readBlock((UNSIGNED8*)&lastWordEE, OD_EEPROMSize-4, 4);
   if(firstWordRAM == firstWordEE && firstWordRAM == lastWordEE)
      EE_readBlock(OD_EEPROMAddress, 0, OD_EEPROMSize);

   //read the CO_OD_ROM from EEPROM and verify CRC
   EE_readBlock(OD_ROMAddress, EE_SIZE/2, OD_ROMSize);
   if(CRC16(OD_ROMAddress, OD_ROMSize) != MBR.CRC){
      //CRC does not match
      return CO_ERROR_CRC;
   }

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
   //make sure, that previous write to EEPROM is not in progress.
   if(!EE_isWriteInProcess()){
      //verify next word
      if(++EE->OD_EEPROMCurrentIndex == EE->OD_EEPROMSize) EE->OD_EEPROMCurrentIndex = 0;
      unsigned int i = EE->OD_EEPROMCurrentIndex;

      //read eeprom
      UNSIGNED8 RAMdata = EE->OD_EEPROMAddress[i];
      UNSIGNED8 EEdata = EE_readByte(i);

      //if bytes in EEPROM and in RAM are different, then write to EEPROM
      if(EEdata != RAMdata)
         EE_writeByteNoWait(RAMdata, i);
   }
}


/******************************************************************************/
/* EEPROM 25LC128 on SPI ******************************************************/
/******************************************************************************/
#define EE_CMD_READ     (unsigned)0b00000011
#define EE_CMD_WRITE    (unsigned)0b00000010
#define EE_CMD_WRDI     (unsigned)0b00000100
#define EE_CMD_WREN     (unsigned)0b00000110
#define EE_CMD_RDSR     (unsigned)0b00000101
#define EE_CMD_WRSR     (unsigned)0b00000001


/*******************************************************************************
   Function - EE_SPIwrite

   Write to SPI and at the same time read from SPI.

   PIC32 used 16bytes long FIFO buffer with SPI. SPI module is initailized in
   <EE_init>.

   Parameters:
      tx          - Ponter to transmitting data. If NULL, zeroes will be transmitted.
      rx          - Ponter to data buffer, where received data wile be stored.      
                    If null, Received data will be disregarded.
      len         - Length of data buffers. Max 16.
*******************************************************************************/
void EE_SPIwrite(UNSIGNED8 *tx, UNSIGNED8 *rx, UNSIGNED8 len){
   UNSIGNED32 i;

   //write bytes into SPI_TXB fifo buffer
   if(tx) for(i=0; i<len; i++) SPIBUF = tx[i];
   else   for(i=0; i<len; i++) SPIBUF = 0;

   //read bytes from SPI_RXB fifo buffer
   for(i=0; i<len; i++){
      UNSIGNED32 a;
      while(SPISTATbits.SPIRBE);	//wait if buffer is empty
      if(rx) rx[i] = SPIBUF;
      else       a = SPIBUF;
   }
}


/*******************************************************************************
   Function - EE_writeEnable

   Enable write operation in EEPROM. It is called before every writing to it.
*******************************************************************************/
void EE_writeEnable(){
   UNSIGNED8 buf = EE_CMD_WREN;
   
   EE_SSLow();
   EE_SPIwrite(&buf, 0, 1);
   EE_SSHigh();
}


/******************************************************************************/
void EE_writeByteNoWait(UNSIGNED8 data, UNSIGNED32 addr){
   UNSIGNED8 buf[4];

   EE_writeEnable();

   buf[0] = EE_CMD_WRITE;
   buf[1] = (UNSIGNED8) (addr >> 8);
   buf[2] = (UNSIGNED8) addr;
   buf[3] = data;

   EE_SSLow();
   EE_SPIwrite(buf, 0, 4);
   EE_SSHigh();
}


/******************************************************************************/
UNSIGNED8 EE_readByte(UNSIGNED32 addr){
   UNSIGNED8 bufTx[4];
   UNSIGNED8 bufRx[4];
   
   bufTx[0] = EE_CMD_READ;
   bufTx[1] = (UNSIGNED8) (addr >> 8);
   bufTx[2] = (UNSIGNED8) addr;
   bufTx[3] = 0;

   EE_SSLow();
   EE_SPIwrite(bufTx, bufRx, 4);
   EE_SSHigh();

   return bufRx[3];
}


/******************************************************************************/
void EE_writeBlock(UNSIGNED8 *data, UNSIGNED32 addr, UNSIGNED32 len){

   #if EE_PAGE_SIZE != 64
      #error incompatibility in function
   #endif

   while(EE_isWriteInProcess());

   while(len){
      UNSIGNED8 buf[3];
      UNSIGNED32 i;

      EE_writeEnable();

      buf[0] = EE_CMD_WRITE;
      buf[1] = (UNSIGNED8) (addr >> 8);
      buf[2] = (UNSIGNED8) addr;
   
      EE_SSLow();
      EE_SPIwrite(buf, 0, 3);

      for(i=0; i<4; i++){
         if(len > 16){
            EE_SPIwrite(data, 0, 16);
            len -= 16;
            data += 16;
         }
         else{
            EE_SPIwrite(data, 0, len);
            len = 0;
            break;
         }
      }
      EE_SSHigh();
   
      // wait for completion of the write operation
      i=EE_readStatus();
      while(EE_isWriteInProcess());
      addr += EE_PAGE_SIZE;
   }
}


/******************************************************************************/
void EE_readBlock(UNSIGNED8 *data, UNSIGNED32 addr, UNSIGNED32 len){
   UNSIGNED8 buf[3];

   buf[0] = EE_CMD_READ;
   buf[1] = (UNSIGNED8) (addr >> 8);
   buf[2] = (UNSIGNED8) addr;

   EE_SSLow();
   EE_SPIwrite(buf, 0, 3);

   while(len){
      if(len > 16){
         EE_SPIwrite(0, data, 16);
         len -= 16;
         data += 16;
      }
      else{
         EE_SPIwrite(0, data, len);
         len = 0;
      }
   }

   EE_SSHigh();
}


/******************************************************************************/
UNSIGNED8 EE_verifyBlock(UNSIGNED8 *data, UNSIGNED32 addr, UNSIGNED32 len){
   UNSIGNED8 buf[16];
   UNSIGNED8 equal = 1;

   buf[0] = EE_CMD_READ;
   buf[1] = (UNSIGNED8) (addr >> 8);
   buf[2] = (UNSIGNED8) addr;

   EE_SSLow();
   EE_SPIwrite(buf, 0, 3);

   while(len){
      if(len > 16){
         UNSIGNED8 i;
         EE_SPIwrite(0, buf, 16);
         for(i=0; i<16; i++) if(buf[i] != data[i]) equal = 0;
         len -= 16;
         data += 16;
      }
      else{
         UNSIGNED8 i;
         EE_SPIwrite(0, buf, len);
         for(i=0; i<len; i++) if(buf[i] != data[i]) equal = 0;
         len = 0;
      }
   }

   EE_SSHigh();
   return equal;
}


/******************************************************************************/
void EE_writeStatus(UNSIGNED8 data){
   UNSIGNED8 buf[2];

   EE_writeEnable();

   EE_SSLow();
   buf[0] = EE_CMD_WRSR;
   buf[1] = data;
   EE_SPIwrite(buf, 0, 2);
   EE_SSHigh();

   while(EE_isWriteInProcess());
}


/******************************************************************************/
UNSIGNED8 EE_readStatus(){
   UNSIGNED8 bufTx[2] = {EE_CMD_RDSR, 0};
   UNSIGNED8 bufRx[2];

   EE_SSLow();
   EE_SPIwrite(bufTx, bufRx, 2);
   EE_SSHigh();

   return bufRx[1];
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
