/******************************************************************************
* 
* Copyright (c) 2012 Freescale Semiconductor;
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
* @file      bootloader.c
*
* @author    b01119
* 
* @version   0.0.4.0
* 
* @date      Jul-25-2012
* 
* @brief     Bootloader state machine file
*
******************************************************************************/
#include "main.h"
#include "rs232.h"
#include "bootloader.h" 
#include "flash_kinetis.h"
#include "wdg.h"

static void SendResult(unsigned long res);
static void SendResultCRC(unsigned long res);
static void JumpToUserApplication(LWord userStartup);

extern void main(void);
extern unsigned long __SP_INIT[];


#pragma  define_section vectors_rom ".vectors_rom" far_abs R
__declspec(vectors_rom) vector_entry rom_vector[] = 
{
	(pointer*)__SP_INIT,	// Initial SP           
	main,					// Initial PC           
	Boot_ResetMCU,     
	Boot_ResetMCU,    
	Boot_ResetMCU,     
	Boot_ResetMCU,     
	Boot_ResetMCU,     
	Boot_ResetMCU,     
	Boot_ResetMCU,     
	Boot_ResetMCU,     
	Boot_ResetMCU,     
	Boot_ResetMCU,     
	Boot_ResetMCU,  
	Boot_ResetMCU,     
	Boot_ResetMCU,     
	Boot_ResetMCU  		
};


typedef union _FlashConfig_t
{
  unsigned int Data[4];
  struct {
    unsigned int BackDoorKey[2];
    unsigned int Protection;
    unsigned int Config;
  };
} FlashConfig_t;

#pragma define_section cfmconfig ".cfmconfig" far_abs R
__declspec(cfmconfig) FlashConfig_t Config  =
{
	0xFFFFFFFF, 0xFFFFFFFF, FLASH_BOOT_PROTECTION, 0xFFFFFFFE,
};



AddressType address;
AddressType tmp_transpone;

unsigned char write_buffer[4096];

unsigned long length;

unsigned long i, result;

unsigned short srs_backup;

AddressType  crc_res;

unsigned long enableBootMode;

unsigned long timeout_cnt;

const BootloaderIdentType bootloaderIdent = 
{
   /** version */
   (BootloaderProtocolType)_BOOTLOADER_PROTOCOL_VERSION,
   /** Sd Id */
   0x0000, // dummy - is gets in runtime
   /** count of flash blocks */
   1,
   /** flash blocks descritor */
   {USER_FLASH_START, USER_FLASH_END},
   /** Relocated interrupts vestor table */
   RELOCATED_VECTORS,
   /** Interrupts vestor table */
   INTERRUPT_VECTORS,
   /** Erase Block Size */
   FLASH_ERASE_PAGE,
   /** Write Block Size */
   FLASH_WRITE_PAGE,
   /** Id string */
   KINETIS_MODEL_STR      
};


//-----------------------------------------------------------------------------
// FUNCTION:    Boot_ResetMCU
// SCOPE:       Bootloader application system function
// DESCRIPTION: The function resets the MCU and support CONNECTED mode
//              
// PARAMETERS:  enableBootloader - TRUE/FALSE
//              
// RETURNS:     none
//-----------------------------------------------------------------------------  
void Boot_ResetMCU(void)
{ 
  WDG_ResetMCU(); 
}

//-----------------------------------------------------------------------------
// FUNCTION:    JumpToUserApplication
// SCOPE:       Bootloader application system function
// DESCRIPTION: The function startup user application
//              
// PARAMETERS:  pointer on user vector table
//              
// RETURNS:     function never go back
//-----------------------------------------------------------------------------  
static void JumpToUserApplication(unsigned long userStartup)
{
	unsigned long* ptr_jump = (unsigned long*)userStartup;

	/* set up stack pointer */  
	asm("LDR      r13, [r1] ");
	ptr_jump++;
	/* jump to application reset vector */	
	asm("BX 	 *ptr_jump");

}

