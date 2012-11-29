#define DISC

#include "stm32f4xx.h"

#define DLY_100US  1000
/* Status LED definitions */
#ifdef DISC
  #define STATUS_LED_PIN      GPIO_Pin_6 | GPIO_Pin_12
  #define STATUS_LED_GPIO     GPIOD
  #define STATUS_LED_GPIO_CLK RCC_AHB1Periph_GPIOD
#else
  #define STATUS_LED_PIN      GPIO_Pin_7
  #define STATUS_LED_GPIO     GPIOF
  #define STATUS_LED_GPIO_CLK RCC_AHB1Periph_GPIOF
#endif
#define STATUS_LED_PATTERN_OK   0xAAAA    /*1010 1010 1010 1010*/
#define STATUS_LED_PATTERN_ERR  0xA800    /*1010 1000 0000 0000*/
#define STATUS_LED_UPDATE_MS    100

void system_tick();
void delay(uint32_t nTime);
//void update_status_led();
void DelayResolution100us(uint32_t Dely);

volatile uint32_t time_left;
uint16_t status_led_pattern = STATUS_LED_PATTERN_OK;
uint32_t system_clock = 0;
RCC_ClocksTypeDef RCC_Clocks;

int main(void)
{  
  time_left = 0;

  /* Initialization sequences --------------------------------------- */
  /* SysTick end of count event each 10ms */
  //RCC_GetClocksFreq(&RCC_Clocks);
  SystemInit();
  //SysTick_Config(RCC_Clocks.HCLK_Frequency/100);
    
  /* set up LED outputs */
  RCC_AHB1PeriphClockCmd(STATUS_LED_GPIO_CLK, ENABLE);
  GPIO_InitTypeDef status_led_gpio_init;
    status_led_gpio_init.GPIO_Pin = STATUS_LED_PIN;
    status_led_gpio_init.GPIO_Mode = GPIO_Mode_OUT;
    status_led_gpio_init.GPIO_OType = GPIO_OType_PP;
    status_led_gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
    status_led_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(STATUS_LED_GPIO, &status_led_gpio_init);
  
 while(1)
 {
        GPIO_SetBits(STATUS_LED_GPIO,STATUS_LED_PIN);
        DelayResolution100us(4000);
        GPIO_ResetBits(STATUS_LED_GPIO,STATUS_LED_PIN);
        DelayResolution100us(4000);
 }
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

void delay(volatile uint32_t nticks)
{
  time_left = nticks;

  while(time_left != 0);
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
    //update_status_led();
    time_left = STATUS_LED_UPDATE_MS/10;
  }
}
