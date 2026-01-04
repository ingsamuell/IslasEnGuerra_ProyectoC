/* src/mapa/mapa.c - VERSIÓN LIMPIA Y CORREGIDA */
#include "mapa.h"                 // En la misma carpeta
#include "../global.h"            // Subir a src/
#include "../recursos/recursos.h" // Subir a src/ y entrar a recursos/
#include <stdio.h>
#include <mmsystem.h> // Necesario para PlaySound
#include <math.h>     // Necesario para calcular distancias de ataque
#include <stdbool.h>

// Función para obtener la imagen correcta de isla según el mapa seleccionado
HBITMAP obtenerImagenIsla(int indiceIsla, int mapaId) {
    switch(mapaId) {
        case 0: // MAPA 1 (original)
            switch(indiceIsla) {
                case 0: return hBmpIslaGrande;
                case 1: return hBmpIslaSec2;
                case 2: return hBmpIslaSec4;
                case 3: return hBmpIslaSec1;
                case 4: return hBmpIslaSec3;
            }
            break;
            
        case 1: // MAPA 2
            switch(indiceIsla) {
                case 0: return hBmpIslaGrandeMapa2;
                case 1: return hBmpIslaSec2Mapa2;
                case 2: return hBmpIslaSec4Mapa2;
                case 3: return hBmpIslaSec1Mapa2;
                case 4: return hBmpIslaSec3Mapa2;
            }
            break;
            
        case 2: // MAPA 3
            switch(indiceIsla) {
                case 0: return hBmpIslaGrandeMapa3;
                case 1: return hBmpIslaSec2Mapa3;
                case 2: return hBmpIslaSec4Mapa3;
                case 3: return hBmpIslaSec1Mapa3;
                case 4: return hBmpIslaSec3Mapa3;
            }
            break;
    }
    return NULL;
}

// Referencia a las imágenes cargadas en recursos.c
extern HBITMAP hBmpVaca[8];
// Estado interno de las vacas
Vaca manada[MAX_VACAS];
Mina misMinas[MAX_MINAS];
Particula chispas[MAX_PARTICULAS];
#define MAX_ISLAS 5
Isla misIslas[MAX_ISLAS];
TextoFlotante listaTextos[MAX_TEXTOS] = {0};
#define MAX_TEXTOS_FLOTANTES 20
// Variable global de árboles
Arbol misArboles[MAX_ARBOLES];
// Variable global
Tesoro misTesoros[MAX_TESOROS];
Unidad misUnidades[MAX_UNIDADES];
Unidad unidades[MAX_UNIDADES];
Vaca vacas[MAX_VACAS]; 
extern Jugador miJugador;            // <--- SOLUCIÓN AL ERROR
extern Unidad unidades[MAX_UNIDADES]; // <--- NECESARIO PARA crearUnidadEnMapa

#define MARGEN_ESTABLO 100 // El radio de la "cerca"

// --- INICIALIZACIÓN DE ISLAS (MODIFICADA) ---
void inicializarIslas(int mapaId)
{
    for (int i = 0; i < MAX_ISLAS; i++)
        misIslas[i].activa = 0;

    // MANTENEMOS LAS MISMAS POSICIONES para todos los mapas
    // Solo cambiarán las imágenes
    
    // ISLA CENTRAL (Grande)
    misIslas[0].activa = 1;
    misIslas[0].x = 1100;
    misIslas[0].y = 1100;
    misIslas[0].ancho = 1000;
    misIslas[0].alto = 1000;
    misIslas[0].margen = 0;

    // ISLA NORTE
    misIslas[1].activa = 1;
    misIslas[1].x = 1250;
    misIslas[1].y = 200;
    misIslas[1].ancho = 700;
    misIslas[1].alto = 700;
    misIslas[1].margen = 0;

    // ISLA SUR
    misIslas[2].activa = 1;
    misIslas[2].x = 1350;
    misIslas[2].y = 2300;
    misIslas[2].ancho = 500;
    misIslas[2].alto = 500;
    misIslas[2].margen = 0;

    // ISLA OESTE
    misIslas[3].activa = 1;
    misIslas[3].x = 580;
    misIslas[3].y = 1475;
    misIslas[3].ancho = 320;
    misIslas[3].alto = 250;
    misIslas[3].margen = 0;

    // ISLA ESTE
    misIslas[4].activa = 1;
    misIslas[4].x = 2300;
    misIslas[4].y = 1400;
    misIslas[4].ancho = 400;
    misIslas[4].alto = 400;
    misIslas[4].margen = 0;
}

void agregarTextoFlotante(int x, int y, char* contenido, COLORREF color) {
    for (int i = 0; i < MAX_TEXTOS; i++) {
        if (!listaTextos[i].activo) {
            listaTextos[i].x = (float)x;
            listaTextos[i].y = (float)y;
            strcpy(listaTextos[i].texto, contenido);
            listaTextos[i].color = color;
            listaTextos[i].vida = 60; // <--- Aumentado a 60 frames
            listaTextos[i].activo = 1;
            break;
        }
    }
}

void actualizarLogicaSistema() {
    // 1. Actualizar Textos Flotantes
    for (int i = 0; i < MAX_TEXTOS; i++) {
        if (listaTextos[i].activo) {
            listaTextos[i].y -= 0.8f; // Velocidad de subida
            listaTextos[i].vida--;
            if (listaTextos[i].vida <= 0) {
                listaTextos[i].activo = 0;
            }
        }
    }
    
    // 2. Podrías mover aquí también la lógica de chispas si quieres centralizar
}
void crearTextoFlotante(int x, int y, const char* formato, int cantidad, COLORREF color) {
    for (int i = 0; i < MAX_TEXTOS_FLOTANTES; i++) {
        if (!listaTextos[i].activo) {
            listaTextos[i].x = (float)x;
            listaTextos[i].y = (float)y;
            sprintf(listaTextos[i].texto, "+%d %s", cantidad, formato);
            listaTextos[i].vida = 40; // Duración en frames
            listaTextos[i].color = color;
            listaTextos[i].activo = true;
            break;
        }
    }
}
void actualizarYDibujarTextos(HDC hdc, Camera cam) {
    SetBkMode(hdc, TRANSPARENT);
    for (int i = 0; i < MAX_TEXTOS; i++) {
        if (listaTextos[i].activo) {

            // Convertir posición del mundo a pantalla
            int sx = (int)((listaTextos[i].x - cam.x) * cam.zoom);
            int sy = (int)((listaTextos[i].y - cam.y) * cam.zoom);

            // Solo dibujamos si está dentro de la pantalla (opcional pero recomendado)
            // Dibujar sombra negra
            SetTextColor(hdc, RGB(0, 0, 0));
            TextOut(hdc, sx + 1, sy + 1, listaTextos[i].texto, (int)strlen(listaTextos[i].texto));
            
            // Dibujar texto principal
            SetTextColor(hdc, listaTextos[i].color);
            TextOut(hdc, sx, sy, listaTextos[i].texto, (int)strlen(listaTextos[i].texto));
        }
    }
}
// --- NUEVA FUNCIÓN MAGICA: ESCANEAR BMP ---
void generarColisionDesdeImagen(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], HBITMAP hBmp, int mundoX, int mundoY)
{
    if (!hBmp)
        return;

    // 1. Crear un contexto de memoria para leer el BMP
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmp);

    // 2. Obtener dimensiones reales de la imagen
    BITMAP bm;
    GetObject(hBmp, sizeof(BITMAP), &bm);

    // 3. Escanear la imagen saltando de celda en celda (cada 16px)
    // Usamos TAMANO_CELDA_BASE (16) para ir rápido y coincidir con la rejilla
    for (int y = 0; y < bm.bmHeight; y += TAMANO_CELDA_BASE)
    {
        for (int x = 0; x < bm.bmWidth; x += TAMANO_CELDA_BASE)
        {

            // Leemos el píxel en el CENTRO de la celda para mayor precisión
            // (x + 8, y + 8)
            COLORREF color = GetPixel(hdcMem, x + (TAMANO_CELDA_BASE / 2), y + (TAMANO_CELDA_BASE / 2));

            // 4. LÓGICA DE DETECCIÓN
            // Si el color NO es Magenta (255, 0, 255), entonces es Tierra.
            if (color != RGB(255, 0, 255))
            {

                // Calcular posición en la Matriz Global
                int gridX = (mundoX + x) / TAMANO_CELDA_BASE;
                int gridY = (mundoY + y) / TAMANO_CELDA_BASE;

                // Validar límites y marcar
                if (gridY >= 0 && gridY < MUNDO_FILAS && gridX >= 0 && gridX < MUNDO_COLUMNAS)
                {
                    mapa[gridY][gridX] = 1; // 1 = TIERRA
                }
            }
        }
    }

    // Limpieza
    SelectObject(hdcMem, hOld);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS])
{
    // 1. Llenar todo de agua al principio
    for (int i = 0; i < MUNDO_FILAS; i++)
        for (int j = 0; j < MUNDO_COLUMNAS; j++)
            mapa[i][j] = 0;

    // 2. Escanear cada imagen de isla y "estamparla" en el mapa
    // Esto copia la forma exacta (sin el magenta) a la matriz de colisión

    // Isla Central
    if (misIslas[0].activa)
        generarColisionDesdeImagen(mapa, hBmpIslaGrande, misIslas[0].x, misIslas[0].y);

    // Isla Norte
    if (misIslas[1].activa)
        generarColisionDesdeImagen(mapa, hBmpIslaSec2, misIslas[1].x, misIslas[1].y);

    // Isla Sur
    if (misIslas[2].activa)
        generarColisionDesdeImagen(mapa, hBmpIslaSec4, misIslas[2].x, misIslas[2].y);

    // Isla Oeste
    if (misIslas[3].activa)
        generarColisionDesdeImagen(mapa, hBmpIslaSec1, misIslas[3].x, misIslas[3].y);

    // Isla Este
    if (misIslas[4].activa)
        generarColisionDesdeImagen(mapa, hBmpIslaSec3, misIslas[4].x, misIslas[4].y);
}

