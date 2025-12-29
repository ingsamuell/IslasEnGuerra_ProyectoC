#ifndef MAPA_H
#define MAPA_H

#include <windows.h>
#include <wingdi.h>
#include "../global.h"

/* --- LÓGICA DEL JUEGO (Setup y Física) --- */
void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy);

// IMPORTANTE: EsSuelo ahora recibe el mapa para la colisión precisa
int EsSuelo(int x, int y);

/* --- SISTEMA DE VACAS --- */
void inicializarVacas(); // Sin argumentos
void actualizarVacas();
void dibujarVacas(HDC hdc, Camera cam, int ancho, int alto);

// (Esta función aún no la hemos creado en mapa.c, la comento para evitar errores)
// void golpearVaca(Jugador *j); 

/* --- SISTEMA DE ÁRBOLES --- */
void inicializarArboles(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void dibujarArboles(HDC hdc, Camera cam, int ancho, int alto);
void intentarTalarArbol(Jugador *j); // <--- AGREGAR ESTA LÍNEA

/* --- CÁMARA --- */
void actualizarCamara(Camera *camara, Jugador jugador);

/* --- DIBUJADO PRINCIPAL (Partida) --- */
void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera cam, int ancho, int alto, int frameTienda);
void dibujarJugador(HDC hdc, Jugador jugador, Camera camara);
void dibujarHUD(HDC hdc, Jugador* jugador, int anchoVentana, int altoVentana);
void dibujarTiendasEnIslas(HDC hdc, Camera cam, int ancho, int alto, int frameTienda);
void dibujarTiendaInteractiva(HDC hdc, Jugador *j);
void dibujarPrecio(HDC hdc, int x, int y, const char* costo1, const char* costo2, BOOL alcanzable);
void dibujarItemRejilla(HDC hdc, HBITMAP icono, int cantidad, int maximo, int x, int y, const char *nombre);

/* --- DIBUJADO DE MENÚ --- */
void dibujarMenuConSprites(HDC hdc, HWND hwnd, EstadoJuego *estado);

/* --- INTERACCIÓN --- */
void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado);
void procesarEnterMenu(HWND hwnd, EstadoJuego *estado);
void procesarClickMochila(int mouseX, int mouseY, Jugador *jugador, HWND hwnd);
void procesarClickMochilaTienda(int mx, int my, int esClickDerecho, Jugador *j, HWND hwnd);

#endif /* MAPA_H */