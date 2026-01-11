/* src/recursos/recursos.c - VERSIÓN FINAL CORREGIDA */
#include "recursos.h"
#include <stdio.h>

// --- VARIABLES GLOBALES ---
HBITMAP hBmpFondoMenu = NULL;

// UI
HBITMAP hBmpBtnJugar = NULL;
HBITMAP hBmpBtnPartidas = NULL;
HBITMAP hBmpBtnInstrucciones = NULL;
HBITMAP hBmpBtnSalir = NULL;
HBITMAP hBmpTitulo = NULL;
HBITMAP hBmpBoton = NULL;
HBITMAP hBmpBotonSel = NULL;
HBITMAP hBmpInvCerrado = NULL;
HBITMAP hBmpInvAbierto = NULL;

// SELECCIÓN DE MAPA
HBITMAP hBmpFondoSeleccionMapa = NULL;
HBITMAP hBmpCuadroMapa1Normal = NULL;
HBITMAP hBmpCuadroMapa2Normal = NULL;
HBITMAP hBmpCuadroMapa3Normal = NULL;
HBITMAP hBmpCuadroMapa1Sel = NULL;
HBITMAP hBmpCuadroMapa2Sel = NULL;
HBITMAP hBmpCuadroMapa3Sel = NULL;

// MAPAS
HBITMAP hBmpIslaGrande = NULL;
HBITMAP hBmpIslaSec1 = NULL;
HBITMAP hBmpIslaSec2 = NULL;
HBITMAP hBmpIslaSec3 = NULL;
HBITMAP hBmpIslaSec4 = NULL;
HBITMAP hBmpMuelle = NULL;

// JUGADOR BASE
HBITMAP hBmpJugador = NULL;
HBITMAP hBmpJugadorAnim[4][3]; // Base

// JUGADOR HERRAMIENTAS (NUEVAS VARIABLES AGREGADAS)
HBITMAP hBmpArmaduraAnim[4][3] = {{NULL}};
HBITMAP hBmpEspadaAnim[4][3] = {{NULL}};
HBITMAP hBmpPicoAnim[4][3] = {{NULL}};
HBITMAP hBmpHachaAnim[4][3] = {{NULL}};

// NPCs
HBITMAP hBmpMineroAnim[4][3];
HBITMAP hBmpLenadorAnim[4][3];
HBITMAP hBmpCazadorAnim[4][3];
HBITMAP hBmpSoldadoAnim[4][3];
HBITMAP hBmpPirataAnim[4][3];
HBITMAP hBmpMagoAnim[4][3];

// Tienda y Varios
HBITMAP hBmpTienda[2] = {NULL, NULL};
HBITMAP hBmpIconoPescado = NULL;
HBITMAP hBmpIconoCana = NULL;
HBITMAP hBmpBote[2] = {NULL, NULL};
HBITMAP hBmpBarco[2] = {NULL, NULL};

// Objetos Mundo
HBITMAP hBmpCastilloJugador = NULL;
HBITMAP hBmpRoca = NULL;
HBITMAP hBmpTesoroCerrado = NULL;
HBITMAP hBmpTesoroOro = NULL;
HBITMAP hBmpTesoroJoyas = NULL;
HBITMAP hBmpTesoroVacio = NULL;
HBITMAP hBmpMina = NULL;
HBITMAP hBmpEstablo = NULL;
HBITMAP hBmpHierroPicar = NULL;
HBITMAP hBmpPiedraPicar = NULL;

// --- DEFINICIÓN DE EDIFICIOS ---
HBITMAP hBmpEdificioPeq[3] = {NULL, NULL, NULL};
HBITMAP hBmpEdificioPeqEnemigo[3] = {NULL, NULL, NULL};
HBITMAP hBmpEdificioMed[3] = {NULL, NULL, NULL};
HBITMAP hBmpEdificioMedEnemigo[3] = {NULL, NULL, NULL};
HBITMAP hBmpEdificioGrande[3] = {NULL, NULL, NULL};
HBITMAP hBmpEdificioGrandeEnemigo[3] = {NULL, NULL, NULL};
HBITMAP hBmpBanderaEnemiga = NULL;

// Mapas Adicionales
HBITMAP hBmpIslaGrandeMapa2 = NULL;
HBITMAP hBmpIslaSec1Mapa2 = NULL;
HBITMAP hBmpIslaSec2Mapa2 = NULL;
HBITMAP hBmpIslaSec3Mapa2 = NULL;
HBITMAP hBmpIslaSec4Mapa2 = NULL;
HBITMAP hBmpArbolChicoMapa2 = NULL;
HBITMAP hBmpArbolGrandeMapa2 = NULL;

HBITMAP hBmpIslaGrandeMapa3 = NULL;
HBITMAP hBmpIslaSec1Mapa3 = NULL;
HBITMAP hBmpIslaSec2Mapa3 = NULL;
HBITMAP hBmpIslaSec3Mapa3 = NULL;
HBITMAP hBmpIslaSec4Mapa3 = NULL;
HBITMAP hBmpArbolChicoMapa3 = NULL;
HBITMAP hBmpArbolGrandeMapa3 = NULL;

