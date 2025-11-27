#include "barrera.h"
#include <stdio.h>

int barrera_init(Barrera* barrera, int N){
    if (barrera == NULL || N <= 0){
        fprintf(stderr, "Error, barrera NULL o N invalido\n");
        return -1;
    }

    barrera->count = 0;
    barrera->N = N;
    barrera->etapa = 0;

    if (pthread_mutex_init(&barrera->mutex, NULL) != 0){
        fprintf(stderr, "Error, no se pudo inicializar el mutex\n");
        return -1;
    }

    if (pthread_cond_init(&barrera->cond, NULL) != 0){
        fprintf(stderr, "Error, no se pudo inicializar la variable de condicion");
        pthread_mutex_destroy(&barrera->mutex);
        return -1;
    }

    printf("[BARRERA] Inicializada para %d hebras\n", N);
    return 0;
}

int barrera_destroy(Barrera* barrera){
    if (barrera == NULL){
        return -1;
    }

    pthread_mutex_destroy(&barrera->mutex);
    pthread_cond_destroy(&barrera->cond);

    printf("[BARRERA] Destruida correctamente");
    return 0;
}

int barrera_wait(Barrera* barrera){
    if (barrera == NULL){
        return -1;
    }

    pthread_mutex_lock(&barrera->mutex);

    int mi_etapa = barrera->etapa;

    barrera->count++;

    printf("[Hebra %ld] Llego a barrera (count=%d/%d, etapa=%d)\n", pthread_self(), barrera->count, barrera->N, barrera->etapa);

    if (barrera->count == barrera->N){
        printf("[Hebra %ld] Ultima hebra\n", pthread_self());
        barrera->count = 0;
        barrera->etapa++;
        pthread_cond_broadcast(&barrera->cond);
    }
    else{
        printf("[Hebra %ld] Esperando... (count=%d/%d)\n", pthread_self(), barrera->count, barrera->N);

        while (mi_etapa == barrera->etapa){
            pthread_cond_wait(&barrera->cond, &barrera->mutex);
        }
        printf("[Hebra %ld] Pasando la barrera\n", pthread_self());
    }
    pthread_mutex_unlock(&barrera->mutex);
    return 0;
}