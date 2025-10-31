#include "recursos.h"
#include <stdio.h>
#include <windows.h>

// Solo el sprite del jugador
Sprite sprite_jugador = {NULL, 32, 32};

// Función auxiliar para verificar si un archivo existe
int archivo_existe(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

int cargar_sprites() {
    printf("=== INICIANDO CARGA DE SPRITES ===\n");
    
    // Lista de rutas posibles a probar
    const char* rutas_posibles[] = {
        "assets/jugador_compatible.bmp",
        "../assets/jugador_compatible.bmp",
        "../../assets/jugador_compatible.bmp",
        "jugador_compatible.bmp",
        "src/recursos/assets/jugador_compatible.bmp",
        "../src/recursos/assets/jugador_compatible.bmp"
    };
    
    int num_rutas = sizeof(rutas_posibles) / sizeof(rutas_posibles[0]);
    const char* ruta_encontrada = NULL;
    
    // Probar todas las rutas
    for (int i = 0; i < num_rutas; i++) {
        printf("Probando ruta: %s\n", rutas_posibles[i]);
        
        if (archivo_existe(rutas_posibles[i])) {
            ruta_encontrada = rutas_posibles[i];
            printf("ARCHIVO ENCONTRADO en: %s\n", ruta_encontrada);
            break;
        } else {
            printf("No encontrado en: %s\n", rutas_posibles[i]);
        }
    }
    
    if (!ruta_encontrada) {
        printf("ERROR: No se encontró el archivo en ninguna ruta\n");
        
        // Mostrar directorio actual para debug
        char current_dir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, current_dir);
        printf("Directorio actual: %s\n", current_dir);
        
        return 0;
    }
    
    // Cargar el sprite desde la ruta encontrada
    printf("Cargando sprite desde: %s\n", ruta_encontrada);
    
    sprite_jugador.bitmap = (HBITMAP)LoadImageA(
        NULL, ruta_encontrada, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    if (!sprite_jugador.bitmap) {
        DWORD error = GetLastError();
        printf("ERROR: LoadImageA falló. Código: %lu\n", error);
        return 0;
    }
    
    // Obtener dimensiones reales del bitmap
    BITMAP bm;
    GetObject(sprite_jugador.bitmap, sizeof(BITMAP), &bm);
    sprite_jugador.ancho = bm.bmWidth;
    sprite_jugador.alto = bm.bmHeight;
    printf("EXITO: Sprite cargado - %dx%d pixels\n", sprite_jugador.ancho, sprite_jugador.alto);
    
    return 1;
}

void liberar_sprites() {
    if (sprite_jugador.bitmap) {
        DeleteObject(sprite_jugador.bitmap);
        sprite_jugador.bitmap = NULL;
        printf("Sprite liberado\n");
    }
}

void dibujar_sprite(HDC hdc, Sprite sprite, int x, int y) {
    if (!sprite.bitmap) {
        return;
    }
    
    HDC hdcMem = CreateCompatibleDC(hdc);
    SelectObject(hdcMem, sprite.bitmap);
    
    // Dibujar con las dimensiones reales del sprite
    BitBlt(hdc, x, y, sprite.ancho, sprite.alto, hdcMem, 0, 0, SRCCOPY);
    
    DeleteDC(hdcMem);
}