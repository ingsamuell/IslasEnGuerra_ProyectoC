#include "unidades.h"
#include "../mundo/mapa.h"       // Para EsSuelo
#include "../mundo/naturaleza.h" // Para buscarArbolCercano, misArboles, etc.
#include "../mundo/fauna.h"      // Para buscarVacaCercana, manada
#include "../recursos/recursos.h"
#include "../jugador/jugador.h"
#include <math.h>
#include <stdio.h>

Unidad unidades[MAX_UNIDADES];

void inicializarUnidades() {
    for (int i = 0; i < MAX_UNIDADES; i++) {
        unidades[i].activa = 0;
        unidades[i].seleccionado = 0;
    }
}

void spawnearEscuadron(int tipo, int cantidad, int x, int y) {
    int creados = 0;
    char *nombre;
    switch (tipo) {
        case TIPO_MINERO: nombre = "Mineros"; break;
        case TIPO_LENADOR: nombre = "Lenadores"; break;
        case TIPO_CAZADOR: nombre = "Cazadores"; break;
        case TIPO_SOLDADO: nombre = "Infanteria"; break;
        default: nombre = "Unidad"; break;
    }

    for (int i = 0; i < MAX_UNIDADES && creados < cantidad; i++) {
        if (!unidades[i].activa) {
            unidades[i].x = x + (creados * 10);
            unidades[i].y = y + (creados * 5);
            unidades[i].tipo = tipo;
            unidades[i].activa = 1;
            unidades[i].estado = ESTADO_IDLE;
            strcpy(unidades[i].nombreGrupo, nombre);
            creados++;
        }
    }
}

