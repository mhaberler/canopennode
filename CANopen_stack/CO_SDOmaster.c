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
#include "crc16-ccitt.h"

#include <stdlib.h> // for malloc, free



#define SDO_BLOCK_STATE_NOT_ACTIVE                 0x00
#define SDO_BLOCK_STATE_UPLOAD_INPROGRESS          0x23

#define SDO_BLOCK_CRC 0

#define SDO_BLOCK_SIZE_MAX 127 // number of segments in one block

#define SDO_BLOCK_STATE_END                        0xAA
#define SDO_BLOCK_STATE_DOWNLOAD_INITIATED         0x11
#define SDO_BLOCK_STATE_DOWNLOAD_SEGMENT           0x13
#define SDO_BLOCK_STATE_DOWNLOAD_CRC               0x14
#define SDO_BLOCK_STATE_DOWNLOAD_WAITING_BLOCK_ACK 0x16
#define SDO_BLOCK_STATE_DOWNLOAD_WAITING_CRC_ACK   0x17
#define SDO_BLOCK_STATE_UPLOAD_INITIATE            0x21
#define SDO_BLOCK_STATE_UPLOAD_WAITING_CRC         0x25
#define SDO_PST_SIZE                               21
#define SDO_BLOCK_MASK_DOWNLOAD_HEAD               0xc0
#define SDO_BLOCK_MASK_UPLOAD_HEAD                 0xa0
#define SDO_BLOCK_MASK_CC                          0x04
#define SDO_BLOCK_MASK_S                           0x02

//Server command specifier, see DS301
#define SCS_DOWNLOAD_INITIATE          3
#define SCS_DOWNLOAD_SEGMENT           1
#define SCS_UPLOAD_INITIATE            2
#define SCS_UPLOAD_SEGMENT             0
#define SCS_ABORT                      4
#define SCS_DOWNLOAD_BLOCK             5
#define SCS_UPLOAD_BLOCK               6


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

   //Optional signal to RTOS, which can resume task, which handles SDO client.
   if(SDO_C->pFunctSignal)
      SDO_C->pFunctSignal(SDO_C->functArg);

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

   SDO_C->pFunctSignal = 0;
   SDO_C->functArg = 0;

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
void CO_SDOclient_abort(CO_SDOclient_t *SDO_C, UNSIGNED32 code){
   SDO_C->CANtxBuff->data[0] = 0x80;
   SDO_C->CANtxBuff->data[1] = SDO_C->index & 0xFF;
   SDO_C->CANtxBuff->data[2] = (SDO_C->index>>8) & 0xFF;
   SDO_C->CANtxBuff->data[3] = SDO_C->subIndex;
   memcpySwap4(&SDO_C->CANtxBuff->data[4], (UNSIGNED8*)&code);
   CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);
   SDO_C->state = 0;
   SDO_C->CANrxNew = 0;
}


/******************************************************************************/
UNSIGNED8 CO_SDObufferClear (UNSIGNED8 *p, UNSIGNED32 length){
	UNSIGNED32 offset =0;

	if (length ==0)
		return 0;

	do{
		*(p + offset) =0;
		offset +=1;
	}
	while (offset<length);
	return 1;
}
/******************************************************************************

	DOWNLOAD


******************************************************************************/


