#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define N_MAX 1000
#define N_MACHINES 24
#define M_SIZE 1023

int main(int argc, char *argv[])
{
    FILE *file = fopen("result.txt", "w+");

    int G1[M_SIZE][M_SIZE] = {{0}};

    //Filling the lower line of the matrix with the highest heat
    for (int i = 0; i < M_SIZE; i++)
    {
        G1[i][0] = 0xffffff; //Hexcode ffffff
    }

    int rank;
    MPI_Status status;
    int send[7] = {0};
    int receive[3] = {0}

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Iterações sobre a difusão de calor

    int it;

    for (int it = 0; it < N_MAX;i++)
    {
        if (rank == 0)
        {
            for (int i = 1; i < M_SIZE - 1; i++)
            {
                for (int j = 1; j < M_SIZE - 1; j++)
                {

                    /*
                    G2[i][j] = (G1[i - 1][j] +
                                G1[i + 1][j] +
                                G1[i][j - 1] +
                                G1[i][j + 1] +
                                G1[i][j]) /
                               5;
                    */

                    send[0] = G1[i - 1][j];
                    send[1] = G1[i + 1][j];
                    send[2] = G1[i][j - 1];
                    send[3] = G1[i][j + 1];
                    send[4] = G1[i][j];
                    send[5] = i;
                    send[6] = j;

                    MPI_Send(send, 7, MPI_INT, ((i + j) % (N_MACHINES - 1)) + 1, 0, MPI_COMM_WORLD);
                }
            }

            //Tells all machines the iterations have ended
            buffer[0] = -1;
            MPI_Bcast(send, 7, MPI_INT, k, 0, MPI_COMM_WORLD);

            //Copiar G2 para G1
            for (int i = 1; i < M_SIZE - 1; i++)
            {
                for (int j = 1; j < M_SIZE - 1; j++)
                {
                    MPI_Receive(receive, 3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
                    G1[receive[1]][receive[2]] = receive[0];
                }
            }
        }
        else
        {
            int done = 0;
            while (!done)
            {
                MPI_Receive(send, 7, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
                if (buffer[0] != -1)
                {
                    receive[0] = (send[0]+send[1]+send[2]+send[3]+send[4]) / 5;
                    receive[1] = send[5];
                    receive[2] = send[6];
                    MPI_Send(receive, 3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
                }
                else
                {
                    done = 1;
                }
            }
        }
    }

    MPI_Finalize();

    //Prints results to a file
    for (int i = 0; i < M_SIZE; i++)
    {
        for (int j = 0; j < M_SIZE; j++)
        {
            fprintf(file, "%d|", G1[i][j]);
        }

        fprintf(file, "\n");
    }
}
