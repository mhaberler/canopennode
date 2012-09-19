/*******************************************************************************

   File - CO_Emergency.c
   CANopen Emergency object.

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

#include <stdlib.h> // for malloc, free


/******************************************************************************/
UNSIGNED32 CO_ODF_1003( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED16 *pLength,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData)
{
   CO_emergencyProcess_t   *EMpr;

   EMpr = (CO_emergencyProcess_t*) object; //this is the correct pointer type of the first argument

   if(subIndex==0) pData = (const void*) &EMpr->preDefinedErrorFieldNumberOfErrors;

   if(dir == 0){  //Reading Object Dictionary variable
      if(subIndex > EMpr->preDefinedErrorFieldNumberOfErrors) return 0x08000024L;  //No data available.
   }
   else{ //Writing Object Dictionary variable
      //only '0' may be written to subIndex 0
      if(subIndex==0){
         if(*((UNSIGNED8*)dataBuff)!=0)
            return 0x06090030L;  //Invalid value for parameter
      }
      else
         return 0x06010002L;  //Attempt to write a read only object.
   }

   return CO_ODF(object, index, subIndex, pLength, attribute, dir, dataBuff, pData);
}


/******************************************************************************/
UNSIGNED32 CO_ODF_1014( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED16 *pLength,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData)
{
   UNSIGNED8 *nodeId;
   UNSIGNED32 cobIdEmcy;
   UNSIGNED32 abortCode = 0;

   nodeId = (UNSIGNED8*) object; //this is the correct pointer type of the first argument

   cobIdEmcy = *((UNSIGNED32*) pData); //read from object dictionary
   cobIdEmcy += *nodeId;

   memcpySwap4((UNSIGNED8*)dataBuff, (UNSIGNED8*)&cobIdEmcy);

   return abortCode;
}


/******************************************************************************/
INTEGER16 CO_emergency_init(
      CO_emergencyReport_t  **ppEmergencyReport,
      CO_emergencyProcess_t **ppEmergencyProcess,
      CO_SDO_t               *SDO,
      UNSIGNED8              *errorStatusBits,
      UNSIGNED8               errorStatusBitsSize,
      UNSIGNED8              *errorRegister,
      UNSIGNED32             *preDefinedErrorField,
      UNSIGNED8               preDefinedErrorFieldSize,
      UNSIGNED8               msgBufferSize,
      CO_CANmodule_t *CANdev, UNSIGNED16 CANdevTxIdx, UNSIGNED16 CANidTxEM)
{
   UNSIGNED8 i;
   CO_emergencyProcess_t *EMpr;
   CO_emergencyReport_t  *EM;

   if(msgBufferSize < 2) msgBufferSize = 2;

   //allocate memory if not already allocated
   if((*ppEmergencyProcess) == NULL){
      if(((*ppEmergencyProcess)           = (CO_emergencyProcess_t*)malloc(sizeof(CO_emergencyProcess_t))) == NULL){                                                                                                      return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppEmergencyReport)            = (CO_emergencyReport_t*) malloc(sizeof(CO_emergencyReport_t)))  == NULL){                                                  free(*ppEmergencyProcess); *ppEmergencyProcess = 0; return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppEmergencyReport)->msgBuffer = (UNSIGNED8*)            malloc(msgBufferSize*8))               == NULL){free(*ppEmergencyReport); *ppEmergencyReport = 0; free(*ppEmergencyProcess); *ppEmergencyProcess = 0; return CO_ERROR_OUT_OF_MEMORY;}
   }
   else if((*ppEmergencyReport) == NULL || (*ppEmergencyReport)->msgBuffer == NULL) return CO_ERROR_ILLEGAL_ARGUMENT;

   EMpr = *ppEmergencyProcess; //pointer to (newly created) object
   EM = *ppEmergencyReport;
   EMpr->reportBuffer = EM;

   //Configure object variables
   EM->errorStatusBits                       = errorStatusBits;
   EM->errorStatusBitsSize                   = errorStatusBitsSize; if(errorStatusBitsSize < 6) return CO_ERROR_ILLEGAL_ARGUMENT;
   EM->msgBufferEnd                          = EM->msgBuffer + msgBufferSize*8;
   EM->msgBufferWritePtr                     = EM->msgBuffer;
   EM->msgBufferReadPtr                      = EM->msgBuffer;
   EM->msgBufferFull                         = 0;
   EM->wrongErrorReport                      = 0;
   EM->errorReportBusy                       = 0;
   EM->errorReportBusyError                  = 0;
   EMpr->errorRegister                       = errorRegister;
   EMpr->preDefinedErrorField                = preDefinedErrorField;
   EMpr->preDefinedErrorFieldSize            = preDefinedErrorFieldSize;
   EMpr->preDefinedErrorFieldNumberOfErrors  = 0;
   EMpr->inhibitEmergencyTimer               = 0;

   //clear error status bits
   for(i=0; i<errorStatusBitsSize; i++) EM->errorStatusBits[i] = 0;

   //Configure SDO server for first argument of CO_ODF_1003 and CO_ODF_1014
   CO_OD_configureArgumentForODF(SDO, 0x1003, (void*)EMpr);
   CO_OD_configureArgumentForODF(SDO, 0x1014, (void*)&SDO->nodeId);

   //configure emergency message CAN transmission
   EMpr->CANdev = CANdev;
   EMpr->CANdev->EM = (void*)EM; //update pointer inside CAN device.
   EMpr->CANtxBuff = CO_CANtxBufferInit(
                           CANdev,           //CAN device
                           CANdevTxIdx,      //index of specific buffer inside CAN module
                           CANidTxEM,        //CAN identifier
                           0,                //rtr
                           8,                //number of data bytes
                           0);               //synchronous message flag bit

   return CO_ERROR_NO;
}


