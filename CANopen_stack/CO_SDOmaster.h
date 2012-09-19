/*******************************************************************************

   File: CO_SDOmaster.h
   CANopen SDO client object (SDO master).

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

#ifndef _CO_SDO_CLIENT_H
#define _CO_SDO_CLIENT_H


/*******************************************************************************
   Object: OD_SDOClientParameter_t

   _SDO Client Parameter_ record from Object dictionary (index 0x1280+).

   *Type definition in CO_OD.h file must be the same!*

   Variables:
      maxSubIndex          - Equal to 3.
      COB_IDClientToServer - Communication object identifier for client transmission:
                              Bit 0...10: 11-bit CAN identifier.
                              Bit 11..30: reserved, set to 0.
                              Bit 31: if 1, SDO client object is not used.
      COB_IDServerToClient - Communication object identifier for message received from
                             server. Meaning of specific bits is the same as above.
      nodeIDOfTheSDOServer - Node-ID of the SDO server.
*******************************************************************************/
#ifndef _CO_OD_H
   typedef struct{
      UNSIGNED8      maxSubIndex;
      UNSIGNED32     COB_IDClientToServer;
      UNSIGNED32     COB_IDServerToClient;
      UNSIGNED8      nodeIDOfTheSDOServer;
   }OD_SDOClientParameter_t;
#endif


/*******************************************************************************
   Object: CO_SDOclient_t

   Object controls SDO client.

   Variables:
      ObjDict_SDOClientParameter - Pointer to _SDO Client Parameter_ record from Object
                       dictionary (index 0x1280+). See <OD_SDOClientParameter_t>.

      SDO            - Pointer to SDO object <CO_SDO_t>. It is used in case, if
                       client is accessing object dictionary from its own device.
                       If NULL, it will be ignored.

      state          - Internal state of the SDO client:
                        Bit0 = 0: not used.
                        Bit1 = 1: segmented download in progress.
                        Bit2 = 1: segmented upload in progress.
                        Bit3 = 0: not used.
                        Bit4: toggled bit from previous object.
                        Bit5 = 1: download initiated.
                        Bit6 = 1: upload initiated.
      buffer         - Pointer to data buffer supplied by user.
      bufferSize     - By download application indicates data size in buffer,
                       by upload application indicates buffer size.
      bufferOffset   - Offset in buffer of next data segment being read/written.
      timeoutTimer   - Timeout timer for SDO communication.
      index          - Index of current object in Object Dictionary.
      subIndex       - Subindex of current object in Object Dictionary.

      CANdevRx       - CAN device for SDO client reception <CO_CANmodule_t>.
      CANdevRxIdx    - Index of receive buffer for SDO client reception.
      CANrxNew       - Variable indicates, if new SDO message received from CAN bus.
      CANrxData      - 8 data bytes of the received message.

      pFunctSignal   - Pointer to optional external function. If defined, it is
                       called from high priority interrupt after new CAN SDO
                       response message is received. Function may wake up
                       external task, which processes SDO client functions.
      functArg       - Optional argument, which is passed to above function.

      CANdevTx       - Pointer to CAN device used for SDO client transmission <CO_CANmodule_t>.
      CANtxBuff      - CAN transmit buffer inside CANdev for CAN tx message.
      CANdevTxIdx    - Index of CAN device TX buffer used for SDO client.
*******************************************************************************/
typedef struct{
   OD_SDOClientParameter_t *ObjDict_SDOClientParameter;
   CO_SDO_t               *SDO;

   UNSIGNED8               state;
   UNSIGNED8              *buffer;
   UNSIGNED16              bufferSize;
   UNSIGNED16              bufferOffset;
   UNSIGNED16              timeoutTimer;
   UNSIGNED16              index;
   UNSIGNED8               subIndex;

   CO_CANmodule_t         *CANdevRx;
   UNSIGNED16              CANdevRxIdx;
   UNSIGNED16              CANrxNew;      //must be 2-byte variable because of correct alignment of CANrxData
   UNSIGNED8               CANrxData[8];  //take care for correct (word) alignment!

   void                  (*pFunctSignal)(UNSIGNED32 arg);
   UNSIGNED32              functArg;

   CO_CANmodule_t         *CANdevTx;
   CO_CANtxArray_t        *CANtxBuff;
   UNSIGNED16              CANdevTxIdx;
}CO_SDOclient_t;



