#include "parse_args.h"
#include "error_handler.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct option long_options[] = {
    {"filename", required_argument, NULL, 'f'}, 
    {"help", no_argument, NULL, 'h'}, 
    {NULL, 0, NULL, 0}};

void parse_args(int argc, char **argv, Args *args)
{
    memset(args, 0, sizeof(*args)); // Inicialization

	int option;
	while ((option = getopt_long(argc, argv, "f:h", long_options, NULL)) != -1) {
		switch (option) {
		case 'f':
			args->cloud_points_file_name = optarg;
			break;
        case 'h':
            printf("Use: %s --filename archivo.pcd\n", argv[0]);
            exit(EXIT_SUCCESS);
        case '?':
            exit(EXIT_FAILURE);
        }
	}

    // Check for obligatory arguments
    if (!args->cloud_points_file_name) {
        handle_error(ERROR_PARSE_ARG, ERR_FATAL, "You must specify the required option --filename or -f\n");
    }
}