INTEGER8 CO_SDOclientDownloadInitiate( CO_SDOclient_t   *SDO_C,
                                       UNSIGNED16        index,
                                       UNSIGNED8         subIndex,
                                       UNSIGNED8        *dataTx,
                                       UNSIGNED32        dataSize,
                                       UNSIGNED8         blockEnable)
{
   //verify parameters
   if(dataTx == 0 || dataSize == 0) return -2;

   //save parameters
   SDO_C->buffer = dataTx;
   SDO_C->bufferSize = dataSize;
   SDO_C->state = 0x20;

   //prepare CAN tx message
   SDO_C->index = index;
   SDO_C->subIndex = subIndex;
   SDO_C->CANtxBuff->data[1] = index & 0xFF;
   SDO_C->CANtxBuff->data[2] = index >> 8;
   SDO_C->CANtxBuff->data[3] = subIndex;

   //if nodeIDOfTheSDOServer == node-ID of this node, then exchange data with this node
   if(SDO_C->ObjDict_SDOClientParameter->nodeIDOfTheSDOServer == SDO_C->SDO->nodeId){
      return 0;
   }

   //continue: prepare dataBuff for CAN message
   SDO_C->block_state = SDO_BLOCK_STATE_NOT_ACTIVE;

   if(dataSize <= 4){
      UNSIGNED16 i;
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
   else if(blockEnable != 0){ // Try to initiate block transfer
      // set state of block transfer
      SDO_C->block_state = SDO_BLOCK_STATE_DOWNLOAD_INITIATED;

      // init HEAD of packet
      SDO_C->CANtxBuff->data[0] = SDO_BLOCK_MASK_DOWNLOAD_HEAD;
      // set flag for CRC
      if(SDO_BLOCK_CRC)
         SDO_C->CANtxBuff->data[0] |= SDO_BLOCK_MASK_CC;

      // size indicator
      // TODO: use of size indicator
      if(1){
         // size indicator:
         SDO_C->CANtxBuff->data[0] |= SDO_BLOCK_MASK_S;
         // set length of data
         SDO_C->CANtxBuff->data[4] = (UNSIGNED8) dataSize;
         SDO_C->CANtxBuff->data[5] = (UNSIGNED8) (dataSize >> 8);
         SDO_C->CANtxBuff->data[6] = (UNSIGNED8) (dataSize >> 16);
         SDO_C->CANtxBuff->data[7] = (UNSIGNED8) (dataSize >> 24);
      }
      else{
         // no size indicator
         SDO_C->CANtxBuff->data[4] = 0;
         SDO_C->CANtxBuff->data[5] = 0;
         SDO_C->CANtxBuff->data[6] = 0;
         SDO_C->CANtxBuff->data[7] = 0;
      }
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
      //If SDO server is busy return error
      if(SDO_C->SDO->state != 0){
         return -9;
      }

      //init ODF_arg
      *pSDOabortCode = CO_SDO_initTransfer(SDO_C->SDO, SDO_C->index, SDO_C->subIndex);
      if(*pSDOabortCode){
         return -10;
      }

      //set buffer
      SDO_C->SDO->ODF_arg.data = SDO_C->buffer;

      //write data to the Object dictionary
      *pSDOabortCode = CO_SDO_writeOD(SDO_C->SDO, SDO_C->bufferSize);
      if(*pSDOabortCode){
         return -10;
      }

      SDO_C->state = 0;
      SDO_C->CANrxNew = 0;
      return 0;
   }
   //check if new SDO segmented or expedited object received
   if(SDO_C->CANrxNew && SDO_C->block_state == SDO_BLOCK_STATE_NOT_ACTIVE){
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
               CO_SDOclient_abort(SDO_C, *pSDOabortCode);
               return -10;
         }
      }
      if(SDO_C->state & 0x02){
         UNSIGNED16 i, j;

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
                     CO_SDOclient_abort(SDO_C, *pSDOabortCode);
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
                  CO_SDOclient_abort(SDO_C, *pSDOabortCode);
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
   else if(SDO_C->CANrxNew){
      //block transfer in progres
      SDO_C->timeoutTimer = 0;
      switch (SDO_C->block_state){
         case SDO_BLOCK_STATE_DOWNLOAD_INITIATED:{ //waiting on reply on block download initiated
            // check index & subindex
            UNSIGNED16 IndexTmp;

            switch (SDO_C->CANrxData[0] >> 5){
               case 4: // abort command
                  SDO_C->CANrxNew = 0;
                  return -1;
                  break;
               case 5: // block
                  IndexTmp = SDO_C->CANrxData[2];
                  IndexTmp = IndexTmp << 8;
                  IndexTmp |= SDO_C->CANrxData[1];

                  if(IndexTmp != SDO_C->index || SDO_C->CANrxData[3] != SDO_C->subIndex) {
                     // TODO: wrong index
                  }
                  // set blksize
                  SDO_C->block_blksize = SDO_C->CANrxData[4];
                  //printf("\n DEBUG DW init block_blksize= %d\n", SDO_C->block_blksize);

                  SDO_C->block_seqno = 0;
                  SDO_C->bufferOffset = 0;
                  SDO_C->bufferOffsetACK = 0;
                  SDO_C->block_state = SDO_BLOCK_STATE_DOWNLOAD_SEGMENT;

                  break;
            }
            break;
         }
         case SDO_BLOCK_STATE_DOWNLOAD_WAITING_BLOCK_ACK:{
            // check server subcommand
            if((SDO_C->CANrxData[0] & 0x02) == 0){
               // TODO:  wrong server sub command insert download interupted and error code
            }
            // check number of segments
            if(SDO_C->CANrxData[1] != SDO_C->block_blksize){
               // NOT all segments transfert sucesfuly
               SDO_C->bufferOffsetACK = SDO_C->CANrxData[1] * 7;
               SDO_C->bufferOffset = SDO_C->bufferOffsetACK;

            }
            else{
               SDO_C->bufferOffsetACK = SDO_C->bufferOffset;
            }
            // set size of next block
            SDO_C->block_blksize = SDO_C->CANrxData[2];
            SDO_C->block_seqno = 0;

			//printf("\nDEBUG WaitingBlockACK %d %d\n",SDO_C->bufferOffset, SDO_C->bufferSize);
            if(SDO_C->bufferOffset >= SDO_C->bufferSize)
               SDO_C->block_state = SDO_BLOCK_STATE_DOWNLOAD_CRC;
            else
               SDO_C->block_state = SDO_BLOCK_STATE_DOWNLOAD_SEGMENT;

            SDO_C->CANrxNew = 0;
            return 4;
         }
         case SDO_BLOCK_STATE_DOWNLOAD_WAITING_CRC_ACK:{
            if((SDO_C->CANrxData[0] & 0x01) == 0){
               // TODO wrong server sub command
            }
            // SDO bloct download sucesfuly transferd
            SDO_C->block_state = SDO_BLOCK_STATE_END;
            SDO_C->CANrxNew = 0;
            return 0;
         }
         case SDO_BLOCK_STATE_DOWNLOAD_SEGMENT:{ // check if
            UNSIGNED16 IndexTmp;

            if((SDO_C->CANrxData[0] >> 5) == 4){
               IndexTmp = SDO_C->CANrxData[2];
               IndexTmp = IndexTmp << 8;
               IndexTmp |= SDO_C->CANrxData[1];

               if(SDO_C->index == IndexTmp && SDO_C->subIndex == SDO_C->CANrxData[3]){
                  // transfer is aborted
                  SDO_C->block_state
                        = SDO_BLOCK_STATE_NOT_ACTIVE;
                  SDO_C->CANrxNew = 0;
                  return -10;
               }
            }
         }

         default:{
            // unknown
            // TODO unknown
            return -16;
         }
      }
      SDO_C->CANrxNew = 0;
   }
   else{
      //verify timeout
      if(SDO_C->timeoutTimer < SDOtimeoutTime) SDO_C->timeoutTimer += timeDifference_ms;
      if(SDO_C->timeoutTimer >= SDOtimeoutTime){
         *pSDOabortCode = 0x05040000L;  //SDO protocol timed out
         CO_SDOclient_abort(SDO_C, *pSDOabortCode);
         return -11;
      }
   }

   if(SDO_C->block_state == SDO_BLOCK_STATE_DOWNLOAD_SEGMENT){
      SDO_C->block_seqno += 1;
      SDO_C->CANtxBuff->data[0] = SDO_C->block_seqno;

      //printf("\nDEBUG DW SEG clock_seqno %d, block_blksize %d\n",
      //      SDO_C->block_seqno, SDO_C->block_blksize);

      if(SDO_C->block_seqno >= SDO_C->block_blksize){
         SDO_C->block_state = SDO_BLOCK_STATE_DOWNLOAD_WAITING_BLOCK_ACK;
      }
      // set data
      SDO_C->block_noData = 0;
      UNSIGNED8 i;
      for(i = 1; i < 8; i++){
         if(SDO_C->bufferOffset < SDO_C->bufferSize){
            SDO_C->CANtxBuff->data[i] = *(SDO_C->buffer
                  + SDO_C->bufferOffset);
         }
         else{
            SDO_C->CANtxBuff->data[i] = 0;
            SDO_C->block_noData += 1;
         }
         SDO_C->bufferOffset += 1;
      }
      if(SDO_C->bufferOffset >= SDO_C->bufferSize){
         SDO_C->CANtxBuff->data[0] |= 0x80;
		 SDO_C->block_blksize = SDO_C->block_seqno;
         SDO_C->block_state = SDO_BLOCK_STATE_DOWNLOAD_WAITING_BLOCK_ACK;
      }
      // tx data
      SDO_C->timeoutTimer = 0;
      CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

      return 1;
   }
   else if(SDO_C->block_state == SDO_BLOCK_STATE_DOWNLOAD_CRC){
      // set head
      SDO_C->CANtxBuff->data[0] = SDO_BLOCK_MASK_DOWNLOAD_HEAD
            | (SDO_C->block_noData << 2) | 0x01;

      // TODO calculate CRC
      SDO_C->CANtxBuff->data[1] = 0x12;
      SDO_C->CANtxBuff->data[2] = 0x34;

      // reserved
      SDO_C->CANtxBuff->data[3] = 0;
      SDO_C->CANtxBuff->data[4] = 0;
      SDO_C->CANtxBuff->data[5] = 0;
      SDO_C->CANtxBuff->data[6] = 0;
      SDO_C->CANtxBuff->data[7] = 0;

      // set state
      SDO_C->block_state = SDO_BLOCK_STATE_DOWNLOAD_WAITING_CRC_ACK;
      // tx data
      SDO_C->timeoutTimer = 0;
      CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

      return 1;
   }

   return 1;
}

/******************************************************************************

	UPLOAD

******************************************************************************/
INTEGER8 CO_SDOclientUploadInitiate(   CO_SDOclient_t   *SDO_C,
                                       UNSIGNED16        index,
                                       UNSIGNED8         subIndex,
                                       UNSIGNED8        *dataRx,
                                       UNSIGNED32        dataRxSize,
                                       UNSIGNED8         blockEnable)
{
   //verify parameters
	if(dataRx == 0 || dataRxSize < 4) return -2;

	//save parameters
	SDO_C->buffer = dataRx;
	SDO_C->bufferSize = dataRxSize;
	SDO_C->state = 0x40;

	//prepare CAN tx message
	CO_SDObufferClear (&SDO_C->CANtxBuff->data[0], 8);

	SDO_C->index = index;
	SDO_C->subIndex = subIndex;

	SDO_C->CANtxBuff->data[1] = index & 0xFF;
	SDO_C->CANtxBuff->data[2] = index >> 8;
	SDO_C->CANtxBuff->data[3] = subIndex;


	if(blockEnable == 0){
		SDO_C->state = SDO_STATE_UPLOAD_INITIATED;
		SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_INITIATE<<5);
	}
	else{
		SDO_C->state = SDO_STATE_BLOCKUPLOAD_INITIATE;

		// header
		SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_BLOCK<<5);

		// set CRC
		SDO_C->CANtxBuff->data[0] |= 0x04;

		// set number of segments in block
		SDO_C->block_blksize = SDO_BLOCK_SIZE_MAX;
		if ((SDO_C->block_blksize *7) > SDO_C->bufferSize){
		  return -3;
		}

		SDO_C->CANtxBuff->data[4] = SDO_C->block_blksize;
		SDO_C->CANtxBuff->data[5] = SDO_PST_SIZE;

		SDO_C->block_seqno = 0;
	}

	//if nodeIDOfTheSDOServer == node-ID of this node, then exchange data with this node
	if(SDO_C->ObjDict_SDOClientParameter->nodeIDOfTheSDOServer == SDO_C->SDO->nodeId){
		return 0;
	}

	//empty receive buffer, reset timeout timer and send message
	SDO_C->CANrxNew = 0;
	SDO_C->timeoutTimer = 0;
	CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

	//printf ("DEBUG: UPLOAT INITIATE STATE = %d", SDO_C->state);
	return 0;
}


