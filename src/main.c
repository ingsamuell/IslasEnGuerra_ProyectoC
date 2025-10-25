#include "raylib.h"
#include "mapa/mapa.h" // Incluimos el header del mapa

// Ancho y alto de la ventana
const int screenWidth = 500;
const int screenHeight = 500;

// Declaramos el mapa
char mapaJuego[FILAS][COLUMNAS];

int main(void)
{
    // 1. Inicialización
    InitWindow(screenWidth, screenHeight, "Islas en Guerra - V1.0 Gráfica");
    SetTargetFPS(60);

    // 2. Mostrar menú y decidir qué hacer
    GameScreen opcion = MostrarMenu();

    if (opcion == ISLA)
    {
        // 3. Cargamos los datos del mapa (SOLO UNA VEZ)
        inicializarMapa(mapaJuego);

        // 4. Bucle principal del juego
        while (!WindowShouldClose())
        {
            // --- ACTUALIZACIÓN (Lógica) ---
            // (Aquí irá la lógica de exploración, etc.)

            // --- DIBUJADO (Gráficos) ---
            BeginDrawing();
                ClearBackground(BLACK);
                dibujarMapa(mapaJuego);
                DrawText("MAPA - ISLAS EN GUERRA", 10, 10, 20, WHITE);
            EndDrawing();

            // Salir con ESC
            if (IsKeyPressed(KEY_ESCAPE)) break;
        }
    }

    // 5. Cierre
    CloseWindow();
    return 0;
}

