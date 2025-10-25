#include "raylib.h"
#include "mapa/mapa.h"

const int screenWidth = 1000;
const int screenHeight = 700;

// Variables globales del juego
char mapaJuego[FILAS][COLUMNAS];
CamaraJuego camaraJuego;
Jugador jugador;
PartidaGuardada partidaAnterior;

int main(void)
{
    // Inicializar ventana
    InitWindow(screenWidth, screenHeight, "Islas en Guerra - Batalla por Recursos");
    SetTargetFPS(60);
    
    // Cargar partida anterior al iniciar
    cargarPartida(&partidaAnterior);
    
    // Mostrar menú principal al inicio
    GameScreen pantallaActual = MostrarMenu(&partidaAnterior);

    // ✅ BUCLE PRINCIPAL DEL JUEGO
    while (!WindowShouldClose()) {
        switch (pantallaActual) {
            case NUEVA_PARTIDA:
                // ✅ INICIAR NUEVA PARTIDA DESDE CERO
                inicializarMapa(mapaJuego);
                inicializarJugador(&jugador);
                inicializarCamara(&camaraJuego);
                pantallaActual = EXPLORACION;
                break;
                
            case CARGAR_PARTIDA:
                // ✅ CARGAR PARTIDA ANTERIOR
                inicializarMapa(mapaJuego);
                
                // Configurar jugador con datos de partida guardada
                jugador.madera = partidaAnterior.madera;
                jugador.oro = partidaAnterior.oro;
                jugador.posicion.x = partidaAnterior.posX;
                jugador.posicion.y = partidaAnterior.posY;
                jugador.tieneBarco = partidaAnterior.tieneBarco;
                jugador.velocidad = 3;
                jugador.golpesDados = 0;
                jugador.recolectando = false;
                jugador.color = RED;
                
                inicializarCamara(&camaraJuego);
                pantallaActual = EXPLORACION;
                break;
                
            case VER_RECURSOS:
                // ✅ PANTALLA DE RECURSOS ACUMULADOS
                BeginDrawing();
                    ClearBackground((Color){15, 50, 100, 255});
                    dibujarMenuRecursos(&partidaAnterior);
                EndDrawing();
                
                if (IsKeyPressed(KEY_ESCAPE)) {
                    pantallaActual = MostrarMenu(&partidaAnterior);
                }
                break;
                
            case EXPLORACION:
                // ✅ MODO EXPLORACIÓN - JUEGO PRINCIPAL
                
                // Actualizar lógica del juego
                actualizarJugador(&jugador, mapaJuego);
                actualizarCamara(&camaraJuego, &jugador);
                
                // Renderizar juego
                BeginDrawing();
                    ClearBackground(BLACK);
                    
                    // ✅ DIBUJAR MUNDO CON CÁMARA 2D
                    BeginMode2D(camaraJuego.camara);
                        dibujarMapa(mapaJuego);
                        dibujarJugador(&jugador);
                    EndMode2D();
                    
                    // ✅ DIBUJAR INTERFAZ DE USUARIO (sin afectar por cámara)
                    dibujarUI(&jugador);
                    DrawText("MODO EXPLORACION - Recolecta recursos para la batalla!", 300, 120, 16, YELLOW);
                    
                EndDrawing();
                
                // ✅ VOLVER AL MENÚ PRINCIPAL
                if (IsKeyPressed(KEY_ESCAPE)) {
                    guardarPartida(&jugador);
                    pantallaActual = MostrarMenu(&partidaAnterior);
                }
                break;
                
            case BATALLA:
                // ✅ MODO BATALLA (PARA IMPLEMENTAR EN EL FUTURO)
                BeginDrawing();
                    // ✅ CORREGIDO: Usar color manual en lugar de DARKRED
                    ClearBackground((Color){139, 0, 0, 255});  // Rojo oscuro
                    DrawText("MODO BATALLA - EN DESARROLLO", 350, 350, 24, YELLOW);
                    DrawText("Presiona ESC para volver al menu", 380, 400, 18, WHITE);
                EndDrawing();
                
                if (IsKeyPressed(KEY_ESCAPE)) {
                    pantallaActual = MostrarMenu(&partidaAnterior);
                }
                break;
                
            case MENU_PRINCIPAL:
                // ✅ VOLVER AL MENÚ PRINCIPAL
                pantallaActual = MostrarMenu(&partidaAnterior);
                break;
                
            case SALIR:
                // ✅ SALIR DEL JUEGO
                CloseWindow();
                return 0;
        }
    }

    // ✅ LIMPIEZA Y CIERRE
    CloseWindow();
    return 0;
}