//-----------------------------------------------------------------------------
// FUNCTION:    SendResult
// SCOPE:       Bootloader application system function
// DESCRIPTION: The function send the ACk or NACK
//              
// PARAMETERS:  res - TRUE/FALSE to send ACK or NACK
//              
// RETURNS:     none
//----------------------------------------------------------------------------- 
static void SendResult(unsigned long res)
{ 
  if(res)
    UART_PutChar(BOOT_CMD_ACK);
  else
    UART_PutChar(BOOT_CMD_NACK);
}

//-----------------------------------------------------------------------------
// FUNCTION:    SendResultCRC
// SCOPE:       Bootloader application system function
// DESCRIPTION: The function send the ACk or NACK with CRC (if enabled)
//              
// PARAMETERS:  res - TRUE/FALSE to send ACK or NACK
//              
// RETURNS:     none
//----------------------------------------------------------------------------- 
static void SendResultCRC(unsigned long res)
{ 
  SendResult(res);
  #if BOOTLOADER_CRC_ENABLE == 1
  if(res)
  {
    UART_PutChar(0xcf);
    UART_PutChar(0x63);
  }else
  {
    UART_PutChar(0xd1);
    UART_PutChar(0x93);
  }
  #endif
}

//-----------------------------------------------------------------------------
// FUNCTION:    ReadAddress
// SCOPE:       Bootloader application system function  
// DESCRIPTION: The functin reads the 4 bytes from IIC and store it to global address variable

//              
// PARAMETERS:  none
//              
// RETURNS:     none
//----------------------------------------------------------------------------- 
static void ReadAddress(void)
{
  address.Bytes.hh = UART_GetChar();
  address.Bytes.hl = UART_GetChar();
  address.Bytes.lh = UART_GetChar();
  address.Bytes.ll = UART_GetChar();
}

//-----------------------------------------------------------------------------
// FUNCTION:    ReadCrc
// SCOPE:       Bootloader application system function  
// DESCRIPTION: The functin reads the 2 bytes from IIC and store it to global crc variable

//              
// PARAMETERS:  none
//              
// RETURNS:     none
//----------------------------------------------------------------------------- 
#if BOOTLOADER_CRC_ENABLE == 1
  static void ReadCrc(void)
  {
    crc_res.Bytes.lh = UART_GetChar();
    crc_res.Bytes.ll = UART_GetChar();
  }
