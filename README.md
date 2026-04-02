# LiDAR Point Cloud Reordering

![Build](https://github.com/Abel-Breaker/LiDAR-Point-Cloud-Reordering/actions/workflows/build.yml/badge.svg)
![Sanitizers](https://github.com/Abel-Breaker/LiDAR-Point-Cloud-Reordering/actions/workflows/sanitizers.yml/badge.svg)
![Clang-Tidy](https://github.com/Abel-Breaker/LiDAR-Point-Cloud-Reordering/actions/workflows/tidy.yml/badge.svg)
![Valgrind](https://github.com/Abel-Breaker/LiDAR-Point-Cloud-Reordering/actions/workflows/valgrind.yml/badge.svg)
![CodeQL](https://github.com/Abel-Breaker/LiDAR-Point-Cloud-Reordering/actions/workflows/codeql.yml/badge.svg)
![cppcheck](https://github.com/Abel-Breaker/LiDAR-Point-Cloud-Reordering/actions/workflows/cppcheck.yml/badge.svg)
![C](https://img.shields.io/badge/C-C2x-blue)

## Description

The main objective of this project is to **reorder a LiDAR point cloud** based on the neighborhood relationships between its points, generating a **sparse symmetric neighborhood matrix** on which different graph reordering algorithms are applied.

These techniques allow us to **reduce the matrix bandwidth** or **obtain block partitioning**, which improves the **spatial locality of the data** and optimizes memory access efficiency in subsequent processing stages.

Although this project is **developed and compiled in C**, LAStools (written in C++) is used for reading points via a small .cpp file (*src/utils/parse_lidar_points.cpp*) that acts as a wrapper for C.


## Main project structure

```
CODE/
├── .github/
│ └── workflows/
├── cloud_points/
│ └── cloud_point_test.laz
├── src/
│ ├── benchmarks/
│ ├── neighborhood_algorithms/
│ ├── points_reorder_algorithms/
│ ├── points_structures/
│ ├── tests/
│ ├── types/
│ ├── utils/
│ │ └── auxiliar_structres/
│ └── main.c
├── third_party/
│ └── LAStools/
├── .clang-format
├── Makefile
├── README.md
└── tidy.mk
```

Brief explanation:
- **.github/workflows/** → GitHub Actions Tests (CI) for compilation, clang-tidy, and valgrind.
- **cloud_points/cloud_point_test.laz** → Little example of a random 1K points - LiDAR point cloud for test and debug.
- **src/** → Main folder with all the code.
- **src/utils/** → Various utilities and functionalities.
- **src/utils/auxiliar_structres/** → TAD structures.
- **src/benchmarks/** → All functions related with benchmarking.
- **src/neighborhood_algorithms/** → Algorithms to search neighbors over differents points_structures.
- **src/points_reorder_algorithms/** → Algorithms for reorganize cloud points (for better cache-locality)
- **src/points_structures/** → Structures for points storage.
- **src/tests/** → All functions related with testing.
- **src/types/** → Primarily the data type for storing lidar points.
- **src/third_party/LAStools/** → Minimal external dependency for reading LiDAR point cloud files.
- **Makefile** → Build system.
- **tidy.mk** → Script for checking code cleanliness and formatting.
- **.clang-format** → Code formatting configuration.


## Compilation

The project includes a Makefile that allows compilation in different modes:

- `make MODE=debug`: Debug mode compilation (default).
- `make MODE=debug_threads`: Mode compilation for thread debug.
- `make MODE=release`: Release-optimized compilation.

> Note: `make tidy` is also included for checking formatting and code cleanup, if desired.

## Example usage

Once the program is compiled, it can be run as follows:

```bash
./program -f cloud_points.laz
```

For more information on using the program:

```bash
./program --help
```

## Minimum Dependencies
- **gcc** (recommended gcc 14)
- **g++**
- **Make**
- **LAStools** (included in the project)