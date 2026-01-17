/* src/mapa/mapa.c */
#include "mapa.h"
#include "fauna.h"
#include "naturaleza.h"
#include "edificios.h"
#include "../global.h"
#include "../recursos/recursos.h"
#include "../jugador/jugador.h"
#include "../unidades/unidades.h"
#include "../sistema/tienda.h"
#include <stdio.h>
#include <mmsystem.h>
#include <math.h>
#include <stdbool.h>

// --- VARIABLES GLOBALES INTERNAS ---
Particula chispas[MAX_PARTICULAS];
Isla misIslas[MAX_ISLAS];
TextoFlotante listaTextos[MAX_TEXTOS] = {0};
char neblina[MUNDO_FILAS][MUNDO_COLUMNAS];
char mapa[MUNDO_FILAS][MUNDO_COLUMNAS];
// Referencias externas necesarias
extern Jugador miJugador;
int screenShake = 0;

// --- 1. SISTEMA DE ISLAS Y RECURSOS GRÁFICOS ---

HBITMAP obtenerImagenIsla(int indiceIsla, int mapaId)
{
    // ISLAS ENEMIGAS (índices 5, 6, 7)
    if (indiceIsla >= 5 && indiceIsla < MAX_ISLAS) {
        switch (mapaId) {
            case 0: // MAPA 1 (ISLAS DEL MAR)
                switch (indiceIsla) {
                    case 5: return hBmpIslaEnemigaGrandeMapa1;   // Grande Inf Der
                    case 6: return hBmpIslaEnemigaSec1Mapa1;     // Sec Sup Izq
                    case 7: return hBmpIslaEnemigaSec2Mapa1;     // Sec Sup Der
                }
                break;
            case 1: // MAPA 2 (ARCHIPIÉLAGO)
                switch (indiceIsla) {
                    case 5: return hBmpIslaEnemigaGrandeMapa2;   // Grande Inf Der
                    case 6: return hBmpIslaEnemigaSec1Mapa2;     // Sec Sup Izq
                    case 7: return hBmpIslaEnemigaSec2Mapa2;     // Sec Sup Der
                }
                break;
            case 2: // MAPA 3 (CONTINENTE)
                switch (indiceIsla) {
                    case 5: return hBmpIslaEnemigaGrandeMapa3;   // Grande Inf Der
                    case 6: return hBmpIslaEnemigaSec1Mapa3;     // Sec Sup Izq
                    case 7: return hBmpIslaEnemigaSec2Mapa3;     // Sec Sup Der
                }
                break;
        }
    }
    
    // ISLAS DEL JUGADOR (índices 0-4)
    switch (mapaId) {
        case 0: // MAPA 1
            switch (indiceIsla) {
                case 0: return hBmpIslaGrande;
                case 1: return hBmpIslaSec1;
                case 2: return hBmpIslaSec2;
                case 3: return hBmpIslaSec3;
                case 4: return hBmpIslaSec4;
            }
            break;
        case 1: // MAPA 2
            switch (indiceIsla) {
                case 0: return hBmpIslaGrandeMapa2;
                case 1: return hBmpIslaSec1Mapa2;
                case 2: return hBmpIslaSec2Mapa2;
                case 3: return hBmpIslaSec3Mapa2;
                case 4: return hBmpIslaSec4Mapa2;
            }
            break;
        case 2: // MAPA 3
            switch (indiceIsla) {
                case 0: return hBmpIslaGrandeMapa3;
                case 1: return hBmpIslaSec1Mapa3;
                case 2: return hBmpIslaSec2Mapa3;
                case 3: return hBmpIslaSec3Mapa3;
                case 4: return hBmpIslaSec4Mapa3;
            }
            break;
    }
    return NULL;
}

