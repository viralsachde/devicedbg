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
  
#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)
 
#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)


//#define CPUINFO_FILE "panda_cpuinfo"
//#define CPUINFO_FILE "bone_cpuinfo"
#define CPUINFO_FILE "/proc/cpuinfo"

#define OMAP4		0
#define AM335x		1

#define DCAN   0
#define GPIO   1
#define I2C    2
#define LCD    3
#define MCASP  4
#define MCSPI  5
#define MMCSD  6
#define TSC    7
#define UART   8
#define USB    9
#define PRODUCT_ID	10
#define LCD_CONTROLLER	11

// [0]DCAN,[1]GPIO,[2]I2C,[3]LCD,[4]MCASP,[5]MCSPI,[6]MMCSD,[7]TSC*,[8]UART,[9]USB*,[10]PRODUT_ID,[11]LCD_CONTROLLER; * are not implemented yet

// representation of a register
struct reg_info {
	unsigned long offset;
	unsigned long old_value;
	unsigned long new_value;
	char name[25];
};

// base 0x4A002000
static struct reg_info product_id_registers[] = {
	{ 0x200, 0x0, 0x0, "STD_FUSE_DIE_ID_0" },
	{ 0x204, 0x0, 0x0, "ID_CODE" },
	{ 0x208, 0x0, 0x0, "STD_FUSE_DIE_ID_1" },
	{ 0x20C, 0x0, 0x0, "STD_FUSE_DIE_ID_2" },
	{ 0x210, 0x0, 0x0, "STD_FUSE_DIE_ID_3" },
	{ 0x214, 0x0, 0x0, "STD_FUSE_PROD_ID_0" },
	{ 0x218, 0x0, 0x0, "STD_FUSE_PROD_ID_1" },
};

// base 0x48040000
static struct reg_info lcd_registers[] = {
	{ 0x00, 0x0, 0x0, "DSS_REVISION" },
	{ 0x10, 0x0, 0x0, "RESERVED" },
	{ 0x14, 0x0, 0x0, "DSS_SYSSTATUS" },
	{ 0x40, 0x0, 0x0, "DSS_CTRL" },
	{ 0x5c, 0x0, 0x0, "DSS_STATUS" }
};

// base 0x48041000
static struct reg_info lcd_controller_registers[] = {
	{ 0x000, 0x0, 0x0, "DISPC_REVISION" },
	{ 0x010, 0x0, 0x0, "DISPC_SYSCONFIG" },
	{ 0x014, 0x0, 0x0, "DISPC_SYSSTATUS" },
	{ 0x018, 0x0, 0x0, "DISPC_IRQSTATUS" },
	{ 0x01C, 0x0, 0x0, "DISPC_IRQENABLE" },
	{ 0x040, 0x0, 0x0, "DISPC_CONTROL1" },
	{ 0x044, 0x0, 0x0, "DISPC_CONFIG1" },
	{ 0x048, 0x0, 0x0, "RESERVED" },
	{ 0x04C, 0x0, 0x0, "DISPC_DEFAULT_COLOR0" },
	{ 0x050, 0x0, 0x0, "DISPC_DEFAULT_COLOR1" },
	{ 0x054, 0x0, 0x0, "DISPC_TRANS_COLOR0" },
	{ 0x058, 0x0, 0x0, "DISPC_TRANS_COLOR1" },
	{ 0x05C, 0x0, 0x0, "DISPC_LINE_STATUS" },
	{ 0x060, 0x0, 0x0, "DISPC_LINE_NUMBER" },
	{ 0x064, 0x0, 0x0, "DISPC_TIMING_H1" },
	{ 0x068, 0x0, 0x0, "DISPC_TIMING_V1" },
	{ 0x06C, 0x0, 0x0, "DISPC_POL_FREQ1" },
	{ 0x070, 0x0, 0x0, "DISPC_DIVISOR1" },
	{ 0x074, 0x0, 0x0, "DISPC_GLOBAL_ALPHA" },
	{ 0x078, 0x0, 0x0, "DISPC_SIZE_TV" },
	{ 0x07C, 0x0, 0x0, "DISPC_SIZE_LCD1" },
	{ 0x088, 0x0, 0x0, "DISPC_GFX_POSITION" },
	{ 0x08C, 0x0, 0x0, "DISPC_GFX_SIZE" },
	{ 0x0A0, 0x0, 0x0, "DISPC_GFX_ATTRIBUTES" },
	{ 0x0A4, 0x0, 0x0, "DISPC_GFX_BUF_THRESHOLD" },
	{ 0x0A8, 0x0, 0x0, "DISPC_GFX_BUF_SIZE_STATUS" },
	{ 0x0AC, 0x0, 0x0, "DISPC_GFX_ROW_INC" },
	{ 0x0B0, 0x0, 0x0, "DISPC_GFX_PIXEL_INC" },
	{ 0x0B4, 0x0, 0x0, "RESERVED" },
	{ 0x0B8, 0x0, 0x0, "DISPC_GFX_TABLE_BA" }
};

