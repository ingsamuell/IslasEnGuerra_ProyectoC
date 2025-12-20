/* src/main.c - CÓDIGO CORREGIDO Y LIMPIO */
#include <windows.h>
#include <stdio.h>
#include "global.h"             // Estructuras del jugador y juego
#include "mapa/mapa.h"          // Funciones del mapa
#include "recursos/recursos.h"  

/* ========== VARIABLES GLOBALES ========== */
char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];
Jugador miJugador;
Camera miCamara;
EstadoJuego estadoJuego;

/* ========== PROCEDIMIENTO DE VENTANA (Eventos) ========== */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;

        /* Evitar parpadeo de pantalla */
        case WM_ERASEBKGND:
            return 1;

        /* --- NUEVO: CONTROLES DE RATÓN (CLICS) --- */
        case WM_LBUTTONDOWN:
        {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);

            // 1. Lógica del Menú
            if (estadoJuego.mostrarMenu) {
                procesarClickMenu(mouseX, mouseY, hwnd, &estadoJuego);
            }
            // 2. Lógica del Juego (Mochila)
            else if (estadoJuego.enPartida) {
                
                // --- NUEVAS COORDENADAS DEL BOTÓN (Deben coincidir con dibujarHUD) ---
                // Ya no necesitamos GetClientRect porque la posición es fija arriba.
                
                int tamanoBolso = 64;
                int btnX = 20;   // Misma X que Vida/Escudo
                int btnY = 140;  // Debajo del Escudo

                // Detectar colisión en la nueva zona
                if (mouseX >= btnX && mouseX <= btnX + tamanoBolso &&
                    mouseY >= btnY && mouseY <= btnY + tamanoBolso) {
                    
                    // Alternar inventario
                    miJugador.inventarioAbierto = !miJugador.inventarioAbierto;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
        }

        /* --- CONTROLES (TECLADO) --- */
        case WM_KEYDOWN:
        {
            /* 1. MENÚ PRINCIPAL */
            if (estadoJuego.mostrarMenu) {
                switch (wParam) {
                    case VK_UP:
                        estadoJuego.opcionSeleccionada--;
                        if (estadoJuego.opcionSeleccionada < 0) estadoJuego.opcionSeleccionada = 2;
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;
                    case VK_DOWN:
                        estadoJuego.opcionSeleccionada++;
                        if (estadoJuego.opcionSeleccionada > 2) estadoJuego.opcionSeleccionada = 0;
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;
                    case VK_RETURN:
                        procesarEnterMenu(hwnd, &estadoJuego);
                        break;
                    case VK_ESCAPE:
                        PostQuitMessage(0);
                        break;
                }
            } 
            /* 2. INVENTARIO (RESUMEN) */
            else if (estadoJuego.mostrarResumen) {
                // Con cualquier tecla cerramos el inventario
                if (wParam == VK_ESCAPE || wParam == 'I' || wParam == VK_RETURN) {
                     estadoJuego.mostrarResumen = 0;
                     miJugador.inventarioAbierto = 0;
                     InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            /* 3. JUGANDO (EN PARTIDA) */
            else if (estadoJuego.enPartida) {
                switch (wParam) {
                    // Movimiento WASD y Flechas
                    case VK_LEFT:  case 'A': moverJugador(&miJugador, mapaMundo, -1, 0); break;
                    case VK_RIGHT: case 'D': moverJugador(&miJugador, mapaMundo, 1, 0); break;
                    case VK_UP:    case 'W': moverJugador(&miJugador, mapaMundo, 0, -1); break;
                    case VK_DOWN:  case 'S': moverJugador(&miJugador, mapaMundo, 0, 1); break;
                    
                    // Abrir Inventario con tecla
                    case 'I': 
                        miJugador.inventarioAbierto = !miJugador.inventarioAbierto; // Alternar
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;

                    // Zoom (+ y -)
                    case VK_ADD: case 0xBB:
                        if (miCamara.zoom < 4.0f) miCamara.zoom += 0.5f;
                        actualizarCamara(&miCamara, miJugador);
                        InvalidateRect(hwnd, NULL, TRUE);
                        break;
                    case VK_SUBTRACT: case 0xBD:
                        if (miCamara.zoom > 0.5f) miCamara.zoom -= 0.5f;
                        actualizarCamara(&miCamara, miJugador);
                        InvalidateRect(hwnd, NULL, TRUE);
                        break;

                    // Pausa / Volver al menú
                    case VK_ESCAPE:
                        estadoJuego.mostrarMenu = 1;
                        estadoJuego.enPartida = 0;
                        InvalidateRect(hwnd, NULL, TRUE);
                        break;
                }
                
                // Actualizar cámara si nos movimos
                if (wParam == 'A' || wParam == 'D' || wParam == 'W' || wParam == 'S' ||
                    wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_UP || wParam == VK_DOWN) {
                    actualizarCamara(&miCamara, miJugador);
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
        }

        /* --- DIBUJADO (RENDER) --- */
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // --- DOBLE BUFFER ---
            HDC hdcMem = CreateCompatibleDC(hdc);
            RECT rect;
            GetClientRect(hwnd, &rect);
            int ancho = rect.right;
            int alto = rect.bottom;
            
            HBITMAP hbmMem = CreateCompatibleBitmap(hdc, ancho, alto);
            HBITMAP hbmOld = SelectObject(hdcMem, hbmMem);

            // --- DIBUJAR ---
            if (estadoJuego.mostrarMenu) {
                dibujarMenuConSprites(hdcMem, hwnd, &estadoJuego);
            } else {
                // 1. DIBUJAR MUNDO (Primero)
                dibujarMapaConZoom(hdcMem, mapaMundo, miCamara, ancho, alto);
                
                // 2. DIBUJAR JUGADOR (¡AQUÍ ESTÁ LA CLAVE!)
                // Asegúrate de que esta línea exista y esté DESPUÉS del mapa
                dibujarJugador(hdcMem, miJugador, miCamara);
                
                // 3. DIBUJAR HUD (Al final, encima de todo)
                dibujarHUD(hdcMem, &miJugador, ancho, alto);
            }

            // Copiar a pantalla
            BitBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, SRCCOPY);

            // Limpieza
            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SIZE:
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/* ========== PUNTO DE ENTRADA PRINCIPAL (MAIN) ========== */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 1. Configurar la Clase de Ventana
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = "ClaseIslasGuerra";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    
    if (!RegisterClass(&wc)) return -1;

    // 2. OBTENER RESOLUCIÓN DE TU PANTALLA
    int anchoPantalla = GetSystemMetrics(SM_CXSCREEN);
    int altoPantalla  = GetSystemMetrics(SM_CYSCREEN);

    // 3. CREAR VENTANA "POPUP" (INVISIBLE AL PRINCIPIO)
    HWND hwnd = CreateWindow("ClaseIslasGuerra", "Islas en Guerra",
                             WS_OVERLAPPEDWINDOW, // <--- CAMBIO CLAVE
                             CW_USEDEFAULT, CW_USEDEFAULT, // Dejamos que Windows decida la posición inicial
                             anchoPantalla, altoPantalla,  // Tamaño inicial
                             NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) return 0;

    // 4. CARGAR RECURSOS (Imágenes, sonidos...)
    CargarRecursos(); 

    // 5. INICIALIZAR DATOS
    // ¡AQUÍ ESTÁ LA MAGIA!
    // Esta función ya pone al jugador en (1600, 1600), le da vida, oro y carga las islas.
    // No hace falta poner nada más aquí.
    inicializarJuego(&miJugador, &estadoJuego, mapaMundo);
    
    // 6. CONFIGURAR CÁMARA INICIAL
    // Queremos empezar con Zoom para ver bien los detalles
    miCamara.zoom = 2.0f;  // <--- ZOOM INICIAL (2x)

    // Sincronizar cámara con jugador (que ya está en 1600,1600 gracias a la función de arriba)
    miCamara.x = miJugador.x;
    miCamara.y = miJugador.y;
    actualizarCamara(&miCamara, miJugador);

    // 7. MOSTRAR LA VENTANA 
    ShowWindow(hwnd, SW_MAXIMIZE); 
    UpdateWindow(hwnd);   

    // 8. BUCLE DEL JUEGO
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    LiberarRecursos();
    
    return 0;
}