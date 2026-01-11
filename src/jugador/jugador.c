/* src/jugador/jugador.c */
#include "jugador.h"
#include "../recursos/recursos.h"
#include "../mundo/mapa.h" // <--- AGREGAR ESTO (Para colisiones)
#include <math.h>          // <--- AGREGAR ESTO (Para distancias)
#include <stdio.h>

// =========================================================
// 1. LÓGICA INTERNA
// =========================================================

int obtenerCapacidadMaxima(int nivel) {
    if (nivel == 1) return 99;
    if (nivel == 2) return 499;
    if (nivel == 3) return 999;
    return 99;
}

// ESTA ES LA FUNCIÓN QUE MAPA.C ESTABA BUSCANDO
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
        crearTextoFlotante(j->x, j->y - 50, "¡SUBIENDO DE NIVEL!", 0, RGB(255, 215, 0));
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

    // E. INVENTARIO ABIERTO (Rejilla)
    if (jugador->inventarioAbierto)
    {
        int px = 90, py = 120;
        int anchoFondo = 360;
        int alturaFondo = 300;

        HBRUSH fondo = CreateSolidBrush(RGB(30, 30, 35));
        RECT r = {px, py, px + anchoFondo, py + alturaFondo};
        FillRect(hdc, &r, fondo); DeleteObject(fondo);
        
        HBRUSH bordeInv = CreateSolidBrush(RGB(255, 255, 255));
        FrameRect(hdc, &r, bordeInv); DeleteObject(bordeInv);

        int startX = px + 20;
        int startY = py + 40;
        int maxCap = obtenerCapacidadMaxima(jugador->nivelMochila);

        // Fila 1
        dibujarItemRejilla(hdc, hBmpIconoMadera, jugador->madera, maxCap, startX, startY, "Madera");
        dibujarItemRejilla(hdc, hBmpIconoPiedra, jugador->piedra, maxCap, startX + 110, startY, "Piedra");
        dibujarItemRejilla(hdc, hBmpIconoHierro, jugador->hierro, maxCap, startX + 220, startY, "Hierro");

        // Fila 2
        dibujarItemRejilla(hdc, hBmpIconoHoja, jugador->hojas, maxCap, startX, startY + 60, "Hojas");
        dibujarItemRejilla(hdc, hBmpIconoComida, jugador->comida, maxCap, startX + 110, startY + 60, "Comida");
        dibujarItemRejilla(hdc, hBmpIconoOro, jugador->oro, maxCap, startX + 220, startY + 60, "Oro");

        // Fila 3 (Herramientas)
        if (jugador->nivelMochila >= 2) {
            if (jugador->tieneHacha) DibujarImagen(hdc, hBmpIconoHacha, startX, startY + 120, 32, 32);
            else { SetTextColor(hdc, RGB(100,100,100)); TextOut(hdc, startX, startY+130, "Bloq.", 5); }

            if (jugador->tienePico) DibujarImagen(hdc, hBmpIconoPico, startX + 110, startY + 120, 32, 32);
            else { SetTextColor(hdc, RGB(100,100,100)); TextOut(hdc, startX+110, startY+130, "Bloq.", 5); }

            if (jugador->tieneEspada) DibujarImagen(hdc, hBmpIconoEspada, startX + 220, startY + 120, 32, 32);
            else { SetTextColor(hdc, RGB(100,100,100)); TextOut(hdc, startX+220, startY+130, "Bloq.", 5); }
            
            SetTextColor(hdc, RGB(150, 150, 150));
            TextOut(hdc, startX, startY + 160, "Usa 1, 2, 3 para equipar", 24);
        } else {
            SetTextColor(hdc, RGB(100, 100, 100));
            TextOut(hdc, startX, startY + 130, "Nivel 2 para desbloquear armas", 30);
        }
        
        char tNivel[32]; sprintf(tNivel, "Mochila Nivel %d", jugador->nivelMochila);
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
}

// --- ARREGLADO: LLAVES EN LOS IF PARA EVITAR WARNINGS ---

