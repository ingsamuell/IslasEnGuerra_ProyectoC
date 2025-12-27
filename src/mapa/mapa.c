/* src/mapa/mapa.c - VERSIÓN LIMPIA Y CORREGIDA */
#include "mapa.h"
#include "recursos/recursos.h"
#include <stdio.h>
#include <math.h>
#include <mmsystem.h> // Necesario para PlaySound

#define MAX_ISLAS 5
Isla misIslas[MAX_ISLAS];

// --- INICIALIZACIÓN ---
void inicializarIslas()
{
    for (int i = 0; i < MAX_ISLAS; i++)
        misIslas[i].activa = 0;

    // Isla Central
    misIslas[0].activa = 1;
    misIslas[0].x = 1350;
    misIslas[0].y = 1350;
    misIslas[0].ancho = 700;
    misIslas[0].alto = 700;
    misIslas[0].margen = 60;

    // Isla Vecina
    misIslas[1].activa = 1;
    misIslas[1].x = 2200;
    misIslas[1].y = 800;
    misIslas[1].ancho = 500;
    misIslas[1].alto = 500;
    misIslas[1].margen = 60;
}

void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS])
{
    for (int i = 0; i < MUNDO_FILAS; i++)
        for (int j = 0; j < MUNDO_COLUMNAS; j++)
            mapa[i][j] = 0;
}

void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS])
{
    inicializarIslas();
    inicializarMapa(mapa);

    jugador->x = 1600;
    jugador->y = 1600;
    jugador->velocidad = 5;
    jugador->vidaActual = 100;
    jugador->vidaMax = 100;
    jugador->armaduraActual = 0;
    jugador->armaduraMax = 100;
    jugador->nivel = 1;
    jugador->experiencia = 0;
    jugador->experienciaSiguienteNivel = 100;
    jugador->nivelMochila = 1; // Empieza con la bolsa pequeña

    // Configuración inicial
    estado->enPartida = 0;
    estado->mostrarMenu = 1;
    estado->opcionSeleccionada = -1;
    estado->frameTienda = 0;
}

// --- FÍSICA Y MOVIMIENTO ---
int EsSuelo(int x, int y)
{
    for (int i = 0; i < MAX_ISLAS; i++)
    {
        if (!misIslas[i].activa)
            continue;
        Isla is = misIslas[i];
        if (x >= is.x + is.margen && x <= is.x + is.ancho - is.margen &&
            y >= is.y + is.margen && y <= is.y + is.alto - is.margen)
            return 1;
    }
    return 0;
}

void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy)
{
    if (dy > 0)
        jugador->direccion = DIR_ABAJO;
    else if (dy < 0)
        jugador->direccion = DIR_ARRIBA;
    else if (dx < 0)
        jugador->direccion = DIR_IZQUIERDA;
    else if (dx > 0)
        jugador->direccion = DIR_DERECHA;

    jugador->pasoAnimacion++;
    int estado = (jugador->pasoAnimacion / 4) % 4;
    if (estado == 0)
        jugador->frameAnim = 1;
    else if (estado == 1)
        jugador->frameAnim = 0;
    else if (estado == 2)
        jugador->frameAnim = 2;
    else if (estado == 3)
        jugador->frameAnim = 0;

    int futuraX = jugador->x + (dx * jugador->velocidad);
    int futuraY = jugador->y + (dy * jugador->velocidad);
    if (EsSuelo(futuraX, futuraY))
    {
        jugador->x = futuraX;
        jugador->y = futuraY;
    }
}

void actualizarCamara(Camera *camara, Jugador jugador)
{
    int anchoPantalla = GetSystemMetrics(SM_CXSCREEN);
    int altoPantalla = GetSystemMetrics(SM_CYSCREEN);
    int centroX = (anchoPantalla / 2) / camara->zoom;
    int centroY = (altoPantalla / 2) / camara->zoom;
    camara->x = jugador.x - centroX;
    camara->y = jugador.y - centroY;
    if (camara->x < 0)
        camara->x = 0;
    if (camara->y < 0)
        camara->y = 0;
}

