#include "mapa.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ✅ MUNDO EXPANDIDO - 12 ISLAS
Isla islasFijas[NUM_ISLAS] = {
    {25, 20, 6, 5, (Color){210, 180, 140, 255}, 1},
    {60, 25, 7, 6, (Color){200, 170, 130, 255}, 1},
    {45, 45, 8, 7, (Color){220, 190, 150, 255}, 2},
    {10, 10, 4, 4, (Color){190, 160, 120, 255}, 0},
    {90, 8, 5, 5, (Color){200, 170, 130, 255}, 0},
    {8, 65, 6, 5, (Color){210, 180, 140, 255}, 1},
    {95, 70, 5, 6, (Color){190, 160, 120, 255}, 1},
    {30, 5, 4, 4, (Color){180, 150, 110, 255}, 0},
    {5, 35, 5, 5, (Color){200, 170, 130, 255}, 0},
    {80, 50, 6, 5, (Color){210, 180, 140, 255}, 1},
    {100, 25, 4, 4, (Color){190, 160, 120, 255}, 0},
    {15, 75, 7, 6, (Color){220, 190, 150, 255}, 2}
};

// ✅ RECURSOS
int arbolesFijos[][2] = {
    {22, 18}, {28, 16}, {20, 22}, {30, 20}, {25, 24},
    {57, 22}, {63, 20}, {55, 28}, {65, 26}, {60, 30},
    {40, 40}, {50, 40}, {42, 48}, {48, 48}, {45, 45},
    {6, 32}, {10, 35}, {4, 38}, {12, 40}, {8, 37},
    {36, 30}, {44, 28}, {40, 35}, {38, 38}, {42, 32}
};
#define NUM_ARBOLES 25

int oroFijo[][2] = {
    {25, 18}, {28, 22},
    {45, 8}, {42, 12},  
    {28, 22}, {22, 28},
    {6, 35}, {10, 38},
    {38, 30}, {42, 35}
};
#define NUM_ORO 10

int monumentosFijos[][2] = {
    {12, 8}, {48, 14}, {25, 25}, {8, 37}, {40, 28}
};
#define NUM_MONUMENTOS 5

int palmerasFijos[][2] = {
    {23, 16}, {27, 24}, {55, 20}, {65, 30}, {38, 38},
    {12, 6}, {88, 4}, {6, 60}, {92, 65}, {32, 3}
};
#define NUM_PALMERAS 10

int rocasFijos[][2] = {
    {20, 16}, {30, 24}, {53, 22}, {67, 26}, {40, 50},
    {8, 12}, {88, 10}, {4, 62}, {92, 68}, {28, 7}
};
#define NUM_ROCAS 10

// ✅ VARIABLES GLOBALES PARA PARTIDA
PartidaGuardada partidaActual;
static double tiempoUltimoGolpe = 0.0;

