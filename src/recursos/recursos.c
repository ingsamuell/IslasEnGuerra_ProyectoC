#include "recursos.h"
#include <stdio.h>

// Solo el sprite del jugador
Sprite sprite_jugador = {NULL, 32, 32};

int cargar_sprites() {
    // Cargar solo el sprite del jugador desde assets/
    sprite_jugador.bitmap = (HBITMAP)LoadImageA(
        NULL, "assets/jugador_compatible.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    // Verificar que se cargó correctamente
    if (!sprite_jugador.bitmap) {
        printf("Error: No se pudo cargar sprite jugador\n");
        return 0;
    }
    
    return 1;
}

void liberar_sprites() {
    if (sprite_jugador.bitmap) DeleteObject(sprite_jugador.bitmap);
}

void dibujar_sprite(HDC hdc, Sprite sprite, int x, int y) {
    if (!sprite.bitmap) return;
    
    HDC hdcMem = CreateCompatibleDC(hdc);
    SelectObject(hdcMem, sprite.bitmap);
    
    BitBlt(hdc, x, y, sprite.ancho, sprite.alto, hdcMem, 0, 0, SRCCOPY);
    
    DeleteDC(hdcMem);
}