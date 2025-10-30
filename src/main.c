/* src/main.c */

#include "global.h"
#include "mapa/mapa.h"
#include "recursos/recursos.h"
#include "sistema/sistema.h"

/* --- Variables Globales del Juego --- */
char mapaJuego[FILAS][COLUMNAS];
Jugador miJugador;


/* Esta función maneja TODOS los eventos de la ventana */
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
            switch (wParam)
            {
                case VK_LEFT:
                    moverJugador(&miJugador, mapaJuego, -1, 0);
                    break;
                case VK_RIGHT:
                    moverJugador(&miJugador, mapaJuego, 1, 0);
                    break;
                case VK_UP:
                    moverJugador(&miJugador, mapaJuego, 0, -1);
                    break;
                case VK_DOWN:
                    moverJugador(&miJugador, mapaJuego, 0, 1);
                    break;
            }
            
            /* Forzamos a Windows a redibujar la pantalla */
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
            return 0;
        }

        /* Evento de Dibujado (el más importante) */
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps); /* Obtenemos el "lienzo" */

            /* Llamamos a nuestras funciones de dibujado */
            dibujarMapa(hdc, mapaJuego);
            dibujarJugador(hdc, miJugador);

            EndPaint(hwnd, &ps); /* Soltamos el "lienzo" */
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

    /* 1. Registrar la clase de la ventana */
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = "IslasEnGuerraClase";
    RegisterClass(&wc);

    /* 2. Crear la ventana */
    /* (Calculamos el tamaño de la ventana + bordes) */
    hwnd = CreateWindowEx(
        0,
        "IslasEnGuerraClase",
        "Islas en Guerra - GDI",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        (COLUMNAS * TAMANO_CELDA) + 16, /* Ancho + bordes */
        (FILAS * TAMANO_CELDA) + 39,    /* Alto + bordes */
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    /* 3. Inicializar el estado del juego */
    inicializarMapa(mapaJuego);
    miJugador.x = 1;
    miJugador.y = 1;

    /* 4. Bucle de mensajes (el "Game Loop") */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}