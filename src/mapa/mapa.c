#include "raylib.h"       
#include "mapa.h" 
#include <stdio.h>

// Función para crear botones
Button CrearBoton(float x, float y, float width, float height, const char *text) {
    Button boton;
    boton.bounds = (Rectangle){x, y, width, height};
    boton.text = text;
    boton.hovered = false;
    return boton;
}

// Función para dibujar botones (VERSIÓN CORREGIDA)
void DibujarBoton(Button boton) {
    Color colorFondo = boton.hovered ? 
        (Color){60, 140, 220, 255} :   // Azul claro al pasar mouse
        (Color){40, 100, 180, 255};    // Azul normal
    
    Color colorBorde = boton.hovered ? YELLOW : (Color){20, 60, 120, 255};
    Color colorTexto = boton.hovered ? YELLOW : WHITE;
    
    // Fondo del botón redondeado - FORMA COMPATIBLE
    DrawRectangleRounded(boton.bounds, 0.3f, 8, colorFondo);
    
    // Líneas del borde - FORMA COMPATIBLE con versiones antiguas
    // En versiones nuevas: DrawRectangleRoundedLines(rec, roundness, segments, lineThick, color)
    // En versiones antiguas: DrawRectangleRoundedLines(rec, roundness, segments, color)
    DrawRectangleRoundedLines(boton.bounds, 0.3f, 8, colorBorde);
    
    // Texto centrado
    int tamanoFuente = 20;
    Vector2 tamanoTexto = MeasureTextEx(GetFontDefault(), boton.text, tamanoFuente, 1);
    Vector2 posTexto = {
        boton.bounds.x + (boton.bounds.width - tamanoTexto.x) / 2,
        boton.bounds.y + (boton.bounds.height - tamanoTexto.y) / 2
    };
    
    DrawTextEx(GetFontDefault(), boton.text, posTexto, tamanoFuente, 1, colorTexto);
}

// Alternativa si la anterior no funciona - usar rectángulo normal
void DibujarBotonAlternativo(Button boton) {
    Color colorFondo = boton.hovered ? 
        (Color){60, 140, 220, 255} : 
        (Color){40, 100, 180, 255};
    
    Color colorBorde = boton.hovered ? YELLOW : (Color){20, 60, 120, 255};
    Color colorTexto = boton.hovered ? YELLOW : WHITE;
    
    // Fondo del botón
    DrawRectangleRec(boton.bounds, colorFondo);
    
    // Borde
    DrawRectangleLinesEx(boton.bounds, 3, colorBorde);
    
    // Texto centrado
    int tamanoFuente = 20;
    Vector2 tamanoTexto = MeasureTextEx(GetFontDefault(), boton.text, tamanoFuente, 1);
    Vector2 posTexto = {
        boton.bounds.x + (boton.bounds.width - tamanoTexto.x) / 2,
        boton.bounds.y + (boton.bounds.height - tamanoTexto.y) / 2
    };
    
    DrawTextEx(GetFontDefault(), boton.text, posTexto, tamanoFuente, 1, colorTexto);
}

// Dibujar menú principal (VERSIÓN CORREGIDA)
void DibujarMenuPrincipal(void) {
    // Fondo azul marino en lugar de gradiente (más simple)
    ClearBackground((Color){15, 50, 100, 255});
    
    // Título principal
    const char *titulo = "ISLAS EN GUERRA";
    int tamanoTitulo = 36;
    Vector2 tamanoTituloVec = MeasureTextEx(GetFontDefault(), titulo, tamanoTitulo, 2);
    Vector2 posTitulo = {250 - tamanoTituloVec.x/2, 80};
    
    // Título con sombra
    DrawTextEx(GetFontDefault(), titulo, 
            (Vector2){posTitulo.x + 2, posTitulo.y + 2}, 
            tamanoTitulo, 2, DARKBLUE);
    DrawTextEx(GetFontDefault(), titulo, posTitulo, tamanoTitulo, 2, GOLD);
    
    // Subtítulo
    const char *subtitulo = "Conquista el Archipiélago";
    DrawText(subtitulo, 250 - MeasureText(subtitulo, 18)/2, 140, 18, SKYBLUE);
    
    // Crear y dibujar botones
    float anchoBoton = 250;
    float altoBoton = 50;
    float inicioY = 200;
    float espacio = 15;
    
    const char *textosBotones[] = {
        "NUEVA PARTIDA",
        "CARGAR PARTIDA", 
        "OPCIONES",
        "SALIR"
    };
    
    for (int i = 0; i < 4; i++) {
        Button boton = CrearBoton(
            250 - anchoBoton/2,
            inicioY + i * (altoBoton + espacio),
            anchoBoton,
            altoBoton,
            textosBotones[i]
        );
        
        boton.hovered = CheckCollisionPointRec(GetMousePosition(), boton.bounds);
        
        // Usa una de estas dos líneas:
        DibujarBoton(boton);                    // Intenta primero esta
        // DibujarBotonAlternativo(boton);      // Si falla, usa esta alternativa
    }
    
    // Información de versión
    DrawText("v1.0 - Presiona ESC para salir", 10, 470, 16, LIGHTGRAY);
}

// Función principal del menú
GameScreen MostrarMenu(void) {
    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        
        // Configuración de botones
        float anchoBoton = 250;
        float altoBoton = 50;
        float inicioY = 200;
        float espacio = 15;
        
        // Verificar clics en botones
        for (int i = 0; i < 4; i++) {
            Rectangle boundsBoton = {
                250 - anchoBoton/2,
                inicioY + i * (altoBoton + espacio),
                anchoBoton,
                altoBoton
            };
            
            if (CheckCollisionPointRec(mousePos, boundsBoton) && 
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                return (GameScreen)i;
            }
        }
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            return SALIR;
        }
        
        BeginDrawing();
            DibujarMenuPrincipal();
            DrawCircleV(mousePos, 2, YELLOW);
        EndDrawing();
    }
    return SALIR;
}

// Tus funciones existentes del mapa
void inicializarMapa(char mapa[FILAS][COLUMNAS]) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            mapa[i][j] = '.';
        }
    }
}

void dibujarMapa(char mapa[FILAS][COLUMNAS]) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            DrawRectangle(j * 25, i * 25 + 40, 24, 24, 
                    mapa[i][j] == '.' ? DARKGREEN : BROWN);
            DrawText(TextFormat("%c", mapa[i][j]), j * 25 + 5, i * 25 + 45, 10, WHITE);
        }
    }
}