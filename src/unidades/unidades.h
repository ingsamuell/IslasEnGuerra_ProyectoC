#ifndef UNIDADES_H
#define UNIDADES_H

#include <windows.h>
#include "../global.h"

// Variables Globales
extern Unidad unidades[MAX_UNIDADES];

// Inicialización y Creación
void inicializarUnidades();
void spawnearEscuadron(int tipo, int cantidad, int x, int y);

// Lógica y Actualización
void aplicarSeparacion(int id, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
void actualizarUnidades(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Jugador *j);
void actualizarAnimacionUnidad(Unidad *u, float dx, float dy);

// Control (Mouse/Teclado)
void seleccionarUnidadesGrupo(int x1, int y1, int x2, int y2, Camera cam);
void darOrdenMovimiento(Unidad unidades[], int max, int clickX, int clickY); // Formación
void ordenarUnidad(int mX, int mY, Camera cam); // Lógica inteligente (Cazar/Talar/Mover)

// Renderizado
void dibujarUnidades(HDC hdc, Camera cam);

#endif