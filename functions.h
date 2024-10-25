#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>

#define TAMANHO_FLORESTA 30

typedef struct {
    int x;
    int y;
} sensor_args;

void* gerador_incendios(void* arg);
void* sensor(void* args);
void* central_controle(void* arg);
void enviar_para_central(int x, int y);
void enviar_para_vizinho(int x, int y);

#endif