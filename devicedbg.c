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
			"reg: desired register selection :[0]DCAN,[1]GPIO,[2]I2C,[3]LCD,[4]MCASP/MCBSP,[5]MCSPI,[6]MMCSD,[7]RTC,[8]TIMER,[9]TSC,[10]UART,[11]USB,[12]WDT,[13]PRODUT_ID,[14]LCD_CONTROLLER;\n",argv[0]);
		exit(1);
	}

        int n = atoi(argv[1]);
	int processor = read_processor();	// which processor we are working on

#ifdef _OMAP4_
	if( processor == OMAP4 ) {
		switch(n) {
			case PRODUCT_ID:
				// base 0x4A002000
				num_regs = ARRAY_SIZE(struct reg_info, omap44x_product_id_registers);
				printf("\n--------------Product ID Registers-----------------:\n");
				show_registers(omap44x_product_id_registers,num_regs,0x4A002000);
				break;

			case LCD_CONTROLLER:
				// base 0x48041000
				num_regs = ARRAY_SIZE(struct reg_info, omap44x_lcd_controller_registers);
				printf("\n------------LCD Controller Registers-------------------:\n");
				show_registers(omap44x_lcd_controller_registers,num_regs,0x48041000);
				break;

			case DCAN:
				printf("Need to write structure for DCAN\n");
				break;

			case GPIO:
				printf("Need to write structure for GPIO\n");
				break;

			case I2C:
				/* bases
				* i2c3 0x48060000
				* i2c1 0x48070000
				* i2c2 0x48072000
				* i2c4 0x48350000
				*/
				num_regs = ARRAY_SIZE(struct reg_info, omap44x_i2c_registers);
				printf("\nI2C Registers:\n");
				printf("Which one of the four I2Cs?? [1,2,3,4]:\n");
				scanf("%d",&choice);

				switch (choice) {
					case 1:
					printf("------------------ I2C1 REGISTERS----------------\n");
					show_registers(omap44x_i2c_registers,num_regs,0x48070000);
					break;

					case 2:
					printf("------------------ I2C2 REGISTERS----------------\n");
					show_registers(omap44x_i2c_registers,num_regs,0x48072000);
					break;

					case 3:
					printf("------------------ I2C3 REGISTERS----------------\n");
					show_registers(omap44x_i2c_registers,num_regs,0x48060000);
					break;

					case 4:
					printf("------------------ I2C4 REGISTERS----------------\n");
					show_registers(omap44x_i2c_registers,num_regs,0x48350000);
					break;

					default:
						printf("Wrong choice\n");
				}
				break;

			case LCD:
				// base 0x48040000
				num_regs = ARRAY_SIZE(struct reg_info, omap44x_lcd_registers);
				printf("\n------------ LCD Registers -----------------:\n");
				show_registers(omap44x_lcd_registers,num_regs,0x48040000);
				break;

			case MCASP:
				// base 0x49028000
				num_regs = ARRAY_SIZE(struct reg_info, omap44x_mcasp_registers);
				printf("\n-------------- McASP Registers----------------:\n");
				show_registers(omap44x_mcasp_registers,num_regs,0x49028000);
				break;

			 case MCSPI:
				/* bases
				* MCSPI1	0x48098000
				* MCSPI2	0x4809A000
				* MCSPI3	0x480B8000
				* MCSPI4	0x480BA000
				*/
				num_regs = ARRAY_SIZE(struct reg_info, omap44x_mcspi_registers);
				printf("\nMcSPI Registers:\n");
				printf("Which one of the four McSPIs?? [1,2,3,4]:\n");
				scanf("%d",&choice);

				switch (choice) {
					case 1:
					printf("------------------ MCSPI1 REGISTERS----------------\n");
					show_registers(omap44x_mcspi_registers,num_regs,0x48098000);
					break;

					case 2:
					printf("------------------ MCSPI2 REGISTERS----------------\n");
					show_registers(omap44x_mcspi_registers,num_regs,0x4809A000);
					break;

					case 3:
					printf("------------------ MCSPI3 REGISTERS----------------\n");
					show_registers(omap44x_mcspi_registers,num_regs,0x480B8000);
					break;

					case 4:
					printf("------------------ MCSPI4 REGISTERS----------------\n");
					show_registers(omap44x_mcspi_registers,num_regs,0x480BA000);
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
				num_regs = ARRAY_SIZE(struct reg_info, omap44x_mmchs_registers);
				printf("\nMMC/SD/SDIO Registers:\n");
				printf("Which one of the 5 MMCHSs?? [1,2,3,4,5]:\n");
				scanf("%d",&choice);

				switch (choice) {
					case 1:
					printf("------------------ MMCHS1 REGISTERS----------------\n");
					show_registers(omap44x_mmchs_registers,num_regs,0x4809C000);
					break;

					case 2:
					printf("------------------ MMCHS2 REGISTERS----------------\n");
					show_registers(omap44x_mmchs_registers,num_regs,0x480B4000);
					break;

					case 3:
					printf("------------------ MMCHS3 REGISTERS----------------\n");
					show_registers(omap44x_mmchs_registers,num_regs,0x480AD000);
					break;

					case 4:
					printf("------------------ MMCHS4 REGISTERS----------------\n");
					show_registers(omap44x_mmchs_registers,num_regs,0x480D1000);
					break;

					case 5:
					printf("------------------ MMCHS5 REGISTERS----------------\n");
					show_registers(omap44x_mmchs_registers,num_regs,0x480D5000);
					break;

					default:
						printf("Wrong choice\n");
				}
				break;

			case RTC:
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
				num_regs = ARRAY_SIZE(struct reg_info, omap44x_uart_registers);
				printf("\nUART Registers:\n");
				printf("Which one of the four UARTs?? [1,2,3,4]:\n");
				scanf("%d",&choice);

				switch (choice) {
					case 1:
					printf("------------------ UART1 REGISTERS----------------\n");
					show_registers(omap44x_uart_registers,num_regs,0x4806A000);
					break;

					case 2:
					printf("------------------ UART2 REGISTERS----------------\n");
					show_registers(omap44x_uart_registers,num_regs,0x4806C000);
					break;

					case 3:
					printf("------------------ UART3 REGISTERS----------------\n");
					show_registers(omap44x_uart_registers,num_regs,0x48020000);
					break;

					case 4:
					printf("------------------ UART4 REGISTERS----------------\n");
					show_registers(omap44x_uart_registers,num_regs,0x4806E000);
					break;

					default:
						printf("Wrong choice\n");
				}
				break;

	 		case USB:
				break;

			case WDT:
                                /*bases
				WDT2   0x4A314000
				WDT3   0x49030000
				*/ 
				num_regs = ARRAY_SIZE(struct reg_info, omap44x_wdt_registers);
				printf("\nWDT Registers:\n");
				printf("Which one of the two WDT's?? [1,2]:\n");
				scanf("%d",&choice);

				switch (choice) {
					case 1:
					printf("------------------ WDT2 REGISTERS----------------\n");
					show_registers(omap44x_wdt_registers,num_regs,0x4A314000);
					break;

					case 2:
					printf("------------------ WDT3 REGISTERS----------------\n");
					show_registers(omap44x_wdt_registers,num_regs,0x49030000);
					break;

					default:
						printf("Wrong choice\n");
				}	                                
					break;
				
			case TIMER:
				/*General Purpose Timers(GPT) bases
				GPTIMER1   0x4A318000
				GPTIMER2   0x48032000
				GPTIMER3   0x48034000
				GPTIMER4   0x48036000
				GPTIMER5   0x49038000
				GPTIMER6   0x4903A000
				GPTIMER7   0x4903C000
				GPTIMER8   0x4903E000
				GPTIMER9   0x4803E000
				GPTIMER10  0x48086000
				GPTIMER11  0x48088000
				*/
				num_regs = ARRAY_SIZE(struct reg_info, omap44x_gpt_registers);
				printf("\nGeneral Purpose Timers Registers:\n");
				printf("Which one of the eleven GPT's?? [1,2,3,4,5,6,7,8,9,10,11]:\n");
				scanf("%d",&choice);

				switch (choice) {
					case 1:
					printf("------------------ GPT1 REGISTERS----------------\n");
					show_registers(omap44x_gpt_registers,num_regs,0x4A318000);
					break;

					case 2:
					printf("------------------ GPT2 REGISTERS----------------\n");
					show_registers(omap44x_gpt_registers,num_regs,0x48032000);
					break;

					case 3:
					printf("------------------ GPT3 REGISTERS----------------\n");
					show_registers(omap44x_gpt_registers,num_regs,0x48034000);
					break;

					case 4:
					printf("------------------ GPT4 REGISTERS----------------\n");
					show_registers(omap44x_gpt_registers,num_regs,0x48036000);
					break;

					case 5:
					printf("------------------ GPT5 REGISTERS----------------\n");
					show_registers(omap44x_gpt_registers,num_regs,0x49038000);
					break;

					case 6:
					printf("------------------ GPT6 REGISTERS----------------\n");
					show_registers(omap44x_gpt_registers,num_regs,0x4903A000);
					break;
					
					case 7:
					printf("------------------ GPT7 REGISTERS----------------\n");
					show_registers(omap44x_gpt_registers,num_regs,0x4903C000);
					break;

					case 8:
					printf("------------------ GPT8 REGISTERS----------------\n");
					show_registers(omap44x_gpt_registers,num_regs,0x4903E000);
					break;

					case 9:
					printf("------------------ GPT9 REGISTERS----------------\n");
					show_registers(omap44x_gpt_registers,num_regs,0x48086000);
					break;

					case 10:
					printf("------------------ GPT10 REGISTERS---------------\n");
					show_registers(omap44x_gpt_registers,num_regs,0x48088000);
					break;

					case 11:
					printf("------------------ GPT11 REGISTERS---------------\n");
					show_registers(omap44x_gpt_registers,num_regs,0x49038000);
					break;

					default:
						printf("Wrong choice\n");
				}
				break;

			default:
				printf("Invalid section\n");
		}
	}
#endif		// _OMAP4_

#ifdef _AM335x_
	if( processor == AM335x ) {
		switch (n) {
			case DCAN  :
			  	    printf("------------------- DCAN REGISTERS---------------- "); 
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_dcan_registers);
				    /*base addresses 
				    DCAN0 0x481CC000
				    DCAN1 0X481D0000
				    */	
		                    printf("Which one of the DCAN's?? [1,2]:\n");
				    scanf("%d",&choice);

				    switch (choice) {
					    case 1:
					    printf("--------------------- DCAN0 REGISTERS------------------- \n");
					    show_registers(am335x_dcan_registers,num_regs,0x481CC000);
					    break;

					    case 2:
					    printf("-------------------- DCAN1 REGISTERS-------------------- \n"); 				
				       	    show_registers(am335x_dcan_registers,num_regs,0x481D0000);
					    break;

					    default:
					    printf("Wrong choice\n");
				    }
	 		            break;
			 
			 case GPIO  :
				    /* base addresses
				    GPIO0   0x44E07000
				    GPIO1   0x4804C000
				    GPIO2   0x481AC000
				    GPIO3   0x481AE000
				    */
				    printf("------------------- GPIO REGISTERS---------------- \n");
			            num_regs = ARRAY_SIZE(struct reg_info, am335x_gpio_registers);
	    		            printf("Which one of the GPIO's?? [1,2,3,4]:\n");
				    scanf("%d",&choice);

				    switch (choice) {
				            case 1:
					    printf("--------------------- GPIO0 REGISTERS-------------------\n "); 
					    show_registers(am335x_gpio_registers,num_regs,0x44E07000);
					    break;

				            case 2:
					    printf("--------------------- GPIO1 REGISTERS------------------- \n"); 				
					    show_registers(am335x_gpio_registers,num_regs,0x4804C000);
					    break;
					
					    case 3:
					    printf("--------------------- GPIO2 REGISTERS------------------- \n"); 				
					    show_registers(am335x_gpio_registers,num_regs,0x480AC000);
					    break;

					    case 4:
					    printf("--------------------- GPIO3 REGISTERS------------------- \n"); 				
					    show_registers(am335x_gpio_registers,num_regs,0x480AE000);
					    break;

					    default:
					    printf("Wrong choice\n");
				    }
				    break;

			 case I2C   :
				    /*base addresses
				    I2C0   0x44E0B000
				    I2C1   0x4802A000
				    I2C2   0x4819C000
				    */				
				    printf("------------------ I2C REGISTERS----------------- ");
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_i2c_registers);
				    printf("Which one of the I2C's?? [1,2,3]:\n");
				    scanf("%d",&choice);

				    switch (choice) {
					    case 1:
					    printf("-------------------- I2C0 REGISTERS-------------------- \n"); 
					    show_registers(am335x_i2c_registers,num_regs,0x44E0B000);
					    break;

				            case 2:
					    printf("-------------------- I2C1 REGISTERS-------------------- \n"); 				
					    show_registers(am335x_i2c_registers,num_regs,0x4802A000);
					    break;
					
					    case 3:
					    printf("-------------------- I2C2 REGISTERS-------------------- \n"); 				
					    show_registers(am335x_i2c_registers,num_regs,0x4819C000);
					    break;

					    default:
					    printf("Wrong choice\n");
				    }
				    break;
			
			 case LCD_CONTROLLER:
				    // base address LCD_CONTROLLER  0x4830E000
				    printf("------------------- LCD CONTROLLER REGISTERS---------------------- \n");
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_lcd_controller_registers);		
				    show_registers(am335x_lcd_controller_registers,num_regs,0x4830E000);
				    break;

			 case MCASP:
				    /* base addresses
				    MCASP0   0x48038000
				    MCASP1   0X4803C000
				    */
				    printf("------------------- MCASP REGISTERS---------------- \n");
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_mcasp_registers);
				    printf("Which one of the MCASP's?? [1,2]:\n");
				    scanf("%d",&choice);

				    switch (choice) {
					    case 1:
					    printf("----------------------- MCASP0 REGISTERS----------------- \n"); 
					    show_registers(am335x_mcasp_registers,num_regs,0x48038000);
					    break;

					    case 2:
					    printf("----------------------- MCASP1 REGISTERS----------------- \n"); 						    show_registers(am335x_mcasp_registers,num_regs,0x4803C000);
					    break;

					    default:
					    printf("Wrong choice\n");
				    }
				    break;

			 case MCSPI :
				    /*base addresses
				    MCSPI0   0x48030000
				    MCSPI1   0x481A0000
				    */
				    printf("-------------- MCSPI REGISTERS------------- \n");
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_mcspi_registers);	
				    printf("Which one of the MCSPI's?? [1,2]:\n");
				    scanf("%d",&choice);

				    switch (choice) {
					    case 1:
					    printf("--------------------- MCSPI0 REGISTERS------------------- \n"); 
					    show_registers(am335x_mcspi_registers,num_regs,0x48030000);
					    break;

				            case 2:
					    printf("--------------------- MCSPI1 REGISTERS------------------- \n"); 				
					    show_registers(am335x_mcspi_registers,num_regs,0x481A0000);
					    break;

				   	    default:
					    printf("Wrong choice\n");
					}
				    break;

			 case MMCSD :
				    /*
				    base addresses 
				    MMCHS0   0x48060000
				    MMC1     0x481D8000
				    MMCHS2   0x47810000
				    */
				    printf("-------------- MMC/SD REGISTERS------------- \n");
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_mmchs_registers);
		                    printf("Which one of the MMCHS's?? [1,2,3]:\n");
				    scanf("%d",&choice);

				    switch (choice) {
				            case 1:
					    printf("--------------------- MMCHS0 REGISTERS------------------- \n"); 
					    show_registers(am335x_mmchs_registers,num_regs,0x48060000);
					    break;

					    case 2:
					    printf("--------------------- MMC1 REGISTERS--------------------- \n"); 				
					    show_registers(am335x_mmchs_registers,num_regs,0x481D8000);
					    break;
					
					    case 3:
					    printf("--------------------- MMCHS2 REGISTERS------------------- \n"); 				
					    show_registers(am335x_mmchs_registers,num_regs,0x47810000);
					    break;

					    default:
					    printf("Wrong choice\n");
				    }
				    break;

			 case RTC   :
				    // base address RTCSS   0x44E3E000
				    printf("-------------------- RTC REGISTERS--------------------- \n");
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_rtc_registers);
		                    show_registers(am335x_rtc_registers,num_regs,0x44E3E000);
				    break;

			 case TIMER :
				    /*
				    base addresses
				    DMTIMER0   0x44E05000
				    DMTIMER1   0x44E31000 
				    DMTIMER2   0x48040000
				    DMTIMER3   0x48042000
				    DMTIMER4   0x48044000
				    DMTIMER5   0x48046000
				    DMTIMER6   0x48048000
				    DMTIMER7   0x4804A000
				    */
				    printf("---------------- TIMER REGISTERS------------------- ");
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_timer_registers);
		                    printf("Which one of the TIMER's?? [1,2,3,4,5,6,7,8]:\n");
				    scanf("%d",&choice);

				    switch (choice) {
				            case 1:
					    printf("-------------------- TIMER0 REGISTERS-------------------- "); 
					    show_registers(am335x_timer_registers,num_regs,0x44E05000);
					    break;

					    case 2:
					    printf("-------------------- TIMER1 REGISTERS-------------------- "); 				
					    show_registers(am335x_timer_registers,num_regs,0x48E31000);
					    break;
				
			    		    case 3:
					    printf("-------------------- TIMER2 REGISTERS-------------------- ");			
					    show_registers(am335x_timer_registers,num_regs,0x48040000);
					    break;

					    case 4:
					    printf("-------------------- TIMER3 REGISTERS-------------------- ");			
					    show_registers(am335x_timer_registers,num_regs,0x48042000);
					    break;

					    case 5:
					    printf("-------------------- TIMER4 REGISTERS-------------------- ");			
					    show_registers(am335x_timer_registers,num_regs,0x48044000);
					    break;

					    case 6:
					    printf("-------------------- TIMER5 REGISTERS-------------------- ");			
					    show_registers(am335x_timer_registers,num_regs,0x48046000);
					    break;

					    case 7:
					    printf("-------------------- TIMER6 REGISTERS-------------------- "); 				
				  	    show_registers(am335x_timer_registers,num_regs,0x48048000);
					    break;

					    case 8:
					    printf("-------------------- TIMER7 REGISTERS-------------------- "); 				
					    show_registers(am335x_timer_registers,num_regs,0x4804A000);
					    break;

					    default:
					    printf("Wrong choice\n");
				    }
				    break;
		 
			 case TSC   :
				    // base address ADC_TSC   0x44E0D000
				    printf("-------------------- TSC REGISTERS--------------------- ");
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_tsc_registers);
		                    show_registers(am335x_tsc_registers,num_regs,0x44E0D000);
				    break;

		         case UART  :
				    /*
				    base addresses
				    UART0   0x44E09000
				    UART1   0x48022000
				    UART2   0x48024000
				    UART3   0x481A6000
				    UART4   0x481A8000
				    UART5   0x481AA000
				    */
				    printf("-------------------- UART REGISTERS--------------- ");
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_uart_registers);
		 	            printf("Which one of the UART's?? [1,2,3,4,5,6]:\n");
				    scanf("%d",&choice);

				    switch (choice) {
					    case 1:
					    printf("----------------------- UART0 REGISTERS----------------- "); 
					    show_registers(am335x_uart_registers,num_regs,0x44E09000);
					    break;

					    case 2:
					    printf("----------------------- UART1 REGISTERS----------------- ");  				
					    show_registers(am335x_uart_registers,num_regs,0x48022000);
					    break;
					
					    case 3:
					    printf("----------------------- UART2 REGISTERS----------------- ");  				
					    show_registers(am335x_uart_registers,num_regs,0x48024000);
					    break;

					    case 4:
					    printf("----------------------- UART3 REGISTERS----------------- "); 				
					    show_registers(am335x_uart_registers,num_regs,0x481A6000);
					    break;

					    case 5:
					    printf("----------------------- UART4 REGISTERS----------------- "); 				
					    show_registers(am335x_uart_registers,num_regs,0x481A8000);
					    break;

					    case 6:
					    printf("----------------------- UART5 REGISTERS----------------- "); 				
					    show_registers(am335x_uart_registers,num_regs,0x481AA000);
					    break;

					    default:
					    printf("Wrong choice\n");
				    }
				    break;

			 case USB   :
				    // base address USBSS   0x47400000
				    printf("---------------------- USBSS REGISTERS----------------- ");
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_usb_registers);
				    show_registers(am335x_usb_registers,num_regs,0x47400000);
				    break;

			 case WDT   :
				    // base addresses WDT1   0x44E35000 
				    printf("---------------- WDT1 REGISTERS------------------------ ");
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_wdt_registers);
		                    show_registers(am335x_wdt_registers,num_regs,0x44E35000);
				    break;
		
			 case PRODUCT_ID:
				    // base 0x4A002000
				    num_regs = ARRAY_SIZE(struct reg_info, am335x_product_id_registers);
				    printf("\n-----------------Product ID Registers:---------------------\n");
				    show_registers(am335x_product_id_registers,num_regs,0x44E10600);
				    break;
			 case LCD   :
  				    break;
		     
		  	 default   :
			 	    printf("Invalid Selection" );
		}
	}
