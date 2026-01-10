#include "edificios.h"
#include "mapa.h"        // Necesario para EsSuelo y la variable 'mapa'
#include "naturaleza.h"  // Necesario para arboles y minas
#include "../recursos/recursos.h"
#include <stdio.h>
#include <math.h>

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

    // Configurar enemigos
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
void dibujarEdificio(HDC hdc, int x, int y, int tipoTamano, int esEnemigo, int mapaID) {
    if (mapaID < 0) mapaID = 0;
    if (mapaID > 2) mapaID = 2;

    HBITMAP bmpADibujar = NULL;

    if (esEnemigo) {
        if (tipoTamano == 1) bmpADibujar = hBmpEdificioPeqEnemigo[mapaID];
        else if (tipoTamano == 2) bmpADibujar = hBmpEdificioMedEnemigo[mapaID];
        else if (tipoTamano == 3) bmpADibujar = hBmpEdificioGrandeEnemigo[mapaID];
    } else {
        if (tipoTamano == 1) bmpADibujar = hBmpEdificioPeq[mapaID];
        else if (tipoTamano == 2) bmpADibujar = hBmpEdificioMed[mapaID];
        else if (tipoTamano == 3) bmpADibujar = hBmpEdificioGrande[mapaID];
    }

    if (bmpADibujar) {
        int anchoVisual = 64;
        int altoVisual = 64;
        if (tipoTamano == 2) { anchoVisual = 96; altoVisual = 96; }
        if (tipoTamano == 3) { anchoVisual = 128; altoVisual = 128; }
        
        DibujarImagen(hdc, bmpADibujar, x, y, anchoVisual, altoVisual);
    }
}

// -------------------------------------------------------------
// 3. MODO FANTASMA (LOGICA DE CONSTRUCCIÓN)
// -------------------------------------------------------------
void dibujarFantasmaConstruccion(HDC hdc, Jugador *j, int mx, int my, int mapaID) {
    if (j->edificioSeleccionado == 0) return; 

    int tam = 64; 
    if (j->edificioSeleccionado == 2) tam = 96;
    if (j->edificioSeleccionado == 3) tam = 128;

    int x = mx - (tam / 2);
    int y = my - (tam / 2);

    int esValido = 1;     
    int costoExtra = 0;   
    COLORREF colorEstado = RGB(0, 255, 0); 

    // IMPORTANTE: Aquí usamos 'mapa' que viene de mapa.h
    if (!EsSuelo(x, y, mapa) || 
        !EsSuelo(x + tam, y, mapa) || 
        !EsSuelo(x, y + tam, mapa) || 
        !EsSuelo(x + tam, y + tam, mapa)) {
        
        esValido = 0;
        colorEstado = RGB(255, 0, 0); 
    }

    if (esValido) {
        for (int i = 0; i < MAX_ARBOLES; i++) {
            if (!misArboles[i].activa) continue;
            if (x < misArboles[i].x + 32 && x + tam > misArboles[i].x &&
                y < misArboles[i].y + 64 && y + tam > misArboles[i].y) {
                costoExtra += 20; 
                colorEstado = RGB(255, 255, 0); 
            }
        }
        for (int i = 0; i < MAX_MINAS; i++) {
            if (!misMinas[i].activa) continue;
            if (x < misMinas[i].x + 32 && x + tam > misMinas[i].x &&
                y < misMinas[i].y + 32 && y + tam > misMinas[i].y) {
                costoExtra += 40; 
                colorEstado = RGB(255, 255, 0); 
            }
        }
    }

    j->costoConstruccionActual = costoExtra;

    // Dibujado
    dibujarEdificio(hdc, x, y, j->edificioSeleccionado, 0, mapaID);

    // Marco
    HBRUSH brocha = CreateHatchBrush(HS_DIAGCROSS, colorEstado);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brocha);
    HPEN lapiz = CreatePen(PS_SOLID, 3, colorEstado);
    HPEN oldPen = (HPEN)SelectObject(hdc, lapiz);

    SetBkMode(hdc, TRANSPARENT);
    Rectangle(hdc, x, y, x + tam, y + tam);

    SelectObject(hdc, oldBrush); SelectObject(hdc, oldPen);
    DeleteObject(brocha); DeleteObject(lapiz);

    if (costoExtra > 0 && esValido) {
        char msg[32];
        sprintf(msg, "Limpieza: +%d Oro", costoExtra);
        SetTextColor(hdc, RGB(255, 255, 0));
        TextOut(hdc, mx + 20, my - 20, msg, strlen(msg));
    }
    
    if (!esValido) {
        SetTextColor(hdc, RGB(255, 50, 50));
        TextOut(hdc, mx + 20, my - 20, "Terreno Invalido", 16);
        j->costoConstruccionActual = -1; 
    }
}

