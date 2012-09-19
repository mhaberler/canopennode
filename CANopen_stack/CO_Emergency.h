/*******************************************************************************

   File: CO_Emergency.h
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

#ifndef _CO_EMERGENCY_H
#define _CO_EMERGENCY_H


/*******************************************************************************
   Topic: Usage

   Emergency object is used for control internal error state and for sending
   Emergency message.

   Each error condition, which may hapen somewhere in program, has assigned its
   specific index (see <Error status bits>). Whenever error occures,
   <CO_errorReport()> function is called with this index (and some extra
   information). Function then sets internal error flag bit and sends CANopen
   Emergency message.

   Specific error condition is reported only the first time. Specific error flag
   bit can also be reset by <CO_errorReset()> function.

   Latest errors can be read from 'Pre Defined Error Field' (object dictionary,
   index 0x1003).


********************************************************************************
   Topic: Emergency message

   Contents of Emergency message (COB-ID = 0x80 + Node-ID).

   For CAN identifier see <Default COB identifiers>.

   Specific data bytes:
      byte 0..1   - <Error code>
      byte 2      - <Error register>
      byte 3      - Index of error condition (see <Error status bits>).
      byte 4..7   - Additional argument to <CO_errorReport()> function.

   Contents of _Pre Defined Error Field_ (object dictionary, index 0x1003):
      byte 0..3   - Equal to bytes 0..3 in the Emergency message


********************************************************************************
   Topic: Error code

   Error codes According to CiA ds-301:
      00xx        - Error Reset or No Error
      10xx        - Generic Error
      20xx        - Current
      21xx        - Current, device input side
      22xx        - Current inside the device
      23xx        - Current, device output side
      30xx        - Voltage
      31xx        - Mains Voltage
      32xx        - Voltage inside the device
      33xx        - Output Voltage
      40xx        - Temperature
      41xx        - Ambient Temperature
      42xx        - Device Temperature
      50xx        - Device Hardware
      60xx        - Device Software
      61xx        - Internal Software
      62xx        - User Software
      63xx        - Data Set
      70xx        - Additional Modules
      80xx        - Monitoring
      81xx        - Communication
      8110        - CAN Overrun (Objects lost)
      8120        - CAN in Error Passive Mode
      8130        - Life Guard Error or Heartbeat Error
      8140        - recovered from bus off
      8150        - CAN-ID collision
      82xx        - Protocol Error
      8210        - PDO not processed due to length error
      8220        - PDO length exceeded
      8230        - DAM MPDO not processed, destination object not available
      8240        - Unexpected SYNC data length
      8250        - RPDO timeout
      90xx        - External Error
      F0xx        - Additional Functions
      FFxx        - Device specific

   Error codes According to CiA ds-401:
      2310        - Current at outputs too high (overload)
      2320        - Short circuit at outputs
      2330        - Load dump at outputs
      3110        - Input voltage too high
      3120        - Input voltage too low
      3210        - Internal voltage too high
      3220        - Internal voltage too low
      3310        - Output voltage too high
      3320        - Output voltage too low


********************************************************************************
   Topic: Error register

   Object dictionary, index 0x1001.

   Error register is calculated from critical internal <Error status bits>.
   Generic and communication bits are calculated in <CO_emergency_process>
   function, device profile or manufacturer specific bits may be calculated
   inside application.

   Internal errors may prevent device to stay in NMT Operational state. Details
   are described in _Error Behavior_ object in Object Dictionary at index 0x1029.

   Specific bits:
      bit 0       - generic error
      bit 1       - current
      bit 2       - voltage
      bit 3       - temperature
      bit 4       - communication error (overrun, error state)
      bit 5       - device profile specific
      bit 6       - Reserved (always 0)
      bit 7       - manufacturer specific
*******************************************************************************/


