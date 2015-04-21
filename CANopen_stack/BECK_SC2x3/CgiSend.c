/*
 * Object for CGI function, which can send any CANopen message over the CANopen
 * network.
 *
 * @file        CgiSend.c
 * @ingroup     CgiSend
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
 * the Free Software Foundation, either version 2.1 of the License, or
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


#include "CgiSend.h"
#include "CANopen.h"

#include <string.h> /*  for memcpy */

#if CO_NO_NMT_MASTER != 1
    #error NMT master is not defined in Object Dictionary
#endif


/**
 * Decodes hex string and returns numerical value.
 *
 * @param str Pointer to string.
 * @param len Length of string.
 * @param val Value (return).
 *
 * @return 0: Operation completed successfully.
 * @return -1: Error in string.
 */
static int32_t hex2dec(char_t *str, int32_t len, uint32_t *val){
    *val = 0;
    int32_t i;
    for(i=0; i<len; i++){
        char_t c = str[i];
        if     (c>='A' && c<='F') c = c - ('A' - 10);
        else if(c>='a' && c<='f') c = c - ('a' - 10);
        else if(c>='0' && c<='9') c = c - '0';
        else return 1;
        *val = *val<<4 | c;
    }
    return 0;
}


/******************************************************************************/
static void huge _pascal CgiSendFunction(rpCgiPtr CgiRequest){
    /* Buffers for building the page */
    extern CgiSend_t *CgiSend;
    extern CO_t *CO;
    char_t *buf = CgiSend->buf;   /* buffer for CGI message */

    buf[0] = 0;

    /*  Construct HTML page */
    if(strlen(CgiRequest->fArgumentBufferPtr)){
        char_t far *name = NULL;
        char_t far *val = NULL;
        if(CGI_GetArgument(&name, &val, CgiRequest) == 0){
            /* decode name */
            uint32_t cobId;
            uint8_t data[8];
            uint16_t dataLen = 0;
            char_t err = 0;

            /* cobId */
            if(!name) err++;
            if(strlen(name) != 3) err++;
            err += hex2dec(&name[0], 3, &cobId);
            if(cobId > 0x7FF) err++;

            /* data */
            int32_t i;
            int32_t valLen;
            if(val) valLen = strlen(val);
            else    valLen = 0;
            for(i=0; i<valLen; i=i+2){
                uint32_t dc;
                err += hex2dec(&val[i], 2, &dc);
                data[dataLen] = dc;
                if(++dataLen >= 8) break;
            }

            /* Error in arguments */
            if(err){
                sprintf(buf, "Error in argument: %s=%s", name, val);
            }
            /* send CAN message (NMT master message is used for this purpose) */
            else{
                CO_CANtx_t *CAN_txBuff;
                char_t str2[5];

                CAN_txBuff = CO_CANtxBufferInit(  /* return pointer to 8-byte CAN data buffer, which should be populated */
                                CO->CANmodule[0], /* pointer to CAN module used for sending this message */
                                0,                /* index of specific buffer inside CAN module (CO_TXCAN_NMT has value of 0) */
                                cobId,            /* CAN identifier */
                                0,                /* rtr */
                                dataLen,          /* number of data bytes */
                                0);               /* synchronous message flag bit */

                sprintf(buf, "sent %03X:", (unsigned int)cobId);

                for(i=0; i<dataLen; i++){
                    CAN_txBuff->data[i] = data[i];
                    sprintf(str2, " %02X", data[i]);
                    strcat(buf, str2);
                }

                CO_CANsend(CO->CANmodule[0], CAN_txBuff); /* 0 = success */

                strcat(buf, "\n");

                /* if this is a NMT message, send NMT command also to this node. */
                if(cobId == 0 && dataLen == 2 && (data[1] == 0 || data[1] == CO->NMT->nodeId)){
                    switch(data[0]){
                        case CO_NMT_ENTER_OPERATIONAL:      if(!(*CO->NMT->emPr->errorRegister))
                                                            CO->NMT->operatingState = CO_NMT_OPERATIONAL;    break;
                        case CO_NMT_ENTER_STOPPED:          CO->NMT->operatingState = CO_NMT_STOPPED;           break;
                        case CO_NMT_ENTER_PRE_OPERATIONAL:  CO->NMT->operatingState = CO_NMT_PRE_OPERATIONAL;   break;
                        case CO_NMT_RESET_NODE:             CO->NMT->resetCommand = 2;                          break;
                        case CO_NMT_RESET_COMMUNICATION:    CO->NMT->resetCommand = 1;                          break;
                    }
                }
            }

        }
    }
    else{
        sprintf(buf,
            "CGI function can send any message over the CANopen network.\n\n"
            "Usage:\n"
            "  send?iii=dddd\n"
            "  iii  - 3 digit CAN identifier in hex format from range 000 .. 7FF.\n"
            "  dddd - data in hex format. Two digits for one byte, max 8 bytes.\n");
    }

    /*  Give page to the web server */
    CgiRequest->fHttpResponse = CgiHttpOk;
    CgiRequest->fDataType = CGIDataTypeText;
    CgiRequest->fResponseBufferPtr = buf;
    CgiRequest->fResponseBufferLength = strlen(buf);
}


/******************************************************************************/
int16_t CgiSend_init_1(
        CgiSend_t              *CgiSend,
        char_t                 *buf,
        uint32_t                bufSize)
{
    if(CgiSend == NULL || buf == NULL) return CO_ERROR_ILLEGAL_ARGUMENT;

    /* setup variables */
    CgiSend->buf = buf;
    CgiSend->bufSize = bufSize;

    /* Install CGI function */
    CgiSend->cgiEntry.PathPtr = "send";            /*  Name of the page */
    CgiSend->cgiEntry.method = CgiHttpGet;         /*  HTTP method */
    CgiSend->cgiEntry.CgiFuncPtr = CgiSendFunction;/*  Function called on browser request */

    if(CGI_Install(&CgiSend->cgiEntry) != 0)
        return CO_ERROR_PARAMETERS;

    return CO_ERROR_NO;
}


/******************************************************************************/
void CgiSend_delete(CgiSend_t *CgiSend){
    CGI_Delete(CgiSend->cgiEntry.PathPtr);
}
