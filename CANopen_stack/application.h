/*******************************************************************************

   File: application.h
   Header for application interface for CANopenNode stack.

   Copyright (C) 2010 Janez Paternoster

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


/*******************************************************************************
Topic: Main program flow chart.

>              (Program Start)
>                     |
>                     V
>   +------------------------------------+
>   |           programStart()           |
>   +------------------------------------+
>                     |
>                     |<-------------------------+
>                     |                          |
>                     V                          |
>            (Initialze CANopen)                 |
>                     |                          |
>                     V                          |
>   +------------------------------------+       |
>   |        communicationReset()        |       |
>   +------------------------------------+       |
>                     |                          |
>                     V                          |
>        (Enable CAN and interrupts)             |
>                     |                          |
>                     |<----------------------+  |
>                     |                       |  |
>                     V                       |  |
>   +------------------------------------+    |  |
>   |           programAsync()           |    |  |
>   +------------------------------------+    |  |
>                     |                       |  |
>                     V                       |  |
>       (Process CANopen asynchronous)        |  |
>                     |                       |  |
>                     +- infinite loop -------+  |
>                     |                          |
>                     +- reset communication ----+
>                     |
>                     V
>   +------------------------------------+
>   |            programEnd()            |
>   +------------------------------------+
>                     |
>                     V
>             (delete CANopen)
>                     |
>                     V
>               (Program end)


Topic: Timer program flow chart.

>       (Timer interrupt 1 millisecond)
>                     |
>                     V
>             (CANopen read RPDOs)
>                     |
>                     V
>   +------------------------------------+
>   |           program1ms()             |
>   +------------------------------------+
>                     |
>                     V
>             (CANopen write TPDOs)


Topic: Receive and transmit high priority interrupt flow chart.

>          (CAN receive event or)
>     (CAN transmit buffer empty event)
>                     |
>                     V
>      (Process received CAN message or)
>  (copy next message to CAN transmit buffer)

*******************************************************************************/
void programStart(void);
void communicationReset(void);
void programEnd(void);
void programAsync(unsigned int timer1msDiff); //time difference since last call
void program1ms(void);
