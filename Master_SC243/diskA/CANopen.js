/*******************************************************************************
   CANopen basic functions for communication with the master
   Copyright (C) 2012 Janez Paternoster

   This Source Code Form is subject to the terms of the Mozilla Public
   License, v. 2.0. If a copy of the MPL was not distributed with this file,
   You can obtain one at http://mozilla.org/MPL/2.0/.
*******************************************************************************/

"use strict";

/*******************************************************************************
   Functions: Helper functions

   Helper functions for use with CANopen.

   Functions:
      Number.toFixedString          - Translates number to string with fixed
                                      length with padded leading zeroes.
      Number.toLittleEndianString   - Translates number to string in little endian
                                      hex format, two digits for each byte.
      String.isHex                  - Return true, if string contains only
                                      hex characters.
*******************************************************************************/
   Number.prototype.toFixedString = function(strLen, radix){
      var str = "0000000000000000" + this.toString(radix).toUpperCase();
      return str.slice(-strLen);
   };

   Number.prototype.toLittleEndianString = function(NoOfBytes){
      var str = "";
      var num = this;
      for(var i=0; i<NoOfBytes; i++){
         str += (num&0xFF).toFixedString(2, 16);
         num >>= 8;
      }
      return str;
   };

   String.prototype.isHex = function(){
      if(this.replace(/[0-9A-F]/gi, "").length) return false;
      return true;
   }


/*******************************************************************************
   Object: CANopen

   Constructor for communication with the master.

   Variables:
*******************************************************************************/
function CANopen(){

}

CANopen.prototype = {
   constructor: CANopen,


   /****************************************************************************
      Function: readFile

      Read file from html server.
      
      Any file from web directory can be read.
      
      Readig emergency temporary buffer (For details see file <CgiLog.h>.):
       - filename = "emcy"
       - Messages are maximum one day old, then are automatically stored to log file.
      
      Reading emergency log file:
       - filename = "emcy.log"
       - Log file contains all Emergency messages, except messages, that are
         still in the temporary buffer.
       - To store temporary buffer to log file manually use filename = "emcy?flush".

      Parameters:
         filename         - Name (and path and arguments) of the file on the server.
         responseFunction - Function is called with response text as argument,
                            after html response is received.
   ****************************************************************************/
   readFile: function(filename, responseFunction){
      var req = new XMLHttpRequest();
      req.open("GET", filename, true);
      if(responseFunction){
         req.onload = function (oEvent){
            responseFunction(req.responseText);
         }
      }
      req.send(null);
   },


   /****************************************************************************
      Function: sendCAN

      Send custom CAN message.

      For details see file <CgiSend.h>.

      Parameters:
         id          - 3 digit string CAN identifier in hex format from "000" to "7FF".
         data        - data in hex format. Two digits for one byte, max 8 bytes.
   ****************************************************************************/
   sendCAN: function(id, data){
      if(id.length != 3 || !id.isHex() || parseInt(id, 16) > 0x7FF){
         alert("sendCAN: CAN ID must be three digits from 000 to 7FF!");
         return;
      }
      data = data.replace(/\s/g, ""); //remove white spaces
      if(data.length%2 || data.length>16 || !id.isHex()){
         alert("sendCAN: Data must be max 8 pairs of hex digits! White spaces are ignored.");
         return;
      }

      var req = new XMLHttpRequest();
      req.open("GET", "send?" + id + "=" + data, true);
      req.send(null);
   },


   /****************************************************************************
      Function: sendNMT

      Send NMT command.

      Parameters:
         nodeId      - nodeId as string (0 .. 127(0x7F)) (0 is broadcast - all devices).
         command     - NMT command as two digit hex string.
   ****************************************************************************/
   sendNMT: function(nodeId, command){
      var id = parseInt(nodeId);
      if(!(id>=0 && id<=0x7F)){
         alert("sendNMT: Node ID must be from 0 to 127 (0x7F)!");
         return;
      }
      if(!(command.length==2 && command.isHex())){
         alert("sendNMT: Command must be two hex digits!");
         return;
      }
      this.sendCAN("000", command + id.toFixedString(2,16));
   },


   /****************************************************************************
      Function: SDORead

      SDO read request.

      For details see file <CgiOD.h>.

      Parameters:
         address     - SDO address (NNIIIISSLLLL): 12 or 8 hex characters
                       NN = NodeId, IIII = Index, SS = Subindex,
                       LLLL = length optionaly. Whitespaces are ignored.
         responseFunction - Function is called with response text argument,
                            after SDO response is received.
   ****************************************************************************/
   SDORead: function(address, responseFunction){
      var req = new XMLHttpRequest();
      req.open("GET", "odcli?r"+address.replace(/\s/g, ""), true);
      req.onload = function (oEvent){
         responseFunction(req.responseText);//"R NNIIIISSLLLL OK: dd dd"
      }
      req.send(null);
   },


   /****************************************************************************
      Function: SDOWrite

      SDO write request.

      Parameters:
         address     - Same as in <SDORead>. Length is required and must match
                       length in data field.
         data        - Two hex characters for one byte. CANopen is Little
                       Endian. Whitespaces are ignored.
         responseFunction - Same as in <SDORead>.
   ****************************************************************************/
   SDOWrite: function(address, data, responseFunction){
      var req = new XMLHttpRequest();
      req.open("GET", "odcli?w"+address.replace(/\s/g, "")+"="+data.replace(/\s/g, ""), true);
      req.onload = function (oEvent){
         responseFunction(req.responseText);//"W NNIIIISSLLLL OK: dd dd"
      }
      req.send(null);
   }
};


