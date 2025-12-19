/* src/mapa/mapa.c (Versión GDI con Cámara) */
#include "mapa.h"
#include "recursos/recursos.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_ISLAS 5

Isla misIslas[MAX_ISLAS];

/* --- Variables globales --- */
static EstadoJuego estadoActual = {1, 0, 0, 0, {0, 0}};

void inicializarIslas()
{
    // Limpiamos el array (desactivamos todas)
    for (int i = 0; i < MAX_ISLAS; i++)
        misIslas[i].activa = 0;

    // --- ISLA 1 (LA PRINCIPAL) ---
    misIslas[0].activa = 1;
    misIslas[0].x = 1350; // Donde empieza la imagen
    misIslas[0].y = 1350;
    misIslas[0].ancho = 500;
    misIslas[0].alto = 500;
    misIslas[0].margen = 60; // El borde de arena/agua que calculamos

    // --- ISLA 2 (FUTURA) ---
    // misIslas[1].activa = 1;
    // misIslas[1].x = 2000; ... ¡Así de fácil será añadirla!
}

void inicializarJuego(Jugador* jugador, EstadoJuego* estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    
    inicializarIslas();

    // 1. LIMPIEZA
    inicializarMapa(mapa); // Pone todo en 0

    // 2. POSICIÓN DEL JUGADOR (COORDENADA 1600)
    jugador->x = 1600;
    jugador->y = 1600;

    // 3. STATS
    jugador->velocidad = 5;
    jugador->vidaActual = 100;
    jugador->vidaMax = 100;
    
    // 4. ESTADO
    estado->enPartida = 0; // OJO: Si pones 1, arranca sin menú. Si pones 0, sale el menú primero.
    estado->mostrarMenu = 1; 
}

void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS])
{
    int i, j;

    // Simplemente limpiamos la matriz para que esté lista para usar en el futuro
    // Ya no ponemos '~' ni '.' porque la colisión la maneja EsSuelo()
    for (i = 0; i < MUNDO_FILAS; i++)
    {
        for (j = 0; j < MUNDO_COLUMNAS; j++)
        {
            mapa[i][j] = 0; // 0 significa "vacío" o "nada especial aquí"
        }
    }

    printf("Matriz de mapa limpiada (La isla visual la maneja el sistema de Sprites)\n");
}

int EsSuelo(int x, int y)
{
    // Recorremos todas las islas buscando si estamos pisando alguna
    for (int i = 0; i < MAX_ISLAS; i++)
    {
        if (!misIslas[i].activa)
            continue; // Si no está activa, la saltamos

        Isla is = misIslas[i];

        // Definimos la CAJA DE COLISIÓN (Hitbox) restando el margen
        int minX = is.x + is.margen;
        int maxX = is.x + is.ancho - is.margen;
        int minY = is.y + is.margen;
        int maxY = is.y + is.alto - is.margen;

        // ¿El punto (x,y) está dentro de esta caja?
        if (x >= minX && x <= maxX && y >= minY && y <= maxY)
        {
            return 1; // ¡ENCONTRADO! Estás pisando la isla 'i'
        }
    }

    return 0; // Revisé todas las islas y no estás en ninguna -> AGUA
}

void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy)
{
    int futuraX = jugador->x + (dx * jugador->velocidad);
    int futuraY = jugador->y + (dy * jugador->velocidad);

    // Solo le preguntamos al Gestor de Islas
    if (EsSuelo(futuraX, futuraY))
    {
        jugador->x = futuraX;
        jugador->y = futuraY;
    }
}

