#include "utils/parse_args.h"
#include <stdio.h>

int main(int argc, char **argv)
{
	Args args;

	parse_args(argc, argv, &args);

	printf("filename: %s\n", args.cloud_points_file_name);

	return 0;
}