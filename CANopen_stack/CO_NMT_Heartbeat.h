/*******************************************************************************

   File: CO_NMT_Heartbeat.h
   CANopen NMT and Heartbeat producer object.

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

#ifndef _CO_NMT_HEARTBEAT_H
#define _CO_NMT_HEARTBEAT_H


/*******************************************************************************
   Topic: NMT message

   Contents of NMT message.

   For CAN identifier see <Default COB identifiers>.

   Specific data bytes:
      byte 0   - <NMT command>
      byte 1   - Node ID. If zero, command addresses all nodes.
*******************************************************************************/


/*******************************************************************************
   Topic: Heartbeat message contents

   Contents of Heartbeat message.

   For CAN identifier see <Default COB identifiers>.

   Specific data bytes:
      byte 0   - <NMT internal state> of the node.
*******************************************************************************/


/*******************************************************************************
   Constants: NMT internal state
      CO_NMT_INITIALIZING           - (0) - Device is initializing.
      CO_NMT_PRE_OPERATIONAL        - (127 = 0x7F) - Device is in pre-operational state.
      CO_NMT_OPERATIONAL            - (5) - Device is in operational state.
      CO_NMT_STOPPED                - (4) - Device is stopped.

   Constants: NMT command
      CO_NMT_ENTER_OPERATIONAL      - (1) - Start device.
      CO_NMT_ENTER_STOPPED          - (2) - Stop device.
      CO_NMT_ENTER_PRE_OPERATIONAL  - (128 = 0x80) - Put device into pre-operational.
      CO_NMT_RESET_NODE             - (129 = 0x81) - Reset device.
      CO_NMT_RESET_COMMUNICATION    - (130 = 0x82) - Reset CANopen communication on device.
*******************************************************************************/
#define  CO_NMT_INITIALIZING           0
#define  CO_NMT_PRE_OPERATIONAL        127
#define  CO_NMT_OPERATIONAL            5
#define  CO_NMT_STOPPED                4

#define  CO_NMT_ENTER_OPERATIONAL      1
#define  CO_NMT_ENTER_STOPPED          2
#define  CO_NMT_ENTER_PRE_OPERATIONAL  128
#define  CO_NMT_RESET_NODE             129
#define  CO_NMT_RESET_COMMUNICATION    130


/*******************************************************************************
   Object: CO_NMT_t

   Object controls NMT state of the device and sends Heartbeat message.

   Variables:
      operatingState    - CANopen device NMT internal state.

      LEDflickering     - see <LEDs>.
      LEDblinking       - see <LEDs>.
      LEDsingleFlash    - see <LEDs>.
      LEDdoubleFlash    - see <LEDs>.
      LEDtripleFlash    - see <LEDs>.
      LEDquadrupleFlash - see <LEDs>.
      LEDgreenRun       - see <LEDs>.
      LEDredError       - see <LEDs>.

      resetCommand      - If different than zero, device will reset.
      nodeId            - CANopen Node ID of this device.
      HBproducerTimer   - Internal timer for HB producer.
      firstHBTime       - See parameters in <CO_NMT_init>.

      EMpr              - Pointer to Emergency object <CO_emergencyProcess_t>.

      HB_CANdev         - Pointer to CAN device used for HB transmission <CO_CANmodule_t>.
      HB_TXbuff         - CAN transmit buffer inside CANdev for CAN tx message.
*******************************************************************************/
typedef struct{
   UNSIGNED8            operatingState;

   INTEGER8                LEDflickering;
   INTEGER8                LEDblinking;
   INTEGER8                LEDsingleFlash;
   INTEGER8                LEDdoubleFlash;
   INTEGER8                LEDtripleFlash;
   INTEGER8                LEDquadrupleFlash;
   INTEGER8                LEDgreenRun;
   INTEGER8                LEDredError;

   UNSIGNED8               resetCommand;
   UNSIGNED8               nodeId;
   UNSIGNED16              HBproducerTimer;
   UNSIGNED16              firstHBTime;

   CO_emergencyProcess_t  *EMpr;

   CO_CANmodule_t   *HB_CANdev;
   CO_CANtxArray_t  *HB_TXbuff;
}CO_NMT_t;