/*******************************************************************************
   Constants: Error status bits

   Indexes and <Error code>s of specific errors. Constants are used with
   <CO_errorReport()> or <CO_errorReset()> functions.

   <CO_errorReport()> sets specific bit in variable _errorStatusBits_ from
   <CO_emergencyReport_t> object. Type of variable _errorStatusBits_ is string with
   lenght 10 unsigned characters (by default). That means, variable has 80 error
   bits (form 0x00 to 0x4F). Maximum length of variable is 32 bytes, that is
   255 error bits.

   First value is unique ID for emergency message, second value is <Error Code>
   as specified in standard.

   Internal errors may prevent device to stay in NMT Operational state. Details
   are described in _Error Behavior_ object in Object Dictionary at index 0x1029.

   Communication or protocol errors (informative):
      ERROR_NO_ERROR                         - 00, 0x0000 - Error Reset or No Error.
      ERROR_CAN_BUS_WARNING                  - 01, 0x0000 - CAN bus warning.
      ERROR_RXMSG_WRONG_LENGTH               - 02, 0x8200 - Wrong data length of received CAN message.
      ERROR_RXMSG_OVERFLOW                   - 03, 0x8200 - Previous received CAN message wasn't processed yet.
      ERROR_RPDO_WRONG_LENGTH                - 04, 0x8210 - Wrong data length of received PDO.
      ERROR_RPDO_OVERFLOW                    - 05, 0x8200 - Previous received PDO wasn't processed yet.
      ERROR_CAN_RX_BUS_PASSIVE               - 06, 0x8120 - CAN receive bus is passive.
      ERROR_CAN_TX_BUS_PASSIVE               - 07, 0x8120 - CAN transmit bus is passive.
      ERROR_NMT_WRONG_COMMAND                - 08, 0x8200 - Wrong NMT command received.
      ERROR_09_unused                        - 09, 0x1000 - (unused)
      ERROR_0A_unused                        - 0A, 0x1000 - (unused)
      ERROR_0B_unused                        - 0B, 0x1000 - (unused)
      ERROR_0C_unused                        - 0C, 0x1000 - (unused)
      ERROR_0D_unused                        - 0D, 0x1000 - (unused)
      ERROR_0E_unused                        - 0E, 0x1000 - (unused)
      ERROR_0F_unused                        - 0F, 0x1000 - (unused)

   Communication or protocol errors (critical):
      ERROR_10_unused                        - 10, 0x1000 - (unused)
      ERROR_11_unused                        - 11, 0x1000 - (unused)
      ERROR_CAN_TX_BUS_OFF                   - 12, 0x8140 - CAN transmit bus is off.
      ERROR_CAN_RXB_OVERFLOW                 - 13, 0x8110 - CAN module receive buffer has overflowed.
      ERROR_CAN_TX_OVERFLOW                  - 14, 0x8110 - CAN transmit buffer has overflowed.
      ERROR_TPDO_OUTSIDE_WINDOW              - 15, 0x8100 - TPDO is outside SYNC window.
      ERROR_16_unused                        - 16, 0x1000 - (unused)
      ERROR_17_unused                        - 17, 0x1000 - (unused)
      ERROR_SYNC_TIME_OUT                    - 18, 0x8100 - SYNC message timeout.
      ERROR_SYNC_LENGTH                      - 19, 0x8240 - Unexpected SYNC data length
      ERROR_PDO_WRONG_MAPPING                - 1A, 0x8200 - Error with PDO mapping.
      ERROR_HEARTBEAT_CONSUMER               - 1B, 0x8130 - Heartbeat consumer timeout.
      ERROR_HEARTBEAT_CONSUMER_REMOTE_RESET  - 1C, 0x8130 - Heartbeat consumer detected remote node reset.
      ERROR_1D_unused                        - 1D, 0x1000 - (unused)
      ERROR_1E_unused                        - 1E, 0x1000 - (unused)
      ERROR_1F_unused                        - 1F, 0x1000 - (unused)

   Generic errors (informative):
      ERROR_EMERGENCY_BUFFER_FULL            - 20, 0x1000 - Emergency buffer is full, Emergency message wasn't sent.
      ERROR_ERROR_REPORT_BUSY                - 21, 0x1000 - <CO_errorReport> is busy, Emergency message wasn't sent.
      ERROR_MICROCONTROLLER_RESET            - 22, 0x1000 - Microcontroller has just started.
      ERROR_23_unused                        - 23, 0x1000 - (unused)
      ERROR_24_unused                        - 24, 0x1000 - (unused)
      ERROR_25_unused                        - 25, 0x1000 - (unused)
      ERROR_26_unused                        - 26, 0x1000 - (unused)
      ERROR_27_unused                        - 27, 0x1000 - (unused)

   Generic errors (critical):
      ERROR_WRONG_ERROR_REPORT               - 28, 0x6100 - Wrong parameters to <CO_errorReport()> function.
      ERROR_ISR_TIMER_OVERFLOW               - 29, 0x6100 - Timer task has overflowed.
      ERROR_MEMORY_ALLOCATION_ERROR          - 2A, 0x6100 - Unable to allocate memory for objects.
      ERROR_GENERIC_ERROR                    - 2B, 0x1000 - Generic error, test usage.
      ERROR_MAIN_TIMER_OVERFLOW              - 2C, 0x6100 - Mainline function exceeded maximum execution time.
      ERROR_INCONSISTENT_OBJECT_DICTIONARY   - 2D, 0x6100 - Object dictionary does not match the software.
      ERROR_CALCULATION_OF_PARAMETERS        - 2E, 0x1000 - Error in calculation of device parameters.
      ERROR_NON_VOLATILE_MEMORY              - 2F, 0x5000 - Error with access to non volatile device memory

   Manufacturer specific errors:
      Manufacturer may define its own constants up to index 0xFF. Of course, he
      must then define large enough buffer for error status bits (up to 32 bytes).
*******************************************************************************/
   //Communication or protocol errors (informative)
   #define ERROR_NO_ERROR                             0x00, 0x0000
   #define ERROR_CAN_BUS_WARNING                      0x01, 0x0000
   #define ERROR_RXMSG_WRONG_LENGTH                   0x02, 0x8200//?
   #define ERROR_RXMSG_OVERFLOW                       0x03, 0x8200//?
   #define ERROR_RPDO_WRONG_LENGTH                    0x04, 0x8210//?
   #define ERROR_RPDO_OVERFLOW                        0x05, 0x8200//?
   #define ERROR_CAN_RX_BUS_PASSIVE                   0x06, 0x8120
   #define ERROR_CAN_TX_BUS_PASSIVE                   0x07, 0x8120
   #define ERROR_NMT_WRONG_COMMAND                    0x08, 0x8200
   #define ERROR_09_unused                            0x09, 0x1000
   #define ERROR_0A_unused                            0x0A, 0x1000
   #define ERROR_0B_unused                            0x0B, 0x1000
   #define ERROR_0C_unused                            0x0C, 0x1000
   #define ERROR_0D_unused                            0x0D, 0x1000
   #define ERROR_0E_unused                            0x0E, 0x1000
   #define ERROR_0F_unused                            0x0F, 0x1000

   //Communication or protocol errors (critical)
   #define ERROR_10_unused                            0x10, 0x1000
   #define ERROR_11_unused                            0x11, 0x1000
   #define ERROR_CAN_TX_BUS_OFF                       0x12, 0x8140
   #define ERROR_CAN_RXB_OVERFLOW                     0x13, 0x8110
   #define ERROR_CAN_TX_OVERFLOW                      0x14, 0x8110
   #define ERROR_TPDO_OUTSIDE_WINDOW                  0x15, 0x8100
   #define ERROR_16_unused                            0x16, 0x1000
   #define ERROR_17_unused                            0x17, 0x1000
   #define ERROR_SYNC_TIME_OUT                        0x18, 0x8100
   #define ERROR_SYNC_LENGTH                          0x19, 0x8240
   #define ERROR_PDO_WRONG_MAPPING                    0x1A, 0x8200
   #define ERROR_HEARTBEAT_CONSUMER                   0x1B, 0x8130
   #define ERROR_HEARTBEAT_CONSUMER_REMOTE_RESET      0x1C, 0x8130
   #define ERROR_1D_unused                            0x1D, 0x1000
   #define ERROR_1E_unused                            0x1E, 0x1000
   #define ERROR_1F_unused                            0x1F, 0x1000

   //Generic errors (informative)
   #define ERROR_EMERGENCY_BUFFER_FULL                0x20, 0x1000
   #define ERROR_ERROR_REPORT_BUSY                    0x21, 0x1000
   #define ERROR_MICROCONTROLLER_RESET                0x22, 0x1000
   #define ERROR_23_unused                            0x23, 0x1000
   #define ERROR_24_unused                            0x24, 0x1000
   #define ERROR_25_unused                            0x25, 0x1000
   #define ERROR_26_unused                            0x26, 0x1000
   #define ERROR_27_unused                            0x27, 0x1000

   //Generic errors (critical)
   #define ERROR_WRONG_ERROR_REPORT                   0x28, 0x6100
   #define ERROR_ISR_TIMER_OVERFLOW                   0x29, 0x6100
   #define ERROR_MEMORY_ALLOCATION_ERROR              0x2A, 0x6100
   #define ERROR_GENERIC_ERROR                        0x2B, 0x1000
   #define ERROR_MAIN_TIMER_OVERFLOW                  0x2C, 0x6100
   #define ERROR_INCONSISTENT_OBJECT_DICTIONARY       0x2D, 0x6100
   #define ERROR_CALCULATION_OF_PARAMETERS            0x2E, 0x6300
   #define ERROR_NON_VOLATILE_MEMORY                  0x2F, 0x5000


