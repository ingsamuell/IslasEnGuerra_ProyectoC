#ifndef MAPA_H
#define MAPA_H

#include <windows.h>
#include <wingdi.h>

/* --- Definiciones del Juego --- */

/* 1. TAMAÑO DEL MUNDO (El mapa completo, sin cambios) */
#define MUNDO_FILAS 100
#define MUNDO_COLUMNAS 100

/* 2. TAMAÑO GRÁFICO (¡NUEVO!) */
/* Este es nuestro "nivel de zoom". Ya no cambiará. */
#define TAMANO_CELDA 30

/* 3. PANTALLA_FILAS y PANTALLA_COLUMNAS ya NO se definen aquí. */
/* Se calcularán dinámicamente. */

/* Estructura del Jugador (sin cambios) */
typedef struct { int x; int y; } Jugador;
/* Estructura de la Cámara (sin cambios) */
typedef struct { int x; int y; } Camera;

/* --- Prototipos de Funciones --- */

/* Lógica del Juego */
void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy);

/* ¡MODIFICADO! Ahora debe saber cuántas celdas caben en la pantalla */
void actualizarCamara(Camera *camara, Jugador jugador, int pantallaFilas, int pantallaColumnas);

/* ¡MODIFICADO! También necesita saber cuántas celdas dibujar */
void dibujarMapa(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera camara, int pantallaFilas, int pantallaColumnas);
void dibujarJugador(HDC hdc, Jugador jugador, Camera camara);

#endif