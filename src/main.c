/* src/main.c */
#include <windows.h>
#include <stdio.h>
#include <math.h> 
#include <time.h>
#include "global.h"
#include "mundo/mapa.h"
#include "mundo/fauna.h"
#include "mundo/naturaleza.h"
#include "recursos/recursos.h"
#include "jugador/jugador.h"
#include "unidades/unidades.h"
#include "sistema/sistema.h"
#include "sistema/tienda.h"
#include "mundo/edificios.h"
#include "unidades/enemigos.h"


/* ========== VARIABLES GLOBALES ========== */
char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];
Jugador miJugador;
Camera miCamara;
EstadoJuego estadoJuego;
int mouseActualX, mouseActualY; 
bool dibujandoCuadro = false;
int mouseStartX, mouseStartY; 
TextoFlotante textos[MAX_TEXTOS];
Arbol arboles[MAX_ARBOLES];
Mina minas[MAX_MINAS];
Particula particulas[MAX_PARTICULAS];
char nombreGuardadoInput[32] = "";
int lenNombre = 0;
char infoPartidaTexto[128] = "Cargando info...";
int existePartida = 0; // 1 si hay archivo, 0 si no

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
        // 1. OBTENER COORDENADAS DEL CLIC
        int mx = LOWORD(lParam);
        int my = HIWORD(lParam);

        // 1. ESTADO: JUGANDO

        if (estadoJuego.estadoActual == ESTADO_PARTIDA)
        {
            // === COMBATE NAVAL (Disparo del Jugador) ===
            // Solo si estás en el Barco de Guerra (Tipo 2)
            if (miJugador.estadoBarco == 2) {
                
                // 1. Verificar si el cañón está listo
                if (miJugador.cooldownCanon <= 0) {
                    // Calcular destino del disparo (Mouse en el mundo)
                    float targetX = (mx / miCamara.zoom) + miCamara.x;
                    float targetY = (my / miCamara.zoom) + miCamara.y;
                    // 2. CREAR BALA (Visual)
                    crearBalaCanon(miJugador.x, miJugador.y, targetX, targetY);
                    
                    // 3. LÓGICA DE DAÑO (Radio de explosión)
                    // Buscamos enemigos cerca del clic
                    for(int i=0; i<MAX_UNIDADES; i++) {
                        if(unidades[i].activa && unidades[i].bando == BANDO_ENEMIGO) {
                            float dx = unidades[i].x - targetX;
                            float dy = unidades[i].y - targetY;
                            float dist = sqrt(dx*dx + dy*dy);
                            
                            if (dist < 60) { // Radio de explosión grande
                                unidades[i].vida -= 20; 
                                crearTextoFlotante(unidades[i].x, unidades[i].y, "BOOM!", 0, RGB(255, 100, 0));
                                crearSangre(unidades[i].x, unidades[i].y);
                                
                                if(unidades[i].vida <= 0) {
                                    unidades[i].activa = 0;
                                    ganarExperiencia(&miJugador, 80);
                                }
                            }
                        }
                    }

                    // 4. INICIAR RECARGA (2 segundos = 120 frames)
                    miJugador.cooldownCanon = 120; 
                    
                } else {
                    crearTextoFlotante(miJugador.x, miJugador.y - 50, "Recargando...", 0, RGB(200, 200, 200));
                }
                return 0; 
            }


        // --- PRIORIDAD B: CLIC EN TIENDA ---
        BOOL clicEnUI = FALSE;
        if (miJugador.tiendaAbierta) {
            RECT rcClient;
            GetClientRect(hwnd, &rcClient);
            int anchoW = 340;
            int tx = rcClient.right - anchoW - 20;
            int ty = 80;

            if (mx >= tx && mx <= tx + anchoW && my >= ty && my <= ty + 480) {
                clicEnUI = TRUE;
                procesarClickMochilaTienda(mx, my, FALSE, &miJugador, hwnd);
                return 0;
            }
        }

        // --- PRIORIDAD C: CONSTRUCCIÓN O SELECCIÓN ---
        if (!clicEnUI) {
            if (miJugador.edificioSeleccionado > 0) {
                int mundoX = (int)((mx / miCamara.zoom) + miCamara.x);
                int mundoY = (int)((my / miCamara.zoom) + miCamara.y);
                intentarColocarEdificio(&miJugador, mundoX, mundoY, mapaMundo);
            } else {
                seleccionando = TRUE;
                inicioSel.x = mx;
                inicioSel.y = my;
                finSel.x = mx;
                finSel.y = my;

                if (!(wParam & MK_SHIFT)) {
                    for (int i = 0; i < MAX_UNIDADES; i++)
                        unidades[i].seleccionado = 0;
                }
            }
        }
    }

        // 2. ESTADO: MENÚ PRINCIPAL 
        else if (estadoJuego.estadoActual == ESTADO_MENU)
        {
            // BOTÓN JUGAR
            if (mx >= 642 && mx <= 942 && my >= 595 && my <= 678) {
                 estadoJuego.estadoActual = ESTADO_SELECCION_MAPA;
                 InvalidateRect(hwnd, NULL, FALSE);
            }

            // BOTÓN PARTIDAS / CARGAR
            if (mx >= 220 && mx <= 450 && my >= 340 && my <= 480) 
            {
                estadoJuego.estadoActual = ESTADO_MENU_CARGAR;
                
                // Verificar si existe partida guardada
                existePartida = ExistePartida(0); // Usamos el slot 0 por defecto
                if (existePartida) {
                    ObtenerInfoPartida(0, infoPartidaTexto);
                } else {
                    strcpy(infoPartidaTexto, "No hay partida guardada");
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }

            // BOTÓN INSTRUCCIONES 
            if (mx >= 1100 && mx <= 1390 && my >= 350 && my <= 455) 
            {
                estadoJuego.estadoActual = ESTADO_INSTRUCCIONES;
                InvalidateRect(hwnd, NULL, FALSE);
            }

            // BOTÓN SALIR
            if (mx >= 1178 && mx <= 1500 && my >= 688 && my <= 768) {
                 PostQuitMessage(0);
            }
        }

        // 3. ESTADO: PANTALLA CARGAR PARTIDA
        else if (estadoJuego.estadoActual == ESTADO_MENU_CARGAR)
        {
// DETECTAR CLIC EN SLOTS Y BOTONES DE BORRAR
    for (int i = 1; i <= 3; i++) {
        int yBase = 100 + ((i-1) * 110);
        
        // 1. CLIC EN EL SLOT (CARGAR / CREAR) 
        if (mx >= 250 && mx <= 650 && my >= yBase && my <= yBase + 90) {
            if (ExistePartida(i)) {
                if (CargarPartida(i, &miJugador, mapaMundo)) {
                    estadoJuego.slotJugado = i;
                    estadoJuego.estadoActual = ESTADO_PARTIDA;
                    actualizarCamara(&miCamara, miJugador);
                    MessageBox(hwnd, "Partida Cargada", "Exito", MB_OK);
                }
            } else {
                estadoJuego.slotJugado = i;
                estadoJuego.estadoActual = ESTADO_SELECCION_MAPA;
            }
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }

        // 2. CLIC EN BOTÓN BORRAR [X] 
        // Solo si existe la partida
        if (ExistePartida(i) && mx >= 660 && mx <= 700 && my >= yBase + 25 && my <= yBase + 65) {
            // Preguntar confirmación
            int resp = MessageBox(hwnd, 
                "Estas seguro de que quieres BORRAR esta partida permanentemente?", 
                "Confirmar Borrado", MB_YESNO | MB_ICONWARNING);
            
            if (resp == IDYES) {
                if (BorrarPartida(i)) {
                    MessageBox(hwnd, "Partida eliminada.", "Info", MB_OK);
                } else {
                    MessageBox(hwnd, "No se pudo eliminar el archivo.", "Error", MB_ICONERROR);
                }
                InvalidateRect(hwnd, NULL, FALSE); // Repintar para que desaparezca
            }
            return 0;
        }
    }

    // BOTÓN VER REGISTRO
    if (mx >= 250 && mx <= 440 && my >= 450 && my <= 500) {
        AbrirArchivoLog();
    }

    // BOTÓN VOLVER
    if (mx >= 460 && mx <= 650 && my >= 450 && my <= 500) {
        estadoJuego.estadoActual = ESTADO_MENU;
        InvalidateRect(hwnd, NULL, FALSE);
    }
    
        }
        // 4. OTROS ESTADOS
        else if (estadoJuego.estadoActual == ESTADO_SELECCION_MAPA)
        {
            procesarClickSeleccionMapa(mx, my, hwnd, &estadoJuego);
        }
        
else if (estadoJuego.estadoActual == ESTADO_INSTRUCCIONES)
        {
            // Coordenadas del botón "VOLVER" 
            RECT rect;
    GetClientRect(hwnd, &rect);
    int ancho = rect.right - rect.left;
    int alto = rect.bottom - rect.top;
            if (mx >= (ancho / 2) - 100 && mx <= (ancho / 2) + 100 &&
                my >= (alto - 80) && my <= (alto - 30))
            {
                estadoJuego.estadoActual = ESTADO_MENU;
                InvalidateRect(hwnd, NULL, FALSE); // Redibujar menú
            }
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
                RECT rcClient;
                GetClientRect(hwnd, &rcClient);
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
        // 1. OBTENER DIMENSIONES DE PANTALLA
        int anchoPantalla = GetSystemMetrics(SM_CXSCREEN);
        int altoPantalla = GetSystemMetrics(SM_CYSCREEN);

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

        // ESTADO: MENÚ PRINCIPAL
        if (estadoJuego.estadoActual == ESTADO_MENU)
        {
            dibujarMenuConSprites(hdcMem, hwnd, &estadoJuego);
        }
        // ESTADO: SELECCIÓN DE MAPA
        else if (estadoJuego.estadoActual == ESTADO_SELECCION_MAPA)
        {
            dibujarSeleccionMapa(hdcMem, hwnd, &estadoJuego);
        }
        // ESTADO: PARTIDA EN JUEGO
        else if (estadoJuego.estadoActual == ESTADO_PARTIDA || estadoJuego.estadoActual == ESTADO_GUARDANDO)
        {
            // --- CAPA 1: MUNDO (Fondo) ---
            dibujarMapaConZoom(hdcMem, mapaMundo, miCamara, ancho, alto, estadoJuego.frameTienda, estadoJuego.mapaSeleccionado);
            dibujarTodosLosEdificios(hdcMem, estadoJuego.mapaSeleccionado, &miCamara);
            dibujarTesoros(hdcMem, miCamara, ancho, alto);
            dibujarMinas(hdcMem, miCamara, ancho, alto);
            dibujarEstablo(hdcMem, miCamara);
            dibujarVacas(hdcMem, miCamara, ancho, alto);
            // Unidades
            dibujarUnidades(hdcMem, miCamara, mapaMundo);
            // --- CAPA 2: JUGADOR ---
            dibujarJugador(hdcMem, &miJugador, miCamara);
            // --- CAPA 3: SELECCIÓN RTS ---
            if (seleccionando)
            {
                HPEN hPenSel = CreatePen(PS_DOT, 1, RGB(0, 255, 0));
                HGDIOBJ hOldPen = SelectObject(hdcMem, hPenSel);
                HGDIOBJ hOldBrush = SelectObject(hdcMem, GetStockObject(NULL_BRUSH));

                Rectangle(hdcMem, inicioSel.x, inicioSel.y, finSel.x, finSel.y);

                SelectObject(hdcMem, hOldBrush);
                SelectObject(hdcMem, hOldPen);
                DeleteObject(hPenSel);
            }

            // Fantasma de Construcción
            if (miJugador.edificioSeleccionado > 0)
            {
                int mundoMouseX = (int)(miCamara.x + (estadoJuego.puntoMouse.x / miCamara.zoom));
                int mundoMouseY = (int)(miCamara.y + (estadoJuego.puntoMouse.y / miCamara.zoom));

                dibujarFantasmaConstruccion(hdcMem, &miJugador, mundoMouseX, mundoMouseY,
                                            estadoJuego.mapaSeleccionado, mapaMundo, &miCamara);
            }

            // MiniMapa
            if (miJugador.tieneMapa)
            {
                dibujarMiniMapa(hdcMem, &miJugador, mapaMundo, ancho, alto);
            }

            // --- CAPA 4: INTERFAZ DE USUARIO (HUD) ---
            dibujarHUD(hdcMem, &miJugador, ancho, alto);

            if (miJugador.tiendaAbierta)
            {
                dibujarTiendaInteractiva(hdcMem, &miJugador, ancho, alto);
            }

            // --- CAPA 5: EFECTOS Y TEXTOS ---
            actualizarYDibujarParticulas(hdcMem, miCamara);
            actualizarYDibujarTextos(hdcMem, miCamara);
        }
        
        // ESTADO: PANTALLA DE CARGAR PARTIDA (Historial)
        else if (estadoJuego.estadoActual == ESTADO_MENU_CARGAR)
        {
           // 1. Fondo Oscuro
    HBRUSH brFondo = CreateSolidBrush(RGB(30, 30, 50));
    RECT rFondo = {0, 0, ancho, alto};
    FillRect(hdcMem, &rFondo, brFondo);
    DeleteObject(brFondo);

    // 2. Título
    SetBkMode(hdcMem, TRANSPARENT);
    SetTextColor(hdcMem, RGB(255, 215, 0)); 
    TextOut(hdcMem, 350, 50, "SELECCIONAR PARTIDA", 19);

    // 3. DIBUJAR LOS 3 SLOTS
    for (int i = 1; i <= 3; i++) {
        int yBase = 100 + ((i-1) * 110); // Espaciado vertical
        
        // Color según si existe o no
        COLORREF colorCaja = ExistePartida(i) ? RGB(60, 60, 80) : RGB(40, 40, 40);
        HBRUSH brSlot = CreateSolidBrush(colorCaja);
        RECT rSlot = {250, yBase, 650, yBase + 90}; 
        FillRect(hdcMem, &rSlot, brSlot);
        DeleteObject(brSlot);

        // Marco si pasas el mouse (opcional, lógica simple visual)
        HBRUSH brBorde = CreateSolidBrush(RGB(200, 200, 200));
        FrameRect(hdcMem, &rSlot, brBorde);
        DeleteObject(brBorde);

        // Texto Info
        char info[128];
        ObtenerInfoPartida(i, info);
        SetTextColor(hdcMem, RGB(255, 255, 255));
        TextOut(hdcMem, 270, yBase + 35, info, strlen(info));
        // --- BOTÓN DE BORRAR [X] (Solo si existe partida) ---
        if (existePartida) {
            HBRUSH brBorrar = CreateSolidBrush(RGB(200, 50, 50)); // Rojo
            // Lo dibujamos a la derecha del slot (x: 660 a 700)
            RECT rBorrar = {660, yBase + 25, 700, yBase + 65}; 
            FillRect(hdcMem, &rBorrar, brBorrar);
            DeleteObject(brBorrar);

            // Borde del botón
            HBRUSH brBordeX = CreateSolidBrush(RGB(255, 255, 255));
            FrameRect(hdcMem, &rBorrar, brBordeX);
            DeleteObject(brBordeX);

            // La "X"
            SetTextColor(hdcMem, RGB(255, 255, 255));
            TextOut(hdcMem, 673, yBase + 35, "X", 1);
        }
    }

    // 4. BOTÓN VER REGISTRO (TXT)
    HBRUSH brLog = CreateSolidBrush(RGB(0, 100, 200)); // Azul
    RECT rLog = {250, 450, 440, 500};
    FillRect(hdcMem, &rLog, brLog);
    DeleteObject(brLog);
    TextOut(hdcMem, 280, 465, "VER REGISTRO .TXT", 17);

    // 5. BOTÓN VOLVER
    HBRUSH brVolver = CreateSolidBrush(RGB(150, 0, 0)); // Rojo
    RECT rVolver = {460, 450, 650, 500}; 
    FillRect(hdcMem, &rVolver, brVolver);
    DeleteObject(brVolver);
    TextOut(hdcMem, 510, 465, "VOLVER", 6);
}

        // OVERLAY: PANTALLA DE GUARDADO (Caja Negra con Nombre)
        if (estadoJuego.estadoActual == ESTADO_GUARDANDO)
        {
            // Calculamos centro exacto de la pantalla
            int cx = anchoPantalla / 2;
            int cy = altoPantalla / 2;

            // Fondo semitransparente (simulado con rect gris oscuro)
            HBRUSH brFondo = CreateSolidBrush(RGB(20, 20, 20));
            RECT rMsg = {cx - 200, cy - 100, cx + 200, cy + 100};
            FillRect(hdcMem, &rMsg, brFondo);
            DeleteObject(brFondo);

            // Marco blanco
            HBRUSH brBorde = CreateSolidBrush(RGB(255, 255, 255));
            FrameRect(hdcMem, &rMsg, brBorde);
            DeleteObject(brBorde);

            // Título
            SetTextColor(hdcMem, RGB(255, 215, 0)); 
            SetBkMode(hdcMem, TRANSPARENT);
            TextOut(hdcMem, cx - 60, cy - 80, "GUARDAR PARTIDA", 15);

            // Caja de Texto (Blanca donde escribes)
            HBRUSH brCaja = CreateSolidBrush(RGB(255, 255, 255));
            RECT rCaja = {cx - 150, cy - 20, cx + 150, cy + 20};
            FillRect(hdcMem, &rCaja, brCaja);
            DeleteObject(brCaja);

            // Lo que escribe el usuario
            SetTextColor(hdcMem, RGB(0, 0, 0)); 
            TextOut(hdcMem, cx - 140, cy - 10, nombreGuardadoInput, strlen(nombreGuardadoInput));
            if ((clock() / 500) % 2 == 0)
            {
                TextOut(hdcMem, cx - 140 + (lenNombre * 8), cy - 10, "|", 1);
            }

            // Instrucciones
            SetTextColor(hdcMem, RGB(200, 200, 200));
            SetBkMode(hdcMem, TRANSPARENT); 
            TextOut(hdcMem, cx - 100, cy + 50, "[ENTER] Guardar   [ESC] Cancelar", 32);
        }
        else if (estadoJuego.estadoActual == ESTADO_GAME_OVER)
        {
            // 1. Fondo Rojo Sangre 
            HBRUSH brFondo = CreateSolidBrush(RGB(40, 0, 0));
            RECT rTotal = {0, 0, ancho, alto};
            FillRect(hdcMem, &rTotal, brFondo);
            DeleteObject(brFondo);

            // 2. Título 
            SetBkMode(hdcMem, TRANSPARENT);
            SetTextColor(hdcMem, RGB(255, 0, 0)); 
            // Usamos una fuente grande para el impacto
            HFONT hFontMuerte = CreateFont(60, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, "Arial");
            HFONT hOld = (HFONT)SelectObject(hdcMem, hFontMuerte);
            RECT rTitulo = {0, alto/2 - 100, ancho, alto/2};
            DrawText(hdcMem, "HAS MUERTO", -1, &rTitulo, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdcMem, hOld);
            DeleteObject(hFontMuerte);

            // 3. Subtítulo
            SetTextColor(hdcMem, RGB(200, 200, 200));
            RECT rSub = {0, alto/2, ancho, alto/2 + 50};
            DrawText(hdcMem, "Tu historia termina aqui...", -1, &rSub, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            // 4. OPCIONES 
            SetTextColor(hdcMem, RGB(255, 255, 255));

            RECT rOpcion1 = {0, alto/2 + 80, ancho, alto/2 + 110};
            DrawText(hdcMem, "[ENTER] Iniciar Nueva Conquista", -1, &rOpcion1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            RECT rOpcion2 = {0, alto/2 + 120, ancho, alto/2 + 150};
            DrawText(hdcMem, "[ESC] Volver al Menu Principal", -1, &rOpcion2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        // PANTALLA: VICTORIA (CONQUISTA)
        else if (estadoJuego.estadoActual == ESTADO_WINNER)
        {
            // 1. Fondo Dorado / Azul Real
            HBRUSH brFondo = CreateSolidBrush(RGB(25, 25, 112)); // Azul noche
            RECT rTotal = {0, 0, ancho, alto};
            FillRect(hdcMem, &rTotal, brFondo);
            DeleteObject(brFondo);

            // 2. Confeti (Puntos de colores aleatorios estáticos por frame)
            for(int k=0; k<100; k++) {
                int cx = rand() % ancho;
                int cy = rand() % alto;
                SetPixel(hdcMem, cx, cy, RGB(rand()%255, rand()%255, rand()%255));
                SetPixel(hdcMem, cx+1, cy, RGB(rand()%255, rand()%255, rand()%255));
                SetPixel(hdcMem, cx, cy+1, RGB(rand()%255, rand()%255, rand()%255));
            }

            // 3. Título Grande
            SetBkMode(hdcMem, TRANSPARENT);
            SetTextColor(hdcMem, RGB(255, 215, 0)); // DORADO
            TextOut(hdcMem, ancho/2 - 100, alto/2 - 60, "VICTORIA!", 10);

            // 4. Descripción
            SetTextColor(hdcMem, RGB(255, 255, 255));
            TextOut(hdcMem, ancho/2 - 150, alto/2, "Has destruido el Castillo Enemigo!", 33);
            
            // 5. Recompensas
            SetTextColor(hdcMem, RGB(0, 255, 0));
            TextOut(hdcMem, ancho/2 - 80, alto/2 + 40, "+500 Oro  |  +1000 XP", 21);

            // 6. Botón Continuar
            SetTextColor(hdcMem, RGB(200, 200, 255));
            TextOut(hdcMem, ancho/2 - 100, alto/2 + 100, "[ENTER] Continuar Explorando", 28);
        }
        else if (estadoJuego.estadoActual == ESTADO_INSTRUCCIONES)
        {
            // 1. Fondo Oscuro (Azul Noche)
            HBRUSH brFondo = CreateSolidBrush(RGB(20, 20, 40));
            RECT rTotal = {0, 0, ancho, alto};
            FillRect(hdcMem, &rTotal, brFondo);
            DeleteObject(brFondo);

            // 2. Título "MANUAL DE GUERRA"
            HFONT hFontTitulo = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
                                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
                                           DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
            HFONT hOldFont = (HFONT)SelectObject(hdcMem, hFontTitulo);
            
            SetBkMode(hdcMem, TRANSPARENT);
            SetTextColor(hdcMem, RGB(255, 215, 0)); // Dorado
            
            RECT rTitulo = {0, 40, ancho, 100};
            DrawText(hdcMem, "MANUAL DE COMANDANTE", -1, &rTitulo, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            // 3. Cuerpo del Texto 
            HFONT hFontTexto = CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                                          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
                                          DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Consolas");
            SelectObject(hdcMem, hFontTexto);
            SetTextColor(hdcMem, RGB(220, 220, 220)); // Blanco humo

            const char* textoInstrucciones = 
                "--- CONTROLES DEL JUGADOR ---\n"
                "W, A, S, D: Mover al personaje\n"
                "ESPACIO: Accion (Atacar, Talar, Minar, Cofres)\n"
                "I: Abrir/Cerrar Mochila\n"
                "T: Abrir Tienda (Cerca del edificio)\n"
                "1, 2, 3: Equipar Herramientas (Hacha, Pico, Espada)\n"
                "R: Curar vida (Consume 1 Hoja)\n\n"
                "--- ESTRATEGIA (RTS) ---\n"
                "Clic Izq (Arrastrar) : Seleccionar Tropas (Cuadro Verde)\n"
                "Clic Der (Terreno)   : Mover Tropas seleccionadas\n"
                "Clic Der (Enemigo)   : Ordenar Ataque\n"
                "Clic Der (Barco)     : Ordenar Subir al Barco (Abordaje)\n\n"
                "--- COMBATE NAVAL ---\n"
                "ESPACIO (En Muelle)  : Subir/Bajar del Barco\n"
                "Clic Izq (En Barco)  : Disparar Canones\n\n"
                "OBJETIVO FINAL: Destruye los castillos enemigos y conquista el archipielago!";

            RECT rTexto = {100, 130, ancho - 100, alto - 100};
            DrawText(hdcMem, textoInstrucciones, -1, &rTexto, DT_CENTER | DT_WORDBREAK);

            // 4. Botón "VOLVER"
            HBRUSH brBoton = CreateSolidBrush(RGB(150, 50, 50)); // Rojo desaturado
            RECT rBoton = {ancho/2 - 100, alto - 80, ancho/2 + 100, alto - 30};
            FillRect(hdcMem, &rBoton, brBoton);
            DeleteObject(brBoton);
            
            // Borde del botón
            HBRUSH brBorde = CreateSolidBrush(RGB(255, 255, 255));
            FrameRect(hdcMem, &rBoton, brBorde);
            DeleteObject(brBorde);

            SetTextColor(hdcMem, RGB(255, 255, 255));
            SelectObject(hdcMem, hFontTitulo); // Reusamos fuente grande pero podríamos achicarla
            
            HFONT hFontBoton = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, "Arial");
            SelectObject(hdcMem, hFontBoton);
            DrawText(hdcMem, "VOLVER", -1, &rBoton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            DeleteObject(hFontBoton);

            SelectObject(hdcMem, hOldFont);
            DeleteObject(hFontTitulo);
            DeleteObject(hFontTexto);
        }
        // --- FINALIZAR DOBLE BUFFER ---
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
        // ESTADO: MENÚ PRINCIPAL
        case ESTADO_MENU:
            if (wParam == VK_RETURN)
                procesarEnterMenu(hwnd, &estadoJuego);
            break;

        // ESTADO: SELECCIÓN DE MAPA
        case ESTADO_SELECCION_MAPA:
            if (wParam == VK_RETURN)
            {
                // --- INICIAR PARTIDA NUEVA ---
                estadoJuego.mapaSeleccionado = estadoJuego.opcionSeleccionada;
                //REGISTRAR EN EL LOG 
        char bufferLog[128];
        const char* nombreMapa = (estadoJuego.mapaSeleccionado == 0) ? "Islas Oceano" : 
                                 (estadoJuego.mapaSeleccionado == 1) ? "Archipielago" : "Continente";
        
        sprintf(bufferLog, "NUEVA PARTIDA INICIADA - Mapa: %s - Slot: %d", nombreMapa, estadoJuego.slotJugado);
        RegistrarLog(bufferLog);

                // Inicializar mapa y colisiones
                inicializarJuego(&miJugador, &estadoJuego, mapaMundo, estadoJuego.mapaSeleccionado);

                // Valores por defecto
                miJugador.nivelMochila = 1;
                miJugador.herramientaActiva = HERRAMIENTA_NADA;
                miJugador.armaduraEquipada = FALSE;
                miJugador.tiendaAbierta = 0;

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
            // --- CONTROLES GAME OVER ---
        case ESTADO_GAME_OVER:
            if (wParam == VK_RETURN) { 
                estadoJuego.estadoActual = ESTADO_SELECCION_MAPA;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            else if (wParam == VK_ESCAPE) { // ESC -> Menú
                estadoJuego.estadoActual = ESTADO_MENU;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;

        // --- CONTROLES VICTORIA ---
        case ESTADO_WINNER:
            if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
                // Volver al juego para seguir looteando
                estadoJuego.estadoActual = ESTADO_PARTIDA;
                // Mover al jugador un poco para que no quede encima de las ruinas
                miJugador.x += 50; 
                
                crearTextoFlotante(miJugador.x, miJugador.y, "La isla es tuya!", 0, RGB(255, 215, 0));
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
        // ESTADO: JUGANDO (PARTIDA EN CURSO)
        case ESTADO_PARTIDA:
            switch (wParam)
            {
            // ---GUARDADO RÁPIDO (F5) ---
            case VK_F5:
                estadoJuego.estadoActual = ESTADO_GUARDANDO;
                strcpy(nombreGuardadoInput, "MiPartida"); // Nombre sugerido
                lenNombre = strlen(nombreGuardadoInput);
                InvalidateRect(hwnd, NULL, FALSE);
                break;

            // --- SALIDA SEGURA (ESC) ---
            case VK_ESCAPE:
            {
                int msg = MessageBox(hwnd,
                                     "Deseas guardar antes de salir al menu?\n\nSI: Guarda como 'AutoSave' y sale.\nNO: Sale sin guardar.\nCANCELAR: Seguir jugando.",
                                     "Salir de la Partida", MB_YESNOCANCEL | MB_ICONQUESTION);

                if (msg == IDYES)
                {
                    GuardarPartida(0, "AutoSave", &miJugador, mapaMundo);
                    estadoJuego.estadoActual = ESTADO_SELECCION_MAPA; // O MENU
                }
                else if (msg == IDNO)
                {
                    estadoJuego.estadoActual = ESTADO_SELECCION_MAPA;
                }
                // Si es CANCEL, no hacemos nada (break)
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break; // Importante el break para no ejecutar el default

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

            // --- INTERFAZ: MOCHILA (I) ---
            case 'I':
            case 'i':
                alternarInventario(&miJugador);
                InvalidateRect(hwnd, NULL, FALSE);
                break;

            // --- INTERFAZ: TIENDA (T) ---
            case 'T':
                if (miJugador.tiendaAbierta)
                {
                    miJugador.tiendaAbierta = 0;
                }
                else
                {
                    float dist = sqrt(pow(miJugador.x - 1450, 2) + pow(miJugador.y - 1900, 2));
                    if (dist < 150)
                    {
                        miJugador.tiendaAbierta = 1;
                        miJugador.modoTienda = 0;
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
                    miJugador.armaduraEquipada = !miJugador.armaduraEquipada;
                    if (miJugador.armaduraEquipada)
                        miJugador.herramientaActiva = HERRAMIENTA_NADA;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                break;

            // --- HERRAMIENTAS: 1, 2, 3 ---
            case '1': // Espada
                if (miJugador.nivelMochila >= 2 && miJugador.tieneEspada)
                {
                    miJugador.herramientaActiva = (miJugador.herramientaActiva == HERRAMIENTA_ESPADA) ? HERRAMIENTA_NADA : HERRAMIENTA_ESPADA;
                    if (miJugador.herramientaActiva)
                        miJugador.armaduraEquipada = FALSE;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                break;
            case '2': // Pico
                if (miJugador.nivelMochila >= 2 && miJugador.tienePico)
                {
                    miJugador.herramientaActiva = (miJugador.herramientaActiva == HERRAMIENTA_PICO) ? HERRAMIENTA_NADA : HERRAMIENTA_PICO;
                    if (miJugador.herramientaActiva)
                        miJugador.armaduraEquipada = FALSE;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                break;
            case '3': // Hacha
                if (miJugador.nivelMochila >= 2 && miJugador.tieneHacha)
                {
                    miJugador.herramientaActiva = (miJugador.herramientaActiva == HERRAMIENTA_HACHA) ? HERRAMIENTA_NADA : HERRAMIENTA_HACHA;
                    if (miJugador.herramientaActiva)
                        miJugador.armaduraEquipada = FALSE;
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
                if (miCamara.zoom < 5)
                    miCamara.zoom++;
                break;
            case VK_SUBTRACT:
            case 0xBD: 
                if (miCamara.zoom > 1)
                    miCamara.zoom--;
                break;

            // --- ACCIÓN (Espacio) ---
            case VK_SPACE:
                talarArbol(&miJugador);
                abrirTesoro(&miJugador);
                golpearVaca(&miJugador);
                picarMina(&miJugador);
                manejarBarcos(&miJugador, mapaMundo);
                golpearEnemigoCercano(&miJugador);
                InvalidateRect(hwnd, NULL, FALSE);
                break;
            }

            // Actualizar cámara tras cualquier input en juego
            actualizarCamara(&miCamara, miJugador);
            InvalidateRect(hwnd, NULL, FALSE);
            break;

        //bbESCRIBIENDO NOMBRE DEL GUARDADO
        case ESTADO_GUARDANDO:
            if (wParam == VK_RETURN) // ENTER -> Confirmar Guardado
            {
                if (lenNombre > 0)
{
    if (estadoJuego.slotJugado == 0) estadoJuego.slotJugado = 1;

    GuardarPartida(estadoJuego.slotJugado, nombreGuardadoInput, &miJugador, mapaMundo);
    
    char msj[64];
    sprintf(msj, "Guardado en Slot %d", estadoJuego.slotJugado);
    MessageBox(hwnd, msj, "Sistema", MB_OK | MB_ICONINFORMATION);
    
    estadoJuego.estadoActual = ESTADO_PARTIDA;
}
            }
            else if (wParam == VK_ESCAPE) // ESC -> Cancelar
            {
                estadoJuego.estadoActual = ESTADO_PARTIDA;
            }
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        return 0;
    }
    case WM_CHAR:
        // Si estamos guardando, capturamos lo que escribas
        if (estadoJuego.estadoActual == ESTADO_GUARDANDO)
        {
            // Tecla Borrar (Backspace)
            if (wParam == VK_BACK && lenNombre > 0)
            {
                nombreGuardadoInput[--lenNombre] = '\0';
            }
            // Escribir letras/números
            else if (lenNombre < 20 && wParam >= 32 && wParam <= 126)
            {
                nombreGuardadoInput[lenNombre++] = (char)wParam;
                nombreGuardadoInput[lenNombre] = '\0';
            }
            InvalidateRect(hwnd, NULL, FALSE); // Repintar para ver los cambios
        }
        break;

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
    inicializarEdificios(); //

    // Verificación de archivos (DEBUG)
    int vacasFaltan = 0;
    for (int i = 0; i < 8; i++)
        if (!hBmpVaca[i])
            vacasFaltan++;
    if (vacasFaltan > 0)
        MessageBox(NULL, "Faltan imagenes de vacas en assets/", "Aviso", MB_OK);

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
            // 2. Lógica del Juego 
            if (estadoJuego.estadoActual == ESTADO_PARTIDA)
            {
            	
            	if (miJugador.cooldownCanon > 0) {
        miJugador.cooldownCanon--;
    }
                // --- A) UNIDADES RTS  ---
                actualizarUnidades(mapaMundo, &miJugador);

                // --- B) FAUNA Y SISTEMAS ---
                actualizarVacas();                   // Movimiento de animales
                actualizarLogicaSistema(&miJugador); // Tiburones, Pesca, etc.
                actualizarRegeneracionRecursos();    // Respawn de árboles/minas/vacas
                actualizarEdificios(0.016f);
                // 1. Actualizar Invasiones 
                actualizarInvasiones(&miJugador);
                // 2. Actualizar IA de los Enemigos
                actualizarIAEnemigos(mapaMundo, &miJugador);
                
				if (miJugador.vidaActual <= 0) {
                estadoJuego.estadoActual = ESTADO_GAME_OVER;
            }

            // 2. DETECTAR VICTORIA (Si destruyes un castillo enemigo)
            // Revisamos el array de edificios enemigos (definido en edificios.c)
            extern Edificio edificiosEnemigos[MAX_EDIFICIOS_ENEMIGOS];
            
            for(int i=0; i<MAX_EDIFICIOS_ENEMIGOS; i++) {
                // Si estaba activo, es enemigo y su vida llegó a 0
                if (edificiosEnemigos[i].activo && edificiosEnemigos[i].esEnemigo && edificiosEnemigos[i].vidaActual <= 0) {
                    
                    // ¡VICTORIA!
                    estadoJuego.estadoActual = ESTADO_WINNER;
                    
                    // Desactivar el edificio para que no vuelva a activar la victoria
                    edificiosEnemigos[i].activo = 0; 
                    
                    // Recompensas
                    miJugador.oro += 500;
                    miJugador.experiencia += 1000;
                }
            }
                // --- C) ANIMACIONES AMBIENTALES ---
                static int timerGato = 0;
                timerGato++;
                if (timerGato > 30)
                {
                    estadoJuego.frameTienda = !estadoJuego.frameTienda;
                    timerGato = 0;
                }
                
            }

            InvalidateRect(hwnd, NULL, FALSE);

            Sleep(16); // Aprox 60 FPS
        }
    }

    LiberarRecursos();
    return 0;
}