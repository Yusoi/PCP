#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N_MAX 1000
#define N_THREADS 24
#define MAT_SIZE 1024
#define M_SIZE MAT_SIZE + 2

int main()
{
    FILE *file = fopen("result.txt", "w+");

    int **G1, **G2;

    G1 = (int **)malloc(sizeof(int *) * M_SIZE);
    G2 = (int **)malloc(sizeof(int *) * M_SIZE);

    for (int i = 0; i < M_SIZE; i++)
    {
        G1[i] = (int *)malloc(sizeof(int) * M_SIZE);
        G2[i] = (int *)malloc(sizeof(int) * M_SIZE);
    }

    for (int i = 0; i < M_SIZE; i++)
    {
        for (int j = 0; j < M_SIZE; j++)
        {
            G1[i][j] = 0;
            G2[i][j] = 0;
        }
    }

    //Filling the lower line of the matrix with the highest heat
    for (int i = 0; i < M_SIZE; i++)
    {
        G1[i][0] = 0xffffff; //Hexcode ffffff
    }

    //------------------------------------------------------------------------------------------

    double start_time = omp_get_wtime();

    //Iterações sobre a difusão de calor
    for (int it = 0; it < N_MAX; it++)
    {

        #pragma omp parallel num_threads(N_THREADS)
        #pragma omp for schedule(static)
        for (int i = 1; i < M_SIZE - 1; i++)
        {
            for (int j = 1; j < M_SIZE - 1; j++)
            {
                G2[i][j] = (G1[i - 1][j] + G1[i + 1][j] + G1[i][j - 1] + G1[i][j + 1] + G1[i][j]) / 5; j++;
            }
        }

        //Copiar G2 para G1
        #pragma omp for schedule(static)
        for (int i = 1; i < M_SIZE - 1; i++)
        {
            for (int j = 1; j < M_SIZE - 1; j++)
            {
                G1[i][j] = G2[i][j]; j++;
            }
        }
    }

    double end_time = omp_get_wtime();

    printf("Time running: %lf\n", end_time - start_time);

    //Prints results to a file
    for (int i = 0; i < M_SIZE; i++)
    {
        for (int j = 0; j < M_SIZE; j++)
            fprintf(file, "%d|", G1[i][j]);
    }
    fprintf(file, "\n");
}
}
