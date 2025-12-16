#ifndef MAPA_H
#define MAPA_H

#include <windows.h>
#include <wingdi.h>

/* --- Definiciones del Juego --- */
#define MUNDO_FILAS 100
#define MUNDO_COLUMNAS 100
#define TAMANO_CELDA_BASE 10     // Tamaño base de celda para zoom
#define PANTALLA_FILAS 60        // 600 / 10 = 60
#define PANTALLA_COLUMNAS 80     // 800 / 10 = 80
#define TAMANO_CELDA 10          // Tamaño actual de celda

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
    float zoom;                 // factor de zoom (0.5x - 4.0x)
    int tamano_celda_actual;    // tamaño actual considerando zoom
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

/* --- Prototipos de Funciones (SOLO las que usas) --- */

/* ========== LÓGICA DEL JUEGO ========== */
void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void moverJugador(Jugador *jugador, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy);

/* ========== SISTEMA DE CÁMARA Y ZOOM ========== */
void actualizarCamara(Camera *camara, Jugador jugador);

/* ========== DIBUJADO DEL JUEGO (ÚNICA FUNCIÓN) ========== */
void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera camara, int anchoVentana, int altoVentana);
void dibujarJugador(HDC hdc, Jugador jugador, Camera camara);

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