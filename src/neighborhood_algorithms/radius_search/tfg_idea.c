#include "tfg_idea.h"
#include "../../utils/parse_args.h"

void tfg_radius_search(const struct matrix_t *matrix, size_t index, RadiusResult *result)
{

	const double x = matrix->points->x[index];
	const double y = matrix->points->y[index];
	const double z = matrix->points->z[index];

	const size_t bandwith = get_matrix_bandwidth(matrix);
	size_t search_start_index = (index > bandwith) ? (index - bandwith) : 0;
	size_t search_end_index = index + bandwith;
	if (search_end_index > matrix->points->num_points) {
		search_end_index = matrix->points->num_points;
	}
	const double radius = get_args()->radius_search;

	size_t local_count = 0;
#pragma omp simd
	for (size_t i = search_start_index; i < search_end_index; i++) {
		double distance =
		    euclidian_distance_3d(matrix->points->x[i], matrix->points->y[i], matrix->points->z[i], x, y, z);
		local_count += distance < radius;
	}

	reserves_memory_radius_result(result, local_count);

	size_t counter = 0;
	for (size_t i = search_start_index; i < search_end_index; i++) {
		double distance =
		    euclidian_distance_3d(matrix->points->x[i], matrix->points->y[i], matrix->points->z[i], x, y, z);
		if (distance < get_args()->radius_search) {
			result->indices[counter] = i;
			result->distances[counter] = distance;
			counter++;
		}
	}
}