/*******************************************************************************
   Object: CO_emergencyReport_t

   Object contains error buffer, to which new emergency messages are written,
   when <CO_errorReport> function is called. This object is in pair with
   <CO_emergencyProcess_t> object.


   Variables:
      errorStatusBits            - See parameters in <CO_emergency_init>.
      errorStatusBitsSize        - See parameters in <CO_emergency_init>.
      msgBuffer                  - Internal buffer for storing unsent emergency messages.
      msgBufferEnd               - End+1 address of the above buffer
      msgBufferWritePtr          - Write pointer in the above buffer.
      msgBufferReadPtr           - Read pointer in the above buffer.
      msgBufferFull              - Above buffer is full.
      wrongErrorReport           - Error in arguments to <CO_errorReport> function.
      errorReportBusy            - Lock mechanism for error report function.
      errorReportBusyError       - Error, higher priority task called <CO_errorReport>
                                   function while it was busy.
*******************************************************************************/
typedef struct{
   UNSIGNED8           *errorStatusBits;
   UNSIGNED8            errorStatusBitsSize;
   UNSIGNED8           *msgBuffer;
   UNSIGNED8           *msgBufferEnd;
   UNSIGNED8           *msgBufferWritePtr;
   UNSIGNED8           *msgBufferReadPtr;
   UNSIGNED8            msgBufferFull;
   UNSIGNED8            wrongErrorReport;
   UNSIGNED8            errorReportBusy;
   UNSIGNED8            errorReportBusyError;
}CO_emergencyReport_t;


