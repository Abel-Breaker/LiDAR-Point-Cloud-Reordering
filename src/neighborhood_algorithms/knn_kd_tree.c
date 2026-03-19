#include "knn_kd_tree.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../types/lidar_points.h"

static inline double euclidian_distance_3d(double x2, double y2, double z2, double x1, double y1, double z1){
    return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
}

static inline KDNode *update_to_oposite_next_node(const KDNode *node, const Points *points, size_t point_index, int deepth)
{
	int axis = deepth % DIMENSIONS;

	switch (axis) {
	case X:
		if (points->x[node->point_index] > points->x[point_index]) {
			return node->right;
		} else {
			return node->left;
		}
		break;
	case Y:
		if (points->y[node->point_index] > points->y[point_index]) {
			return node->right;
		} else {
			return node->left;
		}
		break;
	case Z:
		if (points->z[node->point_index] > points->z[point_index]) {
			return node->right;
		} else {
			return node->left;
		}
		break;
	default:
    return nullptr;
		break;
	}
}
static inline KDNode *update_to_next_node(const KDNode *node, const Points *points, size_t point_index, int deepth)
{
	int axis = deepth % DIMENSIONS;

	switch (axis) {
	case X:
		if (points->x[node->point_index] > points->x[point_index]) {
			return node->left;
		} else {
			return node->right;
		}
		break;
	case Y:
		if (points->y[node->point_index] > points->y[point_index]) {
			return node->left;
		} else {
			return node->right;
		}
		break;
	case Z:
		if (points->z[node->point_index] > points->z[point_index]) {
			return node->left;
		} else {
			return node->right;
		}
		break;
	default:
        return nullptr;
		break;
	}
}

// TODO: Comprobar que actualnode no es null
static void kdtree_knearest(KDTree *tree, const KDNode *actual_node, int deepth, size_t point_index, size_t neighbours_index[K], double neighbours_distances[K])
{
	if(actual_node == nullptr){
		return;
	}

	double x = tree->pts->x[point_index];
	double y = tree->pts->y[point_index];
	double z = tree->pts->z[point_index];

    // Check this node and update best neighbourg
	double distance = euclidian_distance_3d(tree->pts->x[actual_node->point_index], tree->pts->y[actual_node->point_index], tree->pts->z[actual_node->point_index], x, y, z);
	for (size_t j = 0; j < K; ++j) {
		if (neighbours_distances[j] > distance) {
			memmove(neighbours_index + j + 1, neighbours_index + j,
				(K - j - 1) * sizeof(*neighbours_index));
			memmove(neighbours_distances + j + 1, neighbours_distances + j,
				(K - j - 1) * sizeof(*neighbours_distances));
			neighbours_distances[j] = distance;
			neighbours_index[j] = actual_node->point_index;
			break;
		}
	}

	// Check next neighbourg
	const KDNode *next_node = update_to_next_node(actual_node, tree->pts, point_index, deepth);
    kdtree_knearest(tree, next_node, deepth+1, point_index, neighbours_index, neighbours_distances);

	// Check if necessary other branch
    double split_value, plane_distance;

    int axis = deepth % DIMENSIONS;
	switch (axis) {
	case X:
        split_value = tree->pts->x[actual_node->point_index];
        plane_distance = fabs(x - split_value);
		break;
	case Y:
        split_value = tree->pts->y[actual_node->point_index];
        plane_distance = fabs(y - split_value);
		break;
	case Z:
        split_value = tree->pts->z[actual_node->point_index];
        plane_distance = fabs(z - split_value);
		break;
	default:
    plane_distance = INFINITY;
		break;
	}

    // TODO: neighbours_distances[K-1] es INFINITY al principio
    if(plane_distance <= neighbours_distances[K-1]){ // Compare with the last best distance
        next_node = update_to_oposite_next_node(actual_node, tree->pts, point_index, deepth);
		kdtree_knearest(tree, next_node, deepth+1, point_index, neighbours_index, neighbours_distances);
    }
}

void start_kdtree_knearest(KDTree *tree, size_t point_index, size_t neighbours_index[K], double neighbours_distances[K]){

	//double neighbours_distances[K];

	for (size_t i = 0; i < K; ++i) {
		neighbours_distances[i] = INFINITY;
	}

    kdtree_knearest(tree, tree->root, 0, point_index, neighbours_index, neighbours_distances);
}

