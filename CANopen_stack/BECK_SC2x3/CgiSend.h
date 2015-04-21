/**
 * Object for CGI function, which can send any CANopen message over the CANopen
 * network.
 *
 * @file        CgiSend.h
 * @ingroup     CO_CGI_CANSend
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


#ifndef CGI_SEND_H
#define CGI_SEND_H

#include "clib.h"
#include "CO_driver.h"


/**
 * @defgroup CO_CGI_CANSend Web sen CAN message
 * @ingroup CO_CGI
 * @{
 *
 * CGI function can send any message over the CANopen network. It can
 * be used with web browser with html get method. CGI function name is "send".
 *
 * ####Usage
 * http://host/send?iii=dddd
 * iii  - Can identifier in hex format from range 000 .. 7FF.
 * dddd - data in hex format. Two digits for one byte, max 8 bytes.
 *
 * A text file is returned in the following format:
 * sent iii: dd dd
 */


/**
 * CGI send object.
 */
typedef struct{
    char_t             *buf;            /**< Buffer for CGI web page */
    uint32_t            bufSize;        /**< Size of the above buffer */
    CGI_Entry           cgiEntry;       /**< Holds information on the CGI function */
}CgiSend_t;


/**
 * Initialize CGI handler.
 *
 * @param CgiSend This object will be initialized.
 * @param buf Buffer for CGI web page.
 * @param bufSize Size of the above buffer.
 *
 * @return #CO_ReturnError_t CO_ERROR_NO, CO_ERROR_ILLEGAL_ARGUMENT or CO_ERROR_PARAMETERS
 */
int16_t CgiSend_init_1(
        CgiSend_t              *CgiSend,
        char_t                 *buf,
        uint32_t                bufSize);


/**
 * Delete CGI handler.
 *
 * @param CgiSend CGI object, which contains CGI handler.
 */
void CgiSend_delete(CgiSend_t *CgiSend);


/** @} */
#endif
