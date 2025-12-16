//#include "sistema/sistema.h"
/* src/main.c - PUNTO DE ENTRADA PRINCIPAL DEL JUEGO */
#include <windows.h>
#include <stdio.h>
#include "mapa/mapa.h"
#include "recursos/recursos.h"

/* ========== VARIABLES GLOBALES DEL JUEGO ========== */
char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];  // Mapa del mundo 100x100
Jugador miJugador;                            // Datos del jugador
Camera miCamara;                              // Cámara del juego (con zoom)
EstadoJuego estadoJuego;                      // Estado actual del juego

/* ========== PROTOTIPOS DE FUNCIONES LOCALES ========== */
void inicializarVentana(HINSTANCE hInstance, int nCmdShow);
void mostrarInformacionDebug();

/* ========== PROCEDIMIENTO DE VENTANA PRINCIPAL ========== */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        /* --- Mensajes del sistema --- */
        case WM_DESTROY:
            printf("Cerrando juego...\n");
            PostQuitMessage(0);
            return 0;
            
        case WM_CLOSE:
            printf("Ventana cerrada por usuario\n");
            PostQuitMessage(0);
            return 0;
            
        /* --- Evitar parpadeo (doble búfer) --- */
        case WM_ERASEBKGND:
            return 1;  // Indicar que manejamos el fondo nosotros
            
        /* --- Eventos de teclado --- */
        case WM_KEYDOWN:
        {
            printf("Tecla presionada: %d\n", (int)wParam);
            
            /* Menú principal activo */
            if (estadoJuego.mostrarMenu) {
                switch (wParam)
                {
                    case VK_UP:      // Flecha arriba
                        estadoJuego.opcionSeleccionada--;
                        if (estadoJuego.opcionSeleccionada < 0) {
                            estadoJuego.opcionSeleccionada = 2;
                        }
                        printf("Menú: Opción %d seleccionada\n", estadoJuego.opcionSeleccionada);
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;
                        
                    case VK_DOWN:    // Flecha abajo
                        estadoJuego.opcionSeleccionada++;
                        if (estadoJuego.opcionSeleccionada > 2) {
                            estadoJuego.opcionSeleccionada = 0;
                        }
                        printf("Menú: Opción %d seleccionada\n", estadoJuego.opcionSeleccionada);
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;
                        
                    case VK_RETURN:  // Enter
                        printf("Menú: Confirmando opción %d\n", estadoJuego.opcionSeleccionada);
                        procesarEnterMenu(hwnd, &estadoJuego);
                        break;
                        
                    case VK_ESCAPE:  // Escape
                        if (estadoJuego.opcionSeleccionada == 2) {
                            // Si ya está seleccionado "SALIR", salir
                            printf("Menú: Saliendo del juego\n");
                            PostQuitMessage(0);
                        } else {
                            // Si no, seleccionar opción SALIR
                            printf("Menú: Seleccionando opción SALIR\n");
                            estadoJuego.opcionSeleccionada = 2;
                            InvalidateRect(hwnd, NULL, FALSE);
                        }
                        break;
                }
            } 
            /* Panel de recursos activo */
            else if (estadoJuego.mostrarResumen) {
                if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
                    printf("Recursos: Volviendo al menú\n");
                    estadoJuego.mostrarMenu = 1;
                    estadoJuego.mostrarResumen = 0;
                    estadoJuego.opcionSeleccionada = 0;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            /* Partida en curso */
            else if (estadoJuego.enPartida) {
                switch (wParam)
                {
                    /* Movimiento del jugador */
                    case VK_LEFT:
                    case 'A':
                        printf("Movimiento: Izquierda\n");
                        moverJugador(&miJugador, mapaMundo, -1, 0);
                        break;
                        
                    case VK_RIGHT:
                    case 'D':
                        printf("Movimiento: Derecha\n");
                        moverJugador(&miJugador, mapaMundo, 1, 0);
                        break;
                        
                    case VK_UP:
                    case 'W':
                        printf("Movimiento: Arriba\n");
                        moverJugador(&miJugador, mapaMundo, 0, -1);
                        break;
                        
                    case VK_DOWN:
                    case 'S':
                        printf("Movimiento: Abajo\n");
                        moverJugador(&miJugador, mapaMundo, 0, 1);
                        break;
                        
                    /* Sistema de zoom - CON DEBUG MEJORADO */
                    case VK_ADD:       // Tecla +
                    case 0xBB:         // = (Shift++)
                        if (miCamara.zoom < 4.0f) {
                            printf("\n=== ANTES DEL ZOOM IN ===\n");
                            printf("Jugador: (%d,%d)\n", miJugador.x, miJugador.y);
                            printf("Cámara: (%d,%d) Zoom: %.1f\n", miCamara.x, miCamara.y, miCamara.zoom);
                            
                            miCamara.zoom += 0.5f;
                            printf("Nuevo zoom: %.1fx\n", miCamara.zoom);
                            
                            actualizarCamara(&miCamara, miJugador);
                            
                            printf("=== DESPUÉS DEL ZOOM IN ===\n");
                            printf("Cámara nueva: (%d,%d)\n", miCamara.x, miCamara.y);
                            printf("Centro teórico: jugador(%d,%d)\n", miJugador.x, miJugador.y);
                            printf("Tamaño celda: %dpx\n", miCamara.tamano_celda_actual);
                            
                            InvalidateRect(hwnd, NULL, TRUE);
                        }
                        break;
                        
                    case VK_SUBTRACT:  // Tecla -
                    case 0xBD:         // - (guión)
                        if (miCamara.zoom > 0.5f) {
                            printf("\n=== ANTES DEL ZOOM OUT ===\n");
                            printf("Jugador: (%d,%d)\n", miJugador.x, miJugador.y);
                            printf("Cámara: (%d,%d) Zoom: %.1f\n", miCamara.x, miCamara.y, miCamara.zoom);
                            
                            miCamara.zoom -= 0.5f;
                            printf("Nuevo zoom: %.1fx\n", miCamara.zoom);
                            
                            actualizarCamara(&miCamara, miJugador);
                            
                            printf("=== DESPUÉS DEL ZOOM OUT ===\n");
                            printf("Cámara nueva: (%d,%d)\n", miCamara.x, miCamara.y);
                            printf("Centro teórico: jugador(%d,%d)\n", miJugador.x, miJugador.y);
                            printf("Tamaño celda: %dpx\n", miCamara.tamano_celda_actual);
                            
                            InvalidateRect(hwnd, NULL, TRUE);
                        }
                        break;
                        
                    case 'Z':          // Resetear zoom
                        printf("\n=== RESET ZOOM ===\n");
                        miCamara.zoom = 1.0f;
                        printf("Zoom: Reseteado a 1.0x\n");
                        actualizarCamara(&miCamara, miJugador);
                        InvalidateRect(hwnd, NULL, TRUE);
                        break;
                        
                    /* Menú de pausa/salir */
                    case VK_ESCAPE:
                        printf("Partida: Pausa - Volviendo al menú\n");
                        estadoJuego.mostrarMenu = 1;
                        estadoJuego.enPartida = 0;
                        estadoJuego.mostrarResumen = 0;
                        estadoJuego.opcionSeleccionada = 0;
                        InvalidateRect(hwnd, NULL, TRUE);  // FIX: Añadido para redibujar
                        break;
                        
                    /* Teclas de debug/información - MEJORADO */
                    case 'I':
                        printf("\n=== INFORMACIÓN DEL JUEGO ===\n");
                        printf("Jugador: Posición (%d, %d)\n", miJugador.x, miJugador.y);
                        printf("Cámara: Posición (%d, %d) Zoom: %.1fx\n", 
                                miCamara.x, miCamara.y, miCamara.zoom);
                        printf("Tamaño celda: %dpx (base: %dpx)\n", 
                                miCamara.tamano_celda_actual, TAMANO_CELDA_BASE);
                        
                        // Calcular celdas visibles
                        float visible_x = (float)PANTALLA_COLUMNAS / miCamara.zoom;
                        float visible_y = (float)PANTALLA_FILAS / miCamara.zoom;
                        printf("Celdas visibles: %.1f x %.1f\n", visible_x, visible_y);
                        
                        printf("Recursos: Oro=%d, Madera=%d, Piedra=%d\n",
                                miJugador.oro, miJugador.madera, miJugador.piedra);
                        printf("Vida: %d/%d\n", miJugador.vida, miJugador.vida_maxima);
                        printf("============================\n");
                        break;
                }
                
                // Actualizar cámara y redibujar después de movimiento
                if (wParam == VK_LEFT || wParam == 'A' ||
                    wParam == VK_RIGHT || wParam == 'D' ||
                    wParam == VK_UP || wParam == 'W' ||
                    wParam == VK_DOWN || wParam == 'S') {
                    actualizarCamara(&miCamara, miJugador);
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
        }
        
        /* --- Evento de redibujado (con doble búfer) --- */
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Obtener dimensiones del área cliente
            RECT rectClient;
            GetClientRect(hwnd, &rectClient);
            int ancho = rectClient.right - rectClient.left;
            int alto = rectClient.bottom - rectClient.top;
            
            printf("DEBUG WM_PAINT: Estado - Menu=%d, Partida=%d, Resumen=%d\n",
                   estadoJuego.mostrarMenu, estadoJuego.enPartida, estadoJuego.mostrarResumen);
            
            // === CREAR BÚFER EN MEMORIA (DOBLE BÚFER) ===
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmMem = CreateCompatibleBitmap(hdc, ancho, alto);
            HBITMAP hbmOld = SelectObject(hdcMem, hbmMem);
            
            // === DIBUJAR TODO EN EL BÚFER DE MEMORIA ===
            
            /* Menú principal */
            if (estadoJuego.mostrarMenu) {
                printf("Dibujando menú principal...\n");
                dibujarMenuConSprites(hdcMem, hwnd, &estadoJuego);
            }
            /* Panel de recursos */
            else if (estadoJuego.mostrarResumen) {
                printf("Dibujando panel de recursos...\n");
                dibujarResumenRecursos(hdcMem, miJugador, &estadoJuego);
            }
            /* Partida en curso */
            else if (estadoJuego.enPartida) {
                printf("Dibujando partida (Zoom: %.1fx)...\n", miCamara.zoom);
                
                // Dibujar mapa con sistema de zoom
                dibujarMapaConZoom(hdcMem, mapaMundo, miCamara, ancho, alto);
                
                // Dibujar jugador (con zoom aplicado)
                dibujarJugador(hdcMem, miJugador, miCamara);
                
                // Mostrar información de debug en consola (cada 30 frames)
                static int frameCount = 0;
                frameCount++;
                if (frameCount % 30 == 0) {  // Cada 30 frames
                    printf("Frame %d: Jugador en (%d, %d) Cámara en (%d, %d) Zoom: %.1fx\n",
                            frameCount, miJugador.x, miJugador.y,
                            miCamara.x, miCamara.y, miCamara.zoom);
                }
            }
            /* Estado inesperado (debería dibujar algo) */
            else {
                printf("ERROR: Estado inválido - dibujando fondo negro\n");
                HBRUSH hBrushNegro = CreateSolidBrush(RGB(0, 0, 0));
                RECT rectFondo = {0, 0, ancho, alto};
                FillRect(hdcMem, &rectFondo, hBrushNegro);
                DeleteObject(hBrushNegro);
            }
            
            // === COPIAR BÚFER COMPLETO A PANTALLA (INSTANTÁNEO) ===
            BitBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, SRCCOPY);
            
            // === LIMPIAR RECURSOS DEL BÚFER ===
            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        /* --- Evento de cambio de tamaño --- */
        case WM_SIZE:
            printf("Ventana redimensionada\n");
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
            
        /* --- Evento de activación de ventana --- */
        case WM_ACTIVATE:
            if (wParam != WA_INACTIVE) {
                printf("Ventana activada\n");
            }
            return 0;
    }
    
    // Mensajes no manejados pasan al procedimiento por defecto
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/* ========== INICIALIZACIÓN DE VENTANA ========== */
void inicializarVentana(HINSTANCE hInstance, int nCmdShow) {
    // Registrar clase de ventana
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = TEXT("IslasEnGuerraClase");
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.style         = CS_HREDRAW | CS_VREDRAW;  // Redibujar al cambiar tamaño
    
    if (!RegisterClass(&wc)) {
        printf("ERROR: No se pudo registrar clase de ventana\n");
        return;
    }
    
    // Calcular tamaño de ventana (800x600 área cliente + bordes)
    int anchoCliente = 800;
    int altoCliente = 600;
    
    RECT rect = {0, 0, anchoCliente, altoCliente};
    AdjustWindowRect(&rect, 
                    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                    FALSE);
    
    int anchoVentana = rect.right - rect.left;
    int altoVentana = rect.bottom - rect.top;
    
    printf("Creando ventana: Cliente=%dx%d, Total=%dx%d\n",
           anchoCliente, altoCliente, anchoVentana, altoVentana);
    
    // Crear ventana
    HWND hwnd = CreateWindowEx(
        0,                                  // Estilos extendidos
        TEXT("IslasEnGuerraClase"),         // Clase
        TEXT("War Islands v1.0 - 800x600"), // Título (con resolución)
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Estilos
        CW_USEDEFAULT, CW_USEDEFAULT,       // Posición
        anchoVentana, altoVentana,          // Tamaño
        NULL,                               // Ventana padre
        NULL,                               // Menú
        hInstance,                          // Instancia
        NULL                                // Datos adicionales
    );
    
    if (hwnd == NULL) {
        printf("ERROR: No se pudo crear ventana\n");
        return;
    }
    
    // Mostrar y actualizar ventana
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Inicializar juego
    printf("\n=== INICIALIZANDO JUEGO ===\n");
    inicializarJuego(&miJugador, &estadoJuego, mapaMundo);
    
    // Inicializar cámara con zoom
    miCamara.x = 0;
    miCamara.y = 0;
    miCamara.zoom = 1.0f;
    miCamara.tamano_celda_actual = TAMANO_CELDA_BASE;
    
    // Posicionar cámara centrada en el jugador
    actualizarCamara(&miCamara, miJugador);
    
    printf("Juego inicializado. Jugador en posición (%d, %d)\n", miJugador.x, miJugador.y);
    printf("Cámara centrada en (%d, %d) Zoom: %.1fx\n", miCamara.x, miCamara.y, miCamara.zoom);
    printf("Tamaño celda: %dpx\n", miCamara.tamano_celda_actual);
    
    // Cargar recursos gráficos
    printf("\n=== CARGANDO RECURSOS GRÁFICOS ===\n");
    if (!cargar_sprites()) {
        MessageBox(NULL, 
                  TEXT("Error cargando recursos gráficos\nEl juego puede no mostrarse correctamente."), 
                  TEXT("Advertencia"), 
                  MB_OK | MB_ICONWARNING);
    }
    
    // === FIX CRÍTICO: FORZAR PRIMER DIBUJADO ===
    printf("\n=== FORZANDO PRIMER DIBUJADO DEL MENÚ ===\n");
    InvalidateRect(hwnd, NULL, TRUE);    // Marcar toda el área como "sucio"
    UpdateWindow(hwnd);                  // Procesar WM_PAINT INMEDIATAMENTE
    
    // Bucle principal de mensajes
    MSG msg = {0};
    printf("\n=== JUEGO INICIADO - ESPERANDO EVENTOS ===\n");
    printf("Controles:\n");
    printf("  Menú: Flechas + Enter • ESC para salir\n");
    printf("  Movimiento: WASD o Flechas\n");
    printf("  Zoom: + / - (teclas numéricas)\n");
    printf("  Reset Zoom: Z\n");
    printf("  Información: I\n");
    printf("  Volver al menú desde partida: ESC\n\n");
    
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Limpiar antes de salir
    printf("\n=== LIMPIANDO RECURSOS ===\n");
    liberar_sprites();
    printf("Recursos liberados. Saliendo...\n");
}

/* ========== PUNTO DE ENTRADA DE WINDOWS ========== */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow)
{
    // Inicializar consola para debug
    #ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    #endif
    
    printf("========================================\n");
    printf("        WAR ISLANDS v1.0\n");
    printf("        Desarrollado en C\n");
    printf("        Resolución: 800x600\n");
    printf("        Celdas: 10px (%dx%d visibles)\n", 
           PANTALLA_COLUMNAS, PANTALLA_FILAS);
    printf("        Sistema de zoom: 0.5x - 4.0x\n");
    printf("========================================\n\n");
    
    // Inicializar ventana y comenzar juego
    inicializarVentana(hInstance, nCmdShow);
    
    #ifdef _DEBUG
    FreeConsole();
    #endif
    
    return 0;
}

/* ========== FUNCIONES AUXILIARES ========== */

void mostrarInformacionDebug() {
    printf("\n=== INFORMACIÓN DEL SISTEMA ===\n");
    printf("Tamaño de celda: %dpx\n", TAMANO_CELDA);
    printf("Celdas visibles: %dx%d\n", PANTALLA_COLUMNAS, PANTALLA_FILAS);
    printf("Mundo: %dx%d celdas\n", MUNDO_COLUMNAS, MUNDO_FILAS);
    printf("Zoom actual: %.1fx\n", miCamara.zoom);
    printf("Celda con zoom: %dpx\n", miCamara.tamano_celda_actual);
    
    // Calcular celdas visibles actuales
    float visible_x = (float)PANTALLA_COLUMNAS / miCamara.zoom;
    float visible_y = (float)PANTALLA_FILAS / miCamara.zoom;
    printf("Celdas visibles actuales: %.1f x %.1f\n", visible_x, visible_y);
    
    printf("===============================\n");
}