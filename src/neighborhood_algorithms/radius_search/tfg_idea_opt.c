#define _POSIX_C_SOURCE 199309L
#include "tfg_idea.h"
#include "../../utils/parse_args.h"
#include <stdio.h>
#include <time.h>
#include <immintrin.h> 

static double time_bandwidth = 0;
static double time_getargs = 0;
static double time_reserves = 0;
static double time_simd = 0;
static double time_total = 0;
static size_t call_count = 0;

static inline double diff_us(struct timespec a, struct timespec b)
{
	return (b.tv_sec - a.tv_sec) * 1e6 + (b.tv_nsec - a.tv_nsec) / 1e3;
}

void tfg_radius_search_opt(const struct matrix_t *matrix, size_t index, size_t bandwith, RadiusResult *result)
{
	struct timespec t0, tb, tc, t1, t2;

	clock_gettime(CLOCK_MONOTONIC_RAW, &t0);

	const double radius = get_args()->radius_search;

	clock_gettime(CLOCK_MONOTONIC_RAW, &tb);

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

	clock_gettime(CLOCK_MONOTONIC_RAW, &tc);

	size_t *restrict indices = result->indices;
	double *restrict distances = result->distances;
	const double *restrict xs = matrix->points->x + search_start_index;
	const double *restrict ys = matrix->points->y + search_start_index;
	const double *restrict zs = matrix->points->z + search_start_index;
	size_t elements_count=0;

	// Create a vector with de position of the point to compare (x0, y0, z0)
	__m512d x0_vector = _mm512_set1_pd(x);
	__m512d y0_vector = _mm512_set1_pd(y);
	__m512d z0_vector = _mm512_set1_pd(z);

	clock_gettime(CLOCK_MONOTONIC_RAW, &t1);

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
		__m512d radius_vector = _mm512_set1_pd(radius);
		__mmask8 mask = _mm512_cmp_pd_mask(result, radius_vector, _CMP_LT_OQ);

		// Save the result
		__m512i idx_vector =
		    _mm512_set_epi64(7 + search_start_index + i, 6 + search_start_index + i, 5 + search_start_index + i,
				     4 + search_start_index + i, 3 + search_start_index + i, 2 + search_start_index + i,
				     1 + search_start_index + i, 0 + search_start_index + i); // Auxiliar vector of indices
		_mm512_mask_compressstoreu_epi64(indices+elements_count, mask, idx_vector); // Save the indices
		_mm512_mask_compressstoreu_pd(distances+elements_count, mask, result); // Save the distances

		// Update number of elements
		elements_count += _mm_popcnt_u32(mask);
	}

	for (; i < window; i++){
		indices[elements_count] = search_start_index + i;
		distances[elements_count] = euclidian_distance_3d(xs[i], ys[i], zs[i], x, y, z);
		elements_count += (distances[elements_count] <= radius);
	}

	result->count = elements_count;

	clock_gettime(CLOCK_MONOTONIC_RAW, &t2);


	time_getargs += diff_us(t0, tb);
	time_reserves += diff_us(tb, tc);
	time_simd += diff_us(t1, t2);
	time_total += diff_us(t0, t2);
	call_count++;
}

void tfg_print_timing_stats_opt(void)
{
	if (call_count == 0)
		return;
	fprintf(stderr, "\n=== tfg_radius_search timing (%zu calls) ===\n", call_count);
	fprintf(stderr, "                 Total         Per call\n");
	fprintf(stderr, "  get_args:   %9.2f ms   %7.3f us\n", time_getargs / 1e3, time_getargs / call_count);
	fprintf(stderr, "  reserves:   %9.2f ms   %7.3f us\n", time_reserves / 1e3, time_reserves / call_count);
	fprintf(stderr, "  SIMD:       %9.2f ms   %7.3f us\n", time_simd / 1e3, time_simd / call_count);
	fprintf(stderr, "  TOTAL:      %9.2f ms   %7.3f us\n", time_total / 1e3, time_total / call_count);
}