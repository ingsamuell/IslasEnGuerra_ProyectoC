#ifndef TIENDA_H
#define TIENDA_H

#include <windows.h>
#include "../global.h"

// --- INTERFAZ PÚBLICA DE LA TIENDA ---
// Procesa los clics dentro de la ventana de la tienda (compras, pestañas, ventas)
void procesarClickMochilaTienda(int x, int y, int clickDerecho, Jugador *j, HWND hwnd);

// Dibuja la interfaz gráfica (UI) de la tienda cuando está abierta
void dibujarTiendaInteractiva(HDC hdc, Jugador *j, int anchoVentana, int altoVentana);

// Dibuja el edificio físico de la tienda en el mapa del juego
void dibujarTiendasEnIslas(HDC hdc, Camera cam, int ancho, int alto, int frameTienda);

#endif