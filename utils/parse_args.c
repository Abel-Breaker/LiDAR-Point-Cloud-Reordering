#include "parse_args.h"
#include "error_handler.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "types.h"

static Args args;

static struct option long_options[] = {{"filename", required_argument, NULL, 'f'},
				       {"benchmark", no_argument, NULL, 'b'},
				       {"test", no_argument, NULL, 't'},
					   {"radius_reorder", required_argument, NULL, 'R'},
				       {"radius_search", required_argument, NULL, 'r'},
				       {"max_num_of_points", required_argument, NULL, 'n'},
					   {"number_of_blocks", required_argument, NULL, 'm'},
				       {"help", no_argument, NULL, 'h'},
				       {NULL, 0, NULL, 0}};

void parse_args(int argc, char **argv)
{
	memset(&args, 0, sizeof(args)); // Inicialization default

	// Tmp variables for parse size_t
	char *endptr;
	unsigned long long val;

	int option;
	while ((option = getopt_long(argc, argv, "f:btR:r:n:m:h", long_options, NULL)) != -1) {
		switch (option) {
		case 'f':
			args.cloud_points_file_name = optarg;
			break;
		case 'b':
			args.do_benchmark = true;
			break;
		case 't':
			args.do_test = true;
			break;
		case 'R':
			args.radius_reorder = (data_t)atof(optarg);
			args.radius_reorder = args.radius_reorder * args.radius_reorder; // Square
			break;
		case 'r':
			args.radius_search = (data_t)atof(optarg);
			args.radius_search = args.radius_search * args.radius_search; // Square
			break;
		case 'm':
			val = strtoull(optarg, &endptr, 10);

			if (*endptr != '\0') {
				perror("Error al convertir");
			}

			args.number_of_blocks = (index_t)val;
			
			break;
		case 'n':
			val = strtoull(optarg, &endptr, 10);

			if (*endptr != '\0') {
				perror("Error al convertir");
			}

			args.max_num_of_points = (index_t)val;
			printf("%zu\n", args.max_num_of_points);
			break;
		case 'h':
			printf("Use: %s --filename archivo.las\n", argv[0]);
			exit(EXIT_SUCCESS);
		case '?':
			exit(EXIT_FAILURE);
		}
	}

	// Check for obligatory arguments
	if (!args.cloud_points_file_name) {
		handle_error(ERROR_PARSE_ARG, ERR_FATAL, "You must specify the required option --filename or -f\n");
	}
	if (args.number_of_blocks == 0) {
		handle_error(ERROR_PARSE_ARG, ERR_FATAL, "You must specify a number of blocks with option -m different of 0\n");
	}
}

const Args *get_args()
{
	return &args;
}