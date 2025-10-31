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

// Solo el sprite del jugador
extern Sprite sprite_jugador;

#endif