#pragma once

typedef enum {
    ERR_NONFATAL,
    ERR_FATAL
} ErrorSeverity;

typedef enum {
    ERROR_PARSE_ARG,
    ERROR_PARSE_POINTS
} ErrorCode;

/**
 * Handle an error if it happens
 *
 * @param[in] error      ErrorCode
 * @param[in] severity   Severity of the error
 *
 */
void handle_error(ErrorCode error, ErrorSeverity severity, const char *message);