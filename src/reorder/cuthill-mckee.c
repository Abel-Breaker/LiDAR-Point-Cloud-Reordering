#define _GNU_SOURCE
#include "cuthill-mckee.h"
#include "../../utils/auxiliar_structures/queue.h"
#include "../../utils/error_handler.h"
#include "../../utils/parse_args.h"
#include "../../utils/types.h"
#include "../octree/octree.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _OPENMP
#include <omp.h>
#else
static inline int omp_get_thread_num(void)
{
	return 0;
}
#endif

typedef struct {
	index_t *degrees;
	index_t *indices;
	index_t num_points;
} RcmWorkspaceCommonData;

typedef struct {
	Queue *queue;
	bool *visited;
	index_t *positions;
	Solution *solution;
	index_t points_visited;
	const RcmWorkspaceCommonData *di;
	index_t cursor;
	index_t max_bw;
} RcmWorkspace;

/**
 * @brief Compare function for qsort_r. It uses degrees to order indices of struct RcmWorkspaceCommonData
 */
static int compare(const void *a, const void *b, void *context)
{
	const index_t ia = *(const index_t *)a;
	const index_t ib = *(const index_t *)b;
	index_t *degrees = (index_t *)context;
	(void)degrees[0]; // Only to avoid cppcheck warnings

	if (degrees[ia] < degrees[ib])
		return -1;
	if (degrees[ia] > degrees[ib])
		return 1;
	return 0;
}

/**
 * @brief Reserves memory for struct RcmWorkspaceCommonData
 */
static bool create_degree_index(RcmWorkspaceCommonData *di, index_t num_points)
{
	di->degrees = malloc(sizeof(*(di->degrees)) * num_points);
	di->indices = malloc(sizeof(*(di->indices)) * num_points);

	return di->degrees && di->indices;
}

/**
 * @brief Precalculate all points degrees and inicializate an ordered by degree index array
 */
static void setup_degree_index(RcmWorkspaceCommonData *di, const Octree *octree)
{
	di->num_points = octree->points->num_points;
	const double radius = get_args()->radius_search;

#pragma omp parallel for schedule(static) // TODO: Test with schedule(dynamic)
	for (index_t i = 0; i < di->num_points; ++i) {
		di->degrees[i] = octree_radius_neighbor_count(octree, i, radius);
		di->indices[i] = i;
	}

	qsort_r(di->indices, di->num_points, sizeof(*(di->indices)), compare, di->degrees);
}

/**
 * @brief Reserves memory for struct Solution
 */
static Solution *create_solution(index_t num_points)
{
	Solution *solution = malloc(sizeof(*solution) * num_points);

	solution->permutations = malloc(sizeof(*(solution->permutations)) * num_points);
	solution->bandwith_left = calloc(num_points, sizeof(*(solution->bandwith_left)));
	solution->bandwith_right = calloc(num_points, sizeof(*(solution->bandwith_right)));
	solution->avg_neighbours = 0;
	solution->total_neighours = 0;

	return solution;
}

/**
 * @brief Free memory for struct RcmWorkspaceCommonData and clean values
 */
void destroy_solution(Solution *solution)
{
	free(solution->permutations);
	free(solution->bandwith_left);
	free(solution->bandwith_right);
	free(solution);
}

/**
 * @brief Reserves memory for struct RcmWorkspace and clean values
 */
static bool create_rcm_workspace(RcmWorkspace *ws, const RcmWorkspaceCommonData *di_ref)
{
	ws->queue = create_queue();
	ws->visited = calloc(di_ref->num_points, sizeof(*(ws->visited)));
	ws->positions = malloc(sizeof(*(ws->positions)) * di_ref->num_points);
	for (index_t i = 0; i < di_ref->num_points; ++i) {
		ws->positions[i] = INDEX_MAX;
	}
	ws->solution = create_solution(di_ref->num_points);
	ws->di = di_ref;
	ws->cursor = 0;
	ws->max_bw = 0;
	ws->points_visited = 0;

	return ws->queue && ws->visited && ws->positions;
}

