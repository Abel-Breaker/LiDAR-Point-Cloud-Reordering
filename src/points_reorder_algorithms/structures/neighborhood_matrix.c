#include "neighborhood_matrix.h"
#include "../../neighborhood_algorithms/knn_kd_tree.h"
#include "../../types/lidar_points.h"
#include "../../utils/error_handler.h"
#include "../../utils/parse_args.h"
#include <stdint.h>
#include <stdlib.h>

bool create_neighborhood_matrix(const KDTree *tree, Matrix *matrix)
{

	if (!tree) {
		return false;
	}

	// Check for the size of the point cloud because argument is of type size_t
	if (tree->pts->num_points > UINT32_MAX) {
		handle_error(ERROR_POINT_CLOUD_TOO_BIG, ERR_NONFATAL, nullptr);
		return false;
	}

	*matrix = malloc(((tree->pts->num_points * K)) * sizeof(**matrix)); // /2 because it is symetric

	for (unsigned int i = 0; i < tree->pts->num_points; ++i) {
		size_t neighbours[K];
		double neighbours_distances[K];
		start_kdtree_knearest(tree, i, neighbours, neighbours_distances);

		for (unsigned int j = 0; j < K; ++j) {
			(*matrix)[i * K + j] = (unsigned int)neighbours[j];
		}
	}

    return true;
}

void destroy_neighborhood_matrix(Matrix matrix){
    if(!matrix) return;
    free(matrix);
}