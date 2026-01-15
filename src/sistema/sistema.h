#ifndef SISTEMA_H
#define SISTEMA_H

#include "../global.h"

// Estructura para la cabecera del archivo binario
typedef struct {
    char nombrePartida[32];
    char fechaHora[32];
    int nivelJugador;
    int diaJuego;
} EncabezadoGuardado;

// Funciones de Guardado/Carga
int GuardarPartida(int slot, char *nombrePersonalizado, Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
int CargarPartida(int slot, Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
int ExistePartida(int slot);
void ObtenerInfoPartida(int slot, char *bufferDestino);
int BorrarPartida(int slot);

// Registro de Texto (Log)
void RegistrarLog(const char *mensaje);
void AbrirArchivoLog(); // Para abrir el bloc de notas

#endif