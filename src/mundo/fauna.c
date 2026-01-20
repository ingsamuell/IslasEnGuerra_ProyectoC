#include "fauna.h"
#include "mapa.h"      
#include "../recursos/recursos.h" 
#include "../jugador/jugador.h"
#include "../global.h"
#include <math.h>
#include <stdio.h>

// Definición de las variables globales de fauna
Vaca manada[MAX_VACAS];
Tiburon misTiburones[MAX_TIBURONES];

// --- VACAS ---
void inicializarVacas() {
    // 1. Limpiar todas primero
    for (int i = 0; i < MAX_VACAS; i++)
        manada[i].activa = 0;
    
    // 2. Crear las 7 vacas ordenadas
    for (int i = 0; i < 7; i++) {
        manada[i].activa = 1;

        // --- LÓGICA DE POSICIÓN ORDENADA ---
        // Calculamos fila y columna para que no se encimen
        int columna = i % 3; // (0, 1, 2, 0, 1, 2...)
        int fila = i / 3;    // (0, 0, 0, 1, 1, 1...)

        // Espaciado: 60px de ancho y 50px de alto entre cada una
        int separacionX = 60; 
        int separacionY = 50;

        // Posición Base + Cuadrícula + Pequeña variación aleatoria 
        manada[i].x = ESTABLO_X + (columna * separacionX) + (rand() % 10);
        manada[i].y = ESTABLO_Y + (fila * separacionY) + (rand() % 10);
        manada[i].xInicial = manada[i].x;
        manada[i].direccion = (rand() % 2 == 0) ? 1 : -1;
        manada[i].vida = 5;
        manada[i].estadoVida = 0;
        manada[i].frameAnim = (manada[i].direccion == 1) ? 4 : 0;
        manada[i].tiempoMuerte = 0;
        manada[i].timerRegeneracion = 0;
    }
}