/*******************************************************************************
   Function: CO_SDOclient_receive

   Read received message from CAN module.

   Function will be called (by CAN receive interrupt) every time, when CAN
   message with correct identifier will be received. For more information and
   description of parameters see topic <Receive CAN message function>.
*******************************************************************************/
INTEGER16 CO_SDOclient_receive(void *object, CO_CANrxMsg_t *msg);


/*******************************************************************************
   Function: CO_SDOclient_init

   Initialize SDO client object.

   Function must be called in the communication reset section.

   Parameters:
      ppSDOclient                - Pointer to address of SDOclient object <CO_SDOclient_t>.
                                   If address is zero, memory for new object will be
                                   allocated and address will be set.
      SDO_C                      - Pointer to SDO client object <CO_SDOclient_t>.
      SDO                        - Pointer to SDO object <CO_SDO_t>. It is used in case, if
                                   client is accessing object dictionary from its own device.
                                   If NULL, it will be ignored.
      ObjDict_SDOClientParameter - Pointer to _SDO Client Parameter_ record from Object
                                   dictionary (index 0x1280+). Will be written.
      CANdevRx                   - CAN device for SDO client reception <CO_CANmodule_t>.
      CANdevRxIdx                - Index of receive buffer for SDO client reception.
      CANdevTx                   - CAN device for SDO client transmission <CO_CANmodule_t>.
      CANdevTxIdx                - Index of transmit buffer pointer for SDO client transmission.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
      CO_ERROR_OUT_OF_MEMORY      - Memory allocation failed.
*******************************************************************************/
INTEGER16 CO_SDOclient_init(
      CO_SDOclient_t           **ppSDOclient,
      CO_SDO_t                  *SDO,
      OD_SDOClientParameter_t   *ObjDict_SDOClientParameter,
      CO_CANmodule_t *CANdevRx, UNSIGNED16 CANdevRxIdx,
      CO_CANmodule_t *CANdevTx, UNSIGNED16 CANdevTxIdx);


/*******************************************************************************
   Function: CO_SDOclient_delete

   Delete SDOclient object and free memory.

   Parameters:
      ppSDOclient       - Pointer to pointer to SDOclient object <CO_SDOclient_t>.
                          Pointer to SDOclient object is set to 0.
*******************************************************************************/
void CO_SDOclient_delete(CO_SDOclient_t **ppSDOclient);


/*******************************************************************************
   Function: CO_SDOclient_setup

   Setup SDO client object.

   Function must be called before new SDO communication. If previous SDO
   communication was with the same node, function does not need to be called.

   Parameters:
      SDO_C                      - Pointer to SDO client object <CO_SDOclient_t>.
      COB_IDClientToServer       - See <OD_SDOClientParameter_t>. If zero, then
                                   nodeIDOfTheSDOServer is used with default COB-ID.
      COB_IDServerToClient       - See <OD_SDOClientParameter_t>. If zero, then
                                   nodeIDOfTheSDOServer is used with default COB-ID.
      nodeIDOfTheSDOServer       - Node-ID of the SDO server. If zero, SDO client
                                   object is not used. If it is the same as node-ID of
                                   this node, then data will be exchanged with this node.

   Return:
      0  - Success.
     -2  - Wrong arguments.
*******************************************************************************/
INTEGER8 CO_SDOclient_setup(  CO_SDOclient_t            *SDO_C,
                              UNSIGNED32                 COB_IDClientToServer,
                              UNSIGNED32                 COB_IDServerToClient,
                              UNSIGNED8                  nodeIDOfTheSDOServer);


/*******************************************************************************
   Function: CO_SDOclientDownloadInitiate

   Initiate SDO download communication.

   Function initiates SDO download communication with server specified in
   <CO_SDOclient_init()> function. Data will be written to remote node.
   Function is nonblocking.

   Parameters:
      SDO_C    - Pointer to SDO client object <CO_SDOclient_t>.
      index    - Index of object in object dictionary in remote node.
      subIndex - Subindex of object in object dictionary in remote node.
      dataTx   - Pointer to data to be written. Data must be valid untill end
                 of communication. Note that data are aligned in little-endian
                 format, because CANopen itself uses little-endian. Take care,
                 when using processors with big-endian.
      dataSize - Size of data in dataTx.

   Return:
      0  - Success.
     -2  - Wrong arguments.
*******************************************************************************/
INTEGER8 CO_SDOclientDownloadInitiate( CO_SDOclient_t   *SDO_C,
                                       UNSIGNED16        index,
                                       UNSIGNED8         subIndex,
                                       UNSIGNED8        *dataTx,
                                       UNSIGNED16        dataSize);


