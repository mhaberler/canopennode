/*******************************************************************************

   File - CANopen.c
   Main CANopen stack file. It combines Object dictionary (CO_OD) and all other
   CANopen source files. Configuration information are read from CO_OD.h file.

   Copyright (C) 2010 Janez Paternoster

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


#include "CANopen.h"

#include <stdlib.h> // for malloc, free


/* Global variables ***********************************************************/
extern const sCO_OD_object CO_OD[CO_OD_NoOfElements];  //Object Dictionary array

#if defined(__dsPIC33F__) || defined(__PIC24H__)
   //CAN message buffer for one TX and seven RX messages.
   #define CO_CANmsgBuffSize   8
   CO_CANrxMsg_t CO_CANmsg[CO_CANmsgBuffSize] __attribute__((space(dma)));
#endif


/* Verify features from CO_OD *************************************************/
   //generate error, if features are not corectly configured for this project
   #if               CO_NO_NMT_MASTER                           >  1     \
                  || CO_NO_SYNC                                 != 1     \
                  || CO_NO_EMERGENCY                            != 1     \
                  || CO_NO_SDO_SERVER                           != 1     \
                  || (CO_NO_SDO_CLIENT != 0 && CO_NO_SDO_CLIENT != 1)    \
                  || (CO_NO_RPDO < 1 || CO_NO_RPDO > 0x200)              \
                  || (CO_NO_TPDO < 1 || CO_NO_TPDO > 0x200)              \
                  || ODL_consumerHeartbeatTime_arrayLength      == 0     \
                  || ODL_errorStatusBits_stringLength           < 10
      #error Features from CO_OD.h file are not corectly configured for this project!
   #endif


/* Indexes for CANopenNode message objects ************************************/
   #ifdef ODL_consumerHeartbeatTime_arrayLength
      #define CO_NO_HB_CONS   ODL_consumerHeartbeatTime_arrayLength
   #else
      #define CO_NO_HB_CONS   0
   #endif

   #define CO_RXCAN_NMT       0                                      // index for NMT message
   #define CO_RXCAN_SYNC      1                                      // index for SYNC message
   #define CO_RXCAN_RPDO     (CO_RXCAN_SYNC+CO_NO_SYNC)              // start index for RPDO messages
   #define CO_RXCAN_SDO_SRV  (CO_RXCAN_RPDO+CO_NO_RPDO)              // start index for SDO server message (request)
   #define CO_RXCAN_SDO_CLI  (CO_RXCAN_SDO_SRV+CO_NO_SDO_SERVER)     // start index for SDO client message (response)
   #define CO_RXCAN_CONS_HB  (CO_RXCAN_SDO_CLI+CO_NO_SDO_CLIENT)     // start index for Heartbeat Consumer messages
   //total number of received CAN messages
   #define CO_RXCAN_NO_MSGS (1+CO_NO_SYNC+CO_NO_RPDO+CO_NO_SDO_SERVER+CO_NO_SDO_CLIENT+CO_NO_HB_CONS)

   #define CO_TXCAN_NMT       0                                      // index for NMT master message
   #define CO_TXCAN_SYNC      CO_TXCAN_NMT+CO_NO_NMT_MASTER          // index for SYNC message
   #define CO_TXCAN_EMERG    (CO_TXCAN_SYNC+CO_NO_SYNC)              // index for Emergency message
   #define CO_TXCAN_TPDO     (CO_TXCAN_EMERG+CO_NO_EMERGENCY)        // start index for TPDO messages
   #define CO_TXCAN_SDO_SRV  (CO_TXCAN_TPDO+CO_NO_TPDO)              // start index for SDO server message (response)
   #define CO_TXCAN_SDO_CLI  (CO_TXCAN_SDO_SRV+CO_NO_SDO_SERVER)     // start index for SDO client message (request)
   #define CO_TXCAN_HB       (CO_TXCAN_SDO_CLI+CO_NO_SDO_CLIENT)     // index for Heartbeat message
   //total number of transmitted CAN messages
   #define CO_TXCAN_NO_MSGS (CO_NO_NMT_MASTER+CO_NO_SYNC+CO_NO_EMERGENCY+CO_NO_TPDO+CO_NO_SDO_SERVER+CO_NO_SDO_CLIENT+1)


