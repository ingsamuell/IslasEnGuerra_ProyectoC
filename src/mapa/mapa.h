#ifndef MAPA_H
#define MAPA_H

#include <windows.h>
#include <wingdi.h>
#include "../global.h" // <--- IMPORTANTE: Usamos la definición de global.h

/* --- Prototipos de Funciones (SOLO las que usas) --- */

/* ========== LÓGICA DEL JUEGO ========== */
void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy);

/* ========== SISTEMA DE CÁMARA Y ZOOM ========== */
void actualizarCamara(Camera *camara, Jugador jugador);

/* ========== DIBUJADO DEL JUEGO (ÚNICA FUNCIÓN) ========== */
void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera cam, int ancho, int alto, int frameTienda);
void dibujarJugador(HDC hdc, Jugador jugador, Camera camara);
void dibujarHUD(HDC hdc, Jugador* jugador, int anchoVentana, int altoVentana);

/* ========== SISTEMA DE MENÚ ========== */
void dibujarMenu(HDC hdc, HWND hwnd, EstadoJuego *estado);
void dibujarMenuConSprites(HDC hdc, HWND hwnd, EstadoJuego *estado);
void dibujarResumenRecursos(HDC hdc, Jugador jugador, EstadoJuego *estado);
void dibujarBoton(HDC hdc, int x, int y, int ancho, int alto, const char* texto, BOOL seleccionado, BOOL activo);
void dibujarFondoAnimado(HDC hdc, int ancho, int alto, int tiempo);

/* ========== INTERACCIÓN DEL MENÚ ========== */
int verificarColisionBoton(int mouseX, int mouseY, int btnX, int btnY, int btnAncho, int btnAlto);
void actualizarPuntoMenu(EstadoJuego *estado, int x, int y, HWND hwnd);
void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado);
void procesarEnterMenu(HWND hwnd, EstadoJuego *estado);

/* ========== MANEJO DE ESTADO GLOBAL ========== */
EstadoJuego* obtenerEstadoJuego();
void iniciarPartida();
void volverAlMenu();

/* ========== FUNCIONES ADICIONALES (opcionales) ========== */
void dibujarFondoAnimadoMejorado(HDC hdc, int ancho, int alto, int tiempo);
void dibujarBotonMejorado(HDC hdc, int x, int y, int ancho, int alto, const char* texto, BOOL seleccionado, BOOL activo);

#endif /* MAPA_H */