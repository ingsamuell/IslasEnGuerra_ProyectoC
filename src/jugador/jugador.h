/* src/jugador/jugador.h */
#ifndef JUGADOR_H
#define JUGADOR_H

#include "../global.h"
#include <windows.h>

// --- NUEVAS FUNCIONES DE MOVIMIENTO (Traídas de mapa.c) ---
void moverJugador(Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy);
void actualizarCamara(Camera *camara, Jugador j);
void intentarMontarBarco(Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
// Lógica
void dibujarBarraVidaLocal(HDC hdc, int x, int y, int vidaActual, int vidaMax, int ancho);
int obtenerCapacidadMaxima(int nivel);
void ganarExperiencia(Jugador *j, int cantidad);
void intentarCurar(Jugador *j);
void alternarInventario(Jugador *j);
void procesarClickMochila(int x, int y, Jugador *j, HWND hwnd);
void alternarInventario(Jugador *j);
void intentarCurar(Jugador *j);

// --- CORRECCIÓN AQUÍ: Volvemos a la firma que mapa.c necesita ---
void agregarRecurso(int *recurso, int cantidad, int nivelMochila);

// Visuales
void dibujarItemRejilla(HDC hdc, HBITMAP icono, int cantidad, int maximo, int x, int y, const char *nombre);
void dibujarHUD(HDC hdc, Jugador *jugador, int ancho, int alto);
void dibujarJugador(HDC hdc, Jugador *jugador, Camera cam);

// --- SISTEMA DE LÍMITES ---
void mantenerDentroDelMapa(Jugador *j);

#endif