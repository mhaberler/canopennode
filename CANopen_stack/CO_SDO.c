/*******************************************************************************

   File - CO_SDO.c
   CANopen SDO server object (SDO slave).

   Copyright (C) 2012 Janez Paternoster

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
#include "crc16-ccitt.h"

#include <stdlib.h> // for malloc, free


//State machine, values of the SDO->state variable
#define STATE_IDLE                     0x00
#define STATE_DOWNLOAD_INITIATE        0x11
#define STATE_DOWNLOAD_SEGMENTED       0x12
#define STATE_DOWNLOAD_BLOCK_INITIATE  0x14
#define STATE_DOWNLOAD_BLOCK_SUBBLOCK  0x15
#define STATE_DOWNLOAD_BLOCK_END       0x16
#define STATE_UPLOAD_INITIATE          0x21
#define STATE_UPLOAD_SEGMENTED         0x22
#define STATE_UPLOAD_BLOCK_INITIATE    0x24
#define STATE_UPLOAD_BLOCK_INITIATE_2  0x25
#define STATE_UPLOAD_BLOCK_SUBBLOCK    0x26
#define STATE_UPLOAD_BLOCK_END         0x27

//Client command specifier, see DS301
#define CCS_DOWNLOAD_INITIATE          1
#define CCS_DOWNLOAD_SEGMENT           0
#define CCS_UPLOAD_INITIATE            2
#define CCS_UPLOAD_SEGMENT             3
#define CCS_DOWNLOAD_BLOCK             6
#define CCS_UPLOAD_BLOCK               5
#define CCS_ABORT                      0x80


#if CO_SDO_BUFFER_SIZE < 7
   #error CO_SDO_BUFFER_SIZE must be greater than 7
#endif

/******************************************************************************/
INTEGER16 CO_SDO_receive(void *object, CO_CANrxMsg_t *msg){
   CO_SDO_t *SDO;

   SDO = (CO_SDO_t*)object;   //this is the correct pointer type of the first argument

   //verify message length
   if(msg->DLC != 8) return CO_ERROR_RX_MSG_LENGTH;

   //verify message overflow (previous message was not processed yet)
   if(SDO->CANrxNew) return CO_ERROR_RX_OVERFLOW;

   //copy data and set 'new message' flag
   SDO->CANrxData[0] = msg->data[0];
   SDO->CANrxData[1] = msg->data[1];
   SDO->CANrxData[2] = msg->data[2];
   SDO->CANrxData[3] = msg->data[3];
   SDO->CANrxData[4] = msg->data[4];
   SDO->CANrxData[5] = msg->data[5];
   SDO->CANrxData[6] = msg->data[6];
   SDO->CANrxData[7] = msg->data[7];

   SDO->CANrxNew = 1;

   return CO_ERROR_NO;
}


/* SDO server parameter - Object dictionary function **************************/
UNSIGNED32 CO_ODF_1200(CO_ODF_arg_t *ODF_arg){
   UNSIGNED8 *nodeId;
   UNSIGNED32 *value;

   nodeId = (UNSIGNED8*) ODF_arg->object;
   value = (UNSIGNED32*) ODF_arg->data;

   //if SDO reading Object dictionary 0x1200, add nodeId to the value
   if(ODF_arg->reading && ODF_arg->subIndex > 0)
      *value += *nodeId;

   return 0;
}


