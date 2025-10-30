#ifndef MAPA_H
#define MAPA_H

#include <windows.h> /* Incluimos windows.h aquí */
#include <wingdi.h>

/* --- Definiciones del Juego --- */

/* 1. TAMAÑO DEL MUNDO (El mapa completo) */
#define MUNDO_FILAS 100
#define MUNDO_COLUMNAS 100

/* 2. TAMAÑO DE LA PANTALLA (Lo que ve la cámara, en celdas) */
#define PANTALLA_FILAS 15
#define PANTALLA_COLUMNAS 20

/* 3. TAMAÑO GRÁFICO (En píxeles) */
#define TAMANO_CELDA 30

/* Estructura para guardar la posición del jugador (en el MUNDO) */
typedef struct {
    int x; /* Columna */
    int y; /* Fila */
} Jugador;

/* Estructura para la cámara (posición de su esquina superior-izquierda en el MUNDO) */
typedef struct {
    int x; /* Columna */
    int y; /* Fila */
} Camera;


/* --- Prototipos de Funciones --- */

/* Lógica del Juego */
void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy);

/* NUEVA: Actualiza la posición de la cámara para seguir al jugador */
void actualizarCamara(Camera *camara, Jugador jugador);

/* Gráficos del Juego (Ahora reciben la cámara) */
void dibujarMapa(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera camara);
void dibujarJugador(HDC hdc, Jugador jugador, Camera camara);

#endif