#endif

  //-----------------------------------------------------------------------------
  // FUNCTION:    TrimmSettings
  // SCOPE:       Trimming of Internal Oscilator 
  // DESCRIPTION: This function measures break impulse from PC and sets correct trim value

  //              
  // PARAMETERS:  none
  //              
  // RETURNS:     none
  //----------------------------------------------------------------------------- 
  unsigned char SlaveFrequencyCalibration(void)
  {
    Word trim;
    
    #if (KINETIS_REVISION_ID == m_0M33Z)
      Word diff;
    #endif
      
    UART_Deinitialization();  //  uart deinitialization

    BOOT_PIN_INIT_AS_GPIO; 
    GPIO_PDDR_REG(BOOT_CALIBRATION_GPIO_PORT) &= ~GPIO_PDDR_PDD(BOOT_UART_GPIO_PIN_RX);

    //  flex time initialization
    FTM_SC_REG(BOOT_CALIBRATION_TIMER) = 0;
    FTM_MOD_REG(BOOT_CALIBRATION_TIMER) = 0xffff;
    FTM_SC_REG(BOOT_CALIBRATION_TIMER) = FTM_SC_CLKS(1) | FTM_SC_PS(0);

    //  set trimm value
    #if KINETIS_REVISION_ID == m_0M33Z 
      TRIM_REG = 0x80;
    #endif
      
	#if KINETIS_REVISION_ID == m_1N30D || KINETIS_REVISION_ID == m_2N30D || KINETIS_REVISION_ID == m_3N30D || KINETIS_REVISION_ID == m_4N30D
      TRIM_REG = 0x80;//*(unsigned char*)(0x3FF);//0x8E;
      trim = TRIM_REG; 
    #endif    

    while(1)
    {
      //  waiting for rising edge of calibration impuls
      while(((GPIO_PDIR_REG(BOOT_CALIBRATION_GPIO_PORT) & GPIO_OUT_MASK) == GPIO_OUT_MASK)){}

      FTM_SC_REG(BOOT_CALIBRATION_TIMER) &= ~FTM_SC_TOF_MASK;    
      FTM_CNT_REG(BOOT_CALIBRATION_TIMER) = 0;  //  clear timer counter
       
      //  waiting for falling edge of calibration impuls, if timer overflow is performed the baudrated is not relevant 
      while((GPIO_PDIR_REG(BOOT_CALIBRATION_GPIO_PORT) & GPIO_OUT_MASK) == 0)
      {
        if (FTM_SC_REG(BOOT_CALIBRATION_TIMER) & FTM_SC_TOF_MASK)
          Boot_ResetMCU(); // OutOfLimit
      }
      timeout_cnt = FTM_CNT_REG(BOOT_CALIBRATION_TIMER); 

    //  for older silicon mask with bug with internal oscilator
     #if (KINETIS_REVISION_ID == m_0M33Z )

      diff = timeout_cnt - CALIB_PULSE_LENGTH;
      if(diff <= CALIB_DIFFERENCE)
        trim += CALIB_SMALL_STEP;    
      else  
        trim += CALIB_BIG_STEP;    

    //  for newer silicon version        
    #elif (KINETIS_REVISION_ID == m_1N30D || KINETIS_REVISION_ID == m_2N30D || KINETIS_REVISION_ID == m_3N30D || KINETIS_REVISION_ID == m_4N30D)  

      if(timeout_cnt < CALIB_PULSE_LENGTH)
        trim += trim / 2;    
      else  
        trim -= trim / 2;   
    
    #endif   
    //  if calculate value is in this range frequency of internal oscillator is correct set
      if((timeout_cnt > (CALIB_PULSE_LENGTH * 0.98)) && (timeout_cnt < (CALIB_PULSE_LENGTH * 1.02) ))
        break;    
    
      TRIM_REG = trim;  //  set calculated trim value  
    }
        BOOT_PIN_INIT_AS_UART;
        UART_Initialization();  //  uart Initialization
        return 0; 
  }

//-----------------------------------------------------------------------------
// FUNCTION:    Bootloader
// SCOPE:       Bootloader application system function 
// DESCRIPTION: This is main function of the project it substitute normal main function

