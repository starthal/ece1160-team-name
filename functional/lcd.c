#ifdef USE_MATH
  #include <math.h>
#endif
#include "stm32f4xx.h"
#include "lcd.h"
#include "fonts.h"
#include "main.h"

/* Testing */
#include "action.h"
#include "device.h"

/* 17.4 KB */
uint8_t img_buf[132*132];

void lcd_gpio_init()
{
  RCC_AHB1PeriphClockCmd(LCD_GPIO_CLK, ENABLE);
  
  GPIO_InitTypeDef lcd_gpio_init_struct;

  /* SPI pins */
  lcd_gpio_init_struct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
  lcd_gpio_init_struct.GPIO_Mode = GPIO_Mode_OUT;
  lcd_gpio_init_struct.GPIO_OType = GPIO_OType_PP;
  lcd_gpio_init_struct.GPIO_PuPd = GPIO_PuPd_UP;
  lcd_gpio_init_struct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(LCD_GPIO, &lcd_gpio_init_struct);

  /* Enable chip */
  GPIO_ResetBits(LCD_GPIO, LCD_NSS_PIN);
}

/* Sends a 9 bit command over SPI to the LCD */
inline void lcd_send(uint16_t is_data, uint8_t data8)
{
  /* Enable chip */
  //GPIO_ResetBits(LCD_GPIO, LCD_NSS_PIN);

  uint16_t data9 = data8 | (is_data << 8);
  //if (is_data)
    //data9 |= 0x100;

  uint8_t j;
  
#ifdef WRITE_SLOW
  for (j = 0; j < 9; j++)
  {
    if ((data9 & 0x100) == 0x100)
      GPIO_SetBits(LCD_GPIO, LCD_MOSI_PIN);
    else 
      GPIO_ResetBits(LCD_GPIO, LCD_MOSI_PIN);

    /* Clock pulse */
    GPIO_SetBits(LCD_GPIO, LCD_SCL_PIN);
    GPIO_ResetBits(LCD_GPIO, LCD_SCL_PIN);
        
    data9 <<= 1;
  }
#else
  for (j = 0; j < 9; j++)
  {
    /* Clock high */
    uint16_t bits = LCD_SCL_PIN | LCD_RST_PIN;
    bits |= (data9 & 0x100) << 7;
    GPIO_Write(LCD_GPIO, bits);
    /* Clock low */
    bits &= ~LCD_SCL_PIN;
    GPIO_Write(LCD_GPIO, bits);

    data9 <<= 1;
  }
#endif

  /* Disable chip */
  //GPIO_SetBits(LCD_GPIO, LCD_NSS_PIN);  //Disable chip
}

inline void lcd_send_data(uint16_t data9)
{  
  uint16_t bits = LCD_SCL_PIN | LCD_RST_PIN | LCD_MOSI_PIN;
  GPIO_Write(LCD_GPIO, bits);
  bits = LCD_RST_PIN | LCD_MOSI_PIN;
  GPIO_Write(LCD_GPIO, bits);

  /* Clock pulse */
  bits = LCD_SCL_PIN | LCD_RST_PIN | ((data9 & (1<<7)) << 8);
  GPIO_Write(LCD_GPIO, bits);
  bits &= ~LCD_SCL_PIN;
  GPIO_Write(LCD_GPIO, bits);
  /* Clock pulse */
  bits = LCD_SCL_PIN | LCD_RST_PIN | ((data9 & (1<<6)) << 9);
  GPIO_Write(LCD_GPIO, bits);
  bits &= ~LCD_SCL_PIN;
  GPIO_Write(LCD_GPIO, bits);
  /* Clock pulse */
  bits = LCD_SCL_PIN | LCD_RST_PIN | ((data9 & (1<<5)) << 10);
  GPIO_Write(LCD_GPIO, bits);
  bits &= ~LCD_SCL_PIN;
  GPIO_Write(LCD_GPIO, bits);
  /* Clock pulse */
  bits = LCD_SCL_PIN | LCD_RST_PIN | ((data9 & (1<<4)) << 11);
  GPIO_Write(LCD_GPIO, bits);
  bits &= ~LCD_SCL_PIN;
  GPIO_Write(LCD_GPIO, bits);
  /* Clock pulse */
  bits = LCD_SCL_PIN | LCD_RST_PIN | ((data9 & (1<<3)) << 12);
  GPIO_Write(LCD_GPIO, bits);
  bits &= ~LCD_SCL_PIN;
  GPIO_Write(LCD_GPIO, bits);
  /* Clock pulse */
  bits = LCD_SCL_PIN | LCD_RST_PIN | ((data9 & (1<<2)) << 13);
  GPIO_Write(LCD_GPIO, bits);
  bits &= ~LCD_SCL_PIN;
  GPIO_Write(LCD_GPIO, bits);
  /* Clock pulse */
  bits = LCD_SCL_PIN | LCD_RST_PIN | ((data9 & (1<<1)) << 14);
  GPIO_Write(LCD_GPIO, bits);
  bits &= ~LCD_SCL_PIN;
  GPIO_Write(LCD_GPIO, bits);
  /* Clock pulse */
  bits = LCD_SCL_PIN | LCD_RST_PIN | ((data9 & 1     ) << 15);
  GPIO_Write(LCD_GPIO, bits);
  bits &= ~LCD_SCL_PIN;
  GPIO_Write(LCD_GPIO, bits);

  return;
}

