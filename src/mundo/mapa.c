/* src/mapa/mapa.c - VERSIÓN LIMPIA Y PROFESIONAL */
#include "mapa.h"
#include "fauna.h"
#include "naturaleza.h"
#include "../global.h"
#include "../recursos/recursos.h"
#include "../jugador/jugador.h"
#include "../unidades/unidades.h"
#include <stdio.h>
#include <mmsystem.h>
#include <math.h>
#include <stdbool.h>

// --- VARIABLES GLOBALES INTERNAS ---
Particula chispas[MAX_PARTICULAS];
Isla misIslas[MAX_ISLAS];
TextoFlotante listaTextos[MAX_TEXTOS] = {0};

// Referencias externas necesarias
extern Jugador miJugador;

#define MARGEN_ESTABLO 100

// --- 1. SISTEMA DE ISLAS Y RECURSOS GRÁFICOS ---

HBITMAP obtenerImagenIsla(int indiceIsla, int mapaId)
{
    switch (mapaId)
    {
    case 0: // MAPA 1
        switch (indiceIsla)
        {
        case 0:
            return hBmpIslaGrande;
        case 1:
            return hBmpIslaSec2;
        case 2:
            return hBmpIslaSec4;
        case 3:
            return hBmpIslaSec1;
        case 4:
            return hBmpIslaSec3;
        }
        break;
    case 1: // MAPA 2
        switch (indiceIsla)
        {
        case 0:
            return hBmpIslaGrandeMapa2;
        case 1:
            return hBmpIslaSec2Mapa2;
        case 2:
            return hBmpIslaSec4Mapa2;
        case 3:
            return hBmpIslaSec1Mapa2;
        case 4:
            return hBmpIslaSec3Mapa2;
        }
        break;
    case 2: // MAPA 3
        switch (indiceIsla)
        {
        case 0:
            return hBmpIslaGrandeMapa3;
        case 1:
            return hBmpIslaSec2Mapa3;
        case 2:
            return hBmpIslaSec4Mapa3;
        case 3:
            return hBmpIslaSec1Mapa3;
        case 4:
            return hBmpIslaSec3Mapa3;
        }
        break;
    }
    return NULL;
}

void inicializarIslas(int mapaId)
{
    for (int i = 0; i < MAX_ISLAS; i++)
        misIslas[i].activa = 0;

    // ISLA CENTRAL (Grande)
    misIslas[0].activa = 1;
    misIslas[0].x = 1100;
    misIslas[0].y = 1100;
    misIslas[0].ancho = 1000;
    misIslas[0].alto = 1000;

    // ISLA NORTE
    misIslas[1].activa = 1;
    misIslas[1].x = 1250;
    misIslas[1].y = 200;
    misIslas[1].ancho = 700;
    misIslas[1].alto = 700;

    // ISLA SUR
    misIslas[2].activa = 1;
    misIslas[2].x = 1350;
    misIslas[2].y = 2300;
    misIslas[2].ancho = 500;
    misIslas[2].alto = 500;

    // ISLA OESTE
    misIslas[3].activa = 1;
    misIslas[3].x = 580;
    misIslas[3].y = 1475;
    misIslas[3].ancho = 320;
    misIslas[3].alto = 250;

    // ISLA ESTE
    misIslas[4].activa = 1;
    misIslas[4].x = 2300;
    misIslas[4].y = 1400;
    misIslas[4].ancho = 400;
    misIslas[4].alto = 400;
}

// --- 2. SISTEMA DE FÍSICA Y COLISIONES (ESCÁNER) ---

// Función Auxiliar: Obtiene el color visual real en una coordenada (X,Y)
COLORREF ObtenerColorDePunto(int x, int y, int mapaId)
{
    for (int i = 0; i < MAX_ISLAS; i++)
    {
        if (!misIslas[i].activa)
            continue;

        if (x >= misIslas[i].x && x < misIslas[i].x + misIslas[i].ancho &&
            y >= misIslas[i].y && y < misIslas[i].y + misIslas[i].alto)
        {

            int localX = x - misIslas[i].x;
            int localY = y - misIslas[i].y;

            HBITMAP hBmp = obtenerImagenIsla(i, mapaId);
            if (!hBmp)
                continue;

            HDC hdcScreen = GetDC(NULL);
            HDC hdcMem = CreateCompatibleDC(hdcScreen);
            HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmp);
            COLORREF color = GetPixel(hdcMem, localX, localY);
            SelectObject(hdcMem, hOld);
            DeleteDC(hdcMem);
            ReleaseDC(NULL, hdcScreen);

            return color;
        }
    }
    return RGB(0, 100, 180); // Agua Profunda
}

// Genera la matriz de colisión (Se ejecuta UNA VEZ al cargar)
void generarColisionDeMapaCompleto(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int mapaId)
{
    // 1. Limpiamos a AGUA (0)
    for (int y = 0; y < MUNDO_FILAS; y++)
    {
        for (int x = 0; x < MUNDO_COLUMNAS; x++)
        {
            mapa[y][x] = 0;
        }
    }

    // 2. Escaneamos punto por punto (16x16)
    for (int fila = 0; fila < MUNDO_FILAS; fila++)
    {
        for (int col = 0; col < MUNDO_COLUMNAS; col++)
        {
            int mundoX = (col * TAMANO_CELDA_BASE) + (TAMANO_CELDA_BASE / 2);
            int mundoY = (fila * TAMANO_CELDA_BASE) + (TAMANO_CELDA_BASE / 2);

            COLORREF c = ObtenerColorDePunto(mundoX, mundoY, mapaId);

            // Si NO es Magenta (Transparente) y NO es Azul -> TIERRA
            if (c != RGB(255, 0, 255) && c != RGB(0, 100, 180))
            {
                mapa[fila][col] = 1;
            }
        }
    }
}

void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int mapaId)
{
    inicializarIslas(mapaId);
    generarColisionDeMapaCompleto(mapa, mapaId);
}

int EsSuelo(int x, int y, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS])
{
    int col = x / TAMANO_CELDA_BASE;
    int fila = y / TAMANO_CELDA_BASE;
    if (fila < 0 || fila >= MUNDO_FILAS || col < 0 || col >= MUNDO_COLUMNAS)
        return 0;
    return (mapa[fila][col] == 1);
}