/******************************************************************************/
INTEGER16 CO_SDO_init(
      CO_SDO_t                    **ppSDO,
      UNSIGNED16                    COB_IDClientToServer,
      UNSIGNED16                    COB_IDServerToClient,
      UNSIGNED16                    ObjDictIndex_SDOServerParameter,
const CO_OD_entry_t                *OD,
      UNSIGNED16                    ODSize,
      UNSIGNED8                     nodeId,
      CO_CANmodule_t *CANdevRx, UNSIGNED16 CANdevRxIdx,
      CO_CANmodule_t *CANdevTx, UNSIGNED16 CANdevTxIdx)
{
   CO_SDO_t *SDO;

   //allocate memory if not already allocated
   if((*ppSDO) == NULL){
      if(((*ppSDO) = (CO_SDO_t *) malloc(sizeof(CO_SDO_t))) == NULL){return CO_ERROR_OUT_OF_MEMORY;}
      (*ppSDO)->ODExtensions = 0;   //Memory is allocated in CO_OD_configure() function.
   }

   SDO = *ppSDO; //pointer to (newly created) object

   //Configure object variables
   SDO->OD = OD;
   SDO->ODSize = ODSize;
   SDO->nodeId = nodeId;
   SDO->state = STATE_IDLE;
   SDO->CANrxNew = 0;

   //Configure Object dictionary entry at index 0x1200
   if(ObjDictIndex_SDOServerParameter == 0x1200)
      CO_OD_configure(SDO, ObjDictIndex_SDOServerParameter, CO_ODF_1200, (void*)&SDO->nodeId);

   //configure SDO server CAN reception
   CO_CANrxBufferInit(     CANdevRx,               //CAN device
                           CANdevRxIdx,            //rx buffer index
                           COB_IDClientToServer,   //CAN identifier
                           0x7FF,                  //mask
                           0,                      //rtr
                           (void*)SDO,             //object passed to receive function
                           CO_SDO_receive);        //this function will process received message

   //configure SDO server CAN transmission
   SDO->CANdevTx = CANdevTx;
   SDO->CANtxBuff = CO_CANtxBufferInit(
                           CANdevTx,               //CAN device
                           CANdevTxIdx,            //index of specific buffer inside CAN module
                           COB_IDServerToClient,   //CAN identifier
                           0,                      //rtr
                           8,                      //number of data bytes
                           0);                     //synchronous message flag bit

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_SDO_delete(CO_SDO_t **ppSDO){
   CO_SDO_t *SDO = *ppSDO;

   if(SDO){
      if(SDO->ODExtensions){
         UNSIGNED16 i;
         for(i=0; i<SDO->ODSize; i++){
            if(SDO->ODExtensions[i]) free(SDO->ODExtensions[i]);
         }
         free(SDO->ODExtensions);
      }
      free(SDO);
      SDO = 0;
   }
}


/******************************************************************************/
UNSIGNED16 CO_OD_configure(CO_SDO_t      *SDO,
                           UNSIGNED16     index,
                           UNSIGNED32   (*pODFunc)(CO_ODF_arg_t *ODF_arg),
                           void          *object)
{
   UNSIGNED16 entryNo;
   UNSIGNED16 i;
   UNSIGNED8 maxSubIndex;

   entryNo = CO_OD_find(SDO, index);
   if(entryNo == 0xFFFF) return 0xFFFF;   //index not found in Object dictionary

   //allocate ODExtensions array of pointers if not allready allocated
   if(SDO->ODExtensions == 0){
      SDO->ODExtensions = (void*) malloc(SDO->ODSize * sizeof(CO_OD_extension_t *));
      if(SDO->ODExtensions == 0) return 0xFFFF;   //memory allocation failed

      //clear pointers
      for(i=0; i<SDO->ODSize; i++) SDO->ODExtensions[i] = 0;
   }


   //allocate CO_OD_extension_t
   CO_OD_extension_t *ext = SDO->ODExtensions[entryNo];
   maxSubIndex = SDO->OD[entryNo].maxSubIndex;
   if(ext == 0){
      ext = (CO_OD_extension_t*) malloc(sizeof(CO_OD_extension_t) + maxSubIndex);
      if(ext == 0) return 0xFFFF;   //memory allocation failed
      SDO->ODExtensions[entryNo] = ext;
   }

   ext->pODFunc = pODFunc;
   ext->object = object;
   for(i=0; i<=maxSubIndex; i++)
      ext->flags[i] = 0;

   return entryNo;
}


/******************************************************************************/
UNSIGNED16 CO_OD_find(CO_SDO_t *SDO, UNSIGNED16 index){
   //Fast search in ordered Object Dictionary. If indexes are mixed, this won't work.
   //If Object Dictionary has up to 2^N entries, then N is max number of loop passes.
   UNSIGNED16 cur, min, max;
   const CO_OD_entry_t* object;

   min = 0;
   max = SDO->ODSize - 1;
   while(min < max){
      cur = (min + max) / 2;
      object = &SDO->OD[cur];
      //Is object matched
      if(index == object->index){
         return cur;
      }
      if(index < object->index){
         max = cur;
         if(max) max--;
      }
      else
         min = cur + 1;
   }

   if(min == max){
      object = &SDO->OD[min];
      //Is object matched
      if(index == object->index){
         return min;
      }
   }

   return 0xFFFF;  //object does not exist in OD
}


/******************************************************************************/
UNSIGNED16 CO_OD_getLength(CO_SDO_t *SDO, UNSIGNED16 entryNo, UNSIGNED8 subIndex){
   const CO_OD_entry_t* object = &SDO->OD[entryNo];

   if(entryNo == 0xFFFF) return 0;

   if(object->maxSubIndex == 0){    //Object type is Var
      if(object->pData == 0)//data type is domain
         return CO_SDO_BUFFER_SIZE;
      else
         return object->length;
   }
   else if(object->length){         //Object type is Array
      if(subIndex == 0) return 1;
      else if(object->pData == 0)//data type is domain
         return CO_SDO_BUFFER_SIZE;
      return object->length;
   }
   else{                            //Object type is Record
      if(((const CO_OD_entryRecord_t*)(object->pData))[subIndex].pData == 0)//data type is domain
         return CO_SDO_BUFFER_SIZE;
      else
         return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].length;
   }
}


/******************************************************************************/
UNSIGNED16 CO_OD_getAttribute(CO_SDO_t *SDO, UNSIGNED16 entryNo, UNSIGNED8 subIndex){
   const CO_OD_entry_t* object = &SDO->OD[entryNo];

   if(entryNo == 0xFFFF) return 0;

   if(object->maxSubIndex == 0){    //Object type is Var
      return object->attribute;
   }
   else if(object->length){         //Object type is Array
      UNSIGNED16 attr = object->attribute;
      if(subIndex == 0){
         //First subIndex is readonly
         attr &= ~(CO_ODA_WRITEABLE | CO_ODA_RPDO_MAPABLE);
         attr |= CO_ODA_READABLE;
      }
      return attr;
   }
   else{                            //Object type is Record
      return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].attribute;
   }
}


/******************************************************************************/
void* CO_OD_getDataPointer(CO_SDO_t *SDO, UNSIGNED16 entryNo, UNSIGNED8 subIndex){
   const CO_OD_entry_t* object = &SDO->OD[entryNo];

   if(entryNo == 0xFFFF) return 0;

   if(object->maxSubIndex == 0){    //Object type is Var
      return object->pData;
   }
   else if(object->length){         //Object Type is Array
      if(subIndex==0){
         //this is the data, for the subIndex 0 in the array
         return (void*) &object->maxSubIndex;
      }
      else if(object->pData == 0)//data type is domain
         return 0;
      else
         return (void*)(((INTEGER8*)object->pData) + ((subIndex-1) * object->length));
   }
   else{                            //Object Type is Record
      return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].pData;
   }
}


