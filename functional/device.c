#include "stm32f4xx.h"
#include "main.h"
#include "device.h"
#include "main.h"
#include "usart.h"
#include "w_utils.h"

#define EEPROM_ADDR_A0_P0 0xA0 /* 1010000x, A=0, P=0 */
#define EEPROM_ADDR_A1_P0 0xA4 /* 1010100x, A=0, P=0 */

/* Variables */
menuitem_t menu[MAX_MENUITEMS];
uint8_t num_menuitems = 0;
uint8_t profile[65536];
uint16_t pin_current_vals[16] = {0};
uint16_t pin_old_vals[16] = {0};

/* Write 256 bytes at a time */
void eeprom_write_page(uint16_t addr, uint8_t *data, uint16_t size)
{
  uint8_t addr_h = (uint8_t) 0xFF & (addr >> 8);
  uint8_t addr_l = (uint8_t) 0xFF & addr;

  debug_hex(addr_h);
  debug(" ");
  debug_hex(addr_l);
  debug("\r\n");
  
  /* page write */
  debug("Start\r\n");
  i2c_start(I2C1, EEPROM_ADDR_A0_P0, I2C_Direction_Transmitter);
  
  debug("A");
  /* address */
  i2c_write(I2C1, addr_h);
  i2c_write(I2C1, addr_l);

  debug("B");
  uint16_t i;
  if (size > 256) size = 256;
  for (i = 0; i < size; i++)
  {
    usart_send_str(USART2, ".");
    i2c_write(I2C1, data[i]);
  }

  i2c_stop(I2C1);
}

/* Write 64 KBytes of memory (all of page 0) */
void eeprom_bin_write(uint8_t *data)
{
  uint16_t addr = 0;
  GPIO_SetBits(STATUS_LED_GPIO, GPIO_Pin_13);
  /* 256 * 256 = 65536 = 1 page */
  uint16_t i;
  for (i = 0; i < 256; i++)
  {
    usart_send_str(USART2, "Writing page\r\n");
    eeprom_write_page(addr, data + (256*i), 256);
  }
  GPIO_ResetBits(STATUS_LED_GPIO, GPIO_Pin_13);
}

/* Read 64 KBytes of memory */
void eeprom_bin_read(uint8_t *data)
{
  uint32_t i;

  uint16_t addr = 0;
  GPIO_SetBits(STATUS_LED_GPIO, GPIO_Pin_13);
  uint8_t addr_h = 0xFF & (addr >> 8);
  uint8_t addr_l = 0xFF & addr;

  i2c_start(I2C1, EEPROM_ADDR_A0_P0, I2C_Direction_Transmitter);
  
  /* address */
  i2c_write(I2C1, addr_h);
  i2c_write(I2C1, addr_l);

  /* Sequential read */
  i2c_start(I2C1, EEPROM_ADDR_A0_P0, I2C_Direction_Receiver);

  for (i = 0; i < 65536-1; i++)
  {
    data[i] = i2c_read_ack(I2C1);
  }

  data[i] = i2c_read_nack(I2C1);

  GPIO_ResetBits(STATUS_LED_GPIO, GPIO_Pin_13);
}
  

void eeprom_read_page(uint16_t addr, uint8_t *data, uint8_t size)
{

}

void device_load_profile()
{
  uint32_t i, j;
  
  /* Get the database */
  //eeprom_bin_read(profile);
  
  void *db = profile;
  void *curptr = db;

  w_memcpy(&num_menuitems, curptr, sizeof(uint32_t));
  curptr += sizeof(uint32_t);

  debug_uint(num_menuitems);
  debug(" menu items\r\n");

  /* Read menuitems */
  for (i = 0; i < num_menuitems; i++)
  {
    /* Read a menuitem */
    db_menuitem_t *db_menuitem = (db_menuitem_t *) curptr;
    curptr += sizeof(db_menuitem_t);
    /* Debug */
    debug("Menuitem: icon = ");
    debug_uint(db_menuitem->icon_size);
    debug(" num_actions = ");
    debug_uint(db_menuitem->num_actions);
    debug("\r\n");
           
    menu[i].num_actions = db_menuitem->num_actions;
    menu[i].icon_size = db_menuitem->icon_size;
  }
  debug("\r\n");

  for (i = 0; i < num_menuitems; i++)
  {
    //printf("For menuitem %u\n", i);
    
    for (j = 0; j < menu[i].num_actions; j++)
    {
      /* Read an action */
      db_action_t *action = (db_action_t *) curptr;
      curptr += sizeof(db_action_t);

      menu[i].actions[j] = *action;

      debug("Action: ");
      debug(" typ=");
      debug_uint(action->type);
      debug(" pin=");
      debug_uint(action->pin);
      debug(" valu=");
      debug_uint(action->value);
      debug(" time=");
      debug_uint(action->time);
      debug("\r\n");
    }
  }

  /* Read icons */
  for (i = 0; i < num_menuitems; i++)
  {
    menu[i].icon = (uint8_t *) curptr;
    debug("Icon at ");
    debug_hex(menu[i].icon);
    debug("\r\n");
    curptr += menu[i].icon_size;
  }
}

