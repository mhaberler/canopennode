/**
 * CANopen Emergency protocol.
 *
 * @file        CO_Emergency.h
 * @ingroup     CO_Emergency
 * @version     SVN: \$Id$
 * @author      Janez Paternoster
 * @copyright   2004 - 2013 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <http://canopennode.sourceforge.net>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * CANopenNode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef CO_EMERGENCY_H
#define CO_EMERGENCY_H


/**
 * @defgroup CO_Emergency Emergency
 * @ingroup CO_CANopen
 * @{
 *
 * CANopen Emergency protocol.
 *
 * Error control and Emergency is used for control internal error state
 * and for sending a CANopen Emergency message.
 *
 * In case of error condition stack or application calls CO_errorReport()
 * function with indication of the error. Specific error condition is reported
 * (with CANopen Emergency message) only the first time after it occurs.
 * Internal state of the error condition is controlled with
 * #CO_EM_errorStatusBits_t. Specific error condition can also be reset by
 * CO_errorReset() function. If so, Emergency message is sent with
 * CO_EM_NO_ERROR indication.
 *
 * Some error conditions are informative and some are critical. Critical error
 * conditions sets the #CO_errorRegisterBitmask_t.
 *
 * Latest errors can be read from _Pre Defined Error Field_ (object dictionary,
 * index 0x1003). #CO_EM_errorStatusBits_t can also be read form CANopen
 * object dictionary.
 *
 * ###Emergency message contents:
 *
 *   Byte | Description
 *   -----|-----------------------------------------------------------
 *   0..1 | #CO_EM_errorCode_t.
 *   2    | #CO_errorRegisterBitmask_t.
 *   3    | Index of error condition (see #CO_EM_errorStatusBits_t).
 *   4..7 | Additional argument informative to CO_errorReport() function.
 *
 * ####Contents of _Pre Defined Error Field_ (object dictionary, index 0x1003):
 * bytes 0..3 are equal to bytes 0..3 in the Emergency message.
 *
 * @see #CO_Default_CAN_ID_t
 */


/**
 * CANopen Error register.
 *
 * In object dictionary on index 0x1001.
 *
 * Error register is calculated from critical internal #CO_EM_errorStatusBits_t.
 * Generic and communication bits are calculated in CO_EM_process
 * function, device profile or manufacturer specific bits may be calculated
 * inside the application.
 *
 * Internal errors may prevent device to stay in NMT Operational state. Details
 * are described in _Error Behavior_ object in Object Dictionary at index 0x1029.
 */
typedef enum{
    CO_ERR_REG_GENERIC_ERR  = 0x01, /**< bit 0, generic error */
    CO_ERR_REG_CURRENT      = 0x02, /**< bit 1, current */
    CO_ERR_REG_VOLTAGE      = 0x04, /**< bit 2, voltage */
    CO_ERR_ERG_TEMPERATUR   = 0x08, /**< bit 3, temperature */
    CO_ERR_REG_COMM_ERR     = 0x10, /**< bit 4, communication error (overrun, error state) */
    CO_ERR_REG_DEV_PROFILE  = 0x20, /**< bit 5, device profile specific */
    CO_ERR_REG_RESERVED     = 0x40, /**< bit 6, reserved (always 0) */
    CO_ERR_REG_MANUFACTURER = 0x80  /**< bit 7, manufacturer specific */
}CO_errorRegisterBitmask_t;


/**
 * CANopen Error codes.
 *
 * Standard error codes according to CiA DS-301 and DS-401.
 */
