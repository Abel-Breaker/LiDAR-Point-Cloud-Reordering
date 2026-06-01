#pragma once
#include <getopt.h>
#include "types.h"

struct Args{
    char *cloud_points_file_name;
    bool do_benchmark;
    bool do_test;
    data_t radius_reorder;
    data_t radius_search;
    index_t max_num_of_points;
    index_t number_of_blocks;
};

/**
 * @brief Parses command-line arguments into an singleton Args structure.
 *
 * @param[in] argc Number of command-line arguments.
 * @param[in] argv Array of command-line argument strings.
 * 
 * @return True if everything was correctly parsed, false if not.
 */
bool parse_args(int argc, char **argv);

/**
 * @return A const pointer to singleton Args structure to read args.
 */
const struct Args *get_args();