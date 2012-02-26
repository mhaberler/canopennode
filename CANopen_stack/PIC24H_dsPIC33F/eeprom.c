/*******************************************************************************

   File - eeprom.c
   Internal eeprom object for Microchip dsPIC33F or PIC24H and Microchip C30 compiler (>= V3.00).

   Copyright (C) 2004-2008 Janez Paternoster

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


#include "CO_driver.h"
#include "eeprom.h"

/******************************************************************************/
UNSIGNED32 CO_ODF_1010( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED8   length,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData)
{
   return 0x06020000;   //Object does not exist in the object dictionary.
}


/******************************************************************************/
UNSIGNED32 CO_ODF_1011( void       *object,
                        UNSIGNED16  index,
                        UNSIGNED8   subIndex,
                        UNSIGNED8   length,
                        UNSIGNED16  attribute,
                        UNSIGNED8   dir,
                        void       *dataBuff,
                        const void *pData)
{
   return 0x06020000;   //Object does not exist in the object dictionary.
}



