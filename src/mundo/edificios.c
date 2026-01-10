#include "edificios.h"
#include "mapa.h"        // Necesario para EsSuelo
#include "naturaleza.h"  // Necesario para arboles y minas
#include "../recursos/recursos.h"
#include <stdio.h>
#include <math.h>

// Referencia a la matriz global del mapa (definida en main.c o mapa.c)
extern char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];

// Variables Globales de Edificios
Edificio misEdificios[MAX_EDIFICIOS_JUGADOR];
Edificio edificiosEnemigos[MAX_EDIFICIOS_ENEMIGOS];

// -------------------------------------------------------------
// 1. INICIALIZACIÓN
// -------------------------------------------------------------
void inicializarEdificios() {
    // Limpiar jugador
    for (int i = 0; i < MAX_EDIFICIOS_JUGADOR; i++) {
        misEdificios[i].activo = 0;
        misEdificios[i].enConstruccion = 0;
        misEdificios[i].esEnemigo = 0;
        misEdificios[i].tipo = i + 1; 
    }

    // Configurar enemigos (Ejemplo básico)
    float coordsX[4] = {400, 2600, 400, 2600}; 
    float coordsY[4] = {400, 400, 2600, 2600};

    for (int i = 0; i < MAX_EDIFICIOS_ENEMIGOS; i++) {
        edificiosEnemigos[i].activo = 1;
        edificiosEnemigos[i].enConstruccion = 0;
        edificiosEnemigos[i].esEnemigo = 1;
        edificiosEnemigos[i].tipo = 3; 
        edificiosEnemigos[i].x = coordsX[i];
        edificiosEnemigos[i].y = coordsY[i];
        edificiosEnemigos[i].vidaMax = 2000;
        edificiosEnemigos[i].vidaActual = 2000;
    }
}

// -------------------------------------------------------------
// 2. DIBUJAR UN EDIFICIO INDIVIDUAL
// -------------------------------------------------------------
void dibujarEdificio(HDC hdc, int x, int y, int tipoTamano, int esEnemigo, int mapaID, Camera *cam) {
    if (mapaID < 0) mapaID = 0;
    if (mapaID > 2) mapaID = 2;

    HBITMAP bmpADibujar = NULL;

    // NOTA: Asegúrate de que estos arrays existan en recursos.h/recursos.c
    // Si no, usa hBmpCastilloJugador temporalmente para probar.
    if (esEnemigo) {
        // bmpADibujar = hBmpEdificioEnemigo[tipoTamano]; // Ejemplo
        bmpADibujar = hBmpCastilloJugador; // Placeholder por si acaso
    } else {
        // Lógica para seleccionar sprite según tamaño
        if (tipoTamano == 1) bmpADibujar = hBmpCastilloJugador; // Cambiar por hBmpEdificioPeq
        else if (tipoTamano == 2) bmpADibujar = hBmpCastilloJugador;
        else if (tipoTamano == 3) bmpADibujar = hBmpCastilloJugador;
    }

    if (bmpADibujar) {
        // 1. Definir tamaño base según tipo
        int anchoBase = 64; int altoBase = 64;
        if (tipoTamano == 2) { anchoBase = 96; altoBase = 96; }
        if (tipoTamano == 3) { anchoBase = 128; altoBase = 128; }

        // Multiplicamos por el zoom de la cámara
        int anchoVisual = (int)(anchoBase * cam->zoom);
        int altoVisual = (int)(altoBase * cam->zoom);

        // 2. Aplicar Zoom y Desplazamiento a la Posición
        int drawX = (int)((x - cam->x) * cam->zoom);
        int drawY = (int)((y - cam->y) * cam->zoom);
        
        DibujarImagen(hdc, bmpADibujar, drawX, drawY, anchoVisual, altoVisual);
    }
}

