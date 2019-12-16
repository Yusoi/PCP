#!/bin/sh
make clean
make NUM_MACHINES=4 FILENAME=tp2_blocks_timed
mpirun -np 4 bin/tp2_blocks_timed.o

