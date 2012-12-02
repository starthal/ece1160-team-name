#ifndef __ACTION_H
#define __ACTION_H

#include "stm32f4xx.h"


typedef struct action_s
{
  uint8_t* image;
  uint8_t is_action;
  struct menuitem_s* menuitem;
} action_t;

typedef struct menuitem_s
{
  struct action_s function;
  struct menuitem_s *next;
} menuitem_t;

void action_disp();

void action_next();

#endif
