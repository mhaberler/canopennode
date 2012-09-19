/*******************************************************************************

   File - CgiLog.c
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


#include "CgiLog.h"
#include "CANopen.h"

#include <string.h> // for memcpy
#include <stdlib.h> // for malloc, free
#include <stdio.h>


CgiLog_t *globalCgiLog;


/******************************************************************************/
void CgiLogTimestamp(UNSIGNED8 *buf){
   UNSIGNED32 tickCount;
   TimeDateFineS timeDate;

   DISABLE_INTERRUPTS();
   tickCount = RTX_GetTickCount();
   RTX_Get_TimeDate_us(&timeDate);
   ENABLE_INTERRUPTS();

   memcpySwap4(buf, (UNSIGNED8*) &tickCount); buf += 4;
   *buf++ = 16;
   *buf++ = 0;
   *buf++ = 0;
   *buf++ = 0;
   *buf++ = timeDate.yr;
   *buf++ = timeDate.mn;
   *buf++ = timeDate.dy;
   *buf++ = timeDate.hr;
   *buf++ = timeDate.min;
   *buf++ = timeDate.sec;
   memcpySwap2(buf, (UNSIGNED8*) &timeDate.msec);
}


/******************************************************************************/
void huge _pascal CgiLogCANFunction(rpCgiPtr CgiRequest){
   CgiLog_t *CgiLog = globalCgiLog;

   int clearOnly = 0;

   //decode optional arguments
   if(strlen(CgiRequest->fArgumentBufferPtr)){
      char far *name = NULL;
      char far *val = NULL;
      if(CGI_GetArgument(&name, &val, CgiRequest) == 0){
         if(strcasecmp(name, "clear")==0){
            clearOnly = 1;
         }
         if(strcasecmp(name, "dump")==0){
            CgiLogSaveBuffer(CgiLog);
            CgiRequest->fHttpResponse = CgiHttpOKNoDoc;
            return;
         }
      }
   }

   //find buffer
   UNSIGNED8 bufIdx = CgiLog->CANBufIdx;
   UNSIGNED8 *buf = CgiLog->CANBuf[bufIdx];
   UNSIGNED32 bufLen;

   DISABLE_INTERRUPTS();
   //calculate size and write pointer of the oldest message
   if(CgiLog->CANBufOvf){
      bufLen = CgiLog->CANBufSize;
      memcpySwap4(buf+4, (UNSIGNED8*) &CgiLog->CANBufOfs);
   }
   else{
      bufLen = CgiLog->CANBufOfs;
   }
   //swap buffers
   CgiLog->CANBufIdx = 1 - bufIdx;
   CgiLog->CANBufOvf = 0;
   CgiLog->CANBufOfs = 16;
   ENABLE_INTERRUPTS();

   //write timestamp in the new buffer
   CgiLogTimestamp(CgiLog->CANBuf[CgiLog->CANBufIdx]);

   //Give page to the web server
   if(clearOnly){
      CgiRequest->fHttpResponse = CgiHttpOKNoDoc;
   }
   else{
      CgiRequest->fHttpResponse = CgiHttpOk;
      CgiRequest->fDataType = CGIDataTypeOctet;
      CgiRequest->fResponseBufferPtr = (char*)buf;
      CgiRequest->fResponseBufferLength = bufLen;
   }
}


/******************************************************************************/
void huge _pascal CgiLogEmcyFunction(rpCgiPtr CgiRequest){
   CgiLog_t *CgiLog = globalCgiLog;

   //decode optional arguments
   if(strlen(CgiRequest->fArgumentBufferPtr)){
      char far *name = NULL;
      char far *val = NULL;
      if(CGI_GetArgument(&name, &val, CgiRequest) == 0){
         if(strcasecmp(name, "flush")==0){
            //move emergencies from SRAM to file
            if(*CgiLog->emcyBufOfs > 4){
               CgiLog->emcyTempBufHold = 40; //Suppress <CgiLogEmcyProcess> function for 2 seconds
               CgiLogEmcySave(CgiLog);
               CgiRequest->fHttpResponse =  CgiHttpOKNoDoc;
               return;
            }
         }
      }
   }

   //Give page to the web server - last messages
   CgiRequest->fHttpResponse = CgiHttpOk;
   CgiRequest->fDataType = CGIDataTypeText;
   CgiRequest->fResponseBufferPtr = CgiLog->emcyBuf + 4;
   CgiRequest->fResponseBufferLength = *CgiLog->emcyBufOfs - 4;
}