void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy)
{
    // 1. Lógica de Orientación y Animación (Se mantiene igual)
    if (dy > 0)
        jugador->direccion = DIR_ABAJO;
    else if (dy < 0)
        jugador->direccion = DIR_ARRIBA;
    else if (dx < 0)
        jugador->direccion = DIR_IZQUIERDA;
    else if (dx > 0)
        jugador->direccion = DIR_DERECHA;

    if (dx != 0 || dy != 0)
    {
        jugador->pasoAnimacion++;
        int estado = (jugador->pasoAnimacion / 4) % 4;
        jugador->frameAnim = (estado == 0) ? 1 : ((estado == 2) ? 2 : 0);
    }
    else
    {
        jugador->frameAnim = 1;
    }

    // --- VARIABLES DE CONTROL PARA EL LÍMITE DEL MAR ---
    int centroX = 1600;
    int centroY = 1600;
    int radioSeguro = 900;

    // --- FÍSICA EJE X ---
    int futuroX = jugador->x + (dx * jugador->velocidad);
    int piesY = jugador->y + 8;
    int piesX = futuroX + 16;

    int col = piesX / TAMANO_CELDA_BASE;
    int fila = piesY / TAMANO_CELDA_BASE;

    if (col >= 0 && col < MUNDO_COLUMNAS && fila >= 0 && fila < MUNDO_FILAS)
    {
        int terreno = mapa[fila][col];
        // Determinar si el terreno es apto según si es barco o pie
        bool puedeMoverseX = (jugador->estadoBarco == 0 && terreno == 1) ||
                             (jugador->estadoBarco > 0 && terreno == 0);

        // RESTRICCIÓN BOTE DE PESCA EN X (Límite visual de Agua Clara)
        if (puedeMoverseX && jugador->estadoBarco == 1)
        {
            // Calculamos distancia al centro del mapa
            float distCentro = sqrt(pow(futuroX - centroX, 2) + pow(jugador->y - centroY, 2));

            // Si intenta entrar al "Agua Profunda" (fuera del radio 900)
            if (distCentro > radioSeguro)
            {
                // EFECTO CORRIENTE: Empujamos al jugador hacia adentro
                if (dx > 0)
                    jugador->x -= 6; // Si iba hacia afuera (derecha), rebota
                else if (dx < 0)
                    jugador->x += 6; // Si iba hacia afuera (izquierda), rebota

                puedeMoverseX = false; // Bloqueamos el avance original

                // Feedback visual aleatorio
                if (rand() % 30 == 0)
                    crearTextoFlotante(jugador->x, jugador->y, "¡Tiburones a la vista!", 0, RGB(255, 50, 50));
            }
        }

        if (puedeMoverseX)
            jugador->x = futuroX;
    }

    // --- FÍSICA EJE Y ---
    int futuroY = jugador->y + (dy * jugador->velocidad);
    piesX = jugador->x + 16;
    piesY = futuroY + 8;

    col = piesX / TAMANO_CELDA_BASE;
    fila = piesY / TAMANO_CELDA_BASE;

    if (col >= 0 && col < MUNDO_COLUMNAS && fila >= 0 && fila < MUNDO_FILAS)
    {
        int terreno = mapa[fila][col];
        bool puedeMoverseY = (jugador->estadoBarco == 0 && terreno == 1) ||
                             (jugador->estadoBarco > 0 && terreno == 0);

        // RESTRICCIÓN BOTE DE PESCA EN Y (Límite visual de Agua Clara)
        if (puedeMoverseY && jugador->estadoBarco == 1)
        {
            float distCentro = sqrt(pow(jugador->x - centroX, 2) + pow(futuroY - centroY, 2));

            if (distCentro > radioSeguro)
            {
                // EFECTO CORRIENTE
                if (dy > 0)
                    jugador->y -= 6;
                else if (dy < 0)
                    jugador->y += 6;

                puedeMoverseY = false;

                if (rand() % 30 == 0)
                    crearTextoFlotante(jugador->x, jugador->y, "¡Agua muy profunda!", 0, RGB(100, 200, 255));
            }
        }

        if (puedeMoverseY)
            jugador->y = futuroY;
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

// --- 3. TEXTOS Y EFECTOS ---

void crearTextoFlotante(int x, int y, const char *contenido, int cantidad, COLORREF color)
{
    for (int i = 0; i < MAX_TEXTOS; i++)
    {
        if (!listaTextos[i].activo)
        {
            listaTextos[i].x = (float)x;
            listaTextos[i].y = (float)y;
            if (cantidad > 0)
                sprintf(listaTextos[i].texto, "+%d %s", cantidad, contenido);
            else
                strcpy(listaTextos[i].texto, contenido);
            listaTextos[i].vida = 60;
            listaTextos[i].color = color;
            listaTextos[i].activo = 1;
            break;
        }
    }
}

void actualizarYDibujarTextos(HDC hdc, Camera cam)
{
    SetBkMode(hdc, TRANSPARENT);
    for (int i = 0; i < MAX_TEXTOS; i++)
    {
        if (listaTextos[i].activo)
        {
            listaTextos[i].y -= 0.8f;
            listaTextos[i].vida--;
            if (listaTextos[i].vida <= 0)
                listaTextos[i].activo = 0;
            else
            {
                int sx = (int)((listaTextos[i].x - cam.x) * cam.zoom);
                int sy = (int)((listaTextos[i].y - cam.y) * cam.zoom);
                SetTextColor(hdc, RGB(0, 0, 0)); // Sombra
                TextOut(hdc, sx + 1, sy + 1, listaTextos[i].texto, strlen(listaTextos[i].texto));
                SetTextColor(hdc, listaTextos[i].color);
                TextOut(hdc, sx, sy, listaTextos[i].texto, strlen(listaTextos[i].texto));
            }
        }
    }
}

void crearChispas(int x, int y, COLORREF color)
{
    int creadas = 0;
    for (int i = 0; i < MAX_PARTICULAS && creadas < 8; i++)
    {
        if (!chispas[i].activo)
        {
            chispas[i].x = x;
            chispas[i].y = y;
            chispas[i].vx = (rand() % 11 - 5);
            chispas[i].vy = (rand() % 11 - 8);
            chispas[i].vida = 15 + (rand() % 10);
            chispas[i].color = color;
            chispas[i].activo = 1;
            creadas++;
        }
    }
}

void crearChispaBlanca(float x, float y) { crearChispas((int)x, (int)y, RGB(255, 255, 255)); }

void actualizarYDibujarParticulas(HDC hdc, Camera cam)
{
    for (int i = 0; i < MAX_PARTICULAS; i++)
    {
        if (chispas[i].activo)
        {
            chispas[i].x += chispas[i].vx;
            chispas[i].y += chispas[i].vy;
            chispas[i].vy += 0.4f;
            chispas[i].vida--;
            if (chispas[i].vida <= 0)
                chispas[i].activo = 0;
            else
            {
                int sx = (int)((chispas[i].x - cam.x) * cam.zoom);
                int sy = (int)((chispas[i].y - cam.y) * cam.zoom);
                HBRUSH hBr = CreateSolidBrush(chispas[i].color);
                RECT r = {sx, sy, sx + 4, sy + 4};
                FillRect(hdc, &r, hBr);
                DeleteObject(hBr);
            }
        }
    }
}

// --- 4. ENTIDADES DEL JUEGO (VACAS, ARBOLES, MINAS, TESOROS) ---

void dibujarEstablo(HDC hdc, Camera cam)
{
    int sx = (int)((ESTABLO_X - cam.x) * cam.zoom);
    int sy = (int)((ESTABLO_Y - cam.y) * cam.zoom);
    if (hBmpEstablo)
        DibujarImagen(hdc, hBmpEstablo, sx, sy, 200 * cam.zoom, 200 * cam.zoom);
}

// --- 5. LÓGICA DE JUEGO GENERAL ---

void actualizarLogicaSistema(Jugador *j)
{
    // Pesca
    if (j->estadoBarco == 1)
    {
        j->timerPesca++;
        if (j->timerPesca >= 600)
        { // 10 seg
            j->timerPesca = 0;

            // --- CORRECCIÓN LÍMITES ---
            int ant = j->comida; // Usamos comida en lugar de pescado para consistencia
            agregarRecurso(&j->comida, 3, j->nivelMochila);
            int gan = j->comida - ant;

            if (gan > 0)
                crearTextoFlotante(j->x, j->y, "Pescado", gan, RGB(100, 200, 255));
            else
                crearTextoFlotante(j->x, j->y - 40, "Mochila Llena!", 0, RGB(255, 50, 50));
        }
    }
    else
        j->timerPesca = 0;
    actualizarTiburones(j); // <--- AGREGAR AQUÍ
}

void actualizarRegeneracionRecursos()
{
    actualizarRegeneracionNaturaleza();

    actualizarRegeneracionFauna();
}
void intentarMontarBarco(Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS])
{
    // 1. SI YA ESTAMOS EN BARCO -> BAJARSE
    if (j->estadoBarco > 0)
    {
        // Solo bajar si tocamos Muelle o Tierra
        float distMuelle = sqrt(pow(j->x - MUELLE_X, 2) + pow(j->y - MUELLE_Y, 2));

        if (EsSuelo(j->x + 16, j->y + 32, mapa) || distMuelle < 150)
        {
            j->estadoBarco = 0; // A PIE
            crearTextoFlotante(j->x, j->y, "A tierra!", 0, RGB(255, 255, 255));
            // Pequeño empujón hacia el muelle para que no quede en el agua
            if (distMuelle < 150)
            {
                j->x = MUELLE_X + 20;
                j->y = MUELLE_Y + 20;
            }
        }
        else
        {
            crearTextoFlotante(j->x, j->y, "Solo en muelle o costa!", 0, RGB(255, 100, 100));
        }
        return;
    }

    // 2. SI ESTAMOS A PIE -> MONTARSE
    // Solo permitir si estamos cerca del MUELLE
    float dist = sqrt(pow(j->x - MUELLE_X, 2) + pow(j->y - MUELLE_Y, 2));

    if (dist < 150)
    { // Radio de interacción del muelle

        // PRIORIDAD: ¿Qué barco sacamos?
        // Si el jugador quiere pescar (tiene caña y bote)
        if (j->tieneBotePesca && j->tieneCana)
        {
            j->estadoBarco = 1; // MODO PESCA
            j->x += 60;         // Salir un poco al agua
            crearTextoFlotante(j->x, j->y, "A pescar!", 0, RGB(100, 255, 255));
        }
        // Si quiere guerra
        else if (j->cantBarcosGuerra > 0)
        {
            j->estadoBarco = 2; // MODO GUERRA
            j->x += 60;
            crearTextoFlotante(j->x, j->y, "A la batalla!", 0, RGB(255, 50, 50));
        }
        // ERRORES
        else if (j->tieneBotePesca && !j->tieneCana)
        {
            crearTextoFlotante(j->x, j->y, "Falta la cana!", 0, RGB(255, 50, 50));
        }
        else
        {
            crearTextoFlotante(j->x, j->y, "No tienes barcos!", 0, RGB(200, 200, 200));
        }
    }
}

// --- 6. UNIDADES RTS (MINEROS, LEÑADORES, ETC) ---

// 2. CREAR ESCUADRÓN (Flexible: 5, 10 o 15)

// En src/mapa/mapa.c

// 4. DIBUJAR (Nombres y Herramientas)

// 5. CONTROL (Igual que antes pero actualizado)

// Dar orden de movimiento o acción

void dibujarMuelleYFlota(HDC hdc, Camera cam, Jugador *j)
{
    // 1. DIBUJAR MUELLE
    int mx = (int)((MUELLE_X - cam.x) * cam.zoom);
    int my = (int)((MUELLE_Y - cam.y) * cam.zoom);
    int tamMuelle = 80 * cam.zoom;

    if (hBmpMuelle)
    {
        DibujarImagen(hdc, hBmpMuelle, mx, my, tamMuelle, tamMuelle);
    }
    else
    {
        // Fallback si no tienes la imagen aún: Un cuadro marrón
        HBRUSH madera = CreateSolidBrush(RGB(100, 50, 0));
        RECT r = {mx, my, mx + tamMuelle, my + (tamMuelle / 2)};
        FillRect(hdc, &r, madera);
        DeleteObject(madera);
    }

    // 2. DIBUJAR BOTE DE PESCA (Estacionado)
    if (j->tieneBotePesca && j->estadoBarco != 1)
    { // Si lo tienes y NO lo estás usando
        int bx = mx + (30 * cam.zoom);
        int by = my - (50 * cam.zoom); // Arriba del muelle
        int tamBote = 80 * cam.zoom;
        if (hBmpBote[0])
            DibujarImagen(hdc, hBmpBote[0], bx, by, tamBote, tamBote);
    }

    // 3. DIBUJAR FLOTA DE GUERRA (Estacionados)
    // Se dibujan en fila hacia abajo del muelle
    for (int i = 0; i < j->cantBarcosGuerra; i++)
    {
        // Si estás usando el barco de guerra, dibujamos uno menos en el muelle
        if (j->estadoBarco == 2 && i == 0)
            continue;

        int gx = mx + (40 * cam.zoom);
        int gy = my + (80 * cam.zoom) + (i * 50 * cam.zoom); // Uno debajo del otro
        int tamGuerra = 120 * cam.zoom;

        if (hBmpBarco[0])
            DibujarImagen(hdc, hBmpBarco[0], gx, gy, tamGuerra, tamGuerra);
    }
}

// --- 7. LÓGICA DE TIENDA (RTS y OBJETOS) ---
// En src/mapa/mapa.c

void procesarClickMochilaTienda(int mx, int my, int esClickDerecho, Jugador *j, HWND hwnd)
{
    if (!j->tiendaAbierta)
        return;

    // 1. OBTENER POSICIÓN DINÁMICA
    RECT rect;
    GetClientRect(hwnd, &rect);
    int anchoVentana = rect.right;
    int anchoW = 340;
    int tx = anchoVentana - anchoW - 20;
    int ty = 80;

    // --- CLIC EN PESTAÑAS ---
    if (my >= ty && my <= ty + 40)
    {
        if (mx >= tx && mx < tx + 85)
            j->modoTienda = 0;
        else if (mx >= tx + 85 && mx < tx + 170)
            j->modoTienda = 1;
        else if (mx >= tx + 170 && mx < tx + 255)
            j->modoTienda = 2;
        else if (mx >= tx + 255 && mx < tx + 340)
            j->modoTienda = 3;

        PlaySound("SystemSelect", NULL, SND_ASYNC);
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }

    int startY = ty + 50;
    char msg[32];

    // Mensaje sobre la cabeza del jugador
    float msgX = j->x;
    float msgY = j->y - 40;

    // --- TAB 0: HERRAMIENTAS ---
    if (j->modoTienda == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            int iy = startY + (i * 80);
            if (my >= iy && my <= iy + 60 && mx >= tx && mx <= tx + 300)
            {

                // --- CORRECCIÓN 1: BLOQUEO POR NIVEL DE MOCHILA ---
                // Si intenta comprar Espada (0), Pico (1) o Hacha (2) y no tiene Mochila Nvl 2...
                if (i <= 2 && j->nivelMochila < 2)
                {
                    crearTextoFlotante(msgX, msgY, "Req. Mochila Nvl 2", 0, RGB(255, 50, 50));
                    return; // Detener compra
                }
                // --------------------------------------------------

                // Item 0: Espada
                if (i == 0)
                {
                    if (j->tieneEspada)
                        return; // Ya la tiene
                    if (j->oro < 50)
                    {
                        sprintf(msg, "Faltan %d Oro", 50 - j->oro);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                    }
                    else
                    {
                        j->oro -= 50;
                        j->tieneEspada = TRUE;
                        ganarExperiencia(j, 10);
                    }
                }
                // Item 1: Pico
                else if (i == 1)
                {
                    if (j->tienePico)
                        return;
                    if (j->oro < 30)
                    {
                        sprintf(msg, "Faltan %d Oro", 30 - j->oro);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                    }
                    else
                    {
                        j->oro -= 30;
                        j->tienePico = TRUE;
                        ganarExperiencia(j, 10);
                    }
                }
                // Item 2: Hacha
                else if (i == 2)
                {
                    if (j->tieneHacha)
                        return;
                    if (j->oro < 20)
                    {
                        sprintf(msg, "Faltan %d Oro", 20 - j->oro);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                    }
                    else
                    {
                        j->oro -= 20;
                        j->tieneHacha = TRUE;
                        ganarExperiencia(j, 10);
                    }
                }
                // Item 3: Caña (También requiere Nvl 2)
                else if (i == 3)
                {
                    if (j->tieneCana)
                        return;
                    // Check adicional específico para la caña si quieres, o dejar el global
                    if (j->nivelMochila < 2)
                    {
                        crearTextoFlotante(msgX, msgY, "Req. Mochila Nvl 2", 0, RGB(255, 50, 50));
                        return;
                    }

                    BOOL falta = FALSE;
                    if (j->madera < 10)
                    {
                        sprintf(msg, "Faltan %d Mad", 10 - j->madera);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                        falta = TRUE;
                    }
                    if (j->oro < 30)
                    {
                        sprintf(msg, "Faltan %d Oro", 30 - j->oro);
                        crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50));
                        falta = TRUE;
                    }

                    if (!falta)
                    {
                        j->oro -= 30;
                        j->madera -= 10;
                        j->tieneCana = TRUE;
                        ganarExperiencia(j, 20);
                    }
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
    }

    // --- TAB 1: TROPAS (Igual que antes) ---
    else if (j->modoTienda == 1)
    {
        int cant = (esClickDerecho) ? 5 : 1;
        for (int i = 0; i < 4; i++)
        {
            int iy = startY + (i * 80);
            if (my >= iy && my <= iy + 60 && mx >= tx && mx <= tx + 300)
            {
                // Soldado
                if (i == 0)
                {
                    BOOL falta = FALSE;
                    if (j->hierro < 10 * cant)
                    {
                        sprintf(msg, "Faltan %d Hie", (10 * cant) - j->hierro);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                        falta = TRUE;
                    }
                    if (j->oro < 40 * cant)
                    {
                        sprintf(msg, "Faltan %d Oro", (40 * cant) - j->oro);
                        crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50));
                        falta = TRUE;
                    }
                    if (!falta)
                    {
                        j->oro -= 40 * cant;
                        j->hierro -= 10 * cant;
                        j->cantSoldados += cant;
                        spawnearEscuadron(TIPO_SOLDADO, cant, j->x + 50, j->y);
                        ganarExperiencia(j, 10 * cant);
                    }
                }
                // Minero
                else if (i == 1)
                {
                    BOOL falta = FALSE;
                    if (j->piedra < 5 * cant)
                    {
                        sprintf(msg, "Faltan %d Pie", (5 * cant) - j->piedra);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                        falta = TRUE;
                    }
                    if (j->oro < 15 * cant)
                    {
                        sprintf(msg, "Faltan %d Oro", (15 * cant) - j->oro);
                        crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50));
                        falta = TRUE;
                    }
                    if (!falta)
                    {
                        j->oro -= 15 * cant;
                        j->piedra -= 5 * cant;
                        j->cantMineros += cant;
                        spawnearEscuadron(TIPO_MINERO, cant, j->x + 50, j->y);
                        ganarExperiencia(j, 5 * cant);
                    }
                }
                // Leñador
                else if (i == 2)
                {
                    BOOL falta = FALSE;
                    if (j->madera < 10 * cant)
                    {
                        sprintf(msg, "Faltan %d Mad", (10 * cant) - j->madera);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                        falta = TRUE;
                    }
                    if (j->oro < 10 * cant)
                    {
                        sprintf(msg, "Faltan %d Oro", (10 * cant) - j->oro);
                        crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50));
                        falta = TRUE;
                    }
                    if (!falta)
                    {
                        j->oro -= 10 * cant;
                        j->madera -= 10 * cant;
                        j->cantLenadores += cant;
                        spawnearEscuadron(TIPO_LENADOR, cant, j->x + 50, j->y);
                        ganarExperiencia(j, 5 * cant);
                    }
                }
                // Cazador
                else if (i == 3)
                {
                    BOOL falta = FALSE;
                    if (j->hierro < 3 * cant)
                    {
                        sprintf(msg, "Faltan %d Hie", (3 * cant) - j->hierro);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                        falta = TRUE;
                    }
                    if (j->oro < 20 * cant)
                    {
                        sprintf(msg, "Faltan %d Oro", (20 * cant) - j->oro);
                        crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50));
                        falta = TRUE;
                    }
                    if (!falta)
                    {
                        j->oro -= 20 * cant;
                        j->hierro -= 3 * cant;
                        j->cantCazadores += cant;
                        spawnearEscuadron(TIPO_CAZADOR, cant, j->x + 50, j->y);
                        ganarExperiencia(j, 5 * cant);
                    }
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
    }

    // --- TAB 2: LOGÍSTICA (Igual que antes) ---
    else if (j->modoTienda == 2)
    {
        if (my >= startY && my <= startY + 60)
        { // Mochila 2
            if (j->nivelMochila >= 2)
                return;
            BOOL falta = FALSE;
            if (j->hojas < 20)
            {
                sprintf(msg, "Faltan %d Hojas", 20 - j->hojas);
                crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                falta = TRUE;
            }
            if (j->oro < 99)
            {
                sprintf(msg, "Faltan %d Oro", 99 - j->oro);
                crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50));
                falta = TRUE;
            }
            if (!falta)
            {
                j->oro -= 99;
                j->hojas -= 20;
                j->nivelMochila = 2;
                ganarExperiencia(j, 50);
                MessageBox(hwnd, "Capacidad aumentada!", "Mejora", MB_OK);
            }
        }
        else if (my >= startY + 80 && my <= startY + 140)
        { // Mochila 3
            if (j->nivelMochila >= 3)
                return;
            if (j->nivelMochila < 2)
            {
                crearTextoFlotante(msgX, msgY, "Req. Mochila Nvl 2", 0, RGB(255, 50, 50));
                return;
            }
            BOOL falta = FALSE;
            if (j->hierro < 50)
            {
                sprintf(msg, "Faltan %d Hie", 50 - j->hierro);
                crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                falta = TRUE;
            }
            if (j->oro < 200)
            {
                sprintf(msg, "Faltan %d Oro", 200 - j->oro);
                crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50));
                falta = TRUE;
            }
            if (!falta)
            {
                j->oro -= 200;
                j->hierro -= 50;
                j->nivelMochila = 3;
                ganarExperiencia(j, 100);
            }
        }
        else if (my >= startY + 160 && my <= startY + 220)
        { // Galeon
            if (j->cantBarcosGuerra >= 4)
            {
                crearTextoFlotante(msgX, msgY, "Flota Maxima", 0, RGB(255, 100, 100));
                return;
            }
            if (j->nivelMochila < 2)
            {
                crearTextoFlotante(msgX, msgY, "Req. Mochila Nvl 2", 0, RGB(255, 50, 50));
                return;
            }
            BOOL falta = FALSE;
            if (j->madera < 50)
            {
                sprintf(msg, "Faltan %d Mad", 50 - j->madera);
                crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                falta = TRUE;
            }
            if (j->oro < 100)
            {
                sprintf(msg, "Faltan %d Oro", 100 - j->oro);
                crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50));
                falta = TRUE;
            }
            if (!falta)
            {
                j->oro -= 100;
                j->madera -= 50;
                j->cantBarcosGuerra++;
                ganarExperiencia(j, 50);
            }
        }
        else if (my >= startY + 240 && my <= startY + 300)
        { // Bote
            if (j->tieneBotePesca)
                return;
            if (j->nivelMochila < 2)
            {
                crearTextoFlotante(msgX, msgY, "Req. Mochila Nvl 2", 0, RGB(255, 50, 50));
                return;
            }
            BOOL falta = FALSE;
            if (j->madera < 30)
            {
                sprintf(msg, "Faltan %d Mad", 30 - j->madera);
                crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                falta = TRUE;
            }
            if (j->oro < 50)
            {
                sprintf(msg, "Faltan %d Oro", 50 - j->oro);
                crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50));
                falta = TRUE;
            }
            if (!falta)
            {
                j->oro -= 50;
                j->madera -= 30;
                j->tieneBotePesca = TRUE;
                ganarExperiencia(j, 30);
            }
        }
        InvalidateRect(hwnd, NULL, FALSE);
    }

    // --- TAB 3: VENDER ---
    else if (j->modoTienda == 3)
    {
        int precios[] = {1, 2, 5, 1, 3};
        int *stocks[] = {&j->madera, &j->piedra, &j->hierro, &j->hojas, &j->comida};

        // --- CORRECCIÓN 2: OBTENER EL LÍMITE REAL DE ORO ---
        int maxOro = obtenerCapacidadMaxima(j->nivelMochila); //

        for (int i = 0; i < 5; i++)
        {
            int iy = startY + (i * 60);
            if (my >= iy && my <= iy + 40 && mx >= tx && mx <= tx + 300)
            {
                int cant = (esClickDerecho) ? 10 : 1;

                // Primero verificamos si tienes el recurso para vender
                if (*stocks[i] >= cant)
                {

                    int ganancia = cant * precios[i];

                    // --- VERIFICAR SI EL ORO CABE EN LA MOCHILA ---
                    if (j->oro + ganancia > maxOro)
                    {
                        // Si se pasa, mostramos error
                        crearTextoFlotante(msgX, msgY, "Limite de Oro alcanzado!", 0, RGB(255, 50, 50));
                    }
                    else
                    {
                        // Si cabe, hacemos la transacción
                        *stocks[i] -= cant;
                        j->oro += ganancia; // Ahora es seguro sumar

                        sprintf(msg, "+%d Oro", ganancia);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 215, 0));
                    }
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                else
                {
                    crearTextoFlotante(msgX, msgY, "No tienes suficiente", 0, RGB(255, 100, 100));
                }
            }
        }
    }
}

