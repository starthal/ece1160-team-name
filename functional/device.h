#ifndef __DEVICE_H
#define __DEVICE_H

#include "stm32f4xx.h"
#define DRIVER_ADDR_ALLCALL (0xE0)

#define DRIVER_MODE1 0x00
#define DRIVER_MODE2 0x01
#define DRIVER_ALLCALLADDR 0x05
#define DRIVER_LED0_ON_H 0x07
#define DRIVER_LED0_ON_L 0x08
#define DRIVER_LED0_OFF_H 0x09
#define DRIVER_LED0_OFF_L 0x0A

#define DRIVER_ALL_LED_ON_L 0xFA
#define DRIVER_ALL_LED_ON_H 0xFB
#define DRIVER_ALL_LED_OFF_L 0xFC
#define DRIVER_ALL_LED_OFF_H 0xFD


void i2c1_init();
void i2c_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void i2c_write(I2C_TypeDef* I2Cx, uint8_t data);
void i2c_stop(I2C_TypeDef* I2Cx);


#endif
