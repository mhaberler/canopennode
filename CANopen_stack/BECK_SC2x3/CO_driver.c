/*
 * CAN module object for BECK SC243 computer.
 *
 * @file        CO_driver.c
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


#include "CO_driver.h"
#include "CO_Emergency.h"

#include <stdlib.h>     /* for malloc, free */


int CAN1callback(CanEvent event, const CanMsg *msg);
int CAN2callback(CanEvent event, const CanMsg *msg);


/******************************************************************************/
void memcpySwap2(uint8_t* dest, uint8_t* src){
    *(dest++) = *(src+1);
    *(dest) = *(src);
}

void memcpySwap4(uint8_t* dest, uint8_t* src){
    src += 3;
    *(dest++) = *(src--);
    *(dest++) = *(src--);
    *(dest++) = *(src--);
    *(dest) = *(src);
}


/******************************************************************************/
void CO_CANsetConfigurationMode(uint16_t CANbaseAddress){
    canEnableRx(CANbaseAddress, FALSE);
}


/******************************************************************************/
void CO_CANsetNormalMode(uint16_t CANbaseAddress){
    canEnableRx(CANbaseAddress, TRUE);
}


/******************************************************************************/
int16_t CO_CANmodule_init(
        CO_CANmodule_t        **ppCANmodule,
        uint16_t                CANbaseAddress,
        uint16_t                rxSize,
        uint16_t                txSize,
        uint16_t                CANbitRate)
{
    uint16_t i;

    /* allocate memory if not already allocated */
    if((*ppCANmodule) == NULL){
        if(((*ppCANmodule)          = (CO_CANmodule_t *) malloc(       sizeof(CO_CANmodule_t ))) == NULL){                                                                   return CO_ERROR_OUT_OF_MEMORY;}
        if(((*ppCANmodule)->rxArray = (CO_CANrx_t *)malloc(rxSize*sizeof(CO_CANrx_t))) == NULL){                               free(*ppCANmodule); *ppCANmodule=0; return CO_ERROR_OUT_OF_MEMORY;}
        if(((*ppCANmodule)->txArray = (CO_CANtx_t *)malloc(txSize*sizeof(CO_CANtx_t))) == NULL){free((*ppCANmodule)->rxArray); free(*ppCANmodule); *ppCANmodule=0; return CO_ERROR_OUT_OF_MEMORY;}
    }
    else if((*ppCANmodule)->rxArray == NULL || (*ppCANmodule)->txArray == NULL) return CO_ERROR_ILLEGAL_ARGUMENT;

    CO_CANmodule_t *CANmodule = *ppCANmodule; /* pointer to (newly created) object */

    /* Configure object variables */
    CANmodule->CANbaseAddress = CANbaseAddress;
    CANmodule->rxSize = rxSize;
    CANmodule->txSize = txSize;
    CANmodule->curentSyncTimeIsInsideWindow = 0;
    CANmodule->bufferInhibitFlag = 0;
    CANmodule->firstCANtxMessage = 1;
    CANmodule->error = 0;
    CANmodule->CANtxCount = 0;
    CANmodule->errOld = 0;
    CANmodule->EM = 0;
    for(i=0; i<rxSize; i++){
        CANmodule->rxArray[i].ident = 0;
        CANmodule->rxArray[i].pFunct = 0;
    }
    for(i=0; i<txSize; i++){
        CANmodule->txArray[i].bufferFull = 0;
    }

    /* initialize port */
    CanError e = canInit(CANbaseAddress, CANbitRate, 0);
    if(e == CAN_ERROR_ILLEGAL_BAUDRATE)
        e = canInit(CANbaseAddress, 125, 0);
    switch(e){
        case CO_ERROR_NO: break;
        case CAN_ERROR_OUT_OF_MEMORY: return CO_ERROR_OUT_OF_MEMORY;
        default: return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Set acceptance filters to accept all messages with standard identifier, also accept rtr */
    CanFilterSc2x3 filter;
    filter.controllerType         = CAN_FILTER_CONTROLLER_TYPE_SC2X3;
    filter.structVer              = 1;
    filter.mode                   = CAN_FILTER_SC2X3_MODE_32_BIT;
    filter.filters.f32[0].idMask  = canEncodeId(0x7FF, FALSE, TRUE);
    filter.filters.f32[0].idValue = canEncodeId(0x7FF, FALSE, FALSE);
    switch(canSetFilter(CANbaseAddress,  (CanFilter *)&filter)){
        case CO_ERROR_NO: break;
        default: return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    /* purge buffers */
    canEnableRx(CANbaseAddress, FALSE);
    canPurgeRx(CANbaseAddress);
    canPurgeTx(CANbaseAddress, FALSE);

    /* register callback function */
    if(CANbaseAddress == CAN_PORT_CAN1)
        canRegisterCallback(CANbaseAddress, CAN1callback, (1 << CAN_EVENT_RX) | (1 << CAN_EVENT_TX) | (1 << CAN_EVENT_BUS_OFF) | (1 << CAN_EVENT_OVERRUN));
    else if(CANbaseAddress == CAN_PORT_CAN2)
        canRegisterCallback(CANbaseAddress, CAN2callback, (1 << CAN_EVENT_RX) | (1 << CAN_EVENT_TX) | (1 << CAN_EVENT_BUS_OFF) | (1 << CAN_EVENT_OVERRUN));

    return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANmodule_delete(CO_CANmodule_t** ppCANmodule){
    if(*ppCANmodule){
        canDeinit((*ppCANmodule)->CANbaseAddress);
        free((*ppCANmodule)->txArray);
        free((*ppCANmodule)->rxArray);
        free(*ppCANmodule);
        *ppCANmodule = 0;
    }
}


/******************************************************************************/
uint16_t CO_CANrxMsg_readIdent(CO_CANrxMsg_t *rxMsg){
    return (uint16_t) canDecodeId(rxMsg->ident);
}


/******************************************************************************/
int16_t CO_CANrxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        uint16_t                mask,
        uint8_t                 rtr,
        void                   *object,
        int16_t               (*pFunct)(void *object, CO_CANrxMsg_t *message))
{
    CO_CANrx_t *rxBuffer;

    /* safety */
    if(!CANmodule || !object || !pFunct || index >= CANmodule->rxSize){
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }


    /* buffer, which will be configured */
    rxBuffer = CANmodule->rxArray + index;

    /* Configure object variables */
    rxBuffer->object = object;
    rxBuffer->pFunct = pFunct;

    /* Configure CAN identifier and CAN mask, bit aligned with CAN module. */
    /* No hardware filtering is used. */
    rxBuffer->ident = canEncodeId(ident, FALSE, rtr);
    rxBuffer->mask = canEncodeId(mask, FALSE, FALSE);

    return CO_ERROR_NO;
}


/******************************************************************************/
CO_CANtx_t *CO_CANtxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        uint8_t                 rtr,
        uint8_t                 noOfBytes,
        uint8_t                 syncFlag)
{
    /* safety */
    if(!CANmodule || CANmodule->txSize <= index) return 0;

    /* get specific buffer */
    CO_CANtx_t *buffer = &CANmodule->txArray[index];

    /* CAN identifier, bit aligned with CAN module registers */
    buffer->ident = canEncodeId(ident, FALSE, rtr);

    buffer->DLC = noOfBytes;
    buffer->bufferFull = 0;
    buffer->syncFlag = syncFlag?1:0;

    return buffer;
}


/******************************************************************************/
int16_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t  *buffer){

    /* Was previous message sent or it is still waiting? */
    if(buffer->bufferFull){
        if(!CANmodule->firstCANtxMessage)/* don't set error, if bootup message is still on buffers */
            CO_errorReport((CO_EM_t*)CANmodule->EM, ERROR_CAN_TX_OVERFLOW, 0);
        return CO_ERROR_TX_OVERFLOW;
    }

    /* messages with syncFlag set (synchronous PDOs) must be transmited inside preset time window */
    if(CANmodule->curentSyncTimeIsInsideWindow && buffer->syncFlag && !(*CANmodule->curentSyncTimeIsInsideWindow)){
        CO_errorReport((CO_EM_t*)CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 0);
        return CO_ERROR_TX_PDO_WINDOW;
    }

    /* send message */
    CanError err;
    DISABLE_INTERRUPTS();
    err = canSend(CANmodule->CANbaseAddress, (const CanMsg*) buffer, FALSE);
    /* if no buffer is free, message will be sent by interrupt */
    if(err == CAN_ERROR_QUEUE_MAX){
        buffer->bufferFull = 1;
        CANmodule->CANtxCount++;
    }
    ENABLE_INTERRUPTS();

#ifdef CO_LOG_CAN_MESSAGES
    void CO_logMessage(const CanMsg *msg);
    CO_logMessage((const CanMsg*) buffer);
#endif

    return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule){

    DISABLE_INTERRUPTS();
    if(CANmodule->bufferInhibitFlag){
        canPurgeTx(CANmodule->CANbaseAddress, FALSE);
        ENABLE_INTERRUPTS();
        CO_errorReport((CO_EM_t*)CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 1);
    }
    else{
        ENABLE_INTERRUPTS();
    }
}


