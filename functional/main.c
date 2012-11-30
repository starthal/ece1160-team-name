#define DISC

#include "stm32f4xx.h"
#include "lcd.h"
#include "main.h"


/* Status LED definitions */

#define STATUS_LED_PIN      GPIO_Pin_6 | GPIO_Pin_12
#define STATUS_LED_GPIO     GPIOD
#define STATUS_LED_GPIO_CLK RCC_AHB1Periph_GPIOD

#define BUTTON_GPIO       GPIO_Pin_0
#define BUTTON_PIN        GPIOA
#define BUTTON_GPIO_CLK   RCC_AHB1Periph_GPIOA

void system_tick();
void delay(uint32_t nTime);
void update_status_led();
void DelayResolution100us(uint32_t Dely);

volatile uint32_t system_clock = 0;
volatile uint32_t time_left;
volatile uint16_t status_led_pattern = STATUS_LED_PATTERN_OK;
RCC_ClocksTypeDef RCC_Clocks;

#include "bitmap.c"


typedef struct function_s
{
  uint8_t* image;
  uint8_t is_action;
  struct menuitem_s* menuitem;
} function_t;

typedef struct menuitem_s
{
  function_s function;
  struct menuitem_s *next;
} menuitem_t;




void function_next()
{
  /* Find the next function in the tree */
}

int main(void)
{  
  time_left = 0;

  /* Initialization sequences --------------------------------------- */
  {
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
  GPIO_InitTypeDef button_gpio_init;
  button_gpio_init.GPIO_Pin = BUTTON_PIN;
  button_gpio_init.GPIO_Mode = GPIO_Mode_OUT;
  button_gpio_init.GPIO_OType = GPIO_OType_PP;
  button_gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
  button_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(BUTTON_GPIO, &button_gpio_init);
}
  
  /* Send init commands */

  status_led_pattern = STATUS_LED_PATTERN_OK;

  delay(10);
  lcd_init();
  delay(10);
  lcd_fill(BLACK);
  delay(10);

  uint8_t bval, bval_old;

  menuitem_t *main_menu;
  main_menu->function = function_pitt;
  main_menu->next = 

  while(1)
  {
    /* Wait for button press */
    uint8_t bval = GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN);

    if (bval != bval_old)
    {
      function_next();
    }
    
    lcd_bitmap(pitt);
    delay(100);
    lcd_bitmap(arrow_up);
    delay(100);
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

