#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define N_MAX 1000
#define N_MACHINES 4
#define MAT_SIZE 1024
#define M_SIZE MAT_SIZE + 2

int main(int argc, char *argv[])
{
    FILE *file = fopen("result.txt", "w+");

    /*
    int **G1;
    G1 = (int **)malloc(sizeof(int *) * M_SIZE);

    for (int i = 0; i < M_SIZE; i++)
    {
        G1[i] = (int *)malloc(sizeof(int) * M_SIZE);

        for (int j = 0; j < M_SIZE; j++)
        {
            G1[i][j] = 0;
        }
    }

    //Filling the lower line of the matrix with the highest heat
    for (int i = 0; i < M_SIZE; i++)
    {
        G1[i][0] = 0xffffff; //Hexcode ffffff
    }
    */

    //------------------------------------------------------------------------------------------
    int rank;
    MPI_Status status;
    int i_division = MAT_SIZE / N_MACHINES;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double start_time = MPI_Wtime();
    int left_send_buffer[M_SIZE], right_send_buffer[M_SIZE], left_recv_buffer[M_SIZE], right_recv_buffer[M_SIZE];
    MPI_Request left_send_request, right_send_request, left_recv_request, right_recv_request;

    int **G1 = (int **)malloc(sizeof(int *) * i_division);
    for (int i = 0; i < i_division; i++)
    {
        G1[i] = (int *)malloc(sizeof(int) * M_SIZE);
        G2[i] = (int *)malloc(sizeof(int) * M_SIZE);

        for (int j = 0; j < M_SIZE; j++)
        {
            G1[i][j] = 0;
            G2[i][j] = 0;
        }

        G1[i][0] = 0xffffff; //Hexcode ffffff
    }

    for (int j = 0; j < M_SIZE; j++)
    {
        left_recv_buffer[M_SIZE] = 0;
        right_recv_buffer[M_SIZE] = 0;
    }

    //Iterações sobre a difusão de calor
    for (int it = 0; it < N_MAX; it++)
    {
        //Computes the parts with no dependencies
        for (int i = 1; i < i_division - 1; i++)
        {
            for (int j = 1; j < M_SIZE - 1; j++)
            {
                G2[i][j] = (G1[i - 1][j] + G1[i + 1][j] + G1[i][j - 1] + G1[i][j + 1] + G1[i][j]) / 5;
            }
        }

        //Waits to receive the left buffer
        MPI_Wait(&left_recv_request, MPI_STATUS_IGNORE);
        for (int j = 1; j < M_SIZE - 1; j++)
        {
            G2[0][j] = (left_recv_buffer[j] + G1[1][j] + G1[0][j - 1] + G1[0][j + 1] + G1[0][j]) / 5;
        }

        //Waits to receive the right buffer
        MPI_Wait(&right_recv_request, MPI_STATUS_IGNORE);
        for (int j = 1; j < M_SIZE - 1; j++)
        {
            G2[i_division - 1][j] = (G1[i_division - 2][j] + right_recv_buffer[j] + G1[i_division - 1][j - 1] + G1[i_division - 1][j + 1] + G1[i_division - 1][j]) / 5;
        }

        //Guarantees the buffers have been sent
        MPI_Wait(&left_send_request, MPI_STATUS_IGNORE);
        MPI_Wait(&right_send_request, MPI_STATUS_IGNORE);
        //Copies the column to the buffer
        for (int j = 0; j < M_SIZE; j++)
        {
            if (rank != 0)
                left_send_buffer[j] = G1[0][j];
            if (rank != N_MACHINES - 1)
                right_send_buffer[j] = G1[i_division - 1][j];
        }

        //Sends and receives assynchonously the buffers
        if (rank != 0)
        {
            left_send_request = MPI_Isend(&left_send_buffer);
            left_recv_request = MPI_Irecv(&left_recv_buffer);
        }
        if (rank != N_MACHINES - 1)
        {
            right_send_request = MPI_Isend(&right_send_buffer);
            right_recv_request = MPI_Irecv(&right_recv_buffer);
        }

        //Copies from G2 to G1
        for (int i = 0; i < i_division; i++)
        {
            for (int j = 0; j < M_SIZE; j++)
            {
                G1[i][j] = G2[i][j];
            }
        }
    }

    //TODO: rank 0 gathers results from all other ranks

    double end_time = MPI_Wtime();

    MPI_Finalize();

    if (rank == 0)
    {
        printf("Total time: %lf seconds\n", end_time - start_time);

        //Prints results to a file
        for (int i = 0; i < M_SIZE; i++)
        {
            for (int j = 0; j < M_SIZE; j++)
                fprintf(file, "%d|", G1[i][j]);
        }

        fprintf(file, "\n");
    }
}
