#include "recursos.h"
#include <stdio.h>

// =========================================================
// 1. DEFINICIÓN DE VARIABLES
// =========================================================

// UI y Menú
HBITMAP hBmpFondoMenu = NULL;
HBITMAP hBmpTitulo = NULL;
HBITMAP hBmpBoton = NULL;
HBITMAP hBmpBotonSel = NULL;
HBITMAP hBmpInvCerrado = NULL;
HBITMAP hBmpInvAbierto = NULL;

// Jugador (Animación y estático)
HBITMAP hBmpJugador = NULL; // Compatible
HBITMAP hBmpJugadorAnim[4][3]; // Matriz de animación

// Mundo y Estructuras
HBITMAP hBmpIsla = NULL;
HBITMAP hBmpCastilloJugador = NULL;
HBITMAP hBmpCastilloEnemigo1 = NULL;
HBITMAP hBmpCastilloEnemigo2 = NULL;
HBITMAP hBmpCastilloEnemigo3 = NULL;

// Naturaleza
HBITMAP hBmpArbol = NULL;
HBITMAP hBmpRoca = NULL;

// Items
HBITMAP hBmpIconoMadera = NULL;
HBITMAP hBmpIconoPiedra = NULL;
HBITMAP hBmpIconoOro = NULL;
HBITMAP hBmpIconoHierro = NULL;
HBITMAP hBmpIconoComida = NULL;
HBITMAP hBmpIconoHoja = NULL;

// Stats (Vida/Escudo/XP)
HBITMAP hBmpCorazon100 = NULL; HBITMAP hBmpCorazon75 = NULL;
HBITMAP hBmpCorazon50 = NULL;  HBITMAP hBmpCorazon25 = NULL;
HBITMAP hBmpCorazon0 = NULL;

HBITMAP hBmpEscudo100 = NULL; HBITMAP hBmpEscudo75 = NULL;
HBITMAP hBmpEscudo50 = NULL;  HBITMAP hBmpEscudo25 = NULL;
HBITMAP hBmpEscudo0 = NULL;

HBITMAP hBmpBarraXPVacia = NULL;
HBITMAP hBmpBarraXPLlena = NULL;

