# Makefile
# Prefix for silent compilation
Q ?= @

# Nombre del ejecutable
TARGET = build/program

# Rutas de librerías estáticas
LASLIB     = third_party/LAStools/LASlib/lib/libLASlib.a
LASPARSE   = utils/io/lidar/points_reader.a
LASWRITER   = utils/io/lidar/points_writer.a


# Busca todos los archivos .c en el proyecto (incluyendo subcarpetas)
C_SRCS = $(shell find ./src ./bench ./tests ./utils -name "*.c")
C_OBJS = $(C_SRCS:.c=.o)

# Busca todos los archivos .a en el proyecto
STATIC_LIBRARIES = $(LASPARSE) $(LASWRITER) $(LASLIB)

# Todos los objetos combinados
OBJS = $(C_OBJS) $(STATIC_LIBRARIES)

# Flags comunes a todos los modos
COMMON_FLAGS = -std=c2x -lstdc++ -lm

# Base de flags de debug (sin sanitizers) -Wpedantic
CFLAGS_DEBUG_BASE = -O0 -g3 -Wall -Wextra -Wpedantic -Wshadow -Wformat=2\
-Wconversion -Wsign-conversion -Wuninitialized -Wlogical-op -Wduplicated-cond \
-Wunused -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wstrict-overflow=5 \
-Wnull-dereference -Wdouble-promotion -Wstack-protector -Wstrict-aliasing=3 \
-fno-omit-frame-pointer -fstack-clash-protection -fanalyzer \
-Wshift-overflow -Wundef -Wbad-function-cast -Wcast-align -Wwrite-strings \
--strict-flex-arrays=3 -Wshift-negative-value -Walloc-zero -fno-common \
-Wno-unknown-pragmas
#-Wunsuffixed-float-constants #-Wformat=1

# Sanitizers: incompatibles entre sí, se seleccionan por modo
SANITIZE_DEBUG         = -fsanitize=address,undefined,leak,bounds,alignment,object-size
SANITIZE_DEBUG_THREADS = -fsanitize=thread,undefined


CFLAGS_DEBUG         = $(CFLAGS_DEBUG_BASE) $(SANITIZE_DEBUG)
CFLAGS_DEBUG_THREADS = $(CFLAGS_DEBUG_BASE) $(SANITIZE_DEBUG_THREADS) -fopenmp
CFLAGS_RELEASE       = -O2 -march=native -fopenmp -ftree-vectorize -fomit-frame-pointer -flto #-ffast-math #-fno-math-errno #-ffinite-math-only#-ffast-math  #-funroll-loops #-fopt-info-vec -fopt-info-vec-missed -D_FORTIFY_SOURCE=2 3 
CFLAGS_VALGRIND 	 = $(CFLAGS_DEBUG_BASE)


# Define el modo por defecto
MODE ?= debug
ifeq ($(MODE),release)
	CFLAGS = $(CFLAGS_RELEASE) $(COMMON_FLAGS)
else ifeq ($(MODE),debug_threads)
	CFLAGS = $(CFLAGS_DEBUG_THREADS) $(COMMON_FLAGS)
else ifeq ($(MODE),valgrind)
	CFLAGS = $(CFLAGS_VALGRIND) $(COMMON_FLAGS)
else
	CFLAGS = $(CFLAGS_DEBUG) $(COMMON_FLAGS)
endif

# Tipo de dato
DATA_TYPE ?= double
ifeq ($(DATA_TYPE),float)
	CFLAGS += -DUSE_FLOAT
	CXXFLAGS += -DUSE_FLOAT
endif

CC  = gcc
CXX = g++

all: $(LASLIB) $(LASPARSE) $(LASWRITER) $(TARGET)

# ── LAStools: compilar solo si no existe la librería ──────────────────────────
$(LASLIB):
	@echo "[LAStools] Building libLASlib.a..."
	$(Q)cd third_party/LAStools && cmake . --log-level=ERROR && $(MAKE)
	$(Q)rm -rf third_party/LAStools/CMakeFiles \
	           third_party/LAStools/cmake_install.cmake \
	           third_party/LAStools/CMakeCache.txt \
	           third_party/LAStools/Makefile

# ── Wrapper parse_lidar_points: compilar solo si no existe la librería ────────
$(LASPARSE):
	$(Q)$(CXX) -std=c++23 -O2 $(CXXFLAGS)\
		-I./third_party/LAStools/LASlib/inc \
		-I./src/points \
		-I./third_party/LAStools/LASzip/src \
		-I./third_party/LAStools/LASzip/include/laszip \
		-c ./utils/io/lidar/points_reader.cpp \
		-o ./utils/io/lidar/points_reader.o
	$(Q)ar rcs $(LASPARSE) ./utils/io/lidar/points_reader.o
	$(Q)rm ./utils/io/lidar/points_reader.o

# ── Wrapper parse_lidar_points: compilar solo si no existe la librería ────────
$(LASWRITER):
	$(Q)$(CXX) -std=c++23 -O2 $(CXXFLAGS)\
		-I./third_party/LAStools/LASlib/inc \
		-I./src/points \
		-I./third_party/LAStools/LASzip/src \
		-I./third_party/LAStools/LASzip/include/laszip \
		-c ./utils/io/lidar/points_writer.cpp \
		-o ./utils/io/lidar/points_writer.o
	$(Q)ar rcs $(LASWRITER) ./utils/io/lidar/points_writer.o
	$(Q)rm ./utils/io/lidar/points_writer.o

# ── Ejecutable principal ──────────────────────────────────────────────────────
$(TARGET): $(C_OBJS)
	@mkdir -p $(dir $(TARGET))
	$(Q)$(CC) $(C_OBJS) $(STATIC_LIBRARIES) -o $(TARGET) $(CFLAGS)

# Regla para compilar cualquier .c a .o
%.o: %.c
	$(Q)$(CC) -c $< -o $@ $(CFLAGS)

# Limpieza de objetos y ejecutable (no toca las .a)
clean:
	$(Q)rm -f $(C_OBJS) $(TARGET) utils/io/lidar/points_reader.a utils/io/lidar/points_writer.a

# Limpieza total incluyendo las librerías estáticas generadas
distclean: clean
	$(Q)rm -rf third_party/LAStools/CMakeFiles \
	           third_party/LAStools/cmake_install.cmake \
	           third_party/LAStools/CMakeCache.txt \
	           third_party/LAStools/Makefile
			   
include tidy.mk

.PHONY: all clean distclean tidy