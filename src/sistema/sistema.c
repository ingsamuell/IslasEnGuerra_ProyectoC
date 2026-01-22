#include "sistema.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <windows.h> // Para ShellExecute
#include "../mundo/naturaleza.h" 
#include "../mundo/edificios.h"  

// Variables externas a guardar
extern Arbol misArboles[MAX_ARBOLES];
extern Mina misMinas[MAX_MINAS];
extern Tesoro misTesoros[MAX_TESOROS];
extern Edificio misEdificios[MAX_EDIFICIOS_JUGADOR];

// Ayuda para generar nombres de archivo: "partida_1.bin", "partida_2.bin"
void obtenerNombreArchivo(int slot, char *buffer) {
    sprintf(buffer, "partida_%d.bin", slot);
}

void obtenerFechaHora(char *buffer) { 
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(buffer, "%02d/%02d/%d %02d:%02d", 
        tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, 
        tm->tm_hour, tm->tm_min);
}

// --- LOG DE TEXTO (.txt) ---
void RegistrarLog(const char *mensaje) {
    FILE *f = fopen("registro_acciones.txt", "a"); // "a" = Append 
    if (f) {
        char fecha[32];
        obtenerFechaHora(fecha);
        fprintf(f, "[%s] %s\n", fecha, mensaje);
        fclose(f);
    }
}

void AbrirArchivoLog() {
    // Abre el archivo de texto con el programa predeterminado (Bloc de notas)
    ShellExecute(NULL, "open", "registro_acciones.txt", NULL, NULL, SW_SHOW);
}

// --- GUARDADO BINARIO ---
int GuardarPartida(int slot, char *nombrePersonalizado, Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    char nombreArchivo[32];
    obtenerNombreArchivo(slot, nombreArchivo);

    FILE *f = fopen(nombreArchivo, "wb");
    if (!f) return 0;

    EncabezadoGuardado head;
    strncpy(head.nombrePartida, nombrePersonalizado, 31);
    obtenerFechaHora(head.fechaHora);
    head.nivelJugador = j->nivel;
    head.diaJuego = 1; 

    fwrite(&head, sizeof(EncabezadoGuardado), 1, f);
    fwrite(j, sizeof(Jugador), 1, f);
    fwrite(mapa, sizeof(char), MUNDO_FILAS * MUNDO_COLUMNAS, f);
    
    fwrite(misArboles, sizeof(Arbol), MAX_ARBOLES, f);
    fwrite(misMinas, sizeof(Mina), MAX_MINAS, f);
    fwrite(misTesoros, sizeof(Tesoro), MAX_TESOROS, f);
    fwrite(misEdificios, sizeof(Edificio), MAX_EDIFICIOS_JUGADOR, f);

    fclose(f);
    
    // Registrar en el LOG que se guardó
    char msg[100];
    sprintf(msg, "Partida guardada en Slot %d (Nivel %d)", slot, j->nivel);
    RegistrarLog(msg);
    
    return 1;
}

int CargarPartida(int slot, Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    char nombreArchivo[32];
    obtenerNombreArchivo(slot, nombreArchivo);

    FILE *f = fopen(nombreArchivo, "rb");
    if (!f) return 0;

    EncabezadoGuardado head;
    fread(&head, sizeof(EncabezadoGuardado), 1, f);
    fread(j, sizeof(Jugador), 1, f);
    fread(mapa, sizeof(char), MUNDO_FILAS * MUNDO_COLUMNAS, f);
    
    fread(misArboles, sizeof(Arbol), MAX_ARBOLES, f);
    fread(misMinas, sizeof(Mina), MAX_MINAS, f);
    fread(misTesoros, sizeof(Tesoro), MAX_TESOROS, f);
    fread(misEdificios, sizeof(Edificio), MAX_EDIFICIOS_JUGADOR, f);
    fclose(f);

    char msg[100];
    sprintf(msg, "Partida cargada desde Slot %d", slot);
    RegistrarLog(msg);

    return 1;
}

int ExistePartida(int slot) {
    char nombreArchivo[32];
    obtenerNombreArchivo(slot, nombreArchivo);
    FILE *f = fopen(nombreArchivo, "rb");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

int BorrarPartida(int slot) {
    char nombreArchivo[32];
    obtenerNombreArchivo(slot, nombreArchivo);

    // remove() devuelve 0 si tuvo éxito
    if (remove(nombreArchivo) == 0) {
        char msg[100];
        sprintf(msg, "ELIMINADO: Partida del Slot %d borrada por el usuario.", slot);
        RegistrarLog(msg);
        return 1; // Éxito
    }
    
    RegistrarLog("ERROR: Intento fallido de borrar partida.");
    return 0; 
}
void ObtenerInfoPartida(int slot, char *bufferDestino) {
    char nombreArchivo[32];
    obtenerNombreArchivo(slot, nombreArchivo);

    FILE *f = fopen(nombreArchivo, "rb");
    if (!f) {
        sprintf(bufferDestino, "[VACIO] Slot %d", slot);
        return;
    }
    EncabezadoGuardado head;
    fread(&head, sizeof(EncabezadoGuardado), 1, f);
    fclose(f);

    sprintf(bufferDestino, "Slot %d: %s (Nvl %d) - %s", slot, head.nombrePartida, head.nivelJugador, head.fechaHora);
}