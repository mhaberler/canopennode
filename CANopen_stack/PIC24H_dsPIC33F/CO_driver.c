/*******************************************************************************

   File - CO_driver.c
   CAN module object for Microchip dsPIC33F or PIC24H and Microchip C30 compiler (>= V3.00).

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

#include <string.h> // for memcpy
#include <stdlib.h> // for malloc, free


extern const CO_CANbitRateData_t  CO_CANbitRateData[8];

/*******************************************************************************
   Macro and Constants - CAN module registers and DMA registers - offset.
*******************************************************************************/
   #define CAN_REG(base, offset) (*((volatile UNSIGNED16 *) (base + offset)))

   #define C_CTRL1      0x00
   #define C_CTRL2      0x02
   #define C_VEC        0x04
   #define C_FCTRL      0x06
   #define C_FIFO       0x08
   #define C_INTF       0x0A
   #define C_INTE       0x0C
   #define C_EC         0x0E
   #define C_CFG1       0x10
   #define C_CFG2       0x12
   #define C_FEN1       0x14
   #define C_FMSKSEL1   0x18
   #define C_FMSKSEL2   0x1A

   //WIN == 0
   #define C_RXFUL1     0x20
   #define C_RXFUL2     0x22
   #define C_RXOVF1     0x28
   #define C_RXOVF2     0x2A
   #define C_TR01CON    0x30
   #define C_TR23CON    0x32
   #define C_TR45CON    0x34
   #define C_TR67CON    0x36
   #define C_RXD        0x40
   #define C_TXD        0x42

   //WIN == 1
   #define C_BUFPNT1    0x20
   #define C_BUFPNT2    0x22
   #define C_BUFPNT3    0x24
   #define C_BUFPNT4    0x26
   #define C_RXM0SID    0x30
   #define C_RXM1SID    0x34
   #define C_RXM2SID    0x38
   #define C_RXF0SID    0x40  //filter1 = +4, ...., filter 15 = +4*15


   #define DMA_REG(base, offset) (*((volatile UNSIGNED16 *) (base + offset)))

   #define DMA_CON      0x0
   #define DMA_REQ      0x2
   #define DMA_STA      0x4
   #define DMA_STB      0x6
   #define DMA_PAD      0x8
   #define DMA_CNT      0xA


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
   UNSIGNED16 C_CTRL1copy = CAN_REG(CANbaseAddress, C_CTRL1);

   //set REQOP = 0x4
   C_CTRL1copy &= 0xFCFF;
   C_CTRL1copy |= 0x0400;
   CAN_REG(CANbaseAddress, C_CTRL1) = C_CTRL1copy;

   //while OPMODE != 4
   while((CAN_REG(CANbaseAddress, C_CTRL1) & 0x00E0) != 0x0080);
}


/******************************************************************************/
void CO_CANsetNormalMode(UNSIGNED16 CANbaseAddress){
   UNSIGNED16 C_CTRL1copy = CAN_REG(CANbaseAddress, C_CTRL1);

   //set REQOP = 0x0
   C_CTRL1copy &= 0xF8FF;
   CAN_REG(CANbaseAddress, C_CTRL1) = C_CTRL1copy;

   //while OPMODE != 0
   while((CAN_REG(CANbaseAddress, C_CTRL1) & 0x00E0) != 0x0000);
}


