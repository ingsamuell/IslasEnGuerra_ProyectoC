/* src/jugador/jugador.c */
#include "jugador.h"
#include "../recursos/recursos.h"
#include "../mundo/mapa.h" // <--- AGREGAR ESTO (Para colisiones)
#include <math.h>          // <--- AGREGAR ESTO (Para distancias)
#include <stdio.h>

// Variables globales para el barco anclado
float barcoGuardadoX = 0;
float barcoGuardadoY = 0;
int barcoGuardadoTipo = 0;  // 0=nada, 1=bote, 2=guerra
int barcoGuardadoActivo = 0;
int barcoGuardadoIsla = 0;  // 0=muelle, 1-4=islas

// =========================================================
// 1. LÓGICA INTERNA
// =========================================================

int obtenerCapacidadMaxima(int nivel) {
    if (nivel == 1) return 99;
    if (nivel == 2) return 499;
    if (nivel == 3) return 999;
    return 99;
}

void agregarRecurso(int *recurso, int cantidad, int nivelMochila) {
    int max = obtenerCapacidadMaxima(nivelMochila);
    *recurso += cantidad;
    
    // Evitar desbordamiento
    if (*recurso > max) *recurso = max;
    if (*recurso < 0) *recurso = 0;
}

void ganarExperiencia(Jugador *j, int cantidad) {
    j->experiencia += cantidad;

    // Usamos un WHILE por si gana tanta XP que sube 2 niveles de golpe
    while (j->experiencia >= j->experienciaSiguienteNivel) {
        // 1. Restar la XP necesaria (Overflow: te quedas con lo que sobra)
        j->experiencia -= j->experienciaSiguienteNivel;
        
        // 2. Subir Nivel
        j->nivel++;
        
        // 3. Calcular siguiente meta (Formula Lineal: Nivel * 100)
        // Nvl 1->2 (100xp), Nvl 2->3 (200xp), Nvl 3->4 (300xp)...
        j->experienciaSiguienteNivel = j->nivel * 100;
        
        // 4. Recompensas (Curar y aumentar vida max)
        j->vidaMax += 10; 
        j->vidaActual = j->vidaMax;

        // 5. Feedback Visual (Texto dorado)
        crearTextoFlotante(j->x, j->y - 50, "SUBIENDO DE NIVEL!", 0, RGB(255, 215, 0));
    }
}

void intentarCurar(Jugador *j) {
    if (j->hojas > 0 && j->vidaActual < j->vidaMax) {
        j->hojas--;
        j->vidaActual += 10;
        if (j->vidaActual > j->vidaMax) j->vidaActual = j->vidaMax;
    }
}

void alternarInventario(Jugador *j) {
    j->inventarioAbierto = !j->inventarioAbierto;
}

// =========================================================
// 2. FUNCIONES VISUALES
// =========================================================

void dibujarItemRejilla(HDC hdc, HBITMAP icono, int cantidad, int maximo, int x, int y, const char *nombre)
{
    if (icono) DibujarImagen(hdc, icono, x, y, 32, 32);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(200, 200, 200));
    TextOut(hdc, x + 40, y - 5, nombre, strlen(nombre));

    char buf[32];
    sprintf(buf, "%d / %d", cantidad, maximo);
    
    if (cantidad >= maximo) SetTextColor(hdc, RGB(255, 100, 100));
    else SetTextColor(hdc, RGB(255, 255, 255));
    
    TextOut(hdc, x + 40, y + 12, buf, strlen(buf));
}

