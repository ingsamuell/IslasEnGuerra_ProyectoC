/* Contenido completo de src/mapa/mapa.h */

#ifndef MAPA_H
#define MAPA_H

/* Definimos el tamaño del mapa (10x10) */
#define FILAS 10
#define COLUMNAS 10

/* Prototipos de las funciones que usará main.c */
void inicializarMapa(char mapa[FILAS][COLUMNAS]);
void dibujarMapa(char mapa[FILAS][COLUMNAS]);

#endif /* MAPA_H */