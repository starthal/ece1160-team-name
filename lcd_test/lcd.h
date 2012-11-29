#include "stm32f4xx.h"
//#include "nokia.h"

//#define USE_MATH

#define PHILIPS 0
#define EPSON 1
#define LCD_DRIVER EPSON
//#define LCD_DRIVER PHILIPS

#define LCD_GPIO      GPIOB
#define GPIO_LCD_NSS  GPIO_Pin_12
#define GPIO_LCD_SCL  GPIO_Pin_13
#define GPIO_LCD_MOSI GPIO_Pin_15
#define GPIO_LCD_RST  GPIO_Pin_11

#define LCD_CMD      0
#define LCD_DATA     1

#define LOW_RES         0x01                            // 8-bit R-G-B
#define HIGH_RES        0x02                            // 16-bit R-G-B 



#if (LCD_DRIVER == PHILIPS)
  // Philips PCF8833 LCD controller command codes
  #define NOP 0x00 // nop
  #define SWRESET 0x01 // software reset
  #define BSTROFF 0x02 // booster voltage OFF
  #define BSTRON 0x03 // booster voltage ON
  #define RDDIDIF 0x04 // read display identification
  #define RDDST 0x09 // read display status
  #define SLEEPIN 0x10 // sleep in
  #define SLEEPOUT 0x11 // sleep out
  #define PTLON 0x12 // partial display mode
  #define NORON 0x13 // display normal mode
  #define INVOFF 0x20 // inversion OFF
  #define INVON 0x21 // inversion ON
  #define DALO 0x22 // all pixel OFF
  #define DAL 0x23 // all pixel ON
  #define SETCON 0x25 // write contrast
  #define DISPOFF 0x28 // display OFF
  #define DISPON 0x29 // display ON
  #define CASET 0x2A // column address set
  #define PASET 0x2B // page address set
  #define RAMWR 0x2C // memory write
  #define RGBSET 0x2D // colour set
  #define PTLAR 0x30 // partial area
  #define VSCRDEF 0x33 // vertical scrolling definition
  #define TEOFF 0x34 // test mode
  #define TEON 0x35 // test mode
  #define MADCTL 0x36 // memory access control
  #define SEP 0x37 // vertical scrolling start address
  #define IDMOFF 0x38 // idle mode OFF
  #define IDMON 0x39 // idle mode ON
  #define COLMOD 0x3A // interface pixel format
  #define SETVOP 0xB0 // set Vop
  #define BRS 0xB4 // bottom row swap
  #define TRS 0xB6 // top row swap
  #define DISCTR 0xB9 // display control
  #define DOR 0xBA // data order
  #define TCDFE 0xBD // enable/disable DF temperature compensation
  #define TCVOPE 0xBF // enable/disable Vop temp comp
  #define EC 0xC0 // internal or external oscillator
  #define SETMUL 0xC2 // set multiplication factor
  #define TCVOPAB 0xC3 // set TCVOP slopes A and B
  #define TCVOPCD 0xC4 // set TCVOP slopes c and d
  #define TCDF 0xC5 // set divider frequency
  #define DF8COLOR 0xC6 // set divider frequency 8-color mode
  #define SETBS 0xC7 // set bias system
  #define RDTEMP 0xC8 // temperature read back
  #define NLI 0xC9 // n-line inversion#define RDID1
  #define RDID1 0xDA
  #define RDID2 0xDB
  #define RDID3 0xDC // read ID3

  /* Colors */
  #define WHITE 0xFFF
#define BLACK 0x000
#define RED 0xF00
#define GREEN 0x0F0
#define BLUE 0x00F
#define CYAN 0x0FF
#define MAGENTA 0xF0F
#define YELLOW 0xFF0
#define BROWN 0xB22
#define ORANGE 0xFA0
#define PINK 0xF6A
  
#elif (LCD_DRIVER == EPSON)
  #define DISON       0xaf   
  #define DISOFF      0xae   
  #define DISNOR      0xa6  
  #define DISINV      0xa7  
  #define COMSCN      0xbb   
  #define DISCTL      0xca   
  #define SLPIN       0x95   
  #define SLPOUT      0x94   
  #define PASET       0x75   
  #define CASET       0x15   
  #define DATCTL      0xbc   
  #define RGBSET8     0xce   
  #define RAMWR       0x5c   
  #define RAMRD       0x5d   
  #define PTLIN       0xa8   
  #define PTLOUT      0xa9   
  #define RMWIN       0xe0   
  #define RMWOUT      0xee   
  #define ASCSET      0xaa   
  #define SCSTART     0xab   
  #define OSCON       0xd1   
  #define OSCOFF      0xd2   
  #define PWRCTR      0x20   
  #define VOLCTR      0x81   
  #define VOLUP       0xd6   
  #define VOLDOWN     0xd7   
  #define TMPGRD      0x82   
  #define EPCTIN      0xcd   
  #define EPCOUT      0xcc   
  #define EPMWR       0xfc   
  #define EPMRD       0xfd   
  #define EPSRRD1     0x7c   
  #define EPSRRD2     0x7d   
  #define NOP         0x25

// 8bpp Color Mode - Some basic colors defined for ease of use
// remember 8bpp color = 3xRed, 3xGreen & 2xBlue bits
// organised as RRRGGGBB

#define DarkBlue    0x02
#define LightBlue   0x03
#define DarkGreen   0x14
#define LightGreen  0x1c
#define DarkRed     0x60
#define RED    0xe0
#define White       0xff
#define Black       0x00
#define Yellow      0xdd 
#define Purple      0x62
#else
#error
#endif

void lcd_init();
void lcd_goto_pixel(unsigned char x, unsigned char y);
void LCD_PixelPut(unsigned char x, unsigned char y, unsigned char color);
void LCD_Fill (unsigned char xs,unsigned char ys,unsigned char width,unsigned char height, unsigned char color);
void LCD_Box(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color);
void Character (unsigned char Size, unsigned char x, unsigned char y, unsigned char _char, unsigned char color);
void LCD_Line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color);
void LCD_Funct_B(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color );
void LCD_Funct_A(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color );
void LCD_Circle(unsigned char center_x, unsigned char center_y, unsigned int rad, unsigned char color);
void LCD_Thick_Circle(unsigned char center_x, unsigned char center_y,unsigned char diameter, unsigned char thick,unsigned char color);
void LCD_Circle_Line(unsigned char center_x, unsigned char center_y,unsigned char diameter, unsigned char length, float angle,unsigned char lcolor);
void ShowImage(unsigned char x, unsigned char y, const unsigned char *location);
void ShowImage_4096(unsigned char x,unsigned char y, const unsigned char  *location);
void LCD_String (char *lcd_string, const char *font_style,unsigned char x, unsigned char y, unsigned char fcolor, unsigned char bcolor);
void LCD_Set_Resolution(char res);





