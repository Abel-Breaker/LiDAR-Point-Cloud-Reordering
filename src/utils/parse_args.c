#include "parse_args.h"
#include "error_handler.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static Args args;

static struct option long_options[] = {{"filename", required_argument, NULL, 'f'},
				       {"benchmark", no_argument, NULL, 'b'},
				       {"test", no_argument, NULL, 't'},
				       {"radius_search", required_argument, NULL, 'r'},
				       {"max_num_of_points", required_argument, NULL, 'n'},
				       {"help", no_argument, NULL, 'h'},
				       {NULL, 0, NULL, 0}};

void parse_args(int argc, char **argv)
{
	memset(&args, 0, sizeof(args)); // Inicialization default

	int option;
	while ((option = getopt_long(argc, argv, "f:btr:n:h", long_options, NULL)) != -1) {
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
		case 'r':
			args.radius_search = atof(optarg);
			break;
		case 'n':
			char *endptr;

			unsigned long long val = strtoull(optarg, &endptr, 10);

			if (*endptr != '\0') {
				perror("Error al convertir");
			}

			args.max_num_of_points = (size_t)val;
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
}

const Args *get_args()
{
	return &args;
}