HBITMAP hBmpArbolChico = NULL;
HBITMAP hBmpArbolGrande = NULL;
HBITMAP hBmpVaca[8] = {NULL};
HBITMAP hBmpVacaMuerta = NULL;
HBITMAP hBmpTiburonAnim[2][4] = { {NULL} };

// ====================================================
// NUEVAS VARIABLES: ISLAS ENEMIGAS
// ====================================================

// Mapa 1
HBITMAP hBmpIslaEnemigaGrandeMapa1 = NULL;
HBITMAP hBmpIslaEnemigaSec1Mapa1 = NULL;
HBITMAP hBmpIslaEnemigaSec2Mapa1 = NULL;

// Mapa 2
HBITMAP hBmpIslaEnemigaGrandeMapa2 = NULL;
HBITMAP hBmpIslaEnemigaSec1Mapa2 = NULL;
HBITMAP hBmpIslaEnemigaSec2Mapa2 = NULL;

// Mapa 3
HBITMAP hBmpIslaEnemigaGrandeMapa3 = NULL;
HBITMAP hBmpIslaEnemigaSec1Mapa3 = NULL;
HBITMAP hBmpIslaEnemigaSec2Mapa3 = NULL;

// Items (Iconos)
HBITMAP hBmpIconoMadera = NULL;
HBITMAP hBmpIconoPiedra = NULL;
HBITMAP hBmpIconoOro = NULL;
HBITMAP hBmpIconoHierro = NULL;
HBITMAP hBmpIconoComida = NULL;
HBITMAP hBmpIconoHoja = NULL;
HBITMAP hBmpIconoEspada = NULL;
HBITMAP hBmpIconoPico = NULL;
HBITMAP hBmpIconoArmaduraInv = NULL;
HBITMAP hBmpIconoHacha = NULL;
HBITMAP hBmpIconoSoldadoInv = NULL; // (Si lo usas)
HBITMAP hBmpIconoPuno = NULL;

// HUD
HBITMAP hBmpCorazon100 = NULL;
HBITMAP hBmpCorazon75 = NULL;
HBITMAP hBmpCorazon50 = NULL;
HBITMAP hBmpCorazon25 = NULL;
HBITMAP hBmpCorazon0 = NULL;
HBITMAP hBmpEscudo100 = NULL;
HBITMAP hBmpEscudo75 = NULL;
HBITMAP hBmpEscudo50 = NULL;
HBITMAP hBmpEscudo25 = NULL;
HBITMAP hBmpEscudo0 = NULL;
HBITMAP hBmpBarraXPVacia = NULL;
HBITMAP hBmpBarraXPLlena = NULL;

// --- FUNCIONES ---

