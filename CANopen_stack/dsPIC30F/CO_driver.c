/*******************************************************************************

   File - CO_driver.c
   CAN module object for Microchip dsPIC30F and Microchip C30 compiler (>= V3.00).

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


#include "memcpyram2flash.h"
#include "CO_driver.h"
#include "CO_Emergency.h"

#include <string.h> // for memcpy
#include <stdlib.h> // for malloc, free


extern const CO_CANbitRateData_t  CO_CANbitRateData[8];

/*******************************************************************************
   Macro and Constants - CAN module registers - offset.
*******************************************************************************/
   #define CAN_REG(base, offset) (*((volatile UNSIGNED16 *) (base + offset)))

   #define C_RXF0SID    0x00
   #define C_RXF0EIDH   0x02
   #define C_RXF0EIDL   0x04
   #define C_RXF1SID    0x08
   #define C_RXF1EIDH   0x0A
   #define C_RXF1EIDL   0x0C
   #define C_RXF2SID    0x10
   #define C_RXF2EIDH   0x12
   #define C_RXF2EIDL   0x14
   #define C_RXF3SID    0x18
   #define C_RXF3EIDH   0x1A
   #define C_RXF3EIDL   0x1C
   #define C_RXF4SID    0x20
   #define C_RXF4EIDH   0x22
   #define C_RXF4EIDL   0x24
   #define C_RXF5SID    0x28
   #define C_RXF5EIDH   0x2A
   #define C_RXF5EIDL   0x2C
   #define C_RXM0SID    0x30
   #define C_RXM0EIDH   0x32
   #define C_RXM0EIDL   0x34
   #define C_RXM1SID    0x38
   #define C_RXM1EIDH   0x3A
   #define C_RXM1EIDL   0x3C

   #define C_TXBUF2     0x40
   #define C_TXBUF1     0x50
   #define C_TXBUF0     0x60
      #define C_TXSID   0x0
      #define C_TXEID   0x2
      #define C_TXDLC   0x4
      #define C_TXB     0x6
      #define C_TXCON   0xE

   #define C_RXBUF1     0x70
   #define C_RXBUF0     0x80
      #define C_RXCON   0xE
   //register alignment as in CO_CANrxMsg_t

   #define C_CTRL       0x90
   #define C_CFG1       0x92
   #define C_CFG2       0x94
   #define C_INTF       0x96
   #define C_INTE       0x98
   #define C_EC         0x9A


/******************************************************************************/
void memcpySwap2(UNSIGNED8* dest, UNSIGNED8* src){
   *(dest++) = *(src++);
   *(dest) = *(src);
}

void memcpySwap4(UNSIGNED8* dest, UNSIGNED8* src){
   *(dest++) = *(src++);
   *(dest++) = *(src++);
   *(dest++) = *(src++);
   *(dest) = *(src);
}


/******************************************************************************/
void CO_CANsetConfigurationMode(UNSIGNED16 CANbaseAddress){
   UNSIGNED16 C_CTRLcopy = CAN_REG(CANbaseAddress, C_CTRL);

   //set REQOP = 0x4
   C_CTRLcopy &= 0xFCFF;
   C_CTRLcopy |= 0x0400;
   CAN_REG(CANbaseAddress, C_CTRL) = C_CTRLcopy;

   //while OPMODE != 4
   while((CAN_REG(CANbaseAddress, C_CTRL) & 0x00E0) != 0x0080);
}


/******************************************************************************/
void CO_CANsetNormalMode(UNSIGNED16 CANbaseAddress){
   UNSIGNED16 C_CTRLcopy = CAN_REG(CANbaseAddress, C_CTRL);

   //set REQOP = 0x0
   C_CTRLcopy &= 0xF8FF;
   CAN_REG(CANbaseAddress, C_CTRL) = C_CTRLcopy;

   //while OPMODE != 0
   while((CAN_REG(CANbaseAddress, C_CTRL) & 0x00E0) != 0x0000);
}


