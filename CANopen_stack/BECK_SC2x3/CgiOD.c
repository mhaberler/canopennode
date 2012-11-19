/*******************************************************************************

   File - CgiOD.c
   Object for CGI function, which acts as SDO client - it can access CANopen
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


#include "CgiOD.h"
#include "CANopen.h"

#include <string.h> // for memcpy
#include <stdlib.h> // for malloc, free


/*******************************************************************************
   Function - hex2dec

   Decodes hex string and returns numerical value.

   Variables:
      str   - Pointer to string.
      len   - Length of string.
      val   - Value (return).

   Return:
      0     - Operation completed successfully.
      -1    - Error in string.
*******************************************************************************/
int hex2dec(char *str, int len, unsigned int *val){
   *val = 0;
   int i;
   for(i=0; i<len; i++){
      char c = str[i];
      if     (c>='A' && c<='F') c = c - ('A' - 10);
      else if(c>='a' && c<='f') c = c - ('a' - 10);
      else if(c>='0' && c<='9') c = c - '0';
      else return 1;
      *val = *val<<4 | c;
   }
   return 0;
}

/******************************************************************************/
void CgiCli_signal(UNSIGNED32 arg){
   RTX_Wakeup(arg);
}

/******************************************************************************/
void huge _pascal CgiCliFunction(rpCgiPtr CgiRequest){
   //Buffers for building the page
   extern CgiCli_t *CgiCli;
   extern CO_t *CO;
   CO_SDOclient_t *SDO_C = CO->SDOclient;
   unsigned int bufLen = 0;   //current length of the buffer

   //Prepare function, which will wake this task after CAN SDO response is
   //received (inside CAN receive interrupt).
   SDO_C->pFunctSignal = CgiCli_signal; //will wake from RTX_Sleep_Time()
   SDO_C->functArg = RTX_Get_TaskID();  //id of this task

   // Construct HTML page
   if(strlen(CgiRequest->fArgumentBufferPtr)){
      char *buf = CgiCli->buf;   //buffer for CGI message
      char far *name = NULL;
      char far *val = NULL;
      while(CGI_GetArgument(&name, &val, CgiRequest) == 0){
         //decode and verify name
         char rw = name[0];
         unsigned int nodeId, idx, sidx, len;
         char err = 0;
         #define DATA_MAX_LEN 889
         unsigned char data[DATA_MAX_LEN]; //data sent or received on CANopen
         unsigned int dataLen = 0;

         if(!name) err++;
         len = strlen(name);
         if (rw == 'r' || rw == 'R'){
            rw = 'r';
            if(len < 9 || len > 17) err++;
         }
         else if(rw == 'w' || rw == 'W'){
            rw = 'w';
            if(len < 10 || len > 17) err++;
         }
         else{
            err++;
         }
         err += hex2dec(&name[1],  2, &nodeId);
         if(nodeId > 127) err++;
         err += hex2dec(&name[3],  4, &idx);
         err += hex2dec(&name[7],  2, &sidx);
         if(rw == 'w'){
            err += hex2dec(&name[9], len-9, &len);

            //decode value
            int i;
            int valLen;
            if(val) valLen = strlen(val);
            else    valLen = 0;
            for(i=0; i<valLen; i=i+2){
               unsigned int dc;
               err += hex2dec(&val[i], 2, &dc);
               data[dataLen] = dc;
               if(++dataLen >= DATA_MAX_LEN) break;
            }

            //verify value length
            if(dataLen != len || dataLen == 0) err++;
         }

         //SDO clinet access
         UNSIGNED32 SDOabortCode = 0;
         if(err){
            buf += sprintf(buf, "Error in argument: %s=%s\n", name, val);
         }
         //read object dictionary
         else if(rw == 'r'){
            INTEGER8 ret;
            CO_SDOclient_setup(SDO_C, 0, 0, nodeId);
            CO_SDOclientUploadInitiate(SDO_C, idx, sidx, data, DATA_MAX_LEN, 0);
            do{
               RTX_Sleep_Time(10);
               ret = CO_SDOclientUpload(SDO_C, 10, 500, &dataLen, &SDOabortCode);
            }while(ret > 0);

            if(SDOabortCode){
               buf += sprintf(buf, "R %02X%04X%02X%X AB: %08X\n",
                              nodeId, idx, sidx, dataLen, (unsigned int)SDOabortCode);
            }
            else{
               unsigned int i;
               buf += sprintf(buf, "R %02X%04X%02X%X OK:", nodeId, idx, sidx, dataLen);
               for(i=0; i<dataLen; i++) buf += sprintf(buf, " %02X", data[i]);
               buf += sprintf(buf, "\n");
            }
         }
         //write into object dictionary
         else if(rw == 'w'){
            INTEGER8 ret;
            CO_SDOclient_setup(SDO_C, 0, 0, nodeId);
            CO_SDOclientDownloadInitiate(SDO_C, idx, sidx, data, dataLen, 0);
            do{
               RTX_Sleep_Time(10);
               ret = CO_SDOclientDownload(SDO_C, 10, 500, &SDOabortCode);
            }while(ret > 0);

            if(SDOabortCode){
               buf += sprintf(buf, "W %02X%04X%02X%X AB: %08X\n",
                              nodeId, idx, sidx, dataLen, (unsigned int)SDOabortCode);
            }
            else{
               unsigned int i;
               buf += sprintf(buf, "W %02X%04X%02X%X OK:", nodeId, idx, sidx, dataLen);
               for(i=0; i<dataLen; i++) buf += sprintf(buf, " %02X", data[i]);
               buf += sprintf(buf, "\n");
            }
         }

         //calculate buffer length, if not enough space for next object, break
         bufLen = (unsigned int)(buf - CgiCli->buf);
         if((bufLen+1000) > CgiCli->bufSize){
            break;
         }
      }
   }
   else{
      sprintf(CgiCli->buf,
         "CGI function provides access to object dictionary on any device on the CANopen network.\n\n"
         "Usage:\n"
         "  odcli?wnniiiissll=xxxx[&rnniiiissll=]\n"
         "  w    - 'w'rite or 'r'ead.\n"
         "  nn   - node ID in hex format.\n"
         "  iiii - Object dictionary index in hex format.\n"
         "  ss   - Object dictionary subindex in hex format.\n"
         "  ll   - length of variable (1 to FFFFFFFF) in hex format. If reading, this value is ignored.\n"
         "  xxxx - Value to be written in hex and little endian format. If reading, this value is ignored.\n");
      bufLen = strlen(CgiCli->buf);
   }

   //disable SDO client
   CO_SDOclient_setup(SDO_C, 0, 0, 0);

   // Give page to the web server
   CgiRequest->fHttpResponse = CgiHttpOk;
   CgiRequest->fDataType = CGIDataTypeText;
   CgiRequest->fResponseBufferPtr = CgiCli->buf;
   CgiRequest->fResponseBufferLength = bufLen;
}


