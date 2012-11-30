#include "spi9.h"

#define TIM_PREEMPT_PRIORITY 0
#define TIM_SUB_PRIORITY 0

volatile int count = 0;
volatile int max_micros = 0;

void tim_setup(void)
{

  /* Enable TIM2 clock. */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_InternalClockConfig(TIM2);

  TIM_TimeBaseInitTypeDef tim_init;
  tim_init.TIM_Prescaler = 30;
  tim_init.TIM_CounterMode = TIM_CounterMode_Up;
  tim_init.TIM_Period = 1;
  tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM2, &tim_init);

  /* Enable TIM2 interrupt */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitTypeDef tim_nvic_init;
  tim_nvic_init.NVIC_IRQChannel = TIM2_IRQn;
  tim_nvic_init.NVIC_IRQChannelPreemptionPriority = TIM_PREEMPT_PRIORITY;
  tim_nvic_init.NVIC_IRQChannelSubPriority = TIM_SUB_PRIORITY;
  tim_nvic_init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&tim_nvic_init);
    
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  
  return;
}

void tim2_isr(void)
{
	if (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update))
  {
		/* Clear compare interrupt flag. */
		timer_clear_flag(TIM2, TIM_SR_CC1IF);

		/* compare value = 1 microsecond */
		timer_set_oc_value(TIM2, TIM_OC1, 1);
		
		/* count max_micros microseconds, then count = 0. */
		if(count >= 5000000)
			count = 0;
		count++;

	}
}

// delay microseconds
void udelay(int micros){

	max_micros = micros;
	count = 0;
	while(count < micros);

	return;	
	}


void clock_setup(void){
  RCC_Clocks
	//Sys Clock
	rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_120MHZ]);
	}



#define SPI_GPIO      GPIOB
#define GPIO_SPI_NSS  GPIO_Pin_
#define GPIO_SPI_SCL
#define GPIO_SPI_MOSI
#define GPIO_LCD_RST  GPIO_Pin


//Description: Sends a 9 bit command over SPI to the LCD with
//			   the first bit LOW indicating "data" mode.
void spi_write(uint8_t is_cmd, uint8_t data)
{
  /* Enable chip */
	GPIO_ResetBits(SPI_GPIO, GPIO_SPI_SCL);
	GPIO_ResetBits(SPI_GPIO, GPIO_SPI_NSS);
  
  if (is_cmd)
    GPIO_ResetBits(SPI_GPIO, GPIO_SPI_MOSI);
  else
    GPIO_SetBits(SPI_GPIO, GPIO_SPI_MOSI);
  
  /* Clock pulse */
  GPIO_SetBits(SPI_GPIO, GPIO_SPI_SCL);
  GPIO_ResetBits(SPI_GPIO, GPIO_SPI_SCL);

  uint8_t j;
	for (j = 0; j < 8; j++)
  {
		if ((data & 0x80) == 0x80)
			GPIO_SetBits(SPI_GPIO, GPIO_SPI_MOSI);
    else 
			GPIO_ResetBits(SPI_GPIO, GPIO_SPI_MOSI);

    /* Clock pulse */
    GPIO_SetBits(SPI_GPIO, GPIO_SPI_SCL);
    GPIO_ResetBits(SPI_GPIO, GPIO_SPI_SCL);
        
    data <<= 1;
  }

  GPIO_SetBits(SPI_GPIO, GPIO_SPI_NSS);  //Disable chip

  return;
}
