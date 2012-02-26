/*******************************************************************************

   File: CO_driver.h
   CAN module object for Microchip dsPIC30F and Microchip C30 compiler (>= V3.00).

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

#ifndef _CO_DRIVER_H
#define _CO_DRIVER_H


#include <p30fxxxx.h>       //processor header file


/*******************************************************************************
   Topic: Reception of CAN messages

   Received message is processed by <Receive CAN message function>.

   CAN device has number of receive buffers equal to number of different CANopen
   receiving objects. Each buffer accepts CAN messages with specific identifier
   (which can also be masked). If CAN message is accepted by specific buffer,
   then function pointed by _pFunct_ is called with two parameters: pointer
   to object and pointer to received mesaage. Function is valid for specific
   CANopen object and it copies contents from received message into variables
   inside object. For more information see <Receive CAN message function> and
   <CO_CANrxBufferInit> functions.

   Benefits of this principle are: fast response, low memory usage and very
   high flexibility.


********************************************************************************
   Topic: Transmission of CAN messages

   CAN device has number of transmit buffers equal to number of different CANopen
   transmitting objects. Buffers are stored inside <CO_CANmodule_t> as members
   of <CO_CANtxArray_t>.

   Each buffer must be configured with <CO_CANtxBufferInit> function before it
   is used. Function returns pointer to 8 byte CAN data buffer which should be
   populated before CAN message is sent.

   CANmessage is send with calling <CO_CANsend> function, which accepts two
   argument: pointer to CAN device and pointer to tx buffer returned by 
   <CO_CANtxBufferInit> function. If any hardware CAN transmit buffer is free,
   message is copied directly to CAN module and _bufferFull_ variable from
   <CO_CANtxArray_t> stays false. If no hardware CAN transmit buffer is free,
   <CO_CANsend> function sets _bufferFull_ to true; message will be then sent by
   CAN TX interrupt. Until message is not copied to CAN module, its contents
   must not change.

   Advantages of this principle:
    - Code for transmission of messages is very fast.
    - Higher priority messages are transmited first.
    - Buffer full is indicated only on the individual messages. Other buffers
      may be still free.
*******************************************************************************/


/*******************************************************************************
   Constants: CANopen basic data types

   For dsPIC30F with Microchip C30:
      CO_OD_ROM_IDENT   - Identifier for CO_OD_ROM varibbles is 'const', so variables are stored in program flash.
      UNSIGNED8         - Of type unsigned char
      UNSIGNED16        - Of type unsigned int
      UNSIGNED32        - Of type unsigned long int
      UNSIGNED64        - Of type unsigned long long int
      INTEGER8          - Of type char
      INTEGER16         - Of type int
      INTEGER32         - Of type long
      INTEGER64         - Of type long long int
      REAL32            - Of type float
      REAL64            - Of type long double
      VISIBLE_STRING    - Of type char
      OCTET_STRING      - Of type unsigned char
*******************************************************************************/
   #define CO_OD_ROM_IDENT const

   #define UNSIGNED8       unsigned char
   #define UNSIGNED16      unsigned int
   #define UNSIGNED32      unsigned long int
   #define UNSIGNED64      unsigned long long int
   #define INTEGER8        char
   #define INTEGER16       int
   #define INTEGER32       long int
   #define INTEGER64       long long int
   #define REAL32          float
   #define REAL64          long double
   #define VISIBLE_STRING  char
   #define OCTET_STRING    unsigned char


/*******************************************************************************
   Constants: CO_ReturnError

   Return values of most CANopen functions. If function was executed
   successfully it returns 0 otherwise it returns <0.

    0 = CO_ERROR_NO                 - Operation completed successfully.
   -1 = CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
   -2 = CO_ERROR_OUT_OF_MEMORY      - Memory allocation failed.
   -3 = CO_ERROR_TIMEOUT            - Function timeout.
   -4 = CO_ERROR_ILLEGAL_BAUDRATE   - Illegal baudrate passed to function <CO_CANmodule_init>
   -5 = CO_ERROR_RX_OVERFLOW        - Previous message was not processed yet.
   -6 = CO_ERROR_RX_PDO_OWERFLOW    - previous PDO was not processed yet.
   -7 = CO_ERROR_RX_MSG_LENGTH      - Wrong receive message length.
   -8 = CO_ERROR_RX_PDO_LENGTH      - Wrong receive PDO length.
   -9 = CO_ERROR_TX_OVERFLOW        - Previous message is still waiting, buffer full.
   -10 = CO_ERROR_TX_PDO_WINDOW     - Synchronous TPDO is outside window.
   -11 = CO_ERROR_TX_UNCONFIGURED   - Transmit buffer was not confugured properly.
*******************************************************************************/
   enum CO_ReturnError{
      CO_ERROR_NO                = 0,
      CO_ERROR_ILLEGAL_ARGUMENT  = -1,
      CO_ERROR_OUT_OF_MEMORY     = -2,
      CO_ERROR_TIMEOUT           = -3,
      CO_ERROR_ILLEGAL_BAUDRATE  = -4,
      CO_ERROR_RX_OVERFLOW       = -5,
      CO_ERROR_RX_PDO_OWERFLOW   = -6,
      CO_ERROR_RX_MSG_LENGTH     = -7,
      CO_ERROR_RX_PDO_LENGTH     = -8,
      CO_ERROR_TX_OVERFLOW       = -9,
      CO_ERROR_TX_PDO_WINDOW     = -10,
      CO_ERROR_TX_UNCONFIGURED   = -11,
   };


