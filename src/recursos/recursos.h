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
// Agrega estas declaraciones extern
extern HBITMAP hBmpIconoPescado;
extern HBITMAP hBmpIconoCana;
extern HBITMAP hBmpBote[2];  // 0: Izq, 1: Der
extern HBITMAP hBmpBarco[2]; // 0: Izq, 1: Der
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
extern HBITMAP hBmpHierroPicar;
extern HBITMAP hBmpPiedraPicar;
// Variables para mapas adicionales
extern HBITMAP hBmpIslaGrandeMapa2;
extern HBITMAP hBmpIslaSec1Mapa2;
extern HBITMAP hBmpIslaSec2Mapa2;
extern HBITMAP hBmpIslaSec3Mapa2;
extern HBITMAP hBmpIslaSec4Mapa2;

extern HBITMAP hBmpIslaGrandeMapa3;
extern HBITMAP hBmpIslaSec1Mapa3;
extern HBITMAP hBmpIslaSec2Mapa3;
extern HBITMAP hBmpIslaSec3Mapa3;
extern HBITMAP hBmpIslaSec4Mapa3;
extern bool dibujandoCuadro;
extern int mouseStartX, mouseStartY;
extern int mouseActualX, mouseActualY;

// Nuevos árboles y establos para cada mapa
extern HBITMAP hBmpArbolChicoMapa2;
extern HBITMAP hBmpArbolGrandeMapa2;

extern HBITMAP hBmpArbolChicoMapa3;
extern HBITMAP hBmpArbolGrandeMapa3;

extern TextoFlotante textos[20];
// 6. SELECCIÓN DE MAPA (NUEVO)
extern HBITMAP hBmpFondoSeleccionMapa;
// Cuadros normales
extern HBITMAP hBmpCuadroMapa1Normal;
extern HBITMAP hBmpCuadroMapa2Normal;
extern HBITMAP hBmpCuadroMapa3Normal;
// Cuadros seleccionados
extern HBITMAP hBmpCuadroMapa1Sel;
extern HBITMAP hBmpCuadroMapa2Sel;
extern HBITMAP hBmpCuadroMapa3Sel;

extern Arbol arboles[MAX_ARBOLES];
extern Mina minas[MAX_MINAS];
extern Vaca manada[MAX_VACAS];
extern Unidad unidades[MAX_UNIDADES];
extern Particula particulas[MAX_PARTICULAS];

// --- FUNCIONES ---
void CargarRecursos();
void LiberarRecursos();
void DibujarImagen(HDC hdcDestino, HBITMAP hBitmap, int x, int y, int ancho, int alto);
void reclutarTrabajador(Jugador *j, int tipo);

#endif