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

// 12-bit color definitions
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

#else
#error
#endif



#else

    GPIO_ResetBits(LCD_GPIO, LCD_RST_PIN);
    delay(5);
    GPIO_SetBits(LCD_GPIO, LCD_RST_PIN);
    delay(5);

    lcd_send(LCD_CMD,DISCTL);  // display control
    lcd_send(LCD_DATA,0x00);
    lcd_send(LCD_DATA,0x20);
    lcd_send(LCD_DATA,0x00);

    lcd_send(LCD_CMD,COMSCN);        // 
    lcd_send(LCD_DATA,0x01);             // Scan 1-80
    
    lcd_send(LCD_CMD,OSCON);         //
    
    lcd_send(LCD_CMD,SLPOUT);        // Sleep out command (must be done before DISPON)
    
    lcd_send(LCD_CMD,PWRCTR);
    lcd_send(LCD_DATA,0x0f);

    lcd_send(LCD_CMD,DISINV);

    lcd_send(LCD_CMD,DATCTL);
    lcd_send(LCD_DATA,0x01);    // P1: 0x01 = page address inverted, col address normal, address scan in col direction
    lcd_send(LCD_DATA,0x00);    // P2: 0x00 = RGB sequence (default value)
    lcd_send(LCD_DATA,0x02);    // P3: 0x02 = Grayscale -> 16 (selects 12-bit color, type A)

    lcd_send(LCD_CMD,VOLCTR);
    lcd_send(LCD_DATA,32);
    lcd_send(LCD_DATA,3);

    delay(50);

    lcd_send(LCD_CMD,DISON);
#endif
/* Override the default SysTick_Config */

#if 0

static __INLINE uint32_t SysTick_Config(uint32_t ticks)

{

  if (ticks > SysTick_LOAD_RELOAD_Msk)  return (1);            /* Reload value impossible */



  SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */



  /* Enable the SysTick Interrupt at highest priority*/

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitTypeDef comm_nvic_init;

    comm_nvic_init.NVIC_IRQChannel = SysTick_IRQn;

    comm_nvic_init.NVIC_IRQChannelPreemptionPriority = 0;

    comm_nvic_init.NVIC_IRQChannelSubPriority = 0;

    comm_nvic_init.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&comm_nvic_init);

  SysTick->VAL   = 0;                                          /* Load the SysTick Counter */

  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |

                   SysTick_CTRL_TICKINT_Msk   |

                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and Sy*/

  return (0);                                                  /* Function successful */

}

#endif
