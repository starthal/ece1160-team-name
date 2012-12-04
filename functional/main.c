#define DISC

#include "stm32f4xx.h"
#include "lcd.h"
#include "main.h"
#include "action.h"
#include "device.h"
#include "usart.h"


/* Status LED definitions */

#define STATUS_LED_PIN      GPIO_Pin_6 | GPIO_Pin_12
#define STATUS_LED_GPIO     GPIOD
#define STATUS_LED_GPIO_CLK RCC_AHB1Periph_GPIOD

#define BUTTON_PIN       GPIO_Pin_0
#define BUTTON_GPIO        GPIOA
#define BUTTON_GPIO_CLK   RCC_AHB1Periph_GPIOA

void system_tick();
void delay(uint32_t nTime);
void update_status_led();
void DelayResolution100us(uint32_t Dely);

volatile uint32_t system_clock = 0;
volatile uint32_t time_left;
volatile uint16_t status_led_pattern = STATUS_LED_PATTERN_OK;
RCC_ClocksTypeDef RCC_Clocks;

int main(void)
{  
  time_left = 0;

  /* Initialization sequences --------------------------------------- */
  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SystemInit();
  SysTick_Config(RCC_Clocks.HCLK_Frequency/100);
    
  /* set up LED outputs */
  RCC_AHB1PeriphClockCmd(STATUS_LED_GPIO_CLK, ENABLE);
  GPIO_InitTypeDef status_led_gpio_init;
  status_led_gpio_init.GPIO_Pin = STATUS_LED_PIN;
  status_led_gpio_init.GPIO_Mode = GPIO_Mode_OUT;
  status_led_gpio_init.GPIO_OType = GPIO_OType_PP;
  status_led_gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
  status_led_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(STATUS_LED_GPIO, &status_led_gpio_init);

  /* Set up button */
  RCC_AHB1PeriphClockCmd(BUTTON_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  GPIO_InitTypeDef button_gpio_init;
  button_gpio_init.GPIO_Pin = BUTTON_PIN;
  button_gpio_init.GPIO_Mode = GPIO_Mode_IN;
  button_gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
  button_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(BUTTON_GPIO, &button_gpio_init);
  

  status_led_pattern = STATUS_LED_PATTERN_OK;

#if 0
  delay(10);
  lcd_init();
  delay(10);
  lcd_fill(BLACK);
#endif
  delay(10);

  uint8_t bval = 0, bval_old = 0;

  //action_next();

  //usart_1_init(9600, USART_HardwareFlowControl_None);
  delay(10);
  
  //while (1)
  //{
  //  usart_send_str(USART1, "Hello World\n");
  //}

  /* Get device profile from ROM */
  //load_device_profile();
  uint8_t addr = DRIVER_ADDR_ALLCALL; //led driver All Call
  delay(50);

  i2c1_init();

#if 1
  delay(10);
  i2c_start(I2C1, addr, I2C_Direction_Transmitter);
  i2c_write(I2C1, DRIVER_MODE1);
  i2c_write(I2C1, 0x01);
  #if 1
  i2c_stop(I2C1);
  delay(10);
  i2c_start(I2C1, addr, I2C_Direction_Transmitter);
  i2c_write(I2C1, DRIVER_MODE2);
  i2c_write(I2C1, 0x00);
  #endif
  i2c_stop(I2C1);
  delay(10);
#endif
  
  
  while (1)
  {
    i2c_start(I2C1, addr, I2C_Direction_Transmitter);
    i2c_write(I2C1, DRIVER_LED0_ON_H);
    i2c_write(I2C1, 0x0F);
    i2c_stop(I2C1);
    DelayResolution100us(100);
    
    i2c_start(I2C1, addr, I2C_Direction_Transmitter);
    i2c_write(I2C1, DRIVER_LED0_OFF_H);
    i2c_write(I2C1, 0x00);
    i2c_stop(I2C1);
    delay(50);
  }

  while(1)
  {
    /* Wait for button press */
    uint32_t time1 = system_clock;
    while (system_clock - time1 < 100)
    { 
      //bval = GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN);
      if (1) //bval == 0 && bval_old == 1)
      {
        delay(10);
      }
      bval_old = bval;
    }
    //lcd_fill(BLACK);
    //action_next();
  }


  while (1) delay(10);
  return 0;
}

void DelayResolution100us(uint32_t Dely)
{
  uint32_t ct = 0;
  for(ct = Dely; ct!=0; ct--)
  {
    volatile uint32_t j;
    for(j = DLY_100US; j; j--)
    {
    }
  }
}

void delay(uint32_t nticks)
{
  uint32_t time1 = system_clock;
  while(system_clock - time1 < nticks);
}


void system_tick()
{
  if (time_left != 0)
  { 
    system_clock++;
    time_left--;
  }
  if (time_left == 0)
  {
    //GPIO_ToggleBits(STATUS_LED_GPIO, STATUS_LED_PIN);
    update_status_led();
    time_left = STATUS_LED_UPDATE_MS/10;
  }
}

void update_status_led()
{
  /* This should happen on a predefined time delay */

  static uint8_t ind;
  if ((status_led_pattern << ind) & 0x8000) /* Look at MSB of the code as it shifts */
    GPIO_SetBits(STATUS_LED_GPIO,STATUS_LED_PIN);
  else
    GPIO_ResetBits(STATUS_LED_GPIO,STATUS_LED_PIN);
  ind ++;
  if (ind == 16) ind = 0;
}