/******************************************************************************/
INTEGER16 CO_CANmodule_init(
      CO_CANmodule_t     **ppCANmodule,
      UNSIGNED16           CANbaseAddress,
      UNSIGNED16           rxSize,
      UNSIGNED16           txSize,
      UNSIGNED16           CANbitRate)
{
   UNSIGNED16 i;

   //allocate memory if not already allocated
   if((*ppCANmodule) == NULL){
      if(((*ppCANmodule)          = (CO_CANmodule_t *) malloc(       sizeof(CO_CANmodule_t ))) == NULL){                                                                   return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppCANmodule)->rxArray = (CO_CANrxArray_t *)malloc(rxSize*sizeof(CO_CANrxArray_t))) == NULL){                               free(*ppCANmodule); *ppCANmodule=0; return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppCANmodule)->txArray = (CO_CANtxArray_t *)malloc(txSize*sizeof(CO_CANtxArray_t))) == NULL){free((*ppCANmodule)->rxArray); free(*ppCANmodule); *ppCANmodule=0; return CO_ERROR_OUT_OF_MEMORY;}
   }
   else if((*ppCANmodule)->rxArray == NULL || (*ppCANmodule)->txArray == NULL) return CO_ERROR_ILLEGAL_ARGUMENT;

   CO_CANmodule_t *CANmodule = *ppCANmodule; //pointer to (newly created) object

   //Configure object variables
   CANmodule->CANbaseAddress = CANbaseAddress;

   CANmodule->rxSize = rxSize;
   CANmodule->txSize = txSize;
   CANmodule->curentSyncTimeIsInsideWindow = 0;
   CANmodule->bufferInhibitFlag = 0;
   CANmodule->transmittingAborted = 0;
   CANmodule->firstCANtxMessage = 1;
   CANmodule->CANtxCount = 0;
   CANmodule->errOld = 0;
   CANmodule->EM = 0;
   for(i=0; i<rxSize; i++){
      CANmodule->rxArray[i].ident = 0;
      CANmodule->rxArray[i].pFunct = 0;
   }
   for(i=0; i<txSize; i++){
      CANmodule->txArray[i].bufferFull = 0;
   }


   //Configure control register
   CAN_REG(CANbaseAddress, C_CTRL) = 0x0400;


   //Configure CAN timing
   switch(CANbitRate){
      case 10:   i=0; break;
      case 20:   i=1; break;
      case 50:   i=2; break;
      default:
      case 125:  i=3; break;
      case 250:  i=4; break;
      case 500:  i=5; break;
      case 800:  i=6; break;
      case 1000: i=7; break;
   }

   if(CO_CANbitRateData[i].scale == 1)
      CAN_REG(CANbaseAddress, C_CTRL) |= 0x0800;

   CAN_REG(CANbaseAddress, C_CFG1) = (CO_CANbitRateData[i].SJW - 1) << 6 |
                                     (CO_CANbitRateData[i].BRP - 1);

   CAN_REG(CANbaseAddress, C_CFG2) = ((UNSIGNED16)(CO_CANbitRateData[i].phSeg2 - 1)) << 8 |
                                      0x0080 |
                                      (CO_CANbitRateData[i].phSeg1 - 1) << 3 |
                                      (CO_CANbitRateData[i].PROP - 1);


   //setup RX and TX control registers
   CAN_REG(CANbaseAddress, C_RXBUF0 + C_RXCON) = 0x0040;
   CAN_REG(CANbaseAddress, C_RXBUF1 + C_RXCON) = 0x0000;
   CAN_REG(CANbaseAddress, C_TXBUF0 + C_TXCON) = 0x0000;
   CAN_REG(CANbaseAddress, C_TXBUF1 + C_TXCON) = 0x0000;
   CAN_REG(CANbaseAddress, C_TXBUF2 + C_TXCON) = 0x0000;


   //CAN module hardware filters
   CAN_REG(CANbaseAddress, C_RXF0SID) = 0x0000;
   CAN_REG(CANbaseAddress, C_RXF1SID) = 0x0000;
   CAN_REG(CANbaseAddress, C_RXF2SID) = 0x0000;
   CAN_REG(CANbaseAddress, C_RXF3SID) = 0x0000;
   CAN_REG(CANbaseAddress, C_RXF4SID) = 0x0000;
   CAN_REG(CANbaseAddress, C_RXF5SID) = 0x0000;
   //CAN module filters are not used, all messages with standard 11-bit
   //identifier will be received
   //Set masks so, that all messages with standard identifier are accepted
   CAN_REG(CANbaseAddress, C_RXM0SID) = 0x0001;
   CAN_REG(CANbaseAddress, C_RXM1SID) = 0x0001;


   //CAN interrupt registers
   //clear interrupt flags
   CAN_REG(CANbaseAddress, C_INTF) = 0x0000;
   //enable both two receive interrupts and one transmit interrupt for TX0
   CAN_REG(CANbaseAddress, C_INTE) = 0x0007;
   //CAN interrupt (combined) must be configured by application

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANmodule_delete(CO_CANmodule_t** ppCANmodule){
   if(*ppCANmodule){
      free((*ppCANmodule)->txArray);
      free((*ppCANmodule)->rxArray);
      free(*ppCANmodule);
      *ppCANmodule = 0;
   }
}


