/*******************************************************************************

   File - CO_driver.c
   CAN module object for Freescale MCF5282 microcontroller.

   Copyright (C) 2004-2010 	Janez Paternoster
   Copyright (C) 2012 		Laurent GROSBOIS

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


   Author: 	Janez Paternoster
   			Laurent GROSBOIS

*******************************************************************************/


#include "CO_driver.h"
#include "CO_Emergency.h"

#include <string.h>     // for memcpy
#include <stdlib.h>     // for malloc, free

//external definitions of CAN timings
extern const CO_CANbitRateData_t  CO_CANbitRateData[8];

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
	
	/* sets the module as running */
	MCF_FlexCAN_CANMCR &= ~MCF_FlexCAN_CANMCR_STOP;
	
	/* enter debug mode */
	MCF_FlexCAN_CANMCR |= MCF_FlexCAN_CANMCR_FRZ | MCF_FlexCAN_CANMCR_HALT;
	
	/* wait for entering in the mode */
	while(!(MCF_FlexCAN_CANMCR&MCF_FlexCAN_CANMCR_FRZACK)){};
}


/******************************************************************************/
void CO_CANsetNormalMode(UNSIGNED16 CANbaseAddress){
	/* sets the module as running & exit debug mode */
	MCF_FlexCAN_CANMCR &= ~MCF_FlexCAN_CANMCR_STOP & ~MCF_FlexCAN_CANMCR_FRZ & ~MCF_FlexCAN_CANMCR_HALT;
	
	/* wait for entering in the mode */
	while(!(MCF_FlexCAN_CANMCR&&MCF_FlexCAN_CANMCR_NOTRDY)){};
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
   CO_CANmodule_t *CANmodule;
   UNSIGNED8 nb_CANbuff = 16; //16 CAN buffers
   
   //allocate memory if not already allocated
   if((*ppCANmodule) == NULL){
      if(((*ppCANmodule)             = (CO_CANmodule_t *) malloc(         sizeof(CO_CANmodule_t ))) == NULL){                                                                                                     return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppCANmodule)->CANmsgBuff = (CO_CANrxMsg_t *)  malloc(nb_CANbuff*sizeof(CO_CANrxMsg_t  ))) == NULL){                                                                 free(*ppCANmodule); *ppCANmodule=0; return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppCANmodule)->rxArray    = (CO_CANrxArray_t *)malloc(  rxSize*sizeof(CO_CANrxArray_t))) == NULL){                               free((*ppCANmodule)->CANmsgBuff); free(*ppCANmodule); *ppCANmodule=0; return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppCANmodule)->txArray    = (CO_CANtxArray_t *)malloc(  txSize*sizeof(CO_CANtxArray_t))) == NULL){free((*ppCANmodule)->rxArray); free((*ppCANmodule)->CANmsgBuff); free(*ppCANmodule); *ppCANmodule=0; return CO_ERROR_OUT_OF_MEMORY;}
   }
   else if((*ppCANmodule)->CANmsgBuff == NULL || (*ppCANmodule)->rxArray == NULL || (*ppCANmodule)->txArray == NULL) return CO_ERROR_ILLEGAL_ARGUMENT;

   CANmodule = *ppCANmodule; //pointer to (newly created) object

   //Configure object variables
   CANmodule->CANbaseAddress = CANbaseAddress;
   CANmodule->CANmsgBuffSize = nb_CANbuff;
   CANmodule->rxSize = rxSize;
   CANmodule->txSize = txSize;
   CANmodule->curentSyncTimeIsInsideWindow = 0;
   CANmodule->useCANrxFilters = (UNSIGNED8) ((rxSize <= 32) ? 1 : 0);
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
   
   
   /**** HARDWARE CONFIGURATION ****/
   
   /* soft reset */
   MCF_FlexCAN_CANMCR |= MCF_FlexCAN_CANMCR_SOFTRST;
   
   /* Tx & Rx pin modes */
   MCF_FlexCAN_CANCTRL0 &= ~MCF_FlexCAN_CANCTRL0_BOFFMSK & ~MCF_FlexCAN_CANCTRL0_ERRMSK  & ~MCF_FlexCAN_CANCTRL0_RXMODE  & ~MCF_FlexCAN_CANCTRL0_TXMODE(0);
   				/*  - no Bus off interrupt
   					- no error interrupt
   					- Rx mode : 0 is dominant bit
   					- Tx mode : full CMOS positive */
   
   //reset ctrl registers
   MCF_FlexCAN_CANCTRL1 = 0x00;
   MCF_FlexCAN_CANCTRL2 = 0x00;
	
   //Configure CAN timing
   switch(CANbitRate){
      default:
      case 125:  i=3; break;
      case 1000: i=7; break;
   }

   MCF_FlexCAN_CANCTRL1 |= MCF_FlexCAN_CANCTRL1_PROPSEG(CO_CANbitRateData[i].PROP); // PROP
   MCF_FlexCAN_CANCTRL2 |= MCF_FlexCAN_CANCTRL2_RJW(CO_CANbitRateData[i].SJW);		// SJW
   MCF_FlexCAN_CANCTRL2 |= MCF_FlexCAN_CANCTRL2_PSEG1(CO_CANbitRateData[i].phSeg1);	//SEG1PH
   MCF_FlexCAN_CANCTRL2 |= MCF_FlexCAN_CANCTRL2_PSEG2(CO_CANbitRateData[i].phSeg2); //SEG2PH
   MCF_FlexCAN_PRESDIV = CO_CANbitRateData[i].BRP;									//BRP
  
   /*** here should go specific option concerning CTRL1 & CTRL2 registers ***/	


   /* CAN module hardware filters */
   //clear all filter control registers
   MCF_FlexCAN_RXGMASK  = 0x00; //mask for Rx[0-13]
   MCF_FlexCAN_RX14MASK = 0x00; //mask for Rx14
   MCF_FlexCAN_RX15MASK = 0x00; //mask for Rx15
   
   
   //Set masks to accept all all messages with standard 11-bit identifier
   MCF_FlexCAN_RXGMASK  = 0x00080000;
   MCF_FlexCAN_RX14MASK = 0x00080000;
   MCF_FlexCAN_RX15MASK = 0x00080000;
   
   
   /*
   if(CANmodule->useCANrxFilters){
      //CAN module filters are used, they will be configured with
      //CO_CANrxBufferInit() functions, called by separate CANopen
      //init functions.
      //Configure all masks so, that received message must match filter
       MCF_FlexCAN_RXGMASK  = 0xFFE80000;
       MCF_FlexCAN_RX14MASK = 0xFFE80000;
       MCF_FlexCAN_RX15MASK = 0xFFE80000;
   }
   else{
      //CAN module filters are not used, all messages with standard 11-bit
      //identifier will be received
      //Configure all mask so, that all messages with standard identifier are accepted
       MCF_FlexCAN_RXGMASK  = 0x00080000;
       MCF_FlexCAN_RX14MASK = 0x00080000;
       MCF_FlexCAN_RX15MASK = 0x00080000;
   }*/


   /* CAN Module configuration register */
   MCF_FlexCAN_CANMCR &= ~MCF_FlexCAN_CANMCR_STOP & ~MCF_FlexCAN_CANMCR_FRZ & ~MCF_FlexCAN_CANMCR_HALT;


   /* configure buffers 0-13 as input buffers */
   for(i=0;i<14;i++)
   {
   	MCF_CANMB_CTRL(i) = MCF_CANMB_CTRL_CODE(0b0100);
   }

   /* configure buffers 14-15 as output buffers */
   	MCF_CANMB_CTRL14 = MCF_CANMB_CTRL_CODE(0b1000);
   	MCF_CANMB_CTRL15 = MCF_CANMB_CTRL_CODE(0b1000);


   /* CAN interrupt registers */
   //Enable all buffer interrupts (can be either Rx or Tx interrupt depending on buffer configuration)
   MCF_FlexCAN_IMASK = 0xFF;
   MCF_FlexCAN_IFLAG = 0xFF;

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
   return (UNSIGNED16) (rxMsg->sid>>5);
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

   //CAN identifier and CAN mask, bit aligned with CAN module
   rxBuffer->ident = (UNSIGNED16) ((ident & 0x07FF)<<5);
   if(rtr) rxBuffer->ident |= 0x0010;
   rxBuffer->mask = (UNSIGNED16) (((mask & 0x07FF)<<5) | 0x0080);

	/***** question ****/
	/* is it necessary to also initialize hardware ?? */

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
	CO_CANtxArray_t *buffer;
   //safety
   if(!CANmodule || CANmodule->txSize <= index) return 0;

   //get specific buffer
   buffer = &CANmodule->txArray[index];

   //CAN identifier, DLC and rtr, bit aligned with CAN module transmit buffer
   buffer->ident = (UNSIGNED16) ((ident & 0x07FF)<<5);
   if(rtr) buffer->ident|=0x0010;
   
   buffer->DLC = noOfBytes;
   buffer->bufferFull = 0;
   buffer->syncFlag = (UNSIGNED8) (syncFlag?1:0);

   return buffer;
}


