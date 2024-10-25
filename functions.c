#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

extern char floresta[TAMANHO_FLORESTA][TAMANHO_FLORESTA];
extern pthread_mutex_t mutex_flores[TAMANHO_FLORESTA][TAMANHO_FLORESTA];


typedef struct {
    int x;
    int y;
    int detectado;
} Incendio;
Incendio incendios[TAMANHO_FLORESTA * TAMANHO_FLORESTA];
int contador_incendios = 0;
int combate_rodada = 0;

void inicializar_sensores() {
    int num_sensores = TAMANHO_FLORESTA;
    srand(time(NULL));

    for (int i = 0; i < num_sensores; i++) {
        int x = rand() % TAMANHO_FLORESTA;
        int y = rand() % TAMANHO_FLORESTA;
        pthread_mutex_lock(&mutex_flores[x][y]);
        floresta[x][y] = 'T';
        pthread_mutex_unlock(&mutex_flores[x][y]);
    }
}

void* gerador_incendios(void* arg) {
    while (1) {
        sleep(3);
        int x = rand() % TAMANHO_FLORESTA;
        int y = rand() % TAMANHO_FLORESTA;

        pthread_mutex_lock(&mutex_flores[x][y]);
        if (floresta[x][y] == '-') {
            floresta[x][y] = '@';
            printf("Incendio iniciado em (%d, %d)\n", x, y);
        }
        pthread_mutex_unlock(&mutex_flores[x][y]);
    }
    return NULL;
}

void* sensor(void* args) {
    sensor_args* pos = (sensor_args*)args;
    int x = pos->x;
    int y = pos->y;

    while (1) {
        sleep(1);

        pthread_mutex_lock(&mutex_flores[x][y]);
        if (floresta[x][y] == '@') {
            int incendio_detectado = 0;


            for (int i = 0; i < contador_incendios; i++) {
                if (incendios[i].x == x && incendios[i].y == y) {
                    incendio_detectado = 1;
                    break;
                }
            }


            if (!incendio_detectado) {
                printf("Sensor em (%d, %d) detectou fogo!\n", x, y);
                incendios[contador_incendios].x = x;
                incendios[contador_incendios].y = y;
                incendios[contador_incendios].detectado = 1;
                contador_incendios++;
            }
        }
        pthread_mutex_unlock(&mutex_flores[x][y]);
    }
    return NULL;
}

void exibir_floresta() {
    printf("\nEstado atual da floresta:\n");
    for (int i = 0; i < TAMANHO_FLORESTA; i++) {
        for (int j = 0; j < TAMANHO_FLORESTA; j++) {
            printf("%c ", floresta[i][j]);
        }
        printf("\n");
    }
}

void* central_controle(void* arg) {
    while (1) {
        sleep(3);

        if (combate_rodada == 0) {

            printf("\n--- Rodada de Detecção ---\n");
            exibir_floresta();
            printf("Central está monitorando incendios...\n");
        } else {

            printf("\n--- Rodada de Combate ---\n");
            for (int i = 0; i < contador_incendios; i++) {
                int x = incendios[i].x;
                int y = incendios[i].y;

                pthread_mutex_lock(&mutex_flores[x][y]);
                if (floresta[x][y] == '@') {
                    floresta[x][y] = '/';
                    printf("Central: Incendio em (%d, %d) foi combatido e apagado.\n", x, y);
                }
                pthread_mutex_unlock(&mutex_flores[x][y]);
            }


            contador_incendios = 0;
            exibir_floresta();
            printf("Central terminou o combate aos incendios.\n");
        }


        combate_rodada = 1 - combate_rodada;
    }
    return NULL;
}