typedef enum{
    CO_EMC_NO_ERROR         = 0x0000, /**< 0x00xx, error Reset or No Error */
    CO_EMC_GENERIC          = 0x1000, /**< 0x10xx, Generic Error */
    CO_EMC_CURRENT          = 0x2000, /**< 0x20xx, Current */
    CO_EMC_CURRENT_INPUT    = 0x2100, /**< 0x21xx, Current, device input side */
    CO_EMC_CURRENT_INSIDE   = 0x2200, /**< 0x22xx, Current inside the device */
    CO_EMC_CURRENT_OUTPUT   = 0x2300, /**< 0x23xx, Current, device output side */
    CO_EMC_VOLTAGE          = 0x3000, /**< 0x30xx, Voltage */
    CO_EMC_VOLTAGE_MAINS    = 0x3100, /**< 0x31xx, Mains Voltage */
    CO_EMC_VOLTAGE_INSIDE   = 0x3200, /**< 0x32xx, Voltage inside the device */
    CO_EMC_VOLTAGE_OUTPUT   = 0x3300, /**< 0x33xx, Output Voltage */
    CO_EMC_TEMPERATURE      = 0x4000, /**< 0x40xx, Temperature */
    CO_EMC_TEMP_AMBIENT     = 0x4100, /**< 0x41xx, Ambient Temperature */
    CO_EMC_TEMP_DEVICE      = 0x4200, /**< 0x42xx, Device Temperature */
    CO_EMC_HARDWARE         = 0x5000, /**< 0x50xx, Device Hardware */
    CO_EMC_SOFTWARE_DEVICE  = 0x6000, /**< 0x60xx, Device Software */
    CO_EMC_SOFTWARE_INTERNAL= 0x6100, /**< 0x61xx, Internal Software */
    CO_EMC_SOFTWARE_USER    = 0x6200, /**< 0x62xx, User Software */
    CO_EMC_DATA_SET         = 0x6300, /**< 0x63xx, Data Set */
    CO_EMC_ADDITIONAL_MODUL = 0x7000, /**< 0x70xx, Additional Modules */
    CO_EMC_MONITORING       = 0x8000, /**< 0x80xx, Monitoring */
    CO_EMC_COMMUNICATION    = 0x8100, /**< 0x81xx, Communication */
    CO_EMC_CAN_OVERRUN      = 0x8110, /**< 0x8110, CAN Overrun (Objects lost) */
    CO_EMC_CAN_PASSIVE      = 0x8120, /**< 0x8120, CAN in Error Passive Mode */
    CO_EMC_HEARTBEAT        = 0x8130, /**< 0x8130, Life Guard Error or Heartbeat Error */
    CO_EMC_BUS_OFF_RECOVERED= 0x8140, /**< 0x8140, recovered from bus off */
    CO_EMC_CAN_ID_COLLISION = 0x8150, /**< 0x8150, CAN-ID collision */
    CO_EMC_PROTOCOL_ERROR   = 0x8200, /**< 0x82xx, Protocol Error */
    CO_EMC_PDO_LENGTH       = 0x8210, /**< 0x8210, PDO not processed due to length error */
    CO_EMC_PDO_LENGTH_EXC   = 0x8220, /**< 0x8220, PDO length exceeded */
    CO_EMC_DAM_MPDO         = 0x8230, /**< 0x8230, DAM MPDO not processed, destination object not available */
    CO_EMC_SYNC_DATA_LENGTH = 0x8240, /**< 0x8240, Unexpected SYNC data length */
    CO_EMC_RPDO_TIMEOUT     = 0x8250, /**< 0x8250, RPDO timeout */
    CO_EMC_EXTERNAL_ERROR   = 0x9000, /**< 0x90xx, External Error */
    CO_EMC_ADDITIONAL_FUNC  = 0xF000, /**< 0xF0xx, Additional Functions */
    CO_EMC_DEVICE_SPECIFIC  = 0xFF00, /**< 0xFFxx, Device specific */

    CO_EMC401_OUT_CUR_HI    = 0x2310, /**< 0x2310, DS401, Current at outputs too high (overload) */
    CO_EMC401_OUT_SHORTED   = 0x2320, /**< 0x2320, DS401, Short circuit at outputs */
    CO_EMC401_OUT_LOAD_DUMP = 0x2330, /**< 0x2330, DS401, Load dump at outputs */
    CO_EMC401_IN_VOLT_HI    = 0x3110, /**< 0x3110, DS401, Input voltage too high */
    CO_EMC401_IN_VOLT_LOW   = 0x3120, /**< 0x3120, DS401, Input voltage too low */
    CO_EMC401_INTERN_VOLT_HI= 0x3210, /**< 0x3210, DS401, Internal voltage too high */
    CO_EMC401_INTERN_VOLT_LO= 0x3220, /**< 0x3220, DS401, Internal voltage too low */
    CO_EMC401_OUT_VOLT_HIGH = 0x3310, /**< 0x3310, DS401, Output voltage too high */
    CO_EMC401_OUT_VOLT_LOW  = 0x3320  /**< 0x3320, DS401, Output voltage too low */
}CO_EM_errorCode_t;