/* bases
* i2c3 0x48060000
* i2c1 0x48070000
* i2c2 0x48072000
* i2c4 0x48350000
*/
static struct reg_info i2c_registers[] = {
	{ 0x00, 0x0, 0x0, "I2C_REVNB_LO" },
	{ 0x04, 0x0, 0x0, "I2C_REVNB_HI" },
	{ 0x10, 0x0, 0x0, "I2C_SYS" },
	{ 0x20, 0x0, 0x0, "RESERVED" },
	{ 0x24, 0x0, 0x0, "I2C_IRQSTATUS_RAW" },
	{ 0x28, 0x0, 0x0, "I2C_IRQSTATUS" },
	{ 0x2C, 0x0, 0x0, "I2C_RQENABLE_SET" },
	{ 0x30, 0x0, 0x0, "I2C_IRQENABLE_CLR" },
	{ 0x34, 0x0, 0x0, "I2C_WE" },
	{ 0x38, 0x0, 0x0, "I2C_DMARXENABLE_SET" },
	{ 0x3C, 0x0, 0x0, "I2C_DMATXENABLE_SET" },
	{ 0x40, 0x0, 0x0, "I2C_DMARXENABLE_CLR" },
	{ 0x44, 0x0, 0x0, "I2C_DMATXENABLE_CLR" },
	{ 0x48, 0x0, 0x0, "I2C_DMARXWAKE_EN" },
	{ 0x4C, 0x0, 0x0, "I2C_DMATXWAKE_EN" },
	{ 0x84, 0x0, 0x0, "I2C_IE" },
	{ 0x88, 0x0, 0x0, "I2C_STAT" },
	{ 0x90, 0x0, 0x0, "I2C_SYSS" },
	{ 0x94, 0x0, 0x0, "I2C_BUF" },
	{ 0x98, 0x0, 0x0, "I2C_CNT" },
	{ 0x9C, 0x0, 0x0, "I2C_DATA" },
	{ 0xA4, 0x0, 0x0, "I2C_CON" },
	{ 0xA8, 0x0, 0x0, "I2C_OA" },
	{ 0xAC, 0x0, 0x0, "I2C_SA" },
	{ 0xB0, 0x0, 0x0, "I2C_PSC" },
	{ 0xB4, 0x0, 0x0, "I2C_SCLL" },
	{ 0xB8, 0x0, 0x0, "I2C_SCLH" },
	{ 0xBC, 0x0, 0x0, "I2C_SYSTEST" },
	{ 0xC0, 0x0, 0x0, "I2C_BUFSTAT" },
	{ 0xC4, 0x0, 0x0, "I2C_OA1" },
	{ 0xC8, 0x0, 0x0, "I2C_OA2" },
	{ 0xCC, 0x0, 0x0, "I2C_OA3" },
	{ 0xD0, 0x0, 0x0, "I2C_ACTOA" },
	{ 0xD4, 0x0, 0x0, "I2C_SBLOCK" }
};

