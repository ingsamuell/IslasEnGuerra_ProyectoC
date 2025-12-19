#include "recursos.h"
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <math.h>

/* ========== DEFINICIÓN DE SPRITES ========== */

/* --- Sprites del menú principal --- */
Sprite sprite_fondo_menu = {NULL, 900, 620};        // Fondo del menú
Sprite sprite_boton_normal = {NULL, 300, 70};       // Botón normal
Sprite sprite_boton_seleccionado = {NULL, 300, 70}; // Botón seleccionado
Sprite sprite_titulo = {NULL, 400, 100};            // Título del juego

/* --- Sprites del jugador --- */
Sprite sprite_jugador = {NULL, 32, 32};             // Jugador principal

/* --- Sprites del mapa y terreno --- */
Sprite sprite_isla_principal = {NULL, 500, 500};    // Isla principal (500×500)
Sprite sprite_isla_secundaria = {NULL, 160, 160};   // Islas secundarias
Sprite sprite_montana = {NULL, 16, 16};             // Montañas
Sprite sprite_recurso = {NULL, 16, 16};             // Recursos
Sprite sprite_enemigo = {NULL, 16, 16};             // Enemigos
Sprite sprite_agua = {NULL, 16, 16};                // Agua/océano

/* --- Sprites para construcción futura --- */
Sprite sprite_edificio_cuartel = {NULL, 32, 32};    // Cuartel (futuro)
Sprite sprite_edificio_mina = {NULL, 32, 32};       // Mina (futuro)
Sprite sprite_unidad_soldado = {NULL, 16, 16};      // Soldado (futuro)
Sprite sprite_unidad_arquero = {NULL, 16, 16};      // Arquero (futuro)

/* ========== FUNCIONES AUXILIARES ========== */

