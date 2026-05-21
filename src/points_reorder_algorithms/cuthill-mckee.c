#define _GNU_SOURCE
#include "cuthill-mckee.h"
#include "../neighborhood_algorithms/radius_search/octree.h"
#include "../types/neighborhood_matrix_mix/row.h"
#include "../utils/auxiliar_structures/queue.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include <float.h>
#include <stdint.h> // for SIZE_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _OPENMP
#include <omp.h>
#else
static inline int omp_get_max_threads(void)
{
	return 1;
}
static inline int omp_get_thread_num(void)
{
	return 0;
}
#endif

typedef struct {
	size_t *degrees;
	size_t *indices;
	size_t cursor;
} DegreeIndex;

typedef struct {
	Queue *queue;
	bool *visited;
	size_t *permutations;
	size_t num_points;
	DegreeIndex di;
	RadiusResultOctree result; // Reutilice RadiusResultOctree to avoid several allocations
	size_t points_visited;
	size_t aprox_bw;
} RcmWorkspace;

/*
 * Precalculate all points degrees and inicializate index array
 */
static void setup_degree_index(DegreeIndex *di, const Octree *octree)
{
	const size_t num_points = octree->points->num_points;
	const double radius = get_args()->radius_search;

#pragma omp parallel for schedule(static) // TODO: Test with schedule(dynamic)
	for (size_t i = 0; i < num_points; ++i) {
		di->degrees[i] = octree_radius_neighbor_count(octree, i, radius);
		di->indices[i] = i;
	}
}

static bool create_rcm_workspace(RcmWorkspace *ws, size_t num_points)
{
	ws->queue = create_queue(num_points);
	ws->visited = calloc(num_points, sizeof(*(ws->visited)));
	ws->permutations = malloc(sizeof(*(ws->permutations)) * num_points);
	ws->di.degrees = malloc(sizeof(*(ws->di.degrees)) * num_points);
	ws->di.indices = malloc(sizeof(*(ws->di.indices)) * num_points);
	ws->result = (RadiusResultOctree){0};
	ws->num_points = num_points;
	ws->di.cursor = 0;
	ws->aprox_bw = 0;
	ws->points_visited = 0;

	return ws->queue && ws->visited && ws->permutations && ws->di.degrees && ws->di.indices;
}

static void destroy_rcm_workspace(RcmWorkspace *ws)
{
	destroy_queue(ws->queue);
	free(ws->visited);
	free(ws->permutations);
	free(ws->di.degrees);
	free(ws->di.indices);
	radius_result_destroy(&(ws->result));
	ws->num_points = 0;
	ws->di.cursor = 0;
}

static bool clone_rcm_workspace(RcmWorkspace *dst, const RcmWorkspace *src)
{
	const size_t num_points = src->num_points;

	if (!create_rcm_workspace(dst, num_points)) {
		// destroy_rcm_workspace(dst);
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot reserve memory for points");
		return false;
	}

	// Copiar el estado profundo
	memcpy(dst->visited, src->visited, num_points * sizeof(*src->visited));
	memcpy(dst->permutations, src->permutations, num_points * sizeof(*src->permutations));
	memcpy(dst->di.degrees, src->di.degrees, num_points * sizeof(*src->di.degrees));
	memcpy(dst->di.indices, src->di.indices, num_points * sizeof(*src->di.indices));

	return true;
}

static void build_reordered_points(RcmWorkspace *ws, const Points *old_points, Points *new_points)
{
	const size_t num_points = ws->num_points;

	// Reserves memory for new points
	if (!reserve_memory_points(new_points, num_points)) {
		destroy_rcm_workspace(ws);
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot reserve memory for points");
		return;
	}

	// Save the points in the new order
	// TODO: Paralelizar
	for (size_t i = 0; i < num_points; ++i) {
		size_t idx = ws->permutations[num_points - 1 - i];
		add_point(new_points, i, old_points->x[idx], old_points->y[idx], old_points->z[idx]);
	}
}

static int compare(const void *a, const void *b, void *context)
{
	const size_t ia = *(const size_t *)a;
	const size_t ib = *(const size_t *)b;
	RcmWorkspace *ws = (RcmWorkspace *)context;
	(void)ws->di; // Only to avoid cppcheck warnings

	if (ws->di.degrees[ia] < ws->di.degrees[ib])
		return -1;
	if (ws->di.degrees[ia] > ws->di.degrees[ib])
		return 1;
	return 0;
}

/*
 * Return the next point with lowest degree. If there isn't any left return SIZE_MAX
 */
static inline size_t get_next_point_index_lowest_degree(RcmWorkspace *ws)
{
	const size_t num_points = ws->num_points;

	for (; ws->di.cursor < num_points; ++(ws->di.cursor)) {
		if (!ws->visited[ws->di.indices[ws->di.cursor]]) {
			return ws->di.indices[ws->di.cursor];
		}
	}
	return SIZE_MAX;
}

