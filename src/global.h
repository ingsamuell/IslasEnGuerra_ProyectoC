#ifndef GLOBAL_H
#define GLOBAL_H

#include <windows.h> // Necesario para tipos de datos
#include <stdbool.h> // Para el tipo bool en C

// --- DEFINICIONES GLOBALES ---
#define MUNDO_FILAS 200      // Aumentado para mayor precisión (antes 100)
#define MUNDO_COLUMNAS 200   // Aumentado para mayor precisión (antes 100)
#define TAMANO_CELDA_BASE 16 // Reducido a 16px (antes 32) para detectar mejor las curvas

// Esto define el tamaño de los sprites en pantalla (visual), no la física.
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
#define MUNDO_ANCHO 3200  // 200 filas * 16 pixeles
#define MUNDO_ALTO 3200   // 200 cols * 16 pixeles

#define ESTABLO_X 1270    // Coordenada X del establo
#define ESTABLO_Y 1420    // Coordenada Y del establo
#define RADIO_ESTABLO 100 // Tamaño del área del establo
#define MAX_VACAS 8
#define VIDA_VACA 100
#define MAX_ARBOLES 30    // Un buen número para cubrir 5 islas

// --- TESOROS Y RECURSOS ---
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

// Definición de la Cueva
#define CUEVA_X 1800
#define CUEVA_Y 1250
#define TIEMPO_RESPAWN_RECURSOS 1800  // 30 segundos si el juego corre a 60 FPS

// Vida inicial de los recursos
#define VIDA_INICIAL_ARBOL 5
#define VIDA_INICIAL_MINA 10
#define MAX_UNIDADES 100   // Aumentamos capacidad

// --- DEFINICIONES DE HERRAMIENTAS ---
#define HERRAMIENTA_NADA   0
#define HERRAMIENTA_ESPADA 1
#define HERRAMIENTA_PICO   2
#define HERRAMIENTA_HACHA  3
#define HERRAMIENTA_CANA   4
// Coordenadas del Muelle (Lado derecho de la Isla Central)
#define MUELLE_X 2050 
#define MUELLE_Y 1600

// --- ESTRUCTURA DE UNIDAD (NPCs) ---
typedef struct {
    float x, y;
    int tipo;           
    int estado;         
    int seleccionado;   
    int activa;         
    
    // Movimiento
    int destinoX, destinoY;
    bool moviendo;  // true si se está desplazando
    
    // Stats y Visual
    int vida;
    int frameAnim;
    int direccion;      
    char nombreGrupo[32]; 
    int timerTrabajo;     // Para la barra de progreso de caza/mina
    int targetIndex;      // Para saber a qué vaca está siguiendo
    int contadorAnim;     // Para la animación de caminar
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
    float x, y;
    float velocidad;
    
    // Estadísticas
    int vidaActual, vidaMax;
    int armaduraActual, armaduraMax;
    int nivel;
    int experiencia, experienciaSiguienteNivel;
    
    // Inventario y Estados
    int nivelMochila;
    int inventarioAbierto; // <--- SOLO UNA VEZ
    int tiendaAbierta;     // <--- ESTA ES LA NUEVA
    int modoTienda;        // 0=Comprar, 1=Vender
    
    // Recursos
    int madera, piedra, oro, hierro;
    int comida, hojas, carne, pescado;
    
    // Herramientas y Equipo
    BOOL tieneEspada;
    BOOL tienePico;
    BOOL tieneHacha;
    BOOL tieneArmadura;
    BOOL tieneCana;
    BOOL tieneBotePesca;
	int cantBarcosGuerra; // <--- CAMBIO: Ahora es un contador (0 a 4)
    
    // Estados de Equipo
    BOOL armaduraEquipada;
    int herramientaActiva; // Usar constantes HERRAMIENTA_NADA, etc.
    int estadoBarco;       // 0=Pie, 1=Bote, 2=Barco
    int timerPesca;

    // Contadores de Unidades (RTS)
    int cantMineros;
    int cantLenadores;
    int cantCazadores;
    int cantSoldados;
    int cantHachas;    // (Si usas esto como contador de items)

    // Animación
    int direccion;
    int frameAnim;
    int pasoAnimacion;
    int frameDestello;

} Jugador;

// --- OTRAS ESTRUCTURAS ---

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
    int vida;            // 5 golpes
    int estadoVida;      // 0 = Viva, 1 = Muerta
    int tiempoMuerte;    // Contador para desaparecer (5 segundos)
    int timerRegeneracion; // Contador de tiempo para reaparecer
} Vaca;

typedef struct {
    int x;
    int y;
    int tipo;   // 0 = Árbol Chico, 1 = Árbol Grande
    int activa; // 1 = Existe
    int vida;   // Vida del árbol
    int timerRegeneracion; // Contador de tiempo para reaparecer
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
    int timerRegeneracion; 
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