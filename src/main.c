/* src/main.c */

#include "mapa/mapa.h"
#include "recursos/recursos.h"
#include "sistema/sistema.h"

/* --- Variables Globales del Juego --- */
char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];
Jugador miJugador;
Camera miCamara;


/* El cerebro de la aplicación */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        /* Evento de Teclado (¡Con WASD!) */
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_LEFT:   /* Flecha Izquierda */
                case 'A':       /* Tecla A */
                    moverJugador(&miJugador, mapaMundo, -1, 0);
                    break;
                case VK_RIGHT:  /* Flecha Derecha */
                case 'D':       /* Tecla D */
                    moverJugador(&miJugador, mapaMundo, 1, 0);
                    break;
                case VK_UP:     /* Flecha Arriba */
                case 'W':       /* Tecla W */
                    moverJugador(&miJugador, mapaMundo, 0, -1);
                    break;
                case VK_DOWN:   /* Flecha Abajo */
                case 'S':       /* Tecla S */
                    moverJugador(&miJugador, mapaMundo, 0, 1);
                    break;
            }
            
            /* ¡NUEVO! Actualizamos la cámara después de movernos */
            actualizarCamara(&miCamara, miJugador);
            
            /* Forzamos a Windows a redibujar */
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
            return 0;
        }

        /* Evento de Dibujado */
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            /* ¡NUEVO! Pasamos la cámara a las funciones de dibujado */
            dibujarMapa(hdc, mapaMundo, miCamara);
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
    /* Variables de WinMain (C89) */
    WNDCLASS wc = { 0 };
    HWND hwnd;
    MSG msg;

    /* 1. Registrar la clase de la ventana */
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = "IslasEnGuerraClase";
    RegisterClass(&wc);

    /* 2. Crear la ventana (basado en el tamaño de PANTALLA) */
    hwnd = CreateWindowEx(
        0,
        "IslasEnGuerraClase",
        "Islas en Guerra - GDI con Cámara",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        (PANTALLA_COLUMNAS * TAMANO_CELDA) + 16, /* Ancho de Ventana */
        (PANTALLA_FILAS * TAMANO_CELDA) + 39,    /* Alto de Ventana */
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    /* 3. Inicializar el estado del juego */
    inicializarMapa(mapaMundo);
    miJugador.x = 5; /* Posición inicial en el mundo */
    miJugador.y = 5;
    actualizarCamara(&miCamara, miJugador); /* Centrar cámara al inicio */

    /* 4. Bucle de mensajes (el "Game Loop") */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}