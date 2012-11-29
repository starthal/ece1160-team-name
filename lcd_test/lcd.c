#ifdef USE_MATH
  #include <math.h>
#endif
#include "stm32f4xx.h"
#include "lcd.h"
#include "fonts.h"
#include "bitmap.h"
#include "main.h"
//#include "nokia.h"
//#include "defines.h"

void lcd_gpio_init()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  
  GPIO_InitTypeDef lcd_gpio_init;
  /* Reset pin */
  lcd_gpio_init.GPIO_Pin = GPIO_Pin_11;;
  lcd_gpio_init.GPIO_Mode = GPIO_Mode_OUT;
  lcd_gpio_init.GPIO_OType = GPIO_OType_PP;
  lcd_gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
  lcd_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &lcd_gpio_init);
  
  /* SPI pins */
  lcd_gpio_init.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
  lcd_gpio_init.GPIO_Mode = GPIO_Mode_OUT;
  lcd_gpio_init.GPIO_OType = GPIO_OType_PP;
  lcd_gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
  lcd_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &lcd_gpio_init);
}

/* Sends a 9 bit command over SPI to the LCD */
void lcd_send(uint8_t is_cmd, uint8_t data)
{
  /* Enable chip */
	GPIO_ResetBits(LCD_GPIO, GPIO_LCD_SCL);
	GPIO_ResetBits(LCD_GPIO, GPIO_LCD_NSS);
  
  if (is_cmd)
    GPIO_ResetBits(LCD_GPIO, GPIO_LCD_MOSI);
  else
    GPIO_SetBits(LCD_GPIO, GPIO_LCD_MOSI);
  
  /* Clock pulse */
  GPIO_SetBits(LCD_GPIO, GPIO_LCD_SCL);
  GPIO_ResetBits(LCD_GPIO, GPIO_LCD_SCL);

  uint8_t j;
	for (j = 0; j < 8; j++)
  {
		if ((data & 0x80) == 0x80)
			GPIO_SetBits(LCD_GPIO, GPIO_LCD_MOSI);
    else 
			GPIO_ResetBits(LCD_GPIO, GPIO_LCD_MOSI);

    /* Clock pulse */
    GPIO_SetBits(LCD_GPIO, GPIO_LCD_SCL);
    GPIO_ResetBits(LCD_GPIO, GPIO_LCD_SCL);
        
    data <<= 1;
  }

  GPIO_SetBits(LCD_GPIO, GPIO_LCD_NSS);  //Disable chip

  return;
}