/*******************************************************************************
   Object: CANLog

   Constructor for retriving CAN log data from the master.

   For details see file <CgiLog.h>.

   Parameters:
      textBufSize    - Each CAN message is stored as text in internal array buffer,
                       which can be used to display log of CAN messages.
                       'textBufSize' indicates maximum number of messages in the buffer.
      funcProcessMsg - Optional external function, which is called for each CAN
                       message with the following arguments: *time* offset from
                       date in milliseconds; *id*, *length*, *data* of the message.
      funcBoundary   - Optional external function, which is called on two events:
                       date time stamp has changed for one minute. In that case
                       function passes the argument *logDate* typeof Date rounded
                       to minutes. Second event is finish of capturing one message
                       block from the server. Null is passed in the first argument.
                       Second argument is always refference to textBuffer array.
      interval       - Interval of CANmessage capturing from the server. If zero,
                       complete RAM buffer is captured once with history CAN
                       messages. If nonzero, capturing must be started with
                       function <start()>.
      filename       - Optional fliename of the saved file. If omited, log is
                       captured from current RAM buffer. If filename is set,
                       interval is set to 0.
*******************************************************************************/
function CANLog(textBufSize, funcProcessMsg, funcBoundary, interval, filename){
   this.textBufSize = textBufSize;
   this.funcProcessMsg = funcProcessMsg;
   this.funcBoundary = funcBoundary;

   this.interval = 0;
   this.runInterval = false;

   this.textBuf = [];
   this.date = new Date();

   //set filename (file or RAM log buffer) and set interval
   if(filename){
      this.filename = filename;
   }
   else{
      this.filename = "CANlog";
      this.interval = interval;
   }

   //call capture once, if not interval
   if(interval == 0) this.capture(this);
}

