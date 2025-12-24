/* src/mapa/mapa.c - VERSIÓN: BOTONES HORIZONTALES */
#include "mapa.h"
#include "recursos/recursos.h"
#include <stdio.h>
#include <math.h>

#define MAX_ISLAS 5
Isla misIslas[MAX_ISLAS];

// --- INICIALIZACIÓN ---
void inicializarIslas() {
    for (int i = 0; i < MAX_ISLAS; i++) misIslas[i].activa = 0;
    
    // Isla Central
    misIslas[0].activa = 1; misIslas[0].x = 1350; misIslas[0].y = 1350;
    misIslas[0].ancho = 700; misIslas[0].alto = 700; misIslas[0].margen = 60;

    // Isla Vecina
    misIslas[1].activa = 1; misIslas[1].x = 2200; misIslas[1].y = 800;
    misIslas[1].ancho = 500; misIslas[1].alto = 500; misIslas[1].margen = 60;
}

void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    for (int i = 0; i < MUNDO_FILAS; i++)
        for (int j = 0; j < MUNDO_COLUMNAS; j++)
            mapa[i][j] = 0;
}

void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    inicializarIslas();
    inicializarMapa(mapa);
    
    jugador->x = 1600; jugador->y = 1600;
    jugador->velocidad = 5;
    jugador->vidaActual = 100; jugador->vidaMax = 100;
    jugador->armaduraActual = 0; jugador->armaduraMax = 100;
    jugador->nivel = 1; jugador->experiencia = 0; jugador->experienciaSiguienteNivel = 100;
    
    estado->enPartida = 0;
    estado->mostrarMenu = 1;
    estado->opcionSeleccionada = -1;
}

// --- FÍSICA Y MOVIMIENTO ---
int EsSuelo(int x, int y) {
    for (int i = 0; i < MAX_ISLAS; i++) {
        if (!misIslas[i].activa) continue;
        Isla is = misIslas[i];
        if (x >= is.x + is.margen && x <= is.x + is.ancho - is.margen &&
            y >= is.y + is.margen && y <= is.y + is.alto - is.margen) return 1;
    }
    return 0;
}

void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy) {
    if (dy > 0) jugador->direccion = DIR_ABAJO;
    else if (dy < 0) jugador->direccion = DIR_ARRIBA;
    else if (dx < 0) jugador->direccion = DIR_IZQUIERDA;
    else if (dx > 0) jugador->direccion = DIR_DERECHA;

    jugador->pasoAnimacion++;
    int estado = (jugador->pasoAnimacion / 4) % 4;
    if (estado == 0) jugador->frameAnim = 1;
    else if (estado == 1) jugador->frameAnim = 0;
    else if (estado == 2) jugador->frameAnim = 2;
    else if (estado == 3) jugador->frameAnim = 0;

    int futuraX = jugador->x + (dx * jugador->velocidad);
    int futuraY = jugador->y + (dy * jugador->velocidad);
    if (EsSuelo(futuraX, futuraY)) {
        jugador->x = futuraX; jugador->y = futuraY;
    }
}

void actualizarCamara(Camera *camara, Jugador jugador) {
    int anchoPantalla = GetSystemMetrics(SM_CXSCREEN);
    int altoPantalla = GetSystemMetrics(SM_CYSCREEN);
    int centroX = (anchoPantalla / 2) / camara->zoom;
    int centroY = (altoPantalla / 2) / camara->zoom;
    camara->x = jugador.x - centroX;
    camara->y = jugador.y - centroY;
    if (camara->x < 0) camara->x = 0;
    if (camara->y < 0) camara->y = 0;
}

// =========================================================
// AQUÍ ESTÁ EL CAMBIO HORIZONTAL (DIBUJADO)
// =========================================================
void dibujarMenuConSprites(HDC hdc, HWND hwnd, EstadoJuego *estado) {
    RECT rect; GetClientRect(hwnd, &rect);
    int ancho = rect.right;
    int alto = rect.bottom;

    // 1. Fondo
    if (hBmpFondoMenu) {
        BITMAP bm; GetObject(hBmpFondoMenu, sizeof(BITMAP), &bm);
        HDC hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, hBmpFondoMenu);
        SetStretchBltMode(hdc, HALFTONE);
        StretchBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        DeleteDC(hdcMem);
    } else {
        HBRUSH azul = CreateSolidBrush(RGB(20, 20, 60)); FillRect(hdc, &rect, azul); DeleteObject(azul);
    }

    // --- CONFIGURACIÓN HORIZONTAL ---
    int btnAncho = 500; // <--- Reducido para que quepan 3
    int btnAlto = 200;   
    int separacion = 30;
    
    // Calculamos el ancho total del grupo de 3 botones
    int totalAncho = (btnAncho * 3) + (separacion * 2);
    
    // Centramos horizontalmente el grupo
    int startX = (ancho - totalAncho) / 2;
    
    // Posición Y fija (Un poco más abajo del centro)
    int fixedY = (alto / 2) ; 

    // Botón 1: JUGAR (Izquierda)
    HBITMAP imgJugar = (hBmpBtnJugar) ? hBmpBtnJugar : hBmpBoton;
    DibujarImagen(hdc, imgJugar, startX, fixedY, btnAncho, btnAlto);

    // Botón 2: PARTIDAS (Centro)
    HBITMAP imgPartidas = (hBmpBtnPartidas) ? hBmpBtnPartidas : hBmpBoton;
    DibujarImagen(hdc, imgPartidas, startX + btnAncho + separacion, fixedY, btnAncho, btnAlto);

    // Botón 3: INSTRUCCIONES (Derecha)
    HBITMAP imgInstr = (hBmpBtnInstrucciones) ? hBmpBtnInstrucciones : hBmpBoton;
    DibujarImagen(hdc, imgInstr, startX + (btnAncho + separacion)*2, fixedY, btnAncho, btnAlto);

    // --- BOTÓN SALIR (SE MANTIENE EN ESQUINA) ---
    int salirW = 500; int salirH = 200; int margen = 30;
    int salirX = ancho - salirW - margen;
    int salirY = alto - salirH - margen;

    HBITMAP imgSalir = (hBmpBtnSalir) ? hBmpBtnSalir : hBmpBoton;
    DibujarImagen(hdc, imgSalir, salirX, salirY, salirW, salirH);
}

