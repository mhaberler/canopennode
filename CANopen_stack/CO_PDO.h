/*******************************************************************************

   File: CO_PDO.h
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

#ifndef _CO_PDO_H
#define _CO_PDO_H


/*******************************************************************************
   Topic: PDO

   Process data object (PDO).

   Process data objects are used for real-time data transfer with no protocol
   overhead.

   TPDO with specific identifier is transmitted by one device and recieved by
   zero or more devices as RPDO. PDO communication parameters(COB-ID,
   transmission type, etc.) are in Object Dictionary at index 0x1400+ and
   0x1800+. PDO mapping parameters (size and contents of the PDO) are in Object
   Dictionary at index 0x1600+ and 0x1A00+.

   Features of the PDO as implemented here, in CANopenNode:
    - Dynamic PDO mapping.
    - Map granularity of one byte.
    - After RPDO is received from CAN bus, its data are copied to buffer.
      Function <CO_RPDO_process> (called by application) copies data to
      mapped objects in Object Dictionary.
    - Function <CO_TPDO_process> (called by application) sends TPDO if
      necessary. There are possible different transmission types, including
      automatic detection of Change of State of specific variable.
*******************************************************************************/


/*******************************************************************************
   Object: OD_RPDOCommunicationParameter_t

   _RPDO communication parameter_ record from Object dictionary (index 0x1400+).

   *Type definition in CO_OD.h file must be the same!*

   Variables:
      maxSubIndex       - Equal to 2.
      COB_IDUsedByRPDO  - Communication object identifier for message received.:
                           Bit  0-10: COB-ID for PDO, to change it bit 31 must be set.
                           Bit 11-29: set to 0 for 11 bit COB-ID.
                           Bit 30:    If true, rtr are NOT allowed for PDO.
                           Bit 31:    If true, node does NOT use the PDO.
      transmissionType  - Values:
                           0-240:   Reciving is synchronous, process after next reception of the SYNC object.
                           241-253: Not used.
                           254:     Manufacturer specific.
                           255:     Asynchronous.
*******************************************************************************/
#ifndef _CO_OD_H
   typedef struct{
      UNSIGNED8      maxSubIndex;
      UNSIGNED32     COB_IDUsedByRPDO;
      UNSIGNED8      transmissionType;
   }OD_RPDOCommunicationParameter_t;
#endif


/*******************************************************************************
   Object: OD_RPDOMappingParameter_t

   _RPDO mapping parameter_ record from Object dictionary (index 0x1600+).

   *Type definition in CO_OD.h file must be the same!*

   Variables:
      numberOfMappedObjects   - Actual number of mapped objects from 0 to 8. To
                                change mapped object, this value must be 0.
      mappedObject(1..8)      - Bit meanings:
                                 Bit  0-7:  data length in bits.
                                 Bit 8-15:  subindex from object distionary.
                                 Bit 16-31: index from object distionary.
*******************************************************************************/
#ifndef _CO_OD_H
   typedef struct{
      UNSIGNED8      numberOfMappedObjects;
      UNSIGNED32     mappedObject1;
      UNSIGNED32     mappedObject2;
      UNSIGNED32     mappedObject3;
      UNSIGNED32     mappedObject4;
      UNSIGNED32     mappedObject5;
      UNSIGNED32     mappedObject6;
      UNSIGNED32     mappedObject7;
      UNSIGNED32     mappedObject8;
   }OD_RPDOMappingParameter_t;
#endif


/*******************************************************************************
   Object: OD_TPDOCommunicationParameter_t

   _TPDO communication parameter_ record from Object dictionary (index 0x1800+).

   *Type definition in CO_OD.h file must be the same!*

   Variables:
      maxSubIndex        - Equal to 6.
      COB_IDUsedByTPDO   - Communication object identifier for transmitting message.:
                            Bit  0-10: COB-ID for PDO, to change it bit 31 must be set.
                            Bit 11-29: set to 0 for 11 bit COB-ID.
                            Bit 30:    If true, rtr are NOT allowed for PDO.
                            Bit 31:    If true, node does NOT use the PDO.
      transmissionType   - Values:
                            0:       Transmiting is synchronous, specification in device profile.
                            1-240:   Transmiting is synchronous after every N-th SYNC object.
                            241-251: Not used.
                            252-253: Transmited only on reception of Remote Transmission Request.
                            254:     Manufacturer specific.
                            255:     Asinchronous, specification in device profile.
      inhibitTime        - Minimum time between transmissions of the PDO in 100micro seconds.
                           Zero disables functionality.
      compatibilityEntry - Not used.
      eventTimer         - Time between periodic transmissions of the PDO in milliseconds.
                           Zero disables functionality.
      SYNCStartValue     - Values:
                            0:       Counter of the SYNC message shall not be processed.
                            1-240:   The SYNC message with the counter value equal to this value
                                     shall be regarded as the first received SYNC message.
*******************************************************************************/
#ifndef _CO_OD_H
   typedef struct{
      UNSIGNED8      maxSubIndex;
      UNSIGNED32     COB_IDUsedByTPDO;
      UNSIGNED8      transmissionType;
      UNSIGNED16     inhibitTime;
      UNSIGNED8      compatibilityEntry;
      UNSIGNED16     eventTimer;
      UNSIGNED8      SYNCStartValue;
   }OD_TPDOCommunicationParameter_t;