/* bases
* uart1 0x4806A000
* uart2 0x4806C000
* uart3 0x48020000
* uart4 0x4806E000
*/
static struct reg_info uart_registers[] = {
	{ 0x00, 0x0, 0x0, "UART_DLL" },
	{ 0x04, 0x0, 0x0, "UART_DLH" },
	{ 0x08, 0x0, 0x0, "UART_EFR" },
	{ 0x0C, 0x0, 0x0, "UART_LCR" },
	{ 0x10, 0x0, 0x0, "UART_XON1_ADDR1" },
	{ 0x14, 0x0, 0x0, "UART_XON2_ADDR2" },
	{ 0x18, 0x0, 0x0, "UART_XOFF1" },
	{ 0x1C, 0x0, 0x0, "UART_XOFF2" },
	{ 0x20, 0x0, 0x0, "UART_MDR1" },
	{ 0x24, 0x0, 0x0, "UART_MDR2" },
	{ 0x28, 0x0, 0x0, "UART_SFLSR" },
	{ 0x2C, 0x0, 0x0, "UART_RESUME" },
	{ 0x30, 0x0, 0x0, "UART_SFREGL" },
	{ 0x34, 0x0, 0x0, "UART_SFREGH" },
	{ 0x38, 0x0, 0x0, "UART_BLR" },
	{ 0x3C, 0x0, 0x0, "UART_ACREG" },
	{ 0x40, 0x0, 0x0, "UART_SCR" },
	{ 0x44, 0x0, 0x0, "UART_SSR" },
	{ 0x48, 0x0, 0x0, "UART_EBLR" },
	{ 0x50, 0x0, 0x0, "UART_MVR" },
	{ 0x54, 0x0, 0x0, "UART_SYSC" },
	{ 0x58, 0x0, 0x0, "UART_SYSS" },
	{ 0x5C, 0x0, 0x0, "UART_WER" },
	{ 0x60, 0x0, 0x0, "UART_CFPS" },
	{ 0x64, 0x0, 0x0, "UART_RXFIFO_LVL" },
	{ 0x68, 0x0, 0x0, "UART_TXFIFO_LVL" },
	{ 0x6C, 0x0, 0x0, "UART_IER2" },
	{ 0x70, 0x0, 0x0, "UART_ISR2" },
	{ 0x74, 0x0, 0x0, "UART_FREQ_SEL" },
	{ 0x80, 0x0, 0x0, "UART_MDR3" },
	{ 0x84, 0x0, 0x0, "UART_TX_DMA_THRESHOLD" }
};

/* bases
* MMCHS1	0x4809C000
* MMCHS2	0x480B4000
* MMCHS3	0x480AD000
* MMCHS4	0x480D1000
* MMCHS5	0x480D5000
*/
static struct reg_info mmchs_registers[] = {
	{ 0x000, 0x0, 0x0, "MMCHS_HL_REV" },
	{ 0x004, 0x0, 0x0, "MMCHS_HL_HWINFO" },
	{ 0x010, 0x0, 0x0, "MMCHS_HL_SYSCONFIG" },
	{ 0x110, 0x0, 0x0, "MMCHS_SYSCONFIG" },
	{ 0x114, 0x0, 0x0, "MMCHS_SYSSTATUS" },
	{ 0x124, 0x0, 0x0, "MMCHS_CSRE" },
	{ 0x128, 0x0, 0x0, "MMCHS_SYSTEST" },
	{ 0x12C, 0x0, 0x0, "MMCHS_CON" },
	{ 0x130, 0x0, 0x0, "MMCHS_PWCNT" },
	{ 0x200, 0x0, 0x0, "RESERVED" },
	{ 0x204, 0x0, 0x0, "MMCHS_BLK" },
	{ 0x208, 0x0, 0x0, "MMCHS_ARG" },
	{ 0x20C, 0x0, 0x0, "MMCHS_CMD" },
	{ 0x210, 0x0, 0x0, "MMCHS_RSP10" },
	{ 0x214, 0x0, 0x0, "MMCHS_RSP32" },
	{ 0x218, 0x0, 0x0, "MMCHS_RSP54" },
	{ 0x21C, 0x0, 0x0, "MMCHS_RSP76" },
	{ 0x220, 0x0, 0x0, "MMCHS_DATA" },
	{ 0x224, 0x0, 0x0, "MMCHS_PSTATE" },
	{ 0x228, 0x0, 0x0, "MMCHS_HCTL" },
	{ 0x22C, 0x0, 0x0, "MMCHS_SYSCTL" },
	{ 0x230, 0x0, 0x0, "MMCHS_STAT" },
	{ 0x234, 0x0, 0x0, "MMCHS_IE" },
	{ 0x238, 0x0, 0x0, "MMCHS_ISE" },
	{ 0x23C, 0x0, 0x0, "MMCHS_AC12" },
	{ 0x240, 0x0, 0x0, "MMCHS_CAPA" },
	{ 0x248, 0x0, 0x0, "MMCHS_CUR_CAPA" },
	{ 0x250, 0x0, 0x0, "MMCHS_FE" },
	{ 0x254, 0x0, 0x0, "MMCHS_ADMAES" },
	{ 0x258, 0x0, 0x0, "MMCHS_ADMASAL" },
	{ 0x25C, 0x0, 0x0, "RESERVED" },
	{ 0x2FC, 0x0, 0x0, "MMCHS_REV" }
};