// ✅ FUNCIÓN CLAMP MANUAL
float ClampManual(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// ✅ FUNCIÓN PARA VERIFICAR SI UN PUNTO ESTÁ EN UNA ELIPSE
int estaEnElipse(int x, int y, int centroX, int centroY, int radioX, int radioY) {
    return ((x - centroX) * (x - centroX) * (radioY * radioY) + 
            (y - centroY) * (y - centroY) * (radioX * radioX)) <= 
           (radioX * radioX) * (radioY * radioY);
}

// ✅ FUNCIONES DEL MENÚ
Button CrearBoton(float x, float y, float width, float height, const char *text) {
    Button boton;
    boton.bounds = (Rectangle){x, y, width, height};
    boton.text = text;
    boton.hovered = false;
    return boton;
}

void DibujarBoton(Button boton) {
    Color colorFondo = boton.hovered ? 
        (Color){60, 140, 220, 255} : (Color){40, 100, 180, 255};
    Color colorBorde = boton.hovered ? YELLOW : (Color){20, 60, 120, 255};
    Color colorTexto = boton.hovered ? YELLOW : WHITE;
    
    DrawRectangleRounded(boton.bounds, 0.3f, 8, colorFondo);
    DrawRectangleRoundedLines(boton.bounds, 0.3f, 8, colorBorde);
    
    int tamanoFuente = 20;
    int anchoTexto = MeasureText(boton.text, tamanoFuente);
    Vector2 posTexto = {
        boton.bounds.x + (boton.bounds.width - anchoTexto) / 2,
        boton.bounds.y + (boton.bounds.height - tamanoFuente) / 2
    };
    
    DrawText(boton.text, posTexto.x, posTexto.y, tamanoFuente, colorTexto);
}

void DibujarMenuPrincipal(PartidaGuardada *partidaAnterior) {
    ClearBackground((Color){15, 50, 100, 255});
    
    const char *titulo = "ISLAS EN GUERRA - BATALLA";
    int tamanoTitulo = 36;
    int anchoTexto = MeasureText(titulo, tamanoTitulo);
    Vector2 posTitulo = {500 - anchoTexto/2, 80};
    
    DrawText(titulo, posTitulo.x + 2, posTitulo.y + 2, tamanoTitulo, DARKBLUE);
    DrawText(titulo, posTitulo.x, posTitulo.y, tamanoTitulo, GOLD);
    
    const char *subtitulo = "Recolecta recursos y conquista las islas";
    int anchoSubtitulo = MeasureText(subtitulo, 18);
    DrawText(subtitulo, 500 - anchoSubtitulo/2, 140, 18, SKYBLUE);
    
    // Botones del menú
    float anchoBoton = 400;
    float altoBoton = 50;
    float inicioY = 200;
    float espacio = 15;
    
    const char *textosBotones[] = {
        "NUEVA PARTIDA",
        "CONTINUAR PARTIDA ANTERIOR", 
        "VER RECURSOS ACUMULADOS",
        "SALIR"
    };
    
    for (int i = 0; i < 4; i++) {
        Button boton = CrearBoton(500 - anchoBoton/2, inicioY + i * (altoBoton + espacio),
                                anchoBoton, altoBoton, textosBotones[i]);
        boton.hovered = CheckCollisionPointRec(GetMousePosition(), boton.bounds);
        DibujarBoton(boton);
    }
    
    // Información partida anterior
    if (partidaAnterior->partidasGanadas > 0) {
        DrawText(TextFormat("Partida anterior: %d victorias", partidaAnterior->partidasGanadas), 
                10, 670, 14, LIGHTGRAY);
    }
    
    DrawText("v1.0 - Batalla por los Recursos", 700, 670, 14, LIGHTGRAY);
}

GameScreen MostrarMenu(PartidaGuardada *partidaAnterior) {
    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        
        float anchoBoton = 280;
        float altoBoton = 50;
        float inicioY = 200;
        float espacio = 15;
        
        for (int i = 0; i < 4; i++) {
            Rectangle boundsBoton = {
                500 - anchoBoton/2,
                inicioY + i * (altoBoton + espacio),
                anchoBoton,
                altoBoton
            };
            
            if (CheckCollisionPointRec(mousePos, boundsBoton) && 
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                return (GameScreen)(i + 1);  // NUEVA_PARTIDA = 1, etc.
            }
        }
        
        if (IsKeyPressed(KEY_ESCAPE)) return SALIR;
        
        BeginDrawing();
            DibujarMenuPrincipal(partidaAnterior);
            DrawCircleV(mousePos, 2, YELLOW);
        EndDrawing();
    }
    return SALIR;
}

