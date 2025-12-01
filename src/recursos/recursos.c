#include "recursos.h"
#include <stdio.h>
#include <windows.h>
#include <string.h>

// Sprites globales
Sprite sprite_jugador = {NULL, 32, 32};
Sprite sprite_fondo_menu = {NULL, 900, 620};  // Dimensiones exactas
Sprite sprite_boton_normal = {NULL, 300, 70};
Sprite sprite_boton_seleccionado = {NULL, 300, 70};
Sprite sprite_titulo = {NULL, 400, 100};

int archivo_existe(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

HBITMAP cargar_bitmap(const char* filename) {
    printf("Cargando: %s\n", filename);
    
    // Lista de rutas posibles a probar
    const char* rutas_posibles[] = {
        filename,
        "assets/%s",
        "../assets/%s", 
        "../../assets/%s",
        "src/recursos/assets/%s"
    };
    
    int num_rutas = sizeof(rutas_posibles) / sizeof(rutas_posibles[0]);
    char ruta_completa[256];
    
    for (int i = 0; i < num_rutas; i++) {
        if (strchr(rutas_posibles[i], '%')) {
            snprintf(ruta_completa, sizeof(ruta_completa), rutas_posibles[i], filename);
        } else {
            strncpy(ruta_completa, rutas_posibles[i], sizeof(ruta_completa));
        }
        
        printf("Probando ruta: %s\n", ruta_completa);
        
        if (archivo_existe(ruta_completa)) {
            HBITMAP bitmap = (HBITMAP)LoadImageA(
                NULL, ruta_completa, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            
            if (bitmap) {
                printf("EXITO: %s cargado\n", ruta_completa);
                
                // Obtener dimensiones reales
                BITMAP bm;
                GetObject(bitmap, sizeof(BITMAP), &bm);
                printf("Dimensiones: %ldx%ld\n", bm.bmWidth, bm.bmHeight);
                
                return bitmap;
            }
        }
    }
    
    printf("ERROR: No se pudo cargar %s\n", filename);
    return NULL;
}

int cargar_sprites() {
    printf("=== INICIANDO CARGA DE SPRITES ===\n");
    
    // Cargar sprite del jugador (ya existente)
    sprite_jugador.bitmap = cargar_bitmap("jugador_compatible.bmp");
    if (!sprite_jugador.bitmap) {
        printf("ERROR: No se pudo cargar sprite del jugador\n");
        return 0;
    }
    
    // Cargar nuevos sprites para el menú
    sprite_fondo_menu.bitmap = cargar_bitmap("puta.bmp");
    sprite_boton_normal.bitmap = cargar_bitmap("boton_normal.bmp");
    sprite_boton_seleccionado.bitmap = cargar_bitmap("boton_seleccionado.bmp");
    sprite_titulo.bitmap = cargar_bitmap("titulo_juego.bmp");
    
    // Si no se cargan los sprites del menú, no es crítico - usaremos dibujo por código
    if (!sprite_fondo_menu.bitmap) {
        printf("ADVERTENCIA: No se pudo cargar fondo del menú, usando dibujo por código\n");
    }
    if (!sprite_boton_normal.bitmap) {
        printf("ADVERTENCIA: No se pudo cargar botones, usando dibujo por código\n");
    }
    if (!sprite_titulo.bitmap) {
        printf("ADVERTENCIA: No se pudo cargar título, usando dibujo por código\n");
    }
    
    printf("=== CARGA DE SPRITES COMPLETADA ===\n");
    return 1;
}

void liberar_sprites() {
    if (sprite_jugador.bitmap) {
        DeleteObject(sprite_jugador.bitmap);
        sprite_jugador.bitmap = NULL;
    }
    if (sprite_fondo_menu.bitmap) {
        DeleteObject(sprite_fondo_menu.bitmap);
        sprite_fondo_menu.bitmap = NULL;
    }
    if (sprite_boton_normal.bitmap) {
        DeleteObject(sprite_boton_normal.bitmap);
        sprite_boton_normal.bitmap = NULL;
    }
    if (sprite_boton_seleccionado.bitmap) {
        DeleteObject(sprite_boton_seleccionado.bitmap);
        sprite_boton_seleccionado.bitmap = NULL;
    }
    if (sprite_titulo.bitmap) {
        DeleteObject(sprite_titulo.bitmap);
        sprite_titulo.bitmap = NULL;
    }
    printf("Sprites liberados\n");
}

void dibujar_sprite(HDC hdc, Sprite sprite, int x, int y) {
    if (!sprite.bitmap) {
        return;
    }
    
    HDC hdcMem = CreateCompatibleDC(hdc);
    SelectObject(hdcMem, sprite.bitmap);
    
    TransparentBlt(hdc, x, y, sprite.ancho, sprite.alto,
                  hdcMem, 0, 0, sprite.ancho, sprite.alto,
                  RGB(255, 0, 255));  // Magenta transparente
    
    DeleteDC(hdcMem);
}

void dibujar_sprite_escalado(HDC hdc, Sprite sprite, int x, int y, int ancho, int alto) {
    if (!sprite.bitmap) {
        return;
    }
    
    HDC hdcMem = CreateCompatibleDC(hdc);
    SelectObject(hdcMem, sprite.bitmap);
    
    StretchBlt(hdc, x, y, ancho, alto,
               hdcMem, 0, 0, sprite.ancho, sprite.alto,
               SRCCOPY);
    
    DeleteDC(hdcMem);
}