#endif


/*******************************************************************************
   Object: OD_TPDOMappingParameter_t

   _TPDO mapping parameter_ record from Object dictionary (index 0x1A00+).

   *Type definition in CO_OD.h file must be the same!*

   Variables:
      numberOfMappedObjects   - Actual number of mapped objects from 0 to 8. To
                                change mapped object, this value must be 0.
      mappedObject(1..8)      - Bit meanings:
                                 Bit  0-7:  data length in bits.
                                 Bit 8-15:  subindex from object distionary.
                                 Bit 16-31: index from object distionary.
*******************************************************************************/
#ifndef _CO_OD_H
   typedef struct{
      UNSIGNED8      numberOfMappedObjects;
      UNSIGNED32     mappedObject1;
      UNSIGNED32     mappedObject2;
      UNSIGNED32     mappedObject3;
      UNSIGNED32     mappedObject4;
      UNSIGNED32     mappedObject5;
      UNSIGNED32     mappedObject6;
      UNSIGNED32     mappedObject7;
      UNSIGNED32     mappedObject8;
   }OD_TPDOMappingParameter_t;
#endif


/*******************************************************************************
   Object: CO_RPDO_t

   Object controls one receiving PDO object.

   Variables:
      SDO               - Pointer to SDO object <CO_SDO_t>.
      EM                - Pointer to Emergency object <CO_emergencyReport_t>.
      operatingState    - Pointer to variable indicating CANopen device NMT internal state.
      nodeId            - CANopen Node ID of this device.
      defaultCOB_ID     - Default COB ID for this PDO (without NodeId). See <Default COB identifiers>.
      restrictionFlags  - Flag bits indicates, how PDO communication
                          and mapping parameters are handled:
                           Bit1 = 1: communication parameters are writeable
                                     only in pre-operational NMT state.
                           Bit2 = 1: mapping parameters are writeable
                                     only in pre-operational NMT state.
                           Bit3 = 1: communication parameters are read-only.
                           Bit4 = 1: mapping parameters are read-only.

      valid             - True, if PDO is enabled and valid.
      dataLength        - Data length of the received PDO message. Calculated from mapping.

      mapPointer        - Pointers to 8 data objects, where PDO will be copied.

      CANrxNew          - Variable indicates, if new PDO message received from CAN bus.
      CANrxData         - 8 data bytes of the received message.
      CANdevRx          - CAN device for PDO reception <CO_CANmodule_t>.
      CANdevRxIdx       - Index of receive buffer for PDO reception.
*******************************************************************************/
typedef struct{
   CO_emergencyReport_t         *EM;
   CO_SDO_t                     *SDO;
   UNSIGNED8                    *operatingState;
   UNSIGNED8                     nodeId;
   UNSIGNED16                    defaultCOB_ID;
   UNSIGNED8                     restrictionFlags;

   UNSIGNED8                     valid;
   UNSIGNED8                     dataLength;

   UNSIGNED8                    *mapPointer[8];

   UNSIGNED16                    CANrxNew;      //must be 2-byte variable because of correct alignment of CANrxData
   UNSIGNED8                     CANrxData[8];  //take care for correct (word) alignment!
   CO_CANmodule_t               *CANdevRx;
   UNSIGNED16                    CANdevRxIdx;
}CO_RPDO_t;


