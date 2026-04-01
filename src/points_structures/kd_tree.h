#pragma once
#include "../types/lidar_points.h"

enum axis { X = 0, Y = 1, Z = 2 };

typedef struct KDNode {
	size_t point_index;
	struct KDNode *left;
	struct KDNode *right;
} KDNode;

typedef struct KDTree {
	KDNode *nodes;
	size_t *indices;
	KDNode *root;
	const Points *pts;
} KDTree;

/**
 * Creates a KD-tree from a given collection of points.
 *
 * @param[out] tree Pointer to the KD-tree to be initialized.
 * @param[in] pts Pointer to the collection of points used to build the tree.
 *
 * @note The point collection `pts` must be initialized before calling this function.
 * @note The KD-tree `tree` must be properly destroyed after calling this function.
 */
void create_kd_tree(struct KDTree *tree, const Points *pts);

/**
 * Destroys a KD-tree, releasing all associated resources.
 *
 * @param[in] tree Pointer to the KD-tree to be destroyed.
 */
void destroy_kd_tree(struct KDTree *tree);

void check_kd_tree(KDTree *tree);