// --- DIBUJADO DEL HUD (Tu diseño: Escudo Arriba, Puño Abajo) ---
void dibujarHUD(HDC hdc, Jugador *jugador, int ancho, int alto)
{
    // A. VIDA (Corazones) -> Y = 20
    for (int i = 0; i < 5; i++) {
        int vida = jugador->vidaActual - (i * 20);
        HBITMAP cor = hBmpCorazon0;
        if (vida >= 20) cor = hBmpCorazon100;
        else if (vida >= 15) cor = hBmpCorazon75;
        else if (vida >= 10) cor = hBmpCorazon50;
        else if (vida >= 5) cor = hBmpCorazon25;
        DibujarImagen(hdc, cor, 20 + (i * 32), 20, 32, 32);
    }

    // B. ESCUDO (Posición original restaurada: Y = 60)
    HBITMAP bmpEscudo = (jugador->armaduraActual > 0) ? hBmpEscudo100 : hBmpEscudo0;
    if (bmpEscudo) DibujarImagen(hdc, bmpEscudo, 20, 60, 48, 48);
    
    char tEscudo[16];
    sprintf(tEscudo, "%d", jugador->armaduraActual);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    TextOut(hdc, 75, 75, tEscudo, strlen(tEscudo));

    // C. BOTÓN MOCHILA -> Y = 120
    HBITMAP bmpBolso = (jugador->inventarioAbierto) ? hBmpInvAbierto : hBmpInvCerrado;
    if (bmpBolso) DibujarImagen(hdc, bmpBolso, 20, 120, 64, 64);

    // D. CAJA DE MANO / ITEM ACTIVO -> Y = 200 (Posición Abajo)
    int boxX = 20;
    int boxY = 200;

    // Dibujar el cuadro gris
    HBRUSH brushCaja = CreateSolidBrush(RGB(50, 50, 60));
    RECT rBox = {boxX, boxY, boxX + 48, boxY + 48};
    FillRect(hdc, &rBox, brushCaja); DeleteObject(brushCaja);
    
    HBRUSH brushBorde = CreateSolidBrush(RGB(200, 200, 200));
    FrameRect(hdc, &rBox, brushBorde); DeleteObject(brushBorde);

    // Determinar qué icono mostrar (Puño o Herramienta)
    HBITMAP iconoMano = hBmpIconoPuno; // Por defecto: PUÑO

    if (jugador->herramientaActiva == HERRAMIENTA_ESPADA) iconoMano = hBmpIconoEspada;
    else if (jugador->herramientaActiva == HERRAMIENTA_PICO) iconoMano = hBmpIconoPico;
    else if (jugador->herramientaActiva == HERRAMIENTA_HACHA) iconoMano = hBmpIconoHacha;
    
    // Dibujar el icono centrado
    if (iconoMano) DibujarImagen(hdc, iconoMano, boxX + 8, boxY + 8, 32, 32);

    if (jugador->inventarioAbierto)
    {
        int px = 90, py = 100; // Subimos un poco el panel
        int anchoFondo = 460;  // Más ancho para que quepa todo
        int alturaFondo = 400; // Más alto para las herramientas grandes

        // Fondo y Marco
        HBRUSH fondo = CreateSolidBrush(RGB(30, 30, 35));
        RECT r = {px, py, px + anchoFondo, py + alturaFondo};
        FillRect(hdc, &r, fondo); DeleteObject(fondo);
        HBRUSH bordeInv = CreateSolidBrush(RGB(255, 215, 0)); // Dorado
        FrameRect(hdc, &r, bordeInv); DeleteObject(bordeInv);

        int startX = px + 20;
        int startY = py + 30;
        int maxCap = obtenerCapacidadMaxima(jugador->nivelMochila);
        int colW = 110; // Ancho de columna
        int rowH = 60;  // Alto de fila

        // --- FILA 1: MATERIALES BÁSICOS ---
        dibujarItemRejilla(hdc, hBmpIconoMadera, jugador->madera, maxCap, startX, startY, "Madera");
        dibujarItemRejilla(hdc, hBmpIconoPiedra, jugador->piedra, maxCap, startX + colW, startY, "Piedra");
        dibujarItemRejilla(hdc, hBmpIconoHierro, jugador->hierro, maxCap, startX + colW*2, startY, "Hierro");
        dibujarItemRejilla(hdc, hBmpIconoOro, jugador->oro, maxCap, startX + colW*3, startY, "Oro");

        // --- FILA 2: CONSUMIBLES Y PESCA ---
        dibujarItemRejilla(hdc, hBmpIconoHoja, jugador->hojas, maxCap, startX, startY + rowH, "Hojas");
        dibujarItemRejilla(hdc, hBmpIconoComida, jugador->comida, maxCap, startX + colW, startY + rowH, "Comida");
        
        // ¡AQUÍ ESTÁ TU PESCADO!
        dibujarItemRejilla(hdc, hBmpIconoPescado, jugador->pescado, maxCap, startX + colW*2, startY + rowH, "Pescado");

        // --- SECCIÓN INFERIOR: HERRAMIENTAS GRANDES ---
        int toolY = startY + (rowH * 2) + 20;
        
        // Título de sección
        SetTextColor(hdc, RGB(255, 215, 0));
        TextOut(hdc, startX, toolY - 20, "HERRAMIENTAS EQUIPABLES", 23);

        // Estructura para dibujar herramientas en bucle
        struct ToolDisplay {
            HBITMAP bmp;
            char *nombre;
            char *tecla;
            BOOL tiene;
            int tipoID;
        };

        struct ToolDisplay tools[] = {
            {hBmpIconoEspada, "Espada", "[1]", jugador->tieneEspada, HERRAMIENTA_ESPADA},
            {hBmpIconoPico,   "Pico",   "[2]", jugador->tienePico,   HERRAMIENTA_PICO},
            {hBmpIconoHacha,  "Hacha",  "[3]", jugador->tieneHacha,  HERRAMIENTA_HACHA},
            {hBmpIconoCana,   "Cana",   "[--]", jugador->tieneCana,  HERRAMIENTA_CANA} // Sin tecla directa, automático en bote
        };

        for (int i = 0; i < 4; i++) {
            int tx = startX + (i * 110);
            
            // Cuadro de fondo de la herramienta
            RECT rT = {tx, toolY, tx + 64, toolY + 64};
            HBRUSH bgT = CreateSolidBrush(tools[i].tiene ? RGB(60, 60, 70) : RGB(20, 20, 20));
            FillRect(hdc, &rT, bgT); DeleteObject(bgT);
            
            // Borde (Verde si está equipada)
            if (jugador->herramientaActiva == tools[i].tipoID && tools[i].tipoID != 0) {
                HBRUSH brActiva = CreateSolidBrush(RGB(0, 255, 0));
                FrameRect(hdc, &rT, brActiva); DeleteObject(brActiva);
            } else {
                HBRUSH brNormal = CreateSolidBrush(RGB(100, 100, 100));
                FrameRect(hdc, &rT, brNormal); DeleteObject(brNormal);
            }

            // Icono Grande (48x48)
            if (tools[i].tiene && tools[i].bmp) {
                DibujarImagen(hdc, tools[i].bmp, tx + 8, toolY + 8, 48, 48);
                
                // Texto Nombre
                SetTextColor(hdc, RGB(255, 255, 255));
                TextOut(hdc, tx, toolY + 70, tools[i].nombre, strlen(tools[i].nombre));
                
                // Texto Tecla (Amarillo)
                SetTextColor(hdc, RGB(255, 255, 0));
                TextOut(hdc, tx + 40, toolY + 2, tools[i].tecla, strlen(tools[i].tecla));
            } else {
                // Bloqueado
                SetTextColor(hdc, RGB(80, 80, 80));
                TextOut(hdc, tx + 10, toolY + 25, "Bloq.", 5);
            }
        }
        
        // Info de nivel mochila
        char tNivel[32]; sprintf(tNivel, "Capacidad: %d items (Nvl %d)", maxCap, jugador->nivelMochila);
        SetTextColor(hdc, RGB(0, 255, 255));
        TextOut(hdc, px + 20, py + alturaFondo - 25, tNivel, strlen(tNivel));
    }

    // F. BARRA XP (CORREGIDO)
    if (hBmpBarraXPVacia && hBmpBarraXPLlena) {
        BITMAP bm; 
        GetObject(hBmpBarraXPVacia, sizeof(BITMAP), &bm);
        
        // F. BARRA XP (DISEÑO POR CÓDIGO - SIN BMP)
    
    // 1. Configuración de dimensiones
    int anchoBarra = 400; // Largo de la barra
    int altoBarra = 18;   // Grosor de la barra
    int xpX = (ancho - anchoBarra) / 2; // Centrado horizontal
    int xpY = 10; // Pegado arriba

    // 2. Dibujar FONDO (Gris Oscuro)
    RECT rFondo = {xpX, xpY, xpX + anchoBarra, xpY + altoBarra};
    HBRUSH brFondo = CreateSolidBrush(RGB(40, 40, 40)); // Gris muy oscuro
    FillRect(hdc, &rFondo, brFondo);
    DeleteObject(brFondo);

    // 3. Dibujar RELLENO (Cyan Brillante)
    // Prevenimos división por cero
    int metaXP = (jugador->experienciaSiguienteNivel > 0) ? jugador->experienciaSiguienteNivel : 100;
    
    float pct = (float)jugador->experiencia / metaXP;
    if (pct > 1) pct = 1;
    if (pct < 0) pct = 0;
    
    int fillW = (int)(anchoBarra * pct); // Ancho proporcional
    
    if (fillW > 0) {
        RECT rRelleno = {xpX, xpY, xpX + fillW, xpY + altoBarra};
        // Puedes cambiar este color RGB(0, 200, 255) a Amarillo o Verde si prefieres
        HBRUSH brRelleno = CreateSolidBrush(RGB(0, 200, 255)); 
        FillRect(hdc, &rRelleno, brRelleno);
        DeleteObject(brRelleno);
    }

    // 4. Dibujar BORDE (Blanco)
    HBRUSH brBorde = CreateSolidBrush(RGB(200, 200, 200));
    FrameRect(hdc, &rFondo, brBorde);
    DeleteObject(brBorde);

    // 5. TEXTOS (Con sombra para que se lean bien)
    SetBkMode(hdc, TRANSPARENT);
    
    // --- Texto "NIVEL X" a la izquierda ---
    char tNivel[32]; 
    sprintf(tNivel, "NVL %d", jugador->nivel);
    
    // Sombra negra (offset +1, +1)
    SetTextColor(hdc, RGB(0,0,0));
    TextOut(hdc, xpX - 65 + 1, xpY + 1 + 1, tNivel, strlen(tNivel));
    // Texto real (Dorado)
    SetTextColor(hdc, RGB(255, 215, 0)); 
    TextOut(hdc, xpX - 65, xpY + 1, tNivel, strlen(tNivel));

    // --- Texto "XX / 100" dentro de la barra ---
    char tProgreso[32];
    sprintf(tProgreso, "%d / %d", jugador->experiencia, metaXP);
    
    // Centrar texto matemáticamente
    SIZE size;
    GetTextExtentPoint32(hdc, tProgreso, strlen(tProgreso), &size);
    int textX = xpX + (anchoBarra - size.cx) / 2;
    int textY = xpY + (altoBarra - size.cy) / 2;

    // Sombra negra
    SetTextColor(hdc, RGB(0,0,0));
    TextOut(hdc, textX + 1, textY + 1, tProgreso, strlen(tProgreso));
    // Texto real (Blanco)
    SetTextColor(hdc, RGB(255, 255, 255)); 
    TextOut(hdc, textX, textY, tProgreso, strlen(tProgreso));
    }

    // Muestra la posición X, Y actual en la esquina superior derecha
    char tDebug[64];
    sprintf(tDebug, "POS: %d, %d", (int)jugador->x, (int)jugador->y);
    
    // Usamos color MAGENTA brillante para que resalte sobre cualquier fondo
    SetTextColor(hdc, RGB(255, 27, 45)); 
    SetBkMode(hdc, TRANSPARENT);
    
    // Lo dibujamos arriba a la derecha (ancho - 150px)
    TextOut(hdc, ancho - 150, 20, tDebug, strlen(tDebug));
}

