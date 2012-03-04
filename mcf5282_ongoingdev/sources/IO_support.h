/*!
 * \file 		IO_support.h
 * \brief 		Implements Input/output support. 
 *				Provide port initialization fuctions and access functions
 *
 * \date 		01/01/2012
 *
 */


#ifndef __IO_SUPPORT_H__
#define __IO_SUPPORT_H__

#include "mcf5282.h" 

 
void IO_portTC_out_init(void);
void IO_portTD_out_init(void);
void IO_portQB_out_init(void);
void IO_portQA_out_init(void);
void IO_portQS_out_init(void);

#endif