void inicializarIslas(int mapaId)
{
    for (int i = 0; i < MAX_ISLAS; i++)
        misIslas[i].activa = 0;

    // ====================================================
    // ISLAS DEL JUGADOR (5 islas originales - CENTRO/NORTE)
    // ====================================================

    // ISLA CENTRAL (Grande) - BASE DEL JUGADOR
    misIslas[0].activa = 1;
    misIslas[0].x = 1100;
    misIslas[0].y = 1100;
    misIslas[0].ancho = 1000;
    misIslas[0].alto = 1000;
    misIslas[0].esEnemiga = 0;

    // ISLA NORTE - RECURSOS DEL JUGADOR
    misIslas[1].activa = 1;
    misIslas[1].x = 1440;
    misIslas[1].y = 600;
    misIslas[1].ancho = 320;
    misIslas[1].alto = 230;
    misIslas[1].esEnemiga = 0;

    // ISLA SUR - RECURSOS DEL JUGADOR
    misIslas[2].activa = 1;
    misIslas[2].x = 1350;
    misIslas[2].y = 2300;
    misIslas[2].ancho = 700;
    misIslas[2].alto = 700;
    misIslas[2].esEnemiga = 0;

    // ISLA OESTE - RECURSOS DEL JUGADOR
    misIslas[3].activa = 1;
    misIslas[3].x = 2620;
    misIslas[3].y = 905;
    misIslas[3].ancho = 400;
    misIslas[3].alto = 400;
    misIslas[3].esEnemiga = 0;

    // ISLA ESTE - RECURSOS DEL JUGADOR
    misIslas[4].activa = 1;
    misIslas[4].x = 2575;
    misIslas[4].y = 1925;
    misIslas[4].ancho = 500;
    misIslas[4].alto = 500;
    misIslas[4].esEnemiga = 0;

    // ====================================================
    // ISLAS ENEMIGAS (3 nuevas - ESQUINAS/EXTREMOS)
    // ====================================================

    // 1. ISLA ENEMIGA GRANDE (Esquina INFERIOR DERECHA)
    misIslas[5].activa = 1;
    misIslas[5].x = 90;    // Esquina derecha
    misIslas[5].y = 2200;    // Esquina inferior
    misIslas[5].ancho = 900;
    misIslas[5].alto = 900;
    misIslas[5].esEnemiga = 1;

    // 2. ISLA ENEMIGA 2 (Esquina SUPERIOR IZQUIERDA)
    misIslas[6].activa = 1;
    misIslas[6].x = 50;     // Esquina izquierda
    misIslas[6].y = 1150;     // Esquina superior
    misIslas[6].ancho = 500; // Tamaño de isla secundaria
    misIslas[6].alto = 500;
    misIslas[6].esEnemiga = 1;

    // 3. ISLA ENEMIGA 3 (Esquina SUPERIOR DERECHA)
    misIslas[7].activa = 1;
    misIslas[7].x = 300;    // Esquina derecha
    misIslas[7].y = 300;     // Esquina superior
    misIslas[7].ancho = 400; // Tamaño de isla secundaria
    misIslas[7].alto = 400;
    misIslas[7].esEnemiga = 1;
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

// Genera la matriz de colisión
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

    // 2. Escaneamos punto por punto 
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
void dibujarInterfazGuerra(HDC hdc, int ancho) {
    int enemigosVivos = 0;
    int soldadosVivos = 0;

    // Contar unidades
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (unidades[i].activa) {
            if (unidades[i].bando == BANDO_ENEMIGO) enemigosVivos++;
            if (unidades[i].tipo == TIPO_SOLDADO && unidades[i].bando == BANDO_ALIADO) soldadosVivos++;
        }
    }

    // Solo dibujar si hay enemigos 
    if (enemigosVivos > 0) {
        // Fondo Rojo Superior
        RECT r = {0, 0, ancho, 40};
        HBRUSH brochaAlerta = CreateSolidBrush(RGB(150, 0, 0));
        FillRect(hdc, &r, brochaAlerta);
        DeleteObject(brochaAlerta);

        // Texto "MODO BATALLA"
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 0)); // Amarillo
        
        char texto[128];
        sprintf(texto, " MODO BATALLA  Enemigos: %d   |   Tus Soldados: %d", enemigosVivos, soldadosVivos);
        
        // Centrar texto (aprox)
        TextOut(hdc, (ancho / 2) - 150, 10, texto, strlen(texto));
        
        // Borde parpadeante (opcional)
        static int flash = 0; flash++;
        if ((flash / 10) % 2 == 0) {
            HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
            HGDIOBJ old = SelectObject(hdc, hPen);
            MoveToEx(hdc, 0, 38, NULL); LineTo(hdc, ancho, 38);
            SelectObject(hdc, old); DeleteObject(hPen);
        }
    }
}
void inicializarNieblaTotal() {
    for (int y = 0; y < MUNDO_FILAS; y++) {
        for (int x = 0; x < MUNDO_COLUMNAS; x++) {
            neblina[y][x] = 0; 
        }
    }
}

void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int mapaId)
{
    inicializarIslas(mapaId);
    generarColisionDeMapaCompleto(mapa, mapaId);
    inicializarNieblaTotal(); 
}