/******************************************************************************/
UNSIGNED16 CO_CANrxMsg_readIdent(CO_CANrxMsg_t *rxMsg){
   return (rxMsg->ident >> 2) & 0x7FF;
}


/******************************************************************************/
INTEGER16 CO_CANrxBufferInit( CO_CANmodule_t   *CANmodule,
                              UNSIGNED16        index,
                              UNSIGNED16        ident,
                              UNSIGNED16        mask,
                              UNSIGNED8         rtr,
                              void             *object,
                              INTEGER16       (*pFunct)(void *object, CO_CANrxMsg_t *message))
{
   CO_CANrxArray_t *rxBuffer;
   UNSIGNED16 RXF, RXM;

   //safety
   if(!CANmodule || !object || !pFunct || index >= CANmodule->rxSize){
      return CO_ERROR_ILLEGAL_ARGUMENT;
   }


   //buffer, which will be configured
   rxBuffer = CANmodule->rxArray + index;

   //Configure object variables
   rxBuffer->object = object;
   rxBuffer->pFunct = pFunct;


   //CAN identifier and CAN mask, bit aligned with CAN module registers
   RXF = (ident & 0x07FF) << 2;
   if(rtr) RXF |= 0x02;
   RXM = (mask & 0x07FF) << 2;
   RXM |= 0x02;

   //configure filter and mask
   if(RXF != rxBuffer->ident || RXM != rxBuffer->mask){
      rxBuffer->ident = RXF;
      rxBuffer->mask = RXM;
   }

   return CO_ERROR_NO;
}


/******************************************************************************/
CO_CANtxArray_t *CO_CANtxBufferInit(
                              CO_CANmodule_t   *CANmodule,
                              UNSIGNED16        index,
                              UNSIGNED16        ident,
                              UNSIGNED8         rtr,
                              UNSIGNED8         noOfBytes,
                              UNSIGNED8         syncFlag)
{
   //safety
   if(!CANmodule || CANmodule->txSize <= index) return 0;

   //get specific buffer
   CO_CANtxArray_t *buffer = &CANmodule->txArray[index];

   //CAN identifier, bit aligned with CAN module registers
   UNSIGNED16 TXF;
   TXF = ident << 5;
   TXF &= 0xF800;
   TXF |= (ident & 0x003F) << 2;
   if(rtr) TXF |= 0x02;

   //write to buffer
   buffer->ident = TXF;
   buffer->DLC = noOfBytes;
   buffer->bufferFull = 0;
   buffer->syncFlag = syncFlag?1:0;

   return buffer;
}


