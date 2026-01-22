// Andrea Sanchez

#include "enemigos.h"
#include "../unidades/unidades.h"
#include "../mundo/mapa.h" 
#include "../mundo/edificios.h" 
#include <math.h>
#include <stdio.h>

extern Unidad unidades[MAX_UNIDADES];
extern Edificio edificiosEnemigos[MAX_EDIFICIOS_ENEMIGOS];
extern int screenShake; 

// Configuración de Oleadas
int timerInvasion = 0;
int tiempoProximaOleada = 3000; // 1 minuto (a 60fps)

// --- 1. SPAWNEAR ENEMIGO ---
void spawnearEnemigo(int tipo, float x, float y) {
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa) {
            unidades[i].activa = 1;
            unidades[i].tipo = tipo;
            unidades[i].x = x;
            unidades[i].y = y;
            unidades[i].bando = BANDO_ENEMIGO;
            unidades[i].estado = ESTADO_IDLE;
            unidades[i].seleccionado = 0;
            
            // CONFIGURACIÓN DE CLASES
            if (tipo == TIPO_ENEMIGO_PIRATA) {
                unidades[i].vidaMax = 40;
                unidades[i].vida = 40;
                unidades[i].damage = 20;
                unidades[i].rangoAtaque = 40; // Cuerpo a cuerpo (Espada)
                unidades[i].cooldownAtaque = 60; 
            } 
            else if (tipo == TIPO_ENEMIGO_MAGO) {
                unidades[i].vidaMax = 40; 
                unidades[i].vida = 40;
                unidades[i].damage = 15;
                unidades[i].rangoAtaque = 150; // ¡MAGIA A DISTANCIA!
                unidades[i].cooldownAtaque = 100; // Dispara lento
            }
            
            crearChispas((int)x, (int)y, RGB(255, 0, 0)); // Efecto de aparición
            return;
        }
    }
}

// --- 2. COMBATE (CEREBRO) ---
void actualizarIAEnemigos(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Jugador *j) {
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa || unidades[i].bando != BANDO_ENEMIGO) continue;

        Unidad *u = &unidades[i];

        // --- A. BUSCAR OBJETIVO ---
        float distJugador = sqrt(pow(j->x - u->x, 2) + pow(j->y - u->y, 2));
        int targetId = -1;
        float minDist = 9999;

        for(int k = 0; k < MAX_UNIDADES; k++) {
            if(unidades[k].activa && unidades[k].bando == BANDO_ALIADO) {
                float d = sqrt(pow(unidades[k].x - u->x, 2) + pow(unidades[k].y - u->y, 2));
                if(d < 300 && d < minDist) {
                    minDist = d;
                    targetId = k;
                }
            }
        }

        float distObjetivo = (targetId != -1) ? minDist : distJugador;
        float objX = (targetId != -1) ? unidades[targetId].x : j->x;
        float objY = (targetId != -1) ? unidades[targetId].y : j->y;

        // --- B. COMPORTAMIENTO ---
        if (distObjetivo < 600) { 
            
            // 1. ATACAR
            if (distObjetivo <= u->rangoAtaque) {
                u->estado = ESTADO_ATACANDO;
                u->timerAtaque++;

                // --- EFECTO VISUAL DE CARGA  ---
                if (u->tipo == TIPO_ENEMIGO_MAGO && u->timerAtaque == u->cooldownAtaque - 20) {
                    crearProyectilMagico(u->x, u->y, objX, objY); 

                }
                
                // --- MOMENTO DEL GOLPE ---
                if (u->timerAtaque >= u->cooldownAtaque) {
                    u->timerAtaque = 0;
                    
                    if (targetId != -1) {
                        unidades[targetId].vida -= u->damage;
                        crearSangre(objX, objY); // Efecto de sangre
                        if(unidades[targetId].vida <= 0) unidades[targetId].activa = 0;
                        screenShake = 5; // Un temblor pequeño para golpes normales
                    } else {
                        j->vidaActual -= u->damage;
                        crearSangre(j->x, j->y); // Efecto de sangre
                        crearTextoFlotante(j->x, j->y - 30, "-VIDA", 0, RGB(255, 50, 50));
                        screenShake = 5; // Un temblor pequeño para golpes normales
                        if(j->vidaActual < 0) j->vidaActual = 0;
                    }
                }
            } 
            // 2. PERSEGUIR
            else {
                u->estado = ESTADO_PERSIGUIENDO;
                float vel = 1.8f;
                float dx = objX - u->x;
                float dy = objY - u->y;
                
                u->x += (dx / distObjetivo) * vel;
                u->y += (dy / distObjetivo) * vel;
                
                if (fabs(dx) > fabs(dy)) u->direccion = (dx > 0) ? 3 : 2;
                else u->direccion = (dy > 0) ? 0 : 1;
                
                u->frameAnim = (u->frameAnim + 1) % 3; 
                actualizarAnimacionUnidad(u, dx, dy);
            }
        }
        aplicarSeparacion(i, mapa);
    }
}

// --- 3. SISTEMA DE INVASIÓN Y REGENERACIÓN ---
void actualizarInvasiones(Jugador *j) {
    if (j->nivel < 5) return; // Solo nivel 5+

    // A) INVASIÓN A TU ISLA 
    timerInvasion++;
    if (timerInvasion >= tiempoProximaOleada) {
        timerInvasion = 0;
        crearTextoFlotante(j->x, j->y - 80, "BARCOS PIRATAS!", 0, RGB(255, 50, 0));
        
        // Aparecen en la costa (Cerca del muelle)
        spawnearEnemigo(TIPO_ENEMIGO_PIRATA, 1400, 2200);
        spawnearEnemigo(TIPO_ENEMIGO_PIRATA, 1450, 2200);
        spawnearEnemigo(TIPO_ENEMIGO_MAGO, 1420, 2250);
    }

    // B) REGENERACIÓN EN ISLAS ENEMIGAS (Ataque)
    static int timerRespawn = 0;
    timerRespawn++;
    if (timerRespawn > 1200) { // Cada 20 segundos
        timerRespawn = 0;
        
        // Isla Norte (Piratas) - Si el castillo sigue vivo
        if (edificiosEnemigos[0].activo) {
            spawnearEnemigo(TIPO_ENEMIGO_PIRATA, edificiosEnemigos[0].x, edificiosEnemigos[0].y + 100);
        }
        // Isla Sur (Magos) - Si el castillo sigue vivo
        if (edificiosEnemigos[1].activo) {
            spawnearEnemigo(TIPO_ENEMIGO_MAGO, edificiosEnemigos[1].x, edificiosEnemigos[1].y + 100);
        }
    }
}