/******************************************************************************/
INTEGER16 CO_CANsend(   CO_CANmodule_t   *CANmodule,
                        CO_CANtxArray_t  *buffer)
{
   UNSIGNED16 addr = CANmodule->CANbaseAddress;
   UNSIGNED8 i;

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
	
	
   /* Try to find a free sending buffer */
   /* Here, we use buffer 14 & 15 for Tx */
   i=14; //initial sending buffer index
   while(	(MCF_CANMB_CTRL(i) != MCF_CANMB_CTRL_CODE(0b1000))&&
   			(MCF_CANMB_CTRL(i) != MCF_CANMB_CTRL_CODE(0b0100))&&
   			(MCF_CANMB_CTRL(i) != MCF_CANMB_CTRL_CODE(0b1010))&&
   			i<16)//end sending buffer index+1
				   {
				   	i++;
				   }
				   

   DISABLE_INTERRUPTS();
   
   //if CAN TB buffer is free, copy message to it
   if(i<16){
		MCF_CANMB_CTRL(i) 	= 0x0000|MCF_CANMB_CTRL_CODE(0b1000); //Tx MB inactive
		MCF_CANMB_SID(i)	= buffer->ident;
		MCF_CANMB_DATA_WORD_1(i) = (UNSIGNED16) (((buffer->data[0])<<8) | (buffer->data[1]));
		MCF_CANMB_DATA_WORD_2(i) = (UNSIGNED16) (((buffer->data[2])<<8) | (buffer->data[3]));
		MCF_CANMB_DATA_WORD_3(i) = (UNSIGNED16) (((buffer->data[4])<<8) | (buffer->data[5]));
		MCF_CANMB_DATA_WORD_4(i) = (UNSIGNED16) (((buffer->data[6])<<8) | (buffer->data[7]));
		MCF_CANMB_CTRL(i)		 = (UNSIGNED8) (MCF_CANMB_CTRL_CODE(0b1000) | MCF_CANMB_CTRL_LENGTH(buffer->DLC) ); //Tx MB active
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
      MCF_CANMB_CTRL14 = MCF_CANMB_CTRL_CODE(0b1000);  //clear TXREQ
   	  MCF_CANMB_CTRL15 = MCF_CANMB_CTRL_CODE(0b1000);  //clear TXREQ
      ENABLE_INTERRUPTS();
      CO_errorReport((CO_emergencyReport_t*)CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 0);
   }
   else{
      ENABLE_INTERRUPTS();
   }
}


