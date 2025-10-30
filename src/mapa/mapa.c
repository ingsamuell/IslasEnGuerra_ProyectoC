/* src/mapa/mapa.c (Versión GDI con Viewport Dinámico) */
#include "mapa.h"

/* --- Lógica del Juego --- */

void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    int i, j;

    /* 1. Llenar todo el mundo de agua (~) */
    for (i = 0; i < MUNDO_FILAS; i++) {
        for (j = 0; j < MUNDO_COLUMNAS; j++) {
            mapa[i][j] = '~';
        }
    }

    /* 2. ISLA 1 (Superior Izquierda - Spawn del Jugador) */
    /* Coordenadas: (fila 10-24, col 10-29) */
    for (i = 10; i < 25; i++) {
        for (j = 10; j < 30; j++) {
            mapa[i][j] = '.'; /* Tierra */
        }
    }
    /* ¡PUNTO DE SPAWN! */
    mapa[17][20] = 'P'; 

    /* 3. ISLA 2 (Superior Derecha) */
    /* Coordenadas: (fila 15-34, col 70-89) */
    for (i = 15; i < 35; i++) {
        for (j = 70; j < 90; j++) {
            mapa[i][j] = '.';
        }
    }

    /* 4. ISLA 3 (Inferior Izquierda) */
    /* Coordenadas: (fila 70-89, col 15-39) */
    for (i = 70; i < 90; i++) {
        for (j = 15; j < 40; j++) {
            mapa[i][j] = '.';
        }
    }
    
    /* 5. ISLA 4 (Inferior Derecha) */
    /* Coordenadas: (fila 65-84, col 65-84) */
    for (i = 65; i < 85; i++) {
        for (j = 65; j < 85; j++) {
            mapa[i][j] = '.';
        }
    }
}


void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy) {
    int nuevoX = jugador->x + dx;
    int nuevoY = jugador->y + dy;

    if (nuevoX < 0 || nuevoX >= MUNDO_COLUMNAS || nuevoY < 0 || nuevoY >= MUNDO_FILAS) {
        return;
    }
    if (mapa[nuevoY][nuevoX] == '~') {
        return;
    }
    
    jugador->x = nuevoX;
    jugador->y = nuevoY;
}

/* --- Lógica de la Cámara --- */
void actualizarCamara(Camera *camara, Jugador jugador, int pantallaFilas, int pantallaColumnas) {
    camara->x = jugador.x - (pantallaColumnas / 2);
    camara->y = jugador.y - (pantallaFilas / 2);

    if (camara->x < 0) camara->x = 0;       
    if (camara->y < 0) camara->y = 0;
    
    if (camara->x > MUNDO_COLUMNAS - pantallaColumnas) {
        camara->x = MUNDO_COLUMNAS - pantallaColumnas;
    }
    if (camara->y > MUNDO_FILAS - pantallaFilas) {
        camara->y = MUNDO_FILAS - pantallaFilas;
    }
}

/* --- Gráficos del Juego --- */

void dibujarMapa(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera camara, int pantallaFilas, int pantallaColumnas) {
    HBRUSH hBrushAgua = CreateSolidBrush(RGB(0, 0, 255));
    HBRUSH hBrushTierra = CreateSolidBrush(RGB(139, 69, 19));
    HBRUSH hBrushEnemigo = CreateSolidBrush(RGB(255, 0, 0));
    HBRUSH hBrushRecurso = CreateSolidBrush(RGB(255, 255, 0));
    HBRUSH hBrushActual;
    int y_pantalla, x_pantalla, y_mundo, x_mundo;
    RECT rectCelda;

    for (y_pantalla = 0; y_pantalla < pantallaFilas; y_pantalla++) {
        for (x_pantalla = 0; x_pantalla < pantallaColumnas; x_pantalla++) {
            
            y_mundo = y_pantalla + camara.y;
            x_mundo = x_pantalla + camara.x;
            
            if (y_mundo >= MUNDO_FILAS || x_mundo >= MUNDO_COLUMNAS) {
                hBrushActual = hBrushAgua;
            } else {
                switch (mapa[y_mundo][x_mundo]) {
                    case 'P': case '.': hBrushActual = hBrushTierra; break;
                    case 'E': hBrushActual = hBrushEnemigo; break;
                    case '$': hBrushActual = hBrushRecurso; break;
                    case '~': default: hBrushActual = hBrushAgua; break;
                }
            }

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
    HBRUSH hBrushJugador = CreateSolidBrush(RGB(255, 255, 255));
    RECT rectJugador;
    int x_pantalla, y_pantalla;

    x_pantalla = jugador.x - camara.x;
    y_pantalla = jugador.y - camara.y;

    rectJugador.left = x_pantalla * TAMANO_CELDA;
    rectJugador.top = y_pantalla * TAMANO_CELDA;
    rectJugador.right = (x_pantalla + 1) * TAMANO_CELDA;
    rectJugador.bottom = (y_pantalla + 1) * TAMANO_CELDA;

    FillRect(hdc, &rectJugador, hBrushJugador);

    DeleteObject(hBrushJugador);
}