#pragma once
#include "../structures/kd_tree_optimized.h"

#define K 20


void start_kdtree_knearest(KDTree *tree, size_t point_index, size_t neighbours_index[K]);



/*
baja hasta abajo, cuando sea hoja guarda valor y sale, la funcion anterior que la habia llamado primero comprueba qsi debe ver el otro lado, si lo tiene que ver vuelve a llamar,
si no su propio valor chekea y guarda en el array si hace falta y sale, dandole paso a la anterior llamada de la funcion y asi recursivamente
*/