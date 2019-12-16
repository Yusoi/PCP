.DEFAULT_GOAL=all

clean:
	-@rm bin/*.o

all:
	mpicc -Wall -DN_MACHINES=$(NUM_MACHINES) -std=c11 -O3 -o bin/$(FILENAME).o $(FILENAME).c