/*******************************************************************************
   Function: CO_errorReport

   Report error condition.

   Function is called on any error condition inside CANopen stack and may also
   be called by application on custom error condition. Emergency message is sent
   after the first occurence of specific error. In case of critical error, device
   will not be able to stay in NMT_OPERATIONAL state.

   Function is quite short and may be used form any task or interrupt.

   Parameters:
      EM        - Pointer to Emergency object <CO_emergencyReport_t>.
      errorBit  - One of the <Error status bits> constants.
      errorCode - <Error code>
      infoCode  - 32 bit value is passed to byetes 4...7 of the Emergency message.

   Return:
      -3 - CO_errorReport is busy, message is deleted.
      -2 - Emergency buffer is full, message is deleted.
      -1 - Error in arguments.
      0  - Error was allready present before, no action was performed.
      1  - Error is new, Emergency will be send.
*******************************************************************************/
INTEGER8 CO_errorReport(CO_emergencyReport_t *EM, UNSIGNED8 errorBit, UNSIGNED16 errorCode, UNSIGNED32 infoCode);


/*******************************************************************************
   Function: CO_errorReset

   Resets error condition.

   Function is called if any error condition is solved. Emergency message is sent
   with <Error code> 0x0000.

   Function is quite short and may be used form any task or interrupt.

   Parameters:
      EM       - Pointer to Emergency object <CO_emergencyReport_t>.
      errorBit - One of the <Error status bits> constants.
      errorCode - <Error code>
      infoCode - 32 bit value is passed to byetes 4...7 of the Emergency message.

   Return:
      -3 - CO_errorReport is busy, message is deleted.
      -2 - Emergency buffer is full, message is deleted.
      -1 - Error in arguments.
      0  - Error was not present before, no action was performed.
      1  - Error bit is cleared, Emergency with <Error code> 0 will be send.
*******************************************************************************/
INTEGER8 CO_errorReset(CO_emergencyReport_t *EM, UNSIGNED8 errorBit, UNSIGNED16 errorCode, UNSIGNED32 infoCode);


/*******************************************************************************
   Function: CO_isError

   Check specific error condition.

   Function returns 1, if specific internal error is present. Othervise it returns 0.

   Parameters:
      EM       - Pointer to Emergency object <CO_emergencyReport_t>.
      errorBit - One of the <Error status bits> constants.
      dummy    - For compatibility.

   Return:
      0  - Error is *not* present.
      1  - Error is present.
*******************************************************************************/
UNSIGNED8 CO_isError(CO_emergencyReport_t *EM, UNSIGNED8 errorBit, UNSIGNED16 dummy);


#ifdef _CO_SDO_H