/*******************************************************************************
   Constants: Retentive memory usage

   CO_USE_FLASH_WRITE      - If defined, then <memcpyram2flash> will be used to
                             save ROM variables from Object Dictionary into
                             internal flash memory. (EEPROM variables are
                             handled by <EEPROM> object.)
*******************************************************************************/
#define CO_USE_FLASH_WRITE


/*******************************************************************************
   Constants: Peripheral addresses

   ADDR_CAN1      - Starting address of CAN module 1 registers.
   ADDR_CAN2      - Starting address of CAN module 2 registers.
*******************************************************************************/
#define ADDR_CAN1    0x300
#define ADDR_CAN2    0x3C0


/*******************************************************************************
   Macros: Masking interrupts

   Interrupt masking is used to protect critical sections.

   It is used in few places in library to protect short sections of code.

      DISABLE_INTERRUPTS()     - Disable all interrupts.
      ENABLE_INTERRUPTS()      - Reenable interrupts.
*******************************************************************************/
#define DISABLE_INTERRUPTS()     asm volatile ("disi #0x3FFF")
#define ENABLE_INTERRUPTS()      asm volatile ("disi #0x0000")


/*******************************************************************************
   Macros: CAN bit rates

   CAN bit rates are initializers for array of eight <CO_CANbitRateData_t>
   objects.

   Macros are not used by driver itself, they may be used by application with
   combination with object <CO_CANbitRateData_t>.
   Application must declare following global variable depending on CO_FCY used:
   const CO_CANbitRateData_t  CO_CANbitRateData[8] = {CO_CANbitRateDataInitializers};

   There are initializers for eight objects, which corresponds to following
   CAN bit rates (in kbps): 10, 20, 50, 125, 250, 500, 800, 1000.

   CO_FCY is internal instruction cycle clock frequency in kHz units. It is
   calculated from oscillator frequency (FOSC [in kHz]) and PLL mode:
      - If PLL is not used -> FCY = FOSC / 4,
      - If PLL x 4 is used -> FCY = FOSC,
      - If PLL x 16 is used -> FCY = FOSC * 4

   Possible values for FCY are (in three groups):
      - Optimal CAN bit timing on all Baud Rates: 4000, 6000, 16000, 24000.
      - Not so optimal CAN bit timing on all Baud Rates: 2000, 8000.
      - not all CANopen Baud Rates possible: 1000, 1500, 2500, 3000, 5000,
        10000, 12000, 20000, 28000, 30000, 1843 (internal FRC, no PLL),
        7372 (internal FRC + 4*PLL).
*******************************************************************************/
#ifdef CO_FCY
   //Macros, which divides K into (SJW + PROP + PhSeg1 + PhSeg2)
   #define TQ_x_4    1, 1, 1, 1
   #define TQ_x_5    1, 1, 2, 1
   #define TQ_x_6    1, 1, 3, 1
   #define TQ_x_8    1, 2, 3, 2
   #define TQ_x_9    1, 2, 4, 2
   #define TQ_x_10   1, 3, 4, 2
   #define TQ_x_12   1, 3, 6, 2
   #define TQ_x_14   1, 4, 7, 2
   #define TQ_x_15   1, 4, 8, 2  //good timing
   #define TQ_x_16   1, 5, 8, 2  //good timing
   #define TQ_x_19   1, 8, 8, 2  //good timing
   #define TQ_x_20   1, 8, 8, 3  //good timing
   #define TQ_x_21   1, 8, 8, 4
   #define TQ_x_25   1, 8, 8, 8

   #if CO_FCY == 1000
      #define CO_CANbitRateDataInitializers  \
      {4, 10,  TQ_x_20},   /*CAN=10kbps*/    \
      {4, 5,   TQ_x_20},   /*CAN=20kbps*/    \
      {4, 2,   TQ_x_20},   /*CAN=50kbps*/    \
      {4, 1,   TQ_x_16},   /*CAN=125kbps*/   \
      {4, 1,   TQ_x_8 },   /*CAN=250kbps*/   \
      {4, 1,   TQ_x_4 },   /*CAN=500kbps*/   \
      {4, 1,   TQ_x_4 },   /*Not possible*/  \
      {4, 1,   TQ_x_4 }    /*Not possible*/
   #elif CO_FCY == 1500
      #define CO_CANbitRateDataInitializers  \
      {4, 15,  TQ_x_20},   /*CAN=10kbps*/    \
      {4, 10,  TQ_x_15},   /*CAN=20kbps*/    \
      {4, 4,   TQ_x_15},   /*CAN=50kbps*/    \
      {4, 2,   TQ_x_12},   /*CAN=125kbps*/   \
      {4, 1,   TQ_x_12},   /*CAN=250kbps*/   \
      {4, 1,   TQ_x_6 },   /*CAN=500kbps*/   \
      {4, 1,   TQ_x_6 },   /*Not possible*/  \
      {4, 1,   TQ_x_6 }    /*Not possible*/
   #elif CO_FCY == 1843                //internal FRC, no PLL
      #define CO_CANbitRateDataInitializers  \
      {4, 23,  TQ_x_16},   /*CAN=10kbps*/    \
      {4, 23,  TQ_x_8 },   /*CAN=20kbps*/    \
      {4, 23,  TQ_x_8 },   /*Not possible*/  \
      {4, 23,  TQ_x_8 },   /*Not possible*/  \
      {4, 23,  TQ_x_8 },   /*Not possible*/  \
      {4, 23,  TQ_x_8 },   /*Not possible*/  \
      {4, 23,  TQ_x_8 },   /*Not possible*/  \
      {4, 23,  TQ_x_8 }    /*Not possible*/
   #elif CO_FCY == 2000
      #define CO_CANbitRateDataInitializers  \
      {4, 25,  TQ_x_16},   /*CAN=10kbps*/    \
      {4, 10,  TQ_x_20},   /*CAN=20kbps*/    \
      {4, 5,   TQ_x_16},   /*CAN=50kbps*/    \
      {4, 2,   TQ_x_16},   /*CAN=125kbps*/   \
      {4, 1,   TQ_x_16},   /*CAN=250kbps*/   \
      {4, 1,   TQ_x_8 },   /*CAN=500kbps*/   \
      {4, 1,   TQ_x_5 },   /*CAN=800kbps*/   \
      {4, 1,   TQ_x_4 }    /*CAN=1000kbps*/
   #elif CO_FCY == 2500
      #define CO_CANbitRateDataInitializers  \
      {4, 25,  TQ_x_20},   /*CAN=10kbps*/    \
      {4, 10,  TQ_x_25},   /*CAN=20kbps*/    \
      {4, 5,   TQ_x_20},   /*CAN=50kbps*/    \
      {4, 2,   TQ_x_20},   /*CAN=125kbps*/   \
      {4, 1,   TQ_x_20},   /*CAN=250kbps*/   \
      {4, 1,   TQ_x_10},   /*CAN=500kbps*/   \
      {4, 1,   TQ_x_10},   /*Not possible*/  \
      {4, 1,   TQ_x_5 }    /*CAN=1000kbps*/
   #elif CO_FCY == 3000
      #define CO_CANbitRateDataInitializers  \
      {4, 40,  TQ_x_15},   /*CAN=10kbps*/    \
      {4, 20,  TQ_x_15},   /*CAN=20kbps*/    \
      {4, 8,   TQ_x_15},   /*CAN=50kbps*/    \
      {4, 3,   TQ_x_16},   /*CAN=125kbps*/   \
      {4, 2,   TQ_x_12},   /*CAN=250kbps*/   \
      {4, 1,   TQ_x_12},   /*CAN=500kbps*/   \
      {4, 1,   TQ_x_12},   /*Not possible*/  \
      {4, 1,   TQ_x_6 }    /*CAN=1000kbps*/
   #elif CO_FCY == 4000
      #define CO_CANbitRateDataInitializers  \
      {4, 50,  TQ_x_16},   /*CAN=10kbps*/    \
      {4, 25,  TQ_x_16},   /*CAN=20kbps*/    \
      {4, 10,  TQ_x_16},   /*CAN=50kbps*/    \
      {4, 4,   TQ_x_16},   /*CAN=125kbps*/   \
      {4, 2,   TQ_x_16},   /*CAN=250kbps*/   \
      {4, 1,   TQ_x_16},   /*CAN=500kbps*/   \
      {4, 1,   TQ_x_10},   /*CAN=800kbps*/   \
      {4, 1,   TQ_x_8 }    /*CAN=1000kbps*/
   #elif CO_FCY == 5000
      #define CO_CANbitRateDataInitializers  \
      {4, 50,  TQ_x_20},   /*CAN=10kbps*/    \
      {4, 25,  TQ_x_20},   /*CAN=20kbps*/    \
      {4, 10,  TQ_x_20},   /*CAN=50kbps*/    \
      {4, 5,   TQ_x_16},   /*CAN=125kbps*/   \
      {4, 2,   TQ_x_20},   /*CAN=250kbps*/   \
      {4, 1,   TQ_x_20},   /*CAN=500kbps*/   \
      {4, 1,   TQ_x_20},   /*Not possible*/  \
      {4, 1,   TQ_x_10}    /*CAN=1000kbps*/
   #elif CO_FCY == 6000
      #define CO_CANbitRateDataInitializers  \
      {4, 63,  TQ_x_19},   /*CAN=10kbps*/    \
      {4, 40,  TQ_x_15},   /*CAN=20kbps*/    \
      {4, 15,  TQ_x_16},   /*CAN=50kbps*/    \
      {4, 6,   TQ_x_16},   /*CAN=125kbps*/   \
      {4, 3,   TQ_x_16},   /*CAN=250kbps*/   \
      {4, 2,   TQ_x_12},   /*CAN=500kbps*/   \
      {4, 1,   TQ_x_15},   /*CAN=800kbps*/   \
      {4, 1,   TQ_x_12}    /*CAN=1000kbps*/
   #elif CO_FCY == 7372                //internal FRC + 4*PLL
      #define CO_CANbitRateDataInitializers  \
      {1, 23,  TQ_x_16},   /*CAN=10kbps*/    \
      {4, 46,  TQ_x_16},   /*CAN=20kbps*/    \
      {4, 14,  TQ_x_21},   /*CAN=50kbps*/    \
      {4, 13,  TQ_x_9 },   /*CAN=125kbps*/   \
      {4, 13,  TQ_x_9 },   /*Not possible*/  \
      {4, 13,  TQ_x_9 },   /*Not possible*/  \
      {4, 13,  TQ_x_9 },   /*Not possible*/  \
      {4, 13,  TQ_x_9 }    /*Not possible*/
   #elif CO_FCY == 8000
      #define CO_CANbitRateDataInitializers  \
      {1, 25,  TQ_x_16},   /*CAN=10kbps*/    \
      {1, 10,  TQ_x_20},   /*CAN=20kbps*/    \
      {1, 5,   TQ_x_16},   /*CAN=50kbps*/    \
      {1, 2,   TQ_x_16},   /*CAN=125kbps*/   \
      {1, 1,   TQ_x_16},   /*CAN=250kbps*/   \
      {1, 1,   TQ_x_8 },   /*CAN=500kbps*/   \
      {1, 1,   TQ_x_5 },   /*CAN=800kbps*/   \
      {1, 1,   TQ_x_4 }    /*CAN=1000kbps*/
   #elif CO_FCY == 10000
      #define CO_CANbitRateDataInitializers  \
      {1, 25,  TQ_x_20},   /*CAN=10kbps*/    \
      {1, 10,  TQ_x_25},   /*CAN=20kbps*/    \
      {1, 5,   TQ_x_20},   /*CAN=50kbps*/    \
      {1, 2,   TQ_x_20},   /*CAN=125kbps*/   \
      {1, 1,   TQ_x_20},   /*CAN=250kbps*/   \
      {1, 1,   TQ_x_10},   /*CAN=500kbps*/   \
      {1, 1,   TQ_x_10},   /*Not possible*/  \
      {1, 1,   TQ_x_5 }    /*CAN=1000kbps*/
   #elif CO_FCY == 12000
      #define CO_CANbitRateDataInitializers  \
      {1, 40,  TQ_x_15},   /*CAN=10kbps*/    \
      {1, 20,  TQ_x_15},   /*CAN=20kbps*/    \
      {1, 8,   TQ_x_15},   /*CAN=50kbps*/    \
      {1, 3,   TQ_x_16},   /*CAN=125kbps*/   \
      {1, 2,   TQ_x_12},   /*CAN=250kbps*/   \
      {1, 1,   TQ_x_12},   /*CAN=500kbps*/   \
      {1, 1,   TQ_x_12},   /*Not possible*/  \
      {1, 1,   TQ_x_6 }    /*CAN=1000kbps*/
   #elif CO_FCY == 16000
      #define CO_CANbitRateDataInitializers  \
      {1, 50,  TQ_x_16},   /*CAN=10kbps*/    \
      {1, 25,  TQ_x_16},   /*CAN=20kbps*/    \
      {1, 10,  TQ_x_16},   /*CAN=50kbps*/    \
      {1, 4,   TQ_x_16},   /*CAN=125kbps*/   \
      {1, 2,   TQ_x_16},   /*CAN=250kbps*/   \
      {1, 1,   TQ_x_16},   /*CAN=500kbps*/   \
      {1, 1,   TQ_x_10},   /*CAN=800kbps*/   \
      {1, 1,   TQ_x_8 }    /*CAN=1000kbps*/
   #elif CO_FCY == 20000
      #define CO_CANbitRateDataInitializers  \
      {1, 50,  TQ_x_20},   /*CAN=10kbps*/    \
      {1, 25,  TQ_x_20},   /*CAN=20kbps*/    \
      {1, 10,  TQ_x_20},   /*CAN=50kbps*/    \
      {1, 5,   TQ_x_16},   /*CAN=125kbps*/   \
      {1, 2,   TQ_x_20},   /*CAN=250kbps*/   \
      {1, 1,   TQ_x_20},   /*CAN=500kbps*/   \
      {1, 1,   TQ_x_20},   /*Not possible*/  \
      {1, 1,   TQ_x_10}    /*CAN=1000kbps*/
   #elif CO_FCY == 24000
      #define CO_CANbitRateDataInitializers  \
      {1, 63,  TQ_x_19},   /*CAN=10kbps*/    \
      {1, 40,  TQ_x_15},   /*CAN=20kbps*/    \
      {1, 15,  TQ_x_16},   /*CAN=50kbps*/    \
      {1, 6,   TQ_x_16},   /*CAN=125kbps*/   \
      {1, 3,   TQ_x_16},   /*CAN=250kbps*/   \
      {1, 2,   TQ_x_12},   /*CAN=500kbps*/   \
      {1, 1,   TQ_x_15},   /*CAN=800kbps*/   \
      {1, 1,   TQ_x_12}    /*CAN=1000kbps*/
   #elif CO_FCY == 28000
      #define CO_CANbitRateDataInitializers  \
      {1, 56,  TQ_x_25},   /*CAN=10kbps*/    \
      {1, 35,  TQ_x_20},   /*CAN=20kbps*/    \
      {1, 14,  TQ_x_20},   /*CAN=50kbps*/    \
      {1, 7,   TQ_x_16},   /*CAN=125kbps*/   \
      {1, 4,   TQ_x_14},   /*CAN=250kbps*/   \
      {1, 2,   TQ_x_14},   /*CAN=500kbps*/   \
      {1, 2,   TQ_x_14},   /*Not possible*/  \
      {1, 1,   TQ_x_14}    /*CAN=1000kbps*/
   #elif CO_FCY == 30000
      #define CO_CANbitRateDataInitializers  \
      {1, 60,  TQ_x_25},   /*CAN=10kbps*/    \
      {1, 50,  TQ_x_15},   /*CAN=20kbps*/    \
      {1, 20,  TQ_x_15},   /*CAN=50kbps*/    \
      {1, 8,   TQ_x_15},   /*CAN=125kbps*/   \
      {1, 4,   TQ_x_15},   /*CAN=250kbps*/   \
      {1, 2,   TQ_x_15},   /*CAN=500kbps*/   \
      {1, 2,   TQ_x_15},   /*Not possible*/  \
      {1, 1,   TQ_x_15}    /*CAN=1000kbps*/
   #else
      #error define_CO_FCY CO_FCY not supported
   #endif