/* bases
* MCSPI1	0x48098000
* MCSPI2	0x4809A000
* MCSPI3	0x480B8000
* MCSPI4	0x480BA000
*/
static struct reg_info mcspi_registers[] = {
	{ 0x000, 0x0, 0x0, "MCSPI_HL_REV" },
	{ 0x004, 0x0, 0x0, "MCSPI_HL_HWINFO" },
	{ 0x010, 0x0, 0x0, "MCSPI_HL_SYSCONFIG" },
	{ 0x100, 0x0, 0x0, "MCSPI_REVISION" },
	{ 0x110, 0x0, 0x0, "MCSPI_SYSCONFIG" },
	{ 0x114, 0x0, 0x0, "MCSPI_SYSSTATUS" },
	{ 0x118, 0x0, 0x0, "MCSPI_IRQSTATUS" },
	{ 0x11C, 0x0, 0x0, "MCSPI_IRQENABLE" },
	{ 0x120, 0x0, 0x0, "MCSPI_WAKEUPENABLE" },
	{ 0x124, 0x0, 0x0, "MCSPI_SYST" },
	{ 0x128, 0x0, 0x0, "MCSPI_MODULCTRL" },
	{ 0x12C, 0x0, 0x0, "MCSPI_CH0CONF" },
	{ 0x130, 0x0, 0x0, "MCSPI_CH0STAT" },
	{ 0x134, 0x0, 0x0, "MCSPI_CH0CTRL" },
	{ 0x138, 0x0, 0x0, "MCSPI_TX0" },
	{ 0x13C, 0x0, 0x0, "MCSPI_RX0" },
	{ 0x17C, 0x0, 0x0, "MCSPI_XFERLEVEL" }
};

// USB_HOST_HS INSTANCE 
/* bases
* USBTLLHS_CONFIG	0x4A062000
* USBTLLHS_ULPI		0x4A062800
* HSUSBHOST		0x4A064000
* OHCI			0x4A064800
* EHCI			0x4A064C00
*/
static struct reg_info usbttlhs_config_registers[] = {
	{ 0x000, 0x0, 0x0, "USBTTL_REVISION" },
	{ 0x004, 0x0, 0x0, "USBTTL_HWINFO" },
	{ 0x010, 0x0, 0x0, "USBTTL_SYSCONFIG" },
	{ 0x014, 0x0, 0x0, "USBTTL_SYSSTATUS" },
	{ 0x018, 0x0, 0x0, "USBTTL_IRQSTATUS" },
	{ 0x11C, 0x0, 0x0, "USBTTL_IRQENABLE" },
	{ 0x030, 0x0, 0x0, "TTL_SHARED_CONF" },
	{ 0x040, 0x0, 0x0, "TTL_CHANNEL_CONF_0" },
	{ 0x044, 0x0, 0x0, "TTL_CHANNEL_CONF_1" },
	{ 0x400, 0x0, 0x0, "USBTLL_SAR_CNTX_0" },
	{ 0x404, 0x0, 0x0, "USBTLL_SAR_CNTX_1" },
	{ 0x408, 0x0, 0x0, "USBTLL_SAR_CNTX_2" },
	{ 0x40C, 0x0, 0x0, "USBTLL_SAR_CNTX_3" },
	{ 0x410, 0x0, 0x0, "USBTLL_SAR_CNTX_4" },
	{ 0x414, 0x0, 0x0, "USBTLL_SAR_CNTX_5" },
	{ 0x418, 0x0, 0x0, "USBTLL_SAR_CNTX_6" }
};