/******************************************************************************/
void CO_emergency_delete(
      CO_emergencyReport_t  **ppEmergencyReport,
      CO_emergencyProcess_t **ppEmergencyProcess)
{
   if(*ppEmergencyReport){
      free((*ppEmergencyReport)->msgBuffer);
      free(*ppEmergencyReport);
      *ppEmergencyReport = 0;
   }
   if(*ppEmergencyProcess){
      free(*ppEmergencyProcess);
      *ppEmergencyProcess = 0;
   }
}


/******************************************************************************/
void CO_emergency_process( CO_emergencyProcess_t  *EMpr,
                           UNSIGNED8               NMTisPreOrOperational,
                           UNSIGNED16              timeDifference_100us,
                           UNSIGNED16              EMinhTime)
{

   CO_emergencyReport_t *EM = EMpr->reportBuffer;
   UNSIGNED8 errorRegister;

   //verify errors from driver and other
   CO_CANverifyErrors(EMpr->CANdev);
   if(EM->errorReportBusyError){
      CO_errorReport(EM, ERROR_ERROR_REPORT_BUSY, EM->errorReportBusyError);
      EM->errorReportBusyError = 0;
   }
   if(EM->wrongErrorReport){
      CO_errorReport(EM, ERROR_WRONG_ERROR_REPORT, EM->wrongErrorReport);
      EM->wrongErrorReport = 0;
   }


   //calculate Error register
   errorRegister = 0;
   //generic error
   if(EM->errorStatusBits[5])
      errorRegister |= 0x01;
   //communication error (overrun, error state)
   if(EM->errorStatusBits[2] || EM->errorStatusBits[3])
      errorRegister |= 0x10;
   *EMpr->errorRegister = (*EMpr->errorRegister & 0xEE) | errorRegister;

   //inhibit time
   if(EMpr->inhibitEmergencyTimer < EMinhTime) EMpr->inhibitEmergencyTimer += timeDifference_100us;

   //send Emergency message.
   if(   NMTisPreOrOperational &&
         !EMpr->CANtxBuff->bufferFull &&
         EMpr->inhibitEmergencyTimer >= EMinhTime &&
         (EM->msgBufferReadPtr != EM->msgBufferWritePtr || EM->msgBufferFull))
   {
      //copy data from emergency buffer into CAN buffer and preDefinedErrorField buffer
      UNSIGNED8* EMdataPtr = EM->msgBufferReadPtr;
      UNSIGNED8* CANtxData = EMpr->CANtxBuff->data;
      UNSIGNED32 preDEF;
      UNSIGNED8* ppreDEF = (UNSIGNED8*) &preDEF;
      *(CANtxData++) = *EMdataPtr; *(ppreDEF++) = *(EMdataPtr++);
      *(CANtxData++) = *EMdataPtr; *(ppreDEF++) = *(EMdataPtr++);
      *(CANtxData++) = *EMpr->errorRegister; *(ppreDEF++) = *EMpr->errorRegister; EMdataPtr++;
      *(CANtxData++) = *EMdataPtr; *(ppreDEF++) = *(EMdataPtr++);
      *(CANtxData++) = *(EMdataPtr++);
      *(CANtxData++) = *(EMdataPtr++);
      *(CANtxData++) = *(EMdataPtr++);
      *(CANtxData++) = *(EMdataPtr++);

      //Update read buffer pointer and reset inhibit timer
      if(EMdataPtr == EM->msgBufferEnd) EM->msgBufferReadPtr = EM->msgBuffer;
      else                              EM->msgBufferReadPtr = EMdataPtr;
      EMpr->inhibitEmergencyTimer = 0;

      //verify message buffer overflow, then clear full flag
      if(EM->msgBufferFull == 2){
         EM->msgBufferFull = 0;
         CO_errorReport(EM, ERROR_EMERGENCY_BUFFER_FULL, 0);
      }
      else EM->msgBufferFull = 0;

      //write to 'pre-defined error field' (object dictionary, index 0x1003)
      if(EMpr->preDefinedErrorField){
         UNSIGNED8 i;

         if(EMpr->preDefinedErrorFieldNumberOfErrors < EMpr->preDefinedErrorFieldSize)
            EMpr->preDefinedErrorFieldNumberOfErrors++;
         for(i=EMpr->preDefinedErrorFieldNumberOfErrors-1; i>0; i--)
            EMpr->preDefinedErrorField[i] = EMpr->preDefinedErrorField[i-1];
         EMpr->preDefinedErrorField[0] = preDEF;
      }

      CO_CANsend(EMpr->CANdev, EMpr->CANtxBuff);
   }

   return;
}


