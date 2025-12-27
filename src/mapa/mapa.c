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
    jugador->modoTienda = 0; // Empieza en modo comprar

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
void procesarClickMochila(int mouseX, int mouseY, Jugador *jugador, HWND hwnd) {
    if (!jugador->inventarioAbierto) return;

    // Coordenadas base (Deben ser iguales a dibujarHUD)
    int px = 90, py = 120;
    int startX = px + 20;
    int startY = py + 40;

    // La COMIDA es el Ítem #5 en nuestra lista (índice 5)
    // Cálculo de posición en rejilla:
    int i = 5; 
    int col = i % 2; // 1
    int row = i / 2; // 2
    
    int itemX = startX + (col * 150); // 110 + 150 = 260
    int itemY = startY + (row * 50);  // 160 + 100 = 260
    
    // Detectar clic en el icono de comida (32x32)
    if (mouseX >= itemX && mouseX <= itemX + 32 && mouseY >= itemY && mouseY <= itemY + 32) {
        
        // Lógica de comer
        if (jugador->comida > 0 && jugador->vidaActual < jugador->vidaMax) {
            jugador->vidaActual += 25; 
            jugador->comida--;         
            
            if (jugador->vidaActual > jugador->vidaMax) {
                jugador->vidaActual = jugador->vidaMax;
            }
            
            // Feedback sonoro (Opcional, si tienes el sonido)
            // PlaySound(TEXT("assets/sonidos/comer.wav"), NULL, SND_FILENAME | SND_ASYNC);
            
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }
}

// --- INTERACCIÓN TIENDA (Compra - Venta) ---

void procesarClickMochilaTienda(int mx, int my, int esClickDerecho, Jugador *j, HWND hwnd) {
    if (!j->inventarioAbierto) return;

    int tx = 450, ty = 120; // Posición base de la tienda

    // 1. PESTAÑAS (Comprar / Vender)
    // Pestaña Comprar (x: 450 a 600, y: 90 a 120)
    if (mx >= tx && mx <= tx + 150 && my >= ty - 30 && my <= ty) {
        j->modoTienda = 0; // COMPRAR
        PlaySound(TEXT("assets/sonidos/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }
    // Pestaña Vender (x: 600 a 750, y: 90 a 120)
    if (mx >= tx + 150 && mx <= tx + 300 && my >= ty - 30 && my <= ty) {
        j->modoTienda = 1; // VENDER
        PlaySound(TEXT("assets/sonidos/click.wav"), NULL, SND_FILENAME | SND_ASYNC);
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }

    // 2. LÓGICA DE CLICKS SEGÚN MODO
    // Definimos coordenadas de items (misma rejilla que el dibujo)
    int startX = tx + 20;
    int startY = ty + 40;

    if (j->modoTienda == 0) { // --- MODO COMPRAR ---
        // Lista de cosas a comprar (5 items)
        for (int i = 0; i < 5; i++) {
            int col = i % 2; int row = i / 2;
            int ix = startX + (col * 140); int iy = startY + (row * 60);
            
            if (mx >= ix && mx <= ix + 100 && my >= iy && my <= iy + 50) {
                // Precios y Lógica
                BOOL comprado = FALSE;
                
                // 1. ESPADA (20 Oro + 3 Hierro)
                if (i == 0 && !j->tieneEspada) {
                    if (j->oro >= 20 && j->hierro >= 3) {
                        j->oro -= 20; j->hierro -= 3; j->tieneEspada = 1; comprado = TRUE;
                    }
                }
                // 2. PICO (15 Oro + 5 Piedra)
                else if (i == 1 && !j->tienePico) {
                    if (j->oro >= 15 && j->piedra >= 5) {
                        j->oro -= 15; j->piedra -= 5; j->tienePico = 1; comprado = TRUE;
                    }
                }
                // 3. ARMADURA (50 Oro + 5 Hierro)
                else if (i == 2 && !j->tieneArmadura) {
                    if (j->oro >= 50 && j->hierro >= 5) {
                        j->oro -= 50; j->hierro -= 5; j->tieneArmadura = 1; comprado = TRUE;
                    }
                }
                // 4. MOCHILA NIVEL 2 (20 Oro + 30 Hojas)
                else if (i == 3 && j->nivelMochila == 1) {
                    if (j->oro >= 20 && j->hojas >= 30) {
                        j->oro -= 20; j->hojas -= 30; j->nivelMochila = 2; comprado = TRUE;
                    }
                }
                // 5. MOCHILA NIVEL 3 (50 Oro + 10 Hierro)
                else if (i == 4 && j->nivelMochila == 2) {
                    if (j->oro >= 50 && j->hierro >= 10) {
                        j->oro -= 50; j->hierro -= 10; j->nivelMochila = 3; comprado = TRUE;
                    }
                }

                if (comprado) {
                    PlaySound(TEXT("assets/sonidos/kaching.wav"), NULL, SND_FILENAME | SND_ASYNC);
                    InvalidateRect(hwnd, NULL, FALSE);
                } else {
                    // Sonido de error o feedback visual opcional
                }
            }
        }
    } 
    else { // --- MODO VENDER ---
        // Lista de cosas vendibles (Madera, Piedra, Hierro, Hojas, Comida)
        int precios[] = {1, 2, 5, 1, 3}; // Oro por unidad
        int* stocks[] = {&j->madera, &j->piedra, &j->hierro, &j->hojas, &j->comida};
        char* nombres[] = {"Madera", "Piedra", "Hierro", "Hojas", "Comida"};

        for (int i = 0; i < 5; i++) {
            int col = i % 2; int row = i / 2;
            int ix = startX + (col * 140); int iy = startY + (row * 60);

            if (mx >= ix && mx <= ix + 100 && my >= iy && my <= iy + 50) {
                int cantidadAVender = esClickDerecho ? 10 : 1;
                int stockActual = *stocks[i];

                // Ajustar si no tengo 10
                if (cantidadAVender > stockActual) cantidadAVender = stockActual;

                if (cantidadAVender > 0) {
                    int ganancia = cantidadAVender * precios[i];
                    char mensaje[100];
                    sprintf(mensaje, "¿Vender %d %s por %d Oro?", cantidadAVender, nombres[i], ganancia);

                    // --- VENTANA DE CONFIRMACIÓN ---
                    if (MessageBox(hwnd, mensaje, "Confirmar Venta", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                        *stocks[i] -= cantidadAVender;
                        j->oro += ganancia;
                        PlaySound(TEXT("assets/sonidos/kaching.wav"), NULL, SND_FILENAME | SND_ASYNC);
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                }
            }
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

void dibujarPrecio(HDC hdc, int x, int y, const char* costo1, const char* costo2, BOOL alcanzable) {
    SetBkMode(hdc, TRANSPARENT);
    // Color: Verde si alcanza, Rojo si no
    if (alcanzable) SetTextColor(hdc, RGB(50, 255, 50)); 
    else SetTextColor(hdc, RGB(255, 50, 50)); 
    
    // Dibujar Línea 1 (Ej: "20 Oro")
    TextOut(hdc, x, y, costo1, strlen(costo1));
    // Dibujar Línea 2 justo debajo (Ej: "3 Hierro")
    TextOut(hdc, x, y + 12, costo2, strlen(costo2));
}

void dibujarTiendaInteractiva(HDC hdc, Jugador *j) {
    int tx = 450, ty = 120; // Posición base

    // 0. Detectar Mouse (Para Hover)
    POINT p; GetCursorPos(&p); 
    ScreenToClient(WindowFromDC(hdc), &p);
    int mx = p.x; int my = p.y;

    // 1. PESTAÑAS Y FONDO (Igual que antes)
    HBRUSH bComprar = CreateSolidBrush((j->modoTienda == 0) ? RGB(50,50,60) : RGB(30,30,30));
    HBRUSH bVender = CreateSolidBrush((j->modoTienda == 1) ? RGB(50,50,60) : RGB(30,30,30));
    RECT rC = {tx, ty - 30, tx + 150, ty};
    RECT rV = {tx + 150, ty - 30, tx + 300, ty};
    FillRect(hdc, &rC, bComprar); DeleteObject(bComprar);
    FillRect(hdc, &rV, bVender); DeleteObject(bVender);
    SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(255,215,0));
    TextOut(hdc, tx + 40, ty - 25, "COMPRAR", 7);
    TextOut(hdc, tx + 195, ty - 25, "VENDER", 6);

    HBRUSH fondo = CreateSolidBrush(RGB(40, 40, 45));
    RECT rMain = {tx, ty, tx + 300, ty + 350};
    FillRect(hdc, &rMain, fondo); DeleteObject(fondo);
    HBRUSH borde = CreateSolidBrush(RGB(255, 255, 255));
    FrameRect(hdc, &rMain, borde); DeleteObject(borde);

    // 3. CONTENIDO (REJILLA)
    int startX = tx + 20; int startY = ty + 40;

    if (j->modoTienda == 0) { // --- MODO COMPRAR (ACTUALIZADO) ---
        for (int i = 0; i < 5; i++) {
            int col = i % 2; int row = i / 2;
            int x = startX + (col * 140); int y = startY + (row * 60);

            HBITMAP icono = NULL;
            // Usamos dos cadenas para los costos
            char costo1[32] = ""; char costo2[32] = ""; 
            BOOL alcanzable = FALSE; BOOL comprado = FALSE;

            switch(i) {
                case 0: // Espada
                    icono = hBmpIconoEspada;
                    sprintf(costo1, "20 Oro"); sprintf(costo2, "3 Hierro");
                    if (j->tieneEspada) comprado = TRUE;
                    else if (j->oro >= 20 && j->hierro >= 3) alcanzable = TRUE;
                    break;
                case 1: // Pico
                    icono = hBmpIconoPico;
                    sprintf(costo1, "15 Oro"); sprintf(costo2, "5 Piedra");
                    if (j->tienePico) comprado = TRUE;
                    else if (j->oro >= 15 && j->piedra >= 5) alcanzable = TRUE;
                    break;
                case 2: // Armadura
                    icono = hBmpArmaduraAnim[0][0];
                    sprintf(costo1, "50 Oro"); sprintf(costo2, "5 Hierro");
                    if (j->tieneArmadura) comprado = TRUE;
                    else if (j->oro >= 50 && j->hierro >= 5) alcanzable = TRUE;
                    break;
                case 3: // Mochila 2
                    icono = hBmpInvAbierto;
                    sprintf(costo1, "20 Oro"); sprintf(costo2, "30 Hojas");
                    if (j->nivelMochila >= 2) comprado = TRUE;
                    else if (j->oro >= 20 && j->hojas >= 30) alcanzable = TRUE;
                    break;
                case 4: // Mochila 3
                    icono = hBmpInvAbierto; 
                    sprintf(costo1, "50 Oro"); sprintf(costo2, "10 Hierro");
                    if (j->nivelMochila >= 3) comprado = TRUE;
                    else if (j->nivelMochila == 2 && j->oro >= 50 && j->hierro >= 10) alcanzable = TRUE;
                    else if (j->nivelMochila < 2) {
                        sprintf(costo1, "Req. Nivel 2"); 
                        costo2[0] = '\0'; // <--- CORRECCIÓN AQUÍ (Cadena vacía segura)
                    }
                    break;
            }

            // DIBUJAR ÍTEM Y PRECIO
            if (icono) DibujarImagen(hdc, icono, x, y, 32, 32);
            if (comprado) {
                SetTextColor(hdc, RGB(100, 100, 100));
                TextOut(hdc, x + 40, y + 8, "COMPRADO", 8);
            } else {
                dibujarPrecio(hdc, x + 40, y + 8, costo1, costo2, alcanzable);
            }

            // HOVER TOOLTIP (Combina las dos líneas para la descripción abajo)
            // CORRECCIÓN AQUÍ: Aumentamos seguridad con tooltip[100]
            if (mx >= x && mx <= x + 100 && my >= y && my <= y + 40 && !comprado && costo1[0] != '\0') {
               char tooltip[100]; // <--- AUMENTADO DE 64 A 100
               
               if (costo2[0] != '\0') sprintf(tooltip, "Costo: %s - %s", costo1, costo2);
               else sprintf(tooltip, "%s", costo1); 

               SetTextColor(hdc, RGB(255, 255, 255));
               TextOut(hdc, tx + 20, ty + 320, tooltip, strlen(tooltip));
            }
        }

    } else { // --- MODO VENDER (Sin cambios, ya estaba bien) ---
        int precios[] = {1, 2, 5, 1, 3};
        char* nombres[] = {"Madera", "Piedra", "Hierro", "Hojas", "Comida"};
        HBITMAP iconos[] = {hBmpIconoMadera, hBmpIconoPiedra, hBmpIconoHierro, hBmpIconoHoja, hBmpIconoComida};
        int cantidades[] = {j->madera, j->piedra, j->hierro, j->hojas, j->comida};

        for (int i = 0; i < 5; i++) {
            int col = i % 2; int row = i / 2;
            int x = startX + (col * 140); int y = startY + (row * 60);

            if (iconos[i]) DibujarImagen(hdc, iconos[i], x, y, 32, 32);
            
            char texto[32];
            sprintf(texto, "%s (x%d)", nombres[i], cantidades[i]);
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOut(hdc, x + 40, y, texto, strlen(texto));

            char precio[32];
            sprintf(precio, "+%d Oro c/u", precios[i]);
            SetTextColor(hdc, RGB(255, 215, 0));
            TextOut(hdc, x + 40, y + 16, precio, strlen(precio));
            
            if (mx >= x && mx <= x + 100 && my >= y && my <= y + 40) {
               SetTextColor(hdc, RGB(200, 200, 200));
               TextOut(hdc, tx + 20, ty + 320, "Click: Vender 1 | Der: 10", 25);
            }
        }
    }
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