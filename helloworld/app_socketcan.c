/*
 * Header for application interface for CANopenNode stack.
 *
 * @file        application.c
 * @ingroup     application
 * @version     SVN: \$Id$
 * @author      Janez Paternoster
 * @copyright   2012 - 2013 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <http://canopennode.sourceforge.net>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * CANopenNode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "CANopen.h"
#include "app_socketcan.h"



int programStart(const char *ifname)
{
    int retval;
    struct sockaddr_can addr;
    struct ifreq ifr;
    int cansocket;

    LOG("if=%s", ifname);

    if((cansocket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
	perror("Error while opening socket");
	return cansocket;
    }
    strcpy(ifr.ifr_name, ifname);

    if ((retval = ioctl(cansocket, SIOCGIFINDEX, &ifr)) < 0) {
	perror("SIOCGIFINDEX");
	return retval;
    }

    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    LOG("%s at index %d", ifname, ifr.ifr_ifindex);

    if((retval = bind(cansocket, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
	perror("Error in socket bind");
	return retval;
    }
    LOG("cansocket=%d", cansocket);
    return cansocket;
}

void communicationReset(void){
    LOG("");
}



void programEnd(int cansocket)
{
    if (cansocket > 0) {
	LOG("closing socket %d", cansocket);
	close(cansocket);
    }
}


void programAsync(uint16_t timer1msDiff){
    //    LOG("");

}


void program1ms(void){
    //    LOG("");

}
