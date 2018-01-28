###############################
# Author:	Anas Guetarni      #
#			Melvin Gay             #
# Date:		28.01.2018         #
# Name:		makefile           #
# Project:	ioc              #
###############################

#Source, executable, includes
INCL=main.h input.h functions.h profil.h path.h split.h
SRC=main.c input.c functions.c profil.c path.c split.c
OBJS=$(SRC:.c=.o)
EXE=ioc

#Compiler, linker defines, library defines
CFLAGS=-Wall
LIBS=
CC=gcc
RM=rm -rf
DATE=$(shell date +'%d.%m.%Y at %H:%M:%S')

#All target
all : $(EXE)

#Compile and assemble C source files into object files
%.o: %.c
	@echo '//Building file: $<'
	$(CC) $(CFLAGS) -c $*.c
	@echo '//Finished building: $<'
	@echo ' '

# Link all object files with external libraries into binaries
$(EXE): $(OBJS)
	@echo '//Building target: $@'
	$(CC) -o $(EXE) $(OBJS) $(LIBS)
	@echo '//Finished building target: $@ the $(DATE)'

#Objects depend on these libraries
$(OBJS): $(INCL)

#Clean up objects, exectuables
clean:
	$(RM) $(OBJS) $(EXE)
