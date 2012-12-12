#define DISC

#include "stm32f4xx.h"
#include "lcd.h"
#include "main.h"
#include "action.h"
#include "device.h"
#include "usart.h"
#include "w_utils.h"
//#include "usb_core.h"
//#include "usbd_cdc_core.h"
//#include "usbd_usr.h"
//#include "usbd_desc.h"
//#include "vcp.h"

//__ALIGN_BEGIN USB_OTG_CORE_HANDLE  usb_device __ALIGN_END;

#define BUTTON_PIN       GPIO_Pin_1
#define BUTTON_GPIO        GPIOA
#define BUTTON_GPIO_CLK   RCC_AHB1Periph_GPIOA

void system_tick();
void delay(uint32_t nTime);
void update_status_led();
void delay_res_100us(uint32_t Dely);

/* Not sure why aligned(4) is necessary here, but it's used in example code */
USB_OTG_CORE_HANDLE  usb_device __attribute__ ((aligned (4)));

volatile uint32_t system_clock = 0;
volatile uint32_t time_left;
volatile uint16_t status_led_pattern = STATUS_LED_PATTERN_OK;
RCC_ClocksTypeDef RCC_Clocks;

//#define PROGRAM_MODE
  #define TEST_MODE

#ifdef PROGRAM_MODE
#include "bin.c"
#endif

#ifdef TEST_MODE
#include "bin.c"
#endif

void debug(char* str)
{
  usart_send_str(USART2, str);
}

void debug_uint(uint32_t num)
{
  char mesg[11];
  uint32tos(mesg, num, 0);
  debug(mesg);
}

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
  status_led_gpio_init.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
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
  
  usart_2_init(115200, USART_HardwareFlowControl_None);
  delay(10);

  status_led_pattern = STATUS_LED_PATTERN_OK;

  delay(10);
  lcd_init();
  delay(10);
  lcd_fill(BLACK);
  delay(10);



  uint8_t bval = 0, bval_old = 0;



  #ifdef PROGRAM_MODE
  usart_send_str(USART2, "Program Mode\r\n");
  usart_send(USART2, profile_bin, sizeof(profile_bin));
  status_led_pattern = STATUS_LED_PATTERN_ERR;

  i2c1_init();
  usart_send_str(USART2, "\r\nWriting...\r\n");
  delay(50);
  eeprom_bin_write(profile_bin);
  usart_send_str(USART2, "Done!\r\n");

  status_led_pattern = STATUS_LED_PATTERN_OK;
  while(1) delay(10);
  #else
  usart_send_str(USART2, "Run Mode\r\n");
  #endif

  #ifdef TEST_MODE
  w_memcpy(profile, profile_bin, sizeof(profile_bin));
  device_load_profile();

  debug("Done\r\n");
  #endif

  /* Get device profile from ROM */
  delay(50);

  //device_load_profile();
  device_init();
  delay(10);

  uint8_t i;
  for (i = 0; i < 16; i++)
    device_set_pin(i, 0);


  //uint16_t value = 4095;
  //uint8_t pin = 0;
  usart_send_str(USART2, "Begin\r\n");

  action_disp();
  while(1)
  {
    /* Wait for button press */
    uint32_t time1 = system_clock;
    while (system_clock - time1 < 100)
    {
      
      bval = GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN);
      if (bval == 1 && bval_old == 0)
      {
        usart_send_str(USART2, "Button pressed\r\n");
        delay(10);
        action_start();

        while (bval == 1)
        {
          bval = GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN);
          delay(1);
        }
          
        usart_send_str(USART2, "Button released\r\n");

        action_stop();
        time1 = system_clock;
      }

      bval_old = bval;
    }
    
    debug("Clock = ");
    debug_uint(system_clock);
    debug(" ind = ");
    debug_uint(ind);
    debug("\r\n");

    action_next();
  }


  while (1) delay(10);
  return 0;
}

void delay_res_100us(uint32_t dely)
{
  uint32_t ct = 0;
  for(ct = dely; ct!=0; ct--)
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