#endif


/*******************************************************************************
   Object: CO_CANbitRateData_t

   Structure contains timing coefficients for CAN module. See <CAN bit rates>.

   CAN baud rate is calculated from following equations:
   FCAN = FCY * Scale                  - Input frequency to CAN module (MAX 30MHz for dsPIC30F)
   TQ = 2 * BRP / FCAN                 - Time Quanta
   BaudRate = 1 / (TQ * K)             - Can bus Baud Rate
   K = SJW + PROP + PhSeg1 + PhSeg2    - Number of Time Quantas

   Variables:
      scale    - (1 or 4) Scales FCY clock - dsPIC30F specific.
      BRP      - (1...64) Baud Rate Prescaler.
      SJW      - (1...4) SJW time.
      PROP     - (1...8) PROP time.
      PhSeg1   - (1...8) Phase Segment 1 time.
      PhSeg2   - (1...8) Phase Segment 2 time.
*******************************************************************************/
typedef struct{
   UNSIGNED8   scale;
   UNSIGNED8   BRP;
   UNSIGNED8   SJW;
   UNSIGNED8   PROP;
   UNSIGNED8   phSeg1;
   UNSIGNED8   phSeg2;
}CO_CANbitRateData_t;


/*******************************************************************************
   Object: CO_CANrxMsg_t

   CAN receive message structure as aligned in CAN module. Object is passed to
   <Receive CAN message function>. In general, that function only uses _DLC_ and
   _data_ parameters. If it needs to read identifier, it must use function
   <CO_CANrxMsg_readIdent>.

   Variables:
      ident             - Standard Identifier as aligned in CAN module. 16 bits:
                          'UUUSSSSS SSSSSSRE' (U: unused; S: SID; R=SRR; E=IDE).
      extIdent          - Extended identifier, not used here
      DLC               - Data length code (bits 0...3).
      DLCrest           - Not used here (bits 4..15).
      data              - 8 data bytes.
      CON               - Control word.
*******************************************************************************/
typedef struct{
   UNSIGNED16        ident;
   UNSIGNED16        extIdent;
   UNSIGNED16        DLC      :4;
   UNSIGNED16        DLCrest  :12;
   UNSIGNED8         data[8];
   UNSIGNED16        CON;
}CO_CANrxMsg_t;