void lcd_init()
{
    uint8_t i;

#if (LCD_DRIVER == PHILIPS)
// Hardware reset
    GPIO_ResetBits(LCD_GPIO, GPIO_LCD_RST);
    delay(5);
    GPIO_SetBits(LCD_GPIO, GPIO_LCD_RST);
    delay(5);
// Sleep out (command 0x11)
lcd_send(LCD_CMD, SLEEPOUT);
// Inversion on (command 0x20)
lcd_send(LCD_CMD, INVON);
// Color Interface Pixel Format
lcd_send(LCD_CMD, COLMOD);
lcd_send(LCD_DATA, (0x03));
// Memory access controler
lcd_send(LCD_CMD, MADCTL);
lcd_send(LCD_DATA, (0xC8));
// seems to be required for this controller
// 0x03 = 12 bits-per-pixel
// 0xC0 = mirror x and y, reverse rgb
// Write contrast (command 0x25)
lcd_send(LCD_CMD, SETCON);
lcd_send(LCD_DATA, (0x30));
delay(2);
// contrast 0x30
// Display On (command 0x29)
lcd_send(LCD_CMD, DISPON);

#else

    lcd_gpio_init();

    GPIO_ResetBits(LCD_GPIO, GPIO_LCD_RST);
    delay(5);
    GPIO_SetBits(LCD_GPIO, GPIO_LCD_RST);
    delay(5);

    lcd_send(LCD_CMD,DISCTL);  // display control
    lcd_send(LCD_DATA,0x00);
    lcd_send(LCD_DATA,0x1f);
    lcd_send(LCD_DATA,0x0b);

    lcd_send(LCD_CMD,COMSCN);        // 
    lcd_send(LCD_DATA,0x01);             // Scan 1-80
    
    lcd_send(LCD_CMD,OSCON);         //
    
    lcd_send(LCD_CMD,SLPOUT);        // Sleep out command (must be done before DISPON)
    

    lcd_send(LCD_CMD,VOLCTR);  // electronic volume, this is kinda contrast/brightness
    lcd_send(LCD_DATA,0x5B);      // change this to get brighter or darker start point then use volup/down to adjust
    lcd_send(LCD_DATA,0x03);

    lcd_send(LCD_CMD,TMPGRD);  // Temp Gradient
    lcd_send(LCD_DATA,0x01);

 
    LCD_Set_Resolution(LOW_RES);

    lcd_send(LCD_CMD,RGBSET8);   // setup color lookup table
// Setup the color lookup table by choosing evenly spaced color values 
// for a nice even spread. Blue only has 2 bits, so the spacing is wider
// but still spans 0x00 - 0x0f
// Bit organisation is RRRGGGBB

    lcd_send(LCD_CMD,RGBSET8);         
    lcd_send(LCD_DATA,0x00);                // RED
    lcd_send(LCD_DATA,0x02);
    lcd_send(LCD_DATA,0x04);
    lcd_send(LCD_DATA,0x06);
    lcd_send(LCD_DATA,0x08);
    lcd_send(LCD_DATA,0x0a);
    lcd_send(LCD_DATA,0x0c);
    lcd_send(LCD_DATA,0x0f);
    lcd_send(LCD_DATA,0x00);                // GREEN
    lcd_send(LCD_DATA,0x02);
    lcd_send(LCD_DATA,0x04);
    lcd_send(LCD_DATA,0x06);
    lcd_send(LCD_DATA,0x08);
    lcd_send(LCD_DATA,0x0a);
    lcd_send(LCD_DATA,0x0c);
    lcd_send(LCD_DATA,0x0f);
    lcd_send(LCD_DATA,0x00);                // BLUE
    lcd_send(LCD_DATA,0x04);
    lcd_send(LCD_DATA,0x09);
    lcd_send(LCD_DATA,0x0f);

    lcd_send(LCD_CMD,NOP);                                // Data Sheet recomends you send this from time to time

    lcd_send(LCD_CMD,DISINV);

    LCD_Fill(0,0,132,132,Black);                            // clear screen
    delay(10);         // data sheet says to wait 100ms here

    lcd_send(LCD_CMD,PWRCTR);                             // power ctrl
    lcd_send(LCD_DATA,0x0f);                                  //everything on

    delay(10);         // data sheet says to wait 100ms here

    lcd_send(LCD_CMD,NOP);                                // Data Sheet recomends you send this from time to time
    lcd_send(LCD_CMD, DISON);                             // Turn Display ON

#endif

}

