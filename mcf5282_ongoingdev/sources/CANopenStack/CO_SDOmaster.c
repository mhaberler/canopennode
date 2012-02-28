/*******************************************************************************

   File - CO_SDOmaster.c
   CANopen SDO client (master) object.

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
#include "CO_SDOmaster.h"

#include <stdlib.h> // for malloc, free

/******************************************************************************/
INTEGER16 CO_SDOclient_receive(void *object, CO_CANrxMsg_t *msg){
   CO_SDOclient_t *SDO_C;

   SDO_C = (CO_SDOclient_t*)object;   //this is the correct pointer type of the first argument

   //verify message length
   if(msg->DLC != 8) return CO_ERROR_RX_MSG_LENGTH;

   //verify message overflow (previous message was not processed yet)
   if(SDO_C->CANrxNew) return CO_ERROR_RX_OVERFLOW;

   //copy data and set 'new message' flag
   SDO_C->CANrxData[0] = msg->data[0];
   SDO_C->CANrxData[1] = msg->data[1];
   SDO_C->CANrxData[2] = msg->data[2];
   SDO_C->CANrxData[3] = msg->data[3];
   SDO_C->CANrxData[4] = msg->data[4];
   SDO_C->CANrxData[5] = msg->data[5];
   SDO_C->CANrxData[6] = msg->data[6];
   SDO_C->CANrxData[7] = msg->data[7];

   SDO_C->CANrxNew = 1;

   return CO_ERROR_NO;
}


