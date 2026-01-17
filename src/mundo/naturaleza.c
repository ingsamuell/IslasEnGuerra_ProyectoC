#include "naturaleza.h"
#include "mapa.h"            
#include "../recursos/recursos.h" 
#include "../jugador/jugador.h"
#include "../mundo/mapa.h"
#include "../global.h"   
#include <math.h>
#include <stdio.h>

// Definición de variables globales
Arbol misArboles[MAX_ARBOLES];
Mina misMinas[MAX_MINAS];
Tesoro misTesoros[MAX_TESOROS];

// --- FUNCIONES AUXILIARES DE DIBUJO ---
void dibujarBarraVidaLocal(HDC hdc, int x, int y, int vidaActual, int vidaMax, int ancho) {
    if (vidaActual < vidaMax && vidaActual > 0) {
        HBRUSH bBg = CreateSolidBrush(RGB(150, 0, 0));
        RECT rBg = {x, y, x + ancho, y + 4};
        FillRect(hdc, &rBg, bBg);
        DeleteObject(bBg);

        float pct = (float)vidaActual / (float)vidaMax;
        int wGreen = (int)(ancho * pct);
        HBRUSH bFg = CreateSolidBrush(RGB(0, 255, 0));
        RECT rFg = {x, y, x + wGreen, y + 4};
        FillRect(hdc, &rFg, bFg);
        DeleteObject(bFg);
        
        HBRUSH bFrame = CreateSolidBrush(RGB(0,0,0));
        FrameRect(hdc, &rBg, bFrame);
        DeleteObject(bFrame);
    }
}

void crearChispaNaturaleza(float x, float y) { 
    crearChispas((int)x, (int)y, RGB(255, 255, 255)); 
}

// FUNCIONES PARA COLOCACIÓN MANUAL 
void ponerArbol(int id, int x, int y, int tipo) {
    if (id < 0 || id >= MAX_ARBOLES) return;

    misArboles[id].x = x;
    misArboles[id].y = y;
    misArboles[id].tipo = tipo; // 0=Chico, 1=Grande
    misArboles[id].activa = 1;
    misArboles[id].vida = 5;
    misArboles[id].timerRegeneracion = 0;
}

void ponerMina(int id, int x, int y, int tipo) {
    if (id < 0 || id >= MAX_MINAS) return;

    misMinas[id].x = x;
    misMinas[id].y = y;
    misMinas[id].tipo = tipo; // 0=Piedra, 1=Hierro
    misMinas[id].activa = 1;
    misMinas[id].vida = 5;
    misMinas[id].timerRegeneracion = 0;
}

// --- ÁRBOLES ---
void inicializarArboles(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    // 1. Limpiar arreglo
    for (int i = 0; i < MAX_ARBOLES; i++) misArboles[i].activa = 0;

    // 2. COLOCACIÓN MANUAL DE ÁRBOLES
    // -- Bosque al Oeste (Cerca del Establo) --
    ponerArbol(0, 1150, 1350, 1);
    ponerArbol(1, 1200, 1320, 0);
    ponerArbol(2, 1100, 1400, 1);
    ponerArbol(3, 1180, 1450, 0);
    ponerArbol(4, 1250, 1300, 1);

    // -- Bosque al Sur-Este (Cerca de la Tienda) --
    ponerArbol(5, 1330, 1830, 1);
    ponerArbol(6, 1255, 1915, 0);
    ponerArbol(7, 1420, 1790, 1);
    ponerArbol(8, 1580, 1865, 0);
    ponerArbol(9, 1125, 1825, 1);

    // -- Árboles Dispersos --
    ponerArbol(10, 1400, 1600, 0);
    ponerArbol(11, 1450, 1650, 1);
    ponerArbol(12, 1350, 1700, 0);
    ponerArbol(13, 1210, 1235, 1);
    ponerArbol(14, 1375, 1365, 0);
    ponerArbol(15, 1400, 1280, 1);
    ponerArbol(16, 1675, 1775, 0);
    ponerArbol(17, 1725, 1775, 1);
    ponerArbol(18, 1515, 1495, 0);
    ponerArbol(19, 1510, 1240, 1);
    ponerArbol(20, 1610, 1220, 0);
    ponerArbol(21, 1675, 1125, 1);
    ponerArbol(22, 1750, 1125, 0);
    ponerArbol(23, 1850, 1165, 1);
    ponerArbol(24, 1955, 1130, 0);
    ponerArbol(25, 1970, 1270, 1);
    ponerArbol(26, 1785, 1305, 0);

}

