/*******************************************************************************

   File: CO_SDO.h
   CANopen SDO server object (SDO slave).

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

#ifndef _CO_SDO_H
#define _CO_SDO_H


/*******************************************************************************
   Topic: Object Dictionary

   Description of CANopen Object Distionary, as implemented here, in CANopenNode.

   One of the most powerful feature of CANopen is Object Dictionary. It is like
   a table, where are collected all network-accessible data. One varible in
   Object dictionary is located under 16-bit index. If variable type is array
   or structure (Record as notated by CANopen), then every member has assigned
   also a subIndex. CANopen provides SDO objects to access different variables
   in Object Dictionary.

   In CANopenNode Object Dictionary is an array of objects CO_OD[]. Each object
   has its own index. Object Type can be: Variable, Array of variables or Record.
   (Record is like a structure in C.) Each object has also information about
   access, memory type, length and pointer to data variable, etc.

   There are 'Two sides' of Object Dictionary in CANopenNode:
    - Program side: Variables have ordinary names (no index, subIndex) and types;
    - CANopen side: Ordinary variables are collected in CO_OD array and so SDO
      server have access to them through index, subIndex and length (read only,
      read/write etc.).

   Memory strategy in CANopenNode:
   Memory for Object Dictionary variables is divided into three
   groups (structures). Each variable can be in any of the following groups:
    - *RAM variables* are for general use. On startup they have initial value.
      They can be changed by application or from network side (by SDO communication).
    - *EEPROM variables* are retentive (value is retained, if power supply is
      disconnected). They can be changed by application (usage is the same as
      with RAM variables). Of course they can also be changed from network side.
      Access to them is fast. On some processors are memory resources for them
      limited. They can be used for example for counter of events.
    - *ROM variables* are retentive. By application they can be read normally,
      but they can _not_ be changed. Anyway, they are writeable from network side.
      Memory resources for them are large on all processors. On some processors
      writing to it takes some time. They can be used for example for
      many configuration parameters.


********************************************************************************
   Topic: SDO server access function

   Function, which reads/writes Object dictionary variable data.

   SDO server access functions are called through function pointer, which is
   assigned to each object in Object Dictionary. Functions are called by SDO
   Server, which reads (or writes) value from (to) specific Object. (SDO server
   does not have direct access to Object Dictionary variables. When it needs to
   read or write specific variable, it calls a SDO server access function.)

   Each SDO Server access function has a prefix CO_ODF to its function name.

   Basic function is <CO_ODF>, which reads or writes into RAM or ROM. If
   microcontroller uses different memory strategies for retentive usage, this
   function may be implemented in CO_driver.c file.

   To any object can be assigned also a custom function, which has total control
   over the Object Dictionary object. Object Dictionary Editor gives two
   possibilities for custom function:
    - SDO Server _external_ function is for example CO_ODF_RPDOcom() or
      CO_ODF_RPDOmap(), which handle PDO mapping or communication parameters.
      These functions are very powerful, because they can handle for example
      dynamic PDO mapping, etc. User can write his own CO_ODF_ function and
      assign it to his object, to have full control over it. One limitation
      to external function is, that object can not be dynamically mapped into PDO.
    - _Default with user code_ SDO Server access function is simpler. It can be
      used most commonly for verifying data, before SDO server writes them to
      memory. Most usual is to verify low and high limit of the variable and
      return SDO abort code, if value is out of range. Function code can be
      written directly in Object Dictionary Editor.

   First argument (object) to SDO server access function is a little special. It
   can be used with SDO Server _external_ function. It is a void pointer to
   object, which contains additional information, how to handle the data. If
   SDO server access function uses it, it *must be configured* in function
   <CO_OD_configureArgumentForODF>.

   Argument 'dataBuff' contains raw data, which are directly connected with CAN
   (CANopen) message. CANopen itself is a little endian, so 'dataBuff' is also
   little endian. Care must be taken, when manipulating with this argument.
   Helper functions may be used with it, for example <memcpySwapN>.

   Parameters:
      object      - object passed to <CO_OD_configureArgumentForODF> function.
      index       - Index of object in object dictionary (informative).
      subIndex    - Subindex of object in object dictionary (informative).
      attribute   - Attribute of object in object dictionary (informative).
      pLength     - Pointer to number of bytes to be transfered (writeable).
      dir         - Equal to 0 when reading, equal to 1 when writing.
      dataBuff    - Pointer to RAM data buffer to/from where data will be transfered.
      pData       - Pointer to variable in Object Dictionary.

   Return:
      0                 - Data transfer is successful.
      different than 0  - Failure. See <SDO abort code>.

********************************************************************************
   Topic: SDO message contents

   Contents of SDO messages.

   For CAN identifier see <Default COB identifiers>.

   Expedited transfer is used for transmission of up to 4 data bytes. It consists
   of one SDO request and one response. For longer variables is used segmented transfer.

   Initiate SDO download (client request):
      byte 0      - SDO command specifier. 8 bits: '0010nnes' (nn: if e=s=1,
                    number of data bytes, that do _not_ contain data; e=1 for
                    expedited transfer; s=1 if data size is indicated).
      byte 1..2   - Object index.
      byte 3      - Object subIndex.
      byte 4..7   - Expedited data or data size if segmented transfer.

   Initiate SDO download (server response):
      byte 0      - SDO command specifier. 8 bits: '01100000'.
      byte 1..2   - Object index.
      byte 3      - Object subIndex.
      byte 4..7   - reserved.

   Download SDO segment (client request):
      byte 0      - SDO command specifier. 8 bits: '000tnnnc' (t: toggle bit set
                    to 0 in first segment; nnn: number of data bytes, that do
                    _not_ contain data; c=1 if this is the last segment).
      byte 1..7   - Data segment.

   Download SDO segment (server response):
      byte 0      - SDO command specifier. 8 bits: '001t0000' (t: toggle bit set
                    to 0 in first segment).
      byte 1..7   - Reserved.

   Initiate SDO upload (client request):
      byte 0      - SDO command specifier. 8 bits: '01000000'.
      byte 1..2   - Object index.
      byte 3      - Object subIndex.
      byte 4..7   - Reserved.

   Initiate SDO upload (server response):
      byte 0      - SDO command specifier. 8 bits: '0100nnes' (nn: if e=s=1,
                    number of data bytes, that do _not_ contain data; e=1 for
                    expedited transfer; s=1 if data size is indicated).
      byte 1..2   - Object index.
      byte 3      - Object subIndex.
      byte 4..7   - reserved.

   Upload SDO segment (client request):
      byte 0      - SDO command specifier. 8 bits: '011t0000' (t: toggle bit set
                    to 0 in first segment).
      byte 1..7   - Reserved.

   Upload SDO segment (server response):
      byte 0      - SDO command specifier. 8 bits: '000tnnnc' (t: toggle bit set
                    to 0 in first segment; nnn: number of data bytes, that do
                    _not_ contain data; c=1 if this is the last segment).
      byte 1..7   - Data segment.

   Abort SDO transfer (client or server):
      byte 0      - SDO command specifier. 8 bits: '10000000'.
      byte 1..2   - Object index.
      byte 3      - Object subIndex.
      byte 4..7   - <SDO abort code>.

   Byte 0 contents for SDO client request:
      0010nnes    - Download initiate.
      000tnnnc    - Download segment.
      01000000    - Upload initiate.
      011t0000    - Upload segment.
      11000rs0    - Block download initiate.
      csssssss    - Block download sub-block (sequence).
      110nnn01    - Block download end.
      10000000    - Abort transfer.

   Byte 0 contents for server response:
      01100000    - Download initiate.
      001t0000    - Download segment.
      0100nnes    - Upload initiate.
      000tnnnc    - Upload segment.
      10100r00    - Block download initiate.
      10100010    - Block download sub-block (after sequence).
      10100001    - Block download end.
      10000000    - Abort transfer.



********************************************************************************
   Topic: SDO abort code

   Send with Abort SDO transfer message.

   The abort codes not listed here are reserved.

   0x05030000  - Toggle bit not alternated.
   0x05040000  - SDO protocol timed out.
   0x05040001  - Client/server command specifier not valid or unknown.
   0x05040002  - Invalid block size (block mode only).
   0x05040003  - Invalid sequence number (block mode only).
   0x05040004  - CRC error (block mode only).
   0x05040005  - Out of memory.
   0x06010000  - Unsupported access to an object.
   0x06010001  - Attempt to read a write only object.
   0x06010002  - Attempt to write a read only object.
   0x06020000  - Object does not exist in the object dictionary.
   0x06040041  - Object cannot be mapped to the PDO.
   0x06040042  - The number and length of the objects to be mapped would exceed PDO length.
   0x06040043  - General parameter incompatibility reason.
   0x06040047  - General internal incompatibility in the device.
   0x06060000  - Access failed due to an hardware error.
   0x06070010  - Data type does not match, length of service parameter does not match
   0x06070012  - Data type does not match, length of service parameter too high
   0x06070013  - Data type does not match, length of service parameter too low
   0x06090011  - Sub-index does not exist.
   0x06090030  - Invalid value for parameter (download only).
   0x06090031  - Value of parameter written too high (download only).
   0x06090032  - Value of parameter written too low (download only).
   0x06090036  - Maximum value is less than minimum value.
   0x08000000  - general error
   0x08000020  - Data cannot be transferred or stored to the application.
   0x08000021  - Data cannot be transferred or stored to the application because of local control.
   0x08000022  - Data cannot be transferred or stored to the application because of the present device state.
   0x08000023  - Object dictionary dynamic generation fails or no object dictionary is present (e.g. object
                 dictionary  is generated from file and generation fails because of an file error).
   0x08000024  - No data available.
*******************************************************************************/


