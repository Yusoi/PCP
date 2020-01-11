.DEFAULT_GOAL=all

clean:
	-@rm bin/$(FILENAME).o

all:
	mpicc -Wall -DN_MACHINES=$(N_MACHINES) -DMAT_SIZE=$(MAT_SIZE) -std=c11 -O3 -o bin/$(FILENAME).o $(C_FILE).c
