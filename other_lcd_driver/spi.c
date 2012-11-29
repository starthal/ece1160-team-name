#include <targets/AT91SAM7.h>
#include "spi.h" 
#include "defines.h"


//*********************************************************************
//********************  SYSTERM HEARTBEAT @ 10 ms *********************
//*********************************************************************
void InitSPI (void)
{
  //set functionalite to pins:
  //port0.11 -> NPCS0
  //port0.12 -> MISO
  //port0.13 -> MOSI
  //port0.14 -> SPCK
  PIOA_PDR = BIT11 | BIT12 | BIT13 | BIT14;
  PIOA_ASR = BIT11 | BIT12 | BIT13 | BIT14;
  PIOA_BSR = 0;


  PMC_PCER |= 1 << 5; // Enable SPI timer clock.

  /****  Fixed mode ****/
  SPI_CR   = 0x81;					//SPI Enable, Sowtware reset
  SPI_CR   = 0x01;					//SPI Enable



  SPI_MR	= 0x000E0011;                           //Master mode
  SPI_CSR0	= 0x01010B11;                           //9 bit

}
