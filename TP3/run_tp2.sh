#!/bin/sh
#
#PBS -l nodes=2:ppn=32:r641
#PBS -l walltime=1:00:00
#PBS -o out/results.txt
#PBS -e error.txt

module load gcc/5.3.0
module load gnu/openmpi_eth/1.8.4

cd $PBS_O_WORKDIR

array_matrix_size="1024 2048 4096"
array_num_machines="2 4 8 16 32"

for j in $array_matrix_size; 
do
    echo "--------------------- MATRIX SIZE: $j ---------------------"
    for i in $array_num_machines; do
	    export N_MACHINES=$i
	    export C_FILE=tp2
	    export FILENAME=tp2
	    export MAT_SIZE=$j
	    echo "Node Number: $N_MACHINES"
	    make clean FILENAME=$FILENAME
	    make N_MACHINES=$N_MACHINES FILENAME=$FILENAME FILE=$FILE M_SIZE=$M_SIZE
	    mpirun -np $N_MACHINES --map-by ppr:1:socket -mca btl self,sm,tcp bin/${FILENAME}.o
    done
done
