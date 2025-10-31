/* src/main.c */
#include "recursos/recursos.h"
//#include "sistema/sistema.h"
#include <windows.h>
#include "mapa/mapa.h"

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

        /* Evento de Teclado */
        case WM_KEYDOWN:
        {
            if (estadoJuego.mostrarMenu) {
                // Navegación del menú con flechas
                switch (wParam)
                {
                    case VK_UP:
                        estadoJuego.opcionSeleccionada--;
                        if (estadoJuego.opcionSeleccionada < 0) {
                            estadoJuego.opcionSeleccionada = 2; // Volver a la última opción
                        }
                        InvalidateRect(hwnd, NULL, TRUE);
                        break;
                    case VK_DOWN:
                        estadoJuego.opcionSeleccionada++;
                        if (estadoJuego.opcionSeleccionada > 2) {
                            estadoJuego.opcionSeleccionada = 0; // Volver a la primera opción
                        }
                        InvalidateRect(hwnd, NULL, TRUE);
                        break;
                    case VK_RETURN:
                        procesarEnterMenu(hwnd, &estadoJuego);
                        break;
                }
            } 
            else if (estadoJuego.mostrarResumen) {
                // En resumen, solo Enter para volver
                if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
                    estadoJuego.mostrarMenu = 1;
                    estadoJuego.mostrarResumen = 0;
                    estadoJuego.opcionSeleccionada = 0;
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            }
            else if (estadoJuego.enPartida) {
                // Movimiento del jugador en el juego
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
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;
        }

        /* Evento de Dibujado */
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            if (estadoJuego.mostrarMenu) {
                dibujarMenu(hdc, hwnd, &estadoJuego);
            } else if (estadoJuego.mostrarResumen) {
                dibujarResumenRecursos(hdc, miJugador, &estadoJuego);
            } else if (estadoJuego.enPartida) {
                // Obtener el tamaño del área cliente para el mapa
                RECT rectClient;
                GetClientRect(hwnd, &rectClient);
                dibujarMapa(hdc, mapaMundo, miCamara, rectClient.right, rectClient.bottom);
                dibujarJugador(hdc, miJugador, miCamara);
            }

            EndPaint(hwnd, &ps);
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/* El punto de entrada de la aplicación */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
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
    int anchoVentana = PANTALLA_COLUMNAS * TAMANO_CELDA;  // 25 * 32 = 800
    int altoVentana = PANTALLA_FILAS * TAMANO_CELDA;      // 18 * 32 = 576
    
    // Ajustar para incluir bordes de la ventana
    RECT rect = {0, 0, anchoVentana, altoVentana};
    AdjustWindowRect(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
    
    int anchoReal = rect.right - rect.left;
    int altoReal = rect.bottom - rect.top;

    hwnd = CreateWindowEx(
        0,
        TEXT("IslasEnGuerraClase"),
        TEXT("War Islands"),
        // Quitar WS_OVERLAPPEDWINDOW y usar estilo personalizado sin redimensionar
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