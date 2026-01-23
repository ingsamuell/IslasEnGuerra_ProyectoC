// Andrea Sanchez

#include "unidades.h"
#include "../mundo/mapa.h"       
#include "../mundo/naturaleza.h"
#include "../mundo/fauna.h"      
#include "../recursos/recursos.h"
#include "../jugador/jugador.h"
#include <math.h>
#include <stdio.h>

Unidad unidades[MAX_UNIDADES];
extern int screenShake; 
extern void crearBalaCanon(float x, float y, float destX, float destY);
extern void crearSangre(float x, float y);
extern void crearChispas(int x, int y, COLORREF color);
extern void crearTextoFlotante(int x, int y, const char* formato, int cantidad, COLORREF color);

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
            unidades[i].bando = BANDO_ALIADO;
            unidades[i].activa = 1;
            unidades[i].estado = ESTADO_IDLE;
            
            // --- CAMBIO: VIDA SEGÚN TIPO ---
            if (tipo == TIPO_SOLDADO) {
                unidades[i].vidaMax = 40; 
                unidades[i].vida = 40;
            } else {
                unidades[i].vidaMax = 20; // Aldeanos normales
                unidades[i].vida = 20;
            }

            strcpy(unidades[i].nombreGrupo, nombre);
            creados++;
        }
    }
}