/// --- DIBUJAR LA INTERFAZ DE LA TIENDA (TECLA T) ---
void dibujarTiendaInteractiva(HDC hdc, Jugador *j, int ancho, int alto)
{
    // POSICIÓN DERECHA (Para que conviva con el Inventario en la Izquierda)
    int anchoW = 340; // Ancho de la tienda
    int altoW = 480;  // Alto de la tienda

    // CAMBIO DE LÓGICA:
    // En lugar de tx = 600, calculamos: (AnchoPantalla - AnchoTienda - 20 margen)
    int tx = ancho - anchoW - 20;
    int ty = 80;

    // 1. FONDO
    HBRUSH fondo = CreateSolidBrush(RGB(45, 40, 35));
    RECT r = {tx, ty, tx + anchoW, ty + altoW};
    FillRect(hdc, &r, fondo);
    DeleteObject(fondo);
    HBRUSH borde = CreateSolidBrush(RGB(218, 165, 32));
    FrameRect(hdc, &r, borde);
    DeleteObject(borde);

    // 2. PESTAÑAS (Categorías)
    char *tabs[] = {"HERRAM.", "TROPAS", "LOGIST.", "VENDER"};
    int tabW = anchoW / 4;

    SetBkMode(hdc, TRANSPARENT);
    for (int i = 0; i < 4; i++)
    {
        COLORREF colorT = (j->modoTienda == i) ? RGB(255, 215, 0) : RGB(100, 100, 100);
        SetTextColor(hdc, colorT);
        TextOut(hdc, tx + 5 + (i * tabW), ty + 10, tabs[i], strlen(tabs[i]));

        // Línea debajo de la pestaña activa
        if (j->modoTienda == i)
        {
            HPEN penLine = CreatePen(PS_SOLID, 2, RGB(255, 215, 0));
            HGDIOBJ old = SelectObject(hdc, penLine);
            MoveToEx(hdc, tx + (i * tabW), ty + 30, NULL);
            LineTo(hdc, tx + ((i + 1) * tabW), ty + 30); // Sin NULL
            SelectObject(hdc, old);
            DeleteObject(penLine);
        }
    }

    // 3. CONTENIDO
    int startX = tx + 20;
    int startY = ty + 50;
    SetTextColor(hdc, RGB(255, 255, 255));

    // --- TAB 0: HERRAMIENTAS ---
    if (j->modoTienda == 0)
    {
        struct ItemShop
        {
            char *nom;
            HBITMAP img;
            int costO;
            int costMat;
            char *matNom;
            BOOL tiene;
        };
        struct ItemShop items[] = {
            {"Espada", hBmpIconoEspada, 50, 0, "", j->tieneEspada},
            {"Pico", hBmpIconoPico, 30, 0, "", j->tienePico},
            {"Hacha", hBmpIconoHacha, 20, 0, "", j->tieneHacha},
            {"Cana", hBmpIconoCana, 30, 10, "Mad", j->tieneCana}};

        for (int i = 0; i < 4; i++)
        {
            int iy = startY + (i * 80);
            if (items[i].img)
                DibujarImagen(hdc, items[i].img, startX, iy, 40, 40);
            TextOut(hdc, startX + 50, iy, items[i].nom, strlen(items[i].nom));

            if (items[i].tiene)
            {
                SetTextColor(hdc, RGB(0, 255, 0));
                TextOut(hdc, startX + 50, iy + 20, "EN PROPIEDAD", 12);
            }
            else
            {
                char precio[32];
                if (items[i].costMat > 0)
                    sprintf(precio, "%d Oro + %d %s", items[i].costO, items[i].costMat, items[i].matNom);
                else
                    sprintf(precio, "%d Oro", items[i].costO);
                SetTextColor(hdc, RGB(255, 215, 0));
                TextOut(hdc, startX + 50, iy + 20, precio, strlen(precio));
            }
            SetTextColor(hdc, RGB(255, 255, 255));
        }
    }

    // --- TAB 1: TROPAS (Sprites) ---
    else if (j->modoTienda == 1)
    {
        // Usamos el frame [0][0] (De frente) de los arrays de animación
        struct TropaShop
        {
            char *nom;
            HBITMAP sprite;
            int costO;
            int costMat;
            char *matNom;
        };
        struct TropaShop tropas[] = {
            {"Soldado", hBmpSoldadoAnim[0][0], 40, 10, "Hierro"},
            {"Minero", hBmpMineroAnim[0][0], 15, 5, "Piedra"},
            {"Lenador", hBmpLenadorAnim[0][0], 10, 10, "Madera"},
            {"Cazador", hBmpCazadorAnim[0][0], 20, 3, "Hierro"}};

        for (int i = 0; i < 4; i++)
        {
            int iy = startY + (i * 80);
            // DIBUJAR SPRITE DEL PERSONAJE
            if (tropas[i].sprite)
            {
                DibujarImagen(hdc, tropas[i].sprite, startX, iy, 40, 40);
            }
            TextOut(hdc, startX + 50, iy, tropas[i].nom, strlen(tropas[i].nom));
            char costes[32];
            sprintf(costes, "%d Oro + %d %s", tropas[i].costO, tropas[i].costMat, tropas[i].matNom);
            SetTextColor(hdc, RGB(200, 200, 200));
            TextOut(hdc, startX + 50, iy + 20, costes, strlen(costes));
            SetTextColor(hdc, RGB(255, 255, 255));
        }
        TextOut(hdc, startX, startY + 350, "Click Der: Comprar x5", 21);
    }

    // --- TAB 2: LOGÍSTICA ---
    else if (j->modoTienda == 2)
    {
        int iy = startY;
        // M2
        DibujarImagen(hdc, hBmpInvCerrado, startX, iy, 40, 40);
        TextOut(hdc, startX + 50, iy, "Mochila N2", 10);
        if (j->nivelMochila >= 2)
        {
            SetTextColor(hdc, RGB(0, 255, 0));
            TextOut(hdc, startX + 50, iy + 20, "COMPRADO", 8);
        }
        else
        {
            SetTextColor(hdc, RGB(255, 215, 0));
            TextOut(hdc, startX + 50, iy + 20, "99 Oro + 20 Hoj", 15);
        }
        SetTextColor(hdc, RGB(255, 255, 255));

        // M3
        iy += 80;
        DibujarImagen(hdc, hBmpInvCerrado, startX, iy, 40, 40);
        TextOut(hdc, startX + 50, iy, "Mochila N3", 10);
        if (j->nivelMochila >= 3)
        {
            SetTextColor(hdc, RGB(0, 255, 0));
            TextOut(hdc, startX + 50, iy + 20, "COMPRADO", 8);
        }
        else
        {
            SetTextColor(hdc, RGB(255, 215, 0));
            TextOut(hdc, startX + 50, iy + 20, "200 Oro + 50 Hierro", 19);
        }
        SetTextColor(hdc, RGB(255, 255, 255));

        // Galeon
        iy += 80;
        if (hBmpBarco[1])
            DibujarImagen(hdc, hBmpBarco[1], startX, iy, 40, 40);
        TextOut(hdc, startX + 50, iy, "Galeon Guerra", 13);
        char flota[32];
        sprintf(flota, "Flota: %d/4", j->cantBarcosGuerra);
        TextOut(hdc, startX + 50, iy + 20, flota, strlen(flota));
        SetTextColor(hdc, RGB(255, 215, 0));
        TextOut(hdc, startX + 50, iy + 35, "100 O + 50 M", 12);
        SetTextColor(hdc, RGB(255, 255, 255));

        // Bote
        iy += 80;
        if (hBmpBote[1])
            DibujarImagen(hdc, hBmpBote[1], startX, iy, 40, 40);
        TextOut(hdc, startX + 50, iy, "Bote Pesca", 10);
        if (j->tieneBotePesca)
        {
            SetTextColor(hdc, RGB(0, 255, 0));
            TextOut(hdc, startX + 50, iy + 20, "COMPRADO", 8);
        }
        else
        {
            SetTextColor(hdc, RGB(255, 215, 0));
            TextOut(hdc, startX + 50, iy + 20, "50 O + 30 M", 11);
        }
        SetTextColor(hdc, RGB(255, 255, 255));
    }

    // --- TAB 3: VENDER ---
    else if (j->modoTienda == 3)
    {
        char *nombres[] = {"Madera", "Piedra", "Hierro", "Hojas", "Comida"};
        HBITMAP iconos[] = {hBmpIconoMadera, hBmpIconoPiedra, hBmpIconoHierro, hBmpIconoHoja, hBmpIconoComida};
        int cantidades[] = {j->madera, j->piedra, j->hierro, j->hojas, j->comida};
        int valores[] = {1, 2, 5, 1, 3};

        for (int i = 0; i < 5; i++)
        {
            int iy = startY + (i * 60);
            if (iconos[i])
                DibujarImagen(hdc, iconos[i], startX, iy, 32, 32);
            char texto[64];
            sprintf(texto, "%s: %d", nombres[i], cantidades[i]);
            TextOut(hdc, startX + 40, iy, texto, strlen(texto));
            sprintf(texto, "Vender: +%d Oro", valores[i]);
            SetTextColor(hdc, RGB(255, 215, 0));
            TextOut(hdc, startX + 40, iy + 15, texto, strlen(texto));
            SetTextColor(hdc, RGB(255, 255, 255));
        }
    }
}

