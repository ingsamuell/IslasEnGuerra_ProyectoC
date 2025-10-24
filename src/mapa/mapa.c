/* src/mapa/mapa.c (Versión C89 - Corregida) */

#include "raylib.h"
#include "mapa.h" 

/* Definimos el tamaño de la celda */
#define TAMANO_CELDA 50

/*
 * Función: inicializarMapa
 * -------------------------
 */
void inicializarMapa(char mapa[FILAS][COLUMNAS]) {
    
    /* Declaramos las variables al inicio del bloque */
    int i, j;

    /* 1. Llenamos todo el mapa de agua (~) */
    for (i = 0; i < FILAS; i++) {
        for (j = 0; j < COLUMNAS; j++) {
            mapa[i][j] = '~';
        }
    }

    /* 2. Colocamos la isla del jugador (P) */
    mapa[1][1] = 'P';
    mapa[1][2] = '.'; 
    mapa[2][1] = '.';
    mapa[2][2] = '.';

    /* 3. Colocamos una isla enemiga (E) */
    mapa[7][7] = 'E';
    mapa[8][7] = '.';
    mapa[7][8] = '.';

    /* 4. Colocamos algunos recursos ($) */
    mapa[3][5] = '$';
    mapa[8][2] = '$';
}


/*
 * Función: dibujarMapa
 * --------------------
 */
void dibujarMapa(char mapa[FILAS][COLUMNAS]) {
    
    /* Declaramos todas las variables al inicio del bloque */
    int i, j;
    int posX, posY;
    Color colorCelda;
    
    for (i = 0; i < FILAS; i++) {
        for (j = 0; j < COLUMNAS; j++) {
            
            /* Asignamos valores (sin declarar) */
            posX = j * TAMANO_CELDA;
            posY = i * TAMANO_CELDA;
            
            switch (mapa[i][j]) {
                case 'P':
                    colorCelda = GREEN; 
                    break;
                case 'E':
                    colorCelda = RED; 
                    break;
                case '$':
                    colorCelda = YELLOW; 
                    break;
                case '.':
                    colorCelda = BROWN; 
                    break;
                case '~':
                default:
                    colorCelda = BLUE; 
                    break;
            }

            DrawRectangle(posX, posY, TAMANO_CELDA, TAMANO_CELDA, colorCelda);
        }
    }
}