#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <time.h>
#include <string.h>

#define N_MAX 1
#define M_SIZE 1024
//#define N_MACHINES 4
//#define MEASUREMENTS
#define DEBUG

int main(int argc, char *argv[])
{
    FILE *file = fopen("result.txt", "w+");

    int rank;
    MPI_Status status;
    int *send, *receive;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    send = (int *)malloc(sizeof(int)*(M_SIZE*3+1));
    if(send == NULL)
        printf("OOM send");
        fflush(stdout);
    receive = (int *)malloc(sizeof(int)*(M_SIZE-1));
    if(receive == NULL)
        printf("OOM receive");
        fflush(stdout);

    int **G1 = (int **)malloc(sizeof(int *) * M_SIZE);
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

#ifdef DEBUG
    printf("Rank: %d\n", rank);
#endif

    double start_time = MPI_Wtime();
    double end_time = 0;

    //Iterações sobre a difusão de calor
    for (int it = 0; it < N_MAX; it++)
    {
        if (rank == 0)
        {
            int mach = 1;

            for (int i = 1; i < M_SIZE - 1; i++, mach++)
            {

                if (mach > N_MACHINES - 1)
                {
                    
                    mach = 1;
                }

                printf("Mach %d\n", mach);
                
                fflush(stdout);

                send[0] = i;
                memcpy(&send[1], &G1[i - 1] , sizeof(int) * M_SIZE);
                memcpy(&send[M_SIZE + 1], &G1[i], sizeof(int) * M_SIZE);
                memcpy(&send[2 * M_SIZE + 1], &G1[i + 1], sizeof(int) * M_SIZE);

#ifdef DEBUG
                printf("Second Send Rank: %d\n", mach);
                fflush(stdout);
#endif
                MPI_Send(send, M_SIZE*3+1, MPI_INT, mach, 0, MPI_COMM_WORLD);
#ifdef DEBUG
                printf("Second Send Sent\n");
                fflush(stdout);
#endif
            }

            //Tells all machines the iterations have ended
            send[0] = -1;

#ifdef DEBUG
            printf("First Broadcast\n");
            fflush(stdout);
#endif

            for (int i = 1; i <= N_MACHINES - 1; i++)
            {
                MPI_Send(send, (M_SIZE - 2) * 3 + 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }

#ifdef DEBUG
            printf("First Broadcast Sent\n");
            fflush(stdout);
#endif

            //Copiar G2 para G1
            for (int i = 1; i < M_SIZE - 1; i++)
            {

#ifdef DEBUG
                printf("Second Receive\n");
                fflush(stdout);
#endif

                MPI_Recv(receive, M_SIZE - 2 + 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

#ifdef DEBUG
                printf("Second Receive Received\n");
                fflush(stdout);
#endif

                G1[receive[1]][receive[2]] = receive[0];
            }
        }
        else
        {
            int done = 0;
            while (!done)
            {
#ifdef DEBUG
                printf("Third Receive Rank: %d\n", rank);
                fflush(stdout);
#endif

                MPI_Recv(send, M_SIZE * 3 + 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

#ifdef DEBUG
                printf("Third Receive Received Rank: %d\n", rank);
                fflush(stdout);
#endif

                if (send[0] != -1)
                {
                    receive[0] = send[0];
                    for (int j = 2; j <= M_SIZE; j++)
                    {
                        receive[j - 1] = (send[j + M_SIZE - 2] +
                                          send[j + M_SIZE - 1] +
                                          send[j + M_SIZE] +
                                          send[j] +
                                          send[j + (M_SIZE - 1) * 2]) /
                                          5;
                    }

#ifdef DEBUG
                    printf("Third Send Rank: %d\n", rank);
                    fflush(stdout);
#endif

                    MPI_Send(receive, M_SIZE - 2 + 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

#ifdef DEBUG
                    printf("Third Send Sent %d\n", rank);
                    fflush(stdout);
#endif
                }
                else
                {
#ifdef DEBUG
                    printf("Done\n");
                    fflush(stdout);
#endif

                    done = 1;
                }
            }
        }
    }

    end_time = MPI_Wtime();

    if (rank == 0)
        printf("Total time: %lf seconds\n", end_time - start_time);

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

    return 0;
}