// --- LÓGICA DE CLICS DEL MENÚ (CAMBIO HORIZONTAL) ---
int verificarColisionBoton(int mouseX, int mouseY, int btnX, int btnY, int btnAncho, int btnAlto) {
    return (mouseX >= btnX && mouseX <= btnX + btnAncho && mouseY >= btnY && mouseY <= btnY + btnAlto);
}

void actualizarPuntoMenu(EstadoJuego *estado, int x, int y, HWND hwnd) {
    RECT rc; GetClientRect(hwnd, &rc);
    int ancho = rc.right;
    int alto = rc.bottom;

    // MISMOS VALORES QUE EN EL DIBUJADO
    int btnAncho = 500; 
    int btnAlto = 200;
    int separacion = 30;
    int totalAncho = (btnAncho * 3) + (separacion * 2);
    int startX = (ancho - totalAncho) / 2;
    int fixedY = (alto / 2) ;

    estado->opcionSeleccionada = -1;

    // 0: JUGAR (Izquierda)
    if (verificarColisionBoton(x, y, startX, fixedY, btnAncho, btnAlto)) 
        estado->opcionSeleccionada = 0;
    
    // 1: PARTIDAS (Centro) - Sumamos 1 ancho + 1 separación
    else if (verificarColisionBoton(x, y, startX + btnAncho + separacion, fixedY, btnAncho, btnAlto)) 
        estado->opcionSeleccionada = 1;
    
    // 2: INSTRUCCIONES (Derecha) - Sumamos 2 anchos + 2 separaciones
    else if (verificarColisionBoton(x, y, startX + (btnAncho + separacion)*2, fixedY, btnAncho, btnAlto)) 
        estado->opcionSeleccionada = 2;
    
    // 3: SALIR (Esquina)
    int salirW = 500; int salirH = 200; int margen = 30;
    if (verificarColisionBoton(x, y, ancho - salirW - margen, alto - salirH - margen, salirW, salirH))
        estado->opcionSeleccionada = 3;
}

