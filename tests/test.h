#pragma once
#include "../src/points/points.h"

#define NUM_OF_TESTS 100

/**
 * @brief Executes a neighbour search test routine on a set of points.
 *
 * @param points Pointer to the input points dataset.
 * 
 * @note Test Octree and TFG idea (points sorted).
 * @note Compare result with bruteforce (always correct).
 */
void test(const Points *points);