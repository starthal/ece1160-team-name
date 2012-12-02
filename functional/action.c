
#include "lcd.h"
#include "action.h"

#include "bitmap.c"

uint8_t num_items = sizeof(icons) / sizeof(uint8_t *);
int ind = 0;

void action_disp()
{
  lcd_bitmap_rle(icons[ind]);
}

void action_next()
{
  ind++;
  if (ind == num_items)
    ind = 0;

  action_disp();
}