/* Verifica si un archivo existe */
int archivo_existe(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

/* Carga un bitmap desde archivo con búsqueda en múltiples rutas */
HBITMAP cargar_bitmap(const char* filename) {
    printf("Intentando cargar: %s\n", filename);
    
    // Lista de rutas posibles a probar (relativas al ejecutable)
    const char* rutas_posibles[] = {
        filename,                       // Ruta absoluta o relativa directa
        "assets/%s",                    // assets/ en directorio actual
        "../assets/%s",                 // assets/ un nivel arriba  
        "../../assets/%s",              // assets/ dos niveles arriba
        "src/recursos/assets/%s",       // Desde estructura de código fuente
        "../src/recursos/assets/%s",    // Un nivel arriba + estructura
        "resources/%s",                 // Alternativa: resources/
        "../resources/%s"               // resources/ un nivel arriba
    };
    
    int num_rutas = sizeof(rutas_posibles) / sizeof(rutas_posibles[0]);
    char ruta_completa[512];
    HBITMAP bitmap = NULL;
    
    for (int i = 0; i < num_rutas; i++) {
        // Construir ruta completa
        if (strchr(rutas_posibles[i], '%')) {
            snprintf(ruta_completa, sizeof(ruta_completa), rutas_posibles[i], filename);
        } else {
            strncpy(ruta_completa, rutas_posibles[i], sizeof(ruta_completa));
        }
        
        printf("  Probando ruta: %s\n", ruta_completa);
        
        // Verificar si el archivo existe
        if (archivo_existe(ruta_completa)) {
            // Cargar el bitmap
            bitmap = (HBITMAP)LoadImageA(
                NULL,                   // Instancia (NULL = desde archivo)
                ruta_completa,          // Ruta del archivo
                IMAGE_BITMAP,           // Tipo: bitmap
                0, 0,                   // Ancho/alto (0 = tamaño original)
                LR_LOADFROMFILE         // Cargar desde archivo
            );
            
            if (bitmap) {
                printf("  EXITO: Cargado desde %s\n", ruta_completa);
                
                // Obtener dimensiones reales del bitmap
                BITMAP bm;
                GetObject(bitmap, sizeof(BITMAP), &bm);
                printf("  Dimensiones: %ldx%ld píxeles\n", bm.bmWidth, bm.bmHeight);  // FIX: %ld
                
                return bitmap;
            } else {
                DWORD error = GetLastError();
                printf("  ERROR LoadImage: %lu\n", error);
            }
        }
    }
    
    printf("  FALLO: No se encontró %s en ninguna ruta\n", filename);
    return NULL;
}

/* ========== FUNCIÓN PRINCIPAL DE CARGA ========== */

int cargar_sprites() {
    printf("\n=== INICIANDO CARGA DE SPRITES ===\n\n");
    
    // Mostrar directorio actual para debug
    char current_dir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, current_dir);
    printf("Directorio actual: %s\n\n", current_dir);
    
    int sprites_cargados = 0;
    int sprites_fallidos = 0;
    
    /* --- 1. CARGAR SPRITE DEL JUGADOR (ESENCIAL) --- */
    printf("1. Cargando sprite del jugador...\n");
    sprite_jugador.bitmap = cargar_bitmap("jugador_compatible.bmp");
    if (!sprite_jugador.bitmap) {
        printf("  ERROR CRÍTICO: No se pudo cargar sprite del jugador\n");
        printf("  El juego no puede continuar sin el sprite del jugador\n");
        return 0;  // Error crítico
    }
    sprites_cargados++;
    printf("  OK: Jugador cargado (32x32)\n\n");
    
    /* --- 2. CARGAR SPRITES DEL MENÚ (IMPORTANTES) --- */
    printf("2. Cargando sprites del menú...\n");
    
    sprite_fondo_menu.bitmap = cargar_bitmap("puta.bmp");
    if (sprite_fondo_menu.bitmap) sprites_cargados++;
    else sprites_fallidos++;
    
    sprite_boton_normal.bitmap = cargar_bitmap("boton_normal.bmp");
    if (sprite_boton_normal.bitmap) sprites_cargados++;
    else sprites_fallidos++;
    
    sprite_boton_seleccionado.bitmap = cargar_bitmap("boton_seleccionado.bmp");
    if (sprite_boton_seleccionado.bitmap) sprites_cargados++;
    else sprites_fallidos++;
    
    sprite_titulo.bitmap = cargar_bitmap("titulo_juego.bmp");
    if (sprite_titulo.bitmap) sprites_cargados++;
    else sprites_fallidos++;
    
    printf("  OK: Menús cargados (%d/%d sprites)\n\n", 
           sprites_cargados - 1, 4);  // -1 por el jugador
    
    /* --- 3. CARGAR SPRITES DEL MAPA (ESENCIAL: ISLA PRINCIPAL) --- */
    printf("3. Cargando sprites del mapa...\n");
    
    // ISLA PRINCIPAL (ESENCIAL PARA EL JUEGO)
    sprite_isla_principal.bitmap = cargar_bitmap("isla_principal.bmp");
    if (sprite_isla_principal.bitmap) {
        sprites_cargados++;
        printf("  OK: Isla principal cargada (500x500)\n");
        
        // Verificar que el tamaño sea correcto
        BITMAP bm;
        GetObject(sprite_isla_principal.bitmap, sizeof(BITMAP), &bm);
        if (bm.bmWidth != 500 || bm.bmHeight != 500) {
            printf("  ADVERTENCIA: Isla tiene tamaño %ldx%ld (se esperaba 500x500)\n",  // FIX: %ld
                   bm.bmWidth, bm.bmHeight);
            printf("  El sprite se escalará automáticamente\n");
        }
    } else {
        sprites_fallidos++;
        printf("  ERROR: No se pudo cargar isla_principal.bmp (500x500)\n");
        printf("  El juego usará dibujo por código para la isla\n");
    }
    
    // SPRITES OPCIONALES DEL MAPA
    sprite_isla_secundaria.bitmap = cargar_bitmap("isla_secundaria.bmp");
    if (sprite_isla_secundaria.bitmap) sprites_cargados++;
    else sprites_fallidos++;
    
    sprite_montana.bitmap = cargar_bitmap("montana.bmp");
    if (sprite_montana.bitmap) sprites_cargados++;
    else sprites_fallidos++;
    
    sprite_recurso.bitmap = cargar_bitmap("recurso.bmp");
    if (sprite_recurso.bitmap) sprites_cargados++;
    else sprites_fallidos++;
    
    sprite_enemigo.bitmap = cargar_bitmap("enemigo.bmp");
    if (sprite_enemigo.bitmap) sprites_cargados++;
    else sprites_fallidos++;
    
    sprite_agua.bitmap = cargar_bitmap("agua.bmp");
    if (sprite_agua.bitmap) sprites_cargados++;
    else sprites_fallidos++;
    
    printf("  OK: Sprites de mapa cargados (%d/%d sprites)\n\n", 
            (sprite_isla_principal.bitmap ? 1 : 0) + 
            (sprite_isla_secundaria.bitmap ? 1 : 0) +
            (sprite_montana.bitmap ? 1 : 0) +
            (sprite_recurso.bitmap ? 1 : 0) +
            (sprite_enemigo.bitmap ? 1 : 0) +
            (sprite_agua.bitmap ? 1 : 0), 6);
    
    /* --- 4. SPRITES FUTUROS (OPCIONALES) --- */
    printf("4. Cargando sprites futuros (opcionales)...\n");
    
    sprite_edificio_cuartel.bitmap = cargar_bitmap("edificio_cuartel.bmp");
    if (sprite_edificio_cuartel.bitmap) sprites_cargados++;
    
    sprite_edificio_mina.bitmap = cargar_bitmap("edificio_mina.bmp");
    if (sprite_edificio_mina.bitmap) sprites_cargados++;
    
    sprite_unidad_soldado.bitmap = cargar_bitmap("unidad_soldado.bmp");
    if (sprite_unidad_soldado.bitmap) sprites_cargados++;
    
    sprite_unidad_arquero.bitmap = cargar_bitmap("unidad_arquero.bmp");
    if (sprite_unidad_arquero.bitmap) sprites_cargados++;
    
    printf("  OK: Sprites futuros cargados\n\n");
    
    /* --- RESUMEN FINAL --- */
    printf("=== RESUMEN DE CARGA ===\n");
    printf("Sprites cargados exitosamente: %d\n", sprites_cargados);
    printf("Sprites no encontrados: %d\n", sprites_fallidos);
    printf("Total de sprites intentados: %d\n\n", sprites_cargados + sprites_fallidos);
    
    if (!sprite_isla_principal.bitmap) {
        printf("ADVERTENCIA: No se cargó la isla principal\n");
        printf("El juego funcionará con dibujo por código\n");
    }
    
    printf("=== CARGA DE SPRITES COMPLETADA ===\n\n");
    
    // El juego puede continuar incluso si faltan algunos sprites
    // Solo es crítico si falta el sprite del jugador
    return 1;
}