void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int mapaId)
{
    inicializarIslas(mapaId);
    inicializarMapa(mapa);
    inicializarArboles(mapa);
    inicializarVacas();
    inicializarMinas(mapa);
    inicializarTesoros(); 
    inicializarUnidades();

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
    jugador->nivelMochila = 1;
    jugador->modoTienda = 0;
    
    // Inicialización de inventario y recursos
    jugador->inventarioAbierto = 0;
    jugador->tieneEspada = 0;
    jugador->tienePico = 0;
    jugador->tieneArmadura = 0;
    jugador->tieneHacha = FALSE;
    jugador->armaduraEquipada = 0;
    jugador->madera = 0;
    jugador->piedra = 0;
    jugador->oro = 0;
    jugador->hierro = 0;
    jugador->comida = 0;
    jugador->hojas = 0;
    jugador->carne = 0;
    
    // Inicialización de contadores de unidades
    jugador->cantMineros = 0;
    jugador->cantLenadores = 0;
    jugador->cantCazadores = 0;
    jugador->cantSoldados = 0;
    jugador->cantHachas = 0;
    
    jugador->direccion = DIR_ABAJO;
    jugador->frameAnim = 0;
    jugador->pasoAnimacion = 0;
    jugador->frameDestello = 0;

    // Configuración inicial del estado
    estado->estadoActual = ESTADO_PARTIDA;
    estado->mapaSeleccionado = mapaId;
    estado->frameTienda = 0;
}
// --- FÍSICA Y MOVIMIENTO ---
int EsSuelo(int x, int y, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS])
{
    // Convertir pixel a celda
    int col = x / TAMANO_CELDA_BASE;
    int fila = y / TAMANO_CELDA_BASE;

    // Límites del mundo
    if (fila < 0 || fila >= MUNDO_FILAS || col < 0 || col >= MUNDO_COLUMNAS)
    {
        return 0; // Fuera es agua
    }

    return (mapa[fila][col] == 1);
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

    // Verificamos colisión en los PIES (centro abajo)
    int colisionX = futuraX + 16;
    int colisionY = futuraY + 30;

    if (EsSuelo(colisionX, colisionY, mapa))
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

// --- DIBUJADO DE SELECCIÓN DE MAPA ---
void dibujarSeleccionMapa(HDC hdc, HWND hwnd, EstadoJuego *estado)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    int ancho = rect.right;
    int alto = rect.bottom;

    // 1. Fondo de selección de mapa
    if (hBmpFondoSeleccionMapa)
    {
        BITMAP bm;
        GetObject(hBmpFondoSeleccionMapa, sizeof(BITMAP), &bm);
        HDC hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, hBmpFondoSeleccionMapa);
        SetStretchBltMode(hdc, HALFTONE);
        StretchBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        DeleteDC(hdcMem);
    }
    else
    {
        // Fondo por defecto
        HBRUSH fondo = CreateSolidBrush(RGB(30, 30, 50));
        FillRect(hdc, &rect, fondo);
        DeleteObject(fondo);
    }

    // 2. Título
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 215, 0));
    
    HFONT hTitulo = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, VARIABLE_PITCH, "Arial");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hTitulo);
    
    TextOut(hdc, ancho/2 - 150, 50, "SELECCIONA UN MAPA", 19);
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hTitulo);

    // 3. Configuración de los cuadros
    int cuadroAncho = 300;   // Ancho de tus cuadros
    int cuadroAlto = 200;    // Alto de tus cuadros
    int separacion = 50;     // Espacio entre cuadros
    int totalAncho = (cuadroAncho * 3) + (separacion * 2);
    int startX = (ancho - totalAncho) / 2;
    int startY = alto / 2 - 100;

    // 4. Dibujar los 3 cuadros
    for (int i = 0; i < 3; i++)
    {
        int posX = startX + (i * (cuadroAncho + separacion));
        int posY = startY;
        
        // --- DIBUJAR EL CUADRO COMPLETO SEGÚN EL ESTADO ---
        HBITMAP cuadroADibujar = NULL;
        
        if (estado->opcionSeleccionada == i)
        {
            // Cuadro seleccionado
            switch(i)
            {
                case 0: cuadroADibujar = hBmpCuadroMapa1Sel; break;
                case 1: cuadroADibujar = hBmpCuadroMapa2Sel; break;
                case 2: cuadroADibujar = hBmpCuadroMapa3Sel; break;
            }
        }
        else
        {
            // Cuadro normal
            switch(i)
            {
                case 0: cuadroADibujar = hBmpCuadroMapa1Normal; break;
                case 1: cuadroADibujar = hBmpCuadroMapa2Normal; break;
                case 2: cuadroADibujar = hBmpCuadroMapa3Normal; break;
            }
        }
        
        // Dibujar el cuadro completo
        if (cuadroADibujar != NULL)
        {
            // Dibujar el cuadro con su tamaño original
            DibujarImagen(hdc, cuadroADibujar, posX, posY, cuadroAncho, cuadroAlto);
        }
        else
        {
            // Placeholder si no hay imagen del cuadro
            HBRUSH placeholder;
            if (estado->opcionSeleccionada == i)
            {
                placeholder = CreateSolidBrush(RGB(100, 200, 100)); // Verde para seleccionado
            }
            else
            {
                placeholder = CreateSolidBrush(RGB(100, 100, 150)); // Azul para normal
            }
            
            RECT cuadroRect = {posX, posY, posX + cuadroAncho, posY + cuadroAlto};
            FillRect(hdc, &cuadroRect, placeholder);
            DeleteObject(placeholder);
            
            // Dibujar borde
            HPEN borderPen = CreatePen(PS_SOLID, 2, 
                (estado->opcionSeleccionada == i) ? RGB(255, 215, 0) : RGB(150, 150, 150));
            HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, posX, posY, posX + cuadroAncho, posY + cuadroAlto);
            SelectObject(hdc, oldPen);
            SelectObject(hdc, oldBrush);
            DeleteObject(borderPen);
            
            // Texto "Mapa X"
            SetTextColor(hdc, RGB(255, 255, 255));
            char textoMapa[20];
            sprintf(textoMapa, "Mapa %d", i + 1);
            TextOut(hdc, posX + cuadroAncho/2 - 30, posY + cuadroAlto/2 - 10, textoMapa, strlen(textoMapa));
        }

        // 5. Nombre del mapa (debajo del cuadro)
        SetTextColor(hdc, RGB(255, 255, 255));
        char nombreMapa[50];
        
        switch(i)
        {
            case 0:
                sprintf(nombreMapa, "MAPA 1: Isla Barbara");
                break;
            case 1:
                sprintf(nombreMapa, "MAPA 2: Isla Andrea");
                break;
            case 2:
                sprintf(nombreMapa, "MAPA 3: Isla Samuel");
                break;
        }
        
        // Centrar el nombre debajo del cuadro
        int textoAncho = strlen(nombreMapa) * 8; // Aproximación del ancho del texto
        TextOut(hdc, posX + (cuadroAncho - textoAncho)/2, posY + cuadroAlto + 10, nombreMapa, strlen(nombreMapa));
        
        // 6. Indicador "SELECCIONADO" (opcional, si no está en tu sprite)
        if (estado->opcionSeleccionada == i)
        {
            SetTextColor(hdc, RGB(255, 215, 0));
            TextOut(hdc, posX + cuadroAncho/2 - 40, posY + cuadroAlto + 30, "SELECCIONADO", 12);
        }
    }

    // 7. Instrucciones
    SetTextColor(hdc, RGB(200, 200, 200));
    TextOut(hdc, ancho/2 - 100, alto - 100, "Haz clic en un mapa para seleccionarlo", 38);
    TextOut(hdc, ancho/2 - 80, alto - 70, "ENTER para confirmar", 21);
    TextOut(hdc, ancho/2 - 80, alto - 40, "ESC para volver al menú", 23);
}

// --- PROCESAR CLIC EN SELECCIÓN DE MAPA ---
void procesarClickSeleccionMapa(int x, int y, HWND hwnd, EstadoJuego *estado)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    int ancho = rc.right;
    int alto = rc.bottom;

    // Configuración de los cuadros (DEBE COINCIDIR con dibujarSeleccionMapa)
    int cuadroAncho = 300;
    int cuadroAlto = 200;
    int separacion = 50;
    int totalAncho = (cuadroAncho * 3) + (separacion * 2);
    int startX = (ancho - totalAncho) / 2;
    int startY = alto / 2 - 100;

    // Verificar clic en cada cuadro
    for (int i = 0; i < 3; i++)
    {
        int posX = startX + (i * (cuadroAncho + separacion));
        int posY = startY;
        
        if (x >= posX && x <= posX + cuadroAncho && 
            y >= posY && y <= posY + cuadroAlto)
        {
            estado->opcionSeleccionada = i;
            InvalidateRect(hwnd, NULL, TRUE);
            return;
        }
    }
}

