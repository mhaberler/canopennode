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
    CO_EMpr_t *emPr;
    uint8_t *value;

    emPr = (CO_EMpr_t*) ODF_arg->object;
    value = (uint8_t*) ODF_arg->data;

    if(ODF_arg->reading){
        uint8_t noOfErrors;
        noOfErrors = emPr->preDefErrNoOfErrors;

        if(ODF_arg->subIndex == 0)
            *value = noOfErrors;
        else if(ODF_arg->subIndex > noOfErrors)
            return SDO_ABORT_NO_DATA;  /* No data available. */
    }
    else{
        /* only '0' may be written to subIndex 0 */
        if(ODF_arg->subIndex == 0){
            if(*value == 0)
                emPr->preDefErrNoOfErrors = 0;
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
        CO_EM_t                *em,
        CO_EMpr_t              *emPr,
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
    em->errorStatusBits         = errorStatusBits;
    em->errorStatusBitsSize     = errorStatusBitsSize; if(errorStatusBitsSize < 6) return CO_ERROR_ILLEGAL_ARGUMENT;
    em->bufEnd                  = em->buf + CO_EM_INTERNAL_BUFFER_SIZE * 8;
    em->bufWritePtr             = em->buf;
    em->bufReadPtr              = em->buf;
    em->bufFull                 = 0;
    em->wrongErrorReport        = 0;
    emPr->em                    = em;
    emPr->errorRegister         = errorRegister;
    emPr->preDefErr             = preDefErr;
    emPr->preDefErrSize         = preDefErrSize;
    emPr->preDefErrNoOfErrors   = 0;
    emPr->inhibitEmTimer        = 0;

    /* clear error status bits */
    for(i=0; i<errorStatusBitsSize; i++) em->errorStatusBits[i] = 0;

    /* Configure Object dictionary entry at index 0x1003 and 0x1014 */
    CO_OD_configure(SDO, OD_H1003_PREDEF_ERR_FIELD, CO_ODF_1003, (void*)emPr, 0, 0);
    CO_OD_configure(SDO, OD_H1014_COBID_EMERGENCY, CO_ODF_1014, (void*)&SDO->nodeId, 0, 0);

    /* configure emergency message CAN transmission */
    emPr->CANdev = CANdev;
    emPr->CANdev->em = (void*)em; /* update pointer inside CAN device. */
    emPr->CANtxBuff = CO_CANtxBufferInit(
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
        CO_EMpr_t              *emPr,
        uint8_t                 NMTisPreOrOperational,
        uint16_t                timeDifference_100us,
        uint16_t                EMinhTime)
{

    CO_EM_t *em = emPr->em;
    uint8_t errorRegister;

    /* verify errors from driver and other */
    CO_CANverifyErrors(emPr->CANdev);
    if(em->wrongErrorReport){
        CO_errorReport(em, CO_EM_WRONG_ERROR_REPORT, CO_EMC_SOFTWARE_INTERNAL, em->wrongErrorReport);
        em->wrongErrorReport = 0;
    }


    /* calculate Error register */
    errorRegister = 0;
    /* generic error */
    if(em->errorStatusBits[5])
        errorRegister |= CO_ERR_REG_GENERIC_ERR;
    /* communication error (overrun, error state) */
    if(em->errorStatusBits[2] || em->errorStatusBits[3])
        errorRegister |= CO_ERR_REG_COMM_ERR;
    *emPr->errorRegister = (*emPr->errorRegister & 0xEE) | errorRegister;

    /* inhibit time */
    if(emPr->inhibitEmTimer < EMinhTime) emPr->inhibitEmTimer += timeDifference_100us;

    /* send Emergency message. */
    if(     NMTisPreOrOperational &&
            !emPr->CANtxBuff->bufferFull &&
            emPr->inhibitEmTimer >= EMinhTime &&
            (em->bufReadPtr != em->bufWritePtr || em->bufFull))
    {
        uint32_t preDEF;    /* preDefinedErrorField */
        
        /* add error register */
        em->bufReadPtr[2] = *emPr->errorRegister;

        /* copy data to CAN emergency message */
        memcpy((void*)emPr->CANtxBuff->data, (void*)em->bufReadPtr, 8);
        memcpy((void*)&preDEF, (void*)em->bufReadPtr, 4);
        em->bufReadPtr += 8;

        /* Update read buffer pointer and reset inhibit timer */
        if(em->bufReadPtr == em->bufEnd) em->bufReadPtr = em->buf;
        emPr->inhibitEmTimer = 0;

        /* verify message buffer overflow, then clear full flag */
        if(em->bufFull == 2){
            em->bufFull = 0;    /* will be updated below */
            CO_errorReport(em, CO_EM_EMERGENCY_BUFFER_FULL, CO_EMC_GENERIC, 0);
        }
        else{
            em->bufFull = 0;
        }

        /* write to 'pre-defined error field' (object dictionary, index 0x1003) */
        if(emPr->preDefErr){
            uint8_t i;

            if(emPr->preDefErrNoOfErrors < emPr->preDefErrSize)
                emPr->preDefErrNoOfErrors++;
            for(i=emPr->preDefErrNoOfErrors-1; i>0; i--)
                emPr->preDefErr[i] = emPr->preDefErr[i-1];
            emPr->preDefErr[0] = preDEF;
        }

        /* send CAN message */
        CO_CANsend(emPr->CANdev, emPr->CANtxBuff);
    }

    return;
}


/******************************************************************************/
int8_t CO_errorReport(CO_EM_t *em, uint8_t errorBit, uint16_t errorCode, uint32_t infoCode){
    uint8_t index = errorBit >> 3;
    uint8_t bitmask = 1 << (errorBit & 0x7);
    uint8_t *errorStatusBits = &em->errorStatusBits[index];
    uint8_t bufCopy[8];

    if(!em) return -1;

    /* if errorBit value not supported, send emergency 'CO_EM_WRONG_ERROR_REPORT' */
    if(index >= em->errorStatusBitsSize){
        em->wrongErrorReport = errorBit;
        return -1;
    }

    /* if error was allready reported, return */
    if((*errorStatusBits & bitmask) != 0) return 0;

    /* set error bit */
    if(errorBit) *errorStatusBits |= bitmask; /* any error except NO_ERROR */

    /* verify buffer full, set overflow */
    if(em->bufFull){
        em->bufFull = 2;
        return -2;
    }

    /* prepare data for emergency message */
    CO_memcpySwap2(&bufCopy[0], (uint8_t*)&errorCode);
    bufCopy[2] = 0; /* error register will be set later */
    bufCopy[3] = errorBit;
    CO_memcpySwap4(&bufCopy[4], (uint8_t*)&infoCode);

    /* copy data to the buffer, increment writePtr and verify buffer full */
    CO_DISABLE_INTERRUPTS();
    memcpy((void*)em->bufWritePtr, (void*)&bufCopy[0], 8);
    em->bufWritePtr += 8;

    if(em->bufWritePtr == em->bufEnd) em->bufWritePtr = em->buf;
    if(em->bufWritePtr == em->bufReadPtr) em->bufFull = 1;
    CO_ENABLE_INTERRUPTS();

    return 1;
}


/******************************************************************************/
int8_t CO_errorReset(CO_EM_t *em, uint8_t errorBit, uint32_t infoCode){
    uint8_t index = errorBit >> 3;
    uint8_t bitmask = 1 << (errorBit & 0x7);
    uint8_t *errorStatusBits = &em->errorStatusBits[index];
    uint8_t bufCopy[8];

    if(!em) return -1;

    /* if errorBit value not supported, send emergency 'CO_EM_WRONG_ERROR_REPORT' */
    if(index >= em->errorStatusBitsSize){
        em->wrongErrorReport = errorBit;
        return -1;
    }

    /* if error is allready cleared, return */
    if((*errorStatusBits & bitmask) == 0) return 0;

    /* erase error bit */
    *errorStatusBits &= ~bitmask;

    /* verify buffer full */
    if(em->bufFull){
        em->bufFull = 2;
        return -2;
    }

    /* prepare data for emergency message */
    bufCopy[0] = 0;
    bufCopy[1] = 0;
    bufCopy[2] = 0; /* error register will be set later */
    bufCopy[3] = errorBit;
    CO_memcpySwap4(&bufCopy[4], (uint8_t*)&infoCode);

    /* copy data to the buffer, increment writePtr and verify buffer full */
    CO_DISABLE_INTERRUPTS();
    memcpy((void*)em->bufWritePtr, (void*)&bufCopy[0], 8);
    em->bufWritePtr += 8;

    if(em->bufWritePtr == em->bufEnd) em->bufWritePtr = em->buf;
    if(em->bufWritePtr == em->bufReadPtr) em->bufFull = 1;
    CO_ENABLE_INTERRUPTS();

    return 1;
}


/******************************************************************************/
int8_t CO_isError(CO_EM_t *em, uint8_t errorBit){
    uint8_t index = errorBit >> 3;
    uint8_t bitmask = 1 << (errorBit & 0x7);

    if((em->errorStatusBits[index] & bitmask)) return 1;

    return 0;
}
