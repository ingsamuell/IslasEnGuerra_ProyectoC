#ifndef GLOBAL_H
#define GLOBAL_H

#include <windows.h> // Necesario para tipos de datos
#include <stdbool.h> // Para el tipo bool en C

// --- DEFINICIONES GLOBALES ---
#define MUNDO_FILAS 100
#define MUNDO_COLUMNAS 100
#define TAMANO_CELDA_BASE 32     // Tamaño base de celda
#define PANTALLA_FILAS 60        // 600 / 10 = 60
#define PANTALLA_COLUMNAS 80     // 800 / 10 = 80
#define TAMANO_CELDA 32   
#define TILE_SIZE 32

// Definimos direcciones para usar nombres en lugar de números
#define DIR_ABAJO     0
#define DIR_ARRIBA    1
#define DIR_IZQUIERDA 2
#define DIR_DERECHA   3

// --- LIMITES ---
#define MAX_VACAS 8
#define VIDA_VACA 100
#define MAX_ARBOLES 200 // Un buen número para cubrir 5 islas

// --- ESTRUCTURA DEL JUGADOR ---
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
    int experiencia;               // Se incrementa al cazar
    int experienciaSiguienteNivel;
    int nivel;

    // 4. Inventario y Estado
    int inventarioAbierto;
    
    // Materiales
    int madera;
    int piedra;
    int oro;
    int hierro;
    int comida;
    int hojas;
    int carne;            // Cantidad de carne disponible

    // Equipamiento y Tienda
    int tieneArmadura;    // 1 si el objeto existe en el inventario
    int armaduraEquipada; // 1 si la lleva puesta
    int tieneEspada;      // 1 si la compró
    int tienePico;        // 1 si lo compró
    int nivelMochila;     // 1 = Básica, 2 = Herramientas, 3 = Completa
    int modoTienda;       // 0 = COMPRAR, 1 = VENDER

    // Animación
    int direccion;      // 0, 1, 2, 3
    int frameAnim;      // 0=Base, 1=PieIzq, 2=PieDer
    int pasoAnimacion;  // Contador para velocidad de animación
    int frameDestello;  // Para el efecto visual blanco

} Jugador;

typedef struct {
    float x;            
    float y;            
    int direccion;      
    int activa;         
    int vida;           
    float velocidad;    
    int comiendo;       
    int frameAnim;      // Asegúrate de que se llame exactamente así
    int contadorAnim;   // Añade este campo para controlar la velocidad
} Vaca;

// --- ESTRUCTURA DE ÁRBOL ---
typedef struct {
    int x;
    int y;
    int tipo;   // 0 = Árbol Chico, 1 = Árbol Grande
    int activa; // 1 = Existe
} Arbol;

// --- OTRAS ESTRUCTURAS ---
typedef struct {
    int mostrarMenu;
    int enPartida;
    int mostrarResumen; // Inventario
    int opcionSeleccionada;
    POINT puntoMouse;
    int frameTienda;    // Contador para la animación del gato
} EstadoJuego;

typedef struct {
    int x, y;
    float zoom;
    int tamano_celda_actual;
} Camera;

typedef struct {
    int x;          // Posición X en el mundo
    int y;          // Posición Y en el mundo
    int ancho;      // Ancho de la imagen
    int alto;       // Alto de la imagen
    int margen;     // Borde de agua
    int activa;     // 1 = Existe
} Isla;

#endif