#pragma once
#include <getopt.h>
#include <stddef.h>

#define K 20 // Defined as a constant and not an argument for optimization

typedef struct {
    char *cloud_points_file_name;
    bool do_benchmark;
    bool do_test;
    double radius_search;
    size_t max_num_of_points;
} Args;

/**
 * Parses command-line arguments into an singleton Args structure.
 *
 * @param[in] argc Number of command-line arguments.
 * @param[in] argv Array of command-line argument strings.
 */
void parse_args(int argc, char **argv);

const Args *get_args();