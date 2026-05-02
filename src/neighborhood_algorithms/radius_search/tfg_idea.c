#define _POSIX_C_SOURCE 199309L
#include "tfg_idea.h"
#include "../../utils/parse_args.h"
#include <stdio.h>
#include <time.h>

static double time_bandwidth = 0;
static double time_getargs = 0;
static double time_reserves = 0;
static double time_simd = 0;
static double time_compact = 0;
static double time_total = 0;
static size_t call_count = 0;

static inline double diff_us(struct timespec a, struct timespec b)
{
	return (b.tv_sec - a.tv_sec) * 1e6 + (b.tv_nsec - a.tv_nsec) / 1e3;
}

void tfg_radius_search(const struct matrix_t *matrix, size_t index, size_t bandwith, RadiusResult *result)
{
	struct timespec t0, tb, tc, t1, t2, t3;

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

	clock_gettime(CLOCK_MONOTONIC_RAW, &t1);

// Search neighbours - SIMD loop
#pragma omp simd
	for (size_t i = 0; i < window; i++) {
		distances[i] = euclidian_distance_3d(xs[i], ys[i], zs[i], x, y, z);
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &t2);

	// Compaction loop
	size_t local_count = 0;
	for (size_t i = 0; i < window; i++) {
		indices[local_count] = search_start_index + i;
		distances[local_count] = distances[i];
		local_count += (distances[i] <= radius);
	}
	result->count = local_count;

	clock_gettime(CLOCK_MONOTONIC_RAW, &t3);

	time_getargs += diff_us(t0, tb);
	time_reserves += diff_us(tb, tc);
	time_simd += diff_us(t1, t2);
	time_compact += diff_us(t2, t3);
	time_total += diff_us(t0, t3);
	call_count++;
}

void tfg_print_timing_stats(void)
{
	if (call_count == 0)
		return;
	fprintf(stderr, "\n=== tfg_radius_search optimized timing (%zu calls) ===\n", call_count);
	fprintf(stderr, "                 Total         Per call\n");
	fprintf(stderr, "  get_args:   %9.2f ms   %7.3f us\n", time_getargs / 1e3, time_getargs / call_count);
	fprintf(stderr, "  reserves:   %9.2f ms   %7.3f us\n", time_reserves / 1e3, time_reserves / call_count);
	fprintf(stderr, "  SIMD:       %9.2f ms   %7.3f us\n", time_simd / 1e3, time_simd / call_count);
	fprintf(stderr, "  Compact:    %9.2f ms   %7.3f us\n", time_compact / 1e3, time_compact / call_count);
	fprintf(stderr, "  TOTAL:      %9.2f ms   %7.3f us\n", time_total / 1e3, time_total / call_count);
}