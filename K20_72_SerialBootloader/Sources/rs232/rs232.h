/*****************************************************************************
 * (c) Copyright 2011, Freescale Semiconductor Inc.
 * ALL RIGHTS RESERVED.
 ***************************************************************************//*!
 * @file      rs232.h
 * @author    R20253
 * @version   1.0.1.0
 * @date      May-18-2012
 * @brief     RS232 driver header file
 * @par       
 * @include   
 * @par       
 * @include         
 ******************************************************************************/
#ifndef _RS232_H
#define _RS232_H

#include "main.h"
#include "bootloader_cfg.h"

/** Bus clock frequency */
//#define BOOT_BUS_CLOCK  25e6 //TIC setting 21e6, 25e6==> Delbio board test can connect
#define BOOT_BUS_CLOCK  21e6 //TIC setting 21e6 ==> for K20 demoboard setting, can connect 
//#define BOOT_BUS_CLOCK  24000000 //TIC setting 21e6 ==> for K20 demoboard setting, can connect 

#define UART_MODULE BOOT_UART_MODULE
#define UART_BUS_CLK  BOOT_BUS_CLOCK
#define UART_BAUD_RATE  BOOT_UART_BAUD_RATE


#define UART_SBR  (unsigned char)((double)UART_BUS_CLK/(16.0*(double)UART_BAUD_RATE))

#define UART_BRFA (unsigned char)(((((double)UART_BUS_CLK/(16.0*(double)UART_BAUD_RATE))- \
                                      (double)UART_SBR)*32.0)+0.5)

//  driver macros 

//  user macro definitions
#define UART_CLK_GATE UART_CLK_GATE_REG
#define UART_PIN_RX PORTC_PCR16
#define UART_PIN_TX PORTC_PCR17

#define UART_PIN_INIT UART_PIN_RX=PORT_PCR_MUX(3);UART_PIN_TX=PORT_PCR_MUX(3);
#define UART_CLK_INIT UART_CLK_GATE=UART_MODULE_MASK;


//  Default definition of UART macros
#ifndef UART_MODULE
  #define UART_MODULE 3 
  #warning  There is missing define of UART MODULE. It sets to default 3!
#endif

#ifndef UART_BUS_CLK
  #define UART_BUS_CLK 25e6
  #warning  There is missing define of UART MODULE. It sets to default 25 MHz!
#endif 

#ifndef UART_BAUD_RATE
  #define UART_BAUD_RATE 115200
  #warning  There is missing define of UART MODULE. It sets to default 115200!
#endif

//  UART module definition
#if UART_MODULE == 0
  #define UART_BASE UART0_BASE_PTR
#elif UART_MODULE == 1 
  #define UART_BASE UART1_BASE_PTR
#elif UART_MODULE == 2
  #define UART_BASE UART2_BASE_PTR
#elif UART_MODULE == 3
  #define UART_BASE UART3_BASE_PTR
#elif UART_MODULE == 4
  #define UART_BASE UART4_BASE_PTR
#elif UART_MODULE == 5
  #define UART_BASE UART5_BASE_PTR
#endif




//  API
void UART_Initialization(void);
void UART_Deinitialization(void);
void UART_PutChar(unsigned char data);
unsigned char UART_GetChar(void);
void UART_PutString(char * str);
unsigned char UART_IsChar(void);
void UART_ClearBuff(void);

#endif
