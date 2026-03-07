# Makefile
# Prefix for silent compilation
Q ?= @
# Nombre del ejecutable
TARGET = program
# Busca todos los archivos .c en el proyecto (incluyendo subcarpetas)
C_SRCS = $(shell find ./src -name "*.c" ! -path "./src/LAStools/*")
C_OBJS = $(C_SRCS:.c=.o)

# Busca todos los archivos .cpp en el proyecto (incluyendo subcarpetas)
STATIC_LIBRARIES = $(shell find . -name "*.a")

# Todos los objetos combinados
OBJS = $(C_OBJS) $(STATIC_LIBRARIES)

# Flags comunes a todos los modos
# Flags para compilar
COMMON_FLAGS = -std=c2x -lstdc++ -lm -fopt-info-vec

# Base de flags de debug (sin sanitizers)
CFLAGS_DEBUG_BASE = -O0 -g3 -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 \
-Wconversion -Wsign-conversion -Wuninitialized -Wlogical-op -Wduplicated-cond \
-Wunused -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wstrict-overflow=5 \
-Wnull-dereference -Wdouble-promotion -Wstack-protector -Wstrict-aliasing=3 \
-fno-omit-frame-pointer -fstack-clash-protection -fanalyzer \
-Wshift-overflow -Wundef -Wbad-function-cast -Wcast-align -Wwrite-strings \
--strict-flex-arrays=3 -Wshift-negative-value -Walloc-zero -fno-common \
-Wunsuffixed-float-constants 

# Sanitizers: incompatibles entre sí, se seleccionan por modo
SANITIZE_DEBUG         = -fsanitize=address,undefined,leak,bounds,alignment,object-size
SANITIZE_DEBUG_THREADS = -fsanitize=thread,undefined

CFLAGS_DEBUG         = $(CFLAGS_DEBUG_BASE) $(SANITIZE_DEBUG)
CFLAGS_DEBUG_THREADS = $(CFLAGS_DEBUG_BASE) $(SANITIZE_DEBUG_THREADS)
CFLAGS_RELEASE       = -O2 -D_FORTIFY_SOURCE=2

# Define el modo por defecto
MODE ?= debug
ifeq ($(MODE),release)
	CFLAGS = $(CFLAGS_RELEASE) $(COMMON_FLAGS)
else ifeq ($(MODE),debug_threads)
	CFLAGS = $(CFLAGS_DEBUG_THREADS) $(COMMON_FLAGS)
else
	CFLAGS = $(CFLAGS_DEBUG) $(COMMON_FLAGS)
endif

CC = gcc

all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(C_OBJS)
	$(Q)$(CC) $(C_OBJS) $(STATIC_LIBRARIES) -o $(TARGET) $(CFLAGS)

# Regla para compilar cualquier .c a .o
%.o: %.c
	$(Q)$(CC) -c $< -o $@ $(CFLAGS)

# Limpieza de objetos y ejecutable
clean:
	$(Q)rm -f $(C_OBJS) $(TARGET)

.PHONY: all clean