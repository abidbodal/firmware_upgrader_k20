/*****************************************************************************
 * (c) Copyright 2011, Freescale Semiconductor Inc.
 * ALL RIGHTS RESERVED.
 ***************************************************************************//*!
 * @file      flash_kinetis.c
 * @author    R20253
 * @version   1.0.1.0
 * @date      May-18-2012
 * @brief     Flash programming driver
 * @par       
 * @include   
 * @par       
 * @include         
 ******************************************************************************/
#include "flash_kinetis.h"

#include <string.h>

Byte pCommandArray[FCCOB_REGS];
static FCC0B_STR CommandObj;
void (*ExecuteOnStack)(void);
Byte buffer[128];   

#pragma define_section programbuff ".programbuff" far_abs RW
__declspec(programbuff) LWord programbuffer[64];


//  internal driver function
static Byte FTFL_FlashCommandSequence(Byte index);

void ExecuteOnStackStart();
/********************************************************
* Init Function 
*
********************************************************/
void FTFL_Initialization(void)
{
  Word i;

  //  initialize pointer to ram function 
  ExecuteOnStack = (void(*)(void))&buffer[1];   

  //  copy function from ROM to RAM
  for(i=0;i<(128+1);i++)
      buffer[i] = ((Byte*)ExecuteOnStackStart)[i-1];   
 
  //  inititalization of flash clock module 
  FTFL_INIT_FLASH_CLOCK;

}


/********************************************************
* Function for Programming of one Long Word 
*
********************************************************/
Byte FTFL_ProgramLongWord(LWord destination, LWord data32b)
{
  /* preparing passing parameter to program the flash block */
  
  CommandObj.regsLong.fccob3210 = destination;
  CommandObj.regs.fccob0 = FTFL_PROGRAM_LONGWORD;
  CommandObj.regsLong.fccob7654 = data32b; 

  return FTFL_FlashCommandSequence(PROGRAM_LONGWORD_INDEX);
}

/********************************************************
* Function for Programming of one section (maximum is 2048 Bytes) 
*
********************************************************/
Byte FTFL_ProgramSection(LWord destination, LWord* pSource, LWord size)
{
  LWord* pProgBuff = &programbuffer[0]; 
  
   // check the size of memory 
  if(size >= sizeof(programbuffer))
    return FTFL_FAIL;
  
  if(destination & 0x00000003)
    return FTFL_FAIL;
  
  /* preparing passing parameter to program the flash block */
  CommandObj.regsLong.fccob3210 = destination;
  CommandObj.regs.fccob0 = FTFL_PROGRAM_SECTION;
  CommandObj.regs.fccob4 = (Byte)(size >> 8); 
  CommandObj.regs.fccob5 = (Byte)(size & 0xff);
  
  //  copy source data to program buffer
  while(size--)
    *pProgBuff++ = *pSource++;
    
  //  call flash sequence
  return FTFL_FlashCommandSequence(PROGRAM_SECTION_INDEX);
}


/********************************************************
* Function for Programming of one section (maximum is 2048 Bytes) 
*
********************************************************/
Byte FTFL_ProgramSectionPhrases(LWord destination, LWord* pSource, LWord size)
{
  LWord* pProgBuff = &programbuffer[0]; 
  LWord my_size;
   // check the size of memory 
  if(size >= sizeof(programbuffer))
    return FTFL_FAIL;
  
  if(destination & 0x00000003)
    return FTFL_FAIL;
  
  if(destination & 0x00000004)
  {
    if(!FTFL_ProgramLongWord(destination, *pSource++))
      return FTFL_FAIL;
    size--;
    destination += 4;
  }
  my_size = size & 0xFFFFFFFE;
 
  if(my_size)
  {
    /* preparing passing parameter to program the flash block */
    CommandObj.regsLong.fccob3210 = destination;
    CommandObj.regs.fccob0 = FTFL_PROGRAM_SECTION;
    CommandObj.regs.fccob4 = (Byte)(size >> 9); 
    CommandObj.regs.fccob5 = (Byte)((size >> 1) & 0xff);
  
    destination += 4 * my_size; 
    
    //  copy source data to program buffer
    while(my_size--)
      *pProgBuff++ = *pSource++;
      
    //  call flash sequence
    if(!FTFL_FlashCommandSequence(PROGRAM_SECTION_INDEX))
      return FTFL_FAIL;
  }
  
  if(size & 0x00000001)
    if(!FTFL_ProgramLongWord(destination, *pSource))
      return FTFL_FAIL;  
 
  return 1;
}

