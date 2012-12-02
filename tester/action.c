
#include "bitmap.c"

uint8_t *bitmaps[4] = {arrow_rt, arrow_up, arrow_lt, arrow_dn};
uint8_t num_items = 4;
int ind = 0;

void action_disp()
{
  lcd_bitmap_r132(bitmaps[ind]);
}

void action_next()
{
  ind++;
  if (ind == num_items)
    ind = 0;

  action_disp();
}

void parse_menu()
{
  FILE* fp_in = fopen("mario.device", "r");

  fclose(fp_in);
}