void i2c1_init()
{
  /* i2c init */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  /* Set up GPIOB alternate function */
  GPIO_InitTypeDef i2c_gpio_init_struct;
  i2c_gpio_init_struct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  i2c_gpio_init_struct.GPIO_Mode = GPIO_Mode_AF;
  i2c_gpio_init_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  i2c_gpio_init_struct.GPIO_OType = GPIO_OType_OD;
  i2c_gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &i2c_gpio_init_struct);

  /* Connect I2C1 pins to AF */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);	// SCL
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); // SDA
  
  I2C_InitTypeDef i2c_init_struct;
  i2c_init_struct.I2C_Mode = I2C_Mode_I2C;
  i2c_init_struct.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c_init_struct.I2C_ClockSpeed = 50000;
  i2c_init_struct.I2C_OwnAddress1 = 0x42;
  i2c_init_struct.I2C_Ack = I2C_Ack_Enable;
  i2c_init_struct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C1, &i2c_init_struct);
  
  I2C_Cmd(I2C1, ENABLE);
}

void i2c_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction)
{
	// wait until I2C1 is not busy anymore
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	// Send I2C1 START condition 
	I2C_GenerateSTART(I2Cx, ENABLE);
	  
	// wait for I2C1 EV5 --> Slave has acknowledged start condition
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
		
	// Send slave Address for write 
	I2C_Send7bitAddress(I2Cx, address, direction);
	  
	/* wait for I2C1 EV6, check if 
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */ 
	if(direction == I2C_Direction_Transmitter){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if(direction == I2C_Direction_Receiver){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
  debug("a");
}

void i2c_write(I2C_TypeDef* I2Cx, uint8_t data)
{
	I2C_SendData(I2Cx, data);
	// wait for I2C1 EV8_2 --> byte has been transmitted
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

uint8_t i2c_read_ack(I2C_TypeDef* I2Cx)
{
	// enable acknowledge of recieved data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}

uint8_t i2c_read_nack(I2C_TypeDef* I2Cx)
{
	// disabe acknowledge of received data
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}

void i2c_stop(I2C_TypeDef* I2Cx)
{
	// Send I2C1 STOP Condition 
	I2C_GenerateSTOP(I2Cx, ENABLE);
}

void device_init()
{
  i2c1_init();

  //i2c_start(I2C1, DRIVER_ADDR_SWRST, I2C_Direction_Transmitter);
  //i2c_write(I2C1, 0x06);
  //i2c_stop(I2C1);  
  delay(10);
  i2c_start(I2C1, DRIVER_ADDR_ALLCALL, I2C_Direction_Transmitter);
  i2c_write(I2C1, DRIVER_MODE1);
  
  /* RESTART=0, EXTCLK=0, AI=1, SLEEP=0, SUB1=0, SUB2=0, SUB3=0, ALLCALL=1 */
  i2c_write(I2C1, 0x21);
  i2c_stop(I2C1);  
  delay(2);
  i2c_start(I2C1, DRIVER_ADDR_ALLCALL, I2C_Direction_Transmitter);
  i2c_write(I2C1, DRIVER_MODE2);
  
  /* 000, INVRT, OCH, OUTDRV, OUTNE[1:0] */
  i2c_write(I2C1, DRIVER_MODE2_CFG_LED);
  i2c_stop(I2C1);
  delay(2);
}

void device_set_pin(uint8_t pin, uint16_t value)
{
  pin_old_vals[pin] = pin_current_vals[pin];
  pin_current_vals[pin] = value;
  
  if (pin > 15)
  {
    status_led_pattern = STATUS_LED_PATTERN_ERR;
    while (1) delay (1);
  }
  
  uint8_t pin_on_l_reg = DRIVER_LED0_ON_L + 4 * pin;
  
  //uint8_t pin_on_h_reg = DRIVER_LED0_ON_H + 4 * pin;
  //uint8_t pin_off_l_reg = DRIVER_LED0_OFF_L + 4 * pin;
  //uint8_t pin_off_h_reg = DRIVER_LED0_OFF_H + 4 * pin;

  uint8_t value_on_l;
  uint8_t value_on_h;
  uint8_t value_off_l;
  uint8_t value_off_h;

  if (value == 0) /* full OFF */
  {
    value_on_l = 0;
    value_on_h = 0;
    value_off_l = 0;
    value_off_h = 0x10;
  }
  else if (value >= 4095) /* full ON */
  {
    value_on_l = 0;
    value_on_h = 0x10;
    value_off_l = 0;
    value_off_h = 0;
  }
  else
  {
    value_on_l = 0;
    value_on_h = 0;
    value_off_l = (value & 0xFF);
    value_off_h = 0x0F & (value >> 8);
  }
  /* Set the values */
  i2c_start(I2C1, DRIVER_ADDR_ALLCALL, I2C_Direction_Transmitter);
  i2c_write(I2C1, pin_on_l_reg);
  i2c_write(I2C1, value_on_l);
  i2c_write(I2C1, value_on_h);
  i2c_write(I2C1, value_off_l);
  i2c_write(I2C1, value_off_h);
  i2c_stop(I2C1);
  delay_res_100us(20);  

}