/******************************************************************************/
INTEGER16 CO_CANmodule_init(
      CO_CANmodule_t     **ppCANmodule,
      UNSIGNED16           CANbaseAddress,
      UNSIGNED16           DMArxBaseAddress,
      UNSIGNED16           DMAtxBaseAddress,
      CO_CANrxMsg_t       *CANmsgBuff,
      UNSIGNED8            CANmsgBuffSize,
      UNSIGNED16           CANmsgBuffDMAoffset,
      UNSIGNED16           rxSize,
      UNSIGNED16           txSize,
      UNSIGNED16           CANbitRate)
{
   UNSIGNED16 i;
   volatile UNSIGNED16 *pRXF;

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
   CANmodule->CANmsgBuff = CANmsgBuff;
   CANmodule->CANmsgBuffSize = CANmsgBuffSize;
   CANmodule->rxSize = rxSize;
   CANmodule->txSize = txSize;
   CANmodule->curentSyncTimeIsInsideWindow = 0;
   CANmodule->useCANrxFilters = (rxSize <= 16) ? 1 : 0;
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


   //Configure control registers
   CAN_REG(CANbaseAddress, C_CTRL1) = 0x0400;
   CAN_REG(CANbaseAddress, C_CTRL2) = 0x0000;


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
      CAN_REG(CANbaseAddress, C_CTRL1) |= 0x0800;

   CAN_REG(CANbaseAddress, C_CFG1) = (CO_CANbitRateData[i].SJW - 1) << 6 |
                                     (CO_CANbitRateData[i].BRP - 1);

   CAN_REG(CANbaseAddress, C_CFG2) = ((UNSIGNED16)(CO_CANbitRateData[i].phSeg2 - 1)) << 8 |
                                      0x0080 |
                                      (CO_CANbitRateData[i].phSeg1 - 1) << 3 |
                                      (CO_CANbitRateData[i].PROP - 1);


   //setup RX and TX control registers
   CAN_REG(CANbaseAddress, C_CTRL1) &= 0xFFFE;     //WIN = 0 - use buffer registers
   CAN_REG(CANbaseAddress, C_RXFUL1) = 0x0000;
   CAN_REG(CANbaseAddress, C_RXFUL2) = 0x0000;
   CAN_REG(CANbaseAddress, C_RXOVF1) = 0x0000;
   CAN_REG(CANbaseAddress, C_RXOVF2) = 0x0000;
   CAN_REG(CANbaseAddress, C_TR01CON) = 0x0080;    //use one buffer for transmission
   CAN_REG(CANbaseAddress, C_TR23CON) = 0x0000;
   CAN_REG(CANbaseAddress, C_TR45CON) = 0x0000;
   CAN_REG(CANbaseAddress, C_TR67CON) = 0x0000;


   //CAN module hardware filters
   CAN_REG(CANbaseAddress, C_CTRL1) |= 0x0001;     //WIN = 1 - use filter registers
   CAN_REG(CANbaseAddress, C_FEN1) = 0xFFFF;       //enable all 16 filters
   CAN_REG(CANbaseAddress, C_FMSKSEL1) = 0x0000;   //all filters are using mask 0
   CAN_REG(CANbaseAddress, C_FMSKSEL1) = 0x0000;
   CAN_REG(CANbaseAddress, C_BUFPNT1) = 0xFFFF;    //use FIFO for all filters
   CAN_REG(CANbaseAddress, C_BUFPNT2) = 0xFFFF;
   CAN_REG(CANbaseAddress, C_BUFPNT3) = 0xFFFF;
   CAN_REG(CANbaseAddress, C_BUFPNT4) = 0xFFFF;
   //set all filters to 0
   pRXF = &CAN_REG(CANbaseAddress, C_RXF0SID);
   for(i=0; i<16; i++){
      *pRXF = 0x0000;
      pRXF += 2;
   }
   if(CANmodule->useCANrxFilters){
      //CAN module filters are used, they will be configured with
      //CO_CANrxBufferInit() functions, called by separate CANopen
      //init functions.
      //All mask bits are 1, so received message must match filter
      CAN_REG(CANbaseAddress, C_RXM0SID) = 0xFFE8;
      CAN_REG(CANbaseAddress, C_RXM1SID) = 0xFFE8;
      CAN_REG(CANbaseAddress, C_RXM2SID) = 0xFFE8;
   }
   else{
      //CAN module filters are not used, all messages with standard 11-bit
      //identifier will be received
      //Set masks so, that all messages with standard identifier are accepted
      CAN_REG(CANbaseAddress, C_RXM0SID) = 0x0008;
      CAN_REG(CANbaseAddress, C_RXM1SID) = 0x0008;
      CAN_REG(CANbaseAddress, C_RXM2SID) = 0x0008;
   }

   //WIN = 0 - use buffer registers for default
   CAN_REG(CANbaseAddress, C_CTRL1) &= 0xFFFE;


   //Configure DMA controller
   //set size of buffer in DMA RAM (FIFO Area Starts with Tx/Rx buffer TRB1 - FSA = 1)
   if     (CANmsgBuffSize >= 32) CAN_REG(CANbaseAddress, C_FCTRL) = 0xC001;
   else if(CANmsgBuffSize >= 24) CAN_REG(CANbaseAddress, C_FCTRL) = 0xA001;
   else if(CANmsgBuffSize >= 16) CAN_REG(CANbaseAddress, C_FCTRL) = 0x8001;
   else if(CANmsgBuffSize >= 12) CAN_REG(CANbaseAddress, C_FCTRL) = 0x6001;
   else if(CANmsgBuffSize >=  8) CAN_REG(CANbaseAddress, C_FCTRL) = 0x4001;
   else if(CANmsgBuffSize >=  6) CAN_REG(CANbaseAddress, C_FCTRL) = 0x2001;
   else if(CANmsgBuffSize >=  4) CAN_REG(CANbaseAddress, C_FCTRL) = 0x0001;
   else return CO_ERROR_ILLEGAL_ARGUMENT;

   //DMA chanel initialization for ECAN reception
   DMA_REG(DMArxBaseAddress, DMA_CON) = 0x0020;
   DMA_REG(DMArxBaseAddress, DMA_PAD) = (volatile UNSIGNED16) &CAN_REG(CANbaseAddress, C_RXD);
   DMA_REG(DMArxBaseAddress, DMA_CNT) = 7;
   DMA_REG(DMArxBaseAddress, DMA_REQ) = (CANbaseAddress==0x400) ? 34 : 55;
   DMA_REG(DMArxBaseAddress, DMA_STA) = CANmsgBuffDMAoffset;
   DMA_REG(DMArxBaseAddress, DMA_CON) = 0x8020;

   //DMA chanel initialization for ECAN transmission
   DMA_REG(DMAtxBaseAddress, DMA_CON) = 0x2020;
   DMA_REG(DMAtxBaseAddress, DMA_PAD) = (volatile UNSIGNED16) &CAN_REG(CANbaseAddress, C_TXD);
   DMA_REG(DMAtxBaseAddress, DMA_CNT) = 7;
   DMA_REG(DMAtxBaseAddress, DMA_REQ) = (CANbaseAddress==0x400) ? 70 : 71;
   DMA_REG(DMAtxBaseAddress, DMA_STA) = CANmsgBuffDMAoffset;
   DMA_REG(DMAtxBaseAddress, DMA_CON) = 0xA020;


   //CAN interrupt registers
   //clear interrupt flags
   CAN_REG(CANbaseAddress, C_INTF) = 0x0000;
   //enable receive and transmit interrupt
   CAN_REG(CANbaseAddress, C_INTE) = 0x0003;
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
   UNSIGNED16 addr = CANmodule->CANbaseAddress;

   //safety
   if(!CANmodule || !object || !pFunct || index >= CANmodule->rxSize){
      return CO_ERROR_ILLEGAL_ARGUMENT;
   }


   //buffer, which will be configured
   rxBuffer = CANmodule->rxArray + index;

   //Configure object variables
   rxBuffer->object = object;
   rxBuffer->pFunct = pFunct;


   //CAN identifier and CAN mask, bit aligned with CAN module registers (in DMA RAM)
   RXF = (ident & 0x07FF) << 2;
   if(rtr) RXF |= 0x02;
   RXM = (mask & 0x07FF) << 2;
   RXM |= 0x02;

   //configure filter and mask
   if(RXF != rxBuffer->ident || RXM != rxBuffer->mask){
      volatile UNSIGNED16 C_CTRL1old = CAN_REG(addr, C_CTRL1);
      CAN_REG(addr, C_CTRL1) = C_CTRL1old | 0x0001;     //WIN = 1 - use filter registers
      rxBuffer->ident = RXF;
      rxBuffer->mask = RXM;

      //Set CAN hardware module filter and mask.
      if(CANmodule->useCANrxFilters){
         volatile UNSIGNED16 *pRXF;
         volatile UNSIGNED16 *pRXM0, *pRXM1, *pRXM2;
         UNSIGNED16 selectMask;

         //align RXF and RXM with C_RXF_SID and C_RXM_SID registers
         RXF &= 0xFFFD; RXF <<= 3;
         RXM &= 0xFFFD; RXM <<= 3; RXM |= 0x0008;

         //write to filter
         pRXF = &CAN_REG(addr, C_RXF0SID); //pointer to first filter register
         pRXF += index * 2;   //now points to C_RXFiSID (i == index)
         *pRXF = RXF;         //write value to filter

         //configure mask (There are three masks, each of them can be asigned to any filter.
         //                First mask has always the value 0xFFE8 - all 11 bits must match).
         pRXM0 = &CAN_REG(addr, C_RXM0SID);
         pRXM1 = &CAN_REG(addr, C_RXM1SID);
         pRXM2 = &CAN_REG(addr, C_RXM2SID);
         if(RXM == 0xFFE8){
            selectMask = 0;
         }
         else if(RXM == *pRXM1 || *pRXM1 == 0xFFE8){
            //RXM is equal to mask 1 or mask 1 was not yet configured.
            *pRXM1 = RXM;
            selectMask = 1;
         }
         else if(RXM == *pRXM2 || *pRXM2 == 0xFFE8){
            //RXM is equal to mask 2 or mask 2 was not yet configured.
            *pRXM2 = RXM;
            selectMask = 2;
         }
         else{
            //not enough masks
            return CO_ERROR_OUT_OF_MEMORY;
            selectMask = 0;
         }
         //now write to appropriate mask select register
         if(index<8){
            UNSIGNED16 clearMask = ~(0x0003 << (index << 1));
            selectMask = selectMask << (index << 1);
            CAN_REG(addr, C_FMSKSEL1) =
               (CAN_REG(addr, C_FMSKSEL1) & clearMask) | selectMask;
         }
         else{
            UNSIGNED16 clearMask = ~(0x0003 << ((index-8) << 1));
            selectMask = selectMask << ((index-8) << 1);
            CAN_REG(addr, C_FMSKSEL2) =
               (CAN_REG(addr, C_FMSKSEL2) & clearMask) | selectMask;
         }
      }
      CAN_REG(addr, C_CTRL1) = C_CTRL1old;
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
   TXF = (ident & 0x07FF) << 2;
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
      CANbaseAddress - CAN module base address.
      dest           - Pointer to CAN module transmit buffer <CO_CANrxMsg_t>.
      src            - Pointer to source message <CO_CANtxArray_t>.
*******************************************************************************/
void CO_CANsendToModule(UNSIGNED16 CANbaseAddress, CO_CANrxMsg_t *dest, CO_CANtxArray_t *src){
   UNSIGNED8 DLC;
   UNSIGNED8 *CANdataBuffer;
   UNSIGNED8 *pData;
   volatile UNSIGNED16 C_CTRL1old;

   //CAN-ID + RTR
   dest->ident = src->ident;

   //Data lenght
   DLC = src->DLC;
   if(DLC > 8) DLC = 8;
   dest->DLC = DLC;

   //copy data
   CANdataBuffer = &(dest->data[0]);
   pData = src->data;
   for(; DLC>0; DLC--) *(CANdataBuffer++) = *(pData++);

   //control register, transmit request
   C_CTRL1old = CAN_REG(CANbaseAddress, C_CTRL1);
   CAN_REG(CANbaseAddress, C_CTRL1) = C_CTRL1old & 0xFFFE;     //WIN = 0 - use buffer registers
   CAN_REG(CANbaseAddress, C_TR01CON) |= 0x08;
   CAN_REG(CANbaseAddress, C_CTRL1) = C_CTRL1old;
}


/******************************************************************************/
INTEGER16 CO_CANsend(   CO_CANmodule_t   *CANmodule,
                        CO_CANtxArray_t  *buffer)
{
   volatile UNSIGNED16 C_CTRL1old, C_TR01CONcopy;
   UNSIGNED16 C_INTEold;
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

   //read C_TR01CON
   DISABLE_INTERRUPTS();
   C_CTRL1old = CAN_REG(addr, C_CTRL1);
   CAN_REG(addr, C_CTRL1) = C_CTRL1old & 0xFFFE;     //WIN = 0 - use buffer registers
   C_TR01CONcopy = CAN_REG(addr, C_TR01CON);
   CAN_REG(addr, C_CTRL1) = C_CTRL1old;

   //if CAN TB buffer0 is free, copy message to it
   if((C_TR01CONcopy & 0x8) == 0 && CANmodule->CANtxCount == 0){
      CANmodule->bufferInhibitFlag = buffer->syncFlag;
      CO_CANsendToModule(addr, CANmodule->CANmsgBuff, buffer);
   }
   //if no buffer is free, message will be sent by interrupt
   else{
      buffer->bufferFull = 1;
      CANmodule->CANtxCount++;
   }

   ENABLE_INTERRUPTS();
   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule){

   DISABLE_INTERRUPTS();

   if(CANmodule->bufferInhibitFlag){
      volatile UNSIGNED16 C_CTRL1old = CAN_REG(CANmodule->CANbaseAddress, C_CTRL1);
      CAN_REG(CANmodule->CANbaseAddress, C_CTRL1) = C_CTRL1old & 0xFFFE;     //WIN = 0 - use buffer registers
      CAN_REG(CANmodule->CANbaseAddress, C_TR01CON) &= 0xFFF7; //clear TXREQ
      CAN_REG(CANmodule->CANbaseAddress, C_CTRL1) = C_CTRL1old;
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
   if(CAN_REG(CANmodule->CANbaseAddress, C_INTF) & 4) err |= 0x80;

   if(CANmodule->errOld != err){
      CANmodule->errOld = err;

      //CAN RX bus overflow
      if(err & 0xC0){
         CO_errorReport(EM, ERROR_CAN_RXB_OVERFLOW, err);
         CAN_REG(CANmodule->CANbaseAddress, C_INTF) &= 0xFFFB;//clear bits
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
   ICODE = CAN_REG(CANmodule->CANbaseAddress, C_VEC) & 0x1F;

   //receive interrupt (New CAN messagge is available in RX FIFO buffer)
   if(ICODE > 0){
      CO_CANrxMsg_t *rcvMsg;     //pointer to received message in CAN module
      UNSIGNED16 index;          //index of received message
      CO_CANrxArray_t *msgBuff;  //receive message buffer from CO_CANmodule_t object.
      UNSIGNED8 msgMatched = 0;
      volatile UNSIGNED16 C_CTRL1old;

      rcvMsg = &CANmodule->CANmsgBuff[ICODE];
      if(CANmodule->useCANrxFilters){
         //CAN module filters are used. Message with known 11-bit identifier has
         //been received
         index = rcvMsg->FILHIT;
         msgBuff = CANmodule->rxArray + index;
         //verify also RTR
         if(((rcvMsg->ident ^ msgBuff->ident) & msgBuff->mask) == 0)
            msgMatched = 1;
      }
      else{
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
      }

      //Call specific function, which will process the message
      if(msgMatched) msgBuff->pFunct(msgBuff->object, rcvMsg);

      //Clear RXFUL flag
      DISABLE_INTERRUPTS();
      C_CTRL1old = CAN_REG(CANmodule->CANbaseAddress, C_CTRL1);
      CAN_REG(CANmodule->CANbaseAddress, C_CTRL1) = C_CTRL1old & 0xFFFE;     //WIN = 0 - use buffer registers
      if(ICODE < 16) CAN_REG(CANmodule->CANbaseAddress, C_RXFUL1) ^= 1 << ICODE;
      else           CAN_REG(CANmodule->CANbaseAddress, C_RXFUL2) ^= 1 << (ICODE & 0xF);
      CAN_REG(CANmodule->CANbaseAddress, C_CTRL1) = C_CTRL1old;
      ENABLE_INTERRUPTS();

      //Clear interrupt flag
      CAN_REG(CANmodule->CANbaseAddress, C_INTF) &= 0xFFFD;
   }


   //transmit interrupt (TX buffer is free)
   else{
      //Clear interrupt flag
      CAN_REG(CANmodule->CANbaseAddress, C_INTF) &= 0xFFFE;
      //First CAN message (bootup) was sent successfully
      CANmodule->firstCANtxMessage = 0;
      //clear flag from previous message
      CANmodule->bufferInhibitFlag = 0;
      //Are there any new messages waiting to be send and buffer is free
      if(CANmodule->CANtxCount > 0){
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
               CO_CANsendToModule(CANmodule->CANbaseAddress, CANmodule->CANmsgBuff, buffer);

               //release buffer
               buffer->bufferFull = 0;
               //exit interrupt
               break;
            }
         }//end of for loop
      }
   }
}