// Precios base de los edificios (Madera, Piedra, Oro)

int COSTO_PEQ_ORO = 100;
int COSTO_MED_ORO = 300;
int COSTO_GDE_ORO = 600;

void intentarColocarEdificio(Jugador *j, int mx, int my, int mapaID) {
    if (j->edificioSeleccionado == 0) return;

    // 1. Recalcular Datos (Dimensiones y Costo Extra)
    // Repetimos la lógica del fantasma para seguridad
    int tam = 64; 
    if (j->edificioSeleccionado == 2) tam = 96;
    if (j->edificioSeleccionado == 3) tam = 128;

    int x = mx - (tam / 2);
    int y = my - (tam / 2);
    int costoLimpieza = 0;
    int esValido = 1;

    // Chequeo Agua
    if (!EsSuelo(x, y, mapa) || !EsSuelo(x + tam, y, mapa) || 
        !EsSuelo(x, y + tam, mapa) || !EsSuelo(x + tam, y + tam, mapa)) {
        esValido = 0;
    }

    if (!esValido) {
        PlaySound("SystemHand", NULL, SND_ASYNC); // Sonido de Error
        return;
    }

    // 2. Calcular Costo de Limpieza y Lista de Objetos a Borrar
    // Usamos un array temporal para recordar qué índices borrar
    int arbolesABorrar[10]; int numArboles = 0;
    int minasABorrar[10]; int numMinas = 0;

    // Detectar Árboles
    for (int i = 0; i < MAX_ARBOLES; i++) {
        if (!misArboles[i].activa) continue;
        if (x < misArboles[i].x + 32 && x + tam > misArboles[i].x &&
            y < misArboles[i].y + 64 && y + tam > misArboles[i].y) {
            costoLimpieza += 20;
            if (numArboles < 10) arbolesABorrar[numArboles++] = i;
        }
    }
    // Detectar Minas
    for (int i = 0; i < MAX_MINAS; i++) {
        if (!misMinas[i].activa) continue;
        if (x < misMinas[i].x + 32 && x + tam > misMinas[i].x &&
            y < misMinas[i].y + 32 && y + tam > misMinas[i].y) {
            costoLimpieza += 40;
            if (numMinas < 10) minasABorrar[numMinas++] = i;
        }
    }

    // 3. Verificar Dinero Total (Costo Edificio + Limpieza)
    int costoBase = 0;
    if (j->edificioSeleccionado == 1) costoBase = COSTO_PEQ_ORO;
    else if (j->edificioSeleccionado == 2) costoBase = COSTO_MED_ORO;
    else if (j->edificioSeleccionado == 3) costoBase = COSTO_GDE_ORO;

    int costoTotal = costoBase + costoLimpieza;

    if (j->oro < costoTotal) {
        // No tienes dinero
        PlaySound("SystemHand", NULL, SND_ASYNC);
        return; 
    }

    // 4. Buscar espacio libre en el Array de Edificios
    int slotEncontrado = -1;
    
    // REGLA: Solo 1 de cada tipo (según tu petición)
    // El slot 0 es para Peq, Slot 1 para Med, Slot 2 para Grande.
    int slotDeseado = j->edificioSeleccionado - 1; // Tipo 1 va al slot 0...
    
    if (misEdificios[slotDeseado].activo) {
        // Ya tienes un edificio de este tipo
        MessageBox(NULL, "Ya tienes un edificio de este tipo.", "Limite Alcanzado", MB_OK);
        return;
    }
    slotEncontrado = slotDeseado;

    // 5. ¡CONSTRUIR! (Aplicar cambios)
    
    // A) Cobrar
    j->oro -= costoTotal;

    // B) Borrar Naturaleza
    for(int k=0; k<numArboles; k++) misArboles[arbolesABorrar[k]].activa = 0;
    for(int k=0; k<numMinas; k++) misMinas[minasABorrar[k]].activa = 0;

    // C) Guardar Edificio
    misEdificios[slotEncontrado].activo = 1;
    misEdificios[slotEncontrado].tipo = j->edificioSeleccionado;
    misEdificios[slotEncontrado].x = (float)x;
    misEdificios[slotEncontrado].y = (float)y;
    misEdificios[slotEncontrado].esEnemigo = 0;
    
    // Configurar Construcción
    misEdificios[slotEncontrado].enConstruccion = 1;
    misEdificios[slotEncontrado].tiempoProgreso = 0;
    
    // Tiempo según tamaño: Peq=5s, Med=10s, Gde=20s
    if (j->edificioSeleccionado == 1) misEdificios[slotEncontrado].tiempoTotal = 5.0f;
    else if (j->edificioSeleccionado == 2) misEdificios[slotEncontrado].tiempoTotal = 10.0f;
    else misEdificios[slotEncontrado].tiempoTotal = 20.0f;

    misEdificios[slotEncontrado].vidaMax = 500 * j->edificioSeleccionado;
    misEdificios[slotEncontrado].vidaActual = 1; // Empieza con 1 de vida

    // 6. Resetear modo construcción
    j->edificioSeleccionado = 0;
    PlaySound("SystemAsterisk", NULL, SND_ASYNC); // Sonido de éxito
}

