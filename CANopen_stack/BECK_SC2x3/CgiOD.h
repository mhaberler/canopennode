/*******************************************************************************

   File: CgiOD.h
   Object for CGI function, which acts as SDO client. It can access CANopen
   Object Dictionary on any device.

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

#ifndef _CGI_OD_H
#define _CGI_OD_H

#include "clib.h"
#include "CO_driver.h"


/*******************************************************************************
   Topic: OD client

   Object dictionary client is a CGI function, which provides access to object
   dictionary on any device on CANopen network. It can be used with web browser
   with html get method. CGI function name is "ODcli".

   Usage:
   http://host/odcli?wnniiiissll=xxxx[&rnniiiissll=]
   w    - 'w'rite or 'r'ead.
   nn   - node ID in hex format.
   iiii - Object dictionary index in hex format.
   ss   - Object dictionary subindex in hex format.
   ll   - length of variable (1 to FFFFFFFF) in hex format. If reading, this
          value is ignored.
   xxxx - Value to be written in hex and little endian format. Value is ignored
          when reading.

   A text file is returned in following format (AB followed with code means abort):
   w nn iiii ss ll OK: xx xx
   w nn iiii ss ll AB: yyyyyyyy
   r nn iiii ss ll OK: xx xx
   r nn iiii ss ll AB: yyyyyyyy
*******************************************************************************/


/*******************************************************************************
   Object: CgiCli_t

   Variables for CGI OD client object.

   Variables:
      buf            - Buffer for CGI web page.
      bufSize        - Size of the above buffer.
      cgiEntry       - Holds information on the CGI function.
*******************************************************************************/
typedef struct{
   char          *buf;
   unsigned int   bufSize;
   CGI_Entry      cgiEntry;

}CgiCli_t;


/*******************************************************************************
   Function: CgiCli_init

   Initialize CGI handler.

   Parameters:
      ppCgiCli                - Pointer to address of Cgi OD client object <CgiCli_t>.
      bufSize                 - Size of CGI buffer.

   Return:
      CO_ERROR_NO                - Operation completed successfully.
      CO_ERROR_OUT_OF_MEMORY     - Memory allocation failed.
      CO_ERROR_ILLEGAL_ARGUMENT  - Error in function arguments.
      CO_ERROR_PARAMETERS        - CGI initialization failed.
*******************************************************************************/
INTEGER16 CgiCli_init_1(
      CgiCli_t        **ppCgiCli,
      unsigned int      bufSize);


/*******************************************************************************
   Function: CgiCli_delete

   Delete CGI handler and free memory.

   Parameters:
      ppCgiCli          - Pointer to pointer to Cgi OD client object <CgiCli_t>.
                          Pointer to object is set to 0.
*******************************************************************************/
void CgiCli_delete(CgiCli_t **ppCgiCli);


#endif
