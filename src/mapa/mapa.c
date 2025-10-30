#include "mapa.h"
#include <stdio.h>
#include <string.h>
#include <conio.h>

// Variables globales
Partida partidaActual;
int partidaCargada = 0;

void dibujarMarco() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int ancho = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int alto = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    
    SetConsoleTextAttribute(hConsole, COLOR_MENU);
    
    // Esquinas y bordes
    printf("\n");
    for(int i = 0; i < ancho; i++) {
        printf("=");
    }
    
    for(int i = 2; i < alto - 2; i++) {
        COORD pos = {0, i};
        SetConsoleCursorPosition(hConsole, pos);
        printf("||");
        pos.X = ancho - 2;
        SetConsoleCursorPosition(hConsole, pos);
        printf("||");
    }
    
    COORD pos = {0, alto - 3};
    SetConsoleCursorPosition(hConsole, pos);
    for(int i = 0; i < ancho; i++) {
        printf("=");
    }
}

void mostrarTextoCentrado(int y, const char* texto, int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int ancho = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    
    int x = (ancho - strlen(texto)) / 2;
    COORD pos = {x, y};
    
    SetConsoleCursorPosition(hConsole, pos);
    SetConsoleTextAttribute(hConsole, color);
    printf("%s", texto);
}

void mostrarMenuPrincipal() {
    system("cls"); // Limpiar pantalla
    
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int alto = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    
    // Dibujar marco decorativo
    dibujarMarco();
    
    // Título del juego
    mostrarTextoCentrado(5, "╔══════════════════════╗", COLOR_TITULO);
    mostrarTextoCentrado(6, "║    REINO LEGENDARIO  ║", COLOR_TITULO);
    mostrarTextoCentrado(7, "╚══════════════════════╝", COLOR_TITULO);
    
    // Opciones del menú
    mostrarTextoCentrado(12, "► PARTIDA NUEVA", COLOR_MENU);
    mostrarTextoCentrado(14, "  PARTIDA ANTERIOR", COLOR_NORMAL);
    mostrarTextoCentrado(16, "  RESUMEN DE RECURSOS", COLOR_NORMAL);
    mostrarTextoCentrado(20, "Usa las flechas ↑↓ para navegar y ENTER para seleccionar", 8); // Gris
    
    SetConsoleTextAttribute(hConsole, COLOR_NORMAL);
}

int obtenerSeleccionMenu() {
    int seleccion = 0;
    int opciones = 3;
    int tecla;
    
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    do {
        // Actualizar visualización de opciones
        mostrarTextoCentrado(12, seleccion == 0 ? "► PARTIDA NUEVA" : "  PARTIDA NUEVA", 
                        seleccion == 0 ? COLOR_SELECCION : COLOR_NORMAL);
        mostrarTextoCentrado(14, seleccion == 1 ? "► PARTIDA ANTERIOR" : "  PARTIDA ANTERIOR", 
                        seleccion == 1 ? COLOR_SELECCION : COLOR_NORMAL);
        mostrarTextoCentrado(16, seleccion == 2 ? "► RESUMEN DE RECURSOS" : "  RESUMEN DE RECURSOS", 
                        seleccion == 2 ? COLOR_SELECCION : COLOR_NORMAL);
        
        // Leer tecla
        tecla = getch();
        if (tecla == 0 || tecla == 224) { // Tecla especial
            tecla = getch();
            switch(tecla) {
                case 72: // Flecha arriba
                    seleccion = (seleccion - 1 + opciones) % opciones;
                    break;
                case 80: // Flecha abajo
                    seleccion = (seleccion + 1) % opciones;
                    break;
            }
        }
    } while(tecla != 13); // Enter
    
    return seleccion;
}

void partidaNueva() {
    system("cls");
    
    // Inicializar nueva partida
    strcpy(partidaActual.nombre, "Nueva Aventura");
    partidaActual.nivel = 1;
    partidaActual.recursos.oro = 100;
    partidaActual.recursos.madera = 50;
    partidaActual.recursos.piedra = 30;
    partidaActual.recursos.comida = 80;
    partidaActual.dia = 1;
    partidaCargada = 1;
    
    dibujarMarco();
    mostrarTextoCentrado(8, "╔══════════════════════════╗", COLOR_SELECCION);
    mostrarTextoCentrado(9, "║     NUEVA PARTIDA        ║", COLOR_SELECCION);
    mostrarTextoCentrado(10, "╚══════════════════════════╝", COLOR_SELECCION);
    
    mostrarTextoCentrado(13, "¡Bienvenido a tu nueva aventura!", COLOR_MENU);
    mostrarTextoCentrado(15, "Recursos iniciales:", COLOR_NORMAL);
    mostrarTextoCentrado(16, "• Oro: 100", COLOR_RECURSOS);
    mostrarTextoCentrado(17, "• Madera: 50", COLOR_RECURSOS);
    mostrarTextoCentrado(18, "• Piedra: 30", COLOR_RECURSOS);
    mostrarTextoCentrado(19, "• Comida: 80", COLOR_RECURSOS);
    
    mostrarTextoCentrado(22, "Presiona cualquier tecla para continuar...", 8);
    getch();
}

