#ifndef RECURSOS_H
#define RECURSOS_H

#include <windows.h>
#include "../global.h"

// --- 1. MENÚ ---
extern HBITMAP hBmpFondoMenu;
extern HBITMAP hBmpBtnJugar;
extern HBITMAP hBmpBtnPartidas;
extern HBITMAP hBmpBtnInstrucciones;
extern HBITMAP hBmpBtnSalir;
extern HBITMAP hBmpTitulo;
extern HBITMAP hBmpBoton;
extern HBITMAP hBmpBotonSel;

// --- 2. JUGADOR Y ANIMACIONES ---

// Imagen estática (si se usa)
extern HBITMAP hBmpJugador;

// MATRICES DE ANIMACIÓN DEL JUGADOR
// [Dirección][Frame] -> 0:Abajo, 1:Arriba, 2:Izq, 3:Der
// Frames: 0=Quieto, 1=Pie1, 2=Pie2

extern HBITMAP hBmpJugadorAnim[4][3];  // Base (Sin nada)
extern HBITMAP hBmpArmaduraAnim[4][3]; // Con Armadura (Prioridad Visual)

// Nuevas Herramientas (Agregadas para jugador.c)
extern HBITMAP hBmpEspadaAnim[4][3]; // Jugador con Espada
extern HBITMAP hBmpPicoAnim[4][3];   // Jugador con Pico
extern HBITMAP hBmpHachaAnim[4][3];  // Jugador con Hacha

// ANIMACIONES DE UNIDADES (NPCs)
extern HBITMAP hBmpMineroAnim[4][3];  // NPC Minero
extern HBITMAP hBmpLenadorAnim[4][3]; // NPC Leñador
extern HBITMAP hBmpCazadorAnim[4][3]; // NPC Cazador
extern HBITMAP hBmpSoldadoAnim[4][3]; // NPC Soldado

// OTRAS ENTIDADES
extern HBITMAP hBmpVaca[8];
extern HBITMAP hBmpVacaMuerta;
extern HBITMAP hBmpTienda[2];

// BARCOS Y PESCA
extern HBITMAP hBmpBote[2];  // 0: Izq, 1: Der
extern HBITMAP hBmpBarco[2]; // 0: Izq, 1: Der

// --- 3. INTERFAZ (UI) ---
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

// --- 4. ITEMS (ICONOS) ---
extern HBITMAP hBmpIconoMadera;
extern HBITMAP hBmpIconoPiedra;
extern HBITMAP hBmpIconoOro;
extern HBITMAP hBmpIconoHierro;
extern HBITMAP hBmpIconoComida;
extern HBITMAP hBmpIconoHoja;
extern HBITMAP hBmpIconoEspada;
extern HBITMAP hBmpIconoPico;
extern HBITMAP hBmpIconoHacha;
extern HBITMAP hBmpIconoArmaduraInv;
extern HBITMAP hBmpIconoSoldadoInv;
extern HBITMAP hBmpIconoPescado;
extern HBITMAP hBmpIconoCana;
extern HBITMAP hBmpIconoPuno; 
extern Tiburon misTiburones[MAX_TIBURONES];
extern HBITMAP hBmpTiburonAnim[2][4];

// --- 5. MUNDO (MAPAS Y OBJETOS) ---

// Mapa 1
extern HBITMAP hBmpIslaGrande;
extern HBITMAP hBmpIslaSec1;
extern HBITMAP hBmpIslaSec2;
extern HBITMAP hBmpIslaSec3;
extern HBITMAP hBmpIslaSec4;
extern HBITMAP hBmpArbolChico;
extern HBITMAP hBmpArbolGrande;

// Mapa 2
extern HBITMAP hBmpIslaGrandeMapa2;
extern HBITMAP hBmpIslaSec1Mapa2;
extern HBITMAP hBmpIslaSec2Mapa2;
extern HBITMAP hBmpIslaSec3Mapa2;
extern HBITMAP hBmpIslaSec4Mapa2;
extern HBITMAP hBmpArbolChicoMapa2;
extern HBITMAP hBmpArbolGrandeMapa2;

// Mapa 3
extern HBITMAP hBmpIslaGrandeMapa3;
extern HBITMAP hBmpIslaSec1Mapa3;
extern HBITMAP hBmpIslaSec2Mapa3;
extern HBITMAP hBmpIslaSec3Mapa3;
extern HBITMAP hBmpIslaSec4Mapa3;
extern HBITMAP hBmpArbolChicoMapa3;
extern HBITMAP hBmpArbolGrandeMapa3;

// Objetos Generales
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
extern HBITMAP hBmpMuelle;

// (Opcional: Matriz visual del mapa si la necesitas)
extern char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];

// Mouse Debug
extern bool dibujandoCuadro;
extern int mouseStartX, mouseStartY;
extern int mouseActualX, mouseActualY;

// --- 6. SELECCIÓN DE MAPA ---
extern HBITMAP hBmpFondoSeleccionMapa;
extern HBITMAP hBmpCuadroMapa1Normal;
extern HBITMAP hBmpCuadroMapa1Sel;
extern HBITMAP hBmpCuadroMapa2Normal;
extern HBITMAP hBmpCuadroMapa2Sel;
extern HBITMAP hBmpCuadroMapa3Normal;
extern HBITMAP hBmpCuadroMapa3Sel;

// --- 7. ARRAYS GLOBALES (Objetos del juego) ---
extern Arbol arboles[MAX_ARBOLES];
extern Mina minas[MAX_MINAS];
extern Vaca manada[MAX_VACAS];
extern Unidad unidades[MAX_UNIDADES];
extern Particula particulas[MAX_PARTICULAS];
extern TextoFlotante textos[MAX_TEXTOS]; // Usamos MAX_TEXTOS de global.h

// --- FUNCIONES ---
void CargarRecursos();
void LiberarRecursos();
void DibujarImagen(HDC hdcDestino, HBITMAP hBitmap, int x, int y, int ancho, int alto);
void reclutarTrabajador(Jugador *j, int tipo);

#endif