void actualizarCamara(Camera *camara, Jugador jugador)
{
    // 1. Obtener dimensiones de la pantalla
    int anchoPantalla = GetSystemMetrics(SM_CXSCREEN);
    int altoPantalla = GetSystemMetrics(SM_CYSCREEN);

    // 2. Calcular el centro relativo al zoom
    int centroX = (anchoPantalla / 2) / camara->zoom;
    int centroY = (altoPantalla / 2) / camara->zoom;

    // 3. Posicionar cámara
    camara->x = jugador.x - centroX;
    camara->y = jugador.y - centroY;

    // 4. LÍMITES DEL MUNDO (Ahora sí usamos la variable)
    int limiteMundoX = MUNDO_COLUMNAS * TAMANO_CELDA_BASE; // 3200
    int limiteMundoY = MUNDO_FILAS * TAMANO_CELDA_BASE;    // 3200

    // Ancho visible del mundo en píxeles
    int mundoVisibleX = anchoPantalla / camara->zoom;
    int mundoVisibleY = altoPantalla / camara->zoom;

    // --- CLAMPING (Limitar movimiento) ---

    // No salir por la izquierda/arriba
    if (camara->x < 0) camara->x = 0;
    if (camara->y < 0) camara->y = 0;

    // No salir por la derecha
    if (camara->x > limiteMundoX - mundoVisibleX) {
        camara->x = limiteMundoX - mundoVisibleX;
        if (camara->x < 0) camara->x = 0; // Por si la pantalla es más grande que el mundo
    }

    // No salir por abajo
    if (camara->y > limiteMundoY - mundoVisibleY) {
        camara->y = limiteMundoY - mundoVisibleY;
        if (camara->y < 0) camara->y = 0;
    }
}

/* --- Sistema de Menú Mejorado --- */

