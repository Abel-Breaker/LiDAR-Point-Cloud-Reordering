#include "utils/parse_args.h"
#include "types/lidar_points.h"
#include "utils/parse_lidar_points.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	Args args;

	parse_args(argc, argv, &args);

	printf("filename: %s\n", args.cloud_points_file_name);

	Points points = {};

	printf("%d\n", read_las_points(args.cloud_points_file_name, &points));

	printf("%f\n", points.x[0]);
	
	destroy_points(&points);

	return 0;
}