/*******************************************************************************
   Constant: Maximum object size in Object Dictionary
      CO_OD_MAX_OBJECT_SIZE   - Value can be in range from 4 to 889 bytes.
*******************************************************************************/
   #ifndef CO_OD_MAX_OBJECT_SIZE
      #define CO_OD_MAX_OBJECT_SIZE    32
   #endif


/*******************************************************************************
   Constants: Attribute flags for sCO_OD_object
      CO_ODA_MEM_ROM          - (0x01) - Variable is located in ROM memory.
      CO_ODA_MEM_RAM          - (0x02) - Variable is located in RAM memory.
      CO_ODA_MEM_EEPROM       - (0x03) - Variable is located in EEPROM memory.
      CO_ODA_READABLE         - (0x04) - SDO server may read from the variable.
      CO_ODA_WRITEABLE        - (0x08) - SDO server may write to the variable.
      CO_ODA_RPDO_MAPABLE     - (0x10) - Variable is mappable for RPDO.
      CO_ODA_TPDO_MAPABLE     - (0x20) - Variable is mappable for TPDO.
      CO_ODA_TPDO_DETECT_COS  - (0x40) - If variable is mapped to any PDO, then
                                         PDO is automatically send, if variable
                                         changes its value.
      CO_ODA_MB_VALUE         - (0x80) - True when variable is a multibyte value.
*******************************************************************************/
   #define CO_ODA_MEM_ROM          0x01   //same attribute is in CO_driver.c file
   #define CO_ODA_MEM_RAM          0x02   //same attribute is in CO_driver.c file
   #define CO_ODA_MEM_EEPROM       0x03   //same attribute is in CO_driver.c file
   #define CO_ODA_READABLE         0x04
   #define CO_ODA_WRITEABLE        0x08
   #define CO_ODA_RPDO_MAPABLE     0x10
   #define CO_ODA_TPDO_MAPABLE     0x20
   #define CO_ODA_TPDO_DETECT_COS  0x40
   #define CO_ODA_MB_VALUE         0x80   //same attribute is in CO_driver.c file