int EsSuelo(int x, int y, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS])
{
    int col = x / TAMANO_CELDA_BASE;
    int fila = y / TAMANO_CELDA_BASE;
    if (fila < 0 || fila >= MUNDO_FILAS || col < 0 || col >= MUNDO_COLUMNAS)
        return 0;
    return (mapa[fila][col] == 1);
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

void crearChispaBlanca(float x, float y) { crearChispas((int)x, (int)y, RGB(255, 255, 255)); }

// --- VARIABLES GLOBALES INTERNAS ---
// Usamos 'particulas' para TODO (sangre, magia, chispas, cañones)
extern Particula particulas[MAX_PARTICULAS]; 

// ---------------------------------------------------------
// SISTEMA DE PARTÍCULAS UNIFICADO
// ---------------------------------------------------------

// 1. CHISPAS 
void crearChispas(int x, int y, COLORREF color) {
    for (int i = 0; i < 5; i++) { // 5 chispas por impacto
        for (int k = 0; k < MAX_PARTICULAS; k++) {
            if (!particulas[k].activo) {
                particulas[k].activo = 1;
                particulas[k].tipo = -1; // -1 = Chispa genérica
                particulas[k].x = (float)x;
                particulas[k].y = (float)y;
                // Velocidad aleatoria
                particulas[k].vx = (rand() % 10 - 5) * 0.5f; 
                particulas[k].vy = (rand() % 10 - 5) * 0.5f;
                particulas[k].vida = 15; // Duran poco
                particulas[k].color = color;
                break;
            }
        }
    }
}

// 2. SANGRE (Para daño a unidades)
void crearSangre(float x, float y) {
    for (int i = 0; i < 8; i++) { 
        for (int k = 0; k < MAX_PARTICULAS; k++) {
            if (!particulas[k].activo) {
                particulas[k].activo = 1;
                particulas[k].tipo = PART_SANGRE; 
                particulas[k].x = x;
                particulas[k].y = y;
                particulas[k].vx = (rand() % 10 - 5) * 0.6f; 
                particulas[k].vy = (rand() % 10 - 5) * 0.6f;
                particulas[k].vida = 30; 
                particulas[k].color = RGB(200, 0, 0); 
                break;
            }
        }
    }
}

// 3. PROYECTIL MÁGICO (Mago)
void crearProyectilMagico(float x, float y, float destX, float destY) {
    for (int k = 0; k < MAX_PARTICULAS; k++) {
        if (!particulas[k].activo) {
            particulas[k].activo = 1;
            particulas[k].tipo = PART_MAGIA; 
            particulas[k].x = x;
            particulas[k].y = y;
            
            float dx = destX - x;
            float dy = destY - y;
            float dist = sqrt(dx*dx + dy*dy);
            float velocidad = 7.0f; 
            
            particulas[k].vx = (dx / dist) * velocidad;
            particulas[k].vy = (dy / dist) * velocidad;
            particulas[k].vida = 30; 
            particulas[k].color = RGB(0, 255, 255); // Cian
            break; 
        }
    }
}

// 4. BALA DE CAÑÓN (Barcos)
void crearBalaCanon(float x, float y, float destX, float destY) {
    for (int k = 0; k < MAX_PARTICULAS; k++) {
        if (!particulas[k].activo) {
            particulas[k].activo = 1;
            particulas[k].tipo = PART_BALA_CANON;
            particulas[k].x = x;
            particulas[k].y = y;
            particulas[k].destinoX = destX; // Guardamos destino para saber cuándo explota
            particulas[k].destinoY = destY;
            
            float dx = destX - x;
            float dy = destY - y;
            float dist = sqrt(dx*dx + dy*dy);
            float velocidad = 6.0f; 
            
            particulas[k].vx = (dx / dist) * velocidad;
            particulas[k].vy = (dy / dist) * velocidad;
            particulas[k].vida = (int)(dist / velocidad); // Vida exacta para llegar
            particulas[k].color = RGB(0, 0, 0); // Negra
            break;
        }
    }
}

// 5. EXPLOSIÓN DE AGUA (Al impactar bala)
void crearExplosionAgua(float x, float y) {
    for (int i = 0; i < 10; i++) {
        for (int k = 0; k < MAX_PARTICULAS; k++) {
            if (!particulas[k].activo) {
                particulas[k].activo = 1;
                particulas[k].tipo = -1; // Como chispa pero blanca
                particulas[k].x = x;
                particulas[k].y = y;
                particulas[k].vx = (rand() % 10 - 5) * 0.8f;
                particulas[k].vy = (rand() % 10 - 5) * 0.8f;
                particulas[k].vida = 25;
                particulas[k].color = RGB(200, 200, 255); // Espuma
                break;
            }
        }
    }
}

// ---------------------------------------------------------
// ACTUALIZACIÓN Y DIBUJADO UNIFICADO
// ---------------------------------------------------------
void actualizarYDibujarParticulas(HDC hdc, Camera cam) {
    for (int i = 0; i < MAX_PARTICULAS; i++) {
        if (!particulas[i].activo) continue;

        // FÍSICA
        particulas[i].x += particulas[i].vx;
        particulas[i].y += particulas[i].vy;
        particulas[i].vida--;

        // Lógica especial para Bala de Cañón
        if (particulas[i].tipo == PART_BALA_CANON && particulas[i].vida <= 0) {
            crearExplosionAgua(particulas[i].x, particulas[i].y);
            particulas[i].activo = 0;
            continue;
        }

        if (particulas[i].vida <= 0) {
            particulas[i].activo = 0;
            continue;
        }

        // DIBUJADO
        int sx = (int)((particulas[i].x - cam.x) * cam.zoom);
        int sy = (int)((particulas[i].y - cam.y) * cam.zoom);
        
        // Si sale de pantalla, no dibujar (optimización)
        if (sx < -10 || sx > 2000 || sy < -10 || sy > 1500) continue;

        if (particulas[i].tipo == PART_BALA_CANON) {
            // Bala grande
            HBRUSH brocha = CreateSolidBrush(RGB(0, 0, 0));
            HGDIOBJ old = SelectObject(hdc, brocha);
            Ellipse(hdc, sx, sy, sx + (int)(8*cam.zoom), sy + (int)(8*cam.zoom));
            SelectObject(hdc, old);
            DeleteObject(brocha);
        } 
        else if (particulas[i].tipo == PART_MAGIA) {
            // Magia brillante
            HBRUSH brocha = CreateSolidBrush(RGB(0, 255, 255));
            RECT r = {sx, sy, sx + (int)(6*cam.zoom), sy + (int)(6*cam.zoom)};
            FillRect(hdc, &r, brocha);
            DeleteObject(brocha);
        }
        else {
            // Chispas, Sangre y Explosiones (Pixels)
            int size = (int)(2 * cam.zoom);
            // Dibujar un cuadrado pequeño del color correspondiente
            HBRUSH brocha = CreateSolidBrush(particulas[i].color);
            RECT r = {sx, sy, sx + size, sy + size};
            FillRect(hdc, &r, brocha);
            DeleteObject(brocha);
        }
    }
}


// --- 4. ENTIDADES DEL JUEGO (VACAS, ARBOLES, MINAS, TESOROS) ---

void dibujarEstablo(HDC hdc, Camera cam)
{
	int col = (int)((ESTABLO_X + 100) / TAMANO_CELDA_BASE);
    int fila = (int)((ESTABLO_Y + 100) / TAMANO_CELDA_BASE);

    // 2. FILTRO DE NIEBLA
    if (col >= 0 && col < MUNDO_COLUMNAS && fila >= 0 && fila < MUNDO_FILAS) {
        if (neblina[fila][col] == 0) return; // Si está en la oscuridad, ni lo intentamos dibujar
    }
    int sx = (int)((ESTABLO_X - cam.x) * cam.zoom);
    int sy = (int)((ESTABLO_Y - cam.y) * cam.zoom);
    if (hBmpEstablo)
        DibujarImagen(hdc, hBmpEstablo, sx, sy, 200 * cam.zoom, 200 * cam.zoom);
}

// --- 5. LÓGICA DE JUEGO GENERAL ---

void descubrirMapa(float centroX, float centroY, float radio) {
    int radioCeldas = (int)(radio / TAMANO_CELDA_BASE) + 2;
    int cx = (int)(centroX / TAMANO_CELDA_BASE);
    int cy = (int)(centroY / TAMANO_CELDA_BASE);

    int minX = max(0, cx - radioCeldas);
    int maxX = min(MUNDO_COLUMNAS - 1, cx + radioCeldas);
    int minY = max(0, cy - radioCeldas);
    int maxY = min(MUNDO_FILAS - 1, cy + radioCeldas);

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            if (neblina[y][x] == 0) { // Si está oculto
                float dx = (x * TAMANO_CELDA_BASE) + 8 - centroX;
                float dy = (y * TAMANO_CELDA_BASE) + 8 - centroY;
                if (dx*dx + dy*dy < radio*radio) {
                    neblina[y][x] = 1; // ¡DESCUBIERTO!
                }
            }
        }
    }
}

