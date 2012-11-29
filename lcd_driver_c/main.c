/*
 * Basic Nokia 6100 LCD example. Licenced under the GNU GPL.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "lcd_driver.h"

const uint8_t hello_world[] PROGMEM = "Hello World";

int main()
{
	// Setup the faster clock speed
	CLKPR=0x80;
	CLKPR=0x00;

	// Enable the IO lines
	PORTD |= 0xE0;
	DDRD = 0xE0;	
	
	// LCD setup
	init_lcd();

	// Setup the LCD brightness - these defaults seem to work well for
	// the new LCD batch
	lcd_set_brightness_parms(25, 3);

	// Fill screen with Green color
	fillrect(0, 0, 130, 130, 0x1C);

	// Standard greeting
	putstr_pgm(10, 7, hello_world, 0x00, 0x1C);

	// Do nothing... forever
	while (1);
	return 0;
}
