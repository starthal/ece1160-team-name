// Copyright (c) 2004 Rowley Associates Limited.
//
// This file may be distributed under the terms of the License Agreement
// provided with this software.
//
// THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#include "test.h"
#include <ctl_api.h>
#include <targets/AT91SAM7.h>
#include "defines.h"


#define PIOA_ID 2

#define SW1 (1 << 19)
#define SW2 (1 << 20)
#define SW3 (1 << 15)
#define SW4 (1 << 14)

void BoardInit(void)
{  
  //enable the clock of the PIO
  PMC_PCER = 1 << PIOA_ID;

  //set reg to 1
  PIOA_SODR = BIT17 | BIT18 | BIT15;

  //Configure in Output - PA17, PA18 (led 1, led 2)
  PIOA_OER = BIT17 | BIT18 | BIT15;

  //Configure as Input PA19, PA20 (button 1, button 2)
  PIOA_ODR = BIT19 | BIT20;

  //Enable PA17, PA18 PA19, PA20
  PIOA_PER = BIT17 | BIT18 | BIT15 | BIT19 | BIT20;


	RSTC_MR = 0xa5000101;					// enable the RESET pin to reset CPU

}

unsigned long
ProcessorClockFrequency(void)
{
  return 47923200;
}

static CTL_ISR_FN_t userButtonISR;
static void
buttonISR_IRQ0(void)
{
  userButtonISR();
}

static void
buttonISR_PIOB(void)
{
  unsigned long isr = PIOA_ISR;
  unsigned long pdsr = PIOA_PDSR;
  userButtonISR();
}

void
SetButtonPressedISR(CTL_ISR_FN_t isr)
{
  userButtonISR = isr;

  // Setup PIO
  PIOA_ODR = SW1 | SW2 | SW3 | SW4;
  PIOA_PER = SW1 | SW2 | SW3 | SW4;
  PIOA_MDDR = ~(SW1 | SW2 | SW3 | SW4);
  PIOA_MDER = SW1 | SW2 | SW3 | SW4;
  PIOA_IER = SW1 | SW2 | SW3 | SW4;
  ctl_set_isr(PIOA_ID, 0, CTL_ISR_TRIGGER_LOW_LEVEL, buttonISR_PIOB, 0);
  ctl_unmask_isr(PIOA_ID);
}

