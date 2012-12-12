
#include "lcd.h"
#include "action.h"
#include "device.h"
//#include "bitmap.c"

//uint8_t num_items = sizeof(icons) / sizeof(uint8_t *);
uint8_t ind = 0;

void debug_hex(uint32_t num)
{
  char mesg[9];
  uint32tox(mesg, num, 0);
  debug(mesg);
}

void action_disp()
{
  uint16_t i;

  /* Read icons */
  //for (i = 0; i < num_menuitems; i++)
  //{
  //  debug("Icon at ");
  //  debug_hex(menu[i].icon);
  //  debug("\r\n");
  //}

  //debug("?");
  lcd_fill(BLACK);
  #if 1
  /* Debug icon */
  //debug_hex(menu[ind].icon);
  //debug("\r\n");
  /*
  for (i = 0; i < 1; i++)
  {
    char chs[8];
    uint32tox(chs, menu[ind].icon[i], 2);
    debug(chs);
    debug(" ");
  }*/
  debug("\r\n");
  #endif
  lcd_bitmap_rle(menu[ind].icon);
  //debug("!");
}

void action_next()
{
  ind++;
  if (ind == num_menuitems)
    ind = 0;

  action_disp();
}

void action_start()
{
  uint8_t i;
  debug("Start\r\n");
  for (i = 0; i < menu[ind].num_actions; i++)
  {
    action_t *act = &menu[ind].actions[i];
    switch(act->type)
    {
      case ACTION_SET_PIN_MOMENT:
        device_set_pin(act->pin, act->value);
        break;
      case ACTION_SET_PIN_PERM:
        break;
      case ACTION_SET_PIN_TIME:
        break;
      default:
        debug("ERR");
        break;
    }
  }
}

void action_stop()
{
  debug("Stahp\r\n");
  uint8_t i;
  for (i = 0; i < menu[ind].num_actions; i++)
  {
    action_t *act = &menu[ind].actions[i];
    switch(act->type)
    {
      case ACTION_SET_PIN_MOMENT:
        device_set_pin(act->pin, pin_old_vals[act->pin]);
        break;
      case ACTION_SET_PIN_PERM:
        break;
      case ACTION_SET_PIN_TIME:
        break;
      default:
        debug("ERR");
        break;
    }
  }
}
