CANopenNode is an opensource CANopen Stack.

CANopen is the internationally standardized (EN 50325-4) CAN-based higher-layer protocol for embedded control system. For more information on CANopen see http://www.can-cia.org/

Stack is written in ANSI C in object-oriented way. Code is documented. License is LGPL. It includes Example for CANopen Input/Output device.

Project also includes Object Dictionary Editor. Here CANopen accessible variables can be edited.
Editor generates: object dictionary .c and .h files, CANopen EDS and XDD files and HTML documentation file.
On some controllers is implemented memory retention of selected variables from object dictionary.


CANopen Features:
 - NMT slave,
 - Heartbeat producer/consumer error control,
 - PDO linking and dynamic mapping,
 - SDO expedited and segmented transfer,
 - SDO master,
 - Emergency message,
 - Sync producer/consumer,


Supported controllers:
 - From microchip.com:
   - dsPIC30F
   - PIC24H
   - dsPIC33F
   - PIC32
 - From beck-ipc.com
   - SC2x3

   
****************************************************************************************************
Version 3.02
Fixes:
 - SYNC object changes:
    - LED tripple falsh corrected.
    - No need for definition of communication cycle period (index 1006).
 - Correction in 'Object dictionary editor' output.
 - PIC32 now resets correctly when receives NMT command.
 - NMT master example now works.


****************************************************************************************************
Version 3.01
Fixes:
 - It compiles now without errors for all controllers.
 - Newly generated files from object dictionary, including CO_OD.c, .h.




****************************************************************************************************
Version 3.00

