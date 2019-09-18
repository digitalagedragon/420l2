CFLAGS=-Wall -Werror
LIBS=-lm
CC=mpicc
all: test

test: main.o mfunctions.o
	$(CC) -o $@ $(CFLAGS) $< $(LIBS)
