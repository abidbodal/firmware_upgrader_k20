/******************************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved                       
*
*******************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************//*!
*
* @file      kinetis_params.h
*
* @author    b01119
* 
* @version   0.0.3.0
* 
* @date      Jul-25-2012
* 
* @brief     Main configuration file for IIC bootloader
*
******************************************************************************/

#ifndef KINETIS_PARAMS_H
#define KINETIS_PARAMS_H

//#include "MK60D10.h"
#include "MK20D7.h"

//  Kinetis ARM Cortex-M4 Microcontrollers mask sets
#define m_0M33Z 	0x00
#define m_1N30D 	0x01 
#define m_2N30D 	0x02 
#define m_3N30D 	0x03
#define m_4N30D 	0x04

//  Kinetis ARM Cortex-M4 Microcontrollers models macros
#define K10 0x00
#define K20 0x01
#define K30 0x02
#define K40 0x03
#define K50 0x04
#define K60 0x05
#define K70 0x06

//  Kinetis Flash memory sizes
#define FLASH_32K   32
#define FLASH_64K   64
#define FLASH_128K  128
#define FLASH_256K  256
#define FLASH_512K  512
#define FLASH_1024K 1024
 
//  Port definitions
#define A   1
#define B   2
#define C   3
#define D   4
#define E   5

#include "bootloader_cfg.h"

//  Kinetis flash memory macros
#ifndef KINETIS_MODEL
  #define KINETIS_MODEL K60
  #warning There is missing define of Kinetis model. It sets to default: 'K60'!
#endif

#if (KINETIS_MODEL == K70)
 // #error Flash driver for Kinetis K70 must be updated !
#endif


//  Kinetis memory sizes
#ifndef KINETIS_FLASH
  #define KINETIS_FLASH FLASH_512K
  #warning There is missing define of Kinetis flash memory. It sets to default: '512k'! 
#endif

#ifndef BOOTLOADER_FLASH_PROTECTION
  #define BOOTLOADER_FLASH_PROTECTION 1
  #warning There is missing define of Kinetis flash memory protection option. It sets to default: Enabled! 
#endif


//  size of one protection section depends on flash size in Bytes (protected block = flash size / 32)
#define FLASH_PROT_SECTION ((KINETIS_FLASH/32) * 1024)


//  Generation of value for FPROT register (protection of bootloader)
#if BOOTLOADER_FLASH_PROTECTION != 0
  #define FLASH_BOOT_PROTECTION (0xFFFFFFFE << (((BOOTLOADER_SIZE * 1024) / FLASH_PROT_SECTION)))
#else
  #define FLASH_BOOT_PROTECTION 0xFFFFFFFF
#endif

#ifndef KINETIS_MODEL_STR
  
#if KINETIS_MODEL == K10
    #define KINETIS_MODEL_STR "K10"
  #elif KINETIS_MODEL == K20
    #define KINETIS_MODEL_STR "K20"
  #elif KINETIS_MODEL == K30
    #define KINETIS_MODEL_STR "K30"
  #elif KINETIS_MODEL == K40
    #define KINETIS_MODEL_STR "K40"
  #elif KINETIS_MODEL == K50
    #define KINETIS_MODEL_STR "K50"
  #elif KINETIS_MODEL == K60
    #define KINETIS_MODEL_STR "K60"
  #elif KINETIS_MODEL == K70
    #define KINETIS_MODEL_STR "K70"  
  #else  
    #define KINETIS_MODEL_STR "Unknown"
  #endif
#endif


//  revisions : m_0M33Z, m_1N30D, m_2N30D, m_3N30D , m_4N30D   
#ifndef KINETIS_REVISION_ID 
  #define KINETIS_REVISION_ID 2N30D
  #warning There is missing define of Kinetis revision id. It sets to default: '2N30D'!
#endif

