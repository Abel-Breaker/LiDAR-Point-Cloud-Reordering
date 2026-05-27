#pragma once
#include <getopt.h>
#include <stddef.h>
#include "types.h"

typedef struct {
    char *cloud_points_file_name;
    bool do_benchmark;
    bool do_test;
    data_t radius_reorder;
    data_t radius_search;
    index_t max_num_of_points;
    index_t number_of_blocks;
} Args;

/**
 * Parses command-line arguments into an singleton Args structure.
 *
 * @param[in] argc Number of command-line arguments.
 * @param[in] argv Array of command-line argument strings.
 */
void parse_args(int argc, char **argv);

const Args *get_args();