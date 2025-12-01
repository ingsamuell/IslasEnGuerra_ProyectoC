/* src/recursos/recursos.h */
#ifndef RECURSOS_H
#define RECURSOS_H

#include "../global.h"

/* Estructura para sprites */
typedef struct {
    HBITMAP bitmap;
    int ancho, alto;
} Sprite;

/* Variables Globales (extern para que otros archivos las vean) */
extern Sprite sprite_fondo_menu;
extern Sprite sprite_titulo;
extern Sprite sprite_boton_normal;
extern Sprite sprite_boton_seleccionado;
extern Sprite sprite_jugador;
extern Sprite sprite_tienda;

/* Funciones */

/* --- CORRECCIÓN AQUÍ: Añadir (HWND hwnd) --- */
int cargar_sprites(HWND hwnd); 

void liberar_sprites();
void dibujar_sprite(HDC hdc, Sprite sprite, int x, int y);

#endif