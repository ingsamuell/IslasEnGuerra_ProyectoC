#ifndef GLOBAL_H
#define GLOBAL_H

#include <windows.h> // Necesario para tipos de datos

// --- DEFINICIONES GLOBALES ---
#define MUNDO_FILAS 100
#define MUNDO_COLUMNAS 100
#define TAMANO_CELDA_BASE 32     // Tamaño base de celda para zoom
#define PANTALLA_FILAS 60        // 600 / 10 = 60
#define PANTALLA_COLUMNAS 80     // 800 / 10 = 80
#define TAMANO_CELDA 32   

// --- ESTRUCTURA DEL JUGADOR (DEFINIDA SOLO AQUÍ) ---
typedef struct {
    // 1. Posición
    int x;
    int y;
    int velocidad;

    // 2. Estadísticas de Combate
    int vidaActual;
    int vidaMax;
    int armaduraActual;
    int armaduraMax;

    // 3. Progreso
    int experiencia;
    int experienciaSiguienteNivel;
    int nivel;

    // 4. Inventario
    int inventarioAbierto;
    
    // Materiales
    int madera;
    int piedra;
    int oro;
    int hierro;
    int comida;
    int hojas;

} Jugador;

// Otras estructuras globales si las tienes (como EstadoJuego)
typedef struct {
    int mostrarMenu;
    int enPartida;
    int mostrarResumen; // Inventario
    int opcionSeleccionada;

    POINT puntoMouse;
} EstadoJuego;

typedef struct {
    int x, y;
    float zoom;
    int tamano_celda_actual;
} Camera;

typedef struct {
    int x;          // Posición X en el mundo (esquina superior izquierda de la imagen)
    int y;          // Posición Y en el mundo
    int ancho;      // Ancho de la imagen
    int alto;       // Alto de la imagen
    int margen;     // Cuántos píxeles de "borde de agua" tiene la imagen (para la colisión)
    int activa;     // 1 = Existe, 0 = No existe (útil para islas que aparecen/desaparecen)
} Isla;

#endif