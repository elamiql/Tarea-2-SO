#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sim.h"
#include "reloj.h"
#include "utils.h"

// Variables globales
Marco *marcos = NULL;
int Nmarcos = 0;
int puntero = 0;

// Estadisticas
int total_referencias = 0;
int page_faults = 0;
int page_hits = 0;

// Busca si una pagina virtual esta en memoria
// Retorna el indice del marco si esta, -1 si no
int buscar_pagina(int nvp) {
    for (int i = 0; i < Nmarcos; i++) {
        if (marcos[i].usado && marcos[i].pagina == nvp) {
            return i;
        }
    }
    return -1;
}

// Asigna un marco a una pagina virtual
// Primero busca marcos libres, si no hay usa Clock
int asignar_marco(int nvp) {
    // Primero buscar si hay marcos libres
    for (int i = 0; i < Nmarcos; i++) {
        if (!marcos[i].usado) {
            marcos[i].pagina = nvp;
            marcos[i].usado = 1;
            printf("  [ASIGNACION] Pagina %d asignada al marco LIBRE %d\n", nvp, i);
            return i;
        }
    }
    
    // Si no hay marcos libres, usar algoritmo de reemplazo
    printf("  [REEMPLAZO] Memoria llena, ejecutando algoritmo Clock...\n");
    int marco_victima = reloj_reemplazo(nvp);
    printf("  [REEMPLAZO] Pagina %d reemplazada por pagina %d en marco %d\n", 
           marcos[marco_victima].pagina, nvp, marco_victima);
    marcos[marco_victima].pagina = nvp;
    marcos[marco_victima].usado = 1;
    
    return marco_victima;
}

// Traduce direccion virtual a fisica
// Formula: dir_fisica = (marco << b) | offset
unsigned long traducir(int marco, int offset, int b) {
    unsigned long dir_fisica = ((unsigned long)marco << b) | offset;
    return dir_fisica;
}

void mostrar_uso(const char* programa) {
    printf("Uso: %s <num_marcos> <bits_pagina> [archivo_trazas]\n", programa);
    printf("\n");
    printf("Parametros:\n");
    printf("  num_marcos    : Numero de marcos de pagina en memoria fisica\n");
    printf("  bits_pagina   : Numero de bits para el offset (tamaño pagina = 2^bits)\n");
    printf("  archivo_trazas: (Opcional) Archivo con direcciones virtuales\n");
    printf("                  Si no se especifica, lee desde stdin\n");
    printf("\n");
    printf("Ejemplos:\n");
    printf("  %s 4 12              # 4 marcos, paginas de 4KB (2^12)\n", programa);
    printf("  %s 8 10 trazas.txt   # 8 marcos, paginas de 1KB, desde archivo\n", programa);
    printf("\n");
}

void mostrar_estado_memoria() {
    printf("\n┌─────────────────────────────────────┐\n");
    printf("│      ESTADO DE LA MEMORIA           │\n");
    printf("├───────┬──────────┬──────────────────┤\n");
    printf("│ Marco │  Pagina  │      Estado      │\n");
    printf("├───────┼──────────┼──────────────────┤\n");
    
    for (int i = 0; i < Nmarcos; i++) {
        if (marcos[i].usado) {
            printf("│   %2d  │   %4d   │      OCUPADO     │\n", i, marcos[i].pagina);
        } else {
            printf("│   %2d  │    --    │      LIBRE       │\n", i);
        }
    }
    
    printf("└───────┴──────────┴──────────────────┘\n");
    printf("Puntero Clock: %d\n\n", puntero);
}