/******************************************************************************/
INTEGER16 CgiLog_init_1(
      CgiLog_t      **ppCgiLog,
      UNSIGNED8      *emcyBuf,
      UNSIGNED32      emcyBufSize,
      UNSIGNED32      CANBufSize,
      UNSIGNED32      maxDumpFiles)
{
   //verify CANBufSize
   CANBufSize &= 0x1FFFFF0;                             //32Mb max
   if(CANBufSize < 0x10000) CANBufSize = 0x10000;       //64kb min

   //allocate memory if not already allocated
   if((*ppCgiLog) == NULL){
      if(((*ppCgiLog)            = (CgiLog_t*) malloc(sizeof(CgiLog_t))) == NULL){                                                              return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppCgiLog)->CANBuf[0] = (UNSIGNED8*)malloc(      CANBufSize)) == NULL){                              free(*ppCgiLog); *ppCgiLog = 0; return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppCgiLog)->CANBuf[1] = (UNSIGNED8*)malloc(      CANBufSize)) == NULL){free((*ppCgiLog)->CANBuf[0]); free(*ppCgiLog); *ppCgiLog = 0; return CO_ERROR_OUT_OF_MEMORY;}
   }
   else if((*ppCgiLog)->CANBuf[0] == NULL ||
           (*ppCgiLog)->CANBuf[1] == NULL) return CO_ERROR_ILLEGAL_ARGUMENT;

   CgiLog_t *CgiLog = *ppCgiLog; //pointer to (newly created) object
   globalCgiLog = CgiLog;


   //setup variables
   if(emcyBuf == NULL) return CO_ERROR_ILLEGAL_ARGUMENT;
   CgiLog->emcyBuf          = (char*) emcyBuf;
   CgiLog->emcyBufSize      = emcyBufSize;
   CgiLog->emcyBufOfs       = (UNSIGNED32*) emcyBuf;//offset is stored in SRAM
   CgiLog->emcyTempBufStart = 0;
   CgiLog->emcyTempBufStop  = 0;
   CgiLog->emcyTempBufHold  = 0;
   CgiLog->emcyLastSavedDay = 0;
   CgiLog->CANBufSize       = CANBufSize;
   CgiLog->CANBufIdx        = 0;
   CgiLog->CANBufOvf        = 0;
   CgiLog->CANBufOfs        = 16;
   CgiLog->maxDumpFiles     = maxDumpFiles;

   CgiLogTimestamp(CgiLog->CANBuf[0]);

   //if error in SRAM make new buffer
   if(*CgiLog->emcyBufOfs < 4 || *CgiLog->emcyBufOfs >= CgiLog->emcyBufSize)
      *CgiLog->emcyBufOfs = 4;

   //emergency buffer will be saved in CgiLogEmcyProcess automatically

   //Install CGI functions
   CgiLog->cgiEntryEmcy.PathPtr    = "emcy";            // Name of the page
   CgiLog->cgiEntryEmcy.method     = CgiHttpGet;        // HTTP method
   CgiLog->cgiEntryEmcy.CgiFuncPtr = CgiLogEmcyFunction;// Function called on browser request
   if(CGI_Install(&CgiLog->cgiEntryEmcy) != 0)
      return CO_ERROR_PARAMETERS;

   CgiLog->cgiEntryCAN.PathPtr     = "CANlog";          // Name of the page
   CgiLog->cgiEntryCAN.method      = CgiHttpGet;        // HTTP method
   CgiLog->cgiEntryCAN.CgiFuncPtr  = CgiLogCANFunction; // Function called on browser request
   if(CGI_Install(&CgiLog->cgiEntryCAN) != 0)
      return CO_ERROR_PARAMETERS;

   return CO_ERROR_NO;
}


/******************************************************************************/
void CgiLog_delete(CgiLog_t **ppCgiLog){
   if(*ppCgiLog){
      CGI_Delete((*ppCgiLog)->cgiEntryEmcy.PathPtr);
      CGI_Delete((*ppCgiLog)->cgiEntryCAN.PathPtr);
      free((*ppCgiLog)->CANBuf[1]);
      free((*ppCgiLog)->CANBuf[0]);
      free(*ppCgiLog);
      *ppCgiLog = 0;
   }
}


