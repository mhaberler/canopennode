/*******************************************************************************

   File: CO_SDO.h
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

#ifndef _CO_SDO_H
#define _CO_SDO_H


/*******************************************************************************
   Topic: SDO protocol

   CANopen protocol for Service Data Objects (SDO).

   Service data objects (SDOs) allow the access to any entry of the CANopen
   Object dictionary. An SDO establishes a peer-to-peer communication channel
   between two devices. In addition, the SDO protocol enables to transfer any
   amount of data in a segmented way. Therefore the SDO protocol is mainly used
   in order to communicate configuration data.

   All CANopen devices must have implemented SDO server and first SDO server
   channel. Servers serves data from <Object dictionary>. <Object dictionary>
   is a collection of variables, arrays or records (structures), which can be
   used by the stack or by the application. This file (<CO_SDO.h>) implements
   SDO server.

   SDO client can be (optionally) implemented on one (or multiple, if multiple
   SDO channels are used) device in CANopen network. Usually this is master
   device and provides also some kind of user interface, so configuration of
   the network is possible. Code for the SDO client is in file <CO_SDOmaster.h>.

   SDO communication cycle is initiated by client. Client can upload (read) data
   from device or can download (write) data to device. If data are less or equal
   of 4 bytes long, communication is finished by one server response (expedited
   transfer). If data are longer, they are split into multiple segments of
   request/response pairs (normal or segmented transfer). For longer data there
   is also a block transfer protocol, which transfers larger block of data in
   secure way with little protocol overhead. If error occurs during SDO transfer
   <SDO abort code> is send by client or server and transfer is terminated.


********************************************************************************
   Topic: SDO message contents

   Excerpt from CiA DS301, V4.2

   For CAN identifier see <Default COB identifiers>.

   Expedited transfer is used for transmission of up to 4 data bytes. It consists
   of one SDO request and one response. For longer variables is used segmented
   or block transfer.

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

   Block transfer:
      See DS301 V4.2.


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
   0x06020000  - Object does not exist in the Object dictionary.
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
   0x08000023  - Object dictionary dynamic generation fails or no Object dictionary is present (e.g. object
                 dictionary  is generated from file and generation fails because of an file error).
   0x08000024  - No data available.


********************************************************************************
   Topic: Object dictionary

   CANopen Object dictionary implementation in CANopenNode.

   CANopen Object dictionary is a collection of different data items, which can
   be used by the stack or by the application.

   Each Object dictionary entry is located under 16-bit index, as specified
   by the CANopen:
      0x0001..0x025F - Data type definitions.
      0x1000..0x1FFF - Communication profile area.
      0x2000..0x5FFF - Manufacturer-specific profile area.
      0x6000..0x9FFF - Standardized device profile area for eight logical devices.
      0xA000..0xAFFF - Standardized network variable area.
      0xB000..0xBFFF - Standardized system variable area.
      Other          - Reserved.

   If Object dictionary entry has complex data type (array or structure),
   then 8-bit subIndex specifies the sub-member of the entry. In that case
   subIndex 0x00 is encoded as UNSIGNED8 and specifies the highest available
   subIndex with that entry. Subindex 0xFF has special meaning in the standard
   and is not supported by CANopenNode.

   Object type of one Object dictionary entry can be:
      NULL           - Not used by CANopenNode.
      DOMAIN         - Block of data of variable length. Data and length are
                       under control of the application.
      DEFTYPE        - Definition of <CANopen basic data type>, for example
                       INTEGER16.
      DEFSTRUCT      - Definition of complex data type - structure, which is
                       used with RECORD.
      VAR            - Variable of <CANopen basic data type>. Subindex is 0.
      ARRAY          - Array of multiple variables of the same <CANopen basic
                       data type>. Subindex 1..arrayLength specifies sub-member.
      RECORD         - Record or structure of multiple variables of different
                       <CANopen basic data type>. Subindex specifies sub-member.


   Object dictionary in CANopenNode is implemeted in CO_OD.h and CO_OD.c files.
   These files are application specific and must be generated by Object
   dictionary editor (application is included by the stack).

   CO_OD.h and CO_OD.c files include:
    - Structure definitions for records.
    - Global declaration and initialization of all variables, arrays and records
      mapped to Object dictionary. Variables are distrbuted in multiple objects,
      depending on memory location. This eases storage to different memories in
      microcontroller, like eeprom or flash.
    - Constant array of multiple Object dictionary entries of type
      <CO_OD_entry_t>. If object type is record, then entry includes additional
      constant array with members of type <CO_OD_entryRecord_t>. Each OD entry
      includes information: index, maxSubIndex, <OD attributes>, data size and
      pointer to variable.


   Function <CO_SDO_init> initializes object <CO_SDO_t>, which includes SDO
   server and Object dictionary. Interface to Object dictionary is provided by
   following functions: <CO_OD_find> finds OD entry by index, <CO_OD_getLength>
   returns length of variable, <CO_OD_getAttribute> returns attribute and
   <CO_OD_getDataPointer> returns pointer to data. These functions are used by
   SDO server and by PDO configuration.

   Application doesn't need to know anything about the Object dictionary. It can
   use variables specified in CO_OD.h file directly. If it needs more control
   over the CANopen communication with the variables, it can configure additional
   functionality with function <CO_OD_configure>. Additional functionality
   include: <Object dictionary function> and <Object dictionary flags>.


********************************************************************************
   Topic: Object dictionary function

   Optional application specific function, which may manipulate data downloaded
   or uploaded via SDO.

   Object dictionary function is external function defined by application or
   by other stack files. It may be registered for specific Object dictionary
   entry (with specific index). If it is registered, it is called (through
   function pointer) from SDO server. It may verify and manipulate data during
   SDO transfer. Object dictionary function can be registered by function
   <CO_OD_configure>.

   SDO download (writing to Object dictionary):
      After SDO client transfers data to the server, data are stored in internal
      buffer. If data contains multibyte variable and processor is big endian,
      then data bytes are swapped. Object dictionary function is called if
      registered. Data may be verified and manipulated inside that function. After
      function exits, data are copied to location as specified in <CO_OD_entry_t>.

   SDO upload (reading from Object dictionary):
      Before start of SDO upload, data are read from Object dictionary into
      internal buffer. If necessary, bytes are swapped.
      Object dictionary function is called if registered. Data may be
      manipulated inside that function. After function exits, data are
      transfered via SDO server.

   Domain data type:
      If data type is domain, then length is not specified by Object dictionary.
      In that case Object dictionary function must be used. In case of
      download it must store the data in own location. In case of upload it must
      write the data (maximum size is specified by length) into data buffer and
      specify actual length. With domain data type it is possible to transfer
      data, which are longer than <SDO buffer size>. In that case
      Object dictionary function is called multiple times between SDO transfer.

   Parameter:
      ODF_arg     - Pointer to <CO_ODF_arg_t> object filled before function call.

   Return:
      0                 - Data transfer is successful.
      different than 0  - Failure. See <SDO abort code>.

*******************************************************************************/