/******************************************************************************/
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule){
   unsigned rxErrors, txErrors, ESTAT;
   UNSIGNED32 err;
   CO_emergencyReport_t* EM = (CO_emergencyReport_t*)CANmodule->EM;

   rxErrors = MCF_FlexCAN_RXECTR;
   txErrors = MCF_FlexCAN_TXECTR;
   ESTAT = MCF_FlexCAN_ESTAT & 0xFF00;
   
   if(txErrors > 0xFFFF) txErrors = 0xFFFF;
   if(rxErrors > 0xFF) rxErrors = 0xFF;
   
   err = (UNSIGNED32) (txErrors << 16 | rxErrors << 8 | ESTAT>>8);

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
            INTEGER16 wasCleared;
            wasCleared =        CO_errorReset(EM, ERROR_CAN_BUS_WARNING, err);
            if(wasCleared == 1) CO_errorReset(EM, ERROR_CAN_TX_OVERFLOW, err);
         }
      }
	  
	  if(ESTAT&MCF_FlexCAN_ESTAT_TXWARN && ESTAT&MCF_FlexCAN_ESTAT_RXWARN)//bus warning					
	  {
	  	CO_errorReport(EM, ERROR_CAN_BUS_WARNING, err);
	  }
	  else
	  {
	  	CO_errorReset(EM, ERROR_CAN_BUS_WARNING, err);
	  }
   }

}