/******************************************************************************/
UNSIGNED8* CO_OD_getFlagsPointer(CO_SDO_t *SDO, UNSIGNED16 entryNo, UNSIGNED8 subIndex){
   CO_OD_extension_t* obj_ext;

   if(entryNo == 0xFFFF || SDO->ODExtensions == 0) return 0;

   obj_ext = SDO->ODExtensions[entryNo];
   if(obj_ext == 0) return 0;

   return &obj_ext->flags[subIndex];
}


/******************************************************************************/
UNSIGNED32 CO_SDO_initTransfer(CO_SDO_t *SDO, UNSIGNED16 index, UNSIGNED8 subIndex){

   //find object in Object Dictionary
   SDO->entryNo = CO_OD_find(SDO, index);
   if(SDO->entryNo == 0xFFFF)
      return 0x06020000L;     //object does not exist in OD

   //verify existance of subIndex
   if(subIndex > SDO->OD[SDO->entryNo].maxSubIndex)
      return 0x06090011L;     //Sub-index does not exist.

   //pointer to data in Object dictionary
   SDO->ODF_arg.ODdataStorage = CO_OD_getDataPointer(SDO, SDO->entryNo, subIndex);

   //fill ODF_arg
   SDO->ODF_arg.object = 0;
   if(SDO->ODExtensions){
      CO_OD_extension_t *ext = SDO->ODExtensions[SDO->entryNo];
      if(ext) SDO->ODF_arg.object = ext->object;
   }
   SDO->ODF_arg.data = SDO->databuffer;
   SDO->ODF_arg.dataLength = CO_OD_getLength(SDO, SDO->entryNo, subIndex);
   SDO->ODF_arg.attribute = CO_OD_getAttribute(SDO, SDO->entryNo, subIndex);
   SDO->ODF_arg.pFlags = CO_OD_getFlagsPointer(SDO, SDO->entryNo, subIndex);
   SDO->ODF_arg.index = index;
   SDO->ODF_arg.subIndex = subIndex;

   SDO->ODF_arg.firstSegment = 1;
   SDO->ODF_arg.lastSegment = 1;

   //indicate total data length, if not domain
   SDO->ODF_arg.dataLengthTotal = (SDO->ODF_arg.ODdataStorage) ? SDO->ODF_arg.dataLength : 0;

   //verify length
   if(SDO->ODF_arg.dataLength > CO_SDO_BUFFER_SIZE)
      return 0x06040047L;     //general internal incompatibility in the device

   return 0;
}


/******************************************************************************/
UNSIGNED32 CO_SDO_readOD(CO_SDO_t *SDO, UNSIGNED16 SDOBufferSize){
   UNSIGNED8 *SDObuffer = SDO->ODF_arg.data;
   UNSIGNED8 *ODdata = (UNSIGNED8*)SDO->ODF_arg.ODdataStorage;
   UNSIGNED16 length = SDO->ODF_arg.dataLength;
   CO_OD_extension_t *ext = 0;

   //is object readable?
   if(!(SDO->ODF_arg.attribute & CO_ODA_READABLE))
      return 0x06010001L;     //attempt to read a write-only object

   //find extension
   if(SDO->ODExtensions){
      ext = SDO->ODExtensions[SDO->entryNo];
   }

   //copy data from OD to SDO buffer if not domain
   if(ODdata){
      DISABLE_INTERRUPTS();
      while(length--) *(SDObuffer++) = *(ODdata++);
      ENABLE_INTERRUPTS();
   }
   //if domain, Object dictionary function MUST exist
   else{
      if(ext == 0) return 0x06040047L;     //general internal incompatibility in the device
   }

   //call Object dictionary function if registered
   SDO->ODF_arg.reading = 1;
   if(ext){
      UNSIGNED32 abortCode = ext->pODFunc(&SDO->ODF_arg);
      if(abortCode) return abortCode;

      //dataLength (upadted by pODFunc) must be inside limits
      if(SDO->ODF_arg.dataLength == 0 || SDO->ODF_arg.dataLength > SDOBufferSize)
         return 0x06040047L;     //general internal incompatibility in the device
   }
   SDO->ODF_arg.firstSegment = 0;

   //swap data if processor is not little endian (CANopen is)
#ifdef BIG_ENDIAN
   if(SDO->ODF_arg.attribute & CO_ODA_MB_VALUE){
      UNSIGNED16 len = SDO->ODF_arg.dataLength;
      UNSIGNED8 *buf1 = SDO->ODF_arg.data;
      UNSIGNED8 *buf2 = buf1 + len - 1;

      len /= 2;
      while(len--){
         UNSIGNED8 b = *buf1;
         *(buf1++) = *buf2;
         *(buf2--) = b;
      }
   }
#endif

   return 0;
}


