/* Generates a binary image of menu items and associated actions/icons
   for the EEPROM */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "action.h"

int main(int argc, char** argv)
{
  char buf[512] = "xxx";
  char label[512] = "xxx";
  char type[512] = "xxx";
  int num_menu = 0, num_action = 0;
  int num_action_tot, i, pin, pwm, time;

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
   sscanf(buf, "%s %s %d", &label, &type, &num_action);

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

   for(i = 0; i < num_action; i++) // Verify actions
   {
    fgets(buf, 512, fp_in);
    sscanf(buf, "%s %s %d %d %d", &label, &type, &pin, &pwm, &time);
    if(strcmp(label, "action") != 0)
    {
     fprintf(stderr, "Invalid file format on menu item %d.", num_menu-1);
     return 1;
    } else
    {
     printf("%s %s %d %d %d\n", label, type, pin, pwm, time);
    }
   }
  }
 fprintf(stdout, "File format verified.\nMenu items: %d\nAction items:%d\n", num_menu, num_action_tot);

 // Build binary file
 rewind(fp_in);

 FILE *fp_out = fopen(argv[2], "w");
 fwrite(&num_menu, sizeof(int), 1, fp_out);	// write number of menu items

 FILE *img_out = tmpfile(); // Buffer file to store image binaries
 FILE *img;	// Points to the current image
 char byte_buf;

 int img_off = 2*num_menu + num_action_tot; //image offset

 FILE *act_out = tmpfile(); // Buffer file for actions

 while(1)
 {
  fgets(buf,512,fp_in);
  if(feof(fp_in)) break; //Check for EOF

  sscanf(buf, "%s %s %d", &label, &type, &num_action);
  printf("Menu id: %s\n", type);
  //Format should be OKAY so don't check

  //Calculate image offset
  strcat(type, ".bin");
  img = fopen(type, "rb");
  while(1)
  {
   fread(&byte_buf, 1, 1, img);
   if(feof(img)) break;
   fwrite(&byte_buf, 1, 1, img_out);
   img_off++;
  }
  fclose(img);

  //Write out menu_item
  fwrite(&img_off, sizeof(int), 1, fp_out);
  fwrite(&num_action, sizeof(int), 1, fp_out);

  //Parse actions
  int type_id;
  for(i =0; i < num_action; i++)
  {
   fgets(buf, 512, fp_in);
   sscanf(buf, "%s %s %d %d %d", &label, &type, &pin, &pwm, &time);

   if(strcmp(type, "time") == 0) type_id = ACTION_SET_PIN_TIME;
   else if(strcmp(type, "moment") == 0 ) type_id = ACTION_SET_PIN_MOMENT;
        else if(strcmp(type, "perm") == 0) type_id = ACTION_SET_PIN_PERM;
             else
             {
              fprintf(stderr, "Unexpected action type!\nType: %s\n",  type);
              return 1;
             }
   printf("Type id of %s is %d\n", type, type_id);

   fwrite(&type_id, sizeof(int), 1, act_out);
   fwrite(&pin, sizeof(int), 1, act_out);
   fwrite(&pwm, sizeof(int), 1, act_out);
   fwrite(&time, sizeof(int), 1, act_out);
  }
 }
 fclose(fp_in);

 //Write out actions
 rewind(act_out);
 while(1)
 {
  fread(&byte_buf, 1, 1, act_out);
  if(feof(act_out)) break;
  fwrite(&byte_buf, 1, 1, fp_out);
 }
 fclose(act_out);

 //Write out images
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
