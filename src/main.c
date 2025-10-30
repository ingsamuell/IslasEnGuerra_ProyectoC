#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "mapa.h" 

int main() {
    // Configurar consola
    system("title Reino Legendario");
    system("mode con: cols=80 lines=30");
    
    int ejecutando = 1;
    
    while(ejecutando) {
        mostrarMenuPrincipal();
        int seleccion = obtenerSeleccionMenu();
        
        switch(seleccion) {
            case 0: // Partida Nueva
                partidaNueva();
                break;
            case 1: // Partida Anterior
                partidaAnterior();
                break;
            case 2: // Resumen de Recursos
                resumenRecursos();
                break;
        }
    }
    
    return 0;
}