void aplicarSeparacion(int id, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    float sepX = 0, sepY = 0;
    int vecinos = 0;
    float radioPersonal = 25.0f; // Distancia mínima entre aldeanos

    for (int j = 0; j < MAX_UNIDADES; j++) {
        // No compararse con uno mismo ni con unidades muertas
        if (id == j || !unidades[j].activa) continue;

        float dx = unidades[id].x - unidades[j].x;
        float dy = unidades[id].y - unidades[j].y;
        float distSq = dx*dx + dy*dy;

        // Si están dentro del radio personal (muy cerca)
        if (distSq < (radioPersonal * radioPersonal) && distSq > 0.1f) {
            float dist = sqrt(distSq);
            
            // Calculamos una fuerza de repulsión:
            // Cuanto más cerca estén, más fuerte es el empujón.
            float fuerza = (radioPersonal - dist) / radioPersonal; 
            
            sepX += (dx / dist) * fuerza;
            sepY += (dy / dist) * fuerza;
            vecinos++;
        }
    }

    if (vecinos > 0) {
        // Aplicamos el empujón (Separación)
        // El factor 1.5f controla qué tan fuerte se empujan (ajústalo si vibran mucho)
        float nuevoX = unidades[id].x + (sepX * 1.5f);
        float nuevoY = unidades[id].y + (sepY * 1.5f);

        // IMPORTANTE: Solo empujamos si el destino es suelo (para no tirarlos al agua)
        // Usamos +16 para verificar el centro del sprite
        if (EsSuelo((int)nuevoX + 16, (int)nuevoY + 16, mapa)) {
            unidades[id].x = nuevoX;
            unidades[id].y = nuevoY;
        }
    }
}
void actualizarUnidades(char mapa[MUNDO_FILAS][MUNDO_COLUMNAS], Jugador *j) {
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa) continue;
        
        // La IA enemiga suele procesarse en una función aparte para no mezclarse 
        // con la lógica de recolección del jugador
        if (unidades[i].bando == BANDO_ENEMIGO) continue; 

        aplicarSeparacion(i, mapa);

        float dx = unidades[i].destinoX - unidades[i].x;
        float dy = unidades[i].destinoY - unidades[i].y;
        float dist = sqrt(dx * dx + dy * dy);

        switch (unidades[i].estado) {
            
            // --- NUEVO: LÓGICA DE COMBATE ---
            case ESTADO_PERSIGUIENDO: {
                int target = unidades[i].targetIndex;
                // Si el objetivo desaparece o muere, volver a espera
                if (target == -1 || !unidades[target].activa) {
                    unidades[i].estado = ESTADO_IDLE;
                    break;
                }

                float dxE = unidades[target].x - unidades[i].x;
                float dyE = unidades[target].y - unidades[i].y;
                float distE = sqrt(dxE * dxE + dyE * dyE);

                if (distE > 40.0f) { 
        float velocidad = (unidades[i].tipo == TIPO_SOLDADO) ? 2.5f : 2.0f;
        float nuevoX = unidades[i].x + (dxE / distE) * velocidad;
        float nuevoY = unidades[i].y + (dyE / distE) * velocidad;

        // --- LÓGICA DE MOVIMIENTO ---
        // Si es Soldado o Enemigo, SIEMPRE puede moverse (Tierra o Agua)
        // Si es Aldeano, SOLO si EsSuelo
        BOOL puedeMover = FALSE;
        if (unidades[i].tipo == TIPO_SOLDADO || unidades[i].bando == BANDO_ENEMIGO) {
            puedeMover = TRUE; // ¡Barcos activados!
        } else {
            if (EsSuelo((int)nuevoX + 16, (int)nuevoY + 16, mapa)) puedeMover = TRUE;
        }

        if (puedeMover) {
            unidades[i].x = nuevoX;
            unidades[i].y = nuevoY;
        }
        actualizarAnimacionUnidad(&unidades[i], dxE, dyE);
    }
                break;
            }

            case ESTADO_ATACANDO: {
                int target = unidades[i].targetIndex;
                if (target == -1 || !unidades[target].activa) {
                    unidades[i].estado = ESTADO_IDLE;
                    break;
                }

                // Verificar si se alejó (por si el enemigo corre)
                float dxE = unidades[target].x - unidades[i].x;
                float dyE = unidades[target].y - unidades[i].y;
                if (sqrt(dxE*dxE + dyE*dyE) > 55.0f) {
                    unidades[i].estado = ESTADO_PERSIGUIENDO;
                    break;
                }

                unidades[i].timerAtaque++;
                if (unidades[i].timerAtaque >= 60) { // Velocidad de ataque
                    unidades[i].timerAtaque = 0;
                    int dano = (unidades[i].tipo == TIPO_SOLDADO) ? 15 : 5;
                    unidades[target].vida -= dano;
                    
                    crearChispas(unidades[target].x, unidades[target].y, RGB(255, 0, 0));
                    
                    if (unidades[target].vida <= 0) {
                        unidades[target].activa = 0;
                        ganarExperiencia(j, 50);
                        crearTextoFlotante(unidades[i].x, unidades[i].y, "+50 XP", 0, RGB(0, 255, 255));
                        unidades[i].estado = ESTADO_IDLE;
                    }
                }
                break;
            }

            // --- LÓGICA DE TRABAJO (SIN CAMBIOS) ---
            case ESTADO_TALANDO: {
                int a = unidades[i].targetIndex;
                if (a == -1 || !misArboles[a].activa) { 
                    int nuevo = buscarArbolCercano(unidades[i].x, unidades[i].y, 150.0f);
                    if (nuevo != -1) {
                        unidades[i].targetIndex = nuevo;
                        unidades[i].destinoX = misArboles[nuevo].x;
                        unidades[i].destinoY = misArboles[nuevo].y;
                    } else {
                        unidades[i].estado = ESTADO_IDLE;
                    }
                    break; 
                }
                
                float targetX = misArboles[a].x + 16;
                float targetY = misArboles[a].y + 16;
                float dxT = targetX - unidades[i].x;
                float dyT = targetY - unidades[i].y;
                float distObjeto = sqrt(dxT*dxT + dyT*dyT);

                if (distObjeto > 50.0f) { 
                    unidades[i].x += (dxT / distObjeto) * 2.0f;
                    unidades[i].y += (dyT / distObjeto) * 2.0f;
                    actualizarAnimacionUnidad(&unidades[i], dxT, dyT);
                } else {
                    unidades[i].timerTrabajo++;
                    if (unidades[i].timerTrabajo >= 250) { 
                        unidades[i].timerTrabajo = 0;
                        int mad = (misArboles[a].tipo == 1) ? 5 : 3;
                        int hoj = (misArboles[a].tipo == 1) ? 10 : 4;
                        int antMad = j->madera;
                        int antHoj = j->hojas;

                        agregarRecurso(&j->madera, mad, j->nivelMochila);
                        agregarRecurso(&j->hojas, hoj, j->nivelMochila);

                        int ganMad = j->madera - antMad;
                        int ganHoj = j->hojas - antHoj;

                        if (ganMad > 0) crearTextoFlotante(misArboles[a].x, misArboles[a].y, "Madera", ganMad, RGB(150, 75, 0));
                        if (ganHoj > 0) crearTextoFlotante(misArboles[a].x, misArboles[a].y - 20, "Hojas", ganHoj, RGB(34, 139, 34));
                        
                        if (ganMad == 0 && ganHoj == 0) crearTextoFlotante(unidades[i].x, unidades[i].y - 30, "Mochila Llena!", 0, RGB(255, 50, 50));

                        crearChispas(targetX, targetY, RGB(139, 69, 19));
                        misArboles[a].activa = 0;
                        misArboles[a].timerRegeneracion = 0;
                        
                        unidades[i].targetIndex = -1;
                        int next = buscarArbolCercano(unidades[i].x, unidades[i].y, 200.0f);
                        if(next != -1) unidades[i].targetIndex = next;
                        else unidades[i].estado = ESTADO_IDLE;
                    }
                }
                break;
            }

            case ESTADO_MINANDO: {
                int m = unidades[i].targetIndex;
                if (m == -1 || !misMinas[m].activa) { 
                    int nueva = buscarMinaCercana(unidades[i].x, unidades[i].y, 150.0f);
                    if (nueva != -1) unidades[i].targetIndex = nueva;
                    else unidades[i].estado = ESTADO_IDLE;
                    break; 
                }
                
                float targetX = misMinas[m].x + 16;
                float targetY = misMinas[m].y + 16;
                float dxM = targetX - unidades[i].x;
                float dyM = targetY - unidades[i].y;
                float distMina = sqrt(dxM*dxM + dyM*dyM);

                if (distMina > 50.0f) { 
                    unidades[i].x += (dxM / distMina) * 2.0f;
                    unidades[i].y += (dyM / distMina) * 2.0f;
                    actualizarAnimacionUnidad(&unidades[i], dxM, dyM);
                } else {
                    unidades[i].timerTrabajo++;
                    if (unidades[i].timerTrabajo >= 400) { 
                        unidades[i].timerTrabajo = 0;
                        if (misMinas[m].tipo == 0) { // Piedra
                            int ant = j->piedra;
                            agregarRecurso(&j->piedra, 10, j->nivelMochila);
                            int gan = j->piedra - ant;
                            if (gan > 0) crearTextoFlotante(unidades[i].x, unidades[i].y, "Piedra", gan, RGB(150, 150, 150));
                            else crearTextoFlotante(unidades[i].x, unidades[i].y - 30, "Mochila Llena!", 0, RGB(255, 50, 50));
                        } else { // Hierro
                            int ant = j->hierro;
                            agregarRecurso(&j->hierro, 7, j->nivelMochila);
                            int gan = j->hierro - ant;
                            if (gan > 0) crearTextoFlotante(unidades[i].x, unidades[i].y, "Hierro", gan, RGB(192, 192, 192));
                            else crearTextoFlotante(unidades[i].x, unidades[i].y - 30, "Mochila Llena!", 0, RGB(255, 50, 50));
                        }
                        crearChispas(targetX, targetY, RGB(200, 200, 200));
                        misMinas[m].activa = 0;
                        unidades[i].targetIndex = -1;
                        int next = buscarMinaCercana(unidades[i].x, unidades[i].y, 200.0f);
                        if(next != -1) unidades[i].targetIndex = next;
                        else unidades[i].estado = ESTADO_IDLE;
                    }
                }
                break;
            }

            case ESTADO_CAZANDO: {
                int v = unidades[i].targetIndex;
                if (v == -1 || !manada[v].activa || manada[v].estadoVida != 0) {
                    int proxima = buscarVacaCercana(unidades[i].x, unidades[i].y, 500.0f);
                    if (proxima != -1) unidades[i].targetIndex = proxima;
                    else unidades[i].estado = ESTADO_IDLE;
                    break; 
                }

                float dvx = manada[v].x - unidades[i].x;
                float dvy = manada[v].y - unidades[i].y;
                float distVaca = sqrt(dvx * dvx + dvy * dvy);

                if (distVaca > 35.0f) {
                    unidades[i].x += (dvx / distVaca) * 2.2f;
                    unidades[i].y += (dvy / distVaca) * 2.2f;
                    actualizarAnimacionUnidad(&unidades[i], dvx, dvy);
                } else {
                    unidades[i].timerTrabajo++;
                    if (unidades[i].timerTrabajo >= 300) { 
                        manada[v].estadoVida = 1; 
                        manada[v].tiempoMuerte = 300; 
                        int ant = j->comida;
                        agregarRecurso(&j->comida, 15, j->nivelMochila);
                        int gan = j->comida - ant;
                        if (gan > 0) crearTextoFlotante(manada[v].x, manada[v].y, "+15 Comida", 0, RGB(255, 200, 0));
                        else crearTextoFlotante(unidades[i].x, unidades[i].y - 30, "Mochila Llena!", 0, RGB(255, 50, 50));
                        crearChispas(manada[v].x + 16, manada[v].y + 16, RGB(255, 0, 0));
                        unidades[i].timerTrabajo = 0;
                        unidades[i].targetIndex = -1;
                        int siguienteVaca = buscarVacaCercana(unidades[i].x, unidades[i].y, 600.0f);
                        if (siguienteVaca != -1) unidades[i].targetIndex = siguienteVaca;
                        else unidades[i].estado = ESTADO_IDLE;
                    }
                }
                break;
            }

            case ESTADO_MOVIENDO:
                if (dist > 5.0f) {
                    unidades[i].x += (dx / dist) * 2.2f;
                    unidades[i].y += (dy / dist) * 2.2f;
                    actualizarAnimacionUnidad(&unidades[i], dx, dy);
                } else {
                    unidades[i].estado = ESTADO_IDLE;
                }
                break;

            case ESTADO_IDLE:
                if (unidades[i].tipo == TIPO_CAZADOR) {
                    int vacaCerca = buscarVacaCercana(unidades[i].x, unidades[i].y, 200.0f);
                    if (vacaCerca != -1) {
                        unidades[i].estado = ESTADO_CAZANDO;
                        unidades[i].targetIndex = vacaCerca;
                    }
                }
                break;
        }
    }
}