/******************************************************************************/
void CO_logMessage(const CanMsg *msg){
   CgiLog_t *CgiLog = globalCgiLog;

   //variables
   UNSIGNED32   timeStamp = RTX_GetTickCount();
   UNSIGNED8    len       = msg->len;
   UNSIGNED32   id_len    = (UNSIGNED32)len<<24 | (UNSIGNED16) canDecodeId(msg->id);
   UNSIGNED8    isEmcy    = 0;
   UNSIGNED8    isOvfPrev = 0;

   //is emergency message?
   if((id_len & 0xFF80) == 0x0080 && (id_len & 0x7F) != 0 && len == 8) isEmcy = 1;

   //find locations in buffers
   UNSIGNED32   CANbufOffset;   //offset in CAN buffer
   UNSIGNED32*  CANbuf;         //location in CAN buffer for new message
   UNSIGNED16   emcyBufOffset=0;//offset in emergency temporary buffer
   UNSIGNED32*  emcyBuf;        //location in emergency temporary buffer for new message

   DISABLE_INTERRUPTS();
   CANbufOffset = CgiLog->CANBufOfs;
   if((CANbufOffset+32) > CgiLog->CANBufSize){
      CgiLog->CANBufOfs = 16;
      isOvfPrev = CgiLog->CANBufOvf;
      CgiLog->CANBufOvf = 1;
   }
   else{
      CgiLog->CANBufOfs = CANbufOffset+16;
   }
   if(isEmcy){
      emcyBufOffset = CgiLog->emcyTempBufStop;
      CgiLog->emcyTempBufStop = emcyBufOffset + 16;
   }
   ENABLE_INTERRUPTS();

   //update timestamp. (Timestamp on the buffer must be older than the oldest message)
   if(isOvfPrev) memcpy(CgiLog->CANBuf[CgiLog->CANBufIdx], CgiLog->CANBufTimestamp, 16);
   if(CgiLog->CANBufOfs == 16) CgiLogTimestamp(CgiLog->CANBufTimestamp);

   //copy contents
   CANbuf = (UNSIGNED32*)(CgiLog->CANBuf[CgiLog->CANBufIdx] + CANbufOffset);
   if(isEmcy){
      //emergency + CAN buffer
      emcyBuf = (UNSIGNED32*)(CgiLog->emcyTempBuf + emcyBufOffset);

      memcpySwap4((UNSIGNED8*) CANbuf++, (UNSIGNED8*) &timeStamp);
      *emcyBuf++ = timeStamp;
      memcpySwap4((UNSIGNED8*) CANbuf++, (UNSIGNED8*) &id_len);
      *emcyBuf++ = id_len;
      *CANbuf = *(CANbuf+1) = 0;
      memcpy(CANbuf, msg->data, len);
      *emcyBuf++ = *CANbuf++;
      *emcyBuf = *CANbuf;
   }
   else{
      //CAN buffer only
      memcpySwap4((UNSIGNED8*) CANbuf++, (UNSIGNED8*) &timeStamp);
      memcpySwap4((UNSIGNED8*) CANbuf++, (UNSIGNED8*) &id_len);
      *CANbuf = *(CANbuf+1) = 0;
      memcpy(CANbuf, msg->data, len);
   }
}


/******************************************************************************/
void CgiLogSaveBuffer(CgiLog_t *CgiLog){

   if(CgiLog->maxDumpFiles == 0) return;

   //find buffer
   UNSIGNED8 bufIdx = CgiLog->CANBufIdx;
   UNSIGNED8 *buf = CgiLog->CANBuf[bufIdx];
   UNSIGNED32 bufLen;

   DISABLE_INTERRUPTS();
   //calculate size and write pointer of the oldest message
   if(CgiLog->CANBufOvf){
      bufLen = CgiLog->CANBufSize;
      memcpySwap4(buf+4, (UNSIGNED8*) &CgiLog->CANBufOfs);
   }
   else{
      bufLen = CgiLog->CANBufOfs;
   }
   //swap buffers
   CgiLog->CANBufIdx = 1 - bufIdx;
   CgiLog->CANBufOvf = 0;
   CgiLog->CANBufOfs = 16;
   ENABLE_INTERRUPTS();

   //write timestamp in the new buffer
   CgiLogTimestamp(CgiLog->CANBuf[CgiLog->CANBufIdx]);


   //get filename and path of the saving buffer
   char filename[300];
   char *webRootDir;
   TimeDate_Structure td;
   CGI_GetRootDir(&webRootDir);
   RTX_Get_TimeDate(&td);

   //first make directory (if it does not exist)
   int fLen = sprintf(filename, "%c:%sdump", CGI_GetRootDrive()+'A', webRootDir);
   mkdir(filename);

   //write buffer to file
   sprintf(filename+fLen++, "\\%02d%02d%02d-%02d%02d%02d.dmp", td.yr, td.mn, td.dy, td.hr, td.min, td.sec);
   FILE *file = fopen(filename, "wb");
   if(file){
      fwrite(buf, 1, bufLen, file);
      fclose(file);
   }
   else{
      printf("\nCan't open %s", filename);
      return;
   }


   //if there is more files than CgiLog->maxDumpFiles, delete the oldest one.
   LFN_FILE_FIND ff;
   FS_FILETIME fTime;
   int i = 0;

   strcpy(filename+fLen, "*.dmp");
   int ret = BIOS_LFN_Findfirst(filename, &ff, 0);
   fTime.filedate = ff.filetimestamp.filedate;
   fTime.filetime = ff.filetimestamp.filetime;

   while(ret){
      if(ff.filetimestamp.filedate<fTime.filedate || (ff.filetimestamp.filedate==fTime.filedate && ff.filetimestamp.filetime<fTime.filetime)){
         fTime.filedate = ff.filetimestamp.filedate;
         fTime.filetime = ff.filetimestamp.filetime;
         strcpy(filename+fLen, ff.pszLong_Name);
      }
      ret = BIOS_LFN_Findnext(&ff);
      i++;
   }
   if(i > CgiLog->maxDumpFiles){
      remove(filename);
   }
}