void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado)
{
    actualizarPuntoMenu(estado, x, y, hwnd);
    if (estado->opcionSeleccionada != -1)
    {
        switch (estado->opcionSeleccionada)
        {
        case 0: // JUGAR
            estado->estadoActual = ESTADO_SELECCION_MAPA;
            estado->opcionSeleccionada = 0; // Seleccionar primer mapa por defecto
            break;
        case 1: // PARTIDAS
            MessageBox(hwnd, "Guardado no disponible aun.", "Partidas", MB_OK);
            break;
        case 2: // INSTRUCCIONES
            MessageBox(hwnd, "WASD para moverte.", "Ayuda", MB_OK);
            break;
        case 3: // SALIR
            PostQuitMessage(0);
            return;
        }
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void procesarEnterMenu(HWND hwnd, EstadoJuego *estado)
{
    if (estado->estadoActual == ESTADO_MENU && estado->opcionSeleccionada == 0)
    {
        estado->estadoActual = ESTADO_SELECCION_MAPA;
        estado->opcionSeleccionada = 0; // Seleccionar primer mapa por defecto
        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void procesarClickMochila(int mouseX, int mouseY, Jugador *jugador, HWND hwnd)
{
    if (!jugador->inventarioAbierto)
        return;

    // Coordenadas base (Deben coincidir con dibujarHUD)
    int px = 90, py = 120;
    int startX = px + 20;
    int startY = py + 40;

    // Definimos cuántos slots revisar (hasta el 9 para incluir armadura)
    int totalSlots = 9;

    for (int i = 0; i < totalSlots; i++)
    {
        int col = i % 2;
        int row = i / 2;

        int itemX = startX + (col * 150);
        int itemY = startY + (row * 50);

        // Detectar clic en el ícono (32x32)
        if (mouseX >= itemX && mouseX <= itemX + 32 && mouseY >= itemY && mouseY <= itemY + 32)
        {

            // CASO 5: COMIDA
            if (i == 5 && jugador->comida > 0 && jugador->vidaActual < jugador->vidaMax)
            {
                jugador->vidaActual += 25;
                jugador->comida--;
                if (jugador->vidaActual > jugador->vidaMax)
                    jugador->vidaActual = jugador->vidaMax;
                InvalidateRect(hwnd, NULL, FALSE);
            }

            // CASO 8: ARMADURA (Equipar / Desequipar)
            else if (i == 8 && jugador->tieneArmadura)
            {
                jugador->armaduraEquipada = !jugador->armaduraEquipada; // Alternar estado

                if (jugador->armaduraEquipada)
                {
                    // Si se la pone, ajustamos los stats como pediste (5/10)
                    jugador->armaduraMax = 10;
                    jugador->armaduraActual = 5;
                }
                else
                {
                    // Si se la quita
                    jugador->armaduraActual = 0;
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
    }
}

// --- INTERACCIÓN TIENDA (COMPRA DE TROPAS RTS) ---
void procesarClickMochilaTienda(int mx, int my, int esClickDerecho, Jugador *j, HWND hwnd) {
    if (!j->inventarioAbierto) return;

    int tx = 450, ty = 120; 

    // 1. PESTAÑAS
    if (my >= ty - 40 && my <= ty + 10) { 
        if (mx >= tx && mx <= tx + 149) {
            j->modoTienda = 0; // COMPRAR
            InvalidateRect(hwnd, NULL, FALSE);
            return;
        }
        if (mx >= tx + 150 && mx <= tx + 300) {
            j->modoTienda = 1; // VENDER
            InvalidateRect(hwnd, NULL, FALSE);
            return;
        }
    }

    int startX = tx + 20;
    int startY = ty + 40;

    if (j->modoTienda == 0) { // --- MODO COMPRAR ---
        int cantidad = (esClickDerecho) ? 15 : 5;
        
        for (int i = 0; i < 6; i++) {
            int col = i % 2; 
            int row = i / 2;
            int ix = startX + (col * 140); 
            int iy = startY + (row * 65);

            // Detección estricta dentro del cuadro del ítem
            if (mx >= ix && mx <= ix + 130 && my >= iy && my <= iy + 60) {
                
                if (i == 0) { // Espada (Cazador)
                    if (j->oro >= 20 && j->hierro >= 3) {
                        j->oro -= 20; j->hierro -= 3;
                        spawnearEscuadron(TIPO_CAZADOR, cantidad, j->x + 50, j->y);
                    }
                }
                else if (i == 1) { // Pico (Minero)
                    if (j->oro >= 15 && j->piedra >= 5) {
                        j->oro -= 15; j->piedra -= 5;
                        spawnearEscuadron(TIPO_MINERO, cantidad, j->x + 50, j->y);

                    }
                }
                else if (i == 2) { // Hacha (Leñador)
                    if (j->oro >= 20 && j->madera >= 10) {
                        j->oro -= 20; j->madera -= 10;
                        spawnearEscuadron(TIPO_LENADOR, cantidad, j->x + 50, j->y);

                    }
                }
                else if (i == 3) { // Armadura (Soldado)
                    if (j->oro >= 50 && j->hierro >= 10) {
                        j->oro -= 50; j->hierro -= 10;
                        spawnearEscuadron(TIPO_SOLDADO, cantidad, j->x + 50, j->y);

                    }
                }

                InvalidateRect(hwnd, NULL, FALSE);
                return; // Salimos de la función inmediatamente al procesar el click
            }
        }
    }
   

    else { // --- MODO VENDER ---
        int precios[] = {1, 2, 5, 1, 3, 10}; 
        int* stocks[] = {&j->madera, &j->piedra, &j->hierro, &j->hojas, &j->comida, &j->oro};

        for (int i = 0; i < 6; i++) {
            int ix = startX + ((i % 2) * 140); 
            int iy = startY + ((i / 2) * 65);

            if (mx >= ix && mx <= ix + 130 && my >= iy && my <= iy + 60) {
                if (i < 5) { // No vendemos el slot 6 (oro)
                    int cantidadAVender = (esClickDerecho) ? 10 : 1;
                    if (*stocks[i] > 0) {
                        if (cantidadAVender > *stocks[i]) cantidadAVender = *stocks[i];
                        *stocks[i] -= cantidadAVender;
                        j->oro += (cantidadAVender * precios[i]);
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                }
                break;
            }
        }
    }
}

void dibujarMina(HDC hdc, Camera cam) {
    int screenX = (int)((CUEVA_X - cam.x) * cam.zoom);
    int screenY = (int)((CUEVA_Y - cam.y) * cam.zoom);
    int tam = (int)(128 * cam.zoom);

    if (hBmpMina) {
        DibujarImagen(hdc, hBmpMina, screenX, screenY, tam, tam);
        // Texto para confirmar ubicación
        SetTextColor(hdc, RGB(255, 215, 0));
        TextOut(hdc, screenX, screenY - 20, "MINA DE RECURSOS", 16);
    }
}

void inicializarUnidades() {
    for (int i = 0; i < MAX_UNIDADES; i++) {
        unidades[i].activa = 0;
        unidades[i].seleccionado = 0;
    }
}

// 2. CREAR ESCUADRÓN (Flexible: 5, 10 o 15)
void spawnearEscuadron(int tipo, int cantidad, int x, int y) {
    int creados = 0;
    char* nombre;
    switch(tipo) {
        case TIPO_MINERO:  nombre = "Mineros"; break;
        case TIPO_LENADOR: nombre = "Leñadores"; break;
        case TIPO_CAZADOR: nombre = "Cazadores"; break;
        case TIPO_SOLDADO: nombre = "Infantería"; break;
    }

    for (int i = 0; i < MAX_UNIDADES && creados < cantidad; i++) {
        if (!unidades[i].activa) {
            unidades[i].x = x + (creados * 10); // Espaciado pequeño
            unidades[i].y = y + (creados * 5);
            unidades[i].tipo = tipo;
            unidades[i].activa = 1;
            unidades[i].estado = ESTADO_IDLE;
            strcpy(unidades[i].nombreGrupo, nombre);
            creados++;
        }
    }
}

// 3. ACTUALIZAR (IA + MINERÍA)
void actualizarUnidades(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Jugador *j) {
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa) continue;

        // 1. Lógica de Mineros en la Cueva
        if (unidades[i].estado == ESTADO_EN_CUEVA) {
            unidades[i].timerTrabajo++;
            if (unidades[i].timerTrabajo >= 100) { 
                j->oro += 2;
                j->hierro += 1;
                j->piedra += 2;
                unidades[i].timerTrabajo = 0;
                
                // Opcional: Crear texto flotante de recursos aquí
            }
            continue; 
        }

        // 2. Movimiento suave hacia el destino
        if (unidades[i].estado == ESTADO_MOVIENDO) {
            float dx = unidades[i].destinoX - unidades[i].x;
            float dy = unidades[i].destinoY - unidades[i].y;
            float dist = sqrt(dx*dx + dy*dy);

            if (dist > 5) {
                unidades[i].x += (dx / dist) * 2.0f; 
                unidades[i].y += (dy / dist) * 2.0f;
                
                if (abs(dx) > abs(dy)) {
                    unidades[i].direccion = (dx > 0) ? DIR_DERECHA : DIR_IZQUIERDA;
                } else {
                    unidades[i].direccion = (dy > 0) ? DIR_ABAJO : DIR_ARRIBA;
                }
                
                unidades[i].contadorAnim++;
                if (unidades[i].contadorAnim > 10) {
                    unidades[i].frameAnim = (unidades[i].frameAnim + 1) % 3;
                    unidades[i].contadorAnim = 0;
                }
            } else {
                unidades[i].estado = ESTADO_IDLE;
            }
        }
        
        // 3. Lógica de Caza (Sincronizada con 'manada')
        if (unidades[i].estado == ESTADO_CAZANDO) {
            bool vacaEncontrada = false;
            for (int v = 0; v < MAX_VACAS; v++) {
                // IMPORTANTE: Usar 'manada' y verificar que esté viva (estadoVida == 0)
                if (manada[v].activa && manada[v].estadoVida == 0) {
                    vacaEncontrada = true;
                    float dist = sqrt(pow(unidades[i].x - manada[v].x, 2) + pow(unidades[i].y - manada[v].y, 2));
                    
                    if (dist < 35) { 
                        unidades[i].timerTrabajo += 1; 

                        if (unidades[i].timerTrabajo >= 100) { 
                            manada[v].estadoVida = 1;   // La vaca pasa a estado muerta
                            manada[v].tiempoMuerte = 300;
                            j->comida += 10;            
                            unidades[i].timerTrabajo = 0; 
                            
                            // Añadir texto flotante si lo tienes implementado:
                            // crearTextoFlotante("+10 Comida", manada[v].x, manada[v].y, RGB(255, 255, 255));
                        }
                    } else {
                        // Persecución
                        unidades[i].x += (manada[v].x > unidades[i].x) ? 1.4f : -1.4f;
                        unidades[i].y += (manada[v].y > unidades[i].y) ? 1.4f : -1.4f;
                        unidades[i].timerTrabajo = 0; 
                    }
                    break; 
                }
            }
            // Si no hay vacas vivas, el aldeano vuelve a IDLE
            if (!vacaEncontrada) unidades[i].estado = ESTADO_IDLE;
        }
    }
}

// 4. DIBUJAR (Nombres y Herramientas)
void dibujarUnidades(HDC hdc, Camera cam) {
    // El 'int i' debe ir dentro del for
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa || unidades[i].estado == ESTADO_EN_CUEVA) continue;

        // DECLARA LAS VARIABLES AQUÍ (Esto quita los errores rojos de tu imagen)
        int screenX = (int)((unidades[i].x - cam.x) * cam.zoom);
        int screenY = (int)((unidades[i].y - cam.y) * cam.zoom);
        int size = (int)(TAMANO_CELDA * cam.zoom);

        HBITMAP (*anim)[3]; // Declarar el puntero de animación
        
        switch(unidades[i].tipo) {
            case TIPO_MINERO:  anim = hBmpMineroAnim; break;
            case TIPO_LENADOR: anim = hBmpLenadorAnim; break;
            case TIPO_CAZADOR: anim = hBmpCazadorAnim; break;
            case TIPO_SOLDADO: anim = hBmpSoldadoAnim; break;
            default: anim = hBmpJugadorAnim; break;
        }

        DibujarImagen(hdc, anim[unidades[i].direccion][unidades[i].frameAnim], 
                     screenX, screenY, size, size);
                     
                     if (unidades[i].estado == ESTADO_CAZANDO && unidades[i].timerTrabajo > 0) {
    int anchoBarra = 40;
    int progreso = (unidades[i].timerTrabajo * anchoBarra) / 100;
    
    // Dibujamos un rectangulito verde sobre el aldeano
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
    RECT r = { screenX, screenY - 10, screenX + progreso, screenY - 6 };
    FillRect(hdc, &r, hBrush);
    DeleteObject(hBrush);
    
    if (unidades[i].seleccionado) {
    // Dibujar elipse verde en los pies
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
    SelectObject(hdc, hPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH)); // Sin relleno
    
    Ellipse(hdc, screenX, screenY + size - 10, screenX + size, screenY + size + 5);
    
    DeleteObject(hPen);
}
}
    }
}

// 5. CONTROL (Igual que antes pero actualizado)

void seleccionarUnidades(int mouseX, int mouseY, Camera cam) {
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa) continue;

        // Convertir posición del mundo a pantalla (igual que en el dibujo)
        int xEnPantalla = (int)((unidades[i].x - cam.x) * cam.zoom);
int yEnPantalla = (int)((unidades[i].y - cam.y) * cam.zoom);
int ancho = (int)(40 * cam.zoom); // El área de click debe crecer con el zoom

if (mouseX >= xEnPantalla && mouseX <= xEnPantalla + ancho &&
    mouseY >= yEnPantalla && mouseY <= yEnPantalla + ancho) {
    unidades[i].seleccionado = 1;
}
    }
}

// Dar orden de movimiento o acción
void ordenarUnidad(int mouseX, int mouseY, Camera cam, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    float mundoX = (mouseX / cam.zoom) + cam.x;
    float mundoY = (mouseY / cam.zoom) + cam.y;

    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (unidades[i].activa && unidades[i].seleccionado) {
            unidades[i].destinoX = (int)mundoX;
            unidades[i].destinoY = (int)mundoY;
            unidades[i].estado = ESTADO_MOVIENDO;
            
            // Si el clic es cerca de la cueva y es minero, entrar
            if (unidades[i].tipo == TIPO_MINERO) {
                float distCueva = sqrt(pow(mundoX - CUEVA_X, 2) + pow(mundoY - CUEVA_Y, 2));
                if (distCueva < 50) unidades[i].estado = ESTADO_EN_CUEVA;
            }
        }
    }
}

