#ifndef FAUNA_H
#define FAUNA_H

#include <windows.h>
#include "../global.h" // Necesario para las structs Jugador, Vaca, Tiburon

// Variables Globales (Para que otros archivos puedan acceder si es necesario)
extern Vaca manada[MAX_VACAS];
extern Tiburon misTiburones[MAX_TIBURONES];

// Inicialización
void inicializarVacas();
void inicializarTiburones(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]); // Necesita el mapa para saber dónde es agua

// Lógica / Actualización
void actualizarVacas(); // Simplificado: ya no necesita el mapa, solo patrullan
void actualizarTiburones(Jugador *j);
void actualizarRegeneracionFauna(); // Para revivir vacas
void golpearVaca(Jugador *j);
int buscarVacaCercana(float x, float y, float rango); // Útil para la IA (Cazadores)

// Renderizado
void dibujarVacas(HDC hdc, Camera cam, int ancho, int alto);
void dibujarTiburones(HDC hdc, Camera cam, int ancho, int alto);

#endif