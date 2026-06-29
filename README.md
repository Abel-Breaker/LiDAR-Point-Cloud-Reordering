# LiDAR Point Cloud Reordering

![Build](https://github.com/Abel-Breaker/LiDAR-Point-Cloud-Reordering/actions/workflows/build.yml/badge.svg)
![Sanitizers](https://github.com/Abel-Breaker/LiDAR-Point-Cloud-Reordering/actions/workflows/sanitizers.yml/badge.svg)
![Clang-Tidy](https://github.com/Abel-Breaker/LiDAR-Point-Cloud-Reordering/actions/workflows/tidy.yml/badge.svg)
![Valgrind](https://github.com/Abel-Breaker/LiDAR-Point-Cloud-Reordering/actions/workflows/valgrind.yml/badge.svg)
![CodeQL](https://github.com/Abel-Breaker/LiDAR-Point-Cloud-Reordering/actions/workflows/codeql.yml/badge.svg)
![cppcheck](https://github.com/Abel-Breaker/LiDAR-Point-Cloud-Reordering/actions/workflows/cppcheck.yml/badge.svg)
![C](https://img.shields.io/badge/C-C2x-blue)

## Description

The main objective of this project, developed as part of my **Final Degree Project (TFG)**, is to **order a point cloud** based on the neighborhood relationships between its points, generating a **sparse symmetric neighborhood matrix** on which Reverse Cuthill-Mckee algorithm is applied.

These techniques allow us to **reduce the matrix bandwidth**, which improves the **spatial locality of the data** and it allows to perform narrowed neighbor searches.


## Main project structure

```
CODE/
├── .github/
│ └── workflows/
├── cloud_points/
│ └── cloud_point_test.laz
├── bench/
├── test/
├── src/
│ ├── octree/
│ ├── points/
│ ├── points_sorted/
| |└── avx512_query/
| |└── avx2_query/
│ ├── reorder/
│ └── main.c
├── third_party/
│ └── LAStools/
├── utils/
|└── io
|└── auxiliar_structres/
├── .clang-format
├── Makefile
├── README.md
└── tidy.mk
```

Brief explanation:
- **.github/workflows/** → GitHub Actions Tests (CI) for compilation, clang-tidy, and valgrind.
- **cloud_points/cloud_point_test.laz** → Example of a random 1K points - LiDAR point cloud for test and debug.
- **bench/** → All functions related with benchmarking.
- **src/** → Main folder with the core code.
- **src/octree/** → Octree structure, implementation and query for search neighbours.
- **src/points/** → Points structure, implementation and query for search neighbours.
- **src/points_sorted/** → TFG idea: structure, implementation and query for search neighbours.
- **src/reorder/** → Reverse Cuthill-Mckee algorithm to reorder points.
- **tests/** → All functions related with testing.
- **utils/** → Various utilities and functionalities.
- **utils/io** → Utilities and functionalities to read/write cloud points.
- **utils/auxiliar_structres/** → Some auxiliar structures.
- **LAStools/** → Minimal external dependency for reading LiDAR point cloud files.
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
./build/program -f cloud_points/cloud_point_test.laz -b -t -m 2 -R 100 -r 100
```

For more information on using the program:

```bash
./build/program --help
```

## Notes
Although this project is **developed and compiled in C**, LAStools (written in C++) is used for reading and writing LiDAR points through small .cpp files in *utils/io/lidar/* that act as wrappers for C.

## Minimum Dependencies
- **gcc** (recommended gcc 14)
- **g++**
- **Make**
- **LAStools** (included in the project)