#define _POSIX_C_SOURCE 199309L
#include "../../utils/parse_args.h"
#include "avx512_query/avx512.h"
#include "points_sorted.h"
#include <stdio.h>
#include <time.h>

void tfg_radius_search(const Points_TFG *points, index_t index, RadiusResult *result)
{
	const data_t radius = get_args()->radius_search;

	const index_t block_index = get_block_index(index, points->points->num_points);
	const index_t bandwith_left = points->bandwith_left[block_index];
	const index_t bandwith_right = points->bandwith_right[block_index];

	// Obtain coordinates of the point to compare
	const data_t x = points->points->x[index];
	const data_t y = points->points->y[index];
	const data_t z = points->points->z[index];

	// Rango de búsqueda: [index - bandwith, index + bandwith]
	index_t search_start_index = (index > bandwith_left) ? (index - bandwith_left) : 0;
	search_start_index = search_start_index & ~(index_t)63; // redondear hacia abajo al múltiplo de 64
	index_t search_end_index = index + bandwith_right;
	if (search_end_index >= points->points->num_points) {
		search_end_index = points->points->num_points - 1;
	}

	// Número total de puntos incluyendo ambos extremos
	const index_t window = search_end_index - search_start_index + 1;

	reserves_memory_radius_result(result, window);

	index_t *restrict indices = result->indices;
	data_t *restrict distances = result->distances;
	const data_t *restrict xs = points->points->x + search_start_index;
	const data_t *restrict ys = points->points->y + search_start_index;
	const data_t *restrict zs = points->points->z + search_start_index;

	index_t elements_count = 0;
	index_t i = 0;

#ifdef __AVX512F__
	elements_count =
	    tfg_radius_search_avx512(xs, ys, zs, window, search_start_index, x, y, z, radius, indices, distances);
#ifdef USE_FLOAT
	i = (window / 16) * 16;
#else
	i = (window / 8) * 8;
#endif
#elif __AVX2__
#endif

	// Scalar fallback (always available)
	for (; i < window; i++) {
		data_t d = euclidian_distance_3d(xs[i], ys[i], zs[i], x, y, z);
		if (d <= radius) {
			indices[elements_count] = search_start_index + i;
			distances[elements_count] = d;
			elements_count++;
		}
	}

	result->count = elements_count;
}
