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
* @file      bootloader_cfg.h
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

#ifndef BOOTLOADER_CFG_H
#define BOOTLOADER_CFG_H



/**************************************************/
/** USER SETTINGS OF KINETIS MCU */
/**  Kinetis ARM Cortex-M4 model */
#define KINETIS_MODEL K20

#define KINETIS_REVISION_ID m_4N30D

/** Kinetis Flash memory size */
#define KINETIS_FLASH FLASH_256K

/** Bootloader protocol version */
#define BOOTLOADER_PROTOCOL_VERSION VERSION_KINETIS

/** Bootloader flash protection */
#define BOOTLOADER_FLASH_PROTECTION 0

/**  Flash write access */
#define FLASH_WRITE_ACCESS FLASH_WRITE_ACCESS_PHRASES

/**************************************************/
/* Actual used UART module */
#define BOOT_UART_MODULE  1

/* Actual used UART module */
/* A range of UART baudrates is (9600 - 115200) */
#define BOOT_UART_BAUD_RATE  115200 

/** GPIO & UART pins initialization */

#define BOOT_UART_GPIO_PORT PORTE_BASE_PTR

/*  setting of multiplexer for UART alternative of pin */
#define BOOT_PIN_UART_ALTERNATIVE 3

/*  setting of multiplexer for GPIO alternative of pin */
#define BOOT_PIN_GPIO_ALTERNATIVE 1

#define BOOT_UART_GPIO_PIN_RX 1  

#define BOOT_UART_GPIO_PIN_TX 0  

/**************************************************/
/** BOOTLOADER FEATURES SETTINGS */
#define BOOTLOADER_ENABLE_READ_CMD 1

#define BOOTLOADER_INT_WATCHDOG 0

#define BOOTLOADER_ENABLE_VERIFY 1

#define BOOTLOADER_CRC_ENABLE 0

#define BOOTLOADER_AUTO_TRIMMING 0 

/**************************************************/
/** CALIBRATION OF BOOTLOADER TRIM SETTINGS */
#define BOOT_CALIBRATION_TIMER  FTM0_BASE_PTR
#define BOOT_CALIBRATION_GPIO_PORT  PTE_BASE_PTR
/******************************************************************************
*
*
*     Advanced detail settings for non standard settings
*
*
******************************************************************************/


/* Flash block count of this MCU */
//#define FLASH_BLOCK_CNT 1

/* Start address of interrupt vector table */ 
//#define INTERRUPT_VECTORS 0x0000

/* Start address of relocated interrutp vector table */
//#define RELOCATED_VECTORS 0x4000 

/* Flash start address */
//#define USER_FLASH_START RELOCATED_VECTORS + 1024 // + vector table

/* Flash end address */
//#define USER_FLASH_END 0x0003FFFF

/* Size of write block */
//#define FLASH_WRITE_PAGE 128

/* Size of erase block */
//#define FLASH_ERASE_PAGE 2048

/* Maximal length of ID_STRING */
//#define ID_STRING_MAX 5

/* Description string */
//#define KINETIS_MODEL_STR "K53"

#endif //BOOTLOADER_CFG_H
