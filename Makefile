# Makefile for devicedbg program

# compilation variables
CC 	:= gcc
FLAGS	:= -Wall
INCLUDE := devicedbg.h
SRC	:= devicedbg.c

# compilation for both the platforms
devicedbg: devicedbg.c devicedbg.h
	$(CC) $(FLAGS) -D '_OMAP4_=y _AM335x_=y _OMAP35x_=y' devicedbg.c -o devicedbg

# compilation only for omap4 system
devicedbg-omap4: devicedbg.c devicedbg.h
	$(CC) $(FLAGS) -D _OMAP4_ devicedbg.c -o devicedbg-omap4

# comilation only for am335x system
devicedbg-am335x: devicedbg.c devicedbg.h
	$(CC) $(FLAGS) -D _AM335x_ devicedbg.c -o devicedbg-am335x

# comilation only for omap35x system
devicedbg-omap35x: devicedbg.c devicedbg.h
	$(CC) $(FLAGS) -D _OMAP35x_ devicedbg.c -o devicedbg-omap35x

## cleaning phony target
clean:
	rm -rf devicedbg devicedbg-omap4 devicedbg-am335x devicedbg-omap35x