void dibujarTiendasEnIslas(HDC hdc, Camera cam, int ancho, int alto, int frameTienda)
{
    int tx = (1500 - cam.x) * cam.zoom;
    int ty = (1900 - cam.y) * cam.zoom;
    int tam = 50 * cam.zoom;
    int f = (frameTienda / 20) % 2;
    if (hBmpTienda[f])
        DibujarImagen(hdc, hBmpTienda[f], tx, ty, tam, tam);
}

// --- 8. INITIALIZACIÓN JUEGO ---

void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int mapaId)
{
    inicializarMapa(mapa, mapaId);
    inicializarArboles(mapa);
    inicializarVacas();
    inicializarTiburones(mapa);
    inicializarMinas(mapa);
    inicializarTesoros();
    inicializarUnidades();

    jugador->x = 1600;
    jugador->y = 1600;
    jugador->velocidad = 5;
    jugador->vidaActual = 100;
    jugador->vidaMax = 100;
    jugador->nivel = 1;
    jugador->experiencia = 0;

    // Valores por defecto
    jugador->inventarioAbierto = 0;
    jugador->herramientaActiva = HERRAMIENTA_NADA;
    jugador->armaduraEquipada = FALSE;

    estado->estadoActual = ESTADO_PARTIDA;
    estado->mapaSeleccionado = mapaId;
}

