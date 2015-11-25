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
* @file      crc.h
*
* @author    b01119
* 
* @version   0.0.1.0
* 
* @date      May-18-2012
* 
* @brief     CRC simple driver for MCF51AG128 ColdFire MCU
*
******************************************************************************/
#ifndef   _CRC_H
#define   _CRC_H

#if 0
// MCF51AG128 - Peripheral
#define CRC_AddByte(x) CRCL = (unsigned char)(x);
#define CRC_AddLong(x) *((unsigned long*)(&CRCL0)) = (unsigned long)(x);
#define CRC_Init() SCGC2 = 0xff
#define CRC_Reset(x)  *((unsigned short*)(&CRCH)) = (unsigned short)(x);
#define CRC_GetResult() *((unsigned short*)(&CRCH)) 
#define CRC_GetResultHigh() CRCH
#define CRC_GetResultLow() CRCL

#else
// Kinetis - Peripheral
#define CRC_AddByte(x) CRC_CRCLL = (x);
#define CRC_AddLong(x) CRC_CRC = (x);
#define CRC_Init()  CRC_CTRL = 0;
#define CRC_Reset(x)  CRC_CTRL |= CRC_CTRL_WAS_MASK; CRC_CRCL = (x); CRC_CTRL &= ~CRC_CTRL_WAS_MASK;
#define CRC_GetResult() CRC_CRCL
#define CRC_GetResultHigh() CRC_CRCLU
#define CRC_GetResultLow()  CRC_CRCLL

#endif
#endif





