/*******************************************************************************

   File - CO_SYNC.c
   CANopen SYNC object.

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
#include "CO_SYNC.h"

#include <stdlib.h> // for malloc, free

/******************************************************************************/
INTEGER16 CO_SYNC_receive(void *object, CO_CANrxMsg_t *msg){
   CO_SYNC_t *SYNC;

   SYNC = (CO_SYNC_t*)object;   //this is the correct pointer type of the first argument

   if((*SYNC->operatingState == CO_NMT_OPERATIONAL || *SYNC->operatingState == CO_NMT_PRE_OPERATIONAL)){
      if(SYNC->counterOverflowValue){
         if(msg->DLC != 1){
            CO_errorReport(SYNC->EM, ERROR_SYNC_LENGTH, msg->DLC | 0x0100);
            return CO_ERROR_NO;
         }
         SYNC->counter = msg->data[0];
      }
      else{
         if(msg->DLC != 0){
            CO_errorReport(SYNC->EM, ERROR_SYNC_LENGTH, msg->DLC);
            return CO_ERROR_NO;
         }
      }

      SYNC->running = 1;
      SYNC->timer = 0;
   }

   return CO_ERROR_NO;
}


/******************************************************************************/
UNSIGNED32 CO_ODF_1005( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED8   length,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData)
{
   UNSIGNED8 configureSyncProducer = 0;
   UNSIGNED32 abortCode;
   CO_SYNC_t *SYNC;
   UNSIGNED32 COB_ID;

   SYNC = (CO_SYNC_t*) object; //this is the correct pointer type of the first argument
   memcpySwap4((UNSIGNED8*)&COB_ID, (UNSIGNED8*)dataBuff);

   if(dir == 1){  //Writing Object Dictionary variable
      //only 11-bit CAN identifier is supported
      if(COB_ID & 0x20000000L) return 0x06090030L; //Invalid value for parameter (download only).

      //is 'generate Sync messge' bit set?
      if(COB_ID&0x40000000L){
         //if bit was set before, value can not be changed
         if(SYNC->isProducer) return 0x08000022L;   //Data cannot be transferred or stored to the application because of the present device state.
         configureSyncProducer = 1;
      }
   }

   abortCode = CO_ODF(object, index, subIndex, length, attribute, dir, dataBuff, pData);

   if(abortCode == 0 && dir == 1){
      SYNC->COB_ID = COB_ID&0x7FF;

      if(configureSyncProducer){
         UNSIGNED8 len = 0;
         if(SYNC->counterOverflowValue){
            len = 1;
            SYNC->counter = 0;
            SYNC->running = 0;
            SYNC->timer = 0;
         }
         SYNC->CANtxBuff = CO_CANtxBufferInit(
                                 SYNC->CANdevTx,         //CAN device
                                 SYNC->CANdevTxIdx,      //index of specific buffer inside CAN module
                                 SYNC->COB_ID,           //CAN identifier
                                 0,                      //rtr
                                 len,                    //number of data bytes
                                 0);                     //synchronous message flag bit
         SYNC->isProducer = 1;
      }
      else{
         SYNC->isProducer = 0;
      }
      CO_CANrxBufferInit(SYNC->CANdevRx,         //CAN device
                              SYNC->CANdevRxIdx,      //rx buffer index
                              SYNC->COB_ID,           //CAN identifier
                              0x7FF,                  //mask
                              0,                      //rtr
                              (void*)SYNC,            //object passed to receive function
                              CO_SYNC_receive);       //this function will process received message
   }
   return abortCode;
}


