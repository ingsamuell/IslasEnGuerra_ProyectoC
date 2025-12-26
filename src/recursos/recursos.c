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
HBITMAP hBmpInvCerrado = NULL;
HBITMAP hBmpInvAbierto = NULL;

HBITMAP hBmpJugador = NULL;
HBITMAP hBmpJugadorAnim[4][3];

// Armadura (Inicializada en NULL)
HBITMAP hBmpArmaduraAnim[4][3] = {{NULL}};

// Tienda
HBITMAP hBmpTienda[2] = {NULL, NULL};

// Mundo
HBITMAP hBmpIslaCentral = NULL;
HBITMAP hBmpIslaNoreste = NULL;
HBITMAP hBmpCastilloJugador = NULL;
HBITMAP hBmpCastilloEnemigo1 = NULL;
HBITMAP hBmpCastilloEnemigo2 = NULL;
HBITMAP hBmpCastilloEnemigo3 = NULL;
HBITMAP hBmpArbol = NULL;
HBITMAP hBmpRoca = NULL;

// Items
HBITMAP hBmpIconoMadera = NULL;
HBITMAP hBmpIconoPiedra = NULL;
HBITMAP hBmpIconoOro = NULL;
HBITMAP hBmpIconoEspada = NULL;
HBITMAP hBmpIconoPico = NULL;
HBITMAP hBmpIconoHierro = NULL;
HBITMAP hBmpIconoComida = NULL;
HBITMAP hBmpIconoHoja = NULL;

// HUD
HBITMAP hBmpCorazon100 = NULL;
HBITMAP hBmpCorazon75 = NULL;
HBITMAP hBmpCorazon50 = NULL;
HBITMAP hBmpCorazon25 = NULL;
HBITMAP hBmpCorazon0 = NULL;
HBITMAP hBmpEscudo100 = NULL;
HBITMAP hBmpEscudo75 = NULL;
HBITMAP hBmpEscudo50 = NULL;
HBITMAP hBmpEscudo25 = NULL;
HBITMAP hBmpEscudo0 = NULL;
HBITMAP hBmpBarraXPVacia = NULL;
HBITMAP hBmpBarraXPLlena = NULL;

