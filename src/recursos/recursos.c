#include "recursos.h"
#include <stdio.h>

// Definición de variables
HBITMAP hBmpFondoMenu = NULL;
HBITMAP hBmpJugador = NULL;
HBITMAP hBmpIsla = NULL;
HBITMAP hBmpTitulo = NULL;
HBITMAP hBmpBoton = NULL;
HBITMAP hBmpBotonSel = NULL;
HBITMAP hBmpJugadorAnim[4][3];
HBITMAP hBmpInvCerrado = NULL;
HBITMAP hBmpInvAbierto = NULL;

HBITMAP hBmpIconoMadera = NULL;
HBITMAP hBmpIconoPiedra = NULL;
HBITMAP hBmpIconoOro = NULL;
HBITMAP hBmpIconoHierro = NULL;
HBITMAP hBmpIconoComida = NULL;
HBITMAP hBmpIconoHoja = NULL;

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

// Helper privado
HBITMAP CargarImagen(const char* ruta) {
    HBITMAP hBmp = (HBITMAP)LoadImage(NULL, ruta, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (hBmp == NULL) {
        printf("ERROR CRITICO: No se encontro %s\n", ruta);
    }
    return hBmp;
}

void CargarRecursos() {
    // --- CARGAMOS LO VIEJO (Adaptado al nuevo sistema) ---
    hBmpFondoMenu = CargarImagen("assets/imagen_menu.bmp"); // <--- CONFIRMA ESTE NOMBRE
    
    // AGREGAR ESTO PARA PROBAR:
    if (hBmpFondoMenu == NULL) {
        MessageBox(NULL, "¡No encuentro assets/imagen_menu.bmp!", "Error de Carga", MB_OK | MB_ICONERROR);
    }
    hBmpJugador   = CargarImagen("assets/jugador_compatible.bmp");
    // FRENTE (Abajo)
    hBmpJugadorAnim[DIR_ABAJO][0] = CargarImagen("assets/p_frente_base.bmp");
    hBmpJugadorAnim[DIR_ABAJO][1] = CargarImagen("assets/p_frente_pie1.bmp");
    hBmpJugadorAnim[DIR_ABAJO][2] = CargarImagen("assets/p_frente_pie2.bmp");

    // ESPALDA (Arriba)
    hBmpJugadorAnim[DIR_ARRIBA][0] = CargarImagen("assets/p_esp_base.bmp");
    hBmpJugadorAnim[DIR_ARRIBA][1] = CargarImagen("assets/p_esp_pie1.bmp");
    hBmpJugadorAnim[DIR_ARRIBA][2] = CargarImagen("assets/p_esp_pie2.bmp");

    // IZQUIERDA
    hBmpJugadorAnim[DIR_IZQUIERDA][0] = CargarImagen("assets/p_izq_base.bmp");
    hBmpJugadorAnim[DIR_IZQUIERDA][1] = CargarImagen("assets/p_izq_pie1.bmp");
    hBmpJugadorAnim[DIR_IZQUIERDA][2] = CargarImagen("assets/p_izq_pie2.bmp");

    // DERECHA
    hBmpJugadorAnim[DIR_DERECHA][0] = CargarImagen("assets/p_der_base.bmp");
    hBmpJugadorAnim[DIR_DERECHA][1] = CargarImagen("assets/p_der_pie1.bmp");
    hBmpJugadorAnim[DIR_DERECHA][2] = CargarImagen("assets/p_der_pie2.bmp");

    hBmpIsla      = CargarImagen("assets/isla_principal.bmp");
    // Si no tienes botón/título, el juego dibujará negro, no crasheará
    hBmpTitulo    = CargarImagen("assets/titulo.bmp"); 
    hBmpBoton     = CargarImagen("assets/boton.bmp");
    hBmpBotonSel  = CargarImagen("assets/boton_sel.bmp");

    // --- CARGAMOS LO NUEVO ---
    hBmpInvCerrado = CargarImagen("assets/boton_inv_cerrado.bmp");
    hBmpInvAbierto = CargarImagen("assets/boton_inv_abierto.bmp");

    hBmpIconoMadera = CargarImagen("assets/icono_madera.bmp");
    hBmpIconoPiedra = CargarImagen("assets/icono_piedra.bmp");
    hBmpIconoOro    = CargarImagen("assets/icono_oro.bmp");
    hBmpIconoHierro = CargarImagen("assets/icono_hierro.bmp");
    hBmpIconoComida = CargarImagen("assets/icono_comida.bmp");
    hBmpIconoHoja   = CargarImagen("assets/icono_hoja.bmp");

    hBmpCorazon100 = CargarImagen("assets/corazon_100.bmp");
    hBmpCorazon75  = CargarImagen("assets/corazon_75.bmp");
    hBmpCorazon50  = CargarImagen("assets/corazon_50.bmp");
    hBmpCorazon25  = CargarImagen("assets/corazon_25.bmp");
    hBmpCorazon0   = CargarImagen("assets/corazon_0.bmp");

    hBmpEscudo100 = CargarImagen("assets/escudo_100.bmp");
    hBmpEscudo75  = CargarImagen("assets/escudo_75.bmp");
    hBmpEscudo50  = CargarImagen("assets/escudo_50.bmp");
    hBmpEscudo25  = CargarImagen("assets/escudo_25.bmp");
    hBmpEscudo0   = CargarImagen("assets/escudo_0.bmp");

    hBmpBarraXPVacia = CargarImagen("assets/barra_xp_vacia.bmp");
    hBmpBarraXPLlena = CargarImagen("assets/barra_xp_llena.bmp");
}

void LiberarRecursos() {
    for(int i=0; i<4; i++) 
        for(int j=0; j<3; j++) 
            if(hBmpJugadorAnim[i][j]) DeleteObject(hBmpJugadorAnim[i][j]);

    // Borramos todo para no dejar basura en RAM
    DeleteObject(hBmpFondoMenu);
    DeleteObject(hBmpIsla);
    DeleteObject(hBmpTitulo);
    DeleteObject(hBmpBoton);
    DeleteObject(hBmpBotonSel);
    
    DeleteObject(hBmpInvCerrado);
    DeleteObject(hBmpInvAbierto);
    DeleteObject(hBmpIconoMadera);
    DeleteObject(hBmpIconoPiedra);
    DeleteObject(hBmpIconoOro);
    DeleteObject(hBmpIconoHierro);
    DeleteObject(hBmpIconoComida);
    DeleteObject(hBmpIconoHoja);
    DeleteObject(hBmpCorazon100); 
    // ... (puedes añadir el resto aquí si quieres ser purista)
}

// --- FUNCIÓN DE DIBUJADO MÁGICA ---
// Esta función reemplaza a tu antigua "dibujar_sprite"
void DibujarImagen(HDC hdcDestino, HBITMAP hBitmap, int x, int y, int ancho, int alto) {
    if (hBitmap == NULL) return; 

    HDC hdcMemoria = CreateCompatibleDC(hdcDestino);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMemoria, hBitmap);

    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);
    
    // Mejorar calidad al redimensionar (evita que se vea pixelado feo)
    SetStretchBltMode(hdcDestino, HALFTONE);

    // USAMOS TRANSPARENTBLT EN LUGAR DE STRETCHBLT
    // RGB(255, 0, 255) es el MAGENTA puro. Ese color será invisible.
    TransparentBlt(hdcDestino, x, y, ancho, alto, hdcMemoria, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 0, 255)); 

    SelectObject(hdcMemoria, hOld);
    DeleteDC(hdcMemoria);
}