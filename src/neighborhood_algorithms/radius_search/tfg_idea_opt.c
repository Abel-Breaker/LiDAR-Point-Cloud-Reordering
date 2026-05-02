#define _POSIX_C_SOURCE 199309L
#include "tfg_idea.h"
#include "../../utils/parse_args.h"
#include <stdio.h>
#include <time.h>
#include <immintrin.h> 


void tfg_radius_search_opt(const struct matrix_t *matrix, size_t index, size_t bandwith, RadiusResult *result)
{

	const double radius = get_args()->radius_search;



	// Obtain coordinates of the point to compare
	const double x = matrix->points->x[index];
	const double y = matrix->points->y[index];
	const double z = matrix->points->z[index];
	// Calculate search range
	size_t search_start_index = (index > bandwith) ? (index - bandwith) : 0;
	size_t search_end_index = index + bandwith;
	if (search_end_index > matrix->points->num_points) {
		search_end_index = matrix->points->num_points;
	}
	const size_t window = search_end_index - search_start_index;

	reserves_memory_radius_result(result, window);

	size_t *restrict indices = result->indices;
	double *restrict distances = result->distances;
	const double *restrict xs = matrix->points->x + search_start_index;
	const double *restrict ys = matrix->points->y + search_start_index;
	const double *restrict zs = matrix->points->z + search_start_index;
	size_t elements_count=0;

	// Create a vector with the position of the point to compare (x0, y0, z0)
	__m512d x0_vector = _mm512_set1_pd(x);
	__m512d y0_vector = _mm512_set1_pd(y);
	__m512d z0_vector = _mm512_set1_pd(z);

	// Create a vector with the radius
	__m512d radius_vector = _mm512_set1_pd(radius);

	// Create base index (where search starts)
	__m512i base_idx = _mm512_set_epi64(search_start_index + 7, search_start_index + 6, search_start_index + 5,
					    search_start_index + 4, search_start_index + 3, search_start_index + 2,
					    search_start_index + 1, search_start_index + 0);
	
	__m512i increment = _mm512_set1_epi64(8);

	// Search neighbours
	size_t i=0;
	for (; i + 7 < window; i+=8) {

		// Load data from point cloud (x1, y1, z1)
		__m512d x1_vector = _mm512_loadu_pd(xs+i);
		__m512d y1_vector = _mm512_loadu_pd(ys+i);
		__m512d z1_vector = _mm512_loadu_pd(zs+i);


		// Sub: (x1-x0), (y1-y0), (z1-z0)
		__m512d x_vector_result = _mm512_sub_pd(x1_vector, x0_vector);
		__m512d y_vector_result = _mm512_sub_pd(y1_vector, y0_vector);
		__m512d z_vector_result = _mm512_sub_pd(z1_vector, z0_vector);

		// Pow + Add: (x1-x0)^2 + (y1-y0)^2 + (z1-z0)^2 
		__m512d result;
		result = _mm512_mul_pd(x_vector_result, x_vector_result); // Pow: (x1-x0)^2
		result = _mm512_fmadd_pd(y_vector_result, y_vector_result, result); // Pow + add: (y1-y0)^2 + (x1-x0)^2
		result = _mm512_fmadd_pd(z_vector_result, z_vector_result, result); // Pow + add: (z1-z0)^2 + ((y1-y0)^2 + (x1-x0)^2)

		// Obtain a mask of the points where: distance < radius
		__mmask8 mask = _mm512_cmp_pd_mask(result, radius_vector, _CMP_LE_OQ);

		// Save the result
		_mm512_mask_compressstoreu_epi64(indices+elements_count, mask, base_idx); // Save the indices
		_mm512_mask_compressstoreu_pd(distances+elements_count, mask, result); // Save the distances

		// Update number of elements
		elements_count += __builtin_popcount(mask);

		// Update indices
		base_idx = _mm512_add_epi64(base_idx, increment);
	}

	for (; i < window; i++){
		double d = euclidian_distance_3d(xs[i], ys[i], zs[i], x, y, z);
		if (d <= radius) {
    		indices[elements_count] = search_start_index + i;
    		distances[elements_count] = d;
    		elements_count++;
		}
	}

	result->count = elements_count;
}

void tfg_print_timing_stats_opt(void)
{
}