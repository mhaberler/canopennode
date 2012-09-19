/*******************************************************************************

   File: CO_SYNC.h
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

#ifndef _CO_SYNC_H
#define _CO_SYNC_H


/*******************************************************************************
   Topic: SYNC message

   Description and contents of SYNC message.

   For CAN identifier see <Default COB identifiers>.

   SYNC message is used for synchronization of the nodes on network. There is
   one SYNC producer and zero or more SYNC consumers.

   Contents of SYNC message:
      By default SYNC message has no data. If _Synchronous counter overflow value_
      from Object dictionary (index 0x1019) is different than 0, SYNC message has
      one data byte: counter incremented by 1 with every SYNC transmission.
*******************************************************************************/


/*******************************************************************************
   Object: CO_SYNC_t

   Object controls SYNC producer and consumer.

   Variables:
      EM                   - Pointer to Emergency object <CO_emergencyReport_t>.
      operatingState       - Pointer to variable indicating CANopen device NMT internal state.

      isProducer           - True, if device is SYNC producer. Calculated from _COB ID SYNC Message_
                             variable from Object dictionary (index 0x1005).
      COB_ID               - COB_ID of SYNC message. Calculated from _COB ID SYNC Message_
                             variable from Object dictionary (index 0x1005).
      periodTime           - Sync period time in [microseconds]. Calculated from _Communication cycle period_
                             variable from Object dictionary (index 0x1006).
      periodTimeoutTime    - Sync period timeout time in [microseconds]
                             (periodTimeoutTime = periodTime * 1,5).
      counterOverflowValue - Value from _Synchronous counter overflow value_
                             variable from Object dictionary (index 0x1019).

      curentSyncTimeIsInsideWindow - True, if curent time is inside synchronous window.
                             In this case synchronous PDO may be sent.

      running              - True, after first SYNC was received or transmitted.
      timer                - Timer for the SYNC message in [microseconds]. Set to
                             zero after received or transmitted SYNC message.
      counter              - Counter of the SYNC message if counterOverflowValue
                             is different than zero.

      CANdevRx             - CAN device for SYNC reception <CO_CANmodule_t>.
      CANdevRxIdx          - Index of receive buffer for SYNC reception.

      CANdevTx             - Pointer to CAN device used for SYNC transmission <CO_CANmodule_t>.
      CANtxBuff            - CAN transmit buffer inside CANdev for CAN tx message.
      CANdevTxIdx          - Index of CAN device TX buffer used for SYNC.
*******************************************************************************/
typedef struct{
   CO_emergencyReport_t         *EM;
   UNSIGNED8                    *operatingState;

   UNSIGNED8                     isProducer;
   UNSIGNED16                    COB_ID;
   UNSIGNED32                    periodTime;
   UNSIGNED32                    periodTimeoutTime;
   UNSIGNED8                     counterOverflowValue;

   UNSIGNED8                     curentSyncTimeIsInsideWindow;

   UNSIGNED32                    running;
   UNSIGNED32                    timer;
   UNSIGNED8                     counter;

   CO_CANmodule_t               *CANdevRx;
   UNSIGNED16                    CANdevRxIdx;

   CO_CANmodule_t               *CANdevTx;
   CO_CANtxArray_t              *CANtxBuff;
   UNSIGNED16                    CANdevTxIdx;
}CO_SYNC_t;


/*******************************************************************************
   Function: CO_SYNC_receive

   Read received message from CAN module.

   Function will be called (by CAN receive interrupt) every time, when CAN
   message with correct identifier will be received. For more information and
   description of parameters see topic <Receive CAN message function>.
*******************************************************************************/
INTEGER16 CO_SYNC_receive(void *object, CO_CANrxMsg_t *msg);


/*******************************************************************************
   Function: CO_ODF_1005

   Function for accessing _COB ID SYNC Message_ (index 0x1005) from SDO server.

   For more information see topic <SDO server access function> in CO_SDO.h file.
*******************************************************************************/
UNSIGNED32 CO_ODF_1005( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED16 *pLength,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData);


