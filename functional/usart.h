#ifndef __USART_H
#define __USART_H

#include "stm32f4xx_usart.h"

    #define DEBUG_USART             USART2
    #define DEBUG_USART_INIT_FUNC   usart_2_init
    #define DEBUG_BAUD_RATE         ((uint32_t) 115200)
    #define DEBUG_FLOW_CONTROL      USART_HardwareFlowControl_None


#define USART_PREEMPT_PRIORITY 1
#define USART_SUB_PRIORITY 0

extern volatile uint8_t recv_in_progress;

/* Function prototypes */
void usart_2_init(uint32_t baud_rate, uint16_t flow_control);
void usart_send_byte(USART_TypeDef* usart_x, uint8_t byte);
void usart_send(USART_TypeDef*, uint8_t*, uint16_t);
void usart_send_str(USART_TypeDef*, char*);
uint8_t usart_recv(USART_TypeDef*, uint8_t*, uint16_t, uint16_t);


#define usart_printf(pUSART, format, ...) { \
        char string[128]; \
        sprintf (string, format, __VA_ARGS__); \
        usart_send_str (pUSART, string); }
#endif