/******************************************************************************/
UNSIGNED32 CO_SDO_writeOD(CO_SDO_t *SDO, UNSIGNED16 length){
   UNSIGNED8 *SDObuffer = SDO->ODF_arg.data;
   UNSIGNED8 *ODdata = (UNSIGNED8*)SDO->ODF_arg.ODdataStorage;

   //is object writeable?
   if(!(SDO->ODF_arg.attribute & CO_ODA_WRITEABLE))
      return 0x06010002L;     //attempt to write a read-only object

   //length of domain data is application specific and not verified
   if(ODdata == 0)
      SDO->ODF_arg.dataLength = length;

   //verify length except for domain data type
   else if(SDO->ODF_arg.dataLength != length)
      return 0x06070010L;     //Length of service parameter does not match

   //swap data if processor is not little endian (CANopen is)
#ifdef BIG_ENDIAN
   if(SDO->ODF_arg.attribute & CO_ODA_MB_VALUE){
      UNSIGNED16 len = SDO->ODF_arg.dataLength;
      UNSIGNED8 *buf1 = SDO->ODF_arg.data;
      UNSIGNED8 *buf2 = buf1 + len - 1;

      len /= 2;
      while(len--){
         UNSIGNED8 b = *buf1;
         *(buf1++) = *buf2;
         *(buf2--) = b;
      }
   }
#endif

   //call Object dictionary function if registered
   SDO->ODF_arg.reading = 0;
   if(SDO->ODExtensions){
      CO_OD_extension_t *ext = SDO->ODExtensions[SDO->entryNo];

      if(ext){
         UNSIGNED32 abortCode = ext->pODFunc(&SDO->ODF_arg);
         if(abortCode) return abortCode;
      }
   }
   SDO->ODF_arg.firstSegment = 0;

   //copy data from SDO buffer to OD if not domain
   if(ODdata){
      DISABLE_INTERRUPTS();
      while(length--) *(ODdata++) = *(SDObuffer++);
      ENABLE_INTERRUPTS();
   }

   return 0;
}


/******************************************************************************/
void CO_SDO_abort(CO_SDO_t *SDO, UNSIGNED32 code){
   SDO->CANtxBuff->data[0] = 0x80;
   SDO->CANtxBuff->data[1] = SDO->ODF_arg.index & 0xFF;
   SDO->CANtxBuff->data[2] = (SDO->ODF_arg.index>>8) & 0xFF;
   SDO->CANtxBuff->data[3] = SDO->ODF_arg.subIndex;
   memcpySwap4(&SDO->CANtxBuff->data[4], (UNSIGNED8*)&code);
   SDO->state = STATE_IDLE;
   SDO->CANrxNew = 0;
   CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
}


