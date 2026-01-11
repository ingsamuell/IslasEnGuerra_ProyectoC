/* src/enemigos/enemigos.h */
#ifndef ENEMIGOS_H
#define ENEMIGOS_H

#include "../global.h"
#include <windows.h>

// Configuración de oleadas
void actualizarInvasiones(Jugador *j);

// Lógica de la IA
void actualizarIAEnemigos(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Jugador *j);

// Creación (Spawn)
void spawnearEnemigo(int tipo, float x, float y);
void crearOleada(float x, float y, int cantidadPiratas, int cantidadMagos);

#endif