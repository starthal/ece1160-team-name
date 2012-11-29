#include <targets/AT91SAM7.h>
#include "serial.h"
#include "defines.h"


#define PROCESSOR_CLOCK_FREQUENCY 47923200

#define US_CLKS                 0x0030      /* Clock Selection */
#define US_CLKS_MCK             0x00        /* Master Clock */
#define US_CLKS_MCK8            0x10        /* Master Clock divided by 8 */
#define US_CLKS_SCK             0x20        /* External Clock */
#define US_CLKS_SLCK            0x30        /* Slow Clock */

#define US_CHRL                 0x00C0      /* Byte Length */
#define US_CHRL_5               0x00        /* 5 bits */
#define US_CHRL_6               0x40        /* 6 bits */
#define US_CHRL_7               0x80        /* 7 bits */
#define US_CHRL_8               0xC0        /* 8 bits */

#define US_SYNC                 0x0100      /* Synchronous Mode Enable */

#define US_PAR                  0x0E00      /* Parity Mode */
#define US_PAR_EVEN             0x00        /* Even Parity */
#define US_PAR_ODD              0x200       /* Odd Parity */
#define US_PAR_SPACE            0x400       /* Space Parity to 0 */
#define US_PAR_MARK             0x600       /* Marked Parity to 1 */
#define US_PAR_NO               0x800       /* No Parity */
#define US_PAR_MULTIDROP        0xC00       /* Multi-drop Mode */

#define US_NBSTOP               0x3000      /* Stop Bit Number */
#define US_NBSTOP_1             0x0000      /* 1 Stop Bit */
#define US_NBSTOP_1_5           0x1000      /* 1.5 Stop Bits */
#define US_NBSTOP_2             0x2000      /* 2 Stop Bits */

#define US_CHMODE                   0xC000  /* Channel Mode */
#define US_CHMODE_NORMAL            0x0000  /* Normal Mode */
#define US_CHMODE_AUTOMATIC_ECHO    0x4000  /* Automatic Echo */
#define US_CHMODE_LOCAL_LOOPBACK    0x8000  /* Local Loopback */
#define US_CHMODE_REMOTE_LOOPBACK   0xC000  /* Remote Loopback */

#define US_MODE9                0x20000     /* 9 Bit Mode */

#define US_CLKO                 0x40000     /* Baud Rate Output Enable */

#define US_RXRDY            0x1       /* Receiver Ready */
#define US_TXRDY            0x2       /* Transmitter Ready */
#define US_RXBRK            0x4       /* Receiver Break */
#define US_ENDRX            0x8       /* End of Receiver PDC Transfer */
#define US_ENDTX            0x10       /* End of Transmitter PDC Transfer */
#define US_OVRE             0x20       /* Overrun Error */
#define US_FRAME            0x40       /* Framing Error */
#define US_PARE             0x80       /* Parity Error */
#define US_TIMEOUT          0x100       /* Receiver Timeout */
#define US_TXEMPTY          0x200       /* Transmitter Empty */

#define US_RSTRX                0x0004      /* Reset Receiver */
#define US_RSTTX                0x0008      /* Reset Transmitter */
#define US_RXEN                 0x0010      /* Receiver Enable */
#define US_RXDIS                0x0020      /* Receiver Disable */
#define US_TXEN                 0x0040      /* Transmitter Enable */
#define US_TXDIS                0x0080      /* Transmitter Disable */
#define US_RSTSTA               0x0100      /* Reset Status Bits */
#define US_STTBRK               0x0200      /* Start Break */
#define US_STPBRK               0x0400      /* Stop Break */
#define US_STTTO                0x0800      /* Start Time-out */
#define US_SENDA                0x1000      /* Send Address */

void UARTInitialize(unsigned long baudrate)
{
  US0_CR = US_RSTRX | US_RSTTX | US_RXDIS | US_TXDIS;
  US0_RCR = 0x00000000;
  US0_TCR = 0x00000000;
  US0_IDR = 0xFFFFFFFF;
  PIOA_PDR |= 0x00000060;
  US0_MR = US_CLKS_MCK | US_CHRL_8 | US_PAR_NO | US_NBSTOP_2 | US_CHMODE_NORMAL;
  US0_BRGR = PROCESSOR_CLOCK_FREQUENCY / (16 * baudrate);
  US0_TTGR = 0;
  US0_CR = US_TXEN | US_RXEN;

  PMC_PCER |= 1 << 6; // Enable USART0 timer clock.
}

void UARTWriteChar(char ch)
{
  while (!(US0_CSR & US_TXRDY));
  US0_THR = ch;
}

char UARTReadChar()
{
  while (!(US0_CSR & US_RXRDY));
  return (char)US0_RHR;
}

int UARTReadAvailable(void)
{
  return US0_CSR & US_RXRDY;
}

void __putchar(int ch)
{
  UARTWriteChar(ch);
}

