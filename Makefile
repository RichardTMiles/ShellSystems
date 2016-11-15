########################################################################################  
#
# Make file to allow the Makefile to compile and begin running on "Make"
# -Ref: StackOverFlow and Dr. Thompsons PDF
# Source: http://stackoverflow.com/questions/1484817/how-do-i-make-a-simple-makefile-for-gcc-on-linux
#
########################################################################################
CC=gcc
CFLAGS=-wall -g
program:  main.c system.c batchMode.c
	gcc -o program main.c -pthread
clean:
	$(RM) program
