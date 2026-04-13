#pragma once
#include "../types/lidar_points.h"

typedef struct KDNodePrune {
	size_t point_index;
	struct KDNodePrune *left;
	struct KDNodePrune *right;
} KDNodePrune;

typedef struct KDTreePrune {
	KDNodePrune *nodes;
	size_t *indices;
	KDNodePrune *root;
	const Points *pts;
	double *max_distance; // POC: Max distance of N neighbor in KNN search
} KDTreePrune;

/**
 * Creates a KD-tree from a given collection of points.
 *
 * @param[out] tree Pointer to the KD-tree to be initialized.
 * @param[in] pts Pointer to the collection of points used to build the tree.
 *
 * @note The point collection `pts` must be initialized before calling this function.
 * @note The KD-tree `tree` must be properly destroyed after calling this function.
 */
void create_kd_tree_prune(struct KDTreePrune *tree, const Points *pts);

/**
 * Destroys a KD-tree, releasing all associated resources.
 *
 * @param[in] tree Pointer to the KD-tree to be destroyed.
 */
void destroy_kd_tree_prune(struct KDTreePrune *tree);

void check_kd_tree_prune(KDTreePrune *tree);