//*****************************************************************************
//	Name: 		LCD_Set_Resolution(char res)
// 	Copyright:	Free to use at will & at own risk.
// 	Author: 	JCP
// 	Date: 		05.30.06 17:10
// 	Description:    Sets display color resolution 
// 	Useage:		LCD_Set_Resolution(param 1)
//
//			param 1: LOW_RES, MED_RES, HIGH_RES
//
//	Example:	LCD_Set_Resolution(LOW_RES); 
//
//      NOTE:           the only function that works in med_res is ShowImage_4096()
//                      maybe a later version i will make the rest to work with 
//                      the higher color settings.
//*******************************************************************************
void LCD_Set_Resolution(char res)
{
#if (LCD_DRIVER == EPSON)
    lcd_send(LCD_CMD,DATCTL);     // datctl
    lcd_send(LCD_DATA,0x00);          // Param 1  Normal Page address, Normal column, Scan Column dir.
    lcd_send(LCD_DATA,0x04);          // param 2  Set for R-G-B order
    lcd_send(LCD_DATA,res);           // param 3  8-bit or 16 bit ?
#endif
}
//*****************************************************************************
//	Name: 		lcd_goto_pixel(unsigned char x, unsigned char y)
// 	Copyright:	Free to use at will & at own risk.
// 	Author: 	JCP
// 	Date: 		03.30.06 17:10
// 	Description:    Sets position to x-y location on LCD
// 	Useage:		lcd_goto_pixel(param 1,param 2)
//
//			param 1: x direction 0-131
//			param 2: y direction 0-131
//
//	Example:	lcd_goto_pixel(32,17);        
//					
//      Notes:		No bounds checking	
//*******************************************************************************
void lcd_goto_pixel(unsigned char x, unsigned char y)
{
  #if (LCD_DRIVER == PHILIPS)
    lcd_send(LCD_CMD, PASET);                             // page start/end ram
    lcd_send(LCD_DATA,x);                                     // Start Page to display to
    lcd_send(LCD_DATA,x);                                   // End Page to display to
                                                            //
    lcd_send(LCD_CMD, CASET);                             // column start/end ram
    lcd_send(LCD_DATA,y);                                     // Start Column to display to
    lcd_send(LCD_DATA,y);                                   // End Column to display to
  #else
    lcd_send(LCD_CMD, PASET);                             // page start/end ram
    lcd_send(LCD_DATA,y);                                     // Start Page to display to
    lcd_send(LCD_DATA,131);                                   // End Page to display to
                                                            //
    lcd_send(LCD_CMD, CASET);                             // column start/end ram
    lcd_send(LCD_DATA,x);                                     // Start Column to display to
    lcd_send(LCD_DATA,131);                                   // End Column to display to
  #endif
}
//*****************************************************************************
//	Name: 		LCD_PixelPut(unsigned char x, unsigned char y, unsigned char color)
// 	Copyright:	Free to use at will & at own risk.
// 	Author: 	JCP
// 	Date: 		03.30.06 17:10
// 	Description:    Put One Pixel on LCD
// 	Useage:		LCD_PixelPut(param 1,param 2,param 3)
//
//			param 1: x direction 0-131
//			param 2: y direction 0-131
//			param 3: Color of Pixel, RRRGGGBB format
//	Example:	LCD_PixelPut(30,30,0x62)	// put a purple pixel @ 30,30
//					
//      Notes:		No bounds checking	
//*******************************************************************************
void LCD_PixelPut(unsigned char x, unsigned char y, unsigned char color)
{
  #if (LCD_DRIVER == PHILIPS)
    lcd_goto_pixel(x,y);                                            // Set position
    lcd_send(LCD_CMD, RAMWR);
    lcd_send(LCD_DATA, (unsigned char)((color >> 4) & 0xFFFF));
    lcd_send(LCD_DATA, (unsigned char)(((color & 0x0F) << 4) | 0x00));
    lcd_send(LCD_CMD, NOP);
  #else
    lcd_goto_pixel(x,y);                                            // Set position
    lcd_send(LCD_CMD,RAMWR);                              // Now write the pixel to the display
    lcd_send(LCD_DATA,color);                                 // Write the data in the specified Color
  #endif
}
//*****************************************************************************
//	Name: 		LCD_Fill (unsigned char xs,unsigned char ys,unsigned char width,unsigned char height, unsigned char color)
// 	Copyright:	Free to use at will & at own risk.
// 	Author: 	JCP
// 	Date: 		03.30.06 17:10
// 	Description:    Fills an area starting @ xs,ys to width @ heighth with color  
// 	Useage:		LCD_Fill(param 1,param 2,param 3,param 4,param 5)
//
//			param 1: x start direction: 0-131
//			param 2: y start direction: 0-131
//			param 3: width: 0-131
//			param 4: heighth: 0-131
//			param 5: Color of Pixel, RRRGGGBB format
//
//	Example:	LCD_Fill(0,0,132,132,Black);  // fill entire screen with black color
//					
//      Notes:		No bounds checking	
//*******************************************************************************
void LCD_Fill (unsigned char xs,unsigned char ys,unsigned char width,unsigned char height, unsigned char color)
{
    unsigned char i,j;

    for (i=0;i < height;i++)                        // Number of horizontal lines
    {                                               //
	lcd_goto_pixel(xs,ys+i);                            // Goto start of fill area (Top Left)
	lcd_send(LCD_CMD,RAMWR);                  // Write to display
                                                    //
     	for (j=0;j < width;j++)                     // pixels per line
	{                                           //
            lcd_send(LCD_DATA,color);                 //
	}                                           //
                                                    //
        lcd_send(LCD_CMD,NOP);                    // End 
    }
}
///******************************************************************************
//      Name: 		LCD_Funct_A(x1, y1, x2, y2, pixel_on_off, ram_or_lcd)
// 	Copyright:	Free to use at will & at own risk.
// 	Author: 	JCP
// 	Date: 		05.30.06 17:10
//      Description:    Used with LCD line function
//*******************************************************************************
void LCD_Funct_A(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color )
{
    float a,b;
    int x,y;                                              //
                                                                    //
    b = (float)((y2 - y1) + 1) / (float)((x2-x1) + 1);              //
                                                                    //
    a = y1;                                                         //
                                                                    //
    for(x = x1;x <= x2; x++)                                        //
    {                                                               //
                                                                    //
    	for(y=(unsigned char)a;y <= (unsigned char)(a + b); y++)    //
    	{                                                           //
            if(y <= y2)                                             //
            {                                                       //
		lcd_goto_pixel(x,y);                                        // Goto start of fill area (Top Left) 
		lcd_send(LCD_CMD,RAMWR);                                  // write to display 
		lcd_send(LCD_DATA,color);                                     //
            }                                                       //
        }                                                           //
                                                                    //
        a+=b;                                                       //
        
        lcd_send(LCD_CMD,NOP);                                    // Data Sheet recomends you send this from time to time
    }                                                               //
}
//******************************************************************************
//  Name:           LCD_Funct_B(x1, y1, x2, y2, pixel_on_off, ram_or_lcd)
//  Copyright:      Free to use at will & at own risk.
//  Author:         JCP
//  Date:           05.30.06 17:10
//  Description:    Used with LCD line function
//*******************************************************************************
void LCD_Funct_B(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color )
{
    float a,b;
    int x,y;                                                        //
                                                                    //
    b = (float)((y2 - y1) + 1) / (float)((x1-x2) + 1);              //
                                                                    //
    a = y1;                                                         //
                                                                    //
    for(x = x1;x >= x2; x--)                                        //
    {                                                               //
    	for(y=(unsigned char)a;y <= (unsigned char)(a + b); y++)    //
    	{                                                           //
            if(y <= y2)                                             //
            {                                                       //
		lcd_goto_pixel(x,y);                                        // Goto start of fill area (Top Left) 
		lcd_send(LCD_CMD,RAMWR);                          // write to display 
		lcd_send(LCD_DATA,color);                             //
            }                                                       //
        }                                                           //
                                                                    //
        a+=b;                                                       //
                                                                    //
        lcd_send(LCD_CMD,NOP);                                    // Data Sheet recomends you send this from time to time    
    }                                                               //
}
//******************************************************************************
//  Name:           LCD_Line(x1, y1, x2, y2, pixel_on_off, ram_or_lcd)
//  Copyright:      Free to use at will & at own risk.
//  Author:         JCP
//  Date:           05.30.06 17:10
//  Description:    To draw or remove a line from the LCD
//  Notes:          No bounds checking	
//*******************************************************************************
void LCD_Line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color)
{

   	if( (x1 <= x2) && (y1 <= y2) )				//
   	{							//
            LCD_Funct_A(x1,y1,x2,y2,color);			//
	}							//
								//
   	else if( (x1 > x2)  && (y1 < y2) )			//
   	{							//
            LCD_Funct_B(x1,y1,x2,y2,color);			//
	}							//
								//
	else if( (x1 >= x2) && (y1 >= y2) )			//
	{							//
            LCD_Funct_A(x2,y2,x1,y1,color);			//
	}							//
								//
	else if( (x1 < x2) && (y1 > y2) )			//
	{							//
            LCD_Funct_B(x2,y2,x1,y1,color);			//
	}							//
								//
}
//******************************************************************************
//  Name:           LCD_Box(x1, y1, x2, y2, pixel_on_off, ram_or_lcd)
//  Copyright:      Free to use at will & at own risk.
//  Author:         JCP
//  Date:           03.30.06 17:10
//  Description:    To draw a BOX on the LCD
//  Notes:          No bounds checking	
//*******************************************************************************
void LCD_Box(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char color)
{
    unsigned char x,y;

    x = x1;						//
    y = y1;						//
							//
    while(x <= x2)					//
    {							//
       	LCD_PixelPut(x,y1,color);			//
       	LCD_PixelPut(x,y2,color);			//
							//
	x++;						//
    }							//
							//
    while(y <= y2)					//
    {							//
       	LCD_PixelPut(x1,y,color);			//
       	LCD_PixelPut(x2,y,color);			//
							//
	y++;						//
    }							//
}
//******************************************************************************
// 	Name:      	LCD_Circle
// 	Copyright: 	Free to use at will & at own risk.
// 	Date: 		03.30.06 17:10
// 	Description:    To Draw a Circle on LCD
// 	Useage:		LCD_Circle(param 1,param 2,param 3,param 4)
//
//			param 1: x location on screen, 0 - 127
//			param 2: y location on screen, 0 - 63
//			param 3: diameter of circle
//                      param 4: Color of Pixel, RRRGGGBB format
//
//	Example:        LCD_Circle(40,30,30,LightGreen);
//      Notes:		No bounds checking	
//*******************************************************************************
void LCD_Circle(unsigned char center_x, unsigned char center_y, unsigned int rad, unsigned char color)
{
    unsigned int x;
    unsigned int y;

    x = 0;
    y = rad;

    while (x <= y)
    {
        // We make use of 8 axes of symmetry in a circle.
        // This way we have fewer points to calculate on its circumference.
        LCD_PixelPut (center_x + (unsigned char)x, center_y + (unsigned char)y,color);
        LCD_PixelPut (center_x - (unsigned char)x, center_y + (unsigned char)y,color);
        LCD_PixelPut (center_x + (unsigned char)x, center_y - (unsigned char)y,color);
        LCD_PixelPut (center_x - (unsigned char)x, center_y - (unsigned char)y,color);
        LCD_PixelPut (center_x + (unsigned char)y, center_y + (unsigned char)x,color);
        LCD_PixelPut (center_x - (unsigned char)y, center_y + (unsigned char)x,color);
        LCD_PixelPut (center_x + (unsigned char)y, center_y - (unsigned char)x,color);
        LCD_PixelPut (center_x - (unsigned char)y, center_y - (unsigned char)x,color);

        // This is the most important part of the function.
        // We go to the right in all cases (x++).
        // We need to decide whether to go down (y--).
        // This depends on which point is
        // closest to the path of the circle.
        // Good old Pythagoras will tell us what to do.
        x++;

        if( abs( (x * x) + (y * y) - (rad * rad) ) > abs( (x * x) + ((y-1)*(y-1)) - (rad * rad)) )
        {
            y--;
	}
    }
}
//******************************************************************************
//      Name:      	LCD_Thick_Circle
// 	Copyright: 	Free to use at will & at own risk.
//      Date:           03.30.06 17:10
// 	Description:    To Draw a Definable Thickness Circle on LCD
// 	Useage:		LCD_Thick_Circle(param 1,param 2,param 3,param 4,param 5)
//
//			param 1: x start direction: 0-131
//			param 2: y start direction: 0-131
//                      param 3: diameter: 0-255
//			param 4: Thickness of circle wall in pixels
//			param 5: Color of Pixel, RRRGGGBB format
//
//	Example:       	LCD_Thick_Circle(66,66,30,2,0x62);		
// 			start @ 66,66, with a diamater of 30, 2 pixels thick in purple
//
//      Notes:		No bounds checking	
//*******************************************************************************
void LCD_Thick_Circle(unsigned char center_x, unsigned char center_y,unsigned char diameter, unsigned char thick,unsigned char color)
{
    unsigned int a;

    for(a=0;a < 360;a++)
    {
 	LCD_Circle_Line(center_x,center_y,diameter,(diameter + thick),a,color);
    } 	
}
//******************************************************************************
//  Name:           LCD_Circle_Line
//  Copyright:      Free to use at will & at own risk.
//  Date:           03.30.06 17:10
//  Description:    To Draw a line in a circle, using degrees I.E. like a radar screen
//  Useage:         LCD_Circle_Line(param 1,param 2,param 3,param 4,param 5,param 6)
//                  param 1: x start direction: 0-131
//                  param 2: y start direction: 0-131
//                  param 3: End point of line (usually the size diamater of circle - thickness)
//                  param 4: Start point of line (usually 0, but can go up to diameter)
//                  param 5: angle of line on circles outer rim
//                  param 6: Color of Pixel, RRRGGGBB format
//
//  Example:        LCD_Thick_Circle(66,66,30,2,0x62);          // draw a circle first
//                  LCD_Circle_Line(66,66,28,0,90,LightGreen);
//                  circle center point @ 66,66, with a line length of 28 from center
//                  line start @ 0 or center of circle point 90 deg outward with the color light green
//
//  Notes:          No bounds checking  
//                  degrees_to_radians(x) = ((x) * pi / 180)
//                  radians_to_degrees(x) = ((x) * 180 / pi)
//*******************************************************************************
void LCD_Circle_Line(unsigned char center_x, unsigned char center_y,unsigned char diameter, unsigned char length, float angle,unsigned char lcolor)
{
    #ifdef USE_MATH
    unsigned char x,y,x1,y1;						//
                                                                        //
    angle -=90;								// remove lor lanscape mode
									//
    angle *= 0.01745329;						// degrees_to_radians(x) = ((x) * pi / 180)
									//
    x = (char)(length * cos(angle) + center_x);				// figure the center start point of the line
    y = (char)(length * sin(angle) + center_y);				// figure the center start point of the line
									//
    x1 = (char)(diameter * cos(angle) + center_x);			// figure the end point of the line
    y1 = (char)(diameter * sin(angle) + center_y);			// figure the end point of the line
									//
    LCD_Line(x, y, x1, y1, lcolor );					//
    #endif
}
//******************************************************************************
//  Name:           LCD_String
//  Copyright:      Free to use at will & at own risk.
//  Date:           03.30.06 17:10
//  Description:    To print a string of characters to LCD
//  Useage:         LCD_String(param 1,param 2,param 3,param 4,param 5,param 6)
//
//                  param 1: String data
//                  param 2: pointer to data lookup table (I.E. Font table)
//                  param 3: x start direction: 0-131
//                  param 4: y start direction: 0-131
//                  param 5: Font Color, RRRGGGBB format
//                  param 5: Background color, RRRGGGBB format
//
//  Example:        sprintf(thestring,"Hello World"); 
//                  LCD_String(thestring,FONT6x8,52,25,White,Black);
//          
//                  print "Hello World" in white text on a black background
//
//  Notes:          the first line in the font table must contain 3 variables in this order
//                  Loc[0][0] = font x width
//                  Loc[0][1] = font y height
//                  Loc[0][2] = total data bytes per individual font
//
//                  No bounds checking  
//*******************************************************************************
void LCD_String (char *lcd_string, const char *font_style,unsigned char x, unsigned char y, unsigned char fcolor, unsigned char bcolor)
{
    unsigned int  i,j;                                          //
    unsigned char mask,z,xme,yme,offset;			//
    const char *data;						//
								//
    data = font_style;						// point to the start of the font table
								//
    xme = *data;						// get font x width
    data++;							//
    yme = *data;						// get font y length
    data++;							//
    offset = *data;						// get data bytes per font
								//
    do								//
    {								//
								//
        data =  (font_style + offset) +                         // point to data in table to be loaded
                (offset * (int)(*lcd_string - 32));             //
								//
        for (i=0;i < yme;i++)                                   // 
        {                                                       //
            mask |=0x80;					//
        							//
            for (j=x;j < (x + xme);j++)				//
            {							//
                z= y + i;					//
								//
                if (*data & mask)				// if pixel data then put dot
                {						//
                    LCD_PixelPut (j,z,fcolor);			//
                }						//
                else						// else use background color
                {						//
                    LCD_PixelPut (j,z,bcolor);		   	// 
                }					 	//
								//
                mask>>=1;					//
            }							//
                                                                //
            data++;						//
        }							//
								//
        x+=xme;							//
								//
	lcd_string++;						// next character in string
                                                                //
  	lcd_send(LCD_CMD,NOP);					// Its Recomended to send now and then
								//
	}while(*lcd_string !='\0');                            	// keep spitting chars out until end of string
}
//******************************************************************************
// 	Name:      	ShowImage 4096 color mode ONLY!!!
// 	Copyright: 	Free to use at will & at own risk.
// 	Date: 		05.30.06 17:10
// 	Description:    To Display a jpg data color image on lcd
// 	Useage:		ShowImage(param 1,param 2,param 3)
//
//			param 1: x location on screen 
//			param 2: y location on screen
//                      param 3: pointer to starting position of data 
//
//	Example:        ShowImage(0,0,My_Photo);
//      Notes:		No bounds checking	
//*******************************************************************************
void ShowImage_4096(unsigned char x,unsigned char y, const unsigned char  *location)
{
    unsigned char x_max,y_max;
    unsigned char x_logo,y_logo;

    LCD_Set_Resolution(HIGH_RES);                                // Dont forget to switch back to low_res when done!
                                                                //
    x_max =  *location / 2;                                     //
    location++;                                                 //
    y_max = *location;                                          //
    location++;                                                 //
                                                                //
    for(y_logo=0;y_logo <y_max;y_logo++)                        //
    {                                                           //
        lcd_goto_pixel(x,y + y_logo);                                   //
        lcd_send(LCD_CMD,RAMWR);                              // Now write the pixel to the display
                                                                //
        for(x_logo=0;x_logo < x_max;x_logo++)                   //
        {                                                       //
            lcd_send(LCD_DATA,*location);                         // Write the data in the specified Color
            location++;                                         //
            lcd_send(LCD_DATA,*location);                         // Write the data in the specified Color
            location++;                                         //
            lcd_send(LCD_DATA,*location);                         // Write the data in the specified Color
            location++;                                         //
        }                                                       //
    }                                                           //
}                                                               //
//******************************************************************************
// 	Name:      	ShowImage
// 	Copyright: 	Free to use at will & at own risk.
// 	Date: 		05.30.06 17:10
// 	Description:    To Display a jpg data color image on lcd
// 	Useage:		ShowImage(param 1,param 2,param 3)
//
//			param 1: x location on screen
//			param 2: y location on screen
//                      param 3: pointer to starting position of data 
//
//	Example:        ShowImage(0,0,My_Photo);
//      Notes:		No bounds checking	
//*******************************************************************************
void ShowImage(unsigned char x,unsigned char y, const unsigned char  *location)
{
    unsigned char x_max,y_max;
    unsigned char x_logo,y_logo;

    x_max =  *location;                                         //
    location++;                                                 //
    y_max = *location;                                          //
    location++;                                                 //
                                                                //
    for(y_logo=0;y_logo <y_max;y_logo++)                        //
    {                                                           //
        lcd_goto_pixel(x,y + y_logo);                                   //
        lcd_send(LCD_CMD,RAMWR);                              // Now write the pixel to the display
                                                                //
        for(x_logo=0;x_logo < x_max;x_logo++)                   //
        {                                                       //
            lcd_send(LCD_DATA,*location);                         // Write the data in the specified Color
            location++;                                         //
        }                                                       //
    }                                                           //
}                                                               //
