/*******************************************************************************
   Object: CO_TPDO_t

   Object controls one receiving PDO object.

   Variables:
      SDO               - Pointer to SDO object <CO_SDO_t>.
      EM                - Pointer to Emergency object <CO_emergencyReport_t>.
      operatingState    - Pointer to variable indicating CANopen device NMT internal state.
      ObjDict_TPDOCommunicationParameter - Pointer to _TPDO communication parameter_ record
                                           from Object Dictionary (index 0x1800+).
      nodeId            - CANopen Node ID of this device.
      defaultCOB_ID     - Default COB ID for this PDO (without NodeId). See <Default COB identifiers>.
      restrictionFlags  - Flag bits indicates, how PDO communication
                          and mapping parameters are handled. See object <CO_RPDO_t>.

      valid             - True, if PDO is enabled and valid.
      dataLength        - Data length of the transmitting PDO message. Calculated from mapping.
      sendRequest       - If application set this flag, PDO will be later sent by
                          function <CO_TPDO_process> (if correct transmission type).

      mapPointer        - Pointers to 8 data objects, where PDO will be copied.
      sendIfCOSFlags    - Each flag bit is connected with one mapPointer. If flag bit
                          is true, <CO_TPDO_process> functiuon will send PDO if
                          Change of State is detected on value pointed by that mapPointer.

      syncCounter       - SYNC counter used for PDO sending.
      SYNCtimerPrevious - Previous timer from <CO_SYNC_t>.
      inhibitTimer      - Inhibit timer used for inhibit PDO sending.
      eventTimer        - Event timer used for PDO sending.

      CANdevTx          - Pointer to CAN device used for PDO transmission <CO_CANmodule_t>.
      CANtxBuff         - CAN transmit buffer inside CANdev for CAN tx message.
      CANdevTxIdx       - Index of CAN device TX buffer used for this PDO.
*******************************************************************************/
typedef struct{
   CO_emergencyReport_t         *EM;
   CO_SDO_t                     *SDO;
   UNSIGNED8                    *operatingState;
   const OD_TPDOCommunicationParameter_t *ObjDict_TPDOCommunicationParameter;
   UNSIGNED8                     nodeId;
   UNSIGNED16                    defaultCOB_ID;
   UNSIGNED8                     restrictionFlags;

   UNSIGNED8                     valid;
   UNSIGNED8                     dataLength;
   UNSIGNED8                     sendRequest;

   UNSIGNED8                    *mapPointer[8];
   UNSIGNED8                     sendIfCOSFlags;

   UNSIGNED8                     syncCounter;
   UNSIGNED32                    SYNCtimerPrevious;
   UNSIGNED16                    inhibitTimer;
   UNSIGNED16                    eventTimer;

   CO_CANmodule_t               *CANdevTx;
   CO_CANtxArray_t              *CANtxBuff;
   UNSIGNED16                    CANdevTxIdx;
}CO_TPDO_t;


/*******************************************************************************
   Function: CO_PDO_receive

   Read received message from CAN module.

   Function will be called (by CAN receive interrupt) every time, when CAN
   message with correct identifier will be received. For more information and
   description of parameters see topic <Receive CAN message function>.
*******************************************************************************/
INTEGER16 CO_PDO_receive(void *object, CO_CANrxMsg_t *msg);


/*******************************************************************************
   Function: CO_RPDOconfigCom

   Configure RPDO Communication parameter.

   Function is called from commuincation reset or when parameter changes.

   Function configures following variable from <CO_RPDO_t>: _valid_. It also
   configures CAN rx buffer. If configuration fails, emergency message is send
   and device is not able to enter NMT operational.

   Parameters:
      RPDO              - Pointer to RPDO object <CO_RPDO_t>.
      COB_IDUsedByRPDO  - _RPDO communication parameter_, _COB-ID for PDO_ variable
                          from Object dictionary (index 0x1400+, subindex 1).
*******************************************************************************/
void CO_RPDOconfigCom(CO_RPDO_t* RPDO, UNSIGNED32 COB_IDUsedByRPDO);


/*******************************************************************************
   Function: CO_TPDOconfigCom

   Configure TPDO Communication parameter.

   Function is called from commuincation reset or when parameter changes.

   Function configures following variable from <CO_TPDO_t>: _valid_. It also
   configures CAN tx buffer. If configuration fails, emergency message is send
   and device is not able to enter NMT operational.

   Parameters:
      TPDO              - Pointer to TPDO object <CO_TPDO_t>.
      COB_IDUsedByTPDO  - _TPDO communication parameter_, _COB-ID for PDO_ variable
                          from Object dictionary (index 0x1400+, subindex 1).
      syncFlag          - Indicate, if TPDO is synchronous.
*******************************************************************************/
void CO_TPDOconfigCom(CO_TPDO_t* TPDO, UNSIGNED32 COB_IDUsedByTPDO, UNSIGNED8 syncFlag);