// --- 9. MENÚ Y UI GENERAL ---

void dibujarMenuConSprites(HDC hdc, HWND hwnd, EstadoJuego *estado)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    int ancho = rect.right;
    int alto = rect.bottom;

    // 1. DIBUJAR FONDO
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

    // 2. CONFIGURACIÓN DE BOTONES
    int btnAncho = 500;
    int btnAlto = 200;
    int separacion = 200; // Separación entre botones horizontales

    // --- TÍTULO (Arriba centrado) ---
    int tituloWidth = 600;
    int tituloHeight = 120;
    int tituloX = (ancho - tituloWidth) / 2;
    int tituloY = alto / 6; // 16.6% desde arriba

    if (hBmpTitulo)
    {
        BITMAP bm;
        GetObject(hBmpTitulo, sizeof(BITMAP), &bm);
        float aspect = (float)bm.bmWidth / bm.bmHeight;
        tituloHeight = (int)(tituloWidth / aspect);
        tituloY = alto / 8; // Ajustar si hay título gráfico
        DibujarImagen(hdc, hBmpTitulo, tituloX, tituloY, tituloWidth, tituloHeight);
    }

    // 3. BOTONES PRINCIPALES (Centrados verticalmente)
    // Calculamos el centro vertical para la fila de 3 botones
    int centroY = alto / 2;               // Centro de la pantalla
    int startY = centroY - (btnAlto / 2); // Y donde empiezan los botones

    // --- BOTÓN "PARTIDAS" (Izquierda) ---
    int partidasX = (ancho / 2) - btnAncho - separacion;
    HBITMAP imgPartidas = (hBmpBtnPartidas) ? hBmpBtnPartidas : hBmpBoton;
    DibujarImagen(hdc, imgPartidas, partidasX, startY, btnAncho, btnAlto);

    // --- BOTÓN "JUGAR" (Centro) ---
    int jugarX = (ancho - btnAncho) / 2;
    HBITMAP imgJugar = (hBmpBtnJugar) ? hBmpBtnJugar : hBmpBoton;
    DibujarImagen(hdc, imgJugar, jugarX, startY, btnAncho, btnAlto);

    // --- BOTÓN "INSTRUCCIONES" (Derecha) ---
    int instruccionesX = (ancho / 2) + separacion;
    HBITMAP imgInstr = (hBmpBtnInstrucciones) ? hBmpBtnInstrucciones : hBmpBoton;
    DibujarImagen(hdc, imgInstr, instruccionesX, startY, btnAncho, btnAlto);

    // 4. BOTÓN "SALIR" (Esquina inferior derecha)
    int margen = 30;
    int salirX = ancho - btnAncho - margen;
    int salirY = alto - btnAlto - margen;
    HBITMAP imgSalir = (hBmpBtnSalir) ? hBmpBtnSalir : hBmpBoton;
    DibujarImagen(hdc, imgSalir, salirX, salirY, btnAncho, btnAlto);

    // 5. TEXTO EN LOS BOTONES (si usas botones genéricos)
    if (hBmpBoton == imgJugar || hBmpBoton == imgPartidas ||
        hBmpBoton == imgInstr || hBmpBoton == imgSalir)
    {
        SetBkMode(hdc, TRANSPARENT);
        HFONT hFont = CreateFont(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                 CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                 VARIABLE_PITCH, "Arial");
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        // Texto para cada botón
        SetTextColor(hdc, RGB(255, 255, 255));

        // Botón Partidas
        const char *txtPartidas = "PARTIDAS";
        SIZE sizePartidas;
        GetTextExtentPoint32(hdc, txtPartidas, strlen(txtPartidas), &sizePartidas);
        int txtPX = partidasX + (btnAncho - sizePartidas.cx) / 2;
        int txtPY = startY + (btnAlto - sizePartidas.cy) / 2;
        TextOut(hdc, txtPX, txtPY, txtPartidas, strlen(txtPartidas));

        // Botón Jugar
        const char *txtJugar = "JUGAR";
        SIZE sizeJugar;
        GetTextExtentPoint32(hdc, txtJugar, strlen(txtJugar), &sizeJugar);
        int txtJX = jugarX + (btnAncho - sizeJugar.cx) / 2;
        int txtJY = startY + (btnAlto - sizeJugar.cy) / 2;
        TextOut(hdc, txtJX, txtJY, txtJugar, strlen(txtJugar));

        // Botón Instrucciones
        const char *txtInst = "INSTRUCCIONES";
        SIZE sizeInst;
        GetTextExtentPoint32(hdc, txtInst, strlen(txtInst), &sizeInst);
        int txtIX = instruccionesX + (btnAncho - sizeInst.cx) / 2;
        int txtIY = startY + (btnAlto - sizeInst.cy) / 2;
        TextOut(hdc, txtIX, txtIY, txtInst, strlen(txtInst));

        // Botón Salir
        const char *txtSalir = "SALIR";
        SIZE sizeSalir;
        GetTextExtentPoint32(hdc, txtSalir, strlen(txtSalir), &sizeSalir);
        int txtSX = salirX + (btnAncho - sizeSalir.cx) / 2;
        int txtSY = salirY + (btnAlto - sizeSalir.cy) / 2;
        TextOut(hdc, txtSX, txtSY, txtSalir, strlen(txtSalir));

        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
    }
}