void actualizarAnimacionUnidad(Unidad *u, float dx, float dy) {
    if (fabs(dx) > fabs(dy)) u->direccion = (dx > 0) ? DIR_DERECHA : DIR_IZQUIERDA;
    else u->direccion = (dy > 0) ? DIR_ABAJO : DIR_ARRIBA;
    
    u->contadorAnim++;
    if (u->contadorAnim > 10) {
        u->frameAnim = (u->frameAnim + 1) % 3;
        u->contadorAnim = 0;
    }
}

void dibujarUnidades(HDC hdc, Camera cam) {
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa) continue;

        // 1. Coordenadas de pantalla
        int ux = (int)((unidades[i].x - cam.x) * cam.zoom);
        int uy = (int)((unidades[i].y - cam.y) * cam.zoom);
        int tam = (int)(TAMANO_CELDA * cam.zoom);

        // 2. DETECTAR SI ESTÁ EN AGUA (Para dibujar Bote)
        // Usamos la matriz global mapaMundo (asegúrate de tener acceso a ella o pásala)
        extern char mapaMundo[MUNDO_FILAS][MUNDO_COLUMNAS];
        int col = (int)((unidades[i].x + 16) / TAMANO_CELDA_BASE);
        int fila = (int)((unidades[i].y + 16) / TAMANO_CELDA_BASE);
        
        BOOL enAgua = FALSE;
        if (col >= 0 && col < MUNDO_COLUMNAS && fila >= 0 && fila < MUNDO_FILAS) {
            if (mapaMundo[fila][col] == 0) enAgua = TRUE;
        }

        // 3. SELECCIONAR SPRITE
        HBITMAP hBmpActual = NULL;

        // Si está en el agua y es Soldado o Enemigo -> DIBUJAR BOTE
        if (enAgua && (unidades[i].tipo == TIPO_SOLDADO || 
                       unidades[i].tipo == TIPO_ENEMIGO_PIRATA || 
                       unidades[i].tipo == TIPO_ENEMIGO_MAGO)) {
            
            // Elegir dirección del bote
            int dirBote = (unidades[i].direccion == DIR_IZQUIERDA) ? 0 : 1;
            
            // Usamos el mismo sprite de bote que el jugador o uno de enemigo si tienes
            if (unidades[i].bando == BANDO_ENEMIGO) {
                 hBmpActual = hBmpBarco[dirBote]; // Barco pirata (Galeon)
                 tam = (int)(48 * cam.zoom); // Un poco más grande
                 ux -= 10 * cam.zoom; // Ajuste de centro
            } else {
                 hBmpActual = hBmpBote[dirBote]; // Bote aliado
            }
        }
        else {
            // SI ESTÁ EN TIERRA -> DIBUJAR UNIDAD NORMAL
            switch (unidades[i].tipo) {
                case TIPO_MINERO: hBmpActual = hBmpMineroAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
                case TIPO_LENADOR: hBmpActual = hBmpLenadorAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
                case TIPO_CAZADOR: hBmpActual = hBmpCazadorAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
                case TIPO_SOLDADO: hBmpActual = hBmpSoldadoAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
                
                // --- AQUÍ ESTABA EL ERROR ANTES (Faltaban estos casos) ---
                case TIPO_ENEMIGO_PIRATA: hBmpActual = hBmpPirataAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
                case TIPO_ENEMIGO_MAGO:   hBmpActual = hBmpMagoAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
                
                default: hBmpActual = hBmpJugadorAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
            }
        }

        // 4. DIBUJAR IMAGEN FINAL
        if (hBmpActual) {
            DibujarImagen(hdc, hBmpActual, ux, uy, tam, tam);
        }

        // 5. CÍRCULO DE SELECCIÓN (Solo aliados)
        if (unidades[i].seleccionado) {
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
            HGDIOBJ hOld = SelectObject(hdc, hPen);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Ellipse(hdc, ux, uy + tam - (int)(10 * cam.zoom), ux + tam, uy + tam);
            SelectObject(hdc, hOld);
            DeleteObject(hPen);
        }

        // 6. BARRA DE VIDA (Roja para enemigos, Verde para aliados)
        int maxVida = (unidades[i].vidaMax > 0) ? unidades[i].vidaMax : 100;
        
        // Cambiar color de barra según bando (Debes modificar dibujarBarraVidaLocal para aceptar color o hacerlo manualmente)
        // Por ahora usamos la función estándar pero ajustamos posición
        dibujarBarraVidaLocal(hdc, ux, uy - 10, unidades[i].vida, maxVida, tam);
    }
}

