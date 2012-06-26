devicedbg
=========

'devicedbg' program supports debugging of device driver on ARM Cortex-A8 platform. As of now, it is tested on Pandaboard(OMAP4430) & Pandaboard ES (OMAP4460). We intent to test it on wider ARM Cortex-A* based boards. 

build
=====

# FOR COMPILING THE PROGRAM WITHOUT ANY PLATFORM CONSTRAINTS DO
make
# make device-dbg will also work


# FOR COMPILING SPECIFIC TO THE OMAP4 PLATFORM DO
make devicedbg-omap4

# FOR COMPILING SPECIFIC TO THE AM335X PLATFORM DO
make devicedbg-am335x

# FOR CLEANING THE DIRECTORY i.e. REMOVING THE EXECUTABLES, PERFORM
make clean

## Note: YOU SHOULD BE IN THE SAME DIRECTORY WHERE THE SOURCE CODE RESIDES, IN ORDER TO GIVE THESE COMMANDS

usage 
=====

*TODO*


