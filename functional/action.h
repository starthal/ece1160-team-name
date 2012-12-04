#ifndef __FUNCTION_H
#define __FUNCTION_H

#define MAX_MENUITEMS 16
#define MAX_ACTIONS 8

typedef enum
{
  ACTION_SET_PIN_TIME,
  ACTION_SET_PIN_MOMENT,
  ACTION_SET_PIN_PERM,
  NUM_ACTION_TYPES,
} action_type_t;


/* To be used in the database */
typedef struct db_action_s
{
  /* Action variables */
  action_type_t type;
  uint32_t pin; /* 0 - 16 */
  uint32_t value; /* 12-bit */
  uint32_t time; /* units of 10ms */
} db_action_t;

typedef struct db_menuitem_s
{
  //uint32_t actions_offset;
  uint32_t icon_size;
  uint32_t num_actions;
} db_menuitem_t;

/* To be used in RAM */
typedef db_action_t action_t;

typedef struct menuitem_s
{
  /* Points to a uint8_t array */
  uint8_t *icon;
  uint16_t icon_size;
  action_t actions[MAX_ACTIONS];
  uint8_t num_actions;
} menuitem_t;
#endif