/******************************************************************************/

INTEGER8 CO_SDOclientUpload(  CO_SDOclient_t      *SDO_C,
                              UNSIGNED16           timeDifference_ms,
                              UNSIGNED16           SDOtimeoutTime,
                              UNSIGNED32          *pDataSize,
                              UNSIGNED32          *pSDOabortCode)
{
	UNSIGNED16 indexTmp;
	UNSIGNED32 tmp32;

	//clear abort code
	*pSDOabortCode = 0;

	// data from self node ID;
	if(SDO_C->SDO && SDO_C->ObjDict_SDOClientParameter->nodeIDOfTheSDOServer == SDO_C->SDO->nodeId){
		//If SDO server is busy return error
		if(SDO_C->SDO->state != 0){
			*pSDOabortCode = 0x06040047L;   //General internal incompatibility in the device.
			return -9;
		}

		//init ODF_arg
		*pSDOabortCode = CO_SDO_initTransfer(SDO_C->SDO, SDO_C->index, SDO_C->subIndex);
		if(*pSDOabortCode){
			return -10;
		}

		//set buffer and length if domain
		SDO_C->SDO->ODF_arg.data = SDO_C->buffer;
		if(SDO_C->SDO->ODF_arg.ODdataStorage == 0)
			SDO_C->SDO->ODF_arg.dataLength = SDO_C->bufferSize;

		//read data from the Object dictionary
		*pSDOabortCode = CO_SDO_readOD(SDO_C->SDO, SDO_C->bufferSize);
		if(*pSDOabortCode){
			return -10;
		}

		//set data size
		*pDataSize = SDO_C->SDO->ODF_arg.dataLength;

		//is SDO buffer too small
		if(SDO_C->SDO->ODF_arg.lastSegment == 0){
			*pSDOabortCode = 0x05040005L;  //Out of memory
			return -10;
		}

		SDO_C->state = 0;
		SDO_C->CANrxNew = 0;
		return 0;
	}


// RX data ********************************************************************************
	if(SDO_C->CANrxNew){
		UNSIGNED8 SCS = SDO_C->CANrxData[0]>>5;   //Client command specifier

		// ABORT
		if (SDO_C->CANrxData[0] == (SCS_ABORT<<5)){
			SDO_C->state = SDO_STATE_IDLE;
			SDO_C->CANrxNew  =0;
			memcpySwap4((UNSIGNED8*)pSDOabortCode , &SDO_C->CANrxData[4]);
			return SDO_RETURN_END_SERVERABORT;
		}

		//printf ("\n DEBUG: RXdata STATE = %d", SDO_C->state);
		switch (SDO_C->state){
			case SDO_STATE_UPLOAD_INITIATED:{
				if (SCS == SCS_UPLOAD_INITIATED){
					if(SDO_C->CANrxData[0] & 0x02){
						UNSIGNED8 size;
						//Expedited transfer
						if(SDO_C->CANrxData[0] & 0x01)//is size indicated
							size = 4 - ((SDO_C->CANrxData[0]>>2)&0x03);   //size
						else
							size = 4;

						*pDataSize = size;

						//copy data
						while(size--) SDO_C->buffer[size] = SDO_C->CANrxData[4+size];
						SDO_C->state = 0;
						SDO_C->CANrxNew = 0;

						return SDO_RETURN_COMMUNICATION_END;
					}
					else{
						//segmented transfer - prepare first segment
						SDO_C->bufferOffset = 0;
						SDO_C->state = SDO_STATE_UPLOAD_REQUEST;

						SDO_C->toggle =0;
						//continue with segmented upload
					}
				}
				else{
					*pSDOabortCode = 0x05040001L; //Client/server command specifier not valid or unknown
					SDO_C->state = SDO_STATE_ABORT;
				}
				break;
			}

			case SDO_STATE_UPLOAD_RESPONSE:{
				if (SCS == SCS_UPLOAD_SEGMENT){
					 UNSIGNED16 size, i;
					//verify toggle bit
					if((SDO_C->CANrxData[0] &0x10) != (~SDO_C->toggle &0x10)){
						*pSDOabortCode = 0x05030000L;  //toggle bit not alternated
						SDO_C->state = SDO_STATE_ABORT;
						break;
					}
					//get size
					size = 7 - ((SDO_C->CANrxData[0]>>1)&0x07);
					//verify length
					if((SDO_C->bufferOffset + size) > SDO_C->bufferSize){
						*pSDOabortCode = 0x05040005L;  //Out of memory
						SDO_C->state = SDO_STATE_ABORT;
						break;
					}
					//copy data to buffer
					for(i=0; i<size; i++)
					SDO_C->buffer[SDO_C->bufferOffset + i] = SDO_C->CANrxData[1 + i];
					SDO_C->bufferOffset += size;
					//If no more segments to be uploaded, finish communication
					if(SDO_C->CANrxData[0] & 0x01){
						*pDataSize = SDO_C->bufferOffset;
						SDO_C->state = SDO_STATE_NOTDEFINED;
						SDO_C->CANrxNew = 0;
						return SDO_RETURN_COMMUNICATION_END;
					}
					//set state
					SDO_C->state = SDO_STATE_UPLOAD_REQUEST;
					break;
				}
				else{
					*pSDOabortCode = 0x05040001L; //Client/server command specifier not valid or unknown
					SDO_C->state = SDO_STATE_ABORT;
				}
				break;
			}

			case SDO_STATE_BLOCKUPLOAD_INITIATE:{
				if (SCS == SCS_UPLOAD_BLOCK){ // block upload initiate response

					SDO_C->state = SDO_STATE_BLOCKUPLOAD_INITIATE_ACK;

					// SCR support
					if((SDO_C->CANrxData[0] & 0x04) != 0)
						SDO_C->crcEnabled = 1; // CRC suported
					else
						SDO_C->crcEnabled = 0; // CRC not suported

					// chech Index ans subnindex
					indexTmp = SDO_C->CANrxData[2];
					indexTmp = indexTmp << 8;
					indexTmp |= SDO_C->CANrxData[1];

					if(indexTmp != SDO_C->index || SDO_C->CANrxData[3] != SDO_C->subIndex){
						*pSDOabortCode = 0x06040043L;
						SDO_C->state = SDO_STATE_ABORT;
					}

					// set length
					if(SDO_C->CANrxData[0]&0x02){
						UNSIGNED32 len;
						memcpySwap4((UNSIGNED8*)&len, &SDO_C->CANrxData[4]);
						SDO_C->dataSize = len;
					}
					else{
						SDO_C->dataSize = 0;
					}

					// check available buffer size
					if (SDO_C->dataSize > SDO_C->bufferSize){
						*pSDOabortCode = 0x05040005;
						SDO_C->state = SDO_STATE_ABORT;
					}

					SDO_C->dataSizeTransfered =0;
				}
				else if (SCS == SCS_UPLOAD_INITIATE){ // switch to regular segmented transfere
					if(SDO_C->CANrxData[0] & 0x02){
						UNSIGNED8 size;
						//Expedited transfer
						if(SDO_C->CANrxData[0] & 0x01)//is size indicated
							size = 4 - ((SDO_C->CANrxData[0]>>2)&0x03);   //size
						else
							size = 4;

						*pDataSize = size;

						//copy data
						while(size--) SDO_C->buffer[size] = SDO_C->CANrxData[4+size];
						SDO_C->state = 0;
						SDO_C->CANrxNew = 0;

						return SDO_RETURN_COMMUNICATION_END;
					}
					else{
						//segmented transfer - prepare first segment
						SDO_C->bufferOffset = 0;
						SDO_C->state = SDO_STATE_UPLOAD_REQUEST;

						SDO_C->toggle =0;
						//continue with segmented upload
					}
				}
				else{ // unknown SCS
					*pSDOabortCode = 0x05040001L; //Client command specifier not valid or unknown.
					SDO_C->state = SDO_STATE_ABORT;
				}
				break;
			}

			case SDO_STATE_BLOCKUPLOAD_INPROGRES:{
				// check correct seqno
				SDO_C->timeoutTimerBLOCK = 0;
				if((SDO_C->CANrxData[0] & 0x7f) != (SDO_C->block_seqno +1)){


					// wait block tmo to send block ack
					if (SDO_C->block_seqno ==0){
					}
					else if ((SDO_C->CANrxData[0] & 0x7f) != SDO_C->block_seqno ){
						SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_ACK;
					}
					else if (SDO_C->CANrxData[0] & 0x80)
						SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_ACK_LAST;

				}
				else{
					UNSIGNED8 i;
					for (i=1; i<8; i++){
						*(SDO_C->buffer + SDO_C->dataSizeTransfered) = SDO_C->CANrxData[i];
						SDO_C->dataSizeTransfered +=1;
						if (SDO_C->dataSizeTransfered >= SDO_C->bufferSize){
							*pSDOabortCode = 0x05040005;
							SDO_C->state = SDO_STATE_ABORT;
							break;
						}
					}
					SDO_C->block_seqno += 1;

					if (*pSDOabortCode ==0){
						if (SDO_C->CANrxData[0]&0x80){
							if (SDO_C->dataSize !=0){
								if (SDO_C->dataSize > SDO_C->dataSizeTransfered){
									*pSDOabortCode = 0x06070010L;
									SDO_C->state = SDO_STATE_ABORT;
								}
								else
									SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_ACK_LAST;
							}
							else
								SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_ACK_LAST;
						}
						else{
							if(SDO_C->block_seqno >= SDO_C->block_blksize){ // last segment in block
									SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_ACK;
							}

						}
					}
					SDO_C->CANrxNew = 0;
				}
				break;
			}

			case SDO_STATE_BLOCKUPLOAD_BLOCK_CRC:{
				if (SCS == SCS_UPLOAD_BLOCK){
					tmp32 = ((SDO_C->CANrxData[0]>>2) & 0x07);
					SDO_C->dataSizeTransfered -= tmp32;

					SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_END;
					if (SDO_C->crcEnabled){
						UNSIGNED16 tmp16;
						memcpySwap2((UNSIGNED8*)&tmp16, &SDO_C->CANrxData[1]);

						if (tmp16 != crc16_ccitt((unsigned char  *)SDO_C->buffer, (unsigned int)SDO_C->dataSizeTransfered, 0)){
							*pSDOabortCode = 0x05040004L;
							SDO_C->state = SDO_STATE_ABORT;
						}
						else{
							SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_END;
						}
					}
					else{
						SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_END;
					}

				}
				else{
					*pSDOabortCode = 0x08000000L;
					SDO_C->state = SDO_STATE_ABORT;
				}
				break;
			}

			default:{
				*pSDOabortCode = 0x05040001L; //Client command specifier not valid or unknown.
				SDO_C->state = SDO_STATE_ABORT;
			}
		}
		SDO_C->timeoutTimer = 0;
		SDO_C->CANrxNew = 0;
	}

// TMO ***************************************************************************************************
	if(SDO_C->timeoutTimer < SDOtimeoutTime){
		SDO_C->timeoutTimer += timeDifference_ms;
		if (SDO_C->state == SDO_STATE_BLOCKUPLOAD_INPROGRES)
			SDO_C->timeoutTimerBLOCK += timeDifference_ms;
	}
	if(SDO_C->timeoutTimer >= SDOtimeoutTime){ // comunication TMO
		*pSDOabortCode = 0x05040000L;  //SDO protocol timed out
		CO_SDOclient_abort(SDO_C, *pSDOabortCode);
		return SDO_RETURN_END_TMO;
	}
	if(SDO_C->timeoutTimerBLOCK >= (SDOtimeoutTime/2)){ // block TMO
		SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_ACK;
	}


// TX data ********************************************************************************
	CO_SDObufferClear (&SDO_C->CANtxBuff->data[0], 8);

	switch (SDO_C->state){
		case SDO_STATE_ABORT:{
			SDO_C->state = SDO_STATE_NOTDEFINED;
			CO_SDOclient_abort (SDO_C, *pSDOabortCode);
			return SDO_RETURN_END_SERVERABORT;
		}

		// SEGMENTED UPLOAD
		case SDO_STATE_UPLOAD_REQUEST:{
			SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_SEGMENT<<5) | (SDO_C->toggle & 0x10);
			CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

			SDO_C->state = SDO_STATE_UPLOAD_RESPONSE;
			SDO_C->toggle = ~SDO_C->toggle;

			return SDO_RETURN_WAITING_SERVER_RESPONSE;
		}
		// BLOCK
		case SDO_STATE_BLOCKUPLOAD_INITIATE_ACK:{
			//printf ("\n DEBUG: TXdata STATE = %d; INITIATE_ACK", SDO_C->state);
			SDO_C->timeoutTimerBLOCK = 0;
			SDO_C->block_seqno = 0;
			SDO_C->state = SDO_STATE_BLOCKUPLOAD_INPROGRES;

			// header
			SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_BLOCK<<5) | 0x03;
			CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

			return SDO_RETURN_BLOCKUPLOAD_INPROGRES;
		}

		case SDO_STATE_BLOCKUPLOAD_BLOCK_ACK_LAST:{
			//printf ("\n DEBUG: SDO_STATE_BLOCKUPLOAD_BLOCK_ACK_LAST");
			// header
			SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_BLOCK<<5) | 0x02;
			SDO_C->CANtxBuff->data[1] = SDO_C->block_seqno;

			SDO_C->block_seqno = 0;
			SDO_C->timeoutTimerBLOCK = 0;

			SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_CRC;

			SDO_C->CANtxBuff->data[2] = SDO_C->block_blksize;
			CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

			return SDO_RETURN_BLOCKUPLOAD_INPROGRES;
		}

		case SDO_STATE_BLOCKUPLOAD_BLOCK_ACK:{
			// header
			SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_BLOCK<<5) | 0x02;
			SDO_C->CANtxBuff->data[1] = SDO_C->block_seqno;

			//printf ("\n DEBUG: SDO_STATE_BLOCKUPLOAD_BLOCK_ACK_LAST");


			// set next block size
			if (SDO_C->dataSize != 0){
				if(SDO_C->dataSizeTransfered >= SDO_C->dataSize){
					SDO_C->block_blksize = 0;
					SDO_C->state = SDO_STATE_BLOCKUPLOAD_BLOCK_CRC;
					//printf ("\n DEBUG: SDO_STATE_BLOCKUPLOAD_BLOCK_CRC");
				}
				else{
					tmp32 = ((SDO_C->dataSize - SDO_C->dataSizeTransfered) / 7);
					if(tmp32 >= SDO_BLOCK_SIZE_MAX){
						SDO_C->block_blksize = SDO_BLOCK_SIZE_MAX;
					}
					else{
						if((SDO_C->dataSize - SDO_C->dataSizeTransfered) % 7 == 0)
							SDO_C->block_blksize = tmp32;
						else
							SDO_C->block_blksize = tmp32 + 1;
					}
					SDO_C->block_seqno = 0;
					SDO_C->timeoutTimerBLOCK = 0;
					SDO_C->state = SDO_STATE_BLOCKUPLOAD_INPROGRES;
					//printf ("\n DEBUG: SDO_STATE_BLOCKUPLOAD_INPROGRES");
				}
			}
			else{
				SDO_C->block_seqno = 0;
				SDO_C->timeoutTimerBLOCK = 0;

				SDO_C->state = SDO_STATE_BLOCKUPLOAD_INPROGRES;
			}
			SDO_C->CANtxBuff->data[2] = SDO_C->block_blksize;
			CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);

			return SDO_RETURN_BLOCKUPLOAD_INPROGRES;
		}

		case SDO_STATE_BLOCKUPLOAD_BLOCK_END:{
			//printf ("\n DEBUG: TXdata STATE = %d; BLOCK_END", SDO_C->state);
			SDO_C->CANtxBuff->data[0] = (CCS_UPLOAD_BLOCK<<5) | 0x01;

			CO_CANsend(SDO_C->CANdevTx, SDO_C->CANtxBuff);


			*pDataSize = SDO_C->dataSizeTransfered;
			return SDO_RETURN_COMMUNICATION_END;
		}

		default:
			break;
	}

	return SDO_RETURN_WAITING_SERVER_RESPONSE;
}