void actualizarLogicaSistema(Jugador *j)
{
    // Pesca (Si está en el bote de pesca)
    if (j->estadoBarco == 1)
    {
        j->timerPesca++;
        if (j->timerPesca >= 180) // 3 segundos
        { 
            j->timerPesca = 0;

            // --- CAMBIO: AHORA PESCAMOS 'PESCADO', NO 'COMIDA' ---
            int ant = j->pescado; 
            agregarRecurso(&j->pescado, 1, j->nivelMochila); // 1 Pescado por vez
            int gan = j->pescado - ant;

            if (gan > 0) {
                // Mensaje visual con color Cyan
                crearTextoFlotante(j->x, j->y, "+1 Pescado", 0, RGB(0, 255, 255));
            }
            else {
                crearTextoFlotante(j->x, j->y - 40, "Mochila Llena!", 0, RGB(255, 50, 50));
            }
        }
    }
    else {
        j->timerPesca = 0;
    }
    
    actualizarTiburones(j);
    descubrirMapa(j->x + 16, j->y + 16, 100.0f);
}

void actualizarRegeneracionRecursos()
{
    actualizarRegeneracionNaturaleza();

    actualizarRegeneracionFauna();
}

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
    inicializarEdificios();

    jugador->x = 1600;
    jugador->y = 1600;
    jugador->velocidad = 5;
    jugador->vidaActual = 100;
    jugador->vidaMax = 100;
    jugador->nivel = 1;
    jugador->experiencia = 0;
    jugador->experienciaSiguienteNivel = 100;

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

    // 1. DIBUJAR FONDO (sin cambios)
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
    int tituloY = alto / 6; 

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
    int centroY = alto / 2;               // Centro de la pantalla
    int startY = centroY - (btnAlto / 2); // Y donde empiezan PARTIDAS e INSTRUCCIONES

    // --- BOTÓN "PARTIDAS" (Izquierda) ---
    int partidasX = (ancho / 2) - btnAncho - separacion;
    HBITMAP imgPartidas = (hBmpBtnPartidas) ? hBmpBtnPartidas : hBmpBoton;
    DibujarImagen(hdc, imgPartidas, partidasX, startY, btnAncho, btnAlto);

    // --- BOTÓN "JUGAR" (Centro - 150px MÁS ABAJO que los otros) ---
    int jugarX = (ancho - btnAncho) / 2;
    int jugarY = startY + 230; 
    HBITMAP imgJugar = (hBmpBtnJugar) ? hBmpBtnJugar : hBmpBoton;
    DibujarImagen(hdc, imgJugar, jugarX, jugarY, btnAncho, btnAlto);

    // --- BOTÓN "INSTRUCCIONES" (Derecha) ---
    int instruccionesX = (ancho / 2) + separacion;
    HBITMAP imgInstr = (hBmpBtnInstrucciones) ? hBmpBtnInstrucciones : hBmpBoton;
    DibujarImagen(hdc, imgInstr, instruccionesX, startY, btnAncho, btnAlto);

    // 4. BOTÓN "SALIR" (MÁS ABAJO y MÁS A LA DERECHA)
    int margenHorizontal = 0;  
    int margenVertical = 0;   
    int salirX = ancho - btnAncho - margenHorizontal;
    int salirY = alto - btnAlto - margenVertical;
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

        // Botón Jugar (AJUSTAR coordenada Y del texto)
        const char *txtJugar = "JUGAR";
        SIZE sizeJugar;
        GetTextExtentPoint32(hdc, txtJugar, strlen(txtJugar), &sizeJugar);
        int txtJX = jugarX + (btnAncho - sizeJugar.cx) / 2;
        int txtJY = jugarY + (btnAlto - sizeJugar.cy) / 2; // ← USAR jugarY
        TextOut(hdc, txtJX, txtJY, txtJugar, strlen(txtJugar));

        // Botón Instrucciones
        const char *txtInst = "INSTRUCCIONES";
        SIZE sizeInst;
        GetTextExtentPoint32(hdc, txtInst, strlen(txtInst), &sizeInst);
        int txtIX = instruccionesX + (btnAncho - sizeInst.cx) / 2;
        int txtIY = startY + (btnAlto - sizeInst.cy) / 2;
        TextOut(hdc, txtIX, txtIY, txtInst, strlen(txtInst));

        // Botón Salir (AJUSTAR coordenadas del texto)
        const char *txtSalir = "SALIR";
        SIZE sizeSalir;
        GetTextExtentPoint32(hdc, txtSalir, strlen(txtSalir), &sizeSalir);
        int txtSX = salirX + (btnAncho - sizeSalir.cx) / 2;
        int txtSY = salirY + (btnAlto - sizeSalir.cy) / 2; // ← USAR salirY
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

    // Configuración de botones (DEBE COINCIDIR con dibujarMenuConSprites)
    int btnAncho = 500;
    int btnAlto = 200;
    int separacion = 200; // ← IMPORTANTE: usar 200 como en dibujar
    int centroY = alto / 2;
    int startY = centroY - (btnAlto / 2);
    
    // Márgenes para SALIR (DEBE COINCIDIR)
    int margenHorizontal = 0;  // ← Mismo que en dibujar
    int margenVertical = 0;    // ← Mismo que en dibujar

    // --- BOTÓN "PARTIDAS" (Izquierda) ---
    int partidasX = (ancho / 2) - btnAncho - separacion;
    if (puntoEnRect(x, y, partidasX, startY, btnAncho, btnAlto))
    {
        MessageBox(hwnd, "Sistema de guardado en desarrollo.", "Partidas", MB_OK | MB_ICONINFORMATION);
        return;
    }

    // --- BOTÓN "JUGAR" (Centro - 150px más abajo) ---
    int jugarX = (ancho - btnAncho) / 2;
    int jugarY = startY + 230; // ← MISMO VALOR que en dibujar
    if (puntoEnRect(x, y, jugarX, jugarY, btnAncho, btnAlto))
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
                    "I: Abrir Mochila\n"
                    "T: Abrir Tienda (Cerca del edificio)\n"
                    "TAB: Equipar/Quitar Armadura\n"
                    "1, 2, 3: Equipar Herramientas\n"
                    "R: Curar vida (Usa Hojas)\n\n"
                    "OBJETIVO: Recolecta recursos y mejora tu mochila!";
        MessageBox(hwnd, msg, "Instrucciones", MB_OK | MB_ICONQUESTION);
        return;
    }

    // --- BOTÓN "SALIR" (MÁS ABAJO y MÁS A LA DERECHA) ---
    int salirX = ancho - btnAncho - margenHorizontal;
    int salirY = alto - btnAlto - margenVertical;

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
    const char *titulo = " ";

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

