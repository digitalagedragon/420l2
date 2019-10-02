CFLAGS=-g -Wall
LIBS=-lm
CC=mpicc
all: test test2 test_gauss

test: parta.o mfunctions.o
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

test2: main2.o mfunctions.o
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

test_gauss: test_gauss.o mfunctions.o
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)
