/*
 * CAN module object for generic microcontroller.
 *
 * This file is a template for other microcontrollers.
 *
 * @file        CO_driver.c
 * @ingroup     CO_driver
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


/******************************************************************************/
#ifdef BIG_ENDIAN
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
#else
void memcpySwap2(uint8_t* dest, uint8_t* src){
    *(dest++) = *(src++);
    *(dest) = *(src);
}

void memcpySwap4(uint8_t* dest, uint8_t* src){
    *(dest++) = *(src++);
    *(dest++) = *(src++);
    *(dest++) = *(src++);
    *(dest) = *(src);
}
#endif


/******************************************************************************/
void CO_CANsetConfigurationMode(uint16_t CANbaseAddress){
}


/******************************************************************************/
void CO_CANsetNormalMode(uint16_t CANbaseAddress){
}


/******************************************************************************/
int16_t CO_CANmodule_init(
        CO_CANmodule_t         *CANmodule,
        uint16_t                CANbaseAddress,
        CO_CANrx_t             *rxArray,
        uint16_t                rxSize,
        CO_CANtx_t             *txArray,
        uint16_t                txSize,
        uint16_t                CANbitRate)
{
    uint16_t i;

    /* Configure object variables */
    CANmodule->CANbaseAddress = CANbaseAddress;
    CANmodule->rxArray = rxArray;
    CANmodule->rxSize = rxSize;
    CANmodule->txArray = txArray;
    CANmodule->txSize = txSize;
    CANmodule->curentSyncTimeIsInsideWindow = 0;
    CANmodule->useCANrxFilters = (rxSize <= 32) ? 1 : 0;/* microcontroller dependent */
    CANmodule->bufferInhibitFlag = 0;
    CANmodule->firstCANtxMessage = 1;
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


    /* Configure CAN module registers */


    /* Configure CAN timing */


    /* Configure CAN module hardware filters */
    if(CANmodule->useCANrxFilters){
        /* CAN module filters are used, they will be configured with */
        /* CO_CANrxBufferInit() functions, called by separate CANopen */
        /* init functions. */
        /* Configure all masks so, that received message must match filter */
    }
    else{
        /* CAN module filters are not used, all messages with standard 11-bit */
        /* identifier will be received */
        /* Configure mask 0 so, that all messages with standard identifier are accepted */
    }


    /* configure CAN interrupt registers */


    return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANmodule_disable(CO_CANmodule_t *CANmodule){
    CO_CANsetConfigurationMode(CANmodule->CANbaseAddress);
}


/******************************************************************************/
uint16_t CO_CANrxMsg_readIdent(CO_CANrxMsg_t *rxMsg){
    return rxMsg->ident;
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
    int16_t ret = CO_ERROR_NO;

    /* safety */
    if(!CANmodule || !object || !pFunct || index >= CANmodule->rxSize){
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    /* buffer, which will be configured */
    rxBuffer = CANmodule->rxArray + index;

    /* Configure object variables */
    rxBuffer->object = object;
    rxBuffer->pFunct = pFunct;

    /* CAN identifier and CAN mask, bit aligned with CAN module. Different on different microcontrollers. */
    rxBuffer->ident = ident & 0x07FF;
    if(rtr) rxBuffer->ident |= 0x0800;
    rxBuffer->mask = (mask & 0x07FF) | 0x0800;

    /* Set CAN hardware module filter and mask. */
    if(CANmodule->useCANrxFilters){

    }

    return ret;
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

    /* CAN identifier, DLC and rtr, bit aligned with CAN module transmit buffer.
     * Microcontroller specific. */
    buffer->ident = (ident & 0x07FF) | ((noOfBytes & 0xF)<<12) | (rtr?0x8000:0);

    buffer->bufferFull = 0;
    buffer->syncFlag = syncFlag ? 1 : 0;

    return buffer;
}


/******************************************************************************/
int16_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer){

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

    DISABLE_INTERRUPTS();
    /* if CAN TB buffer is free, copy message to it */
    if(1 && CANmodule->CANtxCount == 0){
    }
    /* if no buffer is free, message will be sent by interrupt */
    else{
        buffer->bufferFull = 1;
        CANmodule->CANtxCount++;
        /* Enable 'Tx buffer empty' interrupt */
    }
    ENABLE_INTERRUPTS();

    return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule){

    DISABLE_INTERRUPTS();
    if(CANmodule->bufferInhibitFlag){
        /* clear TXREQ */
        ENABLE_INTERRUPTS();
        CO_errorReport((CO_EM_t*)CANmodule->EM, ERROR_TPDO_OUTSIDE_WINDOW, 1);
    }
    else{
        ENABLE_INTERRUPTS();
    }
}


/******************************************************************************/
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule){
    uint16_t rxErrors, txErrors, overflow;
    CO_EM_t* EM = (CO_EM_t*)CANmodule->EM;

    /* get error counters from module. Id possible, function may use different way to
     * determine errors. */
    rxErrors = 0;
    txErrors = 0;
    overflow = 0;

    uint32_t err = (uint32_t)txErrors << 16 | rxErrors << 8 | overflow;

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
                CO_errorReset(EM, ERROR_CAN_BUS_WARNING, err);
            }
        }

        if(overflow){                       /* CAN RX bus overflow */
            CO_errorReport(EM, ERROR_CAN_RXB_OVERFLOW, err);
        }
    }
}


