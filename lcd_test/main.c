#define DISC

#include "stm32f4xx.h"
#include "lcd.h"
#include "main.h"


/* Status LED definitions */

#define STATUS_LED_PIN      GPIO_Pin_6 | GPIO_Pin_12
#define STATUS_LED_GPIO     GPIOD
#define STATUS_LED_GPIO_CLK RCC_AHB1Periph_GPIOD

void system_tick();
void delay(uint32_t nTime);
void update_status_led();
void DelayResolution100us(uint32_t Dely);

volatile uint32_t system_clock = 0;
volatile uint32_t time_left;
volatile uint16_t status_led_pattern = STATUS_LED_PATTERN_OK;
RCC_ClocksTypeDef RCC_Clocks;

#include "bitmap.c"

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

  
  /* Send init commands */

  status_led_pattern = STATUS_LED_PATTERN_ERR;
  status_led_pattern = STATUS_LED_PATTERN_OK;

  delay(50);
  lcd_init();
  delay(50);
  int i, j;
  
  lcd_fill(BLACK);
  lcd_fill(WHITE);
    
  for (i = 50; i < 100; i++)
  for (j = 50; j < 100; j++)
  {
    lcd_set_pixel(i, j, BLACK);
  }


  delay(100);

while(1)
{
  lcd_bitmap(pitt);
  delay(100);
  lcd_bitmap(arrow_up);
  delay(100);
}
  //status_led_pattern = STATUS_LED_PATTERN_ERR;
  
  while(1)
  {
    DelayResolution100us(10);
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

