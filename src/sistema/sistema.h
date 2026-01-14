#ifndef SISTEMA_H
#define SISTEMA_H

#include "../global.h"

// Estructura que va al inicio del archivo para identificar la partida
typedef struct {
    char nombrePartida[64]; // Lo que escribe el usuario
    char fechaHora[64];     // Automático: "13/01/2026 15:30"
    int nivelJugador;       // Para mostrar en el menú
    int diaJuego;           // (Opcional) Días sobrevividos
} EncabezadoGuardado;

// Funciones principales
void InicializarSistemaGuardado();
int GuardarPartida(char *nombrePersonalizado, Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]);
int CargarPartida(Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]); // Retorna 1 si éxito
int ExistePartidaGuardada();
void ObtenerInfoPartida(char *bufferDestino); // Llena un string con "Nombre - Nivel X - Fecha"

#endif