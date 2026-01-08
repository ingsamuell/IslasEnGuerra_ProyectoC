/* src/jugador/jugador.h */
#ifndef JUGADOR_H
#define JUGADOR_H

#include "../global.h"
#include <windows.h>

// Lógica
int obtenerCapacidadMaxima(int nivel);
void ganarExperiencia(Jugador *j, int cantidad);
void intentarCurar(Jugador *j);
void alternarInventario(Jugador *j);

// --- CORRECCIÓN AQUÍ: Volvemos a la firma que mapa.c necesita ---
void agregarRecurso(int *recurso, int cantidad, int nivelMochila);

// Visuales
void dibujarItemRejilla(HDC hdc, HBITMAP icono, int cantidad, int maximo, int x, int y, const char *nombre);
void dibujarHUD(HDC hdc, Jugador *jugador, int ancho, int alto);
void dibujarJugador(HDC hdc, Jugador *jugador, Camera cam);

#endif