.DEFAULT_GOAL = all

PAPI_DIR=/share/apps/papi/5.4.1

#Comment the next flags as needed#############################################
#Result matrix switching instead of copying into another at the end
#SWITCH=-DM_SWITCH

#Choose one. If you choose none it's the matrix stack memory allocation
#BIG_HEAP=-DBIG_HEAP
#MEDIUM_HEAP=-DMEDIUM_HEAP
SMALL_HEAP=-DSMALL_HEAP

#Turn on Memory alignment and heap memory alignment block size. 
BLOCK_SIZE=-DBLOCKSIZE=32
##############################################################################
FLAGS = $(SWITCH) $(BIG_HEAP) $(MEDIUM_HEAP) $(SMALL_HEAP) $(BLOCK_SIZE) -L$(PAPI_DIR)/lib -I$(PAPI_DIR)/include -fopenmp -Ofast -lpapi 

clean: 
	-rm *.o
	-rm error.txt

cache:
	gcc -DCACHE $(FLAGS) -o tp1_c.o tp1.c 

all:
	gcc -DTOTALS $(FLAGS) -o tp1_t.o tp1.c