CANLog.prototype = {
   constructor: CANLog,

   /****************************************************************************
      Function: start, stop

      Start or stop capturing of the CAN messages.
   ****************************************************************************/
   start: function(){
      this.runInterval = true;
      this.capture(this);
   },

   stop: function(){
      this.runInterval = false;
   },
   
   /****************************************************************************
      Function: clearBufferOnServer

      Clear CAN message buffer on the server.
   ****************************************************************************/
   clearBufferOnServer: function(){
      var req = new XMLHttpRequest();
      req.open("GET", "CANlog?clear", true);
      req.send(null);
   },

   /****************************************************************************
      Function: dumpBufferOnServer

      Save CAN message buffer to filesystem on the server.
   ****************************************************************************/
   dumpBufferOnServer: function(){
      var req = new XMLHttpRequest();
      req.open("GET", "CANlog?dump", true);
      req.send(null);
   },

   /****************************************************************************
      Function: capture

      Capture and process CAN log buffer. Don't use directly.
   ****************************************************************************/
   capture: function(log){
      var req = new XMLHttpRequest();
      req.open("GET", log.filename, true);
      req.responseType = "arraybuffer";

      req.onload = function (oEvent){
         var CANlogBinaryFile = req.response;
         if(!CANlogBinaryFile) return;

         //number of CAN messages + 1(head)
         var msgNo = parseInt(CANlogBinaryFile.byteLength / 16);

         //decode head TTTTMMMMYMDHMSmm - two timestamps of the time for refference. MMMM is offset of the oldest message.
         var logHead = new DataView(CANlogBinaryFile, 0, 16);
         var logTime = logHead.getUint32(0, 1) -             //Millisecond time stamp rounded to minutes
                       logHead.getUint8(13)*1000 -
                       logHead.getUint16(14, 1);
         var logDate = new Date(2000+logHead.getUint8(8),    //date rounded to minutes
                                logHead.getUint8(9)-1,
                                logHead.getUint8(10),
                                logHead.getUint8(11),
                                logHead.getUint8(12),
                                0, 0);
         var msgOffset = logHead.getUint32(4, 1); //offset of the oldest CAN record in the array buffer
         var msgOffsetMax = msgNo * 16;           //max offset

         //send on minute change
         if(log.date.getTime() != logDate.getTime()){
            log.date.setTime(logDate.getTime());
            if(log.funcBoundary) log.funcBoundary(log.date, log.textBuf);
         }

         //Process can messages
         for (var i = 1; i < msgNo; i++){
            //decode message TTTTAA0N dddddddd
            var recordHead = new DataView(CANlogBinaryFile, msgOffset, 8);
            var recordData = new DataView(CANlogBinaryFile, msgOffset+8, 8);
            msgOffset += 16;
            if(msgOffset >= msgOffsetMax) msgOffset = 16; //in case of overflowed buffer

            //get message time, be careful for UNSIGNED32 overflow
            var time = recordHead.getUint32(0, 1) - logTime;
            if(time < 0) time += 0x100000000;
            while(time >= 60000){//increase for one minute if necessary
               time -= 60000;
               logTime += 60000;
               if(logTime >= 0x100000000) logTime -= 0x100000000;
               log.date.setSeconds(60);
               if(log.funcBoundary) log.funcBoundary(log.date, log.textBuf);
            }
            var id = recordHead.getUint16(4, 1);
            var len = recordHead.getUint8(7);

            //text log string
            if(log.textBufSize > 0){
               var sLogData = [];
               for(var j=0; j<len; j++) sLogData.push((recordData.getUint8(j)).toFixedString(2, 16));

               var sLog = ("0" + (time / 1000).toFixed(3)).slice(-6) + " " +
                          id.toFixedString(3, 16) + " " +
                          len.toString() + ": " +
                          sLogData.join(" ");

               log.textBuf.push(sLog);

               while(log.textBuf.length > log.textBufSize) log.textBuf.shift();
            }

            //call user function if available
            if(log.funcProcessMsg)
               log.funcProcessMsg(time, id ,len, recordData);
         }

         //call user function if available
         if(log.funcBoundary) log.funcBoundary(0, log.textBuf);

         //call this function periodically, if set so
         if(log.runInterval)
            setTimeout(log.capture, log.interval, log);
      }
      req.send(null);
   }
};