// ✅ DIBUJAR MENÚ DE RECURSOS DE PARTIDA ANTERIOR
void dibujarMenuRecursos(PartidaGuardada *partida) {
    // Fondo
    DrawRectangle(200, 150, 600, 400, (Color){20, 20, 40, 240});
    DrawRectangleLines(200, 150, 600, 400, GOLD);
    
    // Título
    DrawText("RECURSOS PARTIDA ANTERIOR", 250, 180, 24, YELLOW);
    
    // Recursos
    DrawText(TextFormat("Madera acumulada: %d", partida->madera), 250, 230, 20, GREEN);
    DrawText(TextFormat("Oro acumulado: %d", partida->oro), 250, 260, 20, GOLD);
    DrawText(TextFormat("Barco: %s", partida->tieneBarco ? "CONSTRUIDO" : "NO CONSTRUIDO"), 250, 290, 20, BLUE);
    DrawText(TextFormat("Partidas ganadas: %d", partida->partidasGanadas), 250, 320, 20, LIME);
    DrawText(TextFormat("Partidas perdidas: %d", partida->partidasPerdidas), 250, 350, 20, RED);
    
    // Botón volver
    DrawText("Presiona ESC para volver al menu", 320, 450, 18, LIGHTGRAY);
}

// ✅ SISTEMA DE PARTIDAS
bool guardarPartida(Jugador *jugador) {
    partidaActual.madera = jugador->madera;
    partidaActual.oro = jugador->oro;
    partidaActual.posX = (int)jugador->posicion.x;
    partidaActual.posY = (int)jugador->posicion.y;
    partidaActual.tieneBarco = jugador->tieneBarco;
    partidaActual.partidasGanadas = 3;  // Ejemplo
    partidaActual.partidasPerdidas = 1; // Ejemplo
    
    // Aquí guardarías en archivo .bin
    return true;
}

bool cargarPartida(PartidaGuardada *partida) {
    // Aquí cargarías desde archivo .bin
    // Por ahora usamos datos de ejemplo
    partida->madera = 15;
    partida->oro = 8;
    partida->posX = islasFijas[2].centroX * TAM_CELDA;
    partida->posY = islasFijas[2].centroY * TAM_CELDA;
    partida->tieneBarco = true;
    partida->partidasGanadas = 3;
    partida->partidasPerdidas = 1;
    return true;
}

// ✅ SISTEMA DEL JUGADOR
void inicializarJugador(Jugador *jugador) {
    jugador->posicion = (Vector2){
        islasFijas[2].centroX * TAM_CELDA + TAM_CELDA / 2,
        islasFijas[2].centroY * TAM_CELDA + TAM_CELDA / 2
    };
    jugador->velocidad = 3;
    jugador->madera = 0;
    jugador->oro = 0;
    jugador->tieneBarco = false;
    jugador->golpesDados = 0;
    jugador->recolectando = false;
    jugador->color = RED;
}

bool puedeMoverseAJugador(int x, int y, char mapa[FILAS][COLUMNAS], Jugador *jugador) {
    // Convertir coordenadas de píxeles a celdas del mapa
    int celdaX = x / TAM_CELDA;
    int celdaY = y / TAM_CELDA;
    
    // Verificar límites del mapa
    if (celdaX < 0 || celdaX >= COLUMNAS || celdaY < 0 || celdaY >= FILAS) {
        return false;
    }
    
    char terreno = mapa[celdaY][celdaX];
    
    // ✅ TERRENOS PERMITIDOS EN TIERRA
    if (terreno == 'I' || terreno == 'T' || terreno == 'A' || 
        terreno == 'O' || terreno == 'M' || terreno == 'P' || terreno == 'R') {
        return true;
    }
    
    // ✅ AGUA - SOLO PERMITIDO CON BARCO
    if ((terreno == '~' || terreno == 'D') && jugador->tieneBarco) {
        return true;
    }
    
    // ❌ AGUA SIN BARCO - NO PERMITIDO
    if ((terreno == '~' || terreno == 'D') && !jugador->tieneBarco) {
        return false;
    }
    
    return false;
}