/*******************************************************************************
   Macros: LEDs
      LED_FLICKERING      - 10HZ (100MS INTERVAL)
      LED_BLINKING        - 2.5HZ (400MS INTERVAL)
      LED_SINGLE_FLASH    - 200MS ON, 1000MS OFF
      LED_DOUBLE_FLASH    - 200MS ON, 200MS OFF, 200MS ON, 1000MS OFF
      LED_TRIPLE_FLASH    - 200MS ON, 200MS OFF, 200MS ON, 200MS OFF, 200MS ON, 1000MS OFF
      LED_GREEN_RUN       - CANOPEN RUN LED ACCORDING TO CIA DR 303-3.
      LED_RED_ERROR       - CANopen error LED according to CiA DR 303-3.

   LED_XXX are helper macros for implementing status LED diodes. If macro returns
   1 LED should be ON, otherwise OFF. Function <CO_NMT_blinkingProcess50ms> must
   be called cyclically, to update the variables.


*******************************************************************************/
   #define LED_FLICKERING(NMT)      (((NMT)->LEDflickering>=0)    ? 1 : 0)
   #define LED_BLINKING(NMT)        (((NMT)->LEDblinking>=0)      ? 1 : 0)
   #define LED_SINGLE_FLASH(NMT)    (((NMT)->LEDsingleFlash>=0)   ? 1 : 0)
   #define LED_DOUBLE_FLASH(NMT)    (((NMT)->LEDdoubleFlash>=0)   ? 1 : 0)
   #define LED_TRIPLE_FLASH(NMT)    (((NMT)->LEDtripleFlash>=0)   ? 1 : 0)
   #define LED_QUADRUPLE_FLASH(NMT) (((NMT)->LEDquadrupleFlash>=0)? 1 : 0)
   #define LED_GREEN_RUN(NMT)       (((NMT)->LEDgreenRun>=0)      ? 1 : 0)
   #define LED_RED_ERROR(NMT)       (((NMT)->LEDredError>=0)      ? 1 : 0)

/*******************************************************************************
   Function: CO_NMT_receive

   Read received message from CAN module.

   Function will be called (by CAN receive interrupt) every time, when CAN
   message with correct identifier will be received. For more information and
   description of parameters see topic <Receive CAN message function>.
*******************************************************************************/
INTEGER16 CO_NMT_receive(void *object, CO_CANrxMsg_t *msg);


/*******************************************************************************
   Function: CO_NMT_init

   Initialize NMT and Heartbeat producer.

   Function must be called in the communication reset section.

   Parameters:
      ppNMT       - Pointer to address of NMT object <CO_NMT_t>.
                    If address is zero, memory for new object will be
                    allocated and address will be set.
      EMpr        - Pointer to Emergency object <CO_emergencyProcess_t>.
      nodeId      - CANopen Node ID of this device.
      firstHBTime - Time between bootup and first heartbeat message in milliseconds.
                    If firstHBTime is greater than _Producer Heartbeat time_
                    (object dictionary, index 0x1017), latter is used instead.
      NMT_CANdev  - CAN device for NMT reception <CO_CANmodule_t>.
      NMT_rxIdx   - Index of receive buffer for NMT reception.
      CANidRxNMT  - CAN identifier for NMT message.
      HB_CANdev   - CAN device for HB transmission <CO_CANmodule_t>.
      HB_txIdx    - Index of transmit buffer pointer for HB transmission.
      CANidTxHB   - CAN identifier for HB message.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
      CO_ERROR_OUT_OF_MEMORY      - Memory allocation failed.
*******************************************************************************/
INTEGER16 CO_NMT_init(
      CO_NMT_t              **ppNMT,
      CO_emergencyProcess_t  *EMpr,
      UNSIGNED8               nodeId,
      UNSIGNED16              firstHBTime,
      CO_CANmodule_t *NMT_CANdev, UNSIGNED16 NMT_rxIdx, UNSIGNED16 CANidRxNMT,
      CO_CANmodule_t *HB_CANdev,  UNSIGNED16 HB_txIdx,  UNSIGNED16 CANidTxHB);


/*******************************************************************************
   Function: CO_NMT_delete

   Delete NMT object and free memory.

   Parameters:
      ppNMT       - Pointer to pointer to NMT object <CO_NMT_t>.
                    Pointer to NMT object is set to 0.
*******************************************************************************/
void CO_NMT_delete(CO_NMT_t **ppNMT);


/*******************************************************************************
   Function: CO_NMT_blinkingProcess50ms

   Calculate blinking bytes.

   Function must be called cyclically every 50 milliseconds. See <LEDs>.

   Parameters:
      NMT         - Pointer to NMT object <CO_NMT_t>.
*******************************************************************************/
void CO_NMT_blinkingProcess50ms(CO_NMT_t *NMT);


/*******************************************************************************
   Function: CO_NMT_process

   Process received NMT and produce Heartbeat messages.

   Function must be called cyclically.

   Parameters:
      NMT               - Pointer to NMT object <CO_NMT_t>.
      timeDifference_ms - Time difference from previous function call in [milliseconds].
      HBtime            - _Producer Heartbeat time_ (object dictionary, index 0x1017).
      NMTstartup        - _NMT startup behavior_ (object dictionary, index 0x1F80).
      errReg            - _Error register_ (object dictionary, index 0x1001).
      errBehavior       - pointer to _Error behavior_ array (object dictionary, index 0x1029).
                          Object controls, if device should leave NMT operational state.
                          Length of array must be 6. If pointer is NULL, no calculation is made.

   Return:
      0  - Normal return.
      1  - Application must provide communication reset.
      2  - Application must provide complete device reset.
*******************************************************************************/
UNSIGNED8 CO_NMT_process(  CO_NMT_t         *NMT,
                           UNSIGNED16        timeDifference_ms,
                           UNSIGNED16        HBtime,
                           UNSIGNED32        NMTstartup,
                           UNSIGNED8         errReg,
                           const UNSIGNED8  *errBehavior);


#endif
