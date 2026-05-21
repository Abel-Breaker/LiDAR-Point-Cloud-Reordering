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
#include <time.h>
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
	size_t num_points;
} DegreeIndex;

typedef struct {
	Queue *queue;
	bool *visited;
	size_t *permutations;
	size_t points_visited;
	RadiusResultOctree result; // Reutilice RadiusResultOctree during RCM to avoid several allocations
	const DegreeIndex *di;
	size_t cursor;
	/*
	 * Approximate reordered matrix bandwidth without explicitly building it.
	 * Uses queue length as a heuristic metric during traversal.
	 */
	size_t aprox_bw;
} RcmWorkspace;

/**
 * @brief Compare function for qsort_r. It uses degrees to order indices of struct DegreeIndex
 */
static int compare(const void *a, const void *b, void *context)
{
	const size_t ia = *(const size_t *)a;
	const size_t ib = *(const size_t *)b;
	size_t *degrees = (size_t *)context;
	(void)degrees[0]; // Only to avoid cppcheck warnings

	if (degrees[ia] < degrees[ib])
		return -1;
	if (degrees[ia] > degrees[ib])
		return 1;
	return 0;
}

/**
 * @brief Reserves memory for struct DegreeIndex
 */
static bool create_degree_index(DegreeIndex *di, size_t num_points)
{
	di->degrees = malloc(sizeof(*(di->degrees)) * num_points);
	di->indices = malloc(sizeof(*(di->indices)) * num_points);

	return di->degrees && di->indices;
}

/**
 * @brief Precalculate all points degrees and inicializate an ordered by degree index array
 */
static void setup_degree_index(DegreeIndex *di, const Octree *octree)
{
	di->num_points = octree->points->num_points;
	const double radius = get_args()->radius_search;

#pragma omp parallel for schedule(static) // TODO: Test with schedule(dynamic)
	for (size_t i = 0; i < di->num_points; ++i) {
		di->degrees[i] = octree_radius_neighbor_count(octree, i, radius);
		di->indices[i] = i;
	}

	qsort_r(di->indices, di->num_points, sizeof(*(di->indices)), compare, di->degrees);
}

/**
 * @brief Reserves memory for struct RcmWorkspace and clean values
 */
static bool create_rcm_workspace(RcmWorkspace *ws, const DegreeIndex *di_ref)
{
	ws->queue = create_queue();
	ws->visited = calloc(di_ref->num_points, sizeof(*(ws->visited)));
	ws->permutations = malloc(sizeof(*(ws->permutations)) * di_ref->num_points);
	ws->di= di_ref;
	ws->result = (RadiusResultOctree){0};
	ws->cursor = 0;
	ws->aprox_bw = 0;
	ws->points_visited = 0;

	return ws->queue && ws->visited && ws->permutations;
}

/**
 * @brief Free memory for struct DegreeIndex and clean values
 */
static void destroy_degree_index(DegreeIndex *di)
{
	free(di->degrees);
	free(di->indices);
	di->num_points = 0;
}

/**
 * @brief Free memory for struct RcmWorkspace and clean values
 */
static void destroy_rcm_workspace(RcmWorkspace *ws)
{
	destroy_queue(ws->queue);
	free(ws->visited);
	free(ws->permutations);
	radius_result_destroy(&(ws->result));
	ws->cursor = 0;
	ws->aprox_bw = 0;
	ws->points_visited = 0;
}

/**
 * @brief Clone all data from struct RcmWorkspace source to RcmWorkspace destination
 */
static void clone_rcm_workspace(RcmWorkspace *dst, const RcmWorkspace *src)
{
	const size_t num_points = src->di->num_points;

	memcpy(dst->visited, src->visited, num_points * sizeof(*src->visited));
	memcpy(dst->permutations, src->permutations, num_points * sizeof(*src->permutations));
	dst->di = src->di;
	dst->aprox_bw = src->aprox_bw;
	dst->points_visited = src->points_visited;
	dst->cursor = src->cursor;
}

/**
 * @brief Builds a new Points structure with the permutations order
 */
static void build_reordered_points(const size_t *permutations, const Points *old_points, Points *new_points)
{
	const size_t num_points = old_points->num_points;

	if (!reserve_memory_points(new_points, num_points)) {
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot reserve memory for points");
		return;
	}

	for (size_t i = 0; i < num_points; ++i) { // TODO: parallelize?
		size_t idx = permutations[num_points - 1 - i];
		add_point(new_points, i, old_points->x[idx], old_points->y[idx], old_points->z[idx]);
	}
}

/**
 * @brief Return the next point with lowest degree. If there isn't any left return SIZE_MAX
 */
static inline size_t get_next_point_index_lowest_degree(RcmWorkspace *ws)
{
	const size_t num_points = ws->di->num_points;

	for (; ws->cursor < num_points; ++(ws->cursor)) {
		if (!ws->visited[ws->di->indices[ws->cursor]]) {
			return ws->di->indices[ws->cursor];
		}
	}
	return SIZE_MAX;
}

/**
 * @brief Get num_candidates candidates for start RCM.
 *
 * TODO: 8 candidates, edges of the bounding box that envolves the cloud points?
 *
 * @note - The first candidate is the point with lowest degree
 * @note - The next 6 are the points with the min and max values for each dimesion
 * @note - The rest of candidates are random points of the cloud
 */
