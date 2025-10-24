// En src/mapa/mapa.c
#include "mapa.h"
#include "raylib.h" // ¡Importante incluir raylib aquí también!

/*
 * ESTA FUNCIÓN ES NUEVA.
 * Se llama 60 veces por segundo desde el bucle de main.c
 */
void dibujarMapa(char mapa[10][10])
{
    // Recorremos la matriz
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            // Posición en la pantalla (ej. cada celda es 40x40 píxeles)
            int posX = j * 40;
            int posY = i * 40;

            // Dibujamos según el caracter
            switch(mapa[i][j])
            {
                case 'P': // Jugador
                    DrawRectangle(posX, posY, 40, 40, GREEN);
                    break;
                case 'E': // Enemigo
                    DrawRectangle(posX, posY, 40, 40, RED);
                    break;
                case '$': // Recurso
                    DrawRectangle(posX, posY, 40, 40, YELLOW);
                    break;
                // El 'AGUA' no se dibuja, es el fondo azul
            }
        }
    }
}