void verificarRecoleccion(Jugador *jugador, char mapa[FILAS][COLUMNAS]) {
    int celdaX = (int)jugador->posicion.x / TAM_CELDA;
    int celdaY = (int)jugador->posicion.y / TAM_CELDA;
    
    char recurso = mapa[celdaY][celdaX];
    
    // Solo permite recolectar en árboles y oro
    if (recurso != 'T' && recurso != 'A' && recurso != 'O') {
        jugador->golpesDados = 0;
        jugador->recolectando = false;
        return;
    }
    
    if (IsKeyPressed(KEY_SPACE)) {
        double tiempoActual = GetTime();
        
        // Reset si pasó mucho tiempo desde el último golpe
        if (tiempoActual - tiempoUltimoGolpe > 2.0) {
            jugador->golpesDados = 0;
        }
        
        jugador->golpesDados++;
        jugador->recolectando = true;
        tiempoUltimoGolpe = tiempoActual;
        
        if (jugador->golpesDados >= 3) {
            // ✅ RECOLECCIÓN EXITOSA
            if (recurso == 'T' || recurso == 'A') {
                jugador->madera += 2;  // 2 madera por árbol
                mapa[celdaY][celdaX] = 'I';  // Reemplazar con tierra
            }
            else if (recurso == 'O') {
                jugador->oro += 1;  // 1 oro por depósito
                mapa[celdaY][celdaX] = 'I';  // Reemplazar con tierra
            }
            
            jugador->golpesDados = 0;
            jugador->recolectando = false;
        }
    }
    
    // Reset después de 2 segundos sin presionar espacio
    if (jugador->recolectando && (GetTime() - tiempoUltimoGolpe) > 2.0) {
        jugador->golpesDados = 0;
        jugador->recolectando = false;
    }
}

void actualizarJugador(Jugador *jugador, char mapa[FILAS][COLUMNAS]) {
    Vector2 nuevaPosicion = jugador->posicion;
    
    // Movimiento WASD
    if (IsKeyDown(KEY_W)) nuevaPosicion.y -= jugador->velocidad;
    if (IsKeyDown(KEY_S)) nuevaPosicion.y += jugador->velocidad;
    if (IsKeyDown(KEY_A)) nuevaPosicion.x -= jugador->velocidad;
    if (IsKeyDown(KEY_D)) nuevaPosicion.x += jugador->velocidad;
    
    // Verificar movimiento válido
    if (puedeMoverseAJugador(nuevaPosicion.x, nuevaPosicion.y, mapa, jugador)) {
        jugador->posicion = nuevaPosicion;
    }
    
    // ✅ VERIFICAR RECOLECCIÓN CON ESPACIO
    verificarRecoleccion(jugador, mapa);
    
    // ✅ CONSTRUIR BARCO CON B
    if (IsKeyPressed(KEY_B) && !jugador->tieneBarco && jugador->madera >= 15) {
        jugador->tieneBarco = true;
        jugador->madera -= 15;
    }
}

void dibujarJugador(Jugador *jugador) {
    // Jugador
    DrawCircleV(jugador->posicion, TAM_CELDA / 2 - 2, jugador->color);
    DrawCircleLines(jugador->posicion.x, jugador->posicion.y, TAM_CELDA / 2 - 2, BLACK);
    
    // Barco
    if (jugador->tieneBarco) {
        DrawRectangle(jugador->posicion.x - 8, jugador->posicion.y - 4, 16, 8, BROWN);
    }
    
    // ✅ ANIMACIÓN DE RECOLECCIÓN
    if (jugador->recolectando) {
        DrawText(TextFormat("%d/3", jugador->golpesDados), 
                jugador->posicion.x - 10, jugador->posicion.y - 25, 12, YELLOW);
    }
}

void dibujarUI(Jugador *jugador) {
    // Panel de recursos
    DrawRectangle(10, 10, 180, 100, (Color){0, 0, 0, 200});
    
    DrawText("RECURSOS ACTUALES:", 20, 15, 14, WHITE);
    DrawText(TextFormat("Madera: %d", jugador->madera), 20, 35, 16, GREEN);
    DrawText(TextFormat("Oro: %d", jugador->oro), 20, 55, 16, GOLD);
    
    if (jugador->tieneBarco) {
        DrawText("BARCO: ACTIVO", 20, 80, 14, BLUE);
    } else {
        DrawText(TextFormat("BARCO: %d/15", jugador->madera), 20, 80, 14, RED);
    }
    
    // Controles
    DrawText("WASD: Moverse | ESPACIO: Recolectar (3x) | B: Barco (15 madera)", 10, 670, 12, LIGHTGRAY);
    DrawText("ESC: Menu", 850, 670, 14, LIGHTGRAY);
}