/*static struct reg_info usbttlhs_ulpi_registers[] = {
	{ 0x000, 0x0, 0x0, "VENDOR_ID_LO_0" },
	{ 0x100, 0x0, 0x0, "VENDOR_ID_LO_1" },
	{ 0x001, 0x0, 0x0, "VENDOR_ID_HI_0" },
	{ 0x101, 0x0, 0x0, "VENDOR_ID_HI_1" },
	{ 0x002, 0x0, 0x0, "PRODUCT_ID_LO_0" },
	{ 0x102, 0x0, 0x0, "PRODUCT_ID_LO_1" },
	{ 0x003, 0x0, 0x0, "PRODUCT_ID_HI_0" },
	{ 0x103, 0x0, 0x0, "PRODUCT_ID_HI_1" },
	{ 0x004, 0x0, 0x0, "FUNCTION_CTRL_0" },
	{ 0x104, 0x0, 0x0, "FUNCTION_CTRL_1" },
	{ 0x005, 0x0, 0x0, "FUNCTION_CTRL_SET_0" },
	{ 0x105, 0x0, 0x0, "FUNCTION_CTRL_SET_1" },
	{ 0x006, 0x0, 0x0, "FUNCTION_CTRL_CLR_0" },
	{ 0x106, 0x0, 0x0, "FUNCTION_CTRL_CLR_1" },
	{ 0x007, 0x0, 0x0, "INTERFACE_CTRL_0" },
	{ 0x107, 0x0, 0x0, "INTERFACE_CTRL_1" },
	{ 0x008, 0x0, 0x0, "INTERFACE_CTRL_SET_0" },
	{ 0x108, 0x0, 0x0, "INTERFACE_CTRL_SET_1" },
	{ 0x009, 0x0, 0x0, "INTERFACE_CTRL_CLR_0" },
	{ 0x109, 0x0, 0x0, "INTERFACE_CTRL_CLR_1" },
	{ 0x00A, 0x0, 0x0, "OTG_CTRL_0" },
	{ 0x10A, 0x0, 0x0, "OTG_CTRL_1" },
	{ 0x00B, 0x0, 0x0, "OTG_CTRL_SET_0" },
	{ 0x10B, 0x0, 0x0, "OTG_CTRL_SET_1" },
	{ 0x00C, 0x0, 0x0, "OTG_CTRL_CLR_0" },
	{ 0x10C, 0x0, 0x0, "OTG_CTRL_CLR_1" },
	{ 0x00D, 0x0, 0x0, "USB_INT_EN_RISE_0" },
	{ 0x10D, 0x0, 0x0, "USB_INT_EN_RISE_1" },
	{ 0x00E, 0x0, 0x0, "USB_INT_EN_RISE_SET_0" },
	{ 0x10E, 0x0, 0x0, "USB_INT_EN_RISE_SET_1" },
	{ 0x00F, 0x0, 0x0, "USB_INT_EN_RISE_CLR_0" },
	{ 0x10F, 0x0, 0x0, "USB_INT_EN_RISE_CLR_1" },
	{ 0x010, 0x0, 0x0, "USB_INT_EN_FALL_0" },
	{ 0x110, 0x0, 0x0, "USB_INT_EN_FALL_1" },
	{ 0x011, 0x0, 0x0, "USB_INT_EN_FALL_SET_0" },
	{ 0x111, 0x0, 0x0, "USB_INT_EN_FALL_SET_1" },
	{ 0x012, 0x0, 0x0, "USB_INT_EN_FALL_CLR_0" },
	{ 0x112, 0x0, 0x0, "USB_INT_EN_FALL_CLR_1" },
	{ 0x013, 0x0, 0x0, "USB_INT_STATUS_0" },
	{ 0x113, 0x0, 0x0, "USB_INT_STATUS_1" },
	{ 0x014, 0x0, 0x0, "USB_INT_LATCH_0" },
	{ 0x114, 0x0, 0x0, "USB_INT_LATCH_1" },
	{ 0x015, 0x0, 0x0, "DEBUG_0" },
	{ 0x115, 0x0, 0x0, "DEBUG_1" },
	{ 0x016, 0x0, 0x0, "SCRATCH_REGISTER_0" },
	{ 0x116, 0x0, 0x0, "SCRATCH_REGISTER_1" },
	{ 0x017, 0x0, 0x0, "SCRATCH_REGISTER_SET_0" },
	{ 0x117, 0x0, 0x0, "SCRATCH_REGISTER_SET_1" },
	{ 0x018, 0x0, 0x0, "SCRATCH_REGISTER_CLR_0" },
	{ 0x118, 0x0, 0x0, "SCRATCH_REGISTER_CLR_1" },
	{ 0x02F, 0x0, 0x0, "EXTENDED_SET_ACCESS_0" },
	{ 0x12F, 0x0, 0x0, "EXTENDED_SET_ACCESS_1" },
	{ 0x030, 0x0, 0x0, "UTMI_VCONTROL_EN_RISE_0" },
	{ 0x130, 0x0, 0x0, "UTMI_VCONTROL_EN_RISE_1" },
	{ 0x031, 0x0, 0x0, "UTMI_VCONTROL_EN_RISE_SET_0" },
	{ 0x131, 0x0, 0x0, "UTMI_VCONTROL_EN_RISE_SET_1" },
	{ 0x032, 0x0, 0x0, "UTMI_VCONTROL_EN_RISE_CLR_0" },
	{ 0x132, 0x0, 0x0, "UTMI_VCONTROL_EN_RISE_CLR_1" },
	{ 0x033, 0x0, 0x0, "UTMI_VCONTROL_STATUS_0" },
	{ 0x133, 0x0, 0x0, "UTMI_VCONTROL_STATUS_1" },
	{ 0x034, 0x0, 0x0, "UTMI_VCONTROL_LATCH_0" },
	{ 0x134, 0x0, 0x0, "UTMI_VCONTROL_LATCH_1" },
	{ 0x035, 0x0, 0x0, "UTMI_VSTATUS_0" },
	{ 0x135, 0x0, 0x0, "UTMI_VSTATUS_1" },
	{ 0x036, 0x0, 0x0, "UTMI_VSTATUS_SET_0" },
	{ 0x136, 0x0, 0x0, "UTMI_VSTATUS_SET_1" },
	{ 0x037, 0x0, 0x0, "UTMI_VSTATUS_CLR_0" },
	{ 0x137, 0x0, 0x0, "UTMI_VSTATUS_CLR_1" },
	{ 0x038, 0x0, 0x0, "USB_INT_LATCH_NOCLR_0" },
	{ 0x138, 0x0, 0x0, "USB_INT_LATCH_NOCLR_1" },
	{ 0x030, 0x0, 0x0, "VENDOR_INT_EN_0" },
	{ 0x130, 0x0, 0x0, "VENDOR_INT_EN_1" },
	{ 0x031, 0x0, 0x0, "VENDOR_INT_EN_SET_0" },
	{ 0x131, 0x0, 0x0, "VENDOR_INT_EN_SET_1" },
	{ 0x032, 0x0, 0x0, "VENDOR_INT_EN_CLR_0" },
	{ 0x132, 0x0, 0x0, "VENDOR_INT_EN_CLR_1" },
	{ 0x033, 0x0, 0x0, "VENDOR_INT_STATUS_0" },
	{ 0x133, 0x0, 0x0, "VENDOR_INT_STATUS_1" },
	{ 0x034, 0x0, 0x0, "VENDOR_INT_LATCH_0" },
	{ 0x134, 0x0, 0x0, "VENDOR_INT_LATCH_1" },
};*/


