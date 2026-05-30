#define _POSIX_C_SOURCE 199309L
#include "../../utils/parse_args.h"
#include "avx512_query/avx512.h"
#include "points_sorted.h"
#include <math.h>
#include <stdio.h>
#include <time.h>

typedef struct {
	index_t search_start_index;
	index_t search_end_index;
	index_t window;
} Search_bound;

static inline void set_search_bound(const Points_TFG *points, index_t index, Search_bound *sb)
{
    const data_t radius_search  = get_args()->radius_search;
    const data_t radius_reorder = get_args()->radius_reorder;
    const index_t factor        = (index_t)ceil(radius_search / radius_reorder) + 1;
    const index_t num_points    = points->points->num_points;
    const index_t num_blocks    = get_args()->number_of_blocks;
    const index_t block_index   = get_block_index(index, num_points);

    index_t bw_left = 0, bw_right = 0;

    for (index_t i = 0; i < factor; i++) {
        // Sumar hacia la izquierda: block_index, block_index-1, block_index-2, ...
        index_t bi_left  = (block_index >= i) ? (block_index - i) : 0;
        // Sumar hacia la derecha: block_index, block_index+1, block_index+2, ...
        index_t bi_right = (block_index + i < num_blocks) ? (block_index + i) : (num_blocks - 1);

        bw_left  += points->bandwith_left[bi_left];
        bw_right += points->bandwith_right[bi_right];
    }

    sb->search_start_index = (index > bw_left) ? (index - bw_left) : 0;
    sb->search_start_index = sb->search_start_index & ~(index_t)63;

    sb->search_end_index = index + bw_right;
    if (sb->search_end_index >= num_points)
        sb->search_end_index = num_points - 1;

    sb->window = sb->search_end_index - sb->search_start_index + 1;
}

void tfg_radius_search(const Points_TFG *points, index_t index, RadiusResult *result)
{
	const data_t radius_search = get_args()->radius_search;

	Search_bound search_bound = {};
	set_search_bound(points, index, &search_bound);

	// Prepare pointers
	index_t *restrict indices = result->indices;
	data_t *restrict distances = result->distances;
	// Obtain coordinates of the point to compare
	const data_t x = points->points->x[index];
	const data_t y = points->points->y[index];
	const data_t z = points->points->z[index];
	const data_t *restrict xs = points->points->x + search_bound.search_start_index;
	const data_t *restrict ys = points->points->y + search_bound.search_start_index;
	const data_t *restrict zs = points->points->z + search_bound.search_start_index;

	index_t elements_count = 0;
	index_t i = 0;

#ifdef __AVX512F__
	elements_count = tfg_radius_search_avx512(xs, ys, zs, search_bound.window, search_bound.search_start_index, x,
						  y, z, radius_search, indices, distances);
#ifdef USE_FLOAT
	i = (search_bound.window / 16) * 16;
#else
	i = (search_bound.window / 8) * 8;
#endif
#elif __AVX2__
#endif

	// Scalar fallback (always available)
	for (; i < search_bound.window; i++) {
		data_t d = euclidian_distance_3d(xs[i], ys[i], zs[i], x, y, z);
		if (d <= radius_search) {
			indices[elements_count] = search_bound.search_start_index + i;
			distances[elements_count] = d;
			elements_count++;
		}
	}

	result->count = elements_count;
}
