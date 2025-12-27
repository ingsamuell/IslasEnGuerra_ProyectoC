/* src/recursos/recursos.c - GESTIÓN DE RECURSOS GRÁFICOS */
#include "recursos.h"
#include <stdio.h>

// --- DEFINICIÓN DE VARIABLES GLOBALES ---

// 1. MENÚ
HBITMAP hBmpFondoMenu = NULL;
HBITMAP hBmpBtnJugar = NULL;
HBITMAP hBmpBtnPartidas = NULL;
HBITMAP hBmpBtnInstrucciones = NULL;
HBITMAP hBmpBtnSalir = NULL;
HBITMAP hBmpTitulo = NULL;
HBITMAP hBmpBoton = NULL;
HBITMAP hBmpBotonSel = NULL;

// 2. JUGADOR
HBITMAP hBmpJugador = NULL;
HBITMAP hBmpJugadorAnim[4][3];  // [Dirección][Frame]
HBITMAP hBmpArmaduraAnim[4][3]; // [Dirección][Frame]

// 3. TIENDA Y MUNDO
HBITMAP hBmpTienda[2];
HBITMAP hBmpIslaGrande = NULL;
HBITMAP hBmpIslaSec1 = NULL;
HBITMAP hBmpIslaSec2 = NULL;
HBITMAP hBmpIslaSec3 = NULL;
HBITMAP hBmpIslaSec4 = NULL;
HBITMAP hBmpCastilloJugador = NULL;
HBITMAP hBmpArbol = NULL;
HBITMAP hBmpRoca = NULL;

// 4. INTERFAZ (UI)
HBITMAP hBmpInvCerrado = NULL;
HBITMAP hBmpInvAbierto = NULL;
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

// 5. ÍTEMS
HBITMAP hBmpIconoMadera = NULL;
HBITMAP hBmpIconoPiedra = NULL;
HBITMAP hBmpIconoOro = NULL;
HBITMAP hBmpIconoHierro = NULL;
HBITMAP hBmpIconoComida = NULL;
HBITMAP hBmpIconoHoja = NULL;
HBITMAP hBmpIconoEspada = NULL;
HBITMAP hBmpIconoPico = NULL;

// --- FUNCIÓN AUXILIAR DE CARGA ---
HBITMAP CargarImagen(const char *ruta)
{
    HBITMAP hBmp = (HBITMAP)LoadImage(NULL, ruta, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBmp)
    {
        // Mensaje de depuración en consola si falla (útil para detectar nombres incorrectos)
        printf("ERROR: No se pudo cargar la imagen: %s\n", ruta);
    }
    return hBmp;
}

