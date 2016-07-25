#!/usr/bin/bash -f
# file: ihpapi/Makefile

# ====================================================================
# environment definitions;
# --------------------------------------------------------------------

# include ../make.def

# ====================================================================
#
# --------------------------------------------------------------------

TARGETS= ihpapi.c
CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)gcc
CFLAGS+=-Wall -DMAKEFILE -D__GETOPT_H__ -D_GETOPT_DEFINED_
IFLAGS = -I../../../../include
LFLAGS=
VPATH=
TRASH=*.o *.obj *.exe t t.* *~ *.[0-9][0-9][0-9]

# ====================================================================
# 
# --------------------------------------------------------------------

.PHONY: all compile install clean

all compile: 
	$(CC) $(CFLAGS) $(IFLAGS) -c $(TARGETS)
install: 
scripts:
manuals:
clean:
	rm -f ${TRASH} 

