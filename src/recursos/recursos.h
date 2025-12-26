#ifndef RECURSOS_H
#define RECURSOS_H

#include <windows.h>
#include "../global.h"

// --- VARIABLES GLOBALES (HBITMAP) ---

// 1. MENÚ: Imágenes Específicas (NUEVO)
extern HBITMAP hBmpFondoMenu;
extern HBITMAP hBmpBtnJugar;
extern HBITMAP hBmpBtnPartidas;
extern HBITMAP hBmpBtnInstrucciones;
extern HBITMAP hBmpBtnSalir;

// 2. MENÚ: Compatibilidad (MANTENER)
extern HBITMAP hBmpTitulo;
extern HBITMAP hBmpBoton;    // Botón genérico
extern HBITMAP hBmpBotonSel; // Botón seleccionado genérico

// 3. JUGADOR
extern HBITMAP hBmpJugador;
extern HBITMAP hBmpJugadorAnim[4][3];
extern HBITMAP hBmpTienda[2];
extern HBITMAP hBmpArmaduraAnim[4][3];

// 4. INTERFAZ (Mochila, Vida, etc.)
extern HBITMAP hBmpInvCerrado;
extern HBITMAP hBmpInvAbierto;
extern HBITMAP hBmpCorazon100;
extern HBITMAP hBmpCorazon75;
extern HBITMAP hBmpCorazon50;
extern HBITMAP hBmpCorazon25;
extern HBITMAP hBmpCorazon0;
extern HBITMAP hBmpEscudo100;
extern HBITMAP hBmpEscudo75;
extern HBITMAP hBmpEscudo50;
extern HBITMAP hBmpEscudo25;
extern HBITMAP hBmpEscudo0;
extern HBITMAP hBmpBarraXPVacia;
extern HBITMAP hBmpBarraXPLlena;

// 5. ITEMS
extern HBITMAP hBmpIconoMadera;
extern HBITMAP hBmpIconoPiedra;
extern HBITMAP hBmpIconoOro;
extern HBITMAP hBmpIconoHierro;
extern HBITMAP hBmpIconoComida;
extern HBITMAP hBmpIconoHoja;

extern HBITMAP hBmpIconoEspada;
extern HBITMAP hBmpIconoPico;

// 6. MUNDO
extern HBITMAP hBmpIslaCentral;
extern HBITMAP hBmpIslaNoreste;
extern HBITMAP hBmpCastilloJugador;
extern HBITMAP hBmpCastilloEnemigo1;
extern HBITMAP hBmpCastilloEnemigo2;
extern HBITMAP hBmpCastilloEnemigo3;
extern HBITMAP hBmpArbol;
extern HBITMAP hBmpRoca;

// --- FUNCIONES ---
void CargarRecursos();
void LiberarRecursos();
void DibujarImagen(HDC hdcDestino, HBITMAP hBitmap, int x, int y, int ancho, int alto);

#endif