void actualizarVacasEstablo() {
    for (int i = 0; i < MAX_VACAS; i++) {
        if (!vacas[i].activa) continue;

        // Movimiento aleatorio muy pequeño (vagar)
        vacas[i].x += (rand() % 3 - 1); 
        vacas[i].y += (rand() % 3 - 1);

        // Si se salen del establo, las devolvemos suavemente
        if (vacas[i].x < ESTABLO_X - RADIO_ESTABLO) vacas[i].x++;
        if (vacas[i].x > ESTABLO_X + RADIO_ESTABLO) vacas[i].x--;
        if (vacas[i].y < ESTABLO_Y - RADIO_ESTABLO) vacas[i].y++;
        if (vacas[i].y > ESTABLO_Y + RADIO_ESTABLO) vacas[i].y--;
    }
}

// 1. INICIALIZAR (Posiciones variadas y setup de patrulla)
void inicializarVacas()
{
    for (int i = 0; i < MAX_VACAS; i++)
        manada[i].activa = 0;

    for (int i = 0; i < 7; i++)
    {
        manada[i].activa = 1;

        // --- LÓGICA DE AGRUPAMIENTO NATURAL ---
        // Definimos un área central en el establo
        int basePoolX = ESTABLO_X + 1;
        int basePoolY = ESTABLO_Y + 1;

        // Distribución por "parejas" o grupos pequeños usando el índice i
        // Las vacas con i par e impar se mantienen cerca en el eje Y
        manada[i].x = basePoolX + (rand() % 150); // Se esparcen en un ancho de 150px
        manada[i].y = basePoolY + ( (i / 2) * 40 ) + (rand() % 30); 

        // Un pequeño truco: si son las vacas 0 y 1, o 3 y 4, 
        // les damos un empujón extra para que se encimen un poco
        if (i % 2 == 0) {
            manada[i].x += rand() % 15;
            manada[i].y -= rand() % 10;
        }

        // --- DIRECCIONES Y PATRULLA ---
        manada[i].xInicial = manada[i].x; 
        manada[i].direccion = (rand() % 2 == 0) ? 1 : -1;
        
        manada[i].vida = 5;
        manada[i].estadoVida = 0; 
        manada[i].tiempoMuerte = 300;

        // Animación según dirección inicial
        manada[i].frameAnim = (manada[i].direccion == 1) ? 4 : 0;
    }
}

// 2. ACTUALIZAR (Lógica de 100px y Animación Dividida)
void actualizarVacas(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS])
{
    for (int i = 0; i < MAX_VACAS; i++)
    {
        if (!manada[i].activa)
            continue;

        // SI ESTÁ MUERTA, QUIETA
        if (manada[i].estadoVida == 1)
        {
            manada[i].tiempoMuerte--;
            if (manada[i].tiempoMuerte <= 0)
                manada[i].activa = 0;
            continue;
        }

        // --- LÓGICA DE PATRULLA (100 PX) ---
        int velocidad = 1; // Velocidad de la vaca

        if (manada[i].direccion == 1)
        {
            // CAMINANDO A LA DERECHA
            manada[i].x += velocidad;

            // Si se aleja 100px del inicio, dar vuelta
            if (manada[i].x >= manada[i].xInicial + 30)
            {
                manada[i].direccion = -1; // Cambiar a Izquierda
                manada[i].frameAnim = 0;  // Resetear animación a set de izquierda
            }
        }
        else
        {
            // CAMINANDO A LA IZQUIERDA
            manada[i].x -= velocidad;

            // Si vuelve al inicio (o se pasa), dar vuelta
            if (manada[i].x <= manada[i].xInicial)
            {
                manada[i].direccion = 1; // Cambiar a Derecha
                manada[i].frameAnim = 4; // Resetear animación a set de derecha
            }
        }

        // --- ANIMACIÓN SEPARADA (0-3 IZQ, 4-7 DER) ---
        manada[i].contadorAnim++;
        if (manada[i].contadorAnim > 8)
        { // Ajusta este número para velocidad de patas
            manada[i].frameAnim++;

            if (manada[i].direccion == 1)
            {
                // Rango Derecha: 4, 5, 6, 7 -> vuelve a 4
                if (manada[i].frameAnim > 7)
                    manada[i].frameAnim = 4;
            }
            else
            {
                // Rango Izquierda: 0, 1, 2, 3 -> vuelve a 0
                if (manada[i].frameAnim > 3)
                    manada[i].frameAnim = 0;
            }

            manada[i].contadorAnim = 0;
        }
    }
}

// 3. GOLPEAR (Usando estadoVida)
void golpearVaca(Jugador *j)
{
    int rango = 60;
    int jx = j->x + 16;
    int jy = j->y + 16;

    for (int i = 0; i < MAX_VACAS; i++)
    {
        if (!manada[i].activa)
            continue;
        if (manada[i].estadoVida == 1)
            continue; // No golpear cadáveres

        int vx = manada[i].x + 16;
        int vy = manada[i].y + 16;

        if (abs(jx - vx) < rango && abs(jy - vy) < rango)
        {
            manada[i].vida--;
            if (rand() % 2 == 0) {
            crearChispas(manada[i].x + 16, manada[i].y + 16, RGB(200, 0, 0)); // Rojo
        } else {
            crearChispas(manada[i].x + 16, manada[i].y + 16, RGB(240, 240, 240)); // Blanco/Gris
        }

            if (manada[i].vida <= 0)
            {
                manada[i].estadoVida = 1; // <--- CAMBIO A MUERTA

                int comidaGanada = 7 + (rand() % 4);
                j->comida += comidaGanada;
				crearTextoFlotante(vx, vy, "Comida", comidaGanada, RGB(255, 100, 100));
            }
            return;
        }
    }
}

// 4. DIBUJAR (Mostrar imagen muerta si estadoViva es 1)
void dibujarVacas(HDC hdc, Camera cam, int ancho, int alto)
{
    for (int i = 0; i < MAX_VACAS; i++)
    {
        if (!manada[i].activa)
            continue;

        int sx = (manada[i].x - cam.x) * cam.zoom;
        int sy = (manada[i].y - cam.y) * cam.zoom;
        int tam = 32 * cam.zoom;

        if (sx > -tam && sx < ancho && sy > -tam && sy < alto)
        {

            // --- CORRECCIÓN VISUAL ---
            if (manada[i].estadoVida == 1)
            {
                // DIBUJAR MUERTA
                if (hBmpVacaMuerta)
                    DibujarImagen(hdc, hBmpVacaMuerta, sx, sy, tam, tam);
            }
            else
            {
                // DIBUJAR VIVA
                if (hBmpVaca[manada[i].frameAnim])
                    DibujarImagen(hdc, hBmpVaca[manada[i].frameAnim], sx, sy, tam, tam);
            }
        }
    }
}

