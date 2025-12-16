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
    jugador->x = 50;
    jugador->y = 50;
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
    
    // NOTA: miCamara se inicializa en main.c
    printf("Juego inicializado. Jugador en (%d,%d)\n", jugador->x, jugador->y);
}

void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    int i, j;
    
    // Inicializar todo como agua (océano)
    for (i = 0; i < MUNDO_FILAS; i++) {
        for (j = 0; j < MUNDO_COLUMNAS; j++) {
            mapa[i][j] = '~';  // Agua/océano
        }
    }
    
    // Área de la isla principal (50x50 celdas, centrada en 50,50)
    int centro_x = 50;
    int centro_y = 50;
    int radio_celdas = 25;  // 500px / 10px/celda / 2 = 25 celdas
    
    for (i = 0; i < MUNDO_FILAS; i++) {
        for (j = 0; j < MUNDO_COLUMNAS; j++) {
            int dx = j - centro_x;
            int dy = i - centro_y;
            
            // Si está dentro del círculo de la isla
            if (dx*dx + dy*dy <= radio_celdas*radio_celdas) {
                mapa[i][j] = '.';  // Tierra (transitable)
            }
        }
    }
    
    printf("Mapa inicializado: Isla en centro (%d,%d) radio %d celdas\n", centro_x, centro_y, radio_celdas);
}

void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy) {
    int nuevoX = jugador->x + dx;
    int nuevoY = jugador->y + dy;

    // Validar límites del mundo
    if (nuevoX < 0 || nuevoX >= MUNDO_COLUMNAS || nuevoY < 0 || nuevoY >= MUNDO_FILAS) {
        return;
    }
    
    // NO puede moverse al agua (a menos que tenga barco más adelante)
    if (mapa[nuevoY][nuevoX] == '~') {
        printf("Movimiento bloqueado: Agua\n");
        return;
    }
    
    // Actualizar posición
    jugador->x = nuevoX;
    jugador->y = nuevoY;
    
    printf("Jugador movido a: (%d, %d) - Terreno: %c\n", nuevoX, nuevoY, mapa[nuevoY][nuevoX]);
    
    // Sistema de recursos (puedes expandir esto más adelante)
    if (mapa[nuevoY][nuevoX] == '$') {
        jugador->oro += 25;
        jugador->madera += 15;
        mapa[nuevoY][nuevoX] = '.';
        printf("¡Recurso recolectado! Oro: %d, Madera: %d\n", jugador->oro, jugador->madera);
    }
}