void dibujarJugador(HDC hdc, Jugador *jugador, Camera cam)
{
    // 1. CÁLCULO DE POSICIÓN Y ESCALADO
    int tam = 32 * cam.zoom;
    RECT r; 
    GetClipBox(hdc, &r);
    
    int cx = (r.right / 2) - (tam / 2);
    int cy = (r.bottom / 2) - (tam / 2);

    // =========================================================
    // CASO 1: ESTADO NAVEGACIÓN (Barco o Bote)
    // =========================================================
if (jugador->estadoBarco > 0) {
    HBITMAP imgBarco = NULL;
    
    // A) Barra de Vida (Siempre arriba)
    dibujarBarraVidaLocal(hdc, cx - 10, cy - 25, jugador->vidaActual, jugador->vidaMax, 50 * cam.zoom);
    
    int dir = (jugador->direccion == DIR_IZQUIERDA) ? 0 : 1; 
    
    // ============================================
    // ¡DIFERENCIAR TAMAÑOS! 
    // ============================================
    if (jugador->estadoBarco == 1) { 
        // BOTE DE PESCA - MÁS PEQUEÑO
        imgBarco = hBmpBote[dir];
        if (imgBarco) {
            // Tamaño reducido: 60x48 en lugar de 80x64
            DibujarImagen(hdc, imgBarco, cx - 8, cy - 6, 50 * cam.zoom, 40 * cam.zoom);
            //                          ↑ajuste X  ↑ajuste Y  ↑ancho menor ↑alto menor
        }
        SetTextColor(hdc, RGB(0, 255, 255));
        TextOut(hdc, cx, cy - 40, "PESCANDO", 8);
    } 
    else if (jugador->estadoBarco == 2) { 
        // BARCO DE GUERRA - TAMAÑO ORIGINAL
        imgBarco = hBmpBarco[dir];
        if (imgBarco) {
            // Tamaño original: 80x64
            DibujarImagen(hdc, imgBarco, cx - 16, cy - 16, 80 * cam.zoom, 64 * cam.zoom);
            //                          ↑original   ↑original   ↑ancho orig ↑alto orig
        }
        
        // === BARRA DE RECARGA DE CAÑÓN ===
        int barW = 50 * cam.zoom;
        int barH = 6 * cam.zoom;
        int bx = cx - (10 * cam.zoom);
        int by = cy - (40 * cam.zoom);

        // Fondo de la barra
        HBRUSH bg = CreateSolidBrush(RGB(50, 50, 50));
        RECT rBg = {bx, by, bx + barW, by + barH};
        FillRect(hdc, &rBg, bg);
        DeleteObject(bg);

        // Cálculo de recarga
        float pct = 1.0f - ((float)jugador->cooldownCanon / 120.0f);
        if (pct < 0) { pct = 0; } 
        if (pct > 1) { pct = 1; }

        COLORREF colorBarra = (pct >= 1.0f) ? RGB(0, 255, 255) : RGB(0, 0, 150);
        HBRUSH fg = CreateSolidBrush(colorBarra);
        RECT rFg = {bx, by, bx + (int)(barW * pct), by + barH};
        FillRect(hdc, &rFg, fg);
        DeleteObject(fg);
        
        if (pct >= 1.0f) {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 255, 255));
            TextOut(hdc, bx + 5, by - (15 * cam.zoom), "LISTO", 5);
        }
    }
    return; 
}
    // =========================================================
    // CASO 2: ESTADO A PIE
    // =========================================================
    
    // --- A) LÓGICA DE EMBESTIDA ---
    if (jugador->timerAtaque > 10) {
        int empujeSutil = 8 * cam.zoom; 
        switch(jugador->direccion) {
            case DIR_ABAJO:     cy += empujeSutil; break;
            case DIR_ARRIBA:    cy -= empujeSutil; break;
            case DIR_IZQUIERDA: cx -= empujeSutil; break;
            case DIR_DERECHA:   cx += empujeSutil; break;
        }
    }

    // --- B) SELECCIÓN DE SPRITE ---
    HBITMAP spriteFinal = hBmpJugador; 
    int dir = jugador->direccion;
    int anim = jugador->frameAnim;

    if (jugador->armaduraEquipada) {
        if (hBmpArmaduraAnim[dir][anim]) spriteFinal = hBmpArmaduraAnim[dir][anim];
    } else {
        switch (jugador->herramientaActiva) {
            case HERRAMIENTA_ESPADA: if (hBmpEspadaAnim[dir][anim]) spriteFinal = hBmpEspadaAnim[dir][anim]; break;
            case HERRAMIENTA_PICO:   if (hBmpPicoAnim[dir][anim])   spriteFinal = hBmpPicoAnim[dir][anim]; break;
            case HERRAMIENTA_HACHA:  if (hBmpHachaAnim[dir][anim])  spriteFinal = hBmpHachaAnim[dir][anim]; break;
            default:                 if (hBmpJugadorAnim[dir][anim])spriteFinal = hBmpJugadorAnim[dir][anim]; break;
        }
    }

    // --- C) EFECTO VISUAL DE ATAQUE (Slash) ---
    if (jugador->timerAtaque > 0) { 
        int xCentro = cx + (tam / 2);
        int yCentro = cy + (tam / 2);
        int alcance = 35 * cam.zoom;
        
        HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 200));
        HGDIOBJ old = SelectObject(hdc, hPen);
        
        if (dir == DIR_DERECHA) {
            MoveToEx(hdc, xCentro, yCentro - 10, NULL);
            LineTo(hdc, xCentro + alcance, yCentro + 10);
        } else if (dir == DIR_IZQUIERDA) {
            MoveToEx(hdc, xCentro, yCentro - 10, NULL);
            LineTo(hdc, xCentro - alcance, yCentro + 10);
        } else if (dir == DIR_ARRIBA) {
            MoveToEx(hdc, xCentro - 10, yCentro, NULL);
            LineTo(hdc, xCentro + 10, yCentro - alcance);
        } else if (dir == DIR_ABAJO) {
            MoveToEx(hdc, xCentro - 10, yCentro, NULL);
            LineTo(hdc, xCentro + 10, yCentro + alcance);
        }
        
        SelectObject(hdc, old);
        DeleteObject(hPen);
    }

    // --- D) RENDERIZADO FINAL DEL PERSONAJE ---
    DibujarImagen(hdc, spriteFinal, cx, cy, tam, tam);
}