HBITMAP CargarImagen(const char *ruta)
{
    HBITMAP hBmp = (HBITMAP)LoadImage(NULL, ruta, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (hBmp == NULL)
        printf("ERROR: No se encontro %s\n", ruta);
    return hBmp;
}

void CargarRecursos()
{
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

    // Jugador (Base)
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

    // Estos dos archivos no estaban en tu lista de subida, verifica que existan:
    hBmpIconoEspada = CargarImagen("assets/items/item_espada.bmp");
    hBmpIconoPico = CargarImagen("assets/items/item_pico.bmp");

    // Mundo
    hBmpIslaCentral = CargarImagen("assets/mundo/ISLA_1.bmp");
    hBmpIslaNoreste = CargarImagen("assets/mundo/isla_principal.bmp");

    // Castillos
    hBmpCastilloJugador = CargarImagen("assets/mundo/Castillo_jugador.bmp");
    hBmpCastilloEnemigo1 = CargarImagen("assets/mundo/Castillo_enemigoUNO.bmp");
    hBmpCastilloEnemigo2 = CargarImagen("assets/mundo/Castillo_enemigoDOS.bmp");
    hBmpCastilloEnemigo3 = CargarImagen("assets/mundo/Castillo_enemigoTRES.bmp");

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

    // --- CARGAR TIENDA (Nombres corregidos según tu carpeta) ---
    hBmpTienda[0] = CargarImagen("assets/mundo/tienda-de-ventas-pixilart.bmp");
    hBmpTienda[1] = CargarImagen("assets/mundo/tienda-de-ventas-pixilart-movimiento.bmp");

    // --- CARGAR ARMADURA (Mapeo manual según tus archivos) ---
    // Dirección 0 (Frente / Abajo)
    hBmpArmaduraAnim[0][0] = CargarImagen("assets/jugador/armadura-base-pixilart.bmp");
    hBmpArmaduraAnim[0][1] = CargarImagen("assets/jugador/armadura-base-pie-izquierdo-pixilart(1).bmp");
    hBmpArmaduraAnim[0][2] = CargarImagen("assets/jugador/armadura-base-pierna-derecha-pixilart(1).bmp");

    // Dirección 1 (Espalda / Arriba)
    hBmpArmaduraAnim[1][0] = CargarImagen("assets/jugador/armadura-base-espalda-pixilart.bmp");
    hBmpArmaduraAnim[1][1] = CargarImagen("assets/jugador/armadura-base-espalda-pie-izquierdo-pixilart.bmp");
    hBmpArmaduraAnim[1][2] = CargarImagen("assets/jugador/armadura-base-espalda-pie-derecho-pixilart.bmp");

    // Dirección 2 (Izquierda)
    hBmpArmaduraAnim[2][0] = CargarImagen("assets/jugador/armadura-base-izquierda-pixilart.bmp");
    hBmpArmaduraAnim[2][1] = CargarImagen("assets/jugador/armadura-base-izquierda-pie-izquierdo-pixilart.bmp");
    hBmpArmaduraAnim[2][2] = CargarImagen("assets/jugador/armadura-base-izquierda-pie-derecho-pixilart(1).bmp");

    // Dirección 3 (Derecha)
    hBmpArmaduraAnim[3][0] = CargarImagen("assets/jugador/armadura-base-derecha-pixilart.bmp");
    hBmpArmaduraAnim[3][1] = CargarImagen("assets/jugador/armadura-base-derecho-pie-derecho-pixilart.bmp");
    hBmpArmaduraAnim[3][2] = CargarImagen("assets/jugador/armadura-base-derecho-pie-izquierdo-pixilart(1).bmp");
}

void LiberarRecursos()
{
    // 1. Liberar Botones
    if (hBmpBtnJugar)
        DeleteObject(hBmpBtnJugar);
    if (hBmpBtnPartidas)
        DeleteObject(hBmpBtnPartidas);
    if (hBmpBtnInstrucciones)
        DeleteObject(hBmpBtnInstrucciones);
    if (hBmpBtnSalir)
        DeleteObject(hBmpBtnSalir);

    // 2. Liberar UI
    if (hBmpFondoMenu)
        DeleteObject(hBmpFondoMenu);
    if (hBmpBoton)
        DeleteObject(hBmpBoton);
    if (hBmpBotonSel)
        DeleteObject(hBmpBotonSel);
    if (hBmpTitulo)
        DeleteObject(hBmpTitulo);
    if (hBmpInvCerrado)
        DeleteObject(hBmpInvCerrado);
    if (hBmpInvAbierto)
        DeleteObject(hBmpInvAbierto);

    // 3. Liberar Jugador y Armadura
    if (hBmpJugador)
        DeleteObject(hBmpJugador);
    for (int d = 0; d < 4; d++)
    {
        for (int f = 0; f < 3; f++)
        {
            if (hBmpJugadorAnim[d][f])
                DeleteObject(hBmpJugadorAnim[d][f]);
            if (hBmpArmaduraAnim[d][f])
                DeleteObject(hBmpArmaduraAnim[d][f]);
        }
    }

    // 4. Liberar Tienda
    if (hBmpTienda[0])
        DeleteObject(hBmpTienda[0]);
    if (hBmpTienda[1])
        DeleteObject(hBmpTienda[1]);

    // 5. Liberar Mundo
    if (hBmpIslaCentral)
        DeleteObject(hBmpIslaCentral);
    if (hBmpIslaNoreste)
        DeleteObject(hBmpIslaNoreste);
    if (hBmpCastilloJugador)
        DeleteObject(hBmpCastilloJugador);
    if (hBmpCastilloEnemigo1)
        DeleteObject(hBmpCastilloEnemigo1);
    if (hBmpCastilloEnemigo2)
        DeleteObject(hBmpCastilloEnemigo2);
    if (hBmpCastilloEnemigo3)
        DeleteObject(hBmpCastilloEnemigo3);

    // 6. Liberar Items e Iconos
    if (hBmpIconoMadera)
        DeleteObject(hBmpIconoMadera);
    if (hBmpIconoPiedra)
        DeleteObject(hBmpIconoPiedra);
    if (hBmpIconoOro)
        DeleteObject(hBmpIconoOro);
    if (hBmpIconoHierro)
        DeleteObject(hBmpIconoHierro);
    if (hBmpIconoComida)
        DeleteObject(hBmpIconoComida);
    if (hBmpIconoHoja)
        DeleteObject(hBmpIconoHoja);
    if (hBmpIconoEspada)
        DeleteObject(hBmpIconoEspada);
    if (hBmpIconoPico)
        DeleteObject(hBmpIconoPico);

    // 7. Liberar HUD
    if (hBmpCorazon100)
        DeleteObject(hBmpCorazon100);
    if (hBmpCorazon75)
        DeleteObject(hBmpCorazon75);
    if (hBmpCorazon50)
        DeleteObject(hBmpCorazon50);
    if (hBmpCorazon25)
        DeleteObject(hBmpCorazon25);
    if (hBmpCorazon0)
        DeleteObject(hBmpCorazon0);
    if (hBmpEscudo100)
        DeleteObject(hBmpEscudo100);
    if (hBmpEscudo75)
        DeleteObject(hBmpEscudo75);
    if (hBmpEscudo50)
        DeleteObject(hBmpEscudo50);
    if (hBmpEscudo25)
        DeleteObject(hBmpEscudo25);
    if (hBmpEscudo0)
        DeleteObject(hBmpEscudo0);
    if (hBmpBarraXPVacia)
        DeleteObject(hBmpBarraXPVacia);
    if (hBmpBarraXPLlena)
        DeleteObject(hBmpBarraXPLlena);
}

void DibujarImagen(HDC hdcDestino, HBITMAP hBitmap, int x, int y, int ancho, int alto)
{
    if (hBitmap == NULL)
        return;
    HDC hdcMemoria = CreateCompatibleDC(hdcDestino);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMemoria, hBitmap);
    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);
    SetStretchBltMode(hdcDestino, HALFTONE);
    TransparentBlt(hdcDestino, x, y, ancho, alto, hdcMemoria, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 0, 255));
    SelectObject(hdcMemoria, hOld);
    DeleteDC(hdcMemoria);
}