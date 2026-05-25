#define _POSIX_C_SOURCE 199309L
#include "points_sorted.h"
#include "../../utils/parse_args.h"
#include <stdio.h>
#include <time.h>

#if defined(__AVX512F__)
#include <immintrin.h>
#endif

void tfg_radius_search(const Points_TFG *points, index_t index, RadiusResult *result)
{
	const double radius = get_args()->radius_search;
	
	const index_t block_index = get_block_index(index, points->points->num_points);
	const index_t bandwith_left = points->bandwith_left[block_index];
	const index_t bandwith_right = points->bandwith_right[block_index];

	// Obtain coordinates of the point to compare
	const double x = points->points->x[index];
	const double y = points->points->y[index];
	const double z = points->points->z[index];

	// Rango de búsqueda: [index - bandwith, index + bandwith]
	index_t search_start_index = (index > bandwith_left) ? (index - bandwith_left) : 0;
	search_start_index = search_start_index & ~(index_t)63;  // redondear hacia abajo al múltiplo de 64
	index_t search_end_index = index + bandwith_right;
	if (search_end_index >= points->points->num_points) {
		search_end_index = points->points->num_points - 1;
	}

	// Número total de puntos incluyendo ambos extremos
	const index_t window = search_end_index - search_start_index + 1;

	reserves_memory_radius_result(result, window);

	index_t *restrict indices = result->indices;
	double *restrict distances = result->distances;
	const double *restrict xs = points->points->x + search_start_index;
	const double *restrict ys = points->points->y + search_start_index;
	const double *restrict zs = points->points->z + search_start_index;

	index_t elements_count = 0;
	index_t i = 0;

#if defined(__AVX512F__)

	// Create a vector with the position of the point to compare (x0, y0, z0)
	__m512d x0_vector = _mm512_set1_pd(x);
	__m512d y0_vector = _mm512_set1_pd(y);
	__m512d z0_vector = _mm512_set1_pd(z);

	// Create a vector with the radius
	__m512d radius_vector = _mm512_set1_pd(radius);

	// Create base index (where search starts)
	__m512i base_idx = _mm512_set_epi64(
		search_start_index + 7, search_start_index + 6,
		search_start_index + 5, search_start_index + 4,
		search_start_index + 3, search_start_index + 2,
		search_start_index + 1, search_start_index + 0
	);

	__m512i increment = _mm512_set1_epi64(8);

	// AVX-512 optimized search
	for (; i + 7 < window; i += 8) {

		// Load data from point cloud
		__m512d x1_vector = _mm512_load_pd(xs + i);
		__m512d y1_vector = _mm512_load_pd(ys + i);
		__m512d z1_vector = _mm512_load_pd(zs + i);

		// Differences
		__m512d x_vector_result = _mm512_sub_pd(x1_vector, x0_vector);
		__m512d y_vector_result = _mm512_sub_pd(y1_vector, y0_vector);
		__m512d z_vector_result = _mm512_sub_pd(z1_vector, z0_vector);

		// Squared Euclidean distance
		__m512d result_vec = _mm512_mul_pd(x_vector_result, x_vector_result);
		result_vec = _mm512_fmadd_pd(y_vector_result, y_vector_result, result_vec);
		result_vec = _mm512_fmadd_pd(z_vector_result, z_vector_result, result_vec);

		// Compare distance <= radius
		__mmask8 mask = _mm512_cmp_pd_mask(result_vec, radius_vector, _CMP_LE_OQ);

		if(mask == 0) continue;
		
		// Save matching results
		_mm512_mask_compressstoreu_epi64(indices + elements_count, mask, base_idx);
		_mm512_mask_compressstoreu_pd(distances + elements_count, mask, result_vec);

		// Count valid elements
		elements_count += __builtin_popcount(mask);

		// Increment indices
		base_idx = _mm512_add_epi64(base_idx, increment);
	}

#endif

	// Scalar fallback (always available)
	for (; i < window; i++) {
		double d = euclidian_distance_3d(xs[i], ys[i], zs[i], x, y, z);
		if (d <= radius) {
			indices[elements_count] = search_start_index + i;
			distances[elements_count] = d;
			elements_count++;
		}
	}

	result->count = elements_count;
}