/* Default CANopen COB identifiers ********************************************/
   #define CAN_ID_NMT_SERVICE  0x000
   #define CAN_ID_SYNC         0x080
   #define CAN_ID_EMERGENCY    0x080
   #define CAN_ID_TIME_STAMP   0x100
   #define CAN_ID_TPDO0        0x180
   #define CAN_ID_RPDO0        0x200
   #define CAN_ID_TPDO1        0x280
   #define CAN_ID_RPDO1        0x300
   #define CAN_ID_TPDO2        0x380
   #define CAN_ID_RPDO2        0x400
   #define CAN_ID_TPDO3        0x480
   #define CAN_ID_RPDO3        0x500
   #define CAN_ID_TSDO         0x580
   #define CAN_ID_RSDO         0x600
   #define CAN_ID_HEARTBEAT    0x700


/******************************************************************************/
#if CO_NO_NMT_MASTER == 1
   CO_CANtxArray_t *NMTM_txBuff = 0;
   //Helper function for using:
   UNSIGNED8 CO_sendNMTcommand(CO_t *CO, UNSIGNED8 command, UNSIGNED8 nodeID){
      if(NMTM_txBuff == 0){
         //error, CO_CANtxBufferInit() was not called for this buffer.
         return CO_ERROR_TX_UNCONFIGURED; //-11
      }
      NMTM_txBuff->data[0] = command;
      NMTM_txBuff->data[1] = nodeID;
      return CO_CANsend(CO->CANmodule[0], NMTM_txBuff); //0 = success
   }
#endif


//#define CO_GLOBAL_FOR_DEBUG
#ifdef CO_GLOBAL_FOR_DEBUG
// CO_CANmodule_t          COO_CANmodule[1];
// CO_CANrxMsg_t           COO_CANmodule_CANmsgBuff[33];
// CO_CANrxArray_t         COO_CANmodule_rxArray[CO_RXCAN_NO_MSGS];
// CO_CANtxArray_t         COO_CANmodule_txArray[CO_TXCAN_NO_MSGS];
   CO_SDO_t                COO_SDO;
   void*                   COO_SDO_ODpointers[CO_OD_NoOfElements];
   CO_emergencyReport_t    COO_EM;
   UNSIGNED8               COO_EM_msgBuffer[20*8];//same value is in CO_emergency_init(), argument 9
   CO_emergencyProcess_t   COO_EMpr;
   CO_NMT_t                COO_NMT;
   CO_SYNC_t               COO_SYNC;
   CO_RPDO_t               COO_RPDO[CO_NO_RPDO];
   CO_TPDO_t               COO_TPDO[CO_NO_TPDO];
   CO_HBconsumer_t         COO_HBcons;
   CO_HBconsNode_t         COO_HBcons_monitoredNodes[CO_NO_HB_CONS];
#if CO_NO_SDO_CLIENT == 1
   CO_SDOclient_t          COO_SDOclient;
#endif
   CO_t                    COO;
#endif