void seleccionarUnidadesGrupo(int x1, int y1, int x2, int y2, Camera cam) {
    int minX = min(x1, x2);
    int maxX = max(x1, x2);
    int minY = min(y1, y2);
    int maxY = max(y1, y2);
    bool esClickSimple = (maxX - minX < 5 && maxY - minY < 5);

    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa) continue;

        int ux = (int)((unidades[i].x - cam.x) * cam.zoom);
        int uy = (int)((unidades[i].y - cam.y) * cam.zoom);
        int size = (int)(32 * cam.zoom);

        if (esClickSimple) {
            if (x1 >= ux && x1 <= ux + size && y1 >= uy && y1 <= uy + size) unidades[i].seleccionado = 1;
            else unidades[i].seleccionado = 0;
        } else {
            if (ux + (size / 2) >= minX && ux + (size / 2) <= maxX &&
                uy + (size / 2) >= minY && uy + (size / 2) <= maxY) {
                unidades[i].seleccionado = 1;
            } else {
                unidades[i].seleccionado = 0;
            }
        }
    }
}

void darOrdenMovimiento(Unidad unidades[], int max, int clickX, int clickY) {
    int fila = 0;
    int columna = 0;
    for (int i = 0; i < max; i++) {
        if (unidades[i].activa && unidades[i].seleccionado) {
            unidades[i].destinoX = clickX + (columna * 35);
            unidades[i].destinoY = clickY + (fila * 20);
            columna++;
            if (columna > 3) { columna = 0; fila++; }
        }
    }
}