void actualizarCamara(Camera *camara, Jugador jugador) {
    printf("DEBUG actualizarCamara: INICIO - Jugador(%d,%d) Zoom=%.1f\n", jugador.x, jugador.y, camara->zoom);
    
    // 1. Calcular tamaño de celda con zoom
    camara->tamano_celda_actual = (int)(TAMANO_CELDA_BASE * camara->zoom);
    
    // 2. Calcular cuántas celdas caben en pantalla CON ESTE ZOOM
    // Fórmula CORREGIDA: 
    // Si zoom=2.0, las celdas son 2x más grandes, así que caben LA MITAD
    float celdas_visibles_x = (float)PANTALLA_COLUMNAS / camara->zoom;
    float celdas_visibles_y = (float)PANTALLA_FILAS / camara->zoom;
    
    printf("DEBUG: celdas_visibles=%.1fx%.1f (zoom=%.1f)\n", celdas_visibles_x, celdas_visibles_y, camara->zoom);
    
    // 3. Centrar EXACTAMENTE en el jugador
    // El jugador debe quedar en el centro VISUAL de la pantalla
    camara->x = jugador.x - (int)(celdas_visibles_x / 2);
    camara->y = jugador.y - (int)(celdas_visibles_y / 2);
    
    // 4. Limitar a los bordes del mundo
    if (camara->x < 0) camara->x = 0;
    if (camara->y < 0) camara->y = 0;
    
    int max_x = MUNDO_COLUMNAS - (int)celdas_visibles_x;
    int max_y = MUNDO_FILAS - (int)celdas_visibles_y;
    
    if (max_x < 0) max_x = 0;
    if (max_y < 0) max_y = 0;
    
    if (camara->x > max_x) camara->x = max_x;
    if (camara->y > max_y) camara->y = max_y;
    
    printf("DEBUG actualizarCamara: FIN - Cámara(%d,%d) Celda=%dpx\n", camara->x, camara->y, camara->tamano_celda_actual);
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

/* ========== FUNCIONES CON SPRITES ========== */

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

/* ========== DIBUJAR MAPA CON ZOOM (ÚNICA FUNCIÓN DE DIBUJADO) ========== */
void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera camara, int anchoVentana, int altoVentana) {
    printf("=== DIBUJANDO MAPA CON ZOOM (%.1fx) ===\n", camara.zoom);
    printf("Tamaño celda: %dpx\n", camara.tamano_celda_actual);
    printf("Cámara: (%d, %d)\n", camara.x, camara.y);
    
    // 1. FONDO AZUL DEL OCÉANO
    HBRUSH hBrushAgua = CreateSolidBrush(RGB(30, 144, 255));
    RECT rectFondo = {0, 0, anchoVentana, altoVentana};
    FillRect(hdc, &rectFondo, hBrushAgua);
    DeleteObject(hBrushAgua);
    
    // Calcular celdas visibles basado en zoom
    int celdasX = anchoVentana / camara.tamano_celda_actual;
    int celdasY = altoVentana / camara.tamano_celda_actual;
    
    // Asegurar que no excedamos los límites
    if (celdasX > PANTALLA_COLUMNAS) celdasX = PANTALLA_COLUMNAS;
    if (celdasY > PANTALLA_FILAS) celdasY = PANTALLA_FILAS;
    
    // 2. DIBUJAR ISLA PRINCIPAL CON ZOOM
    if (sprite_isla_principal.bitmap) {
        printf("Sprite isla: %dx%d\n", sprite_isla_principal.ancho, sprite_isla_principal.alto);
        
        // Centro de la isla en el mundo (posición 50,50)
        int centro_mundo_x = 50;
        int centro_mundo_y = 50;
        
        // Calcular dimensiones escaladas según zoom
        int ancho_escalado = (int)(sprite_isla_principal.ancho * camara.zoom);
        int alto_escalado = (int)(sprite_isla_principal.alto * camara.zoom);
        
        // Convertir a coordenadas de pantalla
        // IMPORTANTE: Usar camara.tamano_celda_actual para conversión correcta
        int centro_pantalla_x = (centro_mundo_x - camara.x) * camara.tamano_celda_actual;
        int centro_pantalla_y = (centro_mundo_y - camara.y) * camara.tamano_celda_actual;
        
        // Calcular esquina superior izquierda para centrar
        int isla_x = centro_pantalla_x - (ancho_escalado / 2);
        int isla_y = centro_pantalla_y - (alto_escalado / 2);
        
        printf("DEBUG: Centro mundo(%d,%d) -> pantalla(%d,%d)\n", 
               centro_mundo_x, centro_mundo_y, centro_pantalla_x, centro_pantalla_y);
        printf("DEBUG: Isla escalada %dx%d en (%d,%d)\n", 
               ancho_escalado, alto_escalado, isla_x, isla_y);
        
        // Verificar si es visible
        if (isla_x + ancho_escalado > 0 && isla_x < anchoVentana &&
            isla_y + alto_escalado > 0 && isla_y < altoVentana) {
            
            // Usar TransparentBlt con escalado
            HDC hdcMem = CreateCompatibleDC(hdc);
            SelectObject(hdcMem, sprite_isla_principal.bitmap);
            
            TransparentBlt(hdc, 
                          isla_x, isla_y,                    // Destino (x,y)
                          ancho_escalado, alto_escalado,    // Destino (ancho,alto)
                          hdcMem, 0, 0,                     // Origen (0,0)
                          sprite_isla_principal.ancho,      // Origen ancho
                          sprite_isla_principal.alto,       // Origen alto
                          RGB(255, 0, 255));                // Color transparente
            
            DeleteDC(hdcMem);
            printf("Isla dibujada con zoom\n");
        } else {
            printf("DEBUG: Isla fuera de pantalla\n");
        }
    } else {
        printf("ERROR: No se cargó sprite de isla\n");
    }
    
    // 3. DIBUJAR ELEMENTOS INDIVIDUALES (opcional, para zoom cercano)
    /* 
      if (camara.zoom >= 2.0f) {  // Solo dibujar detalles si el zoom es alto
        HBRUSH hBrushTierra = CreateSolidBrush(RGB(139, 115, 85));
        HBRUSH hBrushRecurso = CreateSolidBrush(RGB(255, 215, 0));
        
        for (int y = 0; y < celdasY; y++) {
            for (int x = 0; x < celdasX; x++) {
                int mundo_y = y + camara.y;
                int mundo_x = x + camara.x;
                
                if (mundo_y >= MUNDO_FILAS || mundo_x >= MUNDO_COLUMNAS) {
                    continue;
                }
                
                int pantalla_x = x * camara.tamano_celda_actual;
                int pantalla_y = y * camara.tamano_celda_actual;
                
                RECT rectCelda = {
                    pantalla_x, 
                    pantalla_y, 
                    pantalla_x + camara.tamano_celda_actual, 
                    pantalla_y + camara.tamano_celda_actual
                };
                
                switch (mapa[mundo_y][mundo_x]) {
                    case '$':  // Recurso
                        FillRect(hdc, &rectCelda, hBrushRecurso);
                        break;
                    case '.':  // Tierra
                        FillRect(hdc, &rectCelda, hBrushTierra);
                        break;
                }
            }
        }
        
        DeleteObject(hBrushTierra);
        DeleteObject(hBrushRecurso);
    } 
    */
    
    printf("=== FIN DIBUJO MAPA CON ZOOM ===\n\n");
}

