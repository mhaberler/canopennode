/*******************************************************************************

   File - CO_driver.c
   CAN module object for Microchip PIC32MX microcontroller.

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
#include "CO_Emergency.h"

#include <string.h>     // for memcpy
#include <stdlib.h>     // for malloc, free


extern const CO_CANbitRateData_t  CO_CANbitRateData[8];
unsigned int CO_interruptStatus = 0;


/*******************************************************************************
   Macro and Constants - CAN module registers
*******************************************************************************/
   #define CAN_REG(base, offset) (*((volatile UNSIGNED32 *) ((base) + _CAN1_BASE_ADDRESS + (offset))))

   #define CLR          0x04
   #define SET          0x08
   #define INV          0x0C

   #define C_CON        0x000                         //Control Register
   #define C_CFG        0x010                         //Baud Rate Configuration Register
   #define C_INT        0x020                         //Interrupt Register
   #define C_VEC        0x030                         //Interrupt Code Register
   #define C_TREC       0x040                         //Transmit/Receive Error Counter Register
   #define C_FSTAT      0x050                         //FIFO Status  Register
   #define C_RXOVF      0x060                         //Receive FIFO Overflow Status Register
   #define C_TMR        0x070                         //CAN Timer Register
   #define C_RXM        0x080 // + (0..3 x 0x10)      //Acceptance Filter Mask Register
   #define C_FLTCON     0x0C0 // + (0..7 x 0x10)      //Filter Control Register
   #define C_RXF        0x140 // + (0..31 x 0x10)     //Acceptance Filter Register
   #define C_FIFOBA     0x340                         //Message Buffer Base Address Register
   #define C_FIFOCON    0x350 // + (0..31 x 0x40)     //FIFO Control Register
   #define C_FIFOINT    0x360 // + (0..31 x 0x40)     //FIFO Interrupt Register
   #define C_FIFOUA     0x370 // + (0..31 x 0x40)     //FIFO User Address Register
   #define C_FIFOCI     0x380 // + (0..31 x 0x40)     //Module Message Index Register


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
   UNSIGNED32 C_CONcopy = CAN_REG(CANbaseAddress, C_CON);

   //switch ON can module
   C_CONcopy |= 0x00008000;
   CAN_REG(CANbaseAddress, C_CON) = C_CONcopy;

   //request configuration mode
   C_CONcopy &= 0xF8FFFFFF;
   C_CONcopy |= 0x04000000;
   CAN_REG(CANbaseAddress, C_CON) = C_CONcopy;

   //wait for configuration mode
   while((CAN_REG(CANbaseAddress, C_CON) & 0x00E00000) != 0x00800000);
}