// --- DIBUJADO DEL MENÚ (MEDIDAS ORIGINALES RESTAURADAS) ---
void dibujarMenuConSprites(HDC hdc, HWND hwnd, EstadoJuego *estado)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    int ancho = rect.right;
    int alto = rect.bottom;

    // 1. Fondo
    if (hBmpFondoMenu)
    {
        BITMAP bm;
        GetObject(hBmpFondoMenu, sizeof(BITMAP), &bm);
        HDC hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, hBmpFondoMenu);
        SetStretchBltMode(hdc, HALFTONE);
        StretchBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        DeleteDC(hdcMem);
    }
    else
    {
        HBRUSH azul = CreateSolidBrush(RGB(20, 20, 60));
        FillRect(hdc, &rect, azul);
        DeleteObject(azul);
    }

    // --- CONFIGURACIÓN HORIZONTAL DE BOTONES ---
    int btnAncho = 500;
    int btnAlto = 200;
    int separacion = 30;
    int totalAncho = (btnAncho * 3) + (separacion * 2);
    int startX = (ancho - totalAncho) / 2;
    int fixedY = (alto / 2);

    // Botones
    HBITMAP imgJugar = (hBmpBtnJugar) ? hBmpBtnJugar : hBmpBoton;
    DibujarImagen(hdc, imgJugar, startX, fixedY, btnAncho, btnAlto);

    HBITMAP imgPartidas = (hBmpBtnPartidas) ? hBmpBtnPartidas : hBmpBoton;
    DibujarImagen(hdc, imgPartidas, startX + btnAncho + separacion, fixedY, btnAncho, btnAlto);

    HBITMAP imgInstr = (hBmpBtnInstrucciones) ? hBmpBtnInstrucciones : hBmpBoton;
    DibujarImagen(hdc, imgInstr, startX + (btnAncho + separacion) * 2, fixedY, btnAncho, btnAlto);

    // Botón Salir
    int salirW = 500;
    int salirH = 200;
    int margen = 30;
    int salirX = ancho - salirW - margen;
    int salirY = alto - salirH - margen;
    HBITMAP imgSalir = (hBmpBtnSalir) ? hBmpBtnSalir : hBmpBoton;
    DibujarImagen(hdc, imgSalir, salirX, salirY, salirW, salirH);
}

// --- INTERACCIÓN MENÚ ---
int verificarColisionBoton(int mouseX, int mouseY, int btnX, int btnY, int btnAncho, int btnAlto)
{
    return (mouseX >= btnX && mouseX <= btnX + btnAncho && mouseY >= btnY && mouseY <= btnY + btnAlto);
}

void actualizarPuntoMenu(EstadoJuego *estado, int x, int y, HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    int ancho = rc.right;
    int alto = rc.bottom;

    int btnAncho = 500;
    int btnAlto = 200;
    int separacion = 30;
    int totalAncho = (btnAncho * 3) + (separacion * 2);
    int startX = (ancho - totalAncho) / 2;
    int fixedY = (alto / 2);

    estado->opcionSeleccionada = -1;

    if (verificarColisionBoton(x, y, startX, fixedY, btnAncho, btnAlto))
        estado->opcionSeleccionada = 0;
    else if (verificarColisionBoton(x, y, startX + btnAncho + separacion, fixedY, btnAncho, btnAlto))
        estado->opcionSeleccionada = 1;
    else if (verificarColisionBoton(x, y, startX + (btnAncho + separacion) * 2, fixedY, btnAncho, btnAlto))
        estado->opcionSeleccionada = 2;

    int salirW = 500;
    int salirH = 200;
    int margen = 30;
    if (verificarColisionBoton(x, y, ancho - salirW - margen, alto - salirH - margen, salirW, salirH))
        estado->opcionSeleccionada = 3;
}

