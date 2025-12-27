#ifndef RECURSOS_H
#define RECURSOS_H

#include <windows.h>
#include "../global.h"

// --- VARIABLES GLOBALES (HBITMAP) ---

// 1. MENÚ
extern HBITMAP hBmpFondoMenu;
extern HBITMAP hBmpBtnJugar;
extern HBITMAP hBmpBtnPartidas;
extern HBITMAP hBmpBtnInstrucciones;
extern HBITMAP hBmpBtnSalir;
extern HBITMAP hBmpTitulo;
extern HBITMAP hBmpBoton;
extern HBITMAP hBmpBotonSel;

// 2. JUGADOR Y TIENDA
extern HBITMAP hBmpJugador;
extern HBITMAP hBmpJugadorAnim[4][3];
extern HBITMAP hBmpTienda[2];
extern HBITMAP hBmpArmaduraAnim[4][3];

// 3. INTERFAZ (UI)
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

// 4. ITEMS E ICONOS
extern HBITMAP hBmpIconoMadera;
extern HBITMAP hBmpIconoPiedra;
extern HBITMAP hBmpIconoOro;
extern HBITMAP hBmpIconoHierro;
extern HBITMAP hBmpIconoComida;
extern HBITMAP hBmpIconoHoja;
extern HBITMAP hBmpIconoEspada;
extern HBITMAP hBmpIconoPico;

// 5. MUNDO (NUEVAS ISLAS)
extern HBITMAP hBmpIslaGrande; // Central (1000x1000)
extern HBITMAP hBmpIslaSec1;   // Oeste (320x250)
extern HBITMAP hBmpIslaSec2;   // Norte (700x700)
extern HBITMAP hBmpIslaSec3;   // Este (400x400)
extern HBITMAP hBmpIslaSec4;   // Sur (500x500)

extern HBITMAP hBmpCastilloJugador;
extern HBITMAP hBmpArbol;
extern HBITMAP hBmpRoca;

// --- FUNCIONES ---
void CargarRecursos();
void LiberarRecursos();
void DibujarImagen(HDC hdcDestino, HBITMAP hBitmap, int x, int y, int ancho, int alto);

#endif