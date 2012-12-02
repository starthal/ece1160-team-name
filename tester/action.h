#ifndef __FUNCTION_H
#define __FUNCTION_H

typedef enum
{
  ACTION_SET_PIN_TIME,
  ACTION_SET_PIN_MOMENT,
  ACTION_SET_PIN_PERM,
  NUM_ACTION_TYPES,
} action_type_t;



//set pin 9 time 3

typedef struct action_s
{
  /* Action variables */
  action_type_t action_type;
  uint8_t pin; /* 0 - 16 */
  uint16_t value; /* 12-bit */
  uint32_t time; /* units of 10ms */
}

  /* Next action to execute */
  struct action_s *next;
  struct menuitem_s* menuitem;
} action_t;

typedef struct menuitem_s
{
  //uint32_t actions_offset;
  action_t actions[12];
  uint32_t icon_offset;
} menuitem_t;

#endif
