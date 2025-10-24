# Makefile para el proyecto Islas en Guerra con Raylib

# 1. Compilador
CC = gcc

# 2. Archivos fuente (.c)
# Añade todos tus archivos .c aquí
SRC = src/main.c \
      src/mapa/mapa.c \
      src/recursos/recursos.c \
      src/sistema/sistema.c

# 3. Nombre del ejecutable final
TARGET = IslasEnGuerra

# 4. Banderas del Compilador (Flags)
# -I le dice dónde buscar los .h (nuestra carpeta 'include')
CFLAGS = -Wall -Iinclude

# 5. Banderas del Enlazador (Linker)
# -L le dice dónde buscar las .a (nuestra carpeta 'lib')
# -lraylib le dice que enlace la biblioteca "raylib"
# Las otras son dependencias de Raylib en Windows
LDFLAGS = -Llib -lraylib -lopengl32 -lgdi32 -lwinmm

# 6. Regla para construir el proyecto
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# 7. Regla para limpiar los archivos compilados
clean:
	del $(TARGET).exe 