/******************************************************************************/
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule){
    unsigned rxErrors, txErrors;
    CO_EM_t* EM = (CO_EM_t*)CANmodule->EM;

    canGetErrorCounters(CANmodule->CANbaseAddress, &rxErrors, &txErrors);
    if(txErrors > 0xFFFF) txErrors = 0xFFFF;
    if(rxErrors > 0xFF) rxErrors = 0xFF;
    uint32_t err = (uint32_t)txErrors << 16 | rxErrors << 8 | CANmodule->error;

    if(CANmodule->errOld != err){
        CANmodule->errOld = err;

        if(txErrors >= 256){                               /* bus off */
            CO_errorReport(EM, ERROR_CAN_TX_BUS_OFF, err);
        }
        else{                                              /* not bus off */
            CO_errorReset(EM, ERROR_CAN_TX_BUS_OFF, err);

            if(rxErrors >= 96 || txErrors >= 96){           /* bus warning */
                CO_errorReport(EM, ERROR_CAN_BUS_WARNING, err);
            }

            if(rxErrors >= 128){                            /* RX bus passive */
                CO_errorReport(EM, ERROR_CAN_RX_BUS_PASSIVE, err);
            }
            else{
                CO_errorReset(EM, ERROR_CAN_RX_BUS_PASSIVE, err);
            }

            if(txErrors >= 128){                            /* TX bus passive */
                if(!CANmodule->firstCANtxMessage){
                    CO_errorReport(EM, ERROR_CAN_TX_BUS_PASSIVE, err);
                }
            }
            else{
                int16_t wasCleared;
                wasCleared =        CO_errorReset(EM, ERROR_CAN_TX_BUS_PASSIVE, err);
                if(wasCleared == 1) CO_errorReset(EM, ERROR_CAN_TX_OVERFLOW, err);
            }

            if(rxErrors < 96 && txErrors < 96){             /* no error */
                int16_t wasCleared;
                wasCleared =        CO_errorReset(EM, ERROR_CAN_BUS_WARNING, err);
                if(wasCleared == 1) CO_errorReset(EM, ERROR_CAN_TX_OVERFLOW, err);
            }
        }

        if(CANmodule->error & 0x02){                       /* CAN RX bus overflow */
            CO_errorReport(EM, ERROR_CAN_RXB_OVERFLOW, err);
        }
    }

}


