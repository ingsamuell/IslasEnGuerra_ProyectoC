/* src/main.c - VERSIÓN CORREGIDA FINAL CON SELECCIÓN DE MAPA */
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

            switch(estadoJuego.estadoActual)
            {
                case ESTADO_MENU:
                    procesarClickMenu(mouseX, mouseY, hwnd, &estadoJuego);
                    break;
                    
                case ESTADO_SELECCION_MAPA:
                    procesarClickSeleccionMapa(mouseX, mouseY, hwnd, &estadoJuego);
                    break;
                    
                case ESTADO_PARTIDA:
                {
                    bool clicEnUI = false;

                    // 1. Botón Mochila (Icono del bolso a la izquierda)
                    int btnX = 20; int btnY = 120; int tamanoBolso = 64;
                    if (mouseX >= btnX && mouseX <= btnX + tamanoBolso &&
                        mouseY >= btnY && mouseY <= btnY + tamanoBolso) {
                        miJugador.inventarioAbierto = !miJugador.inventarioAbierto;
                        clicEnUI = true;
                        InvalidateRect(hwnd, NULL, FALSE);
                    }

                    // 2. Si la mochila/tienda está abierta
                    if (miJugador.inventarioAbierto) {
                        int tx = 450; int ty = 120;

                        // DETECTAR CLIC EN PESTAÑAS (COMPRAR / VENDER)
                        if (mouseY >= ty - 40 && mouseY <= ty) {
                            if (mouseX >= tx && mouseX <= tx + 150) {
                                miJugador.modoTienda = 0; // Pestaña Comprar
                                clicEnUI = true;
                                InvalidateRect(hwnd, NULL, FALSE);
                            }
                            else if (mouseX >= tx + 150 && mouseX <= tx + 300) {
                                miJugador.modoTienda = 1; // Pestaña Vender
                                clicEnUI = true;
                                InvalidateRect(hwnd, NULL, FALSE);
                            }
                        }

                        // 3. Procesar Contenido de la Tienda
                        if (!clicEnUI) {
                            procesarClickMochilaTienda(mouseX, mouseY, FALSE, &miJugador, hwnd);
                            
                            if (mouseX >= tx && mouseX <= tx + 300 && mouseY >= ty - 40 && mouseY <= ty + 350) {
                                clicEnUI = true;
                            }
                        }
                        
                        procesarClickMochila(mouseX, mouseY, &miJugador, hwnd);
                    }

                    // 4. Selección de unidades en el mapa
                    if (!clicEnUI) {
                        seleccionarUnidades(mouseX, mouseY, miCamara);
                    }
                    break;
                }
            }
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            if (estadoJuego.estadoActual == ESTADO_PARTIDA) {
                procesarClickMochilaTienda(LOWORD(lParam), HIWORD(lParam), 1, &miJugador, hwnd);
            }
            return 0;
        }

        case WM_KEYDOWN:
        {
            switch(estadoJuego.estadoActual)
            {
                case ESTADO_MENU:
                    if (wParam == VK_RETURN) 
                        procesarEnterMenu(hwnd, &estadoJuego);
                    break;
                    
                case ESTADO_SELECCION_MAPA:
                    if (wParam == VK_RETURN)
                    {
                        // Confirmar selección de mapa y empezar juego
                        estadoJuego.mapaSeleccionado = estadoJuego.opcionSeleccionada;
                        
                        // Inicializar juego con el mapa seleccionado
                        inicializarJuego(&miJugador, &estadoJuego, mapaMundo, estadoJuego.mapaSeleccionado);
                        
                        // Inicializar estadísticas del jugador
                        miJugador.nivelMochila = 1;
                        miJugador.cantMineros = 0;
                        miJugador.cantLenadores = 0;
                        miJugador.cantCazadores = 0;
                        miJugador.tienePico = FALSE;
                        miJugador.tieneHacha = FALSE;
                        miJugador.tieneEspada = FALSE;
                        
                        miCamara.zoom = 3;
                        actualizarCamara(&miCamara, miJugador);
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    else if (wParam == VK_ESCAPE)
                    {
                        // Volver al menú
                        estadoJuego.estadoActual = ESTADO_MENU;
                        estadoJuego.opcionSeleccionada = 0;
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    break;
                    
                case ESTADO_PARTIDA:
                    switch (wParam) {
                        case VK_ESCAPE: 
                            // Volver a selección de mapa
                            estadoJuego.estadoActual = ESTADO_SELECCION_MAPA;
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
                            talarArbol(&miJugador);
                            abrirTesoro(&miJugador);
                            golpearVaca(&miJugador);
                            picarMina(&miJugador);
                            InvalidateRect(hwnd, NULL, FALSE);
                            break;
                    }
                    actualizarCamara(&miCamara, miJugador);
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
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
            HGDIOBJ hbmOld = SelectObject(hdcMem, hbmMem);

            switch(estadoJuego.estadoActual)
            {
                case ESTADO_MENU:
                    dibujarMenuConSprites(hdcMem, hwnd, &estadoJuego);
                    break;
                    
                case ESTADO_SELECCION_MAPA:
                    dibujarSeleccionMapa(hdcMem, hwnd, &estadoJuego);
                    break;
                    
                case ESTADO_PARTIDA:
                    // 1. EL FONDO
                    dibujarMapaConZoom(hdcMem, mapaMundo, miCamara, ancho, alto, estadoJuego.frameTienda, estadoJuego.mapaSeleccionado);
                    dibujarTesoros(hdcMem, miCamara, ancho, alto); 
                    
                    // 2. ESTRUCTURAS FIJAS
                    dibujarMina(hdcMem, miCamara); 
                    dibujarMinas(hdcMem, miCamara, ancho, alto); 
                    actualizarYDibujarParticulas(hdcMem, miCamara);
                    dibujarEstablo(hdcMem, miCamara);
                    
                    // 3. OBJETOS INTERACTIVOS
                    //dibujarArboles(hdcMem, miCamara, ancho, alto, estadoJuego.mapaSeleccionado);
                    
                    DibujarImagen(hdcMem, hBmpEstablo, 
                          (ESTABLO_X - miCamara.x) * miCamara.zoom, 
                          (ESTABLO_Y - miCamara.y) * miCamara.zoom, 
                          200 * miCamara.zoom, 200 * miCamara.zoom);

                    dibujarVacas(hdcMem, miCamara, ancho, alto);
                    dibujarUnidades(hdcMem, miCamara);
                    dibujarJugador(hdcMem, miJugador, miCamara);
                    actualizarYDibujarTextos(hdcMem, miCamara);

                    // 4. INTERFAZ
                    dibujarHUD(hdcMem, &miJugador, ancho, alto);
                    break;
            }

            // Copiar todo el dibujo procesado de golpe a la pantalla
            BitBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, SRCCOPY);

            // Limpieza
            SelectObject(hdcMem, hbmOld); 
            DeleteObject(hbmMem); 
            DeleteDC(hdcMem);
            
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

    // Estado inicial del juego
    estadoJuego.estadoActual = ESTADO_MENU;
    estadoJuego.opcionSeleccionada = 0;  // Por defecto selecciona el primer mapa
    estadoJuego.mapaSeleccionado = MAPA_ISLAS_OCEANO;
    estadoJuego.frameTienda = 0;

    // NOTA: No inicializamos el juego aquí todavía, solo cuando se selecciona un mapa
    
    miCamara.zoom = 3;  

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
            if (estadoJuego.estadoActual == ESTADO_PARTIDA) {
                // Actualizar sistemas del juego
                actualizarVacas(mapaMundo); 
                actualizarLogicaSistema();
                actualizarUnidades(mapaMundo, &miJugador);
                
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