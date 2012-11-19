/*******************************************************************************

   File: crc16-ccitt.h
   Calculation of CRC 16 CCITT polynomial, x^16 + x^12 + x^5 + 1

   Copyright (C) 2012 Janez Paternoster

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


   Author: Lammert Bies, Janez Paternoster

*******************************************************************************/

#ifndef _CRC16_CCITT_H
#define _CRC16_CCITT_H


/*******************************************************************************
   Function: crc16_ccitt

   Calculate CRC sum on block of data.

   Parameters:
      block       - Pointer to block of data.
      blockLength - Length of data in bytes;
      crc         - Initial value (zero for xmodem). If block is split into
                    multiple segments, previous CRC is used as initial.

   Return:
      Calculated CRC.
*******************************************************************************/
unsigned short crc16_ccitt( unsigned char  *block,
                            unsigned int    blockLength,
                            unsigned short  crc);

#endif
