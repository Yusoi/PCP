.DEFAULT_GOAL=all

clean:
	-@rm bin/tp2.o

all:
	mpicc -DN_MACHINES=$(NUM_MACHINES) -std=c11 -O3 -o bin/tp2.o tp2.c
