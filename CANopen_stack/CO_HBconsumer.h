/*******************************************************************************

   File: CO_HBconsumer.h
   CANopen Heartbeat consumer object.

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

#ifndef _CO_HB_CONS_H
#define _CO_HB_CONS_H


/*******************************************************************************
   Topic: Heartbeat consumer

   Monitoring of remote nodes.

   Heartbeat consumer monitors Heartbeat messages from remote nodes. If any
   monitored node don't send his Heartbeat in specified time, Heartbeat consumer
   sends emergency message. If all monitored nodes are operational, then
   variable _allMonitoredOperational_ inside <CO_HBconsumer_t> is set to true.
   Monitoring starts after the reception of the first HeartBeat (not bootup).

   See also <Heartbeat message contents> and <NMT internal state> in
   CO_NMT_Heartbeat.h file.
*******************************************************************************/


/*******************************************************************************
   Object: CO_HBconsNode_t

   Structure for one monitored node inside <CO_HBconsumer_t>.

   Variables:
      NMTstate     - NMT state of the remote node.
      monStarted   - Monitoring for the remote node started. (After reception
                     of the first Heartbeat mesage.)
      timeoutTimer - Timeout timer for remote node.
      time         - consumer heartbeat time.
      CANrxNew     - Indication, if new Heartbeat message received from CAN bus.
*******************************************************************************/
typedef struct{
   UNSIGNED8         NMTstate;
   UNSIGNED8         monStarted;
   UNSIGNED16        timeoutTimer;
   UNSIGNED16        time;
   UNSIGNED8         CANrxNew;
}CO_HBconsNode_t;


/*******************************************************************************
   Object: CO_HBconsumer_t

   Object controls Heartbeat consumer.

   Variables:
      EM                            - Pointer to Emergency object <CO_emergencyReport_t>.
      ObjDict_consumerHeartbeatTime - Pointer to _Consumer Heartbeat Time_ array
                                      from Object Dictionary (index 0x1016). Size
                                      of array is equal to numberOfMonitoredNodes.
      monitoredNodes                - Pointer to externaly defined array of
                                      <CO_HBconsNode_t> objects. Size of array
                                      is equal to numberOfMonitoredNodes.
      numberOfMonitoredNodes        - Number of monitored nodes.
      allMonitoredOperational       - True, if all monitored nodes are NMT
                                      operational or no node is monitored.
      CANdevRx                      - CAN device for HB_cons reception <CO_CANmodule_t>.
      CANdevRxIdxStart              - Index of receive buffer for HB_cons reception.
*******************************************************************************/
typedef struct{
   CO_emergencyReport_t   *EM;
   const UNSIGNED32       *ObjDict_consumerHeartbeatTime;
   CO_HBconsNode_t        *monitoredNodes;
   UNSIGNED8               numberOfMonitoredNodes;
   UNSIGNED8               allMonitoredOperational;
   CO_CANmodule_t         *CANdevRx;
   UNSIGNED16              CANdevRxIdxStart;
}CO_HBconsumer_t;


/*******************************************************************************
   Function: CO_HBcons_receive

   Read received message from CAN module.

   Function will be called (by CAN receive interrupt) every time, when CAN
   message with correct identifier will be received. For more information and
   description of parameters see topic <Receive CAN message function>.
*******************************************************************************/
INTEGER16 CO_HBcons_receive(void *object, CO_CANrxMsg_t *msg);


/*******************************************************************************
   Function: CO_ODF_1016

   Function for accessing _Consumer Heartbeat Time_ (index 0x1016) from SDO server.

   For more information see topic <SDO server access function> in CO_SDO.h file.
*******************************************************************************/
UNSIGNED32 CO_ODF_1016( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED16 *pLength,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData);


/*******************************************************************************
   Function: CO_HBconsumer_init

   Initialize Heartbeat consumer object.

   Function must be called in the communication reset section.

   Parameters:
      ppHBcons                      - Pointer to address of HBconsumer object <CO_HBconsumer_t>.
                                      If address is zero, memory for new object will be
                                      allocated and address will be set.
      HBcons                        - Pointer to Heartbeat consumer object <CO_HBconsumer_t>.
      EM                            - Pointer to Emergency object <CO_emergencyReport_t>.
      SDO                           - Pointer to SDO object <CO_SDO_t>.
      ObjDict_consumerHeartbeatTime - Pointer to _Consumer Heartbeat Time_ array
                                      from Object Dictionary (index 0x1016). Size
                                      of array is equal to numberOfMonitoredNodes.
      monitoredNodes                - Pointer to externaly defined array of
                                      <CO_HBconsNode_t> objects. Size of array
                                      is equal to numberOfMonitoredNodes.
      numberOfMonitoredNodes        - Number of monitored nodes.
      CANdevRx                      - CAN device for Heartbeat reception <CO_CANmodule_t>.
      CANdevRxIdxStart              - Starting index of receive buffer for Heartbeat
                                      message reception. Number of used indexes
                                      is equal to numberOfMonitoredNodes.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
      CO_ERROR_OUT_OF_MEMORY      - Memory allocation failed.
*******************************************************************************/
INTEGER16 CO_HBconsumer_init(
      CO_HBconsumer_t       **ppHBcons,
      CO_emergencyReport_t   *EM,
      CO_SDO_t               *SDO,
const UNSIGNED32             *ObjDict_consumerHeartbeatTime,
      UNSIGNED8               numberOfMonitoredNodes,
      CO_CANmodule_t *CANdevRx, UNSIGNED16 CANdevRxIdxStart);


/*******************************************************************************
   Function: CO_HBconsumer_delete

   Delete HBconsumer object and free memory.

   Parameters:
      ppHBcons       - Pointer to pointer to HBconsumer object <CO_HBconsumer_t>.
                       Pointer to HBconsumer object is set to 0.
*******************************************************************************/
void CO_HBconsumer_delete(CO_HBconsumer_t **ppHBcons);


/*******************************************************************************
   Function: CO_HBconsumer_process

   Process Heartbeat consumer.

   Function must be called cyclically.

   Parameters:
      HBcons                   - Pointer to Heartbeat consumer object <CO_HBconsumer_t>.
      NMTisPreOrOperational    - Different than zero, if <NMT internal State> is
                                 NMT_PRE_OPERATIONAL or NMT_OPERATIONAL.
      timeDifference_ms        - Time difference from previous function call in [milliseconds].
*******************************************************************************/
void CO_HBconsumer_process(CO_HBconsumer_t     *HBcons,
                           UNSIGNED8            NMTisPreOrOperational,
                           UNSIGNED16           timeDifference_ms);


#endif
