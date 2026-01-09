#ifndef NATURALEZA_H
#define NATURALEZA_H

#include <windows.h>
#include "../global.h"

// Variables Globales (Para acceso desde IA u otros módulos)
extern Arbol misArboles[MAX_ARBOLES];
extern Mina misMinas[MAX_MINAS];
extern Tesoro misTesoros[MAX_TESOROS];

// Inicialización
void inicializarArboles(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void inicializarMinas(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void inicializarTesoros();

// Interacción (Acciones del jugador)
void talarArbol(Jugador *j);
void picarMina(Jugador *j);
void abrirTesoro(Jugador *j);

// Lógica y Regeneración
void actualizarRegeneracionNaturaleza(); // Respawn de árboles y minas
int buscarArbolCercano(float x, float y, float rango); // Para la IA (Leñadores)
int buscarMinaCercana(float x, float y, float rango);  // Para la IA (Mineros)

// Renderizado
void dibujarArboles(HDC hdc, Camera cam, int ancho, int alto, int mapaId);
void dibujarMinas(HDC hdc, Camera cam, int ancho, int alto);
void dibujarTesoros(HDC hdc, Camera cam, int ancho, int alto);

#endif