// =========================================================
// MOVIMIENTO Y CÁMARA (Migrado de mapa.c)
// =========================================================

void moverJugador(Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], int dx, int dy) {
    // 1. Orientación
    if (dy > 0) j->direccion = 0; // Abajo
    else if (dy < 0) j->direccion = 1; // Arriba
    else if (dx < 0) j->direccion = 2; // Izquierda
    else if (dx > 0) j->direccion = 3; // Derecha

    // 2. Animación
    if (dx != 0 || dy != 0) {
        j->pasoAnimacion++;
        int estado = (j->pasoAnimacion / 4) % 4;
        j->frameAnim = (estado == 0) ? 1 : ((estado == 2) ? 2 : 0);
    } else {
        j->frameAnim = 1; // Quieto
    }

    // 3. Límites del mundo
    int centroX = 1600; int centroY = 1600; int radioSeguro = 900;

    // --- EJE X ---
    int futuroX = j->x + (dx * j->velocidad);
    
    // VERIFICACIÓN CON 2 PUNTOS (PIES y CABEZA)
    int esTierraPiesX = EsSuelo(futuroX + 16, j->y + 16, mapa);  // Punto actual (pies)
    int esTierraCabezaX = EsSuelo(futuroX + 8, j->y + 1, mapa);  // Nuevo punto (cabeza)
    
    // Para estar en tierra: AMBOS puntos deben ser tierra
    BOOL puedeX = (j->estadoBarco == 0 && esTierraPiesX && esTierraCabezaX) || 
                  (j->estadoBarco > 0 && !esTierraPiesX && !esTierraCabezaX);
    
    // Límite Bote (solo para bote de pesca, no para barco de guerra)
    if (puedeX && j->estadoBarco == 1) {
        if (sqrt(pow(futuroX - centroX, 2) + pow(j->y - centroY, 2)) > radioSeguro) puedeX = FALSE;
    }
    
    if (puedeX) j->x = futuroX;

    // --- EJE Y ---
    int futuroY = j->y + (dy * j->velocidad);
    
    // VERIFICACIÓN CON 2 PUNTOS (PIES y CABEZA)
    int esTierraPiesY = EsSuelo(j->x + 16, futuroY + 16, mapa);
    int esTierraCabezaY = EsSuelo(j->x + 8, futuroY + 1, mapa);
    
    BOOL puedeY = (j->estadoBarco == 0 && esTierraPiesY && esTierraCabezaY) || 
                  (j->estadoBarco > 0 && !esTierraPiesY && !esTierraCabezaY);
    
    if (puedeY && j->estadoBarco == 1) {
        if (sqrt(pow(j->x - centroX, 2) + pow(futuroY - centroY, 2)) > radioSeguro) puedeY = FALSE;
    }
    
    if (puedeY) j->y = futuroY;

    // --- ¡NUEVO! Aplicar sistema de corrientes ---
    mantenerDentroDelMapa(j);
}