/*******************************************************************************
   Object: CO_OD_entry_t

   Object for one entry with specific index in <Object dictionary>.

   Variables:
      index       - The index of Object from 0x1000 to 0xFFFF.
      maxSubIndex - It is number of (sub-objects - 1). If Object Type is
                    variable, then maxSubIndex is 0, otherwise maxSubIndex is
                    equal or greater than 1.
      attribute   - If Object Type is record, attribute is set to zero. Attribute
                    for each member is then set in special array with members of
                    type <CO_OD_entryRecord_t>. If Object Type is Array, attribute
                    is common for all array members. See <OD attributes>.
      length      - If Object Type is Variable, length is the length of variable in bytes.
                    If Object Type is Array, length is the length of one array member.
                    If Object Type is Record, length is zero. Length for each member is
                    set in special array with members of type <CO_OD_entryRecord_t>.
                    If Object Type is Domain, length is zero. Length is specified
                    by application in <Object dictionary function>.
      pData       - If Object Type is Variable, pData is pointer to data.
                    If Object Type is Array, pData is pointer to data. Data doesn't
                    include Sub-Object 0.
                    If object type is Record, pData is pointer to special array
                    with members of type <CO_OD_entryRecord_t>.
                    If object type is Domain, pData is null.
*******************************************************************************/
typedef struct {
   UNSIGNED16     index;
   UNSIGNED8      maxSubIndex;
   UNSIGNED16     attribute;
   UNSIGNED16     length;
   void          *pData;
}CO_OD_entry_t;


/*******************************************************************************
   Object: CO_OD_entryRecord_t

   Object for record type entry in <Object dictionary>.

   See <CO_OD_entry_t>.

   Variables:
      attribute - See <OD attributes>.
      length    - Length of variable in bytes. If object type is Domain, length is zero.
      pData     - Pointer to data. If object type is Domain, pData is null.
*******************************************************************************/
typedef struct{
   void          *pData;
   UNSIGNED16     attribute;
   UNSIGNED16     length;
}CO_OD_entryRecord_t;