/**
 * Error status bits.
 *
 * Internal indication of the error condition.
 *
 * Each error condition is specified by unique index from 0x00 up to 0xFF.
 * Variable  (from manufacturer section in the Object
 * Dictionary) contains up to 0xFF bits (32bytes) for the identification of the
 * specific error condition. (Type of the variable is CANopen OCTET_STRING.)
 *
 * If specific error occurs in the stack or in the application, CO_errorReport()
 * sets specific bit in the _Error Status Bits_ variable. If bit was already
 * set, function returns without any action. Otherwise it prepares emergency
 * message.
 *
 * CO_errorReport(), CO_errorReset() or CO_isError() functions are called
 * with unique index as an argument. (However CO_errorReport(), for example, may
 * be used with the same index on multiple places in the code.)
 *
 * Macros defined below are combination of two constants: index and
 * #CO_EM_errorCode_t. They represents specific error conditions. They are
 * used as double argument for CO_errorReport(), CO_errorReset() and
 * CO_isError() functions.
 *
 * Stack uses first 6 bytes of the _Error Status Bits_ variable. Device profile
 * or application may define own macros for Error status bits. Note that
 * _Error Status Bits_ must be large enough (up to 32 bytes).
 */
typedef enum{
    CO_EM_NO_ERROR                              = 0x00, /**< 0x00, Error Reset or No Error */
    CO_EM_CAN_BUS_WARNING                       = 0x01, /**< 0x01, communication, info, CAN bus warning limit reached */
    CO_EM_RXMSG_WRONG_LENGTH                    = 0x02, /**< 0x02, communication, info, Wrong data length of the received CAN message */
    CO_EM_RXMSG_OVERFLOW                        = 0x03, /**< 0x03, communication, info, Previous received CAN message wasn't processed yet */
    CO_EM_RPDO_WRONG_LENGTH                     = 0x04, /**< 0x04, communication, info, Wrong data length of received PDO */
    CO_EM_RPDO_OVERFLOW                         = 0x05, /**< 0x05, communication, info, Previous received PDO wasn't processed yet */
    CO_EM_CAN_RX_BUS_PASSIVE                    = 0x06, /**< 0x06, communication, info, CAN receive bus is passive */
    CO_EM_CAN_TX_BUS_PASSIVE                    = 0x07, /**< 0x07, communication, info, CAN transmit bus is passive */
    CO_EM_NMT_WRONG_COMMAND                     = 0x08, /**< 0x08, communication, info, Wrong NMT command received */
    CO_EM_09_unused                             = 0x09, /**< 0x09, (unused) */
    CO_EM_0A_unused                             = 0x0A, /**< 0x0A, (unused) */
    CO_EM_0B_unused                             = 0x0B, /**< 0x0B, (unused) */
    CO_EM_0C_unused                             = 0x0C, /**< 0x0C, (unused) */
    CO_EM_0D_unused                             = 0x0D, /**< 0x0D, (unused) */
    CO_EM_0E_unused                             = 0x0E, /**< 0x0E, (unused) */
    CO_EM_0F_unused                             = 0x0F, /**< 0x0F, (unused) */

    CO_EM_10_unused                             = 0x10, /**< 0x10, (unused) */
    CO_EM_11_unused                             = 0x11, /**< 0x11, (unused) */
    CO_EM_CAN_TX_BUS_OFF                        = 0x12, /**< 0x12, communication, critical, CAN transmit bus is off */
    CO_EM_CAN_RXB_OVERFLOW                      = 0x13, /**< 0x13, communication, critical, CAN module receive buffer has overflowed */
    CO_EM_CAN_TX_OVERFLOW                       = 0x14, /**< 0x14, communication, critical, CAN transmit buffer has overflowed */
    CO_EM_TPDO_OUTSIDE_WINDOW                   = 0x15, /**< 0x15, communication, critical, TPDO is outside SYNC window */
    CO_EM_16_unused                             = 0x16, /**< 0x16, (unused) */
    CO_EM_17_unused                             = 0x17, /**< 0x17, (unused) */
    CO_EM_SYNC_TIME_OUT                         = 0x18, /**< 0x18, communication, critical, SYNC message timeout */
    CO_EM_SYNC_LENGTH                           = 0x19, /**< 0x19, communication, critical, Unexpected SYNC data length */
    CO_EM_PDO_WRONG_MAPPING                     = 0x1A, /**< 0x1A, communication, critical, Error with PDO mapping */
    CO_EM_HEARTBEAT_CONSUMER                    = 0x1B, /**< 0x1B, communication, critical, Heartbeat consumer timeout */
    CO_EM_HEARTBEAT_CONSUMER_REMOTE_RESET       = 0x1C, /**< 0x1C, communication, critical, Heartbeat consumer detected remote node reset */
    CO_EM_1D_unused                             = 0x1D, /**< 0x1D, (unused) */
    CO_EM_1E_unused                             = 0x1E, /**< 0x1E, (unused) */
    CO_EM_1F_unused                             = 0x1F, /**< 0x1F, (unused) */

    CO_EM_EMERGENCY_BUFFER_FULL                 = 0x20, /**< 0x20, generic, info, Emergency buffer is full, Emergency message wasn't sent */
    CO_EM_21_unused                             = 0x21, /**< 0x21, (unused) */
    CO_EM_MICROCONTROLLER_RESET                 = 0x22, /**< 0x22, generic, info, Microcontroller has just started */
    CO_EM_23_unused                             = 0x23, /**< 0x23, (unused) */
    CO_EM_24_unused                             = 0x24, /**< 0x24, (unused) */
    CO_EM_25_unused                             = 0x25, /**< 0x25, (unused) */
    CO_EM_26_unused                             = 0x26, /**< 0x26, (unused) */
    CO_EM_27_unused                             = 0x27, /**< 0x27, (unused) */

    CO_EM_WRONG_ERROR_REPORT                    = 0x28, /**< 0x28, generic, critical, Wrong parameters to CO_errorReport() function */
    CO_EM_ISR_TIMER_OVERFLOW                    = 0x29, /**< 0x29, generic, critical, Timer task has overflowed */
    CO_EM_MEMORY_ALLOCATION_ERROR               = 0x2A, /**< 0x2A, generic, critical, Unable to allocate memory for objects */
    CO_EM_GENERIC_ERROR                         = 0x2B, /**< 0x2B, generic, critical, Generic error, test usage */
    CO_EM_MAIN_TIMER_OVERFLOW                   = 0x2C, /**< 0x2C, generic, critical, Mainline function exceeded maximum execution time */
    CO_EM_INCONSISTENT_OBJECT_DICTIONARY        = 0x2D, /**< 0x2D, generic, critical, Object dictionary does not match the software */
    CO_EM_CALCULATION_OF_PARAMETERS             = 0x2E, /**< 0x2E, generic, critical, Error in calculation of device parameters */
    CO_EM_NON_VOLATILE_MEMORY                   = 0x2F  /**< 0x2F, generic, critical, Error with access to non volatile device memory */
}CO_EM_errorStatusBits_t;


