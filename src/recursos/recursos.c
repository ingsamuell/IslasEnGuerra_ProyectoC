/* src/recursos/recursos.c - VERSIÓN FINAL CON VACAS Y ÁRBOLES */
#include "recursos.h"
#include <stdio.h>
//cambios
// --- VARIABLES GLOBALES ---
HBITMAP hBmpFondoMenu = NULL;

// UI
HBITMAP hBmpBtnJugar = NULL; HBITMAP hBmpBtnPartidas = NULL;
HBITMAP hBmpBtnInstrucciones = NULL; HBITMAP hBmpBtnSalir = NULL;
HBITMAP hBmpTitulo = NULL; HBITMAP hBmpBoton = NULL; HBITMAP hBmpBotonSel = NULL;
HBITMAP hBmpInvCerrado = NULL; HBITMAP hBmpInvAbierto = NULL;


HBITMAP hBmpIslaGrande = NULL;
HBITMAP hBmpIslaSec1 = NULL;
HBITMAP hBmpIslaSec2 = NULL;
HBITMAP hBmpIslaSec3 = NULL;
HBITMAP hBmpIslaSec4 = NULL;

// Jugador
HBITMAP hBmpJugador = NULL;
HBITMAP hBmpJugadorAnim[4][3];
HBITMAP hBmpArmaduraAnim[4][3] = {{NULL}};

// Tienda
HBITMAP hBmpTienda[2] = {NULL, NULL};

// Mundo
HBITMAP hBmpIslaCentral = NULL; HBITMAP hBmpIslaNoreste = NULL;
HBITMAP hBmpCastilloJugador = NULL;
HBITMAP hBmpCastilloEnemigo1 = NULL; HBITMAP hBmpCastilloEnemigo2 = NULL; HBITMAP hBmpCastilloEnemigo3 = NULL;
HBITMAP hBmpRoca = NULL;
HBITMAP hBmpTesoroCerrado = NULL;
HBITMAP hBmpTesoroOro = NULL;
HBITMAP hBmpTesoroJoyas = NULL;
HBITMAP hBmpTesoroVacio = NULL;

// --- VARIABLES FALTANTES (AGREGADAS) ---
HBITMAP hBmpArbolChico = NULL;  // Antes hBmpArbol
HBITMAP hBmpArbolGrande = NULL; 
HBITMAP hBmpVaca[8] = {NULL};   // Array para las vacas

// Items
HBITMAP hBmpIconoMadera = NULL; HBITMAP hBmpIconoPiedra = NULL; HBITMAP hBmpIconoOro = NULL;
HBITMAP hBmpIconoHierro = NULL; HBITMAP hBmpIconoComida = NULL; HBITMAP hBmpIconoHoja = NULL;
HBITMAP hBmpIconoEspada = NULL; HBITMAP hBmpIconoPico = NULL;

// HUD
HBITMAP hBmpCorazon100 = NULL; HBITMAP hBmpCorazon75 = NULL; HBITMAP hBmpCorazon50 = NULL;
HBITMAP hBmpCorazon25 = NULL; HBITMAP hBmpCorazon0 = NULL;
HBITMAP hBmpEscudo100 = NULL; HBITMAP hBmpEscudo75 = NULL; HBITMAP hBmpEscudo50 = NULL;
HBITMAP hBmpEscudo25 = NULL; HBITMAP hBmpEscudo0 = NULL;
HBITMAP hBmpBarraXPVacia = NULL; HBITMAP hBmpBarraXPLlena = NULL;

