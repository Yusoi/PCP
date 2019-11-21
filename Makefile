.DEFAULT_GOAL = all

PAPI_DIR=/share/apps/papi/5.4.1

FLAGS = -L$(PAPI_DIR)/lib -I$(PAPI_DIR)/include -fopenmp -O3 -lpapi -o tp1.o 

clean: 
	-rm *.o
	-rm error.txt

cache:
	gcc -DCACHE $(FLAGS) tp1.c 

all:
	gcc -DTOTALS $(FLAGS) tp1.c