/**
 * Size of internal buffer, whwre emergencies are stored after CO_errorReport().
 * Buffer is cleared by CO_EM_process().
 */
#define CO_EM_INTERNAL_BUFFER_SIZE      10


/**
 * Emergerncy object for CO_errorReport(). It contains error buffer, to which new emergency
 * messages are written, when CO_errorReport() is called. This object is included in
 * CO_EMpr_t object.
 */
typedef struct{
    uint8_t            *errorStatusBits;/**< From CO_EM_init() */
    uint8_t             errorStatusBitsSize;/**< From CO_EM_init() */
    /** Internal buffer for storing unsent emergency messages.*/
    uint8_t             buf[CO_EM_INTERNAL_BUFFER_SIZE * 8];
    uint8_t            *bufEnd;         /**< End+1 address of the above buffer */
    uint8_t            *bufWritePtr;    /**< Write pointer in the above buffer */
    uint8_t            *bufReadPtr;     /**< Read pointer in the above buffer */
    uint8_t             bufFull;        /**< True if above buffer is full */
    uint8_t             wrongErrorReport;/**< Error in arguments to CO_errorReport() */
}CO_EM_t;


/**
 * Report error condition.
 *
 * Function is called on any error condition inside CANopen stack and may also
 * be called by application on custom error condition. Emergency message is sent
 * after the first occurance of specific error. In case of critical error, device
 * will not be able to stay in NMT_OPERATIONAL state.
 *
 * Function is short and may be used form any task or interrupt.
 *
 * @param EM Emergency object.
 * @param errorBit from #CO_EM_errorStatusBits_t.
 * @param errorCode from #CO_EM_errorCode_t.
 * @param infoCode 32 bit value is passed to bytes 4...7 of the Emergency message.
 * It contains optional additional information inside emergency message.
 *
 * @return -3: CO_errorReport is busy, message is deleted.
 * @return -2: Emergency buffer is full, message is deleted.
 * @return -1: Error in arguments.
 * @return  0: Error was already present before, no action was performed.
 * @return  1: Error is new, Emergency will be send.
 */
int8_t CO_errorReport(CO_EM_t *EM, uint8_t errorBit, uint16_t errorCode, uint32_t infoCode);


