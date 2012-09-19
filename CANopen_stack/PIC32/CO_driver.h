/*******************************************************************************

   File: CO_driver.h
   CAN module object for Microchip PIC32MX microcontroller.

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


#include <p32xxxx.h>       //processor header file
#include <plib.h>


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

   For PIC32MX:
      CO_OD_ROM_IDENT   - Identifier for CO_OD_ROM varibbles is none, so variables are stored in RAM.
      UNSIGNED8         - Of type unsigned char
      UNSIGNED16        - Of type unsigned short int
      UNSIGNED32        - Of type unsigned long int
      UNSIGNED64        - Of type unsigned long long int
      INTEGER8          - Of type signed char
      INTEGER16         - Of type signed short int
      INTEGER32         - Of type signed long
      INTEGER64         - Of type signed long long int
      REAL32            - Of type float
      REAL64            - Of type long double
      VISIBLE_STRING    - Of type char
      OCTET_STRING      - Of type unsigned char
      DOMAIN            - Application specific
*******************************************************************************/
   #define CO_OD_ROM_IDENT

   #define UNSIGNED8       unsigned char
   #define UNSIGNED16      unsigned short int
   #define UNSIGNED32      unsigned long int
   #define UNSIGNED64      unsigned long long int
   #define INTEGER8        signed char
   #define INTEGER16       signed short int
   #define INTEGER32       signed long int
   #define INTEGER64       signed long long int
   #define REAL32          float
   #define REAL64          long double
   #define VISIBLE_STRING  char
   #define OCTET_STRING    unsigned char
   #define DOMAIN          unsigned char


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
   -12 = CO_ERROR_PARAMETERS        - Error in function function parameters.
   -13 = CO_ERROR_DATA_CORRUPT      - Stored data are corrupt.
   -14 = CO_ERROR_CRC               - CRC does not match.
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
      CO_ERROR_PARAMETERS        = -12,
      CO_ERROR_DATA_CORRUPT      = -13,
      CO_ERROR_CRC               = -14
   };


/*******************************************************************************
   Constants: Peripheral addresses

   ADDR_CAN1      - Starting address of CAN module 1 registers.
   ADDR_CAN2      - Starting address of CAN module 2 registers.
 *******************************************************************************/
#define ADDR_CAN1    0
#define ADDR_CAN2    (_CAN2_BASE_ADDRESS - _CAN1_BASE_ADDRESS)


/*******************************************************************************
   Macros: Masking interrupts

   Interrupt masking is used to protect critical sections.

   It is used in some places in library to protect short sections of code in
   functions, which may be accessed from different tasks. Alternative solutions
   are possible, for example disable only timer interrupt, if there is only
   mainline and timer task.

      DISABLE_INTERRUPTS()     - Disable all interrupts.
      ENABLE_INTERRUPTS()      - Reenable interrupts.
*******************************************************************************/
extern unsigned int CO_interruptStatus;
#define DISABLE_INTERRUPTS()    CO_interruptStatus = INTDisableInterrupts()
#define ENABLE_INTERRUPTS()     INTRestoreInterrupts(CO_interruptStatus)


