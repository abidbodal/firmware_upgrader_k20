/*****************************************************************************
 * (c) Copyright 2011, Freescale Semiconductor Inc.
 * ALL RIGHTS RESERVED.
 ***************************************************************************//*!
 * @file      flash_kinetis.h
 * @author    R20253
 * @version   1.0.1.0
 * @date      May-18-2012
 * @brief     Flash programming driver header file
 * @par       
 * @include   
 * @par       
 * @include         
 ******************************************************************************/
#ifndef _FLASH_KINETIS_H
#define _FLASH_KINETIS_H

#include "main.h"

//  Flash hardware algorithm operation commands 
#define FTFL_PROGRAM_BYTE        0x04
#define FTFL_PROGRAM_WORD        0x05
#define FTFL_PROGRAM_LONGWORD    0x06
#define FTFL_ERASE_BLOCK         0x08
#define FTFL_ERASE_SECTOR        0x09
#define FTFL_PROGRAM_SECTION     0x0B
#define FTFL_ERASE_ALL_BLOCK     0x44

#define FCCOB_REGS  12
#define FTFL_OK     1
#define FTFL_FAIL   0

//  FCOOB register structure
typedef union 
{
  Byte all[FCCOB_REGS];
  struct
  {
    Byte fccob3;
    Byte fccob2;
    Byte fccob1;
    Byte fccob0;
    Byte fccob7;
    Byte fccob6;
    Byte fccob5;
    Byte fccob4;
    Byte fccobB;
    Byte fccobA;
    Byte fccob9;
    Byte fccob8;
  }regs;
  
  struct
  {
    LWord fccob3210;
    LWord fccob7654;
    LWord fccobBA98;
  }regsLong;
}FCC0B_STR;

#define ERASE_MASS_INDEX       4
#define ERASE_BLOCK_INDEX      4
#define PROGRAM_BYTE_INDEX     5
#define PROGRAM_SECTION_INDEX  7
#define PROGRAM_LONGWORD_INDEX 8

#define FTFL_INIT_FLASH_CLOCK SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV4(16); SIM_SCGC6 |= SIM_SCGC6_FTFL_MASK;

//  API FUNCTION FOR KINETIS FLASH DRIVER
void FTFL_Initialization(void);
Byte FTFL_EraseSector(LWord destination);
Byte FTFL_ProgramLongWord(LWord destination, LWord data);
Byte FTFL_ProgramSection(LWord destination, LWord* pSource, LWord size);
Byte FTFL_ProgramSectionPhrases(LWord destination, LWord* pSource, LWord size);
Byte FTFL_ProgramSectionDoublePhrases(LWord destination, LWord* pSource, LWord size);
Byte FTFL_MassErase(void);


#endif