/**
 * @brief Free memory for struct RcmWorkspaceCommonData and clean values
 */
static void destroy_degree_index(RcmWorkspaceCommonData *di)
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
	free(ws->positions);
	destroy_solution(ws->solution);
	ws->cursor = 0;
	ws->max_bw = 0;
	ws->points_visited = 0;
}

/**
 * @brief Free memory for struct RcmWorkspace except Solution
 */
static Solution *destroy_rcm_workspace_except_solution(RcmWorkspace *ws)
{
	Solution *solution = ws->solution;

	destroy_queue(ws->queue);
	free(ws->visited);
	free(ws->positions);
	ws->cursor = 0;
	ws->max_bw = 0;
	ws->points_visited = 0;

	return solution;
}

/**
 * @brief Return the next point with lowest degree. If there isn't any left return INDEX_MAX
 */
static inline index_t get_next_point_index_lowest_degree(RcmWorkspace *ws)
{
	const index_t num_points = ws->di->num_points;

	for (; ws->cursor < num_points; ++(ws->cursor)) {
		if (!ws->visited[ws->di->indices[ws->cursor]]) {
			return ws->di->indices[ws->cursor];
		}
	}
	return INDEX_MAX;
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
static void get_candidates(const Points *points, index_t *candidates, RcmWorkspaceCommonData *ws)
{
	srand((unsigned int)time(NULL));
	double min[DIMENSIONS] = {DBL_MAX, DBL_MAX, DBL_MAX};
	double max[DIMENSIONS] = {-DBL_MAX, -DBL_MAX, -DBL_MAX};
	index_t candidates_aux[6];

	for (index_t i = 0; i < points->num_points; i++) {
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

	// Slot 0: always ws->indices[0]
	candidates[0] = ws->indices[0];

	// Slots 1..6: extremes, but random if degree == 1
	for (index_t i = 1; i < NUM_PARALLEL_RUNS && i <= 6; i++) {
		if (ws->degrees[candidates_aux[i - 1]] == 1)
			candidates[i] = (index_t)rand() % points->num_points;
		else
			candidates[i] = candidates_aux[i - 1];
	}

	// Slots 7+: random
	for (index_t i = 7; i < NUM_PARALLEL_RUNS; i++) {
		candidates[i] = (index_t)rand() % points->num_points;
	}
}

/**
 * @brief Enqueue points that weren't visited and set bandwith right and left
 *
 * Bandwith left corresponds always to visited points
 * Bandwith right corresponds always to the
 */
static void process_result(RadiusResultOctree *result, RcmWorkspace *ws)
{
	for (index_t i = 0; i < result->count; ++i) {
		index_t nb = result->indices[i];
		if (!ws->visited[nb]) {
			enqueue(ws->queue, nb);
			ws->visited[nb] = true;
			ws->positions[nb] = ws->points_visited + get_num_elements(ws->queue);

		} else {
			// Bandwidth izquierdo: distancia al vecino ya visitado más lejano
			if (ws->positions[nb] <
			    ws->points_visited) { // Evita underflow por vecinos encolados pero no procesados
				index_t bw_l = ws->points_visited - ws->positions[nb];
				if (bw_l > ws->solution->bandwith_left[ws->points_visited])
					ws->solution->bandwith_left[ws->points_visited] = bw_l;
			}
		}
	}
	ws->solution->bandwith_right[ws->points_visited] = get_num_elements(ws->queue);
}

static void update_max_bandwith(RcmWorkspace *ws)
{
	index_t total_bw =
	    ws->solution->bandwith_right[ws->points_visited] + ws->solution->bandwith_left[ws->points_visited];
	if (total_bw > ws->max_bw)
		ws->max_bw = total_bw;
}

/**
 * @brief Reverse Cuthill-Mckee algorithm function to run for each thread
 */
static void run_rcm_thread(const Octree *octree, RcmWorkspace *ws, index_t start_index)
{
	const double radius = get_args()->radius_search;
	RadiusResultOctree result = {};

	ws->visited[start_index] = true;
	enqueue(ws->queue, start_index);
	ws->positions[start_index] = 0;

	while (true) {
		index_t index;

		if (is_queue_empty(ws->queue)) {
			index = get_next_point_index_lowest_degree(ws);

			if (index == INDEX_MAX)
				break;

			ws->visited[index] = true;
			enqueue(ws->queue, index);
			ws->positions[index] = ws->points_visited;
		}

		index = dequeue(ws->queue);
		ws->solution->permutations[ws->points_visited] = index;

		octree_radius_search(octree, index, radius, &result);
		qsort_r(result.indices, result.count, sizeof(*(result.indices)), compare, ws->di->degrees);

		process_result(&result, ws);

		update_max_bandwith(ws);

		// DEBUG: for stats
		ws->solution->total_neighours += result.count;

		++(ws->points_visited);
	}

	// DEBUG: for stats
	ws->solution->avg_neighbours = ws->solution->avg_neighbours / ws->points_visited;

	radius_result_destroy(&result);
}

/**
 * @brief returns the Thread ID of best permutations array between all the thread's solutions
 */
static int get_best_permutation(const RcmWorkspace *ws)
{
	int tid_best_result = 0;
	index_t aprox_bw_best_result = INDEX_MAX;

	for (int i = 0; i < NUM_PARALLEL_RUNS; i++) {
		printf("Max Bandwith Aproximation of thread %d: %zu\n", i, (size_t) ws[i].max_bw);
		if (ws[i].max_bw < aprox_bw_best_result) {
			aprox_bw_best_result = ws[i].max_bw;
			tid_best_result = i;
		}
	}

	return tid_best_result;
}

Solution *reorder_cuthill_mckee(const Octree *octree)
{
	// Common to all threads to avoid innecesary copies and memory usage
	RcmWorkspaceCommonData di = {};
	create_degree_index(&di, octree->points->num_points);
	setup_degree_index(&di, octree);

	// Calculate candidates for each thread to start RCM
	index_t *candidates = malloc(sizeof(*candidates) * NUM_PARALLEL_RUNS);
	get_candidates(octree->points, candidates, &di);

	RcmWorkspace *ws = calloc(NUM_PARALLEL_RUNS, sizeof(*ws));

#pragma omp parallel num_threads(NUM_PARALLEL_RUNS)
	{
		int tid = omp_get_thread_num();

		if (!create_rcm_workspace(&(ws[tid]), &di)) {
			handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot allocate auxiliar structures for reorder RCM");
		}

		run_rcm_thread(octree, &(ws[tid]), candidates[tid]);
	}

	int best_tid = get_best_permutation(ws);
	Solution *sol = destroy_rcm_workspace_except_solution(&(ws[best_tid]));

	for (int i = 0; i < NUM_PARALLEL_RUNS; i++) {
		if (best_tid != i) {
			destroy_rcm_workspace(&(ws[i]));
		}
	}

	destroy_degree_index(&di);
	free(ws);
	free(candidates);

	return sol;
}


void print_solution_stats(const Solution *solution, index_t num_points){
	index_t max_one_side = 0;
	index_t max_total = 0;
	double avg_total = 0.0;

	for (index_t i = 0; i < num_points; ++i) {
		index_t total = solution->bandwith_left[i] + solution->bandwith_right[i];

		// Is symetric
		if (solution->bandwith_left[i] > max_one_side)
			max_one_side = solution->bandwith_left[i];

		if (total > max_total)
			max_total = total;

		avg_total += (double)total;
	}

	avg_total /= (double)num_points;

	printf("\n\033[1;33mSTATS\033[0m\n");
	printf("\tTotal neighbours: %zu\n", (size_t) solution->total_neighours);
	printf("\tAvg neighbours: %f\n", (double)solution->total_neighours/(double)num_points);
	printf("\tMax bandwidth   : %zu\n", (size_t) max_total);
	printf("\tMax one side        : %zu\n", (size_t) max_one_side);
	printf("\tAverage bandwidth   : %.2f\n", avg_total);
}