/********************************************************
* Function for Programming of one section (maximum is 2048 Bytes) 
*
********************************************************/
Byte FTFL_ProgramSectionDoublePhrases(LWord destination, LWord* pSource, LWord size)
{
  LWord* pProgBuff = &programbuffer[0]; 
  LWord my_size;
  LWord i;
  // check the size of memory 
  if(size >= sizeof(programbuffer))
    return FTFL_FAIL;
  
  if(destination & 0x00000003)
    return FTFL_FAIL;
  
  if(destination & 0x0000000C)
  {
    i = (destination & 0x0000000C) >> 2;
    while(i)
    {
      if(!FTFL_ProgramLongWord(destination, *pSource++))
        return FTFL_FAIL;
      size--;
      destination += 4;
      i--;
    }
  }
  
  my_size = size & 0xFFFFFFFC;
 
  if(my_size)
  {
    /* preparing passing parameter to program the flash block */
    CommandObj.regsLong.fccob3210 = destination;
    CommandObj.regs.fccob0 = FTFL_PROGRAM_SECTION;
    CommandObj.regs.fccob4 = (Byte)(size >> 10); 
    CommandObj.regs.fccob5 = (Byte)((size >> 2) & 0xff);
  
    destination += 4 * my_size; 
    
    //  copy source data to program buffer
    while(my_size--)
      *pProgBuff++ = *pSource++;
      
    //  call flash sequence
    if(!FTFL_FlashCommandSequence(PROGRAM_SECTION_INDEX))
      return FTFL_FAIL;
  }
  
  if(size & 0x00000003)
  {
    while(size)
    {
      if(!FTFL_ProgramLongWord(destination, *pSource++))
        return FTFL_FAIL;
      size--;
      destination += 4;
    }        
  }
  return 1;
}

/********************************************************
* 
*
********************************************************/
Byte FTFL_ProgramByte(LWord destination, Byte data8b)
{
  CommandObj.regsLong.fccob3210 = destination;
  CommandObj.regs.fccob0 = FTFL_PROGRAM_BYTE;
  CommandObj.regs.fccob4 = data8b;
  
  return FTFL_FlashCommandSequence(PROGRAM_BYTE_INDEX);
}
        
/********************************************************
* Function for erasing of flash memory sector (0x800)
*
********************************************************/
Byte FTFL_EraseSector(LWord destination)
{
  CommandObj.regsLong.fccob3210 = destination;
  CommandObj.regs.fccob0 = FTFL_ERASE_SECTOR;

  return FTFL_FlashCommandSequence(ERASE_BLOCK_INDEX);
}


/********************************************************
* Function for erasing of all block of memory
*
********************************************************/
Byte FTFL_MassErase(void)
{
  CommandObj.regs.fccob0 = FTFL_ERASE_ALL_BLOCK;
  return FTFL_FlashCommandSequence(ERASE_MASS_INDEX);
}


static void ExecuteOnStackStart(void)
{
	//  launch a command 
  FTFL_FSTAT = FTFL_FSTAT_CCIF_MASK; 
  
  //  waiting for the finishing of the command
  while((FTFL_FSTAT&FTFL_FSTAT_CCIF_MASK) != FTFL_FSTAT_CCIF_MASK){};
}

/********************************************************
* Function for executing of the flash command
*
********************************************************/
static Byte FTFL_FlashCommandSequence(Byte index)
{
  Byte* ptrFccobReg = (Byte*)&FTFL_BASE_PTR->FCCOB3;
  Byte* ptrCommandObj = (Byte*)&CommandObj;
  
 
  ExecuteOnStack = (void(*)(void))&buffer[1]; 
  
  /* wait till CCIF bit is set */
  while((FTFL_FSTAT&FTFL_FSTAT_CCIF_MASK) != FTFL_FSTAT_CCIF_MASK){};

  /* clear RDCOLERR & ACCERR & FPVIOL flag in flash status register */
  FTFL_FSTAT = FTFL_FSTAT_ACCERR_MASK|FTFL_FSTAT_FPVIOL_MASK|FTFL_FSTAT_RDCOLERR_MASK;  
  
  /* load FCCOB registers */  
  while(index--)
    *ptrFccobReg++ = *ptrCommandObj++;
  
  //  jump to RAM function 
  ExecuteOnStack();
  //ExecuteOnStackStart();
  
   /* Check error bits */
  /* Get flash status register value */
  if(FTFL_FSTAT & (FTFL_FSTAT_ACCERR_MASK|FTFL_FSTAT_FPVIOL_MASK|FTFL_FSTAT_MGSTAT0_MASK)) 
    return 0;
  else 
  	return 1;
} 