/*******************************************************************************
   Object: CO_CANrxArray_t

   Array for handling received CAN messages.

   Variables:
      ident    - Standard Identifier as aligned in CAN module.
      mask     - Standard Identifier mask as aligned in CAN module.
      object   - See parameters in <CO_CANrxBufferInit>.
      pFunct   - See parameters in <CO_CANrxBufferInit>.
*******************************************************************************/
typedef struct{
   UNSIGNED16        ident;
   UNSIGNED16        mask;
   void             *object;
   INTEGER16       (*pFunct)(void *object, CO_CANrxMsg_t *message);
}CO_CANrxArray_t;


/*******************************************************************************
   Object: CO_CANtxArray_t

   Array for handling transmitting CAN messages.

   Variables:
      ident             - Standard Identifier as aligned in CAN module. 16 bits:
                          'SSSSSUUU SSSSSSRE' (U: unused; S: SID; R=SRR; E=IDE).
      DLC               - Data length code.
      data              - 8 data bytes.
      bufferFull        - Previous message is still in buffer.
      syncFlag          - Synchronous PDO messages has this flag set. It prevents them to be sent outside the synchronous window.
*******************************************************************************/
typedef struct{
   UNSIGNED16        ident;
   UNSIGNED8         DLC;
   UNSIGNED8         data[8];
   UNSIGNED8         bufferFull;
   UNSIGNED8         syncFlag;
}CO_CANtxArray_t;


