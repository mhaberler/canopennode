/*******************************************************************************

   File: CANopen.h
   Main CANopen stack file. It combines Object dictionary (CO_OD) and all other
   CANopen source files. Configuration information are read from CO_OD.h file.

   Copyright (C) 2010 Janez Paternoster

   License: GNU General Public License (GPL).

   <http://canopennode.sourceforge.net>
*/
/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.


   Author: Janez Paternoster

*******************************************************************************/

#ifndef _CANopen_H
#define _CANopen_H


   #include "CO_driver.h"
   #include "CO_OD.h"
   #include "CO_SDO.h"
   #include "CO_Emergency.h"
   #include "CO_NMT_Heartbeat.h"
   #include "CO_SYNC.h"
   #include "CO_PDO.h"
   #include "CO_HBconsumer.h"
#if CO_NO_SDO_CLIENT == 1
   #include "CO_SDOmaster.h"
#endif


/*******************************************************************************
   Object: CO_t

   CANopen stack object combines all CANopen objects.

   Variables:
      CANmodule[]    - CAN module object(s) of type <CO_CANmodule_t>.
      SDO            - SDO object of type <CO_SDO_t>.
      EM             - Emergency report object of type <CO_emergencyReport_t>.
      EMpr           - Emergency process object of type <CO_emergencyProcess_t>.
      NMT            - NMT object of type <CO_NMT_t>.
      SYNC           - SYNC object of type <CO_SYNC_t>.
      RPDO[]         - RPDO object(s) of type <CO_RPDO_t>.
      TPDO[]         - TPDO object(s) of type <CO_TPDO_t>.
      HBcons         - Heartbeat consumer object of type <CO_HBconsumer_t>.
      SDOclient      - SDO client object of type <CO_SDOclient_t>.
*******************************************************************************/
typedef struct{
   CO_CANmodule_t            *CANmodule[1];
   CO_SDO_t                  *SDO;
   CO_emergencyReport_t      *EM;
   CO_emergencyProcess_t     *EMpr;
   CO_NMT_t                  *NMT;
   CO_SYNC_t                 *SYNC;
   CO_RPDO_t                 *RPDO[CO_NO_RPDO];
   CO_TPDO_t                 *TPDO[CO_NO_TPDO];
   CO_HBconsumer_t           *HBcons;
#if CO_NO_SDO_CLIENT == 1
   CO_SDOclient_t            *SDOclient;
#endif
}CO_t;


/*******************************************************************************
   Object: NMT master

   Function CO_sendNMTcommand() is simple function, which sends CANopen message.
   This part of code is an example of custom definition of simple CANopen
   object. Follow the code in CANopen.c file. If macro CO_NO_NMT_MASTER is 1,
   function CO_sendNMTcommand can be used to send NMT master message.

   Parameters:
      CO                          - Pointer to CANopen object <CO_t>.
      command                     - NMT command.
      noodeID                     - Node ID.

   Return:
      0                           - Operation completed successfully.
      other                       - same as <CO_CANsend>.
*******************************************************************************/
#if CO_NO_NMT_MASTER == 1
   UNSIGNED8 CO_sendNMTcommand(CO_t *CO, UNSIGNED8 command, UNSIGNED8 nodeID);
#endif


/*******************************************************************************
   Function: CO_init

   Initialize CANopen stack.

   Function must be called in the communication reset section.

   Parameters:
      ppCO              - Pointer to address of CANopen object <CO_t>.
                          If address is zero, memory for new object will be
                          allocated and address will be set.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
      CO_ERROR_OUT_OF_MEMORY      - Memory allocation failed.
      CO_ERROR_ILLEGAL_BAUDRATE   - Illegal baudrate passed to function <CO_CANmodule_init>
*******************************************************************************/
INTEGER16 CO_init(CO_t **ppCO);


/*******************************************************************************
   Function: CO_delete

   Delete CANopen object and free memory.

   Parameters:
      ppCO              - Pointer to pointer to CANopen object <CO_t>.
                          Pointer to object is set to 0.
*******************************************************************************/
void CO_delete(CO_t **ppCO);


/*******************************************************************************
   Function: CO_process

   Process CANopen objects.

   Function must be called cyclically. It processes all "asynchronous" CANopen
   objects.

   Parameters:
      CO                - Pointer to CANopen object <CO_t>.
      timeDifference_ms - Time difference from previous function call in [milliseconds].

    Return (from <CO_NMT_process>):
      0  - Normal return, no action.
      1  - Application must provide communication reset.
      2  - Application must provide complete device reset.
*******************************************************************************/
UNSIGNED8 CO_process( CO_t        *CO,
                      UNSIGNED16   timeDifference_ms);


/*******************************************************************************
   Function: CO_process_RPDO

   Process CANopen SYNC and RPDO objects.

   Function must be called cyclically from synchronous 1ms task. It processes
   SYNC and receive PDO CANopen objects.

   Parameters:
      CO                - Pointer to CANopen object <CO_t>.
*******************************************************************************/
void CO_process_RPDO(CO_t *CO);


/*******************************************************************************
   Function: CO_process_TPDO

   Process CANopen TPDO objects.

   Function must be called cyclically from synchronous 1ms task. It processes
   transmit PDO CANopen objects.

   Parameters:
      CO                - Pointer to CANopen object <CO_t>.
*******************************************************************************/
void CO_process_TPDO(CO_t *CO);


#endif