#ifdef  KINETIS_FLASH
    /** Flash block count of this MCU */
    #ifndef FLASH_BLOCK_CNT
      #define FLASH_BLOCK_CNT 1
    #endif

    #ifndef INTERRUPT_VECTORS
      #define INTERRUPT_VECTORS 0x0000
    #endif
  
    #ifndef RELOCATED_VECTORS
      #define RELOCATED_VECTORS FLASH_PROT_SECTION 
    #endif
  
    #ifndef USER_FLASH_START
      #define USER_FLASH_START FLASH_PROT_SECTION + 0x400 // + vector table
    #endif
  
    #ifndef USER_FLASH_END  
      #define USER_FLASH_END ((FLASH_PROT_SECTION*32) - 1)
    #endif
  
    #ifndef FLASH_WRITE_PAGE  
      #define FLASH_WRITE_PAGE 128
    #endif
  
    #ifndef FLASH_ERASE_PAGE  
      #define FLASH_ERASE_PAGE 2048
    #endif

#endif 

//  default values of GPIO Alternatives of the port  (Alternative to UART)
#ifndef BOOT_UART_GPIO_PORT
  #define BOOT_UART_GPIO_PORT   PORTC_BASE_PTR
  #warning  There is missing define of Kinetis GPIO port. It sets to default: 'PORTC_BASE_PTR'!
#endif

#ifndef BOOT_UART_GPIO_PIN_RX
  #define BOOT_UART_GPIO_PIN_RX 16
  #warning  There is missing define of Kinetis GPIO pin for UART Rx. It sets to default: 'PTC16'!
#endif

#ifndef BOOT_UART_GPIO_PIN_TX
  #define BOOT_UART_GPIO_PIN_TX 17
  #warning  There is missing define of Kinetis GPIO pin for UART Tx. It sets to default: 'PTC17'!
#endif

#define GPIO_OUT_MASK   (0x01 << BOOT_UART_GPIO_PIN_RX) 

//  PIN alternative definition
#ifndef BOOT_PIN_UART_ALTERNATIVE
  #define BOOT_PIN_UART_ALTERNATIVE 3 
  #warning There is missing define of Kinetis pin BOOT_PIN_UART_ALTERNATIVE for PORT_PCR_MUX. It sets to default: '3'!
#endif

//  PIN alternative definition
#ifndef BOOT_PIN_GPIO_ALTERNATIVE
  #define BOOT_PIN_GPIO_ALTERNATIVE 1 
  #warning There is missing define of Kinetis pin PIN_ALTERNATIVE for BOOT_PIN_GPIO_ALTERNATIVE. It sets to default: '1'!
#endif

//  init macro for multiplexer setting - alternative UART
#define BOOT_PIN_INIT_AS_UART {PORT_PCR_REG(BOOT_UART_GPIO_PORT, BOOT_UART_GPIO_PIN_RX) = PORT_PCR_MUX(BOOT_PIN_UART_ALTERNATIVE); PORT_PCR_REG(BOOT_UART_GPIO_PORT, BOOT_UART_GPIO_PIN_TX) = PORT_PCR_MUX(BOOT_PIN_UART_ALTERNATIVE);}
//  init macro for multiplexer setting - alternative GPIO
#define BOOT_PIN_INIT_AS_GPIO {PORT_PCR_REG(BOOT_UART_GPIO_PORT, BOOT_UART_GPIO_PIN_RX) = PORT_PCR_MUX(BOOT_PIN_GPIO_ALTERNATIVE);}

//  Trim auto calibration 
//  Trim register
#define TRIM_REG  MCG_C3
#define TRIM_REG2  MCG_C4

#define CALIB_SMALL_STEP  2 //4
#define CALIB_BIG_STEP    8 //15
#define CALIB_PULSE_LENGTH  (BOOT_BUS_CLOCK / (BOOT_UART_BAUD_RATE / 10))
#define CALIB_DIFFERENCE 2000

#if KINETIS_REVISION_ID == m_0M33Z 
  #define CALIB_LIMIT_HI  (CALIB_PULSE_LEN * 1.8) // +80%
#elif (KINETIS_REVISION_ID == m_1N30D || KINETIS_REVISION_ID == m_2N30D || KINETIS_REVISION_ID == m_3N30D || KINETIS_REVISION_ID == m_4N30D) 
  #define CALIB_LIMIT_HI  (CALIB_PULSE_LEN * 1.6) //  +60%
#endif   

//  minimal possible BAUDRATE 
#if BOOT_UART_BAUD_RATE < (CALIB_BASE_BAUDRATE * 64) 
  #error  Baudrate setting is out of range 'CALIB_BASE_BAUDRATE' !!
#endif



#endif //KINETIS_PARAMS_H