static void get_candidates(const Points *points, size_t *candidates, size_t num_candidates, RcmWorkspace *ws)
{
	srand(time(NULL));
	double min[DIMENSIONS] = {DBL_MAX, DBL_MAX, DBL_MAX};
	double max[DIMENSIONS] = {-DBL_MAX, -DBL_MAX, -DBL_MAX};
	size_t candidates_aux[6];

	for (size_t i = 0; i < points->num_points; i++) {
		if (points->x[i] < min[0]) {
			candidates_aux[0] = i;
			min[0] = points->x[i];
		}
		if (points->y[i] < min[1]) {
			candidates_aux[1] = i;
			min[1] = points->y[i];
		}
		if (points->z[i] < min[2]) {
			candidates_aux[2] = i;
			min[2] = points->z[i];
		}
		if (points->x[i] > max[0]) {
			candidates_aux[3] = i;
			max[0] = points->x[i];
		}
		if (points->y[i] > max[1]) {
			candidates_aux[4] = i;
			max[1] = points->y[i];
		}
		if (points->z[i] > max[2]) {
			candidates_aux[5] = i;
			max[2] = points->z[i];
		}
	}

	candidates[0] = ws->di->indices[0];

	for (size_t i = 1; i < num_candidates; i++) {
		if (i <= 6)
			candidates[i] = candidates_aux[i - 1];
		else
			candidates[i] = rand() % points->num_points;
	}
}

/**
 * @brief Reverse Cuthill-Mckee algorithm function to run for each thread
 */
static void run_rcm_thread(const Octree *octree, RcmWorkspace *local_ws, size_t start_index)
{
	const double radius = get_args()->radius_search;

	local_ws->visited[start_index] = true;
	enqueue(local_ws->queue, start_index);

	while (true) {

		size_t index;

		// If queue empty, graph is disconnected so pick next unvisited node with lowest degree
		if (is_queue_empty(local_ws->queue)) {

			index = get_next_point_index_lowest_degree(local_ws);

			// Check if all nodes where visited
			if (index == SIZE_MAX) {
				break;
			}

			local_ws->visited[index] = true;
			enqueue(local_ws->queue, index);
		}

		// Obtain next point of the queue
		index = dequeue(local_ws->queue);
		local_ws->permutations[local_ws->points_visited] = index;
		++(local_ws->points_visited);

		// Get points neighbors and order by degree
		octree_radius_search(octree, index, radius, &(local_ws->result));
		qsort_r(local_ws->result.indices, local_ws->result.count, sizeof(*(local_ws->result.indices)), compare,
			local_ws->di->degrees);

		// Enqueue points not visited
		for (size_t i = 0; i < local_ws->result.count; ++i) {
			if (local_ws->visited[local_ws->result.indices[i]] == false) {
				enqueue(local_ws->queue, local_ws->result.indices[i]);
				local_ws->visited[local_ws->result.indices[i]] = true;
			}
		}

		// Update max BW
		if (get_num_elements(local_ws->queue) > local_ws->aprox_bw) {
			local_ws->aprox_bw = get_num_elements(local_ws->queue);
		}
	}
}

/**
 * @brief returns the best permutations array between all the thread's solutions
 */
static size_t *get_best_permutation(const RcmWorkspace *ws)
{
	int tid_best_result = 0;
	size_t aprox_bw_best_result = SIZE_MAX;

	for (int i = 0; i < omp_get_max_threads(); i++) {
		printf("Max Bandwith Aproximation of thread %d: %zu\n", i, ws[i].aprox_bw);
		if (ws[i].aprox_bw < aprox_bw_best_result) {
			aprox_bw_best_result = ws[i].aprox_bw;
			tid_best_result = i;
		}
	}

	printf("tid: %d\n", tid_best_result);

	return ws[tid_best_result].permutations;
}

void reorder_cuthill_mckee(const Octree *octree, Points *new_points)
{
	// Common to all threads to avoid innecesary copies and memory usage
	DegreeIndex di = {};
	create_degree_index(&di, octree->points->num_points);
	setup_degree_index(&di, octree);

	RcmWorkspace *ws = calloc(omp_get_max_threads(), sizeof(*ws));
	if (!create_rcm_workspace(&(ws[0]), &di)) {
		destroy_rcm_workspace(&(ws[0]));
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot allocate auxiliar structures for reorder RCM");
		return;
	}

	// Calculate candidates for each thread to start RCM
	size_t *candidates = malloc(sizeof(*candidates) * omp_get_max_threads());
	get_candidates(octree->points, candidates, omp_get_max_threads(), &(ws[0]));

	// Prepare and start RCM for each thread with different seeds
#pragma omp parallel
	{
		int tid = omp_get_thread_num();
		if (tid != 0) {
			if (!create_rcm_workspace(&(ws[tid]), &di)) {
				destroy_rcm_workspace(&(ws[tid]));
				handle_error(ERROR_MALLOC, ERR_FATAL,
					     "Cannot allocate auxiliar structures for reorder RCM");
			}
			clone_rcm_workspace(&(ws[tid]), &(ws[0]));
		}
#pragma omp barrier

		run_rcm_thread(octree, &(ws[tid]), candidates[tid]);
	}

	build_reordered_points(get_best_permutation(ws), octree->points, new_points);

	for (int i = 0; i < omp_get_max_threads(); i++) {
		destroy_rcm_workspace(&(ws[i]));
	}

	destroy_degree_index(&di);
	free(ws);
	free(candidates);
}