#include "reloj.h"
#include "sim.h"
#include <stdio.h>

// Algoritmo de reemplazo Clock (Second Chance)
// Mantiene un puntero circular que recorre los marcos
// Cuando necesita reemplazar:
//   - Si el bit de referencia (usado) esta en 1, lo pone en 0 y avanza
//   - Si el bit esta en 0, selecciona ese marco para reemplazo
// 'usado' = 1 significa "pagina recientemente referenciada"
// Al buscar victima, damos segunda oportunidad reseteando a 0
// Retorna: indice del marco a reemplazar
int reloj_reemplazo() {
    int marco_victima = -1;
    int vueltas = 0;
    int max_vueltas = Nmarcos * 2; // Evitar bucle infinito
    
    printf("    [CLOCK] Buscando marco victima (puntero inicial: %d)\n", puntero);
    
    while (marco_victima == -1 && vueltas < max_vueltas) {
        // Si el marco actual tiene segunda oportunidad (usado = 1)
        // le damos otra oportunidad y lo marcamos como no usado
        if (marcos[puntero].usado == 1) {
            printf("    [CLOCK] Marco %d (pagina %d) tiene segunda oportunidad, reseteando...\n", 
                   puntero, marcos[puntero].pagina);
            marcos[puntero].usado = 0;
        } else {
            // Este marco es la victima (usado = 0)
            marco_victima = puntero;
            printf("    [CLOCK] Marco %d seleccionado como victima\n", marco_victima);
        }
        
        // Avanzar el puntero (circular)
        puntero = (puntero + 1) % Nmarcos;
        vueltas++;
    }
    
    // Si no encontramos victima (todos tenian segunda oportunidad),
    // tomamos el marco actual del puntero
    if (marco_victima == -1) {
        marco_victima = puntero;
        puntero = (puntero + 1) % Nmarcos;
        printf("    [CLOCK] Forzando marco %d como victima\n", marco_victima);
    }
    
    return marco_victima;
}
