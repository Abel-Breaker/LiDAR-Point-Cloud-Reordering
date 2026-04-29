#define _POSIX_C_SOURCE 199309L
#include "bench.h"
#include "../neighborhood_algorithms/radius_search/octree.h"
#include "../types/neighborhood_matrix_mix/neighborhood_matrix.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include "neighborhood_bench.h"
#include "points_structures_bench.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void bench(const Points *points)
{
	/*
	int EventSet = PAPI_NULL;
	long long values[1]; // [0] = L1 DCM, [1] = L2 DCM
	int ret;

	// 1. Inicializar PAPI 
	ret = PAPI_library_init(PAPI_VER_CURRENT);
	if (ret != PAPI_VER_CURRENT) {
		fprintf(stderr, "Error init PAPI: %s\n", PAPI_strerror(ret));
		return;
	}

	// 2. Crear el conjunto de eventos 
	ret = PAPI_create_eventset(&EventSet);
	if (ret != PAPI_OK) {
		fprintf(stderr, "Error create_eventset: %s\n", PAPI_strerror(ret));
		return;
	}

	// 3. Añadir eventos de fallos de caché de datos 

	ret = PAPI_add_event(EventSet, PAPI_L2_DCM); // L2 Data Cache Misses
	if (ret != PAPI_OK) {
		fprintf(stderr, "Error add L2_DCM: %s\n", PAPI_strerror(ret));
		return;
	}
*/
	// Octree creation and check
			Octree octree = {};
			create_octree(&octree, points);
		// OCTREE (creation + knn + radius)
		{
			printf("\n\033[1mOCTREE\033[0m\n");
			// Benchmark Octree creation
			octree_benchmark(points);

			// Benchmark neighborhoods
			neighborhoods_octree_knn_bench(&octree);
			neighborhoods_octree_radius_bench(&octree);

			
		}

		// MATRIX
		{
			printf("\n\033[1mMATRIX\033[0m\n");
			// matrix_t creation
			struct matrix_t matrix = {};
			create_neighbourhood_matrix(&matrix, &octree);

			// Benchmark neighborhoods
			neighborhoods_matrix_bench(&matrix);

			print_matrix_stats(&matrix);

			destroy_neighbourhood_matrix(&matrix);
		}

		// TFG IDEA
		{
			printf("\n\033[1mTFG IDEA\033[0m\n");
			// matrix_t creation
			struct matrix_t matrix = {};
			create_neighbourhood_matrix(&matrix, &octree);

			// Benchmark neighborhoods
			//neighborhoods_tfg_bench(&matrix);

			destroy_neighbourhood_matrix(&matrix);
		}

		destroy_octree(&octree);

}