/*******************************************************************************
   Function - CO_CANsendToModule

   Copy message to CAN module - internal usage only.

   Parameters:
      dest        - Destination address of CAN module transmit buffer.
      src         - Pointer to source message <CO_CANtxArray_t>.
*******************************************************************************/
void CO_CANsendToModule(UNSIGNED16 dest, CO_CANtxArray_t *src){
   UNSIGNED8 DLC;
   volatile UNSIGNED8 *CANdataBuffer;
   UNSIGNED8 *pData;

   //CAN-ID + RTR
   CAN_REG(dest, C_TXSID) = src->ident;

   //Data lenght
   DLC = src->DLC;
   if(DLC > 8) DLC = 8;
   CAN_REG(dest, C_TXDLC) = DLC << 3;

   //copy data
   CANdataBuffer = (volatile UNSIGNED8*)(dest + C_TXB);
   pData = src->data;
   for(; DLC>0; DLC--) *(CANdataBuffer++) = *(pData++);

   //control register, transmit request
   CAN_REG(dest, C_TXCON) |= 0x08;
}


/******************************************************************************/
INTEGER16 CO_CANsend(   CO_CANmodule_t   *CANmodule,
                        CO_CANtxArray_t  *buffer)
{
   UNSIGNED16 C_INTEold;
   UNSIGNED16 addr = CANmodule->CANbaseAddress;

   //Code related to CO_CANclearPendingSyncPDOs() function:
   if(CANmodule->transmittingAborted){
      //if message was aborted on buffer, set interrupt flag
      if(CAN_REG(addr, C_TXBUF0 + C_TXCON) & 0x40){CANmodule->transmittingAborted--; CAN_REG(addr, C_INTF) |= 0x04;}
   }

   //Was previous message sent or it is still waiting?
   if(buffer->bufferFull){
      if(!CANmodule->firstCANtxMessage)//don't set error, if bootup message is still on buffers
         CO_errorReport((CO_emergencyReport_t*)CANmodule->EM, ERROR_CAN_TX_OVERFLOW, 0);
      return CO_ERROR_TX_OVERFLOW;
   }

   //messages with syncFlag set (synchronous PDOs) must be transmited inside preset time window
   if(CANmodule->curentSyncTimeIsInsideWindow && buffer->syncFlag && !(*CANmodule->curentSyncTimeIsInsideWindow)){
      CO_errorReport((CO_emergencyReport_t*)CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 0);
      return CO_ERROR_TX_PDO_WINDOW;
   }

   //Disable CAN TX interrupts
   C_INTEold = CAN_REG(addr, C_INTE);
   CAN_REG(addr, C_INTE) &= 0xFFE3;

   //if CAN TB buffer0 is free, copy message to it
   if((CAN_REG(addr, C_TXBUF0 + C_TXCON) & 0x8) == 0 && CANmodule->CANtxCount == 0){
      CANmodule->bufferInhibitFlag = buffer->syncFlag;
      CO_CANsendToModule(addr + C_TXBUF0, buffer);
   }
   //if no buffer is free, message will be sent by interrupt
   else{
      buffer->bufferFull = 1;
      CANmodule->CANtxCount++;
   }

   //enable CAN TX interrupts
   CAN_REG(addr, C_INTE) = C_INTEold;
   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule){

   DISABLE_INTERRUPTS();

   if(CANmodule->bufferInhibitFlag){
      CAN_REG(CANmodule->CANbaseAddress, C_TXBUF0 + C_TXCON) &= 0xFFF7; //clear TXREQ
      CANmodule->transmittingAborted++;
      ENABLE_INTERRUPTS();
      CO_errorReport((CO_emergencyReport_t*)CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 0);
   }

   ENABLE_INTERRUPTS();
}