/******************************************************************************/
INTEGER16 CO_SDOclient_init(
      CO_SDOclient_t           **ppSDOclient,
      CO_SDO_t                  *SDO,
      OD_SDOClientParameter_t   *ObjDict_SDOClientParameter,
      CO_CANmodule_t *CANdevRx, UNSIGNED16 CANdevRxIdx,
      CO_CANmodule_t *CANdevTx, UNSIGNED16 CANdevTxIdx)
{
   CO_SDOclient_t *SDO_C;

   //verify parameters
   if(ObjDict_SDOClientParameter->maxSubIndex!=3) return CO_ERROR_ILLEGAL_ARGUMENT;

   //allocate memory if not already allocated
   if((*ppSDOclient) == NULL){
      if(((*ppSDOclient) = (CO_SDOclient_t*) malloc(sizeof(CO_SDOclient_t))) == NULL){return CO_ERROR_OUT_OF_MEMORY;}
   }

   SDO_C = *ppSDOclient; //pointer to (newly created) object

   //Configure object variables
   SDO_C->state = 0;
   SDO_C->SDO = SDO;
   SDO_C->ObjDict_SDOClientParameter = ObjDict_SDOClientParameter;

   SDO_C->CANdevRx = CANdevRx;
   SDO_C->CANdevRxIdx = CANdevRxIdx;
   SDO_C->CANdevTx = CANdevTx;
   SDO_C->CANdevTxIdx = CANdevTxIdx;

   CO_SDOclient_setup(SDO_C, 0, 0, 0);

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_SDOclient_delete(CO_SDOclient_t **ppSDOclient){
   if(*ppSDOclient){
      free(*ppSDOclient);
      *ppSDOclient = 0;
   }
}


/******************************************************************************/
INTEGER8 CO_SDOclient_setup(  CO_SDOclient_t            *SDO_C,
                              UNSIGNED32                 COB_IDClientToServer,
                              UNSIGNED32                 COB_IDServerToClient,
                              UNSIGNED8                  nodeIDOfTheSDOServer)
{
   //verify parameters
   if((COB_IDClientToServer&0x7FFFF800L) || (COB_IDServerToClient&0x7FFFF800L) ||
      nodeIDOfTheSDOServer > 127) return -2;

   //Configure object variables
   SDO_C->state = 0;

   //setup Object Dictionary variables
   if((COB_IDClientToServer & 0x80000000L) || (COB_IDServerToClient & 0x80000000L) || nodeIDOfTheSDOServer == 0){
      //SDO is NOT used
      SDO_C->ObjDict_SDOClientParameter->COB_IDClientToServer = 0x80000000L;
      SDO_C->ObjDict_SDOClientParameter->COB_IDServerToClient = 0x80000000L;
      SDO_C->ObjDict_SDOClientParameter->nodeIDOfTheSDOServer = 0;
   }
   else{
      if(COB_IDClientToServer == 0 || COB_IDServerToClient == 0){
         SDO_C->ObjDict_SDOClientParameter->COB_IDClientToServer = 0x600 + nodeIDOfTheSDOServer;
         SDO_C->ObjDict_SDOClientParameter->COB_IDServerToClient = 0x580 + nodeIDOfTheSDOServer;
      }
      else{
         SDO_C->ObjDict_SDOClientParameter->COB_IDClientToServer = COB_IDClientToServer;
         SDO_C->ObjDict_SDOClientParameter->COB_IDServerToClient = COB_IDServerToClient;
      }
      SDO_C->ObjDict_SDOClientParameter->nodeIDOfTheSDOServer = nodeIDOfTheSDOServer;
   }

   //configure SDO client CAN reception
   CO_CANrxBufferInit(     SDO_C->CANdevRx,                 //CAN device
                           SDO_C->CANdevRxIdx,              //rx buffer index
                           (UNSIGNED16)SDO_C->ObjDict_SDOClientParameter->COB_IDServerToClient,//CAN identifier
                           0x7FF,                           //mask
                           0,                               //rtr
                           (void*)SDO_C,                    //object passed to receive function
                           CO_SDOclient_receive);           //this function will process received message

   //configure SDO client CAN transmission
   SDO_C->CANtxBuff = CO_CANtxBufferInit(
                           SDO_C->CANdevTx,                 //CAN device
                           SDO_C->CANdevTxIdx,              //index of specific buffer inside CAN module
                           (UNSIGNED16)SDO_C->ObjDict_SDOClientParameter->COB_IDClientToServer,//CAN identifier
                           0,                               //rtr
                           8,                               //number of data bytes
                           0);                              //synchronous message flag bit

   return 0;
}


/******************************************************************************/
INTEGER8 CO_SDOclientDownloadInitiate( CO_SDOclient_t   *SDO_C,
                                       UNSIGNED16        index,
                                       UNSIGNED8         subIndex,
                                       UNSIGNED8        *dataTx,
                                       UNSIGNED8         dataSize)
{
   //verify parameters
   if(dataTx == 0 || dataSize == 0) return -2;

   //save parameters
   SDO_C->buffer = dataTx;
   SDO_C->bufferSize = dataSize;
   SDO_C->state = 0x20;

   //prepare CAN tx message
   SDO_C->CANtxBuff->data[1] = index & 0xFF;
   SDO_C->CANtxBuff->data[2] = index >> 8;
   SDO_C->CANtxBuff->data[3] = subIndex;

   //if nodeIDOfTheSDOServer == node-ID of this node, then exchange data with this node
   if(SDO_C->ObjDict_SDOClientParameter->nodeIDOfTheSDOServer == SDO_C->SDO->nodeId){
      return 0;
   }

   //continue: prepare dataBuff for CAN message
   if(dataSize <= 4){
      UNSIGNED8 i;
      //expedited transfer
      SDO_C->CANtxBuff->data[0] = 0x23 | ((4-dataSize) << 2);
      //clear unused data bytes
      SDO_C->CANtxBuff->data[4] = 0;
      SDO_C->CANtxBuff->data[5] = 0;
      SDO_C->CANtxBuff->data[6] = 0;
      SDO_C->CANtxBuff->data[7] = 0;
      //copy data
      for(i=dataSize+3; i>=4; i--) SDO_C->CANtxBuff->data[i] = dataTx[i-4];
   }
   else{
      UNSIGNED32 len;
      //segmented transfer
      SDO_C->CANtxBuff->data[0] = 0x21;
      len = dataSize;
      memcpySwap4(&SDO_C->CANtxBuff->data[4], (UNSIGNED8*)&len);
   }

   //empty receive buffer, reset timeout timer and send message
   SDO_C->CANrxNew = 0;
   SDO_C->timeoutTimer = 0;
   CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

   return 0;
}


/******************************************************************************/
INTEGER8 CO_SDOclientDownload(   CO_SDOclient_t      *SDO_C,
                                 UNSIGNED16           timeDifference_ms,
                                 UNSIGNED16           SDOtimeoutTime,
                                 UNSIGNED32          *pSDOabortCode)
{
   //clear abort code
   *pSDOabortCode = 0;

   //verify parameters
   if(!(SDO_C->state&0x22)){
      //communication was not properly initiated
      SDO_C->state = 0;
      SDO_C->CANrxNew = 0;
      return -3;
   }

   //if nodeIDOfTheSDOServer == node-ID of this node, then exchange data with this node
   if(SDO_C->SDO && SDO_C->ObjDict_SDOClientParameter->nodeIDOfTheSDOServer == SDO_C->SDO->nodeId){
      const sCO_OD_object* pODE;
      UNSIGNED8 length, attr;
      UNSIGNED16 indexOfFoundObject;
      UNSIGNED16 index = (UNSIGNED16)SDO_C->CANtxBuff->data[2]<<8 | SDO_C->CANtxBuff->data[1];
      UNSIGNED8 subIndex = SDO_C->CANtxBuff->data[3];

      pODE = CO_OD_find(SDO_C->SDO, index, &indexOfFoundObject);
      SDO_C->state = 0;
      SDO_C->CANrxNew = 0;
      if(pODE == 0){
         *pSDOabortCode = 0x06020000L;  //object does not exist in OD
         return -10;
      }
      if(subIndex > pODE->maxSubIndex){
         *pSDOabortCode = 0x06090011L;  //Sub-index does not exist.
         return -10;
      }
      length = CO_OD_getLength(pODE, subIndex);
      if(SDO_C->bufferSize != length){
         *pSDOabortCode = 0x06070010L;   //Length of service parameter does not match
         return -10;
      }
      attr = CO_OD_getAttribute(pODE, subIndex);
      if(!(attr&CO_ODA_WRITEABLE)){
         *pSDOabortCode = 0x06010002L;   //attempt to write a read-only object
         return -10;
      }
      //write to OD variable
      *pSDOabortCode = pODE->pFunct(SDO_C->SDO->ObjectDictionaryPointers[indexOfFoundObject],
                                    index,
                                    subIndex,
                                    length,
                                    attr,
                                    1,
                                    (void*)SDO_C->buffer,
                                    CO_OD_getDataPointer(pODE, subIndex));
      if(*pSDOabortCode){
         return -10;
      }
      return 0;
   }
   //check if new SDO object received
   if(SDO_C->CANrxNew){
      SDO_C->timeoutTimer = 0;
      if(SDO_C->state & 0x20){
         //download initiated
         switch(SDO_C->CANrxData[0]>>5){  //Switch Server Command Specifier
            case 3:  //response OK
               if(SDO_C->bufferSize <= 4){
                  //expedited transfer
                  SDO_C->state = 0;
                  SDO_C->CANrxNew = 0;
                  return 0;
               }
               else{
                  //segmented transfer - prepare the first segment
                  SDO_C->bufferOffset = 0;
                  SDO_C->state = 0x22;
                  //continue with segmented download
               }
               break;
            case 4:  //abort by server
               memcpySwap4((UNSIGNED8*)pSDOabortCode , &SDO_C->CANrxData[4]);
               SDO_C->state = 0;
               SDO_C->CANrxNew = 0;
               return -10;
            default:
               *pSDOabortCode = 0x05040001L; //Client/server command specifier not valid or unknown
               SDO_C->state = 0;
               SDO_C->CANrxNew = 0;
               return -10;
         }
      }
      if(SDO_C->state & 0x02){
         UNSIGNED8 i,j;

         //segmented download in progress
         if(SDO_C->state & 0x20){   //is the first segment?
            SDO_C->state = 0x02;
         }
         else{
            //verify response from previous segment sent
            switch(SDO_C->CANrxData[0]>>5){  //Switch Server Command Specifier
               case 1:  //response OK
                  //verify toggle bit
                  if((SDO_C->CANrxData[0]&0x10) != (SDO_C->state&0x10)){
                     *pSDOabortCode = 0x05030000L;  //toggle bit not alternated
                     SDO_C->state = 0;
                     SDO_C->CANrxNew = 0;
                     return -10;
                  }
                  //is end of transfer?
                  if(SDO_C->bufferOffset == SDO_C->bufferSize){
                     SDO_C->state = 0;
                     SDO_C->CANrxNew = 0;
                     return 0;
                  }
                  //alternate toggle bit
                  if(SDO_C->state&0x10) SDO_C->state &= 0xEF;
                  else SDO_C->state |= 0x10;
                  break;
               case 4:  //abort by server
                  memcpySwap4((UNSIGNED8*)pSDOabortCode , &SDO_C->CANrxData[4]);
                  SDO_C->state = 0;
                  SDO_C->CANrxNew = 0;
                  return -10;
               default:
                  *pSDOabortCode = 0x05040001L; //Client/server command specifier not valid or unknown
                  SDO_C->state = 0;
                  SDO_C->CANrxNew = 0;
                  return -10;
            }
         }
         //calculate length to be sent
         j = SDO_C->bufferSize - SDO_C->bufferOffset;
         if(j > 7) j = 7;
         //fill data bytes
         for(i=0; i<j; i++)
            SDO_C->CANtxBuff->data[i+1] = SDO_C->buffer[SDO_C->bufferOffset + i];
         for(; i<7; i++)
            SDO_C->CANtxBuff->data[i+1] = 0;
         SDO_C->bufferOffset += j;
         //SDO command specifier
         SDO_C->CANtxBuff->data[0] = 0x00 | (SDO_C->state&0x10) | ((7-j)<<1);
         //is end of transfer?
         if(SDO_C->bufferOffset == SDO_C->bufferSize){
            SDO_C->CANtxBuff->data[0] |= 1;
         }
         //Send next SDO message
         CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);
         SDO_C->CANrxNew = 0;
         return 2;
      }
   }
   else{
      //verify timeout
      if(SDO_C->timeoutTimer < SDOtimeoutTime) SDO_C->timeoutTimer += timeDifference_ms;
      if(SDO_C->timeoutTimer >= SDOtimeoutTime){
         *pSDOabortCode = 0x05040000L;  //SDO protocol timed out
         SDO_C->state = 0;
         SDO_C->CANrxNew = 0;
         return -11;
      }
      return 1;
   }

   return 0;
}


