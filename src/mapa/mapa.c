/* src/mapa/mapa.c (Versión GDI con Cámara) */

#include "mapa.h"

/* --- Lógica del Juego --- */

void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    int i, j;
    for (i = 0; i < MUNDO_FILAS; i++) {
        for (j = 0; j < MUNDO_COLUMNAS; j++) {
            /* Océano por defecto */
            mapa[i][j] = '~';
        }
    }
    
    /* Dibuja una isla grande */
    for (i = 5; i < 15; i++) {
        for (j = 5; j < 15; j++) {
            mapa[i][j] = '.';
        }
    }
    mapa[6][7] = 'P'; /* Punto de spawn */

    /* Dibuja una isla enemiga */
    for (i = 20; i < 25; i++) {
        for (j = 30; j < 40; j++) {
            mapa[i][j] = 'E';
        }
    }
    
    /* Dibuja una isla de recursos */
    mapa[10][30] = '$';
    mapa[10][31] = '$';
    mapa[11][30] = '$';
}

void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy) {
    int nuevoX = jugador->x + dx;
    int nuevoY = jugador->y + dy;

    /* Límites del MUNDO */
    if (nuevoX < 0 || nuevoX >= MUNDO_COLUMNAS || nuevoY < 0 || nuevoY >= MUNDO_FILAS) {
        return;
    }
    if (mapa[nuevoY][nuevoX] == '~') {
        return; /* No se puede mover al agua */
    }
    
    jugador->x = nuevoX;
    jugador->y = nuevoY;
}

/* --- Lógica de la Cámara --- */

/* Mueve la cámara para que el jugador esté en el centro */
void actualizarCamara(Camera *camara, Jugador jugador) {
    /* 1. Centra la cámara en el jugador */
    camara->x = jugador.x - (PANTALLA_COLUMNAS / 2);
    camara->y = jugador.y - (PANTALLA_FILAS / 2);

    /* 2. "Clamping" - Evita que la cámara se salga del mundo */
    if (camara->x < 0) {
        camara->x = 0;
    }
    if (camara->y < 0) {
        camara->y = 0;
    }
    if (camara->x > MUNDO_COLUMNAS - PANTALLA_COLUMNAS) {
        camara->x = MUNDO_COLUMNAS - PANTALLA_COLUMNAS;
    }
    if (camara->y > MUNDO_FILAS - PANTALLA_FILAS) {
        camara->y = MUNDO_FILAS - PANTALLA_FILAS;
    }
}

/* --- Gráficos del Juego (GDI) --- */

void dibujarMapa(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera camara) {
    /* Variables GDI */
    HBRUSH hBrushAgua = CreateSolidBrush(RGB(0, 0, 255));
    HBRUSH hBrushTierra = CreateSolidBrush(RGB(139, 69, 19));
    HBRUSH hBrushEnemigo = CreateSolidBrush(RGB(255, 0, 0));
    HBRUSH hBrushRecurso = CreateSolidBrush(RGB(255, 255, 0));
    HBRUSH hBrushActual;
    
    /* Variables de bucle y GDI */
    int y_pantalla, x_pantalla; /* Posición en la PANTALLA */
    int y_mundo, x_mundo;       /* Posición en el MUNDO */
    RECT rectCelda;

    /* Iteramos sobre la PANTALLA (ej. 0 a 15) */
    for (y_pantalla = 0; y_pantalla < PANTALLA_FILAS; y_pantalla++) {
        /* Iteramos sobre la PANTALLA (ej. 0 a 20) */
        for (x_pantalla = 0; x_pantalla < PANTALLA_COLUMNAS; x_pantalla++) {
            
            /* Calculamos qué celda del MUNDO debemos dibujar */
            y_mundo = y_pantalla + camara.y;
            x_mundo = x_pantalla + camara.x;
            
            /* Elegimos el pincel (color) basado en el MUNDO */
            switch (mapa[y_mundo][x_mundo]) {
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

            /* Definimos las coordenadas del rectángulo en la PANTALLA */
            rectCelda.left = x_pantalla * TAMANO_CELDA;
            rectCelda.top = y_pantalla * TAMANO_CELDA;
            rectCelda.right = (x_pantalla + 1) * TAMANO_CELDA;
            rectCelda.bottom = (y_pantalla + 1) * TAMANO_CELDA;

            FillRect(hdc, &rectCelda, hBrushActual);
        }
    }
    
    DeleteObject(hBrushAgua);
    DeleteObject(hBrushTierra);
    DeleteObject(hBrushEnemigo);
    DeleteObject(hBrushRecurso);
}

void dibujarJugador(HDC hdc, Jugador jugador, Camera camara) {
    HBRUSH hBrushJugador = CreateSolidBrush(RGB(255, 255, 255)); /* Blanco */
    RECT rectJugador;
    int x_pantalla, y_pantalla;

    /* Calculamos la posición del jugador EN LA PANTALLA */
    x_pantalla = jugador.x - camara.x;
    y_pantalla = jugador.y - camara.y;

    rectJugador.left = x_pantalla * TAMANO_CELDA;
    rectJugador.top = y_pantalla * TAMANO_CELDA;
    rectJugador.right = (x_pantalla + 1) * TAMANO_CELDA;
    rectJugador.bottom = (y_pantalla + 1) * TAMANO_CELDA;

    FillRect(hdc, &rectJugador, hBrushJugador);

    DeleteObject(hBrushJugador);
}