/*******************************************************************************
   Macros: CAN bit rates

   CAN bit rates are initializers for array of eight <CO_CANbitRateData_t>
   objects.

   Macros are not used by driver itself, they may be used by application with
   combination with object <CO_CANbitRateData_t>.
   Application must declare following global variable depending on CO_FSYS used:
   const CO_CANbitRateData_t  CO_CANbitRateData[8] = {CO_CANbitRateDataInitializers};

   There are initializers for eight objects, which corresponds to following
   CAN bit rates (in kbps): 10, 20, 50, 125, 250, 500, 800, 1000.

   CO_FSYS is internal instruction cycle clock frequency in kHz units. See
   PIC32MX documentation for more information on FSYS.

   Available values for FSYS:
>     kbps = | 10 | 20 | 50 | 125 | 250 | 500 | 800 | 1000
>     -------+----+----+----+-----+-----+-----+-----+-----
>      4 Mhz |  O |  O |  O |  O  |  p  |  -  |  -  |  -
>      8 Mhz |  O |  O |  O |  O  |  O  |  p  |  -  |  -
>     12 Mhz |  O |  O |  O |  O  |  p  |  p  |  -  |  -
>     16 Mhz |  O |  O |  O |  O  |  O  |  O  |  p  |  p
>     20 Mhz |  O |  O |  O |  O  |  O  |  O  |  -  |  p
>     24 Mhz |  O |  O |  O |  O  |  O  |  p  |  O  |  p
>     32 Mhz |  p |  O |  O |  O  |  O  |  O  |  p  |  O
>     36 Mhz |  - |  O |  O |  O  |  O  |  O  |  -  |  O
>     40 Mhz |  - |  O |  O |  O  |  O  |  O  |  p  |  O
>     48 Mhz |  - |  O |  O |  O  |  O  |  O  |  O  |  p
>     56 Mhz |  - |  p |  O |  O  |  O  |  p  | (p) |  p
>     64 Mhz |  - |  p |  O |  O  |  O  |  O  |  O  |  O
>     72 Mhz |  - |  - |  O |  O  |  O  |  O  |  O  |  O
>     80 Mhz |  - |  - |  O |  O  |  O  |  O  |  p  |  O
>     ----------------------------------------------------
>     (O=optimal; p=possible; -=not possible)
*******************************************************************************/
#ifdef CO_FSYS
   //Macros, which divides K into (SJW + PROP + PhSeg1 + PhSeg2)
   #define TQ_x_7    1, 2, 3, 1
   #define TQ_x_8    1, 2, 3, 2
   #define TQ_x_9    1, 2, 4, 2
   #define TQ_x_10   1, 3, 4, 2
   #define TQ_x_12   1, 3, 6, 2
   #define TQ_x_14   1, 4, 7, 2
   #define TQ_x_15   1, 4, 8, 2  //good timing
   #define TQ_x_16   1, 5, 8, 2  //good timing
   #define TQ_x_17   1, 6, 8, 2  //good timing
   #define TQ_x_18   1, 7, 8, 2  //good timing
   #define TQ_x_19   1, 8, 8, 2  //good timing
   #define TQ_x_20   1, 8, 8, 3  //good timing
   #define TQ_x_21   1, 8, 8, 4
   #define TQ_x_22   1, 8, 8, 5
   #define TQ_x_23   1, 8, 8, 6
   #define TQ_x_24   1, 8, 8, 7
   #define TQ_x_25   1, 8, 8, 8

   #if CO_FSYS == 4000
      #define CO_CANbitRateDataInitializers  \
      {10,  TQ_x_20},   /*CAN=10kbps*/       \
      {5,   TQ_x_20},   /*CAN=20kbps*/       \
      {2,   TQ_x_20},   /*CAN=50kbps*/       \
      {1,   TQ_x_16},   /*CAN=125kbps*/      \
      {1,   TQ_x_8 },   /*CAN=250kbps*/      \
      {1,   TQ_x_8 },   /*Not possible*/     \
      {1,   TQ_x_8 },   /*Not possible*/     \
      {1,   TQ_x_8 }    /*Not possible*/
   #elif CO_FSYS == 8000
      #define CO_CANbitRateDataInitializers  \
      {25,  TQ_x_16},   /*CAN=10kbps*/       \
      {10,  TQ_x_20},   /*CAN=20kbps*/       \
      {5,   TQ_x_16},   /*CAN=50kbps*/       \
      {2,   TQ_x_16},   /*CAN=125kbps*/      \
      {1,   TQ_x_16},   /*CAN=250kbps*/      \
      {1,   TQ_x_8 },   /*CAN=500kbps*/      \
      {1,   TQ_x_8 },   /*Not possible*/     \
      {1,   TQ_x_8 }    /*Not possible*/
   #elif CO_FSYS == 12000
      #define CO_CANbitRateDataInitializers  \
      {40,  TQ_x_15},   /*CAN=10kbps*/       \
      {20,  TQ_x_15},   /*CAN=20kbps*/       \
      {8,   TQ_x_15},   /*CAN=50kbps*/       \
      {3,   TQ_x_16},   /*CAN=125kbps*/      \
      {2,   TQ_x_12},   /*CAN=250kbps*/      \
      {1,   TQ_x_12},   /*CAN=500kbps*/      \
      {1,   TQ_x_12},   /*Not possible*/     \
      {1,   TQ_x_12}    /*Not possible*/
   #elif CO_FSYS == 16000
      #define CO_CANbitRateDataInitializers  \
      {50,  TQ_x_16},   /*CAN=10kbps*/       \
      {25,  TQ_x_16},   /*CAN=20kbps*/       \
      {10,  TQ_x_16},   /*CAN=50kbps*/       \
      {4,   TQ_x_16},   /*CAN=125kbps*/      \
      {2,   TQ_x_16},   /*CAN=250kbps*/      \
      {1,   TQ_x_16},   /*CAN=500kbps*/      \
      {1,   TQ_x_10},   /*CAN=800kbps*/      \
      {1,   TQ_x_8 }    /*CAN=1000kbps*/
   #elif CO_FSYS == 20000
      #define CO_CANbitRateDataInitializers  \
      {50,  TQ_x_20},   /*CAN=10kbps*/       \
      {25,  TQ_x_20},   /*CAN=20kbps*/       \
      {10,  TQ_x_20},   /*CAN=50kbps*/       \
      {5,   TQ_x_16},   /*CAN=125kbps*/      \
      {2,   TQ_x_20},   /*CAN=250kbps*/      \
      {1,   TQ_x_20},   /*CAN=500kbps*/      \
      {1,   TQ_x_20},   /*Not possible*/     \
      {1,   TQ_x_10}    /*CAN=1000kbps*/
   #elif CO_FSYS == 24000
      #define CO_CANbitRateDataInitializers  \
      {63,  TQ_x_19},   /*CAN=10kbps*/       \
      {40,  TQ_x_15},   /*CAN=20kbps*/       \
      {15,  TQ_x_16},   /*CAN=50kbps*/       \
      {6,   TQ_x_16},   /*CAN=125kbps*/      \
      {3,   TQ_x_16},   /*CAN=250kbps*/      \
      {2,   TQ_x_12},   /*CAN=500kbps*/      \
      {1,   TQ_x_15},   /*CAN=800kbps*/      \
      {1,   TQ_x_12}    /*CAN=1000kbps*/
   #elif CO_FSYS == 32000
      #define CO_CANbitRateDataInitializers  \
      {64,  TQ_x_25},   /*CAN=10kbps*/       \
      {50,  TQ_x_16},   /*CAN=20kbps*/       \
      {20,  TQ_x_16},   /*CAN=50kbps*/       \
      {8,   TQ_x_16},   /*CAN=125kbps*/      \
      {4,   TQ_x_16},   /*CAN=250kbps*/      \
      {2,   TQ_x_16},   /*CAN=500kbps*/      \
      {2,   TQ_x_10},   /*CAN=800kbps*/      \
      {1,   TQ_x_16}    /*CAN=1000kbps*/
   #elif CO_FSYS == 36000
      #define CO_CANbitRateDataInitializers  \
      {50,  TQ_x_18},   /*CAN=10kbps*/       \
      {50,  TQ_x_18},   /*CAN=20kbps*/       \
      {20,  TQ_x_18},   /*CAN=50kbps*/       \
      {8,   TQ_x_18},   /*CAN=125kbps*/      \
      {4,   TQ_x_18},   /*CAN=250kbps*/      \
      {2,   TQ_x_18},   /*CAN=500kbps*/      \
      {2,   TQ_x_18},   /*Not possible*/     \
      {1,   TQ_x_18}    /*CAN=1000kbps*/
   #elif CO_FSYS == 40000
      #define CO_CANbitRateDataInitializers  \
      {50,  TQ_x_20},   /*Not possible*/     \
      {50,  TQ_x_20},   /*CAN=20kbps*/       \
      {25,  TQ_x_16},   /*CAN=50kbps*/       \
      {10,  TQ_x_16},   /*CAN=125kbps*/      \
      {5,   TQ_x_16},   /*CAN=250kbps*/      \
      {2,   TQ_x_20},   /*CAN=500kbps*/      \
      {1,   TQ_x_25},   /*CAN=800kbps*/      \
      {1,   TQ_x_20}    /*CAN=1000kbps*/
   #elif CO_FSYS == 48000
      #define CO_CANbitRateDataInitializers  \
      {63,  TQ_x_19},   /*Not possible*/     \
      {63,  TQ_x_19},   /*CAN=20kbps*/       \
      {30,  TQ_x_16},   /*CAN=50kbps*/       \
      {12,  TQ_x_16},   /*CAN=125kbps*/      \
      {6,   TQ_x_16},   /*CAN=250kbps*/      \
      {3,   TQ_x_16},   /*CAN=500kbps*/      \
      {2,   TQ_x_15},   /*CAN=800kbps*/      \
      {2,   TQ_x_12}    /*CAN=1000kbps*/
   #elif CO_FSYS == 56000
      #define CO_CANbitRateDataInitializers  \
      {61,  TQ_x_23},   /*Not possible*/     \
      {61,  TQ_x_23},   /*CAN=20kbps*/       \
      {35,  TQ_x_16},   /*CAN=50kbps*/       \
      {14,  TQ_x_16},   /*CAN=125kbps*/      \
      {7,   TQ_x_16},   /*CAN=250kbps*/      \
      {4,   TQ_x_14},   /*CAN=500kbps*/      \
      {5,   TQ_x_7 },   /*CAN=800kbps*/      \
      {2,   TQ_x_14}    /*CAN=1000kbps*/
   #elif CO_FSYS == 64000
      #define CO_CANbitRateDataInitializers  \
      {64,  TQ_x_25},   /*Not possible*/     \
      {64,  TQ_x_25},   /*CAN=20kbps*/       \
      {40,  TQ_x_16},   /*CAN=50kbps*/       \
      {16,  TQ_x_16},   /*CAN=125kbps*/      \
      {8,   TQ_x_16},   /*CAN=250kbps*/      \
      {4,   TQ_x_16},   /*CAN=500kbps*/      \
      {2,   TQ_x_20},   /*CAN=800kbps*/      \
      {2,   TQ_x_16}    /*CAN=1000kbps*/
   #elif CO_FSYS == 72000
      #define CO_CANbitRateDataInitializers  \
      {40,  TQ_x_18},   /*Not possible*/     \
      {40,  TQ_x_18},   /*Not possible*/     \
      {40,  TQ_x_18},   /*CAN=50kbps*/       \
      {16,  TQ_x_18},   /*CAN=125kbps*/      \
      {8,   TQ_x_18},   /*CAN=250kbps*/      \
      {4,   TQ_x_18},   /*CAN=500kbps*/      \
      {3,   TQ_x_15},   /*CAN=800kbps*/      \
      {2,   TQ_x_18}    /*CAN=1000kbps*/
   #elif CO_FSYS == 80000
      #define CO_CANbitRateDataInitializers  \
      {40,  TQ_x_20},   /*Not possible*/     \
      {40,  TQ_x_20},   /*Not possible*/     \
      {40,  TQ_x_20},   /*CAN=50kbps*/       \
      {16,  TQ_x_20},   /*CAN=125kbps*/      \
      {8,   TQ_x_20},   /*CAN=250kbps*/      \
      {4,   TQ_x_20},   /*CAN=500kbps*/      \
      {2,   TQ_x_25},   /*CAN=800kbps*/      \
      {2,   TQ_x_20}    /*CAN=1000kbps*/
   #else
      #error define_CO_FSYS CO_FSYS not supported
   #endif
