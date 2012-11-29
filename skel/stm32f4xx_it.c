/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides all exceptions handler and peripherals interrupt
  *          service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "main.h"
#include "usart.h"
#include "bluetooth.h"
#include "zigbee.h"
#include "host.h"
#include "usb_core.h"
#include "usbd_core.h"
#include "usbd_cdc_core.h"
#include "usb_dcd_int.h"

/* FIXME uncomment if usb_dcd_int.h doesn't work */
//extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

  /* Located in main.c */
  system_tick();
}

/******************************************************************************/
/*                 STM32Fxxx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32fxxx.s).                                               */
/******************************************************************************/

void USART1_IRQHandler(void)
{
  if (USART_GetITStatus(USART1, USART_IT_TXE))
  {
    debug_tx_ready_isr();
  }
}

void USART3_IRQHandler(void)
{
  /* Xbee */
  /* Handle an RXNE flag set (incoming byte) */

  if (USART_GetITStatus(USART3, USART_IT_RXNE))
  {
    #ifdef ZB_BUFFERED
    zb_rx_ready_isr();
    #else
    zb_event();
    #endif
  }
  else
  {
    debug("!"); /* This means the TX interrupt is enabled (it shouldn't be) */
    //USART_ClearITPendingBit(USART3, USART_IT_TXE)
  }
  USART_ClearITPendingBit(USART3, USART_IT_RXNE);
}

void USART6_IRQHandler(void)
{
}

void USART2_IRQHandler(void)
{
  /* Bluetooth */
  #ifdef BOARD_PROD

  if (USART_GetITStatus(USART2, USART_IT_RXNE))
  {
    bt_rx_ready_isr();
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  }
  else if (USART_GetITStatus(USART2, USART_IT_TXE))
  {
    bt_tx_ready_isr();
    USART_ClearITPendingBit(USART2, USART_IT_TXE);
  }

  #endif
}

/**
  * @brief  This function handles EXTI0_IRQ Handler.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  //UserButtonPressed = 0x01;
  
  /* Clear the EXTI line pending bit */
  //EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
}

/**
  * @brief  This function handles EXTI15_10_IRQ Handler.
  * @param  None
  * @retval None
  */
void OTG_FS_WKUP_IRQHandler(void)
{
  if(gbl.usb_device.cfg.low_power)
  {
	/* Reset SLEEPDEEP and SLEEPONEXIT bits */
	SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

	/* After wake-up from sleep mode, reconfigure the system clock */
	SystemInit();
    USB_OTG_UngateClock(&gbl.usb_device);
  }
  //EXTI_ClearITPendingBit(EXTI_Line18);
}

/**
  * @brief  This function handles OTG_FS Handler.
  * @param  None
  * @retval None
  */
void OTG_FS_IRQHandler(void)
{
  USBD_OTG_ISR_Handler (&gbl.usb_device);
}
#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
/**
  * @brief  This function handles EP1_IN Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_IN_IRQHandler(void)
{
  USBD_OTG_EP1IN_ISR_Handler (&gbl.usb_device);
}

/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_OUT_IRQHandler(void)
{
  USBD_OTG_EP1OUT_ISR_Handler (&gbl.usb_device);
}
#endif

