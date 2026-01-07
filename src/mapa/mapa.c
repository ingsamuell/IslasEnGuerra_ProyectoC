/* src/mapa/mapa.c - VERSIÓN LIMPIA Y PROFESIONAL */
#include "mapa.h"                 
#include "../global.h"            
#include "../recursos/recursos.h" 
#include "../jugador/jugador.h"   
#include <stdio.h>
#include <mmsystem.h> 
#include <math.h>     
#include <stdbool.h>

// --- VARIABLES GLOBALES INTERNAS ---
Unidad unidades[MAX_UNIDADES];
Vaca manada[MAX_VACAS];
Mina misMinas[MAX_MINAS];
Particula chispas[MAX_PARTICULAS];
Isla misIslas[MAX_ISLAS];
TextoFlotante listaTextos[MAX_TEXTOS] = {0};
Arbol misArboles[MAX_ARBOLES];
Tesoro misTesoros[MAX_TESOROS];

// Referencias externas necesarias
extern Jugador miJugador; 

#define MARGEN_ESTABLO 100 

// --- 1. SISTEMA DE ISLAS Y RECURSOS GRÁFICOS ---

HBITMAP obtenerImagenIsla(int indiceIsla, int mapaId) {
    switch(mapaId) {
        case 0: // MAPA 1
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

void inicializarIslas(int mapaId)
{
    for (int i = 0; i < MAX_ISLAS; i++) misIslas[i].activa = 0;

    // ISLA CENTRAL (Grande)
    misIslas[0].activa = 1; misIslas[0].x = 1100; misIslas[0].y = 1100;
    misIslas[0].ancho = 1000; misIslas[0].alto = 1000;

    // ISLA NORTE
    misIslas[1].activa = 1; misIslas[1].x = 1250; misIslas[1].y = 200;
    misIslas[1].ancho = 700; misIslas[1].alto = 700;

    // ISLA SUR
    misIslas[2].activa = 1; misIslas[2].x = 1350; misIslas[2].y = 2300;
    misIslas[2].ancho = 500; misIslas[2].alto = 500;

    // ISLA OESTE
    misIslas[3].activa = 1; misIslas[3].x = 580; misIslas[3].y = 1475;
    misIslas[3].ancho = 320; misIslas[3].alto = 250;

    // ISLA ESTE
    misIslas[4].activa = 1; misIslas[4].x = 2300; misIslas[4].y = 1400;
    misIslas[4].ancho = 400; misIslas[4].alto = 400;
}

// --- 2. SISTEMA DE FÍSICA Y COLISIONES (ESCÁNER) ---

// Función Auxiliar: Obtiene el color visual real en una coordenada (X,Y)
COLORREF ObtenerColorDePunto(int x, int y, int mapaId) {
    for (int i = 0; i < MAX_ISLAS; i++) {
        if (!misIslas[i].activa) continue;

        if (x >= misIslas[i].x && x < misIslas[i].x + misIslas[i].ancho &&
            y >= misIslas[i].y && y < misIslas[i].y + misIslas[i].alto) {
            
            int localX = x - misIslas[i].x;
            int localY = y - misIslas[i].y;

            HBITMAP hBmp = obtenerImagenIsla(i, mapaId);
            if (!hBmp) continue;

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
void generarColisionDeMapaCompleto(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int mapaId) {
    // 1. Limpiamos a AGUA (0)
    for (int y = 0; y < MUNDO_FILAS; y++) {
        for (int x = 0; x < MUNDO_COLUMNAS; x++) {
            mapa[y][x] = 0;
        }
    }

    // 2. Escaneamos punto por punto (16x16)
    for (int fila = 0; fila < MUNDO_FILAS; fila++) {
        for (int col = 0; col < MUNDO_COLUMNAS; col++) {
            int mundoX = (col * TAMANO_CELDA_BASE) + (TAMANO_CELDA_BASE / 2);
            int mundoY = (fila * TAMANO_CELDA_BASE) + (TAMANO_CELDA_BASE / 2);

            COLORREF c = ObtenerColorDePunto(mundoX, mundoY, mapaId);

            // Si NO es Magenta (Transparente) y NO es Azul -> TIERRA
            if (c != RGB(255, 0, 255) && c != RGB(0, 100, 180)) {
                mapa[fila][col] = 1; 
            }
        }
    }
}

void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int mapaId) { 
    inicializarIslas(mapaId); 
    generarColisionDeMapaCompleto(mapa, mapaId);
}

int EsSuelo(int x, int y, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS])
{
    int col = x / TAMANO_CELDA_BASE;
    int fila = y / TAMANO_CELDA_BASE;
    if (fila < 0 || fila >= MUNDO_FILAS || col < 0 || col >= MUNDO_COLUMNAS) return 0;
    return (mapa[fila][col] == 1);
}

void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy) {
    
    // Animación
    if (dy > 0) jugador->direccion = DIR_ABAJO;
    else if (dy < 0) jugador->direccion = DIR_ARRIBA;
    else if (dx < 0) jugador->direccion = DIR_IZQUIERDA;
    else if (dx > 0) jugador->direccion = DIR_DERECHA;

    if (dx != 0 || dy != 0) {
        jugador->pasoAnimacion++;
        int estado = (jugador->pasoAnimacion / 4) % 4;
        jugador->frameAnim = (estado == 0) ? 1 : ((estado == 2) ? 2 : 0);
    } else {
        jugador->frameAnim = 1;
    }

    // --- FÍSICA X ---
    int futuroX = jugador->x + (dx * jugador->velocidad);
    
    // Punto de colisión: Pies (+8 para perspectiva Top-Down correcta)
    int piesY = jugador->y + 8; 
    int piesX = futuroX + 16; 
    
    int col = piesX / TAMANO_CELDA_BASE; 
    int fila = piesY / TAMANO_CELDA_BASE;

    if (col >= 0 && col < MUNDO_COLUMNAS && fila >= 0 && fila < MUNDO_FILAS) {
        // Lógica Inversa: Si estamos en barco (estado > 0), necesitamos AGUA (0)
        // Si estamos a pie (estado == 0), necesitamos TIERRA (1)
        int terreno = mapa[fila][col];
        if ((jugador->estadoBarco == 0 && terreno == 1) || 
            (jugador->estadoBarco > 0 && terreno == 0)) {
            jugador->x = futuroX; 
        }
    }

    // --- FÍSICA Y ---
    int futuroY = jugador->y + (dy * jugador->velocidad);
    piesX = jugador->x + 16; 
    piesY = futuroY + 8; 
    
    col = piesX / TAMANO_CELDA_BASE;
    fila = piesY / TAMANO_CELDA_BASE;

    if (col >= 0 && col < MUNDO_COLUMNAS && fila >= 0 && fila < MUNDO_FILAS) {
        int terreno = mapa[fila][col];
        if ((jugador->estadoBarco == 0 && terreno == 1) || 
            (jugador->estadoBarco > 0 && terreno == 0)) {
            jugador->y = futuroY; 
        }
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
    if (camara->x < 0) camara->x = 0;
    if (camara->y < 0) camara->y = 0;
}

// --- 3. TEXTOS Y EFECTOS ---

void crearTextoFlotante(int x, int y, const char* contenido, int cantidad, COLORREF color) {
    for (int i = 0; i < MAX_TEXTOS; i++) {
        if (!listaTextos[i].activo) {
            listaTextos[i].x = (float)x;
            listaTextos[i].y = (float)y;
            if (cantidad > 0) sprintf(listaTextos[i].texto, "+%d %s", cantidad, contenido);
            else strcpy(listaTextos[i].texto, contenido);
            listaTextos[i].vida = 60; 
            listaTextos[i].color = color;
            listaTextos[i].activo = 1;
            break;
        }
    }
}

void actualizarYDibujarTextos(HDC hdc, Camera cam) {
    SetBkMode(hdc, TRANSPARENT);
    for (int i = 0; i < MAX_TEXTOS; i++) {
        if (listaTextos[i].activo) {
            listaTextos[i].y -= 0.8f;
            listaTextos[i].vida--;
            if (listaTextos[i].vida <= 0) listaTextos[i].activo = 0;
            else {
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

void crearChispas(int x, int y, COLORREF color) {
    int creadas = 0;
    for (int i = 0; i < MAX_PARTICULAS && creadas < 8; i++) {
        if (!chispas[i].activo) {
            chispas[i].x = x; chispas[i].y = y;
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

void actualizarYDibujarParticulas(HDC hdc, Camera cam) {
    for (int i = 0; i < MAX_PARTICULAS; i++) {
        if (chispas[i].activo) {
            chispas[i].x += chispas[i].vx;
            chispas[i].y += chispas[i].vy;
            chispas[i].vy += 0.4f; 
            chispas[i].vida--;
            if (chispas[i].vida <= 0) chispas[i].activo = 0;
            else {
                int sx = (int)((chispas[i].x - cam.x) * cam.zoom);
                int sy = (int)((chispas[i].y - cam.y) * cam.zoom);
                HBRUSH hBr = CreateSolidBrush(chispas[i].color);
                RECT r = { sx, sy, sx + 4, sy + 4 };
                FillRect(hdc, &r, hBr);
                DeleteObject(hBr);
            }
        }
    }
}

// --- 4. ENTIDADES DEL JUEGO (VACAS, ARBOLES, MINAS, TESOROS) ---

// --- VACAS ---
void inicializarVacas() {
    for (int i = 0; i < MAX_VACAS; i++) manada[i].activa = 0;
    for (int i = 0; i < 7; i++) {
        manada[i].activa = 1;
        manada[i].x = ESTABLO_X + (rand() % 150);
        manada[i].y = ESTABLO_Y + ((i / 2) * 40) + (rand() % 30);
        manada[i].xInicial = manada[i].x; 
        manada[i].direccion = (rand() % 2 == 0) ? 1 : -1;
        manada[i].vida = 5;
        manada[i].estadoVida = 0; 
        manada[i].frameAnim = (manada[i].direccion == 1) ? 4 : 0;
    }
}

void actualizarVacas(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    for (int i = 0; i < MAX_VACAS; i++) {
        if (!manada[i].activa) continue;
        if (manada[i].estadoVida == 1) { // Muerta
            manada[i].tiempoMuerte--;
            if (manada[i].tiempoMuerte <= 0) {
                manada[i].activa = 0; // Desaparece el cadáver, comienza el Respawn
                manada[i].timerRegeneracion = 0; // Reset por seguridad
            }
            continue;
        }
        // Patrulla
        if (manada[i].direccion == 1) {
            manada[i].x++;
            if (manada[i].x >= manada[i].xInicial + 30) { manada[i].direccion = -1; manada[i].frameAnim = 0; }
        } else {
            manada[i].x--;
            if (manada[i].x <= manada[i].xInicial) { manada[i].direccion = 1; manada[i].frameAnim = 4; }
        }
        // Animación
        manada[i].contadorAnim++;
        if (manada[i].contadorAnim > 8) {
            manada[i].frameAnim++;
            if (manada[i].direccion == 1 && manada[i].frameAnim > 7) manada[i].frameAnim = 4;
            if (manada[i].direccion == -1 && manada[i].frameAnim > 3) manada[i].frameAnim = 0;
            manada[i].contadorAnim = 0;
        }
    }
}

void golpearVaca(Jugador *j) {
    int rango = 50;
    for (int i = 0; i < MAX_VACAS; i++) {
        if (!manada[i].activa || manada[i].estadoVida == 1) continue;
        if (abs((j->x + 16) - (manada[i].x + 16)) < rango && abs((j->y + 16) - (manada[i].y + 16)) < rango) {
            manada[i].vida--;
            crearChispas(manada[i].x+16, manada[i].y+16, RGB(200,0,0));
            if (manada[i].vida <= 0) {
                manada[i].estadoVida = 1;
                manada[i].tiempoMuerte = 300;
                j->comida += 10;
                crearTextoFlotante(manada[i].x, manada[i].y, "Carne", 10, RGB(255, 100, 100));
            }
            return;
        }
    }
}

void dibujarVacas(HDC hdc, Camera cam, int ancho, int alto) {
    for (int i = 0; i < MAX_VACAS; i++) {
        if (!manada[i].activa) continue;
        int sx = (manada[i].x - cam.x) * cam.zoom;
        int sy = (manada[i].y - cam.y) * cam.zoom;
        int tam = 32 * cam.zoom;
        if (sx > -tam && sx < ancho && sy > -tam && sy < alto) {
            if (manada[i].estadoVida == 1) { if(hBmpVacaMuerta) DibujarImagen(hdc, hBmpVacaMuerta, sx, sy, tam, tam); }
            else { if(hBmpVaca[manada[i].frameAnim]) DibujarImagen(hdc, hBmpVaca[manada[i].frameAnim], sx, sy, tam, tam); }
        }
    }
}

void dibujarEstablo(HDC hdc, Camera cam) {
    int sx = (int)((ESTABLO_X - cam.x) * cam.zoom);
    int sy = (int)((ESTABLO_Y - cam.y) * cam.zoom);
    if (hBmpEstablo) DibujarImagen(hdc, hBmpEstablo, sx, sy, 200 * cam.zoom, 200 * cam.zoom);
}

// --- ÁRBOLES ---
void inicializarArboles(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    int cnt = 0, intentos = 0;
    int minX = 1200, maxX = 2000, minY = 1200, maxY = 2000;
    for(int i=0; i<MAX_ARBOLES; i++) misArboles[i].activa = 0;

    while (cnt < MAX_ARBOLES && intentos < 50000) {
        intentos++;
        int rx = minX + (rand() % (maxX - minX));
        int ry = minY + (rand() % (maxY - minY));
        int tipo = rand() % 2; 
        
        // Validar suelo y distancia
        if (!EsSuelo(rx+16, ry+30, mapa)) continue;
        int cerca = 0;
        for(int i=0; i<cnt; i++) {
            if (sqrt(pow(rx - misArboles[i].x, 2) + pow(ry - misArboles[i].y, 2)) < 150) { cerca = 1; break; }
        }
        if (cerca) continue;

        misArboles[cnt].x = rx; misArboles[cnt].y = ry; misArboles[cnt].tipo = tipo;
        misArboles[cnt].activa = 1; misArboles[cnt].vida = 5;
        cnt++;
    }
}

void talarArbol(Jugador *j) {
    // Requiere Hacha nivel 2 si quieres hacerlo estricto, por ahora libre
    int rango = 40;
    for (int i = 0; i < MAX_ARBOLES; i++) {
        if (!misArboles[i].activa) continue;
        int tam = (misArboles[i].tipo == 1) ? 64 : 32;
        if (abs((j->x+16) - (misArboles[i].x + tam/2)) < rango && abs((j->y+16) - (misArboles[i].y + tam/2)) < rango) {
            misArboles[i].vida--;
            crearChispas(misArboles[i].x + tam/2, misArboles[i].y + tam/2, RGB(139, 69, 19));
            if (misArboles[i].vida <= 0) {
                misArboles[i].activa = 0;
                int mad = (misArboles[i].tipo == 1) ? 5 : 3;
                int hoj = (misArboles[i].tipo == 1) ? 10 : 4;
                agregarRecurso(&j->madera, mad, j->nivelMochila); // Usamos la func nueva
                agregarRecurso(&j->hojas, hoj, j->nivelMochila);
                crearTextoFlotante(misArboles[i].x, misArboles[i].y, "Madera", mad, RGB(150, 75, 0));
                crearTextoFlotante(misArboles[i].x, misArboles[i].y - 20, "Hojas", hoj, RGB(34, 139, 34));
            }
            return;
        }
    }
}

void dibujarArboles(HDC hdc, Camera cam, int ancho, int alto, int mapaId) {
    for (int i = 0; i < MAX_ARBOLES; i++) {
        if (!misArboles[i].activa) continue;
        int sx = (misArboles[i].x - cam.x) * cam.zoom;
        int sy = (misArboles[i].y - cam.y) * cam.zoom;
        int tam = (misArboles[i].tipo == 1 ? 64 : 32) * cam.zoom;
        
        HBITMAP img = NULL;
        if(mapaId == 0) img = (misArboles[i].tipo == 1) ? hBmpArbolGrande : hBmpArbolChico;
        else if(mapaId == 1) img = (misArboles[i].tipo == 1) ? hBmpArbolGrandeMapa2 : hBmpArbolChicoMapa2;
        else img = (misArboles[i].tipo == 1) ? hBmpArbolGrandeMapa3 : hBmpArbolChicoMapa3;

        if (img && sx > -tam && sx < ancho && sy > -tam && sy < alto) 
            DibujarImagen(hdc, img, sx, sy, tam, tam);
    }
}

// --- MINAS ---
void inicializarMinas(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    int cnt = 0, intentos = 0;
    int minX = 1200, maxX = 2000, minY = 1200, maxY = 2000;
    for(int i=0; i<MAX_MINAS; i++) misMinas[i].activa = 0;

    while (cnt < MAX_MINAS && intentos < 10000) {
        intentos++;
        int rx = minX + (rand() % (maxX - minX));
        int ry = minY + (rand() % (maxY - minY));
        if (!EsSuelo(rx+16, ry+16, mapa)) continue;
        
        int cerca = 0;
        for(int i=0; i<cnt; i++) if (sqrt(pow(rx - misMinas[i].x, 2) + pow(ry - misMinas[i].y, 2)) < 100) cerca = 1;
        if (cerca) continue;

        misMinas[cnt].x = rx; misMinas[cnt].y = ry; 
        misMinas[cnt].tipo = rand() % 2; 
        misMinas[cnt].vida = 5; misMinas[cnt].activa = 1;
        cnt++;
    }
}

void picarMina(Jugador *j) {
    for (int i = 0; i < MAX_MINAS; i++) {
        if (!misMinas[i].activa) continue;
        if (abs((j->x+16) - (misMinas[i].x+16)) < 40 && abs((j->y+16) - (misMinas[i].y+16)) < 40) {
            misMinas[i].vida--;
            crearChispas(misMinas[i].x+16, misMinas[i].y+16, RGB(200,200,200));
            if (misMinas[i].vida <= 0) {
                misMinas[i].activa = 0;
                if (misMinas[i].tipo == 0) { agregarRecurso(&j->piedra, 5, j->nivelMochila); crearTextoFlotante(misMinas[i].x, misMinas[i].y, "Piedra", 5, RGB(180,180,180)); }
                else { agregarRecurso(&j->hierro, 3, j->nivelMochila); crearTextoFlotante(misMinas[i].x, misMinas[i].y, "Hierro", 3, RGB(255,215,0)); }
            }
            return;
        }
    }
}

void dibujarMinas(HDC hdc, Camera cam, int ancho, int alto) {
    for (int i = 0; i < MAX_MINAS; i++) {
        if (!misMinas[i].activa) continue;
        int sx = (misMinas[i].x - cam.x) * cam.zoom;
        int sy = (misMinas[i].y - cam.y) * cam.zoom;
        HBITMAP img = (misMinas[i].tipo == 1) ? hBmpHierroPicar : hBmpPiedraPicar;
        if (img) DibujarImagen(hdc, img, sx, sy, 32 * cam.zoom, 32 * cam.zoom);
    }
}

// --- FUNCIONES AUXILIARES DE BÚSQUEDA (IA) ---

int buscarArbolCercano(float x, float y, float rango) {
    int mejorIndice = -1;
    float mejorDist = rango;

    for (int i = 0; i < MAX_ARBOLES; i++) {
        if (!misArboles[i].activa) continue;

        // Calculamos distancia al centro del árbol (+16)
        float dx = (misArboles[i].x + 16) - x;
        float dy = (misArboles[i].y + 16) - y;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist < mejorDist) {
            mejorDist = dist;
            mejorIndice = i;
        }
    }
    return mejorIndice;
}

int buscarMinaCercana(float x, float y, float rango) {
    int mejorIndice = -1;
    float mejorDist = rango;

    for (int i = 0; i < MAX_MINAS; i++) {
        if (!misMinas[i].activa) continue;

        float dx = (misMinas[i].x + 16) - x;
        float dy = (misMinas[i].y + 16) - y;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist < mejorDist) {
            mejorDist = dist;
            mejorIndice = i;
        }
    }
    return mejorIndice;
}

int buscarVacaCercana(float x, float y, float rango) {
    int mejorIndice = -1;
    float mejorDist = rango;

    for (int i = 0; i < MAX_VACAS; i++) {
        // Solo buscamos vacas activas y VIVAS (estadoVida 0)
        if (!manada[i].activa || manada[i].estadoVida != 0) continue;

        float dx = (manada[i].x + 16) - x;
        float dy = (manada[i].y + 16) - y;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist < mejorDist) {
            mejorDist = dist;
            mejorIndice = i;
        }
    }
    return mejorIndice;
}

// --- TESOROS ---
void inicializarTesoros() {
    misTesoros[0].x = 1320; misTesoros[0].y = 1250; misTesoros[0].tipo = 0; misTesoros[0].estado = 0; misTesoros[0].activa = 1;
    misTesoros[1].x = 1850; misTesoros[1].y = 1550; misTesoros[1].tipo = 1; misTesoros[1].estado = 0; misTesoros[1].activa = 1;
}

void abrirTesoro(Jugador *j) {
    for (int i = 0; i < MAX_TESOROS; i++) {
        if (!misTesoros[i].activa || misTesoros[i].estado == 2) continue;

        // Comprobación de rango entre jugador y tesoro
        if (abs((j->x + 16) - (misTesoros[i].x + 16)) < 60 && 
            abs((j->y + 16) - (misTesoros[i].y + 16)) < 60) {
            
            // Primer click: Cambia de cerrado (0) a entreabierto/mostrando contenido (1)
            if (misTesoros[i].estado == 0) { 
                misTesoros[i].estado = 1; 
                return; 
            }

            // Segundo click: Recolectar botín (estado 1 -> 2)
            if (misTesoros[i].estado == 1) {
                // 1. RECURSO SIEMPRE PRESENTE: ORO
                int oro = 30 + (rand() % 11); // 30 a 40 de oro
                agregarRecurso(&j->oro, oro, 3); // Capacidad alta para oro
                crearTextoFlotante(misTesoros[i].x, misTesoros[i].y, "Oro", oro, RGB(255, 215, 0));

                // 2. RECURSO EXTRA: HIERRO (Solo si el tipo es 1 - Joyas)
                if (misTesoros[i].tipo == 1) {
                    int hierro = 20 + (rand() % 10); // 5 a 10 de hierro
                    agregarRecurso(&j->hierro, hierro, j->nivelMochila);
                    
                    // Mostramos el texto de hierro un poco más arriba para que no se solape
                    crearTextoFlotante(misTesoros[i].x, misTesoros[i].y - 25, "Hierro", hierro, RGB(192, 192, 192));
                }

                // Marcar como recolectado (vacío)
                misTesoros[i].estado = 2; 
                return;
            }
        }
    }
}
void dibujarTesoros(HDC hdc, Camera cam, int ancho, int alto) {
    for (int i = 0; i < MAX_TESOROS; i++) {
        if (!misTesoros[i].activa) continue;
        int sx = (misTesoros[i].x - cam.x) * cam.zoom;
        int sy = (misTesoros[i].y - cam.y) * cam.zoom;
        HBITMAP img = hBmpTesoroVacio;
        if (misTesoros[i].estado == 0) img = hBmpTesoroCerrado;
        else if (misTesoros[i].estado == 1) img = (misTesoros[i].tipo == 0) ? hBmpTesoroOro : hBmpTesoroJoyas;
        
        if (img) DibujarImagen(hdc, img, sx, sy, 32 * cam.zoom, 32 * cam.zoom);
    }
}

// --- 5. LÓGICA DE JUEGO GENERAL ---

void actualizarLogicaSistema(Jugador *j) {
    // Pesca
    if (j->estadoBarco == 1) { 
        j->timerPesca++;
        if (j->timerPesca >= 600) { // 10 seg
            j->timerPesca = 0;
            agregarRecurso(&j->pescado, 3, j->nivelMochila);
            crearTextoFlotante(j->x, j->y, "Pescado", 3, RGB(100, 200, 255));
        }
    } else j->timerPesca = 0;
}

void actualizarRegeneracionRecursos() {
    // 1. ÁRBOLES (Ya funcionaba, lo dejamos igual)
    for (int i = 0; i < MAX_ARBOLES; i++) {
        if (!misArboles[i].activa) {
            misArboles[i].timerRegeneracion++;
            if (misArboles[i].timerRegeneracion >= TIEMPO_RESPAWN_RECURSOS) {
                misArboles[i].activa = 1;
                misArboles[i].vida = 5;
                misArboles[i].timerRegeneracion = 0;
                crearChispaBlanca(misArboles[i].x, misArboles[i].y);
            }
        }
    }

    // 2. MINAS (NUEVO)
    for (int i = 0; i < MAX_MINAS; i++) {
        if (!misMinas[i].activa) {
            misMinas[i].timerRegeneracion++;
            if (misMinas[i].timerRegeneracion >= TIEMPO_RESPAWN_RECURSOS) {
                misMinas[i].activa = 1;
                misMinas[i].vida = 8; // Restaurar vida completa (8 golpes)
                misMinas[i].timerRegeneracion = 0;
                crearChispaBlanca(misMinas[i].x, misMinas[i].y); // Efecto visual
            }
        }
    }

    // 3. VACAS (NUEVO)
    for (int i = 0; i < MAX_VACAS; i++) {
        if (!manada[i].activa) {
            manada[i].timerRegeneracion++;
            if (manada[i].timerRegeneracion >= TIEMPO_RESPAWN_RECURSOS) {
                manada[i].activa = 1;
                manada[i].estadoVida = 0; // Vuelve a estar VIVA
                manada[i].vida = 5;       // Vida completa
                manada[i].tiempoMuerte = 300; // Resetear timer de animación muerte
                manada[i].timerRegeneracion = 0;
                manada[i].x = manada[i].xInicial; 
                crearChispaBlanca(manada[i].x, manada[i].y);
            }
        }
    }
}
void intentarMontarBarco(Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    // Si estamos en barco, bajar
    if (j->estadoBarco > 0) {
        if (EsSuelo(j->x + 16, j->y + 32, mapa)) {
            j->estadoBarco = 0;
            crearTextoFlotante(j->x, j->y, "A tierra!", 0, RGB(255, 255, 255));
        }
        return;
    }
    // Si estamos a pie, subir
    if (j->tieneBotePesca && j->tieneCana) { // Requiere ambos
        j->estadoBarco = 1;
        j->x += 20; // Empujón al agua
        crearTextoFlotante(j->x, j->y, "A pescar!", 0, RGB(100, 200, 255));
    }
}

// --- 6. UNIDADES RTS (MINEROS, LEÑADORES, ETC) ---

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
        case TIPO_LENADOR: nombre = "Lenadores"; break;
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

void actualizarUnidades(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Jugador *j) {
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa) continue;

        // Calcular distancia al objetivo de movimiento (para ESTADO_MOVIENDO)
        float dx = unidades[i].destinoX - unidades[i].x;
        float dy = unidades[i].destinoY - unidades[i].y;
        float dist = sqrt(dx * dx + dy * dy);

        switch (unidades[i].estado) {

            case ESTADO_TALANDO: {
                int a = unidades[i].targetIndex;
                if (a == -1 || !misArboles[a].activa) { 
                int nuevo = buscarArbolCercano(unidades[i].x, unidades[i].y, 150.0f);
                if (nuevo != -1) {
                    unidades[i].targetIndex = nuevo;
                    unidades[i].destinoX = misArboles[nuevo].x;
                    unidades[i].destinoY = misArboles[nuevo].y;
            } else {
                unidades[i].estado = ESTADO_IDLE;
            }
            break; 
            }
    
            float targetX = misArboles[a].x + 16;
            float targetY = misArboles[a].y + 16;
            float dx = targetX - unidades[i].x;
            float dy = targetY - unidades[i].y;
            float distObjeto = sqrt(dx*dx + dy*dy);

            if (distObjeto > 50.0f) { 
             // Moverse hacia el árbol
             unidades[i].x += (dx / distObjeto) * 2.0f;
             unidades[i].y += (dy / distObjeto) * 2.0f;
            actualizarAnimacionUnidad(&unidades[i], dx, dy);
        } else {
             // Talar el árbol
            unidades[i].timerTrabajo++;
            if (unidades[i].timerTrabajo >= 250) { 
                unidades[i].timerTrabajo = 0;
            
            // AQUÍ ESTÁ LA CLAVE: REDUCIR LA VIDA DEL ÁRBOL
            misArboles[a].vida--;  // ¡IMPORTANTE! Reducir vida
            
            // Mostrar efecto de golpe
            crearChispas(targetX, targetY, RGB(139, 69, 19));
            
            if (misArboles[a].vida <= 0) {
                // Dar recompensa
                j->madera += 10;
                crearTextoFlotante(unidades[i].x, unidades[i].y, "Madera", 10, RGB(139, 69, 19));
                
                // Desactivar árbol
                misArboles[a].activa = 0;
                misArboles[a].timerRegeneracion = 0;
                
                // Actualizar mapa de colisiones si es necesario
                int col = (int)(misArboles[a].x / TAMANO_CELDA_BASE);
                int fil = (int)(misArboles[a].y / TAMANO_CELDA_BASE);
                if(fil >= 0 && fil < MUNDO_FILAS && col >= 0 && col < MUNDO_COLUMNAS)
                    mapa[fil][col] = 1; 
                
                unidades[i].targetIndex = -1;
                
                // Buscar otro árbol inmediatamente
                int next = buscarArbolCercano(unidades[i].x, unidades[i].y, 200.0f);
                if(next != -1) {
                    unidades[i].targetIndex = next;
                } else {
                    unidades[i].estado = ESTADO_IDLE;
                }
            }
        }
    }
    break;
}

            case ESTADO_MINANDO: {
    int m = unidades[i].targetIndex;
    if (m == -1 || !misMinas[m].activa) { 
        int nueva = buscarMinaCercana(unidades[i].x, unidades[i].y, 150.0f);
        if (nueva != -1) unidades[i].targetIndex = nueva;
        else unidades[i].estado = ESTADO_IDLE;
        break; 
    }
    
    float targetX = misMinas[m].x + 16;
    float targetY = misMinas[m].y + 16;
    float dx = targetX - unidades[i].x;
    float dy = targetY - unidades[i].y;
    float distMina = sqrt(dx*dx + dy*dy);

    if (distMina > 50.0f) { 
        // Moverse hacia la mina
        unidades[i].x += (dx / distMina) * 2.0f;
        unidades[i].y += (dy / distMina) * 2.0f;
        actualizarAnimacionUnidad(&unidades[i], dx, dy);
    } else {
        // Picar la mina
        unidades[i].timerTrabajo++;
        if (unidades[i].timerTrabajo >= 400) { 
            unidades[i].timerTrabajo = 0;
            
            // AQUÍ ESTÁ LA CLAVE: REDUCIR LA VIDA DE LA MINA
            misMinas[m].vida--;  // ¡IMPORTANTE! Reducir vida
            
            // Mostrar efecto de golpe
            crearChispas(targetX, targetY, RGB(200, 200, 200));
            
            // Dar recompensa según tipo
            if (misMinas[m].tipo == 0) {  // Piedra
                j->piedra += 10;
                crearTextoFlotante(unidades[i].x, unidades[i].y, "Piedra", 10, RGB(150, 150, 150));
            } else {  // Hierro
                j->hierro += 5;
                crearTextoFlotante(unidades[i].x, unidades[i].y, "Hierro", 5, RGB(192, 192, 192));
            }
            
            if (misMinas[m].vida <= 0) {
                // Desactivar mina
                misMinas[m].activa = 0;
                
                // Actualizar mapa de colisiones
                int col = (int)(misMinas[m].x / TAMANO_CELDA_BASE);
                int fil = (int)(misMinas[m].y / TAMANO_CELDA_BASE);
                if(fil >= 0 && fil < MUNDO_FILAS && col >= 0 && col < MUNDO_COLUMNAS)
                    mapa[fil][col] = 1; 
                
                unidades[i].targetIndex = -1;
                
                // Buscar otra mina inmediatamente
                int next = buscarMinaCercana(unidades[i].x, unidades[i].y, 200.0f);
                if(next != -1) {
                    unidades[i].targetIndex = next;
                } else {
                    unidades[i].estado = ESTADO_IDLE;
                }
            }
        }
    }
    break;
}

            // --- CORRECCIÓN IMPORTANTE: LÓGICA DEL CAZADOR ---
            case ESTADO_CAZANDO: {
                int v = unidades[i].targetIndex;

                // 1. Si el objetivo no existe o ya murió, BUSCAR OTRO INMEDIATAMENTE
                if (v == -1 || !manada[v].activa || manada[v].estadoVida != 0) {
                    // Rango de búsqueda automática (500px)
                    int proxima = buscarVacaCercana(unidades[i].x, unidades[i].y, 500.0f);
                    
                    if (proxima != -1) {
                        unidades[i].targetIndex = proxima;
                        // No cambiamos de estado, seguimos en CAZANDO pero con nuevo target
                    } else {
                        unidades[i].estado = ESTADO_IDLE; // Solo descansa si no hay nada cerca
                    }
                    break; // Salimos del switch para procesar el nuevo target en el siguiente frame
                }

                // 2. Perseguir a la vaca
                float dvx = manada[v].x - unidades[i].x;
                float dvy = manada[v].y - unidades[i].y;
                float distVaca = sqrt(dvx * dvx + dvy * dvy);

                if (distVaca > 35.0f) {
                    unidades[i].x += (dvx / distVaca) * 2.2f; // Velocidad de persecución
                    unidades[i].y += (dvy / distVaca) * 2.2f;
                    actualizarAnimacionUnidad(&unidades[i], dvx, dvy);
                } else {
                    // 3. Atacar/Trabajar
                    unidades[i].timerTrabajo++;
                    if (unidades[i].timerTrabajo >= 300) { 
                        
                        // Matar vaca
                        manada[v].estadoVida = 1; 
                        manada[v].tiempoMuerte = 300; // Timer para desaparecer
                        
                        // Recompensa
                        j->comida += 15;
                        crearTextoFlotante(manada[v].x, manada[v].y, "+15 Comida", 1, RGB(255, 200, 0));
                        crearChispas(manada[v].x + 16, manada[v].y + 16, RGB(255, 0, 0)); // Sangre/Golpe

                        // Resetear timer
                        unidades[i].timerTrabajo = 0;
                        unidades[i].targetIndex = -1;

                        // --- AQUÍ ESTÁ LA CLAVE: ENCADENAMIENTO ---
                        // En lugar de ir a IDLE, buscamos la siguiente vaca inmediatamente
                        int siguienteVaca = buscarVacaCercana(unidades[i].x, unidades[i].y, 600.0f);
                        if (siguienteVaca != -1) {
                            unidades[i].targetIndex = siguienteVaca;
                            // Se mantiene en ESTADO_CAZANDO
                        } else {
                            unidades[i].estado = ESTADO_IDLE;
                        }
                    }
                }
                break;
            }

            case ESTADO_MOVIENDO:
                if (dist > 5.0f) {
                    unidades[i].x += (dx / dist) * 2.2f;
                    unidades[i].y += (dy / dist) * 2.2f;
                    actualizarAnimacionUnidad(&unidades[i], dx, dy);
                } else {
                    unidades[i].estado = ESTADO_IDLE;
                }
                break;

            case ESTADO_IDLE:
                // --- INTELIGENCIA ARTIFICIAL EN IDLE ---
                // Si es un Cazador y está quieto, que mire si pasa una vaca cerca
                if (unidades[i].tipo == TIPO_CAZADOR) {
                    // Rango de detección automática en reposo (200px)
                    int vacaCerca = buscarVacaCercana(unidades[i].x, unidades[i].y, 200.0f);
                    if (vacaCerca != -1) {
                        unidades[i].estado = ESTADO_CAZANDO;
                        unidades[i].targetIndex = vacaCerca;
                    }
                }
                break;
        }
    }

    // Actualizar textos flotantes
    for (int t = 0; t < MAX_TEXTOS; t++) {
        if (textos[t].activo && textos[t].vida > 0) {
            textos[t].y -= 0.5f; 
            textos[t].vida--;
            if (textos[t].vida <= 0) textos[t].activo = 0;
        }
    }
}
void actualizarAnimacionUnidad(Unidad *u, float dx, float dy) {
    if (fabs(dx) > fabs(dy)) {
        u->direccion = (dx > 0) ? DIR_DERECHA : DIR_IZQUIERDA;
    } else {
        u->direccion = (dy > 0) ? DIR_ABAJO : DIR_ARRIBA;
    }
    u->contadorAnim++;
    if (u->contadorAnim > 10) {
        u->frameAnim = (u->frameAnim + 1) % 3;
        u->contadorAnim = 0;
    }
}
// 4. DIBUJAR (Nombres y Herramientas)
void dibujarUnidades(HDC hdc, Camera cam) {
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa) continue;

        // 1. CALCULAR POSICIÓN EN PANTALLA
        // Convertimos las coordenadas del mundo a coordenadas de píxeles en ventana
        int ux = (int)((unidades[i].x - cam.x) * cam.zoom);
        int uy = (int)((unidades[i].y - cam.y) * cam.zoom);
        int tam = (int)(TAMANO_CELDA * cam.zoom);

        // 2. SELECCIONAR EL BITMAP CORRECTO (Animación)
        HBITMAP hBmpActual = NULL;
        switch (unidades[i].tipo) {
            case TIPO_MINERO:  hBmpActual = hBmpMineroAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
            case TIPO_LENADOR: hBmpActual = hBmpLenadorAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
            case TIPO_CAZADOR: hBmpActual = hBmpCazadorAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
            case TIPO_SOLDADO: hBmpActual = hBmpSoldadoAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
            default:           hBmpActual = hBmpJugadorAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
        }

        // 3. DIBUJAR EL SPRITE DE LA UNIDAD
        DibujarImagen(hdc, hBmpActual, ux, uy, tam, tam);

        // 4. DIBUJAR CÍRCULO DE SELECCIÓN (Si está seleccionada)
        if (unidades[i].seleccionado) {
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
            HGDIOBJ hOld = SelectObject(hdc, hPen);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            // Dibujamos una elipse a los pies de la unidad
            Ellipse(hdc, ux, uy + tam - (int)(10 * cam.zoom), ux + tam, uy + tam);
            SelectObject(hdc, hOld);
            DeleteObject(hPen);
        }

        // 5. DIBUJAR BARRA DE PROGRESO DE TRABAJO
        if (unidades[i].estado == ESTADO_TALANDO || 
            unidades[i].estado == ESTADO_MINANDO || 
            unidades[i].estado == ESTADO_CAZANDO) {

            // Determinar el límite según la lógica de actualizarUnidades
            int limite = 0;
            if (unidades[i].estado == ESTADO_TALANDO) limite = 250;
            else if (unidades[i].estado == ESTADO_MINANDO) limite = 400;
            else if (unidades[i].estado == ESTADO_CAZANDO) limite = 300;

            if (limite > 0) {
                int barraAncho = (int)(20 * cam.zoom);
                int barraAlto = (int)(3 * cam.zoom);
                int bx = ux + (tam / 2) - (barraAncho / 2);
                int by = uy - (int)(8 * cam.zoom); // Situada arriba de la cabeza

                // Dibujar Fondo de la barra (Gris oscuro)
                HBRUSH hBrushBg = CreateSolidBrush(RGB(50, 50, 50));
                RECT rectBg = { bx, by, bx + barraAncho, by + barraAlto };
                FillRect(hdc, &rectBg, hBrushBg);
                DeleteObject(hBrushBg);

                // Dibujar Progreso (Verde brillante)
                float progreso = (float)unidades[i].timerTrabajo / limite;
                int anchoActual = (int)(barraAncho * progreso);
                
                if (anchoActual > 0) {
                    HBRUSH hBrushFill = CreateSolidBrush(RGB(0, 255, 100));
                    RECT rectFill = { bx, by, bx + anchoActual, by + barraAlto };
                    FillRect(hdc, &rectFill, hBrushFill);
                    DeleteObject(hBrushFill);
                }
            }
        }
    }
}

// 5. CONTROL (Igual que antes pero actualizado)
void seleccionarUnidadesGrupo(int x1, int y1, int x2, int y2, Camera cam) {
    // 1. Normalizar el cuadro (por si arrastran de derecha a izquierda o arriba)
    int minX = min(x1, x2);
    int maxX = max(x1, x2);
    int minY = min(y1, y2);
    int maxY = max(y1, y2);

    // 2. Si el cuadro es minúsculo (un solo clic), podemos tratarlo como selección individual
    bool esClickSimple = (maxX - minX < 5 && maxY - minY < 5);

    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa) continue;

        // Convertir posición del mundo a pantalla
        int ux = (int)((unidades[i].x - cam.x) * cam.zoom);
        int uy = (int)((unidades[i].y - cam.y) * cam.zoom);
        int size = (int)(32 * cam.zoom);

        if (esClickSimple) {
            // Lógica de clic único: ¿El mouse está sobre el sprite?
            if (x1 >= ux && x1 <= ux + size && y1 >= uy && y1 <= uy + size) {
                unidades[i].seleccionado = 1;
            } else {
                unidades[i].seleccionado = 0; // Deseleccionar si clickeas el suelo
            }
        } else {
            // Lógica de cuadro: ¿La unidad está dentro del rectángulo?
            if (ux + (size/2) >= minX && ux + (size/2) <= maxX && 
                uy + (size/2) >= minY && uy + (size/2) <= maxY) {
                unidades[i].seleccionado = 1;
            } else {
                // Si no estamos presionando SHIFT (opcional), deseleccionamos los de fuera
                unidades[i].seleccionado = 0;
            }
        }
    }
}
void darOrdenMovimiento(Unidad unidades[], int max, int clickX, int clickY) {
    int fila = 0;
    int columna = 0;
    
    for (int i = 0; i < max; i++) {
        if (unidades[i].activa && unidades[i].seleccionado) {
            // Creamos un pequeño desplazamiento (offset) para que no se encimen
            unidades[i].destinoX = clickX + (columna * 35);
            unidades[i].destinoY = clickY + (fila * 20);

            columna++;
            if (columna > 3) { // Máximo 4 unidades por fila en la formación
                columna = 0;
                fila++;
            }
        }
    }
}

// Dar orden de movimiento o acción
// En mapa.c, función ordenarUnidad() - VERSIÓN CORREGIDA
void ordenarUnidad(int mX, int mY, Camera cam) {
    float mundoX = (mX / cam.zoom) + cam.x;
    float mundoY = (mY / cam.zoom) + cam.y;

    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa || !unidades[i].seleccionado) continue;

        unidades[i].timerTrabajo = 0;
        bool objetivoEncontrado = false;

        // --- 1. BUSCAR ÁRBOLES (LEÑADORES) ---
        if (unidades[i].tipo == TIPO_LENADOR) {
            for (int a = 0; a < MAX_ARBOLES; a++) {
                if (!misArboles[a].activa) continue; // CORREGIDO: misArboles

                // CORRECCIÓN: Usamos el CENTRO del árbol
                float centroArbolX = misArboles[a].x + 16; // CORREGIDO: misArboles
                float centroArbolY = misArboles[a].y + 16; // CORREGIDO: misArboles
                
                float d = sqrt(pow(centroArbolX - mundoX, 2) + pow(centroArbolY - mundoY, 2));
                
                // Rango de clic a 60px
                if (d < 60.0f) { 
                    unidades[i].estado = ESTADO_TALANDO;
                    unidades[i].targetIndex = a; 
                    
                    // El destino es el borde del árbol, no el centro exacto
                    unidades[i].destinoX = misArboles[a].x; // CORREGIDO: misArboles
                    unidades[i].destinoY = misArboles[a].y; // CORREGIDO: misArboles
                    
                    objetivoEncontrado = true;
                    break;
                }
            }
        }

        // --- 2. BUSCAR MINAS (MINEROS) ---
        if (!objetivoEncontrado && unidades[i].tipo == TIPO_MINERO) {
            for (int m = 0; m < MAX_MINAS; m++) {
                if (!misMinas[m].activa) continue; // CORREGIDO: misMinas
                
                // CORRECCIÓN: Usamos el CENTRO de la mina
                float centroMinaX = misMinas[m].x + 16; // CORREGIDO: misMinas
                float centroMinaY = misMinas[m].y + 16; // CORREGIDO: misMinas

                float d = sqrt(pow(centroMinaX - mundoX, 2) + pow(centroMinaY - mundoY, 2));
                
                if (d < 60.0f) { // Rango aumentado a 60
                    unidades[i].estado = ESTADO_MINANDO;
                    unidades[i].targetIndex = m;
                    unidades[i].destinoX = misMinas[m].x; // CORREGIDO: misMinas
                    unidades[i].destinoY = misMinas[m].y; // CORREGIDO: misMinas
                    objetivoEncontrado = true;
                    break;
                }
            }
        }

        // --- 3. BUSCAR VACAS (CAZADORES) ---
        if (!objetivoEncontrado && unidades[i].tipo == TIPO_CAZADOR) {
            for (int v = 0; v < MAX_VACAS; v++) {
                if (!manada[v].activa || manada[v].estadoVida != 0) continue;
                
                float centroVacaX = manada[v].x + 16;
                float centroVacaY = manada[v].y + 16;

                float d = sqrt(pow(centroVacaX - mundoX, 2) + pow(centroVacaY - mundoY, 2));
                
                if (d < 60.0f) {
                    unidades[i].estado = ESTADO_CAZANDO;
                    unidades[i].targetIndex = v;
                    objetivoEncontrado = true;
                    break;
                }
            }
        }

        // --- 4. SI NO ES NADA, MOVER ---
        if (!objetivoEncontrado) {
            unidades[i].estado = ESTADO_MOVIENDO;
            unidades[i].destinoX = (int)mundoX;
            unidades[i].destinoY = (int)mundoY;
        }
    }
}




