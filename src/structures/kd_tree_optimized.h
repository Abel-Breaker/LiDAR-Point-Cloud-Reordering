#pragma once
#include "../types/lidar_points.h"
#define DIMENSIONS 3

enum axis { X = 0, Y = 1, Z = 2 };

typedef struct KDNode {
	size_t point_index;
	struct KDNode *left;
	struct KDNode *right;
} KDNode;

typedef struct KDTree {
	KDNode *nodes;	 /* pool contiguo de nodos               */
	size_t *indices; /* array de índices a ordenar            */
    KDNode  *root;  
	const Points *pts;
} KDTree;

void create_kd_tree(struct KDTree *tree, const Points *pts);

void destroy_kd_tree(struct KDTree *tree);

int verify_kd_node(KDTree *tree, size_t start, size_t end, int depth);

void check_number_of_nodes(KDTree *tree);