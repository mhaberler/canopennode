/* Configuracni HW soubor pro ZL27ARM */
#ifndef __HWCONF__
#define __HWCONF__

#pragma pack(4) 


/* CAN interface */
#define CLOCK_CAN				    RCC_APB1Periph_CAN1
#define CLOCK_GPIO_CAN     			RCC_APB2Periph_GPIOB
#define GPIO_Remapping_CAN          GPIO_Remap1_CAN1
#define GPIO_CAN                    GPIOB
#define GPIO_Pin_CAN_RX             GPIO_Pin_8
#define GPIO_Pin_CAN_TX             GPIO_Pin_9
#define GPIO_CAN_Remap_State        ENABLE  

/* LED interface */
#define RCC_APB2Periph_GPIO_LED     RCC_APB2Periph_GPIOB
#define GPIO_LEDS                   GPIOB
#define GPIO_Pin_Led_GREEN          GPIO_Pin_15
#define GPIO_Pin_Led_RED            GPIO_Pin_14
#define LED_POSITIVE                

/* Debug Interface */
#define USART_DBG                   USART1
#define USART_DBG_CLK               RCC_APB2Periph_USART1
#define GPIO_Remapping_DBG          GPIO_Remap_USART1    

#define USART_DBG_GPIO              GPIOB
#define USART_DBG_GPIO_CLK          RCC_APB2Periph_GPIOB
#define USART_DBG_RxPin             GPIO_Pin_7
#define USART_DBG_TxPin             GPIO_Pin_6
#define USART_DBG_Remap_State       ENABLE

//#define USART_DBG_GPIO              GPIOA
//#define USART_DBG_GPIO_CLK          RCC_APB2Periph_GPIOA
//#define USART_DBG_RxPin             GPIO_Pin_10
//#define USART_DBG_TxPin             GPIO_Pin_9
//#define USART_DBG_Remap_State       DISABLE

//#define USARTy_DBG_IRQn             USART1_IRQn
//#define USARTy_DBG_IRQHandler       USART1_IRQHandler

#endif