/*******************************************************************************
   Constants: OD attributes

   Bit masks for attribute in <CO_OD_entry_t>.

   CO_ODA_MEM_ROM          - (0x0001) - Variable is located in ROM memory.
   CO_ODA_MEM_RAM          - (0x0002) - Variable is located in RAM memory.
   CO_ODA_MEM_EEPROM       - (0x0003) - Variable is located in EEPROM memory.
   CO_ODA_READABLE         - (0x0004) - SDO server may read from the variable.
   CO_ODA_WRITEABLE        - (0x0008) - SDO server may write to the variable.
   CO_ODA_RPDO_MAPABLE     - (0x0010) - Variable is mappable for RPDO.
   CO_ODA_TPDO_MAPABLE     - (0x0020) - Variable is mappable for TPDO.
   CO_ODA_TPDO_DETECT_COS  - (0x0040) - If variable is mapped to any PDO, then
                                        PDO is automatically send, if variable
                                        changes its value.
   CO_ODA_MB_VALUE         - (0x0080) - True when variable is a multibyte value.
*******************************************************************************/
   #define CO_ODA_MEM_ROM          0x0001
   #define CO_ODA_MEM_RAM          0x0002
   #define CO_ODA_MEM_EEPROM       0x0003
   #define CO_ODA_READABLE         0x0004
   #define CO_ODA_WRITEABLE        0x0008
   #define CO_ODA_RPDO_MAPABLE     0x0010
   #define CO_ODA_TPDO_MAPABLE     0x0020
   #define CO_ODA_TPDO_DETECT_COS  0x0040
   #define CO_ODA_MB_VALUE         0x0080


/*******************************************************************************
   Constants: Object dictionary flags

   Bit masks for flags associated with variable from <Object dictionary>.

   This additional functionality of any variable in <Object dictionary> can be
   enabled by function <CO_OD_configure>. Location of the flag byte can be
   get from function <CO_OD_getFlagsPointer>.

   CO_ODFL_RPDO_WRITTEN    - (0x01) - Variable was written by RPDO. Flag can
                                      be cleared by application.
   CO_ODFL_TPDO_MAPPED     - (0x02) - Variable is mapped to TPDO.
   CO_ODFL_TPDO_COS_ENABLE - (0x03) - Change of state bit, initially copy of
                                      attribute from <CO_OD_entry_t>. If set
                                      and variable is mapped to TPDO, TPDO
                                      will be automatically send, if variable
                                      changed.
   CO_ODFL_TPDO_SEND       - (0x04) - PDO send bit, can be set by application.
                                      If variable is mapped into TPDO, TPDO
                                      will be send and bit will be cleared.
   CO_ODFL_SDO_DOWNLOADED  - (0x08) - Variable was accessed by SDO download.
   CO_ODFL_SDO_UPLOADED    - (0x10) - Variable was accessed by SDO upload.
   CO_ODFL_BIT_6           - (0x20) - Reserved.
   CO_ODFL_BIT_7           - (0x40) - Reserved.
*******************************************************************************/
   #define CO_ODFL_RPDO_WRITTEN    0x01
   #define CO_ODFL_TPDO_MAPPED     0x02
   #define CO_ODFL_TPDO_COS_ENABLE 0x04
   #define CO_ODFL_TPDO_SEND       0x08
   #define CO_ODFL_SDO_DOWNLOADED  0x10
   #define CO_ODFL_SDO_UPLOADED    0x20
   #define CO_ODFL_BIT_6           0x40
   #define CO_ODFL_BIT_7           0x80