void actualizarCamara(Camera *cam, Jugador j) {
    int anchoPantalla = GetSystemMetrics(SM_CXSCREEN);
    int altoPantalla = GetSystemMetrics(SM_CYSCREEN);
    
    // Centrar
    cam->x = j.x - (anchoPantalla / 2 / cam->zoom);
    cam->y = j.y - (altoPantalla / 2 / cam->zoom);
    
    // --- LÍMITES CON 100px DE MARGEN ---
    int margen = 700;  // ¡CAMBIADO DE 700 A 100!
    
    // Mínimos: -100px
    if (cam->x < -margen) cam->x = -margen;
    if (cam->y < -margen) cam->y = -margen;
    
    // Máximos: 3200 + 100px
    int anchoVisible = anchoPantalla / cam->zoom;
    int altoVisible = altoPantalla / cam->zoom;
    
    int maxX = MUNDO_ANCHO - anchoVisible + margen;
    int maxY = MUNDO_ALTO - altoVisible + margen;
    
    if (cam->x > maxX) cam->x = maxX;
    if (cam->y > maxY) cam->y = maxY;
}

void mantenerDentroDelMapa(Jugador *j) {
    // Área jugable exacta (3200×3200)
    int minX = 0;
    int minY = 0;
    int maxX = MUNDO_ANCHO - 32;  // 3200 - 32 (tamaño sprite)
    int maxY = MUNDO_ALTO - 32;   // 3200 - 32
    
    // Zona de corriente (100px fuera del mapa)
    int zonaCorriente = 100;
    
    // Fuerza de la corriente (ajustable)
    float fuerza = 2.5f;
    
    // -----------------------------------------------------
    // 1. OESTE (x < 0)
    // -----------------------------------------------------
    if (j->x < minX - zonaCorriente) {
        // No permitir ir más allá de -100px
        j->x = minX - zonaCorriente;
    }
    else if (j->x < minX) {
        // Entre -100px y 0px: corriente hacia adentro (ESTE)
        j->x += fuerza;
    }
    
    // -----------------------------------------------------
    // 2. NORTE (y < 0)
    // -----------------------------------------------------
    if (j->y < minY - zonaCorriente) {
        j->y = minY - zonaCorriente;
    }
    else if (j->y < minY) {
        // Corriente hacia adentro (SUR)
        j->y += fuerza;
    }
    
    // -----------------------------------------------------
    // 3. ESTE (x > 3168)
    // -----------------------------------------------------
    if (j->x > maxX + zonaCorriente) {
        j->x = maxX + zonaCorriente;
    }
    else if (j->x > maxX) {
        // Corriente hacia adentro (OESTE)
        j->x -= fuerza;
    }
    
    // -----------------------------------------------------
    // 4. SUR (y > 3168)
    // -----------------------------------------------------
    if (j->y > maxY + zonaCorriente) {
        j->y = maxY + zonaCorriente;
    }
    else if (j->y > maxY) {
        // Corriente hacia adentro (NORTE)
        j->y -= fuerza;
    }
}

