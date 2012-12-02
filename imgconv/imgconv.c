
/* Converts an 132x132 24-bit PNM image into a 12-bit bitmap */
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define OUTPUT_BINARY
#define OUTPUT_LENGTH

#define SIDE_PX (132)

//Make sure 12-bit is not defined if using OUTPUT_LENGTH
#ifdef DEPTH_12
 #ifdef OUTPUT_LENGTH
  #undef DEPTH_12
 #endif
#endif

#ifdef DEPTH_12
   #define NUM_BYTES (SIDE_PX*SIDE_PX/2*3)
#else
  #define NUM_BYTES (SIDE_PX*SIDE_PX)
#endif
#define FILE_WIDTH (80)
#define BYTES_PER_LINE ((FILE_WIDTH-2)/5)

uint8_t img_out[NUM_BYTES];

int main(int argc, char** argv)
{
  int i;
  int width, height;
  char buf[512] = "xxx";

  if (argc < 2)
  {
    fprintf(stderr, "need input and output\n");
    return 1;
  }
  
  FILE *fp_in = fopen(argv[1], "rb");

  while (strncmp(buf, "P6", 2))
  {
    fgets(buf, 512, fp_in);
    if (feof(fp_in))
    {
      fprintf(stderr, "bad format\n");
      return 1;
    }
  }

  fgets(buf, 512, fp_in);

  while (strncmp(buf, "#", 1) == 0)
  {
    fgets(buf, 512, fp_in);
  }
  
  sscanf(buf, "%d %d", &width, &height);
  if (width != SIDE_PX || height != SIDE_PX)
  {
    fprintf(stderr, "Must be %dx%d\n", SIDE_PX, SIDE_PX);
    return 1;
  }
  
  fgets(buf, 512, fp_in);
#ifdef DEPTH_12
  for (i = 0; i < NUM_BYTES; i+=3)
  {
    /* Read 2 pixels, convert to 3 bytes */
    //uint8_t a = fgetc(fp_in);
    uint8_t r = fgetc(fp_in);
    uint8_t g = fgetc(fp_in);
    uint8_t b = fgetc(fp_in);

    uint8_t r1 = r >> 4;
    uint8_t g1 = g >> 4;
    uint8_t b1 = b >> 4;

    //a = fgetc(fp_in);
    r = fgetc(fp_in);
    g = fgetc(fp_in);
    b = fgetc(fp_in);

    uint8_t r2 = r >> 4;
    uint8_t g2 = g >> 4;
    uint8_t b2 = b >> 4;

    img_out[i+0] = (uint8_t) ((r1 << 4) | g1); 
    img_out[i+1] = (uint8_t) ((b1 << 4) | r2);
    img_out[i+2] = (uint8_t) ((g2 << 4) | b2);
  }
#else
  for (i = 0; i < NUM_BYTES; i++)
  {
    uint8_t r = fgetc(fp_in);
    uint8_t g = fgetc(fp_in);
    uint8_t b = fgetc(fp_in);

    r >>= 5;
    g >>= 5;
    b >>= 6;

    img_out[i] = (uint8_t) ((r << 5) | (g << 2) | b);
  }

#endif

fclose(fp_in);

  FILE *fp_out = stdout; //fopen(argv[2], "w");
  #ifndef OUTPUT_BINARY
  fprintf(fp_out, "uint8_t %s[] = \n", argv[2]);
  fprintf(fp_out, "{\n  ");
  #endif

#ifdef OUTPUT_LENGTH
  //Output using run length encoding
  int length;

  for (i = 0; i < NUM_BYTES; i++)
  {
    length = 1;
    while(img_out[i] == img_out[i+1])
    {
      i++;
      length++;
      if(!(i < NUM_BYTES) || !(length < 255)) break;
    }
    #ifdef OUTPUT_BINARY
    fwrite(&length, 1, 1, fp_out);
    fwrite(&img_out[i], 1, 1, fp_out);
    #else
    fprintf(fp_out, "0x%0.2x,",length);
    fprintf(fp_out, "0x%0.2x,", img_out[i]);
    #endif
  }
  fprintf(fp_out, "0x00,");
#else
  //Output hex based char array
  for (i = 0; i < NUM_BYTES; i++)
  {
    fprintf(fp_out, "0x%0.2x,", img_out[i]);
    if (i % BYTES_PER_LINE == (BYTES_PER_LINE - 1))
      fprintf(fp_out, "\n  ");
  }
#endif

  #ifndef OUTPUT_BINARY
  fprintf(fp_out, "\n};\n");
  #endif


  fclose(fp_out);
  return 0;
}