/* ========== FUNCIÓN DE LIMPIEZA ========== */

void liberar_sprites() {
    printf("Liberando recursos gráficos...\n");
    int sprites_liberados = 0;
    
    // Macro para facilitar la liberación
    #define LIBERAR_SPRITE(sprite) \
        if ((sprite).bitmap) { \
            DeleteObject((sprite).bitmap); \
            (sprite).bitmap = NULL; \
            sprites_liberados++; \
        }
    
    // Liberar todos los sprites
    LIBERAR_SPRITE(sprite_jugador);
    LIBERAR_SPRITE(sprite_fondo_menu);
    LIBERAR_SPRITE(sprite_boton_normal);
    LIBERAR_SPRITE(sprite_boton_seleccionado);
    LIBERAR_SPRITE(sprite_titulo);
    LIBERAR_SPRITE(sprite_isla_principal);
    LIBERAR_SPRITE(sprite_isla_secundaria);
    LIBERAR_SPRITE(sprite_montana);
    LIBERAR_SPRITE(sprite_recurso);
    LIBERAR_SPRITE(sprite_enemigo);
    LIBERAR_SPRITE(sprite_agua);
    LIBERAR_SPRITE(sprite_edificio_cuartel);
    LIBERAR_SPRITE(sprite_edificio_mina);
    LIBERAR_SPRITE(sprite_unidad_soldado);
    LIBERAR_SPRITE(sprite_unidad_arquero);

    #undef LIBERAR_SPRITE
    
    printf("Sprites liberados: %d\n", sprites_liberados);
}

/* ========== FUNCIONES DE DIBUJADO ========== */

