// Samuel Revollo

#include "tienda.h"
#include "../recursos/recursos.h"
#include "../unidades/unidades.h" 
#include "../jugador/jugador.h"
#include "../mundo/mapa.h"
#include "../mundo/edificios.h" 
#include <stdio.h>


// --- LÓGICA DE DIBUJO DEL EDIFICIO (MAPA) ---
void dibujarTiendasEnIslas(HDC hdc, Camera cam, int ancho, int alto, int frameTienda) {
    // Coordenadas fijas de la tienda principal 
    int mundoX = 1500; 
    int mundoY = 1900;
    int tx = (mundoX - cam.x) * cam.zoom;
    int ty = (mundoY - cam.y) * cam.zoom;
    int tam = 50 * cam.zoom;

    // Animación simple (2 frames)
    int f = (frameTienda / 20) % 2; 
    
    if (hBmpTienda[f]) {
        DibujarImagen(hdc, hBmpTienda[f], tx, ty, tam, tam);
    }
}

// --- LÓGICA DE INTERACCIÓN (CLICS) ---
void procesarClickMochilaTienda(int mx, int my, int esClickDerecho, Jugador *j, HWND hwnd)
{
    if (!j->tiendaAbierta)
        return;

    // 1. OBTENER POSICIÓN DINÁMICA
    RECT rect;
    GetClientRect(hwnd, &rect);
    int anchoVentana = rect.right;
    int anchoW = 340;
    int tx = anchoVentana - anchoW - 20;
    int ty = 80;

    // --- CLIC EN PESTAÑAS ---
if (my >= ty && my <= ty + 40)
    {
        int tabAncho = 68; // 340 / 5
        if (mx >= tx && mx < tx + tabAncho) j->modoTienda = 0;
        else if (mx >= tx + tabAncho && mx < tx + (tabAncho*2)) j->modoTienda = 1;
        else if (mx >= tx + (tabAncho*2) && mx < tx + (tabAncho*3)) j->modoTienda = 2;
        else if (mx >= tx + (tabAncho*3) && mx < tx + (tabAncho*4)) j->modoTienda = 3;
        else if (mx >= tx + (tabAncho*4) && mx < tx + anchoW) j->modoTienda = 4;

        InvalidateRect(hwnd, NULL, FALSE);
        return;
    }

    int startY = ty + 50;
    char msg[32];
    // Mensaje sobre la cabeza del jugador
    float msgX = j->x;
    float msgY = j->y - 40;

    // TAB 0: HERRAMIENTAS
    if (j->modoTienda == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            int iy = startY + (i * 80);
            if (my >= iy && my <= iy + 60 && mx >= tx && mx <= tx + 300)
            {
                // --- PRIMERO LA MOCHILA ---
                // Bloqueo total si no tienes Mochila Nivel 2
                if (j->nivelMochila < 2) 
                {
                    crearTextoFlotante(msgX, msgY, "Req. Mochila Nvl 2", 0, RGB(255, 50, 50)); 
                    return; // Detiene la función aquí, no compra nada.
                }

                // --- BLOQUEOS POR NIVEL DE JUGADOR (PLAN DE ERAS) ---
                int nivelReq = 1;
                if (i == 0) nivelReq = 3; // Espada -> Nivel 3
                if (i == 1) nivelReq = 2; // Pico -> Nivel 2
                if (i == 2) nivelReq = 2; // Hacha -> Nivel 2
                if (i == 3) nivelReq = 4; // Caña -> Nivel 4

                if (j->nivel < nivelReq)
                {
                    sprintf(msg, "Req. Nivel %d", nivelReq);
                    crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                    return; 
                }
                
                // --- PROCESO DE COMPRA ---
                
                // Item 0: Espada
                if (i == 0)
                {
                    if (j->tieneEspada) return; 
                    
                    if (j->oro < 50) {
                        sprintf(msg, "Faltan %d Oro", 50 - j->oro);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                    } else {
                        j->oro -= 50;
                        j->tieneEspada = TRUE;
                        ganarExperiencia(j, 10);
                        crearTextoFlotante(msgX, msgY, "Espada Equipada!", 0, RGB(0, 255, 0));
                        RegistrarLog("COMPRA: Espada adquirida.");
                    }
                }
                // Item 1: Pico
                else if (i == 1)
                {
                    if (j->tienePico) return;

                    if (j->oro < 30) {
                        sprintf(msg, "Faltan %d Oro", 30 - j->oro);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                    } else {
                        j->oro -= 30;
                        j->tienePico = TRUE;
                        ganarExperiencia(j, 10);
                        crearTextoFlotante(msgX, msgY, "Pico Equipado!", 0, RGB(0, 255, 0));
                        RegistrarLog("COMPRA: Pico adquirido.");
                    }
                }
                // Item 2: Hacha
                else if (i == 2)
                {
                    if (j->tieneHacha) return;

                    if (j->oro < 20) {
                        sprintf(msg, "Faltan %d Oro", 20 - j->oro);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                    } else {
                        j->oro -= 20;
                        j->tieneHacha = TRUE;
                        ganarExperiencia(j, 10);
                        crearTextoFlotante(msgX, msgY, "Hacha Equipada!", 0, RGB(0, 255, 0));
                        RegistrarLog("COMPRA: Hacha adquirida.");
                    }
                }
                // Item 3: Caña
                else if (i == 3)
                {
                    if (j->tieneCana) return;

                    BOOL falta = FALSE;
                    if (j->madera < 10) {
                        sprintf(msg, "Faltan %d Mad", 10 - j->madera);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));
                        falta = TRUE;
                    }
                    if (j->oro < 30) {
                        sprintf(msg, "Faltan %d Oro", 30 - j->oro);
                        crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50));
                        falta = TRUE;
                    }

                    if (!falta) {
                        j->oro -= 30;
                        j->madera -= 10;
                        j->tieneCana = TRUE;
                        ganarExperiencia(j, 20);
                        crearTextoFlotante(msgX, msgY, "Cana Equipada!", 0, RGB(0, 255, 0));
                        RegistrarLog("COMPRA: Cana adquirida."); 
                    }
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
    }

    // TAB 1: TROPAS
    else if (j->modoTienda == 1)
    {
        int cant = (esClickDerecho) ? 5 : 1;
        for (int i = 0; i < 4; i++)
        {
            int iy = startY + (i * 80);
            if (my >= iy && my <= iy + 60 && mx >= tx && mx <= tx + 300)
            {
                // --- BLOQUEOS TROPAS ---
                if (i == 0 && j->nivel < 3) {
                    crearTextoFlotante(msgX, msgY, "Req. Nivel 3", 0, RGB(255, 50, 50));
                    return;
                }

                // Soldado (i=0)
                if (i == 0)
                {
                    BOOL falta = FALSE;
                    if (j->hierro < 10 * cant) { sprintf(msg, "Faltan %d Hie", (10 * cant) - j->hierro); crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
                    if (j->oro < 40 * cant) { sprintf(msg, "Faltan %d Oro", (40 * cant) - j->oro); crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
                    
                    if (!falta) {
                        j->oro -= 40 * cant;
                        j->hierro -= 10 * cant;
                        j->cantSoldados += cant;
                        spawnearEscuadron(TIPO_SOLDADO, cant, j->x + 50, j->y);
                        ganarExperiencia(j, 10 * cant);
                        char logMsg[64];
    					sprintf(logMsg, "COMPRA: %d Soldado(s) reclutado(s).", cant);
    					RegistrarLog(logMsg);
                    }
                }
                // Minero (i=1)
                else if (i == 1)
                {
                    BOOL falta = FALSE;
                    if (j->piedra < 5 * cant) { sprintf(msg, "Faltan %d Pie", (5 * cant) - j->piedra); crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
                    if (j->oro < 15 * cant) { sprintf(msg, "Faltan %d Oro", (15 * cant) - j->oro); crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
                    
                    if (!falta) {
                        j->oro -= 15 * cant;
                        j->piedra -= 5 * cant;
                        j->cantMineros += cant;
                        spawnearEscuadron(TIPO_MINERO, cant, j->x + 50, j->y);
                        ganarExperiencia(j, 5 * cant);
                        char logMsg[64];
    					sprintf(logMsg, "COMPRA: %d Minero(s) reclutado(s).", cant);
    					RegistrarLog(logMsg);
                    }
                }
                // Leñador (i=2)
                else if (i == 2)
                {
                    BOOL falta = FALSE;
                    if (j->madera < 10 * cant) { sprintf(msg, "Faltan %d Mad", (10 * cant) - j->madera); crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
                    if (j->oro < 10 * cant) { sprintf(msg, "Faltan %d Oro", (10 * cant) - j->oro); crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
                    
                    if (!falta) {
                        j->oro -= 10 * cant;
                        j->madera -= 10 * cant;
                        j->cantLenadores += cant;
                        spawnearEscuadron(TIPO_LENADOR, cant, j->x + 50, j->y);
                        ganarExperiencia(j, 5 * cant);
                        char logMsg[64];
    					sprintf(logMsg, "COMPRA: %d Leñador(s) reclutado(s).", cant);
    					RegistrarLog(logMsg);
                    }
                }
                // Cazador (i=3)
                else if (i == 3)
                {
                    BOOL falta = FALSE;
                    if (j->hierro < 3 * cant) { sprintf(msg, "Faltan %d Hie", (3 * cant) - j->hierro); crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
                    if (j->oro < 20 * cant) { sprintf(msg, "Faltan %d Oro", (20 * cant) - j->oro); crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
                    
                    if (!falta) {
                        j->oro -= 20 * cant;
                        j->hierro -= 3 * cant;
                        j->cantCazadores += cant;
                        spawnearEscuadron(TIPO_CAZADOR, cant, j->x + 50, j->y);
                        ganarExperiencia(j, 5 * cant);
                        char logMsg[64];
    					sprintf(logMsg, "COMPRA: %d Cazador(s) reclutado(s).", cant);
    					RegistrarLog(logMsg);
                    }
                }
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
    }

    // TAB 2: LOGÍSTICA / MEJORAS
    else if (j->modoTienda == 2)
    {
        if (my >= startY && my <= startY + 60)
        { // Mochila 2 (Requiere Nivel 2)
            if (j->nivel < 2) { crearTextoFlotante(msgX, msgY, "Req. Nivel 2", 0, RGB(255, 50, 50)); return; }
            if (j->nivelMochila >= 2) return;

            BOOL falta = FALSE;
            if (j->hojas < 20) { sprintf(msg, "Faltan %d Hojas", 20 - j->hojas); crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
            if (j->oro < 99) { sprintf(msg, "Faltan %d Oro", 99 - j->oro); crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
            
            if (!falta) {
                j->oro -= 99;
                j->hojas -= 20;
                j->nivelMochila = 2;
                ganarExperiencia(j, 50);
                MessageBox(hwnd, "Mochila Mejorada (Nvl 2)!", "Mejora", MB_OK);
                RegistrarLog("MEJORA: Mochila ampliada a Nivel 2.");
            }
        }
        else if (my >= startY + 80 && my <= startY + 140)
        { // Mochila 3 (Requiere Nivel 4)
            if (j->nivel < 4) { crearTextoFlotante(msgX, msgY, "Req. Nivel 4", 0, RGB(255, 50, 50)); return; }
            if (j->nivelMochila >= 3) return;
            
            BOOL falta = FALSE;
            if (j->hierro < 50) { sprintf(msg, "Faltan %d Hie", 50 - j->hierro); crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
            if (j->oro < 200) { sprintf(msg, "Faltan %d Oro", 200 - j->oro); crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
            
            if (!falta) {
                j->oro -= 200;
                j->hierro -= 50;
                j->nivelMochila = 3;
                ganarExperiencia(j, 100);
                MessageBox(hwnd, "Mochila MAXIMA (Nvl 3)!", "Mejora", MB_OK);
                RegistrarLog("MEJORA: Mochila ampliada a Nivel 3."); 
            }
        }
        else if (my >= startY + 160 && my <= startY + 220)
        { // Barco de Guerra (Requiere Nivel 5)
            if (j->nivel < 5) { crearTextoFlotante(msgX, msgY, "Req. Nivel 5", 0, RGB(255, 50, 50)); return; }
            if (j->cantBarcosGuerra >= 4) { crearTextoFlotante(msgX, msgY, "Flota Maxima", 0, RGB(255, 100, 100)); return; }
            BOOL falta = FALSE;
            if (j->madera < 50) { sprintf(msg, "Faltan %d Mad", 50 - j->madera); crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
            if (j->oro < 100) { sprintf(msg, "Faltan %d Oro", 100 - j->oro); crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
            
           if (!falta) {
        j->oro -= 100;
        j->madera -= 50;
        j->cantBarcosGuerra++;
        if (j->cantBarcosGuerra > 1) {
            // Spawnear Barco Aliado al lado del muelle
            spawnearEscuadron(TIPO_BARCO_ALIADO, 1, MUELLE_X + 100, MUELLE_Y);
            crearTextoFlotante(j->x, j->y, "Flota: Barco Aliado!", 0, RGB(0, 255, 255));
        } else {
            crearTextoFlotante(j->x, j->y, "Barco Personal Listo!", 0, RGB(0, 255, 255));
        }
        
        ganarExperiencia(j, 50);
    }
        }
        else if (my >= startY + 240 && my <= startY + 300)
        { // Bote de Pesca (Requiere Nivel 4)
            if (j->nivel < 4) { crearTextoFlotante(msgX, msgY, "Req. Nivel 4", 0, RGB(255, 50, 50)); return; }
            if (j->tieneBotePesca) return;
            
            BOOL falta = FALSE;
            if (j->madera < 30) { sprintf(msg, "Faltan %d Mad", 30 - j->madera); crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
            if (j->oro < 50) { sprintf(msg, "Faltan %d Oro", 50 - j->oro); crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
            
            if (!falta) {
                j->oro -= 50;
                j->madera -= 30;
                j->tieneBotePesca = TRUE;
                ganarExperiencia(j, 30);
                crearTextoFlotante(msgX, msgY, "Bote Adquirido!", 0, RGB(0, 255, 255));
                RegistrarLog("COMPRA: Bote adquirido."); 
            }
        }
        else if (my >= startY + 320 && my <= startY + 380)
        { // Armadura de Placas (Requiere Nivel 3)
            if (j->nivel < 3) { crearTextoFlotante(msgX, msgY, "Req. Nivel 3", 0, RGB(255, 50, 50)); return; }
            if (j->tieneArmadura) return; 

            BOOL falta = FALSE;
            // Costo: 200 Oro + 50 Hierro
            if (j->hierro < 50) { sprintf(msg, "Faltan %d Hie", 50 - j->hierro); crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
            if (j->oro < 200) { sprintf(msg, "Faltan %d Oro", 200 - j->oro); crearTextoFlotante(msgX, msgY - 20, msg, 0, RGB(255, 50, 50)); falta = TRUE; }
            
            if (!falta) {
                j->oro -= 200;
                j->hierro -= 50;
                
                j->tieneArmadura = TRUE;    // Habilita el uso
                j->armaduraActual = 100;    // Rellena la barra de escudo visualmente
                j->armaduraEquipada = TRUE; // Se la pone automáticamente al comprar
                j->herramientaActiva = HERRAMIENTA_NADA; // Quita herramientas de la mano
                
                ganarExperiencia(j, 40);
                crearTextoFlotante(msgX, msgY, "Armadura Equipada!", 0, RGB(0, 255, 0));

            }
        }
        InvalidateRect(hwnd, NULL, FALSE);
    }

    // TAB 3: VENDER
    else if (j->modoTienda == 3)
    {
        int precios[] = {1, 2, 5, 1, 3};
        int *stocks[] = {&j->madera, &j->piedra, &j->hierro, &j->hojas, &j->comida};

        int maxOro = obtenerCapacidadMaxima(j->nivelMochila);

        for (int i = 0; i < 5; i++)
        {
            int iy = startY + (i * 60);
            if (my >= iy && my <= iy + 40 && mx >= tx && mx <= tx + 300)
            {
                int cant = (esClickDerecho) ? 10 : 1;

                if (*stocks[i] >= cant)
                {
                    int ganancia = cant * precios[i];

                    if (j->oro + ganancia > maxOro)
                    {
                        crearTextoFlotante(msgX, msgY, "Limite de Oro alcanzado!", 0, RGB(255, 50, 50));
                    }
                    else
                    {
                        *stocks[i] -= cant;
                        j->oro += ganancia; 
                        sprintf(msg, "+%d Oro", ganancia);
                        crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 215, 0));
                        char logVenta[64];
    					char* nombresRecursos[] = {"Madera", "Piedra", "Hierro", "Comida", "Hojas"};
						sprintf(logVenta, "VENTA: %d unidades de %s vendidas.", cant, nombresRecursos[i]);
    					RegistrarLog(logVenta);
                    }
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                else
                {
                    crearTextoFlotante(msgX, msgY, "No tienes suficiente", 0, RGB(255, 100, 100));
                }
            }
        }
    }
    // TAB 4: EDIFICIOS 
    else if (j->modoTienda == 4)
    {
        int costos[] = {100, 300, 600}; // Peq, Med, Gran
        for (int i = 0; i < 3; i++) // 3 tipos de edificios
        {
            int iy = startY + (i * 80);
            if (my >= iy && my <= iy + 60 && mx >= tx && mx <= tx + 300)
            {
                // Validación básica de Dinero 
                if (j->oro < costos[i]) {
                    sprintf(msg, "Faltan %d Oro", costos[i] - j->oro);
                    crearTextoFlotante(msgX, msgY, msg, 0, RGB(255, 50, 50));

                    return;
                }
                // Validación de Límite
                if (misEdificios[i].activo) {
                    crearTextoFlotante(msgX, msgY, "Ya construido!", 0, RGB(255, 100, 100));
                    return;
                }
                // ¡ACTIVAR MODO FANTASMA!
                j->edificioSeleccionado = i + 1; // 1=Peq, 2=Med, 3=Grande
                j->tiendaAbierta = 0; // Cerramos la tienda automáticamente
                crearTextoFlotante(msgX, msgY, "Modo Construccion", 0, RGB(0, 255, 0));
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
    }	
    
}

// --- LÓGICA DE DIBUJO DE LA UI (PANEL) ---
void dibujarTiendaInteractiva(HDC hdc, Jugador *j, int ancho, int alto)
{
    // POSICIÓN DERECHA
    int anchoW = 340; // Ancho de la tienda
    int altoW = 480;  // Alto de la tienda
    int tx = ancho - anchoW - 20;
    int ty = 80;

    // 1. FONDO
    HBRUSH fondo = CreateSolidBrush(RGB(45, 40, 35));
    RECT r = {tx, ty, tx + anchoW, ty + altoW};
    FillRect(hdc, &r, fondo);
    DeleteObject(fondo);
    HBRUSH borde = CreateSolidBrush(RGB(218, 165, 32));
    FrameRect(hdc, &r, borde);
    DeleteObject(borde);

    // 2. PESTAÑAS (Categorías)
	char *tabs[] = {"HERR.", "TROP.", "LOGIST", "VEND.", "EDIF."}; // Nombres cortos
    int tabW = anchoW / 5;

   SetBkMode(hdc, TRANSPARENT);
    for (int i = 0; i < 5; i++)
    {
        COLORREF colorT = (j->modoTienda == i) ? RGB(255, 215, 0) : RGB(100, 100, 100);
        SetTextColor(hdc, colorT);
        TextOut(hdc, tx + 5 + (i * tabW), ty + 10, tabs[i], strlen(tabs[i]));

        if (j->modoTienda == i)
        {
            HPEN penLine = CreatePen(PS_SOLID, 2, RGB(255, 215, 0));
            HGDIOBJ old = SelectObject(hdc, penLine);
            MoveToEx(hdc, tx + (i * tabW), ty + 30, NULL);
            LineTo(hdc, tx + ((i + 1) * tabW), ty + 30);
            SelectObject(hdc, old); DeleteObject(penLine);
        }
    }

    // 3. CONTENIDO
    int startX = tx + 20;
    int startY = ty + 50;
    SetTextColor(hdc, RGB(255, 255, 255));

    // --- TAB 0: HERRAMIENTAS ---
    if (j->modoTienda == 0)
    {
        struct ItemShop
        {
            char *nom;
            HBITMAP img;
            int costO;
            int costMat;
            char *matNom;
            BOOL tiene;
        };
        struct ItemShop items[] = {
            {"Espada", hBmpIconoEspada, 50, 0, "", j->tieneEspada},
            {"Pico", hBmpIconoPico, 30, 0, "", j->tienePico},
            {"Hacha", hBmpIconoHacha, 20, 0, "", j->tieneHacha},
            {"Cana", hBmpIconoCana, 30, 10, "Mad", j->tieneCana}};

        for (int i = 0; i < 4; i++)
        {
            int iy = startY + (i * 80);
            if (items[i].img)
                DibujarImagen(hdc, items[i].img, startX, iy, 40, 40);
            TextOut(hdc, startX + 50, iy, items[i].nom, strlen(items[i].nom));

            if (items[i].tiene)
            {
                SetTextColor(hdc, RGB(0, 255, 0));
                TextOut(hdc, startX + 50, iy + 20, "EN PROPIEDAD", 12);
            }
            else
            {
                char precio[32];
                if (items[i].costMat > 0)
                    sprintf(precio, "%d Oro + %d %s", items[i].costO, items[i].costMat, items[i].matNom);
                else
                    sprintf(precio, "%d Oro", items[i].costO);
                SetTextColor(hdc, RGB(255, 215, 0));
                TextOut(hdc, startX + 50, iy + 20, precio, strlen(precio));
            }
            SetTextColor(hdc, RGB(255, 255, 255));
        }
    }

    // --- TAB 1: TROPAS (Sprites) ---
    else if (j->modoTienda == 1)
    {
        struct TropaShop
        {
            char *nom;
            HBITMAP sprite;
            int costO;
            int costMat;
            char *matNom;
        };
        struct TropaShop tropas[] = {
            {"Soldado", hBmpSoldadoAnim[0][0], 40, 10, "Hierro"},
            {"Minero", hBmpMineroAnim[0][0], 15, 5, "Piedra"},
            {"Lenador", hBmpLenadorAnim[0][0], 10, 10, "Madera"},
            {"Cazador", hBmpCazadorAnim[0][0], 20, 3, "Hierro"}};

        for (int i = 0; i < 4; i++)
        {
            int iy = startY + (i * 80);
            // DIBUJAR SPRITE DEL PERSONAJE
            if (tropas[i].sprite)
            {
                DibujarImagen(hdc, tropas[i].sprite, startX, iy, 40, 40);
            }
            TextOut(hdc, startX + 50, iy, tropas[i].nom, strlen(tropas[i].nom));
            char costes[32];
            sprintf(costes, "%d Oro + %d %s", tropas[i].costO, tropas[i].costMat, tropas[i].matNom);
            SetTextColor(hdc, RGB(200, 200, 200));
            TextOut(hdc, startX + 50, iy + 20, costes, strlen(costes));
            SetTextColor(hdc, RGB(255, 255, 255));
        }
        TextOut(hdc, startX, startY + 350, "Click Der: Comprar x5", 21);
    }

    // --- TAB 2: LOGÍSTICA ---
    else if (j->modoTienda == 2)
    {
        int iy = startY;
        // M2
        DibujarImagen(hdc, hBmpInvCerrado, startX, iy, 40, 40);
        TextOut(hdc, startX + 50, iy, "Mochila N2", 10);
        if (j->nivelMochila >= 2)
        {
            SetTextColor(hdc, RGB(0, 255, 0));
            TextOut(hdc, startX + 50, iy + 20, "COMPRADO", 8);
        }
        else
        {
            SetTextColor(hdc, RGB(255, 215, 0));
            TextOut(hdc, startX + 50, iy + 20, "99 Oro + 20 Hoj", 15);
        }
        SetTextColor(hdc, RGB(255, 255, 255));

        // M3
        iy += 80;
        DibujarImagen(hdc, hBmpInvCerrado, startX, iy, 40, 40);
        TextOut(hdc, startX + 50, iy, "Mochila N3", 10);
        if (j->nivelMochila >= 3)
        {
            SetTextColor(hdc, RGB(0, 255, 0));
            TextOut(hdc, startX + 50, iy + 20, "COMPRADO", 8);
        }
        else
        {
            SetTextColor(hdc, RGB(255, 215, 0));
            TextOut(hdc, startX + 50, iy + 20, "200 Oro + 50 Hierro", 19);
        }
        SetTextColor(hdc, RGB(255, 255, 255));

        // Galeon
        iy += 80;
        if (hBmpBarco[1])
            DibujarImagen(hdc, hBmpBarco[1], startX, iy, 40, 40);
        TextOut(hdc, startX + 50, iy, "Galeon Guerra", 13);
        char flota[32];
        sprintf(flota, "Flota: %d/4", j->cantBarcosGuerra);
        TextOut(hdc, startX + 50, iy + 20, flota, strlen(flota));
        SetTextColor(hdc, RGB(255, 215, 0));
        TextOut(hdc, startX + 50, iy + 35, "100 O + 50 M", 12);
        SetTextColor(hdc, RGB(255, 255, 255));

        // Bote
        iy += 80;
        if (hBmpBote[1])
            DibujarImagen(hdc, hBmpBote[1], startX, iy, 40, 40);
        TextOut(hdc, startX + 50, iy, "Bote Pesca", 10);
        if (j->tieneBotePesca)
        {
            SetTextColor(hdc, RGB(0, 255, 0));
            TextOut(hdc, startX + 50, iy + 20, "COMPRADO", 8);
        }
        else
        {
            SetTextColor(hdc, RGB(255, 215, 0));
            TextOut(hdc, startX + 50, iy + 20, "50 O + 30 M", 11);
        }
        SetTextColor(hdc, RGB(255, 255, 255));

        // Armadura
        iy += 80;
        HBITMAP iconoArm = (hBmpIconoArmaduraInv) ? hBmpIconoArmaduraInv : hBmpIconoPiedra; 
        DibujarImagen(hdc, iconoArm, startX, iy, 40, 40);
        
        TextOut(hdc, startX + 50, iy, "Armadura Pesada", 15);
        
        if (j->tieneArmadura)
        {
            SetTextColor(hdc, RGB(0, 255, 0));
            TextOut(hdc, startX + 50, iy + 20, "EN PROPIEDAD", 12);
        }
        else
        {
            SetTextColor(hdc, RGB(255, 215, 0));
            TextOut(hdc, startX + 50, iy + 20, "200 O + 50 Hie", 14);
        }
        SetTextColor(hdc, RGB(255, 255, 255));
    }

    // --- TAB 3: VENDER ---
    else if (j->modoTienda == 3)
    {
        char *nombres[] = {"Madera", "Piedra", "Hierro", "Hojas", "Comida"};
        HBITMAP iconos[] = {hBmpIconoMadera, hBmpIconoPiedra, hBmpIconoHierro, hBmpIconoHoja, hBmpIconoComida};
        int cantidades[] = {j->madera, j->piedra, j->hierro, j->hojas, j->comida};
        int valores[] = {1, 2, 5, 1, 3};

        for (int i = 0; i < 5; i++)
        {
            int iy = startY + (i * 60);
            if (iconos[i])
                DibujarImagen(hdc, iconos[i], startX, iy, 32, 32);
            char texto[64];
            sprintf(texto, "%s: %d", nombres[i], cantidades[i]);
            TextOut(hdc, startX + 40, iy, texto, strlen(texto));
            sprintf(texto, "Vender: +%d Oro", valores[i]);
            SetTextColor(hdc, RGB(255, 215, 0));
            TextOut(hdc, startX + 40, iy + 15, texto, strlen(texto));
            SetTextColor(hdc, RGB(255, 255, 255));
        }
    }
    // TAB 4: EDIFICIOS (VISUAL)
    else if (j->modoTienda == 4)
    {
        struct EdifUI {
            char *nombre;
            HBITMAP sprite;
            char *costo;
            int tipoID;
        };
        
        // Usamos el sprite [0] de cada array para el icono
        struct EdifUI lista[] = {
            {"Choza", hBmpEdificioPeq[0], "100 Oro", 0},
            {"Torre", hBmpEdificioMed[0], "300 Oro", 1},
            {"Fortaleza", hBmpEdificioGrande[0], "600 Oro", 2}
        };

        for (int i = 0; i < 3; i++)
        {
            int iy = startY + (i * 80);
            // Icono del edificio 
            if (lista[i].sprite)
                DibujarImagen(hdc, lista[i].sprite, startX, iy, 48, 48);
            // Nombre
            TextOut(hdc, startX + 60, iy, lista[i].nombre, strlen(lista[i].nombre));
            
            // Estado (Comprado o Precio)
            if (misEdificios[lista[i].tipoID].activo) {
                SetTextColor(hdc, RGB(100, 255, 100));
                TextOut(hdc, startX + 60, iy + 20, "CONSTRUIDO", 10);
            } else {
                SetTextColor(hdc, RGB(255, 215, 0));
                TextOut(hdc, startX + 60, iy + 20, lista[i].costo, strlen(lista[i].costo));
            }
            SetTextColor(hdc, RGB(255, 255, 255));
        }
        // Instrucción
        SetTextColor(hdc, RGB(200, 200, 200));
        TextOut(hdc, startX, startY + 350, "Click para construir", 20);
    }
}