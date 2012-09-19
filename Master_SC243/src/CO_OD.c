/*******************************************************************************

   File - CO_OD.c
   CANopen Object Dictionary.

   Copyright (C) 2004-2008 Janez Paternoster

   License: GNU Lesser General Public License (LGPL).

   <http://canopennode.sourceforge.net>

   (For more information see <CO_SDO.h>.)
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


   This file was automatically generated with CANopenNode Object
   Dictionary Editor. DON'T EDIT THIS FILE MANUALLY !!!!

*******************************************************************************/


#include "CO_driver.h"
#include "CO_OD.h"
#include "CO_SDO.h"


/*******************************************************************************
   DEFINITION AND INITIALIZATION OF OBJECT DICTIONARY VARIABLES
*******************************************************************************/

/***** Definition for RAM variables *******************************************/
struct sCO_OD_RAM CO_OD_RAM = {
           CO_OD_FIRST_LAST_WORD,

/*1001*/ 0x0,
/*1002*/ 0x0L,
/*1003*/ {0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1010*/ {0x3L},
/*1011*/ {0x1L},
/*1280*/{{0x3, 0x0L, 0x0L, 0x0}},
/*2100*/ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/*2103*/ 0x0,
/*2104*/ 0x0,
/*2107*/ {0x3E8, 0x0, 0x0, 0x0, 0x0},
/*2108*/ {0},
/*2109*/ {0},
/*2110*/ {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},
/*2120*/ {0x4, 0x1234567890ABCDEFLL, 0x234567890ABCDEF1LL, 12.345, 456.789},
/*2121*/ 0,
/*6000*/ {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
/*6200*/ {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
/*6401*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/*6411*/ {0, 0, 0, 0, 0, 0, 0, 0},

           CO_OD_FIRST_LAST_WORD,
};


/***** Definition for EEPROM variables ****************************************/
struct sCO_OD_EEPROM CO_OD_EEPROM = {
           CO_OD_FIRST_LAST_WORD,

/*2106*/ 0x0L,
/*2112*/ {1L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},

           CO_OD_FIRST_LAST_WORD,
};


/***** Definition for ROM variables *******************************************/
   CO_OD_ROM_IDENT struct sCO_OD_ROM CO_OD_ROM = {    //constant variables, stored in flash
           CO_OD_FIRST_LAST_WORD,

/*1000*/ 0x0L,
/*1005*/ 0x80L,
/*1006*/ 0x0L,
/*1007*/ 0x0L,
/*1008*/ {'C', 'A', 'N', 'o', 'p', 'e', 'n', 'N', 'o', 'd', 'e'},
/*1009*/ {'3', '.', '0', '0'},
/*100A*/ {'3', '.', '0', '0'},
/*1014*/ 0x80L,
/*1015*/ 0x64,
/*1016*/ {0x0L, 0x0L, 0x0L, 0x0L},
/*1017*/ 0x3E8,
/*1018*/ {0x4, 0x0L, 0x0L, 0x0L, 0x0L},
/*1019*/ 0x0,
/*1029*/ {0x0, 0x0, 0x1, 0x0, 0x0, 0x0},
/*1200*/{{0x2, 0x600L, 0x580L}},
/*1400*/{{0x2, 0x200L, 0xFF},
/*1401*/ {0x2, 0x300L, 0xFE},
/*1402*/ {0x2, 0x400L, 0xFE},
/*1403*/ {0x2, 0x500L, 0xFE},
/*1404*/ {0x2, 0x1L, 0xFE},
/*1405*/ {0x2, 0x1L, 0xFE},
/*1406*/ {0x2, 0x1L, 0xFE},
/*1407*/ {0x2, 0x1L, 0xFE},
/*1408*/ {0x2, 0x1L, 0xFE},
/*1409*/ {0x2, 0x1L, 0xFE},
/*140A*/ {0x2, 0x1L, 0xFE},
/*140B*/ {0x2, 0x1L, 0xFE},
/*140C*/ {0x2, 0x1L, 0xFE},
/*140D*/ {0x2, 0x1L, 0xFE},
/*140E*/ {0x2, 0x1L, 0xFE},
/*140F*/ {0x2, 0x1L, 0xFE}},
/*1600*/{{0x2, 0x62000108L, 0x62000208L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1601*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1602*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1603*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1604*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1605*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1606*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1607*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1608*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1609*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*160A*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*160B*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*160C*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*160D*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*160E*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*160F*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},
/*1800*/{{0x6, 0x180L, 0xFF, 0x64, 0x0, 0x0, 0x0},
/*1801*/ {0x6, 0x280L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*1802*/ {0x6, 0x380L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*1803*/ {0x6, 0x480L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*1804*/ {0x6, 0x1L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*1805*/ {0x6, 0x1L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*1806*/ {0x6, 0x1L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*1807*/ {0x6, 0x1L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*1808*/ {0x6, 0x1L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*1809*/ {0x6, 0x1L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*180A*/ {0x6, 0x1L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*180B*/ {0x6, 0x1L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*180C*/ {0x6, 0x1L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*180D*/ {0x6, 0x1L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*180E*/ {0x6, 0x1L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*180F*/ {0x6, 0x1L, 0xFE, 0x0, 0x0, 0x0, 0x0}},
/*1A00*/{{0x2, 0x60000108L, 0x60000208L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A01*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A02*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A03*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A04*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A05*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A06*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A07*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A08*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A09*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A0A*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A0B*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A0C*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A0D*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A0E*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A0F*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},
/*1F80*/ 0x0L,
/*2101*/ 0x20,
/*2102*/ 0xFA,
/*2111*/ {1L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},
/*2200*/ {0x3, 0x1000L, 0x80000L, 0xAL},

           CO_OD_FIRST_LAST_WORD
};


/*******************************************************************************
   STRUCTURES FOR RECORD TYPE OBJECTS
*******************************************************************************/
/*0x1018*/ const CO_ODrecord_t ODrecord1018[5] = {
           {(const void*)&CO_OD_ROM.identity.maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.identity.vendorID, 0x85,  4},
           {(const void*)&CO_OD_ROM.identity.productCode, 0x85,  4},
           {(const void*)&CO_OD_ROM.identity.revisionNumber, 0x85,  4},
           {(const void*)&CO_OD_ROM.identity.serialNumber, 0x85,  4}};
/*0x1200*/ const CO_ODrecord_t ODrecord1200[3] = {
           {(const void*)&CO_OD_ROM.SDOServerParameter[0].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.SDOServerParameter[0].COB_IDClientToServer, 0x85,  4},
           {(const void*)&CO_OD_ROM.SDOServerParameter[0].COB_IDServerToClient, 0x85,  4}};
/*0x1280*/ const CO_ODrecord_t ODrecord1280[4] = {
           {(const void*)&CO_OD_RAM.SDOClientParameter[0].maxSubIndex, 0x06,  1},
           {(const void*)&CO_OD_RAM.SDOClientParameter[0].COB_IDClientToServer, 0xBE,  4},
           {(const void*)&CO_OD_RAM.SDOClientParameter[0].COB_IDServerToClient, 0xBE,  4},
           {(const void*)&CO_OD_RAM.SDOClientParameter[0].nodeIDOfTheSDOServer, 0x0E,  1}};
/*0x1400*/ const CO_ODrecord_t ODrecord1400[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[0].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[0].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[0].transmissionType, 0x0D,  1}};
/*0x1401*/ const CO_ODrecord_t ODrecord1401[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[1].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[1].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[1].transmissionType, 0x0D,  1}};
/*0x1402*/ const CO_ODrecord_t ODrecord1402[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[2].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[2].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[2].transmissionType, 0x0D,  1}};
/*0x1403*/ const CO_ODrecord_t ODrecord1403[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[3].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[3].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[3].transmissionType, 0x0D,  1}};
/*0x1404*/ const CO_ODrecord_t ODrecord1404[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[4].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[4].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[4].transmissionType, 0x0D,  1}};
/*0x1405*/ const CO_ODrecord_t ODrecord1405[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[5].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[5].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[5].transmissionType, 0x0D,  1}};
/*0x1406*/ const CO_ODrecord_t ODrecord1406[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[6].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[6].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[6].transmissionType, 0x0D,  1}};
/*0x1407*/ const CO_ODrecord_t ODrecord1407[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[7].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[7].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[7].transmissionType, 0x0D,  1}};
/*0x1408*/ const CO_ODrecord_t ODrecord1408[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[8].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[8].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[8].transmissionType, 0x0D,  1}};
/*0x1409*/ const CO_ODrecord_t ODrecord1409[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[9].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[9].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[9].transmissionType, 0x0D,  1}};
/*0x140A*/ const CO_ODrecord_t ODrecord140A[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[10].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[10].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[10].transmissionType, 0x0D,  1}};
/*0x140B*/ const CO_ODrecord_t ODrecord140B[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[11].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[11].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[11].transmissionType, 0x0D,  1}};
/*0x140C*/ const CO_ODrecord_t ODrecord140C[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[12].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[12].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[12].transmissionType, 0x0D,  1}};
/*0x140D*/ const CO_ODrecord_t ODrecord140D[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[13].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[13].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[13].transmissionType, 0x0D,  1}};
/*0x140E*/ const CO_ODrecord_t ODrecord140E[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[14].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[14].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[14].transmissionType, 0x0D,  1}};
/*0x140F*/ const CO_ODrecord_t ODrecord140F[3] = {
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[15].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[15].COB_IDUsedByRPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOCommunicationParameter[15].transmissionType, 0x0D,  1}};
/*0x1600*/ const CO_ODrecord_t ODrecord1600[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject8, 0x8D,  4}};
/*0x1601*/ const CO_ODrecord_t ODrecord1601[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[1].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject8, 0x8D,  4}};
/*0x1602*/ const CO_ODrecord_t ODrecord1602[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[2].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject8, 0x8D,  4}};
/*0x1603*/ const CO_ODrecord_t ODrecord1603[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[3].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject8, 0x8D,  4}};
/*0x1604*/ const CO_ODrecord_t ODrecord1604[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[4].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[4].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[4].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[4].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[4].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[4].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[4].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[4].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[4].mappedObject8, 0x8D,  4}};
/*0x1605*/ const CO_ODrecord_t ODrecord1605[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[5].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[5].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[5].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[5].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[5].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[5].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[5].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[5].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[5].mappedObject8, 0x8D,  4}};
/*0x1606*/ const CO_ODrecord_t ODrecord1606[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[6].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[6].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[6].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[6].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[6].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[6].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[6].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[6].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[6].mappedObject8, 0x8D,  4}};
/*0x1607*/ const CO_ODrecord_t ODrecord1607[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[7].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[7].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[7].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[7].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[7].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[7].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[7].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[7].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[7].mappedObject8, 0x8D,  4}};
/*0x1608*/ const CO_ODrecord_t ODrecord1608[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[8].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[8].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[8].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[8].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[8].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[8].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[8].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[8].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[8].mappedObject8, 0x8D,  4}};
/*0x1609*/ const CO_ODrecord_t ODrecord1609[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[9].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[9].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[9].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[9].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[9].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[9].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[9].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[9].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[9].mappedObject8, 0x8D,  4}};
/*0x160A*/ const CO_ODrecord_t ODrecord160A[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[10].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[10].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[10].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[10].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[10].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[10].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[10].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[10].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[10].mappedObject8, 0x8D,  4}};
/*0x160B*/ const CO_ODrecord_t ODrecord160B[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[11].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[11].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[11].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[11].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[11].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[11].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[11].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[11].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[11].mappedObject8, 0x8D,  4}};
/*0x160C*/ const CO_ODrecord_t ODrecord160C[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[12].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[12].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[12].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[12].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[12].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[12].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[12].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[12].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[12].mappedObject8, 0x8D,  4}};
/*0x160D*/ const CO_ODrecord_t ODrecord160D[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[13].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[13].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[13].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[13].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[13].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[13].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[13].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[13].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[13].mappedObject8, 0x8D,  4}};
/*0x160E*/ const CO_ODrecord_t ODrecord160E[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[14].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[14].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[14].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[14].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[14].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[14].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[14].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[14].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[14].mappedObject8, 0x8D,  4}};
/*0x160F*/ const CO_ODrecord_t ODrecord160F[9] = {
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[15].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[15].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[15].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[15].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[15].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[15].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[15].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[15].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.RPDOMappingParameter[15].mappedObject8, 0x8D,  4}};
/*0x1800*/ const CO_ODrecord_t ODrecord1800[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[0].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[0].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[0].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[0].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[0].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[0].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[0].SYNCStartValue, 0x0D,  1}};
/*0x1801*/ const CO_ODrecord_t ODrecord1801[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[1].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[1].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[1].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[1].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[1].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[1].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[1].SYNCStartValue, 0x0D,  1}};
/*0x1802*/ const CO_ODrecord_t ODrecord1802[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[2].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[2].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[2].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[2].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[2].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[2].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[2].SYNCStartValue, 0x0D,  1}};
/*0x1803*/ const CO_ODrecord_t ODrecord1803[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[3].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[3].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[3].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[3].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[3].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[3].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[3].SYNCStartValue, 0x0D,  1}};
/*0x1804*/ const CO_ODrecord_t ODrecord1804[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[4].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[4].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[4].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[4].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[4].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[4].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[4].SYNCStartValue, 0x0D,  1}};
/*0x1805*/ const CO_ODrecord_t ODrecord1805[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[5].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[5].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[5].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[5].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[5].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[5].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[5].SYNCStartValue, 0x0D,  1}};
/*0x1806*/ const CO_ODrecord_t ODrecord1806[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[6].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[6].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[6].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[6].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[6].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[6].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[6].SYNCStartValue, 0x0D,  1}};
/*0x1807*/ const CO_ODrecord_t ODrecord1807[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[7].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[7].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[7].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[7].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[7].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[7].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[7].SYNCStartValue, 0x0D,  1}};
/*0x1808*/ const CO_ODrecord_t ODrecord1808[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[8].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[8].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[8].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[8].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[8].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[8].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[8].SYNCStartValue, 0x0D,  1}};
/*0x1809*/ const CO_ODrecord_t ODrecord1809[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[9].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[9].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[9].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[9].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[9].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[9].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[9].SYNCStartValue, 0x0D,  1}};
/*0x180A*/ const CO_ODrecord_t ODrecord180A[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[10].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[10].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[10].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[10].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[10].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[10].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[10].SYNCStartValue, 0x0D,  1}};
/*0x180B*/ const CO_ODrecord_t ODrecord180B[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[11].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[11].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[11].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[11].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[11].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[11].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[11].SYNCStartValue, 0x0D,  1}};
/*0x180C*/ const CO_ODrecord_t ODrecord180C[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[12].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[12].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[12].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[12].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[12].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[12].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[12].SYNCStartValue, 0x0D,  1}};
/*0x180D*/ const CO_ODrecord_t ODrecord180D[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[13].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[13].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[13].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[13].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[13].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[13].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[13].SYNCStartValue, 0x0D,  1}};
/*0x180E*/ const CO_ODrecord_t ODrecord180E[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[14].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[14].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[14].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[14].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[14].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[14].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[14].SYNCStartValue, 0x0D,  1}};
/*0x180F*/ const CO_ODrecord_t ODrecord180F[7] = {
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[15].maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[15].COB_IDUsedByTPDO, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[15].transmissionType, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[15].inhibitTime, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[15].compatibilityEntry, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[15].eventTimer, 0x8D,  2},
           {(const void*)&CO_OD_ROM.TPDOCommunicationParameter[15].SYNCStartValue, 0x0D,  1}};
