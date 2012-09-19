/*******************************************************************************

   File - CO_NMT_Heartbeat.c
   CANopen NMT and Heartbeat producer object.

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
#include "CO_SDO.h"
#include "CO_Emergency.h"
#include "CO_NMT_Heartbeat.h"

#include <stdlib.h> // for malloc, free

/******************************************************************************/
INTEGER16 CO_NMT_receive(void *object, CO_CANrxMsg_t *msg){
   CO_NMT_t *NMT;
   UNSIGNED8 command, nodeId;

   NMT = (CO_NMT_t*)object;   //this is the correct pointer type of the first argument

   //verify message length
   if(msg->DLC != 2) return CO_ERROR_RX_MSG_LENGTH;

   nodeId = msg->data[1];

   if(nodeId == 0 || nodeId == NMT->nodeId){
      command = msg->data[0];

      switch(command){
         case CO_NMT_ENTER_OPERATIONAL:      if(!(*NMT->EMpr->errorRegister))
                                                NMT->operatingState = CO_NMT_OPERATIONAL;    break;
         case CO_NMT_ENTER_STOPPED:          NMT->operatingState = CO_NMT_STOPPED;           break;
         case CO_NMT_ENTER_PRE_OPERATIONAL:  NMT->operatingState = CO_NMT_PRE_OPERATIONAL;   break;
         case CO_NMT_RESET_NODE:             NMT->resetCommand = 2;                          break;
         case CO_NMT_RESET_COMMUNICATION:    NMT->resetCommand = 1;                          break;
         default: CO_errorReport(NMT->EMpr->reportBuffer, ERROR_NMT_WRONG_COMMAND, command);
      }
   }

   return CO_ERROR_NO;
}


