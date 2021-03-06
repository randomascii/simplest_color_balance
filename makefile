# Copyright 2009-2011 Nicolas Limare <nicolas.limare@cmla.ens-cachan.fr>
#
# Copying and distribution of this file, with or without
# modification, are permitted in any medium without royalty provided
# the copyright notice and this notice are preserved.  This file is
# offered as-is, without any warranty.

# source code, C language
SRC	= io_png.c balance_lib.c colorbalance_lib.c balance.c
# object files (partial compilation)
OBJ	= $(SRC:.c=.o)
# binary executable programs
BIN	= balance

# C compiler optimization options
COPT	= -O2
# complete C compiler options
CFLAGS	= $(COPT)
# preprocessor options
CPPFLAGS	= -I. -DNDEBUG
# linker options
LDFLAGS	=
# libraries
LDLIBS	= -lpng

# default target: the binary executable programs
default: $(BIN)

# dependencies
-include makefile.dep

# partial C compilation xxx.c -> xxx.o
%.o	: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

# final link
balance	: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# cleanup
.PHONY	: clean distclean
clean	:
	$(RM) $(OBJ)
distclean	: clean
	$(RM) $(BIN)
	$(RM) -r srcdoc

################################################
# dev tasks
PROJECT	= simplest_color_balance
-include	makefile.dev
