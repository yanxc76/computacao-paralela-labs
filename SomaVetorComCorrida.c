#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4
#define VECTOR_SIZE 100000000

// Variáveis globais
int* vector;
long long global_sum = 0;

// Estrutura para passar argumentos para as threads
typedef struct {
    int thread_id;
    int start;
    int end;
} ThreadArgs;

// Função executada por cada thread
void* sum_worker(void* args) {
    ThreadArgs* my_args = (ThreadArgs*)args;
    long long local_sum = 0;

    // 1. Cada thread calcula a soma da sua fatia
    for (int i = my_args->start; i < my_args->end; i++) {
        local_sum += vector[i];
    }
    
    printf("Thread %d calculou a soma parcial: %lld\n", my_args->thread_id, local_sum);

    // 2. Condição de Corrida! Múltiplas threads tentam atualizar
    // a variável global ao mesmo tempo.
    global_sum += local_sum;

    free(my_args); // Libera a memória dos argumentos
    return NULL;
}

int main() {
    // --- Preparação ---
    vector = (int*)malloc(VECTOR_SIZE * sizeof(int));
    long long expected_sum = 0;
    for (int i = 0; i < VECTOR_SIZE; i++) {
        vector[i] = 1; // Preenche com 1 para facilitar a verificação
        expected_sum += 1;
    }

    pthread_t threads[NUM_THREADS];
    int chunk_size = VECTOR_SIZE / NUM_THREADS;

    // --- Criação das Threads ---
    for (int i = 0; i < NUM_THREADS; i++) {
        ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        args->thread_id = i;
        args->start = i * chunk_size;
        // Garante que a última thread processe até o final do vetor
        args->end = (i == NUM_THREADS - 1) ? VECTOR_SIZE : (i + 1) * chunk_size;
        
        pthread_create(&threads[i], NULL, sum_worker, args);
    }

    // --- Espera pelas Threads ---
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // --- Resultados ---
    printf("\n--- Resultados ---\n");
    printf("Soma Global Calculada: %lld\n", global_sum);
    printf("Soma Esperada:         %lld\n", expected_sum);
    if (global_sum != expected_sum) {
        printf("ERRO: Condicao de corrida ocorreu!\n");
    } else {
        // É possível, mas muito raro, que o resultado seja correto por sorte.
        printf("Resultado correto (isso foi sorte!).\n");
    }

    free(vector);
    return 0;
}