/******************************************************************************/
INTEGER8 CO_SDO_process(CO_SDO_t         *SDO,
                        UNSIGNED8         NMTisPreOrOperational,
                        UNSIGNED16        timeDifference_ms,
                        UNSIGNED16        SDOtimeoutTime)
{
   UNSIGNED8 state = STATE_IDLE;
   UNSIGNED8 timeoutSubblockDownolad = 0;

   //return if idle
   if(SDO->state == STATE_IDLE && SDO->CANrxNew == 0){
      return 0;
   }

   //SDO is allowed to work only in operational or pre-operational NMT state
   if(!NMTisPreOrOperational){
      SDO->state = STATE_IDLE;
      SDO->CANrxNew = 0;
      return 0;
   }

   //Is something new to process?
   if(!SDO->CANtxBuff->bufferFull && (SDO->CANrxNew || SDO->state == STATE_UPLOAD_BLOCK_SUBBLOCK)){
      UNSIGNED8 CCS = SDO->CANrxData[0] >> 5;   //Client command specifier

      //reset timeout
      if(SDO->state != STATE_UPLOAD_BLOCK_SUBBLOCK)
         SDO->timeoutTimer = 0;

      //clear response buffer
      SDO->CANtxBuff->data[0] = SDO->CANtxBuff->data[1] = SDO->CANtxBuff->data[2] = SDO->CANtxBuff->data[3] = 0;
      SDO->CANtxBuff->data[4] = SDO->CANtxBuff->data[5] = SDO->CANtxBuff->data[6] = SDO->CANtxBuff->data[7] = 0;

      //Is abort from client?
      if(SDO->CANrxNew && SDO->CANrxData[0] == CCS_ABORT){
         SDO->state = STATE_IDLE;
         SDO->CANrxNew = 0;
         return -1;
      }

      //continue with previous SDO communication or start new
      if(SDO->state != STATE_IDLE){
         state = SDO->state;
      }
      else{
         UNSIGNED32 abortCode;

         //Is client command specifier valid
         if(CCS != CCS_DOWNLOAD_INITIATE && CCS != CCS_UPLOAD_INITIATE &&
            CCS != CCS_DOWNLOAD_BLOCK && CCS != CCS_UPLOAD_BLOCK){
            CO_SDO_abort(SDO, 0x05040001L);//Client command specifier not valid or unknown.
            return -1;
         }

         //init ODF_arg
         abortCode = CO_SDO_initTransfer(SDO, (UNSIGNED16)SDO->CANrxData[2]<<8 | SDO->CANrxData[1], SDO->CANrxData[3]);
         if(abortCode){
            CO_SDO_abort(SDO, abortCode);
            return -1;
         }

         //download
         if(CCS == CCS_DOWNLOAD_INITIATE || CCS == CCS_DOWNLOAD_BLOCK){
            if(!(SDO->ODF_arg.attribute & CO_ODA_WRITEABLE)){
               CO_SDO_abort(SDO, 0x06010002L); //attempt to write a read-only object
               return -1;
            }

            //set state machine to normal or block download
            if(CCS == CCS_DOWNLOAD_INITIATE)
               state = STATE_DOWNLOAD_INITIATE;
            else
               state = STATE_DOWNLOAD_BLOCK_INITIATE;
         }

         //upload
         else{
            abortCode = CO_SDO_readOD(SDO, CO_SDO_BUFFER_SIZE);
            if(abortCode){
               CO_SDO_abort(SDO, abortCode);
               return -1;
            }

            //if data size is large enough set state machine to block upload, otherwise set to normal transfer
            if(CCS == CCS_UPLOAD_BLOCK && SDO->ODF_arg.dataLength > SDO->CANrxData[5])
               state = STATE_UPLOAD_BLOCK_INITIATE;
            else
               state = STATE_UPLOAD_INITIATE;
         }
      }
   }

   //verify SDO timeout
   if(SDO->timeoutTimer < SDOtimeoutTime) SDO->timeoutTimer += timeDifference_ms;
   if(SDO->timeoutTimer >= SDOtimeoutTime){
      if(SDO->state == STATE_DOWNLOAD_BLOCK_SUBBLOCK && SDO->sequence && !SDO->CANtxBuff->bufferFull){
         timeoutSubblockDownolad = 1;
         state = SDO->state;
      }
      else{
         CO_SDO_abort(SDO, 0x05040000L); //SDO protocol timed out
         return -1;
      }
   }

   //return immediately if still idle
   if(state == STATE_IDLE) return 0;

   //state machine (buffer is freed (SDO->CANrxNew = 0;) at the end)
   switch(state){
      UNSIGNED32 abortCode;
      UNSIGNED16 len, i, err;

      case STATE_DOWNLOAD_INITIATE:{
         //default response
         SDO->CANtxBuff->data[0] = 0x60;
         SDO->CANtxBuff->data[1] = SDO->CANrxData[1];
         SDO->CANtxBuff->data[2] = SDO->CANrxData[2];
         SDO->CANtxBuff->data[3] = SDO->CANrxData[3];

         //Expedited transfer
         if(SDO->CANrxData[0] & 0x02){
            //is size indicated? Get message length
            if(SDO->CANrxData[0] & 0x01)
               len = 4 - ((SDO->CANrxData[0]>>2)&0x03);
            else
               len = 4;

            //copy data to SDO buffer
            SDO->ODF_arg.data[0] = SDO->CANrxData[4];
            SDO->ODF_arg.data[1] = SDO->CANrxData[5];
            SDO->ODF_arg.data[2] = SDO->CANrxData[6];
            SDO->ODF_arg.data[3] = SDO->CANrxData[7];

            //write data to the Object dictionary
            abortCode = CO_SDO_writeOD(SDO, len);
            if(abortCode){
               CO_SDO_abort(SDO, abortCode);
               return -1;
            }

            //finish the communication
            SDO->state = STATE_IDLE;
            CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
         }

         //Segmented transfer
         else{
            //verify length if size is indicated
            if(SDO->CANrxData[0]&0x01){
               UNSIGNED32 lenRx;
               memcpySwap4((UNSIGNED8*)&lenRx, &SDO->CANrxData[4]);
               SDO->ODF_arg.dataLengthTotal = lenRx;

               //verify length except for domain data type
               if(lenRx != SDO->ODF_arg.dataLength && SDO->ODF_arg.ODdataStorage != 0){
                  CO_SDO_abort(SDO, 0x06070010L);  //Length of service parameter does not match
                  return -1;
               }
            }
            SDO->bufferOffset = 0;
            SDO->sequence = 0;
            SDO->state = STATE_DOWNLOAD_SEGMENTED;
            CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
         }
         break;
      }

      case STATE_DOWNLOAD_SEGMENTED:{
         //verify client command specifier
         if((SDO->CANrxData[0]&0xE0) != 0x00){
            CO_SDO_abort(SDO, 0x05040001L);//Client command specifier not valid or unknown.
            return -1;
         }

         //verify toggle bit
         i = (SDO->CANrxData[0]&0x10) ? 1 : 0;
         if(i != SDO->sequence){
            CO_SDO_abort(SDO, 0x05030000L);//toggle bit not alternated
            return -1;
         }

         //get size of data in message
         len = 7 - ((SDO->CANrxData[0]>>1)&0x07);

         //verify length. Domain data type enables length larger than SDO buffer size
         if((SDO->bufferOffset + len) > SDO->ODF_arg.dataLength){
            if(SDO->ODF_arg.ODdataStorage != 0){
               CO_SDO_abort(SDO, 0x06070012L);  //Length of service parameter too high
               return -1;
            }
            else{
               //empty buffer in domain data type
               SDO->ODF_arg.lastSegment = 0;
               abortCode = CO_SDO_writeOD(SDO, SDO->bufferOffset);
               if(abortCode){
                  CO_SDO_abort(SDO, abortCode);
                  return -1;
               }

               SDO->ODF_arg.dataLength = CO_SDO_BUFFER_SIZE;
               SDO->bufferOffset = 0;
            }
         }

         //copy data to buffer
         for(i=0; i<len; i++)
            SDO->ODF_arg.data[SDO->bufferOffset++] = SDO->CANrxData[i+1];

         //If no more segments to be downloaded, write data to the Object dictionary
         if(SDO->CANrxData[0] & 0x01){
            SDO->ODF_arg.lastSegment = 1;
            abortCode = CO_SDO_writeOD(SDO, SDO->bufferOffset);
            if(abortCode){
               CO_SDO_abort(SDO, abortCode);
               return -1;
            }

            //finish
            SDO->state = STATE_IDLE;
         }

         //download segment response and alternate toggle bit
         SDO->CANtxBuff->data[0] = 0x20 | (SDO->sequence ? 0x10 : 0x00);
         SDO->sequence = (SDO->sequence) ? 0 : 1;
         CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
         break;
      }

      case STATE_DOWNLOAD_BLOCK_INITIATE:{
         //verify client command specifier and subcommand
         if((SDO->CANrxData[0]&0xE1) != 0xC0){
            CO_SDO_abort(SDO, 0x05040001L);//Client command specifier not valid or unknown.
            return -1;
         }

         //prepare response
         SDO->CANtxBuff->data[0] = 0xA4;
         SDO->CANtxBuff->data[1] = SDO->CANrxData[1];
         SDO->CANtxBuff->data[2] = SDO->CANrxData[2];
         SDO->CANtxBuff->data[3] = SDO->CANrxData[3];

         //blksize
         SDO->blksize = (CO_SDO_BUFFER_SIZE > (7*127)) ? 127 : (CO_SDO_BUFFER_SIZE / 7);
         SDO->CANtxBuff->data[4] = SDO->blksize;

         //is CRC enabled
         SDO->crcEnabled = (SDO->CANrxData[0] & 0x04) ? 1 : 0;
         SDO->crc = 0;

         //verify length if size is indicated
         if(SDO->CANrxData[0]&0x02){
            UNSIGNED32 lenRx;
            memcpySwap4((UNSIGNED8*)&lenRx, &SDO->CANrxData[4]);
            SDO->ODF_arg.dataLengthTotal = lenRx;

            //verify length except for domain data type
            if(lenRx != SDO->ODF_arg.dataLength && SDO->ODF_arg.ODdataStorage != 0){
               CO_SDO_abort(SDO, 0x06070010L);  //Length of service parameter does not match
               return -1;
            }
         }

         SDO->bufferOffset = 0;
         SDO->sequence = 0;
         SDO->state = STATE_DOWNLOAD_BLOCK_SUBBLOCK;

         //send response
         CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
         break;
      }

      case STATE_DOWNLOAD_BLOCK_SUBBLOCK:{
         err = 1;

         //no new message received, SDO timeout occured, try to response
         if(timeoutSubblockDownolad){
            SDO->CANrxData[0] = 0;
         }
         else{
            //if waiting for first segment and received wrong segment just ignore it
            if(SDO->sequence == 0 && (SDO->CANrxData[0] & 0x7f) != 1){
               break;
            }

            //previous segment is received again, ignore it
            else if((SDO->CANrxData[0] & 0x7f) == SDO->sequence){
               break;
            }

            //correct segment is received, copy data
            else if((SDO->CANrxData[0] & 0x7f) == (SDO->sequence+1)){
               SDO->sequence++;
               err = 0;
               for(i=0; i<7; i++)
                  SDO->ODF_arg.data[SDO->bufferOffset++] = SDO->CANrxData[i+1];
            }
         }

         //send response if: wrong segment or last segment or end of transfer or timeout
         if(err || SDO->sequence >= SDO->blksize || (SDO->CANrxData[0] & 0x80) || timeoutSubblockDownolad){
            //prepare response
            SDO->CANtxBuff->data[0] = 0xA2;
            SDO->CANtxBuff->data[1] = SDO->sequence;
            SDO->sequence = 0;

            //empty buffer in domain data type if not last segment
            if(SDO->ODF_arg.ODdataStorage == 0 && SDO->bufferOffset && !(SDO->CANrxData[0] & 0x80)){
               //calculate CRC on next bytes, if enabled
               if(SDO->crcEnabled)
                  SDO->crc = crc16_ccitt(SDO->ODF_arg.data, SDO->bufferOffset, SDO->crc);

               //write data to the Object dictionary
               SDO->ODF_arg.lastSegment = 0;
               abortCode = CO_SDO_writeOD(SDO, SDO->bufferOffset);
               if(abortCode){
                  CO_SDO_abort(SDO, abortCode);
                  return -1;
               }

               SDO->ODF_arg.dataLength = CO_SDO_BUFFER_SIZE;
               SDO->bufferOffset = 0;
            }

            //blksize
            len = CO_SDO_BUFFER_SIZE - SDO->bufferOffset;
            SDO->blksize = (len > (7*127)) ? 127 : (len / 7);
            SDO->CANtxBuff->data[2] = SDO->blksize;

            //last segment
            if((SDO->CANrxData[0] & 0x80) && !err){
               SDO->state = STATE_DOWNLOAD_BLOCK_END;
            }

            //send response
            CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
         }

         //don't clear SDO->CANrxNew flag on timeout
         if(timeoutSubblockDownolad) return 1;

         break;
      }

      case STATE_DOWNLOAD_BLOCK_END:{
         //verify client command specifier and subcommand
         if((SDO->CANrxData[0]&0xE1) != 0xC1){
            CO_SDO_abort(SDO, 0x05040001L);//Client command specifier not valid or unknown.
            return -1;
         }

         //number of bytes in the last segment of the last block that do not contain data.
         len = (SDO->CANrxData[0]>>2) & 0x07;
         SDO->bufferOffset -= len;

         //calculate and verify CRC, if enabled
         if(SDO->crcEnabled){
            UNSIGNED16 crc;
            SDO->crc = crc16_ccitt(SDO->ODF_arg.data, SDO->bufferOffset, SDO->crc);

            memcpySwap2((UNSIGNED8*)&crc, &SDO->CANrxData[1]);

            if(SDO->crc != crc){
               CO_SDO_abort(SDO, 0x05040004);   //CRC error (block mode only).
               return -1;
            }
         }

         //write data to the Object dictionary
         SDO->ODF_arg.lastSegment = 1;
         abortCode = CO_SDO_writeOD(SDO, SDO->bufferOffset);
         if(abortCode){
            CO_SDO_abort(SDO, abortCode);
            return -1;
         }

         //send response
         SDO->CANtxBuff->data[0] = 0xA1;
         CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
         SDO->state = STATE_IDLE;
         break;
      }

      case STATE_UPLOAD_INITIATE:{
         //default response
         SDO->CANtxBuff->data[1] = SDO->CANrxData[1];
         SDO->CANtxBuff->data[2] = SDO->CANrxData[2];
         SDO->CANtxBuff->data[3] = SDO->CANrxData[3];

         //Expedited transfer
         if(SDO->ODF_arg.dataLength <= 4){
            for(i=0; i<SDO->ODF_arg.dataLength; i++)
               SDO->CANtxBuff->data[4+i] = SDO->ODF_arg.data[i];

            SDO->CANtxBuff->data[0] = 0x43 | ((4-SDO->ODF_arg.dataLength) << 2);
            CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);

            //finish
            SDO->state = STATE_IDLE;
         }

         //Segmented transfer
         else{
            SDO->bufferOffset = 0;
            SDO->sequence = 0;
            SDO->state = STATE_UPLOAD_SEGMENTED;

            //indicate data size, if known
            if(SDO->ODF_arg.dataLengthTotal){
               UNSIGNED32 len = SDO->ODF_arg.dataLengthTotal;
               memcpySwap4(&SDO->CANtxBuff->data[4], (UNSIGNED8*)&len);
               SDO->CANtxBuff->data[0] = 0x41;
            }
            else{
               SDO->CANtxBuff->data[0] = 0x40;
            }

            //send response
            CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
         }
         break;
      }

      case STATE_UPLOAD_SEGMENTED:{
         //verify client command specifier
         if((SDO->CANrxData[0]&0xE0) != 0x60){
            CO_SDO_abort(SDO, 0x05040001L);//Client command specifier not valid or unknown.
            return -1;
         }

         //verify toggle bit
         i = (SDO->CANrxData[0]&0x10) ? 1 : 0;
         if(i != SDO->sequence){
            CO_SDO_abort(SDO, 0x05030000L);//toggle bit not alternated
            return -1;
         }

         //calculate length to be sent
         len = SDO->ODF_arg.dataLength - SDO->bufferOffset;
         if(len > 7) len = 7;

         //If data type is domain, re-fill the data buffer if neccessary and indicated so.
         if(SDO->ODF_arg.ODdataStorage == 0 && len < 7 && SDO->ODF_arg.lastSegment == 0){
            //copy previous data to the beginning
            for(i=0; i<len; i++)
               SDO->ODF_arg.data[i] = SDO->ODF_arg.data[SDO->bufferOffset+i];

            //move the beginning of the data buffer
            SDO->ODF_arg.data += len;
            SDO->ODF_arg.dataLength = CO_OD_getLength(SDO, SDO->entryNo, SDO->ODF_arg.subIndex) - len;

            //read next data from Object dictionary function
            abortCode = CO_SDO_readOD(SDO, CO_SDO_BUFFER_SIZE);
            if(abortCode){
               CO_SDO_abort(SDO, abortCode);
               return -1;
            }

            //return to the original data buffer
            SDO->ODF_arg.data -= len;
            SDO->ODF_arg.dataLength +=  len;
            SDO->bufferOffset = 0;

            //re-calculate the length
            len = SDO->ODF_arg.dataLength;
            if(len > 7) len = 7;
         }

         //fill response data bytes
         for(i=0; i<len; i++)
            SDO->CANtxBuff->data[i+1] = SDO->ODF_arg.data[SDO->bufferOffset++];

         //first response byte
         SDO->CANtxBuff->data[0] = 0x00 | (SDO->sequence ? 0x10 : 0x00) | ((7-len)<<1);
         SDO->sequence = (SDO->sequence) ? 0 : 1;

         //verify end of transfer
         if(SDO->bufferOffset==SDO->ODF_arg.dataLength && SDO->ODF_arg.lastSegment){
            SDO->CANtxBuff->data[0] |= 0x01;
            SDO->state = STATE_IDLE;
         }

         //send response
         CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
         break;
      }

      case STATE_UPLOAD_BLOCK_INITIATE:{
         //default response
         SDO->CANtxBuff->data[1] = SDO->CANrxData[1];
         SDO->CANtxBuff->data[2] = SDO->CANrxData[2];
         SDO->CANtxBuff->data[3] = SDO->CANrxData[3];

         //calculate CRC, if enabled
         if(SDO->CANrxData[0] & 0x04){
            SDO->crcEnabled = 1;
            SDO->crc = crc16_ccitt(SDO->ODF_arg.data, SDO->ODF_arg.dataLength, 0);
         }
         else{
            SDO->crcEnabled = 0;
            SDO->crc = 0;
         }

         //Number of segments per block
         SDO->blksize = SDO->CANrxData[4];

         //verify client subcommand and blksize
         if((SDO->CANrxData[0]&0x03) != 0x00 || SDO->blksize < 1 || SDO->blksize > 127){
            CO_SDO_abort(SDO, 0x05040001L);//Client command specifier not valid or unknown.
            return -1;
         }

         //verify if SDO data buffer is large enough
         if((SDO->blksize*7) > SDO->ODF_arg.dataLength && !SDO->ODF_arg.lastSegment){
            CO_SDO_abort(SDO, 0x05040002); //Invalid block size (block mode only).
            return -1;
         }

         //indicate data size, if known
         if(SDO->ODF_arg.dataLengthTotal){
            UNSIGNED32 len = SDO->ODF_arg.dataLengthTotal;
            memcpySwap4(&SDO->CANtxBuff->data[4], (UNSIGNED8*)&len);
            SDO->CANtxBuff->data[0] = 0xC6;
         }
         else{
            SDO->CANtxBuff->data[0] = 0xC4;
         }

         //send response
         SDO->state = STATE_UPLOAD_BLOCK_INITIATE_2;
         CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
         break;
      }

      case STATE_UPLOAD_BLOCK_INITIATE_2:{
         //verify client command specifier and subcommand
         if((SDO->CANrxData[0]&0xE3) != 0xA3){
            CO_SDO_abort(SDO, 0x05040001L);//Client command specifier not valid or unknown.
            return -1;
         }

         SDO->bufferOffset = 0;
         SDO->sequence = 0;
         SDO->endOfTransfer = 0;
         SDO->CANrxNew = 0;
         SDO->state = STATE_UPLOAD_BLOCK_SUBBLOCK;
         //continue in next case
      }

      case STATE_UPLOAD_BLOCK_SUBBLOCK:{
         //is block confirmation received
         if(SDO->CANrxNew){
            UNSIGNED8 ackseq;
            UNSIGNED16 j;

            //verify client command specifier and subcommand
            if((SDO->CANrxData[0]&0xE3) != 0xA2){
               CO_SDO_abort(SDO, 0x05040001L);//Client command specifier not valid or unknown.
               return -1;
            }

            ackseq = SDO->CANrxData[1];   //sequence number of the last segment, that was received correctly.

            //verify if response is too early
            if(ackseq > SDO->sequence){
               CO_SDO_abort(SDO, 0x05040002); //Invalid block size (block mode only).
               return -1;
            }

            //end of transfer
            if(SDO->endOfTransfer && ackseq == SDO->blksize){
               //first response byte
               SDO->CANtxBuff->data[0] = 0xC1 | ((7 - SDO->lastLen) << 2);

               //CRC
               if(SDO->crcEnabled)
                  memcpySwap2(&SDO->CANtxBuff->data[1], (UNSIGNED8*)&SDO->crc);

               SDO->state = STATE_UPLOAD_BLOCK_END;

               //send response
               CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);
               break;
            }

            //move remaining data to the beginning
            for(i=ackseq*7, j=0; i<SDO->ODF_arg.dataLength; i++, j++)
               SDO->ODF_arg.data[j] = SDO->ODF_arg.data[i];

            //set remaining data length in buffer
            SDO->ODF_arg.dataLength -= ackseq * 7;

            //new block size
            SDO->blksize = SDO->CANrxData[2];

            //If data type is domain, re-fill the data buffer if neccessary and indicated so.
            if(SDO->ODF_arg.ODdataStorage == 0 && SDO->ODF_arg.dataLength < (SDO->blksize*7) && SDO->ODF_arg.lastSegment == 0){
               //move the beginning of the data buffer
               len = SDO->ODF_arg.dataLength; //length of valid data in buffer
               SDO->ODF_arg.data += len;
               SDO->ODF_arg.dataLength = CO_OD_getLength(SDO, SDO->entryNo, SDO->ODF_arg.subIndex) - len;

               //read next data from Object dictionary function
               abortCode = CO_SDO_readOD(SDO, CO_SDO_BUFFER_SIZE);
               if(abortCode){
                  CO_SDO_abort(SDO, abortCode);
                  return -1;
               }

               //calculate CRC on next bytes, if enabled
               if(SDO->crcEnabled)
                  SDO->crc = crc16_ccitt(SDO->ODF_arg.data, SDO->ODF_arg.dataLength, SDO->crc);

              //return to the original data buffer
               SDO->ODF_arg.data -= len;
               SDO->ODF_arg.dataLength +=  len;
            }

            //verify if SDO data buffer is large enough
            if((SDO->blksize*7) > SDO->ODF_arg.dataLength && !SDO->ODF_arg.lastSegment){
               CO_SDO_abort(SDO, 0x05040002); //Invalid block size (block mode only).
               return -1;
            }

            SDO->bufferOffset = 0;
            SDO->sequence = 0;
            SDO->endOfTransfer = 0;

            //clear flag here
            SDO->CANrxNew = 0;
         }

         //return, if all segments was allready transfered or on end of transfer
         if(SDO->sequence == SDO->blksize || SDO->endOfTransfer)
            return 1;//don't clear the SDO->CANrxNew flag, so return directly

         //reset timeout
         SDO->timeoutTimer = 0;

         //calculate length to be sent
         len = SDO->ODF_arg.dataLength - SDO->bufferOffset;
         if(len > 7) len = 7;

         //fill response data bytes
         for(i=0; i<len; i++)
            SDO->CANtxBuff->data[i+1] = SDO->ODF_arg.data[SDO->bufferOffset++];

         //first response byte
         SDO->CANtxBuff->data[0] = ++SDO->sequence;

         //verify end of transfer
         if(SDO->bufferOffset==SDO->ODF_arg.dataLength && SDO->ODF_arg.lastSegment){
            SDO->CANtxBuff->data[0] |= 0x80;
            SDO->lastLen = len;
            SDO->blksize = SDO->sequence;
            SDO->endOfTransfer = 1;
         }

         //send response
         CO_CANsend(SDO->CANdevTx, SDO->CANtxBuff);

         //don't clear the SDO->CANrxNew flag, so return directly
         return 1;
      }

      case STATE_UPLOAD_BLOCK_END:{
         //verify client command specifier
         if((SDO->CANrxData[0]&0xE1) != 0xA1){
            CO_SDO_abort(SDO, 0x05040001L);//Client command specifier not valid or unknown.
            return -1;
         }

         SDO->state = STATE_IDLE;
         break;
      }

      default:{
         CO_SDO_abort(SDO, 0x06040047L);//general internal incompatibility in the device
         return -1;
      }
   }

   //free buffer
   SDO->CANrxNew = 0;

   if(SDO->state != STATE_IDLE) return 1;

   return 0;
}
