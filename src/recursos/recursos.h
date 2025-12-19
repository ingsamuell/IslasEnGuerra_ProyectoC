#ifndef RECURSOS_H
#define RECURSOS_H

#include <windows.h>

/* ========== ESTRUCTURA PARA SPRITES ========== */
typedef struct {
    HBITMAP bitmap;     // Handle al bitmap de Windows
    int ancho;          // Ancho en píxeles
    int alto;           // Alto en píxeles
} Sprite;

/* ========== DECLARACIONES DE FUNCIONES ========== */

/* --- Gestión de sprites --- */
int cargar_sprites();                           // Carga todos los sprites del juego
void liberar_sprites();                         // Libera memoria de todos los sprites

/* --- Funciones de dibujado --- */
void dibujar_sprite(HDC hdc, Sprite sprite, int x, int y);                     // Dibuja sprite en posición
void dibujar_sprite_escalado(HDC hdc, Sprite sprite, int x, int y,             // Dibuja sprite escalado
                            int ancho, int alto);

/* ========== SPRITES DEL JUEGO (variables externas) ========== */

/* --- Sprites del menú principal --- */
extern Sprite sprite_fondo_menu;               // Fondo del menú (900×620)
extern Sprite sprite_boton_normal;             // Botón en estado normal (300×70)
extern Sprite sprite_boton_seleccionado;       // Botón seleccionado (300×70)
extern Sprite sprite_titulo;                   // Título del juego (400×100)

/* --- Sprites del jugador y personajes --- */
extern Sprite sprite_jugador;                  // Sprite del jugador principal (32×32)

/* --- Sprites del mapa y terreno --- */
extern Sprite sprite_isla_principal;           // Isla principal del juego (500×500)
extern Sprite sprite_isla_secundaria;          // Islas secundarias (160×160)
extern Sprite sprite_montana;                  // Montañas/colinas (16×16)
extern Sprite sprite_recurso;                  // Recursos coleccionables (16×16)
extern Sprite sprite_enemigo;                  // Áreas enemigas (16×16)
extern Sprite sprite_agua;                     // Patrón de agua/océano (16×16)

/* --- Sprites para construcción futura --- */
extern Sprite sprite_edificio_cuartel;         // Edificio: Cuartel (32×32) - FUTURO
extern Sprite sprite_edificio_mina;            // Edificio: Mina (32×32) - FUTURO
extern Sprite sprite_unidad_soldado;           // Unidad: Soldado (16×16) - FUTURO
extern Sprite sprite_unidad_arquero;           // Unidad: Arquero (16×16) - FUTURO

#endif  /* RECURSOS_H */