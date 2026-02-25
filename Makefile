# Makefile

# Prefix for silent compilation
Q ?= @

# Nombre del ejecutable
TARGET = program

# Busca todos los archivos .c en el proyecto (incluyendo subcarpetas)
SRCS = $(shell find . -name "*.c")
OBJS = $(SRCS:.c=.o)

# Flags comunes a todos los modos
COMMON_FLAGS = -pthread -std=c2x

# Flags específicos
CFLAGS_DEBUG = -O0 -g3 -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 \
-Wconversion -Wsign-conversion -Wuninitialized -Wlogical-op -Wduplicated-cond \
-Wunused -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wstrict-overflow=5 \
-Wnull-dereference -Wdouble-promotion -Wstack-protector -Wstrict-aliasing=2 \
-fno-omit-frame-pointer -fsanitize=address,undefined,leak,bounds,alignment,object-size \
-fstack-clash-protection -D_FORTIFY_SOURCE=2 -fanalyzer -Wshift-overflow \
-Wundef -Wbad-function-cast -Wcast-align -Wwrite-strings --strict-flex-arrays=3 \
-Wshift-negative-value -Walloc-zero

CFLAGS_RELEASE = -O2

# Define el modo por defecto
MODE ?= debug

ifeq ($(MODE),release)
	CFLAGS = $(CFLAGS_RELEASE) $(COMMON_FLAGS)
else
	CFLAGS = $(CFLAGS_DEBUG) $(COMMON_FLAGS)
endif

CC = gcc

all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(OBJS)
	$(Q)$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

# Regla para compilar cualquier .c a .o
%.o: %.c
	$(Q)$(CC) -c $< -o $@ $(CFLAGS)

# Limpieza de objetos y ejecutable
clean:
	$(Q)rm -f $(OBJS) $(TARGET)

.PHONY: all clean