/*******************************************************************************

   File - CO_HBconsumer.c
   CANopen Heartbeat consumer object.

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
#include "CO_HBconsumer.h"

#include <stdlib.h> // for malloc, free

/******************************************************************************/
INTEGER16 CO_HBcons_receive(void *object, CO_CANrxMsg_t *msg){
   CO_HBconsNode_t *HBconsNode;

   HBconsNode = (CO_HBconsNode_t*) object; //this is the correct pointer type of the first argument

   //verify message length
   if(msg->DLC != 1) return CO_ERROR_RX_MSG_LENGTH;

   //verify message overflow (previous message was not processed yet)
   if(HBconsNode->CANrxNew) return CO_ERROR_RX_OVERFLOW;

   //copy data and set 'new message' flag
   HBconsNode->NMTstate = msg->data[0];
   HBconsNode->CANrxNew = 1;

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_HBconsumer_monitoredNodeConfig(
      CO_HBconsumer_t        *HBcons,
      UNSIGNED8               idx)
{
   UNSIGNED16 COB_ID;
   UNSIGNED16 NodeID = (UNSIGNED16)((HBcons->ObjDict_consumerHeartbeatTime[idx]>>16)&0xFF);
   CO_HBconsNode_t *monitoredNode = &HBcons->monitoredNodes[idx];

   monitoredNode->time = (UNSIGNED16)HBcons->ObjDict_consumerHeartbeatTime[idx];
   monitoredNode->NMTstate = 0;
   monitoredNode->monStarted = 0;

   //is channel used
   if(NodeID && monitoredNode->time){
      COB_ID = NodeID + 0x700;
   }
   else{
      COB_ID = 0;
      monitoredNode->time = 0;
   }

   //configure Heartbeat consumer CAN reception
   CO_CANrxBufferInit(     HBcons->CANdevRx,       //CAN device
                           HBcons->CANdevRxIdxStart + idx, //rx buffer index
                           COB_ID,                 //CAN identifier
                           0x7FF,                  //mask
                           0,                      //rtr
                           (void*)&HBcons->monitoredNodes[idx], //object passed to the receive function
                           CO_HBcons_receive);     //this function will process received message

}


/******************************************************************************/
UNSIGNED32 CO_ODF_1016( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED16 *pLength,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData)
{
   UNSIGNED32 abortCode;
   CO_HBconsumer_t *HBcons;

   HBcons = (CO_HBconsumer_t*) object; //this is the correct pointer type of the first argument

   if(dir == 1){  //Writing Object Dictionary variable
      UNSIGNED8 i;
      UNSIGNED32 dataBuffCopy;
      UNSIGNED8 NodeID;
      UNSIGNED16 HBconsTime;

      memcpySwap4((UNSIGNED8*)&dataBuffCopy, (UNSIGNED8*)dataBuff);
      NodeID = (dataBuffCopy>>16) & 0xFF;
      HBconsTime = dataBuffCopy & 0xFFFF;

      if(dataBuffCopy & 0xFF800000)
         return 0x06040043L;  //General parameter incompatibility reason.

      if(HBconsTime && NodeID){
         //there must not be more entries with same index and time different than zero
         for(i = 0; i<HBcons->numberOfMonitoredNodes; i++){
            UNSIGNED32 objectCopy = HBcons->ObjDict_consumerHeartbeatTime[i];
            UNSIGNED8 NodeIDObj = (objectCopy>>16) & 0xFF;
            UNSIGNED16 HBconsTimeObj = objectCopy & 0xFFFF;
            if((subIndex-1)!=i && HBconsTimeObj && (NodeID == NodeIDObj))
               return 0x06040043L;  //General parameter incompatibility reason.
         }
      }
   }

   abortCode = CO_ODF(object, index, subIndex, pLength, attribute, dir, dataBuff, pData);

   //Configure
   if(dir == 1 && abortCode == 0 && subIndex >= 1)
      CO_HBconsumer_monitoredNodeConfig(HBcons, subIndex-1);

   return abortCode;
}


/******************************************************************************/
INTEGER16 CO_HBconsumer_init(
      CO_HBconsumer_t       **ppHBcons,
      CO_emergencyReport_t   *EM,
      CO_SDO_t               *SDO,
const UNSIGNED32             *ObjDict_consumerHeartbeatTime,
      UNSIGNED8               numberOfMonitoredNodes,
      CO_CANmodule_t *CANdevRx, UNSIGNED16 CANdevRxIdxStart)
{
   UNSIGNED8 i;
   CO_HBconsumer_t *HBcons;

   //allocate memory if not already allocated
   if((*ppHBcons) == NULL){
      if(((*ppHBcons)                 = (CO_HBconsumer_t*)malloc(                       sizeof(CO_HBconsumer_t))) == NULL){                                return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppHBcons)->monitoredNodes = (CO_HBconsNode_t*)malloc(numberOfMonitoredNodes*sizeof(CO_HBconsNode_t))) == NULL){free(*ppHBcons); *ppHBcons = 0; return CO_ERROR_OUT_OF_MEMORY;}
   }
   else if((*ppHBcons)->monitoredNodes == NULL) return CO_ERROR_ILLEGAL_ARGUMENT;

   HBcons = *ppHBcons; //pointer to (newly created) object

   //Configure object variables
   HBcons->EM = EM;
   HBcons->ObjDict_consumerHeartbeatTime = ObjDict_consumerHeartbeatTime;
   HBcons->numberOfMonitoredNodes = numberOfMonitoredNodes;
   HBcons->allMonitoredOperational = 0;
   HBcons->CANdevRx = CANdevRx;
   HBcons->CANdevRxIdxStart = CANdevRxIdxStart;

   for(i=0; i<HBcons->numberOfMonitoredNodes; i++)
      CO_HBconsumer_monitoredNodeConfig(HBcons, i);

   //Configure SDO server for first argument of CO_ODF_1016
   CO_OD_configureArgumentForODF(SDO, 0x1016, (void*)HBcons);

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_HBconsumer_delete(CO_HBconsumer_t **ppHBcons){
   if(*ppHBcons){
      free((*ppHBcons)->monitoredNodes);
      free(*ppHBcons);
      *ppHBcons = 0;
   }
}


