#!/bin/sh
#
#PBS -q mei
#PBS -l nodes=1:ppn=24:phi,walltime=30:00
#PBS -N pcp_tp1
#PBS -e ./error.txt
#PBS -o ./Outputs/output.txt

module load gcc/5.3.0
module load papi/5.4.1

cd "$PBS_O_WORKDIR"

make clean
make

echo "Totals:"

for i in {1..20}
do
	echo "Run $i -----------------------------------------------------------------------------------------------"
	./tp1.o
done

make clean
make cache

echo "Caches:"

for i in {1..20}
do
	echo "Run $i -----------------------------------------------------------------------------------------------"
	./tp1.o
done
