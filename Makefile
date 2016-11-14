########################################################################################  
#
# Make file to allow the Makefile to compile and begin running on "Make"
# -Ref: StackOverFlow and Dr. Thompsons PDF
# Source: http://stackoverflow.com/questions/1484817/how-do-i-make-a-simple-makefile-for-gcc-on-linux
#
########################################################################################

CC=gcc 
CFLAGS=-Wall -g

all: program

program: main.o system.o batchMode.o

program.o: main.c system.c batchMode.c history.h 

clean:
	rm -f *.o

run: program
	./program
