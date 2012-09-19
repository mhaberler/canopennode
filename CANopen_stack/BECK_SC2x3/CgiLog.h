/*******************************************************************************

   File: CgiLog.h
   Object for CGI function, which logs CAN messages as two separate logs. One
   for Emergency messages (saving to file) and one temporary all messages log.

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

#ifndef _CGI_LOG_H
#define _CGI_LOG_H

#include "clib.h"
#include "CO_driver.h"

//Define global macro:     projectProperties -> C/C++General
//      -> PathsAndSymbols -> Symbols -> add -> CO_LOG_CAN_MESSAGES
#ifndef CO_LOG_CAN_MESSAGES
#error Macro CO_LOG_CAN_MESSAGES must be defined globally!
#endif


/*******************************************************************************
   Topic: CAN message logging

   All CAN messages are stored into RAM buffer.

   Received or transmitted messages from <CO_driver.h> are inside <CO_logMessage>
   function copied into RAM buffer 'CANBuf'. Buffer is circular binary. The
   oldest message may be in the middle of the buffer. Head contains two
   timestamps, both represents to the same time. Timestamp in the head is always
   older than timestamps of the messages, but not to old.

   Contents of CAN message log:
    - First 16 bytes is message head.
    - All other are CAN messages, each message is 16-byte wide.

   Format of the head:
   TTTTMMMMYMDHMSmm
   T - 32 bit timestamp in milliseconds (little endian). It overflows in 50 days.
   M - Address of the oldest CAN message.
   Y - Year (0-99).
   M - Month (1-12).
   D - Day (1-31).
   H - Hour (0-23).
   M - Minute (0-59).
   S - Second (0-59).
   m - 16 bit millisecond (0-999) (little endian).

   Format of the CAN message:
   TTTTAA0Ndddddddd
   T - 32 bit timestamp in milliseconds (little endian). It overflows in 50 days.
   A - 11-bit CAN address (little endian).
   0 - Zero byte.
   N - Number of data bytes.
   d - 8 data bytes.

   RAM buffer access:
   Buffer is accessible via CGI function <http://host/CANlog>. It returns direct
   copy of RAM buffer, which is in binary format. <http://host/CANlog?clear>
   clears the RAM buffer and returns nothing. <http://host/CANlog?dump>
   saves the the RAM buffer to disc and returns nothing.
   Internally there are two RAM buffers. When CGI access occurs, buffer maps to
   web server. In the same time the second buffer empties and maps to CAN
   logging. So CGI function always returns only new CAN messages since last read.
   RAM buffer may also be stored to file. (In case of fatal error.)

   Parameters:
   'CAN log size' - Size of CAN log buffer in bytes.
*******************************************************************************/


/*******************************************************************************
   Topic: Emergency message logging

   Emergency messages from all CANopen devices are logged into file.

   Received or transmitted messages from <CO_driver.h> are inside
   <CO_logMessage> function first verified if they are emergency type. If yes,
   raw message is first copied to temporary circular buffer 'emcyTempBuf'.

   Function <CgiLogEmcyProcess> is called cyclically from main. It formats raw
   emergency messages from temporary buffer and stores them to battery powered
   SRAM as texts separated by newlines. SRAM contains emergency messages from
   midnight. Contents of SRAM can be accessed via CGI function <http://host/emcy>.

   Contents of SRAM are copied to the file "emcy.log" on the following
   conditions:
    - After midnight. (If there is more Emergencies per day then is the size of
      the SRAM buffer, extra emergencies are lost.)
    - After the CGI call: <http://host/emcy?flush>. (CGI updates the emcy.log
      file and returns nothing.)
    - On the program startup.

   emcy.log file contents:
   TT.TT.TT TT:TT:TT - NN - CCCC EE II AAAAAAAA
   T - timestamp (date, time).
   N - NodeId.
   C - CANopen error code.
   E - CANopen error register.
   I - Index of error condition (stack specific).
   A - Additional information (stack specific).
   Byte order of emergency message in log file is not equal as in CAN message.
   For more information on emergency message see <CO_Emergency.h>. First
   four bytes in SRAM are used as a pointer, all other are as described above.

   Temporary buffer message format:
   TTTTAAAAdddddddd - 16 bytes
   T - 32 bit timestamp in milliseconds (big endian).
   A - 11-bit CAN address with length in MSB byte (big endian).
   d - 8 data bytes.


   Parameters:
   'Emergency SRAM size' - Size in bytes in SRAM for emergency messages. It
   limits maximum messages per day and prevents from too large log file.
   Variable is accessible via CANopen.
*******************************************************************************/