/******************************************************************************/
INTEGER8 CO_SDOclientUploadInitiate(   CO_SDOclient_t   *SDO_C,
                                       UNSIGNED16        index,
                                       UNSIGNED8         subIndex,
                                       UNSIGNED8        *dataRx,
                                       UNSIGNED8         dataRxSize)
{
   //verify parameters
   if(dataRx == 0 || dataRxSize < 4) return -2;

   //save parameters
   SDO_C->buffer = dataRx;
   SDO_C->bufferSize = dataRxSize;
   SDO_C->state = 0x40;

   //prepare CAN tx message
   SDO_C->CANtxBuff->data[0] = 0x40;
   SDO_C->CANtxBuff->data[1] = index & 0xFF;
   SDO_C->CANtxBuff->data[2] = index >> 8;
   SDO_C->CANtxBuff->data[3] = subIndex;
   *((UNSIGNED32 *)(&SDO_C->CANtxBuff->data[4])) = 0;

   //if nodeIDOfTheSDOServer == node-ID of this node, then exchange data with this node
   if(SDO_C->ObjDict_SDOClientParameter->nodeIDOfTheSDOServer == SDO_C->SDO->nodeId){
      return 0;
   }

   //empty receive buffer, reset timeout timer and send message
   SDO_C->CANrxNew = 0;
   SDO_C->timeoutTimer = 0;
   CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

   return 0;
}


