.DEFAULT_GOAL=all

clean:
	-@rm bin/$(FILENAME).o

all:
	mpicc -Wall -DN_MACHINES=$(NUM_MACHINES) -DM_SIZE=$(M_SIZE) -std=c11 -O3 -o bin/$(FILENAME).o $(C_FILE).c
