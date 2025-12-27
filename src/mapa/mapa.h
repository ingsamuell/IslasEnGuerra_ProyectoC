#ifndef MAPA_H
#define MAPA_H

#include <windows.h>
#include <wingdi.h>
#include "../global.h" // Necesario para struct Jugador y EstadoJuego

/* --- LÓGICA DEL JUEGO (Setup y Física) --- */
void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy);

/* --- CÁMARA --- */
void actualizarCamara(Camera *camara, Jugador jugador);

/* --- DIBUJADO PRINCIPAL (Partida) --- */
void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera cam, int ancho, int alto, int frameTienda);
void dibujarJugador(HDC hdc, Jugador jugador, Camera camara);
void dibujarHUD(HDC hdc, Jugador* jugador, int anchoVentana, int altoVentana);

/* --- DIBUJADO DE MENÚ --- */
void dibujarMenuConSprites(HDC hdc, HWND hwnd, EstadoJuego *estado);

/* --- INTERACCIÓN MENÚ --- */
void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado);
void procesarEnterMenu(HWND hwnd, EstadoJuego *estado);

/* --- INTERACCIÓN JUEGO (Mochila y Tienda) - NUEVAS --- */
void procesarClickMochila(int mouseX, int mouseY, Jugador *jugador, HWND hwnd);
void procesarClickMochilaTienda(int mx, int my, int esClickDerecho, Jugador *j, HWND hwnd);

#endif /* MAPA_H */