/******************************************************************************/
void CO_CANinterrupt(CO_CANmodule_t *CANmodule, UNSIGNED16 ICODE){
   
   //receive interrupt (New CAN message is available in one of the Rx buffers)
   if(ICODE <= 13){
      CO_CANrxMsg_t *rcvMsg;     //pointer to received message in CAN module
      UNSIGNED16 index;          //index of received message
      UNSIGNED16 rcvMsgIdent;    //identifier of the received message
      CO_CANrxArray_t *msgBuff;  //receive message buffer from CO_CANmodule_t object.
      UNSIGNED8 msgMatched = 0;
	  
	  
      rcvMsg = &MCF_CANMB_MSG(ICODE);	//structures are aligned
      
      rcvMsgIdent = (UNSIGNED16) rcvMsg->sid;
      if(rcvMsg->RTR) rcvMsgIdent |= 0x0800;
      
      //CAN module filters are not used, message with any standard 11-bit identifier
      //has been received. Search rxArray form CANmodule for the same CAN-ID.
      msgBuff = CANmodule->rxArray;
      for(index = 0; index < CANmodule->rxSize; index++){
         if(((rcvMsgIdent ^ msgBuff->ident) & msgBuff->mask) == 0){
            msgMatched = 1;
            break;
         }
         msgBuff++;
      }

      //Call specific function, which will process the message
      if(msgMatched) msgBuff->pFunct(msgBuff->object, rcvMsg);

   }

   //transmit interrupt (TX buffer 14 or 15 has finished sending)
   else if(ICODE >14 ){
   
      //First CAN message (bootup) was sent successfully
      CANmodule->firstCANtxMessage = 0;
      //Are there any new messages waiting to be send and buffer is free
      if(CANmodule->CANtxCount > 0){
         UNSIGNED16 index;          //index of transmitting message
         CO_CANtxArray_t *buffer;	 //Tx buffer
         
         CANmodule->CANtxCount--;

         //first buffer
         buffer = CANmodule->txArray;
         //search through whole array of pointers to transmit message buffers.
         for(index = CANmodule->txSize; index > 0; index--){
         
            //get specific buffer
            buffer = &CANmodule->txArray[index];
            
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
				MCF_CANMB_CTRL(ICODE) 			= 0x0000|MCF_CANMB_CTRL_CODE(0b1000); //Tx MB inactive
				MCF_CANMB_SID(ICODE)			= buffer->ident;
				MCF_CANMB_DATA_WORD_1(ICODE) 	= (UNSIGNED16) (((buffer->data[0])<<8) | (buffer->data[1]));
				MCF_CANMB_DATA_WORD_2(ICODE) 	= (UNSIGNED16) (((buffer->data[2])<<8) | (buffer->data[3]));
				MCF_CANMB_DATA_WORD_3(ICODE) 	= (UNSIGNED16) (((buffer->data[4])<<8) | (buffer->data[5]));
				MCF_CANMB_DATA_WORD_4(ICODE) 	= (UNSIGNED16) (((buffer->data[6])<<8) | (buffer->data[7]));
				MCF_CANMB_CTRL(ICODE)			= (UNSIGNED8) (MCF_CANMB_CTRL_CODE(0b1000) | MCF_CANMB_CTRL_LENGTH(buffer->DLC) ); //Tx MB active
				
               //release buffer
               buffer->bufferFull = 0;
               //exit for loop
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

/******************************************************************************/
