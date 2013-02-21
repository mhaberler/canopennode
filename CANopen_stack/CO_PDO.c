/*******************************************************************************

   File - CO_PDO.c
   CANopen Process Data Object.

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
#include "CO_PDO.h"

#include <stdlib.h> // for malloc, free

/******************************************************************************/
INTEGER16 CO_PDO_receive(void *object, CO_CANrxMsg_t *msg){
   CO_RPDO_t *RPDO;

   RPDO = (CO_RPDO_t*)object;   //this is the correct pointer type of the first argument

   if(RPDO->valid && *RPDO->operatingState == CO_NMT_OPERATIONAL){
      //verify message length
      if(RPDO->dataLength && msg->DLC < RPDO->dataLength) return CO_ERROR_RX_PDO_LENGTH;

      //verify message overflow (previous message was not processed yet)
      if(RPDO->CANrxNew) return CO_ERROR_RX_PDO_OVERFLOW;

      //copy data and set 'new message' flag
      RPDO->CANrxData[0] = msg->data[0];
      RPDO->CANrxData[1] = msg->data[1];
      RPDO->CANrxData[2] = msg->data[2];
      RPDO->CANrxData[3] = msg->data[3];
      RPDO->CANrxData[4] = msg->data[4];
      RPDO->CANrxData[5] = msg->data[5];
      RPDO->CANrxData[6] = msg->data[6];
      RPDO->CANrxData[7] = msg->data[7];

      RPDO->CANrxNew = 1;

      //verify message length
      if(RPDO->dataLength && msg->DLC > RPDO->dataLength) return CO_ERROR_RX_PDO_LENGTH;
   }
   else RPDO->CANrxNew = 0;

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_RPDOconfigCom(CO_RPDO_t* RPDO, UNSIGNED32 COB_IDUsedByRPDO){
   UNSIGNED16 ID;
   enum CO_ReturnError r;

   ID = (UNSIGNED16)COB_IDUsedByRPDO;

   //is RPDO used?
   if((COB_IDUsedByRPDO & 0xBFFFF800L) == 0 && RPDO->dataLength && ID){
      //is used default COB-ID?
      if(ID == RPDO->defaultCOB_ID) ID += RPDO->nodeId;
      RPDO->valid = 1;
   }
   else{
      ID = 0;
      RPDO->valid = 0;
      RPDO->CANrxNew = 0;
   }
   r = CO_CANrxBufferInit( RPDO->CANdevRx,         //CAN device
                           RPDO->CANdevRxIdx,      //rx buffer index
                           ID,                     //CAN identifier
                           0x7FF,                  //mask
                           0,                      //rtr
                           (void*)RPDO,            //object passed to receive function
                           CO_PDO_receive);        //this function will process received message
   if(r != CO_ERROR_NO){
      RPDO->valid = 0;
      RPDO->CANrxNew = 0;
   }
}


/******************************************************************************/
void CO_TPDOconfigCom(CO_TPDO_t* TPDO, UNSIGNED32 COB_IDUsedByTPDO, UNSIGNED8 syncFlag){
   UNSIGNED16 ID;

   ID = (UNSIGNED16)COB_IDUsedByTPDO;

   //is TPDO used?
   if((COB_IDUsedByTPDO & 0xBFFFF800L) == 0 && TPDO->dataLength && ID){
      //is used default COB-ID?
      if(ID == TPDO->defaultCOB_ID) ID += TPDO->nodeId;
      TPDO->valid = 1;
   }
   else{
      ID = 0;
      TPDO->valid = 0;
   }

   TPDO->CANtxBuff = CO_CANtxBufferInit(
                           TPDO->CANdevTx,            //CAN device
                           TPDO->CANdevTxIdx,         //index of specific buffer inside CAN module
                           ID,                        //CAN identifier
                           0,                         //rtr
                           TPDO->dataLength,          //number of data bytes
                           syncFlag);                 //synchronous message flag bit

   if(TPDO->CANtxBuff == 0){
      TPDO->valid = 0;
   }
}


/******************************************************************************/
UNSIGNED32 CO_PDOfindMap(  CO_SDO_t      *SDO,
                           UNSIGNED32     map,
                           UNSIGNED8      R_T,
                           UNSIGNED8    **ppData,
                           UNSIGNED8     *pLength,
                           UNSIGNED8     *pSendIfCOSFlags,
                           UNSIGNED8     *pIsMultibyteVar){
   UNSIGNED16 entryNo;
   UNSIGNED16 index;
   UNSIGNED8 subIndex;
   UNSIGNED8 dataLen;
   UNSIGNED8 objectLen;
   UNSIGNED8 attr;

   index = (UNSIGNED16)(map>>16);
   subIndex = (UNSIGNED8)(map>>8);
   dataLen = (UNSIGNED8) map;   //data length in bits

   //data length must be byte aligned
   if(dataLen&0x07) return 0x06040041L;   //Object cannot be mapped to the PDO.

   dataLen >>= 3;    //new data length is in bytes
   *pLength += dataLen;

   //total PDO length can not be more than 8 bytes
   if(*pLength > 8) return 0x06040042L;  //The number and length of the objects to be mapped would exceed PDO length.

   //is there a reference to dummy entries
   if(index <=7 && subIndex == 0){
      static UNSIGNED32 dummyTX = 0;
      static UNSIGNED32 dummyRX;
      UNSIGNED8 dummySize = 4;

      if(index<2) dummySize = 0;
      else if(index==2 || index==5) dummySize = 1;
      else if(index==3 || index==6) dummySize = 2;

      //is size of variable big enough for map
      if(dummySize < dataLen) return 0x06040041L;   //Object cannot be mapped to the PDO.

      //Data and ODE pointer
      if(R_T == 0) *ppData = (UNSIGNED8*) &dummyRX;
      else         *ppData = (UNSIGNED8*) &dummyTX;

      return 0;
   }

   //find object in Object Dictionary
   entryNo = CO_OD_find(SDO, index);

   //Does object exist in OD?
   if(entryNo == 0xFFFF || subIndex > SDO->OD[entryNo].maxSubIndex)
      return 0x06020000L;   //Object does not exist in the object dictionary.

   attr = CO_OD_getAttribute(SDO, entryNo, subIndex);
   //Is object Mappable for RPDO?
   if(R_T==0 && !(attr&CO_ODA_RPDO_MAPABLE && attr&CO_ODA_WRITEABLE)) return 0x06040041L;   //Object cannot be mapped to the PDO.
   //Is object Mappable for TPDO?
   if(R_T!=0 && !(attr&CO_ODA_TPDO_MAPABLE && attr&CO_ODA_READABLE)) return 0x06040041L;   //Object cannot be mapped to the PDO.

   //is size of variable big enough for map
   objectLen = CO_OD_getLength(SDO, entryNo, subIndex);
   if(objectLen < dataLen) return 0x06040041L;   //Object cannot be mapped to the PDO.

   //mark multibyte variable
   *pIsMultibyteVar = (attr&CO_ODA_MB_VALUE) ? 1 : 0;

   //pointer to data
   *ppData = (UNSIGNED8*) CO_OD_getDataPointer(SDO, entryNo, subIndex);
#ifdef BIG_ENDIAN
   //skip unused MSB bytes
   if(*pIsMultibyteVar){
      *ppData += objectLen - dataLen;
   }
#endif

   //setup change of state flags
   if(attr&CO_ODA_TPDO_DETECT_COS){
      INTEGER16 i;
      for(i=*pLength-dataLen; i<*pLength; i++){
         *pSendIfCOSFlags |= 1<<i;
      }
   }

   return 0;
}


/******************************************************************************/
UNSIGNED32 CO_RPDOconfigMap(CO_RPDO_t* RPDO, UNSIGNED8 noOfMappedObjects){
   INTEGER16 i;
   UNSIGNED8 length = 0;
   UNSIGNED32 ret = 0;
   const UNSIGNED32* pMap = &RPDO->RPDOMapPar->mappedObject1;

   for(i=noOfMappedObjects; i>0; i--){
      INTEGER16 j;
      UNSIGNED8* pData;
      UNSIGNED8 dummy = 0;
      UNSIGNED8 prevLength = length;
      UNSIGNED8 MBvar;
      UNSIGNED32 map = *(pMap++);

      //function do much checking of errors in map
      ret = CO_PDOfindMap( RPDO->SDO,
                           map,
                           0,
                           &pData,
                           &length,
                           &dummy,
                           &MBvar);
      if(ret){
         length = 0;
         CO_errorReport(RPDO->EM, ERROR_PDO_WRONG_MAPPING, map);
         break;
      }

      //write PDO data pointers
#ifdef BIG_ENDIAN
      if(MBvar){
         for(j=length-1; j>=prevLength; j--)
            RPDO->mapPointer[j] = pData++;
      }
      else{
         for(j=prevLength; j<length; j++)
            RPDO->mapPointer[j] = pData++;
      }
#else
      for(j=prevLength; j<length; j++){
         RPDO->mapPointer[j] = pData++;
      }
#endif

   }

   RPDO->dataLength = length;

   return ret;
}


/******************************************************************************/
UNSIGNED32 CO_TPDOconfigMap(CO_TPDO_t* TPDO, UNSIGNED8 noOfMappedObjects){
   INTEGER16 i;
   UNSIGNED8 length = 0;
   UNSIGNED32 ret = 0;
   const UNSIGNED32* pMap = &TPDO->TPDOMapPar->mappedObject1;

   TPDO->sendIfCOSFlags = 0;

   for(i=noOfMappedObjects; i>0; i--){
      INTEGER16 j;
      UNSIGNED8* pData;
      UNSIGNED8 prevLength = length;
      UNSIGNED8 MBvar;
      UNSIGNED32 map = *(pMap++);

      //function do much checking of errors in map
      ret = CO_PDOfindMap( TPDO->SDO,
                           map,
                           1,
                           &pData,
                           &length,
                           &TPDO->sendIfCOSFlags,
                           &MBvar);
      if(ret){
         length = 0;
         CO_errorReport(TPDO->EM, ERROR_PDO_WRONG_MAPPING, map);
         break;
      }

      //write PDO data pointers
#ifdef BIG_ENDIAN
      if(MBvar){
         for(j=length-1; j>=prevLength; j--)
            TPDO->mapPointer[j] = pData++;
      }
      else{
         for(j=prevLength; j<length; j++)
            TPDO->mapPointer[j] = pData++;
      }
#else
      for(j=prevLength; j<length; j++){
         TPDO->mapPointer[j] = pData++;
      }
#endif

   }

   TPDO->dataLength = length;

   return ret;
}


/* RPDO communication parameter - Object dictionary function ******************/
UNSIGNED32 CO_ODF_RPDOcom(CO_ODF_arg_t *ODF_arg){
   CO_RPDO_t *RPDO;

   RPDO = (CO_RPDO_t*) ODF_arg->object;

   //Reading Object Dictionary variable
   if(ODF_arg->reading){
      if(ODF_arg->subIndex == 1){
         UNSIGNED32 *value = (UNSIGNED32*) ODF_arg->data;

         //if default COB ID is used, write default value here
         if(((*value)&0xFFFF) == RPDO->defaultCOB_ID && RPDO->defaultCOB_ID)
            *value += RPDO->nodeId;

         //If PDO is not valid, set bit 31
         if(!RPDO->valid) *value |= 0x80000000L;
      }
      return 0;
   }

   //Writing Object Dictionary variable
   if(RPDO->restrictionFlags & 0x04)
      return 0x06010002L;  //Attempt to write a read only object.
   if(*RPDO->operatingState == CO_NMT_OPERATIONAL && (RPDO->restrictionFlags & 0x01))
      return 0x08000022L;   //Data cannot be transferred or stored to the application because of the present device state.

   if(ODF_arg->subIndex == 1){   //COB_ID
      UNSIGNED32 *value = (UNSIGNED32*) ODF_arg->data;

      //bits 11...29 must be zero
      if(*value & 0x3FFF8000L)
         return 0x06090030L;  //Invalid value for parameter (download only).

      //if default COB-ID is being written, write defaultCOB_ID without nodeId
      if(((*value)&0xFFFF) == (RPDO->defaultCOB_ID + RPDO->nodeId)){
         *value &= 0xC0000000L;
         *value += RPDO->defaultCOB_ID;
      }

      //if PDO is valid, bits 0..29 can not be changed
      if(RPDO->valid && ((*value ^ RPDO->RPDOCommPar->COB_IDUsedByRPDO) & 0x3FFFFFFFL))
         return 0x06090030L;  //Invalid value for parameter (download only).

      //configure RPDO
      CO_RPDOconfigCom(RPDO, *value);
   }
   else if(ODF_arg->subIndex == 2){   //Transmission_type
      UNSIGNED8 *value = (UNSIGNED8*) ODF_arg->data;

      //values from 241...253 are not valid
      if(*value >= 241 && *value <= 253)
         return 0x06090030L;  //Invalid value for parameter (download only).
   }

   return 0;
}


/* TPDO communication parameter - Object dictionary function ******************/
UNSIGNED32 CO_ODF_TPDOcom(CO_ODF_arg_t *ODF_arg){
   CO_TPDO_t *TPDO;

   TPDO = (CO_TPDO_t*) ODF_arg->object;

   if(ODF_arg->subIndex == 4) return 0x06090011L;  //Sub-index does not exist.

   //Reading Object Dictionary variable
   if(ODF_arg->reading){
      if(ODF_arg->subIndex == 1){   //COB_ID
         UNSIGNED32 *value = (UNSIGNED32*) ODF_arg->data;

         //if default COB ID is used, write default value here
         if(((*value)&0xFFFF) == TPDO->defaultCOB_ID && TPDO->defaultCOB_ID)
            *value += TPDO->nodeId;

         //If PDO is not valid, set bit 31
         if(!TPDO->valid) *value |= 0x80000000L;
      }
      return 0;
   }

   //Writing Object Dictionary variable
   if(TPDO->restrictionFlags & 0x04)
      return 0x06010002L;  //Attempt to write a read only object.
   if(*TPDO->operatingState == CO_NMT_OPERATIONAL && (TPDO->restrictionFlags & 0x01))
      return 0x08000022L;   //Data cannot be transferred or stored to the application because of the present device state.

   if(ODF_arg->subIndex == 1){   //COB_ID
      UNSIGNED32 *value = (UNSIGNED32*) ODF_arg->data;

      //bits 11...29 must be zero
      if(*value & 0x3FFF8000L)
         return 0x06090030L;  //Invalid value for parameter (download only).

      //if default COB-ID is being written, write defaultCOB_ID without nodeId
      if(((*value)&0xFFFF) == (TPDO->defaultCOB_ID + TPDO->nodeId)){
         *value &= 0xC0000000L;
         *value += TPDO->defaultCOB_ID;
      }

      //if PDO is valid, bits 0..29 can not be changed
      if(TPDO->valid && ((*value ^ TPDO->TPDOCommPar->COB_IDUsedByTPDO) & 0x3FFFFFFFL))
         return 0x06090030L;  //Invalid value for parameter (download only).

      //configure TPDO
      CO_TPDOconfigCom(TPDO, *value, TPDO->CANtxBuff->syncFlag);
      TPDO->syncCounter = 255;
   }
   else if(ODF_arg->subIndex == 2){   //Transmission_type
      UNSIGNED8 *value = (UNSIGNED8*) ODF_arg->data;

      //values from 241...253 are not valid
      if(*value >= 241 && *value <= 253)
         return 0x06090030L;  //Invalid value for parameter (download only).
      TPDO->CANtxBuff->syncFlag = (*value <= 240) ? 1 : 0;
      TPDO->syncCounter = 255;
   }
   else if(ODF_arg->subIndex == 3){   //Inhibit_Time
      //if PDO is valid, value can not be changed
      if(TPDO->valid)
         return 0x06090030L;  //Invalid value for parameter (download only).

      TPDO->inhibitTimer = 0;
   }
   else if(ODF_arg->subIndex == 5){   //Event_Timer
      UNSIGNED16 *value = (UNSIGNED16*) ODF_arg->data;

      TPDO->eventTimer = *value;
   }
   else if(ODF_arg->subIndex == 6){   //SYNC start value
      UNSIGNED8 *value = (UNSIGNED8*) ODF_arg->data;

      //if PDO is valid, value can not be changed
      if(TPDO->valid)
         return 0x06090030L;  //Invalid value for parameter (download only).

      //values from 240...255 are not valid
      if(*value > 240)
         return 0x06090030L;  //Invalid value for parameter (download only).
   }

   return 0;
}


/* RPDO mapping parameter - Object dictionary function ************************/
UNSIGNED32 CO_ODF_RPDOmap(CO_ODF_arg_t *ODF_arg){
   CO_RPDO_t *RPDO;

   RPDO = (CO_RPDO_t*) ODF_arg->object;

   //Reading Object Dictionary variable
   if(ODF_arg->reading){
      UNSIGNED8 *value = (UNSIGNED8*) ODF_arg->data;

      if(ODF_arg->subIndex == 0){
         //If there is error in mapping, dataLength is 0, so numberOfMappedObjects is 0.
         if(!RPDO->dataLength) *value = 0;
      }
      return 0;
   }

   //Writing Object Dictionary variable
   if(RPDO->restrictionFlags & 0x08)
      return 0x06010002L;  //Attempt to write a read only object.
   if(*RPDO->operatingState == CO_NMT_OPERATIONAL && (RPDO->restrictionFlags & 0x02))
      return 0x08000022L;   //Data cannot be transferred or stored to the application because of the present device state.
   if(RPDO->valid)
      return 0x06090030L;  //Invalid value for parameter (download only).

   //numberOfMappedObjects
   if(ODF_arg->subIndex == 0){
      UNSIGNED8 *value = (UNSIGNED8*) ODF_arg->data;

      if(*value > 8)
         return 0x06090031L;  //Value of parameter written too high.

      //configure mapping
      return CO_RPDOconfigMap(RPDO, *value);
   }

   //mappedObject
   else{
      UNSIGNED32 *value = (UNSIGNED32*) ODF_arg->data;
      UNSIGNED8* pData;
      UNSIGNED8 length = 0;
      UNSIGNED8 dummy = 0;
      UNSIGNED8 MBvar;

      if(RPDO->dataLength)
         return 0x06090030L;  //Invalid value for parameter (download only).

      //verify if mapping is correct
      return CO_PDOfindMap(RPDO->SDO,
                           *value,
                           0,
                           &pData,
                           &length,
                           &dummy,
                           &MBvar);
   }

   return 0;
}


/* TPDO mapping parameter - Object dictionary function ************************/
UNSIGNED32 CO_ODF_TPDOmap(CO_ODF_arg_t *ODF_arg){
   CO_TPDO_t *TPDO;

   TPDO = (CO_TPDO_t*) ODF_arg->object;

   //Reading Object Dictionary variable
   if(ODF_arg->reading){
      UNSIGNED8 *value = (UNSIGNED8*) ODF_arg->data;

      if(ODF_arg->subIndex == 0){
         //If there is error in mapping, dataLength is 0, so numberOfMappedObjects is 0.
         if(!TPDO->dataLength) *value = 0;
      }
      return 0;
   }

   //Writing Object Dictionary variable
   if(TPDO->restrictionFlags & 0x08)
      return 0x06010002L;  //Attempt to write a read only object.
   if(*TPDO->operatingState == CO_NMT_OPERATIONAL && (TPDO->restrictionFlags & 0x02))
      return 0x08000022L;   //Data cannot be transferred or stored to the application because of the present device state.
   if(TPDO->valid)
      return 0x06090030L;  //Invalid value for parameter (download only).

   //numberOfMappedObjects
   if(ODF_arg->subIndex == 0){
      UNSIGNED8 *value = (UNSIGNED8*) ODF_arg->data;

      if(*value > 8)
         return 0x06090031L;  //Value of parameter written too high.

      //configure mapping
      return CO_TPDOconfigMap(TPDO, *value);
   }

   //mappedObject
   else{
      UNSIGNED32 *value = (UNSIGNED32*) ODF_arg->data;
      UNSIGNED8* pData;
      UNSIGNED8 length = 0;
      UNSIGNED8 dummy = 0;
      UNSIGNED8 MBvar;

      if(TPDO->dataLength)
         return 0x06090030L;  //Invalid value for parameter (download only).

      //verify if mapping is correct
      return CO_PDOfindMap(TPDO->SDO,
                           *value,
                           1,
                           &pData,
                           &length,
                           &dummy,
                           &MBvar);
   }

   return 0;
}


/******************************************************************************/
INTEGER16 CO_RPDO_init(
      CO_RPDO_t                      **ppRPDO,
      CO_emergencyReport_t            *EM,
      CO_SDO_t                        *SDO,
      UNSIGNED8                       *operatingState,
      UNSIGNED8                        nodeId,
      UNSIGNED16                       defaultCOB_ID,
      UNSIGNED8                        restrictionFlags,
      const CO_RPDOCommPar_t          *RPDOCommPar,
      const CO_RPDOMapPar_t           *RPDOMapPar,
      UNSIGNED16                       ObjDictIndex_RPDOCommPar,
      UNSIGNED16                       ObjDictIndex_RPDOMapPar,
      CO_CANmodule_t *CANdevRx, UNSIGNED16 CANdevRxIdx)
{
   CO_RPDO_t *RPDO;

   //allocate memory if not already allocated
   if((*ppRPDO) == NULL){
      if(((*ppRPDO) = (CO_RPDO_t*) malloc(sizeof(CO_RPDO_t))) == NULL){ return CO_ERROR_OUT_OF_MEMORY;}
   }

   RPDO = *ppRPDO; //pointer to (newly created) object

   //Configure object variables
   RPDO->EM = EM;
   RPDO->SDO = SDO;
   RPDO->RPDOCommPar = RPDOCommPar;
   RPDO->RPDOMapPar = RPDOMapPar;
   RPDO->operatingState = operatingState;
   RPDO->nodeId = nodeId;
   RPDO->defaultCOB_ID = defaultCOB_ID;
   RPDO->restrictionFlags = restrictionFlags;

   //Configure Object dictionary entry at index 0x1400+ and 0x1600+
   CO_OD_configure(SDO, ObjDictIndex_RPDOCommPar, CO_ODF_RPDOcom, (void*)RPDO);
   CO_OD_configure(SDO, ObjDictIndex_RPDOMapPar, CO_ODF_RPDOmap, (void*)RPDO);

   //configure communication and mapping
   RPDO->CANrxNew = 0;
   RPDO->CANdevRx = CANdevRx;
   RPDO->CANdevRxIdx = CANdevRxIdx;

   CO_RPDOconfigMap(RPDO, RPDOMapPar->numberOfMappedObjects);
   CO_RPDOconfigCom(RPDO, RPDOCommPar->COB_IDUsedByRPDO);

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_RPDO_delete(CO_RPDO_t **ppRPDO){
   if(*ppRPDO){
      free(*ppRPDO);
      *ppRPDO = 0;
   }
}


/******************************************************************************/
INTEGER16 CO_TPDO_init(
      CO_TPDO_t                      **ppTPDO,
      CO_emergencyReport_t            *EM,
      CO_SDO_t                        *SDO,
      UNSIGNED8                       *operatingState,
      UNSIGNED8                        nodeId,
      UNSIGNED16                       defaultCOB_ID,
      UNSIGNED8                        restrictionFlags,
      const CO_TPDOCommPar_t          *TPDOCommPar,
      const CO_TPDOMapPar_t           *TPDOMapPar,
      UNSIGNED16                       ObjDictIndex_TPDOCommPar,
      UNSIGNED16                       ObjDictIndex_TPDOMapPar,
      CO_CANmodule_t *CANdevTx, UNSIGNED16 CANdevTxIdx)
{
   CO_TPDO_t *TPDO;

   //allocate memory if not already allocated
   if((*ppTPDO) == NULL){
      if(((*ppTPDO) = (CO_TPDO_t*) malloc(sizeof(CO_TPDO_t))) == NULL){return CO_ERROR_OUT_OF_MEMORY;}
   }

   TPDO = *ppTPDO; //pointer to (newly created) object

   //Configure object variables
   TPDO->EM = EM;
   TPDO->SDO = SDO;
   TPDO->TPDOCommPar = TPDOCommPar;
   TPDO->TPDOMapPar = TPDOMapPar;
   TPDO->operatingState = operatingState;
   TPDO->nodeId = nodeId;
   TPDO->defaultCOB_ID = defaultCOB_ID;
   TPDO->restrictionFlags = restrictionFlags;

   //Configure Object dictionary entry at index 0x1800+ and 0x1A00+
   CO_OD_configure(SDO, ObjDictIndex_TPDOCommPar, CO_ODF_TPDOcom, (void*)TPDO);
   CO_OD_configure(SDO, ObjDictIndex_TPDOMapPar, CO_ODF_TPDOmap, (void*)TPDO);

   //configure communication and mapping
   TPDO->CANdevTx = CANdevTx;
   TPDO->CANdevTxIdx = CANdevTxIdx;
   TPDO->syncCounter = 255;
   TPDO->inhibitTimer = 0;
   TPDO->eventTimer = TPDOCommPar->eventTimer;
   TPDO->SYNCtimerPrevious = 0;
   if(TPDOCommPar->transmissionType>=254) TPDO->sendRequest = 1;

   CO_TPDOconfigMap(TPDO, TPDOMapPar->numberOfMappedObjects);
   CO_TPDOconfigCom(TPDO, TPDOCommPar->COB_IDUsedByTPDO, ((TPDOCommPar->transmissionType<=240) ? 1 : 0));

   if((TPDOCommPar->transmissionType>240 &&
       TPDOCommPar->transmissionType<254) ||
       TPDOCommPar->SYNCStartValue>240){
         TPDO->valid = 0;
   }

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_TPDO_delete(CO_TPDO_t **ppTPDO){
   if(*ppTPDO){
      free(*ppTPDO);
      *ppTPDO = 0;
   }
}


/******************************************************************************/
UNSIGNED8 CO_TPDOisCOS(CO_TPDO_t *TPDO){

   //Prepare TPDO data automatically from Object Dictionary variables
   UNSIGNED8* pPDOdataByte;
   UNSIGNED8** ppODdataByte;

   pPDOdataByte = &TPDO->CANtxBuff->data[TPDO->dataLength];
   ppODdataByte = &TPDO->mapPointer[TPDO->dataLength];

   switch(TPDO->dataLength){
      case 8: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x80)) return 1;
      case 7: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x40)) return 1;
      case 6: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x20)) return 1;
      case 5: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x10)) return 1;
      case 4: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x08)) return 1;
      case 3: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x04)) return 1;
      case 2: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x02)) return 1;
      case 1: if(*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags&0x01)) return 1;
   }

   return 0;
}


