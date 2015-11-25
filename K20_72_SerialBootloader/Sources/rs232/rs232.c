/*****************************************************************************
 * (c) Copyright 2011, Freescale Semiconductor Inc.
 * ALL RIGHTS RESERVED.
 ***************************************************************************//*!
 * @file      rs232.c
 * @author    R20253
 * @version   1.0.1.0
 * @date      May-18-2012
 * @brief     RS232 driver
 * @par       
 * @include   
 * @par       
 * @include         
 ******************************************************************************/
#include "rs232.h"
#include "wdg.h"

/**************************************************************//*!
* UART Initialization  
******************************************************************/
void UART_Initialization(void)
{
  UART_BDH_REG(UART_BASE)     = ((UART_SBR>>8)&0x1f);
  UART_BDL_REG(UART_BASE)     = (UART_SBR&0xff);
  UART_C1_REG(UART_BASE)      = 0;
  UART_C2_REG(UART_BASE)      = UART_C2_TE_MASK|UART_C2_RE_MASK;
  UART_S2_REG(UART_BASE)      = 0;
  UART_C3_REG(UART_BASE)      = 0;
  UART_MA1_REG(UART_BASE)     = 0;
  UART_MA2_REG(UART_BASE)     = 0;
  UART_C4_REG(UART_BASE)      = (UART_BRFA&0x1f);
  UART_MODEM_REG(UART_BASE)   = 0;
  UART_IR_REG(UART_BASE)      = 0;
  UART_PFIFO_REG(UART_BASE)   = 0;
  UART_CFIFO_REG(UART_BASE)   = 0;
  UART_SFIFO_REG(UART_BASE)   = 0;
  UART_TWFIFO_REG(UART_BASE) |= 0;
  UART_RWFIFO_REG(UART_BASE) |= 1;  
 
  while(UART_IsChar())
    (void)UART_GetChar();
}


/**************************************************************//*!
* Function for deinitialization  
******************************************************************/
void UART_Deinitialization(void)
{
  UART_C2_REG(UART_BASE) = 0;
}

/**************************************************************//*!
* Function for sending one character   
******************************************************************/
void UART_PutChar(unsigned char data)
{
  while((UART_S1_REG(UART_BASE)&UART_S1_TC_MASK) == 0){};
  UART_D_REG(UART_BASE) = data;
}

/**************************************************************//*!
* Function for checking the input buffer
******************************************************************/
unsigned char UART_IsChar(void)
{
  if(UART_S1_REG(UART_BASE) & UART_S1_RDRF_MASK)
    return 1;
  
  return 0;
}

/**************************************************************//*!
* Function for receiving of one character  
******************************************************************/
unsigned char UART_GetChar(void)
{
  while(UART_IsChar() == 0){

#if BOOTLOADER_INT_WATCHDOG == 1
    WDG_Refresh(); /* feeds the dog */
#endif
		
  };
  
  return UART_D_REG(UART_BASE);
}



/**************************************************************//*!
* Function for sending string 
******************************************************************/
void UART_PutString(char * str) 
{
  do 
  {
    UART_PutChar((unsigned char)*str);
  }while(*str++);
    
}

/**************************************************************//*!
* Function for clearing of data buffer  
******************************************************************/
void UART_ClearBuff(void)
{
  UART_D_REG(UART_BASE) = 0;
}