void lcd_init()
{
  lcd_gpio_init();

  // Hardware reset
  GPIO_ResetBits(LCD_GPIO, LCD_RST_PIN);
  delay(5);
  GPIO_SetBits(LCD_GPIO, LCD_RST_PIN);
  delay(5);
  
  // Sleep out (command 0x11)
  lcd_send(LCD_CMD, SLEEPOUT);
  
  // Inversion off
  lcd_send(LCD_CMD, INVOFF);
  
  // Color Interface Pixel Format
  lcd_send(LCD_CMD, COLMOD);
  #ifdef DEPTH_12
  lcd_send(LCD_DATA, (0x03)); // 0x03 = 12 bits-per-pixel
  #else
  lcd_send(LCD_DATA, (0x02)); // 8 bpp
  #endif
  
  #ifndef DEPTH_12
  lcd_send(LCD_CMD, RGBSET);
  lcd_send(LCD_DATA, 0);
  lcd_send(LCD_DATA, 2);
  lcd_send(LCD_DATA, 5);
  lcd_send(LCD_DATA, 7);
  lcd_send(LCD_DATA, 9);
  lcd_send(LCD_DATA, 11);
  lcd_send(LCD_DATA, 14);
  lcd_send(LCD_DATA, 16);
  lcd_send(LCD_DATA, 0);
  lcd_send(LCD_DATA, 2);
  lcd_send(LCD_DATA, 5);
  lcd_send(LCD_DATA, 7);
  lcd_send(LCD_DATA, 9);
  lcd_send(LCD_DATA, 11);
  lcd_send(LCD_DATA, 14);
  lcd_send(LCD_DATA, 16);
  lcd_send(LCD_DATA, 0);
  lcd_send(LCD_DATA, 6);
  lcd_send(LCD_DATA, 11);
  lcd_send(LCD_DATA, 15);
  // Define Color Table (command 0x2D)
  // red 000 value
  // red 001 value
  // red 010 value
  // red 011 value
  // red 100 value
  // red 101 value
  // red 110 value
  // red 111 value
  // green 000 value
  // green 001 value
  // green 010 value
  // green 011 value
  // green 100 value
  // green 101 value
  // green 110 value
  // green 111 value
  // blue 000 value
  // blue 001 value
  // blue 010 value
  // blue 011 value
  #endif
  
  // Memory access controler
  lcd_send(LCD_CMD, MADCTL);
  lcd_send(LCD_DATA, (0xC8)); // 0xC0 = mirror x and y, reverse rgb
  
  // Write contrast (command 0x25)
  lcd_send(LCD_CMD, SETCON);
  lcd_send(LCD_DATA, (0x30));
  delay(2);
  
  // Display On (command 0x29)
  lcd_send(LCD_CMD, DISPON);


}

