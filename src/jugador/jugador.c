/* src/jugador/jugador.c */
#include "jugador.h"
#include "../recursos/recursos.h"
#include <stdio.h>

// =========================================================
// 1. LÓGICA INTERNA
// =========================================================

int obtenerCapacidadMaxima(int nivel) {
    if (nivel == 1) return 99;
    if (nivel == 2) return 499;
    if (nivel == 3) return 999;
    return 99;
}

// ESTA ES LA FUNCIÓN QUE MAPA.C ESTABA BUSCANDO
void agregarRecurso(int *recurso, int cantidad, int nivelMochila) {
    int max = obtenerCapacidadMaxima(nivelMochila);
    *recurso += cantidad;
    
    // Evitar desbordamiento
    if (*recurso > max) *recurso = max;
    if (*recurso < 0) *recurso = 0;
}

void ganarExperiencia(Jugador *j, int cantidad) {
    j->experiencia += cantidad;
    if (j->experiencia >= j->experienciaSiguienteNivel) {
        j->nivel++;
        j->experiencia = 0;
        j->experienciaSiguienteNivel = (int)(j->experienciaSiguienteNivel * 1.5);
        j->vidaMax += 20; 
        j->vidaActual = j->vidaMax;
    }
}

void intentarCurar(Jugador *j) {
    if (j->hojas > 0 && j->vidaActual < j->vidaMax) {
        j->hojas--;
        j->vidaActual += 10;
        if (j->vidaActual > j->vidaMax) j->vidaActual = j->vidaMax;
    }
}

void alternarInventario(Jugador *j) {
    j->inventarioAbierto = !j->inventarioAbierto;
}

// =========================================================
// 2. FUNCIONES VISUALES
// =========================================================

void dibujarItemRejilla(HDC hdc, HBITMAP icono, int cantidad, int maximo, int x, int y, const char *nombre)
{
    if (icono) DibujarImagen(hdc, icono, x, y, 32, 32);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(200, 200, 200));
    TextOut(hdc, x + 40, y - 5, nombre, strlen(nombre));

    char buf[32];
    sprintf(buf, "%d / %d", cantidad, maximo);
    
    if (cantidad >= maximo) SetTextColor(hdc, RGB(255, 100, 100));
    else SetTextColor(hdc, RGB(255, 255, 255));
    
    TextOut(hdc, x + 40, y + 12, buf, strlen(buf));
}

