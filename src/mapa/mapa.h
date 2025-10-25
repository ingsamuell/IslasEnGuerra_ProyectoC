#ifndef MAPA_H
#define MAPA_H

#include "raylib.h"

#define FILAS 20
#define COLUMNAS 20

// Estados de pantalla del juego
typedef enum {
    MENU_PRINCIPAL,
    ISLA,
    OPCIONES,
    SALIR
} GameScreen;

// Estructura para botones del menú
typedef struct {
    Rectangle bounds;
    const char *text;
    bool hovered;
} Button;

// Funciones del mapa
void inicializarMapa(char mapa[FILAS][COLUMNAS]);
void dibujarMapa(char mapa[FILAS][COLUMNAS]);

// Funciones del menú
GameScreen MostrarMenu(void);
Button CrearBoton(float x, float y, float width, float height, const char *text);
void DibujarBoton(Button boton);
void DibujarMenuPrincipal(void);

#endif

