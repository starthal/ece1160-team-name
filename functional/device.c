
#include "stm32f4xx.h"

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
  i2c_gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &i2c_gpio_init_struct);

  /* Connect I2C1 pins to AF */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);	// SCL
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); // SDA
  
  I2C_InitTypeDef i2c_init_struct;
  i2c_init_struct.I2C_Mode = I2C_Mode_I2C;
  i2c_init_struct.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c_init_struct.I2C_ClockSpeed = 100000;
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
}

void i2c_write(I2C_TypeDef* I2Cx, uint8_t data)
{
	I2C_SendData(I2Cx, data);
	// wait for I2C1 EV8_2 --> byte has been transmitted
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

void i2c_stop(I2C_TypeDef* I2Cx)
{
	// Send I2C1 STOP Condition 
	I2C_GenerateSTOP(I2Cx, ENABLE);
}
