# 1. Compilador
CC = gcc

# 2. Archivos fuente (.c)
SRC = src/main.c \
      src/mapa/mapa.c \
      src/recursos/recursos.c \
      src/sistema/sistema.c

# 3. Nombre del ejecutable final
TARGET = IslasEnGuerra

# 4. Banderas del Compilador (Flags)
CFLAGS = -Wall -Iinclude -Isrc/mapa -Isrc/sistema -Isrc/recursos

# 5. Banderas del Enlazador (Linker)
LDFLAGS = 

# 6. Bibliotecas del Enlazador (solo API de Windows)
LDLIBS = -lgdi32 -lwinmm

# 7. Regla principal
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS) $(LDLIBS)

# 8. Limpieza
clean:
	-del $(TARGET).exe