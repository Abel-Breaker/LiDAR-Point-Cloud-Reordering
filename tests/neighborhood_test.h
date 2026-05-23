#pragma once
#include "../src/octree/octree.h"
#include "../src/points_sorted/opt/points_sorted_opt.h"
#include "../src/points_sorted/points_sorted.h"

void check_neighborhoods_octree_radius(const Octree *octree);

void check_neighborhoods_tfg(const Points_TFG *points);

void check_neighborhoods_tfg_opt(const Points_TFG_opt *points);