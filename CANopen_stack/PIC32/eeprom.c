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


#include "CO_driver.h"
#include "CO_SDO.h"
#include "eeprom.h"
#include "crc16-ccitt.h"


/******************************************************************************/
#define EE_SS_TRIS()    TRISGCLR = 0x0200
#define EE_SSLow()      PORTGCLR = 0x0200
#define EE_SSHigh()     PORTGSET = 0x0200
#define SPIBUF          SPI2ABUF
#define SPICON          SPI2ACON
#define SPISTAT         SPI2ASTAT
#define SPISTATbits     SPI2ASTATbits
#define SPIBRG          SPI2ABRG


/* Store parameters ***********************************************************/
UNSIGNED32 CO_ODF_1010(CO_ODF_arg_t *ODF_arg){
   EE_t *EE;
   UNSIGNED32 *value;

   EE = (EE_t*) ODF_arg->object;
   value = (UNSIGNED32*) ODF_arg->data;

   if(!ODF_arg->reading){
      //don't change the old value
      UNSIGNED32 valueCopy = *value;
      UNSIGNED32 *valueOld = (UNSIGNED32*) ODF_arg->ODdataStorage;
      *value = *valueOld;

      if(ODF_arg->subIndex == 1){
         if(valueCopy == 0x65766173){
            EE_MBR_t MBR;

            //read the master boot record from the last page in eeprom
            EE_readBlock((UNSIGNED8*)&MBR, EE_SIZE - EE_PAGE_SIZE, sizeof(MBR));
            //if EEPROM is not yet initilalized, enable it now
            if(MBR.OD_EEPROMSize != EE->OD_EEPROMSize)
               EE->OD_EEPROMWriteEnable = 1;

            //prepare MBR
            MBR.CRC = crc16_ccitt(EE->OD_ROMAddress, EE->OD_ROMSize, 0);
            MBR.OD_EEPROMSize = EE->OD_EEPROMSize;
            MBR.OD_ROMSize = EE->OD_ROMSize;

            //write to eeprom (blocking function)
            EE_writeStatus(0); //unprotect data
            EE_writeBlock((UNSIGNED8*)&MBR, EE_SIZE - EE_PAGE_SIZE, sizeof(MBR));
            EE_writeBlock(EE->OD_ROMAddress, EE_SIZE/2, EE->OD_ROMSize);
            EE_writeStatus(0x88); //protect data

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

   return 0;
}


/* Restore default parameters *************************************************/
UNSIGNED32 CO_ODF_1011(CO_ODF_arg_t *ODF_arg){
   EE_t *EE;
   UNSIGNED32 *value;

   EE = (EE_t*) ODF_arg->object;
   value = (UNSIGNED32*) ODF_arg->data;

   if(!ODF_arg->reading){
      //don't change the old value
      UNSIGNED32 valueCopy = *value;
      UNSIGNED32 *valueOld = (UNSIGNED32*) ODF_arg->ODdataStorage;
      *value = *valueOld;

      if(ODF_arg->subIndex >= 1){
         if(valueCopy == 0x64616F6C){
            EE_MBR_t MBR;

            //read the master boot record from the last page in eeprom
            EE_readBlock((UNSIGNED8*)&MBR, EE_SIZE - EE_PAGE_SIZE, sizeof(MBR));
            //verify MBR for safety
            if(EE_verifyBlock((UNSIGNED8*)&MBR, EE_SIZE - EE_PAGE_SIZE, sizeof(MBR)) == 0)
               return 0x06060000;   //Access failed due to an hardware error.

            switch(ODF_arg->subIndex){
               case 0x01: MBR.OD_ROMSize = 0;              break; //clear the ROM
               //following don't work, if not enabled in object dictionary
               case 0x77: MBR.OD_ROMSize = EE->OD_ROMSize; break; //restore the ROM back
               case 0x7F: MBR.OD_EEPROMSize = 0;           break; //clear EEPROM
               default: return 0x06090011;                        //Sub-index does not exist.
            }

            //write changed MBR
            EE_writeStatus(0); //unprotect data
            EE_writeBlock((UNSIGNED8*)&MBR, EE_SIZE - EE_PAGE_SIZE, sizeof(MBR));
            EE_writeStatus(0x88); //protect data

            //verify MBR and status register
            if(EE_verifyBlock((UNSIGNED8*)&MBR, EE_SIZE - EE_PAGE_SIZE, sizeof(MBR)) == 1
               && (EE_readStatus()&0x8C) == 0x88){
               //write successfull
               return 0;
            }
            else{
               return 0x06060000;   //Access failed due to an hardware error.
            }
         }
         else
            return 0x08000020; //Data cannot be transferred or stored to the application.
      }
   }

   return 0;
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
   EE->OD_EEPROMWriteEnable = 0;

   //read the master boot record from the last page in eeprom
   EE_MBR_t MBR;
   EE_readBlock((UNSIGNED8*)&MBR, EE_SIZE - EE_PAGE_SIZE, sizeof(MBR));

   //read the CO_OD_EEPROM from EEPROM, first verify, if data are OK
   if(MBR.OD_EEPROMSize == OD_EEPROMSize && (MBR.OD_ROMSize == OD_ROMSize || MBR.OD_ROMSize == 0)){
      UNSIGNED32 firstWordRAM = *((UNSIGNED32*)OD_EEPROMAddress);
      UNSIGNED32 firstWordEE, lastWordEE;
      EE_readBlock((UNSIGNED8*)&firstWordEE, 0, 4);
      EE_readBlock((UNSIGNED8*)&lastWordEE, OD_EEPROMSize-4, 4);
      if(firstWordRAM == firstWordEE && firstWordRAM == lastWordEE){
         EE_readBlock(OD_EEPROMAddress, 0, OD_EEPROMSize);
         EE->OD_EEPROMWriteEnable = 1;
      }
      else{
         return CO_ERROR_DATA_CORRUPT;
      }
   }
   else{
      return CO_ERROR_DATA_CORRUPT;
   }

   //read the CO_OD_ROM from EEPROM and verify CRC
   if(MBR.OD_ROMSize == OD_ROMSize){
      EE_readBlock(OD_ROMAddress, EE_SIZE/2, OD_ROMSize);
      if(crc16_ccitt(OD_ROMAddress, OD_ROMSize, 0) != MBR.CRC){
         return CO_ERROR_CRC;
      }
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
   if(EE && EE->OD_EEPROMWriteEnable && !EE_isWriteInProcess()){
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
