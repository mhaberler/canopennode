/*
 * CANopen Emergency object.
 *
 * @file        CO_Emergency.c
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


#include <string.h> /* for memcpy */

#include "CO_driver.h"
#include "CO_SDO.h"
#include "CO_Emergency.h"


/*
 * Function for accessing _Pre-Defined Error Field_ (index 0x1003) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
static uint32_t CO_ODF_1003(CO_ODF_arg_t *ODF_arg){
    CO_EMpr_t *EMpr;
    uint8_t *value;

    EMpr = (CO_EMpr_t*) ODF_arg->object;
    value = (uint8_t*) ODF_arg->data;

    if(ODF_arg->reading){
        uint8_t noOfErrors;
        noOfErrors = EMpr->preDefErrNoOfErrors;

        if(ODF_arg->subIndex == 0)
            *value = noOfErrors;
        else if(ODF_arg->subIndex > noOfErrors)
            return SDO_ABORT_NO_DATA;  /* No data available. */
    }
    else{
        /* only '0' may be written to subIndex 0 */
        if(ODF_arg->subIndex == 0){
            if(*value == 0)
                EMpr->preDefErrNoOfErrors = 0;
            else
                return SDO_ABORT_INVALID_VALUE;  /* Invalid value for parameter */
        }
        else
            return SDO_ABORT_READONLY;  /* Attempt to write a read only object. */
    }

    return 0;
}


/*
 * Function for accessing _COB ID EMCY_ (index 0x1014) from SDO server.
 *
 * For more information see file CO_SDO.h.
 */
static uint32_t CO_ODF_1014(CO_ODF_arg_t *ODF_arg){
    uint8_t *nodeId;
    uint32_t *value;

    nodeId = (uint8_t*) ODF_arg->object;
    value = (uint32_t*) ODF_arg->data;

    /* add nodeId to the value */
    if(ODF_arg->reading)
        *value += *nodeId;

    return 0;
}


/******************************************************************************/
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
        uint16_t                CANidTxEM)
{
    uint8_t i;

    /* Configure object variables */
    EM->errorStatusBits         = errorStatusBits;
    EM->errorStatusBitsSize     = errorStatusBitsSize; if(errorStatusBitsSize < 6) return CO_ERROR_ILLEGAL_ARGUMENT;
    EM->bufEnd                  = EM->buf + CO_EM_INTERNAL_BUFFER_SIZE * 8;
    EM->bufWritePtr             = EM->buf;
    EM->bufReadPtr              = EM->buf;
    EM->bufFull                 = 0;
    EM->wrongErrorReport        = 0;
    EMpr->EM                    = EM;
    EMpr->errorRegister         = errorRegister;
    EMpr->preDefErr             = preDefErr;
    EMpr->preDefErrSize         = preDefErrSize;
    EMpr->preDefErrNoOfErrors   = 0;
    EMpr->inhibitEmTimer        = 0;

    /* clear error status bits */
    for(i=0; i<errorStatusBitsSize; i++) EM->errorStatusBits[i] = 0;

    /* Configure Object dictionary entry at index 0x1003 and 0x1014 */
    CO_OD_configure(SDO, OD_H1003_PREDEF_ERR_FIELD, CO_ODF_1003, (void*)EMpr, 0, 0);
    CO_OD_configure(SDO, OD_H1014_COBID_EMERGENCY, CO_ODF_1014, (void*)&SDO->nodeId, 0, 0);

    /* configure emergency message CAN transmission */
    EMpr->CANdev = CANdev;
    EMpr->CANdev->EM = (void*)EM; /* update pointer inside CAN device. */
    EMpr->CANtxBuff = CO_CANtxBufferInit(
            CANdev,             /* CAN device */
            CANdevTxIdx,        /* index of specific buffer inside CAN module */
            CANidTxEM,          /* CAN identifier */
            0,                  /* rtr */
            8,                  /* number of data bytes */
            0);                 /* synchronous message flag bit */

    return CO_ERROR_NO;
}