/*0x1A00*/ const CO_ODrecord_t ODrecord1A00[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject8, 0x8D,  4}};
/*0x1A01*/ const CO_ODrecord_t ODrecord1A01[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[1].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject8, 0x8D,  4}};
/*0x1A02*/ const CO_ODrecord_t ODrecord1A02[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[2].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject8, 0x8D,  4}};
/*0x1A03*/ const CO_ODrecord_t ODrecord1A03[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[3].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[3].mappedObject8, 0x8D,  4}};
/*0x1A04*/ const CO_ODrecord_t ODrecord1A04[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[4].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[4].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[4].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[4].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[4].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[4].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[4].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[4].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[4].mappedObject8, 0x8D,  4}};
/*0x1A05*/ const CO_ODrecord_t ODrecord1A05[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[5].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[5].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[5].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[5].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[5].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[5].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[5].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[5].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[5].mappedObject8, 0x8D,  4}};
/*0x1A06*/ const CO_ODrecord_t ODrecord1A06[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[6].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[6].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[6].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[6].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[6].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[6].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[6].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[6].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[6].mappedObject8, 0x8D,  4}};
/*0x1A07*/ const CO_ODrecord_t ODrecord1A07[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[7].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[7].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[7].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[7].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[7].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[7].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[7].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[7].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[7].mappedObject8, 0x8D,  4}};
/*0x1A08*/ const CO_ODrecord_t ODrecord1A08[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[8].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[8].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[8].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[8].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[8].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[8].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[8].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[8].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[8].mappedObject8, 0x8D,  4}};
/*0x1A09*/ const CO_ODrecord_t ODrecord1A09[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[9].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[9].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[9].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[9].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[9].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[9].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[9].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[9].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[9].mappedObject8, 0x8D,  4}};
/*0x1A0A*/ const CO_ODrecord_t ODrecord1A0A[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[10].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[10].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[10].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[10].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[10].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[10].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[10].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[10].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[10].mappedObject8, 0x8D,  4}};
/*0x1A0B*/ const CO_ODrecord_t ODrecord1A0B[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[11].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[11].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[11].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[11].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[11].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[11].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[11].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[11].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[11].mappedObject8, 0x8D,  4}};
/*0x1A0C*/ const CO_ODrecord_t ODrecord1A0C[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[12].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[12].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[12].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[12].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[12].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[12].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[12].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[12].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[12].mappedObject8, 0x8D,  4}};
/*0x1A0D*/ const CO_ODrecord_t ODrecord1A0D[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[13].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[13].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[13].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[13].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[13].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[13].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[13].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[13].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[13].mappedObject8, 0x8D,  4}};
/*0x1A0E*/ const CO_ODrecord_t ODrecord1A0E[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[14].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[14].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[14].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[14].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[14].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[14].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[14].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[14].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[14].mappedObject8, 0x8D,  4}};
/*0x1A0F*/ const CO_ODrecord_t ODrecord1A0F[9] = {
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[15].numberOfMappedObjects, 0x0D,  1},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[15].mappedObject1, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[15].mappedObject2, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[15].mappedObject3, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[15].mappedObject4, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[15].mappedObject5, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[15].mappedObject6, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[15].mappedObject7, 0x8D,  4},
           {(const void*)&CO_OD_ROM.TPDOMappingParameter[15].mappedObject8, 0x8D,  4}};
