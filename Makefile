# 1. Compilador
CC = gcc

# 2. Archivos fuente (.c)
# Incluimos todos los archivos .c de tu proyecto
SRC = src/main.c \
      src/sistema/sistema.c \
      src/recursos/recursos.c \
      src/jugador/jugador.c \
      src/mundo/mapa.c \
      src/mundo/fauna.c \
      src/mundo/terreno.c \
      src/mundo/naturaleza.c \
      src/unidades/unidades.c \
      src/sistema/tienda.c

# 3. Nombre del ejecutable final
TARGET = IslasEnGuerra

# 4. Banderas del Compilador (Flags)
# -Isrc permite que main.c encuentre #include "mapa/mapa.h"
CFLAGS = -Wall -Isrc

# 5. Banderas del Enlazador (Linker)
# -mwindows le dice al enlazador que es una app GUI (sin consola negra)
LDFLAGS = 

# 5b. Bibliotecas del Enlazador
# ¡Necesitamos la biblioteca GDI (Graphics Device Interface)!
LDLIBS = -lgdi32 -lmsimg32 -lwinmm

# 6. Regla para construir el proyecto
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS) $(LDLIBS)

# 7. Regla para limpiar los archivos compilados
clean:
	-del $(TARGET).exe