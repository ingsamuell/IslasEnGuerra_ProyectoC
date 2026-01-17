#ifndef MAPA_H
#define MAPA_H

#include <windows.h>
#include <wingdi.h>
#include <stdbool.h>
#include "../global.h"


  // 1. VARIABLES EXTERNAS Y ESTRUCTURAS
extern char mapa[MUNDO_FILAS][MUNDO_COLUMNAS];
extern Unidad unidades[MAX_UNIDADES]; 

  // 2. CONFIGURACIÓN INICIAL Y CICLO DE VIDA
void inicializarMapa(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int mapaId);
void inicializarJuego(Jugador *jugador, EstadoJuego *estado, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int mapaId);
void inicializarIslas(int mapaId);
void inicializarRecursosEstaticos(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);

   //3. FÍSICA Y COLISIONES
int EsSuelo(int x, int y, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
int VerificarColisionCaja(int x, int y, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);

   //4. SISTEMA DE RECURSOS (NATURALEZA, MINAS Y TESOROS)
// Árboles
void inicializarArboles(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void dibujarArboles(HDC hdc, Camera cam, int ancho, int alto, int mapaId);
void talarArbol(Jugador *j);
int buscarArbolCercano(float x, float y, float rango);

// Minas
void inicializarMinas(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void dibujarMinas(HDC hdc, Camera cam, int ancho, int alto);
void dibujarMina(HDC hdc, Camera cam); // Específica de renderizado
void picarMina(Jugador *j);
int buscarMinaCercana(float x, float y, float rango);

// Tesoros y Regeneración
void inicializarTesoros();
void abrirTesoro(Jugador *j);
void dibujarTesoros(HDC hdc, Camera cam, int ancho, int alto);
void actualizarRegeneracionRecursos();

   //5. SISTEMA DE PARTÍCULAS Y EFECTOS VISUALES
void crearChispas(int x, int y, COLORREF color);
void crearChispaBlanca(float x, float y);
void crearSangre(float x, float y);
void crearExplosionAgua(float x, float y);
void crearProyectilMagico(float x, float y, float destinoX, float destinoY);
void crearBalaCanon(float x, float y, float destX, float destY);
void actualizarParticulas();
void actualizarYDibujarParticulas(HDC hdc, Camera cam);

  // 6. INTERFAZ DE USUARIO (HUD, MENÚS Y TEXTO)
// Menú Principal y Selección
void dibujarMenuConSprites(HDC hdc, HWND hwnd, EstadoJuego *estado);
void dibujarSeleccionMapa(HDC hdc, HWND hwnd, EstadoJuego *estado);
void dibujarInterfazSeleccion(HDC hdc);

// HUD e Indicadores en Partida
void dibujarHUD(HDC hdc, Jugador* jugador, int anchoVentana, int altoVentana);
void dibujarBarraVidaLocal(HDC hdc, int x, int y, int vidaActual, int vidaMax, int anchoTotal);
void dibujarMiniMapa(HDC hdc, Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int anchoPantalla, int altoPantalla);

// Textos Flotantes
void agregarTextoFlotante(int x, int y, char* contenido, COLORREF color);
void crearTextoFlotante(int x, int y, const char* formato, int cantidad, COLORREF color);
void actualizarYDibujarTextos(HDC hdc, Camera cam);

   //7. ECONOMÍA: TIENDA Y MOCHILA
void dibujarTiendasEnIslas(HDC hdc, Camera cam, int ancho, int alto, int frameTienda);
void dibujarTiendaInteractiva(HDC hdc, Jugador *j, int ancho, int alto);
void dibujarPrecio(HDC hdc, int x, int y, const char* costo1, const char* costo2, BOOL alcanzable);
void dibujarItemRejilla(HDC hdc, HBITMAP icono, int cantidad, int maximo, int x, int y, const char *nombre);

   //8. SISTEMA RTS (UNIDADES Y NIEBLA)
void seleccionarUnidades(int mouseX, int mouseY, Camera cam);
void dibujarUnidades(HDC hdc, Camera cam);
void dibujarEstablo(HDC hdc, Camera cam);
void reclutarTrabajador(Jugador *j, int tipo);
void crearUnidadEnMapa(int tipo);

// Niebla de Guerra
void inicializarNieblaTotal();
void descubrirMapa(float centroX, float centroY, float radio);

   //9. RENDERIZADO PRINCIPAL Y LÓGICA DE SISTEMA
void dibujarMapaConZoom(HDC hdc, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera cam, int ancho, int alto, int frameTienda, int mapaId);
void actualizarLogicaSistema(Jugador *j);

   //10. PROCESAMIENTO DE ENTRADA (CLICKS)
void procesarClickMenu(int x, int y, HWND hwnd, EstadoJuego *estado);
void procesarEnterMenu(HWND hwnd, EstadoJuego *estado);
void procesarClickSeleccionMapa(int x, int y, HWND hwnd, EstadoJuego *estado);
void procesarClickMochila(int mouseX, int mouseY, Jugador *jugador, HWND hwnd);
void procesarClickMochilaTienda(int mx, int my, int esClickDerecho, Jugador *j, HWND hwnd);

void dibujarBarcoAnclado(HDC hdc, Camera cam);

#endif /* MAPA_H */