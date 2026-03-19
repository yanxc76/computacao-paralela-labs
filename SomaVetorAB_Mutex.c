#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 8
#define VECTOR_SIZE 100000000

// Variáveis globais
int* vector_A;
int* vector_B;
long long dot_product = 0;
pthread_mutex_t mutex; // Mutex para proteger a variável global

// Estrutura para passar argumentos para as threads
typedef struct {
    int thread_id;
    int start;
    int end;
} ThreadArgs;

// Função executada por cada thread
void* dot_product_worker(void* args) {
    ThreadArgs* my_args = (ThreadArgs*)args;
    long long local_dot_product = 0; // Variável local para armazenar o produto escalar parcial

    // 1. Cada thread calcula o produto escalar da sua fatia dos vetores
    for (int i = my_args->start; i < my_args->end; i++) {
        local_dot_product += vector_A[i] * vector_B[i];
    }

    // 2. Protege a variável global com mutex para evitar condições de corrida
    pthread_mutex_lock(&mutex);
    dot_product += local_dot_product;
    pthread_mutex_unlock(&mutex);

    free(my_args); // Libera a memória dos argumentos
    return NULL;
}

int main() {
    // --- Inicializa o mutex ---
    pthread_mutex_init(&mutex, NULL);

    // --- Preparação ---
    vector_A = (int*)malloc(VECTOR_SIZE * sizeof(int));
    vector_B = (int*)malloc(VECTOR_SIZE * sizeof(int));

    // Inicializa os vetores A e B
    for (int i = 0; i < VECTOR_SIZE; i++) {
        vector_A[i] = 1;  // Para simplicidade, preenchendo com 1
        vector_B[i] = 1;  // Para simplicidade, preenchendo com 1
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

        pthread_create(&threads[i], NULL, dot_product_worker, args);
    }

    // --- Espera pelas Threads ---
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // --- Resultados ---
    printf("\n--- Resultados ---\n");
    printf("Produto Escalar Calculado: %lld\n", dot_product);
    printf("Produto Escalar Esperado:  %lld\n", (long long)VECTOR_SIZE); // Todos os valores são 1, então a soma é igual a VECTOR_SIZE
    if (dot_product != VECTOR_SIZE) {
        printf("ERRO: Condição de corrida ocorreu!\n");
    } else {
        printf("Resultado correto.\n");
    }

    // --- Finaliza o mutex ---
    pthread_mutex_destroy(&mutex);

    // Libera a memória dos vetores
    free(vector_A);
    free(vector_B);
    return 0;
}
