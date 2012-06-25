/*
 * devicedbg.c: Program to provide register value of various section of SoC 
 * 
 * This program is based on devmem2.c originally developed by Jan-Derk Bakker 
 * 'devmem2.c' is modified to read SoC identification from '/proc/cpuinfo' and
 * based on SoC, current value of registers of various section of SoC can monitored.
 * 
 *  Copyright (C) 2000, Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
 *  Copyright (C) 2012, Viral Sachde 	(viralsachde@gmail.com) 
 *  Copyright (C) 2012, Anil Kag 	(anilkagak2e@gmail.com) 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* Note:
 * If you encounter 'bus error' while reading the contents then,
 * recompile your linux kernel with the configuration setting
 *	CONFIG_OMAP_RESET_CLOCKS = n
 * This option do not reset the inactive clocks so that you have the access to 
 * the unused modules also.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "devicedbg.h"

/*
 * Main Routine for the program
 */
int main(int argc, char **argv) {
	int num_regs = 0;
	int choice = 0;

	if(argc < 2) {
		fprintf(stderr, "Usage:\t%s { reg }\n"
			"reg: desired register selection : [0]DCAN,[1]GPIO,[2]I2C,[3]LCD,[4]MCASP,[5]MCSPI,[6]MMCSD,[7]TSC*,[8]UART,[9]USB*,[10]PRODUT_ID,[11]LCD_CONTROLLER; \n\t* are not implemented yet \n",argv[0]);
		exit(1);
	}

        int n = atoi(argv[1]);
	int processor = read_processor();	// which processor we are working on

	if( processor == OMAP4 ) {
		switch(n) {
			case PRODUCT_ID:
				// base 0x4A002000
				num_regs = ARRAY_SIZE(struct reg_info, product_id_registers);
				printf("\nProduct ID Registers:\n");
				show_registers(product_id_registers,num_regs,argc,argv,0x4A002000);
				break;

			case LCD_CONTROLLER:
				// base 0x48041000
				num_regs = ARRAY_SIZE(struct reg_info, lcd_controller_registers);
				printf("\nLCD Controller Registers:\n");
				show_registers(lcd_controller_registers,num_regs,argc,argv,0x48041000);
				break;

			case DCAN:
				printf("Not available on pandaboard\n");
				break;

			case GPIO:
				printf("Not available on pandaboard\n");
				break;

			case I2C:
				/* bases
				* i2c3 0x48060000
				* i2c1 0x48070000
				* i2c2 0x48072000
				* i2c4 0x48350000
				*/
				num_regs = ARRAY_SIZE(struct reg_info, i2c_registers);
				printf("\nI2C Registers:\n");
				printf("Which one of the four I2Cs?? [1,2,3,4]:\n");
				scanf("%d",&choice);

				switch (choice) {
					case 1:
					show_registers(i2c_registers,num_regs,argc,argv,0x48070000);
					break;

					case 2:
					show_registers(i2c_registers,num_regs,argc,argv,0x48072000);
					break;

					case 3:
					show_registers(i2c_registers,num_regs,argc,argv,0x48060000);
					break;

					case 4:
					show_registers(i2c_registers,num_regs,argc,argv,0x48350000);
					break;

					default:
						printf("Wrong choice\n");
				}
				break;

			case LCD:
				// base 0x48040000
				num_regs = ARRAY_SIZE(struct reg_info, lcd_registers);
				printf("\nLCD Registers:\n");
				show_registers(lcd_registers,num_regs,argc,argv,0x48040000);
				break;

			case MCASP:
				// base 0x49028000
				num_regs = ARRAY_SIZE(struct reg_info, mcasp_registers);
				printf("\nMcASP Registers:\n");
				show_registers(mcasp_registers,num_regs,argc,argv,0x49028000);
				break;

			 case MCSPI:
				/* bases
				* MCSPI1	0x48098000
				* MCSPI2	0x4809A000
				* MCSPI3	0x480B8000
				* MCSPI4	0x480BA000
				*/
				num_regs = ARRAY_SIZE(struct reg_info, mcspi_registers);
				printf("\nMcSPI Registers:\n");
				printf("Which one of the four McSPIs?? [1,2,3,4]:\n");
				scanf("%d",&choice);

				switch (choice) {
					case 1:
					show_registers(mcspi_registers,num_regs,argc,argv,0x48098000);
					break;

					case 2:
					show_registers(mcspi_registers,num_regs,argc,argv,0x4809A000);
					break;

					case 3:
					show_registers(mcspi_registers,num_regs,argc,argv,0x480B8000);
					break;

					case 4:
					show_registers(mcspi_registers,num_regs,argc,argv,0x480BA000);
					break;

					default:
						printf("Wrong choice\n");
				}
				break;

			 case MMCSD:
				/* bases
				* MMCHS1	0x4809C000
				* MMCHS2	0x480B4000
				* MMCHS3	0x480AD000
				* MMCHS4	0x480D1000
				* MMCHS5	0x480D5000
				*/
				num_regs = ARRAY_SIZE(struct reg_info, mmchs_registers);
				printf("\nMMC/SD/SDIO Registers:\n");
				printf("Which one of the 5 MMCHSs?? [1,2,3,4,5]:\n");
				scanf("%d",&choice);

				switch (choice) {
					case 1:
					show_registers(mmchs_registers,num_regs,argc,argv,0x4809C000);
					break;

					case 2:
					show_registers(mmchs_registers,num_regs,argc,argv,0x480B4000);
					break;

					case 3:
					show_registers(mmchs_registers,num_regs,argc,argv,0x480AD000);
					break;

					case 4:
					show_registers(mmchs_registers,num_regs,argc,argv,0x480D1000);
					break;

					case 5:
					show_registers(mmchs_registers,num_regs,argc,argv,0x480D5000);
					break;

					default:
						printf("Wrong choice\n");
				}
				break;

			 case TSC:
				    break;

			 case UART:
				/* bases
				* uart1 0x4806A000
				* uart2 0x4806C000
				* uart3 0x48020000
				* uart4 0x4806E000
				*/
				num_regs = ARRAY_SIZE(struct reg_info, uart_registers);
				printf("\nUART Registers:\n");
				printf("Which one of the four UARTs?? [1,2,3,4]:\n");
				scanf("%d",&choice);

				switch (choice) {
					case 1:
					show_registers(uart_registers,num_regs,argc,argv,0x4806A000);
					break;

					case 2:
					show_registers(uart_registers,num_regs,argc,argv,0x4806C000);
					break;

					case 3:
					show_registers(uart_registers,num_regs,argc,argv,0x48020000);
					break;

					case 4:
					show_registers(uart_registers,num_regs,argc,argv,0x4806E000);
					break;

					default:
						printf("Wrong choice\n");
				}
				break;

	 			case USB:
					break;

			default:
				printf("Invalid section\n");
		}
	}

	else if( processor == AM335x ) {
		printf("Currently code is not mixed with this program\n");
	}

	else {
		printf("Processor not supported\n");
	}

	return 0;
}
