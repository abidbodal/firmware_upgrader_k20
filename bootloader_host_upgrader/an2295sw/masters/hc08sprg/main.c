/**********************************************************************
* HEADER_START                                                        *
*                                                                     *
*      Name:           MAIN.C                                         *
*      Project:        Developper's HC08 Bootloader Master            *
*      Description:    MAIN file                                      *
*      Platform:       Linux/Win32                                    *
*      Revision:       1.0                                            *
*      Date:           Apr 8 2002                                     *
*      Author:         Michal Hanak                                   *
*      Company:        Freescale                                      *
*      Security:       General Business                               *
*                                                                     *
* =================================================================== *
* Copyright (c):       FREESCALE Inc., 2004, All rights reserved.     *
*                                                                     *
* =================================================================== *
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY              *
* EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR  *
* PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL FREESCALE OR             *
* ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    *
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT        *
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;        *
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)            *
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, *
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED *
* OF THE POSSIBILITY OF SUCH DAMAGE.                                  *
* =================================================================== *
*
* HEADER_END
*/


/*
	Changelog:
		30-May-2008		MC9S08(SG/SH)(16/32) family added (SDID 0x1A)
		12-Jun-2008		MC9S08(DZ/DV)(96/128) family added (SDID 0x19), BL_HCS08_LONG
		05-Jan-2009		MC9S08AC(32-60) family added (SDID 0x1D), BL_HCS08
		08-Jan-2009		MC9S08MP(12-16) family added (SDID 0x27), BL_HCS08
		29-Jul-2009		ColdFire support completely added & tested
        06-Aug-2009 	MC9S08AC(96/128) family added (SDID 0x1B), BL_HCS08_LARGE
                        MC9S08SE(4/8) family added (SDID 0x025), BL_HCS08
        17-Nov-2009 	MC9S08LL(8/16) family added (SDID 0x21), BL_HCS08
                        MC9S08(LL/LH)(36/64) family added (SDID 0x26), BL_HCS08_LONG
        23-Nov-2009     MC9S08SC4 family added (SDID 0x2B),	BL_HCS08
        24-Jul-2011     MC1323x family added (SDID 0x28),	BL_HCS08_LARGE
		15-Mar-2012		MC1231x / QE32 (SDID 0x1F) added,	BL_HCS08

*/


#include "sysdep.h"
#include "hc08sprg.h"
#include "serial.h"
#include "main.h"
#include "stdarg.h"
#include <time.h>

// The global variables has been moved into prog.c file due to beeter compatibility with Windows version of code!

extern const char an2295Version[];

void printCF(FILE *stream, char *format, ...)
{
	va_list args;

	va_start(args, format);
	vprintf(format, args);
	vfprintf(stream, format, args);
	va_end(args);
}

void getLocalTime(char * buff, int size)
{
	//char buff[20];
	struct tm newTime;
	time_t now = time(0);
	localtime_s(&newTime, (&now));
	//gmtime_s(&newTime, (&now));
	strftime(buff, size, "%Y-%m-%d_%H-%M-%S", &newTime);
}

