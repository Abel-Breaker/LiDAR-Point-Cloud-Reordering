#pragma once
#include "../src/octree/octree.h"
#include "../src/points_sorted/points_sorted.h"

void neighborhoods_octree_radius_bench(const Octree *structure);

void neighborhoods_tfg_bench(const Points_TFG *points);