/******************************************************************************/
void CO_CANinterrupt(CO_CANmodule_t *CANmodule){

    /* receive interrupt */
    if(1){
        CO_CANrxMsg_t *rcvMsg;  /* pointer to received message in CAN module */
        uint16_t index;         /* index of received message */
        uint16_t rcvMsgIdent;   /* identifier of the received message */
        CO_CANrx_t *msgBuff;    /* receive message buffer from CO_CANmodule_t object. */
        uint8_t msgMatched = 0;

        rcvMsg = 0; /* get message from module here */
        rcvMsgIdent = rcvMsg->ident;
        if(CANmodule->useCANrxFilters){
            /* CAN module filters are used. Message with known 11-bit identifier has */
            /* been received */
            index = 0;  /* get index of the received message here. Or something similar */
            msgBuff = CANmodule->rxArray + index;
            /* verify also RTR */
            if(((rcvMsgIdent ^ msgBuff->ident) & msgBuff->mask) == 0)
                msgMatched = 1;
        }
        else{
            /* CAN module filters are not used, message with any standard 11-bit identifier */
            /* has been received. Search rxArray form CANmodule for the same CAN-ID. */
            msgBuff = CANmodule->rxArray;
            for(index = CANmodule->rxSize; index > 0; index--){
                if(((rcvMsgIdent ^ msgBuff->ident) & msgBuff->mask) == 0){
                    msgMatched = 1;
                    break;
                }
                msgBuff++;
            }
        }

        /* Call specific function, which will process the message */
        if(msgMatched) msgBuff->pFunct(msgBuff->object, rcvMsg);

        /* Clear interrupt flag */
    }


    /* transmit interrupt */
    else if(0){
        /* First CAN message (bootup) was sent successfully */
        CANmodule->firstCANtxMessage = 0;
        /* clear flag from previous message */
        CANmodule->bufferInhibitFlag = 0;
        /* Are there any new messages waiting to be send and buffer is free */
        if(CANmodule->CANtxCount > 0){
            uint16_t index;          /* index of transmitting message */
            CANmodule->CANtxCount--;

            /* first buffer */
            CO_CANtx_t *buffer = CANmodule->txArray;
            /* search through whole array of pointers to transmit message buffers. */
            for(index = CANmodule->txSize; index > 0; index--){
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


                    /* release buffer */
                    buffer->bufferFull = 0;
                    /* exit for loop */
                    break;
                }
                buffer++;
            }/* end of for loop */
        }
        else{
            /* if no more messages, disable 'Tx buffer empty' interrupt */
        }
        /* Clear interrupt flag */
    }
}