/******************************************************************************/
void CgiLogEmcyProcessOvf(UNSIGNED8* tBuf, UNSIGNED8 errorBit, UNSIGNED16 errorCode){
   tBuf[8] = errorCode & 0xFF; tBuf[9] = (errorCode>>8) & 0xFF;
   tBuf[11] = errorBit;
   tBuf[6] = tBuf[7] = tBuf[10] = tBuf[12] = tBuf[13] = tBuf[14] = tBuf[15] = 0;
}
void CgiLogEmcyProcess(CgiLog_t *CgiLog){
   //if suppressed, return
   if(CgiLog->emcyTempBufHold){
      CgiLog->emcyTempBufHold--;
      return;
   }

   //verify temporary buffer for new messages
   UNSIGNED16 start = CgiLog->emcyTempBufStart;
   UNSIGNED16 stop  = CgiLog->emcyTempBufStop;
   if(start==stop) return;
   CgiLog->emcyTempBufStart = stop;

   //get time
   char sTime[30];
   TimeDate_Structure td;
   RTX_Get_TimeDate(&td);
   sprintf(sTime, "%02d.%02d.%02d %02d:%02d:%02d", td.dy, td.mn, td.yr, td.hr, td.min, td.sec);

   //get buffers
   UNSIGNED32 SramOfs = *CgiLog->emcyBufOfs;
   UNSIGNED8 *tBuf = CgiLog->emcyTempBuf;

   //write emergencies from temporary buffer into SRAM
   while(start != stop){
      //verify if SRAM is full
      if((SramOfs + 50) > CgiLog->emcyBufSize) break;//SRAM is full
      if((SramOfs + 100) > CgiLog->emcyBufSize){
         //write emergency about the buffer full
         CgiLogEmcyProcessOvf(&tBuf[start], ERROR_EMERGENCY_BUFFER_FULL);
      }
      UNSIGNED8  nodeId         = tBuf[start+7]&0x7F;
      UNSIGNED16 errorCode      = (UNSIGNED16) tBuf[start+9] << 8 | tBuf[start+8];
      UNSIGNED8  errorRegister  = tBuf[start+10];
      UNSIGNED8  errorIndex     = tBuf[start+11];
      UNSIGNED32 info;
      memcpySwap4((UNSIGNED8*)&info, &tBuf[start+12]);

      SramOfs += sprintf((CgiLog->emcyBuf+SramOfs),
            "%s - %02X - %04X %02X %02X %08X\n",
            sTime, nodeId, errorCode, errorRegister, errorIndex, (unsigned int)info);

      start += 16;
   }
   *CgiLog->emcyBufOfs = SramOfs;

   //if next day, store emergencies from SRAM into file
   if(CgiLog->emcyLastSavedDay != td.dy) CgiLogEmcySave(CgiLog);
}


/******************************************************************************/
void CgiLogEmcySave(CgiLog_t *CgiLog){
   //get filename on the web root dir
   char filename[300];
   char *webRootDir;
   CGI_GetRootDir(&webRootDir);
   sprintf(filename, "%c:%semcy.log", CGI_GetRootDrive()+'A', webRootDir);

   FILE *file;
   file = fopen(filename, "at");
   if(file){
      fputs(CgiLog->emcyBuf + 4, file);    //write SRAM to file
      *CgiLog->emcyBufOfs = 4;             //start SRAM from the beginning
      fclose(file);
   }
   else
      printf("\nCan't open %s", filename);

   //update the emcyLastSavedDay
   TimeDate_Structure td;
   RTX_Get_TimeDate(&td);
   CgiLog->emcyLastSavedDay = td.dy;
}
