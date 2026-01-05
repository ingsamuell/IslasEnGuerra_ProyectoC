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
int mouseActualX, mouseActualY; // Posición actual mientras arrastras
bool dibujandoCuadro = false;
int mouseStartX, mouseStartY;   // Punto donde hiciste clic
TextoFlotante textos[20];
Arbol arboles[MAX_ARBOLES];
Mina minas[MAX_MINAS];

TextoFlotante textos[MAX_TEXTOS];

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

            if (estadoJuego.estadoActual == ESTADO_PARTIDA) {
                // Iniciar cuadro de selección
                mouseStartX = mouseX;
                mouseStartY = mouseY;
                mouseActualX = mouseX;
                mouseActualY = mouseY;
                dibujandoCuadro = true;

                // Si no presionas SHIFT, deseleccionamos todo lo anterior
                if (!(wParam & MK_SHIFT)) {
                    for(int i=0; i<MAX_UNIDADES; i++) unidades[i].seleccionado = 0;
                }

                // Lógica de UI (Mochila/Botones)
                bool clicEnUI = false;
                int btnX = 20; int btnY = 120; int tamanoBolso = 64;
                if (mouseX >= btnX && mouseX <= btnX + tamanoBolso &&
                    mouseY >= btnY && mouseY <= btnY + tamanoBolso) {
                    miJugador.inventarioAbierto = !miJugador.inventarioAbierto;
                    clicEnUI = true;
                }

                if (miJugador.inventarioAbierto && !clicEnUI) {
                    procesarClickMochilaTienda(mouseX, mouseY, FALSE, &miJugador, hwnd);
                    procesarClickMochila(mouseX, mouseY, &miJugador, hwnd);
                }
            } else if (estadoJuego.estadoActual == ESTADO_MENU) {
                procesarClickMenu(mouseX, mouseY, hwnd, &estadoJuego);
            } else if (estadoJuego.estadoActual == ESTADO_SELECCION_MAPA) {
                procesarClickSeleccionMapa(mouseX, mouseY, hwnd, &estadoJuego);
            }
            
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (dibujandoCuadro && estadoJuego.estadoActual == ESTADO_PARTIDA) {
                mouseActualX = LOWORD(lParam);
                mouseActualY = HIWORD(lParam);
                InvalidateRect(hwnd, NULL, FALSE); 
            }
            return 0;
        }

        case WM_LBUTTONUP:
        {
            if (dibujandoCuadro && estadoJuego.estadoActual == ESTADO_PARTIDA) {
                dibujandoCuadro = false;
                int mouseEndX = LOWORD(lParam);
                int mouseEndY = HIWORD(lParam);

                // Normalizar el rectángulo de selección
                int xMin = min(mouseStartX, mouseEndX);
                int xMax = max(mouseStartX, mouseEndX);
                int yMin = min(mouseStartY, mouseEndY);
                int yMax = max(mouseStartY, mouseEndY);

                // Si el cuadro es muy pequeño, tratarlo como un click simple
                bool esClickSimple = (xMax - xMin < 5 && yMax - yMin < 5);

                for(int i=0; i<MAX_UNIDADES; i++) {
                    if(!unidades[i].activa) continue;

                    // Posición de unidad en pantalla
                    int ux = (int)((unidades[i].x - miCamara.x) * miCamara.zoom);
                    int uy = (int)((unidades[i].y - miCamara.y) * miCamara.zoom);
                    int size = (int)(32 * miCamara.zoom);

                    if(esClickSimple) {
                        if(mouseEndX >= ux && mouseEndX <= ux + size && mouseEndY >= uy && mouseEndY <= uy + size)
                            unidades[i].seleccionado = 1;
                    } else {
                        if(ux >= xMin && ux <= xMax && uy >= yMin && uy <= yMax)
                            unidades[i].seleccionado = 1;
                    }
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            if (estadoJuego.estadoActual == ESTADO_PARTIDA) {
                int mX = LOWORD(lParam);
                int mY = HIWORD(lParam);
                
                // 1. Procesar venta si la tienda está abierta
                procesarClickMochilaTienda(mX, mY, 1, &miJugador, hwnd);

                // 2. Dar orden a unidades seleccionadas (Movimiento o Caza)
                ordenarUnidad(mX, mY, miCamara, mapaMundo);
                
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
            HGDIOBJ hbmOld = SelectObject(hdcMem, hbmMem);

            if (estadoJuego.estadoActual == ESTADO_MENU) {
                dibujarMenuConSprites(hdcMem, hwnd, &estadoJuego);
            } 
            else if (estadoJuego.estadoActual == ESTADO_SELECCION_MAPA) {
                dibujarSeleccionMapa(hdcMem, hwnd, &estadoJuego);
            } 
            else if (estadoJuego.estadoActual == ESTADO_PARTIDA) {
                // Dibujar el mundo
                dibujarMapaConZoom(hdcMem, mapaMundo, miCamara, ancho, alto, estadoJuego.frameTienda, estadoJuego.mapaSeleccionado);
                dibujarTesoros(hdcMem, miCamara, ancho, alto); 
                dibujarMinas(hdcMem, miCamara, ancho, alto); 
                actualizarYDibujarParticulas(hdcMem, miCamara);
                dibujarEstablo(hdcMem, miCamara);
                dibujarVacas(hdcMem, miCamara, ancho, alto);
                dibujarUnidades(hdcMem, miCamara);
                dibujarJugador(hdcMem, miJugador, miCamara);
                actualizarYDibujarTextos(hdcMem, miCamara);

                // Dibujar el cuadro verde de selección (HUD dinámico)
                if (dibujandoCuadro) {
                    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
                    SelectObject(hdcMem, hPen);
                    SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
                    Rectangle(hdcMem, mouseStartX, mouseStartY, mouseActualX, mouseActualY);
                    DeleteObject(hPen);
                }

                dibujarHUD(hdcMem, &miJugador, ancho, alto);
            }

            BitBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, hbmOld); 
            DeleteObject(hbmMem); 
            DeleteDC(hdcMem);
            EndPaint(hwnd, &ps);
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