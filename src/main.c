/* src/main.c - VERSIÓN CORREGIDA FINAL (SISTEMA COMPLETO) */
#include <windows.h>
#include <stdio.h>
#include <math.h> // Necesario para sqrt y pow
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
int mouseActualX, mouseActualY; // Posición actual mientras arrastras
bool dibujandoCuadro = false;
int mouseStartX, mouseStartY; // Punto donde hiciste clic
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

        // =========================================================
        // 🔴 DEBUG DE COORDENADAS (ACTIVADO TEMPORALMENTE)
        // =========================================================
        // Esta ventana te dirá dónde estás haciendo clic.
        // Úsala para anotar los números y luego BORRA estas 3 líneas.
        //char coordenadas[64];
        //sprintf(coordenadas, "Clic en X: %d, Y: %d", mx, my);
        //MessageBox(hwnd, coordenadas, "Debug Coordenadas", MB_OK);
        // =========================================================

        // ---------------------------------------------------------
        // 1. ESTADO: JUGANDO
        // ---------------------------------------------------------
if (estadoJuego.estadoActual == ESTADO_PARTIDA)
    {
        // --- PRIORIDAD A: COMBATE NAVAL (Si el jugador está en barco de guerra) ---
        if (miJugador.estadoBarco == 2) { 
            if (miJugador.cooldownCanon <= 0) {
                float mouseWorldX = (mx / miCamara.zoom) + miCamara.x;
                float mouseWorldY = (my / miCamara.zoom) + miCamara.y;
                
                crearBalaCanon(miJugador.x, miJugador.y, mouseWorldX, mouseWorldY);
                
                // Lógica de Daño en área
                for(int i = 0; i < MAX_UNIDADES; i++) {
                    if(unidades[i].activa && unidades[i].bando == BANDO_ENEMIGO) {
                        float d = sqrt(pow(unidades[i].x - mouseWorldX, 2) + pow(unidades[i].y - mouseWorldY, 2));
                        if (d < 50) { 
                            unidades[i].vida -= 40;
                            if(unidades[i].vida <= 0) {
                                unidades[i].activa = 0;
                                crearExplosionAgua(unidades[i].x, unidades[i].y);
                            }
                        }
                    }
                }
                
                miJugador.cooldownCanon = 120; // 2 segundos de recarga
                crearTextoFlotante(miJugador.x, miJugador.y - 40, "Recargando...", 0, RGB(200, 200, 200));
            } else {
                crearTextoFlotante(miJugador.x, miJugador.y - 40, "Cañones no listos!", 0, RGB(255, 50, 50));
                PlaySound("SystemHand", NULL, SND_ASYNC);
            }
            return 0; // Salimos para no procesar selección de unidades mientras disparamos
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

        // ---------------------------------------------------------
        // 2. ESTADO: MENÚ PRINCIPAL (Aquí es donde debes ajustar)
        // ---------------------------------------------------------
        else if (estadoJuego.estadoActual == ESTADO_MENU)
        {
            // BOTÓN JUGAR
            // [AJUSTAR]: Cambia los números 340, 460, 200, 240 por los tuyos
            if (mx >= 642 && mx <= 942 && my >= 595 && my <= 678) {
                 estadoJuego.estadoActual = ESTADO_SELECCION_MAPA;
                 InvalidateRect(hwnd, NULL, FALSE);
            }

            // BOTÓN PARTIDAS / CARGAR
            // [AJUSTAR]: Cambia los números según donde dibujaste el botón
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

            // BOTÓN INSTRUCCIONES (¡NUEVO!)
            // [AJUSTAR] Coordenadas (Usualmente debajo de Partidas)
            if (mx >= 1100 && mx <= 1390 && my >= 350 && my <= 455) 
            {
                estadoJuego.estadoActual = ESTADO_INSTRUCCIONES;
                InvalidateRect(hwnd, NULL, FALSE);
            }

            // BOTÓN SALIR
            // [AJUSTAR]: Cambia los números
            if (mx >= 1178 && mx <= 1500 && my >= 688 && my <= 768) {
                 PostQuitMessage(0);
            }
        }

        // ---------------------------------------------------------
        // 3. ESTADO: PANTALLA CARGAR PARTIDA
        // ---------------------------------------------------------
        else if (estadoJuego.estadoActual == ESTADO_MENU_CARGAR)
        {
// DETECTAR CLIC EN SLOTS Y BOTONES DE BORRAR
    for (int i = 1; i <= 3; i++) {
        int yBase = 100 + ((i-1) * 110);
        
        // 1. CLIC EN EL SLOT (CARGAR / CREAR) - (Coordenadas originales)
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

        // 2. CLIC EN BOTÓN BORRAR [X] (Coordenadas nuevas: 660-700)
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
        
        // ---------------------------------------------------------
        // 4. OTROS ESTADOS
        // ---------------------------------------------------------
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

        // =========================================================
        // ESTADO: MENÚ PRINCIPAL
        // =========================================================
        if (estadoJuego.estadoActual == ESTADO_MENU)
        {
            // NOTA: Si quieres mover los botones del menú principal (Jugar, Partidas, Salir),
            // tienes que entrar a esta función 'dibujarMenuConSprites' y ajustarlos ahí.
            dibujarMenuConSprites(hdcMem, hwnd, &estadoJuego);
        }
        // =========================================================
        // ESTADO: SELECCIÓN DE MAPA
        // =========================================================
        else if (estadoJuego.estadoActual == ESTADO_SELECCION_MAPA)
        {
            dibujarSeleccionMapa(hdcMem, hwnd, &estadoJuego);
        }
        // =========================================================
        // ESTADO: PARTIDA EN JUEGO
        // =========================================================
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
            dibujarUnidades(hdcMem, miCamara);

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
        
        // =========================================================
        // ESTADO: PANTALLA DE CARGAR PARTIDA (Historial)
        // =========================================================
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
        // --- NUEVO: BOTÓN DE BORRAR [X] (Solo si existe partida) ---
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

        // =========================================================
        // OVERLAY: PANTALLA DE GUARDADO (Caja Negra con Nombre)
        // =========================================================
        if (estadoJuego.estadoActual == ESTADO_GUARDANDO)
        {
            // Calculamos centro exacto de la pantalla
            int cx = anchoPantalla / 2;
            int cy = altoPantalla / 2;

            // Fondo semitransparente (simulado con rect gris oscuro)
            HBRUSH brFondo = CreateSolidBrush(RGB(20, 20, 20));
            // [AJUSTAR]: Tamaño de la caja de guardado (200px a cada lado del centro)
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
            // [AJUSTAR]: Tamaño del campo de texto
            RECT rCaja = {cx - 150, cy - 20, cx + 150, cy + 20};
            FillRect(hdcMem, &rCaja, brCaja);
            DeleteObject(brCaja);

            // Lo que escribe el usuario
            SetTextColor(hdcMem, RGB(0, 0, 0)); 
            TextOut(hdcMem, cx - 140, cy - 10, nombreGuardadoInput, strlen(nombreGuardadoInput));

            // Cursor parpadeante
            if ((clock() / 500) % 2 == 0)
            {
                TextOut(hdcMem, cx - 140 + (lenNombre * 8), cy - 10, "|", 1);
            }

            // Instrucciones
            SetTextColor(hdcMem, RGB(200, 200, 200));
            SetBkMode(hdcMem, TRANSPARENT); 
            TextOut(hdcMem, cx - 100, cy + 50, "[ENTER] Guardar   [ESC] Cancelar", 32);
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
        // -----------------------------------------------------
        // ESTADO: MENÚ PRINCIPAL
        // -----------------------------------------------------
        case ESTADO_MENU:
            if (wParam == VK_RETURN)
                procesarEnterMenu(hwnd, &estadoJuego);
            break;

        // -----------------------------------------------------
        // ESTADO: SELECCIÓN DE MAPA
        // -----------------------------------------------------
        case ESTADO_SELECCION_MAPA:
            if (wParam == VK_RETURN)
            {
                // --- INICIAR PARTIDA NUEVA ---
                estadoJuego.mapaSeleccionado = estadoJuego.opcionSeleccionada;
                //REGISTRAR EN EL LOG (Añade esto aquí)
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

        // -----------------------------------------------------
        // ESTADO: JUGANDO (PARTIDA EN CURSO)
        // -----------------------------------------------------
        case ESTADO_PARTIDA:
            switch (wParam)
            {
            // --- NUEVO: GUARDADO RÁPIDO (F5) ---
            case VK_F5:
                estadoJuego.estadoActual = ESTADO_GUARDANDO;
                strcpy(nombreGuardadoInput, "MiPartida"); // Nombre sugerido
                lenNombre = strlen(nombreGuardadoInput);
                InvalidateRect(hwnd, NULL, FALSE);
                break;

            // --- MODIFICADO: SALIDA SEGURA (ESC) ---
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
            // case 't':
                if (miJugador.tiendaAbierta)
                {
                    miJugador.tiendaAbierta = 0;
                }
                else
                {
                    // Verificar distancia a la tienda (Coord: 1450, 1900)
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
            case 0xBB: // Tecla + estándar
                if (miCamara.zoom < 5)
                    miCamara.zoom++;
                break;
            case VK_SUBTRACT:
            case 0xBD: // Tecla - estándar
                if (miCamara.zoom > 1)
                    miCamara.zoom--;
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

            // Actualizar cámara tras cualquier input en juego
            actualizarCamara(&miCamara, miJugador);
            InvalidateRect(hwnd, NULL, FALSE);
            break;

        // -----------------------------------------------------
        // NUEVO ESTADO: ESCRIBIENDO NOMBRE DEL GUARDADO
        // -----------------------------------------------------
        case ESTADO_GUARDANDO:
            if (wParam == VK_RETURN) // ENTER -> Confirmar Guardado
            {
                if (lenNombre > 0)
{
    // Si no tenemos slot asignado (ej. pruebas), usar el 1 por defecto
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
            // Escribir letras/números (y evitar desbordamiento)
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
            // 2. Lógica del Juego (Solo si estamos jugando)
            if (estadoJuego.estadoActual == ESTADO_PARTIDA)
            {
            	
            	if (miJugador.cooldownCanon > 0) {
        miJugador.cooldownCanon--;
    }
                // --- A) UNIDADES RTS (Lo más importante) ---
                actualizarUnidades(mapaMundo, &miJugador);

                // --- B) FAUNA Y SISTEMAS ---
                actualizarVacas();                   // Movimiento de animales
                actualizarLogicaSistema(&miJugador); // Tiburones, Pesca, etc.
                actualizarRegeneracionRecursos();    // Respawn de árboles/minas/vacas
                actualizarEdificios(0.016f);
                // 1. Actualizar Invasiones (El Director)
                actualizarInvasiones(&miJugador);
                // 2. Actualizar IA de los Enemigos
                actualizarIAEnemigos(mapaMundo, &miJugador);
                // 3. IA de Aliados (Tus soldados atacando enemigos)
                actualizarUnidades(mapaMundo, &miJugador);

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