// ✅ SISTEMA DE CÁMARA
void inicializarCamara(CamaraJuego *camara) {
    // Centrar en la isla principal (índice 2)
    float islaCentroX = islasFijas[2].centroX * TAM_CELDA;
    float islaCentroY = islasFijas[2].centroY * TAM_CELDA;
    
    camara->camara.target = (Vector2){islaCentroX, islaCentroY};
    camara->camara.offset = (Vector2){500, 350};
    camara->camara.rotation = 0.0f;
    camara->camara.zoom = 1.0f;
    camara->objetivo = (Vector2){islaCentroX, islaCentroY};
    camara->zoom = 1.0f;
}

void actualizarCamara(CamaraJuego *camara, Jugador *jugador) {
    // La cámara sigue al jugador suavemente
    camara->objetivo.x += (jugador->posicion.x - camara->objetivo.x) * 0.1f;
    camara->objetivo.y += (jugador->posicion.y - camara->objetivo.y) * 0.1f;
    camara->camara.target = camara->objetivo;
    
    // Zoom con rueda del ratón (solo zoom in)
    float wheel = GetMouseWheelMove();
    if (wheel > 0) {
        camara->zoom += wheel * 0.1f;
        if (camara->zoom > 2.0f) camara->zoom = 2.0f;
        camara->camara.zoom = camara->zoom;
    }
    
    // ✅ CLAMPING DE LA CÁMARA
    float screenWidth = 1000.0f;
    float screenHeight = 700.0f;
    float mapWidth = COLUMNAS * TAM_CELDA;
    float mapHeight = FILAS * TAM_CELDA;
    
    float mundoVisibleAncho = screenWidth / camara->zoom;
    float mundoVisibleAlto = screenHeight / camara->zoom;
    
    if (mundoVisibleAncho >= mapWidth) {
        camara->camara.target.x = mapWidth / 2.0f;
    } else {
        float halfVisibleAncho = mundoVisibleAncho / 2.0f;
        float minX = halfVisibleAncho;
        float maxX = mapWidth - halfVisibleAncho;
        camara->camara.target.x = ClampManual(camara->camara.target.x, minX, maxX);
    }
    
    if (mundoVisibleAlto >= mapHeight) {
        camara->camara.target.y = mapHeight / 2.0f;
    } else {
        float halfVisibleAlto = mundoVisibleAlto / 2.0f;
        float minY = halfVisibleAlto;
        float maxY = mapHeight - halfVisibleAlto;
        camara->camara.target.y = ClampManual(camara->camara.target.y, minY, maxY);
    }
}

// ✅ SISTEMA DEL MAPA
void dibujarIsla(Isla isla, char mapa[FILAS][COLUMNAS]) {
    for (int y = 0; y < FILAS; y++) {
        for (int x = 0; x < COLUMNAS; x++) {
            if (estaEnElipse(x, y, isla.centroX, isla.centroY, isla.radioX, isla.radioY)) {
                mapa[y][x] = 'I';
            }
        }
    }
}

