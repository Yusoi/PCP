#!/bin/sh
#
#PBS -l nodes=2:ppn=32:r641
#PBS -l walltime=1:00:00
#PBS -o out/results.txt
#PBS -e error.txt

module load gcc/5.3.0
module load gnu/openmpi_eth/1.8.4

cd $PBS_O_WORKDIR

array_num_machines=(2 4 6 8 10 12 14 16 20 24 28 32)

for i in $array_num_machines
do	
	export NUM_MACHINES=$i
	echo "Number of Machines: $NUM_MACHINES"
	make clean
	make NUM_MACHINES=$NUM_MACHINES
	for i in {1..10}
	do
		mpirun -np $NUM_MACHINES --map-by ppr:1:core bin/tp2.o
	done
done