/******************************************************************************/
INTEGER8 CO_errorReport(CO_emergencyReport_t *EM, UNSIGNED8 errorBit, UNSIGNED16 errorCode, UNSIGNED32 infoCode){
   UNSIGNED8 index = errorBit >> 3;
   UNSIGNED8 bitmask = 1 << (errorBit & 0x7);
   UNSIGNED8 *errorStatusBits = &EM->errorStatusBits[index];
   UNSIGNED8 *msgBufferWritePtrCopy;

   //if error was allready reported, return
   if((*errorStatusBits & bitmask) != 0) return 0;

   if(!EM) return -1;

   //if errorBit value not supported, send emergency 'ERROR_WRONG_ERROR_REPORT'
   if(index >= EM->errorStatusBitsSize){
      EM->wrongErrorReport = errorBit;
      return -1;
   }

   //set error bit
   if(errorBit) *errorStatusBits |= bitmask; //any error except NO_ERROR

   //set busy flag. If allready busy, prepare for emergency and return.
   DISABLE_INTERRUPTS();
   if(EM->errorReportBusy++){
      EM->errorReportBusy--;
      ENABLE_INTERRUPTS();
      EM->errorReportBusyError = errorBit;
      return -3;
   }
   ENABLE_INTERRUPTS();

   //verify buffer full
   if(EM->msgBufferFull){
      EM->msgBufferFull = 2;
      EM->errorReportBusy--;
      return -2;
   }

   //copy data for emergency message
   msgBufferWritePtrCopy = EM->msgBufferWritePtr;
   memcpySwap2(msgBufferWritePtrCopy, (UNSIGNED8*)&errorCode);
   msgBufferWritePtrCopy += 3;   //third bit is Error register - written later
   *(msgBufferWritePtrCopy++) = errorBit;
   memcpySwap4(msgBufferWritePtrCopy, (UNSIGNED8*)&infoCode);
   msgBufferWritePtrCopy += 4;

   //Update write buffer pointer
   if(msgBufferWritePtrCopy == EM->msgBufferEnd) EM->msgBufferWritePtr = EM->msgBuffer;
   else                                          EM->msgBufferWritePtr = msgBufferWritePtrCopy;

   //verify buffer full, clear busy flag and return
   if(EM->msgBufferWritePtr == EM->msgBufferReadPtr) EM->msgBufferFull = 1;
   EM->errorReportBusy--;
   return 1;
}


