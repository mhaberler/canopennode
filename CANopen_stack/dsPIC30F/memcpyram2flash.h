/*******************************************************************************

   File: memcpyram2flash.h
   Internal flash write for Microchip dsPIC30F and Microchip C30 compiler (>= V3.00).

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


/*******************************************************************************
   Function: memcpyram2flash

   Copies contents of RAM variable into Flash program memory space.

   Its use can be dangerous, since it may "reprogram" the microcontroller if
   wrong used. It is tested to work with dsPIC30Fxxx. Function takes about 10 ms,
   because writing to flash is slow. If reset occurs between erasing and writing
   new values, up to 64 bytes of memory locations may stay erased.

   Parameters:
      dest        - Destination address in flash memory.
      src         - Source address in RAM memory.
      memsize     - Number of bytes to copy.
*******************************************************************************/
void memcpyram2flash(const void* dest, void* src, unsigned int memsize);