/*******************************************************************************
   Object: CO_CANmodule_t

   Object controls CAN module.

   Variables:
      CANbaseAddress    - See parameters in <CO_CANmodule_init>.
      rxArray           - See parameters in <CO_CANmodule_init>.
      rxSize            - See parameters in <CO_CANmodule_init>.
      txArray           - See parameters in <CO_CANmodule_init>.
      txSize            - See parameters in <CO_CANmodule_init>.
      curentSyncTimeIsInsideWindow - Pointer to variable with same name inside
                          <CO_SYNC_t> object. This pointer is configured inside
                          <CO_SYNC_init> function.
      bufferInhibitFlag - If flag is true, then message in transmitt buffer is
                          synchronous PDO message, which will be aborted, if
                          <CO_clearPendingSyncPDOs> function will be called by
                          application. This may be necessary if Synchronous
                          window time was expired.
      transmittingAborted - If true, CAN transmit buffer was just aborted.
      firstCANtxMessage - Equal to 1, when the first transmitted message
                          (bootup message) is in CAN TX buffers.
      CANtxCount        - Number of messages in transmit buffer, which are
                          waiting to be copied to CAN module.
      errOld            - Previous state of CAN errors.
      EM                - Pointer to Emergency object <CO_emergencyReport_t>.
*******************************************************************************/
typedef struct{
   UNSIGNED16              CANbaseAddress;
   CO_CANrxArray_t        *rxArray;
   UNSIGNED16              rxSize;
   CO_CANtxArray_t        *txArray;
   UNSIGNED16              txSize;
   UNSIGNED8              *curentSyncTimeIsInsideWindow;
   UNSIGNED8               bufferInhibitFlag;
   UNSIGNED8               transmittingAborted;
   UNSIGNED8               firstCANtxMessage;
   UNSIGNED16              CANtxCount;
   UNSIGNED8               errOld;
   void                   *EM;
}CO_CANmodule_t;


