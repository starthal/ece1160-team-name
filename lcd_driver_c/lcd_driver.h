#include <stdint.h>
#include <stdbool.h>

void init_lcd(void);
void lcd_setup_window(uint8_t sx, uint8_t sy, uint8_t ex, uint8_t ey);
void fillrect(uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy, uint8_t color);
void setup_pix_blit(uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy);
void pix_blit(uint8_t color);

void putch(uint8_t x, uint8_t y, char c);

uint8_t putstr_pgm(uint8_t x, uint8_t y, PGM_P p, uint8_t fg, uint8_t bg);
uint8_t putstr_ram(uint8_t x, uint8_t y, char * p, uint8_t fg, uint8_t bg);
uint8_t printDecimal(uint8_t x, uint8_t y, uint32_t val, uint8_t fg, uint8_t bg);

void lcd_set_brightness_parms(uint8_t a, uint8_t b);