/******************************************************************************/
UNSIGNED32 CO_ODF_1006( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED8   length,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData)
{
   UNSIGNED32 abortCode;
   UNSIGNED32 period;
   CO_SYNC_t *SYNC;

   SYNC = (CO_SYNC_t*) object; //this is the correct pointer type of the first argument
   memcpySwap4((UNSIGNED8*)&period, (UNSIGNED8*)dataBuff);

   abortCode = CO_ODF(object, index, subIndex, length, attribute, dir, dataBuff, pData);

   if(abortCode == 0 && dir == 1){
      //period transition from 0 to something
      if(SYNC->periodTime == 0 && period)
         SYNC->counter = 0;

      SYNC->periodTime = period;
      SYNC->periodTimeoutTime = period / 2 * 3;
      //overflow?
      if(SYNC->periodTimeoutTime < period) SYNC->periodTimeoutTime = 0xFFFFFFFFL;

      SYNC->running = 0;
      SYNC->timer = 0;
   }
   return abortCode;
}


/******************************************************************************/
UNSIGNED32 CO_ODF_1019( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED8   length,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData)
{
   UNSIGNED32 abortCode;
   CO_SYNC_t *SYNC;

   SYNC = (CO_SYNC_t*) object; //this is the correct pointer type of the first argument

   if(dir == 1){  //Writing Object Dictionary variable
      if(SYNC->periodTime) return 0x08000022L; //Data cannot be transferred or stored to the application because of the present device state.
   }

   abortCode = CO_ODF(object, index, subIndex, length, attribute, dir, dataBuff, pData);

   if(abortCode == 0 && dir == 1){
      UNSIGNED8 len = 0;
      if(SYNC->counterOverflowValue) len = 1;
      SYNC->counterOverflowValue = *((UNSIGNED8*)dataBuff);

      SYNC->CANtxBuff = CO_CANtxBufferInit(
                              SYNC->CANdevTx,         //CAN device
                              SYNC->CANdevTxIdx,      //index of specific buffer inside CAN module
                              SYNC->COB_ID,           //CAN identifier
                              0,                      //rtr
                              len,                    //number of data bytes
                              0);                     //synchronous message flag bit
   }
   return abortCode;
}


