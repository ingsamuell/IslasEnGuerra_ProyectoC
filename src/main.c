/* src/main.c - VERSIÓN CORREGIDA FINAL (SISTEMA COMPLETO) */
#include <windows.h>
#include <stdio.h>
#include <math.h> // Necesario para sqrt y pow
#include "global.h"
#include "mundo/mapa.h"
#include "mundo/fauna.h"
#include "mundo/naturaleza.h"
#include "recursos/recursos.h"
#include "jugador/jugador.h"
#include "unidades/unidades.h"
#include "sistema/tienda.h"
#include "mundo/edificios.h"

/* ========== VARIABLES GLOBALES ========== */
char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];
Jugador miJugador;
Camera miCamara;
EstadoJuego estadoJuego;
int mouseActualX, mouseActualY; // Posición actual mientras arrastras
bool dibujandoCuadro = false;
int mouseStartX, mouseStartY; // Punto donde hiciste clic
TextoFlotante textos[MAX_TEXTOS];
Arbol arboles[MAX_ARBOLES];
Mina minas[MAX_MINAS];
Particula particulas[MAX_PARTICULAS];

/* ========== PROCEDIMIENTO DE VENTANA ========== */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    static BOOL seleccionando = FALSE;
    static POINT inicioSel = {0};
    static POINT finSel = {0};
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
    int mx = LOWORD(lParam);
    int my = HIWORD(lParam);

    if (estadoJuego.estadoActual == ESTADO_PARTIDA)
    {
        // --- 1. PRIORIDAD: CLIC EN LA INTERFAZ (TIENDA / MOCHILA) ---
        BOOL clicEnUI = FALSE;

        if (miJugador.tiendaAbierta)
        {
            RECT rcClient; GetClientRect(hwnd, &rcClient);
            int anchoW = 340;
            int tx = rcClient.right - anchoW - 20;
            int ty = 80;

            if (mx >= tx && mx <= tx + anchoW && my >= ty && my <= ty + 480)
            {
                clicEnUI = TRUE;
                procesarClickMochilaTienda(mx, my, FALSE, &miJugador, hwnd);
            }
        }


        // --- 2. INTERACCIÓN CON EL MUNDO (Solo si no clicamos en la UI) ---
        if (!clicEnUI)
        {
            // A) MODO CONSTRUCCIÓN
            if (miJugador.edificioSeleccionado > 0) 
            {
                // Convertir coordenadas de PANTALLA a MUNDO
                int mundoX = (int)((mx / miCamara.zoom) + miCamara.x);
                int mundoY = (int)((my / miCamara.zoom) + miCamara.y);

                // Intentar construir (Pasamos mapaMundo que es la matriz char[200][200])
                intentarColocarEdificio(&miJugador, mundoX, mundoY, mapaMundo);
            }
            // B) SELECCIÓN DE UNIDADES (RTS)
            else 
            {
                seleccionando = TRUE;
                inicioSel.x = mx;
                inicioSel.y = my;
                finSel.x = mx;
                finSel.y = my;

                // Si no usas SHIFT, limpiar selección previa de tropas
                if (!(wParam & MK_SHIFT))
                {
                    for (int i = 0; i < MAX_UNIDADES; i++) unidades[i].seleccionado = 0;
                }
            }
        }
    }
    // --- OTROS ESTADOS DEL JUEGO ---
    else if (estadoJuego.estadoActual == ESTADO_MENU) 
    {
        procesarClickMenu(mx, my, hwnd, &estadoJuego);
    }
    else if (estadoJuego.estadoActual == ESTADO_SELECCION_MAPA) 
    {
        procesarClickSeleccionMapa(mx, my, hwnd, &estadoJuego);
    }

    InvalidateRect(hwnd, NULL, FALSE);
    return 0;
}

    case WM_MOUSEMOVE:
{
    // 1. Guardar posición global del mouse
    estadoJuego.puntoMouse.x = LOWORD(lParam);
    estadoJuego.puntoMouse.y = HIWORD(lParam);

    // 2. Si estamos arrastrando el cuadro de selección (RTS)
    if (estadoJuego.estadoActual == ESTADO_PARTIDA)
    {
        if (seleccionando)
        {
            finSel.x = LOWORD(lParam);
            finSel.y = HIWORD(lParam);
            // TRUE en el tercer parámetro para un redibujado más agresivo del cuadro
            InvalidateRect(hwnd, NULL, FALSE);
        }
        
        // 3. Si tenemos un edificio seleccionado para construir
        // Necesitamos redibujar para que el "fantasma" siga al mouse
        if (miJugador.edificioSeleccionado > 0) 
        {
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }
    break;
}

    case WM_LBUTTONUP:
    {
        // Solo si estábamos en modo selección de partida
        if (estadoJuego.estadoActual == ESTADO_PARTIDA && seleccionando)
        {
            seleccionando = FALSE; // Apagamos el cuadro verde
            
            // Actualizamos la coordenada final
            finSel.x = LOWORD(lParam);
            finSel.y = HIWORD(lParam);

            // ¡AQUÍ ESTÁ LA CLAVE! Llamamos a la lógica de selección
            seleccionarUnidadesGrupo(inicioSel.x, inicioSel.y, finSel.x, finSel.y, miCamara);
            
            // También revisamos si soltó el click sobre un botón de la tienda
            procesarClickMochilaTienda(finSel.x, finSel.y, 0, &miJugador, hwnd);

            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }
    
   
    case WM_RBUTTONDOWN:
    {
        if (estadoJuego.estadoActual == ESTADO_PARTIDA)
        {
            int mX = LOWORD(lParam);
            int mY = HIWORD(lParam);
            BOOL accionRealizada = FALSE;

            // 1. Intentar VENDER solo si el clic es DENTRO de la tienda
            if (miJugador.tiendaAbierta)
            {
                RECT rcClient; GetClientRect(hwnd, &rcClient);
                int anchoW = 340;
                int tx = rcClient.right - anchoW - 20;
                int ty = 80;

                if (mX >= tx && mX <= tx + anchoW && mY >= ty && mY <= ty + 480)
                {
                    procesarClickMochilaTienda(mX, mY, 1, &miJugador, hwnd);
                    accionRealizada = TRUE;
                }
            }

            // 2. Si no fue una venta, es una ORDEN DE MOVIMIENTO
            if (!accionRealizada)
            {
                ordenarUnidad(mX, mY, miCamara);
                
                // Feedback visual (punto verde en el suelo)
                float mundoX = (mX / miCamara.zoom) + miCamara.x;
                float mundoY = (mY / miCamara.zoom) + miCamara.y;
                crearChispas(mundoX, mundoY, RGB(0, 255, 0)); 
            }
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        int ancho = rc.right;
        int alto = rc.bottom;

        // Doble Buffer (Memoria)
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, ancho, alto);
        HGDIOBJ hbmOld = SelectObject(hdcMem, hbmMem);

        if (estadoJuego.estadoActual == ESTADO_MENU)
        {
            dibujarMenuConSprites(hdcMem, hwnd, &estadoJuego);
        }
        else if (estadoJuego.estadoActual == ESTADO_SELECCION_MAPA)
        {
            dibujarSeleccionMapa(hdcMem, hwnd, &estadoJuego);
        }
        else if (estadoJuego.estadoActual == ESTADO_PARTIDA)
        {
            // --- CAPA 1: MUNDO (Fondo) ---
            dibujarMapaConZoom(hdcMem, mapaMundo, miCamara, ancho, alto, estadoJuego.frameTienda, estadoJuego.mapaSeleccionado);
            dibujarTodosLosEdificios(hdcMem, estadoJuego.mapaSeleccionado, &miCamara);
            dibujarTesoros(hdcMem, miCamara, ancho, alto);
            dibujarMinas(hdcMem, miCamara, ancho, alto);
            dibujarEstablo(hdcMem, miCamara);
            dibujarVacas(hdcMem, miCamara, ancho, alto);
            
            // Unidades
            dibujarUnidades(hdcMem, miCamara);

            // --- CAPA 2: JUGADOR ---
            dibujarJugador(hdcMem, &miJugador, miCamara);

            // --- CAPA 3: SELECCIÓN RTS (CORREGIDO) ---
            // Usamos 'seleccionando', dibujamos en 'hdcMem' y usamos las variables nuevas
            if (seleccionando)
            {
                HPEN hPenSel = CreatePen(PS_DOT, 1, RGB(0, 255, 0)); // Punteado Verde
                HGDIOBJ hOldPen = SelectObject(hdcMem, hPenSel);     // Nota: hdcMem
                HGDIOBJ hOldBrush = SelectObject(hdcMem, GetStockObject(NULL_BRUSH)); 
                
                // Dibujar el rectángulo
                Rectangle(hdcMem, inicioSel.x, inicioSel.y, finSel.x, finSel.y);
                
                SelectObject(hdcMem, hOldBrush);
                SelectObject(hdcMem, hOldPen);
                DeleteObject(hPenSel);
            }
            
			int mundoMouseX = (int)(miCamara.x + (estadoJuego.puntoMouse.x / miCamara.zoom));
int mundoMouseY = (int)(miCamara.y + (estadoJuego.puntoMouse.y / miCamara.zoom));


if (miJugador.edificioSeleccionado > 0) {
    // Pasamos &miCamara al final como pide tu edificios.c
    dibujarFantasmaConstruccion(hdcMem, &miJugador, mundoMouseX, mundoMouseY, estadoJuego.mapaSeleccionado, &miCamara);
}

// 2. Corregir la llamada al MiniMapa (El orden exacto que pide tu .h)
if (miJugador.tieneMapa) {
    dibujarMiniMapa(hdcMem, &miJugador, mapaMundo, ancho, alto);
}
            // --- CAPA 4: INTERFAZ DE USUARIO (HUD y Ventanas) ---
            dibujarHUD(hdcMem, &miJugador, ancho, alto);

            if (miJugador.tiendaAbierta)
            {
                dibujarTiendaInteractiva(hdcMem, &miJugador, ancho, alto);
            }
            

            // --- CAPA 5: EFECTOS Y TEXTOS (Top Layer) ---
            actualizarYDibujarParticulas(hdcMem, miCamara);
            actualizarYDibujarTextos(hdcMem, miCamara);
        }

        // --- FINALIZAR DOBLE BUFFER ---
        // Copiamos todo de memoria a pantalla de un solo golpe
        BitBlt(hdc, 0, 0, ancho, alto, hdcMem, 0, 0, SRCCOPY);
        
        // Limpieza
        SelectObject(hdcMem, hbmOld);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);
        
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_KEYDOWN:
    {
        switch (estadoJuego.estadoActual)
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
            switch (wParam)
            {
            case VK_ESCAPE:
                estadoJuego.estadoActual = ESTADO_SELECCION_MAPA;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;

            // --- MOVIMIENTO ---
            case 'W':
                moverJugador(&miJugador, mapaMundo, 0, -1);
                break;
            case 'S':
                moverJugador(&miJugador, mapaMundo, 0, 1);
                break;
            case 'A':
                moverJugador(&miJugador, mapaMundo, -1, 0);
                break;
            case 'D':
                moverJugador(&miJugador, mapaMundo, 1, 0);
                break;

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
                if (miJugador.tiendaAbierta)
                {
                    miJugador.tiendaAbierta = 0; // Cerrar tienda
                }
                else
                {
                    // Verificar distancia a la tienda (Coord: 1450, 1900)
                    float dist = sqrt(pow(miJugador.x - 1450, 2) + pow(miJugador.y - 1900, 2));
                    if (dist < 150)
                    {
                        miJugador.tiendaAbierta = 1;
                        miJugador.modoTienda = 0; // Reset a Comprar
                        // Nota: No cerramos el inventario (miJugador.inventarioAbierto = 0)
                        // para que puedas ver tus items mientras compras.
                    }
                    else
                    {
                        crearTextoFlotante(miJugador.x, miJugador.y, "Acercate a la tienda!", 0, RGB(255, 100, 100));
                    }
                }
                InvalidateRect(hwnd, NULL, FALSE);
                break;

            // --- EQUIPAMIENTO: ARMADURA (TAB) ---
            case VK_TAB:
                if (miJugador.tieneArmadura)
                {
                    if (miJugador.armaduraEquipada)
                    {
                        miJugador.armaduraEquipada = FALSE; // Quitar
                    }
                    else
                    {
                        miJugador.armaduraEquipada = TRUE; // Poner
                        miJugador.herramientaActiva = HERRAMIENTA_NADA;
                    }
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                break;

            // --- HERRAMIENTAS: 1, 2, 3 ---
            case '1': // Espada
                if (miJugador.nivelMochila >= 2 && miJugador.tieneEspada)
                {
                    if (miJugador.herramientaActiva == HERRAMIENTA_ESPADA)
                        miJugador.herramientaActiva = HERRAMIENTA_NADA;
                    else
                    {
                        miJugador.herramientaActiva = HERRAMIENTA_ESPADA;
                        miJugador.armaduraEquipada = FALSE;
                    }
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                break;
            case '2': // Pico
                if (miJugador.nivelMochila >= 2 && miJugador.tienePico)
                {
                    if (miJugador.herramientaActiva == HERRAMIENTA_PICO)
                        miJugador.herramientaActiva = HERRAMIENTA_NADA;
                    else
                    {
                        miJugador.herramientaActiva = HERRAMIENTA_PICO;
                        miJugador.armaduraEquipada = FALSE;
                    }
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                break;
            case '3': // Hacha
                if (miJugador.nivelMochila >= 2 && miJugador.tieneHacha)
                {
                    if (miJugador.herramientaActiva == HERRAMIENTA_HACHA)
                        miJugador.herramientaActiva = HERRAMIENTA_NADA;
                    else
                    {
                        miJugador.herramientaActiva = HERRAMIENTA_HACHA;
                        miJugador.armaduraEquipada = FALSE;
                    }
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
            case VK_ADD:
            case 0xBB:
                miCamara.zoom += 1;
                if (miCamara.zoom > 5)
                    miCamara.zoom = 5;
                break;
            case VK_SUBTRACT:
            case 0xBD:
                miCamara.zoom -= 1;
                if (miCamara.zoom < 1)
                    miCamara.zoom = 1;
                break;

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

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/* ========== MAIN ========== */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ClaseIslasGuerra";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    if (!RegisterClass(&wc))
        return -1;

    int ancho = GetSystemMetrics(SM_CXSCREEN);
    int alto = GetSystemMetrics(SM_CYSCREEN);
    HWND hwnd = CreateWindow("ClaseIslasGuerra", "Islas en Guerra", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, ancho, alto, NULL, NULL, hInstance, NULL);

    if (!hwnd)
        return 0;

    CargarRecursos();

    // Verificación de archivos (DEBUG)
    int vacasFaltan = 0;
    for (int i = 0; i < 8; i++)
        if (!hBmpVaca[i])
            vacasFaltan++;
    if (vacasFaltan > 0)
        MessageBox(NULL, "Faltan imágenes de vacas en assets/", "Aviso", MB_OK);

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
    // Bucle principal del juego
    while (msg.message != WM_QUIT)
    {
        // 1. Procesar Mensajes de Windows (Input, Cierre, etc.)
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // 2. Lógica del Juego (Solo si estamos jugando)
            if (estadoJuego.estadoActual == ESTADO_PARTIDA)
            {
                // --- A) UNIDADES RTS (Lo más importante) ---
                actualizarUnidades(mapaMundo, &miJugador);

                // --- B) FAUNA Y SISTEMAS ---
                actualizarVacas();                    // Movimiento de animales
                actualizarLogicaSistema(&miJugador);  // Tiburones, Pesca, etc.
                actualizarRegeneracionRecursos();// Respawn de árboles/minas/vacas
                actualizarEdificios(0.016f);     

                // --- C) ANIMACIONES AMBIENTALES ---
                static int timerGato = 0;
                timerGato++;
                if (timerGato > 30)
                {
                    estadoJuego.frameTienda = !estadoJuego.frameTienda;
                    timerGato = 0;
                }
            }

            // 3. Renderizado (Para TODOS los estados: Menú, Selección, Partida)
            // Lo sacamos del 'if' para que el Menú también se anime y redibuje
            InvalidateRect(hwnd, NULL, FALSE);

            // 4. Control de Frames (Evitar 100% CPU)
            // También lo sacamos fuera para que el PC descanse en el menú
            Sleep(16); // Aprox 60 FPS
        }
    }

    LiberarRecursos();
    return 0;
}