/******************************************************************************/
void CO_CANsetNormalMode(UNSIGNED16 CANbaseAddress){

   //request normal mode
   CAN_REG(CANbaseAddress, C_CON+CLR) = 0x07000000;

   //wait for normal mode
   while((CAN_REG(CANbaseAddress, C_CON) & 0x00E00000) != 0x00000000);
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
   UNSIGNED8 FIFOSize = 33; //32 buffers for receive, 1 buffer for transmit

   //allocate memory if not already allocated
   if((*ppCANmodule) == NULL){
      if(((*ppCANmodule)             = (CO_CANmodule_t *) malloc(         sizeof(CO_CANmodule_t ))) == NULL){                                                                                                     return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppCANmodule)->CANmsgBuff = (CO_CANrxMsg_t *)  malloc(FIFOSize*sizeof(CO_CANrxMsg_t  ))) == NULL){                                                                 free(*ppCANmodule); *ppCANmodule=0; return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppCANmodule)->rxArray    = (CO_CANrxArray_t *)malloc(  rxSize*sizeof(CO_CANrxArray_t))) == NULL){                               free((*ppCANmodule)->CANmsgBuff); free(*ppCANmodule); *ppCANmodule=0; return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppCANmodule)->txArray    = (CO_CANtxArray_t *)malloc(  txSize*sizeof(CO_CANtxArray_t))) == NULL){free((*ppCANmodule)->rxArray); free((*ppCANmodule)->CANmsgBuff); free(*ppCANmodule); *ppCANmodule=0; return CO_ERROR_OUT_OF_MEMORY;}
   }
   else if((*ppCANmodule)->CANmsgBuff == NULL || (*ppCANmodule)->rxArray == NULL || (*ppCANmodule)->txArray == NULL) return CO_ERROR_ILLEGAL_ARGUMENT;

   CO_CANmodule_t *CANmodule = *ppCANmodule; //pointer to (newly created) object

   //Configure object variables
   CANmodule->CANbaseAddress = CANbaseAddress;
   CANmodule->CANmsgBuffSize = FIFOSize;
   CANmodule->rxSize = rxSize;
   CANmodule->txSize = txSize;
   CANmodule->curentSyncTimeIsInsideWindow = 0;
   CANmodule->useCANrxFilters = (rxSize <= 32) ? 1 : 0;
   CANmodule->bufferInhibitFlag = 0;
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
   //clear FIFO
   if(sizeof(CO_CANrxMsg_t) != 16) while(1);//some safety
   UNSIGNED32* f = (UNSIGNED32*) CANmodule->CANmsgBuff;
   for(i=0; i<(FIFOSize*4); i++){
      *(f++) = 0;
   }


   //Configure control register (configuration mode, receive timer stamp is enabled, module is on)
   CAN_REG(CANbaseAddress, C_CON) = 0x04108000;


   //Configure FIFO
   CAN_REG(CANbaseAddress, C_FIFOBA) = KVA_TO_PA(CANmodule->CANmsgBuff);//FIFO base address
   CAN_REG(CANbaseAddress, C_FIFOCON) = 0x001F0000;     //FIFO0: receive FIFO, 32 buffers
   CAN_REG(CANbaseAddress, C_FIFOCON+0x40) = 0x00000080;//FIFO1: transmit FIFO, 1 buffer


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
   CAN_REG(CANbaseAddress, C_CFG) = 
      ((UNSIGNED32)(CO_CANbitRateData[i].phSeg2 - 1)) << 16 |  //SEG2PH
      0x00008000                                            |  //SEG2PHTS = 1, SAM = 0
      ((UNSIGNED32)(CO_CANbitRateData[i].phSeg1 - 1)) << 11 |  //SEG1PH
      ((UNSIGNED32)(CO_CANbitRateData[i].PROP - 1))   << 8  |  //PRSEG
      ((UNSIGNED32)(CO_CANbitRateData[i].SJW - 1))    << 6  |  //SJW
      ((UNSIGNED32)(CO_CANbitRateData[i].BRP - 1));            //BRP


   //CAN module hardware filters
   //clear all filter control registers (disable filters, mask 0 and FIFO 0 selected for all filters)
   for(i=0; i<8; i++)
      CAN_REG(CANbaseAddress, C_FLTCON+i*0x10) = 0x00000000;
   if(CANmodule->useCANrxFilters){
      //CAN module filters are used, they will be configured with
      //CO_CANrxBufferInit() functions, called by separate CANopen
      //init functions.
      //Configure all masks so, that received message must match filter
      CAN_REG(CANbaseAddress, C_RXM) = 0xFFE80000;
      CAN_REG(CANbaseAddress, C_RXM+0x10) = 0xFFE80000;
      CAN_REG(CANbaseAddress, C_RXM+0x20) = 0xFFE80000;
      CAN_REG(CANbaseAddress, C_RXM+0x30) = 0xFFE80000;
   }
   else{
      //CAN module filters are not used, all messages with standard 11-bit
      //identifier will be received
      //Configure mask 0 so, that all messages with standard identifier are accepted
      CAN_REG(CANbaseAddress, C_RXM) = 0x00080000;
      //configure one filter for FIFO 0 and enable it
      CAN_REG(CANbaseAddress, C_RXF) = 0x00000000;
      CAN_REG(CANbaseAddress, C_FLTCON) = 0x00000080;
   }


   //CAN interrupt registers
   //Enable 'RX buffer not empty' (RXNEMPTYIE) interrupt in FIFO 0 (third layer interrupt)
   CAN_REG(CANbaseAddress, C_FIFOINT) = 0x00010000;
   //Enable 'Tx buffer empty' (TXEMPTYIE) interrupt in FIFO 1 (third layer interrupt)
   //CAN_REG(CANbaseAddress, C_FIFOINT+0x40) = 0x01000000; //will be enabled in CO_CANsend
   //Enable receive (RBIE) and transmit (TBIE) buffer interrupt (secont layer interrupt)
   CAN_REG(CANbaseAddress, C_INT) = 0x00030000;
   //CAN interrupt (first layer) must be configured by application

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANmodule_delete(CO_CANmodule_t** ppCANmodule){
   if(*ppCANmodule){
      free((*ppCANmodule)->txArray);
      free((*ppCANmodule)->rxArray);
      free((*ppCANmodule)->CANmsgBuff);
      free(*ppCANmodule);
      *ppCANmodule = 0;
   }
}