/*******************************************************************************
   Object: CgiLog_t

   Variables for CGI log object.

   Variables:
      emcyBuf           - Emergency buffer in SRAM.
      emcyBufSize       - Size in bytes of the above buffer.
      emcyBufOfs        - Pointer to location, which contains offset on the
                          above buffer for new message.
      emcyTempBuf       - Temporary FIFO buffer for emergencies.
      emcyTempBufStart  - Start location in buffer.
      emcyTempBufStop   - Stop location in buffer.
      emcyTempBufHold   - Suppress <CgiLogEmcyProcess> function for number of cycles.
      emcyLastSavedDay  - The day in month, SRAM was last saved.
      CANBuf            - Two CAN log buffers used also for CGI web page.
      CANBufTimestamp   - Auxiliary buffer for timestamp.
      CANBufSize        - Size of the above buffer.
      CANBufIdx         - Index of the buffer used.
      CANBufOvf         - True, if circular buffer is full.
      CANBufOfs         - Offset in the buffer for the new message.
      cgiEntryEmcy      - Holds information on the CGI emcy function.
      cgiEntryCAN       - Holds information on the CGI CAN function.
*******************************************************************************/
typedef struct{
   char           *emcyBuf;
   UNSIGNED32      emcyBufSize;
   UNSIGNED32     *emcyBufOfs;
   UNSIGNED8       emcyTempBuf[0x10000];//on this size UNSIGNED16 emcyTempBufStart/stop will overflow, don't change this. Before this must be 32-bit variable.
   UNSIGNED16      emcyTempBufStart;
   UNSIGNED16      emcyTempBufStop;
   UNSIGNED16      emcyTempBufHold;
   UNSIGNED8       emcyLastSavedDay;
   UNSIGNED8      *CANBuf[2];
   UNSIGNED8       CANBufTimestamp[16];
   UNSIGNED32      CANBufSize;
   UNSIGNED8       CANBufIdx;
   UNSIGNED8       CANBufOvf;
   UNSIGNED32      CANBufOfs;
   UNSIGNED32      maxDumpFiles;
   CGI_Entry       cgiEntryEmcy;
   CGI_Entry       cgiEntryCAN;
}CgiLog_t;


/*******************************************************************************
   Function: CgiLog_init_1

   Initialize CGI handler.

   Parameters:
      ppCgiLog          - Pointer to address of CGI log object <CgiLog_t>.
      emcyBuf           - Pointer to the emergency buffer in SRAM.
      emcyBufSize       - Size of the emergency buffer.
      CANBufSize        - Size of the CAN buffer(s).

   Return:
      CO_ERROR_NO               - Operation completed successfully.
      CO_ERROR_OUT_OF_MEMORY    - Memory allocation failed.
      CO_ERROR_ILLEGAL_ARGUMENT - Error in function arguments.
      CO_ERROR_PARAMETERS       - CGI initialization failed.
*******************************************************************************/
INTEGER16 CgiLog_init_1(
      CgiLog_t      **ppCgiLog,
      UNSIGNED8      *emcyBuf,
      UNSIGNED32      emcyBufSize,
      UNSIGNED32      CANBufSize,
      UNSIGNED32      maxDumpFiles);


/*******************************************************************************
   Function: CgiLog_delete

   Delete CGI handler and free memory.

   Parameters:
      ppCgiLog          - Pointer to pointer to CGI log object <CgiLog_t>.
                          Pointer to object is set to 0.
*******************************************************************************/
void CgiLog_delete(CgiLog_t **ppCgiLog);


/*******************************************************************************
   Function: CO_logMessage

   Function copies CAN message to buffers. It is called by driver.

   Parameters:
      msg               - Pointer to CAN message <CO_CANrxMsg_t>.
*******************************************************************************/
void CO_logMessage(const CanMsg *msg);


/*******************************************************************************
   Function: CgiLogSaveBuffer

   Function stores the complete RAM buffer (CAN log) to filesystem.

   File path is: webRootDir\dump\. File name is generated automatically from
   date of storage and is: YYMMDD-HHMMSS.dmp. To prevent disk full, older dump
   files are automatically deleted in this function. Variable 'maxDumpFiles'
   is used to limit the number of files on disc.

   Parameters:
      CgiLog            - Pointer to CgiLog object <CgiLog_t>.
*******************************************************************************/
void CgiLogSaveBuffer(CgiLog_t *CgiLog);


/*******************************************************************************
   Function: CgiLogEmcyProcess

   Process emergency messages from temporary RAM buffer to SRAM buffer.

   Parameters:
      CgiLog            - Pointer to CgiLog object <CgiLog_t>.
*******************************************************************************/
void CgiLogEmcyProcess(CgiLog_t *CgiLog);


/*******************************************************************************
   Function: CgiLogEmcySave

   Save emergencies from SRAM into file.

   Parameters:
      CgiLog            - Pointer to CgiLog object <CgiLog_t>.
*******************************************************************************/
void CgiLogEmcySave(CgiLog_t *CgiLog);


#endif
