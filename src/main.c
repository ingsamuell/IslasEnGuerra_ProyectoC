/* src/main.c */
//#include "sistema/sistema.h"
/* src/main.c */
#include <windows.h>
#include <stdio.h>
#include "mapa/mapa.h"
#include "recursos/recursos.h"

/* --- Variables Globales del Juego --- */
char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];
Jugador miJugador;
Camera miCamara;
EstadoJuego estadoJuego;

/* El cerebro de la aplicación */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        /* Evitar que Windows borre el fondo (causa parpadeo) */
        case WM_ERASEBKGND:
            return 1;

        /* Evento de Teclado */
        case WM_KEYDOWN:
        {
            if (estadoJuego.mostrarMenu) {
                switch (wParam)
                {
                    case VK_UP:
                        estadoJuego.opcionSeleccionada--;
                        if (estadoJuego.opcionSeleccionada < 0) {
                            estadoJuego.opcionSeleccionada = 2;
                        }
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;
                    case VK_DOWN:
                        estadoJuego.opcionSeleccionada++;
                        if (estadoJuego.opcionSeleccionada > 2) {
                            estadoJuego.opcionSeleccionada = 0;
                        }
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;
                    case VK_RETURN:
                        procesarEnterMenu(hwnd, &estadoJuego);
                        break;
                }
            } 
            else if (estadoJuego.mostrarResumen) {
                if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
                    estadoJuego.mostrarMenu = 1;
                    estadoJuego.mostrarResumen = 0;
                    estadoJuego.opcionSeleccionada = 0;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            else if (estadoJuego.enPartida) {
                switch (wParam)
                {
                    case VK_LEFT:
                    case 'A':
                        moverJugador(&miJugador, mapaMundo, -1, 0);
                        break;
                    case VK_RIGHT:
                    case 'D':
                        moverJugador(&miJugador, mapaMundo, 1, 0);
                        break;
                    case VK_UP:
                    case 'W':
                        moverJugador(&miJugador, mapaMundo, 0, -1);
                        break;
                    case VK_DOWN:
                    case 'S':
                        moverJugador(&miJugador, mapaMundo, 0, 1);
                        break;
                    case VK_ESCAPE:
                        estadoJuego.mostrarMenu = 1;
                        estadoJuego.enPartida = 0;
                        estadoJuego.mostrarResumen = 0;
                        estadoJuego.opcionSeleccionada = 0;
                        break;
                }
                
                actualizarCamara(&miCamara, miJugador);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        /* Evento de Dibujado CON DOBLE BÚFER */
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rectClient;
            GetClientRect(hwnd, &rectClient);
            int ancho = rectClient.right - rectClient.left;
            int alto = rectClient.bottom - rectClient.top;
            
            // CREAR BÚFER EN MEMORIA
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmMem = CreateCompatibleBitmap(hdc, ancho, alto);
            HBITMAP hbmOld = SelectObject(hdcMem, hbmMem);
            
            // DIBUJAR TODO EN EL BÚFER
            if (estadoJuego.mostrarMenu) {
                dibujarMenuConSprites(hdcMem, hwnd, &estadoJuego);  // Nuevo menú con sprites
            } else if (estadoJuego.mostrarResumen) {
                dibujarResumenRecursos(hdcMem, miJugador, &estadoJuego);
            } else if (estadoJuego.enPartida) {
                dibujarMapa(hdcMem, mapaMundo, miCamara, ancho, alto);
                dibujarJugador(hdcMem, miJugador, miCamara);
            }
            
            // COPIAR BÚFER COMPLETO A PANTALLA (INSTANTÁNEO)
            BitBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, SRCCOPY);
            
            // LIMPIAR RECURSOS DEL BÚFER
            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);
            
            EndPaint(hwnd, &ps);
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/* El punto de entrada de la aplicación */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // CONSOLA PARA DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    
    printf("=== WAR ISLANDS INICIADO (CON DOBLE BÚFER) ===\n");

    WNDCLASS wc = { 0 };
    HWND hwnd;
    MSG msg;

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = TEXT("IslasEnGuerraClase");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    // Calcular tamaño fijo de la ventana para el mapa
    int anchoVentana = PANTALLA_COLUMNAS * TAMANO_CELDA;
    int altoVentana = PANTALLA_FILAS * TAMANO_CELDA;
    
    RECT rect = {0, 0, anchoVentana, altoVentana};
    AdjustWindowRect(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
    
    int anchoReal = rect.right - rect.left;
    int altoReal = rect.bottom - rect.top;

    hwnd = CreateWindowEx(
        0,
        TEXT("IslasEnGuerraClase"),
        TEXT("War Islands"),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        anchoReal, altoReal,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    inicializarJuego(&miJugador, &estadoJuego, mapaMundo);

    if (!cargar_sprites()) {
        MessageBox(NULL, TEXT("Error cargando sprite del jugador"), TEXT("Error"), MB_OK);
        return 1;
    }

    actualizarCamara(&miCamara, miJugador);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    liberar_sprites();

    return (int)msg.wParam;
}