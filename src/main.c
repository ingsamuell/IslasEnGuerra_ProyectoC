#include "raylib.h"
#include "mapa/mapa.h" // Incluimos el header del mapa

// Ancho y alto de la ventana
// (10 celdas * 50 píxeles por celda = 500)
const int screenWidth = 500; 
const int screenHeight = 500;

// Declaramos el mapa
char mapaJuego[FILAS][COLUMNAS];

int main(void)
{
    // 1. Inicialización
    InitWindow(screenWidth, screenHeight, "Islas en Guerra - V1.0 Gráfica");
    SetTargetFPS(60);

    // 2. Cargamos los datos del mapa (SOLO UNA VEZ)
    inicializarMapa(mapaJuego); // Llamamos a la función de mapa.c

    // 3. Bucle principal del juego
    while (!WindowShouldClose())
    {
        // --- 4. ACTUALIZACIÓN (Lógica) ---
        // (Aquí irá la lógica de exploración, etc.)


        // --- 5. DIBUJADO (Gráficos) ---
        BeginDrawing();

            // Limpiamos el fondo (aunque dibujarMapa lo cubre todo)
            ClearBackground(BLACK);

            // ¡Llamamos a nuestra función para dibujar el mapa!
            dibujarMapa(mapaJuego);
            
            // (Opcional) Puedes añadir texto encima
            DrawText("MAPA - ISLAS EN GUERRA", 10, 10, 20, WHITE);

        EndDrawing();
    }

    // 6. Cierre
    CloseWindow();

    return 0;
}