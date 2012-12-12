/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * usart.c
 * Stephen Albert
 * Deere Smart Unloading: RF Boards
 * Created 16 May 2012
 * 
 * This file contains all of the high-level USART code.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TODO:
 * - Right now the USART[1236] have to correspond with specific hardware 
 *   and this code doesn't make that clear.
 */

#include "w_utils.h"
#include "stm32f4xx.h"
#include "main.h"
#include "usart.h"



uint8_t flag_usart_sending;
volatile uint8_t recv_in_progress;


void usart_2_init(uint32_t baud_rate, uint16_t flow_control)
{
  /* Enable peripheral clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Alternate function mapping */
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource2, GPIO_AF_USART2); /* TX */
  
  GPIO_InitTypeDef comm_gpio_init;
    comm_gpio_init.GPIO_Pin = GPIO_Pin_2;
    comm_gpio_init.GPIO_Mode = GPIO_Mode_AF;
    comm_gpio_init.GPIO_OType = GPIO_OType_PP;
    comm_gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
    comm_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &comm_gpio_init);

  USART_InitTypeDef comm_usart_init;
    comm_usart_init.USART_BaudRate = baud_rate;
    comm_usart_init.USART_WordLength = USART_WordLength_8b;
    comm_usart_init.USART_StopBits = USART_StopBits_1;
    comm_usart_init.USART_Parity = USART_Parity_No;
    comm_usart_init.USART_Mode = USART_Mode_Tx;
    comm_usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //flow_control;
  USART_Init(USART2, &comm_usart_init);

#if 0
  /* NVIC configuration */
  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
 
  /* Enable the USARTx Interrupt */
  NVIC_InitTypeDef comm_nvic_init;
    comm_nvic_init.NVIC_IRQChannel = USART1_IRQn;
    comm_nvic_init.NVIC_IRQChannelPreemptionPriority = USART_PREEMPT_PRIORITY;
    comm_nvic_init.NVIC_IRQChannelSubPriority = USART_SUB_PRIORITY;
    comm_nvic_init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&comm_nvic_init);
#endif

  USART_Cmd(USART2, ENABLE);

  /* Enable interrupt */
  //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  //flag_usart1_up = 1;
}


void usart_send_byte(USART_TypeDef* usart_x, uint8_t byte)
{
    while (USART_GetFlagStatus(usart_x, USART_FLAG_TXE) == RESET);
    USART_SendData(usart_x, byte);
}

void usart_send(USART_TypeDef* usart_x, uint8_t* data, uint16_t size)
{
  uint16_t i;
  flag_usart_sending = 1;
  for (i = 0; i < size; i++)
  {
    while (USART_GetFlagStatus(usart_x, USART_FLAG_TXE) == RESET);
    USART_SendData(usart_x, data[i]);
  }
}

void usart_send_str(USART_TypeDef* usart_x, char* str)
{
  uint8_t len = w_strlen(str);
  usart_send(usart_x, (uint8_t*) str, len);
}

uint8_t usart_recv(USART_TypeDef* usart_x, uint8_t* recv_buf, uint16_t size, uint16_t timeout)
{
  USART_ITConfig(usart_x, USART_IT_RXNE, DISABLE);
  //flag_usart_recving = 1;
  int i;
  for (i = 0; i < size; i++)
  {
      uint32_t start_time = system_clock;
      while (USART_GetFlagStatus(usart_x, USART_FLAG_RXNE) == RESET)
      {
        if (timeout != 0 && system_clock - start_time > timeout)
        {
          USART_ITConfig(usart_x, USART_IT_RXNE, ENABLE);
          return 0;
        }
      }  
      recv_buf[i] = 0xFF & USART_ReceiveData(usart_x);
      /* FIXME display as we recv */
      //USART_SendData(DEBUG_USART, recv_buf[i]);
      /* end */
  }

  USART_ITConfig(usart_x, USART_IT_RXNE, ENABLE);
  return 1;
}