/*******************************************************************************
   Function: memcpySwapN

   Copy N data bytes from source to destination. Swap bytes if necessary.
   CANopen uses little endian. dsPIC33F and PIC24H with Microchip C30 uses
   little endian. Functions just copy bytes without inversion.

   Parameters:
      dest        - Destination location.
      src         - Source location.
*******************************************************************************/
void memcpySwap2(UNSIGNED8* dest, UNSIGNED8* src);
void memcpySwap4(UNSIGNED8* dest, UNSIGNED8* src);


/*******************************************************************************
   Function: Receive CAN message function

   Process received CAN message.

   Different CANopen objects (like SDO) register their own functions, which
   will be called by CAN receive interrupt every time, when CAN message with
   matched identifier will be received. Function is then responsible to verify
   some basics, like message length or overflow. It must copy message contents
   to objects buffer.

   Parameters:
      object   - Pointer to object registered by <CO_CANrxBufferInit>.
      msg      - Pointer to CAN message (directly on CAN module) <CO_CANrxMsg_t>.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_RX_OVERFLOW        - Previous message was not processed yet.
      CO_ERROR_RX_PDO_OWERFLOW    - previous PDO was not processed yet.
      CO_ERROR_RX_MSG_LENGTH      - Wrong receive message length.
      CO_ERROR_RX_PDO_LENGTH      - Wrong receive PDO length.
*******************************************************************************/