/*******************************************************************************
   Object: CO_ODF_arg_t

   Object contains all information about the object being transfered by SDO server.

   Object is used as an argument to <Object dictionary function>. It is also
   part of the <CO_SDO_t> object.

   Variables:
      object      - Informative parameter, which may point to object, which is
                    connected with this OD entry. It can be used inside
                    <Object dictionary function>, ONLY if it was registered by
                    <CO_OD_configure> function before.
      data        - SDO data buffer contains data, which are exchanged in SDO
                    transfer. <Object dictionary function> may verify or
                    manupulate that data before (after) they are written to
                    (read from) Object dictionary. Data have the same endianes
                    as processor. Pointer must NOT be changed. (Data up to
                    length can be changed.)
      ODdataStorage - Pointer to location in object dictionary, where data are
                    stored. (informative refference to old data, read only).
                    Data have the same endianes as processor. If data type is
                    Domain, this variable is null.
      dataLength  - Length of data in the above buffer. Read only, except for
                    domain. If data type is domain see
                    <Object dictionary function> for special rules by upload.
      attribute   - Attribute of object in Object dictionary (informative, must
                    NOT be changed).
      pFlags      - Pointer to the <Object dictionary flags> byte.
      index       - Index of object in Object dictionary (informative, must NOT
                    be changed).
      subIndex    - Subindex of object in Object dictionary (informative, must
                    NOT be changed).
      reading     - True, if SDO upload is in progress, false if SDO download
                    is in progress.
      firstSegment - Used by domain data type. Indicates the first segment.
                    Variable is informative.
      lastSegment - Used by domain data type. If false by download,
                    then application will receive more segments during SDO
                    communication cycle. If uploading, application may set
                    variable to false, so SDO server will call
                    <Object dictionary function> again for filling the next data.
      dataLengthTotal - Used by domain data type. By upload
                    <Object dictionary function> may write total data length, so
                    this information will be send in SDO upload initiate phase.
                    It is not necessary to specify this variable. By download
                    this variable contains total data size, if size is indicated
                    in SDO download initiate phase.
*******************************************************************************/
typedef struct{
   void                   *object;
   UNSIGNED8              *data;
   const void             *ODdataStorage;
   UNSIGNED16              dataLength;
   UNSIGNED16              attribute;
   UNSIGNED8              *pFlags;
   UNSIGNED16              index;
   UNSIGNED8               subIndex;
   UNSIGNED8               reading;
   UNSIGNED8               firstSegment;
   UNSIGNED8               lastSegment;
   UNSIGNED32              dataLengthTotal;
}CO_ODF_arg_t;


/*******************************************************************************
   Object: CO_OD_extension_t

   Object is used as array inside <CO_SDO_t>, parallel to <Object dictionary>.

   Object is generated by function <CO_OD_configure>. It is then used as
   extension to Object dictionary entry at specific index.

   Variables:
      pODFunc     - Pointer to <Object dictionary function>.
      object      - Pointer to object, which will be passed to
                    <Object dictionary function>.
      flags       - Array of <Object dictionary flags>. If object type is array
                    or record, length of flags[] is larger than 1.
*******************************************************************************/
typedef struct{
   UNSIGNED32   (*pODFunc)(CO_ODF_arg_t *ODF_arg);
   void          *object;
   UNSIGNED8      flags[1];
}CO_OD_extension_t;


/*******************************************************************************
   Constant: SDO buffer size

   Size of the internal SDO buffer.

   Size must be at least equal to size of largest variable in <Object dictionary>.
   If data type is domain, data length is not limited to SDO buffer size. If
   block transfer is implemeted, value should be set to 889.

   CO_SDO_BUFFER_SIZE   - Value can be in range from 7 to 889 bytes.
*******************************************************************************/
   #ifndef CO_SDO_BUFFER_SIZE
      #define CO_SDO_BUFFER_SIZE    32
   #endif


