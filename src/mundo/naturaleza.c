#include "naturaleza.h"
#include "mapa.h"             // Para EsSuelo(), crearChispas(), crearTextoFlotante()
#include "../recursos/recursos.h" // Para las imágenes (HBITMAPs)
#include "../jugador/jugador.h"   // Para modificar el inventario del jugador
#include <math.h>
#include <stdio.h>

// Definición de variables globales
Arbol misArboles[MAX_ARBOLES];
Mina misMinas[MAX_MINAS];
Tesoro misTesoros[MAX_TESOROS];

// Función auxiliar local (copia simple para no depender de mapa.c)
void crearChispaNaturaleza(float x, float y) { 
    crearChispas((int)x, (int)y, RGB(255, 255, 255)); 
}

// --- ÁRBOLES ---

void inicializarArboles(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    int cnt = 0, intentos = 0;
    int minX = 1200, maxX = 2000, minY = 1200, maxY = 2000;
    
    for (int i = 0; i < MAX_ARBOLES; i++) misArboles[i].activa = 0;

    while (cnt < MAX_ARBOLES && intentos < 50000) {
        intentos++;
        int rx = minX + (rand() % (maxX - minX));
        int ry = minY + (rand() % (maxY - minY));
        int tipo = rand() % 2;

        if (!EsSuelo(rx + 16, ry + 30, mapa)) continue;
        
        int cerca = 0;
        for (int i = 0; i < cnt; i++) {
            if (sqrt(pow(rx - misArboles[i].x, 2) + pow(ry - misArboles[i].y, 2)) < 150) {
                cerca = 1;
                break;
            }
        }
        if (cerca) continue;

        misArboles[cnt].x = rx;
        misArboles[cnt].y = ry;
        misArboles[cnt].tipo = tipo;
        misArboles[cnt].activa = 1;
        misArboles[cnt].vida = 5;
        cnt++;
    }
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

        if (img && sx > -tam && sx < ancho && sy > -tam && sy < alto)
            DibujarImagen(hdc, img, sx, sy, tam, tam);
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
    int cnt = 0, intentos = 0;
    int minX = 1200, maxX = 2000, minY = 1200, maxY = 2000;
    
    for (int i = 0; i < MAX_MINAS; i++) misMinas[i].activa = 0;

    while (cnt < MAX_MINAS && intentos < 10000) {
        intentos++;
        int rx = minX + (rand() % (maxX - minX));
        int ry = minY + (rand() % (maxY - minY));
        if (!EsSuelo(rx + 16, ry + 16, mapa)) continue;

        int cerca = 0;
        for (int i = 0; i < cnt; i++)
            if (sqrt(pow(rx - misMinas[i].x, 2) + pow(ry - misMinas[i].y, 2)) < 100) cerca = 1;
        if (cerca) continue;

        misMinas[cnt].x = rx;
        misMinas[cnt].y = ry;
        misMinas[cnt].tipo = rand() % 2;
        misMinas[cnt].vida = 5;
        misMinas[cnt].activa = 1;
        cnt++;
    }
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

                if (misMinas[i].tipo == 0) { // Piedra
                    int ant = j->piedra;
                    agregarRecurso(&j->piedra, 5, j->nivelMochila);
                    int gan = j->piedra - ant;
                    if (gan > 0) crearTextoFlotante(misMinas[i].x, misMinas[i].y, "Piedra", gan, RGB(150, 150, 150));
                    else crearTextoFlotante(j->x, j->y - 40, "Mochila Llena!", 0, RGB(255, 50, 50));
                } else { // Hierro
                    int ant = j->hierro;
                    agregarRecurso(&j->hierro, 3, j->nivelMochila);
                    int gan = j->hierro - ant;
                    if (gan > 0) crearTextoFlotante(misMinas[i].x, misMinas[i].y, "Hierro", gan, RGB(192, 192, 192));
                    else crearTextoFlotante(j->x, j->y - 40, "Mochila Llena!", 0, RGB(255, 50, 50));
                }
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

        if (abs((j->x + 16) - (misTesoros[i].x + 16)) < 60 && 
            abs((j->y + 16) - (misTesoros[i].y + 16)) < 60) {
            
            if (misTesoros[i].estado == 0) { misTesoros[i].estado = 1; return; }
            if (misTesoros[i].estado == 1) {
                int oro = 30 + (rand() % 11);
                int antOro = j->oro;
                agregarRecurso(&j->oro, oro, j->nivelMochila); 
                int ganOro = j->oro - antOro;

                if (ganOro > 0) crearTextoFlotante(misTesoros[i].x, misTesoros[i].y, "Oro", ganOro, RGB(255, 215, 0));
                else crearTextoFlotante(j->x, j->y - 40, "Bolsa de Oro Llena!", 0, RGB(255, 50, 50));

                if (misTesoros[i].tipo == 1) {
                    int hierro = 20 + (rand() % 10);
                    int antHierro = j->hierro;
                    agregarRecurso(&j->hierro, hierro, j->nivelMochila);
                    int ganHierro = j->hierro - antHierro;
                    if (ganHierro > 0) crearTextoFlotante(misTesoros[i].x, misTesoros[i].y - 25, "Hierro", ganHierro, RGB(192, 192, 192));
                }
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