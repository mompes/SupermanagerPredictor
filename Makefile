# I am a comment, and I want to say that the variable CC will be
# the compiler to use.
CC=gcc
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
CFLAGS=-c -O3 -Wall
FLAGS=-O3 -fopenmp -Wall

all: supermanager

supermanager: util.c supermanager.c
	$(CC) $(FLAGS) supermanager.c util.c -o supermanager

clean:
	rm -rf *.o supermanager
