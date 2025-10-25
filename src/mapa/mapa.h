#ifndef MAPA_H
#define MAPA_H

#include "raylib.h"

#define FILAS 80
#define COLUMNAS 120
#define NUM_ISLAS 12
#define TAM_CELDA 16

// Estados del juego
typedef enum {
    MENU_PRINCIPAL,
    NUEVA_PARTIDA,
    CARGAR_PARTIDA,
    VER_RECURSOS,
    EXPLORACION,
    BATALLA,
    SALIR
} GameScreen;

// Estructura para guardar partidas
typedef struct {
    int madera;
    int oro;
    int posX;
    int posY;
    bool tieneBarco;
    int partidasGanadas;
    int partidasPerdidas;
} PartidaGuardada;

// Estructura del jugador
typedef struct {
    Vector2 posicion;      // Posición en píxeles
    int velocidad;
    int madera;           // Recursos del jugador
    int oro;
    bool tieneBarco;
    int golpesDados;      // Contador para espacio (recolección)
    bool recolectando;    // Estado de recolección
    Color color;
} Jugador;

// Estructura de una isla
typedef struct {
    int centroX, centroY;
    int radioX, radioY;
    Color colorBase;
    int tipo;
} Isla;

// Estructura para la cámara
typedef struct {
    Camera2D camara;
    Vector2 objetivo;
    float zoom;
} CamaraJuego;

// Estructura para botones del menú
typedef struct {
    Rectangle bounds;
    const char *text;
    bool hovered;
} Button;

// Declaraciones de funciones del menú
GameScreen MostrarMenu(PartidaGuardada *partidaAnterior);
Button CrearBoton(float x, float y, float width, float height, const char *text);
void DibujarBoton(Button boton);
void DibujarMenuPrincipal(PartidaGuardada *partidaAnterior);
void dibujarMenuRecursos(PartidaGuardada *partida);

// Declaraciones de funciones del sistema de partidas
bool guardarPartida(Jugador *jugador);
bool cargarPartida(PartidaGuardada *partida);

// Declaraciones de funciones del jugador
void inicializarJugador(Jugador *jugador);
void actualizarJugador(Jugador *jugador, char mapa[FILAS][COLUMNAS]);
void dibujarJugador(Jugador *jugador);
void dibujarUI(Jugador *jugador);
bool puedeMoverseAJugador(int x, int y, char mapa[FILAS][COLUMNAS], Jugador *jugador);
void verificarRecoleccion(Jugador *jugador, char mapa[FILAS][COLUMNAS]);

// Declaraciones de funciones de la cámara
void inicializarCamara(CamaraJuego *camara);
void actualizarCamara(CamaraJuego *camara, Jugador *jugador);

// Declaraciones de funciones del mapa
void inicializarMapa(char mapa[FILAS][COLUMNAS]);
void dibujarMapa(char mapa[FILAS][COLUMNAS]);
void dibujarIsla(Isla isla, char mapa[FILAS][COLUMNAS]);
void agregarVegetacionFija(char mapa[FILAS][COLUMNAS]);
void agregarOroFijo(char mapa[FILAS][COLUMNAS]);
void agregarMonumentosFijos(char mapa[FILAS][COLUMNAS]);
void agregarPalmerasFijas(char mapa[FILAS][COLUMNAS]);
void agregarRocasFijas(char mapa[FILAS][COLUMNAS]);
void agregarAguaTexturizada(char mapa[FILAS][COLUMNAS]);
int estaEnElipse(int x, int y, int centroX, int centroY, int radioX, int radioY);

// Función utilitaria
float ClampManual(float value, float min, float max);

#endif