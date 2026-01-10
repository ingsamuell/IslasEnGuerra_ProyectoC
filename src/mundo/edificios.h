#ifndef EDIFICIOS_H
#define EDIFICIOS_H

#include <windows.h>
#include "../jugador/jugador.h" // Necesario para 'Jugador'
#include "../global.h"

// Constantes
#define MAX_EDIFICIOS_JUGADOR 3
#define MAX_EDIFICIOS_ENEMIGOS 4

// Arrays Globales (Externos)
extern Edificio misEdificios[MAX_EDIFICIOS_JUGADOR];
extern Edificio edificiosEnemigos[MAX_EDIFICIOS_ENEMIGOS];

// Prototipos de Funciones
void inicializarEdificios();
void dibujarEdificio(HDC hdc, int x, int y, int tipoTamano, int esEnemigo, int mapaID);
void dibujarFantasmaConstruccion(HDC hdc, Jugador *j, int mx, int my, int mapaID);
void intentarColocarEdificio(Jugador *j, int mx, int my, int mapaID);
void actualizarEdificios(float deltaTiempo);
#endif