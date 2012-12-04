/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * w_utils.c
 * Stephen Albert
 * Deere Smart Unloading: RF Boards
 * Created 16 May 2012
 *
 * These are various utility functions. Some are replicas of functions
 * in string.h preceded by "w_", because string.h causes problems
 * with the stack. 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


#include "w_utils.h"


#define POLY 0x8408

/*
 *                                      16   12   5
 * this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
 * This works out to be 0x1021, but the way the algorithm works
 * lets us use 0x8408 (the reverse of the bit pattern).  The high
 * bit is always assumed to be set, thus we only use 16 bits to
 * represent the 17 bit value.
 */

uint16_t crc16(uint8_t *data_p, uint16_t length)
{
      uint8_t i;
      uint32_t data;
      uint32_t crc = 0xffff;

      if (length == 0)
            return (~crc);

      do
      {
            for (i=0, data=(uint32_t)0xff & *data_p++;
                 i < 8; 
                 i++, data >>= 1)
            {
                  if ((crc & 0x0001) ^ (data & 0x0001))
                        crc = (crc >> 1) ^ POLY;
                  else  crc >>= 1;
            }
      } while (--length);

      crc = ~crc;
      data = crc;
      crc = (crc << 8) | (data >> 8 & 0xff);

      return (crc);
}

uint8_t ctoi(char ch)
{
  uint8_t val = (uint8_t)(0xFF & ch);
  if (val >= '0' && val <= '9')
    return (val - '0');
  else if (val >= 'a' && val <= 'f')
    return (val - 'a' + 10);
  else if (val >= 'A' && val <= 'F')
    return (val - 'A' + 10);
  else
    return 0;
}

/*
char uint8tox(uint8_t num)
{
  
}*/

void uint32tox(char* dest, uint32_t num, uint8_t width)
{
  char chs[9];
  int8_t i, j;
  for (i=7; i>=0; i--)
  {
    if (num%16 > 9)
      chs[i] = 'a' - 10 + (num%16);
    else
      chs[i] = '0' + (num%16);
    num /= 16;
  }
  if (width == 0)
  {
    i = 0;
    while (i < 7 && chs[i] == '0') i++;
  }
  else
  {
    i = 8 - width;
  }
  j = i;
  for ( ; i < 8; i++)
  {
    dest[i-j] = chs[i];
  }
  dest[i-j] = '\0';
}

void uint32tos(char* dest, uint32_t num, uint8_t width)
{
  /* max 4294967296: 10 digits */
  char chs[11];
  int8_t i, j;
  for (i = 9; i >= 0; i--)
  {
    chs[i] = 48 + num % 10;
    num /= 10;
  }
  if (width == 0)
  {
    i = 0;
    while (i < 9 && chs[i] == '0') i++;
  }
  else
  {
    i = 10 - width;
  }
  j = i;
  for ( ; i < 10; i++)
  {
    dest[i-j] = chs[i];
  }
  dest[i-j] = '\0';
}

/* Converts an integer to a string */
/* dest must have size >= 4 */
void uint8tos(char* dest, uint8_t num)
{
  /* max 255: 3 digits */
  char chs[3];
  int i, j;
  for (i = 2; i >= 0; i--)
  {
    chs[i] = 48 + num % 10;
    num /= 10;
  }
  i = 0;
  while (i < 2 && chs[i] == '0') i++;
  j = i;
  while (i < 3)
  {
    dest[i-j] = chs[i];
    i++;
  }
  dest[i-j] = '\0';
}

void* w_memcpy(void* dest, const void* src, size_t n)
{
  size_t i;
  for (i = 0; i < n; i++)
    ((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
  return dest;
}

char* w_strncpy(char *dest, const char *src, size_t n)
{
   size_t i;

   for (i = 0; i < n && src[i] != '\0'; i++)
       dest[i] = src[i];
   for ( ; i < n; i++)
       dest[i] = '\0';

   return dest;
}

char* w_strcpy(char *dest, const char *src)
{
   size_t i;
   for (i = 0; src[i] != '\0'; i++)
       dest[i] = src[i];
   return dest;
}

int w_strcmp(char* stra, char* strb)
{
  unsigned int ind = 0;
  while (stra[ind] != 0 && strb[ind] !=0)
  {
    if (stra[ind] != strb[ind])
      return -1;
    ind++;
  }
  if (stra[ind] == 0 && strb[ind] == 0)
    return 0;
  else
    return -1;
}

int w_strncmp(char* stra, char* strb, size_t len)
{
  unsigned int ind = 0;
  while (ind < len && stra[ind] != 0 && strb[ind] !=0)
  {
    if (stra[ind] != strb[ind])
      return -1;
    ind++;
  }
  if (ind == len || (stra[ind] == 0 && strb[ind] == 0))
    return 0;
  else
    return -1;
}

size_t w_strlen(const char* str)
{
  size_t i = 0;
  while (str[i] != '\0') i++;
  return i;
}

char* w_strncat(char *dest, const char *src, size_t n)
{
   size_t dest_len = w_strlen(dest);
   size_t i;

   for (i = 0 ; i < n && src[i] != '\0' ; i++)
       dest[dest_len + i] = src[i];
   dest[dest_len + i] = '\0';

   return dest;
}

char* w_strcat(char *dest, const char *src)
{
  return w_strncat(dest, src, w_strlen(src));
}
