#include <stdio.h>

#define N_MAX 200
#define M_SIZE 25

void generate_image(int** matrix){
    
}

int main(){

    int G1[M_SIZE][M_SIZE] ={{0}};
    int G2[M_SIZE][M_SIZE] = {{0}};

    for(int i=0;i<M_SIZE;i++){
        G1[i][0] = 16777215;
    }

    //Iterações sobre a difusão de calor
    for(int it = 0; it < N_MAX ; it++){    
        for(int i = 1; i<M_SIZE-1;i++){
            for(int j = 1; j<M_SIZE-1;j++){

                //Transferência de calor
                G2[i][j] = (
                    G1[i-1][j]+
                    G1[i+1][j]+
                    G1[i][j-1]+
                    G1[i][j+1]+
                    G1[i][j]) /5;

            }   
        }


        //Copiar G2 para G1
        for(int i=1;i<M_SIZE-1;i++){
            for(int j=1;j<M_SIZE-1;j++){
                G1[i][j] = G2[i][j];
            }
        }
    }

    for(int j=M_SIZE-1;j>=0;j--){
        for(int i=M_SIZE-1;i>=0;i--){
            printf("%d|",G1[i][j]);
        }
        printf("\n");
    }

}


