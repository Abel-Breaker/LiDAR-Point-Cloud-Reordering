#pragma once

typedef enum {
    ERR_NONFATAL,
    ERR_FATAL
} ErrorSeverity;

typedef enum {
    ERROR_PARSE_ARG,
    ERROR_PARSE_POINTS,
    ERROR_TREE_NOT_INITIALIZED,
    ERROR_POINT_CLOUD_TOO_BIG
} ErrorCode;

/**
 * Handle an error if it happens
 *
 * @param[in] error      ErrorCode
 * @param[in] severity   Severity of the error
 * @param[in] message    Personalizated message
 *
 */
void handle_error(ErrorCode error, ErrorSeverity severity, const char *message);