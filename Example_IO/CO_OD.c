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
/*2100*/ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/*2103*/ 0x0,
/*2104*/ 0x0,
/*2107*/ {0x3E8, 0x0, 0x0, 0x0, 0x0},
/*2108*/ {0},
/*2109*/ {0},
/*2110*/ {0L, 0L},
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
/*1403*/ {0x2, 0x500L, 0xFE}},
/*1600*/{{0x2, 0x62000108L, 0x62000208L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1601*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1602*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1603*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},
/*1800*/{{0x6, 0x180L, 0xFF, 0x64, 0x0, 0x0, 0x0},
/*1801*/ {0x6, 0x280L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*1802*/ {0x6, 0x380L, 0xFE, 0x0, 0x0, 0x0, 0x0},
/*1803*/ {0x6, 0x480L, 0xFE, 0x0, 0x0, 0x0, 0x0}},
/*1A00*/{{0x2, 0x60000108L, 0x60000208L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A01*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A02*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A03*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},
/*1F80*/ 0x0L,
/*2101*/ 0x30,
/*2102*/ 0xFA,
/*2111*/ {1L, 0L},

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


/*******************************************************************************
   SDO SERVER ACCESS FUNCTIONS WITH USER CODE
*******************************************************************************/
#define WRITING (dir == 1)
#define READING (dir == 0)
UNSIGNED32 CO_ODF(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1003(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1005(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1006(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1010(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1011(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1014(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1016(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1019(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_1200(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_RPDOcom(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_RPDOmap(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_TPDOcom(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_TPDOmap(void*, UNSIGNED16, UNSIGNED8, UNSIGNED8, UNSIGNED16, UNSIGNED8, void*, const void*);

UNSIGNED32 CO_ODF_2101(void *object, UNSIGNED16 index, UNSIGNED8 subIndex, UNSIGNED8 length,
                       UNSIGNED16 attribute, UNSIGNED8 dir, void* dataBuff, const void* pData){
  UNSIGNED32 abortCode;
  if(WRITING){
    UNSIGNED8 var = *((UNSIGNED8*)dataBuff);
    if(var < 1)   return 0x06090032L;  //Value of parameter written too low.
    if(var > 127) return 0x06090031L;  //Value of parameter written too high.
  }
  abortCode = CO_ODF(object, index, subIndex, length, attribute, dir, dataBuff, pData);
  return abortCode;
}

UNSIGNED32 CO_ODF_2102(void *object, UNSIGNED16 index, UNSIGNED8 subIndex, UNSIGNED8 length,
                       UNSIGNED16 attribute, UNSIGNED8 dir, void* dataBuff, const void* pData){
  UNSIGNED32 abortCode;
  if(WRITING){
    UNSIGNED16 var;
    memcpySwap2((UNSIGNED8*)&var, (UNSIGNED8*)dataBuff);
    if(!(var == 10 || var == 20 || var == 50 || var == 125 || var == 250 || var == 500 || var == 800 || var == 1000)) return 0x06090030L;  //Invalid value for the parameter
  }
  abortCode = CO_ODF(object, index, subIndex, length, attribute, dir, dataBuff, pData);
  return abortCode;
}

UNSIGNED32 CO_ODF_2107(void *object, UNSIGNED16 index, UNSIGNED8 subIndex, UNSIGNED8 length,
                       UNSIGNED16 attribute, UNSIGNED8 dir, void* dataBuff, const void* pData){
  UNSIGNED32 abortCode;
  if(WRITING){
    UNSIGNED16 var;
    memcpySwap2((UNSIGNED8*)&var, (UNSIGNED8*)dataBuff);
    if(!((subIndex == 3 || subIndex == 5) && var == 0))
      return 0x06090030; //Invalid value for parameter
  }
  abortCode = CO_ODF(object, index, subIndex, length, attribute, dir, dataBuff, pData);
  return abortCode;
}


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
{0x1400, 0x02, 0x00,  0, (const void*)&ODrecord1400,                                    CO_ODF_RPDOcom},
{0x1401, 0x02, 0x00,  0, (const void*)&ODrecord1401,                                    CO_ODF_RPDOcom},
{0x1402, 0x02, 0x00,  0, (const void*)&ODrecord1402,                                    CO_ODF_RPDOcom},
{0x1403, 0x02, 0x00,  0, (const void*)&ODrecord1403,                                    CO_ODF_RPDOcom},
{0x1600, 0x08, 0x00,  0, (const void*)&ODrecord1600,                                    CO_ODF_RPDOmap},
{0x1601, 0x08, 0x00,  0, (const void*)&ODrecord1601,                                    CO_ODF_RPDOmap},
{0x1602, 0x08, 0x00,  0, (const void*)&ODrecord1602,                                    CO_ODF_RPDOmap},
{0x1603, 0x08, 0x00,  0, (const void*)&ODrecord1603,                                    CO_ODF_RPDOmap},
{0x1800, 0x06, 0x00,  0, (const void*)&ODrecord1800,                                    CO_ODF_TPDOcom},
{0x1801, 0x06, 0x00,  0, (const void*)&ODrecord1801,                                    CO_ODF_TPDOcom},
{0x1802, 0x06, 0x00,  0, (const void*)&ODrecord1802,                                    CO_ODF_TPDOcom},
{0x1803, 0x06, 0x00,  0, (const void*)&ODrecord1803,                                    CO_ODF_TPDOcom},
{0x1A00, 0x08, 0x00,  0, (const void*)&ODrecord1A00,                                    CO_ODF_TPDOmap},
{0x1A01, 0x08, 0x00,  0, (const void*)&ODrecord1A01,                                    CO_ODF_TPDOmap},
{0x1A02, 0x08, 0x00,  0, (const void*)&ODrecord1A02,                                    CO_ODF_TPDOmap},
{0x1A03, 0x08, 0x00,  0, (const void*)&ODrecord1A03,                                    CO_ODF_TPDOmap},
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
{0x2110, 0x02, 0xFE,  4, (const void*)&CO_OD_RAM.variableInt32[0],                      CO_ODF},
{0x2111, 0x02, 0xFD,  4, (const void*)&CO_OD_ROM.variableROMInt32[0],                   CO_ODF},
{0x6000, 0x08, 0x76,  1, (const void*)&CO_OD_RAM.readInput8Bit[0],                      CO_ODF},
{0x6200, 0x08, 0x3E,  1, (const void*)&CO_OD_RAM.writeOutput8Bit[0],                    CO_ODF},
{0x6401, 0x0C, 0xB6,  2, (const void*)&CO_OD_RAM.readAnalogueInput16Bit[0],             CO_ODF},
{0x6411, 0x08, 0xBE,  2, (const void*)&CO_OD_RAM.writeAnalogueOutput16Bit[0],           CO_ODF},
};
