#ifndef __MAIN_H
#define __MAIN_H

void system_tick();
void delay(uint32_t nticks);
extern volatile uint16_t status_led_pattern;
extern volatile uint32_t system_clock;

#define STATUS_LED_PATTERN_OK   0xAAAA    /*1010 1010 1010 1010*/
#define STATUS_LED_PATTERN_ERR  0xA800    /*1010 1000 0000 0000*/
#define STATUS_LED_UPDATE_MS    100

#define DLY_100US  10
#endif
