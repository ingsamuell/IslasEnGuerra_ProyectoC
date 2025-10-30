/* src/mapa/mapa.c (Versión GDI - C89) */

#include "mapa.h"

/* --- Lógica del Juego --- */

void inicializarMapa(char mapa[FILAS][COLUMNAS]) {
    int i, j;
    for (i = 0; i < FILAS; i++) {
        for (j = 0; j < COLUMNAS; j++) {
            mapa[i][j] = '~';
        }
    }
    mapa[1][1] = 'P'; /* 'P' es la tierra de la isla */
    mapa[1][2] = '.';
    mapa[2][1] = '.';
    mapa[2][2] = '.';
    mapa[7][7] = 'E';
    mapa[8][7] = '.';
    mapa[7][8] = '.';
    mapa[3][5] = '$';
    mapa[8][2] = '$';
}

void moverJugador(Jugador *jugador, char mapa[FILAS][COLUMNAS], int dx, int dy) {
    int nuevoX = jugador->x + dx;
    int nuevoY = jugador->y + dy;

    if (nuevoX < 0 || nuevoX >= COLUMNAS || nuevoY < 0 || nuevoY >= FILAS) {
        return; /* Fuera de los límites */
    }
    if (mapa[nuevoY][nuevoX] == '~') {
        return; /* No se puede mover al agua */
    }
    
    /* Movimiento válido */
    jugador->x = nuevoX;
    jugador->y = nuevoY;
}

/* --- Gráficos del Juego (GDI) --- */

void dibujarMapa(HDC hdc, char mapa[FILAS][COLUMNAS]) {
    /* Variables GDI */
    HBRUSH hBrushAgua = CreateSolidBrush(RGB(0, 0, 255));
    HBRUSH hBrushTierra = CreateSolidBrush(RGB(139, 69, 19));
    HBRUSH hBrushEnemigo = CreateSolidBrush(RGB(255, 0, 0));
    HBRUSH hBrushRecurso = CreateSolidBrush(RGB(255, 255, 0));
    HBRUSH hBrushActual;
    
    /* Variables de bucle y GDI */
    int i, j;
    RECT rectCelda;

    for (i = 0; i < FILAS; i++) {
        for (j = 0; j < COLUMNAS; j++) {
            
            switch (mapa[i][j]) {
                case 'P':
                case '.':
                    hBrushActual = hBrushTierra;
                    break;
                case 'E':
                    hBrushActual = hBrushEnemigo;
                    break;
                case '$':
                    hBrushActual = hBrushRecurso;
                    break;
                case '~':
                default:
                    hBrushActual = hBrushAgua;
                    break;
            }

            /* Definimos las coordenadas del rectángulo */
            rectCelda.left = j * TAMANO_CELDA;
            rectCelda.top = i * TAMANO_CELDA;
            rectCelda.right = (j + 1) * TAMANO_CELDA;
            rectCelda.bottom = (i + 1) * TAMANO_CELDA;

            /* Dibujamos el rectángulo relleno */
            FillRect(hdc, &rectCelda, hBrushActual);
        }
    }
    
    /* Liberamos los recursos GDI */
    DeleteObject(hBrushAgua);
    DeleteObject(hBrushTierra);
    DeleteObject(hBrushEnemigo);
    DeleteObject(hBrushRecurso);
}

void dibujarJugador(HDC hdc, Jugador jugador) {
    HBRUSH hBrushJugador = CreateSolidBrush(RGB(255, 255, 255)); /* Blanco */
    RECT rectJugador;

    rectJugador.left = jugador.x * TAMANO_CELDA;
    rectJugador.top = jugador.y * TAMANO_CELDA;
    rectJugador.right = (jugador.x + 1) * TAMANO_CELDA;
    rectJugador.bottom = (jugador.y + 1) * TAMANO_CELDA;

    FillRect(hdc, &rectJugador, hBrushJugador);

    DeleteObject(hBrushJugador);
} 