// -------------------------------------------------------------
// 3. MODO FANTASMA (LOGICA DE CONSTRUCCIÓN)
// -------------------------------------------------------------
void dibujarFantasmaConstruccion(HDC hdc, Jugador *j, int mx, int my, int mapaID, Camera *cam) {
    int tamBase = 64; 
    if (j->edificioSeleccionado == 2) tamBase = 96;
    if (j->edificioSeleccionado == 3) tamBase = 128;

    int esValido = 1;
    COLORREF colorEstado = RGB(0, 255, 0); // Verde

    // Verificar las 4 esquinas del edificio en el mapaMundo
    if (!EsSuelo(mx, my, mapaMundo) || 
        !EsSuelo(mx + tamBase, my, mapaMundo) || 
        !EsSuelo(mx, my + tamBase, mapaMundo) || 
        !EsSuelo(mx + tamBase, my + tamBase, mapaMundo)) {
        esValido = 0;
        colorEstado = RGB(255, 0, 0); // Rojo
    }

    // Dibujamos el edificio (Visualización previa)
    dibujarEdificio(hdc, mx, my, j->edificioSeleccionado, 0, mapaID, cam);

    // Dibujar el cuadro de estado (Borde de color)
    int drawX = (int)((mx - cam->x) * cam->zoom);
    int drawY = (int)((my - cam->y) * cam->zoom);
    int drawTam = (int)(tamBase * cam->zoom);

    HPEN hPen = CreatePen(PS_SOLID, 2, colorEstado);
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc, drawX, drawY, drawX + drawTam, drawY + drawTam);
    
    if (!esValido) {
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 50, 50));
        TextOut(hdc, drawX, drawY - 20, "Terreno Invalido", 16);
    }

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

// -------------------------------------------------------------
// 4. COLOCACIÓN DE EDIFICIOS
// -------------------------------------------------------------
int COSTO_PEQ_ORO = 100;
int COSTO_MED_ORO = 300;
int COSTO_GDE_ORO = 600;

void intentarColocarEdificio(Jugador *j, int mx, int my, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    if (j->edificioSeleccionado == 0) return;

    // 1. Recalcular Datos
    int tam = 64; 
    if (j->edificioSeleccionado == 2) tam = 96;
    if (j->edificioSeleccionado == 3) tam = 128;

    // Centrar en el mouse (opcional, ajusta según prefieras esquina o centro)
    int x = mx; // Usamos la esquina superior izquierda tal cual viene del mouse
    int y = my;
    
    int costoLimpieza = 0;
    int esValido = 1;

    // --- CORRECCIÓN: Usar mapaMundo en lugar de 'mapa' ---
    if (!EsSuelo(x, y, mapaMundo) || !EsSuelo(x + tam, y, mapaMundo) || 
        !EsSuelo(x, y + tam, mapaMundo) || !EsSuelo(x + tam, y + tam, mapaMundo)) {
        esValido = 0;
    }

    if (!esValido) {
        PlaySound("SystemHand", NULL, SND_ASYNC); // Error
        return;
    }

    // 2. Calcular Costo de Limpieza y Objetos a Borrar
    int arbolesABorrar[20]; int numArboles = 0;
    int minasABorrar[20];   int numMinas = 0;

    // Detectar Árboles
    for (int i = 0; i < MAX_ARBOLES; i++) {
        if (!misArboles[i].activa) continue;
        // Colisión simple AABB
        if (x < misArboles[i].x + 32 && x + tam > misArboles[i].x &&
            y < misArboles[i].y + 32 && y + tam > misArboles[i].y) {
            costoLimpieza += 20;
            if (numArboles < 20) arbolesABorrar[numArboles++] = i;
        }
    }
    // Detectar Minas
    for (int i = 0; i < MAX_MINAS; i++) {
        if (!misMinas[i].activa) continue;
        if (x < misMinas[i].x + 32 && x + tam > misMinas[i].x &&
            y < misMinas[i].y + 32 && y + tam > misMinas[i].y) {
            costoLimpieza += 40;
            if (numMinas < 20) minasABorrar[numMinas++] = i;
        }
    }

    // 3. Verificar Dinero
    int costoBase = 0;
    if (j->edificioSeleccionado == 1) costoBase = COSTO_PEQ_ORO;
    else if (j->edificioSeleccionado == 2) costoBase = COSTO_MED_ORO;
    else if (j->edificioSeleccionado == 3) costoBase = COSTO_GDE_ORO;

    int costoTotal = costoBase + costoLimpieza;

    if (j->oro < costoTotal) {
        PlaySound("SystemHand", NULL, SND_ASYNC);
        return; 
    }

    // 4. Buscar slot (Regla: 1 de cada tipo)
    int slotDeseado = j->edificioSeleccionado - 1; 
    
    if (misEdificios[slotDeseado].activo) {
        MessageBox(NULL, "Ya tienes un edificio de este tipo.", "Limite Alcanzado", MB_OK);
        return;
    }

    // 5. ¡CONSTRUIR!
    j->oro -= costoTotal;

    // Borrar Naturaleza
    for(int k=0; k<numArboles; k++) misArboles[arbolesABorrar[k]].activa = 0;
    for(int k=0; k<numMinas; k++) misMinas[minasABorrar[k]].activa = 0;

    // Guardar Edificio
    misEdificios[slotDeseado].activo = 1;
    misEdificios[slotDeseado].tipo = j->edificioSeleccionado;
    misEdificios[slotDeseado].x = (float)x;
    misEdificios[slotDeseado].y = (float)y;
    misEdificios[slotDeseado].esEnemigo = 0;
    
    // Configurar Construcción
    misEdificios[slotDeseado].enConstruccion = 1;
    misEdificios[slotDeseado].tiempoProgreso = 0;
    
    if (j->edificioSeleccionado == 1) misEdificios[slotDeseado].tiempoTotal = 5.0f;
    else if (j->edificioSeleccionado == 2) misEdificios[slotDeseado].tiempoTotal = 10.0f;
    else misEdificios[slotDeseado].tiempoTotal = 20.0f;

    misEdificios[slotDeseado].vidaMax = 500 * j->edificioSeleccionado;
    misEdificios[slotDeseado].vidaActual = 1;

    // 6. Resetear modo
    j->edificioSeleccionado = 0;
    PlaySound("SystemAsterisk", NULL, SND_ASYNC);
}

