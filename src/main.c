#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "barrera.h"

Barrera barrera_global; 
int num_etapas = 4;

typedef struct {
    int tid;
} DatosHebra;

void* funcion_hebra(void* arg){
    DatosHebra* datos = (DatosHebra*)arg;
    int tid = datos->tid;
    free(datos);

    for (int e = 0; e < num_etapas; e++){
        int tiempo_trabajo = (rand() % 500000) + 100000;
        usleep(tiempo_trabajo);
        printf("[%d] esperando en etapa %d\n", tid, e);
        fflush(stdout);
        barrera_wait(&barrera_global);
        printf("[%d] paso barrera en etapa %d\n", tid, e);
        fflush(stdout);
    }

    return NULL;
}

void mostrar_uso(const char* nombre_programa) {
    printf("Uso: %s [N] [E]\n", nombre_programa);
    printf("  N: Número de hebras (default: 5)\n");
    printf("  E: Número de etapas (default: 4)\n");
    printf("\nEjemplos:\n");
    printf("  %s           # 5 hebras, 4 etapas\n", nombre_programa);
    printf("  %s 3         # 3 hebras, 4 etapas\n", nombre_programa);
    printf("  %s 4 6       # 4 hebras, 6 etapas\n", nombre_programa);
}


int main(int argc, char* argv[]) {
    int N = 5;
    if (argc > 1) {
        N = atoi(argv[1]);
        if (N <= 0) {
            fprintf(stderr, "Error: N debe ser mayor que 0\n");
            mostrar_uso(argv[0]);
            return 1;
        }
    }
    
    if (argc > 2){
        num_etapas = atoi(argv[2]);
        if (num_etapas <= 0){
            fprintf(stderr, "Error: E debe ser mayor que 0\n");
            mostrar_uso(argv[0]);
            return 1;
        }
    }

    srand(time(NULL));
    printf("\n");
    printf("Configuración:\n");
    printf("  • Número de hebras (N): %d\n", N);
    printf("  • Número de etapas (E): %d\n", num_etapas);
    printf("\n");
    
    if (barrera_init(&barrera_global, N) != 0) {
        fprintf(stderr, "Error al inicializar barrera\n");
        return 1;
    }
    
    pthread_t* hebras = malloc(N * sizeof(pthread_t));
    if (hebras == NULL) {
        fprintf(stderr, "Error al asignar memoria\n");
        barrera_destroy(&barrera_global);
        return 1;
    }
    
    printf("════════════════════════════════════════════════════════════\n");
    printf("Iniciando ejecución...\n");
    printf("════════════════════════════════════════════════════════════\n\n");

    for (int i = 0; i < N; i++) {
        DatosHebra* datos = malloc(sizeof(DatosHebra));
        if (datos == NULL){
            fprintf(stderr, "Error: No se pudo asignar memoria para datos de hebra %d", i);
        
            for (int j = 0; j < i; j++){
                pthread_join(hebras[j], NULL);
            }

            free(hebras);
            barrera_destroy(&barrera_global);
            return 1;
        }

        datos->tid = i;

        if (pthread_create(&hebras[i], NULL, funcion_hebra, datos) != 0){
            fprintf(stderr, "Error: No se pudo crear la hebra %d\n", i);
            free(datos);

            for (int j = 0; j < i; j++){
                pthread_join(hebras[j], NULL);
            }

            free(hebras);
            barrera_destroy(&barrera_global);
            return 1;
        }
    }

    for (int i = 0; i < N; i++){
        pthread_join(hebras[i], NULL);
    }

    free(hebras);
    barrera_destroy(&barrera_global);
    
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("EJECUCIÓN COMPLETADA\n");
    printf("════════════════════════════════════════════════════════════\n\n");
}