/*0x2120*/ const CO_ODrecord_t ODrecord2120[5] = {
           {(const void*)&CO_OD_RAM.testVar.maxSubIndex, 0x06,  1},
           {(const void*)&CO_OD_RAM.testVar.I64, 0xBE,  8},
           {(const void*)&CO_OD_RAM.testVar.U64, 0xBE,  8},
           {(const void*)&CO_OD_RAM.testVar.R32, 0xBE,  4},
           {(const void*)&CO_OD_RAM.testVar.R64, 0xBE,  8}};
/*0x2200*/ const CO_ODrecord_t ODrecord2200[4] = {
           {(const void*)&CO_OD_ROM.CANopenLog.maxSubIndex, 0x05,  1},
           {(const void*)&CO_OD_ROM.CANopenLog.emcySRAMSize, 0x85,  4},
           {(const void*)&CO_OD_ROM.CANopenLog.CANLogSize, 0x8D,  4},
           {(const void*)&CO_OD_ROM.CANopenLog.maxDumpFiles, 0x8D,  4}};


/*******************************************************************************
   SDO SERVER ACCESS FUNCTIONS WITH USER CODE
*******************************************************************************/
#define WRITING (dir == 1)
#define READING (dir == 0)
UNSIGNED32 CO_ODF(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1003(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1005(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1006(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1010(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1011(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1014(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1016(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1019(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1200(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_RPDOcom(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_RPDOmap(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_TPDOcom(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_TPDOmap(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_2101(void *object, UNSIGNED16 index, UNSIGNED8 subIndex, UNSIGNED16* pLength,
                       UNSIGNED16 attribute, UNSIGNED8 dir, void* dataBuff, const void* pData){
  UNSIGNED32 abortCode;
  if(WRITING){
    UNSIGNED8 var = *((UNSIGNED8*)dataBuff);
    if(var < 1)   return 0x06090032L;  //Value of parameter written too low.
    if(var > 127) return 0x06090031L;  //Value of parameter written too high.
  }
  abortCode = CO_ODF(object, index, subIndex, pLength, attribute, dir, dataBuff, pData);
  return abortCode;
}

UNSIGNED32 CO_ODF_2102(void *object, UNSIGNED16 index, UNSIGNED8 subIndex, UNSIGNED16* pLength,
                       UNSIGNED16 attribute, UNSIGNED8 dir, void* dataBuff, const void* pData){
  UNSIGNED32 abortCode;
  if(WRITING){
    UNSIGNED16 var;
    memcpySwap2((UNSIGNED8*)&var, (UNSIGNED8*)dataBuff);
    if(!(var == 10 || var == 20 || var == 50 || var == 125 || var == 250 || var == 500 || var == 800 || var == 1000)) return 0x06090030L;  //Invalid value for the parameter
  }
  abortCode = CO_ODF(object, index, subIndex, pLength, attribute, dir, dataBuff, pData);
  return abortCode;
}

UNSIGNED32 CO_ODF_2107(void *object, UNSIGNED16 index, UNSIGNED8 subIndex, UNSIGNED16* pLength,
                       UNSIGNED16 attribute, UNSIGNED8 dir, void* dataBuff, const void* pData){
  UNSIGNED32 abortCode;
  if(WRITING){
    UNSIGNED16 var;
    memcpySwap2((UNSIGNED8*)&var, (UNSIGNED8*)dataBuff);
    if(!((subIndex == 3 || subIndex == 5) && var == 0))
      return 0x06090030; //Invalid value for parameter
  }
  abortCode = CO_ODF(object, index, subIndex, pLength, attribute, dir, dataBuff, pData);
  return abortCode;
}

UNSIGNED32 ODF_testDomain(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);


/*******************************************************************************
   OBJECT DICTIONARY
*******************************************************************************/
const sCO_OD_object CO_OD[CO_OD_NoOfElements] = {
{0x1000, 0x00, 0x85,  4, (const void*)&CO_OD_ROM.deviceType,                            CO_ODF},
{0x1001, 0x00, 0x36,  1, (const void*)&CO_OD_RAM.errorRegister,                         CO_ODF},
{0x1002, 0x00, 0xB6,  4, (const void*)&CO_OD_RAM.manufacturerStatusRegister,            CO_ODF},
{0x1003, 0x08, 0x8E,  4, (const void*)&CO_OD_RAM.preDefinedErrorField[0],               CO_ODF_1003},
{0x1005, 0x00, 0x8D,  4, (const void*)&CO_OD_ROM.COB_ID_SYNCMessage,                    CO_ODF_1005},
{0x1006, 0x00, 0x8D,  4, (const void*)&CO_OD_ROM.communicationCyclePeriod,              CO_ODF_1006},
{0x1007, 0x00, 0x8D,  4, (const void*)&CO_OD_ROM.synchronousWindowLength,               CO_ODF},
{0x1008, 0x00, 0x05, 11, (const void*)&CO_OD_ROM.manufacturerDeviceName[0],             CO_ODF},
{0x1009, 0x00, 0x05,  4, (const void*)&CO_OD_ROM.manufacturerHardwareVersion[0],        CO_ODF},
{0x100A, 0x00, 0x05,  4, (const void*)&CO_OD_ROM.manufacturerSoftwareVersion[0],        CO_ODF},
{0x1010, 0x01, 0x8E,  4, (const void*)&CO_OD_RAM.storeParameters[0],                    CO_ODF_1010},
{0x1011, 0x01, 0x8E,  4, (const void*)&CO_OD_RAM.restoreDefaultParameters[0],           CO_ODF_1011},
{0x1014, 0x00, 0x85,  4, (const void*)&CO_OD_ROM.COB_ID_EMCY,                           CO_ODF_1014},
{0x1015, 0x00, 0x8D,  2, (const void*)&CO_OD_ROM.inhibitTimeEMCY,                       CO_ODF},
{0x1016, 0x04, 0x8D,  4, (const void*)&CO_OD_ROM.consumerHeartbeatTime[0],              CO_ODF_1016},
{0x1017, 0x00, 0x8D,  2, (const void*)&CO_OD_ROM.producerHeartbeatTime,                 CO_ODF},
{0x1018, 0x04, 0x00,  0, (const void*)&ODrecord1018,                                    CO_ODF},
{0x1019, 0x00, 0x0D,  1, (const void*)&CO_OD_ROM.synchronousCounterOverflowValue,       CO_ODF_1019},
{0x1029, 0x06, 0x0D,  1, (const void*)&CO_OD_ROM.errorBehavior[0],                      CO_ODF},
{0x1200, 0x02, 0x00,  0, (const void*)&ODrecord1200,                                    CO_ODF_1200},
{0x1280, 0x03, 0x00,  0, (const void*)&ODrecord1280,                                    CO_ODF},
{0x1400, 0x02, 0x00,  0, (const void*)&ODrecord1400,                                    CO_ODF_RPDOcom},
{0x1401, 0x02, 0x00,  0, (const void*)&ODrecord1401,                                    CO_ODF_RPDOcom},
{0x1402, 0x02, 0x00,  0, (const void*)&ODrecord1402,                                    CO_ODF_RPDOcom},
{0x1403, 0x02, 0x00,  0, (const void*)&ODrecord1403,                                    CO_ODF_RPDOcom},
{0x1404, 0x02, 0x00,  0, (const void*)&ODrecord1404,                                    CO_ODF_RPDOcom},
{0x1405, 0x02, 0x00,  0, (const void*)&ODrecord1405,                                    CO_ODF_RPDOcom},
{0x1406, 0x02, 0x00,  0, (const void*)&ODrecord1406,                                    CO_ODF_RPDOcom},
{0x1407, 0x02, 0x00,  0, (const void*)&ODrecord1407,                                    CO_ODF_RPDOcom},
{0x1408, 0x02, 0x00,  0, (const void*)&ODrecord1408,                                    CO_ODF_RPDOcom},
{0x1409, 0x02, 0x00,  0, (const void*)&ODrecord1409,                                    CO_ODF_RPDOcom},
{0x140A, 0x02, 0x00,  0, (const void*)&ODrecord140A,                                    CO_ODF_RPDOcom},
{0x140B, 0x02, 0x00,  0, (const void*)&ODrecord140B,                                    CO_ODF_RPDOcom},
{0x140C, 0x02, 0x00,  0, (const void*)&ODrecord140C,                                    CO_ODF_RPDOcom},
{0x140D, 0x02, 0x00,  0, (const void*)&ODrecord140D,                                    CO_ODF_RPDOcom},
{0x140E, 0x02, 0x00,  0, (const void*)&ODrecord140E,                                    CO_ODF_RPDOcom},
{0x140F, 0x02, 0x00,  0, (const void*)&ODrecord140F,                                    CO_ODF_RPDOcom},
{0x1600, 0x08, 0x00,  0, (const void*)&ODrecord1600,                                    CO_ODF_RPDOmap},
{0x1601, 0x08, 0x00,  0, (const void*)&ODrecord1601,                                    CO_ODF_RPDOmap},
{0x1602, 0x08, 0x00,  0, (const void*)&ODrecord1602,                                    CO_ODF_RPDOmap},
{0x1603, 0x08, 0x00,  0, (const void*)&ODrecord1603,                                    CO_ODF_RPDOmap},
{0x1604, 0x08, 0x00,  0, (const void*)&ODrecord1604,                                    CO_ODF_RPDOmap},
{0x1605, 0x08, 0x00,  0, (const void*)&ODrecord1605,                                    CO_ODF_RPDOmap},
{0x1606, 0x08, 0x00,  0, (const void*)&ODrecord1606,                                    CO_ODF_RPDOmap},
{0x1607, 0x08, 0x00,  0, (const void*)&ODrecord1607,                                    CO_ODF_RPDOmap},
{0x1608, 0x08, 0x00,  0, (const void*)&ODrecord1608,                                    CO_ODF_RPDOmap},
{0x1609, 0x08, 0x00,  0, (const void*)&ODrecord1609,                                    CO_ODF_RPDOmap},
{0x160A, 0x08, 0x00,  0, (const void*)&ODrecord160A,                                    CO_ODF_RPDOmap},
{0x160B, 0x08, 0x00,  0, (const void*)&ODrecord160B,                                    CO_ODF_RPDOmap},
{0x160C, 0x08, 0x00,  0, (const void*)&ODrecord160C,                                    CO_ODF_RPDOmap},
{0x160D, 0x08, 0x00,  0, (const void*)&ODrecord160D,                                    CO_ODF_RPDOmap},
{0x160E, 0x08, 0x00,  0, (const void*)&ODrecord160E,                                    CO_ODF_RPDOmap},
{0x160F, 0x08, 0x00,  0, (const void*)&ODrecord160F,                                    CO_ODF_RPDOmap},
{0x1800, 0x06, 0x00,  0, (const void*)&ODrecord1800,                                    CO_ODF_TPDOcom},
{0x1801, 0x06, 0x00,  0, (const void*)&ODrecord1801,                                    CO_ODF_TPDOcom},
{0x1802, 0x06, 0x00,  0, (const void*)&ODrecord1802,                                    CO_ODF_TPDOcom},
{0x1803, 0x06, 0x00,  0, (const void*)&ODrecord1803,                                    CO_ODF_TPDOcom},
{0x1804, 0x06, 0x00,  0, (const void*)&ODrecord1804,                                    CO_ODF_TPDOcom},
{0x1805, 0x06, 0x00,  0, (const void*)&ODrecord1805,                                    CO_ODF_TPDOcom},
{0x1806, 0x06, 0x00,  0, (const void*)&ODrecord1806,                                    CO_ODF_TPDOcom},
{0x1807, 0x06, 0x00,  0, (const void*)&ODrecord1807,                                    CO_ODF_TPDOcom},
{0x1808, 0x06, 0x00,  0, (const void*)&ODrecord1808,                                    CO_ODF_TPDOcom},
{0x1809, 0x06, 0x00,  0, (const void*)&ODrecord1809,                                    CO_ODF_TPDOcom},
{0x180A, 0x06, 0x00,  0, (const void*)&ODrecord180A,                                    CO_ODF_TPDOcom},
{0x180B, 0x06, 0x00,  0, (const void*)&ODrecord180B,                                    CO_ODF_TPDOcom},
{0x180C, 0x06, 0x00,  0, (const void*)&ODrecord180C,                                    CO_ODF_TPDOcom},
{0x180D, 0x06, 0x00,  0, (const void*)&ODrecord180D,                                    CO_ODF_TPDOcom},
{0x180E, 0x06, 0x00,  0, (const void*)&ODrecord180E,                                    CO_ODF_TPDOcom},
{0x180F, 0x06, 0x00,  0, (const void*)&ODrecord180F,                                    CO_ODF_TPDOcom},
{0x1A00, 0x08, 0x00,  0, (const void*)&ODrecord1A00,                                    CO_ODF_TPDOmap},
{0x1A01, 0x08, 0x00,  0, (const void*)&ODrecord1A01,                                    CO_ODF_TPDOmap},
{0x1A02, 0x08, 0x00,  0, (const void*)&ODrecord1A02,                                    CO_ODF_TPDOmap},
{0x1A03, 0x08, 0x00,  0, (const void*)&ODrecord1A03,                                    CO_ODF_TPDOmap},
{0x1A04, 0x08, 0x00,  0, (const void*)&ODrecord1A04,                                    CO_ODF_TPDOmap},
{0x1A05, 0x08, 0x00,  0, (const void*)&ODrecord1A05,                                    CO_ODF_TPDOmap},
{0x1A06, 0x08, 0x00,  0, (const void*)&ODrecord1A06,                                    CO_ODF_TPDOmap},
{0x1A07, 0x08, 0x00,  0, (const void*)&ODrecord1A07,                                    CO_ODF_TPDOmap},
{0x1A08, 0x08, 0x00,  0, (const void*)&ODrecord1A08,                                    CO_ODF_TPDOmap},
{0x1A09, 0x08, 0x00,  0, (const void*)&ODrecord1A09,                                    CO_ODF_TPDOmap},
{0x1A0A, 0x08, 0x00,  0, (const void*)&ODrecord1A0A,                                    CO_ODF_TPDOmap},
{0x1A0B, 0x08, 0x00,  0, (const void*)&ODrecord1A0B,                                    CO_ODF_TPDOmap},
{0x1A0C, 0x08, 0x00,  0, (const void*)&ODrecord1A0C,                                    CO_ODF_TPDOmap},
{0x1A0D, 0x08, 0x00,  0, (const void*)&ODrecord1A0D,                                    CO_ODF_TPDOmap},
{0x1A0E, 0x08, 0x00,  0, (const void*)&ODrecord1A0E,                                    CO_ODF_TPDOmap},
{0x1A0F, 0x08, 0x00,  0, (const void*)&ODrecord1A0F,                                    CO_ODF_TPDOmap},
{0x1F80, 0x00, 0x8D,  4, (const void*)&CO_OD_ROM.NMTStartup,                            CO_ODF},
{0x2100, 0x00, 0x36, 10, (const void*)&CO_OD_RAM.errorStatusBits[0],                    CO_ODF},
{0x2101, 0x00, 0x0D,  1, (const void*)&CO_OD_ROM.CANNodeID,                             CO_ODF_2101},
{0x2102, 0x00, 0x8D,  2, (const void*)&CO_OD_ROM.CANBitRate,                            CO_ODF_2102},
{0x2103, 0x00, 0x8E,  2, (const void*)&CO_OD_RAM.SYNCCounter,                           CO_ODF},
{0x2104, 0x00, 0x86,  2, (const void*)&CO_OD_RAM.SYNCTime,                              CO_ODF},
{0x2106, 0x00, 0x87,  4, (const void*)&CO_OD_EEPROM.powerOnCounter,                     CO_ODF},
{0x2107, 0x05, 0xBE,  2, (const void*)&CO_OD_RAM.performance[0],                        CO_ODF_2107},
{0x2108, 0x01, 0xB6,  2, (const void*)&CO_OD_RAM.temperature[0],                        CO_ODF},
{0x2109, 0x01, 0xB6,  2, (const void*)&CO_OD_RAM.voltage[0],                            CO_ODF},
{0x2110, 0x10, 0xFE,  4, (const void*)&CO_OD_RAM.variableInt32[0],                      CO_ODF},
{0x2111, 0x10, 0xFD,  4, (const void*)&CO_OD_ROM.variableROMInt32[0],                   CO_ODF},
{0x2112, 0x10, 0xFF,  4, (const void*)&CO_OD_EEPROM.variableNVInt32[0],                 CO_ODF},
{0x2120, 0x04, 0x00,  0, (const void*)&ODrecord2120,                                    CO_ODF},
{0x2121, 0x00, 0x3E,  0, 0,                                                             ODF_testDomain},
{0x2200, 0x03, 0x00,  0, (const void*)&ODrecord2200,                                    CO_ODF},
{0x6000, 0x08, 0x76,  1, (const void*)&CO_OD_RAM.readInput8Bit[0],                      CO_ODF},
{0x6200, 0x08, 0x3E,  1, (const void*)&CO_OD_RAM.writeOutput8Bit[0],                    CO_ODF},
{0x6401, 0x0C, 0xB6,  2, (const void*)&CO_OD_RAM.readAnalogueInput16Bit[0],             CO_ODF},
{0x6411, 0x08, 0xBE,  2, (const void*)&CO_OD_RAM.writeAnalogueOutput16Bit[0],           CO_ODF},
};