void actualizarEdificios(float deltaTiempo) {
    for (int i = 0; i < MAX_EDIFICIOS_JUGADOR; i++) {
        if (!misEdificios[i].activo) continue;

        if (misEdificios[i].enConstruccion) {
            misEdificios[i].tiempoProgreso += deltaTiempo;

            // Vida proporcional
            float porcentaje = misEdificios[i].tiempoProgreso / misEdificios[i].tiempoTotal;
            misEdificios[i].vidaActual = (int)(misEdificios[i].vidaMax * porcentaje);

            if (misEdificios[i].tiempoProgreso >= misEdificios[i].tiempoTotal) {
                misEdificios[i].enConstruccion = 0;
                misEdificios[i].vidaActual = misEdificios[i].vidaMax;
            }
        }
    }
}

// --- CORRECCIÓN: Agregar Camera *cam como parámetro ---
void dibujarTodosLosEdificios(HDC hdc, int mapaID, Camera *cam) {
    // 1. Dibujar Enemigos
    for (int i = 0; i < MAX_EDIFICIOS_ENEMIGOS; i++) {
        if (edificiosEnemigos[i].activo) {
            dibujarEdificio(hdc, (int)edificiosEnemigos[i].x, (int)edificiosEnemigos[i].y, edificiosEnemigos[i].tipo, 1, mapaID, cam);
        }
    }

    // 2. Dibujar Jugador
    for (int i = 0; i < MAX_EDIFICIOS_JUGADOR; i++) {
        if (misEdificios[i].activo) {
            // Dibujar Sprite
            dibujarEdificio(hdc, (int)misEdificios[i].x, (int)misEdificios[i].y, 
                misEdificios[i].tipo, 0, mapaID, cam);

            // Dibujar Barra de Construcción
            if (misEdificios[i].enConstruccion) {
                int bx = (int)((misEdificios[i].x - cam->x) * cam->zoom);
                int by = (int)((misEdificios[i].y - cam->y) * cam->zoom) - 10;
                
                int anchoBase = 64;
                if(misEdificios[i].tipo == 2) anchoBase = 96;
                if(misEdificios[i].tipo == 3) anchoBase = 128;
                
                int ancho = (int)(anchoBase * cam->zoom);
                float pct = misEdificios[i].tiempoProgreso / misEdificios[i].tiempoTotal;
                
                // Fondo Negro
                RECT rBg = {bx, by, bx + ancho, by + 5};
                FillRect(hdc, &rBg, (HBRUSH)GetStockObject(BLACK_BRUSH));
                
                // Relleno Amarillo
                RECT rFill = {bx, by, bx + (int)(ancho * pct), by + 5};
                HBRUSH brAm = CreateSolidBrush(RGB(255, 255, 0));
                FillRect(hdc, &rFill, brAm);
                DeleteObject(brAm);
            }
        }
    }
}