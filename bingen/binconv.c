#include <stdio.h>
#include <inttypes.h>

int main(int argc, char** argv)
{
  char fname[512] = "bin.c";
  FILE* fp_in = fopen(argv[1], "r");
  FILE* fp_out = fopen(fname, "w");

  if (fp_in == NULL || fp_out == NULL)
  {
    perror("");
    return 1;
  }
  
  fprintf(fp_out, "uint8_t profile_bin[] = { ");
  while (!feof(fp_in))
  {
    uint8_t* ch = 0;
    fread(&ch, 1, 1, fp_in);
    fprintf(fp_out, "0x%x,", ch);
  }
  fprintf(fp_out, "};");
  fclose(fp_out);
  return 0;
}
    
