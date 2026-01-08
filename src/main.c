/* src/main.c - VERSIÓN CORREGIDA FINAL (SISTEMA COMPLETO) */
#include <windows.h>
#include <stdio.h>
#include <math.h>               // Necesario para sqrt y pow
#include "global.h"             
#include "mapa/mapa.h"          
#include "recursos/recursos.h" 
#include "jugador/jugador.h"

/* ========== VARIABLES GLOBALES ========== */
char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];
Jugador miJugador;
Camera miCamara;
EstadoJuego estadoJuego;
int mouseActualX, mouseActualY; // Posición actual mientras arrastras
bool dibujandoCuadro = false;
int mouseStartX, mouseStartY;   // Punto donde hiciste clic
TextoFlotante textos[MAX_TEXTOS]; 
Arbol arboles[MAX_ARBOLES];
Mina minas[MAX_MINAS];
Particula particulas[MAX_PARTICULAS];

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
                
                // PRIORIDAD 1: LA TIENDA (Si está abierta, el clic es para comprar)
                if (miJugador.tiendaAbierta) {
                    procesarClickMochilaTienda(mouseX, mouseY, FALSE, &miJugador, hwnd);
                    return 0; // Consumimos el clic, no hace nada más
                }

                // PRIORIDAD 2: JUEGO (Selección de Unidades RTS)
                // Solo si la tienda NO está abierta
                if (!miJugador.tiendaAbierta) {
                    mouseStartX = mouseX;
                    mouseStartY = mouseY;
                    mouseActualX = mouseX;
                    mouseActualY = mouseY;
                    dibujandoCuadro = true;

                    // Si no presionas SHIFT, deseleccionamos todo lo anterior
                    if (!(wParam & MK_SHIFT)) {
                        for(int i=0; i<MAX_UNIDADES; i++) unidades[i].seleccionado = 0;
                    }
                }
            } 
            else if (estadoJuego.estadoActual == ESTADO_MENU) {
                procesarClickMenu(mouseX, mouseY, hwnd, &estadoJuego);
            } 
            else if (estadoJuego.estadoActual == ESTADO_SELECCION_MAPA) {
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

        for(int i = 0; i < MAX_UNIDADES; i++) {
            if(!unidades[i].activa) continue;

            // Deseleccionar todas primero si es un click simple para no acumular selección
            if(esClickSimple) unidades[i].seleccionado = 0;

            // Convierte la posición del mundo de la unidad a posición de pantalla
            int ux = (int)((unidades[i].x - miCamara.x) * miCamara.zoom);
            int uy = (int)((unidades[i].y - miCamara.y) * miCamara.zoom);
            int size = (int)(32 * miCamara.zoom);

            if(esClickSimple) {
                // CORRECCIÓN: Condición completa para click simple (X e Y)
                if(mouseEndX >= ux && mouseEndX <= ux + size && 
                   mouseEndY >= uy && mouseEndY <= uy + size) 
                {
                    unidades[i].seleccionado = 1;
                }
            } 
            else {
                // LÓGICA ADICIONAL: Selección por cuadro (arrastrar mouse)
                // Verifica si el cuadrado de la unidad se solapa con el cuadro dibujado
                if(ux + size >= xMin && ux <= xMax && 
                   uy + size >= yMin && uy <= yMax) 
                {
                    unidades[i].seleccionado = 1;
                } else {
                    unidades[i].seleccionado = 0;
                }
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
                
                // 1. Si la tienda está abierta: VENDER (Click derecho)
                if (miJugador.tiendaAbierta) {
                    // Nota: 1 significa que es click derecho (vender)
                    procesarClickMochilaTienda(mX, mY, 1, &miJugador, hwnd);
                }
                else {
                    // 2. Si no hay tienda: Dar orden a unidades (Movimiento/Caza)
                    // Calcular coordenadas de mundo
                    float mundoX = (mX / miCamara.zoom) + miCamara.x;
                    float mundoY = (mY / miCamara.zoom) + miCamara.y;
                    
                    ordenarUnidad(mX, mY, miCamara);
                    crearChispas(mundoX, mundoY, RGB(0, 255, 0)); // Feedback visual verde
                }
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
                // --- CAPA 1: MUNDO (Fondo) ---
                dibujarMapaConZoom(hdcMem, mapaMundo, miCamara, ancho, alto, estadoJuego.frameTienda, estadoJuego.mapaSeleccionado);
                dibujarTesoros(hdcMem, miCamara, ancho, alto); 
                dibujarMinas(hdcMem, miCamara, ancho, alto); 
                dibujarEstablo(hdcMem, miCamara);
                dibujarVacas(hdcMem, miCamara, ancho, alto);
                dibujarUnidades(hdcMem, miCamara);
                
                // --- CAPA 2: JUGADOR ---
                // Nota: Usamos '&' porque la función ahora pide un puntero
                dibujarJugador(hdcMem, &miJugador, miCamara);
                
                // --- CAPA 3: SELECCIÓN RTS (Cuadro verde) ---
                if (dibujandoCuadro) {
                    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
                    SelectObject(hdcMem, hPen);
                    SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
                    Rectangle(hdcMem, mouseStartX, mouseStartY, mouseActualX, mouseActualY);
                    DeleteObject(hPen);
                }

                // --- CAPA 4: INTERFAZ DE USUARIO (HUD y Ventanas) ---
                // Dibujamos esto ANTES de los textos para que los textos queden encima
                
                // Barra inferior (Vida, XP, etc.)
                dibujarHUD(hdcMem, &miJugador, ancho, alto);

                // Tienda (Derecha)
                if (miJugador.tiendaAbierta) {
                    dibujarTiendaInteractiva(hdcMem, &miJugador, ancho, alto);
                }

                // --- CAPA 5: EFECTOS Y TEXTOS (Top Layer) ---
                // ESTO ES LO CRÍTICO: Al ponerlo al final, nada lo tapa.
                actualizarYDibujarParticulas(hdcMem, miCamara);
                actualizarYDibujarTextos(hdcMem, miCamara);
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
                        // --- INICIAR PARTIDA ---
                        estadoJuego.mapaSeleccionado = estadoJuego.opcionSeleccionada;
                        
                        // Inicializar mapa y colisiones
                        inicializarJuego(&miJugador, &estadoJuego, mapaMundo, estadoJuego.mapaSeleccionado);
                        
                        // Inicializar valores por defecto
                        miJugador.nivelMochila = 1;
                        miJugador.herramientaActiva = HERRAMIENTA_NADA;
                        miJugador.armaduraEquipada = FALSE;
                        miJugador.tiendaAbierta = 0; // Asegurar tienda cerrada al inicio
                        
                        miCamara.zoom = 3;
                        actualizarCamara(&miCamara, miJugador);
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    else if (wParam == VK_ESCAPE)
                    {
                        estadoJuego.estadoActual = ESTADO_MENU;
                        estadoJuego.opcionSeleccionada = 0;
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    break;
                    
                case ESTADO_PARTIDA:
                    switch (wParam) {
                        case VK_ESCAPE: 
                            estadoJuego.estadoActual = ESTADO_SELECCION_MAPA;
                            InvalidateRect(hwnd, NULL, FALSE);
                            return 0;

                        // --- MOVIMIENTO ---
                        case 'W': moverJugador(&miJugador, mapaMundo, 0, -1); break;
                        case 'S': moverJugador(&miJugador, mapaMundo, 0, 1); break;
                        case 'A': moverJugador(&miJugador, mapaMundo, -1, 0); break;
                        case 'D': moverJugador(&miJugador, mapaMundo, 1, 0); break;

                        // --- INTERFAZ: MOCHILA (Bloq Mayús) ---
                        // --- INTERFAZ: MOCHILA (Tecla I) ---
                        case 'I': 
                        case 'i':
                            alternarInventario(&miJugador);
                            // Nota: No cerramos la tienda aquí para permitir ver ambas ventanas a la vez.
                            InvalidateRect(hwnd, NULL, FALSE);
                            break;

                        // --- INTERFAZ: TIENDA (T) ---
                        case 'T':
                        case 't':
                            if (miJugador.tiendaAbierta) {
                                miJugador.tiendaAbierta = 0; // Cerrar tienda
                            } else {
                                // Verificar distancia a la tienda (Coord: 1450, 1900)
                                float dist = sqrt(pow(miJugador.x - 1450, 2) + pow(miJugador.y - 1900, 2));
                                if (dist < 150) {
                                    miJugador.tiendaAbierta = 1;
                                    miJugador.modoTienda = 0; // Reset a Comprar
                                    // Nota: No cerramos el inventario (miJugador.inventarioAbierto = 0)
                                    // para que puedas ver tus items mientras compras.
                                } else {
                                    crearTextoFlotante(miJugador.x, miJugador.y, "Acercate a la tienda!", 0, RGB(255, 100, 100));
                                }
                            }
                            InvalidateRect(hwnd, NULL, FALSE);
                            break;

                        // --- EQUIPAMIENTO: ARMADURA (TAB) ---
                        case VK_TAB:
                            if (miJugador.tieneArmadura) {
                                if (miJugador.armaduraEquipada) {
                                    miJugador.armaduraEquipada = FALSE; // Quitar
                                } else {
                                    miJugador.armaduraEquipada = TRUE;  // Poner
                                    miJugador.herramientaActiva = HERRAMIENTA_NADA; 
                                }
                                InvalidateRect(hwnd, NULL, FALSE);
                            }
                            break;

                        // --- HERRAMIENTAS: 1, 2, 3 ---
                        case '1': // Espada
                            if (miJugador.nivelMochila >= 2 && miJugador.tieneEspada) {
                                if (miJugador.herramientaActiva == HERRAMIENTA_ESPADA) miJugador.herramientaActiva = HERRAMIENTA_NADA;
                                else { miJugador.herramientaActiva = HERRAMIENTA_ESPADA; miJugador.armaduraEquipada = FALSE; }
                                InvalidateRect(hwnd, NULL, FALSE);
                            }
                            break;
                        case '2': // Pico
                            if (miJugador.nivelMochila >= 2 && miJugador.tienePico) {
                                if (miJugador.herramientaActiva == HERRAMIENTA_PICO) miJugador.herramientaActiva = HERRAMIENTA_NADA;
                                else { miJugador.herramientaActiva = HERRAMIENTA_PICO; miJugador.armaduraEquipada = FALSE; }
                                InvalidateRect(hwnd, NULL, FALSE);
                            }
                            break;
                        case '3': // Hacha
                            if (miJugador.nivelMochila >= 2 && miJugador.tieneHacha) {
                                if (miJugador.herramientaActiva == HERRAMIENTA_HACHA) miJugador.herramientaActiva = HERRAMIENTA_NADA;
                                else { miJugador.herramientaActiva = HERRAMIENTA_HACHA; miJugador.armaduraEquipada = FALSE; }
                                InvalidateRect(hwnd, NULL, FALSE);
                            }
                            break;

                        // --- CURACIÓN (R) ---
                        case 'R':
                        case 'r':
                            intentarCurar(&miJugador);
                            InvalidateRect(hwnd, NULL, FALSE);
                            break;

                        // --- ZOOM ---
                        case VK_ADD: case 0xBB: 
                            miCamara.zoom += 1; if (miCamara.zoom > 5) miCamara.zoom = 5; break;
                        case VK_SUBTRACT: case 0xBD: 
                            miCamara.zoom -= 1; if (miCamara.zoom < 1) miCamara.zoom = 1; break;
                            
                        // --- ACCIÓN (Espacio) ---
                        case VK_SPACE: 
                            talarArbol(&miJugador);
                            abrirTesoro(&miJugador);
                            golpearVaca(&miJugador);
                            picarMina(&miJugador);
                            intentarMontarBarco(&miJugador, mapaMundo);
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

    // Verificación de archivos (DEBUG)
    int vacasFaltan = 0;
    for(int i=0; i<8; i++) if(!hBmpVaca[i]) vacasFaltan++;
    if(vacasFaltan > 0) MessageBox(NULL, "Faltan imágenes de vacas en assets/", "Aviso", MB_OK);

    // Estado inicial
    estadoJuego.estadoActual = ESTADO_MENU;
    estadoJuego.opcionSeleccionada = -1;
    estadoJuego.mapaSeleccionado = MAPA_ISLAS_OCEANO;
    estadoJuego.frameTienda = 0;
    
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
                actualizarLogicaSistema(&miJugador);
                actualizarUnidades(mapaMundo, &miJugador);
                actualizarRegeneracionRecursos();
                
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