/*******************************************************************************
   Function: CO_CANsetConfigurationMode

   Requests CAN configuration (stopped) mode and *wait* untill it is set.

   Parameters:
      CANbaseAddress    - CAN module base address.
*******************************************************************************/
void CO_CANsetConfigurationMode(UNSIGNED16 CANbaseAddress);


/*******************************************************************************
   Function: CO_CANsetNormalMode

   Requests CAN normal (opearational) mode and *wait* untill it is set.

   Parameters:
      CANbaseAddress    - CAN module base address.
*******************************************************************************/
void CO_CANsetNormalMode(UNSIGNED16 CANbaseAddress);


/*******************************************************************************
   Function: CO_CANmodule_init

   Initialize CAN module object.

   Function must be called in the communication reset section. CAN module must
   be in Configuration Mode before.

   Parameters:
      ppCANmodule       - Pointer to address of CAN module object <CO_CANmodule_t>.
                          If address is zero, memory for new object will be
                          allocated and address will be set.
      CANbaseAddress    - CAN module base address. See <Peripheral addresses>.
      rxSize            - Size of above array is equal to number of receiving CAN objects.
      txSize            - Size of above array is equal to number of transmitting CAN objects.
      CANbitRate        - CAN bit rate. Valid values are (in kbps): 10, 20, 50,
                          125, 250, 500, 800, 1000. If value is illegal, bitrate
                          defaults to 125.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
      CO_ERROR_OUT_OF_MEMORY      - Memory allocation failed.
      CO_ERROR_ILLEGAL_BAUDRATE   - Illegal baudrate passed to function <CO_CANmodule_init>
*******************************************************************************/
INTEGER16 CO_CANmodule_init(
      CO_CANmodule_t     **ppCANmodule,
      UNSIGNED16           CANbaseAddress,
      UNSIGNED16           rxSize,
      UNSIGNED16           txSize,
      UNSIGNED16           CANbitRate);


/*******************************************************************************
   Function: CO_CANmodule_delete

   Delete CANmodule object and free memory.

   Parameters:
      ppCANmodule       - Pointer to pointer to CAN module object <CO_CANmodule_t>.
                          Pointer to CAN module object is set to 0.
*******************************************************************************/
void CO_CANmodule_delete(CO_CANmodule_t **ppCANmodule);


/*******************************************************************************
   Function: CO_CANrxMsg_readIdent

   Reads CAN identifier from received message.

   Parameters:
      rxMsg    - Pointer to received message. See <CO_CANrxMsg_t>.

   Return:
      11-bit CAN standard identifier.
*******************************************************************************/
UNSIGNED16 CO_CANrxMsg_readIdent(CO_CANrxMsg_t *rxMsg);


/*******************************************************************************
   Function: CO_CANrxBufferInit

   Configure CAN message receive buffer.

   Function configures specific CAN receive buffer. It sets CAN identifier
   and connects buffer with specific object. For more information see
   topic <Reception of CAN messages>.

   Parameters:
      CANmodule   - Pointer to CAN module object <CO_CANmodule_t>.
      index       - Index of CAN device receive buffer.
      ident       - 11-bit standard CAN Identifier.
      mask        - 11-bit mask for identifier. Most usually set to 0x7FF.
                    Received message (rcvMsg) will be accepted if the following
                    condition is true: (((rcvMsgId ^ ident) & mask) == 0).
      rtr         - If true, 'Remote Transmit Request' messages will be accepted.
      object      - CANopen object, to which buffer is connected. It will be used as
                    an argument to pFunct. Its type is (void), pFunct will change its
                    type back to the correct object type.
      pFunct      - Pointer to function, which will be called, if received CAN
                    message matches the identifier. It must be fast function.
                    See <Receive CAN message function>.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
*******************************************************************************/
INTEGER16 CO_CANrxBufferInit( CO_CANmodule_t   *CANmodule,
                              UNSIGNED16        index,
                              UNSIGNED16        ident,
                              UNSIGNED16        mask,
                              UNSIGNED8         rtr,
                              void             *object,
                              INTEGER16       (*pFunct)(void *object, CO_CANrxMsg_t *message));