/*******************************************************************************
   Object: sCO_OD_object

   Type for Object Dictionary array.

   As mentioned in <Object Dictionary> topic, all Object dictionary variables
   are 'indexed' inside one array. Each variable, array or record has its own
   16-bit index and its own member in Object Dictionary array.

   Variables:
      index       - The index of Object from 0x1000 to 0xFFFF.
      maxSubIndex - It is number of (sub-objects - 1). If Object Type is
                    variable, then maxSubIndex is 0, otherwise maxSubIndex is
                    equal or greater than 1.
      attribute   - If Object Type is record, attribute is set to zero. Attribute
                    for each member is then set in special array with members of
                    type <CO_ODrecord_t>. If Object Type is Array, attribute is
                    common for all array members. See <Attribute flags for sCO_OD_object>.
      length      - If Object Type is Variable, length is the length of variable in bytes.
                    If Object Type is Array, length is the length of one array member.
                    If Object Type is record, length is zero. Length for each member is
                    set in special array with members of type <CO_ODrecord_t>.
      pData       - If Object Type is Variable, pData is pointer to data.
                    If Object Type is Array, pData is pointer to data. Data doesn't include Sub-Object 0.
                    If object type is Record, pData is pointer to special array
                    with members of type <CO_ODrecord_t>.
      pFunct      - is pointer to <SDO server access function>, which reads/writes
                    the data object, when SDO is accessing it.
*******************************************************************************/
typedef struct {
   UNSIGNED16    index;
   UNSIGNED8     maxSubIndex;
   UNSIGNED8     attribute;
   UNSIGNED8     length;
   const void*   pData;
   UNSIGNED32  (*pFunct)(void *object, UNSIGNED16 index, UNSIGNED8 subIndex,
                   UNSIGNED16 *pLength, UNSIGNED16 attribute, UNSIGNED8 dir,
                   void* dataBuff, const void* pData);
}sCO_OD_object;