void talarArbol(Jugador *j) {
    int rango = 40;
    for (int i = 0; i < MAX_ARBOLES; i++) {
        if (!misArboles[i].activa) continue;
        int tam = (misArboles[i].tipo == 1) ? 64 : 32;
        
        if (abs((j->x+16) - (misArboles[i].x + tam/2)) < rango && 
            abs((j->y+16) - (misArboles[i].y + tam/2)) < rango) {
            
            misArboles[i].vida--;
            crearChispas(misArboles[i].x + tam/2, misArboles[i].y + tam/2, RGB(139, 69, 19));
            
            if (misArboles[i].vida <= 0) {
                misArboles[i].activa = 0;
                misArboles[i].timerRegeneracion = 0;

                int mad = (misArboles[i].tipo == 1) ? 5 : 3;
                int hoj = (misArboles[i].tipo == 1) ? 10 : 4;
                
                int antMad = j->madera;
                int antHoj = j->hojas;

                agregarRecurso(&j->madera, mad, j->nivelMochila);
                agregarRecurso(&j->hojas, hoj, j->nivelMochila);
                
                int ganMad = j->madera - antMad;
                int ganHoj = j->hojas - antHoj;

                if (ganMad > 0) crearTextoFlotante(misArboles[i].x, misArboles[i].y, "Madera", ganMad, RGB(150, 75, 0));
                if (ganHoj > 0) crearTextoFlotante(misArboles[i].x, misArboles[i].y - 20, "Hojas", ganHoj, RGB(34, 139, 34));
                
                ganarExperiencia(j, XP_ARBOL); 
                crearTextoFlotante(misArboles[i].x, misArboles[i].y - 40, "+15 XP", 0, RGB(0, 255, 255));

                if (ganMad == 0 && ganHoj == 0) {
                    crearTextoFlotante(j->x, j->y - 40, "Mochila Llena!", 0, RGB(255, 50, 50));
                }
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
        if (mapaId == 0) img = (misArboles[i].tipo == 1) ? hBmpArbolGrande : hBmpArbolChico;
        else if (mapaId == 1) img = (misArboles[i].tipo == 1) ? hBmpArbolGrandeMapa2 : hBmpArbolChicoMapa2;
        else img = (misArboles[i].tipo == 1) ? hBmpArbolGrandeMapa3 : hBmpArbolChicoMapa3;

        if (img && sx > -tam && sx < ancho && sy > -tam && sy < alto){
            DibujarImagen(hdc, img, sx, sy, tam, tam);
            dibujarBarraVidaLocal(hdc, sx, sy - 8, misArboles[i].vida, 5, 32 * cam.zoom);
        }
    }
}

int buscarArbolCercano(float x, float y, float rango) {
    int mejorIndice = -1;
    float mejorDist = rango;
    for (int i = 0; i < MAX_ARBOLES; i++) {
        if (!misArboles[i].activa) continue;
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

// --- MINAS ---
void inicializarMinas(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    // 1. Limpiar arreglo
    for (int i = 0; i < MAX_MINAS; i++) misMinas[i].activa = 0;

    // 2. COLOCACIÓN MANUAL DE MINAS
    // -- Cantera de Piedra (Cerca del Establo) --
    ponerMina(0, 1170, 1540, 0);
    ponerMina(1, 1215, 1515, 0);
    ponerMina(2, 1135, 1630, 0);

    // -- Depósito de Hierro (Más al norte) --
    ponerMina(3, 1300, 1200, 1);
    ponerMina(4, 1350, 1220, 1);

    // -- Mixtas (Cerca de la tienda) --
    ponerMina(5, 1360, 1940, 0); // Piedra
    ponerMina(6, 1255, 1965, 1); // Hierro
    ponerMina(7, 1580, 1710, 0); // Piedra
    ponerMina(8, 1685, 1730, 1); // Hierro
    ponerMina(9, 1820, 1760, 0); // Piedra
    ponerMina(10, 1675, 1825, 1); // Hierro
    ponerMina(11, 1335, 1785, 0); // Piedra
}

void picarMina(Jugador *j) {
    for (int i = 0; i < MAX_MINAS; i++) {
        if (!misMinas[i].activa) continue;
        
        if (abs((j->x+16) - (misMinas[i].x+16)) < 40 && 
            abs((j->y+16) - (misMinas[i].y+16)) < 40) {
            
            misMinas[i].vida--;
            crearChispas(misMinas[i].x+16, misMinas[i].y+16, RGB(200,200,200));
            
            if (misMinas[i].vida <= 0) {
                misMinas[i].activa = 0;
                misMinas[i].timerRegeneracion = 0;

                // PIEDRA
                if (misMinas[i].tipo == 0) { 
                    int ant = j->piedra;
                    agregarRecurso(&j->piedra, 10, j->nivelMochila);
                    int gan = j->piedra - ant;
                    
                    if (gan > 0) crearTextoFlotante(misMinas[i].x, misMinas[i].y, "Piedra", gan, RGB(150, 150, 150));
                    else crearTextoFlotante(j->x, j->y - 40, "Mochila Llena!", 0, RGB(255, 50, 50));
                    
                    ganarExperiencia(j, XP_PIEDRA); 
                    crearTextoFlotante(misMinas[i].x, misMinas[i].y - 25, "+10 XP", 0, RGB(0, 255, 255));
                } 
                // HIERRO
                else { 
                    int ant = j->hierro;
                    agregarRecurso(&j->hierro, 15, j->nivelMochila);
                    int gan = j->hierro - ant;
                    
                    if (gan > 0) crearTextoFlotante(misMinas[i].x, misMinas[i].y, "Hierro", gan, RGB(192, 192, 192));
                    else crearTextoFlotante(j->x, j->y - 40, "Mochila Llena!", 0, RGB(255, 50, 50));
                    
                    ganarExperiencia(j, XP_HIERRO); 
                    crearTextoFlotante(misMinas[i].x, misMinas[i].y - 25, "+15 XP", 0, RGB(0, 255, 255));
                }
            }
            return;
        }
    }
}

void dibujarMinas(HDC hdc, Camera cam, int ancho, int alto) {
    for (int i = 0; i < MAX_MINAS; i++) {
        if (!misMinas[i].activa) continue;

        int col = (int)(misMinas[i].x / TAMANO_CELDA_BASE);
        int fila = (int)(misMinas[i].y / TAMANO_CELDA_BASE);

        if (col >= 0 && col < MUNDO_COLUMNAS && fila >= 0 && fila < MUNDO_FILAS) {
            if (neblina[fila][col] == 0) continue; 
        }

        int sx = (int)((misMinas[i].x - cam.x) * cam.zoom);
        int sy = (int)((misMinas[i].y - cam.y) * cam.zoom);
        int tam = 32 * cam.zoom;

        if (sx > -tam && sx < ancho && sy > -tam && sy < alto) {
            HBITMAP img = (misMinas[i].tipo == 1) ? hBmpHierroPicar : hBmpPiedraPicar;
            
            if (img) {
                DibujarImagen(hdc, img, sx, sy, tam, tam);
                dibujarBarraVidaLocal(hdc, sx, sy - 8, misMinas[i].vida, 5, tam);
            }
        }
    }
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

// --- TESOROS ---
void inicializarTesoros() {
    misTesoros[0].x = 1320; misTesoros[0].y = 1250; misTesoros[0].tipo = 0; misTesoros[0].estado = 0; misTesoros[0].activa = 1;
    misTesoros[1].x = 1850; misTesoros[1].y = 1550; misTesoros[1].tipo = 1; misTesoros[1].estado = 0; misTesoros[1].activa = 1;
}

void abrirTesoro(Jugador *j) {
    for (int i = 0; i < MAX_TESOROS; i++) {
        if (!misTesoros[i].activa || misTesoros[i].estado == 2) continue;

        float dx = j->x - misTesoros[i].x;
        float dy = j->y - misTesoros[i].y;
        float dist = sqrt(dx*dx + dy*dy);

        if (dist < 50 && misTesoros[i].estado == 0) {
            misTesoros[i].estado = 1; 
            int tx = misTesoros[i].x;
            int ty = misTesoros[i].y;

            if (misTesoros[i].tipo == 0) {
                int oroSolo = 50 + (rand() % 50);
                j->oro += oroSolo;
                crearTextoFlotante(tx, ty, "+Oro", 0, RGB(255, 215, 0));
            }
            else { 
                int oroGanado = 50 + (rand() % 10);
                int hierroGanado = 10 + (rand() % 10);
                j->oro += oroGanado;
                j->hierro += hierroGanado;
                
                crearTextoFlotante(tx, ty, "Joyas encontradas!", 0, RGB(255, 0, 255));
                crearTextoFlotante(tx, ty - 25, "+Oro y Hierro", 0, RGB(255, 215, 0));

                if (!j->tieneMapa) {
                    j->tieneMapa = TRUE;
                    crearTextoFlotante(tx, ty - 50, "MINIMAPA OBTENIDO!", 0, RGB(50, 255, 50));
                }
            }
            misTesoros[i].estado = 2; 
        }
    }
}

void dibujarTesoros(HDC hdc, Camera cam, int ancho, int alto) {
    for (int i = 0; i < MAX_TESOROS; i++) {
        if (!misTesoros[i].activa) continue;
        int col = (int)(misTesoros[i].x / TAMANO_CELDA_BASE);
        int fila = (int)(misTesoros[i].y / TAMANO_CELDA_BASE);

        if (col >= 0 && col < MUNDO_COLUMNAS && fila >= 0 && fila < MUNDO_FILAS) {
            if (neblina[fila][col] == 0) continue; 
        }
        int sx = (misTesoros[i].x - cam.x) * cam.zoom;
        int sy = (misTesoros[i].y - cam.y) * cam.zoom;
        HBITMAP img = hBmpTesoroVacio;
        if (misTesoros[i].estado == 0) img = hBmpTesoroCerrado;
        else if (misTesoros[i].estado == 1) img = (misTesoros[i].tipo == 0) ? hBmpTesoroOro : hBmpTesoroJoyas;

        if (img) DibujarImagen(hdc, img, sx, sy, 32 * cam.zoom, 32 * cam.zoom);
    }
}

void actualizarRegeneracionNaturaleza() {
    // Regenerar Árboles
    for (int i = 0; i < MAX_ARBOLES; i++) {
        if (!misArboles[i].activa) {
            misArboles[i].timerRegeneracion++;
            if (misArboles[i].timerRegeneracion >= TIEMPO_RESPAWN_RECURSOS) {
                misArboles[i].activa = 1;
                misArboles[i].vida = 5;
                misArboles[i].timerRegeneracion = 0;
                crearChispaNaturaleza(misArboles[i].x, misArboles[i].y);
            }
        }
    }
    // Regenerar Minas
    for (int i = 0; i < MAX_MINAS; i++) {
        if (!misMinas[i].activa) {
            misMinas[i].timerRegeneracion++;
            if (misMinas[i].timerRegeneracion >= TIEMPO_RESPAWN_RECURSOS) {
                misMinas[i].activa = 1;
                misMinas[i].vida = 8;
                misMinas[i].timerRegeneracion = 0;
                crearChispaNaturaleza(misMinas[i].x, misMinas[i].y);
            }
        }
    }
}