HBITMAP CargarImagen(const char *ruta)
{
    HBITMAP hBmp = (HBITMAP)LoadImage(NULL, ruta, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (hBmp == NULL)
        printf("ERROR: No se encontro %s\n", ruta);
    return hBmp;
}

void CargarSetEnemigo(HBITMAP destino[4][3], char *prefijo) {
    char *nombresDir[4] = {"base", "espalda", "izquierda", "derecha"}; // Orden: Abajo, Arriba, Izq, Der
    char ruta[128];

    for (int d = 0; d < 4; d++) {
        // Frame 0: Quieto (ej. "Mago-base.bmp")
        sprintf(ruta, "assets/enemigos/%s-%s.bmp", prefijo, nombresDir[d]);
        destino[d][0] = CargarImagen(ruta);

        // Frame 1: Pie derecho (ej. "Mago-base-1.bmp")
        sprintf(ruta, "assets/enemigos/%s-%s-1.bmp", prefijo, nombresDir[d]);
        destino[d][1] = CargarImagen(ruta);

        // Frame 2: Pie izquierdo (ej. "Mago-base-2.bmp")
        sprintf(ruta, "assets/enemigos/%s-%s-2.bmp", prefijo, nombresDir[d]);
        destino[d][2] = CargarImagen(ruta);
    }
}

// Carga automática para NPCs (y Jugador con herramientas si sigue patrón)
void CargarSetUnidad(HBITMAP destino[4][3], char *herramienta)
{
    char *direcciones[4] = {"base", "espalda", "izquierda", "derecha"}; // 0, 1, 2, 3
    char ruta[128];

    for (int dir = 0; dir < 4; dir++)
    {
        for (int frame = 0; frame < 3; frame++)
        {
            // Lógica de nombres: Personaje-base-pico.bmp / Personaje-base-1-pico.bmp
            if (frame == 0)
            {
                sprintf(ruta, "assets/unidades/Personaje-%s-%s.bmp", direcciones[dir], herramienta);
            }
            else
            {
                sprintf(ruta, "assets/unidades/Personaje-%s-%d-%s.bmp", direcciones[dir], frame, herramienta);
            }
            destino[dir][frame] = CargarImagen(ruta);
        }
    }
}

void CargarRecursos()
{
	
    // 1. MENÚ
    hBmpFondoMenu = CargarImagen("assets/ui/fondo_menu.bmp");
    hBmpBtnJugar = CargarImagen("assets/ui/btn_jugar.bmp");
    hBmpBtnPartidas = CargarImagen("assets/ui/btn_partidas.bmp");
    hBmpBtnInstrucciones = CargarImagen("assets/ui/btn_instrucciones.bmp");
    hBmpBtnSalir = CargarImagen("assets/ui/btn_salir.bmp");
    hBmpInvCerrado = CargarImagen("assets/ui/boton_inv_cerrado.bmp");
    hBmpInvAbierto = CargarImagen("assets/ui/boton_inv_abierto.bmp");

    // 2. SELECCIÓN DE MAPA
    hBmpFondoSeleccionMapa = CargarImagen("assets/ui/fondo_seleccion_mapa.bmp");
    hBmpCuadroMapa1Normal = CargarImagen("assets/ui/cuadro_mapa1_normal.bmp");
    hBmpCuadroMapa2Normal = CargarImagen("assets/ui/cuadro_mapa2_normal.bmp");
    hBmpCuadroMapa3Normal = CargarImagen("assets/ui/cuadro_mapa3_normal.bmp");
    hBmpCuadroMapa1Sel = CargarImagen("assets/ui/cuadro_mapa1_sel.bmp");
    hBmpCuadroMapa2Sel = CargarImagen("assets/ui/cuadro_mapa2_sel.bmp");
    hBmpCuadroMapa3Sel = CargarImagen("assets/ui/cuadro_mapa3_sel.bmp");

    // 3. MUNDO Y OBJETOS
    hBmpArbolChico = CargarImagen("assets/mundo/Arbol_islaUno.bmp");
    hBmpArbolGrande = CargarImagen("assets/mundo/Arbolgrande_islaUno.bmp");

    // --- CARGAR EDIFICIOS (PEQUEÑOS) ---
    hBmpEdificioPeq[0] = CargarImagen("./assets/edificios/EdificioPeq_mapa1.bmp");
    hBmpEdificioPeq[1] = CargarImagen("./assets/edificios/EdificioPeq_mapa2.bmp");
    hBmpEdificioPeq[2] = CargarImagen("./assets/edificios/EdificioPeq_mapa3.bmp");

    hBmpEdificioPeqEnemigo[0] = CargarImagen("./assets/edificios/EdificioPeq_enemigo_mapa1.bmp");
    hBmpEdificioPeqEnemigo[1] = CargarImagen("./assets/edificios/EdificioPeq_enemigo_mapa2.bmp");
    hBmpEdificioPeqEnemigo[2] = CargarImagen("./assets/edificios/EdificioPeq_enemigo_mapa3.bmp");

    // --- CARGAR EDIFICIOS (MEDIANOS) ---
    hBmpEdificioMed[0] = CargarImagen("./assets/edificios/EdificioMed_mapa1.bmp");
    hBmpEdificioMed[1] = CargarImagen("./assets/edificios/EdificioMed_mapa2.bmp");
    hBmpEdificioMed[2] = CargarImagen("./assets/edificios/EdificioMed_mapa3.bmp");

    hBmpEdificioMedEnemigo[0] = CargarImagen("./assets/edificios/EdificioMed_enemigo_mapa1.bmp");
    hBmpEdificioMedEnemigo[1] = CargarImagen("./assets/edificios/EdificioMed_enemigo_mapa2.bmp");
    hBmpEdificioMedEnemigo[2] = CargarImagen("./assets/edificios/EdificioMed_enemigo_mapa3.bmp");

    // --- CARGAR EDIFICIOS (GRANDES) ---
    hBmpEdificioGrande[0] = CargarImagen("./assets/edificios/EdificioGrande_mapa1.bmp");
    hBmpEdificioGrande[1] = CargarImagen("./assets/edificios/EdificioGrande_mapa2.bmp");
    hBmpEdificioGrande[2] = CargarImagen("./assets/edificios/EdificioGrande_mapa3.bmp");

    hBmpEdificioGrandeEnemigo[0] = CargarImagen("./assets/edificios/EdificioGrande_enemigo_mapa1.bmp");
    hBmpEdificioGrandeEnemigo[1] = CargarImagen("./assets/edificios/EdificioGrande_enemigo_mapa2.bmp");
    hBmpEdificioGrandeEnemigo[2] = CargarImagen("./assets/edificios/EdificioGrande_enemigo_mapa3.bmp");

    // --- CARGAR BANDERA ---
    hBmpBanderaEnemiga = CargarImagen("./assets/edificios/Bandera_islaEnemiga.bmp");

    // Vacas
    hBmpVaca[0] = CargarImagen("assets/animales/vaca-0.bmp");
    hBmpVaca[1] = CargarImagen("assets/animales/vaca-1.bmp");
    hBmpVaca[2] = CargarImagen("assets/animales/vaca-2.bmp");
    hBmpVaca[3] = CargarImagen("assets/animales/vaca-3.bmp");
    hBmpVaca[4] = CargarImagen("assets/animales/vaca-4.bmp");
    hBmpVaca[5] = CargarImagen("assets/animales/vaca-5.bmp");
    hBmpVaca[6] = CargarImagen("assets/animales/vaca-6.bmp");
    hBmpVaca[7] = CargarImagen("assets/animales/vaca-7.bmp");
    hBmpVacaMuerta = CargarImagen("assets/animales/vaca_muerta.bmp");
    hBmpEstablo = CargarImagen("assets/animales/ranchito_vacas.bmp");
    
    //Tiburones
    hBmpTiburonAnim[0][0] = CargarImagen("assets/animales/Tiburon-0-2.bmp"); // Saliendo
	hBmpTiburonAnim[0][1] = CargarImagen("assets/animales/Tiburon-1-2.bmp"); // Aire alto
	hBmpTiburonAnim[0][2] = CargarImagen("assets/animales/Tiburon-2-2.bmp"); // Aire bajo
	hBmpTiburonAnim[0][3] = CargarImagen("assets/animales/Tiburon-3-2.bmp"); // Entrando
	hBmpTiburonAnim[1][0] = CargarImagen("assets/animales/Tiburon-0-1.bmp");
	hBmpTiburonAnim[1][1] = CargarImagen("assets/animales/Tiburon-1-1.bmp");
	hBmpTiburonAnim[1][2] = CargarImagen("assets/animales/Tiburon-2-1.bmp");
	hBmpTiburonAnim[1][3] = CargarImagen("assets/animales/Tiburon-3-1.bmp");

    // Mapas
    hBmpIslaGrande = CargarImagen("assets/mundo/Isla_grande.bmp");
    hBmpIslaSec1 = CargarImagen("assets/mundo/islasecundaria_UNO.bmp");
    hBmpIslaSec2 = CargarImagen("assets/mundo/islasecundaria_DOS.bmp");
    hBmpIslaSec3 = CargarImagen("assets/mundo/islasecundaria_TRES.bmp");
    hBmpIslaSec4 = CargarImagen("assets/mundo/islasecundaria_CUATRO.bmp");
    hBmpTienda[0] = CargarImagen("assets/mundo/tienda-de-ventas-pixilart.bmp");
    hBmpTienda[1] = CargarImagen("assets/mundo/tienda-de-ventas-pixilart-movimiento.bmp");
    hBmpHierroPicar = CargarImagen("assets/mundo/Hierro-picar.bmp");
    hBmpPiedraPicar = CargarImagen("assets/mundo/Piedra-picar.bmp");
    hBmpMina = CargarImagen("assets/mundo/Mina-cueva.bmp");

    // Mapa 2 - ARCHIPIELAGO (SI EXISTEN)
    hBmpIslaGrandeMapa2 = CargarImagen("assets/mundo/Isla_grande_mapa2.bmp");
    hBmpIslaSec1Mapa2 = CargarImagen("assets/mundo/islasecundaria_UNO_mapa2.bmp");
    hBmpIslaSec2Mapa2 = CargarImagen("assets/mundo/islasecundaria_DOS_mapa2.bmp");
    hBmpIslaSec3Mapa2 = CargarImagen("assets/mundo/islasecundaria_TRES_mapa2.bmp");
    hBmpIslaSec4Mapa2 = CargarImagen("assets/mundo/islasecundaria_CUATRO_mapa2.bmp");

    // Mapa 3 - CONTINENTE (SI EXISTEN)
    hBmpIslaGrandeMapa3 = CargarImagen("assets/mundo/Isla_grande_mapa3.bmp");
    hBmpIslaSec1Mapa3 = CargarImagen("assets/mundo/islasecundaria_UNO_mapa3.bmp");
    hBmpIslaSec2Mapa3 = CargarImagen("assets/mundo/islasecundaria_DOS_mapa3.bmp");
    hBmpIslaSec3Mapa3 = CargarImagen("assets/mundo/islasecundaria_TRES_mapa3.bmp");
    hBmpIslaSec4Mapa3 = CargarImagen("assets/mundo/islasecundaria_CUATRO_mapa3.bmp");

    hBmpArbolChicoMapa2 = CargarImagen("assets/mundo/Arbol_islaUno_mapa2.bmp");
    hBmpArbolGrandeMapa2 = CargarImagen("assets/mundo/Arbolgrande_islaUno_mapa2.bmp");
    hBmpArbolChicoMapa3 = CargarImagen("assets/mundo/Arbol_islaUno_mapa3.bmp");
    hBmpArbolGrandeMapa3 = CargarImagen("assets/mundo/Arbolgrande_islaUno_mapa3.bmp");

// ====================================================
// CARGAR ISLAS ENEMIGAS (3 por mapa × 3 mapas = 9 imágenes)
// ====================================================

// Mapa 1 (ISLAS DEL MAR)
hBmpIslaEnemigaGrandeMapa1 = CargarImagen("assets/mundo/Isla_enemiga_grande_mapa1.bmp");
hBmpIslaEnemigaSec1Mapa1 = CargarImagen("assets/mundo/islasecundaria_enemiga1_mapa1.bmp");
hBmpIslaEnemigaSec2Mapa1 = CargarImagen("assets/mundo/islasecundaria_enemiga2_mapa1.bmp");

// Mapa 2 (ARCHIPIÉLAGO)
hBmpIslaEnemigaGrandeMapa2 = CargarImagen("assets/mundo/Isla_enemiga_grande_mapa2.bmp");
hBmpIslaEnemigaSec1Mapa2 = CargarImagen("assets/mundo/islasecundaria_enemiga1_mapa2.bmp");
hBmpIslaEnemigaSec2Mapa2 = CargarImagen("assets/mundo/islasecundaria_enemiga2_mapa2.bmp");

// Mapa 3 (CONTINENTE)
hBmpIslaEnemigaGrandeMapa3 = CargarImagen("assets/mundo/Isla_enemiga_grande_mapa3.bmp");
hBmpIslaEnemigaSec1Mapa3 = CargarImagen("assets/mundo/islasecundaria_enemiga1_mapa3.bmp");
hBmpIslaEnemigaSec2Mapa3 = CargarImagen("assets/mundo/islasecundaria_enemiga2_mapa3.bmp");

    // Tesoros
    hBmpTesoroCerrado = CargarImagen("assets/mundo/tesoro-pixilart.bmp");
    hBmpTesoroOro = CargarImagen("assets/mundo/tesoro-abierto-con-oro-pixilart.bmp");
    hBmpTesoroJoyas = CargarImagen("assets/mundo/tesoro-abierto-con-piedras-preciosas-pixilart.bmp");
    hBmpTesoroVacio = CargarImagen("assets/mundo/tesoro-abierto-vacio-pixilart.bmp");

    // 4. ANIMACIONES DEL JUGADOR (EL GRAN CAMBIO)

    // A. Jugador Base (Sin herramientas) - Archivos "p_frente..."
    hBmpJugadorAnim[0][0] = CargarImagen("assets/jugador/p_frente_base.bmp");
    hBmpJugadorAnim[0][1] = CargarImagen("assets/jugador/p_frente_pie1.bmp");
    hBmpJugadorAnim[0][2] = CargarImagen("assets/jugador/p_frente_pie2.bmp");
    hBmpJugadorAnim[1][0] = CargarImagen("assets/jugador/p_esp_base.bmp");
    hBmpJugadorAnim[1][1] = CargarImagen("assets/jugador/p_esp_pie1.bmp");
    hBmpJugadorAnim[1][2] = CargarImagen("assets/jugador/p_esp_pie2.bmp");
    hBmpJugadorAnim[2][0] = CargarImagen("assets/jugador/p_izq_base.bmp");
    hBmpJugadorAnim[2][1] = CargarImagen("assets/jugador/p_izq_pie1.bmp");
    hBmpJugadorAnim[2][2] = CargarImagen("assets/jugador/p_izq_pie2.bmp");
    hBmpJugadorAnim[3][0] = CargarImagen("assets/jugador/p_der_base.bmp");
    hBmpJugadorAnim[3][1] = CargarImagen("assets/jugador/p_der_pie1.bmp");
    hBmpJugadorAnim[3][2] = CargarImagen("assets/jugador/p_der_pie2.bmp");
    hBmpJugador = CargarImagen("assets/jugador/jugador_compatible.bmp");

    // B. Jugador con Herramientas (Usamos CargarSetUnidad porque siguen el patrón de NPCs)
    // Asegúrate de que los archivos existan en "assets/unidades/"
    // Ejemplo: assets/unidades/Personaje-base-espada.bmp
    CargarSetUnidad(hBmpArmaduraAnim, "armadura");
    CargarSetUnidad(hBmpEspadaAnim, "espada");
    CargarSetUnidad(hBmpPicoAnim, "pico");
    CargarSetUnidad(hBmpHachaAnim, "hacha");

    // 5. NPCs
    CargarSetUnidad(hBmpMineroAnim, "pico");
    CargarSetUnidad(hBmpLenadorAnim, "hacha");
    CargarSetUnidad(hBmpCazadorAnim, "espada");
    CargarSetUnidad(hBmpSoldadoAnim, "armadura");

    // 6. ITEMS (ICONOS)
    hBmpIconoMadera = CargarImagen("assets/items/icono_madera.bmp");
    hBmpIconoPiedra = CargarImagen("assets/items/icono_piedra.bmp");
    hBmpIconoOro = CargarImagen("assets/items/icono_oro.bmp");
    hBmpIconoHierro = CargarImagen("assets/items/icono_hierro.bmp");
    hBmpIconoHoja = CargarImagen("assets/items/icono_hoja.bmp");
    hBmpIconoComida = CargarImagen("assets/items/icono_comida.bmp");
    hBmpIconoPescado = CargarImagen("assets/items/pescado-item-pixilart.bmp");
    hBmpIconoCana = CargarImagen("assets/items/cana-de-pescar-item-pixilart.bmp");
    hBmpIconoEspada = CargarImagen("assets/items/item_espada.bmp");
    hBmpIconoPico = CargarImagen("assets/items/item_pico.bmp");
    hBmpIconoHacha = CargarImagen("assets/items/Hacha.bmp");
    hBmpIconoArmaduraInv = CargarImagen("assets/items/icono_armadura_inv.bmp");
    hBmpIconoPuno = CargarImagen("assets/ui/icono_puno.bmp");

    // Barcos
    hBmpBote[0] = CargarImagen("assets/barco/Bote_izqui.bmp");
    hBmpBote[1] = CargarImagen("assets/barco/Bote_dere.bmp");
    hBmpBarco[0] = CargarImagen("assets/barco/Barco_izqui.bmp");
    hBmpBarco[1] = CargarImagen("assets/barco/Barco_dere.bmp");
    hBmpMuelle = CargarImagen("assets/barco/Puente_barco.bmp");

    // 7. HUD
    hBmpCorazon100 = CargarImagen("assets/ui/corazon_100.bmp");
    hBmpCorazon75 = CargarImagen("assets/ui/corazon_75.bmp");
    hBmpCorazon50 = CargarImagen("assets/ui/corazon_50.bmp");
    hBmpCorazon25 = CargarImagen("assets/ui/corazon_25.bmp");
    hBmpCorazon0 = CargarImagen("assets/ui/corazon_0.bmp");
    hBmpEscudo100 = CargarImagen("assets/ui/escudo_100.bmp");
    hBmpEscudo75 = CargarImagen("assets/ui/escudo_75.bmp");
    hBmpEscudo50 = CargarImagen("assets/ui/escudo_50.bmp");
    hBmpEscudo25 = CargarImagen("assets/ui/escudo_25.bmp");
    hBmpEscudo0 = CargarImagen("assets/ui/escudo_0.bmp");
    hBmpBarraXPVacia = CargarImagen("assets/ui/barra_xp_vacia.bmp");
    hBmpBarraXPLlena = CargarImagen("assets/ui/barra_xp_llena.bmp");
    // --- 8. CARGAR ENEMIGOS ---

	CargarSetEnemigo(hBmpMagoAnim, "Mago");
	CargarSetEnemigo(hBmpPirataAnim, "pirata");
    
}

void LiberarRecursos() {
    // --- 1. UI Y MENÚ ---
    if (hBmpFondoMenu) DeleteObject(hBmpFondoMenu);
    if (hBmpBtnJugar) DeleteObject(hBmpBtnJugar);
    if (hBmpBtnPartidas) DeleteObject(hBmpBtnPartidas);
    if (hBmpBtnInstrucciones) DeleteObject(hBmpBtnInstrucciones);
    if (hBmpBtnSalir) DeleteObject(hBmpBtnSalir);
    if (hBmpBoton) DeleteObject(hBmpBoton);
    if (hBmpBotonSel) DeleteObject(hBmpBotonSel);
    if (hBmpTitulo) DeleteObject(hBmpTitulo);

    // --- 2. SELECCIÓN DE MAPA ---
    if (hBmpFondoSeleccionMapa) DeleteObject(hBmpFondoSeleccionMapa);
    if (hBmpCuadroMapa1Normal) DeleteObject(hBmpCuadroMapa1Normal);
    if (hBmpCuadroMapa1Sel) DeleteObject(hBmpCuadroMapa1Sel);
    if (hBmpCuadroMapa2Normal) DeleteObject(hBmpCuadroMapa2Normal);
    if (hBmpCuadroMapa2Sel) DeleteObject(hBmpCuadroMapa2Sel);
    if (hBmpCuadroMapa3Normal) DeleteObject(hBmpCuadroMapa3Normal);
    if (hBmpCuadroMapa3Sel) DeleteObject(hBmpCuadroMapa3Sel);

    // --- 3. HUD (INTERFAZ DE JUEGO) ---
    if (hBmpInvCerrado) DeleteObject(hBmpInvCerrado);
    if (hBmpInvAbierto) DeleteObject(hBmpInvAbierto);
    
    // Corazones
    if (hBmpCorazon100) DeleteObject(hBmpCorazon100);
    if (hBmpCorazon75) DeleteObject(hBmpCorazon75);
    if (hBmpCorazon50) DeleteObject(hBmpCorazon50);
    if (hBmpCorazon25) DeleteObject(hBmpCorazon25);
    if (hBmpCorazon0) DeleteObject(hBmpCorazon0);
    
    // Escudos
    if (hBmpEscudo100) DeleteObject(hBmpEscudo100);
    if (hBmpEscudo75) DeleteObject(hBmpEscudo75);
    if (hBmpEscudo50) DeleteObject(hBmpEscudo50);
    if (hBmpEscudo25) DeleteObject(hBmpEscudo25);
    if (hBmpEscudo0) DeleteObject(hBmpEscudo0);
    
    // Barras
    if (hBmpBarraXPVacia) DeleteObject(hBmpBarraXPVacia);
    if (hBmpBarraXPLlena) DeleteObject(hBmpBarraXPLlena);

    // --- 4. ITEMS (ICONOS) ---
    if (hBmpIconoMadera) DeleteObject(hBmpIconoMadera);
    if (hBmpIconoPiedra) DeleteObject(hBmpIconoPiedra);
    if (hBmpIconoOro) DeleteObject(hBmpIconoOro);
    if (hBmpIconoHierro) DeleteObject(hBmpIconoHierro);
    if (hBmpIconoComida) DeleteObject(hBmpIconoComida);
    if (hBmpIconoHoja) DeleteObject(hBmpIconoHoja);
    
    // Herramientas y Armas
    if (hBmpIconoEspada) DeleteObject(hBmpIconoEspada);
    if (hBmpIconoPico) DeleteObject(hBmpIconoPico);
    if (hBmpIconoHacha) DeleteObject(hBmpIconoHacha);
    if (hBmpIconoArmaduraInv) DeleteObject(hBmpIconoArmaduraInv);
    if (hBmpIconoSoldadoInv) DeleteObject(hBmpIconoSoldadoInv);
    if (hBmpIconoPescado) DeleteObject(hBmpIconoPescado);
    if (hBmpIconoCana) DeleteObject(hBmpIconoCana);
    if (hBmpIconoPuno) DeleteObject(hBmpIconoPuno); // <--- El nuevo puño

    // --- 5. MUNDO Y OBJETOS ---
    if (hBmpCastilloJugador) DeleteObject(hBmpCastilloJugador);
    if (hBmpRoca) DeleteObject(hBmpRoca);
    if (hBmpMina) DeleteObject(hBmpMina);
    if (hBmpEstablo) DeleteObject(hBmpEstablo);
    if (hBmpHierroPicar) DeleteObject(hBmpHierroPicar);
    if (hBmpPiedraPicar) DeleteObject(hBmpPiedraPicar);

    // Tesoros
    if (hBmpTesoroCerrado) DeleteObject(hBmpTesoroCerrado);
    if (hBmpTesoroOro) DeleteObject(hBmpTesoroOro);
    if (hBmpTesoroJoyas) DeleteObject(hBmpTesoroJoyas);
    if (hBmpTesoroVacio) DeleteObject(hBmpTesoroVacio);

    // Mapas (Islas)
    if (hBmpIslaGrande) DeleteObject(hBmpIslaGrande);
    if (hBmpIslaSec1) DeleteObject(hBmpIslaSec1);
    if (hBmpIslaSec2) DeleteObject(hBmpIslaSec2);
    if (hBmpIslaSec3) DeleteObject(hBmpIslaSec3);
    if (hBmpIslaSec4) DeleteObject(hBmpIslaSec4);
    
    // Mapa 2
    if (hBmpIslaGrandeMapa2) DeleteObject(hBmpIslaGrandeMapa2);
    if (hBmpIslaSec1Mapa2) DeleteObject(hBmpIslaSec1Mapa2);
    if (hBmpIslaSec2Mapa2) DeleteObject(hBmpIslaSec2Mapa2);
    if (hBmpIslaSec3Mapa2) DeleteObject(hBmpIslaSec3Mapa2);
    if (hBmpIslaSec4Mapa2) DeleteObject(hBmpIslaSec4Mapa2);

    // Mapa 3
    if (hBmpIslaGrandeMapa3) DeleteObject(hBmpIslaGrandeMapa3);
    if (hBmpIslaSec1Mapa3) DeleteObject(hBmpIslaSec1Mapa3);
    if (hBmpIslaSec2Mapa3) DeleteObject(hBmpIslaSec2Mapa3);
    if (hBmpIslaSec3Mapa3) DeleteObject(hBmpIslaSec3Mapa3);
    if (hBmpIslaSec4Mapa3) DeleteObject(hBmpIslaSec4Mapa3);

    // Árboles
    if (hBmpArbolChico) DeleteObject(hBmpArbolChico);
    if (hBmpArbolGrande) DeleteObject(hBmpArbolGrande);
    if (hBmpArbolChicoMapa2) DeleteObject(hBmpArbolChicoMapa2);
    if (hBmpArbolGrandeMapa2) DeleteObject(hBmpArbolGrandeMapa2);
    if (hBmpArbolChicoMapa3) DeleteObject(hBmpArbolChicoMapa3);
    if (hBmpArbolGrandeMapa3) DeleteObject(hBmpArbolGrandeMapa3);

     // --- 1. LIBERAR ISLAS ENEMIGAS (NUEVO) ---
    if (hBmpIslaEnemigaGrandeMapa1) DeleteObject(hBmpIslaEnemigaGrandeMapa1);
    if (hBmpIslaEnemigaSec1Mapa1) DeleteObject(hBmpIslaEnemigaSec1Mapa1);
    if (hBmpIslaEnemigaSec2Mapa1) DeleteObject(hBmpIslaEnemigaSec2Mapa1);
    
    if (hBmpIslaEnemigaGrandeMapa2) DeleteObject(hBmpIslaEnemigaGrandeMapa2);
    if (hBmpIslaEnemigaSec1Mapa2) DeleteObject(hBmpIslaEnemigaSec1Mapa2);
    if (hBmpIslaEnemigaSec2Mapa2) DeleteObject(hBmpIslaEnemigaSec2Mapa2);
    
    if (hBmpIslaEnemigaGrandeMapa3) DeleteObject(hBmpIslaEnemigaGrandeMapa3);
    if (hBmpIslaEnemigaSec1Mapa3) DeleteObject(hBmpIslaEnemigaSec1Mapa3);
    if (hBmpIslaEnemigaSec2Mapa3) DeleteObject(hBmpIslaEnemigaSec2Mapa3);

    // --- 6. ARRAYS DE ANIMACIÓN (JUGADOR Y NPCs) ---
    if (hBmpJugador) DeleteObject(hBmpJugador); // Imagen estática

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            // Jugador y sus variantes
            if (hBmpJugadorAnim[i][j]) DeleteObject(hBmpJugadorAnim[i][j]);
            if (hBmpArmaduraAnim[i][j]) DeleteObject(hBmpArmaduraAnim[i][j]);
            if (hBmpEspadaAnim[i][j]) DeleteObject(hBmpEspadaAnim[i][j]);
            if (hBmpPicoAnim[i][j]) DeleteObject(hBmpPicoAnim[i][j]);
            if (hBmpHachaAnim[i][j]) DeleteObject(hBmpHachaAnim[i][j]);

            // NPCs
            if (hBmpMineroAnim[i][j]) DeleteObject(hBmpMineroAnim[i][j]);
            if (hBmpLenadorAnim[i][j]) DeleteObject(hBmpLenadorAnim[i][j]);
            if (hBmpCazadorAnim[i][j]) DeleteObject(hBmpCazadorAnim[i][j]);
            if (hBmpSoldadoAnim[i][j]) DeleteObject(hBmpSoldadoAnim[i][j]);
        }
    }

    // --- 7. OTROS ARRAYS Y ENTIDADES ---
    
    // Vacas
    for (int i = 0; i < 8; i++) {
        if (hBmpVaca[i]) DeleteObject(hBmpVaca[i]);
    }
    if (hBmpVacaMuerta) DeleteObject(hBmpVacaMuerta);

	for (int d = 0; d < 2; d++) {
    for (int f = 0; f < 4; f++) {
         if (hBmpTiburonAnim[d][f]) DeleteObject(hBmpTiburonAnim[d][f]);
    }
}
    // Tienda (Animación)
    if (hBmpTienda[0]) DeleteObject(hBmpTienda[0]);
    if (hBmpTienda[1]) DeleteObject(hBmpTienda[1]);

    // Barcos
    if (hBmpBote[0]) DeleteObject(hBmpBote[0]);
    if (hBmpBote[1]) DeleteObject(hBmpBote[1]);
    if (hBmpBarco[0]) DeleteObject(hBmpBarco[0]);
    if (hBmpBarco[1]) DeleteObject(hBmpBarco[1]);
    if (hBmpMuelle) DeleteObject(hBmpMuelle);
    // --- LIBERAR ENEMIGOS (PIRATA Y MAGO) ---
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            if (hBmpPirataAnim[i][j]) {
                DeleteObject(hBmpPirataAnim[i][j]);
                hBmpPirataAnim[i][j] = NULL; // Buena práctica
            }
            if (hBmpMagoAnim[i][j]) {
                DeleteObject(hBmpMagoAnim[i][j]);
                hBmpMagoAnim[i][j] = NULL;
            }
        }
    }
}

void DibujarImagen(HDC hdcDestino, HBITMAP hBitmap, int x, int y, int ancho, int alto)
{
    if (hBitmap == NULL)
        return;
    HDC hdcMemoria = CreateCompatibleDC(hdcDestino);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMemoria, hBitmap);
    BITMAP bm;
    GetObject(hBitmap, sizeof(BITMAP), &bm);
    SetStretchBltMode(hdcDestino, HALFTONE);
    TransparentBlt(hdcDestino, x, y, ancho, alto, hdcMemoria, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255, 0, 255));
    SelectObject(hdcMemoria, hOld);
    DeleteDC(hdcMemoria);
}