/*******************************************************************************
   Object: CO_ODrecord_t

   Type for record type objects in Object Dictionary array.

   See <sCO_OD_object>.

   Variables:
      attribute - See <Attribute flags for sCO_OD_object>.
      length    - Length of variable in bytes.
      pData     - Pointer to data.
*******************************************************************************/
typedef struct{
   const void* pData;
   UNSIGNED8   attribute;
   UNSIGNED8   length;
}CO_ODrecord_t;


/*******************************************************************************
   Object: OD_SDOServerParameter_t

   _SDO Server Parameter_ record from Object dictionary (index 0x1200+).

   *Type definition in CO_OD.h file must be the same!*

   Variables:
      maxSubIndex          - Equal to 2.
      COB_IDClientToServer - Communication object identifier for message received from client:
                              Bit 0...10: 11-bit CAN identifier.
                              Bit 11..30: reserved, set to 0.
                              Bit 31: if 1, SDO server object is not used.
      COB_IDServerToClient - Communication object identifier for server transmission.
                             Meaning of specific bits is the same as above.
*******************************************************************************/
#ifndef _CO_OD_H
   typedef struct{
      UNSIGNED8      maxSubIndex;
      UNSIGNED32     COB_IDClientToServer;
      UNSIGNED32     COB_IDServerToClient;
   }OD_SDOServerParameter_t;
#endif


/*******************************************************************************
   Object: CO_SDO_t

   Object controls SDO server.

   Variables:
      ObjectDictionary           - Pointer to Object dictionary.
      ObjectDictionarySize       - Size of Object Dictionary.
      ObjectDictionaryPointers   - Array of pointers of size ObjectDictionarySize.
                                   See also <SDO server access function> topic.

      nodeId               - CANopen Node ID of this device.

      dataLength           - Actual length of data in databuffer.
      databuffer           - Data buffer.
      bufferOffset         - Offset in buffer of next data segment being read/written.
      state                - Current internal state of the SDO server:
                              Bit1 = 1: segmented download in progress.
                              Bit2 = 1: segmented upload in progress.
                              Bit4: toggled bit from previous object.
      pODE                 - Pointer to current Object Dictionary object.
      indexOfFoundObject   - Similar to pODE, but this is the index of current
                             Object Dictionary object.
      index                - Index of current object in Object Dictionary.
      subIndex             - Subindex of current object in Object Dictionary.
      timeoutTimer         - Timeout timer for SDO communication.

      CANrxNew             - Variable indicates, if new SDO message received from CAN bus.
      CANrxData            - 8 data bytes of the received message.

      CANdevTx             - Pointer to CAN device used for SDO server transmission <CO_CANmodule_t>.
      CANtxBuff            - CAN transmit buffer inside CANdev for CAN tx message.
*******************************************************************************/
typedef struct{
   UNSIGNED8                     CANrxData[8];  //take care for correct (word) alignment!
   UNSIGNED8                     databuffer[CO_OD_MAX_OBJECT_SIZE];  //take care for correct (word) alignment!

   const sCO_OD_object          *ObjectDictionary;
   UNSIGNED16                    ObjectDictionarySize;
   void                        **ObjectDictionaryPointers;

   UNSIGNED8                     nodeId;

   UNSIGNED16                    dataLength;
   UNSIGNED16                    bufferOffset;
   UNSIGNED8                     state;
   const sCO_OD_object          *pODE;
   UNSIGNED16                    indexOfFoundObject;
   UNSIGNED16                    index;
   UNSIGNED8                     subIndex;
   UNSIGNED16                    timeoutTimer;

   UNSIGNED8                     CANrxNew;

   CO_CANmodule_t               *CANdevTx;
   CO_CANtxArray_t              *CANtxBuff;
}CO_SDO_t;


