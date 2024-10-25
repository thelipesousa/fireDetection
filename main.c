#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "functions.h"

// Definição das variáveis externas para comunicação com a central
pthread_mutex_t central_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t central_cond = PTHREAD_COND_INITIALIZER;

int main() {
    srand(time(NULL)); // Inicializa a semente do rand()
    initialize_forest();

    pthread_t sensor_threads[SIZE][SIZE];
    pthread_t fire_thread;
    pthread_t central_thread;

    // Criar threads dos nós sensores
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (forest[i][j].state == SENSOR) {
                if (pthread_create(&sensor_threads[i][j], NULL, sensor_node_func, (void*)&forest[i][j])) {
                    fprintf(stderr, "Erro ao criar thread do sensor (%d, %d)\n", i, j);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    // Criar thread geradora de incêndios
    if (pthread_create(&fire_thread, NULL, fire_generator_func, NULL)) {
        fprintf(stderr, "Erro ao criar thread geradora de incêndios\n");
        exit(EXIT_FAILURE);
    }

    // Criar thread central de controle
    if (pthread_create(&central_thread, NULL, central_control_func, NULL)) {
        fprintf(stderr, "Erro ao criar thread central\n");
        exit(EXIT_FAILURE);
    }

    // Loop principal para atualizar a visualização
    while (1) {
        print_forest();
        sleep(1);
    }

    // Aguardar término das threads (nunca alcançado neste caso)
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (forest[i][j].state == SENSOR) {
                pthread_join(sensor_threads[i][j], NULL);
            }
        }
    }
    pthread_join(fire_thread, NULL);
    pthread_join(central_thread, NULL);

    return 0;
}