void dibujarEstablo(HDC hdc, Camera cam) {  // Quitar el parámetro mapaId
    int screenX = (int)((ESTABLO_X - cam.x) * cam.zoom);
    int screenY = (int)((ESTABLO_Y - cam.y) * cam.zoom);
    int tam = (int)(200 * cam.zoom);

    if (hBmpEstablo != NULL) {
        DibujarImagen(hdc, hBmpEstablo, screenX, screenY, tam, tam);
    }
}

// 1. INICIALIZAR ÁRBOLES (Aleatorio inteligente)
void inicializarArboles(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    int contador = 0;
    int intentos = 0;
    
    // --- CONFIGURACIÓN DE ESPACIO ---
    int distMinima = 150; // <--- AUMENTA ESTO PARA SEPARARLOS MÁS (Píxeles)
    
    // Límites manuales de la Isla Central
    int minX = 1200; 
    int maxX = 2000; 
    int minY = 1200; 
    int maxY = 2000; 

    // 1. Limpiar el array
    for(int i=0; i<MAX_ARBOLES; i++) misArboles[i].activa = 0;

    // 2. Bucle de colocación
    // Aumentamos mucho los intentos (50000) porque al pedir separación es más difícil encontrar hueco
    while (contador < MAX_ARBOLES && intentos < 50000) {
        intentos++;

        // Generar coordenada dentro de la zona manual
        int rx = minX + (rand() % (maxX - minX));
        int ry = minY + (rand() % (maxY - minY));
        int tipo = rand() % 2; 

        // PASO A: VALIDAR SUELO (Tu lógica de doble piso)
        int sueloValido = 0;
        if (tipo == 0) { 
            if (EsSuelo(rx + 5, ry + 30, mapa) && EsSuelo(rx + 25, ry + 30, mapa)) sueloValido = 1;
        } else { 
            if (EsSuelo(rx + 20, ry + 60, mapa) && EsSuelo(rx + 44, ry + 60, mapa)) sueloValido = 1;
        }

        if (!sueloValido) continue; // Si toca agua, probar otro

        // PASO B: VALIDAR DISTANCIA (Nueva lógica de reparto)
        int muyCerca = 0;
        for(int i=0; i<contador; i++) {
            // Calculamos distancia con Pitágoras
            float dx = rx - misArboles[i].x;
            float dy = ry - misArboles[i].y;
            float distancia = sqrt(dx*dx + dy*dy);

            if (distancia < distMinima) {
                muyCerca = 1; 
                break; // ¡Alto! Está muy pegado a uno existente
            }
        }

        if (muyCerca) continue; // Si está cerca, probar otro lugar

        // PASO C: GUARDAR (Si pasó todas las pruebas)
        misArboles[contador].x = rx;
        misArboles[contador].y = ry;
        misArboles[contador].tipo = tipo;
        misArboles[contador].activa = 1;
        misArboles[contador].vida = 5; 
        contador++;
    }
}

// 2. DIBUJAR ÁRBOLES (Con selección de imagen por mapa)
void dibujarArboles(HDC hdc, Camera cam, int ancho, int alto, int mapaId)
{
    for (int i = 0; i < MAX_ARBOLES; i++)
    {
        if (!misArboles[i].activa)
            continue;

        // Calcular posición en pantalla
        int sx = (misArboles[i].x - cam.x) * cam.zoom;
        int sy = (misArboles[i].y - cam.y) * cam.zoom;

        // El árbol grande mide el doble (64px) aprox, el chico 32px
        int tamBase = 32 * cam.zoom;
        if (misArboles[i].tipo == 1)
            tamBase = 64 * cam.zoom;

        // Optimización: Solo dibujar si está en pantalla
        if (sx > -tamBase && sx < ancho && sy > -tamBase && sy < alto)
        {
            HBITMAP img = NULL;
            
            // SELECCIONAR IMAGEN SEGÚN MAPA
            switch(mapaId) {
                case 0: // Mapa 1
                    img = (misArboles[i].tipo == 1) ? hBmpArbolGrande : hBmpArbolChico;
                    break;
                case 1: // Mapa 2
                    img = (misArboles[i].tipo == 1) ? hBmpArbolGrandeMapa2 : hBmpArbolChicoMapa2;
                    break;
                case 2: // Mapa 3
                    img = (misArboles[i].tipo == 1) ? hBmpArbolGrandeMapa3 : hBmpArbolChicoMapa3;
                    break;
            }
            
            if (img)
                DibujarImagen(hdc, img, sx, sy, tamBase, tamBase);
        }
    }
}

// --- FUNCIÓN DE TALAR ACTUALIZADA ---
void talarArbol(Jugador *j)
{
    int rango = 40; // Distancia para poder golpear (píxeles)

    // Centro del jugador
    int jx = j->x + 16;
    int jy = j->y + 16;

    for (int i = 0; i < MAX_ARBOLES; i++)
    {
        if (!misArboles[i].activa)
            continue;

        // Calcular centro del árbol (aprox)
        int tam = (misArboles[i].tipo == 1) ? 64 : 32;
        int ax = misArboles[i].x + (tam / 2);
        int ay = misArboles[i].y + (tam / 2);

        // Verificar distancia (Si está cerca)
        if (abs(jx - ax) < rango && abs(jy - ay) < rango)
        {
            // 1. Restar vida
            misArboles[i].vida--;
			if (rand() % 2 == 0) {
    	crearChispas(ax, ay, RGB(139, 69, 19)); // Marrón Madera
			} else {
    	crearChispas(ax, ay, RGB(34, 139, 34));  // Verde Hojas
			}
            // 2. ¿Se cayó el árbol?
            if (misArboles[i].vida <= 0)
            {
            	int maderaGanadas = (misArboles[i].tipo == 1) ? 5 : 3;
   				int hojasGanadass = (misArboles[i].tipo == 1) ? 10 : 4;
                misArboles[i].activa = 0; // El árbol desaparece

                // 3. DAR RECOMPENSA (Madera y HOJAS)
                int maderaGanada = 0;
                int hojasGanadas = 0;

                if (misArboles[i].tipo == 1) { // Árbol Grande
                    maderaGanada = 5;
                    hojasGanadas = 10; // El grande da más hojas
                } else { // Árbol Chico
                    maderaGanada = 3;
                    hojasGanadas = 4;  // El chico da menos
                }

                // Sumar al inventario del jugador
                j->madera += maderaGanada;
                j->hojas += hojasGanadas; // <--- ¡AQUÍ SE SUMAN LAS HOJAS!
				crearTextoFlotante(ax, ay, "Madera", maderaGanadas, RGB(150, 75, 0));
    			crearTextoFlotante(ax, ay - 15, "Hojas", hojasGanadass, RGB(50, 200, 50));
                
            }

            return; // Solo golpeamos un árbol a la vez
        }
    }
}
// 1. INICIALIZAR TESOROS (Escondidos detrás de árboles)
void inicializarTesoros()
{
    // Tesoro 1: TIPO ORO (0)
    // Escondido "detrás" de un árbol (Y menor que el árbol para que el árbol lo tape si dibujas el árbol después)
    // O simplemente cerca de coordenadas boscosas.
    misTesoros[0].x = 1320;
    misTesoros[0].y = 1250;
    misTesoros[0].tipo = 0;   // Solo Oro
    misTesoros[0].estado = 0; // Cerrado
    misTesoros[0].activa = 1;

    // Tesoro 2: TIPO JOYAS/HIERRO (1)
    misTesoros[1].x = 1850;
    misTesoros[1].y = 1550;
    misTesoros[1].tipo = 1;   // Oro + Hierro
    misTesoros[1].estado = 0; // Cerrado
    misTesoros[1].activa = 1;
}

// --- 2. ABRIR TESORO (Lógica de 2 Pasos) ---
void abrirTesoro(Jugador *j)
{
    int rango = 60; // Distancia para interactuar
    int jx = j->x + 16;
    int jy = j->y + 16;

    for (int i = 0; i < MAX_TESOROS; i++)
    {
        if (!misTesoros[i].activa)
            continue;

        // Si ya está vacío (Estado 2), no hacemos nada
        if (misTesoros[i].estado == 2)
            continue;

        int tx = misTesoros[i].x + 16;
        int ty = misTesoros[i].y + 16;

        if (abs(jx - tx) < rango && abs(jy - ty) < rango)
        {

            // PASO 1: ABRIR EL COFRE
            if (misTesoros[i].estado == 0)
            {
                misTesoros[i].estado = 1; // Pasa a "Abierto con cosas"
                return;                   // Terminamos por este frame
            }

            // PASO 2: TOMAR EL LOOT
            if (misTesoros[i].estado == 1)
            {
                // Generar Loot Random
                int oroGanado = 30 + (rand() % 11); // 30-40

                if (misTesoros[i].tipo == 0)
                {
                    // Solo Oro
                    j->oro += oroGanado;
                    crearTextoFlotante(tx, ty, "Oro", oroGanado, RGB(255, 255, 0));
                }
                else
                {
                    // Oro + Hierro (Joyas)
                    int hierroGanado = 15 + (rand() % 11); // 15-25
                    j->oro += oroGanado;
                    j->hierro += hierroGanado;
    				crearTextoFlotante(tx, ty, "Oro", 5, RGB(180, 180, 180));
    				crearTextoFlotante(tx, ty, "Hierro", 3, RGB(255, 215, 0));
					}
                }

                misTesoros[i].estado = 2; // Pasa a "Vacío"
                return;
            }
        }
    }