/******************************************************************************/
INTEGER16 CO_SYNC_init(
      CO_SYNC_t             **ppSYNC,
      CO_emergencyReport_t   *EM,
      CO_SDO_t               *SDO,
      UNSIGNED8              *operatingState,

      UNSIGNED32              ObjDict_COB_ID_SYNCMessage,
      UNSIGNED32              ObjDict_communicationCyclePeriod,
      UNSIGNED8               ObjDict_synchronousCounterOverflowValue,

      CO_CANmodule_t *CANdevRx, UNSIGNED16 CANdevRxIdx,
      CO_CANmodule_t *CANdevTx, UNSIGNED16 CANdevTxIdx)
{
   UNSIGNED8 len = 0;

   //allocate memory if not already allocated
   if((*ppSYNC) == NULL){
      if(((*ppSYNC) = (CO_SYNC_t*) malloc(sizeof(CO_SYNC_t))) == NULL){ return CO_ERROR_OUT_OF_MEMORY;}
   }

   CO_SYNC_t *SYNC = *ppSYNC; //pointer to (newly created) object

   //Configure object variables
   SYNC->isProducer = (ObjDict_COB_ID_SYNCMessage&0x40000000L) ? 1 : 0;
   SYNC->COB_ID = ObjDict_COB_ID_SYNCMessage&0x7FF;

   SYNC->periodTime = ObjDict_communicationCyclePeriod;
   SYNC->periodTimeoutTime = ObjDict_communicationCyclePeriod / 2 * 3;
   //overflow?
   if(SYNC->periodTimeoutTime < ObjDict_communicationCyclePeriod) SYNC->periodTimeoutTime = 0xFFFFFFFFL;

   SYNC->counterOverflowValue = ObjDict_synchronousCounterOverflowValue;
   if(ObjDict_synchronousCounterOverflowValue) len = 1;

   SYNC->curentSyncTimeIsInsideWindow = 1;
   CANdevTx->curentSyncTimeIsInsideWindow = &SYNC->curentSyncTimeIsInsideWindow; //parameter inside CAN module.

   SYNC->running = 0;
   SYNC->timer = 0;
   SYNC->counter = 0;

   SYNC->EM = EM;
   SYNC->operatingState = operatingState;
   SYNC->CANdevRx = CANdevRx;
   SYNC->CANdevRxIdx = CANdevRxIdx;

   //Configure SDO server for first argument of CO_ODF_1005, CO_ODF_1006 and CO_ODF_1019.
   CO_OD_configureArgumentForODF(SDO, 0x1005, (void*)SYNC);
   CO_OD_configureArgumentForODF(SDO, 0x1006, (void*)SYNC);
   CO_OD_configureArgumentForODF(SDO, 0x1019, (void*)SYNC);

   //configure SYNC CAN reception
   CO_CANrxBufferInit(     CANdevRx,               //CAN device
                           CANdevRxIdx,            //rx buffer index
                           SYNC->COB_ID,           //CAN identifier
                           0x7FF,                  //mask
                           0,                      //rtr
                           (void*)SYNC,            //object passed to receive function
                           CO_SYNC_receive);       //this function will process received message

   //configure SYNC CAN transmission
   SYNC->CANdevTx = CANdevTx;
   SYNC->CANdevTxIdx = CANdevTxIdx;
   SYNC->CANtxBuff = CO_CANtxBufferInit(
                           CANdevTx,               //CAN device
                           CANdevTxIdx,            //index of specific buffer inside CAN module
                           SYNC->COB_ID,           //CAN identifier
                           0,                      //rtr
                           len,                    //number of data bytes
                           0);                     //synchronous message flag bit

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_SYNC_delete(CO_SYNC_t **ppSYNC){
   if(*ppSYNC){
      free(*ppSYNC);
      *ppSYNC = 0;
   }
}


/******************************************************************************/
UNSIGNED8 CO_SYNC_process( CO_SYNC_t           *SYNC,
                           UNSIGNED32           timeDifference_us,
                           UNSIGNED32           ObjDict_synchronousWindowLength)
{
   UNSIGNED8 ret = 0;
   UNSIGNED32 timerNew;

   if((*SYNC->operatingState == CO_NMT_OPERATIONAL || *SYNC->operatingState == CO_NMT_PRE_OPERATIONAL)){
      //was SYNC just received
      if(SYNC->running && SYNC->timer == 0)
         ret = 1;

      //update sync timer, no overflow
      timerNew = SYNC->timer + timeDifference_us;
      if(timerNew > SYNC->timer) SYNC->timer = timerNew;

      //SYNC producer
      if(SYNC->isProducer && SYNC->periodTime){
         if(SYNC->timer >= SYNC->periodTime){
            if(++SYNC->counter > SYNC->counterOverflowValue) SYNC->counter = 1;
            SYNC->running = 1;
            SYNC->timer = 0;
            SYNC->CANtxBuff->data[0] = SYNC->counter;
            CO_CANsend(SYNC->CANdevTx, SYNC->CANtxBuff);
            ret = 1;
         }
      }

      //Synchronous PDOs are allowed only inside time window
      if(ObjDict_synchronousWindowLength){
         if(SYNC->timer > ObjDict_synchronousWindowLength){
            if(SYNC->curentSyncTimeIsInsideWindow == 1){
               ret = 2;
            }
            SYNC->curentSyncTimeIsInsideWindow = 0;
         }
         else{
            SYNC->curentSyncTimeIsInsideWindow = 1;
         }
      }
      else{
         SYNC->curentSyncTimeIsInsideWindow = 1;
      }

      //Verify timeout of SYNC
      if(SYNC->periodTime && SYNC->timer > SYNC->periodTimeoutTime && *SYNC->operatingState == CO_NMT_OPERATIONAL)
         CO_errorReport(SYNC->EM, ERROR_SYNC_TIME_OUT, SYNC->timer);
   }
   else{
      SYNC->running = 0;
   }
   return ret;
}
