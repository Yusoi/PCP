#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>

#define N_MAX 1000
#define N_MACHINES 4
#define MAT_SIZE 1024
#define M_SIZE (MAT_SIZE + 2)
#define TIME_RESOLUTION 1000000


int max(int num1, int num2){
    return (num1 > num2) ? num1 : num2;
}

int main(int argc, char *argv[]){
    struct timeval time;
    gettimeofday(&time, NULL);
    long long unsigned start_time, end_time, start_rx, end_rx, tcomm1, tcomp1, u_time, t_time, v_time;
    tcomm1 = 0;


    FILE *file = fopen("result.txt", "w+");

    int rank;
    int i_division = MAT_SIZE / N_MACHINES;
    int MACH_MAT_SIZE = i_division * M_SIZE;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    gettimeofday(&time, NULL);
    start_time = time.tv_sec * TIME_RESOLUTION + time.tv_usec;
    int left_send_buffer[M_SIZE+1], right_send_buffer[M_SIZE+1], left_recv_buffer[M_SIZE+1], right_recv_buffer[M_SIZE+1];
    int *final_send_buffer = (int *)malloc(sizeof(int) * M_SIZE * i_division);
    int *final_result_buffer = (int *)malloc(sizeof(int) * M_SIZE * MAT_SIZE);
    if (final_send_buffer == NULL || final_result_buffer == NULL){
        printf("NULL, not enough memory\n");
    }
    MPI_Request left_send_request, right_send_request, left_recv_request, right_recv_request;

    int **G1 = (int **)malloc(sizeof(int *) * i_division);
    int **G2 = (int **)malloc(sizeof(int *) * i_division);
    for (int i = 0; i < i_division; i++){
        G1[i] = (int *)malloc(sizeof(int) * M_SIZE);
        G2[i] = (int *)malloc(sizeof(int) * M_SIZE);

        for (int j = 0; j < M_SIZE; j++){
            G1[i][j] = 0;
            G2[i][j] = 0;
        }

        G1[i][0] = 0xffffff; //Hexcode ffffff
        G2[i][0] = 0xffffff;
    }

    for (int j = 0; j < M_SIZE+1; j++){
        left_recv_buffer[j] = 0;
        right_recv_buffer[j] = 0;
    }

    //Iterações sobre a difusão de calor
    for (int it = 0; it < N_MAX; it++){
        gettimeofday(&time, NULL);
        start_rx = time.tv_sec * TIME_RESOLUTION + time.tv_usec;
        
        //Computes the parts with no dependencies
        for (int i = 1; i < i_division - 1; i++)
        {
            for (int j = 1; j < M_SIZE - 1; j++)
            {
                G2[i][j] = (G1[i - 1][j] + G1[i + 1][j] + G1[i][j - 1] + G1[i][j + 1] + G1[i][j]) / 5;
            }
        }
        gettimeofday(&time, NULL);
        end_rx = time.tv_sec * TIME_RESOLUTION + time.tv_usec;

        //Waits to receive the left buffer
        if (it != 0 && rank != 0){
            MPI_Wait(&left_recv_request, MPI_STATUS_IGNORE);
        }
        gettimeofday(&time, NULL);
        u_time = time.tv_sec * TIME_RESOLUTION + time.tv_usec;
        t_time = left_recv_buffer[M_SIZE];
        tcomm1 = max(tcomm1,u_time-t_time);

        for (int j = 1; j < M_SIZE - 1; j++){
            G2[0][j] = (left_recv_buffer[j] + G1[1][j] + G1[0][j - 1] + G1[0][j + 1] + G1[0][j]) / 5;
        }

        //Waits to receive the right buffer
        if (it != 0 && rank != N_MACHINES - 1){
            MPI_Wait(&right_recv_request, MPI_STATUS_IGNORE);
        }
        gettimeofday(&time, NULL);
        u_time = time.tv_sec * TIME_RESOLUTION + time.tv_usec;
        t_time = right_recv_buffer[M_SIZE];
        tcomm1 = max(tcomm1,u_time-t_time);

        for (int j = 1; j < M_SIZE - 1; j++){
            G2[i_division - 1][j] = (G1[i_division - 2][j] + right_recv_buffer[j] + G1[i_division - 1][j - 1] + G1[i_division - 1][j + 1] + G1[i_division - 1][j]) / 5;
        }
        gettimeofday(&time, NULL);
        v_time = time.tv_sec * TIME_RESOLUTION + time.tv_usec;
        tcomp1 = max(tcomp1, v_time - t_time);

        //Guarantees the buffers have been sent
        if (it != 0){
            if (rank != 0){
                MPI_Wait(&left_send_request, MPI_STATUS_IGNORE);
            }
            if (rank != N_MACHINES - 1){
                MPI_Wait(&right_send_request, MPI_STATUS_IGNORE);
            }
        }
        //Copies the column to the buffer
        for (int j = 0; j < M_SIZE; j++){
            if (rank != 0)
                left_send_buffer[j] = G1[0][j];
            if (rank != N_MACHINES - 1)
                right_send_buffer[j] = G1[i_division - 1][j];
        }

        //Sends and receives assynchonously the buffers
        if (rank != 0){
            gettimeofday(&time, NULL);
            t_time = time.tv_sec * TIME_RESOLUTION + time.tv_usec;
            left_send_buffer[M_SIZE] = t_time;
            MPI_Isend(left_send_buffer, M_SIZE+1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &left_send_request);
            MPI_Irecv(left_recv_buffer, M_SIZE+1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &left_recv_request);
        }
        if (rank != N_MACHINES - 1){
            t_time = time.tv_sec * TIME_RESOLUTION + time.tv_usec;
            right_send_buffer[M_SIZE] = t_time;
            MPI_Isend(right_send_buffer, M_SIZE+1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &right_send_request);
            MPI_Irecv(right_recv_buffer, M_SIZE+1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &right_recv_request);
        }

        //Copies from G2 to G1
        for (int i = 0; i < i_division; i++){
            for (int j = 0; j < M_SIZE; j++){
                G1[i][j] = G2[i][j];
            }
        }
    }

    for (int i = 0; i < i_division; i++){
        for (int j = 0; j < M_SIZE; j++){
            final_send_buffer[i * M_SIZE + j] = G1[i][j];
        }
    }

    //Rank 0 gathers results from all other ranks
    //int MPI_Gather(void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
    MPI_Gather(final_send_buffer, MACH_MAT_SIZE, MPI_INT, final_result_buffer, MACH_MAT_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    gettimeofday(&time, NULL);
    end_time = time.tv_sec * TIME_RESOLUTION + time.tv_usec;

    if (rank == 0){
        //Creates the final matrix to output the result
        int **FINAL_MAT = (int **)malloc(sizeof(int *) * M_SIZE);
        for (int i = 0; i < M_SIZE; i++){
            FINAL_MAT[i] = (int *)malloc(sizeof(int) * M_SIZE);
        }
        for (int j = 0; j < M_SIZE; j++){
            FINAL_MAT[0][j] = 0;
            FINAL_MAT[M_SIZE - 1][j] = 0;
        }
        FINAL_MAT[0][0] = 0xffffff;
        FINAL_MAT[M_SIZE - 1][0] = 0xffffff;

        //Copies the result from the receive buffer to the result matrix
        for (int i = 0; i < MAT_SIZE; i++){
            for (int j = 0; j < M_SIZE; j++){
                FINAL_MAT[i + 1][j] = final_result_buffer[i * M_SIZE + j];
            }
        }

        printf("Total time: %lld us\n", end_time - start_time);
        printf("Comm time: %lld us\n", tcomm1);
        printf("Comp time: %lld us\n", end_rx-start_rx);

        //Prints results to a file
        for (int i = 0; i < M_SIZE; i++){
            for (int j = 0; j < M_SIZE; j++){
                fprintf(file, "%d|", FINAL_MAT[i][j]);
            }
            fprintf(file, "\n");
        }
    }

    MPI_Finalize();
}