// --- CARGA DE TODOS LOS RECURSOS ---
void CargarRecursos()
{
    // --- 1. UI DEL MENÚ ---
    hBmpFondoMenu = CargarImagen("assets/ui/fondo_menu.bmp");
    hBmpBtnJugar = CargarImagen("assets/ui/btn_jugar.bmp");
    hBmpBtnPartidas = CargarImagen("assets/ui/btn_partidas.bmp");
    hBmpBtnInstrucciones = CargarImagen("assets/ui/btn_instrucciones.bmp");
    hBmpBtnSalir = CargarImagen("assets/ui/btn_salir.bmp");

    // Fallbacks (por si acaso)
    hBmpBoton = CargarImagen("assets/ui/btn_jugar.bmp");

    // --- 2. INTERFAZ DE JUEGO (HUD) ---
    hBmpInvCerrado = CargarImagen("assets/ui/boton_inv_cerrado.bmp");
    hBmpInvAbierto = CargarImagen("assets/ui/boton_inv_abierto.bmp");

    // Corazones
    hBmpCorazon100 = CargarImagen("assets/ui/corazon_100.bmp");
    hBmpCorazon75 = CargarImagen("assets/ui/corazon_75.bmp");
    hBmpCorazon50 = CargarImagen("assets/ui/corazon_50.bmp");
    hBmpCorazon25 = CargarImagen("assets/ui/corazon_25.bmp");
    hBmpCorazon0 = CargarImagen("assets/ui/corazon_0.bmp");

    // Escudos
    hBmpEscudo100 = CargarImagen("assets/ui/escudo_100.bmp");
    hBmpEscudo75 = CargarImagen("assets/ui/escudo_75.bmp");
    hBmpEscudo50 = CargarImagen("assets/ui/escudo_50.bmp");
    hBmpEscudo25 = CargarImagen("assets/ui/escudo_25.bmp");
    hBmpEscudo0 = CargarImagen("assets/ui/escudo_0.bmp");

    // Barras
    hBmpBarraXPVacia = CargarImagen("assets/ui/barra_xp_vacia.bmp");
    hBmpBarraXPLlena = CargarImagen("assets/ui/barra_xp_llena.bmp");

    // --- 3. MUNDO E ISLAS ---
    // Nombres exactos basados en tus archivos subidos
    hBmpIslaGrande = CargarImagen("assets/mundo/Isla_grande.bmp");
    hBmpIslaSec1 = CargarImagen("assets/mundo/islasecundaria_UNO.bmp");
    hBmpIslaSec2 = CargarImagen("assets/mundo/islasecundaria_DOS.bmp");
    hBmpIslaSec3 = CargarImagen("assets/mundo/islasecundaria_TRES.bmp");
    hBmpIslaSec4 = CargarImagen("assets/mundo/islasecundaria_CUATRO.bmp");

    hBmpCastilloJugador = CargarImagen("assets/mundo/Castillo_jugador.bmp");
    hBmpArbol = CargarImagen("assets/mundo/Arbol_islaUno.bmp"); // Ejemplo
    // Si tienes roca: hBmpRoca = CargarImagen("assets/mundo/Roca.bmp");

    // Tienda (Gato)
    hBmpTienda[0] = CargarImagen("assets/mundo/tienda-de-ventas-pixilart.bmp");
    hBmpTienda[1] = CargarImagen("assets/mundo/tienda-de-ventas-pixilart-movimiento.bmp");

    // --- 4. ÍTEMS ---
    hBmpIconoMadera = CargarImagen("assets/items/icono_madera.bmp");
    hBmpIconoPiedra = CargarImagen("assets/items/icono_piedra.bmp");
    hBmpIconoOro = CargarImagen("assets/items/icono_oro.bmp");
    hBmpIconoHierro = CargarImagen("assets/items/icono_hierro.bmp");
    hBmpIconoComida = CargarImagen("assets/items/icono_comida.bmp");
    hBmpIconoHoja = CargarImagen("assets/items/icono_hoja.bmp");

    hBmpIconoEspada = CargarImagen("assets/items/item_espada.bmp");
    hBmpIconoPico = CargarImagen("assets/items/item_pico.bmp");

    // --- 5. JUGADOR (ANIMACIONES) ---
    // Mapeo: 0=Abajo(Frente), 1=Arriba(Espalda), 2=Izquierda, 3=Derecha

    // Frente
    hBmpJugadorAnim[0][0] = CargarImagen("assets/jugador/p_frente_base.bmp");
    hBmpJugadorAnim[0][1] = CargarImagen("assets/jugador/p_frente_pie1.bmp");
    hBmpJugadorAnim[0][2] = CargarImagen("assets/jugador/p_frente_pie2.bmp");

    // Espalda
    hBmpJugadorAnim[1][0] = CargarImagen("assets/jugador/p_esp_base.bmp");
    hBmpJugadorAnim[1][1] = CargarImagen("assets/jugador/p_esp_pie1.bmp");
    hBmpJugadorAnim[1][2] = CargarImagen("assets/jugador/p_esp_pie2.bmp");

    // Izquierda
    hBmpJugadorAnim[2][0] = CargarImagen("assets/jugador/p_izq_base.bmp");
    hBmpJugadorAnim[2][1] = CargarImagen("assets/jugador/p_izq_pie1.bmp");
    hBmpJugadorAnim[2][2] = CargarImagen("assets/jugador/p_izq_pie2.bmp");

    // Derecha
    hBmpJugadorAnim[3][0] = CargarImagen("assets/jugador/p_der_base.bmp");
    hBmpJugadorAnim[3][1] = CargarImagen("assets/jugador/p_der_pie1.bmp");
    hBmpJugadorAnim[3][2] = CargarImagen("assets/jugador/p_der_pie2.bmp");

    // Base fallback
    hBmpJugador = hBmpJugadorAnim[0][0];

    // --- ARMADURA ---
    // Carga básica de armadura (puedes expandir si tienes todos los frames)
    hBmpArmaduraAnim[0][0] = CargarImagen("assets/jugador/armadura-base-pixilart.bmp");
    hBmpArmaduraAnim[1][0] = CargarImagen("assets/jugador/armadura-base-espalda-pixilart.bmp");
    hBmpArmaduraAnim[2][0] = CargarImagen("assets/jugador/armadura-base-izquierda-pixilart.bmp");
    hBmpArmaduraAnim[3][0] = CargarImagen("assets/jugador/armadura-base-derecha-pixilart.bmp");
}

// --- LIBERACIÓN DE RECURSOS ---
void LiberarRecursos()
{
    // Liberar UI
    if (hBmpFondoMenu)
        DeleteObject(hBmpFondoMenu);
    if (hBmpBtnJugar)
        DeleteObject(hBmpBtnJugar);
    // ... (Se pueden agregar todos, Windows libera recursos al cerrar el proceso,
    // pero es buena práctica hacerlo aquí si el juego reinicia mucho)

    // Liberar Islas
    if (hBmpIslaGrande)
        DeleteObject(hBmpIslaGrande);
    if (hBmpIslaSec1)
        DeleteObject(hBmpIslaSec1);
    if (hBmpIslaSec2)
        DeleteObject(hBmpIslaSec2);
    if (hBmpIslaSec3)
        DeleteObject(hBmpIslaSec3);
    if (hBmpIslaSec4)
        DeleteObject(hBmpIslaSec4);
}

// --- DIBUJADO GENÉRICO (TRANSPARENTE MAGENTA) ---
void DibujarImagen(HDC hdcDestino, HBITMAP hBitmap, int x, int y, int ancho, int alto)
{
    if (!hBitmap)
        return;

    HDC hdcMem = CreateCompatibleDC(hdcDestino);
    SelectObject(hdcMem, hBitmap);

    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);

    SetStretchBltMode(hdcDestino, HALFTONE);

    // Usamos RGB(255, 0, 255) (Magenta) como color transparente
    TransparentBlt(hdcDestino, x, y, ancho, alto, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 0, 255));

    DeleteDC(hdcMem);
}