// base 0x49028000
static struct reg_info mcasp_registers[] = {
	{ 0x000, 0x0, 0x0, "MCASP_PID" },
	{ 0x004, 0x0, 0x0, "MCASP_SYSCONFIG" },
	{ 0x010, 0x0, 0x0, "MCASP_PFUNC" },
	{ 0x014, 0x0, 0x0, "MCASP_PDIR" },
	{ 0x018, 0x0, 0x0, "MCASP_PDOUT" },
	{ 0x01C, 0x0, 0x0, "MCASP_PDIN" },
	{ 0x020, 0x0, 0x0, "MCASP_PDCLR" },
	{ 0x044, 0x0, 0x0, "MCASP_GBLCTL" },
	{ 0x048, 0x0, 0x0, "MCASP_AMUTE" },
	{ 0x050, 0x0, 0x0, "MCASP_TXDITCTL" },
	{ 0x0A4, 0x0, 0x0, "MCASP_TXMASK" },
	{ 0x0A8, 0x0, 0x0, "MCASP_TXFMT" },
	{ 0x0AC, 0x0, 0x0, "MCASP_TXFMCTL" },
	{ 0x0B0, 0x0, 0x0, "MCASP_ACLKXCTL" },
	{ 0x0B4, 0x0, 0x0, "MCASP_AHCLKXCTL" },
	{ 0x0B8, 0x0, 0x0, "MCASP_TXTDM" },
	{ 0x0BC, 0x0, 0x0, "MCASP_EVTCTLX" },
	{ 0x0C0, 0x0, 0x0, "MCASP_TXSTAT" },
	{ 0x0C4, 0x0, 0x0, "MCASP_TXTDMSLOT" },
	{ 0x0C8, 0x0, 0x0, "MCASP_TXCLKCHK" },
	{ 0x0CC, 0x0, 0x0, "MCASP_TXEVTCTL" },
	{ 0x100, 0x0, 0x0, "MCASP_DITCSRA0" },
	{ 0x104, 0x0, 0x0, "MCASP_DITCSRA1" },
	{ 0x108, 0x0, 0x0, "MCASP_DITCSRA2" },
	{ 0x10C, 0x0, 0x0, "MCASP_DITCSRA3" },
	{ 0x110, 0x0, 0x0, "MCASP_DITCSRA4" },
	{ 0x114, 0x0, 0x0, "MCASP_DITCSRA5" },
	{ 0x118, 0x0, 0x0, "MCASP_DITCSRB0" },
	{ 0x11C, 0x0, 0x0, "MCASP_DITCSRB1" },
	{ 0x120, 0x0, 0x0, "MCASP_DITCSRB2" },
	{ 0x124, 0x0, 0x0, "MCASP_DITCSRB3" },
	{ 0x128, 0x0, 0x0, "MCASP_DITCSRB4" },
	{ 0x12C, 0x0, 0x0, "MCASP_DITCSRB5" },
	{ 0x130, 0x0, 0x0, "MCASP_DITUDRA0" },
	{ 0x134, 0x0, 0x0, "MCASP_DITUDRA1" },
	{ 0x138, 0x0, 0x0, "MCASP_DITUDRA2" },
	{ 0x13C, 0x0, 0x0, "MCASP_DITUDRA3" },
	{ 0x140, 0x0, 0x0, "MCASP_DITUDRA4" },
	{ 0x144, 0x0, 0x0, "MCASP_DITUDRA5" },
	{ 0x148, 0x0, 0x0, "MCASP_DITUDRB0" },
	{ 0x14C, 0x0, 0x0, "MCASP_DITUDRB1" },
	{ 0x150, 0x0, 0x0, "MCASP_DITUDRB2" },
	{ 0x154, 0x0, 0x0, "MCASP_DITUDRB3" },
	{ 0x158, 0x0, 0x0, "MCASP_DITUDRB4" },
	{ 0x15C, 0x0, 0x0, "MCASP_DITUDRB5" },
	{ 0x180, 0x0, 0x0, "MCASP_XRSRCTL0" },
	{ 0x200, 0x0, 0x0, "MCASP_TXBUF0" }
};

