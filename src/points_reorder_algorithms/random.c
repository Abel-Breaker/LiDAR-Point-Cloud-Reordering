#include "cuthill-mckee.h"
#include "../neighborhood_algorithms/knn/kd_tree.h"
#include "../utils/auxiliar_structures/queue.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void reorder_random(const KDTree *tree, const Points *points, Points *new_points)
{
	(void)tree;
	unsigned int number_of_points = (unsigned int)points->num_points;

	unsigned int *permutations = malloc(number_of_points * sizeof(*permutations));

    
	for (unsigned int i = 0; i < number_of_points; ++i) {
        permutations[i] = i;
	}

    srand((unsigned int)time(NULL));
    for (unsigned int i = number_of_points - 1; i > 0; --i) {
        unsigned int j = (unsigned int)rand() % (i + 1);

        unsigned int temp = permutations[i];
        permutations[i] = permutations[j];
        permutations[j] = temp;
	}

	if (!reserve_memory_points(new_points, points->num_points)) {
		free(permutations);
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot reserve memory for points");
		return;
	}
	

	for (unsigned int i = 0; i < number_of_points; ++i) {
		add_point(new_points, i, points->x[permutations[i]], points->y[permutations[i]],
			  points->z[permutations[i]]);
	}

	free(permutations);
}