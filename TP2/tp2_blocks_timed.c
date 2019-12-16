#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define N_MAX 1000
#define M_SIZE 1024
//#define N_MACHINES 4
//#define MEASUREMENTS
//#define DEBUG

int max(int num1, int num2)
{
    return (num1 > num2 ) ? num1 : num2;
}

int main(int argc, char *argv[])
{
    double global_start_time = MPI_Wtime();
    double end_r0, global_end_time, tcomp1, tcomp2, tcomp3;
    tcomp1 = tcomp2 = tcomp3 = 0;
    int rank;
    MPI_Status status;

    
    

    //send = (int *)calloc((edges_area + 2),sizeof(int));
    //receive = (int *)calloc((block_area + 2),sizeof(int));

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int block_side = (int)floor(sqrt(M_SIZE - 2));
    int edges_area = (block_side + 2) * (block_side + 2);
    int block_area = block_side * block_side;

    int send[edges_area + 2];
    int receive[block_area + 2 + 1];

    int **G1 = (int **)malloc(M_SIZE * sizeof(int *));
    for (int i = 0; i < M_SIZE; i++)
    {
        G1[i] = (int *)calloc(M_SIZE, sizeof(int));
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
    fflush(stdout);
#endif

    start_time = MPI_Wtime();

    //Iterações sobre a difusão de calor
    for (int it = 0; it < N_MAX; it++)
    {
        if (rank == 0)
        {
            int mach = 1;
            int count = 0;

            double start_r0 = MPI_Wtime();

            for (int i = 1; i < M_SIZE - 1; i += block_side)
            {
                for (int j = 1; j < M_SIZE - 1; j += block_side, mach++)
                {

                    if (mach > N_MACHINES - 1)
                    {
                        mach = 1;
                    }

                    if ((i + block_side - 1) >= M_SIZE - 1)
                    {
                        i = M_SIZE - 1 - block_side;
                    }

                    if ((j + block_side - 1) >= M_SIZE - 1)
                    {
                        j = M_SIZE - 1 - block_side;
                    }

                    //printf("I: %d J: %d\n",i,j);

                    //printf("Mach %d\n", mach);
                    //fflush(stdout);

                    send[0] = i;
                    send[1] = j;

                    for (int u = 0; u < block_side + 2; u++)
                    {
                        for (int v = 0; v < block_side + 2; v++)
                        {
                            //memcpy(&send[2 + u * (block_side + 2)], &G1[i - 1][j - 1], (block_side + 2) * sizeof(int));
                            send[2 + u * block_side + v] = G1[i - 1 + u][j - 1 + v];
                        }
                    }

#ifdef DEBUG
                    printf("Second Send Rank: %d\n", mach);
                    fflush(stdout);
#endif
                    double end_r0 = MPI_Wtime();
                    tcomp1 = max(end_r0-start_r0, tcomp1);
                    MPI_Send(&send, edges_area + 2, MPI_INT, mach, 0, MPI_COMM_WORLD);
#ifdef DEBUG
                    printf("Second Send Sent\n");
                    fflush(stdout);
#endif

                    count++;
                }
            }

            //Tells all machines the iterations have ended
            send[0] = -1;

#ifdef DEBUG
            printf("First Broadcast\n");
            fflush(stdout);
#endif

            for (int i = 1; i <= N_MACHINES - 1; i++)
            {
                MPI_Send(&send, edges_area + 2, MPI_INT, i, 0, MPI_COMM_WORLD);
            }

#ifdef DEBUG
            printf("First Broadcast Sent\n");
            fflush(stdout);
#endif

            //Copiar G2 para G1
            for (int i = 0; i < count; i++)
            {

#ifdef DEBUG
                printf("Second Receive\n");
                fflush(stdout);
#endif

                MPI_Recv(&receive, block_area + 2 + 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
                tcomp2 = max(tcomp2,receive[block_side*block_side+2]);//verificar que é o fim do array

#ifdef DEBUG
                printf("Second Receive Received\n");
                fflush(stdout);
#endif
                
                start_r0 = MPI_Wtime();
                for (int u = 0; u < block_side; u++)
                {
                    for (int v = 0; v < block_side; v++)
                    {
                        //memcpy(&G1[receive[0] + u], receive[2 + u * block_side], block_side * sizeof(int));
                        G1[receive[0] + u][receive[1] + v] = receive[2 + u * block_side + v];
                    }
                }
                end_r0 = MPI_Wtime();
                tcomp3 = max (end_r0-start_r0, tcomp3);
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

                MPI_Recv(&send, edges_area + 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

#ifdef DEBUG
                printf("Third Receive Received Rank: %d\n", rank);
                fflush(stdout);
#endif

                if (send[0] != -1)
                {
                    double start_rx = MPI_Wtime();
                    receive[0] = send[0];
                    receive[1] = send[1];
                    for (int u = 0; u < block_side; u++)
                    {
                        for (int v = 0; v < block_side; v++)
                        {
                            receive[2+u*block_side+v] = (send[2+(u-1)*block_side+v] +
                                                         send[2+u*block_side+v] +
                                                         send[2+(u+1)*block_side+v] +
                                                         send[2+u*block_side+(v+1)] +
                                                         send[2+u*block_side+(v-1)])
                                                         /5;
                        }
                    }
                    double end_rx = MPI_Wtime();
                    receive[block_side*block_side+2] = end_rx-start_rx;

#ifdef DEBUG
                    printf("Third Send Rank: %d\n", rank);
                    fflush(stdout);
#endif
                    MPI_Send(&receive, block_area + 2 + 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

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

    global_end_time = MPI_Wtime();

    
    if (rank == 0)
    {
        printf("Total time: %lf seconds\n", global_end_time - global_start_time);
        printf("Time tcomp1: %lf seconds\n", tcomp1);
        printf("Time tcomp2: %lf seconds\n", tcomp2);
        printf("Time tcomp3: %lf seconds\n", tcomp3);

        //Prints results to a file
        FILE *file = fopen("result.txt", "w+");

        for (int i = 0; i < M_SIZE; i++)
        {
            for (int j = 0; j < M_SIZE; j++)
            {
                //printf("Crash i: %d j: %d\n",i,j);
                //fflush(stdout);
                fprintf(file, "%d|", G1[i][j]);
            }

            fprintf(file, "\n");
        }
    }

    MPI_Finalize();

    return 0;
}