/*******************************************************************************
   Object: CO_SDO_t

   Object controls the SDO server.

   Variables:
      ownOD             - Internal flag indicates, that this object has own OD.
      OD                - Pointer to the <Object dictionary>.
      ODSize            - Size of the <Object dictionary>.
      ODExtensions      - Pointer to array of pointers to <CO_OD_extension_t>
                          object. Separate objects are generated dynamically
                          (malloc) inside <CO_OD_configure>.

      databuffer        - SDO data buffer of size <SDO buffer size>.
      bufferOffset      - Offset in buffer of next data segment being read/written.

      entryNo           - Sequence number of OD entry as returned from <CO_OD_find>.
      ODF_arg           - <CO_ODF_arg_t> object with additional variables. Reference
                          to this object is passed to <Object dictionary function>.

      nodeId            - CANopen Node ID of this device.
      state             - Current internal state of the SDO server state machine.
      sequence          - Toggle bit in segmented transfer or block sequence in
                          block transfer.
      timeoutTimer      - Timeout timer for SDO communication.

      blksize           - Number of segments per block with 1 <= blksize <= 127.
      crcEnabled        - True, if CRC calculation by block transfer is enabled.
      crc               - Calculated CRC code.
      lastLen           - Length of data in the last segment in block upload.
      endOfTransfer     - Indication end of block transfer.

      CANrxData         - 8 data bytes of the received message.
      CANrxNew          - Variable indicates, if new SDO message received from CAN bus.

      CANdevTx          - Pointer to CAN device used for SDO server transmission <CO_CANmodule_t>.
      CANtxBuff         - CAN transmit buffer inside CANdev for CAN tx message.
*******************************************************************************/
typedef struct{
   UNSIGNED8                     CANrxData[8];   //take care for correct (word) alignment!
   UNSIGNED8                     databuffer[CO_SDO_BUFFER_SIZE];  //take care for correct (word) alignment!

   UNSIGNED8                     ownOD;
   const CO_OD_entry_t          *OD;
   UNSIGNED16                    ODSize;
   CO_OD_extension_t           **ODExtensions;

   UNSIGNED16                    bufferOffset;

   UNSIGNED16                    entryNo;
   CO_ODF_arg_t                  ODF_arg;

   UNSIGNED8                     nodeId;
   UNSIGNED8                     state;
   UNSIGNED8                     sequence;
   UNSIGNED16                    timeoutTimer;

   UNSIGNED8                     blksize;
   UNSIGNED8                     crcEnabled;
   UNSIGNED16                    crc;
   UNSIGNED8                     lastLen;
   UNSIGNED8                     endOfTransfer;

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
   Function: CO_ODF_1200

   Function for accessing _SDO server parameter_ (index 0x1200+) from SDO server.

   For more information see topic <Object dictionary function>.
*******************************************************************************/
UNSIGNED32 CO_ODF_1200(CO_ODF_arg_t *ODF_arg);


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
      ObjDictIndex_SDOServerParameter - Index in Object dictionary.

      parentSDO                  - Pointer to SDO object, which contains object
                                   dictionary and its extension. For first (default)
                                   SDO object this argument must be NULL. If this
                                   argument is specified, then OD and ODSize
                                   arguments are ignored.
      OD                         - Pointer to <Object dictionary>.
      ODSize                     - Size of <Object dictionary>.
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
      CO_SDO_t                     *parentSDO,
const CO_OD_entry_t                *OD,
      UNSIGNED16                    ODSize,
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
   Function: CO_SDO_process

   Process SDO communication.

   Function must be called cyclically.

   Parameters:
      SDO                      - Pointer to SDO object <CO_SDO_t>.
      NMTisPreOrOperational    - Different than zero, if <NMT internal State> is
                                 NMT_PRE_OPERATIONAL or NMT_OPERATIONAL.
      timeDifference_ms        - Time difference from previous function call in [milliseconds].
      SDOtimeoutTime           - Timeout time for SDO communication in milliseconds.

   Return:
      0 - SDO server is iddle.
      1 - SDO server is in transfer state.
     -1 - SDO abort just occured.
*******************************************************************************/
INTEGER8 CO_SDO_process(CO_SDO_t         *SDO,
                        UNSIGNED8         NMTisPreOrOperational,
                        UNSIGNED16        timeDifference_ms,
                        UNSIGNED16        SDOtimeoutTime);


/*******************************************************************************
   Function: CO_OD_configure

   Configure additional functionality to one <Object dictionary> entry.

   Additional functionality include: <Object dictionary function> and
   <Object dictionary flags>. First call to this function allocates memory for
   'ODExtensions' in <CO_SDO_t> object. Then it finds <Object dictionary> entry
   specified with index. In corresponding position in 'ODExtensions' function
   then allocates <CO_OD_extension_t> object and initializes it.

   Parameters:
      SDO      - Pointer to SDO object <CO_SDO_t>.
      index    - Index of object in the Object dictionary.
      pODFunc  - Pointer to <Object dictionary function>, specified by application.
      object   - Pointer to object, which will be passed to
                 <Object dictionary function>.

   Return:
      Sequence number of the <Object dictionary> entry, 0xFFFF if not found.
      Value is the same as in <CO_OD_find>. If memory allocation fails, function
      also returns 0xFFFF.
*******************************************************************************/
UNSIGNED16 CO_OD_configure(CO_SDO_t      *SDO,
                           UNSIGNED16     index,
                           UNSIGNED32   (*pODFunc)(CO_ODF_arg_t *ODF_arg),
                           void          *object);


/*******************************************************************************
   Function: CO_OD_find

   Find object with specific index in Object dictionary.

   Parameters:
      SDO      - Pointer to SDO object <CO_SDO_t>.
      index    - Index of the object in Object dictionary.

   Return:
      Sequence number of the <Object dictionary> entry, 0xFFFF if not found.
*******************************************************************************/
UNSIGNED16 CO_OD_find(CO_SDO_t *SDO, UNSIGNED16 index);


/*******************************************************************************
   Function: CO_OD_getLength

   Get length of the given object with specific subIndex.

   Parameters:
      SDO         - Pointer to SDO object <CO_SDO_t>.
      entryNo     - Sequence number of OD entry as returned from <CO_OD_find>.
      subIndex    - Sub-index of the object in Object dictionary.

   Return:
      Data length of the variable.
*******************************************************************************/
UNSIGNED16 CO_OD_getLength(CO_SDO_t *SDO, UNSIGNED16 entryNo, UNSIGNED8 subIndex);


/*******************************************************************************
   Function: CO_OD_getAttribute

   Get attribute of the given object with specific subIndex. See <OD attributes>.

   If Object Type is array and subIndex is zero, functon always returns
   'read-only' attribute.

   Parameters:
      SDO         - Pointer to SDO object <CO_SDO_t>.
      entryNo     - Sequence number of OD entry as returned from <CO_OD_find>.
      subIndex    - Sub-index of the object in Object dictionary.

   Return:
      Attribute of the variable.
*******************************************************************************/
UNSIGNED16 CO_OD_getAttribute(CO_SDO_t *SDO, UNSIGNED16 entryNo, UNSIGNED8 subIndex);


/*******************************************************************************
   Function: CO_OD_getDataPointer

   Get pointer to data of the given object with specific subIndex.

   If Object Type is array and subIndex is zero, functon returns pointer to
   object->maxSubIndex variable.

   Parameters:
      SDO         - Pointer to SDO object <CO_SDO_t>.
      entryNo     - Sequence number of OD entry as returned from <CO_OD_find>.
      subIndex    - Sub-index of the object in Object dictionary.

   Return:
      Pointer to the variable in <Object dictionary>.
*******************************************************************************/
void* CO_OD_getDataPointer(CO_SDO_t *SDO, UNSIGNED16 entryNo, UNSIGNED8 subIndex);


/*******************************************************************************
   Function: CO_OD_getFlagsPointer

   Get pointer to the <Object dictionary flags> byte of the given object with
   specific subIndex.

   Parameters:
      SDO         - Pointer to SDO object <CO_SDO_t>.
      entryNo     - Sequence number of OD entry as returned from <CO_OD_find>.
      subIndex    - Sub-index of the object in Object dictionary.

   Return:
      Pointer to the <Object dictionary flags> of the variable.
*******************************************************************************/
UNSIGNED8* CO_OD_getFlagsPointer(CO_SDO_t *SDO, UNSIGNED16 entryNo, UNSIGNED8 subIndex);


/*******************************************************************************
   Function: CO_SDO_initTransfer

   Initialize SDO transfer.

   Find object in OD, verify, fill ODF_arg s.

   Parameters:
      SDO      - Pointer to SDO object <CO_SDO_t>.
      index    - Index of the object in Object dictionary.
      subIndex - subIndex of the object in Object dictionary.

   Return:
      0 on success, otherwise <SDO abort code>.
*******************************************************************************/
UNSIGNED32 CO_SDO_initTransfer(CO_SDO_t *SDO, UNSIGNED16 index, UNSIGNED8 subIndex);


/*******************************************************************************
   Function: CO_SDO_readOD

   Read data from <Object dictionary> to internal buffer.

   ODF_arg s must be initialized before with <CO_SDO_initTransfer>.
   <Object dictionary function> is called if configured.

   Parameters:
      SDO            - Pointer to SDO object <CO_SDO_t>.
      SDOBufferSize  - Total size of the SDO buffer.

   Return:
      0 on success, otherwise <SDO abort code>.
*******************************************************************************/
UNSIGNED32 CO_SDO_readOD(CO_SDO_t *SDO, UNSIGNED16 SDOBufferSize);


/*******************************************************************************
   Function: CO_SDO_writeOD

   Write data from internal buffer to <Object dictionary>.

   ODF_arg s must be initialized before with <CO_SDO_initTransfer>.
   <Object dictionary function> is called if configured.

   Parameters:
      SDO      - Pointer to SDO object <CO_SDO_t>.
      length   - Length of data (received from network) to write.

   Return:
      0 on success, otherwise <SDO abort code>.
*******************************************************************************/
UNSIGNED32 CO_SDO_writeOD(CO_SDO_t *SDO, UNSIGNED16 length);


#endif
