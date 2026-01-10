#ifndef MAPA_H
#define MAPA_H

#include <windows.h>
#include <wingdi.h>
#include "../global.h"

// Constantes de Límites
#define MAX_EDIFICIOS_JUGADOR 3   // 1 de cada tipo
#define MAX_EDIFICIOS_ENEMIGOS 4  // 1 por cada isla secundaria

// Arrays Globales (Externos)
extern Edificio misEdificios[MAX_EDIFICIOS_JUGADOR];
extern Edificio edificiosEnemigos[MAX_EDIFICIOS_ENEMIGOS];

// Prototipo para inicializar los edificios al arrancar
void inicializarEdificios();

/* --- LÓGICA DEL JUEGO (Setup y Física) --- */
void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int mapaId);
void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int mapaId);

void dibujarMiniMapa(HDC hdc, Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int anchoPantalla, int altoPantalla);

// IMPORTANTE: EsSuelo ahora recibe el mapa para la colisión precisa
int EsSuelo(int x, int y, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);

/* --- SISTEMA DE ÁRBOLES --- */
void dibujarBarraVidaLocal(HDC hdc, int x, int y, int vidaActual, int vidaMax, int anchoTotal);
void inicializarArboles(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void dibujarArboles(HDC hdc, Camera cam, int ancho, int alto, int mapaId);
void talarArbol(Jugador *j); 

// Inicializa las minas en el mapa
void inicializarMinas(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);

// Dibuja las minas usando los bitmaps de hierro y piedra
void dibujarMinas(HDC hdc, Camera cam, int ancho, int alto);

// Lógica para detectar si el jugador está picando una mina
void picarMina(Jugador *j);
void crearChispas(int x, int y, COLORREF color);
void actualizarYDibujarParticulas(HDC hdc, Camera cam);
void agregarTextoFlotante(int x, int y, char* contenido, COLORREF color);
void actualizarYDibujarTextos(HDC hdc, Camera cam);
void actualizarLogicaSistema(Jugador *j);
void crearTextoFlotante(int x, int y, const char* formato, int cantidad, COLORREF color);


// --- NUEVO: Función auxiliar para colisiones de caja (si la necesitas pública) ---
int VerificarColisionCaja(int x, int y, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);

/* --- SISTEMA DE TESOROS --- */
void inicializarTesoros();
void abrirTesoro(Jugador *j);
void dibujarTesoros(HDC hdc, Camera cam, int ancho, int alto);

/* --- CÁMARA --- */

//

// --- SISTEMA RTS ---
void seleccionarUnidades(int mouseX, int mouseY, Camera cam);
void dibujarEstablo(HDC hdc, Camera cam);


void dibujarUnidades(HDC hdc, Camera cam);

// Control


/* --- DIBUJADO PRINCIPAL (Partida) --- */
void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera cam, int ancho, int alto, int frameTienda, int mapaId);
void dibujarHUD(HDC hdc, Jugador* jugador, int anchoVentana, int altoVentana);
void dibujarTiendasEnIslas(HDC hdc, Camera cam, int ancho, int alto, int frameTienda);
void dibujarTiendaInteractiva(HDC hdc, Jugador *j, int ancho, int alto);
void dibujarPrecio(HDC hdc, int x, int y, const char* costo1, const char* costo2, BOOL alcanzable);
void dibujarItemRejilla(HDC hdc, HBITMAP icono, int cantidad, int maximo, int x, int y, const char *nombre);
void inicializarNieblaTotal();
void descubrirMapa(float centroX, float centroY, float radio);

// --- LÓGICA DE RECLUTAMIENTO ---
void reclutarTrabajador(Jugador *j, int tipo);
void crearUnidadEnMapa(int tipo);

/* --- DIBUJADO DE MENÚ Y SELECCIÓN DE MAPA --- */
void dibujarMenuConSprites(HDC hdc, HWND hwnd, EstadoJuego *estado);
void dibujarSeleccionMapa(HDC hdc, HWND hwnd, EstadoJuego *estado);

/* --- INTERACCIÓN --- */
void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado);
void procesarEnterMenu(HWND hwnd, EstadoJuego *estado);
void procesarClickMochila(int mouseX, int mouseY, Jugador *jugador, HWND hwnd);
void procesarClickMochilaTienda(int mx, int my, int esClickDerecho, Jugador *j, HWND hwnd);
void procesarClickSeleccionMapa(int x, int y, HWND hwnd, EstadoJuego *estado);
void dibujarInterfazSeleccion(HDC hdc); // La función del cuadro verde

// Inicialización de islas con parámetro de mapa
void inicializarIslas(int mapaId);
// Prototipos de Funciones de Sistema
int buscarArbolCercano(float x, float y, float rango);
void inicializarRecursosEstaticos(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
int buscarMinaCercana(float x, float y, float rango);
void actualizarRegeneracionRecursos();
void crearChispaBlanca(float x, float y);
void actualizarParticulas();





// Al final del archivo, antes del #endif
void dibujarMina(HDC hdc, Camera cam);
extern Unidad unidades[MAX_UNIDADES]; // Indica a otros archivos que el arreglo existe

#endif /* MAPA_H */