/*******************************************************************************
   Function: CO_PDOfindMap

   Find mapped variable in Object Dictionary.

   Function is called from CO_R(T)PDOconfigMap or when mapping parameter changes.

   Parameters:
      SDO             - Pointer to SDO object <CO_SDO_t>.
      map             - PDO mapping parameter.
      R_T             - 0 for RPDO map, 1 for TPDO map.
      ppData          - Pointer to returning parameter: pointer to data of mapped variable.
      pLength         - Pointer to returning parameter: *add* length of mapped variable.
      pSendIfCOSFlags - Pointer to returning parameter: sendIfCOSFlags variable.
      pIsMultibyteVar - Pointer to returning parameter: true for multibyte variable.

   Return:
      0 on success, otherwise <SDO abort code>.
*******************************************************************************/
UNSIGNED32 CO_PDOfindMap(  CO_SDO_t      *SDO,
                           UNSIGNED32     map,
                           UNSIGNED8      R_T,
                           UNSIGNED8    **ppData,
                           UNSIGNED8     *pLength,
                           UNSIGNED8     *pSendIfCOSFlags,
                           UNSIGNED8     *pIsMultibyteVar);


/*******************************************************************************
   Function: CO_RPDOconfigMap

   Configure RPDO Mapping parameter.

   Function is called from communication reset or when parameter changes.

   Function configures following variables from <CO_RPDO_t>: _dataLength_ and
   _mapPointer_.

   Parameters:
      RPDO                          - Pointer to RPDO object <CO_RPDO_t>.
      ObjDict_RPDOMappingParameter  - Pointer to _RPDO mapping parameter_ record
                                      from Object dictionary (index 0x1600+).

   Return:
      0 on success, otherwise <SDO abort code>.
*******************************************************************************/
UNSIGNED8 CO_RPDOconfigMap(   CO_RPDO_t* RPDO,
                        const OD_RPDOMappingParameter_t *ObjDict_RPDOMappingParameter);


/*******************************************************************************
   Function: CO_TPDOconfigMap

   Configure TPDO Mapping parameter.

   Function is called from communication reset or when parameter changes.

   Function configures following variables from <CO_TPDO_t>: _dataLength_,
   _mapPointer_ and _sendIfCOSFlags_.

   Parameters:
      TPDO                          - Pointer to TPDO object <CO_TPDO_t>.
      ObjDict_TPDOMappingParameter  - Pointer to _TPDO mapping parameter_ record
                                      from Object dictionary (index 0x1600+).

   Return:
      0 on success, otherwise <SDO abort code>.
*******************************************************************************/
UNSIGNED8 CO_TPDOconfigMap(   CO_TPDO_t* TPDO,
                        const OD_TPDOMappingParameter_t *ObjDict_TPDOMappingParameter);


/*******************************************************************************
   Function: CO_ODF_RPDOcom

   Function for accessing _RPDO communication parameter_ (index 0x1400+) from SDO server.

   For more information see topic <SDO server access function> in CO_SDO.h file.
*******************************************************************************/
UNSIGNED32 CO_ODF_RPDOcom( void       *object,
                           UNSIGNED16  index,
                           UNSIGNED8   subIndex,
                           UNSIGNED16 *pLength,
                           UNSIGNED16  attribute,
                           UNSIGNED8   dir,
                           void       *dataBuff,
                           const void *pData);


/*******************************************************************************
   Function: CO_ODF_TPDOcom

   Function for accessing _TPDO communication parameter_ (index 0x1800+) from SDO server.

   For more information see topic <SDO server access function> in CO_SDO.h file.
*******************************************************************************/
UNSIGNED32 CO_ODF_TPDOcom( void       *object,
                           UNSIGNED16  index,
                           UNSIGNED8   subIndex,
                           UNSIGNED16 *pLength,
                           UNSIGNED16  attribute,
                           UNSIGNED8   dir,
                           void       *dataBuff,
                           const void *pData);


/*******************************************************************************
   Function: CO_ODF_RPDOmap

   Function for accessing _RPDO mapping parameter_ (index 0x1600+) from SDO server.

   For more information see topic <SDO server access function> in CO_SDO.h file.
*******************************************************************************/
UNSIGNED32 CO_ODF_RPDOmap( void       *object,
                           UNSIGNED16  index,
                           UNSIGNED8   subIndex,
                           UNSIGNED16 *pLength,
                           UNSIGNED16  attribute,
                           UNSIGNED8   dir,
                           void       *dataBuff,
                           const void *pData);


