#ifndef MAPA_H
#define MAPA_H

/* Incluimos las definiciones de windows (HDC, etc) */
#include "global.h"

/* --- Definiciones del Juego --- */
#define FILAS 10
#define COLUMNAS 10
#define TAMANO_CELDA 50

/* Estructura para guardar la posición del jugador */
typedef struct {
    int x; /* Columna */
    int y; /* Fila */
} Jugador;

/* --- Prototipos de Funciones --- */

/* Lógica del Juego (no GDI) */
void inicializarMapa(char mapa[FILAS][COLUMNAS]);
void moverJugador(Jugador *jugador, char mapa[FILAS][COLUMNAS], int dx, int dy);

/* Gráficos del Juego (GDI) */
void dibujarMapa(HDC hdc, char mapa[FILAS][COLUMNAS]);
void dibujarJugador(HDC hdc, Jugador jugador);

#endif