/* Dibuja un sprite en la posición especificada con transparencia */
void dibujar_sprite(HDC hdc, Sprite sprite, int x, int y) {
    if (!sprite.bitmap) {
        printf("ADVERTENCIA: Intentando dibujar sprite no cargado\n");
        return;  // Sprite no cargado
    }
    
    // Crear contexto de dispositivo en memoria
    HDC hdcMem = CreateCompatibleDC(hdc);
    if (!hdcMem) {
        printf("ERROR: No se pudo crear DC compatible\n");
        return;
    }
    
    // Seleccionar el bitmap en el DC de memoria
    SelectObject(hdcMem, sprite.bitmap);
    
    // Dibujar con transparencia (color magenta = transparente)
    BOOL resultado = TransparentBlt(
        hdc,                    // Destino
        x, y,                   // Posición destino
        sprite.ancho,           // Ancho destino
        sprite.alto,            // Alto destino
        hdcMem,                 // Origen
        0, 0,                   // Posición origen
        sprite.ancho,           // Ancho origen
        sprite.alto,            // Alto origen
        RGB(255, 0, 255)        // Color transparente (magenta)
    );
    
    if (!resultado) {
        DWORD error = GetLastError();
        printf("ERROR TransparentBlt: %lu en (%d, %d) sprite %dx%d\n", 
               error, x, y, sprite.ancho, sprite.alto);
    }
    
    // Liberar recursos
    DeleteDC(hdcMem);
}

/* Dibuja un sprite escalado a un tamaño específico */
void dibujar_sprite_escalado(HDC hdc, Sprite sprite, int x, int y, int ancho, int alto) {
    if (!sprite.bitmap) {
        return;  // Sprite no cargado
    }
    
    // Crear contexto de dispositivo en memoria
    HDC hdcMem = CreateCompatibleDC(hdc);
    if (!hdcMem) {
        return;
    }
    
    // Seleccionar el bitmap
    SelectObject(hdcMem, sprite.bitmap);
    
    // Para escalado con transparencia
    BOOL resultado = TransparentBlt(
        hdc,                    // Destino
        x, y,                   // Posición destino
        ancho,                  // Ancho destino (escalado)
        alto,                   // Alto destino (escalado)
        hdcMem,                 // Origen
        0, 0,                   // Posición origen
        sprite.ancho,           // Ancho origen (original)
        sprite.alto,            // Alto origen (original)
        RGB(255, 0, 255)        // Color transparente
    );
    
    if (!resultado) {
        // Fallback: escalado simple sin transparencia
        StretchBlt(
            hdc, x, y, ancho, alto,
            hdcMem, 0, 0, sprite.ancho, sprite.alto,
            SRCCOPY
        );
    }
    
    // Liberar recursos
    DeleteDC(hdcMem);
}

/* ========== FUNCIONES ADICIONALES ÚTILES ========== */

/* Crea un sprite de respaldo para la isla (dibujo por código) */
void crear_isla_respaldo(HDC hdc) {
    printf("Creando isla de respaldo (dibujo por código)...\n");
    
    // Esta función podría crear un bitmap temporal si no hay sprite
    // Actualmente no implementada - solo para futuro desarrollo
}

/* Verifica si un sprite está cargado */
int sprite_cargado(Sprite sprite) {
    return (sprite.bitmap != NULL);
}

/* Obtiene información de un sprite */
void obtener_info_sprite(Sprite sprite, char* buffer, int buffer_size) {
    if (!sprite.bitmap) {
        snprintf(buffer, buffer_size, "Sprite no cargado");
        return;
    }
    
    BITMAP bm;
    GetObject(sprite.bitmap, sizeof(BITMAP), &bm);
    
    // FIX: Usar %ld para LONG en lugar de %d
    snprintf(buffer, buffer_size, 
            "Sprite: %ldx%ld píxeles (esperado: %dx%d) - %s",  // FIX: %ld
            bm.bmWidth, bm.bmHeight,
            sprite.ancho, sprite.alto,
            (bm.bmWidth == sprite.ancho && bm.bmHeight == sprite.alto) ? 
            "OK" : "TAMAÑO DIFERENTE");
}