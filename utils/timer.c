#define _POSIX_C_SOURCE 199309L
#include "timer.h"
#include <stdio.h>
#include <time.h>

static _Thread_local struct timespec start;

void timer_start(void)
{
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
}

void timer_stop_and_print(const char *message)
{
	struct timespec end;

	clock_gettime(CLOCK_MONOTONIC_RAW, &end);

	if (message) {
		printf("\n%s: %.6f s\n", message,
		       (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9);
	} else {
		printf("\n%.6f s\n",
		       (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9);
	}
}