/*******************************************************************************
   Function: CO_ODF_TPDOmap

   Function for accessing _TPDO mapping parameter_ (index 0x1A00+) from SDO server.

   For more information see topic <SDO server access function> in CO_SDO.h file.
*******************************************************************************/
UNSIGNED32 CO_ODF_TPDOmap( void       *object,
                           UNSIGNED16  index,
                           UNSIGNED8   subIndex,
                           UNSIGNED16 *pLength,
                           UNSIGNED16  attribute,
                           UNSIGNED8   dir,
                           void       *dataBuff,
                           const void *pData);


/*******************************************************************************
   Function: CO_RPDO_init

   Initialize receiving PDO object.

   Function must be called in the communication reset section.

   Parameters:
      ppRPDO                        - Pointer to address of RPDO object <CO_RPDO_t>.
                                      If address is zero, memory for new object will be
                                      allocated and address will be set.
      EM                            - Pointer to Emergency object <CO_emergencyReport_t>.
      SDO                           - Pointer to SDO object <CO_SDO_t>.
      operatingState                - Pointer to variable indicating CANopen device NMT internal state.
      nodeId                        - CANopen Node ID of this device. If default COB_ID is used, value will be added.
      defaultCOB_ID                 - Default COB ID for this PDO (without NodeId). See <Default COB identifiers>.
      restrictionFlags              - Flag bits indicates, how PDO communication
                                      and mapping parameters are handled. See object <CO_RPDO_t>.
      ObjDict_RPDOCommunicationParameter - Pointer to _RPDO communication parameter_ record from Object
                                      dictionary (index 0x1400+).
      ObjDict_RPDOMappingParameter  - Pointer to _RPDO mapping parameter_ record from Object
                                      dictionary (index 0x1600+).
      ObjDictIndex_RPDOCommunicationParameter - Index in Object Dictionary.
      ObjDictIndex_RPDOMappingParameter - Index in Object Dictionary.
      CANdevRx                      - CAN device for PDO reception <CO_CANmodule_t>.
      CANdevRxIdx                   - Index of receive buffer for PDO reception.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
      CO_ERROR_OUT_OF_MEMORY      - Memory allocation failed.
*******************************************************************************/
INTEGER16 CO_RPDO_init(
      CO_RPDO_t                      **ppRPDO,
      CO_emergencyReport_t            *EM,
      CO_SDO_t                        *SDO,
      UNSIGNED8                       *operatingState,
      UNSIGNED8                        nodeId,
      UNSIGNED16                       defaultCOB_ID,
      UNSIGNED8                        restrictionFlags,
const OD_RPDOCommunicationParameter_t *ObjDict_RPDOCommunicationParameter,
const OD_RPDOMappingParameter_t       *ObjDict_RPDOMappingParameter,
      UNSIGNED16                       ObjDictIndex_RPDOCommunicationParameter,
      UNSIGNED16                       ObjDictIndex_RPDOMappingParameter,
      CO_CANmodule_t *CANdevRx, UNSIGNED16 CANdevRxIdx);


/*******************************************************************************
   Function: CO_RPDO_delete

   Delete RPDO object and free memory.

   Parameters:
      ppRPDO       - Pointer to pointer to RPDO object <CO_RPDO_t>.
                     Pointer to RPDO object is set to 0.
*******************************************************************************/
void CO_RPDO_delete(CO_RPDO_t **ppRPDO);