// Dibuja un efecto de "corte" o "golpe" frente al personaje
void dibujarEfectoAtaque(HDC hdc, int x, int y, int direccion, int tam) {
    // Configuración del lápiz: Blanco brillante y grueso
    HPEN hPen = CreatePen(PS_SOLID, 4, RGB(255, 255, 200)); 
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    
    // Calcular el punto central del golpe según la dirección
    int centroX = x + (tam / 2);
    int centroY = y + (tam / 2);
    int alcance = tam; // Qué tan lejos llega el golpe

    int inicioX, inicioY, finX, finY;

    // Coordenadas para simular un arco o tajo
    switch (direccion) {
        case DIR_DERECHA:
            inicioX = centroX + 10; inicioY = centroY - 10;
            finX = centroX + alcance; finY = centroY + 10;
            break;
        case DIR_IZQUIERDA:
            inicioX = centroX - 10; inicioY = centroY - 10;
            finX = centroX - alcance; finY = centroY + 10;
            break;
        case DIR_ABAJO:
            inicioX = centroX + 10; inicioY = centroY + 10;
            finX = centroX - 10; finY = centroY + alcance;
            break;
        case DIR_ARRIBA:
            inicioX = centroX - 10; inicioY = centroY - 10;
            finX = centroX + 10; finY = centroY - alcance;
            break;
    }

    // Dibujar la línea de ataque
    MoveToEx(hdc, inicioX, inicioY, NULL);
    LineTo(hdc, finX, finY);

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}
void aplicarSeparacion(int id, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    float sepX = 0, sepY = 0;
    int vecinos = 0;
    float radioPersonal = 25.0f; // Distancia mínima entre aldeanos

    for (int j = 0; j < MAX_UNIDADES; j++) {
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
        // El factor 1.5f controla qué tan fuerte se empujan 
        float nuevoX = unidades[id].x + (sepX * 1.5f);
        float nuevoY = unidades[id].y + (sepY * 1.5f);

        // IMPORTANTE: Solo empujamos si el destino es suelo
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
        if (unidades[i].bando == BANDO_ENEMIGO) continue; 

        // BARCO ALIADO
        if (unidades[i].tipo == TIPO_BARCO_ALIADO) {
            if (unidades[i].pasajeros > 0) {
                float distJ = sqrt(pow(j->x - unidades[i].x, 2) + pow(j->y - unidades[i].y, 2));
                if (distJ > 120) { 
                    float dx = (j->x - 60) - unidades[i].x;
                    float dy = j->y - unidades[i].y;
                    float dist = sqrt(dx*dx + dy*dy);
                    if(dist > 5.0f) {
                        unidades[i].x += (dx / dist) * 1.8f;
                        unidades[i].y += (dy / dist) * 1.8f;
                    }
                    actualizarAnimacionUnidad(&unidades[i], dx, dy);
                }
                if (unidades[i].timerAtaque > 0) unidades[i].timerAtaque--;
                if (unidades[i].timerAtaque == 0) {
                    for(int k=0; k<MAX_UNIDADES; k++) {
                        if(unidades[k].activa && unidades[k].bando == BANDO_ENEMIGO) {
                            float distE = sqrt(pow(unidades[k].x - unidades[i].x, 2) + pow(unidades[k].y - unidades[i].y, 2));
                            if(distE < 400) { 
                                crearBalaCanon(unidades[i].x, unidades[i].y, unidades[k].x, unidades[k].y);
                                int recarga = 180 - (unidades[i].pasajeros * 30);
                                unidades[i].timerAtaque = recarga; 
                                break; 
                            }
                        }
                    }
                }
            }
            continue;
        }

        aplicarSeparacion(i, mapa);

        float dx = unidades[i].destinoX - unidades[i].x;
        float dy = unidades[i].destinoY - unidades[i].y;
        float dist = sqrt(dx * dx + dy * dy);

        switch (unidades[i].estado) {
            case ESTADO_SUBIR_BARCO: {
                int idBarco = unidades[i].targetIndex;
                if (idBarco == -1 || !unidades[idBarco].activa) { unidades[i].estado = ESTADO_IDLE; break; }
                float dxB = unidades[idBarco].x - unidades[i].x;
                float dyB = unidades[idBarco].y - unidades[i].y;
                float distB = sqrt(dxB*dxB + dyB*dyB);
                if (distB > 30.0f) {
                    unidades[i].x += (dxB / distB) * 2.0f;
                    unidades[i].y += (dyB / distB) * 2.0f;
                    actualizarAnimacionUnidad(&unidades[i], dxB, dyB);
                } else {
                    if (unidades[idBarco].pasajeros < 3) {
                        unidades[idBarco].pasajeros++;
                        unidades[i].activa = 0; 
                        crearTextoFlotante(unidades[idBarco].x, unidades[idBarco].y, "+1 Tripulante", 0, RGB(0, 255, 0));
                    } else { unidades[i].estado = ESTADO_IDLE; }
                }
                break;
            }
            case ESTADO_PERSIGUIENDO: {
                int target = unidades[i].targetIndex;
                if (target == -1 || !unidades[target].activa) { unidades[i].estado = ESTADO_IDLE; break; }
                float dxE = unidades[target].x - unidades[i].x;
                float dyE = unidades[target].y - unidades[i].y;
                float distE = sqrt(dxE * dxE + dyE * dyE);
                if (distE > 40.0f) { 
                    float velocidad = (unidades[i].tipo == TIPO_SOLDADO) ? 1.6f : 1.1f;
                    float nuevoX = unidades[i].x + (dxE / distE) * velocidad;
                    float nuevoY = unidades[i].y + (dyE / distE) * velocidad;
                    BOOL pase = FALSE;
                    if (EsSuelo((int)nuevoX + 16, (int)nuevoY + 16, mapa)) pase = TRUE;
                    else if (unidades[i].tipo == TIPO_SOLDADO) pase = TRUE;
                    if (pase) { unidades[i].x = nuevoX; unidades[i].y = nuevoY; }
                    actualizarAnimacionUnidad(&unidades[i], dxE, dyE);
                } else { unidades[i].estado = ESTADO_ATACANDO; unidades[i].timerAtaque = 40; }
                break;
            }
            case ESTADO_ATACANDO: {
                int target = unidades[i].targetIndex;
                if (target == -1 || !unidades[target].activa) { unidades[i].estado = ESTADO_IDLE; break; }
                float dxE = unidades[target].x - unidades[i].x;
                float dyE = unidades[target].y - unidades[i].y;
                if (sqrt(dxE*dxE + dyE*dyE) > 55.0f) { unidades[i].estado = ESTADO_PERSIGUIENDO; break; }
                unidades[i].timerAtaque++;
                if (unidades[i].timerAtaque >= 60) { 
                    unidades[i].timerAtaque = 0;
                    int dano = (unidades[i].tipo == TIPO_SOLDADO) ? 15 : 5;
                    unidades[target].vida -= dano;
                    crearSangre(unidades[target].x, unidades[target].y); 
                    if (unidades[target].vida <= 0) {
                        unidades[target].activa = 0;
                        ganarExperiencia(j, 50);
                        crearTextoFlotante(unidades[i].x, unidades[i].y, "+50 XP", 0, RGB(0, 255, 255));
                        unidades[i].estado = ESTADO_IDLE;
                    }
                }
                break;
            }
            // --- LÓGICA DE TRABAJO---
            case ESTADO_TALANDO: {
                int a = unidades[i].targetIndex;
                if (a == -1 || !misArboles[a].activa) { 
                    int nuevo = buscarArbolCercano(unidades[i].x, unidades[i].y, 350.0f);
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
                    unidades[i].x += (dxT / distObjeto) * 2.2f;
                    unidades[i].y += (dyT / distObjeto) * 2.2f;
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
                        int next = buscarArbolCercano(unidades[i].x, unidades[i].y, 450.0f);
                        if(next != -1) unidades[i].targetIndex = next;
                        else unidades[i].estado = ESTADO_IDLE;
                    }
                }
                break;
            }

            case ESTADO_MINANDO: {
                int m = unidades[i].targetIndex;
                if (m == -1 || !misMinas[m].activa) { 
                    int nueva = buscarMinaCercana(unidades[i].x, unidades[i].y, 350.0f);
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
                    unidades[i].x += (dxM / distMina) * 2.2f;
                    unidades[i].y += (dyM / distMina) * 2.2f;
                    actualizarAnimacionUnidad(&unidades[i], dxM, dyM);
                } else {
                    unidades[i].timerTrabajo++;
                    if (unidades[i].timerTrabajo >= 250) { 
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
                        int next = buscarMinaCercana(unidades[i].x, unidades[i].y, 450.0f);
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
                    if (unidades[i].timerTrabajo >= 250) { 
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

void dibujarUnidades(HDC hdc, Camera cam, char mapa[MUNDO_FILAS][MUNDO_COLUMNAS]) {
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa) continue;

        int ux = (int)((unidades[i].x - cam.x) * cam.zoom);
        int uy = (int)((unidades[i].y - cam.y) * cam.zoom);
        int tam = (int)(TAMANO_CELDA * cam.zoom);

        // 1. DETECTAR TIERRA USANDO EsSuelo
        BOOL esTierra = EsSuelo((int)unidades[i].x + 16, (int)unidades[i].y + 16, mapa);

        BOOL enAgua = !esTierra; 

        HBITMAP hBmpActual = NULL;
        int dir = (unidades[i].direccion == DIR_IZQUIERDA) ? 0 : 1;

        // CASO 1: BARCO ALIADO 
        if (unidades[i].tipo == TIPO_BARCO_ALIADO) {
             hBmpActual = hBmpBarco[dir]; 
             tam = (int)(64 * cam.zoom);
             ux -= 15 * cam.zoom;             
             uy -= 15 * cam.zoom;
             // Texto de la cantidad de tripulación
             char buff[32];
             sprintf(buff, "(%d/3)", unidades[i].pasajeros);
             SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(0,255,255));
             TextOut(hdc, ux, uy-20, buff, strlen(buff));
        }
        
        // CASO 2: UNIDADES MILITARES (SOLDADOS, PIRATAS, MAGOS)
        else if (unidades[i].tipo == TIPO_SOLDADO || 
                 unidades[i].tipo == TIPO_ENEMIGO_PIRATA || 
                 unidades[i].tipo == TIPO_ENEMIGO_MAGO) {
            
            // SOLO si están REALMENTE en el agua -> BARCO
            if (enAgua) {
                hBmpActual = hBmpBarco[dir]; // Barco de Guerra
                tam = (int)(48 * cam.zoom);
                ux -= 10 * cam.zoom;
            } 
            // SI ESTÁN EN TIERRA -> SU SPRITE ORIGINAL
            else {
                if (unidades[i].tipo == TIPO_SOLDADO) {
                    hBmpActual = hBmpSoldadoAnim[unidades[i].direccion][unidades[i].frameAnim];
                } else if (unidades[i].tipo == TIPO_ENEMIGO_PIRATA) {
                    hBmpActual = hBmpPirataAnim[unidades[i].direccion][unidades[i].frameAnim];
                } else {
                    hBmpActual = hBmpMagoAnim[unidades[i].direccion][unidades[i].frameAnim];
                }
            }
        }

        // CASO 3: ENEMIGOS (Piratas en Tierra, Barcos en Agua)
        else if (unidades[i].bando == BANDO_ENEMIGO) {
            if (enAgua) {
                hBmpActual = hBmpBarco[dir]; // Barco Pirata
                tam = (int)(48 * cam.zoom);
                ux -= 10 * cam.zoom;
            } else {
                if(unidades[i].tipo == TIPO_ENEMIGO_PIRATA)
                    hBmpActual = hBmpPirataAnim[unidades[i].direccion][unidades[i].frameAnim];
                else
                    hBmpActual = hBmpMagoAnim[unidades[i].direccion][unidades[i].frameAnim];
            }
        }

        // CASO 4: ALDEANOS (Mineros, Leñadores, Cazadores)
        else {
            // NUNCA USAN BOTE, SIEMPRE SU SPRITE
            switch (unidades[i].tipo) {
                case TIPO_MINERO: hBmpActual = hBmpMineroAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
                case TIPO_LENADOR: hBmpActual = hBmpLenadorAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
                case TIPO_CAZADOR: hBmpActual = hBmpCazadorAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
                default: hBmpActual = hBmpJugadorAnim[unidades[i].direccion][unidades[i].frameAnim]; break;
            }
        }

        // --- DIBUJADO ---
        if (hBmpActual) {
            DibujarImagen(hdc, hBmpActual, ux, uy, tam, tam);
        }

        // --- SELECCIÓN ---
        if (unidades[i].seleccionado) {
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
            HGDIOBJ hOld = SelectObject(hdc, hPen);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Ellipse(hdc, ux, uy + tam - (int)(10 * cam.zoom), ux + tam, uy + tam);
            SelectObject(hdc, hOld);
            DeleteObject(hPen);
        }
        // --- BARRA DE VIDA ---
        int maxVida = (unidades[i].vidaMax > 0) ? unidades[i].vidaMax : 100;
        dibujarBarraVidaLocal(hdc, ux, uy - 10, unidades[i].vida, maxVida, 32 * cam.zoom);

        // --- EFECTO VISUAL DE ATAQUE ---
        if (unidades[i].estado == ESTADO_ATACANDO && unidades[i].timerAtaque > 30 && unidades[i].timerAtaque < 50) {
             HPEN hPenAtk = CreatePen(PS_SOLID, 3, RGB(255, 255, 200));
             HGDIOBJ oldPen = SelectObject(hdc, hPenAtk);
             MoveToEx(hdc, ux + tam/2, uy + tam/2, NULL);
             if(unidades[i].direccion == DIR_DERECHA) LineTo(hdc, ux + tam, uy + tam/2);
             else if(unidades[i].direccion == DIR_IZQUIERDA) LineTo(hdc, ux, uy + tam/2);
             else LineTo(hdc, ux + tam/2, uy + tam);
             SelectObject(hdc, oldPen);
             DeleteObject(hPenAtk);
        }

        // --- BARRA DE PROGRESO (TRABAJO) ---
        if (unidades[i].estado == ESTADO_TALANDO || unidades[i].estado == ESTADO_MINANDO || unidades[i].estado == ESTADO_CAZANDO) {
            int limite = 150; // Ajustado a 2.5 segundos
            if (limite > 0) {
                int barraAncho = (int)(20 * cam.zoom);
                int barraAlto = (int)(4 * cam.zoom);
                int bx = ux + (tam / 2) - (barraAncho / 2);
                int by = uy - (int)(12 * cam.zoom);

                HBRUSH hBrushBg = CreateSolidBrush(RGB(50, 50, 50));
                RECT rectBg = {bx, by, bx + barraAncho, by + barraAlto};
                FillRect(hdc, &rectBg, hBrushBg);
                DeleteObject(hBrushBg);

                float progreso = (float)unidades[i].timerTrabajo / limite;
                if (progreso > 1.0f) progreso = 1.0f;
                int anchoActual = (int)(barraAncho * progreso);

                if (anchoActual > 0) {
                    HBRUSH hBrushFill = CreateSolidBrush(RGB(255, 215, 0));
                    RECT rectFill = {bx, by, bx + anchoActual, by + barraAlto};
                    FillRect(hdc, &rectFill, hBrushFill);
                    DeleteObject(hBrushFill);
                }
            }
        }
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

    // 1. DETECTAR ENEMIGO O BARCO ALIADO
    int enemigoID = -1;
    int barcoAliadoID = -1;

    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa) continue;
        
        float dx = unidades[i].x - mundoX;
        float dy = unidades[i].y - mundoY;
        float dist = sqrt(dx*dx + dy*dy);

        if (dist < 40) {
            if (unidades[i].bando == BANDO_ENEMIGO) {
                enemigoID = i;
                break; 
            }
            // DETECTAR BARCO ALIADO PARA ABORDAR
            else if (unidades[i].tipo == TIPO_BARCO_ALIADO) {
                barcoAliadoID = i;
                break;
            }
        }
    }

    // 2. ASIGNAR ÓRDENES
    for (int i = 0; i < MAX_UNIDADES; i++) {
        if (!unidades[i].activa || !unidades[i].seleccionado) continue;
        if (unidades[i].bando == BANDO_ENEMIGO || unidades[i].tipo == TIPO_BARCO_ALIADO) continue; 

        unidades[i].timerTrabajo = 0;
        bool accionAsignada = false;

        // A) SUBIR AL BARCO (Solo soldados)
        if (barcoAliadoID != -1 && unidades[i].tipo == TIPO_SOLDADO) {
            if (unidades[barcoAliadoID].pasajeros < 3) {
                unidades[i].estado = ESTADO_SUBIR_BARCO;
                unidades[i].targetIndex = barcoAliadoID;
                crearTextoFlotante(unidades[i].x, unidades[i].y, "A bordar!", 0, RGB(0, 255, 255));
                accionAsignada = true;
            } else {
                crearTextoFlotante(unidades[i].x, unidades[i].y, "Barco Lleno", 0, RGB(255, 50, 50));
            }
        }

        // B) ATAQUE
        if (!accionAsignada && enemigoID != -1 && unidades[i].tipo == TIPO_SOLDADO) {
            unidades[i].estado = ESTADO_PERSIGUIENDO;
            unidades[i].targetIndex = enemigoID;
            crearTextoFlotante(unidades[i].x, unidades[i].y, "Al ataque!", 0, RGB(255, 0, 0));
            accionAsignada = true;
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

        // PRIORIDAD C: MOVIMIENTO SIMPLE 
        if (!accionAsignada) {
            unidades[i].estado = ESTADO_MOVIENDO;
            unidades[i].destinoX = (int)mundoX;
            unidades[i].destinoY = (int)mundoY;
            unidades[i].targetIndex = -1; 
        }
    }
}
// Función para que el JUGADOR pegue con la Espada 
void golpearEnemigoCercano(Jugador *j) {
    int rango = 60; // Distancia de la espada
    BOOL golpeoAlguien = FALSE;

    for (int i = 0; i < MAX_UNIDADES; i++) {
        // Solo atacar enemigos vivos
        if (!unidades[i].activa || unidades[i].bando != BANDO_ENEMIGO) continue;

        // Calcular distancia
        float dx = unidades[i].x - j->x;
        float dy = unidades[i].y - j->y;
        float dist = sqrt(dx*dx + dy*dy);

        if (dist < rango) {
            // CALCULAR DAÑO DEL JUGADOR
            int danoBase = 25; // Puño
            if (j->tieneEspada && j->herramientaActiva == HERRAMIENTA_ESPADA) danoBase = 50; // Espada
            
            // APLICAR DAÑO
            unidades[i].vida -= danoBase;
            golpeoAlguien = TRUE;

            // EFECTOS VISUALES
            crearSangre(unidades[i].x, unidades[i].y);
            crearTextoFlotante(unidades[i].x, unidades[i].y - 20, "-HIT!", 0, RGB(255, 0, 0));
            
            // EMPUJE para que se sienta el golpe
            float fuerza = 15.0f;
            unidades[i].x += (dx / dist) * fuerza;
            unidades[i].y += (dy / dist) * fuerza;

            // MATAR ENEMIGO
            if (unidades[i].vida <= 0) {
                unidades[i].activa = 0;
                ganarExperiencia(j, 100); // Mucha XP por matar tú mismo
                crearTextoFlotante(j->x, j->y, "ENEMIGO CAIDO!", 0, RGB(0, 255, 255));
            }
        }
    }

    if (golpeoAlguien) {
       
    }
}