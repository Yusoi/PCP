.DEFAULT_GOAL = all

clean: 
	-rm *.o
	-rm error.txt

all:
	gcc -L$(PAPI_DIR)/lib -I$(PAPI_DIR)/include -fopenmp -O2 -o tp1.o tp1.c -lpapi


