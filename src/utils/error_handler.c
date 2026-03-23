#include "error_handler.h"
#include <stdio.h>
//#include <errno.h>
#include <stdlib.h>

#define ANSI_COLOR_RED     "\x1b[31;1m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static const char* get_error_message(ErrorCode err)
{
    switch(err) 
    {
		case ERROR_PARSE_ARG: return "Error parsing args: ";
		case ERROR_PARSE_POINTS: return "Error parsing points: ";
		case ERROR_TREE_NOT_INITIALIZED: return "Error ERROR_TREE_NOT_INITIALIZED: ";
		case ERROR_POINT_CLOUD_TOO_BIG: return "Error point cloud to big (max UINT32)";
        default: return "Unkown error";
    }
}

void handle_error(ErrorCode error, ErrorSeverity severity, const char *message)
{
	fprintf(stderr, "%s%s%s%s\n", ANSI_COLOR_RED, get_error_message(error), message, ANSI_COLOR_RESET);
	
	if (severity == ERR_FATAL) { // TODO: Que mande sigint y ya
		exit(EXIT_FAILURE);
	}
}