void actualizarVacas() {
    for (int i = 0; i < MAX_VACAS; i++) {
        if (!manada[i].activa) continue;

        if (manada[i].estadoVida == 1) { // Muerta
            manada[i].tiempoMuerte--;
            if (manada[i].tiempoMuerte <= 0) {
                manada[i].activa = 0;            // Desaparece
                manada[i].timerRegeneracion = 0; // Comienza respawn
            }
            continue;
        }

        // Patrulla simple (Izquierda/Derecha)
        if (manada[i].direccion == 1) {
            manada[i].x++;
            if (manada[i].x >= manada[i].xInicial + 30) {
                manada[i].direccion = -1;
                manada[i].frameAnim = 0;
            }
        } else {
            manada[i].x--;
            if (manada[i].x <= manada[i].xInicial) {
                manada[i].direccion = 1;
                manada[i].frameAnim = 4;
            }
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
        // Ignorar vacas inactivas o ya muertas
        if (!manada[i].activa || manada[i].estadoVida == 1) continue;
        // Detección de golpe
        if (abs((j->x + 16) - (manada[i].x + 16)) < rango && 
            abs((j->y + 16) - (manada[i].y + 16)) < rango) {
            manada[i].vida--;
            crearChispas(manada[i].x + 16, manada[i].y + 16, RGB(200, 0, 0)); // Sangre
            if (manada[i].vida <= 0) {
                manada[i].estadoVida = 1; // Marcar como muerta
                manada[i].tiempoMuerte = 300; // Tiempo para desaparecer
                // 1. Dar recursos (Comida)
                int ant = j->comida;
                agregarRecurso(&j->comida, 10, j->nivelMochila);
                int gan = j->comida - ant;
                if (gan > 0) crearTextoFlotante(manada[i].x, manada[i].y, "Carne", gan, RGB(255, 100, 100));
                else crearTextoFlotante(j->x, j->y - 40, "Mochila Llena!", 0, RGB(255, 50, 50));
                // 2. Dar Experiencia
                ganarExperiencia(j, XP_VACA); // +15 XP
                crearTextoFlotante(manada[i].x, manada[i].y - 25, "+15 XP", 0, RGB(0, 255, 255)); // Texto Cyan
            }
            return; // Solo golpear una vaca a la vez
        }
    }
}

void dibujarVacas(HDC hdc, Camera cam, int ancho, int alto) {
    for (int i = 0; i < MAX_VACAS; i++) {
        // 1. Omitir si la vaca no existe
        if (!manada[i].activa) continue;
        // 2. FILTRO DE NIEBLA 
        int col = (int)(manada[i].x / TAMANO_CELDA_BASE);
        int fila = (int)(manada[i].y / TAMANO_CELDA_BASE);

        // Validar que estamos dentro de los límites de la matriz neblina
        if (col >= 0 && col < MUNDO_COLUMNAS && fila >= 0 && fila < MUNDO_FILAS) {
            if (neblina[fila][col] == 0) continue; 
        }

        // 3. CÁLCULO DE POSICIÓN EN PANTALLA
        int sx = (int)((manada[i].x - cam.x) * cam.zoom);
        int sy = (int)((manada[i].y - cam.y) * cam.zoom);
        int tam = 32 * cam.zoom;

        // 4. OPTIMIZACIÓN: Solo dibujar si está dentro de la cámara
        if (sx > -tam && sx < ancho && sy > -tam && sy < alto) {
            
            if (manada[i].estadoVida == 1) { 
                // CASO: VACA MUERTA
                if (hBmpVacaMuerta) {
                    DibujarImagen(hdc, hBmpVacaMuerta, sx, sy, tam, tam);
                }
            } 
            else { 
                // CASO: VACA VIVA
                if (hBmpVaca[manada[i].frameAnim]) {
                    DibujarImagen(hdc, hBmpVaca[manada[i].frameAnim], sx, sy, tam, tam);
                }
                // DIBUJAR BARRA DE VIDA (Justo encima de la vaca)
                dibujarBarraVidaLocal(hdc, sx, sy - 10, manada[i].vida, 5, tam);
            }
        }
    }
}

int buscarVacaCercana(float x, float y, float rango) {
    int mejorIndice = -1;
    float mejorDist = rango;

    for (int i = 0; i < MAX_VACAS; i++) {
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

// --- TIBURONES ---

void inicializarTiburones(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    // Inicializar array
    for(int i=0; i<MAX_TIBURONES; i++) misTiburones[i].activa = 0;
    int tiburonesCreados = 0;
    int intentos = 0;
    while (tiburonesCreados < MAX_TIBURONES && intentos < 2000) {
        int tx = rand() % (MUNDO_COLUMNAS * TAMANO_CELDA_BASE);
        int ty = rand() % (MUNDO_FILAS * TAMANO_CELDA_BASE);

        float distCentro = sqrt(pow(tx - 1600, 2) + pow(ty - 1600, 2));
        // Usamos EsSuelo (función de mapa.h) para verificar que sea agua
        if (!EsSuelo(tx, ty, mapa) && distCentro > 1100) {
            misTiburones[tiburonesCreados].x = (float)tx;
            misTiburones[tiburonesCreados].y = (float)ty;
            misTiburones[tiburonesCreados].direccion = rand() % 2; 
            misTiburones[tiburonesCreados].frameAnim = rand() % 4; 
            misTiburones[tiburonesCreados].timerAnim = rand() % 10;
            misTiburones[tiburonesCreados].activa = 1;
            tiburonesCreados++;
        }
        intentos++;
    }
}

void actualizarTiburones(Jugador *j) {
    // Timer de inmunidad del jugador
    if (j->timerInmunidadBarco > 0) j->timerInmunidadBarco--;
    for (int i = 0; i < MAX_TIBURONES; i++) {
        if (!misTiburones[i].activa) continue;
        // Animación
        misTiburones[i].timerAnim++;
        if (misTiburones[i].timerAnim >= 8) { 
            misTiburones[i].timerAnim = 0;
            misTiburones[i].frameAnim++;
            if (misTiburones[i].frameAnim >= 4) {
                misTiburones[i].frameAnim = 0; 
            }
        }
        // Colisión con Barco de Guerra
        if (j->estadoBarco == 2 && j->timerInmunidadBarco == 0) { 
            float dist = sqrt(pow(j->x - misTiburones[i].x, 2) + pow(j->y - misTiburones[i].y, 2));
            
            if (dist < 40.0f) { 
                j->vidaActual -= 10; 
                if (j->vidaActual < 0) j->vidaActual = 0;
                crearTextoFlotante(j->x, j->y, "-10 VIDA!", 0, RGB(255, 0, 0));
                crearChispas(j->x + 16, j->y + 16, RGB(200, 0, 0)); 
                j->timerInmunidadBarco = 60; 
            }
        }
    }
}

void dibujarTiburones(HDC hdc, Camera cam, int ancho, int alto) {
    for (int i = 0; i < MAX_TIBURONES; i++) {
        if (!misTiburones[i].activa) continue;

        // No dibujar si está fuera de cámara
        if (misTiburones[i].x > cam.x - 100 && misTiburones[i].x < cam.x + (ancho/cam.zoom) + 100 &&
            misTiburones[i].y > cam.y - 100 && misTiburones[i].y < cam.y + (alto/cam.zoom) + 100) {
            int screenX = (int)((misTiburones[i].x - cam.x) * cam.zoom);
            int screenY = (int)((misTiburones[i].y - cam.y) * cam.zoom);
            int tam = 48 * cam.zoom;
            HBITMAP hBmpTibu = hBmpTiburonAnim[misTiburones[i].direccion][misTiburones[i].frameAnim];
            if (hBmpTibu) {
                 // Usamos un fondo rosa transparente estándar
                 HDC hdcMem = CreateCompatibleDC(hdc);
                 HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmpTibu);
                 BITMAP bm; GetObject(hBmpTibu, sizeof(BITMAP), &bm);
                 
                 TransparentBlt(hdc, screenX, screenY, tam, tam, 
                                hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, 
                                RGB(255, 0, 255));

                 SelectObject(hdcMem, hOld); DeleteDC(hdcMem);
            }
        }
    }
}

// Regeneración global de fauna (se llama cada frame o cada cierto tiempo)
void actualizarRegeneracionFauna() {
    for (int i = 0; i < MAX_VACAS; i++) {
        if (!manada[i].activa) {
            manada[i].timerRegeneracion++;
            if (manada[i].timerRegeneracion >= TIEMPO_RESPAWN_RECURSOS) {
                manada[i].activa = 1;
                manada[i].estadoVida = 0;     
                manada[i].vida = 5;           
                manada[i].tiempoMuerte = 0; 
                manada[i].timerRegeneracion = 0;
                manada[i].x = manada[i].xInicial;
                crearChispaBlanca(manada[i].x, manada[i].y);
            }
        }
    }
}