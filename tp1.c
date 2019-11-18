#include <stdio.h>
#include <omp.h>
#include <time.h>
#include "papi.h"

#define N_MAX 1000
#define M_SIZE 1023
#define NUM_EVENTS 8



void generate_image(int **matrix)
{
}

int main()
{

    int G1[M_SIZE][M_SIZE] = {{0}};
    int G2[M_SIZE][M_SIZE] = {{0}};

    for (int i = 0; i < M_SIZE; i++)
    {
        G1[i][0] = 0xffffff; //Hexcode ffffff
    }

    //------------------------------------------------------------------------------------------
    
    long long values[NUM_EVENTS];
    unsigned int Events[NUM_EVENTS] = {PAPI_TOT_INS,PAPI_TOT_CYC,PAPI_L1_DCM,PAPI_L2_TCM,PAPI_L3_TCM,PAPI_L1_DCA,PAPI_L2_TCA,PAPI_L3_TCA};
    

    PAPI_start_counters((int*)Events,NUM_EVENTS);
    double start_time = omp_get_wtime();
    int retval = 0;

    #pragma omp parallel num_threads(1)
    #pragma omp for schedule(static)
    //Iterações sobre a difusão de calor
    for (int it = 0; it < N_MAX; it++)
    {
        for (int i = 1; i < M_SIZE - 1; i++)
        {
            for (int j = 1; j < M_SIZE - 1; j++)
            {

                //Transferência de calor
                G2[i][j] = (G1[i - 1][j] +
                            G1[i + 1][j] +
                            G1[i][j - 1] +
                            G1[i][j + 1] +
                            G1[i][j]) /
                           5;
            }
        }

        //Copiar G2 para G1
        for (int i = 1; i < M_SIZE - 1; i++)
        {
            for (int j = 1; j < M_SIZE - 1; j++)
            {
                G1[i][j] = G2[i][j];
            }
        }
    }


    double end_time = omp_get_wtime();
    retval = PAPI_stop_counters(values,NUM_EVENTS);
    printf("Total Instructions: %lld\nTotal Clock Cycles: %lld\nL1 Data Cache Misses: %lld\n",values[0],values[1],values[2]);
    printf("L2 Total Cache Misses: %lld\nL3 Total Cache Misses: %lld\nL1 Data Cache Accesses: %lld\n",values[3],values[4],values[5]);
    printf("L2 Total Cache Accesses: %lld\nL3 Total Cache Accesses: %lld\n",values[6],values[7]);
    printf("Start Time: %lf  End Time: %lf  Time running: %lf\n", start_time, end_time, end_time - start_time);

    //------------------------------------------------------------------------------------------
}

#ifdef PRINT
#pragma omp parallel for
for (int j = M_SIZE - 1; j >= 0; j--)
{
    for (int i = M_SIZE - 1; i >= 0; i--)
    {
        printf("%d|", G1[i][j]);
    }
    printf("\n");
}

#endif /*PRINT*/
