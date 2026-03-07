#pragma once

#include <getopt.h>

typedef struct {
    char *cloud_points_file_name;
} Args;

void parse_args(int argc, char **argv, Args *args);