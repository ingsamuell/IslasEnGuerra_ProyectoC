/* src/recursos/recursos.c */
#include "recursos.h"
#include <stdio.h>

/* Definición de Variables Globales */
Sprite sprite_jugador;
Sprite sprite_fondo_menu;
Sprite sprite_titulo;
Sprite sprite_boton_normal;
Sprite sprite_boton_seleccionado;
Sprite sprite_tienda;

/* --- FUNCIÓN AUXILIAR (Debe ir PRIMERO) --- */
Sprite cargar_imagen_bmp(const char* ruta) {
    Sprite s = {0};
    /* Cargar la imagen desde el archivo */
    s.bitmap = (HBITMAP)LoadImage(NULL, ruta, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    if (s.bitmap) {
        /* Si cargó bien, obtener sus dimensiones */
        BITMAP bm;
        GetObject(s.bitmap, sizeof(BITMAP), &bm);
        s.ancho = bm.bmWidth;
        s.alto = bm.bmHeight;
    } else {
        printf("Error: No se pudo cargar la imagen '%s'\n", ruta);
    }
    return s;
}

/* --- FUNCIÓN PRINCIPAL DE CARGA (Usa la auxiliar) --- */
int cargar_sprites(HWND hwnd) {
    /* 1. Cargar Fondo del Menú */
    /* Intenta ruta completa (para ejecutar desde fuera) */
    sprite_fondo_menu = cargar_imagen_bmp("src/recursos/assets/puta.bmp");
    
    /* Si falla, intenta ruta corta (para ejecutar desde src o si moviste la carpeta) */
    if (!sprite_fondo_menu.bitmap) {
        sprite_fondo_menu = cargar_imagen_bmp("assets/puta.bmp");
    }
    
    /* Si sigue fallando, intenta en la raíz (último recurso) */
    if (!sprite_fondo_menu.bitmap) {
        sprite_fondo_menu = cargar_imagen_bmp("puta.bmp");
    }

    /* 2. Cargar Jugador */
    sprite_jugador = cargar_imagen_bmp("src/recursos/assets/jugador_compatible.bmp");
    if (!sprite_jugador.bitmap) {
        sprite_jugador = cargar_imagen_bmp("assets/jugador_compatible.bmp");
    }

    /* 3. Cargar Tienda */
    sprite_tienda = cargar_imagen_bmp("src/recursos/assets/tienda.bmp");
    if (!sprite_tienda.bitmap) {
        sprite_tienda = cargar_imagen_bmp("assets/tienda.bmp");
    }

    /* Retorna 1 (verdadero) si al menos el fondo cargó, para que el juego continúe */
    return (sprite_fondo_menu.bitmap != NULL);
}

void liberar_sprites() {
    if (sprite_jugador.bitmap) DeleteObject(sprite_jugador.bitmap);
    if (sprite_fondo_menu.bitmap) DeleteObject(sprite_fondo_menu.bitmap);
    if (sprite_titulo.bitmap) DeleteObject(sprite_titulo.bitmap);
    if (sprite_boton_normal.bitmap) DeleteObject(sprite_boton_normal.bitmap);
    if (sprite_boton_seleccionado.bitmap) DeleteObject(sprite_boton_seleccionado.bitmap);
    if (sprite_tienda.bitmap) DeleteObject(sprite_tienda.bitmap);
}

void dibujar_sprite(HDC hdc, Sprite sprite, int x, int y) {
    if (!sprite.bitmap) return;

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, sprite.bitmap);

    /* Dibujar con transparencia (Color clave: Magenta RGB 255,0,255) */
    TransparentBlt(hdc, x, y, sprite.ancho, sprite.alto,
                   hdcMem, 0, 0, sprite.ancho, sprite.alto,
                   RGB(255, 0, 255));

    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);
}