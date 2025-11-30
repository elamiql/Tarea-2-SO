#ifndef SIM_H
#define SIM_H

typedef struct {
    int pagina;
    int usado;
} Marco;

extern Marco *marcos;
extern int Nmarcos;
extern int puntero;

int buscar_pagina(int nvp);
int asignar_marco(int nvp);
unsigned long traducir(int marco, int offset, int b);

#endif