// --- 7. LÓGICA DE TIENDA (RTS y OBJETOS) ---

void procesarClickMochilaTienda(int mx, int my, int esClickDerecho, Jugador *j, HWND hwnd) {
    if (!j->tiendaAbierta) return;

    // 1. COORDENADAS (Deben coincidir con dibujarTiendaInteractiva)
    int tx = 250; 
    int ty = 100; 
    
    // 2. DETECTAR CLIC EN PESTAÑAS (Comprar / Vender)
    // El área de las pestañas está un poco más arriba del cuerpo principal
    if (my >= ty + 10 && my <= ty + 45) { 
        // Pestaña Izquierda: COMPRAR
        if (mx >= tx + 40 && mx <= tx + 140) { 
            j->modoTienda = 0; 
            PlaySound("SystemSelect", NULL, SND_ASYNC);
            InvalidateRect(hwnd, NULL, FALSE); 
            return; 
        }
        // Pestaña Derecha: VENDER
        if (mx >= tx + 180 && mx <= tx + 280) { 
            j->modoTienda = 1; 
            PlaySound("SystemSelect", NULL, SND_ASYNC);
            InvalidateRect(hwnd, NULL, FALSE); 
            return; 
        }
    }

    // Coordenadas de inicio de la rejilla de items
    int startX = tx + 20;
    int startY = ty + 50;

    // --- MODO COMPRAR ---
    if (j->modoTienda == 0) { 
        int cantidad = (esClickDerecho) ? 15 : 5;
        
        for (int i = 0; i < 9; i++) { 
            int col = i % 2; 
            int row = i / 2;
            int ix = startX + (col * 160); // Ajustado al ancho del dibujo
            int iy = startY + (row * 65);

            if (mx >= ix && mx <= ix + 130 && my >= iy && my <= iy + 60) {
                
                // --- SLOT 0-3: UNIDADES (Se mantienen igual) ---
                if (i == 0) { // Cazador
                    if (j->oro >= 20 && j->hierro >= 3) {
                        j->oro -= 20; j->hierro -= 3; 
                        j->cantCazadores++; j->tieneEspada = TRUE;
                        spawnearEscuadron(TIPO_CAZADOR, cantidad, j->x + 50, j->y);
                    }
                }
                else if (i == 1) { // Minero
                    if (j->oro >= 15 && j->piedra >= 5) {
                        j->oro -= 15; j->piedra -= 5; 
                        j->cantMineros++; j->tienePico = TRUE;
                        spawnearEscuadron(TIPO_MINERO, cantidad, j->x + 50, j->y);
                    }
                }
                else if (i == 2) { // Leñador
                    if (j->oro >= 10 && j->madera >= 10) {
                        j->oro -= 10; j->madera -= 10; 
                        j->cantLenadores++; j->tieneHacha = TRUE;
                        spawnearEscuadron(TIPO_LENADOR, cantidad, j->x + 50, j->y);
                    }
                }
                else if (i == 3) { // Soldado
                    if (j->oro >= 40 && j->hierro >= 10) { 
                        j->oro -= 40; j->hierro -= 10; 
                        j->cantSoldados++; j->tieneArmadura = TRUE;
                        spawnearEscuadron(TIPO_SOLDADO, cantidad, j->x + 50, j->y);
                    }
                }
                
                // --- SLOT 4: MOCHILA NIVEL 2 (NUEVA LÓGICA) ---
                else if (i == 4) { 
                     if (j->nivelMochila == 1) {
                         // PRECIO: 300 Oro + 50 Madera
                         if (j->oro >= 100 && j->hojas >= 20) {
                            j->oro -= 100; j->hojas -= 20;
                            j->nivelMochila = 2;
                            crearTextoFlotante(j->x, j->y - 50, "MOCHILA MEJORADA!", 0, RGB(255, 215, 0));
                            MessageBox(hwnd, "Capacidad aumentada!\nAhora puedes construir barcos.", "Nivel Up", MB_OK);
                         } else {
                            crearTextoFlotante(j->x, j->y, "Falta Oro/Madera", 0, RGB(255, 0, 0));
                         }
                     }
                }

                // --- SLOT 5: MOCHILA NIVEL 3 (NUEVA LÓGICA) ---
                else if (i == 5) {
                    if (j->nivelMochila == 2) {
                        // PRECIO: 800 Oro + 100 Hierro
                        if (j->oro >= 200 && j->hierro >= 50) { 
                            j->oro -= 200; j->hierro -= 50;
                            j->nivelMochila = 3;
                            crearTextoFlotante(j->x, j->y - 50, "MOCHILA MAXIMA!", 0, RGB(255, 215, 0));
                        } else {
                             crearTextoFlotante(j->x, j->y, "Falta Oro/Hierro", 0, RGB(255, 0, 0));
                        }
                    }
                }
                
                // --- SLOT 6-8: BARCOS (Se mantienen) ---
                else if (i == 6 && j->nivelMochila >= 2 && !j->tieneCana) { // Caña
                     if (j->oro >= 30 && j->madera >= 10) { j->oro -= 30; j->madera -= 10; j->tieneCana = TRUE; }
                }
                else if (i == 7 && j->nivelMochila >= 2 && !j->tieneBotePesca) { // Bote
                    if (j->oro >= 50 && j->madera >= 30) { j->oro -= 50; j->madera -= 30; j->tieneBotePesca = TRUE; }
                }
                else if (i == 8 && j->nivelMochila >= 2 && !j->tieneBarcoGuerra) { // Galeon
                    if (j->oro >= 100 && j->hierro >= 20 && j->madera >= 50) { 
                        j->oro -= 100; j->hierro -= 20; j->madera -= 50; j->tieneBarcoGuerra = TRUE; 
                    }
                }

                InvalidateRect(hwnd, NULL, FALSE);
                return;
            }
        }
    }
    
    // --- MODO VENDER ---
    else { 
        int precios[] = {1, 2, 5, 1, 3}; 
        int* stocks[] = {&j->madera, &j->piedra, &j->hierro, &j->hojas, &j->comida};
        
        for (int i = 0; i < 5; i++) {
            int ix = startX; 
            int iy = startY + (i * 50); // Lista vertical

            // Ajustamos el área de clic para la lista vertical de venta
            if (mx >= ix && mx <= ix + 250 && my >= iy && my <= iy + 40) {
                int cantidadAVender = (esClickDerecho) ? 10 : 1;
                
                if (*stocks[i] > 0) {
                    if (cantidadAVender > *stocks[i]) cantidadAVender = *stocks[i];
                    
                    // IMPORTANTE: Usamos la función interna para restar
                    // pero aquí lo hacemos directo al puntero
                    *stocks[i] -= cantidadAVender;
                    j->oro += (cantidadAVender * precios[i]);
                    
                    char txtVenta[32];
                    sprintf(txtVenta, "+%d Oro", cantidadAVender * precios[i]);
                    crearTextoFlotante(j->x, j->y, txtVenta, 0, RGB(255, 215, 0));
                    
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
        }
    }
}

/// --- DIBUJAR LA INTERFAZ DE LA TIENDA (TECLA T) ---
void dibujarTiendaInteractiva(HDC hdc, Jugador *j) {
    int tx = 250; 
    int ty = 100;
    int anchoW = 340;
    int altoW = 400;

    // 1. FONDO
    HBRUSH fondo = CreateSolidBrush(RGB(45, 40, 35));
    RECT r = {tx, ty, tx + anchoW, ty + altoW};
    FillRect(hdc, &r, fondo); DeleteObject(fondo);
    HBRUSH borde = CreateSolidBrush(RGB(218, 165, 32));
    FrameRect(hdc, &r, borde); DeleteObject(borde);

    // 2. TÍTULO Y PESTAÑAS
    SetBkMode(hdc, TRANSPARENT);
    
    COLORREF colorC = (j->modoTienda == 0) ? RGB(255, 215, 0) : RGB(100, 100, 100);
    SetTextColor(hdc, colorC);
    TextOut(hdc, tx + 40, ty + 10, "[ COMPRAR ]", 11);

    COLORREF colorV = (j->modoTienda == 1) ? RGB(255, 215, 0) : RGB(100, 100, 100);
    SetTextColor(hdc, colorV);
    TextOut(hdc, tx + 180, ty + 10, "[ VENDER ]", 10);

    HPEN linea = CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
    HGDIOBJ oldPen = SelectObject(hdc, linea);
    MoveToEx(hdc, tx, ty + 35, NULL);
    LineTo(hdc, tx + anchoW, ty + 35);
    SelectObject(hdc, oldPen); DeleteObject(linea);

    // 3. DIBUJAR ITEMS
    int startX = tx + 20;
    int startY = ty + 50;

    if (j->modoTienda == 0) { // COMPRAR
        for (int i = 0; i < 9; i++) {
            int col = i % 2;
            int row = i / 2;
            int ix = startX + (col * 160);
            int iy = startY + (row * 65);

            HBITMAP icono = NULL;
            char nombre[32] = "";
            char precio1[32] = "";
            char precio2[32] = "";
            BOOL posible = FALSE;
            BOOL yaTiene = FALSE;

            switch(i) {
                case 0: // CAZADOR
                    icono = hBmpIconoEspada; strcpy(nombre, "Cazador");
                    sprintf(precio1, "20 Oro"); sprintf(precio2, "3 Hierro");
                    if (j->oro >= 20 && j->hierro >= 3) posible = TRUE;
                    break;
                case 1: // MINERO
                    icono = hBmpIconoPico; strcpy(nombre, "Minero");
                    sprintf(precio1, "15 Oro"); sprintf(precio2, "5 Piedra");
                    if (j->oro >= 15 && j->piedra >= 5) posible = TRUE;
                    break;
                case 2: // LEÑADOR
                    icono = hBmpIconoHacha; strcpy(nombre, "Lenador");
                    sprintf(precio1, "10 Oro"); sprintf(precio2, "10 Mad.");
                    if (j->oro >= 10 && j->madera >= 10) posible = TRUE;
                    break;
                case 3: // SOLDADO
                    icono = hBmpIconoArmaduraInv; strcpy(nombre, "Soldado");
                    sprintf(precio1, "40 Oro"); sprintf(precio2, "10 Hierro");
                    if (j->oro >= 40 && j->hierro >= 10) posible = TRUE;
                    break;
                    
                // --- AQUÍ ESTÁN LOS CAMBIOS DE TEXTO ---
                case 4: // MOCHILA N2
                    icono = hBmpInvCerrado; strcpy(nombre, "Mochila N2");
                    if (j->nivelMochila >= 2) { yaTiene = TRUE; }
                    else { 
                        sprintf(precio1, "100 Oro"); sprintf(precio2, "20 Hoj."); // Nuevo precio
                        if(j->oro >= 300 && j->madera >= 50) posible = TRUE;
                    }
                    break;
                case 5: // MOCHILA N3
                    icono = hBmpInvCerrado; strcpy(nombre, "Mochila N3");
                    if (j->nivelMochila >= 3) { yaTiene = TRUE; }
                    else if (j->nivelMochila == 2) {
                        sprintf(precio1, "200 Oro"); sprintf(precio2, "50 Hierro"); // Nuevo precio
                        if(j->oro >= 800 && j->hierro >= 100) posible = TRUE;
                    } else sprintf(precio1, "Bloq.");
                    break;
                    
                case 6: // CAÑA
                    icono = hBmpIconoCana; strcpy(nombre, "Cana Pesca");
                    if (j->tieneCana) yaTiene = TRUE;
                    else if (j->nivelMochila >= 2) {
                        sprintf(precio1, "30 Oro"); sprintf(precio2, "10 Mad.");
                        if(j->oro >= 30 && j->madera >= 10) posible = TRUE;
                    } else sprintf(precio1, "Req. Nvl 2");
                    break;
                case 7: // BOTE
                    icono = hBmpBote[1]; strcpy(nombre, "Bote");
                    if (j->tieneBotePesca) yaTiene = TRUE;
                    else if (j->nivelMochila >= 2) {
                        sprintf(precio1, "50 Oro"); sprintf(precio2, "30 Mad.");
                        if(j->oro >= 50 && j->madera >= 30) posible = TRUE;
                    } else sprintf(precio1, "Req. Nvl 2");
                    break;
                case 8: // BARCO
                    icono = hBmpBarco[1]; strcpy(nombre, "Galeon");
                    if (j->tieneBarcoGuerra) yaTiene = TRUE;
                    else if (j->nivelMochila >= 2) {
                        sprintf(precio1, "100 Oro"); sprintf(precio2, "50 Mad.");
                        if(j->oro >= 100 && j->madera >= 50) posible = TRUE;
                    } else sprintf(precio1, "Req. Nvl 2");
                    break;
            }

            if (icono) DibujarImagen(hdc, icono, ix, iy, 32, 32);
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOut(hdc, ix + 40, iy - 5, nombre, strlen(nombre));

            if (yaTiene) {
                SetTextColor(hdc, RGB(100, 100, 100));
                TextOut(hdc, ix + 40, iy + 10, "COMPRADO", 8);
            } else {
                SetTextColor(hdc, posible ? RGB(0, 255, 0) : RGB(255, 50, 50));
                TextOut(hdc, ix + 40, iy + 10, precio1, strlen(precio1));
                TextOut(hdc, ix + 40, iy + 22, precio2, strlen(precio2));
            }
        }
    } 
    else { // VENDER
        char *nombres[] = {"Madera", "Piedra", "Hierro", "Hojas", "Comida"};
        HBITMAP iconos[] = {hBmpIconoMadera, hBmpIconoPiedra, hBmpIconoHierro, hBmpIconoHoja, hBmpIconoComida};
        int cantidades[] = {j->madera, j->piedra, j->hierro, j->hojas, j->comida};
        int valores[] = {1, 2, 5, 1, 3};

        for (int i = 0; i < 5; i++) {
            int ix = startX; 
            int iy = startY + (i * 50);

            if (iconos[i]) DibujarImagen(hdc, iconos[i], ix, iy, 32, 32);
            
            char texto[64];
            sprintf(texto, "%s (Tienes: %d)", nombres[i], cantidades[i]);
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOut(hdc, ix + 40, iy, texto, strlen(texto));

            sprintf(texto, "Vender: +%d Oro", valores[i]);
            SetTextColor(hdc, RGB(255, 215, 0)); 
            TextOut(hdc, ix + 40, iy + 15, texto, strlen(texto));
        }
    }
}

void dibujarTiendasEnIslas(HDC hdc, Camera cam, int ancho, int alto, int frameTienda) {
    int tx = (1450 - cam.x) * cam.zoom;
    int ty = (1900 - cam.y) * cam.zoom;
    int tam = 50 * cam.zoom;
    int f = (frameTienda / 20) % 2;
    if (hBmpTienda[f]) DibujarImagen(hdc, hBmpTienda[f], tx, ty, tam, tam);
}

// --- 8. INITIALIZACIÓN JUEGO ---

void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int mapaId)
{
    inicializarMapa(mapa, mapaId);
    inicializarArboles(mapa);
    inicializarVacas();
    inicializarMinas(mapa);
    inicializarTesoros(); 
    // inicializarUnidades();

    jugador->x = 1600; jugador->y = 1600;
    jugador->velocidad = 5;
    jugador->vidaActual = 100; jugador->vidaMax = 100;
    jugador->nivel = 1; jugador->experiencia = 0;
    
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
    int separacion = 30;
    
    // Centrado
    int totalAncho = (btnAncho * 3) + (separacion * 2);
    int startX = (ancho - totalAncho) / 2;
    int fixedY = (alto / 2) - (btnAlto / 2);

    // 3. DIBUJAR BOTONES (Imágenes limpias)
    
    // Botón 1: JUGAR
    HBITMAP imgJugar = (hBmpBtnJugar) ? hBmpBtnJugar : hBmpBoton;
    DibujarImagen(hdc, imgJugar, startX, fixedY, btnAncho, btnAlto);

    // Botón 2: PARTIDAS
    HBITMAP imgPartidas = (hBmpBtnPartidas) ? hBmpBtnPartidas : hBmpBoton;
    DibujarImagen(hdc, imgPartidas, startX + btnAncho + separacion, fixedY, btnAncho, btnAlto);

    // Botón 3: INSTRUCCIONES
    HBITMAP imgInstr = (hBmpBtnInstrucciones) ? hBmpBtnInstrucciones : hBmpBoton;
    DibujarImagen(hdc, imgInstr, startX + (btnAncho + separacion) * 2, fixedY, btnAncho, btnAlto);

    // Botón 4: SALIR
    int margen = 30;
    int salirX = ancho - btnAncho - margen;
    int salirY = alto - btnAlto - margen;
    HBITMAP imgSalir = (hBmpBtnSalir) ? hBmpBtnSalir : hBmpBoton;
    DibujarImagen(hdc, imgSalir, salirX, salirY, btnAncho, btnAlto);

    // (HEMOS ELIMINADO LA SECCIÓN 4 QUE DIBUJABA EL BORDE DORADO)
    
    // 5. TÍTULO DEL JUEGO
    if (hBmpTitulo) {
        int tituloW = 600; 
        int tituloH = 150;
        DibujarImagen(hdc, hBmpTitulo, (ancho - tituloW)/2, fixedY - 180, tituloW, tituloH);
    }
}

// Función auxiliar para detectar colisión simple (Local)
int puntoEnRect(int x, int y, int rx, int ry, int rw, int rh) {
    return (x >= rx && x <= rx + rw && y >= ry && y <= ry + rh);
}

// --- PROCESAR CLIC EN EL MENÚ ---
void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    int ancho = rc.right;
    int alto = rc.bottom;

    // Configuración idéntica a dibujarMenuConSprites
    int btnAncho = 500;
    int btnAlto = 200;
    int separacion = 30;
    
    // Cálculo de posición inicial (Centrado)
    int totalAncho = (btnAncho * 3) + (separacion * 2);
    int startX = (ancho - totalAncho) / 2;
    int startY = (alto / 2) - (btnAlto / 2); // Centrado verticalmente

    // 1. BOTÓN JUGAR
    if (puntoEnRect(x, y, startX, startY, btnAncho, btnAlto))
    {
        // Sonido de confirmación (Opcional)
        PlaySound("SystemStart", NULL, SND_ASYNC);
        
        // Cambiar estado a Selección de Mapa
        estado->estadoActual = ESTADO_SELECCION_MAPA;
        estado->opcionSeleccionada = 0; // Resetear selección del siguiente menú
        
        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }

    // 2. BOTÓN PARTIDAS
    int xPartidas = startX + btnAncho + separacion;
    if (puntoEnRect(x, y, xPartidas, startY, btnAncho, btnAlto))
    {
        MessageBox(hwnd, "Sistema de guardado en desarrollo.", "Partidas", MB_OK | MB_ICONINFORMATION);
        return;
    }

    // 3. BOTÓN INSTRUCCIONES
    int xInstr = startX + (btnAncho + separacion) * 2;
    if (puntoEnRect(x, y, xInstr, startY, btnAncho, btnAlto))
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

    // 4. BOTÓN SALIR (Esquina Inferior Derecha)
    int margen = 30;
    int xSalir = ancho - btnAncho - margen;
    int ySalir = alto - btnAlto - margen;
    
    if (puntoEnRect(x, y, xSalir, ySalir, btnAncho, btnAlto))
    {
        // Confirmación antes de salir
        if (MessageBox(hwnd, "Seguro que quieres salir?", "Salir", MB_YESNO | MB_ICONQUESTION) == IDYES) {
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
    const char* titulo = "SELECCIONA TU DESTINO";
    
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
        switch(i) {
            case 0: img = seleccionado ? hBmpCuadroMapa1Sel : hBmpCuadroMapa1Normal; break;
            case 1: img = seleccionado ? hBmpCuadroMapa2Sel : hBmpCuadroMapa2Normal; break;
            case 2: img = seleccionado ? hBmpCuadroMapa3Sel : hBmpCuadroMapa3Normal; break;
        }

        // B. Dibujar Imagen
        if (img) {
            DibujarImagen(hdc, img, x, y, cuadroAncho, cuadroAlto);
        } else {
            // Placeholder si falla la imagen
            HBRUSH pBrush = CreateSolidBrush(seleccionado ? RGB(100,100,200) : RGB(50,50,50));
            RECT rP = {x, y, x+cuadroAncho, y+cuadroAlto};
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
        switch(i) {
            case 0: strcpy(nombre, "MAPA 1: ISLAS DEL MAR"); break;
            case 1: strcpy(nombre, "MAPA 2: ARCHIPIELAGO"); break;
            case 2: strcpy(nombre, "MAPA 3: CONTINENTE"); break;
        }

        SetTextColor(hdc, seleccionado ? RGB(255, 255, 255) : RGB(150, 150, 150));
        TextOut(hdc, x + 10, y + cuadroAlto + 15, nombre, strlen(nombre));
    }

    // 4. INSTRUCCIONES INFERIORES
    SetTextColor(hdc, RGB(200, 200, 200));
    const char* instr = "Haz Clic para seleccionar  |  Presiona ENTER para Iniciar  |  ESC para Volver";
    
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
            if (estado->opcionSeleccionada != i) {
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
    if (estado->estadoActual != ESTADO_MENU) return;

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
                            "BLOQ MAYUS: Abrir Mochila\n"
                            "T: Abrir Tienda (Cerca del edificio)\n"
                            "TAB: Equipar/Quitar Armadura\n"
                            "1, 2, 3: Equipar Herramientas\n"
                            "R: Curar vida (Usa Hojas)\n\n"
                            "OBJETIVO: Recolecta recursos y mejora tu mochila!";
                MessageBox(hwnd, msg, "Instrucciones", MB_OK | MB_ICONQUESTION);
            }
            break;

        case 3: // SALIR
            if (MessageBox(hwnd, "Seguro que quieres salir?", "Salir", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                PostQuitMessage(0);
            }
            break;
    }
}

// --- 10. RENDERIZADO FINAL ---

void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera cam, int ancho, int alto, int frameTienda, int mapaId)
{
    // 1. FONDO DE AGUA (La capa más baja)
    HBRUSH agua = CreateSolidBrush(RGB(0, 100, 180));
    RECT r = {0, 0, ancho, alto}; 
    FillRect(hdc, &r, agua); 
    DeleteObject(agua);

    // 2. ISLAS (Tierra firme)
    for (int i = 0; i < MAX_ISLAS; i++) {
        if (!misIslas[i].activa) continue;
        
        int sx = (misIslas[i].x - cam.x) * cam.zoom;
        int sy = (misIslas[i].y - cam.y) * cam.zoom;
        
        // Optimización: No dibujar si está fuera de pantalla
        if (sx + (misIslas[i].ancho * cam.zoom) < 0 || sx > ancho) continue; 

        HBITMAP img = obtenerImagenIsla(i, mapaId);
        if (img) DibujarImagen(hdc, img, sx, sy, misIslas[i].ancho * cam.zoom, misIslas[i].alto * cam.zoom);
    }

    // 3. OBJETOS ESTÁTICOS (Parte del escenario)
    dibujarTiendasEnIslas(hdc, cam, ancho, alto, frameTienda);
    dibujarArboles(hdc, cam, ancho, alto, mapaId);
    
    // NOTA: Las vacas, minas y el jugador se dibujan en main.c 
    // para asegurar que queden ENCIMA del mapa.
}