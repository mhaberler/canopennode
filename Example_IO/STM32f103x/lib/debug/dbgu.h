#ifndef DBGU_H
#define DBGU_H
#include"stm32f10x_conf.h"

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------
 
//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

void DBGU_Configure(unsigned int baudrate);
extern unsigned char DBGU_GetChar(void);
extern void DBGU_PutChar(unsigned char c);
extern unsigned int DBGU_IsRxReady(void);

#endif //#ifndef DBGU_H