/******************************************************************************/
void CO_HBconsumer_process(CO_HBconsumer_t     *HBcons,
                           UNSIGNED8            NMTisPreOrOperational,
                           UNSIGNED16           timeDifference_ms)
{
   UNSIGNED8 i;
   UNSIGNED8 AllMonitoredOperationalCopy;
   CO_HBconsNode_t *monitoredNode;

   AllMonitoredOperationalCopy = 5;
   monitoredNode = &HBcons->monitoredNodes[0];

   if(NMTisPreOrOperational){
      for(i=0; i<HBcons->numberOfMonitoredNodes; i++){
         if(monitoredNode->time){//is node monitored
            //Verify if new Consumer Heartbeat message received
            if(monitoredNode->CANrxNew){
               if(monitoredNode->NMTstate){
                  //not a bootup message
                  monitoredNode->monStarted = 1;
                  monitoredNode->timeoutTimer = 0;  //reset timer
                  timeDifference_ms = 0;
               }
               monitoredNode->CANrxNew = 0;
            }
            //Verify timeout
            if(monitoredNode->timeoutTimer < monitoredNode->time) monitoredNode->timeoutTimer += timeDifference_ms;

            if(monitoredNode->monStarted){
               if(monitoredNode->timeoutTimer >= monitoredNode->time){
                  CO_errorReport(HBcons->EM, ERROR_HEARTBEAT_CONSUMER, i);
                  monitoredNode->NMTstate = 0;
               }
               else if(monitoredNode->NMTstate == 0){
                  //there was a bootup message
                  CO_errorReport(HBcons->EM, ERROR_HEARTBEAT_CONSUMER_REMOTE_RESET, i);
               }
            }
            if(monitoredNode->NMTstate != CO_NMT_OPERATIONAL)
               AllMonitoredOperationalCopy = 0;
         }
         monitoredNode++;
      }
   }
   else{ //not in (pre)operational state
      for(i=0; i<HBcons->numberOfMonitoredNodes; i++){
         monitoredNode->NMTstate = 0;
         monitoredNode->CANrxNew = 0;
         monitoredNode->monStarted = 0;
         monitoredNode++;
      }
      AllMonitoredOperationalCopy = 0;
   }
   HBcons->allMonitoredOperational = AllMonitoredOperationalCopy;
}
