#include "recursos.h"
#include <stdio.h>

// 1. DEFINICIÓN DE VARIABLES
HBITMAP hBmpFondoMenu = NULL;

// Nuevos Botones
HBITMAP hBmpBtnJugar = NULL;
HBITMAP hBmpBtnPartidas = NULL;
HBITMAP hBmpBtnInstrucciones = NULL;
HBITMAP hBmpBtnSalir = NULL;

// Compatibilidad
HBITMAP hBmpTitulo = NULL;
HBITMAP hBmpBoton = NULL;
HBITMAP hBmpBotonSel = NULL;

// Resto del juego
HBITMAP hBmpInvCerrado = NULL; HBITMAP hBmpInvAbierto = NULL;
HBITMAP hBmpJugador = NULL; HBITMAP hBmpJugadorAnim[4][3];
HBITMAP hBmpIslaCentral = NULL; HBITMAP hBmpIslaNoreste = NULL;
HBITMAP hBmpCastilloJugador = NULL; HBITMAP hBmpCastilloEnemigo1 = NULL;
HBITMAP hBmpCastilloEnemigo2 = NULL; HBITMAP hBmpCastilloEnemigo3 = NULL;
HBITMAP hBmpArbol = NULL; HBITMAP hBmpRoca = NULL;
HBITMAP hBmpIconoMadera = NULL; HBITMAP hBmpIconoPiedra = NULL; HBITMAP hBmpIconoOro = NULL; HBITMAP hBmpIconoEspada = NULL; HBITMAP hBmpIconoPico = NULL;
HBITMAP hBmpIconoHierro = NULL; HBITMAP hBmpIconoComida = NULL; HBITMAP hBmpIconoHoja = NULL;
HBITMAP hBmpCorazon100 = NULL; HBITMAP hBmpCorazon75 = NULL; HBITMAP hBmpCorazon50 = NULL; HBITMAP hBmpCorazon25 = NULL; HBITMAP hBmpCorazon0 = NULL;
HBITMAP hBmpEscudo100 = NULL; HBITMAP hBmpEscudo75 = NULL; HBITMAP hBmpEscudo50 = NULL; HBITMAP hBmpEscudo25 = NULL; HBITMAP hBmpEscudo0 = NULL;
HBITMAP hBmpBarraXPVacia = NULL; HBITMAP hBmpBarraXPLlena = NULL;

HBITMAP hBmpArmaduraAnim[4][3];
HBITMAP hBmpTienda[2];

HBITMAP CargarImagen(const char *ruta) {
    HBITMAP hBmp = (HBITMAP)LoadImage(NULL, ruta, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (hBmp == NULL) printf("ERROR: No se encontro %s\n", ruta);
    return hBmp;
}

void CargarRecursos() {
    hBmpFondoMenu = CargarImagen("assets/ui/fondo_menu.bmp");

    // --- CARGAMOS LOS BOTONES NUEVOS ---
    hBmpBtnJugar = CargarImagen("assets/ui/btn_jugar.bmp");
    hBmpBtnPartidas = CargarImagen("assets/ui/btn_partidas.bmp");
    hBmpBtnInstrucciones = CargarImagen("assets/ui/btn_instrucciones.bmp");
    hBmpBtnSalir = CargarImagen("assets/ui/btn_salir.bmp");

    // Compatibilidad
    hBmpBoton = CargarImagen("assets/ui/boton.bmp");
    hBmpBotonSel = CargarImagen("assets/ui/boton_sel.bmp");
    hBmpTitulo = CargarImagen("assets/ui/titulo.bmp");
    hBmpInvCerrado = CargarImagen("assets/ui/boton_inv_cerrado.bmp");
    hBmpInvAbierto = CargarImagen("assets/ui/boton_inv_abierto.bmp");

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

    // Items
    hBmpIconoMadera = CargarImagen("assets/items/icono_madera.bmp");
    hBmpIconoPiedra = CargarImagen("assets/items/icono_piedra.bmp");
    hBmpIconoOro = CargarImagen("assets/items/icono_oro.bmp");
    hBmpIconoHierro = CargarImagen("assets/items/icono_hierro.bmp");
    hBmpIconoComida = CargarImagen("assets/items/icono_comida.bmp");
    hBmpIconoHoja = CargarImagen("assets/items/icono_hoja.bmp");

    // Mundo y Stats (Resumido para brevedad, mantén lo tuyo si es igual)
    hBmpIslaCentral = CargarImagen("assets/mundo/ISLA_1.bmp");
    hBmpIslaNoreste = CargarImagen("assets/mundo/isla_principal.bmp");
    hBmpCorazon100 = CargarImagen("assets/ui/corazon_100.bmp");
    // ... (el resto de tus cargas habituales van aquí) ...

    // Cargar Tienda Animada (Gato)
    hBmpTienda[0] = CargarImagen("assets/mundo/tienda_gato1.bmp");
    hBmpTienda[1] = CargarImagen("assets/mundo/tienda_gato2.bmp");
    // Cargar Armadura (12 frames)
 char ruta[100];
    for (int d = 0; d < 4; d++) {
        for (int f = 0; f < 3; f++) {
            sprintf(ruta, "assets/jugador/arm_%d_%d.bmp", d, f);
            hBmpArmaduraAnim[d][f] = CargarImagen(ruta);
        }
    }
    hBmpIconoEspada = CargarImagen("assets/items/item_espada.bmp");
    hBmpIconoPico = CargarImagen("assets/items/item_pico.bmp");
}

void LiberarRecursos() {
    // Liberar lo nuevo
    if (hBmpBtnJugar) DeleteObject(hBmpBtnJugar);
    if (hBmpBtnPartidas) DeleteObject(hBmpBtnPartidas);
    if (hBmpBtnInstrucciones) DeleteObject(hBmpBtnInstrucciones);
    if (hBmpBtnSalir) DeleteObject(hBmpBtnSalir);
    // Liberar el resto
    if (hBmpFondoMenu) DeleteObject(hBmpFondoMenu);
    if (hBmpBoton) DeleteObject(hBmpBoton);
    // ... (agrega aquí el resto de DeleteObject que ya tenías) ...
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