#define ARRAY_SIZE(type,object) \
	sizeof(object) / sizeof(type)

/* reads the /proc/cpuinfo and finds out which processor we are working on */
int read_processor() {
	FILE *fp = fopen(CPUINFO_FILE,"r");
	char field[30];

	if(fp == NULL) {
		fprintf(stderr,"Cannot open the cpuinfo file for reading\n");
		exit(1);
	}

	while(fscanf(fp,"%s",field) != EOF) {
		if(strcmp(field,"Hardware") == 0) {
//			printf("%s",field);

			// seek past three characters to get the "Hardware" field's value
			fseek(fp,3,SEEK_CUR);
			fgets(field,sizeof(field),fp);
			printf("%s",field);

			// OMAP4 one
			if(strstr(field,"OMAP4") != NULL) {
				printf("OMAP4 Processor\n");
				fclose(fp);
				return OMAP4;
			}

			// AM335x series one
			else if(strstr(field,"AM335") != NULL) {
				printf("AM335x Processor\n");
				fclose(fp);
				return AM335x;
			}

			break;
		}

//		printf("%s",field);
		fgets(field,sizeof(field),fp);
	}

	// not able to recognize the processor
	fclose(fp);
	return -1;
}

/*
 * Reads the register contents from the memory
 * Input:
 *	struct reg_info rinfo[] -	structure having the register offset, name ,etc.
 *	int num_regs		-	number of registers contained in the structure
 *	int argc, char **argv	-	commandline parameters passed to the program by user
 *	unsigned long base	-	base address for the memory location to be read
 *
 * Output:
 *	Register values are shown
 */