void intentarMontarBarco(Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    // Coordenadas EXACTAS del muelle principal
    #define MUELLE_X 2050
    #define MUELLE_Y 1600
    #define RADIO_PERMITIDO 100 
    
    float dx = j->x - MUELLE_X;
    float dy = j->y - MUELLE_Y;
    float distancia = sqrt(dx*dx + dy*dy);
    
    // ============================================
    // 1. CASO: ESTÁ EN BARCO → BAJAR (Desembarcar)
    // ============================================
    if (j->estadoBarco > 0) {
        if (distancia < RADIO_PERMITIDO) {
            j->estadoBarco = 0;  // Pie
            j->x = MUELLE_X + 30; 
            j->y = MUELLE_Y + 20;
            crearTextoFlotante(j->x, j->y, "Desembarcado", 0, RGB(0, 255, 200));
        } else {
            crearTextoFlotante(j->x, j->y, "Ve al muelle para bajar", 0, RGB(255, 50, 50));
        }
        return;
    }
    
    // ============================================
    // 2. CASO: ESTÁ A PIE → ELEGIR BARCO
    // ============================================
    if (distancia < RADIO_PERMITIDO) {
        
        // A) TIENE LOS DOS BARCOS -> PREGUNTAR
        if (j->tieneBotePesca && j->cantBarcosGuerra > 0) {
            
            // Pausar un momento para preguntar
            HWND hwnd = GetActiveWindow();
            int eleccion = MessageBox(hwnd, 
                "Tienes dos tipos de embarcaciones disponibles.\n\n"
                "Cual quieres usar?\n"
                "SI: Barco de Guerra (Canones)\n"
                "NO: Bote de Pesca (Cana)\n"
                "CANCELAR: Quedarse en tierra", 
                "Elegir Navio", MB_YESNOCANCEL | MB_ICONQUESTION);

            if (eleccion == IDYES) {
                // ELEGIR GUERRA
                j->estadoBarco = 2; 
                j->x += 60;
                crearTextoFlotante(j->x, j->y, "A la batalla!", 0, RGB(255, 150, 0));
            } 
            else if (eleccion == IDNO) {
                // ELEGIR PESCA
                j->estadoBarco = 1; 
                j->x += 60;
                crearTextoFlotante(j->x, j->y, "Dia de pesca...", 0, RGB(0, 255, 255));
            }
            // Si es CANCEL, no hace nada
        }
        
        // B) SOLO TIENE BARCO DE GUERRA
        else if (j->cantBarcosGuerra > 0) {
            j->estadoBarco = 2;
            j->x += 60;
            crearTextoFlotante(j->x, j->y, "Barco de Guerra", 0, RGB(255, 200, 0));
        }
        
        // C) SOLO TIENE BOTE DE PESCA
        else if (j->tieneBotePesca) {
            j->estadoBarco = 1; 
            j->x += 60;
            crearTextoFlotante(j->x, j->y, "Bote de Pesca", 0, RGB(0, 200, 255));
        }
        
        // D) NO TIENE NADA
        else {
            crearTextoFlotante(j->x, j->y, "Compra un barco en la tienda!", 0, RGB(255, 100, 100));
        }
    } 
}

