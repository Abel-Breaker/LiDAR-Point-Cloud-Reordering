#pragma once
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#ifdef USE_FLOAT
typedef unsigned int index_t;
#define INDEX_MAX UINT_MAX

typedef float data_t;
#define DATA_MAX FLT_MAX
#else
typedef size_t index_t;
#define INDEX_MAX SIZE_MAX

typedef double data_t;
#define DATA_MAX DBL_MAX
#endif