void ordenarUnidad(int mX, int mY, Camera cam) {
    float mundoX = (mX / cam.zoom) + cam.x;
    float mundoY = (mY / cam.zoom) + cam.y;

    // --- 1. DETECTAR SI EL CLIC FUE SOBRE UN ENEMIGO ---
    int enemigoID = -1;
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (unidades[i].activa && unidades[i].bando == BANDO_ENEMIGO) {
            // Radio de clic para enemigos (40px aprox)
            if (sqrt(pow(unidades[i].x - mundoX, 2) + pow(unidades[i].y - mundoY, 2)) < 40) {
                enemigoID = i;
                break;
            }
        }
    }

    // --- 2. ASIGNAR ÓRDENES A CADA UNIDAD SELECCIONADA ---
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa || !unidades[i].seleccionado) continue;
        if (unidades[i].bando == BANDO_ENEMIGO) continue; // Solo controlamos nuestras unidades

        unidades[i].timerTrabajo = 0;
        bool accionAsignada = false;

        // PRIORIDAD A: ATAQUE (Si hay un enemigo seleccionado)
        if (enemigoID != -1) {
            // Solo los soldados o unidades de combate atacan directamente
            // Si quieres que los aldeanos también luchen, quita la condición de TIPO_SOLDADO
            if (unidades[i].tipo == TIPO_SOLDADO) {
                unidades[i].estado = ESTADO_PERSIGUIENDO;
                unidades[i].targetIndex = enemigoID;
                crearTextoFlotante(unidades[i].x, unidades[i].y, "¡Al ataque!", 0, RGB(255, 0, 0));
                accionAsignada = true;
            }
        }

        // PRIORIDAD B: TRABAJO (Si no hay enemigo o la unidad no es guerrera)
        if (!accionAsignada) {
            // Leñador -> Árboles
            if (unidades[i].tipo == TIPO_LENADOR) {
                for (int a = 0; a < MAX_ARBOLES; a++) {
                    if (!misArboles[a].activa) continue;
                    if (sqrt(pow((misArboles[a].x + 16) - mundoX, 2) + pow((misArboles[a].y + 16) - mundoY, 2)) < 60.0f) {
                        unidades[i].estado = ESTADO_TALANDO;
                        unidades[i].targetIndex = a;
                        unidades[i].destinoX = misArboles[a].x;
                        unidades[i].destinoY = misArboles[a].y;
                        accionAsignada = true;
                        break;
                    }
                }
            }
            
            // Minero -> Minas
            if (!accionAsignada && unidades[i].tipo == TIPO_MINERO) {
                for (int m = 0; m < MAX_MINAS; m++) {
                    if (!misMinas[m].activa) continue;
                    if (sqrt(pow((misMinas[m].x + 16) - mundoX, 2) + pow((misMinas[m].y + 16) - mundoY, 2)) < 60.0f) {
                        unidades[i].estado = ESTADO_MINANDO;
                        unidades[i].targetIndex = m;
                        unidades[i].destinoX = misMinas[m].x;
                        unidades[i].destinoY = misMinas[m].y;
                        accionAsignada = true;
                        break;
                    }
                }
            }

            // Cazador -> Vacas
            if (!accionAsignada && unidades[i].tipo == TIPO_CAZADOR) {
                for (int v = 0; v < MAX_VACAS; v++) {
                    if (!manada[v].activa || manada[v].estadoVida != 0) continue;
                    if (sqrt(pow((manada[v].x + 16) - mundoX, 2) + pow((manada[v].y + 16) - mundoY, 2)) < 60.0f) {
                        unidades[i].estado = ESTADO_CAZANDO;
                        unidades[i].targetIndex = v;
                        accionAsignada = true;
                        break;
                    }
                }
            }
        }

        // PRIORIDAD C: MOVIMIENTO SIMPLE (Si no hubo ataque ni trabajo)
        if (!accionAsignada) {
            unidades[i].estado = ESTADO_MOVIENDO;
            unidades[i].destinoX = (int)mundoX;
            unidades[i].destinoY = (int)mundoY;
            // Opcional: limpiar targets previos
            unidades[i].targetIndex = -1; 
        }
    }
}