void dibujarMiniMapa(HDC hdc, Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int anchoPantalla, int altoPantalla) {
    // 1. Configuración
    int tamMapa = 150;
    int margen = 20;
    int bordeGrosor = 4; // Grosor del marco negro
    
    // Posición
    int xBase = anchoPantalla - tamMapa - margen;
    int yBase = altoPantalla - tamMapa - margen;

    // 2. DIBUJAR MARCO NEGRO (FONDO)
    // Dibujamos un rectángulo negro más grande primero
    RECT rMarco = {
        xBase - bordeGrosor, 
        yBase - bordeGrosor, 
        xBase + tamMapa + bordeGrosor, 
        yBase + tamMapa + bordeGrosor
    };
    HBRUSH brochaNegra = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &rMarco, brochaNegra);
    DeleteObject(brochaNegra);

    // 3. FONDO DEL MAPA (Interior color crema)
    HBRUSH fondoMapa = CreateSolidBrush(RGB(240, 230, 180)); 
    RECT rMapa = {xBase, yBase, xBase + tamMapa, yBase + tamMapa};
    FillRect(hdc, &rMapa, fondoMapa);
    DeleteObject(fondoMapa);

    // 4. TERRENO EXPLORADO
    float escalaX = (float)tamMapa / MUNDO_COLUMNAS;
    float escalaY = (float)tamMapa / MUNDO_FILAS;

    for (int y = 0; y < MUNDO_FILAS; y += 2) { // Optimizamos saltando de 2 en 2 píxeles
        for (int x = 0; x < MUNDO_COLUMNAS; x += 2) {
            
            // Solo si está explorado
            if (neblina[y][x] == 1) {
                int px = xBase + (int)(x * escalaX);
                int py = yBase + (int)(y * escalaY);
                
                COLORREF color;
                if (mapa[y][x] == 1) color = RGB(50, 150, 50); // Tierra (Verde)
                else color = RGB(60, 120, 200); // Agua (Azul)

                SetPixel(hdc, px, py, color);
                // Pintamos 4 pixeles para que se vea sólido (bloque 2x2)
                SetPixel(hdc, px+1, py, color);
                SetPixel(hdc, px, py+1, color);
                SetPixel(hdc, px+1, py+1, color);
            }
        }
    }

    // 5. TEXTOS Y PUNTOS CARDINALES
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0)); // Letras Negras
    // Dibujamos N, S, E, O dentro del marco o justo afuera
    TextOut(hdc, xBase + (tamMapa/2) - 4, yBase + 2, "N", 1); 
    TextOut(hdc, xBase + (tamMapa/2) - 4, yBase + tamMapa - 14, "S", 1); 
    TextOut(hdc, xBase + tamMapa - 12, yBase + (tamMapa/2) - 8, "E", 1); 
    TextOut(hdc, xBase + 4, yBase + (tamMapa/2) - 8, "O", 1); 

    // 6. JUGADOR (Punto Rojo)
    int jX = xBase + (int)((j->x / TAMANO_CELDA_BASE) * escalaX);
    int jY = yBase + (int)((j->y / TAMANO_CELDA_BASE) * escalaY);

    // Validar que el punto no se salga del cuadrito
    if (jX < xBase) jX = xBase;
    if (jX > xBase + tamMapa) jX = xBase + tamMapa;
    if (jY < yBase) jY = yBase;
    if (jY > yBase + tamMapa) jY = yBase + tamMapa;

    HBRUSH puntoRojo = CreateSolidBrush(RGB(255, 0, 0));
    RECT rJug = {jX - 3, jY - 3, jX + 4, jY + 4}; // Punto un poco más grande
    FillRect(hdc, &rJug, puntoRojo);
    DeleteObject(puntoRojo);

    // 7. FLECHA DE DIRECCIÓN (Hacia donde mira)
    HPEN plumaFlecha = CreatePen(PS_SOLID, 2, RGB(200, 0, 0));
    HGDIOBJ oldPen = SelectObject(hdc, plumaFlecha);
    
    int len = 10; 
    int endX = jX, endY = jY;

    if (j->direccion == DIR_ARRIBA)    endY -= len;
    else if (j->direccion == DIR_ABAJO)     endY += len;
    else if (j->direccion == DIR_IZQUIERDA) endX -= len;
    else if (j->direccion == DIR_DERECHA)   endX += len;

    MoveToEx(hdc, jX, jY, NULL);
    LineTo(hdc, endX, endY);

    SelectObject(hdc, oldPen);
    DeleteObject(plumaFlecha);
}