// --- 3. DIBUJAR TESOROS CON TEXTO ---
void dibujarTesoros(HDC hdc, Camera cam, int ancho, int alto)
{
    // Calculamos la posición aproximada del jugador (centro de pantalla)
    // para saber si mostrar el mensaje.
    int jugadorScreenX = ancho / 2;
    int jugadorScreenY = alto / 2;

    for (int i = 0; i < MAX_TESOROS; i++)
    {
        if (!misTesoros[i].activa)
            continue;

        int sx = (misTesoros[i].x - cam.x) * cam.zoom;
        int sy = (misTesoros[i].y - cam.y) * cam.zoom;
        int tam = 32 * cam.zoom;

        // Solo dibujar si está en pantalla
        if (sx > -tam && sx < ancho && sy > -tam && sy < alto)
        {
            HBITMAP img = NULL;

            // SELECCIÓN DE IMAGEN SEGÚN ESTADO
            if (misTesoros[i].estado == 0)
            {
                img = hBmpTesoroCerrado;
            }
            else if (misTesoros[i].estado == 1)
            {
                // Está abierto pero lleno: mostramos Oro o Joyas
                if (misTesoros[i].tipo == 0)
                    img = hBmpTesoroOro;
                else
                    img = hBmpTesoroJoyas;
            }
            else
            {
                img = hBmpTesoroVacio; // Ya lo tomaste
            }

            if (img)
                DibujarImagen(hdc, img, sx, sy, tam, tam);

            int dist = sqrt(pow(sx + (tam / 2) - jugadorScreenX, 2) + pow(sy + (tam / 2) - jugadorScreenY, 2));

            if (dist < 100 && misTesoros[i].estado < 2)
            { // 100px de radio en pantalla
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, RGB(255, 255, 0)); // Amarillo

                char mensaje[64];
                if (misTesoros[i].estado == 0)
                {
                    sprintf(mensaje, "ESPACIO: Abrir Tesoro");
                }
                else
                {
                    sprintf(mensaje, "ESPACIO: Tomar Objetos");
                }

                // Dibujar texto centrado sobre el cofre
                TextOut(hdc, sx - 20, sy - 20, mensaje, strlen(mensaje));
            }
        }
    }
}


void inicializarMinas(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    int contador = 0;
    int intentos = 0;
    int minX = 1200, maxX = 2000, minY = 1200, maxY = 2000;

    for(int i = 0; i < MAX_MINAS; i++) misMinas[i].activa = 0;

    while (contador < MAX_MINAS && intentos < 10000) {
        intentos++;
        int rx = minX + (rand() % (maxX - minX));
        int ry = minY + (rand() % (maxY - minY));
        
        // Validar suelo (usando tu función EsSuelo)
        if (!EsSuelo(rx + 16, ry + 16, mapa)) continue;

        // Validar distancia mínima entre minas (ej. 100 píxeles)
        int muyCerca = 0;
        for(int i = 0; i < contador; i++) {
            float dist = sqrt(pow(rx - misMinas[i].x, 2) + pow(ry - misMinas[i].y, 2));
            if (dist < 100) { muyCerca = 1; break; }
        }
        
        if (muyCerca) continue;

        misMinas[contador].x = rx;
        misMinas[contador].y = ry;
        misMinas[contador].tipo = rand() % 2; // Piedra o Hierro
        misMinas[contador].vida = 5;          // 5 toques requeridos
        misMinas[contador].activa = 1;
        contador++;
    }
}
void picarMina(Jugador *j) {
    // Rango muy amplio para asegurar que detecte al jugador
    float rango = 40.0f; 
    
    // Centro del jugador
    float jx = j->x + 16.0f;
    float jy = j->y + 16.0f;

    for (int i = 0; i < MAX_MINAS; i++) {
        if (!misMinas[i].activa) continue;

        // Centro de la mina (basado en el sprite de 48px)
        float mx = misMinas[i].x + 24.0f;
        float my = misMinas[i].y + 24.0f;

        // Cálculo de distancia real
        float dx = jx - mx;
        float dy = jy - my;
        float distancia = sqrt(dx * dx + dy * dy);

        // Debug: Si quieres ver la distancia en consola desenta la línea de abajo
        // printf("Distancia a mina %d: %.2f\n", i, distancia);

        if (distancia <= rango) {
            // COMENTAMOS EL REQUISITO DEL PICO PARA PRUEBAS:
            // if (j->tienePico) { 
                
                misMinas[i].vida--;

                // Lanzar chispas: Gris para piedra, Dorado para hierro
                COLORREF color = (misMinas[i].tipo == 0) ? RGB(200, 200, 200) : RGB(255, 215, 0);
                crearChispas(mx, my, color);

                if (misMinas[i].vida <= 0) {
                    misMinas[i].activa = 0;
                    if (misMinas[i].tipo == 0) j->piedra += 5; // Recompensa Piedra
                    else j->hierro += 3;                      // Recompensa Hierro
				if (misMinas[i].tipo == 0) {
    				
    			crearTextoFlotante(mx, my, "Piedra", 5, RGB(180, 180, 180));
				} else {
    			
    			crearTextoFlotante(mx, my, "Hierro", 3, RGB(255, 215, 0));
					}
                }
                return; 
            // }
        }
    }
}

void dibujarMinas(HDC hdc, Camera cam, int ancho, int alto) {
    for (int i = 0; i < MAX_MINAS; i++) {
        if (!misMinas[i].activa) continue;

        int sx = (misMinas[i].x - cam.x) * cam.zoom;
        int sy = (misMinas[i].y - cam.y) * cam.zoom;
        int tam = 32 * cam.zoom;

        if (sx > -tam && sx < ancho && sy > -tam && sy < alto) {
            HBITMAP img = (misMinas[i].tipo == 1) ? hBmpHierroPicar : hBmpPiedraPicar;
            if (img) DibujarImagen(hdc, img, sx, sy, tam, tam);
        }
    }
}

void crearChispas(int x, int y, COLORREF color) {
    int creadas = 0;
    for (int i = 0; i < MAX_PARTICULAS && creadas < 8; i++) {
        if (!chispas[i].activo) {
            chispas[i].x = x;
            chispas[i].y = y;
            // Velocidad aleatoria en todas direcciones
            chispas[i].vx = (rand() % 11 - 5); 
            chispas[i].vy = (rand() % 11 - 8); // Saltan más hacia arriba
            chispas[i].vida = 15 + (rand() % 10);
            chispas[i].color = color;
            chispas[i].activo = 1;
            creadas++;
        }
    }
}

void actualizarYDibujarParticulas(HDC hdc, Camera cam) {
    for (int i = 0; i < MAX_PARTICULAS; i++) {
        if (chispas[i].activo) {
            // Movimiento físico simple
            chispas[i].x += chispas[i].vx;
            chispas[i].y += chispas[i].vy;
            chispas[i].vy += 0.4f; // Gravedad
            chispas[i].vida--;

            if (chispas[i].vida <= 0) {
                chispas[i].activo = 0;
                continue;
            }

            // Convertir a coordenadas de pantalla
            int sx = (int)((chispas[i].x - cam.x) * cam.zoom);
            int sy = (int)((chispas[i].y - cam.y) * cam.zoom);

            // Solo dibujar si está en el área visible
            HBRUSH hBr = CreateSolidBrush(chispas[i].color);
            RECT r = { sx, sy, sx + 4, sy + 4 };
            FillRect(hdc, &r, hBr);
            DeleteObject(hBr);
        }
    }
}

void crearUnidadEnMapa(int tipo) {
    // Buscamos un slot libre en el array de unidades
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa) {
            unidades[i].activa = 1;
            unidades[i].tipo = tipo;
            
            // Aparece cerca del jugador (con un pequeño desfase aleatorio)
            unidades[i].x = miJugador.x + (rand() % 40 - 20);
            unidades[i].y = miJugador.y + (rand() % 40 - 20);
            
            unidades[i].vida = 100;
            unidades[i].estado = 0; // Estado inicial (ej. quieto o siguiendo)
            unidades[i].frameAnim = 0;
            
            // Si es un soldado, podrías darle más vida
            if (tipo == TIPO_SOLDADO) {
                unidades[i].vida = 200;
            }

            return; // Salimos tras crear la unidad
        }
    }
    // Opcional: Si llegamos aquí, el array está lleno
    // MessageBox(NULL, "Población máxima alcanzada", "Aviso", MB_OK);
}
void reclutarTrabajador(Jugador *j, int tipo) {
    switch(tipo) {
        case 0: // Cazador
            if(j->oro >= 20) { 
                j->oro -= 20; j->cantCazadores++; j->tieneEspada = TRUE; 
                crearUnidadEnMapa(TIPO_CAZADOR); 
            } break;
        case 1: // Minero
            if(j->oro >= 15) { 
                j->oro -= 15; j->cantMineros++; j->tienePico = TRUE; 
                crearUnidadEnMapa(TIPO_MINERO);
            } break;
        case 2: // Leñador
            if(j->oro >= 10) { 
                j->oro -= 10; j->cantLenadores++; j->tieneHacha = TRUE; 
                crearUnidadEnMapa(TIPO_LENADOR);
            } break;
        case 3: // SOLDADO (Nuevo)
            // El soldado podría pedir Oro y Hierro para ser más balanceado
            if(j->oro >= 40 && j->hierro >= 10) { 
                j->oro -= 40; 
                j->hierro -= 10;
                j->cantSoldados++; 
                j->tieneArmadura = TRUE; // Al comprar soldados, desbloqueas visualmente la armadura
                crearUnidadEnMapa(TIPO_SOLDADO);
            } break;
    }

    // --- LÓGICA DE NIVEL DE MOCHILA ---
    // Nivel 2: Necesitas el set básico de herramientas (Espada, Pico, Hacha)
    if(j->nivelMochila == 1 && j->tieneEspada && j->tienePico && j->tieneHacha) {
        j->nivelMochila = 2;
        MessageBox(NULL, "Mochila Nivel 2! Has equipado las herramientas basicas.", "LOGRO", MB_OK | MB_ICONINFORMATION);
    }
    
    // Nivel 3: Se desbloquea al reclutar a tu primer Soldado (opcional)
    if(j->nivelMochila == 2 && j->cantSoldados > 0) {
        j->nivelMochila = 3;
        MessageBox(NULL, "Mochila Nivel 3! La armada te respalda.", "LOGRO", MB_OK | MB_ICONINFORMATION);
    }
}
// --- DIBUJADO DE TIENDAS ---
void dibujarTiendasEnIslas(HDC hdc, Camera cam, int ancho, int alto, int frameTienda)
{
    // Misma lógica de tiendas
    int tiendaMundoX = 1450;
    int tiendaMundoY = 1900;
    int tx = (tiendaMundoX - cam.x) * cam.zoom;
    int ty = (tiendaMundoY - cam.y) * cam.zoom;
    int tam = 50 * cam.zoom;
    int f = (frameTienda / 20) % 2;

    if (tx + tam >= 0 && tx - tam <= ancho && ty + tam >= 0 && ty - tam <= alto)
    {
        if (hBmpTienda[f] != NULL)
            DibujarImagen(hdc, hBmpTienda[f], tx - (tam / 2), ty - (tam / 2), tam, tam);
    }
}