void show_registers(struct reg_info rinfo[],int num_regs, int argc, char **argv,unsigned long base) {
	int fd, i=0;
	int access_type = 'w';
	void *map_base, *virt_addr;
	unsigned long read_result, writeval;
	off_t target;

//	printf("%s Registers\n",reg_type);
//	num_regs = ARRAY_SIZE(struct reg_info, rinfo);
	printf("No of registers: %d\n", num_regs);

//	target = strtoul(argv[1], 0, 0);
	target = base;

	if(argc > 2)
		access_type = tolower(argv[2][0]);

	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;
	printf("/dev/mem opened.\n");

	/* Map one page */
	map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~MAP_MASK);

	if(map_base == (void *) -1) FATAL;
	printf("Memory mapped at address %p.\n", map_base); 

	for(i=0; i < num_regs; i++) {
		target = rinfo[i].offset;
		virt_addr = map_base + (target & MAP_MASK);

	    switch(access_type) {
			case 'b':
				read_result = *((unsigned char *) virt_addr);
				break;
			case 'h':
				read_result = *((unsigned short *) virt_addr);
				break;
			case 'w':
				read_result = *((unsigned long *) virt_addr);
				break;
			default:
				fprintf(stderr, "Illegal data type '%c'.\n", access_type);
				exit(2);
		}

		rinfo[i].old_value = read_result;
//		printf("Reg Name: %s & Value at address 0x%X (%p): 0x%X\n",rinfo[i].name, target, 							virt_addr, read_result);

		printf("REGISTER NAME: %s \t\tValue at address 0x%lu \t offset 0x%lu \t (%p) \t: 0x%lu\n",rinfo[i].name,target,rinfo[i].offset ,virt_addr, read_result);
	}


/*	char num[20];
	if(argc > 3) {
		writeval = strtoul(argv[4], 0, 0);
		switch(access_type) {
			case 'b':
				*((unsigned char *) virt_addr) = writeval;
				read_result = *((unsigned char *) virt_addr);
				break;
			case 'h':
				*((unsigned short *) virt_addr) = writeval;
				read_result = *((unsigned short *) virt_addr);
				break;
			case 'w':
				*((unsigned long *) virt_addr) = writeval;
				read_result = *((unsigned long *) virt_addr);
				break;
		}
                 printf("Written 0x%lu; readback 0x%lu\n 0x%s", writeval, read_result,(char *) virt_addr);
                 sprintf(num,"%lu",read_result);
                 fflush(stdout);
	}
*/
	if(munmap(map_base, MAP_SIZE) == -1) FATAL;
	close(fd);
}

/*
 * Main Routine for the program
 */
int main(int argc, char **argv) {
	int num_regs = 0;
	int choice = 0;

	if(argc < 2) {
		fprintf(stderr, "Usage:\t%s { reg } [ type [ data ] ] \n"
//			"\taddress : memory address to act upon\n"
			"\treg     : desired register selection : [0]DCAN,[1]GPIO,[2]I2C,[3]LCD,[4]MCASP,[5]MCSPI,[6]MMCSD,[7]TSC*,[8]UART,[9]USB*,[10]PRODUT_ID,[11]LCD_CONTROLLER; * are not implemented yet \n"
			"\ttype    : access operation type : [b]yte, [h]alfword, [w]ord\n"
			"\tdata    : data to be written\n\n",
                        
			argv[0]);
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
