#include "parse_args.h"
#include "error_handler.h"
#include "terminal_formating.h"
#include "types.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct Args args;

static struct option long_options[] = {{"filename", required_argument, NULL, 'f'},
				       {"benchmark", no_argument, NULL, 'b'},
				       {"test", no_argument, NULL, 't'},
					   {"save_ordered_cloud", required_argument, NULL, 's'},
					   {"radius_reorder", required_argument, NULL, 'R'},
				       {"radius_search", required_argument, NULL, 'r'},
				       {"max_num_of_points", required_argument, NULL, 'n'},
					   {"number_of_blocks", required_argument, NULL, 'm'},
				       {"help", no_argument, NULL, 'h'},
				       {NULL, 0, NULL, 0}};

/**
 * @brief Set all args to default value
 */
static inline void init_args()
{
	memset(&args, 0, sizeof(args));
}

/**
 * @brief Prints help message.
 */
static inline void print_help(const char *program_name)
{
	printf(BOLD "Usage: %s [OPTIONS]\n" COLOR_RESET, program_name);

	printf(BOLD "\nRequired options:\n" COLOR_RESET);
	printf("  -f, --filename <file>            Input cloud points file\n");
	printf("  -m, --number_of_blocks <value>   Number of blocks (must be > 0)\n");
	printf("  -R, --radius_reorder <value>     Radius for reorder (must be > 0)\n");
	printf("  -r, --radius_search <value>      Radius for search (must be > 0)\n");

	printf(BOLD "\nOptional flags:\n" COLOR_RESET);
	printf("  -b, --benchmark                  Enable benchmarks\n");
	printf("  -t, --test                       Enable tests\n");
	printf("  -s, --save_ordered_cloud         Order the cloud points in the specified file and save it\n");
	printf("  -n, --max_num_of_points <value>  Set maximum number of points to process\n");

	printf(BOLD "\nOther:\n" COLOR_RESET);
	printf("  -h, --help                       Show this help message\n");

	printf(BOLD "\nNotes:\n" COLOR_RESET);
	printf("  - Radii values are squared internally.\n");
	printf("  - Missing required arguments will cause an error.\n");
}

/**
 * @brief Check for obligatory arguments.
 * 
 * @return True if all required argumnets were specified, fasle if not.
 */
static inline bool check_obligatory_arguments(){
	if (!args.cloud_points_file_name) {
		handle_error(ERROR_PARSE_ARG, ERR_NONFATAL, "You must specify the required option --filename or -f");
		return false;
	}
	if (args.number_of_blocks == 0) {
		handle_error(ERROR_PARSE_ARG, ERR_NONFATAL, "You must specify a number of blocks with option -m greater than 0");
		return false;
	}
	if (args.radius_reorder == 0) {
		handle_error(ERROR_PARSE_ARG, ERR_NONFATAL, "You must specify the required option --radius_reorder or -R greater than 0");
		return false;
	}
	if (args.radius_search == 0) {
		handle_error(ERROR_PARSE_ARG, ERR_NONFATAL, "You must specify the required option --radius_search or -r greater than 0");
		return false;
	}
	
	return true;
}

bool parse_args(int argc, char **argv)
{
	init_args();

	// Tmp variables for parse size_t
	char *endptr;
	unsigned long long val;

	int option;
	while ((option = getopt_long(argc, argv, "f:bts:R:r:n:m:h", long_options, NULL)) != -1) {
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
		case 's':
			args.save_ordered_cloud = optarg;
			break;
		case 'R':
			args.radius_reorder = (data_t)atof(optarg);
			args.radius_reorder = args.radius_reorder * args.radius_reorder;
			break;
		case 'r':
			args.radius_search = (data_t)atof(optarg);
			args.radius_search = args.radius_search * args.radius_search;
			break;
		case 'm':
			val = strtoull(optarg, &endptr, 10);

			if (*endptr != '\0') {
				handle_error(ERROR_PARSE_ARG, ERR_NONFATAL, "Error parsing value of -m");
				return false;
			}

			args.number_of_blocks = (index_t)val;
			break;
		case 'n':
			val = strtoull(optarg, &endptr, 10);

			if (*endptr != '\0') {
				handle_error(ERROR_PARSE_ARG, ERR_NONFATAL, "Error parsing value of -n");
				return false;
			}

			args.max_num_of_points = (index_t)val;
			break;
		case 'h':
			print_help(argv[0]);
			exit(EXIT_SUCCESS);
		case '?':
			handle_error(ERROR_PARSE_ARG, ERR_NONFATAL, "Cannot recognize option");
			return false;
		}
	}

	return check_obligatory_arguments();
}

const struct Args *get_args()
{
	return &args;
}