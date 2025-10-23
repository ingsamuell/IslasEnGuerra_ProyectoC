#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <conio.h>  // Uso esta librería para capturar teclas como las flechas

// Defino el tamaño del mapa: 25 filas y 60 columnas
#define FILAS 25
#define COLUMNAS 60

// Creo una estructura para guardar la posición del jugador
typedef struct {
    int x;
    int y;
} Jugador;

// Esta función inicializa el mapa y coloca los elementos del juego
void inicializarMapa(char mapa[FILAS][COLUMNAS], Jugador *j) {
    // Centro de la isla y radios para formar una elipse horizontal
    int centroX = FILAS / 2;
    int centroY = COLUMNAS / 2;
    int radioVertical = 6;
    int radioHorizontal = 20;

    // Recorro toda la matriz para colocar agua y bordes
    for (int i = 0; i < FILAS; i++) {
        for (int k = 0; k < COLUMNAS; k++) {
            if (i == 0 || i == FILAS - 1) {
                mapa[i][k] = '-'; // Borde superior e inferior
            } else if (k == 0 || k == COLUMNAS - 1) {
                mapa[i][k] = '|'; // Borde lateral
            } else {
                // Uso la fórmula de elipse para formar la isla horizontal
                double valor = pow(i - centroX, 2) / pow(radioVertical, 2) +
                               pow(k - centroY, 2) / pow(radioHorizontal, 2);
                if (valor < 1.0 + (rand() % 3 - 1) * 0.1) {
                    mapa[i][k] = '.'; // Tierra
                } else {
                    mapa[i][k] = '~'; // Agua
                }
            }
        }
    }

    // Coloco la base del jugador y guardo su posición inicial
    mapa[centroX][centroY - 15] = 'P';
    j->x = centroX;
    j->y = centroY - 15;

    // Coloco oro en el centro de la isla
    mapa[centroX][centroY] = '$';
    mapa[centroX + 1][centroY + 1] = '$';
    mapa[centroX - 1][centroY - 1] = '$';

    // Coloco enemigos en el extremo derecho
    mapa[centroX][centroY + 18] = 'E';
    mapa[centroX - 1][centroY + 17] = 'E';

    // Coloco una montaña y un templo
    mapa[centroX - 4][centroY] = 'A';
    mapa[centroX + 4][centroY] = 'T';
}

// Esta función muestra el mapa en consola con colores
void mostrarMapa(char mapa[FILAS][COLUMNAS], Jugador j) {
    for (int i = 0; i < FILAS; i++) {
        for (int k = 0; k < COLUMNAS; k++) {
            // Si la celda es la posición del jugador, muestro '@'
            if (i == j.x && k == j.y) {
                printf("\033[1;37m@\033[0m");
            } else {
                // Muestro cada símbolo con su color correspondiente
                switch (mapa[i][k]) {
                    case '~': printf("\033[1;34m~\033[0m"); break;
                    case '.': printf("\033[1;32m.\033[0m"); break;
                    case 'P': printf("\033[1;35mP\033[0m"); break;
                    case '$': printf("\033[1;33m$\033[0m"); break;
                    case 'E': printf("\033[1;31mE\033[0m"); break;
                    case 'A': printf("\033[1;37mA\033[0m"); break;
                    case 'T': printf("\033[1;36mT\033[0m"); break;
                    case '|': case '-': printf("%c", mapa[i][k]); break;
                    default: printf("%c", mapa[i][k]); break;
                }
            }
        }
        printf("\n");
    }
}

// Esta función mueve al jugador según la flecha que presione
void moverJugador(Jugador *j, char mapa[FILAS][COLUMNAS], int tecla) {
    int nuevoX = j->x;
    int nuevoY = j->y;

    // Asigno la nueva posición según la flecha
    if (tecla == 72) nuevoX--;      // Flecha arriba
    else if (tecla == 80) nuevoX++; // Flecha abajo
    else if (tecla == 75) nuevoY--; // Flecha izquierda
    else if (tecla == 77) nuevoY++; // Flecha derecha

    // Verifico que no se salga del mapa ni se meta al agua
    if (nuevoX > 0 && nuevoX < FILAS - 1 &&
        nuevoY > 0 && nuevoY < COLUMNAS - 1 &&
        mapa[nuevoX][nuevoY] != '~') {
        j->x = nuevoX;
        j->y = nuevoY;
    }
}

// Esta función permite explorar la celda actual del jugador
void explorarCelda(Jugador j, char mapa[FILAS][COLUMNAS]) {
    char celda = mapa[j.x][j.y];

    // Muestro un mensaje según el contenido de la celda
    switch (celda) {
        case '$': printf("¡Encontraste oro!\n"); break;
        case 'E': printf("¡Enemigo detectado! Prepara combate.\n"); break;
        case 'T': printf("Has descubierto un templo antiguo.\n"); break;
        case 'A': printf("Montaña intransitable.\n"); break;
        case '.': printf("Tierra firme.\n"); break;
        case 'P': printf("Estás en tu base.\n"); break;
        default: printf("Nada interesante aquí.\n"); break;
    }

    // Pausa para que el jugador lea el mensaje
    printf("Presiona cualquier tecla para continuar...");
    getch();
}

// Esta es la función principal del juego
int main() {
    srand(time(NULL)); // Inicializo la semilla para los números aleatorios

    char mapa[FILAS][COLUMNAS];
    Jugador jugador;

    // Llamo a la función para crear el mapa y colocar al jugador
    inicializarMapa(mapa, &jugador);

    int tecla;
    while (1) {
        system("cls"); // Limpio la consola (en Windows)

        // Muestro el mapa actualizado
        mostrarMapa(mapa, jugador);

        // Instrucciones para el jugador
        printf("\nUsa flechas para moverte | [E] Explorar | [Q] Salir\n");

        tecla = getch(); // Capturo la tecla presionada

        if (tecla == 'q' || tecla == 'Q') break; // Salir del juego
        else if (tecla == 'e' || tecla == 'E') explorarCelda(jugador, mapa); // Explorar celda
        else if (tecla == 0 || tecla == 224) {
            tecla = getch(); // Capturo el código de la flecha
            moverJugador(&jugador, mapa, tecla); // Muevo al jugador
        }
    }

    return 0;
}