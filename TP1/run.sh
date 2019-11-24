#!/bin/sh
#
#PBS -q mei
#PBS -l nodes=1:ppn=24:phi,walltime=20:00
#PBS -N pcp_tp1
#PBS -e ./error.txt
#PBS -o ./Outputs/Align_Stack_Default_Program_32/Ofast_24_thread.txt
#PBS -m ea -M "bernardosilva.business@gmail.com"


module load gcc/5.3.0
module load papi/5.4.1

cd "$PBS_O_WORKDIR"

make clean
make
make cache

echo "Totals:"

for i in {1..20}
do
	echo "Run $i -----------------------------------------------------------------------------------------------"
	./tp1_t.o
done

echo "Caches:"

for i in {1..20}
do
	echo "Run $i -----------------------------------------------------------------------------------------------"
	./tp1_c.o
done
