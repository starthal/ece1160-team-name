#ifndef __W_UTILS
#define __W_UTILS

#include "stm32f4xx.h"

/* Useful for reading and writing addresses */
#define GET_BYTE(_i,_n) ((uint8_t)(0xFF & ( ((uint64_t)_i) >> (8 * (_n)))))
#define GET_HI_32(_i)   ((uint32_t) 0x00000000FFFFFFFF & ( ((uint64_t) _i)) >> 32 )
#define GET_LO_32(_i)   ((uint32_t) 0x00000000FFFFFFFF & ((uint64_t) _i))

typedef unsigned int size_t;

uint16_t crc16(uint8_t *data_p, uint16_t length);
void* w_memcpy(void* dest, const void* src, size_t n);
char* w_strncpy(char *dest, const char *src, size_t n);
char* w_strcpy(char *dest, const char *src);
int w_strcmp(char* stra, char* strb);
int w_strncmp(char* stra, char* strb, size_t len);
size_t w_strlen(const char* str);
char* w_strncat(char *dest, const char *src, size_t n);
char* w_strcat(char *dest, const char *src);
void uint8tos(char* dest, uint8_t num);
void uint32tox(char* dest, uint32_t num, uint8_t width);
void uint32tos(char* dest, uint32_t num, uint8_t width);
uint8_t ctoi(char ch);
#endif