/*******************************************************************************
   Function: CO_SDO_receive

   Read received message from CAN module.

   Function will be called (by CAN receive interrupt) every time, when CAN
   message with correct identifier will be received. For more information and
   description of parameters see topic <Receive CAN message function>.
*******************************************************************************/
INTEGER16 CO_SDO_receive(void *object, CO_CANrxMsg_t *msg);


/*******************************************************************************
   Function: CO_ODF

   Default function for SDO server access of variables from Object Dictionary.

   Function copies data, when CANopen SDO communication is triggered. If read
   from Object Dictionary is requested, data are copied from pData to dataBuff.
   In case of write, data are copied from dataBuff to pData.
   If microcontroller is Big Endian, and data is multibyte variable (for
   example INTEGER32), byte order is reversed.

   If for specific microcontroller needs to be implemented special CO_ODF
   function (for example if copy of ROM variables is special), then macro
   'CO_ODF_MASK_DEFAULT' must be defined in CO_driver.h file. It will mask
   default function.

   For more information see topic <SDO server access function>.
*******************************************************************************/
UNSIGNED32 CO_ODF(   void       *object,
                     UNSIGNED16  index,
                     UNSIGNED8   subIndex,
                     UNSIGNED16 *pLength,
                     UNSIGNED16  attribute,
                     UNSIGNED8   dir,
                     void       *dataBuff,
                     const void *pData);


/*******************************************************************************
   Function: CO_ODF_1200

   Function for accessing _SDO server parameter_ (index 0x1200+) from SDO server.

   For more information see topic <SDO server access function> in CO_SDO.h file.
*******************************************************************************/
UNSIGNED32 CO_ODF_1200( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED16 *pLength,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData);


/*******************************************************************************
   Function: CO_OD_configureArgumentForODF

   Configure first argument, which will be passed to CO_ODFxxx function.

   This function configures one element in 'ObjectDictionaryPointers' array.
   Function assigns it the value of object. Later, when <SDO server access function>
   will be called on Object Dictionary element with specific index, this pointer
   will be passed to it as a first argument.

   Parameters:
      SDO                        - Pointer to SDO object <CO_SDO_t>.
      index                      - Index of object in object dictionary.
      object                     - Pointer to object, which will be passed to CO_ODFxxxx.
*******************************************************************************/
void CO_OD_configureArgumentForODF( CO_SDO_t      *SDO,
                                    UNSIGNED16     index,
                                    void          *object);


/*******************************************************************************
   Function: CO_SDO_init

   Initialize SDO object.

   Function must be called in the communication reset section.

   Parameters:
      ppSDO                      - Pointer to address of SDO object <CO_SDO_t>.
                                   If address is zero, memory for new object will be
                                   allocated and address will be set.
      COB_IDClientToServer       - 0x600 + nodeId by default.
      COB_IDServerToClient       - 0x580 + nodeId by default.
      ObjDict_SDOServerParameter - Pointer to _SDO Server Parameter_ record from Object
                                   dictionary (index 0x1200+).
      ObjDictIndex_SDOServerParameter - Index in Object Dictionary.
      ObjectDictionary           - Pointer to Object dictionary.
      ObjectDictionarySize       - Size of Object Dictionary.
      nodeId                     - CANopen Node ID of this device. Value will be added to
                                   COB_IDs.
      CANdevRx                   - CAN device for SDO server reception <CO_CANmodule_t>.
      CANdevRxIdx                - Index of receive buffer for SDO server reception.
      CANdevTx                   - CAN device for SDO server transmission <CO_CANmodule_t>.
      CANdevTxIdx                - Index of transmit buffer pointer for SDO server transmission.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
      CO_ERROR_OUT_OF_MEMORY      - Memory allocation failed.
*******************************************************************************/
INTEGER16 CO_SDO_init(
      CO_SDO_t                    **ppSDO,
      UNSIGNED16                    COB_IDClientToServer,
      UNSIGNED16                    COB_IDServerToClient,
      UNSIGNED16                    ObjDictIndex_SDOServerParameter,
const sCO_OD_object                *ObjectDictionary,
      UNSIGNED16                    ObjectDictionarySize,
      UNSIGNED8                     nodeId,
      CO_CANmodule_t *CANdevRx, UNSIGNED16 CANdevRxIdx,
      CO_CANmodule_t *CANdevTx, UNSIGNED16 CANdevTxIdx);


