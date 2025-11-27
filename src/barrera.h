#ifndef BARRERA_H
#define BARRERA_H

#include <pthread.h>

typedef struct {
    int count;
    int N;
    int etapa;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Barrera;

int barrera_init(Barrera* barrera, int N);
int barrera_destroy(Barrera* barrera);
int barrera_wait(Barrera* barrera);

#endif