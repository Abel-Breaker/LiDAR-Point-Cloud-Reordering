#include "error_handler.h"
#include "terminal_formating.h"
#include <stdio.h>
#include <stdlib.h>

static const char* get_error_message(ErrorCode err)
{
    switch(err) 
    {
		case ERROR_MALLOC: return "Error malloc: ";
		case ERROR_PARSE_ARG: return "Error parsing args: ";
		case ERROR_PARSE_POINTS: return "Error parsing points: ";
		case ERROR_POINT_CLOUD_TOO_BIG: return "Error point cloud to big: ";
		case ERROR_REORDER_RCM: return "Error when reorderind the point cloud with RCM: ";
        default: return "Unkown error";
    }
}

void handle_error(ErrorCode error, ErrorSeverity severity, const char *message)
{
	fprintf(stderr, "%s%s%s%s\n", BOLD_RED, get_error_message(error), message, COLOR_RESET);
	
	if (severity == ERR_FATAL) {
		exit(EXIT_FAILURE);
	}
}