// --- 10. RENDERIZADO FINAL ---

void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera cam, int ancho, int alto, int frameTienda, int mapaId)
{
	
	
	// --- APLICAR TEMBLOR ---
    int shakeX = 0, shakeY = 0;
    if (screenShake > 0) {
        shakeX = (rand() % 10) - 5; // Mueve la cámara aleatoriamente
        shakeY = (rand() % 10) - 5;
        screenShake--; // El efecto se desvanece
    }
    
    // Usamos una cámara temporal para no afectar la lógica del juego, solo el dibujo
    Camera camEfecto = cam;
    camEfecto.x += shakeX;
    camEfecto.y += shakeY;
    // ---------------------------------------------------------
    // 1. CAPA FONDO: AGUA
    // ---------------------------------------------------------
    // A) Agua Profunda
    HBRUSH aguaProfunda = CreateSolidBrush(RGB(0, 0, 100));
    RECT r = {0, 0, ancho, alto};
    FillRect(hdc, &r, aguaProfunda);
    DeleteObject(aguaProfunda);

    // B) Agua Costa (Zona Segura)
    int radio = 900; 
    int centroX = 1600; int centroY = 1600;
    int screenX = (centroX - cam.x) * cam.zoom;
    int screenY = (centroY - cam.y) * cam.zoom;
    int radioZoom = radio * cam.zoom;

    HBRUSH aguaCosta = CreateSolidBrush(RGB(0, 100, 180));
    HGDIOBJ oldBrush = SelectObject(hdc, aguaCosta);
    HPEN nullPen = CreatePen(PS_NULL, 0, 0);
    HGDIOBJ oldPen = SelectObject(hdc, nullPen);
    
    Ellipse(hdc, screenX - radioZoom, screenY - radioZoom, screenX + radioZoom, screenY + radioZoom);
    
    SelectObject(hdc, oldPen); SelectObject(hdc, oldBrush);
    DeleteObject(aguaCosta); DeleteObject(nullPen);

    // ---------------------------------------------------------
    // 2. CAPA SUELO: ISLAS
    // ---------------------------------------------------------
        for (int i = 0; i < MAX_ISLAS; i++) {
        if (!misIslas[i].activa) continue;
        int sx = (misIslas[i].x - cam.x) * cam.zoom;
        int sy = (misIslas[i].y - cam.y) * cam.zoom;
        if (sx + (misIslas[i].ancho * cam.zoom) < 0 || sx > ancho) continue;

        HBITMAP img = obtenerImagenIsla(i, mapaId);
        if (img) {
            DibujarImagen(hdc, img, sx, sy, 
                         misIslas[i].ancho * cam.zoom, 
                         misIslas[i].alto * cam.zoom);
            
            // Opcional: Dibujar overlay rojo para islas enemigas
            if (misIslas[i].esEnemiga) {
                HBRUSH hRedOverlay = CreateSolidBrush(RGB(255, 0, 0));
                HBRUSH hOldBrush = SelectObject(hdc, hRedOverlay);
                SetBkMode(hdc, TRANSPARENT);
                
                // Modo de mezcla para semi-transparencia
                BLENDFUNCTION blend = {AC_SRC_OVER, 0, 128, AC_SRC_ALPHA};
                AlphaBlend(hdc, sx, sy, 
                          misIslas[i].ancho * cam.zoom, 
                          misIslas[i].alto * cam.zoom,
                          hdc, 0, 0, 1, 1, blend);
                
                SelectObject(hdc, hOldBrush);
                DeleteObject(hRedOverlay);
            }
        }
    }

    // ---------------------------------------------------------
    // 3. CAPA OBJETOS: NATURALEZA Y FAUNA
    // ---------------------------------------------------------
    // (Aquí dibujamos todo lo que debe poder ser tapado por la niebla)
    dibujarTiendasEnIslas(hdc, cam, ancho, alto, frameTienda);
    dibujarTesoros(hdc, cam, ancho, alto);
    dibujarMinas(hdc, cam, ancho, alto);
    dibujarArboles(hdc, cam, ancho, alto, mapaId);
    
    dibujarTiburones(hdc, cam, ancho, alto);
    dibujarBarcoAnclado(hdc, cam);
    dibujarVacas(hdc, cam, ancho, alto); // Ahora las vacas se dibujan ANTES de la niebla
    
    // Muelle y Tienda
    dibujarMuelleYFlota(hdc, cam, &miJugador); 
    // ---------------------------------------------------------
    // 4. CAPA PERSONAJES: JUGADOR Y UNIDADES
    // ---------------------------------------------------------
    dibujarUnidades(hdc, cam);
    dibujarJugador(hdc, &miJugador, cam);
    // ---------------------------------------------------------
    // 5. CAPA SUPERIOR: NIEBLA DE GUERRA
    // ---------------------------------------------------------
    // Al dibujarla al final, tapará todo lo anterior (vacas, arboles, etc)
    // donde neblina == 0.

    HBRUSH brochaNiebla = CreateSolidBrush(RGB(220, 220, 230)); // Gris nube
    HPEN sinBorde = CreatePen(PS_NULL, 0, 0);
	oldBrush = SelectObject(hdc, brochaNiebla); // Reutilizamos la variable existente
	oldPen = SelectObject(hdc, sinBorde);       // Reutilizamos la variable existente
    // Optimización: Solo recorrer lo que ve la cámara
    int inicioCol = max(0, (int)(cam.x / TAMANO_CELDA_BASE) - 2);
    int inicioFila = max(0, (int)(cam.y / TAMANO_CELDA_BASE) - 2);
    int celdasAncho = (int)(ancho / (TAMANO_CELDA_BASE * cam.zoom)) + 4;
    int celdasAlto = (int)(alto / (TAMANO_CELDA_BASE * cam.zoom)) + 4;
    int finCol = min(MUNDO_COLUMNAS, inicioCol + celdasAncho);
    int finFila = min(MUNDO_FILAS, inicioFila + celdasAlto);

    for (int f = inicioFila; f < finFila; f++) {
        for (int c = inicioCol; c < finCol; c++) {
            // SI ESTÁ OCULTO (0) 
            if (neblina[f][c] == 0) {
                int x = (int)(((c * TAMANO_CELDA_BASE) - cam.x) * cam.zoom);
                int y = (int)(((f * TAMANO_CELDA_BASE) - cam.y) * cam.zoom);
                int celdaTam = (int)(TAMANO_CELDA_BASE * cam.zoom);
                
                // Dibujar círculo expandido para efecto "nube esponjosa"
                int expansion = celdaTam / 2; 
                Ellipse(hdc, x - expansion, y - expansion, 
                             x + celdaTam + expansion, y + celdaTam + expansion);
            }
        }
    }

    SelectObject(hdc, oldPen); SelectObject(hdc, oldBrush);
    DeleteObject(brochaNiebla); DeleteObject(sinBorde);

    // ---------------------------------------------------------
    // 6. CAPA INTERFAZ: HUD (Siempre visible sobre la niebla)
    // ---------------------------------------------------------
    if (miJugador.tieneMapa) {
        dibujarMiniMapa(hdc, &miJugador, mapa, ancho, alto);
    }
	dibujarHUD(hdc, &miJugador, ancho, alto);
    
    // Textos flotantes (Números de daño, recursos, etc.)
    for (int i = 0; i < MAX_TEXTOS; i++) {
        if (listaTextos[i].activo) {
             int tx = (int)((listaTextos[i].x - cam.x) * cam.zoom);
             int ty = (int)((listaTextos[i].y - cam.y) * cam.zoom);
             
             // Sombra del texto
             SetTextColor(hdc, RGB(0,0,0));
             SetBkMode(hdc, TRANSPARENT);
             TextOut(hdc, tx+1, ty+1, listaTextos[i].texto, strlen(listaTextos[i].texto));
             
             // Texto real
             SetTextColor(hdc, listaTextos[i].color);
             TextOut(hdc, tx, ty, listaTextos[i].texto, strlen(listaTextos[i].texto));
        }
    }
    dibujarInterfazGuerra(hdc, ancho);
}