/*******************************************************************************
   Function: CO_CANtxBufferInit

   Configure CAN message transmit buffer.

   Function configures specific CAN transmit buffer. For more information see
   topic <Transmission of CAN messages>.

   Parameters:
      CANmodule   - Pointer to CAN module object <CO_CANmodule_t>.
      index       - Index of CAN device transmit buffer to configure.
      ident       - 11-bit standard CAN Identifier.
      rtr         - If true, 'Remote Transmit Request' messages will be transmitted.
      noOfBytes   - Length of CAN message in bytes (0 to 8 bytes).
      syncFlag    - This flag bit is used for synchronous TPDO messages. If it is set,
                    message will not be sent, if curent time is outside synchronous window.

   Return:
                    Pointer to CAN transmit message buffer <CO_CANtxArray_t>. 
                    8 bytes data array inside buffer should be written, before
                    <CO_CANsend> function is called.
                    Zero is returned in case of wrong arguments.
*******************************************************************************/
CO_CANtxArray_t *CO_CANtxBufferInit(
                              CO_CANmodule_t   *CANmodule,
                              UNSIGNED16        index,
                              UNSIGNED16        ident,
                              UNSIGNED8         rtr,
                              UNSIGNED8         noOfBytes,
                              UNSIGNED8         syncFlag);


/*******************************************************************************
   Function: CO_CANsend

   Send CAN message.

   For more information see topic <Transmission of CAN messages>. Before this
   function <CO_CANtxBufferInit> must be caled, which configures TX message
   buffer. Also 8 bytes of CAN data should be populated.

   Parameters:
      CANmodule   - Pointer to CAN module object <CO_CANmodule_t>.
      buffer      - Pointer to transmit buffer, returned by <CO_CANtxBufferInit>.

   Return <CO_ReturnError>:
      CO_ERROR_NO             - Operation completed successfully.
      CO_ERROR_TX_OVERFLOW    - Previous message is still waiting, buffer full.
      CO_ERROR_TX_PDO_WINDOW  - Synchronous TPDO is outside window.
*******************************************************************************/
INTEGER16 CO_CANsend(   CO_CANmodule_t   *CANmodule,
                        CO_CANtxArray_t  *buffer);


/*******************************************************************************
   Function: CO_CANclearPendingSyncPDOs

   Clear all synchronous TPDOs from CAN module transmit buffers.

   CANopen allows synchronous PDO communication only inside time between SYNC
   message and SYNC Window. If time is outside this window, new synch. PDOs
   must not be sent and all pending sync TPDOs, which may be on CAN TX buffers,
   must be cleared.

   This function checks (and aborts transmission if necessary) CAN TX buffers
   when it is called. Function should be called by application in the moment,
   when SYNC time was just passed out of synchronous window.

   Parameters:
      CANmodule   - Pointer to CAN module object <CO_CANmodule_t>.
*******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule);


/*******************************************************************************
   Function: CO_CANverifyErrors

   Verify all errors of CAN module.

   Function is called directly from <CO_emergency_process> function.

   Parameters:
      CANmodule   - Pointer to CAN module object <CO_CANmodule_t>.
*******************************************************************************/
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule);


/*******************************************************************************
   Function: CO_CANinterrupt

   CAN interrupt receives and transmits CAN messages.

   Function must be called directly from _C1Interrupt or _C2Interrupt with
   high priority. dsPIC30F uses two receive buffers and one transmit buffer.
   For more information see topics: <Reception of CAN messages>
   and <Transmission of CAN messages>.

   Parameters:
      CANmodule   - Pointer to CAN module object <CO_CANmodule_t>.
*******************************************************************************/
void CO_CANinterrupt(CO_CANmodule_t *CANmodule);


/*******************************************************************************
   Function: CO_ODF

   Default function for SDO server access of variables from Object Dictionary.

   dsPIC30F specifics: if variable is RAM or EEPROM type, normal copy is used.
   If variable is ROM type and macro <CO_USE_FLASH_WRITE> is defined,
   <memcpyram2flash> function is used. It uses microcontroler's self programming
   technique. CANopen and dsPIC30F uses Little endian, so no byte inversion is
   implemented. With Big endianes, bytes must be inverted if CO_ODA_MB_VALUE bit
   is true.

   For more information see topic <SDO server access function> in CO_SDO.h file.
*******************************************************************************/
UNSIGNED32 CO_ODF(   void       *object,
                     UNSIGNED16  index,
                     UNSIGNED8   subIndex,
                     UNSIGNED8   length,
                     UNSIGNED16  attribute,
                     UNSIGNED8   dir,
                     void       *dataBuff,
                     const void *pData);


#endif