/*******************************************************************************
   Function: CO_SDOclientDownload

   Process SDO download communication.

   Function must be called cyclically untill it returns <=0. It Proceeds SDO
   download communication initiated with <CO_SDOclientDownloadInitiate()>.
   Function is nonblocking.

   Parameters:
      SDO_C             - Pointer to SDO client object <CO_SDOclient_t>.
      timeDifference_ms - Time difference from previous function call in [milliseconds].
      SDOtimeoutTime    - Timeout time for SDO communication in milliseconds.
      pSDOabortCode     - Pointer to external variable written by this function
                          in case of error in communication.

   Return:
      2  - Server responded, new client request was sent.
      1  - Waiting for server response.
      0  - End of communication.
     -3  - Error: communication was not properly initiaded.
     -10 - Error in SDO communication. SDO abort code is in value pointed by pSDOabortCode.
     -11 - Error: timeout in SDO communication, SDO abort code is in value pointed by pSDOabortCode.
*******************************************************************************/
INTEGER8 CO_SDOclientDownload(   CO_SDOclient_t      *SDO_C,
                                 UNSIGNED16           timeDifference_ms,
                                 UNSIGNED16           SDOtimeoutTime,
                                 UNSIGNED32          *pSDOabortCode);


/*******************************************************************************
   Function: CO_SDOclientUploadInitiate

   Initiate SDO upload communication.

   Function initiates SDO upload communication with server specified in
   <CO_SDOclient_init()> function. Data will be read from remote node.
   Function is nonblocking.

   Parameters:
      SDO_C       - Pointer to SDO client object <CO_SDOclient_t>.
      index       - Index of object in object dictionary in remote node.
      subIndex    - Subindex of object in object dictionary in remote node.
      dataRx      - Pointer to data buffer data will be written. Buffer must be
                    valid untill end of communication. Note that data are aligned
                    in little-endian format, because CANopen itself uses
                    little-endian. Take care, when using processors with big-endian.
      dataRxSize  - Size of dataRx.

   Return:
      0  - Success.
     -2  - Wrong arguments.
*******************************************************************************/
INTEGER8 CO_SDOclientUploadInitiate(   CO_SDOclient_t   *SDO_C,
                                       UNSIGNED16        index,
                                       UNSIGNED8         subIndex,
                                       UNSIGNED8        *dataRx,
                                       UNSIGNED16        dataRxSize);


/*******************************************************************************
   Function: CO_SDOclientUpload

   Process SDO upload communication.

   Function must be called cyclically untill it returns <=0. It Proceeds SDO
   upload communication initiated with <CO_SDOclientUploadInitiate()>.
   Function is nonblocking.

   Parameters:
      SDO_C             - Pointer to SDO client object <CO_SDOclient_t>.
      timeDifference_ms - Time difference from previous function call in [milliseconds].
      SDOtimeoutTime    - Timeout time for SDO communication in milliseconds.
      pDataSize         - pointer to external variable, where size of received
                          data will be written.
      pSDOabortCode     - Pointer to external variable written by this function
                          in case of error in communication.

   Return:
      2  - Server responded, new client request was sent.
      1  - Waiting for server response.
      0  - End of communication.
     -3  - Error: communication was not properly initiated.
     -10 - Error in SDO communication. SDO abort code is in value pointed by pSDOabortCode.
     -11 - Error: timeout in SDO communication, SDO abort code is in value pointed by pSDOabortCode.
*******************************************************************************/
INTEGER8 CO_SDOclientUpload(  CO_SDOclient_t      *SDO_C,
                              UNSIGNED16           timeDifference_ms,
                              UNSIGNED16           SDOtimeoutTime,
                              UNSIGNED16          *pDataSize,
                              UNSIGNED32          *pSDOabortCode);

#endif