/******************************************************************************/
INTEGER8 CO_SDOclientUpload(  CO_SDOclient_t      *SDO_C,
                              UNSIGNED16           timeDifference_ms,
                              UNSIGNED16           SDOtimeoutTime,
                              UNSIGNED16          *pDataSize,
                              UNSIGNED32          *pSDOabortCode)
{
   //clear abort code
   *pSDOabortCode = 0;

   //verify parameters
   if(!(SDO_C->state&0x44)){
      //communication was not properly initiaded
      SDO_C->state = 0;
      SDO_C->CANrxNew = 0;
      return -3;
   }

   //if nodeIDOfTheSDOServer == node-ID of this node, then exchange data with this node
   if(SDO_C->SDO && SDO_C->ObjDict_SDOClientParameter->nodeIDOfTheSDOServer == SDO_C->SDO->nodeId){
      const sCO_OD_object* pODE;
      UNSIGNED8 length, attr;
      UNSIGNED16 indexOfFoundObject;
      UNSIGNED16 index = (UNSIGNED16)SDO_C->CANtxBuff->data[2]<<8 | SDO_C->CANtxBuff->data[1];
      UNSIGNED8 subIndex = SDO_C->CANtxBuff->data[3];

      pODE = CO_OD_find(SDO_C->SDO, index, &indexOfFoundObject);
      SDO_C->state = 0;
      SDO_C->CANrxNew = 0;
      if(pODE == 0){
         *pSDOabortCode = 0x06020000L;  //object does not exist in OD
         return -10;
      }
      if(subIndex > pODE->maxSubIndex){
         *pSDOabortCode = 0x06090011L;  //Sub-index does not exist.
         return -10;
      }
      length = CO_OD_getLength(pODE, subIndex);
      if(SDO_C->bufferSize < length){
         *pSDOabortCode = 0x05040005L;  //Out of memory
         return -10;
      }
      attr = CO_OD_getAttribute(pODE, subIndex);
      if(!(attr&CO_ODA_READABLE)){
         *pSDOabortCode = 0x06010001L;  //attempt to read a write-only object
         return -10;
      }
      //set data size
      *pDataSize = length;
      //copy data from OD
      *pSDOabortCode = pODE->pFunct(SDO_C->SDO->ObjectDictionaryPointers[indexOfFoundObject],
                                    index,
                                    subIndex,
                                    length,
                                    attr,
                                    0,
                                    (void*)SDO_C->buffer,
                                    CO_OD_getDataPointer(pODE, subIndex));
      if(*pSDOabortCode){
         return -10;
      }
      return 0;
   }
   //check if new SDO object received
   if(SDO_C->CANrxNew){
      SDO_C->timeoutTimer = 0;
      if(SDO_C->state & 0x40){
         //upload initiated
         switch(SDO_C->CANrxData[0]>>5){  //Switch Server Command Specifier
            case 2:  //response OK
               if(SDO_C->CANrxData[0] & 0x02){
                  UNSIGNED8 size;
                  //Expedited transfer
                  if(SDO_C->CANrxData[0] & 0x01)//is size indicated
                     size = 4 - ((SDO_C->CANrxData[0]>>2)&0x03);   //size
                  else size = 4;
                  *pDataSize = size;
                  //copy data
                  while(size--) SDO_C->buffer[size] = SDO_C->CANrxData[4+size];
                  SDO_C->state = 0;
                  SDO_C->CANrxNew = 0;
                  return 0;
               }
               else{
                  //segmented transfer - prepare first segment
                  SDO_C->bufferOffset = 0;
                  SDO_C->state = 0x44;
                  //continue with segmented upload
               }
               break;
            case 4:  //abort by server
               memcpySwap4((UNSIGNED8*)pSDOabortCode , &SDO_C->CANrxData[4]);
               SDO_C->state = 0;
               SDO_C->CANrxNew = 0;
               return -10;
            default:
               *pSDOabortCode = 0x05040001L; //Client/server command specifier not valid or unknown
               SDO_C->state = 0;
               SDO_C->CANrxNew = 0;
               return -10;
         }
      }
      if(SDO_C->state & 0x04){
         //segmented upload in progress
         if(SDO_C->state & 0x40){   //is the first segment?
            SDO_C->state = 0x04;
         }
         else{
            //verify response from previous segment sent
            switch(SDO_C->CANrxData[0]>>5){  //Switch Server Command Specifier
               UNSIGNED8 size;
               case 0:  //response OK
                  //verify toggle bit
                  if((SDO_C->CANrxData[0]&0x10) != (SDO_C->state&0x10)){
                     *pSDOabortCode = 0x05030000L;  //toggle bit not alternated
                     SDO_C->state = 0;
                     SDO_C->CANrxNew = 0;
                     return -10;
                  }
                  //get size
                  size = 7 - ((SDO_C->CANrxData[0]>>1)&0x07);
                  //verify length
                  if((SDO_C->bufferOffset + size) > SDO_C->bufferSize){
                     *pSDOabortCode = 0x05040005L;  //Out of memory
                     SDO_C->state = 0;
                     SDO_C->CANrxNew = 0;
                     return -10;
                  }
                  SDO_C->bufferOffset += size;
                  //copy data to buffer
                  while(size--) SDO_C->buffer[SDO_C->bufferOffset + size] = SDO_C->CANrxData[1 + size];
                  //If no more segments to be uploaded, finish communication
                  if(SDO_C->CANrxData[0] & 0x01){
                     *pDataSize = SDO_C->bufferOffset;
                     SDO_C->state = 0;
                     SDO_C->CANrxNew = 0;
                     return 0;
                  }
                  //alternate toggle bit
                  if(SDO_C->state&0x10) SDO_C->state &= 0xEF;
                  else SDO_C->state |= 0x10;
                  break;
               case 4:  //abort by server
                  memcpySwap4((UNSIGNED8*)pSDOabortCode , &SDO_C->CANrxData[4]);
                  SDO_C->state = 0;
                  SDO_C->CANrxNew = 0;
                  return -10;
               default:
                  *pSDOabortCode = 0x05040001L; //Client/server command specifier not valid or unknown
                  SDO_C->state = 0;
                  SDO_C->CANrxNew = 0;
                  return -10;
            }
         }
         //prepare next segment
         SDO_C->CANtxBuff->data[0] = 0x60 | (SDO_C->state&0x10);
         SDO_C->CANtxBuff->data[1] = 0;
         *((UNSIGNED16 *)(&SDO_C->CANtxBuff->data[2])) = 0;
         *((UNSIGNED32 *)(&SDO_C->CANtxBuff->data[4])) = 0;
         CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);
         SDO_C->CANrxNew = 0;
         return 2;
      }
   }
   else{
      //verify timeout
      if(SDO_C->timeoutTimer < SDOtimeoutTime) SDO_C->timeoutTimer += timeDifference_ms;
      if(SDO_C->timeoutTimer >= SDOtimeoutTime){
         *pSDOabortCode = 0x05040000L;  //SDO protocol timed out
         SDO_C->state = 0;
         SDO_C->CANrxNew = 0;
         return -11;
      }
      return 1;
   }

   return 0;
}
