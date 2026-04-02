#pragma once

#include <getopt.h>

#define K 150

typedef struct {
    char *cloud_points_file_name;
} Args;

/**
 * Parses command-line arguments into an Args structure.
 *
 * @param[in] argc Number of command-line arguments.
 * @param[in] argv Array of command-line argument strings.
 * @param[out] args Pointer to the Args structure where parsed values will be stored.
 */
void parse_args(int argc, char **argv, Args *args);