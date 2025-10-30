/* src/main.c */

#include "mapa/mapa.h"
#include "recursos/recursos.h"
#include "sistema/sistema.h"

/* --- Variables Globales del Juego --- */
char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];
Jugador miJugador;
Camera miCamara;

/* --- Variables para Pantalla Completa --- */
BOOL g_esPantallaCompleta = FALSE;
RECT g_rectVentanaAntigua;

/* Prototipo de nuestra nueva función */
void AlternarPantallaCompleta(HWND hwnd);

/* El cerebro de la aplicación */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    RECT rectCliente;
    int pantallaFilas, pantallaColumnas;

    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_LEFT:   case 'A': moverJugador(&miJugador, mapaMundo, -1, 0); break;
                case VK_RIGHT:  case 'D': moverJugador(&miJugador, mapaMundo, 1, 0); break;
                case VK_UP:     case 'W': moverJugador(&miJugador, mapaMundo, 0, -1); break;
                case VK_DOWN:   case 'S': moverJugador(&miJugador, mapaMundo, 0, 1); break;
                case VK_F11: AlternarPantallaCompleta(hwnd); break;
            }
            
            GetClientRect(hwnd, &rectCliente);
            pantallaFilas = (rectCliente.bottom / TAMANO_CELDA) + 1;
            pantallaColumnas = (rectCliente.right / TAMANO_CELDA) + 1;
            actualizarCamara(&miCamara, miJugador, pantallaFilas, pantallaColumnas);
            
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            GetClientRect(hwnd, &rectCliente);
            pantallaFilas = (rectCliente.bottom / TAMANO_CELDA) + 1;
            pantallaColumnas = (rectCliente.right / TAMANO_CELDA) + 1;

            dibujarMapa(hdc, mapaMundo, miCamara, pantallaFilas, pantallaColumnas);
            dibujarJugador(hdc, miJugador, miCamara);

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
    RECT rectCliente;
    int pantallaFilas, pantallaColumnas;

    int anchoInicial = (27 * TAMANO_CELDA);
    int altoInicial = (20 * TAMANO_CELDA);

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = "IslasEnGuerraClase";
    wc.style = CS_HREDRAW | CS_VREDRAW; 
    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0, "IslasEnGuerraClase", "Islas en Guerra",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        anchoInicial, altoInicial,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    /* 3. Inicializar el estado del juego */
    inicializarMapa(mapaMundo);
    
    /* ¡PUNTO DE SPAWN CORRECTO! */
    miJugador.x = 20; /* Columna 20 */
    miJugador.y = 17; /* Fila 17 */
    
    GetClientRect(hwnd, &rectCliente);
    pantallaFilas = (rectCliente.bottom / TAMANO_CELDA) + 1;
    pantallaColumnas = (rectCliente.right / TAMANO_CELDA) + 1;
    actualizarCamara(&miCamara, miJugador, pantallaFilas, pantallaColumnas);

    /* 4. Bucle de mensajes */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

/* --- Función de Pantalla Completa --- */
void AlternarPantallaCompleta(HWND hwnd)
{
    DWORD dwEstilo = GetWindowLong(hwnd, GWL_STYLE);
    if (g_esPantallaCompleta == FALSE)
    {
        g_esPantallaCompleta = TRUE;
        GetWindowRect(hwnd, &g_rectVentanaAntigua);
        SetWindowLong(hwnd, GWL_STYLE, (dwEstilo & ~WS_OVERLAPPEDWINDOW) | WS_POPUP);
        SetWindowPos(hwnd, HWND_TOP, 0, 0,
            GetSystemMetrics(SM_CXSCREEN),
            GetSystemMetrics(SM_CYSCREEN),
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
    else
    {
        g_esPantallaCompleta = FALSE;
        SetWindowLong(hwnd, GWL_STYLE, (dwEstilo & ~WS_POPUP) | WS_OVERLAPPEDWINDOW);
        SetWindowPos(hwnd, NULL,
            g_rectVentanaAntigua.left,
            g_rectVentanaAntigua.top,
            g_rectVentanaAntigua.right - g_rectVentanaAntigua.left,
            g_rectVentanaAntigua.bottom - g_rectVentanaAntigua.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}