# Tarea 2 - Parte I: Barrera Reutilizable

### Integrantes:
- Joseph Matamala
- Juan Pablo Núñez
- Agustín Salgado
- Ignacio Silva


Implementación de una barrera reutilizable utilizando monitores (`pthread_mutex_t` y `pthread_cond_t`) en C.

## Requisitos
* GCC (Compilador de C)
* Entorno Linux/Unix
* Biblioteca `pthread`

## Compilación

El proyecto incluye un `Makefile` para facilitar la compilación.

Escribimos en consola:
```bash
make run
```
El programa tomará por defecto 5 hebras y 4 etapas.

O manualmente, para controlar cuantas hebras y etapas:

```bash
gcc -pthread -o main src/main.c src/barrera.c
```

Luego utilizamos

```bash
./main [N] [E]
```
siendo N el número de hebras y E el número de etapas.


## Verificación 

Criterios de éxito:

- **Orden de llegada**: Todas las hebras deben imprimir `[tid] esperando en etapa X` antes de que alguna imprima `[tid] paso barrera en etapa X`.

- **Sincronización de etapas**: Ninguna hebra debe avanzar a la etapa X+1 (imprimir "esperando en etapa X+1") hasta que todas las hebras hayan completado la etapa X (imprimir "paso barrera en etapa X").

Si el log muestra que las etapas no se mezclan, la barrera funciona correctamente.