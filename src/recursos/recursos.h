#ifndef RECURSOS_H
#define RECURSOS_H

#include <windows.h>
// Necesitamos las definiciones globales (DIR_ABAJO, etc.)
#include "../global.h"

// ==========================================
//   VARIABLES GLOBALES (HBITMAP)
// ==========================================

// --- MENÚ Y JUEGO (Lo que faltaba) ---
extern HBITMAP hBmpFondoMenu;
extern HBITMAP hBmpJugadorAnim[4][3]; // 4 direcciones, 3 frames cada una
extern HBITMAP hBmpIsla;       // Isla principal
extern HBITMAP hBmpTitulo;     // Título del juego (si lo tienes)
extern HBITMAP hBmpBoton;      // Botón normal
extern HBITMAP hBmpBotonSel;   // Botón seleccionado

// --- INTERFAZ (Mochila) ---
extern HBITMAP hBmpInvCerrado;
extern HBITMAP hBmpInvAbierto;

// --- MATERIALES ---
extern HBITMAP hBmpIconoMadera;
extern HBITMAP hBmpIconoPiedra;
extern HBITMAP hBmpIconoOro;
extern HBITMAP hBmpIconoHierro;
extern HBITMAP hBmpIconoComida;
extern HBITMAP hBmpIconoHoja;

// --- VIDA ---
extern HBITMAP hBmpCorazon100;
extern HBITMAP hBmpCorazon75;
extern HBITMAP hBmpCorazon50;
extern HBITMAP hBmpCorazon25;
extern HBITMAP hBmpCorazon0;

// --- ARMADURA ---
extern HBITMAP hBmpEscudo100;
extern HBITMAP hBmpEscudo75;
extern HBITMAP hBmpEscudo50;
extern HBITMAP hBmpEscudo25;
extern HBITMAP hBmpEscudo0;

// --- EXPERIENCIA ---
extern HBITMAP hBmpBarraXPVacia;
extern HBITMAP hBmpBarraXPLlena;

// ==========================================
//   FUNCIONES DE GESTIÓN
// ==========================================
void CargarRecursos();
void LiberarRecursos();

// Función auxiliar para dibujar fácil en mapa.c
void DibujarImagen(HDC hdcDestino, HBITMAP hBitmap, int x, int y, int ancho, int alto);

#endif