// Función auxiliar para detectar colisión simple (Local)
int puntoEnRect(int x, int y, int rx, int ry, int rw, int rh)
{
    return (x >= rx && x <= rx + rw && y >= ry && y <= ry + rh);
}

// --- PROCESAR CLIC EN EL MENÚ ---
void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    int ancho = rc.right;
    int alto = rc.bottom;

    // Configuración de botones (debe coincidir con dibujarMenuConSprites)
    int btnAncho = 500;
    int btnAlto = 200;
    int separacion = 50;
    int centroY = alto / 2;
    int startY = centroY - (btnAlto / 2);
    int margen = 30;

    // --- BOTÓN "PARTIDAS" (Izquierda) ---
    int partidasX = (ancho / 2) - btnAncho - separacion;
    if (puntoEnRect(x, y, partidasX, startY, btnAncho, btnAlto))
    {
        MessageBox(hwnd, "Sistema de guardado en desarrollo.", "Partidas", MB_OK | MB_ICONINFORMATION);
        return;
    }

    // --- BOTÓN "JUGAR" (Centro) ---
    int jugarX = (ancho - btnAncho) / 2;
    if (puntoEnRect(x, y, jugarX, startY, btnAncho, btnAlto))
    {
        estado->estadoActual = ESTADO_SELECCION_MAPA;
        estado->opcionSeleccionada = 0;
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }

    // --- BOTÓN "INSTRUCCIONES" (Derecha) ---
    int instruccionesX = (ancho / 2) + separacion;
    if (puntoEnRect(x, y, instruccionesX, startY, btnAncho, btnAlto))
    {
        char *msg = "CONTROLES:\n\n"
                    "W, A, S, D: Mover personaje\n"
                    "ESPACIO: Accion (Talar, Minar, Abrir)\n"
                    "BLOQ MAYUS: Abrir Mochila\n"
                    "T: Abrir Tienda (Cerca del edificio)\n"
                    "TAB: Equipar/Quitar Armadura\n"
                    "1, 2, 3: Equipar Herramientas\n"
                    "R: Curar vida (Usa Hojas)\n\n"
                    "OBJETIVO: Recolecta recursos y mejora tu mochila!";
        MessageBox(hwnd, msg, "Instrucciones", MB_OK | MB_ICONQUESTION);
        return;
    }

    // --- BOTÓN "SALIR" (Esquina inferior derecha) ---
    int salirX = ancho - btnAncho - margen;
    int salirY = alto - btnAlto - margen;

    if (puntoEnRect(x, y, salirX, salirY, btnAncho, btnAlto))
    {
        if (MessageBox(hwnd, "Seguro que quieres salir?", "Salir", MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            PostQuitMessage(0);
        }
    }
}

