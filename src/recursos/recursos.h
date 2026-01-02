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

// 2. JUGADOR Y ENTIDADES
extern HBITMAP hBmpJugador;

// --- ANIMACIONES DE UNIDADES ---
// [Dirección][Frame] -> 0:Abajo, 1:Arriba, 2:Izq, 3:Der
extern HBITMAP hBmpMineroAnim[4][3];   // Usa Pico
extern HBITMAP hBmpLenadorAnim[4][3];  // Usa Hacha
extern HBITMAP hBmpCazadorAnim[4][3];  // Usa Espada (Sin armadura)
extern HBITMAP hBmpSoldadoAnim[4][3];  // Usa Espada (Con armadura)
extern HBITMAP hBmpJugadorAnim[4][3];
extern HBITMAP hBmpTienda[2];
extern HBITMAP hBmpArmaduraAnim[4][3];
extern HBITMAP hBmpVaca[8]; 
extern HBITMAP hBmpVacaMuerta; // Nueva variable
extern HBITMAP hBmpArbolChico;
extern HBITMAP hBmpArbolGrande;

// (Opcional, pero útil si lo usas en otros lados)
extern char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];

// 3. INTERFAZ (UI)
extern HBITMAP hBmpInvCerrado;
extern HBITMAP hBmpInvAbierto;
extern HBITMAP hBmpCorazon100; extern HBITMAP hBmpCorazon75; extern HBITMAP hBmpCorazon50;
extern HBITMAP hBmpCorazon25; extern HBITMAP hBmpCorazon0;
extern HBITMAP hBmpEscudo100; extern HBITMAP hBmpEscudo75; extern HBITMAP hBmpEscudo50;
extern HBITMAP hBmpEscudo25; extern HBITMAP hBmpEscudo0;
extern HBITMAP hBmpBarraXPVacia; extern HBITMAP hBmpBarraXPLlena;

// 4. ITEMS
extern HBITMAP hBmpIconoMadera; extern HBITMAP hBmpIconoPiedra; extern HBITMAP hBmpIconoOro;
extern HBITMAP hBmpIconoHierro; extern HBITMAP hBmpIconoComida; extern HBITMAP hBmpIconoHoja;
extern HBITMAP hBmpIconoEspada; extern HBITMAP hBmpIconoPico;

extern HBITMAP hBmpIconoHacha;
extern HBITMAP hBmpIconoArmaduraInv;
extern HBITMAP hBmpIconoSoldadoInv;


// 5. MUNDO
extern HBITMAP hBmpIslaGrande;
extern HBITMAP hBmpIslaSec1;
extern HBITMAP hBmpIslaSec2;
extern HBITMAP hBmpIslaSec3;
extern HBITMAP hBmpIslaSec4;
extern HBITMAP hBmpCastilloJugador;
extern HBITMAP hBmpRoca;
extern HBITMAP hBmpTesoroCerrado;
extern HBITMAP hBmpTesoroOro;     
extern HBITMAP hBmpTesoroJoyas;   
extern HBITMAP hBmpTesoroVacio;
extern HBITMAP hBmpMina; 
extern HBITMAP hBmpEstablo;

// --- FUNCIONES ---
void CargarRecursos();
void LiberarRecursos();
void DibujarImagen(HDC hdcDestino, HBITMAP hBitmap, int x, int y, int ancho, int alto);
void reclutarTrabajador(Jugador *j, int tipo);

#endif