void mostrar_estadisticas() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║         ESTADISTICAS FINALES           ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║ Total de referencias:  %8d       ║\n", total_referencias);
    printf("║ Page Hits:             %8d       ║\n", page_hits);
    printf("║ Page Faults:           %8d       ║\n", page_faults);
    printf("╠════════════════════════════════════════╣\n");
    
    if (total_referencias > 0) {
        double hit_rate = (double)page_hits / total_referencias * 100.0;
        double fault_rate = (double)page_faults / total_referencias * 100.0;
        printf("║ Tasa de Hits:          %7.2f%%      ║\n", hit_rate);
        printf("║ Tasa de Faults:        %7.2f%%      ║\n", fault_rate);
    }
    
    printf("╚════════════════════════════════════════╝\n\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        mostrar_uso(argv[0]);
        return 1;
    }
    
    // Parsear argumentos
    Nmarcos = atoi(argv[1]);
    int b = atoi(argv[2]);
    
    if (Nmarcos <= 0 || b <= 0 || b > 31) {
        fprintf(stderr, "Error: Parametros invalidos\n");
        fprintf(stderr, "  - num_marcos debe ser > 0\n");
        fprintf(stderr, "  - bits_pagina debe estar entre 1 y 31\n");
        mostrar_uso(argv[0]);
        return 1;
    }
    
    unsigned long tamano_pagina = 1UL << b;
    
    // Inicializar memoria
    marcos = (Marco*)calloc(Nmarcos, sizeof(Marco));
    if (marcos == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para los marcos\n");
        return 1;
    }
    
    // Configuracion
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("   SIMULADOR DE MEMORIA VIRTUAL - Algoritmo Clock\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("\nConfiguracion:\n");
    printf("  • Numero de marcos:      %d\n", Nmarcos);
    printf("  • Bits de offset:        %d bits\n", b);
    printf("  • Tamaño de pagina:      %lu bytes (%lu KB)\n", 
           tamano_pagina, tamano_pagina / 1024);
    printf("\n");
    
    // Abrir archivo de entrada si se especificó
    FILE *input = stdin;
    if (argc > 3) {
        input = fopen(argv[3], "r");
        if (input == NULL) {
            fprintf(stderr, "Error: No se pudo abrir el archivo '%s'\n", argv[3]);
            free(marcos);
            return 1;
        }
        printf("Leyendo trazas desde: %s\n", argv[3]);
    } else {
        printf("Leyendo direcciones desde stdin (Ctrl+D para terminar)...\n");
    }
    
    printf("═══════════════════════════════════════════════════════════\n\n");
    
    // Procesar referencias de memoria
    char linea[256];
    unsigned long mascara_offset = (1UL << b) - 1;
    
    while (fgets(linea, sizeof(linea), input) != NULL) {
        // Ignorar lineas vacias y comentarios
        if (linea[0] == '\n' || linea[0] == '#') {
            continue;
        }
        
        unsigned long dir_virtual = parse_address(linea);
        total_referencias++;
        
        // Extraer numero de pagina virtual y offset
        int nvp = dir_virtual >> b;
        unsigned long offset = dir_virtual & mascara_offset;
        
        printf("[REF %d] Dir Virtual: 0x%lx (Pagina: %d, Offset: 0x%lx)\n", 
               total_referencias, dir_virtual, nvp, offset);
        
        // Buscar pagina en memoria
        int marco = buscar_pagina(nvp);
        
        if (marco != -1) {
            // Page Hit
            page_hits++;
            printf("  ✓ [HIT] Pagina %d ya esta en marco %d\n", nvp, marco);
        } else {
            // Page Fault
            page_faults++;
            printf("  ✗ [FAULT] Pagina %d no esta en memoria\n", nvp);
            marco = asignar_marco(nvp);
        }
        
        // Traducir a direccion fisica
        unsigned long dir_fisica = traducir(marco, offset, b);
        printf("  → Dir Fisica: 0x%lx (Marco: %d, Offset: 0x%lx)\n\n", 
               dir_fisica, marco, offset);
        
        // Mostrar estado cada 5 referencias (opcional)
        if (total_referencias % 10 == 0) {
            mostrar_estado_memoria();
        }
    }
    
    // Cerrar archivo si no es stdin
    if (input != stdin) {
        fclose(input);
    }
    
    // Mostrar estado final
    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("   ESTADO FINAL DE LA MEMORIA\n");
    printf("═══════════════════════════════════════════════════════════\n");
    mostrar_estado_memoria();
    
    // Mostrar estadisticas
    mostrar_estadisticas();
    
    // Liberar memoria
    free(marcos);
    
    return 0;
}