#endif


/*******************************************************************************
   Object: CO_CANbitRateData_t

   Structure contains timing coefficients for CAN module. See <CAN bit rates>.

   CAN baud rate is calculated from following equations:
   Fsys                                - System clock (MAX 80MHz for PIC32MX)
   TQ = 2 * BRP / Fsys                 - Time Quanta
   BaudRate = 1 / (TQ * K)             - Can bus Baud Rate
   K = SJW + PROP + PhSeg1 + PhSeg2    - Number of Time Quantas

   Variables:
      BRP      - (1...64) Baud Rate Prescaler.
      SJW      - (1...4) SJW time.
      PROP     - (1...8) PROP time.
      PhSeg1   - (1...8) Phase Segment 1 time.
      PhSeg2   - (1...8) Phase Segment 2 time.
*******************************************************************************/
typedef struct{
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
   In PIC32MX this structure is used for both: transmitting and
   receiving to and from CAN module. (Object is ownded by CAN module).

   Variables:
      ident             - Standard Identifier.
      FILHIT            - Filter hit, see PIC32MX documentation.
      CMSGTS            - CAN message timestamp, see PIC32MX documentation.
      DLC               - Data length code (bits 0...3).
      RTR               - Remote Transmission Request bit.
      data              - 8 data bytes.
*******************************************************************************/
typedef struct{
   unsigned       ident    :11;
   unsigned       FILHIT   :5;
   unsigned       CMSGTS   :16;
   unsigned       DLC      :4;
   unsigned                :5;
   unsigned       RTR      :1;
   unsigned                :22;
   UNSIGNED8      data[8];
}CO_CANrxMsg_t;


/*******************************************************************************
   Object: CO_CANrxArray_t

   Array for handling received CAN messages.

   Variables:
      ident    - Standard Identifier (bits 0..10) + RTR (bit 11).
      mask     - Standard Identifier mask with same alignment as ident.
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
      CMSGSID           - Equal to register in transmit message buffer. Includes standard Identifier.
      CMSGEID           - Equal to register in transmit message buffer. Includes data length code and RTR.
      data              - 8 data bytes. Aligned to registers in transmit message buffer.
      bufferFull        - Previous message is still in buffer.
      syncFlag          - Synchronous PDO messages has this flag set. It prevents them to be sent outside the synchronous window.
*******************************************************************************/
typedef struct{
   UNSIGNED32           CMSGSID;
   UNSIGNED32           CMSGEID;
   UNSIGNED8            data[8];
   volatile UNSIGNED8   bufferFull;
   volatile UNSIGNED8   syncFlag;
}CO_CANtxArray_t;


/*******************************************************************************
   Object: CO_CANmodule_t

   Object controls CAN module.

   Variables:
      CANbaseAddress    - See parameters in <CO_CANmodule_init>.
      CANmsgBuff        - CAN message buffer. See <CO_CANrxMsg_t>.
      CANmsgBuffSize    - Size of the above buffer.
      rxArray           - Array for handling received CAN messages. See <CO_CANrxArray_t>.
      rxSize            - Size of above array is equal to number of receiving CAN objects.
      txArray           - Array for handling transmitting CAN messages. See <CO_CANtxArray_t>.
      txSize            - Size of above array is equal to number of transmitting CAN objects.
      curentSyncTimeIsInsideWindow - Pointer to variable with same name inside
                          <CO_SYNC_t> object. This pointer is configured inside
                          <CO_SYNC_init> function.
      useCANrxFilters   - Value different than zero indicates, that CAN module
                          hardware filters are used for CAN reception. If there
                          is not enough hardware filters, they won't be used. In
                          this case will be _all_ received CAN messages processed
                          by software. PIC32MX devices have 16 CAN rx filters.
      bufferInhibitFlag - If flag is true, then message in transmitt buffer is
                          synchronous PDO message, which will be aborted, if
                          <CO_clearPendingSyncPDOs> function will be called by
                          application. This may be necessary if Synchronous
                          window time was expired.
      firstCANtxMessage - Equal to 1, when the first transmitted message
                          (bootup message) is in CAN TX buffers.
      CANtxCount        - Number of messages in transmit buffer, which are
                          waiting to be copied to CAN module.
      errOld            - Previous state of CAN errors.
      EM                - Pointer to Emergency object <CO_emergencyReport_t>.
*******************************************************************************/
typedef struct{
   UNSIGNED16              CANbaseAddress;
   CO_CANrxMsg_t          *CANmsgBuff;
   UNSIGNED8               CANmsgBuffSize;
   CO_CANrxArray_t        *rxArray;
   UNSIGNED16              rxSize;
   CO_CANtxArray_t        *txArray;
   UNSIGNED16              txSize;
   volatile UNSIGNED8     *curentSyncTimeIsInsideWindow;
   volatile UNSIGNED8      useCANrxFilters;
   volatile UNSIGNED8      bufferInhibitFlag;
   volatile UNSIGNED8      firstCANtxMessage;
   volatile UNSIGNED16     CANtxCount;
   UNSIGNED32              errOld;
   void                   *EM;
}CO_CANmodule_t;


/*******************************************************************************
   Function: memcpySwapN

   Copy N data bytes from source to destination. Swap bytes if necessary.
   CANopen uses little endian. PIC32MX uses little endian. Functions just copy
   bytes without inversion.

   Parameters:
      dest        - Destination location.
      src         - Source location.
*******************************************************************************/
void memcpySwap2(UNSIGNED8* dest, UNSIGNED8* src);
void memcpySwap4(UNSIGNED8* dest, UNSIGNED8* src);
//#define BIG_ENDIAN


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
   PIC32MX CAN FIFO configuration: Two FIFOs are used. First FIFO is 32 messages
   long and is used for reception. Second is used for transmission and is 1
   message long. Format of message in fifo is described by <CO_CANrxMsg_t> for
   both: receiving and transmitting messages. However transmitting messages does
   not use all structure members.

   Parameters:
      ppCANmodule       - Pointer to address of CAN module object <CO_CANmodule_t>.
                          If address is zero, memory for new object will be
                          allocated and address will be set.
      CANbaseAddress    - CAN module base address. See <Peripheral addresses>.
      rxSize            - Size of receive array is equal to number of receiving CAN objects.
      txSize            - Size of transmit array is equal to number of transmitting CAN objects.
      CANbitRate        - CAN bit rate. Valid values are (in kbps): 10, 20, 50,
                          125, 250, 500, 800, 1000. If value is illegal, bitrate
                          defaults to 125.

   Return <CO_ReturnError>:
      CO_ERROR_NO                 - Operation completed successfully.
      CO_ERROR_ILLEGAL_ARGUMENT   - Error in function arguments.
      CO_ERROR_OUT_OF_MEMORY      - Memory allocation failed.
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
      CO_ERROR_OUT_OF_MEMORY      - Not enough masks for configuration.
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
   high priority. For more information see topics: <Reception of CAN messages>
   and <Transmission of CAN messages>.

   Parameters:
      CANmodule   - Pointer to CAN module object <CO_CANmodule_t>.
*******************************************************************************/
void CO_CANinterrupt(CO_CANmodule_t *CANmodule);


#endif