/* ========== DIBUJAR JUGADOR CON ZOOM ========== */
void dibujarJugador(HDC hdc, Jugador jugador, Camera camara) {
    // Calcular posición en pantalla considerando zoom
    int x_pantalla = (jugador.x - camara.x) * camara.tamano_celda_actual;
    int y_pantalla = (jugador.y - camara.y) * camara.tamano_celda_actual;
    
    // Calcular tamaño escalado del jugador
    int ancho_jugador = (int)(sprite_jugador.ancho * camara.zoom);
    int alto_jugador = (int)(sprite_jugador.alto * camara.zoom);
    
    // Ajustar para centrar (el jugador está en el centro de su celda)
    int ajuste_x = x_pantalla - (ancho_jugador - camara.tamano_celda_actual) / 2;
    int ajuste_y = y_pantalla - (alto_jugador - camara.tamano_celda_actual) / 2;

    // Verificar si está visible
    if (ajuste_x + ancho_jugador > 0 && ajuste_x < PANTALLA_COLUMNAS * TAMANO_CELDA_BASE && 
        ajuste_y + alto_jugador > 0 && ajuste_y < PANTALLA_FILAS * TAMANO_CELDA_BASE) {
        
        // Dibujar sprite escalado con zoom
        HDC hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, sprite_jugador.bitmap);
        
        TransparentBlt(hdc, 
                      ajuste_x, ajuste_y,           // Destino
                      ancho_jugador, alto_jugador,  // Destino escalado
                      hdcMem, 0, 0,                 // Origen
                      sprite_jugador.ancho,         // Origen ancho
                      sprite_jugador.alto,          // Origen alto
                      RGB(255, 0, 255));            // Transparente
        
        DeleteDC(hdcMem);
        
        printf("DEBUG dibujarJugador: Jugador en (%d,%d) -> pantalla(%d,%d) tamaño %dx%d\n",
               jugador.x, jugador.y, ajuste_x, ajuste_y, ancho_jugador, alto_jugador);
    } else {
        printf("DEBUG: Jugador fuera de pantalla\n");
    }
}

/* ========== FUNCIONES ADICIONALES (opcionales) ========== */

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