/******************************************************************************/
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule){
   UNSIGNED8 err;
   CO_emergencyReport_t* EM = (CO_emergencyReport_t*)CANmodule->EM;

   err = CAN_REG(CANmodule->CANbaseAddress, C_INTF)>>8;

   if(CANmodule->errOld != err){
      CANmodule->errOld = err;

      //CAN RX bus overflow
      if(err & 0xC0){
         CO_errorReport(EM, ERROR_CAN_RXB_OVERFLOW, err);
         CAN_REG(CANmodule->CANbaseAddress, C_INTF) &= 0x3FFF;//clear bits
      }

      //CAN TX bus off
      if(err & 0x20) CO_errorReport(EM, ERROR_CAN_TX_BUS_OFF, err);
      else           CO_errorReset(EM, ERROR_CAN_TX_BUS_OFF, err);

      //CAN TX bus passive
      if(err & 0x10){
         if(!CANmodule->firstCANtxMessage) CO_errorReport(EM, ERROR_CAN_TX_BUS_PASSIVE, err);
      }
      else{
         INTEGER16 wasCleared;
         wasCleared =        CO_errorReset(EM, ERROR_CAN_TX_BUS_PASSIVE, err);
         if(wasCleared == 1) CO_errorReset(EM, ERROR_CAN_TX_OVERFLOW, err);
      }

      //CAN RX bus passive
      if(err & 0x08) CO_errorReport(EM, ERROR_CAN_RX_BUS_PASSIVE, err);
      else           CO_errorReset(EM, ERROR_CAN_RX_BUS_PASSIVE, err);

      //CAN TX or RX bus warning
      if(err & 0x19){
         CO_errorReport(EM, ERROR_CAN_BUS_WARNING, err);
      }
      else{
         CO_errorReset(EM, ERROR_CAN_BUS_WARNING, err);
      }

   }
}


