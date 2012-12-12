#ifndef __MAIN_H
#define __MAIN_H

#include "usb_core.h"

void system_tick();
void delay(uint32_t nticks);
void delay_res_100us(uint32_t dely);
extern volatile uint16_t status_led_pattern;
extern volatile uint32_t system_clock;
extern USB_OTG_CORE_HANDLE  usb_device;

/* Status LED definitions */
#define STATUS_LED_PIN      GPIO_Pin_12
#define STATUS_LED_GPIO     GPIOD
#define STATUS_LED_GPIO_CLK RCC_AHB1Periph_GPIOD

#define STATUS_LED_PATTERN_OK   0xAAAA    /*1010 1010 1010 1010*/
#define STATUS_LED_PATTERN_ERR  0xA800    /*1010 1000 0000 0000*/
#define STATUS_LED_UPDATE_MS    100

#define DLY_100US  10

void debug(char* str);
void debug_uint(uint32_t num);

extern uint8_t profile_bin[];

#endif