// --- DIBUJADO DE LA PANTALLA DE SELECCIÓN DE MAPA ---
void dibujarSeleccionMapa(HDC hdc, HWND hwnd, EstadoJuego *estado)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    int ancho = rect.right;
    int alto = rect.bottom;

    // 1. DIBUJAR FONDO
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
        // Fondo oscuro por defecto
        HBRUSH fondo = CreateSolidBrush(RGB(20, 20, 30));
        FillRect(hdc, &rect, fondo);
        DeleteObject(fondo);
    }

    // 2. TÍTULO SUPERIOR
    SetBkMode(hdc, TRANSPARENT);

    // Fuente Grande para el Título
    HFONT hFontTitulo = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                   DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                   CLEARTYPE_QUALITY, VARIABLE_PITCH, "Arial");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFontTitulo);

    SetTextColor(hdc, RGB(255, 215, 0)); // Dorado
    const char *titulo = "SELECCIONA TU DESTINO";

    // Centrar texto (cálculo aproximado o usando GetTextExtentPoint32)
    SIZE size;
    GetTextExtentPoint32(hdc, titulo, strlen(titulo), &size);
    TextOut(hdc, (ancho - size.cx) / 2, 50, titulo, strlen(titulo));

    SelectObject(hdc, hOldFont);
    DeleteObject(hFontTitulo);

    // 3. DIBUJAR LAS 3 OPCIONES DE MAPA
    int cuadroAncho = 300;
    int cuadroAlto = 200;
    int separacion = 50;

    // Calcular inicio para centrar los 3 bloques
    int totalAncho = (cuadroAncho * 3) + (separacion * 2);
    int startX = (ancho - totalAncho) / 2;
    int startY = (alto - cuadroAlto) / 2;

    for (int i = 0; i < 3; i++)
    {
        int x = startX + (i * (cuadroAncho + separacion));
        int y = startY;
        BOOL seleccionado = (estado->opcionSeleccionada == i);

        // A. Seleccionar Imagen (Normal vs Seleccionada)
        HBITMAP img = NULL;
        switch (i)
        {
        case 0:
            img = seleccionado ? hBmpCuadroMapa1Sel : hBmpCuadroMapa1Normal;
            break;
        case 1:
            img = seleccionado ? hBmpCuadroMapa2Sel : hBmpCuadroMapa2Normal;
            break;
        case 2:
            img = seleccionado ? hBmpCuadroMapa3Sel : hBmpCuadroMapa3Normal;
            break;
        }

        // B. Dibujar Imagen
        if (img)
        {
            DibujarImagen(hdc, img, x, y, cuadroAncho, cuadroAlto);
        }
        else
        {
            // Placeholder si falla la imagen
            HBRUSH pBrush = CreateSolidBrush(seleccionado ? RGB(100, 100, 200) : RGB(50, 50, 50));
            RECT rP = {x, y, x + cuadroAncho, y + cuadroAlto};
            FillRect(hdc, &rP, pBrush);
            DeleteObject(pBrush);
        }

        // C. Dibujar Marco (Dorado si seleccionado, Gris si no)
        int bordeGrosor = seleccionado ? 4 : 2;
        COLORREF colorBorde = seleccionado ? RGB(255, 215, 0) : RGB(100, 100, 100);

        HPEN hPen = CreatePen(PS_SOLID, bordeGrosor, colorBorde);
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);
        HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

        Rectangle(hdc, x - bordeGrosor, y - bordeGrosor, x + cuadroAncho + bordeGrosor, y + cuadroAlto + bordeGrosor);

        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);

        // D. Texto del Nombre del Mapa
        char nombre[32];
        switch (i)
        {
        case 0:
            strcpy(nombre, "MAPA 1: ISLAS DEL MAR");
            break;
        case 1:
            strcpy(nombre, "MAPA 2: ARCHIPIELAGO");
            break;
        case 2:
            strcpy(nombre, "MAPA 3: CONTINENTE");
            break;
        }

        SetTextColor(hdc, seleccionado ? RGB(255, 255, 255) : RGB(150, 150, 150));
        TextOut(hdc, x + 10, y + cuadroAlto + 15, nombre, strlen(nombre));
    }

    // 4. INSTRUCCIONES INFERIORES
    SetTextColor(hdc, RGB(200, 200, 200));
    const char *instr = "Haz Clic para seleccionar  |  Presiona ENTER para Iniciar  |  ESC para Volver";

    // Centrar instrucciones
    SIZE sizeI;
    GetTextExtentPoint32(hdc, instr, strlen(instr), &sizeI);
    TextOut(hdc, (ancho - sizeI.cx) / 2, alto - 80, instr, strlen(instr));
}

