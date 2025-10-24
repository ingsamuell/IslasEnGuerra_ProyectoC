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
CFLAGS = -Wall -Iinclude

# 5. Banderas del Enlazador (Linker)
LDFLAGS = -Llib

# 5b. Bibliotecas del Enlazador (ESTA ES LA LÍNEA CLAVE)
LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# 6. Regla para construir el proyecto (ESTA LÍNEA ES DIFERENTE)
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS) $(LDLIBS)

# 7. Regla para limpiar los archivos compilados
clean:
	-del $(TARGET).exe