static void get_candidates(const Points *points, size_t *candidates, size_t num_candidates, RcmWorkspace *ws)
{
    double min[DIMENSIONS] = {DBL_MAX, DBL_MAX, DBL_MAX};
    double max[DIMENSIONS] = {-DBL_MAX, -DBL_MAX, -DBL_MAX};
    size_t candidates_aux[6];

    for (size_t i = 0; i < points->num_points; i++) {
        if (points->x[i] < min[0]) { candidates_aux[0] = i; min[0] = points->x[i]; }
        if (points->y[i] < min[1]) { candidates_aux[1] = i; min[1] = points->y[i]; }
        if (points->z[i] < min[2]) { candidates_aux[2] = i; min[2] = points->z[i]; }
        if (points->x[i] > max[0]) { candidates_aux[3] = i; max[0] = points->x[i]; }
        if (points->y[i] > max[1]) { candidates_aux[4] = i; max[1] = points->y[i]; }
        if (points->z[i] > max[2]) { candidates_aux[5] = i; max[2] = points->z[i]; }
    }

    //candidates[0] = get_next_point_index_lowest_degree(ws);
	candidates[0] = 0;

    for (size_t i = 1; i < num_candidates; i++) {
        if (i <= 6)
            candidates[i] = candidates_aux[i - 1];
        else
            candidates[i] = rand() % points->num_points;
    }
}

void reorder_cuthill_mckee(const Octree *octree, Points *new_points)
{
	// Prepare some constant values
	const size_t num_points = octree->points->num_points;
	const double radius = get_args()->radius_search;

	RcmWorkspace *ws = calloc(omp_get_max_threads(), sizeof(*ws));
	if (!create_rcm_workspace(&(ws[0]), num_points)) {
		destroy_rcm_workspace(&(ws[0]));
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot allocate auxiliar structures for reorder RCM");
		return;
	}

	setup_degree_index(&(ws[0].di), octree);

	// Preorder all indices depending of the degrees
	qsort_r(ws[0].di.indices, num_points, sizeof(*(ws[0].di.indices)), compare, &(ws[0]));

	size_t *candidates = malloc(sizeof(*candidates) * omp_get_max_threads());
	get_candidates(octree->points, candidates, omp_get_max_threads(), &(ws[0]));


#pragma omp parallel
	{
		int tid = omp_get_thread_num();

		if (tid != 0) {
			if (!clone_rcm_workspace(&(ws[tid]), &(ws[0]))) {
				handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot clone workspace");
			}
		}

		RcmWorkspace *local_ws = &(ws[tid]);
		/*
		 * This variable is for calculate and aproximation ON THE FLY of the "matrix" reordered
		 * whithout the necesity to create the matrix, so we can choose the best reorder solution
		 * before build the new reordered points
		 * It only saves the queue lenght for each point processed, obtaining his mas bandwith
		 * through one of the halves (triangles) of the matrix, assuming the same distance through the
		 * other
		 */

		local_ws->visited[candidates[tid]] = true;
		enqueue(local_ws->queue, candidates[tid]);

		// Start the algorithm RCM to obtain permutations vector
		while (true) {

			// If queue is empty, graph is disconnected so pick next unvisited node with lowest
			// degree
			if (is_queue_empty(local_ws->queue)) {

				size_t min_degree_point_index = get_next_point_index_lowest_degree(local_ws);

				// Check if all nodes where visited
				if (min_degree_point_index == SIZE_MAX) {
					break;
				}

				local_ws->visited[min_degree_point_index] = true;
				enqueue(local_ws->queue, min_degree_point_index);
			}

			// Obtain next point
			size_t index = dequeue(local_ws->queue);
			local_ws->permutations[local_ws->points_visited] = index;
			++(local_ws->points_visited);

			// Get points neighbors
			octree_radius_search(octree, index, radius, &(local_ws->result));
			qsort_r(local_ws->result.indices, local_ws->result.count, sizeof(*(local_ws->result.indices)),
				compare, local_ws);

			// Process next point searching for new points to enqueue
			for (size_t i = 0; i < local_ws->result.count; ++i) {
				if (local_ws->visited[local_ws->result.indices[i]] == false) {
					enqueue(local_ws->queue, local_ws->result.indices[i]);
					local_ws->visited[local_ws->result.indices[i]] = true;
				}
			}

			if(get_num_elements(local_ws->queue) > local_ws->aprox_bw){
				local_ws->aprox_bw = get_num_elements(local_ws->queue);
			}
			
		}
		printf("Max Bandwith Aproximation of thread %d: %zu\n", tid, local_ws->aprox_bw);
		// destroy_rcm_workspace(&local_ws);
	}

	int tid_best_result = 0;
	size_t aprox_bw_best_result = SIZE_MAX;
	for(int i=0; i<omp_get_max_threads(); i++){
		if(ws[i].aprox_bw < aprox_bw_best_result){
			aprox_bw_best_result = ws[i].aprox_bw;
			tid_best_result = i;
		}
	}
	printf("tid: %d\n", tid_best_result);

	build_reordered_points(&(ws[tid_best_result]), octree->points, new_points);

	for (int i = 0; i < omp_get_max_threads(); i++) {
		destroy_rcm_workspace(&(ws[i]));
	}
	free(ws);
}