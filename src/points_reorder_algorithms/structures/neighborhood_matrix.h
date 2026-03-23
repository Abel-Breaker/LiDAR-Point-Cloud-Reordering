#pragma once

#include "../../structures/kd_tree.h"

typedef unsigned int *Matrix;

bool create_neighborhood_matrix(const KDTree *tree, Matrix *matrix);

void destroy_neighborhood_matrix(Matrix matrix);