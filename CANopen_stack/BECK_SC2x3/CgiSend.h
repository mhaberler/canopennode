/*******************************************************************************

   File: CgiSend.h
   Object for CGI function, which can send any CANopen message over the CANopen
   network.

   Copyright (C) 2004-2010 Janez Paternoster

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

#ifndef _CGI_SEND_H
#define _CGI_SEND_H

#include "clib.h"
#include "CO_driver.h"


/*******************************************************************************
   Topic: CGI send message

   CGI send message is a CGI function, can send any message over the CANopen
   network. It can be used with web browser with html get method. CGI function
   name is "send".

   Usage:
   http://host/send?iii=dddd
   iii  - Can identifier in hex format from range 000 .. 7FF.
   dddd - data in hex format. Two digits for one byte, max 8 bytes.

   A text file is returned in the following format:
   sent iii: dd dd
*******************************************************************************/


/*******************************************************************************
   Object: CgiSend_t

   Variables for CGI send object.

   Variables:
      buf            - Buffer for CGI web page.
      bufSize        - Size of the above buffer.
      cgiEntry       - Holds information on the CGI function.
*******************************************************************************/
typedef struct{
   char          *buf;
   unsigned int   bufSize;
   CGI_Entry      cgiEntry;
}CgiSend_t;


/*******************************************************************************
   Function: CgiSend_init

   Initialize CGI handler.

   Parameters:
      ppCgiSend               - Pointer to address of CGI send object <CgiSend_t>.
      bufSize                 - Size of CGI buffer.

   Return:
      CO_ERROR_NO                - Operation completed successfully.
      CO_ERROR_OUT_OF_MEMORY     - Memory allocation failed.
      CO_ERROR_ILLEGAL_ARGUMENT  - Error in function arguments.
      CO_ERROR_PARAMETERS        - CGI initialization failed.
*******************************************************************************/
INTEGER16 CgiSend_init_1(
      CgiSend_t       **ppCgiSend,
      unsigned int      bufSize);


/*******************************************************************************
   Function: CgiSend_delete

   Delete CGI handler and free memory.

   Parameters:
      ppCgiSend         - Pointer to pointer to CGI send object <CgiSend_t>.
                          Pointer to object is set to 0.
*******************************************************************************/
void CgiSend_delete(CgiSend_t **ppCgiSend);


#endif
