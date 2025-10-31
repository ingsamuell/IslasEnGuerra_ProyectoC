#ifndef MAPA_H
#define MAPA_H

#include <windows.h>
#include <wingdi.h>

/* --- Definiciones del Juego --- */
#define MUNDO_FILAS 100
#define MUNDO_COLUMNAS 100
#define PANTALLA_FILAS 18
#define PANTALLA_COLUMNAS 25
#define TAMANO_CELDA 32  // Aumenté el tamaño para mejor visibilidad

/* Estructuras del juego */
typedef struct {
    int x; 
    int y;
    int oro;
    int madera;
    int piedra;
    int vida;
    int vida_maxima;
} Jugador;

typedef struct {
    int x;
    int y;
} Camera;

typedef struct {
    int x;
    int y;
} PuntoMenu;

typedef struct {
    int mostrarMenu;
    int enPartida;
    int mostrarResumen;
    int opcionSeleccionada;
    PuntoMenu puntoMouse;
} EstadoJuego;

/* --- Prototipos de Funciones --- */
void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy);
void actualizarCamara(Camera *camara, Jugador jugador);
void dibujarMapa(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera camara, int anchoVentana, int altoVentana);
void dibujarJugador(HDC hdc, Jugador jugador, Camera camara);

/* Funciones del sistema de menú mejorado */
void dibujarMenu(HDC hdc, HWND hwnd, EstadoJuego *estado);
void dibujarResumenRecursos(HDC hdc, Jugador jugador, EstadoJuego *estado);
void dibujarBoton(HDC hdc, int x, int y, int ancho, int alto, const char* texto, BOOL seleccionado, BOOL activo);
void dibujarFondoAnimado(HDC hdc, int ancho, int alto, int tiempo);
int verificarColisionBoton(int mouseX, int mouseY, int btnX, int btnY, int btnAncho, int btnAlto);
void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado);
void procesarEnterMenu(HWND hwnd, EstadoJuego *estado);
void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);

#endif