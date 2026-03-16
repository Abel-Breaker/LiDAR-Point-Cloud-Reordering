#include "../types/lidar_points.h"
#include "../utils/error_handler.h"
#include <stdlib.h>
#include <stdio.h>
/*
#define DIMENISIONS 3

enum axis{
    X = 0,
    Y = 1, 
    Z = 2
};

typedef struct KDNode {
    size_t point_index;
    struct KDNode *left;
    struct KDNode *right;
} KDNode;

static void create_kd_tree_node(struct KDNode **node, size_t point_index){
    *node = malloc(sizeof(**node));
    (*node)->point_index = point_index;
    (*node)->left = nullptr;
    (*node)->right = nullptr;
}

void create_kd_tree(struct KDNode **root, size_t point_index){
    create_kd_tree_node(root, point_index);
}

void insert_point(struct KDNode **root, Points *points, size_t index){

    KDNode **actual_node = root;
    size_t deepth = 0;

    if(*actual_node == nullptr){
        handle_error(ERROR_TREE_NOT_INITIALIZED, ERR_FATAL, nullptr);
    }

    while(*actual_node != nullptr){
        enum axis current_axis = deepth%DIMENISIONS;

        // TODO: Para optimización se puede eliminar la comparacion del eje si en cada iteracion se modifica un puntero a una funcion (3 funciones, una por cada eje)
        switch (current_axis)
        {
        case X:
            if (points->x[(*actual_node)->point_index] > points->x[index]){ // left
                actual_node = &(*actual_node)->left;
            }
            else{
                actual_node = &(*actual_node)->right;
            }
            break;
        case Y:
            if (points->y[(*actual_node)->point_index] > points->y[index]){ // left
                actual_node = &(*actual_node)->left;
            }
            else{
                actual_node = &(*actual_node)->right;
            }
            break;
        case Z:
            if (points->z[(*actual_node)->point_index] > points->z[index]){ // left
                actual_node = &(*actual_node)->left;
            }
            else{
                actual_node = &(*actual_node)->right;
            }
            break;
        //default:
            //break;
        }
        ++deepth;
    }

    KDNode *new_node = nullptr;
    create_kd_tree_node(&new_node, index);
    (*actual_node) = new_node;
}

void print_kd_tree(struct KDNode *root, Points *points) {
    if (root == NULL) return;

    struct KDNode *queue[1024];
    int depth_queue[1024];
    char side_queue[1024]; // 'R' = root, 'L' = left, 'r' = right
    int front = 0, back = 0;

    queue[back] = root;
    depth_queue[back] = 0;
    side_queue[back] = 'R';
    back++;

    int current_depth = 0;
    printf("Depth 0:\n");

    while (front < back) {
        struct KDNode *node = queue[front];
        int depth = depth_queue[front];
        char side = side_queue[front];
        front++;

        if (depth != current_depth) {
            current_depth = depth;
            printf("Depth %d:\n", current_depth);
        }

        printf("  [%zu] (%c) X: %f, Y: %f, Z: %f\n",
            node->point_index,
            side,
            points->x[node->point_index],
            points->y[node->point_index],
            points->z[node->point_index]);

        if (node->left) {
            queue[back] = node->left;
            depth_queue[back] = depth + 1;
            side_queue[back] = 'l';
            back++;
        }
        if (node->right) {
            queue[back] = node->right;
            depth_queue[back] = depth + 1;
            side_queue[back] = 'r';
            back++;
        }
    }
}

void destroy_kd_tree(struct KDNode *node){
    if(node == nullptr){
        return;
    }
    destroy_kd_tree(node->left);
    destroy_kd_tree(node->right);
    free(node);
}*/