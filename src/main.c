/* src/main.c  */
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

        /* --- CLIC IZQUIERDO (Comprar, Comer, Abrir Bolsa, Menú) --- */
        case WM_LBUTTONDOWN:
        {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);

            // A. Lógica del Menú Principal
            if (estadoJuego.mostrarMenu) {
                procesarClickMenu(mouseX, mouseY, hwnd, &estadoJuego);
            }
            // B. Lógica del Juego
            else if (estadoJuego.enPartida) {
                
                // 1. DETECTAR BOTÓN MOCHILA (Para Abrir/Cerrar)
                int btnX = 20;   
                int btnY = 120;  
                int tamanoBolso = 64;

                if (mouseX >= btnX && mouseX <= btnX + tamanoBolso &&
                    mouseY >= btnY && mouseY <= btnY + tamanoBolso) {
                    miJugador.inventarioAbierto = !miJugador.inventarioAbierto;
                    InvalidateRect(hwnd, NULL, FALSE);
                }

                // 2. DETECTAR CLICS DENTRO DE LA MOCHILA (Comer)
                procesarClickMochila(mouseX, mouseY, &miJugador, hwnd);

                // 3. DETECTAR TIENDA (Comprar / Vender-1)
                procesarClickMochilaTienda(mouseX, mouseY, 0, &miJugador, hwnd);
            }
            return 0;
        }

        /* --- CLIC DERECHO (Solo para Vender en lote) --- */
        case WM_RBUTTONDOWN:
        {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);

            if (estadoJuego.enPartida) {
                // El '1' indica Clic Derecho (Vender 10)
                procesarClickMochilaTienda(mouseX, mouseY, 1, &miJugador, hwnd);
            }
            return 0;
        }

        /* --- TECLADO (Movimiento, Zoom y ESCAPE) --- */
        case WM_KEYDOWN:
        {
            // A. MENÚ
            if (estadoJuego.mostrarMenu) {
                if (wParam == VK_RETURN) procesarEnterMenu(hwnd, &estadoJuego);
            } 
            // B. EN PARTIDA
            else if (estadoJuego.enPartida) {
                switch (wParam) {
                    // --- SALIR AL MENÚ ---
                    case VK_ESCAPE:
                        estadoJuego.enPartida = 0;
                        estadoJuego.mostrarMenu = 1;
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;

                    // --- MOVIMIENTO ---
                    case 'W': moverJugador(&miJugador, mapaMundo, 0, -1); break;
                    case 'S': moverJugador(&miJugador, mapaMundo, 0, 1); break;
                    case 'A': moverJugador(&miJugador, mapaMundo, -1, 0); break;
                    case 'D': moverJugador(&miJugador, mapaMundo, 1, 0); break;
                    case 'I': miJugador.inventarioAbierto = !miJugador.inventarioAbierto; break;

                    // --- ZOOM (Teclas + y -) ---
                    case VK_ADD:      // Tecla '+' (NumPad)
                    case 0xBB:        // Tecla '+' (Teclado estándar)
                        miCamara.zoom += 1; 
                        if (miCamara.zoom > 5) miCamara.zoom = 5; 
                        break;

                    case VK_SUBTRACT: // Tecla '-' (NumPad)
                    case 0xBD:        // Tecla '-' (Teclado estándar)
                        miCamara.zoom -= 1; 
                        if (miCamara.zoom < 1) miCamara.zoom = 1; 
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

            // Doble Buffering
            RECT rc; GetClientRect(hwnd, &rc);
            int ancho = rc.right; int alto = rc.bottom;
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmMem = CreateCompatibleBitmap(hdc, ancho, alto);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

            if (estadoJuego.mostrarMenu) {
                dibujarMenuConSprites(hdcMem, hwnd, &estadoJuego);
            } 
            else if (estadoJuego.enPartida) {
                dibujarMapaConZoom(hdcMem, mapaMundo, miCamara, ancho, alto, estadoJuego.frameTienda);
                dibujarJugador(hdcMem, miJugador, miCamara);
                dibujarHUD(hdcMem, &miJugador, ancho, alto);
            }

            BitBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);
            EndPaint(hwnd, &ps);
            return 0;
        }

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
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

    // 2. OBTENER RESOLUCIÓN
    int anchoPantalla = GetSystemMetrics(SM_CXSCREEN);
    int altoPantalla  = GetSystemMetrics(SM_CYSCREEN);

    // 3. CREAR VENTANA
    HWND hwnd = CreateWindow("ClaseIslasGuerra", "Islas en Guerra",
                             WS_OVERLAPPEDWINDOW, 
                             CW_USEDEFAULT, CW_USEDEFAULT, 
                             anchoPantalla, altoPantalla,  
                             NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) return 0;

    // 4. CARGAR RECURSOS
    CargarRecursos(); 

    // 5. INICIALIZAR DATOS
    inicializarJuego(&miJugador, &estadoJuego, mapaMundo);
    
    // 6. CONFIGURAR CÁMARA INICIAL (ZOOM x3)
    miCamara.zoom = 3;  

    // Sincronizar cámara
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