/*******************************************************************************
   Function: CO_TPDO_init

   Initialize transmitting PDO object.

   Function must be called in the communication reset section.

   Parameters:
      ppTPDO                        - Pointer to address of TPDO object <CO_TPDO_t>.
                                      If address is zero, memory for new object will be
                                      allocated and address will be set.
      EM                            - Pointer to Emergency object <CO_emergencyReport_t>.
      SDO                           - Pointer to SDO object <CO_SDO_t>.
      operatingState                - Pointer to variable indicating CANopen device NMT internal state.
      nodeId                        - CANopen Node ID of this device. If default COB_ID is used, value will be added.
      defaultCOB_ID                 - Default COB ID for this PDO (without NodeId). See <Default COB identifiers>.
      restrictionFlags              - Flag bits indicates, how PDO communication
                                      and mapping parameters are handled. See object <CO_RPDO_t>.
      ObjDict_TPDOCommunicationParameter - Pointer to _TPDO communication parameter_ record from Object
                                      dictionary (index 0x1400+).
      ObjDict_TPDOMappingParameter  - Pointer to _TPDO mapping parameter_ record from Object
                                      dictionary (index 0x1600+).
      ObjDictIndex_TPDOCommunicationParameter - Index in Object Dictionary.
      ObjDictIndex_TPDOMappingParameter - Index in Object Dictionary.
      CANdevTx                      - Pointer to CAN device used for PDO transmission <CO_CANmodule_t>.
      CANdevTxIdx                   - Index of CAN device TX buffer used for this PDO.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
      CO_ERROR_OUT_OF_MEMORY      - Memory allocation failed.
*******************************************************************************/
INTEGER16 CO_TPDO_init(
      CO_TPDO_t                      **ppTPDO,
      CO_emergencyReport_t            *EM,
      CO_SDO_t                        *SDO,
      UNSIGNED8                       *operatingState,
      UNSIGNED8                        nodeId,
      UNSIGNED16                       defaultCOB_ID,
      UNSIGNED8                        restrictionFlags,
const OD_TPDOCommunicationParameter_t *ObjDict_TPDOCommunicationParameter,
const OD_TPDOMappingParameter_t       *ObjDict_TPDOMappingParameter,
      UNSIGNED16                       ObjDictIndex_TPDOCommunicationParameter,
      UNSIGNED16                       ObjDictIndex_TPDOMappingParameter,
      CO_CANmodule_t *CANdevTx, UNSIGNED16 CANdevTxIdx);


/*******************************************************************************
   Function: CO_TPDO_delete

   Delete TPDO object and free memory.

   Parameters:
      ppTPDO       - Pointer to pointer to TPDO object <CO_TPDO_t>.
                     Pointer to TPDO object is set to 0.
*******************************************************************************/
void CO_TPDO_delete(CO_TPDO_t **ppTPDO);


/*******************************************************************************
   Function: CO_TPDOisCOS

   Verify Change of State of the PDO.

   Function verifies if variable mapped to TPDO has changed its value. Verified
   are only variables, which has set attribute _CO_ODA_TPDO_DETECT_COS_ in
   <Attribute flags for sCO_OD_object>.

   Function may be called by application just before <CO_TPDO_process> function,
   for example:
>  TPDOx->sendRequest = CO_TPDOisCOS(TPDOx);
>  CO_TPDO_process(TPDOx, ....

   Parameters:
      TPDO  - Pointer to TPDO object <CO_TPDO_t>.

   Return:
      True if COS was detected.
*******************************************************************************/
UNSIGNED8 CO_TPDOisCOS(CO_TPDO_t *TPDO);


/*******************************************************************************
   Function: CO_TPDOsend

   Send TPDO message. Prepare TPDO data from Object Dictionary variables.

   Parameters:
      TPDO  - Pointer to TPDO object <CO_TPDO_t>.

   Return:
      Same as <CO_CANsend>.
*******************************************************************************/
INTEGER16 CO_TPDOsend(CO_TPDO_t *TPDO);


/*******************************************************************************
   Function: CO_RPDO_process

   Process received PDO messages.

   Function must be called cyclically in any NMT state. It copies data from RPDO
   to Object Dictionary variables if: new PDO receives and PDO is valid and NMT
   operating state is operational. It does not verify _transmission type_.

   Parameters:
      RPDO     - Pointer to RPDO object <CO_RPDO_t>.
*******************************************************************************/
void CO_RPDO_process(CO_RPDO_t *RPDO);


/*******************************************************************************
   Function: CO_TPDO_process

   Process transmitting PDO messages.

   Function must be called cyclically in any NMT state. It prepares and sends
   TPDO if necessary. If Change of State needs to be detected, function
   <CO_TPDOisCOS> must be called before.

   Parameters:
      TPDO                 - Pointer to TPDO object <CO_TPDO_t>.
      SYNC                 - Pointer to SYNC object <CO_SYNC_t>. Ignored if NULL.
      timeDifference_100us - Time difference from previous function call in [100 * microseconds].
      timeDifference_ms    - Time difference from previous function call in [milliseconds].
*******************************************************************************/
void CO_TPDO_process(   CO_TPDO_t     *TPDO,
                        CO_SYNC_t     *SYNC,
                        UNSIGNED16     timeDifference_100us,
                        UNSIGNED16     timeDifference_ms);


#endif