void partidaAnterior() {
    system("cls");
    
    dibujarMarco();
    
    if (!partidaCargada) {
        mostrarTextoCentrado(8, "╔══════════════════════════╗", 12); // Rojo
        mostrarTextoCentrado(9, "║   PARTIDA ANTERIOR       ║", 12);
        mostrarTextoCentrado(10, "╚══════════════════════════╝", 12);
        
        mostrarTextoCentrado(13, "No se encontró ninguna partida guardada.", 12);
        mostrarTextoCentrado(15, "¡Inicia una nueva partida para comenzar!", COLOR_MENU);
    } else {
        mostrarTextoCentrado(8, "╔══════════════════════════╗", COLOR_SELECCION);
        mostrarTextoCentrado(9, "║   PARTIDA CARGADA        ║", COLOR_SELECCION);
        mostrarTextoCentrado(10, "╚══════════════════════════╝", COLOR_SELECCION);
        
        mostrarTextoCentrado(12, partidaActual.nombre, COLOR_TITULO);
        mostrarTextoCentrado(13, "────────────────────────", COLOR_MENU);
        
        char buffer[100];
        sprintf(buffer, "Nivel: %d", partidaActual.nivel);
        mostrarTextoCentrado(14, buffer, COLOR_NORMAL);
        
        sprintf(buffer, "Día: %d", partidaActual.dia);
        mostrarTextoCentrado(15, buffer, COLOR_NORMAL);
        
        mostrarTextoCentrado(17, "Recursos actuales:", COLOR_NORMAL);
        sprintf(buffer, "• Oro: %d", partidaActual.recursos.oro);
        mostrarTextoCentrado(18, buffer, COLOR_RECURSOS);
        sprintf(buffer, "• Madera: %d", partidaActual.recursos.madera);
        mostrarTextoCentrado(19, buffer, COLOR_RECURSOS);
        sprintf(buffer, "• Piedra: %d", partidaActual.recursos.piedra);
        mostrarTextoCentrado(20, buffer, COLOR_RECURSOS);
        sprintf(buffer, "• Comida: %d", partidaActual.recursos.comida);
        mostrarTextoCentrado(21, buffer, COLOR_RECURSOS);
    }
    
    mostrarTextoCentrado(24, "Presiona cualquier tecla para continuar...", 8);
    getch();
}

void resumenRecursos() {
    system("cls");
    
    dibujarMarco();
    
    mostrarTextoCentrado(5, "╔══════════════════════════════╗", COLOR_RECURSOS);
    mostrarTextoCentrado(6, "║      RESUMEN DE RECURSOS     ║", COLOR_RECURSOS);
    mostrarTextoCentrado(7, "╚══════════════════════════════╝", COLOR_RECURSOS);
    
    if (!partidaCargada) {
        mostrarTextoCentrado(12, "No hay partida activa", 12);
        mostrarTextoCentrado(13, "Inicia una nueva partida para ver los recursos", COLOR_MENU);
    } else {
        // Mostrar recursos con iconos y barras de progreso
        mostrarTextoCentrado(10, "▓▓▓▓▓ RECURSOS ACTUALES ▓▓▓▓▓", COLOR_TITULO);
        
        char buffer[100];
        
        // Oro
        sprintf(buffer, "💰 Oro: %d", partidaActual.recursos.oro);
        mostrarTextoCentrado(12, buffer, 14); // Amarillo
        
        // Madera
        sprintf(buffer, "🌲 Madera: %d", partidaActual.recursos.madera);
        mostrarTextoCentrado(13, buffer, 6); // Naranja
        
        // Piedra
        sprintf(buffer, "⛰️  Piedra: %d", partidaActual.recursos.piedra);
        mostrarTextoCentrado(14, buffer, 8); // Gris
        
        // Comida
        sprintf(buffer, "🍖 Comida: %d", partidaActual.recursos.comida);
        mostrarTextoCentrado(15, buffer, 10); // Verde
        
        // Estadísticas
        mostrarTextoCentrado(17, "────────────────────────", COLOR_MENU);
        sprintf(buffer, "Nivel: %d", partidaActual.nivel);
        mostrarTextoCentrado(18, buffer, COLOR_NORMAL);
        sprintf(buffer, "Día: %d", partidaActual.dia);
        mostrarTextoCentrado(19, buffer, COLOR_NORMAL);
    }
    
    mostrarTextoCentrado(22, "Presiona cualquier tecla para volver al menú...", 8);
    getch();
}