/* src/main.c - VERSIÓN CORREGIDA Y COMPATIBLE */
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

        /* --- CLIC IZQUIERDO --- */
        case WM_LBUTTONDOWN:
        {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);

            if (estadoJuego.mostrarMenu) {
                procesarClickMenu(mouseX, mouseY, hwnd, &estadoJuego);
            }
            else if (estadoJuego.enPartida) {
                // 1. Botón Mochila
                int btnX = 20; int btnY = 120; int tamanoBolso = 64;
                if (mouseX >= btnX && mouseX <= btnX + tamanoBolso &&
                    mouseY >= btnY && mouseY <= btnY + tamanoBolso) {
                    miJugador.inventarioAbierto = !miJugador.inventarioAbierto;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                // 2. Interacciones
                procesarClickMochila(mouseX, mouseY, &miJugador, hwnd);
                procesarClickMochilaTienda(mouseX, mouseY, 0, &miJugador, hwnd);
            }
            return 0;
        }

        /* --- CLIC DERECHO --- */
        case WM_RBUTTONDOWN:
        {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);
            if (estadoJuego.enPartida) {
                procesarClickMochilaTienda(mouseX, mouseY, 1, &miJugador, hwnd);
            }
            return 0;
        }

        /* --- TECLADO --- */
        case WM_KEYDOWN:
        {
            if (estadoJuego.mostrarMenu) {
                if (wParam == VK_RETURN) procesarEnterMenu(hwnd, &estadoJuego);
            } 
            else if (estadoJuego.enPartida) {
                switch (wParam) {
                    case VK_ESCAPE: // Salir al menú
                        estadoJuego.enPartida = 0;
                        estadoJuego.mostrarMenu = 1;
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;

                    // Movimiento
                    case 'W': moverJugador(&miJugador, mapaMundo, 0, -1); break;
                    case 'S': moverJugador(&miJugador, mapaMundo, 0, 1); break;
                    case 'A': moverJugador(&miJugador, mapaMundo, -1, 0); break;
                    case 'D': moverJugador(&miJugador, mapaMundo, 1, 0); break;
                    case 'I': miJugador.inventarioAbierto = !miJugador.inventarioAbierto; break;

                    // Zoom
                    case VK_ADD: case 0xBB: // '+'
                        miCamara.zoom += 1; if (miCamara.zoom > 5) miCamara.zoom = 5; break;
                    case VK_SUBTRACT: case 0xBD: // '-'
                        miCamara.zoom -= 1; if (miCamara.zoom < 1) miCamara.zoom = 1; break;

                    case VK_SPACE: 
                        // golpearVaca(&miJugador); // <-- AÚN NO CREADA EN MAPA.C (Comentada para que compile)
                        break;
                }
                actualizarCamara(&miCamara, miJugador);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        /* --- DIBUJADO --- */
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rc; GetClientRect(hwnd, &rc);
            int ancho = rc.right; int alto = rc.bottom;
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmMem = CreateCompatibleBitmap(hdc, ancho, alto);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

            if (estadoJuego.mostrarMenu) {
                dibujarMenuConSprites(hdcMem, hwnd, &estadoJuego);
            } 
            else if (estadoJuego.enPartida) {
                // Dibujamos capas en orden:
                dibujarMapaConZoom(hdcMem, mapaMundo, miCamara, ancho, alto, estadoJuego.frameTienda);
                dibujarArboles(hdcMem, miCamara, ancho, alto); 
                dibujarVacas(hdcMem, miCamara, ancho, alto); 
                dibujarJugador(hdcMem, miJugador, miCamara);
                dibujarHUD(hdcMem, &miJugador, ancho, alto);
            }

            BitBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld); DeleteObject(hbmMem); DeleteDC(hdcMem);
            EndPaint(hwnd, &ps);
            return 0;
        }
        default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/* ========== MAIN ========== */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = "ClaseIslasGuerra";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    if (!RegisterClass(&wc)) return -1;

    int ancho = GetSystemMetrics(SM_CXSCREEN);
    int alto  = GetSystemMetrics(SM_CYSCREEN);
    HWND hwnd = CreateWindow("ClaseIslasGuerra", "Islas en Guerra", WS_OVERLAPPEDWINDOW, 
                             CW_USEDEFAULT, CW_USEDEFAULT, ancho, alto, NULL, NULL, hInstance, NULL);

    if (!hwnd) return 0;

    CargarRecursos(); 

    // --- DEBUG DE RECURSOS ---
    {
        int faltan = 0;
        char msg[512] = "";
        for (int i = 0; i < 8; i++) {
            if (!hBmpVaca[i]) {
                char tmp[64]; sprintf(tmp, "Falta hBmpVaca[%d]\n", i);
                strcat(msg, tmp); faltan++;
            }
        }
        if (faltan) MessageBox(NULL, msg, "Recursos faltantes: vacas", MB_OK | MB_ICONWARNING);
    }

    // Inicializar lógica
    inicializarJuego(&miJugador, &estadoJuego, mapaMundo);
    
    // NOTA: inicializarJuego ya llama internamente a vacas y árboles en mapa.c
    // Si quieres reiniciarlas aquí explícitamente:
    inicializarVacas();       // <--- CORREGIDO: Sin argumentos
    inicializarArboles(mapaMundo); 

    miCamara.zoom = 3;  
    actualizarCamara(&miCamara, miJugador);

    ShowWindow(hwnd, SW_MAXIMIZE); 
    UpdateWindow(hwnd);   

    // --- BUCLE DE JUEGO (GAME LOOP) ---
    MSG msg = {0};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } 
        else {
            if (estadoJuego.enPartida) {
                // LÓGICA CONTINUA
                actualizarVacas(mapaMundo); // <--- CORREGIDO: Pasamos el mapa para colisiones
                
                // Animación Gato
                static int timerGato = 0;
                if (++timerGato > 30) { 
                    estadoJuego.frameTienda = !estadoJuego.frameTienda;
                    timerGato = 0;
                }

                InvalidateRect(hwnd, NULL, FALSE);
                Sleep(16); // ~60 FPS
            }
        }
    }

    LiberarRecursos();
    return 0;
}