/* Generates a binary image of menu items and associated actions/icons
   for the EEPROM */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include "action.h"

uint32_t main(uint32_t argc, char** argv)
{
  char buf[512] = "xxx";
  char label[512] = "xxx";
  char type[512] = "xxx";
  uint32_t num_menu = 0, num_action = 0;
<<<<<<< HEAD
  uint32_t num_action_tot = 0;
  uint32_t i, pin, pwm, time;
=======
  uint32_t num_action_tot, i, pin, pwm, time;
>>>>>>> ca0f99fbb4d987681885badb1ea0332558cc339d

  if (argc != 3)
  {
    fprintf(stderr, "need input and output\n");
    return 1;
  }

  FILE *fp_in = fopen(argv[1], "rb");

  // Verify file structure and count menu_item and actions
  while(1)
  {
    fgets(buf, 512, fp_in);
    if(feof(fp_in)) break;	//Check for EOF
    sscanf(buf, "%s %s %u", label, type, &num_action);

    if(strcmp(label, "menu_item") != 0)
    {
      fprintf(stderr, "feof: %d\n", feof(fp_in));
      fprintf(stderr, "Invalid file format. Expected 'menu_item'. num_menu: %d\n", num_menu);
      fprintf(stderr, "Label: %s\n", label);
      return 1;
    }
    if(num_action == 0)
    {
      fprintf(stderr, "Invalid number of actions on menu item %d\n", num_menu);
      return 1;
    }

    num_menu++;
    num_action_tot += num_action;

<<<<<<< HEAD
    for(i = 0; i < num_action; i++) // Verify actions
    {
      fgets(buf, 512, fp_in);
      sscanf(buf, "%s %s %u %u %u", label, type, &pin, &pwm, &time);
      if(strcmp(label, "action") != 0)
      {
        fprintf(stderr, "Invalid file format on menu item %d.", num_menu-1);
        return 1;
      }
      else
      {
        printf("%s %s %u %u %u\n", label, type, pin, pwm, time);
      }
    }
  }
  fprintf(stdout, "File format verified.\nMenu items: %d\nAction items:%d\n", num_menu, num_action_tot);

  // Build binary file
  rewind(fp_in);

  FILE *fp_out = fopen(argv[2], "w");
  fwrite(&num_menu, sizeof(uint32_t), 1, fp_out);	// write number of menu items
=======
 FILE *fp_out = fopen(argv[2], "w");
 fwrite(&num_menu, sizeof(uint32_t), 1, fp_out);	// write number of menu items

 FILE *img_out = tmpfile(); // Buffer file to store image binaries
 FILE *img;	// Pouint32_ts to the current image
 char byte_buf;

 uint32_t img_off; //image offset
>>>>>>> ca0f99fbb4d987681885badb1ea0332558cc339d

  FILE *img_out = tmpfile(); // Buffer file to store image binaries
  FILE *img;	// Pouint32_ts to the current image
  char byte_buf;

<<<<<<< HEAD
  //uint32_t img_off = 2*num_menu + num_action_tot * sizeof(db_action_t); //image offset
=======
 while(1)
 {
  img_off = 0;
  fgets(buf,512,fp_in);
  if(feof(fp_in)) break; //Check for EOF
>>>>>>> ca0f99fbb4d987681885badb1ea0332558cc339d

  FILE *act_out = tmpfile(); // Buffer file for actions

  while(1)
  {
    db_menuitem_t menuitem;

<<<<<<< HEAD
    fgets(buf,512,fp_in);
    if(feof(fp_in)) break; //Check for EOF

    sscanf(buf, "%s %s %u", label, type, &num_action);
    menuitem.num_actions = num_action;

    printf("Menu id: %s\n", type);
    //Format should be OKAY so don't check

    //Calculate image offset
    strcat(type, ".bin");
    img = fopen(type, "rb");
    if (img == NULL)
    {
      fprintf(stderr, "Icon file %s failed to open", type);
      perror(" ");
      exit(1);
    }
    fseek(img, 0, SEEK_END);
    menuitem.icon_size = ftell(img);
    printf("Icon size: %d\n", menuitem.icon_size);
    rewind(img);
    while(1)
    {
     fread(&byte_buf, 1, 1, img);
     if(feof(img)) break;
     fwrite(&byte_buf, 1, 1, img_out);
     //img_off++;
    }
    fclose(img);

    //Write out menu_item
    printf("Writing menuitem at output offset %ld\n", ftell(fp_out));
    fwrite(&menuitem, sizeof(db_menuitem_t), 1, fp_out);
    //fwrite(&img_off, sizeof(uint32_t), 1, fp_out);
    //fwrite(&num_action, sizeof(uint32_t), 1, fp_out);

    //Parse actions
    uint32_t type_id;
    for(i =0; i < num_action; i++)
    {
      db_action_t action;
      
        fgets(buf, 512, fp_in);
        sscanf(buf, "%s %s %u %u %u", label, type, &pin, &pwm, &time);
=======
  //Write out menu_item
  fwrite(&img_off, sizeof(uint32_t), 1, fp_out);
  fwrite(&num_action, sizeof(uint32_t), 1, fp_out);

  //Parse actions
  uint32_t type_id;
  for(i =0; i < num_action; i++)
  {
   fgets(buf, 512, fp_in);
   sscanf(buf, "%s %s %d %d %d", &label, &type, &pin, &pwm, &time);
>>>>>>> ca0f99fbb4d987681885badb1ea0332558cc339d

        if(strcmp(type, "time") == 0) type_id = ACTION_SET_PIN_TIME;
        else if(strcmp(type, "moment") == 0 ) type_id = ACTION_SET_PIN_MOMENT;
        else if(strcmp(type, "perm") == 0) type_id = ACTION_SET_PIN_PERM;
<<<<<<< HEAD
        else
        {
          fprintf(stderr, "Unexpected action type!\nType: %s\n",  type);
          return 1;
        }
        printf("Type id of %s is %d\n", type, type_id);

        // Write action
        action.type = type_id;
        action.pin = pin;
        action.value = pwm;
        action.time = time;

        printf("Writing an action at act_out offset %ld\n", ftell(act_out));
        fwrite(&action, sizeof(db_action_t), 1, act_out);
        //fwrite(&type_id, sizeof(uint32_t), 1, act_out);
        //fwrite(&pin, sizeof(uint32_t), 1, act_out);
        //fwrite(&pwm, sizeof(uint32_t), 1, act_out);
        //fwrite(&time, sizeof(uint32_t), 1, act_out);
    }
=======
             else
             {
              fprintf(stderr, "Unexpected action type!\nType: %s\n",  type);
              return 1;
             }
   printf("Type id of %s is %d\n", type, type_id);

   fwrite(&type_id, sizeof(uint32_t), 1, act_out);
   fwrite(&pin, sizeof(uint32_t), 1, act_out);
   fwrite(&pwm, sizeof(uint32_t), 1, act_out);
   fwrite(&time, sizeof(uint32_t), 1, act_out);
  }
>>>>>>> ca0f99fbb4d987681885badb1ea0332558cc339d
 }
 fclose(fp_in);

  //Write out actions
  printf("Actions begin at offset %ld\n", ftell(fp_out));
  rewind(act_out);
  while(1)
  {
    fread(&byte_buf, 1, 1, act_out);
    if(feof(act_out)) break;
    fwrite(&byte_buf, 1, 1, fp_out);
  }
  fclose(act_out);

  //Write out images
  printf("Images begin at offset %ld\n", ftell(fp_out));
  rewind(img_out);
  while(1)
  {
    fread(&byte_buf, 1, 1, img_out);
    if(feof(img_out)) break;
    fwrite(&byte_buf, 1, 1, fp_out);
  }
    fclose(img_out);
    fclose(fp_out);

 return 0;
}
