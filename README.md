devicedbg
=========

'devicedbg' program supports debugging of device driver on ARM Cortex-A8 platform. As of now, it is tested on Pandaboard(OMAP4430) & Pandaboard ES (OMAP4460). We intent to test it on wider ARM Cortex-A* based boards. 

build
=====

For Compiling program without any platform constraints, do: 
$ make 
'make device-dbg' will also work. 

For compiling specific to OMAP4 or AM335x or OMAP35x series: 
$ make devicedbg-omap4
$ make devicedbg-am335x
$ make devicedbg-omap35x

usage 
=====

# command given below may require root previleges as it accesses the "/dev/mem" file.
# In that case invoke the command with sudo

$ ./devicedbg [ 0-14 ]

# invoking the program without any commandline arguments, prints the Usage of the program
$ ./devicedbg

# Output :
Usage:	./devicedbg { reg }
reg: desired register selection :[0]DCAN,[1]GPIO,[2]I2C,[3]LCD,[4]MCASP/MCBSP,[5]MCSPI,[6]MMCSD,[7]RTC,[8]TIMER,[9]TSC,[10]UART,[11]USB,[12]WDT,[13]PRODUT_ID,[14]LCD_CONTROLLER;

# For reading a section's registers say "PRODUCT_ID" registers
$ ./devicedbg 13