void dibujarPrecio(HDC hdc, int x, int y, const char *costo1, const char *costo2, BOOL alcanzable)
{
    SetBkMode(hdc, TRANSPARENT);
    // Color: Verde si alcanza, Rojo si no
    if (alcanzable)
        SetTextColor(hdc, RGB(50, 255, 50));
    else
        SetTextColor(hdc, RGB(255, 50, 50));

    // Dibujar Línea 1 (Ej: "20 Oro")
    TextOut(hdc, x, y, costo1, strlen(costo1));
    // Dibujar Línea 2 justo debajo (Ej: "3 Hierro")
    TextOut(hdc, x, y + 12, costo2, strlen(costo2));
}
void venderItem(Jugador *j, int indice) {
    int precios[] = {1, 2, 5, 1, 3, 8};
    switch(indice) {
        case 0: if(j->madera > 0) { j->madera--; j->oro += precios[0]; } break;
        case 1: if(j->piedra > 0) { j->piedra--; j->oro += precios[1]; } break;
        case 2: if(j->hierro > 0) { j->hierro--; j->oro += precios[2]; } break;
        case 3: if(j->hojas > 0)  { j->hojas--;  j->oro += precios[3]; } break;
        case 4: if(j->comida > 0) { j->comida--; j->oro += precios[4]; } break;
        case 5: if(j->cantHachas > 0) { j->cantHachas--; j->oro += precios[5]; } break;
    }
}
void dibujarTiendaInteractiva(HDC hdc, Jugador *j)
{
    int tx = 450, ty = 120; // Posición base de la tienda

    // 0. Detectar Mouse (Para efectos de Hover)
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(WindowFromDC(hdc), &p);
    int mx = p.x;
    int my = p.y;

    // 1. DIBUJAR PESTAÑAS CON EFECTO HOVER
    // Pestaña COMPRAR
    BOOL hoverC = (mx >= tx && mx <= tx + 150 && my >= ty - 40 && my <= ty);
    COLORREF colorC = (j->modoTienda == 0) ? RGB(80, 80, 90) : (hoverC ? RGB(60, 60, 70) : RGB(35, 35, 40));
    HBRUSH bComprar = CreateSolidBrush(colorC);
    RECT rC = {tx, ty - 35, tx + 150, ty}; // ty - 35 para que sea una barra gruesa
    FillRect(hdc, &rC, bComprar);
    DeleteObject(bComprar);

    // Pestaña VENDER
    BOOL hoverV = (mx >= tx + 150 && mx <= tx + 300 && my >= ty - 40 && my <= ty);
    COLORREF colorV = (j->modoTienda == 1) ? RGB(80, 80, 90) : (hoverV ? RGB(60, 60, 70) : RGB(35, 35, 40));
    HBRUSH bVender = CreateSolidBrush(colorV);
    RECT rV = {tx + 150, ty - 35, tx + 300, ty};
    FillRect(hdc, &rV, bVender);
    DeleteObject(bVender);

    // Textos de las pestañas
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 215, 0)); // Dorado
    TextOut(hdc, tx + 40, ty - 25, "COMPRAR", 7);
    TextOut(hdc, tx + 195, ty - 25, "VENDER", 6);

    // 2. FONDO PRINCIPAL DE LA TIENDA
    HBRUSH fondo = CreateSolidBrush(RGB(40, 40, 45));
    RECT rMain = {tx, ty, tx + 300, ty + 350};
    FillRect(hdc, &rMain, fondo);
    DeleteObject(fondo);
    
    HBRUSH borde = CreateSolidBrush(RGB(255, 255, 255));
    FrameRect(hdc, &rMain, borde);
    DeleteObject(borde);

    // 3. CONTENIDO DE LA REJILLA
    int startX = tx + 20;
    int startY = ty + 40;

    if (j->modoTienda == 0) // --- MODO COMPRAR ---
    {
        for (int i = 0; i < 6; i++) 
        {
            int col = i % 2;
            int row = i / 2;
            int x = startX + (col * 140);
            int y = startY + (row * 65);

            // Efecto Hover en el Item
            if (mx >= x && mx <= x + 130 && my >= y && my <= y + 60) {
                HBRUSH hHover = CreateSolidBrush(RGB(60, 60, 70));
                RECT rH = {x - 5, y - 5, x + 135, y + 60};
                FillRect(hdc, &rH, hHover);
                DeleteObject(hHover);
            }

            HBITMAP icono = NULL;
            char costo1[32] = "", costo2[32] = "";
            BOOL alcanzable = FALSE, comprado = FALSE;

            switch (i) {
                case 0: icono = hBmpIconoEspada; sprintf(costo1, "20 Oro"); sprintf(costo2, "3 Hierro");
                        if (j->tieneEspada) comprado = TRUE; else if (j->oro >= 20 && j->hierro >= 3) alcanzable = TRUE; break;
                case 1: icono = hBmpIconoPico; sprintf(costo1, "15 Oro"); sprintf(costo2, "5 Piedra");
                        if (j->tienePico) comprado = TRUE; else if (j->oro >= 15 && j->piedra >= 5) alcanzable = TRUE; break;
                case 2: icono = hBmpIconoHacha; sprintf(costo1, "10 Oro"); sprintf(costo2, "10 Mad.");
                        if (j->tieneHacha) comprado = TRUE; else if (j->oro >= 10 && j->madera >= 10) alcanzable = TRUE; break;
                case 3: icono = hBmpIconoArmaduraInv; sprintf(costo1, "40 Oro"); sprintf(costo2, "10 Hierro");
                        if (j->tieneArmadura) comprado = TRUE; else if (j->oro >= 40 && j->hierro >= 10) alcanzable = TRUE; break;
                case 4: icono = hBmpInvCerrado; sprintf(costo1, "20 Oro"); sprintf(costo2, "30 Hojas");
                        if (j->nivelMochila >= 2) comprado = TRUE; else if (j->oro >= 20 && j->hojas >= 30) alcanzable = TRUE; break;
                case 5: icono = hBmpInvCerrado; 
                        if (j->nivelMochila >= 3) comprado = TRUE;
                        else if (j->nivelMochila == 2) { 
                            sprintf(costo1, "50 Oro"); sprintf(costo2, "10 Hierro");
                            if (j->oro >= 50 && j->hierro >= 10) alcanzable = TRUE;
                        } else sprintf(costo1, "Req. Nivel 2"); break;
            }

            if (icono) DibujarImagen(hdc, icono, x, y, 32, 32);
            if (comprado) {
                SetTextColor(hdc, RGB(100, 100, 100));
                TextOut(hdc, x + 40, y + 8, "COMPRADO", 8);
            } else {
                dibujarPrecio(hdc, x + 40, y + 8, costo1, costo2, alcanzable);
            }
        }
    }
    else // --- MODO VENDER ---
    {
        int precios[] = {1, 2, 5, 1, 3, 8}; 
        char *nombres[] = {"Madera", "Piedra", "Hierro", "Hojas", "Comida", "Hacha"};
        HBITMAP iconos[] = {hBmpIconoMadera, hBmpIconoPiedra, hBmpIconoHierro, hBmpIconoHoja, hBmpIconoComida, hBmpIconoHacha};
        int cantidades[] = {j->madera, j->piedra, j->hierro, j->hojas, j->comida, j->cantHachas};

        for (int i = 0; i < 6; i++) {
            int x = startX + ((i % 2) * 140);
            int y = startY + ((i / 2) * 65);

            // Efecto Hover en Venta
            if (mx >= x && mx <= x + 130 && my >= y && my <= y + 60) {
                HBRUSH hHoverV = CreateSolidBrush(RGB(60, 60, 70));
                RECT rHoverV = {x - 5, y - 5, x + 135, y + 60};
                FillRect(hdc, &rHoverV, hHoverV);
                DeleteObject(hHoverV);
            }

            if (iconos[i]) DibujarImagen(hdc, iconos[i], x, y, 32, 32);
            
            char txt[32]; 
            sprintf(txt, "%s (x%d)", nombres[i], cantidades[i]);
            SetTextColor(hdc, RGB(255, 255, 255)); 
            TextOut(hdc, x + 40, y, txt, (int)strlen(txt));
            
            sprintf(txt, "+%d Oro", precios[i]);
            SetTextColor(hdc, RGB(255, 215, 0)); 
            TextOut(hdc, x + 40, y + 16, txt, (int)strlen(txt));
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

    // LÓGICA DE VISIBILIDAD
    if (jugador.armaduraEquipada)
    {
        // SOLO DIBUJAR ARMADURA (El personaje desaparece)
        HBITMAP sArma = hBmpArmaduraAnim[jugador.direccion][jugador.frameAnim];
        if (sArma)
            DibujarImagen(hdc, sArma, cx, cy, tam, tam);
    }
    else
    {
        // DIBUJAR JUGADOR NORMAL (Sin armadura)
        HBITMAP sprite = hBmpJugadorAnim[jugador.direccion][jugador.frameAnim];
        DibujarImagen(hdc, (sprite ? sprite : hBmpJugador), cx, cy, tam, tam);
    }

    // Destello (Efecto de daño o magia)
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
void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera cam, int ancho, int alto, int frameTienda, int mapaId)
{
    // 1. FONDO DE AGUA (Primero de todo, para que sea la base)
    HBRUSH agua = CreateSolidBrush(RGB(0, 100, 180));
    RECT r = {0, 0, ancho, alto};
    FillRect(hdc, &r, agua);
    DeleteObject(agua);

    // 2. DIBUJAR LAS 5 ISLAS (MODIFICADO)
    for (int i = 0; i < MAX_ISLAS; i++)
    {
        if (!misIslas[i].activa)
            continue;

        int sx = (misIslas[i].x - cam.x) * cam.zoom;
        int sy = (misIslas[i].y - cam.y) * cam.zoom;
        int sw = misIslas[i].ancho * cam.zoom;
        int sh = misIslas[i].alto * cam.zoom;

        // USAR LA FUNCIÓN AUXILIAR para obtener imagen según mapa
        HBITMAP img = obtenerImagenIsla(i, mapaId);

        if (img)
            DibujarImagen(hdc, img, sx, sy, sw, sh);
    }

    // 3. DIBUJAR LAS VACAS (Encima de las islas, debajo de la tienda/HUD)
    dibujarVacas(hdc, cam, ancho, alto);
    dibujarArboles(hdc, cam, ancho, alto, mapaId); 
    dibujarTiendasEnIslas(hdc, cam, ancho, alto, frameTienda);

    // --- DEBUG VISUAL (Opcional: Descomentar para ver la rejilla generada por la imagen) ---
    /*
    HBRUSH bTierra = CreateSolidBrush(RGB(255, 0, 0)); // Rojo
    for (int y = 0; y < MUNDO_FILAS; y++) {
        for (int x = 0; x < MUNDO_COLUMNAS; x++) {
            if (mapa[y][x] == 1) { // Si es tierra
                RECT rd = {
                    (x * TAMANO_CELDA_BASE - cam.x) * cam.zoom,
                    (y * TAMANO_CELDA_BASE - cam.y) * cam.zoom,
                    (x * TAMANO_CELDA_BASE + TAMANO_CELDA_BASE - cam.x) * cam.zoom,
                    (y * TAMANO_CELDA_BASE + TAMANO_CELDA_BASE - cam.y) * cam.zoom
                };
                FrameRect(hdc, &rd, bTierra);
            }
        }
    }
    DeleteObject(bTierra);
    */
}

void dibujarItemRejilla(HDC hdc, HBITMAP icono, int cantidad, int maximo, int x, int y, const char *nombre)
{
    // 1. Dibujar Icono
    if (icono)
        DibujarImagen(hdc, icono, x, y, 32, 32);

    // 2. Texto del Nombre
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(200, 200, 200));
    TextOut(hdc, x + 40, y - 5, nombre, strlen(nombre));

    // 3. Texto de Cantidad
    char buf[32];
    if (maximo > 0)
    {
        // Formato para Recursos: "12 / 99"
        sprintf(buf, "%d / %d", cantidad, maximo);
        if (cantidad >= maximo)
            SetTextColor(hdc, RGB(255, 100, 100)); // Rojo si está lleno
        else
            SetTextColor(hdc, RGB(255, 255, 255)); // Blanco
    }
    else
    {
        // Formato para Población/Equipo: "x5"
        sprintf(buf, "x%d", cantidad);
        SetTextColor(hdc, RGB(255, 215, 0)); // Dorado
    }
    TextOut(hdc, x + 40, y + 12, buf, strlen(buf));
}

