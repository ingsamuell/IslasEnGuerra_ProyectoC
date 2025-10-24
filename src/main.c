// src/main.c

// 1. Incluimos raylib y nuestros archivos
#include "raylib.h"
#include "global.h" // (Tu archivo global.h si lo tienes)
#include "mapa/mapa.h"
#include "recursos/recursos.h"
#include "sistema/sistema.h"

// 2. Definimos el tamaño de la ventana
const int screenWidth = 800;
const int screenHeight = 600;

int main(void)
{
    // 3. Inicializar la ventana
    InitWindow(screenWidth, screenHeight, "Islas en Guerra - Versión Gráfica");
    SetTargetFPS(60); // Para que el juego corra a 60 FPS

    // --- Aquí puedes cargar tus estructuras ---
    // char mapa[10][10];
    // inicializarMapa(mapa); // Esta función de mapa.c ya NO debe imprimir nada
    // Recurso recursos[3];
    // ...etc.


    // 4. Bucle principal del juego (se repite 60 veces por segundo)
    while (!WindowShouldClose()) // Se ejecuta hasta que cierres la ventana
    {
        // --- 5. ACTUALIZACIÓN (Lógica) ---
        // Aquí va la lógica de qué pasa cuando presionas una tecla.
        // Por ejemplo:
        if (IsKeyPressed(KEY_M))
        {
            // Llama a una función de tu módulo de mapa
            // explorarMapa(mapa);
        }

        // --- 6. DIBUJADO (Gráficos) ---
        // Todo lo que se dibuja DEBE ir entre BeginDrawing y EndDrawing
        BeginDrawing();

            // Primero, limpia la pantalla con un color de fondo (Agua)
            ClearBackground(BLUE);

            // Ahora, llamas a TUS funciones de dibujado
            // (Tendrás que crear estas funciones en tus módulos)
            
            // dibujarMapa(mapa); // Llama a la función de mapa.c
            // dibujarRecursos(recursos); // Llama a la función de recursos.c

            // Ejemplo de texto:
            DrawText("¡Mi Interfaz Gráfica!", 190, 200, 20, LIGHTGRAY);


        EndDrawing();
    }

    // 7. Cierre
    CloseWindow(); // Cierra la ventana y libera recursos

    return 0;
}