void lcd_goto_pixel(uint8_t  x, uint8_t  y)
{
    lcd_send(LCD_CMD, PASET);                             // page start/end ram
    lcd_send(LCD_DATA,x);                                     // Start Page to display to
    lcd_send(LCD_DATA,x);                                   // End Page to display to
                                                            //
    lcd_send(LCD_CMD, CASET);                             // column start/end ram
    lcd_send(LCD_DATA,y);                                     // Start Column to display to
    lcd_send(LCD_DATA,y);                                   // End Column to display to
}

void lcd_set_pixel(uint8_t  x, uint8_t  y, uint8_t  color)
{
    lcd_goto_pixel(x,y);                                            // Set position
    lcd_send(LCD_CMD, RAMWR);
    lcd_send(LCD_DATA, (uint8_t)((color >> 4) & 0xFF));
    lcd_send(LCD_DATA, (uint8_t)(((color & 0x0F) << 4) & 0xF0));
    lcd_send(LCD_CMD, NOP);
}

void lcd_fill(uint16_t color)
{
  /* Set work area */
  lcd_send(LCD_CMD, PASET);
  lcd_send(LCD_DATA, 0);
  lcd_send(LCD_DATA, 131);
  lcd_send(LCD_CMD, CASET);
  lcd_send(LCD_DATA, 0);
  lcd_send(LCD_DATA, 131);


  /* Set color */
  uint32_t i;

  lcd_send(LCD_CMD, RAMWR);
  for (i = 0; i < (132*132)/2; i ++)
  {
    lcd_send(LCD_DATA, (uint8_t)((color >> 4) & 0xFF));
    lcd_send(LCD_DATA, (uint8_t)((color & 0x0F) << 4) | ((color >> 8) & 0x0F));
    lcd_send(LCD_DATA, (uint8_t)((color) & 0xFF));
  }
}


void lcd_bitmap_rle(uint8_t* data)
{
  uint32_t i = 0, j = 0, k;
  uint8_t color = 0;
  uint8_t length = 1;

  i = 0;
  while (length != 0 && j < (132*132))
  {
    length = data[i];
    color = data[i+1];
    for (k = 0; k < length; k++)
    {
      img_buf[j++] = color;
    }
    i+=2;
  }
  
  lcd_bitmap_r132(img_buf);
}

void lcd_bitmap_r132(uint8_t *data)
{
  /* Set work area */
  lcd_send(LCD_CMD, PASET);
  lcd_send(LCD_DATA, 0);
  lcd_send(LCD_DATA, 131);
  lcd_send(LCD_CMD, CASET);
  lcd_send(LCD_DATA, 0);
  lcd_send(LCD_DATA, 131);

  uint32_t i;

  lcd_send(LCD_CMD, RAMWR);
#ifdef DEPTH_12
  for (i = 0; i < (132*132)/2*3; i++)
  {
    //lcd_send(LCD_DATA, data[i]);
    lcd_send_data(data[i]);
  }
#else
  for (i = 0; i < (132*132); i+=1)
  {
    //uint16_t c1 = data[i] << 4;
    //uint16_t c2 = data[i+1] << 4;
    
    //lcd_send_data((c1 >> 4) & 0xFF);
    //lcd_send_data(((c1 & 0x0F) << 4) | ((c2 >> 8) & 0x0F));
    //lcd_send_data(c2 & 0xFF);

    lcd_send_data(data[i]);
  }
  
#endif
}

void lcd_bitmap_r66(uint8_t* data)
{
  /* Set work area */
  lcd_send(LCD_CMD, PASET);
  lcd_send(LCD_DATA, 33);
  lcd_send(LCD_DATA, 98);
  lcd_send(LCD_CMD, CASET);
  lcd_send(LCD_DATA, 33);
  lcd_send(LCD_DATA, 98);
  uint32_t i;

  lcd_send(LCD_CMD, RAMWR);
  for (i = 0; i < (66*66)/2*3; i++)
  {
    lcd_send_data(data[i]);
  }
}













