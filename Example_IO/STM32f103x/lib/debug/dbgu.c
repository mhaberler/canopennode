//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "stm32f10x_conf.h"
#include <stdarg.h>

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Initializes the DBGU with the given parameters, and enables both the
/// transmitter and the receiver. The mode parameter contains the value of the
/// DBGU_MR register.
/// Value DBGU_STANDARD can be used for mode to get the most common configuration
/// (i.e. aysnchronous, 8bits, no parity, 1 stop bit, no flow control).
/// \param baudrate  Desired baudrate (e.g. 115200).
//------------------------------------------------------------------------------
                                                      
void DBGU_Configure(unsigned int baudrate) {
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  /* Enable GPIO and USART_DBG clock */
  RCC_APB2PeriphClockCmd(USART_DBG_GPIO_CLK | USART_DBG_CLK | RCC_APB2Periph_AFIO, ENABLE);

  /* Remap USART */
  GPIO_PinRemapConfig(GPIO_Remapping_DBG, USART_DBG_Remap_State);
  
  /* Configure USART_DBG Tx  */
  GPIO_InitStructure.GPIO_Pin = USART_DBG_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(USART_DBG_GPIO, &GPIO_InitStructure);
  
  /* Configure USART_DBG Rx */
  GPIO_InitStructure.GPIO_Pin = USART_DBG_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(USART_DBG_GPIO, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART_DBG, &USART_InitStructure);     
  USART_Cmd(USART_DBG, ENABLE);
}

//------------------------------------------------------------------------------
/// Outputs a character on the DBGU line.
/// \note This function is synchronous (i.e. uses polling).
/// \param c  Character to send.
//------------------------------------------------------------------------------
void DBGU_PutChar(unsigned char c) {
    while( !USART_GetFlagStatus(USART_DBG, USART_FLAG_TXE) ) ; //TODO!
    USART_SendData(USART_DBG, c);
}

//------------------------------------------------------------------------------
/// Return 1 if a character can be read in DBGU
//------------------------------------------------------------------------------
unsigned int DBGU_IsRxReady() {
    //TODO
    return 0;
}

//------------------------------------------------------------------------------
/// Reads and returns a character from the DBGU.
/// \note This function is synchronous (i.e. uses polling).
/// \return Character received.
//------------------------------------------------------------------------------
unsigned char DBGU_GetChar(void) {
    //TODO
    return 0;
}


//------------------------------------------------------------------------------
/// \exclude
/// Implementation of fputc using the DBGU as the standard output. Required
/// for printf().
/// \param c  Character to write.
/// \param pStream  Output stream.
/// \param The character written if successful, or -1 if the output stream is
/// not stdout or stderr.
//------------------------------------------------------------------------------
signed int fputc(signed int c, FILE *pStream)
{
    if ((pStream == stdout) || (pStream == stderr)) {
    
        DBGU_PutChar(c);
        return c;
    }
    else {

        return EOF;
    }
}

//------------------------------------------------------------------------------
/// \exclude
/// Implementation of fputs using the DBGU as the standard output. Required
/// for printf(). Does NOT currently use the PDC.
/// \param pStr  String to write.
/// \param pStream  Output stream.
/// \return Number of characters written if successful, or -1 if the output
/// stream is not stdout or stderr.
//------------------------------------------------------------------------------
signed int fputs(const char *pStr, FILE *pStream)
{
    signed int num = 0;

    while (*pStr != 0) {

        if (fputc(*pStr, pStream) == -1) {

            return -1;
        }
        num++;
        pStr++;
    }

    return num;
}

//------------------------------------------------------------------------------
/// \exclude
/// Outputs a character on the DBGU.
/// \param c  Character to output.
/// \return The character that was output.
//------------------------------------------------------------------------------
#undef putchar
signed int putchar(signed int c)
{
    return fputc(c, stdout);
}