// =========================================================
// 1. DIBUJAR BARCO ANCLADO (Visible donde lo dejaste)
// =========================================================
void dibujarBarcoAnclado(HDC hdc, Camera cam) {
    if (!barcoGuardadoActivo) return;
    
    // Calcular posición en pantalla
    int sx = (int)((barcoGuardadoX - cam.x) * cam.zoom);
    int sy = (int)((barcoGuardadoY - cam.y) * cam.zoom);
    
    // ¡DIFERENCIAR TAMAÑOS TAMBIÉN AQUÍ!
    int tamAncho = 0, tamAlto = 0;
    int offsetX = 0, offsetY = 0;
    HBITMAP imgBarco = NULL;
    int dir = 1; // Derecha por defecto
    
    if (barcoGuardadoTipo == 1) {
        // BOTE DE PESCA ANCLADO - MUY PEQUEÑO (50×40)
        imgBarco = hBmpBote[dir];
        tamAncho = 50 * cam.zoom;  // Ancho 50px
        tamAlto = 40 * cam.zoom;   // Alto 40px
        offsetX = tamAncho / 2;    // 25
        offsetY = tamAlto / 2;     // 20
    } else if (barcoGuardadoTipo == 2) {
        // BARCO DE GUERRA ANCLADO - GRANDE (80×64)
        imgBarco = hBmpBarco[dir];
        tamAncho = 80 * cam.zoom;  // Ancho 80px
        tamAlto = 64 * cam.zoom;   // Alto 64px
        offsetX = tamAncho / 2;    // 40
        offsetY = tamAlto / 2;     // 32
    }
    
    // Dibujar el barco si está en pantalla
    if (imgBarco && sx > -tamAncho && sx < 2000 && sy > -tamAlto && sy < 1500) {
        DibujarImagen(hdc, imgBarco, sx - offsetX, sy - offsetY, tamAncho, tamAlto);
        //                    ↑centrado según tamaño diferente ancho/alto
    }
}

