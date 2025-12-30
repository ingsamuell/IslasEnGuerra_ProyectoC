/* src/main.c - VERSIÓN CORREGIDA FINAL */
#include <windows.h>
#include <stdio.h>
#include "global.h"             
#include "mapa/mapa.h"          
#include "recursos/recursos.h"  

/* ========== VARIABLES GLOBALES ========== */
char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];
Jugador miJugador;
Camera miCamara;
EstadoJuego estadoJuego;

/* ========== PROCEDIMIENTO DE VENTANA ========== */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;

        case WM_ERASEBKGND:
            return 1;

case WM_LBUTTONDOWN:
{
    int mouseX = LOWORD(lParam);
    int mouseY = HIWORD(lParam);

    if (estadoJuego.mostrarMenu) {
        procesarClickMenu(mouseX, mouseY, hwnd, &estadoJuego);
    }
    else if (estadoJuego.enPartida) {
        // 1. Primero revisamos si el clic fue en el Botón Mochila
        int btnX = 20; int btnY = 120; int tamanoBolso = 64;
        bool clicEnUI = false;

        if (mouseX >= btnX && mouseX <= btnX + tamanoBolso &&
            mouseY >= btnY && mouseY <= btnY + tamanoBolso) {
            miJugador.inventarioAbierto = !miJugador.inventarioAbierto;
            clicEnUI = true;
            InvalidateRect(hwnd, NULL, FALSE);
        }

        // 2. Procesamos clics de inventario
        procesarClickMochila(mouseX, mouseY, &miJugador, hwnd);
        procesarClickMochilaTienda(mouseX, mouseY, 0, &miJugador, hwnd);

        // 3. SI NO TOCAMOS LA UI, seleccionamos unidades en el mapa
        // Usamos una pequeña lógica para que no selecciones unidades por accidente al tocar la mochila
        if (!clicEnUI && !miJugador.inventarioAbierto) {
            seleccionarUnidades(mouseX, mouseY, miCamara);
        }
    }
    return 0;
}

        case WM_RBUTTONDOWN:
        {
            if (estadoJuego.enPartida) {
                procesarClickMochilaTienda(LOWORD(lParam), HIWORD(lParam), 1, &miJugador, hwnd);
            }
            return 0;
        }

        case WM_KEYDOWN:
        {
            if (estadoJuego.mostrarMenu) {
                if (wParam == VK_RETURN) procesarEnterMenu(hwnd, &estadoJuego);
            } 
            else if (estadoJuego.enPartida) {
                switch (wParam) {
                    case VK_ESCAPE: 
                        estadoJuego.enPartida = 0;
                        estadoJuego.mostrarMenu = 1;
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;

                    case 'W': moverJugador(&miJugador, mapaMundo, 0, -1); break;
                    case 'S': moverJugador(&miJugador, mapaMundo, 0, 1); break;
                    case 'A': moverJugador(&miJugador, mapaMundo, -1, 0); break;
                    case 'D': moverJugador(&miJugador, mapaMundo, 1, 0); break;
                    case 'I': miJugador.inventarioAbierto = !miJugador.inventarioAbierto; break;

                    case VK_ADD: case 0xBB: 
                        miCamara.zoom += 1; if (miCamara.zoom > 5) miCamara.zoom = 5; break;
                    case VK_SUBTRACT: case 0xBD: 
                        miCamara.zoom -= 1; if (miCamara.zoom < 1) miCamara.zoom = 1; break;
                        case VK_SPACE: 
                        // Intentar talar lo que esté enfrente
                        talarArbol(&miJugador);
                        abrirTesoro(&miJugador);
                        golpearVaca(&miJugador);
                        
                        // (Si implementas golpearVaca después, puedes poner ambas aquí 
                        // y el juego decidirá qué golpeó primero)
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;
                }
                actualizarCamara(&miCamara, miJugador);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

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
               // 1. EL FONDO (El mapa siempre va primero)
			dibujarMapaConZoom(hdcMem, mapaMundo, miCamara, ancho, alto, estadoJuego.frameTienda);
	dibujarTesoros(hdcMem, miCamara, ancho, alto); 
			// 2. ESTRUCTURAS FIJAS (La cueva/mina)
			dibujarMina(hdcMem, miCamara); 
dibujarEstablo(hdcMem, miCamara); // <-- NUEVA LLAMADA
		// 3. OBJETOS INTERACTIVOS (Tesoros y Árboles)
	
		dibujarArboles(hdcMem, miCamara, ancho, alto); 
			DibujarImagen(hdcMem, hBmpEstablo, 
              (ESTABLO_X - miCamara.x) * miCamara.zoom, 
              (ESTABLO_Y - miCamara.y) * miCamara.zoom, 
              200 * miCamara.zoom, 200 * miCamara.zoom);
		dibujarVacas(hdcMem, miCamara, ancho, alto);
		dibujarUnidades(hdcMem, miCamara); // Tus grupos de aldeanos/soldados
		dibujarJugador(hdcMem, miJugador, miCamara);

		// 5. INTERFAZ (HUD siempre al final para que esté arriba de todo)
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

    // Verificación de archivos de vacas (DEBUG)
    int vacasFaltan = 0;
    for(int i=0; i<8; i++) if(!hBmpVaca[i]) vacasFaltan++;
    if(vacasFaltan > 0) MessageBox(NULL, "Faltan imágenes de vacas en assets/animales/", "Error Recursos", MB_OK);

    // Inicializar lógica
    inicializarJuego(&miJugador, &estadoJuego, mapaMundo);
    
    // CORRECCIÓN: Llamadas sin argumentos o con los correctos
    inicializarVacas();           
    inicializarArboles(mapaMundo);
    inicializarTesoros(); 
    inicializarUnidades();
    

    miCamara.zoom = 3;  
    actualizarCamara(&miCamara, miJugador);

    ShowWindow(hwnd, SW_MAXIMIZE); 
    UpdateWindow(hwnd);   

    // Bucle del juego
    MSG msg = {0};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } 
        else {
            if (estadoJuego.enPartida) {
                // CORRECCIÓN: Pasar el mapa a actualizarVacas
                actualizarVacas(mapaMundo); 
                
                // Animación tienda
                static int timerGato = 0;
                if (++timerGato > 30) { 
                    estadoJuego.frameTienda = !estadoJuego.frameTienda;
                    timerGato = 0;
                }
                InvalidateRect(hwnd, NULL, FALSE);
                Sleep(16); 
            }
        }
    }

    LiberarRecursos();
    return 0;
}