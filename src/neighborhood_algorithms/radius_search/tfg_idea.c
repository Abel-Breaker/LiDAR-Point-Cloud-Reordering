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

void tfg_radius_search(const Points *points, size_t index, size_t bandwith_left, size_t bandwith_right, RadiusResult *result)
{
	struct timespec t0, tb, tc, t1, t2, t3;

	clock_gettime(CLOCK_MONOTONIC_RAW, &t0);

	const double radius = get_args()->radius_search;

	clock_gettime(CLOCK_MONOTONIC_RAW, &tb);

	// Coordenadas del punto central
	const double x = points->x[index];
	const double y = points->y[index];
	const double z = points->z[index];

	// Rango de búsqueda: [index - bandwith, index + bandwith]
	size_t search_start_index = (index > bandwith_left) ? (index - bandwith_left) : 0;

	size_t search_end_index = index + bandwith_right;
	if (search_end_index >= points->num_points) {
		search_end_index = points->num_points - 1;
	}

	// Número total de puntos incluyendo ambos extremos
	const size_t window = search_end_index - search_start_index + 1;

	reserves_memory_radius_result(result, window);

	clock_gettime(CLOCK_MONOTONIC_RAW, &tc);

	size_t *restrict indices = result->indices;
	double *restrict distances = result->distances;

	const double *restrict xs = points->x + search_start_index;
	const double *restrict ys = points->y + search_start_index;
	const double *restrict zs = points->z + search_start_index;

	clock_gettime(CLOCK_MONOTONIC_RAW, &t1);

	// Calcular distancia para todos los puntos del rango
#pragma omp simd
	for (size_t i = 0; i < window; i++) {
		distances[i] = euclidian_distance_3d(xs[i], ys[i], zs[i], x, y, z);
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &t2);

	// Compactación de vecinos válidos dentro del radio
	size_t local_count = 0;
	for (size_t i = 0; i < window; i++) {
		if (distances[i] <= radius) {
			indices[local_count] = search_start_index + i;
			distances[local_count] = distances[i];
			local_count++;
		}
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