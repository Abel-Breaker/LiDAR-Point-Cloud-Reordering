#include "kd_tree_optimized.h"
#include "../types/lidar_points.h"
#include "../utils/error_handler.h"
#include <stdio.h>
#include <stdlib.h>


static double get_coord(const Points *pts, size_t idx, enum axis axis)
{
    switch (axis) {
        case X: return pts->x[idx];
        case Y: return pts->y[idx];
        case Z: return pts->z[idx];
        default: return 0.0f;
    }
}

static size_t partition_lomuto(KDTree *tree, size_t start, size_t end, int axis)
{
    size_t pivot_idx = start + (end - start) / 2;
    double pivot_val = get_coord(tree->pts, tree->indices[pivot_idx], axis);

    // Mueve el pivote al final
    size_t tmp = tree->indices[pivot_idx];
    tree->indices[pivot_idx] = tree->indices[end - 1];
    tree->indices[end - 1] = tmp;

    size_t store = start;
    for (size_t k = start; k < end - 1; k++) {
        if (get_coord(tree->pts, tree->indices[k], axis) < pivot_val) {
            tmp = tree->indices[k];
            tree->indices[k] = tree->indices[store];
            tree->indices[store] = tmp;
            store++;
        }
    }
    // Devuelve el pivote a su posición final
    tmp = tree->indices[store];
    tree->indices[store] = tree->indices[end - 1];
    tree->indices[end - 1] = tmp;
    return store;
}

// Reordena indices[start..end) tal que indices[nth] queda en su posición
// correcta, con menores a la izquierda y mayores a la derecha
static void nth_element(KDTree *tree, size_t start, size_t nth, size_t end, int axis)
{
    while (end - start > 1) {
        size_t pivot_pos = partition_lomuto(tree, start, end, axis);

        if (pivot_pos == nth)
            return;
        else if (nth < pivot_pos)
            end = pivot_pos;
        else
            start = pivot_pos + 1;
    }
}

static KDNode *kd_build(KDTree *tree, size_t start, size_t end, int depth)
{
    // 1. Caso base
    if (start >= end) return NULL;

    // 2. Eje actual
    int axis = depth % DIMENSIONS;

    // 4. Mediano
    size_t mid = start + (end - start) / 2;
    // 3. Ordenar indices[start..end) por coordenada axis
    nth_element(tree, start, mid, end, axis); 

    // 5. Nodo del pool
    KDNode *node = &tree->nodes[mid];
    node->point_index = tree->indices[mid];

    // 6. Recursión
    node->left  = kd_build(tree, start, mid,     depth + 1);
    node->right = kd_build(tree, mid + 1, end,   depth + 1);

    return node;
}

void create_kd_tree(struct KDTree *tree, const Points *pts)
{
    size_t num_points = pts->num_points;

    tree->pts = pts;
	tree->nodes = malloc(pts->num_points * sizeof(*(tree->nodes)));
	tree->indices = malloc(pts->num_points * sizeof(*(tree->indices)));
	for (size_t i = 0; i < num_points; i++) {
		tree->indices[i] = i;
	}

    tree->root = kd_build(tree, 0, num_points, 0);
}


void destroy_kd_tree(struct KDTree *tree)
{
	free(tree->indices);
    free(tree->nodes);
}

int verify_kd_node(KDTree *tree, size_t start, size_t end, int depth)
{
    if (end - start <= 1) return 1;

    int axis      = depth % DIMENSIONS;
    size_t mid    = start + (end - start) / 2;
    double mid_val = get_coord(tree->pts, tree->indices[mid], axis);

    for (size_t i = start; i < mid; i++) {
        if (get_coord(tree->pts, tree->indices[i], axis) > mid_val) {
            printf("ERROR en depth=%d axis=%d: indices[%zu]=%.3f > mediano=%.3f\n",
                   depth, axis,
                   tree->indices[i],
                   get_coord(tree->pts, tree->indices[i], axis),
                   mid_val);
            return 0;
        }
    }

    return verify_kd_node(tree, start, mid, depth + 1)
        && verify_kd_node(tree, mid + 1, end, depth + 1);
}

static int count_nodes(KDNode *node)
{
    if (node == NULL) return 0;
    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

void check_number_of_nodes(KDTree *tree)
{
    int count = count_nodes(tree->root);
    printf("Número de nodos en el árbol: %d\n", count);
}

/*
KDNode* kdtree_find_leaf(KDTree *tree, size_t point_index){

    double x = tree->pts->x[point_index];
    double y = tree->pts->y[point_index];
    double z = tree->pts->z[point_index];
    int depth = 0;

    KDNode *actual_node = tree->nodes;
    while(actual_node->left != nullptr && actual_node->right != nullptr){
        int axis = depth % DIMENISIONS;

        switch (axis)
        {
        case X:
            if(tree->pts->x[actual_node->point_index] > x){
                if(actual_node->left == nullptr){ // Leaf reached
                    break;
                }
                else{
                    actual_node = actual_node->left;
                }
            }
            else{
                if(actual_node->right == nullptr){ // Leaf reached
                    break;
                }
                else{
                    actual_node = actual_node->right;
                }
            }
            break;
        case Y:
            if(tree->pts->y[actual_node->point_index] > y){
                if(actual_node->left == nullptr){ // Leaf reached
                    break;
                }
                else{
                    actual_node = actual_node->left;
                }
            }
            else{
                if(actual_node->right == nullptr){ // Leaf reached
                    break;
                }
                else{
                    actual_node = actual_node->right;
                }
            }
            break;
        case Z:
            if(tree->pts->z[actual_node->point_index] > z){
                if(actual_node->left == nullptr){ // Leaf reached
                    break;
                }
                else{
                    actual_node = actual_node->left;
                }
            }
            else{
                if(actual_node->right == nullptr){ // Leaf reached
                    break;
                }
                else{
                    actual_node = actual_node->right;
                }
            }
            break;
        
        default:
            break;
        }
    }

    printf("Leaf reached at depth %d\n", depth);

}*/