/******************************************************************************/
UNSIGNED16 CO_CANrxMsg_readIdent(CO_CANrxMsg_t *rxMsg){
   return rxMsg->ident;
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
   UNSIGNED16 addr = CANmodule->CANbaseAddress;
   INTEGER16 ret = CO_ERROR_NO;

   //safety
   if(!CANmodule || !object || !pFunct || index >= CANmodule->rxSize){
      return CO_ERROR_ILLEGAL_ARGUMENT;
   }

   //buffer, which will be configured
   rxBuffer = CANmodule->rxArray + index;

   //Configure object variables
   rxBuffer->object = object;
   rxBuffer->pFunct = pFunct;

   //CAN identifier and CAN mask, bit aligned with CAN module FIFO buffers (RTR is extra)
   rxBuffer->ident = ident & 0x07FF;
   if(rtr) rxBuffer->ident |= 0x0800;
   rxBuffer->mask = (mask & 0x07FF) | 0x0800;

   //Set CAN hardware module filter and mask.
   if(CANmodule->useCANrxFilters){
      UNSIGNED32 RXF, RXM;
      volatile UNSIGNED32 *pRXF;
      volatile UNSIGNED32 *pRXM0, *pRXM1, *pRXM2, *pRXM3;
      volatile UNSIGNED8 *pFLTCON;
      UNSIGNED8 selectMask;

      //align RXF and RXM with C_RXF and C_RXM registers
      RXF = (UNSIGNED32)ident << 21;
      RXM = (UNSIGNED32)mask << 21 | 0x00080000;

      //write to filter
      pRXF = &CAN_REG(addr, C_RXF); //pointer to first filter register
      pRXF += index * (0x10/4);   //now points to C_RXFi (i == index)
      *pRXF = RXF;         //write value to filter

      //configure mask (There are four masks, each of them can be asigned to any filter.
      //                First mask has always the value 0xFFE80000 - all 11 bits must match).
      pRXM0 = &CAN_REG(addr, C_RXM);
      pRXM1 = &CAN_REG(addr, C_RXM+0x10);
      pRXM2 = &CAN_REG(addr, C_RXM+0x20);
      pRXM3 = &CAN_REG(addr, C_RXM+0x30);
      if(RXM == 0xFFE80000){
         selectMask = 0;
      }
      else if(RXM == *pRXM1 || *pRXM1 == 0xFFE80000){
         //RXM is equal to mask 1 or mask 1 was not yet configured.
         *pRXM1 = RXM;
         selectMask = 1;
      }
      else if(RXM == *pRXM2 || *pRXM2 == 0xFFE80000){
         //RXM is equal to mask 2 or mask 2 was not yet configured.
         *pRXM2 = RXM;
         selectMask = 2;
      }
      else if(RXM == *pRXM3 || *pRXM3 == 0xFFE80000){
         //RXM is equal to mask 3 or mask 3 was not yet configured.
         *pRXM3 = RXM;
         selectMask = 3;
      }
      else{
         //not enough masks
         selectMask = 0;
         ret = CO_ERROR_OUT_OF_MEMORY;
      }
      //now write to appropriate filter control register
      pFLTCON = (volatile UNSIGNED8*)(&CAN_REG(addr, C_FLTCON)); //pointer to first filter control register
      pFLTCON += (index/4) * 0x10;   //now points to the correct C_FLTCONi
      pFLTCON += index%4;   //now points to correct part of the correct C_FLTCONi
      *pFLTCON = 0x80 | (selectMask << 5); //enable filter and write filter mask select bit
   }

   return ret;
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

   //CAN identifier, DLC and rtr, bit aligned with CAN module transmit buffer
   buffer->CMSGSID = ident & 0x07FF;
   buffer->CMSGEID = (noOfBytes & 0xF) | (rtr?0x0200:0);

   buffer->bufferFull = 0;
   buffer->syncFlag = syncFlag?1:0;

   return buffer;
}