void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado)
{
    actualizarPuntoMenu(estado, x, y, hwnd);
    if (estado->opcionSeleccionada != -1)
    {
        switch (estado->opcionSeleccionada)
        {
        case 0:
            estado->mostrarMenu = 0;
            estado->enPartida = 1;
            break;
        case 1:
            MessageBox(hwnd, "Guardado no disponible aun.", "Partidas", MB_OK);
            break;
        case 2:
            MessageBox(hwnd, "WASD para moverte.", "Ayuda", MB_OK);
            break;
        case 3:
            PostQuitMessage(0);
            return;
        }
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void procesarEnterMenu(HWND hwnd, EstadoJuego *estado)
{
    if (estado->mostrarMenu)
    {
        estado->mostrarMenu = 0;
        estado->enPartida = 1;
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

// --- INTERACCIÓN MOCHILA (Comida) ---
void procesarClickMochila(int mouseX, int mouseY, Jugador *jugador, HWND hwnd)
{
    if (!jugador->inventarioAbierto)
        return;

    int px = 90, py = 120;    // Posición del panel de mochila
    int iy_comida = py + 240; // Altura aproximada del icono comida

    if (mouseX >= px + 10 && mouseX <= px + 200 && mouseY >= iy_comida && mouseY <= iy_comida + 32)
    {
        if (jugador->comida > 0 && jugador->vidaActual < jugador->vidaMax)
        {
            jugador->vidaActual += 25;
            jugador->comida--;
            if (jugador->vidaActual > jugador->vidaMax)
                jugador->vidaActual = jugador->vidaMax;
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }
}

// --- INTERACCIÓN TIENDA (Compra) ---
void procesarClickMochilaTienda(int mx, int my, Jugador *j, HWND hwnd)
{
    if (!j->inventarioAbierto)
        return;

    int tx = 450, ty = 120; // Posición de la tienda

    // COMPRAR ARMADURA (Clic en opción 3)
    if (mx >= tx && mx <= tx + 300 && my >= ty + 160 && my <= ty + 190)
    {
        if (j->hierro >= 5 && j->oro >= 50 && !j->tieneArmadura)
        {
            j->hierro -= 5;
            j->oro -= 50;
            j->tieneArmadura = 1;
            j->frameDestello = 15;
            PlaySound(TEXT("assets/sonidos/miau.wav"), NULL, SND_FILENAME | SND_ASYNC);
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }

    // EQUIPAR (Clic en el icono de armadura dentro de la mochila)
    // Coordenadas aproximadas en la mochila
    int px = 90, py = 120;
    if (mx >= px && mx <= px + 200 && my >= py + 200 && my <= py + 240)
    { // Ajustar según posición real
        if (j->tieneArmadura)
        {
            j->armaduraEquipada = !j->armaduraEquipada;
            j->frameDestello = 8;
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }
}

// --- DIBUJADO DE TIENDAS ---
void dibujarTiendasEnIslas(HDC hdc, Camera cam, int ancho, int alto, int frameTienda)
{
    // Coordenadas FIJAS en el mundo (Isla Central, esquina inferior izquierda)
    int tiendaMundoX = 1450;
    int tiendaMundoY = 1900;

    // Conversión a Pantalla (Igual que las islas: Mundo - Camara * Zoom)
    int tx = (tiendaMundoX - cam.x) * cam.zoom;
    int ty = (tiendaMundoY - cam.y) * cam.zoom;

    // Tamaño que escala con el zoom
    int tam = 50 * cam.zoom;

    // Animación
    int f = (frameTienda / 20) % 2;

    // Solo dibujamos si la tienda está dentro de la pantalla (Optimización básica)
    if (tx + tam >= 0 && tx - tam <= ancho && ty + tam >= 0 && ty - tam <= alto)
    {
        if (hBmpTienda[f] != NULL)
        {
            // Dibujamos centrada en la coordenada (tx, ty)
            DibujarImagen(hdc, hBmpTienda[f], tx - (tam / 2), ty - (tam / 2), tam, tam);

            // (Opcional) Texto pequeño para identificarla si quieres debug
            // SetTextColor(hdc, RGB(255, 255, 0)); TextOut(hdc, tx, ty, "TIENDA", 6);
        }
    }
}

void dibujarTiendaInteractiva(HDC hdc, Jugador *j)
{
    int tx = 450, ty = 120;

    // 1. Fondo Negro
    SelectObject(hdc, GetStockObject(BLACK_BRUSH));
    Rectangle(hdc, tx, ty, tx + 300, ty + 350);

    // 2. Configurar Texto Transparente (IMPORTANTE PARA QUE SE VEA BIEN)
    SetBkMode(hdc, TRANSPARENT);

    // 3. Título Dorado
    SetTextColor(hdc, RGB(255, 215, 0));
    TextOut(hdc, tx + 80, ty + 10, "--- MERCADER ---", 16);

    // 4. Opciones (Blanco)
    char b[100];
    SetTextColor(hdc, RGB(255, 255, 255));

    sprintf(b, "1. Espada: 3 Hierro + 20 Oro %s", j->tieneEspada ? "[OK]" : "");
    TextOut(hdc, tx + 20, ty + 60, b, strlen(b));

    sprintf(b, "2. Pico: 5 Piedra + 15 Oro %s", j->tienePico ? "[OK]" : "");
    TextOut(hdc, tx + 20, ty + 110, b, strlen(b));

    sprintf(b, "3. Armadura: 5 Hierro + 50 Oro %s", j->tieneArmadura ? "[OK]" : "");
    TextOut(hdc, tx + 20, ty + 160, b, strlen(b));
}

// --- DIBUJADO DE JUGADOR ---
void dibujarJugador(HDC hdc, Jugador jugador, Camera cam)
{
    RECT r;
    GetClipBox(hdc, &r);
    int tam = 32 * cam.zoom;
    int cx = (r.right / 2) - (tam / 2);
    int cy = (r.bottom / 2) - (tam / 2);

    // 1. Cuerpo
    HBITMAP sprite = hBmpJugadorAnim[jugador.direccion][jugador.frameAnim];
    DibujarImagen(hdc, (sprite ? sprite : hBmpJugador), cx, cy, tam, tam);

    // 2. Armadura (Usa la variable externa de recursos.h)
    if (jugador.armaduraEquipada)
    {
        HBITMAP sArma = hBmpArmaduraAnim[jugador.direccion][jugador.frameAnim];
        if (sArma)
            DibujarImagen(hdc, sArma, cx, cy, tam, tam);
    }

    // 3. Destello
    if (jugador.frameDestello > 0)
    {
        HBRUSH luz = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH old = (HBRUSH)SelectObject(hdc, luz);
        Ellipse(hdc, cx - 10, cy - 10, cx + tam + 10, cy + tam + 10);
        SelectObject(hdc, old);
        DeleteObject(luz);
    }
}

// --- DIBUJADO PRINCIPAL (MAPA + HUD) ---
void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera cam, int ancho, int alto, int frameTienda)
{
    HBRUSH agua = CreateSolidBrush(RGB(0, 100, 180));
    RECT r = {0, 0, ancho, alto};
    FillRect(hdc, &r, agua);
    DeleteObject(agua);

    // Dibujar Islas
    for (int i = 0; i < MAX_ISLAS; i++)
    {
        if (!misIslas[i].activa)
            continue;
        int sx = (misIslas[i].x - cam.x) * cam.zoom;
        int sy = (misIslas[i].y - cam.y) * cam.zoom;
        int sw = misIslas[i].ancho * cam.zoom;
        int sh = misIslas[i].alto * cam.zoom;
        HBITMAP img = (i == 0) ? hBmpIslaCentral : hBmpIslaNoreste;
        if (img)
            DibujarImagen(hdc, img, sx, sy, sw, sh);
    }

    // Dibujar Tiendas
    dibujarTiendasEnIslas(hdc, cam, ancho, alto, frameTienda);
}

void dibujarItemRejilla(HDC hdc, HBITMAP icono, int cantidad, int maximo, int x, int y, const char *nombre)
{
    // 1. Dibujar Icono
    if (icono)
        DibujarImagen(hdc, icono, x, y, 32, 32);

    // 2. Texto del Nombre (Gris Claro)
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(200, 200, 200));
    TextOut(hdc, x + 40, y - 5, nombre, strlen(nombre));

    // 3. Texto de Cantidad (Blanco o Rojo si está lleno)
    char buf[32];
    if (maximo > 0)
    {
        // Modo Material: "12 / 50"
        sprintf(buf, "%d / %d", cantidad, maximo);
        if (cantidad >= maximo)
            SetTextColor(hdc, RGB(255, 100, 100)); // Rojo si lleno
        else
            SetTextColor(hdc, RGB(255, 255, 255)); // Blanco normal
    }
    else
    {
        // Modo Herramienta: "Equipado" o "En Bolsa"
        sprintf(buf, "%s", cantidad ? "Equipado" : "En Bolsa");
        SetTextColor(hdc, RGB(255, 255, 0)); // Dorado
    }
    TextOut(hdc, x + 40, y + 12, buf, strlen(buf));
}

void dibujarHUD(HDC hdc, Jugador *jugador, int ancho, int alto)
{
    // 1. VIDA (Código original)
    for (int i = 0; i < 5; i++)
    {
        int vida = jugador->vidaActual - (i * 20);
        HBITMAP cor = hBmpCorazon0;
        if (vida >= 20)
            cor = hBmpCorazon100;
        else if (vida >= 15)
            cor = hBmpCorazon75;
        else if (vida >= 10)
            cor = hBmpCorazon50;
        else if (vida >= 5)
            cor = hBmpCorazon25;
        DibujarImagen(hdc, cor, 20 + (i * 32), 20, 32, 32);
    }

    // 2. ARMADURA (Código original)
    HBITMAP bmpEscudo = hBmpEscudo0;
    float pEscudo = (jugador->armaduraMax > 0) ? (float)jugador->armaduraActual / jugador->armaduraMax : 0;
    if (pEscudo >= 0.8)
        bmpEscudo = hBmpEscudo100;
    else if (pEscudo >= 0.6)
        bmpEscudo = hBmpEscudo75;
    else if (pEscudo >= 0.4)
        bmpEscudo = hBmpEscudo50;
    else if (pEscudo > 0)
        bmpEscudo = hBmpEscudo25;
    if (bmpEscudo)
        DibujarImagen(hdc, bmpEscudo, 20, 60, 48, 48);

    char tEscudo[10];
    sprintf(tEscudo, "%d", jugador->armaduraActual);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    TextOut(hdc, 75, 75, tEscudo, strlen(tEscudo));

    // 3. ICONO DE MOCHILA
    HBITMAP bmpBolso = (jugador->inventarioAbierto) ? hBmpInvAbierto : hBmpInvCerrado;
    if (bmpBolso)
        DibujarImagen(hdc, bmpBolso, 20, 120, 64, 64);

    // --- 4. PANEL DE INVENTARIO MEJORADO (REJILLA) ---
    if (jugador->inventarioAbierto)
    {

        // A. Calcular Capacidad Máxima
        int maxCapacidad = 50; // Nivel 1
        if (jugador->nivelMochila == 2)
            maxCapacidad = 150;
        if (jugador->nivelMochila == 3)
            maxCapacidad = 300;

        // B. Definir cuántos huecos mostramos
        // Nivel 1: 6 items (Materiales)
        // Nivel 2: 8 items (+ Herramientas)
        // Nivel 3: 9 items (+ Armadura)
        int totalItems = 6;
        if (jugador->nivelMochila >= 2)
            totalItems = 8;
        if (jugador->nivelMochila >= 3)
            totalItems = 9;

        // C. Calcular Altura del Fondo (Adaptable)
        int filas = (totalItems + 1) / 2;    // División entera redondeada hacia arriba
        int alturaFondo = (filas * 50) + 60; // 50px por fila + margen

        int px = 90, py = 120;
        int anchoFondo = 320;

        // Dibujar Fondo Gris Oscuro
        HBRUSH fondo = CreateSolidBrush(RGB(30, 30, 35));
        RECT r = {px, py, px + anchoFondo, py + alturaFondo};
        FillRect(hdc, &r, fondo);
        DeleteObject(fondo);

        // Borde Blanco Fino
        HBRUSH borde = CreateSolidBrush(RGB(255, 255, 255));
        FrameRect(hdc, &r, borde);
        DeleteObject(borde);

        // Título
        SetTextColor(hdc, RGB(255, 215, 0)); // Dorado
        char titulo[50];
        sprintf(titulo, "MOCHILA (NVL %d)", jugador->nivelMochila);
        TextOut(hdc, px + 100, py + 10, titulo, strlen(titulo));

        // D. Dibujar la Rejilla
        int startX = px + 20;
        int startY = py + 40;

        for (int i = 0; i < totalItems; i++)
        {
            int col = i % 2;
            int row = i / 2;
            int itemX = startX + (col * 150);
            int itemY = startY + (row * 50);

            switch (i)
            {
            case 0:
                dibujarItemRejilla(hdc, hBmpIconoMadera, jugador->madera, maxCapacidad, itemX, itemY, "Madera");
                break;
            case 1:
                dibujarItemRejilla(hdc, hBmpIconoPiedra, jugador->piedra, maxCapacidad, itemX, itemY, "Piedra");
                break;
            case 2:
                dibujarItemRejilla(hdc, hBmpIconoOro, jugador->oro, maxCapacidad, itemX, itemY, "Oro");
                break;
            case 3:
                dibujarItemRejilla(hdc, hBmpIconoHierro, jugador->hierro, maxCapacidad, itemX, itemY, "Hierro");
                break;
            // NOTA: Aquí uso hBmpIconoHoja (singular) para coincidir con recursos.h
            case 4:
                dibujarItemRejilla(hdc, hBmpIconoHoja, jugador->hojas, maxCapacidad, itemX, itemY, "Hojas");
                break;
            case 5:
                dibujarItemRejilla(hdc, hBmpIconoComida, jugador->comida, maxCapacidad, itemX, itemY, "Comida");
                break;

            // Herramientas (Nivel 2+)
            case 6:
                if (jugador->tieneEspada)
                    dibujarItemRejilla(hdc, hBmpIconoEspada, 1, 0, itemX, itemY, "Espada");
                break;
            case 7:
                if (jugador->tienePico)
                    dibujarItemRejilla(hdc, hBmpIconoPico, 1, 0, itemX, itemY, "Pico");
                break;

            // Armadura (Nivel 3+)
            case 8:
                if (jugador->tieneArmadura)
                    dibujarItemRejilla(hdc, hBmpArmaduraAnim[0][0], 1, 0, itemX, itemY, "Armadura");
                break;
            }
        }
    }

    // 5. BARRA DE EXPERIENCIA (Código original)
    if (hBmpBarraXPVacia && hBmpBarraXPLlena)
    {
        BITMAP bm;
        GetObject(hBmpBarraXPVacia, sizeof(BITMAP), &bm);
        int xpW = bm.bmWidth;
        int xpH = bm.bmHeight;
        int xpX = (ancho - xpW) / 2;
        int xpY = alto - xpH - 20;
        DibujarImagen(hdc, hBmpBarraXPVacia, xpX, xpY, xpW, xpH);
        float pct = 0;
        if (jugador->experienciaSiguienteNivel > 0)
            pct = (float)jugador->experiencia / jugador->experienciaSiguienteNivel;
        if (pct > 1)
            pct = 1;
        int fillW = (int)(xpW * pct);
        if (fillW > 0)
        {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmpBarraXPLlena);
            TransparentBlt(hdc, xpX, xpY, fillW, xpH, hdcMem, 0, 0, fillW, xpH, RGB(255, 0, 255));
            SelectObject(hdcMem, hOld);
            DeleteDC(hdcMem);
        }
        char tNivel[32];
        sprintf(tNivel, "NVL %d", jugador->nivel);
        SetTextColor(hdc, RGB(0, 255, 255));
        TextOut(hdc, xpX - 60, xpY + 5, tNivel, strlen(tNivel));
    }

    // 6. DETECCIÓN DE TIENDA (Código con coordenadas fijas de la Isla Central)
    BOOL cercaDeTienda = FALSE;
    int tiendaX = 1450;
    int tiendaY = 1900;
    float dist = sqrt(pow(jugador->x - tiendaX, 2) + pow(jugador->y - tiendaY, 2));

    if (dist < 80)
        cercaDeTienda = TRUE; // Radio de interacción ajustado

    if (jugador->inventarioAbierto && cercaDeTienda)
    {
        dibujarTiendaInteractiva(hdc, jugador);
    }
}