/******************************************************************************/
INTEGER8 CO_errorReset(CO_emergencyReport_t *EM, UNSIGNED8 errorBit, UNSIGNED16 errorCode, UNSIGNED32 infoCode){
   UNSIGNED8 index = errorBit >> 3;
   UNSIGNED8 bitmask = 1 << (errorBit & 0x7);
   UNSIGNED8 *errorStatusBits = &EM->errorStatusBits[index];
   UNSIGNED8 *msgBufferWritePtrCopy;

   //if error is allready cleared, return
   if((*errorStatusBits & bitmask) == 0) return 0;

   if(!EM) return -1;

   //if errorBit value not supported, send emergency 'ERROR_WRONG_ERROR_REPORT'
   if(index >= EM->errorStatusBitsSize){
      EM->wrongErrorReport = errorBit;
      return -1;
   }

   //set busy flag. If allready busy, return.
   DISABLE_INTERRUPTS();
   if(EM->errorReportBusy++){
      EM->errorReportBusy--;
      ENABLE_INTERRUPTS();
      return -3;
   }
   ENABLE_INTERRUPTS();

   //erase error bit
   *errorStatusBits &= ~bitmask;

   //verify buffer full
   if(EM->msgBufferFull){
      EM->msgBufferFull = 2;
      EM->errorReportBusy--;
      return -2;
   }

   //copy data for emergency message
   msgBufferWritePtrCopy = EM->msgBufferWritePtr;
   *(msgBufferWritePtrCopy++) = 0;
   *(msgBufferWritePtrCopy++) = 0;
   *(msgBufferWritePtrCopy++) = 0;
   *(msgBufferWritePtrCopy++) = errorBit;
   memcpySwap4(msgBufferWritePtrCopy, (UNSIGNED8*)&infoCode);
   msgBufferWritePtrCopy += 4;

   //Update write buffer pointer
   if(msgBufferWritePtrCopy == EM->msgBufferEnd) EM->msgBufferWritePtr = EM->msgBuffer;
   else                                          EM->msgBufferWritePtr = msgBufferWritePtrCopy;

   //verify buffer full, clear busy flag and return
   if(EM->msgBufferWritePtr == EM->msgBufferReadPtr) EM->msgBufferFull = 1;
   EM->errorReportBusy--;
   return 1;
}


/******************************************************************************/
UNSIGNED8 CO_isError(CO_emergencyReport_t *EM, UNSIGNED8 errorBit, UNSIGNED16 dummy){
   UNSIGNED8 index = errorBit >> 3;
   UNSIGNED8 bitmask = 1 << (errorBit & 0x7);

   if((EM->errorStatusBits[index] & bitmask)) return 1;

   return 0;
}
