#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>   // necessário para wait()

#define VECTOR_SIZE 200000000
#define NUM_PROCESSES 4

// Funcao que simula uma carga de trabalho pesada
void heavy_work ( double * vector , int start , int end) {
    for (int i = start ; i < end ; ++i) {
        vector [i] = sin( vector [i]) * cos( vector [i]) + sqrt ( vector [i]);
    }
}

int main () {

double * vector = ( double *) malloc ( VECTOR_SIZE * sizeof ( double ));
for(int i = 0; i < VECTOR_SIZE ; i++) vector [i] = ( double )i;

int chunk_size = VECTOR_SIZE / NUM_PROCESSES;
struct timespec start , end;

clock_gettime ( CLOCK_MONOTONIC , & start );

for(int i = 0; i < NUM_PROCESSES; i++){
    __pid_t pid = fork();

    if(pid == 0){
        int inicio = i * chunk_size;

        // Corrige o último processo
        int fim;

	    if (i == NUM_PROCESSES - 1) {
    	  fim = VECTOR_SIZE;
	    } else {
          fim = (i + 1) * chunk_size;
        }

        heavy_work(vector,inicio,fim);

        exit(0);
    }
}

// Espera todos os processos filhos terminarem
for(int i = 0; i < NUM_PROCESSES; i++){
    wait(NULL);
}

clock_gettime ( CLOCK_MONOTONIC , &end);

double time_spent = ( end.tv_sec - start.tv_sec ) + 
                    (end.tv_nsec - start.tv_nsec ) / 1e9;

printf (" Versao sequencial executou em %f segundos \n", time_spent );
// printf (" Resultado de verificacao : vector [10] = %f\n", vector [10]) ;

free ( vector );
return 0;
}