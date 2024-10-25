#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "functions.h"

#define TAMANHO_FLORESTA 30


char floresta[TAMANHO_FLORESTA][TAMANHO_FLORESTA];
pthread_mutex_t mutex_flores[TAMANHO_FLORESTA][TAMANHO_FLORESTA];

void inicializar_forest() {
    for (int i = 0; i < TAMANHO_FLORESTA; i++) {
        for (int j = 0; j < TAMANHO_FLORESTA; j++) {
            floresta[i][j] = '-';
            pthread_mutex_init(&mutex_flores[i][j], NULL);
        }
    }
}

int main() {
    pthread_t gerador_thread, central_thread;
    sensor_args sensores_args[TAMANHO_FLORESTA * TAMANHO_FLORESTA];
    pthread_t sensores_threads[TAMANHO_FLORESTA * TAMANHO_FLORESTA];
    int id = 0;

    inicializar_forest();
    inicializar_sensores();

    // Cria a thread geradora de incêndios
    pthread_create(&gerador_thread, NULL, gerador_incendios, NULL);

    pthread_create(&central_thread, NULL, central_controle, NULL);


    for (int i = 0; i < TAMANHO_FLORESTA; i++) {
        for (int j = 0; j < TAMANHO_FLORESTA; j++) {
            sensores_args[id].x = i;
            sensores_args[id].y = j;
            pthread_create(&sensores_threads[id], NULL, sensor, &sensores_args[id]);
            id++;
        }
    }


    pthread_join(gerador_thread, NULL);
    pthread_join(central_thread, NULL);
    for (int i = 0; i < TAMANHO_FLORESTA * TAMANHO_FLORESTA; i++) {
        pthread_join(sensores_threads[i], NULL);
    }

    return 0;
}