/*******************************************************************************
   Object: CO_emergencyProcess_t

   Object controls internal error state and sends emergency message.

   Variables:
      errorRegister              - See parameters in <CO_emergency_init>.
      preDefinedErrorField       - See parameters in <CO_emergency_init>.
      preDefinedErrorFieldSize   - See parameters in <CO_emergency_init>.
      preDefinedErrorFieldNumberOfErrors - Number of errors stored in
                                   _preDefinedErrorField_ array.

      reportBuffer               - Here new emergencies are written by <CO_errorReport>.

      inhibitEmergencyTimer      - Internal timer for emergency message.

      CANdev                     - Pointer to CAN device used for EM transmission <CO_CANmodule_t>.
      CANtxBuff                  - CAN transmit buffer inside CANdev for emergency message.
*******************************************************************************/
typedef struct{
   UNSIGNED8              *errorRegister;
   UNSIGNED32             *preDefinedErrorField;
   UNSIGNED8               preDefinedErrorFieldSize;
   UNSIGNED8               preDefinedErrorFieldNumberOfErrors;
   UNSIGNED16              inhibitEmergencyTimer;

   CO_emergencyReport_t   *reportBuffer;

   CO_CANmodule_t         *CANdev;
   CO_CANtxArray_t        *CANtxBuff;
}CO_emergencyProcess_t;


/*******************************************************************************
   Function: CO_ODF_1003

   Function for accessing _Pre-Defined Error Field_ (index 0x1003) from SDO server.

   For more information see topic <SDO server access function> in CO_SDO.h file.
*******************************************************************************/
UNSIGNED32 CO_ODF_1003( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED16 *pLength,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData);


/*******************************************************************************
   Function: CO_ODF_1014

   Function for accessing _COB ID EMCY_ (index 0x1014) from SDO server.

   For more information see topic <SDO server access function> in CO_SDO.h file.
*******************************************************************************/
UNSIGNED32 CO_ODF_1014( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED16 *pLength,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData);


/*******************************************************************************
   Function: CO_emergency_init

   Create and initialize Emergency object.

   Function must be called in the communication reset section.

   Parameters:
      ppEmergencyReport        - Pointer to address of emergency object <CO_emergencyReport_t>.
                                 If address is zero, memory for new object will be
                                 allocated and address will be set.
      ppEmergencyProcess       - Pointer to address of emergency object <CO_emergencyProcess_t>.
                                 If address is zero, memory for new object will be
                                 allocated and address will be set.
      SDO                      - Pointer to SDO object <CO_SDO_t>.
      errorStatusBits          - Pointer to externaly defined array for storing
                                 <Error status bits>.
      errorStatusBitsSize      - Size of above array. Must be >= 6.
      errorRegister            - Pointer to <Error register> (object dictionary, index 0x1001).
      preDefinedErrorField     - Pointer to _Pre defined error field_ (object dictionary, index 0x1003).
      preDefinedErrorFieldSize - Size of _Pre defined error field_.
      msgBufferSize            - Size for internal Emergency message buffer.
      CANdev                   - CAN device for Emergency transmission <CO_CANmodule_t>.
      CANdevTxIdx              - Index of transmit buffer pointer for Emergency transmission.
      CANidTxEM                - CAN identifier for Emergency message.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
      CO_ERROR_OUT_OF_MEMORY      - Memory allocation failed.
*******************************************************************************/
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
      CO_CANmodule_t *CANdev, UNSIGNED16 CANdevTxIdx, UNSIGNED16 CANidTxEM);


/*******************************************************************************
   Function: CO_emergency_delete

   Delete Emergency object and free memory.

   Parameters:
      ppEmergencyReport  - Pointer to pointer to Emergency report object
                           <CO_emergencyReport_t>. It is set to 0.
      ppEmergencyProcess - Pointer to pointer to Emergency process object
                           <CO_emergencyProcess_t>. It is set to 0.
*******************************************************************************/
void CO_emergency_delete(
      CO_emergencyReport_t  **ppEmergencyReport,
      CO_emergencyProcess_t **ppEmergencyProcess);


/*******************************************************************************
   Function: CO_emergency_process

   Process Emergency object.

   Function must be called cyclically. It verifies some communication errors,
   calculates bit 0 and bit 4 from <Error register> and sends emergency message
   if necessary.

   Parameters:
      EMpr                    - Pointer to Emergency object <CO_emergencyProcess_t>.
      NMTisPreOrOperational   - Different than zero, if <NMT internal State> is
                                NMT_PRE_OPERATIONAL or NMT_OPERATIONAL.
      timeDifference_100us    - Time difference from previous function call in [100 * microseconds].
      EMinhTime               - _Inhibit time EMCY_ (object dictionary, index 0x1015).
*******************************************************************************/
void CO_emergency_process( CO_emergencyProcess_t  *EMpr,
                           UNSIGNED8               NMTisPreOrOperational,
                           UNSIGNED16              timeDifference_100us,
                           UNSIGNED16              EMinhTime);


#endif

#endif
