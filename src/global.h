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

// --- ESTADOS DEL JUEGO ---
#define ESTADO_MENU 0
#define ESTADO_SELECCION_MAPA 1
#define ESTADO_PARTIDA 2

// --- MAPAS DISPONIBLES ---
#define MAPA_ISLAS_OCEANO 0
#define MAPA_ARCHIPIELAGO 1
#define MAPA_CONTINENTE 2
#define TOTAL_MAPAS 3

// --- CONSTANTES DE ISLAS ---
#define MAX_ISLAS 5

// Definimos direcciones para usar nombres en lugar de números
#define DIR_ABAJO     0
#define DIR_ARRIBA    1
#define DIR_IZQUIERDA 2
#define DIR_DERECHA   3

// --- LIMITES ---
#define MUNDO_ANCHO 2000  // Ajusta este valor al tamaño real de tu mapa
#define MUNDO_ALTO 2000   // Ajusta este valor al tamaño real de tu mapa
// En global.h o mapa.c
#define ESTABLO_X 1350 // Coordenada X del establo
#define ESTABLO_Y 1420   // Coordenada Y del establo
#define RADIO_ESTABLO 100 // Tamaño del área del establo
#define MAX_VACAS 8
#define VIDA_VACA 100
#define MAX_ARBOLES 30// Un buen número para cubrir 5 islas
// --- TESOROS ---
#define MAX_TESOROS 2
#define MAX_MINAS 20
#define MAX_PARTICULAS 50
#define MAX_TEXTOS 20

// --- TIPOS DE UNIDADES ---
#define TIPO_ALDEANO 0
#define TIPO_CAZADOR 1    // Espada
#define TIPO_LENADOR 2    // Hacha
#define TIPO_MINERO  3    // Pico
#define TIPO_SOLDADO 4    // Armadura

// Estados de la unidad
#define ESTADO_IDLE      0
#define ESTADO_MOVIENDO  1
#define ESTADO_CAZANDO   2
#define ESTADO_TALANDO   3
#define ESTADO_MINANDO   4
#define ESTADO_EN_CUEVA  5


// Definición de la Cueva (Ubicación fija por ahora)
#define CUEVA_X 1800
#define CUEVA_Y 1250
#ifndef TILE_ARBOL
#define TILE_ARBOL 'A'
#endif

#ifndef TILE_MINA
#define TILE_MINA 'M'
#endif

#define MAX_UNIDADES 100   // Aumentamos capacidad


typedef struct {
    float x, y;
    int tipo;           
    int estado;         
    int seleccionado;   
    int activa;         
    
    // Movimiento
    int destinoX, destinoY;
    
    // Stats y Visual
    int vida;
    int frameAnim;
    int direccion;      
    char nombreGrupo[32]; 
    int timerTrabajo;     // Para la barra de progreso de caza/mina
	int targetIndex;      // Para saber a qué vaca está siguiendoo
    int contadorAnim;     // <--- AGREGADO: Para la animación de caminar

} Unidad;

// --- ESTRUCTURA DE ISLA ---
typedef struct {
    int x;          // Posición X en el mundo
    int y;          // Posición Y en el mundo
    int ancho;      // Ancho de la imagen
    int alto;       // Alto de la imagen
    int margen;     // Borde de agua
    int activa;     // 1 = Existe
} Isla;

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
    
    // Contadores de oficios/unidades
    int cantMineros;
    int cantLenadores;
    int cantCazadores;
    int cantSoldados;
    int cantHachas;
    
    // Flags para saber si ya compró al menos uno
    BOOL tieneHacha;
    // Animación
    int direccion;      // 0, 1, 2, 3
    int frameAnim;      // 0=Base, 1=PieIzq, 2=PieDer
    int pasoAnimacion;  // Contador para velocidad de animación
    int frameDestello;  // Para el efecto visual blanco

} Jugador;

typedef struct {
    float x;
    float y;
    float xInicial;      // Para el límite de 100px
    int direccion;       // -1: Izquierda, 1: Derecha
    int estado;          // 0: Caminando, 1: Quieta
    int contadorEspera;  
    int activa;
    int frameAnim;       // 0 a 3 (el offset se suma al dibujar)
    int contadorAnim;
    int vida;           // 5 golpes
    int estadoVida;         // 0 = Viva, 1 = Muerta
    int tiempoMuerte;   // Contador para desaparecer (5 segundos)
} Vaca;

// --- ESTRUCTURA DE ÁRBOL ---
typedef struct {
    int x;
    int y;
    int tipo;   // 0 = Árbol Chico, 1 = Árbol Grande
    int activa; // 1 = Existe
    int vida;   // <--- NUEVO: Vida del árbol (5 golpes)
} Arbol;
typedef struct {
    float x, y;
    char texto[32];
    int vida;      // Duración del texto (ej. 40 frames)
    COLORREF color;
    int activo;
} TextoFlotante;

typedef struct {
    float x, y;
    int tipo;   // 0 para Piedra, 1 para Hierro
    int vida;   // 5 golpes para recolectar
    int activa;
} Mina;
typedef struct {
    float x, y;
    float vx, vy; // Velocidad
    int vida;     // Duración del efecto
    COLORREF color;
    int activo;
} Particula;

typedef struct {
    int x;
    int y;
    int tipo;   // 0 = Solo Oro, 1 = Oro + Hierro (Joyas)
    int estado; // 0 = Cerrado, 1 = Abierto/Vacío
    int activa; // 1 = Existe
} Tesoro;

// --- ESTRUCTURA DE ESTADO DEL JUEGO ---
typedef struct {
    int estadoActual;      // 0=Menu, 1=SeleccionMapa, 2=Partida
    int opcionSeleccionada;
    int mapaSeleccionado;  // 0, 1 o 2
    int frameTienda;       // Contador para la animación del gato
    POINT puntoMouse;
} EstadoJuego;

typedef struct {
    int x, y;
    float zoom;
    int tamano_celda_actual;
} Camera;

#endif /* GLOBAL_H */