void dibujarBoton(HDC hdc, int x, int y, int ancho, int alto, const char *texto, BOOL seleccionado, BOOL activo)
{
    COLORREF colorFondo, colorTexto, colorBorde, colorSombra;

    if (seleccionado)
    {
        colorFondo = RGB(100, 180, 255);
        colorTexto = RGB(255, 255, 255);
        colorBorde = RGB(255, 215, 0);
        colorSombra = RGB(30, 100, 180);
    }
    else if (activo)
    {
        colorFondo = RGB(70, 130, 180);
        colorTexto = RGB(255, 255, 255);
        colorBorde = RGB(50, 100, 150);
        colorSombra = RGB(20, 60, 100);
    }
    else
    {
        colorFondo = RGB(100, 100, 120);
        colorTexto = RGB(200, 200, 200);
        colorBorde = RGB(80, 80, 100);
        colorSombra = RGB(50, 50, 70);
    }

    // Dibujar sombra
    HBRUSH hBrushSombra = CreateSolidBrush(colorSombra);
    RECT rectSombra = {x + 3, y + 3, x + ancho + 3, y + alto + 3};
    FillRect(hdc, &rectSombra, hBrushSombra);
    DeleteObject(hBrushSombra);

    // Dibujar borde
    HBRUSH hBrushBorde = CreateSolidBrush(colorBorde);
    RECT rectBorde = {x - 2, y - 2, x + ancho + 2, y + alto + 2};
    FillRect(hdc, &rectBorde, hBrushBorde);
    DeleteObject(hBrushBorde);

    // Dibujar fondo principal
    HBRUSH hBrushFondo = CreateSolidBrush(colorFondo);
    RECT rectBoton = {x, y, x + ancho, y + alto};
    FillRect(hdc, &rectBoton, hBrushFondo);

    // Efecto de gradiente (simulado)
    if (seleccionado || activo)
    {
        HBRUSH hBrushGradiente = CreateSolidBrush(RGB(120, 180, 255));
        RECT rectGradiente = {x, y, x + ancho, y + alto / 3};
        FillRect(hdc, &rectGradiente, hBrushGradiente);
        DeleteObject(hBrushGradiente);
    }

    // Configurar texto
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, colorTexto);

    // Fuente del botón
    HFONT hFont = CreateFont(
        seleccionado ? 24 : 20,
        0, 0, 0,
        seleccionado ? FW_BOLD : FW_SEMIBOLD,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        VARIABLE_PITCH,
        TEXT("Arial"));

    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    // Dibujar texto usando caracteres UNICODE correctos
    if (strcmp(texto, "INICIAR PARTIDA") == 0)
    {
        DrawText(hdc, TEXT("START GAME"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    else if (strcmp(texto, "RESUMEN RECURSOS") == 0)
    {
        DrawText(hdc, TEXT("RESOURCES"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    else if (strcmp(texto, "SALIR") == 0)
    {
        DrawText(hdc, TEXT("EXIT"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    else if (strcmp(texto, "VOLVER AL MENÚ") == 0)
    {
        DrawText(hdc, TEXT("BACK TO MENU"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    else
    {
        DrawTextA(hdc, texto, -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    // Restaurar y limpiar
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    DeleteObject(hBrushFondo);
}

void dibujarFondoAnimado(HDC hdc, int ancho, int alto, int tiempo)
{
    // Fondo con gradiente azul marino
    for (int y = 0; y < alto; y++)
    {
        int azul = 25 + (y * 30) / alto;
        HBRUSH hBrushLinea = CreateSolidBrush(RGB(10, 10, azul));
        RECT rectLinea = {0, y, ancho, y + 1};
        FillRect(hdc, &rectLinea, hBrushLinea);
        DeleteObject(hBrushLinea);
    }

    // Estrellas parpadeantes
    srand(tiempo / 10);
    for (int i = 0; i < 80; i++)
    {
        int x = rand() % ancho;
        int y = rand() % alto;
        int brillo = 100 + (rand() % 155);
        int tamano = 1 + (rand() % 2);

        HBRUSH hBrushEstrella = CreateSolidBrush(RGB(brillo, brillo, brillo));
        RECT rectEstrella = {x, y, x + tamano, y + tamano};
        FillRect(hdc, &rectEstrella, hBrushEstrella);
        DeleteObject(hBrushEstrella);
    }
}

void dibujarMenu(HDC hdc, HWND hwnd, EstadoJuego *estado)
{
    RECT rectClient;
    GetClientRect(hwnd, &rectClient);
    int ancho = rectClient.right - rectClient.left;
    int alto = rectClient.bottom - rectClient.top;

    static int tiempo = 0;
    tiempo++;

    // Fondo animado
    dibujarFondoAnimado(hdc, ancho, alto, tiempo);

    // Título principal con efecto
    HFONT hFontTitulo = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                   DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                   CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                   VARIABLE_PITCH, TEXT("Arial"));
    HFONT hOldFont = SelectObject(hdc, hFontTitulo);

    // Sombra del título
    SetTextColor(hdc, RGB(0, 0, 60));
    RECT rectTituloSombra = {ancho / 2 - 150 + 2, 52, ancho / 2 + 150 + 2, 132};
    DrawText(hdc, TEXT("WAR ISLANDS"), -1, &rectTituloSombra, DT_CENTER | DT_SINGLELINE);

    // Título principal
    SetTextColor(hdc, RGB(255, 215, 0));
    SetBkMode(hdc, TRANSPARENT);
    RECT rectTitulo = {ancho / 2 - 150, 50, ancho / 2 + 150, 130};
    DrawText(hdc, TEXT("WAR ISLANDS"), -1, &rectTitulo, DT_CENTER | DT_SINGLELINE);

    // Subtítulo
    HFONT hFontSubtitulo = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                      CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                      VARIABLE_PITCH, TEXT("Arial"));
    SelectObject(hdc, hFontSubtitulo);
    SetTextColor(hdc, RGB(180, 220, 255));

    RECT rectSubtitulo = {0, 130, ancho, 170};
    DrawText(hdc, TEXT("Conquer - Build - Survive"), -1, &rectSubtitulo, DT_CENTER | DT_SINGLELINE);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFontTitulo);
    DeleteObject(hFontSubtitulo);

    // Botones interactivos
    int centroX = ancho / 2;
    int anchoBoton = 300;
    int altoBoton = 60;
    int espacioBoton = 20;

    int yBase = 220;

    // Determinar qué botón está seleccionado
    int btnStartSel = (estado->opcionSeleccionada == 0);
    int btnResourcesSel = (estado->opcionSeleccionada == 1);
    int btnExitSel = (estado->opcionSeleccionada == 2);

    dibujarBoton(hdc, centroX - anchoBoton / 2, yBase, anchoBoton, altoBoton,
                 "INICIAR PARTIDA", btnStartSel, TRUE);
    dibujarBoton(hdc, centroX - anchoBoton / 2, yBase + altoBoton + espacioBoton, anchoBoton, altoBoton,
                 "RESUMEN RECURSOS", btnResourcesSel, TRUE);
    dibujarBoton(hdc, centroX - anchoBoton / 2, yBase + (altoBoton + espacioBoton) * 2, anchoBoton, altoBoton,
                 "SALIR", btnExitSel, TRUE);

    // Instrucciones
    HFONT hFontInstrucciones = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                          DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                          CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                          VARIABLE_PITCH, TEXT("Arial"));
    SelectObject(hdc, hFontInstrucciones);
    SetTextColor(hdc, RGB(150, 200, 255));

    RECT rectInstrucciones = {0, alto - 60, ancho, alto - 30};
    DrawText(hdc, TEXT("Use UP/DOWN arrows to navigate and ENTER to select"), -1, &rectInstrucciones,
             DT_CENTER | DT_SINGLELINE);

    // Créditos
    RECT rectCreditos = {0, alto - 30, ancho, alto};
    DrawText(hdc, TEXT("War Islands v1.0 - Developed with C - 2024"), -1, &rectCreditos,
             DT_CENTER | DT_SINGLELINE);

    DeleteObject(hFontInstrucciones);
}

int verificarColisionBoton(int mouseX, int mouseY, int btnX, int btnY, int btnAncho, int btnAlto)
{
    return (mouseX >= btnX && mouseX <= btnX + btnAncho &&
            mouseY >= btnY && mouseY <= btnY + btnAlto);
}

void actualizarPuntoMenu(EstadoJuego *estado, int x, int y, HWND hwnd)
{
    estado->puntoMouse.x = x;
    estado->puntoMouse.y = y;

    RECT rectClient;
    GetClientRect(hwnd, &rectClient);
    int ancho = rectClient.right;

    if (estado->mostrarMenu)
    {
        int centroX = ancho / 2;
        int anchoBoton = 300;
        int altoBoton = 70;
        int espacioBoton = 30;
        int yBase = 220;

        estado->opcionSeleccionada = -1;

        if (verificarColisionBoton(x, y, centroX - anchoBoton / 2, yBase, anchoBoton, altoBoton))
        {
            estado->opcionSeleccionada = 0;
        }
        else if (verificarColisionBoton(x, y, centroX - anchoBoton / 2, yBase + altoBoton + espacioBoton, anchoBoton, altoBoton))
        {
            estado->opcionSeleccionada = 1;
        }
        else if (verificarColisionBoton(x, y, centroX - anchoBoton / 2, yBase + (altoBoton + espacioBoton) * 2, anchoBoton, altoBoton))
        {
            estado->opcionSeleccionada = 2;
        }
    }
    else if (estado->mostrarResumen)
    {
        int btnVolverX = ancho / 2 - 100;
        int btnVolverY = 120 + 300 + 20;

        estado->opcionSeleccionada = -1;
        if (verificarColisionBoton(x, y, btnVolverX, btnVolverY, 200, 50))
        {
            estado->opcionSeleccionada = 0;
        }
    }
}

void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado)
{
    actualizarPuntoMenu(estado, x, y, hwnd);

    if (estado->opcionSeleccionada != -1)
    {
        if (estado->mostrarMenu)
        {
            switch (estado->opcionSeleccionada)
            {
            case 0:
                estado->mostrarMenu = 0;
                estado->enPartida = 1;
                estado->mostrarResumen = 0;
                break;
            case 1:
                estado->mostrarMenu = 0;
                estado->enPartida = 0;
                estado->mostrarResumen = 1;
                break;
            case 2:
                PostQuitMessage(0);
                return;
            }
        }
        else if (estado->mostrarResumen)
        {
            if (estado->opcionSeleccionada == 0)
            {
                estado->mostrarMenu = 1;
                estado->enPartida = 0;
                estado->mostrarResumen = 0;
            }
        }

        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void procesarEnterMenu(HWND hwnd, EstadoJuego *estado)
{
    if (estado->opcionSeleccionada != -1)
    {
        if (estado->mostrarMenu)
        {
            switch (estado->opcionSeleccionada)
            {
            case 0:
                estado->mostrarMenu = 0;
                estado->enPartida = 1;
                estado->mostrarResumen = 0;
                break;
            case 1:
                estado->mostrarMenu = 0;
                estado->enPartida = 0;
                estado->mostrarResumen = 1;
                break;
            case 2:
                PostQuitMessage(0);
                return;
            }
        }

        InvalidateRect(hwnd, NULL, TRUE);
    }
}

EstadoJuego *obtenerEstadoJuego()
{
    return &estadoActual;
}

void iniciarPartida()
{
    estadoActual.mostrarMenu = 0;
    estadoActual.enPartida = 1;
    estadoActual.mostrarResumen = 0;
}

void volverAlMenu()
{
    estadoActual.mostrarMenu = 1;
    estadoActual.enPartida = 0;
    estadoActual.mostrarResumen = 0;
}

/* ========== FUNCIONES CON SPRITES ========== */

void dibujarMenuConSprites(HDC hdc, HWND hwnd, EstadoJuego *estado)
{
    // 1. OBTENER TAMAÑO ACTUAL DE LA VENTANA
    RECT rect;
    GetClientRect(hwnd, &rect);
    int anchoVentana = rect.right;
    int altoVentana = rect.bottom;

    // 2. DIBUJAR FONDO ESTIRADO (SCALING)
    // Usamos StretchBlt directamente aquí para asegurar que llene todo sin transparencia
    if (hBmpFondoMenu != NULL)
    {
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmpFondoMenu);

        // Obtenemos tamaño real de la imagen
        BITMAP bm;
        GetObject(hBmpFondoMenu, sizeof(BITMAP), &bm);

        // ESTIRAMOS la imagen desde (0,0) hasta (anchoVentana, altoVentana)
        SetStretchBltMode(hdc, HALFTONE); // Para que no se vea pixelada al estirar
        StretchBlt(hdc, 0, 0, anchoVentana, altoVentana,
                   hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

        SelectObject(hdcMem, hOld);
        DeleteDC(hdcMem);
    }
    else
    {
        // Si falló la carga, fondo azul de emergencia
        HBRUSH brochaAzul = CreateSolidBrush(RGB(0, 0, 50));
        FillRect(hdc, &rect, brochaAzul);
        DeleteObject(brochaAzul);
    }

    // 3. DIBUJAR BOTONES (Calculados proporcionalmente al centro)
    int btnAncho = 300;
    int btnAlto = 60;

    // Centrar horizontalmente
    int btnX = (anchoVentana - btnAncho) / 2;

    // Empezar un poco más abajo del centro vertical
    int startY = (altoVentana / 2) - 50;

    for (int i = 0; i < 3; i++)
    {
        int btnY = startY + (i * 80); // 80 pixeles de separación

        // ¿Qué imagen usamos?
        HBITMAP imgBtn = (i == estado->opcionSeleccionada) ? hBmpBotonSel : hBmpBoton;

        if (imgBtn != NULL)
        {
            // Usamos tu función auxiliar para los botones (mantienen transparencia si la tienen)
            DibujarImagen(hdc, imgBtn, btnX, btnY, btnAncho, btnAlto);
        }
        else
        {
            // Si no hay imagen de botón, dibujamos rectángulos de colores
            HBRUSH brocha;
            if (i == estado->opcionSeleccionada)
                brocha = CreateSolidBrush(RGB(255, 215, 0)); // Dorado seleccionado
            else
                brocha = CreateSolidBrush(RGB(192, 192, 192)); // Gris normal

            RECT rBtn = {btnX, btnY, btnX + btnAncho, btnY + btnAlto};
            FillRect(hdc, &rBtn, brocha);
            DeleteObject(brocha);
        }
    }
}

/* ========== DIBUJAR MAPA CON ZOOM (ÚNICA FUNCIÓN DE DIBUJADO) ========== */
void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera cam, int anchoVentana, int altoVentana) {
    // 1. Fondo Azul
    HBRUSH brochaAgua = CreateSolidBrush(RGB(0, 100, 180));
    RECT rectTotal = {0, 0, anchoVentana, altoVentana};
    FillRect(hdc, &rectTotal, brochaAgua);
    DeleteObject(brochaAgua);

    // 2. Usar el sistema de Islas (Array) en lugar de coordenadas fijas
    // Esto asegura que lo que ves coincida con la colisión
    for (int i = 0; i < MAX_ISLAS; i++) {
        if (!misIslas[i].activa) continue;

        // Coordenadas lógicas (1350, 1350)
        int worldX = misIslas[i].x;
        int worldY = misIslas[i].y;

        // --- FÓRMULA CORREGIDA (SIN DOBLE CENTRADO) ---
        // Simplemente: (PosiciónMundo - Cámara) * Zoom
        int screenX = (worldX - cam.x) * cam.zoom;
        int screenY = (worldY - cam.y) * cam.zoom;
        
        int wPantalla = misIslas[i].ancho * cam.zoom;
        int hPantalla = misIslas[i].alto * cam.zoom;

        if (hBmpIsla != NULL) {
            // DIBUJAR
            DibujarImagen(hdc, hBmpIsla, screenX, screenY, wPantalla, hPantalla);
        }
    }
}

/* ========== DIBUJAR JUGADOR CON ZOOM ========== */
void dibujarJugador(HDC hdc, Jugador jugador, Camera cam)
{
    // Tamaño del jugador ajustado al zoom
    int tamano = 32 * cam.zoom;

    // Calcular posición en pantalla (siempre centro si la cámara sigue al jugador)
    // Pero lo calculamos bien por si acaso
    RECT rect;
    GetClipBox(hdc, &rect); // Truco para obtener ancho pantalla
    int centroX = (rect.right / 2) - (tamano / 2);
    int centroY = (rect.bottom / 2) - (tamano / 2);

    // Usar la nueva función
    DibujarImagen(hdc, hBmpJugador, centroX, centroY, tamano, tamano);
}

/* ========== FUNCIONES ADICIONALES (opcionales) ========== */

void dibujarFondoAnimadoMejorado(HDC hdc, int ancho, int alto, int tiempo)
{
    // Gradiente azul marino más profundo
    for (int y = 0; y < alto; y++)
    {
        int azul = 30 + (y * 40) / alto;
        int verde = 10 + (y * 10) / alto;
        HBRUSH hBrushLinea = CreateSolidBrush(RGB(5, verde, azul));
        RECT rectLinea = {0, y, ancho, y + 1};
        FillRect(hdc, &rectLinea, hBrushLinea);
        DeleteObject(hBrushLinea);
    }

    // Estrellas más dinámicas
    srand(tiempo / 8);
    for (int i = 0; i < 120; i++)
    {
        int x = rand() % ancho;
        int y = rand() % alto;
        int brillo = 80 + (rand() % 175);
        int tamano = 1 + (rand() % 3);
        int parpadeo = (tiempo + i) % 100;

        if (parpadeo < 80)
        { // Efecto de parpadeo
            HBRUSH hBrushEstrella = CreateSolidBrush(RGB(brillo, brillo, brillo));
            RECT rectEstrella = {x, y, x + tamano, y + tamano};
            FillRect(hdc, &rectEstrella, hBrushEstrella);
            DeleteObject(hBrushEstrella);
        }
    }
}

void dibujarBotonMejorado(HDC hdc, int x, int y, int ancho, int alto, const char *texto, BOOL seleccionado, BOOL activo)
{
    COLORREF colorFondo, colorTexto, colorBorde, colorSombra, colorBrillo;

    if (seleccionado)
    {
        colorFondo = RGB(80, 160, 240);
        colorTexto = RGB(255, 255, 255);
        colorBorde = RGB(255, 225, 50);
        colorSombra = RGB(20, 80, 160);
        colorBrillo = RGB(120, 200, 255);
    }
    else if (activo)
    {
        colorFondo = RGB(60, 120, 200);
        colorTexto = RGB(255, 255, 255);
        colorBorde = RGB(40, 90, 150);
        colorSombra = RGB(15, 50, 100);
        colorBrillo = RGB(90, 160, 220);
    }
    else
    {
        colorFondo = RGB(80, 80, 120);
        colorTexto = RGB(200, 200, 200);
        colorBorde = RGB(60, 60, 90);
        colorSombra = RGB(40, 40, 70);
        colorBrillo = RGB(100, 100, 140);
    }

    // Dibujar sombra más suave
    HBRUSH hBrushSombra = CreateSolidBrush(colorSombra);
    RECT rectSombra = {x + 4, y + 4, x + ancho + 4, y + alto + 4};
    FillRect(hdc, &rectSombra, hBrushSombra);
    DeleteObject(hBrushSombra);

    // Dibujar borde
    HBRUSH hBrushBorde = CreateSolidBrush(colorBorde);
    RECT rectBorde = {x - 3, y - 3, x + ancho + 3, y + alto + 3};
    FillRect(hdc, &rectBorde, hBrushBorde);
    DeleteObject(hBrushBorde);

    // Dibujar fondo principal con gradiente
    HBRUSH hBrushFondo = CreateSolidBrush(colorFondo);
    RECT rectBoton = {x, y, x + ancho, y + alto};
    FillRect(hdc, &rectBoton, hBrushFondo);

    // Efecto de gradiente superior
    if (seleccionado || activo)
    {
        HBRUSH hBrushGradiente = CreateSolidBrush(colorBrillo);
        RECT rectGradiente = {x, y, x + ancho, y + alto / 4};
        FillRect(hdc, &rectGradiente, hBrushGradiente);
        DeleteObject(hBrushGradiente);
    }

    // Configurar texto
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, colorTexto);

    // Fuente del botón mejorada
    HFONT hFont = CreateFont(
        seleccionado ? 26 : 22,
        0, 0, 0,
        seleccionado ? FW_HEAVY : FW_BOLD,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        VARIABLE_PITCH,
        TEXT("Arial"));

    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    // Dibujar texto
    if (strcmp(texto, "INICIAR PARTIDA") == 0)
    {
        DrawText(hdc, TEXT("START GAME"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    else if (strcmp(texto, "RESUMEN RECURSOS") == 0)
    {
        DrawText(hdc, TEXT("RESOURCES SUMMARY"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    else if (strcmp(texto, "SALIR") == 0)
    {
        DrawText(hdc, TEXT("EXIT GAME"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    else if (strcmp(texto, "VOLVER AL MENÚ") == 0)
    {
        DrawText(hdc, TEXT("BACK TO MENU"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    else
    {
        DrawTextA(hdc, texto, -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    // Restaurar y limpiar
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    DeleteObject(hBrushFondo);
}