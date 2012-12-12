#ifndef __DEVICE_H
#define __DEVICE_H

#include "stm32f4xx.h"
#include "action.h"

#define DRIVER_ADDR_ALLCALL (0xE0)
#define DRIVER_ADDR_SWRST (0x00)

#define DRIVER_MODE1 0x00
#define DRIVER_MODE2 0x01
//#define DRIVER_ALLCALLADDR 0x05
#define DRIVER_LED0_ON_L 0x06
#define DRIVER_LED0_ON_H 0x07
#define DRIVER_LED0_OFF_L 0x08
#define DRIVER_LED0_OFF_H 0x09

#define DRIVER_ALL_LED_ON_L 0xFA
#define DRIVER_ALL_LED_ON_H 0xFB
#define DRIVER_ALL_LED_OFF_L 0xFC
#define DRIVER_ALL_LED_OFF_H 0xFD

/* 000, INVRT=0, OCH=0, OUTDRV=0, OUTNE[1:0]=00 */
#define DRIVER_MODE2_CFG_NTYPE 0x00
/* 000, INVRT=1, OCH=0, OUTDRV=0, OUTNE[1:0]=00 */
#define DRIVER_MODE2_CFG_LED 0x10


//extern menuitem_t menu[MAX_MENUITEMS];
//extern uint8_t num_menuitems = 0;

void i2c1_init();
void i2c_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void i2c_write(I2C_TypeDef* I2Cx, uint8_t data);
void i2c_stop(I2C_TypeDef* I2Cx);

void device_load_profile();
void device_init();
void device_set_pin(uint8_t pin, uint16_t value);
void eeprom_bin_write(uint8_t *data);

uint8_t i2c_read_ack(I2C_TypeDef* I2Cx);
uint8_t i2c_read_nack(I2C_TypeDef* I2Cx);

extern menuitem_t menu[MAX_MENUITEMS];
extern uint8_t num_menuitems;
extern uint8_t profile[65536];
extern uint16_t pin_current_vals[16];
extern uint16_t pin_old_vals[16];

#endif
