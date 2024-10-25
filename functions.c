#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "functions.h"

SensorNode forest[SIZE][SIZE];
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

// Variáveis externas para comunicação com a central
extern pthread_mutex_t central_mutex;
extern pthread_cond_t central_cond;

// Função para inicializar a floresta e os nós sensores
void initialize_forest() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int random_value = rand() % 2;
            if (random_value == 0) {
                forest[i][j].state = EMPTY;
            } else {
                forest[i][j].state = SENSOR;
            }
            forest[i][j].x = i;
            forest[i][j].y = j;
            forest[i][j].is_edge = (i == 0 || i == SIZE - 1 || j == 0 || j == SIZE - 1);
            pthread_mutex_init(&forest[i][j].mutex, NULL);
            pthread_cond_init(&forest[i][j].cond, NULL);
        }
    }
}

// Função para imprimir o estado atual da floresta
void print_forest() {
    pthread_mutex_lock(&print_mutex);

    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            switch (forest[i][j].state) {
                case EMPTY:
                    printf("- ");
                    break;
                case SENSOR:
                    printf("T ");
                    break;
                case FIRE:
                    printf("@ ");
                    break;
                case BURNED:
                    printf("/ ");
                    break;
            }
        }
        printf("\n");
    }
    pthread_mutex_unlock(&print_mutex);
}

// Função para combater o fogo em uma determinada célula
void fight_fire(int x, int y) {
    pthread_mutex_lock(&forest[x][y].mutex);
    if (forest[x][y].state == FIRE) {
        forest[x][y].state = BURNED;
        printf("Central: Combate ao fogo na posição (%d, %d)\n", x, y);
    }
    pthread_mutex_unlock(&forest[x][y].mutex);
}

// Função da thread do nó sensor
void* sensor_node_func(void* arg) {
    SensorNode* node = (SensorNode*)arg;

    while (1) {
        pthread_mutex_lock(&node->mutex);
        while (node->state != FIRE) {
            pthread_cond_wait(&node->cond, &node->mutex);
        }
        printf("Sensor (%d, %d): Fogo detectado!\n", node->x, node->y);

        // Notificar vizinhos
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nx = node->x + dx;
                int ny = node->y + dy;
                if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && (dx != 0 || dy != 0)) {
                    pthread_mutex_lock(&forest[nx][ny].mutex);
                    if (forest[nx][ny].state == SENSOR || forest[nx][ny].state == EMPTY) {
                        forest[nx][ny].state = FIRE;
                        pthread_cond_signal(&forest[nx][ny].cond);
                    }
                    pthread_mutex_unlock(&forest[nx][ny].mutex);
                }
            }
        }

        // Se estiver na borda, notificar a central
        if (node->is_edge) {
            pthread_mutex_lock(&central_mutex);
            pthread_cond_signal(&central_cond);
            pthread_mutex_unlock(&central_mutex);
        }

        pthread_mutex_unlock(&node->mutex);

        sleep(2);  // Simular tempo de queima antes de mudar para queimado

        // Mudar estado para queimado
        pthread_mutex_lock(&node->mutex);
        if (node->state == FIRE) {
            node->state = BURNED;
            printf("Sensor (%d, %d): Célula queimada.\n", node->x, node->y);
        }
        pthread_mutex_unlock(&node->mutex);
    }
    pthread_exit(NULL);
}

// Função da thread geradora de incêndios
void* fire_generator_func(void* arg) {
    while (1) {
        sleep(3);
        int x = rand() % SIZE;
        int y = rand() % SIZE;

        pthread_mutex_lock(&forest[x][y].mutex);
        if (forest[x][y].state == EMPTY || forest[x][y].state == SENSOR) {
            forest[x][y].state = FIRE;
            pthread_cond_signal(&forest[x][y].cond);
            printf("Gerador de Incêndio: Fogo iniciado em (%d, %d)\n", x, y);
        }
        pthread_mutex_unlock(&forest[x][y].mutex);
    }
    pthread_exit(NULL);
}

// Função da thread central de controle
void* central_control_func(void* arg) {
    while (1) {
        pthread_mutex_lock(&central_mutex);
        pthread_cond_wait(&central_cond, &central_mutex);

        for (int i = 0; i < SIZE; i++) {
            int edges[4][2] = {{0, i}, {SIZE - 1, i}, {i, 0}, {i, SIZE - 1}};
            for (int e = 0; e < 4; e++) {
                int x = edges[e][0];
                int y = edges[e][1];
                pthread_mutex_lock(&forest[x][y].mutex);
                if (forest[x][y].state == FIRE) {
                    fight_fire(x, y);
                }
                pthread_mutex_unlock(&forest[x][y].mutex);
            }
        }
        pthread_mutex_unlock(&central_mutex);
        sleep(1);
    }
    pthread_exit(NULL);
}