/******************************************************************************/
void CO_CANinterrupt(CO_CANmodule_t *CANmodule){
   UNSIGNED16 ICODE;
   ICODE = CAN_REG(CANmodule->CANbaseAddress, C_CTRL) & 0xE;

   //receive interrupt 0 (New CAN messagge is available in RX buffer 0)
   if(ICODE == 0xC){
      CO_CANrxMsg_t *rcvMsg;     //pointer to received message in CAN module
      UNSIGNED16 index;          //index of received message
      CO_CANrxArray_t *msgBuff;  //receive message buffer from CO_CANmodule_t object.
      UNSIGNED8 msgMatched = 0;

      rcvMsg = (CO_CANrxMsg_t*) (CANmodule->CANbaseAddress + C_RXBUF0);
      //CAN module filters are not used, message with any standard 11-bit identifier
      //has been received. Search rxArray form CANmodule for the same CAN-ID.
      msgBuff = CANmodule->rxArray;
      for(index = 0; index < CANmodule->rxSize; index++){
         if(((rcvMsg->ident ^ msgBuff->ident) & msgBuff->mask) == 0){
            msgMatched = 1;
            break;
         }
         msgBuff++;
      }

      //Call specific function, which will process the message
      if(msgMatched && msgBuff->pFunct) msgBuff->pFunct(msgBuff->object, rcvMsg);

      //Clear RXFUL flag
      rcvMsg->CON &= 0xFF7F;

      //Clear interrupt flag
      CAN_REG(CANmodule->CANbaseAddress, C_INTF) &= 0xFFFE;
   }


   //receive interrupt 1 (New CAN messagge is available in RX buffer 1)
   else if(ICODE == 0xA){
      CO_CANrxMsg_t *rcvMsg;     //pointer to received message in CAN module
      UNSIGNED16 index;          //index of received message
      CO_CANrxArray_t *msgBuff;  //receive message buffer from CO_CANmodule_t object.
      UNSIGNED8 msgMatched = 0;

      rcvMsg = (CO_CANrxMsg_t*) (CANmodule->CANbaseAddress + C_RXBUF1);
      //CAN module filters are not used, message with any standard 11-bit identifier
      //has been received. Search rxArray form CANmodule for the same CAN-ID.
      msgBuff = CANmodule->rxArray;
      for(index = 0; index < CANmodule->rxSize; index++){
         if(((rcvMsg->ident ^ msgBuff->ident) & msgBuff->mask) == 0){
            msgMatched = 1;
            break;
         }
         msgBuff++;
      }

      //Call specific function, which will process the message
      if(msgMatched && msgBuff->pFunct) msgBuff->pFunct(msgBuff->object, rcvMsg);

      //Clear RXFUL flag
      rcvMsg->CON &= 0xFF7F;

      //Clear interrupt flag
      CAN_REG(CANmodule->CANbaseAddress, C_INTF) &= 0xFFFD;
   }


   //transmit interrupt (TX buffer is free)
   else if(ICODE == 0x8){
      //Clear interrupt flag
      CAN_REG(CANmodule->CANbaseAddress, C_INTF) &= 0xFFFB;
      //First CAN message (bootup) was sent successfully
      CANmodule->firstCANtxMessage = 0;
      //clear flag from previous message
      CANmodule->bufferInhibitFlag = 0;
      //Are there any new messages waiting to be send and buffer is free
      if(CANmodule->CANtxCount > 0 && (CAN_REG(CANmodule->CANbaseAddress, C_TXBUF0 + C_TXCON) & 0x8) == 0){
         UNSIGNED16 index;          //index of transmitting message
         CANmodule->CANtxCount--;
         //search through whole array of pointers to transmit message buffers.
         for(index = 0; index < CANmodule->txSize; index++){
            //get specific buffer
            CO_CANtxArray_t *buffer = &CANmodule->txArray[index];
            //if message buffer is full, send it.
            if(buffer->bufferFull){
               //messages with syncFlag set (synchronous PDOs) must be transmited inside preset time window
               if(CANmodule->curentSyncTimeIsInsideWindow && buffer->syncFlag){
                  if(!(*CANmodule->curentSyncTimeIsInsideWindow)){
                     CO_errorReport((CO_emergencyReport_t*)CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 0);
                     //release buffer
                     buffer->bufferFull = 0;
                     //exit interrupt
                     break;
                  }
                  CANmodule->bufferInhibitFlag = 1;
               }

               //Copy message to CAN buffer
               CO_CANsendToModule(CANmodule->CANbaseAddress + C_TXBUF0, buffer);

               //release buffer
               buffer->bufferFull = 0;
               //exit interrupt
               break;
            }
         }//end of for loop
      }
   }
}


/******************************************************************************/
UNSIGNED32 CO_ODF(   void       *object,
                     UNSIGNED16  index,
                     UNSIGNED8   subIndex,
                     UNSIGNED16 *pLength,
                     UNSIGNED16  attribute,
                     UNSIGNED8   dir,
                     void       *dataBuff,
                     const void *pData)
{
   #define CO_ODA_MEM_ROM          0x01   //same attribute is in CO_SDO.h file
   #define CO_ODA_MEM_RAM          0x02   //same attribute is in CO_SDO.h file
   #define CO_ODA_MEM_EEPROM       0x03   //same attribute is in CO_SDO.h file
   #define CO_ODA_MB_VALUE         0x80   //same attribute is in CO_SDO.h file
   #ifdef __BIG_ENDIAN__
      #error Function does not work with BIG ENDIAN
   #endif

   if(dir==0){ //Reading Object Dictionary variable
      DISABLE_INTERRUPTS();
      memcpy(dataBuff, pData, *pLength);
      ENABLE_INTERRUPTS();
   }

   else{ //Writing Object Dictionary variable
      if((attribute&0x3) == CO_ODA_MEM_ROM){
         #ifdef CO_USE_FLASH_WRITE
            memcpyram2flash(pData, dataBuff, *pLength);
         #else
            return 0x06010002L;     //Attempt to write a read only object
         #endif
      }
      else{
         DISABLE_INTERRUPTS();
         memcpy((void*)pData, dataBuff, *pLength);
         ENABLE_INTERRUPTS();
      }
   }

   return 0L;
}
