/* src/mapa/mapa.c (Versión GDI con Cámara) */
#include "mapa.h"
#include "recursos/recursos.h" 
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* --- Variables globales --- */
static EstadoJuego estadoActual = {1, 0, 0, 0, {0, 0}};

/* --- Lógica del Juego --- */

void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    jugador->x = 15;
    jugador->y = 15;
    jugador->oro = 150;
    jugador->madera = 80;
    jugador->piedra = 50;
    jugador->vida = 100;
    jugador->vida_maxima = 100;
    
    estado->mostrarMenu = 1;
    estado->enPartida = 0;
    estado->mostrarResumen = 0;
    estado->opcionSeleccionada = 0;
    estado->puntoMouse.x = 100;
    estado->puntoMouse.y = 100;
    
    inicializarMapa(mapa);
}

void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    int i, j;
    
    // Inicializar todo como agua
    for (i = 0; i < MUNDO_FILAS; i++) {
        for (j = 0; j < MUNDO_COLUMNAS; j++) {
            mapa[i][j] = '~';
        }
    }
    
    // Crear islas principales
    for (i = 10; i < 30; i++) {
        for (j = 10; j < 30; j++) {
            if ((i-20)*(i-20) + (j-20)*(j-20) < 64) {
                mapa[i][j] = '.';
            }
        }
    }
    
    for (i = 40; i < 60; i++) {
        for (j = 40; j < 60; j++) {
            if ((i-50)*(i-50) + (j-50)*(j-50) < 100) {
                mapa[i][j] = '.';
            }
        }
    }
    
    // Agregar montañas
    mapa[15][15] = 'P';
    mapa[16][15] = 'P';
    mapa[15][16] = 'P';
    mapa[45][45] = 'P';
    mapa[46][45] = 'P';
    mapa[45][46] = 'P';
    
    // Agregar recursos
    mapa[12][18] = '$';
    mapa[18][12] = '$';
    mapa[22][22] = '$';
    mapa[42][48] = '$';
    mapa[48][42] = '$';
    mapa[52][52] = '$';
    
    // Agregar áreas enemigas
    for (i = 70; i < 80; i++) {
        for (j = 70; j < 80; j++) {
            mapa[i][j] = 'E';
        }
    }
    
    // Conectar islas con puentes de tierra
    for (i = 25; i < 40; i++) {
        mapa[i][25] = '.';
        mapa[i][26] = '.';
    }
}

void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy) {
    int nuevoX = jugador->x + dx;
    int nuevoY = jugador->y + dy;

    if (nuevoX < 0 || nuevoX >= MUNDO_COLUMNAS || nuevoY < 0 || nuevoY >= MUNDO_FILAS) {
        return;
    }
    if (mapa[nuevoY][nuevoX] == '~') {
        return;
    }
    
    jugador->x = nuevoX;
    jugador->y = nuevoY;
    
    if (mapa[nuevoY][nuevoX] == '$') {
        jugador->oro += 25;
        jugador->madera += 15;
        mapa[nuevoY][nuevoX] = '.';
    }
}

void actualizarCamara(Camera *camara, Jugador jugador) {
    // Centrar la cámara en el jugador
    camara->x = jugador.x - (PANTALLA_COLUMNAS / 2);
    camara->y = jugador.y - (PANTALLA_FILAS / 2);

    // Mantener la cámara dentro de los límites del mundo
    if (camara->x < 0) camara->x = 0;
    if (camara->y < 0) camara->y = 0;
    if (camara->x > MUNDO_COLUMNAS - PANTALLA_COLUMNAS) 
        camara->x = MUNDO_COLUMNAS - PANTALLA_COLUMNAS;
    if (camara->y > MUNDO_FILAS - PANTALLA_FILAS) 
        camara->y = MUNDO_FILAS - PANTALLA_FILAS;
}

/* --- Sistema de Menú Mejorado --- */