// --- DIBUJADO DEL HUD (Tu diseño: Escudo Arriba, Puño Abajo) ---
void dibujarHUD(HDC hdc, Jugador *jugador, int ancho, int alto)
{
    // A. VIDA (Corazones) -> Y = 20
    for (int i = 0; i < 5; i++) {
        int vida = jugador->vidaActual - (i * 20);
        HBITMAP cor = hBmpCorazon0;
        if (vida >= 20) cor = hBmpCorazon100;
        else if (vida >= 15) cor = hBmpCorazon75;
        else if (vida >= 10) cor = hBmpCorazon50;
        else if (vida >= 5) cor = hBmpCorazon25;
        DibujarImagen(hdc, cor, 20 + (i * 32), 20, 32, 32);
    }

    // B. ESCUDO (Posición original restaurada: Y = 60)
    HBITMAP bmpEscudo = (jugador->armaduraActual > 0) ? hBmpEscudo100 : hBmpEscudo0;
    if (bmpEscudo) DibujarImagen(hdc, bmpEscudo, 20, 60, 48, 48);
    
    char tEscudo[16];
    sprintf(tEscudo, "%d", jugador->armaduraActual);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    TextOut(hdc, 75, 75, tEscudo, strlen(tEscudo));

    // C. BOTÓN MOCHILA -> Y = 120
    HBITMAP bmpBolso = (jugador->inventarioAbierto) ? hBmpInvAbierto : hBmpInvCerrado;
    if (bmpBolso) DibujarImagen(hdc, bmpBolso, 20, 120, 64, 64);

    // D. CAJA DE MANO / ITEM ACTIVO -> Y = 200 (Posición Abajo)
    int boxX = 20;
    int boxY = 200;

    // Dibujar el cuadro gris
    HBRUSH brushCaja = CreateSolidBrush(RGB(50, 50, 60));
    RECT rBox = {boxX, boxY, boxX + 48, boxY + 48};
    FillRect(hdc, &rBox, brushCaja); DeleteObject(brushCaja);
    
    HBRUSH brushBorde = CreateSolidBrush(RGB(200, 200, 200));
    FrameRect(hdc, &rBox, brushBorde); DeleteObject(brushBorde);

    // Determinar qué icono mostrar (Puño o Herramienta)
    HBITMAP iconoMano = hBmpIconoPuno; // Por defecto: PUÑO

    if (jugador->herramientaActiva == HERRAMIENTA_ESPADA) iconoMano = hBmpIconoEspada;
    else if (jugador->herramientaActiva == HERRAMIENTA_PICO) iconoMano = hBmpIconoPico;
    else if (jugador->herramientaActiva == HERRAMIENTA_HACHA) iconoMano = hBmpIconoHacha;
    
    // Dibujar el icono centrado
    if (iconoMano) DibujarImagen(hdc, iconoMano, boxX + 8, boxY + 8, 32, 32);

    // E. INVENTARIO ABIERTO (Rejilla)
    if (jugador->inventarioAbierto)
    {
        int px = 90, py = 120;
        int anchoFondo = 360;
        int alturaFondo = 300;

        HBRUSH fondo = CreateSolidBrush(RGB(30, 30, 35));
        RECT r = {px, py, px + anchoFondo, py + alturaFondo};
        FillRect(hdc, &r, fondo); DeleteObject(fondo);
        
        HBRUSH bordeInv = CreateSolidBrush(RGB(255, 255, 255));
        FrameRect(hdc, &r, bordeInv); DeleteObject(bordeInv);

        int startX = px + 20;
        int startY = py + 40;
        int maxCap = obtenerCapacidadMaxima(jugador->nivelMochila);

        // Fila 1
        dibujarItemRejilla(hdc, hBmpIconoMadera, jugador->madera, maxCap, startX, startY, "Madera");
        dibujarItemRejilla(hdc, hBmpIconoPiedra, jugador->piedra, maxCap, startX + 110, startY, "Piedra");
        dibujarItemRejilla(hdc, hBmpIconoHierro, jugador->hierro, maxCap, startX + 220, startY, "Hierro");

        // Fila 2
        dibujarItemRejilla(hdc, hBmpIconoHoja, jugador->hojas, maxCap, startX, startY + 60, "Hojas");
        dibujarItemRejilla(hdc, hBmpIconoComida, jugador->comida, maxCap, startX + 110, startY + 60, "Comida");
        dibujarItemRejilla(hdc, hBmpIconoOro, jugador->oro, 9999, startX + 220, startY + 60, "Oro");

        // Fila 3 (Herramientas)
        if (jugador->nivelMochila >= 2) {
            if (jugador->tieneHacha) DibujarImagen(hdc, hBmpIconoHacha, startX, startY + 120, 32, 32);
            else { SetTextColor(hdc, RGB(100,100,100)); TextOut(hdc, startX, startY+130, "Bloq.", 5); }

            if (jugador->tienePico) DibujarImagen(hdc, hBmpIconoPico, startX + 110, startY + 120, 32, 32);
            else { SetTextColor(hdc, RGB(100,100,100)); TextOut(hdc, startX+110, startY+130, "Bloq.", 5); }

            if (jugador->tieneEspada) DibujarImagen(hdc, hBmpIconoEspada, startX + 220, startY + 120, 32, 32);
            else { SetTextColor(hdc, RGB(100,100,100)); TextOut(hdc, startX+220, startY+130, "Bloq.", 5); }
            
            SetTextColor(hdc, RGB(150, 150, 150));
            TextOut(hdc, startX, startY + 160, "Usa 1, 2, 3 para equipar", 24);
        } else {
            SetTextColor(hdc, RGB(100, 100, 100));
            TextOut(hdc, startX, startY + 130, "Nivel 2 para desbloquear armas", 30);
        }
        
        char tNivel[32]; sprintf(tNivel, "Mochila Nivel %d", jugador->nivelMochila);
        SetTextColor(hdc, RGB(0, 255, 255));
        TextOut(hdc, px + 20, py + alturaFondo - 25, tNivel, strlen(tNivel));
    }

    // F. BARRA XP
    if (hBmpBarraXPVacia && hBmpBarraXPLlena) {
        BITMAP bm; GetObject(hBmpBarraXPVacia, sizeof(BITMAP), &bm);
        int xpX = (ancho - bm.bmWidth) / 2;
        int xpY = alto - bm.bmHeight - 20;
        DibujarImagen(hdc, hBmpBarraXPVacia, xpX, xpY, bm.bmWidth, bm.bmHeight);

        float pct = (float)jugador->experiencia / jugador->experienciaSiguienteNivel;
        if (pct > 1) pct = 1;
        int fillW = (int)(bm.bmWidth * pct);
        if (fillW > 0) {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmpBarraXPLlena);
            TransparentBlt(hdc, xpX, xpY, fillW, bm.bmHeight, hdcMem, 0, 0, fillW, bm.bmHeight, RGB(255, 0, 255));
            SelectObject(hdcMem, hOld); DeleteDC(hdcMem);
        }
        char tNivel[32]; sprintf(tNivel, "NVL %d", jugador->nivel);
        SetTextColor(hdc, RGB(0, 255, 255));
        TextOut(hdc, xpX - 60, xpY + 5, tNivel, strlen(tNivel));
    }
}