/******************************************************************************/
INTEGER16 CO_CANsend(   CO_CANmodule_t   *CANmodule,
                        CO_CANtxArray_t  *buffer)
{
   UNSIGNED16 addr = CANmodule->CANbaseAddress;

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

   //CAN TX message buffer
   UNSIGNED32* TXmsgBuffer = PA_TO_KVA1(CAN_REG(addr, C_FIFOUA+0x40));
   UNSIGNED32* message = (UNSIGNED32*) buffer;
   volatile UNSIGNED32* TX_FIFOcon = &CAN_REG(addr, C_FIFOCON+0x40);
   volatile UNSIGNED32* TX_FIFOconSet = &CAN_REG(addr, C_FIFOCON+0x48);

   DISABLE_INTERRUPTS();
   //if CAN TB buffer is free, copy message to it
   if((*TX_FIFOcon & 0x8) == 0 && CANmodule->CANtxCount == 0){
      CANmodule->bufferInhibitFlag = buffer->syncFlag;
      *(TXmsgBuffer++) = *(message++);
      *(TXmsgBuffer++) = *(message++);
      *(TXmsgBuffer++) = *(message++);
      *(TXmsgBuffer++) = *(message++);
      *TX_FIFOconSet = 0x2000;   //set UINC
      *TX_FIFOconSet = 0x0008;   //set TXREQ
   }
   //if no buffer is free, message will be sent by interrupt
   else{
      buffer->bufferFull = 1;
      CANmodule->CANtxCount++;
      //Enable 'Tx buffer empty' (TXEMPTYIE) interrupt in FIFO 1 (third layer interrupt)
      CAN_REG(addr, C_FIFOINT+0x48) = 0x01000000;
   }
   ENABLE_INTERRUPTS();

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule){

   volatile UNSIGNED32* TX_FIFOconClr = &CAN_REG(CANmodule->CANbaseAddress, C_FIFOCON+0x44);

   DISABLE_INTERRUPTS();
   if(CANmodule->bufferInhibitFlag){
      *TX_FIFOconClr = 0x0008;   //clear TXREQ
      ENABLE_INTERRUPTS();
      CO_errorReport((CO_emergencyReport_t*)CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 0);
   }
   else{
      ENABLE_INTERRUPTS();
   }
}


/******************************************************************************/
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule){
   UNSIGNED16 rxErrors, txErrors, overflow;
   UNSIGNED32 TREC;
   CO_emergencyReport_t* EM = (CO_emergencyReport_t*)CANmodule->EM;

   TREC = CAN_REG(CANmodule->CANbaseAddress, C_TREC);
   rxErrors = (UNSIGNED8) TREC;
   txErrors = (UNSIGNED8) (TREC>>8);
   if(TREC&0x00200000) txErrors = 256; //bus off
   overflow = (CAN_REG(CANmodule->CANbaseAddress, C_FIFOINT)&0x8) ? 1 : 0;

   UNSIGNED32 err = (UNSIGNED32)txErrors << 16 | rxErrors << 8 | overflow;

   if(CANmodule->errOld != err){
      CANmodule->errOld = err;

      if(txErrors >= 256){                               //bus off
         CO_errorReport(EM, ERROR_CAN_TX_BUS_OFF, err);
      }
      else{                                              //not bus off
         CO_errorReset(EM, ERROR_CAN_TX_BUS_OFF, err);

         if(rxErrors >= 96 || txErrors >= 96){           //bus warning
            CO_errorReport(EM, ERROR_CAN_BUS_WARNING, err);
         }

         if(rxErrors >= 128){                            //RX bus passive
            CO_errorReport(EM, ERROR_CAN_RX_BUS_PASSIVE, err);
         }
         else{
            CO_errorReset(EM, ERROR_CAN_RX_BUS_PASSIVE, err);
         }

         if(txErrors >= 128){                            //TX bus passive
            if(!CANmodule->firstCANtxMessage){
               CO_errorReport(EM, ERROR_CAN_TX_BUS_PASSIVE, err);
            }
         }
         else{
            INTEGER16 wasCleared;
            wasCleared =        CO_errorReset(EM, ERROR_CAN_TX_BUS_PASSIVE, err);
            if(wasCleared == 1) CO_errorReset(EM, ERROR_CAN_TX_OVERFLOW, err);
         }

         if(rxErrors < 96 && txErrors < 96){             //no error
            CO_errorReset(EM, ERROR_CAN_BUS_WARNING, err);
         }
      }

      if(overflow){                       //CAN RX bus overflow
         CO_errorReport(EM, ERROR_CAN_RXB_OVERFLOW, err);
      }
   }
}