void dibujarBoton(HDC hdc, int x, int y, int ancho, int alto, const char* texto, BOOL seleccionado, BOOL activo) {
    COLORREF colorFondo, colorTexto, colorBorde, colorSombra;
    
    if (seleccionado) {
        colorFondo = RGB(100, 180, 255);
        colorTexto = RGB(255, 255, 255);
        colorBorde = RGB(255, 215, 0);
        colorSombra = RGB(30, 100, 180);
    } else if (activo) {
        colorFondo = RGB(70, 130, 180);
        colorTexto = RGB(255, 255, 255);
        colorBorde = RGB(50, 100, 150);
        colorSombra = RGB(20, 60, 100);
    } else {
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
    if (seleccionado || activo) {
        HBRUSH hBrushGradiente = CreateSolidBrush(RGB(120, 180, 255));
        RECT rectGradiente = {x, y, x + ancho, y + alto/3};
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
        TEXT("Arial")
    );
    
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    
    // Dibujar texto usando caracteres UNICODE correctos
    if (strcmp(texto, "INICIAR PARTIDA") == 0) {
        DrawText(hdc, TEXT("START GAME"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else if (strcmp(texto, "RESUMEN RECURSOS") == 0) {
        DrawText(hdc, TEXT("RESOURCES"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else if (strcmp(texto, "SALIR") == 0) {
        DrawText(hdc, TEXT("EXIT"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else if (strcmp(texto, "VOLVER AL MENÚ") == 0) {
        DrawText(hdc, TEXT("BACK TO MENU"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else {
        DrawTextA(hdc, texto, -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
    // Restaurar y limpiar
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    DeleteObject(hBrushFondo);
}

void dibujarFondoAnimado(HDC hdc, int ancho, int alto, int tiempo) {
    // Fondo con gradiente azul marino
    for (int y = 0; y < alto; y++) {
        int azul = 25 + (y * 30) / alto;
        HBRUSH hBrushLinea = CreateSolidBrush(RGB(10, 10, azul));
        RECT rectLinea = {0, y, ancho, y + 1};
        FillRect(hdc, &rectLinea, hBrushLinea);
        DeleteObject(hBrushLinea);
    }
    
    // Estrellas parpadeantes
    srand(tiempo / 10);
    for (int i = 0; i < 80; i++) {
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

void dibujarMenu(HDC hdc, HWND hwnd, EstadoJuego *estado) {
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
    RECT rectTituloSombra = {ancho/2 - 150 + 2, 52, ancho/2 + 150 + 2, 132};
    DrawText(hdc, TEXT("WAR ISLANDS"), -1, &rectTituloSombra, DT_CENTER | DT_SINGLELINE);
    
    // Título principal
    SetTextColor(hdc, RGB(255, 215, 0));
    SetBkMode(hdc, TRANSPARENT);
    RECT rectTitulo = {ancho/2 - 150, 50, ancho/2 + 150, 130};
    DrawText(hdc, TEXT("WAR ISLANDS"), -1, &rectTitulo, DT_CENTER | DT_SINGLELINE);
    
    // Subtítulo
    HFONT hFontSubtitulo = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                    CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                    VARIABLE_PITCH, TEXT("Arial"));
    SelectObject(hdc, hFontSubtitulo);
    SetTextColor(hdc, RGB(180, 220, 255));
    
    RECT rectSubtitulo = {0, 130, ancho, 170};
    DrawText(hdc, TEXT("Conquer - Build - Survive"), -1, &rectSubtitulo,
             DT_CENTER | DT_SINGLELINE);
    
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
    
    dibujarBoton(hdc, centroX - anchoBoton/2, yBase, anchoBoton, altoBoton, 
                 "INICIAR PARTIDA", btnStartSel, TRUE);
    dibujarBoton(hdc, centroX - anchoBoton/2, yBase + altoBoton + espacioBoton, anchoBoton, altoBoton, 
                 "RESUMEN RECURSOS", btnResourcesSel, TRUE);
    dibujarBoton(hdc, centroX - anchoBoton/2, yBase + (altoBoton + espacioBoton) * 2, anchoBoton, altoBoton, 
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

void dibujarResumenRecursos(HDC hdc, Jugador jugador, EstadoJuego *estado) {
    RECT rectClient;
    GetClientRect(WindowFromDC(hdc), &rectClient);
    int ancho = rectClient.right - rectClient.left;
    int alto = rectClient.bottom - rectClient.top;
    
    static int tiempo = 0;
    tiempo++;
    
    // Fondo animado
    dibujarFondoAnimado(hdc, ancho, alto, tiempo);
    
    // Título
    HFONT hFontTitulo = CreateFont(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                 CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                 VARIABLE_PITCH, TEXT("Arial"));
    HFONT hOldFont = SelectObject(hdc, hFontTitulo);
    
    SetTextColor(hdc, RGB(255, 215, 0));
    SetBkMode(hdc, TRANSPARENT);
    
    RECT rectTitulo = {0, 40, ancho, 100};
    DrawText(hdc, TEXT("RESOURCES SUMMARY"), -1, &rectTitulo, DT_CENTER | DT_SINGLELINE);
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hFontTitulo);
    
    // Panel de recursos
    int panelAncho = 400;
    int panelAlto = 300;
    int panelX = (ancho - panelAncho) / 2;
    int panelY = (alto - panelAlto) / 2 - 30;
    
    // Fondo del panel
    HBRUSH hBrushPanel = CreateSolidBrush(RGB(40, 40, 80));
    HBRUSH hBrushBordePanel = CreateSolidBrush(RGB(255, 215, 0));
    
    RECT rectBordePanel = {panelX - 4, panelY - 4, panelX + panelAncho + 4, panelY + panelAlto + 4};
    RECT rectPanel = {panelX, panelY, panelX + panelAncho, panelY + panelAlto};
    
    FillRect(hdc, &rectBordePanel, hBrushBordePanel);
    FillRect(hdc, &rectPanel, hBrushPanel);
    
    DeleteObject(hBrushPanel);
    DeleteObject(hBrushBordePanel);
    
    // Información de recursos
    HFONT hFontRecursos = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                  CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  VARIABLE_PITCH, TEXT("Arial"));
    hOldFont = SelectObject(hdc, hFontRecursos);
    
    int yPos = panelY + 40;
    int lineHeight = 40;
    int textoX = panelX + 50;
    int valorX = panelX + panelAncho - 100;
    
    // Oro
    SetTextColor(hdc, RGB(255, 215, 0));
    TextOutA(hdc, textoX, yPos, "GOLD:", 5);
    SetTextColor(hdc, RGB(255, 255, 255));
    char bufferOro[50];
    sprintf(bufferOro, "%d", jugador.oro);
    TextOutA(hdc, valorX, yPos, bufferOro, strlen(bufferOro));
    
    // Madera
    SetTextColor(hdc, RGB(210, 180, 140));
    TextOutA(hdc, textoX, yPos + lineHeight, "WOOD:", 5);
    SetTextColor(hdc, RGB(255, 255, 255));
    char bufferMadera[50];
    sprintf(bufferMadera, "%d", jugador.madera);
    TextOutA(hdc, valorX, yPos + lineHeight, bufferMadera, strlen(bufferMadera));
    
    // Piedra
    SetTextColor(hdc, RGB(169, 169, 169));
    TextOutA(hdc, textoX, yPos + lineHeight * 2, "STONE:", 6);
    SetTextColor(hdc, RGB(255, 255, 255));
    char bufferPiedra[50];
    sprintf(bufferPiedra, "%d", jugador.piedra);
    TextOutA(hdc, valorX, yPos + lineHeight * 2, bufferPiedra, strlen(bufferPiedra));
    
    // Vida
    SetTextColor(hdc, RGB(255, 80, 80));
    TextOutA(hdc, textoX, yPos + lineHeight * 3, "HEALTH:", 7);
    SetTextColor(hdc, RGB(255, 255, 255));
    char bufferVida[50];
    sprintf(bufferVida, "%d/%d", jugador.vida, jugador.vida_maxima);
    TextOutA(hdc, valorX, yPos + lineHeight * 3, bufferVida, strlen(bufferVida));
    
    // Barra de vida
    int barraAncho = panelAncho - 150;
    int barraX = textoX;
    int barraY = yPos + lineHeight * 3 + 25;
    int vidaWidth = (jugador.vida * barraAncho) / jugador.vida_maxima;
    
    // Fondo barra
    HBRUSH hBrushBarraFondo = CreateSolidBrush(RGB(50, 50, 50));
    RECT rectBarraFondo = {barraX, barraY, barraX + barraAncho, barraY + 12};
    FillRect(hdc, &rectBarraFondo, hBrushBarraFondo);
    DeleteObject(hBrushBarraFondo);
    
    // Barra de vida
    HBRUSH hBrushBarraVida = CreateSolidBrush(RGB(50, 200, 50));
    RECT rectBarraVida = {barraX, barraY, barraX + vidaWidth, barraY + 12};
    FillRect(hdc, &rectBarraVida, hBrushBarraVida);
    DeleteObject(hBrushBarraVida);
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hFontRecursos);
    
    // Botón volver
    int btnAncho = 200;
    int btnVolverX = ancho/2 - btnAncho/2;
    int btnVolverY = panelY + panelAlto + 30;
    int btnVolverSel = (estado->opcionSeleccionada == 0);
    
    dibujarBoton(hdc, btnVolverX, btnVolverY, btnAncho, 45, "VOLVER AL MENÚ", btnVolverSel, TRUE);
    
    // Instrucciones para volver
    HFONT hFontInstrucciones = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                       DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                       CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                       VARIABLE_PITCH, TEXT("Arial"));
    SelectObject(hdc, hFontInstrucciones);
    SetTextColor(hdc, RGB(150, 200, 255));
    
    RECT rectInstrucciones = {0, alto - 30, ancho, alto};
    DrawText(hdc, TEXT("Press ENTER or ESC to return to menu"), -1, &rectInstrucciones,
             DT_CENTER | DT_SINGLELINE);
    
    DeleteObject(hFontInstrucciones);
}

int verificarColisionBoton(int mouseX, int mouseY, int btnX, int btnY, int btnAncho, int btnAlto) {
    return (mouseX >= btnX && mouseX <= btnX + btnAncho &&
            mouseY >= btnY && mouseY <= btnY + btnAlto);
}

void actualizarPuntoMenu(EstadoJuego *estado, int x, int y, HWND hwnd) {
    estado->puntoMouse.x = x;
    estado->puntoMouse.y = y;
    
    RECT rectClient;
    GetClientRect(hwnd, &rectClient);
    int ancho = rectClient.right;
    
    if (estado->mostrarMenu) {
        int centroX = ancho / 2;
        int anchoBoton = 300;
        int altoBoton = 70;
        int espacioBoton = 30;
        int yBase = 220;
        
        estado->opcionSeleccionada = -1;
        
        if (verificarColisionBoton(x, y, centroX - anchoBoton/2, yBase, anchoBoton, altoBoton)) {
            estado->opcionSeleccionada = 0;
        } else if (verificarColisionBoton(x, y, centroX - anchoBoton/2, yBase + altoBoton + espacioBoton, anchoBoton, altoBoton)) {
            estado->opcionSeleccionada = 1;
        } else if (verificarColisionBoton(x, y, centroX - anchoBoton/2, yBase + (altoBoton + espacioBoton) * 2, anchoBoton, altoBoton)) {
            estado->opcionSeleccionada = 2;
        }
    } else if (estado->mostrarResumen) {
        int btnVolverX = ancho/2 - 100;
        int btnVolverY = 120 + 300 + 20;
        
        estado->opcionSeleccionada = -1;
        if (verificarColisionBoton(x, y, btnVolverX, btnVolverY, 200, 50)) {
            estado->opcionSeleccionada = 0;
        }
    }
}

void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado) {
    actualizarPuntoMenu(estado, x, y, hwnd);
    
    if (estado->opcionSeleccionada != -1) {
        if (estado->mostrarMenu) {
            switch (estado->opcionSeleccionada) {
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
        } else if (estado->mostrarResumen) {
            if (estado->opcionSeleccionada == 0) {
                estado->mostrarMenu = 1;
                estado->enPartida = 0;
                estado->mostrarResumen = 0;
            }
        }
        
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void procesarEnterMenu(HWND hwnd, EstadoJuego *estado) {
    if (estado->opcionSeleccionada != -1) {
        if (estado->mostrarMenu) {
            switch (estado->opcionSeleccionada) {
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

EstadoJuego* obtenerEstadoJuego() {
    return &estadoActual;
}

void iniciarPartida() {
    estadoActual.mostrarMenu = 0;
    estadoActual.enPartida = 1;
    estadoActual.mostrarResumen = 0;
}

void volverAlMenu() {
    estadoActual.mostrarMenu = 1;
    estadoActual.enPartida = 0;
    estadoActual.mostrarResumen = 0;
}

/* --- Gráficos del Juego --- */

void dibujarMapa(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera camara, int anchoVentana, int altoVentana) {
    HBRUSH hBrushAgua = CreateSolidBrush(RGB(30, 144, 255));
    HBRUSH hBrushTierra = CreateSolidBrush(RGB(139, 115, 85));
    HBRUSH hBrushCesped = CreateSolidBrush(RGB(34, 139, 34));
    HBRUSH hBrushEnemigo = CreateSolidBrush(RGB(220, 20, 60));
    HBRUSH hBrushRecurso = CreateSolidBrush(RGB(255, 215, 0));
    HBRUSH hBrushMontana = CreateSolidBrush(RGB(128, 128, 128));
    HBRUSH hBrushActual;
    
    // Calcular celdas visibles
    int celdasX = anchoVentana / TAMANO_CELDA;
    int celdasY = altoVentana / TAMANO_CELDA;
    
    if (celdasX > PANTALLA_COLUMNAS) celdasX = PANTALLA_COLUMNAS;
    if (celdasY > PANTALLA_FILAS) celdasY = PANTALLA_FILAS;
    
    int y_pantalla, x_pantalla;
    int y_mundo, x_mundo;
    RECT rectCelda;

    for (y_pantalla = 0; y_pantalla < celdasY; y_pantalla++) {
        for (x_pantalla = 0; x_pantalla < celdasX; x_pantalla++) {
            y_mundo = y_pantalla + camara.y;
            x_mundo = x_pantalla + camara.x;
            
            if (y_mundo >= MUNDO_FILAS || x_mundo >= MUNDO_COLUMNAS) {
                continue;
            }
            
            switch (mapa[y_mundo][x_mundo]) {
                case 'P':
                    hBrushActual = hBrushMontana;
                    break;
                case '.':
                    hBrushActual = hBrushCesped;
                    break;
                case 'E':
                    hBrushActual = hBrushEnemigo;
                    break;
                case '$':
                    hBrushActual = hBrushRecurso;
                    break;
                case '~':
                default:
                    hBrushActual = hBrushAgua;
                    break;
            }

            rectCelda.left = x_pantalla * TAMANO_CELDA;
            rectCelda.top = y_pantalla * TAMANO_CELDA;
            rectCelda.right = (x_pantalla + 1) * TAMANO_CELDA;
            rectCelda.bottom = (y_pantalla + 1) * TAMANO_CELDA;

            FillRect(hdc, &rectCelda, hBrushActual);
            
            // Dibujar borde de la celda
            HPEN hPenBorde = CreatePen(PS_SOLID, 1, RGB(50, 50, 50));
            HPEN hOldPen = SelectObject(hdc, hPenBorde);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, rectCelda.left, rectCelda.top, rectCelda.right, rectCelda.bottom);
            SelectObject(hdc, hOldPen);
            DeleteObject(hPenBorde);
        }
    }
    
    DeleteObject(hBrushAgua);
    DeleteObject(hBrushTierra);
    DeleteObject(hBrushCesped);
    DeleteObject(hBrushEnemigo);
    DeleteObject(hBrushRecurso);
    DeleteObject(hBrushMontana);
}

void dibujarJugador(HDC hdc, Jugador jugador, Camera camara) {
    int x_pantalla = (jugador.x - camara.x) * TAMANO_CELDA;
    int y_pantalla = (jugador.y - camara.y) * TAMANO_CELDA;

    // Verificar si el jugador está dentro del área visible
    if (x_pantalla >= 0 && x_pantalla < PANTALLA_COLUMNAS * TAMANO_CELDA && 
        y_pantalla >= 0 && y_pantalla < PANTALLA_FILAS * TAMANO_CELDA) {
        
        // Dibujar sprite con transparencia magenta
        dibujar_sprite(hdc, sprite_jugador, x_pantalla, y_pantalla);
    }
}

/* ========== NUEVAS FUNCIONES CON SPRITES ========== */

void dibujarMenuConSprites(HDC hdc, HWND hwnd, EstadoJuego *estado) {
    RECT rectClient;
    GetClientRect(hwnd, &rectClient);
    int ancho = rectClient.right - rectClient.left;
    int alto = rectClient.bottom - rectClient.top;
    
    printf("Dibujando menu con sprites - Ventana: %dx%d\n", ancho, alto);
    
    // 1. DIBUJAR FONDO CON SPRITE - CENTRADO
    if (sprite_fondo_menu.bitmap) {
        printf("Usando sprite de fondo: %dx%d\n", sprite_fondo_menu.ancho, sprite_fondo_menu.alto);
        // Centrar el fondo exactamente
        int x = (ancho - sprite_fondo_menu.ancho) / 2;
        int y = (alto - sprite_fondo_menu.alto) / 2;
        dibujar_sprite(hdc, sprite_fondo_menu, x, y);
    } else {
        printf("Usando fondo animado por código\n");
        // Fallback: usar fondo animado por código
        static int tiempo = 0;
        tiempo++;
        dibujarFondoAnimado(hdc, ancho, alto, tiempo);
    }
    
    // 2. DIBUJAR TÍTULO CON SPRITE
    if (sprite_titulo.bitmap) {
        int tituloX = (ancho - sprite_titulo.ancho) / 2;
        int tituloY = 50;
        dibujar_sprite(hdc, sprite_titulo, tituloX, tituloY);
    } else {
        // Fallback: título por código
        HFONT hFontTitulo = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                     CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                     VARIABLE_PITCH, TEXT("Arial"));
        HFONT hOldFont = SelectObject(hdc, hFontTitulo);
        
        SetTextColor(hdc, RGB(255, 215, 0));
        SetBkMode(hdc, TRANSPARENT);
        RECT rectTitulo = {ancho/2 - 150, 50, ancho/2 + 150, 130};
        DrawText(hdc, TEXT("WAR ISLANDS"), -1, &rectTitulo, DT_CENTER | DT_SINGLELINE);
        
        SelectObject(hdc, hOldFont);
        DeleteObject(hFontTitulo);
    }
    
    // 3. DIBUJAR BOTONES CON SPRITES
    int centroX = ancho / 2;
    int anchoBoton = 300;
    int altoBoton = 70;
    int espacioBoton = 25;
    int yBase = 220;
    
    // Botones
    for (int i = 0; i < 3; i++) {
        int btnX = centroX - anchoBoton/2;
        int btnY = yBase + i * (altoBoton + espacioBoton);
        BOOL seleccionado = (estado->opcionSeleccionada == i);
        
        if (sprite_boton_normal.bitmap && sprite_boton_seleccionado.bitmap) {
            // Usar sprites de botones
            Sprite sprite_a_usar = seleccionado ? sprite_boton_seleccionado : sprite_boton_normal;
            dibujar_sprite(hdc, sprite_a_usar, btnX, btnY);
            
            // Dibujar texto sobre el botón
            HFONT hFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                   DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                   CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                   VARIABLE_PITCH, TEXT("Arial"));
            HFONT hOldFont = SelectObject(hdc, hFont);
            
            SetTextColor(hdc, seleccionado ? RGB(255, 255, 200) : RGB(255, 255, 255));
            SetBkMode(hdc, TRANSPARENT);
            
            RECT rectTexto = {btnX, btnY, btnX + anchoBoton, btnY + altoBoton};
            const char* textos[] = {"START GAME", "RESOURCES", "EXIT GAME"};
            DrawTextA(hdc, textos[i], -1, &rectTexto, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
        } else {
            // Fallback: botones por código
            const char* textos[] = {"INICIAR PARTIDA", "RESUMEN RECURSOS", "SALIR"};
            dibujarBoton(hdc, btnX, btnY, anchoBoton, altoBoton, textos[i], seleccionado, TRUE);
        }
    }
    
    // 4. INSTRUCCIONES Y CRÉDITOS (siempre por código)
    HFONT hFontInstrucciones = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                       DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                       CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                       VARIABLE_PITCH, TEXT("Arial"));
    SelectObject(hdc, hFontInstrucciones);
    SetTextColor(hdc, RGB(200, 230, 255));
    
    RECT rectInstrucciones = {0, alto - 60, ancho, alto - 30};
    DrawText(hdc, TEXT("Use ARROW KEYS to navigate • ENTER to select"), -1, &rectInstrucciones,
             DT_CENTER | DT_SINGLELINE);
    
    RECT rectCreditos = {0, alto - 30, ancho, alto};
    DrawText(hdc, TEXT("War Islands v1.0 • Developed in C • 2024"), -1, &rectCreditos,
             DT_CENTER | DT_SINGLELINE);
    
    DeleteObject(hFontInstrucciones);
    
    printf("Menu con sprites dibujado exitosamente\n");
}

void dibujarFondoAnimadoMejorado(HDC hdc, int ancho, int alto, int tiempo) {
    // Gradiente azul marino más profundo
    for (int y = 0; y < alto; y++) {
        int azul = 30 + (y * 40) / alto;
        int verde = 10 + (y * 10) / alto;
        HBRUSH hBrushLinea = CreateSolidBrush(RGB(5, verde, azul));
        RECT rectLinea = {0, y, ancho, y + 1};
        FillRect(hdc, &rectLinea, hBrushLinea);
        DeleteObject(hBrushLinea);
    }
    
    // Estrellas más dinámicas
    srand(tiempo / 8);
    for (int i = 0; i < 120; i++) {
        int x = rand() % ancho;
        int y = rand() % alto;
        int brillo = 80 + (rand() % 175);
        int tamano = 1 + (rand() % 3);
        int parpadeo = (tiempo + i) % 100;
        
        if (parpadeo < 80) {  // Efecto de parpadeo
            HBRUSH hBrushEstrella = CreateSolidBrush(RGB(brillo, brillo, brillo));
            RECT rectEstrella = {x, y, x + tamano, y + tamano};
            FillRect(hdc, &rectEstrella, hBrushEstrella);
            DeleteObject(hBrushEstrella);
        }
    }
}

void dibujarBotonMejorado(HDC hdc, int x, int y, int ancho, int alto, const char* texto, BOOL seleccionado, BOOL activo) {
    COLORREF colorFondo, colorTexto, colorBorde, colorSombra, colorBrillo;
    
    if (seleccionado) {
        colorFondo = RGB(80, 160, 240);
        colorTexto = RGB(255, 255, 255);
        colorBorde = RGB(255, 225, 50);
        colorSombra = RGB(20, 80, 160);
        colorBrillo = RGB(120, 200, 255);
    } else if (activo) {
        colorFondo = RGB(60, 120, 200);
        colorTexto = RGB(255, 255, 255);
        colorBorde = RGB(40, 90, 150);
        colorSombra = RGB(15, 50, 100);
        colorBrillo = RGB(90, 160, 220);
    } else {
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
    if (seleccionado || activo) {
        HBRUSH hBrushGradiente = CreateSolidBrush(colorBrillo);
        RECT rectGradiente = {x, y, x + ancho, y + alto/4};
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
        TEXT("Arial")
    );
    
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    
    // Dibujar texto
    if (strcmp(texto, "INICIAR PARTIDA") == 0) {
        DrawText(hdc, TEXT("START GAME"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else if (strcmp(texto, "RESUMEN RECURSOS") == 0) {
        DrawText(hdc, TEXT("RESOURCES SUMMARY"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else if (strcmp(texto, "SALIR") == 0) {
        DrawText(hdc, TEXT("EXIT GAME"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else if (strcmp(texto, "VOLVER AL MENÚ") == 0) {
        DrawText(hdc, TEXT("BACK TO MENU"), -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else {
        DrawTextA(hdc, texto, -1, &rectBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
    // Restaurar y limpiar
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    DeleteObject(hBrushFondo);
}