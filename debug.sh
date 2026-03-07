# If an error ocurs stop
set -e

clear

# Colors
GREEN='\033[1;32m'
YELLOW='\033[1;33m'
RED='\033[1;31m'
BLUE='\033[1;34m'
NC='\033[0m'  # No color / reset

# Clang-tidy
echo -e "\n${RED}Analyzing code with clang-tidy...${NC}"
clang-tidy ./src/*.c -checks=*,-llvmlibc-restrict-system-libc-headers -- -Iutils -std=c2x

# Compile with Make
echo -e "\n\n\n${YELLOW}Compiling...${NC}"
make

# Execute
echo -e "\n\n\n${GREEN}Executing...${NC}"
./program -f cloud_points/pnoa_2016_rio_542-4688_cla-001-irc.laz

# Clean
make clean

: '
g++ -std=c++23 \
  -I./src/LAStools/LASlib/inc \
  -I./src/types \
  -I./src/LAStools/LASzip/src \
  -I./src/LAStools/LASzip/include/laszip \
  -c ./src/utils/parse_lidar_points.cpp \
  -o ./src/utils/parse_lidar_points.o

ar rcs ./src/utils/libparse_lidar_points.a ./src/utils/parse_lidar_points.o


(cd ./src/LAStools && cmake . && make)
rm src/LAStools/CMakeFiles src/LAStools/cmake_install.cmake src/LAStools/CMakeCache.txt src/LAStools/Makefile
'