int main(int argc, char *argv[])
{
	int i;
	int short_trim = 0;
	int serialPortRetryTime = 0;
	errno_t err;
	char timebuff[20];

	err = fopen_s(&logfile, "hc08sprg.log", "a+");
	getLocalTime(timebuff, 20);
	fprintf(logfile, "------------------------------------------------------------------------------\n");
	fprintf(logfile, "hc08sprg - Developer's HC/S08/CFV1/V2/Kinetis Serial Bootloader\n");
	fprintf(logfile, "%s\n", an2295Version);
	fprintf(logfile, "------------------------------------------------------------------------------\n");
	fprintf(logfile, "FC protocol versions supported:\n"
		"\t0x%02X (%s)\n"
		"\t0x%02X (%s)\n"
		"\t0x%02X (%s)\n"
		"\t0x%02X (%s)\n"
		"\t0x%02X (%s)\n"
		"\t0x%02X (%s)\n"
		"\t0x%02X (%s)\n",
		BL_HC08, Version_string(BL_HC08),
		BL_HC08_LARGE, Version_string(BL_HC08_LARGE),
		BL_HCS08, Version_string(BL_HCS08),
		BL_HCS08_LONG, Version_string(BL_HCS08_LONG),
		BL_HCS08_LARGE, Version_string(BL_HCS08_LARGE),
		BL_CFV1, Version_string(BL_CFV1),
		BL_KINETIS, Version_string(BL_KINETIS));
	fprintf(logfile, "program start time %s \n", timebuff);
	fprintf(logfile, "------------------------------------------------------------------------------\n\n");

	if (argc != 3 && argc != 4)
	{


		printf( 
			"usage:  hc08sprg port[:|!][D|d|S|s|?][*] [speed] file\n"
			"        port:D ... dual wire mode [default]\n"
			"        port:d ... dual wire mode with verification supressed\n"
			"        port:S ... single wire mode\n"
			"        port:s ... single wire mode with verification supressed\n"
			"        port:? ... detect single/dual wire mode (use with caution)\n"
			"            !  ... batch mode, no questions\n"
			"            *  ... short trim\n"
			"        speed  ... speed in bps\n"
			"        file   ... S19 file\n\n");

		if ((argc == 2) && ((argv[1][0] == '?') || ((argv[1][1] == '?'))))
		{
			fprintf(logfile, "Recognized S08/CFV1/Kinetis MCU families:\n\n");
			fprintf(logfile, "SDID\tproto\tfamily\n");

			i = 0;
			do
			{
				fprintf(logfile, "0x%02X\t0x%02X\t%s\n", sdidprops[i].sdid, sdidprops[i].proto, sdidprops[i].name);
			} while (sdidprops[++i].sdid != SDID_UNDEF);

			fprintf(logfile, "\n");
		}

		fprintf(logfile, "See Freescale Application Note AN2295 and AN2295SW for updates.\n");

		exit(0);
	}

	// Check the additional short trim option
	i = strlen(argv[1]);

	if (i)
		i--;

	if (argv[1][i] == '*')
	{
		fprintf(logfile, "short Trim enabled\n");
		short_trim = 1;
		argv[1][i] = 0;
	}
	fprintf(logfile, "arg0 %s arg1 %s, arg2 %s, arg3 %s, arg4 %s \n", argv[0], argv[1], argv[2], argv[3], argv[4]);

	// open serial port (speed specified ?)
	printCF(logfile, "Connecting to bootloader USB port... \n");
	while (init_uart(argv[1], argc == 4 ? argv[2] : NULL) < 0)
	{	
		if (serialPortRetryTime > 5000) //5 seconds
		{
			printCF(logfile,  "Timed out on opening bootloader USB port %s\n", argv[1]);
			exit(1);
		}
		serialPortRetryTime += 50;
		Sleep(50);
	}
	fprintf(logfile, "Connected to bootloader USB port in %d ms \n", serialPortRetryTime);
	
		

	// wait for reset
	if(hook_reset() < 0) 
		exit(3);

	fprintf(logfile, "Hook reset completed \n");
	// calibrate target's oscilator
	if(calibrate_speed(short_trim) < 0)
		exit(4);

	fprintf(logfile, "Calibrate completed \n");
	// read info
	if(!read_mcu_info())
	{
		fprintf(logfile, "\nBootloader protocol version: 0x%02X (%s, ", bl_version /*| (bl_rcs ? 0x80 : 0)*/, Version_string(bl_version));
		fprintf(logfile, "read command ");
		if (!bl_rcs) fprintf(logfile, "NOT ");
		fprintf(logfile, "supported,\n");
		fprintf(logfile, "Protocol secured:");
		if (!bl_crcs) 
			fprintf(logfile, "none)\n) ");		
		else
			fprintf(logfile, " CRC-CCITT)\n");
		fprintf(logfile, "Bootloader version string: %s\n", ident.targ_name);

		if (bl_version == BL_HC08) // HC08
		{
			unsigned i;
			fprintf(logfile, "Available flash memory: 0x%04X-0x%04X\n", ident.mem_start[0], ident.mem_end[0]-1);
			fprintf(logfile, "Erase block size: %3d bytes\n", ident.erblk);
			fprintf(logfile, "Write block size: %3d bytes\n", ident.wrblk);
			fprintf(logfile, "Original vector table: 0x%04X\n",ident.int_vect_tbl);
			fprintf(logfile, "Bootloader user table: 0x%04X\n",ident.bl_tbl);
			fprintf(logfile, "Bootloader data (hex): ");
			for(i=0; i<sizeof(ident.priv_data); i++)
				fprintf(logfile, "%02x ", ident.priv_data[i]);
			fprintf(logfile, "\n");
		}
		else if (bl_version == BL_HCS08) // S08
		{
			unsigned i;
			fprintf(logfile, "System device ID: 0x%03X [%s] rev. %u\n", ident.sdid, chipid ,ident.sdidrev);
			fprintf(logfile, "Number of memory blocks: %u\n", ident.num_blocks);
			for(i=0; i<ident.num_blocks; i++)
				fprintf(logfile, "Memory block #%u: 0x%04X-0x%04X\n", i+1, ident.mem_start[i], ident.mem_end[i]-1);
			fprintf(logfile, "Erase block size: %3d bytes\n", ident.erblk);
			fprintf(logfile, "Write block size: %3d bytes\n", ident.wrblk);
			fprintf(logfile, "Original vector table: 0x%04X-0xFFFF\n",ident.int_vect_tbl);
			fprintf(logfile, "New vector table:      0x%04X-0x%04X\n",ident.bl_tbl, ident.bl_tbl+(0xFFFF-ident.int_vect_tbl));
		}
		else if (bl_version == BL_HC08_LARGE)	// large HC08
		{
			unsigned i;
			fprintf(logfile, "Number of memory blocks: %u\n", ident.num_blocks);
			for(i=0; i<ident.num_blocks; i++)
				fprintf(logfile, "Memory block #%u: 0x%04X-0x%04X\n", i+1, ident.mem_start[i], ident.mem_end[i]-1);
			fprintf(logfile, "Erase block size: %3d bytes\n", ident.erblk);
			fprintf(logfile, "Write block size: %3d bytes\n", ident.wrblk);
			fprintf(logfile, "Original vector table: 0x%04X\n",ident.int_vect_tbl);
			fprintf(logfile, "Bootloader user table: 0x%04X\n",ident.bl_tbl);
		}
		else if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))// large + long S08
		{
			unsigned i;
			fprintf(logfile, "System device ID: 0x%03X [%s] rev. %u\n", ident.sdid, chipid ,ident.sdidrev);
			fprintf(logfile, "Number of memory blocks: %u\n", ident.num_blocks);
			for(i=0; i<ident.num_blocks; i++)
				fprintf(logfile, "Memory block #%u: 0x%06X-0x%06X\n", i+1, ident.mem_start[i], ident.mem_end[i]-1);
			fprintf(logfile, "Erase block size: %3d bytes\n", ident.erblk);
			fprintf(logfile, "Write block size: %3d bytes\n", ident.wrblk);
			fprintf(logfile, "Original vector table: 0x%04X\n",ident.int_vect_tbl);
			fprintf(logfile, "Bootloader user table: 0x%04X\n",ident.bl_tbl);
		}
		else if (bl_version == BL_CFV1) // CFV1
		{
			unsigned i;
			fprintf(logfile, "System device ID: 0x%03X [%s] rev. %u\n", ident.sdid, chipid ,ident.sdidrev);
			fprintf(logfile, "Number of memory blocks: %u\n", ident.num_blocks);
			for(i=0; i<ident.num_blocks; i++)
				fprintf(logfile, "Memory block #%u: 0x%08X-0x%08X\n", i+1, ident.mem_start[i], ident.mem_end[i]);
			fprintf(logfile, "Erase block size: %3d bytes\n", ident.erblk);
			fprintf(logfile, "Write block size: %3d bytes\n", ident.wrblk);
            if (ident.int_vect_tbl != 0) 
            {
    			fprintf(logfile, "Original vector table: 0x%08X\n-0x%08X\n",NULL, ident.int_vect_tbl-1);
    			fprintf(logfile, "New vector table: 0x%08X\n-0x%08X\n",ident.bl_tbl,ident.bl_tbl+ident.int_vect_tbl-1);
            } else
    			fprintf(logfile, "Vector table not relocated\n");
		}
		else if (bl_version == BL_KINETIS) // KINETIS
		{
			unsigned i;
			fprintf(logfile, "System device ID: 0x%03X [%s] rev. %u\n", ident.sdid, chipid ,ident.sdidrev);
			fprintf(logfile, "Kinetis Package: %s .\n", get_package_str(ident.sdid));
			fprintf(logfile, "Number of memory blocks: %u\n", ident.num_blocks);
			for(i=0; i<ident.num_blocks; i++)
				fprintf(logfile, "Memory block #%u: 0x%08X-0x%08X\n", i+1, ident.mem_start[i], ident.mem_end[i]);
			fprintf(logfile, "Erase block size: %3d bytes\n", ident.erblk);
			fprintf(logfile, "Write block size: %3d bytes\n", ident.wrblk);
        	fprintf(logfile, "Original vector table: 0x%08X-0x%08X\n",ident.int_vect_tbl,ident.int_vect_tbl + 0x3FF);
			fprintf(logfile, "New vector table: 0x%08X-0x%08X\n",ident.bl_tbl, ident.bl_tbl + 0x3FF);
		}
		else
		{
			fprintf(logfile,  "This version [0x%02X] of protocol is not (yet) supported! Check AN2295SW for update!\n", bl_version);
			exit(9);
		}
	}
	else 
	{
		fprintf(logfile,  "Can't read MCU info. Could be protocol error.\n");
		fprintf(logfile,  "(Or forgot to set single wire mode?)\n");
		exit(5);
	}

	read_bootloader_version();


	// read S19 file
	if (read_s19(argv[argc - 1], &image) < 0)
		exit(2);
	
	// masquerade vector table
	if(setup_vect_tbl(ident.int_vect_tbl , ident.bl_tbl) < 0)
		exit(6);

	// check if code goes to valid memory only - issue warning only, but continue! (r30323)
	check_image();

	if (!defaultyes)
	{	
		// last bless
		fprintf(logfile, "\nAre you sure to program part? [y/N]: ");
		if(toupper(getchar()) != 'Y') exit(-1);
	}
	else
	{
		putchar('\n');
	}

	// program it!
	if(prg_mem() < 0)
	{
		printCF(logfile, "Program memory failed.\n");
		exit(8);
	}

	// exit program mode, start application
	unhook();

	close_uart();

	getLocalTime(timebuff, 20);
	fprintf(logfile, "program end time %s\n", timebuff);
	fprintf(logfile, "\n");
	fprintf(logfile, "\n");
	fprintf(logfile, "------------------------------------------------------------------------------\n");
	fclose(logfile);
	return 0;
}
