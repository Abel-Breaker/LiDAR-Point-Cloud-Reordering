#include "bruteforce.h"
#include <string.h>
#include <stdio.h>

void find_knn_neighbors(const Points *points, size_t point_index, size_t neighbours_index[K], double neighbours_distances[K]){

    double x = points->x[point_index];
    double y = points->y[point_index];
    double z = points->z[point_index];

    for(size_t i=0; i<K; ++i){
        neighbours_distances[i] = INFINITY;
    }

    for(size_t i=0; i<points->num_points; ++i){
        if(i == point_index){
            //continue;
        }

        double distance = euclidian_distance_3d(points->x[i], points->y[i], points->z[i], x, y, z);
        for(size_t j=0; j<K; ++j){
            if(neighbours_distances[j] > distance){
                memmove(neighbours_index+j+1, neighbours_index+j, (K - j-1) * sizeof(*neighbours_index));
                memmove(neighbours_distances+j+1, neighbours_distances+j, (K - j-1) * sizeof(*neighbours_distances));
                neighbours_distances[j] = distance;
                neighbours_index[j] = i;
                break;
            }
        }
    }

}