/******************************************************************************/
void CO_CANinterrupt(CO_CANmodule_t *CANmodule){
   UNSIGNED8 ICODE;
   ICODE = (UNSIGNED8) CAN_REG(CANmodule->CANbaseAddress, C_VEC) & 0x7F;

   //receive interrupt (New CAN messagge is available in RX FIFO 0 buffer)
   if(ICODE == 0){
      CO_CANrxMsg_t *rcvMsg;     //pointer to received message in CAN module
      UNSIGNED16 index;          //index of received message
      UNSIGNED16 rcvMsgIdent;    //identifier of the received message
      CO_CANrxArray_t *msgBuff;  //receive message buffer from CO_CANmodule_t object.
      UNSIGNED8 msgMatched = 0;

      rcvMsg = (CO_CANrxMsg_t*) PA_TO_KVA1(CAN_REG(CANmodule->CANbaseAddress, C_FIFOUA));
      rcvMsgIdent = rcvMsg->ident;
      if(rcvMsg->RTR) rcvMsgIdent |= 0x0800;
      if(CANmodule->useCANrxFilters){
         //CAN module filters are used. Message with known 11-bit identifier has
         //been received
         index = rcvMsg->FILHIT;
         msgBuff = CANmodule->rxArray + index;
         //verify also RTR
         if(((rcvMsgIdent ^ msgBuff->ident) & msgBuff->mask) == 0)
            msgMatched = 1;
      }
      else{
         //CAN module filters are not used, message with any standard 11-bit identifier
         //has been received. Search rxArray form CANmodule for the same CAN-ID.
         msgBuff = CANmodule->rxArray;
         for(index = CANmodule->rxSize; index > 0; index--){
            if(((rcvMsgIdent ^ msgBuff->ident) & msgBuff->mask) == 0){
               msgMatched = 1;
               break;
            }
            msgBuff++;
         }
      }

      //Call specific function, which will process the message
      if(msgMatched) msgBuff->pFunct(msgBuff->object, rcvMsg);

      //Update the message buffer pointer
      CAN_REG(CANmodule->CANbaseAddress, C_FIFOCON+0x08) = 0x2000;   //set UINC

      //Clear interrupt flag
      //(not needed)
   }


   //transmit interrupt (TX buffer FIFO 1 is free)
   else if(ICODE == 1){
      //First CAN message (bootup) was sent successfully
      CANmodule->firstCANtxMessage = 0;
      //Are there any new messages waiting to be send and buffer is free
      if(CANmodule->CANtxCount > 0){
         UNSIGNED16 index;          //index of transmitting message
         CANmodule->CANtxCount--;

         //first buffer
         CO_CANtxArray_t *buffer = CANmodule->txArray;
         //search through whole array of pointers to transmit message buffers.
         for(index = CANmodule->txSize; index > 0; index--){
            //if message buffer is full, send it.
            if(buffer->bufferFull){
               //messages with syncFlag set (synchronous PDOs) must be transmited inside preset time window
               CANmodule->bufferInhibitFlag = 0;
               if(CANmodule->curentSyncTimeIsInsideWindow && buffer->syncFlag){
                  if(!(*CANmodule->curentSyncTimeIsInsideWindow)){
                     CO_errorReport((CO_emergencyReport_t*)CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 0);
                     //release buffer
                     buffer->bufferFull = 0;
                     //exit for loop
                     break;
                  }
                  CANmodule->bufferInhibitFlag = 1;
               }

               //Copy message to CAN buffer
               UNSIGNED32* TXmsgBuffer = PA_TO_KVA1(CAN_REG(CANmodule->CANbaseAddress, C_FIFOUA+0x40));
               UNSIGNED32* message = (UNSIGNED32*) buffer;
               volatile UNSIGNED32* TX_FIFOconSet = &CAN_REG(CANmodule->CANbaseAddress, C_FIFOCON+0x48);
               *(TXmsgBuffer++) = *(message++);
               *(TXmsgBuffer++) = *(message++);
               *(TXmsgBuffer++) = *(message++);
               *(TXmsgBuffer++) = *(message++);
               *TX_FIFOconSet = 0x2000;   //set UINC
               *TX_FIFOconSet = 0x0008;   //set TXREQ

               //release buffer
               buffer->bufferFull = 0;
               //exit for loop
               break;
            }
            buffer++;
         }//end of for loop
      }
      else{
         //if no more messages, disable 'Tx buffer empty' (TXEMPTYIE) interrupt
         CAN_REG(CANmodule->CANbaseAddress, C_FIFOINT+0x44) = 0x01000000;
      }
      //Clear interrupt flag
      //(not needed)
   }
}


/******************************************************************************/
UNSIGNED32 CO_ODF(   void       *object,
                     UNSIGNED16  index,
                     UNSIGNED8   subIndex,
                     UNSIGNED8   length,
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
      memcpy(dataBuff, pData, length);
      ENABLE_INTERRUPTS();
   }

   else{ //Writing Object Dictionary variable
      DISABLE_INTERRUPTS();
      memcpy((void*)pData, dataBuff, length);
      ENABLE_INTERRUPTS();
   }

   return 0L;
}