#endif	// _AM335x_

#ifdef _OMAP35x_
	if( processor == OMAP35x ) {
		switch(n) {
			case PRODUCT_ID:
					// base 0x4830A204
					num_regs = ARRAY_SIZE(struct reg_info, omap35x_product_id_registers);
					printf("\n-------------------Product ID Registers:--------------------\n");
					show_registers(omap35x_product_id_registers,num_regs,0x4830A204);
					break;

			case LCD_CONTROLLER:
					// base 0x48050400
					num_regs = ARRAY_SIZE(struct reg_info, omap35x_lcd_controller_registers);
					printf("\n----------------LCD Controller Registers:-----------------------\n");
					show_registers(omap35x_lcd_controller_registers,num_regs,0x48050400);
					break;

			case DCAN:
					printf("DCAN not available on the Processor\n");
					break;

			case GPIO:
					printf("GPIO not available on the Processor\n");
					break;

			case I2C:
					/* base addresses
					I2C1   0x48070000  
					I2C2   0x48072000 
					I2C3   0x48060000
					*/
					num_regs = ARRAY_SIZE(struct reg_info, omap35x_i2c_registers);
					printf("\nI2C Registers:\n");
					printf("Which one of the three I2Cs?? [1,2,3]:\n");
					scanf("%d",&choice);

					switch (choice) {
						case 1:
						printf("-------------------- I2C0 REGISTERS----------------------"); 
						show_registers(omap35x_i2c_registers,num_regs,0x48070000);
						break;

						case 2:
						printf("------------------- I2C1 REGISTERS-----------------------"); 
						show_registers(omap35x_i2c_registers,num_regs,0x48072000);
						break;

						case 3:
						printf("-------------------- I2C2 REGISTERS----------------------"); 
						show_registers(omap35x_i2c_registers,num_regs,0x48060000);
						break;

						default:
							printf("Wrong choice\n");
					}
					break;

			case LCD:
					// base 0x48050000
					num_regs = ARRAY_SIZE(struct reg_info, omap35x_lcd_registers);
					printf("\nLCD Registers:\n");
					show_registers(omap35x_lcd_registers,num_regs,0x48050000);
					break;

			case MCBSP:
					/* base addresses
					McBSP1         	  0x48074000 
					McBSP2 		  0x49022000 
					McBSP3 		  0x49024000 
					McBSP4 		  0x49026000 
					McBSP5 		  0x48096000 
					SIDETONE_McBSP2   0x49028000 
					SIDETONE_McBSP3   0x4902A000
					*/
					num_regs = ARRAY_SIZE(struct reg_info, omap35x_mcbsp_registers);
					printf("\nMcBSP Registers:\n");
					printf("Which one of the seven McBSP's?? [1,2,3,4,5,6,7]:\n");
					scanf("%d",&choice);

					switch (choice) {
						case 1:
						printf("-------------------- MCBSP1 REGISTERS----------------------"); 
						show_registers(omap35x_mcbsp_registers,num_regs,0x48074000);
						break;

						case 2:
						printf("------------------- MCBSP2 REGISTERS-----------------------"); 
						show_registers(omap35x_mcbsp_registers,num_regs,0x49022000);
						break;

						case 3:
						printf("-------------------- MCBSP3 REGISTERS----------------------"); 
						show_registers(omap35x_mcbsp_registers,num_regs,0x49024000);
						break;

						case 4:
						printf("-------------------- MCBSP4 REGISTERS----------------------"); 
						show_registers(omap35x_mcbsp_registers,num_regs,0x49026000);
						break;

						case 5:
						printf("------------------- MCBSP5 REGISTERS-----------------------"); 
						show_registers(omap35x_mcbsp_registers,num_regs,0x48096000);
						break;

						case 6:
						printf("----------------- SIDETONE_MCBSP2 REGISTERS----------------"); 
						show_registers(omap35x_mcbsp_registers,num_regs,0x49028000);
						break;

						case 7:
						printf("----------------- SIDETONE_MCBSP3 REGISTERS----------------"); 
						show_registers(omap35x_mcbsp_registers,num_regs,0x4902A000);
						break;

						default:
							printf("Wrong choice\n");
					}
					break;

			 case MCSPI:
					/* base addresses 
					MCSPI1   0x48098000 
					MCSPI2   0x4809A000 
					MCSPI3   0x480B8000 
					MCSPI4   0x480BA000
					*/
					num_regs = ARRAY_SIZE(struct reg_info, omap35x_mcspi_registers);
					printf("\nMcSPI Registers:\n");
					printf("Which one of the four McSPIs?? [1,2,3,4]:\n");
					scanf("%d",&choice);

					switch (choice) {
						case 1:
						printf("--------------------- MCSPI1 REGISTERS---------------------"); 
						show_registers(omap35x_mcspi_registers,num_regs,0x48098000);
						break;

						case 2:
						printf("--------------------- MCSPI2 REGISTERS---------------------"); 
						show_registers(omap35x_mcspi_registers,num_regs,0x4809A000);
						break;

						case 3:
						printf("--------------------- MCSPI3 REGISTERS---------------------"); 
						show_registers(omap35x_mcspi_registers,num_regs,0x480B8000);
						break;

						case 4:
						printf("--------------------- MCSPI4 REGISTERS---------------------"); 
						show_registers(omap35x_mcspi_registers,num_regs,0x480BA000);
						break;

						default:
							printf("Wrong choice\n");
					}
					break;

			 case MMCSD:
					/* base addresses 
					MMCHS1   0x4809C000 
					MMCHS2   0x480B4000  
					MMCHS3   0x480AD000
					*/
					num_regs = ARRAY_SIZE(struct reg_info, omap35x_mmchs_registers);
					printf("\nMMC/SD/SDIO Registers:\n");
					printf("Which one of the 3 MMCHSs?? [1,2,3]:\n");
					scanf("%d",&choice);

					switch (choice) {
						case 1:
						printf("--------------------- MMCHS1 REGISTERS---------------------");
						show_registers(omap35x_mmchs_registers,num_regs,0x4809C000);
						break;

						case 2:
						printf("--------------------- MMCHS2 REGISTERS---------------------");
						show_registers(omap35x_mmchs_registers,num_regs,0x480B4000);
						break;

						case 3:
						printf("--------------------- MMCHS3 REGISTERS---------------------");
						show_registers(omap35x_mmchs_registers,num_regs,0x480AD000);
						break;

						default:
							printf("Wrong choice\n");
					}
					break;
			 case RTC:
 				        break;
			 case TSC:
				    	break;

			 case UART:
					/* base addresses
					UART1   0x4806A000
					UART2   0x4806C000
					UART3   0x49020000
					*/
					num_regs = ARRAY_SIZE(struct reg_info, omap35x_uart_registers);
					printf("\nUART Registers:\n");
					printf("Which one of the three UARTs?? [1,2,3]:\n");
					scanf("%d",&choice);

					switch (choice) {
						case 1:
						printf("--------------------- UART1 REGISTERS---------------------");
						show_registers(omap35x_uart_registers,num_regs,0x4806A000);
						break;

						case 2:
						printf("--------------------- UART2 REGISTERS---------------------");
						show_registers(omap35x_uart_registers,num_regs,0x4806C000);
						break;

						case 3:
						printf("--------------------- UART3 REGISTERS---------------------");
						show_registers(omap35x_uart_registers,num_regs,0x49020000);
						break;

						default:
							printf("Wrong choice\n");
					}
					break;

	 		case USB:
					// base address USBTLL   0x48062000
					num_regs = ARRAY_SIZE(struct reg_info, omap35x_usbttlhs_config_registers);
					printf("\n----------------------USBTLL Registers:--------------------------\n");
					show_registers(omap35x_usbttlhs_config_registers,num_regs,0x48062000);
					break;				

			case WDT:
					/* base addresses
					WDTIMER2   0x48314000 
					WDTIMER3   0x49030000
					*/
					num_regs = ARRAY_SIZE(struct reg_info, omap35x_wdt_registers);
					printf("\nWDT Registers:\n");
					printf("Which one of the two WDT's?? [1,2]:\n");
					scanf("%d",&choice);

					switch (choice) {
						case 1:
						printf("--------------------- WDT2 REGISTERS---------------------");
						show_registers(omap35x_wdt_registers,num_regs,0x48314000);
						break;

						case 2:
						printf("--------------------- WDT3 REGISTERS---------------------");
						show_registers(omap35x_wdt_registers,num_regs,0x49030000);
						break;

						default:
							printf("Wrong choice\n");
					}	                                
						break;
				
			case TIMER:
					/* base addresses
					GPTIMER1   0x48318000 
					GPTIMER2   0x49032000 
					GPTIMER3   0x49034000 
					GPTIMER4   0x49036000 
					GPTIMER5   0x49038000 
					GPTIMER6   0x4903A000 
					GPTIMER7   0x4903C000 
					GPTIMER8   0x4903E000 
					GPTIMER9   0x49040000
					GPTIMER10  0x48086000 
					GPTIMER11  0x48088000
					*/
					num_regs = ARRAY_SIZE(struct reg_info, omap35x_gpt_registers);
					printf("\nGeneral Purpose Timers Registers:\n");
					printf("Which one of the eleven GPT's?? [1,2,3,4,5,6,7,8,9,10,11]:\n");
					scanf("%d",&choice);

					switch (choice) {
						case 1:
						printf("--------------------- GPT1 REGISTERS---------------------");
						show_registers(omap35x_gpt_registers,num_regs,0x48318000);
						break;

						case 2:
						printf("--------------------- GPT2 REGISTERS---------------------");
						show_registers(omap35x_gpt_registers,num_regs,0x49032000);
						break;

						case 3:
						printf("--------------------- GPT3 REGISTERS---------------------");
						show_registers(omap35x_gpt_registers,num_regs,0x49034000);
						break;

						case 4:
						printf("--------------------- GPT4 REGISTERS---------------------");
						show_registers(omap35x_gpt_registers,num_regs,0x49036000);
						break;

						case 5:
						printf("--------------------- GPT5 REGISTERS---------------------");
						show_registers(omap35x_gpt_registers,num_regs,0x49038000);
						break;

						case 6:
						printf("--------------------- GPT6 REGISTERS---------------------");
						show_registers(omap35x_gpt_registers,num_regs,0x4903A000);
						break;
					
						case 7:
						printf("--------------------- GPT7 REGISTERS---------------------");
						show_registers(omap35x_gpt_registers,num_regs,0x4903C000);
						break;

						case 8:
						printf("--------------------- GPT8 REGISTERS---------------------");
						show_registers(omap35x_gpt_registers,num_regs,0x4903E000);
						break;

						case 9:
						printf("--------------------- GPT9 REGISTERS---------------------");
						show_registers(omap35x_gpt_registers,num_regs,0x49040000);
						break;

						case 10:
						printf("--------------------- GPT10 REGISTERS--------------------");
						show_registers(omap35x_gpt_registers,num_regs,0x48086000);
						break;

						case 11:
						printf("--------------------- GPT11 REGISTERS--------------------");
						show_registers(omap35x_gpt_registers,num_regs,0x48088000);
						break;

						default:
							printf("Wrong choice\n");
					}
					break;

			default:
				printf("Invalid section\n");
		}
	}
#endif 		// _OMAP35x_

	else {
		printf("Processor not supported\n");
	}

	return 0;
}