/******************************************************************************/
INTEGER16 CgiCli_init_1(
      CgiCli_t        **ppCgiCli,
      unsigned int      bufSize)
{
   //allocate memory if not already allocated
   if((*ppCgiCli) == NULL){
      if(((*ppCgiCli)      = (CgiCli_t*)malloc(sizeof(CgiCli_t))) == NULL){                                return CO_ERROR_OUT_OF_MEMORY;}
      if(((*ppCgiCli)->buf = (char*)    malloc(         bufSize)) == NULL){free(*ppCgiCli); *ppCgiCli = 0; return CO_ERROR_OUT_OF_MEMORY;}
   }
   else if((*ppCgiCli)->buf == NULL) return CO_ERROR_ILLEGAL_ARGUMENT;

   CgiCli_t *CgiCli = *ppCgiCli; //pointer to (newly created) object


   //setup variables
   CgiCli->bufSize = bufSize;

   //Install CGI function
   CgiCli->cgiEntry.PathPtr = "odcli";          // Name of the page
   CgiCli->cgiEntry.method = CgiHttpGet;        // HTTP method
   CgiCli->cgiEntry.CgiFuncPtr = CgiCliFunction;// Function called on browser request

   if(CGI_Install(&CgiCli->cgiEntry) != 0)
      return CO_ERROR_PARAMETERS;

   return CO_ERROR_NO;
}


/******************************************************************************/
void CgiCli_delete(CgiCli_t **ppCgiCli){
   if(*ppCgiCli){
      CGI_Delete((*ppCgiCli)->cgiEntry.PathPtr);
      free((*ppCgiCli)->buf);
      free(*ppCgiCli);
      *ppCgiCli = 0;
   }
}
