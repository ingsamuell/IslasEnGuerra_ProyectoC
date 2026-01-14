#include "sistema.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "../mundo/naturaleza.h" // Para acceder a misArboles, misMinas
#include "../mundo/edificios.h"  // Para misEdificios

#define ARCHIVO_GUARDADO "partida.bin"

// Variables externas que necesitamos guardar
extern Arbol misArboles[MAX_ARBOLES];
extern Mina misMinas[MAX_MINAS];
extern Tesoro misTesoros[MAX_TESOROS];
extern Edificio misEdificios[MAX_EDIFICIOS_JUGADOR];
// Si tienes enemigos o neblina, agrégalos aquí también como extern

void obtenerFechaHora(char *buffer) { 
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(buffer, "%02d/%02d/%d %02d:%02d", 
        tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, 
        tm->tm_hour, tm->tm_min);
}

int GuardarPartida(char *nombrePersonalizado, Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    FILE *f = fopen(ARCHIVO_GUARDADO, "wb"); // 'wb' = Write Binary
    if (!f) return 0;

    // 1. PREPARAR ENCABEZADO
    EncabezadoGuardado head;
    strcpy(head.nombrePartida, nombrePersonalizado);
    obtenerFechaHora(head.fechaHora);
    head.nivelJugador = j->nivel;
    head.diaJuego = 1; // Si tienes contador de días, ponlo aquí

    // 2. ESCRIBIR DATOS (El orden es CRUCIAL)
    fwrite(&head, sizeof(EncabezadoGuardado), 1, f);
    fwrite(j, sizeof(Jugador), 1, f);
    fwrite(mapa, sizeof(char), MUNDO_FILAS * MUNDO_COLUMNAS, f); // El mapa entero
    
    // Guardar Entidades
    fwrite(misArboles, sizeof(Arbol), MAX_ARBOLES, f);
    fwrite(misMinas, sizeof(Mina), MAX_MINAS, f);
    fwrite(misTesoros, sizeof(Tesoro), MAX_TESOROS, f);
    fwrite(misEdificios, sizeof(Edificio), MAX_EDIFICIOS_JUGADOR, f);

    // Guardar Neblina (Importante: Si la tienes en main.c, pásala como argumento o usa extern)
    // fwrite(neblina, sizeof(int), MUNDO_FILAS * MUNDO_COLUMNAS, f);

    fclose(f);
    return 1;
}

int CargarPartida(Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    FILE *f = fopen(ARCHIVO_GUARDADO, "rb"); // 'rb' = Read Binary
    if (!f) return 0;

    EncabezadoGuardado head;
    
    // LEER EN EL MISMO ORDEN QUE ESCRIBIMOS
    fread(&head, sizeof(EncabezadoGuardado), 1, f);
    fread(j, sizeof(Jugador), 1, f);
    fread(mapa, sizeof(char), MUNDO_FILAS * MUNDO_COLUMNAS, f);
    
    fread(misArboles, sizeof(Arbol), MAX_ARBOLES, f);
    fread(misMinas, sizeof(Mina), MAX_MINAS, f);
    fread(misTesoros, sizeof(Tesoro), MAX_TESOROS, f);
    fread(misEdificios, sizeof(Edificio), MAX_EDIFICIOS_JUGADOR, f);
    
    fclose(f);
    return 1;
}

int ExistePartidaGuardada() {
    FILE *f = fopen(ARCHIVO_GUARDADO, "rb");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

void ObtenerInfoPartida(char *bufferDestino) {
    FILE *f = fopen(ARCHIVO_GUARDADO, "rb");
    if (!f) {
        strcpy(bufferDestino, "Vacío");
        return;
    }
    EncabezadoGuardado head;
    fread(&head, sizeof(EncabezadoGuardado), 1, f);
    fclose(f);

    sprintf(bufferDestino, "%s (Nvl %d) - %s", head.nombrePartida, head.nivelJugador, head.fechaHora);
}