void dibujarJugador(HDC hdc, Jugador *jugador, Camera cam)
{
    int tam = 32 * cam.zoom;
    RECT r; GetClipBox(hdc, &r);
    int cx = (r.right / 2) - (tam / 2);
    int cy = (r.bottom / 2) - (tam / 2);

    // =========================================================
    // CASO 1: ESTÁ EN BARCO (Dibujar SOLO barco y SALIR)
    // =========================================================
    if (jugador->estadoBarco > 0) {
        HBITMAP imgBarco = NULL;
        dibujarBarraVidaLocal(hdc, cx - 10, cy - 25, jugador->vidaActual, jugador->vidaMax, 50 * cam.zoom);
        int dir = (jugador->direccion == DIR_IZQUIERDA) ? 0 : 1; 
        
        // Determinar imagen según tipo de barco
        if (jugador->estadoBarco == 1) imgBarco = hBmpBote[dir];       
        else if (jugador->estadoBarco == 2) imgBarco = hBmpBarco[dir]; 
        
        if (imgBarco) {
            // Dibujamos el barco centrado
            DibujarImagen(hdc, imgBarco, cx - 16, cy - 16, 80 * cam.zoom, 64 * cam.zoom);
        }
        
        if (jugador->estadoBarco == 1) {
            SetTextColor(hdc, RGB(0, 255, 255));
            TextOut(hdc, cx, cy - 40, "PESCANDO", 8);
        }

        return; // Detiene la función aquí si está en barco
    }

    // =========================================================
    // CASO 2: A PIE (Dibujar Personaje normal)
    // =========================================================
    HBITMAP spriteFinal = hBmpJugador; // Default
    int dir = jugador->direccion;
    int anim = jugador->frameAnim;

    if (jugador->armaduraEquipada) {
        if (hBmpArmaduraAnim[dir][anim]) spriteFinal = hBmpArmaduraAnim[dir][anim];
    } else {
        switch (jugador->herramientaActiva) {
            case HERRAMIENTA_ESPADA:
                if (hBmpEspadaAnim[dir][anim]) { spriteFinal = hBmpEspadaAnim[dir][anim]; }
                break;
            case HERRAMIENTA_PICO:
                if (hBmpPicoAnim[dir][anim]) { spriteFinal = hBmpPicoAnim[dir][anim]; }
                break;
            case HERRAMIENTA_HACHA:
                if (hBmpHachaAnim[dir][anim]) { spriteFinal = hBmpHachaAnim[dir][anim]; }
                break;
            default:
                if (hBmpJugadorAnim[dir][anim]) { spriteFinal = hBmpJugadorAnim[dir][anim]; }
                break;
        }
    }
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
    
    // ✅ VERIFICACIÓN CON 2 PUNTOS (PIES y CABEZA)
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
    
    // ✅ VERIFICACIÓN CON 2 PUNTOS (PIES y CABEZA)
    int esTierraPiesY = EsSuelo(j->x + 16, futuroY + 16, mapa);
    int esTierraCabezaY = EsSuelo(j->x + 8, futuroY + 1, mapa);
    
    BOOL puedeY = (j->estadoBarco == 0 && esTierraPiesY && esTierraCabezaY) || 
                  (j->estadoBarco > 0 && !esTierraPiesY && !esTierraCabezaY);
    
    if (puedeY && j->estadoBarco == 1) {
        if (sqrt(pow(j->x - centroX, 2) + pow(futuroY - centroY, 2)) > radioSeguro) puedeY = FALSE;
    }
    
    if (puedeY) j->y = futuroY;
}

void actualizarCamara(Camera *cam, Jugador j) {
    int anchoPantalla = GetSystemMetrics(SM_CXSCREEN);
    int altoPantalla = GetSystemMetrics(SM_CYSCREEN);
    
    // Centrar
    cam->x = j.x - (anchoPantalla / 2 / cam->zoom);
    cam->y = j.y - (altoPantalla / 2 / cam->zoom);
    
    // Límites (0,0)
    if (cam->x < 0) cam->x = 0;
    if (cam->y < 0) cam->y = 0;
}

void intentarMontarBarco(Jugador *j, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    // Coordenadas EXACTAS del muelle principal
    #define MUELLE_X 2050
    #define MUELLE_Y 1600
    #define RADIO_PERMITIDO 100  // Radio de 100px alrededor del muelle
    
    // Calcular distancia al muelle
    float dx = j->x - MUELLE_X;
    float dy = j->y - MUELLE_Y;
    float distancia = sqrt(dx*dx + dy*dy);
    
    // ============================================
    // 1. CASO: ESTÁ EN BARCO → INTENTAR BAJAR
    // ============================================
    if (j->estadoBarco > 0) {
        // ¿Está lo suficientemente cerca del muelle?
        if (distancia < RADIO_PERMITIDO) {
            // ¡SÍ! Puede bajar en el muelle
            
            // 1. Cambiar estado
            j->estadoBarco = 0;  // Ahora está a pie
            
            // 2. Ajustar posición EXACTA en el muelle
            // (Opcional: puedes ajustar esto para que quede mejor visualmente)
            j->x = MUELLE_X + 30;  // 30px a la derecha del centro del muelle
            j->y = MUELLE_Y + 20;  // 20px abajo del centro del muelle
            
            // 3. Feedback visual y de sonido
            crearTextoFlotante(j->x, j->y, "Desembarcado en muelle", 0, RGB(0, 255, 200));
            
            // (Opcional) Sonido de éxito
            // PlaySound("SystemExclamation", NULL, SND_ASYNC);
            
        } else {
            // ¡NO! Está demasiado lejos del muelle
            crearTextoFlotante(j->x, j->y, "Ve al muelle para desembarcar", 0, RGB(255, 50, 50));
            
            // (Opcional) Sonido de error
            // PlaySound("SystemHand", NULL, SND_ASYNC);
        }
        return;
    }
    
    // ============================================
    // 2. CASO: ESTÁ A PIE → INTENTAR SUBIR
    // ============================================
    // (Mantener lógica original, pero solo en el muelle)
    if (distancia < RADIO_PERMITIDO) {
        if (j->tieneBotePesca) { 
            j->estadoBarco = 1;  // Sube a Bote de Pesca
            j->x += 60;          // Pequeño ajuste visual
            
            crearTextoFlotante(j->x, j->y, "Embarcando en bote de pesca", 0, RGB(0, 200, 255));
        }
        else if (j->cantBarcosGuerra > 0) { 
            j->estadoBarco = 2;  // Sube a Barco de Guerra
            j->x += 60;          // Pequeño ajuste visual
            
            crearTextoFlotante(j->x, j->y, "Embarcando en barco de guerra", 0, RGB(255, 200, 0));
        } else {
            // Tiene barco pero no está disponible (caso raro)
            crearTextoFlotante(j->x, j->y, "No tienes barcos disponibles", 0, RGB(255, 100, 100));
        }
    } 
}

