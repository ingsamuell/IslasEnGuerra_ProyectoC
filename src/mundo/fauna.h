#ifndef FAUNA_H
#define FAUNA_H

#include <windows.h>
#include "../global.h" 

// Variables Globales
extern Vaca manada[MAX_VACAS];
extern Tiburon misTiburones[MAX_TIBURONES];
// Inicialización
void inicializarVacas();
void inicializarTiburones(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]); // Necesita el mapa para saber dónde es agua
// Lógica / Actualización
void actualizarVacas(); 
void actualizarTiburones(Jugador *j);
void actualizarRegeneracionFauna(); // Para revivir vacas
void golpearVaca(Jugador *j);
int buscarVacaCercana(float x, float y, float rango); // Útil para los cazadores
// Renderizado
void dibujarVacas(HDC hdc, Camera cam, int ancho, int alto);
void dibujarTiburones(HDC hdc, Camera cam, int ancho, int alto);

#endif