/******************************************************************************/
void CO_EM_process(
        CO_EMpr_t              *EMpr,
        uint8_t                 NMTisPreOrOperational,
        uint16_t                timeDifference_100us,
        uint16_t                EMinhTime)
{

    CO_EM_t *EM = EMpr->EM;
    uint8_t errorRegister;

    /* verify errors from driver and other */
    CO_CANverifyErrors(EMpr->CANdev);
    if(EM->wrongErrorReport){
        CO_errorReport(EM, CO_EM_WRONG_ERROR_REPORT, CO_EMC_SOFTWARE_INTERNAL, EM->wrongErrorReport);
        EM->wrongErrorReport = 0;
    }


    /* calculate Error register */
    errorRegister = 0;
    /* generic error */
    if(EM->errorStatusBits[5])
        errorRegister |= CO_ERR_REG_GENERIC_ERR;
    /* communication error (overrun, error state) */
    if(EM->errorStatusBits[2] || EM->errorStatusBits[3])
        errorRegister |= CO_ERR_REG_COMM_ERR;
    *EMpr->errorRegister = (*EMpr->errorRegister & 0xEE) | errorRegister;

    /* inhibit time */
    if(EMpr->inhibitEmTimer < EMinhTime) EMpr->inhibitEmTimer += timeDifference_100us;

    /* send Emergency message. */
    if(     NMTisPreOrOperational &&
            !EMpr->CANtxBuff->bufferFull &&
            EMpr->inhibitEmTimer >= EMinhTime &&
            (EM->bufReadPtr != EM->bufWritePtr || EM->bufFull))
    {
        uint32_t preDEF;    /* preDefinedErrorField */
        
        /* add error register */
        EM->bufReadPtr[2] = *EMpr->errorRegister;

        /* copy data to CAN emergency message */
        memcpy((void*)EMpr->CANtxBuff->data, (void*)EM->bufReadPtr, 8);
        memcpy((void*)&preDEF, (void*)EM->bufReadPtr, 4);
        EM->bufReadPtr += 8;

        /* Update read buffer pointer and reset inhibit timer */
        if(EM->bufReadPtr == EM->bufEnd) EM->bufReadPtr = EM->buf;
        EMpr->inhibitEmTimer = 0;

        /* verify message buffer overflow, then clear full flag */
        if(EM->bufFull == 2){
            EM->bufFull = 0;    /* will be updated below */
            CO_errorReport(EM, CO_EM_EMERGENCY_BUFFER_FULL, CO_EMC_GENERIC, 0);
        }
        else{
            EM->bufFull = 0;
        }

        /* write to 'pre-defined error field' (object dictionary, index 0x1003) */
        if(EMpr->preDefErr){
            uint8_t i;

            if(EMpr->preDefErrNoOfErrors < EMpr->preDefErrSize)
                EMpr->preDefErrNoOfErrors++;
            for(i=EMpr->preDefErrNoOfErrors-1; i>0; i--)
                EMpr->preDefErr[i] = EMpr->preDefErr[i-1];
            EMpr->preDefErr[0] = preDEF;
        }

        /* send CAN message */
        CO_CANsend(EMpr->CANdev, EMpr->CANtxBuff);
    }

    return;
}


/******************************************************************************/
int8_t CO_errorReport(CO_EM_t *EM, uint8_t errorBit, uint16_t errorCode, uint32_t infoCode){
    uint8_t index = errorBit >> 3;
    uint8_t bitmask = 1 << (errorBit & 0x7);
    uint8_t *errorStatusBits = &EM->errorStatusBits[index];
    uint8_t bufCopy[8];

    if(!EM) return -1;

    /* if errorBit value not supported, send emergency 'CO_EM_WRONG_ERROR_REPORT' */
    if(index >= EM->errorStatusBitsSize){
        EM->wrongErrorReport = errorBit;
        return -1;
    }

    /* if error was allready reported, return */
    if((*errorStatusBits & bitmask) != 0) return 0;

    /* set error bit */
    if(errorBit) *errorStatusBits |= bitmask; /* any error except NO_ERROR */

    /* verify buffer full, set overflow */
    if(EM->bufFull){
        EM->bufFull = 2;
        return -2;
    }

    /* prepare data for emergency message */
    memcpySwap2(&bufCopy[0], (uint8_t*)&errorCode);
    bufCopy[2] = 0; /* error register will be set later */
    bufCopy[3] = errorBit;
    memcpySwap4(&bufCopy[4], (uint8_t*)&infoCode);

    /* copy data to the buffer, increment writePtr and verify buffer full */
    DISABLE_INTERRUPTS();
    memcpy((void*)EM->bufWritePtr, (void*)&bufCopy[0], 8);
    EM->bufWritePtr += 8;

    if(EM->bufWritePtr == EM->bufEnd) EM->bufWritePtr = EM->buf;
    if(EM->bufWritePtr == EM->bufReadPtr) EM->bufFull = 1;
    ENABLE_INTERRUPTS();

    return 1;
}


/******************************************************************************/
int8_t CO_errorReset(CO_EM_t *EM, uint8_t errorBit, uint32_t infoCode){
    uint8_t index = errorBit >> 3;
    uint8_t bitmask = 1 << (errorBit & 0x7);
    uint8_t *errorStatusBits = &EM->errorStatusBits[index];
    uint8_t bufCopy[8];

    if(!EM) return -1;

    /* if errorBit value not supported, send emergency 'CO_EM_WRONG_ERROR_REPORT' */
    if(index >= EM->errorStatusBitsSize){
        EM->wrongErrorReport = errorBit;
        return -1;
    }

    /* if error is allready cleared, return */
    if((*errorStatusBits & bitmask) == 0) return 0;

    /* erase error bit */
    *errorStatusBits &= ~bitmask;

    /* verify buffer full */
    if(EM->bufFull){
        EM->bufFull = 2;
        return -2;
    }

    /* prepare data for emergency message */
    bufCopy[0] = 0;
    bufCopy[1] = 0;
    bufCopy[2] = 0; /* error register will be set later */
    bufCopy[3] = errorBit;
    memcpySwap4(&bufCopy[4], (uint8_t*)&infoCode);

    /* copy data to the buffer, increment writePtr and verify buffer full */
    DISABLE_INTERRUPTS();
    memcpy((void*)EM->bufWritePtr, (void*)&bufCopy[0], 8);
    EM->bufWritePtr += 8;

    if(EM->bufWritePtr == EM->bufEnd) EM->bufWritePtr = EM->buf;
    if(EM->bufWritePtr == EM->bufReadPtr) EM->bufFull = 1;
    ENABLE_INTERRUPTS();

    return 1;
}


/******************************************************************************/
int8_t CO_isError(CO_EM_t *EM, uint8_t errorBit){
    uint8_t index = errorBit >> 3;
    uint8_t bitmask = 1 << (errorBit & 0x7);

    if((EM->errorStatusBits[index] & bitmask)) return 1;

    return 0;
}