//              
// PARAMETERS:  none
//              
// RETURNS:     none
//----------------------------------------------------------------------------- 
/* Entry point of bootloader */
/* Entry point of bootloader */
void main(void)
{
	unsigned int bootWaitTime = 0;
	uint8_t switch_to_ram_mode = 0;
	
  // Initialize the GPRs
 // asm("     CPSID   i       ");                   //; Disable interrupt by default
  
  SCB_VTOR = 0; 
  //TRIM_REG =  0x2E;//*(unsigned char*)(0x3FF);//
  //TRIM_REG2 = *(unsigned char*)(0x3FE);
  /* initialize any hardware specific issues */
  #if BOOTLOADER_INT_WATCHDOG == 1
    WDG_Enable(); 
  #else
    WDG_Disable();
  #endif
    
  // this setup all clock
  SIM_SOPT4   = -1;
  //SIM_SOPT5  default values - UART settings
  //SIM_SOPT6  reset filter option - default
  //SIM_SOPT7  adc - default
  SIM_SCGC1 |= SIM_SCGC1_UART4_MASK; // uart4/5 clock gate  | SIM_SCGC1_UART5_MASK
  //SIM_SCGC2  dac / enet
  //SIM_SCGC3  default
  // enable uart clock gate
  SIM_SCGC4 |= (SIM_SCGC4_UART0_MASK | SIM_SCGC4_UART1_MASK | SIM_SCGC4_UART2_MASK | SIM_SCGC4_UART3_MASK); // uarts enables
  // except bit 0, enable port A,B,C,D,E clock gate
  SIM_SCGC5 |= -1;  // ports
  // enable flexmemory / ftm0 / crc module clock gate
  SIM_SCGC6 |= (SIM_SCGC6_FTFL_MASK | SIM_SCGC6_FTM0_MASK | SIM_SCGC6_CRC_MASK);
  //SIM_SCGC7   default

  /* my code */
  // initialize variables
  timeout_cnt = 0;
  enableBootMode = 1;
  
  //Determine if a watchdog reset was induced by app code
  if (RCM_SRS0 & RCM_SRS0_WDOG_MASK)
  {
  	bootWaitTime = BOOT_WAITING_TIMEOUT;
  	enableBootMode = 1;
  }
  else
  {
  	bootWaitTime = 0;
  	enableBootMode = 0;
  }
  
#if KINETIS_MODEL == K60
    srs_backup = MC_SRSH << 8;
    srs_backup |= (Word)MC_SRSL;

    if(srs_backup & MC_SRSL_POR_MASK)
      enableBootMode = 1;
#else
    
    srs_backup = RCM_SRS0 << 8;
    srs_backup |= (Word)RCM_SRS1;
    
    if(srs_backup & (RCM_SRS0_POR_MASK << 8))
         enableBootMode = 1;
#endif 
  
  // Application is present ?
  if((*(unsigned long*)(RELOCATED_VECTORS + 8)) == -1)
    enableBootMode = 1;
//temp for test
//  enableBootMode = 0;
  
  if(enableBootMode)
  {
    // run the timer on 10ms
      
    FTM0_SC = 0;
    FTM0_MOD = (unsigned short)(0.01 / (128.0 / BOOT_BUS_CLOCK));    // TODO must be changed 
    FTM0_SC = FTM_SC_CLKS(1) |  FTM_SC_PS_MASK;

	#if BOOTLOADER_CRC_ENABLE == 1
      CRC_Init();    
    #endif
    
      //  multiplexer setting  to UART alternative
    BOOT_PIN_INIT_AS_UART;

    // init UART module
    UART_Initialization(); 
    
    while(1)
    {
      #if BOOTLOADER_INT_WATCHDOG == 1
        WDG_Refresh(); /* feeds the dog */
      #endif
      
      if(FTM0_SC & FTM_SC_TOF_MASK)
      {
        // increment timeout value
        timeout_cnt++;
        if((timeout_cnt % 25) == 1)
          SendResult(1);
        
        FTM0_SC &= ~FTM_SC_TOF_MASK;  
        if((timeout_cnt > bootWaitTime) && ((*(unsigned long*)(RELOCATED_VECTORS + 8)) != -1))
        {
        	timeout_cnt++;
          break;
        }
      }
      
      // check the UART activity
      if(UART_IsChar())
      {
        //  measuring of brake signal due to using of Internal oscilator..
        #if (BOOTLOADER_AUTO_TRIMMING == 1) 
          (void)SlaveFrequencyCalibration();
        #else 
          #warning Auto Trimming feature of the Bootloader is not enabled !!
        #endif        

          SendResult(1);

        // init the flash module
        FTFL_Initialization(); 
        enableBootMode++;
        SendResult(1);
        break;
      }  
    }

     
    //  Bootloader protocol runs !
    while(enableBootMode > 1)
    {
      // never ending loop - except quit commnad that disable  enableBootMode
      result = 1;
      
      //If first write request disable EEPROM Mode
      if (!switch_to_ram_mode)
      {
      	FTFL_FCCOB0 = FTFL_FCCOB0_CCOBn(0x81); // Selects the PGMPART command
      	FTFL_FCCOB1 = 0xFF;

        /* All required FCCOBx registers are written, so launch the command */
        FTFL_FSTAT = FTFL_FSTAT_CCIF_MASK;

        /* Wait for the command to complete */
        while(!(FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK));
        
        while (!(FTFL_FCNFG & FTFL_FCNFG_RAMRDY_MASK))
        {
        	switch_to_ram_mode++;
        }

      	switch_to_ram_mode = 1;
      }
      
      #if BOOTLOADER_CRC_ENABLE == 1
        CRC_Reset(0xffff);
      #endif
      
      switch(UART_GetChar())
      {
        case BOOT_CMD_QUIT:                                 // QUIT command
          #if BOOTLOADER_CRC_ENABLE == 1
            CRC_AddByte(BOOT_CMD_QUIT);  
            ReadCrc();
            if(crc_res.Words.low != CRC_GetResult())
            {            
              SendResultCRC(0);
              break;
            }
          #endif
        
        // send the ack
          SendResultCRC(result);
          // reset MCU
         // Boot_ResetMCU();
          enableBootMode = 0; //continue to App code
          break;
          

        case BOOT_CMD_ACK:
//protocol:
          SendResult(1);                                   // ACK command
          break;
        
        case BOOT_CMD_IDENT:                               // IDENT command
          //and send the all data with/without CRC 
          #if BOOTLOADER_CRC_ENABLE == 1
            CRC_AddByte(BOOT_CMD_IDENT);  
            CRC_AddByte(bootloaderIdent.version);
          #endif
          UART_PutChar(bootloaderIdent.version);
          #if BOOTLOADER_CRC_ENABLE == 1
            CRC_AddByte(SIM_SDID >> 8);
          #endif
          UART_PutChar(SIM_SDID >> 8); // high
          #if BOOTLOADER_CRC_ENABLE == 1
            CRC_AddByte(SIM_SDID);
          #endif
          UART_PutChar(SIM_SDID);      
          
          for(i=0; i < ((sizeof(bootloaderIdent) - 3 - ID_STRING_MAX) / 4); i++)
          {
            #if BOOTLOADER_CRC_ENABLE == 1
              CRC_AddLong(((unsigned long*)(&bootloaderIdent.blocksCnt))[i]);
            #endif
              tmp_transpone.complete = ((unsigned long*)(&bootloaderIdent.blocksCnt))[i];
              UART_PutChar(tmp_transpone.Bytes.hh);
              UART_PutChar(tmp_transpone.Bytes.hl);
              UART_PutChar(tmp_transpone.Bytes.lh);
              UART_PutChar(tmp_transpone.Bytes.ll);
          }
          i = 0;
          do
          {
            #if BOOTLOADER_CRC_ENABLE == 1
              CRC_AddByte(bootloaderIdent.idString[i]);
            #endif
            UART_PutChar(bootloaderIdent.idString[i]);   
          }while(bootloaderIdent.idString[i++]);  
            
          #if BOOTLOADER_CRC_ENABLE == 1
            UART_PutChar(CRC_GetResultHigh());         
            UART_PutChar(CRC_GetResultLow());
          #endif
              
          break;
          
        case BOOT_CMD_ERASE:                                // ERASE command
          // Read Address
          ReadAddress();
          // Check the CRC
          #if BOOTLOADER_CRC_ENABLE == 1
            
            CRC_AddByte(BOOT_CMD_ERASE);
            CRC_AddLong(address.complete);
            
            ReadCrc();
            if(crc_res.Words.low != CRC_GetResult())
            {            
              SendResultCRC(0);
              break;
            }
          #endif
          // Erase the flash and send result
          SendResultCRC(FTFL_EraseSector(address.complete));
          break;

        case BOOT_CMD_MASS_ERASE:                                // ERASE command
          // Read Address
          //ReadAddress();
          // Check the CRC
          #if BOOTLOADER_CRC_ENABLE == 1
            
            CRC_AddByte(BOOT_CMD_ERASE);
            CRC_AddLong(address.complete);
            
            ReadCrc();
            if(crc_res.Words.low != CRC_GetResult())
            {            
              SendResultCRC(0);
              break;
            }
          #endif
          // Erase the flash and send result
          SendResultCRC(FTFL_MassErase());
          break;
          
        case BOOT_CMD_WRITE:
          // Read Address                                                   // WRITE command
          ReadAddress();
          // Read length
          length = UART_GetChar();
          
          #if BOOTLOADER_CRC_ENABLE == 1
            CRC_AddByte(BOOT_CMD_WRITE);
            CRC_AddLong(address.complete);
            CRC_AddByte((unsigned char)length);
          #endif
          
          // Load the data to write
          for(i = 0;i<length; i++)
          {
            write_buffer[i] =  UART_GetChar(); 
            #if BOOTLOADER_CRC_ENABLE == 1
              CRC_AddByte(write_buffer[i]);
            #endif  
          }
          
          length >>= 2;    // divide by four
          
          // check the CRC of input data
          
          #if BOOTLOADER_CRC_ENABLE == 1
            ReadCrc();
            if(crc_res.Words.low != CRC_GetResult())
            {            
              SendResultCRC(0);
              break;
            }
          #endif
                
          if(!WRITE_FLASH_FUNC(address.complete, (LWord*)write_buffer, length))
          {            
            SendResultCRC(0);
            break;
          }
          
          
          // Verify flashed data (if enabled)
          #if BOOTLOADER_ENABLE_VERIFY == 1                 
            for(i = 0;i<length; i++)
            {              
              if(((unsigned long*)write_buffer)[i] != ((unsigned long*)address.complete)[i])
                result = 0;

            }
          #endif    
          
          SendResultCRC(result);
          break;
       
      #if BOOTLOADER_ENABLE_READ_CMD == 1       
          
        case BOOT_CMD_READ:                                   // READ comamnd    
          ReadAddress();
          length = UART_GetChar();
          
          #if BOOTLOADER_CRC_ENABLE == 1
            ReadCrc();
            
            CRC_AddByte(BOOT_CMD_READ);
            CRC_AddLong(address.complete);
            CRC_AddByte((unsigned char)length);
            
            if(crc_res.Words.low != CRC_GetResult())
            {            
              SendResultCRC(0);
              break;
            }
          #endif
          
          #if BOOTLOADER_CRC_ENABLE == 1
            CRC_Reset(0xffff);
          #endif
          
          for(i = 0;i<length; i++)
          {
            #if BOOTLOADER_CRC_ENABLE == 1
              CRC_AddByte(((unsigned char*)(address.complete))[i]);
            #endif              
              UART_PutChar(((unsigned char*)(address.complete))[i]); 
          }
          
          #if BOOTLOADER_CRC_ENABLE == 1
            UART_PutChar(CRC_GetResultHigh());         
            UART_PutChar(CRC_GetResultLow());
          #endif
          
          break;
         
        case BOOT_CMD_VERSION:
        	UART_PutChar(BOOTLOADER_FIRMWARE_MAJOR_VERSION);
        	UART_PutChar(BOOTLOADER_FIRMWARE_MINOR_VERSION);
        	break;
          
       #endif   
          
        default:
          //Boot_ProtocolReset();
        	enableBootMode = 0; //continue to App code
          break;  
          
      }      
    } 
  }
  
/*	printf(" ======================================== \r\n");
  	printf(" [[ K60 Kinetis Serial Bootloader Test ]] \r\n");
  	printf(" ======================================== \r\n");*/

  	//switch back to eeprom mode
		if (switch_to_ram_mode)
		{
			FTFL_FCCOB0 = FTFL_FCCOB0_CCOBn(0x80); // Selects the PGMPART command
			FTFL_FCCOB1 = 0x00;
			
      /* All required FCCOBx registers are written, so launch the command */
      FTFL_FSTAT = FTFL_FSTAT_CCIF_MASK;

      /* Wait for the command to complete */
      while(!(FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK));

			switch_to_ram_mode = 0;
		}
  
     // relocate vector table
    SCB_VTOR = RELOCATED_VECTORS;
    
    // Jump to user application
    JumpToUserApplication(RELOCATED_VECTORS);   
}
