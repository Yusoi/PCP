#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "papi.h"

#define N_MAX 1000
#define N_THREADS 1

#ifdef BIG_HEAP
#define M_SIZE 10000
#elif defined MEDIUM_HEAP
#define M_SIZE 5000
#elif defined SMALL_HEAP
#define M_SIZE 2000
#else
#define M_SIZE 1023
#endif

#ifdef TOTALS
#define NUM_EVENTS 3
#endif
#ifdef CACHE
#define NUM_EVENTS 5
#endif

int main()
{
    FILE *file = fopen("result.txt", "w+");

#if defined(BIG_HEAP) || defined(MEDIUM_HEAP) || defined(SMALL_HEAP)

    int **G1, **G2;

    G1 = (int **)_mm_malloc(sizeof(int) * M_SIZE * M_SIZE, 64);
    G2 = (int **)_mm_malloc(sizeof(int) * M_SIZE * M_SIZE, 64);

    for (int i = 0; i < M_SIZE; i++)
    {
        for (int j = 0; j < M_SIZE; j++)
        {
            G1[i][j] = 0;
            G2[i][j] = 0;
        }
    }

#else

    int G1[M_SIZE][M_SIZE] = {{0}};
    int G2[M_SIZE][M_SIZE] = {{0}};

#endif

    //Filling the lower line of the matrix with the highest heat
    for (int i = 0; i < M_SIZE; i++)
    {
        G1[i][0] = 0xffffff; //Hexcode ffffff
    }

    //------------------------------------------------------------------------------------------

    //PAPI variables

#ifdef TOTALS
    int Events[NUM_EVENTS] = {PAPI_TOT_INS, PAPI_TOT_CYC, PAPI_FP_INS};
#endif
#ifdef CACHE
    int Events[NUM_EVENTS] = {PAPI_L1_DCM, PAPI_L2_TCM, PAPI_L3_TCM, PAPI_L2_TCA, PAPI_L3_TCA};
#endif
    long long values[NUM_EVENTS];
    int retval = 1000;
    int it = 0;

    retval = PAPI_start_counters((int *)Events, NUM_EVENTS);
    if (retval != PAPI_OK)
    {
        PAPI_strerror(retval);
    }

    double start_time = omp_get_wtime();

    //Iterações sobre a difusão de calor
    for (it = 0; it < N_MAX; it++)
    {

        //#pragma omp parallel num_threads(N_THREADS)
        //#pragma omp for schedule(static)
        for (int i = 1; i < M_SIZE - 1; i++)
        {
            for (int j = 1; j < M_SIZE - 1; j++)
            {

//Transferência de calor
#ifdef M_SWITCH
                if (!it % 2)
                {
#endif
                    G2[i][j] = (G1[i - 1][j] +
                                G1[i + 1][j] +
                                G1[i][j - 1] +
                                G1[i][j + 1] +
                                G1[i][j]) /
                               5;
#ifdef M_SWITCH
                }
                else
                {
                    G1[i][j] = (G2[i - 1][j] +
                                G2[i + 1][j] +
                                G2[i][j - 1] +
                                G2[i][j + 1] +
                                G2[i][j]) /
                               5;
                }
#endif
            }
        }

#ifndef M_SWITCH
        //Copiar G2 para G1
        //#pragma omp for schedule(static)
        for (int i = 1; i < M_SIZE - 1; i++)
        {
            for (int j = 1; j < M_SIZE - 1; j++)
            {
                G1[i][j] = G2[i][j];
            }
        }
#endif
    }

    double end_time = omp_get_wtime();

    //Stops PAPI counters and checks for errors
    retval = PAPI_stop_counters(values, NUM_EVENTS);
    if (retval != PAPI_OK)
    {
        PAPI_strerror(retval);
    }

#ifdef TOTALS
    printf("Total Instructions: %lld\nTotal Clock Cycles: %lld\nTotal Flops: %lld\n", values[0], values[1], values[2]);
#endif

#ifdef CACHE
    printf("L1 Data Cache Misses: %lld\nL2 Total Cache Misses: %lld\nL3 Total Cache Misses: %lld\nL2 Total Cache Accesses: %lld\nL3 Total Cache Accesses: %lld\n", values[0], values[1], values[2], values[3], values[4]);
#endif

    printf("Time running: %lf\n", end_time - start_time);

    //Prints results to a file
    for (int i = 0; i < M_SIZE; i++)
    {
        for (int j = 0; j < M_SIZE; j++)
        {
#ifdef M_SWITCH
            if (!it % 2)
            {
#endif
                fprintf(file, "%d|", G1[i][j]);
#ifdef M_SWITCH
            }
            else
            {
                fprintf(file, "%d|", G2[i][j]);
            }
#endif
        }
        fprintf(file, "\n");
    }
}
