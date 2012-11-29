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
