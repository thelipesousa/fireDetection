#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>

#define SIZE 30  // Tamanho da matriz

typedef enum {
    EMPTY,    // Área livre '-'
    SENSOR,   // Nó sensor 'T'
    FIRE,     // Em chamas '@'
    BURNED    // Queimado '/'
} CellState;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    CellState state;
    int x, y;
    int is_edge;
} SensorNode;

extern SensorNode forest[SIZE][SIZE];
void initialize_forest();
void print_forest();
void fight_fire(int x, int y);
void* sensor_node_func(void* arg);
void* fire_generator_func(void* arg);
void* central_control_func(void* arg);

#endif
