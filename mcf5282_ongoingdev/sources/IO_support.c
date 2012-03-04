/*!
 * \file 		IO_support.c
 * \brief   	Implements Input/output support. 
 *				Provide port initialization fuctions and access functions
 *
 * \author GROSBOIS
 *
 * \date 01/01/2012
 *
 */

 
 #include "IO_support.h"
 
 
/**************************************************************/ 
/*!
 * \fn  void IO_portTC_out_init()
 *  		Initialize hardware TC GPIO port use as output
 *
 /*************************************************************/
void IO_portTC_out_init(void)
{
MCF_PAD_PTCPAR = 0x00; //port configured as GPIO
MCF_GPIO_DDRTC = 0xFF; //output
MCF_GPIO_PORTTC = 0x00; //clear at init
}


/**************************************************************/ 
/*!
 * \fn  void IO_portTD_out_init()
 *  		Initialize hardware TD GPIO port use as output
 *
 /*************************************************************/
void IO_portTD_out_init(void)
{
MCF_PAD_PTDPAR = 0x00; //port configured as GPIO
MCF_GPIO_DDRTD = 0xFF; //output
MCF_GPIO_PORTTD = 0x00; //clear at init
}

/**************************************************************/ 
/*!
 * \fn  void IO_portQA_out_init()
 *  		Initialize hardware port QA GPIO port use as output
 *			NB : this port can also be used for ADC and is
 *				 known as AN[5
 *
 /*************************************************************/
void IO_portQA_out_init(void)
{
/* config as GPIO */
MCF_QADC_DDRQA = 0xFF;	//output
MCF_QADC_PORTQA = 0xFF; //init
}

/**************************************************************/ 
/*!
 * \fn  void IO_portQB_out_init()
 *  		Initialize hardware port QB GPIO port use as output
 *			NB : this port can also be used for ADC
 *
 /*************************************************************/
void IO_portQB_out_init(void)
{
/* config as GPIO */
MCF_QADC_DDRQB = 0xFF; //output
MCF_QADC_PORTQB = 0xFF; //init
}

/**************************************************************/ 
/*!
 * \fn  void IO_portQS_out_init(void)
 *  		Initialize hardware port QS GPIO port use as output
 *			NB : this port can also be used for ADC
 *
 /*************************************************************/
void IO_portQS_out_init(void)
{

MCF_PAD_PQSPAR = 0x00; //port configured as GPIO
MCF_GPIO_DDRQS = 0x00; //input
}