/******************************************************************************/
int CO_CANinterrupt(CO_CANmodule_t *CANmodule, CanEvent event, const CanMsg *msg){

    /* receive interrupt (New CAN messagge is available in RX FIFO buffer) */
    if(event == CAN_EVENT_RX){
        CO_CANrxMsg_t *rcvMsg;     /* pointer to received message in CAN module */
        uint16_t index;          /* index of received message */
        CO_CANrx_t *msgBuff;  /* receive message buffer from CO_CANmodule_t object. */
        uint8_t msgMatched = 0;

        rcvMsg = (CO_CANrxMsg_t *) msg;  /* structures are aligned */
        /* CAN module filters are not used, message with any standard 11-bit identifier */
        /* has been received. Search rxArray form CANmodule for the same CAN-ID. */
        msgBuff = CANmodule->rxArray;
        for(index = 0; index < CANmodule->rxSize; index++){
            if(((rcvMsg->ident ^ msgBuff->ident) & msgBuff->mask) == 0){
                msgMatched = 1;
                break;
            }
            msgBuff++;
        }

        /* Call specific function, which will process the message */
        if(msgMatched) msgBuff->pFunct(msgBuff->object, rcvMsg);

#ifdef CO_LOG_CAN_MESSAGES
        void CO_logMessage(const CanMsg *msg);
        CO_logMessage(msg);
#endif

        return 0;
    }

    /* transmit interrupt (TX buffer is free) */
    if(event == CAN_EVENT_TX){
        /* First CAN message (bootup) was sent successfully */
        CANmodule->firstCANtxMessage = 0;
        /* clear flag from previous message */
        CANmodule->bufferInhibitFlag = 0;
        /* Are there any new messages waiting to be send and buffer is free */
        if(CANmodule->CANtxCount > 0){
            uint16_t index;          /* index of transmitting message */
            CANmodule->CANtxCount--;
            /* search through whole array of pointers to transmit message buffers. */
            for(index = 0; index < CANmodule->txSize; index++){
                /* get specific buffer */
                CO_CANtx_t *buffer = &CANmodule->txArray[index];
                /* if message buffer is full, send it. */
                if(buffer->bufferFull){
                    /* messages with syncFlag set (synchronous PDOs) must be transmited inside preset time window */
                    if(CANmodule->curentSyncTimeIsInsideWindow && buffer->syncFlag){
                        if(!(*CANmodule->curentSyncTimeIsInsideWindow)){
                            CO_errorReport((CO_EM_t*)CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 2);
                            /* release buffer */
                            buffer->bufferFull = 0;
                            /* exit for loop */
                            break;
                        }
                        CANmodule->bufferInhibitFlag = 1;
                    }

                    /* Copy message to CAN buffer */
                    canSend(CANmodule->CANbaseAddress, (const CanMsg*) buffer, FALSE);

                    /* release buffer */
                    buffer->bufferFull = 0;
                    /* exit for loop */
                    break;
                }
            }/* end of for loop */
        }
        return 0;
    }

    if(event == CAN_EVENT_BUS_OFF){
        CANmodule->error |= 0x01;
        return 0;
    }

    if(event == CAN_EVENT_OVERRUN){
        CANmodule->error |= 0x02;
        return 0;
    }
    return 0;
}