void dibujarHUD(HDC hdc, Jugador *jugador, int ancho, int alto)
{
    // 1. VIDA
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

    // 2. ARMADURA (Texto corregido a Actual / Max)
    HBITMAP bmpEscudo = hBmpEscudo0;
    if (jugador->armaduraActual > 0)
        bmpEscudo = hBmpEscudo100; // Icono lleno si tiene algo
    if (bmpEscudo)
        DibujarImagen(hdc, bmpEscudo, 20, 60, 48, 48);

    char tEscudo[16];
    // Muestra "5 / 10" o "0 / 100"
    sprintf(tEscudo, "%d / %d", jugador->armaduraActual, jugador->armaduraMax);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    TextOut(hdc, 75, 75, tEscudo, strlen(tEscudo));

    // 3. ICONO DE MOCHILA

    HBITMAP bmpBolso = (jugador->inventarioAbierto) ? hBmpInvAbierto : hBmpInvCerrado;
    if (bmpBolso)
        DibujarImagen(hdc, bmpBolso, 20, 120, 64, 64);

// --- 4. PANEL DE INVENTARIO Y POBLACIÓN REORGANIZADO ---
if (jugador->inventarioAbierto)
{
    int px = 90, py = 120;
    int anchoFondo = 360; 
    int alturaFondo = 420; // Aumentamos un poco el fondo total para el nuevo espacio

    // Dibujar Fondo y Borde
    HBRUSH fondo = CreateSolidBrush(RGB(30, 30, 35));
    RECT r = {px, py, px + anchoFondo, py + alturaFondo};
    FillRect(hdc, &r, fondo);
    DeleteObject(fondo);
    HBRUSH borde = CreateSolidBrush(RGB(255, 255, 255));
    FrameRect(hdc, &r, borde);
    DeleteObject(borde);

    int startX = px + 20;
    int startY = py + 40;

    // --- SECCIÓN 1: RECURSOS Y EQUIPO ---
    SetTextColor(hdc, RGB(255, 215, 0));
    TextOut(hdc, px + 20, py + 10, "RECURSOS Y EQUIPO", 17);

    // Fila 1: Madera y Piedra
    dibujarItemRejilla(hdc, hBmpIconoMadera, jugador->madera, 99, startX, startY, "Madera");
    dibujarItemRejilla(hdc, hBmpIconoPiedra, jugador->piedra, 99, startX + 170, startY, "Piedra");

    // Fila 2: Oro e Hierro
    dibujarItemRejilla(hdc, hBmpIconoOro, jugador->oro, 999, startX, startY + 50, "Oro");
    dibujarItemRejilla(hdc, hBmpIconoHierro, jugador->hierro, 99, startX + 170, startY + 50, "Hierro");

    // Fila 3: Hojas y Comida
    dibujarItemRejilla(hdc, hBmpIconoHoja, jugador->hojas, 99, startX, startY + 100, "Hojas");
    dibujarItemRejilla(hdc, hBmpIconoComida, jugador->comida, 99, startX + 170, startY + 100, "Comida");

    // Fila 4: Bolso/Nivel (Ahora tiene más aire arriba)
    char tBolso[16]; sprintf(tBolso, "Nivel %d", jugador->nivelMochila);
    dibujarItemRejilla(hdc, hBmpInvCerrado, 1, 0, startX, startY + 150, tBolso);

    // --- LÍNEA DIVISORIA (MÁS ABAJO) ---
    HPEN penDiv = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
    HPEN oldPen = (HPEN)SelectObject(hdc, penDiv);
    MoveToEx(hdc, px + 10, py + 245, NULL); // Bajamos de 215 a 245
    LineTo(hdc, px + anchoFondo - 10, py + 245);
    SelectObject(hdc, oldPen);
    DeleteObject(penDiv);

    // --- SECCIÓN 2: POBLACIÓN ---
    SetTextColor(hdc, RGB(0, 255, 255));
    TextOut(hdc, px + 20, py + 255, "POBLACION / UNIDADES", 20); // Bajamos de 225 a 255

    int popY = py + 285; // Bajamos el inicio de los iconos de población

    // Población Fila 1: Leñadores y Mineros
    dibujarItemRejilla(hdc, hBmpLenadorAnim[DIR_ABAJO][0], jugador->cantLenadores, 0, startX, popY, "Lenadores");
    dibujarItemRejilla(hdc, hBmpMineroAnim[DIR_ABAJO][0], jugador->cantMineros, 0, startX + 170, popY, "Mineros");

    // Población Fila 2: Cazadores y Soldados
    dibujarItemRejilla(hdc, hBmpCazadorAnim[DIR_ABAJO][0], jugador->cantCazadores, 0, startX, popY + 50, "Cazadores");
    dibujarItemRejilla(hdc, hBmpSoldadoAnim[DIR_ABAJO][0], jugador->cantSoldados, 0, startX + 170, popY + 50, "Soldados");
}

    // 5. BARRA DE EXPERIENCIA (Sin cambios importantes)
    if (hBmpBarraXPVacia && hBmpBarraXPLlena)
    {
        BITMAP bm;
        GetObject(hBmpBarraXPVacia, sizeof(BITMAP), &bm);
        int xpX = (ancho - bm.bmWidth) / 2;
        int xpY = alto - bm.bmHeight - 20;
        DibujarImagen(hdc, hBmpBarraXPVacia, xpX, xpY, bm.bmWidth, bm.bmHeight);

        float pct = (float)jugador->experiencia / jugador->experienciaSiguienteNivel;
        if (pct > 1)
            pct = 1;
        int fillW = (int)(bm.bmWidth * pct);

        if (fillW > 0)
        {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmpBarraXPLlena);
            TransparentBlt(hdc, xpX, xpY, fillW, bm.bmHeight, hdcMem, 0, 0, fillW, bm.bmHeight, RGB(255, 0, 255));
            SelectObject(hdcMem, hOld);
            DeleteDC(hdcMem);
        }
        char tNivel[32];
        sprintf(tNivel, "NVL %d", jugador->nivel);
        SetTextColor(hdc, RGB(0, 255, 255));
        TextOut(hdc, xpX - 60, xpY + 5, tNivel, strlen(tNivel));
    }

    // 6. TIENDA (Lógica existente)
    BOOL cercaDeTienda = FALSE;
    if (sqrt(pow(jugador->x - 1450, 2) + pow(jugador->y - 1900, 2)) < 80)
        cercaDeTienda = TRUE;
    if (jugador->inventarioAbierto && cercaDeTienda)
        dibujarTiendaInteractiva(hdc, jugador);
}