/******************************************************************************/
INTEGER16 CO_NMT_init(
      CO_NMT_t              **ppNMT,
      CO_emergencyProcess_t  *EMpr,
      UNSIGNED8               nodeId,
      UNSIGNED16              firstHBTime,
      CO_CANmodule_t *NMT_CANdev, UNSIGNED16 NMT_rxIdx, UNSIGNED16 CANidRxNMT,
      CO_CANmodule_t *HB_CANdev,  UNSIGNED16 HB_txIdx,  UNSIGNED16 CANidTxHB)
{
   CO_NMT_t *NMT;

   //allocate memory if not already allocated
   if((*ppNMT) == NULL){
      if(((*ppNMT) = (CO_NMT_t*) malloc(sizeof(CO_NMT_t))) == NULL){ return CO_ERROR_OUT_OF_MEMORY;}
   }

   NMT = *ppNMT; //pointer to (newly created) object

   //blinking bytes
   NMT->LEDflickering      = 0;
   NMT->LEDblinking        = 0;
   NMT->LEDsingleFlash     = 0;
   NMT->LEDdoubleFlash     = 0;
   NMT->LEDtripleFlash     = 0;
   NMT->LEDquadrupleFlash  = 0;

   //Configure object variables
   NMT->operatingState  = CO_NMT_INITIALIZING;
   NMT->LEDgreenRun     = -1;
   NMT->LEDredError     = 1;
   NMT->nodeId          = nodeId;
   NMT->firstHBTime     = firstHBTime;
   NMT->resetCommand    = 0;
   NMT->HBproducerTimer = 0xFFFF;
   NMT->EMpr            = EMpr;

   //configure NMT CAN reception
   CO_CANrxBufferInit(     NMT_CANdev,       //CAN device
                           NMT_rxIdx,        //rx buffer index
                           CANidRxNMT,       //CAN identifier
                           0x7FF,            //mask
                           0,                //rtr
                           (void*)NMT,       //object passed to receive function
                           CO_NMT_receive);  //this function will process received message

   //configure HB CAN transmission
   NMT->HB_CANdev = HB_CANdev;
   NMT->HB_TXbuff = CO_CANtxBufferInit(
                           HB_CANdev,        //CAN device
                           HB_txIdx,         //index of specific buffer inside CAN module
                           CANidTxHB,        //CAN identifier
                           0,                //rtr
                           1,                //number of data bytes
                           0);               //synchronous message flag bit

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_NMT_delete(CO_NMT_t **ppNMT){
   if(*ppNMT){
      free(*ppNMT);
      *ppNMT = 0;
   }
}


/******************************************************************************/
void CO_NMT_blinkingProcess50ms(CO_NMT_t *NMT){

   if(++NMT->LEDflickering >= 1) NMT->LEDflickering = -1;

   if(++NMT->LEDblinking >= 4) NMT->LEDblinking = -4;

   if(++NMT->LEDsingleFlash >= 4) NMT->LEDsingleFlash = -20;

   switch(++NMT->LEDdoubleFlash){
      case    4:  NMT->LEDdoubleFlash = -104; break;
      case -100:  NMT->LEDdoubleFlash =  100; break;
      case  104:  NMT->LEDdoubleFlash =  -20; break;
   }

   switch(++NMT->LEDtripleFlash){
      case    4:  NMT->LEDtripleFlash = -104; break;
      case -100:  NMT->LEDtripleFlash =  100; break;
      case  104:  NMT->LEDtripleFlash = -114; break;
      case -110:  NMT->LEDtripleFlash =  110; break;
      case  114:  NMT->LEDtripleFlash =  -20; break;
   }

   switch(++NMT->LEDquadrupleFlash){
      case    4:  NMT->LEDquadrupleFlash = -104; break;
      case -100:  NMT->LEDquadrupleFlash =  100; break;
      case  104:  NMT->LEDquadrupleFlash = -114; break;
      case -110:  NMT->LEDquadrupleFlash =  110; break;
      case  114:  NMT->LEDquadrupleFlash = -124; break;
      case -120:  NMT->LEDquadrupleFlash =  120; break;
      case  124:  NMT->LEDquadrupleFlash =  -20; break;
   }
}


/******************************************************************************/
UNSIGNED8 CO_NMT_process(  CO_NMT_t         *NMT,
                           UNSIGNED16        timeDifference_ms,
                           UNSIGNED16        HBtime,
                           UNSIGNED32        NMTstartup,
                           UNSIGNED8         errReg,
                           const UNSIGNED8  *errBehavior)
{
   UNSIGNED8 CANpassive;

   if(NMT->HBproducerTimer < HBtime) NMT->HBproducerTimer += timeDifference_ms;

   //Heartbeat producer message & Bootup message
   if((HBtime && NMT->HBproducerTimer >= HBtime) || NMT->operatingState == CO_NMT_INITIALIZING){

      NMT->HBproducerTimer = 0;

      NMT->HB_TXbuff->data[0] = NMT->operatingState;
      CO_CANsend(NMT->HB_CANdev, NMT->HB_TXbuff);

      if(NMT->operatingState == CO_NMT_INITIALIZING){
         if(HBtime > NMT->firstHBTime) NMT->HBproducerTimer = HBtime - NMT->firstHBTime;

         if((NMTstartup & 0x04) == 0)  NMT->operatingState = CO_NMT_OPERATIONAL;
         else                          NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
      }
   }


   //CAN passive flag
   CANpassive = 0;
   if(CO_isError(NMT->EMpr->reportBuffer, ERROR_CAN_TX_BUS_PASSIVE) || CO_isError(NMT->EMpr->reportBuffer, ERROR_CAN_RX_BUS_PASSIVE))
      CANpassive = 1;


   //CANopen green RUN LED (DR 303-3)
   switch(NMT->operatingState){
      case CO_NMT_STOPPED:          NMT->LEDgreenRun = NMT->LEDsingleFlash;   break;
      case CO_NMT_PRE_OPERATIONAL:  NMT->LEDgreenRun = NMT->LEDblinking;      break;
      case CO_NMT_OPERATIONAL:      NMT->LEDgreenRun = 1;                     break;
   }


   //CANopen red ERROR LED (DR 303-3)
   if(CO_isError(NMT->EMpr->reportBuffer, ERROR_CAN_TX_BUS_OFF))
      NMT->LEDredError = 1;

   else if(CO_isError(NMT->EMpr->reportBuffer, ERROR_SYNC_TIME_OUT))
      NMT->LEDredError = NMT->LEDtripleFlash;

   else if(CO_isError(NMT->EMpr->reportBuffer, ERROR_HEARTBEAT_CONSUMER) || CO_isError(NMT->EMpr->reportBuffer, ERROR_HEARTBEAT_CONSUMER_REMOTE_RESET))
      NMT->LEDredError = NMT->LEDdoubleFlash;

   else if(CANpassive || CO_isError(NMT->EMpr->reportBuffer, ERROR_CAN_BUS_WARNING))
      NMT->LEDredError = NMT->LEDsingleFlash;

   else if(errReg)
      NMT->LEDredError = (NMT->LEDblinking>=0)?-1:1;

   else
      NMT->LEDredError = -1;


   //in case of error enter pre-operational state
   if(errBehavior && (NMT->operatingState == CO_NMT_OPERATIONAL)){
      if(CANpassive && (errBehavior[2] == 0 || errBehavior[2] == 2)) errReg |= 0x10;

      if(errReg){
         //Communication error
         if(errReg&0x10){
            if      (errBehavior[1] == 0) NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
            else if (errBehavior[1] == 2) NMT->operatingState = CO_NMT_STOPPED;
            else if (CO_isError(NMT->EMpr->reportBuffer, ERROR_CAN_TX_BUS_OFF) ||
                     CO_isError(NMT->EMpr->reportBuffer, ERROR_HEARTBEAT_CONSUMER) ||
                     CO_isError(NMT->EMpr->reportBuffer, ERROR_HEARTBEAT_CONSUMER_REMOTE_RESET))
            {
               if      (errBehavior[0] == 0) NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
               else if (errBehavior[0] == 2) NMT->operatingState = CO_NMT_STOPPED;
            }
         }

         //Generic error
         if(errReg&0x01){
            if      (errBehavior[3] == 0) NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
            else if (errBehavior[3] == 2) NMT->operatingState = CO_NMT_STOPPED;
         }

         //Device profile error
         if(errReg&0x20){
            if      (errBehavior[4] == 0) NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
            else if (errBehavior[4] == 2) NMT->operatingState = CO_NMT_STOPPED;
         }

         //Manufacturer specific error
         if(errReg&0x80){
            if      (errBehavior[5] == 0) NMT->operatingState = CO_NMT_PRE_OPERATIONAL;
            else if (errBehavior[5] == 2) NMT->operatingState = CO_NMT_STOPPED;
         }

         //if operational state is lost, send HB immediatelly.
         if(NMT->operatingState != CO_NMT_OPERATIONAL)
            NMT->HBproducerTimer = HBtime;
      }
   }


   return NMT->resetCommand;
}
