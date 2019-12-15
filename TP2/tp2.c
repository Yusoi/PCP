#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <time.h>

#define N_MAX 1000
#define M_SIZE 1023
//#define N_MACHINES 4
//#define MEASUREMENTS
//#define DEBUG


int main(int argc, char *argv[])
{
    FILE *file = fopen("result.txt", "w+");

    int rank;
    MPI_Status status;
    int send[7] = {0};
    int receive[3] = {0};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int **G1 = (int**)malloc(sizeof(int*)*M_SIZE);
    for(int i=0;i<M_SIZE;i++){
	    G1[i] = (int*)malloc(sizeof(int)*M_SIZE);
        for(int j = 0;j<M_SIZE;j++){
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

    #ifdef MEASUREMENTS 
    //TODO: Ping-pong Tcomm calculation
    int ex = 1;

    if (rank == 0)
    {
        #ifdef DEBUG
        printf("First Send\n");
        fflush(stdout);
        #endif

        MPI_Send(&ex, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        #ifdef DEBUG
        printf("First Send Sent\n");
        fflush(stdout);
        #endif
    }
    if (rank == 1)
    {
        #ifdef DEBUG
        printf("First Receive\n");
        fflush(stdout);
        #endif

        MPI_Recv(&ex, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        #ifdef DEBUG
        printf("First Receive Received\n");
        fflush(stdout);
        #endif

        end_time = MPI_Wtime();                                //V Isto está errado
        printf("Ts: %lf\nTcomm: %lf\n", end_time - start_time, (end_time - start_time) * N_MAX);
    }

    //TODO: Tcomp calculation
    if(rank == 0)
    {
        int pos = M_SIZE/2;
        double s = MPI_Wtime();
        int k = (G1[pos-1][pos]+
                 G1[pos+1][pos]+
                 G1[pos][pos-1]+
                 G1[pos][pos+1]+
                 G1[pos][pos])/5; 
        double e = MPI_Wtime();
        printf("Tcomp: %lf, k: %d\n",e-s,k);
    }

    #endif

    //Iterações sobre a difusão de calor
    for (int it = 0; it < N_MAX; it++)
    {
        if (rank == 0)
        {
            for (int i = 1; i < M_SIZE - 1; i++)
            {
                for (int j = 1; j < M_SIZE - 1; j++)
                {
                    send[0] = G1[i - 1][j];
                    send[1] = G1[i + 1][j];
                    send[2] = G1[i][j - 1];
                    send[3] = G1[i][j + 1];
                    send[4] = G1[i][j];
                    send[5] = i;
                    send[6] = j;

                    #ifdef DEBUG
                    printf("Second Send Rank: %d\n",((i + j) % (N_MACHINES - 1)) + 1);
                    fflush(stdout);
                    #endif

                    MPI_Send(&send, 7, MPI_INT, ((i + j) % (N_MACHINES - 1)) + 1, 0, MPI_COMM_WORLD);

                    #ifdef DEBUG
                    printf("Second Send Sent\n");
                    fflush(stdout);
                    #endif
                }
            }

            //Tells all machines the iterations have ended
            send[0] = -1;

            #ifdef DEBUG
            printf("First Broadcast\n");
            fflush(stdout);
            #endif

            for(int i = 1; i <= N_MACHINES - 1 ; i++){
                MPI_Send(&send, 7, MPI_INT, i, 0, MPI_COMM_WORLD);
            }

            #ifdef DEBUG
            printf("First Broadcast Sent\n");
            fflush(stdout);
            #endif

            //Copiar G2 para G1
            for (int i = 1; i < M_SIZE - 1; i++)
            {
                for (int j = 1; j < M_SIZE - 1; j++)
                {
                    #ifdef DEBUG
                    printf("Second Receive\n");
                    fflush(stdout);
                    #endif

                    MPI_Recv(&receive, 3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

                    #ifdef DEBUG
                    printf("Second Receive Received\n");
                    fflush(stdout);
                    #endif

                    G1[receive[1]][receive[2]] = receive[0];
                }
            }
        }
        else
        {
            int done = 0;
            while (!done)
            {
                #ifdef DEBUG
                printf("Third Receive\n");
                fflush(stdout);
                #endif

                MPI_Recv(&send, 7, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

                #ifdef DEBUG
                printf("Third Receive Received\n");
                fflush(stdout);
                printf("%d %d %d %d %d %d %d\n", send[0], send[1], send[2], send[3], send[4], send[5], send[6]);
                #endif

                if (send[0] != -1)
                {
                    receive[0] = (send[0] + send[1] + send[2] + send[3] + send[4]) / 5;
                    receive[1] = send[5];
                    receive[2] = send[6];

                    #ifdef DEBUG
                    printf("Third Send\n");
                    fflush(stdout);
                    #endif

                    MPI_Send(&receive, 3, MPI_INT, 0, 0, MPI_COMM_WORLD);

                    #ifdef DEBUG
                    printf("Third Send Sent\n");
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

    if(rank == 0)
        printf("Total time: %lf seconds\n",end_time-start_time);

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
