#!/bin/sh
#
#PBS -q mei
#PBS -l nodes=1:ppn=4:k20
#PBS -N pcp_tp1
#PBS -e ./error.txt
#PBS -o ./output.txt

module load gcc/5.3.0
module load papi/5.4.1

#cd "$PBS_O_WORKDIR"

make

./tp1.o
./tp1.o
./tp1.o
./tp1.o
./tp1.o
./tp1.o
./tp1.o
./tp1.o
./tp1.o
./tp1.o