/******************************************************************************/
INTEGER16 CO_TPDOsend(CO_TPDO_t *TPDO){
   INTEGER16 i;
   UNSIGNED8* pPDOdataByte;
   UNSIGNED8** ppODdataByte;

   pPDOdataByte = &TPDO->CANtxBuff->data[0];
   ppODdataByte = &TPDO->mapPointer[0];

   for(i=TPDO->dataLength; i>0; i--)
      *(pPDOdataByte++) = **(ppODdataByte++);

   TPDO->sendRequest = 0;

   return CO_CANsend(TPDO->CANdevTx, TPDO->CANtxBuff);
}


/******************************************************************************/
void CO_RPDO_process(CO_RPDO_t *RPDO){

   if(RPDO->CANrxNew && RPDO->valid && *RPDO->operatingState == CO_NMT_OPERATIONAL){
      INTEGER16 i;
      UNSIGNED8* pPDOdataByte;
      UNSIGNED8** ppODdataByte;

      pPDOdataByte = &RPDO->CANrxData[0];
      ppODdataByte = &RPDO->mapPointer[0];
      for(i=RPDO->dataLength; i>0; i--)
         **(ppODdataByte++) = *(pPDOdataByte++);
   }

   RPDO->CANrxNew = 0;
}


/******************************************************************************/
void CO_TPDO_process(   CO_TPDO_t     *TPDO,
                        CO_SYNC_t     *SYNC,
                        UNSIGNED16     timeDifference_100us,
                        UNSIGNED16     timeDifference_ms)
{
   INTEGER32 i;

   if(TPDO->valid && *TPDO->operatingState == CO_NMT_OPERATIONAL){

      //Send PDO by application request or by Event timer
      if(TPDO->TPDOCommPar->transmissionType >= 253){
         if(TPDO->inhibitTimer == 0 && (TPDO->sendRequest || (TPDO->TPDOCommPar->eventTimer && TPDO->eventTimer == 0))){
            if(CO_TPDOsend(TPDO) == CO_ERROR_NO){
               //successfully sent
               TPDO->inhibitTimer = TPDO->TPDOCommPar->inhibitTime;
               TPDO->eventTimer = TPDO->TPDOCommPar->eventTimer;
            }
         }
      }

      //Synchronous PDOs
      else if(SYNC && SYNC->running && SYNC->curentSyncTimeIsInsideWindow){
         //detect SYNC message
         if(SYNC->timer < TPDO->SYNCtimerPrevious){
            //send synchronous acyclic PDO
            if(TPDO->TPDOCommPar->transmissionType == 0){
               if(TPDO->sendRequest) CO_TPDOsend(TPDO);
            }
            //send synchronous cyclic PDO
            else{
               //is the start of synchronous TPDO transmission
               if(TPDO->syncCounter == 255){
                  if(SYNC->counterOverflowValue && TPDO->TPDOCommPar->SYNCStartValue)
                     TPDO->syncCounter = 254;   //SYNCStartValue is in use
                  else
                     TPDO->syncCounter = TPDO->TPDOCommPar->transmissionType;
               }
               //if the SYNCStartValue is in use, start first TPDO after SYNC with matched SYNCStartValue.
               if(TPDO->syncCounter == 254){
                  if(SYNC->counter == TPDO->TPDOCommPar->SYNCStartValue){
                     TPDO->syncCounter = TPDO->TPDOCommPar->transmissionType;
                     CO_TPDOsend(TPDO);
                  }
               }
               //Send PDO after every N-th Sync
               else if(--TPDO->syncCounter == 0){
                  TPDO->syncCounter = TPDO->TPDOCommPar->transmissionType;
                  CO_TPDOsend(TPDO);
               }
            }
         }
         TPDO->SYNCtimerPrevious = SYNC->timer;
      }

   }
   else{
      //Not operational or valid. Force TPDO first send after operational or valid.
      if(TPDO->TPDOCommPar->transmissionType>=254) TPDO->sendRequest = 1;
      else                                         TPDO->sendRequest = 0;
   }

   //update timers
   i = TPDO->inhibitTimer;
   i -= timeDifference_100us;
   TPDO->inhibitTimer = (i<=0) ? 0 : (UNSIGNED16)i;

   i = TPDO->eventTimer;
   i -= timeDifference_ms;
   TPDO->eventTimer = (i<=0) ? 0 : (UNSIGNED16)i;
}