void agregarVegetacionFija(char mapa[FILAS][COLUMNAS]) {
    for (int i = 0; i < NUM_ARBOLES; i++) {
        int x = arbolesFijos[i][0];
        int y = arbolesFijos[i][1];
        
        if (x >= 0 && x < COLUMNAS && y >= 0 && y < FILAS && mapa[y][x] == 'I') {
            mapa[y][x] = 'T';
            
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && nx < COLUMNAS && ny >= 0 && ny < FILAS && 
                        mapa[ny][nx] == 'I' && (dx != 0 || dy != 0)) {
                        mapa[ny][nx] = 'A';
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < NUM_PALMERAS; i++) {
        int x = palmerasFijos[i][0];
        int y = palmerasFijos[i][1];
        if (x >= 0 && x < COLUMNAS && y >= 0 && y < FILAS && mapa[y][x] == 'I') {
            mapa[y][x] = 'P';
        }
    }
}

void agregarRocasFijas(char mapa[FILAS][COLUMNAS]) {
    for (int i = 0; i < NUM_ROCAS; i++) {
        int x = rocasFijos[i][0];
        int y = rocasFijos[i][1];
        if (x >= 0 && x < COLUMNAS && y >= 0 && y < FILAS && mapa[y][x] == 'I') {
            mapa[y][x] = 'R';
        }
    }
}

void agregarOroFijo(char mapa[FILAS][COLUMNAS]) {
    for (int i = 0; i < NUM_ORO; i++) {
        int x = oroFijo[i][0];
        int y = oroFijo[i][1];
        if (x >= 0 && x < COLUMNAS && y >= 0 && y < FILAS && mapa[y][x] == 'I') {
            mapa[y][x] = 'O';
        }
    }
}

void agregarMonumentosFijos(char mapa[FILAS][COLUMNAS]) {
    for (int i = 0; i < NUM_MONUMENTOS; i++) {
        int x = monumentosFijos[i][0];
        int y = monumentosFijos[i][1];
        if (x >= 0 && x < COLUMNAS && y >= 0 && y < FILAS && mapa[y][x] == 'I') {
            mapa[y][x] = 'M';
        }
    }
}

void agregarAguaTexturizada(char mapa[FILAS][COLUMNAS]) {
    for (int y = 0; y < FILAS; y++) {
        for (int x = 0; x < COLUMNAS; x++) {
            if (mapa[y][x] == '~') {
                int cercaDeIsla = 0;
                for (int i = 0; i < NUM_ISLAS; i++) {
                    if (abs(x - islasFijas[i].centroX) < 20 && abs(y - islasFijas[i].centroY) < 20) {
                        cercaDeIsla = 1;
                        break;
                    }
                }
                
                if (!cercaDeIsla && (x % 4 == 0 || y % 4 == 0)) {
                    mapa[y][x] = 'D';
                }
            }
        }
    }
}

void inicializarMapa(char mapa[FILAS][COLUMNAS]) {
    for (int y = 0; y < FILAS; y++) {
        for (int x = 0; x < COLUMNAS; x++) {
            mapa[y][x] = '~';
        }
    }
    
    for (int i = 0; i < NUM_ISLAS; i++) {
        dibujarIsla(islasFijas[i], mapa);
    }
    
    agregarAguaTexturizada(mapa);
    agregarVegetacionFija(mapa);
    agregarOroFijo(mapa);
    agregarMonumentosFijos(mapa);
    agregarRocasFijas(mapa);
}

void dibujarMapa(char mapa[FILAS][COLUMNAS]) {
    for (int y = 0; y < FILAS; y++) {
        for (int x = 0; x < COLUMNAS; x++) {
            Color color;
            char caracter = mapa[y][x];
            
            switch (caracter) {
                case '~': color = (Color){30, 144, 255, 255}; break;
                case 'D': color = (Color){25, 25, 112, 255}; break;
                case 'I': color = (Color){210, 180, 140, 255}; break;
                case 'T': color = (Color){101, 67, 33, 255}; break;
                case 'A': color = (Color){0, 100, 0, 255}; break;
                case 'O': color = (Color){255, 215, 0, 255}; break;
                case 'M': color = (Color){128, 128, 128, 255}; break;
                case 'P': color = (Color){139, 69, 19, 255}; break;
                case 'R': color = (Color){105, 105, 105, 255}; break;
                default: color = (Color){30, 144, 255, 255}; break;
            }
            
            DrawRectangle(x * TAM_CELDA, y * TAM_CELDA, TAM_CELDA - 1, TAM_CELDA - 1, color);
        }
    }
    
    // ✅ DIBUJAR BORDE DEL MAPA PARA VISUALIZAR LÍMITES
    DrawRectangleLines(0, 0, COLUMNAS * TAM_CELDA, FILAS * TAM_CELDA, RED);
}