/******************************************************************************/
INTEGER16 CO_init(CO_t **ppCO){

   INTEGER16 i;
   CO_t *CO;
   UNSIGNED8 nodeId;
   UNSIGNED16 CANBitRate;	
   enum CO_ReturnError err;

   //Initialize CANopen global variables if set so
   #ifdef CO_GLOBAL_FOR_DEBUG
   COO.CANmodule[0]                    = 0;
// COO.CANmodule[0]                    = &COO_CANmodule[0];
// COO.CANmodule[0]->CANmsgBuff        = &COO_CANmodule_CANmsgBuff[0];
// COO.CANmodule[0]->rxArray           = &COO_CANmodule_rxArray[0];
// COO.CANmodule[0]->txArray           = &COO_CANmodule_txArray[0];
   COO.SDO                             = &COO_SDO;
   COO.SDO->ObjectDictionaryPointers   = &COO_SDO_ODpointers[0];
   COO.EM                              = &COO_EM;
   COO.EM->msgBuffer                   = &COO_EM_msgBuffer[0];
   COO.EMpr                            = &COO_EMpr;
   COO.NMT                             = &COO_NMT;
   COO.SYNC                            = &COO_SYNC;
   for(i=0; i<CO_NO_RPDO; i++)
   COO.RPDO[i]                         = &COO_RPDO[i];
   for(i=0; i<CO_NO_TPDO; i++)
   COO.TPDO[i]                         = &COO_TPDO[i];
   COO.HBcons                          = &COO_HBcons;
   COO.HBcons->monitoredNodes          = &COO_HBcons_monitoredNodes[0];
   #if CO_NO_SDO_CLIENT == 1
   COO.SDOclient                       = &COO_SDOclient;
   #endif
   *ppCO                            = &COO;
   #endif

   //allocate memory if not already allocated
   if((*ppCO) == NULL){
      if(((*ppCO) = (CO_t *) calloc(1, sizeof(CO_t))) == NULL){return CO_ERROR_OUT_OF_MEMORY;}
   }

   CO = *ppCO; //pointer to (newly created) object


   CO_CANsetConfigurationMode(ADDR_CAN1);

   //Read CANopen Node-ID and CAN bit-rate from object dictionary
   nodeId = OD_CANNodeID; if(nodeId<1 || nodeId>127) nodeId = 0x10;
   CANBitRate = OD_CANBitRate;//in kbps


   err = CO_CANmodule_init(
                       &CO->CANmodule[0],
                        ADDR_CAN1,
#if defined(__dsPIC33F__) || defined(__PIC24H__)
                        ADDR_DMA0,
                        ADDR_DMA1,
                       &CO_CANmsg[0],
                        CO_CANmsgBuffSize,
                        __builtin_dmaoffset(&CO_CANmsg[0]),
#endif
                        CO_RXCAN_NO_MSGS,
                        CO_TXCAN_NO_MSGS,
                        CANBitRate);
   if(err){CO_delete(ppCO); return err;}

   err = CO_SDO_init(
                       &CO->SDO,
                        0x600 + nodeId,
                        0x580 + nodeId,
                        0x1200,
                       &CO_OD[0],
                        CO_OD_NoOfElements,
                        nodeId,
                        CO->CANmodule[0], CO_RXCAN_SDO_SRV,
                        CO->CANmodule[0], CO_TXCAN_SDO_SRV);
   if(err){CO_delete(ppCO); return err;}

   err = CO_emergency_init(
                       &CO->EM,
                       &CO->EMpr,
                        CO->SDO,
                       &OD_errorStatusBits[0],
                        ODL_errorStatusBits_stringLength,
                       &OD_errorRegister,
                       &OD_preDefinedErrorField[0],
                        ODL_preDefinedErrorField_arrayLength,
                        20,
                        CO->CANmodule[0], CO_TXCAN_EMERG,  CAN_ID_EMERGENCY + nodeId);
   if(err){CO_delete(ppCO); return err;}

   err = CO_NMT_init(  &CO->NMT,
                        CO->EMpr,
                        nodeId,
                        500,
                        CO->CANmodule[0], CO_RXCAN_NMT, CAN_ID_NMT_SERVICE,
                        CO->CANmodule[0], CO_TXCAN_HB,  CAN_ID_HEARTBEAT + nodeId);
   if(err){CO_delete(ppCO); return err;}

#if CO_NO_NMT_MASTER == 1
   NMTM_txBuff = CO_CANtxBufferInit(      //return pointer to 8-byte CAN data buffer, which should be populated
                        CO->CANmodule[0], //pointer to CAN module used for sending this message
                        CO_TXCAN_NMT,     //index of specific buffer inside CAN module
                        0x0000,           //CAN identifier
                        0,                //rtr
                        2,                //number of data bytes
                        0);               //synchronous message flag bit
#endif

   err = CO_SYNC_init( &CO->SYNC,
                        CO->EM,
                        CO->SDO,
                       &CO->NMT->operatingState,
                        OD_COB_ID_SYNCMessage,
                        OD_communicationCyclePeriod,
                        OD_synchronousCounterOverflowValue,
                        CO->CANmodule[0], CO_RXCAN_SYNC,
                        CO->CANmodule[0], CO_TXCAN_SYNC);
   if(err){CO_delete(ppCO); return err;}

   for(i=0; i<CO_NO_RPDO; i++){
      err = CO_RPDO_init(
                       &CO->RPDO[i],
                        CO->EM,
                        CO->SDO,
                       &CO->NMT->operatingState,
                        nodeId,
                        ((i<=4) ? (CAN_ID_RPDO0+i*0x100) : 0),
                        0,
                       &OD_RPDOCommunicationParameter[i],
                       &OD_RPDOMappingParameter[i],
                        0x1400+i,
                        0x1600+i,
                        CO->CANmodule[0], CO_RXCAN_RPDO+i);
      if(err){CO_delete(ppCO); return err;}
   }

   for(i=0; i<CO_NO_TPDO; i++){
      err = CO_TPDO_init(
                       &CO->TPDO[i],
                        CO->EM,
                        CO->SDO,
                       &CO->NMT->operatingState,
                        nodeId,
                        ((i<=4) ? (CAN_ID_TPDO0+i*0x100) : 0),
                        0,
                       &OD_TPDOCommunicationParameter[i],
                       &OD_TPDOMappingParameter[i],
                        0x1800+i,
                        0x1A00+i,
                        CO->CANmodule[0], CO_TXCAN_TPDO+i);
      if(err){CO_delete(ppCO); return err;}
   }

   err = CO_HBconsumer_init(
                       &CO->HBcons,
                        CO->EM,
                        CO->SDO,
                       &OD_consumerHeartbeatTime[0],
                        ODL_consumerHeartbeatTime_arrayLength,
                        CO->CANmodule[0], CO_RXCAN_CONS_HB);
   if(err){CO_delete(ppCO); return err;}


#if CO_NO_SDO_CLIENT == 1
   err = CO_SDOclient_init(
                       &CO->SDOclient,
                        CO->SDO,
                        OD_SDOClientParameter,
                        CO->CANmodule[0], CO_RXCAN_SDO_CLI,
                        CO->CANmodule[0], CO_TXCAN_SDO_CLI);
   if(err){CO_delete(ppCO); return err;}
#endif

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_delete(CO_t **ppCO){
   if(*ppCO){
      INTEGER16 i;
      CO_t *CO = *ppCO;

   #if CO_NO_SDO_CLIENT == 1
      CO_SDOclient_delete(&CO->SDOclient);
   #endif
      CO_HBconsumer_delete(&CO->HBcons);
      for(i=CO_NO_TPDO-1; i>=0; i--)
         CO_TPDO_delete(&CO->TPDO[i]);
      for(i=CO_NO_RPDO-1; i>=0; i--)
         CO_RPDO_delete(&CO->RPDO[i]);
      CO_SYNC_delete(&CO->SYNC);
      CO_NMT_delete(&CO->NMT);
      CO_emergency_delete(&CO->EM, &CO->EMpr);
      CO_SDO_delete(&CO->SDO);
      CO_CANmodule_delete(&CO->CANmodule[0]);

      free(*ppCO);
      *ppCO = 0;
   }
}


/******************************************************************************/
UNSIGNED8 CO_process( CO_t        *CO,
                      UNSIGNED16   timeDifference_ms)
{
   UNSIGNED8 NMTisPreOrOperational = 0;
   UNSIGNED8 reset = 0;
   static UNSIGNED8 ms50 = 0;

   if(CO->NMT->operatingState == CO_NMT_PRE_OPERATIONAL || CO->NMT->operatingState == CO_NMT_OPERATIONAL)
      NMTisPreOrOperational = 1;

   ms50 += timeDifference_ms;
   if(ms50 >= 50){
      ms50 = 0;
      CO_NMT_blinkingProcess50ms(CO->NMT);
   }

   CO_SDO_process(         CO->SDO,
                           NMTisPreOrOperational,
                           timeDifference_ms,
                           1000);

   CO_emergency_process(   CO->EMpr,
                           NMTisPreOrOperational,
                           timeDifference_ms * 10,
                           OD_inhibitTimeEMCY);

   reset = CO_NMT_process( CO->NMT,
                           timeDifference_ms,
                           OD_producerHeartbeatTime,
                           OD_NMTStartup,
                           OD_errorRegister,
                           OD_errorBehavior);

   CO_HBconsumer_process(  CO->HBcons,
                           NMTisPreOrOperational,
                           timeDifference_ms);

   return reset;
}


/******************************************************************************/
void CO_process_RPDO(CO_t *CO){
   UNSIGNED8 SYNCret;
   INTEGER16 i;

   SYNCret = CO_SYNC_process(CO->SYNC, 1000L, OD_synchronousWindowLength);
   if(SYNCret == 2) CO_CANclearPendingSyncPDOs(CO->CANmodule[0]);

   for(i=0; i<CO_NO_RPDO; i++){
      CO_RPDO_process(CO->RPDO[i]);
   }
}


/******************************************************************************/
void CO_process_TPDO(CO_t *CO){
   INTEGER16 i;

   //Verify PDO Change Of State and process PDOs
   for(i=0; i<CO_NO_TPDO; i++){
      if(!CO->TPDO[i]->sendRequest) CO->TPDO[i]->sendRequest = CO_TPDOisCOS(CO->TPDO[i]);
      CO_TPDO_process(CO->TPDO[i], CO->SYNC, 10, 1);
   }
}
