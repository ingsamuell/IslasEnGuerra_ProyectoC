#include "edificios.h"
#include "mapa.h"      
#include "naturaleza.h" 
#include "../recursos/recursos.h"
#include <stdio.h>
#include <math.h>

extern char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];

// Variables Globales de Edificios
Edificio misEdificios[MAX_EDIFICIOS_JUGADOR];
Edificio edificiosEnemigos[MAX_EDIFICIOS_ENEMIGOS];

// 1. INICIALIZACIÓN
void inicializarEdificios()
{
    // Limpiar jugador
    for (int i = 0; i < MAX_EDIFICIOS_JUGADOR; i++)
    {
        misEdificios[i].activo = 0;
        misEdificios[i].enConstruccion = 0;
        misEdificios[i].esEnemigo = 0;
        misEdificios[i].tipo = i + 1;
    }

    // Configurar enemigos 
    float coordsX[4] = {400, 2600, 400, 2600};
    float coordsY[4] = {400, 400, 2600, 2600};

    for (int i = 0; i < MAX_EDIFICIOS_ENEMIGOS; i++)
    {
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

// 2. DIBUJAR UN EDIFICIO INDIVIDUAL
void dibujarEdificio(HDC hdc, int x, int y, int tipoTamano, int esEnemigo, int mapaID, Camera *cam)
{
    // 1. Selección de Sprite
    if (mapaID < 0)
        mapaID = 0;
    if (mapaID > 2)
        mapaID = 2;

    HBITMAP bmpADibujar = NULL;
    if (esEnemigo)
    {
        if (tipoTamano == 1)
            bmpADibujar = hBmpEdificioPeqEnemigo[mapaID];
        else if (tipoTamano == 2)
            bmpADibujar = hBmpEdificioMedEnemigo[mapaID];
        else if (tipoTamano == 3)
            bmpADibujar = hBmpEdificioGrandeEnemigo[mapaID];
    }
    else
    {
        if (tipoTamano == 1)
            bmpADibujar = hBmpEdificioPeq[mapaID];
        else if (tipoTamano == 2)
            bmpADibujar = hBmpEdificioMed[mapaID];
        else if (tipoTamano == 3)
            bmpADibujar = hBmpEdificioGrande[mapaID];
    }

    if (bmpADibujar)
    {
        // 2. TAMAÑO CON ZOOM
        int anchoBase = 64;
        int altoBase = 64;
        if (tipoTamano == 2)
        {
            anchoBase = 96;
            altoBase = 96;
        }
        if (tipoTamano == 3)
        {
            anchoBase = 128;
            altoBase = 128;
        }

        int anchoVisual = anchoBase * cam->zoom;
        int altoVisual = altoBase * cam->zoom;

        // 3. POSICIÓN CON CÁMARA (Mundo -> Pantalla)
        // Restamos la cámara y multiplicamos por zoom
        int pantallaX = (x - cam->x) * cam->zoom;
        int pantallaY = (y - cam->y) * cam->zoom;

        // 4. Dibujar
        DibujarImagen(hdc, bmpADibujar, pantallaX, pantallaY, anchoVisual, altoVisual);
    }
}

// 3. MODO FANTASMA (LOGICA DE CONSTRUCCIÓN)
void dibujarFantasmaConstruccion(HDC hdc, Jugador *j, int mx, int my, int mapaID, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Camera *cam) {
    if (j->edificioSeleccionado == 0) return; 

    // 1. Definir tamaño
    int tamSprite = 64; 
    if (j->edificioSeleccionado == 2) tamSprite = 96;
    if (j->edificioSeleccionado == 3) tamSprite = 128;

    int spriteX = mx - (tamSprite / 2);
    int spriteY = my - (tamSprite / 2);

    // 2. Definir HUELLA (Validación)
    int margen = 10;
    int hitX = spriteX + margen;
    int hitY = spriteY + (tamSprite / 2); 
    int hitW = tamSprite - (margen * 2);
    int hitH = (tamSprite / 2) - margen;

    // Variables de Estado
    int esValido = 1;     
    int costoExtra = 0;   
    COLORREF colorEstado = RGB(0, 255, 0); // Verde

    // 3. Validar Terreno 
    if (!EsSuelo(hitX, hitY, mapa) || 
        !EsSuelo(hitX + hitW, hitY, mapa) || 
        !EsSuelo(hitX, hitY + hitH, mapa) || 
        !EsSuelo(hitX + hitW, hitY + hitH, mapa)) {
        esValido = 0;
        colorEstado = RGB(255, 0, 0); // Rojo
    }

    // 4. Chequeo de Obstáculos
    if (esValido) {
        for (int i = 0; i < MAX_ARBOLES; i++) {
            if (!arboles[i].activa) continue;
            if (hitX < arboles[i].x + 40 && hitX + hitW > arboles[i].x + 10 &&
                hitY < arboles[i].y + 60 && hitY + hitH > arboles[i].y + 20) {
                costoExtra += 20; 
                colorEstado = RGB(255, 255, 0); 
            }
        }
        for (int i = 0; i < MAX_MINAS; i++) {
            if (!minas[i].activa) continue;
            if (hitX < minas[i].x + 40 && hitX + hitW > minas[i].x &&
                hitY < minas[i].y + 40 && hitY + hitH > minas[i].y) {
                costoExtra += 40; 
                colorEstado = RGB(255, 255, 0); 
            }
        }
    }

    j->costoConstruccionActual = costoExtra;

    // 5. Dibujar Sprite y Marco
    dibujarEdificio(hdc, spriteX, spriteY, j->edificioSeleccionado, 0, mapaID, cam);

    int screenX = (hitX - cam->x) * cam->zoom;
    int screenY = (hitY - cam->y) * cam->zoom;
    int screenW = hitW * cam->zoom;
    int screenH = hitH * cam->zoom;

    HBRUSH brocha = CreateHatchBrush(HS_DIAGCROSS, colorEstado);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brocha);
    HPEN lapiz = CreatePen(PS_SOLID, 3, colorEstado);
    HPEN oldPen = (HPEN)SelectObject(hdc, lapiz);

    SetBkMode(hdc, TRANSPARENT);
    Rectangle(hdc, screenX, screenY, screenX + screenW, screenY + screenH);

    SelectObject(hdc, oldBrush); SelectObject(hdc, oldPen);
    DeleteObject(brocha); DeleteObject(lapiz);

    if (costoExtra > 0 && esValido) {
        char msg[32]; sprintf(msg, "Limpieza: +%d", costoExtra);
        SetTextColor(hdc, RGB(255, 255, 0));
        TextOut(hdc, screenX, screenY - 20, msg, strlen(msg));
    }
}

// 4. COLOCACIÓN DE EDIFICIOS
int COSTO_PEQ_ORO = 100;
int COSTO_MED_ORO = 300;
int COSTO_GDE_ORO = 600;

void intentarColocarEdificio(Jugador *j, int mx, int my, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    if (j->edificioSeleccionado == 0) return;

    // Definir tamaño y huella 
    int tamSprite = 64; 
    if (j->edificioSeleccionado == 2) tamSprite = 96;
    if (j->edificioSeleccionado == 3) tamSprite = 128;

    int spriteX = mx - (tamSprite / 2);
    int spriteY = my - (tamSprite / 2);
    int margen = 10;
    int hitX = spriteX + margen;
    int hitY = spriteY + (tamSprite / 2); 
    int hitW = tamSprite - (margen * 2);
    int hitH = (tamSprite / 2) - margen;

    // Validar Terreno
    if (!EsSuelo(hitX, hitY, mapa) || 
        !EsSuelo(hitX + hitW, hitY, mapa) || 
        !EsSuelo(hitX, hitY + hitH, mapa) || 
        !EsSuelo(hitX + hitW, hitY + hitH, mapa)) {
        PlaySound("SystemHand", NULL, SND_ASYNC); 
        return;
    }

    // Calcular costos
    int costoLimpieza = 0;
    int arbolesABorrar[10]; int numArboles = 0;
    int minasABorrar[10]; int numMinas = 0;

    for (int i = 0; i < MAX_ARBOLES; i++) {
        if (!arboles[i].activa) continue;
        if (hitX < arboles[i].x + 40 && hitX + hitW > arboles[i].x + 10 &&
            hitY < arboles[i].y + 60 && hitY + hitH > arboles[i].y + 20) {
            costoLimpieza += 20;
            if (numArboles < 10) arbolesABorrar[numArboles++] = i;
        }
    }
    for (int i = 0; i < MAX_MINAS; i++) {
        if (!minas[i].activa) continue;
        if (hitX < minas[i].x + 40 && hitX + hitW > minas[i].x &&
            hitY < minas[i].y + 40 && hitY + hitH > minas[i].y) {
            costoLimpieza += 40;
            if (numMinas < 10) minasABorrar[numMinas++] = i;
        }
    }

    int costoBase = 0;
    if (j->edificioSeleccionado == 1) costoBase = 100;
    else if (j->edificioSeleccionado == 2) costoBase = 300;
    else if (j->edificioSeleccionado == 3) costoBase = 600;

    if (j->oro < costoBase + costoLimpieza) {
        PlaySound("SystemHand", NULL, SND_ASYNC);
        return; 
    }

    // Buscar Slot y Guardar
    int slotDeseado = j->edificioSeleccionado - 1; 
    if (misEdificios[slotDeseado].activo) return;

    j->oro -= (costoBase + costoLimpieza);
    for(int k=0; k<numArboles; k++) arboles[arbolesABorrar[k]].activa = 0;
    for(int k=0; k<numMinas; k++) minas[minasABorrar[k]].activa = 0;

    misEdificios[slotDeseado].activo = 1;
    misEdificios[slotDeseado].tipo = j->edificioSeleccionado;
    misEdificios[slotDeseado].x = (float)spriteX;
    misEdificios[slotDeseado].y = (float)spriteY;
    misEdificios[slotDeseado].esEnemigo = 0;
    misEdificios[slotDeseado].enConstruccion = 1;
    misEdificios[slotDeseado].tiempoProgreso = 0;
    
    if (j->edificioSeleccionado == 1) misEdificios[slotDeseado].tiempoTotal = 5.0f;
    else if (j->edificioSeleccionado == 2) misEdificios[slotDeseado].tiempoTotal = 10.0f;
    else misEdificios[slotDeseado].tiempoTotal = 20.0f;

    misEdificios[slotDeseado].vidaMax = 500 * j->edificioSeleccionado;
    misEdificios[slotDeseado].vidaActual = 1;

    j->edificioSeleccionado = 0;
    PlaySound("SystemAsterisk", NULL, SND_ASYNC);
}

void actualizarEdificios(float deltaTiempo)
{
    for (int i = 0; i < MAX_EDIFICIOS_JUGADOR; i++)
    {
        if (!misEdificios[i].activo)
            continue;

        if (misEdificios[i].enConstruccion)
        {
            misEdificios[i].tiempoProgreso += deltaTiempo;

            // Vida proporcional
            float porcentaje = misEdificios[i].tiempoProgreso / misEdificios[i].tiempoTotal;
            misEdificios[i].vidaActual = (int)(misEdificios[i].vidaMax * porcentaje);

            if (misEdificios[i].tiempoProgreso >= misEdificios[i].tiempoTotal)
            {
                misEdificios[i].enConstruccion = 0;
                misEdificios[i].vidaActual = misEdificios[i].vidaMax;
            }
        }
    }
}

void dibujarTodosLosEdificios(HDC hdc, int mapaID, Camera *cam)
{
    // 1. DIBUJAR ENEMIGOS
    for (int i = 0; i < MAX_EDIFICIOS_ENEMIGOS; i++)
    {
        if (edificiosEnemigos[i].activo)
        {
            dibujarEdificio(hdc, (int)edificiosEnemigos[i].x, (int)edificiosEnemigos[i].y,
                            edificiosEnemigos[i].tipo, 1, mapaID, cam);
        }
    }

    // 2. DIBUJAR JUGADOR
    for (int i = 0; i < MAX_EDIFICIOS_JUGADOR; i++)
    {
        if (misEdificios[i].activo)
        {

            // A) Dibujar el Edificio (Pasando la cámara)
            dibujarEdificio(hdc, (int)misEdificios[i].x, (int)misEdificios[i].y,
                            misEdificios[i].tipo, 0, mapaID, cam);

            // B) Dibujar Barra de Construcción (Ajustada a la cámara)
            if (misEdificios[i].enConstruccion)
            {
                // Calculamos posición en PANTALLA
                int bx = ((int)misEdificios[i].x - cam->x) * cam->zoom;
                int by = ((int)misEdificios[i].y - cam->y) * cam->zoom - 10;

                // Calculamos ancho visual según zoom
                int anchoBase = 64;
                if (misEdificios[i].tipo == 2)
                    anchoBase = 96;
                if (misEdificios[i].tipo == 3)
                    anchoBase = 128;
                int anchoVisual = anchoBase * cam->zoom;

                float pct = misEdificios[i].tiempoProgreso / misEdificios[i].tiempoTotal;
                if (pct > 1.0f)
                    pct = 1.0f;

                // Fondo Negro
                RECT rBg = {bx, by, bx + anchoVisual, by + (6 * cam->zoom)};
                HBRUSH brNegro = (HBRUSH)GetStockObject(BLACK_BRUSH);
                FillRect(hdc, &rBg, brNegro);

                // Relleno Amarillo
                if (pct > 0)
                {
                    RECT rFill = {bx + 1, by + 1, bx + (int)(anchoVisual * pct), by + (6 * cam->zoom) - 1};
                    HBRUSH brAm = CreateSolidBrush(RGB(255, 215, 0));
                    FillRect(hdc, &rFill, brAm);
                    DeleteObject(brAm);
                }
            }
        }
    }
}