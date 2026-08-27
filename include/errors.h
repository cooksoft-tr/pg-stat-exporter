#ifndef ERRORS_H
#define ERRORS_H

typedef enum {
    ERR_OK = 0,
    ERR_INVALID_ARG = 101,
    ERR_NO_MEMORY = 102,
    ERR_PATH_TOO_LONG = 103,
    ERR_CONFIG = 104,
    ERR_NOT_FOUND = 105,
    PG_DB_ERROR = 106
} ErrorCode;

#endif