/**
 * Reset error condition.
 *
 * Function is called if any error condition is solved. Emergency message is sent
 * with #CO_EM_errorCode_t 0x0000.
 *
 * Function is short and may be used form any task or interrupt.
 *
 * @param EM Emergency object.
 * @param errorBit from #CO_EM_errorStatusBits_t.
 * @param infoCode 32 bit value is passed to bytes 4...7 of the Emergency message.
 *
 * @return -3: CO_errorReport is busy, message is deleted.
 * @return -2: Emergency buffer is full, message is deleted.
 * @return -1: Error in arguments.
 * @return  0: Error was already present before, no action was performed.
 * @return  1: Error bit is cleared, Emergency with #CO_EM_errorCode_t 0 will be send.
 */
int8_t CO_errorReset(CO_EM_t *EM, uint8_t errorBit, uint32_t infoCode);


/**
 * Check specific error condition.
 *
 * Function returns 1, if specific internal error is present. Otherwise it returns 0.
 *
 * @param EM Emergency object.
 * @param errorBit from #CO_EM_errorStatusBits_t.
 *
 * @return 0: Error is not present.
 * @return 1: Error is present.
 */
int8_t CO_isError(CO_EM_t *EM, uint8_t errorBit);


#ifdef CO_DOXYGEN
/** Skip section, if CO_SDO.h is not included */
    #define CO_SDO_H
#endif
#ifdef CO_SDO_H


/**
 * Error control and Emergency object. It controls internal error state and
 * sends emergency message, if error condition was reported. Object is initialized
 * by CO_EM_init(). It contains CO_EM_t object.
 */
typedef struct{
    uint8_t            *errorRegister;  /**< From CO_EM_init() */
    uint32_t           *preDefErr;      /**< From CO_EM_init() */
    uint8_t             preDefErrSize;  /**< From CO_EM_init() */
    uint8_t             preDefErrNoOfErrors;/**< Number of active errors in preDefErr */
    uint16_t            inhibitEmTimer; /**< Internal timer for emergency message */
    CO_EM_t            *EM;             /**< CO_EM_t sub object is included here */
    CO_CANmodule_t     *CANdev;         /**< From CO_EM_init() */
    CO_CANtx_t         *CANtxBuff;      /**< CAN transmit buffer */
}CO_EMpr_t;


/**
 * Initialize Error control and Emergency object.
 *
 * Function must be called in the communication reset section.
 *
 * @param EMpr This object will be initialized.
 * @param EM Emergency object defined separately. Will be included in EMpr and
 * initialized too.
 * @param SDO SDO server object.
 * @param errorStatusBits Pointer to _Error Status Bits_ array from Object Dictionary
 * (manufacturer specific section). See #CO_EM_errorStatusBits_t.
 * @param errorStatusBitsSize Total size of the above array. Must be >= 6.
 * @param errorRegister Pointer to _Error Register_ (Object dictionary, index 0x1001).
 * @param preDefErr Pointer to _Pre defined error field_ array from Object
 * dictionary, index 0x1003.
 * @param preDefErrSize Size of the above array.
 * @param CANdev CAN device for Emergency transmission.
 * @param CANdevTxIdx Index of transmit buffer in the above CAN device.
 * @param CANidTxEM CAN identifier for Emergency message.
 *
 * @return #CO_ReturnError_t CO_ERROR_NO or CO_ERROR_ILLEGAL_ARGUMENT.
 */
int16_t CO_EM_init(
        CO_EM_t                *EM,
        CO_EMpr_t              *EMpr,
        CO_SDO_t               *SDO,
        uint8_t                *errorStatusBits,
        uint8_t                 errorStatusBitsSize,
        uint8_t                *errorRegister,
        uint32_t               *preDefErr,
        uint8_t                 preDefErrSize,
        CO_CANmodule_t         *CANdev,
        uint16_t                CANdevTxIdx,
        uint16_t                CANidTxEM);


/**
 * Process Error control and Emergency object.
 *
 * Function must be called cyclically. It verifies some communication errors,
 * calculates bit 0 and bit 4 from _Error register_ and sends emergency message
 * if necessary.
 *
 * @param EMpr This object.
 * @param NMTisPreOrOperational True if this node is NMT_PRE_OPERATIONAL or NMT_OPERATIONAL.
 * @param timeDifference_100us Time difference from previous function call in [100 * microseconds].
 * @param EMinhTime _Inhibit time EMCY_ (object dictionary, index 0x1015).
 */
void CO_EM_process(
        CO_EMpr_t              *EMpr,
        uint8_t                 NMTisPreOrOperational,
        uint16_t                timeDifference_100us,
        uint16_t                EMinhTime);


#endif

/** @} */
#endif
