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

#define TC0_ID 12

#define TC_CLKEN            0x1
#define TC_CLKDIS           0x2
#define TC_SWTRG            0x4

#define TC_CLKS                  0x7
#define TC_CLKS_MCK2             0x0
#define TC_CLKS_MCK8             0x1
#define TC_CLKS_MCK32            0x2
#define TC_CLKS_MCK128           0x3
#define TC_CLKS_MCK1024          0x4

#define TC_CLKS_SLCK             0x4

#define TC_CLKS_XC0              0x5
#define TC_CLKS_XC1              0x6
#define TC_CLKS_XC2              0x7

#define TC_CMR_CPCTRG (1 << 14)

#define TC_COVFS            0x1         /* Counter Overflow Status */
#define TC_LOVRS            0x2         /* Load Overrun Status */
#define TC_CPAS             0x4         /* RA Compare Status */
#define TC_CPBS             0x8         /* RB Compare Status */
#define TC_CPCS             0x10        /* RC Compare Status */
#define TC_LDRAS            0x20        /* RA Loading Status */
#define TC_LDRBS            0x40        /* RB Loading Status */
#define TC_ETRGS            0x80        /* External Trigger Status */
#define TC_CLKSTA           0x10000     /* Clock Status */
#define TC_MTIOA            0x20000     /* TIOA Mirror */
#define TC_MTIOB            0x40000     /* TIOB Status */


static CTL_ISR_FN_t userTimerISR;

unsigned long idleCount;

static int count;

static void
timerISR(void)
{
  userTimerISR();
  /* Clear the timer interrupt */
  TC0_SR;
}

void
SetTimerISR(CTL_ISR_FN_t isr)
{
  PMC_PCER = 1 << TC0_ID; /* Ensure T0 peripheral clock is running */
  TC0_CCR = TC_CLKDIS;
  TC0_IDR = 0xFFFFFFFF;
  TC0_CMR = TC_CLKS_MCK128 | TC_CMR_CPCTRG;
  TC0_RC = ProcessorClockFrequency() / 128 / TicksPerSecond();
  TC0_IER = TC_CPCS;
  TC0_CCR = TC_CLKEN | TC_SWTRG;
  userTimerISR = isr;
  ctl_set_isr(TC0_ID, 1, CTL_ISR_TRIGGER_LOW_LEVEL, timerISR, 0);
  ctl_unmask_isr(TC0_ID);
}

unsigned long
TicksPerSecond(void)
{
  return 1000;
}

void
Idle()
{
  ++idleCount;
}