/*******************************************************************************
   Function: CO_SDO_delete

   Delete SDO object and free memory.

   Parameters:
      ppSDO       - Pointer to pointer to SDO object <CO_SDO_t>.
                    Pointer to SDO object is set to 0.
*******************************************************************************/
void CO_SDO_delete(CO_SDO_t **ppSDO);


/*******************************************************************************
   Function: CO_OD_find

   Find object with specific index in Object Dictionary.

   Parameters:
      SDO                        - Pointer to SDO object <CO_SDO_t>.
      index                      - Index of object in object dictionary.
      indexOfFoundObject         - Pointer to externaly defined variable, where
                                   second return value will be written - index of
                                   object in Object Dictionary array. If pointer
                                   is NULL, it will be ignored.

   Return:
      Pointer of type <sCO_OD_object> pointing to found object, NULL if not found.
*******************************************************************************/
const sCO_OD_object* CO_OD_find( CO_SDO_t            *SDO,
                                 UNSIGNED16           index,
                                 UNSIGNED16          *indexOfFoundObject);


/*******************************************************************************
   Function: CO_OD_getLength

   Get length of given object with specific subIndex.

   Parameters:
      object      - Pointer to object returned by <CO_OD_find>.
      subIndex    - Sub-index of object in object dictionary.

   Return:
      Data length of the variable.
*******************************************************************************/
UNSIGNED16 CO_OD_getLength(const sCO_OD_object* object, UNSIGNED8 subIndex);


/*******************************************************************************
   Function: CO_OD_getAttribute

   Get attribute of given object with specific subIndex.

   If Object Type is array and subIndex is zero, functon always returns
   'read-only' attribute.

   Parameters:
      object      - Pointer to object returned by <CO_OD_find>.
      subIndex    - Sub-index of object in object dictionary.

   Return:
      Attribute of the variable.
*******************************************************************************/
UNSIGNED8 CO_OD_getAttribute(const sCO_OD_object* object, UNSIGNED8 subIndex);


/*******************************************************************************
   Function: CO_OD_getDataPointer

   Get pointer to data of given object with specific subIndex.

   If Object Type is array and subIndex is zero, functon returns pointer to
   object->maxSubIndex variable.

   Parameters:
      object      - Pointer to object returned by <CO_OD_find>.
      subIndex    - Sub-index of object in object dictionary.

   Return:
      Pointer to data of the variable.
*******************************************************************************/
const void* CO_OD_getDataPointer(const sCO_OD_object* object, UNSIGNED8 subIndex);


/*******************************************************************************
   Function: CO_SDO_process

   Process SDO communication.

   Function must be called cyclically.

   Parameters:
      SDO                      - Pointer to SDO object <CO_SDO_t>.
      NMTisPreOrOperational    - Different than zero, if <NMT internal State> is
                                 NMT_PRE_OPERATIONAL or NMT_OPERATIONAL.
      timeDifference_ms        - Time difference from previous function call in [milliseconds].
      SDOtimeoutTime           - Timeout time for SDO communication in milliseconds.
*******************************************************************************/
void CO_SDO_process( CO_SDO_t            *SDO,
                     UNSIGNED8            NMTisPreOrOperational,
                     UNSIGNED16           timeDifference_ms,
                     UNSIGNED16           SDOtimeoutTime);


#endif