HBITMAP CargarImagen(const char *ruta) {
    HBITMAP hBmp = (HBITMAP)LoadImage(NULL, ruta, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (hBmp == NULL) printf("ERROR: No se encontro %s\n", ruta);
    return hBmp;
}

void CargarRecursos() {
    hBmpFondoMenu = CargarImagen("assets/ui/fondo_menu.bmp");

    // UI
    hBmpBtnJugar = CargarImagen("assets/ui/btn_jugar.bmp");
    hBmpBtnPartidas = CargarImagen("assets/ui/btn_partidas.bmp");
    hBmpBtnInstrucciones = CargarImagen("assets/ui/btn_instrucciones.bmp");
    hBmpBtnSalir = CargarImagen("assets/ui/btn_salir.bmp");
    hBmpBoton = CargarImagen("assets/ui/boton.bmp");
    hBmpBotonSel = CargarImagen("assets/ui/boton_sel.bmp");
    hBmpTitulo = CargarImagen("assets/ui/titulo.bmp");
    hBmpInvCerrado = CargarImagen("assets/ui/boton_inv_cerrado.bmp");
    hBmpInvAbierto = CargarImagen("assets/ui/boton_inv_abierto.bmp");

    // --- CARGAR ÁRBOLES (Usando los nombres de tu lista) ---
    hBmpArbolChico = CargarImagen("assets/mundo/Arbol_islaUno.bmp");
    hBmpArbolGrande = CargarImagen("assets/mundo/Arbolgrande_islaUno.bmp");

    // --- CARGAR VACAS ---
    hBmpVaca[0] = CargarImagen("assets/mundo/vaca_0.bmp");
    hBmpVaca[1] = CargarImagen("assets/mundo/vaca-1.bmp");
    hBmpVaca[2] = CargarImagen("assets/mundo/vaca-2.bmp");
    hBmpVaca[3] = CargarImagen("assets/mundo/vaca-3.bmp");
    hBmpVaca[4] = CargarImagen("assets/mundo/vaca-4.bmp");
    hBmpVaca[5] = CargarImagen("assets/mundo/vaca-5.bmp");
    hBmpVaca[6] = CargarImagen("assets/mundo/vaca-6.bmp");
    hBmpVaca[7] = CargarImagen("assets/mundo/vaca-7.bmp");

    // Jugador
    hBmpJugadorAnim[0][0] = CargarImagen("assets/jugador/p_frente_base.bmp");
    hBmpJugadorAnim[0][1] = CargarImagen("assets/jugador/p_frente_pie1.bmp");
    hBmpJugadorAnim[0][2] = CargarImagen("assets/jugador/p_frente_pie2.bmp");
    hBmpJugadorAnim[1][0] = CargarImagen("assets/jugador/p_esp_base.bmp");
    hBmpJugadorAnim[1][1] = CargarImagen("assets/jugador/p_esp_pie1.bmp");
    hBmpJugadorAnim[1][2] = CargarImagen("assets/jugador/p_esp_pie2.bmp");
    hBmpJugadorAnim[2][0] = CargarImagen("assets/jugador/p_izq_base.bmp");
    hBmpJugadorAnim[2][1] = CargarImagen("assets/jugador/p_izq_pie1.bmp");
    hBmpJugadorAnim[2][2] = CargarImagen("assets/jugador/p_izq_pie2.bmp");
    hBmpJugadorAnim[3][0] = CargarImagen("assets/jugador/p_der_base.bmp");
    hBmpJugadorAnim[3][1] = CargarImagen("assets/jugador/p_der_pie1.bmp");
    hBmpJugadorAnim[3][2] = CargarImagen("assets/jugador/p_der_pie2.bmp");
    hBmpJugador = CargarImagen("assets/jugador/jugador_compatible.bmp");

    // Armadura
    hBmpArmaduraAnim[0][0] = CargarImagen("assets/jugador/armadura-base-pixilart.bmp");
    hBmpArmaduraAnim[0][1] = CargarImagen("assets/jugador/armadura-base-pie-izquierdo-pixilart(1).bmp");
    hBmpArmaduraAnim[0][2] = CargarImagen("assets/jugador/armadura-base-pierna-derecha-pixilart(1).bmp");
    hBmpArmaduraAnim[1][0] = CargarImagen("assets/jugador/armadura-base-espalda-pixilart.bmp");
    hBmpArmaduraAnim[1][1] = CargarImagen("assets/jugador/armadura-base-espalda-pie-izquierdo-pixilart.bmp");
    hBmpArmaduraAnim[1][2] = CargarImagen("assets/jugador/armadura-base-espalda-pie-derecho-pixilart.bmp");
    hBmpArmaduraAnim[2][0] = CargarImagen("assets/jugador/armadura-base-izquierda-pixilart.bmp"); // Verifica nombre
    hBmpArmaduraAnim[2][1] = CargarImagen("assets/jugador/armadura-base-izquierda-pie-izquierdo-pixilart.bmp");
    hBmpArmaduraAnim[2][2] = CargarImagen("assets/jugador/armadura-base-izquierda-pie-derecho-pixilart(1).bmp");
    hBmpArmaduraAnim[3][0] = CargarImagen("assets/jugador/armadura-base-derecha-pixilart.bmp");
    hBmpArmaduraAnim[3][1] = CargarImagen("assets/jugador/armadura-base-derecho-pie-derecho-pixilart.bmp");
    hBmpArmaduraAnim[3][2] = CargarImagen("assets/jugador/armadura-base-derecho-pie-izquierdo-pixilart(1).bmp");

    // Items
    hBmpIconoMadera = CargarImagen("assets/items/icono_madera.bmp");
    hBmpIconoPiedra = CargarImagen("assets/items/icono_piedra.bmp");
    hBmpIconoOro = CargarImagen("assets/items/icono_oro.bmp");
    hBmpIconoHierro = CargarImagen("assets/items/icono_hierro.bmp");
    hBmpIconoComida = CargarImagen("assets/items/icono_comida.bmp");
    hBmpIconoHoja = CargarImagen("assets/items/icono_hoja.bmp");
    hBmpIconoEspada = CargarImagen("assets/items/item_espada.bmp");
    hBmpIconoPico = CargarImagen("assets/items/item_pico.bmp");

    // Mundo
    hBmpIslaCentral = CargarImagen("assets/mundo/ISLA_1.bmp");
    hBmpIslaGrande = CargarImagen("assets/mundo/Isla_grande.bmp");
    hBmpIslaSec1 = CargarImagen("assets/mundo/islasecundaria_UNO.bmp");
    hBmpIslaSec2 = CargarImagen("assets/mundo/islasecundaria_DOS.bmp");
    hBmpIslaSec3 = CargarImagen("assets/mundo/islasecundaria_TRES.bmp");
    hBmpIslaSec4 = CargarImagen("assets/mundo/islasecundaria_CUATRO.bmp");
    hBmpCastilloJugador = CargarImagen("assets/mundo/Castillo_jugador.bmp");
    hBmpCastilloEnemigo1 = CargarImagen("assets/mundo/Castillo_enemigoUNO.bmp");
    hBmpCastilloEnemigo2 = CargarImagen("assets/mundo/Castillo_enemigoDOS.bmp");
    hBmpCastilloEnemigo3 = CargarImagen("assets/mundo/Castillo_enemigoTRES.bmp");
    hBmpTienda[0] = CargarImagen("assets/mundo/tienda-de-ventas-pixilart.bmp");
    hBmpTienda[1] = CargarImagen("assets/mundo/tienda-de-ventas-pixilart-movimiento.bmp");
    // --- TESOROS ---
    hBmpTesoroCerrado = CargarImagen("assets/mundo/tesoro-pixilart.bmp");
    hBmpTesoroOro     = CargarImagen("assets/mundo/tesoro-abierto-con-oro-pixilart.bmp");
    hBmpTesoroJoyas   = CargarImagen("assets/mundo/tesoro-abierto-con-piedras-preciosas-pixilart.bmp");
    hBmpTesoroVacio   = CargarImagen("assets/mundo/tesoro-abierto-vacio-pixilart.bmp");
    // HUD
    hBmpCorazon100 = CargarImagen("assets/ui/corazon_100.bmp");
    hBmpCorazon75 = CargarImagen("assets/ui/corazon_75.bmp");
    hBmpCorazon50 = CargarImagen("assets/ui/corazon_50.bmp");
    hBmpCorazon25 = CargarImagen("assets/ui/corazon_25.bmp");
    hBmpCorazon0 = CargarImagen("assets/ui/corazon_0.bmp");
    hBmpEscudo100 = CargarImagen("assets/ui/escudo_100.bmp");
    hBmpEscudo75 = CargarImagen("assets/ui/escudo_75.bmp");
    hBmpEscudo50 = CargarImagen("assets/ui/escudo_50.bmp");
    hBmpEscudo25 = CargarImagen("assets/ui/escudo_25.bmp");
    hBmpEscudo0 = CargarImagen("assets/ui/escudo_0.bmp");
    hBmpBarraXPVacia = CargarImagen("assets/ui/barra_xp_vacia.bmp");
    hBmpBarraXPLlena = CargarImagen("assets/ui/barra_xp_llena.bmp");
}

void LiberarRecursos() {
    if (hBmpBtnJugar) DeleteObject(hBmpBtnJugar);
    if (hBmpBtnPartidas) DeleteObject(hBmpBtnPartidas);
    if (hBmpBtnInstrucciones) DeleteObject(hBmpBtnInstrucciones);
    if (hBmpBtnSalir) DeleteObject(hBmpBtnSalir);
    if (hBmpFondoMenu) DeleteObject(hBmpFondoMenu);
    if (hBmpBoton) DeleteObject(hBmpBoton);
    if (hBmpBotonSel) DeleteObject(hBmpBotonSel);
    if (hBmpTitulo) DeleteObject(hBmpTitulo);
    if (hBmpInvCerrado) DeleteObject(hBmpInvCerrado);
    if (hBmpInvAbierto) DeleteObject(hBmpInvAbierto);

    if (hBmpArbolChico) DeleteObject(hBmpArbolChico);
    if (hBmpArbolGrande) DeleteObject(hBmpArbolGrande);
	if(hBmpTesoroCerrado) DeleteObject(hBmpTesoroCerrado);
    if(hBmpTesoroOro) DeleteObject(hBmpTesoroOro);
    if(hBmpTesoroJoyas) DeleteObject(hBmpTesoroJoyas);
    if(hBmpTesoroVacio) DeleteObject(hBmpTesoroVacio);;

    // Liberar Vacas
    for (int i = 0; i < 8; i++) {
        if(hBmpVaca[i]) DeleteObject(hBmpVaca[i]);
    }
    
    // ... resto de liberaciones (Jugador, Items, etc.) ...
}

void DibujarImagen(HDC hdcDestino, HBITMAP hBitmap, int x, int y, int ancho, int alto) {
    if (hBitmap == NULL) return;
    HDC hdcMemoria = CreateCompatibleDC(hdcDestino);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMemoria, hBitmap);
    BITMAP bm; GetObject(hBitmap, sizeof(BITMAP), &bm);
    SetStretchBltMode(hdcDestino, HALFTONE);
    TransparentBlt(hdcDestino, x, y, ancho, alto, hdcMemoria, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 0, 255));
    SelectObject(hdcMemoria, hOld);
    DeleteDC(hdcMemoria);
}