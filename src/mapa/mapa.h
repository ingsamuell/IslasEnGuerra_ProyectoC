#ifndef MAPA_H
#define MAPA_H

#include <windows.h>

// Estructuras
typedef struct {
    int oro;
    int madera;
    int piedra;
    int comida;
} Recursos;

typedef struct {
    char nombre[50];
    int nivel;
    Recursos recursos;
    int dia;
} Partida;

// Colores para el menú
#define COLOR_TITULO 14     // Amarillo
#define COLOR_MENU 11       // Cian
#define COLOR_SELECCION 10  // Verde
#define COLOR_NORMAL 7      // Blanco
#define COLOR_RECURSOS 13   // Magenta

// Funciones del menú
void mostrarMenuPrincipal();
void partidaNueva();
void partidaAnterior();
void resumenRecursos();
void dibujarMarco();
void mostrarTextoCentrado(int y, const char* texto, int color);
int obtenerSeleccionMenu();

#endif