// =========================================================
// 2. FUNCIÓN DE CARGA (RUTAS ORGANIZADAS)
// =========================================================
HBITMAP CargarImagen(const char* ruta) {
    HBITMAP hBmp = (HBITMAP)LoadImage(NULL, ruta, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (hBmp == NULL) {
        printf("ERROR CRITICO: No se encontro %s\n", ruta);
        // Tip: Si falla, verifica que el archivo esté en la carpeta correcta
    }
    return hBmp;
}

void CargarRecursos() {
    
    // --- 1. JUGADOR (Carpeta assets/jugador/) ---
    // Animaciones
    hBmpJugadorAnim[DIR_ABAJO][0] = CargarImagen("assets/jugador/p_frente_base.bmp");
    hBmpJugadorAnim[DIR_ABAJO][1] = CargarImagen("assets/jugador/p_frente_pie1.bmp");
    hBmpJugadorAnim[DIR_ABAJO][2] = CargarImagen("assets/jugador/p_frente_pie2.bmp");

    hBmpJugadorAnim[DIR_ARRIBA][0] = CargarImagen("assets/jugador/p_esp_base.bmp");
    hBmpJugadorAnim[DIR_ARRIBA][1] = CargarImagen("assets/jugador/p_esp_pie1.bmp");
    hBmpJugadorAnim[DIR_ARRIBA][2] = CargarImagen("assets/jugador/p_esp_pie2.bmp");

    hBmpJugadorAnim[DIR_IZQUIERDA][0] = CargarImagen("assets/jugador/p_izq_base.bmp");
    hBmpJugadorAnim[DIR_IZQUIERDA][1] = CargarImagen("assets/jugador/p_izq_pie1.bmp");
    hBmpJugadorAnim[DIR_IZQUIERDA][2] = CargarImagen("assets/jugador/p_izq_pie2.bmp");

    hBmpJugadorAnim[DIR_DERECHA][0] = CargarImagen("assets/jugador/p_der_base.bmp");
    hBmpJugadorAnim[DIR_DERECHA][1] = CargarImagen("assets/jugador/p_der_pie1.bmp");
    hBmpJugadorAnim[DIR_DERECHA][2] = CargarImagen("assets/jugador/p_der_pie2.bmp");

    // Imagen estática (si la sigues usando)
    hBmpJugador = CargarImagen("assets/jugador/jugador_compatible.bmp");

    // --- 2. UI / INTERFAZ (Carpeta assets/ui/) ---
    hBmpFondoMenu  = CargarImagen("assets/ui/imagen_menu.bmp");
    hBmpTitulo     = CargarImagen("assets/ui/titulo.bmp");
    hBmpBoton      = CargarImagen("assets/ui/boton.bmp");
    hBmpBotonSel   = CargarImagen("assets/ui/boton_sel.bmp");
    
    hBmpInvCerrado = CargarImagen("assets/ui/boton_inv_cerrado.bmp");
    hBmpInvAbierto = CargarImagen("assets/ui/boton_inv_abierto.bmp");

    // Vida
    hBmpCorazon100 = CargarImagen("assets/ui/corazon_100.bmp");
    hBmpCorazon75  = CargarImagen("assets/ui/corazon_75.bmp");
    hBmpCorazon50  = CargarImagen("assets/ui/corazon_50.bmp");
    hBmpCorazon25  = CargarImagen("assets/ui/corazon_25.bmp");
    hBmpCorazon0   = CargarImagen("assets/ui/corazon_0.bmp");

    // Escudo
    hBmpEscudo100 = CargarImagen("assets/ui/escudo_100.bmp");
    hBmpEscudo75  = CargarImagen("assets/ui/escudo_75.bmp");
    hBmpEscudo50  = CargarImagen("assets/ui/escudo_50.bmp");
    hBmpEscudo25  = CargarImagen("assets/ui/escudo_25.bmp");
    hBmpEscudo0   = CargarImagen("assets/ui/escudo_0.bmp");

    // XP
    hBmpBarraXPVacia = CargarImagen("assets/ui/barra_xp_vacia.bmp");
    hBmpBarraXPLlena = CargarImagen("assets/ui/barra_xp_llena.bmp");

    // --- 3. ITEMS (Carpeta assets/items/) ---
    hBmpIconoMadera = CargarImagen("assets/items/icono_madera.bmp");
    hBmpIconoPiedra = CargarImagen("assets/items/icono_piedra.bmp");
    hBmpIconoOro    = CargarImagen("assets/items/icono_oro.bmp");
    hBmpIconoHierro = CargarImagen("assets/items/icono_hierro.bmp");
    hBmpIconoComida = CargarImagen("assets/items/icono_comida.bmp");
    hBmpIconoHoja   = CargarImagen("assets/items/icono_hoja.bmp");

    // --- 4. MUNDO (Carpeta assets/mundo/) ---
    hBmpIsla = CargarImagen("assets/mundo/isla_principal.bmp"); // O ISLA_Primero.bmp

    // Castillos
    hBmpCastilloJugador   = CargarImagen("assets/mundo/Castillo_jugador.bmp");
    hBmpCastilloEnemigo1  = CargarImagen("assets/mundo/Castillo_enemigoUNO.bmp");
    hBmpCastilloEnemigo2  = CargarImagen("assets/mundo/Castillo_enemigoDOS.bmp");
    hBmpCastilloEnemigo3  = CargarImagen("assets/mundo/Castillo_enemigoTRES.bmp");

    // Naturaleza (Asegúrate de tener estos archivos o comenta las líneas si aun no los creas)
    // hBmpArbol = CargarImagen("assets/mundo/arbol.bmp");
    // hBmpRoca  = CargarImagen("assets/mundo/roca.bmp");
}

// =========================================================
// 3. LIBERACIÓN DE MEMORIA
// =========================================================
void LiberarRecursos() {
    // Jugador
    for(int i=0; i<4; i++) 
        for(int j=0; j<3; j++) 
            if(hBmpJugadorAnim[i][j]) DeleteObject(hBmpJugadorAnim[i][j]);
    if(hBmpJugador) DeleteObject(hBmpJugador);

    // UI
    DeleteObject(hBmpFondoMenu); DeleteObject(hBmpTitulo);
    DeleteObject(hBmpBoton); DeleteObject(hBmpBotonSel);
    DeleteObject(hBmpInvCerrado); DeleteObject(hBmpInvAbierto);
    
    DeleteObject(hBmpCorazon100); DeleteObject(hBmpCorazon75);
    DeleteObject(hBmpCorazon50);  DeleteObject(hBmpCorazon25); DeleteObject(hBmpCorazon0);
    
    DeleteObject(hBmpEscudo100);  DeleteObject(hBmpEscudo75);
    DeleteObject(hBmpEscudo50);   DeleteObject(hBmpEscudo25); DeleteObject(hBmpEscudo0);

    DeleteObject(hBmpBarraXPVacia); DeleteObject(hBmpBarraXPLlena);

    // Items
    DeleteObject(hBmpIconoMadera); DeleteObject(hBmpIconoPiedra);
    DeleteObject(hBmpIconoOro);    DeleteObject(hBmpIconoHierro);
    DeleteObject(hBmpIconoComida); DeleteObject(hBmpIconoHoja);

    // Mundo
    DeleteObject(hBmpIsla);
    DeleteObject(hBmpCastilloJugador);
    DeleteObject(hBmpCastilloEnemigo1);
    DeleteObject(hBmpCastilloEnemigo2);
    DeleteObject(hBmpCastilloEnemigo3);
    if(hBmpArbol) DeleteObject(hBmpArbol);
    if(hBmpRoca) DeleteObject(hBmpRoca);
}

// Función auxiliar de dibujado con transparencia
void DibujarImagen(HDC hdcDestino, HBITMAP hBitmap, int x, int y, int ancho, int alto) {
    if (hBitmap == NULL) return; 

    HDC hdcMemoria = CreateCompatibleDC(hdcDestino);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMemoria, hBitmap);

    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);
    
    SetStretchBltMode(hdcDestino, HALFTONE);
    // Transparencia con MAGENTA (255, 0, 255)
    TransparentBlt(hdcDestino, x, y, ancho, alto, hdcMemoria, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 0, 255)); 

    SelectObject(hdcMemoria, hOld);
    DeleteDC(hdcMemoria);
}