/*******************************************************************************
   Function: CO_ODF_1006

   Function for accessing _Communication cycle period_ (index 0x1006) from SDO server.

   For more information see topic <SDO server access function> in CO_SDO.h file.
*******************************************************************************/
UNSIGNED32 CO_ODF_1006( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED16 *pLength,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData);


/*******************************************************************************
   Function: CO_ODF_1019

   Function for accessing _Synchronous counter overflow value_ (index 0x1019) from SDO server.

   For more information see topic <SDO server access function> in CO_SDO.h file.
*******************************************************************************/
UNSIGNED32 CO_ODF_1019( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED16 *pLength,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData);


/*******************************************************************************
   Function: CO_SYNC_init

   Initialize SYNC object.

   Function must be called in the communication reset section.

   Parameters:
      ppSYNC                     - Pointer to address of SYNC object <CO_SYNC_t>.
                                   If address is zero, memory for new object will be
                                   allocated and address will be set.
      EM                         - Pointer to Emergency object <CO_emergencyReport_t>.
      SDO                        - Pointer to SDO object <CO_SDO_t>.
      operatingState             - Pointer to variable indicating CANopen device NMT internal state.

      ObjDict_COB_ID_SYNCMessage - _COB ID SYNC Message_ variable from
                                   Object dictionary (index 0x1005).
      ObjDict_communicationCyclePeriod - _Communication cycle period_ variable from
                                   Object dictionary (index 0x1006).
      ObjDict_synchronousCounterOverflowValue - _Synchronous counter overflow value_
                                   variable from Object dictionary (index 0x1019).

      CANdevRx                   - CAN device for SYNC reception <CO_CANmodule_t>.
      CANdevRxIdx                - Index of receive buffer for SYNC reception.
      CANdevTx                   - CAN device for SYNC transmission <CO_CANmodule_t>.
      CANdevTxIdx                - Index of transmit buffer pointer for SYNC transmission.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
      CO_ERROR_OUT_OF_MEMORY      - Memory allocation failed.
*******************************************************************************/
INTEGER16 CO_SYNC_init(
      CO_SYNC_t             **ppSYNC,
      CO_emergencyReport_t   *EM,
      CO_SDO_t               *SDO,
      UNSIGNED8              *operatingState,

      UNSIGNED32              ObjDict_COB_ID_SYNCMessage,
      UNSIGNED32              ObjDict_communicationCyclePeriod,
      UNSIGNED8               ObjDict_synchronousCounterOverflowValue,

      CO_CANmodule_t *CANdevRx, UNSIGNED16 CANdevRxIdx,
      CO_CANmodule_t *CANdevTx, UNSIGNED16 CANdevTxIdx);


/*******************************************************************************
   Function: CO_SYNC_delete

   Delete SYNC object and free memory.

   Parameters:
      ppSYNC       - Pointer to pointer to SYNC object <CO_SYNC_t>.
                     Pointer to SYNC object is set to 0.
*******************************************************************************/
void CO_SYNC_delete(CO_SYNC_t **ppSYNC);


/*******************************************************************************
   Function: CO_SYNC_process

   Process SYNC communication.

   Function must be called cyclically.

   Parameters:
      SYNC                     - Pointer to SYNC object <CO_SYNC_t>.
      timeDifference_us        - Time difference from previous function call in [microseconds].
      ObjDict_synchronousWindowLength - _Synchronous window length_ variable from
                                 Object dictionary (index 0x1007).

   Return:
      0 - No special meaning.
      1 - New SYNC message recently received or was just transmitted.
      2 - SYNC time was just passed out of window.
*******************************************************************************/
UNSIGNED8 CO_SYNC_process( CO_SYNC_t           *SYNC,
                           UNSIGNED32           timeDifference_us,
                           UNSIGNED32           ObjDict_synchronousWindowLength);


#endif