void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado) {
    actualizarPuntoMenu(estado, x, y, hwnd);
    if (estado->opcionSeleccionada != -1) {
        switch (estado->opcionSeleccionada) {
            case 0: estado->mostrarMenu = 0; estado->enPartida = 1; break;
            case 1: MessageBox(hwnd, "Guardado no disponible aun.", "Partidas", MB_OK); break;
            case 2: MessageBox(hwnd, "WASD para moverte.", "Ayuda", MB_OK); break;
            case 3: PostQuitMessage(0); return;
        }
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void procesarEnterMenu(HWND hwnd, EstadoJuego *estado) {
    if (estado->mostrarMenu) {
        estado->mostrarMenu = 0;
        estado->enPartida = 1;
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

// --- DIBUJADO DEL JUEGO Y HUD (SIN CAMBIOS) ---
void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera cam, int ancho, int alto) {
    HBRUSH agua = CreateSolidBrush(RGB(0, 100, 180)); RECT r = {0, 0, ancho, alto};
    FillRect(hdc, &r, agua); DeleteObject(agua);

    for (int i = 0; i < MAX_ISLAS; i++) {
        if (!misIslas[i].activa) continue;
        int sx = (misIslas[i].x - cam.x) * cam.zoom;
        int sy = (misIslas[i].y - cam.y) * cam.zoom;
        int sw = misIslas[i].ancho * cam.zoom;
        int sh = misIslas[i].alto * cam.zoom;
        HBITMAP img = (i == 0) ? hBmpIslaCentral : hBmpIslaNoreste;
        if (img) DibujarImagen(hdc, img, sx, sy, sw, sh);
    }
}

void dibujarJugador(HDC hdc, Jugador jugador, Camera cam) {
    RECT r; GetClipBox(hdc, &r);
    int tam = 32 * cam.zoom;
    int cx = (r.right / 2) - (tam/2);
    int cy = (r.bottom / 2) - (tam/2);
    HBITMAP sprite = hBmpJugadorAnim[jugador.direccion][jugador.frameAnim];
    if (!sprite) sprite = hBmpJugador;
    DibujarImagen(hdc, sprite, cx, cy, tam, tam);
}

void dibujarHUD(HDC hdc, Jugador* jugador, int ancho, int alto) {
    // 1. VIDA
    for (int i = 0; i < 5; i++) {
        int vida = jugador->vidaActual - (i * 20);
        HBITMAP cor = hBmpCorazon0;
        if (vida >= 20) cor = hBmpCorazon100;
        else if (vida >= 15) cor = hBmpCorazon75;
        else if (vida >= 10) cor = hBmpCorazon50;
        else if (vida >= 5) cor = hBmpCorazon25;
        DibujarImagen(hdc, cor, 20 + (i * 32), 20, 32, 32);
    }

    // 2. ARMADURA
    HBITMAP bmpEscudo = hBmpEscudo0;
    float pEscudo = (jugador->armaduraMax > 0) ? (float)jugador->armaduraActual / jugador->armaduraMax : 0;
    if (pEscudo >= 0.8) bmpEscudo = hBmpEscudo100;
    else if (pEscudo >= 0.6) bmpEscudo = hBmpEscudo75;
    else if (pEscudo >= 0.4) bmpEscudo = hBmpEscudo50;
    else if (pEscudo > 0)    bmpEscudo = hBmpEscudo25;
    if (bmpEscudo) DibujarImagen(hdc, bmpEscudo, 20, 60, 48, 48);
    char tEscudo[10]; sprintf(tEscudo, "%d", jugador->armaduraActual);
    SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(255,255,255));
    TextOut(hdc, 75, 75, tEscudo, strlen(tEscudo));

    // 3. MOCHILA
    HBITMAP bmpBolso = (jugador->inventarioAbierto) ? hBmpInvAbierto : hBmpInvCerrado;
    if (bmpBolso) DibujarImagen(hdc, bmpBolso, 20, 120, 64, 64);

    // 4. PANEL
    if (jugador->inventarioAbierto) {
        int px = 90, py = 120, pw = 200, ph = 350;
        HBRUSH fondo = CreateSolidBrush(RGB(40,40,40)); RECT r = {px, py, px+pw, py+ph};
        FillRect(hdc, &r, fondo); DeleteObject(fondo);
        SetTextColor(hdc, RGB(255,255,255)); TextOut(hdc, px+10, py+10, "MOCHILA", 7);
        int iy = py + 40; char buf[32];
        if(hBmpIconoMadera) DibujarImagen(hdc, hBmpIconoMadera, px+10, iy, 32, 32);
        sprintf(buf, "x %d", jugador->madera); TextOut(hdc, px+50, iy+8, buf, strlen(buf)); iy+=40;
        if(hBmpIconoPiedra) DibujarImagen(hdc, hBmpIconoPiedra, px+10, iy, 32, 32);
        sprintf(buf, "x %d", jugador->piedra); TextOut(hdc, px+50, iy+8, buf, strlen(buf)); iy+=40;
        if(hBmpIconoOro) DibujarImagen(hdc, hBmpIconoOro, px+10, iy, 32, 32);
        sprintf(buf, "x %d", jugador->oro); TextOut(hdc, px+50, iy+8, buf, strlen(buf));
    }

    // 5. XP
    if (hBmpBarraXPVacia && hBmpBarraXPLlena) {
        BITMAP bm; GetObject(hBmpBarraXPVacia, sizeof(BITMAP), &bm);
        int xpW = bm.bmWidth; int xpH = bm.bmHeight;
        int xpX = (ancho - xpW) / 2; int xpY = alto - xpH - 20;
        DibujarImagen(hdc, hBmpBarraXPVacia, xpX, xpY, xpW, xpH);
        float pct = 0;
        if (jugador->experienciaSiguienteNivel > 0) pct = (float)jugador->experiencia / jugador->experienciaSiguienteNivel;
        if (pct > 1) pct = 1;
        int fillW = (int)(xpW * pct);
        if (fillW > 0) {
            HDC hdcMem = CreateCompatibleDC(hdc); HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmpBarraXPLlena);
            TransparentBlt(hdc, xpX, xpY, fillW, xpH, hdcMem, 0, 0, fillW, xpH, RGB(255, 0, 255));
            SelectObject(hdcMem, hOld); DeleteDC(hdcMem);
        }
        char tNivel[32]; sprintf(tNivel, "NVL %d", jugador->nivel);
        SetTextColor(hdc, RGB(0, 255, 255)); TextOut(hdc, xpX - 60, xpY + 5, tNivel, strlen(tNivel));
    }
}