// --- ARREGLADO: LLAVES EN LOS IF PARA EVITAR WARNINGS ---
void dibujarJugador(HDC hdc, Jugador jugador, Camera cam)
{
    int tam = 32 * cam.zoom;
    RECT r; GetClipBox(hdc, &r);
    int cx = (r.right / 2) - (tam / 2);
    int cy = (r.bottom / 2) - (tam / 2);

    HBITMAP spriteFinal = hBmpJugador; // Default
    int dir = jugador.direccion;
    int anim = jugador.frameAnim;
    
	if (jugador.estadoBarco > 0) {
        HBITMAP imgBarco = NULL;
        int dir = (jugador.direccion == DIR_IZQUIERDA) ? 0 : 1; 
        
        if (jugador.estadoBarco == 1) imgBarco = hBmpBote[dir];       
        else if (jugador.estadoBarco == 2) imgBarco = hBmpBarco[dir]; 
        
        if (imgBarco) {
            // Dibujamos el barco un poco más grande (64px) y centrado
            DibujarImagen(hdc, imgBarco, cx - 16, cy - 16, 80 * cam.zoom, 64 * cam.zoom);
        }
        
        if (jugador.estadoBarco == 1) {
            SetTextColor(hdc, RGB(0, 255, 255));
            TextOut(hdc, cx, cy - 40, "PESCANDO", 11);
        }
    }

    if (jugador.armaduraEquipada) {
        if (hBmpArmaduraAnim[dir][anim]) spriteFinal = hBmpArmaduraAnim[dir][anim];
    } else {
        switch (jugador.herramientaActiva) {
            case HERRAMIENTA_ESPADA:
                if (hBmpEspadaAnim[dir][anim]) { spriteFinal = hBmpEspadaAnim[dir][anim]; }
                break;
            case HERRAMIENTA_PICO:
                if (hBmpPicoAnim[dir][anim]) { spriteFinal = hBmpPicoAnim[dir][anim]; }
                break;
            case HERRAMIENTA_HACHA:
                if (hBmpHachaAnim[dir][anim]) { spriteFinal = hBmpHachaAnim[dir][anim]; }
                break;
            default:
                if (hBmpJugadorAnim[dir][anim]) { spriteFinal = hBmpJugadorAnim[dir][anim]; }
                break;
        }
    }
    DibujarImagen(hdc, spriteFinal, cx, cy, tam, tam);
} 

