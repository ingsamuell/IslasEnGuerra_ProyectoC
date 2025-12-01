#ifndef RECURSOS_H
#define RECURSOS_H

#include <windows.h>

// Estructura para sprites
typedef struct {
    HBITMAP bitmap;
    int ancho, alto;
} Sprite;

// Declaraciones de funciones
int cargar_sprites();
void liberar_sprites();
void dibujar_sprite(HDC hdc, Sprite sprite, int x, int y);
void dibujar_sprite_escalado(HDC hdc, Sprite sprite, int x, int y, int ancho, int alto);

// Sprites del juego
extern Sprite sprite_jugador;
extern Sprite sprite_fondo_menu;
extern Sprite sprite_boton_normal;
extern Sprite sprite_boton_seleccionado;
extern Sprite sprite_titulo;

#endif