void actualizarEdificios(float deltaTiempo) {
    for (int i = 0; i < MAX_EDIFICIOS_JUGADOR; i++) {
        if (!misEdificios[i].activo) continue;

        // Si está en construcción, avanzar el reloj
        if (misEdificios[i].enConstruccion) {
            misEdificios[i].tiempoProgreso += deltaTiempo;

            // Calcular vida proporcional (Efecto visual: la vida sube mientras se construye)
            float porcentaje = misEdificios[i].tiempoProgreso / misEdificios[i].tiempoTotal;
            misEdificios[i].vidaActual = (int)(misEdificios[i].vidaMax * porcentaje);

            // ¿Terminó?
            if (misEdificios[i].tiempoProgreso >= misEdificios[i].tiempoTotal) {
                misEdificios[i].enConstruccion = 0;
                misEdificios[i].vidaActual = misEdificios[i].vidaMax;
                // Aquí podrías reproducir un sonido de "Trabajo terminado"
            }
        }
    }
}

void dibujarTodosLosEdificios(HDC hdc, int mapaID) {
    // 1. Dibujar Enemigos
    for (int i = 0; i < MAX_EDIFICIOS_ENEMIGOS; i++) {
        if (edificiosEnemigos[i].activo) {
            dibujarEdificio(hdc, (int)edificiosEnemigos[i].x, (int)edificiosEnemigos[i].y, 
                            edificiosEnemigos[i].tipo, 1, mapaID);
        }
    }

    // 2. Dibujar Jugador
    for (int i = 0; i < MAX_EDIFICIOS_JUGADOR; i++) {
        if (misEdificios[i].activo) {
            // Dibujar Sprite
            dibujarEdificio(hdc, (int)misEdificios[i].x, (int)misEdificios[i].y, 
                            misEdificios[i].tipo, 0, mapaID);

            // Dibujar Barra de Construcción
            if (misEdificios[i].enConstruccion) {
                int bx = (int)misEdificios[i].x;
                int by = (int)misEdificios[i].y - 10;
                int ancho = 64; // Ancho base
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