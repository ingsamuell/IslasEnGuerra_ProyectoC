#include "enemigos.h"
#include "../unidades/unidades.h"
#include "../mundo/mapa.h" 
#include "../mundo/edificios.h" // Para ver si los castillos siguen vivos
#include <math.h>
#include <stdio.h>

extern Unidad unidades[MAX_UNIDADES];
extern Edificio edificiosEnemigos[MAX_EDIFICIOS_ENEMIGOS];

// Configuración de Oleadas
int timerInvasion = 0;
int tiempoProximaOleada = 3600; // 1 minuto (a 60fps)

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
                unidades[i].vidaMax = 100;
                unidades[i].vida = 100;
                unidades[i].damage = 5;
                unidades[i].rangoAtaque = 40; // Cuerpo a cuerpo (Espada)
                unidades[i].cooldownAtaque = 60; // 1 segundo
            } 
            else if (tipo == TIPO_ENEMIGO_MAGO) {
                unidades[i].vidaMax = 60; // Menos vida
                unidades[i].vida = 60;
                unidades[i].damage = 8;
                unidades[i].rangoAtaque = 250; // ¡MAGIA A DISTANCIA!
                unidades[i].cooldownAtaque = 120; // Dispara lento
            }
            
            crearChispas((int)x, (int)y, RGB(255, 0, 0)); // Efecto de aparición
            return;
        }
    }
}

// --- 2. IA DE COMBATE (CEREBRO) ---
void actualizarIAEnemigos(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Jugador *j) {
    for (int i = 0; i < MAX_UNIDADES; i++) {
        // Solo procesar enemigos activos
        if (!unidades[i].activa || unidades[i].bando != BANDO_ENEMIGO) continue;

        Unidad *u = &unidades[i];

        // A. BUSCAR OBJETIVO (Soldado cercano o Jugador)
        float distJugador = sqrt(pow(j->x - u->x, 2) + pow(j->y - u->y, 2));
        int targetId = -1;
        float minDist = 9999;

        // Buscar si hay algún soldado aliado cerca para atacar
        for(int k=0; k<MAX_UNIDADES; k++) {
            if(unidades[k].activa && unidades[k].bando == BANDO_ALIADO) {
                float d = sqrt(pow(unidades[k].x - u->x, 2) + pow(unidades[k].y - u->y, 2));
                if(d < 300 && d < minDist) { // Prioridad a soldados cercanos
                    minDist = d;
                    targetId = k;
                }
            }
        }

        float distObjetivo = (targetId != -1) ? minDist : distJugador;
        float objX = (targetId != -1) ? unidades[targetId].x : j->x;
        float objY = (targetId != -1) ? unidades[targetId].y : j->y;

        // B. COMPORTAMIENTO
        if (distObjetivo < 600) { // Si ve a alguien
            
            // 1. ATACAR (Si está en rango)
            if (distObjetivo <= u->rangoAtaque) {
                u->estado = ESTADO_ATACANDO;
                u->timerAtaque++;
                
                if (u->timerAtaque >= u->cooldownAtaque) {
                    u->timerAtaque = 0;
                    
                    // Aplicar Daño
                    if (targetId != -1) {
                        unidades[targetId].vida -= u->damage; // Dañar Soldado
                        crearChispas(objX, objY, RGB(255, 0, 0));
                        if(unidades[targetId].vida <= 0) unidades[targetId].activa = 0; // Matar soldado
                    } else {
                        j->vidaActual -= u->damage; // Dañar Jugador
                        crearTextoFlotante(j->x, j->y - 30, "-VIDA", 0, RGB(200, 0, 0));
                        if(j->vidaActual < 0) j->vidaActual = 0;
                    }

                    // EFECTO VISUAL DEL ATAQUE
                    if (u->tipo == TIPO_ENEMIGO_MAGO) {
                        crearChispas(objX, objY, RGB(0, 255, 255)); // Magia Azul
                        PlaySound("SystemAsterisk", NULL, SND_ASYNC); 
                    } else {
                        // Pirata (Espada)
                        crearChispas(objX, objY, RGB(200, 200, 200)); 
                    }
                }
            } 
            // 2. PERSEGUIR (Si está lejos)
            else {
                u->estado = ESTADO_PERSIGUIENDO;
                float vel = 1.8f;
                float dx = objX - u->x;
                float dy = objY - u->y;
                
                u->x += (dx / distObjetivo) * vel;
                u->y += (dy / distObjetivo) * vel;
                
                // Animación simple
                if (fabs(dx) > fabs(dy)) u->direccion = (dx > 0) ? 3 : 2;
                else u->direccion = (dy > 0) ? 0 : 1;
                u->frameAnim = (u->frameAnim + 1) % 3; 
                actualizarAnimacionUnidad(u, dx, dy);
            }
        }
        
        aplicarSeparacion(i, mapa); // Evitar que se amontonen
        
    }
}

// --- 3. SISTEMA DE INVASIÓN Y REGENERACIÓN ---
void actualizarInvasiones(Jugador *j) {
    if (j->nivel < 5) return; // Solo nivel 5+

    // A) INVASIÓN A TU ISLA (Defensa)
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