// =========================================================
// 2. DESEMBARCAR EN PLAYA (Nueva función separada)
// =========================================================
int desembarcarEnPlaya(Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    // Solo si está en barco
    if (j->estadoBarco == 0) return 0;
    
    // Referencia a las islas (de mapa.c)
    extern Isla misIslas[MAX_ISLAS];
    
    // 1. Verificar si está cerca de alguna isla secundaria (1-4)
    for (int i = 1; i <= 4; i++) {
        if (!misIslas[i].activa) continue;
        
        // Calcular centro aproximado de la isla
        int centroIslaX = misIslas[i].x + (misIslas[i].ancho / 2);
        int centroIslaY = misIslas[i].y + (misIslas[i].alto / 2);
        
        // Calcular distancia al borde de la isla (radio aproximado)
        float radioIsla = (misIslas[i].ancho + misIslas[i].alto) / 4;
        float dist = sqrt(pow(j->x - centroIslaX, 2) + pow(j->y - centroIslaY, 2));
        
        // Si está cerca del borde de la isla (en el agua, pero cerca)
        if (dist < radioIsla + 100 && dist > radioIsla - 50) {
            // Buscar un punto de tierra cerca para desembarcar
            int puntoTierraX = 0, puntoTierraY = 0;
            int encontrado = 0;
            
            // Buscar en las 8 direcciones alrededor del barco
            int offsets[8][2] = {
                {-50, 0}, {50, 0}, {0, -50}, {0, 50},
                {-35, -35}, {35, -35}, {-35, 35}, {35, 35}
            };
            
            for (int d = 0; d < 8 && !encontrado; d++) {
                int testX = (int)j->x + offsets[d][0];
                int testY = (int)j->y + offsets[d][1];
                
                if (EsSuelo(testX, testY, mapa)) {
                    puntoTierraX = testX;
                    puntoTierraY = testY;
                    encontrado = 1;
                    
                    // Guardar posición del barco (en el agua donde está)
                    barcoGuardadoX = j->x;
                    barcoGuardadoY = j->y;
                    barcoGuardadoTipo = j->estadoBarco;
                    barcoGuardadoActivo = 1;
                    barcoGuardadoIsla = i;
                    
                    // Desembarcar jugador
                    j->estadoBarco = 0;
                    j->x = puntoTierraX;
                    j->y = puntoTierraY;
                    
                    // Feedback
                    char mensaje[64];
                    sprintf(mensaje, "Barco anclado en Isla %d", i);
                    crearTextoFlotante(j->x, j->y, mensaje, 0, RGB(0, 255, 255));
                    
                    // Descubrir área
                    descubrirMapa(j->x, j->y, 120.0f);
                    return 1;
                }
            }
        }
    }
    
    // Si no encontró playa, mostrar mensaje
    // crearTextoFlotante(j->x, j->y, "Acercate a una playa para anclar", 0, RGB(255, 100, 100));
    return 0;
}

// =========================================================
// 3. RE-EMBARCAR DESDE PLAYA
// =========================================================
int reembarcarDesdePlaya(Jugador *j) {
    // Solo si está a pie y hay barco guardado
    if (j->estadoBarco != 0) return 0;
    if (!barcoGuardadoActivo) return 0;
    
    // Calcular distancia al barco guardado
    float dist = sqrt(pow(j->x - barcoGuardadoX, 2) + pow(j->y - barcoGuardadoY, 2));
    
    if (dist < 100) { // Cerca del barco
        // Embarcar de nuevo
        j->estadoBarco = barcoGuardadoTipo;
        j->x = barcoGuardadoX;
        j->y = barcoGuardadoY;
        barcoGuardadoActivo = 0; // El barco ya no se dibuja aparte
        
        crearTextoFlotante(j->x, j->y, "Zarpando!", 0, RGB(0, 255, 255));
        return 1;
    }
    
    // Mostrar dirección aproximada
    float dx = barcoGuardadoX - j->x;
    float dy = barcoGuardadoY - j->y;
    
    char direccion[32];
    if (fabs(dx) > fabs(dy)) {
        strcpy(direccion, (dx > 0) ? "Este" : "Oeste");
    } else {
        strcpy(direccion, (dy > 0) ? "Sur" : "Norte");
    }
    
    return 0;
}

// =========================================================
// 4. FUNCIÓN UNIFICADA PARA MANEJAR BARCOS (MODIFICAR)
// =========================================================
void manejarBarcos(Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    // ============================================
    // A. SI ESTÁ A PIE
    // ============================================
    if (j->estadoBarco == 0) {
        // 1. Primero ver si puede subir a su barco anclado
        if (barcoGuardadoActivo) {
            if (reembarcarDesdePlaya(j)) {
                return; // Subió a su barco anclado
            }
        }
        
        // 2. Si no, usar la lógica original (muelle central)
        intentarMontarBarco(j, mapa);
        return;
    }
    
    // ============================================
    // B. SI ESTÁ EN BARCO
    // ============================================
    if (j->estadoBarco > 0) {
        // 1. Intentar desembarcar en playa (nuevo)
        if (desembarcarEnPlaya(j, mapa)) {
            return; // Desembarcó en playa
        }
        
        // 2. Si no pudo en playa, usar lógica original (muelle)
        intentarMontarBarco(j, mapa);
    }
}