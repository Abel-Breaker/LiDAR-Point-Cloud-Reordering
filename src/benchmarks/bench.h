#pragma once
#include "../types/lidar_points.h"

void bench(const Points *points);

void bench_prune(const Points *points);

//void cold_bench(const Points *points);