// --- PROCESAR CLIC EN SELECCIÓN DE MAPA ---
void procesarClickSeleccionMapa(int x, int y, HWND hwnd, EstadoJuego *estado)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    int ancho = rc.right;
    int alto = rc.bottom;

    // Configuración de dimensiones (Debe ser IDÉNTICA a dibujarSeleccionMapa)
    int cuadroAncho = 300;
    int cuadroAlto = 200;
    int separacion = 50;

    // Recálculo de posiciones dinámicas
    int totalAncho = (cuadroAncho * 3) + (separacion * 2);
    int startX = (ancho - totalAncho) / 2;
    int startY = (alto - cuadroAlto) / 2;

    // Verificar clic en cada uno de los 3 mapas
    for (int i = 0; i < 3; i++)
    {
        int boxX = startX + (i * (cuadroAncho + separacion));
        int boxY = startY;

        // Detección de colisión (Punto dentro de Rectángulo)
        if (x >= boxX && x <= boxX + cuadroAncho &&
            y >= boxY && y <= boxY + cuadroAlto)
        {
            // Solo actualizamos si cambiamos de selección
            if (estado->opcionSeleccionada != i)
            {
                estado->opcionSeleccionada = i;

                // Sonido de selección (Opcional, usa sonido de sistema por defecto)
                PlaySound("SystemSelect", NULL, SND_ASYNC);

                // Forzar redibujado inmediato para ver el marco dorado
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return; // Ya encontramos el clic, salimos
        }
    }
}

// --- PROCESAR TECLA ENTER EN EL MENÚ ---
void procesarEnterMenu(HWND hwnd, EstadoJuego *estado)
{
    // Solo actuamos si estamos en el menú principal
    if (estado->estadoActual != ESTADO_MENU)
        return;

    switch (estado->opcionSeleccionada)
    {
    case 0: // JUGAR
        // Sonido de inicio
        PlaySound("SystemStart", NULL, SND_ASYNC);

        // Cambiar de estado
        estado->estadoActual = ESTADO_SELECCION_MAPA;
        estado->opcionSeleccionada = 0; // Pre-seleccionar el primer mapa por defecto

        // Redibujar
        InvalidateRect(hwnd, NULL, FALSE);
        break;

    case 1: // PARTIDAS
        MessageBox(hwnd, "Sistema de guardado en desarrollo.", "Partidas", MB_OK | MB_ICONINFORMATION);
        break;

    case 2: // INSTRUCCIONES
    {
        char *msg = "CONTROLES:\n\n"
                    "W, A, S, D: Mover personaje\n"
                    "ESPACIO: Accion (Talar, Minar, Abrir)\n"
                    "I: Abrir Mochila\n"
                    "T: Abrir Tienda (Cerca del edificio)\n"
                    "1, 2, 3: Equipar Herramientas\n"
                    "R: Curar vida (Usa Hojas)\n\n"
                    "OBJETIVO: Recolecta recursos y mejora tu mochila!";
        MessageBox(hwnd, msg, "Instrucciones", MB_OK | MB_ICONQUESTION);
    }
    break;

    case 3: // SALIR
        if (MessageBox(hwnd, "Seguro que quieres salir?", "Salir", MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            PostQuitMessage(0);
        }
        break;
    }
}

// --- 10. RENDERIZADO FINAL ---

void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera cam, int ancho, int alto, int frameTienda, int mapaId)
{
    // 1. FONDO DE AGUA (MEJORADO: Profundo vs Costa)

    // A) Pintamos TODO de "Agua Profunda" (Azul Oscuro)
    HBRUSH aguaProfunda = CreateSolidBrush(RGB(0, 0, 120)); // Azul marino oscuro
    RECT r = {0, 0, ancho, alto};
    FillRect(hdc, &r, aguaProfunda);
    DeleteObject(aguaProfunda);

    // B) Pintamos la "Zona Segura" (Agua Clara) como un círculo gigante
    // El centro es (1600, 1600) y el radio es 900 (mismo que en la lógica de movimiento)
    int radio = 900;
    int centroX = 1600;
    int centroY = 1600;

    // Convertir a coordenadas de pantalla
    int screenX = (centroX - cam.x) * cam.zoom;
    int screenY = (centroY - cam.y) * cam.zoom;
    int radioZoom = radio * cam.zoom;

    // Crear brocha de agua clara (la original)
    HBRUSH aguaCosta = CreateSolidBrush(RGB(0, 100, 180));
    HGDIOBJ oldBrush = SelectObject(hdc, aguaCosta);

    // Quitamos el borde del círculo (NULL_PEN) para que se vea suave
    HPEN nullPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
    HGDIOBJ oldPen = SelectObject(hdc, nullPen);

    // Dibujar el círculo de agua segura
    Ellipse(hdc, screenX - radioZoom, screenY - radioZoom, screenX + radioZoom, screenY + radioZoom);

    // Restaurar objetos GDI
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(aguaCosta);
    DeleteObject(nullPen);

    // 2. ISLAS (Tierra firme)
    for (int i = 0; i < MAX_ISLAS; i++)
    {
        if (!misIslas[i].activa)
            continue;

        int sx = (misIslas[i].x - cam.x) * cam.zoom;
        int sy = (misIslas[i].y - cam.y) * cam.zoom;

        // Optimización: No dibujar si está fuera de pantalla
        if (sx + (misIslas[i].ancho * cam.zoom) < 0 || sx > ancho)
            continue;

        HBITMAP img = obtenerImagenIsla(i, mapaId);
        if (img)
            DibujarImagen(hdc, img, sx, sy, misIslas[i].ancho * cam.zoom, misIslas[i].alto * cam.zoom);
    }
    // 2. DIBUJAR TIBURONES (NUEVO BLOQUE)
    dibujarTiburones(hdc, cam, ancho, alto);

    // 3. OBJETOS ESTÁTICOS (Parte del escenario)
    dibujarTiendasEnIslas(hdc, cam, ancho, alto, frameTienda);
    dibujarArboles(hdc, cam, ancho, alto, mapaId);
    dibujarMuelleYFlota(hdc, cam, &miJugador);

    // NOTA: Las vacas, minas y el jugador se dibujan en main.c
    // para asegurar que queden ENCIMA del mapa.
}